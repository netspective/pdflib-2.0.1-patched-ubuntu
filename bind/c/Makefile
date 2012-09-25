# Generated automatically from Makefile.in by configure.
# Makefile for PDFlib C samples

VERSION = 2.01

SHELL = /bin/sh

srcdir = .

PDFLIB = libpdf2.01.so
LIBS	= -lpdf2.01   -lm

EXE	= 
OBJ	= o
AR	= ar rc
CC	= gcc
RM      = rm -f

CFLAGS	= -c   -g -O2  $(PIC_OPT) -I../../pdflib
LDFLAGS = -L../../pdflib

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $<

# ------------------------------

SRC	= \
	$(srcdir)/hello.c		\
	$(srcdir)/pdfclock.c

OBJS	= \
	$(srcdir)/hello.$(OBJ)		\
	$(srcdir)/pdfclock.$(OBJ)

all:	hello$(EXE) pdfclock$(EXE)

test: hello$(EXE) pdfclock$(EXE)
	LD_LIBRARY_PATH=../../pdflib ./hello
	LD_LIBRARY_PATH=../../pdflib ./pdfclock

hello$(EXE): hello.$(OBJ) ../../pdflib/$(PDFLIB)
	$(CC) $(LDFLAGS) -o $@ hello.$(OBJ) $(LIBS)

pdfclock$(EXE): pdfclock.$(OBJ) ../../pdflib/$(PDFLIB)
	$(CC) $(LDFLAGS) -o $@ pdfclock.$(OBJ) $(LIBS)

clean:
	-$(RM) $(OBJS) hello$(EXE) pdfclock$(EXE) hello.pdf pdfclock.pdf

maintainer-clean: clean
	-$(RM) Makefile.in.bak

depend:
	cp Makefile.in Makefile.in.bak
	sed '/^# Automatically generated dependencies/q' Makefile.in.bak >Makefile.in
	$(CC) $(CFLAGS) -MM $(SRC)>>Makefile.in

# Automatically generated dependencies
hello.o: ./hello.c ../../pdflib/pdflib.h
pdfclock.o: ./pdfclock.c ../../pdflib/pdflib.h
