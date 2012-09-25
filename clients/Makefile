# Generated automatically from Makefile.in by configure.
# Makefile for PDFlib clients

VERSION = 2.01

SHELL = /bin/sh

srcdir = .

PDFLIB = libpdf2.01.so
LIBS	= -L../pdflib -lpdf2.01   -lm

EXE	= 
OBJ	= o
AR	= ar rc
CC	= gcc
RM      = rm -f

CFLAGS	= -c   -g -O2  $(PIC_OPT) -I../pdflib
LDFLAGS = 

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $<

# ------------------------------

SRC	= \
	$(srcdir)/pdfimage.c	\
	$(srcdir)/pdfgraph.c	\
	$(srcdir)/text2pdf.c

OBJS	= \
	$(srcdir)/pdfimage.$(OBJ)	\
	$(srcdir)/pdfgraph.$(OBJ)	\
	$(srcdir)/text2pdf.$(OBJ)

all:	 pdfimage$(EXE) pdfgraph$(EXE) text2pdf$(EXE)

test:	all

pdfimage$(EXE): pdfimage.$(OBJ) ../pdflib/$(PDFLIB) ../pdflib/pdflib.h
	$(CC) $(LDFLAGS) -o $@ pdfimage.$(OBJ) $(LIBS)

pdfgraph$(EXE): pdfgraph.$(OBJ) ../pdflib/$(PDFLIB) ../pdflib/pdflib.h
	$(CC) $(LDFLAGS) -o $@ pdfgraph.$(OBJ) $(LIBS)

text2pdf$(EXE): text2pdf.$(OBJ) ../pdflib/$(PDFLIB) ../pdflib/pdflib.h
	$(CC) $(LDFLAGS) -o $@ text2pdf.$(OBJ) $(LIBS)

clean:
	-$(RM) $(OBJS) pdfimage$(EXE) pdfgraph$(EXE) text2pdf$(EXE)

maintainer-clean: clean
	-$(RM) Makefile.in.bak

depend:
	cp Makefile.in Makefile.in.bak
	sed '/^# Automatically generated dependencies/q' Makefile.in.bak >Makefile.in
	$(CC) $(CFLAGS) -MM $(SRC)>>Makefile.in

# Automatically generated dependencies
pdfimage.o: ./pdfimage.c ../pdflib/pdflib.h ../pdflib/p_config.h
pdfgraph.o: ./pdfgraph.c ../pdflib/pdflib.h ../pdflib/p_config.h
text2pdf.o: ./text2pdf.c ../pdflib/pdflib.h ../pdflib/p_config.h
