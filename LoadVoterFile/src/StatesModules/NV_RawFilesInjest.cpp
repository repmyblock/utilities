#include "../RawFilesInjest.h"

// NEVADA

void RawFilesInjest::NV_RawFilesInjest(void) { 
  std::cout << "I am in the Raw Nevada Injest File";
  FileName = "/home/usracct/VoterFiles/" + StateNameAbbrev + "/" + TableDate + "/SWVF_" + TableDate + ".txt";  
}

void RawFilesInjest::NV_parseLineToVoterInfo(std::queue<std::string>& queue) {
  std::cout << "I need to write the parser";
  std::cout << std::endl;
  exit(1);
}