#ifndef RAWFILESINJEST_H
#define RAWFILESINJEST_H

#include <vector>
#include <string>
#include "Voter.h"  // Include your Voter class definition

class RawFilesInjest {
public:
    RawFilesInjest();
    ~RawFilesInjest();

		void loadFile(const std::string& filename);
    VoterInfoRaw getVoters(int counter);
    int getTotalVoters();

private:		
    std::vector<VoterInfoRaw> voters;
    std::string ToUpperAccents(const std::string& input);
   	std::string ConvertLatin1ToUTF8(const std::string& latin1String);

};

#endif // RAWFILESINJEST_H
