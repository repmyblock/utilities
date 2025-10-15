#include "RawFilesInjest.h"

#define CLOCK_START     auto start = std::chrono::high_resolution_clock::now();
#define CLOCK_END       auto end = std::chrono::high_resolution_clock::now();  \
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
#include <iomanip>

std::mutex fileMutex;
std::condition_variable cv;
bool allChunksProcessed = false;
  
RawFilesInjest::RawFilesInjest(const std::string& IncomingState, const std::string& IncomingTableDate) {    
  numThreads = 1;
  StateNameAbbrev = IncomingState;
  TableDate = IncomingTableDate;
	TableDateNumber = std::stoi(TableDate);
  std::cout << "Opening the RawFilesInjest with for the State of " << StateNameAbbrev << " in TableDate: " << TableDate << std::endl;   
}

RawFilesInjest::~RawFilesInjest() {
  // Destructor to clean up resources if needed
}

void RawFilesInjest::SetNumbersThreads(int coresNumbers) {
  numThreads = coresNumbers * 2;
}

void RawFilesInjest::SetParseProcess(bool processflag, std::vector<std::string>& vectorline) {
	donotparse = ! processflag;
	std::cout << "Process Flag: " << donotparse << std::endl;
	sortedVectorLine = vectorline;
}

std::string RawFilesInjest::printFilename(void) {
  return FileName;
}

