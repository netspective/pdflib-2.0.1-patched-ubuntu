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

/* p_intern.h
 *
 * PDFlib internal definitions
 *
 */

#ifndef P_INTERN_H
#define P_INTERN_H

#include <stdio.h>

#ifdef HAVE_LIBTIFF
#include <tiffio.h>
#endif

#include "pdflib.h"
#include "p_config.h"

#define pdf_false	0
#define pdf_true	1
typedef int pdf_bool;

#define RESOURCEFILE		"PDFLIBRESOURCE"
#define DEFAULTRESOURCEFILE	"pdflib.upr"

/* Allocation chunk sizes. These don't affect the generated documents
 * in any way. In order to save initial memory, however, you can lower 
 * the values. Increasing the values will bring some performance gain
 * for large documents.
 */
#define PAGES_CHUNKSIZE		512		/* pages */
#define ID_CHUNKSIZE		1024		/* object ids */
#define CONTENTS_CHUNKSIZE	64		/* page content streams */
#define FONTS_CHUNKSIZE		16		/* document fonts */
#define XOBJECTS_CHUNKSIZE	16		/* document xobjects */
#define IMAGES_CHUNKSIZE	16		/* document images */
#define OUTLINE_CHUNKSIZE	128		/* document outlines */

#define FLOATBUFSIZE		20		/* buffer length for floats */

/* Border styles for links */
typedef enum {
    border_solid, border_dashed, border_beveled,
    border_inset, border_underlined
} pdf_border_style;

/* Transition types for page transition effects */
typedef enum {
    trans_none, trans_split, trans_blinds, trans_box, 
    trans_wipe, trans_dissolve, trans_glitter, trans_replace
} pdf_transition;

/* icons for file attachments and text annotations */
typedef enum {
    icon_file_graph, icon_file_paperclip,
    icon_file_pushpin, icon_file_tag,

    icon_text_comment, icon_text_insert,
    icon_text_note, icon_text_paragraph,
    icon_text_newparagraph, icon_text_key,
    icon_text_help
} pdf_icon;

/* Symbolic names for predefined font encoding vectors */
typedef enum { builtin = 0, pdfdoc, macroman, macexpert, winansi } pdf_encoding;

typedef enum { ImageB = 1, ImageC = 2, ImageI = 4, Text = 8 } pdf_procset;

typedef enum {
    DeviceGray, DeviceRGB, DeviceCMYK, 
    CalGray, CalRGB, Lab, 
    Indexed, PatternCS, Separation
} pdf_colorspace;

typedef enum { c_none, c_stream, c_text } pdf_content_type;

typedef long id;

typedef const char *pdf_encodingvector[256];

typedef enum { none, lzw, runlength, ccitt, dct, flate } pdf_compression;

typedef unsigned char byte;

typedef byte pdf_colormap[256][3];

#define NEW_ID		0L
#define BAD_ID		-1L

/* Note: pdf_begin_obj() is a function */
#define pdf_end_obj(p)		(void) fputs("endobj\n", p->fp)

#define pdf_begin_dict(p)	(void) fputs("<<", p->fp)
#define pdf_end_dict(p)		(void) fputs(">>\n", p->fp)

#define pdf_begin_stream(p)	(void) fputs("stream\n", p->fp)
#define pdf_end_stream(p)	(void) fputs("endstream\n", p->fp)

/* The Unicode byte order mark (BOM) signals Unicode strings */
#define BOM0	'\xFE'
#define BOM1	'\xFF'

/* check whether the string is plain C or unicode by looking for the BOM
 * s must not be NULL.
 */
#define pdf_is_unicode(s) (s[0] == BOM0 && s[0] != 0 && s[1] == BOM1)

/* Destination types for internal and external links */
typedef enum { retain, fitpage, fitwidth, fitheight, fitbbox } pdf_desttype;

/* Destination structure for bookmarks and links */
typedef struct {
    pdf_desttype type;
    int page;
} pdf_dest;

typedef struct { float llx, lly, urx, ury; } PDF_rectangle;

/* A PDF xobject */
typedef struct {
    id		obj_id;			/* object id of this xobject */
    pdf_bool	used_on_current_page;	/* this xobject used on current page */
} pdf_xobject;

typedef struct pdf_res_s pdf_res;

struct pdf_res_s {
    char		*name;
    char		*filename;
    pdf_res		*next;
};

typedef struct pdf_category_s pdf_category;

