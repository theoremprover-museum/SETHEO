/* -----------------------------------------------------------------
 * Project:			  plop
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
 *				3/26/96		english error messages
 *				4/1/96		long skolem-identifiers
 *                              960816          PJ: BUGFIX memory trouble 
 *						-neg
 *				11/7/96		bug in tautred_weg
 *	j16-4 11/7/96
 *	j16-5 11/23/96		tautored_weg + skolemisieren-bug
 *
 * -----------------------------------------------------------------
 */

/*  
 * $Header: /home/jakobi/work/setheo/pl12lop/RCS/j16.c,v 1.1 1994/12/12 12:39:34 jakobi Exp jakobi $
 *
 * $Log: j16.c,v $
 * Revision 1.1  1994/12/12  12:39:34  jakobi
 * Initial revision
 *
 */

/*
			**********
			** PLOP **
			**********

SCCS:		@(#)j16.c	1.1 22 Mar 1995


		Martin van der Koelen  1989

	bugfixes by Peter Jakobi



------------------------------------------------------------------------------
PLOP uebersetzt Formeln der Praedikatenlogik 1.Stufe in LOP-Form.
------------------------------------------------------------------------------




*********************************************************************
*********************************************************************

 
*/








#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#include <skolem.h>

Fuer Compiler, die mit Funktionsprototypen arbeiten

*/
#include "j16_protos.h"

int maxmemsize;
int currentmemsize;

/* ============================================================== */
void *mymalloc(size)
size_t size;
{
   char *p;
   int i;

   if (maxmemsize && (currentmemsize>maxmemsize)) {
	fprintf(stderr,"FATAL: memory exhausted - use -mem<size>?\n"); 
	exit(20);}
   p = (char *)malloc(size);
   if(!p) { fprintf(stderr,"FATAL: no memory available\n"); exit(20); }
   currentmemsize+=size;
   for(i=0;i<size;i++)
      p[i]=0;
   return(p);
}

/* ============================================================== */
void druckbaum(k)
struct gabel  *k;
{
if (k != NULL)
	{
	if (k->txt != NULL) printf("%s|\n",k->txt);
	druckbaum(k->links);
	druckbaum(k->rechts);
	}
}




/* ============================================================== */
void ddd(k)
struct gabel  *k;
{
printf("***************************** \n");
druckbaum(k);
}



/* ============================================================== */
void fehlermeldung (fel)
int fel;
{
if ((fel < 0) && (fel >= -300))
	{
	GERMAN(printf("ERROR   Programm wurde abgebrochen.\n"); )
	printf("ERROR   program aborted.\n"); 
	GERMAN(printf ("Ausgabedatei nicht bzw. nicht korrekt erzeugt!\n\n");)
	printf ("Output file (.pl1) not or partially generated\n");
	}
if (fel > 0)
	{
	GERMAN(printf("WARNING    Programm wurde zwar ausgefuehrt,");)
	GERMAN(printf(" eventuell aber Versehen bei Eingabe?\n\n");)
	printf("WARNING    possible input error: check your input\n");
	}
switch (fel)
{
case -125:
GERMAN(printf("-125 : Funktion Allquantoren;\n");)
GERMAN(printf("Hinter einem Quantor stand keine mit Grossbuchstaben beginnende\n");)
GERMAN(printf("Variable. Wahrscheinlich Eingabefehler!\n");)
printf("Error in function 'Allquantoren':\n");
printf("An all-quantifier was not followed by a variable");
printf(" starting with an uppercase character\n");
break;

case -127:
GERMAN(printf ("-127 : Funktion Allquantoren;\n");)
GERMAN(printf ("Hinter einer Variablenbindung (d.h. Quantor + Variable) stand\n"); )
GERMAN(printf ("ein syntaktisch unzulaessiges Zeichen.\n"); )
GERMAN(printf ("Wahrscheinlich Eingabefehler!\n");)
printf("Error in function 'Allquantoren':\n");
printf ("A quantifier expression (quantifier + variable) was followed\n"); 
printf ("by an illegal character\n"); 
break;

case -135:
GERMAN(printf ("-135 : Funktion lesen; Datei konnte nicht geoeffnet werden.\n");)
printf("Error in function 'lesen':\n");
printf ("Input file could not be opened for reading.\n");
break;
								
case -136:
GERMAN(printf("-136 : Funktion lesen; Probleme beim Schliessen der Datei.\n");)
printf("Error in function 'lesen':\n");
printf("Could not close input file.\n");
break;								
 
case -143:
GERMAN(printf("-143 : Funktion rbereich gibt negativen Wert zurueck (Aufruf in baum)\n");)
printf("Error in function 'rbereich' (internal):\n");
printf("function returns negative value\n");
break;

case -144:
GERMAN(printf("-144 : Funktion lbereich gibt negativen Wert zurueck (Aufruf in baum)\n");)
printf("Error in function 'lbereich' (internal):\n");
printf("function returns negative value\n");
break;

case -153:
GERMAN(printf ("-153 : Funktion tautored_weg;\n");)
GERMAN(printf("Schwerwiegender Fehler in der Klammerung!!!\n");)
GERMAN(printf("Eine Negation stand nicht ganz aussen\n");)
GERMAN(printf("Ausgabedatei kann nicht korrekt sein !\n");)
GERMAN(printf("*************************************.\n");)
printf("Error in function 'tautored_weg':\n");
printf("Imbalanced parentheses. Output-file corrupted\n");
break;

case -151: 
GERMAN(printf ("-151 : Funktion baum.\nEingabefehler?\n");)
printf("Error in function 'baum':\n");
printf("Syntax error in input-file\n");
break;

case -152:
GERMAN(printf ("-152 : Funktion fuegen.\nEingabefehler?\n");)
printf("Error in function 'fuegen':\n");
printf("Syntax error in input-file\n");
break; 

case -154:
GERMAN(printf("-154 : Funktion fuegen;\n");)
GERMAN(printf("Fehler beim Zusammensetzen des Baumes \n");)
GERMAN(printf("Diese Funktion soll nur einzelne Klauseln in sich zusammensetzen\n");)
GERMAN(printf("Es wurde aber noch ein 'und' gefunden -> Fehler im Programm\n");)
printf("Error in function 'fuegen' (internal):\n");
printf("Please report this error\n");
break;

case -157:
GERMAN(printf("-157 :  Funktion negrein;\n");)
GERMAN(printf("Wahrscheinlich Syntaxfehler in Eingabe:\n");)
GERMAN(printf("Fehlende oder kleingeschriebene Variable.\n");)
printf("Error in function 'negrein':\n");
printf("Syntax error: Check for missing variable or variable name starting with a lowercase letter\n");
break;

case -179:
GERMAN(printf("-179 : Funktion regel1_2;");)
GERMAN(printf("Es wurde ein Quantor ohne Variable gefunden.\n");)
GERMAN(printf("Da solche schon in 'leerquant_weg' beseitigt werden sollten,\n");)
GERMAN(printf("handelt es sich um einen Programmfehler. \n");)
printf("Error in function 'regel1_2' (internal):\n");
printf("Please report this error\n");
break; 

case -187:
GERMAN(printf("-187 : Funktion schreiben;\n");)
GERMAN(printf ("Datei kann nicht auf Platte geschrieben werden!\n");)
GERMAN(printf("Keine Schreibberechtigung ???\n");)
printf("Error in function 'schreiben':\n");
printf("Could not open file for writing.\n");
break; 
 
case -189:
GERMAN(printf ("-189 : Funktion regel1_2;\n");)
GERMAN(printf ("Noch einmal genau ueberpruefen!\n");)
printf("Error in function 'regel1_2' (internal):\n");
printf("Please report this error\n");
break;


case -191:
GERMAN(printf ("-191 : Funktion lesen\n");)
GERMAN(printf ("Ein Kommentar wurde nicht durch    */   abgeschlossen\n");)
printf("Error in function 'lesen':\n");
printf("Syntax error: comment not closed by a '*/'.\n");
break;



case -192:
GERMAN(printf ("-192 : Funktion lesen\n");)
GERMAN(printf ("Kommentarende  */  ohne vorausgehenden Anfang  /*  gefunden!\n"); )
printf("Error in function 'lesen':\n");
printf("Syntax error: closing comment '*/' found without corresponding '/*'.\n");
break;


case -193:
GERMAN(printf ("-193 : Funktion lesen\n");)
GERMAN(printf ("Unerlaubte Verschachtelung von Kommentaren!\n");)
printf("Error in function 'lesen':\n");
printf("Syntax error: Illegal netsing of comments\n");
break;

case -195:
GERMAN(printf ("-195 : Funktion lesen\n");)
GERMAN(printf("Eingabedatei ist groesser als die festgelegten %d Zeichen!\n",groesse1_c);)
GERMAN(printf("Bitte Definition von  >groesse1_c< im Quelltext aendern und \n");)
GERMAN(printf("neu compilieren.\n");)
printf("Error in function 'lesen':\n");
printf("Input-file too large. Recompilation needed\n");
break;

case 159:
GERMAN(printf("+159 : Funktion leerquant_weg\n");)
GERMAN(printf("Es wurde ein Quantor ohne Variable gefunden!\n");)
GERMAN(printf ("Quantor wurde beseitigt.\n");)
/* Dieser Fall kann eigentlich nicht eintreten, weil schon vorher
   Fehler -125 das Programm abbricht!     */
printf("Error in function 'leerquant_weg' (internal):\n");
printf("Please report this error\n");
break;


case 169:
GERMAN(printf ("+169 : Funktion leerquant_weg\n");)
GERMAN(printf ("Es wurde eine leere Variablenbindung gefunden.\n");)
GERMAN(printf ("Quantor und Variable wurden beseitigt. \n");)
printf("Error in function 'leerquant_weg':\n");
printf("Empty variable binding found (ignored).\n");
break;

case -175:
printf("Internal error during Skolemization: please report.\n");
break;
}
}




/* ============================================================== */
void    space_weg(s)
mittelformel s;
{
int x, y;
mittelformel t;
y = 0;
x = 0;
for (x = 0; s[x] != '\0'; x++)
    {
    if (s[x] > 32) 
                        /* Auch Zeilenvorschuebe weg*/
        {
        t[y] = s[x];
        y++;
        }
    }
t[y] = '\0';
strcpy(s,t);
}            
         

 

