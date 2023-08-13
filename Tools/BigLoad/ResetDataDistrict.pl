#!/usr/bin/perl

local $| = 1; # activate autoflush to immediately show the prompt

use strict;
use DBI;
use Time::HiRes;

my $dbname = "RepMyBlock";
my $dbhost = "data.theochino.us";
my $dbport = "3306";
my $dbuser = "usracct";
my $dbpass = "usracct";

my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
$dbh->{mysql_auto_reconnect} = 1;

my $sth = $dbh->prepare("SELECT * FROM DataDistrict");
$sth->execute() or die "$! $DBI::errstr";

my %DBData = ();
my $start_time = Time::HiRes::gettimeofday();
while (my @row = $sth->fetchrow_array()) {
	$DBData{$row[1]}{$row[2]}{$row[3]}{$row[4]}{$row[5]}{$row[6]}{$row[7]}{$row[8]}{$row[9]}{$row[10]} = 1;		
}
my $stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

my $mysql = "INSERT INTO DataDistrict VALUES ";
my $string = "";
my $first_time = 0;

my $Counter = 0;

foreach my $key1 (keys %DBData) {
	foreach my $key2 (keys %{$DBData{$key1}}) {
		foreach my $key3 (keys %{$DBData{$key1}{$key2}}) {
			foreach my $key4 (keys %{$DBData{$key1}{$key2}{$key3}}) {
				foreach my $key5 (keys %{$DBData{$key1}{$key2}{$key3}{$key4}}) {
					foreach my $key6 (keys %{$DBData{$key1}{$key2}{$key3}{$key4}{$key5}}) {
						foreach my $key7 (keys %{$DBData{$key1}{$key2}{$key3}{$key4}{$key5}{$key6}}) {
							foreach my $key8 (keys %{$DBData{$key1}{$key2}{$key3}{$key4}{$key5}{$key6}{$key7}}) {
								foreach my $key9 (keys %{$DBData{$key1}{$key2}{$key3}{$key4}{$key5}{$key6}{$key7}{$key8}}) {
									foreach my $key10 (keys %{$DBData{$key1}{$key2}{$key3}{$key4}{$key5}{$key6}{$key7}{$key8}{$key9}}) {
										
										if ( ($Counter % 1000000) == 0 && $Counter > 1) {
											my $sth = $dbh->prepare($mysql);
											$sth->execute() or die "$! $DBI::errstr";
											
											$mysql = "INSERT INTO DataDistrict VALUES ";
											$first_time = 0;
										}
										
										if ( $first_time == 0 ) { $first_time = 1 } else { $mysql .= ","; }	
									
										my $skey1; my $skey2; my $skey3;
										my $skey4; my $skey5; my $skey6;
										my $skey7; my $skey8; my $skey9;
										my $skey10;
										
										if ($key1 eq "") { $skey1 = "null"; } else { $skey1 = $dbh->quote($key1)}
										if ($key2 eq "") { $skey2 = "null"; } else { $skey2 = $dbh->quote($key2)}
										if ($key3 eq "") { $skey3 = "null"; } else { $skey3 = $dbh->quote($key3)}
										if ($key4 eq "") { $skey4 = "null"; } else { $skey4 = $dbh->quote($key4)}
										if ($key5 eq "") { $skey5 = "null"; } else { $skey5 = $dbh->quote($key5)}
										if ($key6 eq "") { $skey6 = "null"; } else { $skey6 = $dbh->quote($key6)}
										if ($key7 eq "") { $skey7 = "null"; } else { $skey7 = $dbh->quote($key7)}
										if ($key8 eq "") { $skey8 = "null"; } else { $skey8 = $dbh->quote($key8)}
										if ($key9 eq "") { $skey9 = "null"; } else { $skey9 = $dbh->quote($key9)}
										if ($key10 eq "") { $skey10 = "null"; } else { $skey10 = $dbh->quote($key10)}
																				
										$string = "(null," .	$skey1 . "," . $skey2 . "," . $skey3 . "," . $skey4 . "," . $skey5 . "," . $skey6 . "," . 
																		$skey7 . "," . $skey8 . "," . $skey9 . "," . $skey10 . ")"; 
										$mysql .= $string;				
										
										$Counter++;
																									
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

#my $sth = $dbh->prepare("TRUNCATE RepMyBlock.DataDistrict");
#$sth->execute() or die "$! $DBI::errstr";

my $sth = $dbh->prepare($mysql);
$sth->execute() or die "$! $DBI::errstr";