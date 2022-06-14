#!/usr/bin/perl

use Cwd 'getcwd';

BEGIN {
	my $module_path = getcwd();
	unshift @INC, $module_path . "/Modules";
	unshift @INC, $module_path;
}

# Change the Package name each time below.
package C220302_FinishYourRegistration;

use strict;
use warnings;
no warnings 'uninitialized';

use SendEmail;
use RemoteDB;
use Frame;
use POSIX qw(strftime);
use HTML::FormatText::WithLinks;

my $TodayDate = strftime( '%A, %B %e, %Y', localtime );	

use Data::Dumper;

my $ConnectToLocalDB;

my $PackagePath = "/home/theo/RepMyBlock/SendEmail";
my $Database = "NoInUserRightNow";
my $TableToQuery = "SELECT * FROM RepMyBlock.SystemUserTemporary WHERE SystemUser_ID is null AND SystemUserTemporary_createtime < DATE_SUB(NOW(), INTERVAL 24 HOUR) AND SystemUserTemporary_createtime > DATE_SUB(NOW(), INTERVAL 96 HOUR)";

my %CacheReturnRaw = ();
my $TotalEmails = 0;

our %SystemUserEmail;
our $IgnoreSentEmail = 1;

sub ReturnIgnoreFlag {
	return 1;
}

sub AttachedFiles {
	my $self = shift;
	my @AttachedFiles = $_[0];
	
	@AttachedFiles = ();
	#$AttachedFiles[0] = $PackagePath . "/attachments/Ft-Washington-Armory-Li_20210603131652.pdf";
	#$AttachedFiles[1] = $PackagePath . "/attachments/ResolutionforGreaterTransparencyinPLPContracts.pdf";
	#$AttachedFiles[2] = $PackagePath . "/attachments/AfroSocDSAResolution.pdf";
	return @AttachedFiles;
}

sub ExecuteQuery {
	my $self = shift;
	$TotalEmails = $ConnectToLocalDB->ExecuteQuery($TableToQuery, \%CacheReturnRaw);

	return $TotalEmails;
}
sub PrepareEmail {
	my $class = shift; 
  my $self = {}; 
  
  my $Frame = Frame->new();

	${$_[1]}{'TestTo'} = "theo.chino\@gmail.com";	
	${$_[1]}{'Subject'} = "Please complete your Rep My Block registration.";

	${$_[1]}{'To'} = $CacheReturnRaw{$_[0]}{'SystemUserTemporary_email'};
	${$_[1]}{'RawEmail'} = $CacheReturnRaw{$_[0]}{'SystemUserTemporary_email'};
	$Frame::EmailToSendForFooter = $CacheReturnRaw{$_[0]}{'SystemUserTemporary_email'};
	#${$_[1]}{'From'} = "\"Rep My Block\" <repmyblocknyc\@gmail.com>";	
	${$_[1]}{'From'} = "\"Rep My Block\" <repmyblocknyc\@gmail.com>";	
	
	my $uid = "TmpID" . $CacheReturnRaw{$_[0]}{'SystemUserTemporary_ID'};
	
	${$_[1]}{'BodyHTML'} = 	${$_[1]}{'BodyHTML'} = Frame::TopEmail() . ReturnEmailHTML($CacheReturnRaw{$_[0]}{'SystemUserTemporary_username'}, $uid) . Frame::BottomEmail(${$_[1]}{'SystemUser_email'});
	${$_[1]}{'BodyText'} = ReturnEmailText(ReturnEmailHTML($CacheReturnRaw{$_[0]}{'SystemUserTemporary_username'}. $uid));
}

sub ReturnEmailHTML {
	
	my $username = $_[0];
	my $tmpuid = $_[1];

	return <<EOF;

<P>
	Hello,
</P>

</P>
	You registered on the RepMyBlock website, but you did not complete the registration.
</P>

<P>
	All you need to do is log into RepMyBlock with your username <I>"<B>$username</B>"</I> and the password 
	you selected and update your Firstname and Lastname on the Profile menu. 
	<A HREF="https://www.repmyblock.org/${tmpuid}/exp/login/login">Click here to access the login page</A>.
</P>

<P>
	Since you did not finish the registration process, <B>you cannot use the change 
	password link</B> to change your password. If you forgot your password, reply to this 
	message, and we'll reset the password manually.
</P>

<P>
 Regards,
</P>

<P>
	<B>Theo Chino</B><BR>
	Rep My Block co-founder<BR>
	(929) 359-3349 
</P>
EOF
}

sub ReturnEmailText {
	my $f2 = HTML::FormatText::WithLinks->new(
    before_link => '',
    after_link => ' [%l]',
    footnote => '');
	
	return $f2->parse($_[0]);
}

### This is the standart to get the ball rolling into the database.

sub InitDatabase {
	my $self = shift;
	my $dbh = shift;

	$ConnectToLocalDB->InitDatabase(".db_rmb_NYSVoters");
	
}

sub new {
	my $class = shift; 
  my $self = {}; 
  $ConnectToLocalDB	= SendEmail->new($Database);     
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