struct pdf_category_s {
    char		*category;
    pdf_res		*kids;
    pdf_category	*next;
};

typedef struct pdf_outline_s pdf_outline;
struct pdf_outline_s {
    id			self;		/* id of this outline object */
    id			prev;		/* previous entry at this level */
    id			next;		/* next entry at this level */
    int			parent;		/* ancestor's index */
    int			first;		/* first sub-entry */
    int			last;		/* last sub-entry */
    char 		*text;		/* bookmark text */
    int			count;		/* number of open sub-entries */
    int			open;		/* whether or not to display children */
    pdf_dest		dest;		/* destination */
};

/* Internal PDFlib states for error checking */
typedef enum {
    pdf_state_null, pdf_state_open,
    pdf_state_page_description, pdf_state_path, pdf_state_text
} pdf_state;

typedef struct { float a, b, c, d, e, f; } pdf_matrix;

/* Annotation types */
typedef enum {
    ann_text, ann_locallink,
    ann_pdflink, ann_weblink,
    ann_launchlink, ann_attach
} pdf_annot_type;

/* Fill rules */
typedef enum {pdf_fill_winding, pdf_fill_evenodd} pdf_fillrule;

/* Annotations */
typedef struct pdf_annot_s pdf_annot;
struct pdf_annot_s {
    pdf_annot_type	type;		/* used for all annotation types */
    PDF_rectangle	rect;		/* used for all annotation types */
    id			obj_id;		/* used for all annotation types */
    pdf_annot		*next;		/* used for all annotation types */

    pdf_icon		icon;		/* attach and text */
    char		*filename;	/* attach, launchlink, pdflink,weblink*/
    char		*contents;	/* text, attach, pdflink */
    char		*mimetype;	/* attach */

    char		*title;		/* text */
    int			open;		/* text */
    pdf_dest		dest;		/* locallink, pdflink */

    /* -------------- annotation border style and color -------------- */
    pdf_border_style	border_style;
    float		border_width;
    float		border_red;
    float		border_green;
    float		border_blue;
    float		border_dash1;
    float		border_dash2;
};

typedef struct pdf_image_s pdf_image;
typedef struct pdf_font_s pdf_font;

/*
 * *************************************************************************
 * The core PDF document descriptor
 * *************************************************************************
 */

struct PDF_s {
    /* ------------------- PDF Info dictionary entries ----------------- */
    char	*Keywords;
    char	*Subject;
    char	*Title;
    char	*Creator;
    char	*Author;
    /* user-defined key and value */
    char	*userkey;		/* ASCII string */
    char	*userval;		/* Unicode string */

    /* ------------------------- PDF output file ----------------------- */
    FILE	*fp;
    char	*filename;

    /* -------------- error handling and memory management ------------- */
    void 	(*errorhandler)(PDF *p, int level, const char* msg);
    void	*(*malloc)(PDF *p, size_t size, const char *caller);
    void	*(*calloc)(PDF *p, size_t size, const char *caller);
    void	*(*realloc)(PDF *p, void *mem, size_t size,const char *caller);
    void	 (*free)(PDF *p, void *mem);
    void	*opaque;		/* user-specific, opaque data */

    /* ------------------------ resource stuff ------------------------- */
    pdf_category *resources;		/* anchor for the resource list */
    char	*resourcefilename;	/* name of the resource file */
    pdf_bool	resourcefile_loaded;	/* already loaded the resource file */

    /* ------------------- object and id bookkeeping ------------------- */
    id		root_id;
    id		info_id;
    id		pages_id;
    id		open_action;

    long	*file_offset;		/* the objects' file offsets */
    int		file_offset_capacity;
    id		currentobj;

    id		*pages;			/* page ids */
    int		pages_capacity;
    int 	current_page;		/* current page number (1-based) */

    /* ------------------- document resources ------------------- */
    pdf_font	*fonts;			/* all fonts in document */
    int		fonts_capacity;		/* currently allocated size */
    int		fonts_number;		/* next available font number */

    pdf_xobject *xobjects;		/* all xobjects in document */
    int		xobjects_capacity;	/* currently allocated size */
    int		xobjects_number;	/* next available xobject number */

    pdf_image  *images;			/* all images in document */
    int		images_capacity;	/* currently allocated size */

