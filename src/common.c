#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "common.h"
#include "misc.h"

/* print an error and die */
void panic (char *fmt, ...)
{
    va_list args;
    char *s;

#ifndef CLOSURE_MODE
    va_start (args, fmt);
    fprintf (stderr, "*** Panic: ");
    vfprintf (stderr, fmt, args);
    fprintf (stderr, "\n");
    va_end (args);
#endif
    exit (1);
}

/* print an error and die if we are told to */
void error (char *fmt, ...)
{
  va_list args;
  char c;

#ifndef CLOSURE_MODE
  va_start (args, fmt);
  fprintf (stderr, "*** Error: ");
  vfprintf (stderr, fmt, args);
  fprintf (stderr, "\n");
  va_end (args);
#endif
}

/* print a warning and return */
void warn (char *fmt, ...)
{
    va_list args;

#ifndef CLOSURE_MODE
    va_start (args, fmt);
    fprintf (stderr, "*** Warning: ");
    vfprintf (stderr, fmt, args);
    fprintf (stderr, "\n");
    va_end (args);
#endif
}

/* print a warning and return */
void disp (char *fmt, ...)
{
    va_list args;

#ifndef CLOSURE_MODE
    va_start (args, fmt);
    fprintf (stderr, "--- ");
    vfprintf (stderr, fmt, args);
    fprintf (stderr, "\n");
    va_end (args);
#endif
}


/* comment is all white space and all text behind '#' to EOL (inlcuding '#') */
void SkipComment (FILE *fp)
{
    int c, fini = 0;
    while (!fini && ((c = fgetc(fp)) != EOF)) {
	if (c == '#')
	    while ((c = fgetc(fp)) != '\n');
	else if (c!=' ' && c!='\t' && c!='\n') {
	    ungetc(c, fp);
	    fini = 1;
	}
    }
}
