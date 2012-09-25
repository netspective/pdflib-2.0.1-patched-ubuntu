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

/* compileafm.c
 *
 * Generate C header file with font metrics
 *
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef NeXT
#include <libc.h>	/* for getopt(), optind, optarg */
#endif

#ifdef __CYGWIN32__
#include <getopt.h>	/* for getopt(), optind, optarg */
#endif

#include "p_intern.h"
#include "p_afmparse.h"

#include "ansi_e.h"
#include "macrom_e.h"
#include "pdfdoc_e.h"

#ifdef WIN32
int getopt(int nargc, char **nargv, char *ostr);
char *optarg;
int optind;
#endif

const char *pdf_encoding_enum_names[] = {
    "builtin", "pdfdoc", "macroman", "macexpert", "winansi"
};

pdf_encodingvector *pdf_encodings[] = {
    NULL, &pdf_pdfdoc, &pdf_macroman, NULL, &pdf_winansi
};

void
add_entry(PDF *p, FILE *out, pdf_font font, pdf_encoding enc)
{
    int		i;

    fprintf(out, 
    	"\n/* ---------------------------------------------------------- */\n");
    fprintf(out, "{");
    fprintf(out, "\t\"%s\",\t\t\t/* FontName */\n", font.name);
    fprintf(out, "\t%s,\t\t\t/* Encoding */\n",
    	pdf_encoding_enum_names[font.encoding]);
    fprintf(out, "\t0,\t\t\t\t/* used on current page */\n");
    fprintf(out, "\t0,\t\t\t\t/* embed flag */\n");
    fprintf(out, "\t(char *) NULL,\t\t\t/* Name of external font file */\n");
    fprintf(out, "\t0L,\t\t\t\t/* Object id */\n");
    fprintf(out, "\t%ul,\t\t\t/* Font flags */\n", font.flags);
    fprintf(out, "\tpdf_false,\t\t\t/* Read from AFM file */\n");
    fprintf(out, "\t\"%s\",\t\t\t/* Full name */\n", font.fullName);
    fprintf(out, "\t\"%s\",\t\t/* Native encoding */\n", font.encodingScheme);
    fprintf(out, "\t\"%s\",\t\t\t/* Family name */\n", font.familyName);
    fprintf(out, "\t\"%s\",\t\t\t\t/* Weight */\n", font.weight);

    fprintf(out, "\t(float) %2.1f,\t\t\t/* ItalicAngle */\n", font.italicAngle);
    fprintf(out, "\t%d,\t\t\t\t/* isFixedPitch */\n", font.isFixedPitch);
    fprintf(out, "\t%d,\t\t\t\t/* llx */\n", font.llx);
    fprintf(out, "\t%d,\t\t\t\t/* lly */\n", font.lly);
    fprintf(out, "\t%d,\t\t\t\t/* urx */\n", font.urx);
    fprintf(out, "\t%d,\t\t\t\t/* ury */\n", font.ury);
    fprintf(out, "\t%d,\t\t\t\t/* UnderlinePosition */\n", font.underlinePosition);
    fprintf(out, "\t%d,\t\t\t\t/* UnderlineThickness */\n",font.underlineThickness);
    fprintf(out, "\t%d,\t\t\t\t/* CapHeight */\n", font.capHeight);
    fprintf(out, "\t%d,\t\t\t\t/* xHeight */\n", font.xHeight);
    fprintf(out, "\t%d,\t\t\t\t/* Ascender */\n", font.ascender);
    fprintf(out, "\t%d,\t\t\t\t/* Descender */\n", font.descender);
    fprintf(out, "\t%d,\t\t\t\t/* StdVW */\n", font.StdVW);
    fprintf(out, "\t%d,\t\t\t\t/* StdHW */\n", font.StdHW);

    fprintf(out, "\n\t0,\t\t\t\t/* numOfChars */\n");
    fprintf(out, "\t(CharMetricInfo *) NULL,\t/* cmi */\n");
    fprintf(out, "\t0,\t\t\t\t/* numOfTracks */\n");
    fprintf(out, "\t(TrackKernData *) NULL,\t\t/* tkd */\n");
    fprintf(out, "\t0,\t\t\t\t/* numOfPairs */\n");
    fprintf(out, "\t(PairKernData *) NULL,\t\t/* pkd */\n");
    fprintf(out, "\t0,\t\t\t\t/* numOfComps */\n");
    fprintf(out, "\t(CompCharData *) NULL,\t\t/* ccd */\n");

    fprintf(out, "\n/* Character metrics for font %s */\n", font.name);
    fprintf(out, "{");

    for (i=0; i < 256; i++) {
	fprintf(out, "\t%3d", font.widths[i]);
	if (i == 255)
	    fprintf(out, " ");
	else
	    fprintf(out, ",");

	if (i % 8 == 7)
	    fprintf(out, " /* 0x%02x */\n", i);
    }

    fprintf(out, "}\n");
    fprintf(out, "},\n");

    /* free AFM parser's storage */
    (void) pdf_cleanup_afm(p, &font);
}

int
main(int argc, char *argv[])
{
    int   opt;
    char *filename, *outfilename = NULL;
    pdf_encoding enc;
    FILE *out;
    PDF  *p;
    pdf_font font;
    size_t len;

    /* This is only a dummy to provide a PDF* for the auxiliary functions */
    if ((p = PDF_new()) == NULL) {
	fprintf(stderr, "Couldn't generate internal PDF object - aborting\n");
	exit(99);
    }

    /* By default, generate compiled metrics for the current platform.
     * This may also be changed since the encoding is recorded in the
     * generated C structs. */
#ifdef MAC
    enc = macroman;
#else
    enc = winansi;
#endif

    while ((opt = getopt(argc, argv, "e:o:")) != -1)
	switch (opt) {
	    case 'e':
		if (!strcmp(optarg, "macroman"))
		    enc = macroman;
		else if (!strcmp(optarg, "winansi"))
		    enc = winansi;
		else if (!strcmp(optarg, "pdfdoc"))
		    enc = pdfdoc;
		else {
		    fprintf(stderr, "Encoding %s not supported!\n", optarg);
		    exit(88);
		}
		break;

	    case 'o':
	    	outfilename = optarg;
		if ((out = fopen(outfilename, "w")) == NULL) {
		    fprintf(stderr, "Couldn't open output file %s!\n",
		    	outfilename);
		    exit(99);
		}

		break;

	    default:
	    	break;
	}

    if (outfilename == NULL) {
	fprintf(stderr, "%s: Compile AFM font metrics to C code\n", argv[0]);
	fprintf(stderr, 
	    "Usage: %s -e [winansi|macroman|pdfdoc] -o outfile afmfiles...\n",
	    argv[0]);
	exit(1);
    }

    while (optind < argc) {
	filename = argv[optind++];
	fprintf(stderr, "Adding %s\n", filename);

	/* parse PFM file */
	len = strlen(filename);
	if (len >= 5 && !strcmp(filename + len - 4, ".pfm")) {
	    if (!pdf_get_metrics_pfm(p, &font, NULL,  enc, filename)) {
		fprintf(stderr, "Error parsing AFM file '%s' - skipped!\n",
		filename);
	    } else
		add_entry(p, out, font, enc);
	} else {
	    /* parse AFM file */
	    if (!pdf_get_metrics_afm(p, &font, NULL,  enc, filename)) {
		fprintf(stderr, "Error parsing AFM file '%s' - skipped!\n",
		filename);
	    } else
		add_entry(p, out, font, enc);
	}
    }

    fclose(out);
    exit(0);
}
