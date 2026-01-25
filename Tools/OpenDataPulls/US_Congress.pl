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

# ============================================================
# CONFIG
# ============================================================
my $YEAR = shift || die "Usage: $0 <4-digit-year>\n";
die "Invalid year\n" unless $YEAR =~ /^\d{4}$/;

my $DRY_RUN = 0;

my $YY        = substr($YEAR, 2, 2);
my $BASE_URL  = "https://www.fec.gov/files/bulk-downloads";
my $ZIP_URL   = "$BASE_URL/$YEAR/cn$YY.zip";

my $WORKDIR   = $ENV{HOME} . "/RepMyBlockData/US/$YEAR";
my $ZIP_FILE  = "$WORKDIR/cn$YY.zip";
my $TXT_FILE  = "$WORKDIR/cn.txt";

make_path($WORKDIR);

# ============================================================
# DB CONNECTION (same pattern as CongressGov script)
# ============================================================
my $dbh = DBI->connect(
    "DBI:mysql:database=RepMyBlock;mysql_read_default_group=RepMyBlockDev",
    undef, undef,
    { RaiseError => 1, PrintError => 0 }
) or die "DB connection failed";

# ============================================================
# LOAD ELECTIONS (TEMP PRIMARY FOR <YEAR>)
# ============================================================
my %ElectionLookup;

my $sth = $dbh->prepare("
    SELECT Elections_ID, DataState_Abbrev
    FROM Elections
    LEFT JOIN DataState USING (DataState_ID)
    WHERE Elections_Date = ?
");
$sth->execute("$YEAR-12-31");

while (my ($eid, $abbr) = $sth->fetchrow_array) {
    $ElectionLookup{$abbr} = $eid;
}

die "No elections found for $YEAR" unless %ElectionLookup;

# ============================================================
# LOAD POSITIONS
# ============================================================
my (%Pos_House, %Pos_Senate);

$sth = $dbh->prepare("
    SELECT ElectionsPosition_ID, ElectionsPosition_Name, DataState_Name
    FROM ElectionsPosition
    LEFT JOIN DataState USING (DataState_ID)
    WHERE ElectionsPosition_Name IN ('U.S. Congress','U.S. Senate')
");
$sth->execute();

while (my $r = $sth->fetchrow_hashref) {
    if ($r->{ElectionsPosition_Name} eq 'U.S. Congress') {
        $Pos_House{$r->{DataState_Name}} = $r->{ElectionsPosition_ID};
    } else {
        $Pos_Senate{$r->{DataState_Name}} = $r->{ElectionsPosition_ID};
    }
}

# ============================================================
# DOWNLOAD + UNZIP
# ============================================================
my $ua = Mojo::UserAgent->new;
my $tx = $ua->get($ZIP_URL);
die "Download failed" unless $tx->result->is_success;

open my $zf, ">", $ZIP_FILE or die;
binmode $zf;
print $zf $tx->result->body;
close $zf;

my $zip = Archive::Zip->new;
$zip->read($ZIP_FILE) == AZ_OK or die;
$zip->extractTree('', "$WORKDIR/") == AZ_OK or die;

# ============================================================
# PARSE + INSERT
# ============================================================
open my $fh, "<", $TXT_FILE or die;

while (my $line = <$fh>) {
    chomp $line;

    my ($regid, $name, $party, $year, $state, $chamber, $district) =
        (split /\|/, $line)[0..6];

    next unless $regid && $name;

    my ($LastName, $FirstName) = split /\s*,\s*/, $name, 2;
    $FirstName ||= '';

    if ($chamber eq 'S') {
        $district = '01';
    } else {
        next unless defined $district;
        $district = sprintf('%02d', $district || 1);
    }

    my $Election_ID = $ElectionLookup{$state} or next;
    my $ElectionsPositionID =
        ($chamber eq 'S')
            ? $Pos_Senate{$state}
            : $Pos_House{$state};

    next unless $ElectionsPositionID;

    print "→ $name ($state-$district)\n";

    next if $DRY_RUN;

    add_candidate(
        dbh                  => $dbh,
        api_name             => $regid,
        first_name           => $FirstName,
        last_name            => $LastName,
        party                => $party,
        state                => $state,
        district             => $district,
        district_text        => "$state-$district",
        district_explain     => "$state district $district",
        elections_id         => $Election_ID,
        elections_position_id=> $ElectionsPositionID,
        dbtable              => ($chamber eq 'S') ? "$state\SN" : "$state\CG",
    );
}

close $fh;

print "\n🎉 DONE — FEC WebWall imported for $YEAR\n";
