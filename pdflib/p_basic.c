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

/* p_basic.c
 *
 * PDFlib general routines
 *
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "p_intern.h"

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef MAC
#include <Files.h>
#endif

#ifdef __WATCOMC__
#include <io.h>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef HAVE_SETLOCALE
#include <locale.h>
#endif

/* Boot the library core. Currently not much happens here.
 * It is safe to call PDF_boot() multiply -- not all bindings
 * support a single initialization routine.
 */
PDFLIB_API void PDFLIB_CALL
PDF_boot(void)
{
#ifdef HAVE_SETLOCALE
    /* Avoid NLS messing with our decimal output separator since
     * PDF doesn't like decimal commas very much...
     * Systems without setlocale most probably don't mess with printf()
     * formats in the first place, so it doesn't hurt to leave this
     * code out on such systems.
     */
    setlocale(LC_NUMERIC, "C");
#endif
}

PDFLIB_API void PDFLIB_CALL
PDF_shutdown(void)
{
    /* */
}

#if defined(WIN32) && defined(PDFLIB_EXPORTS) && !defined(SWIGWIN)

/* DLL entry function as required by Visual C++.
 * It is currently not necessary on Windows, but will eventually 
 * be used to boot thread-global resources for PDFlib
 * (mainly font-related stuff).
 */
BOOL WINAPI
DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	    PDF_boot();
	    break;
	case DLL_THREAD_ATTACH:
	    break;
	case DLL_THREAD_DETACH:
	    break;
	case DLL_PROCESS_DETACH:
	    PDF_shutdown();
	    break;
    }

    return TRUE;
}
#endif	/* WIN32 && PDFLIB_EXPORT && !SWIGWIN */

PDFLIB_API int PDFLIB_CALL
PDF_get_majorversion()
{
    return PDFLIB_MAJORVERSION;
}

PDFLIB_API int PDFLIB_CALL
PDF_get_minorversion()
{
    return PDFLIB_MINORVERSION;
}

/* This list must be kept in sync with the corresponding #defines in pdflib.h */
static const char *pdf_error_names[] = {
    NULL,
    "memory error",
    "I/O error",
    "runtime error",
    "index error",
    "type error",
    "division by zero error",
    "overflow error",
    "syntax error",
    "value error",
    "system error",
    "warning (ignored)",
    "unknown error"
};

static void
pdf_c_errorhandler(PDF *p, int type, const char* shortmsg)
{
    char msg[256];

    /* A line like this allows custom error handlers to supply their 
     * own program name
     */
    sprintf(msg, "PDFlib %s: %s\n", pdf_error_names[type], shortmsg);

    switch (type) {
	/* Issue a warning message and continue */
	case PDF_NonfatalError:
		fprintf(stderr, msg);
		return;

	/* give up in all other cases */
	case PDF_MemoryError:
	case PDF_IOError:
	case PDF_RuntimeError:
	case PDF_IndexError:
	case PDF_TypeError:
	case PDF_DivisionByZero:
	case PDF_OverflowError:
	case PDF_SyntaxError:
	case PDF_ValueError:
	case PDF_SystemError:
	case PDF_UnknownError:
	default:
		fprintf(stderr, msg);		/* print message */

		if (p != NULL) {		/* first allocation? */
		    /* delete incomplete PDF file */
		    if(!p->debug['u'])
			(void) unlink(p->filename);

		    PDF_delete(p);	/* clean up PDFlib */
		}
		exit(99);			/* good-bye */
    }
}

void
pdf_error(PDF *p, int type, const char *fmt, ...)
{
    char msg[256];

    va_list ap;

    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);

    /*
     * We catch non-fatals here since user-supplied error handlers
     * don't know about the debug level.
     */
    if (type != PDF_NonfatalError || ((PDF *)p)->debug['e'])
	(p->errorhandler)(p, type, msg);

    va_end(ap);
}

