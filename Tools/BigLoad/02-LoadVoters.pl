#!/usr/bin/perl

local $| = 1; # activate autoflush to immediately show the prompt

my $InsertDBEachCount = 1000000; 
my $TimePerCycle = 1000000;

my $SystemState = "1";

use strict;
use DBI;
use Time::HiRes;
use Text::CSV;
use Lingua::EN::NameCase;
use Encode;

my $dbname = "RepMyBlock";
my $dbhost = "data.theochino.us";
my $dbport = "3306";
my $dbuser = "usracct";
my $dbpass = "usracct";

my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
$dbh->{mysql_auto_reconnect} = 1;

### Load all the Datas
my %DBData = ();
my %DBMailing = ();
my %DBDataVoterIdx = ();
my %DBDataAddress = ();
my %DBDataDistrict = ();
my %DBDataCounty = ();


my $Val1; my $Val2; my $Val3; my $Val4; my $Val5; my $Val6; my $Val7; my $Val8; my $Val9; my $Val10;
#my @TableNames = qw/DataFirstName DataLastName DataMiddleName DataCity DataDistrictTown DataStreet DataState/;
# TRUNCATE DataFirstName; TRUNCATE DataLastName; TRUNCATE DataMiddleName; TRUNCATE DataCity; TRUNCATE DataDistrictTown; TRUNCATE DataStreet;

##### THIS IS THE PRE 2022 FIELDS 	
#	NY_Raw_ID
#	0 -> LastName -> varchar(50)						#	1 -> FirstName -> varchar(50)						#	2 -> MiddleName -> varchar(50)					#	3 -> Suffix -> varchar(10)
#	4 -> ResHouseNumber -> varchar(10)			#	5 -> ResFracAddress -> varchar(10)			#	6 -> ResApartment -> varchar(15)				#	7 -> ResPreStreet -> varchar(10)
#	8 -> ResStreetName -> varchar(70)				#	9 -> ResPostStDir -> varchar(10)				#	10 -> ResCity -> varchar(50)						#	11 -> ResZip -> char(5)
#	12 -> ResZip4 -> char(4)								#	13 -> ResMail1 -> varchar(100)     			#	14 -> ResMail2 -> varchar(100)   				#	15 -> ResMail3 -> varchar(100)
#	16 -> ResMail4 -> varchar(100)     			#	17 -> DOB -> char(8)	       						#	18 -> Gender -> char(1)  		    				#	19 -> EnrollPolParty -> char(3)
#	20 -> OtherParty -> varchar(30)					# 21 -> CountyCode -> char(2)							#	22 -> ElectDistr -> char(3)							#	23 -> LegisDistr -> char(3)
#	24 -> TownCity -> varchar(30)						#	25 -> Ward -> char(3)										#	26 -> CongressDistr -> char(3)					#	27 -> SenateDistr -> char(3)
#	28 -> AssemblyDistr -> char(3)					#	29 -> LastDateVoted -> char(8)					#	30 -> PrevYearVoted -> varchar(4) 			#	31 -> PrevCounty -> char(2)
#	32 -> PrevAddress -> varchar(100) 			#	33 -> PrevName -> varchar(150)					#	34 -> CountyVoterNumber -> varchar(50)	#	35 -> RegistrationCharacter -> char(8)
#	36 -> ApplicationSource -> varchar(10)	#	37 -> IDRequired -> char(1)							#	38 -> IDMet ->char(1)										#	39 -> Status -> varchar(10)							
#	40 -> ReasonCode -> varchar(15)    			#	41 -> VoterMadeInactive -> char(8)			#	42 -> VoterPurged -> char(8)						#	43 -> UniqNYSVoterID -> varchar(50)			
#	44 -> VoterHistory -> text 		

##### This is the key
### DataFirstName -> 1  DataLastName -> 0 DataMiddleName -> 2 DataCity -> 10 DataDistrictTown -> 24 DataStreet -> 8

