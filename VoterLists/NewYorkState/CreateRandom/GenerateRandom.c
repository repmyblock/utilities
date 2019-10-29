// This C file is to generate a random voter files using the data in the 
// real voter file. Because addresses are real in each district, the file
// need to comply to some sense of reality when it come to cartography but 
// at the same time be able to have a subset to be able to devellop.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#ifdef TEST
#define BLK_TOREAD 		1000
#define FILENAME      		"MyTestFile.txt"
#else
#define BLK_TOREAD    		50000000
#define FILENAME      		"AllNYSVoters_20190903.txt"
#endif

#define BLK_TOTEST		5000

#define NUM_COUNTIES 		63
#define NUM_DESC 		38

#define ASCII_YELLOW 		"\033[33m"
#define ASCII_B_BLACK 		"\033[30;1m"
#define ASCII_B_RED 		"\033[31;1m"
#define ASCII_B_GREEN		"\033[32;1m"
#define ASCII_B_YELLOW 		"\033[33;1m"
#define ASCII_B_BLUE 		"\033[34;1m"
#define ASCII_B_MAGENTA 	"\033[35;1m"
#define ASCII_B_CYAN 		"\033[36;1m"
#define ASCII_B_WHITE 		"\033[37;1m"

#define ASCII_BB_RED 		"\033[41;1m"
#define ASCII_BB_GREEN 		"\033[42;1m"
#define ASCII_BB_YELLOW 	"\033[43;1m"
#define ASCII_BB_BLUE 		"\033[44;1m"
#define ASCII_BB_MAGENTA 	"\033[45;1m"
#define ASCII_BB_CYAN 		"\033[46;1m"
#define ASCII_BB_WHITE	 	"\033[47;1m"

#define ASCII_F_RED 	       	"\033[41;5m"
#define ASCII_F_GREEN 		"\033[42;5m"
#define ASCII_F_YELLOW 		"\033[43;5m"
#define ASCII_F_BLUE 		"\033[44;5m"
#define ASCII_F_MAGENTA 	"\033[45;5m"
#define ASCII_F_CYAN 		"\033[46;5m"
#define ASCII_F_WHITE	 	"\033[47;5m"

#define ASCII_RESET		"\033[0m"

char *Counties[] = { "", "Albany", "Allegany", "Bronx", "Broome", "Cattaraugus", "Cayuga", "Chautauqua", "Chemung",
		     "Chenango", "Clinton", "Columbia", "Cortland", "Delaware", "Dutchess", "Erie", "Essex", "Franklin", 
		     "Fulton", "Genesee", "Greene", "Hamilton", "Herkimer", "Jefferson", "Kings", "Lewis", "Livingston", 
		     "Madison", "Monroe", "Montgomery", "Nassau", "New York", "Niagara", "Oneida", "Onondaga", "Ontario", 
		     "Orange", "Orleans", "Oswego", "Otsego", "Putnam", "Queens", "Rensselaer", "Richmond", "Rockland", 
		     "Saratoga", "Schenectady", "Schoharie", "Schuyler", "Seneca", "St.Lawrence", "Steuben", "Suffolk", 
		     "Sullivan", "Tioga", "Tompkins", "Ulster", "Warren", "Washington", "Wayne", "Westchester", "Wyoming", 
		     "Yates" };

char *Description[] = { "Yes", "No", "Democratic", "Republican", "Conservative", "Working Families", "Green", "Libertarian", 
			"Independence", "Serve America Mvmnt", "Other", "No party affiliation", "Women's Equality Party", 
			"Reform", "Agency", "County Board of Elections", "Department of Motor Vehicle", "Local Registrar", 
			"Mail-in through USPS", "School", "Active", "Active Military", "Active Special Federal", 
			"Active Special Presidential", "Active UOCAVA", "Inactive", "Purged", "Prereg - 17 Year Olds",
			"Adjudged Incompetent", "Death", "Duplicate", "Felon", "Mail Check", "Moved out of County", 
			"NCOA","NVRA", "Returned Mail", "Voter Request" };
										    											
