#!/usr/bin/perl

use Cwd 'getcwd';

BEGIN {
	my $module_path = getcwd();
	unshift @INC, $module_path . "/Modules";
	unshift @INC, $module_path;
}

# Change the Package name each time below.
package C210830_Prospect;

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
my $ConnectToRMBProdDB;

my $Database = "NoInUserRightNow";
my $ExtraValue = 1;
my $TableToQuery = "SELECT * FROM AdminNotif LEFT JOIN SystemUser ON (SystemUser.SystemUser_ID = AdminNotif.SystemUser_ID) WHERE (AdminNotif_PrivA & ?) > 0";
#my $MailToPeople = "SELECT * FROM MailReportTo";
my $RMB_SystemQuery = "SELECT * FROM SystemUser WHERE SystemUser_createtime > DATE_SUB(CURDATE(), INTERVAL 1 DAY);";
my $RMB_TotalQuery = "SELECT COUNT(*) AS TotalMembers FROM SystemUser";

my $RMB_SystemTempQuery = "SELECT * FROM SystemUserTemporary WHERE SystemUserTemporary_createtime > DATE_SUB(CURDATE(), INTERVAL 1 DAY);";
my $RMB_TotalTempQuery = "SELECT COUNT(*) AS TotalMembers FROM SystemUserTemporary";

my $SocDemsProdQueryTotal = "SELECT sum(MemberPledge_Amount) as TotalAmount, count(MemberPledge_FirstName) as TotalMembers FROM MemberPledge";

my %CacheReturnRaw = ();
my %RMBMembers = ();
my %RMBTempMembers = ();
my %RMBTotalQuery = ();
my %RMBTempTotalQuery = ();
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
	$TotalEmails = $ConnectToDatabase->ExecuteQuery($TableToQuery, \%CacheReturnRaw, $ExtraValue);
	$ConnectToRMBProdDB->ExecuteQuery($RMB_SystemQuery, \%RMBMembers);
	$ConnectToRMBProdDB->ExecuteQuery($RMB_TotalQuery, \%RMBTotalQuery);
	$ConnectToRMBProdDB->ExecuteQuery($RMB_SystemTempQuery, \%RMBTempMembers);
	$ConnectToRMBProdDB->ExecuteQuery($RMB_TotalTempQuery, \%RMBTempTotalQuery);
	#$ConnectToSocDemProdDB->ExecuteQuery($SocDemsProdQueryTotal, \%SocDemsTotals);
	return $TotalEmails;
}

sub PrepareEmail {
	my $class = shift; 
  my $self = {}; 
  
  my $Frame = Frame->new();
	
	${$_[1]}{'Subject'} = "[RepMyBlock Users] New Users Report for $TodayDate";
	${$_[1]}{'To'} = "\"" . $CacheReturnRaw{$_[0]}{'SystemUser_FirstName'} . " " . $CacheReturnRaw{$_[0]}{'SystemUser_LastName'} . "\" <" . $CacheReturnRaw{$_[0]}{'SystemUser_email'} . ">";
	${$_[1]}{'RawEmail'} =  $CacheReturnRaw{$_[0]}{'SystemUser_email'};
	my $EmailToSendForFooter = $CacheReturnRaw{$_[0]}{'SystemUser_FirstName'} . " " . $CacheReturnRaw{$_[0]}{'SystemUser_LastName'} . " at" . $CacheReturnRaw{$_[0]}{'SystemUser_email'};
	${$_[1]}{'From'} = "\"Automated Reporter\" <no-reply\@repmyblock.org>";	
	${$_[1]}{'BodyText'} = ReturnEmailText();
	${$_[1]}{'BodyHTML'} = Frame::TopEmail() . ReturnEmailHTML() . Frame::BottomEmail($EmailToSendForFooter);
}

