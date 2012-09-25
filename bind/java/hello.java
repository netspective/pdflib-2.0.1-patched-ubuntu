/* hello.java
 * Copyright (C) 1997-99 Thomas Merz. All rights reserved.
 *
 * PDFlib client: hello example in Java
 */

public class hello
{
    public static void main (String argv[])
    {
	long p;
	int font;

	p = pdflib.PDF_new();

	if (pdflib.PDF_open_file(p, "hello_java.pdf") == -1) {
	    System.err.println("Couldn't open PDF file hello_java.pdf\n");
	    System.exit(1);
	}

	pdflib.PDF_set_info(p, "Creator", "hello.java");
	pdflib.PDF_set_info(p, "Author", "Thomas Merz");
	pdflib.PDF_set_info(p, "Title", "Hello world (Java)");

	pdflib.PDF_begin_page(p, 595, 842);

	font = pdflib.PDF_findfont(p, "Helvetica-Bold", "default", 0);
	if (font == -1){
	    System.err.println("Couldn't find font!\n");
	    System.exit(1);
	}

	pdflib.PDF_setfont(p, font, 18);

	pdflib.PDF_set_text_pos(p, 50, 700);
	pdflib.PDF_show(p, "Hello world!");
	pdflib.PDF_continue_text(p, "(says Java)");
	pdflib.PDF_end_page(p);

	pdflib.PDF_close(p);
	pdflib.PDF_delete(p);
    }
}
