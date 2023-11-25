#ifndef VOTER_H
#define VOTER_H

#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define PINK    "\e[0;35m"
#define WHITE   "\e[0;37m"

#define HI_RED      "\e[1;91m"
#define HI_YELLOW   "\e[1;93m"
#define HI_BLUE     "\e[1;94m"
#define HI_PINK     "\e[1;95m"
#define HI_CYAN     "\e[1;96m"
#define HI_WHITE    "\e[1;97m"
#define HI_GREEN    "\e[1;92m"

#define HI_BK_BLACK "\e[0;100m"
#define HI_BK_BLUE  "\e[0;104m"

/*
  \e[0;30m  Black \e[0;31m  Red \e[0;32m  Green \e[0;33m  Yellow \e[0;34m  Blue \e[0;35m  Purple \e[0;36m  Cyan \e[0;37m  White
  \e[1;30m  Black \e[1;31m  Red \e[1;32m  Green \e[1;33m  Yellow \e[1;34m  Blue \e[1;35m  Purple \e[1;36m  Cyan \e[1;37m  White
  \e[40m    Black \e[41m    Red \e[42m    Green \e[43m    Yellow \e[44m    Blue \e[45m    Purple \e[46m    Cyan \e[47m    White
  \e[0;90m  Black \e[0;91m  Red \e[0;92m  Green \e[0;93m  Yellow \e[0;94m  Blue \e[0;95m  Purple \e[0;96m  Cyan \e[0;97m  White
  \e[1;90m  Black \e[1;91m  Red \e[1;92m  Green \e[1;93m  Yellow \e[1;94m  Blue \e[1;95m  Purple \e[1;96m  Cyan \e[1;97m  White
  \e[0;100m Black \e[0;101m Red \e[0;102m Green \e[0;103m Yellow \e[0;104m Blue \e[0;105m Purple \e[0;106m Cyan \e[0;107m White
*/

#define NC          "\e[0m"

#include <string>
#include <unordered_map>
#include <optional>

#define NIL     -2
#define NILSTRG ""

#define TO_INT_OR_NIL(str) ((str).empty() ? NIL : std::stoi(str))
#define TO_STR_OR_NIL(str) ((str).empty() ? NILSTRG : str)

// Used to represent NULL or unspecified reason code

enum class Gender {
  Male, Female, Other,  Undisclosed, Undetermined, Unspecified, Intersex, Undefined  
};

enum class ReasonCode {
  AdjudgedIncompetent, Death, Duplicate, Felon, MailCheck, MovedOutCounty,
  NCOA, NVRA, ReturnMail, VoterRequest, Other, Court, Inactive, Unspecified,
  Undefined
};

enum class Status {
  Active, ActiveMilitary, ActiveSpecialFederal, ActiveSpecialPresidential, 
  ActiveUOCAVA, Inactive, Purged, Prereg17YearOlds, Confirmation, Unspecified, Undefined
};

enum class RegSource {
  Agency, CBOE, DMV, LocalRegistrar, MailIn, School, OVR, Unspecified, Undefined
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
  int dataDOB;
  std::string dataUniqStateId;
    
  VoterIdx(int dataLastNameId, int dataFirstNameId, int dataMiddleNameId, const std::string& dataNameSuffix,
            int dataBOB, const std::string& dataUniqStateId);
      
  bool operator==(const VoterIdx& other) const;
};
  
struct VoterComplementInfo {
  int VotersId;
  std::string VCIPrevName;
  std::string VCIPrevAddress;
  int VCIdataCountyId;
  int VCILastYearVote;
  int VCILastDateVote;
  std::string VCIOtherParty;
    
  VoterComplementInfo(int VotersId, const std::string& VCIPrevName, const std::string& VCIPrevAddress,
                      int VCIdataCountyId, int VCILastYearVote, int VCILastDateVote, const std::string& VCIOtherParty);
            
