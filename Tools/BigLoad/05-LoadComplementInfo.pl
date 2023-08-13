#!/usr/bin/perl

local $| = 1; # activate autoflush to immediately show the prompt

my $InsertDBEachCount = 1000000; 
my $TimePerCycle = 1000000;

my $SystemState = "1";
my $TestVoterUNIQID = "";

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
my %DBVoter = ();
my %DBVoterIdx = ();
my %DBMailingAddress = ();
my %DBVoterComplement = ();
my %DBDataDistrict = ();
my %DBDataTemporal = ();

my $tabledate = $ARGV[0];

my $filename = "/home/usracct/VoterFiles/NY/" . $tabledate . "/AllNYSVoters_" . $tabledate . ".txt";
my $filename = "/home/usracct/Test/TestData/Random_250/" . $tabledate . ".txt";
my $FileLastDateSeen = $tabledate;
print "Working on " . $filename . "\n";

open(my $fh, "< :encoding(Latin1)", $filename ) or die "cannot open $filename: $!";

my @ValueKey = ([0,1,2,3,4,5, 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46],
								[0,1,2,3,4,5,10,6,7,8,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46, 9,11]);


my @TableNames  = qw/VotersComplementInfo Voters DataDistrictTemporal VotersIndexes DataDistrict  DataFirstName DataLastName DataMiddleName  DataHouse  DataMailingAddress DataCounty DataAddress DataStreet DataCity DataDistrictTown DataStreetNonStdFormat/;
my $LocalLimit;

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
	print "Loading $Table \t"; 
	my $Stmt_FirstName = "SELECT * FROM " . $Table  . " " . $LocalLimit;
	my $sth = $dbh->prepare( $Stmt_FirstName ); $sth->execute() or die "$! $DBI::errstr";
	my $start_time = Time::HiRes::gettimeofday();
	
	while (my @row = $sth->fetchrow_array()) {
		$row[1] =~ s/\s+$//;    ### The address need triming.
		
		# this is Voters Table
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
	
	if ($Table eq "VotersComplementInfo") {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (Voters_ID,VotersComplementInfo_PrevName,VotersComplementInfo_PrevAddress,DataCountyID_PrevCounty,VotersComplementInfo_LastYearVoted,VotersComplementInfo_LastDateVoted,VotersComplementInfo_OtherParty) VALUES ";
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
	while (my $row = <$fh>) {	$TheWholeFile[$Counter++] = $row; }
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
  	$values[$ValueKey[$KeyTurn][31]] = int($values[$ValueKey[$KeyTurn][31]]);   ### To remove the leading 0 on the County
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
			
			my $valprevname = 					$values[$ValueKey[$KeyTurn][33]]; if ($valprevname eq "") { $valprevname = undef; }    												
			my $valprevaddress = 				$values[$ValueKey[$KeyTurn][32]]; if ($valprevaddress eq "") { $valprevaddress = undef; }    												
			my $valprevcounty = 				lc $values[$ValueKey[$KeyTurn][31]]; if ($valprevcounty eq "" || $valprevcounty == 0 ) { $valprevcounty = undef; }                      
			my $valprevyearvoted = 			$values[$ValueKey[$KeyTurn][30]]; 	 if ($valprevyearvoted eq "") { $valprevyearvoted = undef; }               
			
			my $valotherparty = 				$values[$ValueKey[$KeyTurn][20]];if ($valotherparty eq "") { $valotherparty = undef; }                      	
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
			
			if (uc $valdbuniqid eq $TestVoterUNIQID) {
			#if ( ! defined $FinalDataDistrictID ) {
			#	print "\n\nProblem with the District not found when it should have\n";
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
																			
			if (defined $valDBVoter ) {
				
				# VotersComplementInfo_ID, Voters_ID, VotersComplementInfo_PrevName, VotersComplementInfo_PrevAddress, 
				# DataCountyID_PrevCounty, VotersComplementInfo_LastYearVoted, VotersComplementInfo_LastDateVoted, VotersComplementInfo_OtherParty
				
				my $DonotAdd = 1;
		
				if ( ! defined ($DBVoterComplement{$valDBVoter}{lc $valprevname}{lc $valprevaddress}{$valprevcounty}{$valprevyearvoted}{$valprevdatevoted}{lc $valotherparty})) {

				
					if ( defined $valprevname) { $valprevname = $dbh->quote($valprevname); $DonotAdd = 0; } else { $valprevname = "null"; }
					if ( defined $valprevaddress) { $valprevaddress = $dbh->quote($valprevaddress); $DonotAdd = 0; } else { $valprevaddress = "null"; }
					if ( defined $valprevcounty) { $valprevcounty = $dbh->quote($valprevcounty); $DonotAdd = 0; } else { $valprevcounty = "null"; }
					if ( defined $valprevyearvoted) { $valprevyearvoted = $dbh->quote($valprevyearvoted); $DonotAdd = 0; } else { $valprevyearvoted = "null"; }
					if ( defined $valotherparty) { $valotherparty = $dbh->quote($valotherparty); $DonotAdd = 0; } else { $valotherparty = "null"; }
					if ( defined $valprevdatevoted) { $valprevdatevoted = $dbh->quote($valprevdatevoted); $DonotAdd = 0; } else { $valprevdatevoted = "null"; }
				
					if ($DonotAdd == 0) {
						if ( $FirstTime[0] == 0 ) { $FirstTime[0] = 1; } else { $sqldata[0] .= ","; }
						$sqldata[0] .= "(" . 	$dbh->quote($valDBVoter) . "," . $valprevname . "," .
																	$valprevaddress  . "," . $valprevcounty  . "," .	$valprevyearvoted . "," .  
																	$valprevdatevoted. "," . $valotherparty . ")";	
						$DBAddCounter[0]++;				
					}
				}
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
