# Generated automatically from Makefile.in by configure.
# Makefile for PDFlib

VERSION = 2.01

SHELL = /bin/sh
INSTALL = /usr/bin/install -c

srcdir = .

PDFLIB = ../pdflib/libpdf2.01.so
LIBS	= -lpdf2.01   -lm

EXE	= 
OBJ	= o
AR	= ar rc
CXX	= c++
RM      = rm -f
SO	= .so
RANLIB	= ranlib

# installation directories
prefix = /usr/local
exec_prefix = ${prefix}

PDFLIB = libpdf2.01.so
CXXFLAGS = -c   -fpic -I../../pdflib
LDFLAGS = -L../../pdflib

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) $(CXXFLAGS) $(DEFINES) $<

# ----------------------------------

SRC	= \
	$(srcdir)/pdflib.cpp	\
	$(srcdir)/hello.cpp	\
	$(srcdir)/pdfclock.cpp

OBJS	= \
	$(srcdir)/pdflib.o	\
	$(srcdir)/hello.o	\
	$(srcdir)/pdfclock.o

all:	pdflib.o hello$(EXE) pdfclock$(EXE)

test: hello$(EXE) pdfclock$(EXE)
	LD_LIBRARY_PATH=../../pdflib ./hello
	LD_LIBRARY_PATH=../../pdflib ./pdfclock

pdflib.o: pdflib.cpp

pdfclock$(EXE): pdfclock.o pdflib.o ../../pdflib/$(PDFLIB)
	$(CXX) $(LDFLAGS) -o $@ pdfclock.$(OBJ) pdflib.o $(LIBS)

hello$(EXE): hello.o pdflib.o ../../pdflib/$(PDFLIB)
	$(CXX) $(LDFLAGS) -o $@ hello.$(OBJ) pdflib.o $(LIBS)

install: 

uninstall:

clean:
	-$(RM) $(OBJS) hello$(EXE) pdfclock$(EXE) hello_cpp.pdf pdfclock_cpp.pdf

maintainer-clean: clean
	-$(RM) Makefile.in.bak

depend:
	cp Makefile.in Makefile.in.bak
	sed '/^# Automatically generated dependencies/q' Makefile.in.bak >Makefile.in
	$(CXX) $(CXXFLAGS) -MM $(SRC)>>Makefile.in

# Automatically generated dependencies
pdflib.o: ./pdflib.cpp pdflib.hpp ../../pdflib/pdflib.h
hello.o: ./hello.cpp pdflib.hpp ../../pdflib/pdflib.h
pdfclock.o: ./pdfclock.cpp pdflib.hpp ../../pdflib/pdflib.h
