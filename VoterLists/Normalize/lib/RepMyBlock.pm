#!/usr/bin/perl

# At this time the assumption is that before you load each table,
# the cache is correct and not missing, otherwise the data might
# end up with whole.

# The order is Names first, then City and Street, then the rest of the address.

package RepMyBlock;

use strict;
#use warnings;
use Config::Simple;
use DateTime::Format::MySQL;
use Lingua::EN::NameCase 'NameCase' ;
use Data::Dumper;
use Time::HiRes qw ( clock );

# Data to be automated later.
our $DataStateID = undef;
our $DBTableName = undef;

our %CacheLastName = ();
our $CounterLastName = 0;

our %CacheFirstName = (); 
our $CounterFirstName = 0;

our %CacheMiddleName = ();
our $CounterMiddleName = 0;
 
our %CacheNYCVoterID = ();

our %CacheCityName = ();
our %CacheStreetName = ();
our %CacheAddress = ();
our %CacheHouse = ();



our %CacheStateName = ();
our %CacheCountyName = ();

our @AddPoolLastNames = ();
our @AddPoolFirstNames = ();
our @AddPoolMiddleNames = ();
our @AddPoolCityName = ();
our @AddPoolStreetName = ();

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

our @CacheAdress_ResHouseNumber;
our @CacheAdress_ResFracAddress; 
our @CacheAdress_ResApartment;
our @CacheAdress_ResPreStreet; 
our @CacheAdress_ResStreetName; 
our @CacheAdress_ResPostStDir; 
our @CacheAdress_ResCity; 
our @CacheAdress_ResZip; 
our @CacheAdress_ResZip4;

our @CacheAdress_ResStreetNameID;
our @CacheAdress_ResCityNameID;

our %CachePlainVoter = ();
our %CacheVoterHistory = ();


our $DateTable;
our $dbhRawVoters;	
our $dbhVoters;
our $dbh;

sub new {
	my $class = shift; # defining shift in $myclass 
  my $self = {}; # the hashed reference 
     
  return bless $self, $class; 
}

sub SetDatabase {
	my $self = shift;
	my $dbh = shift;
	$self->{"dbh"} = $dbh;
}

sub InitializeVoterFile {
	my $self = shift;
	my $dbh = shift;
		
	# Read the Table Directory in the file
	my $filename = '/home/usracct/.rmbvoter';
	open(my $fh, '<:encoding(UTF-8)', $filename) or die "Could not open file '$filename' $!";
	my $tabledate = <$fh>;
	chomp($tabledate);
	close($fh);
	
	$self->{"tabledate"} = $tabledate;	
	return $tabledate;
}

sub InitDatabase {
	my $self = shift;
	my $params = shift;
	
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
	$self->{"dbh"} = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
	
	return $self->{"dbh"};
}

sub SetVoterState {
	my $self = shift;

	# I need to check that I have everything for the state later on.
	# $DBTableName = "NY";
	$DataStateID = $_[0];
}


sub InitNamesCaches {
	my $self = shift;

	$self->LoadLastNameCache();
	$self->LoadFirstNameCache();
	$self->LoadMiddleNameCache();
}

sub InitStreetCaches() {
	my $self = shift;

	$self->LoadResStreetName();
	$self->LoadResCity();
	$self->LoadResState();
	$self->LoadAddressesCaches($DataStateID);
}

sub AddNamesCacheIntoDatabase{
	my $self = shift;
		
	$self->AddLastName();
	$self->AddFirstName();
	$self->AddMiddleName();
}

sub AddAddressCacheIntoDatabase{
	my $self = shift;
	
	$self->AddDataStreet();
	$self->AddDataCity();
	
}

### Load Cache
sub LoadLastNameCache { my $self = shift; $self->LoadCaches( \%CacheLastName, "VotersLastName", 0, 0); }
sub LoadFirstNameCache { my $self = shift; $self->LoadCaches( \%CacheFirstName, "VotersFirstName", 0, 0 ); }
sub LoadMiddleNameCache { my $self = shift; $self->LoadCaches( \%CacheMiddleName, "VotersMiddleName", 0, 0); }

sub AddFirstName { my $self = shift; $self->AddToDatabase("VotersFirstName", \@AddPoolFirstNames, \%CacheFirstName, 1); }
sub AddLastName { my $self = shift; $self->AddToDatabase("VotersLastName", \@AddPoolLastNames, \%CacheLastName, 1); }
sub AddMiddleName { my $self = shift; $self->AddToDatabase("VotersMiddleName", \@AddPoolMiddleNames, \%CacheMiddleName, 1); }