/* ============================================================== */
void init_varliste(s) /*nummeriert die Variablen*/
char  *s;
{
wort vari;
int  y;
char *z, *nextvar();

for (y = 0; y < groesse3; y++)
    varliste[y] = NULL;
    
z = s;
while((z != NULL) && (*z != '\0'))
   {
   while ((*z != '\0') && ((*z < 'A') || (*z > 'Z')))
           {
            
            z++;
                       }     
       
   if (*z != '\0')
    {
    z = nextvar(z,vari); 	/*
 		               	Prozedur nextvar beginnt direkt
                	 	mit z zu suchen  */    
    y = 1;  /* Es ist wichtig, die Liste erst ab 1 zu beginnen! */  
    while
    ((varliste[y] != NULL) && (strcmp(vari,varliste[y]) != 0))
        y++;
    if (varliste[y] == NULL)
        {
        varliste[y] = (char *) mymalloc(groesse2);
        strcpy(varliste[y],vari);

        }
        
    
        z = (z + strlen(vari));
        

    }
    }
}





/* ============================================================== */
void subst_init(s)
char    *s;
{
int  weiter, dernicht;
char  *z, *strposi(); 
wort w, w1, w2, w3;

Funknr = 1;     /* Man darf nicht mit 0 anfangen, sonst gibt es
 	           Probleme in der Funktion 'neufunktor'! */


strcpy(w1,"f");
strcpy(w2,"g");
strcpy(w3,"h");
strcpy(w,w1);

dernicht = wahr;
while (dernicht)
   {
   z = s;
   dernicht = falsch;
   weiter = wahr;
   while (weiter)
    {
    if ((z = strposi(z,w)) != NULL) 
        {
        z = (z+ strlen(w1));
        if((*z >= '0'  &&  *z<= '9') || (*z == '_'))
            {
            weiter = falsch;     /* Wenn gefunden und danach Ziffer */
                    /* oder U_Strich aufhoeren!   */
            dernicht = wahr;
            }
        }
    else weiter = falsch;   /* wenn nicht gefunden im String: aufhoeren */ 
        }           /* den koennen wir nehmen! */    
   if (dernicht)
    {
    if (w[0] == 'h')
        {
        strcat(w1,"f");
        strcat(w2, "g");
        strcat(w3,"h");
        strcpy(w, w1);
        }
         
    if (w[0] == 'f')   strcpy(w,w2);
    if (w[0] == 'g')   strcpy(w,w3);
    }
   }
strcpy(Funksymbol,w);



Konstnr = 1;  


strcpy(w1,"a");
strcpy(w2,"b");
strcpy(w3,"c");
strcpy(w,w1);

dernicht = wahr;
while (dernicht)
   {
   z = s;
   dernicht = falsch;
   weiter = wahr;
   while (weiter)
    {
    if ((z = strposi(z,w)) != NULL) 
        {
        z = (z+ strlen(w1));
        if((*z >= '0'  &&  *z<= '9') || (*z == '_'))
            {
            weiter = falsch;     /* Wenn gefunden und danach Ziffer */
                    /* oder U_Strich aufhoeren!   */
            dernicht = wahr;
            }
        }
    else weiter = falsch;   /* wenn nicht gefunden im String: aufhoeren */ 
        }           /* den koennen wir nehmen! */    
   if (dernicht)
    {
    if (w[0] == 'c')
        {
        strcat(w1,"a");
        strcat(w2, "b");
        strcat(w3,"c");
        strcpy(w, w1);
        }
         
    if (w[0] == 'a')   strcpy(w,w2);
    if (w[0] == 'b')   strcpy(w,w3);
    }
   }
strcat(w,"_");
strcpy(Konstsymbol,w);


}






/* ============================================================== */
char *neukonstante(varname)
char *varname;
{
static	char	z[groesse1];

if (SHORTSK){
	 sprintf(z,"%s_%d", Konstsymbol, Konstnr++);
	}
else {
	 sprintf(z,"%s_%s_%d", Konstsymbol, varname, Konstnr++);
	}


return (z);
}



/* ============================================================== */
char *neufunktor(varname)
char *varname;
{
static	char	z[groesse1];

if (SHORTSK){
	sprintf(z,"%s_%d", Funksymbol, Funknr++);
	}
else {
	sprintf(z,"%s_%s_%d", Funksymbol, varname, Funknr++);
	}

return (z);
}


/* ============================================================== */
struct gabel *talloc()
{
struct gabel  *k;   
    card_gabel++;	
    k = (struct gabel *) mymalloc(sizeof(struct gabel));
    k->txt = (char *) mymalloc(groesse1);
return(k);
}




/* ============================================================== */
struct gabel *sparalloc()
{
struct gabel  *k;   
    card_spar++;	
    k = (struct gabel *) mymalloc(sizeof(struct winzgabel));
    k->txt = (char *) mymalloc (groesse1);
return(k);
}


/* ============================================================== */
struct gabel *winzalloc()
{
 
struct gabel  *k;   
    card_winz++;	
    k = (struct gabel *) mymalloc(sizeof(struct winzgabel));
    k->txt = NULL;
return(k);
}

      

/* ============================================================== */
struct operator   *opalloc()
{ 
return ((struct operator *) mymalloc(sizeof(struct operator)));
} 



/* ============================================================== */
int   zeichen(c)
char c;
{
if
((c > 96 && c < 123) || (c > 64 && c < 91) || (c > 47 && c < 58) || (c == 95))
/*      LC-alpha             UC-alpha            digit                _ */
return (1); else return(0);
}





/* ============================================================== */
char *strmartinchr(s, ca)
char *s, ca;
{
char  *t;
t = s;

while(*t != '\0')
    {
    if (ca == *t) return(t);
    t++;
    }
return(NULL);
}


/* ============================================================== */
char *strposi (za, zb)
char *za, *zb;
{
int x;
char  *z1 , *z, *z2;
z1 = za;
while ((z = strmartinchr(z1,*zb)) != NULL)
    {

    z1 = z;
    z2 = zb;
    x = 0;
    while((*z == *z2)  &&   x < strlen(zb))
        {
        x++;
        z++;
        z2++;
        }
        
    if (x == strlen(zb)) return(z1);
    z1++;
    }

return (NULL);
}
     





/* ============================================================== */
int allquantoren(kette, Quantor)
char         *kette; 
formel       Quantor ;
{
langformel s1;
char *z1, *z2, *z3, ca, quant;
int len;

if (DEBUGGEN) printf("%s\n",Quantor);
len = strlen(kette);
while((z1 = strposi(kette,Quantor)) != NULL)  
    {
    if (strcmp(Quantor, Allquantor) == 0 ) quant = '['; else quant = ']';
    z2 = (z1 + strlen(Quantor));
    while (*z2 == 32) z2++;   /* Leerzeichen uebergehen */
    if ((*z2 < 'A') || (*z2 > 'Z')) return(-125); 
                                 /* Variablen muessen gross anfangen */
    z2++;
    while (zeichen(*z2))  z2++;
                     /* Danach sind Buchst., Ziffern u. U_Strich
                   erlaubt */
    while (*z2 == 32) z2++;   /* Leerzeichen uebergehen */
    ca = *z2;
    if (ca >96 && ca < 123) ca = 97;
                    
                     
         *z1 = quant; 
     z1++;  /*ersten Quantor ersetzen   */
         len = 1;
     while (len < strlen(Quantor))
    {
     *z1 = 32; z1++; 
    len++;
    }
    switch (ca) {
           case ',':      {
                          *z2 = '\0';
                         
                             strcpy(s1,kette);
                            
                          z3 = strcat(s1,Quantor);
                         
                          z3 = strcat(z3,z2+1);
                         
                          strcpy(kette,z3);                  
                                                   }
           case '~':
           case '(':
           case '[':
           case ']':
           case 'a':  break;  /* falls der naechste Buchst. klein ist */
           default    : printf("Illegal character: %s\n",z2); return(-127); 
                  /* Sonst darf hier nichts stehen */
/* Achtung: Falls spaeter noch einmal Sonderzeichen oder mathematische
Ausdruecke verarbeitet werden sollen, muessen diese in der case- Unterscheidung
beruecksichtigt werden */

                  } 
    }
return(0);
}


/* ============================================================== */
int verbinden(gesamt, z, kom)
char          *gesamt;
mittelformel  z;
int           *kom;
{
char *zgr2, *zgr;
mittelformel hilf;
int m;
m = 0;
zgr = z;
while (*zgr != '\0')
	{
	if  (*kom == 0)
	    {
	    if ((*zgr != '/') && (*zgr != '*') )
	    	{
	  	hilf[m] = *zgr;
	    	m++;
	    	zgr++;
	    	}
	     else 
		{
		zgr2 = (zgr + 1);
		if ((*zgr == '*') && (*zgr2 == '/')) return(-1); 
		if ((*zgr == '/') && (*zgr2 == '*')) { (*kom)++;  zgr++;} 
	   	if (*kom == 0) 
			{
			hilf[m] = *zgr;
			m++;
			}
		zgr++;
		}
	    }
	else        /* *kom ist 1 , wir befinden uns also in einem Kommentar */
	    {
	    if (( *zgr == '/' ) || (*zgr == '*'))
		{	
		zgr2 = (zgr + 1);
		if ((*zgr == '/') && (*zgr2 == '*')) return(2);
		if ((*zgr == '*') && (*zgr2 == '/')) (*kom)--;
		zgr++;
		}
	     zgr++;
	     }


	}
hilf[m] = '\0';
strcat (gesamt,hilf);
return(0);

}




	
/* ============================================================== */
int lesen (DATEI,zeile)
mittelformel     DATEI;
langformel      zeile;
{
mittelformel 	temps;
int	x, dat_len, KETTEN, kommentar, verschachtelt;

KETTEN = 0;
dat_len = 2;
x = 0; 
zeile[x] = '\0';
strcat(DATEI,".pl1");
if ((dat = fopen(DATEI,"r")) == NULL)
   {
   DATEI[strlen(DATEI)-4] = '\0'; 
   while ((DATEI[0] != '!') 
	&& ((dat = fopen(DATEI,"r")) == NULL)
	&& ((dat = fopen(strcat(DATEI,".pl1"),"r")) == NULL))
	{									
	GERMAN(printf("Datei %s nicht gefunden!\n",DATEI);)
	GERMAN(printf("\n !  bricht ab; \nsonst bitte Dateinamen eingeben:\n> ");)
	GERMAN(scanf("%s",DATEI); )
	printf("File %s not found!\n",DATEI);
	printf("\n Enter file name (! aborts):\n> ");
	scanf("%s",DATEI); 
	}
    }
if ((dat == NULL) || (DATEI[0] == '!'))  return(-135);


	verschachtelt = 0;
	kommentar = 0;
        while ((fgets(temps,groesse1_b,dat) != NULL) && (verschachtelt == 0))
		{
		KETTEN++;
		dat_len = (dat_len + strlen(temps));
		if (dat_len > groesse1_c)
			{
			fclose(dat);
			return(-195);
			}
    		
		verschachtelt = verbinden (zeile,temps,&kommentar);
		}
  
if (fclose(dat) != 0) return(-136);
if (verschachtelt == -1) return(-192);
if (verschachtelt == 2) return(-193);
if (kommentar) return(-191);
        
if (ECHO)
	{    	
	GERMAN(printf("\nDatei: %s",DATEI);)
	GERMAN(printf("    ( %d Zeile",KETTEN);)
	GERMAN(if (KETTEN != 1) printf("n");)
	GERMAN(printf(" )\n\n");	)
	GERMAN(printf("Eingabeformel:\n==============\n%s\n",zeile);)
	printf("\nFile: %s",DATEI);
	printf("    ( %d line",KETTEN);
	if (KETTEN != 1) printf("s");
	printf(" )\n\n");	
	printf("Input formula:\n==============\n%s\n",zeile);
	}
for (x = 0; zeile[x] > '\0';  x++)
	if (zeile[x] < 32) zeile[x] = 32;
		/* Steuerzeichen entfernen */
return(0);
       
}
















