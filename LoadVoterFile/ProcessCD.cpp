#include "DatabaseConnector.h"
#include "DataCollector.h"
#include "RawFilesInjest.h"
#include "Voter.h"
#include <iostream>
#include <fstream>      // Necessary for file handling
#include <locale>       // Necessary for setting locale
#include <unistd.h>     // Necessary for process ID retrieval
#include <future>
#include <iomanip>
#include <ctime>

std::string PrintCurrentTime() {
    std::time_t currentTime = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%H:%M:%S") << "\t"; 
    return oss.str();
}

int main(int argc, char* argv[]) {
  std::locale loc("en_US.UTF-8");

  // Check for two arguments: state abbreviation and table date
  if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << " <state abbreviation> <tabledate>";
      exit(1);
  }

  // Assign the state abbreviation and table date from command line arguments
  std::string StateNameAbbrev = argv[1];
  std::string tabledate = argv[2];

  unsigned int numCores = std::thread::hardware_concurrency() * 2;
  
  if (numCores == 0) {
    std::cout << "Unable to determine the number of CPU cores." << std::endl;
    exit(1);
  } 
    
  std::cout << "DEBUG: accessfile() called by process " << getpid() << " (parent: " << getppid() << ")" << std::endl;
  std::cout << "Number of CPU cores: " << numCores << std::endl;
  std::cout << "clear; pidstat -r 1 -p " << getpid() << std::endl;
  

  std::cout.imbue(loc); 
     
  RawFilesInjest injest(StateNameAbbrev, tabledate);
  std::cout << PrintCurrentTime() << "Working on " << injest.printFilename() << std::endl;
  
  injest.SetNumbersThreads(numCores);
  injest.loadFile();
  
  std::cout << PrintCurrentTime() << "Finish the ingest of the file" << std::endl;
  std::cout << PrintCurrentTime() << "Injested TotalVoters:\t" << HI_YELLOW << injest.getTotalVoters() << NC << std::endl;

  if ( injest.getTotalVoters() < 0) {
    std::cout << PrintCurrentTime() << HI_RED << "We did not find any new entry to add" << NC << std::endl;
    exit(1);
  }
  
  DatabaseConnector dbConnector; 
  
  DataCollector 
    CollectFirstName(dbConnector), CollectLastName(dbConnector), CollectMiddleName(dbConnector),
    CollectStateName(dbConnector), CollectStateAbbrev(dbConnector), CollectStreetName(dbConnector),  
    CollectCity(dbConnector), CollectDistrictTown(dbConnector), CollectNonStdFormat(dbConnector),
    CollectCounty(dbConnector);
      
  VoterMap
    FirstNames, LastNames, MiddleNames, StateName, StateAbbrev,
    StreetName, DistrictTown, City, NonStdFormat, County;

  std::cout << PrintCurrentTime() << "Loading the databases from the Database" << NC << std::endl;
  // auto VoterGroup1 = std::async(std::launch::async, [&]() {
    std::cout << PrintCurrentTime() << "Loading Thread 1 with FirstName, City, and County" << NC << std::endl;
    std::cout << PrintCurrentTime() << "Loading Thread 1 with FirstName" << NC << std::endl;
    CollectFirstName.LoadFirstName(FirstNames); 
    std::cout << PrintCurrentTime() << "Loading Thread 1 with City" << NC << std::endl;
    CollectCity.LoadCity(City);   
    std::cout << PrintCurrentTime() << "Loading Thread 1 with County" << NC << std::endl;
    CollectCounty.LoadCounty(County);  
    std::cout << PrintCurrentTime() << "End Thread 1" << NC << std::endl;
  // });
  // VoterGroup1.get();
  
  // auto VoterGroup2 = std::async(std::launch::async, [&]() {
    std::cout << PrintCurrentTime() << "Loading Thread 2 with Middle Names, street names, and State" << NC << std::endl;
    std::cout << PrintCurrentTime() << "Loading Thread 2 with MiddleNames" << NC << std::endl;
    CollectMiddleName.LoadMiddleName(MiddleNames);
    std::cout << PrintCurrentTime() << "Loading Thread 2 with StreetName" << NC << std::endl;
    CollectStreetName.LoadStreetName(StreetName); 
    std::cout << PrintCurrentTime() << "Loading Thread 2 with StateName" << NC << std::endl;
    CollectStateName.LoadStateName(StateName); 
    std::cout << PrintCurrentTime() << "End Thread 2" << NC << std::endl;
  // });
  // VoterGroup2.get();
  
  // auto VoterGroup3 = std::async(std::launch::async, [&]() {
    std::cout << PrintCurrentTime() << "Loading Thread 3 with Last Names, State Abbrevv" << NC << std::endl;
    std::cout << PrintCurrentTime() << "Loading Thread 3 with Last Names" << NC << std::endl;
    CollectLastName.LoadLastName(LastNames);
    std::cout << PrintCurrentTime() << "Loading Thread 3 with State Abbrev" << NC << std::endl;
    CollectStateAbbrev.LoadStateAbbrev(StateAbbrev);    
    std::cout << PrintCurrentTime() << "End Thread 3" << NC << std::endl;
  // });
  // VoterGroup3.get();
  
  // auto VoterGroup4 = std::async(std::launch::async, [&]() {
    std::cout << PrintCurrentTime() << "Loading Thread 4 with Town, NonStdFormat" << NC << std::endl;
    std::cout << PrintCurrentTime() << "Loading Thread 3 with Town" << NC << std::endl;
    CollectDistrictTown.LoadDistrictTown(DistrictTown); 
    std::cout << PrintCurrentTime() << "Loading Thread 3 with Non Standart" << NC << std::endl;
    CollectNonStdFormat.LoadNonStdFormat(NonStdFormat);
    std::cout << PrintCurrentTime() << "End Thread 4" << NC << std::endl;
  // });
  // VoterGroup4.get();
    
  std::cout << PrintCurrentTime() << "Numbers of First Names in Database:\t" << HI_YELLOW << CollectFirstName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectFirstName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Last Names in Database:\t" << HI_YELLOW << CollectLastName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectLastName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Middle Names in Database:\t" << HI_YELLOW << CollectMiddleName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectMiddleName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Street Names in Database:\t" << HI_YELLOW << CollectStreetName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectStreetName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Abbrev Names in Database:\t" << HI_YELLOW << CollectStateAbbrev.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectStateAbbrev.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of State Names in Database:\t" << HI_YELLOW << CollectStateName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectStateName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of District Town in Database:\t" << HI_YELLOW << CollectDistrictTown.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectDistrictTown.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Local City in Database:\t" << HI_YELLOW << CollectCity.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectCity.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Load Non Std in Database:\t" << HI_YELLOW << CollectNonStdFormat.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectNonStdFormat.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Load County in Database:\t" << HI_YELLOW << CollectCounty.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectCounty.returnQueryTimes() << NC << " milliseconds" << std::endl;

  // Before we do anything, let's check that each single one has an id, if not, we need to save them
  
  VoterInfoRaw VotersFilesGroup1, VotersFilesGroup2, VotersFilesGroup3, VotersFilesGroup4;
  std::cout << PrintCurrentTime() << "Entering the CheckIndexes TotalVoters:\t" << HI_YELLOW << injest.getTotalVoters() << NC << std::endl;
  
  // auto VoterGroup5 = std::async(std::launch::async, [&]() {
    for (int i = 0; i < injest.getTotalVoters() ; i++) {
      VotersFilesGroup1 = injest.getVoters(i);
      CollectFirstName.CheckIndex(VotersFilesGroup1.firstName); 
      CollectCity.CheckIndex(VotersFilesGroup1.residentialCity);            
    }
  // });
  
  CollectFirstName.TriggerSaveFirstNameDB();
  CollectCity.TriggerSaveCityDB();
  // VoterGroup5.get();
  
  // auto VoterGroup6 = std::async(std::launch::async, [&]() {
    for (int i = 0; i < injest.getTotalVoters() ; i++) {
      VotersFilesGroup2 = injest.getVoters(i);
      CollectMiddleName.CheckIndex(VotersFilesGroup2.middleName);   
      CollectStreetName.CheckIndex(VotersFilesGroup2.residentialStreetName);
  }
  // });

  CollectMiddleName.TriggerSaveMiddleNameDB();
  CollectStreetName.TriggerSaveStreetNameDB();
  // VoterGroup6.get();

  // auto VoterGroup7 = std::async(std::launch::async, [&]() {
    for (int i = 0; i < injest.getTotalVoters() ; i++) {
      VotersFilesGroup3 = injest.getVoters(i);
      CollectLastName.CheckIndex(VotersFilesGroup3.lastName);     
      CollectStateAbbrev.CheckIndex(StateNameAbbrev);     
    }
  // });

  CollectLastName.TriggerSaveLastNameDB();
  // VoterGroup7.get();
  
  // auto VoterGroup8 = std::async(std::launch::async, [&]() {
    for (int i = 0; i < injest.getTotalVoters() ; i++) {
      VotersFilesGroup4 = injest.getVoters(i);
      CollectDistrictTown.CheckIndex(VotersFilesGroup4.townCity);
      CollectNonStdFormat.CheckIndex(VotersFilesGroup4.residentialNonStandartAddress);
    }
  // });
  
  CollectDistrictTown.TriggerSaveDistrictTownDB();
  CollectNonStdFormat.TriggerSaveNonStdFormatDB();  
  // VoterGroup8.get();
  
  std::cout << PrintCurrentTime() << "Loaded " << NC << HI_YELLOW << injest.getTotalVoters() << NC << " raws voters" << std::endl; 
  std::cout << PrintCurrentTime() << HI_GREEN << "After triggers " << NC<< std::endl;
  
  std::cout << PrintCurrentTime() << "Numbers of First Names in Database:\t" << HI_YELLOW << CollectFirstName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectFirstName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Last Names in Database:\t" << HI_YELLOW << CollectLastName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectLastName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Middle Names in Database:\t" << HI_YELLOW << CollectMiddleName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectMiddleName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Street Names in Database:\t" << HI_YELLOW << CollectStreetName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectStreetName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Abbrev Names in Database:\t" << HI_YELLOW << CollectStateAbbrev.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectStateAbbrev.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of State Names in Database:\t" << HI_YELLOW << CollectStateName.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectStateName.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of District Town in Database:\t" << HI_YELLOW << CollectDistrictTown.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectDistrictTown.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Local City in Database:\t" << HI_YELLOW << CollectCity.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectCity.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Load Non Std in Database:\t" << HI_YELLOW << CollectNonStdFormat.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectNonStdFormat.returnQueryTimes() << NC << " milliseconds" << std::endl;
  std::cout << PrintCurrentTime() << "Numbers of Load County in Database:\t" << HI_YELLOW << CollectCounty.returnNumberOfEntries() << NC << "\t- " << HI_PINK << CollectCounty.returnQueryTimes() << NC << " milliseconds" << std::endl;

          
