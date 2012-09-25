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

/* p_draw.c
 *
 * PDFlib drawing routines
 *
 */

#include <stdio.h>
#include <math.h>

#include "p_intern.h"

/* Path segment operators */

PDFLIB_API void PDFLIB_CALL
PDF_moveto(PDF *p, float x, float y)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    pdf_end_text(p);
    (void) fprintf(p->fp,"%s %s m\n", pdf_float(buf1, x), pdf_float(buf2, y));
}

PDFLIB_API void PDFLIB_CALL
PDF_lineto(PDF *p, float x, float y)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    pdf_end_text(p);
    (void) fprintf(p->fp,"%s %s l\n", pdf_float(buf1, x), pdf_float(buf2, y));
}

PDFLIB_API void PDFLIB_CALL
PDF_curveto(PDF *p, float x1, float y1, float x2, float y2, float x3, float y3)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];
    char buf5[FLOATBUFSIZE], buf6[FLOATBUFSIZE];

    pdf_end_text(p);

    if (x2 == x3 && y2 == y3)	/* second c.p. coincides with final point */
	(void) fprintf(p->fp,"%s %s %s %s y\n",
		    pdf_float(buf1, x1), pdf_float(buf2, y1),
		    pdf_float(buf3, x3), pdf_float(buf4, y3));
    else			/* general case with four distinct points */
	(void) fprintf(p->fp,"%s %s %s %s %s %s c\n",
		    pdf_float(buf1, x1), pdf_float(buf2, y1),
		    pdf_float(buf3, x2), pdf_float(buf4, y2),
		    pdf_float(buf5, x3), pdf_float(buf6, y3));
}

/* Convenience routines */

/* 
 * "Best fit" BCP approximation for an arc segment as suggested
 * by Berthold K.P.Horn. Thanks BKPH!
 */

#define ARC_MAGIC	((float) 0.5523)
#define MIN(x, y)	((x) < (y) ? (x) : (y))

/* PDF_arc() assumes that the current point is already at the arc's start */

PDFLIB_API void PDFLIB_CALL
PDF_arc(PDF *p, float x, float y, float r, float alpha, float beta)
{
    float bcp;
    float cos_alpha, cos_beta, sin_alpha, sin_beta;

    if (r < 0)
	pdf_error(p, PDF_ValueError, "Negative circle radius %f", r);
    
    if (beta - alpha > 90.0)
    {
	/* cut down in 90 degree segments until done */
	PDF_arc(p, x, y, r, alpha, MIN(alpha + 90, beta));
	if (alpha + 90 < beta)
	    PDF_arc(p, x, y, r, alpha + 90, beta);
	return;
    }

    /* speed up special case for quadrants and circles */
    if ((int) alpha == alpha && (int) beta == beta &&
    	beta - alpha == 90 && (int) alpha % 90 == 0)
    {
	/* prune angle values */
	alpha = (float) ((int) alpha % 360);
	beta = (float) ((int) beta % 360);

	switch ((int) alpha) {
	    case 0:
		PDF_curveto(p, x + r, y + r*ARC_MAGIC, 
			x + r*ARC_MAGIC, y + r, x, y + r);
		break;
	    case 90:
		PDF_curveto(p, x - r*ARC_MAGIC, y + r, 
			x - r, y + r*ARC_MAGIC, x - r, y);
		break;
	    case 180:
		PDF_curveto(p, x - r, y - r*ARC_MAGIC, 
			x - r*ARC_MAGIC, y - r, x, y - r);
		break;
	    case 270:
		PDF_curveto(p, x + r*ARC_MAGIC, y - r, 
			x + r, y - r*ARC_MAGIC, x + r, y);
		break;
	    default:
		break;
	}
	return;
    }

    alpha = (float) (alpha * M_PI / 180);		/* convert to radians */
    beta = (float) (beta * M_PI / 180);		/* convert to radians */

    /* This formula yields ARC_MAGIC for alpha == 0, beta == 90 degrees */
    bcp = (float) (4/3 * (1 - cos((beta - alpha)/2)) / sin((beta - alpha)/2));
    
    sin_alpha = (float) sin(alpha);
    sin_beta = (float) sin(beta);
    cos_alpha = (float) cos(alpha);
    cos_beta = (float) cos(beta);

    PDF_curveto(p, 
		x + r * (cos_alpha - bcp * sin_alpha),		/* p1 */
		y + r * (sin_alpha + bcp * cos_alpha),
		x + r * (cos_beta + bcp * sin_beta),		/* p2 */
		y + r * (sin_beta - bcp * cos_beta),
		x + r * cos_beta, y + r * sin_beta);		/* p3 */
}

