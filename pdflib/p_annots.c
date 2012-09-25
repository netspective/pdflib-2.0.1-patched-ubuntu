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

/* p_annots.c
 *
 * PDFlib routines for annnotations
 *
 */

#include <string.h>

#include "p_intern.h"

static const char *pdf_border_style_names[] = {
    "S",	/* solid border */
    "D",	/* dashed border */
    "B",	/* beveled (three-dimensional) border */
    "I",	/* inset border */
    "U"		/* underlined border */
};

static const char *pdf_icon_names[] = {
    /* embedded file icon names */
    "Graph", "Paperclip", "Pushpin", "Tag",

    /* text annotation icon names */
    "Comment", "Insert", "Note", "Paragraph", "NewParagraph", "Key", "Help"
};

/* flags for annotation properties */
typedef enum {
    pdf_ann_flag_invisible	= 1,
    pdf_ann_flag_hidden		= 2,
    pdf_ann_flag_print		= 4,
    pdf_ann_flag_nozoom		= 8,
    pdf_ann_flag_norotate	= 16,
    pdf_ann_flag_noview		= 32,
    pdf_ann_flag_readonly	= 64
} pdf_ann_flag;

void
pdf_init_annots(PDF *p)
{
    /* annotation border style defaults */
    p->border_style	= border_solid;
    p->border_width	= (float) 1.0;
    p->border_red	= (float) 0.0;
    p->border_green	= (float) 0.0;
    p->border_blue	= (float) 0.0;
    p->border_dash1	= (float) 3.0;
    p->border_dash2	= (float) 3.0;
}

/* Write annotation border style and color */
static void
pdf_write_border_style(PDF *p, pdf_annot *ann)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];

    /* don't write the default values */
    if (ann->border_style == border_solid && ann->border_width == (float) 1.0 &&
	p->border_red == (float) 0.0 && p->border_green == (float) 0.0 &&
	p->border_blue == (float) 0.0 &&
	p->border_dash1 == (float) 3.0 && p->border_dash2 == (float) 3.0)
	return;

    (void) fprintf(p->fp, "/BS ");
    pdf_begin_dict(p);			/* BS dict */
    (void) fputs("/Type /Border\n", p->fp);

    if (ann->border_style != border_solid)
	(void) fprintf(p->fp, "/S /%s\n",
	    pdf_border_style_names[ann->border_style]);

    if (ann->border_style == border_dashed)
	(void) fprintf(p->fp, "/D [%s %s]\n",
	    pdf_float(buf1, ann->border_dash1),
	    pdf_float(buf2, ann->border_dash2));

    pdf_end_dict(p);			/* BS dict */

    /* Write the Border key in old-style PDF 1.1 format */
    (void) fprintf(p->fp, "/Border [0 0 %s",
	pdf_float(buf1, ann->border_width));

    if (ann->border_style == border_dashed &&
	(ann->border_dash1 != (float) 0.0 || ann->border_dash2 != (float) 0.0))
	/* set dashed border */
	(void) fprintf(p->fp, "[%s %s]", pdf_float(buf1, ann->border_dash1),
					   pdf_float(buf2, ann->border_dash2));
    (void) fprintf(p->fp, "]\n");


    /* write annotation color */
    (void) fprintf(p->fp, "/C [%s %s %s]\n",
	    pdf_float(buf1, ann->border_red),
	    pdf_float(buf2, ann->border_green),
	    pdf_float(buf3, ann->border_blue));
}

