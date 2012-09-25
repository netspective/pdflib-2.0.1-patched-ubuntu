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

/* p_tiff.c
 *
 * TIFF processing for PDFlib
 *
 */

#include "p_intern.h"

#ifdef HAVE_LIBTIFF

#include <tiffio.h>

static void
pdf_data_source_TIFF_init(PDF *p, PDF_data_source *src)
{
  pdf_image	*image;

  image = (pdf_image *) src->private_data;
  image->image_info.tiff.cur_line = 0;

  src->buffer_length = image->components * image->width;
  src->buffer_start = (byte *)
  	p->malloc(p, src->buffer_length, "PDF_data_source_TIFF_init");
}

static int
pdf_data_source_TIFF_fill(PDF *p, PDF_data_source *src)
{
  pdf_image	*image;
  int		col;
  byte		*dest;
  uint32	*s;

  image = (pdf_image *) src->private_data;

  if (image->image_info.tiff.cur_line++ == image->height)
    return pdf_false;

  src->next_byte = src->buffer_start;
  src->bytes_available = src->buffer_length;

  dest = src->buffer_start;
  s = image->image_info.tiff.raster + 
  	(image->height - image->image_info.tiff.cur_line) * image->width;
  
  switch (image->components) {
      case 1:
		  for (col = 0; col < image->width; col++, s++) {
			*dest++ = (byte) TIFFGetR(*s);
		  }
		  break;

      case 3:
		  for (col = 0; col < image->width; col++, s++) {
			*dest++ = (byte) TIFFGetR(*s);
			*dest++ = (byte) TIFFGetG(*s);
			*dest++ = (byte) TIFFGetB(*s);
		  }
		  break;

      default:
		pdf_error(p, PDF_IOError,
			"Unknown color space in TIFF image %s (%d components)", 
			image->filename, image->components);
  }

  return pdf_true;
}

static void
pdf_data_source_TIFF_terminate(PDF *p, PDF_data_source *src)
{
  p->free(p, (void *) src->buffer_start);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_TIFF(PDF *p, const char *filename)
{
    pdf_image *image;
    int32 w, h;
    uint16 bpc;
    tsample_t components;
    size_t npixels;
    uint16	*rmap, *gmap, *bmap;
    int im;

    for (im = 0; im < p->images_capacity; im++)
	if (p->images[im].in_use == 0)		/* found free slot */
	    break;

    if (im == p->images_capacity) 
	pdf_grow_images(p);

    image = &p->images[im];

    /* Suppress all warnings and error messages */
    (void) TIFFSetErrorHandler(NULL);
    (void) TIFFSetWarningHandler(NULL);

    image->image_info.tiff.tif = TIFFOpen(filename, READMODE);

    if (!image->image_info.tiff.tif)
	return -1;

    TIFFGetField(image->image_info.tiff.tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(image->image_info.tiff.tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetFieldDefaulted(image->image_info.tiff.tif, TIFFTAG_BITSPERSAMPLE, &bpc);
    TIFFGetFieldDefaulted(image->image_info.tiff.tif, TIFFTAG_SAMPLESPERPIXEL, &components);

    image->width		= w;
    image->height		= h;
    /* we use 8 bit to retrieve the image data in all cases */
    image->bpc			= 8;
    image->components		= components;
    image->compression		= none;
    image->BitPixel		= 1 << image->bpc;
    image->indexed		= pdf_false;
    image->reference		= pdf_ref_direct;

    switch (image->components) {
	case 1:
		if (TIFFGetField(image->image_info.tiff.tif, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap)) {
			/* if it is a palette image we have to adapt these accordingly */
			image->colorspace	= DeviceRGB;
			image->components	= 3;
		} else
			/* plain old grayscale image */
			image->colorspace	= DeviceGray;
	    break;
	case 3:
	    image->colorspace	= DeviceRGB;
	    break;
	case 4:
	    image->colorspace	= DeviceCMYK;
	    break;
	default:
	    TIFFClose(image->image_info.tiff.tif);
	    return -1;
    }

    image->src.init		= pdf_data_source_TIFF_init;
    image->src.fill		= pdf_data_source_TIFF_fill;
    image->src.terminate	= pdf_data_source_TIFF_terminate;
    image->src.private_data	= (void *) image;

    npixels = w * h;
    image->image_info.tiff.raster =
    	(uint32*) _TIFFmalloc((tsize_t) (npixels * sizeof (uint32)));

    if (image->image_info.tiff.raster == NULL ||
	!TIFFReadRGBAImage(image->image_info.tiff.tif,
		(uint32) w, (uint32) h, image->image_info.tiff.raster, 1)) {
	TIFFClose(image->image_info.tiff.tif);
	return -1;
    }

    image->in_use = 1;			/* mark slot as used */
    image->filename		= pdf_strdup(p, filename);

    pdf_put_image(p, im);

    _TIFFfree(image->image_info.tiff.raster);
    TIFFClose(image->image_info.tiff.tif);

    return im;
}

#else	/* not HAVE_LIBTIFF */

/* Define dummies if we can't use TIFFlib */
PDFLIB_API int PDFLIB_CALL
PDF_open_TIFF(PDF *p, const char *filename)
{
    return -1;
}

#endif	/* HAVE_LIBTIFF */
