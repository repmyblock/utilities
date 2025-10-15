// g++ -std=c++17 -O2 MD5load_dir.cpp -lcrypto -o MD5load_dir
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <openssl/evp.h>

namespace fs = std::filesystem;
using clock_type = std::chrono::steady_clock;

static inline double ms_since(clock_type::time_point t0) {
    using namespace std::chrono;
    return duration_cast<duration<double, std::milli>>(clock_type::now() - t0).count();
}

static bool is_valid_yyyymmdd(const std::string& s) {
    if (s.size() != 8) return false;
    for (char c : s) if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    int y = std::stoi(s.substr(0,4));
    int m = std::stoi(s.substr(4,2));
    int d = std::stoi(s.substr(6,2));
    if (y < 1900 || y > 3000) return false;
    if (m < 1 || m > 12) return false;
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    bool leap = ( (y%4==0 && y%100!=0) || (y%400==0) );
    int maxd = (m==2 && leap) ? 29 : mdays[m-1];
    return d >= 1 && d <= maxd;
}

static std::string md5_hex(const std::string& data) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");
    if (EVP_DigestInit_ex(ctx, EVP_md5(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, data.data(), data.size()) != 1 ||
        EVP_DigestFinal_ex(ctx, digest, &len) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("EVP MD5 failed");
    }
    EVP_MD_CTX_free(ctx);

    static const char* hex = "0123456789abcdef";
    std::string out;
    out.resize(len * 2); // MD5 -> 16 bytes -> 32 hex
    for (unsigned int i = 0; i < len; ++i) {
        unsigned char b = digest[i];
        out[i*2]   = hex[b >> 4];
        out[i*2+1] = hex[b & 0xF];
    }
    return out;
}

// Read the next *logical* CSV record (RFC4180-ish) from stream.
// Returns false on EOF; true and fills 'record' otherwise.
static bool read_next_csv_record(std::istream& in, std::string& record) {
    record.clear();
    if (!in.good()) return false;

    bool in_quotes = false;
    bool saw_any = false;
    for (;;) {
        int ch = in.get();
        if (ch == EOF) {
            if (!saw_any) return false;
            // EOF inside a record; accept what we have.
            return true;
        }
        saw_any = true;
        record.push_back(static_cast<char>(ch));

        if (ch == '"') {
            if (!in_quotes) {
                in_quotes = true;
            } else {
                int nxt = in.peek();
                if (nxt == '"') {
                    // Escaped quote "" -> consume and keep in_quotes
                    record.push_back(static_cast<char>(in.get()));
                } else {
                    in_quotes = false; // closing quote
                }
            }
        } else if ((ch == '\n' || ch == '\r') && !in_quotes) {
            // End of logical record; normalize CRLF
            if (ch == '\r' && in.peek() == '\n') {
                record.push_back(static_cast<char>(in.get()));
            }
            return true;
        }
    }
}

// Parse a CSV record (string) into fields, honoring quotes/escapes.
// Keeps raw field contents (without surrounding quotes; doubled quotes unescaped to ").
static void split_csv_fields(const std::string& rec, std::vector<std::string>& out) {
    out.clear();
    std::string cur;
    bool in_quotes = false;
    size_t i = 0, n = rec.size();

    auto push_field = [&](){ out.push_back(cur); cur.clear(); };

    while (i < n) {
        char c = rec[i++];
        if (in_quotes) {
            if (c == '"') {
                if (i < n && rec[i] == '"') { cur.push_back('"'); ++i; } // escaped quote
                else { in_quotes = false; }
            } else {
                cur.push_back(c);
            }
        } else {
            if (c == '"') {
                in_quotes = true;
            } else if (c == ',') {
                push_field();
            } else if (c == '\r' || c == '\n') {
                // logical end; ignore remainder
                break;
            } else {
                cur.push_back(c);
            }
        }
    }
    // finalize last field
    push_field();
    // If it was a completely empty record, you might get one empty field; that's fine.
}