static void
pdf_init_document(PDF *p)
{
    id i;

    p->file_offset_capacity = ID_CHUNKSIZE;
    p->file_offset = (long *) p->malloc(p, 
	    sizeof(long) * p->file_offset_capacity, "PDF_init_document");

    p->contents_ids_capacity = CONTENTS_CHUNKSIZE;
    p->contents_ids = (id *) p->malloc(p, 
	    sizeof(id) * p->contents_ids_capacity, "PDF_init_document");

    p->pages_capacity = PAGES_CHUNKSIZE;
    p->pages = (id *) p->malloc(p,
	    sizeof(id) * p->pages_capacity, "PDF_init_document");

    /* mark ids to allow for pre-allocation of page ids */
    for (i = 0; i < p->pages_capacity; i++)
	p->pages[i] = BAD_ID;

    p->filename		= NULL;
    p->resources	= NULL;
    p->resourcefile_loaded = pdf_false;
    p->fp		= NULL;
    p->currentobj	= (id) 0;
    p->current_page	= 0;
    p->pages_id		= pdf_alloc_id(p);
    p->root_id		= pdf_alloc_id(p);
    p->state		= pdf_state_open;
}

/* Free all document-related resources */
static void
pdf_cleanup_document(PDF *p)
{
    if (p->file_offset)
	p->free(p, p->file_offset);
    if (p->contents_ids)
	p->free(p, p->contents_ids);
    if (p->pages)
	p->free(p, p->pages);
    if (p->resourcefilename)
	p->free(p, p->resourcefilename);
}

/* Free all page-related resources */
static void
pdf_cleanup_page(PDF *p)
{
    pdf_cleanup_page_annots(p);
}

/* p may be NULL on the first call - we don't use it anyway */
void *
pdf_malloc(PDF *p, size_t size, const char *caller)
{
    void *ret;

    ret = malloc(size);

#ifdef DEBUG
    if (p != NULL && p->debug['m'])
	printf("malloc %x, size %d from %s, page %d\n",
		(int)ret, size, caller, p->current_page);
#endif

    /* Special error handling at first allocation (for p itself) */
    if (p != NULL) {
	if (ret == NULL) {
	    pdf_error(p, PDF_MemoryError,
		    "Couldn't allocate memory in %s!\n", caller);
	}
    }

    return ret;
}

/* We cook up our own calloc routine, using the caller-supplied 
 * malloc and memset.
 */
void *
pdf_calloc(PDF *p, size_t size, const char *caller)
{
    void *ret;

    if ((ret = p->malloc(p, size, caller)) == NULL) {
	pdf_error(p, PDF_MemoryError,
		"Couldn't (c)allocate memory in %s!\n", caller);
    }
    memset(ret, 0, size);

#ifdef DEBUG
    if (p->debug['m'])
	printf("calloc %x, size %d from %s, page %d\n",
		(int)ret, size, caller, p->current_page);
#endif

    return ret;
}

void *
pdf_realloc(PDF *p, void *mem, size_t size, const char *caller)
{
    void *ret;

    if ((ret = realloc(mem, size)) == NULL) {
	pdf_error(p, PDF_MemoryError,
		"Couldn't reallocate memory in %s!\n", caller);
    }

#ifdef DEBUG
    if (p->debug['r'])
	printf("realloced %d at %x from %s, page %d\n",
		size,(int) ret, caller, p->current_page);
#endif
    return ret;
}

void
pdf_free(PDF *p, void *mem)
{
#ifdef DEBUG
    if (p->debug['f'])
	printf("free %x, page %d\n", (int) mem, p->current_page);
#endif

    /* We mustn't raise a fatal error here to avoid potential recursion */
    if (mem == NULL) {
	pdf_error(p, PDF_NonfatalError,
		"(Internal) Tried to free null pointer!");
	return;
    }

    free(mem);
}

#undef PDF_new

