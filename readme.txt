PDFlib V2.01
============
Portable C library for dynamically generating PDF ("Adobe Acrobat") files.
Copyright (c) 1997-1999 Thomas Merz (tm@muc.de)

The PDFlib distribution is available from the following URL:
http://www.ifconnection.de/~tm

THIS IS NOT PUBLIC DOMAIN OR FREEWARE SOFTWARE -- 
see end of this file for an overview of licensing and
usage issues, and the file doc/license.pdf for details.


Overview
========
PDFlib is a C library for generating PDF files. It offers a graphics
API with support for drawing, text, fonts, images, and hypertext. Call PDFlib
routines from within your client program and voila: dynamic PDF files!
For detailed instructions on PDFlib programming and the associated API,
see the PDFlib Programming Manual, included in PDF format in the
PDFlib distribution.


Supported Programming Environments
==================================
The PDFlib core library can be built as a static library or a shared
C library/DLL. PDFlib can be used from the following environments:

- ANSI C libary (static or dynamic)
- ANSI C++ via an object wrapper
- Perl5  with help from SWIG
- Tcl with help from SWIG
- Python with help from SWIG
- Java via the JNI with help from a hacked SWIG version
- Visual Basic with a DLL and a type library

- PHP3: PDFlib support is already included in the PHP3 distribution,
  see http://www.php.net. Note that PHP3 is not directly supported by
  the author of PDFlib.

The necessary "glue" for attaching PDFlib to these environments is
included in the distribution, as well as sample programs for all
supported environments.

Two sample applications are supplied for all supported language bindings:

- hello:	simple "hello, world!" program
- pdfclock:	generate a PDF file with an analog clock image


PDF Features
============
PDFlib's many features are covered in detail in the programming manual.
So let's just take a brief look at the most important ones:

- basic drawing
  lines, rectangles, arcs, Bezier curves, ...
  gray scale or RGB color
- text
  text in different fonts and sizes, formatting
  type 1 font embedding, built-in font metrics, AFM metrics files for 
  additional fonts
- image file formats
  support for CCITT, TIFF, GIF, and JPEG files
- PDF output compression via Zlib
- hypertext features
  bookmarks, page transitions, Web and local links, PDF file attachments
- Unicode support for annotations, bookmarks, and document information


PDFlib application ideas
========================
- Dynamic PDF generation on a Web server
- Directly convert text or graphics files to PDF
- Report generator for applications such as databases
- PDF-generating printer driver for all kinds of applications
- ...many more!


Supplied Library Clients
========================
PDFlib includes some test and real client programs:

- pdftest:	general C test program, illustrates many PDF features
- pdfimage:	convert GIF/TIFF/JPEG images to PDF
- text2pdf:	convert text files to PDF
- pdfgraph:	draw a graph according to data read from a text file


Platform support
================
PDFlib requires an ANSI C compiler and runs on many platforms since
it doesn't make use of an platform-specific features, and doesn't
have a user interface. PDFlib is primarily developed and tested
on the following platforms:

- Linux 2.2.1 with GCC
- Windows NT 4 with Microsoft Visual C++ 6.0
- MacOS 8.6 with Metrowerks CodeWarrior Pro 4

In addition to these platforms, PDFlib has been compiled and tested on 
many other flavors of Unix.


Where to start?
===============
PDFlib comes with a comprehensive manual which can be found as a
PDF document in the doc directory.


Building the library on Unix systems
====================================
Building PDFlib on Unix basically requires the usual

./configure
make
make test
make install

sequence. More information on configure options can be found
in the file doc/install.txt.

If your system doesn't offer getopt, you can use the supplied
public domain getopt module instead. By default, this module
is not used, so you will have to add it manually in the makefile.
Note that the core library doesn't use getopt, only some sample clients.


Building the library on Windows NT
===================================
To compile PDFlib with MS Visual C++, open the supplied workspace
file which contains several projects for the core library, test programs,
and language bindings. Set "test" to be the active project, and build.
Currently you will have to either copy the PDFlib DLL to the test
directory in order to execute the pdftest program (which starts
several threads for generating PDF), or seth your PATH appropriately.

A project file called pdflib_static may be used for building a
static version of the PDFlib library.


Building the library on the Mac
===============================
To compile PDFlib with Metrowerks CodeWarrior, open the supplied
project file PDFlib.mcp with the Metrowerks IDE. The pdftest target
will build the pdftest program in the test folder. This console program
can be used to test the library, and serve as a starting point for
your own PDFlib-powered applications.


Building the library on other platforms or with other compilers
===============================================================
In order to build PDFlib with other compilers, you will have to write
a suitable makefile or project file. Simply stuff all *.c and *.h
files in the pdflib subdirectory into your project.

If you want to build a DLL for Windows NT, set the PDFLIB_EXPORTS
define. In order to build a static library for PDFlib under Windows, set the 
PDFLIB_STATIC define. Set neither of these defines for including the
PDFlib header file in your application.


External libraries used in PDFlib
=================================
PDFlib makes use of other libraries if they are available:

- Sam Leffler's TIFFlib if you want to read TIFFs with PDFlib
  TIFFlib is available from ftp://ftp.sgi.com/graphics/tiff

  A Mac version of TIFFlib used to be available from
  http://www.cwi.nl/~jack/macsoftware.html

- The zlib compression library for compressed PDF output
  zlib is available from http://www.cdrom.com/pub/infozip/zlib

  A Windows DLL version of zlib used to be available from
  http://www.winimage.com/zLibDll/

  A Mac version of ZLIB used to be available from
  http://www.cwi.nl/~jack/macsoftware.html

Note that the use of these libraries on non-Unix platforms has not
yet been tested, the above URLs are quoted for your convenience only.
The Unix configure script will automatically find out which libraries
are available. Note that certain builds of the TIFF library also need
the JPEG library.

If the TIFFlib or zlib libraries are not available, PDFlib will
work anyway, although (of course) it will be lacking the respective features.


A Shameless Plug
================

My book has more information on PDFlib and PDF/Web integration.
The book is currently available in English, German, and Japanese:

Mit Acrobat ins World Wide Web
Effiziente Erstellung von PDF-Dateien und ihre Einbindung ins Web.
Thomas Merz Verlag Muenchen, ISBN 3-9804943-1-4.

Web Publishing with Acrobat/PDF
Springer-Verlag Heidelberg Berlin New York 1998, ISBN 3-540-63762-1

Japanese edition:
Tokyo Denki Daigaku 1999, ISBN 4-501-53020-0
See http://plaza4.mbn.or.jp/~unit


Licensing and Copyright
=======================

THIS IS NOT PUBLIC DOMAIN OR FREEWARE SOFTWARE!

This software is subject to the "ALADDIN FREE PUBLIC LICENSE". The
complete text of the license agreement can be found in the file
doc/license.pdf. In short and non-legal terms:

- You may develop and use software based on PDFlib without paying a fee
- You may non-commercially re-distribute PDFlib without paying a fee,
  provided you don't modify the package
- Selling PDFlib-based products requires a commercial license. Please
  contact the author for licensing details.

Note that this is only a 10-second-description which is not legally
binding. Only the text in the license file is considered to completely
describe the licensing conditions. In order to obtain a commercial
license, please contact the author (see below).

If you use this software you need to honor the copyright notices. I would
also be interested in hearing about what you use PDFlib for (and, hopefully,
be acknowledged).

Copyright (c) 1997-1999 Thomas Merz. All rights reserved.


Thomas Merz
Consulting & Publishing
Tal 40
80331 Muenchen, Germany

tm@muc.de
http://www.ifconnection.de/~tm
fax +49/89/29 16 46 86
