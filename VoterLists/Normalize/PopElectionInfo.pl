#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;

use FindBin::libs;
use RepMyBlock;
use RepMyBlock::NYS;

print "Start the program\n";
print "Connecting to the databases\n";
$RepMyBlock::dbhRawVoters = RepMyBlock::InitDatabase("dbname_voters");
$RepMyBlock::dbhVoters		= RepMyBlock::InitDatabase("dbname_rmb");

print "Copying the VoterCD Stuff\n";
$RepMyBlock::DateTable 		= RepMyBlock::InitTheVoter();
my $DateTableID 					= RepMyBlock::DateDBID();
RepMyBlock::InitCaches();
my $TableDated = "Raw_Voter_" . $RepMyBlock::DateTable;
use Data::Dumper;

print "Done Loading Historical Stuff\n";
RepMyBlock::NYS::LoadVoterHistoryData($TableDated);

print "Loading the historical data\n";
RepMyBlock::LoadHistoryCache();

print "Updating Historical Stuff\n";
RepMyBlock::WriteHistoryData();
