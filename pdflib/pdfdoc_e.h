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

/* pdfdoc_e.h
 *
 * PDFDoc encoding vector
 *
 */

static pdf_encodingvector pdf_pdfdoc = {
NULL,		/* 0x00		0 */
NULL,		/* 0x01		1 */
NULL,		/* 0x02		2 */
NULL,		/* 0x03		3 */
NULL,		/* 0x04		4 */
NULL,		/* 0x05		5 */
NULL,		/* 0x06		6 */
NULL,		/* 0x07		7 */
NULL,		/* 0x08		8 */
NULL,		/* 0x09		9 */
NULL,		/* 0x0A		10 */
NULL,		/* 0x0B		11 */
NULL,		/* 0x0C		12 */
NULL,		/* 0x0D		13 */
NULL,		/* 0x0E		14 */
NULL,		/* 0x0F		15 */
NULL,		/* 0x10		16 */
NULL,		/* 0x11		17 */
NULL,		/* 0x12		18 */
NULL,		/* 0x13		19 */
NULL,		/* 0x14		20 */
NULL,		/* 0x15		21 */
NULL,		/* 0x16		22 */
NULL,		/* 0x17		23 */
"breve",	/* 0x18		24 */
"caron",	/* 0x19		25 */
"circumflex",	/* 0x1A		26 */
"dotaccent",	/* 0x1B		27 */
"hungarumlaut",	/* 0x1C		28 */
"ogonek",	/* 0x1D		29 */
"ring",		/* 0x1E		30 */
"tilde",	/* 0x1F		31 */
"space",	/* 0x20		32 */
"exclam",	/* 0x21		33 */
"quotedbl",	/* 0x22		34 */
"numbersign",	/* 0x23		35 */
"dollar",	/* 0x24		36 */
"percent",	/* 0x25		37 */
"ampersand",	/* 0x26		38 */
"quotesingle",	/* 0x27		39 */
"parenleft",	/* 0x28		40 */
"parenright",	/* 0x29		41 */
"asterisk",	/* 0x2A		42 */
"plus",		/* 0x2B		43 */
"comma",	/* 0x2C		44 */
"hyphen",	/* 0x2D		45 */
"period",	/* 0x2E		46 */
"slash",	/* 0x2F		47 */
"zero",		/* 0x30		48 */
"one",		/* 0x31		49 */
"two",		/* 0x32		50 */
"three",	/* 0x33		51 */
"four",		/* 0x34		52 */
"five",		/* 0x35		53 */
"six",		/* 0x36		54 */
"seven",	/* 0x37		55 */
"eight",	/* 0x38		56 */
"nine",		/* 0x39		57 */
"colon",	/* 0x3A		58 */
"semicolon",	/* 0x3B		59 */
"less",		/* 0x3C		60 */
"equal",	/* 0x3D		61 */
"greater",	/* 0x3E		62 */
"question",	/* 0x3F		63 */
"at",		/* 0x40		64 */
"A",		/* 0x41		65 */
"B",		/* 0x42		66 */
"C",		/* 0x43		67 */
"D",		/* 0x44		68 */
"E",		/* 0x45		69 */
"F",		/* 0x46		70 */
"G",		/* 0x47		71 */
"H",		/* 0x48		72 */
"I",		/* 0x49		73 */
"J",		/* 0x4A		74 */
"K",		/* 0x4B		75 */
"L",		/* 0x4C		76 */
"M",		/* 0x4D		77 */
"N",		/* 0x4E		78 */
"O",		/* 0x4F		79 */
"P",		/* 0x50		80 */
"Q",		/* 0x51		81 */
"R",		/* 0x52		82 */
"S",		/* 0x53		83 */
"T",		/* 0x54		84 */
"U",		/* 0x55		85 */
"V",		/* 0x56		86 */
"W",		/* 0x57		87 */
"X",		/* 0x58		88 */
"Y",		/* 0x59		89 */
"Z",		/* 0x5A		90 */
"bracketleft",	/* 0x5B		91 */
"backslash",	/* 0x5C		92 */
"bracketright",	/* 0x5D		93 */
"asciicircum",	/* 0x5E		94 */
"underscore",	/* 0x5F		95 */
"grave",	/* 0x60		96 */
"a",		/* 0x61		97 */
"b",		/* 0x62		98 */
"c",		/* 0x63		99 */
"d",		/* 0x64		100 */
"e",		/* 0x65		101 */
"f",		/* 0x66		102 */
"g",		/* 0x67		103 */
"h",		/* 0x68		104 */
"i",		/* 0x69		105 */
"j",		/* 0x6A		106 */
"k",		/* 0x6B		107 */
"l",		/* 0x6C		108 */
"m",		/* 0x6D		109 */
"n",		/* 0x6E		110 */
"o",		/* 0x6F		111 */
"p",		/* 0x70		112 */
"q",		/* 0x71		113 */
"r",		/* 0x72		114 */
"s",		/* 0x73		115 */
"t",		/* 0x74		116 */
"u",		/* 0x75		117 */
"v",		/* 0x76		118 */
"w",		/* 0x77		119 */
"x",		/* 0x78		120 */
"y",		/* 0x79		121 */
"z",		/* 0x7A		122 */
"braceleft",	/* 0x7B		123 */
"bar",		/* 0x7C		124 */
"braceright",	/* 0x7D		125 */
"asciitilde",	/* 0x7E		126 */
NULL,		/* 0x7F		127 */
"bullet",	/* 0x80		128 */
"dagger",	/* 0x81		129 */
"daggerdbl",	/* 0x82		130 */
"ellipsis",	/* 0x83		131 */
"emdash",	/* 0x84		132 */
"endash",	/* 0x85		133 */
"florin",	/* 0x86		134 */
"fraction",	/* 0x87		135 */
"guilsinglleft",/* 0x88		136 */
"guilsinglright",/* 0x89		137 */
"minus",	/* 0x8A		138 */
"perthousand",	/* 0x8B		139 */
"quotedblbase",	/* 0x8C		140 */
"quotedblleft",	/* 0x8D		141 */
"quotedblright",/* 0x8E		142 */
"quoteleft",	/* 0x8F		143 */
"quoteright",	/* 0x90		144 */
"quotesinglbase",/* 0x91		145 */
"trademark",	/* 0x92		146 */
"fi",		/* 0x93		147 */
"fl",		/* 0x94		148 */
"Lslash",	/* 0x95		149 */
"OE",		/* 0x96		150 */
"Scaron",	/* 0x97		151 */
"Ydieresis",	/* 0x98		152 */
"Zcaron",	/* 0x99		153 */
"dotlessi",	/* 0x9A		154 */
"lslash",	/* 0x9B		155 */
"oe",		/* 0x9C		156 */
"scaron",	/* 0x9D		157 */
"zcaron",	/* 0x9E		158 */
NULL,		/* 0x9F		159 */
"Euro",		/* 0xA0		160 */	/* new in PDF 1.3 */
"exclamdown",	/* 0xA1		161 */
"cent",		/* 0xA2		162 */
"sterling",	/* 0xA3		163 */
"currency",	/* 0xA4		164 */
"yen",		/* 0xA5		165 */
"brokenbar",	/* 0xA6		166 */
"section",	/* 0xA7		167 */
"dieresis",	/* 0xA8		168 */
"copyright",	/* 0xA9		169 */
"ordfeminine",	/* 0xAA		170 */
"guillemotleft",/* 0xAB		171 */
"logicalnot",	/* 0xAC		172 */
NULL,		/* 0xAD		173 */
"registered",	/* 0xAE		174 */
"macron",	/* 0xAF		175 */
"degree",	/* 0xB0		176 */
"plusminus",	/* 0xB1		177 */
"twosuperior",	/* 0xB2		178 */
"threesuperior",/* 0xB3		179 */
"acute",	/* 0xB4		180 */
"mu",		/* 0xB5		181 */
"paragraph",	/* 0xB6		182 */
"periodcentered",/* 0xB7		183 */
"cedilla",	/* 0xB8		184 */
"onesuperior",	/* 0xB9		185 */
"ordmasculine",	/* 0xBA		186 */
"guillemotright",/* 0xBB		187 */
"onequarter",	/* 0xBC		188 */
"onehalf",	/* 0xBD		189 */
"threequarters",/* 0xBE		190 */
"questiondown",	/* 0xBF		191 */
"Agrave",	/* 0xC0		192 */
"Aacute",	/* 0xC1		193 */
"Acircumflex",	/* 0xC2		194 */
"Atilde",	/* 0xC3		195 */
"Adieresis",	/* 0xC4		196 */
"Aring",	/* 0xC5		197 */
"AE",		/* 0xC6		198 */
"Ccedilla",	/* 0xC7		199 */
"Egrave",	/* 0xC8		200 */
"Eacute",	/* 0xC9		201 */
"Ecircumflex",	/* 0xCA		202 */
"Edieresis",	/* 0xCB		203 */
"Igrave",	/* 0xCC		204 */
"Iacute",	/* 0xCD		205 */
"Icircumflex",	/* 0xCE		206 */
"Idieresis",	/* 0xCF		207 */
"Eth",		/* 0xD0		208 */
"Ntilde",	/* 0xD1		209 */
"Ograve",	/* 0xD2		210 */
"Oacute",	/* 0xD3		211 */
"Ocircumflex",	/* 0xD4		212 */
"Otilde",	/* 0xD5		213 */
"Odieresis",	/* 0xD6		214 */
"multiply",	/* 0xD7		215 */
"Oslash",	/* 0xD8		216 */
"Ugrave",	/* 0xD9		217 */
"Uacute",	/* 0xDA		218 */
"Ucircumflex",	/* 0xDB		219 */
"Udieresis",	/* 0xDC		220 */
"Yacute",	/* 0xDD		221 */
"Thorn",	/* 0xDE		222 */
"germandbls",	/* 0xDF		223 */
"agrave",	/* 0xE0		224 */
"aacute",	/* 0xE1		225 */
"acircumflex",	/* 0xE2		226 */
"atilde",	/* 0xE3		227 */
"adieresis",	/* 0xE4		228 */
"aring",	/* 0xE5		229 */
"ae",		/* 0xE6		230 */
"ccedilla",	/* 0xE7		231 */
"egrave",	/* 0xE8		232 */
"eacute",	/* 0xE9		233 */
"ecircumflex",	/* 0xEA		234 */
"edieresis",	/* 0xEB		235 */
"igrave",	/* 0xEC		236 */
"iacute",	/* 0xED		237 */
"icircumflex",	/* 0xEE		238 */
"idieresis",	/* 0xEF		239 */
"eth",		/* 0xF0		240 */
"ntilde",	/* 0xF1		241 */
"ograve",	/* 0xF2		242 */
"oacute",	/* 0xF3		243 */
"ocircumflex",	/* 0xF4		244 */
"otilde",	/* 0xF5		245 */
"odieresis",	/* 0xF6		246 */
"divide",	/* 0xF7		247 */
"oslash",	/* 0xF8		248 */
"ugrave",	/* 0xF9		249 */
"uacute",	/* 0xFA		250 */
"ucircumflex",	/* 0xFB		251 */
"udieresis",	/* 0xFC		252 */
"yacute",	/* 0xFD		253 */
"thorn",	/* 0xFE		254 */
"ydieresis"	/* 0xFF		255 */
};
