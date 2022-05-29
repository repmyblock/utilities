To use the modules, you need to have the username and password of the database in ~/.<filename> with the following entries.

dbname_prod: NYSVoters              <- Database table space.
dbhost: someproddb.repmyblock.org   <- Database hostname
dbport: 3306                        <- Database port
dbuser: usracct                     <- Database user
dbpass: usracct                     <- Database password

At the moment all the perl files need to start this way:

my $StateToUse = "RepMyBlock::NY";
my $RepMyBlock 		= ${StateToUse}->new();
my $dbhRawVoters 	= $RepMyBlock->InitDatabase("dbname_voters");
my $dbhVoters			= $RepMyBlock->InitDatabase("dbname_rmb");
print "Init the voters: " . $RepMyBlock->InitializeVoterFile() . "\n";

In the RepMyBlock.pm file, the config is called directly.
ToDo: Load a config file that is in the home directory that will not by accident end up in the GitHub reposatory.

sub InitDatabase {
	my $cfg = new Config::Simple('/home/usracct/.repmyblockdb');


What each file does.
UploadBOENYStickers.pl -> This is a tool to upload the BOE Stickers into the DB automatically when the BOE gives the numbers.
As of 3/29/2022, it should load the file FillingTrack that should reference the paperwork in the FillingDoc table.

