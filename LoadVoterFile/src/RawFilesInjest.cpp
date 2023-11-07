#include "RawFilesInjest.h"

#define CLOCK_START     auto start = std::chrono::high_resolution_clock::now();
#define CLOCK_END				auto end = std::chrono::high_resolution_clock::now();  \
                        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
     
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <functional>
#include <condition_variable>

std::mutex fileMutex;
std::condition_variable cv;
bool allChunksProcessed = false;
	
RawFilesInjest::RawFilesInjest() {		
	numThreads = 1;
}

RawFilesInjest::~RawFilesInjest() {
  // Destructor to clean up resources if needed
}

void RawFilesInjest::SetNumbersThreads(int coresNumbers) {
	numThreads = coresNumbers;
}

void RawFilesInjest::processChunk(const std::string& filename, size_t start, size_t end, int threadID) {
  std::ifstream file(filename);

  std::cout << HI_PINK <<  "Begin thread: " << threadID << NC << std::endl;
  
  file.seekg(start);
  std::string line;
  
  if (start != 0) {
    // Skip the partial line if not at the beginning of the file
    std::getline(file, line);
  }

   // Read lines up to the end point
  while (file.tellg() < end && std::getline(file, line)) {
    std::unique_lock<std::mutex> lock(fileMutex); // Lock the mutex to protect the queue
    lineQueue.push(line); // Add the line to the queue
    cv.notify_one(); // Notify one waiting thread
    lock.unlock(); // Unlock the mutex
  }

  std::cout << HI_PINK << "End thread: " << threadID << NC << std::endl;

  // The last thread will signal that all chunks have been processed
  if (threadID == numThreads - 1) {
    std::lock_guard<std::mutex> lock(fileMutex); // Use lock_guard for RAII style locking
    allChunksProcessed = true;
    cv.notify_all(); // Notify all waiting threads
  }
}

void RawFilesInjest::processChunkWrapper(const std::string& filename, size_t start, size_t end, int threadID) {
	processChunk(filename, start, end, threadID);
}

void RawFilesInjest::loadFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file) {
    std::cerr << "Error opening file: " << filename << std::endl;
    exit(1);
  }

  file.seekg(0, std::ios::end);
  size_t fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  size_t chunkSize = fileSize / numThreads;
  size_t start = 0;

  std::vector<std::thread> threads;

  for (int i = 0; i < numThreads; ++i) {
    size_t end = (i == numThreads - 1) ? fileSize : start + chunkSize;
    threads.emplace_back(&RawFilesInjest::processChunkWrapper, this, filename, start, end, i);
    start = end;
  }

  for (auto& thread : threads) {
  	thread.join();
  }

	allChunksProcessed = true; // Set the flag to true to indicate all threads have finished
  cv.notify_all(); // Notify in case the main thread is waiting

  file.close(); // Close the input file as we're done reading it

  // Open the output file for writing
  std::ofstream outFile("reconstructed_file.txt");
  if (!outFile.is_open()) {
    std::cerr << "Error opening file for writing.\n";
    exit(1);
  }

  // Read lines from the queue and write to the output file
  int TotalFileCounter = 0;
 	while (true) {
    std::unique_lock<std::mutex> lock(fileMutex);
    cv.wait(lock, [this] { return !lineQueue.empty() || allChunksProcessed; }); // 'this' captures all member variables

		while (!lineQueue.empty()) {
			std::string line = lineQueue.front();
			parseLineToVoterInfo(line);
			lineQueue.pop();
			// outFile << line << std::endl; // Write each line to the output file
			// std::cout << line << std::endl; // Also print each line to standard output
		}
		lock.unlock(); // Unlock once after finishing the inner loop
		
	  if (lineQueue.empty() && allChunksProcessed) {
	    break; // All lines have been processed
	  }
	}

  // File writing done
  outFile.close();

  std::cout << HI_RED << "This is the end" << NC << std::endl;
}

VoterInfoRaw RawFilesInjest::getVoters(int counter) {
	if ( counter > voters.size() - 1 ) return voters[(voters.size()-1)];	
  return voters[counter];
}

int RawFilesInjest::getTotalVoters() {
  return voters.size() - 1;
}

std::string RawFilesInjest::ConvertLatin1ToUTF8(const std::string& latin1String) {
  std::string utf8String;
  utf8String.reserve(latin1String.length());

  for (char c : latin1String) {
    if (static_cast<unsigned char>(c) < 0x80) {
      // ASCII character, no conversion needed
      utf8String.push_back(c);
    } else {
      // Non-ASCII character, convert to UTF-8
      utf8String.push_back(0xC0 | static_cast<unsigned char>(c) >> 6);
      utf8String.push_back(0x80 | (static_cast<unsigned char>(c) & 0x3F));
    }
  }

  return utf8String;
}

