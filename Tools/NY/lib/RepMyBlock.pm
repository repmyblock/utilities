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
our %CacheDataDistrict = ();
our %CacheDataHouseDistrict = ();
our %CacheDataDistrictTown = ();
our %CacheCountyTranslation = ();


our %LastInsertID = ();

our @AddPoolLastNames = ();
our @AddPoolFirstNames = ();
our @AddPoolMiddleNames = ();
our @AddPoolCityName = ();
our @AddPoolStreetName = ();
our @AddPoolDistrictTown = ();

our @CacheIdxLastName;
our @CacheIdxFirstName;
our @CacheIdxMiddleName;

our @CacheIdxSuffix;
our @CacheIdxDOB;
our @CacheIdxCode;


our @CacheVoter_LastName;
our @CacheVoter_FirstName;
our @CacheVoter_MiddleName;

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

our @CacheDistrict_CountyCode; 
our @CacheDistrict_ElectDistr; 
our @CacheDistrict_LegisDistr; 
our @CacheDistrict_TownCity; 
our @CacheDistrict_Ward;
our @CacheDistrict_CongressDistr;
our @CacheDistrict_SenateDistr;
our @CacheDistrict_AssemblyDistr;

our @CacheDistrict_Election_District; 
our @CacheDistrict_Assembly_District;
our @CacheDistrict_Congress_District;
our @CacheDistrict_Council_District; 
our @CacheDistrict_Senate_District;
our @CacheDistrict_Civil_Court_District;
our @CacheDistrict_Judicial_District;
our @CacheDistrict_NYC_CountyID;

our %CacheVotersIndex = ();
our %CacheVoters = ();


our %CachePlainVoter = ();
our %CacheVoterHistory = ();

our %TemporalCacheGroupID = ();

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

sub InitLastInsertID {
	my $self = shift;
	
	my @TableToCheck = qw/Voters VotersIndexes DataFirstName DataLastName DataMiddleName DataAddress DataCity DataCounty DataHouse DataState DataStreet/;
	foreach ( @TableToCheck ) {
		$LastInsertID { $_ } = 	$self->FindMaxID($_);
		printf ("\t%-15s: %s\n", $_, $LastInsertID {$_} );
	}
}

sub ListCountsTables {
	my $self = shift;
	
	my @TableToCheck = qw/Voters VotersIndexes DataFirstName DataLastName DataMiddleName DataAddress DataCity DataCounty DataHouse DataState DataStreet/;
	foreach ( @TableToCheck ) {
		my $TableCount = 	$self->CountDBTables($_);
		printf ("\t%-15s: %s\n", $_, $TableCount );
	}
	
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
sub LoadLastNameCache { my $self = shift; $self->LoadCaches( \%CacheLastName, "DataLastName", 0, 0); }
sub LoadFirstNameCache { my $self = shift; $self->LoadCaches( \%CacheFirstName, "DataFirstName", 0, 0 ); }
sub LoadMiddleNameCache { my $self = shift; $self->LoadCaches( \%CacheMiddleName, "DataMiddleName", 0, 0); }

sub AddFirstName { my $self = shift; $self->AddToDatabase("DataFirstName", \@AddPoolFirstNames, \%CacheFirstName, 1); }
sub AddLastName { my $self = shift; $self->AddToDatabase("DataLastName", \@AddPoolLastNames, \%CacheLastName, 1); }
sub AddMiddleName { my $self = shift; $self->AddToDatabase("DataMiddleName", \@AddPoolMiddleNames, \%CacheMiddleName, 1); }

sub AddDataCity { my $self = shift; $self->AddToDatabase("DataCity", \@AddPoolCityName, \%CacheCityName, 0); }
sub AddDataStreet { my $self = shift; $self->AddToDatabase("DataStreet", \@AddPoolStreetName, \%CacheStreetName, 0); }
sub AddDistrictTown { my $self = shift; $self->AddToDatabase("DataDistrictTown", \@AddPoolDistrictTown, \%CacheDataDistrictTown, 0); }

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
sub LoadDistrictTown { my $self = shift; $self->LoadCaches( \%CacheDataDistrictTown, "DataDistrict", 0, 0); }

sub LoadUpdateNamesCache {
	my $self = shift;
	my $LimitCounter = shift;

	my %CacheRawLastName = (); my %CacheRawFirstName = (); my %CacheRawMiddleName = ();
	$CounterLastName = 0;	$CounterFirstName = 0; $CounterMiddleName = 0;
	
	my $stmt = $self->{"dbh"}->prepare($self->ReturnNamesQuery($LimitCounter));
	$stmt->execute() or die "Connection error: $DBI::errstr";

	while ( my @row = $stmt->fetchrow_array) {
		$self->LoadColumnIntoCache($row[1], \%CacheRawLastName, \@AddPoolLastNames, $CounterLastName, \%CacheLastName);
		$self->LoadColumnIntoCache($row[2], \%CacheRawFirstName, \@AddPoolFirstNames, $CounterFirstName, \%CacheFirstName);
		$self->LoadColumnIntoCache($row[3], \%CacheRawMiddleName, \@AddPoolMiddleNames, $CounterMiddleName, \%CacheMiddleName);
	}
}

sub FindMaxID() {
	my $self = shift;
	
	my $sql = "SELECT MAX(" . $_[0] . "_ID) FROM " . $_[0];
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute() or die "Connection error: $DBI::errstr";
	my @row = $QueryDB->fetchrow_array;	
	
	if ( ! defined $row[0] ) {	return 0;	}
	return $row[0];
}

sub CountDBTables() {
	my $self = shift;
	
	my $sql = "SELECT COUNT(" . $_[0] . "_ID) FROM " . $_[0];
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute() or die "Connection error: $DBI::errstr";
	my @row = $QueryDB->fetchrow_array;	
	
	if ( ! defined $row[0] ) {	return 0;	}
	return $row[0];
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
		if (!(${$_[4]}{$_[0]})) {
			if (!${$_[1]}{$_[0]}) {
				${$_[2]}[($_[3]++)] = $_[0];
				${$_[1]}{$_[0]} = 1;
			}
		}
	}
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
	$QueryDB->execute() or die "Connection error: $DBI::errstr";
	
	for (my $i = 0; $i < $QueryDB->rows; $i++) {
		my @row = $QueryDB->fetchrow_array;	
		if ( defined $row[1]) {
			${$_[0]}{ $self->trimstring($row[1])} = $self->trimstring($row[0]);
		}
	}	
	
	$self->{"LoadCache"}{ $_[1] } = 'yes';
	
	my $clock_inside1 = clock();
	printf ("\tLoaded cache for %-15sin %-20s seconds\n", $_[1], ($clock_inside1 - $clock_inside0));
}



