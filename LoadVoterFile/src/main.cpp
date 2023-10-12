#include "DatabaseConnector.h"
#include "DataCollector.h"
#include "RawFilesInjest.h"
#include "Voter.h"
#include <iostream>
#include <fstream>      // Necessary for file handling
#include <locale>       // Necessary for setting locale
#include <unistd.h>     // Necessary for process ID retrieval
#include <future>

int main(int argc, char* argv[]) {
	std::locale loc("en_US.UTF-8");

	if (argc < 2) {
	std::cerr << "Usage: " << argv[0] << " <tabledate>" << std::endl;
	return 1;
	}

	std::cout << "DEBUG: accessfile() called by process " << getpid() << " (parent: " << getppid() << ")" << std::endl;
	std::cout << "clear; pidstat -r 1 -p " << getpid() << std::endl;
	// std::cout << "Press any key to continue";
	// std::cin.get();

	std::string tabledate = argv[1];
	std::string filename = "/home/usracct/VoterFiles/NY/" + tabledate + "/AllNYSVoters_" + tabledate + ".txt";
	std::string FileLastDateSeen = tabledate;
	std::cout << "Working on " << filename << std::endl;

	std::cout.imbue(loc);  // Apply the locale to the cout stream

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return 1;
	}
	
	// Load the voter files.
	RawFilesInjest injest;
	auto future1 = std::async(std::launch::async, [&]() {
		injest.loadFile(filename);
		std::cout << "Finish the ingest of the file" << std::endl;
	});

	// Separate the loads in multi threads

	DatabaseConnector 			dbConnectorComplex;		
	VoterIdxMap 						voterIdxMap;
	VoterComplementInfoMap 	voterComplementInfoMap;		
	DataMailingAddressMap 	dataMailingAddressMap;
	DataDistrictMap					dataDistrictMap;
	DataDistrictTemporalMap	dataDistrictTemporalMap;
	DataHouseMap						dataHouseMap;
	DataAddressMap          dataAddressMap;

	auto future2 = std::async(std::launch::async, [&]() {
		if (dbConnectorComplex.connect(voterIdxMap)) {
			dbConnectorComplex.LoadVotersIdx(voterIdxMap);
			dbConnectorComplex.LoadVotersComplementInfo(voterComplementInfoMap);
			dbConnectorComplex.LoadDataMailingAddress(dataMailingAddressMap);
			dbConnectorComplex.LoadDataDistrict(dataDistrictMap);
			dbConnectorComplex.LoadDataDistrictTemporal(dataDistrictTemporalMap);
			dbConnectorComplex.LoadDataAddress(dataAddressMap);
		} else {
			std::cerr << "Failed to connect to the database." << std::endl;
		}
		std::cout << "Finish the ingest of the Voter Idx" << std::endl;
	});

	VoterMap voterMap;
	DatabaseConnector dbConnectorVoter;
	auto future3 = std::async(std::launch::async, [&]() {
		if (dbConnectorVoter.connect(voterMap)) {
			dbConnectorVoter.LoadVoters(voterMap);
		} else {
			std::cerr << "Failed to connect to the database." << std::endl;
			// Handle the error appropriately, either throw an exception or set a flag
		}
		std::cout << "Finish the ingest of the Voters" << std::endl;
	});
	
	// Wait for the other database load
	future2.get();	
	future3.get();	
	
	// Load the single formats
	DatabaseConnector 
		dbConnectorFirstName, dbConnectorLastName, dbConnectorMiddleName,
		dbConnectorStateName, dbConnectorStateAbbrev, dbConnectorStreetName,  
		dbConnectorCity, dbConnectorDistrictTown, dbConnectorNonStdFormat; 

	VoterMap 
		FirstNames, LastNames, MiddleNames, StateName, StateAbbrev, 
		StreetName, VoterMap, DistrictTown, City, NonStdFormat;

	if (dbConnectorFirstName.connect(FirstNames)) { dbConnectorFirstName.LoadFirstName(FirstNames);  } else { return 1; }
	if (dbConnectorLastName.connect(LastNames)) { dbConnectorLastName.LoadLastName(LastNames); } else { return 1; }
	if (dbConnectorMiddleName.connect(MiddleNames)) {	dbConnectorMiddleName.LoadMiddleName(MiddleNames); } else { return 1; }
	if (dbConnectorStateName.connect(StateName)) { dbConnectorStateName.LoadStateName(StateName); } else { return 1; }
	if (dbConnectorStateAbbrev.connect(StateAbbrev)) { dbConnectorStateAbbrev.LoadStateAbbrev(StateAbbrev); } else { return 1; }	
	if (dbConnectorStreetName.connect(StreetName)) { dbConnectorStreetName.LoadStreetName(StreetName); } else { return 1; }
	if (dbConnectorDistrictTown.connect(DistrictTown)) { dbConnectorDistrictTown.LoadDistrictTown(DistrictTown); } else { return 1; }
	if (dbConnectorCity.connect(City)) { dbConnectorCity.LoadCity(City); } else { return 1; }
	if (dbConnectorNonStdFormat.connect(NonStdFormat)) { dbConnectorNonStdFormat.LoadNonStdFormat(NonStdFormat); } else { return 1; }

	// Wait for the tasks to finish.
	future1.get();
	
	std::cout << "Done Loading what is needed, start the work." << std::endl;
	// dataCollector.collectData();
	std::cout << "Total Voters in file: " << injest.getTotalVoters() << std::endl;
	VoterInfoRaw VotersFiles;
	
	Voter myVoter();
	VoterIdx myVoterIdx();
	
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
			
		std::cout << i << " Found this index for First Name: " << VotersFiles.firstName << ": " << dbConnectorFirstName.ReturnIndex(VotersFiles.firstName) << std::endl;
		std::cout << i << " Found this index for Middle Name: " << VotersFiles.middleName << ": " << dbConnectorLastName.ReturnIndex(VotersFiles.middleName) << std::endl;
		std::cout << i << " Found this index for Last Name: " << VotersFiles.lastName << ": " << dbConnectorMiddleName.ReturnIndex(VotersFiles.lastName) << std::endl;
		std::cout << i << " Found this index for Street: " << VotersFiles.residentialStreetName << ": " << dbConnectorStateName.ReturnIndex(VotersFiles.residentialStreetName) << std::endl;
		std::cout << i << " Found this index for City: " << VotersFiles.residentialCity << ": " << dbConnectorStateName.ReturnIndex(VotersFiles.residentialCity) << std::endl;
		std::cout << i << " Found this index for State FullName: " << " Abbrev " << ": " << dbConnectorStateAbbrev.ReturnIndex("NY") << std::endl;
		std::cout << i << " Found this index for State Abrev: " << " FULL NAME " << ": " << dbConnectorStreetName.ReturnIndex("NEW YORK") << std::endl;
		std::cout << i << " Found this index for Town: " << VotersFiles.townCity << ": " << dbConnectorDistrictTown.ReturnIndex(VotersFiles.townCity) << std::endl;
		std::cout << i << " Found this index for SpcFormat: " << VotersFiles.residentialNonStandartAddress << ": " << dbConnectorNonStdFormat.ReturnIndex(VotersFiles.residentialNonStandartAddress) << std::endl;
		
		std::cout << "myVoterIndex: " <<
			":" << dbConnectorLastName.ReturnIndex(VotersFiles.middleName) << 
			":" << dbConnectorMiddleName.ReturnIndex(VotersFiles.lastName) << 
			":" << dbConnectorFirstName.ReturnIndex(VotersFiles.firstName) << 
			":" << VotersFiles.nameSuffix <<
			":" << VotersFiles.dateOfBirth <<
			":" << VotersFiles.sboeId << std::endl;
		
		/*
		VoterIdx
		*/
		
		/*
		std::cout << "myVoter: " << ":" << 14409655 << ":" << 8277403 << ":" << Gender::Male << 
			":" << VotersFiles.sboeId <<	":" << VotersFiles.enrollment << ":" << ReasonCode::Other <<	
			":" << Status::Purged << ":" << 0 << ":" << false << ":" << true << ":" << 20130507 <<	
			":" << RegSource::MailIn <<	":" << 0 <<	":" << 20220212 << ":" << "411449898" <<	
			":" << true << std::endl;
		*/
			
		/*
		myVoter = Voter(
			14409655, 
			8277403, 
			Gender::Male, 
			VotersFiles.sboeId, 
			VotersFiles.enrollment, 
			ReasonCode::Other, 
			Status::Purged, 
			0, 
			false, 
			true, 
			20130507, 
			RegSource::MailIn, 
			0, 
			20220212, 
			"411449898", 
			true
		);
		*/
		
		// std::cout << "Associated index: " << voterMap[myVoter] << std::endl;
			
		if ( i % 10 == 0 ) {
			std::cout << "Press any key to continue";
			std::cin.get();
		}
	}

	

	//auto index = voterMap[myVoter];
	int number;
	while ( number >= 0) {
		std::cout << "Please enter a number: ";
		std::cin >> number;
		auto VoterInfoRaw = injest.getVoters(number);
		std::cout << "This is the outside " << VoterInfoRaw.sboeId << std::endl;
	}

	// ... (the rest of your original main function)
	return 0;

	for(int i = 0; i < injest.getTotalVoters(); i++) {
		auto VoterInfoRaw = injest.getVoters(i);
		std::cout << "Name: " << VoterInfoRaw.firstName << " Index: " << dbConnectorFirstName.ReturnIndex(VoterInfoRaw.firstName) << std::endl;    
	}


	
	std::cout << "Data collection and ingestion completed." << std::endl;

	return 0;
	}
