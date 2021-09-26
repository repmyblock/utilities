#!/usr/bin/perl

### This is to process the OH raw data.
package RepMyBlock::NY;
 
use strict;
use RepMyBlock;
use Lingua::EN::NameCase 'NameCase';
use Time::HiRes qw ( clock );

use parent 'RepMyBlock';

### Before we start, we need to figure these three items.
#$RepMyBlock::DataStateID = "1";
#$RepMyBlock::DBTableName = "OHPRCNT";
#my $TableDated = "OH_Raw_" . $RepMyBlock::DateTable;


sub new { 
  my $class = shift; # defining shift in $myclass 
  my $self = {}; # the hashed reference 

	$RepMyBlock::DataStateID = "1";  
	$RepMyBlock::DBTableName = "ADED";
  return bless $self, $class; 
}


sub ReturnNamesQuery {
	my $self = shift;
	my $LimitCounter = shift;
	my $sql = "SELECT NY_Raw_ID, LastName, FirstName, MiddleName FROM NY_Raw_" . $self->{"tabledate"};	
	if ( defined $LimitCounter && $LimitCounter > 0 ) {	$sql .= " LIMIT $LimitCounter"; print "Limiting to $LimitCounter\n"; }
	return $sql;
}

sub NumberOfVotersInDB {
	my $self = shift;
	
	my $sql = "SELECT count(*) AS SizeTotal FROM " . $_[0] ;
	my $stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	my @row = $stmt->fetchrow_array;

}

sub LoadOneVoter {
	my $self = shift;
	
	my $sql = "SELECT * FROM NY_Raw_" . $self->{"tabledate"} . " WHERE UniqNYSVoterID = ? AND Status LIKE '%CTIVE'"; 
	my $stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute($_[0]);
	return $stmt->fetchrow_hashref();
}


