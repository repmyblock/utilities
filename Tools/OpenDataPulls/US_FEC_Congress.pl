#!/usr/bin/perl
use strict;
use warnings;

use Mojo::UserAgent;
use File::Path qw(make_path);
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);
use FindBin;
use DBI;

use lib "$FindBin::Bin";
use RepMyBlock::Candidate qw(add_candidate);

BEGIN {
    print "Loaded Candidate.pm from: $INC{'RepMyBlock/Candidate.pm'}\n";
}


# ============================================================
# CONFIG
# ============================================================
my $YEAR = shift || die "Usage: $0 <4-digit-year>\n";
die "Invalid year\n" unless $YEAR =~ /^\d{4}$/;

my $DRY_RUN = 0;   # set to 1 to test without DB writes

my $YY        = substr($YEAR, 2, 2);
my $BASE_URL  = "https://www.fec.gov/files/bulk-downloads";
my $ZIP_URL   = "$BASE_URL/$YEAR/cn$YY.zip";

my $WORKDIR   = "fec_webwall/$YEAR";
my $ZIP_FILE  = "$WORKDIR/cn$YY.zip";
my $TXT_FILE  = "$WORKDIR/cn.txt";

make_path($WORKDIR);

# ============================================================
# DB CONNECTION
# ============================================================
my $dbh = DBI->connect(
    "DBI:mysql:database=RepMyBlock;mysql_read_default_group=RepMyBlockDev",
    undef, undef,
    { RaiseError => 1, PrintError => 0, AutoCommit => 1 }
) or die "❌ DB connection failed";

print "✅ DB connected\n";

# ============================================================
# LOAD TEMP PRIMARY ELECTIONS FOR YEAR
# ============================================================
my %ElectionLookup;

my $sth = $dbh->prepare(q{
    SELECT Elections_ID, DataState_Abbrev
    FROM Elections
    LEFT JOIN DataState USING (DataState_ID)
});
$sth->execute();

while (my ($eid, $abbr) = $sth->fetchrow_array) {
    $ElectionLookup{$abbr} = $eid;
}

die "❌ No TEMP PRIMARY Elections for $YEAR"
    unless %ElectionLookup;

# ============================================================
# LOAD HOUSE / SENATE POSITIONS
# ============================================================
my (%Pos_House, %Pos_Senate);

$sth = $dbh->prepare(q{
    SELECT *
    FROM ElectionsPosition
    LEFT JOIN DataState USING (DataState_ID)
    WHERE ElectionsPosition_Name IN ('U.S. Congress','U.S. Senate')
});
$sth->execute();

while (my $r = $sth->fetchrow_hashref) {
	$Pos_House{$r->{ElectionsPosition_DBTable}} = $r->{ElectionsPosition_ID};
}

# ============================================================
# DOWNLOAD + UNZIP FEC WEBWALL
# ============================================================

my $ua = Mojo::UserAgent->new(
    max_redirects => 5,
    inactivity_timeout => 30,
);

$ua->transactor->name(
    'Mozilla/5.0 (X11; Linux x86_64) Gecko/20100101 Firefox/121.0'
);

my $tx = $ua->get($ZIP_URL);

unless ($tx->result->is_success) {
    my $res = $tx->result;
    die sprintf(
        "❌ Download failed: HTTP %s %s\nURL: %s\n",
        $res->code // 'N/A',
        $res->message // 'No message',
        $ZIP_URL,
    );
}

open my $zf, ">", $ZIP_FILE or die "❌ Cannot write ZIP";
binmode $zf;
print $zf $tx->result->body;
close $zf;

my $zip = Archive::Zip->new;
$zip->read($ZIP_FILE) == AZ_OK or die "❌ ZIP read error";
$zip->extractTree('', "$WORKDIR/") == AZ_OK or die "❌ ZIP extract error";

# ============================================================
# PARSE + INSERT
# ============================================================
open my $fh, "<", $TXT_FILE or die "❌ Cannot open cn.txt";

while (my $line = <$fh>) {
    chomp $line;
  
    my ($regid, $name, $party, $year, $state, $chamber, $district) =
        (split /\|/, $line)[0..6];

		#print "PARTY: $party\n";
		if (! $party) { $party = "IND";	print "I am here\n"; }

    next unless $regid && $name;

    # Split "LAST, FIRST"
    my ($LastName, $FirstName) = split /\s*,\s*/, $name, 2;
    $FirstName ||= '';

    # Normalize district
    if ($chamber eq 'S') {
        $district = '01';          # Senate = whole state
    } else {
        next unless defined $district;
        $district = sprintf('%02d', $district || 1);
    }

    my $Election_ID = $ElectionLookup{$state} or next;
    print "Chamber: $chamber Election ID: $state $Election_ID\t";


   my $ElectionsPositionID = 0;
#        ($chamber eq 'S')
#            ? $Pos_Senate{$state}
#            : $Pos_House{$state};
#            
#            print "CHAMBER: $chamber\n";
#            print "Post_Senate: " . $Pos_Senate{$state} . "\n";
#    next unless $ElectionsPositionID;

		
    # DBTable mapping (IMPORTANT)
    my $DBTable =
        ($chamber eq 'S')
            ? "${state}SN"   # <STATE>SN
            : "${state}CG";  # <STATE>CG

    print "→ $name [$state-$district]\t($DBTable) - $party - $district -> " . $Pos_House{$DBTable} . "\n";
    
    if ( ! defined $Pos_House{$DBTable} ) { 
    	if ( $DBTable ne "DCSN") {
	    	exit(); 
			}
		}
    next if $DRY_RUN;


    add_candidate( 
        dbh                   => $dbh,
        api_name              => $name,
        first_name            => $FirstName,
        last_name             => $LastName,
        party                 => $party,
        state                 => $state,
        district              => $district,
        district_text         => $DBTable,
        district_explain      => "$state district $district",
        elections_id          => $Election_ID,
        elections_position_id => $Pos_House{$DBTable},
        dbtable               => $DBTable,
        regid									=> $regid,
    ) if ($Pos_House{$DBTable});
   
}

close $fh;

print "\n🎉 DONE — FEC WebWall candidates inserted for $YEAR\n";