char *Abbreviation[] = { "Y", "N", "DEM", "REP", "CON", "WOR", "GRE", "LBT", "IND", "SAM", "OTH", "BLK", "WEP", "REF",
			 "AGCY", "CBOE", "DMV", "LOCALREG", "MAIL", "SCHOOL", "A", "AM", "AF", "AP", "AU", "I", "P", "17", 
			 "ADJ-INCOMP", "DEATH", "DUPLICATE", "FELON", "MAIL-CHECK", "MOVED", "NCOA", "NVRA", "RETURN-MAIL", 
			 "VOTER-REQ" };
													
char *Usages[] = { "LASTNAME", "FIRSTNAME", "MIDDLENAME", "NAMESUFFIX", "RADDNUMBER", 
		   "RHALFCODE", "RAPARTMENT", "RPREDIRECTION", "RSTREETNAME", "RPOSTDIRECTION",
		   "RCITY", "RZIP5", "RZIP4", "MAILADD1", "MAILADD2", "MAILADD3", "MAILADD4", "DOB",
		   "GENDER", "ENROLLMENT", "OTHERPARTY", "COUNTYCODE", "ED", "LD", "TOWNCITY",	
		   "WARD", "CD", "SD", "AD", "LASTVOTEDDATE", "PREVYEARVOTED", "PREVCOUNTY",
		   "PREVADDRESS", "PREVNAME", "COUNTYVRNUMBER", "REGDATE", "VRSOURCE", 		
		   "IDREQUIRED", "IDMET", "STATUS", "REASONCODE", "INACT_DATE", "PURGE_DATE", 		
		   "SBOEID", "VoterHistory" };
							
typedef struct node {
  char *val;
  struct node *next;
} node_t;