void
pdf_write_page_annots(PDF *p)
{
    pdf_annot	*ann;
    long	length, start_pos;
    id		length_id;
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];
    PDF_data_source src;

    for (ann = p->annots; ann != NULL; ann = ann->next) {
	pdf_begin_obj(p, ann->obj_id);	/* Annotation object */
	pdf_begin_dict(p);		/* Annotation dict */

	(void) fputs("/Type /Annot\n", p->fp);
	switch (ann->type) {
	    case ann_text:
		(void) fputs("/Subtype /Text\n", p->fp);
		fprintf(p->fp, "/Rect [%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		if (ann->open)
		    (void) fputs("/Open true\n", p->fp);

		if (ann->icon != icon_text_note)	/* note is default */
		    (void) fprintf(p->fp, "/Name /%s\n",
		    	pdf_icon_names[ann->icon]);

		/* Contents key is required, but may be empty */
		(void) fprintf(p->fp, "/Contents ");

		if (ann->contents) {
		    pdf_quote_string(p, ann->contents);
		    fputs("\n", p->fp);
		} else
		    fputs("()\n", p->fp);	/* empty contents is OK */

		/* title is optional */
		if (ann->title) {
		    (void) fprintf(p->fp, "/T ");
		    pdf_quote_string(p, ann->title);
		    fputs("\n", p->fp);
		}

		break;

	    case ann_locallink:
		(void) fputs("/Subtype /Link\n", p->fp);
		fprintf(p->fp, "/Rect [%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		/* preallocate page object id for a later page */
		if (ann->dest.page > p->current_page) {
		    while (ann->dest.page >= p->pages_capacity)
			pdf_grow_pages(p);

		    /* if this page has already been used as a link target
		     * it will already have an object id.
		     */
		    if (p->pages[ann->dest.page] == BAD_ID)
			p->pages[ann->dest.page] = pdf_alloc_id(p);
		}

		if (ann->dest.type == retain) {
		    (void) fprintf(p->fp, "/Dest [%ld 0 R /XYZ null null 0]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitpage) {
		    (void) fprintf(p->fp, "/Dest [%ld 0 R /Fit]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitwidth) {
		    (void) fprintf(p->fp, "/Dest [%ld 0 R /FitH 0]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitheight) {
		    (void) fprintf(p->fp, "/Dest [%ld 0 R /FitV 0]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitbbox) {
		    (void) fprintf(p->fp, "/Dest [%ld 0 R /FitB]\n",
			    p->pages[ann->dest.page]);
		}

		break;

	    case ann_pdflink:
		(void) fputs("/Subtype /Link\n", p->fp);
		fprintf(p->fp, "/Rect [%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		(void) fprintf(p->fp, "/A ");
		pdf_begin_dict(p);			/* A dict */
		(void) fprintf(p->fp, "/Type /Action /S /GoToR\n");

		if (ann->dest.type == retain) {
		    (void) fprintf(p->fp, "/D [%d 0 R /XYZ null null 0]\n",
			    ann->dest.page-1);	/* zero-based */
		} else if (ann->dest.type == fitpage) {
		    (void) fprintf(p->fp, "/D [%d 0 R /Fit]\n",
			    ann->dest.page-1);	/* zero-based */
		} else if (ann->dest.type == fitwidth) {
		    /* Trick: we don't know the height of a future page yet,
		     * so we use a "large" value for top which will do for
		     * most pages. If it doesn't work, not much harm is done.
		     */
		    (void) fprintf(p->fp, "/D [%d 0 R /FitH 2000]\n",
			    ann->dest.page-1);	/* zero-based */
		} else if (ann->dest.type == fitheight) {
		    (void) fprintf(p->fp, "/D [%d 0 R /FitV 0]\n",
			    ann->dest.page-1);	/* zero-based */
		} else if (ann->dest.type == fitbbox) {
		    (void) fprintf(p->fp, "/D [%d 0 R /FitB]\n",
			    ann->dest.page-1);	/* zero-based */
		}

		(void) fprintf(p->fp, "/F ");
		pdf_begin_dict(p);			/* F dict */
		(void) fprintf(p->fp, "/Type /FileSpec\n");
		(void) fprintf(p->fp, "/F (%s)\n", ann->filename);
		pdf_end_dict(p);			/* F dict */

		pdf_end_dict(p);			/* A dict */

		break;

	    case ann_launchlink:
		(void) fputs("/Subtype /Link\n", p->fp);
		fprintf(p->fp, "/Rect [%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		(void) fprintf(p->fp, "/A ");
		pdf_begin_dict(p);			/* A dict */
		(void) fprintf(p->fp, "/Type /Action /S /Launch\n");

		(void) fprintf(p->fp, "/F ");
		pdf_begin_dict(p);			/* F dict */
		(void) fprintf(p->fp, "/Type /FileSpec\n");
		(void) fprintf(p->fp, "/F (%s)\n", ann->filename);
		pdf_end_dict(p);			/* F dict */

		pdf_end_dict(p);			/* A dict */

		break;

	    case ann_weblink:
		(void) fputs("/Subtype /Link\n", p->fp);
		fprintf(p->fp, "/Rect [%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		(void) fprintf(p->fp, "/A <</S /URI /URI (%s)>>\n", 
				    ann->filename);
		break;

	    case ann_attach:
		(void) fputs("/Subtype /FileAttachment\n", p->fp);
		fprintf(p->fp, "/Rect [%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		if (ann->icon != icon_file_pushpin)	/* pushpin is default */
		    (void) fprintf(p->fp, "/Name /%s\n",
		    	pdf_icon_names[ann->icon]);

		if (ann->title) {
		    (void) fprintf(p->fp, "/T ");
		    pdf_quote_string(p, ann->title);
		    fputs("\n", p->fp);
		}

		if (ann->contents) {
		    (void) fprintf(p->fp, "/Contents ");
		    pdf_quote_string(p, ann->contents);
		    fputs("\n", p->fp);
		}
		    
		/* the icon is too small without these flags (=28) */
		(void) fprintf(p->fp, "/F %d\n",
			pdf_ann_flag_print |
			pdf_ann_flag_nozoom | 
			pdf_ann_flag_norotate);

		(void) fprintf(p->fp, "/FS ");
		pdf_begin_dict(p);			/* FS dict */
		(void) fprintf(p->fp, "/Type /FileSpec\n");

		(void) fprintf(p->fp, "/F (%s)\n", ann->filename);

		/* alloc id for the actual embedded file stream */
		ann->obj_id = pdf_alloc_id(p);
		(void) fprintf(p->fp, "/EF << /F %ld 0 R>>\n", ann->obj_id);
		pdf_end_dict(p);			/* FS dict */

		break;

	    default:
		pdf_error(p, PDF_SystemError, "Unknown annotation type %d!",
				ann->type);
	}

	pdf_end_dict(p);		/* Annotation dict */
	pdf_end_obj(p);			/* Annotation object */
    }

    /* Write the actual embedded files with preallocated ids */
    for (ann = p->annots; ann != NULL; ann = ann->next) {
	if (ann->type != ann_attach)
	    continue;

	pdf_begin_obj(p, ann->obj_id);	/* EmbeddedFile */
	(void) fprintf(p->fp, "<</Type /EmbeddedFile\n");

	if (ann->mimetype)
	    (void) fprintf(p->fp, "/Subtype (%s)\n", ann->mimetype);

	if (p->compress)
	    (void) fprintf(p->fp, "/Filter /FlateDecode\n");

	length_id = pdf_alloc_id(p);
	(void) fprintf(p->fp, "/Length %ld 0 R\n", length_id);
	pdf_end_dict(p);		/* F dict */

	pdf_begin_stream(p);		/* Embedded file stream */
	start_pos = ftell(p->fp);

	/* write the file in the PDF */
	src.private_data = (void *) ann->filename;
	src.init	= pdf_data_source_file_init;
	src.fill	= pdf_data_source_file_fill;
	src.terminate	= pdf_data_source_file_terminate;

	if (p->compress)
	    pdf_compress(p, &src);
	else
	    pdf_copy(p, &src);

	length = ftell(p->fp) - start_pos;
	pdf_end_stream(p);		/* Embedded file stream */
	pdf_end_obj(p);			/* EmbeddedFile object */

	pdf_begin_obj(p, length_id);	/* Length object */
	(void) fprintf(p->fp, "%ld\n", length);
	pdf_end_obj(p);			/* Length object */
    }
}

void
pdf_init_page_annots(PDF *p)
{
    p->annots = NULL;
}

void
pdf_cleanup_page_annots(PDF *p)
{
    pdf_annot *ann, *old;

    for (ann = p->annots; ann != (pdf_annot *) NULL; /* */ ) {
	switch (ann->type) {
	    case ann_text:
		if (ann->contents)
		    p->free(p, ann->contents);
		if (ann->title)
		    p->free(p, ann->title);
		break;

	    case ann_locallink:
		break;

	    case ann_launchlink:
		p->free(p, ann->filename);
		break;

	    case ann_pdflink:
		p->free(p, ann->filename);
		break;

	    case ann_weblink:
		p->free(p, ann->filename);
		break;

	    case ann_attach:
		p->free(p, ann->filename);
		if (ann->contents)
		    p->free(p, ann->contents);
		if (ann->title)
		    p->free(p, ann->title);
		if (ann->mimetype)
		    p->free(p, ann->mimetype);
		break;

	    default:
		pdf_error(p, PDF_SystemError, "Unknown annotation type %d!",
				ann->type);
	}
	old = ann;
	ann = old->next;
	p->free(p, old);
    }
}

/* Insert new annotation at the end of the annots chain */
static void
pdf_add_annot(PDF *p, pdf_annot *ann)
{
    pdf_annot *last;

    /* fetch current border state from p */
    ann->border_style	= p->border_style;
    ann->border_width	= p->border_width;
    ann->border_red	= p->border_red;
    ann->border_green	= p->border_green;
    ann->border_blue	= p->border_blue;
    ann->border_dash1	= p->border_dash1;
    ann->border_dash2	= p->border_dash2;

    ann->next = NULL;

    if (p->annots == NULL)
	p->annots = ann;
    else {
	for (last = p->annots; last->next != NULL; /* */ )
	    last = last->next;
	last->next = ann;
    }
}

/* Attach an arbitrary file to the PDF. Note that the actual
 * embedding takes place in PDF_end_page().
 * description, author, and mimetype may be NULL.
 */
PDFLIB_API void PDFLIB_CALL
PDF_attach_file(PDF *p, float llx, float lly, float urx, float ury, const char *filename, const char *description, const char *author, const char *mimetype, const char *icon)
{
    pdf_annot *ann;

    if (filename == NULL)
	pdf_error(p, PDF_ValueError, "Empty file name for file attachment!");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot),"PDF_attach_file");

    ann->type	  = ann_attach;
    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    if (icon == NULL)
	ann->icon = icon_file_pushpin;
    else if (!strcmp(icon, "graph"))
	ann->icon = icon_file_graph;
    else if (!strcmp(icon, "paperclip"))
	ann->icon = icon_file_paperclip;
    else if (!strcmp(icon, "pushpin"))
	ann->icon = icon_file_pushpin;
    else if (!strcmp(icon, "tag"))
	ann->icon = icon_file_tag;
    else
	pdf_error(p, PDF_ValueError, "Unknown icon type '%s'for embedded file!",
			icon);

    ann->filename = (char *) pdf_strdup(p, filename);

    if (description != NULL)
	ann->contents = (char *) pdf_strdup(p, description);

    if (author != NULL)
	ann->title = (char *) pdf_strdup(p, author);

    if (mimetype != NULL)
	ann->mimetype = (char *) pdf_strdup(p, mimetype);

    pdf_add_annot(p, ann);
}

PDFLIB_API void PDFLIB_CALL
PDF_add_note(PDF *p, float llx, float lly, float urx, float ury, const char *contents, const char *title, const char *icon, int open)
{
    pdf_annot *ann;
    size_t len;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_note)!");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "pdf_add_note");

    ann->type	  = ann_text;
    ann->open	  = open;
    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    if (icon == NULL)
	ann->icon = icon_text_note;
    else if (!strcmp(icon, "comment"))
	ann->icon = icon_text_comment;
    else if (!strcmp(icon, "insert"))
	ann->icon = icon_text_insert;
    else if (!strcmp(icon, "note"))
	ann->icon = icon_text_note;
    else if (!strcmp(icon, "paragraph"))
	ann->icon = icon_text_paragraph;
    else if (!strcmp(icon, "newparagraph"))
	ann->icon = icon_text_newparagraph;
    else if (!strcmp(icon, "key"))
	ann->icon = icon_text_key;
    else if (!strcmp(icon, "help"))
	ann->icon = icon_text_help;
    else
	pdf_error(p, PDF_ValueError,
		"Unknown icon type '%s' for text note!", icon);
	
    /* title may be NULL */
    if (title != NULL) {
	len = pdf_strlen(title);
	ann->title = (char *) p->malloc(p, len, "pdf_add_note_core");
	memcpy(ann->title, title, len);
    } else
	ann->title = NULL;

    /* It is legal to create an empty text annnotation */
    if (contents != NULL) {
	len = pdf_strlen(contents);
	ann->contents = (char *) p->malloc(p, len, "pdf_add_note_core");
	memcpy(ann->contents, contents, len);
    } else
	ann->contents = NULL;

    pdf_add_annot(p, ann);
}

/* Add a link to another PDF file */
PDFLIB_API void PDFLIB_CALL
PDF_add_pdflink(PDF *p, float llx, float lly, float urx, float ury, const char *filename, int page, const char *desttype)
{
    pdf_annot *ann;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_pdflink)!");

    if (filename == NULL)
	pdf_error(p, PDF_ValueError, "NULL filename in PDF_add_pdflink");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_pdflink");

    ann->filename = pdf_strdup(p, filename);

    ann->type	  	= ann_pdflink;
    ann->dest.page 	= page;

    if (desttype == NULL)
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "retain"))
	ann->dest.type 	= retain;
    else if (!strcmp(desttype, "fitpage"))
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "fitwidth"))
	ann->dest.type 	= fitwidth;
    else if (!strcmp(desttype, "fitheight"))
	ann->dest.type 	= fitheight;
    else if (!strcmp(desttype, "fitbbox"))
	ann->dest.type 	= fitbbox;
    else
	pdf_error(p, PDF_ValueError,
		"Unknown destination type '%s' in PDF_add_pdflink!", desttype);

    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;


    pdf_add_annot(p, ann);
}