/* This is the easy version with the default C error handler */
PDFLIB_API PDF * PDFLIB_CALL
PDF_new()
{
    return PDF_new2(pdf_c_errorhandler, NULL, NULL, NULL, NULL);
}

/* This is the spiced-up version with user-defined error and memory handlers */

PDFLIB_API PDF * PDFLIB_CALL
PDF_new2(
    void  (*errorhandler)(PDF *p, int type, const char *msg),
    void* (*allocproc)(PDF *p, size_t size, const char *caller),
    void* (*reallocproc)(PDF *p, void *mem, size_t size, const char *caller),
    void  (*freeproc)(PDF *p, void *mem),
    void   *opaque)
{
    PDF *p;
    int i;


    if (errorhandler == NULL)
	errorhandler = pdf_c_errorhandler;

    /* If allocproc is NULL, all entries are supplied internally by PDFlib */
    if (allocproc == NULL) {
	allocproc	= pdf_malloc;
	reallocproc	= pdf_realloc;
	freeproc	= pdf_free;
    }
	
    /* If this goes wrong the error handler won't be able to jump in
     * automatically because there's no p; Therefore call the handler manually
     * in case of error.
     */
    p = (PDF *) (*allocproc) (NULL, sizeof(PDF), "PDF_new2");

    if (p == NULL) {
	(*errorhandler)(NULL, PDF_MemoryError, "Couldn't allocate PDF object");
	return NULL;	/* Won't happen because of the error handler */
    }

    p->errorhandler	= errorhandler;
    p->malloc		= allocproc;
    p->realloc		= reallocproc;
    p->free		= freeproc;

    p->opaque		= opaque;
    p->calloc		= pdf_calloc;

#ifdef HAVE_LIBZ
    p->compress		= 1;
#else
    p->compress		= 0;
#endif

    p->resourcefilename	= NULL;
    p->filename		= NULL;

    /* clear all debug flags... */
    for(i = 0; i < 128; i++)
	p->debug[i] = 0;

    /* ...except warning messages for non-fatal errors -- the
     * client must explicitly disable these.
     */
    p->debug['e'] = 1;

    return p;
}

PDFLIB_API void * PDFLIB_CALL
PDF_get_opaque(PDF *p)
{
    return p->opaque;
}

static void
pdf_close_file(PDF *p)
{
#ifdef MAC
    FCBPBRec	fcbInfo;
    Str32	name;
    FInfo	fInfo;
    FSSpec	fSpec;
#endif


#ifdef MAC
/*  Contributed by Leonard Rosenthol:
 *  On the MacOS, files are not associated with applications by extensions.
 *  Instead, it uses a pair of values called the type & creator.
 *  This block of code sets those values for PDF files.
 */
    memset(&fcbInfo, 0, sizeof(FCBPBRec));
    fcbInfo.ioRefNum = (short) p->fp->handle;
    fcbInfo.ioNamePtr = name;
    if (!PBGetFCBInfoSync(&fcbInfo) &&
	FSMakeFSSpec(fcbInfo.ioFCBVRefNum, fcbInfo.ioFCBParID, name, &fSpec)
		== noErr) {
	    FSpGetFInfo(&fSpec, &fInfo);
	    fInfo.fdType = 'PDF ';
	    fInfo.fdCreator = 'CARO';
	    FSpSetFInfo(&fSpec, &fInfo);
	}
#endif

    /* If filename is set, we started with PDF_open_file; therefore
     * we also close the file ourselves.
     */
    if (p->filename) {
	p->free(p, p->filename);
	fclose(p->fp);
    }

    /* mark p as dead in case the error handler jumps in later */
    p->fp = NULL;
}

/* 
 * PDF_delete must be called for cleanup in case of error,
 * or when the client is done producing PDF.
 * It must never be called more than once for a given PDF.
 */

