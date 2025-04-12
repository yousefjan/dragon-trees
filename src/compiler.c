/* --------------------------------------------------------------------- *
										 	 ___								 		 _
											// \\  	 _   |	 _	 	 	 \\
									   //	 	 		 \\  |	//		 	 //
									  //		  		\\___//		 	 	//
						  			\\____	 		//   \\   	 //
										   	 \\ ___//		  \\___	//
													\\	 \\		  //   //
									 _      //	  \\___//   //
									 \\    //	  	//   \\  //			_
									  \\__//	 	 //  |  \\ \\____//
									   \--/			/    |  	\ \----/
                             Developments!
 * --------------------------------------------------------------------- *
 	SoL Developments - SunCode -- DTIFS Compiler -- v 0.02.09 SoL 2005.

  	start-ed 27-04-05
		last-ed 03-05-05

	I'm using tab-size = 2 chars and font = Courier new 13 pixels.
 * --------------------------------------------------------------------- *
   /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
/* -------------------------------------------------------------------- *
			DEFINES
 * --------------------------------------------------------------------- */
#define TITLE	"SunCode's DTIFS Compiler v 0.02.09"
#define WIN32_LEAN_AND_MEAN

#define true  ( 0 == 0 )
#define false ( 0 == 1 )
// Number of keywords in list:
#define KEYWORDS 	( 33 )

// KW words:
#define KWCOMM 			( 0 )
#define KWENDCOMM 	( 1 )
#define KWDTIFS 		( 2 )
#define KWENDDTIFS	( 3 )
#define KWNAME 			( 4 )
#define KWBRANCH		( 5 )
#define KWUSEH			( 6 )
#define KWGLOBAL 		( 7 )
#define KWSCBHIG		( 8 )
#define KWUSETW 		( 9 )
#define KWRADIUS 		( 10 )
#define KWHIGH 			( 11 )
#define KWSTEM 			( 12 )
#define KWENDSTEM		( 13 )
#define KWBRA1 			( 14 )
#define KWENDBRA1		( 15 )
#define KWBRA2 			( 16 )
#define KWENDBRA2		( 17 )
#define KWBRA3 			( 18 )
#define KWENDBRA3		( 19 )
#define KWBRA4 			( 20 )
#define KWENDBRA4		( 21 )
#define KWBRA5 			( 22 )
#define KWENDBRA5		( 23 )
#define KWBRA6 			( 24 )
#define KWENDBRA6		( 25 )
#define KWBRA7 			( 26 )
#define KWENDBRA7		( 27 )
#define KWHEIGHT		( 28 )
#define KWSCALE			( 29 )
#define KWLEAN			( 30 )
#define KWROTATE		( 31 )
#define KWTWIST			( 32 )

// WAS words:
#define WASNOTFOUND ((char*)-1)
#define	WASNOERROR	( 0 )
#define	WASERROR		( 1 )
#define	WASNOMATCH	( 2 )
#define	WASMATCH		( 3 )

/* -------------------------------------------------------------------- *
			INCLUDES
 * --------------------------------------------------------------------- */
//#include <windows.h>
//#include <windowsx.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
/* -------------------------------------------------------------------- *
			VARIABLES.
 * --------------------------------------------------------------------- */
typedef char bool;
#include "atree.h"

	// Keyword list:
	char	* keywords [ KEYWORDS ] = { "Comm:",\
  																	"EndComm",\
                                    "DTIFS:",\
                                    "EndDTIFS",\
                                    "Name = ",\
                                    "Branches = ",\
                                    "Use heights = ",\
                                    "Global scale = ",\
                                    "Scale by height = ",\
                                    "Use twist = ",\
                                    "Radius = ",\
                                    "Height = ",\
                                    "Stem:",\
                                    "EndStem",\
                                    "Branch1:",\
                                    "EndBranch1",\
                                    "Branch2:",\
                                    "EndBranch2",\
                                    "Branch3:",\
                                    "EndBranch3",\
                                    "Branch4:",\
                                    "EndBranch4",\
                                    "Branch5:",\
                                    "EndBranch5",\
                                    "Branch6:",\
                                    "EndBranch6",\
                                    "Branch7:",\
                                    "EndBranch7",\
                                    "Height = ",\
                                    "Scale = ",\
                                    "Lean = ",\
                                    "Rotate = ",\
                                    "Twist = "\
  																};

	int			kwrdlen [ KEYWORDS ] = { 	5,\
  																	7,\
                                    6,\
                                    8,\
                                    7,\
                                    11,\
                                    14,\
                                    15,\
                                    18,\
                                    12,\
                                    9,\
                                    9,\
                                    5,\
                                    7,\
                                    8,\
                                    10,\
                                    8,\
                                    10,\
                                    8,\
                                    10,\
                                    8,\
                                    10,\
                                    8,\
                                    10,\
                                    8,\
                                    10,\
                                    8,\
                                    10,\
                                    9,\
                                    8,\
                                    7,\
                                    9,\
                                    8\
  																};

	int			Branchkeyword [ 8 ]		= { KWSTEM,\
  																	KWBRA1,\
  																	KWBRA2,\
  																	KWBRA3,\
  																	KWBRA4,\
  																	KWBRA5,\
  																	KWBRA6,\
  																	KWBRA7\
  																};

  // Needed for angles calculation:
  long double radian = 0.01745329252f;

  // The couzinz of 'cos' and 'sin':
  // (z to avoid name conflictz :) The angles biz
  //  given in degrees but saved as cos and sin)
	long double coz, zin;

	// Buffer for source file:
	char 		filebuf [ 32768 ];

	// Size of input file:
  unsigned int	filesize;

	// Buffers for filenames:
  char		inname 	[ 128 ];
  char		outname [ 128 ];

	// File handles:
  FILE	* infile;
  FILE	* outfile;

	// Temporary text buffer:
  char		tmptxt 	[ 256 ];

	// Current position in text:
  char	* curpos;

  // The tree being processed:
  int			current_tree;

  // Useful ints:
  int 		i;
  int 		j;
  int			n;
	int			result;
  int			count = 0;

  // Useful floats:
  long double	t;
  long double tempval;

  // Useful pointers:
	char	* tptr;
	char	* pointer;
	char	* inpointer;
	char	* outpointer;
	char	* blockstart;
	char	* treestart;

	// Used for index-pointers:
  long		ptrdiff;
  long		treesize;
  long		blocksize;

  // Buffer for trees:
  struct ATREE tree [ 31 ];

  // Temp flag:
  bool	flag;

