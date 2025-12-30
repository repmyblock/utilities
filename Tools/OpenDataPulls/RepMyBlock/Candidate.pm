package RepMyBlock::Candidate;

use strict;
use warnings;
use DBI;

use Exporter 'import';     # 🔥 THIS IS THE MISSING LINE
our @EXPORT_OK = qw(add_candidate);

# ============================================================
# add_candidate
# ------------------------------------------------------------
# Inserts or links:
# - CandidateProfile
# - CandidateElection
# - Candidate
# - ElectResultCandidate
#
# Returns: Candidate_ID
# ============================================================
sub add_candidate {
    my (%args) = @_;

    my $dbh                  = $args{dbh}                  or die "dbh required";
    my $api_name             = $args{api_name}             or die "api_name required";
    my $FirstName            = $args{first_name}            or die "first_name required";
    my $LastName             = $args{last_name}             or die "last_name required";
    my $party                = $args{party}                 or die "party required";
    my $state                = $args{state}                 or die "state required";
    my $district             = $args{district}              or die "district required";
    my $district_text        = $args{district_text}         or die "district_text required";
    my $district_explain     = $args{district_explain}      or die "district_explain required";
    my $Election_ID          = $args{elections_id}          or die "elections_id required";
    my $ElectionsPositionID  = $args{elections_position_id} or die "elections_position_id required";
    my $DBTable              = $args{dbtable}               or die "dbtable required";
		my $regid                = $args{regid}               	or die "regid required";

    my ($sth, $CandidateProfile_ID, $CandidateElection_ID, $Candidate_ID);

    # --------------------------------------------------------
    # CandidateProfile
    # --------------------------------------------------------
    # Checking that the Candidate is not in the Candidate Table.
       
    $sth = $dbh->prepare("SELECT Candidate_ID FROM Candidate WHERE Candidate_PetitionNameset = ?");
    $sth->execute($api_name);
    ($Candidate_ID) = $sth->fetchrow_array;
		
		if ( $Candidate_ID ) { 
			$sth = $dbh->prepare ("SELECT CandidateProfile_ID FROM RepMyBlock.PublicProfile WHERE Candidate_ID = ?");
    	$sth->execute($Candidate_ID);
    	($CandidateProfile_ID) = $sth->fetchrow_array;

			if ( $CandidateProfile_ID ) {
				print "Candidate_ID: $Candidate_ID - CandidateProfileID: $CandidateProfile_ID\n";
				return $CandidateProfile_ID;		
	
			} else {
				print "Candidate_ID: $Candidate_ID - ";
				print "Need to add into CandidateProfile_ID\n";
				
				$sth = $dbh->prepare("INSERT INTO CandidateProfile SET CandidateProfile_PicVerif = 'no', " . 
															"CandidateProfile_PDFVerif = 'no', CandidateProfile_FirstName = ?, " . 
															"CandidateProfile_LastName = ?, CandidateProfile_Alias = ?, " . 
															"CandidateRegAuthority_ID = ?, CandidateProfile_RegID = ?, " .
															"CandidateProfile_PublishProfile = 'yes', CandidateProfile_Complain = 'no', " . 
															"CandidateProfile_LastModified = NOW()");
				$sth->execute($FirstName, $LastName, $api_name, '1', $regid);
				$CandidateElection_ID = $dbh->{mysql_insertid};
				
				$sth = $dbh->prepare("SELECT PublicProfile_ID FROM PublicProfile WHERE CandidateProfile_ID = ? AND Candidate_ID = ?");
				$sth->execute($CandidateElection_ID, $Candidate_ID);
				my ($PublicProfile_ID) = $sth->fetchrow_array;
				
			
				
				if ( ! $PublicProfile_ID ) { 
					print "INSERTING Public Profile\n";
					$sth = $dbh->prepare("INSERT PublicProfile SET CandidateProfile_ID = ?, Candidate_ID = ?, " . 
																"PublicProfile_PublishProfile = 'yes', CandidateProfile_LastModified = NOW()");
					$sth->execute($CandidateElection_ID, $Candidate_ID);
				} else {
					print "Public Profile: ID " . $PublicProfile_ID . "\n";
					$sth = $dbh->prepare("UPDATE PublicProfile SET CandidateProfile_ID = ?, Candidate_ID = ?, " . 
																"PublicProfile_PublishProfile = 'yes', CandidateProfile_LastModified = NOW() " . 
																"WHERE PublicProfile_ID = ?");
					$sth->execute($CandidateElection_ID, $Candidate_ID, $PublicProfile_ID);											
				}
				
			
			}
		} else {
			
				
			# If it doesn't exist, see if you can copy it.
			
			print "Need to find in the Candidate\n";		
				
			$sth = $dbh->prepare("SELECT * FROM RepMyBlock.CandidateElection WHERE " . 
														"CandidateElection_DBTable = ? AND CandidateElection_DBTableValue = ? " .
														" AND ElectionsPosition_ID = ? ORDER BY Elections_ID DESC");
														
			print "SELECT * FROM RepMyBlock.CandidateElection WHERE " . 
														"CandidateElection_DBTable = '" . $district_text . 
														"' AND CandidateElection_DBTableValue = " . $district .
														" AND ElectionsPosition_ID = " . $ElectionsPositionID . " ORDER BY Elections_ID ASC\n";
														
			$sth->execute($district_text, $district, $ElectionsPositionID);
			my %ElectionDates;
			#$ElectionDates = $sth->fetchrow_hashref;
  	  my $ElectionDates = $sth->fetchrow_hashref;
			
			print "ElectionID : $Election_ID = " . $ElectionDates->{"Elections_ID"} . "\n";
			
			
			if ( $ElectionDates->{"Elections_ID"} ne $Election_ID) {
				$sth = $dbh->prepare("INSERT INTO CandidateElection SET Elections_ID = ?, ElectionsPosition_ID = ?, " . 
															"CandidateElection_PositionType = ?, CandidateElection_Text = ?, " . 
															"CandidateElection_PetitionText = ?, CandidateElection_Number = ?, " . 
															"CandidateElection_Display = ?, CandidateElection_DBTable = ?, " . 
															"CandidateElection_DBTableValue = ?");
				$sth->execute($Election_ID, $ElectionDates->{"ElectionsPosition_ID"}, $ElectionDates->{"CandidateElection_PositionType"}, 
											$ElectionDates->{"CandidateElection_Text"}, $ElectionDates->{"CandidateElection_PetitionText"}, 
											$ElectionDates->{"CandidateElection_Number"}, $ElectionDates->{"CandidateElection_Display"}, 
											$ElectionDates->{"CandidateElection_DBTable"}, $ElectionDates->{"CandidateElection_DBTableValue"});
											
				$CandidateElection_ID = $dbh->{mysql_insertid};
			} else {
				$CandidateElection_ID = $ElectionDates->{"CandidateElection_ID"};
			}				
			
		
			
		}
		
		print "Adding the candidate.\n";
							
		$sth = $dbh->prepare ("INSERT INTO Candidate SET Candidate_Party = ?, Candidate_DispName = ?, " .
																	"Candidate_PetitionNameset = ?, CandidateElection_DBTable = ?, " .
																	"CandidateElection_DBTableValue = ?, CandidateElection_ID = ?, " .
																	"Candidate_Status = 'pending'");
		$sth->execute($party, $FirstName . " " . $LastName, $api_name, $district_text, $district, $CandidateElection_ID);
		$Candidate_ID = $dbh->{mysql_insertid};
		
		
		
		
		print "Adding the candidateprofile\n";
		$sth = $dbh->prepare("INSERT INTO CandidateProfile SET CandidateProfile_PicVerif = 'no', " . 
															"CandidateProfile_PDFVerif = 'no', CandidateProfile_FirstName = ?, " . 
															"CandidateProfile_LastName = ?, CandidateProfile_Alias = ?, " . 
															"CandidateRegAuthority_ID = ?, CandidateProfile_RegID = ?, " .
															"CandidateProfile_PublishProfile = 'yes', CandidateProfile_Complain = 'no', " . 
															"CandidateProfile_LastModified = NOW()");
		$sth->execute($FirstName, $LastName, $api_name, '1', $regid);
		$CandidateElection_ID = $dbh->{mysql_insertid};
		
		$sth = $dbh->prepare("SELECT PublicProfile_ID FROM PublicProfile WHERE CandidateProfile_ID = ? AND Candidate_ID = ?");
		$sth->execute($CandidateElection_ID, $Candidate_ID);
		my ($PublicProfile_ID) = $sth->fetchrow_array;
		
		
		
		if ( ! $PublicProfile_ID ) { 
			print "INSERTING Public Profile\n";
			$sth = $dbh->prepare("INSERT PublicProfile SET CandidateProfile_ID = ?, Candidate_ID = ?, " . 
														"PublicProfile_PublishProfile = 'yes', CandidateProfile_LastModified = NOW()");
			$sth->execute($CandidateElection_ID, $Candidate_ID);
		} else {
			print "Public Profile: ID " . $PublicProfile_ID . "\n";
			$sth = $dbh->prepare("UPDATE PublicProfile SET CandidateProfile_ID = ?, Candidate_ID = ?, " . 
														"PublicProfile_PublishProfile = 'yes', CandidateProfile_LastModified = NOW() " . 
														"WHERE PublicProfile_ID = ?");
			$sth->execute($CandidateElection_ID, $Candidate_ID, $PublicProfile_ID);											
		}
		

		
		
}

1;
