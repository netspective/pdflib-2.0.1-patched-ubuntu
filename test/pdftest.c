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

/* pdftest.c
 *
 * Test bed and sample application for PDFlib
 */

#ifdef WIN32
    /* Visual C++ seems to need this in order to fetch the thread functions */
    #ifndef _MT
	#define _MT
    #endif

    #include <windows.h>
    #include <process.h>
#endif	/* WIN32 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "pdflib.h"

#if __POWERPC__ || __CFM68K__ || __MC68K__
#define MAC
#endif

/* Don't clutter output from many threads with messages */
#ifdef WIN32
#define MESSAGE(m)		/* */
#define PDF_ENDTHREAD(val)	_endthreadex(val);
#else
#define MESSAGE(m)		fprintf(stderr, m)
#define PDF_ENDTHREAD(val)	/* */
#endif

/* ------------------------------------------------------------- */
static void
bookmarks(PDF *p)
{
#define UNICODEINFO    "\xFE\xFF\0001\000.\000A\0\0"
#define LEFT 50
#define FONTSIZE 24
#define LEAD ((int) (FONTSIZE * 1.5))

    unsigned char buf[64], tmp[64];
    int c, i, pos;
    float y=700;
    int level1, level2=0, level3=0, font;

    MESSAGE("Bookmark test...");

    font = PDF_findfont(p, "Helvetica", "default", 0);
    if (font == -1) {
	fprintf(stderr, "\nFont %s not found!\n", "Helvetica");
	return;
    }

    PDF_begin_page(p, a4_width, a4_height);

    PDF_setfont(p, font, FONTSIZE);

    PDF_show_xy(p, "The bookmarks for this page contain all", LEFT, y-=LEAD);
    PDF_show_xy(p, "Unicode characters.", LEFT, y-=LEAD);
    PDF_show_xy(p, "Depending on the available fonts,", LEFT, y-=LEAD);
    PDF_show_xy(p, "only a certain subset will be visible on", LEFT, y-=LEAD);
    PDF_show_xy(p, "your system.", LEFT, y-=LEAD);
    
    /* private Unicode info entry */
    PDF_set_info(p, "Revision", UNICODEINFO);

    /* Generate Unicode bookmarks */
    level1 = PDF_add_bookmark(p, "Unicode bookmarks", 0, 0);

    for (c = 0; c < 65535; c += 16) {
	if (c % 4096 == 0) {
	    sprintf((char *) tmp, "U+%04X", c);
	    level2 = PDF_add_bookmark(p, (char *) tmp, level1, 0);
	}
	if (c % 256 == 0) {
	    sprintf((char *) tmp, "U+%04X", c);
	    level3 = PDF_add_bookmark(p, (char *) tmp, level2, 0);
	}
	/* now comes the actual Unicode string with the BOM */
	sprintf((char *) tmp, "0x%04X: ", c);

	/* write the Unicode byte order mark */
	strcpy((char *) buf, "\376\377");

	for (pos = 0; tmp[pos/2]; /* */ ) {
	    buf[2+pos++] = 0;
	    buf[2+pos++] = tmp[pos/2];	/* construct Unicode string */
	}

	pos += 2; 	/* account for the BOM */
	for (i = 0; i < 16; i++) {	/* continue filling buf with chars */
	    buf[pos++] = (unsigned char) ((((c+i)) & 0xFF00) >> 8);
	    buf[pos++] = (unsigned char) (((c+i)) & 0x00FF);
	}

	/* signal end of string with two null bytes */
	buf[pos++] = (unsigned char) 0;
	buf[pos++] = (unsigned char) 0;

	(void) PDF_add_bookmark(p, (char *) buf, level3, 1);
    }

    PDF_end_page(p);

    MESSAGE("done\n");

#undef FONTSIZE
#undef LEFT
#undef UNICODEINFO
#undef LEAD
}