sub ReturnEmailHTML {
	my $TotalBigMoney = big_money($RMBMembers{'0'}{'TotalAmount'});
	my $text =<<EOF;
	
<P><H2>Stats for $TodayDate</H2></P>

<P>
	<B>Total members:</B> $RMBTotalQuery{'0'}{'TotalMembers'}<BR>
  <B>New Temporary Signup:</B> $RMBTempTotalQuery{'0'}{'TotalMembers'}<BR>
</P>

<P><H2>New Unfinished Signups</H2></P>

<P>
<TABLE style="max-width: 700px;" border="1" width="100%" cellspacing="1" cellpadding="1">

	<TR style="1px solid black;">
		<TH style="1px solid black;">Username</TD>
		<TH style="1px solid black;">IP</TD>
		<TH style="1px solid black;">Signup</TD>
	</TR>
EOF

	if ( keys %RMBMembers > 0 ) {

		use Date::Manip;
		use DateTime;
		use DateTime::Format::DateManip;
	
		#$text .= "<PRE>" . Dumper(%SocDemsMembers) . "</PRE>";
		foreach my $var (sort keys %RMBMembers) {
			my $outputtime = "";
			my $date = ParseDate($RMBTempMembers{$var}{'SystemTemporaryUser_createtime'});
			$date = DateTime::Format::DateManip->parse_datetime($date);
			eval{ $outputtime = $date->strftime("%H:%M:%S <I>(%m/%d)</I>"); };
	  
	    $text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\">" . $RMBTempMembers{$var}{'SystemTemporaryUser_username'} . "</TD>" .
	    	"<TD style=\"1px solid black;\">" . $RMBTempMembers{$var}{'SystemTemporaryUser_IP'} . "</TD>" .
	    	"<TD style=\"1px solid black;\" ALIGN=RIGHT>" . $outputtime  . "</TD></TR>" .
	    "</TR>\n";
		}
	} else {
		
		$text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\" COLSPAN=4 ALIGN=CENTER>No new temporary members signed up on $TodayDate</TD></TR>\n";
	
	}
	
	
$text .=<<EOF;
</TABLE>
</P>

<P><H2>New Transformations</H2></P>

<P>
<TABLE style="max-width: 700px;" border="1" width="100%" cellspacing="1" cellpadding="1">

	<TR style="1px solid black;">
		<TH style="1px solid black;">First&nbsp;Name</TD>
		<TH style="1px solid black;">Last&nbsp;Name</TD>
		<TH style="1px solid black;">AD/ED</TD>
		<TH style="1px solid black;">Signup</TD>
	</TR>
EOF

	if ( keys %RMBMembers > 0 ) {

		use Date::Manip;
		use DateTime;
		use DateTime::Format::DateManip;
	
		#$text .= "<PRE>" . Dumper(%SocDemsMembers) . "</PRE>";
		foreach my $var (sort keys %RMBMembers) {
			my $outputtime = "";
			my $date = ParseDate($RMBMembers{$var}{'SystemUser_createtime'});
			$date = DateTime::Format::DateManip->parse_datetime($date);
			eval{ $outputtime = $date->strftime("%H:%M:%S <I>(%m/%d)</I>"); };
	  
	    $text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\">" . $RMBMembers{$var}{'SystemUser_FirstName'} . "</TD>" .
	    	"<TD style=\"1px solid black;\">" . $RMBMembers{$var}{'SystemUser_LastName'} . "</TD>" .
	    	"<TD style=\"1px solid black;\" ALIGN=CENTER>" . $RMBMembers{$var}{'SystemUser_EDAD'} . "</TD>" .
	    	"<TD style=\"1px solid black;\" ALIGN=RIGHT>" . $outputtime  . "</TD></TR>" .
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
	
	$ConnectToDatabase->InitDatabase(".db_mysqldb02_RepMyBlock");
	$ConnectToRMBProdDB->InitDatabase();
}



sub new {
	my $class = shift; 
  my $self = {}; 
  $ConnectToDatabase	= SendEmail->new($Database);   
  $ConnectToRMBProdDB	= RemoteDB->new($Database);   
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
	my $number = $_[0];
	#  my $number = sprintf "%.2f", shift @_;
  # Add one comma each time through the do-nothing loop
  #1 while $number =~ s/^(-?\d+)(\d\d\d)/$1,$2/;
  # Put the dollar sign in the right place
  #$number =~ s/^(-?)/$1\$ /;
  return $number;
}
	
	


1;