PDFLIB_API void PDFLIB_CALL
PDF_delete(PDF *p)
{
    /* close the output file if necessary */
    if (p->fp)
	pdf_close_file(p);

    /* clean up page-related stuff if necessary */
    if (p->state == pdf_state_page_description)
	pdf_cleanup_page(p);

    /* clean up document-related stuff */
    pdf_cleanup_document(p);
    pdf_cleanup_info(p);
    pdf_cleanup_images(p);
    pdf_cleanup_xobjects(p);
    pdf_cleanup_fonts(p);
    pdf_cleanup_outlines(p);
    pdf_cleanup_resources(p);

    /* free the PDF structure */
    p->free(p, (void *)p);
}

static void
pdf_init_all(PDF *p)
{
    pdf_init_document(p);
    pdf_init_info(p);
    pdf_init_images(p);
    pdf_init_xobjects(p);
    pdf_init_fonts(p);
    pdf_init_transition(p);
    pdf_init_outlines(p);
    pdf_init_annots(p);
}

static void
pdf_write_header(PDF *p)
{
    /* Write the document header */
    /* The %PDF-1.3 header should also work with older viewers */

    (void) fputs("%PDF-1.3\n", p->fp);	      /* Header */
    (void) fputs("%\342\343\317\323\n", p->fp);   /* binary magic number */

}

PDFLIB_API int PDFLIB_CALL
PDF_open_fp(PDF *p, FILE *fp)
{
    pdf_init_all(p);
    p->fp = fp;

    pdf_write_header(p);

    return pdf_true;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_file(PDF *p, const char *filename)
{
    pdf_init_all(p);

    if ((p->fp = fopen(filename, WRITEMODE)) == NULL)
	return -1;

    p->filename = pdf_strdup(p, filename);

    pdf_write_header(p);

    return pdf_true;
}

/* Write all pending document information up to the xref table and trailer */
static void
pdf_wrapup_document(PDF *p)
{
    long	pos;
    int		page;
    id		i;

    if (p->state == pdf_state_page_description) {
	PDF_end_page(p);
	pdf_error(p, PDF_NonfatalError, "Didn't close last page");
    }
    if (p->state != pdf_state_open)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_close)");

    if (p->current_page == 0 ) {		/* avoid empty document */
	/* create 1-page document with arbitrary page size */
	PDF_begin_page(p, (float) 100, (float) 100);
	PDF_end_page(p);
	pdf_error(p, PDF_NonfatalError, "Empty document");
    }

    pdf_write_info(p);

    p->open_action = pdf_begin_obj(p, NEW_ID);
    (void) fprintf(p->fp,"[ %ld 0 R /Fit ]\n", p->pages[1]);
    pdf_end_obj(p);

    pdf_write_doc_fonts(p);			/* font objects */

    pdf_begin_obj(p, p->pages_id);		/* root pages object */
    pdf_begin_dict(p);
    (void) fputs("/Type /Pages\n", p->fp);
    (void) fprintf(p->fp,"/Count %d\n", p->current_page);

    (void) fputs("/Kids [", p->fp);
    for (page = 1; page <= p->current_page; page++) {
	(void) fprintf(p->fp,"%ld 0 R", p->pages[page]);
	(void) fputs(page % 8 ? " " : "\n", p->fp);
    }
    (void) fputs("]\n", p->fp);

    pdf_end_dict(p);
    pdf_end_obj(p);

    pdf_begin_obj(p, p->root_id);		/* Catalog or Root object */
    pdf_begin_dict(p);
    (void) fputs("/Type /Catalog\n", p->fp);

    /* make the first page fit in the window */
    (void) fprintf(p->fp,"/OpenAction %ld 0 R\n", p->open_action);

    /* open outlines if the document has any */
    if (p->outline_count > 0)
	(void) fputs("/PageMode /UseOutlines\n", p->fp);

    						/* Pages object */
    (void) fprintf(p->fp,"/Pages %ld 0 R\n", p->pages_id);
    if (p->outline_count != 0)
	(void) fprintf(p->fp,"/Outlines %ld 0 R\n", p->outlines[0].self);

    pdf_end_dict(p);
    pdf_end_obj(p);

    pdf_write_outlines(p);

#ifdef DEBUG
    if (p->debug['s']) {
	printf("PDF document statistics:\n");
	printf("    %d pages\n", p->current_page);
	printf("    %d fonts\n", p->fonts_number);
	printf("    %d xobjects\n", p->xobjects_number);
	printf("    %d objects\n", p->currentobj + 1);
    }
#endif

    /* Don't write any object after this check! */
    for (i = 1; i <= p->currentobj; i++) {
	if (p->file_offset[i] == BAD_ID) {
	    pdf_error(p, PDF_NonfatalError, 
	    	"Object %ld allocated but not used", i);
	    pdf_begin_obj(p, i);
	    pdf_end_obj(p);
	}
    }

    pos = ftell(p->fp);				/* xref table */
    (void) fputs("xref\n", p->fp);
    (void) fprintf(p->fp,"0 %ld\n", p->currentobj + 1);
    (void) fputs("0000000000 65535 f \n", p->fp);
    for (i = 1; i <= p->currentobj; i++) {
	(void) fprintf(p->fp,"%010ld 00000 n \n", p->file_offset[i]);
    }

    (void) fputs("trailer\n", p->fp);

    pdf_begin_dict(p);				/* trailer */
    (void) fprintf(p->fp,"/Size %ld\n", p->currentobj + 1);
    (void) fprintf(p->fp,"/Info %ld 0 R\n", p->info_id);
    (void) fprintf(p->fp,"/Root %ld 0 R\n", p->root_id);
    pdf_end_dict(p);				/* trailer */

    (void) fputs("startxref\n", p->fp);
    (void) fprintf(p->fp,"%ld\n", pos);
    (void) fputs("%%EOF\n", p->fp);
}