/* ------------------------------------------------------------- */
static void
ccitt_image(PDF *p)
{
    int	image;
    float	sx, sy;

#define CCITTFILE	"tm.g3"
#define WIDTH		591
#define HEIGHT		236

/* Scale to non-standard fax resolutions */
#define FAX_DPI_X	72
#define FAX_DPI_Y	72

    sx = (float) (72.0 / FAX_DPI_X);
    sy = (float) (72.0 / FAX_DPI_Y);

    MESSAGE("CCITT test...");

    if ((image = PDF_open_CCITT(p, CCITTFILE, WIDTH, HEIGHT, 0, 0, 0)) == -1) {
	fprintf(stderr, "Error: Couldn't open CCITT image %s.\n", CCITTFILE);
	return;
    }

    PDF_begin_page(p, (float) PDF_get_image_width(p, image) * sx,
			(float) PDF_get_image_height(p, image) * sy);

    (void) PDF_add_bookmark(p, "CCITT image", 0, 1);

    PDF_scale(p, sx, sy);
    PDF_place_image(p, image, (float) 0.0, (float) 0.0, (float) 1.0);

    PDF_end_page(p);

    MESSAGE("done\n");

#undef CCITTFILE
}

/* ------------------------------------------------------------- */
static void
gif_image(PDF *p)
{
    int	image;

#define GIFFILE		"pdflib.gif"

    MESSAGE("GIF test...");

    if ((image = PDF_open_GIF(p, GIFFILE)) == -1) {
	fprintf(stderr, "Error: Couldn't analyze GIF image %s.\n", GIFFILE);
	return;
    }

    PDF_begin_page(p, (float) PDF_get_image_width(p, image),
    			(float) PDF_get_image_height(p, image));
    (void) PDF_add_bookmark(p, "GIF image", 0, 1);

    PDF_place_image(p, image, (float) 0.0, (float) 0.0, (float) 1.0);

    PDF_end_page(p);

    MESSAGE("done\n");

#undef GIFFILE
}

/* ------------------------------------------------------------- */
#ifdef HAVE_LIBTIFF
static void
tiff_image(PDF *p)
{
    int	image;

#define TIFFFILE	"acroweb_j.tif"

    MESSAGE("TIFF test...");

    if ((image = PDF_open_TIFF(p, TIFFFILE)) == -1) {
	fprintf(stderr, "Error: Couldn't analyze TIFF image %s.\n", TIFFFILE);
	return;
    }

    PDF_begin_page(p, PDF_get_image_width(p, image), PDF_get_image_height(p, image));
    (void) PDF_add_bookmark(p, "TIFF image", 0, 1);

    PDF_place_image(p, image, 0.0, 0.0, 1.0);

    PDF_end_page(p);

    MESSAGE("done\n");

#undef TIFFFILE
}
#endif

/* ------------------------------------------------------------- */
static void
jpeg_image(PDF *p)
{
    int	image;
    float	scale;

#define JPEGFILE	"nesrin.jpg"

    MESSAGE("JPEG test...");

    if ((image = PDF_open_JPEG(p, JPEGFILE)) == -1) {
	fprintf(stderr, "Error: Couldn't analyze JPEG image %s.\n", JPEGFILE);
	return;
    }

    PDF_begin_page(p, a4_width, a4_height);
    (void) PDF_add_bookmark(p, "JPEG image", 0, 1);

    /* ----------------- first image ------------------- */
    /* fit image to page width */
    scale = (float) a4_width/PDF_get_image_width(p, image);
    PDF_place_image(p, image, (float) 0.0, 
    	a4_height - PDF_get_image_height(p, image) * scale, scale);

    /* ----------------- second image ------------------- */
    scale = (float) 0.5;
    PDF_save(p);
    PDF_rotate(p, (float) 90.0);
    PDF_place_image(p, image, (float) 0, (float) (-1 * a4_width), scale);
    PDF_restore(p);

    /* ----------------- third image ------------------- */
    scale = (float) 0.25;
    PDF_save(p);
    PDF_rotate(p, (float) 45.0);
    PDF_place_image(p, image, (float) 200, (float) 0.0, scale);
    PDF_restore(p);

    PDF_end_page(p);

    MESSAGE("done\n");

#undef JPEGFILE
}

