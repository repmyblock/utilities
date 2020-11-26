#!/usr/bin/perl

package RepMyBlock::NYC;
 
use strict;
use warnings;
use Lingua::EN::NameCase 'NameCase' ;

sub TransferRawTables {
	my $DateTable = $_[0];

	my %CacheRawLastName = ();	
	my %CacheRawFirstName = (); 
	my %CacheRawMiddleName = (); 
	
	### Counters 
	my @Counters;
	$Counters[0] = 0; # 0 -> Last Names
	$Counters[1] = 0; # 1 -> First Names
	$Counters[2] = 0; # 2 -> Middle Names
	
	my $sql = "";
	my $stmt = "";
	
	### Last Name
	$sql = "SELECT Raw_Voter_ID, Raw_Voter_LastName, Raw_Voter_FirstName, Raw_Voter_MiddleName FROM " . $DateTable; # . " LIMIT 60";
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while ( my @row = $stmt->fetchrow_array) { #} or die "can't execute the query: " . $stmt->errstr ) {
	
		# print "Name Found: " . $row[1] . " - " . $row[2] . " - " . $row[3] . "\n";
		
		### Last Name
		if ( defined ($row[1])) {		
			if ( ! ($RepMyBlock::CacheLastName { $row[1] })) {
				if ( ! $CacheRawLastName { $row[1] } ) {
					$RepMyBlock::AddPoolLastNames[($Counters[0]++)] = $row[1];
					$CacheRawLastName { $row[1] } = 1;
				}
			}
		}
		
		### First Name
		if ( defined ($row[2])) {
			if ( ! ( $RepMyBlock::CacheFirstName { $row[2] } )) {
				if ( ! $CacheRawFirstName { $row[2] } ) {
					$RepMyBlock::AddPoolFirstNames[($Counters[1]++)] = $row[2];
					$CacheRawFirstName { $row[2] } = 1;
				}
			}
		}
		
		### Middle Name
		if ( defined ($row[3])) {
			if ( ! ( $RepMyBlock::CacheMiddleName { $row[3] } )) {
				if ( ! $CacheRawMiddleName { $row[3] } ) {
					$RepMyBlock::AddPoolMiddleNames[($Counters[2]++)] = $row[3];
					$CacheRawMiddleName { $row[3] } = 1;
				}
			}
		}
		
	}
		
	RepMyBlock::AddToDatabase("LastName", $Counters[0], \@RepMyBlock::AddPoolLastNames, \%RepMyBlock::CacheLastName);
	RepMyBlock::AddToDatabase("FirstName", $Counters[1], \@RepMyBlock::AddPoolFirstNames, \%RepMyBlock::CacheFirstName);
	RepMyBlock::AddToDatabase("MiddleName", $Counters[2], \@RepMyBlock::AddPoolMiddleNames, \%RepMyBlock::CacheMiddleName);
}

sub LoadFromRawData {
	my $DateTable = $_[0];
	
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	### Last Name
	$sql = "SELECT Raw_Voter_FirstName, Raw_Voter_MiddleName, Raw_Voter_LastName, Raw_Voter_Suffix, Raw_Voter_DOB, " .
					"Raw_Voter_Gender, Raw_Voter_EnrollPolParty, Raw_Voter_ElectDistr, Raw_Voter_AssemblyDistr, Raw_Voter_CountyVoterNumber, " . 
					"Raw_Voter_RegistrationCharacter, Raw_Voter_ApplicationSource, Raw_Voter_IDRequired, Raw_Voter_IDMet, Raw_Voter_Status, " . 
					"Raw_Voter_ReasonCode, Raw_Voter_VoterMadeInactive, Raw_Voter_VoterPurged, Raw_Voter_UniqNYSVoterID " . 
					"FROM " . $DateTable; # . " LIMIT 500000";
	
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		# Index Part
		if ( defined ($row[0]) ) { $RepMyBlock::CacheIdxFirstName[$Counter] = $RepMyBlock::CacheFirstName { $row[0] }; };
		if ( defined ($row[1]) ) { $RepMyBlock::CacheIdxMiddleName[$Counter] = $RepMyBlock::CacheMiddleName { $row[1] }; };
		if ( defined ($row[2]) ) { $RepMyBlock::CacheIdxLastName[$Counter] =  $RepMyBlock::CacheLastName { $row[2] }; };
		if ( defined ($row[3]) ) { $RepMyBlock::CacheIdxSuffix[$Counter] = $row[3]; };
		if ( defined ($row[4]) ) { $RepMyBlock::CacheIdxDOB[$Counter] = $row[4]; };

		#Voter Part
		if ( defined ($row[5]) ) { $RepMyBlock::CacheVoter_Gender[$Counter] = $row[5] }; 
		if ( defined ($row[6]) ) { $RepMyBlock::CacheVoter_EnrollPolParty[$Counter] = $row[6] };
		if ( defined ($row[7]) && defined($row[8])) { $RepMyBlock::CacheVoter_DBTableValue[$Counter] =  $row[8] . sprintf("%03d", $row[7]) };
		if ( defined ($row[9]) ) { $RepMyBlock::CacheVoter_CountyVoterNumber[$Counter] = $row[9]; };
		if ( defined ($row[10]) ) { $RepMyBlock::CacheVoter_RegistrationCharacter[$Counter] = $row[10]; };
		if ( defined ($row[11]) ) { $RepMyBlock::CacheVoter_ApplicationSource[$Counter] = $row[11]; };
		if ( defined ($row[12]) ) { $RepMyBlock::CacheVoter_IDRequired[$Counter] = $row[12]; };
		if ( defined ($row[13]) ) { $RepMyBlock::CacheVoter_IDMet[$Counter] = $row[13]; };
		if ( defined ($row[14]) ) { $RepMyBlock::CacheVoter_Status[$Counter] = $row[14]; };
		if ( defined ($row[15]) ) { $RepMyBlock::CacheVoter_ReasonCode[$Counter] = $row[15]; };
		if ( defined ($row[16]) ) { $RepMyBlock::CacheVoter_VoterMadeInactive[$Counter] = $row[16]; };
		if ( defined ($row[17]) ) { $RepMyBlock::CacheVoter_VoterPurged[$Counter] = $row[17]; };
		if ( defined ($row[18]) ) { 
			$RepMyBlock::CacheVoter_UniqStateVoterID[$Counter] = $row[18]; 
			$RepMyBlock::CacheIdxCode[$Counter] = $row[18];
		};
		
		$Counter++;
		if (( $Counter % 500000) == 0)  { print "Done $Counter \n\033[1A"; }
	}
	print "Loaded into CacheIDX and CacheVoter: $Counter\n";
	
	return $Counter;
}