sub AddDataCity { my $self = shift; $self->AddToDatabase("DataCity", \@AddPoolCityName, \%CacheCityName, 0); }
sub AddDataStreet { my $self = shift; $self->AddToDatabase("DataStreet", \@AddPoolStreetName, \%CacheStreetName, 0); }

sub PrintLastName { my $self = shift; my $answer = shift; return $CacheLastName {$answer}; }
sub PrintFirstName { my $self = shift; my $answer = shift; return $CacheFirstName {$answer}; }
sub PrintMiddleName { my $self = shift; my $answer = shift; return $CacheMiddleName {$answer}; }

sub PrintAll_VotersLastName { my $self = shift; print "\nLast Names\n"; $self->PrintAll_Voters(\%CacheLastName); }
sub PrintAll_VotersFirstName { my $self = shift; print "\nFirst Names\n"; $self->PrintAll_Voters(\%CacheFirstName); }
sub PrintAll_VotersMiddleName { my $self = shift; print "\nMiddle Names\n"; $self->PrintAll_Voters(\%CacheMiddleName); }

sub LoadIndexCache { my $self = shift; $self->LoadCaches( \%CacheNYCVoterID, "VotersIndexes", 0, "VotersIndexes_UniqStateVoterID"); }
sub LoadHistoryCache { my $self = shift; $self->LoadCaches( \%CacheVoterHistory, "Elections", 0, 0); }
sub LoadResStreetName { my $self = shift; $self->LoadCaches( \%CacheStreetName, "DataStreet", 0, 0); }
sub LoadResCity { my $self = shift; $self->LoadCaches( \%CacheCityName, "DataCity", 0, 0); }
sub LoadResState { my $self = shift; $self->LoadCaches( \%CacheStateName, "DataState", 0, 0); }

sub LoadUpdateNamesCache {
	my $self = shift;
	my $LimitCounter = shift;

	my %CacheRawLastName = (); my %CacheRawFirstName = (); my %CacheRawMiddleName = ();
	$CounterLastName = 0;	$CounterFirstName = 0; $CounterMiddleName = 0;
	
	my $stmt = $self->{"dbh"}->prepare($self->ReturnNamesQuery($LimitCounter));
	$stmt->execute();

	while ( my @row = $stmt->fetchrow_array) {
		$self->LoadColumnIntoCache($row[1], \%CacheRawLastName, \@AddPoolLastNames, $CounterLastName, \%CacheLastName);
		$self->LoadColumnIntoCache($row[2], \%CacheRawFirstName, \@AddPoolFirstNames, $CounterFirstName, \%CacheFirstName);
		$self->LoadColumnIntoCache($row[3], \%CacheRawMiddleName, \@AddPoolMiddleNames, $CounterMiddleName, \%CacheMiddleName);
	}
}

sub PrintAll_Voters() {
	my $self = shift;
	
	# CacheRaw = %{$_[0]};
	foreach my $key (keys %{$_[0]} ) {		
		print $key . "\n";
	}
}

sub LoadColumnIntoCache {
	my $self = shift;
	# $row[] = $_[0]; %CacheRaw = %{$_[1]}; @AddPoolVar = @{$_[2]}; $Counter = $_[3]; %CacheName = %{$_[4]};
	
	if (defined ($_[0])) {
		#print "\nCacheName[" . $_[0] . "]: ";
		#if (defined(${$_[1]}{$_[0]})) { print ${$_[1]}{$_[0]}; } else { print "not defined.";}
		#print "\t%CacheName{" . $_[0] . "}: ";
		#if (defined(${$_[4]}{$_[0]})) { print ${$_[4]}{$_[0]}; } else { print "not defined.";}
		#print "\n";
		
		if (!(${$_[4]}{$_[0]})) {
			if (!${$_[1]}{$_[0]}) {
				${$_[2]}[($_[3]++)] = $_[0];
				${$_[1]}{$_[0]} = 1;
			}
		}
	}
	
	# my $TotalCount = @{$_[2]};
	 #print "Size of pool: " . $TotalCount . "\n";
	 #for (my $i = 0; $i < $TotalCount; $i++) {
	# 	print "Pool[$i]: " . ${$_[2]}[$i] . "\n";
	# }
}
		