/*****************************************************************
 * Data Mailing Address                                          *
 *****************************************************************/

  std::cout << PrintCurrentTime() << HI_WHITE << "Start loading " << NC << HI_PINK << "DataMailingAddress" << NC << HI_WHITE << " database table" << NC << std::endl;

  DataMailingAddressMap   dataMailingAddressMap;
  DataCollector           CollectDataMailingAddress(dbConnector);   
  CollectDataMailingAddress.LoadData(dataMailingAddressMap);
  
  std::cout << PrintCurrentTime() << HI_WHITE << "Done loading " << NC << HI_PINK << "DataMailingAddress" << NC << HI_WHITE << " database table " << NC << HI_YELLOW << dataMailingAddressMap.size() << NC << " rows" << std::endl;

  // Save Table DataAddress 
  // auto FutureMailingAddress = std::async(std::launch::async, [&]() { 
  // Save Table DataMailingAddress 
 
  std::cout << PrintCurrentTime() << HI_WHITE << "Start work on " << NC << HI_PINK << "DataMailingAddress" << NC << HI_WHITE << " database table" << NC << std::endl;
  for (int i = 0; i < injest.getTotalVoters() ; i++) {
    VoterInfoRaw VotersFileFut = injest.getVoters(i);
    
    if (CollectDataMailingAddress.simpleHash(VotersFileFut.mailingAddress1) !=  3974729684 ) {
      
      if ( dataMailingAddressMap[
        DataMailingAddress (
          CollectDataMailingAddress.simpleHash(VotersFileFut.mailingAddress1),
          VotersFileFut.mailingAddress1, VotersFileFut.mailingAddress2, 
          VotersFileFut.mailingAddress3, VotersFileFut.mailingAddress4
        )] < 1) {
      
        dataMailingAddressMap[
          DataMailingAddress (
            CollectDataMailingAddress.simpleHash(VotersFileFut.mailingAddress1),
            VotersFileFut.mailingAddress1, VotersFileFut.mailingAddress2, 
            VotersFileFut.mailingAddress3, VotersFileFut.mailingAddress4
          ) 
        ];
      }
    }
      
    if ( i % 500000 == 0 ) {
      std::cout << PrintCurrentTime() << "\t\tDataMailingAddress processed: " << i << std::endl;
    }
  
  }
    
  int MapSize = dataMailingAddressMap.size();
  std::cout << PrintCurrentTime() << HI_WHITE << "\tStarting saving " << NC << HI_YELLOW << MapSize << NC << HI_WHITE << " entries in DataMailingAddress" << NC << std::endl;
  CollectDataMailingAddress.SaveDataBase(dataMailingAddressMap);  
  std::cout << PrintCurrentTime() << HI_WHITE << "\tDone saving " << NC << HI_YELLOW << dataMailingAddressMap.size() << NC << HI_WHITE << " entries in DataMailingAddress" << NC << std::endl;
    
  // Data Address difference to trigger error is ZERO because each field can be NULL.
  // if (dataMailingAddressMap.size() - MapSize != 0) { std::cout << PrintCurrentTime() << HI_RED << "\tWe have a problem in DataMailingAddress" << NC << std::endl;  } 
  std::cout << std::endl;
  