/* ------------------------------------------------------------- */
static void
character_table(PDF *p)
{
    char	text[50];
    int		i, j, font;
    float	x, y;

#define LEFT		50
#define TOP		700
#define FONTSIZE	16
#define FONTNAME	"Times-Roman"

#ifdef MAC
    #define ENCODING	"macroman"
#else
    #define ENCODING	"winansi"
#endif

    MESSAGE("Character encoding test...");

    font = PDF_findfont(p, FONTNAME, ENCODING, 1);
    if (font == -1) {
	fprintf(stderr, "Font not found!\n");
	return;
    }

    PDF_begin_page(p, a4_width, a4_height);

#ifdef MAC
    (void) PDF_add_bookmark(p, "MacOS Encoding", 0, 1);

    PDF_setfont(p, font, 2*FONTSIZE);
    PDF_show_xy(p, "MacOS Encoding (no Symbol characters)",
    		LEFT, TOP+2*FONTSIZE);
#else
    (void) PDF_add_bookmark(p, "Windows Encoding", 0, 1);

    PDF_setfont(p, font, 2*FONTSIZE);
    PDF_show_xy(p, "Windows Encoding", LEFT, TOP+2*FONTSIZE);
#endif

    PDF_setfont(p, font, FONTSIZE);
    text[1] = 0;

    y = TOP;
    for (i = 2; i < 16; i++) {
	y -=  2*FONTSIZE;
	x = LEFT;
	for (j = 0; j < 16; j++) {
	    text[0] = (char) (i*16 + j);
	    PDF_show_xy(p, text, x, y);
	    x += 2*FONTSIZE;
	}
    }

    PDF_end_page(p);

    MESSAGE("done\n");

#undef LEFT
#undef TOP
#undef FONTSIZE
#undef FONTNAME
}

/* ------------------------------------------------------------- */
static void
grid(PDF *p)
{
#define STEP		10
#define FONTSIZE	((float) 10.0)
#define THICK		((float) 1.0)
#define THIN		((float) 0.01)

    char buf[10];
    float i;
    float width = a4_width, height = a4_height;
    int font;

    MESSAGE("Grid test...");

    PDF_begin_page(p, width, height);
    (void) PDF_add_bookmark(p, "Grid", 0, 1);

    PDF_setlinewidth(p, THIN);
    PDF_setdash(p, (float) 1.0, (float) 2.0);

    /* draw vertical lines */
    for (i = 0; i < width; i += STEP) {
	PDF_save(p);
	if ((int) i % 100 == 0)
	    PDF_setlinewidth(p, THICK);
	if ((int) i % 50 == 0)
	    PDF_setdash(p, (float) 0.0, (float) 0.0);
	PDF_moveto(p, i, 0);
	PDF_lineto(p, i, (float) height);
	PDF_stroke(p);
	PDF_restore(p);
    }

    /* draw horizontal lines */
    for (i = 0; i < height; i += STEP) {
	PDF_save(p);
	if ((int) i % 50 == 0)
	    PDF_setdash(p, (float) 0.0, (float) 0.0);
	if ((int) i % 100 == 0)
	    PDF_setlinewidth(p, THICK);
	PDF_moveto(p, 0, i);
	PDF_lineto(p, (float) width, i);
	PDF_stroke(p);
	PDF_restore(p);
    }

#define FONTNAME	"Helvetica-Bold"
#define DELTA	((float) 9)
#define RADIUS	((float) 12.0)

    font = PDF_findfont(p, FONTNAME, "default", 0);

    if (font == -1)
	fprintf(stderr, "Font %s not found!\n", FONTNAME);

    PDF_setfont(p, font, FONTSIZE);

    /* print captions */
    for (i = 100; i < width; i+= 100) {
	PDF_save(p);
	PDF_setgray_fill(p, (float) 1.0);
	PDF_circle(p, i, (float) 20.0, RADIUS);
	PDF_fill(p);
	PDF_restore(p);
	sprintf(buf, "%d", (int) i);
	PDF_show_xy(p, buf, i - DELTA, (float) 20.0 - FONTSIZE/3);
    }

    for (i = 100; i < height; i+= 100) {
	PDF_save(p);
	PDF_setgray_fill(p, (float) 1.0);
	PDF_circle(p, (float) 40.0, i, RADIUS);
	PDF_fill(p);
	PDF_restore(p);
	sprintf(buf, "%d", (int) i);
	PDF_show_xy(p, buf, (float) 40.0 - DELTA, i - FONTSIZE/3);
    }

    PDF_end_page(p);

    MESSAGE("done\n");

#undef STEP
#undef FONTSIZE
#undef DELTA
#undef RADIUS
#undef FONTNAME
}


