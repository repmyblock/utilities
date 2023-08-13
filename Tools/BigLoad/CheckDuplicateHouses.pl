#!/usr/bin/perl

local $| = 1; # activate autoflush to immediately show the prompt

my $InsertDBEachCount = 1000000; 
my $TimePerCycle = 1000000;

my $SystemState = "1";

use strict;
use DBI;
use Time::HiRes;
use Text::CSV;
use Lingua::EN::NameCase;
use Encode;

my $dbname = "RepMyBlock";
my $dbhost = "data.theochino.us";
my $dbport = "3306";
my $dbuser = "usracct";
my $dbpass = "usracct";

my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
$dbh->{mysql_auto_reconnect} = 1;

### Load all the Datas
my %DBData = ();

my @TableNames  = qw/DataDistrictTemporal/;
my $LocalLimit;

my $TableCounter = 0;


#### LoadDataFromFile
foreach my $Table (@TableNames) { 
	print "Loading $Table \t"; 
	my $Stmt_FirstName = "SELECT * FROM " . $Table  . " " . $LocalLimit;
	my $sth = $dbh->prepare( $Stmt_FirstName ); $sth->execute() or die "$! $DBI::errstr";
	my $start_time = Time::HiRes::gettimeofday();
	
	while (my @row = $sth->fetchrow_array()) {
		$row[1] =~ s/\s+$//;    ### The address need triming.

		if ( $Table eq "DataDistrictTemporal") {			
			$DBData{$row[2]}{$row[1]}{$row[3]} = $row[0];
		}
	}
		
	my $stop_time = Time::HiRes::gettimeofday();
	printf("%.2f\n", $stop_time - $start_time);
} 

my $DataHouseID = 0;
foreach my $key1 (keys %DBData) {
	foreach my $key2 (keys %{$DBData{$key1}}) {
		foreach my $key3 (keys %{$DBData{$key1}{$key2}}) {
			print "\$DBData{" . $key1 . "}{" . $key2 . "}{" . $key3 . "} \t-> \t";
			print "#" . $DBData{$key1}{$key2}{$key3}. "#\n";											
			
			if ( $key1 > $DataHouseID ) {
				$DataHouseID = $key1;
			}
			
			
		}
	}
}
				
print "DataHouse: $DataHouseID\n";

