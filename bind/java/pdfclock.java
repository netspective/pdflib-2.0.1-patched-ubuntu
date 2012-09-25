/* hello.java
 * Copyright (C) 1997-99 Thomas Merz. All rights reserved.
 *
 * PDFlib client: pdfclock example in Java
 */

import java.text.*;		// SimpleDateFormat
import java.util.*;		// Date

public class pdfclock
{
    public static void main (String argv[])
    {
	long p;
	int tm_hour, tm_min, tm_sec, alpha;
	float RADIUS = 200.0f;
	float MARGIN = 20.0f;
	SimpleDateFormat format;
	Date now = new Date();

	p = pdflib.PDF_new();

	if (pdflib.PDF_open_file(p, "pdfclock_java.pdf") == -1) {
	    System.err.println("Couldn't open PDF file" +"pdfclock_java.pdf\n");
	    System.exit(1);
	}

	pdflib.PDF_set_info(p, "Creator", "pdfclock.java");
	pdflib.PDF_set_info(p, "Author", "Thomas Merz");
	pdflib.PDF_set_info(p, "Title", "PDF clock (Java)");

	pdflib.PDF_begin_page(p,   (int) (2 * (RADIUS + MARGIN)),
				(int) (2 * (RADIUS + MARGIN)));

	pdflib.PDF_set_transition(p, "wipe");
	pdflib.PDF_set_duration(p, 0.5f);

	pdflib.PDF_translate(p, RADIUS + MARGIN, RADIUS + MARGIN);
	pdflib.PDF_setrgbcolor(p, 0.0f, 0.0f, 1.0f);
	pdflib.PDF_save(p);

	// minute strokes 
	pdflib.PDF_setlinewidth(p, 2.0f);
	for (alpha = 0; alpha < 360; alpha += 6)
	{
	    pdflib.PDF_rotate(p, 6.0f);
	    pdflib.PDF_moveto(p, RADIUS, 0.0f);
	    pdflib.PDF_lineto(p, RADIUS-MARGIN/3, 0.0f);
	    pdflib.PDF_stroke(p);
	}

	pdflib.PDF_restore(p);
	pdflib.PDF_save(p);

	// 5 minute strokes
	pdflib.PDF_setlinewidth(p, 3.0f);
	for (alpha = 0; alpha < 360; alpha += 30)
	{
	    pdflib.PDF_rotate(p, 30.0f);
	    pdflib.PDF_moveto(p, RADIUS, 0.0f);
	    pdflib.PDF_lineto(p, RADIUS-MARGIN, 0.0f);
	    pdflib.PDF_stroke(p);
	}

	// how to correctly set the time zone?
	format = new SimpleDateFormat("hh");
	tm_hour= Integer.parseInt(format.format(now));
	format = new SimpleDateFormat("mm");
	tm_min = Integer.parseInt(format.format(now));
	format = new SimpleDateFormat("ss");
	tm_sec = Integer.parseInt(format.format(now));

	// draw hour hand 
	pdflib.PDF_save(p);
	pdflib.PDF_rotate(p, (-((tm_min/60.0f) + tm_hour - 3.0f) * 30.0f));
	pdflib.PDF_moveto(p, -RADIUS/10, -RADIUS/20);
	pdflib.PDF_lineto(p, RADIUS/2, 0.0f);
	pdflib.PDF_lineto(p, -RADIUS/10, RADIUS/20);
	pdflib.PDF_closepath(p);
	pdflib.PDF_fill(p);
	pdflib.PDF_restore(p);

	// draw minute hand
	pdflib.PDF_save(p);
	pdflib.PDF_rotate(p, (-((tm_sec/60.0f) + tm_min - 15.0f) * 6.0f));
	pdflib.PDF_moveto(p, -RADIUS/10, -RADIUS/20);
	pdflib.PDF_lineto(p, RADIUS * 0.8f, 0.0f);
	pdflib.PDF_lineto(p, -RADIUS/10, RADIUS/20);
	pdflib.PDF_closepath(p);
	pdflib.PDF_fill(p);
	pdflib.PDF_restore(p);

	// draw second hand
	pdflib.PDF_setrgbcolor(p, 1.0f, 0.0f, 0.0f);
	pdflib.PDF_setlinewidth(p, 2);
	pdflib.PDF_save(p);
	pdflib.PDF_rotate(p, -((tm_sec - 15.0f) * 6.0f));
	pdflib.PDF_moveto(p, -RADIUS/5, 0.0f);
	pdflib.PDF_lineto(p, RADIUS, 0.0f);
	pdflib.PDF_stroke(p);
	pdflib.PDF_restore(p);

	// draw little circle at center
	pdflib.PDF_circle(p, 0f, 0f, RADIUS/30);
	pdflib.PDF_fill(p);

	pdflib.PDF_restore(p);

	pdflib.PDF_end_page(p);

	pdflib.PDF_close(p);

	pdflib.PDF_delete(p);
    }
}