sub LoadCaches {
	my $self = shift;
		
	my $clock_inside0 = clock();
	
	# %CacheLastNam = %{$_[0]}; $tblname = $_[1]; $tblid = $_[2]; $colcheck = $_[3]	
	my $Col = "*";

	if ( $_[3] ) { $Col = $_[1] . "_ID, " . $_[3];	}
	my $sql = "SELECT $Col FROM " . $_[1];
	if ( $_[3] > 0) {
		$sql .= " WHERE " . $_[1] . "_ID >= " . $self->{"dbh"}->quote($_[3]);
	}

	if ( ! defined $self->{"dbh"} ) {
		print "Database not defined\n";	exit();
	}
	
  my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute();
	
	for (my $i = 0; $i < $QueryDB->rows; $i++) {
		my @row = $QueryDB->fetchrow_array;	
		if ( defined $row[1]) {
			${$_[0]}{ $self->trimstring($row[1])} = $self->trimstring($row[0]);
		}
	}	

	$self->{"LoadCache"}{ $_[1] } = 'yes';
	
	my $clock_inside1 = clock();
	print "Loaded cache for " . $_[1] . " in " . ($clock_inside1 - $clock_inside0) . " seconds\n";
	
}


### This are the addresses

sub LoadAddressesCaches () {
	my $self = shift;
	
	my $clock_inside0 = clock();
	
	if ( $_[0] < 1 ) {
		print "Need to provide the State ID to load\n";
		exit();
	}
	
	my $sql = "SELECT DataAddress_ID, DataStreet_ID, DataCity_ID, DataAddress_HouseNumber, DataAddress_zipcode, DataAddress_zip4, " .
						"DataAddress_PreStreet, DataAddress_PostStreet, DataAddress_FracAddress FROM DataAddress WHERE DataState_ID = " . $_[0];
  my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute();

	while (my @row = $QueryDB->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		$RepMyBlock::CacheAddress	{ $row[1] } { $row[2] } { $row[3] } { $row[4] } { $row[5] } { $row[6] } { $row[7] } { $row[8] } = $row[0];
	}	
	
	my $clock_inside1 = clock();
	print "Loaded cache for DataAddress in StateID " . $_[0] . " in " . ($clock_inside1 - $clock_inside0) . " seconds\n";
}