##### THIS IS THE POST 2022 FIELDS 	
#	NY_Raw_ID
#	0 -> LastName -> varchar(50)						#	1 -> FirstName -> varchar(50)						#	2 -> MiddleName -> varchar(50)					#	3 -> Suffix -> varchar(10)
#	4 -> ResHouseNumber -> varchar(10)			#	5 -> ResFracAddress -> varchar(10)			#	6 -> ResPreStreet -> varchar(10)				#	7 -> ResStreetName -> varchar(70)
#	8 -> ResPostStDir -> varchar(10)				#	9 -> ResType -> varchar(10)  						#	10 -> ResApartment -> varchar(15)				#	11 -> ResNonStdFormat -> varchar(250)
#	12 -> ResCity -> varchar(50)						#	13 -> ResZip -> char(5)     						#	14 -> ResZip4 -> char(4)   		   				#	15 -> ResMail1 -> varchar(100)
#	16 -> ResMail2 -> varchar(100)     			#	17 -> ResMail3 -> varchar(100)     			#	18 -> ResMail4 -> varchar(100)					#	19 -> DOB -> char(8)
#	20 -> Gender -> char(1)      						#	21 -> EnrollPolParty -> char(3)					#	22 -> OtherParty -> varchar(30)	  			#	23 -> CountyCode -> char(2)
#	24 -> ElectDistr -> char(3)							#	25 -> LegisDistr -> char(3)    					#	26 -> TownCity -> varchar(30)   				#	27 -> Ward -> char(3)
#	28 -> CongressDistr -> char(3)					#	29 -> SenateDistr -> char(3)  					#	30 -> AssemblyDistr -> char(3) 					#	31 -> LastDateVoted -> char(8)
#	32 -> PrevYearVoted -> varchar(4)   		#	33 -> PrevCounty -> char(2)   					#	34 -> PrevAddress -> varchar(100)				#	35 -> PrevName -> varchar(150)
#	36 -> CountyVoterNumber -> varchar(50)	#	37 -> RegistrationCharacter -> char(8)	#	38 -> ApplicationSource -> varchar(10)	#	39 -> IDRequired -> char(1)    				
#	40 -> IDMet -> char(1)        					#	41 -> Status -> varchar(10)      				#	42 -> ReasonCode -> varchar(15)					#	43 -> VoterMadeInactive -> char(8)		
#	44 -> VoterPurged -> char(8)  					#	45 -> UniqNYSVoterID -> varchar(50)			#	46 -> VoterHistory -> text


my @ValueKey = ([ 0, 1, 2, 3, 17, 43, 4, 5, 7, 8, 9, 10, 21, 11, 12, 22, 28, 27, 23, 25, 26],
								[ 0, 1, 2, 3, 19, 45, 4, 5, 6, 7, 8, 12, 23, 13, 14, 24, 30, 29, 25, 27, 28] );

my @TableNames  = qw/DataFirstName DataLastName DataMiddleName DataCity DataDistrictTown DataStreet DataMailingAddress VotersIndexes DataAddress DataDistrict DataCounty/;
my $LocalLimit;

my $TableCounter = 0;
my @stmts = ();
my @sqlintro = ();
my @sqldata = ();
my @FirstTime = ();
my @DBAddCounter = ();