/******************************************************************
 * Data Address                                                   *
 ******************************************************************/
  std::cout << PrintCurrentTime() << HI_WHITE << "Start loading " << NC << HI_PINK << "DataAddress" << NC << HI_WHITE << " database table" << NC << std::endl;
  DataAddressMap          dataAddressMap;
  DataCollector           CollectDataAddress(dbConnector);    
  CollectDataAddress.LoadData(dataAddressMap);
  std::cout << PrintCurrentTime() << HI_WHITE << "Done loading " << NC << HI_PINK << "DataAddress" << NC << HI_WHITE << " database table " << NC << HI_YELLOW << dataAddressMap.size() << NC << " rows" << std::endl;

  for (int i = 0; i < injest.getTotalVoters() ; i++) {
    VoterInfoRaw VotersFileFut = injest.getVoters(i);
    
    if ( dataAddressMap[
        DataAddress(
          VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
          CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
          CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
          CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
          VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
        )] < 1) { 
      dataAddressMap[
        DataAddress(
          VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
          CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
          CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
          CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
          VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
        )
      ];
    } 
    if ( i % 500000 == 0 ) {
      std::cout << PrintCurrentTime() << "\t\tDataAddress processed: " << i << std::endl;
    }
  }
  
  MapSize = dataAddressMap.size();
  std::cout << PrintCurrentTime() << HI_WHITE << "\tStarting saving " << NC << HI_YELLOW << MapSize << NC << HI_WHITE << " entries in DataAddress" << NC << std::endl;
  CollectDataAddress.SaveDataBase(dataAddressMap);
  std::cout << PrintCurrentTime() << HI_WHITE << "\tDone saving " << NC << HI_YELLOW << dataAddressMap.size() << NC << HI_WHITE << " entries in DataAddress"<< NC << std::endl;
  if (dataAddressMap.size() - MapSize != 1) { std::cout << PrintCurrentTime() << HI_RED << "\tWe have a problem in DataAddress" << NC << std::endl; } 
  std::cout << std::endl;
 
