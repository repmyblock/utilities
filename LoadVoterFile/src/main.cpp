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
	std::string StateNameAbbrev = "NY";

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <tabledate>";
		return 1;
	}

	std::cout << "DEBUG: accessfile() called by process " << getpid() << " (parent: " << getppid() << ")" << std::endl;
	std::cout << "clear; pidstat -r 1 -p " << getpid() << std::endl;
	
	std::string tabledate = argv[1];
	std::string filename = "/home/usracct/VoterFiles/" + StateNameAbbrev + "/" + tabledate + "/AllNYSVoters_" + tabledate + ".txt";
	std::string FileLastDateSeen = tabledate;
	std::cout << "Working on " << filename << std::endl;

	std::cout.imbue(loc);

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Error opening file: " << filename;
		return 1;
	}
	
	// Load the voter files.
	RawFilesInjest injest;
	auto future1 = std::async(std::launch::async, [&]() {
		injest.loadFile(filename);
		std::cout << "Finish the ingest of the file" << std::endl;
	});

	// Separate the loads in multi threads
	DatabaseConnector 			dbConnectorComplex(StateNameAbbrev);		

	VoterIdxMap 						voterIdxMap;
	VoterComplementInfoMap 	voterComplementInfoMap;		
	DataMailingAddressMap 	dataMailingAddressMap;
	DataDistrictMap					dataDistrictMap;
	DataDistrictTemporalMap	dataDistrictTemporalMap;
	DataHouseMap						dataHouseMap;
	DataAddressMap          dataAddressMap;

	auto future2 = std::async(std::launch::async, [&]() {
		dbConnectorComplex.LoadVotersIdx(voterIdxMap);
		dbConnectorComplex.LoadVotersComplementInfo(voterComplementInfoMap);
		dbConnectorComplex.LoadDataMailingAddress(dataMailingAddressMap);
		dbConnectorComplex.LoadDataDistrict(dataDistrictMap);
		dbConnectorComplex.LoadDataDistrictTemporal(dataDistrictTemporalMap);
		dbConnectorComplex.LoadDataAddress(dataAddressMap);
		dbConnectorComplex.LoadDataHouse(dataHouseMap);
		std::cout << "Finish the ingest of the Voter Idx" << std::endl;
	});

	VoterMap voterMap;
	DatabaseConnector dbConnectorVoter(StateNameAbbrev);
	auto future3 = std::async(std::launch::async, [&]() {
		dbConnectorVoter.LoadVoters(voterMap);
		std::cout << "Finish the ingest of the Voters" << std::endl;
	});
	
	// Wait for the other database load
	future2.get();	
	future3.get();	
	
	std::cout << "Future 2 & 3 are done" << std::endl;
	
	// Load the single formats
	DatabaseConnector 
		dbConnectorFirstName(StateNameAbbrev), dbConnectorLastName(StateNameAbbrev), dbConnectorMiddleName(StateNameAbbrev),
		dbConnectorStateName(StateNameAbbrev), dbConnectorStateAbbrev(StateNameAbbrev), dbConnectorStreetName(StateNameAbbrev),  
		dbConnectorCity(StateNameAbbrev), dbConnectorDistrictTown(StateNameAbbrev), dbConnectorNonStdFormat(StateNameAbbrev),
		dbConnectorCounty(StateNameAbbrev);

	VoterMap 
		FirstNames, LastNames, MiddleNames, StateName, StateAbbrev, 
		StreetName, DistrictTown, City, NonStdFormat, County;

	dbConnectorFirstName.LoadFirstName(FirstNames); 
	dbConnectorLastName.LoadLastName(LastNames);
	dbConnectorMiddleName.LoadMiddleName(MiddleNames);
	dbConnectorStreetName.LoadStreetName(StreetName); 
	dbConnectorStateAbbrev.LoadStateAbbrev(StateAbbrev); 
	dbConnectorStateName.LoadStateName(StateName); 
	dbConnectorDistrictTown.LoadDistrictTown(DistrictTown); 
	dbConnectorCity.LoadCity(City); 
	dbConnectorNonStdFormat.LoadNonStdFormat(NonStdFormat); 
	dbConnectorCounty.LoadCounty(County);

	// Wait for the tasks to finish.
	future1.get();
	std::cout << "Future 1 is done" << std::endl;

	
	std::cout << "Done Loading what is needed, start the work." << std::endl;
	// dataCollector.collectData();
	std::cout << "Total Voters in file: " << injest.getTotalVoters() << std::endl;
	VoterInfoRaw VotersFiles;
	
	// Before we do anything, let's check that each single one has an id, if not, we need to add them
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
		dbConnectorFirstName.CheckIndex(VotersFiles.firstName);	
		dbConnectorMiddleName.CheckIndex(VotersFiles.middleName);
		dbConnectorLastName.CheckIndex(VotersFiles.lastName);
		dbConnectorStreetName.CheckIndex(VotersFiles.residentialStreetName);
		dbConnectorCity.CheckIndex(VotersFiles.residentialCity);
		dbConnectorStateAbbrev.CheckIndex("NY");
		dbConnectorStateName.CheckIndex("NEW YORK");
		dbConnectorDistrictTown.CheckIndex(VotersFiles.townCity);
 		dbConnectorNonStdFormat.CheckIndex(VotersFiles.residentialNonStandartAddress);
	}	
	
	// VotersFiles = injest.getVoters(10);
	// std::cout << "Return First Name: " << GREEN << dbConnectorMiddleName.ReturnIndex("") << NC <<  std::endl;
			
	dbConnectorFirstName.TriggerSaveFirstNameDB();
	dbConnectorLastName.TriggerSaveLastNameDB();
	dbConnectorMiddleName.TriggerSaveMiddleNameDB();
	dbConnectorStreetName.TriggerSaveStreetNameDB();
	dbConnectorCity.TriggerSaveCityDB();
	dbConnectorDistrictTown.TriggerSaveDistrictTownDB();
	dbConnectorNonStdFormat.TriggerSaveNonStdFormatDB();
	
	// End information.
	
	// Voter myVoter;
	std::cout << HI_PINK << "Setup going to work on " << injest.getTotalVoters() << " injest " << NC << std::endl;	
	std::cout << std::endl;
		
	// Save Indexes
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
		voterIdxMap[
			VoterIdx (
				dbConnectorLastName.ReturnIndex(VotersFiles.lastName), dbConnectorFirstName.ReturnIndex(VotersFiles.firstName),
				dbConnectorMiddleName.ReturnIndex(VotersFiles.middleName), VotersFiles.nameSuffix, 
				TO_INT_OR_NIL(VotersFiles.dateOfBirth),	VotersFiles.sboeId
			)
		];
	}
	std::cout << std::endl << HI_PINK << "Saving Save DB Voter Idx" << NC << std::endl;
	dbConnectorVoter.SaveDbVoterIdx(voterIdxMap);
	
	
	// Save Mailing Addresses	
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
		dataMailingAddressMap[
			DataMailingAddress (
				VotersFiles.mailingAddress1,VotersFiles.mailingAddress2,VotersFiles.mailingAddress3,VotersFiles.mailingAddress4
			)
		];
	}
	std::cout << std::endl << HI_PINK << "Saving Save DB Data Mailing Address" << NC << std::endl;
	dbConnectorVoter.SaveDbDataMailingAddress(dataMailingAddressMap);
						
	// Save Data Addresses	
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
		dataAddressMap[
			DataAddress(
					VotersFiles.residentialAddressNumber, VotersFiles.residentialHalfCode, VotersFiles.residentialPredirection,
					dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName),	VotersFiles.residentialPostdirection, 
					dbConnectorCity.ReturnIndex(VotersFiles.residentialCity), 
					dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))),
					VotersFiles.residentialZip5, VotersFiles.residentialZip4, NIL, NIL
			)
		];
		
		if (dataAddressMap[
					DataAddress(
						VotersFiles.residentialAddressNumber, VotersFiles.residentialHalfCode, VotersFiles.residentialPredirection,
						dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName),	VotersFiles.residentialPostdirection, 
						dbConnectorCity.ReturnIndex(VotersFiles.residentialCity), 
						dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))),
						VotersFiles.residentialZip5, VotersFiles.residentialZip4, NIL, NIL
				)
			] == 0) {
		
		std::cout << "DataAddress:"
			<< HI_WHITE << " Number: "	<< NC << HI_PINK << VotersFiles.residentialAddressNumber << NC
			<< HI_WHITE << " Half: "	<< NC << HI_PINK << VotersFiles.residentialHalfCode << NC
			<< HI_WHITE << " Pre Dir: "	<< NC << HI_PINK << VotersFiles.residentialPredirection << NC
			<< HI_WHITE << " Street: "	<< NC << HI_YELLOW << VotersFiles.residentialStreetName << NC << " (" << HI_PINK << dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName) << NC << ")"
			<< HI_WHITE << " Post Dir:"	<< NC << HI_PINK << VotersFiles.residentialPostdirection << NC
			<< HI_WHITE << " City: "	<< NC << HI_YELLOW << VotersFiles.residentialCity << NC << " (" << HI_PINK << dbConnectorCity.ReturnIndex(VotersFiles.residentialCity) << NC << ")"
			<< HI_WHITE << " County: "	<< NC << HI_YELLOW << TO_INT_OR_NIL(VotersFiles.countyCode) << NC << " (" << HI_PINK << dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))) << NC << ")"
			<< HI_WHITE << " Zip: "	<< NC << HI_PINK <<VotersFiles.residentialZip5 << NC
			<< HI_WHITE << " Zip4: "	<< NC << HI_PINK << VotersFiles.residentialZip4 << NC << std::endl;
		
		}	
	}
	std::cout << std::endl << HI_PINK << "Saving Save DB Address Map" << NC << std::endl;
	dbConnectorVoter.SaveDbDataAddress(dataAddressMap);

	// Save Data Houses	
	
	std::cout  << std::endl << HI_PINK << "Starting Saving Save DB House Map" << NC << std::endl;	
	DataHouse	myDataHouse(NIL,NILSTRG,NILSTRG,NIL,NIL,NIL);	
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
		
		// This is to check the DataAddress ID and should not be zero ever.
		if (	dataAddressMap[DataAddress(
					VotersFiles.residentialAddressNumber, VotersFiles.residentialHalfCode, VotersFiles.residentialPredirection,
					dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName),	VotersFiles.residentialPostdirection, 
					dbConnectorCity.ReturnIndex(VotersFiles.residentialCity), 
					dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))),
					VotersFiles.residentialZip5, VotersFiles.residentialZip4, NIL, NIL
				)] == 0 ) {
		
			std::cout << "DataAddress:"
				<< HI_WHITE << " Number: "	<< NC << HI_PINK << VotersFiles.residentialAddressNumber << NC
				<< HI_WHITE << " Half: "	<< NC << HI_PINK << VotersFiles.residentialHalfCode << NC
				<< HI_WHITE << " Pre Dir: "	<< NC << HI_PINK << VotersFiles.residentialPredirection << NC
				<< HI_WHITE << " Street: "	<< NC << HI_YELLOW << VotersFiles.residentialStreetName << NC << " (" << HI_PINK << dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName) << NC << ")"
				<< HI_WHITE << " Post Dir:"	<< NC << HI_PINK << VotersFiles.residentialPostdirection << NC
				<< HI_WHITE << " City: "	<< NC << HI_YELLOW << VotersFiles.residentialCity << NC << " (" << HI_PINK << dbConnectorCity.ReturnIndex(VotersFiles.residentialCity) << NC << ")"
				<< HI_WHITE << " County: "	<< NC << HI_YELLOW << TO_INT_OR_NIL(VotersFiles.countyCode)<< NC << " (" << HI_PINK << 
					dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))) << NC << ")"
				<< HI_WHITE << " Zip: "	<< NC << HI_PINK <<VotersFiles.residentialZip5 << NC
				<< HI_WHITE << " Zip4: "	<< NC << HI_PINK << VotersFiles.residentialZip4 << NC << std::endl;
					
			std::cout << "DataHouse:"
				<< HI_WHITE << " Address Map: "	<< NC << HI_PINK << 
					dataAddressMap[DataAddress(
						VotersFiles.residentialAddressNumber, VotersFiles.residentialHalfCode, VotersFiles.residentialPredirection,
						dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName),	VotersFiles.residentialPostdirection, 
						dbConnectorCity.ReturnIndex(VotersFiles.residentialCity), 
						dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))),
						VotersFiles.residentialZip5, VotersFiles.residentialZip4, NIL, NIL
					)] 
				<< NC
				<< HI_WHITE << " Apt Number: "	<< NC << HI_PINK << VotersFiles.residentialAptNumber << NC
				<< HI_WHITE << " Type: "	<< NC << HI_PINK << VotersFiles.residentialApartment << NC
				<< HI_WHITE << " City: "	<< NC << HI_YELLOW << VotersFiles.townCity << NC << " (" << HI_PINK << dbConnectorDistrictTown.CheckIndex(VotersFiles.townCity) << NC << ")"
				<< HI_WHITE << " Non Std: "	<< NC << HI_YELLOW << VotersFiles.residentialNonStandartAddress << NC << " (" << HI_PINK << dbConnectorNonStdFormat.CheckIndex(VotersFiles.residentialNonStandartAddress) << NC << ")"
				<< std::endl;
					
			std::cout << HI_RED << "Exiting because Data Address is ZERO" << NC << std::endl;	
			exit(1);
				
		}
		
		dataHouseMap[
			DataHouse(
				dataAddressMap[
					DataAddress(
						VotersFiles.residentialAddressNumber, VotersFiles.residentialHalfCode, VotersFiles.residentialPredirection,
						dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName),	VotersFiles.residentialPostdirection, 
						dbConnectorCity.ReturnIndex(VotersFiles.residentialCity), 
						dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))),
						VotersFiles.residentialZip5, VotersFiles.residentialZip4, NIL, NIL
					)
				], 
				VotersFiles.residentialAptNumber, VotersFiles.residentialApartment,
				dbConnectorDistrictTown.CheckIndex(VotersFiles.townCity),	
				dbConnectorNonStdFormat.CheckIndex(VotersFiles.residentialNonStandartAddress), NIL
			)
		];
	}
	std::cout << std::endl << HI_PINK << "Saving Save DB District Map" << NC << std::endl;
	dbConnectorVoter.SaveDbDataHouse(dataHouseMap);
	
	// Save Districts
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
		
		dataDistrictMap[
			DataDistrict(
				dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))), 
				TO_INT_OR_NIL(VotersFiles.electionDistrict), TO_INT_OR_NIL(VotersFiles.assemblyDistrict),
				TO_INT_OR_NIL(VotersFiles.senateDistrict), TO_INT_OR_NIL(VotersFiles.legislativeDistrict),
				VotersFiles.ward, TO_INT_OR_NIL(VotersFiles.congressionalDistrict)
			)
		];
		
		if (dataDistrictMap[DataDistrict(
				dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))), 
				TO_INT_OR_NIL(VotersFiles.electionDistrict), TO_INT_OR_NIL(VotersFiles.assemblyDistrict),
				TO_INT_OR_NIL(VotersFiles.senateDistrict), TO_INT_OR_NIL(VotersFiles.legislativeDistrict),
				VotersFiles.ward, TO_INT_OR_NIL(VotersFiles.congressionalDistrict))] == 0) {
		
		std::cout << "DataDistrict:"
			<< HI_WHITE << " County: "	<< NC << HI_YELLOW << dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))) << NC 
			<< HI_WHITE << " ED: "	<< NC << HI_PINK << VotersFiles.electionDistrict << NC
			<< HI_WHITE << " AD: "	<< NC << HI_PINK << VotersFiles.assemblyDistrict << NC
			<< HI_WHITE << " Senate: "	<< NC << HI_YELLOW << VotersFiles.senateDistrict << NC 
			<< HI_WHITE << " Legislative:"	<< NC << HI_PINK << VotersFiles.legislativeDistrict << NC
			<< HI_WHITE << " Ward: "	<< NC << HI_YELLOW << VotersFiles.ward << NC
			<< HI_WHITE << " Congress: "	<< NC << HI_PINK <<VotersFiles.congressionalDistrict << NC<< std::endl;
		
		}
		
	}
	std::cout << std::endl << HI_PINK << "Saving Save DB Address Map" << NC << std::endl;
	dbConnectorVoter.SaveDbDataDistrict(dataDistrictMap);

	// Now we save Voter_ID
	for (int i = 0; i < injest.getTotalVoters() ; i++) {
		VotersFiles = injest.getVoters(i);
		

		std::cout <<
			"sboeId: " << HI_PINK << VotersFiles.sboeId << NC << std::endl <<
			"registrationDate: " << VotersFiles.registrationDate << " - " << TO_INT_OR_NIL(VotersFiles.registrationDate) << std::endl <<
			"inactivityDate: " << VotersFiles.inactivityDate << " - " << TO_INT_OR_NIL(VotersFiles.inactivityDate) << std::endl <<
			"purgeDate: " << VotersFiles.purgeDate << " " << TO_INT_OR_NIL(VotersFiles.purgeDate) << std::endl <<
			"dateOfBirth: " << VotersFiles.dateOfBirth << " " << TO_INT_OR_NIL(VotersFiles.dateOfBirth) << std::endl <<
			"lastNameL: " << dbConnectorLastName.ReturnIndex(VotersFiles.lastName) << std::endl <<
			"firstName: " << dbConnectorFirstName.ReturnIndex(VotersFiles.firstName) << std::endl <<
			"middleName: " << dbConnectorMiddleName.ReturnIndex(VotersFiles.middleName) <<  std::endl <<
			"suffix: " << TO_STR_OR_NIL(VotersFiles.nameSuffix) << std::endl <<
				
			"VoterIDX: " << voterIdxMap[
					VoterIdx (
						dbConnectorLastName.ReturnIndex(VotersFiles.lastName), dbConnectorFirstName.ReturnIndex(VotersFiles.firstName),
						dbConnectorMiddleName.ReturnIndex(VotersFiles.middleName), TO_STR_OR_NIL(VotersFiles.nameSuffix),
						TO_INT_OR_NIL(VotersFiles.dateOfBirth),	VotersFiles.sboeId
					)
				] << std::endl << 
			"Gender: " << VotersFiles.gender << " " << std::endl << 
			"ReasonCode: " << VotersFiles.reasonCode << " " << std::endl << 
			"Status: " << VotersFiles.status << " " << std::endl << 
			"Reg Source: " << VotersFiles.vrSource << " " << 		
			std::endl;
		
			// std::string genderToString(Gender gender);
			// std::string reasonCodeToString(ReasonCode code);
			// std::string statusToString(Status status);
			// std::string regSourceToString(RegSource source);

		
		voterMap[
			Voter(
				voterIdxMap[
					VoterIdx (
						dbConnectorLastName.ReturnIndex(VotersFiles.lastName), dbConnectorFirstName.ReturnIndex(VotersFiles.firstName),
						dbConnectorMiddleName.ReturnIndex(VotersFiles.middleName), TO_STR_OR_NIL(VotersFiles.nameSuffix),
						TO_INT_OR_NIL(VotersFiles.dateOfBirth),	VotersFiles.sboeId
					)
				],
				dataHouseMap[
					DataHouse(
						dataAddressMap[
							DataAddress(
								VotersFiles.residentialAddressNumber, VotersFiles.residentialHalfCode, VotersFiles.residentialPredirection,
								dbConnectorStreetName.ReturnIndex(VotersFiles.residentialStreetName),	VotersFiles.residentialPostdirection, 
								dbConnectorCity.ReturnIndex(VotersFiles.residentialCity), dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))),
								VotersFiles.residentialZip5, VotersFiles.residentialZip4, NIL, NIL
							)
						], 
						VotersFiles.residentialAptNumber, VotersFiles.residentialApartment,
						dbConnectorDistrictTown.CheckIndex(VotersFiles.townCity),	
						dbConnectorNonStdFormat.CheckIndex(VotersFiles.residentialNonStandartAddress), NIL
					)	
				],
				dbConnectorVoter.stringToGender(VotersFiles.gender),
				VotersFiles.sboeId,
				VotersFiles.enrollment,
				dbConnectorVoter.stringToReasonCode(VotersFiles.reasonCode),  // ReasonCode::Felon,
				dbConnectorVoter.stringToStatus(VotersFiles.status),      // Status::ActiveMilitary,
				dataMailingAddressMap[
					DataMailingAddress (
						VotersFiles.mailingAddress1,VotersFiles.mailingAddress2,VotersFiles.mailingAddress3,VotersFiles.mailingAddress4
					)
				],
				dbConnectorVoter.stringToBool(VotersFiles.idRequired),
				dbConnectorVoter.stringToBool(VotersFiles.idMet),
		  	TO_INT_OR_NIL(VotersFiles.registrationDate),
				dbConnectorVoter.stringToRegSource(VotersFiles.vrSource),   // RegSource::Agency,
				TO_INT_OR_NIL(VotersFiles.inactivityDate),
				TO_INT_OR_NIL(VotersFiles.purgeDate),			
				VotersFiles.countyVrNumber,
				true
			)
		];
		
		/*
		if (dataDistrictMap[DataDistrict(
				dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))), 
				TO_INT_OR_NIL(VotersFiles.electionDistrict), TO_INT_OR_NIL(VotersFiles.assemblyDistrict),
				TO_INT_OR_NIL(VotersFiles.senateDistrict), TO_INT_OR_NIL(VotersFiles.legislativeDistrict),
				VotersFiles.ward, TO_INT_OR_NIL(VotersFiles.congressionalDistrict))] == 0) {
		
		std::cout << "DataDistrict:"
			<< HI_WHITE << " County: "	<< NC << HI_YELLOW << dbConnectorCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFiles.countyCode))) << NC 
			<< HI_WHITE << " ED: "	<< NC << HI_PINK << VotersFiles.electionDistrict << NC
			<< HI_WHITE << " AD: "	<< NC << HI_PINK << VotersFiles.assemblyDistrict << NC
			<< HI_WHITE << " Senate: "	<< NC << HI_YELLOW << VotersFiles.senateDistrict << NC 
			<< HI_WHITE << " Legislative:"	<< NC << HI_PINK << VotersFiles.legislativeDistrict << NC
			<< HI_WHITE << " Ward: "	<< NC << HI_YELLOW << VotersFiles.ward << NC
			<< HI_WHITE << " Congress: "	<< NC << HI_PINK <<VotersFiles.congressionalDistrict << NC<< std::endl;
		
		}
		*/
		
	}
	std::cout << std::endl << HI_PINK << "Saving Save DB Voter Map" << NC << std::endl;
	dbConnectorVoter.SaveDbVoters(voterMap);
	
	std::cout << "Data collection and ingestion completed." << std::endl;
	return 0;
}
