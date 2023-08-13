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
my %DBDataAddress = ();
my %DBDataHouse = ();
my %DBDataCounty = ();
my %DBVoterComplement = ();


my $Val1; my $Val2; my $Val3; my $Val4; my $Val5; my $Val6; my $Val7; my $Val8; my $Val9; my $Val10;

my $tabledate = $ARGV[0];
#foreach my $tabledate (@TableDates) {

	my $filename = "/home/usracct/VoterFiles/NY/" . $tabledate . "/AllNYSVoters_" . $tabledate . ".txt";
	my $filename = "/home/usracct/Test/TestData/Random_250/" . $tabledate . ".txt";
	my $FileLastDateSeen = $tabledate;
	print "Working on " . $filename . "\n";

	open(my $fh, "< :encoding(Latin1)", $filename ) or die "cannot open $filename: $!";

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
#	[0] 0 -> LastName -> varchar(50)						#	[1] 1 -> FirstName -> varchar(50)				#	[2] 2 -> MiddleName -> varchar(50)			#	[3] 3 -> Suffix -> varchar(10)
#	[5] 4 -> ResHouseNumber -> varchar(10)			#	[6] 5 -> ResFracAddress -> varchar(10)	#	[7] 6 -> ResPreStreet -> varchar(10)		#	[8] 7 -> ResStreetName -> varchar(70)
#	[9] 8 -> ResPostStDir -> varchar(10)				#	[29] 9 -> ResType -> varchar(10)  			#	[26] 10 -> ResApartment -> varchar(15)	#	[30] 11 -> ResNonStdFormat -> varchar(250)
#	[10] 12 -> ResCity -> varchar(50)						#	[12] 13 -> ResZip -> char(5)     				#	[13] 14 -> ResZip4 -> char(4)   		   	#	15 -> ResMail1 -> varchar(100)
#	16 -> ResMail2 -> varchar(100)     					#	17 -> ResMail3 -> varchar(100)     			#	18 -> ResMail4 -> varchar(100)					#	[4] 19 -> DOB -> char(8)
#	20 -> Gender -> char(1)      								#	21 -> EnrollPolParty -> char(3)					#	[25] 22 -> OtherParty -> varchar(30)	  #	[11] 23 -> CountyCode -> char(2)
#	[14] 24 -> ElectDistr -> char(3)						#	[17] 25 -> LegisDistr -> char(3)    		#	[26] 26 -> TownCity -> varchar(30)   		#	[18] 27 -> Ward -> char(3)
#	[19] 28 -> CongressDistr -> char(3)					#	[16] 29 -> SenateDistr -> char(3)  			#	[15] 30 -> AssemblyDistr -> char(3) 		#	[23] 31 -> LastDateVoted -> char(8)
#	[27] 32 -> PrevYearVoted -> varchar(4)   		#	[22] 33 -> PrevCounty -> char(2)   			#	[21] 34 -> PrevAddress -> varchar(100)	#	[20] 35 -> PrevName -> varchar(150)
#	36 -> CountyVoterNumber -> varchar(50)			#	37 -> RegistrationCharacter -> char(8)	#	38 -> ApplicationSource -> varchar(10)	#	39 -> IDRequired -> char(1)    				
#	40 -> IDMet -> char(1)        							#	41 -> Status -> varchar(10)      				#	42 -> ReasonCode -> varchar(15)					#	43 -> VoterMadeInactive -> char(8)		
#	44 -> VoterPurged -> char(8)  							#	[] 45 -> UniqNYSVoterID -> varchar(50)	#	46 -> VoterHistory -> text

#                 0  1  2  3   4   5  6  7  8  9 10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29     30     
my @ValueKey = ([ 0, 1, 2, 3, 17, 43, 4, 5, 7, 8, 9, 10, 21, 11, 12, 22, 28, 27, 23, 25, 26, 33, 32, 31, 29, 20,  6, 24, 30, undef, undef],
								[ 0, 1, 2, 3, 19, 45, 4, 5, 6, 7, 8, 12, 23, 13, 14, 24, 30, 29, 25, 27, 28, 35, 34, 33, 31, 22, 10, 26, 32, 9,     11] );