/* -------------------------------------------------------------------- *
			FUNCTION DEFS
 * --------------------------------------------------------------------- */

	int		cntrltowhite ( void );
	int		compiletree ( void );
	void	dumpbuf ( char * tpos, int count );
	int		filenames ( char * fname );
  void	filltree ( void );
	int		findbranch ( int index );
	int		findbranches ( void );
	int		findflags ( void );
	int		findheight ( void );
	int		findlean ( void );
	int		findname ( void );
	int		findrotate ( void );
	int		findscale ( void );
  int		findsizes ( void );
	int		findtree ( void );
	int		findtwist ( void );
	void	getflag ( int index );
	void	heightswitcher ( int index, long double value );
	void	leanswitcher ( int index, long double value );
	int 	main ( int argc, char * argv[] );
  int 	opensource ( char * fname );
  int 	preproc ( void );
	int		remcomm ( void );
	int		remtree ( void );
	int		remwhite ( void );
	void	rotateswitcher ( int index, long double value );
	void	scaleswitcher ( int index, long double value );
	char	*	scanbuf ( char * tpos, int count, char * match, int matchlen );
	void	twistswitcher ( int index, long double value );
  void	viewbuf ( char * tpos, int count );
  int 	writebin ( char * fname );


/* -------------------------------------------------------------------- *
			FUNCTIONS
 * --------------------------------------------------------------------- */
int main ( int argc, char * argv[] )
{
	printf ("Hello World! This is %s at your service. \n\n", TITLE );

	// Any argument found?:
  if ( argc != 2 )
  {
  	// Else print usage text:
    printf ( "Usage: IFSC.EXE filename.TXT\n\n" );
    printf ( "Always use a extension or you will get an error.\n" );
    printf ( "'TXT' is no absolute, use any, like: *.C, *.ASM, *.*\n" );
    printf ( "The in-name is used to create the out-name, (*.IFS).\n" );
	  return ( 0 );
  } // Usage text.

  // Create filenames from argv:
  if ( WASERROR == filenames ( argv [ 1 ] ) )
  	return ( WASERROR );

  // Open source file:
  if ( WASERROR == opensource ( inname ) )
  	return ( WASERROR );
	printf ( " ... Done!\n" );

  // Preprocess:
  if ( WASERROR == preproc ( ) )
  	return ( WASERROR );
	printf ( " ... Done!\n" );

  // Buffer dumps:
//  viewbuf ( filebuf, 256 );
//  dumpbuf ( filebuf, 256 );

	// Set all trees to 'The Dummy Tree!':
	filltree ( );

  // Start from tree #00
	current_tree = 0;

  // Scan for trees:
  result = WASNOERROR;
	while ( WASMATCH == ( result = findtree ( ) ) )
  {
    // Something is not good?
    if ( result == WASERROR )
  		return ( WASERROR );
    // Na! it is!

		// Compile the tree:
		if ( WASERROR == compiletree ( ) )
    	return ( WASERROR );
		printf ( " ... Done!\n" );
    // Update tree counter:
	  current_tree++;
    // Remove compiled tree from list:
		remtree ( );
  }
	printf ( " #%i trees compiled and, ... Done!\n", current_tree );
	// Bad source-code?

  // Write destination file:
  if ( WASERROR == writebin ( outname ) )
  	return ( WASERROR );
	printf ( " ... Done!\n" );

	printf ( "\n\nPeeewh!, that was something! I hope this will work for you :)" );
	printf ( "\n\nSunCode May 2005." );

  return ( WASNOERROR );
}
/* -------------------------------------------------------------------- *
			Create output filename from input namne:
 * --------------------------------------------------------------------- */
int	filenames ( char * fname )
{
	strncpy ( inname, fname, 120 );
 	printf ( "INFILE: '%s' \n", inname );
	strncpy ( outname, inname, 120 );
	if ( NULL == ( curpos = strchr ( outname, '.' ) ) )
  {
  	fprintf ( stderr, "\n *** ERROR ***\n Bad filename!\n *************\n" );
    return ( WASERROR );
  }
 	strncpy ( curpos, ".IFS", 4 );
 	printf ( "OUTFILE: '%s' \n\n", outname );
  return ( WASNOERROR );
} // filenames.
/* -------------------------------------------------------------------- *
			Open and read source file:
 * --------------------------------------------------------------------- */
