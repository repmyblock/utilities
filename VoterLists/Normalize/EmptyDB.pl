#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;

use FindBin::libs;
use RepMyBlock;
use RepMyBlock::NYS;

print "Start the Empty Database program\n";

my $dbh 				= RepMyBlock::InitDatabase();
my $DateTable 	= RepMyBlock::InitTheVoter();
my $DateTableID = RepMyBlock::DateDBID();

RepMyBlock::InitCaches();
RepMyBlock::EmptyDatabases("VotersLastName", 1);
RepMyBlock::EmptyDatabases("VotersFirstName", 1);
RepMyBlock::EmptyDatabases("VotersMiddleName", 1);



