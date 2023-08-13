#!/usr/bin/perl

local $| = 1; # activate autoflush to immediately show the prompt

use strict;
use Text::CSV;
use DBI;
use Time::HiRes;
use Data::Dumper;
use Lingua::EN::NameCase;

my $Verbose = 0;
my $StaticDataState = "1";

my $dbname = "RepMyBlock";
my $dbhost = "data.theochino.us";
my $dbport = "3306";
my $dbuser = "usracct";
my $dbpass = "usracct";

my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
$dbh->{mysql_auto_reconnect} = 1;

my $LimitTxt = "";
if ( $ARGV[0] > 0 ) {
	$LimitTxt = " LIMIT " . $ARGV[0];
}

#### Add the query.
my $SmtmFirstName = $dbh->prepare("INSERT INTO DataFirstName SET DataFirstName_Text = ?, DataFirstName_Compress = ?");
my $SmtmMiddleName = $dbh->prepare("INSERT INTO DataMiddleName SET DataMiddleName_Text = ?, DataMiddleName_Compress = ?");
my $SmtmLastName  = $dbh->prepare("INSERT INTO DataLastName SET DataLastName_Text = ?, DataLastName_Compress = ?");
my $SmtmDataStreet = $dbh->prepare("INSERT INTO DataStreet SET DataStreet_Name = ?");
my $SmtmDataCity  = $dbh->prepare("INSERT INTO DataCity SET DataCity_Name = ?");
my $SmtmCounty  = $dbh->prepare("INSERT INTO DataCounty SET DataCounty_Name = ?, DataCounty_BOEID = ?, DataState_ID = ?");
my $SmtmDistrictTown  = $dbh->prepare("INSERT INTO DataDistrictTown SET DataDistrictTown_Name = ?");
   				
### Load all the Datas
my %DBData = ();
my @TableNames = qw/DataFirstName DataLastName DataMiddleName DataCity DataDistrictTown DataState DataStreet/;

my $LocalLimit;
if (@ARGV > 1) {
	$LocalLimit = $LimitTxt;
}

my @DBArrDataStreet;
my @DBArrDataCity;
my @DBArrDataDistrictTown;

foreach my $Table (@TableNames) { 
	print "Loading $Table \t"; 
	my $Stmt_FirstName = "SELECT * FROM " . $Table  . " " . $LocalLimit;
	my $sth = $dbh->prepare( $Stmt_FirstName ); $sth->execute() or die "$! $DBI::errstr";
	my $start_time = Time::HiRes::gettimeofday();
	while (my @row = $sth->fetchrow_array()) {
		$row[1] =~ s/\s+$//;    ### The address need triming.
		$DBData { $Table } { lc $row[1] } = $row[0];
			
		if ($Table eq "DataStreet") { $DBArrDataStreet[$row[0]] = $row[1]; }
		elsif ($Table eq "DataCity") { $DBArrDataCity[$row[0]] = $row[1]; }
		elsif ($Table eq "DataDistrictTown") { $DBArrDataDistrictTown[$row[0]] = $row[1]; }
	}
	my $stop_time = Time::HiRes::gettimeofday();
	printf("%.2f\n", $stop_time - $start_time);
} 

### Load the Data County
my $Stmt_DataDistrict = "SELECT * FROM DataCounty " . $LocalLimit;
my $sth = $dbh->prepare( $Stmt_DataDistrict ); $sth->execute() or die "$! $DBI::errstr";

print "Loading County DB Table: ";
my $start_time = Time::HiRes::gettimeofday();
while (my @row = $sth->fetchrow_array()) {
	$DBData { "DataCounty" } { $row[1] } { $row[2] } = $row[0];
	$DBData { "DataFileCounty" } { $row[1] } { $row[3] } = $row[0];
}
my $stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

### Load the data districts
my %DBDataDistrict = ();
my $Stmt_DataDistrict = "SELECT * FROM DataDistrict " . $LocalLimit;
my $sth = $dbh->prepare( $Stmt_DataDistrict ); $sth->execute() or die "$! $DBI::errstr";

print "Loading Data District: ";
$start_time = Time::HiRes::gettimeofday();
while (my @row = $sth->fetchrow_array()) {
	$DBDataDistrict	{ $row[1] } { $row[2] } { $row[3] } { $row[4] } { $row[5] } { $row[6] } { $row[7] } { $row[8] } { $row[9] } { $row[10] }	= $row[0];
}
$stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

### Load the Adresses 
my $Stmt_DataDistrict = "SELECT * FROM DataAddress " . $LimitTxt;
my $sth = $dbh->prepare( $Stmt_DataDistrict ); $sth->execute() or die "$! $DBI::errstr";

print "Loading Address DB Table: ";
my $start_time = Time::HiRes::gettimeofday();

my @DBArrAddress_DataAddress_ID = ();
my @DBArrAddress_DataAddress_HouseNumber = ();
my @DBArrAddress_DataAddress_FracAddress = ();
my @DBArrAddress_DataAddress_PreStreet = ();
my @DBArrAddress_DataStreet_ID = ();
my @DBArrAddress_DataAddress_PostStreet = ();
my @DBArrAddress_DataCity_ID = ();
my @DBArrAddress_DataCounty_ID = ();
my @DBArrAddress_DataAddress_zipcode = (); 
my @DBArrAddress_DataAddress_zip4 = ();
my @DBArrAddress_Cordinate_ID = ();
my @DBArrAddress_PG_OSM_osm = ();

