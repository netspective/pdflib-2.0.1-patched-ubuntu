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

/* p_font.c
 *
 * PDFlib font handling routines
 *
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "p_intern.h"
#include "p_afmparse.h"

#include "ansi_e.h"
#include "macrom_e.h"
#include "pdfdoc_e.h"
#include "p_metrics.h"

#define LINEBUFLEN	512

/* PDF's base 14 fonts, guaranteed to be always available */
static const char *pdf_base14_names[] = {
  "Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
  "Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
  "Symbol",
  "Times-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
  "ZapfDingbats",
  NULL
};

static const char *pdf_encoding_names[] = {
    NULL, "PDFDocEncoding", "MacRomanEncoding", 
    "MacExpertEncoding", "WinAnsiEncoding"
};

static pdf_encodingvector *pdf_encodings[] = {
    NULL, &pdf_pdfdoc, &pdf_macroman, NULL, &pdf_winansi
};

/* Type 1 font portions: ASCII, encrypted, zeros */
typedef enum { ascii, encrypted, zeros } pdf_t1portion;

typedef struct {
    pdf_t1portion portion;
    long length1, length2, length3;
    FILE *fontfile;
} t1_private_data;

PDFLIB_API int PDFLIB_CALL
PDF_get_font(PDF *p)
{
    if (p->fonts_number == 0) 	/* no font set */
	pdf_error(p, PDF_RuntimeError,
		"Queried current font before setting one!");

    return p->current_font;
}

PDFLIB_API const char * PDFLIB_CALL
PDF_get_fontname(PDF *p)
{
    if (p->fonts_number == 0) 	/* no font set */
	pdf_error(p, PDF_RuntimeError,
		"Queried current font name before setting one!");

    return p->fonts[p->current_font].name;
}

PDFLIB_API float PDFLIB_CALL
PDF_get_fontsize(PDF *p)
{
    if (p->fonts_number == 0) {	/* no font set */
	pdf_error(p, PDF_RuntimeError,
		"Queried current font size before setting a font");
	return 0;
    }
    return p->fontsize;
}

/* Bit positions for the font descriptor flag */
#define FIXEDWIDTH	(long) (1L<<0)
#define SERIF		(long) (1L<<1)
#define SYMBOL		(long) (1L<<2)
#define SCRIPT		(long) (1L<<3)
#define ADOBESTANDARD	(long) (1L<<5)
#define ITALIC		(long) (1L<<6)
#define SMALLCAPS	(long) (1L<<17)
#define FORCEBOLD	(long) (1L<<18)

static void
pdf_make_fontflags(PDF *p, pdf_font *font)
{
    font->flags = 0L;

    if (font->isFixedPitch)
    	font->flags |= FIXEDWIDTH;

    if (!strcmp(font->encodingScheme, "AdobeStandardEncoding"))
    	font->flags |= ADOBESTANDARD;
    else
    	font->flags |= SYMBOL;

    if (font->italicAngle < 0)
    	font->flags |= ITALIC;

    /* heuristic to identify (small) caps fonts */
    if (font->name && 
	(strstr(font->name, "Caps") ||
	!strcmp(font->name + strlen(font->name) - 2, "SC")))
    	font->flags |= SMALLCAPS;

    /* this doesn't catch all cases but will do for the moment */
    if (font->weight && !strcmp(font->weight, "Bold"))
    	font->flags |= FORCEBOLD;
}