sub LoadFromRawData {
	my $self = shift;
		
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	#	00:FirstName             01:MiddleName        02:LastName      03:Suffix         04:DOB
	# 05:Gender                06:EnrollPolParty    07:ElectDistr    08:AssemblyDistr  09:CountyVoterNumber
	#	10:RegistrationCharacter 11:ApplicationSource 12:IDRequired    13:IDMet          14:Status
	#	15:ReasonCode            16:VoterMadeInactive 17:VoterPurged   18:ResHouseNumber 19:ResFracAddress
	# 20:ResApartment          21:ResPreStreet      22:ResStreetName 23:ResPostStDir   24:ResCity
	#	25:ResZip                26:ResZip4           27:UniqNYSVoterID 
	
	### Last Name
	$sql = "SELECT FirstName, MiddleName, LastName, Suffix, DOB, " .
					"Gender, EnrollPolParty, ElectDistr, AssemblyDistr, CountyVoterNumber, " . 
					"RegistrationCharacter, ApplicationSource, IDRequired, IDMet, Status, " . 
					"ReasonCode, VoterMadeInactive, VoterPurged, " .
					"ResHouseNumber, ResFracAddress, ResApartment, " . 
					"ResPreStreet, ResStreetName, ResPostStDir, ResCity, " . 
					"ResZip, ResZip4, " .
					"UniqNYSVoterID " . 
					"FROM " . $_[0] ;
					
	if (defined $_[2] && $_[1] > 0 && $_[2] >= 0) { $sql .= " LIMIT " . $_[2] . ", " . $_[1]; print "Query DB Start at index: " . $_[2] . " Add by: " . $_[1] . "\n"; }
	elsif (defined $_[1] && $_[1] > 0) { $sql .= " LIMIT " . $_[1]; }
		
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();

	### THe Counter way 20.597232

	my $clock_inside0 = clock();
	while (my @row = $stmt->fetchrow_array) { #} or die "can't execute the query: $stmt->errstr \nSQL: $sql\n\n" ) {
		# Index Part
			
		if ( defined ($row[0]) ) { 
			$RepMyBlock::CacheVoter_FirstName[$Counter] = $self->trimstring($row[0]);
		 	$RepMyBlock::CacheIdxFirstName[$Counter] = $RepMyBlock::CacheFirstName { $RepMyBlock::CacheVoter_FirstName[$Counter] };
		}
		
		if ( defined ($row[1]) ) { 
			$RepMyBlock::CacheVoter_MiddleName[$Counter] = $self->trimstring($row[1]);
		 	$RepMyBlock::CacheIdxMiddleName[$Counter] = $RepMyBlock::CacheMiddleName { $RepMyBlock::CacheVoter_MiddleName[$Counter] };
		}
		
		if ( defined ($row[2]) ) { 
			$RepMyBlock::CacheVoter_LastName[$Counter] = $self->trimstring($row[2]);
		  $RepMyBlock::CacheIdxLastName[$Counter] =  $RepMyBlock::CacheLastName { $RepMyBlock::CacheVoter_LastName[$Counter] };
		}
		
		if ( defined ($row[3]) ) { $RepMyBlock::CacheIdxSuffix[$Counter] = $row[3]; };
		if ( defined ($row[4]) ) { $RepMyBlock::CacheIdxDOB[$Counter] = $self->ParseDatesToDB($row[4]); };

		#Voter Part
		if ( defined ($row[5]) ) { $RepMyBlock::CacheVoter_Gender[$Counter] = ReturnGender($row[5]) }; 
		if ( defined ($row[6]) ) { $RepMyBlock::CacheVoter_EnrollPolParty[$Counter] = $row[6] };
		if ( defined ($row[7]) && defined($row[8])) { $RepMyBlock::CacheVoter_DBTableValue[$Counter] =  $row[8] . sprintf("%03d", $row[7]) };
		if ( defined ($row[9]) ) { $RepMyBlock::CacheVoter_CountyVoterNumber[$Counter] = $row[9]; };
		if ( defined ($row[10]) ) { $RepMyBlock::CacheVoter_RegistrationCharacter[$Counter] = $row[10]; };
		if ( defined ($row[11]) ) { $RepMyBlock::CacheVoter_ApplicationSource[$Counter] = ReturnRegistrationSource($row[11]); };
		if ( defined ($row[12]) ) { $RepMyBlock::CacheVoter_IDRequired[$Counter] = ReturnYesNo($row[12]); };
		if ( defined ($row[13]) ) { $RepMyBlock::CacheVoter_IDMet[$Counter] = ReturnYesNo($row[13]); };
		if ( defined ($row[14]) ) { $RepMyBlock::CacheVoter_Status[$Counter] = ReturnStatusCode($row[14]); };
		if ( defined ($row[15]) ) { $RepMyBlock::CacheVoter_ReasonCode[$Counter] = ReturnReasonCode($row[15]); };
		if ( defined ($row[16]) ) { $RepMyBlock::CacheVoter_VoterMadeInactive[$Counter] = $self->ParseDatesToDB($row[16]); };
		if ( defined ($row[17]) ) { $RepMyBlock::CacheVoter_VoterPurged[$Counter] = $self->ParseDatesToDB($row[17]); };
		
		# Address Part
		if ( defined ($row[18]) ) { $RepMyBlock::CacheAdress_ResHouseNumber[$Counter]= $self->trimstring($row[18]); }; 
		if ( defined ($row[19]) ) { $RepMyBlock::CacheAdress_ResFracAddress[$Counter] = $self->trimstring($row[19]); }; 
		if ( defined ($row[20]) ) { $RepMyBlock::CacheAdress_ResApartment[$Counter]= $self->trimstring($row[20]); }; 
		if ( defined ($row[21]) ) { $RepMyBlock::CacheAdress_ResPreStreet[$Counter] = $self->trimstring($row[21]); }; 
		
		if ( defined ($row[22]) ) { 
			$RepMyBlock::CacheAdress_ResStreetName[$Counter] = $self->trimstring($row[22]);
			$RepMyBlock::CacheAdress_ResStreetNameID[$Counter] = $RepMyBlock::CacheStreetName { $RepMyBlock::CacheAdress_ResStreetName[$Counter] };
		}
			
		
		if ( defined ($row[23]) ) { $RepMyBlock::CacheAdress_ResPostStDir[$Counter] = $row[23]; }; 
		
		if ( defined ($row[24]) ) { 
			$RepMyBlock::CacheAdress_ResCityName[$Counter] = $self->trimstring($row[24]); 
			$RepMyBlock::CacheAdress_ResCityNameID[$Counter] = $RepMyBlock::CacheCityName { $RepMyBlock::CacheAdress_ResCityName[$Counter] };
		}
			
		if ( defined ($row[25]) ) { $RepMyBlock::CacheAdress_ResZip[$Counter] = $row[25]; }; 
		if ( defined ($row[26]) ) { $RepMyBlock::CacheAdress_ResZip4[$Counter]= $row[26]; }; 

		if ( defined ($row[27]) ) { 
			$RepMyBlock::CacheVoter_UniqStateVoterID[$Counter] = $row[27]; 
			$RepMyBlock::CacheIdxCode[$Counter] = $row[27];
		};
		
		
		$Counter++;
		if (( $Counter % 1000) == 0)  { print "Done $Counter \n\033[1A"; }
	}

	my $clock_inside1 = clock();
	print "Loaded into $Counter entries in CacheIDX and CacheVoter in " . ($clock_inside1 - $clock_inside0) . " seconds\n";
	
	return $Counter;
}

