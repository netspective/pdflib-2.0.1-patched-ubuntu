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

/* p_image.c
 *
 * PDFlib image routines
 *
 */

#include <stdio.h>
#include <string.h>

#include "p_intern.h"

static const char *pdf_filter_names[] = {
    "", "LZWDecode", "RunLengthDecode", "CCITTFaxDecode",
    "DCTDecode", "FlateDecode"
};

static const char *pdf_colorspace_names[] = {
    "DeviceGray", "DeviceRGB", "DeviceCMYK", "CalGray", "CalRGB", "Lab", 
    "Indexed", "Pattern", "Separation"
};

void
pdf_init_images(PDF *p)
{
    int im;

    p->images_capacity = IMAGES_CHUNKSIZE;

    p->images = (pdf_image *) 
    	p->malloc(p, sizeof(pdf_image) * p->images_capacity, "pdf_init_images");

    for (im = 0; im < p->images_capacity; im++)
	p->images[im].in_use = 0;		/* mark slot as unused*/
}

void
pdf_grow_images(PDF *p)
{
    int im;

    p->images = (pdf_image *) p->realloc(p, p->images,
	sizeof(pdf_image) * 2 * p->images_capacity, "pdf_grow_images");

    for (im = p->images_capacity; im < 2 * p->images_capacity; im++)
	p->images[im].in_use = 0;		/* mark slot as unused*/

    p->images_capacity *= 2;
}

void
pdf_cleanup_images(PDF *p)
{
    int im;

    /* Free images which the caller left open */
    for (im = 0; im < p->images_capacity; im++)
	if (p->images[im].in_use == 1)		/* found used slot */
	    PDF_close_image(p, im);	/* free image descriptor */

    if (p->images)
	p->free(p, p->images);
}

void
pdf_init_xobjects(PDF *p)
{
    int index;

    p->xobjects_number	= 0;
    p->xobjects_capacity = XOBJECTS_CHUNKSIZE;

    p->xobjects = (pdf_xobject *) 
    	p->malloc(p, sizeof(pdf_xobject) * p->xobjects_capacity,
	"pdf_init_xobjects");

    for (index = 0; index < p->xobjects_capacity; index++)
	p->xobjects[index].used_on_current_page = pdf_false;
}

void
pdf_grow_xobjects(PDF *p)
{
    int index;

    p->xobjects = (pdf_xobject *) p->realloc(p, p->xobjects,
	sizeof(pdf_xobject) * 2 * p->xobjects_capacity, "pdf_grow_xobjects");

    for (index=p->xobjects_capacity; index < 2 * p->xobjects_capacity; index++)
	p->xobjects[index].used_on_current_page = pdf_false;

    p->xobjects_capacity *= 2;
}

void
pdf_write_xobjects(PDF *p)
{
    int index;

    if (p->xobjects_number > 0) {
	(void) fputs("/XObject ", p->fp);

	pdf_begin_dict(p);	/* XObject */

	for (index = 0; index < p->xobjects_number; index++) {
	    if (p->xobjects[index].used_on_current_page) {
		(void) fprintf(p->fp, "/I%d %ld 0 R\n", index, 
			p->xobjects[index].obj_id);
		p->xobjects[index].used_on_current_page = pdf_false;
	    }
	}

	pdf_end_dict(p);	/* XObject */
    }
}

void
pdf_cleanup_xobjects(PDF *p)
{
    if (p->xobjects)
	p->free(p, p->xobjects);
}

/* These are offered because we don't expose the pdf_image structure
 * to clients any more. The PDF* is not yet used but may be needed
 * in the future.
 */
PDFLIB_API int PDFLIB_CALL
PDF_get_image_width(PDF *p, int im)
{
    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use) {
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_get_image_width", im);
	/* satisfy compiler since pdf_error won't return */
	return 0;
    } else
	return (int) (p->images[im].width);
}

PDFLIB_API int PDFLIB_CALL
PDF_get_image_height(PDF *p, int im)
{
    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use) {
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_get_image_height", im);
	/* satisfy compiler since pdf_error won't return */
	return 0;
    } else
	return (int) (p->images[im].height);
}

