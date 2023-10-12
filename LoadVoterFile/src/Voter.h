#ifndef VOTER_H
#define VOTER_H

#include <string>
#include <unordered_map>

// Used to represent NULL or unspecified reason code

enum class Gender {
	Male, Female, Other, Undetermined, Unspecified  
};

enum class ReasonCode {
  AdjudgedIncompetent, Death, Duplicate, Felon, MailCheck, MovedOutCounty,
  NCOA, NVRA, ReturnMail, VoterRequest, Other, Court, Inactive, Unspecified
};

enum class Status {
  Active, ActiveMilitary, ActiveSpecialFederal, ActiveSpecialPresidential, 
  ActiveUOCAVA, Inactive, Purged, Prereg17YearOlds, Confirmation, Unspecified
};

enum class RegSource {
	Agency, CBOE, DMV, LocalRegistrar, MailIn, School, Unspecified
};

struct Voter {
  int votersIndexesId;
  int dataHouseId;
  Gender gender;
  std::string uniqStateVoterId;
  std::string regParty;
  ReasonCode reasonCode;
  Status status;
  int mailingAddressId;
  bool idRequired;
  bool idMet;
  int applyDate;
  RegSource regSource;
  int dateInactive;
  int datePurged;
  std::string countyVoterNumber;
  bool rmbActive;

  Voter(int votersIndexesId, int dataHouseId, Gender gender, const std::string& uniqStateVoterId,
        const std::string& regParty, ReasonCode reasonCode, Status status, int mailingAddressId,
        bool idRequired, bool idMet, int applyDate, RegSource regSource,
        int dateInactive, int datePurged, const std::string& countyVoterNumber,
        bool rmbActive);
  
  bool operator==(const Voter& other) const;
};

struct VoterIdx {
	int dataLastNameId;
	int dataFirstNameId;
	int dataMiddleNameId;
	std::string dataNameSuffix;
	int dataBOB;
	std::string dataUniqStateId;
		
	VoterIdx(int dataLastNameId, int dataFirstNameId, int dataMiddleNameId, const std::string& dataNameSuffix,
    				int dataBOB, const std::string& dataUniqStateId);
    	
 	bool operator==(const VoterIdx& other) const;
};
  
struct VoterComplementInfo {
	int VotersId;
	const std::string& VCIPrevName;
	const std::string& VCIPrevAddress;
	int VCIdataCountyId;
	int VCILastYearVote;
	int VCILastDateVote;
	const std::string& VCIOtherParty;
		
	VoterComplementInfo(int VotersId, const std::string& VCIPrevName, const std::string& VCIPrevAddress,
											int VCIdataCountyId, int VCILastYearVote, int VCILastDateVote, const std::string& VCIOtherParty);
		      	
 	bool operator==(const VoterComplementInfo& other) const;
};
  
struct DataMailingAddress {
	std::string dataMailAdrL1;
	std::string dataMailAdrL2;
	std::string dataMailAdrL3;
	std::string dataMailAdrL4;
		
	DataMailingAddress(const std::string& dataMailAdrL1, const std::string& dataMailAdrL2, 
    									const std::string& dataMailAdrL3, const std::string& dataMailAdrL4);
    	
 	bool operator==(const DataMailingAddress& other) const;
};

struct DataDistrict {
	int dataCountyId;
	int dataElectoral;
	int dataStateAssembly;
	int dataStateSenate;
	int dataLegislative;
	const std::string& dataWard;
	int DataCongress;
		
	DataDistrict(int dataCountyId, int dataElectoral, int dataStateAssembly, int dataStateSenate, int dataLegislative,
    const std::string& dataWard, int DataCongress);
    	
 	bool operator==(const DataDistrict& other) const;
};

struct DataDistrictTemporal {
	int dataDistrictCycleId;
	int dataHouseId;
	int dataDistrictId;
		
	DataDistrictTemporal(int dataDistrictCycleId, int dataHouseId, int dataDistrictId);  	
 	bool operator==(const DataDistrictTemporal& other) const;
};

