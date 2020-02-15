#!/usr/bin/perl

package RepMyBlock::NYS;
 
use strict;
use warnings;


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
	$stmt = $RepMyBlock::dbh->prepare($sql);
	$stmt->execute();
	while (my @row = $stmt->fetchrow_array) {
		
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

sub LoadTheIndexes {
	my $DateTable = $_[0];
	
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	### Last Name
	$sql = "SELECT Raw_Voter_FirstName, Raw_Voter_MiddleName, Raw_Voter_LastName, Raw_Voter_Suffix, Raw_Voter_DOB, Raw_Voter_UniqNYSVoterID FROM " . $DateTable; # . " LIMIT 60";
	$stmt = $RepMyBlock::dbh->prepare($sql);
	$stmt->execute();
	
	while (my @row = $stmt->fetchrow_array) {
		if ( defined ($row[0]) ) { $RepMyBlock::CacheIdxFirstName[$Counter] = $RepMyBlock::CacheFirstName { $row[0] }; };
		if ( defined ($row[1]) ) { $RepMyBlock::CacheIdxMiddleName[$Counter] = $RepMyBlock::CacheMiddleName { $row[1] }; };
		if ( defined ($row[2]) ) { $RepMyBlock::CacheIdxLastName[$Counter] =  $RepMyBlock::CacheLastName { $row[2] }; };
		if ( defined ($row[3]) ) { $RepMyBlock::CacheIdxSuffix[$Counter] = $row[3]; };
		if ( defined ($row[4]) ) { $RepMyBlock::CacheIdxDOB[$Counter] = $row[4]; };
		if ( defined ($row[5]) ) { $RepMyBlock::CacheIdxCode[$Counter] = $row[5]; };
		$Counter++;
		if (( $Counter % 500000) == 0)  { print "Done $Counter \n"; }
	}
	
	return $Counter;

}


sub InsideTheNormalize {
	my $FieldContent = $_[0];
	my $FieldVariable = $_[1];
	my $DataBaseName = $_[2];
	
	print "Inside RepMyBlock package -> " . $FieldVariable -> { $FieldContent } . "\n";
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

	print "Catastrophic ReturnRegistrationSource problem as it is empty\n";
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
	
	print "Catastrophic ReturnStatusCode problem as it is empty\n";
	exit();
	
	return undef;
}

sub ReturnGender {
	my ($Gender) = @_;
	if ( $Gender eq 'M') { return "male"; } 
	if ( $Gender eq 'F') { return "Female";	}
	if ($Gender eq 'U') { return 'undetermined'; }	
	return undef;
} 


sub ReturnYesNo {
	my ($Question) = @_;
	if ($Question eq 'Y') { return 'yes';	}	
	elsif ($Question eq 'N') { return 'no'; }
	return undef;
}
 
1;