sub LoadDataHouseDistrictCaches {
	my $self = shift;
	my $clock_inside0 = clock();
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
	
	my $firsttime = 0;
	my $sql = "SELECT DataHouse.DataHouse_ID, DataDistrictTemporal_GroupID, CountyCode, ElectDistr, LegisDistr, " .
						"Ward, CongressDistr, SenateDistr, AssemblyDistr, Council_District, Civil_Court_District, Judicial_District " . 
						"FROM RepMyBlock.DataHouse " .
						"LEFT JOIN Voters ON (Voters.DataHouse_ID = DataHouse.DataHouse_ID) " .
						"LEFT JOIN RawVoterFiles." . $_[0] . " ON (Voters.Voters_UniqStateVoterID = RawVoterFiles." . $_[0] . ".UniqNYSVoterID) " .
						"LEFT JOIN RawVoterFiles." . $_[1] . " ON (RawVoterFiles." . $_[0] . ".CountyVoterNumber = RawVoterFiles." . $_[1] . ".County_EMSID) " .
						"WHERE Voters_Status = 'Active' AND RawVoterFiles." . $_[0] . ".Status = 'ACTIVE' AND RawVoterFiles." . $_[0] . ".CountyVoterNumber IS NOT NULL";
						
	if ($_[2] > 0) {
		$sql .= " LIMIT " . $_[2];
	}
	
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute() or die "Connection error: $DBI::errstr";

	while (my @row = $QueryDB->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		$CacheDataHouseDistrict { $row[1] } { $row[2] } { $row[3] } { $row[4] } { $row[5] } { $row[6] } { $row[7] } { $row[8] } { $row[9] } { $row[10] } { $row[11] } = $row[0];
	}	
	
	my $clock_inside1 = clock();
	printf ("\tLoaded cache for %-15sin %-20s seconds (State ID: %s)\n", "LoadDataHouseDistrictCaches", ($clock_inside1 - $clock_inside0), $_[0]);
}

sub LoadDistrictCaches {
	my $self = shift;
	my $clock_inside0 = clock();
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
	
	my $firsttime = 0;
	my $sql = "SELECT DataDistrict_ID, DataCounty_ID, DataDistrict_Electoral, DataDistrict_StateAssembly, " .
						"DataDistrict_SenateSenate, DataDistrict_Legislative, DataDistrict_Ward, " . 
						"DataDistrict_Congress, DataDistrict_Council, DataDistrict_CivilCourt, DataDistrict_Judicial " .
						"FROM DataDistrict";
						
	
	
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute() or die "Connection error: $DBI::errstr";

	while (my @row = $QueryDB->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		$CacheDataDistrict { $row[1] } { $row[2] } { $row[3] } { $row[4] } { $row[5] } { $row[6] } { $row[7] } { $row[8] } { $row[9] } { $row[10] } = $row[0];
	}	
	
	my $clock_inside1 = clock();
	printf ("\tLoaded cache for %-15sin %-20s seconds (State ID: %s)\n", "District", ($clock_inside1 - $clock_inside0), $_[0]);
}
					

