#!/usr/bin/perl

use Cwd 'getcwd';

BEGIN {
	my $module_path = getcwd();
	unshift @INC, $module_path . "/Modules";
	unshift @INC, $module_path;
}

# Change the Package name each time below.
package C210929_BetaReport;

use strict;
use warnings;
no warnings 'uninitialized';

use SendEmail;
use RemoteDB;
use Frame;
use POSIX qw(strftime);
my $TodayDate = strftime( '%A, %B %e, %Y', localtime );	

use Data::Dumper;

my $ConnectToDatabase;
my $ConnectToSocDemProdDB;

my $Database = "NoInUserRightNow";
my $TableToQuery = "SELECT * FROM SocDemsAmerica WHERE SocDemsAmerica_Contact = 'yes'";
my $MailToPeople = "SELECT * FROM MailReportTo";
my $SocDemsProdQuery = "SELECT * FROM MemberPledge WHERE MemberPledge_RecordCreated > DATE_SUB(CURDATE(), INTERVAL 1 DAY);";
my $SocDemsProdQueryTotal = "SELECT sum(MemberPledge_Amount) as TotalAmount, count(MemberPledge_FirstName) as TotalMembers FROM MemberPledge";

my %CacheReturnRaw = ();
my %SocDemsMembers = ();
my %SocDemsTotals = ();
my $TotalEmails = 0;

our %SystemUserEmail;
our $IgnoreSentEmail = 1;

sub ReturnIgnoreFlag {
	return 1;
}

sub AttachedFiles {
	my @AttachedFiles;
	#$AttachedFiles[0] = $PackagePath . "/attachments/Ft-Washington-Armory-Li_20210603131652.pdf";
	#$AttachedFiles[1] = $PackagePath . "/attachments/ResolutionforGreaterTransparencyinPLPContracts.pdf";
	#$AttachedFiles[2] = $PackagePath . "/attachments/AfroSocDSAResolution.pdf";
	return @AttachedFiles;
}

sub ExecuteQuery {
	my $self = shift;
	$TotalEmails = $ConnectToDatabase->ExecuteQuery($TableToQuery, \%CacheReturnRaw);
	#$ConnectToSocDemProdDB->ExecuteQuery($SocDemsProdQuery, \%SocDemsMembers);
	#$ConnectToSocDemProdDB->ExecuteQuery($SocDemsProdQueryTotal, \%SocDemsTotals);
	return $TotalEmails;
}

sub PrepareEmail {
	my $class = shift; 
  my $self = {}; 
  
  my $Frame = Frame->new();
	
	${$_[1]}{'Subject'} = "[RepMyBlock Users] Beta Report for $TodayDate";
	${$_[1]}{'To'} = "\"" . $CacheReturnRaw{$_[0]}{'SocDemsAmerica_FirstName'} . " " . $CacheReturnRaw{$_[0]}{'SocDemsAmerica_LastName'} . "\" <" . $CacheReturnRaw{$_[0]}{'SocDemsAmerica_Email'} . ">";
	${$_[1]}{'RawEmail'} =  $CacheReturnRaw{$_[0]}{'SocDemsAmerica_Email'};
	$Frame::EmailToSendForFooter = $CacheReturnRaw{$_[0]}{'SocDemsAmerica_FirstName'} . " " . $CacheReturnRaw{$_[0]}{'SocDemsAmerica_LastName'} . " at" . $CacheReturnRaw{$_[0]}{'SocDemsAmerica_Email'};
	${$_[1]}{'From'} = "\"Automated Reporter\" <no-reply\@repmyblock.org>";	
	${$_[1]}{'BodyText'} = ReturnEmailText();
	${$_[1]}{'BodyHTML'} = Frame::TopEmail() . ReturnEmailHTML() . Frame::BottomEmail();
	
}

