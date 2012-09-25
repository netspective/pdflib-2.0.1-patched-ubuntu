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

/* p_config.h
 *
 * PDFlib portability and configuration definitions
 *
 */

#ifndef P_CONFIG_H
#define P_CONFIG_H

#define HAVE_GETOPT

/* ---------------------- WIN32  -------------------*/
#if (defined(_WIN32) || defined(__WATCOMC__)) && !defined(WIN32)
#define WIN32
#endif

#ifdef WIN32
#define READMODE	"rb"
#define WRITEMODE	"wb"
#undef	HAVE_GETOPT
#endif	/* WIN32 */

/* ---------------------- Macintosh -------------------*/
/* try to identify Mac compilers */

#if __POWERPC__ || __CFM68K__ || __MC68K__
#define MAC
#endif

#ifdef MAC
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define PATHSEP		":"
#undef	HAVE_GETOPT
#endif

/* ---------------------- Defaults -------------------*/
#ifndef READMODE
#define READMODE	"r"
#endif
#ifndef WRITEMODE
#define WRITEMODE	"w"
#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846      /* pi */
#endif

#ifndef PATHSEP
#define PATHSEP		"/"
#endif

/* #undef this if you don't have setlocale() */
#define HAVE_SETLOCALE

#ifdef _DEBUG
#define DEBUG
#endif

#endif /* P_CONFIG_H */
