#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;

use FindBin::libs;
use RMBSchemas;

use RepMyBlock::OH;
use RepMyBlock::NY;

my $StateToUse = "RepMyBlock::NY";

print "Start the program: $StateToUse\n";
print "Connecting to the databases\n";
my $RepMyBlock 		= ${StateToUse}->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");

print "Init the voters: " . $RepMyBlock->InitializeVoterFile() . "\n";

my $Schemas = RMBSchemas->new();
$Schemas->SetDatabase($dbhVoters);

while(defined(my $answer = prompt("type q or quit to exit (initcache, deletedata, deletealldata, loaddata, recreatedb, printall): "))) {	
  last if $answer eq "q"
       or $answer eq "quit";
  
  if ($answer eq "deletedata") {
  	
  	my $total = prompt("How many do you want to delete: ");
  	print "Deleting Name Database\n";
  	$RepMyBlock->EmptyDatabases("VotersFirstName", $total);
		$RepMyBlock->EmptyDatabases("VotersMiddleName", $total);
		$RepMyBlock->EmptyDatabases("VotersLastName", $total);
		
	} elsif ($answer eq "deletealldata") {
  	
  	print "Deleting all data from database\n";
  	$RepMyBlock->EmptyDatabases("VotersFirstName");
		$RepMyBlock->EmptyDatabases("VotersMiddleName");
		$RepMyBlock->EmptyDatabases("VotersLastName");	
		
	}	elsif ($answer eq "loaddata") {
  	
  	print "Loading the data\n";
		$RepMyBlock->SetDatabase($dbhRawVoters);
		$RepMyBlock->LoadUpdateNamesCache();
		$RepMyBlock->SetDatabase($dbhVoters);
		$RepMyBlock->AddNamesCacheIntoDabase();
		print "Loaded the data\n";
	
	}	elsif ($answer eq "recreatedb") {
  	
  	print "Recreate the data\n";	
		$Schemas->CreateTable_VotersFirstName(1);
		$Schemas->CreateTable_VotersMiddleName(1);
		$Schemas->CreateTable_VotersLastName(1);
		
	}	elsif ($answer eq "printall") {
  	
  	print "Printing the list\n";	
		$RepMyBlock->PrintAll_VotersFirstName();
		$RepMyBlock->PrintAll_VotersMiddleName();
		$RepMyBlock->PrintAll_VotersLastName();
  
  }	elsif ($answer eq "initcache") {
  
 	 $RepMyBlock->InitNamesCaches();
  
  } else {
  
	  print "The name $answer as an ID of for:\n";
  	print "\tFirst Name: " . $RepMyBlock->PrintFirstName($answer) . "\n";
  	print "\tMiddle Name: " . $RepMyBlock->PrintMiddleName($answer) . "\n";
  	print "\tLast Name: " . $RepMyBlock->PrintLastName($answer) . "\n";
  	print "\n";
  }
       
	print "Counter in Cache - last name: " . $RepMyBlock::CounterLastName . 
					" - firstname: " . $RepMyBlock::CounterFirstName . 
					" - middle name: " . $RepMyBlock::CounterMiddleName . "\n";
}

sub prompt {
  my ($challenge) = @_;
  local $| = 1;  # set autoflush;
  print $challenge;
  chomp( my $answer = <STDIN> // return undef);
  return $answer;
}