PDFLIB_API void PDFLIB_CALL
PDF_close(PDF *p)
{
    pdf_wrapup_document(p);	/* dump the remaining PDF structures to file */
    pdf_close_file(p);		/* close the output file */
}

void
pdf_begin_contents_section(PDF *p)
{
    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_SystemError,
		"Tried to start contents section outside page description");

    if (p->contents != c_none)
	return;

    if (p->next_content >= p->contents_ids_capacity) {
	p->contents_ids_capacity *= 2;
	p->contents_ids = (id *) p->realloc(p, p->contents_ids,
			    sizeof(long) * p->contents_ids_capacity,
			    "pdf_begin_contents_section");
    }
    							/* Contents object */
    p->contents_ids[p->next_content] = pdf_begin_obj(p, NEW_ID);
    p->contents	= c_stream;
    pdf_begin_dict(p);
    p->contents_length_id = pdf_alloc_id(p);
    (void) fprintf(p->fp,"/Length %ld 0 R\n", p->contents_length_id);
    pdf_end_dict(p);

    pdf_begin_stream(p);
    p->start_contents_pos = ftell(p->fp);
    p->next_content++;
}

void
pdf_end_contents_section(PDF *p)
{
    long length;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_SystemError,
		"Tried to end contents section outside page description");

    if (p->contents == c_none)
	return;

    pdf_end_text(p);
    p->contents = c_none;

    length = ftell(p->fp) - p->start_contents_pos;
    pdf_end_stream(p);
    pdf_end_obj(p);

    pdf_begin_obj(p, p->contents_length_id);	/* Length object */
    (void) fprintf(p->fp,"%ld\n", length);
    pdf_end_obj(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_begin_page(PDF *p, float width, float height)
{
#define ACRO3_MINPAGE	(float) 72		/* 1 inch = 2.54 cm */
#define ACRO3_MAXPAGE	(float) 3240		/* 45 inch = 114,3 cm */
#define ACRO4_MINPAGE	(float) 18		/* 0.25 inch = 0.635 cm */
#define ACRO4_MAXPAGE	(float) 14400		/* 200  inch = 508 cm */

    if (p->state != pdf_state_open)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_begin_page)");

    if (height < ACRO4_MINPAGE || width < ACRO4_MINPAGE ||
	height > ACRO4_MAXPAGE || width > ACRO4_MAXPAGE)
	pdf_error(p, PDF_NonfatalError,
		"Page size incompatible with Acrobat 4");

    else if (height < ACRO3_MINPAGE || width < ACRO3_MINPAGE ||
	height > ACRO3_MAXPAGE || width > ACRO3_MAXPAGE)
	pdf_error(p, PDF_NonfatalError,
		"Page size incompatible with Acrobat 3");

    if (++(p->current_page) >= p->pages_capacity)
	pdf_grow_pages(p);

    /* no id has been preallocated */
    if (p->pages[p->current_page] == BAD_ID)
	p->pages[p->current_page] = pdf_alloc_id(p);

    p->height		= height;
    p->width		= width;
    p->thumb_id		= BAD_ID;
    p->state		= pdf_state_page_description;
    p->next_content	= 0;
    p->contents 	= c_none;
    p->procset		= 0;
    p->fill_rule	= pdf_fill_winding;

    pdf_init_page_annots(p);

    pdf_begin_contents_section(p);
}