while (my @row = $sth->fetchrow_array()) {
	$DBArrAddress_DataAddress_HouseNumber[$row[0]] = $row[1];
	$DBArrAddress_DataAddress_FracAddress[$row[0]] = $row[2];
	$DBArrAddress_DataAddress_PreStreet[$row[0]] = $row[3];
	$DBArrAddress_DataStreet_ID[$row[0]] = $row[4];
	$DBArrAddress_DataAddress_PostStreet[$row[0]] = $row[5];
	$DBArrAddress_DataCity_ID[$row[0]] = $row[6];
	$DBArrAddress_DataCounty_ID[$row[0]] = $row[7];
	$DBArrAddress_DataAddress_zipcode[$row[0]] = $row[8];
	$DBArrAddress_DataAddress_zip4[$row[0]] = $row[9];
	$DBArrAddress_Cordinate_ID[$row[0]] = $row[10];
	$DBArrAddress_PG_OSM_osm[$row[0]] = $row[11];
}

my $stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

### Load the DataHouse
my $Stmt_DataDistrict = "SELECT * FROM DataHouse " . $LimitTxt;
my $sth = $dbh->prepare( $Stmt_DataDistrict ); $sth->execute() or die "$! $DBI::errstr";

print "Loading DataHouse DB Table: ";
my $start_time = Time::HiRes::gettimeofday();

my @DBArrHouse_DataAddress_ID = ();
my @DBArrHouse_DataHouse_Apt = ();
my @DBArrHouse_DataDistrictTemporal_GroupID = ();
my @DBArrHouse_DataDistrictTown_ID = ();
my @DBArrHouse_DataHouse_BIN = ();

while (my @row = $sth->fetchrow_array()) {
	$DBArrHouse_DataAddress_ID[$row[0]] = $row[1];
	$DBArrHouse_DataHouse_Apt[$row[0]] = $row[2];
	$DBArrHouse_DataDistrictTemporal_GroupID[$row[0]] = $row[3];
	$DBArrHouse_DataDistrictTown_ID[$row[0]] = $row[4];
	$DBArrHouse_DataHouse_BIN[$row[0]] = $row[5];
}

my $stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

### Load the Voters 
my $Stmt_DataDistrict = "SELECT * FROM Voters " . $LimitTxt;
my $sth = $dbh->prepare( $Stmt_DataDistrict ); $sth->execute() or die "$! $DBI::errstr";

print "Loading Voters DB Table: ";
my $start_time = Time::HiRes::gettimeofday();

my %DBVoter = ();
my %DBVoterCnt = ();

my @DBArrVoter_VotersIndexes_ID = ();
my @DBArrVoter_ElectionsDistricts_DBTable = ();
my @DBArrVoter_ElectionsDistricts_DBTableValue = (); 
my @DBArrVoter_DataHouse_ID = ();
my @DBArrVoter_Voters_Gender = ();
my @DBArrVoter_VotersComplementInfo_ID = ();
my @DBArrVoter_Voters_UniqStateVoterID = ();
my @DBArrVoter_Voters_RegParty = ();
my @DBArrVoter_Voters_ReasonCode = ();
my @DBArrVoter_Voters_Status = ();
my @DBArrVoter_VotersMailingAddress_ID = ();
my @DBArrVoter_Voters_IDRequired = ();
my @DBArrVoter_Voters_IDMet = ();
my @DBArrVoter_Voters_ApplyDate = ();
my @DBArrVoter_Voters_RegSource = ();
my @DBArrVoter_Voters_DateInactive = ();
my @DBArrVoter_Voters_DatePurged = ();
my @DBArrVoter_Voters_CountyVoterNumber = ();
my @DBArrVoter_Voters_RMBActive = ();
my @DBArrVoter_Voters_RecFirstSeen = ();
my @DBArrVoter_Voters_RecLastSeen = ();

while (my @row = $sth->fetchrow_array()) {
	
	$DBVoter { $row[5] } .= $row[0] . " ";
	
	if ( defined ($DBVoter { $row[5] })) {
		$DBVoterCnt { $row[5] } += 1;
	} else {
		$DBVoterCnt { $row[5] } = 1;
	}
	
	$DBArrVoter_VotersIndexes_ID[$row[0]] = $row[1];
	$DBArrVoter_DataHouse_ID[$row[0]] = $row[2];
	$DBArrVoter_Voters_Gender[$row[0]] = $row[3];
	$DBArrVoter_VotersComplementInfo_ID[$row[0]] = $row[4];
	$DBArrVoter_Voters_UniqStateVoterID[$row[0]] = $row[5];
	$DBArrVoter_Voters_RegParty[$row[0]] = $row[6];
	$DBArrVoter_Voters_ReasonCode[$row[0]] = $row[7];
	$DBArrVoter_Voters_Status[$row[0]] = $row[8];
	$DBArrVoter_VotersMailingAddress_ID[$row[0]] = $row[9];
	$DBArrVoter_Voters_IDRequired[$row[0]] = $row[10];
	$DBArrVoter_Voters_IDMet[$row[0]] = $row[11];
	$DBArrVoter_Voters_ApplyDate[$row[0]] = $row[12];
	$DBArrVoter_Voters_RegSource[$row[0]] = $row[13];
	$DBArrVoter_Voters_DateInactive[$row[0]] = $row[14];
	$DBArrVoter_Voters_DatePurged[$row[0]] = $row[15];
	$DBArrVoter_Voters_CountyVoterNumber[$row[0]] = $row[16];
	$DBArrVoter_Voters_RMBActive[$row[0]] = $row[17];
	$DBArrVoter_Voters_RecFirstSeen[$row[0]] = $row[18];
	$DBArrVoter_Voters_RecLastSeen[$row[0]] = $row[19];
}

