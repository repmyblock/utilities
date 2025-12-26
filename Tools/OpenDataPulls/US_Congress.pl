#!/usr/bin/perl
use strict;
use warnings;
use Mojo::UserAgent;
use JSON::XS;
use Time::Piece;
use File::Path qw(make_path);
use File::Basename;
use Getopt::Long;
use DBI;

# ================================================================
# CONFIGURATION
# ================================================================
my $CACHE_DIR = "cache";
make_path($CACHE_DIR) unless -d $CACHE_DIR;

my $DB_NAME = "RepMyBlock";

# Command-line flags
my $FORCE_REFRESH = 0;

GetOptions(
    "refresh" => \$FORCE_REFRESH,
);

# ================================================================
# Load API Key
# ================================================================
my $keyfile = "$ENV{HOME}/keys/opendata.congressgov\@repmyblock.org";
open(my $fh, "<", $keyfile) or die "Cannot read API key file: $!";
my $API_KEY = <$fh>;
chomp($API_KEY);
close($fh);

my $ua = Mojo::UserAgent->new;

# ================================================================
# Fetch Congress Metadata
# ================================================================
sub fetch_congress_metadata {
    my $url = "https://api.congress.gov/v3/congress?api_key=$API_KEY";
    return $ua->get($url)->result->json;
}

print "⏳ Fetching Congress metadata…\n";
my $metadata = fetch_congress_metadata();

my $today = localtime;
my $current_year = $today->year;

print "📅 Current Year: $current_year\n";

# Determine active congress
my ($current_congress_name, $current_congress_id);

foreach my $entry (@{ $metadata->{congresses} }) {

    my $start = $entry->{startYear} + 0;
    my $end   = $entry->{endYear}   + 0;

    if ($current_year >= $start && $current_year <= $end) {
        $current_congress_name = $entry->{name};
        ($current_congress_id  = $entry->{name}) =~ s/\D//g;
        last;
    }
}

die "❌ No congress matches year $current_year\n"
    unless $current_congress_id;

print "🏛 Using Congress: $current_congress_name (ID $current_congress_id)\n";

# ================================================================
# CACHE FUNCTIONS
# ================================================================
sub load_cache {
    my ($file) = @_;
    return unless -e $file;
    print "📦 Loading cache: $file\n";
    open my $cfh, "<", $file or die "Cannot open cache: $!";
    local $/;
    my $json = <$cfh>;
    close $cfh;
    return decode_json($json);
}

sub save_cache {
    my ($file, $data) = @_;
    open my $cfh, ">", $file or die "Cannot write cache: $!";
    print $cfh encode_json($data);
    close $cfh;
    print "💾 Cache saved: $file\n";
}

# ================================================================
# FETCH MEMBERS (WITH CACHE)
# ================================================================
my $cache_file = "$CACHE_DIR/members_congress_$current_congress_id.json";
my @all_members;

if (!$FORCE_REFRESH && -e $cache_file) {
    my $cached = load_cache($cache_file);
    @all_members = @{ $cached->{members} };
    print "✔ Loaded " . scalar(@all_members) . " members from cache.\n";
}
else {
    print "🌐 Fetching members from Congress API…\n";

    my $offset = 0;
    my $limit  = 250;

    while (1) {
        my $url =
          "https://api.congress.gov/v3/member/congress/$current_congress_id"
          . "?limit=$limit&offset=$offset&api_key=$API_KEY";

        my $response = $ua->get($url)->result->json;
        my $batch = $response->{members};

        last unless $batch && @$batch;

        push @all_members, @$batch;

        print "  → Fetched offset=$offset count=" . scalar(@$batch) . "\n";

        last if scalar(@$batch) < $limit;
        $offset += $limit;
    }

    save_cache($cache_file, { members => \@all_members });
}

print "\n📊 Total Members Available: " . scalar(@all_members) . "\n";

# ================================================================
# PARTY NORMALIZATION
# ================================================================
sub normalize_party {
    my ($party) = @_;
    return "DEM" if $party =~ /Dem/i;
    return "REP" if $party =~ /Rep/i;
    return "IND" if $party =~ /Ind/i;
    return "OTH";
}

