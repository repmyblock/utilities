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

print "Slow Line by Line check of the Database\n";

### Connecting and Initializing.
my $RepMyBlock 		= RepMyBlock::NY->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");

print "State being considered: " . $RepMyBlock::DataStateID . "\n";

print "\nFinal Last Insert ID:\n";
$RepMyBlock->InitLastInsertID();

print "\nFinal Count Tables ID:\n";
$RepMyBlock->ListCountsTables();