### This are the addresses

sub LoadAddressesCaches () {
	my $self = shift;
	my $clock_inside0 = clock();
	
	if ( $_[0] < 1 ) {
		print "Need to provide the State ID to load\n";
		exit();
	}
	
	my $sql = "SELECT DataAddress.DataAddress_ID, DataStreet_ID, DataCity_ID, DataAddress_HouseNumber, DataAddress_zipcode, DataAddress_zip4, " .
						"DataAddress_PreStreet, DataAddress_PostStreet, DataAddress_FracAddress, DataHouse.DataAddress_ID, DataHouse_ID, DataHouse_Apt " .
						"FROM DataAddress " . 
						"LEFT JOIN DataHouse ON (DataAddress.DataAddress_ID = DataHouse.DataAddress_ID) " .
						"WHERE DataState_ID = " . $_[0];
						
	my $InfoStr = "";						
	if ( $_[1] > 0 ) {
		$InfoStr = " from ID " . $_[1] . " ";
		$sql .= " AND DataAddress.DataAddress_ID > " . $_[1];
	}
						
  my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute() or die "Connection error: $DBI::errstr";

	while (my @row = $QueryDB->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		$CacheAddress	{ $row[1] } { $row[2] } { $row[3] } { $row[4] } { $row[5] } { $row[6] } { $row[7] } { $row[8] } = $row[0];
		if ( defined ($row[9])) { $CacheHouse { $row[9] } { $row[11] } = $row[10]; }
	}	
	
	my $clock_inside1 = clock();
	printf ("\tLoaded cache for %-15sin %-20s seconds (State ID: %s)\n", "DataAddress", ($clock_inside1 - $clock_inside0), $_[0]);

}

sub LoadHouseCaches() {
	my $self = shift;
	my $clock_inside0 = clock();
	
	
	my $sql = "SELECT DataHouse_ID, DataAddress_ID, DataHouse_Apt " .
						"FROM DataHouse ";
						
	my $InfoStr = "";						
	if ( $_[0] > 0 ) {
		$InfoStr = " from ID " . $_[0] . " ";
		$sql .= " WHERE DataHouse_ID > " . $_[0];
	}
									
	my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute() or die "Connection error: $DBI::errstr";	
	while (my @row = $QueryDB->fetchrow_array) {					
		$RepMyBlock::CacheHouse { $row[1] } { $row[2] } = $row[0];
	}
		
	my $clock_inside1 = clock();
	printf ("\tLoaded cache for %-15s in %s seconds (State ID: %s)\n", "DataHouse", ($clock_inside1 - $clock_inside0), $InfoStr);

}

sub LoadVotersCaches () {
	my $self = shift;
	my $clock_inside0 = clock();
	
	if ( $_[0] < 1 ) {
		print "Need to provide the State ID to load\n";
		exit();
	}
			
	my $sql = "SELECT VotersIndexes_ID, DataFirstName_ID, DataLastName_ID, DataMiddleName_ID, " . 
						"VotersIndexes_Suffix, VotersIndexes_DOB, VotersIndexes_UniqStateVoterID " .
						"FROM VotersIndexes WHERE DataState_ID = " . $_[0];
						
	my $InfoStr = "";						
	if ( $_[1] > 0 ) {
		$InfoStr = " from ID " . $_[1] . " ";
		$sql .= " AND VotersIndexes_ID > " . $_[1];
	}
												
  my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute() or die "Connection error: $DBI::errstr";

	while (my @row = $QueryDB->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		$CacheVotersIndex	{ $row[1] } { $row[2] } { $row[3] } { $row[4] } { $row[5] } { $row[6] }  = $row[0];
	}	
	
	my $clock_inside1 = clock();
	printf ("\tLoaded cache for %-15sin %s seconds (State ID: %s)\n", "VotersIndexes", ($clock_inside1 - $clock_inside0), $_[0]);

}

sub LoadCacheCountyTranslation {
	my $self = shift;
	my $clock_inside0 = clock();

	if ($DataStateID < 1) {
		print "The DataState is not defined ... \n";
		exit();
	}
	
	my $sql = "SELECT DataCounty_ID, DataCounty_BOEID FROM DataCounty WHERE DataState_ID = ?";											
  my $QueryDB = $self->{"dbh"}->prepare($sql);
	$QueryDB->execute($DataStateID) or die "Connection error: $DBI::errstr";

	while (my @row = $QueryDB->fetchrow_array) { #  or die "can't execute the query: $stmt->errstr" ) {
		$CacheCountyTranslation	{ $row[1] } = $row[0];
	}	
	
	my $clock_inside1 = clock();
	printf ("\tLoaded cache for %-15sin %s seconds (State ID: %s)\n", "DataCounty", ($clock_inside1 - $clock_inside0), $DataStateID);
}

