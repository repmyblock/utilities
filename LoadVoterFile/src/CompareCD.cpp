#include "DatabaseConnector.h"
#include "DataCollector.h"
#include "RawFilesInjest.h"
#include "Voter.h"
#include <iostream>
#include <fstream>      // Necessary for file handling
#include <locale>       // Necessary for setting locale
#include <unistd.h>     // Necessary for process ID retrieval
#include <future>
#include <iomanip>
#include <ctime>

#include "DatabaseConnector.h"
#include "DataCollector.h"
#include "RawFilesInjest.h"
#include "Voter.h"
#include <iostream>
#include <fstream>      // Necessary for file handling
#include <locale>       // Necessary for setting locale
#include <unistd.h>     // Necessary for process ID retrieval
#include <future>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <iterator>     // For std::set_difference
#include <vector>

std::string PrintCurrentTime() {
    std::time_t currentTime = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%H:%M:%S") << "\t"; 
    return oss.str();
}

int main(int argc, char* argv[]) {
  std::locale loc("en_US.UTF-8");
  
  // Check for three arguments: state abbreviation and two table dates
  if (argc != 4) {
      std::cerr << "Usage: " << argv[0] << " <state abbreviation> <tabledate1> <tabledate2>" << std::endl;
      exit(1);
  }
  
  // Assign the state abbreviation and table dates from command line arguments
  std::string StateNameAbbrev = argv[1];
  std::string tabledate1 = argv[2];
  std::string tabledate2 = argv[3];

  unsigned int numCores = std::thread::hardware_concurrency() * 2;
  
  if (numCores == 0) {
      std::cout << "Unable to determine the number of CPU cores." << std::endl;
      exit(1);
  } 
  
  std::cout << PrintCurrentTime() << "DEBUG: accessfile() called by process " << getpid() << " (parent: " << getppid() << ")" << std::endl;
  std::cout << PrintCurrentTime() << "Number of CPU cores: " << numCores << std::endl;
  std::cout << PrintCurrentTime() << "clear; pidstat -r 1 -p " << getpid() << std::endl;
  
  std::cout.imbue(loc); 
  
  RawFilesInjest injest1(StateNameAbbrev, tabledate1);
  injest1.SetParseProcess(false, vec1);        // This is simply so it doesn't parse.
  injest1.SetNumbersThreads(numCores);
  injest1.loadFile();
  std::cout << "The size of vec1: " << vec1.size() << std::endl;
  
  std::cout << PrintCurrentTime() << "Worked on " << injest1.printFilename() << std::endl;
  std::cout << PrintCurrentTime() << "Finish the ingest of the file" << std::endl;
  std::cout << PrintCurrentTime() << "Injested TotalVoters:\t" << injest1.getTotalVoters() << std::endl;

  RawFilesInjest injest2(StateNameAbbrev, tabledate2);
  injest2.SetParseProcess(false, vec2);        // This is simply so it doesn't parse.
  injest2.SetNumbersThreads(numCores);
  injest2.loadFile();
  
  std::cout << PrintCurrentTime() << "Worked on " << injest2.printFilename() << std::endl;
  std::cout << PrintCurrentTime() << "Finish the ingest of the file" << std::endl;
  std::cout << PrintCurrentTime() << "Injested TotalVoters:\t" << injest2.getTotalVoters() << std::endl;
  
 
  std::cout << PrintCurrentTime() << "Data collection and ingestion completed." << std::endl;
  return 0;
}