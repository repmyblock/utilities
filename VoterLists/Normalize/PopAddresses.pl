#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;

use FindBin::libs;
use RepMyBlock;
use RepMyBlock::NYS;
use RMBSchemas;

print "Start the program\n";
print "Connecting to the databases\n";
$RepMyBlock::dbhRawVoters = RepMyBlock::InitDatabase("dbname_voters");
$RepMyBlock::dbhVoters		= RepMyBlock::InitDatabase("dbname_rmb");

print "Blank the whole database\n";
RMBSchemas::CreateTable_DataCity();
RMBSchemas::CreateTable_DataStreet();
RMBSchemas::CreateTable_DataState();
RMBSchemas::CreateTable_DataCounty();
#RMBSchemas::CreateTable_DataHouse();
#RMBSchemas::CreateTable_DataAddress();

#RMBSchemas::CreateTable_Cordinate();
#RMBSchemas::CreateTable_CordinateBox();
#RMBSchemas::CreateTable_CordinateGroup();

print "Purging the data\n";
#RepMyBlock::EmptyDatabases("Voters", 1);

print "Copying the VoterCD Stuff\n";
$RepMyBlock::DateTable 		= RepMyBlock::InitTheVoter();
my $DateTableID 					= RepMyBlock::DateDBID();

print "Caching the data from the CD from date: " . $RepMyBlock::DateTable . "\n";
RepMyBlock::InitStreetCaches();
my $TableDated = "Raw_Voter_" . $RepMyBlock::DateTable;

print "Dealing with the names and adding them to the Cache and Database if new\n";
print "Loading the Voter Data\n";
my $VoterCounter = RepMyBlock::NYS::LoadAddressesFromRawData($TableDated);

print "Starting to write the data to the database\n";
RepMyBlock::BulkAddToShortDatabase("DataCity", scalar(@RepMyBlock::CacheVoter_City), \@RepMyBlock::CacheVoter_City, \%RepMyBlock::CacheCityName);
RepMyBlock::BulkAddToShortDatabase("DataStreet", scalar(@RepMyBlock::CacheVoter_City), \@RepMyBlock::CacheVoter_Street, \%RepMyBlock::CacheStreetName);

#### THis is the static Counties and State for the time being.
my @State = ();
$State[0] = 'New York';
$State[1] = 'Ohio';
RepMyBlock::BulkAddToShortDatabase("DataState", scalar(@State), \@State, \%RepMyBlock::CacheStateName);

