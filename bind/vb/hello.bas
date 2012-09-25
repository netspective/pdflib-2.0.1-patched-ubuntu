Attribute VB_Name = "Module1"
'
' hello.bas
' Copyright (C) 1997-99 Thomas Merz. All rights reserved.
'
' PDFlib client: hello example in Visual Basic
' Requires the PDFlib type library
' Load pdflib_vb.tlb via Project, References, Browse

Option Explicit


Sub main()
    Dim p As Long
    Dim err, font As Integer
    
    p = PDF_new
    
    ' Open new PDF file
    err = PDF_open_file(p, "hello_vb.pdf")
    If (err = -1) Then
        MsgBox "Couldn't open PDF file!"
        End
    End If
    
    PDF_set_info p, "Creator", "hello.bas"
    PDF_set_info p, "Author", "Thomas Merz"
    PDF_set_info p, "Title", "Hello, world (Visual Basic)!"

    ' start a new page
    PDF_begin_page p, 595, 842

    font = PDF_findfont(p, "Helvetica-Bold", "winansi", 0)
    If (font = -1) Then
        MsgBox "Couldn't set font"
        End
    End If
        
    PDF_setfont p, font, 24

    PDF_set_text_pos p, 50, 700
    PDF_show p, "Hello, world!"
    PDF_continue_text p, "(says Visual Basic)"
            
    PDF_end_page p		' finish page
    
    PDF_close p			' close PDF document

    PDF_delete p
End Sub