int opensource ( char * fname )
{
	printf ( "Attempting to open input-file: '%s',", fname );
  if ( NULL == ( infile = fopen ( fname, "r" ) ) )
  {
  	fprintf ( stderr, "\n\n **** ERROR ****\n File not found!\n ***************\n" );
    return ( WASERROR );
  }
 	if ( 0 == ( filesize = fread ( filebuf, sizeof ( char ), 32000, infile ) ) )
  {
  	fprintf ( stderr, "\n\n **** ERROR ****\n File is empty!\n ***************\n" );
    return ( WASERROR );
  }
	printf ( " reading %i bytes", filesize );
  return ( WASNOERROR );
} // opensource.
/* -------------------------------------------------------------------- *
			Write destination file:
 * --------------------------------------------------------------------- */
int writebin ( char * fname )
{
	printf ( "Attempting to write to output-file: '%s',", fname );
  if ( NULL == ( outfile = fopen ( fname, "w" ) ) )
  {
  	fprintf ( stderr, "\n\n **** ERROR ****\n File not found!\n ***************\n" );
    return ( WASERROR );
  }
  filesize = sizeof ( tree [ 0 ] ) * 31;
  if ( filesize > fwrite ( tree, sizeof ( char ), filesize, outfile ) )
  {
  	fprintf ( stderr, "\n\n **** ERROR *****\n Creation failed!\n ****************\n" );
    return ( WASERROR );
  }
	printf ( " writing %i bytes", filesize );
  return ( WASNOERROR );
} // writebin.
/* -------------------------------------------------------------------- *
			View n bytes of buffer, (used for testing):
 * --------------------------------------------------------------------- */
void viewbuf ( char * tpos, int count )
{
	printf ( "\nBuffer view:\n\n" );
  for ( i = 0; i < count; i++)
  	putchar ( tpos [ i ] );
	printf ( "\n\n" );
  return;
} // viewbuf.
/* -------------------------------------------------------------------- *
			View n bytes of buffer, (used for testing):
 * --------------------------------------------------------------------- */
void dumpbuf ( char * tpos, int count )
{
	printf ( "\nBuffer dump:\n\n" );
  for ( i = 0; i < count; i++)
  	printf ( "%c%i", tpos [ i ], tpos [ i ] );
	printf ( "\n\n" );
  return;
} // dumpbuf.
/* -------------------------------------------------------------------- *
			Preprocess:
 * --------------------------------------------------------------------- */
int preproc ( void )
{
	printf ( "Pre-processing: " );

  // Exchange all ASCII control-codes to spaces:
	cntrltowhite ( );

	count = 0;
  // Scan for and remove comments:
	while ( WASMATCH == ( result = remcomm ( ) ) )
  {
		count++;
  } // Remove comments.
	// Bad source-code?
  if ( result == WASERROR )
 			return ( WASERROR );
  else
  	printf ( " Removed %i Comm: fields,", count );

  // Scan for and remove double 'white' chars:
	count = 0;
//	while ( WASMATCH == remwhite ( ) )
//  {
//		count++;
//  } // Remove whites.
//  printf ( " removed %i white chars,", count );

 	return ( WASNOERROR );

} // preproc.
/* -------------------------------------------------------------------- *
			Scan for keyword:
 * --------------------------------------------------------------------- */
char	*	scanbuf ( char * tpos, int count, char * match, int matchlen )
{
	int index;

	while ( count >= 0 )
  {
  	for ( index = 0; index < matchlen; index++ )
			if ( tpos [ index ] != match [ index ] )
    		break;
    if ( index == matchlen )
    {
  		return ( tpos );
    }
    tpos++;
    --count;
  }
  return ( WASNOTFOUND );
} // scanbuf.
/* -------------------------------------------------------------------- *
			ASCII control-codes to 'white' space:
 * --------------------------------------------------------------------- */
int cntrltowhite ( void )
{
	// Scan filebuf for unwanted characters:
	for ( i = 0; (unsigned) i < filesize; i++ )
  	if ( ( filebuf [ i ] < 0x20 ) || ( filebuf [ i ] > 0x7E ) )
    {
    	filebuf [ i ] = 0x20;
      count++;
    }
  return ( WASNOERROR );
} // cntrltowhite.
/* -------------------------------------------------------------------- *
			Remove one comment:
 * --------------------------------------------------------------------- */