/******************************************************************
 * Data House                                                     *
 ******************************************************************/
  DataHouseMap            dataHouseMap;
  DataCollector           CollectDataHouse(dbConnector);    
  CollectDataHouse.LoadData(dataHouseMap);
  
  std::cout << PrintCurrentTime() << HI_WHITE << "Start work on " << NC << HI_PINK << "DataHouse" << NC << HI_WHITE << " database table" << NC << std::endl;
  for (int i = 0; i < injest.getTotalVoters() ; i++) {
    VoterInfoRaw VotersFileFut = injest.getVoters(i);
    
    if (dataHouseMap[
        DataHouse(
          dataAddressMap[
            DataAddress(
              VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
              CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
              CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
              CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
              VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
            )
          ], 
          VotersFileFut.residentialAptNumber, VotersFileFut.residentialApartment,
          CollectDistrictTown.CheckIndex(VotersFileFut.townCity), 
          CollectNonStdFormat.CheckIndex(VotersFileFut.residentialNonStandartAddress), NIL
        )
      ] < 1) { 
        dataHouseMap[
          DataHouse(
            dataAddressMap[
              DataAddress(
                VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
                CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
                CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
                CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
                VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
              )
            ], 
            VotersFileFut.residentialAptNumber, VotersFileFut.residentialApartment,
            CollectDistrictTown.CheckIndex(VotersFileFut.townCity), 
            CollectNonStdFormat.CheckIndex(VotersFileFut.residentialNonStandartAddress), NIL
          )
        ];
    }
    
    if ( i % 500000 == 0 ) {
      std::cout << PrintCurrentTime() << "\t\tDataHouse processed: " << i << std::endl;
    }
  }
  
  MapSize = dataHouseMap.size();
  std::cout << PrintCurrentTime() << HI_WHITE << "\tStarting saving " << NC << HI_YELLOW << MapSize << NC << HI_WHITE << " entries in DataHouse"<< NC << std::endl;
  CollectDataAddress.SaveDataBase(dataHouseMap);
  std::cout << PrintCurrentTime() << HI_WHITE << "\tDone saving " << NC << HI_YELLOW << dataHouseMap.size() << NC << HI_WHITE << " entries in DataHouse"<< NC << std::endl;
  if (dataHouseMap.size() - MapSize != 1) { std::cout << PrintCurrentTime() << HI_RED << "\tWe have a problem in DataHouse" << NC << std::endl;  } 
  std::cout << std::endl;
 