sub AddAddressToDatabase {
	my $self = shift;
	
	my $clock_inside0 = clock();
	
	my $QueryDB = $self->{"dbh"}->prepare("SELECT AUTO_INCREMENT FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = ? AND TABLE_NAME = ?");
	$QueryDB->execute("VoterData", "DataAddress");
	my @row = $QueryDB->fetchrow_array;	
	my $LastInsertID = $row[0];
			
	print "Last Insert ID before adding more addresses: $LastInsertID \n";
	
	my $firsttime = 0;
	my $sql = "INSERT INTO DataAddress " .
						"(DataAddress_HouseNumber, DataAddress_FracAddress, DataAddress_PreStreet, DataStreet_ID, DataAddress_PostStreet, " . 
						"DataCity_ID, DataState_ID, DataAddress_zipcode, DataAddress_zip4) VALUES ";
	
	foreach my $ResStreetNameID (keys %CacheAddress) {
		foreach my $ResCityNameID (keys %{$CacheAddress{$ResStreetNameID}}) {
			foreach my $ResHouseNumber (keys %{$CacheAddress{$ResStreetNameID}{$ResCityNameID}}) {
				foreach my $ResZip (keys %{$CacheAddress{$ResStreetNameID}{$ResCityNameID}{$ResHouseNumber}}) {
					foreach my $ResZip4 (keys %{$CacheAddress{$ResStreetNameID}{$ResCityNameID}{$ResHouseNumber}{$ResZip}}) {
						foreach my $ResPreStreet (keys %{$CacheAddress{$ResStreetNameID}{$ResCityNameID}{$ResHouseNumber}{$ResZip}{$ResZip4}}) {
							foreach my $ResPostStDir (keys %{$CacheAddress{$ResStreetNameID}{$ResCityNameID}{$ResHouseNumber}{$ResZip}{$ResZip4}{$ResPreStreet}}) {
								foreach my $ResFracAddress (keys %{$CacheAddress{$ResStreetNameID}{$ResCityNameID}{$ResHouseNumber}{$ResZip}{$ResZip4}{$ResPreStreet}{$ResPostStDir}}) {
							 		
							 		if ( $CacheAddress { $ResStreetNameID } {$ResCityNameID } {$ResHouseNumber } {$ResZip } {$ResZip4 } {$ResPreStreet } {$ResPostStDir } {$ResFracAddress } < 1 ) {
								 		if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1;	print "Je suis dans le ADD Address Database\n";}
								 									
										$sql .= "(";
										if ( length($ResHouseNumber) > 0 ) { $sql .= $self->{"dbh"}->quote($ResHouseNumber) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResFracAddress) > 0 ) { $sql .= $self->{"dbh"}->quote($ResFracAddress) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResPreStreet)> 0  ) { $sql .= $self->{"dbh"}->quote($ResPreStreet) . ", " } else { $sql .= "NULL,"; }
		 							  if ( length($ResStreetNameID)> 0  ) { $sql .= $self->{"dbh"}->quote($ResStreetNameID) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResPostStDir)> 0  ) { $sql .= $self->{"dbh"}->quote($ResPostStDir) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResCityNameID)> 0  ) { $sql .= $self->{"dbh"}->quote($ResCityNameID) . ", " } else { $sql .= "NULL,"; }
										if ( length($DataStateID)> 0  ) { $sql .= $self->{"dbh"}->quote($DataStateID) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResZip)> 0  ) { $sql .= $self->{"dbh"}->quote($ResZip) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResZip4) > 0 ) { $sql .= $self->{"dbh"}->quote($ResZip4) } else { $sql .= "NULL"; }
										$sql .= ")";
										
									}
								}
							}
						}
					}
				}
			}
 	 	}
 	}
 	
 	if ( ! defined $self->{"dbh"} ) {
		print "Database not defined\n";	exit();
	}
	
	if ($firsttime > 0) {
	  my $QueryDB = $self->{"dbh"}->prepare($sql);
		$QueryDB->execute();
	} 
	
	my $clock_inside1 = clock();
	print "Wrote to database table DataAddress in " . ($clock_inside1 - $clock_inside0) . " seconds\n";
}

sub AddToDataHouse {
	my $self = shift;
	
	my $clock_inside0 = clock();
	
	# To speed it up I removed the variable name.
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
		
	my $QueryDB = $self->{"dbh"}->prepare("SELECT AUTO_INCREMENT FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = ? AND TABLE_NAME = ?");
	$QueryDB->execute("VoterData", "DataHouse");
	my @row = $QueryDB->fetchrow_array;	
	my $LastInsertID = $row[0];
			
	print "Last Insert ID before adding more addresses: $LastInsertID \n";
	
	my $firsttime = 0;
	my $sql = "INSERT INTO DataHouse (DataAddress_ID, DataHouse_Apt) VALUES ";

	foreach my $DataAddress_ID (keys %CacheHouse) {
		foreach my $DataHouse_Apt (keys %{$CacheAddress{$DataAddress_ID}}) {
	 		if ( $CacheHouse { $DataAddress_ID } {$DataHouse_Apt } < 1 ) {
				if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1;	print "Je suis dans le ADD Address Database\n";}	
				$sql .= "(";
				$sql .= $self->{"dbh"}->quote($DataAddress_ID) . ",";
				if ( length($DataHouse_Apt) > 0 ) { $sql .= $self->{"dbh"}->quote($DataHouse_Apt); } else { $sql .= "NULL"; }
				$sql .= ")";
			}
		}
	}
	

	if ( ! defined $self->{"dbh"} ) {
		print "Database not defined\n";	exit();
	}
	
	if ($firsttime > 0) {
	  my $QueryDB = $self->{"dbh"}->prepare($sql);
		$QueryDB->execute();
	} 
	
	my $clock_inside1 = clock();
	print "Wrote to database table DataHouse in " . ($clock_inside1 - $clock_inside0) . " seconds\n";
}


