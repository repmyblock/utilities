#!/usr/bin/perl

### Need to document what this file is about.
### This Pop need to have to work the State and County loaded in the database.

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

print "Copying the VoterCD Stuff\n";
$RepMyBlock::DateTable 		= RepMyBlock::InitTheVoter();
my $DateTableID 					= RepMyBlock::DateDBID();

print "Caching the data from the CD from date: " . $RepMyBlock::DateTable . "\n";
RepMyBlock::InitStreetCaches();
my $TableDated = "Raw_Voter_" . $RepMyBlock::DateTable;

print "Loading the State Cache\n";
RepMyBlock::LoadResState();

print "Dealing with the names and adding them to the Cache and Database if new\n";
print "Loading the Voter Data\n";
my $VoterCounter = RepMyBlock::NYS::LoadAddressesFromRawData($TableDated);

print "Starting to write the data to the database\n";
RepMyBlock::BulkAddToShortDatabase("DataCity", scalar(@RepMyBlock::CacheVoter_City), \@RepMyBlock::CacheVoter_City, \%RepMyBlock::CacheCityName);
RepMyBlock::BulkAddToShortDatabase("DataStreet", scalar(@RepMyBlock::CacheVoter_City), \@RepMyBlock::CacheVoter_Street, \%RepMyBlock::CacheStreetName);