struct DataHouse {
	int dataAddressId;
	const std::string& dataHouse_Type;
	const std::string& dataHouse_Apt;
	int dataDistrictTownId;
	int dataStreetNonStdFormatId;
	int dataHouseBIN;
		
	DataHouse(int dataAddressId, const std::string& dataHouse_Type, const std::string& dataHouse_Apt, 
						int dataDistrictTownId, int dataStreetNonStdFormatId, int dataHouseBIN);
		      	
 	bool operator==(const DataHouse& other) const;
};

struct DataAddress {
	const std::string& dataHouseNumber;
	const std::string& dataFracAddress;
	const std::string& dataPreStreet;
	int dataStreetId;
	const std::string& dataPostStreet;
	int dataCityId;
	int dataCountyId;
	const std::string& dataZipcode;
	const std::string& dataZip4;
	int CordinateId;
	int PGOSMosmid;
		
	DataAddress(const std::string& dataHouseNumber, const std::string& dataFracAddress, const std::string& dataPreStreet,
							int dataStreetId,	const std::string& dataPostStreet, int dataCityId, int dataCountyId,
							const std::string& dataZipcode, const std::string& dataZip4,int CordinateId, int PGOSMosmid);
		      	
 	bool operator==(const DataAddress& other) const;
};

// Define the structure
struct VoterInfoRaw {
  std::string lastName;  												// 	LASTNAME
  std::string firstName;												//	FIRSTNAME
  std::string middleName;												//	MIDDLENAME
  std::string nameSuffix;												//	NAMESUFFIX
  std::string residentialAddressNumber;					//	RADDNUMBER
  std::string residentialHalfCode;							//	RHALFCODE
  std::string residentialApartment;							//	RAPARTMENT
  std::string residentialPredirection;					//	RPREDIRECTION
  std::string residentialStreetName;         		//	RSTREETNAME <- Out of order
  std::string residentialPostdirection;					//	RPOSTDIRECTION <- Out of order
  std::string residentialCity;									//	RCITY
	std::string residentialNonStandartAddress; 		//	RADDRNONSTD
	std::string residentialAptNumber;          		//	RAPARTMENTTYPE
  std::string residentialZip5;									//	RZIP5
  std::string residentialZip4;                 	//	RZIP4
  std::string mailingAddress1;                 	//	MAILADD1
  std::string mailingAddress2;                 	//	MAILADD2
  std::string mailingAddress3;									//	MAILADD3
  std::string mailingAddress4;									//	MAILADD4
  std::string dateOfBirth;											//	DOB
  std::string gender;														//	GENDER
  std::string enrollment;												//	ENROLLMENT
  std::string otherParty;												//	OTHERPARTY
  std::string countyCode;												//	COUNTYCODE
  std::string electionDistrict;									//	ED
  std::string legislativeDistrict;							//	LD
  std::string townCity;													//	TOWNCITY
  std::string ward;															//	WARD
  std::string congressionalDistrict;						//	CD
  std::string senateDistrict;										//	SD
  std::string assemblyDistrict;									//	AD
  std::string lastVotedDate;										//	LASTVOTERDATE
  std::string prevYearVoted;										//	PREVYEARVOTED
  std::string prevCounty;												//	PREVCOUNTY
  std::string prevAddress;											//	PREVADDRESS
  std::string prevName;													//	PREVNAME
  std::string countyVrNumber;										//	COUNTYVRNUMBER
  std::string registrationDate;									//	REGDATE
  std::string vrSource;													//	VRSOURCE
  std::string idRequired;												//	IDREQUIRED
  std::string idMet;														//	IDMET
  std::string status;														//	STATUS
  std::string reasonCode;												//	REASONCODE
  std::string inactivityDate;										//	INACT_DATE
  std::string purgeDate;												//	PURGE_DATE
  std::string sboeId;														//	SBOEID
  std::string voterHistory;											//	VoterHistory
};

