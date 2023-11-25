#include "../RawFilesInjest.h"

// OHIO

void RawFilesInjest::OH_RawFilesInjest(void) {
  FileName = "/home/usracct/VoterFiles/" + StateNameAbbrev + "/" + TableDate + "/SWVF_" + TableDate + ".txt";  
}

void RawFilesInjest::OH_parseLineToVoterInfo(const std::string& line) {
  
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
    voter.lastName                      = fields[0];
    voter.firstName                     = fields[1];
    voter.middleName                    = fields[2];
    voter.nameSuffix                    = fields[3];
    voter.residentialAddressNumber      = fields[4];
    voter.residentialHalfCode           = fields[5];
    voter.residentialPredirection       = fields[6];
    voter.residentialStreetName         = fields[7];
    voter.residentialPostdirection      = fields[8];
    voter.residentialAptNumber          = fields[9];
    voter.residentialApartment          = fields[10];
    voter.residentialNonStandartAddress = fields[11];
    voter.residentialCity               = fields[12];
    voter.residentialZip5               = fields[13];
    voter.residentialZip4               = fields[14];
    voter.mailingAddress1               = fields[15];
    voter.mailingAddress2               = fields[16];
    voter.mailingAddress3               = fields[17];
    voter.mailingAddress4               = fields[18];
    voter.dateOfBirth                   = fields[19];
    voter.gender                        = fields[20];
    voter.enrollment                    = fields[21];
    voter.otherParty                    = fields[22];
    voter.countyCode                    = fields[23];
    voter.electionDistrict              = fields[24];
    voter.legislativeDistrict           = fields[25];
    voter.townCity                      = fields[26];
    voter.ward                          = fields[27];
    voter.congressionalDistrict         = fields[28];
    voter.senateDistrict                = fields[29];
    voter.assemblyDistrict              = fields[30];
    voter.lastVotedDate                 = fields[31];
    voter.prevYearVoted                 = fields[32];
    voter.prevCounty                    = fields[33];
    voter.prevAddress                   = fields[34];
    voter.prevName                      = fields[35];
    voter.countyVrNumber                = fields[36];
    voter.registrationDate              = fields[37];
    voter.vrSource                      = fields[38];
    voter.idRequired                    = fields[39];
    voter.idMet                         = fields[40];
    voter.status                        = fields[41];
    voter.reasonCode                    = fields[42];
    voter.inactivityDate                = fields[43];
    voter.purgeDate                     = fields[44];
    voter.sboeId                        = fields[45];
    voter.voterHistory                  = fields[46];
    
    // Voters is a a private variable defined in RawFilesInjest.cpp as std::vector<VoterInfoRaw> voters;
    voters.push_back(voter);

  } else {    
    std::cout << "Error with the numbers of fields at line " <<  std::endl;
    exit(1);
  }
}
 