sub TransferAddressesToHash {
	my $self = shift;
	
	if ( ! defined ($RepMyBlock::CacheAddress
									{ $RepMyBlock::CacheStreetName { $RepMyBlock::CacheAdress_ResStreetName[$_[0]] }} 
									{ $RepMyBlock::CacheCityName { $RepMyBlock::CacheAdress_ResCityName[$_[0]] }} 
										{ $RepMyBlock::CacheAdress_ResHouseNumber[$_[0]] } { $RepMyBlock::CacheAdress_ResZip[$_[0]] }	
									{ $RepMyBlock::CacheAdress_ResZip4[$_[0]] } { $RepMyBlock::CacheAdress_ResPreStreet[$_[0]] } 
									{ $RepMyBlock::CacheAdress_ResPostStDir[$_[0]] } { $RepMyBlock::CacheAdress_ResFracAddress[$_[0]] } )) {					
		
		$RepMyBlock::CacheAddress
			{ $RepMyBlock::CacheStreetName { $RepMyBlock::CacheAdress_ResStreetName[$_[0]] }} 
			{ $RepMyBlock::CacheCityName { $RepMyBlock::CacheAdress_ResCityName[$_[0]] }} 
			{ $RepMyBlock::CacheAdress_ResHouseNumber[$_[0]] } 
			{ $RepMyBlock::CacheAdress_ResZip[$_[0]] }	{ $RepMyBlock::CacheAdress_ResZip4[$_[0]] } 
			{ $RepMyBlock::CacheAdress_ResPreStreet[$_[0]] } { $RepMyBlock::CacheAdress_ResPostStDir[$_[0]] }	
			{ $RepMyBlock::CacheAdress_ResFracAddress[$_[0]] }	= "0";
			
	}
		
		
}

sub TransferHousesToHash {
	my $self = shift;
	
	my $CacheAddress = $RepMyBlock::CacheAddress
											{ $RepMyBlock::CacheStreetName { $RepMyBlock::CacheAdress_ResStreetName[$_[0]] }} 
											{ $RepMyBlock::CacheCityName { $RepMyBlock::CacheAdress_ResCityName[$_[0]] }}
											{ $RepMyBlock::CacheAdress_ResHouseNumber[$_[0]] } 
											{ $RepMyBlock::CacheAdress_ResZip[$_[0]] }	{ $RepMyBlock::CacheAdress_ResZip4[$_[0]] } 
											{ $RepMyBlock::CacheAdress_ResPreStreet[$_[0]] } { $RepMyBlock::CacheAdress_ResPostStDir[$_[0]] }	
											{ $RepMyBlock::CacheAdress_ResFracAddress[$_[0]] };
																					
	if ( ! defined ($RepMyBlock::CacheHouse { $CacheAddress } {	$RepMyBlock::CacheAdress_ResApartment[$_[0]] }))  {	
		$RepMyBlock::CacheHouse { $CacheAddress } {	$RepMyBlock::CacheAdress_ResApartment[$_[0]] } = "0";
	}
	
}