    /* The GIF stuff really belongs into the image descriptor but
     * we don't want to waste 64K for each image. Since GIF decoding
     * is thread-specific, we pack the tables into the PDF struct.
     * TODO: move the GIF stuff to automatic variables in p_gif.c.
     */
#define MAX_LWZ_BITS	12
    int			gif_table[2][(1<< MAX_LWZ_BITS)];
    int			gif_stack[(1<<(MAX_LWZ_BITS))*2];
#undef MAX_LWZ_BITS

    /* ------------------- document outline tree ------------------- */
    int		outline_capacity;	/* currently allocated size */
    int		outline_count;		/* total number of outlines */
    pdf_outline	*outlines;		/* dynamic array of outlines */

    /* ------------------- page specific stuff ------------------- */
    pdf_state	state;			/* state within the library */
    id		res_id;			/* id of this page's res dict */
    id		contents_length_id;	/* id of current cont section's length*/
    id		*contents_ids;		/* content sections' chain */
    int		contents_ids_capacity;	/* # of content sections */
    id		next_content;		/* # of current content section */
    pdf_content_type	contents;	/* type of current content section */
    pdf_transition	transition;	/* type of page transition */
    float	duration;		/* duration of page transition */

    pdf_annot	*annots;		/* annotation chain */

    int		procset;		/* procsets for this page */
    long	start_contents_pos;	/* start offset of contents section */

    float	width;			/* current page's width */
    float	height;			/* current page's height */
    id		thumb_id;		/* id of this page's thumb, or BAD_ID */

    /* ------------------- general graphics state ------------------- */
    /* NYI */

    /* ---------------- other graphics-related stuff ---------------- */
    /* fill rule: non-zero winding number or even-odd */
    pdf_fillrule	fill_rule;

    /* ----------------------- text state --------------------------- */
    float	char_spacing;
    float	leading;
    int		current_font;		/* slot number of current font */
    float	fontsize;		/* current font size */

    /* -------------- annotation border style and color -------------- */
    pdf_border_style	border_style;
    float		border_width;
    float		border_red;
    float		border_green;
    float		border_blue;
    float		border_dash1;
    float		border_dash2;

    /* ------------------------ miscellaneous ------------------------ */
    int		chars_on_this_line;
    int		compress;
    char	debug[128];			/* debug flags */
};

/* Data source for images, compression, ASCII encoding, fonts, etc. */
typedef struct PDF_data_source_s PDF_data_source;
struct PDF_data_source_s {
    byte 		*next_byte;
    size_t		bytes_available;
    void		(*init)(PDF *, PDF_data_source *src);
    int			(*fill)(PDF *, PDF_data_source *src);
    void		(*terminate)(PDF *, PDF_data_source *src);

    byte		*buffer_start;
    size_t		buffer_length;
    void		*private_data;
};

/* JPEG specific image information */
typedef struct PDF_jpeg_info_t {
    int			adobe;		/* JPEG image with Adobe marker */
    long		startpos;	/* start of image data in file */
} PDF_jpeg_info;

/* GIF specific image information */
typedef struct PDF_gif_info_t {
    int			useGlobalColormap;
    int			interlace;

    /* formely static data from p_gif.c */
    char		DoExtension_buf[256];
    int			ZeroDataBlock;
    int			curbit;
    int			lastbit;
    int			done;
    int			last_byte;
    unsigned char	buf[280];
    int			fresh;
    int			code_size, set_code_size;
    int			max_code, max_code_size;
    int			firstcode, oldcode;
    int			clear_code, end_code;
    int			*sp;
} PDF_gif_info;

#ifdef HAVE_LIBTIFF
/* TIFF specific image information */
typedef struct PDF_tiff_info_t {
    TIFF		*tif;		/* pointer to TIFF data structure */
    uint32		*raster;	/* frame buffer */
    int			cur_line;	/* current image line */
} PDF_tiff_info;
#endif

/* CCITT specific image information */
typedef struct PDF_ccitt_info_t {
    char		*params;	/* /K and /BlackIs1 parameters */
    int			BitReverse;	/* reverse all bits prior to use */
} PDF_ccitt_info;

/* Type of image reference */
typedef enum {pdf_ref_direct, pdf_ref_file, pdf_ref_url} pdf_ref_type;

/* The image descriptor */
struct pdf_image_s {
    FILE		*fp;		/* image file pointer */
    char		*filename;	/* image file name or url */
    pdf_ref_type	reference;	/* kind of image data reference */
    int			width;		/* image width in pixels */
    int			height;		/* image height in pixels */
    int			bpc;		/* bits per color component */
    int			components;	/* number of color components */
    pdf_compression	compression;	/* image compression type */
    pdf_colorspace	colorspace;	/* image color space */
    int			indexed;	/* image contains colormap (palette) */
    int			BitPixel;	/* HACK: use bpc instead */
    pdf_colormap 	colormap;
    float    		dpi_x;		/* horiz. resolution in dots per inch */
    float    		dpi_y;		/* vert. resolution in dots per inch */

