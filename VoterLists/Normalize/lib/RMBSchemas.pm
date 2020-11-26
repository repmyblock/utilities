#!/usr/bin/perl

package RMBSchemas;

use strict;
use warnings;

sub CreateTable_Voters {
	my $sql = "DROP TABLE IF EXISTS Voters";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
			CREATE TABLE Voters (
			 	Voters_ID int unsigned NOT NULL AUTO_INCREMENT, 
			  ElectionsDistricts_DBTable varchar(256) DEFAULT NULL,
			  ElectionsDistricts_DBTableValue varchar(256) DEFAULT NULL,
			  Voters_Gender enum('male','female','other','undetermined') DEFAULT NULL,
			  VotersComplementInfo_ID int unsigned DEFAULT NULL,
			  Voters_UniqStateVoterID varchar(50) DEFAULT NULL,
			  DataState_ID tinyint unsigned DEFAULT NULL,
			  Voters_RegParty char(3) DEFAULT NULL,
			  Voters_ReasonCode enum('AdjudgedIncompetent','Death','Duplicate','Felon','MailCheck','MouvedOutCounty','NCOA','NVRA','ReturnMail','VoterRequest','Other','Court','Inactive') DEFAULT NULL,
			  Voters_Status enum('Active','ActiveMilitary','ActiveSpecialFederal','ActiveSpecialPresidential','ActiveUOCAVA','Inactive','Purged','Prereg17YearOlds') DEFAULT NULL,
			  VotersMailingAddress_ID int unsigned DEFAULT NULL,
			  Voters_IDRequired enum('yes','no') DEFAULT NULL,
			  Voters_IDMet enum('yes','no') DEFAULT NULL,
			  Voters_ApplyDate date DEFAULT NULL,
			  Voters_RegSource enum('Agency','CBOE','DMV','LocalRegistrar','MailIn','School') DEFAULT NULL,
			  Voters_DateInactive date DEFAULT NULL,
			  Voters_DatePurged date DEFAULT NULL,
			  Voters_CountyVoterNumber varchar(50) DEFAULT NULL,
			  Voters_RecFirstSeen date DEFAULT NULL,
			  Voters_RecLastSeen date DEFAULT NULL,
			  PRIMARY KEY (Voters_ID),
			  KEY Voters_UniqStateVoterID_IDX (Voters_UniqStateVoterID),
			  KEY Voters_ID_IDX (Voters_ID),
			  KEY Voters_RegParty_IDX (Voters_RegParty),
			  KEY Voters_StatusRegParty_IDX (Voters_Status,Voters_RegParty),
			  KEY Voters_Status_IDX (Voters_Status),
			  KEY Voters_DataState_IDX (DataState_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_VotersFirstName {
	my $sql = "DROP TABLE IF EXISTS VotersFirstName";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE VotersFirstName (
		  VotersFirstName_ID int unsigned NOT NULL AUTO_INCREMENT,
		  VotersFirstName_Text varchar(256) DEFAULT NULL,
		  VotersFirstName_Compress varchar(256) DEFAULT NULL,
		  PRIMARY KEY (VotersFirstName_ID),
		  KEY VotersFirstName_Text_IDX (VotersFirstName_Text),
		  KEY VotersFirstName_Compress_IDX (VotersFirstName_Compress)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_VotersMiddleName {
	my $sql = "DROP TABLE IF EXISTS VotersMiddleName";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE VotersMiddleName (
		  VotersMiddleName_ID int unsigned NOT NULL AUTO_INCREMENT,
		  VotersMiddleName_Text varchar(256) DEFAULT NULL,
		  VotersMiddleName_Compress varchar(256) DEFAULT NULL,
		  PRIMARY KEY (VotersMiddleName_ID),
		  KEY VotersMiddleName_Text_IDX (VotersMiddleName_Text),
		  KEY VotersMiddleName_Compress_IDX (VotersMiddleName_Compress)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_VotersLastName {
	my $sql = "DROP TABLE IF EXISTS VotersLastName";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE VotersLastName (
		  VotersLastName_ID int unsigned NOT NULL AUTO_INCREMENT,
		  VotersLastName_Text varchar(256) DEFAULT NULL,
		  VotersLastName_Compress varchar(256) DEFAULT NULL,
		  PRIMARY KEY (VotersLastName_ID),
		  KEY VotersLastName_Text_IDX (VotersLastName_Text),
		  KEY VotersLastName_Compress_IDX (VotersLastName_Compress)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_VotersIndexes {
	my $sql = "DROP TABLE IF EXISTS VotersIndexes";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE VotersIndexes (
		  VotersIndexes_ID int unsigned NOT NULL AUTO_INCREMENT,
		  Voters_ID int unsigned DEFAULT NULL,
		  DataState_ID tinyint unsigned DEFAULT NULL,
		  VotersLastName_ID int unsigned DEFAULT NULL,
		  VotersFirstName_ID int unsigned DEFAULT NULL,
		  VotersMiddleName_ID int unsigned DEFAULT NULL,
		  VotersIndexes_Suffix varchar(10) DEFAULT NULL,
		  VotersIndexes_DOB date DEFAULT NULL,
		  VotersIndexes_UniqStateVoterID char(20) DEFAULT NULL,
		  PRIMARY KEY (VotersIndexes_ID),
		  KEY VotersIndexes_UniqStateVoterID_IDX (VotersIndexes_UniqStateVoterID),
		  KEY VotersIndexes_VotersFirstName_IDX (VotersFirstName_ID),
		  KEY VotersIndexes_VotersMiddleName_IDX (VotersMiddleName_ID),
		  KEY VotersIndexes_VotersLastName_IDX (VotersLastName_ID),
		  KEY VotersIndexes_DOB_IDX (VotersIndexes_DOB),
		  KEY VotersIndexes_Voters_ID_IDX (Voters_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SystemUserQuery {
	my $sql = "DROP TABLE IF EXISTS SystemUserQuery";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SystemUserQuery (
		  SystemUserQuery_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SystemUserQuery_FirstName varchar(255) DEFAULT NULL,
		  SystemUserQuery_LastName varchar(255) DEFAULT NULL,
		  SystemUserQuery_DateOfBirth date DEFAULT NULL,
		  SystemUserQuery_DatedFileID int unsigned DEFAULT NULL,
		  SystemUserQuery_Email varchar(255) DEFAULT NULL,
		  SystemUserQuery_UniqNYSVoterID char(20) DEFAULT NULL,
		  SystemUserQuery_IP varchar(100) DEFAULT NULL,
		  SystemUserQuery_Date datetime DEFAULT NULL,
		  PRIMARY KEY (SystemUserQuery_ID)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SystemUser {
	my $sql = "DROP TABLE IF EXISTS SystemUser";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SystemUser (
		  SystemUser_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SystemUserProfile_ID int unsigned DEFAULT NULL,
		  Voters_ID int unsigned DEFAULT NULL,
		  Voters_UniqStateVoterID varchar(50) DEFAULT NULL,
		  SystemUser_EDAD int DEFAULT NULL,
		  SystemUser_NumVoters smallint DEFAULT NULL,
		  SystemUser_Party char(3) DEFAULT NULL,
		  SystemUser_ComplexMenu enum('yes','no') DEFAULT 'no',
		  SystemUser_email varchar(256) DEFAULT NULL,
		  SystemUser_emailverified enum('yes','no') DEFAULT 'no',
		  SystemUser_username varchar(256) DEFAULT NULL,
		  SystemUser_password varchar(256) DEFAULT NULL,
		  SystemUser_FirstName varchar(256) DEFAULT NULL,
		  SystemUser_LastName varchar(256) DEFAULT NULL,
		  SystemUser_Priv int unsigned DEFAULT NULL,
		  SystemUser_loginmethod enum('password','emaillink') DEFAULT 'password',
		  SystemUser_emaillinkid varchar(256) DEFAULT NULL,
		  SystemUser_mobilephone varchar(256) DEFAULT NULL,
		  SystemUser_mobileverified enum('yes','no') DEFAULT 'no',
		  SystemUser_facebookusername varchar(256) DEFAULT NULL,
		  SystemUser_facebookverified enum('yes','no') DEFAULT 'no',
		  SystemUser_googleusername varchar(256) DEFAULT NULL,
		  SystemUser_googleverified enum('yes','no') DEFAULT 'no',
		  SystemUser_googleapimapid varchar(256) DEFAULT NULL,
		  SystemUser_createtime datetime DEFAULT NULL,
		  SystemUser_lastlogintime datetime DEFAULT NULL,
		  PRIMARY KEY (SystemUser_ID),
		  UNIQUE KEY SystemUser_email (SystemUser_email),
		  UNIQUE KEY SystemUser_username (SystemUser_username),
		  KEY SystemUser_EDAD_IDX (SystemUser_EDAD),
		  KEY SystemUser_email_IDX (SystemUser_email),
		  KEY SystemUser_UniqID_IDX (Voters_UniqStateVoterID),
		  KEY SystemUser_username_IDX (SystemUser_username),
		  KEY SystemUser_mobilephone_IDX (SystemUser_mobilephone)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}


### Data Segment
sub CreateTable_DataState {
	my $sql = "DROP TABLE IF EXISTS DataState";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE DataState (
		  DataState_ID int unsigned NOT NULL AUTO_INCREMENT,
		  DataState_Name varchar(255) UNIQUE DEFAULT NULL,
		  PRIMARY KEY (DataState_ID),
		  KEY DataStateName_IDX (DataState_Name)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_DataCity {
	my $sql = "DROP TABLE IF EXISTS DataCity";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE DataCity (
		  DataCity_ID int unsigned NOT NULL AUTO_INCREMENT,
		  DataCity_Name varchar(255) UNIQUE DEFAULT NULL,
		  PRIMARY KEY (DataCity_ID),
		  KEY DataCityName_IDX (DataCity_Name)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_DataCounty {
	my $sql = "DROP TABLE IF EXISTS DataCounty";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE DataCounty (
		  DataCounty_ID int unsigned NOT NULL AUTO_INCREMENT,
		  DataState_ID int unsigned DEFAULT NULL,
		  DataCounty_Name varchar(40) DEFAULT NULL,
		  DataCounty_BOEID int unsigned DEFAULT NULL,
		  PRIMARY KEY (DataCounty_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_DataHouse {
	my $sql = "DROP TABLE IF EXISTS DataHouse";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE DataHouse (
		  DataHouse_ID int unsigned NOT NULL AUTO_INCREMENT,
		  DataAddress_ID int unsigned DEFAULT NULL,
		  DataHouse_Apt varchar(100) DEFAULT NULL,
		  DataHouse_BIN int unsigned DEFAULT NULL,
		  PRIMARY KEY (DataHouse_ID),
		  KEY DataHouseDataAddress_IDX (DataAddress_ID),
		  KEY DataHouseDataAddressApt_IDX (DataAddress_ID,DataHouse_Apt),
		  KEY DataHouse_BIN_IDX (DataHouse_BIN)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_DataAddress {
	my $sql = "DROP TABLE IF EXISTS DataAddress";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE DataAddress (
		  DataAddress_ID int unsigned NOT NULL AUTO_INCREMENT,
		  DataAddress_HouseNumber varchar(100) DEFAULT NULL,
		  DataAddress_FracAddress varchar(100) DEFAULT NULL,
		  DataAddress_PreStreet varchar(100) DEFAULT NULL,
		  DataStreet_ID int unsigned DEFAULT NULL,
		  DataAddress_PostStreet varchar(100) DEFAULT NULL,
		  DataCity_ID int unsigned DEFAULT NULL,
		  DataState_ID int unsigned DEFAULT NULL,
		  DataAddress_zipcode varchar(30) DEFAULT NULL,
		  DataAddress_zip4 varchar(10) DEFAULT NULL,
		  Cordinate_ID int unsigned DEFAULT NULL,
		  PG_OSM_osmid bigint DEFAULT NULL,
		  PRIMARY KEY (DataAddress_ID),
		  KEY DataAddressAll_IDX (DataAddress_HouseNumber,DataAddress_FracAddress,DataAddress_PreStreet,DataStreet_ID,DataAddress_PostStreet,DataCity_ID,DataState_ID,DataAddress_zipcode,DataAddress_zip4),
		  KEY DataAddressZipcodes_IDX (DataAddress_zipcode),
		  KEY DataAddressMost (DataAddress_HouseNumber,DataAddress_FracAddress,DataAddress_PreStreet,DataStreet_ID,DataAddress_PostStreet,DataCity_ID,DataState_ID,DataAddress_zipcode),
		  KEY DataAddressCordinate_IDX (Cordinate_ID),
		  KEY DataAddress_ID_IDX (DataAddress_ID),
		  KEY DataAddress_DataStreet_IDX (DataStreet_ID),
		  KEY DataAddress_City_IDX (DataCity_ID),
		  KEY DataAddress_State_IDX (DataState_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_DataStreet {
	my $sql = "DROP TABLE IF EXISTS DataStreet";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE DataStreet (
		  DataStreet_ID int unsigned NOT NULL AUTO_INCREMENT,
		  DataStreet_Name varchar(255) UNIQUE DEFAULT NULL,
		  PRIMARY KEY (DataStreet_ID),
		  KEY DataStreetName_IDX (DataStreet_Name),
		  KEY DataStreet_ID_IDX (DataStreet_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

### Cordinate Segment
sub CreateTable_Cordinate {
	my $sql = "DROP TABLE IF EXISTS Cordinate";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE Cordinate (
		  Cordinate_ID int unsigned NOT NULL AUTO_INCREMENT,
		  Cordinate_Latitude decimal(20,15) DEFAULT NULL,
		  Cordinate_Longitude decimal(20,15) DEFAULT NULL,
		  PRIMARY KEY (Cordinate_ID),
		  KEY Cordinate_LatLong_IDX (Cordinate_Latitude,Cordinate_Longitude)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_CordinateBox {
	my $sql = "DROP TABLE IF EXISTS CordinateBox";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE CordinateBox (
		  CordinateBox_ID int unsigned NOT NULL AUTO_INCREMENT,
		  CordinateBox_ShapeArea varchar(100) DEFAULT NULL,
		  CordinateBox_ElectDist varchar(100) DEFAULT NULL,
		  CordinateBox_ShapeLeng varchar(100) DEFAULT NULL,
		  CordinateBox_Shape varchar(100) DEFAULT NULL,
		  CordinateBox_ValidStartDate date DEFAULT NULL,
		  CordinateBox_ValidEndDate date DEFAULT NULL,
		  PRIMARY KEY (CordinateBox_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_CordinateGroup {
	my $sql = "DROP TABLE IF EXISTS CordinateGroup";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE CordinateGroup (
		  CordinateGroup_Segment int unsigned DEFAULT NULL,
		  CordinateGroup_Order int unsigned DEFAULT NULL,
		  Cordinate_ID int unsigned DEFAULT NULL,
		  CordinateBox_ID int unsigned DEFAULT NULL
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
		EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

### Election Segment

sub CreateTable_Elections {
	my $sql = "DROP TABLE IF EXISTS Elections";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE Elections (
		  Elections_ID int unsigned NOT NULL AUTO_INCREMENT,
		  Elections_Text varchar(256) DEFAULT NULL,
		  Elections_Date date DEFAULT NULL,
		  Elections_Type enum('primary','general','special','other') DEFAULT NULL,
		  PRIMARY KEY (Elections_ID),
		  UNIQUE KEY Elections_Text (Elections_Text),
		  KEY Elections_ID_IDX (Elections_Text)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_ElectionsDistricts {
	my $sql = "DROP TABLE IF EXISTS ElectionsDistricts";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE ElectionsDistricts (
		  ElectionsDistricts_ID int unsigned NOT NULL AUTO_INCREMENT,
		  ElectionsDistricts_Party char(3) DEFAULT NULL,
		  Elections_ID int unsigned DEFAULT NULL,
		  CandidatePositions_ID int unsigned DEFAULT NULL,
		  DataCounty_ID int unsigned DEFAULT NULL,
		  ElectionsDistricts_DBTable varchar(256) DEFAULT NULL,
		  ElectionsDistricts_DBTableValue varchar(256) DEFAULT NULL,
		  ElectionsDistricts_NumberFemale int unsigned DEFAULT NULL,
		  Elections_ID_Female int unsigned DEFAULT NULL,
		  ElectionsDistricts_NumberMale int unsigned DEFAULT NULL,
		  Elections_ID_Male int unsigned DEFAULT NULL,
		  ElectionsDistricts_NumberUnixSex int unsigned DEFAULT NULL,
		  Elections_ID_Unisex int unsigned DEFAULT NULL,
		  PRIMARY KEY (ElectionsDistricts_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_ElectionsDistrictsConv {
	my $sql = "DROP TABLE IF EXISTS ElectionsDistrictsConv";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE ElectionsDistrictsConv (
		  ElectionsDistrictsConv_ID int unsigned NOT NULL AUTO_INCREMENT,
		  Elections_ID int unsigned DEFAULT NULL,
		  DataCounty_ID int unsigned DEFAULT NULL,
		  ElectionsDistricts_Party char(3) DEFAULT NULL,
		  ElectionsDistricts_DBTable varchar(256) DEFAULT NULL,
		  ElectionsDistricts_DBTableValue varchar(256) DEFAULT NULL,
		  ElectionsDistrictsConv_DBTable varchar(256) DEFAULT NULL,
		  ElectionsDistrictsConv_DBTableValue varchar(256) DEFAULT NULL,
		  PRIMARY KEY (ElectionsDistrictsConv_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_ElectionsPosition {
	my $sql = "DROP TABLE IF EXISTS ElectionsPosition";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE ElectionsPosition (
		  ElectionsPosition_ID int unsigned NOT NULL AUTO_INCREMENT,
		  ElectionsPosition_DBTable varchar(100) DEFAULT NULL,
		  DataState_ID int unsigned DEFAULT NULL,
		  ElectionsPosition_Type enum('party','office') DEFAULT NULL,
		  ElectionsPosition_Name varchar(100) DEFAULT NULL,
		  ElectionsPosition_Order int unsigned DEFAULT NULL,
		  ElectionsPosition_Explanation text,
		  PRIMARY KEY (ElectionsPosition_ID),
		  UNIQUE KEY ElectionsPosition_DBTable (ElectionsPosition_DBTable)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}


### SMS Account
sub CreateTable_SMSAccountHolder {
	my $sql = "DROP TABLE IF EXISTS SMSAccountHolder";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSAccountHolder (
		  SMSAccountHolder_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SMSProvider_ID int unsigned DEFAULT NULL,
		  SystemUser_ID int unsigned DEFAULT NULL,
		  SMSAccountHolder_UserName varchar(255) DEFAULT NULL,
		  SMSAccountHolder_EncryptedPassWord varchar(255) DEFAULT NULL,
		  SMSAccountHolder_PortalWebsite varchar(255) DEFAULT NULL,
		  SMSAccountHolder_Rate smallint DEFAULT NULL,
		  SMSAccountHolder_RateValue enum('year','month','week','day','hour','minute','second') DEFAULT NULL,
		  SMSAccountHolder_DateValidFrom datetime DEFAULT NULL,
		  SMSAccountHolder_DateValidTo datetime DEFAULT NULL,
		  PRIMARY KEY (SMSAccountHolder_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSAuthorizedUsers {
	my $sql = "DROP TABLE IF EXISTS SMSAuthorizedUsers";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSAuthorizedUsers (
		  SMSAuthorizedUsers_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SystemUser_ID int unsigned DEFAULT NULL,
		  SMSCampaign_ID int unsigned DEFAULT NULL,
		  SMSAuthorizedUsers_FromDate datetime DEFAULT NULL,
		  SMSAuthorizedUsers_ToDate datetime DEFAULT NULL,
		  PRIMARY KEY (SMSAuthorizedUsers_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSCampaign {
	my $sql = "DROP TABLE IF EXISTS SMSCampaign";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSCampaign (
		  SMSCampaign_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SystemUser_ID int unsigned DEFAULT NULL,
		  Candidate_ID int unsigned DEFAULT NULL,
		  SMSAccountHolder int unsigned DEFAULT NULL,
		  SMSCampaign_Text text,
		  SMSTestPlan_ID int unsigned DEFAULT NULL,
		  SMSCampaign_DateWriten datetime DEFAULT NULL,
		  PRIMARY KEY (SMSCampaign_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSPopInfo {
	my $sql = "DROP TABLE IF EXISTS SMSPopInfo";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSPopInfo (
		  SMSPopInfo_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SMSAccountHolder_ID int unsigned DEFAULT NULL,
		  SystemUser_ID int unsigned DEFAULT NULL,
		  Candidate_ID int unsigned DEFAULT NULL,
		  SMSPopInfo_Phone varchar(20) DEFAULT NULL,
		  SMSPopInfo_Rate smallint DEFAULT NULL,
		  SMSPopInfo_RateValue enum('year','month','week','day','hour','minute','second') DEFAULT NULL,
		  SMSPopInfo_APIKey varchar(256) DEFAULT NULL,
		  SMSPopInfo_DateValidFrom datetime DEFAULT NULL,
		  SMSPopInfo_DateValidTo datetime DEFAULT NULL,
		  PRIMARY KEY (SMSPopInfo_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSProvider {
	my $sql = "DROP TABLE IF EXISTS SMSProvider";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSProvider (
		  SMSProvider_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SMSProvider_Name varchar(100) DEFAULT NULL,
		  SMSProvider_Website varchar(256) DEFAULT NULL,
		  PRIMARY KEY (SMSProvider_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSTestPlan {
	my $sql = "DROP TABLE IF EXISTS SMSTestPlan";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSTestPlan (
		  SMSTestPlan_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SMSCampaign_ID int unsigned DEFAULT NULL,
		  SMSTestPlan_HourFrom time DEFAULT NULL,
		  SMSTestPlan_HourTo time DEFAULT NULL,
		  SMSTestPlan_DateValidFrom datetime DEFAULT NULL,
		  SMSTestPlan_DateValidTo datetime DEFAULT NULL,
		  PRIMARY KEY (SMSTestPlan_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSTestPlanNumbers {
	my $sql = "DROP TABLE IF EXISTS SMSTestPlanNumbers";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSTestPlanNumbers (
		  SMSTestPlanNumbers_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SMSTestPlan_ID int unsigned NOT NULL,
		  SMSTestPlanNumbers_PhoneTo varchar(20) DEFAULT NULL,
		  SMSTestPlanNumbers_HourFrom time DEFAULT NULL,
		  SMSTestPlanNumbers_HourTo time DEFAULT NULL,
		  PRIMARY KEY (SMSTestPlanNumbers_ID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSText {
	my $sql = "DROP TABLE IF EXISTS SMSText";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSText (
		  SMSText_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SMSCampaign_ID int unsigned DEFAULT NULL,
		  SystemUser_ID int unsigned DEFAULT NULL,
		  Candidate_ID int unsigned DEFAULT NULL,
		  VoterFile_ID int unsigned DEFAULT NULL,
		  Raw_Voter_UniqNYSVoterID varchar(50) DEFAULT NULL,
		  SMSText_PhoneFrom varchar(20) DEFAULT NULL,
		  SMSText_PhoneTo varchar(20) DEFAULT NULL,
		  SMSText_PreviousID int unsigned DEFAULT NULL,
		  SMSText_Text text,
		  SMSText_direction enum('outbound','inbound') DEFAULT NULL,
		  SMSText_WholeJSON text,
		  SMSText_DateWriten datetime DEFAULT NULL,
		  PRIMARY KEY (SMSText_ID),
		  KEY SMSText_PhoneToIDX (SMSText_PhoneTo),
		  KEY SMSText_PhoneFromIDX (SMSText_PhoneFrom)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}

sub CreateTable_SMSVerifyPhone {
	my $sql = "DROP TABLE IF EXISTS SMSVerifyPhone";
	my $QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
	
	$sql =<<EOF;
		CREATE TABLE SMSVerifyPhone (
		  SMSVerifyPhone_ID int unsigned NOT NULL AUTO_INCREMENT,
		  SMSCampaign_ID int unsigned DEFAULT NULL,
		  VoterFile_ID int unsigned DEFAULT NULL,
		  UniqNYSVoterID varchar(50) DEFAULT NULL,
		  SMSVerifyPhone_Number varchar(40) DEFAULT NULL,
		  SMSVerifyPhone_WholeJSON text,
		  SMSVerifyPhone_Date datetime DEFAULT NULL,
		  PRIMARY KEY (SMSVerifyPhone_ID),
		  KEY SMSVerifyPhone_Phone_IDX (SMSVerifyPhone_Number),
		  KEY SMSVerifyPhone_VoterFile_IDX (SMSVerifyPhone_ID),
		  KEY SMSVerifyPhone_UniqNYSVoterID_IDX (UniqNYSVoterID)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
EOF
	$QueryDB = $RepMyBlock::dbhVoters->prepare($sql);
	$QueryDB->execute();		
}


1;

