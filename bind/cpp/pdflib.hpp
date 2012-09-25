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

// pdflib.hpp
//
// C++ wrapper for PDFlib
//
//

#ifndef PDFLIB_HPP
#define PDFLIB_HPP

// Add your compiler here if it doesn't support ANSI C++ string handling
#ifdef __WATCOMC__
    #define BROKEN_STRINGS
    #define BROKEN_BOOL
#endif

#ifdef BROKEN_BOOL
    #define bool	int
#endif

// If ANSI C++ strings don't work we revert to plain old C char *
#ifndef BROKEN_STRINGS
    #include <string>

    // VC++ needs this to recognize the string class, but Watcom 10.6
    // and gcc don't understand it...
//    #ifdef _MSC_VER
	using namespace std;
//    #endif

    #define pdfstring	string
#else
    #define pdfstring	const char *
#endif

// We use PDF as a C++ class name, therefore hide the actual C struct
// name for PDFlib usage with C++.

#define PDF PDF_c
#include <pdflib.h>
#undef PDF


// The C++ class wrapper for PDFlib

class PDF {
public:
    PDF(errorproc errorhandler = NULL,
	void* (*allocproc) (PDF_c *p, size_t size, const char *caller) = NULL,
	void* (*reallocproc)
		(PDF_c *p, void *mem, size_t size, const char *caller) = NULL,
	void  (*freeproc)(PDF_c *p, void *mem) = NULL,
	void *opaque = NULL);
    ~PDF();

    void * get_opaque();
    static int get_majorversion();
    static int get_minorversion();

    // Overloaded generic open and close methods
    int  open(pdfstring filename);
    int  open(FILE *fp);
    void close();

    void begin_page(float width, float height);
    void end_page();
    void set_transition(pdfstring type);
    void set_duration(float t);
    void set_parameter(pdfstring key, pdfstring value);
    void show(pdfstring text);
    void show_xy(pdfstring text, float x, float y);
    void continue_text(pdfstring text);
    void set_leading(float l);
    void set_text_rise(float rise);
    void set_horiz_scaling(float scale);
    void set_text_rendering(int mode);
    void set_text_matrix(float a, float b, float c, float d, float e, float f);
    void set_text_pos(float x, float y);
    void set_char_spacing(float spacing);
    void set_word_spacing(float spacing);

    pdfstring get_fontname();
    float get_fontsize();
    int  get_font();
    float stringwidth(pdfstring text, int font, float size);

    int  findfont(pdfstring fontname, pdfstring encoding, int embed);
    void setfont(int font, float fontsize);
    void save();
    void restore();
    void translate(float tx, float ty);
    void scale(float sx, float sy);
    void rotate(float phi);
    void setdash(float d1, float d2);
    void setpolydash(float *darray, int length);
    void setflat(float flat);
    void setlinejoin(int join);
    void set_fillrule(const char* fillrule);
    void setlinecap(int cap);
    void setmiterlimit(float miter);
    void setlinewidth(float width);
    void moveto(float x, float y);
    void lineto(float x, float y);
    void curveto(float x1, float y1, float x2, float y2, float x3, float y3);
    void circle(float x, float y, float r);
    void arc(float x, float y, float r, float alpha1, float alpha2);
    void rect(float x, float y, float width, float height);
    void closepath();
    void stroke();
    void closepath_stroke();
    void fill();
    void fill_stroke();
    void closepath_fill_stroke();
    void endpath();
    void clip();
    void setgray_fill(float g);
    void setgray_stroke(float g);
    void setgray(float g);
    void setrgbcolor_fill(float red, float green, float blue);
    void setrgbcolor_stroke(float red, float green, float blue);
    void setrgbcolor(float red, float green, float blue);
    int get_image_width(int image);
    int get_image_height(int image);
    void place_image(int image, float x, float y, float scale);
    int open_memory_image(pdfstring *buffer, int width, int height,
    	int components, int bpc);
    int open_image(pdfstring type, pdfstring source, pdfstring data, long len,
    	int width, int height, int components, int bpc, pdfstring params);
    int open_JPEG(pdfstring filename);
    int open_TIFF(pdfstring filename);
    int open_GIF(pdfstring filename);
    int open_CCITT(pdfstring filename, int width, int height,
    	bool BitReverse, int K, bool BlackIs1);
    int add_bookmark(pdfstring text, int parent, bool open);
    void set_info(pdfstring key, pdfstring value);
    void attach_file(float llx, float lly, float urx, float ury,
    	pdfstring filename, pdfstring description, pdfstring author,
	pdfstring mimetype, pdfstring icon);
    void add_note(float llx, float lly, float urx, float ury,
    	pdfstring contents, pdfstring title, pdfstring icon, bool open);
    void add_pdflink(float llx, float lly, float urx, float ury,
    	pdfstring filename, int page, pdfstring dest);
    void add_launchlink(float llx, float lly, float urx, float ury,
    	pdfstring filename);
    void add_locallink(float llx, float lly, float urx, float ury,
    	int page, pdfstring dest);
    void add_weblink(float llx, float lly, float urx, float ury, pdfstring url);

    void set_border_style(pdfstring style, float width);
    void set_border_color(float red, float green, float blue);
    void set_border_dash(float d1, float d2);

private:
    PDF_c *p;
    enum pdf_openmethod { pdf_file, pdf_fp };
    pdf_openmethod openmethod;
};

#endif	// PDFLIB_HPP