/* ============================================================== */
void standardisieren (s)
mittelformel   s;
{
char     *zgr1, *zgr2;
int      wiederholen;

zgr2 = s;
while ( zgr2 != NULL) 
                                    /* Doppelte Negationen beseitigen */
    {
    if ((zgr1 = strmartinchr(s,'~')) != NULL)
        {
        zgr2 = (zgr1 + 1);
        if ((zgr2 = strmartinchr(zgr2,'~')) != NULL)
            {
            *zgr1 = 32;
            *zgr2 = 32;
            }
        }
               
    else
        zgr2 = NULL;
    }           


space_weg (s);

wiederholen = 1;
while(wiederholen)
    {
    wiederholen = 0;
    zgr2 = s;
    while ( (zeichen(*zgr2) == 0) && (*zgr2 != '\0')) zgr2++;
    if ( ((zgr1 = strmartinchr(s,'(')) != NULL) && (zgr1 < zgr2))
        {
        *zgr1 = 32;
        s[strlen(s)-1] = 32;
        space_weg(s);
        wiederholen = 1;
        }
    }     
 
}



/* ============================================================== */
/* Achtung : Diese Funktion verlangt, dass keine doppelten Negationen mehr
vorkommen, sonst funktioniert sie nicht!    */
/* Diese Funktion ist eine voellig neue Version vom 3.April. Probleme gibt
es noch mit der Klammerung. So wird z.B. eine Negation nicht in jedem Fall
gefunden wenn sie ausserhalb der Klammerung steht? Dies ist alles in der 
Funktion "standardisieren" erledigt.  
*/

