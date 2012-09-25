# Generated automatically from Makefile.in by configure.
# Makefile for PDFlib

VERSION = 2.01

SHELL = /bin/sh
INSTALL = /usr/bin/install -c

srcdir = .

EXE	= 
OBJ	= o
AR	= ar rc
CC	= gcc
RM      = rm -f
SO	= .so
RANLIB	= ranlib

# installation directories
prefix = /usr/local
exec_prefix = ${prefix}

# position independent code for shared libraries
LDSHARED = gcc -shared

PDFLIB = libpdf2.01.so
PDFLIBLINK = -L../../pdflib -lpdf2.01   -lm
CFLAGS = -c   -fpic -I../../pdflib

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $<

# Language-dependent definitions
LANG_INCDIR	= -I/usr/local/include
LANG_LIBDIR 	= nope/pdflib
TCLBIN		= nope

SWIG		= swig1.2
SWIG_LIB	= pdflib$(SO)
SWIG_OBJ	= pdflib_tcl.$(OBJ)
SWIG_SRC	= pdflib_tcl.c
SWIG_EXTRA	= pkgIndex.tcl
SWIG_DOC	= pdflib_tcl.doc
SWIGFLAGS	= -I../../pdflib -tcl -d pdflib_tcl -stat

all:	$(SWIG_LIB)

PDFLIB_I	= ../../pdflib/pdflib.i

# The swig-generated wrapper file
$(SWIG_SRC): $(PDFLIB_I) ../../pdflib/pdflib.h
	$(SWIG) $(SWIGFLAGS) -o $@ $(PDFLIB_I)

$(SWIG_OBJ):	$(SWIG_SRC)
	$(CC) $(CFLAGS) $(SWIG_SRC) $(LANG_INCDIR) $(DEFINES) $<

# The actual shared library for the language support plus, the TCL index file
$(SWIG_LIB): ../../pdflib/$(PDFLIB) $(SWIG_OBJ)
	$(RM) $(SWIG_LIB)
	$(LDSHARED) $(SWIG_OBJ) -o $(SWIG_LIB) $(PDFLIBLINK)
	echo "pkg_mkIndex . $(SWIG_LIB)" | LD_LIBRARY_PATH=../../pdflib $(TCLBIN)

test: all
	-LD_LIBRARY_PATH=../../pdflib $(TCLBIN) hello.tcl
	-LD_LIBRARY_PATH=../../pdflib $(TCLBIN) pdfclock.tcl

install: $(SWIG_LIB)
	-mkdir $(LANG_LIBDIR)
	-$(INSTALL) $(SWIG_LIB) $(LANG_LIBDIR)
	-$(INSTALL) $(SWIG_EXTRA) $(LANG_LIBDIR)

uninstall:
	-$(RM) $(LANG_LIBDIR)/$(SWIG_LIB) $(LANG_LIBDIR)/$(SWIG_EXTRA)

clean:
	-$(RM) $(SWIG_LIB) $(SWIG_OBJ) hello_tcl.pdf pdfclock_tcl.pdf

maintainer-clean: clean
	-$(RM) $(SWIG_SRC) $(SWIG_DOC) $(SWIG_EXTRA)