sub TransferVotersIndexToHash {
	my $self = shift;
	
	if ( ! defined ($RepMyBlock::CacheVotersIndex
									{ $RepMyBlock::CacheFirstName { $RepMyBlock::CacheVoter_FirstName[$_[0]] }} 
									{ $RepMyBlock::CacheLastName { $RepMyBlock::CacheVoter_LastName[$_[0]] }} 
									{ $RepMyBlock::CacheMiddleName { $RepMyBlock::CacheVoter_MiddleName[$_[0]] }}
									{ $RepMyBlock::CacheIdxSuffix[$_[0]] }
									{ $RepMyBlock::CacheIdxDOB[$_[0]] }
									{ $RepMyBlock::CacheVoter_UniqStateVoterID[$_[0]] })) {					
		
		$RepMyBlock::CacheVotersIndex
									{ $RepMyBlock::CacheFirstName { $RepMyBlock::CacheVoter_FirstName[$_[0]] }} 
									{ $RepMyBlock::CacheLastName { $RepMyBlock::CacheVoter_LastName[$_[0]] }} 
									{ $RepMyBlock::CacheMiddleName { $RepMyBlock::CacheVoter_MiddleName[$_[0]] }}
									{ $RepMyBlock::CacheIdxSuffix[$_[0]] }
									{ $RepMyBlock::CacheIdxDOB[$_[0]] }
									{ $RepMyBlock::CacheVoter_UniqStateVoterID[$_[0]] }	= "0";
										
	}
}

sub TransferVotersToHash {
	my $self = shift;
	
	my $CacheVotersIndex = $RepMyBlock::CacheVotersIndex
													{ $RepMyBlock::CacheFirstName { $RepMyBlock::CacheVoter_FirstName[$_[0]] }} 
													{ $RepMyBlock::CacheLastName { $RepMyBlock::CacheVoter_LastName[$_[0]] }} 
													{ $RepMyBlock::CacheMiddleName { $RepMyBlock::CacheVoter_MiddleName[$_[0]] }}
													{ $RepMyBlock::CacheIdxSuffix[$_[0]] }
													{ $RepMyBlock::CacheIdxDOB[$_[0]] }
													{ $RepMyBlock::CacheVoter_UniqStateVoterID[$_[0]] };
																									
	my $CacheHouse =	$RepMyBlock::CacheHouse {
											$RepMyBlock::CacheAddress
												{ $RepMyBlock::CacheStreetName { $RepMyBlock::CacheAdress_ResStreetName[$_[0]] }} 
												{ $RepMyBlock::CacheCityName { $RepMyBlock::CacheAdress_ResCityName[$_[0]] }}
												{ $RepMyBlock::CacheAdress_ResHouseNumber[$_[0]] } 
												{ $RepMyBlock::CacheAdress_ResZip[$_[0]] }	{ $RepMyBlock::CacheAdress_ResZip4[$_[0]] } 
												{ $RepMyBlock::CacheAdress_ResPreStreet[$_[0]] } { $RepMyBlock::CacheAdress_ResPostStDir[$_[0]] }	
												{ $RepMyBlock::CacheAdress_ResFracAddress[$_[0]] } 
										} {	$RepMyBlock::CacheAdress_ResApartment[$_[0]] };
														
				
		if ( ! defined ($RepMyBlock::CacheVoters
											{ $RepMyBlock::CacheVoter_DBTableValue[$_[0]] }	{ $RepMyBlock::CacheVoter_Gender[$_[0]] }
											{ $CacheVotersIndex } { $CacheHouse } { $RepMyBlock::CacheVoter_EnrollPolParty[$_[0]] }
											{ $RepMyBlock::CacheVoter_ReasonCode[$_[0]] }	{ $RepMyBlock::CacheVoter_Status[$_[0]] }
											{ $RepMyBlock::CacheVoter_IDRequired[$_[0]] }	{ $RepMyBlock::CacheVoter_IDMet[$_[0]] }
											{ $RepMyBlock::CacheVoter_ApplicationSource[$_[0]] } 
											{ $RepMyBlock::CacheVoter_VoterMadeInactive[$_[0]] }
											{ $RepMyBlock::CacheVoter_VoterPurged[$_[0]] } 
											{ $RepMyBlock::CacheVoter_CountyVoterNumber[$_[0]] }
											{ $RepMyBlock::CacheVoter_UniqStateVoterID[$_[0]] }	)) {	
												
			if ( $RepMyBlock::CacheVoter_Status[$_[0]] ne "Active" && $RepMyBlock::CacheVoter_Status[$_[0]] ne "Inactive" && $RepMyBlock::CacheVoter_Status[$_[0]] ne "Prereg17YearOlds") {
				undef $RepMyBlock::CacheVoter_DBTableValue[$_[0]];
			}
						
			$RepMyBlock::CacheVoters
											{ $RepMyBlock::CacheVoter_DBTableValue[$_[0]] }	{ $RepMyBlock::CacheVoter_Gender[$_[0]] }
											{ $CacheVotersIndex } { $CacheHouse } { $RepMyBlock::CacheVoter_EnrollPolParty[$_[0]] }
											{ $RepMyBlock::CacheVoter_ReasonCode[$_[0]] }	{ $RepMyBlock::CacheVoter_Status[$_[0]] }
											{ $RepMyBlock::CacheVoter_IDRequired[$_[0]] }	{ $RepMyBlock::CacheVoter_IDMet[$_[0]] }
											{ $RepMyBlock::CacheVoter_ApplicationSource[$_[0]] } 
											{ $RepMyBlock::CacheVoter_VoterMadeInactive[$_[0]] }
											{ $RepMyBlock::CacheVoter_VoterPurged[$_[0]] } 
											{ $RepMyBlock::CacheVoter_CountyVoterNumber[$_[0]] }
											{ $RepMyBlock::CacheVoter_UniqStateVoterID[$_[0]] }	= "0";
		}	
}

