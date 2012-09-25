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

/* p_hyper.c
 *
 * PDFlib routines for hypertext stuff: bookmarks, document info, transitions
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "p_intern.h"

/* We can't work with pointers in the outline objects because
 * the complete outline block may be reallocated. Therefore we use
 * this simple mechanism for achieving indirection.
 */
#define COUNT(index)	(p->outlines[index].count)
#define OPEN(index)	(p->outlines[index].open)
#define LAST(index)	(p->outlines[index].last)
#define PARENT(index)	(p->outlines[index].parent)
#define FIRST(index)	(p->outlines[index].first)
#define SELF(index)	(p->outlines[index].self)
#define PREV(index)	(p->outlines[index].prev)
#define NEXT(index)	(p->outlines[index].next)

void
pdf_init_outlines(PDF *p)
{
    p->outline_count	= 0;
}

/* Free outline entries */
void
pdf_cleanup_outlines(PDF *p)
{
    int i;

    if (p->outline_count == 0)
	return;

    /* outlines[0] is the outline root object */
    for (i = 0; i <= p->outline_count; i++)
	if (p->outlines[i].text)
	    p->free(p, p->outlines[i].text);

    p->free(p, (void*) p->outlines);
}

static void
pdf_write_outline_dict(PDF *p, int entry)
{
    pdf_begin_obj(p, SELF(entry));	/* outline object */
    pdf_begin_dict(p);

    (void) fprintf(p->fp, "/Parent %ld 0 R\n", SELF(PARENT(entry)));

    /* outline destination */
    (void) fprintf(p->fp, "/Dest [%ld 0 R /XYZ 0 0 0]\n", 
    	p->pages[p->outlines[entry].dest.page]);

    (void) fputs("/Title ", p->fp);	/* outline text */
    pdf_quote_string(p, p->outlines[entry].text);
    (void) fputs("\n", p->fp);

    if (PREV(entry))
	(void) fprintf(p->fp, "/Prev %ld 0 R\n", PREV(entry));
    if (NEXT(entry))
	(void) fprintf(p->fp, "/Next %ld 0 R\n", NEXT(entry));

    if (FIRST(entry)) {
	(void) fprintf(p->fp, "/First %ld 0 R\n", SELF(FIRST(entry)));
	(void) fprintf(p->fp, "/Last %ld 0 R\n", SELF(LAST(entry)));
    }
    if (COUNT(entry)) {
	if (OPEN(entry))
	    (void) fprintf(p->fp, "/Count %d\n", COUNT(entry));	/* open */
	else
	    (void) fprintf(p->fp, "/Count %d\n", -COUNT(entry));/* closed */
    }

    pdf_end_dict(p);
    pdf_end_obj(p);			/* outline object */
}

void
pdf_write_outlines(PDF *p)
{
    int i;

    if (p->outline_count == 0)		/* no outlines: return */
	return;

    pdf_begin_obj(p, p->outlines[0].self);	/* root outline object */
    pdf_begin_dict(p);

    if (p->outlines[0].count != 0)
	(void) fprintf(p->fp, "/Count %d\n", COUNT(0));
    (void) fprintf(p->fp, "/First %ld 0 R\n", SELF(FIRST(0)));
    (void) fprintf(p->fp, "/Last %ld 0 R\n", SELF(LAST(0)));

    pdf_end_dict(p);
    pdf_end_obj(p);				/* root outline object */

    for (i = 1; i <= p->outline_count; i++)
	pdf_write_outline_dict(p, i);
}

PDFLIB_API int PDFLIB_CALL
PDF_add_bookmark(PDF *p, const char *text, int parent, int open)
{
    pdf_outline *self;			/* newly created outline */

    if (text == NULL)
	pdf_error(p, PDF_ValueError, "Null bookmark text");

    if (parent < 0 || parent > p->outline_count)
	pdf_error(p, PDF_ValueError,
		"Bad parent id %d for subordinate bookmark!", parent);

    /* create the root outline object */
    if (p->outline_count == 0) {
	p->outlines = (pdf_outline *) p->calloc(p, 
	    sizeof(pdf_outline) * OUTLINE_CHUNKSIZE, "PDF_add_bookmark");
	p->outline_capacity = OUTLINE_CHUNKSIZE;

	/* populate the root outline object */
	p->outlines[0].self	= pdf_alloc_id(p);
	p->outlines[0].count	= 0;
	p->outlines[0].parent	= 0;
	p->outlines[0].open	= 1;
    }

    /* It's crucial to increase p->outline_count only after 
     * successfully having realloc()ed. Otherwise the error handler
     * may try to free to much if the realloc goes wrong.
     */
    if (p->outline_count+1 >= p->outline_capacity) { /* used up all space */
	p->outlines = (pdf_outline *) p->realloc(p, p->outlines,
			sizeof(pdf_outline) * 2 * p->outline_capacity,
			"PDF_add_bookmark");
	p->outline_capacity *= 2;
    }

    p->outline_count++;

    self = &p->outlines[p->outline_count];

    /* If the error handler pops in in pdf_strdup below, we don't
     * want to have the cleanup function try to free self->text.
     */
    self->text		= NULL;

    self->text		= pdf_strdup(p, text);
    self->dest.page	= p->current_page;
    self->self		= pdf_alloc_id(p);
    self->first		= 0;
    self->last		= 0;
    self->prev		= 0;
    self->next		= 0;
    self->count		= 0;
    self->open		= open;
    self->parent	= parent;

    /* insert new outline at the end of the chain or start a new chain */
    if (FIRST(parent) == 0) {
	FIRST(parent) = p->outline_count;
    } else {
	self->prev = SELF(LAST(parent));
	NEXT(LAST(parent))= self->self;
    }

    /* insert new outline as last child of parent in all cases */
    LAST(parent) = p->outline_count;

    /* increase the number of open sub-entries for all relevant ancestors */
    do {
	COUNT(parent)++;
    } while (OPEN(parent) && (parent = PARENT(parent)) != 0);

    return (p->outline_count);		/* caller may use this as handle */
}

