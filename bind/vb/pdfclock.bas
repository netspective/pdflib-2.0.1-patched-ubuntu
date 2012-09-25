Attribute VB_Name = "pdfclock"
' pdfclock.bas
' Requires the PDFlib type library
' Load pdflib_vb.tlb via Project, References, Browse

Option Explicit

Sub main()
    Dim p As Long
    Dim err, font As Integer
    Dim alpha, tm_hour, tm_min, tm_sec As Double
    Const RADIUS! = 200
    Const MARGIN! = 20
    
    p = PDF_new
    
    ' Open new PDF file
    err = PDF_open_file(p, "pdfclock_vb.pdf")
    If (err = -1) Then
        MsgBox "Couldn't open PDF file!"
        End
    End If
    
    PDF_set_info p, "Creator", "pdfclock.bas"
    PDF_set_info p, "Author", "Thomas Merz"
    PDF_set_info p, "Title", "PDF clock (Visual Basic)"

    ' start a new page
    PDF_begin_page p, 2 * (RADIUS + MARGIN), 2 * (RADIUS + MARGIN)
    PDF_translate p, RADIUS + MARGIN, RADIUS + MARGIN
    PDF_setrgbcolor p, 0, 0, 1
    PDF_save (p)

    ' minute strokes
    PDF_setlinewidth p, 2
    
    For alpha = 0 To 354 Step 6
        PDF_rotate p, 6
        PDF_moveto p, RADIUS, 0
        PDF_lineto p, RADIUS - MARGIN / 3, 0
        PDF_stroke p
    Next alpha

    PDF_restore p
    PDF_save p
    
    ' 5 minute strokes
    PDF_setlinewidth p, 3
    For alpha = 0 To 330 Step 30
        PDF_rotate p, 30
        PDF_moveto p, RADIUS, 0
        PDF_lineto p, RADIUS - MARGIN, 0
        PDF_stroke p
    Next alpha

    tm_hour = Hour(Now)
    tm_min = Minute(Now)
    tm_sec = Second(Now)

    ' draw hour hand
    PDF_save p
    PDF_rotate p, -((tm_min / 60) + tm_hour - 3) * 30
    PDF_moveto p, -RADIUS / 10, -RADIUS / 20
    PDF_lineto p, RADIUS / 2, 0#
    PDF_lineto p, -RADIUS / 10, RADIUS / 20
    PDF_closepath p
    PDF_fill p
    PDF_restore p

    ' draw minute hand
    PDF_save p
    PDF_rotate p, -((tm_sec / 60) + tm_min - 15) * 6
    PDF_moveto p, -RADIUS / 10, -RADIUS / 20
    PDF_lineto p, RADIUS * 0.8, 0
    PDF_lineto p, -RADIUS / 10, RADIUS / 20
    PDF_closepath p
    PDF_fill p
    PDF_restore p

    ' draw second hand
    PDF_setrgbcolor p, 1, 0, 0
    PDF_setlinewidth p, 2
    PDF_save p
    PDF_rotate p, -(tm_sec - 15) * 6
    PDF_moveto p, -RADIUS / 5, 0
    PDF_lineto p, RADIUS, 0
    PDF_stroke p
    PDF_restore p

    ' draw little circle at center
    PDF_circle p, 0, 0, RADIUS / 30
    PDF_fill p
            
    PDF_restore p
    PDF_end_page p		' finish page
    
    PDF_close p			' close PDF document
    PDF_delete p
End Sub