sub DbAddToAddress {
	my $self = shift;
	my $clock_inside0 = clock();
	
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
								 		if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1;	}
								 									
										$sql .= "(";
										if ( length($ResHouseNumber) > 0 ) { $sql .= $self->{"dbh"}->quote($ResHouseNumber) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResFracAddress) > 0 ) { $sql .= $self->{"dbh"}->quote($ResFracAddress) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResPreStreet)> 0  ) { $sql .= $self->{"dbh"}->quote($ResPreStreet) . ", " } else { $sql .= "NULL,"; }
		 							  if ( length($ResStreetNameID) > 0  ) { $sql .= $self->{"dbh"}->quote($ResStreetNameID) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResPostStDir)> 0  ) { $sql .= $self->{"dbh"}->quote($ResPostStDir) . ", " } else { $sql .= "NULL,"; }
										if ( length($ResCityNameID) > 0  ) { $sql .= $self->{"dbh"}->quote($ResCityNameID) . ", " } else { $sql .= "NULL,"; }
										if ( length($DataStateID) > 0  ) { $sql .= $self->{"dbh"}->quote($DataStateID) . ", " } else { $sql .= "NULL,"; }
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
		$QueryDB->execute() or die "Connection error: $DBI::errstr";
	} 
	
	$self->LoadAddressesCaches($DataStateID, $LastInsertID { "DataHouse" } );
	$LastInsertID { "DataAddress" } = $self->FindMaxID("DataAddress");	
	my $clock_inside1 = clock();
	print "\tWrote table DataAddress in " . ($clock_inside1 - $clock_inside0) . " seconds and last ID is " . $LastInsertID { "DataAddress" } . "\n\n";

}

sub DbAddToDataDistrict {
	my $self = shift;
	my $clock_inside0 = clock();
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
	
	my $firsttime = 0;
	my $sql = "INSERT INTO DataDistrict (DataCounty_ID, " . 
																			"DataDistrict_Electoral, " . 
																			"DataDistrict_StateAssembly, DataDistrict_SenateSenate, DataDistrict_Legislative, DataDistrict_Ward, " . 
																			"DataDistrict_Congress, DataDistrict_Council, DataDistrict_CivilCourt, DataDistrict_Judicial " . 
																			") VALUES ";

	foreach my $DataDistrictCycle_ID (keys %CacheDataDistrict) {
		foreach my $DBTable (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}}) {
			foreach my $DBTableValue (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}}) {
				foreach my $DataCounty_ID (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}}) {
					foreach my $Electoral (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}}) {
						foreach my $StateAssembly (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}}) {
							foreach my $SenateSenate (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}}) {
								foreach my $Legis (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}{$SenateSenate}}) {
									foreach my $Ward (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}{$SenateSenate}{$Legis}}) {
										foreach my $Congress (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}{$SenateSenate}{$Legis}{$Ward}}) {
											foreach my $Council (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}{$SenateSenate}{$Legis}{$Ward}{$Congress}}) {
												foreach my $CivilCourt (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}{$SenateSenate}{$Legis}{$Ward}{$Congress}{$Council}}) {
													foreach my $Judicial (keys %{$CacheDataDistrict{$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}{$SenateSenate}{$Legis}{$Ward}{$Congress}{$Council}{$CivilCourt}}) {
						 			
												 		if ( $CacheDataDistrict {$DataDistrictCycle_ID}{$DBTable}{$DBTableValue}{$DataCounty_ID}{$Electoral}{$StateAssembly}{$SenateSenate}{$Legis}{$Ward}{$Congress}{$Council}{$CivilCourt}{$Judicial} < 1 ) {
													 		if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1;	}
													 									
															$sql .= "(";
															#if ( length($DataDistrictCycle_ID) > 0 ) { $sql .= $self->{"dbh"}->quote($DataDistrictCycle_ID) . ", " } else { $sql .= "NULL,"; }
															#if ( length($DBTable) > 0 ) { $sql .= $self->{"dbh"}->quote($DBTable) . ", " } else { $sql .= "NULL,"; }
															#if ( length($DBTableValue) > 0 ) { $sql .= $self->{"dbh"}->quote($DBTableValue) . ", " } else { $sql .= "NULL,"; }
															if ( length($DataCounty_ID) > 0 ) { $sql .= $self->{"dbh"}->quote($DataCounty_ID) . ", " } else { $sql .= "NULL,"; }
															if ( length($Electoral) > 0 ) { $sql .= $self->{"dbh"}->quote($Electoral) . ", " } else { $sql .= "NULL,"; }
															if ( length($StateAssembly) > 0 ) { $sql .= $self->{"dbh"}->quote($StateAssembly) . ", " } else { $sql .= "NULL,"; }
															if ( length($SenateSenate) > 0 ) { $sql .= $self->{"dbh"}->quote($SenateSenate) . ", " } else { $sql .= "NULL,"; }
															if ( length($Legis) > 0 & $Legis > 0) { $sql .= $self->{"dbh"}->quote($Legis) . ", " } else { $sql .= "NULL,"; }
															if ( length($Ward) > 0 ) { $sql .= $self->{"dbh"}->quote($Ward) . ", " } else { $sql .= "NULL,"; }
															if ( length($Congress) > 0 ) { $sql .= $self->{"dbh"}->quote($Congress) . ", " } else { $sql .= "NULL,"; }
															if ( length($Council) > 0 ) { $sql .= $self->{"dbh"}->quote($Council) . ", " } else { $sql .= "NULL,"; }
															if ( length($CivilCourt) > 0 ) { $sql .= $self->{"dbh"}->quote($CivilCourt) . ", " } else { $sql .= "NULL,"; }
															if ( length($Judicial) > 0 ) { $sql .= $self->{"dbh"}->quote($Judicial) } else { $sql .= "NULL"; }
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
		$QueryDB->execute() or die "Connection error: $DBI::errstr";
	}

	### This is for the end.
	#	$self->LoadHouseCaches($LastInsertID { "DataHouse" } );
	# $LastInsertID { "DataHouse" } = $self->FindMaxID("DataHouse");	
		
	my $clock_inside1 = clock();
	
	print "\tWrote table DataDistrict in " . ($clock_inside1 - $clock_inside0) . " seconds and last ID is " . 
				$LastInsertID { "DataDistrict" } . "\n\n";
	
}

