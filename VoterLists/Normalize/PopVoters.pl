#!/usr/bin/perl

### Need to document what this file is about.
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

print "Blank the whole database";
RMBSchemas::CreateTable_Voters();
RMBSchemas::CreateTable_VotersFirstName();
RMBSchemas::CreateTable_VotersMiddleName();
RMBSchemas::CreateTable_VotersLastName();
RMBSchemas::CreateTable_VotersIndexes();
RMBSchemas::CreateTable_SystemUserQuery();
RMBSchemas::CreateTable_Elections();
RMBSchemas::CreateTable_ElectionsDistricts();
RMBSchemas::CreateTable_ElectionsDistrictsConv();
RMBSchemas::CreateTable_ElectionsPosition();

#RMBSchemas::CreateTable_SMSAccountHolder();
#RMBSchemas::CreateTable_SMSAuthorizedUsers();
#RMBSchemas::CreateTable_SMSCampaign();
#RMBSchemas::CreateTable_SMSPopInfo();
#RMBSchemas::CreateTable_SMSProvider();
#RMBSchemas::CreateTable_SMSTestPlan();
#RMBSchemas::CreateTable_SMSTestPlanNumbers();
#RMBSchemas::CreateTable_SMSText();
#RMBSchemas::CreateTable_SMSVerifyPhone();

print "Purging the data\n";
RepMyBlock::EmptyDatabases("Voters", 1);
RepMyBlock::EmptyDatabases("VotersIndexes", 1);

print "Copying the VoterCD Stuff\n";
$RepMyBlock::DateTable 		= RepMyBlock::InitTheVoter();
my $DateTableID 					= RepMyBlock::DateDBID();

print "Caching the data from the CD from date: " . $RepMyBlock::DateTable . "\n";
RepMyBlock::InitCaches();
my $TableDated = "Raw_Voter_" . $RepMyBlock::DateTable;

print "Dealing with the names and adding them to the Cache and Database if new\n";
RepMyBlock::NYS::TransferRawTables($TableDated);

my $start = time();

print "Loading the Voter Data\n";
my $VoterCounter = RepMyBlock::NY::LoadFromRawData($TableDated);

print "Starting to write the data to the database";
RepMyBlock::ReplaceVoterData($VoterCounter);
RepMyBlock::LoadVotersCache();
RepMyBlock::ReplaceIdxDatabase($VoterCounter);