int remcomm ( void )
{
	// Look for "Comm:" tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( inpointer = scanbuf ( filebuf, filesize, keywords [ KWCOMM ], kwrdlen [ KWCOMM ] ) ) )
  	return ( WASNOMATCH );

	// Look for "EndComm":
  // (error if not found else process comment)
	if ( WASNOTFOUND == ( outpointer = scanbuf ( filebuf, filesize, keywords [ KWENDCOMM ], kwrdlen [ KWENDCOMM ] ) ) )
	{
  	fprintf ( stderr, "\n\n ******** ERROR *********\n Comm: tags do not match!\n ************************\n" );
  	return ( WASERROR );
  }
	else
  {
		// Look for nested comments:
    // (error if found coz it's not allowed here =)
	  tptr = inpointer + kwrdlen [ 0 ];
  	ptrdiff = ( kwrdlen [ 1 ] + outpointer - tptr );
		if ( WASNOTFOUND != scanbuf ( tptr, ptrdiff, keywords [ KWCOMM ], kwrdlen [ KWCOMM ] ) )
		{
  		fprintf ( stderr, "\n\n ******* ERROR *******\n Comm: found in Comm:!\n *******************\n" );
	  	return ( WASERROR );
  	}

		// A comment was found, remove:
  	ptrdiff = ( ( kwrdlen [ KWENDCOMM ] + outpointer ) - inpointer );
 		for ( i = 0; (unsigned) i < filesize; i++ )
      inpointer [ i ] = inpointer [ i + ptrdiff ];

//    memcpy( inpointer, outpointer + kwrdlen [ KWENDCOMM ], filesize );
  } // Process comment.

  // File has shrunken by n bytes:
	filesize -= ptrdiff;

  return ( WASMATCH );
} // remcomm.
/* -------------------------------------------------------------------- *
			Remove double 'white' spaces:
 * --------------------------------------------------------------------- */
int remwhite ( void )
{
	char		line [ 1 ] [ 2 ] = 	{ "  " };

	// Look for "n\n\":
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( filebuf, filesize, line [ 0 ], 2 ) ) )
		return ( WASNOMATCH );

	// A white space was found, remove:
	for ( i = 0; (unsigned) i < filesize; i++ )
    pointer [ i ] = pointer [ i + 1 ];

  // File has shrunken by one byte:
	--filesize;

  return ( WASMATCH );
} // remwhite.
/* -------------------------------------------------------------------- *
			Search for a tree:
 * --------------------------------------------------------------------- */
int findtree ( void )
{
  printf ( "Scanning for tree," );
	// Look for "DTIFS:" tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( inpointer = scanbuf ( filebuf, filesize, keywords [ 2 ], kwrdlen [ 2 ] ) ) )
	{
    if ( 0 == current_tree )
    {
	  	fprintf ( stderr, "\n\n **** ERROR ****\n No trees found!\n ***************\n" );
  		return ( WASERROR );
    }
    else
    {
			printf ( " no more trees found," );
  		return ( WASNOERROR );
    }
  }

	// Look for "EndDTIFS":
  // (error if not found else process comment)
	if ( WASNOTFOUND == ( outpointer = scanbuf ( filebuf, filesize, keywords [ 3 ], kwrdlen [ 3 ] ) ) )
	{
  	fprintf ( stderr, "\n\n ************ ERROR ************\n DTIFS: whith no EndDTIFS match!\n *******************************\n" );
 		return ( WASERROR );
  }
	else
  {
		// Look for nested trees:
    // (error if found)
	  tptr = inpointer + kwrdlen [ 2 ];
  	ptrdiff = kwrdlen [ 3 ] + outpointer - tptr;
		if ( WASNOTFOUND != scanbuf ( tptr, ptrdiff, keywords [ 2 ], kwrdlen [ 2 ] ) )
		{
  		fprintf ( stderr, "\n\n ******** ERROR ********\n DTIFS: found in DTIFS:!\n ***********************\n" );
	  	return ( WASERROR );
  	}
  }

	// If the 'DTIFS:' tag are not the first chars
  // in buffer, then move tree to this position:
  if ( 0 != ( ptrdiff = inpointer - filebuf ) )
		for ( i = 0; (unsigned) i < ( filesize - ptrdiff ); i++ )
   		filebuf [ i ] = filebuf [ i + ptrdiff ];
  filesize -= ptrdiff;
  inpointer -= ptrdiff;
  outpointer -= ptrdiff;

	// A tree was found:
	printf ( " found a tree" );
  treestart = inpointer;
  treesize = kwrdlen [ KWENDDTIFS ] + outpointer - treestart;
	return ( WASMATCH );
} // findtree.
/* -------------------------------------------------------------------- *
			Compile tree data:
 * --------------------------------------------------------------------- */