/* This array must be kept in sync with the pdf_transition enum in p_intern.h */
static const char *pdf_transition_names[] = {
    "", "Split", "Blinds", "Box", "Wipe", "Dissolve", "Glitter", "R"
};

PDFLIB_API void PDFLIB_CALL
PDF_end_page(PDF *p)
{
    int		index = 0;
    char	buf[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    pdf_annot	*ann;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_end_page)!");

    pdf_end_contents_section(p);

    /* Page object */
    pdf_begin_obj(p, p->pages[p->current_page]);

    pdf_begin_dict(p);
    (void) fputs("/Type /Page\n", p->fp);
    (void) fprintf(p->fp,"/Parent %ld 0 R\n", p->pages_id);

    p->res_id = pdf_alloc_id(p);
    (void) fprintf(p->fp,"/Resources %ld 0 R\n", p->res_id);

    (void) fprintf(p->fp,"/MediaBox [0 0 %s %s]\n",
	    pdf_float(buf, p->width), pdf_float(buf2, p->height));

    if (p->duration > 0)
	(void) fprintf(p->fp, "/D %s\n", pdf_float(buf, p->duration));

    if (p->transition != trans_none) {
	(void) fputs("/Trans ", p->fp);
	pdf_begin_dict(p);
	    (void) fprintf(p->fp, "/S /%s",
	    			pdf_transition_names[p->transition]);
	pdf_end_dict(p);
    }

    (void) fputs("/Contents [", p->fp);
    for (index = 0; index < p->next_content; index++) {
	(void) fprintf(p->fp,"%ld 0 R", p->contents_ids[index]);
	(void) fputs(index+1 % 8 ? " " : "\n", p->fp);
    }
    (void) fputs("]\n", p->fp);

    /* Thumbnail image */
    if (p->thumb_id != BAD_ID)
	(void) fprintf(p->fp, "/Thumb %ld 0 R\n", p->thumb_id);

    /* Annotations array */
    if (p->annots) {
	(void) fputs("/Annots [ ", p->fp);

	for (ann = p->annots; ann != NULL; ann = ann->next) {
	    ann->obj_id = pdf_alloc_id(p);
	    (void) fprintf(p->fp,"%ld 0 R ", ann->obj_id);
	}

	(void) fputs("]\n", p->fp);
    }

    pdf_end_dict(p);		/* Page object */
    pdf_end_obj(p);

    pdf_write_page_annots(p);	/* Annotation dicts */

    /* Resource object */
    
    pdf_begin_obj(p, p->res_id);
    pdf_begin_dict(p);		/* Resource dict */

    /* ProcSet resources */

    (void) fputs("/ProcSet [/PDF", p->fp);
    if ( p->procset & ImageB)
	(void) fputs(" /ImageB", p->fp);
    if ( p->procset & ImageC)
	(void) fputs(" /ImageC", p->fp);
    if ( p->procset & ImageI)
	(void) fputs(" /ImageI", p->fp);
    if ( p->procset & Text)
	(void) fputs(" /Text", p->fp);
    (void) fputs("]\n", p->fp);

    /* Font resources */
    pdf_write_page_fonts(p);

    /* XObject resources */
    pdf_write_xobjects(p);

    pdf_end_dict(p);	/* resource dict */
    pdf_end_obj(p);	/* resource object */

    pdf_cleanup_page(p);

    p->state	= pdf_state_open;
}

