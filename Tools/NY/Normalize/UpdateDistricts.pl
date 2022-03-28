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
my $EmptyDatabase = 0;
my $StopCounterPass = 0;

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

$RepMyBlock->EmptyDatabases("DataDistrict");
$RepMyBlock->EmptyDatabases("DataDistrictTown");
#$RepMyBlockLocal::dbhRawVoters = $dbhRawVoters;
##$RepMyBlockLocal->SetDatabase($dbhVoters);

print "TableDated: $TableDated\n";
my $VoterCounterLocal = $RepMyBlockLocal->LoadRawDistrict($TableDatedLocal);
my $clock0 = clock();

$RepMyBlock->LoadCacheCountyTranslation();

my %LocalCouncil; 
my %LocalCivil;
my %LocalJudicial;

for (my $i = 0; $i < $VoterCounterLocal; $i++) {
	my $ED = RemoveLeadingZero($RepMyBlock::CacheDistrict_Election_District[$i]);
	my $AD = RemoveLeadingZero($RepMyBlock::CacheDistrict_Assembly_District[$i]);
	$LocalCouncil{$AD}{$ED} = RemoveLeadingZero($RepMyBlock::CacheDistrict_Council_District[$i]);
	$LocalCivil{$AD}{$ED} = RemoveLeadingZero($RepMyBlock::CacheDistrict_Civil_Court_District[$i]);
	$LocalJudicial{$AD}{$ED} = RemoveLeadingZero($RepMyBlock::CacheDistrict_Judicial_District[$i]);
}
	
my $VoterCounter = $RepMyBlock->LoadRawDistrict($TableDated);
print "\nStarted with $VoterCounter\n";

my $StartDate = "2010-11-03";

#### Before going into the cycle, we need to load the Towns.
my %CacheTown = ();
for (my $i = 0; $i < $VoterCounter; $i++) {
	if ( defined $RepMyBlock::CacheDistrict_TownCity[$i]) {
		$CacheTown { uc $RepMyBlock::CacheDistrict_TownCity[$i] } = 1;	
	}
}
my $i = 0;
foreach my $TownName (keys %CacheTown) {
	$RepMyBlock::AddPoolDistrictTown[$i] = $TownName;
	$i++;
}

$RepMyBlock->AddDistrictTown();

for (my $i = 0; $i < $VoterCounter; $i++) {
	my $ED = RemoveLeadingZero($RepMyBlock::CacheDistrict_ElectDistr[$i]);
	my $AD = RemoveLeadingZero($RepMyBlock::CacheDistrict_AssemblyDistr[$i]);

	my $DBTable = "ADED";
	my $DBValue = $AD . sprintf( "%03d", $ED );

	$RepMyBlock::CacheDataDistrict
		{'1'}
		{$DBTable}
		{$DBValue}
		{$RepMyBlock::CacheCountyTranslation{RemoveLeadingZero($RepMyBlock::CacheDistrict_CountyCode[$i])}}{$ED}{$AD}
		{$RepMyBlock::CacheDistrict_SenateDistr[$i]}
		{$RepMyBlock::CacheDistrict_LegisDistr[$i]}
		{$RepMyBlock::CacheDistrict_Ward[$i]}
		{$RepMyBlock::CacheDistrict_CongressDistr[$i]}
		{$LocalCouncil{$AD}{$ED}}
		{$LocalCivil{$AD}{$ED}}
		{$LocalJudicial{$AD}{$ED}} = -1;
}

$RepMyBlock->DbAddToDataDistrict();

sub RemoveLeadingZero {
	my $str = $_[0];
	
	$str =~ s/^0+(?=[0-9])//;
	return $str;
}