int compiletree ( void )
{
	// Tree header:
	if ( WASERROR == findname ( ) )
  	return ( WASERROR );
	printf ( ", compiling" );
	if ( WASERROR == findbranches ( ) )
  	return ( WASERROR );
	if ( WASERROR == findflags ( ) )
  	return ( WASERROR );
	if ( WASERROR == findsizes ( ) )
  	return ( WASERROR );

  // Branches:
  for ( j = 0 ; j < tree [ current_tree ].branches; j++ )
  {
		if ( WASERROR == ( result = findbranch ( Branchkeyword [ j ] ) ) )
  		return ( WASERROR );
		if ( result == WASNOMATCH )
    {
  		fprintf ( stderr, "\n\n ************* ERROR *************\n More 'Branches' than definitions!\n *********************************\n" );
			return ( WASERROR );
    }

		// Set pointer to branch-block:
    blockstart = inpointer;
		// Size of branch-block:
		blocksize = kwrdlen [ KWENDSTEM ] + outpointer - inpointer;

		// Set all branch-heights to 1.0 if flag is not true,
    // else look for heights: (stem is always at 'top of stub')
		if ( ! j )
    	tempval = 1.0f;
    else
    {
			// Set all the rest to 0.5 even if the flag is off
      // (This option is switchable in the DTIFS renderer,
      //  it's more fun to hit the button if there is a height
      //  to use in the setup)
	   	tempval = 0.5f;
			if ( ( tree [ current_tree ].usehig ) )
			{
    		if ( WASERROR == findheight ( ) )
      		return ( WASERROR );
    	}
    }
		heightswitcher ( j, tempval );

		// Set all branch-scales to "Global scale" if flag is true,
    // else look for scales: (First for stem and then branch(j))
		if ( ( j ) && ( tree [ current_tree ].glblscl ) )
    {
			// In the first leap (0 == j) the program vill not go here -
      // but set this value, (Scale is a must parameter for 'Stem')
			tempval = tree [ current_tree ].scale0;
    }
    else
    {
			if ( WASERROR == findscale ( ) )
  			return ( WASERROR );
    }
    scaleswitcher ( j, tempval );

		// Look for lean angle:
    // (All branches must have one such)
    if ( WASERROR == findlean ( ) )
    	return ( WASERROR );
    leanswitcher ( j, tempval );

		// Look for rotation angle:
    // (All branches must have one such)
    if ( WASERROR == findrotate ( ) )
    	return ( WASERROR );
    rotateswitcher ( j, tempval );

		// Set all branch-twist to zero degrees if flag is not true,
    // else look for twists: (First for stem and then branch(j))
		if ( true == tree [ current_tree ].usetwst )
    {
			if ( WASERROR == findtwist ( ) )
  			return ( WASERROR );
    }
    else
    {
    	tempval = 0.0f;
    }
		twistswitcher ( j, tempval );

  }

	return ( WASNOERROR );
} // compiletree.
/* -------------------------------------------------------------------- *
			Look for namne:
 * --------------------------------------------------------------------- */
int findname ( void )
{
	// Look for "Name = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( filebuf, treesize, keywords [ KWNAME ], kwrdlen [ KWNAME ] ) ) )
  {
  	fprintf ( stderr, "\n\n **** ERROR *****\n No 'Name' found!\n ****************\n" );
    return ( WASERROR );
  }

  // Extract the name:
  pointer += kwrdlen [ KWNAME ] + 1;
  i = 0;
	while ( ( '"' != pointer [ i ] ) && ( i < 15 ) )
  	tree [ current_tree ].name [ i ] = pointer [ i++ ];

  // End of string must be a null byte:
  tree [ current_tree ].name [ i ]= (char) 0x00;
  // Assure last position in string is always null:
  tree [ current_tree ].name [ 15 ] = (char) 0x00;

	// Print the namne:
	printf ( " named: '%s'", tree [ current_tree ].name );

	return ( WASNOERROR );
} // findname.
/* -------------------------------------------------------------------- *
			Look for number of branches in tree:
 * --------------------------------------------------------------------- */
int findbranches ( void )
{
	// Look for "Branches = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( filebuf, treesize, keywords [ KWBRANCH ], kwrdlen [ KWBRANCH ] ) ) )
  {
  	fprintf ( stderr, "\n\n ******** ERROR ********\n Missing 'Branches' tag!\n ***********************\n" );
    return ( WASERROR );
  }

  // Extract number of branches:
  pointer += kwrdlen [ KWBRANCH ];
  tree [ current_tree ].branches = ( short int ) atoi ( pointer );

	return ( WASNOERROR );
} // findbranches.
/* -------------------------------------------------------------------- *
			Look for switches flags:
 * --------------------------------------------------------------------- */
int findflags ( void )
{
  const char	flagerror [ 4 ] [ 128 ] = { "\n\n ********** ERROR *********\n Missing 'Use heights' tag!\n **************************\n",\
 																					"\n\n *********** ERROR *********\n Missing 'Global scale' tag!\n ***************************\n",\
																					"\n\n ************ ERROR ***********\n Missing 'Scale by height' tag!\n ******************************\n",\
                            							"\n\n ********* ERROR ********\n Missing 'Use twist' tag!\n ************************\n" };

  // Look for flags:
  // (exit if not found)
  for ( j = 0; j < 4; j++ )
  {
		if ( WASNOTFOUND == ( pointer = scanbuf ( filebuf, treesize, keywords [ 6 + j ], kwrdlen [ 6 + j ] ) ) )
  	{
  		fprintf ( stderr, "%s", &flagerror [ j ] );
    	return ( WASERROR );
	  }
  	// Extract flag:
  	getflag ( j );
    switch ( j )
    {
			case 0:
				tree [ current_tree ].usehig = flag;
      break;
			case 1:
				tree [ current_tree ].glblscl = flag;
      break;
			case 2:
				tree [ current_tree ].sctrnsl = flag;
      break;
			case 3:
				tree [ current_tree ].usetwst = flag;
      break;
    }
  }

	return ( WASNOERROR );
} // findflags.
/* -------------------------------------------------------------------- *
			Get flag:
 * --------------------------------------------------------------------- */
void getflag ( int index )
{
  // Get flag from the position next after the tag:
	pointer += kwrdlen [ 6 + index ];
  flag = ( bool ) atoi ( pointer );
} // getflag.
/* -------------------------------------------------------------------- *
			Look for radius and length of tree:
 * --------------------------------------------------------------------- */