my @TableNames  = qw/DataHouse VotersComplementInfo DataAddress DataStreet DataCity DataDistrictTown DataCounty DataStreetNonStdFormat/;
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
		
		# this is Voters Table
		if ( $Table eq "DataAddress") {			
			$DBDataAddress{uc $row[1]}{lc $row[2]}{lc $row[3]}{$row[4]}{lc $row[5]}{$row[6]}{$row[7]}{lc $row[8]}{lc $row[9]} = $row[0];
		} elsif ( $Table eq "DataCounty") {		
			$DBDataCounty{$row[1]}{$row[3]} = $row[0];
		} elsif ( $Table eq "DataHouse") {		
			$DBDataHouse{$row[1]}{lc $row[2]}{lc $row[3]}{$row[4]}{$row[5]} = $row[0];
		} elsif ($Table eq "VotersComplementInfo") {
			Encode::from_to($row[1], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[2], "UTF-8", "iso-8859-1" );
			$DBVoterComplement{lc $row[1]}{lc $row[2]}{lc $row[3]}{$row[4]}{lc $row[5]}{lc $row[6]} = $row[0];
		} else {
			Encode::from_to($row[1], "UTF-8", "iso-8859-1" );
			$DBData { $Table } { lc $row[1] } = $row[0];			
		}
	}
	
	if ( $Table eq "DataHouse" ) {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (DataAddress_ID,DataHouse_Type,DataHouse_Apt,DataDistrictTown_ID,DataStreetNonStdFormat_ID) VALUES ";
	} 
	$FirstTime[$TableCounter] = 0;
		
	$TableCounter++;
	my $stop_time = Time::HiRes::gettimeofday();
	printf("%.2f\n", $stop_time - $start_time);
} 

