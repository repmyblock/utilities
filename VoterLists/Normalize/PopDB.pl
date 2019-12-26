#!/usr/bin/perl

### Need to document what this file is about.


use strict;
use DBI;
use Text::CSV;

use FindBin::libs;
use RepMyBlock;
use RepMyBlock::NYS;

print "Start the program\n";

my $dbh 				= RepMyBlock::InitDatabase();
my $DateTable 	= RepMyBlock::InitTheVoter();
my $DateTableID = RepMyBlock::DateDBID();
RepMyBlock::InitCaches();

my $TableDated = "Raw_Voter_" . $DateTable;
RepMyBlock::NYS::TransferRawTables($TableDated);


print "This is in the MAIN PROGRAM\n";
RepMyBlock::PrintCache(\%RepMyBlock::CacheLastName);


#use Data::Dumper;
##print Dumper %RepMyBlock::Cache_LastName;
print "Cache last name in main: " . $RepMyBlock::CacheLastName { "AABERG" } . "\n";	
print "Cache last name in main: " . $RepMyBlock::CacheLastName { "AARNES" } . "\n";	

#$my $FullName = "Los Reyes de los cabos";
#print "Full Name: $FullName\n";
#print RepMyBlock::ReturnCompressed($FullName);
##print "Using TABLEDATE: $DateTable\n";
#print "DateTableID: $DateTableID\n";

print "Start Program\n";
my $start = time();
print "Set Variables\t";

