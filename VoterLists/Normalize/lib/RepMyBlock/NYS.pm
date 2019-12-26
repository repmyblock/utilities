#!/usr/bin/perl

package RepMyBlock::NYS;
 
use strict;
use warnings;

sub TransferRawTables {
	my $DateTable = $_[0];
	
	my %CacheRawFirstName = (); 
	my %CacheRawMiddleName = (); 
	my %CacheRawLastName = ();
	
	### Counters 
	# 0 -> LastNames;

	my @Counters;
	$Counters[0] = 0;
	
	my $sql = "SELECT Raw_Voter_ID, Raw_Voter_LastName FROM " . $DateTable . " LIMIT 60";
	
	my $stmt = $RepMyBlock::dbh->prepare($sql);
	$stmt->execute();
	while (my @row = $stmt->fetchrow_array) {
		### Find the Name in the cache.
		if ( ! $RepMyBlock::CacheLastName { $row[1] } ) {
			if ( ! $CacheRawLastName { $row[1] } ) {
				$RepMyBlock::AddPoolLastNames[($Counters[0]++)] = $row[1];
				$CacheRawLastName { $row[1] } = 1;
			}
		}
	}
		
	RepMyBlock::AddToDatabase("LastName", $Counters[0], \@RepMyBlock::AddPoolLastNames, \%RepMyBlock::CacheLastName);
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
