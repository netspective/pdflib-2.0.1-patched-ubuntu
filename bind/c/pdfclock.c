/*---------------------------------------------------------------------------*
 |        PDFlib - A library for dynamically generating PDF files            |
 +---------------------------------------------------------------------------+
 |        Copyright (c) 1997-1999 Thomas Merz. All rights reserved.          |
 +---------------------------------------------------------------------------+
 |    This software is not in the public domain.  It is subject to the       |
 |    "Aladdin Free Public License".  See the file license.txt for details.  |
 |    This license grants you the right to use and redistribute PDFlib       |
 |    under certain conditions. Among other things, the license requires     |
 |    that the copyright notice and this notice be preserved on all copies.  |
 |    This requirement extends to ports to other programming languages.      |
 |                                                                           |
 |    In short, you are allowed to develop and use PDFlib-based software     |
 |    as long as you don't sell it. Commercial use of PDFlib requires a      |
 |    commercial license which can be obtained from the author of PDFlib.    |
 |    Contact information can be found in the accompanying PDFlib manual.    |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* pdfclock.c
 *
 * A little PDFlib application to draw an analog clock.
 *
 */

#include <stdio.h>
#include <time.h>

#include "pdflib.h"

#define RADIUS		200.0
#define MARGIN		20.0

int
main(int argc, char *argv[])
{
    PDF		*p;
    float	alpha;
    time_t	timer;
    struct tm	ltime;
    
    p = PDF_new();

    if (PDF_open_file(p, "pdfclock_c.pdf") == -1) {
	fprintf(stderr, "Error: cannot open PDF file pdfclock_c.pdf.\n");
	exit(2);
    }

    PDF_set_info(p, "Creator", "pdfclock.c");
    PDF_set_info(p, "Author", "Thomas Merz");
    PDF_set_info(p, "Title", "PDF clock (C)");

    PDF_begin_page(p, (unsigned int) (2 * (RADIUS + MARGIN)),
		      (unsigned int) (2 * (RADIUS + MARGIN)));
    
    PDF_translate(p, RADIUS + MARGIN, RADIUS + MARGIN);
    PDF_setrgbcolor(p, 0.0, 0.0, 1.0);
    PDF_save(p);

    /* minute strokes */
    PDF_setlinewidth(p, 2.0);
    for (alpha = 0; alpha < 360; alpha += 6)
    {
	PDF_rotate(p, 6.0);
	PDF_moveto(p, RADIUS, 0.0);
	PDF_lineto(p, (float) (RADIUS-MARGIN/3), 0.0);
	PDF_stroke(p);
    }

    PDF_restore(p);
    PDF_save(p);

    /* 5 minute strokes */
    PDF_setlinewidth(p, 3.0);
    for (alpha = 0; alpha < 360; alpha += 30)
    {
	PDF_rotate(p, 30.0);
	PDF_moveto(p, RADIUS, 0.0);
	PDF_lineto(p, RADIUS-MARGIN, 0.0);
	PDF_stroke(p);
    }

    time(&timer);
    ltime = *localtime(&timer);

    /* draw hour hand */
    PDF_save(p);
    PDF_rotate(p, 
	    (float)(-((ltime.tm_min/60.0) + ltime.tm_hour - 3.0) * 30.0));
    PDF_moveto(p, -RADIUS/10, -RADIUS/20);
    PDF_lineto(p, RADIUS/2, 0.0);
    PDF_lineto(p, -RADIUS/10, RADIUS/20);
    PDF_closepath(p);
    PDF_fill(p);
    PDF_restore(p);

    /* draw minute hand */
    PDF_save(p);
    PDF_rotate(p,
	    (float) (-((ltime.tm_sec/60.0) + ltime.tm_min - 15.0) * 6.0));
    PDF_moveto(p, -RADIUS/10, -RADIUS/20);
    PDF_lineto(p, RADIUS * 0.8, 0.0);
    PDF_lineto(p, -RADIUS/10, RADIUS/20);
    PDF_closepath(p);
    PDF_fill(p);
    PDF_restore(p);

    /* draw second hand */
    PDF_setrgbcolor(p, 1.0, 0.0, 0.0);
    PDF_setlinewidth(p, 2);
    PDF_save(p);
    PDF_rotate(p, (float) -((ltime.tm_sec - 15.0) * 6.0));
    PDF_moveto(p, -RADIUS/5, 0.0);
    PDF_lineto(p, RADIUS, 0.0);
    PDF_stroke(p);
    PDF_restore(p);

    /* draw little circle at center */
    PDF_circle(p, 0, 0, (float) RADIUS/30);
    PDF_fill(p);

    PDF_restore(p);

    PDF_end_page(p);

    PDF_close(p);
    PDF_delete(p);

    exit(0);
}
