/*
	COMGETOPT.C
	Analyse et interprÇtation des options de la ligne de commande.

	Possible types of options:
		Single char                 (GETOPT_CHAR)
		One *or more* text strings    (GETOPT_TEXTE)
		One *or more* 32-bit integers (GETOPT_INT32)
		Double                      (GETOPT_INT32TOGL)
		Toggle (32-bit integer),    (GETOPT_CHAR)
			possibly followed by another value, text, double or int.
			(for parameter couples: one YES/NO + one additional value if YES)
		Separator                   (GETOPT_SEPARATEUR, for help display)

	Any different option names admitted, no confusion if one name
		includes another. (This features makes use of abbreviations
		impossible)

	Option explanations automatically formatted ;
		no rightmost zeroes after a decimal point ;
		correct plurals

	Functions:
	Options display with pause after each screenful,
		whatever the screen height.
	Thorough argument presence, error and type checking, with choice to
		continue or not in case of error
	File redirection (option @)
	Special maintenance option (@@), checks for duplicates.
	Possibility to ask if a variable was mentioned in the command line,
		by name or by address (function com_go_option_was_used_p).

	*/


#include <conio.h>
#include <ctype.h>
#include <graph.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"

/* comgetopt.c */
int com_go_getopt(struct _getopt_option *opts,int nbo,char *usage,int *argc,char * *argv);

void com_go_show_sorted_options(struct _getopt_option *opts,int nbo);
	int sort_opt_by_name(void const *e1,void const *e2);
	int sort_opt_by_length(void const *e1,void const *e2);
	int sort_opt_by_original_order(void const *e1,void const *e2);

void com_go_display_options(struct _getopt_option *opts,char *usage,char *path);
int com_go_option_was_used_p(char *nom,void *adresse);
int com_go_lire_redir(char *filename,char * *argv,char * *new_argv,int argc);
void com_go_verify_options ( struct _getopt_option *opts, int nbo ) ;



#ifdef FRANCAIS
char
	str_go_0[] = "Pour imprimer cette liste d'options, tapez \"%s ? > PRN\" Ö l'invite DOS.\n",
	str_go_0b[] = "Aucune autre option pour ce logiciel.",
	str_go_0c[] = "Aucune option pour ce logiciel.",

	str_go_1[] = "Il faut indiquer une valeur apräs l'option \"/%s\".\n",
	str_go_1a[] = "Les seules valeurs admises apräs l'option bascule \"/%s\" sont 0(Non) ou 1(Oui).\n",
	str_go_1b[] = "Il faut indiquer un nombre (entier) apräs l'option \"/%s\", sans espace.\n",
	str_go_1c[] = "Il faut indiquer un texte apräs l'option \"/%s\", sans espace.\n",
	str_go_1d[] = "Il faut indiquer un nombre entier, ou bien rien, apräs l'option \"/%s\".\n",
	str_go_1e[] = "Il faut indiquer un nombre (avec ou sans point dÇcimal)\n\tapräs l'option \"/%s\", sans espace.\n",
	str_go_2[] = "Option %s: nbv_or_type2 est nul ?!\n",
	str_go_3[] = "Option %s: erreur dans l'ÇnumÇration \"%s\": '%c'.\n",
	str_go_4[] = "Option inconnue: \"%s\".\n",
	str_go_4b[] = "Option interne inconnue: %d.\n",

	str_go_5[] = "Option @: fichier \"%s\" introuvable.\n",
	str_go_6a[] = "Options de %s:",
	str_go_6b[] = "[valeurs actuelles]",
	str_go_7[] = "OUI",
	str_go_8[] = "NON",
	str_go_9a[] = "%d erreur%s dans les options.\n",
	str_go_9[] = "Appuyez sur <Escape> pour finir, ou sur une autre touche pour continuer...",
	str_go_9b[] = "Appuyez sur la barre d'espace pour voir les Options, sur <Escape> pour finir,\n"
		"\tou sur une autre touche pour continuer en ignorant l'erreur..." ;
