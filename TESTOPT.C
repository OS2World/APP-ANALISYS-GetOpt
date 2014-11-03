/* testopt.c */




#include <conio.h>
#include <ctype.h>
#include <graph.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"

/* Miscellaneous global variables ------------------------------------ */
char com_big_buffer[1200] ;

#ifdef FRANCAIS
char com_yes_word[] = "Yes" ;
char com_no_word[] = "No" ;
#endif

#ifdef ANGLAIS
char com_yes_word[] = "Oui" ;
char com_no_word[] = "Non" ;
#endif





/* Test variables ------------------------------------------------------ */
char g_texte[100] = "" ;
char g_texte1[100] = "" ;
char g_texte2[100] = "" ;
char *g_texte3[2] = { g_texte1, g_texte2 } ;
int32 i = 0, togl = 0, row[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
double g_double = -1.23, g_double2 = 0.00231001, togl_value2 = 0 ;


struct _getopt_option options_test[] = {
	{ "D", GETOPT_DOUBLE, (void *) &g_double,
		"Floating-point (double) argument", 1 },
	{ "T", GETOPT_TEXTE, (void *) g_texte,
		"Text argument", 1 },
	{ "T2", GETOPT_TEXTE, (void *) g_texte3,
		"2 text strings (separated with \"&\" or \",\")", 2 },

	{ "", GETOPT_SEPARATEUR, (void *) g_texte3, "\nNow a separator..." },
	{ "I", GETOPT_INT32, (void *) &i, "32-bit integer", 1 },
	{ "ROW", GETOPT_INT32, (void *) row,
		"Array of 10 32-bit integers (separated with \"&\" or \",\")", 10 },
	{ "TOGL", GETOPT_INT32TOGL, (void *) &togl,
		"32-bit integer toggle with second (double) value",
		GETOPT_DOUBLE, (void *) &togl_value2 },
	} ;

int nbo = sizeof ( options_test ) / sizeof ( *options_test ) ;




int main ( int argc, char *argv[] ) {
if ( com_go_getopt ( options_test, nbo, "GETOPT demo software.",
		&argc, argv ) != GETOPT_OK )
	return 0 ;
printf ( "/D = %2.2lf\n", g_double ) ;
printf ( "/I = %d\n", (int) i ) ;
printf ( "/T = \"%s\"\n", g_texte ) ;
printf ( "/T2 = \"%s\", \"%s\"\n", g_texte1, g_texte2 ) ;
printf ( "/ROW = %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
	(int) row[0], (int) row[1], (int) row[2], (int) row[3], (int) row[4],
	(int) row[5], (int) row[6], (int) row[7], (int) row[8], (int) row[9] ) ;
printf ( "/TOGL = %d    Value after /TOGL = %2.2lf\n",
	(int) togl, togl_value2 ) ;

if ( !com_go_option_was_used_p ( "d" , NULL )
		&& !com_go_option_was_used_p ( "i" , NULL )
		&& !com_go_option_was_used_p ( "t" , NULL )
		&& !com_go_option_was_used_p ( "t2" , NULL )
		&& !com_go_option_was_used_p ( "togl" , NULL ) ) {
	puts ( "Type GETOPT ? or GETOPT /? or GETOPT -? to see the options list" ) ;
	}
else {
	puts ( "You already know how to use the options." ) ;
	}
/* ] Artificial intelligence !:-) */
return 0 ;
}






int com_str_length_in_80_lines ( char *buf, int tab_width ) {
char *p = buf ;
int nbl = 1, x = 0 ;

while ( *p ) {
	if ( x >= 80 ) {
		++nbl ;
		x = 0 ;
		} ;
	if ( *p == '\t' )
		do {
			++x ;
			} while ( x % tab_width ) ;
	if ( *p == '\n' && *(p+1) ) {
	/* ignorer \n a la fin d'une ligne, il est implicite */
		++nbl ;
		x = 0 ;
		} ;
	++p ;
	++x ;
	} ;
if ( !nbl )
	nbl = 1 ;
return nbl ;
}


void com_str_vire_blancs_terminaux ( char *pnt ) {
/* removes trailing blanks */
int l = strlen ( pnt ) ;
char *ptr = pnt + l - 1 ;

if ( !l )
	return ;
while ( isspace ( *ptr ) )
	*ptr-- = 0 ;
}




void com_str_vire_blancs_trm_et_init ( char *pnt ) {
/* removes beginning & trailing blanks */
int i = 0, l = strlen ( pnt ) ;
char *ptr = pnt + l - 1, *ptr2 = pnt ;

if ( !l )
	return ;
while ( isspace ( *ptr ) ) {
	*ptr-- = 0 ;
	--l ;
	} ;
while ( isspace ( *pnt ) ) {
	++i ;
	++pnt ;
	} ;
if ( i )
	memmove ( ptr2, pnt, (size_t) (l-i+1) ) ;
}





void com_abort ( char *txt ) {
fprintf ( stderr, "%s\n", txt ) ;
exit ( EXIT_SUCCESS ) ;
}



void com_format_double_with_decimals ( double d, int nbmaxdec, char *dest ) {
char *p, *p1 ;

sprintf ( dest, "%.12lf", d ) ;

p1 = strchr ( dest, '.' ) ;
if ( p1 ) {
	p = dest+strlen(dest)-1 ;
	while ( *p == '0' ) {
		*p = 0 ;
		--p ;
		} ;
	if ( *p == '.' )
		*p = 0 ;
	} ;

p1 = strchr ( dest, '.' ) ;
if ( p1 ) {
	p = p1+1 ;
	while ( *p == '0' ) {
		++p ;
		} ;
	p[nbmaxdec] = 0 ;
	} ;
}