/* ------------------------------------------------------------- */
static void
shaded_circle(PDF *p)
{
    int i, step = 1;
    float gray = (float) 0.1, r;

    MESSAGE("Shaded circle test...");

    /* generate perceptual linear color blend */
    r = (float) pow(1.0/gray, 1.0/255.0);

    PDF_begin_page(p, a4_width, a4_height);
    (void) PDF_add_bookmark(p, "Shaded circle", 0, 1);

    for (i = 255; i >= 0; i--) {
	PDF_setrgbcolor_fill(p, gray, gray, (float) 1.0);
	PDF_circle(p, (float) 300.0, (float) 400.0, (float) i);
	PDF_fill(p);
	gray *= r;
    }
    PDF_end_page(p);

    MESSAGE("done\n");
}


/* ------------------------------------------------------------- */
static void
annotations(PDF *p)
{
#define FILENAME	"pdftest.c"
#define MIMETYPE	"text/plain"
#define NOTETEXT	"Hi! I'm a text annotation!"

/* Some greek and russian words for the unicode annotation test */
#define GREEKTEXT	"\xFE\xFF\003\233\003\237\003\223\003\237\003\243\0\0"
#define RUSSIANTEXT	"\xFE\xFF\004\037\004\036\004\024\004\040\004\043\004\023\004\020\0\0"

    MESSAGE("Annotation test...");

    PDF_begin_page(p, a4_width, a4_height);

    (void) PDF_add_bookmark(p, "Annotations", 0, 1);

    (void) PDF_attach_file(p, 100, 200, 150, 250, FILENAME,
    		"C source file", "Thomas Merz", MIMETYPE, "graph");

    PDF_set_border_style(p, "dashed", (float) 3.0);
    PDF_set_border_color(p, (float) 0.0, (float) 0.0, (float) 1.0);
    PDF_set_border_dash(p, (float) 5.0, (float) 1.0);

    PDF_add_note(p, 200, 400, 300, 450, NOTETEXT, "Thomas Merz", "comment", 0);

    PDF_set_border_style(p, "dashed", (float) 4.0);
    PDF_set_border_color(p, (float) 1.0, (float) 0.0, (float) 0.0);
    PDF_set_border_dash(p, (float) 1.0, (float) 4.0);

    PDF_add_note(p, 200, 600, 550, 750, GREEKTEXT, "Greek annotation",
    		"insert",1);

    PDF_add_note(p, 100, 500, 400, 650, RUSSIANTEXT,
    		"Russian annotation", "paragraph", 1);

    PDF_add_launchlink(p, 300, 300, 400, 350, "foo.txt");

    PDF_set_border_color(p, (float) 0.0, (float) 1.0, (float) 0.0);
    PDF_set_border_dash(p, (float) 3.0, (float) 2.0);

    PDF_add_pdflink(p, 400, 200, 500, 250, "cell.pdf", 2, "fitpage");

    PDF_set_border_color(p, (float) 1.0, (float) 1.0, (float) 0.0);
    PDF_set_border_dash(p, (float) 3.0, (float) 2.0);

    PDF_add_locallink(p, 500, 100, 550, 150, 8, "fitwidth");

    PDF_set_border_style(p, "solid", (float) 7.0);
    PDF_set_border_color(p, (float) 0.0, (float) 1.0, (float) 1.0);
    PDF_set_border_dash(p, (float) 0.0, (float) 0.0);

    PDF_add_weblink(p, 100, 150, 300, 250, "http://malik.piripiri.de");

    PDF_end_page(p);

    MESSAGE("done\n");

#undef NOTETEXT
#undef FILENAME
#undef MIMETYPE
#undef RUSSIANTEXT
#undef GREEKTEXT
}

