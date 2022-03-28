#!/usr/bin/perl

### Need to document what this file is about.

use strict;
use DBI;
use Text::CSV;
use NYS_Normalize;

print "Start the program\n";

# Read the Table Directory in the file
my $filename = '/home/usracct/.voter_file';
open(my $fh, '<:encoding(UTF-8)', $filename) or die "Could not open file '$filename' $!";
my $tabledate = <$fh>;
chomp($tabledate);
close($fh);

print "Using TABLEDATE: $tabledate\n";

my $DateTable = $tabledate;

### NEED TO FIND THE ID of that table.
my $dbname = "NYSVoters";
my $dbhost = "localhost";
my $dbport = "3306";
my $dbuser = "";
my $dbpass = "";

my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";

my $stmtFindDatesID = $dbh->prepare("SELECT Raw_Voter_Dates_ID FROM NYSVoters.Raw_Voter_Dates WHERE Raw_Voter_Dates_Date = ?");
$stmtFindDatesID->execute($DateTable);
my @row = $stmtFindDatesID->fetchrow_array;
my $DateTableID = $row[0];

print "DateTableID: $DateTableID\n";

my $TableDated = "Raw_Voter_" . $DateTable;

print "Start Program\n";
my $start = time();
print "Set Variables\t";
### Cache variables.
my %Cache_FirstName = (); my %Cache_MiddleName = (); my %Cache_LastName = (); my %Cache_DataCity = (); my %Cache_DataCounty = ();
my %Cache_DataState = (); my %Cache_DataStreet = (); my %Cache_Elections = (); my %Cache_VoterIndex = (); my %Cache_DataAddress = ();
my %Cache_DataHouse = (); my %Cache_RawVoter = (); my %Cache_RawVoterByID = (); my %Cache_ComplementInfo = (); my %Cache_MailingAddress = (); 
