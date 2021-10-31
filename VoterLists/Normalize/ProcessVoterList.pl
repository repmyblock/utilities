#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;
use POSIX qw(strftime);
use Time::HiRes qw ( clock );
use Data::Dumper;

use FindBin::libs;
use RMBSchemas;

use RepMyBlock::NY;
my $EmptyDatabase = 1;
my $StopCounterPass = 0;

print "Start the program\n";

### Connecting and Initializing.
my $RepMyBlock 		= RepMyBlock::NY->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");

print "State being considered: " . $RepMyBlock::DataStateID . "\n";

if ( $EmptyDatabase == 1) { 
	print "Emptying the database\n";
	$RepMyBlock->EmptyDatabases("DataAddress");
	$RepMyBlock->EmptyDatabases("DataCity");
	$RepMyBlock->EmptyDatabases("DataStreet");
	$RepMyBlock->EmptyDatabases("DataHouse");
	$RepMyBlock->EmptyDatabases("DataFirstName");
	$RepMyBlock->EmptyDatabases("DataLastName");
	$RepMyBlock->EmptyDatabases("DataMiddleName");
	## $RepMyBlock->EmptyDatabases("DataDistrict");  ## This one if off because it need to be loaded before starting.
}

$RepMyBlock->EmptyDatabases("Voters");
$RepMyBlock->EmptyDatabases("VotersIndexes");

print "Initializing files\n";
$RepMyBlock->InitializeVoterFile();

print "\nCaching the data from the CD from date: " .  $RepMyBlock->{tabledate} . "\n";
my $TableDated = "NY_Raw_" . $RepMyBlock->{tabledate};

$RepMyBlock->InitLastInsertID();

print "\nLoading the caches\n";
$RepMyBlock->InitStreetCaches();
$RepMyBlock->InitNamesCaches();

print "\nCharging the raw database\n";
$RepMyBlock::dbhRawVoters =  $dbhRawVoters;
#my $GrandDBTotal = $RepMyBlock->NumberOfVotersInDB($TableDated);
my $GrandDBTotal = 20318188; 
my $AmountToAdd = 50000;
my $AmountToAdd = 1000;
my $Start = 0;
my $PassCounter = 0;

$RepMyBlock->SetDatabase($dbhVoters);
print "Found $GrandDBTotal voters\n";

while (my $VoterCounter = $RepMyBlock->LoadFromRawData($TableDated, $AmountToAdd, $Start)) {
	
	my $clock0 = clock();
	print "\nStarted with $VoterCounter\n";

	my $CounterCity = 0; 
	my $CounterStreet = 0;
	
	my %ToAddToDBCityNameToAddToDB = ();
	my %ToAddToDBStreetNameToAddToDB = ();
	my %ToAddToDBFirstNameToAddToDB = ();
	my %ToAddToDBLastNameToAddToDB = ();
	my %ToAddToDBMiddleNameToAddToDB = ();

	print "VoterCounter: $VoterCounter\n";
	print "\nProcess Identity information\n";
	
	for (my $i = 0; $i < $VoterCounter; $i++) {	
		if ( ! defined $RepMyBlock::CacheIdxFirstName[$i] && length( $RepMyBlock::CacheVoter_FirstName[$i]) > 0 ) {
			$ToAddToDBFirstNameToAddToDB { $RepMyBlock::CacheVoter_FirstName[$i] } = 1;
		}
		
		if ( ! defined $RepMyBlock::CacheIdxMiddleName[$i] && length( $RepMyBlock::CacheVoter_MiddleName[$i]) > 0 ) {
			$ToAddToDBMiddleNameToAddToDB { $RepMyBlock::CacheVoter_MiddleName[$i] } = 1;
		}
		
		if ( ! defined $RepMyBlock::CacheIdxLastName[$i] && length( $RepMyBlock::CacheVoter_LastName[$i]) > 0 ) {
			$ToAddToDBLastNameToAddToDB { $RepMyBlock::CacheVoter_LastName[$i] } = 1;
		}
	}
	
	foreach my $key (keys %ToAddToDBFirstNameToAddToDB) { if ( length($key) > 0) { push @RepMyBlock::AddPoolFirstNames, $key;	}	}
	foreach my $key (keys %ToAddToDBLastNameToAddToDB) { if ( length($key) > 0) { push @RepMyBlock::AddPoolLastNames, $key;	}	}
	foreach my $key (keys %ToAddToDBMiddleNameToAddToDB) { if ( length($key) > 0) { push @RepMyBlock::AddPoolMiddleNames, $key;	}	}
	$RepMyBlock->AddNamesCacheIntoDatabase();

	print "\nProcess Location information\n";
	for (my $i = 0; $i < $VoterCounter; $i++) {	
		if ( ! defined $RepMyBlock::CacheAdress_ResStreetNameID[$i] && length( $RepMyBlock::CacheAdress_ResStreetName[$i]) > 0 ) {
			$ToAddToDBStreetNameToAddToDB { $RepMyBlock::CacheAdress_ResStreetName[$i] } = 1;
		}
		
		if ( ! defined $RepMyBlock::CacheAdress_ResCityNameID[$i] && length( $RepMyBlock::CacheAdress_ResCityName[$i]) > 0 ) {
			$ToAddToDBCityNameToAddToDB { $RepMyBlock::CacheAdress_ResCityName[$i] } = 1;
		}
	}
	
	foreach my $key (keys %ToAddToDBStreetNameToAddToDB) { if ( length($key) > 0) { push @RepMyBlock::AddPoolStreetName, $key;	}	}
	foreach my $key (keys %ToAddToDBCityNameToAddToDB) { if ( length($key) > 0) { push @RepMyBlock::AddPoolCityName, $key;	}	}
	$RepMyBlock->AddAddressCacheIntoDatabase();

	for (my $i = 0; $i < $VoterCounter; $i++) {	$RepMyBlock->TransferAddressesToHash($i); }	$RepMyBlock->DbAddToAddress();
	for (my $i = 0; $i < $VoterCounter; $i++) {	$RepMyBlock->TransferHousesToHash($i); } $RepMyBlock->DbAddToDataHouse();		
	for (my $i = 0; $i < $VoterCounter; $i++) {	$RepMyBlock->TransferVotersIndexToHash($i); } $RepMyBlock->DbAddToVotersIndex();
	for (my $i = 0; $i < $VoterCounter; $i++) {	$RepMyBlock->TransferVotersToHash($i); } $RepMyBlock->DbAddToVoters();
			
	$Start += $AmountToAdd;
	my $clock1 = clock();
	$PassCounter++;

	print "\nCycle " . $PassCounter . " took " . ($clock1 - $clock0) . " seconds - New Start: $Start\n";
	
	if ( $StopCounterPass > 0 && $StopCounterPass <= $PassCounter ) {
		print "\nFinal Last Insert ID:\n";
		$RepMyBlock->InitLastInsertID();
		
		print "\nFinal Count Tables ID:\n";
		$RepMyBlock->ListCountsTables();
		
		exit();
	}
	
	print "\n";
	
}

print "\nFinal Last Insert ID:\n";
$RepMyBlock->InitLastInsertID();

print "\nFinal Count Tables ID:\n";
$RepMyBlock->ListCountsTables();