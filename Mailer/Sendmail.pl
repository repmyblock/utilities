#!/usr/bin/perl

use Cwd 'getcwd';


BEGIN {
	my $module_path = getcwd();
	unshift @INC, $ENV{'HOME'} . "/SendEmail/Modules";
	unshift @INC, $module_path . "/Modules";
	unshift @INC, $module_path;
}

use Data::Dumper;
use POSIX qw(strftime);
use Email::MIME;
use Email::MIME::CreateHTML;
use MIME::Types;
use Email::Send;
use Email::Abstract;
use ServermailDB;
use File::Slurp;
#use Time::HiRes qw ( clock );

my $ModuleToSend;
my $EmailToSendForTest  = "theo\@repmyblock.nyc";
my $Flag_Test = 0;

if ($ARGV[0] eq 'customemail') {
	$Flag_Test = 2;
	$ModuleToSend = $ARGV[1];
	$EmailToSendForTest = $ARGV[2];
} elsif ($ARGV[0] eq 'sendprod') {
	$Flag_Test = 1;
	$ModuleToSend = $ARGV[1];
} else {
	$ModuleToSend = $ARGV[0];
}


# Test = 0 is the test mode. -1 doesn't send email; Sending the mail is =1
my $Flag_SendEmail = 1;
my $Flag_PvtKey = 1;
my $Flag_Verbose = 1;
my $CounterTest = 1;
my $SentEmailCounter =1;
our %SystemUserEmail = ();
my $Subject = "";
my @AttachedFiles = ();
my %SentEmailTable = ();
my $Flag_IgnoreSentEmail = 0;

print "ModuleToSend: $ModuleToSend\n";
#use strict;
#use warning;

my $module_path = getcwd();
my ($name_module) = ($ModuleToSend =~ m/(.*)\.pm/);
my $module = $module_path . "/" . $name_module . ".pm";

unless (-e $module) {
	print "The module $module doesn't exist!\n";
	exit();
}

# Calculate today's date
my $TodayDate = strftime( '%A, %B %e, %Y', localtime );
my $TimeStamp = time();

print "Starting sending mail module $name_module\n";
use Module::Load;
load $name_module;
autoload $name_module;

my $ServerMailDB = ServermailDB->new();
my $ZentModule_ID = 0;

$ZentModule_ID = $ServerMailDB->CheckModule($name_module);
if ( $Flag_IgnoreSentEmail == 0 ) {
	$ServerMailDB->LoadSentEmail($ZentModule_ID, \%SentEmailTable);
}

my $SendEmail = $name_module->new();
$Flag_IgnoreSentEmail = $SendEmail->ReturnIgnoreFlag;
my @attach = $SendEmail->AttachedFiles(\@attach);

$SendEmail->InitDatabase();
my $TotalEmails = $SendEmail->ExecuteQuery();
#my @AttachedFiles = $SendEmail->AttachedFiles();

print "Today's date: $TodayDate - $TimeStamp\n";
print "Total Emails adresses in the database: $TotalEmails\n";
print "Ignore Email Sent Flag: " . $Flag_IgnoreSentEmail . "\n";

