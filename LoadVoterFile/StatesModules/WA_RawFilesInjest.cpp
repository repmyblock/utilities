#include "../RawFilesInjest.h"

// WASHINGTON STATE

void RawFilesInjest::WA_RawFilesInjest(void) {
  std::cout << "I am in the Raw Washington Injest File";
  FileName = "/home/usracct/VoterFiles/" + StateNameAbbrev + "/" + TableDate + "/SWVF_" + TableDate + ".txt";  
}

void RawFilesInjest::WA_parseLineToVoterInfo(const std::string& line) {
  std::cout << "I need to write the parser";
  std::cout << std::endl;
  exit(1);
}