/******************************************************************
 * Data District                                                  *
 ******************************************************************/
  DataDistrictMap         dataDistrictMap;
  DataCollector           CollectDataDistrict(dbConnector);   
  CollectDataDistrict.LoadData(dataHouseMap);
 
  std::cout << PrintCurrentTime() << HI_WHITE << "Start work on " << NC << HI_PINK << "DataDistrict" << NC << HI_WHITE << " database table" << NC << std::endl;
  for (int i = 0; i < injest.getTotalVoters() ; i++) {
    VoterInfoRaw VotersFileFut = injest.getVoters(i);
    
    if (dataDistrictMap[
        DataDistrict(
          CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))), 
          TO_INT_OR_NIL(VotersFileFut.electionDistrict), TO_INT_OR_NIL(VotersFileFut.assemblyDistrict),
          TO_INT_OR_NIL(VotersFileFut.senateDistrict), TO_INT_OR_NIL(VotersFileFut.legislativeDistrict),
          VotersFileFut.ward, TO_INT_OR_NIL(VotersFileFut.congressionalDistrict)
        )] < 1 ) {
    
      dataDistrictMap[
        DataDistrict(
          CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))), 
          TO_INT_OR_NIL(VotersFileFut.electionDistrict), TO_INT_OR_NIL(VotersFileFut.assemblyDistrict),
          TO_INT_OR_NIL(VotersFileFut.senateDistrict), TO_INT_OR_NIL(VotersFileFut.legislativeDistrict),
          VotersFileFut.ward, TO_INT_OR_NIL(VotersFileFut.congressionalDistrict)
        )
      ];
    }
    
    if ( i % 500000 == 0 ) {
      std::cout << PrintCurrentTime() << "\t\tDataDistrict processed: " << i << std::endl;
    }
  }
 
  MapSize = dataDistrictMap.size();
  std::cout << PrintCurrentTime() << HI_WHITE << "\tStarting saving " << NC << HI_YELLOW << MapSize << NC << HI_WHITE << " entries in DataDistrict"<< NC << std::endl;
  CollectDataDistrict.SaveDataBase(dataDistrictMap);
  std::cout << PrintCurrentTime() << HI_WHITE << "\tDone saving " << NC << HI_YELLOW << dataDistrictMap.size() << NC << HI_WHITE << " entries in DataDistrict"<< NC << std::endl;
  if (dataDistrictMap.size() - MapSize > 1) { std::cout << PrintCurrentTime() << HI_RED << "\tWe have a problem in DataDistrict" << NC << std::endl;  } 
  std::cout << std::endl;
    
/******************************************************************
 * VotersIndex Maps                                               *
 ******************************************************************/
  VoterIdxMap             voterIdxMap;
  DataCollector           CollectVoterIdx(dbConnector);   
  CollectVoterIdx.LoadData(voterIdxMap);
  
  std::cout << PrintCurrentTime() << HI_WHITE << "Start work on " << NC << HI_PINK << "VotersIndex" << NC << HI_WHITE << " database table" << NC << std::endl;
  for (int i = 0; i < injest.getTotalVoters() ; i++) {
    VoterInfoRaw VotersFileFut = injest.getVoters(i);
    
    if (voterIdxMap[
        VoterIdx (
          CollectLastName.ReturnIndex(VotersFileFut.lastName), CollectFirstName.ReturnIndex(VotersFileFut.firstName),
          CollectMiddleName.ReturnIndex(VotersFileFut.middleName), TO_STR_OR_NIL(VotersFileFut.nameSuffix),
          TO_INT_OR_NIL(VotersFileFut.dateOfBirth), VotersFileFut.sboeId
        )] < 1) {
    
      voterIdxMap[
        VoterIdx (
          CollectLastName.ReturnIndex(VotersFileFut.lastName), CollectFirstName.ReturnIndex(VotersFileFut.firstName),
          CollectMiddleName.ReturnIndex(VotersFileFut.middleName), TO_STR_OR_NIL(VotersFileFut.nameSuffix),
          TO_INT_OR_NIL(VotersFileFut.dateOfBirth), VotersFileFut.sboeId
        )
      ];
    }
    
    if ( i % 500000 == 0 ) {
      std::cout << PrintCurrentTime() << "\t\tVoter Indexes processed: " << i << std::endl;
    }
  }
  
  MapSize = voterIdxMap.size();
  std::cout << PrintCurrentTime() << HI_WHITE << "\tStarting saving " << NC << HI_YELLOW << MapSize << NC << HI_WHITE << " entries in VotersIndex"<< NC << std::endl;
  CollectVoterIdx.SaveDataBase(voterIdxMap);
  std::cout << PrintCurrentTime() << HI_WHITE << "\tDone saving " << NC << HI_YELLOW << voterIdxMap.size() << NC << HI_WHITE << " entries in voterIdxMap"<< NC << std::endl;
  if (voterIdxMap.size() - MapSize > 2) { std::cout << PrintCurrentTime() << HI_RED << "\tWe have a problem in VotersIndex" << NC << std::endl;  } 
  std::cout << std::endl;


/******************************************************************************
 ******************************************************************************
 **** THIS IS TO BE RUN WITHOUT VERIFICATION BECAUSE THE FILE IS ALWAYS NEW ***
 ******************************************************************************
 ******************************************************************************/



