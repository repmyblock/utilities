#!/usr/bin/perl
use strict;
use DBI;
use Time::HiRes;
use Text::CSV;
use Lingua::EN::NameCase;
use Encode;

local $| = 1; # activate autoflush to immediately show the prompt
my $InsertDBEachCount = 1000000; 
my $TimePerCycle = 1000000;

#my @TableDates = qw/00000000/; # 20151215 20170515 20180127 20180423 20180529 20180924 20181029 20181203 20190204 20190225 20190325 
																#	20190408 20190513 20190617 20190702 20190805 20190903 20191021 20191125 20191209 20200113 20200203
																#	20200218 20200309 20200406 20200420 20200615 20200717 20200721 20201005 20201116 20201221 20210119
																#	20210222 20210308 20210405 20210614 20210816 20210927 20211018 20211122 20220214 20220305 20220316
																#	20220425 20220516 20220613 20220801 20220822 20220919 20221107/;
																										
																# 20151215 20170515 20180127 20180423 20180529 20180924 20181029 20181203 20190204 20190225 20190325 
																#	20190408 20190513 20190617 20190702 20190805 20190903 20191021 20191125 20191209 20200113 20200203
																#	20200218 20200309 20200406 20200420 20200615 20200717 20200721 20201005 20201116 20201221 20210119
																#	20210222 20210308 20210405 20210614 20210816 20210927 20211018 20211122 20220214 20220305 20220316
																#	20220425 20220516 20220613 20220801 20220822 20220919 20221107

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

##### This is the key
### DataFirstName -> 1  DataLastName -> 0 DataMiddleName -> 2 DataCity -> 12 DataDistrictTown -> 26 DataStreet -> 7

#### ALWAY KEEP EACH Grouping to the same elemnts
my @ValueKey = ([ 1, 0, 2, 10, 24, 8, 13, 14, 15, 16],
								[ 1, 0, 2, 12, 26, 7, 15, 16, 17, 18]);

my $dbname = "RepMyBlock"; my $dbhost = "data.theochino.us"; my $dbport = "3306";
my $dbuser = "usracct"; my $dbpass = "usracct";
my $dsn = "dbi:mysql:dbname=$dbname;host=$dbhost;port=$dbport;";
my $dbh = DBI->connect($dsn, $dbuser, $dbpass) or die "Connection error: $DBI::errstr";
$dbh->{mysql_auto_reconnect} = 1;

### Load all the Datas
my %DBData = ();
my %DBMailing = ();

my @TableNames  = qw/DataFirstName DataLastName DataMiddleName DataCity DataDistrictTown DataStreet DataStreetNonStdFormat DataMailingAddress/;
my $LocalLimit;

my $TableCounter = 0;
my @stmts = ();
my @sqlintro = ();
my @sqldata = ();
my @FirstTime = ();
my @DBAddCounter = ();


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
		} else {
			Encode::from_to($row[1], "UTF-8", "iso-8859-1" );
			$DBData { $Table } { lc $row[1] } = $row[0];			
		}
	}
		
	if ( $Table eq "DataMailingAddress" ) {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (" . $Table . "_Line1," . $Table . "_Line2," . $Table . "_Line3," . $Table . "_Line4) VALUES ";
	} elsif ($Table eq "DataFirstName" ||  $Table eq "DataLastName" ||  $Table eq "DataMiddleName") {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (" . $Table . "_Text," . $Table . "_Compress) VALUES ";			
	} elsif ($Table eq "DataCity" ||  $Table eq "DataDistrictTown" ||  $Table eq "DataStreet") {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (" . $Table . "_Name) VALUES ";
	} elsif ($Table eq "DataStreetNonStdFormat") {
		$sqlintro[$TableCounter] = "INSERT INTO " . $Table . " (" . $Table . "_Text) VALUES ";
	} 

	$FirstTime[$TableCounter] = 0;
	$TableCounter++;
	
	my $stop_time = Time::HiRes::gettimeofday();
	printf("%.2f\n", $stop_time - $start_time);
} 
			
