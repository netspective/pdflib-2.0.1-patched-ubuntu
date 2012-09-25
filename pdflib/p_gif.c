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

/* p_gif.c
 *
 * GIF processing for PDFlib
 *
 */

/* 
 * This module is basically a rewrite of David Koblas' giftoppm.c,
 * taken from the PBMPLUS package. It contained the following notice:
 */

/* +-------------------------------------------------------------------+ */
/* | Copyright 1990, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

#include <string.h>

#include "p_intern.h"

#define CM_RED			0
#define CM_GREEN		1
#define CM_BLUE			2

#define	MAX_LWZ_BITS		12

#define INTERLACE		0x40
#define LOCALCOLORMAP		0x80
#define BitSet(byte, bit)	(((byte) & (bit)) == (bit))

#define	ReadOK(file,buffer,len)	(fread(buffer, len, 1, file) != 0)

#define LM_to_uint(a,b)			(((b)<<8)|(a))

static int ReadColorMap(FILE *fp, int number, pdf_colormap buffer);
static int DoExtension(pdf_image *image, int label);
static int GetDataBlock(pdf_image *image, unsigned char  *buf);
static void ReadImage(PDF *p, pdf_image *image, PDF_data_source *src, int ignore);

static void
pdf_data_source_GIF_init(PDF *p, PDF_data_source *src)
{
    pdf_image		*image;

    image = (pdf_image *) src->private_data;

    src->buffer_length	= (size_t) (image->width * image->height * 1);
    src->buffer_start	= (byte*) p->malloc(p, src->buffer_length,
				"pdf_data_source_GIF_init");
    src->bytes_available= 0;
    src->next_byte	= NULL;

    if (!image->image_info.gif.useGlobalColormap) {
	if (ReadColorMap(image->fp, image->BitPixel, image->colormap)) {
	    PDF_close_image(p, image - p->images);
	    pdf_error(p, PDF_IOError, "Error reading local colormap");
	}
    }
}

static int
pdf_data_source_GIF_fill(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;

    if (src->next_byte != NULL)		/* all finished in one turn */
	return pdf_false;

    image = (pdf_image *) src->private_data;

    src->next_byte = src->buffer_start;
    src->bytes_available = src->buffer_length;

    ReadImage(p, image, src, pdf_false);

    return pdf_true;
}