my $stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

### Load all the Indexes
my %DBVoterIndexes = ();
my %DBVoterIndexesCnt = ();
my @DBVIdx_DataLastName_ID = ();
my @DBVIdx_DataFirstName_ID = ();
my @DBVIdx_DataMiddleName_ID = ();
my @DBVIdx_VotersIndexes_Suffix = ();
my @DBVIdx_VotersIndexes_DOB = ();
my @DBVIdx_VotersIndexes_UniqStateVoterID = ();

my $Stmt_FirstName = "SELECT * FROM VotersIndexes " . $LimitTxt;
my $sth = $dbh->prepare( $Stmt_FirstName ); $sth->execute() or die "$! $DBI::errstr";

print "Loading Voter Indexes: ";
my $start_time = Time::HiRes::gettimeofday();

while (my @row = $sth->fetchrow_array()) {
	$DBVoterIndexes { $row[6] } .= $row[0] . " ";
	
	if ( defined ($DBVoterIndexesCnt { $row[6] })) {
		$DBVoterIndexesCnt { $row[6] } += 1;
	} else {
		$DBVoterIndexesCnt { $row[6] } = 1;
	}
	
	$DBVIdx_DataLastName_ID[$row[0]] = $row[1];
	$DBVIdx_DataFirstName_ID[$row[0]] = $row[2];
	$DBVIdx_DataMiddleName_ID[$row[0]] = $row[3];
	$DBVIdx_VotersIndexes_Suffix[$row[0]] = $row[4];
	$DBVIdx_VotersIndexes_DOB[$row[0]] = $row[5];
	$DBVIdx_VotersIndexes_UniqStateVoterID[$row[0]] = $row[6];
}
my $stop_time = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time - $start_time);

# Read the Table Directory in the file
my $filename = '/home/usracct/.voter_file';
open(my $fh, '<:encoding(UTF-8)', $filename) or die "Could not open file '$filename' $!";
my $tabledate = <$fh>;
chomp($tabledate);
close($fh);

$filename = "/home/usracct/VoterFiles/NY/" . $tabledate . "/AllNYSVoters_" . $tabledate . ".txt";
my $FileLastDateSeen = $tabledate;
print "Working on " . $filename . "\n";

open(my $fh, $filename ) or die "cannot open $filename: $!";

my $FileCounter = 0;
my $Counter = 0;
my @TheWholeFile = ();
my $StringFile = "";

$start_time = Time::HiRes::gettimeofday();
while (my $row = <$fh>) {
	$TheWholeFile[$Counter] = $row;
	#	$StringFile .= $row;
  $Counter++;
 	last if ( @ARGV > 0 && $Counter == @ARGV[0] );  
}
my $stop_time = Time::HiRes::gettimeofday();
printf("Loading the CD Information: %.2f\n", $stop_time - $start_time);

print "done\n";
print "Loaded into memory $Counter lines\n";

