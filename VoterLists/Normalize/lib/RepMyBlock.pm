#!/usr/bin/perl

package RepMyBlock;

use strict;
use warnings;
use Config::Simple;

our %CacheLastName = ();
our %CacheFirstName = (); 
our %CacheMiddleName = (); 
our %CacheNYCVoterID = ();

our @AddPoolLastNames;
our @AddPoolFirstNames;
our @AddPoolMiddleNames;

our @CacheIdxLastName;
our @CacheIdxFirstName;
our @CacheIdxMiddleName;
our @CacheIdxSuffix;
our @CacheIdxDOB;
our @CacheIdxCode;

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
	my $cfg = new Config::Simple('/home/usracct/.repmyblockdb');
	
	### NEED TO FIND THE ID of that table.
	my $dbname = $cfg->param('dbname');
	my $dbhost = $cfg->param('dbhost');
	my $dbport = $cfg->param('dbport');
	my $dbuser = $cfg->param('dbuser');
	my $dbpass = $cfg->param('dbpass');

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
sub LoadLastNameCache { LoadCaches( \%CacheLastName, "VotersLastName", 0, 0); }
sub LoadFirstNameCache { LoadCaches( \%CacheFirstName, "VotersFirstName", 0, 0 ); }
sub LoadMiddleNameCache { LoadCaches( \%CacheMiddleName, "VotersMiddleName", 0, 0); }
sub LoadIndexCache { LoadCaches( \%CacheNYCVoterID, "VotersIndexes", 0, "VotersIndexes_UniqNYSVoterID"); }

#%Cache_DataCity = LoadCaches("SELECT * FROM DataCity");
##Cache_DataCounty = LoadCaches("SELECT * FROM DataCounty");
##Cache_DataState = LoadCaches("SELECT * FROM DataState");
#%Cache_DataStreet = LoadCaches("SELECT * FROM DataStreet");
#%Cache_Elections = LoadCaches("SELECT * FROM Elections");

sub LoadCaches {
	my $rhOptions = $_[0];
	my $tblname = $_[1];
	my $tblid = $_[2];
	my $colcheck = $_[3];
	my $Col = "*";

	if ( $colcheck ) { 
		$Col = $tblname . "_ID, " . $colcheck;
	}
 	
	my $sql = "SELECT $Col FROM " . $tblname;
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
		LoadCaches( $rhOptions, "Voters" . $Name, $LastInsertID, 0);
	}
}	


sub ReplaceIdxDatabase {
	my $Counter = $_[0];
	
	my $ExistCounter = 0;
	my @DontAddToDB = ();
	
	print "Counter: $Counter\n";
	LoadIndexCache();
	
	print "Donne LoadingIndexCache: $Counter\n";

	# We need to remove all the entries that already exist.
	for (my $i = 0; $i < $Counter; $i++) {
		if ( ! defined $RepMyBlock::CacheNYCVoterID { $RepMyBlock::CacheIdxCode[$i] } ) {
			$DontAddToDB[$ExistCounter] = $i;
			$ExistCounter++;			
		}
	}

	print "Folks I need to add: " . $ExistCounter . "\n";
	
	my $QueryDB;
	
	if ( $Counter > 0 && $ExistCounter > 0) {
		my $sql = "";
		my $first_time = 0;
		my $whole_sql = "";
		
		for ( my $j = 0; $j < $ExistCounter; $j++) {	
			my $i =	$DontAddToDB[$j];
			if ($first_time == 1) { $sql .= ","; } else { $first_time = 1; }
			
			$sql .= "(null," . $dbh->quote($RepMyBlock::CacheIdxLastName[$i]) . "," .	$dbh->quote($RepMyBlock::CacheIdxFirstName[$i]) . "," .
							$dbh->quote($RepMyBlock::CacheIdxMiddleName[$i]) . "," . $dbh->quote($RepMyBlock::CacheIdxSuffix[$i]) . "," .
							$dbh->quote($RepMyBlock::CacheIdxDOB[$i]) . "," . $dbh->quote($RepMyBlock::CacheIdxCode[$i]) . ")";

			if  ( (($i+1) % 50000) == 0 ) {
				#print "Doing a new insert: $i - $sql\n";
				$whole_sql = "INSERT INTO VotersIndexes VALUES " . $sql;
				$QueryDB = $dbh->prepare($whole_sql);
				$QueryDB->execute();		
				$first_time = 0;
				$sql = "";
			}
		}
		
		if ( $first_time == 1) {		
			#print "Doing a last insert: - $sql\n";	
			$whole_sql = "INSERT INTO VotersIndexes VALUES " . $sql;
			
			$QueryDB->execute();		
			$sql = "";
		}	
	}
	
	print "Done with added\n";
	
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
		my $sql = "TRUNCATE " . $tblname;
		
		my $QueryDB = $dbh->prepare($sql);
		$QueryDB->execute();		
		#$sql = "ALTER TABLE " . $tblname . " AUTO_INCREMENT=" . $number;
		#$QueryDB = $dbh->prepare($sql);
		#$QueryDB->execute();
	}
}

sub PrepareDBAddElection {
	my $sql = "INSERT INTO CandidateElection SET Elections_ID = ?, CandidateElection_PositionType = ?, CandidateElection_Party = ?, " . 
						"CandidateElection_Text = ?, CandidateElection_PetitionText = ?, CandidateElection_URLExplain = ?, " .
						"CandidateElection_Number = ?, CandidateElection_DisplayOrder = ?, CandidateElection_Display = ?, " .
						"CandidateElection_Sex = ?,	CandidateElection_DBTable = ?, CandidateElection_DBTableValue = ?, " .
						"CandidateElection_CountVoter = ?";
	my $DBAddElection = $dbh->prepare($sql);
	return $DBAddElection;
}

sub LoadPartyCall {
	my $Elections_ID = $_[0];
	my $rhOptions = $_[1];
		
	my $QueryDB = $dbh->prepare("SELECT * FROM ElectionsPartyCall " . 
															"LEFT JOIN CandidatePositions ON (CandidatePositions.CandidatePositions_ID = ElectionsPartyCall.CandidatePositions_ID) " .
															"LEFT JOIN DataCounty ON (DataCounty.DataCounty_ID = ElectionsPartyCall.DataCounty_ID) " .
															"WHERE Elections_ID = ?");
	$QueryDB->execute($Elections_ID);
	while (my $row = $QueryDB->fetchrow_hashref) {
		$rhOptions->{ $row->{'ElectionsPartyCall_ID'} } = $row;
	}
	
	return $rhOptions;
}


sub PartyAdjective {
	my $Party = $_[0];
	
	if ( ! defined $Party ) { return undef; }
	
	if ( $Party eq "DEM") {	return "Democratic"; } 
	if ( $Party eq "REP") { return "Republican"; } 
	if ( $Party eq "BLK") { return "No party"; } 
	if ( $Party eq "CON") { return "Conservatives"; } 
	if ( $Party eq "IND") { return "Independence Party"; } 
	if ( $Party eq "WOR") { return "Working Families"; } 
	if ( $Party eq "GRE") { return "Green"; } 
	if ( $Party eq "LBT") { return "Libertarian"; } 
	if ( $Party eq "OTH") { return "Other"; } 
	if ( $Party eq "WEP") { return "Women\'s Equality Party"; } 
	if ( $Party eq "REF") { return "Reform"; } 
	if ( $Party eq "SAM") { return "SAM"; }
	
	return undef;
}

1;
