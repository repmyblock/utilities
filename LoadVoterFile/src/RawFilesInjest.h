#ifndef RAWFILESINJEST_H
#define RAWFILESINJEST_H

#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <string>
#include <mutex>  // Add this line to include <mutex>
#include <atomic> // Add this line to include <atomic>
#include "Voter.h"  // Include your Voter class definition

#include <queue> // Include for std::queue
#include <string>
#include <mutex>
#include <condition_variable> // Include for std::condition_variable

#define PRINTBLOCK 2500000

class RawFilesInjest {
public:
  RawFilesInjest(const std::string&, const std::string&);
  ~RawFilesInjest();

  // Updated function signature to match the implementation
  void processChunk(const std::string&, size_t, size_t, int);
  void loadFile(void);

  // This should match the way you're going to use the wrapper
  void processChunkWrapper(const std::string&, size_t, size_t, int);
  void SetNumbersThreads(int);

  VoterInfoRaw getVoters(int);
  int getTotalVoters(void);
  std::string printFilename(void);
  
  void MN_RawFilesInjest(void);
  void NV_RawFilesInjest(void);
  void NY_RawFilesInjest(void);
  void OH_RawFilesInjest(void);
  void WA_RawFilesInjest(void);

private:
  int numThreads;
  std::chrono::milliseconds duration;
    
  std::string StateNameAbbrev;
  std::string TableDate;
  int TableDateNumber;
  std::string FileName;
    
  std::vector<VoterInfoRaw> voters;
  std::string ToUpperAccents(const std::string&);
  std::string ConvertLatin1ToUTF8(const std::string&);
  std::mutex outputMutex;

  std::vector<std::vector<std::string>> threadData;
  void countLinesInThread(const std::string&, int, int);
  
  std::queue<std::string> lineQueue;
  std::mutex fileMutex;
  std::condition_variable cv;
    
  bool allChunksProcessed = false;
  
  void RunStateFileNameLoader(void);
  
  void parseLineToVoterInfo(std::queue<std::string>&);
  
  void MN_parseLineToVoterInfo(std::queue<std::string>& queue);
  void NV_parseLineToVoterInfo(std::queue<std::string>& queue);
  void NY_parseLineToVoterInfo(std::queue<std::string>& queue);
  void OH_parseLineToVoterInfo(std::queue<std::string>& queue);
  void WA_parseLineToVoterInfo(std::queue<std::string>& queue);
  void PrintLineAsHex(const std::string&);
    
  std::vector<std::string> parseCSVLine(const std::string&);
  std::string trim(const std::string&);
    
};

#endif // RAWFILESINJEST_H
