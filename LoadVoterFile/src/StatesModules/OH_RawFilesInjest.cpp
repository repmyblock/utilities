#include "../RawFilesInjest.h"

// OHIO

void RawFilesInjest::OH_RawFilesInjest(void) {
  FileName = "/home/usracct/VoterFiles/" + StateNameAbbrev + "/" + TableDate + "/SWVF_" + TableDate + ".txt";  
}

void RawFilesInjest::OH_parseLineToVoterInfo(std::queue<std::string>& queue) {
   
  while (!queue.empty()) {
    std::string line = queue.front();
    std::vector<std::string> fields = parseCSVLine(ToUpperAccents(ConvertLatin1ToUTF8(line)));
    VoterInfoRaw voter; 
      

    // https://www6.ohiosos.gov/ords/f?p=VOTERFTP:HOME::::::
    if (fields.size() >= 47) { // Adjust the size according to your data
      
      voter.sboeId                        = fields[0];  // 13 - SOS Voter Id
      voter.countyCode                    = fields[1];  //  2 - County Number
      voter.countyVrNumber                = fields[2];  // 50 - County Id
      voter.lastName                      = fields[3];  // 50 - Last Name
      voter.firstName                     = fields[4];  // 50 - First Name
      voter.middleName                    = fields[5];  // 50 - Middle Name
      voter.nameSuffix                    = fields[6];  // 10 - Suffix
      voter.dateOfBirth                   = fields[7];  // 10 - Date Of Birth
      voter.registrationDate              = fields[8];  // 10 - Registration Date
      voter.status                        = fields[9];  // 10 - Voter Status
      voter.enrollment                    = fields[10]; //  1 - Party Affiliation
      
      // Missing here are 
      // 100 - Residential Address1   - Field 11
      // 100 - Residential Address2   - Field 12
      
      std::cout << "OH : Residential Address 1: " << HI_YELLOW << fields[11] << NC << std::endl;
      std::cout << "OH : Residential Address 2: " << HI_YELLOW << fields[12] << NC << std::endl;
      
      /*
        voter.residentialAddressNumber      = fields[]; 
        voter.residentialHalfCode           = fields[];
        voter.residentialPredirection       = fields[];
        voter.residentialStreetName         = fields[];
        voter.residentialPostdirection      = fields[];
        voter.residentialAptType            = fields[];
        voter.residentialApartment          = fields[];
        voter.residentialNonStandartAddress = fields[];
      */

      voter.residentialCity               = fields[13]; // 50 - Residential City
      voter.residentialZip5               = fields[14]; //  5 - Residential Zip
      voter.residentialZip4               = fields[15]; //  4 - Residential Zip Plus 4

      // 50 - Residential Country      - Field 16
      // 10 - Residential Postal Code  - Field 17

      voter.mailingAddress1               = fields[18]; // 100 - Mailing Address1
      voter.mailingAddress2               = fields[19]; // 100 - Mailing Address 2 
          
      // 50 - Mailing City              - Field 20        
      // 20 - Mailing State             - Field 21
      //  5 - Mailing Zip               - Field 22
      //  4 - Mailing Zip Plus          - Field 23
      // 50 - Mailing Country           - Field 24
      // 50 - Mailing Postal Code       - Field 25

      /*
        voter.mailingAddress3               = fields[];
        voter.mailingAddress4               = fields[];
      */
       
      // 80 - Career Center                    - Field 26
      voter.townCity                      = fields[27];   // 80 - City
      // 80 - City School District             - Field 28
      // 80 - County Court  District           - Field 29
      voter.congressionalDistrict         = fields[30];   //  2 - Congressional District
      //  2 - Court of Appeals                 - Field 31
      // 80 - Education Service Center         - Field 32
      // 80 - Exempted Village School District - Field 33
      // 80 - Library District                 - Field 34
      // 80 - Local School District            - Field 35
      // 80 - Municipal Court District         - Field 36
      voter.electionDistrict              = fields[37];   // 80 - Precinct
      // 20 - Precinct Code                    - Field 38
      // 39 - State Board of Education         - Field 39
      voter.assemblyDistrict              = fields[40];   //   2 - State Representative District
      voter.senateDistrict                = fields[41];   //   2 - State Senate District
      // 20 - Township                         - Field 42
      // 20 - Village                          - Field 43
      voter.ward                          = fields[44];   // 20 - Ward

      /*
        voter.voterHistory                  = fields[46]; 45 and following
      */
        
      // Voters is a a private variable defined in RawFilesInjest.cpp as std::vector<VoterInfoRaw> voters;
      voters.push_back(voter);

    } else {    
      std::cout << "Error with the numbers of fields at line " <<  std::endl;
      exit(1);
    }
    
    voters.push_back(voter);
    queue.pop();  
  }  
}
 