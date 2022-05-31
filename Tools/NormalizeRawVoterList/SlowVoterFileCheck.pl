#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;
use POSIX qw(strftime);
use Time::HiRes qw ( clock );
use Data::Dumper;
use Time::HiRes;
use Number::Format 'format_number';
use Time::Seconds;

use FindBin::libs;
use RMBSchemas;

use RepMyBlock::NY;
my $StopCounterPass = 0;

#### This can be extrapoled from the INIT DB File
my $LastSeenBOEFile = "2022-04-25";
my $DataDistrictCycle_ID = "2";
my $RawTableName = "NY_Raw_20220425";

my $Counter = int($ARGV[0]);
my $CounterBeg = int($ARGV[1]);

if ($CounterBeg == 0) {
		$CounterBeg = 1;
}
	
	
print "Slow Line by Line check of the Database $CounterBeg\n";

### Connecting and Initializing.
my $RepMyBlock 		= RepMyBlock::NY->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");

print "State being considered: " . $RepMyBlock::DataStateID . "\n";

$RepMyBlock::dbhRawVoters =  $dbhRawVoters;
$RepMyBlock::dbhVoters = $dbhVoters;

my $TotalNumberOfVoters;

if ($Counter == 0) {
	$TotalNumberOfVoters = ($RepMyBlock->NumberOfVotersInDB($RawTableName))[0];
	print "Total Voters in DB: " . $TotalNumberOfVoters . "\n";	
 	$Counter = $TotalNumberOfVoters; 
} else {
	$TotalNumberOfVoters = $Counter;
	print "Total Voters in DB: " . $TotalNumberOfVoters . "\n";
}
my $PrintedNumber = format_number($TotalNumberOfVoters);
$Counter++;

my $start_time = Time::HiRes::gettimeofday();
my $prev_time = Time::HiRes::gettimeofday();

print "\033c";
my $ProcessCounter = 1;
my $Avg100Time = 0 , my $Avg1000Time = 0 , my $Avg10000Time = 0 ;
my $Tot100Time = 0 , my $Tot1000Time = 0 , my $Tot10000Time = 0 ;
my $stop_100_time = 0 , my $stop_1000_time = 0 , my $stop_10000_time = 0 ;

my $EachPercent = $TotalNumberOfVoters / 100;
my $PercentComp = 0;
my $timetocompletion = 0;


