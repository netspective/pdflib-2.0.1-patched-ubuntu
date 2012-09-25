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

/* p_text.c
 *
 * PDFlib text routines
 *
 */

#include <stdio.h>
#include <string.h>

#include "p_intern.h"
#include "p_afmparse.h"

/* Auxiliary routines */

void
pdf_begin_text(PDF *p)
{
    if (p->contents == c_text)
	return;

    p->procset	|= Text;
    p->contents	= c_text;

    (void) fputs( "BT\n", p->fp);
}

void
pdf_end_text(PDF *p)
{
    if (p->contents != c_text)
	return;

    p->contents	= c_stream;

    (void) fputs("ET\n", p->fp);
}

void
pdf_quote_string(PDF *p, const char *text)
{
    const unsigned char *goal, *s;
    size_t len;

    len = pdf_strlen(text)-1;	/* subtract a null byte... */

    if (pdf_is_unicode(text))	/* ...and possibly another one */
	len--;

    fputc('(', p->fp);
    goal = (const unsigned char *) text + len;

    for (s = (const unsigned char *) text; s < goal; s++) {
		/* expand tabs */
		if (*s == '\t') {
			fputs("    ", p->fp);
		} else {
		    if (*s == '(' || *s == ')' || *s == '\\')
			fputc('\\', p->fp);
		    fputc(*s, p->fp);
		}
    }
    fputc(')', p->fp);
}

/* Text state operators */

PDFLIB_API void PDFLIB_CALL
PDF_set_leading(PDF *p, float l)
{
    char buf[FLOATBUFSIZE];

    pdf_begin_text(p);
    (void) fprintf(p->fp, "%s TL\n", pdf_float(buf, l));
}

PDFLIB_API void PDFLIB_CALL
PDF_set_text_rendering(PDF *p, int mode)
{
#define LAST_MODE	7
	if (mode > LAST_MODE) {
		pdf_error(p, PDF_NonfatalError,
			"Bogus text rendering mode %d", mode);
		return;
	}
		
    pdf_begin_text(p);
    (void) fprintf(p->fp, "%d Tr\n", mode);
#undef LAST_MODE
}

PDFLIB_API void PDFLIB_CALL
PDF_set_text_rise(PDF *p, float rise)
{
    char buf[FLOATBUFSIZE];

    pdf_begin_text(p);
    (void) fprintf(p->fp, "%s Ts\n", pdf_float(buf, rise));
}

PDFLIB_API void PDFLIB_CALL
PDF_set_horiz_scaling(PDF *p, float scale)
{
    char buf[FLOATBUFSIZE];

    pdf_begin_text(p);
    (void) fprintf(p->fp, "%s Tz\n", pdf_float(buf, scale));
}

/* Text positioning operators */

PDFLIB_API void PDFLIB_CALL
PDF_set_text_matrix(PDF *p, float a, float b, float c, float d, float e, float f)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];
    char buf5[FLOATBUFSIZE], buf6[FLOATBUFSIZE];

    pdf_begin_text(p);
    (void) fprintf(p->fp,"%s %s %s %s %s %s Tm\n",
		  pdf_float(buf1, a), pdf_float(buf2, b), pdf_float(buf3, c),
		  pdf_float(buf4, d), pdf_float(buf5, e), pdf_float(buf6, f));
}

PDFLIB_API void PDFLIB_CALL
PDF_set_text_pos(PDF *p, float x, float y)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    pdf_begin_text(p);
    (void) fprintf(p->fp,"1 0 0 1 %s %s Tm\n",
    	pdf_float(buf1, x), pdf_float(buf2, y));
}

/* Text string operators */

PDFLIB_API void PDFLIB_CALL
PDF_show(PDF *p, const char *text)
{
    pdf_begin_text(p);
    pdf_quote_string(p, text);
    (void) fputs("Tj\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_continue_text(PDF *p, const char *text)
{
    pdf_begin_text(p);
    pdf_quote_string(p, text);
    (void) fputs("'\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_show_xy(PDF *p, const char *text, float x, float y)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    pdf_begin_text(p);

    (void) fprintf(p->fp,"1 0 0 1 %s %s Tm\n",
    	pdf_float(buf1, x), pdf_float(buf2, y));
    pdf_quote_string(p, text);
    (void) fputs("Tj\n", p->fp);
}

/* character spacing for justified lines */
PDFLIB_API void PDFLIB_CALL
PDF_set_char_spacing(PDF *p, float spacing)
{
    char buf[FLOATBUFSIZE];

    pdf_begin_text(p);
    (void) fprintf(p->fp,"%s Tc\n", pdf_float(buf, spacing));
}

/* word spacing for justified lines */
PDFLIB_API void PDFLIB_CALL
PDF_set_word_spacing(PDF *p, float spacing)
{
    char buf[FLOATBUFSIZE];

    pdf_begin_text(p);
    (void) fprintf(p->fp,"%s Tw\n", pdf_float(buf, spacing));
}

PDFLIB_API float PDFLIB_CALL
PDF_stringwidth(PDF *p, const char *text, int font, float size)
{
    const char *cp;
    float width = (float) 0.0;

    /* Check parameters */
    if (font < 0 || font >= p->fonts_number)
	pdf_error(p, PDF_ValueError, "Bad font descriptor %d", font);
	
    if (size < 0)
	pdf_error(p, PDF_ValueError, "Bad font size %f", size);

    for (cp = text; *cp; cp++)
	width += size * p->fonts[font].widths[(unsigned char) *cp]/1000;

    return width;
}