my %County = ();
$County{'New York'}{'Albany'} = 1;
$County{'New York'}{'Allegany'} = 2;
$County{'New York'}{'Bronx'} = 3;
$County{'New York'}{'Broome'} = 4;
$County{'New York'}{'Cattaraugus'} = 5;
$County{'New York'}{'Cayuga'} = 6;
$County{'New York'}{'Chautauqua'} = 7;
$County{'New York'}{'Chemung'} = 8;
$County{'New York'}{'Chenango'} = 9;
$County{'New York'}{'Clinton'} = 10;
$County{'New York'}{'Columbia'} = 11;
$County{'New York'}{'Cortland'} = 12;
$County{'New York'}{'Delaware'} = 13;
$County{'New York'}{'Dutchess'} = 14;
$County{'New York'}{'Erie'} = 15;
$County{'New York'}{'Essex'} = 16;
$County{'New York'}{'Franklin'} = 17;
$County{'New York'}{'Fulton'} = 18;
$County{'New York'}{'Genesee'} = 19;
$County{'New York'}{'Greene'} = 20;
$County{'New York'}{'Hamilton'} = 21;
$County{'New York'}{'Herkimer'} = 22;
$County{'New York'}{'Jefferson'} = 23;
$County{'New York'}{'Kings'} = 24;
$County{'New York'}{'Lewis'} = 25;
$County{'New York'}{'Livingston'} = 26;
$County{'New York'}{'Madison'} = 27;
$County{'New York'}{'Monroe'} = 28;
$County{'New York'}{'Montgomery'} = 29;
$County{'New York'}{'Nassau'} = 30;
$County{'New York'}{'New York'} = 31;
$County{'New York'}{'Niagara'} = 32;
$County{'New York'}{'Oneida'} = 33;
$County{'New York'}{'Onondaga'} = 34;
$County{'New York'}{'Ontario'} = 35;
$County{'New York'}{'Orange'} = 36;
$County{'New York'}{'Orleans'} = 37;
$County{'New York'}{'Oswego'} = 38;
$County{'New York'}{'Otsego'} = 39;
$County{'New York'}{'Putnam'} = 40;
$County{'New York'}{'Queens'} = 41;
$County{'New York'}{'Rensselaer'} = 42;
$County{'New York'}{'Richmond'} = 43;
$County{'New York'}{'Rockland'} = 44;
$County{'New York'}{'Saratoga'} = 45;
$County{'New York'}{'Schenectady'} = 46;
$County{'New York'}{'Schoharie'} = 47;
$County{'New York'}{'Schuyler'} = 48;
$County{'New York'}{'Seneca'} = 49;
$County{'New York'}{'St. Lawrence'} = 50;
$County{'New York'}{'Steuben'} = 51;
$County{'New York'}{'Suffolk'} = 52;
$County{'New York'}{'Sullivan'} = 53;
$County{'New York'}{'Tioga'} = 54;
$County{'New York'}{'Tompkins'} = 55;
$County{'New York'}{'Ulster'} = 56;
$County{'New York'}{'Warren'} = 57;
$County{'New York'}{'Washington'} = 58;
$County{'New York'}{'Wayne'} = 59;
$County{'New York'}{'Westchester'} = 60;
$County{'New York'}{'Wyoming'} = 61;
$County{'New York'}{'Yates'} = 62;
$County{'Ohio'}{'ADAMS'} = 1;
$County{'Ohio'}{'ALLEN'} = 2;
$County{'Ohio'}{'ASHLAND'} = 3;
$County{'Ohio'}{'ASHTABULA'} = 4;
$County{'Ohio'}{'ATHENS'} = 5;
$County{'Ohio'}{'AUGLAIZE'} = 6;
$County{'Ohio'}{'BELMONT'} = 7;
$County{'Ohio'}{'BROWN'} = 8;
$County{'Ohio'}{'BUTLER'} = 9;
$County{'Ohio'}{'CARROLL'} = 10;
$County{'Ohio'}{'CHAMPAIGN'} = 11;
$County{'Ohio'}{'CLARK'} = 12;
$County{'Ohio'}{'CLERMONT'} = 13;
$County{'Ohio'}{'CLINTON'} = 14;
$County{'Ohio'}{'COLUMBIANA'} = 15;
$County{'Ohio'}{'COSHOCTON'} = 16;
$County{'Ohio'}{'CRAWFORD'} = 17;
$County{'Ohio'}{'CUYAHOGA'} = 18;
$County{'Ohio'}{'DARKE'} = 19;
$County{'Ohio'}{'DEFIANCE'} = 20;
$County{'Ohio'}{'DELAWARE'} = 21;
$County{'Ohio'}{'ERIE'} = 22;
$County{'Ohio'}{'FAIRFIELD'} = 23;
$County{'Ohio'}{'FAYETTE'} = 24;
$County{'Ohio'}{'FRANKLIN'} = 25;
$County{'Ohio'}{'FULTON'} = 26;
$County{'Ohio'}{'GALLIA'} = 27;
$County{'Ohio'}{'GEAUGA'} = 28;
$County{'Ohio'}{'GREENE'} = 29;
$County{'Ohio'}{'GUERNSEY'} = 30;
$County{'Ohio'}{'HAMILTON'} = 31;
$County{'Ohio'}{'HANCOCK'} = 32;
$County{'Ohio'}{'HARDIN'} = 33;
$County{'Ohio'}{'HARRISON'} = 34;
$County{'Ohio'}{'HENRY'} = 35;
$County{'Ohio'}{'HIGHLAND'} = 36;
$County{'Ohio'}{'HOCKING'} = 37;
$County{'Ohio'}{'HOLMES'} = 38;
$County{'Ohio'}{'HURON'} = 39;
$County{'Ohio'}{'JACKSON'} = 40;
$County{'Ohio'}{'JEFFERSON'} = 41;
$County{'Ohio'}{'KNOX'} = 42;
$County{'Ohio'}{'LAKE'} = 43;
$County{'Ohio'}{'LAWRENCE'} = 44;
$County{'Ohio'}{'LICKING'} = 45;
$County{'Ohio'}{'LOGAN'} = 46;
$County{'Ohio'}{'LORAIN'} = 47;
$County{'Ohio'}{'LUCAS'} = 48;
$County{'Ohio'}{'MADISON'} = 49;
$County{'Ohio'}{'MAHONING'} = 50;
$County{'Ohio'}{'MARION'} = 51;
$County{'Ohio'}{'MEDINA'} = 52;
$County{'Ohio'}{'MEIGS'} = 53;
$County{'Ohio'}{'MERCER'} = 54;
$County{'Ohio'}{'MIAMI'} = 55;
$County{'Ohio'}{'MONROE'} = 56;
$County{'Ohio'}{'MONTGOMERY'} = 57;
$County{'Ohio'}{'MORGAN'} = 58;
$County{'Ohio'}{'MORROW'} = 59;
$County{'Ohio'}{'MUSKINGUM'} = 60;
$County{'Ohio'}{'NOBLE'} = 61;
$County{'Ohio'}{'OTTAWA'} = 62;
$County{'Ohio'}{'PAULDING'} = 63;
$County{'Ohio'}{'PERRY'} = 64;
$County{'Ohio'}{'PICKAWAY'} = 65;
$County{'Ohio'}{'PIKE'} = 66;
$County{'Ohio'}{'PORTAGE'} = 67;
$County{'Ohio'}{'PREBLE'} = 68;
$County{'Ohio'}{'PUTNAM'} = 69;
$County{'Ohio'}{'RICHLAND'} = 70;
$County{'Ohio'}{'ROSS'} = 71;
$County{'Ohio'}{'SANDUSKY'} = 72;
$County{'Ohio'}{'SCIOTO'} = 73;
$County{'Ohio'}{'SENECA'} = 74;
$County{'Ohio'}{'SHELBY'} = 75;
$County{'Ohio'}{'STARK'} = 76;
$County{'Ohio'}{'SUMMIT'} = 77;
$County{'Ohio'}{'TRUMBULL'} = 78;
$County{'Ohio'}{'TUSCARAWAS'} = 79;
$County{'Ohio'}{'UNION'} = 80;
$County{'Ohio'}{'VANWERT'} = 81;
$County{'Ohio'}{'VINTON'} = 82;
$County{'Ohio'}{'WARREN'} = 83;
$County{'Ohio'}{'WASHINGTON'} = 84;
$County{'Ohio'}{'WAYNE'} = 85;
$County{'Ohio'}{'WILLIAMS'} = 86;
$County{'Ohio'}{'WOOD'} = 87;
$County{'Ohio'}{'WYANDOT'} = 88;

RepMyBlock::BulkAddToCountyTable(%County);