#!/usr/bin/perl

# At this time the assumption is that before you load each table,
# the cache is correct and not missing, otherwise the data might
# end up with whole.

# The order is Names first, then City and Street, then the rest of the address.

package Petition;

use strict;
#use warnings;
use Config::Simple;
use DateTime::Format::MySQL;
use Data::Dumper;
use Time::HiRes qw ( clock );

# Data to be automated later.
 
sub new { 
  my $class = shift; # defining shift in $myclass 
  my $self = {}; # the hashed reference 

  return bless $self, $class; 
} 

sub ExecuteQuery {
	my $self = shift;
	my $sql = shift;
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute();

	return $QueryDB->fetchrow_hashref;	
}

sub InitProdDatabase {
	my $self = shift;
	
	my $cfg = new Config::Simple('/home/usracct/.rmbproddb');

	### NEED TO FIND THE ID of that table.
	my $dbname = $cfg->param('dbname_prod');
	my $dbhost = $cfg->param('dbhost');
	my $dbport = $cfg->param('dbport');
	my $dbuser = $cfg->param('dbuser');
	my $dbpass = $cfg->param('dbpass');

	my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;mysql_ssl=1;";
	$self->{"dbh"} = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
	
	return $self->{"dbh"};
}

sub NextBatchID {
	my $self = shift;
	return $self->ExecuteQuery("SELECT MAX(CandidatePetitionSet_ID) AS LastID FROM NYSVoters.CanPetitionSet");
}

sub AddToCandidateTable {
	my $self = shift;
	my $sql = "INSERT INTO Candidate SET Candidate_UniqNYSVoterID = ?, CandidateElection_ID = ?, " .
															"Candidate_Party = ?, Candidate_DispName = ?, Candidate_DispResidence = ?, " .
															"Candidate_Status = ?";
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute($_[0], $_[1], $_[2], $_[3], $_[4], $_[5] );
	return $QueryDB->{mysql_insertid};
}

sub AddToCandidateSetTable {
	my $self = shift;
	my $sql = "INSERT INTO CanPetitionSet SET CandidatePetitionSet_ID = ?, Candidate_ID = ?, DataCounty_ID = ?, CanPetitionSet_Party = ?";
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute($_[0], $_[1], $_[2], $_[3]);
}

sub ReturnCandidateElection {
	my $self = shift;
	my $sql = "SELECT * FROM CandidateElection WHERE Elections_ID = ? AND CandidateElection_DBTable = ? AND CandidateElection_DBTableValue = ?";
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute($_[0], $_[1], $_[2]);
	return $QueryDB->fetchrow_hashref();
}

1;