sub LoadVoterHistoryData {
	my $DateTable = $_[0];
	my $Counter = 0;
	
	my $sql = "";
	my $stmt = "";
	
	$sql = "SELECT DISTINCT Raw_Voter_VoterHistory FROM " . $DateTable . " WHERE Raw_Voter_VoterHistory IS NOT NULL";
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
	$sql = "SELECT Raw_Voter_FirstName, Raw_Voter_MiddleName, Raw_Voter_LastName, Raw_Voter_Suffix, Raw_Voter_DOB, " . 
					"Raw_Voter_UniqNYSVoterID FROM " . $DateTable; # . " LIMIT 60";
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
	
	$sql = "SELECT Raw_Voter_Gender, Raw_Voter_EnrollPolParty, Raw_Voter_ElectDistr, Raw_Voter_AssemblyDistr, Raw_Voter_CountyVoterNumber, " . 
					"Raw_Voter_RegistrationCharacter, Raw_Voter_ApplicationSource, Raw_Voter_IDRequired, Raw_Voter_IDMet, Raw_Voter_Status, " . 
					"Raw_Voter_ReasonCode, Raw_Voter_VoterMadeInactive, Raw_Voter_VoterPurged, Raw_Voter_UniqNYSVoterID " . 
					"FROM " . $DateTable; # . " LIMIT 60";
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		                          
		if ( defined ($row[0]) ) { $RepMyBlock::CacheVoter_Gender[$Counter] = $row[0] }; 
		if ( defined ($row[1]) ) { $RepMyBlock::CacheVoter_EnrollPolParty[$Counter] = $row[1] };
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

sub LoadAddressesFromRawData {
	my $DateTable = $_[0];
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	my %LocalCacheCity = ();
	my %LocalCacheStreet = ();
	
	print "Reading the Street and City\n";
	
	$sql = "SELECT Raw_Voter_ResStreetName, Raw_Voter_ResCity FROM " . $DateTable ;
	$stmt = $RepMyBlock::dbhRawVoters->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {		                          
		if ( defined ($row[1]) ) { $LocalCacheCity { $row[1] } = 0 }; 
		if ( defined ($row[0]) ) { $LocalCacheStreet { $row[0] } = 0 };
		$Counter++;
		if (( $Counter % 50000) == 0)  { print "Done $Counter \n\033[1A"; }
	}
	
	print "Loaded into cache: $Counter\nStart compressing\n";
	
	# Spagetting sauce for City
	my $j = 0; foreach my $key (keys %LocalCacheStreet ) {	$RepMyBlock::CacheVoter_Street[$j++] = NameCase($key); }
	print "Loaded into cache: $j streets\n";
	
	$j = 0; foreach my $key (keys %LocalCacheCity) { $RepMyBlock::CacheVoter_City[$j++] = NameCase($key);	}
	print "Loaded into cache: $j cities\n";
	
	return $Counter;
}


sub LoadVoterAddressFromRawData {
	my $DateTable = $_[0];
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	$sql = "SELECT DISTINCT Raw_Voter_ResHouseNumber, Raw_Voter_ResFracAddress, Raw_Voter_ResApartment, " . 
					"Raw_Voter_ResPreStreet, Raw_Voter_ResStreetName, Raw_Voter_ResPostStDir, Raw_Voter_ResCity, " . 
					"Raw_Voter_ResZip, Raw_Voter_ResZip4 FROM " . $DateTable . " LIMIT 100";
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
		if (( $Counter % 500000) == 0)  { print "Done $Counter \n\033[1A"; }
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
		if ( $Gender eq 'F') { return "Female";	}
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

sub trim {
	my $str = $_[0];
	$str =~ s/^\s+|\s+$//g;
	return $str;
}

 
1;