#endif

#ifdef ANGLAIS
char
	str_go_0[] = "To print this option list, type \"%s ? > PRN\" at the DOS prompt.\n",
	str_go_0b[] = "No other options available for this software.",
	str_go_0c[] = "No options available for this software.",
	str_go_1[] = "A value must be given after option \"/%s\".\n",
	str_go_1a[] = "The only allowed values after option \"/%s\" are 0 (No) or 1 (Yes).\n",
	str_go_1b[] = "An integer value must be given right after option \"/%s\".\n",
	str_go_1c[] = "A text value must be given right after option \"/%s\".\n",
	str_go_1d[] = "An integer value, or nothing at all, must be given right after option \"/%s\".\n",
	str_go_1e[] = "A number (with or without point) must be given right after option \"/%s\".\n",
	str_go_2[] = "Option %s: nbv_or_type2 is 0 ?!\n",
	str_go_3[] = "Error in enumeration \"%s\": '%c'.\n",
	str_go_4[] = "Unrecognized option: \"%s\".\n",
	str_go_4b[] = "Unrecognized internal option: %d.\n",
	str_go_5[] = "Option @: cannot find file \"%s\".\n",
	str_go_6a[] = "Options of %s:",
	str_go_6b[] = "[default values]",
	str_go_7[] = "ON",
	str_go_8[] = "OFF",
	str_go_9a[] = "%d error%s in the options.\n",
	str_go_9b[] = "Hit <Space> to see the options, <Escape> to end or any other key to continue...",
	str_go_9[] = "Hit <Escape> to end, or any other key to ignore the error & continue..." ;
#endif




char getopt_error_message[160] = "" ;

/* [ File-wide access to options list */
struct _getopt_option *ref_opts = NULL ;
int nb_ref_opts = 0 ;