int findsizes ( void )
{
	// Look for "Radius = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( filebuf, treesize, keywords [ KWRADIUS ], kwrdlen [ KWRADIUS ] ) ) )
  {
  	fprintf ( stderr, "\n\n ******* ERROR *******\n Missing 'Radius' tag!\n *********************\n" );
    return ( WASERROR );
  }

  // Extract number of branches:
  pointer += kwrdlen [ KWRADIUS ];
  tree [ current_tree ].radius = (long double) atof ( pointer );

	// Look for "Height = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( filebuf, treesize, keywords [ KWHIGH ], kwrdlen [ KWHIGH ] ) ) )
  {
  	fprintf ( stderr, "\n\n ******* ERROR *******\n Missing 'Height' tag!\n *********************\n" );
    return ( WASERROR );
  }

  // Extract number of branches:
  pointer += kwrdlen [ KWHIGH ];
  tree [ current_tree ].height = (long double) atof ( pointer );

	return ( WASNOERROR );
} // findsizes.
/* -------------------------------------------------------------------- *
			Find start an stop tags for one branch:

      The index is used to distinguish the names.
      (I will most likley rewrite this tool later
       so this function is useful for all the tags.
       This is just the "Get it to work!" version)
 * --------------------------------------------------------------------- */
int findbranch ( int index )
{
	// Look for "Stem/Branchx" tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( inpointer = scanbuf ( filebuf, treesize, keywords [ index ], kwrdlen [ index ] ) ) )
  	return ( WASNOMATCH );

	// Look for "EndStem/EndBranch":
  // (error if not found)
	if ( WASNOTFOUND == ( outpointer = scanbuf ( filebuf, treesize, keywords [ index + 1 ], kwrdlen [ index + 1 ] ) ) )
	{
  	fprintf ( stderr, "\n\n ************* ERROR **************\n Stem: or Branch tags do not match!\n **********************************\n" );
  	return ( WASERROR );
  }
  return ( WASMATCH );
} // findbranch.
/* -------------------------------------------------------------------- *
			Look for height of branch:
 * --------------------------------------------------------------------- */
int findheight ( void )
{
	// Look for "Height = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( blockstart, blocksize, keywords [ KWHEIGHT ], kwrdlen [ KWHEIGHT ] ) ) )
  {
  	fprintf ( stderr, "\n\n ******* ERROR *******\n Missing 'Height' tag!\n *********************\n" );
    return ( WASERROR );
  }

  // Extract height:
  pointer += kwrdlen [ KWHEIGHT ];
  tempval = (long double) atof ( pointer );

	return ( WASNOERROR );
} // findheight.
/* -------------------------------------------------------------------- *
			Set height:
 * --------------------------------------------------------------------- */
void heightswitcher ( int index, long double value )
{
	switch ( index )
  {
		default:
  	case 0:
    	tree [ current_tree ].height0 = value;
    break;
  	case 1:
    	tree [ current_tree ].height1 = value;
    break;
    case 2:
    	tree [ current_tree ].height2 = value;
    break;
    case 3:
    	tree [ current_tree ].height3 = value;
    break;
    case 4:
    	tree [ current_tree ].height4 = value;
    break;
    case 5:
    	tree [ current_tree ].height5 = value;
    break;
    case 6:
    	tree [ current_tree ].height6 = value;
		break;
		case 7:
			tree [ current_tree ].height7 = value;
  	break;
	}
} // heightwitcher.
/* -------------------------------------------------------------------- *
			Look for scale ratio:
 * --------------------------------------------------------------------- */
int findscale ( void )
{
	// Look for "Scale = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( blockstart, blocksize, keywords [ KWSCALE ], kwrdlen [ KWSCALE ] ) ) )
  {
  	fprintf ( stderr, "\n\n ****** ERROR *******\n Missing 'Scale' tag!\n ********************\n" );
    return ( WASERROR );
  }

  // Extract scale ratio:
  pointer += kwrdlen [ KWSCALE ];
  tempval = ( long double ) atof ( pointer );

	return ( WASNOERROR );
} // findscale.
/* -------------------------------------------------------------------- *
			Set scale:
 * --------------------------------------------------------------------- */
void scaleswitcher ( int index, long double value )
{
	switch ( index )
  {
		default:
  	case 0:
    	tree [ current_tree ].scale0 = value;
    break;
  	case 1:
    	tree [ current_tree ].scale1 = value;
    break;
    case 2:
    	tree [ current_tree ].scale2 = value;
    break;
    case 3:
    	tree [ current_tree ].scale3 = value;
    break;
    case 4:
    	tree [ current_tree ].scale4 = value;
    break;
    case 5:
    	tree [ current_tree ].scale5 = value;
    break;
    case 6:
    	tree [ current_tree ].scale6 = value;
		break;
		case 7:
			tree [ current_tree ].scale7 = value;
  	break;
	}
} // scaleswitcher.
/* -------------------------------------------------------------------- *
			Look for angle to lean the branch:
 * --------------------------------------------------------------------- */