### From down here I need to replace to make it work.

sub LoadVoterHistoryData {
	my $DateTable = $_[0];
	my $Counter = 0;
	
	my $sql = "";
	my $stmt = "";
	
	$sql = "SELECT DISTINCT VoterHistory FROM " . $DateTable . " WHERE VoterHistory IS NOT NULL";
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {	
		my @splitoncolong = split /;/, $row[0];
		foreach my $wap (@splitoncolong) {
			$RepMyBlock::CacheVoterHistory{ $wap } = "0";
		}
		if (( ++$Counter % 500000) == 0)  { print "Done $Counter \n\033[1A"; }
	}
}

sub LoadTheIndexes {
	my $DateTable = $_[0];
	
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	### Last Name
	$sql = "SELECT FirstName, MiddleName, LastName, Suffix, DOB, " . 
					"UniqNYSVoterID FROM " . $DateTable; # . " LIMIT 60";
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		if ( defined ($row[0]) ) { $RepMyBlock::CacheIdxFirstName[$Counter] = $RepMyBlock::CacheFirstName { $row[0] }; };
		if ( defined ($row[1]) ) { $RepMyBlock::CacheIdxMiddleName[$Counter] = $RepMyBlock::CacheMiddleName { $row[1] }; };
		if ( defined ($row[2]) ) { $RepMyBlock::CacheIdxLastName[$Counter] =  $RepMyBlock::CacheLastName { $row[2] }; };
		if ( defined ($row[3]) ) { $RepMyBlock::CacheIdxSuffix[$Counter] = $row[3]; };
		if ( defined ($row[4]) ) { $RepMyBlock::CacheIdxDOB[$Counter] = $row[4]; };
		if ( defined ($row[5]) ) { $RepMyBlock::CacheIdxCode[$Counter] = $row[5]; };
		
		$Counter++;
		if (( $Counter % 500000) == 0)  { print "Done $Counter \n\033[1A"; }
	}
	print "Loaded into cache: $Counter\n";
	
	return $Counter;
}

