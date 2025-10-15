#include "../RawFilesInjest.h"

// NEW YORK

void RawFilesInjest::NY_RawFilesInjest(void) { 
  if (diffMode) {
  	FileName = "../VoterFiles/" + StateNameAbbrev + "/" + TableDate + "/Difference_" + TableDate + ".txt";
  } else {
    FileName = "../VoterFiles/" + StateNameAbbrev + "/" + TableDate + "/AllNYSVoters_" + TableDate + ".txt";
	}
}

void RawFilesInjest::NY_parseLineToVoterInfo(std::queue<std::string>& queue) {
	int counter = 0;
	
	if (queue.size() < 1) return;
	std::cout << "At start of the Size of the Queue " << HI_YELLOW << queue.size() << NC << std::endl;

	if (TableDateNumber < 20230213) {
	 	std::cout << HI_BLUE << "Entering NY for TableDateNumber " << NC << HI_YELLOW << std::to_string(TableDateNumber) << NC 
	 						<< HI_BLUE << " < " << std::to_string(20230213) << NC << std::endl;
	  
		 while (! queue.empty()) {
			std::string line = queue.front();
			std::vector<std::string> fields = parseCSVLine(ToUpperAccents(ConvertLatin1ToUTF8(line)));
			VoterInfoRaw voter;	
			
			// This is the format for the old format
			// https://web.archive.org/web/20200218173449/https://www.elections.ny.gov/NYSBOE/Forms/FOIL_VOTER_LIST_LAYOUT.pdf
			if (fields.size() == 45) { 
		    voter.lastName                      = fields[0];  // LASTNAME
		    voter.firstName                     = fields[1];  // FIRSTNAME
		    voter.middleName                    = fields[2];  // MIDDLENAME
		    voter.nameSuffix                    = fields[3];  // NAMESUFFIX
		    voter.residentialAddressNumber      = fields[4];  // RADDNUMBER
		    voter.residentialHalfCode           = fields[5];  // RHALFCODE
		    voter.residentialApartment          = fields[6];  // RAPARTMENT
	 	    voter.residentialPredirection       = fields[7];  // RPREDIRECTION
	 	    voter.residentialStreetName         = fields[8];  // RSTREETNAME
	 	    voter.residentialPostdirection      = fields[9];  // RPOSTDIRECTION
	 	    voter.residentialCity               = fields[10]; // RCITY
	 	    voter.residentialZip5               = fields[11]; // RZIP5
		    voter.residentialZip4               = fields[12]; // RZIP4
		    voter.mailingAddress1               = fields[13]; // MAILADD1
		    voter.mailingAddress2               = fields[14]; // MAILADD2
		    voter.mailingAddress3               = fields[15]; // MAILADD3
		    voter.mailingAddress4               = fields[16]; // MAILADD4
		    voter.dateOfBirth                   = fields[17]; // DOB
		    voter.gender                        = fields[18]; // GENDER
	 	    voter.enrollment                    = fields[19]; // ENROLLMENT
	 	    voter.otherParty                    = fields[20]; // OTHERPARTY
	 	    voter.countyCode                    = fields[21]; // COUNTYCODE
	 	    voter.electionDistrict              = fields[22]; // ED
		    voter.legislativeDistrict           = fields[23]; // LD
		    voter.townCity                      = fields[24]; // TOWNCITY
		    voter.ward                          = fields[25]; // WARD
		    voter.congressionalDistrict         = fields[26]; // CD - Congressional district
		    voter.senateDistrict                = fields[27]; // SD - Senate district
		    voter.assemblyDistrict              = fields[28]; // AD - Assembly district
		    voter.lastVotedDate                 = fields[29]; // LASTVOTERDATE
		    voter.prevYearVoted                 = fields[30]; // PREVYEARVOTED
		    voter.prevCounty                    = fields[31]; // PREVCOUNTY
		    voter.prevAddress                   = fields[32]; // PREVADDRESS
		    voter.prevName                      = fields[33]; // PREVNAME
		    voter.countyVrNumber                = fields[34]; // COUNTYVRNUMBER
		    voter.registrationDate              = fields[35]; // REGDATE
		    voter.vrSource                      = fields[36]; // VRSOURCE
		    voter.idRequired                    = fields[37]; // IDREQUIRED
		    voter.idMet                         = fields[38]; // IDMET
		    voter.status                        = fields[39]; // STATUS
		    voter.reasonCode                    = fields[40]; // REASONCODE
		    voter.inactivityDate                = fields[41]; // INACT_DATE
		    voter.purgeDate                     = fields[42]; // PURGE_DATE
		    voter.sboeId                        = fields[43]; // SBOEID
		    voter.voterHistory                  = fields[44]; // VoterHistory
		  } else {
		  	
		  	std::cout << HI_RED << "Error with the numbers of fields at line " << NC << std::endl;
	      std::cout << HI_YELLOW << counter << NC << " " << line << std::endl;
	  	  exit(1);
	  	}
	  	
	  	if ( ++counter % PRINTBLOCK == 0 ) {
	  		std::cout << "\tProceed reading " << HI_PINK << counter << NC;
	  		std::cout << "\tSize of the Queue " << HI_YELLOW << queue.size() << NC << std::endl;
	  	}
	  	
	  	voters.push_back(voter);
	  	queue.pop();
	  }
		
		std::cout << "\tSize of the Queue " << HI_YELLOW << queue.size() << NC << std::endl;
		
	} else {
	 	std::cout << HI_BLUE << "Entering NY for the else TableDateNumber " << NC << HI_YELLOW << std::to_string(TableDateNumber) << NC << std::endl;
	 		
		// Ver 2.5: 
		// https://web.archive.org/web/20221108125924/https://www.elections.ny.gov/NYSBOE/Forms/FOIL_VOTER_LIST_LAYOUT.pdf
		// https://www.elections.ny.gov/NYSBOE/Forms/FOIL_VOTER_LIST_LAYOUT.pdf
		while (!queue.empty()) {
			std::string line = queue.front();
			std::vector<std::string> fields = parseCSVLine(ToUpperAccents(ConvertLatin1ToUTF8(line)));
			VoterInfoRaw voter;	
			
			if (fields.size() >= 47) { 
		    voter.lastName                      = fields[0];  // LASTNAME
		    voter.firstName                     = fields[1]; 	// FIRSTNAME
		    voter.middleName                    = fields[2];  // MIDDLENAME
		    voter.nameSuffix                    = fields[3];  // NAMESUFFIX
		    voter.residentialAddressNumber      = fields[4];  // RADDNUMBER
		    voter.residentialHalfCode           = fields[5];  // RHALFCODE
		    voter.residentialPredirection       = fields[6];  // RPREDIRECTION
		    voter.residentialStreetName         = fields[7];  // RSTREETNAME
		    voter.residentialPostdirection      = fields[8];  // RPOSTDIRECTION
		    voter.residentialAptType            = fields[9];  // RAPARTMENTTYPE
		    voter.residentialApartment          = fields[10]; // RAPARTMENT
		    voter.residentialNonStandartAddress = fields[11]; // RADDRNONSTD
		    voter.residentialCity               = fields[12]; // RCITY
		    voter.residentialZip5               = fields[13]; // RZIP5
		    voter.residentialZip4               = fields[14]; // RZIP4
		    voter.mailingAddress1               = fields[15]; // MAILADD1
		    voter.mailingAddress2               = fields[16]; // MAILADD2
		    voter.mailingAddress3               = fields[17]; // MAILADD3
		    voter.mailingAddress4               = fields[18]; // MAILADD4
		    voter.dateOfBirth                   = fields[19]; // DOB
		    voter.gender                        = fields[20]; // GENDER
		    voter.enrollment                    = fields[21]; // ENROLLMENT
		    voter.otherParty                    = fields[22]; // OTHERPARTY
		    voter.countyCode                    = fields[23]; // COUNTYCODE
		    voter.electionDistrict              = fields[24]; // ED
		    voter.legislativeDistrict           = fields[25]; // LD
		    voter.townCity                      = fields[26]; // TOWNCITY
		    voter.ward                          = fields[27]; // WARD
		    voter.congressionalDistrict         = fields[28]; // CD - Congressional district
		    voter.senateDistrict                = fields[29]; // SD - Senate district
		    voter.assemblyDistrict              = fields[30]; // AD - Assembly district
		    voter.lastVotedDate                 = fields[31]; // LASTVOTERDATE
		    voter.prevYearVoted                 = fields[32]; // PREVYEARVOTED
		    voter.prevCounty                    = fields[33]; // PREVCOUNTY
		    voter.prevAddress                   = fields[34]; // PREVADDRESS
		    voter.prevName                      = fields[35]; // PREVNAME
		    voter.countyVrNumber                = fields[36]; // COUNTYVRNUMBER
		    voter.registrationDate              = fields[37]; // REGDATE
		    voter.vrSource                      = fields[38]; // VRSOURCE
		    voter.idRequired                    = fields[39]; // IDREQUIRED
		    voter.idMet                         = fields[40]; // IDMET
		    voter.status                        = fields[41]; // STATUS
		    voter.reasonCode                    = fields[42]; // REASONCODE
		    voter.inactivityDate                = fields[43]; // INACT_DATE
		    voter.purgeDate                     = fields[44]; // PURGE_DATE
		    voter.sboeId                        = fields[45]; // SBOEID
		    voter.voterHistory                  = fields[46]; // VoterHistory
		  } else {
	      std::cout << HI_RED << "Error with the numbers of fields at line " << NC << std::endl;
	      std::cout << HI_YELLOW << counter << NC << " " << line << std::endl;
	  	  exit(1);
	  	}
	  	
	  	if ( ++counter % PRINTBLOCK == 0 ) {
	  		std::cout << "\tProceed reading " << HI_PINK << counter << NC << std::endl;
	  	}
	  
	  	voters.push_back(voter);
	  	queue.pop();
	 	}
 	}
	// Voters is a a private variable defined in RawFilesInjest.cpp as std::vector<VoterInfoRaw> voters;
}
 