typedef struct VoterList  {				
  char *LASTNAME; 	// 1 50 	LASTNAME 	CHARACTER Last name 
  char *FIRSTNAME; 	// 2 50 	FIRSTNAME 	CHARACTER First name 
  char *MIDDLENAME; 	// 3 50 	MIDDLENAME 	CHARACTER Middle Name 
  char *NAMESUFFIX; 	// 4 10 	NAMESUFFIX 	CHARACTER Name suffix  Jr, Sr, I, II,, 1, 2, etc 
  char *RADDNUMBER; 	// 5 10 	RADDNUMBER 	CHARACTER Residence House Number Hyphenated numbers allowed  
  char *RHALFCODE; 	// 6 10 	RHALFCODE 	CHARACTER Residence Fractional Address ., 1/3, etc. 
  char *RAPARTMENT; 	// 7 15 	RAPARTMENT 	CHARACTER Residence Apartment 
  char *RPREDIRECTION; 	// 8 10 	RPREDIRECTION 	CHARACTER Residence Pre Street Direction (e.g.. the 'E' in 52 E Main St) 
  char *RSTREETNAME; 	// 9 70 	RSTREETNAME 	CHARACTER Residence Street Name 
  char *RPOSTDIRECTION; // 10 10 	RPOSTDIRECTION 	CHARACTER Residence Post Street Direction (e.g. the 'SW' in 1200 Pecan Blvd SW ) 
  char *RCITY; 		// 11 50 	RCITY 		CHARACTER Residence City 
  char *RZIP5; 		// 12 5 	RZIP5 		CHARACTER Residence Zip Code 5 
  char *RZIP4; 		// 13 4 	RZIP4 		CHARACTER Zip code plus 4 
  char *MAILADD1; 	// 14 100 	MAILADD1 	CHARACTER Mailing Address 1 Free Form address 
  char *MAILADD2; 	// 15 100 	MAILADD2 	CHARACTER Mailing Address 2 Free Form address 
  char *MAILADD3; 	// 16 100 	MAILADD3 	CHARACTER Mailing Address 3 Free Form address
  char *MAILADD4; 	// 17 100 	MAILADD4 	CHARACTER Mailing Address 4 Free Form address
  char *DOB; 		// 18 8 	DOB 		CHARACTER Date of Birth YYYYMMDD	  
  char *GENDER; 	// 19 1 	GENDER 		CHARACTER Gender M = Male F = Female OPTIONAL 
  char *ENROLLMENT; 	// 20 3 	ENROLLMENT 	CHARACTER Political Party 
  char *OTHERPARTY; 	// 21 30 	OTHERPARTY 	CHARACTER Name or Party if Voter Checks 'Other' on registration form.
  char *COUNTYCODE; 	// 22 2 	COUNTYCODE 	NUMBER	County code 2 Digit County Code see countycodes below 
  char *ED; 		// 23 3 	ED 		NUMBER	Election district LOCAL  
  char *LD; 		// 24 3 	LD 		NUMBER	Legislative district LOCAL 
  char *TOWNCITY; 	// 25 30	TOWNCITY 	CHARACTER Town/City LOCAL 
  char *WARD; 		// 26 3 	WARD 		CHARACTER Ward LOCAL 
  char *CD;		// 27 3 	CD 		NUMBER	Congressional district 
  char *SD; 		// 28 3 	SD 		NUMBER	Senate district 
  char *AD; 		// 29 3 	AD 		NUMBER	Assembly district 
  char *LASTVOTEDDATE; 	// 30 8 	LASTVOTEDDATE 	CHARACTER Last date voted YYYYMMDD 
  char *PREVYEARVOTED; 	// 31 4 	PREVYEARVOTED 	CHARACTER Last year voted (from registration form) Optional 
  char *PREVCOUNTY; 	// 32 2 	PREVCOUNTY 	CHARACTER Last county voted in (from registration form ). Optional 
  char *PREVADDRESS; 	// 33 100 	PREVADDRESS 	CHARACTER Last registered address Optional 
  char *PREVNAME; 	// 34 150 	PREVNAME 	CHARACTER Last registered name (if different) Optional Field Position Field Size (Maximum)
                        //                                        SBOE Field Name/Type Description Notes  
  char *COUNTYVRNUMBER; // 35 50 	COUNTYVRNUMBER 	CHARACTER County Voter Registration Number. Assigned County 
  char *REGDATE;	// 36 8 	REGDATE 	CHARACTER Application Date YYYYMMDD (date application was received or postmarked) 
  char *VRSOURCE;	// 37 10 	VRSOURCE 	CHARACTER Application Source
  char *IDREQUIRED; 	// 38 1 	IDREQUIRED 	CHARACTER Identification Required Flag.
  char *IDMET; 		// 39 1 	IDMET 		CHARACTER Identification Verification Requirement Met Flag.
                        //                                        Indicates verification requirements (SSN, Driver ID verified or other acceptable ID) 
  char *STATUS; 	// 40 10 	STATUS 		CHARACTER Voter Status Codes.  
  char *REASONCODE; 	// 41 15 	REASONCODE 	CHARACTER Status Reason Codes 
  char *INACT_DATE; 	// 42 8 	INACT_DATE 	CHARACTER Date Voter made 'Inactive' YYYYMMDD 
  char *PURGE_DATE; 	// 43 8 	PURGE_DATE	CHARACTER Date voter was 'Purged' YYYYMMDD 
  char *SBOEID;		// 44 50 	SBOEID 		CHARACTER Unique NYS Voter ID 
  char *VoterHistory; 	// 45 1200 	VoterHistory 	CHARACTER Voter History Last 5 years voting history separated by semicolons 
  struct VoterList *next;
} VoterList; 

// "SEXTON","COLLEEN","M","","703","","","","WURLITZER DR","","N TONAWANDA","14120","1948","",
// "","","","19710331","F","DEM","","32","6","9","N Tonawanda","003","26","62","140","20121106",
// "","  ","","","M098132","19890413","CBOE","N","Y","PURGED","MOVED","","20140611","NY000000000003306194",
//"2012 General Election;2010 General Election;2008 General Election"

int push(node_t **, node_t **, char *);
void PushVoterList(VoterList *, char *, int);
void PrintVoterList(VoterList *);
void print_ASCII(node_t **);
void print_list(node_t *);
int PrintValue(char *, int);
char *CheckDup(node_t *, node_t **, char **);
int *AssignVoterToNode();

