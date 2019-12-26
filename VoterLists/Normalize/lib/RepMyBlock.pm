#!/usr/bin/perl

package RepMyBlock;

use strict;
use warnings;

our %CacheFirstName = (); 
our %CacheMiddleName = (); 
our %CacheLastName = ();

our @AddPoolLastNames;

our $DateTable;
our $dbh;	

sub InitTheVoter {
	# Read the Table Directory in the file
	my $filename = '/home/usracct/.voter_file';
	open(my $fh, '<:encoding(UTF-8)', $filename) or die "Could not open file '$filename' $!";
	my $tabledate = <$fh>;
	chomp($tabledate);
	close($fh);
	return $tabledate;
}

sub InitDatabase {
	### NEED TO FIND THE ID of that table.
	my $dbname = "NYSVoters";
	my $dbhost = "localhost";
	my $dbport = "3306";
	my $dbuser = "";
	my $dbpass = "";

	my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
	$dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
	
	return $dbh;
}

sub InitCaches {
	LoadLastNameCache();
}

sub DateDBID {	
	my $stmtFindDatesID = $dbh->prepare("SELECT Raw_Voter_Dates_ID FROM Raw_Voter_Dates WHERE Raw_Voter_Dates_Date = ?");
	$stmtFindDatesID->execute($DateTable);
	my @row = $stmtFindDatesID->fetchrow_array;
	return $row[0];
}

sub ReturnCompressed {
	my $FieldContent = $_[0];
	my $CompressFieldName = $FieldContent;
	$CompressFieldName =~ tr/a-zA-Z//dc;
	return $CompressFieldName;
}

### Load Cache
sub LoadLastNameCache { LoadCaches( \%CacheLastName, "VotersLastName", 0); }
sub LoadFirstNameCache { LoadCaches( \%CacheFirstName, "VotersFirstName", 0); }
sub LoadMiddleNameCache { LoadCaches( \%CacheMiddleName, "VotersMiddleName", 0); }

#%Cache_DataCity = LoadCaches("SELECT * FROM DataCity");
##Cache_DataCounty = LoadCaches("SELECT * FROM DataCounty");
##Cache_DataState = LoadCaches("SELECT * FROM DataState");
#%Cache_DataStreet = LoadCaches("SELECT * FROM DataStreet");
#%Cache_Elections = LoadCaches("SELECT * FROM Elections");

sub LoadCaches {
	my $rhOptions = $_[0];
	my $tblname = $_[1];
	my $tblid = $_[2];
	
	my $sql = "SELECT * FROM " . $tblname;
	if ( $tblid > 0) {
		$sql .= " WHERE " . $tblname . "_ID >= " . $dbh->quote($tblid);
	}
	
  my $QueryDB = $dbh->prepare($sql);
	$QueryDB->execute();
	
	for (my $i = 0; $i < $QueryDB->rows; $i++) {
		my @row = $QueryDB->fetchrow_array;	
		$rhOptions->{ $row[1] } = $row[0];
	}
}

sub AddToDatabase {
	my $Name = $_[0];
	my $Counter = $_[1];
	my @Pool = @{$_[2]};
	my $rhOptions = $_[3];
		
	my $QueryDB = $dbh->prepare("SELECT AUTO_INCREMENT FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = ? AND TABLE_NAME = ?");
	$QueryDB->execute("NYSVoters", "Voters" . $Name);
	my @row = $QueryDB->fetchrow_array;	
	my $LastInsertID = $row[0];
	
	if ( $Counter > 0 ) {
		my $sql = "INSERT INTO Voters" . $Name . " VALUES ";	
		for (my $i = 0; $i < $Counter; $i++) {
			if ($i > 0) { $sql .= ","; }
			$sql .= "(null," . $dbh->quote($Pool[$i]) . "," . $dbh->quote(ReturnCompressed($Pool[$i])) . ")";
		}
		$QueryDB = $dbh->prepare($sql);
		$QueryDB->execute();		
		LoadCaches( $rhOptions, "Voters" . $Name, $LastInsertID);
	}
}	

sub PrintCache {
	my $rhOptions = $_[0];
	print "Printing cache ...\n";	
	while ( my ($key, $value) = each(%$rhOptions) ) {
  	print "\t$key => $value\n";
  }
}
	
sub EmptyDatabases {
	my $tblname = $_[0];
	my $number = $_[1];
	
	if ( length ($tblname) > 0 && $number > 0) {
		my $sql = "DELETE FROM " . $tblname;
		my $QueryDB = $dbh->prepare($sql);
		$QueryDB->execute();		
		$sql = "ALTER TABLE " . $tblname . " AUTO_INCREMENT=" . $number;
		$QueryDB = $dbh->prepare($sql);
		$QueryDB->execute();
	}
}
 
1;
