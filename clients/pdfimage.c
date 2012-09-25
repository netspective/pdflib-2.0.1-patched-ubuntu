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

/* imagepdf.c
 *
 * Convert TIFF/GIF/JPEG images to PDF
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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
    fprintf(stderr, "imagepdf - Convert ");
#ifdef HAVE_LIBTIFF
    fprintf(stderr, "TIFF/");
#endif
    fprintf(stderr, "GIF/JPEG images to PDF. (C) Thomas Merz 1997-99\n");
    fprintf(stderr, "usage: imagepdf [options] imagefile(s)\n");
    fprintf(stderr, "Available options:\n");
    fprintf(stderr, "-a         ASCII mode (default: binary)\n");
    fprintf(stderr, "-c         print caption\n");
    fprintf(stderr, "-o <file>  output file\n");

    exit(1);
}

/* Several magic numbers for image file formats */

#define	GIF_MAGIC	"GIF"
#define TIFF_MAGIC_M	"MM"
#define TIFF_MAGIC_I	"II"
#define PNG_MAGIC	"\x89PNG"
#define JPEG_MAGIC	"\xFF\xD8"

#define MAGIC_LEN_MAX	10		/* maximum length of magic strings */

int
main(int argc, char *argv[])
{
    char	*pdffilename = NULL;
    FILE	*imagefile;
    PDF		*p;
    int		image;
    int		opt;
    int		caption = 0;
    float	scale;
    char	header[MAGIC_LEN_MAX];
    
    while ((opt = getopt(argc, argv, "o:")) != -1)
	switch (opt) {
	    case 'c':
		caption = 1;
		break;

	    case 'o':
		pdffilename = optarg;
		break;
	}

    if (optind == argc) {
	fprintf(stderr, "Error: no image files given.\n");
	usage();
    }

    if (pdffilename == NULL) {
	fprintf(stderr, "Error: no output file given.\n");
	usage();
    }

    p = PDF_new();

    PDF_open_file(p, pdffilename);

    if (p == NULL) {
	fprintf(stderr, "Error: cannot open output file %s.\n", pdffilename);
	exit(1);
    }


    PDF_set_info(p, "Creator", "imagepdf");

    while (optind++ < argc) {
	fprintf(stderr, "Processing image file %s...\n", argv[optind-1]);

	if ((imagefile = fopen(argv[optind-1], READMODE)) == NULL) {
	   fprintf(stderr, "Error: Couldn't open image file %s - skipped.\n",
		argv[optind-1]);
	   continue;
	}

	if (fread(header, MAGIC_LEN_MAX, 1, imagefile) != 1)  {
	   fprintf(stderr, "Error: Couldn't read from image file %s - skipped.\n",
		argv[optind-1]);
	   fclose(imagefile);
	   continue;
	}
	fclose(imagefile);

	if (!strncmp(header, GIF_MAGIC, strlen(GIF_MAGIC)))
	   image = PDF_open_GIF(p, argv[optind-1]);

#ifdef HAVE_LIBTIFF
	else if (!strncmp(header, TIFF_MAGIC_I, strlen(TIFF_MAGIC_I)) ||
	         !strncmp(header, TIFF_MAGIC_M, strlen(TIFF_MAGIC_M)))
	   image = PDF_open_TIFF(p, argv[optind-1]);
#endif

	else if (!strncmp(header, JPEG_MAGIC, strlen(JPEG_MAGIC)))
	   image = PDF_open_JPEG(p, argv[optind-1]);

	else
	   image = -1;	/* unknown file type */

	if (image == -1) {
		fprintf(stderr,"Error: Couldn't analyze image %s - skipped.\n",
			    argv[optind-1]);
	    continue;
	}

	scale = 1.0;

	PDF_begin_page(p, PDF_get_image_width(p, image) * scale, 
			PDF_get_image_height(p, image)* scale);
	
	/* define outline with filename */
	PDF_add_bookmark(p, argv[optind-1], 0, 0);

	PDF_place_image(p, image, 0.0, 0.0, scale);

	PDF_end_page(p);
    }

    PDF_close(p);
    exit(0);
}