int com_go_getopt ( struct _getopt_option *opts, int nbo, char *usage,
	int *argc, char *argv[] ) {
/* Entry point - interprets args according to opts */
/* Returns 0: OK, 1: erreurs, 2: help,exit */
int i, j, ok_p, k, l, nb_erreurs = 0, new_argc ;
char *new_argv[150], *p, *start_valeur_arg, *start_nom_arg, tmp_buf[40] ;

if ( !*argc ) {
	puts ( "argc=NULL ???" ) ;
	abort ( ) ;
	} ;
if ( !ref_opts ) { /* stuff to do only once */
	com_go_verify_options ( opts, nbo ) ;
	ref_opts = opts ;
	/* module-wide reference to the option struct */
	nb_ref_opts = nbo ;
	for ( j = 0 ; j < nbo ; ++j ) {
		/* Completing the option struct */
		if ( !opts[j].nom ) {
			sprintf ( tmp_buf, "GETOPT: !opts[%d].nom", j ) ;
			com_abort ( tmp_buf ) ;
			} ;
		opts[j].lg_nom = strlen ( opts[j].nom ) ;
		opts[j].used_p = 0 ;
		opts[j].index = j ;
		} ;
	} ;

for ( i = 0 ; i < *argc ; ++i ) {    /* le num. 0 contient le path !! */
	if ( argv[i][0] == '@' && argv[i][1] == '@' && argv[i][2] == 0 ) {
		/* option @@ maintenance: show sorted list,
			check for duplicate options */
		com_go_show_sorted_options ( opts, nbo ) ;
		return GETOPT_INFO ;
		} ;
	if ( argv[i][0] == '?' || argv[i][1] == '?'
			|| !strnicmp ( argv[i]+1, "help", 4 )
			|| !strnicmp ( argv[i]+1, "aide", 4 ) ) {
		/* lancement with ? or /?: see options list */
		com_go_display_options ( opts, usage, argv[0] ) ;
		return GETOPT_INFO ;
		} ;
	if ( argv[i][0] == '@' && argv[i][1] != '@' ) {
		/* file redirection: @<filename> */
		new_argc = com_go_lire_redir ( argv[i]+1, argv, new_argv, *argc ) ;
		if ( new_argc == -1 )
			return GETOPT_ERROR ;
		for ( j = 0 ; j < new_argc ; ++j )
			/* add file args to the argument list */
			argv[j] = new_argv[j] ;
		*argc = new_argc ;
		break ;
		} ;
	} ;

qsort ( opts, (size_t) nbo, sizeof ( struct _getopt_option ), sort_opt_by_length ) ;
for ( i = 1 ; i < *argc ; ++i ) {    /* # 0 contains path !! */
	if ( argv[i][0] == '/' || argv[i][0] == '-' ) {
		/* Dash or slash admitted */
		ok_p = 0 ;
		start_nom_arg = argv[i] + 1 ;
		for ( j = 0 ; j < nbo  && !ok_p ; ++j ) {
			if ( opts[j].type == GETOPT_SEPARATEUR )
				/* separator, no option contained */
				continue ;
			strncpy ( tmp_buf, start_nom_arg, opts[j].lg_nom ) ;
			tmp_buf[opts[j].lg_nom] = 0 ;
			/*com_str_vire_accents ( tmp_buf ) ;*/
			if ( !stricmp ( tmp_buf, opts[j].nom ) ) {
				start_valeur_arg = start_nom_arg + opts[j].lg_nom ;
				ok_p = 1 ;
				opts[j].used_p = 1 ;
				switch ( opts[j].type ) {
					case GETOPT_CHAR:
						if ( !*start_valeur_arg ) {
							sprintf ( getopt_error_message, str_go_1, start_nom_arg ) ;
							fputs ( getopt_error_message, stderr ) ;
							++nb_erreurs ;
							break ;
							}
						((char *)opts[j].valeur)[0] = *start_valeur_arg ;
						break ;

					case GETOPT_TEXTE:
						if ( !*start_valeur_arg ) {
							sprintf ( getopt_error_message, str_go_1c, start_nom_arg ) ;
							fputs ( getopt_error_message, stderr ) ;
							++nb_erreurs ;
							break ;
							}
						if ( opts[j].nbv_or_type2 <= 0 ) {
							sprintf ( getopt_error_message, str_go_2,
								opts[j].nom ) ;
							fputs ( getopt_error_message, stderr ) ;
							/*
							++nb_erreurs ;
							break ;
							*/
							opts[j].nbv_or_type2 = 0 ;
							} ;
						if ( opts[j].nbv_or_type2 == 1 ) {
							strcpy ( (char *) opts[j].valeur, argv[i]+opts[j].lg_nom+1 ) ;
							break ;
							} ;
						{
						/* sinon desosser et chercher les separateurs */
						char *oldp, *p2 ;

						oldp = p = start_valeur_arg ;
						for ( k = 0 ; k < opts[j].nbv_or_type2 ; ++k ) {
							p2 = ((char **) opts[j].valeur)[k] ;
							l = 0 ;
							while ( *p && (*p != ',') && (*p != '&') ) {
								++p ;
								++l ;
								} ;
							strncpy ( p2, oldp, (size_t) l ) ;
							p2[l] = 0 ;
							if ( !l )
								continue ;
							if ( !l && !*p )
								break ;
							oldp = p+1 ;
							++p ;
							} ;
						} ;
						break ;

					case GETOPT_INT32:
						p = start_valeur_arg ;
						if ( !*p || !isdigit(*p) ) {
							*start_valeur_arg = 0 ;
							sprintf ( getopt_error_message, str_go_1b, start_nom_arg ) ;
							fputs ( getopt_error_message, stderr ) ;
							++nb_erreurs ;
							break ;
							}
						if ( opts[j].nbv_or_type2 < 1 )
							opts[j].nbv_or_type2 = 1 ;
						for ( k = l = 0 ; k < opts[j].nbv_or_type2 ; ++k ) {
							((int32 *)opts[j].valeur)[k] = (int32) atol ( p ) ;
							while ( isdigit ( *p ) )
								++p ;
							if ( !*p )
								break ;
							++p ; /* separateur */
							if ( !*p )
								break ; /* on tolere que ca se termine par un separateur */
							if ( !isdigit(*p) ) {
								sprintf ( getopt_error_message, str_go_3,
									opts[j].nom, start_valeur_arg, *p ) ;
								fputs ( getopt_error_message, stderr ) ;
								++nb_erreurs ;
								break ;
								} ;
							} ;
						break ;

					case GETOPT_INT32TOGL:
						p = start_valeur_arg ;
						if ( *p ) {
							if ( (*p == '0' || *p == '1') && !p[1] ) {
								/* si valeur mentionnee l'imposer */
								*((int32 *)opts[j].valeur) = (int32) atol ( p ) ;
								break ;
								} ;
							if ( !stricmp ( com_yes_word, p ) ) {
								/* si valeur mentionnee l'imposer */
								*((int32 *)opts[j].valeur) = (int32) 1 ;
								break ;
								} ;
							if ( !stricmp ( com_no_word, p ) ) {
								/* si valeur mentionnee l'imposer */
								*((int32 *)opts[j].valeur) = (int32) 0 ;
								break ;
								} ;
							/* Read second option's value, three types: */
							if ( opts[j].nbv_or_type2 == GETOPT_INT32 ) {
								if ( !isdigit ( *p ) ) {
									*start_valeur_arg = 0 ;
									sprintf ( getopt_error_message, str_go_1d, start_nom_arg ) ;
									fputs ( getopt_error_message, stderr ) ;
									++nb_erreurs ;
									break ;
									} ;
								*((int32 *)opts[j].valeur) = 1 ;
								*((int32 *)opts[j].valeur2) = (int32) atol ( p ) ;
								break ;
								} ;
							if ( opts[j].nbv_or_type2 == GETOPT_DOUBLE ) {
								*((int32 *)opts[j].valeur) = 1 ;
								*((double *)opts[j].valeur2) = (double) atof ( start_valeur_arg ) ;
								break ;
								} ;
							if ( opts[j].nbv_or_type2 == GETOPT_TEXTE ) {
								*((int32 *)opts[j].valeur) = 1 ;
								strcpy ( (char *) opts[j].valeur2, start_valeur_arg ) ;
								break ;
								} ;
							*start_valeur_arg = 0 ;
							sprintf ( getopt_error_message, str_go_1a, start_nom_arg ) ;
							fputs ( getopt_error_message, stderr ) ;
							++nb_erreurs ;
							}
						else    {
							/* sinon basculer */
							*((int32 *)opts[j].valeur) = (int32)
								(1 - *((int32 *)opts[j].valeur)) ;
							} ;
						break ;

					case GETOPT_DOUBLE:
						p = start_valeur_arg ;
						if ( !*p || (!isdigit(*p) && *p != '.' && *p != '-') ) {
							*start_valeur_arg = 0 ;
							sprintf ( getopt_error_message, str_go_1e,
								start_nom_arg ) ;
							fputs ( getopt_error_message, stderr ) ;
							++nb_erreurs ;
							break ;
							}
						*((double *)opts[j].valeur) = (double) atof ( start_valeur_arg ) ;
						break ;

					default:
						sprintf ( getopt_error_message, str_go_4b,
							(int) opts[j].type ) ;
						fputs ( getopt_error_message, stderr ) ;
						++nb_erreurs ;
						break ;
					} ;
				argv[i][0] = 0 ;
				break ;
				} ;
			} ;
		if ( !ok_p ) {
			sprintf ( getopt_error_message, str_go_4, argv[i] ) ;
			fputs ( getopt_error_message, stderr ) ;
			++nb_erreurs ;
			/*break ;*/
			} ;
		} ;
	} ;

for ( i = 1, j = 1 ; i < *argc ; ++i ) {
	/* copy remaining, uninterpreted args (maybe data file name etc.) */
	if ( argv[i][0] ) {
		argv[j] = argv[i] ;
		++j ;
		} ;
	} ;
*argc = j ;

if ( nb_erreurs ) {
	fprintf ( stderr, str_go_9a,
		nb_erreurs, COM_PLURIEL(nb_erreurs,"s") ) ;
	fprintf ( stderr, "%s\n", str_go_9b ) ;
	while ( kbhit ( ) )
		getch ( ) ;
	j = getch ( ) ;
	while ( kbhit ( ) )
		getch ( ) ;
	if ( j == 27 )
		return GETOPT_ERROR ;
	if ( j == ' ' ) {
		qsort ( opts, (size_t) nbo, sizeof ( struct _getopt_option ),
			sort_opt_by_original_order ) ;
		com_go_display_options ( opts, usage, argv[0] ) ;
		return GETOPT_INFO ;
		} ;
	} ;

return GETOPT_OK ;
}




