#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;

use FindBin::libs;
use RepMyBlock;

print "Start the Empty Database program\n";

my $dbh 				= RepMyBlock::InitDatabase();

RepMyBlock::EmptyDatabases("VotersLastName", 1);
RepMyBlock::EmptyDatabases("VotersFirstName", 1);
RepMyBlock::EmptyDatabases("VotersMiddleName", 1);