void
pdf_init_info(PDF *p)
{
    p->Keywords		= NULL;
    p->Subject		= NULL;
    p->Title		= NULL;
    p->Creator		= NULL;
    p->Author		= NULL;
    p->userkey		= NULL;
    p->userval		= NULL;

    p->info_id		= pdf_alloc_id(p);	/* Info object */
}

/* Set Info dictionary entries */
PDFLIB_API void PDFLIB_CALL
PDF_set_info(PDF *p, const char *key, const char *val)
{
    char *key_buf, *val_buf;

    if (key == NULL || val == NULL) {
	pdf_error(p, PDF_NonfatalError, "Null key or value in PDF_set_info");
	return;
    }

    /* this is needed in all cases */
    val_buf = pdf_strdup(p, val);

    if (!strcmp(key, "Keywords")) {
	p->Keywords = val_buf;
    } else if (!strcmp(key, "Subject")) {
	p->Subject = val_buf;
    } else if (!strcmp(key, "Title")) {
	p->Title = val_buf;
    } else if (!strcmp(key, "Creator")) {
	p->Creator = val_buf;
    } else if (!strcmp(key, "Author")) {
	p->Author = val_buf;
    } else {
	/* user-defined keyword */
	p->userval = val_buf;

	key_buf = pdf_strdup(p, key);
	p->userkey = key_buf;
    }
}

void
pdf_write_info(PDF *p)
{
    time_t	timer;
    struct tm	ltime;

    pdf_begin_obj(p, p->info_id);	/* Info object */

    pdf_begin_dict(p);

    if (p->Keywords) {
	(void) fprintf(p->fp, "/Keywords ");
	pdf_quote_string(p, p->Keywords);
	(void) fprintf(p->fp, "\n");
    }
    if (p->Subject) {
	(void) fprintf(p->fp, "/Subject ");
	pdf_quote_string(p, p->Subject);
	(void) fprintf(p->fp, "\n");
    }
    if (p->Title) {
	(void) fprintf(p->fp, "/Title ");
	pdf_quote_string(p, p->Title);
	(void) fprintf(p->fp, "\n");
    }
    if (p->Creator) {
	(void) fprintf(p->fp, "/Creator ");
	pdf_quote_string(p, p->Creator);
	(void) fprintf(p->fp, "\n");
    }
    if (p->Author) {
	(void) fprintf(p->fp, "/Author ");
	pdf_quote_string(p, p->Author);
	(void) fprintf(p->fp, "\n");
    }
    if (p->userval) {
	(void) fprintf(p->fp, "/%s ", p->userkey);
	pdf_quote_string(p, p->userval);
	(void) fprintf(p->fp, "\n");
    }

    time(&timer);
    ltime = *localtime(&timer);
    (void) fprintf(p->fp, "/CreationDate (D:%04d%02d%02d%02d%02d%02d)\n",
	    ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday,
	    ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

    /* If you change the /Producer entry your license to use PDFlib
     * will be void!
     */
    (void) fprintf(p->fp,
	"/Producer (PDFlib %d.%02d \\(c\\) 1997-99 Thomas Merz)\n",
	PDFLIB_MAJORVERSION, PDFLIB_MINORVERSION);

    pdf_end_dict(p);
    pdf_end_obj(p);			/* Info object */
}

void
pdf_cleanup_info(PDF *p)
{
    /* Free Info dictionary entries */
    if (p->Keywords)  p->free(p, p->Keywords);
    if (p->Subject)  p->free(p, p->Subject);
    if (p->Title)  p->free(p, p->Title);
    if (p->Creator)  p->free(p, p->Creator);
    if (p->Author)  p->free(p, p->Author);
    if (p->userkey)  p->free(p, p->userkey);
    if (p->userval)  p->free(p, p->userval);
}

/* Page transition effects */

void
pdf_init_transition(PDF *p)
{
    p->transition = trans_none;
    p->duration = 0;
}

/* set page display duration for current and future pages */
PDFLIB_API void PDFLIB_CALL
PDF_set_duration(PDF *p, float t)
{
    p->duration = t;
}

/* set transition mode for current and future pages */
PDFLIB_API void PDFLIB_CALL
PDF_set_transition(PDF *p, const char *type)
{
    if (type == NULL)
	p->transition = trans_none;
    else if (!strcmp(type, "none"))
	p->transition = trans_none;
    else if (!strcmp(type, "split"))
	p->transition = trans_split;
    else if (!strcmp(type, "blinds"))
	p->transition = trans_blinds;
    else if (!strcmp(type, "box"))
	p->transition = trans_box;
    else if (!strcmp(type, "wipe"))
	p->transition = trans_wipe;
    else if (!strcmp(type, "dissolve"))
	p->transition = trans_dissolve;
    else if (!strcmp(type, "glitter"))
	p->transition = trans_glitter;
    else if (!strcmp(type, "replace"))
	p->transition = trans_replace;
    else
	pdf_error(p, PDF_ValueError,
		"Unknown page transition type '%s'!", type);
}