void com_go_show_sorted_options ( struct _getopt_option *opts, int nbo ) {
char *buf[180] ;
int i, n = 0 ;

printf ( "%d option%s:\n", nbo, COM_PLURIEL(nbo,"s") ) ;
for ( i = 0 ; i < nb_ref_opts && i < 180 ; ++i ) {
	if ( opts[i].type != GETOPT_SEPARATEUR ) {
		buf[n] = opts[i].nom ;
		++n ;
		} ;
	} ;
qsort ( buf, (size_t) n, (size_t) sizeof ( char * ), sort_opt_by_name ) ;
for ( i = 0 ; i < n ; ++i ) {
	printf ( "%s ", buf[i] ) ;
	if ( i && !stricmp ( buf[i-1], buf[i] ) )
		printf ( "dupliquÇe " ) ;
	} ;
}




void com_go_verify_options ( struct _getopt_option *opts, int nbo ) {
char *buf[180], buf2[40] ;
int i, n = 0 ;

for ( i = 0 ; i < nb_ref_opts && i < 180 ; ++i ) {
	if ( opts[i].type != GETOPT_SEPARATEUR ) {
		buf[n] = opts[i].nom ;
		++n ;
		} ;
	} ;
qsort ( buf, (size_t) n, (size_t) sizeof ( char * ), sort_opt_by_name ) ;
for ( i = 0 ; i < n ; ++i ) {
	if ( i && !stricmp ( buf[i-1], buf[i] ) ) {
		sprintf ( buf2, "%s dupliquÇe ", buf[i] ) ;
		com_abort ( buf2 ) ;
		} ;
	} ;
}




	int sort_opt_by_name ( const void *e1, const void *e2 ) {
	int n = stricmp ( * ( ( char ** ) e1 ), * ( ( char ** ) e2 ) ) ;
	return n ;
	}

	int sort_opt_by_length ( const void *e1, const void *e2 ) {
	/* mettre le + long d'abord, sinon conflit entre /+ et /++ */
	struct _getopt_option const *opt1 = e1, *opt2 = e2 ;
	return opt2->lg_nom-opt1->lg_nom ;
	}

	int sort_opt_by_original_order ( const void *e1, const void *e2 ) {
	/* remettre dans l'ordre de dÇpart avant d'afficher */
	struct _getopt_option const *opt1 = e1, *opt2 = e2 ;
	return opt1->index-opt2->index ;
	}