char *tautored_weg(kette)
mittelformel   kette;
{
char      *suchraum, *anfang, *ende,*z1, *z2;
int      negativ, y;
mittelformel    jetzt;
int 	cont;

if (DEBUGGEN){
	printf("tautored_weg(%s)\n",kette);
	}
anfang = kette;
z1 = kette;
while (wahr)
   	{
   	ende = anfang;
   	while (*ende == 32) ende++;
   	if (*ende == '\0'){
		if (DEBUGGEN){
			printf("tautored_weg: loop-return=>%s<\n",ende);
			}
		 return (ende);
		}
   	negativ = 0;	
   	if (*ende == '~')
		{
		negativ = 1;
		ende++;
		}
	y = 0;
	while
	(zeichen(*ende)||(*ende == ',') ||  (*ende == ')') || (*ende == '('))	
		{
		jetzt[y] = *ende;
		ende++;
		y++;
		} 
	if (*ende == '~')
		{
		fehlnum = -153; /* schwerer Fehler */
		ende++;
		*ende = '\0';
		if (DEBUGGEN){
			printf("tautored_weg: error-return=>%s<\n",ende);
			}
		return(ende-1);
		}
	jetzt[y] = '\0';
	while((*ende !=';') && (*ende!= '.')) ende++;
	if (*ende == '.')
		{
		if (DEBUGGEN){
			printf("tautored_weg: fullstop return >%s<\n",z1);
			}
		return (z1);
		}
	ende++;   /* ende ist jetzt das erste Zeichen hinter dem ";" */
	if (DEBUGGEN){
		printf("tautored_weg post ; ende=>%s<\n",ende);
		printf("tautored_weg post ; jetzt=>%s<\n",jetzt);
		printf("tautored_weg post ; negativ=>%d<\n",negativ);
		}
	suchraum = ende;

#ifndef JSC_BUGFIX
	/* BUG!!!
	   PJ: "jetzt" should have been renamed to an unique string not part of other substrings
	   if ((z2 = strposi(suchraum,jetzt)) != NULL)
	*/
	cont=1; z2=suchraum;
	while(z2 && *z2 && cont) {
           z2 = strposi(z2,jetzt);
	   if (z2 && *z2) {
	      if (!zeichen(*(z2+strlen(jetzt)+1))) {
                 if (z2>suchraum) { if (!zeichen(*(z2-1))) {cont=0;} }
		 else { cont=0; }
	      }
	   }
	   if (cont && z2 && *z2) {z2++;}
	}
	if (z2)
	/* end bugfix */
		{

#else
	if (((z2 = strposi(suchraum,jetzt)) != NULL) &&
		!(zeichen(*(z2+strlen(jetzt))) ||
		 *(z2+strlen(jetzt)) == '('))
			/* 2nd/3rd line bugfix by johann 11/96 */
			/* to prevent tauto reduct in case of prop */
			/* e.g.,  ~p ; paa(....).              */
		{
		if (DEBUGGEN){
			printf("tautored_weg strposi=z2=>%s<\n",z2);
			}
#endif

		while ((*z2 != ';') && (*z2 != '~')) z2--;
		if (DEBUGGEN){
			printf("tautored_weg move backward:z2=>%s<\n",z2);
			}
		if ((negativ && (*z2 != '~'))||((negativ == 0)&&(*z2 == '~')))
			{      /*Tautologie */	
			*z1 = '\0';
			if (DEBUGGEN){
				printf("tautored_weg tautol. return >%s<\n",z1);
				}
			return(z1);
			}
		else 
			{       /* Redundanz  */
				     /* "jetzt" rausstreichen ! */
			*anfang = '\0';
			z1 = strcat(z1,ende);
			while( *z1 == 32) z1++;
			if (anfang < z1) anfang = z1;
			ende = anfang;
			}
		} 			
	
	anfang = ende;
	}
if (DEBUGGEN){
	printf("tautored_weg:end: return ???\n");
	}
}
		



/* ============================================================== */
char   *fuegen(k)
struct gabel *k;
{
char  *zgr;
mittelformel kette;
if (k->ju == 1) fehlnum = -154;
if (k->ju == 0)
    {
    zgr = strcpy(kette,k->txt);
    space_weg(kette);
    if (optimieren) standardisieren(kette);
    if (kla_pruefen(kette)  < 0)
	{
	fehlnum = -152;
	GERMAN(printf("\nKlammerfehler in Teilformel gefunden: %s\n",kette);)
	printf("\nParenthesis mismatch in formula: >>%s<<\n",kette);
	}
    return(zgr);
    }
if (k->ju == 2)
    {
    zgr = strcpy(kette, fuegen(k->links));
    zgr = strcat(zgr," ; ");
    zgr = strcat(zgr,fuegen(k->rechts));
    return(zgr);
    }
return (NULL);
}




/* ============================================================== */
void lopisieren(ke)
mittelformel ke;
{
mittelformel temp, lin,rec;
char	*neg, *zgr1, *zgr3, *zgr2;
int	n;

lin[0] = '\0';
rec[0] = '\0';
if ((zgr1 = strmartinchr(ke,';')) == NULL)  zgr1 = strmartinchr(ke,'.'); 
zgr3 = ke;
while (zgr1 !=NULL)
	{
	n = 0;	
	while (zgr3 < zgr1)
	        {
		temp[n] = *zgr3;
		zgr3++;
		n++;
		}
		/* zgr3 zeigt jetzt auf ';' oder auf '.' */
 
	zgr3++;
	temp[n] = '\0';
	if ((neg = strmartinchr(temp,'~')) == NULL)
		{
		zgr2 = temp;
		if (temp[0] == 32) zgr2++;
			/* Leerstelle uebergehen! Das erste Literal einer 
			Zeile hat allerdings keine Leerstelle vorne,
			deshalb if-Abfrage!                 */
		strcat(lin,zgr2);			
		strcat(lin,"; ");
		}	
	else
		{
		*neg = 32;	
		neg = &temp[1]; /* entstandene Leerstelle sparen */
		strcat(rec,neg);
		strcat(rec,",");
		}
	if ((zgr1 = strmartinchr(zgr3,';')) == NULL)  
		zgr1 = strmartinchr(zgr3,'.'); 
	}	

if ((n = strlen(lin)) > 0) 
	lin[n-2] = '\0';	/* letztes ';' entfernen */
	
strcat(lin,"<- ");
if ((n = strlen(rec)) > 0)
	rec[n-1] = '.';
else
	{
	rec[0] = '.';
	rec[1] = '\0';
	}
zgr2 = rec;
if (rec[0] == 32) zgr2++; 
strcat(lin,zgr2);
strcpy(ke,lin);
}			








/* ============================================================== */
void ernten(kette, k)
char		 *kette;
struct gabel     *k;
{
char *zgr;
if (fehlnum >= 0)
{

if (k->ju == 1)
    {
    ernten(kette, k->links);
    ernten(kette, k->rechts);
    }
else 
    {
    zgr = strcpy(kette,fuegen(k));
    zgr = strcat(kette," .");
    if (optimieren)
	zgr = tautored_weg(kette);
    if (*zgr != '\0')
	{
	if (SKLNF == falsch) lopisieren(zgr);
	klauseln++;	
    	if (ECHO) printf("%s\n",zgr);
    	fputs(zgr,dat);
    	fputc('\n',dat);
	}
    }
}
}


/* ============================================================== */
int schreiben(DATEI,k)
formel 		 DATEI;
struct gabel     *k;
{
char 	*zgr1, *anf, *p; 
mittelformel kette;
if ((zgr1 = strposi(DATEI,".pl1")) != NULL)
	{
	zgr1++;  *zgr1 = 'l';
	zgr1++;  *zgr1 = 'o';
	zgr1++;  *zgr1 = 'p';
	}
else 
	{
	anf = DATEI;
	        	/* eventuelle Punkte in <FILENAME> durch
			   Unterstrich ersetzen!  */
	p = &DATEI[strlen(DATEI)];
	while
	((p > anf) && (*p != '.') && (*p != '/') && (*p != '\\'))
	p--;
	if (*p == '.') *p = '_';
	strcat(DATEI,".lop");
	}
dat = fopen(DATEI,"w");
if (dat != NULL)
	{
	ernten(kette,k);
	if (fclose(dat) != 0){
	  GERMAN(printf("Fehler beim Schliessen der Ausgabedatei!\n");)
	  printf("Could not close output file\n");
	  }
	return(0);
	}
else return(-187);
}














/* Fassung <neutree>    7. Maerz;  8.Maerz   
   Hier wird die Formel eingelesen und auf den Baum verteilt.
   Die Junktoren werden als Integer kodiert und in den Baumknoten
   abgelegt:  & - 1,  ; - 2,  -> - 3,  <-> - 4.(In: Knoten->ju)
   Wenn zwei gleichstarke Junktoren  parallel stehen, wird der
   vordere als enger bindend verstanden. Die Negation wird hier
   noch garnicht aufgeloest. Der Text im Baumknoten ist als vor
   dem Junktor des Knotens stehend bzw als diesem uebergeordnet anzusehen.
   Wenn knoten->ju == 0 , dann handelt es sich um eine Astspitze, d.h.,
   in knoten->txt steht nur noch eine atomare Formel (eventuell aber noch
   mit Negation-Quantoren Prefix), die aussagenlogisch nicht weiter
   zerlegbar ist.
*/





/* ============================================================== */
void vergleiche(op, stark, wo, klammer, groesse)
int stark, wo, klammer, groesse;
struct operator  *op;
{
if (klammer <= op->rekla)
        {
    if(klammer < op->rekla || stark > op->staerke)
            {
        op->rekla = klammer;
        op->staerke = stark;
        op->ort = wo;
        op->ch_zahl = groesse;
        
            }
        }
} 






/* ============================================================== */
struct operator *zentrop(kette,lang)
char   *kette; 
int    lang; 
{
int                x, rk;
struct operator  *junk, *opalloc(); 
char         ca;


junk = opalloc();
junk->rekla = 5000;
junk->staerke = 0;
rk = 0;
x = (lang - 1);
while ( x >= 0)
    {
    ca = kette[x];
    switch(ca) {
    case '(': rk--; break;
    case ')': rk++; break;
    case '&': vergleiche(junk,1,x,rk,1); break;
    case ';': vergleiche(junk,2,x,rk,1); break;
        case '>': { 
        if (kette[x-2] == '<') vergleiche(junk,4,x-2,rk,3);
           else vergleiche(junk,3,x-1,rk,2);
          } 
          break;
       
                   }
    x--;
    }
return(junk);
}






/* ============================================================== */
int    rbereich(kette,lang)   /* neue Fassung. 11.Mai */
char *kette;
int  lang;
{
int    rk, x;
rk = 1;
while (rk > 0)
    {
    x = 0;   
    rk = 0;
    while (x < lang)   /* Die Laenge ist 1 groesser als der Index des
                          letzten Zeichens!  */    
        {
        if (kette[x] == ')') rk++;
        if (kette[x] == '(') rk--;
            x++;
        }
    if (rk > 0)  lang--;
        /* Damit soll die Klammer rechts aussen entfernt werden
           weil diese selbst ja nicht mehr zum Skopus gehoert! */
    }


if (rk == 0) return (lang); 
else 
	{ 
	GERMAN(printf ("\nReturnwert von rbereich: %d\n",rk); )
	printf ("\nReturnwert von rbereich: %d\n",rk); 
	return (rk);
	} 
}





/* ============================================================== */
int lbereich (kette, lang, a)      /* auch neu, 11.Mai   */
char   **kette;
int    *lang, a;
	           /* bei der Uebergabe ist a die Klammertiefe 
        	      wenn a == 0 dann gibt es kein Prefix und der gesuchte
                      Skopus = kette;         
	              Returnwert(x) ist die Prefixlaenge, d.h
        	      Prefix ist x Zeichen lang aber das x-te Zeichen
	              gehoert schon zum l-skopus, ist dessen 0-tes.    */

{
int x, lk;
lk = 0;
x = 0;
while (lk < a)
    {
    if (**kette == '(')   lk++;
    (*kette)++;	 
    x++;
    }
*lang = ((*lang) - x);
return(x);
}




/* ============================================================== */
/* Neue Fassung: hier wird nicht mehr mit dem Typ formel
   gearbeitet. Die Funktion erwartet nun einen Zeiger und
   die Information ueber die Laenge. Dies ist noetig, weil
   weil die Abschlussnull hier nicht verwendet werden kann,
   da die Gesamtformel ueberhaupt nur einmal gespeichert ist
   und wir immer auf diesen einen Speicher zugreifen. Bei sehr
   tiefen Rekursionen ergibt sich so eine bemerkenswerte
   Ersparnis von Speicherplatz. 
   Es sind nun natuerlich auch die Funktionen lbereich und 
   rbereich zu aendern.   Datum: 11.Mai
*/ 	
struct gabel *baum(kette,len)

int 		len;
char            *kette;
{
char                *prefix, *lskopus,  *rskopus; 
struct operator     *op;
struct gabel        *knoten;
int                 x, y, z, relen, lilen, prelen;

op = zentrop(kette,len);  

knoten = talloc();

if (op->staerke > 0)
    {
    x =  op->ort + op->ch_zahl; 
    rskopus = (kette + x);
    relen = (len - x); 
    
    if ((y = rbereich(rskopus,relen)) < 0)
			{  fehlnum = -143;
			GERMAN(printf("Fehler......."); )
			printf("Error (internal)\n");
			}
	else relen = y;
    z = op->rekla;
    lskopus = kette;
    lilen = op->ort;
    if ((y = lbereich(&lskopus, &lilen, z)) < 0) fehlnum = -144;
        
    prefix = kette;
    prelen = y;

    y = 0;
    while (y < prelen)
        { 
        if(prefix[y]=='(') knoten->txt[y] = 32;  /* Leerzeichen */
                                              /* ueberfluessige Klammern weg */
        else  knoten->txt[y] = prefix[y]; 
        if(prefix[y]==')') fehlnum = -151;
        y++;
        }
   
    knoten->txt[y] = '\0';
    knoten->links = baum(lskopus,lilen);
    knoten->rechts = baum(rskopus,relen);
    }
else
    {
    y = 0;
    while (y < len)	
	{
	knoten->txt[y] = kette[y];
	y++;
	}
    knoten->txt[y] = '\0';
    knoten->links = NULL;
    knoten->rechts = NULL;
    }

if (kla_pruefen(knoten->txt) < 0)
	{
	GERMAN(printf("Klammerfehler in Teilformel gefunden: %s\n",knoten->txt);)
	printf("Parenthesis imbalance in formula: >>%s<<\n",knoten->txt);
	fehlnum = -151;
	if (fehlspez > 0){
		GERMAN(printf("rechte Klammer zu frueh an der %d.ten Stelle\n",fehlspez);)
		printf("closing parenthsis too early (position %d).\n",fehlspez);
		}
	else {
		GERMAN(printf("Am Schluss der Teilformeln fehlen rechte Klammern!\n"); )
		printf("Missing closing parentheses\n"); 
		}
	}
knoten->ju = op->staerke;

return(knoten);
}











/* ============================================================== */
struct gabel  *astcopy(quelle)
/* Achtung! Diese Funktion kopiert keine Tabellen etc.!
   Dies ist voraussichtlich nicht noetig, weil sie nur fuer
   - die Aufloesung des Bikonditionals gebraucht wird, was vor
   der Einrichtung der Tabellen geschieht.
   - zum Ausmultiplizieren, was nach der letzten Benutzung der 
   Tabellen geschieht.
*/ 
struct gabel  * quelle;
{
struct gabel  *ziel;

if (quelle != NULL)
   {
   ziel = talloc();
   strcpy(ziel->txt, quelle->txt);
   ziel->ju  = quelle->ju;
   ziel->links = astcopy(quelle->links);
   ziel->rechts = astcopy(quelle->rechts);
   }
else ziel = NULL;
return(ziel);
}   


/* ============================================================== */
struct gabel  *sparcopy(quelle)
struct gabel  * quelle;
{
struct gabel  *ziel;

if (quelle != NULL)
  {
  if ( quelle->ju > 0)
   	{
   	ziel = winzalloc();
   	ziel->txt = NULL;
   	}
  else 
	{
	ziel = sparalloc();
	strcpy(ziel->txt,quelle->txt);
	}
   ziel->ju  = quelle->ju;
   ziel->links = sparcopy(quelle->links);
   ziel->rechts = sparcopy(quelle->rechts);
   }
else ziel = NULL;
return(ziel);
}   


void negieren(satz)
char  *satz;
{
char   *z1, *z2;
int   x;
x = strlen(satz);
z1 = (satz + x);
z2 = (z1 +1);
while (z1 >= satz)
    	{
    	*z2 = *z1;
    	z1--;
    	z2--;	 
	}
*z2 = '~';
}





/* ============================================================== */
/* Diese Funktion soll die Pfeiljunktoren -> und <-> aufloesen.
   Fassung vom 13.Maerz 89
*/ 
void pfeileweg(knoten)
struct gabel  *knoten;
{
struct gabel *neul, *neur, *temp, *astcopy();
char  *z;
   
if (knoten->ju > 0)



    {
    if (knoten->ju == 4)                   /* Bikonditional : a <-> b */
        {
         neul = talloc();
        neul->ju = 1;            /* wird zu : (a & b).... */
        neul->links = knoten->links;
        neul->rechts = knoten->rechts;
        z = neul->txt;

        knoten->links = neul;
        knoten->ju = 2;            /*   ....oder.....  */
        neur = talloc();
        neur->ju = 2;            /*  ....~(a oder b)   */
        z = neur->txt;
	*z =  '~';
        z++;
	*z = '\0';
        neur->links = astcopy(neul->links);
        neur->rechts = astcopy(neul->rechts);
        knoten->rechts = neur;
        }
    if (knoten->ju == 3)        /* materiale Implikation : a -> b  */
        {
        knoten->ju = 2;        /* wird zu (~a oder b)           */
        temp = knoten->links;
        negieren(temp->txt); 
        }
    pfeileweg(knoten->links);
    pfeileweg(knoten->rechts);
    }
}
    

















/* ============================================================== */
/* Diese Funktion soll die Negationen ganz nach innen vor die Praedikate
   ziehen. Es wird vorausgesetzt, dass an Junktoren nur noch und und oder 
   vorhanden sind! Fassung vom 30.Mai */
void negrein(k)
struct gabel *k;
{
char    *zgr1, *zgr2, *zgr3, *zgrA, *zgrE, *strmartinchr();
int    fall;
struct gabel     *temp;

if (k->ju  > 0)              /* in solchen Knoten duerften also keine
                              Atomformeln bzw. Praedikate stehen !
                                                                    */
    {
    while ((zgr1 = strmartinchr(k->txt,'~')) != NULL)
        {
        zgr2 = zgr1;
        fall = 0;
        while (fall < 3)   /* Solange noch nicht-umgedrehte Quantoren
                      hinter dieser Negation stehen
                    */
           {
        zgrA = strmartinchr(zgr2,'[');
        zgrE = strmartinchr(zgr2,']');
        fall = 0;
        if (zgrA == NULL) fall = 1;
        if (zgrE == NULL) fall = 2;
        if (zgrA == zgrE) fall = 3;  /* beide NULL */
        switch (fall)
            {
        case 0 : if (zgrA < zgrE) {
                    *zgrA = ']';
                    zgr2 = zgrA+1;
                       }
                    else
                           {
                    *zgrE = '[';
                    zgr2 = zgrE+1;
                        }
                    break;
        case 1 : *zgrE = '['; zgr2 = zgrE+1;  break;
        case 2 : *zgrA = ']'; zgr2 = zgrA+1;  break;
            }
                   }
    k->ju = 2 / k->ju;      /*  und / oder   vertauschen  */
    temp = k->links;    /* Negationen in beide Operanden */
    negieren(temp->txt);
    temp = k->rechts;
    negieren(temp->txt);
    *zgr1 = 32;           /* Space fuer die Negation setzen   */
          }
    negrein(k->links);
    negrein(k->rechts);
    }
else                               /* hier gibt es nur noch Atomformeln
                      Quantoren koennen aber noch drin sein
                         */
    {
    zgr2 = k->txt;
    while ((zgr1 = strmartinchr(zgr2,'~')) != NULL)
        {

        zgr3 = (zgr1+1);
        while ((*zgr3!=91) && (*zgr3!=93) && (*zgr3!='~') && (*zgr3 != '\0'))    /* Diese Zeilen */
               zgr3++;                                                           /* wurden       */      
        if (*zgr3 == '\0')                                                       /* am 30.Mai    */
            zgr2 = zgr3;    /* 3. Fall: Keine Negation und kein Quantor mehr */  /* gendert     */
         if (*zgr3 == '~')         /* 1.Fall: noch eine Negation */
            {
            *zgr1 = 32;
            *zgr3 = 32;
            zgr2 = zgr3;
                   }            /* 2. Fall : Quantor   */
        if ((*zgr3 == ']') || (*zgr3 == '['))
            {
            if (*zgr3 == '[') *zgr3 = ']';
            else *zgr3 = '[';
                    /* Quantor umdrehen  */
            zgr3++;
            while  (*zgr3 == 32) zgr3++;
            if ((*zgr3 < 'A') || (*zgr3 > 'Z'))
                fehlnum = -157;
                /* Var fangen gross an               Fehler!!!!!!!*/
            zgr3++;
            while( zeichen(*zgr3)) zgr3++;
               /* Jetzt sind wir hinter der Variablen
                  und muessen die Negation hinterziehen
                */
            while (zgr1 < zgr3)    /* ganzen Block eins vor */
                {
                zgr2 = (zgr1+1);
                *zgr1 = *zgr2;
                zgr1++;
                }
            zgr1--;     /* Wir stehen 1 zeichen zu weit rechts */
            *zgr1 = '~';
            zgr2 = zgr1;
            }


        }      /* while Schleife */
    }           /*  else Teil   */

}










/* ============================================================== */
/* Fassung vom  16. Maerz */
void tabvereinigen (k)     /* Uebernimmt die Listen der freien
                              Variablen beider Kinder fuer den
                              Vater     */

struct gabel   *k;
{
struct gabel   *l, *r;
int            x, y, z;

l = k->links;
r = k->rechts;
x = 0;
while (l->tab[x] >= 0)
   {
   k->tab[x] = l->tab[x];
   x++;
   }
   
y = 0;
while (r->tab[y] >= 0)
   {
   z = 0;
   while ((z < x) && (r->tab[y] != k->tab[z])) z++;
   if (z == x)    /* d.h. die momentane r-Variable wurde in
                     k nicht gefunden */
      {
      k->tab[x] = r->tab[y];
      x++;
      }
   y++;
   }
k->tab[x] = -1;           /* -1 markiert das Ende der Variablenliste */
}    
       
      
   
 
/* ============================================================== */
void tab_init(k)
struct gabel *k;
{
if (k != NULL)
    {
    k->tab = (int *) mymalloc(sizeof(tabelle));	
    k->tab[0] = -1;
    tab_init(k->links);
    tab_init(k->rechts);
    }
}





/* ============================================================== */
/* Die Funtion tabfuellen soll fuer jede Teilformel eine Liste
   mit den Nummern der dort freien Variablen anlegen! Unter
   Teilformeln fallen auch die schon junktorenlogisch aus	
   andern Teilformeln zusammengesetzten ! */	 
void tabfuellen (k)
struct gabel       *k;
{
int         x, y;
void         tabvereinigen();
char        *zgr1, *zgr2, *zgr3;

if (k->ju > 0)
    {
    tabfuellen(k->links);
    tabfuellen(k->rechts);
                                 /* Zuerst die Tabelle der freien Variablen
                                    in den Astspitzen anlegen
                                 */

    tabvereinigen(k);
                                 /* Dann die Tabellen der Kinder fuer den
                                 Vater uebernehmen
                                 */


    x = 0;
    while ((y = k->tab[x]) > -1)
        { 
/* if (DEBUGGEN) printf("schleife1\n"); */
        if ((k->tab[x] != 0) && (strposi(k->txt, varliste[y]) != NULL))
            k->tab[x] = 0;
        x++;
        }
                                 /* ... und schliesslich die wieder aus-
                                 streichen, die im Vater durch Quantoren
                                 gebunden sind. Neue (freie) Variablen
                                 koennen hier nicht mehr dazu kommen,
                                 weil sich nur auf der untersten Ebene
                                 echte Atomformeln befinden. Auf jeder
                                 hoeheren Ebene stehen im knoten->txt
                                 nur Quantorenpraefixe! Wir koennen also
                schon streichen, wenn wir bloss die Variable
                finden und brauchen garnicht mehr nach dem 
                bindenden Quantor zu schauen.
                                 */

    }
else
    {
    x = 1;
    while (varliste[x] != NULL)      /* varliste muss natuerlich schon
                                     vorher mit allen vorkommenden 
                                     Variablen gefuellt sein.
                                     Dies geschieht am besten in der 
                                     Funktion Allquantoren.
                                     Die 0te Stelle soll NICHT belegt
                                     werden, da mit 0 spaeter die Streichungen 
                                     erfolgen sollen!
                                     Deshalb beginnt hier auch die Zaehlung
                                     mit x = 1.
                                   */    
        {

if (DEBUGGEN) printf("schleife2\n");
if (DEBUGGEN) printf("varliste[x]: %s , x = %d\n",varliste[x],x);

        if ((zgr1 = strposi(k->txt, varliste[x])) != NULL)
            {
            zgr2  = zgr1;
            zgr2--;
            zgr3  = k->txt;
            while ((zgr2  > zgr3) && (*zgr2 == 32))
                zgr2--;
                
            if ((*zgr2 != '[') && (*zgr2 != ']'))
                {
                                      /* Falls in den Astspitzen eine
                                      Variable steht, ist diese in dieser
                                      betreffenden  Teilformel gebunden,
                                      gdw direkt vor ihrem ersten Vorkommen
                                      ein Quantor steht!
                                      */
                y = 0;
                while (k->tab[y] > -1)
                    y++; /* letzte Stelle in tab suchen */
                k->tab[y] =  x; /* x=Nr.der Var als frei
                            eintragen */
                y++;
                k->tab[y] = -1;    
                if ( strcmp(varliste[x],"Z") == 0)
                             {
                             /* getc(stdin); */
                             }
                }
            }
        x++;    /* naechste Variable pruefen */
        }
    }
if (DEBUGGEN) 
  {
  printf("\n(fkt:tabfuel) Text: %s\n freie Variablen: ",k->txt);
  y = 0;
  while ((x = k->tab[y]) > -1)
	{
	printf("%d. %s    ",y,varliste[x]);
	y++;
	}
  } 
}



/* ============================================================== */
char *letztquant(z)
char  *z;
{
char *zgr1, *zgr2, *zgrE, *zgrA;
zgr1 = z;
zgr2 = NULL;
while((zgrE = strmartinchr(zgr1,']')) != NULL)
   {
   zgr2 = zgrE;
   zgr1 = zgr2+1;
   }
zgrE = zgr2;  
   
zgr2 = NULL;
while((zgrA = strmartinchr(zgr1,'[')) != NULL)
   {
   zgr2 = zgrA;
   zgr1 = zgr2+1;
   }
zgrA = zgr2;

if(zgrA == NULL) return (zgrE);
if(zgrE == NULL) return (zgrA);
if (zgrA > zgrE) return (zgrA); else return (zgrE);
}




/* ============================================================== */
int ist_frei_in (variablennr, k)
int            variablennr;
struct gabel   *k;
{
int      x;
if (k == NULL) return(0);   
x = 0;
while(k->tab[x] >= 0)
      {        
      if (k->tab[x] == variablennr) return (1);
      x++;
      } 
return (0);
}



/* ============================================================== */
void  Q_V_runter(Qua, Var, wohin)
char           Qua;
formel         Var;
struct gabel   *wohin;  
{ 
formel         temp;
char           *z;
   temp[0] = Qua;
   temp[1] = '\0';
   z = strcat(temp, Var);                                             
   z = strcat(z," "); 
   z = strcat(z, wohin->txt);
   strcpy(wohin->txt,z);
}



/* ============================================================== */
void nichtfrei(vnr,k)
int            vnr;
struct gabel   *k;         
{
int            x;

      x = 0;
      while((k->tab[x] != vnr) && (k->tab[x] >= 0)) x++;
      k->tab[x] = 0;
}      





/* ============================================================== */
char  *nextvar(abwo,vari)  /* Funktionsergebnis zeigt auf den Anfang der
                              Variablen innerhalb des abwo uebergeben 
                              Strings (abwo kann auch auf 1. Zeichen der
	                      gesuchten Variablen zeigen!)    
                              und in vari wird die Variable noch einmal
                              extra zurueckgegeben    */
char     *abwo;
formel   vari;
{
char     *zgr1, *zgr2;
int      x;

zgr1 = abwo;
while(((*zgr1 >90) || (*zgr1 < 65)) && (*zgr1 != '\0'))
zgr1++;
if (*zgr1 == '\0') return (NULL);
else
   {
   zgr2 = zgr1;
   x = 0;
   while (zeichen(*zgr2))
      {
      vari[x] = *zgr2;  
      zgr2++;
      x++;
      }
   vari[x] = '\0';
   }
return (zgr1);
}        




/* ============================================================== */
/* Folgende Funktion entfernt die leeren, nichtsbindenden Quantoren mit
ihren Variablen. Dies ist unbedingt notwendig, sonst wuerde die
Skolemisierung zu grosse, ueberfluessige Abhaengigkeiten der 
Existenzvariablen erzeugen */ 
 
void leerquant_weg(k)
struct gabel *k;
{
int    y, varnum;
formel    vari;
char     *zgr1, *zgr2, *zgrV;

zgr1 = k->txt;
while (*zgr1 != '\0')
  {
  while((*zgr1 != '[') && (*zgr1 != ']') && (*zgr1 != '\0')) zgr1++;
  if (*zgr1 != '\0')
    {
    zgrV = (zgr1+1);
    while(*zgrV == 32 ) zgrV++;
    if ((*zgrV < 'A') || (*zgrV > 'Z'))
          {
          fehlnum = 159;
	  *zgr1 = 32;
          }
    else
        {
            zgrV = nextvar(zgr1,vari);
          
           y = 1;
             while((strcmp(vari,varliste[y]) != 0)  && (varliste[y] != NULL))
            y++;      
            varnum = y;   
        if 
          ((ist_frei_in(varnum,k->links) + ist_frei_in(varnum,k->rechts)) < 1)
         {
         zgr2 = strposi((zgrV+1),vari);
         if (zgr2 != NULL)
            {
            zgr2--;
            while(*zgr2 == 32) zgr2--;
            if ((*zgr2 == '[') || (*zgr2 == ']')) zgr2 = NULL;
                /* pruefen ob vor dieser Variablen wieder 
                   ein Quantor steht!   */
            }
         if (zgr2 == NULL)
            {    /* Quantor und Variable streichen  */
            GERMAN(printf("Leere Variablenbindung gefunden: %c %s\n",*zgr1,vari);)
            printf("Empty variable binding found: %c %s\n",*zgr1,vari);
	    fehlnum = 169;	
            *zgr1 = 32;
            for (y = 0; y < strlen(vari); y++)
                {
                *zgrV = 32;
                zgrV++;
                }
            }         
             }            
        }  /* else */
    zgr1++;   /* Weitersuchen  */
    }    
  } /* while */    

if (k->links != NULL) leerquant_weg(k->links);
if (k->rechts != NULL) leerquant_weg(k->rechts);
}



/* ============================================================== */
/* Diese Funktion bindet die freien Variablen durch aeusseren
   Allabschluss. Hierbei brauchen wir nur den Wurzelknoten zu untersuchen,
   weil in tabfuellen schon die in den Zweigen freien Variablen an die
   Listen der Vaeter uebermittelt wurden!
*/ 
void varbinden (k)
struct gabel *k;
{
int x, y;
formel qvari;
char    *zgr;

x = 0;
while (k->tab[x] >= 0)
    {
    if ((y = k->tab[x]) >= 1)
        {
        k->tab[x] = 0;    /* kann aus der Tabelle der freien 
        	                 gestrichen werden!        */ 
        strcpy(qvari,"[");
        zgr = strcat(qvari, varliste[y]);
	zgr = strcat(qvari, " ");

        zgr = strcat(qvari,k->txt);
        strcpy(k->txt,zgr);
        }
    x++;
    }
}     








/* ============================================================== */
/* Vgl. zu den Regeln 1 - 3 : Loveland, ATP, S.34 */

int regel1_2(knoten)
struct gabel   *knoten;
{
int      y, frei_re, frei_li, varnum;
formel   vari; 
char     *zgrQ;
if((zgrQ = letztquant(knoten->txt)) != NULL) 
    {
    if (nextvar(zgrQ,vari) == NULL) 
    	{
	fehlnum = -179;
	}

                                    /* Damit haben wir die letzte Variable
                                    in knoten->txt.
                                    
                                    Zunaechst ihre Nummer feststellen.
                  
                                    */
   y = 1;
   while((strcmp(vari,varliste[y]) != 0)  && (varliste[y] != NULL))
      y++;      
   varnum = y;   
                              /* Feststellen, in welchem Kind sie frei ist */
   frei_li = ist_frei_in(varnum, knoten->links);
   frei_re = ist_frei_in(varnum, knoten->rechts);

   if (frei_re > frei_li)
      {                     
      Q_V_runter (*zgrQ,vari,knoten->rechts); /* Quantor + Var ins rechte Kind
                                              hinunter ziehen und aus Liste der
                                              freien Variablen entfernen.
                                                   */
      nichtfrei(varnum, knoten->rechts);
      *zgrQ = '\0';                      /* Var + Quant aus Vater loeschen */
      return(2);    
      }                                          
                                          /* In die Liste der freien
                                             Variablen war sie dort
                                             sowieso nicht eingetragen.
    Nun dasselbe fuer links */
   
   if (frei_re < frei_li)
      {                
      Q_V_runter (*zgrQ,vari,knoten->links);
      nichtfrei(varnum, knoten->links);
      *zgrQ = '\0';
      return(1);    
      }
      

   if (frei_re == frei_li)        
                                 /* Jetzt Regel zwei aus ATP S.34*/
      {
      if (frei_re == falsch) 
                                  /* Eigentlich kann das nicht passieren.
                Es sollte ja gerade durch leerquant_weg
                vermieden werden! Allerdings kann es
                sich um Astspitzen handeln.  */
    {
    printf("Error (internal): Empty qunatifier found\n");
    GERMAN(printf("  Es wurde ein leerer Quantor gefunden \n");)
    GERMAN(printf( "(Vielleicht Astspitzen?? \n :@@%c@@  @@@%s@@@\n\n\n",*zgrQ,vari);)
    fehlnum = -189;	
     } 
  
      else 
         {
         if 
   (((*zgrQ == '[')&&(knoten->ju ==1)) || ((*zgrQ == ']')&&(knoten->ju == 2)))
           
         /* Allquantor darf nur ueber 'und', Ex-quant dagegen nur ber 'oder'
            distribuiert werden
         */
            {
            Q_V_runter (*zgrQ,vari,knoten->links);
            nichtfrei(varnum, knoten->links);
           
            Q_V_runter (*zgrQ,vari,knoten->rechts);
            nichtfrei(varnum, knoten->rechts);
            
            *zgrQ = '\0';
            return(3); 
            }
         }     /* else - Schleife */
      }     /* wenn in beiden Kindern frei */
   }     /* if zgrQ != NULL   */
return(0);
}






/* ============================================================== */
int regel_3(k)        /* return= 3 wenn Regel angewendet werden konnte, 
                          sonst 0 */
struct gabel   *k;
{
char      merkQ, *zgrletzt, *zgrvorletzt, *zgr1, *zgr2,*z;
formel    varl, varvl, hilf;
int       y, x,  varnuml, varnumvl;  

if 
((zgrletzt = letztquant(k->txt)) == NULL) return(0);



merkQ = *zgrletzt;
*zgrletzt = '?';
if
((zgrvorletzt = letztquant(k->txt) ) == NULL) {*zgrletzt = merkQ; return(0);}


*zgrletzt = merkQ;
if 
(*zgrvorletzt != merkQ)       /* Sind letzten beiden Quantoren ungleich? */
 	      return (0);                   /* -> Regel nicht anwendbar*/
   

if
(((merkQ == '[') && (k->ju == 1)) || ((merkQ == ']') && (k->ju == 2)))
      return (0);                   /* Regel nicht anwendbar;
                                     nur bei Kombinationen: Allq. - ODER  
                                     bzw. Exqua. - UND                 */
   /* Regel ferner nur anwendbar, wenn die Variable des letzten
   Quantors in beiden Kindern frei vorkommt, die Variable des 
   vorletzten aber mindestens in einem Kind gebunden ist! */


      

zgr2 = nextvar(zgrletzt,varl);
y = 1;
while((strcmp(varl,varliste[y]) != 0)  && (varliste[y] != NULL))      y++;      
varnuml = y; 
if 
((y = (ist_frei_in(varnuml,k->links) + ist_frei_in(varnuml,k->rechts))) < 2)
         return (0);                   /* Regel nicht anwendbar*/      
         

zgr1 = nextvar(zgrvorletzt,varvl);
y = 1;
while((strcmp(varvl,varliste[y]) != 0)  && (varliste[y] != NULL))    y++;      
varnumvl = y; 
if 
((y = (ist_frei_in(varnumvl,k->links) + ist_frei_in(varnumvl,k->rechts))) > 1)
         return (0);                   /* Regel nicht anwendbar*/
         
               
/* nun sind alle Bedingungen erfuellt, wir koennen die Regel anwenden!  */
            
z = k->txt;
x = 0;
while (z != zgr1)
               {
               hilf[x] = *z;    /* Anfang uebernehmen*/
               z++;
	       x++;	
               }

y = 0;    
while (varl[y] != '\0')
               {
               hilf[x] = varl[y];      /* hintere Variable zuerst*/
               x++;
               y++;
               }

hilf[x] = merkQ;           /* zweiten Quantor */
x++;
y = 0;    
            while (varvl[y] != '\0')
               {                       /* vorletzte Variable nach hinten */
               hilf[x] = varvl[y];
               x++;
               y++;
               }    
            
zgr1 = (zgr2 + strlen(varl));   
                                    /* Wir brauchen noch den Rest, der
                                    frueher hinter der letzten Variablen
                                    stand */
            
while ( *zgr1  != '\0')
               {
               hilf[x] = *zgr1;
               zgr1++;
               x++;
               }
            
hilf[x] = '\0';      
strcpy(k->txt,hilf);
return (3);
}              




/* naechsten 4 Funktionen sind  Neufassungen vom 7.Juni.
  Die alten Fassungen haben trotz genialer Programmierung
  nicht die geringsten Anstalten gemacht, die Quantoren nach
  innen zu ziehen.  */

/* ============================================================== */
void schon_init (k)
struct gabel    *k;
{

if (DEBUGGEN){
	printf("schon_init: ju=%d\n",k->ju);
	druckbaum(k);
	}
if (k != NULL)
    {
    if (k->ju == 0)
        k->schonmal = wahr;
    else
	{
        k->schonmal = falsch;
        schon_init(k->links);
        schon_init(k->rechts);
  	}
    }
}








/* ============================================================== */
void    Q_klein(k)
struct gabel   *k;
{
int        y;

if (k->ju > 0)
    {
    y = regel1_2(k);
    if ((y == 1) || (y == 3)) Q_klein(k->links);
    if ((y == 2) || (y == 3)) Q_klein(k->rechts);
    if (y > 0) Q_klein(k);       
    else   
       if (regel_3(k) == 3)   Q_klein(k);    
    }

} 










/* ============================================================== */
void Q_gross (k)
struct gabel  *k;
{
struct gabel *sohn, *tochter;

sohn = k->links;
tochter = k->rechts;
if  ((sohn->schonmal) && (tochter->schonmal))
    {
    Q_klein(k);
    k->schonmal = wahr;
    }
else 
    {
    if (sohn->schonmal == falsch) Q_gross(k->links);
    if (tochter->schonmal == falsch) Q_gross(k->rechts);
    }
}




/* ============================================================== */
void Q_nach_innen(k)
struct gabel     *k;
{
if (k->ju > 0)
   {
   if (DEBUGGEN) printf("Q_nach: k->ju = %d > 0\n",k->ju);
   schon_init(k);
   while(k->schonmal == falsch) Q_gross(k);
   }
}


/* Ueble Rekursion: wenn regel1_2 angewendet werden konnte, muss man
   wieder ganz unten in den Astspitzen anfangen! Hoffentlich geht das 
   durch.
   Bei der Anwendung von Regel 3 wird in den Kindern ja nichts
   veraendert, sodass man also nicht hinabsteigen muss!

Alles Neu macht der Juni. grosse Fehlerbeseitigung am 7.6.! 
*/












/* ============================================================== */
void listen_erben (vater, kind)
struct gabel    *vater, *kind;
{
int x;
x = 0;
while (vater->tab[x] > -1)
    {
    kind->tab[x] = vater->tab[x];
    x++;
    }
kind->tab[x] =    -1;
x =  0;
while (x <= groesse2)
    {
    kind->substliste[x] = vater->substliste[x];
    x++;
    }    
}


/* ============================================================== */
void listen_init(k)
struct gabel    *k;
{
int x;

k->tab[0] = -1;
x = 0;
while( x < groesse3)
    {
    k->substliste[x] = NULL;
    x++;
    }
if (k->links != NULL)
    listen_init(k->links);
if (k->rechts != NULL)
    listen_init(k->rechts);
}





/* ============================================================== */
void skolemisieren(k)

struct gabel    *k;
{ 
int          y, varnum ;
char         *zgr1, *zgr2,  *neufunktor(), *nextvar();
formel        vari, temp;

if (DEBUGGEN) {
	printf("skolemisieren: \n");
	druckbaum(k);
	printf("\n");
	}
zgr1 = k->txt;

while( *zgr1 != '\0')
    {
    if (*zgr1 == '[')
            /* Wenn wir auf einen Allquantor stossen:
            Wir nehmen die zugehoerige Variable und tragen ihre
            Nummer an das Ende von Knoten->tab ein. 
            (Spaeter weden diese Listen an die Kinder vererbt,
            sodass man in jedem Knoten die Information ueber
            die Allvariablen hat, von denen man abhaengt)

            Ausserdem koennen Quantor und zugehoerige 
            Variable geloescht werden.
            */         
        {
        *zgr1 = 32;      /* Quantor loeschen  */
        zgr1 = nextvar (zgr1,vari);
        y = 1; 
        while ((strcmp(vari,varliste[y]) != 0) && (varliste[y] != NULL))
        y++;         /* Variablennr. feststellen */ 
        varnum = y;
        y = 0;
        while (y < strlen(vari))
            {
            *zgr1 = 32;
            zgr1++;
            y++;           /* Variable loeschen */
            }    
        zgr1--;    
        y = 0;
	if (DEBUGGEN){
		printf("skolemisiere: trage var %d in Liste ein\n",varnum);
		}
        while (k->tab[y] > -1) y++; /* Listenende suchen und
                        Variablennummer eintragen */
	if (DEBUGGEN){
		printf("skolemisiere: trage var %d in Liste[%d] ein\n",varnum,
			y);
		}
        k->tab[y] = varnum; y++; k->tab[y] = -1;
        /* natuerlich muss sie aus der Substitutionsliste
           fuer diese Hoehe UND alle Kinder gestrichen
           werden : Innen engere Bindung  */
        k->substliste[varnum] = NULL;
           
        
        }
    if (*zgr1 == ']')
            /* Wenn man auf einen Existenzquantor stoesst: */
            
        {
        *zgr1 = 32;
        zgr1 = nextvar (zgr1,vari);
         y = 1;        
        while ((strcmp(vari,varliste[y]) != 0) && (varliste[y] != NULL))
        y++;
        varnum = y;

        y = 0;
        while (y < strlen(vari))
            {
            *zgr1 = 32;
            zgr1++;
            y++;           /* Variable loeschen */
            }
        zgr1--;                    
 
        zgr2 = (char *) mymalloc(groesse3); 
                    /* Platz fuer den Listeneintrag 
                      der substituierenden Skolem Funktion
                      schaffen */ 
        k->substliste[varnum] = zgr2;
                    /* Wenn es keine hoeheren Allvariablen
                    gibt : Konstante fuer diese Existenz-
                    bindung in die Substliste eintragen */
	if (DEBUGGEN){
		printf("skolemisieren: k->tab[0] = %d\n",k->tab[0]);
		}
        if (k->tab[0] == -1){
            strcpy(zgr2,neukonstante(vari));
		}
        else             /* sonst */
            {    
            strcpy(zgr2,neufunktor(vari));  
                    /* neuen Funktor eintragen */
            zgr2 = strcat(zgr2, "(");
            y = 0;
            while (k->tab[y] > -1)  
                    /* alle hoeherstehenden Allvariablen
                    eintragen */   
                {
                zgr2 = strcat(zgr2,varliste[k->tab[y]]);
                zgr2 = strcat(zgr2, ",");
                y++;
                }
            zgr2 = (zgr2 + strlen(zgr2));
            zgr2--;         /* Das letzte Komma ist noch zuviel */
            strcpy(zgr2, ")");
            }             
        }
    if ((*zgr1 >= 'A') && (*zgr1 <= 'Z'))
            /* Variable gefunden ! */
        {
        
         zgr1 = nextvar(zgr1,vari);
         

         y = 1;        
        while ((strcmp(vari,varliste[y]) != 0) && (varliste[y] != NULL))
        y++;
        varnum = y;
        zgr2 = k->substliste[varnum];
        if (zgr2 != NULL)
            /* Nur wenn diese Variale einen Eintrag in
            die Substitutionsliste hat, wird sie ersetzt! */
            {
            if (DEBUGGEN) printf("Substlisteneintrag: %s\n",zgr2);
            y = 0;
            while (y < strlen(vari))
                {
                *zgr1 = 32;
                zgr1++;
                y++;           /* Variable loeschen */
                }
            zgr1--;    
            strcpy(temp,zgr2);   /* Substitut eintragen */ 
            strcat(temp,zgr1);    /* Den alten Schwanz anhaengen */
            *zgr1 = '\0';         /* alten Kopf abschneiden */
            zgr1--;
            strcat(zgr1,temp);     /* Zusammenkleben  */
            zgr1 = (zgr1 + strlen(zgr2)); /* Substitut uebergehen */
            zgr1--;                   /* Nachfolgende Inkrementierung
                                         unwirksam machen! */ 
            
             /* Wenn das Programm hier haengen bleibt liegt es daran,
                dass eine Allbindung derselben Variablen 
                uebergeordnet ist! */ 
            if (strlen(k->txt) > (groesse1-25))
            		{
			/* BUG: jsc 11/23/96: was >175 */
			/* this seems to be an exmergency exit,
			/* in case, the txt gets too long
			/* there was a getc + a simplre return
			/**/
			fehlnum=-175;
            		return;
            		}
            }    
        else zgr1 = (zgr1 + (strlen(vari)-1));    
        }
          
    zgr1++;


    }
if (k->links != NULL)
    {
    listen_erben(k,k->links);
    skolemisieren(k->links);
    }
if (k->rechts != NULL)
    {
    listen_erben(k,k->rechts);
    skolemisieren(k->rechts);
    } 

}








/* ============================================================== */
void b_multiplizieren(k)
struct gabel *k;
{
struct gabel   *neu, *l, *r;
int				falschrum;

if (k != NULL) 
    {
    /*	printf("Anfang multiplizieren \n");  */
    
    l = k->links;
    r = k->rechts;
    if ((k->ju == 2) &&  ((l->ju == 1) || (r->ju == 1)))
        {
	if (DEBUGGEN) printf( "Schleife1; multivar: %d\n",multivar); 
        multivar++;
        falschrum = falsch; 
        if (r->ju != 1) 
            {
		if (DEBUGGEN) printf("Schleife 2\n");
            falschrum = wahr;
            l = k->rechts;
            r = k->links;
            }    /* im Knoten r steckt jedenfalls das und */
        neu = winzalloc();
        neu->links = l;
        neu->rechts = r->links;
        neu->ju = 2;
        if (falschrum ) k->rechts = neu; else k->links = neu;
        k->ju = 1;
        r->ju = 2;
        r->links = sparcopy(neu->links);
        
        }
           
       
     b_multiplizieren(k->links);
     b_multiplizieren(k->rechts);
       
       
     }

}        /* Einfache Rekursion genuegt nicht; Wenn zB. 2 'oder'
        uebereinanderstehen und dann erst ein 'und' kommt,
        wuerde nur das  zweite 'oder' erwischt */

 





     

/* ============================================================== */
void ausmultiplizieren (knoten)


struct gabel   *knoten; 
{

multivar = 1;
while (multivar > 0)
    {
    multivar = 0;
    b_multiplizieren(knoten);
    
    }
}





	



/* ============================================================== */
int kla_pruefen(s)
/* Der Rueckgabewert von rekla wird  in der globalen Variablen fehlspez 
   abgelegt und 
   hat zwei verschiedene Bedeutungen:
   Ist er positiv so kommt eine rechte Klammer zu frueh oder zuviel
   vor; fehlspez bezeichnet die Stelle an der der Fehler auftritt.
   Ist er negativ so bezeichnet er die Anzahl der am Ende fehlenden
   rechten Klammern.
*/  
char *s;
{
int x, reklam;

reklam = 0;
x = 0;
while (s[x] != '\0')
	{
	if (s[x] == '(') reklam--;
	if (s[x] == ')') reklam++;
	if (reklam > 0)
		{
		
		fehlspez = x;
		return(-29);
		}
	x++;
	}
if (reklam != 0)
	{
	fehlspez = reklam; 
	return (-29); 
	}
else return(0);	
}



/* ============================================================== */
int stauchen(z)
langformel z;
{
int len, zel, x, doppelt;
char *zgr;

zel = 0;
x = 0;
doppelt = falsch;
len = strlen(z);
zgr = z;
while ( zel < len) 
	{
	if (zgr[zel] == 32) doppelt = wahr;
	z[x] = zgr[zel];  
	x++; zel++;
	while (doppelt)
		{
		if (zgr[zel] == 32)    zel++;
  		else doppelt = falsch;
		}
	}
z[x] = '\0';

return(x);
}


/* ============================================================== */
void defies()
	{
fehlnum = -303;
GERMAN(printf("\nmaximaler benutzbarer Speicher: %d bytes (Option -mem0 ohne Limits)\n",maxmemsize);)
GERMAN(printf("\ngroesse1   : %d characters\n",groesse1); )
GERMAN(printf("- Literale\n");)
GERMAN(printf("- groesstmoegliches vorkommendes Quantorenpraefix; Alle Allbindungen");)
GERMAN(printf("\n  werden ja zusammen an den Anfang gezogen. Eine Bindung hat die\n");)
GERMAN(printf("  Laenge: 1(Quantor) + 2 (Space) + Variablenlaenge.\n");)
GERMAN(printf("  Alle zusammen duerfen die groesse1 nicht ueberschreiten!\n"); 	)
GERMAN(printf("\ngroesse1_b : %d characters\n",groesse1_b); 	)
GERMAN(printf("- groesstmoegliche Laenge einer Zeile der Ausgabedatei; d.h.,\n");)
GERMAN(printf("  eines LOP - Konjunktionsgliedes\n");)
GERMAN(printf("- groesste Laenge einer Zeile der Eingabedatei.\n");)
GERMAN(printf("\ngroesse1_c : %d characters\n",groesse1_c); 	)
GERMAN(printf("- groesste Laenge der Eingabedatei (*.pl1).\n");)
GERMAN(printf("\ngroesse2   : %d characters\n",groesse2);)
GERMAN(printf("- Groesse fuer Strings in denen einzelne Ausdruecke abgelegt werden\n");)
GERMAN(printf("  (Variablen, Funktoren, sonstige Konstanten).\n");  )
GERMAN(printf("\ngroesse3   : %d integers\n",groesse1); 	)
GERMAN(printf("- fuer Listen in denen die Nummern aller nichtlogischen Ausdruecke\n");)
GERMAN(printf("  eingetragen koennen werden koennen. Hoechstens so viele\n");)
GERMAN(printf("  nichtlogische Ausdruecke duerfen also vorkommen!\n"); 	)

printf("\n%d bytes usable memory (-mem 0: no limits)\n",maxmemsize);
printf("\n``groesse1''   : %d characters\n",groesse1); 
printf("-  max. size of literals\n");
printf("-  max. size of qunmator prefix: each variable binding is of size [bytes]:");
printf("   1(quantifier) + 2 (Spaces) + length of variable name\n");
printf("\n``groesse1_b'' : %d characters\n",groesse1_b); 	
printf("-  max. length of a LOP clause (on output)\n");
printf("-  max. length of line in input file.\n");
printf("\n``groesse1_c'' : %d characters\n",groesse1_c); 	
printf("-  max size of input file(*.pl1).\n");
printf("\n``groesse2''   : %d characters\n",groesse2);
printf("-  xmax. size of variables and constants.\n");
printf("\n``groesse3''   : %d integers\n",groesse1); 	
printf("-  max. number of non-logical expressions.\n");
	}




/* ============================================================== */
/* ============================================================== */
/* ============================================================== */
main(argc, argv)
int 	argc;
char	*argv[];
{
struct gabel    *ast;
mittelformel    filnam;
langformel      zeile, zei2;
int		lang, i, param;
wort		argumente;
char            *size;

currentmemsize=0;
maxmemsize=2000000; /* 2 meg maximum as default */
if (argc > 1) 
	strcpy(filnam,argv[argc-1]);
else
   {
   printf("\nPLOP     Version: %s\n\n",Versionsnr);
   printf("usage:\n");
   printf("plop  [-necho][-nopti][-sko][-neg]\n");
   printf("      [-tree][-debu][-defgro][-mem<... or 0>]\n");
   printf("      [-longsk]  file[.pl1]\n");
   fehlnum = -301;
   }




argumente[0] = '\0';
param = argc-1;
i = 1;
while (i < param)
	{
	strcat(argumente,argv[i]);
	i++;
	}
if (strposi(argumente,"-necho") != NULL) ECHO = falsch; else ECHO = wahr; 
if (strposi(argumente,"-nopti") != NULL) optimieren = falsch; 
				else optimieren = wahr; 
if (strposi(argumente,"-debu") != NULL) DEBUGGEN = wahr; 
				else DEBUGGEN = falsch;
									
if (strposi(argumente,"-neg") != NULL) NEGAT = wahr;
				else NEGAT = falsch;
if (strposi(argumente,"-sko") != NULL) SKLNF = wahr;
				else SKLNF = falsch;
if (strposi(argumente,"-tree") != NULL) DIMENSION = wahr; 
				else DIMENSION = falsch;					
if (strposi(argumente,"-longsk") != NULL){
	SHORTSK = falsch;					
	}
else {
	SHORTSK = wahr; 
	}

if ((size=strposi(argumente,"-mem")) != NULL) maxmemsize=atoi(&size[4]);


if ((strposi(argumente,"-defgro") != NULL) ||
    (strposi(filnam,"-defgro") != NULL) )
	defies();

klauseln = 0;
card_winz = 0;
card_spar = 0;
card_gabel = 0;

if (fehlnum >= 0)
	fehlnum = lesen(filnam, zeile);

if (fehlnum >= 0)   lang = stauchen(zeile);
	
if ((fehlnum >= 0) && (DIMENSION || ECHO)) 
	{
	GERMAN(printf ("Laenge der Eingabeformel: %d Zeichen.\n\n",lang);)
	printf ("Length of input formula: %d characters.\n\n",lang);
        if(ECHO)
		{
		GERMAN(printf ("\nAusgabeformel:\n");)
		printf ("\nOutput formula:\n");
		printf ("===============\n");
		}
	}
if ((fehlnum >= 0) && ((fehlnum = kla_pruefen(zeile)) != 0))
	{
	if (fehlspez > 0)
		{
	        GERMAN(printf ("Rechte Klammer zuviel oder zu frueh an");)
		GERMAN(printf(" der %d.Stelle!\n",fehlspez);)
		GERMAN(printf("%s\n",zeile);)
	        printf ("Too many closing parentheses or");
		printf(" closing parenthesis too early (position %d).\n",fehlspez);
		printf("Line: %s\n",zeile);
		strcpy(zei2,zeile);
		zei2[fehlspez] = '@';
		printf("%s\n",zei2);
		}

	else	{
		fehlspez = (0 - fehlspez);
		GERMAN(printf("Am Ende fehlen rechte Klammern! (%d)\n",fehlspez);)
		printf("Closing parentheses are missing (%d)\n",fehlspez);
		}
	}
	
				if (DEBUGGEN) printf("lesen fertig\n");


if (fehlnum >= 0)
	init_varliste(zeile);

if (fehlnum >= 0)
	fehlnum = allquantoren(zeile, Allquantor); 
if (fehlnum >= 0)
	fehlnum = allquantoren(zeile, Existenzquantor);
				if (DEBUGGEN) 
				    printf("Ende von allquantoren\n");

if (fehlnum >= 0)
	subst_init(zeile);   if (DEBUGGEN) printf("Ende von subst_init\n");   
if (fehlnum >= 0) lang = stauchen(zeile); 
if (fehlnum >= 0)

	ast = baum(zeile,lang);
card_baum = card_gabel;

				if (DEBUGGEN) 
			     	  {
				  printf("Ende von Baumwachsen !\n");
 				  getc(stdin); 
				  }

if (fehlnum >= 0)
	pfeileweg(ast);

				if (DEBUGGEN) {
				   printf("Ende von pfeileweg !\n");
					druckbaum(ast);
					getc(stdin);
				}
card_pfeil = card_gabel;
if (fehlnum >= 0)
	negrein(ast);


if (fehlnum >= 0)
	tab_init(ast); 

if (fehlnum >= 0)
	tabfuellen(ast);

				if (DEBUGGEN) {
				   printf("Ende von tabfuellen !\n");
					druckbaum(ast);
					getc(stdin);
				}


if (fehlnum >= 0)
	leerquant_weg(ast);

			if (DEBUGGEN) 
				printf("Ende von leerquantweg !\n");
				/* getc(stdin); */


if (fehlnum >= 0)
	varbinden(ast);
			if (DEBUGGEN) 
				{
				printf("Ende von varbinden!!\n");
				druckbaum(ast);
				getc(stdin);
				} 


if (NEGAT && (fehlnum >= 0))
	{ 
	/* 960816 PJ: BUGFIX memory trouble */
	/* my 1st attempt: ast->txt = strcat(ast->txt,"~"); */
	/* original version leading to memory errors: 
	/* ast->txt = strcat("~", ast->txt); */
	/* my 2nd attempt: order was correct, only constness was not.
	   Note: old ast->txt should be freed ...*/
	char *buf;
	buf = (char *) mymalloc(groesse1); buf[0]='\0';
	strcat(buf,"~");
	strcat(buf,ast->txt);
	ast->txt=buf;
	negrein(ast);
	}

if (fehlnum >= 0)
	Q_nach_innen(ast);

			     if (DEBUGGEN) 
                       		printf("Ende von Q_nach_innen!\n");

if (fehlnum >= 0)
	listen_init(ast);
			      if (DEBUGGEN) 
				{
				printf("Ende von listen_init!\n");
				getc(stdin);    
				}
if (fehlnum >= 0)
	skolemisieren(ast);

				if (DEBUGGEN)
				   { 
				   printf("Ende von skolemisieren!\n");
				   getc(stdin);
				   }
if (fehlnum >= 0)
	ausmultiplizieren(ast);

			    if (DEBUGGEN)
			     printf("Ende von  ausmultiplizieren!\n\n");

if (fehlnum >= 0)
	fehlnum = schreiben(filnam,ast);
if ((fehlnum >= 0) && DIMENSION)
   {
   printf("\nKnotenzahl nach Baumwachsen       :%d\n",card_baum);
   printf("Knotenzahl nach Pfeilbeseitigung  :%d\n",card_pfeil);
   printf("Spar-Knoten nach Ausmultiplizieren :%d\n",card_spar);
   printf("Winz-Knoten nach Ausmultiplizieren :%d\n",card_winz);
   }
if (fehlnum != 0)
	fehlermeldung(fehlnum);

if ((fehlnum >= 0) && (ECHO || DIMENSION))
   {
   GERMAN(printf("\nErzeugte Klauseln: %d\n",klauseln);)
   printf("\nNumber of generated clauses: %d\n",klauseln);
   if (klauseln == 0) {
	GERMAN(printf("D.h., Eingabeformel ist Theorem!\n"); )
	printf("Input formula is satisfiable.\n"); 
	}
   printf("\n");
   }

return (0);
}












