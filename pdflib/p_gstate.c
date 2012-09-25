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

/* p_gstate.c
 *
 * PDFlib routines dealing with the graphics states
 *
 */

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "p_intern.h"

/* Special Graphics State */

PDFLIB_API void PDFLIB_CALL
PDF_save(PDF *p)
{
    pdf_end_text(p);
    (void) fputs("q\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_restore(PDF *p)
{
    pdf_end_text(p);
    (void) fputs("Q\n", p->fp);
}

void
pdf_concat(PDF *p, pdf_matrix m)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];
    char buf4[FLOATBUFSIZE], buf5[FLOATBUFSIZE], buf6[FLOATBUFSIZE];

    pdf_end_text(p);
    (void) fprintf(p->fp,"%s %s %s %s %s %s cm\n",
	      pdf_float(buf1, m.a), pdf_float(buf2, m.b), pdf_float(buf3, m.c),
	      pdf_float(buf4, m.d), pdf_float(buf5, m.e), pdf_float(buf6, m.f));
}

/* Convenience routines */

PDFLIB_API void PDFLIB_CALL
PDF_translate(PDF *p, float tx, float ty)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    pdf_end_text(p);
    (void) fprintf(p->fp,"1 0 0 1 %s %s cm\n",
	    pdf_float(buf1, tx), pdf_float(buf2, ty));

}

PDFLIB_API void PDFLIB_CALL
PDF_scale(PDF *p, float sx, float sy)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    pdf_end_text(p);
    (void) fprintf(p->fp,"%s 0 0 %s 0 0 cm\n",
	    pdf_float(buf1, sx), pdf_float(buf2, sy));
}

PDFLIB_API void PDFLIB_CALL
PDF_rotate(PDF *p, float phi)
{
    float c, s;
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];

    if (phi == 0)
	return;

    phi = (float) (phi * M_PI / 180);	/* convert to radians */

    c = (float) cos(phi);
    s = (float) sin(phi);

    pdf_end_text(p);
    (void) fprintf(p->fp,"%s %s %s %s 0 0 cm\n",
	    pdf_float(buf1, c), pdf_float(buf2, s),
	    pdf_float(buf3, -s), pdf_float(buf4, c));
}

/* General Graphics State */

PDFLIB_API void PDFLIB_CALL
PDF_setdash(PDF *p, float d1, float d2)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    if (d1 < 0.0 || d2 < 0.0) {
	pdf_error(p, PDF_NonfatalError, "Negative dash value %f, %f",
		d1, d2);
	return;
    }

    if (d1 == 0.0 && d2 == 0.0)		/* both zero means solid line */
	(void) fputs("[] 0 d\n", p->fp);
    else
	(void) fprintf(p->fp,"[%s %s] 0 d\n",
		pdf_float(buf1, d1), pdf_float(buf2, d2));
}

PDFLIB_API void PDFLIB_CALL
PDF_setpolydash(PDF *p, float *darray, int length)
{
    char buf[FLOATBUFSIZE];

    int i;

    if (length == 0 || length == 1) {	/* length == 0 or 1 means solid line */
	(void) fputs("[] 0 d\n", p->fp);
	return;
    }

    /* sanity checks */
    if (length < 0) {
	pdf_error(p, PDF_NonfatalError, "Negative dash array length %d",
		length);
	return;
    }

    for (i = 0; i < length; i++) {
	if (darray[i] < 0.0) {
	    pdf_error(p, PDF_NonfatalError, "Negative dash value %d",
	    	darray[i]);
	    return;
	}
    }

    (void) fputs("[", p->fp);

    for (i = 0; i < length; i++) {
	(void) fprintf(p->fp,"%s ", pdf_float(buf, darray[i]));
    }
    (void) fputs("] 0 d\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_setflat(PDF *p, float flat)
{
    char buf[FLOATBUFSIZE];

    if (flat < 0.0 || flat > 100.0) {
	pdf_error(p, PDF_NonfatalError, "Illegal flat value %f", flat);
	return;
    }

    (void) fprintf(p->fp,"%s i\n", pdf_float(buf, flat));
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinejoin(PDF *p, int join)
{
#define LAST_JOIN	2
	if (join > LAST_JOIN) {
		pdf_error(p, PDF_NonfatalError, "Bogus line join value %d",
			join);
		return;
	}

    (void) fprintf(p->fp,"%d j\n", join);
#undef LAST_JOIN
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinecap(PDF *p, int cap)
{
#define LAST_CAP	2
	if (cap > LAST_CAP) {
		pdf_error(p, PDF_NonfatalError, "Bogus line cap value %d",
			cap);
		return;
	}

    (void) fprintf(p->fp,"%d J\n", cap);
#undef LAST_CAP
}

PDFLIB_API void PDFLIB_CALL
PDF_setmiterlimit(PDF *p, float miter)
{
    char buf[FLOATBUFSIZE];

    if (miter < 1.0) {
	pdf_error(p, PDF_NonfatalError, "Bogus miter limit %f", miter);
	return;
    }

    (void) fprintf(p->fp,"%s M\n", pdf_float(buf, miter));
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinewidth(PDF *p, float width)
{
    char buf[FLOATBUFSIZE];

    if (width <= 0.0) {
	pdf_error(p, PDF_NonfatalError, "Bogus line width %f", width);
	return;
    }

    (void) fprintf(p->fp,"%s w\n", pdf_float(buf, width));
}

PDFLIB_API void PDFLIB_CALL
PDF_set_fillrule(PDF *p, const char* fillrule)
{
    if (fillrule == NULL) {
	pdf_error(p, PDF_ValueError, "Null name for fill rule");
	return;
    }

    if (!strcmp(fillrule, "winding"))
	p->fill_rule = pdf_fill_winding;
    else if (!strcmp(fillrule, "evenodd"))
	p->fill_rule = pdf_fill_evenodd;
    else
	pdf_error(p, PDF_ValueError, "Bogus fill rule '%s'", fillrule);
}
