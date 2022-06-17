#!/usr/bin/perl

use Cwd 'getcwd';

BEGIN {
	my $module_path = getcwd();
	unshift @INC, $module_path . "/Modules";
	unshift @INC, $module_path;
}

# Change the Package name each time below.
package C220614_WeekAdminReport;

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

my $ExtraValue = 1;   # This is the flag for the query.

# The AdminNotifA Need to be into the format of Priviledge. To Be fixed one day.
my $TableToQuery = "SELECT DISTINCT SystemUser.SystemUser_ID, Team.Team_ID, SystemUser_FirstName, SystemUser_LastName, SystemUser_email, " . 
										"Team_Name, Team_AccessCode, Team_WebCode, Team_EmailCode, Team_Public " .
										"FROM RepMyBlock.TeamMember " .
										"LEFT JOIN AdminNotif ON (TeamMember.Team_ID = AdminNotif.Team_ID AND TeamMember.SystemUser_ID = AdminNotif.SystemUser_ID) " .
										"LEFT JOIN Team ON (Team.Team_ID = TeamMember.Team_ID) " .
										"LEFT JOIN SystemUser ON (TeamMember.SystemUser_ID = SystemUser.SystemUser_ID) " . 
										"WHERE TeamMember_Active = 'yes' AND Team_Active = 'yes' AND Team.Team_ID IS NOT NULL AND " . 
										"SystemUser_emailverified = 'both' AND AdminNotif_PrivA = '8'";

my $TableAttempsQuery = "SELECT * FROM RepMyBlock.Team " .
												"LEFT JOIN SystemUserEmail ON (SystemUserEmail.SystemUserEmail_WebCode = Team.Team_WebCode) " .
												"WHERE Team_ID = ? AND SystemUserEmail_Received > DATE_SUB(NOW(), INTERVAL 1 WEEK)";

my $TableMembersQuery = "SELECT * FROM RepMyBlock.TeamMember " . 
												"LEFT JOIN AdminNotif ON (TeamMember.Team_ID = AdminNotif.Team_ID AND TeamMember.SystemUser_ID = AdminNotif.SystemUser_ID) " . 
												"LEFT JOIN Team ON (Team.Team_ID = TeamMember.Team_ID) " . 
												"LEFT JOIN SystemUser ON (TeamMember.SystemUser_ID = SystemUser.SystemUser_ID) " . 
												"WHERE Team_Active = 'yes' AND Team.Team_ID = ? AND SystemUser.SystemUser_ID > 1 " . 
												"ORDER BY SystemUser_lastlogintime DESC";
												
my %CacheReturnRaw = ();
my $TotalEmails = 0;


## Not being used yet.
my %RMBTotalQuery = ();
my %RMBTempTotalQuery = ();
my %RMBMembers = ();
my %RMBTempMembers= ();



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

	#$ConnectToRMBProdDB->ExecuteQuery($RMB_SystemQuery, \%RMBMembers);
	#$ConnectToRMBProdDB->ExecuteQuery($RMB_TotalQuery, \%RMBTotalQuery);
	#$ConnectToRMBProdDB->ExecuteQuery($RMB_SystemTempQuery, \%RMBTempMembers);
	#$ConnectToRMBProdDB->ExecuteQuery($RMB_TotalTempQuery, \%RMBTempTotalQuery);
	#$ConnectToSocDemProdDB->ExecuteQuery($SocDemsProdQueryTotal, \%SocDemsTotals);
	return $TotalEmails;
}

sub PrepareEmail {
	my $class = shift; 
  my $self = {}; 
  
  my $Frame = Frame->new();
	${$_[1]}{'TestTo'} = "theo.chino\@gmail.com";
	${$_[1]}{'Subject'} = "[RepMyBlock Users] New Users Report for the week ending $TodayDate";
	${$_[1]}{'To'} = "\"" . $CacheReturnRaw{$_[0]}{'SystemUser_FirstName'} . " " . $CacheReturnRaw{$_[0]}{'SystemUser_LastName'} . "\" <" . $CacheReturnRaw{$_[0]}{'SystemUser_email'} . ">";
	${$_[1]}{'RawEmail'} =  $CacheReturnRaw{$_[0]}{'SystemUser_email'};
	my $EmailToSendForFooter = $CacheReturnRaw{$_[0]}{'SystemUser_FirstName'} . " " . $CacheReturnRaw{$_[0]}{'SystemUser_LastName'} . " at " . $CacheReturnRaw{$_[0]}{'SystemUser_email'};
	${$_[1]}{'From'} = "\"Automated Reporter\" <no-reply\@repmyblock.org>";	
	${$_[1]}{'BodyText'} = ReturnEmailText();
	${$_[1]}{'BodyHTML'} = Frame::TopEmail() . ReturnEmailHTML($CacheReturnRaw{$_[0]}{'Team_ID'}, $CacheReturnRaw{$_[0]}{'Team_Name'}) . Frame::BottomEmail($EmailToSendForFooter);
}