my $start_time_total = Time::HiRes::gettimeofday();
for (my $i = 0; $i < $Counter; $i++) {
	my $number = $i;
	
	my $csv = Text::CSV->new();
	$csv->always_quote(1);

	if ($csv->parse($TheWholeFile[$number])){
	
    my @values = $csv->fields();
   	my $status = $csv->combine(@values);
   	
   	if ($Verbose > 5) {
 			print "\n##################################################### $number ####################################\n";
	  	print "Line $number: " . $TheWholeFile[$number] . "\n";
 			print "CVS String: " . $csv->string() . "\n\n";
   	}
   	
		# 		NY_Raw_ID
		#			0 -> LastName -> "MALLARO"		#			1 -> FirstName -> "ELIZABETH"		#			2 -> MiddleName -> "A"		    #			3 -> Suffix -> ""
		#			4 -> ResHouseNumber -> "124"	#			5 -> ResFracAddress -> ""    		#			6 -> ResPreStreet -> ""		    #			7 -> ResStreetName -> "TOAS AVE "
		#			8 -> ResPostStDir -> ""   		#			9 -> ResType -> ""          		#			10 -> ResApartment -> ""	    #			11 -> ResNonStdFormat -> ""
		#			12 -> ResCity -> "SYRACUSE"		#			13 -> ResZip -> "13211"     		#			14 -> ResZip4 -> ""   		    #			15 -> ResMail1 -> ""
		#			16 -> ResMail2 -> ""      		#			17 -> ResMail3 -> ""        		#			18 -> ResMail4 -> ""  		    #			19 -> DOB -> "19380526"
		#			20 -> Gender -> "F",      		#			21 -> EnrollPolParty -> "REP",	#			22 -> OtherParty -> "",		    #			23 -> CountyCode -> "34",
		#			24 -> ElectDistr -> "21", 		#			25 -> LegisDistr -> "5",    		#			26 -> TownCity -> "SALINA"    #			27 -> Ward -> "000",
		#			28 -> CongressDistr -> "22",	#			29 -> SenateDistr -> "50",  		#			30 -> AssemblyDistr -> "128" 	#			31 -> LastDateVoted -> "20201103",
		#			32 -> PrevYearVoted -> "",    #			33 -> PrevCounty -> "  ",   		#			34 -> PrevAddress -> "",   		#			35 -> PrevName -> "",
		#			36 -> CountyVoterNumber -> "H23007" #			37 -> RegistrationCharacter -> "19940726",	                	#			38 -> ApplicationSource -> "CBOE",
		#			39 -> IDRequired -> "N",    	#			40 -> IDMet -> "Y",           	#			41 -> Status -> "A",      		#			42 -> ReasonCode -> "",
		#			43 -> VoterMadeInactive -> ""	#			44 -> VoterPurged -> "",    		#			45 -> UniqNYSVoterID -> "NY000000000009186673",
		#			46 -> VoterHistory -> "2020 General Election(A)"
	
   	### Trim the problematic values
   	$values[7] =~ s/\s+$//;    ### The address need triming.
   	$values[7] =~ s/\s+/ /g;   ### Remove all white spaces
   	$values[26] =~ s/\s+/ /g;   ### Remove all white spaces
   	
 		$values[19] =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/;    ### To match the DOB from file to DB
 		$values[37] =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/;    
 		$values[44] =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/;   
 		$FileLastDateSeen =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/;
 		
 		
   	### Check the single table values
   	my $LocalFirstNameID = $DBData { "DataFirstName" } { lc $values[1] };
		my $LocalMiddleNameID = $DBData { "DataMiddleName" } { lc $values[2] };
		my $LocalLastNameID = $DBData { "DataLastName" } { lc $values[0] };
		my $LocalDataStreetID = $DBData { "DataStreet" } { lc $values[7] };
		my $LocalDataCityID = $DBData { "DataCity" } { lc $values[12] };
		my $LocalCountyID = $DBData { "DataFileCounty" } { $StaticDataState } { $values[23] };
		my $LocalDistrictTownID = $DBData { "DataDistrictTown" } { lc $values[26] };
   	
   	if ( 	! defined $LocalFirstNameID || ! defined $LocalMiddleNameID || ! defined  $LocalLastNameID  || 
   				! defined $LocalDataStreetID || ! defined $LocalDataCityID  || ! defined  $LocalCountyID  || 	
   				! defined  $LocalDistrictTownID  ) {
   					
   	  ### Verify that it's not empty
   	  my $Continue = 0;
  
   	  if ( ! defined $LocalFirstNameID) {
	   	  	if (length($values[1] ) == 0 ) {
	   	  	$Continue = 1;;
		   	} else {
		   		$SmtmFirstName->execute(nc($values[1]), ReturnCompress(lc $values[1])) or die "$! $DBI::errstr";  	  
	   	  	$DBData { "DataFirstName" } { lc $values[1] }	= $SmtmFirstName->{'mysql_insertid'}; 
	   	  	$LocalFirstNameID = $DBData { "DataFirstName" } { lc $values[1] };
	   	  }
   	  }
   	  
   	  if ( ! defined $LocalMiddleNameID) {
   	  	if (length($values[2]) == 0 ) { 
	   	  	$Continue = 1;;
		   	} else {
		   		$SmtmMiddleName->execute(nc($values[2]), ReturnCompress(lc $values[2])) or die "$! $DBI::errstr";
	   	  	$DBData { "DataMiddleName" } { lc $values[2] } = $SmtmMiddleName->{'mysql_insertid'}; 
	   	  	$LocalMiddleNameID = $DBData { "DataMiddleName" } { lc $values[2] };
	 	  	}
	 	  }

   	  if ( ! defined $LocalLastNameID) {
   	  	if (length($values[0]) == 0 ) { 
   	  		$Continue = 1;
		   	} else {
		   		$SmtmLastName->execute(nc($values[0]), ReturnCompress(lc $values[0])) or die "$! $DBI::errstr";
	   	  	$DBData { "DataLastName" } { lc $values[0] } = $SmtmLastName->{'mysql_insertid'}; 
	   	  	$LocalLastNameID = $DBData { "DataLastName" } { lc $values[0] };
	   	  }
   	  }
   	  
   	  if ( ! defined $LocalDataStreetID) {
   	  	if (length($values[7]) == 0 ) { 
   	  		$Continue = 1;
	   		} else {
		   		$SmtmDataStreet->execute(nc($values[7])) or die "$! $DBI::errstr to add -> " . lc $values[7] . " - " . $number . " -> " . $DBData { "DataStreet" } { lc $values[7] };
	   	  	$DBData { "DataStreet" } { lc $values[7] } = $SmtmDataStreet->{'mysql_insertid'}; 
	   	  	$LocalDataStreetID = $DBData { "DataStreet" } { lc $values[7] };
	   	  	print "MYSQL ADD DataStreet: " . $LocalDataStreetID . "\n";
	   	  }
	   	}
	   	  
   	  if ( ! defined $LocalDataCityID) {
   	  	if (length($values[12]) == 0 ) { 
	   	  	$Continue = 1;
	   		} else {
   	  		$SmtmDataCity->execute(nc($values[12])) or die "$! $DBI::errstr";
   	  		$DBData { "DataCity" } { lc $values[12] } = $SmtmDataCity->{'mysql_insertid'}; 
   	  		$LocalDataCityID = $DBData { "DataCity" } { lc $values[12] };
				}
			}
   	  
   	  if ( ! defined $LocalCountyID) {
   	  	if (length($values[23]) == 0 ) { 
	   	  	$Continue = 1;
		   	} else {
		   		$SmtmCounty->execute("Empty Name to Replace", $values[23], $StaticDataState) or die "$! $DBI::errstr";
	   	  	$DBData { "DataFileCounty" } { $StaticDataState } { $values[23] } = $SmtmCounty->{'mysql_insertid'}; 
	   	  	$LocalCountyID = $DBData { "DataFileCounty" } { $StaticDataState } { $values[23] };
				}
   	  }
   	  
   	  if ( ! defined $LocalDistrictTownID) {
   	  	if (length($values[26]) == 0 ) { 
	   	  	$Continue = 1;
		   	} else {
		   		$SmtmDistrictTown->execute(nc($values[26])) or die "$! $DBI::errstr";
		   	  $DBData { "DataDistrictTown" } { lc $values[26] } = $SmtmDistrictTown->{'mysql_insertid'}; 
		   	  $LocalDistrictTownID = $DBData { "DataDistrictTown" } { lc $values[26] };
	   	  }
   	  }
   	  
   		if ( $Continue == 0  ) {		
   			if ($Verbose > 5) {	
	   			print "\n##################################################### $number ####################################\n";
		  		print "Line $number: " . $TheWholeFile[$number] . "\n";
	 				print "CVS String: " . $csv->string() . "\n\n";					  
		 	  	print "Simple Request IDs:\n";
		 	  	print "\t$LocalFirstNameID\tFirstName: " . 						$values[1] 	. "\n";
		 		  print "\t$LocalMiddleNameID\tMiddle: " . 							$values[2] 	. "\n";
		 		  print "\t$LocalLastNameID\tLastName: " . 							$values[0] 	. "\n";
		 	 		print "\t$LocalDataStreetID\tDataStreet: " . 					$values[7] 	. "\n";
		 	  	print "\t$LocalDataCityID\tCity: " . 									$values[12] . "\n"; 
		 		 	print "\t$LocalCountyID\tDataCounty: " . 							$values[23] . "\n"; 
		 	 		print "\t$LocalDistrictTownID\tDataDistrictTown: " . 	$values[26] . "\n"; 
		  		print "\n";
		  		print "Found problem ...\n\n";
		  		#exit();
		  	}
	  	} 	  	
   	}

   	#### Let's work the voter Indexe
   	if ( defined $DBVoterIndexes { $values[45] } ) {
   		 my $Continue = 0;

   		print "\nUniqStatID: " . $values[45]  . "\n" if ($Verbose > 5);
   		foreach my $IndexToFind ( split(' ', $DBVoterIndexes { $values[45] })) {
   			if ( $Verbose > 10) {
	   			print "\tIndexes to check: " . $IndexToFind . "\n";
					print "\t\tFirstNameID: " . $DBVIdx_DataFirstName_ID[$IndexToFind] . "/" . $LocalFirstNameID . "\t";
					print "MiddleNameID: " . $DBVIdx_DataMiddleName_ID[$IndexToFind] . "/" . $LocalMiddleNameID . "\t";
					print "LastNameID: " . $DBVIdx_DataLastName_ID[$IndexToFind] . "/" . $LocalLastNameID .  "\t";
					print "Suffix: " . $DBVIdx_VotersIndexes_Suffix[$IndexToFind] . "/" . $values[3] . "\n";
					print "\t\tDOB:" . $DBVIdx_VotersIndexes_DOB[$IndexToFind] . "/" . $values[19] . "\t";
					print "UniqSt: " . $DBVIdx_VotersIndexes_UniqStateVoterID[$IndexToFind] . "\n";
				}
			
				### Compare against the line.
				#if ( $DBVIdx_DataFirstName_ID[$IndexToFind] 
				if ( $DBVIdx_DataFirstName_ID[$IndexToFind] != $LocalFirstNameID ) { print "Problem with First name\nExiting" if ($Verbose > 5); $Continue = 1; }
				if ( $DBVIdx_DataMiddleName_ID[$IndexToFind] != $LocalMiddleNameID ) { print "Problem with Middle name\nExiting" if ($Verbose > 5); $Continue = 1; }
				if ( $DBVIdx_DataLastName_ID[$IndexToFind] != $LocalLastNameID  ) { print "Problem with Last Name\nExiting" if ($Verbose > 5); $Continue = 1; }
				if ( $DBVIdx_VotersIndexes_Suffix[$IndexToFind] != $values[3] ) { print "Problem with Suffix \nExiting" if ($Verbose > 5); $Continue = 1; }
				if ( $DBVIdx_VotersIndexes_DOB[$IndexToFind] != $values[19] ) { print "Problem with DOB \nExiting" if ($Verbose > 5); $Continue = 1; }

				if ( $Continue == 1) {
					if ( $Verbose > 10) {
						print "\n##################################################### $number ####################################\n";
		    		print "Line $number: " . $TheWholeFile[$number] . "\n";
	   				print "Unique Voter ID " . $values[45] . " is not defined\nExiting...\n\n";
	   			}
				}
			}

			#			0 -> LastName -> "MALLARO"		#			1 -> FirstName -> "ELIZABETH"		#			2 -> MiddleName -> "A"		    #			3 -> Suffix -> ""
			#			4 -> ResHouseNumber -> "124"	#			5 -> ResFracAddress -> ""    		#			6 -> ResPreStreet -> ""		    #			7 -> ResStreetName -> "TOAS AVE "
			#			8 -> ResPostStDir -> ""   		#			9 -> ResType -> ""          		#			10 -> ResApartment -> ""	    #			11 -> ResNonStdFormat -> ""
			#			12 -> ResCity -> "SYRACUSE"		#			13 -> ResZip -> "13211"     		#			14 -> ResZip4 -> ""   		    #			15 -> ResMail1 -> ""
			#			16 -> ResMail2 -> ""      		#			17 -> ResMail3 -> ""        		#			18 -> ResMail4 -> ""  		    #			19 -> DOB -> "19380526"
			#			20 -> Gender -> "F",      		#			21 -> EnrollPolParty -> "REP",	#			22 -> OtherParty -> "",		    #			23 -> CountyCode -> "34",
			#			24 -> ElectDistr -> "21", 		#			25 -> LegisDistr -> "5",    		#			26 -> TownCity -> "SALINA"    #			27 -> Ward -> "000",
			#			28 -> CongressDistr -> "22",	#			29 -> SenateDistr -> "50",  		#			30 -> AssemblyDistr -> "128" 	#			31 -> LastDateVoted -> "20201103",
			#			32 -> PrevYearVoted -> "",    #			33 -> PrevCounty -> "  ",   		#			34 -> PrevAddress -> "",   		#			35 -> PrevName -> "",
			#			36 -> CountyVoterNumber -> "H23007" #			37 -> RegistrationCharacter -> "19940726",	                	#			38 -> ApplicationSource -> "CBOE",
			#			39 -> IDRequired -> "N",    	#			40 -> IDMet -> "Y",           	#			41 -> Status -> "A",      		#			42 -> ReasonCode -> "",
			#			43 -> VoterMadeInactive -> ""	#			44 -> VoterPurged -> "",    		#			45 -> UniqNYSVoterID -> "NY000000000009186673",
			#			46 -> VoterHistory -> "2020 General Election(A)"

			print "\n\tVoters entries from: " . $DBVoter { $values[45] } . "\n" if ($Verbose > 5);
			foreach my $IndexToFind ( split(' ', $DBVoter { $values[45] })) {
				
				if ( $Verbose > 10) {
					print "\tCheck the DBVoter: " . $IndexToFind . "\n";
					print "\t\tVotersIndexes_ID: " . $DBArrVoter_VotersIndexes_ID[$IndexToFind] . "\n";
					print "\t\tDataHouse_ID: " . $DBArrVoter_DataHouse_ID[$IndexToFind] . "\n";
					print "\t\tLoading Address DB Table:\n";
					print "\t\tVoters_Gender: " . $DBArrVoter_Voters_Gender[$IndexToFind] . "/" . $values[20] . "-" . ReturnGender($values[20]) . "\n";
					print "\t\tVotersComplementInfo_ID: " . $DBArrVoter_VotersComplementInfo_ID[$IndexToFind] . "\n";
					print "\t\tVoters_UniqStateVoterID: " . $DBArrVoter_Voters_UniqStateVoterID[$IndexToFind]  . "/" . $values[45] . "\n";
					print "\t\tVoters_RegParty: " . $DBArrVoter_Voters_RegParty[$IndexToFind]  . "/" . $values[21] . "\n";
					
					print "\t\tVoters_ReasonCode: " . $DBArrVoter_Voters_ReasonCode[$IndexToFind]  . "/" . $values[42] . "\n";
					print "\t\tVoters_ReasonCode: " . $DBArrVoter_Voters_ReasonCode[$IndexToFind]  . "/" . $values[42] . "-" . ReturnReasonCode($values[42]) . "\n";
					
					print "\t\tVoters_Status: " . $DBArrVoter_Voters_Status[$IndexToFind] . "/" . $values[41]  . "-" . ReturnStatusCode($values[41]). "\n";
					print "\t\tVotersMailingAddress_ID: " . $DBArrVoter_VotersMailingAddress_ID[$IndexToFind] . "\n";
					print "\t\tVoters_IDRequired: " . $DBArrVoter_Voters_IDRequired[$IndexToFind]  . "/" . $values[39] . "-" . ReturnYesNo($values[39]) .  "\n";
					print "\t\tVoters_IDMet: " . $DBArrVoter_Voters_IDMet[$IndexToFind]  . "/" . $values[40]  . "-" . ReturnYesNo($values[40]) .  "\n";
					print "\t\tVoters_ApplyDate: " . $DBArrVoter_Voters_ApplyDate[$IndexToFind]  . "/" . $values[37] . "\n";
					print "\t\tVoters_RegSource: " . $DBArrVoter_Voters_RegSource[$IndexToFind]  . "/" . $values[38] . "-" . ReturnRegistrationSource($values[38]) . "\n";
					print "\t\tVoters_DateInactive: " . $DBArrVoter_Voters_DateInactive[$IndexToFind]  . "/" . $values[43] . "\n";
					print "\t\tVoters_DatePurged: " . $DBArrVoter_Voters_DatePurged[$IndexToFind]  . "/" . $values[44] . "\n";
					print "\t\tVoters_CountyVoterNumber: " . $DBArrVoter_Voters_CountyVoterNumber[$IndexToFind]  . "/" . $values[36] . "\n";
					print "\t\tVoters_RMBActive: " . $DBArrVoter_Voters_RMBActive[$IndexToFind] . "\n";
					print "\t\tVoters_RecFirstSeen: " . $DBArrVoter_Voters_RecFirstSeen[$IndexToFind] . "\n";
					print "\t\tVoters_RecLastSeen: " . $DBArrVoter_Voters_RecLastSeen[$IndexToFind] . "/" . $FileLastDateSeen . "\n";
					print "\n";
					
					print "\tDataHouse Information: " . $DBArrVoter_DataHouse_ID[$IndexToFind] . "\n";
					print "\t\tDataAddress_ID: " . $DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";
					print "\t\tDataHouse_Apt: " . $DBArrHouse_DataHouse_Apt[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";
					print "\t\tDataDistrictTemporal_GroupID: " . $DBArrHouse_DataDistrictTemporal_GroupID[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";
					print "\t\tDataDistrictTown_ID: " . $DBArrHouse_DataDistrictTown_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";
					print "\t\tDataHouse_BIN: " . $DBArrHouse_DataHouse_BIN[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";				
					print "\n";
					
					print "\tDataAddress Information: " . $DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";
					print "\t\tHouseNumber: " . $DBArrAddress_DataAddress_HouseNumber[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tFracAddress: " . $DBArrAddress_DataAddress_FracAddress[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tPreStreet: " . $DBArrAddress_DataAddress_PreStreet[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tDataStreet_ID: " . $DBArrAddress_DataStreet_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tPostStreet: " . $DBArrAddress_DataAddress_PostStreet[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tDataCity_ID: " . $DBArrAddress_DataCity_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tDataCounty_ID: " . $DBArrAddress_DataCounty_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tZipcode: " . $DBArrAddress_DataAddress_zipcode[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tZip4: " . $DBArrAddress_DataAddress_zip4[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tCordinate_ID: " . $DBArrAddress_Cordinate_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tPG_OSM_osm: " . $DBArrAddress_PG_OSM_osm[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\n";
					
					print "\tDataStreet Information: " . $DBArrAddress_DataStreet_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tDataStreet_ID: " . $DBArrAddress_DataStreet_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tDataStreet_Name: " . $DBArrDataStreet[$DBArrAddress_DataStreet_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]]] . "\n";
					print "\n";

					print "\tDataCity Information: " . $DBArrAddress_DataCity_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tDataCity_ID: " . $DBArrAddress_DataCity_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\t\tDataStreet_Name: " . $DBArrDataCity[$DBArrAddress_DataCity_ID[$DBArrHouse_DataAddress_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]]] . "\n";
					print "\n";

					print "\tDataDistrictTown Information: " . $DBArrHouse_DataDistrictTown_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";
					print "\t\tDataDistrictTown_ID: " . $DBArrHouse_DataDistrictTown_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]] . "\n";
					print "\t\tDataDistrictTown_Name: " . $DBArrDataDistrictTown[$DBArrHouse_DataDistrictTown_ID[$DBArrVoter_DataHouse_ID[$IndexToFind]]] . "\n";
					print "\n";
				}
				
				# print "Data District: " . $DBDataDistrict	{ undef  } 
				#																					{ $values[24] } { $values[30] } { $values[29] } { $values[25] } 
				#																					{ $values[27] } { $values[28] } { undef } { undef } { undef }	. "\n";
				# print "\n\n";
		
 			}
   	} else {
   		print "\n##################################################### $number ####################################\n";
	    print "Line $number: " . $TheWholeFile[$number] . "\n";
   		print "Unique Voter ID " . $values[45] . " is not defined\nExiting...\n\n";
   		#exit();
   	}
   	
   	if ( $number % 10000 == 0 ) {
   		my $stop_time_local = Time::HiRes::gettimeofday();
			printf("Processing the whole $number of entries: %.2f\n", $stop_time_local - $start_time_total);
   	}
   	
		# Redo
		#     23 -> CountyCode -> "34",
		#			24 -> ElectDistr -> "21",
		#			25 -> LegisDistr -> "5",
		#			26 -> TownCity -> "SALINA",
		#			27 -> Ward -> "000",
		#			28 -> CongressDistr -> "22",
		#			29 -> SenateDistr -> "50",
		#			30 -> AssemblyDistr -> "128",
		
		# { $row->{ "DataCounty_ID" }
		# { $row->{ "DataDistrict_Electoral" }      -> 24
 		# { $row->{ "DataDistrict_StateAssembly" }  -> 30
		# { $row->{ "DataDistrict_SenateSenate" }   -> 29
		# { $row->{ "DataDistrict_Legislative" }    -> 25
		# { $row->{ "DataDistrict_Ward" }           -> 27
		# { $row->{ "DataDistrict_Congress" }       -> 28
		# { $row->{ "DataDistrict_Council" } 
		# { $row->{ "DataDistrict_CivilCourt" }
		# { $row->{ "DataDistrict_Judicial" }

	}
}  

my $stop_time_total = Time::HiRes::gettimeofday();
printf("Processing the whole $Counter of entries: %.2f\n", $stop_time_total - $start_time_total);

#### These are the standart questions in the NYS voter file.
sub	ReturnReasonCode {	
	if ( $_[0] ne "" ) { 
		if ($_[0] eq "ADJ-INCOMP") { return "AdjudgedIncompetent" }
		elsif ($_[0] eq "DEATH") {  return "Death" }
		elsif ($_[0] eq "DUPLICATE") {  return "Duplicate" }
		elsif ($_[0] eq "FELON") {  return "Felon" }
		elsif ($_[0] eq "MAIL-CHECK") { return "MailCheck" }
		elsif ($_[0] eq "MAILCHECK") { return "MailCheck" }
		elsif ($_[0] eq "MOVED") { return "MouvedOutCounty" }
		elsif ($_[0] eq "NCOA") {  return "NCOA" }
		elsif ($_[0] eq "NVRA") {  return "NVRA" }
		elsif ($_[0] eq "RETURN-MAIL") {  return "ReturnMail" }
		elsif ($_[0] eq "VOTER-REQ") {  return "VoterRequest" }
		elsif ($_[0] eq "OTHER") {  return "Other" }
		elsif ($_[0] eq "COURT") {  return "Court" }
		elsif ($_[0] eq "INACTIVE") {  return "Inactive" }
	
		print "Catastrophic ReturnReasonCode problem as #" . $_[0] . "#\n";
		exit();
	} 
	
	return undef;
}

sub ReturnRegistrationSource {
	if ( defined $_[0] ) {
		if ($_[0] eq "AGCY") { return "Agency"; }
		elsif ($_[0] eq "CBOE") { return "CBOE"; }
		elsif ($_[0] eq "DMV") { return "DMV"; }
		elsif ($_[0] eq "LOCALREG") { return "LocalRegistrar"; }
		elsif ($_[0] eq "MAIL") { return "MailIn"; }
		elsif ($_[0] eq "SCHOOL") { return "School"; }
		print "Catastrophic ReturnRegistrationSource problem as it is empty: $_[0]\n";
		exit();
	}
	
	return undef;
}
	
sub	ReturnStatusCode {
	if ( defined $_[0]) { 
		if ($_[0] eq "ACTIVE") { return "Active"; }
		elsif ($_[0] eq "AM") { return "ActiveMilitary"; }
		elsif ($_[0] eq "AF") { return "ActiveSpecialFederal"; }
		elsif ($_[0] eq "AP") { return "ActiveSpecialPresidential"; }
		elsif ($_[0] eq "AU") { return "ActiveUOCAVA"; }
		elsif ($_[0] eq "INACTIVE") { return "Inactive"; }
		elsif ($_[0] eq "PURGED") { return "Purged"; }
		elsif ($_[0] eq "PREREG") { return "Prereg17YearOlds"; }
		elsif ($_[0] eq "RETURN-MAIL") { return "ReturnMail"; }
		elsif ($_[0] eq "VOTER-REQ") { return "VoterRequest"; }
		elsif ($_[0] eq "A") { return "Active"; }
		elsif ($_[0] eq "P") { return "Purged"; }
		elsif ($_[0] eq "I") { return "Inactive"; }
		elsif ($_[0] eq "17") { return "Prereg17YearOlds"; }	

		print "Catastrophic ReturnStatusCode problem as it is empty: " . $_[0] . "\n";
		exit();
	}

	return undef;
}


sub ReturnGender {
	if ( defined $_[0] ) { 
		if ( $_[0] eq 'M') { return "male"; } 
		if ( $_[0] eq 'F') { return "female";	}
		if ( $_[0] eq 'U') { return 'undetermined'; }	
		
		print "Catastrophic ReturnGender problem as it is empty: $_[0]\n";
		exit();
	}
	
	
	
	return undef;
} 


sub ReturnYesNo {
	if ($_[0] eq 'Y') { return 'yes';	}	
	elsif ($_[0] eq 'N') { return 'no'; }
	
	print "Catastrophic ReturnYesNo problem as it is empty: $_[0]\n";
	exit();
	
	return undef;
}

sub ReturnCompress {
	my $string = $_[0];
	$string =~ tr/a-zA-Z//dc;
	return $string;
}