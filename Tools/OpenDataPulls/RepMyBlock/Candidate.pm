package RepMyBlock::Candidate;

use strict;
use warnings;
use DBI;
use Exporter 'import';

our @EXPORT_OK = qw(add_candidate);

sub add_candidate {
    my (%args) = @_;

    my $dbh = $args{dbh} or die "dbh required";

    # Required args
    for my $k (qw(
        api_name first_name last_name party state
        district district_text district_explain
        elections_id elections_position_id
        dbtable regid
    )) {
        die "$k required" unless defined $args{$k};
    }

    my ($sth, $Candidate_ID, $CandidateProfile_ID, $CandidateElection_ID);

    # =====================================================
    # 1️⃣ FIND EXISTING CANDIDATE
    # =====================================================
    $sth = $dbh->prepare(
        "SELECT Candidate_ID FROM Candidate WHERE Candidate_PetitionNameset = ?"
    );
    $sth->execute($args{api_name});
    ($Candidate_ID) = $sth->fetchrow_array;

    # =====================================================
    # EXISTING CANDIDATE
    # =====================================================
    if ($Candidate_ID) {

        # ---- CandidateProfile
        $sth = $dbh->prepare(
            "SELECT CandidateProfile_ID FROM PublicProfile WHERE Candidate_ID = ?"
        );
        $sth->execute($Candidate_ID);
        ($CandidateProfile_ID) = $sth->fetchrow_array;

        if (!$CandidateProfile_ID) {
            $CandidateProfile_ID = _insert_candidate_profile($dbh, \%args);
            _upsert_public_profile($dbh, $CandidateProfile_ID, $Candidate_ID);
        }

        return $CandidateProfile_ID;
    }

    # =====================================================
    # 2️⃣ RESOLVE / CREATE CandidateElection
    # =====================================================
    $sth = $dbh->prepare(
        "SELECT * FROM CandidateElection
         WHERE CandidateElection_DBTable = ?
           AND CandidateElection_DBTableValue = ?
           AND ElectionsPosition_ID = ?
         ORDER BY Elections_ID DESC"
    );
    $sth->execute(
        $args{district_text},
        $args{district},
        $args{elections_position_id}
    );

   my $ElectionRow = $sth->fetchrow_hashref;

		unless ($ElectionRow) {
    warn sprintf(
        "⚠️  Skipping candidate — no CandidateElection template found: DBTable=%s PositionID=%s\n",
        $args{district_text},
        $args{elections_position_id},
    	);
    	return;   # ⬅ jump back to caller, next record continues
		}		
		
    if ($ElectionRow->{Elections_ID} != $args{elections_id}) {

        $sth = $dbh->prepare(
            "INSERT INTO CandidateElection SET
                Elections_ID                    = ?,
                ElectionsPosition_ID            = ?,
                CandidateElection_PositionType  = ?,
                CandidateElection_Text          = ?,
                CandidateElection_PetitionText  = ?,
                CandidateElection_Number        = ?,
                CandidateElection_Display       = ?,
                CandidateElection_DBTable       = ?,
                CandidateElection_DBTableValue  = ?"
        );

        $sth->execute(
            $args{elections_id},
            $ElectionRow->{ElectionsPosition_ID},
            $ElectionRow->{CandidateElection_PositionType},
            $ElectionRow->{CandidateElection_Text},
            $ElectionRow->{CandidateElection_PetitionText},
            $ElectionRow->{CandidateElection_Number},
            $ElectionRow->{CandidateElection_Display},
            $ElectionRow->{CandidateElection_DBTable},
            $ElectionRow->{CandidateElection_DBTableValue}
        );

        $CandidateElection_ID = $dbh->{mysql_insertid};
    }
    else {
        $CandidateElection_ID = $ElectionRow->{CandidateElection_ID};
    }

    # =====================================================
    # 3️⃣ INSERT Candidate
    # =====================================================
    $sth = $dbh->prepare(
        "INSERT INTO Candidate SET
            Candidate_Party                 = ?,
            Candidate_DispName              = ?,
            Candidate_PetitionNameset       = ?,
            CandidateElection_DBTable       = ?,
            CandidateElection_DBTableValue  = ?,
            CandidateElection_ID            = ?,
            Candidate_Status                = 'pending'"
    );

    $sth->execute(
        $args{party},
        "$args{first_name} $args{last_name}",
        $args{api_name},
        $args{district_text},
        $args{district},
        $CandidateElection_ID
    );

    $Candidate_ID = $dbh->{mysql_insertid};

    # =====================================================
    # 4️⃣ CandidateProfile + PublicProfile
    # =====================================================
    $CandidateProfile_ID = _insert_candidate_profile($dbh, \%args);
    _upsert_public_profile($dbh, $CandidateProfile_ID, $Candidate_ID);

    return $CandidateProfile_ID;
}

# =====================================================
# HELPERS
# =====================================================
sub _insert_candidate_profile {
    my ($dbh, $args) = @_;

    my $sth = $dbh->prepare(
        "INSERT INTO CandidateProfile SET
            CandidateProfile_PicVerif        = 'no',
            CandidateProfile_PDFVerif        = 'no',
            CandidateProfile_FirstName       = ?,
            CandidateProfile_LastName        = ?,
            CandidateProfile_Alias           = ?,
            CandidateRegAuthority_ID         = ?,
            CandidateProfile_RegID           = ?,
            CandidateProfile_PublishProfile  = 'yes',
            CandidateProfile_Complain        = 'no',
            CandidateProfile_LastModified    = NOW()"
    );

    $sth->execute(
        $args->{first_name},
        $args->{last_name},
        $args->{api_name},
        1,
        $args->{regid}
    );

    return $dbh->{mysql_insertid};
}

sub _upsert_public_profile {
    my ($dbh, $CandidateProfile_ID, $Candidate_ID) = @_;

    my $sth = $dbh->prepare(
        "SELECT PublicProfile_ID FROM PublicProfile
         WHERE CandidateProfile_ID = ? AND Candidate_ID = ?"
    );
    $sth->execute($CandidateProfile_ID, $Candidate_ID);
    my ($PublicProfile_ID) = $sth->fetchrow_array;

    if ($PublicProfile_ID) {
        $sth = $dbh->prepare(
            "UPDATE PublicProfile SET
                PublicProfile_PublishProfile = 'yes',
                CandidateProfile_LastModified = NOW()
             WHERE PublicProfile_ID = ?"
        );
        $sth->execute($PublicProfile_ID);
    }
    else {
        $sth = $dbh->prepare(
            "INSERT INTO PublicProfile SET
                CandidateProfile_ID           = ?,
                Candidate_ID                  = ?,
                PublicProfile_PublishProfile  = 'yes',
                CandidateProfile_LastModified = NOW()"
        );
        $sth->execute($CandidateProfile_ID, $Candidate_ID);
    }
}
1;
