#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;
use POSIX qw(strftime);
use Time::HiRes qw ( clock );

use FindBin::libs;
use RMBSchemas;

use RepMyBlock::NY;

print "Start the program\n";

### Connecting and Initializing.
my $RepMyBlock 		= RepMyBlock::NY->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");

$RepMyBlock->InitializeVoterFile();

print "State being considered: " . $RepMyBlock::DataStateID . "\n";


#$RepMyBlock::DBTableName = "NY";

print "Caching the data from the CD from date: " .  $RepMyBlock->{tabledate} . "\n";
my $TableDated = "NY_Raw_" . $RepMyBlock->{tabledate};

$RepMyBlock->InitLastInsertID();
$RepMyBlock->InitStreetCaches();
$RepMyBlock::dbhRawVoters =  $dbhRawVoters;


my $GrandDBTotal = $RepMyBlock->NumberOfVotersInDB($TableDated);
my $AmountToAdd = 1000000;
my $Start = 0;

$RepMyBlock->SetDatabase($dbhVoters);
print "Found $GrandDBTotal voters\n";

while (my $VoterCounter = $RepMyBlock->LoadFromRawData($TableDated, $AmountToAdd, $Start)) {
	
	my $clock0 = clock();
	print "Started with $VoterCounter\n";

	my $CounterCity = 0; 
	my $CounterStreet = 0;
	my %ToAddToDBCityNameToAddToDB = ();
	my %ToAddToDBStreetNameToAddToDB = ();
	
	print "VoterCounter: $VoterCounter\n";

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

	print "Counter Street: " . scalar @RepMyBlock::AddPoolStreetName . "\n";
	print "Counter City: " .  scalar @RepMyBlock::AddPoolCityName . "\n";
	$RepMyBlock->AddAddressCacheIntoDatabase();

	my @HouseAddress;
	for (my $i = 0; $i < $VoterCounter; $i++) {	
		$RepMyBlock->TransferAddressesToHash($i);
	}
	$RepMyBlock->AddAddressToDatabase();
		
	for (my $i = 0; $i < $VoterCounter; $i++) {	
		$RepMyBlock->TransferHousesToHash($i);
	}
	$RepMyBlock->AddToDataHouse();
	
	
	### Once processing of the names and the addresses is done, we can start with the voters.
	#### Table Voters
	# Voters_ID : 
	# ElectionsDistricts_DBTable, ElectionsDistricts_DBTableValue, Voters_Gender, VotersComplementInfo_ID, Voters_UniqStateVoterID, 
	# DataState_ID, Voters_RegParty, Voters_ReasonCode, Voters_Status, VotersMailingAddress_ID, Voters_IDRequired, Voters_IDMet, Voters_ApplyDate, 
	# Voters_RegSource, Voters_DateInactive, Voters_DatePurged, Voters_CountyVoterNumber, Voters_RecFirstSeen, Voters_RecLastSeen
	
	#$RepMyBlock::DBTableName
	#$RepMyBlock::CacheVoter_DBTableValue[$Counter]
	#$RepMyBlock::CacheVoter_Gender[$Counter] 
	## NADA
	#$RepMyBlock::CacheVoter_UniqStateVoterID[$Counter]
	#$RepMyBlock::DataStateID 
	#$RepMyBlock::CacheVoter_EnrollPolParty[$Counter] = $row[6] };
	#$RepMyBlock::CacheVoter_ReasonCode[$Counter]
	#$RepMyBlock::CacheVoter_Status[$Counter]
	#NADA
	#$RepMyBlock::CacheVoter_IDRequired[$Counter]
	#$RepMyBlock::CacheVoter_IDMet[$Counter]
	### Found the Variable for Apply Date
	#$RepMyBlock::CacheVoter_ApplicationSource[$Counter] 
	#$RepMyBlock::CacheVoter_VoterMadeInactive[$Counter]
	#$RepMyBlock::CacheVoter_VoterPurged[$Counter]
	#$RepMyBlock::CacheVoter_CountyVoterNumber[$Counter] =
	### FIST SEENS
	### LAST SEEN
	
	
	#### Table VotersIndexes
	# VotersIndexes_ID:
	# Voters_ID, DataState_ID, VotersLastName_ID, VotersFirstName_ID, VotersMiddleName_ID, VotersIndexes_Suffix, VotersIndexes_DOB, 
	# VotersIndexes_UniqStateVoterID
	
	# From Table Above
	#$RepMyBlock::DataStateID 
	#LASTNAME_ID
	#FIRSTNAME_ID
	#MIDDLENAME_ID
	#$RepMyBlock::CacheIdxSuffix[$Counter] = $row[3]; };
	#$RepMyBlock::CacheIdxDOB[$Counter] = $row[4]; };
	#$RepMyBlock::CacheVoter_UniqStateVoterID[$Counter]
	
	$Start += $AmountToAdd;
	my $clock1 = clock();
	print "Cycle took " . ($clock1 - $clock0) . " seconds - New Start: $Start\n";
	
	#exit();
}