void com_go_display_options ( struct _getopt_option *opts, char *usage,
	char *path ) {
char buf[800], bufg[40], bufd[40], buf2[40], *p ;
/* 10 lignes maxi */
int i, j, nbbn, nbl_par_page, nbl_this_opt,
	nbl_total = 1 ;
/* 1 car qd on Çcrit 1 ligne le curseur est sur la suivante = 2 */
struct videoconfig v ;

_getvideoconfig ( &v ) ;
nbl_par_page = v.numtextrows ;

if ( usage && *usage ) {
	puts ( usage ) ;
	nbl_total += com_str_length_in_80_lines ( usage, 8 ) ;
	} ;

if ( !nb_ref_opts ) {
	puts ( strchr ( usage, '<' ) ? str_go_0b: str_go_0c ) ;
	/* if usage is "MYPROG <data file name>" there is actually an option */
	return ;
	} ;

p = strrchr ( path, '\\' ) ;
if ( p ) {
	++p ;
	}
else    {
	p = path ;
	} ;
strncpy ( bufg, p, 20 ) ;
p = strchr ( bufg, '.' ) ;
if ( p )
	*p = 0 ;
/*strcpy ( com_prog_exec_nom, bufg ) ;*/
printf ( str_go_0, bufg ) ;
++nbl_total ;
/* printf ( str_go_0, com_prog_exec_nom ) ;
] pas forcÇment renseignÇ Ö ce point, ou si stand-alone */

sprintf ( buf, str_go_6a, path ) ;
nbbn = COM_DISPLAY_WIDTH-strlen(buf)-strlen(str_go_6b)-1 ;
p = buf + strlen ( buf ) ;
while ( nbbn-- > 0 )
	*p++ = ' ' ;
*p = 0 ;
printf ( "%s%s\n", buf, str_go_6b ) ;
++nbl_total ;

for ( i = 0 ; i < nb_ref_opts ; ++i ) {
	com_str_vire_blancs_terminaux ( opts[i].explication ) ;
	*buf = 0 ;
	if ( opts[i].type == GETOPT_SEPARATEUR ) {
		strncpy ( buf, opts[i].explication, COM_DISPLAY_WIDTH ) ;
		p = buf ;
		while ( *p == '\n' )
			++p ;
		strcat ( buf, " ---------------------------------------------------------------------------" ) ;
		p[COM_DISPLAY_WIDTH-1] = '\n' ;
		p[COM_DISPLAY_WIDTH] = 0 ;
		nbl_this_opt = com_str_length_in_80_lines ( buf, 8 )-1 ;
		}
	else {
		sprintf ( bufg, "  %s: ", opts[i].nom ) ;
		nbl_this_opt = com_str_length_in_80_lines ( opts[i].explication, 8 ) ;
		switch ( opts[i].type ) {
			case GETOPT_CHAR:
				sprintf ( bufd, "[%c]", *((char *) opts[i].valeur) ) ;
				break ;

			case GETOPT_DOUBLE:
				com_format_double_with_decimals (
					(double) *((double *) opts[i].valeur), 4, buf2 ) ;
				sprintf ( bufd, "[%s]", buf2 ) ;
				break ;

			case GETOPT_INT32:
				sprintf ( bufd, "[%ld]", (long) *((int32 *) opts[i].valeur) ) ;
				break ;

			case GETOPT_INT32TOGL:
				sprintf ( bufd, "[%s]",
					(*((int32 *) opts[i].valeur) ? str_go_7: str_go_8 ) ) ;
				p = bufd + strlen ( bufd ) - 1 ;
				switch ( opts[i].nbv_or_type2 ) {
					case GETOPT_DOUBLE:
						com_format_double_with_decimals (
							(double) *((double *) opts[i].valeur2), 4, buf2 ) ;
						sprintf ( p, ",%s]", buf2 ) ;
						break ;
					case GETOPT_INT32:
						sprintf ( p, ",%ld]", (long) *((int32 *) opts[i].valeur2) ) ;
						break ;
					case GETOPT_TEXTE:
						sprintf ( p, ",\"%s\"]", (char *) opts[i].valeur2 ) ;
						break ;
					} ;
				break ;

			case GETOPT_TEXTE:
				sprintf ( bufd, "[\"%s\"]",
					( opts[i].nbv_or_type2 > 1 ? ((char **) opts[i].valeur)[0]:
						((char *) opts[i].valeur) ) ) ;
				break ;

			default:
				break ;
			} ;
		sprintf ( buf, "%s%s", bufg, opts[i].explication ) ;
		} ;
	if ( nbl_total + nbl_this_opt >= nbl_par_page ) {
		/* + 1 ligne pour pouvoir mettre le message */
		/* et + 1 autre parce qu'on est Ö la ligne Ö ce point */
		while ( kbhit ( ) )
			getch ( ) ;
		fprintf ( stderr, str_go_9 ) ;
		while ( !kbhit ( ) ) ;
		j = getch ( ) ;
		while ( kbhit ( ) )
			getch ( ) ;
		if ( j == 27 )
			return ;
		fprintf ( stderr, "\n" ) ;
		nbl_total = 0 ;
		} ;
	nbl_total += nbl_this_opt ;

	printf ( "%s", buf ) ;
	if ( opts[i].type != GETOPT_SEPARATEUR ) {
		*buf = 0 ;
		p = strrchr ( opts[i].explication, '\n' ) ;
		if ( p ) {
			++p ;
			nbbn = COM_DISPLAY_WIDTH-strlen(p)-strlen(bufd)-1 ;
			while ( *p == '\t' ) {
				++p ;
				nbbn += 1 ;
				nbbn -= 8 ;
				} ;
			}
		else    {
			p = opts[i].explication ;
			nbbn = COM_DISPLAY_WIDTH-strlen(p)-strlen(bufg)-strlen(bufd)-1 ;
			} ;
		p = buf ;
		while ( nbbn-- > 0 )
			*p++ = ' ' ;
		*p = 0 ;
		printf ( "%s%s\n", buf, bufd ) ;
		} ;
	} ;
}




