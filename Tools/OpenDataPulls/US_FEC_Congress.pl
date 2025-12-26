#!/usr/bin/perl
use strict;
use warnings;
use Mojo::UserAgent;
use File::Path qw(make_path);
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);

# ============================================================
# CONFIG
# ============================================================
my $YEAR = shift || die "Usage: $0 <4-digit-year>\n";
die "Invalid year\n" unless $YEAR =~ /^\d{4}$/;

my $YY        = substr($YEAR, 2, 2);
my $BASE_URL  = "https://www.fec.gov/files/bulk-downloads";
my $ZIP_URL   = "$BASE_URL/$YEAR/cn$YY.zip";
my $WORKDIR   = "fec_webwall/$YEAR";
my $ZIP_FILE  = "$WORKDIR/cn$YY.zip";
my $TXT_FILE  = "$WORKDIR/cn.txt";

make_path($WORKDIR);

# ============================================================
# DOWNLOAD ZIP
# ============================================================
print "⬇ Downloading $ZIP_URL\n";

my $ua = Mojo::UserAgent->new;
$ua->max_redirects(5);

my $tx = $ua->get($ZIP_URL);
die "Download failed\n" unless $tx->result->is_success;

open my $zf, ">", $ZIP_FILE or die "Cannot write ZIP\n";
binmode $zf;
print $zf $tx->result->body;
close $zf;

print "✔ ZIP saved: $ZIP_FILE\n";

# ============================================================
# UNZIP
# ============================================================
print "📦 Extracting ZIP\n";

my $zip = Archive::Zip->new();
die "ZIP read error\n" unless $zip->read($ZIP_FILE) == AZ_OK;
die "Unzip error\n" unless $zip->extractTree('', "$WORKDIR/") == AZ_OK;

die "Missing cn file\n" unless -e $TXT_FILE;
print "✔ Extracted: $TXT_FILE\n";

# ============================================================
# PARSE FILE
# ============================================================
print "📄 Parsing candidates\n";

open my $fh, "<", $TXT_FILE or die "Cannot open txt\n";

while (my $line = <$fh>) {
    chomp $line;

    my (
        $code,
        $full_name,
        $party,
        $year,
        $state,
        $chamber,
        $district,
        @rest
    ) = split /\|/, $line;

    next unless $code && $full_name;

    my $CandidateProfile_RegID     = $code;
    my $CandidateRegAuthority_ID   = 1;

    # Normalize district
    if ($chamber eq 'S') {
        $district = '01';
    } elsif ($district =~ /^\d+$/) {
        $district = sprintf('%02d', $district);
    }

    print join("\t",
        $CandidateProfile_RegID,
        $CandidateRegAuthority_ID,
        $full_name,
        $party,
        $year,
        $state,
        $chamber,
        $district
    ), "\n";

    # ========================================================
    # INSERT POINT (DBI)
    # ========================================================
    # INSERT INTO CandidateProfile (...)
    # CandidateProfile_RegID = $CandidateProfile_RegID
    # CandidateRegAuthority_ID = 1
}

close $fh;

print "🎉 DONE — FEC WebWall processed\n";
