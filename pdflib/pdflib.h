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

/* pdflib.h
 *
 * PDFlib public function declarations
 *
 */

/* This file is nearly unreadable because it serves many purposes
 * at the same time:
 *
 * - PDFlib C API header file
 * - Input for the C++ wrapper
 * - DLL import/export control
 * - SWIG declaration file
 * - Input for the SWIG documentation system
 */

#ifndef PDFLIB_H
#define PDFLIB_H

/* 
 * ----------------------------------------------------------------------
 * Setup stuff
 * ----------------------------------------------------------------------
 */

#ifndef SWIG
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef WIN32

/* Visual Basic needs DLLs with standard calling conventions */
#ifdef VISUALBASIC_EXPORTS
	#define PDFLIB_CALL	__stdcall
	#define PDFLIB_API	__declspec(dllexport)

#else /* !VISUALBASIC_EXPORTS */

#define PDFLIB_CALL	__cdecl

#ifdef PDFLIB_EXPORTS		/* prepare a DLL */
    #define PDFLIB_API	__declspec(dllexport)
#elif defined(PDFLIB_STATIC)
    #define PDFLIB_API
#else	/* !PDFLIB_STATIC */
    #define PDFLIB_API   __declspec(dllimport)
#endif	/* !PDFLIB_STATIC */
#endif	/* !VISUALBASIC_EXPORTS */

#else	/* !WIN32 */

#define PDFLIB_CALL
#define PDFLIB_API

#endif	/* !WIN32 */

