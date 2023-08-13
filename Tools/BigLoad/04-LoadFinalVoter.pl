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

##### The Date of the File
my $lastdate = $ARGV[0];
$lastdate =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; 	

### Load all the Datas
my %DBData = ();
my %DBDataAddress = ();
my %DBDataHouse = ();
my %DBDataCounty = ();
my %DBVoter = ();
my %DBVoterIdx = ();
my %DBMailingAddress = ();
my %DBVoterComplement = ();
my %DBDataDistrict = ();
my %DBDataTemporal = ();

##### THIS IS THE PRE 2022 FIELDS 	
#	NY_Raw_ID


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

my @ValueKey = ([0,1,2,3,4,5, 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46],
								[0,1,2,3,4,5,10,6,7,8,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46, 9,11]);

### This is to be able to debug the data correctly based on the information in the Raw Voter File.
my @ValueKeyDesc = (["LastName","FirstName","MiddleName","Suffix","ResHouseNumber","ResFracAddress","ResApartment","ResPreStreet",
											"ResStreetName","ResPostStDir","ResCity","ResZip","ResZip4","ResMail1","ResMail2","ResMail3",
											"ResMail4","DOB","Gender","EnrollPolParty","OtherParty","CountyCode","ElectDistr","LegisDistr",
											"TownCity","Ward","CongressDistr","SenateDistr","AssemblyDistr","LastDateVoted","PrevYearVoted","PrevCounty",
											"PrevAddress","PrevName","CountyVoterNumber","RegistrationCharacter","ApplicationSource","IDRequired","IDMet","Status",
											"ReasonCode","VoterMadeInactive","VoterPurged","UniqNYSVoterID","VoterHistory"], 
										["LastName","FirstName","MiddleName","Suffix","ResHouseNumber","ResFracAddress","ResPreStreet","ResStreetName",
											"ResPostStDir","ResType","ResApartment","ResNonStdFormat","ResCity","ResZip","ResZip4","ResMail1",
											"ResMail2","ResMail3","ResMail4","DOB","Gender","EnrollPolParty","OtherParty","CountyCode",
											"ElectDistr","LegisDistr","TownCity","Ward","CongressDistr","SenateDistr","AssemblyDistr","LastDateVoted",
											"PrevYearVoted","PrevCounty","PrevAddress","PrevName","CountyVoterNumber","RegistrationCharacter","ApplicationSource",
											"IDRequired",	"IDMet","Status","ReasonCode","VoterMadeInactive","VoterPurged","UniqNYSVoterID","VoterHistory"]);
	
	
my @TableNames  = qw/Voters DataDistrictTemporal VotersIndexes DataDistrict  DataFirstName DataLastName DataMiddleName  DataHouse VotersComplementInfo DataMailingAddress DataCounty DataAddress DataStreet DataCity DataDistrictTown DataStreetNonStdFormat/;
my $LocalLimit; # = " LIMIT 771600;";
#y $LocalLimit = " LIMIT 10;";

my $TableCounter = 0;
my @stmts = ();
my @sqlintro = ();
my @sqldata = ();
my @FirstTime = ();
my @DBAddCounter = ();

### These variable are to update the Voter Last Seen Date
my @VoterIDSeenInFile = undef;
my @VoterFistSeen = undef;
my @VoterLastSeen = undef;

my $start_time_total = Time::HiRes::gettimeofday();
my $tabledate = $ARGV[0];
my $filename = "/home/usracct/VoterFiles/NY/" . $tabledate . "/AllNYSVoters_" . $tabledate . ".txt";
my $filename = "/home/usracct/Test/TestData/Random_250/" . $tabledate . ".txt";
my $FileLastDateSeen = $tabledate;
print "\nWorking on " . $filename . "\n";
open(my $fh, "< :encoding(Latin1)", $filename ) or die "cannot open $filename: $!";

#my $TestVoterUNIQID = "NY000000000008508435";
my $TestVoterUNIQID = ""; #NY000000000023728122";

### Find The Data Cycle
my $sqlDataCycle = "SELECT * FROM DataDistrictCycle WHERE " .  		
  									"DATE(\"" . $tabledate . "\") >= DataDistrictCycle_CycleStartDate " . 
  									"AND DATE(\"" . $tabledate . "\") <= DataDistrictCycle_CycleEndDate";
my $sth = $dbh->prepare($sqlDataCycle); $sth->execute() or die "$! $DBI::errstr";
my @row = $sth->fetchrow_array();
my $DataCycle = $row[0];

if ( ! defined $DataCycle ) {
	$sqlDataCycle = "SELECT * FROM DataDistrictCycle WHERE DataDistrictCycle_CycleEndDate IS NULL";
	my $sth = $dbh->prepare($sqlDataCycle); $sth->execute() or die "$! $DBI::errstr";
	my @row = $sth->fetchrow_array();
	$DataCycle = $row[0];
}

if ( ! defined $DataCycle ) {
	print "We have a problem $DataCycle finding the correct datacycle. Check the database DataDistrictCycle Table\n";
	exit();
}

