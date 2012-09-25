/*---------------------------------------------------------------------------*
 |        PDFlib - A library for dynamically generating PDF files            |
 +---------------------------------------------------------------------------+
 |        Copyright (c) 1997-1999 Thomas Merz. All rights reserved.          |
 +---------------------------------------------------------------------------+
 |    This software is not in the public domain.  It is subject to the       |
 |    "Aladdin Free Public License".  See the file license.txt for details.  |
 |    This license grants you the right to use and redistribute PDFlib       |
 |    under certain conditions. Among other things, the license requires     |
 |    that the copyright notice and this notice be preserved on all copies.  |
 |    This requirement extends to ports to other programming languages.      |
 |                                                                           |
 |    In short, you are allowed to develop and use PDFlib-based software     |
 |    as long as you don't sell it. Commercial use of PDFlib requires a      |
 |    commercial license which can be obtained from the author of PDFlib.    |
 |    Contact information can be found in the accompanying PDFlib manual.    |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* p_afmparse.h
 *
 * Header file for the PDFlib AFM parsing routines
 *
 * This source is based on Adobe's AFM parser which is available
 * via the Adobe Developers Association. It has been adapted
 * to PDFlib programming conventions, some bugs fixed, and
 * improved in several ways.
 * The original file had the following notice:
 */
/*
 * Copyright (C) 1988, 1989, 1991 by Adobe Systems Incorporated. 
 * All rights reserved.
 */

#ifndef P_AFMPARSE_H
#define P_AFMPARSE_H

/* Flags that can be AND'ed together to specify exactly what
 * information from the AFM file should be saved.
 */
#define P_G	0x01	/* 0000 0001 */   /* Global Font Info      */
#define P_W	0x02	/* 0000 0010 */   /* Character Widths ONLY */
#define P_M	0x06	/* 0000 0110 */   /* All Char Metric Info  */
#define P_P	0x08	/* 0000 1000 */   /* Pair Kerning Info     */
#define P_T	0x10	/* 0001 0000 */   /* Track Kerning Info    */
#define P_C	0x20	/* 0010 0000 */   /* Composite Char Info   */

/* Possible return codes from the parseFile procedure. */
#define ok		0
#define parseError	-1
#define earlyEOF	-2

/* Ligature definition is a linked list since any character can have
 * any number of ligatures.
 */
typedef struct _t_ligature {
    char *succ, *lig;
    struct _t_ligature *next;
} Ligature;

/* Character Metric Information. This structure is used only if ALL 
 * character metric information is requested. If only the character
 * widths is requested, then only an array of the character x-widths
 * is returned.
 */
typedef struct {
    int code, 		/* key: C */
        wx,		/* key: WX */
        wy;		/* together wx and wy are associated with key: W */
    char *name; 	/* key: N */
    PDF_rectangle charBBox;	/* key: B */
    Ligature *ligs;	/* key: L (linked list; not a fixed number of Ls */
} CharMetricInfo;

/* Track kerning data structure.  */
typedef struct {
    int degree;  
    float minPtSize, 
          minKernAmt, 
          maxPtSize, 
          maxKernAmt;
} TrackKernData;

/* Pair Kerning data structure. */
typedef struct {
    char *name1;
    char *name2;
    int xamt,
        yamt;
} PairKernData;

/* PCC is a piece of a composite character. This is a sub structure of a
 * compCharData described below.
 */
typedef struct {
    char *pccName;
    int deltax,
        deltay;
} Pcc;

/* Composite Character Information data structure. 
 * The fields ccName and numOfPieces are filled with the values associated
 * with the key CC. The field pieces points to an array (size = numOfPieces)
 * of information about each of the parts of the composite character. That
 * array is filled in with the values from the key PCC.
 */
typedef struct {
    char *ccName;
    int numOfPieces;
    Pcc *pieces;
} CompCharData;

/* The core PDFlib font structure */
struct pdf_font_s {
    char	*name;			/* PostScript name of the font */
    pdf_encoding encoding;		/* font encoding */
    int		used_on_current_page;	/* this font is in use on current p. */
    int		embed;			/* whether or not to embed this font */
    char	*fontfilename;		/* name of external font file */
    id		obj_id;			/* object id of this font */

    unsigned long flags;		/* font flags for font descriptor */
    pdf_bool	afm;			/* this font parsed from AFM file */

    char	*fullName;		/* AFM key: FullName */
    char	*encodingScheme;	/* AFM key: EncodingScheme */
    char	*familyName;		/* AFM key: FamilyName */
    char	*weight;		/* AFM key: Weight */
    float	italicAngle;		/* AFM key: ItalicAngle */
    int		isFixedPitch;		/* AFM key: IsFixedPitch */
    int		llx;			/* AFM key: FontBBox */
    int		lly;			/* AFM key: FontBBox */
    int		urx;			/* AFM key: FontBBox */
    int		ury;			/* AFM key: FontBBox */
    int		underlinePosition;  	/* AFM key: UnderlinePosition */
    int		underlineThickness; 	/* AFM key: UnderlineThickness */
    int		capHeight;		/* AFM key: CapHeight */
    int		xHeight;		/* AFM key: XHeight */
    int		ascender;		/* AFM key: Ascender */
    int		descender;		/* AFM key: Descender */
    int		StdVW;			/* AFM key: StdVW */
    int		StdHW;			/* AFM key: StdHW */

    int			numOfChars;	/* # of entries in char metrics */
    CharMetricInfo	*cmi;		/* ptr to char metrics array */
    int 		numOfTracks;	/* # of entries in track kerning array*/
    TrackKernData	*tkd;		/* ptr to track kerning array */
    int			numOfPairs;	/* # of entries in pair kerning array */
    PairKernData	*pkd;		/* ptr to pair kerning array */
    int			numOfComps;	/* # of entries in comp char array */
    CompCharData	*ccd;		/* ptr to comp char array */

    int        		widths[256];	/* From AFM char metrics and encoding */
};

#endif	/* P_AFMPARSE_H */
