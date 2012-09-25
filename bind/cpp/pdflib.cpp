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

// pdflib.cpp
//
// Implementation of C++ wrapper for PDFlib
//
//

#include "pdflib.hpp"

// If we don't have ANSI C++ strings we do it the old way.
// The Watcom also uses a cast operator instead of ANSI C++'s c_str() method
#ifdef BROKEN_STRINGS
    #define CHAR(s)	(char const *)(s)
#else
    #define CHAR(s)	s.c_str()
#endif

PDF::PDF(errorproc errorhandler,
    void* (*allocproc)(PDF_c *p, size_t size, const char *caller),
    void* (*reallocproc)(PDF_c *p, void *mem, size_t size, const char *caller),
    void  (*freeproc)(PDF_c *p, void *mem),
    void *opaque)
{
	PDF_boot();
	p = ::PDF_new2(errorhandler, allocproc, reallocproc, freeproc, opaque);
}

PDF::~PDF()
{
    ::PDF_delete(p);
}

void *
PDF::get_opaque()
{
    return ::PDF_get_opaque(p);
}

int
PDF::get_majorversion()
{
    return ::PDF_get_majorversion();
}

int
PDF::get_minorversion()
{
    return ::PDF_get_minorversion();
}

int
PDF::open(pdfstring filename)
{
    return ::PDF_open_file(p, CHAR(filename));
}

int
PDF::open(FILE *fp)
{
    return ::PDF_open_fp(p, fp);
}

void
PDF::close()
{
    ::PDF_close(p);
}

void
PDF::begin_page(float width, float height)
{
    ::PDF_begin_page(p, width, height);
}

void
PDF::end_page()
{
    ::PDF_end_page(p);
}

void
PDF::set_transition(pdfstring type)
{
    ::PDF_set_transition(p, CHAR(type));
}

void
PDF::set_duration(float t)
{
    ::PDF_set_duration(p, t);
}

void
PDF::set_parameter(pdfstring key, pdfstring value)
{
    ::PDF_set_parameter(p, CHAR(key), CHAR(value));
}

void
PDF::show(pdfstring text)
{
    ::PDF_show(p, CHAR(text));
}

void
PDF::show_xy(pdfstring text, float x, float y)
{
    ::PDF_show_xy(p, CHAR(text), x, y);
}

void
PDF::continue_text(pdfstring text)
{
    ::PDF_continue_text(p, CHAR(text));
}

void
PDF::set_leading(float l)
{
    ::PDF_set_leading(p, l);
}

void
PDF::set_text_rise(float rise)
{
    ::PDF_set_text_rise(p, rise);
}

void
PDF::set_horiz_scaling(float scale)
{
    ::PDF_set_horiz_scaling(p, scale);
}

void
PDF::set_text_rendering(int mode)
{
    ::PDF_set_text_rendering(p, mode);
}

void
PDF::set_text_matrix(float a, float b, float c, float d, float e, float f)
{
    ::PDF_set_text_matrix(p, a, b, c, d, e, f);
}

void
PDF::set_text_pos(float x, float y)
{
    ::PDF_set_text_pos(p, x, y);
}

void
PDF::set_char_spacing(float spacing)
{
    ::PDF_set_char_spacing(p, spacing);
}

void
PDF::set_word_spacing(float spacing)
{
    ::PDF_set_word_spacing(p, spacing);
}

pdfstring
PDF::get_fontname()
{
    return ::PDF_get_fontname(p);
}

float
PDF::get_fontsize()
{
    return ::PDF_get_fontsize(p);
}

int
PDF::get_font()
{
    return ::PDF_get_font(p);
}

float
PDF::stringwidth(pdfstring text, int font, float size)
{
    return ::PDF_stringwidth(p, CHAR(text), font, size);
}

int
PDF::findfont(pdfstring fontname, pdfstring encoding, int embed)
{
    return ::PDF_findfont(p, CHAR(fontname), CHAR(encoding), embed);
}

void
PDF::setfont(int font, float fontsize)
{
    ::PDF_setfont(p, font, fontsize);
}

void
PDF::save()
{
    ::PDF_save(p);
}

void
PDF::restore()
{
    ::PDF_restore(p);
}

void
PDF::translate(float tx, float ty)
{
    ::PDF_translate(p, tx, ty);
}

void
PDF::scale(float sx, float sy)
{
    ::PDF_scale(p, sx, sy);
}

void
PDF::rotate(float phi)
{
    ::PDF_rotate(p, phi);
}

void
PDF::setdash(float d1, float d2)
{
    ::PDF_setdash(p, d1, d2);
}

void
PDF::setpolydash(float *darray, int length)
{
    ::PDF_setpolydash(p, darray, length);
}

void
PDF::setflat(float flat)
{
    ::PDF_setflat(p, flat);
}

void
PDF::set_fillrule(const char *fillrule)
{
    ::PDF_set_fillrule(p, fillrule);
}

void
PDF::setlinejoin(int join)
{
    ::PDF_setlinejoin(p, join);
}

void
PDF::setlinecap(int cap)
{
    ::PDF_setlinecap(p, cap);
}

void
PDF::setmiterlimit(float miter)
{
    ::PDF_setmiterlimit(p, miter);
}

void
PDF::setlinewidth(float width)
{
    ::PDF_setlinewidth(p, width);
}

void
PDF::moveto(float x, float y)
{
    ::PDF_moveto(p, x, y);
}

