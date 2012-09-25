# Generated automatically from Makefile.PL.in by configure.
#!/usr/bin/perl
# Makefile.PL
# Copyright (C) 1997-99 Thomas Merz. All rights reserved.
#
# Input for generating a Perl binding for PDFlib with MakeMaker

use ExtUtils::MakeMaker;

WriteMakefile(
'CCFLAGS' => '',
'DEFINE' => '-Dbool=char -DHAS_BOOL -g -O2  -fpic',
'INC' => '-I../../pdflib -I/usr/lib/perl/5.14/CORE',
'LIBS' => '-L../../pdflib -lpdf2.01   -lm ',
'NAME' => 'pdflib',
'OBJECT' => 'pdflib_pl.o',
'PM' => {'pdflib.pm' => '$(INST_LIBDIR)/pdflib.pm'},
'VERSION' => '2.01',
);
