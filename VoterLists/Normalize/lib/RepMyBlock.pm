#!/usr/bin/perl

package RepMyBlock;

use strict;
use warnings;
use Config::Simple;
use DateTime::Format::MySQL;
use Lingua::EN::NameCase 'NameCase' ;

# Data to be automated later.
our $DataStateID = "1";
our $DBTableName = "EDAD";

our %CacheLastName = ();
our %CacheFirstName = (); 
our %CacheMiddleName = (); 
our %CacheNYCVoterID = ();
our %CacheCityName = ();
our %CacheStreetName = ();
our %CacheStateName = ();
our %CacheCountyName = ();

our @AddPoolLastNames;
our @AddPoolFirstNames;
our @AddPoolMiddleNames;

our @CacheIdxLastName;
our @CacheIdxFirstName;
our @CacheIdxMiddleName;
our @CacheIdxSuffix;
our @CacheIdxDOB;
our @CacheIdxCode;

our @CacheVoter_Gender; 
our @CacheVoter_EnrollPolParty;
our @CacheVoter_DBTableValue; 
our @CacheVoter_CountyVoterNumber;
our @CacheVoter_RegistrationCharacter;
our @CacheVoter_ApplicationSource;
our @CacheVoter_IDRequired;
our @CacheVoter_IDMet;
our @CacheVoter_Status;
our @CacheVoter_ReasonCode;
our @CacheVoter_VoterMadeInactive;
our @CacheVoter_VoterPurged;
our @CacheVoter_UniqStateVoterID;

our @CacheVoter_Street; 
our @CacheVoter_City;

our %CachePlainVoter = ();
our %CacheVoterHistory = ();

our $DateTable;
our $dbhRawVoters;	
our $dbhVoters;
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
	my $params= $_[0];
	my $cfg = new Config::Simple('/home/usracct/.repmyblockdb');
	
	### NEED TO FIND THE ID of that table.
	#dbname_voters: NYSVoters
	#dbname_rmb: VoterData
	my $dbname = $cfg->param($params);
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
	LoadFirstNameCache();
	LoadMiddleNameCache();
}

sub InitStreetCaches() {
	LoadResStreetName();
	LoadResCity();
}

sub DateDBID {
	my $stmtFindDatesID = $dbhRawVoters->prepare("SELECT Raw_Voter_Dates_ID FROM Raw_Voter_Dates WHERE Raw_Voter_Dates_Date = ?");
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
sub LoadIndexCache { LoadCaches( \%CacheNYCVoterID, "VotersIndexes", 0, "VotersIndexes_UniqStateVoterID"); }
sub LoadHistoryCache { LoadCaches( \%CacheVoterHistory, "Elections", 0, 0); }
sub LoadResStreetName { LoadCaches( \%CacheStreetName, "DataStreet", 0, 0); }
sub LoadResCity { LoadCaches( \%CacheCityName, "DataCity", 0, 0); }
sub LoadResState { LoadCaches( \%CacheStateName, "DataState", 0, 0); }
	
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
	
  my $QueryDB = $dbhVoters->prepare($sql);
	$QueryDB->execute();
	
	for (my $i = 0; $i < $QueryDB->rows; $i++) {
		my @row = $QueryDB->fetchrow_array;	
		$rhOptions->{ $row[1] } = trim($row[0]);
	}
}

