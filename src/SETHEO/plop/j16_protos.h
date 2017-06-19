/* -----------------------------------------------------------------
 * Project:			  ARB
 *
 * Module:                        x
 *
 * Exported Classes:              x
 *
 * Global Functions:		  x
 *
 * Global Variables:              x
 *
 * Global Structures:             x
 *
 * Private Classes:               .
 *
 * Private Functions:             .
 *
 * Private Variables:             .
 *
 * Dependencies:		  .
 *
 * Description:                   x
 * 
 * Integration Notes:             x
 *				4/1/96		englich error messages
 *
 * -----------------------------------------------------------------
 */

/*  
 * $Header: /home/jakobi/work/setheo/pl12lop/RCS/j16_protos.h,v 1.1 1994/12/12 12:39:34 jakobi Exp jakobi $
 *
 * $Log: j16_protos.h,v $
 * Revision 1.1  1994/12/12  12:39:34  jakobi
 * Initial revision
 *
 */

/* includes */

/* private prototypes */
#define OLDVersionsnr "j16         25.Oktober 1989    Martin van der Koelen" 
#define Versionsnr "j16-5 11/23/96 (TUM)" 



#define Allquantor "forall"
#define Existenzquantor "exists"
    /* Achtung: Gross-/Kleinschreibung muss beachtet werden */ 




#define groesse1 2000
								/*
- Literale
- groesstmoegliches vorkommendes Quantorenpraefix; Alle Allbindungen
  werden ja zusammen an den Anfang gezogen. Eine Bindung hat die
  Laenge: 1(Quantor) + 2 (Space) + Variablenlaenge.
  Alle zusammen duerfen die groesse1 nicht ueberschreiten!
								*/



#define groesse1_b 4000
								/*
- groesstmoegliche Laenge einer Zeile der Ausgabedatei; d.h.,
  eines LOP - Konjunktionsgliedes
- groesste Laenge einer Zeile der Eingabedatei.
								*/



#define groesse1_c 45000
								/*
- groesste Laenge der Eingabedatei (*.pl1).
								*/



#define groesse2 400
								/*
- Groesse fuer Strings in denen einzelne Ausdruecke abgelegt werden
  (Variablen, Funktoren, sonstige Konstanten).
								*/



#define groesse3 1000
								/*
- fuer Listen in denen die Nummern aller nichtlogischen Ausdruecke
  eingetragen koennen werden koennen. Hoechstens so viele
  nichtlogische Ausdruecke duerfen also vorkommen! 	
								*/

#ifdef LANG_GERMAN
#define	GERMAN(X)	X
#else
#define GERMAN(X)
#endif

 

#define wahr 1
#define falsch 0

typedef char *liste[groesse3];
typedef int  tabelle[groesse3];     
typedef char wort[groesse2]; 
typedef char formel[groesse1];
typedef char mittelformel[groesse1_b];
typedef char langformel[groesse1_c];

struct gabel
    {
    char          *txt;
    int           ju;
    struct gabel  *links;
    struct gabel  *rechts;
    liste         substliste;
    int           *tab;    
    int           schonmal;        
    };



struct winzgabel
    {
    char          *txt;
    int           ju;
    struct gabel  *links;
    struct gabel  *rechts;
    };



struct operator

       {
    int rekla;
    int staerke;
    int ort;
    int ch_zahl;
    }; 




/****************************************************************/
/*   GLOBALE VARIABLEN:                            */

liste     	varliste;
wort    	Funksymbol, Konstsymbol;
int    		card_winz, card_gabel, card_pfeil, card_baum, card_spar,
		klauseln, fehlnum,
		optimieren, DIMENSION, SKLNF, ECHO, DEBUGGEN, NEGAT, 
		fehlspez, Funknr, Konstnr, multivar;
int		SHORTSK;
FILE    	*dat; 

/* Prototypes for functions defined in j16.c  */

void *mymalloc(size_t size);
void druckbaum(struct gabel * k);
void ddd(struct gabel * k);
void fehlermeldung(int fel);
void space_weg(mittelformel s);
void init_varliste(char * s);
void subst_init(char * s);
char * neukonstante(char *);
char * neufunktor(char *);
struct gabel * talloc(void);
struct gabel * sparalloc(void);
struct gabel * winzalloc(void);
struct operator * opalloc(void);
int zeichen(int c);
char * strmartinchr(char * s, int ca);
char * strposi(char * za, char * zb);
int allquantoren(char * kette, formel Quantor);
int verbinden(char * gesamt, mittelformel z, int * kom);
int lesen(mittelformel DATEI, langformel zeile);
void standardisieren(mittelformel s);
char * tautored_weg(mittelformel kette);
char * fuegen(struct gabel * k);
void lopisieren(mittelformel ke);
void ernten(char * kette, struct gabel * k);
int schreiben(formel DATEI, struct gabel * k);
void vergleiche(struct operator * op, int stark, int wo, int klammer, int groesse);
struct operator * zentrop(char * kette, int lang);
int rbereich(char * kette, int lang);
int lbereich(char ** kette, int * lang, int a);
struct gabel * baum(char * kette, int len);
struct gabel * astcopy(struct gabel * quelle);
struct gabel * sparcopy(struct gabel * quelle);
void negieren(char * satz);
void pfeileweg(struct gabel * knoten);
void negrein(struct gabel * k);
void tabvereinigen(struct gabel * k);
void tab_init(struct gabel * k);
void tabfuellen(struct gabel * k);
char * letztquant(char * z);
int ist_frei_in(int variablennr, struct gabel * k);
void Q_V_runter(int Qua, formel Var, struct gabel * wohin);
void nichtfrei(int vnr, struct gabel * k);
char * nextvar(char * abwo, formel vari);
void leerquant_weg(struct gabel * k);
void varbinden(struct gabel * k);
int regel1_2(struct gabel * knoten);
int regel_3(struct gabel * k);
void schon_init(struct gabel * k);
void Q_klein(struct gabel * k);
void Q_gross(struct gabel * k);
void Q_nach_innen(struct gabel * k);
void listen_erben(struct gabel * vater, struct gabel * kind);
void listen_init(struct gabel * k);
void skolemisieren(struct gabel * k);
void b_multiplizieren(struct gabel * k);
void ausmultiplizieren(struct gabel * knoten);
int kla_pruefen(char * s);
int stauchen(langformel z);
void defies(void);
int main(int argc, char ** argv);