struct VoterInfoRawReplaced {
	bool lastName;  											// 	LASTNAME
  bool firstName;												//	FIRSTNAME
  bool middleName;											//	MIDDLENAME
  bool nameSuffix;											//	NAMESUFFIX
  bool residentialAddressNumber;				//	RADDNUMBER
  bool residentialHalfCode;							//	RHALFCODE
  bool residentialApartment;						//	RAPARTMENT
  bool residentialPredirection;					//	RPREDIRECTION
  bool residentialStreetName;         	//	RSTREETNAME <- Out of order
  bool residentialPostdirection;				//	RPOSTDIRECTION <- Out of order
  bool residentialCity;									//	RCITY
	bool residentialNonStandartAddress; 	//	RADDRNONSTD
	bool residentialAptNumber;          	//	RAPARTMENTTYPE
  bool residentialZip5;									//	RZIP5
  bool residentialZip4;                 //	RZIP4
  bool mailingAddress1;                 //	MAILADD1
  bool mailingAddress2;                 //	MAILADD2
  bool mailingAddress3;									//	MAILADD3
  bool mailingAddress4;									//	MAILADD4
  bool dateOfBirth;											//	DOB
  bool gender;													//	GENDER
  bool enrollment;											//	ENROLLMENT
  bool otherParty;											//	OTHERPARTY
  bool countyCode;											//	COUNTYCODE
  bool electionDistrict;								//	ED
  bool legislativeDistrict;							//	LD
  bool townCity;												//	TOWNCITY
  bool ward;														//	WARD
  bool congressionalDistrict;						//	CD
  bool senateDistrict;									//	SD
  bool assemblyDistrict;								//	AD
  bool lastVotedDate;										//	LASTVOTERDATE
  bool prevYearVoted;										//	PREVYEARVOTED
  bool prevCounty;											//	PREVCOUNTY
  bool prevAddress;											//	PREVADDRESS
  bool prevName;												//	PREVNAME
  bool countyVrNumber;									//	COUNTYVRNUMBER
  bool registrationDate;								//	REGDATE
  bool vrSource;												//	VRSOURCE
  bool idRequired;											//	IDREQUIRED
  bool idMet;														//	IDMET
  bool status;													//	STATUS
  bool reasonCode;											//	REASONCODE
  bool inactivityDate;									//	INACT_DATE
  bool purgeDate;												//	PURGE_DATE
  bool sboeId;													//	SBOEID
  bool voterHistory;										//	VoterHistory
};



namespace std {
  template <> struct hash<Voter> { std::size_t operator()(const Voter& voter) const; };
  template <> struct hash<VoterIdx> { std::size_t operator()(const VoterIdx& voteridx) const; };
  template <> struct hash<VoterComplementInfo> { std::size_t operator()(const VoterComplementInfo& votercomplementinfo) const; };
  template <> struct hash<DataMailingAddress> { std::size_t operator()(const DataMailingAddress& datamailingaddress) const; };
  template <> struct hash<DataDistrict> { std::size_t operator()(const DataDistrict& datadistrict) const; };
  template <> struct hash<DataDistrictTemporal> { std::size_t operator()(const DataDistrictTemporal& datadistricttemporal) const; };
  template <> struct hash<DataHouse> { std::size_t operator()(const DataHouse& datahouse) const; };
  template <> struct hash<DataAddress> { std::size_t operator()(const DataAddress& dataaddress) const; };
}

using VoterMap 								= std::unordered_map<Voter, int>;
using VoterIdxMap 						= std::unordered_map<VoterIdx, int>;
using VoterComplementInfoMap 	= std::unordered_map<VoterComplementInfo, int>;
using DataMailingAddressMap 	= std::unordered_map<DataMailingAddress, int>;
using DataDistrictMap 				= std::unordered_map<DataDistrict, int>;
using DataDistrictTemporalMap = std::unordered_map<DataDistrictTemporal, int>;
using DataHouseMap 						= std::unordered_map<DataHouse, int>;
using DataAddressMap 					= std::unordered_map<DataAddress, int>;

#endif //VOTER_H
