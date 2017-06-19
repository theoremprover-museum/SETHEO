/*****************************************************************************
;;; MODULE NAME : filehandler.c
;;;
;;; PARENT      : main.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)main.c	15.2 01 Apr 1996
;;;
;;; AUTHOR      : Rudolf Zeilhofer
;;;
;;; DATE        : 18.02.97
;;;
;;; DESCRIPTION : opens and closes the input/output files
;;;
;;; REMARKS     :
******************************************************************************/

#include "types.h"
#include "extern.h"

/*****************************************************************************/
/* functions (inwasm/filehandler.c)                                          */
/*****************************************************************************/
void open_io_files();
void open_input_file();
void open_lop_output_file();
void open_code_output_file();
void open_del_output_file(); 


/*****************************************************************************/
/* open_io_files()                                                           */
/* Description: opens the input and output files                             */
/*****************************************************************************/
void open_io_files(argc, argv)
int argc;
char *argv[];
{
  open_input_file(argc, argv);
  open_lop_output_file(argc, argv);
  open_code_output_file();
  open_del_output_file(); 
}

/*****************************************************************************/
/* open_input_file()                                                         */
/* Description: opens the specified input file (*.lop) for parsing           */
/*****************************************************************************/
void open_input_file(argc, argv)
int argc;
char *argv[];
{
  char s[100];

  if ((yyin = fopen(argv[argc-1], "r")) == NULL) {
    /* now, let's try if the file extension is missing */
    sprintf(s, "%s.lop", argv[argc-1]);
    if ((yyin = fopen(s, "r")) == NULL) {
      fprintf(stderr,"  Error: Could not open %s or %s.lop.\n", 
	      argv[argc-1],argv[argc-1]);
      exit(20);
    }
    else {
      strcpy(filename_base, argv[argc-1]);
    }
  }
  else {
    /* we have a filename: x.lop, strip off the .lop for further processing */
    strcpy(filename_base, argv[argc-1]);
    *(filename_base+strlen(argv[argc-1])-4)= '\0';
  }
}


/*****************************************************************************/
/* open_lop_output_file()                                                    */
/* Description: opens the specified output file (*.lop) for parsing          */
/*****************************************************************************/
void open_lop_output_file(argc,argv)
int argc;
char *argv[];
{
  char s[100];

  /* if inmasw-flag 'nocode` is set: generate the lop-output */
  if (inwasm_flags.lop){
    sprintf(s, "%s_pp.lop", argv[argc-1]);
    if ((lopout_file = fopen(s, "w")) == NULL) {
      fprintf(stderr,"  Error: Could not open %s.lopout.\n", argv[argc-1]);
      exit(21);
    }
  }
}


/*****************************************************************************/
/* open_code_output_file()                                                   */
/* Description: opens the *.hex output file (code).                          */
/*****************************************************************************/
void open_code_output_file()
{
  char s[100];

  sprintf(s, "%s.%s", filename_base, (inwasm_flags.hex_out) ? "hex" : "s");
  if ((!inwasm_flags.nocode) &&
      (code_file = fopen(s, "w")) == NULL) {
    fprintf(stderr,"  Error: Could not open %s.\n",s);
    exit(22);
  }
}


/*****************************************************************************/
/* open_del_output_file()                                                    */
/* Description: opens the output file for deleted connections.               */
/*****************************************************************************/
void open_del_output_file()
{
#ifdef CDEL_FILE
  sprintf(filename_cdel, "%s.cdel", filename_base);
  if ((inwasm_flags.linksubs) && 
      (del_file = fopen(filename_cdel, "w")) == NULL) {
    fprintf(stderr, "  Error: Could not open %s\n", filename_cdel);
    exit(23);
  }
#endif
}