void
pdf_put_image(PDF *p, int im)
{
    id		length_id, colormap_id = 0;
    long	length;
    pdf_image	*image;

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in pdf_put_image", im);

    image = &p->images[im];

    switch (image->colorspace) {
        case DeviceGray:
        case DeviceRGB:
        case DeviceCMYK:
	    break;

	default:
	    pdf_error(p, PDF_SystemError, "Bad number of color components");
	    break;
    }

    /* Images may also be written to the output before the first page */
    if (p->state == pdf_state_page_description)
	pdf_end_contents_section(p);

    /* Write colormap information for indexed color spaces */
    if (image->indexed) {
	colormap_id = pdf_begin_obj(p, NEW_ID);	/* Colormap object */
	pdf_begin_dict(p); 		

	/* Length of colormap object */
	length_id = pdf_alloc_id(p);
	(void) fprintf(p->fp,"/Length %ld 0 R\n", length_id);
	pdf_end_dict(p);

	pdf_begin_stream(p);
	p->start_contents_pos = ftell(p->fp);

	if (image->components != 1 && image->components != 3) {
	    pdf_error(p, PDF_SystemError,
	    	"Bogus indexed colorspace (%d color components)",
		image->components);
	}

	/* Write colormap data */
	/* HACK */
	fwrite(image->colormap, (size_t) (image->BitPixel * 3), 1, p->fp);

	length = ftell(p->fp) - p->start_contents_pos;

	/* Don't count the newline in order to not mess up some
	 * PDF viewers or their PostScript print streams.
	 */
	(void) fputs("\n", p->fp);

	pdf_end_stream(p);
	pdf_end_obj(p);

	pdf_begin_obj(p, length_id);		/* Length object for colormap */
	(void) fprintf(p->fp,"%ld\n", length);
	pdf_end_obj(p);
    }

    /* Image object */

    image->no = p->xobjects_number++;
    p->xobjects[image->no].obj_id = pdf_begin_obj(p, NEW_ID);	/* XObject */

    if (p->xobjects_number >= p->xobjects_capacity)
	pdf_grow_xobjects(p);

    pdf_begin_dict(p); 		/* XObject */

    (void) fputs("/Type /XObject\n", p->fp);
    (void) fputs("/Subtype /Image\n", p->fp);

    /* Name is only required for old-style PDF 1.0 */
    /* (void) fprintf(p->fp,"/Name /I%d\n", image->no); */

    (void) fprintf(p->fp,"/Width %d\n", image->width);
    (void) fprintf(p->fp,"/Height %d\n", image->height);
    (void) fprintf(p->fp,"/BitsPerComponent %d\n", image->bpc);

    (void) fputs("/ColorSpace ", p->fp);
    if (image->indexed) {
	(void) fputs("[/Indexed ", p->fp);
	(void) fprintf(p->fp, "/%s %d %ld 0 R ]\n",
		(image->components == 3 || image->indexed ? 
		pdf_colorspace_names[DeviceRGB] :
		pdf_colorspace_names[DeviceGray]),
		image->BitPixel - 1, colormap_id);
    } else {
	(void) fprintf(p->fp, "/%s\n", pdf_colorspace_names[image->colorspace]);
    }

    /* special case: referenced image data instead of direct data */
    if (image->reference != pdf_ref_direct) {

	if (image->compression != none) {
	    (void) fprintf(p->fp, "/FFilter [/%s]\n",
		    pdf_filter_names[image->compression]);
	}

	if (image->compression == ccitt) {
	    (void) fprintf(p->fp, "/FDecodeParms [<<");

	    if (image->width != 1728)	/* CCITT default width */
		(void) fprintf(p->fp, "/Columns %d ", image->width);

	    (void) fprintf(p->fp, "/Rows %d ", image->height);

	    /* write /K and /BlackIs1 parameters, if not defaults */
	    if (image->image_info.ccitt.params != NULL)
		(void) fprintf(p->fp, image->image_info.ccitt.params);

	    (void) fprintf(p->fp, ">>]\n");

	}

	if (image->reference == pdf_ref_file) {

	    /* TODO: make image file name platform-neutral:
	     * Change : to / on the Mac
	     * Change \ to / on Windows
	     */
	    (void) fprintf(p->fp, "/F (%s) /Length 0 ", image->filename);

	} else if (image->reference == pdf_ref_url) {

	    (void) fprintf(p->fp, "/F << /FS /URL /F (%s) >> /Length 0 ",
		    image->filename);
	}

	pdf_end_dict(p);		/* XObject */

	pdf_begin_stream(p);		/* dummy image data */
	pdf_end_stream(p);		/* dummy image data */

	if (p->state == pdf_state_page_description)
	    pdf_begin_contents_section(p);

	return;
    }

    /* now the (more common) handling of actual image data to be include
     * in the PDF output.
     */

    /* do we need a filter (either ASCII or decompression)? */

    if (p->debug['a']) {
	(void) fputs("/Filter [/ASCIIHexDecode ", p->fp);
	if (image->compression != none)
	    (void) fprintf(p->fp, "/%s", pdf_filter_names[image->compression]);
	(void) fprintf(p->fp, "]\n");
    } else {
	/* not a recognized precompressed image format */
	if (image->compression == none && p->compress)
	    image->compression = flate;

	if (image->compression != none)
	    (void) fprintf(p->fp, "/Filter [/%s]\n",
		    pdf_filter_names[image->compression]);
    }

    if (image->compression == lzw) {
	(void) fprintf(p->fp, "/DecodeParms [%s <</EarlyChange 0>>]\n",
		(p->debug['a'] ? "null" : ""));
    }

    if (image->compression == ccitt) {
	(void) fprintf(p->fp, "/DecodeParms [%s <<",
		(p->debug['a'] ? "null" : ""));

	if (image->width != 1728)	/* CCITT default width */
	    (void) fprintf(p->fp, "/Columns %d ", image->width);

	(void) fprintf(p->fp, "/Rows %d ", image->height);

	/* write /K and /BlackIs1 parameters, if not defaults */
	if (image->image_info.ccitt.params != NULL)
	    (void) fprintf(p->fp, image->image_info.ccitt.params);

	(void) fprintf(p->fp, ">>]\n");
    }

    /* special handling of Photoshop-generated CMYK JPEG files */
    if (image->compression == dct &&
	image->image_info.jpeg.adobe && image->components == 4) {
	(void) fprintf(p->fp, "/Decode [1 0 1 0 1 0 1 0]\n");
    }

    /* Write the actual image data */
    length_id = pdf_alloc_id(p);

    (void) fprintf(p->fp,"/Length %ld 0 R\n", length_id);
    pdf_end_dict(p);		/* XObject */

    pdf_begin_stream(p);	/* image data */
    p->start_contents_pos = ftell(p->fp);

    /* image data */

    if (p->debug['a'])
	pdf_ASCIIHexEncode(p, &image->src);
    else {
	if (image->compression == flate && p->compress)
	    pdf_compress(p, &image->src);
	else
	    pdf_copy(p, &image->src);
    }

    length = ftell(p->fp) - p->start_contents_pos;

    pdf_end_stream(p);	/* image data */
    pdf_end_obj(p);	/* XObject */

    pdf_begin_obj(p, length_id);		/* Length object */
    (void) fprintf(p->fp,"%ld\n", length);
    pdf_end_obj(p);
    /* image data done */

    if (p->state == pdf_state_page_description)
	pdf_begin_contents_section(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_place_image(PDF *p, int im, float x, float y, float scale)
{
    pdf_matrix m;
    pdf_image *image;

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_place_image", im);

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_place_image)");

    image = &p->images[im];

    if (scale == 0.0)
	pdf_error(p, PDF_ValueError,
		"Scale factor 0 for image %s", image->filename);

    if (image->components == 3 || image->indexed)
	p->procset	|= ImageC;
    else
	p->procset	|= ImageB;

    pdf_end_text(p);
    pdf_begin_contents_section(p);

    PDF_save(p);

    m.a = image->width * scale;
    m.d = image->height * scale;
    m.b = m.c = (float) 0.0;
    m.e = x;
    m.f = y;
    pdf_concat(p, m);

    (void) fprintf(p->fp,"/I%d Do\n", image->no);
    p->xobjects[image->no].used_on_current_page = pdf_true;

    PDF_restore(p);
}

/* The following function is only supplied for Leonard Rosenthol's
 * PDFWriter clone which has access to thumbnail JPEG images for the page.
 * The would-be thumbnail image must be acquired like other PDFlib images
 * (via PDF_open_JPEG(), for example), and hooked in as thumbnail image
 * sometime during a page description. Not all pages need to have their
 * own thumbnail image.
 *
 * Note that the PDF spec calls for a maximum thumbnail size of
 * 106 x 106 pixels, and demands DeviceGray or indexed DeviceRGB.
 * While the former doesn't seem to be a strict condition, the latter is.
 *
 * Strictly speaking, re-using Xobjects as thumbnails is a little kludgey
 * since thumbnails are not supposed to have /Type and /Subtype entries,
 * but Acrobat simply doesn't care.
 */

PDFLIB_API void PDFLIB_CALL
PDF_add_thumbnail(PDF *p, int im)
{
    pdf_image *image;

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_add_thumb", im);

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_thumb)");
    
    image = &p->images[im];

    if (image->width > 106 || image->height > 106)
    	pdf_error(p, PDF_NonfatalError, "Thumbnail image too large", im);

    /* now for the real work :-) */
    p->thumb_id = p->xobjects[image->no].obj_id;
}