/* ------------------------------------------------------------- */
static void
centered_text(PDF *p)
{
    float	y, width;
    char	**cp;
    int		textfont, titlefont;
    char	*title = "Der Zauberlehrling";
    char	*subtitle ="Johann Wolfgang von Goethe";
#ifndef MAC
    char	*poem[] = {
		    "Hat der alte Hexenmeister",
		    "Sich doch einmal wegbegeben!",
		    "Und nun sollen seine Geister",
		    "Auch nach meinem Willen leben.",
		    "Seine Wort\222 und Werke",
		    "Merkt ich und den Brauch,",
		    "Und mit Geistesst\344rke",
		    "Tu ich Wunder auch.",
		    "Walle! walle",
		    "Manche Strecke,",
		    "Da\337, zum Zwecke,",
		    "Wasser flie\337e",
		    "Und mit reichem, vollem Schwalle",
		    "Zu dem Bade sich ergie\337e.",
		    NULL };
#else
/* We don't want 8-bit characters in the source, therefore different
 * flavors of the sample text will be used for Mac and non-Mac systems.
 * These only differ in the special characters (octal notation).
 */
    char	*poem[] = {
		    "Hat der alte Hexenmeister",
		    "Sich doch einmal wegbegeben!",
		    "Und nun sollen seine Geister",
		    "Auch nach meinem Willen leben.",
		    "Seine Wort\325 und Werke",
		    "Merkt ich und den Brauch,",
		    "Und mit Geistesst\212rke",
		    "Tu ich Wunder auch.",
		    "Walle! walle",
		    "Manche Strecke,",
		    "Da\247, zum Zwecke,",
		    "Wasser flie\247e",
		    "Und mit reichem, vollem Schwalle",
		    "Zu dem Bade sich ergie\247e.",
		    NULL };
#endif

#define FONTSIZE	24
#define CENTER		290
#define TOP		750

    MESSAGE("Centered text test...");

    PDF_begin_page(p, a4_width, a4_height);
    (void) PDF_add_bookmark(p, "Centered text", 0, 1);

    textfont = PDF_findfont(p, "Times-Roman", ENCODING, 1);
    if (textfont == -1) {
	fprintf(stderr, "Font 'Times-Roman' not found!\n");
	return;
    }

    y = TOP;

    titlefont = PDF_findfont(p, "Helvetica-Bold", ENCODING, 1);
    if (titlefont == -1) {
	fprintf(stderr, "Font 'Helvetica-Bold' not found!\n");
	return;
    }

    PDF_setfont(p, titlefont, (float) (1.5 * FONTSIZE));
    width = PDF_stringwidth(p, title, textfont, PDF_get_fontsize(p));
    PDF_show_xy(p, title, CENTER - width/2, y);
    y -= 1.5 * FONTSIZE;

    PDF_setfont(p, titlefont, FONTSIZE/2);
    width = PDF_stringwidth(p, subtitle, textfont, PDF_get_fontsize(p));
    PDF_show_xy(p, subtitle, CENTER - width/2, y);
    y -= 3 * FONTSIZE;

    PDF_setfont(p, textfont, FONTSIZE);

    for (cp = poem; *cp; cp++) {
	width = PDF_stringwidth(p, *cp, textfont, PDF_get_fontsize(p));
	PDF_show_xy(p, *cp, CENTER - width/2, y);
	y -= 1.5 * FONTSIZE;
    }

    PDF_end_page(p);

    MESSAGE("done\n");

#undef FONTSIZE
#undef RIGHT
#undef TOP
}

