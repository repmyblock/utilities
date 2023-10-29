#include "Voter.h"

Voter::Voter(int votersIndexesId, int dataHouseId, Gender gender, const std::string& uniqStateVoterId,
              const std::string& regParty, ReasonCode reasonCode, Status status, int mailingAddressId,
              bool idRequired, bool idMet, int applyDate, RegSource regSource,
              int dateInactive, int datePurged, const std::string& countyVoterNumber,
              bool rmbActive)
    : votersIndexesId(votersIndexesId), dataHouseId(dataHouseId), gender(gender), uniqStateVoterId(uniqStateVoterId),
      regParty(regParty), reasonCode(reasonCode), status(status), mailingAddressId(mailingAddressId),
      idRequired(idRequired), idMet(idMet), applyDate(applyDate), regSource(regSource), dateInactive(dateInactive),
      datePurged(datePurged), countyVoterNumber(countyVoterNumber), rmbActive(rmbActive) {}

bool Voter::operator==(const Voter& other) const {
    return votersIndexesId == other.votersIndexesId && dataHouseId == other.dataHouseId && gender == other.gender &&
           uniqStateVoterId == other.uniqStateVoterId && regParty == other.regParty && reasonCode == other.reasonCode &&
           status == other.status && mailingAddressId == other.mailingAddressId && idRequired == other.idRequired &&
           idMet == other.idMet && applyDate == other.applyDate && regSource == other.regSource &&
           dateInactive == other.dateInactive && datePurged == other.datePurged &&
           countyVoterNumber == other.countyVoterNumber && rmbActive == other.rmbActive;
}

// Do VoterIdx
VoterIdx::VoterIdx(int dataLastNameId, int dataFirstNameId, int dataMiddleNameId, const std::string& dataNameSuffix,
                    int dataDOB, const std::string& dataUniqStateId) 
     :  dataLastNameId(dataLastNameId), dataFirstNameId(dataFirstNameId), dataMiddleNameId(dataMiddleNameId),
        dataNameSuffix(dataNameSuffix), dataDOB(dataDOB), dataUniqStateId(dataUniqStateId) {}

bool VoterIdx::operator==(const VoterIdx& other) const {
    return  dataLastNameId == other.dataLastNameId && dataFirstNameId == other.dataFirstNameId && 
            dataMiddleNameId == other.dataMiddleNameId && dataNameSuffix == other.dataNameSuffix &&
            dataDOB == other.dataDOB && dataUniqStateId == other.dataUniqStateId;
}

// Do Data Address
DataAddress::DataAddress(const std::string& dataHouseNumber, const std::string& dataFracAddress, const std::string& dataPreStreet,
                int dataStreetId, const std::string& dataPostStreet, int dataCityId, int dataCountyId,
                const std::string& dataZipcode, const std::string& dataZip4,int CordinateId, int PGOSMosmid)
    : dataHouseNumber(dataHouseNumber), dataFracAddress(dataFracAddress), dataPreStreet(dataPreStreet), dataStreetId(dataStreetId),
      dataPostStreet(dataPostStreet), dataCityId(dataCityId), dataCountyId(dataCountyId), dataZipcode(dataZipcode),
      dataZip4(dataZip4), CordinateId(CordinateId), PGOSMosmid(PGOSMosmid) {}

bool DataAddress::operator==(const DataAddress& other) const {
    return dataHouseNumber == other.dataHouseNumber && dataFracAddress == other.dataFracAddress && dataPreStreet == other.dataPreStreet &&
           dataStreetId == other.dataStreetId && dataPostStreet == other.dataPostStreet && dataCityId == other.dataCityId &&
           dataCountyId == other.dataCountyId && dataZipcode == other.dataZipcode && dataZip4 == other.dataZip4 &&
           CordinateId == other.CordinateId && PGOSMosmid == other.PGOSMosmid;
}
// Do Data Address

// Do Voter Complement d'infos.
VoterComplementInfo::VoterComplementInfo(int VotersId, const std::string& VCIPrevName, const std::string& VCIPrevAddress,
                                          int VCIdataCountyId, int VCILastYearVote, int VCILastDateVote, const std::string& VCIOtherParty)
    : VotersId(VotersId), VCIPrevName(VCIPrevName), VCIPrevAddress(VCIPrevAddress), VCIdataCountyId(VCIdataCountyId),
      VCILastYearVote(VCILastYearVote), VCILastDateVote(VCILastDateVote), VCIOtherParty(VCIOtherParty) {}