pdf_bool
pdf_get_metrics_afm(PDF *p, pdf_font *font, const char *fontname, pdf_encoding enc, const char *filename)
{
    FILE	*afmfile;
    int		*widths;
    int		i, j;
    const char	*charname;
    CharMetricInfo *cmi;

    /* open AFM file */
    if ((afmfile = fopen(filename, "r")) == NULL) {
	pdf_error(p, PDF_NonfatalError, "Couldn't open AFM file %s", filename);
	return pdf_false;
    }

    /* parse AFM file */
    if (pdf_parse_afm(p, afmfile, font) != 0) {
	fclose(afmfile);
	pdf_error(p, PDF_NonfatalError, "Error parsing AFM file %s", filename);
	return pdf_false;
    }

    fclose(afmfile);

    if (font->cmi == NULL) {
	pdf_error(p, PDF_NonfatalError, 
		"Couldn't parse char metrics in AFM file %s", filename);
	return pdf_false;
    }

    if (fontname && strcmp(font->name, fontname)) {
	pdf_error(p, PDF_NonfatalError,
		"Font name mismatch in AFM file %s", filename);
	return pdf_false;
    }

    /* 
     * Generate character width array according to chosen encoding vector
     * or the font's default encoding vector.
     */
    if (enc != builtin && 
    	strcmp(font->encodingScheme, "AdobeStandardEncoding")) {
	pdf_error(p, PDF_NonfatalError,
		"Can't reencode Symbol font '%s' (using builtin)", font->name);
	enc = builtin;
    }

    font->encoding = enc;

    widths = font->widths;		/* shortcut */

    if (enc != builtin && pdf_encodings[enc]) {	/* text font */
	for (i = 0; i < 256; i++) {
	    charname = (*pdf_encodings[enc])[i];
	    widths[i] = 250;		/* some reasonable default */
	    if (charname == NULL)	/* unencoded character */
		continue;
	    for (j = 0, cmi = font->cmi; j < font->numOfChars; ++j, ++cmi) {
		if (!strcmp(cmi->name, charname)) {
		    widths[i] = cmi->wx;
		    break;
		}
	    }
	}
    } else {					/* symbol or pi font */
	for (i = 0; i < 256; i++)
		widths[i] = 250;	/* some reasonable default */
	for (i = 0, cmi = font->cmi; i < font->numOfChars; i++, cmi++) {
	    if (cmi->code >= 0 && cmi->code < 256)
		widths[cmi->code] = cmi->wx;
	}
    }

    pdf_make_fontflags(p, font);

    font->afm = pdf_true;

    return pdf_true;
}

void
pdf_cleanup_fonts(PDF *p)
{
    int slot;

    for (slot = 0; slot < p->fonts_number; slot++)
	if (p->fonts[slot].afm)
	    pdf_cleanup_afm(p, &p->fonts[slot]);

    /* p->fontfilename was malloc'ed from the resource machinery
     * and only referenced in the font struct.
     */

    if (p->fonts)
	p->free(p, p->fonts);
}

void
pdf_init_fonts(PDF *p)
{
    int slot;

    p->fonts_number     = 0;
    p->fonts_capacity	= FONTS_CHUNKSIZE;

    p->fonts = (pdf_font *) p->malloc(p,
    		sizeof(pdf_font) * p->fonts_capacity, "PDF_init_fonts");

    for (slot = 0; slot < p->fonts_capacity; slot++)
	p->fonts[slot].afm = pdf_false;
}

static void
pdf_grow_fonts(PDF *p)
{
    int slot;

    p->fonts = (pdf_font *) p->realloc(p, p->fonts,
		sizeof(pdf_font) * 2 * p->fonts_capacity, "pdf_grow_fonts");

    for (slot = p->fonts_capacity; slot < 2 * p->fonts_capacity; slot++)
	p->fonts[slot].afm = pdf_false;

    p->fonts_capacity *= 2;
}

void
pdf_write_page_fonts(PDF *p)
{
    int i, total = 0;

    for (i = 0; i < p->fonts_number; i++)
	if (p->fonts[i].used_on_current_page == 1)
	    total++;

    if (total > 0) {
	(void) fputs("/Font ", p->fp);

	pdf_begin_dict(p);		/* font resource dict */

	for (i = 0; i < p->fonts_number; i++)
	    if (p->fonts[i].used_on_current_page == 1) {
		p->fonts[i].used_on_current_page = 0;	/* reset */
		(void) fprintf(p->fp,"/F%d %ld 0 R\n", i, p->fonts[i].obj_id);
	    }

	pdf_end_dict(p);		/* font resource dict */
    }
}