// Quote a CSV field per RFC4180 (double internal quotes, surround with quotes).
static std::string csv_quote(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        if (c == '"') out.push_back('"');
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

// Build a normalized CSV line by removing 1-based positions in drop list.
static std::string build_normalized_line(const std::vector<std::string>& fields,
                                         const std::vector<int>& drop_1based) {
    std::unordered_set<int> drop(drop_1based.begin(), drop_1based.end());
    std::string out;
    bool first = true;
    for (int idx1 = 1; idx1 <= static_cast<int>(fields.size()); ++idx1) {
        if (drop.count(idx1)) continue;
        if (!first) out.push_back(',');
        first = false;
        out += csv_quote(fields[idx1 - 1]);
    }
    // No trailing newline here; hashing uses exactly this string
    return out;
}

struct FilePhaseResult {
    size_t lines = 0;
    int field_count = 0;
    // Arrays of MD5 (hex 32 chars + '\0')
    char (*ARRAY)[33] = nullptr;
    // For File #2 only: md5 -> index map
    std::unordered_map<std::string, size_t> map_md5_to_idx;
    // First examples to print:
    std::string first_original;
    std::string first_normalized;
};

// Determine drop list based on field count; throw if unsupported
static std::vector<int> drop_list_for_count(int count) {
    if (count == 45)      return {30,31,32,45};
    else if (count == 47) return {32,33,34,35,47};
    else if (count == 49) return {32,33,34,35,47,48,49};
    throw std::runtime_error("Unsupported field count: " + std::to_string(count));
}

// Count fields in first record (logical)
static int count_fields_first_record(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open: " + p.string());
    std::string rec;
    if (!read_next_csv_record(in, rec)) return 0;
    std::vector<std::string> fields;
    split_csv_fields(rec, fields);
    return static_cast<int>(fields.size());
}

static size_t parse_progress_arg(int argc, char** argv, size_t defaultN, int& parent_idx, int& date_idx) {
    size_t progress_every = defaultN;
    parent_idx = -1; date_idx = -1;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a.rfind("--progress=", 0) == 0) {
            try {
                size_t n = std::stoull(a.substr(11));
                if (n) progress_every = n;
            } catch (...) {}
        } else if (a == "--progress") {
            if (i+1 < argc) {
                try {
                    size_t n = std::stoull(argv[i+1]);
                    if (n) progress_every = n;
                } catch (...) {}
                ++i;
            }
        } else if (!a.empty() && a[0] != '-') {
            if (parent_idx == -1) parent_idx = i;
            else if (date_idx == -1) { date_idx = i; break; }
        }
    }
    return progress_every;
}

