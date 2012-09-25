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
LANG_LIBDIR 	= ${exec_prefix}/lib

# Java support requires swig1.2
SWIG		= swig1.2
SWIG_LIB	= libpdf_java2.01$(SO)
SWIG_OBJ	= pdflib_java.$(OBJ)
SWIG_SRC	= pdflib_java.c
SWIG_DOC	= pdflib_java.doc
SWIG_EXTRA	= pdflib.java
SWIGFLAGS	= -I../../pdflib -java -d pdflib_java -stat

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
	-javac hello.java && LD_LIBRARY_PATH=../../pdflib:. java hello
	-javac pdfclock.java && LD_LIBRARY_PATH=../../pdflib:. java pdfclock

install: $(SWIG_LIB)
	-$(INSTALL) $(SWIG_EXTRA) $(LANG_LIBDIR)
	-$(INSTALL) $(SWIG_LIB) $(LANG_LIBDIR)
	-cd $(LANG_LIBDIR); ln -s $(SWIG_LIB) libpdf_java$(SO)
	-echo "Make sure to have your CLASSPATH point to pdflib.class"

uninstall:
	-$(RM) $(LANG_LIBDIR)/$(SWIG_LIB) $(LANG_LIBDIR)/$(SWIG_EXTRA)

clean:
	-$(RM) $(SWIG_LIB) $(SWIG_OBJ)
	-$(RM) hello_java.pdf pdfclock_java.pdf
	-$(RM) pdflib.class hello.class pdfclock.class

maintainer-clean: clean
	-$(RM) $(SWIG_SRC) $(SWIG_DOC) $(SWIG_EXTRA)