sub AddToDatabase {
	my $Name = $_[0];
	my $Counter = $_[1];
	my @Pool = @{$_[2]};
	my $rhOptions = $_[3];
		
	my $QueryDB = $dbhVoters->prepare("SELECT AUTO_INCREMENT FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = ? AND TABLE_NAME = ?");
	$QueryDB->execute("VoterData", "Voters" . $Name);
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

sub AddToShortDatabase {
	my $Name = $_[0];
	my $Counter = $_[1];
	my @Pool = @{$_[2]};
	my $rhOptions = $_[3];
			
	my $QueryDB = $dbhVoters->prepare("SELECT AUTO_INCREMENT FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = ? AND TABLE_NAME = ?");
	$QueryDB->execute("VoterData", "Voters" . $Name);
	my @row = $QueryDB->fetchrow_array;	
	my $LastInsertID = $row[0];
	
	if ( $Counter > 0 ) {
		my $sql = "INSERT INTO " . $Name . " VALUES ";	
		for (my $i = 0; $i < $Counter; $i++) {
			if ($i > 0) { $sql .= ","; }
			$sql .= "(null," . $dbh->quote(trim($Pool[$i])) . ")";
		}
		$QueryDB = $dbh->prepare($sql);
		$QueryDB->execute();		
		LoadCaches( $rhOptions, $Name, $LastInsertID, 0);
	}
}	

sub BulkAddToShortDatabase {
	my $Name = $_[0];
	my $Counter = $_[1];
	my @Pool = @{$_[2]};
	my $rhOptions = $_[3];
	
	my $sql = "";
	
	if ( $Counter > 0 ) {
		$sql = "INSERT IGNORE INTO " . $Name . " VALUES ";	
		for (my $i = 0; $i < $Counter; $i++) {
			if ($i > 0) { $sql .= ","; }
			$sql .= "(null," . $dbh->quote(trim($Pool[$i])) . ")";
		}
	}

	my $QueryDB = $dbhVoters->prepare($sql);
	$QueryDB->execute();		
}	

sub LoadVotersCache {
	
	my $sql = "SELECT Voters_ID, Voters_UniqStateVoterID, Voters_Status, Voters_DatePurged, Voters_DateInactive, Voters_ApplyDate, " . 
						"Voters_RecLastSeen FROM Voters WHERE DataState_ID = ?";
  my $QueryDB = $dbhVoters->prepare($sql);
	$QueryDB->execute($DataStateID);
	
	my %LocalCache = ();
	my $Counter = 0;
			
	print "Loading function LoadVoterCache\n";
	while(my @row = $QueryDB->fetchrow_array) {
		
		if ( ++$Counter % 500000 == 0 ) {
			print "Counter: $Counter\n\033[1A";
		}
		
		
		if ( defined $RepMyBlock::CachePlainVoter->{ $row[1] } ) {
			
			#print "We already have this voter defined: " . $row[1] . "\tCachePlainVoter: " . $RepMyBlock::CachePlainVoter->{ $row[1] } . "\n";
			
			if ( $row[2] eq "active" && $LocalCache { $row[1] } { 'status' }  eq "active" ) {
				print "Duplicate but have two active ...\n";
				exit();
		
			}	else {

				#print "Already done voter: " . $row[1] . "\tCachePlainVoter: " . $RepMyBlock::CachePlainVoter->{ $row[1] } . "\n";
				
 				if ( $row[2] eq 'active' ) {
					
					$LocalCache { $row[1] } { 'datelastfileseen' } = $row[6];
					$LocalCache { $row[1] } { 'applydate' } = $row[5];
					$LocalCache { $row[1] } { 'dateinactive' } = $row[4];
					$LocalCache { $row[1] } { 'datepurged' } = $row[3];
					$LocalCache { $row[1] } { 'status' } = $row[2];
					$RepMyBlock::CachePlainVoter->{ $row[1] } = $row[0];
					
				} else {		
			
					
					if ( $row[2] eq "Inactive" && $LocalCache { $row[1] } { 'status' }  ne "active") {
						$RepMyBlock::CachePlainVoter->{ $row[1] } = $row[0];
						
					} else {
									
						if ( $LocalCache { $row[1] } { 'status' } eq $row[2] ) {
							
							my $OlDate = DateTime::Format::MySQL->parse_date( $LocalCache { $row[1] } { 'applydate' } );
							my $NewDate = DateTime::Format::MySQL->parse_date( $row[5]);
						
							if ( $OlDate < $NewDate ) {
								$RepMyBlock::CachePlainVoter->{ $row[1] } = $row[0];
							} 						
						}
						
					}
				}													
			}
			
			# print "We have finished swapping defined voter: " . $row[1] . "\tCachePlainVoter: " . $RepMyBlock::CachePlainVoter->{ $row[1] } . "\n";
			# if ($row[1] eq "NY000000000034338533") {  #Example Pre Reg: NY000000000058192734
			#	  exit();
			# }
			
		} else {
			$LocalCache { $row[1] } { 'datelastfileseen' } = $row[6];
			$LocalCache { $row[1] } { 'applydate' } = $row[5];
			$LocalCache { $row[1] } { 'dateinactive' } = $row[4];
			$LocalCache { $row[1] } { 'datepurged' } = $row[3];
			$LocalCache { $row[1] } { 'status' } = $row[2];
			$RepMyBlock::CachePlainVoter->{ $row[1] } = $row[0];
		}
	}

	print "I am done loading the Cache\n";

}

sub UpdateIndexTable() {
	
	print "Update the Index Table\n";	
	my $Counter = 0;
	my $sql = "UPDATE VotersIndexes SET Voters_ID = ? WHERE VotersIndexes_UniqStateVoterID = ?";
  my $QueryDB = $dbhVoters->prepare($sql);
	
	foreach my $key (keys %{ $RepMyBlock::CachePlainVoter} ) {		
	  $QueryDB->execute($RepMyBlock::CachePlainVoter->{ $key }, $key);	  
	  if ( ++$Counter % 500000 == 0 ) {
			print "Counter: $Counter\n\033[1A";
		}
		print "Done $Counter\n";
	}
	
}


sub ReplaceIdxDatabase {
	my $Counter = $_[0];
	
	my $ExistCounter = 0;
	my @DontAddToDB = ();
	
	print "ReplaceIDXDB: Received Counter: $Counter\n";
	LoadIndexCache();
	
	#	ReplaceIdxDatabase
	# print "Donne LoadingIndexCache: $Counter\n";

	# We need to remove all the entries that already exist.
	for (my $i = 0; $i < $Counter; $i++) {
		if ( ! defined $RepMyBlock::CacheNYCVoterID { $RepMyBlock::CacheIdxCode[$i] } ) {
			$DontAddToDB[$ExistCounter] = $i;
			$ExistCounter++;			
		}
	}

	print "ReplaceIDXDB: After calculation, the new total is: " . $ExistCounter . "\n";
	
	my $QueryDB;

	print "ReplaceIDXDB: Printing the VotersID\n";

	if ( $Counter > 0 && $ExistCounter > 0) {
		my $sql = "";
		my $first_time = 0;
		my $whole_sql = "";
			
		for ( my $j = 0; $j < $ExistCounter; $j++) {	
			my $i =	$DontAddToDB[$j];
			if ($first_time == 1) { $sql .= ","; } else { $first_time = 1; }
		  
			
			
			$sql .= "(null, " . $dbh->quote( $RepMyBlock::CachePlainVoter->{ $RepMyBlock::CacheIdxCode[$i] } ) . ", '1', " . 
							$dbh->quote($RepMyBlock::CacheIdxLastName[$i]) . "," .	$dbh->quote($RepMyBlock::CacheIdxFirstName[$i]) . "," .
							$dbh->quote($RepMyBlock::CacheIdxMiddleName[$i]) . "," . $dbh->quote($RepMyBlock::CacheIdxSuffix[$i]) . "," .
							$dbh->quote($RepMyBlock::CacheIdxDOB[$i]) . "," . $dbh->quote($RepMyBlock::CacheIdxCode[$i]) . ")";

			if  ( (($i+1) % 50000) == 0 ) {
				print "Counter: $j - To insert in VotersIndexes: " . ($ExistCounter - $j) . "\n\033[1A";
				$whole_sql = "INSERT INTO VotersIndexes VALUES " . $sql;
				$QueryDB = $dbhVoters->prepare($whole_sql);
				$QueryDB->execute();		
				$first_time = 0;
				$sql = "";
			}
		}
		
		if ( $first_time == 1) {		
			$whole_sql = "INSERT INTO VotersIndexes VALUES " . $sql;		
			$QueryDB = $dbhVoters->prepare($whole_sql);
			$QueryDB->execute();		
			$sql = "";
		}	
	}
	
	print "Done with added\n";
	
}	

sub ReplaceVoterData {
	my $Counter = $_[0];
	my $ExistCounter =  $Counter;
	my @DontAddToDB = ();
	
	print "Counter: $Counter\n";
	##LoadVoterCache();
	
	print "Donne LoadingVoterCache: $Counter\n";

	# We need to remove all the entries that already exist.
	# for (my $i = 0; $i < $Counter; $i++) {
	# 	if ( ! defined $RepMyBlock::CacheNYCVoterID { $RepMyBlock::CacheIdxCode[$i] } ) {
	# 		$DontAddToDB[$ExistCounter] = $i;
	# 		$ExistCounter++;			
	# 	}
 	# }

	print "Folks I need to add: " . $ExistCounter . "\n";
	
	my $QueryDB;
	my $j = 0;
	 
	if ( $Counter > 0 && $ExistCounter > 0) {
		my $sql = "";
		my $first_time = 0;
		my $whole_sql = "";
		
		for ($j = 0; $j < $ExistCounter; $j++) {	
			my $i =	$j; #$DontAddToDB[$j];
			if ($first_time == 1) { $sql .= ","; } else { $first_time = 1; }
			
			$sql .= "(null, " . $dbh->quote($DBTableName) . "," . 
													$dbh->quote($RepMyBlock::CacheVoter_DBTableValue[$i]) . "," . 
													$dbh->quote(RepMyBlock::NYS::ReturnGender($RepMyBlock::CacheVoter_Gender[$i])) . ",null," .
													$dbh->quote($RepMyBlock::CacheVoter_UniqStateVoterID[$i]) . "," . 
													$dbh->quote($RepMyBlock::DataStateID) . "," . 
													$dbh->quote($RepMyBlock::CacheVoter_EnrollPolParty[$i]) . "," . 
													$dbh->quote(RepMyBlock::NYS::ReturnReasonCode($RepMyBlock::CacheVoter_ReasonCode[$i])) . "," . 
													$dbh->quote(RepMyBlock::NYS::ReturnStatusCode($RepMyBlock::CacheVoter_Status[$i])) . ",null," . 
													$dbh->quote(RepMyBlock::NYS::ReturnYesNo($RepMyBlock::CacheVoter_IDRequired[$i])) . "," . 
													$dbh->quote(RepMyBlock::NYS::ReturnYesNo($RepMyBlock::CacheVoter_IDMet[$i])) . "," . 
													$dbh->quote($RepMyBlock::CacheVoter_RegistrationCharacter[$i]) . "," . 
													$dbh->quote(RepMyBlock::NYS::ReturnRegistrationSource($RepMyBlock::CacheVoter_ApplicationSource[$i])) . "," . 
													$dbh->quote($RepMyBlock::CacheVoter_VoterMadeInactive[$i]) . "," . 
													$dbh->quote($RepMyBlock::CacheVoter_VoterPurged[$i]) . "," . 
													$dbh->quote($RepMyBlock::CacheVoter_CountyVoterNumber[$i]) . "," . 
													$dbh->quote($DateTable) . "," .
													$dbh->quote($DateTable) . 
													")";
		
			if  ( (($i+1) % 75000) == 0 ) {
				print "Inserted " . ($i+1) . " records - Still " . ($ExistCounter - $i) . " to go\n\033[1A";
				$whole_sql = "INSERT INTO Voters VALUES " . $sql;
				$QueryDB = $dbhVoters->prepare($whole_sql);
				$QueryDB->execute();		
				$first_time = 0;
				$sql = "";
			}
		}
		
		if ( $first_time == 1) {		
			print "Doing a last insert of $j records\n";	
			$whole_sql = "INSERT INTO Voters VALUES " . $sql;
			$QueryDB = $dbhVoters->prepare($whole_sql);
			$QueryDB->execute();		
			$sql = "";
		}	
	}
	
	print "Done inserting $j record to Voters table\n";		
}



sub PrintCache {
	my $rhOptions = $_[0];
	print "Printing cache ...\n";	
	while ( my ($key, $value) = each(%$rhOptions) ) {
  	print "\t$key => $value\n";
  }
}

sub WriteHistoryData {
	my $QueryDB;
	my $i = 0;
	my $sql = "";
	my $first_time = 0;
	my $whole_sql = "";

	foreach my $key ( keys %RepMyBlock::CacheVoterHistory ) {		
		
		if ( $RepMyBlock::CacheVoterHistory {$key} == 0 && length($key) > 0 ) {
			if ($first_time == 1) { $sql .= ","; } else { $first_time = 1; }
			$sql .= "(null, " . $dbh->quote(trim($key)) . ", null, null)";
		}
		
		if  ( (++$i % 50000) == 0 ) {
			print "Inserted " . ($i+1) . " records\n\033[1A";
			$whole_sql = "INSERT IGNORE INTO Elections VALUES " . $sql;
			$QueryDB = $dbhVoters->prepare($whole_sql);
			$QueryDB->execute();		
			$first_time = 0;
			$sql = "";
		}
				
	}
			
	if ( $first_time == 1) {		
		print "Doing a last insert\n";	
		$whole_sql = "INSERT IGNORE INTO Elections VALUES " . $sql;
		$QueryDB = $dbhVoters->prepare($whole_sql);
		$QueryDB->execute();		
	}
	
	print "Done inserting history into election table\n";		
}

### County is a class on it's own because the BOE Seems to use custom County IDs 
### and we need to record them.
sub BulkAddToCountyTable {
	my %Counties = @_;
	
	my $sql = "";
	my $first_time = 0;
	my $whole_sql = "";
	
	### Need to load the state IDs
	LoadResState();
		
	foreach my $State (keys %Counties ) {  
	 	my %CState = %{$Counties{$State}};
	 	foreach my $County (keys %CState) {	 		
 			if ($first_time == 1) { $sql .= ","; } else { $first_time = 1; }
			$sql .= "(null, " . $dbh->quote($CacheStateName { $State }) . "," . $dbh->quote(NameCase ($County)) . "," . $dbh->quote($Counties{$State}{$County}) . ")";
	 	}
	}
	
	$whole_sql = "INSERT IGNORE INTO DataCounty VALUES " . $sql;
	my $QueryDB = $dbhVoters->prepare($whole_sql);
	$QueryDB->execute();		
}


sub EmptyDatabases {
	my $tblname = $_[0];
	my $number = $_[1];
	
	if ( length ($tblname) > 0 && $number > 0) {
		my $sql = "TRUNCATE " . $tblname;
		my $QueryDB = $dbhVoters->prepare($sql);
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

sub trim {
	my $str = $_[0];
	$str =~ s/^\s+|\s+$//g;
	return $str;
}

1;
