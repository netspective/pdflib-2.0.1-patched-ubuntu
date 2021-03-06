Notes on the PDFlib Perl binding:

Perl versions 5.005_54 and newer
================================

Sometime after Perl 5.005_03 (most probably 5.005_54 -- I don't know the
exact version number) an incompatible change was introduced in the Perl
headers and source code which isn't yet reflected in the SWIG-generated
C wrapper file.

If you use a version of Perl newer thant 5.005_03 and experience lots 
of compiler errors, try adding the following lines at the beginning of
pdflib_pl.c:

#define sv_yes          PL_sv_yes
#define na              PL_na
#define sv_undef        PL_sv_undef


ActivePerl on Win32
===================

Given the blazing speed of Perl development, we had a
hard time making the PDFlib Perl extension module work on both
Unix and Win32. We finally came up with the following solution,
which has (on Windows NT 4.0) been tested with

- MS Visual C++ 6.0
- ActiveState Perl build 517, based on Perl 5.005_03

Since the whole topic is heavily in motion, our solution may
break with other versions or compilers. Due to the local pathnames
involved, it doesn't hurt to note what's going on since many
people will have to change some settings.

You may find the following notes useful when trying to make other
combinations work:

- We must use SWIG 1.2 (currently alpha) because older versions can't
  deal with the macro definitions used in pdflib.h. Don't try to
  use older versions of SWIG -- it doesn't solve any problems you
  may have!

- The following have already been taken care of in the SWIG input
  file pdflib.i:
  - Both the Perl and Microsoft include files redefine setjmp/longjmp
    with macro definitions. We try to work around this mess by adding
    another layer of macro mess.

- The following have already been taken care of in the VC++ project file:
  - Add PERL_OBJECT to the list of pre-defined names since ActiveState
    uses the C++ object wrapper for the Perl interpreter.
  - Run the compiler in C++ mode (required by ActiveState). There
    doesn't seem to be a GUI option for C++ mode, but the compiler switch
    /Tp works.
  - Add the equivalent of the following to the list of pre-defined names:
    #define CPerl CPerlObj
  - Add the following directories to the include path, where <AP>
    denotes the ActivePerl source directory:
    <AP>
    <AP>\win32
    <AP>\win32\include

- pdflib_pl.c as contained in the distribution was hacked after
  being generated by SWIG:
  - Change the following line in pdflib_pl.c:
      SWIGEXPORT(void,boot_pdflib)(CPerl *, CV *cv);
    to:
      SWIGEXPORT(void,boot_pdflib)(CV *cv, CPerlObj *pPerl);

    Make sure to change this line in the PERL_OBJECT branch of the
    respective #ifdef.
 
- Prepare the ActiveState Perl sources:
  - rename <AP>\win32\config_h.vc to config.h and copy it to <AP>.


MacPerl
=======

In order to build a PDFlib Perl extension on the Mac, the PDFlib shared
library must be linked against a library named PerlStub which is supplied
with the Perl source distribution.
