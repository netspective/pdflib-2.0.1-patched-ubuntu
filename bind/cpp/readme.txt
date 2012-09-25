Notes on the PDFlib C++ binding:

- What's the correct procedure for errors in the PDF constructor?

- Do we need -lstdc++ for linking the sample programs? Is this portable?

- How to best integrate PDFlib's error handling machinery with C++
  exceptions? Throwing exceptions from C code seems to be highly
  compiler-specific and unreliable.