void
PDF::lineto(float x, float y)
{
    ::PDF_lineto(p, x, y);
}

void
PDF::curveto(float x1, float y1, float x2, float y2, float x3, float y3)
{
    ::PDF_curveto(p, x1, y1, x2, y2, x3, y3);
}

void
PDF::circle(float x, float y, float r)
{
    ::PDF_circle(p, x, y, r);
}

void
PDF::arc(float x, float y, float r, float alpha1, float alpha2)
{
    ::PDF_arc(p, x, y, r, alpha1, alpha2);
}

void
PDF::rect(float x, float y, float width, float height)
{
    ::PDF_rect(p, x, y, width, height);
}

void
PDF::closepath()
{
    ::PDF_closepath(p);
}

void
PDF::stroke()
{
    ::PDF_stroke(p);
}

void
PDF::closepath_stroke()
{
    ::PDF_closepath_stroke(p);
}

void
PDF::fill()
{
    ::PDF_fill(p);
}

void
PDF::fill_stroke()
{
    ::PDF_fill_stroke(p);
}

void
PDF::closepath_fill_stroke()
{
    ::PDF_closepath_fill_stroke(p);
}

void
PDF::endpath()
{
    ::PDF_endpath(p);
}

void
PDF::clip()
{
    ::PDF_clip(p);
}

void
PDF::setgray_fill(float g)
{
    ::PDF_setgray_fill(p, g);
}

void
PDF::setgray_stroke(float g)
{
    ::PDF_setgray_stroke(p, g);
}

void
PDF::setgray(float g)
{
    ::PDF_setgray(p, g);
}

void
PDF::setrgbcolor_fill(float red, float green, float blue)
{
    ::PDF_setrgbcolor_fill(p, red, green, blue);
}

void
PDF::setrgbcolor_stroke(float red, float green, float blue)
{
    ::PDF_setrgbcolor_stroke(p, red, green, blue);
}

void
PDF::setrgbcolor(float red, float green, float blue)
{
    ::PDF_setrgbcolor(p, red, green, blue);
}

int
PDF::get_image_width(int image)
{
    return ::PDF_get_image_width(p, image);
}

int
PDF::get_image_height(int image)
{
    return ::PDF_get_image_height(p, image);
}

void
PDF::place_image(int image, float x, float y, float scale)
{
    ::PDF_place_image(p, image, x, y, scale);
}

int
PDF::open_image(pdfstring type, pdfstring source, pdfstring data,
    long len, int width, int height, int components, int bpc, pdfstring params)
{
    return ::PDF_open_image(p, CHAR(type), CHAR(source), CHAR(data), len,
    	width, height, components, bpc, CHAR(params));
}

int
PDF::open_JPEG(pdfstring filename)
{
    return ::PDF_open_JPEG(p, CHAR(filename));
}

int
PDF::open_TIFF(pdfstring filename)
{
    return ::PDF_open_TIFF(p, CHAR(filename));
}

int
PDF::open_GIF(pdfstring filename)
{
    return ::PDF_open_GIF(p, CHAR(filename));
}

int
PDF::open_CCITT(pdfstring filename, int width, int height, bool BitReverse, int K, bool BlackIs1)
{
    return ::PDF_open_CCITT(p, CHAR(filename), width, height, BitReverse, K, BlackIs1);
}

int
PDF::add_bookmark(pdfstring text, int parent, bool open)
{
    return ::PDF_add_bookmark(p, CHAR(text), parent, open);
}

void
PDF::set_info(pdfstring key, pdfstring value)
{
    ::PDF_set_info(p, CHAR(key), CHAR(value));
}

void
PDF::attach_file(float llx, float lly, float urx, float ury, pdfstring filename, pdfstring description, pdfstring author, pdfstring mimetype, pdfstring icon)
{
    ::PDF_attach_file(p, llx, lly, urx, ury, CHAR(filename), CHAR(description),
    		CHAR(author), CHAR(mimetype), CHAR(icon));
}

void
PDF::add_note(float llx, float lly, float urx, float ury, pdfstring contents, pdfstring title, pdfstring icon, bool open)
{
    ::PDF_add_note(p, llx, lly, urx, ury, CHAR(contents),
    	CHAR(title), CHAR(icon), open);
}

void
PDF::add_pdflink(float llx, float lly, float urx, float ury, pdfstring filename, int page, pdfstring dest)
{
    ::PDF_add_pdflink(p, llx, lly, urx, ury, CHAR(filename), page, CHAR(dest));
}

void
PDF::add_launchlink(float llx, float lly, float urx, float ury, pdfstring filename)
{
    ::PDF_add_launchlink(p, llx, lly, urx, ury, CHAR(filename));
}

void
PDF::add_locallink(float llx, float lly, float urx, float ury, int page, pdfstring dest)
{
    ::PDF_add_locallink(p, llx, lly, urx, ury, page, CHAR(dest));
}

void
PDF::add_weblink(float llx, float lly, float urx, float ury, pdfstring url)
{
    ::PDF_add_weblink(p, llx, lly, urx, ury, CHAR(url));
}

void
PDF::set_border_style(pdfstring style, float width)
{
    ::PDF_set_border_style(p, CHAR(style), width);
}

void
PDF::set_border_color(float red, float green, float blue)
{
    ::PDF_set_border_color(p, red, green, blue);
}

void
PDF::set_border_dash(float d1, float d2)
{
    ::PDF_set_border_dash(p, d1, d2);
}