int main(void) {
  FILE *fptr;

  char *TextFile = NULL;
  char *pTextFile = NULL;
  char *pBegMarker = NULL;
  char *pEndMarker = NULL;
  char *pStoredValue = NULL;
  char *pLeftOver = NULL;
  char *pReturnValue = NULL;
  char *pNewLineLoc = NULL;

  int i = 0, marker = 0;
  int toggle_inside = 0;
  int debug = 0;
  int NewStringSize = 0;
  long int SizeLeftOver = 0;
  long int total_size_read = 0;
  int StringSegmentCounter = 0;
	
  long int FileCounter = 0;
	
  time_t PrevTime = 0, CurrTime = 0, MiddleTime = 0;
	
  unsigned long int BlockNumber = 0;

  // This is the node where to store the links	
  node_t *head = NULL;
  
  // This is to store the upper bound and lower bound of pointer to each letters.
  // This is needed to traverse only the names starting by the same letter.
  // This allow to speed the search of duplicates. 0-127 is the lower_bound of a letter, 128-256 is the upper bound.
  node_t *ByASCIICode[256] = { NULL };  

  head = malloc(sizeof(node_t));
  if (head == NULL) { printf("Error at malloc\n"); exit(1); }		
  head->next = NULL;
  
  // This is to assign the Voter to the node
  // Note: At the botton, VotersHead->next == NULL is checked to see if the first block is read so to advance pointer.
  VoterList *VotersHead = NULL;
  VotersHead = malloc(sizeof(VoterList));
  VoterList *VotersStart = VotersHead;
  VoterList *VotersPrev = NULL;
  if (VotersHead == NULL) { printf("Error at malloc\n"); exit(1); }
  VotersHead->next = NULL;
  VoterList *VotersList = VotersHead;
  
  // This is to open the file
  fptr = fopen(FILENAME, "r");
  TextFile = malloc ((BLK_TOREAD + 1) * sizeof(char));
  if (TextFile == NULL) { printf("Error at malloc\n"); exit(1); }
	
  //while ( ! feof(fptr) ) {	
  while ((total_size_read = fread((TextFile + SizeLeftOver), 1, BLK_TOREAD, fptr)) ) {
    pTextFile = TextFile;		
    BlockNumber++;

    CurrTime = time(NULL);
    PrevTime = CurrTime;
		
    if ( SizeLeftOver > 0) {
      memcpy(TextFile, pLeftOver, SizeLeftOver);
      free(pLeftOver);
      pLeftOver = NULL;
    }
		
    pTextFile[BLK_TOREAD+SizeLeftOver] = '\0';
    char *TextFileMax = pTextFile + strlen(pTextFile);
		
    // Need to figure out if the , and the " are outside or inside the system
    // Need to check that the \" is not actually being escaped by
    char *pSpecial;
    
    pSpecial = strstr(pTextFile, "\",\"");
    if (pSpecial == pTextFile) { pTextFile++; }
    
    pSpecial = strstr(pTextFile, "\"\r\n\"");
    if (pSpecial == pTextFile) { pTextFile += 2; }

    int SizeMarker = 0;

    do {
      int OffSet = 3;
      pBegMarker = strstr(pTextFile, "\"");
      pBegMarker++;

      if (pBegMarker != NULL) {
	pEndMarker = strstr(pBegMarker, "\",\"");
	if ( pEndMarker != NULL) {
	  pEndMarker += OffSet;
	}
	pNewLineLoc = strstr(pBegMarker, "\"\r\n\"");
      }
      
      if (pEndMarker == NULL && pNewLineLoc != NULL) {
	pEndMarker = pNewLineLoc;
      }
      
      if (pEndMarker != NULL) {
	SizeMarker = pEndMarker - pBegMarker - OffSet;
	if ( SizeMarker > 0) {	  
	  pStoredValue = NULL;
	  if (pNewLineLoc != NULL && StringSegmentCounter > 43) {						
	    SizeMarker = pNewLineLoc - pBegMarker;
	  }

	  if ( SizeMarker > 0) {
	    pStoredValue = malloc ((SizeMarker + 1) * sizeof(char));
	    if (pStoredValue == NULL) { printf("Error at malloc\n"); exit(1); }
	    strncpy(pStoredValue, pBegMarker, SizeMarker);
	    pStoredValue[SizeMarker] = '\0';	    
	    pReturnValue = CheckDup(head, ByASCIICode, &pStoredValue);
	    if (pReturnValue == pStoredValue && pStoredValue != NULL) {
	      push(&head, ByASCIICode, pReturnValue);
	    }
	  }
	}
				
	// Chunk dealing with storing the data				
	if ( StringSegmentCounter == 0) {	  
	  if ( VotersHead->next != NULL) { pTextFile++; } // This is to account for the new line by checking
	  VotersList = malloc(sizeof(VoterList));
	  if (VotersList == NULL) { printf("Error at malloc\n"); exit(1); }
	  VotersList->next = VotersHead;
	  VotersHead = VotersList;
	}				
				
	PushVoterList(VotersList, pReturnValue, StringSegmentCounter);
	pReturnValue = NULL;

	// End of Chunk
	pTextFile += SizeMarker + OffSet;
	StringSegmentCounter++;			
      } 
			
      if ( StringSegmentCounter > 44) {
	StringSegmentCounter = 0;
      }
    } while (pTextFile < TextFileMax && pEndMarker != NULL );	
			
    // it ended the copy. Whatever is left get copied to another string
    SizeLeftOver = TextFileMax - pTextFile + 1;
		
    if (SizeLeftOver > 0 ) {	
      pLeftOver = malloc (SizeLeftOver * sizeof(char));
      if (pLeftOver == NULL) { printf("Error at malloc\n"); exit(1); }
      memcpy(pLeftOver, (pTextFile-1), SizeLeftOver);
    }
		
    free(TextFile);
    TextFile = NULL;
    TextFile = malloc ((BLK_TOREAD + SizeLeftOver + 1) * sizeof(char));	
    if (TextFile == NULL) { printf("Error at malloc\n"); exit(1); }		       
  }
	
  printf("\n");
  printf("\nVoter List\n");
  PrintVoterList(VotersList->next);
  printf("Done\n");
  //print_ASCII(ByASCIICode);
  //print_list(head);
}