# Entries with problems
# "NY000000000055010317" -> Apt: "2 "G"" (Need to remove the Double "G")

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
	my $Val1; my $Val2; my $Val3; my $Val4;

	undef(@TheWholeFile);
	$Counter = 0;
	$FileCounter = 0;
	$LocalCounter = 0;

	my $start_time = Time::HiRes::gettimeofday();
	while (my $row = <$fh>) {	$TheWholeFile[$Counter++] = $row; } #if ($Counter == 20000) { last; }}
	my $stop_time = Time::HiRes::gettimeofday();
	printf("Loading the CD Information in %.2f\n", $stop_time - $start_time);
	print "Loaded into memory $Counter lines\n";

	$start_time = Time::HiRes::gettimeofday();
	my $i;
	for ($i = 0; $i < $Counter; $i++) {
		
		my $ArtificialProblem = 0;
		my $csv = Text::CSV->new();
		$csv->always_quote(1);

		### I need to clean these entries.		
		# Coutts Way with '\xC2\x9F'
   	# "DELANEY-OLSON","ASHLEY","M","","118","","","","COUTTS WAY<9f> ","","GLOVERSVILLE","12078","0000","PO BOX 31","MAYFIELD, NY 12117","","",
   	# "19870826","F","REP","","18","2","4","JOHNSTOWN","000","21","49","118","20161108","","  ","","","206426","20080717","DMV","N","Y","ACTIVE",
   	# "","","","NY000000000050637660","2016 GENERAL ELECTION;2016 PRESIDENTIAL PRIMARY;2015 GENERAL ELECTION;2015 PRIMARY ELECTION;2015 VILLAGE ELECTION;
   	# 2014 GENERAL ELECTION;2014 FEDERAL PRIMARY;2012 GENERAL ELECTION;2009 GENERAL ELECTION"   	

		if ($csv->parse($TheWholeFile[$i]) || $ArtificialProblem == 1) {
	    my @values = $csv->fields();
	    my $status = $csv->combine(@values);
	   	
	   	my $LocalKeyCounter = 0;
			my $KeyTurn;
			
			if ( @values == 45 ) { $KeyTurn = 0; }
			elsif ( @values == 47 ) { $KeyTurn = 1; }	
			else {
				print "\n##################################################### Problem with " . @values . " -> $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
			}
				
  		if ( $i == 0 || $i > $Counter) {
    		print "Number of fields: " . @values . "\n"; 	
		 		print "\n##################################################### $i ####################################\n";
				print "Line $i: " . $TheWholeFile[$i] . "\n";
		 		print "CVS String: " . $csv->string() . "\n\n";
	  	}
    
	  	for (my $j = 0 ; $j < @values; $j++) {
	  		
	  		if ((@values == 45 && ($j == 0 || $j == 1 || $j == 2 || $j == 10 || $j == 24 || $j == 8  || $j == 13 || $j == 14 || $j == 15 || $j == 16)) || 
	  			  (@values == 47 && ($j == 0 || $j == 1 || $j == 2 || $j == 12 || $j == 26 || $j == 7 || $j == 15 || $j == 16 || $j == 17 || $j == 18))) {
	  			  	
	  			$values[$ValueKey[$KeyTurn][$LocalKeyCounter]] =~ s/\x{9f}//g;   ### Remove all weird control character.
	  			$values[$ValueKey[$KeyTurn][$LocalKeyCounter]] =~ s/\s+$//g;    ### The address need triming.
			   	$values[$ValueKey[$KeyTurn][$LocalKeyCounter]] =~ s/\s+/ /g;   ### Remove all white spaces
			   	
	  			if ( $LocalKeyCounter < 6) {
	  			
		  			if ( length $values[$ValueKey[$KeyTurn][$LocalKeyCounter]] > 0 ) {
			  			if ( ! defined $DBData { $TableNames[$LocalKeyCounter] } { lc $values[$ValueKey[$KeyTurn][$LocalKeyCounter]] } ) {
					  		if ( $FirstTime[$LocalKeyCounter] == 0 ) { $FirstTime[$LocalKeyCounter] = 1; } else { $sqldata[$LocalKeyCounter] .= ","; }
				  			
			  				if ($LocalKeyCounter > 2) {
						 			$sqldata[$LocalKeyCounter] .= "(" . $dbh->quote(nc($values[$ValueKey[$KeyTurn][$LocalKeyCounter]])) . ")";
					 			} else {
					 				$sqldata[$LocalKeyCounter] .= "(" . $dbh->quote(nc($values[$ValueKey[$KeyTurn][$LocalKeyCounter]])) . "," . $dbh->quote(ReturnCompress($values[$ValueKey[$KeyTurn][$LocalKeyCounter]])) . ")";
					 			}
				  		
				  			$DBData { $TableNames[$LocalKeyCounter] } { lc $values[$ValueKey[$KeyTurn][$LocalKeyCounter]] } = "ADD";
				  			$DBAddCounter[$LocalKeyCounter]++;
				  		}
				  	}
				  	
				  } elsif ( $LocalKeyCounter == 7) {
				  	
				  	### This portion is about the extra mailing address
				  	if ( ! defined ($DBMailing 	{ $TableNames[$LocalKeyCounter] } { lc $values[$ValueKey[$KeyTurn][$LocalKeyCounter]] } 
													  							{ lc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+1)]] }{ lc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+2)]] }
													  							{ lc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+3)]] })) {
													  		
	  					if ( $values[$ValueKey[$KeyTurn][$LocalKeyCounter]] eq "") { $Val1 = "null"; } else { $Val1 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][$LocalKeyCounter]]); }
	  					if ( $values[$ValueKey[$KeyTurn][($LocalKeyCounter+1)]] eq "") { $Val2 = "null"; } else { $Val2 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+1)]]); }
	  					if ( $values[$ValueKey[$KeyTurn][($LocalKeyCounter+2)]] eq "") { $Val3 = "null"; } else { $Val3 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+2)]]); }
	  					if ( $values[$ValueKey[$KeyTurn][($LocalKeyCounter+3)]] eq "") { $Val4 = "null"; } else { $Val4 = $dbh->quote(nc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+3)]]); }
	  					
	  					if ( $FirstTime[$LocalKeyCounter] != 0 ) { $sqldata[$LocalKeyCounter] .= ","; }
	  					$sqldata[$LocalKeyCounter] .= "(" . $Val1 . "," . $Val2 . "," .$Val3 . "," .$Val4 . ")";
	  					
  						$DBMailing 	{ $TableNames[$LocalKeyCounter] } { lc $values[$ValueKey[$KeyTurn][$LocalKeyCounter]] } 
					  							{ lc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+1)]] }{ lc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+2)]] }
					  							{ lc $values[$ValueKey[$KeyTurn][($LocalKeyCounter+3)]] } = "ADD";
					  							
	  					$FirstTime[$LocalKeyCounter] = 1;
		  				$DBAddCounter[$LocalKeyCounter]++;
				  	}
				  	
				  } elsif ( $LocalKeyCounter == 6 && @values == 47 ) {
				  	if ( length $values[11] > 0 ) {
					  	if ( ! defined ($DBData{"DataStreetNonStdFormat"}{lc $values[11]})) {
		  					if ( $FirstTime[$LocalKeyCounter] != 0 ) { $sqldata[$LocalKeyCounter] .= ","; }
	  						$sqldata[$LocalKeyCounter] .= "(" . $dbh->quote(nc($values[11])) . ")";
  							$DBData{"DataStreetNonStdFormat"}{ lc $values[11] } = "ADD";
	  						$FirstTime[$LocalKeyCounter] = 1;
		  					$DBAddCounter[$LocalKeyCounter]++;
				  		}
				  	} 				  	
				  }
					
			  	if ( $DBAddCounter[$LocalKeyCounter] % $InsertDBEachCount == 0 && $DBAddCounter[$LocalKeyCounter] > 0 && $FirstTime[$LocalKeyCounter] > 0) {
			  		my $db_start_time = Time::HiRes::gettimeofday();
			  		print "\n##################################################### $i ####################################\n";
			  		print "Counter: $Counter - LocalCounter: $LocalCounter - DBAddCounter: " . $DBAddCounter[$LocalKeyCounter] . "\n";
			  		print "Line $i: " . $TheWholeFile[$i] . "\n";
				 		print "CVS String: " . $csv->string() . "\n\n";
				 		print "Table Name: " . $TableNames[$LocalKeyCounter] . "\n"; 
				 		#print "SQL: " . $sqlintro[$LocalKeyCounter] . $sqldata[$LocalKeyCounter]  . "\n";
				 		
			 			my $sth = $dbh->prepare( $sqlintro[$LocalKeyCounter] . $sqldata[$LocalKeyCounter] ); 
				 		$sth->execute() or die "$! $DBI::errstr " . $sqlintro[$LocalKeyCounter] . $sqldata[$LocalKeyCounter];	
				 		$FirstTime[$LocalKeyCounter] = 0;
				 		$sqldata[$LocalKeyCounter] = "";
		  		
						print "\n";
			  	}
			  	
			  	if ( $i == 0) {	$counter_start_time = Time::HiRes::gettimeofday(); }
			  	
			  	if ( $i % $TimePerCycle == 0 && $i > 0 ) { 
						if ( $LocalKeyCounter == 0) {
				  		my $counter_stop_time = Time::HiRes::gettimeofday();
							printf("Time to process batch: %.2f\n", $counter_stop_time - $counter_start_time);
				  		$counter_start_time = Time::HiRes::gettimeofday();
				  	}
				  	print "\tCounter: " . $i . "\t" . $TableNames[$LocalKeyCounter] . "\t" . $DBAddCounter[$LocalKeyCounter] . "\n";
				  	if ( ($LocalKeyCounter+1) == @ValueKey[$KeyTurn]) { print "\n" };
			  	}
			  	$LocalKeyCounter++;
	  		}		  		
	  	}
	  
		 
		} else {
			print "\n##################################################### Problem $i ####################################\n";
			print "Line $i: " . $TheWholeFile[$i] . "\n";
		}
		
		$LocalCounter++;
	}
	
	print "Checking last entries: " . "\n";
	for (my $j = 0 ; $j < @{$ValueKey[0]} ; $j++) {
		
#		print "\n##################################################### Adding the Last pieces $j ####################################\n";
#		print "Counter: $Counter - LocalCounter: $LocalCounter - DBAddCounter: " . $DBAddCounter[$j] . "\n";
#		print "Table Name: " . $TableNames[$j] . "\n"; 
		#print "SQL: " . $sqldata[$j]  . "\n";
		if ( length $sqldata[$j] > 0) {
			my $sth = $dbh->prepare( $sqlintro[$j] . $sqldata[$j] ); 
			$sth->execute() or die "$! $DBI::errstr " . $sqlintro[$j] . $sqldata[$j];	
			$sqldata[$j] = "";
			$FirstTime[$j] = 0;
			print "\n";	
		}
	}
	
	print "Counter: $Counter - LocalCounter: $LocalCounter\n";
	my $stop_time = Time::HiRes::gettimeofday();
	printf("Loading the CD Information in %.2f\n", $stop_time - $start_time);
	print "\n";
	
#}

my $stop_time_total = Time::HiRes::gettimeofday();
printf("%.2f\n", $stop_time_total - $start_time_total);

### A few functions
sub ReturnCompress {
	my $string = lc $_[0];
	$string =~ tr/a-zA-ZÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖØòóôõöøÙÚÛÜùúûüÇçÑñİıÿß//dc;
	return $string;
}

#sub PrintHexCode {
#	my $string = lc $_[0];
#	my @ArrayTo = ();
#	if ($row[1] =~ /Nu.*ez/) {
#		print $row[1] . "\t";
#		my $str =$row[1];
#		Encode::from_to($str, "UTF-8", "iso-8859-1");
#		foreach (0..length($str)-1){
#			print substr($str, $_, 1);
#      printf ("#%v04X# ", substr($str, $_, 1));
#		}
#		print "\n";
#	}
#}