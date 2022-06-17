#!/usr/bin/perl

package ServermailDB;

use strict;
use warnings;

use Cwd;
use Config::Simple;
use Data::Dumper;
use DBI;

sub new {
	my $class = shift; # defining shift in $myclass 
  my $self = {};     # the hashed reference 
  
	#y $TempConfigFile = ".db_postal_Servermail";
	#my $ConfigFile = $ENV{'HOME'} . "/SendEmail/Modules/" . $TempConfigFile;
	
	my $TempConfigFile = ".db_mysqldb01_ServerMail";
	my $ConfigFile = $ENV{'HOME'} . "/" . $TempConfigFile;
	
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

	my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;mysql_ssl=1;";
	$self->{"dbh"} = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
	
	return bless $self, $class; 
}

sub CheckModule {
	my $self = shift;
	
	my $stmt = $self->{"dbh"}->prepare("SELECT * FROM ZentModules WHERE ZentModules_Name = ?");
	$stmt->execute($_[0]);
	
	my $row = $stmt->fetchrow_hashref;
	if ( ! defined $row->{'ZentModules_ID'} ) {
		$stmt = $self->{"dbh"}->prepare("INSERT INTO ZentModules SET ZentModules_Name = ?, ZentModules_TimeStamp = NOW()");
		$stmt->execute($_[0]);
		return $stmt->{mysql_insertid};
	}
	return $row->{'ZentModules_ID'};
}


sub LoadSentEmail {
	my $self = shift;
	
	my $stmt = $self->{"dbh"}->prepare("SELECT * FROM ZentEmails WHERE ZentModules_ID = ?");
	$stmt->execute($_[0]);
	
	while (my $row = $stmt->fetchrow_hashref) {
		${$_[1]}{$row->{'ZentEmails_Email'}} = 1;
	}
}

sub InsertSentEmail {
	my $self = shift;
	
	my $stmt = $self->{"dbh"}->prepare("INSERT INTO ZentEmails SET ZentEmails_Email = ?, ZentModules_ID = ?, ZentModules_TimeStamp = NOW()");
	$stmt->execute($_[0], $_[1]);
	${$_[2]}{$_[0]} = 1;
}

sub ExecuteQuery {
	my $self = shift;
		
	my $Counter = 0;
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


1;

