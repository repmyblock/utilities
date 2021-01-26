#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;

use FindBin::libs;
use RMBSchemas;

use RepMyBlock::NY;
use RepMyBlock::OH;

print "Start the program\n";
print "Connecting to the databases\n";
my $RepMyBlock 		= RepMyBlock::OH->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");

print "Set Database Schema\n";
my $Schemas = RMBSchemas->new();
$Schemas->SetDatabase($dbhVoters);
$Schemas->CreateTable_VotersIndexes(1);
$Schemas->CreateTable_Voters(1);

print "Set the databases\n";
print "Init the voters: " . $RepMyBlock->InitializeVoterFile() . "\n";

#exit();
##$RepMyBlock::DateTable 		= RepMyBlock::InitTheVoter();
#print "Purging the data\n";
#RepMyBlock::EmptyDatabases("Voters", 1);
#RepMyBlock::EmptyDatabases("VotersIndexes", 1);
#my $DateTableID 					= RepMyBlock::DateDBID();

# I need to find the State of the file.
$RepMyBlock::DataStateID = "2";
$RepMyBlock::DBTableName = "OHPRCNT";

$RepMyBlock->InitNamesCaches();
print "Caching the data from the CD from date: " . $RepMyBlock::DateTable . "\n";
my $TableDated = "OH_Raw_" . $RepMyBlock::DateTable;
print "TableDated: $TableDated\n";

$RepMyBlock->LoadUpdateNamesCache();

print "Dealing with the names and adding them to the Cache and Database if new\n";
#RepMyBlock::OH::TransferRawTables($TableDated);

exit();
my $start = time();

print "Loading the Voter Data\n";
#my $VoterCounter = RepMyBlock::NY::LoadFromRawData($TableDated);
my $VoterCounter = RepMyBlock::OH::LoadFromRawData($TableDated);

print "Starting to write the data to the database";
RepMyBlock::ReplaceVoterData($VoterCounter);
RepMyBlock::LoadVotersCache();
RepMyBlock::ReplaceIdxDatabase($VoterCounter);