sub ReturnEmailHTML {
	
	my %CacheReturnAttemps = ();
	my %CacheReturnMembers = ();

	my $TotalPeopleAttemps = $ConnectToDatabase->ExecuteQuery($TableAttempsQuery, \%CacheReturnAttemps, $_[0]);
	my $TotalMembers = $ConnectToDatabase->ExecuteQuery($TableMembersQuery, \%CacheReturnMembers, $_[0]);
		
	my $ActivePeople = 0;
	
	for(my $i = 0; $i < $TotalMembers; $i++) {
		$ActivePeople++ if ($CacheReturnMembers{$i}{'TeamMember_Active'} eq 'yes');
	}
	
	my $text =<<EOF;

<P><H1>$_[1]</H1></P>
	
<P><H2>Stats for the week ending $TodayDate</H2></P>

<P>
	<B>Total members:</B> $ActivePeople<BR>
  <B>New Temporary Signup:</B> $TotalPeopleAttemps<BR>
</P>

<P><H2>New Signup Request Received</H2></P>

<P>
<TABLE border="1" width="100%" cellspacing="1" cellpadding="1">

	<TR style="1px solid black;">
		<TH style="1px solid black;">Email from</TH>
		<TH style="1px solid black;">Tracking Number</TH>
		<TH style="1px solid black;">Received on</TH>
	</TR>
EOF

	if ( $TotalPeopleAttemps > 0 ) {
		use Date::Manip;
		use DateTime;
		use DateTime::Format::DateManip;
	
		#$text .= "<PRE>" . Dumper(%SocDemsMembers) . "</PRE>";

		for(my $i = 0; $i < $TotalPeopleAttemps; $i++) {
			my $outputtime = "";
				my $date = ParseDate($CacheReturnAttemps{$i}{'SystemUserEmail_Received'});
				$date = DateTime::Format::DateManip->parse_datetime($date);
				eval{ $outputtime = $date->strftime("%m/%d/%Y at %I:%M:%S %p"); };
					
	    $text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\">" . $CacheReturnAttemps{$i}{'SystemUserEmail_AddFrom'} . "</TD>" .
	    	"<TD style=\"1px solid black;\">" . $CacheReturnAttemps{$i}{'SystemUserEmail_MailCode'} . "</TD>" .
	    	"<TD style=\"1px solid black;\" ALIGN=RIGHT>" . $outputtime  . "</TD>" .
	    "</TR>\n";
			
		}
		
	} else {
		
		$text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\" COLSPAN=4 ALIGN=CENTER>No new signup attemps this week</TD></TR>\n";
	
	}
	
	
$text .=<<EOF;
</TABLE>
</P>

<P><H2>Active Members</H2></P>

<P>
<TABLE border="1" width="100%" cellspacing="1" cellpadding="1">
<TR style="1px solid black;"><TH style="1px solid black;">Name</TH><TH style="1px solid black;">Email</TH><TH style="1px solid black;">Last Login</TH></TR>
EOF

	if ( $TotalMembers > 0 ) {

		use Date::Manip;
		use DateTime;
		use DateTime::Format::DateManip;
	
		#$text .= "<PRE>" . Dumper(%SocDemsMembers) . "</PRE>";
		
		for(my $i = 0; $i < $TotalMembers; $i++) {
			if ($CacheReturnMembers{$i}{'TeamMember_Active'} eq 'yes') {
				my $outputtime = "";
					my $date = ParseDate($CacheReturnMembers{$i}{'SystemUser_lastlogintime'});
					$date = DateTime::Format::DateManip->parse_datetime($date);
					eval{ $outputtime = $date->strftime("%m/%d/%Y <I>(%I:%M %p)</I>"); };
						
		    $text .= "<TR style=\"1px solid black;\">" . 
		    	"<TD style=\"1px solid black;\">" . $CacheReturnMembers{$i}{'SystemUser_FirstName'} . " " .  $CacheReturnMembers{$i}{'SystemUser_LastName'} . "</TD>" .
		    	"<TD style=\"1px solid black;\">" . $CacheReturnMembers{$i}{'SystemUser_email'} . "</TD>" .
		    	"<TD style=\"1px solid black;\" ALIGN=RIGHT>" . $outputtime  . "</TD>" .
		    "</TR>\n";
			}
		}
	} else {
		
		$text .= "<TR style=\"1px solid black;\">" . 
	    	"<TD style=\"1px solid black;\" COLSPAN=4 ALIGN=CENTER>No active members</TD></TR>\n";
	
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
	$ConnectToRMBProdDB->InitDatabase(".db_mysqldb02_RepMyBlock");
}



sub new {
	my $class = shift; 
  my $self = {}; 
  $ConnectToDatabase	= SendEmail->new();   
  $ConnectToRMBProdDB	= RemoteDB->new();   
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