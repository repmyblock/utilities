#!/usr/bin/perl
use strict;
use warnings;
use Mojo::UserAgent;
use Mojo::File qw(path);

my $URL = 'https://s3.amazonaws.com/dl.ncsbe.gov/Elections/2026/Candidate%20Filing/Candidate_Listing_2026.csv';
my $OUT = 'CandidateList.txt';

# --------------------------------------------------
# FORM — USE DECODED VALUES (Mojo will encode)
# --------------------------------------------------
my %FORM = (
    elecID     => '20261103-GEN',
    office     => 'All',
    status     => 'All',
    cantype    => 'ALL',
    FormSubmit => 'Download Candidate List',
);

# --------------------------------------------------
# USER AGENT
# --------------------------------------------------
my $ua = Mojo::UserAgent->new;
$ua->transactor->name(
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:146.0) Gecko/20100101 Firefox/146.0'
);

# --------------------------------------------------
# BUILD TRANSACTION (DON’T SEND YET)
# --------------------------------------------------
my $tx = $ua->build_tx(
    POST => $URL => {
        Referer => 'https://dos.elections.myflorida.com/candidates/downloadcanlist.asp',
        Origin  => 'https://dos.elections.myflorida.com',
        Accept  => 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
    } => form => \%FORM
);

# --------------------------------------------------
# DEBUG: SHOW EXACT REQUEST
# --------------------------------------------------
print "\n=== OUTGOING REQUEST ===\n";
print $tx->req->to_string;

# --------------------------------------------------
# SEND
# --------------------------------------------------
$ua->start($tx);

my $res = $tx->result;

die "Request failed: " . $res->message
    unless $res->is_success;

# --------------------------------------------------
# SAVE FILE
# --------------------------------------------------
path($OUT)->spurt($res->body);

print "\n✅ Downloaded $OUT (" . length($res->body) . " bytes)\n";