  bool operator==(const VoterComplementInfo& other) const;
};
  
struct DataMailingAddress {
  uint32_t id;
  std::string dataMailAdrL1;
  std::string dataMailAdrL2;
  std::string dataMailAdrL3;
  std::string dataMailAdrL4;
    
  DataMailingAddress(uint32_t id, const std::string& dataMailAdrL1, const std::string& dataMailAdrL2, 
                      const std::string& dataMailAdrL3, const std::string& dataMailAdrL4);
      
  bool operator==(const DataMailingAddress& other) const;
};

struct DataDistrict {
  int dataCountyId;
  int dataElectoral;
  int dataStateAssembly;
  int dataStateSenate;
  int dataLegislative;
  std::string dataWard;
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
  std::string dataHouse_Type;
  std::string dataHouse_Apt;
  int dataDistrictTownId;
  int dataStreetNonStdFormatId;
  int dataHouseBIN;
    
  DataHouse(int dataAddressId, const std::string& dataHouse_Type, const std::string& dataHouse_Apt, 
            int dataDistrictTownId, int dataStreetNonStdFormatId, int dataHouseBIN);
            
  bool operator==(const DataHouse& other) const;
};

struct DataAddress {
  std::string dataHouseNumber;
  std::string dataFracAddress;
  std::string dataPreStreet;
  int dataStreetId;
  std::string dataPostStreet;
  int dataCityId;
  int dataCountyId;
  std::string dataZipcode;
  std::string dataZip4;
  int CordinateId;
  int PGOSMosmid;
    
  DataAddress(const std::string& dataHouseNumber, const std::string& dataFracAddress, const std::string& dataPreStreet,
              int dataStreetId, const std::string& dataPostStreet, int dataCityId, int dataCountyId,
              const std::string& dataZipcode, const std::string& dataZip4,int CordinateId, int PGOSMosmid);
            
  bool operator==(const DataAddress& other) const;
};

// Define the structure
struct VoterInfoRaw {
  std::string lastName;                         //  LASTNAME
  std::string firstName;                        //  FIRSTNAME
  std::string middleName;                       //  MIDDLENAME
  std::string nameSuffix;                       //  NAMESUFFIX
  std::string residentialAddressNumber;         //  RADDNUMBER
  std::string residentialHalfCode;              //  RHALFCODE
  std::string residentialApartment;             //  RAPARTMENT
  std::string residentialPredirection;          //  RPREDIRECTION
  std::string residentialStreetName;            //  RSTREETNAME <- Out of order
  std::string residentialPostdirection;         //  RPOSTDIRECTION <- Out of order
  std::string residentialCity;                  //  RCITY
  std::string residentialNonStandartAddress;    //  RADDRNONSTD
  std::string residentialAptNumber;             //  RAPARTMENTTYPE
  std::string residentialZip5;                  //  RZIP5
  std::string residentialZip4;                  //  RZIP4
  std::string mailingAddress1;                  //  MAILADD1
  std::string mailingAddress2;                  //  MAILADD2
  std::string mailingAddress3;                  //  MAILADD3
  std::string mailingAddress4;                  //  MAILADD4
  std::string dateOfBirth;                      //  DOB
  std::string gender;                           //  GENDER
  std::string enrollment;                       //  ENROLLMENT
  std::string otherParty;                       //  OTHERPARTY
  std::string countyCode;                       //  COUNTYCODE
  std::string electionDistrict;                 //  ED
  std::string legislativeDistrict;              //  LD
  std::string townCity;                         //  TOWNCITY
  std::string ward;                             //  WARD
  std::string congressionalDistrict;            //  CD
  std::string senateDistrict;                   //  SD
  std::string assemblyDistrict;                 //  AD
  std::string lastVotedDate;                    //  LASTVOTERDATE
  std::string prevYearVoted;                    //  PREVYEARVOTED
  std::string prevCounty;                       //  PREVCOUNTY
  std::string prevAddress;                      //  PREVADDRESS
  std::string prevName;                         //  PREVNAME
  std::string countyVrNumber;                   //  COUNTYVRNUMBER
  std::string registrationDate;                 //  REGDATE
  std::string vrSource;                         //  VRSOURCE
  std::string idRequired;                       //  IDREQUIRED
  std::string idMet;                            //  IDMET
  std::string status;                           //  STATUS
  std::string reasonCode;                       //  REASONCODE
  std::string inactivityDate;                   //  INACT_DATE
  std::string purgeDate;                        //  PURGE_DATE
  std::string sboeId;                           //  SBOEID
  std::string voterHistory;                     //  VoterHistory
};