/* Make our declarations C++ compatible */
#ifdef __cplusplus
extern "C" {
#endif

/* Define the basic PDF type. This is used opaquely at the API level. */
typedef struct PDF_s PDF;

/* 
 * ----------------------------------------------------------------------
 * p_basic.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%section "General Functions"
#endif

#ifndef SWIG
/* The version defines below may be used to check the version of the
 * include file against the library. This is not reasonable at the
 * scripting API level since both version constants and version functions
 * are supplied from the library in this case.
 */

/* PDFlib version number, major part */
#define PDFLIB_MAJORVERSION	2

/* PDFlib version number, minor part (must use two decimal places if != 0) */
#define PDFLIB_MINORVERSION	01
#define PDFLIB_VERSIONSTRING	"2.01"

/* VB, Tcl and Perl have intrinsic versioning which we make use of.
 * Java uses a version-specific shared library name.
 */
#if !defined(SWIGTCL) && !defined(SWIGPERL) && !defined(SWIGJAVA) && !defined(VISUALBASIC_EXPORTS)
/* Returns the PDFlib major version number. */
PDFLIB_API int PDFLIB_CALL PDF_get_majorversion(void);

/* Returns the PDFlib minor version number. */
PDFLIB_API int PDFLIB_CALL PDF_get_minorversion(void);
#endif
#endif

/* Boot PDFlib. Recommended although currently not required.
 Booting is done automatically for Java, Tcl, Perl, and Python. */
PDFLIB_API void PDFLIB_CALL PDF_boot(void);

/* Shut down PDFlib. Recommended although currently not required. */
PDFLIB_API void PDFLIB_CALL PDF_shutdown(void);

#ifndef SWIG

/* This typedef is required to pacify the Watcom compiler in C++ mode. */
typedef void (*errorproc)(PDF *p, int type, const char *msg);

/* Create a new PDF object.  Returns a pointer to the opaque PDF datatype 
which you will need as the "p" argument for all other functions. All 
function pointers may be NULL if default handlers are to be used. */
PDFLIB_API PDF * PDFLIB_CALL PDF_new2(errorproc errorhandler,
    void* (*allocproc)(PDF *p, size_t size, const char *caller),
    void* (*reallocproc)(PDF *p, void *mem, size_t size, const char *caller),
    void  (*freeproc)(PDF *p, void *mem),
    void   *opaque);

/* Fetch opaque application pointer stored in PDFlib (useful for multi-threading */
PDFLIB_API void * PDFLIB_CALL PDF_get_opaque(PDF *p);

#endif	/* SWIG */

/* Create a new PDF object. */
PDFLIB_API PDF * PDFLIB_CALL PDF_new(void);

/* Delete the PDF object. */
PDFLIB_API void PDFLIB_CALL PDF_delete(PDF *p);

/* Open a new PDF file associated with p, using the supplied file name.
 Returns -1 on error. */
PDFLIB_API int PDFLIB_CALL PDF_open_file(PDF *p, const char *filename);

/* Close the generated PDF file. */
PDFLIB_API void PDFLIB_CALL PDF_close(PDF *p);

#if !defined(SWIG) && !defined(VISUALBASIC_EXPORTS)
/* Open a new PDF file associated with p, using the supplied file handle.
 Returns -1 on error. */
PDFLIB_API int PDFLIB_CALL PDF_open_fp(PDF *p, FILE *fp);

#endif	/* !defined(SWIG) && !defined(VISUALBASIC_EXPORTS) */

/* Start a new page. */
PDFLIB_API void PDFLIB_CALL PDF_begin_page(PDF *p, float width, float height);

/* Finish the page. */
PDFLIB_API void PDFLIB_CALL PDF_end_page(PDF *p);

#ifndef SWIG
/* PDFlib exceptions which may be handled by a user-supplied error handler */
#define PDF_MemoryError    1
#define PDF_IOError        2
#define PDF_RuntimeError   3
#define PDF_IndexError     4
#define PDF_TypeError      5
#define PDF_DivisionByZero 6
#define PDF_OverflowError  7
#define PDF_SyntaxError    8
#define PDF_ValueError     9
#define PDF_SystemError   10
#define PDF_NonfatalError 11
#define PDF_UnknownError  12

#endif	/* !SWIG */

/* Set some PDFlib-internal parameters controlling PDF generation.
 Currently supported parameters are "resourcefile", "debug", "nodebug",
 and all UPR category names. */
PDFLIB_API void PDFLIB_CALL PDF_set_parameter(PDF *p, const char *key, const char *value);

/* 
 * ----------------------------------------------------------------------
 * p_font.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%section "Text Functions"
%subsection "Font Handling Functions"
#endif

/* Prepare a font for later use with PDF_setfont(). The metrics will be
 loaded, and if embed is nonzero, the font file will be checked (but not
 yet used. Encoding is one of "builtin", "pdfdoc", "macroman", "macexpert",
 "winansi", or "default". Note that in order to use arbitrary encodings you
 will need AFM files. */
PDFLIB_API int PDFLIB_CALL PDF_findfont(PDF *p, const char *fontname,
	const char *encoding, int embed);

/* Set the current font in the give size. The font descriptor must have
 been retrieved via PDF_findfont(). */
PDFLIB_API void PDFLIB_CALL PDF_setfont(PDF *p, int font, float fontsize);

/* Obsolete function, use PDF_findfont() and PDF_setfont() instead. */
PDFLIB_API void PDFLIB_CALL
PDF_set_font(PDF *p, const char *fontname, float fontsize,const char *encoding);

/* Return the name of the current font. */
PDFLIB_API const char * PDFLIB_CALL PDF_get_fontname(PDF *p);

/* Return the size of the current font. */
PDFLIB_API float PDFLIB_CALL PDF_get_fontsize(PDF *p);

/* Return the identifier of the current font. */
PDFLIB_API int PDFLIB_CALL PDF_get_font(PDF *p);

/* 
 * ----------------------------------------------------------------------
 * p_text.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%subsection "Text Output Functions"
#endif
/* Print text in the current font and size at the current position. */
PDFLIB_API void PDFLIB_CALL PDF_show(PDF *p, const char *text);

/* Print text in the current font at (x, y). */
PDFLIB_API void PDFLIB_CALL PDF_show_xy(PDF *p, const char *text, float x, float y);

/* Print text at the next line. */
PDFLIB_API void PDFLIB_CALL PDF_continue_text(PDF *p, const char *text);

/* Set the distance between text baselines. */
PDFLIB_API void PDFLIB_CALL PDF_set_leading(PDF *p, float leading);

/* Set the text rise parameter. */
PDFLIB_API void PDFLIB_CALL PDF_set_text_rise(PDF *p, float rise);

/* Set horizontal text scaling in percent. */
PDFLIB_API void PDFLIB_CALL PDF_set_horiz_scaling(PDF *p, float scale);

/* Set text rendering mode. */
PDFLIB_API void PDFLIB_CALL PDF_set_text_rendering(PDF *p, int mode);

/* Set a transformation matrix to be applied to the current font. */
PDFLIB_API void PDFLIB_CALL PDF_set_text_matrix(PDF *p, float a, float b, float c, float d, float e, float f);

/* Set the text output position to (x, y). */
PDFLIB_API void PDFLIB_CALL PDF_set_text_pos(PDF *p, float x, float y);

/* Set the horizontal character spacing in text space units. */
PDFLIB_API void PDFLIB_CALL PDF_set_char_spacing(PDF *p, float spacing);

/* Set the horizontal word spacing in text space units. */
PDFLIB_API void PDFLIB_CALL PDF_set_word_spacing(PDF *p, float spacing);

/* Return the width of text in an arbitrary font which has been selected
 with PDF_findfont(). */
PDFLIB_API float PDFLIB_CALL PDF_stringwidth(PDF *p, const char *text, int font, float size);

/* 
 * ----------------------------------------------------------------------
 * p_gstate.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%section "Graphics Functions"
%subsection "General Graphics State Functions"
#endif

/* Set the current dash pattern to d1 white and d2 black units. */
PDFLIB_API void PDFLIB_CALL PDF_setdash(PDF *p, float b, float w);

/* Set a more complicated dash pattern defined by an array. */
PDFLIB_API void PDFLIB_CALL PDF_setpolydash(PDF *p, float *darray, int length);

/* Set the flatness to a value between 0 and 100 inclusive. */
PDFLIB_API void PDFLIB_CALL PDF_setflat(PDF *p, float flatness);

/* Set the line join parameter to a value between 0 and 2 inclusive. */
PDFLIB_API void PDFLIB_CALL PDF_setlinejoin(PDF *p, int linejoin);

/* Set the linecap parameter to a value between 0 and 2 inclusive. */
PDFLIB_API void PDFLIB_CALL PDF_setlinecap(PDF *p, int linecap);

/* Set the miter limit to a value greater than or equal to 1. */
PDFLIB_API void PDFLIB_CALL PDF_setmiterlimit(PDF *p, float miter);

/* Set the current linewidth to width. */
PDFLIB_API void PDFLIB_CALL PDF_setlinewidth(PDF *p, float width);

/* Set the current fill rule to "winding" or "evenodd". */
PDFLIB_API void PDFLIB_CALL PDF_set_fillrule(PDF *p, const char* fillrule);

#ifdef SWIG
%subsection "Special Graphics State Functions"
#endif

/* Save the current graphics state. */
PDFLIB_API void PDFLIB_CALL PDF_save(PDF *p);

/* Restore the most recently saved graphics state. */
PDFLIB_API void PDFLIB_CALL PDF_restore(PDF *p);

/* Translate the origin of the coordinate system to (tx, ty). */
PDFLIB_API void PDFLIB_CALL PDF_translate(PDF *p, float tx, float ty);

/* Scale the coordinate system by (sx, sy). */
PDFLIB_API void PDFLIB_CALL PDF_scale(PDF *p, float sx, float sy);

/* Rotate the coordinate system by phi degrees. */
PDFLIB_API void PDFLIB_CALL PDF_rotate(PDF *p, float phi);

/* 
 * ----------------------------------------------------------------------
 * p_draw.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%subsection "Path Segment Functions"
#endif

/* Set the current point to (x, y). */
PDFLIB_API void PDFLIB_CALL PDF_moveto(PDF *p, float x, float y);

/* Draw a line from the current point to (x, y). */
PDFLIB_API void PDFLIB_CALL PDF_lineto(PDF *p, float x, float y);

/* Draw a Bezier curve from the current point, using 3 more control points. */
PDFLIB_API void PDFLIB_CALL PDF_curveto(PDF *p, float x1, float y1, float x2, float y2, float x3, float y3);

/* Draw a circle with center (x, y) and radius r. */
PDFLIB_API void PDFLIB_CALL PDF_circle(PDF *p, float x, float y, float r);

/* Draw a circular arc with center (x, y), radius r from alpha1 to alpha2. */
PDFLIB_API void PDFLIB_CALL PDF_arc(PDF *p, float x, float y, float r, float alpha1, float alpha2);

/* Draw a rectangle at lower left (x, y) with width and height. */
PDFLIB_API void PDFLIB_CALL PDF_rect(PDF *p, float x, float y, float width, float height);

/* Close the current path. */
PDFLIB_API void PDFLIB_CALL PDF_closepath(PDF *p);

#ifdef SWIG
%subsection "Path Painting and Clipping Functions"
#endif

/* Stroke the path with the current color and line width,and clear it. */
PDFLIB_API void PDFLIB_CALL PDF_stroke(PDF *p);

/* Close the path, and stroke it. */
PDFLIB_API void PDFLIB_CALL PDF_closepath_stroke(PDF *p);

/* Fill the interior of the path with the current fill color. */
PDFLIB_API void PDFLIB_CALL PDF_fill(PDF *p);

/* Fill and stroke the path with the current fill and stroke color. */
PDFLIB_API void PDFLIB_CALL PDF_fill_stroke(PDF *p);

/* Close the path, fill, and stroke it. */
PDFLIB_API void PDFLIB_CALL PDF_closepath_fill_stroke(PDF *p);

/* End the current path. */
PDFLIB_API void PDFLIB_CALL PDF_endpath(PDF *p);

/* Use the current path as clipping path. */
PDFLIB_API void PDFLIB_CALL PDF_clip(PDF *p);

/* 
 * ----------------------------------------------------------------------
 * p_color.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%section "Color Functions"
#endif

/* Set the current fill color to a gray value between 0 and 1 inclusive. */
PDFLIB_API void PDFLIB_CALL PDF_setgray_fill(PDF *p, float g);

/* Set the current stroke color to a gray value between 0 and 1 inclusive. */
PDFLIB_API void PDFLIB_CALL PDF_setgray_stroke(PDF *p, float g);

/* Set the current fill and stroke color. */
PDFLIB_API void PDFLIB_CALL PDF_setgray(PDF *p, float g);

/* Set the current fill color to the supplied RGB values. */
PDFLIB_API void PDFLIB_CALL PDF_setrgbcolor_fill(PDF *p, float red, float green, float blue);

/* Set the current stroke color to the supplied RGB values. */
PDFLIB_API void PDFLIB_CALL PDF_setrgbcolor_stroke(PDF *p, float red, float green, float blue);

/* Set the current fill and stroke color to the supplied RGB values. */
PDFLIB_API void PDFLIB_CALL PDF_setrgbcolor(PDF *p, float red, float green, float blue);

/* 
 * ----------------------------------------------------------------------
 * p_image.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%section "Image Functions"
#endif

/* Return the width of an image. */
PDFLIB_API int PDFLIB_CALL PDF_get_image_width(PDF *p, int image);

/* Return the height of an image. */
PDFLIB_API int PDFLIB_CALL PDF_get_image_height(PDF *p, int image);

/* Place an image at the lower left corner (x, y), and scale it. */
PDFLIB_API void PDFLIB_CALL PDF_place_image(PDF *p, int image, float x, float y, float scale);

/* Use image data from a variety of data sources. Returns an image descriptor
 or -1. Supported types are "jpeg", "ccitt", "raw". Supported sources are
 "memory", "fileref", "url". len is only used for type="raw", params is only
 used for type="ccitt". */
PDFLIB_API int PDFLIB_CALL PDF_open_image(PDF *p, const char *type, const char *source, const char *data, long length, int width, int height, int components, int bpc, const char *params);

/* Close an image retrieved with one of the PDF_open_*() image functions.
 Usually not required, the image file will be closed anyway when the open
 function is done. */
PDFLIB_API void PDFLIB_CALL PDF_close_image(PDF *p, int image);

#if !defined(SWIG) && !defined(VISUALBASIC_EXPORTS)
/* Add an existing image as thumbnail for the current page. PDFlib doesn't
help with preparing the thumbnail, but simply places it in the output. */
PDFLIB_API void PDFLIB_CALL PDF_add_thumbnail(PDF *p, int im);
#endif

/* 
 * ----------------------------------------------------------------------
 * p_jpeg.c
 * ----------------------------------------------------------------------
 */

/* Open a JPEG image for later use. Returns an image descriptor or -1. */
PDFLIB_API int PDFLIB_CALL PDF_open_JPEG(PDF *p, const char *filename);

/* 
 * ----------------------------------------------------------------------
 * p_tiff.c
 * ----------------------------------------------------------------------
 */

/* Open a TIFF image for later use. Returns an image descriptor or -1. */
PDFLIB_API int PDFLIB_CALL PDF_open_TIFF(PDF *p, const char *filename);

/* 
 * ----------------------------------------------------------------------
 * p_gif.c
 * ----------------------------------------------------------------------
 */

/* Open a GIF image for later use. Returns an image descriptor or -1. */
PDFLIB_API int PDFLIB_CALL PDF_open_GIF(PDF *p, const char *filename);

/* 
 * ----------------------------------------------------------------------
 * p_ccitt.c
 * ----------------------------------------------------------------------
 */

/* Open a raw CCITT image for later use. Returns an image descriptor or -1. */
PDFLIB_API int PDFLIB_CALL PDF_open_CCITT(PDF *p, const char *filename, int width, int height, int BitReverse, int K, int BlackIs1);

/* 
 * ----------------------------------------------------------------------
 * p_filter.c
 * ----------------------------------------------------------------------
 */

/* no public functions */

/* 
 * ----------------------------------------------------------------------
 * p_hyper.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%section "Hypertext Functions"
%subsection "Bookmarks"
#endif

/* Add a nested bookmark under parent, or a new top-level bookmark if 
 parent = 0. text may be Unicode. Returns a bookmark descriptor which may be
 used as parent for subsequent nested bookmarks. If open = 1, child
 bookmarks will be folded out, and invisible if open = 0. */
PDFLIB_API int PDFLIB_CALL PDF_add_bookmark(PDF *p, const char *text, int parent, int open);

#ifdef SWIG
%subsection "Document Information Fields"
#endif

/* Fill document information field key with value. value may be Unicode. */
PDFLIB_API void PDFLIB_CALL PDF_set_info(PDF *p, const char *key, const char *value);

#ifdef SWIG
%subsection "Page Transitions"
#endif

/* Set a transition effect for the current page. The type is one of
 "none", "split", "blinds", "box", "wipe", "dissolve", "glitter", or 
 "replace". */
PDFLIB_API void PDFLIB_CALL PDF_set_transition(PDF *p, const char *type);

/* Set the duration for a page transition effect. */
PDFLIB_API void PDFLIB_CALL PDF_set_duration(PDF *p, float t);

/* 
 * ----------------------------------------------------------------------
 * p_annots.c
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%subsection "File Attachments"
#endif

/* Add a file attachment annotation. description and author may be Unicode. 
 icon is one of "graph, "paperclip", "pushpin", or "tag" */
PDFLIB_API void PDFLIB_CALL PDF_attach_file(PDF *p, float llx, float lly, float urx, float ury, const char *filename, const char *description, const char *author, const char *mimetype, const char *icon);

#ifdef SWIG
%subsection "Note Annotations"
#endif

/* Add a note annotation. contents and title may be Unicode. icon is one
 of "comment, "insert", "note", "paragraph", "newparagraph", "key", or "help" */
PDFLIB_API void PDFLIB_CALL PDF_add_note(PDF *p, float llx, float lly, float urx, float ury, const char *contents, const char *title, const char *icon, int open);

#ifdef SWIG
%subsection "Links"
#endif
/* Add a file link annotation (to a PDF file). */
PDFLIB_API void PDFLIB_CALL PDF_add_pdflink(PDF *p, float llx, float lly, float urx, float ury, const char *filename, int page, const char *dest);

/* Add a launch annotation (arbitrary file type). */
PDFLIB_API void PDFLIB_CALL PDF_add_launchlink(PDF *p, float llx, float lly, float urx, float ury, const char *filename);

/* Add a link annotation with a target within the current file. dest can be
 "fullpage" or "fitwidth". */
PDFLIB_API void PDFLIB_CALL PDF_add_locallink(PDF *p, float llx, float lly, float urx, float ury, int page, const char *dest);

/* Add a weblink annotation. */
PDFLIB_API void PDFLIB_CALL PDF_add_weblink(PDF *p, float llx, float lly, float urx, float ury, const char *url);

/* Set the border style for all kinds of annotations.
 These settings are used for all annotations until a new style is set. 
 Supported border style names are "solid" and "dashed". */
PDFLIB_API void PDFLIB_CALL PDF_set_border_style(PDF *p, const char *style, float width);

/* Set the border color for all kinds of annotations. */
PDFLIB_API void PDFLIB_CALL PDF_set_border_color(PDF *p, float red, float green, float blue);

/* Set the border dash style for all kinds of annotations. See PDF_setdash(). */
PDFLIB_API void PDFLIB_CALL PDF_set_border_dash(PDF *p, float w, float b);

/* 
 * ----------------------------------------------------------------------
 * Convenience stuff
 * ----------------------------------------------------------------------
 */

#ifdef SWIG
%section "Convenience Stuff"

%text %{
 Page size constants are not supported in the PDFlib scripting language
 bindings. Please refer to the constants supplied in pdflib.h instead.
%}
#endif

/*
Although PDF doesn´t impose any restrictions on the usable page size,
Acrobat implementations suffer from architectural limits concerning
the page size.
Although PDFlib will generate PDF documents with page sizes outside
these limits, the default error handler will issue a warning message.

Acrobat 3 minimum page size: 1" = 72 pt = 2.54 cm
Acrobat 3 maximum page size: 45" = 3240 pt = 114.3 cm
Acrobat 4 minimum page size: 0.25" = 18 pt = 0.635 cm
Acrobat 4 maximum page size: 200" = 14400 pt = 508 cm
*/

/* The page sizes are only available to the C and C++ bindings */
#ifndef SWIG
#define a0_width	 (float) 2380.0
#define a0_height	 (float) 3368.0
#define a1_width	 (float) 1684.0
#define a1_height	 (float) 2380.0
#define a2_width	 (float) 1190.0
#define a2_height	 (float) 1684.0
#define a3_width	 (float) 842.0
#define a3_height	 (float) 1190.0
#define a4_width	 (float) 595.0
#define a4_height	 (float) 842.0
#define a5_width	 (float) 421.0
#define a5_height	 (float) 595.0
#define a6_width	 (float) 297.0
#define a6_height	 (float) 421.0
#define b5_width	 (float) 501.0
#define b5_height	 (float) 709.0
#define letter_width	 (float) 612.0
#define letter_height	 (float) 792.0
#define legal_width 	 (float) 612.0
#define legal_height 	 (float) 1008.0
#define ledger_width	 (float) 1224.0
#define ledger_height	 (float) 792.0
#define p11x17_width	 (float) 792.0
#define p11x17_height	 (float) 1224.0
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif	/* PDFLIB_H */