sub DbAddToDataDistrictTemporal {
	my $self = shift;
	my $clock_inside0 = clock();
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
	
	my $firsttime = 0;
	my $sql = "INSERT INTO DataDistrictTemporal (" .
						"DataDistrictCycle_ID, DataDistrictTemporal_GroupID, DataDistrict_ID, " .
						"DataDistrictTemporal_DBTable, DataDistrictTemporal_DBTableValue" . 
						") VALUES ";

	foreach my $CycleID (keys %TemporalCacheGroupID) {
		foreach my $DataDistrictID (keys %{$TemporalCacheGroupID{$CycleID}}) {
			foreach my $HouseID (keys %{$TemporalCacheGroupID{$CycleID}{$DataDistrictID}}) {
				foreach my $DBTable (keys %{$TemporalCacheGroupID{$CycleID}{$DataDistrictID}{$HouseID}}) {
					foreach my $DBTableValue (keys %{$TemporalCacheGroupID{$CycleID}{$DataDistrictID}{$HouseID}{$DBTable}}) {
					
				 		if ( $TemporalCacheGroupID {$CycleID}{$DataDistrictID}{$HouseID}{$DBTable}{$DBTableValue} > 0 ) {
					 		if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1;	}
					 									
							$sql .= "(";
							if ( length($CycleID) > 0 ) { $sql .= $self->{"dbh"}->quote($CycleID) . ", " } else { $sql .= "NULL,"; }
							if ( length($TemporalCacheGroupID {$CycleID}{$DataDistrictID}{$HouseID}{$DBTable}{$DBTableValue} ) > 0 ) { $sql .= $self->{"dbh"}->quote($TemporalCacheGroupID {$CycleID}{$DataDistrictID}{$HouseID}{$DBTable}{$DBTableValue}) . ", " } else { $sql .= "NULL,"; }
							if ( length($DataDistrictID) > 0 ) { $sql .= $self->{"dbh"}->quote($DataDistrictID) . ", " } else { $sql .= "NULL,"; }
							if ( length($DBTable) > 0 ) { $sql .= $self->{"dbh"}->quote($DBTable) . ", " } else { $sql .= "NULL,"; }
							if ( length($DBTableValue) > 0 ) { $sql .= $self->{"dbh"}->quote($DBTableValue) } else { $sql .= "NULL"; }
							$sql .= ")";
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
		$QueryDB->execute() or die "Connection error: $DBI::errstr";
	}

	### This is for the end.
	#$$self->LoadHouseCaches($LastInsertID { "DataDistrictTemporal_ID" } );
	$LastInsertID { "DataDistrictTemporal_ID" } = $self->FindMaxID("DataDistrictTemporal");	
		
	my $clock_inside1 = clock();
	
	print "\tWrote table DataDistrictTemporal in " . ($clock_inside1 - $clock_inside0) . " seconds and last ID is " . 
				$LastInsertID { "DataDistrictTemporal_ID" } . "\n\n";
	
}

sub DbAddToDataHouse {
	my $self = shift;
	my $clock_inside0 = clock();
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
	
	my $firsttime = 0;
	my $sql = "INSERT INTO DataHouse (DataAddress_ID, DataHouse_Apt) VALUES ";

	foreach my $DataAddress_ID (keys %CacheHouse) {
		foreach my $DataHouse_Apt (keys %{$CacheHouse{$DataAddress_ID}}) {
			
	 		if ( $CacheHouse { $DataAddress_ID } {$DataHouse_Apt } < 1 ) {
				if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1; }	
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
		$QueryDB->execute() or die "Connection error: $DBI::errstr";
	} 

	$self->LoadHouseCaches($LastInsertID { "DataHouse" } );
	$LastInsertID { "DataHouse" } = $self->FindMaxID("DataHouse");	
	
	my $clock_inside1 = clock();
	
	print "\tWrote table DataHouse in " . ($clock_inside1 - $clock_inside0) . " seconds and last ID is " . 
				$LastInsertID { "DataHouse" } . "\n\n";
}


	

sub DBUpdateDataHouseDB {
	my $self = shift;
	my $clock_inside0 = clock();
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
	
	if ( ! defined $self->{"dbh"} ) {
		print "Database not defined\n";	exit();
	}

	my $Counter = 0;

	if ($DataStateID > 0) {
		$self->LoadHouseCaches($DataStateID);
	} else {
		print "The DataState is not defined ... \n";
		exit();
	}

	### Need to modified the Cache for my use.
	my %LocalDataAddress = ();
	foreach my $HouseDataAddressID (keys %CacheHouse) {
		foreach my $HouseDataApt (keys %{$CacheHouse{$HouseDataAddressID}}) {
			$LocalDataAddress {$CacheHouse{$HouseDataAddressID}{$HouseDataApt}} = $HouseDataAddressID;
		}
	}

	my $sql = "UPDATE DataHouse SET DataDistrictTemporal_GroupID = ? WHERE DataAddress_ID = ?";
  my $QueryDB = $self->{"dbh"}->prepare($sql);
  
	foreach my $CycleID (keys %TemporalCacheGroupID) {
		foreach my $DataDistrictID (keys %{$TemporalCacheGroupID{$CycleID}}) {
			foreach my $HouseID (keys %{$TemporalCacheGroupID{$CycleID}{$DataDistrictID}}) {
				foreach my $DBTable (keys %{$TemporalCacheGroupID{$CycleID}{$DataDistrictID}{$HouseID}}) {
					foreach my $DBTableValue (keys %{$TemporalCacheGroupID{$CycleID}{$DataDistrictID}{$HouseID}{$DBTable}}) {
				 		if ( $TemporalCacheGroupID {$CycleID}{$DataDistrictID}{$HouseID}{$DBTable}{$DBTableValue} > 0 ) {
							$QueryDB->execute( $TemporalCacheGroupID {$CycleID}{$DataDistrictID}{$HouseID}{$DBTable}{$DBTableValue}, $LocalDataAddress{$HouseID} ) or die "Connection error: $DBI::errstr";
							if ($Counter % 10000 == 0 ) { print "Done updating $Counter districts\n\033[1A"; }
							$Counter++;
						}
					}
				}
			}
		}
	}

	my $clock_inside1 = clock();
	print "\tUpdate table DataHouse with DataTemporal in " . ($clock_inside1 - $clock_inside0) . " seconds\n\n";
}

sub DbAddToVotersIndex {
	my $self = shift;
	my $clock_inside0 = clock();
	
	my $firsttime = 0;
	my $sql = "INSERT INTO VotersIndexes " .
						"(DataState_ID, DataLastName_ID, DataFirstName_ID, DataMiddleName_ID, VotersIndexes_Suffix, " . 
						"VotersIndexes_DOB, VotersIndexes_UniqStateVoterID) VALUES ";
	
	foreach my $FirstNameID (keys %CacheVotersIndex) {
		foreach my $LastNameID (keys %{$CacheVotersIndex{$FirstNameID}}) {
			foreach my $MiddleNameID (keys %{$CacheVotersIndex{$FirstNameID}{$LastNameID}}) {
				foreach my $Suffix (keys %{$CacheVotersIndex{$FirstNameID}{$LastNameID}{$MiddleNameID}}) {
					foreach my $DOB (keys %{$CacheVotersIndex{$FirstNameID}{$LastNameID}{$MiddleNameID}{$Suffix}}) {
						foreach my $UniqStateVoterID (keys %{$CacheVotersIndex{$FirstNameID}{$LastNameID}{$MiddleNameID}{$Suffix}{$DOB}}) {
						
					 		if ( $CacheVotersIndex { $FirstNameID } { $LastNameID } { $MiddleNameID } {$Suffix } {$DOB } {$UniqStateVoterID } < 1 ) {
						 		if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1;	}
						 									
								$sql .= "(" . $self->{"dbh"}->quote($DataStateID) . ",";
								if ( length($LastNameID) > 0 ) { $sql .= $self->{"dbh"}->quote($LastNameID) . ", " } else { $sql .= "NULL,"; }
								if ( length($FirstNameID) > 0 ) { $sql .= $self->{"dbh"}->quote($FirstNameID) . ", " } else { $sql .= "NULL,"; }
								if ( length($MiddleNameID)> 0  ) { $sql .= $self->{"dbh"}->quote($MiddleNameID) . ", " } else { $sql .= "NULL,"; }
							  if ( length($Suffix) > 0  ) { $sql .= $self->{"dbh"}->quote($Suffix) . ", " } else { $sql .= "NULL,"; }
								if ( length($DOB)> 0  ) { $sql .= $self->{"dbh"}->quote($DOB) . ", " } else { $sql .= "NULL,"; }
								if ( length($UniqStateVoterID) > 0  ) { $sql .= $self->{"dbh"}->quote($UniqStateVoterID) } else { $sql .= "NULL"; }
								$sql .= ")";
										
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
		$QueryDB->execute() or die "Connection error: $DBI::errstr";
	} 
	
	$self->LoadVotersCaches($DataStateID, $LastInsertID { "VotersIndexes" } );
	$LastInsertID { "VotersIndexes" } = $self->FindMaxID("VotersIndexes");	
	my $clock_inside1 = clock();
	print "\tWrote table VotersIndexes in " . ($clock_inside1 - $clock_inside0) . " seconds and last ID is " . $LastInsertID { "VotersIndexes" } . 
	"\n\n";
	
	
}

sub DbAddToVoters {
	my $self = shift;
	my $clock_inside0 = clock();
		
	
	my $firsttime = 0;
	my $sql = "INSERT INTO Voters " .
						"(DataState_ID, ElectionsDistricts_DBTable, VotersIndexes_ID, ElectionsDistricts_DBTableValue, DataHouse_ID, " .
						"Voters_Gender, VotersComplementInfo_ID, Voters_UniqStateVoterID,  Voters_RegParty, Voters_ReasonCode, " .
						"Voters_Status, VotersMailingAddress_ID, Voters_IDRequired, Voters_IDMet, Voters_ApplyDate, Voters_RegSource, Voters_DateInactive, " . 
						"Voters_DatePurged, Voters_CountyVoterNumber, Voters_RecFirstSeen, Voters_RecLastSeen) VALUES ";
		
	foreach my $DBTableValue (keys %CacheVoters) {
		foreach my $Gender (keys %{$CacheVoters{$DBTableValue}}) {
			foreach my $CacheVoterIndex (keys %{$CacheVoters{$DBTableValue}{$Gender}}) {
				foreach my $CacheDataHouse (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}}) {
					foreach my $EnrollPolParty (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}}) {
						foreach my $ReasonCode (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}}) {
							foreach my $Status (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}}) {
								foreach my $IDRequired (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}{$Status}}) {
									foreach my $IDMet (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}{$Status}{$IDRequired}}) {
										foreach my $ApplicationSource (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}{$Status}{$IDRequired}{$IDMet}}) {
											foreach my $VoterMadeInactive (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}{$Status}{$IDRequired}{$IDMet}{$ApplicationSource}}) {
												foreach my $VoterPurged (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}{$Status}{$IDRequired}{$IDMet}{$ApplicationSource}{$VoterMadeInactive}}) {
													foreach my $CountyVoterNumber (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}{$Status}{$IDRequired}{$IDMet}{$ApplicationSource}{$VoterMadeInactive}{$VoterPurged}}) {
														foreach my $UniqStateVoterID (keys %{$CacheVoters{$DBTableValue}{$Gender}{$CacheVoterIndex}{$CacheDataHouse}{$EnrollPolParty}{$ReasonCode}{$Status}{$IDRequired}{$IDMet}{$ApplicationSource}{$VoterMadeInactive}{$VoterPurged}{$CountyVoterNumber}}) {
													 		if ( $firsttime == 1) {	$sql .= ","; } else { $firsttime = 1;	}
														 									
															$sql .= "(" . $self->{"dbh"}->quote($DataStateID) . ",";
															if ( length($DBTableValue) > 0) { $sql .= $self->{"dbh"}->quote($DBTableName) . ","; } else { $sql .= "NULL,"; }
															if ( length($CacheVoterIndex) > 0  ) { $sql .= $self->{"dbh"}->quote($CacheVoterIndex) . ","; } else { $sql .= "NULL,"; }
															if ( length($DBTableValue) > 0  ) { $sql .= $self->{"dbh"}->quote($DBTableValue) . ","; } else { $sql .= "NULL,"; }
															if ( length($CacheDataHouse) > 0  ) { $sql .= $self->{"dbh"}->quote($CacheDataHouse) . ","; } else { $sql .= "NULL,"; }
															if ( length($Gender) > 0  ) { $sql .= $self->{"dbh"}->quote($Gender) . ","; } else { $sql .= "NULL,"; }
															$sql .= "NULL, ";
															if ( length($UniqStateVoterID) > 0  ) { $sql .= $self->{"dbh"}->quote($UniqStateVoterID) . ","; } else { $sql .= "NULL,"; }
															if ( length($EnrollPolParty) > 0  ) { $sql .= $self->{"dbh"}->quote($EnrollPolParty) . ","; } else { $sql .= "NULL,"; }
															if ( length($ReasonCode) > 0  ) { $sql .= $self->{"dbh"}->quote($ReasonCode) . ","; } else { $sql .= "NULL,"; }
															if ( length($Status) > 0  ) { $sql .= $self->{"dbh"}->quote($Status) . ","; } else { $sql .= "NULL,"; }
															$sql .= "NULL, ";
															if ( length($IDRequired) > 0  ) { $sql .= $self->{"dbh"}->quote($IDRequired) . ","; } else { $sql .= "NULL,"; }										
															if ( length($IDMet) > 0  ) { $sql .= $self->{"dbh"}->quote($IDMet) . ","; } else { $sql .= "NULL,"; }									
															$sql .= "NULL, ";
															if ( length($ApplicationSource) > 0  ) { $sql .= $self->{"dbh"}->quote($ApplicationSource) . ","; } else { $sql .= "NULL,"; }
															if ( length($VoterMadeInactive) > 0  ) { $sql .= $self->{"dbh"}->quote($VoterMadeInactive) . ","; } else { $sql .= "NULL,"; }
															if ( length($VoterPurged) > 0  ) { $sql .= $self->{"dbh"}->quote($VoterPurged) . ","; } else { $sql .= "NULL,"; }
															if ( length($CountyVoterNumber) > 0  ) { $sql .= $self->{"dbh"}->quote($CountyVoterNumber) . ","; } else { $sql .= "NULL,"; }
															$sql .= "NOW(), NOW()";
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
		$QueryDB->execute() or die "Connection error: $DBI::errstr";
	} 
	
	undef %CacheVoters;
	undef %CacheVotersIndex;
		
	#$self->LoadVotersCaches($DataStateID, $LastInsertID { "Voters" } );
	#$LastInsertID { "Voters" } = $self->FindMaxID("Voters");	
	my $clock_inside1 = clock();
	print "\tWrote table VotersData in " . ($clock_inside1 - $clock_inside0) . " seconds and last ID is " . $LastInsertID { "VotersIndexes" } . 
				"\n\n";
}

sub AddToDatabase {
	my $self = shift;
	
	my $clock_inside0 = clock();	
	# To speed it up I removed the variable name.
	# $Name = $_[0]; @Pool = @{$_[1]}; $rhOptions = $_[2], $AddCompress = $_[3], 
	#print "Received " . $_[0] . " counter: " . scalar @{$_[1]} . "\n";

	 my $QueryDB;

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
	  
	  $self->LoadCaches($_[2], $_[0], $LastInsertID { $_[0] }, 0);	
	  $LastInsertID {  $_[0] } = $self->FindMaxID( $_[0]);	
	}

	my $clock_inside1 = clock();
	print "\tWrote table " . $_[0] . " in " . ($clock_inside1 - $clock_inside0) . " seconds and last ID is " . 
				$LastInsertID { $_[0] } . "\n\n";
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

sub CreateTemporalCache() {
	
	print Dumper(%TemporalCacheGroupID);
	
	
	
	
}


sub UpdateDataHouseWithTemporal() {
	
	my $Counter = 0;
	## our %TemporalCacheGroupID = ();
	### Search if the Temporal in question exist
	#SELECT max(DataDistrictTemporal_GroupID) FROM RepMyBlock.DataDistrictTemporal;
	#SELECT * FROM DataDistrictTemporal WHERE DataDistrict_ID = XXX AND DataDistrictCycle_ID = XXX

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


sub ParseDatesToDB {
	my $self = shift;
	my $str = shift;
	
	if (defined $str) { 
		$str =~ /(.{4})(.{2})(.{2})/; 
		return $1 . "-" . $2 . "-" . $3;
	}
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