std::string RawFilesInjest::ToUpperAccents(const std::string& input) {
	std::string result;
  bool lastWasSpace = false;
  bool ignoreNext = false; // flag to ignore the character following '\xC2'
	
	for (unsigned char c : input) {

		// Ignore sequence '\xC2\x9F'
		if (ignoreNext) {	
			if (c == 0x9F) {
				ignoreNext = false; // reset flag if the ignored character is encountered
				continue;
			} else {
				// if it wasn't the specific character we're ignoring, don't ignore the next ones
				ignoreNext = false;
			}
		}
		
		if (c == 0xC2) {
			ignoreNext = true; // set flag to potentially ignore the next character
		  continue;
		}
		
    if (std::isalpha(c)) {
        try {
            result += std::toupper(c);
            lastWasSpace = false;
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << c << " One: " << e.what() << std::endl;
            exit(1);
        }
    } else if (c == '\t' || c == ' ') {
        if (!lastWasSpace) {
            result += ' ';
            lastWasSpace = true;
        }
    } else {
			switch (c) {		
	  		case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5:  	// àáâãäå
				case 0xA8: case 0xA9: case 0xAA: case 0xAB:  												// èéêë
				case 0xAC: case 0xAD: case 0xAE: case 0xAF: 												// ìíîï
				case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB8: 	// òóôõöø
				case 0xB9: case 0xBA: case 0xBB: case 0xBC: 												// ùúûü
				case 0xA7: case 0xB1: 																							// çñ
					result += (c - 32);
          break;
				
				default: case 0x9F: 	// ß
      		result += c;
      		break;
			}
      lastWasSpace = false;
		}
	}
	return result;
}

///// These are the parse 

void RawFilesInjest::parseLineToVoterInfo(const std::string& line) {
	
	auto parseCSVLine = [](const std::string& line) -> std::vector<std::string> {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    
    auto trim = [](const std::string& str) -> std::string {
	    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
	    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
	    return (start < end) ? std::string(start, end) : std::string();
		};

    for (char c : line) {
      if (c == '"') {
        inQuotes = !inQuotes;
  	  } else if (c == ',' && !inQuotes) {
        fields.push_back(trim(field));
        field.clear();
      } else {
      	field += toupper(c);
    	}
  	}

    fields.push_back(trim(field)); // Add the last field
    return fields;
	};
		
	VoterInfoRaw voter;
  std::vector<std::string> fields = parseCSVLine(ToUpperAccents(ConvertLatin1ToUTF8(line)));

  if (fields.size() >= 47) { // Adjust the size according to your data
    voter.lastName 											= fields[0];
    voter.firstName 										= fields[1];
    voter.middleName 										= fields[2];
    voter.nameSuffix 										= fields[3];
    voter.residentialAddressNumber 			= fields[4];
    voter.residentialHalfCode 					= fields[5];
    voter.residentialPredirection 			= fields[6];
    voter.residentialStreetName 				= fields[7];
    voter.residentialPostdirection 			= fields[8];
    voter.residentialAptNumber 					= fields[9];
    voter.residentialApartment 					= fields[10];
    voter.residentialNonStandartAddress = fields[11];
    voter.residentialCity 							= fields[12];
    voter.residentialZip5 							= fields[13];
    voter.residentialZip4 							= fields[14];
    voter.mailingAddress1 							= fields[15];
    voter.mailingAddress2 							= fields[16];
    voter.mailingAddress3 							= fields[17];
    voter.mailingAddress4 							= fields[18];
    voter.dateOfBirth 									= fields[19];
    voter.gender 												= fields[20];
    voter.enrollment 										= fields[21];
    voter.otherParty 										= fields[22];
    voter.countyCode 										= fields[23];
    voter.electionDistrict 							= fields[24];
    voter.legislativeDistrict 					= fields[25];
    voter.townCity 											= fields[26];
    voter.ward 													= fields[27];
    voter.congressionalDistrict 				= fields[28];
    voter.senateDistrict 								= fields[29];
    voter.assemblyDistrict 							= fields[30];
    voter.lastVotedDate 								= fields[31];
    voter.prevYearVoted									= fields[32];
    voter.prevCounty 										= fields[33];
    voter.prevAddress 									= fields[34];
    voter.prevName 											= fields[35];
    voter.countyVrNumber 								= fields[36];
    voter.registrationDate 							= fields[37];
    voter.vrSource 											= fields[38];
    voter.idRequired 										= fields[39];
    voter.idMet													= fields[40];
    voter.status 												= fields[41];
    voter.reasonCode 										= fields[42];
    voter.inactivityDate 								= fields[43];
    voter.purgeDate 										= fields[44];
    voter.sboeId 												= fields[45];
		voter.voterHistory 									= fields[46];
		voters.push_back(voter);

  } else {  	
  	std::cout << "Error with the numbers of fields at line " <<  std::endl;
  	exit(1);
  }
}
 