for ( my $i = 0; $i < $TotalEmails; $i++) {
	$SendEmail->PrepareEmail($i, \%SystemUserEmail);
	print "Sending Email To: " . $SystemUserEmail{'RawEmail'} . "\n"; 
	
	#print "SentEmailTable: " . $SentEmailTable {$SystemUserEmail{'RawEmail'}} . " - ";
	#print "Flag IgnoreSentEmail = " . $Flag_IgnoreSentEmail . " - Test = $Flag_Test\n";
	
	
	if ( $SentEmailTable {$SystemUserEmail{'RawEmail'}} == 1 && $Flag_IgnoreSentEmail != 1 && $Flag_Test == 1) {
		print "Email already sent to: " . $SystemUserEmail{'RawEmail'} . "\n";	
		
	}	elsif (! defined $SystemUserEmail{'RawEmail'}) {
		print "Email index $i not defined with a valid email\n";
	
	} else {
		
	
		if ( $Flag_Test == 2) {
			print "Test TimeStamp Flag 2: " . $TimeStamp . "\n";
			$SystemUserEmail{'To'} = $EmailToSendForTest;
			$Subject = $TimeStamp . " - Custom - " . $SystemUserEmail{'Subject'};
		
		
		} elsif ( $Flag_Test == 0 ) {
			print "Test TimeStamp Flag 0:" . $TimeStamp . "\n";
			$SystemUserEmail{'To'} = $SystemUserEmail{'TestTo'};
			$Subject = $TimeStamp . " - " . $SystemUserEmail{'Subject'};
			
		} else {
			$Subject = $SystemUserEmail{'Subject'};
			$ServerMailDB->InsertSentEmail($SystemUserEmail{'RawEmail'}, $ZentModule_ID, \%SentEmailTable);
		}

		print "Sending to: " . $SystemUserEmail{'To'} . "\n";
		if (undef $SystemUserEmail{'To'}) {
			print "No recipients ... exiting\n";
			exit();
		}
	
		my $htmlmail = Email::MIME->create_html(
		    header => [],
		    body => $SystemUserEmail{'BodyHTML'},
		    body_attributes => {
		    disposition => 'inline' },
		    text_body => $SystemUserEmail{'BodyText'});
		
		# ----- Create base message
		my $email = Email::MIME->create(
		    header => [
		        From => $SystemUserEmail{'From'},
		        To => $SystemUserEmail{'To'},
		        Bcc => $SystemUserEmail{'BCC'},
		        Reply-To => $SystemUserEmail{'Reply-To'},
		        Subject => $Subject,
				],
		    attributes => { content_type => "multipart/mixed" },
		    parts => [$htmlmail]
		);
		
		
		
		# ----- Add attachments
		
		print "\nNumber of attached files: " . @attach . "\n";		
		if ( @attach > 0 && length($attach[0]) > 0) {
			my $Content_ID = strftime('%Y%M%d', localtime);
			$Content_ID .= "_" . rand(100);
			$Content_ID =~ s/\.//;
			print "\nAttachment Content_ID: $Content_ID\n";
			
			for (my $l=0; $l < @attach ; $l++) {
				$body[$l] = read_file($attach[$l]);			
				next unless $attach[$l];
				($mimetype,$encoding) = MIME::Types::by_suffix($attach[$l]);
				
				print "Attachment $l Mimetype: $mimetype - ";
				print "Encoding: $encoding\n";
				my ($Filename) = ($attach[$l]) =~ /([^\/]+)$/;
				my ($FilenameWO) = ($Filename) =~ /([^\.]*)\.*/;
				
				
				$att = Email::MIME->create(
					attributes => {
						content_type => $mimetype,
						filename => $Filename,
						encoding => $encoding,
						name => $FilenameWO,
						disposition => "attachment"
					},
					header => [ 'Content-ID' => "${Content_ID}_$l" ],
					body => $body[$l]
				);
				
				$att->header_set('MIME-Version');
				$att->header_set('Date');
				$email->parts_add([$att]);
			}
		}	
	
		# ----- Send mail
		if ( $Flag_SendEmail == 1 ) {
			
			use Email::Sender::Simple qw(sendmail);
			use Email::Sender::Transport::SMTP qw();
			use Try::Tiny;
			
			print " -> " . $SystemUserEmail{'From'} . " to " . $SystemUserEmail{'To'};
			 
			try {
			  sendmail(
			    $email,
			    {
			      from => $FromDomain,
			      transport => Email::Sender::Transport::SMTP->new({
			          host =>"localhost",
			          port =>"25",
			      })
			    }
			  );
			} catch {
			    warn "sending failed: $_";
			};
			
			if ($Flag_Test == 0 || $Flag_Test == 2) {
				
				if ($SentEmailCounter++ >= $CounterTest) {
					print "\n";
					exit();
				}
			}	
		}
		
		print "\n";
	}		
}