# ================================================================
# CONNECT TO DATABASE
# ================================================================
print "\n🔌 Connecting to MySQL (using login-path=RepMyBlock)…\n";

my $dbh = DBI->connect(
    "DBI:mysql:database=$DB_NAME;mysql_read_default_group=RepMyBlock",
    undef, undef,
    { RaiseError => 1, PrintError => 0 }
) or die "❌ DB connection failed\n";


# ================================================================
# LOAD REQUIRED DB IDs
# ================================================================
print "🔍 Loading election + position IDs…\n";

# --------------------------------------------------------------
# Load ALL Elections and State information for 2024-11-05
# --------------------------------------------------------------

my $sth = $dbh->prepare("
    SELECT
        Elections.Elections_ID,
        Elections.DataState_ID,
        DataState.DataState_Name,
        DataState.DataState_Abbrev
    FROM Elections
    LEFT JOIN DataState
           ON Elections.DataState_ID = DataState.DataState_ID
    WHERE Elections.Elections_Date = '2024-11-05' ORDER BY DataState.DataState_Abbrev
");
$sth->execute();

# Hash of all elections indexed by state name & abbrev
my (%ElectionLookup, %StateLookup);

while (my ($Elections_ID, $DataState_ID, $State_Name, $State_Abbrev) = $sth->fetchrow_array) {
    print "Loaded Election $Elections_ID for $State_Name ($State_Abbrev)\n";
    # Store two-way lookup
    $ElectionLookup{$State_Abbrev} = $Elections_ID;
    $StateLookup{$State_Name}   = $State_Abbrev;
}

# Safety check
if (!%ElectionLookup) {
   die "❌ No elections found for 2024-11-05!\n";
}

print "\nTotal Elections Loaded: " . (scalar keys %ElectionLookup) . "\n";

# --------------------------------------------------------------
# Load all states from DataState where ID < 57
# --------------------------------------------------------------

$sth = $dbh->prepare("
    SELECT DataState_Name, DataState_Abbrev
    FROM DataState
    WHERE DataState_ID < 57
");
$sth->execute();

my (%AllStates, %AllAbbrev);
while (my ($name, $abbrev) = $sth->fetchrow_array) {
    $AllStates{$abbrev} = $name;
    $AllAbbrev{$name} = $abbrev;
}

print "Total states/territories from DataState (<57): " . scalar(keys %AllStates) . "\n";

# --------------------------------------------------------------
# Compare to your loaded Elections list
# %ElectionLookup contains abbrev => Elections_ID
# --------------------------------------------------------------

my @missing;

foreach my $abbrev (sort keys %AllStates) {
    if (!exists $ElectionLookup{$abbrev}) {
        push @missing, sprintf("%s (%s)", $AllStates{$abbrev}, $abbrev);
    }
}

# --------------------------------------------------------------
# Print results
# --------------------------------------------------------------

if (@missing) {
    print "\n❌ Missing Elections for the following states/territories:\n";
    print " - $_\n" for @missing;
} else {
    print "\n🎉 All DataState entries (<57) have matching Elections rows.\n";
}

# Load Senate + House positions
$sth = $dbh->prepare("
    SELECT *
    FROM ElectionsPosition LEFT JOIN DataState ON (ElectionsPosition.DataState_ID = DataState.DataState_ID)
    WHERE ElectionsPosition_Name IN ('U.S. Congress','U.S. Senate')
");
$sth->execute();

my (%Pos_House, %Pos_Senate) = ();
while (my $row = $sth->fetchrow_hashref) {
	$Pos_House{$row->{'DataState_Name'}}  = $row->{'ElectionsPosition_ID'} if $row->{'ElectionsPosition_Name'} eq 'U.S. Congress';
	$Pos_Senate{$row->{'DataState_Name'}} = $row->{'ElectionsPosition_ID'} if $row->{'ElectionsPosition_Name'} eq 'U.S. Senate';
	print $row->{'ElectionsPosition_Name'} . "\tState: " . $row->{'DataState_Name'} . "\tElectionsPosition_ID: " .  $row->{'ElectionsPosition_ID'} . "\n";
}

# ================================================================
# PROCESS MEMBERS (MATCH / INSERT / UPDATE)
# ================================================================
foreach my $m (@all_members) {

	my $api_name = $m->{name};
	next unless $api_name;

	#	my ($LastName, $FirstName) = $api_name =~ /^([^,]+),\s*(.+)$/;
	my ($LastName, $FirstName) = $api_name =~ /^([^,]+),\s*([^\s,]+)/;

	my $party_long = $m->{partyName} // "Unknown";
	my $party3     = normalize_party($party_long);

	my $state    = $m->{state} // 'Unknown';
	
	if ( $state eq 'Virgin Islands') { $state = 'U.S. Virgin Islands'; }
	
	# Determine chamber
	my $chamber = $m->{terms}{item}[0]{chamber} // "Unknown";

	# Fix district rules
	my $district;
	
	my $district_text = "Representative in Congress";
	my $district_explain = "District Congress";
	my $district_type = "Congressional";
	my $district_ordinal;
	
	print "Elected: " . $api_name . " - Chamber: " . $chamber . "\n";
	
	
	if ($chamber eq "Senate") {
	  # All Senators represent the entire state
	  $district = "01";
	  $district_text = "Representative in Senate";
	  $district_explain = "District Senate";
	 	$district_type = "Senatorial";
	 	$district_ordinal = '1st';
	  
	} elsif (!defined $m->{district}) {
	  # At-Large House representatives also have null district
	  $district = "XX";
	  next;
	  
	}	elsif ($m->{district} == 0) {
	  # Incorrect 0 value → convert to district 1
	  $district = "01";
	  $district_ordinal = '1st';
	  
	}	else {		
		if ( $m->{district} < 10) { $district = sprintf('%02d', $m->{district}); } else { $district = $m->{district}; }
		if    ($m->{district} == 1) { $district_ordinal = '1st' }
	  elsif ($m->{district} == 2) { $district_ordinal = '2nd' }
	  elsif ($m->{district} == 3) { $district_ordinal = '3rd' }
	  else                        { $district_ordinal = $m->{district} . 'th' }
	}
	
  # Optional ordinal logic (example)
	$district_text .= ", " . $district_ordinal . " " . $district_type . " District, State of " . $state;
	$district_explain =  $district_ordinal .  " " . $district_explain;
	
	my $ElectionsPositionID =
	    ($chamber eq "Senate") ? $Pos_Senate{$state} : $Pos_House{$state};

  print "\n➡ Processing $api_name ($chamber) - $state - $district\n";    
  print "State => Abbrev: " . $AllAbbrev{$state} . "\tElectionsPositionID: " . $ElectionsPositionID . "\n";

	if (! $ElectionsPositionID > 0 ) { die "The Candidate Election ID doesn't exist"; }
	print "First Name: ". $api_name . " First: " .  $FirstName . " Last: " .  $LastName . "\n";
	
  # Find CandidateProfile
  $sth = $dbh->prepare("SELECT CandidateProfile_ID, Candidate_ID FROM CandidateProfile WHERE CandidateProfile_Alias = ?");
  $sth->execute($api_name);
  my ($Profile_ID, $Candidate_ID) = $sth->fetchrow_array;
  
  if (!$Profile_ID) {
    print " ❌ Missing CandidateProfile for $api_name need to add it.\n";
    $sth = $dbh->prepare("INSERT INTO CandidateProfile SET CandidateProfile_Alias = ?,
    			CandidateProfile_FirstName = ?, CandidateProfile_LastName = ?,
    			CandidateProfile_LastModified = NOW(), CandidateProfile_PublishProfile = 'yes'");
		$sth->execute($api_name, $FirstName, $LastName);
		$Profile_ID = $dbh->{mysql_insertid};
		#exit();
  }

	# Find candidate in Candidate ELection
  $sth = $dbh->prepare("SELECT * FROM CandidateElection WHERE ElectionsPosition_ID = ? AND CandidateElection_DBTableValue = ?");
  $sth->execute($ElectionsPositionID, $district);
  my $CandidateElection_ID = ($sth->fetchrow_array())[0];
  print "CandidateElection_ID found: " . $CandidateElection_ID . "\n" if defined($CandidateElection_ID);
  
	if (!$CandidateElection_ID) {
    print " ❌ Missing CandidateElection for $ElectionsPositionID need to add it:";
		print " Elections_ID = " . $ElectionLookup{$StateLookup{$state}} . "\n";

		$sth = $dbh->prepare("SELECT * FROM ElectionsPosition WHERE ElectionsPosition_ID = ?");
		$sth->execute($ElectionsPositionID);
		my $row = $sth->fetchrow_hashref();
		print "Inserting the State DT Table: " . $row->{'ElectionsPosition_DBTable'} . "\n";
		
    $sth = $dbh->prepare("INSERT INTO CandidateElection SET Elections_ID = ?,
			    ElectionsPosition_ID = ?,
					CandidateElection_PositionType = ?, CandidateElection_Text = ?,
					CandidateElection_PetitionText = ?, CandidateElection_Number = ?,
					CandidateElection_DBTable = ?, CandidateElection_DBTableValue = ?");
		$sth->execute($ElectionLookup{$StateLookup{$state}}, $ElectionsPositionID, 'electoral', 
					$district_explain, $district_text, 1, $row->{'ElectionsPosition_DBTable'}, 
					$district);
					
		$CandidateElection_ID = $dbh->{mysql_insertid};
					
		#exit();
  }
  
  $sth = $dbh->prepare("SELECT * FROM Candidate WHERE CandidateElection_ID = ? AND Candidate_PetitionNameset = ?");
	$sth->execute($CandidateElection_ID, $api_name);
	$Candidate_ID = ($sth->fetchrow_array())[0];

	if (!$Candidate_ID) {
    print " ❌ Missing Candidate for $api_name need to add it:";
		print " PARTY = " . $party3  . "\n";

		$sth = $dbh->prepare("SELECT * FROM ElectionsPosition WHERE ElectionsPosition_ID = ?");
		$sth->execute($ElectionsPositionID);
		my $row = $sth->fetchrow_hashref();
		print "Inserting the State DT Table: " . $row->{'ElectionsPosition_DBTable'} . "\n";
		
    $sth = $dbh->prepare("INSERT INTO Candidate SET CandidateProfile_ID = ?,
			    Candidate_PetitionNameset = ?,
					CandidateElection_ID = ?, Candidate_Party = ?,
					Candidate_DispName = ?, CandidateElection_DBTable = ?,
					CandidateElection_DBTableValue = ?, Candidate_Status = ?");
		$sth->execute($Profile_ID, $api_name, $CandidateElection_ID, $party3, $api_name, 
					$row->{'ElectionsPosition_DBTable'},$district, 'published');
					
		$Candidate_ID = $dbh->{mysql_insertid};
					
		#exit();
  }
  
	### UPDATE THE CANDIDATE_PROFILE NOW
	$sth = $dbh->prepare("SELECT * FROM CandidateProfile WHERE CandidateProfile_ID = ? AND Candidate_ID IS NULL");
	$sth->execute($Profile_ID);
	my $NewCandidateProfile_ID = ($sth->fetchrow_array())[0];
	
	if ($NewCandidateProfile_ID) {
    print " ❌ Updating the CandidateProfile for $api_name need to add it\n";
		$sth = $dbh->prepare("UPDATE CandidateProfile SET Candidate_ID = ? WHERE CandidateProfile_ID = ?");
		$sth->execute($Candidate_ID, $NewCandidateProfile_ID);
    #exit();
	}
	
	
	
	### CHECK THAT It's IN THE ELECTED GROUP.
	$sth = $dbh->prepare("SELECT * FROM ElectResultCandidate WHERE CandidateProfile_ID = ? AND CandidateElection_ID = ?");
	$sth->execute($Profile_ID, $CandidateElection_ID);
	my $ElectResultCandidate = ($sth->fetchrow_array())[0];
	if (!$ElectResultCandidate) {
    print " ❌ Missing Result for $api_name need to add it.\n";
    $sth = $dbh->prepare("INSERT INTO ElectResultCandidate SET CandidateProfile_ID = ?,
    			CandidateElection_ID = ?, ElectResultCandidate_Elected = ?,
    			ElectResultCandidate_InOffice = ?");
		$sth->execute($Profile_ID, $CandidateElection_ID, 'yes', 'yes');
		#exit();
  }
	

}


print "\n🎉 DONE — All Congress members processed.\n";
