#!/usr/bin/perl

### Need to document what this file is about.
use strict;
use DBI;
use Text::CSV;
use POSIX qw(strftime);
use Time::HiRes qw ( clock );
use Data::Dumper;

use FindBin::libs;
use RMBSchemas;

use RepMyBlock::NY;
use RepMyBlock::NYC;
my $EmptyDatabase = 1;
my $StopCounterPass = 0;

my $Cycle_ID = 1;

print "Start the program\n";

### Connecting and Initializing.
my $RepMyBlock 		= RepMyBlock::NY->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");

my $RepMyBlockLocal = RepMyBlock::NYC->new();
#my $dbhRawVotersLocal 	= $RepMyBlockLocal->InitDatabase("dbname_voters");

print "State being considered: " . $RepMyBlock::DataStateID . "\n";
print "Initializing files\n";
$RepMyBlock->InitializeVoterFile();

print "\nCaching the data from the CD from date: " .  $RepMyBlock->{tabledate} . "\n";
my $TableDated = "NY_Raw_" . $RepMyBlock->{tabledate};
my $TableDatedLocal = "NYC_Raw_20211022";

print "\nCharging the raw database\n";
$RepMyBlock::dbhRawVoters = $dbhRawVoters;
$RepMyBlock->SetDatabase($dbhVoters);

$RepMyBlock->EmptyDatabases("DataDistrictTemporal");

# Load stuff for the end.
print "Loading the District Caches\n";
my $Count = $RepMyBlock->LoadDataHouseDistrictCaches($TableDated, $TableDatedLocal);
my $Count = $RepMyBlock->LoadDistrictCaches();
$RepMyBlock->LoadDistrictTown();
$RepMyBlock->LoadCacheCountyTranslation();

### NEED TO FIX A COUNTY HERE so it doesn't eat the other county
### The bug is that I have to change the DataCounty_BOID on the DataCounty_ID.
### DataCounty_ID, DataState_ID, DataCounty_Name, DataCounty_BOEID

print "Prepping the Temporal Cache Group ID\n";
my $Counter = 1;
foreach my $DataDistrictTemporal_GroupID (keys %RepMyBlock::CacheDataHouseDistrict) {
	foreach my $CountyCode (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}}) {
		foreach my $ElectDistr (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}}) {
			foreach my $LegisDistr (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}}) {
				foreach my $Ward (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}}) {
					foreach my $CongressDistr (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}{$Ward}}) {
						foreach my $SenateDistr (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}{$Ward}{$CongressDistr}}) {
							foreach my $AssemblyDistr (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}{$Ward}{$CongressDistr}{$SenateDistr}}) {
								foreach my $Council_District (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}{$Ward}{$CongressDistr}{$SenateDistr}{$AssemblyDistr}}) {
									foreach my $Civil_Court_District (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}{$Ward}{$CongressDistr}{$SenateDistr}{$AssemblyDistr}{$Council_District}}) {
										foreach my $Judicial_District (keys %{$RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}{$Ward}{$CongressDistr}{$SenateDistr}{$AssemblyDistr}{$Council_District}{$Civil_Court_District}}) {
	
											#print "Result: CountY:$CountyCode ED:$ElectDistr AD:$AssemblyDistr Sen:$SenateDistr LegI:$LegisDistr Town:$TownCity Ward:$Ward Congress:$CongressDistr Council:$Council_District  Civil:$Civil_Court_District Judicial:$Judicial_District:\t";
											#print $RepMyBlock::CacheDataDistrict {RemoveLeadingZero($CountyCode)}{$ElectDistr}{$AssemblyDistr}{$SenateDistr}{$LegisDistr}{$TownCity}{$Ward}{$CongressDistr}{RemoveLeadingZero($Council_District)}{RemoveLeadingZero($Civil_Court_District)}{RemoveLeadingZero($Judicial_District)} .  "\n";	


											my $DataHouseDistrictID = $RepMyBlock::CacheDataHouseDistrict{$DataDistrictTemporal_GroupID}{$CountyCode}{$ElectDistr}{$LegisDistr}{$Ward}{$CongressDistr}{$SenateDistr}{$AssemblyDistr}{$Council_District}{$Civil_Court_District}{$Judicial_District};			
										 	my $DataDistrictID = $RepMyBlock::CacheDataDistrict {$RepMyBlock::CacheCountyTranslation{RemoveLeadingZero($CountyCode)}}{$ElectDistr}{$AssemblyDistr}{$SenateDistr}{RemoveLeadingZero($LegisDistr)}{$Ward}{$CongressDistr}{RemoveLeadingZero($Council_District)}{RemoveLeadingZero($Civil_Court_District)}{RemoveLeadingZero($Judicial_District)};
										 	
											$RepMyBlock::TemporalCacheGroupID {$Cycle_ID}{ $DataDistrictID } {$DataHouseDistrictID} {''} {''} = $Counter++;
											
											if ( ($Counter % 100000) == 0 ) {
												print "Done loading $Counter districts\n\033[1A";													
											}
																																		
											# DataCounty_ID, DataDistrict_Electoral, DataDistrict_StateAssembly, DataDistrict_SenateSenate, DataDistrict_Legislative, DataDistrict_TownCity, DataDistrict_Ward, DataDistrict_Congress, DataDistrict_Council, DataDistrict_CivilCourt, DataDistrict_Judicial " .
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
#print "Done going the prep of Temporal\n";
#print Dumper (%RepMyBlock::TemporalCacheGroupID);

$RepMyBlock->DbAddToDataDistrictTemporal();			
$RepMyBlock->DBUpdateDataHouseDB();
exit();

sub RemoveLeadingZero {
	my $str = $_[0];
	
	$str =~ s/^0+(?=[0-9])//;
	
	if ($str == 0) { return undef; }
	
	return $str;
}