my $start_time_total = Time::HiRes::gettimeofday();


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
		
		$TheWholeFile[$i] =~ s/\x{9f}//g;   ### Remove all weird control character.
	
		if ($csv->parse($TheWholeFile[$i]) || $ArtificialProblem == 1) {
	    my @values = $csv->fields();
	    my $status = $csv->combine(@values);
	   	
	   	my $HouseKeyCounter = 0;
	   	my $ComplementCounter = 1;
			my $KeyTurn;
			
			if ( @values == 45 ) { $KeyTurn = 0; }
			elsif ( @values == 47 ) { $KeyTurn = 1; }	
			else {
				print "\n##################################################### Problem with " . @values . " -> $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
			}

			$values[$ValueKey[$KeyTurn][12]] = int($values[$ValueKey[$KeyTurn][12]]);   ### To remove the leading 0 on the County
			for (my $j = 0 ; $j < @{$ValueKey[0]} ; $j++) {
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\s+$//g;    ### The address need triming.
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\s+/ /g;   ### Remove all white spaces
			}
			
   		if ( $i < 1 || $i > $Counter) {
    		print "Number of fields: " . @values . "\n"; 	
		 		print "\n##################################################### $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
		 		print "CVS String: " . $csv->string() . "\n\n";
		 		print "Keyturn : $KeyTurn\n";
	  	}
	  	
	  	$values[4] =~ s/\s+$//; 
			if ( $values[$ValueKey[$KeyTurn][6]] eq "") { $Val1 = undef; } else { $Val1 = uc $values[$ValueKey[$KeyTurn][6]]; }
			if ( $values[$ValueKey[$KeyTurn][7]] eq "") { $Val2 = undef; } else { $Val2 = lc $values[$ValueKey[$KeyTurn][7]]; }
			if ( $values[$ValueKey[$KeyTurn][8]] eq "") { $Val3 = undef; } else { $Val3 = lc $values[$ValueKey[$KeyTurn][8]]; }
			if ( $DBData{"DataStreet"}{lc $values[$ValueKey[$KeyTurn][9]]} eq "" ) { $Val4 = undef; } else { $Val4 = $DBData{"DataStreet"}{lc $values[$ValueKey[$KeyTurn][9]]}; }
			if ( $values[$ValueKey[$KeyTurn][10]] eq "") { $Val5 = undef; } else { $Val5 = lc $values[$ValueKey[$KeyTurn][10]]; }
			if ( $DBData{"DataCity"}{lc $values[$ValueKey[$KeyTurn][11]]} eq "" ) { $Val6 = undef; } else { $Val6 = $DBData{"DataCity"}{lc $values[$ValueKey[$KeyTurn][11]]}; }
			if ( $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]} eq "") { $Val7 = undef; } else { $Val7 = $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][12]]}; }
			if ( $values[$ValueKey[$KeyTurn][13]] eq "") { $Val8 = undef; } else { $Val8 =lc $values[$ValueKey[$KeyTurn][13]]; }
			if ( $values[$ValueKey[$KeyTurn][14]] eq "") { $Val9 = undef; } else { $Val9 =lc $values[$ValueKey[$KeyTurn][14]]; }
			

			
			#### Now deal House
			if ( $DBDataAddress{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9} eq "" ) { 
				
				print "\033[1;31mProblem with House\033[0;m\n";
				print "\tFound DBDataAddress_ID : " . $DBDataAddress{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9} . "\n";
				print "\tFound DataDistrict Town: " . $DBData{"DataDistrictTown"}{lc $values[$ValueKey[$KeyTurn][27]]} . "\n";
				print "\tProblem with DBDadress {".$Val1."}{".$Val2."}{".$Val3."}{".$Val4."}{".$Val5."}{".$Val6."}{".$Val7."}{".$Val8."}{".$Val9."} returned NULL\n";

    		print "\tNumber of fields: " . @values . "\n"; 	
		 		print "\n##################################################### $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
		 		print "CVS String: " . $csv->string() . "\n\n";
				exit 137;
				
			} else { $Val1 = $DBDataAddress{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}{$Val7}{$Val8}{$Val9}; }
			
			if ( defined $ValueKey[$KeyTurn][29] ) {
				if ( $values[$ValueKey[$KeyTurn][29]] eq "") { $Val2 = undef; } else { $Val2 = lc $values[$ValueKey[$KeyTurn][29]]; }
			} else {
				$Val2 = undef;
			}
			if ( $values[$ValueKey[$KeyTurn][26]] eq "") { $Val3 = undef; } else { $Val3 = lc $values[$ValueKey[$KeyTurn][26]]; }
			if ( $DBData{"DataDistrictTown"}{lc $values[$ValueKey[$KeyTurn][27]]} eq "" ) { $Val4 = undef; } else { $Val4 = $DBData{"DataDistrictTown"}{lc $values[$ValueKey[$KeyTurn][27]]}; }
			
			if ( defined $ValueKey[$KeyTurn][30] ) {
				if ( $DBData{"DataStreetNonStdFormat"}{lc $values[$ValueKey[$KeyTurn][30]]} eq "" ) { $Val5 = undef; } else { $Val5= $DBData{"DataStreetNonStdFormat"}{lc $values[$ValueKey[$KeyTurn][30]]}; }				
			} else {
				$Val5 = undef;
			}
				
			if (! defined $DBDataHouse{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}) {
				$DBDataHouse{$Val1}{$Val2}{$Val3}{$Val4}{$Val5} = "ADD";									
				$Val1 = $dbh->quote($Val1);
				
				if ( defined $ValueKey[$KeyTurn][29] ) {
					if ( $values[$ValueKey[$KeyTurn][29]] eq "") { $Val2 = "null"; } else { $Val2 = $dbh->quote(uc $values[$ValueKey[$KeyTurn][29]]); }
				} else {
					 $Val2 = "null";
				}
				if ( $values[$ValueKey[$KeyTurn][26]] eq "") { $Val3 = "null"; } else { $Val3 = $dbh->quote(uc $values[$ValueKey[$KeyTurn][26]]); }
				if ( $DBData{"DataDistrictTown"}{lc $values[$ValueKey[$KeyTurn][27]]} eq "" ) { $Val4 = "null"; } else { $Val4 = $dbh->quote($DBData{"DataDistrictTown"}{lc $values[$ValueKey[$KeyTurn][27]]}); }
				
				if ( defined $ValueKey[$KeyTurn][30] ) {
					if ( $DBData{"DataStreetNonStdFormat"}{lc $values[$ValueKey[$KeyTurn][30]]} eq "" ) { $Val5 = "null"; } else { $Val5 = $dbh->quote($DBData{"DataStreetNonStdFormat"}{lc $values[$ValueKey[$KeyTurn][30]]}); }
				} else {
					 $Val5 = "null";
				}
				if ( $FirstTime[$HouseKeyCounter] == 0 ) { $FirstTime[$HouseKeyCounter] = 1; } else { $sqldata[$HouseKeyCounter] .= ","; }
				$sqldata[$HouseKeyCounter] .= "(". $Val1 .",". $Val2 .",". $Val3 . ",". $Val4 . "," . $Val5 . ")";				
				$DBAddCounter[$HouseKeyCounter]++;
			}
			
	  	#### Need to find the mailing address
	  	$values[$ValueKey[$KeyTurn][24]] =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; 	  	
	  	
	  	if ( $values[$ValueKey[$KeyTurn][21]] eq "") { $Val1 = undef; } else { $Val1 = lc $values[$ValueKey[$KeyTurn][21]]; }
			if ( $values[$ValueKey[$KeyTurn][22]] eq "") { $Val2 = undef; } else { $Val2 = lc $values[$ValueKey[$KeyTurn][22]]; }
			if ( $values[$ValueKey[$KeyTurn][23]] eq "") { $Val3 = undef; } else { $Val3 = $DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][23]]}; }
			if ( $values[$ValueKey[$KeyTurn][28]] eq "") { $Val4 = undef; } else { $Val4 = lc $values[$ValueKey[$KeyTurn][28]]; }
			if ( $values[$ValueKey[$KeyTurn][24]] eq "") { $Val5 = undef; } else { $Val5 = $values[$ValueKey[$KeyTurn][24]]; }
			if ( $values[$ValueKey[$KeyTurn][25]] eq "") { $Val6 = undef; } else { $Val6 = lc $values[$ValueKey[$KeyTurn][25]]; }

			if ((! defined $DBVoterComplement{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6}) && 
						(defined $Val1 || defined $Val2 || defined $Val3 || defined $Val4 || defined $Val5 || defined $Val6)) {
				$DBVoterComplement{$Val1}{$Val2}{$Val3}{$Val4}{$Val5}{$Val6} = "ADD";							

				if ( $values[$ValueKey[$KeyTurn][21]] eq "") { $Val1 = "null"; } else { $Val1 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][21]]); }
				if ( $values[$ValueKey[$KeyTurn][22]] eq "") { $Val2 = "null"; } else { $Val2 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][22]]); }
				if ( $values[$ValueKey[$KeyTurn][23]] eq "") { $Val3 = "null"; } else { $Val3 = $dbh->quote($DBDataCounty{$SystemState}{$values[$ValueKey[$KeyTurn][23]]}); }
				if ( $values[$ValueKey[$KeyTurn][28]] eq "") { $Val4 = "null"; } else { $Val4 = $dbh->quote(uc $values[$ValueKey[$KeyTurn][28]]); }
				if ( $values[$ValueKey[$KeyTurn][24]] eq "") { $Val5 = "null"; } else { $Val5 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][24]]); }
				if ( $values[$ValueKey[$KeyTurn][25]] eq "") { $Val6 = "null"; } else { $Val6 = $dbh->quote($values[$ValueKey[$KeyTurn][25]]); }
			}
			
			for (my $j = 0; $j < 2; $j++) {		
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

for (my $j = 0; $j < 2; $j++) {		
	if ( length($sqldata[$j]) > 0 ) {
		my $db_start_time = Time::HiRes::gettimeofday();
		print "\n##################################################### FINAL LOOP $DBAddCounter[$j] ####################################\n";
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