sub AddToDatabase {
	my $self = shift;
	
	my $clock_inside0 = clock();
	
	# To speed it up I removed the variable name.
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
		
	my $QueryDB = $self->{"dbh"}->prepare("SELECT AUTO_INCREMENT FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = ? AND TABLE_NAME = ?");
	$QueryDB->execute("VoterData", $_[0]);
	my @row = $QueryDB->fetchrow_array;	
	my $LastInsertID = $row[0];
			
	#print "Received " . $_[0] . " counter: " . scalar @{$_[1]} . "\n";

	if ( scalar @{$_[1]} > 0 ) {
	
		my $Counter = 0;
		my $sql = "INSERT IGNORE INTO " . $_[0] . " VALUES ";	
			
		while (@{$_[1]}) {
					
			if ($Counter > 0) { $sql .= ","; }
			if ( $_[3] == 1 ) {
				my $value = pop(@{$_[1]});
				$sql .= "(null," . $self->{"dbh"}->quote(NameCase($value)) . "," . 
													 $self->{"dbh"}->quote(ReturnCompressed($value)) . ")";												 
			} else {
				$sql .= "(null," . $self->{"dbh"}->quote(NameCase(pop(@{$_[1]}))) . ")";												 
			}   
			$Counter++;
			
			if (($Counter % 1000) == 0) { 	
				$QueryDB = $self->{"dbh"}->prepare($sql);
				$QueryDB->execute();		
				$Counter = 0;	
				$sql = "INSERT IGNORE INTO " . $_[0] . " VALUES ";	
			}	
	  }
	  
	  print "Finalizing the adding: $Counter rows\n";
	  if ($Counter > 0) {
	  	$QueryDB = $self->{"dbh"}->prepare($sql);
			$QueryDB->execute();		
	  }
	  
	  $self->LoadCaches($_[2], $_[0], $LastInsertID, 0);	
	}
	my $clock_inside1 = clock();
	print "Wrote to database table " . $_[0] . " in " . ($clock_inside1 - $clock_inside0) . " seconds\n";
}

sub DateDBID {
	my $stmtFindDatesID = $dbhRawVoters->prepare("SELECT Raw_Voter_Dates_ID FROM Raw_Voter_Dates WHERE Raw_Voter_Dates_Date = ?");
	$stmtFindDatesID->execute($DateTable);
	my @row = $stmtFindDatesID->fetchrow_array;
	
	return $row[0];
}

sub ReturnCompressed {
	my $CompressFieldName = $_[0];
	if (defined ($CompressFieldName)) { $CompressFieldName =~ tr/a-zA-Z//dc; }
	return $CompressFieldName;
}


sub EmptyDatabases {
	my $self = shift;
	my $tblname = shift;
	my $number = shift;
	
	my $sql = "";
	
	if ( length ($tblname) > 0 && ! defined $number) {
	
		$sql = "TRUNCATE " . $tblname;
		my $QueryDB = $self->{"dbh"}->prepare($sql);
		$QueryDB->execute();
	
	} else {

		$sql = "DELETE FROM " . $tblname . " ORDER BY " . $tblname . "_ID" . " DESC LIMIT " . $number;
		my $QueryDB = $self->{"dbh"}->prepare($sql);
		$QueryDB->execute();

		$sql = "ALTER TABLE " . $tblname . " AUTO_INCREMENT=" . $number;
		$QueryDB = $self->{"dbh"}->prepare($sql);
		$QueryDB->execute();		
	
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
			
			$sql .= "(null, " . 
									$dbh->quote($DBTableName) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_DBTableValue[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_Gender[$i]) . 
									",null," .
									$dbh->quote($RepMyBlock::CacheVoter_UniqStateVoterID[$i]) . "," . 
									$dbh->quote($RepMyBlock::DataStateID) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_EnrollPolParty[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_ReasonCode[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_Status[$i]) . 
									",null," . 
									$dbh->quote($RepMyBlock::CacheVoter_IDRequired[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_IDMet[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_RegistrationCharacter[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_ApplicationSource[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_VoterMadeInactive[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_VoterPurged[$i]) . "," . 
									$dbh->quote($RepMyBlock::CacheVoter_CountyVoterNumber[$i]) . "," . 
									$dbh->quote($DateTable) . "," .
									$dbh->quote($DateTable) . 
							")";
		
			if  ( (($i+1) % 75000) == 0 ) {
				print "Inserted " . ($i+1) . " records - Still " . ($ExistCounter - $i) . " to go\n\033[1A";
				$whole_sql = "INSERT INTO Voters VALUES " . $sql;
				
				#print $whole_sql . "\n";
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

sub trimstring {
	my $self = shift;
	my $str = shift;
	
	if (defined $str) { 
		$str =~ s/^\s+|\s+$//g; 
		$str =~ s/\s+/ /g;
	}
	return lc $str;
}

1;
