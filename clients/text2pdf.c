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

/* text2pdf.c
 * 
 * Convert text files to PDF
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if !defined(WIN32) && !defined(MAC)
#include <unistd.h>
#endif

#ifdef WIN32
#include <process.h>
#endif

#ifdef NeXT
#include <libc.h>	/* for getopt(), optind, optarg */
#endif

#ifdef __CYGWIN32__
#include <getopt.h>	/* for getopt(), optind, optarg */
#endif

#include "pdflib.h"
#include "p_config.h"

static void
usage(void)
{
    fprintf(stderr, "text2pdf - convert text files to pdf. (C) Thomas Merz 1997-99\n");
    fprintf(stderr, "usage: text2pdf [options] [textfile]\n");
    fprintf(stderr, "Available options:\n");
    fprintf(stderr, "-f fontname   name of font to use\n");
    fprintf(stderr, "-h height     page height in points\n");
    fprintf(stderr, "-m margin     margin size in points\n");
    fprintf(stderr, "-o filename   PDF output file name\n");
    fprintf(stderr, "-s size       font size\n");
    fprintf(stderr, "-w width      page width in points\n");

    exit(1);
}

#define BUFLEN 512

int
main(int argc, char *argv[])
{
    char	buf[BUFLEN], *s;
    char	*pdffilename = NULL;
    FILE	*textfile = stdin;
    PDF		*p;
    int		opt;
    char	*fontname;
    float	fontsize;
    float	x, y, width = a4_width, height = a4_height, margin = 20;
    
    fontname		= "Courier";
    fontsize		= 12.0;

    while ((opt = getopt(argc, argv, "f:h:m:o:s:w:")) != -1)
	switch (opt) {
	    case 'f':
		fontname = optarg;
		break;

	    case 'h':
		height = atoi(optarg);
		if (height < 0) {
		    fprintf(stderr, "Error: bad page height %f!\n", height);
		    usage();
		}
		break;

	    case 'm':
		margin = atoi(optarg);
		if (margin < 0) {
		    fprintf(stderr, "Error: bad margin %f!\n", margin);
		    usage();
		}
		break;

	    case 'o':
		pdffilename = optarg;
		break;

	    case 's':
		fontsize = atoi(optarg);
		if (fontsize < 0) {
		    fprintf(stderr, "Error: bad font size %f!\n", fontsize);
		    usage();
		}
		break;

	    case 'w':
		width = atoi(optarg);
		if (width < 0) {
		    fprintf(stderr, "Error: bad page width %f!\n", width);
		    usage();
		}
		break;

	    case '?':
	    default:
		usage();
	}

    if (pdffilename == NULL)
	usage();

    if (optind < argc) {
	if ((textfile = fopen(argv[optind], READMODE)) == NULL) {
	    fprintf(stderr, "Error: cannot open input file %s.\n",argv[optind]);
	    exit(2);
	}
    } else
	textfile = stdin;

    p = PDF_new();
    if (p == NULL) {
	fprintf(stderr, "Error: cannot open output file %s.\n", pdffilename);
	exit(1);
    }

    PDF_open_file(p, pdffilename);

    PDF_set_info(p, "Title", "Converted text");
    PDF_set_info(p, "Creator", "text2pdf");

    x = margin;
    y = height - margin;

    while ((s = fgets(buf, BUFLEN, textfile)) != NULL) {
	if (s[0] == '\f') {
	    if (y == height - margin)
		PDF_begin_page(p, width, height);
	    PDF_end_page(p);
	    y = height - margin;
	    continue;
	}

	if (s[0] != '\0' && s[strlen(s) - 1] == '\n')
	    s[strlen(s) - 1] = '\0';	/* remove newline character */

	if (y < margin) {		/* page break necessary? */
	    y = height - margin;
	    PDF_end_page(p);
	}

	if (y == height - margin) {
	    PDF_begin_page(p, width, height);
	    PDF_set_font(p, fontname, fontsize, "winansi");
	    PDF_set_text_pos(p, x, y);
	    y -= fontsize;
	}

	PDF_continue_text(p, s);
	y -= fontsize;

    }

    if (y != height - margin)
	PDF_end_page(p);

    PDF_close(p);

    exit(0);
}
