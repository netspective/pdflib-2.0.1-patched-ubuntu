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
CFLAGS = -c   -g -O2  -fpic -I../../pdflib

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $<

# Language-dependent definitions
LANG_INCDIR	= -I/usr/local/include/Py
LANG_LIBDIR 	= ${exec_prefix}/lib/python

SWIG		= swig1.2
SWIG_LIB	= pdflib$(SO)
SWIG_OBJ	= pdflib_py.$(OBJ)
SWIG_SRC	= pdflib_py.c
SWIG_EXTRA	= 
SWIG_DOC	= pdflib_py.doc
SWIGFLAGS	= -I../../pdflib -python -d pdflib_py -stat

all:	$(SWIG_LIB)

PDFLIB_I	= ../../pdflib/pdflib.i

# The swig-generated wrapper file
$(SWIG_SRC): $(PDFLIB_I) ../../pdflib/pdflib.h
	$(SWIG) $(SWIGFLAGS) -o $@ $(PDFLIB_I)

$(SWIG_OBJ):	$(SWIG_SRC)
	$(CC) $(CFLAGS) $(SWIG_SRC) $(LANG_INCDIR) $(DEFINES) $<

# The actual shared library for the language support
$(SWIG_LIB): ../../pdflib/$(PDFLIB) $(SWIG_OBJ)
	$(RM) $(SWIG_LIB)
	$(LDSHARED) $(SWIG_OBJ) -o $(SWIG_LIB) $(PDFLIBLINK)

test: all
	-LD_LIBRARY_PATH=../../pdflib python hello.py
	-LD_LIBRARY_PATH=../../pdflib python pdfclock.py

install: $(SWIG_LIB)
	-$(INSTALL) $(SWIG_LIB) $(LANG_LIBDIR)

uninstall:
	-$(RM) $(LANG_LIBDIR)/$(SWIG_LIB)

clean:
	-$(RM) $(SWIG_LIB) $(SWIG_OBJ) hello_py.pdf pdfclock_py.pdf

maintainer-clean: clean
	-$(RM) $(SWIG_SRC) $(SWIG_DOC)
