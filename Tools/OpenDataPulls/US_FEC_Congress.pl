#!/usr/bin/perl
use strict;
use warnings;

use Mojo::UserAgent;
use File::Path qw(make_path);
use Archive::Zip qw(:ERROR_CODES);
use FindBin;
use DBI;

use lib "$FindBin::Bin";
use RepMyBlock::Candidate qw(add_candidate);

# ============================================================
# CONFIG / ARGUMENTS
# ============================================================
my $YEAR = shift or die "Usage: $0 <4-digit-year>\n";
die "Invalid year\n" unless $YEAR =~ /^\d{4}$/;

my $DRY_RUN = 0;

my $YY       = substr($YEAR, 2, 2);
my $BASE_URL = "https://www.fec.gov/files/bulk-downloads";
my $ZIP_URL  = "$BASE_URL/$YEAR/cn$YY.zip";

my $WORKDIR  = "fec_webwall/$YEAR";
my $ZIP_FILE = "$WORKDIR/cn$YY.zip";
my $TXT_FILE = "$WORKDIR/cn.txt";

make_path($WORKDIR);

# ============================================================
# DB CONNECTION
# ============================================================
my $dbh = DBI->connect(
    "DBI:mysql:database=RepMyBlock;mysql_read_default_group=RepMyBlockProd;mysql_ssl=1",
    undef, undef,
    { RaiseError => 1, PrintError => 0, AutoCommit => 1 }
) or die "❌ DB connection failed";

print "✅ DB connected\n";

# ============================================================
# LOAD ELECTIONS (STATE → Elections_ID)
# ============================================================
my %ElectionByState;

my $sth = $dbh->prepare(q{
    SELECT Elections_ID, DataState_Abbrev
    FROM Elections
    LEFT JOIN DataState USING (DataState_ID)
});
$sth->execute;

while (my ($eid, $state) = $sth->fetchrow_array) {
    $ElectionByState{$state} = $eid;
}

die "❌ No Elections loaded for $YEAR\n" unless %ElectionByState;

# ============================================================
# LOAD POSITIONS (DBTABLE → ElectionsPosition_ID)
# ============================================================
my %PositionByDBTable;

$sth = $dbh->prepare(q{
    SELECT ElectionsPosition_ID, ElectionsPosition_DBTable
    FROM ElectionsPosition
});
$sth->execute;

while (my ($pid, $dbtable) = $sth->fetchrow_array) {
    $PositionByDBTable{$dbtable} = $pid;
}

# ============================================================
# DOWNLOAD + EXTRACT FEC ZIP
# ============================================================
my $ua = Mojo::UserAgent->new(
    max_redirects => 5,
    inactivity_timeout => 30,
);

$ua->transactor->name(
    'Mozilla/5.0 (X11; Linux x86_64) Gecko/20100101 Firefox/121.0'
);

my $tx = $ua->get($ZIP_URL);
die "❌ Download failed\n" unless $tx->result->is_success;

open my $zf, ">", $ZIP_FILE or die "❌ Cannot write ZIP\n";
binmode $zf;
print $zf $tx->result->body;
close $zf;

my $zip = Archive::Zip->new;
$zip->read($ZIP_FILE) == AZ_OK     or die "❌ ZIP read error\n";
$zip->extractTree('', "$WORKDIR/") == AZ_OK or die "❌ ZIP extract error\n";

# ============================================================
# PARSE + INSERT CANDIDATES
# ============================================================
open my $fh, "<", $TXT_FILE or die "❌ Cannot open cn.txt\n";

while (my $line = <$fh>) {
    chomp $line;

    my ($regid, $name, $party, undef, $state, $chamber, $district) =
        (split /\|/, $line)[0..6];

    next unless $regid && $name && $state && $chamber;

		if ($district eq "00") { $district = "01"; }

    $party ||= 'IND';

    # Name: LAST, FIRST
    my ($LastName, $FirstName) = split /\s*,\s*/, $name, 2;
    $FirstName ||= '';

    # Normalize district
    if ($chamber eq 'S') {
        $district = '01';
    } else {
        next unless defined $district;
        $district = sprintf('%02d', $district || 1);
    }

    my $Election_ID = $ElectionByState{$state} or next;

    # DBTable mapping
    my $DBTable =
        ($chamber eq 'S')
            ? "${state}SN"
            : "${state}CG";

    my $ElectionsPositionID = $PositionByDBTable{$DBTable};
    next unless $ElectionsPositionID;

    print "→ $name [$state-$district] $party ($DBTable)\n";

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
        elections_position_id => $ElectionsPositionID,
        dbtable               => $DBTable,
        regid                 => $regid,
    );
}

close $fh;

print "\n🎉 DONE — FEC WebWall candidates inserted for $YEAR\n";