int findlean ( void )
{
	// Look for "Lean = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( blockstart, blocksize, keywords [ KWLEAN ], kwrdlen [ KWLEAN ] ) ) )
  {
  	fprintf ( stderr, "\n\n ****** ERROR ******\n Missing 'Lean' tag!\n *******************\n" );
    return ( WASERROR );
  }

  // Extract leaning angle:
  pointer += kwrdlen [ KWLEAN ];
  tempval = ( long double ) atof ( pointer );

	return ( WASNOERROR );
} // findlean.
/* -------------------------------------------------------------------- *
			Set leaning angles:
 * --------------------------------------------------------------------- */
void leanswitcher ( int index, long double value )
{
  coz = cosl ( radian * value );
  zin = sinl ( radian * value );

	switch ( index )
  {
		default:
  	case 0:
 			tree [ current_tree ].leanc0 = coz;
 			tree [ current_tree ].leans0 = zin;
    break;
  	case 1:
 			tree [ current_tree ].leanc1 = coz;
 			tree [ current_tree ].leans1 = zin;
    break;
    case 2:
 			tree [ current_tree ].leanc2 = coz;
 			tree [ current_tree ].leans2 = zin;
    break;
    case 3:
 			tree [ current_tree ].leanc3 = coz;
 			tree [ current_tree ].leans3 = zin;
    break;
    case 4:
 			tree [ current_tree ].leanc4 = coz;
 			tree [ current_tree ].leans4 = zin;
    break;
    case 5:
 			tree [ current_tree ].leanc5 = coz;
 			tree [ current_tree ].leans5 = zin;
    break;
    case 6:
 			tree [ current_tree ].leanc6 = coz;
 			tree [ current_tree ].leans6 = zin;
		break;
		case 7:
 			tree [ current_tree ].leanc7 = coz;
 			tree [ current_tree ].leans7 = zin;
  	break;
	}
} // leanswitcher.
/* -------------------------------------------------------------------- *
			Look for angle to rotate the branch:
 * --------------------------------------------------------------------- */
int findrotate ( void )
{
	// Look for "Rotate = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( blockstart, blocksize, keywords [ KWROTATE ], kwrdlen [ KWROTATE ] ) ) )
  {
  	fprintf ( stderr, "\n\n ******* ERROR *******\n Missing 'Rotate' tag!\n *********************\n" );
    return ( WASERROR );
  }

  // Extract rotation angle:
  pointer += kwrdlen [ KWROTATE ];
  tempval = ( long double ) atof ( pointer );

	return ( WASNOERROR );
} // findrotate.
/* -------------------------------------------------------------------- *
			Set rotation angles:
 * --------------------------------------------------------------------- */
void rotateswitcher ( int index, long double value )
{
  coz = cosl ( radian * value );
  zin = sinl ( radian * value );

	switch ( index )
  {
		default:
  	case 0:
 			tree [ current_tree ].rotatec0 = coz;
 			tree [ current_tree ].rotates0 = zin;
    break;
  	case 1:
 			tree [ current_tree ].rotatec1 = coz;
 			tree [ current_tree ].rotates1 = zin;
    break;
    case 2:
 			tree [ current_tree ].rotatec2 = coz;
 			tree [ current_tree ].rotates2 = zin;
    break;
    case 3:
 			tree [ current_tree ].rotatec3 = coz;
 			tree [ current_tree ].rotates3 = zin;
    break;
    case 4:
 			tree [ current_tree ].rotatec4 = coz;
 			tree [ current_tree ].rotates4 = zin;
    break;
    case 5:
 			tree [ current_tree ].rotatec5 = coz;
 			tree [ current_tree ].rotates5 = zin;
    break;
    case 6:
 			tree [ current_tree ].rotatec6 = coz;
 			tree [ current_tree ].rotates6 = zin;
		break;
		case 7:
 			tree [ current_tree ].rotatec7 = coz;
 			tree [ current_tree ].rotates7 = zin;
  	break;
	}
} // rotateswitcher.
/* -------------------------------------------------------------------- *
			Look for angle to twist the branch:
 * --------------------------------------------------------------------- */
int findtwist ( void )
{
	// Look for "Twist = " tag:
  // (exit if not found)
	if ( WASNOTFOUND == ( pointer = scanbuf ( blockstart, blocksize, keywords [ KWTWIST ], kwrdlen [ KWTWIST ] ) ) )
  {
  	fprintf ( stderr, "\n\n ****** ERROR *******\n Missing 'Twist' tag!\n ********************\n" );
    return ( WASERROR );
  }

  // Extract rotation angle:
  pointer += kwrdlen [ KWTWIST ];
  tempval = ( long double ) atof ( pointer );

	return ( WASNOERROR );
} // findtwist.
/* -------------------------------------------------------------------- *
			Set twisting angles:
 * --------------------------------------------------------------------- */
void twistswitcher ( int index, long double value )
{
  coz = cosl ( radian * value );
  zin = sinl ( radian * value );

	switch ( index )
  {
		default:
  	case 0:
 			tree [ current_tree ].twistc0 = coz;
 			tree [ current_tree ].twists0 = zin;
    break;
  	case 1:
 			tree [ current_tree ].twistc1 = coz;
 			tree [ current_tree ].twists1 = zin;
    break;
    case 2:
 			tree [ current_tree ].twistc2 = coz;
 			tree [ current_tree ].twists2 = zin;
    break;
    case 3:
 			tree [ current_tree ].twistc3 = coz;
 			tree [ current_tree ].twists3 = zin;
    break;
    case 4:
 			tree [ current_tree ].twistc4 = coz;
 			tree [ current_tree ].twists4 = zin;
    break;
    case 5:
 			tree [ current_tree ].twistc5 = coz;
 			tree [ current_tree ].twists5 = zin;
    break;
    case 6:
 			tree [ current_tree ].twistc6 = coz;
 			tree [ current_tree ].twists6 = zin;
		break;
		case 7:
 			tree [ current_tree ].twistc7 = coz;
 			tree [ current_tree ].twists7 = zin;
  	break;
	}
} // twistswitcher.
/* -------------------------------------------------------------------- *
			Remove compiled tree from list:
 * --------------------------------------------------------------------- */