/* ------------------------------------------------------------- */
static void
memory_image(PDF *p)
{
    char 	*buf, *bp;
    int		width, height, components;
    int		bpc;
    int		image;
    int		i, j;
    float	y, sx, sy;

#define LEFT		((float) 50.0)

    width	= 256;
    height	= 1;
    bpc		= 8;
    components	= 3;

    MESSAGE("Memory image test...");

    sx = (float) 3.0;		/* desired horizontal scaling factor */
    sy = (float) 128.0;		/* desired height of one color band */

    buf = calloc((size_t) (width * height * components), 1);

    if (buf == NULL) {
	fprintf(stderr, "Not enough memory for memory image!\n");
	return;
    }

    /* now fill the buffer with fake image data (simple color ramp) */
    for (bp = buf, i=0; i<height; i++) {
	for (j=0; j<width; j++) {
	    *bp++ = (char) (j % 256);	/* red blend */
	    *bp++ = 0;
	    *bp++ = 0;
	}
    }

    /* 
     * In positioning the images below, we will have to compensate 
     * for the scaling.
     */
    y = LEFT;

    image = PDF_open_image(p, "raw", "memory", buf, width*height*components,
    	width, height, components, bpc, "");
    if (image == -1) {
	fprintf(stderr, "Not enough memory for memory image!\n");
	free(buf);
	return;
    }

    PDF_begin_page(p, 900, 600);
    (void) PDF_add_bookmark(p, "Memory image", 0, 1);

    /* 
     * Since the image interface doesn't support non-proportional
     * scaling, we will use PDF_scale() instead to stretch the image.
     */
    PDF_scale(p, sx, sy);		/* stretch image */

    PDF_place_image(p, image, LEFT/sx, y/sy, (float) 1.0);

    for (bp = buf, i=0; i<height; i++) {
	for (j=0; j<width; j++) {
	    *bp++ = 0;
	    *bp++ = (char) (j % 256);	/* green blend */
	    *bp++ = 0;
	}
    }

    y += height * sy;		/* position the image */

    image = PDF_open_image(p, "raw", "memory", buf, width*height*components,
    	width, height, components, bpc, "");
    if (image == -1) {
	fprintf(stderr, "Not enough memory for memory image!\n");
	free(buf);
	PDF_end_page(p);
	return;
    }

    PDF_place_image(p, image, LEFT/sx, y/sy, (float) 1.0);

    for (bp = buf, i=0; i<height; i++) {
	for (j=0; j<width; j++) {
	    *bp++ = 0;
	    *bp++ = 0;
	    *bp++ = (char) (j % 256);	/* blue blend */
	}
    }

    y += height * sy;		/* position the image */

    image = PDF_open_image(p, "raw", "memory", buf, width*height*components,
    	width, height, components, bpc, "");
    if (image == -1) {
	fprintf(stderr, "Not enough memory for memory image!\n");
	free(buf);
	PDF_end_page(p);
	return;
    }

    PDF_place_image(p, image, LEFT/sx, y/sy, (float) 1.0);

    for (bp = buf, i=0; i<height; i++) {
	for (j=0; j<width; j++) {
	    *bp++ = (char) (j % 256);	/* gray blend */
	}
    }

    y += height * sy;		/* position the image */

    components	= 1;		/* now a single component image */
    image = PDF_open_image(p, "raw", "memory", buf, width*height*components,
    	width, height, components, bpc, "");
    if (image == -1) {
	fprintf(stderr, "Not enough memory for memory image!\n");
	free(buf);
	PDF_end_page(p);
	return;
    }

    PDF_place_image(p, image, LEFT/sx, y/sy, (float) 1.0);

    free(buf);
    PDF_end_page(p);

    MESSAGE("done\n");

#undef LEFT
}

/* ------------------------------------------------------------- */
static void
radial_structure(PDF *p)
{
    float alpha;

    MESSAGE("Radial structure test...");

    PDF_begin_page(p, a4_width, a4_height);
    (void) PDF_add_bookmark(p, "Radial structure", 0, 1);

    PDF_translate(p, (float) 300.0, (float) 400.0);
    PDF_setlinewidth(p, (float) 0.1);

#ifdef NOT_EXACT
    /* 
     * Mathematically, the following are equivalent. However, due
     * to massive accumulation of rounding errors the first variant
     * produces visible inaccuracy artifacts (try it!)
     */
    for (alpha = 0; alpha < 360; alpha++) {
	PDF_moveto(p, 0.0, 0.0);
	PDF_lineto(p, 250.0, 0.0);
	PDF_stroke(p);
	PDF_rotate(p, 1.0);
    }
#endif

    /* better solution: don't accumulate rounding errors */
    for (alpha = 0; alpha < 360; alpha++) {
	PDF_save(p);
	PDF_rotate(p, alpha);
	PDF_moveto(p, (float) 0.0, (float) 0.0);
	PDF_lineto(p, (float) 250.0, (float) 0.0);
	PDF_stroke(p);
	PDF_restore(p);
    }

    PDF_end_page(p);

    MESSAGE("done\n");
}

/* ------------------------------------------------------------- */
static void
random_data_graph(PDF *p)
{
    float x;

    MESSAGE("Random graph test...");

    PDF_begin_page(p, a4_width, a4_height);
    (void) PDF_add_bookmark(p, "Random graph", 0, 1);

#define STEP	((float) 10.0)
#define MARGIN	((float) 50.0)
#define RIGHT	((float) 500.0)
#define TOP	((float) 800.0)
    PDF_setlinewidth(p, 2);
    PDF_moveto(p, RIGHT, MARGIN);
    PDF_lineto(p, MARGIN, MARGIN);
    PDF_lineto(p, MARGIN, TOP);
    PDF_setgray_stroke(p, 0);		/* black */
    PDF_stroke(p);

    PDF_setlinewidth(p, 1);
    PDF_moveto(p, MARGIN, MARGIN);

    /* construct some random graph data */
    PDF_setrgbcolor_stroke(p, (float) 1.0, (float) 0, (float) 0);/* red */
    for (x=MARGIN; x<RIGHT; x+=STEP)
	PDF_lineto(p, x, x + (TOP-MARGIN)/(float)2.0*rand()/(RAND_MAX+(float)1.0));

    PDF_stroke(p);

    PDF_setrgbcolor_stroke(p, (float) 0, (float) 1.0, (float) 0);/* green */
    PDF_moveto(p, MARGIN, MARGIN);
    for (x=MARGIN; x<RIGHT; x+=STEP)
	PDF_lineto(p, x, 
	    MARGIN + TOP*(x-MARGIN)*(x-MARGIN)/((RIGHT-MARGIN)*(RIGHT-MARGIN)));

    PDF_stroke(p);

    PDF_setrgbcolor_stroke(p, (float) 0, (float) 0, (float) 1.0);/* blue */
    PDF_moveto(p, MARGIN, MARGIN);
    for (x=MARGIN; x<RIGHT; x+=STEP)
	PDF_lineto(p, x, MARGIN + x + MARGIN*rand()/(RAND_MAX+(float)1.0));

    PDF_stroke(p);

    PDF_end_page(p);

    MESSAGE("done\n");

#undef STEP
#undef MARGIN
#undef RIGHT
#undef TOP
}