for (my $i = $CounterBeg; $i < $Counter; $i++) {
	print "\033c";
	print "\033[1;1H";
	
	my $stop_time = Time::HiRes::gettimeofday();
	printf("Counter: %d of %s General Time: %.2f seconds\nPer Round: %.5f\n", $ProcessCounter, $PrintedNumber, $stop_time - $start_time, $stop_time - $prev_time);
	printf("Tot Times per: 100 %6.5f 1000: %-7.0f 10000: %-8.0f\n", $Tot100Time, $Tot1000Time, $Tot10000Time);
	printf("Avg Times per: 100 %6.5f 1000: %6.5f 10000: %6.5f\n", $Avg100Time, $Avg1000Time, $Avg10000Time);
	printf("%4.2f%% Completed - Time to complete: %s\n", $PercentComp, $timetocompletion);
	
	print "\n";
	if ( ($ProcessCounter % 100) == 0) {		
		print "\033c";
		$Tot100Time = ($stop_time - $stop_100_time);# / 100;		
		$Avg100Time = ($stop_time - $stop_100_time)/ 100;		
		$stop_100_time = Time::HiRes::gettimeofday();
		
		if ( ($ProcessCounter % 1000) == 0) {		
			$Tot1000Time = ($stop_time - $stop_1000_time);# / 1000;		
			$Avg1000Time = ($stop_time - $stop_1000_time) / 1000;		
			$stop_1000_time = $stop_100_time;
			
			$PercentComp = ($ProcessCounter / $TotalNumberOfVoters) * 100;
			
			
			#Total time
			my $TempTime = ($stop_time - $start_time) / $ProcessCounter;
			my $TempNumberToGo = $TotalNumberOfVoters - $ProcessCounter;
			my $TempFinish = $TempNumberToGo * $TempTime;
			
			my $t = Time::Seconds->new($TempFinish);
			$timetocompletion = $t->pretty;
			
			if ( ($ProcessCounter % 10000) == 0) {		
				$Tot10000Time = ($stop_time - $stop_10000_time);# / 10000;		
				$Avg10000Time = ($stop_time - $stop_10000_time) / 10000;		
				$stop_10000_time = $stop_100_time;
				
			}
		}
	}
	
	$prev_time = $stop_time;
	
	my $result = $RepMyBlock->IDLoadFromRawData($RawTableName, $i);
	
	print "Processing and finding IDs:\n";
	printf ("\tNY_Raw_ID\t\tID: %-8d \n", $result->{'NY_Raw_ID'} );

my $LastNameID = $RepMyBlock->SlowReturnLastName($result->{'LastName'});
	$LastNameID = $RepMyBlock->SlowAddLastName ($result->{'LastName'}) if (! defined $LastNameID->{'DataLastName_ID'} && length($result->{'LastName'} > 0));
	printf ("\tLastName\t\tID: %8d %-50s\n", $LastNameID->{'DataLastName_ID'}, $result->{'LastName'});

	my $FistNameID = $RepMyBlock->SlowReturnFirstName($result->{'FirstName'});
	$FistNameID = $RepMyBlock->SlowAddFirstName ($result->{'FirstName'}) if (! defined $FistNameID->{'DataFirstName_ID'} && length($result->{'FirstName'} > 0));
	printf ("\tFirstName\t\tID: %8d %-50s\n", $FistNameID->{'DataFirstName_ID'}, $result->{'FirstName'});
	
	my $MiddleNameID = $RepMyBlock->SlowReturnMiddleName($result->{'MiddleName'});
	$MiddleNameID = $RepMyBlock->SlowAddMiddleName ($result->{'MiddleName'}) if (! defined $MiddleNameID->{'DataMiddleName_ID'} && length($result->{'MiddleName'} > 0));
	printf ("\tMiddleName\t\tID: %8d %-50s\n", $MiddleNameID->{'DataMiddleName_ID'}, $result->{'MiddleName'});

	printf ("\tSuffix\t\t\tID: %-8d %-50s\n", -1, $result->{'Suffix'});	
	printf ("\tResHouseNumber\t\tID: %-8d %-50s\n", -1, $result->{'ResHouseNumber'});	
	printf ("\tResFracAddress\t\tID: %-8d %-50s\n", -1, $result->{'ResFracAddress'});	
	printf ("\tResPreStreet\t\tID: %-8d %-50s\n", -1, $result->{'ResPreStreet'});	
	
	my $Street = $RepMyBlock->SlowRetunStreet($result->{'ResStreetName'});
	$Street = $RepMyBlock->SlowAddStreet ($result->{'ResStreetName'}) if (! defined $Street->{'DataStreet_ID'} && length($result->{'ResStreetName'} > 0));
	printf ("\tResStreetName\t\tID: %8d %-50s\n", $Street->{'DataStreet_ID'}, $result->{'ResStreetName'});	
	
	printf ("\tResPostStDir\t\tID: %-8d %-50s\n", -1, $result->{'ResPostStDir'});	
	printf ("\tResType\t\t\tID: %-8d %-50s\n", -1, $result->{'ResType'});	
	printf ("\tResApartment\t\tID: %-8d %-50s\n", -1, $result->{'ResApartment'});	
	printf ("\tResNonStdFormat\t\tID: %-8d %-50s\n", -1, $result->{'ResNonStdFormat'});	

	my $City = $RepMyBlock->SlowReturnCity($result->{'ResCity'});
	$City = $RepMyBlock->SlowAddCity($result->{'ResCity'}) if (! defined $City->{'DataCity_ID'} && length($result->{'ResCity'} > 0));
	printf ("\tResCity\t\t\tID: %8d %-50s\n", $City->{'DataCity_ID'}, $result->{'ResCity'});
	
	printf ("\tResZip\t\t\tID: %-8d %-50s\n", -1, $result->{'ResZip'});	
	printf ("\tResZip4\t\t\tID: %-8d %-50s\n", -1, $result->{'ResZip4'});	
	printf ("\tResMail1\t\tID: %-8d %-50s\n", -1, $result->{'ResMail1'});	
	printf ("\tResMail2\t\tID: %-8d %-50s\n", -1, $result->{'ResMail2'});	
	printf ("\tResMail3\t\tID: %-8d %-50s\n", -1, $result->{'ResMail3'});	
	printf ("\tResMail4\t\tID: %-8d %-50s\n", -1, $result->{'ResMail4'});	
	
	printf ("\tDOB\t\t\tID: %-8d %-50s\n", -1, $result->{'DOB'});	
	printf ("\tGender\t\t\tID: %-8d %-50s\n", -1, $result->{'Gender'});	
	printf ("\tEnrollPolParty\t\tID: %-8d %-50s\n", -1, $result->{'EnrollPolParty'});	
	printf ("\tOtherParty\t\tID: %-8d %-50s\n", -1, $result->{'OtherParty'});	
	
	my $County = $RepMyBlock->SlowRetunBOEIDCounty($result->{'CountyCode'}, $RepMyBlock::DataStateID);
	printf ("\tCountyCode\t\tID: %8d %-50s\n", $County->{'DataCounty_ID'}, $result->{'CountyCode'});	
	
	printf ("\tElectDistr\t\tID: %-8d %-50s\n", -1, $result->{'ElectDistr'});	
	printf ("\tLegisDistr\t\tID: %-8d %-50s\n", -1, $result->{'LegisDistr'});	
	
	my $Town = $RepMyBlock->SlowRetunTown($result->{'TownCity'});
	$Town = $RepMyBlock->SlowAddTown($result->{'TownCity'}) if (! defined $Town->{'DataDistrictTown_ID'} && length($result->{'TownCity'} > 0));
	printf ("\tTownCity\t\tID: %8d %-50s\n", $Town->{'DataDistrictTown_ID'}, $result->{'TownCity'});	
	
	printf ("\tWard\t\t\tID: %-8d %-50s\n", -1, $result->{'Ward'});	
	printf ("\tCongressDistr\t\tID: %-8d %-50s\n", -1, $result->{'CongressDistr'});	
	printf ("\tSenateDistr\t\tID: %-8d %-50s\n", -1, $result->{'SenateDistr'});	
	printf ("\tAssemblyDistr\t\tID: %-8d %-50s\n", -1, $result->{'AssemblyDistr'});	
	printf ("\tLastDateVoted\t\tID: %-8d %-50s\n", -1, $result->{'LastDateVoted'});	
	printf ("\tPrevYearVoted\t\tID: %-8d %-50s\n", -1, $result->{'PrevYearVoted'});	
	printf ("\tPrevCounty\t\tID: %-8d %-50s\n", -1, $result->{'PrevCounty'});	
	printf ("\tPrevAddress\t\tID: %-8d %-50s\n", -1, $result->{'PrevAddress'});	
	printf ("\tPrevName\t\tID: %-8d %-50s\n", -1, $result->{'PrevName'});	
	printf ("\tCountyVoterNumber\tID: %-8d %-50s\n", -1, $result->{'CountyVoterNumber'});	
	printf ("\tRegistrationCharacter\tID: %-8d %-50s\n", -1, $result->{'RegistrationCharacter'});	
	printf ("\tApplicationSource\tID: %-8d %-50s\n", -1, $result->{'ApplicationSource'});	
	printf ("\tIDRequired\t\tID: %-8d %-50s\n", -1, $result->{'IDRequired'});	
	printf ("\tIDMet\t\t\tID: %-8d %-50s\n", -1, $result->{'IDMet'});	
	printf ("\tStatus\t\t\tID: %-8d %-50s\n", -1, $result->{'Status'});	
	printf ("\tReasonCode\t\tID: %-8d %-50s\n", -1, $result->{'ReasonCode'});	
	printf ("\tVoterMadeInactive\tID: %-8d %-50s\n", -1, $result->{'VoterMadeInactive'});	
	printf ("\tVoterPurged\t\tID: %-8d %-50s\n", -1, $result->{'VoterPurged'});	
	printf ("\tUniqNYSVoterID\t\tID: %-8d %-60s\n", -1, $result->{'UniqNYSVoterID'});	
		
	####################################################
	### ADRESS FUCTIONS                              ###
	####################################################
	### We need to add the house in the data
	
	printf ("\nFinding the Compress information for State: $RepMyBlock::DataStateID\n");
	my $Address = $RepMyBlock->SlowReturnAddress(
		$result->{'ResHouseNumber'}, $result->{'ResFracAddress'},
		$result->{'ResPreStreet'}, $Street->{'DataStreet_ID'},
		$result->{'ResPostStDir'}, $City->{'DataCity_ID'},
		$RepMyBlock::DataStateID, $result->{'ResZip'}, $result->{'ResZip4'}
	);
	printf ("\tDataAddress\t\tID: %8d %-50s\n", $Address->{'DataAddress_ID'}, undef);	

	####################################################
	### HOUSE FUCTIONS                               ###
	####################################################
	### We need to add the house in the data
	my $House = $RepMyBlock->SlowReturnHouse(
		$Address->{'DataAddress_ID'}, $result->{'ResApartment'}, $DataDistrictCycle_ID, $Town->{'DataDistrictTown_ID'}
	);
	
	if ( ! defined ($House->{'DataHouse_ID'})) {
		$House = $RepMyBlock->SlowAddHouse (
			$Address->{'DataAddress_ID'}, $result->{'ResApartment'}, $DataDistrictCycle_ID, $Town->{'DataDistrictTown_ID'}
		);
		printf ("\tDataHouse\t\tID: %8d %-50s\n", $House->{'DataHouse_ID'}, "******* ADDED ********");	

	} else {
		printf ("\tDataHouse\t\tID: %8d %-50s\n", $House->{'DataHouse_ID'}, "******* ALLREADY FOUND ********");	

	}
	
	
	####################################################
	### DISTRICT FUCTIONS                            ###
	####################################################
	
	### This to read the districts
	my $District = $RepMyBlock->SlowReturnDistrict(
		$County->{'DataCounty_ID'}, int($result->{'ElectDistr'}), int($result->{'AssemblyDistr'}), 
		int($result->{'SenateDistr'}), int($result->{'LegisDistr'}), $result->{'Ward'}, 
		int($result->{'CongressDistr'}), undef, undef, undef
	);
	
	if ( ! defined $District->{'DataDistrict_ID'} ) {

		$District = $RepMyBlock->SlowAddDistrict (
			$County->{'DataCounty_ID'}, int($result->{'ElectDistr'}), int($result->{'AssemblyDistr'}), 
			int($result->{'SenateDistr'}), int($result->{'LegisDistr'}), $result->{'Ward'}, 
			int($result->{'CongressDistr'}), undef, undef, undef
		);
		
		printf ("\tADDED DataDistrict\t\tID: %8d %-50s\n", $District->{'DataDistrict_ID'}, "******* ADDED ********");
	} else {
		
		printf ("\tDataDistrict\t\tID: %8d %-50s\n", $District->{'DataDistrict_ID'}, "******* ALLREADY FOUND ********");	
	}
	
	### These three need to be found in the NYC file.
	#	$result->{'DataDistrict_Council'}, 
	#	$result->{'DataDistrict_CivilCourt'}, 
	#	$result->{'DataDistrict_Judicial'}		
	# );


	####################################################
	### VOTERS INDEXES                               ###
	####################################################
	### This is for Voter to be updated
	my $VotersIndexes = $RepMyBlock->SlowReturnVotersIndexes(
				$RepMyBlock::DataStateID,
				$LastNameID->{'DataLastName_ID'}, $FistNameID->{'DataFirstName_ID'}, 
				$MiddleNameID->{'DataMiddleName_ID'}, $result->{'Suffix'}, 
				$result->{'DOB'}, $result->{'UniqNYSVoterID'}
	);
	printf ("\tVoterIndexes\t\tID: %8d %-50s\n", $VotersIndexes->{'VotersIndexes_ID'}, undef);	
	
	if ( ! defined($VotersIndexes->{'VotersIndexes_ID'})) {
		
		### Verify that the middle name is not in the initinal
		$VotersIndexes = $RepMyBlock->SlowReturnVotersIndexesByUniqStateID ($result->{'UniqNYSVoterID'});

		if ( defined($VotersIndexes->{'VotersIndexes_ID'})) {
			my $CheckMiddleName = $RepMyBlock->SlowReturnMiddleNameByID($VotersIndexes->{'DataMiddleName_ID'});
				
			### This is to find which to keep.
			if ( length($CheckMiddleName->{"DataMiddleName_Text"}) < length( $result->{'MiddleName'} )  &&
					 substr( $result->{'MiddleName'}, 0, 1) eq $CheckMiddleName->{"DataMiddleName_Text"} ) {				
				print "In RMB File Middle name: " . $CheckMiddleName->{"DataMiddleName_Text"} . "\n";
				print "In original by BOE Middle name: " . $result->{'MiddleName'} . "\n";
				print "The middle name can be changed with the other\n";
				$RepMyBlock->SlowUpdateMiddleNameInVoterByIDXID($VotersIndexes->{'VotersIndexes_ID'}, $MiddleNameID->{'DataMiddleName_ID'});
			}
		
		} else {	
		
			$VotersIndexes = $RepMyBlock->SlowInsertVotersIndexes(
				$RepMyBlock::DataStateID,
				$LastNameID->{'DataLastName_ID'}, $FistNameID->{'DataFirstName_ID'}, 
				$MiddleNameID->{'DataMiddleName_ID'}, $result->{'Suffix'}, 
				$result->{'DOB'}, $result->{'UniqNYSVoterID'}
			);
		}	
	}
		
	my $VotersByIndex = $RepMyBlock->SlowReturnVotersByVoterIndexID($VotersIndexes->{'VotersIndexes_ID'});
	printf ("\n\tVotersByIndex\t\tID: %8d %-50s\n", $VotersByIndex->{'Voters_ID'}, undef);	
	
	####################################################
	### VOTERS                                       ###
	####################################################
	### This is for Voter to be updated
	my $Voter = $RepMyBlock->SlowReturnVoters(
				$VotersIndexes->{'VotersIndexes_ID'}, $House->{'DataHouse_ID'}, 
				$RepMyBlock->ReturnGender($result->{'Gender'}), 
				$result->{'VotersComplementInfo_ID'}, $result->{'UniqNYSVoterID'}, 
				$RepMyBlock::DataStateID, $result->{'EnrollPolParty'}, 
				$RepMyBlock->ReturnReasonCode($result->{'ReasonCode'}), 				
				$RepMyBlock->ReturnStatusCode($result->{'Status'}), 
				$result->{'VotersMailingAddress_ID'}, $RepMyBlock->ReturnYesNo($result->{'IDRequired'}), 
				$RepMyBlock->ReturnYesNo($result->{'IDMet'}), $result->{'RegistrationCharacter'}, 
				$RepMyBlock->ReturnRegistrationSource($result->{'ApplicationSource'}), 
				$result->{'VoterMadeInactive'}, $result->{'VoterPurged'}, $result->{'CountyVoterNumber'}
	);
	
	printf ("\tVoter\t\t\tID: %8d %-50s\n", $Voter->{'Voters_ID'}, undef);	
	
	if ( ! defined($Voter->{'Voters_ID'})) {	
			
		if ( $VotersByIndex->{'Voters_UniqStateVoterID'} eq $result->{'UniqNYSVoterID'} ) {

			$RepMyBlock->SlowUpdateVotersByVoterIndexID(
					$VotersIndexes->{'VotersIndexes_ID'}, $House->{'DataHouse_ID'}, 
					$RepMyBlock->ReturnGender($result->{'Gender'}), 
					$result->{'VotersComplementInfo_ID'}, $result->{'UniqNYSVoterID'}, 
					$RepMyBlock::DataStateID, $result->{'EnrollPolParty'}, 
					$RepMyBlock->ReturnReasonCode($result->{'ReasonCode'}), 				
					$RepMyBlock->ReturnStatusCode($result->{'Status'}), 
					$result->{'VotersMailingAddress_ID'}, $RepMyBlock->ReturnYesNo($result->{'IDRequired'}), 
					$RepMyBlock->ReturnYesNo($result->{'IDMet'}), $result->{'RegistrationCharacter'}, 
					$RepMyBlock->ReturnRegistrationSource($result->{'ApplicationSource'}), 
					$result->{'VoterMadeInactive'}, $result->{'VoterPurged'}, 
					$result->{'CountyVoterNumber'}, $LastSeenBOEFile
			);

		} else {
		
			$Voter = $RepMyBlock->SlowInsertNewVoter (
						$VotersIndexes->{'VotersIndexes_ID'}, $House->{'DataHouse_ID'}, 
						$RepMyBlock->ReturnGender($result->{'Gender'}), 
						$result->{'VotersComplementInfo_ID'}, $result->{'UniqNYSVoterID'}, 
						$RepMyBlock::DataStateID, $result->{'EnrollPolParty'}, 
						$RepMyBlock->ReturnReasonCode($result->{'ReasonCode'}), 				
						$RepMyBlock->ReturnStatusCode($result->{'Status'}), 
						$result->{'VotersMailingAddress_ID'}, $RepMyBlock->ReturnYesNo($result->{'IDRequired'}), 
						$RepMyBlock->ReturnYesNo($result->{'IDMet'}), $result->{'RegistrationCharacter'}, 
						$RepMyBlock->ReturnRegistrationSource($result->{'ApplicationSource'}), 
						$result->{'VoterMadeInactive'}, $result->{'VoterPurged'}, 
						$result->{'CountyVoterNumber'}, $LastSeenBOEFile
			);
		}
				
		### Find the voter by attributes Voters_UniqStateVoterID
		
		$Voter = $RepMyBlock->SlowFixDoubleVotersTableByUniqStateID ($result->{'UniqNYSVoterID'}, $LastSeenBOEFile);
		
	} else {
			$RepMyBlock->SlowUpdateVoterLastSeen($Voter->{'Voters_ID'}, $LastSeenBOEFile);
	}
	
	print "\n";
	printf ("\tVoterHistory\t\tID: %8d %-1500s\n", undef, $result->{'VoterHistory'});	
	$ProcessCounter++;
}

print "\n";