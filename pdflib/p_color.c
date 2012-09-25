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

/* p_color.c
 *
 * PDFlib color routines
 *
 */

#include <stdio.h>
#include <math.h>

#include "p_intern.h"

/* Color operators */

/* avoid wrong error messages due to rounding artifacts
 * This doesn't do any harm since we truncate to 5 decimal places anyway
 * when producing PDF output.
 */
#define EPSILON	1.00001

PDFLIB_API void PDFLIB_CALL
PDF_setgray_fill(PDF *p, float g)
{
    char buf[FLOATBUFSIZE];

    if (g < 0.0 || g > EPSILON ) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus gray value (%f) in PDF_setgray_fill", g);
	return;
    }

    (void) fprintf(p->fp,"%s g\n", pdf_float(buf, g));
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray_stroke(PDF *p, float g)
{
    char buf[FLOATBUFSIZE];

    if (g < 0.0 || g > EPSILON ) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus gray value (%f) in PDF_setgray_stroke", g);
	return;
    }

    (void) fprintf(p->fp,"%s G\n", pdf_float(buf, g));
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray(PDF *p, float g)
{
    if (g < 0.0 || g > EPSILON ) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus gray value (%f) in PDF_setgray", g);
	return;
    }

    PDF_setgray_stroke(p, g);
    PDF_setgray_fill(p, g);
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_fill(PDF *p, float red, float green, float blue)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];

    if (red < 0.0 || red > EPSILON || green < 0.0 || green > EPSILON ||
	blue < 0.0 || blue > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f) in PDF_setrgbcolor_fill",
	    red, green, blue);
	return;
    }

    if (red == green && green == blue)
	PDF_setgray_fill(p, red);
    else {
	(void) fprintf(p->fp, "%s %s %s rg\n",
	   pdf_float(buf1, red), pdf_float(buf2, green), pdf_float(buf3, blue));
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_stroke(PDF *p, float red, float green, float blue)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];

    if (red < 0.0 || red > EPSILON || green < 0.0 || green > EPSILON ||
	blue < 0.0 || blue > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f) in PDF_setrgbcolor_stroke",
	    red, green, blue);
	return;
    }

    if (red == green && green == blue)
	PDF_setgray_stroke(p, red);
    else {
	(void) fprintf(p->fp, "%s %s %s RG\n",
	   pdf_float(buf1, red), pdf_float(buf2, green), pdf_float(buf3, blue));
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor(PDF *p, float red, float green, float blue)
{
    if (red < 0.0 || red > EPSILON || green < 0.0 || green > EPSILON ||
	blue < 0.0 || blue > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f) in PDF_setrgbcolor",
	    red, green, blue);
	return;
    }

    if (red == green && green == blue)
	PDF_setgray(p, red);
    else {
	PDF_setrgbcolor_fill(p, red, green, blue);
	PDF_setrgbcolor_stroke(p, red, green, blue);
    }
}