/* Add a link to another file of an arbitrary type */
PDFLIB_API void PDFLIB_CALL
PDF_add_launchlink(PDF *p, float llx, float lly, float urx, float ury, const char *filename)
{
    pdf_annot *ann;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_launchlink)!");

    if (filename == NULL)
	pdf_error(p, PDF_ValueError, "NULL filename in PDF_add_launchlink");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_launchlink");

    ann->filename = pdf_strdup(p, filename);

    ann->type	  = ann_launchlink;

    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;


    pdf_add_annot(p, ann);
}

/* Add a link to a destination in the current PDF file */
PDFLIB_API void PDFLIB_CALL
PDF_add_locallink(PDF *p, float llx, float lly, float urx, float ury, int page, const char *desttype)
{
    pdf_annot *ann;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_locallink)!");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_locallink");

    ann->type	  	= ann_locallink;
    ann->dest.page	= page;

    if (desttype == NULL)
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "retain"))
	ann->dest.type 	= retain;
    else if (!strcmp(desttype, "fitpage"))
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "fitwidth"))
	ann->dest.type 	= fitwidth;
    else if (!strcmp(desttype, "fitheight"))
	ann->dest.type 	= fitheight;
    else if (!strcmp(desttype, "fitbbox"))
	ann->dest.type 	= fitbbox;
    else
	pdf_error(p, PDF_ValueError,	
	    "Unknown destination type '%s' in PDF_add_locallink!", desttype);

    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    pdf_add_annot(p, ann);
}

