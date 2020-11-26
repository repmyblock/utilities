#!/usr/bin/perl

### Need to document what this file is about.
### This Pop need to have to work the State and County loaded in the database.

use strict;
use DBI;
use Text::CSV;

use FindBin::libs;
use RepMyBlock;
use RepMyBlock::NY;
use RMBSchemas;

print "Start the program\n";
print "Connecting to the databases\n";
$RepMyBlock::dbhRawVoters = RepMyBlock::InitDatabase("dbname_voters");
$RepMyBlock::dbhVoters		= RepMyBlock::InitDatabase("dbname_rmb");

print "Blank the whole database\n";
RMBSchemas::CreateTable_DataAddress();
#RMBSchemas::CreateTable_DataHouse();

#RMBSchemas::CreateTable_Cordinate();
#RMBSchemas::CreateTable_CordinateBox();
#RMBSchemas::CreateTable_CordinateGroup();

print "Copying the VoterCD Stuff\n";
$RepMyBlock::DateTable 		= RepMyBlock::InitTheVoter();
my $DateTableID 					= RepMyBlock::DateDBID();

print "Caching the data from the CD from date: " . $RepMyBlock::DateTable . "\n";
RepMyBlock::LoadResState();
RepMyBlock::InitStreetCaches();
my $TableDated = "Raw_Voter_" . $RepMyBlock::DateTable;


print "Dealing with the names and adding them to the Cache and Database if new\n";
print "Loading the Voter Data\n";
my $VoterCounter = RepMyBlock::NY::LoadVoterAddressFromRawData($TableDated);

print "Starting to write the data to the database\n";
for (my $i = 0; $i < $VoterCounter; $i++) {

	print "I: $i\n";
	print "Info CacheAdress_ResHouseNumber: " . $RepMyBlock::CacheAdress_ResHouseNumber[$i] . "\n";
	print "Info CacheAdress_ResFracAddress: " . $RepMyBlock::CacheAdress_ResFracAddress[$i] . "\n";
	print "Info CacheAdress_ResApartment: " . $RepMyBlock::CacheAdress_ResApartment[$i] . "\n";
	print "Info CacheAdress_ResPreStreet: " . $RepMyBlock::CacheAdress_ResPreStreet[$i] . "\n";
	print "Info CacheAdress_ResStreetName: " . $RepMyBlock::CacheAdress_ResStreetName[$i] . "\n";
	print "Info CacheAdress_ResPostStDir: " . $RepMyBlock::CacheAdress_ResPostStDir[$i] . "\n";
	print "Info CacheAdress_ResCity: " . $RepMyBlock::CacheAdress_ResCity[$i] . "\n";
	print "Info CacheAdress_ResZip: " . $RepMyBlock::CacheAdress_ResZip[$i] . "\n";
	print "Info CacheAdress_ResZip4: " . $RepMyBlock::CacheAdress_ResZip4[$i] . "\n";
	print "\n";
}