static void
pdf_data_source_GIF_terminate(PDF *p, PDF_data_source *src)
{
  p->free(p, (void *) src->buffer_start);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_GIF(PDF *p, const char *filename)
{
    unsigned char	buf[16];
    char	c;
    int		imageCount = 0;
    char	version[4];
    int		imageNumber = 1;	/* we read only the first image */
    pdf_image	*image;
    int im;

    for (im = 0; im < p->images_capacity; im++)
	if (p->images[im].in_use == 0)	/* found free slot */
	    break;

    if (im == p->images_capacity) 
	pdf_grow_images(p);

    image = &p->images[im];

    if ((image->fp = fopen(filename, READMODE)) == NULL)
	return -1;	/* Couldn't open GIF file */

    /* Error reading magic number or not a GIF file */
    if (!ReadOK(image->fp,buf,6) || strncmp((const char *) buf, "GIF",3) != 0) {
	fclose(image->fp);
	return -1;
    }

    strncpy(version, (const char *) buf + 3, 3);
    version[3] = '\0';

    /* Bad version number */
    if ((strcmp(version, "87a") != 0) && (strcmp(version, "89a") != 0)) {
	fclose(image->fp);
	return -1;
    }

    /* Failed to read screen descriptor */
    if (! ReadOK(image->fp,buf,7)) {
	fclose(image->fp);
	return -1;
    }

    /* size of the global color table*/
    image->BitPixel		= 2<<(buf[4]&0x07);

    if (BitSet(buf[4], LOCALCOLORMAP)) {	/* Global Colormap */
	if (ReadColorMap(image->fp, image->BitPixel, image->colormap)) {
	    /* Error reading global colormap */
	    fclose(image->fp);
	    return -1;
	}
    }

#ifdef ORIG
    if (image->AspectRatio != 0 && image->AspectRatio != 49) {
	float	r;
	r = ( (float) image->AspectRatio + 15.0 ) / 64.0;
	/*pm_message("warning - non-square pixels");*/
    }
#endif /* ORIG */

    for (;;) {
	/* EOF / read error in image data */
	if (! ReadOK(image->fp,&c,1)) {
	    fclose(image->fp);
	    return -1;
	}

	if (c == ';') {		/* GIF terminator */
	    /* Not enough images found in file */
	    if (imageCount < imageNumber) {
		fclose(image->fp);
		return -1;
	    }
	    break;
	}

	if (c == '!') { 	/* Extension */
	    if (! ReadOK(image->fp,&c,1)) {
		/* EOF / read error on extension function code */
		fclose(image->fp);
		return -1;
	    }
	    DoExtension(image, c);
	    continue;
	}

	if (c != ',') {		/* Not a valid start character */
	    /* Bogus character, ignoring */
	    continue;
	}

	++imageCount;

	if (! ReadOK(image->fp,buf,9)) {
	    /* Couldn't read left/top/width/height */
	    fclose(image->fp);
	    return -1;
	}

	image->colorspace = DeviceRGB;	/* HACK */
	image->indexed = pdf_true;
	image->components = 3;		/* HACK */
	image->bpc = 8;			/* HACK */
	image->width	= LM_to_uint(buf[4],buf[5]);
	image->height	= LM_to_uint(buf[6],buf[7]);
	image->image_info.gif.useGlobalColormap = !BitSet(buf[8], LOCALCOLORMAP);
#ifdef ORIG
	image->image_info.gif.bitPixel = 1<<((buf[8]&0x07)+1);
#endif
	image->image_info.gif.interlace= BitSet(buf[8], INTERLACE);

	if (imageCount == 1)
	    break;
    }
    
    image->src.init		= pdf_data_source_GIF_init;
    image->src.fill		= pdf_data_source_GIF_fill;
    image->src.terminate	= pdf_data_source_GIF_terminate;
    image->src.private_data	= (void *) image;

    image->compression		= none;
    image->in_use		= 1;		/* mark slot as used */
    image->filename		= pdf_strdup(p, filename);
    image->reference		= pdf_ref_direct;

    pdf_put_image(p, im);
    fclose(image->fp);

    return im;
}

static int
ReadColorMap(FILE *fp, int number, pdf_colormap buffer)
{
    int		i;
    unsigned char	rgb[3];

    for (i = 0; i < number; ++i) {
	    if (! ReadOK(fp, rgb, sizeof(rgb))) {
		return pdf_true;
	    }

	    buffer[i][0] = rgb[0] ;
	    buffer[i][1] = rgb[1] ;
	    buffer[i][2] = rgb[2] ;
    }
    return pdf_false;
}

static int
DoExtension(pdf_image *image, int label)
{
#define buf	image->image_info.gif.DoExtension_buf

	const char		*str;

	switch (label) {
	case 0x01:		/* Plain Text Extension */
		str = "Plain Text Extension";
#ifdef ORIG
		if (GetDataBlock(image, (unsigned char*) buf) == 0)
			;

		lpos   = LM_to_uint(buf[0], buf[1]);
		tpos   = LM_to_uint(buf[2], buf[3]);
		width  = LM_to_uint(buf[4], buf[5]);
		height = LM_to_uint(buf[6], buf[7]);
		cellw  = buf[8];
		cellh  = buf[9];
		foreground = buf[10];
		background = buf[11];

		while (GetDataBlock(image, (unsigned char*) buf) != 0) {
			PPM_ASSIGN(image[ypos][xpos],
					cmap[v][CM_RED],
					cmap[v][CM_GREEN],
					cmap[v][CM_BLUE]);
			++index;
		}

		return pdf_false;
#else	/* not ORIG */
		break;
#endif	/* not ORIG */
	case 0xff:		/* Application Extension */
		str = "Application Extension";
		break;
	case 0xfe:		/* Comment Extension */
		str = "Comment Extension";
		while (GetDataBlock(image, (unsigned char*) buf) != 0) {
			/*pm_message("gif comment: %s", buf );*/
		}
		return pdf_false;
	case 0xf9:		/* Graphic Control Extension */
		str = "Graphic Control Extension";
		(void) GetDataBlock(image, (unsigned char*) buf);
#ifdef ORIG
		disposal    = (buf[0] >> 2) & 0x7;
		inputFlag   = (buf[0] >> 1) & 0x1;
		delayTime   = LM_to_uint(buf[1],buf[2]);
		if ((buf[0] & 0x1) != 0)
		    transparent = buf[3];
#endif	/* ORIG */

		while (GetDataBlock(image, (unsigned char*) buf) != 0)
			;
		return pdf_false;
	default:
		str = buf;
		sprintf(buf, "UNKNOWN (0x%02x)", label);
		break;
	}

	/*pm_message("got a '%s' extension", str );*/

	while (GetDataBlock(image, (unsigned char*) buf) != 0)
		;

	return pdf_false;
#undef buf
}

static int
GetDataBlock(pdf_image *image, unsigned char *buf)
{
	unsigned char	count;
	FILE *fp = image->fp;

	if (! ReadOK(fp,&count,1)) {
		/* Error in getting DataBlock size */
		return -1;
	}

	image->image_info.gif.ZeroDataBlock = (count == 0);

	if ((count != 0) && (! ReadOK(fp, buf, count))) {
		/* Error in reading DataBlock */
		return -1;
	}

	return count;
}

/* Dummies for LZW code */
#define pm_message(x)	/* */
#define pm_error(x)	/* */

static int
GetCode(pdf_image *image, int code_size, int flag)
{
#define curbit		image->image_info.gif.curbit
#define lastbit		image->image_info.gif.lastbit
#define done		image->image_info.gif.done
#define last_byte 	image->image_info.gif.last_byte
#define buf 		image->image_info.gif.buf

	int			i, j, ret;
	unsigned char		count;

	if (flag) {
		curbit = 0;
		lastbit = 0;
		last_byte = 0;
		done = pdf_false;
		return 0;
	}

	if ( (curbit+code_size) >= lastbit) {
		if (done) {
			if (curbit >= lastbit) {
				pm_error("ran off the end of my bits" );
			}
			return -1;
		}
		buf[0] = buf[last_byte-2];
		buf[1] = buf[last_byte-1];

		if ((count = (unsigned char) GetDataBlock(image, &buf[2])) == 0)
			done = pdf_true;

		last_byte = 2 + count;
		curbit = (curbit - lastbit) + 16;
		lastbit = (2+count)*8 ;
	}

	ret = 0;
	for (i = curbit, j = 0; j < code_size; ++i, ++j)
		ret |= ((buf[ i / 8 ] & (1 << (i % 8))) != 0) << j;

	curbit += code_size;

	return ret;
#undef curbit
#undef lastbit
#undef done
#undef last_byte
#undef buf
}

static int
LWZReadByte(PDF *p, pdf_image *image, int flag, int input_code_size)
{
#define fresh		image->image_info.gif.fresh
#define code_size	image->image_info.gif.code_size
#define set_code_size	image->image_info.gif.set_code_size
#define max_code	image->image_info.gif.max_code
#define max_code_size	image->image_info.gif.max_code_size
#define firstcode	image->image_info.gif.firstcode
#define oldcode		image->image_info.gif.oldcode
#define clear_code	image->image_info.gif.clear_code
#define end_code	image->image_info.gif.end_code
#define sp		image->image_info.gif.sp
#define table		p->gif_table
#define stack		p->gif_stack

    int		code, incode;
    register int	i;

    if (flag) {
	set_code_size = input_code_size;
	code_size = set_code_size+1;
	clear_code = 1 << set_code_size ;
	end_code = clear_code + 1;
	max_code_size = 2*clear_code;
	max_code = clear_code+2;

	GetCode(image, 0, pdf_true);
	
	fresh = pdf_true;

	for (i = 0; i < clear_code; ++i) {
	    table[0][i] = 0;
	    table[1][i] = i;
	}
	for (; i < (1<<MAX_LWZ_BITS); ++i)
		table[0][i] = table[1][0] = 0;

	sp = stack;

	return 0;
    } else if (fresh) {
	fresh = pdf_false;
	do {
		firstcode = oldcode =
			GetCode(image, code_size, pdf_false);
	} while (firstcode == clear_code);
	return firstcode;
    }

    if (sp > stack)
	return *--sp;

    while ((code = GetCode(image, code_size, pdf_false)) >= 0) {
	if (code == clear_code) {
	    for (i = 0; i < clear_code; ++i) {
		    table[0][i] = 0;
		    table[1][i] = i;
	    }
	    for (/* */ ; i < (1<<MAX_LWZ_BITS); ++i)
		table[0][i] = table[1][i] = 0;
	    code_size = set_code_size+1;
	    max_code_size = 2*clear_code;
	    max_code = clear_code+2;
	    sp = stack;
	    firstcode = oldcode = GetCode(image, code_size, pdf_false);
	    return firstcode;
	} else if (code == end_code) {
	    int		count;
	    unsigned char	buf[260];

	    if (image->image_info.gif.ZeroDataBlock)
		return -2;

	    while ((count = GetDataBlock(image, buf)) > 0)
		/* */ ;

	    if (count != 0) {
		pm_message("missing EOD in data stream (common occurence)");
	    }
	    return -2;
	}

	incode = code;

	if (code >= max_code) {
	    *sp++ = firstcode;
	    code = oldcode;
	}

	while (code >= clear_code) {
	    *sp++ = table[1][code];
	    if (code == table[0][code]) {
		    pm_error("circular table entry BIG ERROR");
	    }
	    code = table[0][code];
	}

	*sp++ = firstcode = table[1][code];

	if ((code = max_code) <(1<<MAX_LWZ_BITS)) {
	    table[0][code] = oldcode;
	    table[1][code] = firstcode;
	    ++max_code;
	    if ((max_code >= max_code_size) &&
		    (max_code_size < (1<<MAX_LWZ_BITS))) {
		    max_code_size *= 2;
		    ++code_size;
	    }
	}

	oldcode = incode;

	if (sp > stack)
	    return *--sp;
    }
    return code;

#undef fresh
#undef code_size
#undef set_code_size
#undef max_code
#undef max_code_size
#undef firstcode
#undef oldcode
#undef clear_code
#undef end_code
#undef table
#undef stack
#undef sp
}


static void
ReadImage(PDF *p, pdf_image *image, PDF_data_source *src, int ignore)
{
    unsigned char	c;	
    int		v;
    unsigned int	xpos = 0, ypos = 0, pass = 0;
    byte		*dest;

    /*
    **  Initialize the Compression routines
    */
    image->image_info.gif.fresh = pdf_false;
    image->image_info.gif.ZeroDataBlock = pdf_false;

    if (! ReadOK(image->fp,&c,1)) {
	    pm_error("EOF / read error on image data" );
    }

    if (LWZReadByte(p, image, pdf_true, c) < 0) {
	    pm_error("error reading image" );
    }

    /*
    **  If this is an "uninteresting picture" ignore it.
    */
    if (ignore) {
	while (LWZReadByte(p, image, pdf_false, c) >= 0)
		;
	return;
    }

    dest = src->buffer_start;
    while ((v = LWZReadByte(p, image, pdf_false, c)) >= 0 ) {

	*dest++ = (byte) v;	/* fetch pixel value */
	++xpos;

	if (xpos == (unsigned int) image->width) {
		xpos = 0;

		if (image->image_info.gif.interlace) {
		    switch (pass) {
			case 0:
			case 1:
				ypos += 8; break;
			case 2:
				ypos += 4; break;
			case 3:
				ypos += 2; break;
		    }

		    if (ypos >= (unsigned int) image->height) {
			++pass;
			switch (pass) {
			case 1:
				ypos = 4; break;
			case 2:
				ypos = 2; break;
			case 3:
				ypos = 1; break;
			default:
				goto fini;
			}
		    }
		} else {
		    ++ypos;
		}
		dest = src->buffer_start + ypos * image->width;
	}
	if (ypos >= (unsigned int) image->height)
	    break;
    }

fini:
    if (LWZReadByte(p, image, pdf_false, c) >= 0)
	pdf_error(p, PDF_NonfatalError,
	    "Too much input data in GIF file '%s', ignoring extra.",
	    image->filename);
}