pdf_bool
pdf_get_metrics_pfm(PDF *p, pdf_font *font, const char *fontname, pdf_encoding enc, const char *filename)
{
    /* Read PFM metrics. NYI */
    pdf_error(p, PDF_SystemError, "PFM file parser not implemented");

    /* TODO: Adapt to PFM logic */
    if (enc != builtin && 
    	strcmp(font->encodingScheme, "AdobeStandardEncoding")) {
	pdf_error(p, PDF_NonfatalError,
		"Can't reencode Symbol font '%'s (using builtin)", font->name);
	enc = builtin;
    }

    font->afm = pdf_false;

    if (strcmp(font->name, fontname)) {
	pdf_error(p, PDF_NonfatalError,
		"Font name mismatch in AFM file %s", filename);
	return pdf_false;
    }

    pdf_make_fontflags(p, font);

    return pdf_false;
}

static pdf_bool
pdf_get_metrics_core(PDF *p, pdf_font *font, const char *fontname, pdf_encoding enc)
{
    int slot;

    font->afm = pdf_false;

    for (slot = 0; slot < SIZEOF_CORE_METRICS; slot++) {
	if (!strcmp(pdf_core_metrics[slot].name, fontname) &&
	    pdf_core_metrics[slot].encoding == enc) {
		memcpy(font, &pdf_core_metrics[slot], sizeof(pdf_font));
		return pdf_true;
	}
    }

    return pdf_false;
}

PDFLIB_API int PDFLIB_CALL
PDF_findfont(PDF *p, const char *fontname, const char *encoding, int embed)
{
    int slot;
    pdf_encoding enc = builtin;
    const char **cp;
    char *filename;

    /* Check parameters. */

    if (fontname == NULL)
	pdf_error(p, PDF_ValueError, "Null fontname");

    if (encoding == NULL)
	pdf_error(p, PDF_ValueError, "Null encoding for font '%s'", fontname);

    if (!strcmp(encoding, "builtin"))
	enc = builtin;
    else if (!strcmp(encoding, "pdfdoc"))
	enc = pdfdoc;
    else if (!strcmp(encoding, "macroman"))
	enc = macroman;
    else if (!strcmp(encoding, "macexpert"))
	enc = macexpert;
    else if (!strcmp(encoding, "winansi"))
	enc = winansi;
    else if (!strcmp(encoding, "default"))	/* special encoding */
#ifdef MAC
	enc = macroman;
#else
	enc = winansi;
#endif
    else
	pdf_error(p, PDF_ValueError, "Bogus encoding '%s' for font '%s'",
		encoding, fontname);

    /* If font is in base 14 set, don't embed regardless of what the 
     * caller said.
     */
    for (cp = pdf_base14_names; *cp; ++cp)
	if (!strcmp(*cp, fontname)) {
	    embed = 0;
	    break;
    }

    /* Look whether font is already in the cache.
     * If font with same encoding and embedding flag is found,
     * return its descriptor.
     */

    for (slot = 0; slot < p->fonts_number; slot++) {
	if (!strcmp(p->fonts[slot].name, fontname) &&
		    p->fonts[slot].encoding == enc)
	    return slot;
    }
    
    /* Grow font cache if necessary.
     * If an error happens and the slot won't be used, we have
     * allocated too early but haven't done any harm.
     */

    if (slot >= p->fonts_capacity)
	pdf_grow_fonts(p);

    /* Try to locate font metrics */
    while(pdf_true) {
	/* Check available in-core metrics */
	if (pdf_get_metrics_core(p, &p->fonts[slot], fontname, enc))
	    break;

	/* Check available AFM files */
	filename = pdf_find_resource(p, "FontAFM", fontname);
	if (filename != NULL &&
	    pdf_get_metrics_afm(p, &p->fonts[slot], fontname, enc, filename))
	    break;

#ifdef NYI
	/* TODO: Check available PFM files */
	filename = pdf_find_resource(p, "FontPFM", fontname);
	if (filename != NULL &&
	    pdf_get_metrics_pfm(p, &p->fonts[slot], fontname, enc, filename))
	    break;
#endif

	return -1;
    }

    /* TODO: compare encodings symbol/text */

    /* If embedding was requested, find font file (or return -1)
     * - Look for font file, using "FontOutline" entries from resource file
     */

    p->fonts[slot].fontfilename	= NULL;

    if (embed) {
	filename = pdf_find_resource(p, "FontOutline", fontname);

	if (filename == NULL)	/* font file not configured */
	    return -1;
	else
	    p->fonts[slot].fontfilename = filename;
    }

    /* Now everything is fine; fill the remaining font cache entries */

    p->fonts[slot].obj_id = pdf_alloc_id(p);
    p->fonts[slot].encoding = enc;
    p->fonts[slot].embed = embed;

    p->fonts_number++;

    /* return valid font descriptor */
    return slot;
}

