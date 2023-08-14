#!/usr/bin/perl

use strict;
use Time::HiRes qw(time);
use DateTime;
use DBI;

my $dbname = "RepMyBlockTwo"; my $dbhost = "data.theochino.us"; 
my $dbport = "3306";
my $dbuser = "usracct"; my $dbpass = "usracct";
my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) 
						or die "Connection error: $DBI::errstr";
$dbh->{mysql_auto_reconnect} = 1;

my $start_time = Time::HiRes::gettimeofday();
my %DBData = ();

my $Stmt = "SELECT DataFirstName_ID, DataFirstName_Text FROM DataFirstName";
my $sth = $dbh->prepare( $Stmt ); $sth->execute() 
						or die "$! $DBI::errstr";
while (my @row = $sth->fetchrow_array()) {
	$DBData{ lc($row[1]) } = $row[0];
}

my $stop_time = Time::HiRes::gettimeofday();
printf("Perl loading the DB Information in %f\n", $stop_time - $start_time);

print "What is tne name you are seeking?\n";
my $name = lc <>;
$name=~ s/^\s+|\s+$//g;

$start_time = Time::HiRes::gettimeofday();
print "The name $name is index " . $DBData{$name} . "\n";
$stop_time = Time::HiRes::gettimeofday();
printf("Finding the Index Information in %f\n", $stop_time - $start_time);