id
pdf_begin_obj(PDF *p, id obj_id)
{
    if (obj_id == NEW_ID)
	obj_id = pdf_alloc_id(p);

    p->file_offset[obj_id] = ftell(p->fp); 
    (void) fprintf(p->fp, "%ld 0 obj\n", obj_id);
    return obj_id;
}

id
pdf_alloc_id(PDF *p)
{
    p->currentobj++;

    if (p->currentobj >= p->file_offset_capacity) {
	p->file_offset_capacity *= 2;
	p->file_offset = (long *) p->realloc(p, p->file_offset,
		    sizeof(long) * p->file_offset_capacity, "pdf_alloc_id");
    }

    /* only needed for verifying obj table in PDF_close() */
    p->file_offset[p->currentobj] = BAD_ID;

    return p->currentobj;
}

void
pdf_grow_pages(PDF *p)
{
    int i;

    p->pages_capacity *= 2;
    p->pages = (id *) p->realloc(p, p->pages,
		sizeof(id) * p->pages_capacity, "pdf_grow_pages");
    for (i = p->current_page; i < p->pages_capacity; i++)
	p->pages[i] = BAD_ID;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_parameter(PDF *p, const char *key, const char *value)
{
    const unsigned char *c;
    char *filename, *resource;

    if (key == NULL || !*key || value == NULL)
	pdf_error(p, PDF_ValueError, "Tried to set NULL parameter");

    /* file name of the resource file */
    if (!strcmp(key, "resourcefile")) {
	/* discard old resource file name, if any */
	if (p->resourcefilename)
	    p->free(p, p->resourcefilename);
	p->resourcefilename = pdf_strdup(p, value);
	return;
    /* add a single resource line */
    } else if (!strcmp(key, "FontAFM") ||
		!strcmp(key, "FontPFM") ||
		!strcmp(key, "FontOutline") ||
		!strcmp(key, "FontTT")) {
	    /* don't manipulate the caller's buffer */
	    resource = pdf_strdup(p, value);

	    if ((filename = strchr(resource, '=')) == NULL) {
		pdf_free(p, resource);
		pdf_error(p, PDF_ValueError, "Bogus resource line");
		return;
	    }
	    *filename++ = '\0';
	    pdf_add_resource(p, key, resource, filename, NULL);
	    pdf_free(p, resource);
	    return;
    /* activate debug flags */
    } else if (!strcmp(key, "debug")) {
	for (c = (const unsigned char *) value; *c; c++)
	    p->debug[(int) *c] = 1;
	if (*value == 'c')
	    p->compress = 0;
	return;
    /* deactivate debug flags */
    } else if (!strcmp(key, "nodebug")) {
	for (c = (const unsigned char *) value; *c; c++)
	    p->debug[(int) *c] = 0;
#ifdef HAVE_LIBZ
	if (*value == 'c')
	    p->compress = 1;
#endif
	return;
    } else {
	pdf_error(p, PDF_NonfatalError,
		"Tried to set unknown parameter '%s'", key);
    }
}