PDFLIB_API void PDFLIB_CALL
PDF_setfont(PDF *p, int font, float fontsize)
{
    char buf[FLOATBUFSIZE];

    /* Check parameters */
    if (font < 0 || font >= p->fonts_number)
	pdf_error(p, PDF_ValueError, "Bad font descriptor %d", font);
	
    if (fontsize < 0)
	pdf_error(p, PDF_ValueError, "Bad font size %f", fontsize);

    /* make font the current font */

    p->fonts[font].used_on_current_page = 1;
    p->fontsize = fontsize;
    p->current_font = font;

    pdf_begin_text(p);
    (void) fprintf(p->fp, "/F%d %s Tf\n", font, pdf_float(buf, fontsize));
    PDF_set_leading(p, fontsize);
}

#ifdef NYI
PDFLIB_API void PDFLIB_CALL
PDF_get_metrics(PDF *p, int font, char *text)
{
    /* Check parameters */
}
#endif

/* Obsolete function, use PDF_findfont() and PDF_setfont() instead. */

PDFLIB_API void PDFLIB_CALL
PDF_set_font(PDF *p, const char *fontname, float fontsize, const char *encoding)
{
    int font;

    font = PDF_findfont(p, fontname, encoding, 0);

    if (font == -1)
	pdf_error(p, PDF_IOError, "Couldn't set font '%s'", fontname);

    PDF_setfont(p, font, fontsize);
}

static void
t1data_init(PDF *p, PDF_data_source *src)
{
    t1_private_data *t1_private;

    t1_private = (t1_private_data *) src->private_data;

    t1_private->portion = ascii;
    t1_private->length1 = 0;
    t1_private->length2 = 0;
    t1_private->length3 = 0;

    src->buffer_start = (byte *) p->malloc(p, LINEBUFLEN,"t1data_init");
    src->buffer_length = LINEBUFLEN;
}

static int
t1data_fill(PDF *p, PDF_data_source *src)
{
    static const char HexToBin[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0,
	10, 11, 12, 13, 14, 15 }; 
    char *s, *c;
    int i;
    t1_private_data *t1_private;

    t1_private = (t1_private_data *) src->private_data;

    if ((s = fgets((char *) src->buffer_start, LINEBUFLEN, 
    	t1_private->fontfile))== NULL)
	return pdf_false;

    /* check for line of zeros: set zero flag if found */
    for (c = s; *c == '0'; c++)
	;

    if (*c == '\n')
	t1_private->portion = zeros;

    src->next_byte = src->buffer_start;

    switch (t1_private->portion) {
	case ascii:
	    t1_private->length1 += strlen(s);
	    src->bytes_available = strlen(s);
	    break;

	case encrypted:
	    t1_private->length2 += strlen(s)/2;	/* ASCII -> binary */
	    src->bytes_available = strlen(s)/2;

	    /* Convert to upper case for safe binary conversion */
	    for (c = s; *c != '\n'; c++)
		    *c = (char) toupper(*c);

	    /* convert ASCII to binary in-place */
	    for (i=0; s[i] != '\n'; i += 2)
	    {
		if (s[i] < '0' || s[i] > 'F' || s[i+1] < '0' || s[i+1] > 'F')
		    pdf_error(p, PDF_IOError, 
			"Bogus Type 1 font ASCII data (0x%02X, 0x%02X)",
			s[i], s[i+1]);
		s[i/2] = (char) (16*HexToBin[s[i]-'0'] + HexToBin[s[i+1]-'0']);
	    }
	    break;

	case zeros:
	    t1_private->length3 += strlen(s);
	    src->bytes_available = strlen(s);
	    break;
    }

    if (t1_private->portion != encrypted &&
		!strncmp((const char *)s, "currentfile eexec", 17))
	t1_private->portion = encrypted;

    return pdf_true;
}

