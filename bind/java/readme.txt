Notes on the PDFlib Java binding:

- On some of my systems Java uses the wrong timezone for the PDF clock -- why?.

- I seem to be unable to write catch clauses for some specific
  exceptions, e.g. IOException.

- Is it reasonable to introduce our own PDFexception class?

- It doesn't seem to be possible to catch malloc errors since the JNI
  generates a core dump signal before the PDFlib error handler jumps in.

- What would be the appropriate way of introducing packages and
  import statements for PDFlib, and getting rid of the ridiculous
  pdflib.PDF_... constructs?

- On some systems you will have to set the CLASSPATH environment variable
  to include . (the current directory) in order for pdflib.class to be
  found.