    int			in_use;		/* image slot currently in use */
    /* image format specific information */
    union {
	PDF_jpeg_info	jpeg;
	PDF_gif_info	gif;
#ifdef HAVE_LIBTIFF
	PDF_tiff_info	tiff;
#endif
	PDF_ccitt_info	ccitt;
    } image_info;

    int			no;		/* PDF image number */
    PDF_data_source	src;
    void		(*closefunc)(PDF *p, int image);
};

/* ------ Private functions for library-internal use only --------- */

/* p_basic.c */
void	pdf_begin_contents_section(PDF *p);
void	pdf_end_contents_section(PDF *p);
void	pdf_error(PDF *, int level, const char *fmt, ...);
id	pdf_begin_obj(PDF *p, id obj_id);
id	pdf_alloc_id(PDF *p);
void	pdf_grow_pages(PDF *p);
void	*pdf_malloc(PDF *p, size_t size, const char *caller);
void	*pdf_calloc(PDF *p, size_t size, const char *caller);
void	*pdf_realloc(PDF *p, void *mem, size_t size, const char *caller);
void	pdf_free(PDF *p, void *mem);

/* p_text.c */
void	pdf_begin_text(PDF *p);
void	pdf_end_text(PDF *p);
void	pdf_quote_string(PDF *p, const char *string);

/* p_gstate.c */
void	pdf_concat(PDF *p, pdf_matrix m);

/* p_image.c */
void	pdf_init_images(PDF *p);
void	pdf_cleanup_images(PDF *p);
void	pdf_init_xobjects(PDF *p);
void	pdf_write_xobjects(PDF *p);
void	pdf_grow_xobjects(PDF *p);
void	pdf_cleanup_xobjects(PDF *p);
void	pdf_put_image(PDF *p, int im);
void	pdf_grow_images(PDF *p);

/* p_filter.c */
void	pdf_ASCIIHexEncode(PDF *p, PDF_data_source *src);

void	pdf_data_source_file_init(PDF *p, PDF_data_source *src);
int	pdf_data_source_file_fill(PDF *p, PDF_data_source *src);
void	pdf_data_source_file_terminate(PDF *p, PDF_data_source *src);

void	pdf_copy(PDF *p, PDF_data_source *src);
void	pdf_compress(PDF *p, PDF_data_source *src);

/* p_font.c */
void	pdf_init_fonts(PDF *p);
void	pdf_write_page_fonts(PDF *p);
void	pdf_write_doc_fonts(PDF *p);
void	pdf_cleanup_fonts(PDF *p);
pdf_bool	pdf_get_metrics_afm(PDF *p, pdf_font *font, const char *fontname, pdf_encoding enc, const char *filename);
pdf_bool	pdf_get_metrics_pfm(PDF *p, pdf_font *font, const char *fontname, pdf_encoding enc, const char *filename);

/* p_afmparse.c */
int pdf_parse_afm (PDF *p, FILE *fp, pdf_font *font);
void	pdf_cleanup_afm(PDF *p, pdf_font *font);

/* p_annots.c */
void	pdf_init_annots(PDF *p);
void	pdf_init_page_annots(PDF *p);
void	pdf_write_page_annots(PDF *p);
void	pdf_cleanup_page_annots(PDF *p);

/* p_hyper.c */
void	pdf_init_transition(PDF *p);
void	pdf_init_outlines(PDF *p);
void	pdf_write_outlines(PDF *p);
void	pdf_cleanup_outlines(PDF *p);

void	pdf_init_info(PDF *p);
void	pdf_write_info(PDF *p);
void	pdf_cleanup_info(PDF *p);

/* p_util.c */
const char   *pdf_float(char *buf, float f);
char   *pdf_strdup(PDF *p, const char *text);
size_t	pdf_strlen(const char *text);

void	pdf_init_resources(PDF *p);
char   *pdf_find_resource(PDF *p, const char *category, const char *resourcename);
void	pdf_add_resource(PDF *p, const char *category, const char *resource,
		const char *filename, const char *prefix);
void	pdf_cleanup_resources(PDF *p);

#endif	/* P_INTERN_H */
