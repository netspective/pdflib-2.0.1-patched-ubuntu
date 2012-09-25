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

/* pdfgraph.c
 *
 * A micro language for drawing PDF graphics
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
    fprintf(stderr, "pdfgraph - draw PDF graph. (C) Thomas Merz 1997-99\n");
    fprintf(stderr, "usage: pdfgraph [options] [datafile]\n");
    fprintf(stderr, "Available options:\n");
    fprintf(stderr, "-b 		binary mode (default: ASCII)\n");
    fprintf(stderr, "-o filename	PDF output file name\n");

    exit(1);
}

#define BUFLEN 512

int
main(int argc, char *argv[])
{
    char	buf[BUFLEN], *cmd;
    char	*pdffilename = NULL;
    FILE	*datafile = stdin;
    PDF		*p;
    int		opt;
    float	page_width = 595, page_height = 842;
    float	x, y, gray;
    float	red, green, blue;

    while ((opt = getopt(argc, argv, "o:")) != -1)
	switch (opt) {
	    case 'o':
		pdffilename = optarg;
		break;

	    case '?':
	    default:
		usage();
	}

    if (pdffilename == NULL)
	usage();

    if (optind < argc) {
	if ((datafile = fopen(argv[optind], READMODE)) == NULL) {
	    fprintf(stderr, "Error: cannot open data file %s.\n",argv[optind]);
	    exit(1);
	}
    } else
	usage();

    p = PDF_new();
    if (p == NULL) {
	fprintf(stderr, "Error: cannot open output file %s.\n", pdffilename);
	usage();
    }

    PDF_open_file(p, pdffilename);

    PDF_set_info(p, "Title", "Converted graphics");
    PDF_set_info(p, "Creator", "pdfgraph");

    PDF_begin_page(p, page_width, page_height);

    while ((cmd = fgets(buf, BUFLEN, datafile)) != NULL) {
	switch (cmd[0]) {
	case 'M':
	    if (sscanf(buf+1, "%f %f", &x, &y) != 2) {
		fprintf(stderr, "Error in line: %s", buf);
		continue;
	    }
	    PDF_moveto(p, x, y);
	    break;

	case 'L':
	    if (sscanf(buf+1, "%f %f", &x, &y) != 2) {
		fprintf(stderr, "Error in line: %s", buf);
		continue;
	    }
	    PDF_lineto(p, x, y);
	    break;

	case 'S':
	    PDF_stroke(p);
	    break;

	case 'f':
	    PDF_fill(p);
	    break;

	case 'F':
	    PDF_fill_stroke(p);
	    break;

	case 'g':
	    if (sscanf(buf+1, "%f", &gray) != 1) {
		fprintf(stderr, "Error in line: %s", buf);
		continue;
	    }
	    PDF_setgray(p, gray);
	    break;

	case 'C':
	    if (sscanf(buf+1, "%f %f %f", &red, &green, &blue) != 3) {
		fprintf(stderr, "Error in line: %s", buf);
		continue;
	    }
	    PDF_setrgbcolor(p, red, green, blue);
	    break;

	case '%':
	default:
	    break;;
	}
    }

    PDF_end_page(p);
    PDF_close(p);

    fclose(datafile);
    exit(0);
}