int com_go_option_was_used_p ( char *nom, void *adresse ) {
int i ;

if ( !ref_opts )
	return 0 ;
for ( i = 0 ; i < nb_ref_opts ; ++i ) {
	if ( nom && *nom && !stricmp ( ref_opts[i].nom, nom ) )
		return ref_opts[i].used_p ;
	if ( adresse && ref_opts[i].valeur == adresse )
		return ref_opts[i].used_p ;
	} ;
return 0 ;
}




int com_go_lire_redir ( char *filename, char *argv[], char *new_argv[],
	int argc ) {
char *p ;
int nbnew = 0, i ;
FILE *fp = fopen ( filename, "rt" ) ;

if ( !fp ) {
	while ( kbhit ( ) )
		getch ( ) ;
	fprintf ( stderr, str_go_5, filename ) ;
	fprintf ( stderr, getopt_error_message ) ;
	fprintf ( stderr, str_go_9 ) ;
	while ( kbhit ( ) )
		getch ( ) ;
	i = getch ( ) ;
	while ( kbhit ( ) )
		getch ( ) ;
	if ( i == 27 )
		return -1 ;
	return 0 ;
	} ;
for ( i = 0 ; i < argc ; ++i ) {    /* le num. 0 contient le path !! */
	if ( argv[i][0] != '@' ) {
		new_argv[nbnew] = (char *) strdup ( argv[i] ) ;
		++nbnew ;
		} ;
	} ;
*com_big_buffer = 0 ;
while ( fgets ( com_big_buffer+strlen(com_big_buffer), 599, fp ) )
	com_str_vire_blancs_trm_et_init ( com_big_buffer ) ;
p = com_big_buffer ;
p = strtok ( p, "/" ) ;
while ( nbnew < 50 && p ) {
	new_argv[nbnew] = (char *) calloc ( strlen ( p )+2, sizeof ( char ) ) ;
	strcpy ( new_argv[nbnew]+1, p ) ;
	*new_argv[nbnew] = '/' ;
	++nbnew ;
	p = strtok ( NULL, "/" ) ;
	} ;
fclose ( fp ) ;
return nbnew ;
}
