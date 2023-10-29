#include "RawFilesInjest.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

//RawFilesInjest injest(filename, filecontent);

RawFilesInjest::RawFilesInjest() {		
}

RawFilesInjest::~RawFilesInjest() {
  // Destructor to clean up resources if needed
}

void RawFilesInjest::loadFile(const std::string& filename) {
  std::ifstream file(filename);

  if (!file) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }
  
  std::vector<std::string> filecontent;
  int TotalFileCounter = 0;
  std::string row;
  	
 	 // Custom CSV parsing function
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
  	
	while (getline(file, row)) {
		
	  VoterInfoRaw voter;
	  int TotalFileCounterAgain = 0;
    std::vector<std::string> fields = parseCSVLine(ToUpperAccents(ConvertLatin1ToUTF8(row)));

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
  		
  		if ( ++TotalFileCounter % 500000 == 0 ) {
  			std::cout << "Total Injested lines second type: " << TotalFileCounter << std::endl;
  		}

    } else {  	
    	std::cout << "Error with the numbers of fields at line " << TotalFileCounter << std::endl;
    	exit(1);
    }
	}
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
	
	for (unsigned char c : input) {
  	if (isalpha(c)) {	
    	try {
        result += toupper(static_cast<unsigned char>(c));
      } catch (const std::exception& e) {
			 	std::cerr << "Exception: " << c << " One: " << e.what() << std::endl;
			 	exit(1);
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
		}
	}
	return result;
}