int PrintValue(char *pStoredValue, int debug) {
  int j;
	
  for (j = 0; j < strlen(pStoredValue) + 1; j++) {
    printf("\t%c %d\n", *(pStoredValue + j), *(pStoredValue + j));
  }

  return 0;
}

void print_ASCII(node_t **ASCII) {
  int i = 0;
	
  for (i = 32; i < 126; i++) {
    if ( (ASCII[i]) != NULL) {
      printf("\tASCII[" ASCII_B_RED "%c - %3d" ASCII_RESET "]: %s\t" ASCII_B_BLUE "%p %p" ASCII_RESET "\n", i, i, (ASCII[i])->val, (ASCII[i]), (ASCII[i+127]));
    } else {
      printf("\tASCII[" ASCII_B_RED "%c - %3d" ASCII_RESET "]: %s\t" ASCII_B_BLUE "%p %p" ASCII_RESET "\n", i, i, "NON EXISTANT", (ASCII[i]), (ASCII[i+127]));
    }
  }
}

void print_list(node_t *head) {
  int counter = 0;
  node_t *current = head;
  
  while (current->next != NULL) {
    printf("\t%5d\tNODE VAL:\t%s\t-> %p\tCURRENT: %p\tNEXT: %p\n", counter++, current->val, current->val, current, current->next); 		
    current = current->next;
  } 
  
  printf("Counter: %d\n", counter);
}
	