/******************************************************************
 * Voters Maps                                                    *
 ******************************************************************/
  VoterMap                voterMap;
  DataCollector           CollectVoter(dbConnector);    
  CollectVoter.LoadData(voterMap);

  std::cout << PrintCurrentTime() << HI_WHITE << "Start work on " << NC << HI_PINK << "Voters" << NC << HI_WHITE << " database table" << NC << std::endl;
  for (int i = 0; i < injest.getTotalVoters() ; i++) {
    VoterInfoRaw VotersFileFut = injest.getVoters(i);
    
    if (voterMap[
        Voter(
          voterIdxMap[
            VoterIdx (
              CollectLastName.ReturnIndex(VotersFileFut.lastName), CollectFirstName.ReturnIndex(VotersFileFut.firstName),
              CollectMiddleName.ReturnIndex(VotersFileFut.middleName), TO_STR_OR_NIL(VotersFileFut.nameSuffix),
              TO_INT_OR_NIL(VotersFileFut.dateOfBirth), VotersFileFut.sboeId
            )
          ],
          dataHouseMap[
            DataHouse(
              dataAddressMap[
                DataAddress(
                  VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
                  CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
                  CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
                  CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
                  VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
                )
              ], 
              VotersFileFut.residentialAptNumber, VotersFileFut.residentialApartment,
              CollectDistrictTown.CheckIndex(VotersFileFut.townCity), 
              CollectNonStdFormat.CheckIndex(VotersFileFut.residentialNonStandartAddress), NIL
            ) 
          ],
          CollectVoter.stringToGender(VotersFileFut.gender),
          VotersFileFut.sboeId,
          VotersFileFut.enrollment,
          CollectVoter.stringToReasonCode(VotersFileFut.reasonCode),    // ReasonCode::Felon,
          CollectVoter.stringToStatus(VotersFileFut.status),            // Status::ActiveMilitary,
          dataMailingAddressMap[
            DataMailingAddress (
             CollectVoter.simpleHash(VotersFileFut.mailingAddress1), VotersFileFut.mailingAddress1,VotersFileFut.mailingAddress2,VotersFileFut.mailingAddress3,VotersFileFut.mailingAddress4
            )
          ],
          CollectVoter.stringToBool(VotersFileFut.idRequired),
          CollectVoter.stringToBool(VotersFileFut.idMet),
          TO_INT_OR_NIL(VotersFileFut.registrationDate),
          CollectVoter.stringToRegSource(VotersFileFut.vrSource),       // RegSource::Agency,
          TO_INT_OR_NIL(VotersFileFut.inactivityDate),
          TO_INT_OR_NIL(VotersFileFut.purgeDate),     
          VotersFileFut.countyVrNumber,
          true
        )] < 1) {  
        
          voterMap[
            Voter(
              voterIdxMap[
                VoterIdx (
                  CollectLastName.ReturnIndex(VotersFileFut.lastName), CollectFirstName.ReturnIndex(VotersFileFut.firstName),
                  CollectMiddleName.ReturnIndex(VotersFileFut.middleName), TO_STR_OR_NIL(VotersFileFut.nameSuffix),
                  TO_INT_OR_NIL(VotersFileFut.dateOfBirth), VotersFileFut.sboeId
                )
              ],
              dataHouseMap[
                DataHouse(
                  dataAddressMap[
                    DataAddress(
                      VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
                      CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
                      CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
                      CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
                      VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
                    )
                  ], 
                  VotersFileFut.residentialAptNumber, VotersFileFut.residentialApartment,
                  CollectDistrictTown.CheckIndex(VotersFileFut.townCity), 
                  CollectNonStdFormat.CheckIndex(VotersFileFut.residentialNonStandartAddress), NIL
                ) 
              ],
              CollectVoter.stringToGender(VotersFileFut.gender),
              VotersFileFut.sboeId,
              VotersFileFut.enrollment,
              CollectVoter.stringToReasonCode(VotersFileFut.reasonCode),    // ReasonCode::Felon,
              CollectVoter.stringToStatus(VotersFileFut.status),            // Status::ActiveMilitary,
              dataMailingAddressMap[
                DataMailingAddress (
                 CollectVoter.simpleHash(VotersFileFut.mailingAddress1), VotersFileFut.mailingAddress1,VotersFileFut.mailingAddress2,VotersFileFut.mailingAddress3,VotersFileFut.mailingAddress4
                )
              ],
              CollectVoter.stringToBool(VotersFileFut.idRequired),
              CollectVoter.stringToBool(VotersFileFut.idMet),
              TO_INT_OR_NIL(VotersFileFut.registrationDate),
              CollectVoter.stringToRegSource(VotersFileFut.vrSource),       // RegSource::Agency,
              TO_INT_OR_NIL(VotersFileFut.inactivityDate),
              TO_INT_OR_NIL(VotersFileFut.purgeDate),     
              VotersFileFut.countyVrNumber,
              true
            )
          ];
    }
    
    if ( i % 500000 == 0 ) {
      std::cout << PrintCurrentTime() << "\t\tVoter processed: " << i << std::endl;
    }
  }
  
  MapSize = voterMap.size();
  std::cout << PrintCurrentTime() << HI_WHITE << "\tStarting saving " << NC << HI_YELLOW << MapSize << NC << HI_WHITE << " entries in voter"<< NC << std::endl;
  CollectVoter.SaveDataBase(voterMap);
  std::cout << PrintCurrentTime() << HI_WHITE << "\tDone saving " << NC << HI_YELLOW << voterMap.size() << NC << HI_WHITE << " entries in voter"<< NC << std::endl;
  if (voterIdxMap.size() - MapSize == 0) { std::cout << PrintCurrentTime() << HI_RED << "\tWe have a problem in voter" << NC << std::endl;  } 
  std::cout << std::endl;