struct VoterInfoRawReplaced {
  bool lastName;                        //  LASTNAME
  bool firstName;                       //  FIRSTNAME
  bool middleName;                      //  MIDDLENAME
  bool nameSuffix;                      //  NAMESUFFIX
  bool residentialAddressNumber;        //  RADDNUMBER
  bool residentialHalfCode;             //  RHALFCODE
  bool residentialApartment;            //  RAPARTMENT
  bool residentialPredirection;         //  RPREDIRECTION
  bool residentialStreetName;           //  RSTREETNAME <- Out of order
  bool residentialPostdirection;        //  RPOSTDIRECTION <- Out of order
  bool residentialCity;                 //  RCITY
  bool residentialNonStandartAddress;   //  RADDRNONSTD
  bool residentialAptNumber;            //  RAPARTMENTTYPE
  bool residentialZip5;                 //  RZIP5
  bool residentialZip4;                 //  RZIP4
  bool mailingAddress1;                 //  MAILADD1
  bool mailingAddress2;                 //  MAILADD2
  bool mailingAddress3;                 //  MAILADD3
  bool mailingAddress4;                 //  MAILADD4
  bool dateOfBirth;                     //  DOB
  bool gender;                          //  GENDER
  bool enrollment;                      //  ENROLLMENT
  bool otherParty;                      //  OTHERPARTY
  bool countyCode;                      //  COUNTYCODE
  bool electionDistrict;                //  ED
  bool legislativeDistrict;             //  LD
  bool townCity;                        //  TOWNCITY
  bool ward;                            //  WARD
  bool congressionalDistrict;           //  CD
  bool senateDistrict;                  //  SD
  bool assemblyDistrict;                //  AD
  bool lastVotedDate;                   //  LASTVOTERDATE
  bool prevYearVoted;                   //  PREVYEARVOTED
  bool prevCounty;                      //  PREVCOUNTY
  bool prevAddress;                     //  PREVADDRESS
  bool prevName;                        //  PREVNAME
  bool countyVrNumber;                  //  COUNTYVRNUMBER
  bool registrationDate;                //  REGDATE
  bool vrSource;                        //  VRSOURCE
  bool idRequired;                      //  IDREQUIRED
  bool idMet;                           //  IDMET
  bool status;                          //  STATUS
  bool reasonCode;                      //  REASONCODE
  bool inactivityDate;                  //  INACT_DATE
  bool purgeDate;                       //  PURGE_DATE
  bool sboeId;                          //  SBOEID
  bool voterHistory;                    //  VoterHistory
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

using VoterMap                = std::unordered_map<Voter, int>;
using VoterIdxMap             = std::unordered_map<VoterIdx, int>;
using VoterComplementInfoMap  = std::unordered_map<VoterComplementInfo, int>;
using DataMailingAddressMap   = std::unordered_map<DataMailingAddress, int>;
using DataDistrictMap         = std::unordered_map<DataDistrict, int>;
using DataDistrictTemporalMap = std::unordered_map<DataDistrictTemporal, int>;
using DataHouseMap            = std::unordered_map<DataHouse, int>;
using DataAddressMap          = std::unordered_map<DataAddress, int>;

#endif //VOTER_H
