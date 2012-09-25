# Generated automatically from Makefile.in by configure.
# Makefile for PDFlib utilities
# Before running this Makefile, a "make install" must have been
# run to install the PDFlib library.

VERSION = 2.01

SHELL = /bin/sh
INSTALL = /usr/bin/install -c

srcdir = .

PDFLIB = libpdf2.01.so
LIBS	= -L../pdflib -lpdf2.01   -lm

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

# directories for the final library and include files for PDFlib users
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

TIFFINC	= 

CFLAGS = -c   -g -O2  -I../pdflib $(TIFFINC)

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $<

SRC	= \
	compile_metrics.c

OBJS	= \
	compile_metrics.$(OBJ)

PROGS	= \
	compile_metrics$(EXE)

all:	$(PROGS)

clean:
	$(RM) $(OBJS) $(PROGS) Makefile.in.bak

maintainer-clean: clean

distclean:

maintainer-clean:

all:	$(PROGS)

distclean:

maintainer-clean:

depend:
	cp Makefile.in Makefile.in.bak
	sed '/^# Automatically generated dependencies/q' Makefile.in.bak >Makefile.in
	$(CC) $(CFLAGS) -MM $(SRC)>>Makefile.in

compile_metrics$(EXE):	compile_metrics.$(OBJ)
	$(CC) $(LDFLAGS) -o $@ compile_metrics.$(OBJ) $(LIBS)

# Automatically generated dependencies
compile_metrics.o: compile_metrics.c ../pdflib/p_intern.h \
 ../pdflib/pdflib.h ../pdflib/p_config.h ../pdflib/p_afmparse.h \
 ../pdflib/ansi_e.h ../pdflib/macrom_e.h ../pdflib/pdfdoc_e.h