static void
t1data_terminate(PDF *p, PDF_data_source *src)
{
    p->free(p, (void *) src->buffer_start);
}

static void
pdf_put_t1font(PDF *p, int slot)
{
    FILE	*fontfile = NULL;
    int		i, j;
    id		fontdescriptor_id, fontfile_id = 0l;
    id		length_id, length1_id, length2_id, length3_id;
    long	length, stream_start;
    PDF_data_source t1src;
    const char	**cp;
    pdf_font	*font;

    font	= &p->fonts[slot];

    /* check whether we have one of the base 14 fonts */
    for (cp = pdf_base14_names; *cp; ++cp)
	if (!strcmp(*cp, font->name)) {
	    pdf_begin_obj(p, font->obj_id);	/* font */
	    pdf_begin_dict(p);

	    (void) fputs("/Type /Font\n", p->fp);
	    (void) fputs("/Subtype /Type1\n", p->fp);
	    (void) fprintf(p->fp, "/Name /F%d\n", slot);

	    if (font->encoding != builtin) {
		(void) fprintf(p->fp, "/Encoding /%s\n", 
			    pdf_encoding_names[font->encoding]);
	    }
	    (void) fprintf(p->fp, "/BaseFont /%s\n", *cp);

	    pdf_end_dict(p);
	    pdf_end_obj(p);			/* font */

	    return;
	}

    /* check for font file */
    if (font->embed) {
	fontfile = fopen(font->fontfilename, "r");

	if (fontfile == NULL)
	    pdf_error(p, PDF_IOError, "Couldn't open font file '%s'",
	    			font->fontfilename);
    }

    pdf_begin_obj(p, font->obj_id);		/* font attributes */
    pdf_begin_dict(p);
    (void) fputs("/Type /Font\n", p->fp);
    (void) fputs("/Subtype /Type1\n", p->fp);
    (void) fprintf(p->fp, "/Name /F%d\n", slot);
    (void) fputs("/FirstChar 0\n", p->fp);
    (void) fputs("/LastChar 255\n", p->fp);

    (void) fputs("/Widths [\n", p->fp);

    for (i = 0; i < 16; i++) {
	for (j = 0; j < 16; j++)
	    (void) fprintf(p->fp, " %d", font->widths[16*i + j]);
	(void) fputs("\n", p->fp);
    }

    (void) fputs("]\n", p->fp);

    if (font->encoding != builtin) {
	(void) fprintf(p->fp, "/Encoding /%s\n", 
		    pdf_encoding_names[font->encoding]);
    }
    (void) fprintf(p->fp, "/BaseFont /%s\n", font->name);

    fontdescriptor_id = pdf_alloc_id(p);
    (void) fprintf(p->fp, "/FontDescriptor %ld 0 R\n", fontdescriptor_id);

    pdf_end_dict(p);
    pdf_end_obj(p);			/* font attributes */

    /* font descriptors */
    pdf_begin_obj(p, fontdescriptor_id);
    pdf_begin_dict(p);
    (void) fputs("/Type /FontDescriptor\n", p->fp);
    (void) fprintf(p->fp, "/Ascent %d\n", font->ascender);
    (void) fprintf(p->fp, "/CapHeight %d\n", font->capHeight);
    (void) fprintf(p->fp, "/Descent %d\n", font->descender);
    (void) fprintf(p->fp, "/Flags %ld\n", font->flags);
    (void) fprintf(p->fp, "/FontBBox [%d %d %d %d ]\n",
		    font->llx, font->lly, font->urx, font->ury);

    (void) fprintf(p->fp, "/FontName /%s\n", font->name);
    (void) fprintf(p->fp, "/ItalicAngle %d\n", (int) (font->italicAngle));
    (void) fprintf(p->fp, "/StemV %d\n", font->StdVW);

    if (font->embed) {
	fontfile_id = pdf_alloc_id(p);
	(void) fprintf(p->fp, "/FontFile %ld 0 R\n", fontfile_id);
    }

    pdf_end_dict(p);
    pdf_end_obj(p);		/* font descriptor */

    /* font files */
    if (font->embed) {
	pdf_begin_obj(p, fontfile_id);
	pdf_begin_dict(p);

	if (p->debug['a'])
	    (void) fputs("/Filter /ASCIIHexDecode\n", p->fp);

	length_id = pdf_alloc_id(p);
	length1_id = pdf_alloc_id(p);
	length2_id = pdf_alloc_id(p);
	length3_id = pdf_alloc_id(p);
	(void) fprintf(p->fp, "/Length %ld 0 R\n", length_id);
	(void) fprintf(p->fp, "/Length1 %ld 0 R\n", length1_id);
	(void) fprintf(p->fp, "/Length2 %ld 0 R\n", length2_id);
	(void) fprintf(p->fp, "/Length3 %ld 0 R\n", length3_id);

	/* Don't omit zeros since more data may follow at the end! */
	pdf_end_dict(p);

	pdf_begin_stream(p);			/* font data stream */
	stream_start = ftell(p->fp);

	t1src.init		= t1data_init;
	t1src.fill		= t1data_fill;
	t1src.terminate		= t1data_terminate;

	t1src.private_data = (unsigned char *)
		p->malloc(p, sizeof(t1_private_data), "pdf_put_t1font");
	((t1_private_data *) t1src.private_data)->fontfile = fontfile;

	if (p->debug['a']) {
	    pdf_ASCIIHexEncode(p, &t1src);
	} else {
	    t1src.init(p, &t1src);

	    while (t1src.fill(p, &t1src))
		(void) fwrite(t1src.next_byte, 1, t1src.bytes_available, p->fp);

	    t1src.terminate(p, &t1src);
	}

	length = ftell(p->fp) - stream_start;
	pdf_end_stream(p);
	pdf_end_obj(p);				/* font file object */

	pdf_begin_obj(p, length_id);		/* Length object */
	(void) fprintf(p->fp,"%ld\n", length);
	pdf_end_obj(p);

	pdf_begin_obj(p, length1_id);		/* Length1 object */
	(void) fprintf(p->fp,"%ld\n", 
		((t1_private_data *) t1src.private_data)->length1);
	pdf_end_obj(p);

	pdf_begin_obj(p, length2_id);		/* Length2 object */
	(void) fprintf(p->fp,"%ld\n",
		((t1_private_data *) t1src.private_data)->length2);
	pdf_end_obj(p);

	pdf_begin_obj(p, length3_id);		/* Length3 object */
	(void) fprintf(p->fp,"%ld\n",
		((t1_private_data *) t1src.private_data)->length3);
	pdf_end_obj(p);

	fclose(((t1_private_data *) t1src.private_data)->fontfile);
	p->free(p, (void *) t1src.private_data);
    }
}

void
pdf_write_doc_fonts(PDF *p)
{
    int	i;

    /* output pending font objects */
    for (i = 0; i < p->fonts_number; i++)
	pdf_put_t1font(p, i);
}
