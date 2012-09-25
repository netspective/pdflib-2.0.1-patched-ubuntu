# Generated automatically from Makefile.in by configure.
# Makefile for PDFlib tests

VERSION = 2.01

SHELL = /bin/sh

srcdir	= .

PDFLIB	= libpdf2.01.so
LIBS	= -L../pdflib -lpdf2.01   -lm

EXE	= 
OBJ	= o
AR	= ar rc
CC	= gcc
RM      = rm -f

CFLAGS	= -c   -g -O2  -I../pdflib
LDFLAGS = 

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $<

# ------------------------------

SRC	= \
	$(srcdir)/pdftest.c

OBJS	= \
	$(srcdir)/pdftest.$(OBJ)

all:	pdftest$(EXE)

test: pdftest$(EXE) 
	LD_LIBRARY_PATH=../pdflib ./pdftest

pdftest$(EXE): $(OBJS) ../pdflib/$(PDFLIB)
	$(CC) $(LDFLAGS) -o $@ pdftest.$(OBJ) $(LIBS)

clean:
	-$(RM) pdftest$(EXE) pdftest*.pdf $(OBJS)

maintainer-clean: clean
	-$(RM) Makefile.in.bak

depend:
	cp Makefile.in Makefile.in.bak
	sed '/^# Automatically generated dependencies/q' Makefile.in.bak >Makefile.in
	$(CC) $(CFLAGS) -MM $(SRC)>>Makefile.in

# Automatically generated dependencies
pdftest.o: ./pdftest.c ../pdflib/pdflib.h
