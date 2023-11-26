#include "../RawFilesInjest.h"

// MINNESOTA

void RawFilesInjest::MN_RawFilesInjest(void) {
  std::cout << "I am in the Raw Minessota Injest File";  
  FileName = "/home/usracct/VoterFiles/" + StateNameAbbrev + "/" + TableDate + "/SWVF_" + TableDate + ".txt";  
}

void RawFilesInjest::MN_parseLineToVoterInfo(std::queue<std::string>& queue) {
  std::cout << "I need to write the parser";
  std::cout << std::endl;
  exit(1);
}