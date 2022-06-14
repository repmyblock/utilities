#!/usr/bin/perl

package Municipal;

use strict;
use warnings;

use Cwd;
use Config::Simple;
use Data::Dumper;
use DBI;

sub new {
	my $class = shift; # defining shift in $myclass 
  my $self = {};     # the hashed reference 
  return bless $self, $class; 
}

sub InitDatabase {
	my $self = shift;
	my $params = shift;
	
	my $TempConfigFile = ".db_postal_Courriel";
	my $ConfigFile = getcwd() . "/Modules/" . $TempConfigFile;
	
	print "ConfigFile: $ConfigFile\n";	
	my $cfg = new Config::Simple($ConfigFile);
	# print "Snd.pm Dumper cfg: " . Dumper($cfg) . "\n";

	### NEED TO FIND THE ID of that table.
	# dbname_voters: NYSVoters
	# dbname_rmb: VoterData
	my $dbname = $cfg->param('dbname');
	my $dbhost = $cfg->param('dbhost');
	my $dbport = $cfg->param('dbport');
	my $dbuser = $cfg->param('dbuser');
	my $dbpass = $cfg->param('dbpass');

	my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
	$self->{"dbh"} = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
	return $self->{"dbh"};
}

sub ExecuteQuery {
	my $self = shift;
		
	my $Counter = 0;
	my $sql = "";
	my $stmt = "";
	
	$stmt = $self->{"dbh"}->prepare($_[0]);
	$stmt->execute();

	while (my $row = $stmt->fetchrow_hashref) {
		foreach my $key (keys %$row ) {		
			${$_[1]}{$Counter}{$key} = $row->{$key};
		}
		$Counter++;
	}
		
	return $Counter;
}

sub PrintAllTables() {
	my $self = shift;
	
	# CacheRaw = %{$_[0]};
	foreach my $key (keys %{$_[0]} ) {		
		print $key . "\n";
	}
}


1;