#### LoadDataFromFile
foreach my $Table (@TableNames) { 
	print "Loading $Table \t"; 
	my $Stmt_FirstName = "SELECT * FROM " . $Table  . " " . $LocalLimit;
	my $sth = $dbh->prepare( $Stmt_FirstName ); $sth->execute() or die "$! $DBI::errstr";
	my $start_time = Time::HiRes::gettimeofday();
	
	while (my @row = $sth->fetchrow_array()) {
		$row[1] =~ s/\s+$//;    ### The address need triming.

		if ( $Table eq "DataMailingAddress" ) {
			Encode::from_to($row[1], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[2], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[3], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[4], "UTF-8", "iso-8859-1" );
			$DBMailing { $Table } { lc $row[1] } { lc $row[2] } { lc $row[3] } { lc $row[4] } = $row[0];
			
		} elsif ( $Table eq "VotersIndexes" ) {
			$DBDataVoterIdx{$row[1]}{$row[2]}{$row[3]}{uc $row[4]}{$row[5]}{uc $row[6]} = $row[0];

		} elsif ( $Table eq "DataAddress") {			
			$DBDataAddress{uc $row[1]}{lc $row[2]}{lc $row[3]}{$row[4]}{lc $row[5]}{$row[6]}{$row[7]}{lc $row[8]}{lc $row[9]} = $row[0];

		} elsif ( $Table eq "DataDistrict") {	
			if ( $row[1] eq "" || $row[1] == 0) { $Val1 = undef; } else { $Val1 = $row[1]; }
			if ( $row[2] eq "" || $row[2] == 0) { $Val2 = undef; } else { $Val2 = $row[2]; }
			if ( $row[3] eq "" || $row[3] == 0) { $Val3 = undef; } else { $Val3 = $row[3]; }
			if ( $row[4] eq "" || $row[4] == 0) { $Val4 = undef; } else { $Val4 = $row[4]; }
			if ( $row[5] eq "" || $row[5] == 0) { $Val5 = undef; } else { $Val5 = $row[5]; }
			if ( $row[6] eq "" || $row[6] == 0) { $Val6 = undef; } else { $Val6 = lc $row[6]; }
			if ( $row[7] eq "" || $row[7] == 0) { $Val7 = undef; } else { $Val7 = $row[7]; }
			if ( $row[8] eq "" || $row[8] == 0) { $Val8 = undef; } else { $Val8 = $row[8]; }
			if ( $row[9] eq "" || $row[9] == 0) { $Val9 = undef; } else { $Val9 = $row[9]; }
			if ( $row[10] eq "" || $row[10] == 0) { $Val10 = undef; } else { $Val10 = $row[10]; }
			$DBDataDistrict{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9}{$Val10} = $row[0];		
		} elsif ( $Table eq "DataCounty") {		
			$DBDataCounty{$row[1]}{$row[3]} = $row[0];

		} else {
			Encode::from_to($row[1], "UTF-8", "iso-8859-1" );
			$DBData { $Table } { lc $row[1] } = $row[0];			
		}
	}
	
	if ( $Table eq "DataMailingAddress" ) {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (" . $Table . "_Line1," . $Table . "_Line2," . $Table . "_Line3," . $Table . "_Line4) VALUES ";
	} elsif ( $Table eq "DataAddress" ) {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (DataAddress_HouseNumber, DataAddress_FracAddress, DataAddress_PreStreet, DataStreet_ID, DataAddress_PostStreet, DataCity_ID, DataCounty_ID, DataAddress_zipcode, DataAddress_zip4) VALUES ";
	} elsif ( $Table eq "VotersIndexes" ) {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (DataLastName_ID, DataFirstName_ID, DataMiddleName_ID, VotersIndexes_Suffix, VotersIndexes_DOB, VotersIndexes_UniqStateVoterID) VALUES ";
	} elsif ( $Table eq "DataDistrict") {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (DataCounty_ID, DataDistrict_Electoral, DataDistrict_StateAssembly, DataDistrict_StateSenate, DataDistrict_Legislative, DataDistrict_Ward, DataDistrict_Congress,DataDistrict_Council, DataDistrict_CivilCourt, DataDistrict_Judicial) VALUES ";
	}
	$FirstTime[$TableCounter] = 0;
		
	$TableCounter++;
	my $stop_time = Time::HiRes::gettimeofday();
	printf("%.2f\n", $stop_time - $start_time);
} 

