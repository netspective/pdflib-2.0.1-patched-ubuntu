public class pdflib {
// pragma

    // The initialization code for loading the PDFlib shared library.
    // The library name will be transformed into something platform-
    // specific by the VM, e.g. libpdf_java2.0.so or pdf_java2.0.dll.
    static {
	try {
	    System.loadLibrary("pdf_java2.01");
	} catch (UnsatisfiedLinkError e) {
	    System.err.println(
	    "Cannot load the PDFlib shared library for Java.\n" +
	    "Make sure that LD_LIBRARY_PATH (Unix) or PATH (Win32)\n" +
	    "contain the current directory\n" + e);
	    System.exit(1);
	}
	PDF_boot();
    }


  public final static native void PDF_boot();
  public final static native void PDF_shutdown();
  public final static native long PDF_new();
  public final static native void PDF_delete(long jarg0);
  public final static native int PDF_open_file(long jarg0, String jarg1);
  public final static native void PDF_close(long jarg0);
  public final static native void PDF_begin_page(long jarg0, float jarg1, float jarg2);
  public final static native void PDF_end_page(long jarg0);
  public final static native void PDF_set_parameter(long jarg0, String jarg1, String jarg2);
  public final static native int PDF_findfont(long jarg0, String jarg1, String jarg2, int jarg3);
  public final static native void PDF_setfont(long jarg0, int jarg1, float jarg2);
  public final static native void PDF_set_font(long jarg0, String jarg1, float jarg2, String jarg3);
  public final static native String PDF_get_fontname(long jarg0);
  public final static native float PDF_get_fontsize(long jarg0);
  public final static native int PDF_get_font(long jarg0);
  public final static native void PDF_show(long jarg0, String jarg1);
  public final static native void PDF_show_xy(long jarg0, String jarg1, float jarg2, float jarg3);
  public final static native void PDF_continue_text(long jarg0, String jarg1);
  public final static native void PDF_set_leading(long jarg0, float jarg1);
  public final static native void PDF_set_text_rise(long jarg0, float jarg1);
  public final static native void PDF_set_horiz_scaling(long jarg0, float jarg1);
  public final static native void PDF_set_text_rendering(long jarg0, int jarg1);
  public final static native void PDF_set_text_matrix(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, float jarg5, float jarg6);
  public final static native void PDF_set_text_pos(long jarg0, float jarg1, float jarg2);
  public final static native void PDF_set_char_spacing(long jarg0, float jarg1);
  public final static native void PDF_set_word_spacing(long jarg0, float jarg1);
  public final static native float PDF_stringwidth(long jarg0, String jarg1, int jarg2, float jarg3);
  public final static native void PDF_setdash(long jarg0, float jarg1, float jarg2);
  public final static native void PDF_setpolydash(long jarg0, float [] jarg1, int jarg2);
  public final static native void PDF_setflat(long jarg0, float jarg1);
  public final static native void PDF_setlinejoin(long jarg0, int jarg1);
  public final static native void PDF_setlinecap(long jarg0, int jarg1);
  public final static native void PDF_setmiterlimit(long jarg0, float jarg1);
  public final static native void PDF_setlinewidth(long jarg0, float jarg1);
  public final static native void PDF_set_fillrule(long jarg0, String jarg1);
  public final static native void PDF_save(long jarg0);
  public final static native void PDF_restore(long jarg0);
  public final static native void PDF_translate(long jarg0, float jarg1, float jarg2);
  public final static native void PDF_scale(long jarg0, float jarg1, float jarg2);
  public final static native void PDF_rotate(long jarg0, float jarg1);
  public final static native void PDF_moveto(long jarg0, float jarg1, float jarg2);
  public final static native void PDF_lineto(long jarg0, float jarg1, float jarg2);
  public final static native void PDF_curveto(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, float jarg5, float jarg6);
  public final static native void PDF_circle(long jarg0, float jarg1, float jarg2, float jarg3);
  public final static native void PDF_arc(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, float jarg5);
  public final static native void PDF_rect(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4);
  public final static native void PDF_closepath(long jarg0);
  public final static native void PDF_stroke(long jarg0);
  public final static native void PDF_closepath_stroke(long jarg0);
  public final static native void PDF_fill(long jarg0);
  public final static native void PDF_fill_stroke(long jarg0);
  public final static native void PDF_closepath_fill_stroke(long jarg0);
  public final static native void PDF_endpath(long jarg0);
  public final static native void PDF_clip(long jarg0);
  public final static native void PDF_setgray_fill(long jarg0, float jarg1);
  public final static native void PDF_setgray_stroke(long jarg0, float jarg1);
  public final static native void PDF_setgray(long jarg0, float jarg1);
  public final static native void PDF_setrgbcolor_fill(long jarg0, float jarg1, float jarg2, float jarg3);
  public final static native void PDF_setrgbcolor_stroke(long jarg0, float jarg1, float jarg2, float jarg3);
  public final static native void PDF_setrgbcolor(long jarg0, float jarg1, float jarg2, float jarg3);
  public final static native int PDF_get_image_width(long jarg0, int jarg1);
  public final static native int PDF_get_image_height(long jarg0, int jarg1);
  public final static native void PDF_place_image(long jarg0, int jarg1, float jarg2, float jarg3, float jarg4);
  public final static native int PDF_open_image(long jarg0, String jarg1, String jarg2, String jarg3, long jarg4, int jarg5, int jarg6, int jarg7, int jarg8, String jarg9);
  public final static native void PDF_close_image(long jarg0, int jarg1);
  public final static native int PDF_open_JPEG(long jarg0, String jarg1);
  public final static native int PDF_open_TIFF(long jarg0, String jarg1);
  public final static native int PDF_open_GIF(long jarg0, String jarg1);
  public final static native int PDF_open_CCITT(long jarg0, String jarg1, int jarg2, int jarg3, int jarg4, int jarg5, int jarg6);
  public final static native int PDF_add_bookmark(long jarg0, String jarg1, int jarg2, int jarg3);
  public final static native void PDF_set_info(long jarg0, String jarg1, String jarg2);
  public final static native void PDF_set_transition(long jarg0, String jarg1);
  public final static native void PDF_set_duration(long jarg0, float jarg1);
  public final static native void PDF_attach_file(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, String jarg5, String jarg6, String jarg7, String jarg8, String jarg9);
  public final static native void PDF_add_note(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, String jarg5, String jarg6, String jarg7, int jarg8);
  public final static native void PDF_add_pdflink(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, String jarg5, int jarg6, String jarg7);
  public final static native void PDF_add_launchlink(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, String jarg5);
  public final static native void PDF_add_locallink(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, int jarg5, String jarg6);
  public final static native void PDF_add_weblink(long jarg0, float jarg1, float jarg2, float jarg3, float jarg4, String jarg5);
  public final static native void PDF_set_border_style(long jarg0, String jarg1, float jarg2);
  public final static native void PDF_set_border_color(long jarg0, float jarg1, float jarg2, float jarg3);
  public final static native void PDF_set_border_dash(long jarg0, float jarg1, float jarg2);
}