bool VoterComplementInfo::operator==(const VoterComplementInfo& other) const {
    return VotersId == other.VotersId && VCIPrevName == other.VCIPrevName && VCIPrevAddress == other.VCIPrevAddress &&
           VCIdataCountyId == other.VCIdataCountyId && VCILastYearVote == other.VCILastYearVote && 
           VCILastDateVote == other.VCILastDateVote && VCIOtherParty == other.VCIOtherParty;
}

// Do Data Mailing Address
DataMailingAddress::DataMailingAddress(const std::string& dataMailAdrL1, const std::string& dataMailAdrL2, 
                                        const std::string& dataMailAdrL3, const std::string& dataMailAdrL4)
    : dataMailAdrL1(dataMailAdrL1), dataMailAdrL2(dataMailAdrL2), dataMailAdrL3(dataMailAdrL3), dataMailAdrL4(dataMailAdrL4) {}

bool DataMailingAddress::operator==(const DataMailingAddress& other) const {
    return dataMailAdrL1 == other.dataMailAdrL1 && dataMailAdrL2 == other.dataMailAdrL2 && dataMailAdrL3 == other.dataMailAdrL3 &&
           dataMailAdrL4 == other.dataMailAdrL4;
}

// Do Data District
DataDistrict::DataDistrict(int dataCountyId, int dataElectoral, int dataStateAssembly, int dataStateSenate, int dataLegislative,
                            const std::string& dataWard, int DataCongress)
    : dataCountyId(dataCountyId), dataElectoral(dataElectoral), dataStateAssembly(dataStateAssembly), 
      dataStateSenate(dataStateSenate),  dataLegislative(dataLegislative), dataWard(dataWard), DataCongress(DataCongress) {}

bool DataDistrict::operator==(const DataDistrict& other) const {
    return dataCountyId == other.dataCountyId && dataElectoral == other.dataElectoral && dataStateAssembly == other.dataStateAssembly &&
           dataStateSenate == other.dataStateSenate && dataLegislative == other.dataLegislative && dataWard == other.dataWard &&
           DataCongress == other.DataCongress;
}

// Do Data District Temportal
DataDistrictTemporal::DataDistrictTemporal(int dataDistrictCycleId, int dataHouseId, int dataDistrictId)
    : dataDistrictCycleId(dataDistrictCycleId), dataHouseId(dataHouseId), dataDistrictId(dataDistrictId) {}

bool DataDistrictTemporal::operator==(const DataDistrictTemporal& other) const {
    return dataDistrictCycleId == other.dataDistrictCycleId && dataHouseId == other.dataHouseId && dataDistrictId == other.dataDistrictId;
}

// Do Data House
DataHouse::DataHouse(int dataAddressId, const std::string& dataHouse_Type, const std::string& dataHouse_Apt, 
              int dataDistrictTownId, int dataStreetNonStdFormatId, int dataHouseBIN)
    : dataAddressId(dataAddressId), dataHouse_Type(dataHouse_Type), dataHouse_Apt(dataHouse_Apt), dataDistrictTownId(dataDistrictTownId),
      dataStreetNonStdFormatId(dataStreetNonStdFormatId), dataHouseBIN(dataHouseBIN) {}

bool DataHouse::operator==(const DataHouse& other) const {
    return dataAddressId == other.dataAddressId && dataHouse_Type == other.dataHouse_Type && dataHouse_Apt == other.dataHouse_Apt &&
           dataDistrictTownId == other.dataDistrictTownId && dataStreetNonStdFormatId == other.dataStreetNonStdFormatId && 
           dataHouseBIN == other.dataHouseBIN ;
}



namespace std {
    size_t hash<Voter>::operator()(const Voter& voter) const { return hash<int>()(voter.votersIndexesId); }
    size_t hash<VoterIdx>::operator()(const VoterIdx& voteridx) const { return hash<string>()(voteridx.dataUniqStateId); }
    size_t hash<VoterComplementInfo>::operator()(const VoterComplementInfo& votercomplementinfo) const { return hash<int>()(votercomplementinfo.VotersId); }
    size_t hash<DataMailingAddress>::operator()(const DataMailingAddress& datamailingaddress) const { return hash<string>()(datamailingaddress.dataMailAdrL1); }
    size_t hash<DataDistrict>::operator()(const DataDistrict& datadistrict) const { return hash<int>()(datadistrict.dataCountyId); }
    size_t hash<DataDistrictTemporal>::operator()(const DataDistrictTemporal& datadistricttemporal) const { return hash<int>()(datadistricttemporal.dataHouseId); }
    size_t hash<DataHouse>::operator()(const DataHouse& datahouse) const { return hash<int>()(datahouse.dataAddressId); }
    size_t hash<DataAddress>::operator()(const DataAddress& dataaddress) const { return hash<int>()(dataaddress.dataStreetId);  }
}