my $start_time_total = Time::HiRes::gettimeofday();
my $tabledate = $ARGV[0];
#foreach my $tabledate (@TableDates) {

	my $filename = "/home/usracct/VoterFiles/NY/" . $tabledate . "/AllNYSVoters_" . $tabledate . ".txt";
	my $filename = "/home/usracct/Test/TestData/Random_250/" . $tabledate . ".txt";
	my $FileLastDateSeen = $tabledate;
	print "Working on " . $filename . "\n";

	open(my $fh, "< :encoding(Latin1)", $filename ) or die "cannot open $filename: $!";

	my $FileCounter;
	my $Counter;
	my @TheWholeFile = ();
	my $StringFile = "";
	my $LocalCounter = 0;
	my $counter_start_time;
	
	my $mysql_voteridx = "";

	undef(@TheWholeFile);
	$Counter = 0;
	$FileCounter = 0;
	$LocalCounter = 0;

	my $start_time = Time::HiRes::gettimeofday();
	while (my $row = <$fh>) {	$TheWholeFile[$Counter++] = $row; } # if ($Counter == 20000) { last; }}
	my $stop_time = Time::HiRes::gettimeofday();
	printf("Loading the CD Information in %.2f\n", $stop_time - $start_time);
	print "Loaded into memory $Counter lines\n";

	$start_time = Time::HiRes::gettimeofday();
	my $i;
	
	for ($i = 0; $i < $Counter; $i++) {
		
		my $ArtificialProblem = 0;
		my $csv = Text::CSV->new();
		$csv->always_quote(1);
	
		if ($csv->parse($TheWholeFile[$i]) || $ArtificialProblem == 1) {
	    my @values = $csv->fields();
	    my $status = $csv->combine(@values);
	   	
	   	my $IndexKeyCounter = 7;
	   	my $AddressKeyCounter = 8;
	   	my $DistrictMapCounter = 9;
			my $KeyTurn;
			
			if ( @values == 45 ) { $KeyTurn = 0; }
			elsif ( @values == 47 ) { $KeyTurn = 1; }	
			else {
				print "\n##################################################### Problem with " . @values . " -> $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
			}
				
   		if ( $i < 1 || $i > $Counter) {
    		print "Number of fields: " . @values . "\n"; 	
		 		print "\n##################################################### $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
		 		print "CVS String: " . $csv->string() . "\n\n";
	  	}
	  		  	
			$values[$ValueKey[$KeyTurn][4]] =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; 
			$values[$ValueKey[$KeyTurn][12]] = int($values[$ValueKey[$KeyTurn][12]]);
			
			for (my $j = 0 ; $j < @{$ValueKey[0]} ; $j++) {
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\x{9f}//g;   ### Remove all weird control character.
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\s+$//g;    ### The address need triming.
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\s+/ /g;   ### Remove all white spaces
			}
				
			### This is the do Voter Indexes
			if (! defined $DBDataVoterIdx{$DBData{"DataLastName"}{lc $values[$ValueKey[$KeyTurn][0]]}}{$DBData{"DataFirstName"}{lc $values[$ValueKey[$KeyTurn][1]]}}{$DBData{"DataMiddleName"}{lc $values[$ValueKey[$KeyTurn][2]]}}{uc $values[$ValueKey[$KeyTurn][3]]}{lc $values[$ValueKey[$KeyTurn][4]]}{uc $values[$ValueKey[$KeyTurn][5]]}) {
				if ( $FirstTime[$IndexKeyCounter] == 0 ) { $FirstTime[$IndexKeyCounter] = 1; } else { $sqldata[$IndexKeyCounter] .= ","; }
				if ( $values[$ValueKey[$KeyTurn][3]] eq "" ) {
					$Val1 = "null";
				} else {
					$Val1 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][3]]);
				}
				
				$sqldata[$IndexKeyCounter] .= "(" . $dbh->quote($DBData{"DataLastName"}{lc $values[$ValueKey[$KeyTurn][0]]}) . "," . $dbh->quote($DBData{"DataFirstName"}{lc $values[$ValueKey[$KeyTurn][1]]}) . "," . 
																	$dbh->quote($DBData{"DataMiddleName"}{lc $values[$ValueKey[$KeyTurn][2]]}) . "," . 
																	$Val1 . "," . $dbh->quote(lc $values[$ValueKey[$KeyTurn][4]]) . "," . 
																	$dbh->quote(uc $values[$ValueKey[$KeyTurn][5]]) . ")";				
				$DBDataVoterIdx{$DBData{"DataLastName"}{lc $values[$ValueKey[$KeyTurn][0]]}}{$DBData{"DataFirstName"}{lc $values[$ValueKey[$KeyTurn][1]]}}{$DBData{"DataMiddleName"}{lc $values[$ValueKey[$KeyTurn][2]]}}{uc $values[$ValueKey[$KeyTurn][3]]}{lc $values[$ValueKey[$KeyTurn][4]]}{uc $values[$ValueKey[$KeyTurn][5]]} = "ADD";
			}
		
			#### Now deal Addresses
			if ( $values[$ValueKey[$KeyTurn][6]] eq "") { $Val1 = undef; } else { $Val1 = uc $values[$ValueKey[$KeyTurn][6]]; }
			if ( $values[$ValueKey[$KeyTurn][7]] eq "") { $Val2 = undef; } else { $Val2 = lc $values[$ValueKey[$KeyTurn][7]]; }
			if ( $values[$ValueKey[$KeyTurn][8]] eq "") { $Val3 = undef; } else { $Val3 = lc $values[$ValueKey[$KeyTurn][8]]; }
			if ( $DBData{"DataStreet"}{lc $values[$ValueKey[$KeyTurn][9]]} eq "" ) { $Val4 = undef; } else { $Val4 = $DBData{"DataStreet"}{lc $values[$ValueKey[$KeyTurn][9]]}; }
			if ( $values[$ValueKey[$KeyTurn][10]] eq "") { $Val5 = undef; } else { $Val5 = lc $values[$ValueKey[$KeyTurn][10]]; }
			if ( $DBData{"DataCity"}{lc $values[$ValueKey[$KeyTurn][11]]} eq "" ) { $Val6 = undef; } else { $Val6 = $DBData{"DataCity"}{lc $values[$ValueKey[$KeyTurn][11]]}; }
			if ( $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]} eq "") { $Val7 = undef; } else { $Val7 = $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]}; }
			if ( $values[$ValueKey[$KeyTurn][13]] eq "") { $Val8 = undef; } else { $Val8 =lc $values[$ValueKey[$KeyTurn][13]]; }
			if ( $values[$ValueKey[$KeyTurn][14]] eq "") { $Val9 = undef; } else { $Val9 =lc $values[$ValueKey[$KeyTurn][14]]; }
			
			

			if (! defined $DBDataAddress{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9}) {
				$DBDataAddress{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9} = "ADD";
				if ( $values[$ValueKey[$KeyTurn][6]] eq "") { $Val1 = 'null'; } else { $Val1 = $dbh->quote(uc $values[$ValueKey[$KeyTurn][6]]); }
				if ( $values[$ValueKey[$KeyTurn][7]] eq "") { $Val2 = 'null'; } else { $Val2 = $dbh->quote(lc $values[$ValueKey[$KeyTurn][7]]); }
				if ( $values[$ValueKey[$KeyTurn][8]] eq "") { $Val3 = 'null'; } else { $Val3 = $dbh->quote(lc $values[$ValueKey[$KeyTurn][8]]); }
				if ( $DBData{"DataStreet"}{lc $values[$ValueKey[$KeyTurn][9]]} eq "" ) { $Val4 = 'null'; } else { $Val4 = $dbh->quote($DBData{"DataStreet"}{lc $values[$ValueKey[$KeyTurn][9]]}); }
				if ( $values[$ValueKey[$KeyTurn][10]] eq "") { $Val5 = 'null'; } else { $Val5 = $dbh->quote(lc $values[$ValueKey[$KeyTurn][10]]); }
				if ( $DBData{"DataCity"}{lc $values[$ValueKey[$KeyTurn][11]]} eq "" ) { $Val6 = 'null'; } else { $Val6 = $dbh->quote($DBData{"DataCity"}{lc $values[$ValueKey[$KeyTurn][11]]}); }
				if ( $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]} eq "") { $Val7 = 'null'; } else { $Val7 = $dbh->quote($DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]}); }
				if ( $values[$ValueKey[$KeyTurn][13]] eq "") { $Val8 = 'null'; } else { $Val8 = $dbh->quote(lc $values[$ValueKey[$KeyTurn][13]]); }
				if ( $values[$ValueKey[$KeyTurn][14]] eq "") { $Val9 = 'null'; } else { $Val9 = $dbh->quote(lc $values[$ValueKey[$KeyTurn][14]]); }
				
				if ( $FirstTime[$AddressKeyCounter] == 0 ) { $FirstTime[$AddressKeyCounter] = 1; } else { $sqldata[$AddressKeyCounter] .= ","; }
				$sqldata[$AddressKeyCounter] .= "(".$Val1.",".$Val2.",".$Val3.",".$Val4.",".$Val5.",".$Val6.",".$Val7.",".$Val8.",".$Val9.")";				
				
			}

			#	  	my @ValueKey = ([ 0, 1, 2, 3, 17, 43, 4, 5, 7, 8, 9, 10, 21, 11, 12, 22, 28, 27, 23, 25, 26],
			#                       0  1  2  3  4  5    6  7  8  9  10 11  12  13  14  15  16  17  18  19  20  21			
			#### Now deal districts
		
			
			
			if ( $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]} eq "") { $Val1 = undef; } else { $Val1 = $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]}; }
			if ( $values[$ValueKey[$KeyTurn][15]] eq "" || $values[$ValueKey[$KeyTurn][15]] == 0) { $Val2 = undef; } else { $Val2 = uc $values[$ValueKey[$KeyTurn][15]]; }
			if ( $values[$ValueKey[$KeyTurn][16]] eq "" || $values[$ValueKey[$KeyTurn][16]] == 0) { $Val3 = undef; } else { $Val3 = lc $values[$ValueKey[$KeyTurn][16]]; }
			if ( $values[$ValueKey[$KeyTurn][17]] eq "" || $values[$ValueKey[$KeyTurn][17]] == 0) { $Val4 = undef; } else { $Val4 = lc $values[$ValueKey[$KeyTurn][17]]; }
			if ( $values[$ValueKey[$KeyTurn][18]] eq "" || $values[$ValueKey[$KeyTurn][18]] == 0) { $Val5 = undef; } else { $Val5 =lc $values[$ValueKey[$KeyTurn][18]]; }
			if ( $values[$ValueKey[$KeyTurn][19]] eq "" || $values[$ValueKey[$KeyTurn][19]] == 0) { $Val6 = undef; } else { $Val6 =lc $values[$ValueKey[$KeyTurn][19]]; }
			if ( $values[$ValueKey[$KeyTurn][20]] eq "" || $values[$ValueKey[$KeyTurn][20]] == 0) { $Val7 = undef; } else { $Val7 = uc $values[$ValueKey[$KeyTurn][20]]; }
			
			$Val8 = undef;
			$Val9 = undef;
			$Val10 = undef;
					
			if (! defined $DBDataDistrict{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9}{$Val10}) {
				$DBDataDistrict{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9}{$Val10} = "ADD";
				
				if ( $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]} eq "") { $Val1 = 'null'; } else { $Val1 = $dbh->quote($DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]}); }
				if ( $values[$ValueKey[$KeyTurn][15]] eq "" || $values[$ValueKey[$KeyTurn][15]] == 0) { $Val2 = 'null'; } else { $Val2 = $dbh->quote($values[$ValueKey[$KeyTurn][15]]); }
				if ( $values[$ValueKey[$KeyTurn][16]] eq "" || $values[$ValueKey[$KeyTurn][16]] == 0) { $Val3 = 'null'; } else { $Val3 = $dbh->quote($values[$ValueKey[$KeyTurn][16]]); }
				if ( $values[$ValueKey[$KeyTurn][17]] eq "" || $values[$ValueKey[$KeyTurn][17]] == 0) { $Val4 = 'null'; } else { $Val4 = $dbh->quote($values[$ValueKey[$KeyTurn][17]]); }
				if ( $values[$ValueKey[$KeyTurn][18]] eq "" || $values[$ValueKey[$KeyTurn][18]] == 0) { $Val5 = 'null'; } else { $Val5 = $dbh->quote($values[$ValueKey[$KeyTurn][18]]); }
				if ( $values[$ValueKey[$KeyTurn][19]] eq "" || $values[$ValueKey[$KeyTurn][19]] == 0) { $Val6 = 'null'; } else { $Val6 = $dbh->quote(uc $values[$ValueKey[$KeyTurn][19]]); }
				if ( $values[$ValueKey[$KeyTurn][20]] eq "" || $values[$ValueKey[$KeyTurn][20]] == 0) { $Val7 = 'null'; } else { $Val7 = $dbh->quote($values[$ValueKey[$KeyTurn][20]]); }
	
				if ( $FirstTime[$DistrictMapCounter] == 0 ) { $FirstTime[$DistrictMapCounter] = 1; } else { $sqldata[$DistrictMapCounter] .= ","; }
				$sqldata[$DistrictMapCounter] .= "(".$Val1.",".$Val2.",".$Val3.",".$Val4.",".$Val5.",".$Val6.",".$Val7.",null,null,null)";
				
			}		
					
			for (my $j = 7; $j < 10; $j++) {		
				if ( ($i % $InsertDBEachCount) == 0 && $i > 0 && length($sqldata[$j]) > 0 ) {
					my $db_start_time = Time::HiRes::gettimeofday();
					print "\n##################################################### $i ####################################\n";
					print "Counter: $Counter - LocalCounter: $LocalCounter - DBAddCounter: " . $DBAddCounter[$j] . "\n";
					print "Line $i: " . $TheWholeFile[$i] . "\n";
					print "CVS String: " . $csv->string() . "\n\n";
					print "Table Name: " . $TableNames[$j] . "\n"; 
					#print "SQL: " . $sqlintro[$j] . $sqldata[$j]  . "\n";
					
					my $sth = $dbh->prepare( $sqlintro[$j] . $sqldata[$j]  ); 
					$sth->execute() or die "$! $DBI::errstr " . $sqlintro[$j] . $sqldata[$j];	
					$FirstTime[$j] = 0;
					$sqldata[$j] = "";
					
					print "\n";
				}
			}
		}
	}
	
#}
			   	

print "\n##################################################### FINAL LOOP ####################################\n";

for (my $j = 7; $j < 10; $j++) {		
	if ( length($sqldata[$j]) > 0 ) {
		my $db_start_time = Time::HiRes::gettimeofday();
		
		print "Counter: $Counter - LocalCounter: $LocalCounter - DBAddCounter: " . $DBAddCounter[$j] . "\n";
		print "Table Name: " . $TableNames[$j] . "\n"; 
		#print "SQL: " . $sqlintro[$j] . $sqldata[$j]  . "\n";
		
		my $sth = $dbh->prepare( $sqlintro[$j] . $sqldata[$j]  ); 
		$sth->execute() or die "$! $DBI::errstr " . $sqlintro[$j] . $sqldata[$j];	
		$FirstTime[$j] = 0;
		$sqldata[$j] = "";
		
		print "\n";
	}
}							



exit();