static FilePhaseResult process_file(const fs::path& file,
                                    const char* tag,
                                    size_t progress_every,
                                    int enforced_field_count, // 0 = detect from first record, else check equals
                                    bool build_map) {
    FilePhaseResult R;

    // 1) Count & get schema from first record
    auto t0 = clock_type::now();
    int fields_first = count_fields_first_record(file);
    if (fields_first == 0) throw std::runtime_error("Empty or unreadable first record: " + file.string());
    if (enforced_field_count != 0 && enforced_field_count != fields_first) {
        throw std::runtime_error(std::string("Field count mismatch: expected ") +
                                 std::to_string(enforced_field_count) + " got " + std::to_string(fields_first) +
                                 " in " + file.string());
    }
    R.field_count = fields_first;
    auto drops = drop_list_for_count(fields_first);
    double t_first_ms = ms_since(t0);

    // 2) Count logical records (fast-ish by scanning stream)
    t0 = clock_type::now();
    {
        std::ifstream in(file, std::ios::binary);
        if (!in) throw std::runtime_error("Cannot open: " + file.string());
        std::string rec;
        size_t cnt = 0;
        auto t_start = clock_type::now();
        while (read_next_csv_record(in, rec)) {
            ++cnt;
            if (progress_every && (cnt % progress_every == 0)) {
                double ms = ms_since(t_start);
                std::cerr << "[" << tag << " count] lines=" << cnt
                          << " rate=" << (cnt * 1000.0 / (ms > 0 ? ms : 1.0)) << " lines/sec\n";
            }
        }
        R.lines = cnt;
    }
    double t_count_ms = ms_since(t0);

    // 3) Allocate ARRAY
    t0 = clock_type::now();
    if (R.lines > 0) R.ARRAY = new char[R.lines][33];
    double t_alloc_ms = ms_since(t0);

    // 4) Hash pass (+ optional map build)
    t0 = clock_type::now();
    std::ifstream in(file, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open for hash: " + file.string());
    if (build_map && R.lines > 0) R.map_md5_to_idx.reserve(static_cast<size_t>(R.lines * 1.3));

    std::string rec;
    std::vector<std::string> fields;
    size_t idx = 0;
    bool saved_first_example = false;
    auto t_hash_start = clock_type::now();

    while (read_next_csv_record(in, rec)) {
        split_csv_fields(rec, fields);
        if (static_cast<int>(fields.size()) != R.field_count) {
            throw std::runtime_error("Inconsistent field count encountered in " + file.string());
        }
        std::string norm = build_normalized_line(fields, drops);
        std::string h = md5_hex(norm);
        std::memcpy(R.ARRAY[idx], h.data(), 32);
        R.ARRAY[idx][32] = '\0';

        if (!saved_first_example) {
            R.first_original = rec;
            // strip trailing newline(s) for clean display
            while (!R.first_original.empty() &&
                   (R.first_original.back() == '\n' || R.first_original.back() == '\r'))
                R.first_original.pop_back();
            R.first_normalized = norm;
            saved_first_example = true;
        }

        if (build_map) R.map_md5_to_idx.emplace(h, idx);

        ++idx;
        if (progress_every && (idx % progress_every == 0)) {
            double ms = ms_since(t_hash_start);
            std::cerr << "[" << tag << " hash] lines=" << idx
                      << " rate=" << (idx * 1000.0 / (ms > 0 ? ms : 1.0)) << " lines/sec\n";
        }
    }
    double t_hash_ms = ms_since(t0);

    // Timings per phase
    std::cout << "[" << tag << "] First-record parse: " << t_first_ms << " ms\n";
    std::cout << "[" << tag << "] Count: " << R.lines << " lines in " << t_count_ms
              << " ms  (" << (R.lines ? (R.lines * 1000.0 / t_count_ms) : 0.0) << " lines/sec)\n";
    std::cout << "[" << tag << "] Allocation: " << t_alloc_ms << " ms  ("
              << (R.lines * 33.0 / (1024.0 * 1024.0)) << " MiB)\n";
    std::cout << "[" << tag << "] Hash" << (build_map ? "+map" : "")
              << ": " << t_hash_ms << " ms  ("
              << (R.lines ? (R.lines * 1000.0 / t_hash_ms) : 0.0) << " lines/sec)\n";

    return R;
}

int main(int argc, char** argv) {
    try {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " [--progress N] <parent_dir> <DATE: YYYYMMDD>\n";
            return 2;
        }
        int parent_idx=-1, date_idx=-1;
        const size_t default_progress = 1000000;
        size_t progress_every = parse_progress_arg(argc, argv, default_progress, parent_idx, date_idx);
        if (parent_idx == -1 || date_idx == -1) {
            std::cerr << "Error: missing <parent_dir> and/or <DATE>\n";
            return 2;
        }

        fs::path parent = argv[parent_idx];
        std::string date = argv[date_idx];
        if (!is_valid_yyyymmdd(date)) {
            std::cerr << "Error: DATE must be YYYYMMDD\n";
            return 2;
        }

        std::error_code ec;
        if (!fs::exists(parent, ec) || !fs::is_directory(parent, ec)) {
            std::cerr << "Error: not a directory: " << parent << "\n";
            return 1;
        }

        // Collect valid yyyymmdd subdirs and sort
        std::vector<std::string> dates;
        for (const auto& e : fs::directory_iterator(parent, ec)) {
            if (ec) continue;
            if (!e.is_directory()) continue;
            std::string name = e.path().filename().string();
            if (is_valid_yyyymmdd(name)) dates.push_back(name);
        }
        std::sort(dates.begin(), dates.end());

        auto it = std::find(dates.begin(), dates.end(), date);
        if (it == dates.end()) {
            std::cerr << "Error: DATE directory not found: " << date << "\n";
            return 1;
        }
        if (std::next(it) == dates.end()) {
            std::cerr << "Error: no next date directory after " << date << "\n";
            return 1;
        }
        std::string date1 = *it;
        std::string date2 = *std::next(it);

        fs::path file1 = parent / date1 / ("AllNYSVoters_" + date1 + ".txt");
        fs::path file2 = parent / date2 / ("AllNYSVoters_" + date2 + ".txt");
        if (!fs::exists(file1, ec) || !fs::is_regular_file(file1, ec)) {
            std::cerr << "Error: missing File1: " << file1 << "\n";
            return 1;
        }
        if (!fs::exists(file2, ec) || !fs::is_regular_file(file2, ec)) {
            std::cerr << "Error: missing File2: " << file2 << "\n";
            return 1;
        }

        std::cout << "File1: " << file1 << "\n";
        std::cout << "File2: " << file2 << "\n";

        auto t_total = clock_type::now();

        // Detect schema from each first record
        int fc1 = count_fields_first_record(file1);
        int fc2 = count_fields_first_record(file2);
        if (!(fc1 == 45 || fc1 == 47 || fc1 == 49) || !(fc2 == 45 || fc2 == 47 || fc2 == 49)) {
            std::cerr << "Error: unsupported field counts (only 45, 47, 49 supported): "
                      << fc1 << " vs " << fc2 << "\n";
            return 1;
        }
        // Rule: 47 and 49 can compare with each other; 45 cannot compare with 47/49
        if ((fc1 == 45 && (fc2 == 47 || fc2 == 49)) ||
            (fc2 == 45 && (fc1 == 47 || fc1 == 49))) {
            std::cerr << "Error: cannot compare 45-field file with 47/49-field file.\n";
            return 1;
        }

        // Process File #1 (ARRAY1)
        auto R1 = process_file(file1, "f1", progress_every, fc1, /*build_map*/false);

        // Process File #2 (ARRAY2 + ARRAY3)
        auto R2 = process_file(file2, "f2", progress_every, fc2, /*build_map*/true);

        // Show first original + normalized (used for hash)
        std::cout << "\n[Sample File1 first original]\n" << R1.first_original << "\n";
        std::cout << "[Sample File1 normalized]\n" << R1.first_normalized << "\n\n";

        std::cout << "[Sample File2 first original]\n" << R2.first_original << "\n";
        std::cout << "[Sample File2 normalized]\n" << R2.first_normalized << "\n\n";

        // Compare: ARRAY1[i] in ARRAY3?
        auto t0 = clock_type::now();
        size_t found = 0, not_found = 0;
        std::unordered_set<std::string> matched_md5s; matched_md5s.reserve(R2.lines);
        for (size_t i = 0; i < R1.lines; ++i) {
            std::string key(R1.ARRAY[i], 32);
            auto it3 = R2.map_md5_to_idx.find(key);
            if (it3 != R2.map_md5_to_idx.end()) {
                ++found;
                matched_md5s.insert(key);
            } else {
                ++not_found;
            }
        }
        double t_lookup_ms = ms_since(t0);
        std::cout << "[Compare] Found: " << found << "  Not found: " << not_found
                  << "  | Time: " << t_lookup_ms << " ms\n";

        // Write NotFound_File2.txt: original records from File #2 whose normalized MD5 wasn't matched
        std::ofstream outNF("NotFound_File2.txt");
        if (!outNF) throw std::runtime_error("Cannot open NotFound_File2.txt for writing");
        t0 = clock_type::now();
        {
            std::ifstream in(file2, std::ios::binary);
            if (!in) throw std::runtime_error("Cannot re-open File2 for output");
            std::string rec;
            std::vector<std::string> fields;
            auto drops2 = drop_list_for_count(R2.field_count);
            size_t emitted = 0, idx = 0;

            while (read_next_csv_record(in, rec)) {
                split_csv_fields(rec, fields);
                std::string norm = build_normalized_line(fields, drops2);
                std::string h = md5_hex(norm);
                if (!matched_md5s.count(h)) {
                    outNF << rec; // write the original record exactly as read
                    // ensure newline termination
                    if (rec.empty() || (rec.back() != '\n' && rec.back() != '\r')) outNF << '\n';
                    ++emitted;
                }
                ++idx;
                if (progress_every && (idx % progress_every == 0)) {
                    double ms = ms_since(t0);
                    std::cerr << "[output] processed=" << idx
                              << " unmatched_written=" << emitted
                              << " elapsed_ms=" << ms
                              << " rate=" << (idx * 1000.0 / (ms > 0 ? ms : 1.0)) << " recs/sec\n";
                }
            }
            std::cout << "[Output] Unmatched lines from File2 written: " << emitted
                      << "  (" << ms_since(t0) << " ms)\n";
        }

        // Cleanup
        delete[] R1.ARRAY;
        delete[] R2.ARRAY;

        std::cout << "Total time: " << ms_since(t_total) << " ms\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
