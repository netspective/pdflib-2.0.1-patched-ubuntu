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

/* pdflib.i
 *
 * PDFlib interface file for SWIG
 *
 */

#if	defined(SWIGJAVA)
%title "PDFlib scripting API reference for Java (currently incomplete)"
#elif	defined(SWIGPERL)
%title "PDFlib scripting API reference for Perl"
#elif	defined(SWIGPYTHON)
%title "PDFlib scripting API reference for Python"
#elif	defined(SWIGTCL)
%title "PDFlib scripting API reference for Tcl"
#endif

%module pdflib
%style before, noinfo

/* Fetch SWIG's exception handling mechanism */
%include exception.i

%text %{
This is only a brief summary of the PDFlib API interface.
For a more complete description of the functions see the PDFlib API
reference manual.

Most functions do not return any value. For those functions which do
return a value, the meaning of the returned value is explained in the
function description below.

\\
%}

/* The JNI currently doesn't support initialization routines - we handle
 this ourselves with a static method in the Java pragma further below.
 */
#if !defined(SWIGJAVA)
%init %{
	/* Boot the PDFlib core */
	PDF_boot();
%}
#endif

#ifdef SWIGTCL
/* to get Tcl_Interp *interp handled correctly */
%import typemaps.i

/* THE PDFLIB_VERSIONSTRING macro will be substituted in the wrapper file */
%init %{
	/* Tell Tcl which package we are going to define */
	Tcl_PkgProvide(interp, "pdflib", PDFLIB_VERSIONSTRING);
%}
#endif

/* The version number of the shared library will be patched by make */
%pragma(java) module = "
    // The initialization code for loading the PDFlib shared library.
    // The library name will be transformed into something platform-
    // specific by the VM, e.g. libpdf_java2.0.so or pdf_java2.0.dll.
    static {
	try {
	    System.loadLibrary(\"pdf_java2.01\");
	} catch (UnsatisfiedLinkError e) {
	    System.err.println(
	    \"Cannot load the PDFlib shared library for Java.\n\" +
	    \"Make sure that LD_LIBRARY_PATH (Unix) or PATH (Win32)\n\" +
	    \"contain the current directory\n\" + e);
	    System.exit(1);
	}
	PDF_boot();
    }
";

/* The following block will be copied verbatim to the wrapper files. */
%{
#include <setjmp.h>

/* The following sick stuff is an attempt to make recent Perl versions
 * work with recent SWIG versions compiled with recent MS VC++ versions.
 */
#ifdef _MSC_VER
    #undef setjmp
    #define setjmp _setjmp
    #undef longjmp
#endif

#include "pdflib.h"

/* Map PDFlib errors to SWIG exceptions */
static const int pdf_swig_exceptions[] = {
    0,
    SWIG_MemoryError,	/* PDF_MemoryError    1 */
    SWIG_IOError,	/* PDF_IOError        2 */
    SWIG_RuntimeError,	/* PDF_RuntimeError   3 */
    SWIG_IndexError,	/* PDF_IndexError     4 */
    SWIG_TypeError,	/* PDF_TypeError      5 */
    SWIG_DivisionByZero,/* PDF_DivisionByZero 6 */
    SWIG_OverflowError,	/* PDF_OverflowError  7 */
    SWIG_SyntaxError,	/* PDF_SyntaxError    8 */
    SWIG_ValueError,	/* PDF_ValueError     9 */
    SWIG_SystemError,	/* PDF_SystemError   10 */
    SWIG_UnknownError,	/* PDF_NonfatalError 11 */ /* PDFlib-special */
    SWIG_UnknownError,	/* PDF_UnknownError  12 */
};

/* SWIG exception handling is currently not thread-safe! */
static jmp_buf		exception_buffer;
static int		exception_status;
static char		error_message[256];

/* Exception handling for SWIG. Note that these are not used for the C API */
#define try		if ((exception_status = setjmp(exception_buffer)) == 0)
#define catch(error)	else if (exception_status == error)
#define throw(error)	longjmp(exception_buffer, error)
#define finally		else

/* PDFlib error handler for the SWIG libraries */

static void
pdf_swig_errorhandler(PDF *p, int type, const char* shortmsg)
{
    sprintf(error_message, "PDFlib: %s\n", shortmsg);

    /* Issue a warning message and continue for non-fatal errors */
    if (type == PDF_NonfatalError) {
	fprintf(stderr, error_message);
	return;
    }
	
    /* Clean up PDFlib internals and delete p in all other cases */
    if (p != NULL)
	PDF_delete(p);

    /* ...and throw an exception */
    throw(type);
}

/* Force the SWIG error handler for all clients */
#define PDF_new() PDF_new2(pdf_swig_errorhandler, NULL, NULL, NULL, NULL)
%}

#ifdef SWIGJAVA
%{
/* Map PDFlib errors to Java exceptions */
static const char *pdf_java_exceptions[] = {
NULL,
"java/lang/OutOfMemoryError",		/* PDF_MemoryError    1 */
"java/io/IOException",			/* PDF_IOError        2 */
"java/lang/IllegalArgumentException",	/* PDF_RuntimeError   3 */
"java/lang/IndexOutOfBoundsException",	/* PDF_IndexError     4 */
"java/lang/ClassCastException",		/* PDF_TypeError      5 */
"java/lang/ArithmeticException",	/* PDF_DivisionByZero 6 */
"java/lang/ArithmeticException",	/* PDF_OverflowError  7 */
"java/lang/RuntimeException",		/* PDF_SyntaxError    8 */
"java/lang/IllegalArgumentException",	/* PDF_ValueError     9 */
"java/lang/InternalError",		/* PDF_SystemError   10 */
"java/lang/UnknownError",		/* PDF_NonfatalError 11 */
"java/lang/UnknownError",		/* PDF_UnknownError  12 */
};

/* This should come from SWIG but SWIG's experimental Java support
 * currently doesn't cover exceptions.
 */

#define SWIG_exception(type, msg) (*jenv)->ThrowNew(jenv, \
        (*jenv)->FindClass(jenv, pdf_java_exceptions[type]), msg);

%}

#endif	/* SWIGJAVA */

/* This error handler which will be used for all languages.
 * The construction is somewhat uglier than the usual try...catch...catch,
 * but this will save us ca. 2000 (!) lines of code in the generated
 * wrapper files since all wrapped functions will be affected by
 * code bloat from the exception handling mechanism.
 */
%except {
    try { $function }
    else {
	SWIG_exception(pdf_swig_exceptions[exception_status], error_message);
    }
}

%{
/* export the PDFlib routines to the shared library */
#ifdef __MWERKS__
#pragma export on
#endif

%}

/* This #include will actually be carried out by SWIG. */
%include "pdflib.h"

#ifdef SWIGPERL
%echo %{
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Don't forget to change the following line in pdflib_pl.c:
    SWIGEXPORT(void,boot_pdflib)(CPerl *, CV *cv);
to:
    SWIGEXPORT(void,boot_pdflib)(CV *cv, CPerlObj *pPerl);
This is only necessary for using PDFlib with ActivePerl on Win32.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
%}
#endif /* SWIGPERL */
