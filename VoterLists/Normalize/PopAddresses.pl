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

$RepMyBlock->InitStreetCaches();
$RepMyBlock::dbhRawVoters =  $dbhRawVoters;


my $GrandDBTotal = $RepMyBlock->NumberOfVotersInDB($TableDated);
my $AmountToAdd = 210000;
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
		$RepMyBlock::CacheHouse {  
			$RepMyBlock::CacheAddress
			{ $RepMyBlock::CacheAdress_ResStreetNameID[$i] } 
			{ $RepMyBlock::CacheAdress_ResCityNameID[$i] } { $RepMyBlock::CacheAdress_ResHouseNumber[$i] } 
			{ $RepMyBlock::CacheAdress_ResZip[$i] }	{ $RepMyBlock::CacheAdress_ResZip4[$i] } 
			{ $RepMyBlock::CacheAdress_ResPreStreet[$i] } { $RepMyBlock::CacheAdress_ResPostStDir[$i] }	
			{ $RepMyBlock::CacheAdress_ResFracAddress[$i] } 
		} {	$RepMyBlock::CacheAdress_ResApartment[$i] } = "0";
	}
	
	$RepMyBlock->AddToDataHouse();
	
	$Start += $AmountToAdd;
	my $clock1 = clock();
	print "Cycle took " . ($clock1 - $clock0) . " seconds - New Start: $Start\n";
	

}

#DataAddress_ID, DataAddress_HouseNumber, DataAddress_FracAddress, DataAddress_PreStreet, 
#DataStreet_ID, DataAddress_PostStreet, DataCity_ID, DataState_ID, DataAddress_zipcode, 
#DataAddress_zip4, Cordinate_ID, PG_OSM_osmid

#sub AddDataCity { my $self = shift; $self->AddToDatabase("DataCity", $CounterFirstName, \@AddPoolCityName, \%CounterCityName, 0); }
#sub AddDataStreet { my $self = shift; $self->AddToDatabase("DataStreet", $CounterLastName, \@AddPoolStreetName, \%CounterStreetName, 0); }

	
	#print "  Info CityStreet_ID: " . $RepMyBlock::CacheAdress_ResStreetNameID[$i] . "\n";
	#print "  Info CityName_ID: " . $RepMyBlock::CacheAdress_ResCityNameID[$i] . "\n";
	#print "  Info CacheAdress_ResStreetName: " . $RepMyBlock::CacheAdress_ResStreetName[$i] . "\n";
	#print "  Info CacheAdress_ResCity: " . $RepMyBlock::CacheAdress_ResCity[$i] . "\n";
	
	
		
	#print "  Info CacheAdress_ResHouseNumber: " . $RepMyBlock::CacheAdress_ResHouseNumber[$i] . "\n";
	#print "  Info CacheAdress_ResFracAddress: " . $RepMyBlock::CacheAdress_ResFracAddress[$i] . "\n";
	#print "  Info CacheAdress_ResApartment: " . $RepMyBlock::CacheAdress_ResApartment[$i] . "\n";
	#print "  Info CacheAdress_ResPreStreet: " . $RepMyBlock::CacheAdress_ResPreStreet[$i] . "\n";
	#print "  Info CacheAdress_ResStreetName: " . $RepMyBlock::CacheAdress_ResStreetName[$i] . "\n";
	#print "  Info CacheAdress_ResPostStDir: " . $RepMyBlock::CacheAdress_ResPostStDir[$i] . "\n";
	#print "  Info CacheAdress_ResCity: " . $RepMyBlock::CacheAdress_ResCity[$i] . "\n";
	#print "  Info CacheAdress_ResZip: " . $RepMyBlock::CacheAdress_ResZip[$i] . "\n";
	#print "  Info CacheAdress_ResZip4: " . $RepMyBlock::CacheAdress_ResZip4[$i] . "\n";
	#print "\n";
#}