sub ReturnEmailHTML {
	my $TotalBigMoney = $SocDemsTotals{'0'}{'TotalAmount'};
	my $text =<<EOF;
	
<P><H2>Beta Stats for $TodayDate</H2></P>

<P>
	<B>Total testers:</B> $SocDemsTotals{'0'}{'TotalMembers'}<BR>
  <B>Total new bugs found:</B> $TotalBigMoney<BR>
</P>

<P><H2>New pledges</H2></P>

<P>
<TABLE style="max-width: 700px;" border="1" width="100%" cellspacing="1" cellpadding="1">

	<TR style="1px solid black;">
		<TH style="1px solid black;">First&nbsp;Name</TD>
		<TH style="1px solid black;">Last&nbsp;Name</TD>
		<TH style="1px solid black;">Zip</TD>
		<TH style="1px solid black;">Signup</TD>
	</TR>
	
	<TR style="1px solid black;">
		<TH style="1px solid black;">Amount</TD>
		<TH style="1px solid black;">Referral</TD>
		<TH COLSPAN=2 style="1px solid black;">Email</TD>
	</TR>
EOF

	if ( keys %SocDemsMembers > 0 ) {

		use Date::Manip;
		use DateTime;
		use DateTime::Format::DateManip;
	
		#$text .= "<PRE>" . Dumper(%SocDemsMembers) . "</PRE>";
		foreach my $var (sort keys %SocDemsMembers) {
			my $outputtime = "";
			my $date = ParseDate($SocDemsMembers{$var}{'MemberPledge_RecordCreated'});
			$date = DateTime::Format::DateManip->parse_datetime($date);
			eval{ $outputtime = $date->strftime("%H:%M:%S <I>(%m/%d)</I>"); };
	  
	    $text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\">" . $SocDemsMembers{$var}{'MemberPledge_FirstName'} . "</TD>" .
	    	"<TD style=\"1px solid black;\">" . $SocDemsMembers{$var}{'MemberPledge_LastName'} . "</TD>" .
	    	"<TD style=\"1px solid black;\" ALIGN=CENTER>" . $SocDemsMembers{$var}{'MemberPledge_Zipcode'} . "</TD>" .
	    	"<TD style=\"1px solid black;\" ALIGN=RIGHT>" . $outputtime  . "</TD></TR>" .
	     	"<TR style=\"1px solid black;\"><TD style=\"1px solid black;\" ALIGN=RIGHT>" . big_money($SocDemsMembers{$var}{'MemberPledge_Amount'}) . "</TD>" .
	     	"<TD style=\"1px solid black;\">" . $SocDemsMembers{$var}{'MemberPledge_Refereral'} . "</TD>" .
	    	"<TD style=\"1px solid black;\" COLSPAN=2>" . $SocDemsMembers{$var}{'MemberPledge_Email'}. "</TD>" .
	    "</TR>\n";
		}
	} else {
		
		$text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\" COLSPAN=4 ALIGN=CENTER>No new members signed up on $TodayDate</TD></TR>\n";
	
	}
	
	
$text .=<<EOF;
</TABLE>
</P>




EOF
return $text;
}


sub ReturnEmailText {
	
	return <<EOF;
New Pledge on $TodayDate
EOF
}

### This is the standart to get the ball rolling into the database.

sub InitDatabase {
	my $self = shift;
	my $dbh = shift;
	$ConnectToDatabase->InitDatabase();
	#ConnectToSocDemProdDB->InitDatabase();
}



sub new {
	my $class = shift; 
  my $self = {}; 
  $ConnectToDatabase	= SendEmail->new($Database);   
  #$ConnectToSocDemProdDB	= RemoteDB->new($Database);   
  return bless $self, $class;
}


### Function to carry over.
#sub EncryptURL {
#	my $string = $_[0];

#	use Crypt::OpenSSL::RSA;
#	use MIME::Base64;
#	use URL::Encode;

#  my $public = Crypt::OpenSSL::RSA->new_public_key($PubKey);
#	my $string = "AAAAAUK" . URL::Encode::url_encode(encode_base64($public->encrypt($string), ""));
#	print "String: $string\n";
  
#  return $string;
#}


sub big_money {
  my $number = sprintf "%.2f", shift @_;
  # Add one comma each time through the do-nothing loop
  1 while $number =~ s/^(-?\d+)(\d\d\d)/$1,$2/;
  # Put the dollar sign in the right place
  $number =~ s/^(-?)/$1\$ /;
  return $number;
}
	
	


1;