PDFLIB_API void PDFLIB_CALL
PDF_circle(PDF *p, float x, float y, float r)
{
    if (r < 0)
	pdf_error(p, PDF_ValueError, "Negative circle radius %f", r);

    /* draw four Bezier curves to approximate a circle */
    PDF_moveto(p, x + r, y);
    PDF_curveto(p, x + r, y + r*ARC_MAGIC, x + r*ARC_MAGIC, y + r, x, y + r);
    PDF_curveto(p, x - r*ARC_MAGIC, y + r, x - r, y + r*ARC_MAGIC, x - r, y);
    PDF_curveto(p, x - r, y - r*ARC_MAGIC, x - r*ARC_MAGIC, y - r, x, y - r);
    PDF_curveto(p, x + r*ARC_MAGIC, y - r, x + r, y - r*ARC_MAGIC, x + r, y);
}

PDFLIB_API void PDFLIB_CALL
PDF_rect(PDF *p, float x, float y, float width, float height)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];

    pdf_end_text(p);

    (void) fprintf(p->fp,"%s %s %s %s re\n",
    			pdf_float(buf1, x), pdf_float(buf2, y),
			pdf_float(buf3, width), pdf_float(buf4, height));
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath(PDF *p)
{
    pdf_end_text(p);

    (void) fputs("h\n", p->fp);
}

/* Path painting operators */

PDFLIB_API void PDFLIB_CALL
PDF_endpath(PDF *p)
{
    pdf_end_text(p);

    (void) fputs("n\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_stroke(PDF *p)
{
    pdf_end_text(p);

    (void) fputs("S\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_stroke(PDF *p)
{
    pdf_end_text(p);

    (void) fputs("s\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_fill(PDF *p)
{
    pdf_end_text(p);

    switch (p->fill_rule) {
	case pdf_fill_winding:
	    (void) fputs("f\n", p->fp);
	    break;
	case pdf_fill_evenodd:
	    (void) fputs("f*\n", p->fp);
	    break;
	default:
	    pdf_error(p, PDF_RuntimeError,
		"Inconsistent fill rule value in PDF_fill");
	    return;
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_fill_stroke(PDF *p)
{
    pdf_end_text(p);

    switch (p->fill_rule) {
	case pdf_fill_winding:
	    (void) fputs("B\n", p->fp);
	    break;
	case pdf_fill_evenodd:
	    (void) fputs("B*\n", p->fp);
	    break;
	default:
	    pdf_error(p, PDF_RuntimeError,
		"Inconsistent fill rule value in PDF_fill_stroke");
	    return;
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_fill_stroke(PDF *p)
{
    pdf_end_text(p);

    switch (p->fill_rule) {
	case pdf_fill_winding:
	(void) fputs("b\n", p->fp);
	break;
    case pdf_fill_evenodd:
	(void) fputs("b*\n", p->fp);
	break;
    default:
	pdf_error(p, PDF_RuntimeError,
	    "Inconsistent fill rule value in PDF_closepath_fill_stroke");
	return;
    }
}

/* Path clipping operators */

PDFLIB_API void PDFLIB_CALL
PDF_clip(PDF *p)
{
    pdf_end_text(p);

    switch (p->fill_rule) {
	case pdf_fill_winding:
	(void) fputs("W\n", p->fp);
    break;
	case pdf_fill_evenodd:
	(void) fputs("W*\n", p->fp);
	break;
    default:
	pdf_error(p, PDF_RuntimeError,
	    "Inconsistent fill rule value in PDF_clip");
	return;
    }
}
