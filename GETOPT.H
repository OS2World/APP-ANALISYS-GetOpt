/* getopt.h */


#ifdef __WATCOMC__
#define FLAT
#endif

#ifdef FLAT
typedef int int32 ;
typedef unsigned int uint32 ;
#else
typedef long int32 ;
typedef unsigned long uint32 ;
#endif




/* getopt.C --------------------------- */
#define GETOPT_CHAR -1           /* arg = 1 character */
#define GETOPT_DOUBLE -2         /* arg = 1 double */
#define GETOPT_INT32 -3          /* arg = 1 or more int32's */
#define GETOPT_INT32TOGL -4      /* toggle 1 or 0 (int32), possibly with a second value */
#define GETOPT_TEXTE -5          /* arg = 1 or more text strings */
#define GETOPT_SEPARATEUR -6     /* texte d'info, not an option */

#define GETOPT_OK 0     /*  */
#define GETOPT_ERROR 1     /*  */
#define GETOPT_INFO 2     /* /? */

struct _getopt_option {
	char *nom ;      /* char(s) that follow the slash or dash */
	int type ;     /* Option type: GETOPT_CHAR or GETOPT_TEXTE, etc. */
	void *valeur ; /* must be allocated, no checking performed */
	char *explication ; /* Options text - 1-line expalantion */
	int nbv_or_type2 ;
	/* number of values to read (GETOPT_INT32 & GETOPT_TEXTE only),
		or type of the second option (GETOPT_INT32TOGL only) */
	void *valeur2 ; /* pour bi-valeurs */

	/* Internes */
	int used_p ; /* 1 if option was used  */
	int lg_nom ; /* Option name length  */
	int index ; /* indice original pour pouvoir remettre dans l'ordre  */
	} ;

int com_go_getopt ( struct _getopt_option *opts, int nbo, char *usage,
	int *argc, char *argv[] ) ;
/* interprŠts args according to options */
/* Renvoie GETOPT_OK : OK, GETOPT_ERROR : error,
	GETOPT_INFO : help was requested, exit */

void com_go_display_options ( struct _getopt_option *opts, char *usage,
	char *path ) ;
int com_go_option_was_used_p ( char *nom, void *adresse ) ;
void com_notif_erreur ( char *txt ) ;




/* Miscellaneous auxiliary functions, in TESTOPT.C ----------------------- */
int com_str_length_in_80_lines ( char *buf, int tab_width ) ;
void com_str_vire_blancs_terminaux ( char *pnt ) ;
void com_str_vire_blancs_trm_et_init ( char *pnt ) ;
void com_abort ( char *txt ) ;
void com_format_double_with_decimals ( double d, int nbmaxdec, char *dest ) ;


/* Miscellaneous data, in TESTOPT.C, and macros ----------------------- */
extern char com_yes_word[] ;
extern char com_no_word[] ;
extern char com_big_buffer[] ;

#define COM_PLURIEL(n,s) (((n)>1)?(s):"")
#define COM_DISPLAY_WIDTH 80