/* ------------------------------------------------------------- */
static void
many_pages(PDF *p)
{
    char buf[128];
    int i, font;

#define FONTSIZE	24
#define FONTNAME	"Times-Roman"
#define PAGECOUNT	1000

    MESSAGE("Many pages test...");

    for (i = 1; i <= PAGECOUNT; i++) {
	PDF_begin_page(p, a4_width, a4_height);

	if (i == 1)
	    (void) PDF_add_bookmark(p, "Many pages", 0, 1);

	font = PDF_findfont(p, FONTNAME, "default", 1);
	if (font == -1) {
	    fprintf(stderr, "Font %s not found!\n", FONTNAME);
	    return;
	}
	PDF_setfont(p, font, FONTSIZE);

	sprintf(buf, "Page %d", i);
	PDF_show_xy(p, buf, 200, 600);

	PDF_end_page(p);
    }

    MESSAGE("done\n");
}

static unsigned
#ifdef WIN32
__stdcall		/* needed for threads */
#endif
Do_PDFlib_tests(void *arg)
{
    char	filename[50];
    PDF		*p;

    sprintf(filename, "pdftest%03ld.pdf", (long) arg);

    printf("Creating PDFlib test file '%s'!\n", filename);

    p = PDF_new();

    if (PDF_open_file(p, filename) == -1) {
    	printf("Couldn't open PDF file '%s'!\n", filename);
	PDF_ENDTHREAD(1);
	return 1;
    }

    PDF_set_info(p, "Keywords", "image graphics text hypertext");
    PDF_set_info(p, "Subject", "Check many PDFlib function calls");
    PDF_set_info(p, "Title", "PDFlib test program");
    PDF_set_info(p, "Creator", "PDFtest");
    PDF_set_info(p, "Author", "Thomas Merz");

    gif_image(p);
    jpeg_image(p);
#ifdef HAVE_LIBTIFF
    tiff_image(p);
#endif
    ccitt_image(p);
    centered_text(p);
    character_table(p);
    annotations(p);
    bookmarks(p);
    grid(p);
    shaded_circle(p);
    memory_image(p);
    radial_structure(p);
    random_data_graph(p);
    many_pages(p);

    PDF_close(p);
    PDF_delete(p);

    printf("\nPDFlib test file '%s' finished!\n", filename);

    PDF_ENDTHREAD(0);

    return 0;
}

#define PDF_MAXTHREADS 4

int
main(int argc, char *argv[])
{
#ifdef WIN32
    unsigned t, w, thread_id;

    HANDLE hThread[PDF_MAXTHREADS];

    /* do a multi-threaded test */
    for (t=0; t < PDF_MAXTHREADS; t++) {
	hThread[t] =(HANDLE) _beginthreadex(NULL, 0, &Do_PDFlib_tests,
					(void *)t, 0, &thread_id );
	if (hThread[t] == 0) {
	    printf("Couldn't create thread %ud\n", t);
	    break;
	}
    }

    for (w=0; w < t; w++) {
	WaitForSingleObject(hThread[w], INFINITE);
	CloseHandle(hThread[w]);
    }
#else

    /* do a single test */
    Do_PDFlib_tests((void *) 0L);

#endif

    exit(0);

    return 0;	/* shut up compiler warnings */
}
