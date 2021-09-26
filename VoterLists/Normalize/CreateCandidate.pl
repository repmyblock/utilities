#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;
use POSIX qw(strftime);
use Time::HiRes qw ( clock );
use Data::Dumper;
use Lingua::EN::NameCase 'NameCase' ;

use FindBin::libs;
use RepMyBlock::NY;
use Petition;

print "Start the program\n";
my $YearElection = 1369;


### Connecting and Initializing.
my $RepMyBlock 		= RepMyBlock::NY->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
$RepMyBlock::dbhRawVoters =  $dbhRawVoters;

my $ProdPetition = Petition->new();

my $dbhProd = $ProdPetition->InitProdDatabase();

print "Table Initialized: " . $RepMyBlock->InitializeVoterFile() . "\n";
print "State being considered: " . $RepMyBlock::DataStateID . "\n";
print "Caching the data from the CD from date: " .  $RepMyBlock->{tabledate} . "\n";
my $TableDated = "NY_Raw_" . $RepMyBlock->{tabledate};

my $ElectionID = $ARGV[1];

my ($state, $id) = $ARGV[0] =~ /(NY)(.*)/;
my $Voter = $state . sprintf("%018d", $id);
my $OriginalVoter = $state . $id;

my $DBVoter = $RepMyBlock->LoadOneVoter($Voter);

my $FullName = "";
if ( length ($DBVoter->{"FirstName"}) > 0) { $FullName .= NameCase($DBVoter->{"FirstName"}) . " "; }
if ( length ($DBVoter->{"MiddleName"}) > 0) { $FullName .= uc substr($DBVoter->{"MiddleName"}, 0, 1)  . ". " }
if ( length ($DBVoter->{"LastName"}) > 0) { $FullName .= NameCase($DBVoter->{"LastName"}) . " "; }
								
my $Address = "";
my $Space = "";
if ( length ($DBVoter->{"ResHouseNumber"}) > 0) { $Address .= $DBVoter->{"ResHouseNumber"}; $Space = " ";}
if ( length ($DBVoter->{"ResFracAddress"}) > 0) { $Address .= $Space . NameCase($DBVoter->{"ResFracAddress"}); $Space = " ";}
if ( length ($DBVoter->{"ResPreStreet"}) > 0) { $Address .= $Space . NameCase($DBVoter->{"ResPreStreet"}); $Space = " ";}
if ( length ($DBVoter->{"ResStreetName"}) > 0) { $Address .= $Space . NameCase($DBVoter->{"ResStreetName"}); $Space = " ";}
if ( length ($DBVoter->{"ResPostStDir"}) > 0) { $Address .= $Space . NameCase($DBVoter->{"ResPostStDir"}); $Space = " ";}
if ( length ($DBVoter->{"ResApartment"}) > 0) { $Address .= $Space . "- Apt " . uc($DBVoter->{"ResApartment"}); $Space = " ";}
if ( length ($DBVoter->{"ResCity"}) > 0) { $Address .= ", " . NameCase($DBVoter->{"ResCity"}) . ", "; }
if ( length ($state) > 0) { $Address .= uc $state . " "; }
if ( length ($DBVoter->{"ResZip"}) > 0) { $Address .= NameCase($DBVoter->{"ResZip"}) . " "; }
								
print "FullName: $FullName\n";
print "Address: $Address\n";

print "\nPrep the petitions\n";

my $CanSet = $ProdPetition->NextBatchID();
my $SID = int($CanSet->{"LastID"}) + 1;

my $ElectionID = $ProdPetition->ReturnCandidateElection( $YearElection, $ARGV[1], $ARGV[2]);
my $ReturnID = $ProdPetition->AddToCandidateTable($OriginalVoter, int($ElectionID->{"CandidateElection_ID"}), $DBVoter->{"EnrollPolParty"},  $FullName, $Address, "published");
$ProdPetition->AddToCandidateSetTable ($SID , $ReturnID,  int($DBVoter->{"CountyCode"}), $DBVoter->{"EnrollPolParty"});

if ($OriginalVoter ne "NY37464161") {
	$ProdPetition->AddToCandidateSetTable ($SID , "316",  int($DBVoter->{"CountyCode"}), $DBVoter->{"EnrollPolParty"});
}
$ProdPetition->AddToCandidateSetTable ($SID , "110",  int($DBVoter->{"CountyCode"}), $DBVoter->{"EnrollPolParty"});




print "\n";
print "https://pdf.repmyblock.nyc/NYS/s" . $SID  . "/multipetitions\n";
print "https://pdf.repmyblock.nyc/NYS/p" . $ReturnID  . "/multipetitions\n";
print "\n";