int remtree ( void )
{
	if ( treesize < 14 )
  {
  	fprintf ( stderr, "\n\n *********** ERROR ************\n Can't remove less than a tree!\n ******************************\n" );
  	return ( WASERROR );
  }
	// Remove tree:
	for ( i = 0; ( unsigned ) i < ( filesize - treesize ); i++ )
   	filebuf [ i ] = filebuf [ i + treesize ];
  // File has shrunken by n bytes:
	filesize -= treesize;
	return ( WASNOERROR );
} // remtree.
/* -------------------------------------------------------------------- *
			FILL TREES:

		This function fills all the 31 trees to default settings.
 * --------------------------------------------------------------------- */
void filltree ( void )
{
  t = sqrtl ( 1.0f / 2.0f );
  for ( i = 0; i < 31; i++ )
  {
		strcpy ( tree [ i ].name, "The Dummy Tree!" );
		tree [ i ].branches = 2;
		tree [ i ].usehig = 0;
		tree [ i ].glblscl = 1;
		tree [ i ].sctrnsl = 0;
		tree [ i ].usetwst = 1;
		tree [ i ].radius = 0.01f;
 		tree [ i ].height = -0.23f;
 		tree [ i ].height0 = 1.0f;
		tree [ i ].scale0 = t;
 		tree [ i ].leanc0 = t;
 		tree [ i ].leans0 = t;
 		tree [ i ].rotatec0 = 1.0f;
 		tree [ i ].rotates0 = 0.0f;
 		tree [ i ].twistc0 = t;
 		tree [ i ].twists0 = -t;
 		tree [ i ].height1 = 0.5f;
		tree [ i ].scale1 = 0.5f;
 		tree [ i ].leanc1 = t;
 		tree [ i ].leans1 = -t;
 		tree [ i ].rotatec1 = t;
 		tree [ i ].rotates1 = t;
 		tree [ i ].twistc1 = -1.0f;
 		tree [ i ].twists1 = 0.0f;
 		tree [ i ].height2 = 0.5f;
		tree [ i ].scale2 = 0.5f;
 		tree [ i ].leanc2 = 1.0f;
 		tree [ i ].leans2 = 0.0f;
 		tree [ i ].rotatec2 = 1.0f;
 		tree [ i ].rotates2 = 0.0f;
 		tree [ i ].twistc2 = 1.0f;
 		tree [ i ].twists2 = 0.0f;
 		tree [ i ].height3 = 1.0f;
		tree [ i ].scale3 = 0.5f;
 		tree [ i ].leanc3 = 1.0f;
 		tree [ i ].leans3 = 0.0f;
 		tree [ i ].rotatec3 = 1.0f;
 		tree [ i ].rotates3 = 0.0f;
 		tree [ i ].twistc3 = 1.0f;
 		tree [ i ].twists3 = 0.0f;
 		tree [ i ].height4 = 1.0f;
		tree [ i ].scale4 = 0.5f;
 		tree [ i ].leanc4 = 1.0f;
 		tree [ i ].leans4 = 0.0f;
 		tree [ i ].rotatec4 = 1.0f;
 		tree [ i ].rotates4 = 0.0f;
 		tree [ i ].twistc4 = 1.0f;
 		tree [ i ].twists4 = 0.0f;
 		tree [ i ].height5 = 1.0f;
		tree [ i ].scale5 = 0.5f;
 		tree [ i ].leanc5 = 1.0f;
 		tree [ i ].leans5 = 0.0f;
 		tree [ i ].rotatec5 = 1.0f;
 		tree [ i ].rotates5 = 0.0f;
 		tree [ i ].twistc5 = 1.0f;
 		tree [ i ].twists5 = 0.0f;
 		tree [ i ].height6 = 1.0f;
		tree [ i ].scale6 = 0.5f;
 		tree [ i ].leanc6 = 1.0f;
 		tree [ i ].leans6 = 0.0f;
 		tree [ i ].rotatec6 = 1.0f;
 		tree [ i ].rotates6 = 0.0f;
 		tree [ i ].twistc6 = 1.0f;
 		tree [ i ].twists6 = 0.0f;
 		tree [ i ].height7 = 1.0f;
		tree [ i ].scale7 = 0.5f;
 		tree [ i ].leanc7 = 1.0f;
 		tree [ i ].leans7 = 0.0f;
 		tree [ i ].rotatec7 = 1.0f;
 		tree [ i ].rotates7 = 0.0f;
 		tree [ i ].twistc7 = 1.0f;
 		tree [ i ].twists7 = 0.0f;
  } // forlooping
} // filltree.
/* -------------------------------------------------------------------- *
			THE END!
 * --------------------------------------------------------------------- */


