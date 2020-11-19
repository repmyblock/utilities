#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Config::Simple;
use Lingua::EN::Numbers::Ordinate qw(ordinate th);

use FindBin::libs;
use RepMyBlock;

my $ElectionID = 1367;

print "Initializing the database\n";
my $dbh = RepMyBlock::InitDatabase();

my $DBAddElection = RepMyBlock::PrepareDBAddElection();
my %PartyCall;

print "Load Party Call\n";
RepMyBlock::LoadPartyCall($ElectionID, \%PartyCall);

foreach my $key (keys %PartyCall) {
	print "Party Call ID: " . $PartyCall{$key}->{'ElectionsPartyCall_ID'} . "\t";
	print "County: " . $PartyCall{$key}->{'DataCounty_Name'} ."\t State: " . $PartyCall{$key}->{'CandidatePositions_State'} . "\t";
	print $PartyCall{$key}->{'ElectionsPartyCall_DBTableValue'} . ": " . $PartyCall{$key}->{'ElectionsPartyCall_DBTable'} . "\t";
	print "Position: " . $PartyCall{$key}->{'CandidatePositions_Name'} . "\n";

	if ( $PartyCall{$key}->{'CandidatePositions_ID'}  == 1 ) {
	
		
		# I need to break this: 
	
		my ($nAD, $nED) = $PartyCall{$key}->{'ElectionsPartyCall_DBTableValue'} =~ m/(\d\d)(\d\d\d)/;	
		my $AD = th(int($nAD));
		my $ED = th(int($nED));
		my $URL = "/explain/CountyCommittee";
	
		if ( defined ($PartyCall{$key}->{'ElectionsPartyCall_NumberUnixSex'} )) {
			my $NumberOfPositions = $PartyCall{$key}->{'ElectionsPartyCall_NumberUnixSex'};
			my $HeadingTitle = 	"Member of the " . RepMyBlock::PartyAdjective( $PartyCall{$key}->{'ElectionsPartyCall_Party'} ) . 
													" Party County Committee from the " . 
													$ED . " election district in the " . $AD . " assembly district " .
													$PartyCall{$key}->{'DataCounty_Name'} . " County, New York State";
			
			$DBAddElection->execute($ElectionID, $PartyCall{$key}->{'CandidatePositions_Type'}, $PartyCall{$key}->{'ElectionsPartyCall_Party'}, 
															$PartyCall{$key}->{'DataCounty_Name'} . " " . $PartyCall{$key}->{'CandidatePositions_Name'},
															$HeadingTitle, $URL,
			 												$NumberOfPositions, $PartyCall{$key}->{'CandidatePositions_Order'}, 'no', undef, 
															$PartyCall{$key}->{'ElectionsPartyCall_DBTable'}, 
															$PartyCall{$key}->{'ElectionsPartyCall_DBTableValue'}, undef);
		}
		
		if ( defined ($PartyCall{$key}->{'ElectionsPartyCall_NumberFemale'} )) {
			my $NumberOfPositions = $PartyCall{$key}->{'ElectionsPartyCall_NumberFemale'};
			my $HeadingTitle = 	"Female member of the " . RepMyBlock::PartyAdjective( $PartyCall{$key}->{'ElectionsPartyCall_Party'} ) . 
													" Party County Committee from the " . 
													$ED . " election district in the " . $AD . " assembly district " .
											$PartyCall{$key}->{'DataCounty_Name'} . " County, New York State";

			$DBAddElection->execute($ElectionID, $PartyCall{$key}->{'CandidatePositions_Type'}, $PartyCall{$key}->{'ElectionsPartyCall_Party'}, 
															$PartyCall{$key}->{'DataCounty_Name'} . " " . $PartyCall{$key}->{'CandidatePositions_Name'},
															$HeadingTitle, $URL,
			 												$NumberOfPositions, $PartyCall{$key}->{'CandidatePositions_Order'}, 'no', 'female', 
															$PartyCall{$key}->{'ElectionsPartyCall_DBTable'}, 
															$PartyCall{$key}->{'ElectionsPartyCall_DBTableValue'}, undef);
		}
		
		
		if ( defined ($PartyCall{$key}->{'ElectionsPartyCall_NumberMale'} )) {
			my $NumberOfPositions = $PartyCall{$key}->{'ElectionsPartyCall_NumberMale'};
			my $HeadingTitle =	"Male member of the " . RepMyBlock::PartyAdjective( $PartyCall{$key}->{'ElectionsPartyCall_Party'} ) . 
													" Party County Committee from the " . 
													$ED . " election district in the " . $AD . " assembly district " .
													$PartyCall{$key}->{'DataCounty_Name'} . " County, New York State";

			$DBAddElection->execute($ElectionID, $PartyCall{$key}->{'CandidatePositions_Type'}, $PartyCall{$key}->{'ElectionsPartyCall_Party'}, 
															$PartyCall{$key}->{'DataCounty_Name'} . " " . $PartyCall{$key}->{'CandidatePositions_Name'},
															$HeadingTitle, $URL,
			 												$NumberOfPositions, $PartyCall{$key}->{'CandidatePositions_Order'}, 'no', 'male', 
															$PartyCall{$key}->{'ElectionsPartyCall_DBTable'}, 
															$PartyCall{$key}->{'ElectionsPartyCall_DBTableValue'}, undef);
		}
	}
  
}

print "Adding into election\n";