sub InsideTheNormalize {
	my $FieldContent = $_[0];
	my $FieldVariable = $_[1];
	my $DataBaseName = $_[2];
	print "Inside RepMyBlock package -> " . $FieldVariable -> { $FieldContent } . "\n";
}

sub LoadVoterData {
	my $DateTable = $_[0];
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	$sql = "SELECT Gender, EnrollPolParty, ElectDistr, AssemblyDistr, CountyVoterNumber, " . 
					"RegistrationCharacter, ApplicationSource, IDRequired, IDMet, Status, " . 
					"ReasonCode, VoterMadeInactive, VoterPurged, UniqNYSVoterID " . 
					"FROM " . $DateTable; # . " LIMIT 60";
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		                          
		if ( defined ($row[0]) ) { $RepMyBlock::CacheVoter_Gender[$Counter] = $row[0] }; 
		if ( defined ($row[1]) ) { $RepMyBlock::CacheVoter_EnrollPolParty[$Counter] = PartyAdjective ($row[1]) };
		if ( defined ($row[2]) && defined($row[3])) { $RepMyBlock::CacheVoter_DBTableValue[$Counter] =  $row[2] . $row[3] };
		if ( defined ($row[4]) ) { $RepMyBlock::CacheVoter_CountyVoterNumber[$Counter] = $row[4]; };
		if ( defined ($row[5]) ) { $RepMyBlock::CacheVoter_RegistrationCharacter[$Counter] = $row[5]; };
		if ( defined ($row[6]) ) { $RepMyBlock::CacheVoter_ApplicationSource[$Counter] = $row[6]; };
		if ( defined ($row[7]) ) { $RepMyBlock::CacheVoter_IDRequired[$Counter] = $row[7]; };
		if ( defined ($row[8]) ) { $RepMyBlock::CacheVoter_IDMet[$Counter] = $row[8]; };
		if ( defined ($row[9]) ) { $RepMyBlock::CacheVoter_Status[$Counter] = $row[9]; };
		if ( defined ($row[10]) ) { $RepMyBlock::CacheVoter_ReasonCode[$Counter] = $row[10]; };
		if ( defined ($row[11]) ) { $RepMyBlock::CacheVoter_VoterMadeInactive[$Counter] = $row[11]; };
		if ( defined ($row[12]) ) { $RepMyBlock::CacheVoter_VoterPurged[$Counter] = $row[12]; };
		if ( defined ($row[13]) ) { $RepMyBlock::CacheVoter_UniqStateVoterID[$Counter] = $row[13]; };

		$Counter++;
		if (( $Counter % 500000) == 0)  { print "Done $Counter \n\033[1A"; }
	}
	print "Loaded into cache: $Counter\n";	
	return $Counter;
}



sub LoadVoterAddressFromRawData {
	my $DateTable = $_[0];
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	$sql = "SELECT DISTINCT ResHouseNumber, ResFracAddress, ResApartment, " . 
					"ResPreStreet, ResStreetName, ResPostStDir, ResCity, " . 
					"ResZip, ResZip4 FROM " . $DateTable . " LIMIT 100";
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {		
		if ( defined ($row[0]) ) { $RepMyBlock::CacheAdress_ResHouseNumber[$Counter]= $row[0] }; 
		if ( defined ($row[1]) ) { $RepMyBlock::CacheAdress_ResFracAddress[$Counter] = $row[1] }; 
		if ( defined ($row[2]) ) { $RepMyBlock::CacheAdress_ResApartment[$Counter]= $row[2] }; 
		if ( defined ($row[3]) ) { $RepMyBlock::CacheAdress_ResPreStreet[$Counter] = $row[3] }; 
		if ( defined ($row[4]) ) { $RepMyBlock::CacheAdress_ResStreetName[$Counter] = $RepMyBlock::CacheStreetName {NameCase($row[4]) } }; 
		if ( defined ($row[5]) ) { $RepMyBlock::CacheAdress_ResPostStDir[$Counter] = $row[5] }; 
		if ( defined ($row[6]) ) { $RepMyBlock::CacheAdress_ResCity[$Counter] = $RepMyBlock::CacheCityName { NameCase($row[6]) } }; 
		if ( defined ($row[7]) ) { $RepMyBlock::CacheAdress_ResZip[$Counter] = $row[7] }; 
		if ( defined ($row[8]) ) { $RepMyBlock::CacheAdress_ResZip4[$Counter]= $row[8] }; 
		
		$Counter++;
		if (( $Counter % 1000) == 0)  { print "Done $Counter \n\033[1A"; }
	}
	print "Loaded into cache: $Counter\n";
		
	return $Counter;
}

