#!/usr/bin/perl

local $| = 1; # activate autoflush to immediately show the prompt

use strict;
use DBI;
use Time::HiRes;
use Lingua::EN::NameCase;

my $dbname = "RepMyBlock";
my $dbhost = "data.theochino.us";
my $dbport = "3306";
my $dbuser = "usracct";
my $dbpass = "usracct";

my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
$dbh->{mysql_auto_reconnect} = 1;

my $sth = $dbh->prepare("SELECT * FROM DataCity");
my %DBData = ();

$sth->execute() or die "$! $DBI::errstr";

my $start_time = Time::HiRes::gettimeofday();
while (my @row = $sth->fetchrow_array()) {
	$DBData{$row[1]} = 1;		
}
my $stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

my $mysql = "INSERT INTO DataCity VALUES ";
my $string = "";
my $first_time = 0;

my $Counter = 0;

foreach my $key1 (keys %DBData) {
	if ( length($key1) > 0 ) {
		$key1 =~ s/\s+/ /g;
		my $name = nc($key1);
		if ( $first_time == 0 ) { $first_time = 1 } else { $mysql .= ","; }
		$mysql .= "(null, " . $dbh->quote($name) . ")";
	}
}

my $sth = $dbh->prepare("TRUNCATE DataCity");
$sth->execute() or die "$! $DBI::errstr";

my $sth = $dbh->prepare($mysql);
$sth->execute() or die "$! $DBI::errstr";