#### LoadDataFromFile
foreach my $Table (@TableNames) { 
	printf("Loading %-20s\t", $Table); 
	my $Stmt_FirstName = "SELECT * FROM " . $Table  . " " . $LocalLimit;
	my $sth = $dbh->prepare( $Stmt_FirstName ); $sth->execute() or die "$! $DBI::errstr";
	my $start_time = Time::HiRes::gettimeofday();
	
	while (my @row = $sth->fetchrow_array()) {
		$row[1] =~ s/\s+$//;    ### The address need triming.
	
		if ( $Table eq "Voters" ) {
				
			$DBVoter{$row[1]}{$row[2]}{$row[3]}{$row[4]}{$row[5]}{$row[6]}{$row[7]}{$row[8]}{$row[9]}{$row[10]}{$row[11]}{$row[12]}{$row[13]}{$row[14]}{$row[15]}{$row[16]} = $row[0];
			$VoterIDSeenInFile[$row[0]] = 1;
			$VoterFistSeen[$row[0]] = $row[17];
			$VoterLastSeen[$row[0]] = $row[18];
			
#			# This is to debug.
#			if ($row[5] eq $TestVoterUNIQID) {
#				for (my $i = 0; $i < 20; $i++) { print "\tRow[$i]: #" . $row[$i] . "#\n";	}
#				print "\n\t\$DBVoter{" . $row[1] . "}{" . $row[2] . "}{" .  $row[3] . "}{" . $row[4] . "}{" . $row[5] . "}{" . 
#																 $row[6] . "}{" . $row[7] . "}{" .  $row[8] . "}{" . $row[9] . "}{" . $row[10] . "}{" . 
#																 $row[11] . "}{" . $row[12] . "}{" .  $row[13] . "}{" . $row[14] . "}{" . $row[15] . "}{" . 
#																 $row[16] . "}{" . $row[17] . "}\n";
#			}

			
		} elsif ( $Table eq "DataAddress") {	
			$DBDataAddress{lc $row[1]}{lc $row[2]}{lc $row[3]}{$row[4]}{lc $row[5]}{$row[6]}{$row[7]}{lc $row[8]}{lc $row[9]} = $row[0];
			
		} elsif ( $Table eq "DataCounty") {
			$DBDataCounty{$row[1]}{$row[3]} = $row[0];
			
		} elsif ( $Table eq "DataHouse") {		
			$DBDataHouse{$row[1]}{lc $row[2]}{lc $row[3]}{$row[4]}{$row[5]} = $row[0];
			
		} elsif ( $Table eq "VotersIndexes") {
			$DBVoterIdx {lc $row[1]}{lc $row[2]}{lc $row[3]}{lc $row[4]}{$row[5]}{uc $row[6]} = $row[0];
			
			# This is to debug.
			if ($row[6] eq $TestVoterUNIQID) {
				for (my $i = 0; $i < 7; $i++) { print "\tRow[$i]: #" . $row[$i] . "#\n";	}
				print "\n\t\$VotersIndexes{" . $row[1] . "}{" . $row[2] . "}{" .  $row[3] . "}{" . $row[4] . "}{" . $row[5] . "}{" .  $row[6] . "}\n";
			}
			
		} elsif ( $Table eq "DataDistrict" ) {
			$DBDataDistrict{$row[1]}{$row[2]}{$row[3]}{$row[4]}{$row[5]}{lc $row[6]}{$row[7]} = $row[0];
			
		} elsif ( $Table eq "DataDistrictTemporal" ) {
			$DBDataTemporal{$row[1]}{$row[2]}{$row[3]} = $row[0];
			
		} elsif ( $Table eq "DataMailingAddress") {
			Encode::from_to($row[1], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[2], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[3], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[4], "UTF-8", "iso-8859-1" );
			$DBMailingAddress {lc $row[1]}{lc $row[2]}{lc $row[3]}{lc $row[4]} = $row[0];
			
		} elsif ($Table eq "VotersComplementInfo") {
			Encode::from_to($row[2], "UTF-8", "iso-8859-1" );
			Encode::from_to($row[3], "UTF-8", "iso-8859-1" );
			$DBVoterComplement{$row[1]}{lc $row[2]}{lc $row[3]}{lc $row[4]}{$row[5]}{lc $row[6]}{lc $row[7]} = $row[0];
			
		} else {
			Encode::from_to($row[1], "UTF-8", "iso-8859-1" );
			$DBData { $Table } { lc $row[1] } = $row[0];					
		}
	}
	
	
	if ( $Table eq "Voters" ) {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (VotersIndexes_ID,DataHouse_ID,Voters_Gender,Voters_UniqStateVoterID," . 
																														"Voters_RegParty,Voters_ReasonCode,Voters_Status,VotersMailingAddress_ID,Voters_IDRequired," . 
																														"Voters_IDMet,Voters_ApplyDate,Voters_RegSource,Voters_DateInactive,Voters_DatePurged," . 
																														"Voters_CountyVoterNumber,Voters_RMBActive,Voters_RecFirstSeen,Voters_RecLastSeen) VALUES ";
	}	elsif ( $Table eq "DataDistrictTemporal" ) {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (DataDistrictCycle_ID, DataHouse_ID, DataDistrict_ID) VALUES ";
	}
	
	$FirstTime[$TableCounter] = 0;
		
	$TableCounter++;
	my $stop_time = Time::HiRes::gettimeofday();
	printf("%.2f\n", $stop_time - $start_time);
} 

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
		
		#print "Processign Counter: $i";
		
		my $ArtificialProblem = 0;
		my $csv = Text::CSV->new();
		$csv->always_quote(1);
	
		if ($csv->parse($TheWholeFile[$i]) || $ArtificialProblem == 1) {
	    my @values = $csv->fields();
	    my $status = $csv->combine(@values);
	   	
	   	my $HouseKeyCounter = 0;
			my $KeyTurn;
			
			if ( @values == 45 ) { $KeyTurn = 0; }
			elsif ( @values == 47 ) { $KeyTurn = 1; }	
			else {
				print "\n##################################################### Problem with " . @values . " -> $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
			}
				
   		if ( $i <  1 || $i > $Counter) {
    		print "Number of fields: " . @values . "\n"; 	
		 		print "\n##################################################### $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
		 		print "CVS String: " . $csv->string() . "\n\n";
	  	}

	  	$values[$ValueKey[$KeyTurn][21]] = int($values[$ValueKey[$KeyTurn][21]]);   ### To remove the leading 0 on the County
	  	for (my $j = 0 ; $j < @{$ValueKey[0]} ; $j++) {
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\x{9f}//g;   ### Remove all weird control character.
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\s+$//g;    ### The address need triming.
				$values[$ValueKey[$KeyTurn][$j]] =~ s/\s+/ /g;   ### Remove all white spaces
			}
			
		
	  	
	  	my $VotersIndex_ID = "";
	  	my $DataHouse_ID = "";
	  	my $VotersIndexes_ID = "";
	  	my $VotersComplementInfo_ID = "";
	  	my $VotersMailingAddress_ID = "";
		
			my $valvotdob = 						$values[$ValueKey[$KeyTurn][17]]; if ($valvotdob eq "") { $valvotdob = undef; } else { $valvotdob =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; }
			my $valvotinactive = 				$values[$ValueKey[$KeyTurn][41]]; if ($valvotinactive eq "") { $valvotinactive = undef; } else { $valvotinactive =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; }		
			my $valvotpurged = 					$values[$ValueKey[$KeyTurn][42]]; if ($valvotpurged eq "") { $valvotpurged = undef; } else { $valvotpurged =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; }
			my $valregchar = 						$values[$ValueKey[$KeyTurn][35]]; if ($valregchar eq "") { $valregchar = undef; } else { $valregchar =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; } 
			my $valprevdatevoted = 			$values[$ValueKey[$KeyTurn][29]];	if ($valprevdatevoted eq "") { $valprevdatevoted = undef; } else { $valprevdatevoted =~ s/(\d{4})(\d{2})(\d{2})/$1-$2-$3/; } 

			my $vallastnameid = 				lc $values[$ValueKey[$KeyTurn][0]]; if ($vallastnameid eq "") { $vallastnameid = undef; } else { $vallastnameid = $DBData{"DataLastName"}{$vallastnameid} };
			my $valfirstnameid = 				lc $values[$ValueKey[$KeyTurn][1]]; if ($valfirstnameid eq "") { $valfirstnameid = undef; } else { $valfirstnameid = $DBData{ "DataFirstName"}{$valfirstnameid} };
			my $valmiddlenameid = 			lc $values[$ValueKey[$KeyTurn][2]]; if ($valmiddlenameid eq "") { $valmiddlenameid = undef; } else { $valmiddlenameid = $DBData{"DataMiddleName"}{$valmiddlenameid} };
			
			my $valsuffix = 						lc $values[$ValueKey[$KeyTurn][3]]; 	if ($valsuffix eq "") { $valsuffix = undef; }
			my $valdbuniqid = 					uc $values[$ValueKey[$KeyTurn][43]]; 	if ($valdbuniqid eq "") { $valdbuniqid = undef; }   
			                            
			my $valstreetnonformat = 		lc $values[$ValueKey[$KeyTurn][46]]; if ($valstreetnonformat eq "") { $valstreetnonformat = undef; }
			my $valcityname = 					lc $values[$ValueKey[$KeyTurn][10]]; if ($valcityname eq "") { $valcityname = undef; } 
			my $valstreet = 						lc $values[$ValueKey[$KeyTurn][8]];  if ($valstreet eq "") { $valstreet = undef; }
			my $valdistrictownid = 			lc $values[$ValueKey[$KeyTurn][24]]; if ($valdistrictownid eq "") { $valdistrictownid = undef; } else { $valdistrictownid = $DBData{"DataDistrictTown"}{$valdistrictownid} };
			
			my $valprevname = 					lc $values[$ValueKey[$KeyTurn][33]]; if ($valprevname eq "") { $valprevname = undef; }    												
			my $valprevaddress = 				lc $values[$ValueKey[$KeyTurn][32]]; if ($valprevaddress eq "") { $valprevaddress = undef; }    												
			my $valprevcounty = 				lc $values[$ValueKey[$KeyTurn][31]]; if ($valprevcounty eq "") { $valprevcounty = undef; }                      
			my $valprevyearvoted = 			$values[$ValueKey[$KeyTurn][30]]; 	 if ($valprevyearvoted eq "") { $valprevyearvoted = undef; }               
			
			my $valotherparty = 				lc $values[$ValueKey[$KeyTurn][20]];if ($valotherparty eq "") { $valotherparty = undef; }                      	
			my $FinalDataVoterIndexID = $DBVoterIdx{$vallastnameid}{$valfirstnameid}{$valmiddlenameid}{$valsuffix}{$valvotdob}{uc $valdbuniqid};	
			
			my $valgender = 						ReturnGender($values[$ValueKey[$KeyTurn][18]]); 						if ( $valgender eq "") { $valgender = undef; } 								# 18
			my $valdbvotcomp = 					$DBVoterComplement{$valprevname}{$valprevaddress}{$DBDataCounty{$SystemState}{$valprevcounty}}{$valprevyearvoted}{$valprevdatevoted}{$valotherparty}; if ( $valdbvotcomp == 0) { $valdbvotcomp = undef; } 
			my $valreasoncode = 				ReturnReasonCode(uc $values[$ValueKey[$KeyTurn][40]]); 			if ( $valreasoncode eq "") { $valreasoncode = undef; }       	# 40
			my $validreq = 							ReturnYesNo($values[$ValueKey[$KeyTurn][37]]); 							if ( $validreq eq "") { $validreq = undef; }          				# 37
			my $validmet = 							ReturnYesNo($values[$ValueKey[$KeyTurn][38]]); 							if ( $validmet eq "") { $validmet = undef; }          				# 38
			my $valaplsource = 					ReturnRegistrationSource($values[$ValueKey[$KeyTurn][36]]); if ( $valaplsource eq "") { $valaplsource = undef; }       		# 36
			my $valregparty = 					$values[$ValueKey[$KeyTurn][19]]; 									if ($valregparty eq "") { $valregparty = undef; }	          					# 19
			my $valuserstatus = 				ReturnStatusCode($values[$ValueKey[$KeyTurn][39]]); if ($valuserstatus eq "") { $valuserstatus = undef; }	          			# 39
	
			my $DataCustomCycle = 1;
			### The status define the DataCycle;
			### If the voter is purged, the DataCycle need to be 1, else the regular
			if ( $valuserstatus eq "Active" ||  $valuserstatus eq "Inactive" ) {
				$DataCustomCycle = $DataCycle;
			}
			
			my $valcountyid = 					$values[$ValueKey[$KeyTurn][34]]; 		if ($valcountyid eq "") { $valcountyid = undef; }																		# 34
		
			my $valhousnumber = 				lc $values[$ValueKey[$KeyTurn][4]]; 	if ($valhousnumber eq "") { $valhousnumber = undef; }          											# 4
			my $valresfracaddr = 				lc $values[$ValueKey[$KeyTurn][5]]; 	if ($valresfracaddr eq "") { $valresfracaddr = undef; }      		 										# 5
			my $valresprestreet = 			lc $values[$ValueKey[$KeyTurn][7]]; 	if ($valresprestreet eq "") { $valresprestreet = undef; }        										# 7 
			my $valrespostst = 					lc $values[$ValueKey[$KeyTurn][9]]; 	if ($valrespostst eq "") { $valrespostst = undef; }          			 									# 9 
			my $valzip = 								lc $values[$ValueKey[$KeyTurn][11]]; 	if ($valzip eq "") { $valzip = undef; }         												 						# 11
			my $valzip4 = 							lc $values[$ValueKey[$KeyTurn][12]]; 	if ($valzip4 eq "") { $valzip4 = undef; }       												 						# 12
			
			my $valcountycode = 				$values[$ValueKey[$KeyTurn][21]]; 		if ($valcountycode eq "") { $valcountycode = undef; }    										 		 								# 21
			
			my $valmail1 = 							lc $values[$ValueKey[$KeyTurn][13]]; 	if ($valmail1 eq "") { $valmail1 = undef; }     																			# 13
			my $valmail2 = 							lc $values[$ValueKey[$KeyTurn][14]]; 	if ($valmail2 eq "") { $valmail2 = undef; }    																				# 14
			my $valmail3 = 							lc $values[$ValueKey[$KeyTurn][15]]; 	if ($valmail3 eq "") { $valmail3 = undef; }    																				# 15
			my $valmail4 = 							lc $values[$ValueKey[$KeyTurn][16]]; 	if ($valmail4 eq "") { $valmail4 = undef; }    																				# 16
			
			my $valapt = 								lc $values[$ValueKey[$KeyTurn][6]]; 	if ($valapt eq "") { $valapt = undef; }    																					# 6
			

			my $valdistward = 					lc $values[$ValueKey[$KeyTurn][25]];	if ( $valdistward < 1 ) {	$valdistward = undef;	} 					# 25
			my $valdisted = 						$values[$ValueKey[$KeyTurn][22]]; 		if ( $valdisted eq "" || $valdisted == 0 ) {	$valdisted = undef;	} 	        	# 22
			my $valdistad = 						$values[$ValueKey[$KeyTurn][28]]; 		if ( $valdistad eq "" || $valdistad == 0 ) {	$valdistad = undef;	} 	        	# 28 
			my $valdistsn = 						$values[$ValueKey[$KeyTurn][27]]; 		if ( $valdistsn eq "" || $valdistsn == 0 ) {	$valdistsn = undef;	} 	       	 	# 27
			my $valdistle = 						$values[$ValueKey[$KeyTurn][23]]; 		if ( $valdistle eq "" || $valdistle == 0 ) {	$valdistle = undef;	} 	      	  # 23
			my $valdistcg = 						$values[$ValueKey[$KeyTurn][26]]; 		if ( $valdistcg eq "" ) {	$valdistcg = undef;	} 	      	  # 26
						
			my $valrestype = 						lc $values[$ValueKey[$KeyTurn][45]]; 		if ( $valrestype eq "" ) {	$valrestype = undef;	} 
			my $valnonstd = 						$DBData{"DataStreetNonStdFormat"}{$valstreetnonformat}; if ( $valnonstd eq "" ) {	$valnonstd = undef;	} 
			
			
			### Prep the Voter file		    
			my $FinalDataStreetID = 		$DBData{"DataStreet"}{$valstreet};                     
			my $FinalDataCityID = 			$DBData{"DataCity"}{$valcityname};  
			my $FinalDataCountyID = 		$DBDataCounty{$SystemState}{$valcountycode};
			my $FinalDataAddressID = 		$DBDataAddress{$valhousnumber}{$valresfracaddr}{$valresprestreet}{$FinalDataStreetID}{$valrespostst}{$FinalDataCityID}{$FinalDataCountyID}{$valzip}{$valzip4};
			my $FinalDataVoterIndexID = $DBVoterIdx{$vallastnameid}{$valfirstnameid}{$valmiddlenameid}{$valsuffix}{$valvotdob}{$valdbuniqid};												
			my $FinalDataMailingID = 		$DBMailingAddress{$valmail1}{$valmail2}{$valmail3}{$valmail4};
			my $valdbmailingid = 				$FinalDataMailingID; if ( $valdbmailingid == 0) { $valdbmailingid = undef; }
			my $FinalDataHouseID = 			$DBDataHouse{$FinalDataAddressID}{$valrestype}{$valapt}{$valdistrictownid}{$valnonstd};
			my $FinalDataDistrictID = 	$DBDataDistrict{$FinalDataCountyID}{$valdisted}{$valdistad}{$valdistsn}{$valdistle}{$valdistward}{$valdistcg};
			
			#if (uc $valdbuniqid eq $TestVoterUNIQID) {
			if ( ! defined $FinalDataDistrictID ) {
				print "\n\nProblem with the District not found when it should have\n";
				print "\tFINAL Data District: ->" . $FinalDataDistrictID . "<-\t";
				print "{$FinalDataCountyID}{$valdisted}{$valdistad}{$valdistsn}{$valdistle}{$valdistward}{$valdistcg}\n";
				print "\tUNIQ CODE: $valdbuniqid\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
				exit 137;
			} 	
			
			my $FinalDataTemporalID = 	$DBDataTemporal{$DataCustomCycle}{$FinalDataHouseID}{$FinalDataDistrictID};
						
			my $valrmbactive = 'yes';
			my $valDBVoter = $DBVoter{$FinalDataVoterIndexID}{$FinalDataHouseID}{$valgender}{$valdbuniqid}{$valregparty}
																{$valreasoncode}{$valuserstatus}{$valdbmailingid}{$validreq}{$validmet}{$valregchar}
																{$valaplsource}{$valvotinactive}{$valvotpurged}{$valcountyid}{$valrmbactive};	
																
			
		
			
											
#			if (uc $valdbuniqid eq $TestVoterUNIQID) {
#				#DBVoter: $DBVoter{20382}{21907}{male}{}{NY000000000008509102}{BLK}{Other}{PURGED}{}{no}{yes}{2004-09-01}{DMV}{}{2013-01-24}{1024080}{yes} ->    ##
#				print "\n\tCHECKING DBVoterIdx: " . 
#								"\t\$DBVoterIdx{" . $vallastnameid . "}{" . $valfirstnameid . "}{" . $valmiddlenameid . "}{" . $valsuffix . "}{" . 
#																$valvotdob . "}{" . $valdbuniqid .  "} -> \t";
#				print "#" . $FinalDataVoterIndexID	. "#\n";
#				
#				print "\tCHECKING DBVoter: " . 
#							"\t\$DBVoter{" . $FinalDataVoterIndexID . "}{" . $FinalDataHouseID . "}{" . $valgender . "}{" . 
#															$valdbuniqid . "}{" . $valregparty . "}{" . $valreasoncode . "}{" . $valuserstatus . "}{" . $valdbmailingid . "}{" . 
#															$validreq . "}{" . $validmet . "}{" . $valregchar . "}{" . $valaplsource . "}{" . $valvotinactive . "}{" . 
#															$valvotpurged . "}{" . $valcountyid . "}{" . $valrmbactive . "} -> \t";
#				print "#" . $valDBVoter	. "#\n";	
#			}	
										
			if (! defined $valDBVoter ) {
															
				$DBVoter{$FinalDataVoterIndexID}{$FinalDataHouseID}{$valgender}{$valdbuniqid}{$valregparty}{$valreasoncode}{$valuserstatus}
								{$valdbmailingid}{$validreq}{$validmet}{$valregchar}{$valaplsource}{$valvotinactive}{$valvotpurged}{$valcountyid}{$valrmbactive} = "ADD";
								
#				print "\t\tVALDBVOTER: #" . $valDBVoter . "#\t";
#				print $DBVoter{$FinalDataVoterIndexID}{$FinalDataHouseID}{$valgender}{$valdbvotcomp}{$values[43]}{$values[19]}{$valreasoncode}{$values[39]}
#											{$valdbmailingid}{$validreq}{$validmet}{$values[35]}{$valaplsource}{$valvotinactive}{$valvotpurged}{$values[34]}{$valrmbactive} . "\n";
														
				if ( defined $valvotinactive) { $valvotinactive = $dbh->quote($valvotinactive) } else { $valvotinactive = "null"; }
				if ( defined $valvotpurged) { $valvotpurged = $dbh->quote($valvotpurged) } else { $valvotpurged = "null"; }
				if ( defined $valgender) { $valgender = $dbh->quote($valgender) } else { $valgender = "null"; }
				if ( defined $valdbvotcomp) { $valdbvotcomp = $dbh->quote($valdbvotcomp) } else { $valdbvotcomp = "null"; }
				if ( defined $valreasoncode) { $valreasoncode = $dbh->quote($valreasoncode) } else { $valreasoncode = "null"; }
				if ( defined $validreq) { $validreq = $dbh->quote($validreq) } else { $validreq = "null"; }
				if ( defined $validmet) { $validmet = $dbh->quote($validmet) } else { $validmet = "null"; }
				if ( defined $valaplsource) { $valaplsource = $dbh->quote($valaplsource) } else { $valaplsource = "null"; }
				if ( defined $valrmbactive) { $valrmbactive = $dbh->quote($valrmbactive) } else { $valrmbactive = "null"; }
				if ( defined $valdbmailingid) { $valdbmailingid = $dbh->quote($valdbmailingid) } else { $valdbmailingid = "null"; }
				if ( defined $valregparty) { $valregparty = $dbh->quote($valregparty) } else { $valregparty = "null"; }
				if ( defined $valuserstatus) { $valuserstatus = $dbh->quote($valuserstatus) } else { $valuserstatus = "null"; }
				if ( defined $valregchar) { $valregchar = $dbh->quote($valregchar) } else { $valregchar = "null"; }
				if ( defined $valcountyid) { $valcountyid = $dbh->quote($valcountyid) } else { $valcountyid = "null"; }
				if ( defined $valdbuniqid) { $valdbuniqid = $dbh->quote(uc $valdbuniqid) } else { $valdbuniqid = "null"; }
				
							
				
				### More debug hook
#				if ($valdbuniqid eq $TestVoterUNIQID) {
#					
#					
#					print "SAVING DBVoter: " . 
#								"\$DBVoter{" . $FinalDataVoterIndexID . "}{" . $FinalDataHouseID . "}{" . $valgender . "}{" . $valdbvotcomp . "}{" . 
#																$valdbuniqid . "}{" . $valregparty . "}{" . $valreasoncode . "}{" . $valuserstatus . "}{" . $valdbmailingid . "}{" . 
#																$validreq . "}{" . $validmet . "}{" . $valregchar . "}{" . $valaplsource . "}{" . $valvotinactive . "}{" . 
#																$valvotpurged . "}{" . $valcountyid . "}{" . $valrmbactive . "} -> \t";
#
#					print "#" . $valDBVoter	. "#\n";	
#					exit();
#				}	
															
				if ( $FirstTime[0] == 0 ) { $FirstTime[0] = 1; } else { $sqldata[0] .= ","; }
				$sqldata[0] .= "(" . 	$dbh->quote($FinalDataVoterIndexID) . "," . $dbh->quote($FinalDataHouseID) . "," .
															$valgender . "," . $valdbuniqid . "," .	$valregparty . "," . $valreasoncode . "," . 
															$valuserstatus . "," . $valdbmailingid . "," . $validreq . "," . $validmet . "," . $valregchar . "," . 
															$valaplsource . "," . $valvotinactive . "," . $valvotpurged . "," . $valcountyid . "," . $valrmbactive . "," .
															$dbh->quote($lastdate) . "," . $dbh->quote($lastdate) . ")";	
				$DBAddCounter[0]++;
			} else {
				if ( $valDBVoter ne "ADD") {
					#print " - VALDBVoter: $valDBVoter\n";
					if ( $valDBVoter == 0) {
						print " - DBVOTER {$FinalDataVoterIndexID}{$FinalDataHouseID}{$valgender}{$valdbuniqid}{$valregparty}" . 
									"{$valreasoncode}{$valuserstatus}{$valdbmailingid}{$validreq}{$validmet}{$valregchar}" . 
									"{$valaplsource}{$valvotinactive}{$valvotpurged}{$valcountyid}{$valrmbactive} == ZERO\n";
						print "\tUNIQ CODE: $valdbuniqid\n";
						print "Line $i: " . $TheWholeFile[$i] . "\n";
						exit 137;
					} 	
					
					$VoterIDSeenInFile[$valDBVoter] += 2;
				}
			}

			#### NEED TO ATTACH THE DISTICTS
			if (! defined $DBDataTemporal{$DataCustomCycle}{$FinalDataHouseID}{$FinalDataDistrictID}) {															
				$DBDataTemporal{$DataCustomCycle}{$FinalDataHouseID}{$FinalDataDistrictID} = "ADD";											
				if ( $FirstTime[1] == 0 ) { $FirstTime[1] = 1; } else { $sqldata[1] .= ","; }
				$sqldata[1] .= "(" . $dbh->quote($DataCustomCycle) . "," . $dbh->quote($FinalDataHouseID) . "," .
															$dbh->quote($FinalDataDistrictID) . ")";
				$DBAddCounter[1]++;
			}
			
			### This is full of debug statements to make sure that everything is correct
			if ( $FinalDataHouseID eq "" || $FinalDataVoterIndexID eq "") {
				
				print "\033[1;31mCatastrophic error, missing ";
				if ( $FinalDataHouseID eq "") {	print "DataHouseID information for voter on line $i of " . $ARGV[0] . " data file.\n";	}
				if ( $FinalDataHouseID eq "") {	print "FinalDataVoterIndexID information for voter on line $i of " . $ARGV[0] . " data file.\n";	}
				
				print "\033[0m\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";

				print "FinalDataHouseID: $FinalDataHouseID\t";
				print "FinalDataMailingID: $FinalDataMailingID\t"; 
				print "valdbvotcomp: $valdbvotcomp\t";
				print "FinalDataDistrictID: $FinalDataDistrictID\n\n";
			
				my @VariablesNames = ("valvotdob", "valvotinactive", "valvotpurged", "valregchar", "vallastnameid", "valfirstnameid", "valmiddlenameid", 
															"valsuffix", "valdbuniqid", "valstreetnonformat", "valcityname", "valstreet", "valdistrictownid", "valprevname", 
															"valprevaddress", "valprevcounty:", "valprevyearvoted", "valprevdatevoted", "valotherparty", "valgender",
															"valreasoncode", "validreq", "validmet", "valaplsource", "valregparty", "valuserstatus", "valcountyid", 
															"valhousnumber", "valresfracaddr", 	"valresprestreet", "valrespostst", "valzip", "valzip4", "valcountycode", 
															"valmail1", "valmail2", "valmail3", "valmail4", "valapt", "valdistward", 
															"valdisted", "valdistad", "valdistsn", "valdistle", "valdistcg", "valrestype", "valnonstd");


				my @VariablesContent = ($valvotdob, $valvotinactive, $valvotpurged, $valregchar, $vallastnameid, $valfirstnameid, $valmiddlenameid, 
																$valsuffix, $valdbuniqid, $valstreetnonformat, $valcityname, $valstreet, $valdistrictownid, $valprevname, 
																$valprevaddress, $valprevcounty, $valprevyearvoted, $valprevdatevoted, $valotherparty,$valgender, 
																$valreasoncode, $validreq, $validmet, $valaplsource, $valregparty, $valuserstatus, $valcountyid, 
																$valhousnumber, $valresfracaddr, $valresprestreet, $valrespostst, $valzip, $valzip4, $valcountycode,
																$valmail1, $valmail2, $valmail3, $valmail4, $valapt, $valdistward, 
																$valdisted, $valdistad, $valdistsn, $valdistle, $valdistcg, $valrestype, $valnonstd);
																
				my @VariableCounts = (17,41,42,35,0,1,2,3,43,46,10,8,24,33,32,31,30,29,20,18,40,37,38,36,19,39,34,4,5,7,9,11,12,21,13,14,15,16,6,25,22,28,27,23,26,45,46);




				for (my $i = 0; $i < @VariablesContent; $i++) {
					printf("%-15s:\t    \$values[$KeyTurn][%2d]]: %-20s\t - define # %2d: %-22s:\t%-25s\n", 
									$VariablesNames[$i],$ValueKey[$KeyTurn][$VariableCounts[$i]],$values[$ValueKey[$KeyTurn][$VariableCounts[$i]]],
									$VariableCounts[$i], $ValueKeyDesc[$KeyTurn][$ValueKey[$KeyTurn][$VariableCounts[$i]]],$VariablesContent[$i]);			
				}

				print "\n";
				print "FinalDataVoterIndexID:\t" . $FinalDataVoterIndexID . "\t" .
							"\$DBVoterIdx{\$vallastnameid:" . $vallastnameid . "}{\$valfirstnameid:" . $valfirstnameid . "}{\$valmiddlenameid:" . 
							$valmiddlenameid . "}{\$valsuffix:" . $valsuffix . "}{\$valvotdob:" . $valvotdob . "}{\$valdbuniqid:" . $valdbuniqid . "}\n";
								
				print "valdbvotcomp:\t\t" . $valdbvotcomp . "\t" .
							"\$DBVoterComplement{\$valprevname:" . $valprevname . "}{\$valprevaddress:" . $valprevaddress . "}{\$DBDataCounty:" .
								$DBDataCounty{$SystemState}{$valprevcounty} . "{\$valprevyearvoted:" . $valprevyearvoted . "}{\$valprevdatevoted:" . 
									$valprevdatevoted . "}{\$valotherparty:" . $valotherparty . "}\n";
								
				print "FinalDataStreetID:\t" . $FinalDataStreetID . "\t" .
							"\$DBData{\"DataStreet\"}{\$valstreet:" . $valstreet . "}\n";
				
				print "FinalDataCityID:\t" . $FinalDataCityID . "\t" .
							"\$DBData{\"DataCity\"}{\$valcityname:" . $valcityname . "}\n";
							
							
				print "FinalDataCountyID:\t" . $FinalDataCountyID . "\t" .
							"\$DBData{\"DataCounty\"}{\$SystemState:" . $SystemState . "}{\$valcountycode:" . $valcountycode . "}\n";
							
					
				print "valnonstd:\t\t" . $valnonstd . "\t" .
							"\$DBData{\"DataStreetNonStdFormat_ID\"}{\$valstreetnonformat:" . $valstreetnonformat . "}\n";
				
				print "FinalDataCountyID:\t" . $FinalDataCountyID . "\t" .
							"\$DBDataCounty{\$SystemState:" . $SystemState . "}{\$valcountycode:" . $valcountycode . "}\n";
				
				print "FinalDataAddressID:\t" . $FinalDataAddressID . "\t" .
							"\$DBDataAddress{\$valhousnumber:" . $valhousnumber . "}{\$valresfracaddr:" . $valresfracaddr . "}{\$valresprestreet:" . 
							$valresprestreet . "}{\$FinalDataStreetID:" . $FinalDataStreetID . "}{\$valrespostst:" . $valrespostst . 
							"}{\$FinalDataCityID:" . $FinalDataCityID . 
							"}{\$FinalDataCountyID:" . $FinalDataCountyID . "}{\$valzip:" . $valzip .  
							"}{\$valzip4:" . $valzip4 . "}\n";

				print "FinalDataVoterIndexID:\t" . $FinalDataVoterIndexID . "\t" .
							"\$DBVoterIdx{\$vallastnameid:" . $vallastnameid . "}{\$valfirstnameid:" . $valfirstnameid . "}{\$valmiddlenameid:" . 
							$valmiddlenameid . "}{\$valsuffix:" . $valsuffix . "}{\$valvotdob:" . $valvotdob . "}{\$valdbuniqid:" . $valdbuniqid . "}\n";

				print "FinalDataMailingID:\t" . $FinalDataMailingID . "\t" .
							"\$DBMailingAddress{\$valmail1:" . $valmail1 . "}{\$valmail2:" . $valmail2 . "}{\$valmail3:" . 
							$valmail3 . "}{\$valmail4:" . $valmail4 . "}\n";

				print "FinalDataHouseID:\t" . $FinalDataHouseID . "\t" .
							"\$DBDataHouse{\$FinalDataAddressID:" . $FinalDataAddressID . "}{\$valrestype:" . $valrestype . "}{\$valapt:" . 
							$valapt . "}{\$valdistrictownid:" . $valdistrictownid . "}{\$valnonstd:" . $valnonstd . "}\n";

			
				print "valdbmailingid:\t" . $valdbmailingid . "\n";
				
				print "FinalDataDistrictID:\t" . $FinalDataDistrictID . "\t" .
							"\$DBDataDistrict{\$FinalDataCountyID:" . $FinalDataCountyID . "}{\$valdisted:" . $valdisted . "}{\$valdistad:" . 
							$valdistad . "}{\$valdistsn:" . $valdistsn . "}{\$valdistle:" . $valdistle . "}{\$$valdistward:" .
						 	$valdistward . "}{\$valdistcg:" . $valdistcg . "}\n";

				print "FinalDataTemporalID:\t" . $FinalDataTemporalID . "\t" .
							"\$DBDataTemporal{\$DataCycle:" . $DataCycle . "}{\$FinalDataHouseID:" . $FinalDataHouseID . "}{\$FinalDataDistrictID:" . 
							$FinalDataDistrictID . "}\n";
						
				print "\n";
				
				print "Search in DBIndex\n";
				#if (uc $valdbuniqid eq $TestVoterUNIQID) {
				#DBVoter: $DBVoter{20382}{21907}{male}{}{NY000000000008509102}{BLK}{Other}{PURGED}{}{no}{yes}{2004-09-01}{DMV}{}{2013-01-24}{1024080}{yes} ->    ##
#					foreach my $key1 (keys %DBVoterIdx) {
#						foreach my $key2 (keys %{$DBVoterIdx{$key1}}) {
#							foreach my $key3 (keys %{$DBVoterIdx{$key1}{$key2}}) {
#								foreach my $key4 (keys %{$DBVoterIdx{$key1}{$key2}{$key3}}) {
#									foreach my $key5 (keys %{$DBVoterIdx{$key1}{$key2}{$key3}{$key4}}) {
#										foreach my $key6 (keys %{$DBVoterIdx{$key1}{$key2}{$key3}{$key4}{$key5}}) {
#											
#										
#											#$DBVoterIdx{$vallastnameid:1}{$valfirstnameid:1}{$valmiddlenameid:1}{$valsuffix:jr}{$valvotdob:1944-02-06}{$valdbuniqid:'NY000000000003306213'}
#											print "CHECKING DBVoterIdx: " . 
#																		"\$DBVoterIdx{" . $vallastnameid . "}{" . $valfirstnameid . "}{" . $valmiddlenameid . "}{" . $valsuffix . "}{" . 
#																										$valvotdob . "}{" . $valdbuniqid .  "} -> \t";
#											print "#" . $DBVoterIdx{$vallastnameid}{$valfirstnameid}{$valmiddlenameid}{$valsuffix}{$valvotdob}{$valdbuniqid}	. "#\n";		
#																											
#											print "CHECKING DBVoterIdx: " . 
#																		"\$DBVoterIdx{" . $key1 . "}{" . $key2 . "}{" . $key3 . "}{" . $key4 . "}{" . 
#																										$key5 . "}{" . $key6 .  "} -> \t";
#											print "#" . $DBVoterIdx{$key1}{$key2}{$key3}{$key4}{$key5}{$key6}	. "#\n";											
#																														
#										}
#									}
#								}
#							}
#						}
#					}
#				
				#}
				
				print "DB ORDER KEY:\n";
				for (my $i  = 0; $i < 47; $i++) {
					printf("Key: %d 0: %d\t%-20s\t1: %d\t%-20s\t|*| %d: %-25s\t%d: %-25s\n", 
							$i, $ValueKey[0][$i], $ValueKeyDesc[0][$ValueKey[0][$i]], $ValueKey[1][$i], $ValueKeyDesc[1][$ValueKey[1][$i]],
							$i, $ValueKeyDesc[0][$i], $i, $ValueKeyDesc[1][$i]); 
				}

				
				exit 137;
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


### Need to update the last seen voter files
$i = 0;

my $CountFirst = 0;
my $CountLast = undef;

#for($i = 0; $i < @VoterIDSeenInFile; $i++) {
#	print "Found Counter $i: " . $VoterIDSeenInFile[$i] . "\n";
#}

for($i = 0; $i < @VoterIDSeenInFile; $i++) {
	if ( $VoterIDSeenInFile[$i] == 3) {
		if ( ($i - $CountLast) > 1) {
#	 		print "CountLastSeen: $CountLast \t Record ID to update: $i ... " . ($i - $CountLast) . "\t" ; 
#	 		print "Need to update database from: $CountFirst to $CountLast\t"; 	
#	 		print "$i not sequencital";
#	 		print "\n";
	 		
			my $updatesql = "UPDATE Voters SET Voters_RecLastSeen = " . $dbh->quote($lastdate) . " WHERE " . 
											"Voters_ID >= " . $dbh->quote($CountFirst) . " AND Voters_ID <= " .  $dbh->quote($CountLast);
			my $sth = $dbh->prepare( $updatesql ); 
			$sth->execute() or die "$! $DBI::errstr " . $updatesql . "\n";
	 		$CountFirst = $i;
	 	}
	 	$CountLast = $i; 
 	}
}

print "I: $i \n";
print "Count: Scalar: " . scalar @VoterIDSeenInFile . "\n";
print "VoterIDSeenInFile: " . @VoterIDSeenInFile . "\n";
print "Count: VOterID: " . $#VoterIDSeenInFile . "\n";

exit();

sub	ReturnReasonCode {	
	if ( $_[0] ne "" ) { 
		if ($_[0] eq "ADJ-INCOMP") { return "AdjudgedIncompetent" }
		elsif ($_[0] eq "DEATH") {  return "Death" }
		elsif ($_[0] eq "DUPLICATE") {  return "Duplicate" }
		elsif ($_[0] eq "FELON") {  return "Felon" }
		elsif ($_[0] eq "MAIL-CHECK") { return "MailCheck" }
		elsif ($_[0] eq "MAILCHECK") { return "MailCheck" }
		elsif ($_[0] eq "MOVED") { return "MovedOutCounty" }
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
		if ( $_[0] eq 'I') { return 'undetermined'; }	
		if ( $_[0] eq 'X') { return 'other'; }	
		
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