void RawFilesInjest::processChunk(const std::string& filename, size_t start, size_t end, int threadID) {
  std::ifstream file(filename);  
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


void RawFilesInjest::loadFile() {
    
  RunStateFileNameLoader();
  
  std::ifstream file(FileName);
  if (!file) {
    std::cerr << "Error opening file: " << FileName << std::endl;
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
    threads.emplace_back(&RawFilesInjest::processChunkWrapper, this, FileName, start, end, i);
    start = end;
  }

  for (auto& thread : threads) {
    thread.join();
  }

  allChunksProcessed = true; // Set the flag to true to indicate all threads have finished
  cv.notify_all(); // Notify in case the main thread is waiting
  file.close(); // Close the input file as we're done reading it
          
  // Read lines from the queue and write to the output file
  int TotalFileCounter = 0;
  while (true) {
    std::unique_lock<std::mutex> lock(fileMutex);
    cv.wait(lock, [this] { return !lineQueue.empty() || allChunksProcessed; }); // 'this' captures all member variables
    
		std::cout << "Size of lineQueue: " << lineQueue.size() << std::endl;
    while (! lineQueue.empty()) {
    	std::cout << "Starting the queue Inside the lineQueue -> Size of lineQueue: " << lineQueue.size() << std::endl;
    	if (donotparse == false) {
    		
	      parseLineToVoterInfo(lineQueue);
	      
	    } else {
	    	// This is for the CompareCD function.
	    	parseLineToList(lineQueue);
	    }
      std::cout << "Finishing the queue Inside the lineQueue -> Size of lineQueue: " << lineQueue.size() << std::endl;
    }
    lock.unlock(); // Unlock once after finishing the inner loop
   
    if (lineQueue.empty() && allChunksProcessed) {
	    std::queue<std::string>().swap(lineQueue); // Free memory used by lineQueue
      break; // All lines have been processed
    }
  }
 
}

VoterInfoRaw RawFilesInjest::getVoters(int counter) {
	int VoterSize = voters.size();
  if (counter > VoterSize) return voters[VoterSize];  
  return voters[counter];
}

int RawFilesInjest::getTotalVoters() {
  return voters.size();
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

void RawFilesInjest::PrintLineAsHex(const std::string& line) {
  std::stringstream hexStream;

  // For each character in the string, convert it to hex and append it to the stringstream
  for (unsigned char c : line) {
    hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << "(" << c << ") ";
  }

  // Print the stringstream's content
  std::cout << hexStream.str() << std::endl;
}

std::string RawFilesInjest::ToUpperAccents(const std::string& input) {
  std::string result;
  bool lastWasSpace = false;
  bool ignoreNext = false; // flag to ignore the character following '\xC2'
 
  for (unsigned char c : input) {
    
    if (c == 0xC2) {
      ignoreNext = true; // set flag to potentially ignore the next character
      continue;
    }
      
    // Ignore sequence '\xC2\x9F'
    if (ignoreNext) { 
      if (c == 0x9F) {
        ignoreNext = false; // reset flag if the ignored character is encountered
        continue;
      } else {
        // if it wasn't the specific character we're ignoring, don't ignore the next ones
        ignoreNext = false;
        result += 0xC2;
      }
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
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5:   // àáâãäå
        case 0xA8: case 0xA9: case 0xAA: case 0xAB:                         // èéêë
        case 0xAC: case 0xAD: case 0xAE: case 0xAF:                         // ìíîï
        case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB8:   // òóôõöø
        case 0xB9: case 0xBA: case 0xBB: case 0xBC:                         // ùúûü
        case 0xA7: case 0xB1:                                               // çñ
          result += (c - 32);
          break;
                
        default: // case 0x9F:  // ß
          result += c;  
          break;
      }
      lastWasSpace = false;
    }
  }
    
  return result;
}

void RawFilesInjest::RunStateFileNameLoader(void) {
  if ( StateNameAbbrev == "OH") { OH_RawFilesInjest(); } 
  else if ( StateNameAbbrev == "MN") { MN_RawFilesInjest(); }
  else if ( StateNameAbbrev == "NV") { NV_RawFilesInjest(); } 
  else if ( StateNameAbbrev == "NY") { NY_RawFilesInjest(); }
  else if ( StateNameAbbrev == "WA") { WA_RawFilesInjest(); } 
  else {
    std::cout << HI_RED << "Quitting as we don't have a parser for the state of " << NC << HI_YELLOW << StateNameAbbrev << NC << std::endl;
    exit(1);
  }
}

void RawFilesInjest::parseLineToVoterInfo(std::queue<std::string>& queue) {
  if ( StateNameAbbrev == "OH") { OH_parseLineToVoterInfo(queue); } 
  else if ( StateNameAbbrev == "MN") { MN_parseLineToVoterInfo(queue); }
  else if ( StateNameAbbrev == "NV") { NV_parseLineToVoterInfo(queue); } 
  else if ( StateNameAbbrev == "NY") { NY_parseLineToVoterInfo(queue); }
  else if ( StateNameAbbrev == "WA") { WA_parseLineToVoterInfo(queue); } 
  else {
    std::cout << HI_RED << "Quitting as we don't have a parser for the state of " << NC << HI_YELLOW << StateNameAbbrev << NC << std::endl;
    exit(1);
  }
}

 
std::vector<std::string> RawFilesInjest::parseCSVLine(const std::string& line) {
	std::vector<std::string> fields;
	std::string field;
	bool inQuotes = false;

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
}

std::string RawFilesInjest::trim(const std::string& str) {
  auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
  auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
  return (start < end) ? std::string(start, end) : std::string();
}

std::string RawFilesInjest::PrintCurrentTime(void) {
    std::time_t currentTime = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%H:%M:%S") << "\t"; 
    return oss.str();
}

void RawFilesInjest::PrintDebug_ForDataDistrict(int counter) {
	std::cout << PrintCurrentTime() <<	"Counter: " << counter << std::endl;	
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - dataCountyID: " << voters[counter].countyCode << std::endl;
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - dataDistrictTownId: " << voters[counter].townCity << std::endl;
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - dataElectoral: " << voters[counter].electionDistrict << std::endl;
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - dataStateAssembly: " << voters[counter].assemblyDistrict<< std::endl;
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - dataStateSenate: " << voters[counter].senateDistrict << std::endl;
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - dataLegislative: " << voters[counter].legislativeDistrict << std::endl;
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - dataWard: " << voters[counter].ward << std::endl;
	std::cout << PrintCurrentTime() <<	"  Debug DataDistrict - DataCongress: " << voters[counter].congressionalDistrict << std::endl;
}

void RawFilesInjest::parseLineToList(std::queue<std::string>& queue) {
	int counter = 0;
	
	if (queue.size() < 1) return;
	std::cout << "At start of the Size of the Queue " << HI_YELLOW << queue.size() << NC << std::endl;
	  
	 while (! queue.empty()) {
		std::string line = queue.front();		
		// std::cout << "Line: " << line << std::endl;					
		sortedVectorLine.push_back(line);
	
  	if ( ++counter % PRINTBLOCK == 0 ) {
  		std::cout << "\tProceed reading " << HI_PINK << counter << NC;
  		std::cout << "\tSize of the Queue " << HI_YELLOW << queue.size() << NC << std::endl;
  	}
  	
  	queue.pop();
  }
  
  std::sort(sortedVectorLine.begin(), sortedVectorLine.end());
	std::cout << "\tSize of the Queue " << HI_YELLOW << queue.size() << NC << std::endl;
}