char *CheckDup(node_t *head, node_t **ASCII, char **val) {
  if ( *val == NULL ) { return NULL; } 
  node_t *current = ASCII[**val];
  int upper_bound = **val;
  
  if ( current != NULL ) {
    while ( current->next != NULL  ) {
      if ( ! strcmp(*val, current->val)) {
	free(*val);
	*val = NULL;
	return current->val;
      }	 	   

      if ( current == ASCII[upper_bound+127] ) {
	break;
      }

      current = current->next;
    } 
  }
  return *val;
}

	
int push(node_t **head, node_t **ASCII, char *val) {	
  node_t *new_node = NULL;
  node_t *existing_node = NULL;
  node_t *swaped_node;
  node_t *last_node;
	
  new_node = malloc(sizeof(node_t));
  if (new_node == NULL) {
    printf("Error at malloc\n");
    exit(1);
  }
  new_node->val = val;
	
  if (val == NULL) {
    return 1;
  }
	
  int lower_bound = 0, upper_bound = 0;
	
  // If ASCII is null it means it was not used so we need to use it		
  // It's where the next goes that is important.
  if ( ASCII[*val] != NULL ) {	
    existing_node = (ASCII[*val])->next;
    (ASCII[*val])->next = new_node;
    if ( ASCII[(*val)] == ASCII[(*val)+127] ) { ASCII[(*val)+127] = new_node; }
    new_node->next = existing_node;

				
  } else {
    // Verify each of the ASCII to make sure they are added in order.
    // Search the next one up with a value not null
    for ( lower_bound = (*val); lower_bound > 31 && (ASCII[lower_bound]) == NULL; lower_bound--);
    for ( upper_bound = (*val); upper_bound < 127 && (ASCII[upper_bound]) == NULL; upper_bound++);
    ASCII[*val] = new_node;
	
    if (ASCII[lower_bound] == NULL) {
      // Need to move pointer to end of upper.
      existing_node = ASCII[lower_bound];
      if ( existing_node != NULL) {
	while (existing_node->next != NULL) {
	  existing_node = existing_node->next;
	} 
      }
      
      ASCII[(*val)] = new_node;
      ASCII[(*val)+127] = new_node;
      
      new_node->next = *head;
      *head = ASCII[(*val)];

    } else if ( ASCII[lower_bound] != NULL) {
      
      ASCII[(*val)] = new_node;
			
      existing_node = ASCII[lower_bound];
      last_node = ASCII[lower_bound+127];

      while (existing_node->next != NULL && ASCII[upper_bound+127] == last_node ) { 		
	last_node = existing_node;
	existing_node = existing_node->next;
      }
      
      swaped_node = last_node->next;
      last_node->next = new_node;
      ASCII[(*val)+127] = new_node;
      new_node->next = swaped_node;			
    }		
  }
  return 1;	
}

/*
  int oldpush(node_t **head, node_t **ASCII, char *val) {	
  node_t *new_node;
	
  new_node = malloc(sizeof(node_t));
  new_node->val = val;
  new_node->next = *head;

  *head = new_node;	

  return 1;	
  }
*/