PDFLIB_API void PDFLIB_CALL
PDF_close_image(PDF *p, int im)
{
    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_close_image", im);

    p->images[im].in_use = 0;			/* free the image slot */

    /* clean up CCITT image parameter string if necessary */
    if (p->images[im].compression == ccitt &&
	p->images[im].image_info.ccitt.params != NULL)
	p->free(p, p->images[im].image_info.ccitt.params);

    if (p->images[im].filename)
	p->free(p, p->images[im].filename);
}

/* methods for constructing a data source from a memory buffer */

/* dummy for use in PDF_data_source */
static void
pdf_noop(PDF *p, PDF_data_source *src)
{
}

static int
pdf_data_source_buf_fill(PDF *p, PDF_data_source *src)
{
    if (src->next_byte == NULL) {
	src->next_byte		= src->buffer_start;
	src->bytes_available	= src->buffer_length;
	return pdf_true;
    }

    return pdf_false;
}

/* interface for using image data directly in memory */

PDFLIB_API int PDFLIB_CALL
PDF_open_image(PDF *p, const char *type, const char *source, const char *data, long length, int width, int height, int components, int bpc, const char *params)
{
    pdf_image *image;
    int im;

    if (type == NULL || *type == '\0')
    	pdf_error(p, PDF_ValueError,
		"No image type %d in PDF_open_image");

    if (source == NULL || *source == '\0')
    	pdf_error(p, PDF_ValueError,
		"No image source %d in PDF_open_image");

    if (!strcmp(type, "raw") && data == NULL)
    	pdf_error(p, PDF_ValueError,
		"Bad raw image pointer in PDF_open_image");

    if (strcmp(type, "ccitt") && params != NULL && *params != '\0')
    	pdf_error(p, PDF_NonfatalError,
		"Unnecessary CCITT params in PDF_open_image");

    for (im = 0; im < p->images_capacity; im++)
	if (p->images[im].in_use == 0)		/* found free slot */
	    break;

    if (im == p->images_capacity) 
	pdf_grow_images(p);

    image = &p->images[im];

    if (!strcmp(type, "jpeg")) {

	image->compression = dct;

    } else if (!strcmp(type, "ccitt")) {
	image->compression = ccitt;

	if (length < 0L) {
	    image->image_info.ccitt.BitReverse = pdf_true;
	    length = -length;
	}

	if (params != NULL && *params != '\0')
	    image->image_info.ccitt.params = pdf_strdup(p, params);
	else
	    image->image_info.ccitt.params = NULL;

    } else if (!strcmp(type, "raw")) {

	image->compression = none;

    } else
	pdf_error(p, PDF_ValueError,
	    "Bogus image type '%s' in PDF_open_image", type);

    switch (components) {
	case 1:
	    image->colorspace = DeviceGray;
	    break;
	case 3:
	    image->colorspace = DeviceRGB;
	    break;
	case 4:
	    image->colorspace = DeviceCMYK;
	    break;
	default:
	    pdf_error(p, PDF_ValueError,
	    	"Bogus number of components (%d) in PDF_open_image",
		components);
    }

    image->width		= width;
    image->height		= height;
    image->bpc			= bpc;
    image->components		= components;
    image->indexed		= pdf_false;
    image->in_use		= 1;			/* mark slot as used */

    if (!strcmp(source, "memory")) {	/* in-memory image data */
	if (image->compression == none &&
	    length != (long) (width * height * components))
	    pdf_error(p, PDF_ValueError,
		"Bogus image data length '%ld' in PDF_open_image", length);

	image->filename		= NULL;
	image->reference	= pdf_ref_direct;
	image->src.init		= pdf_noop;
	image->src.fill		= pdf_data_source_buf_fill;
	image->src.terminate	= pdf_noop;

	image->src.buffer_start	= (unsigned char *) data;
	image->src.buffer_length= (size_t) length;

	image->src.bytes_available = 0;
	image->src.next_byte	= NULL;

    } else if (!strcmp(source, "fileref")) {	/* file reference */

	image->reference	= pdf_ref_file;
	image->filename		= pdf_strdup(p, data);

    } else if (!strcmp(source, "url")) {	/* url reference */

	image->reference	= pdf_ref_url;
	image->filename		= pdf_strdup(p, data);

    } else			/* error */
	pdf_error(p, PDF_ValueError,
	    "Bogus image data source '%s' in PDF_open_image", source);

    pdf_put_image(p, im);
    return im;
}
