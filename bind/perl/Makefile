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
CFLAGS = -c -Dbool=char -DHAS_BOOL   -g -O2  -fpic -I../../pdflib

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $<

# Language-dependent definitions
LANG_INCDIR	= -I/usr/lib/perl/5.14/CORE
LANG_LIBDIR 	= /usr/local/lib/perl/5.14.2
PERLMODDIR	= /usr/local/share/perl/5.14.2
SWIGFLAGS	= -I../../pdflib -perl5 -shadow -d pdflib_pl -stat

SWIG		= swig1.2
SWIG_LIB	= pdflib$(SO)
SWIG_OBJ	= pdflib_pl.$(OBJ)
SWIG_SRC	= pdflib_pl.c
SWIG_EXTRA	= pdflib.pm
SWIG_DOC	= pdflib_pl.doc

all:	$(SWIG_LIB)

PDFLIB_I	= ../../pdflib/pdflib.i

# The swig-generated wrapper file
$(SWIG_SRC): $(PDFLIB_I) ../../pdflib/pdflib.h
	$(SWIG) $(SWIGFLAGS) -o $@ $(PDFLIB_I)
	awk '/@ISA = qw\(Exporter DynaLoader\)/{print "$$VERSION=2.01;"};{print }' < pdflib.pm >pdflib.pm.tmp;
	rm pdflib.pm  && mv pdflib.pm.tmp pdflib.pm

$(SWIG_OBJ):	$(SWIG_SRC)
	$(CC) $(CFLAGS) $(SWIG_SRC) $(LANG_INCDIR) $(DEFINES)

# The actual shared library for the language support
$(SWIG_LIB): ../../pdflib/$(PDFLIB) $(SWIG_OBJ)
	$(RM) $(SWIG_LIB)
	$(LDSHARED) $(SWIG_OBJ) -o $(SWIG_LIB) $(PDFLIBLINK)

test: all
	-LD_LIBRARY_PATH=../../pdflib perl hello.pl
	-LD_LIBRARY_PATH=../../pdflib perl pdfclock.pl

install: $(SWIG_LIB)
	-$(INSTALL) $(SWIG_LIB) $(LANG_LIBDIR)
	-$(INSTALL) $(SWIG_EXTRA) $(LANG_LIBDIR)

uninstall:
	-$(RM) $(LANG_LIBDIR)/$(SWIG_LIB) $(LANG_LIBDIR)/$(SWIG_EXTRA) 

clean:
	-$(RM) $(SWIG_LIB) $(SWIG_OBJ) hello_pl.pdf pdfclock_pl.pdf

maintainer-clean: clean
	-$(RM) $(SWIG_SRC) $(SWIG_DOC) $(SWIG_EXTRA)