void PrintVoterList(VoterList *Voters) {	
  VoterList *Voter = Voters;
	
  printf("Getting Begging address: %p -> Next: %p\n", Voter, Voter->next);
	
  while (Voter->next != NULL ) { 		
    printf("Getting Begin address: %p\n", Voter);
    if ( Voter->LASTNAME != NULL) { printf("\tLASTNAME:\t"ASCII_B_MAGENTA"%p\t"ASCII_RESET ASCII_B_BLUE"#" ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->LASTNAME,Voter->LASTNAME); } 
    if ( Voter->FIRSTNAME != NULL) { printf("\tFIRSTNAME:\t"ASCII_B_MAGENTA"%p\t"ASCII_RESET ASCII_B_BLUE"#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->FIRSTNAME,Voter->FIRSTNAME); } 
    if ( Voter->MIDDLENAME != NULL) { printf("\tMIDDLENAME:\t"ASCII_B_MAGENTA"%p\t"ASCII_RESET ASCII_B_BLUE"#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->MIDDLENAME, Voter->MIDDLENAME); } 
    if ( Voter->NAMESUFFIX != NULL) { printf("\tNAMESUFFIX:\t"ASCII_B_MAGENTA"%p\t"ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->NAMESUFFIX, Voter->NAMESUFFIX); } 
    if ( Voter->RADDNUMBER != NULL) { printf("\tRADDNUMBER:\t"ASCII_B_MAGENTA"%p\t"ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RADDNUMBER, Voter->RADDNUMBER); } 
    if ( Voter->RHALFCODE != NULL) { printf("\tRHALFCODE:\t"ASCII_B_MAGENTA"%p\t"ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RHALFCODE, Voter->RHALFCODE); } 
    if ( Voter->RAPARTMENT != NULL) { printf("\tRAPARTMENT:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RAPARTMENT, Voter->RAPARTMENT); } 
    if ( Voter->RPREDIRECTION != NULL) { printf("\tRPREDIRECTION:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RPREDIRECTION, Voter->RPREDIRECTION); } 
    if ( Voter->RSTREETNAME != NULL) { printf("\tRSTREETNAME:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RSTREETNAME, Voter->RSTREETNAME); } 
    if ( Voter->RPOSTDIRECTION != NULL) { printf("\tRPOSTDIRECTION:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RPOSTDIRECTION, Voter->RPOSTDIRECTION); } 
    if ( Voter->RCITY != NULL) { printf("\tRCITY:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RCITY, Voter->RCITY); } 
    if ( Voter->RZIP5 != NULL) { printf("\tRZIP5:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RZIP5, Voter->RZIP5); } 
    if ( Voter->RZIP4 != NULL) { printf("\tRZIP4:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->RZIP4, Voter->RZIP4); } 
    if ( Voter->MAILADD1 != NULL) { printf("\tMAILADD1:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->MAILADD1, Voter->MAILADD1); } 
    if ( Voter->MAILADD2 != NULL) { printf("\tMAILADD2:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->MAILADD2, Voter->MAILADD2); } 
    if ( Voter->MAILADD3 != NULL) { printf("\tMAILADD3:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->MAILADD3, Voter->MAILADD3); } 
    if ( Voter->MAILADD4 != NULL) { printf("\tMAILADD4:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->MAILADD4, Voter->MAILADD4); } 
    if ( Voter->DOB != NULL) { printf("\tDOB:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->DOB, Voter->DOB); } 
    if ( Voter->GENDER != NULL) { printf("\tGENDER:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->GENDER, Voter->GENDER); } 
    if ( Voter->ENROLLMENT != NULL) { printf("\tENROLLMENT:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->ENROLLMENT, Voter->ENROLLMENT); } 
    if ( Voter->OTHERPARTY != NULL) { printf("\tOTHERPARTY:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->OTHERPARTY, Voter->OTHERPARTY); } 
    if ( Voter->COUNTYCODE != NULL) { printf("\tCOUNTYCODE:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->COUNTYCODE, Voter->COUNTYCODE); } 
    if ( Voter->ED != NULL) { printf("\tED:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->ED, Voter->ED); } 
    if ( Voter->LD != NULL) { printf("\tLD:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->LD, Voter->LD); } 
    if ( Voter->TOWNCITY != NULL) { printf("\tTOWNCITY:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->TOWNCITY, Voter->TOWNCITY); } 
    if ( Voter->WARD != NULL) { printf("\tWARD:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->WARD, Voter->WARD); } 
    if ( Voter->CD != NULL) { printf("\tCD:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->CD, Voter->CD); } 
    if ( Voter->SD != NULL) { printf("\tSD:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->SD, Voter->SD); } 
    if ( Voter->AD != NULL) { printf("\tAD:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->AD, Voter->AD); } 
    if ( Voter->LASTVOTEDDATE != NULL) { printf("\tLASTVOTEDDATE:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->LASTVOTEDDATE, Voter->LASTVOTEDDATE); } 
    if ( Voter->PREVYEARVOTED != NULL) { printf("\tPREVYEARVOTED:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->PREVYEARVOTED, Voter->PREVYEARVOTED); } 
    if ( Voter->PREVCOUNTY != NULL) { printf("\tPREVCOUNTY:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->PREVCOUNTY, Voter->PREVCOUNTY); } 
    if ( Voter->PREVADDRESS != NULL) { printf("\tPREVADDRESS:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->PREVADDRESS, Voter->PREVADDRESS); } 
    if ( Voter->PREVNAME != NULL) { printf("\tPREVNAME:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->PREVNAME, Voter->PREVNAME); } 
    if ( Voter->COUNTYVRNUMBER != NULL) { printf("\tCOUNTYVRNUMBER:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->COUNTYVRNUMBER, Voter->COUNTYVRNUMBER); } 
    if ( Voter->REGDATE != NULL) { printf("\tREGDATE:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->REGDATE, Voter->REGDATE); } 
    if ( Voter->VRSOURCE != NULL) { printf("\tVRSOURCE:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->VRSOURCE, Voter->VRSOURCE); } 
    if ( Voter->IDREQUIRED != NULL) { printf("\tIDREQUIRED:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->IDREQUIRED, Voter->IDREQUIRED); } 
    if ( Voter->IDMET != NULL) { printf("\tIDMET:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->IDMET, Voter->IDMET); } 
    if ( Voter->STATUS != NULL) { printf("\tSTATUS:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->STATUS, Voter->STATUS); } 
    if ( Voter->REASONCODE != NULL) { printf("\tREASONCODE:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->REASONCODE, Voter->REASONCODE); } 
    if ( Voter->INACT_DATE != NULL) { printf("\tINACT_DATE:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->INACT_DATE, Voter->INACT_DATE); } 
    if ( Voter->PURGE_DATE != NULL) { printf("\tPURGE_DATE:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->PURGE_DATE, Voter->PURGE_DATE); } 
    if ( Voter->SBOEID != NULL) { printf("\tSBOEID:\t\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->SBOEID, Voter->SBOEID); } 
    if ( Voter->VoterHistory != NULL) { printf("\tVoterHistory:\t" ASCII_B_MAGENTA"%p\t" ASCII_RESET ASCII_B_BLUE  "#"ASCII_RESET "%s" ASCII_B_BLUE  "#" ASCII_RESET "\n", Voter->VoterHistory, Voter->VoterHistory); } 
    printf("Voter List: %p\n", Voter->next);
		
    Voter = Voter->next;
  } 
}

void PushVoterList(VoterList *Voter, char *val, int count) {
	
  //printf("Address Voter: %p\tAddress Val: %p %s\tCount: %d\n", Voter, val, val, count);	

  switch(count) {
  case 0: Voter->LASTNAME = val; break;		
  case 1: Voter->FIRSTNAME = val; break;
  case 2: Voter->MIDDLENAME = val; break;		
  case 3: Voter->NAMESUFFIX = val; break;		
  case 4: Voter->RADDNUMBER = val; break;		
  case 5: Voter->RHALFCODE = val; break;		
  case 6: Voter->RAPARTMENT = val; break;		
  case 7: Voter->RPREDIRECTION = val; break;		
  case 8: Voter->RSTREETNAME = val; break;		
  case 9: Voter->RPOSTDIRECTION = val; break;		
  case 10: Voter->RCITY = val; break;		
  case 11: Voter->RZIP5 = val; break;		
  case 12: Voter->RZIP4 = val; break;		
  case 13: Voter->MAILADD1 = val; break;		
  case 14: Voter->MAILADD2 = val; break;		
  case 15: Voter->MAILADD3 = val; break;		
  case 16: Voter->MAILADD4 = val; break;		
  case 17: Voter->DOB = val; break;		
  case 18: Voter->GENDER = val; break;		
  case 19: Voter->ENROLLMENT = val; break;		
  case 20: Voter->OTHERPARTY = val; break;		
  case 21: Voter->COUNTYCODE = val; break;		
  case 22: Voter->ED = val; break;		
  case 23: Voter->LD = val; break;		
  case 24: Voter->TOWNCITY = val; break;		
  case 25: Voter->WARD = val; break;		
  case 26: Voter->CD = val; break;		
  case 27: Voter->SD = val; break;		
  case 28: Voter->AD = val; break;		
  case 29: Voter->LASTVOTEDDATE = val; break;		
  case 30: Voter->PREVYEARVOTED = val; break;		
  case 31: Voter->PREVCOUNTY = val; break;		
  case 32: Voter->PREVADDRESS = val; break;		
  case 33: Voter->PREVNAME = val; break;		
  case 34: Voter->COUNTYVRNUMBER = val; break;		
  case 35: Voter->REGDATE = val; break;		
  case 36: Voter->VRSOURCE = val; break;		
  case 37: Voter->IDREQUIRED = val; break;		
  case 38: Voter->IDMET = val; break;		
  case 39: Voter->STATUS = val; break;		
  case 40: Voter->REASONCODE = val; break;		
  case 41: Voter->INACT_DATE = val; break;		
  case 42: Voter->PURGE_DATE = val; break;		
  case 43: Voter->SBOEID = val; break;		
  case 44: Voter->VoterHistory = val;break;
  }
	
}