#### These are the standart questions in the NYS voter file.
sub	ReturnReasonCode {
	my ($Question) = @_;
		
	if ( ! defined $Question ) { return undef; }
	
	if ($Question eq "ADJ-INCOMP") { return "AdjudgedIncompetent" }
	elsif ($Question eq "DEATH") {  return "Death" }
	elsif ($Question eq "DUPLICATE") {  return "Duplicate" }
	elsif ($Question eq "FELON") {  return "Felon" }
	elsif ($Question eq "MAIL-CHECK") { return "MailCheck" }
	elsif ($Question eq "MAILCHECK") { return "MailCheck" }
	elsif ($Question eq "MOVED") { return "MouvedOutCounty" }
	elsif ($Question eq "NCOA") {  return "NCOA" }
	elsif ($Question eq "NVRA") {  return "NVRA" }
	elsif ($Question eq "RETURN-MAIL") {  return "ReturnMail" }
	elsif ($Question eq "VOTER-REQ") {  return "VoterRequest" }
	elsif ($Question eq "OTHER") {  return "Other" }
	elsif ($Question eq "COURT") {  return "Court" }
	elsif ($Question eq "INACTIVE") {  return "Inactive" }
	
	print "Catastrophic ReturnReasonCode problem as $Question\n";
	exit();
	
	return undef;
}

sub ReturnRegistrationSource {
	my ($Question) = @_;
	
	if ( ! defined $Question ) { return undef; }
	if ($Question eq "AGCY") { return "Agency"; }
	elsif ($Question eq "CBOE") { return "CBOE"; }
	elsif ($Question eq "DMV") { return "DMV"; }
	elsif ($Question eq "LOCALREG") { return "LocalRegistrar"; }
	elsif ($Question eq "MAIL") { return "MailIn"; }
	elsif ($Question eq "SCHOOL") { return "School"; }

	print "Catastrophic ReturnRegistrationSource problem as it is empty: $Question\n";
	exit();
	
	return undef;
}
	
sub	ReturnStatusCode {
	my ($Question) = @_;				

	if ( ! defined $Question ) { return undef; }
	
	if ($Question eq "ACTIVE") { return "Active"; }
	elsif ($Question eq "AM") { return "ActiveMilitary"; }
	elsif ($Question eq "AF") { return "ActiveSpecialFederal"; }
	elsif ($Question eq "AP") { return "ActiveSpecialPresidential"; }
	elsif ($Question eq "AU") { return "ActiveUOCAVA"; }
	elsif ($Question eq "INACTIVE") { return "Inactive"; }
	elsif ($Question eq "PURGED") { return "Purged"; }
	elsif ($Question eq "PREREG") { return "Prereg17YearOlds"; }
	elsif ($Question eq "RETURN-MAIL") { return "ReturnMail"; }
	elsif ($Question eq "VOTER-REQ") { return "VoterRequest"; }
	
	print "Catastrophic ReturnStatusCode problem as it is empty: $Question\n";
	exit();
	
	return undef;
}

sub ReturnGender {
	my ($Gender) = @_;
	
	if ( defined $Gender ) { 
		if ( $Gender eq 'M') { return "male"; } 
		if ( $Gender eq 'F') { return "female";	}
		if ($Gender eq 'U') { return 'undetermined'; }	
	}
	
	return undef;
} 


sub ReturnYesNo {
	my ($Question) = @_;
	if ($Question eq 'Y') { return 'yes';	}	
	elsif ($Question eq 'N') { return 'no'; }
	return undef;
}


1;
