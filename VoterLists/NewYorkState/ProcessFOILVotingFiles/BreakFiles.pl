#!/usr/bin/perl

# Read the Table Directory in the file
my $filename = '/home/usracct/.voter_file';
open(my $fh, '<:encoding(UTF-8)', $filename) or die "Could not open file '$filename' $!";
my $tabledate = <$fh>;
chomp($tabledate);
close($fh);

$sizetobreak = 100000;
$sizetobreak = 25000;  # For 1 GB of Ram Virtual Machine, this is a good amount.

$filename = "/home/usracct/RawVoterFiles/AllNYSVoters_" . $tabledate . ".txt";

open(my $fh, $filename ) or die "cannot open input.txt: $!";

$FileCounter = 0;
$Counter = 0;

$PATH = "/home/usracct/WorkingFiles/";

$FILENAME =  $PATH . "CVSVOTER_" . $FileCounter . ".csv";
open ( $out, '>', $FILENAME);


while (my $row = <$fh>) {
  chomp $row;

	if ( $Counter > $sizetobreak ) {
		$FileCounter++;
		$Counter = 0;
		
		close ($out);
		$FILENAME =  $PATH . "CVSVOTER_" . $FileCounter . ".csv";
		open ( $out, '>', $FILENAME);
	}

  print $out "$row\n";
  $Counter++;
}

close ($out);
print "done\n";