/* Add a link to an arbitrary Internet resource (URL) */
PDFLIB_API void PDFLIB_CALL
PDF_add_weblink(PDF *p, float llx, float lly, float urx, float ury, const char *url)
{
    pdf_annot *ann;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_weblink)!");

    if (url == NULL || *url == '\0')
	pdf_error(p, PDF_ValueError, "NULL URL in PDF_add_weblink!");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_weblink");

    ann->filename = pdf_strdup(p, url);

    ann->type	  = ann_weblink;
    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    pdf_add_annot(p, ann);
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_style(PDF *p, const char *style, float width)
{
    if (style == NULL)
	p->border_style = border_solid;
    else if (!strcmp(style, "solid"))
	p->border_style = border_solid;
    else if (!strcmp(style, "dashed"))
	p->border_style = border_dashed;
    else
	pdf_error(p, PDF_ValueError,
		"Unknown annotation border style '%s'", style);

    if (width < 0.0)
	pdf_error(p, PDF_ValueError,
		"Negative annotation border width %f", width);

    p->border_width = width;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_color(PDF *p, float red, float green, float blue)
{
    if (red < 0.0 || red > 1.0)
	pdf_error(p, PDF_ValueError,
		"Bogus red color value %f for annotation border", red);
    if (green < 0.0 || green > 1.0)
	pdf_error(p, PDF_ValueError,
		"Bogus green color value %f for annotation border", green);
    if (blue < 0.0 || blue > 1.0)
	pdf_error(p, PDF_ValueError,
		"Bogus blue color value %f for annotation border", blue);

    p->border_red = red;
    p->border_green = green;
    p->border_blue = blue;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_dash(PDF *p, float d1, float d2)
{
    if (d1 < 0.0)
	pdf_error(p, PDF_ValueError,
		"Negative first annotation border dash value %f", d1);
    if (d2 < 0.0)
	pdf_error(p, PDF_ValueError,
		"Negative second annotation border dash value %f", d2);

    p->border_dash1 = d1;
    p->border_dash2 = d2;
}
