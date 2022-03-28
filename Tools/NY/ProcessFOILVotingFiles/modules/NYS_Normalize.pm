package NYS_Normalize;
# NYS_Normalize.pm
 
use strict;
use warnings;

sub CheckFieldReturn() {
	my $FieldContent = $_[0];
	my $FieldVariable = $_[1];
	my $DataBaseName = $_[2];
	
	print "Field Variable:  $FieldContent -> " . $FieldVariable -> { $FieldContent } . "\n";
	
	if ( ! $FieldVariable -> { $FieldContent } && ! $FieldContent =~ /^\s*$/ ) {
		my $sql = "";
		my $CompressFieldName = $FieldContent;
		$CompressFieldName =~ tr/a-zA-Z//dc;
		
		if ( $DataBaseName eq "VotersLastName" || $DataBaseName eq "VotersMiddleName" || $DataBaseName eq "VotersFirstName") {
			$sql = "INSERT INTO " . $DataBaseName . " SET " . $DataBaseName . "_Text = " . $dbh->quote($FieldContent) . 	", " .
																	$DataBaseName . "_Compress = " . $dbh->quote($CompressFieldName);
		} elsif ($DataBaseName eq "DataStreet" || $DataBaseName eq "DataCity" ) {
			$sql = "INSERT INTO " . $DataBaseName . " SET " . $DataBaseName . "_Name = " . $dbh->quote($FieldContent);
		}
		
		$sth_query = $dbh->prepare($sql);
		$sth_query->execute();
		$FieldVariable -> { $FieldContent } = $sth_query->{'mysql_insertid'};	
		print "Inserting\n";
		exit();
	}
	
	return $FieldVariable -> { $FieldContent };
}

 
1;