/******************************************************************
 * Data District Temporal Map                                     *
 ******************************************************************/
  DataDistrictTemporalMap dataDistrictTemporalMap;
  DataCollector           CollectDistrictTemporalMap(dbConnector);    
  // CollectVoter.LoadData(dataDistrictTemporalMap);
  
/******************************************************************
 * Voter Complement Info Map                                      *
 ******************************************************************/
  VoterComplementInfoMap  voterComplementInfoMap;     
  DataCollector           VoterComplementInfoMap(dbConnector);    
  CollectVoter.LoadData(voterComplementInfoMap);

  std::cout << PrintCurrentTime() << HI_WHITE << "Start work on " << NC << HI_PINK << "VotersComplementInfo" << NC << HI_WHITE << " database table" << NC << std::endl;
  for (int i = 0; i < injest.getTotalVoters() ; i++) {
    VoterInfoRaw VotersFileFut = injest.getVoters(i);
              
    if (voterComplementInfoMap[
        VoterComplementInfo(  
          voterMap[
            Voter(
              voterIdxMap[
                VoterIdx (
                  CollectLastName.ReturnIndex(VotersFileFut.lastName), CollectFirstName.ReturnIndex(VotersFileFut.firstName),
                  CollectMiddleName.ReturnIndex(VotersFileFut.middleName), TO_STR_OR_NIL(VotersFileFut.nameSuffix),
                  TO_INT_OR_NIL(VotersFileFut.dateOfBirth), VotersFileFut.sboeId
                )
              ],
              dataHouseMap[
                DataHouse(
                  dataAddressMap[
                    DataAddress(
                      VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
                      CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
                      CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
                      CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
                      VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
                    )
                  ], 
                  VotersFileFut.residentialAptNumber, VotersFileFut.residentialApartment,
                  CollectDistrictTown.CheckIndex(VotersFileFut.townCity), 
                  CollectNonStdFormat.CheckIndex(VotersFileFut.residentialNonStandartAddress), NIL
                )
              ],
              CollectVoter.stringToGender(VotersFileFut.gender),
              VotersFileFut.sboeId,
              VotersFileFut.enrollment,
              CollectVoter.stringToReasonCode(VotersFileFut.reasonCode),  // ReasonCode::Felon,
              CollectVoter.stringToStatus(VotersFileFut.status),      // Status::ActiveMilitary,
              dataMailingAddressMap[
                DataMailingAddress (
                  CollectVoter.simpleHash(VotersFileFut.mailingAddress1), VotersFileFut.mailingAddress1,VotersFileFut.mailingAddress2,VotersFileFut.mailingAddress3,VotersFileFut.mailingAddress4
                )
              ],
              CollectVoter.stringToBool(VotersFileFut.idRequired),
              CollectVoter.stringToBool(VotersFileFut.idMet),
              TO_INT_OR_NIL(VotersFileFut.registrationDate),
              CollectVoter.stringToRegSource(VotersFileFut.vrSource),   // RegSource::Agency,
              TO_INT_OR_NIL(VotersFileFut.inactivityDate),
              TO_INT_OR_NIL(VotersFileFut.purgeDate),     
              VotersFileFut.countyVrNumber,
              true
            )
          ],                                                // Voter  ID
          TO_STR_OR_NIL(VotersFileFut.prevName),              // std::string VCIPrevName; 
          TO_STR_OR_NIL(VotersFileFut.prevAddress),           // std::string VCIPrevAddress;  
          TO_INT_OR_NIL(VotersFileFut.prevCounty),            // int VCIdataCountyId;
          TO_INT_OR_NIL(VotersFileFut.prevYearVoted),         // int VCILastYearVote;
          TO_INT_OR_NIL(VotersFileFut.lastVotedDate),         // int VCILastDateVote;
          TO_STR_OR_NIL(VotersFileFut.otherParty)             // std::string VCIOtherParty;
        )] < 1 ) {
           
      voterComplementInfoMap[
        VoterComplementInfo(  
          voterMap[
            Voter(
              voterIdxMap[
                VoterIdx (
                  CollectLastName.ReturnIndex(VotersFileFut.lastName), CollectFirstName.ReturnIndex(VotersFileFut.firstName),
                  CollectMiddleName.ReturnIndex(VotersFileFut.middleName), TO_STR_OR_NIL(VotersFileFut.nameSuffix),
                  TO_INT_OR_NIL(VotersFileFut.dateOfBirth), VotersFileFut.sboeId
                )
              ],
              dataHouseMap[
                DataHouse(
                  dataAddressMap[
                    DataAddress(
                      VotersFileFut.residentialAddressNumber, VotersFileFut.residentialHalfCode, VotersFileFut.residentialPredirection,
                      CollectStreetName.ReturnIndex(VotersFileFut.residentialStreetName), VotersFileFut.residentialPostdirection, 
                      CollectCity.ReturnIndex(VotersFileFut.residentialCity), 
                      CollectCounty.ReturnIndex(std::to_string(TO_INT_OR_NIL(VotersFileFut.countyCode))),
                      VotersFileFut.residentialZip5, VotersFileFut.residentialZip4, NIL, NIL
                    )
                  ], 
                  VotersFileFut.residentialAptNumber, VotersFileFut.residentialApartment,
                  CollectDistrictTown.CheckIndex(VotersFileFut.townCity), 
                  CollectNonStdFormat.CheckIndex(VotersFileFut.residentialNonStandartAddress), NIL
                )
              ],
              CollectVoter.stringToGender(VotersFileFut.gender),
              VotersFileFut.sboeId,
              VotersFileFut.enrollment,
              CollectVoter.stringToReasonCode(VotersFileFut.reasonCode),  // ReasonCode::Felon,
              CollectVoter.stringToStatus(VotersFileFut.status),      // Status::ActiveMilitary,
              dataMailingAddressMap[
                DataMailingAddress (
                  CollectVoter.simpleHash(VotersFileFut.mailingAddress1), VotersFileFut.mailingAddress1,VotersFileFut.mailingAddress2,VotersFileFut.mailingAddress3,VotersFileFut.mailingAddress4
                )
              ],
              CollectVoter.stringToBool(VotersFileFut.idRequired),
              CollectVoter.stringToBool(VotersFileFut.idMet),
              TO_INT_OR_NIL(VotersFileFut.registrationDate),
              CollectVoter.stringToRegSource(VotersFileFut.vrSource),   // RegSource::Agency,
              TO_INT_OR_NIL(VotersFileFut.inactivityDate),
              TO_INT_OR_NIL(VotersFileFut.purgeDate),     
              VotersFileFut.countyVrNumber,
              true
            )
          ],                                                // Voter  ID
          TO_STR_OR_NIL(VotersFileFut.prevName),              // std::string VCIPrevName; 
          TO_STR_OR_NIL(VotersFileFut.prevAddress),           // std::string VCIPrevAddress;  
          TO_INT_OR_NIL(VotersFileFut.prevCounty),            // int VCIdataCountyId;
          TO_INT_OR_NIL(VotersFileFut.prevYearVoted),         // int VCILastYearVote;
          TO_INT_OR_NIL(VotersFileFut.lastVotedDate),         // int VCILastDateVote;
          TO_STR_OR_NIL(VotersFileFut.otherParty)             // std::string VCIOtherParty;
        )
      ];
    }
    
    if ( i % 500000 == 0 ) {
      std::cout << PrintCurrentTime() << "\t\tVoter Complement processed: " << i << std::endl;
    }
    
  }
  
  
  MapSize = voterComplementInfoMap.size();
  std::cout << PrintCurrentTime() << HI_WHITE << "\tStarting saving " << NC << HI_YELLOW << MapSize << NC << HI_WHITE << " entries in VotersComplementInfo" << NC << std::endl;
  CollectVoter.SaveDataBase(voterComplementInfoMap);
  std::cout << PrintCurrentTime() << HI_WHITE << "\tDone saving " << NC << HI_YELLOW << voterComplementInfoMap.size() << NC << HI_WHITE << " entries in VotersComplementInfo"<< NC << std::endl;
  // if (voterComplementInfoMap.size() - MapSize > 1) { std::cout << PrintCurrentTime() << HI_RED << "\tWe have a problem in VotersComplementInfo" << NC << std::endl; exit(1); } 
  std::cout << std::endl;

  std::cout << PrintCurrentTime() << HI_WHITE << "Data collection and ingestion completed." << NC << std::endl;
  return 0;
}


