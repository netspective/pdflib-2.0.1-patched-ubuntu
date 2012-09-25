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

/* p_filter.c
 *
 * ASCII85 and Hex encoding for PDFlib 
 *
 */

#include <stdio.h>

#include "p_intern.h"

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#ifdef UNUSED
/* output one ASCII byte and keep track of characters per line */
static void 
pdf_outbyte(PDF *p, byte c)
{
    if (fputc(c, p->fp) == EOF)
	pdf_error(p, PDF_IOError, "Write error in pdf_outbyte()");

#define MAX_CHARS_PER_LINE	64
    /* insert line feed */
    if (++(p->chars_on_this_line) == MAX_CHARS_PER_LINE)
    {
	fputc('\n', p->fp);
	p->chars_on_this_line = 0;
    }
}

static const unsigned long power85[5] = 
	{ 1L, 85L, 85L*85, 85L*85*85, 85L*85*85*85};

void
pdf_ASCII85Encode(PDF *p, PDF_data_source *src)
{
    unsigned long word, v;
    int i, fetched;
    byte buf[4];

    src->init(p, src);
    if (!src->fill(p, src)) {
	pdf_error(p, PDF_IOError, "Data underrun in pdf_ASCII85Encode");
	fputs("~>\n", p->fp);			/* EOD marker */
	return;
    }

    p->chars_on_this_line = 0;

    while (1)
    {
	for (fetched = 0; fetched < 4; fetched++)
	{
	    if (src->bytes_available == 0 && !src->fill(p, src))
		break;
	    buf[fetched] = *(src->next_byte);
	    src->next_byte++;
	    src->bytes_available--;
	}
	if (fetched < 4)
	    break;

	/* 4 bytes available ==> output 5 bytes */
	word = ((unsigned long)(((unsigned int)buf[0] << 8) + buf[1]) << 16) +
	       (((unsigned int)buf[2] << 8) + buf[3]);
	if (word == 0)
	    pdf_outbyte(p, (byte) 'z');       /* shortcut for 0 */
	else
	{
	    /* calculate 5 ASCII85 bytes and output them */
	    for (i = 4; i >= 0; i--) {
		v = word / power85[i];
		pdf_outbyte(p, (byte) (v + '!'));
		word -= v * power85[i];
	    }
	}
    }

    word = 0;

    /* 0-3 bytes left */
    if (fetched != 0)
    {
	for (i = fetched - 1; i >= 0; i--)   /* accumulate bytes */
	    word += (unsigned long)buf[i] << 8 * (3-i);

	/* encoding as above, but output only fetched+1 bytes */
	for (i = 4; i >= 4-fetched; i--)
	{
	    v = word / power85[i];
	    pdf_outbyte(p, (byte)(v + '!'));
	    word -= v * power85[i];
	}
    }

    src->terminate(p, src);

    fputs("~>\n", p->fp);			/* EOD marker */
}
#endif

/* We only generate ASCII output for debugging purposes */
void 
pdf_ASCIIHexEncode(PDF *p, PDF_data_source *src)
{
    static const char BinToHex[] = "0123456789ABCDEF";
    int CharsPerLine;
    size_t i;
    byte *data;

    CharsPerLine = 0;

    src->init(p, src);

    while (src->fill(p, src))
    {
	
	for (data=src->next_byte, i=src->bytes_available; i > 0; i--, data++)
	{
	  fputc((int) BinToHex[*data>>4], p->fp);           /* first nibble  */
	  fputc((int) BinToHex[*data & 0x0F], p->fp);       /* second nibble */
	  if ((CharsPerLine += 2) >= 64) {
	    putc('\n', p->fp);
	    CharsPerLine = 0;
	  }
	}
    }

    src->terminate(p, src);
    (void) fputs(">\n", p->fp);         /* EOD marker for PDF hex strings */
}

/* methods for constructing a data source from a file */

#define FILE_BUFSIZE 1024

void
pdf_data_source_file_init(PDF *p, PDF_data_source *src)
{
  FILE		*fp;

  src->buffer_length = FILE_BUFSIZE;
  src->buffer_start = (byte *)
  	p->malloc(p, src->buffer_length, "PDF_data_source_file_init");

  fp = fopen((char *) src->private_data, READMODE);

  if (fp == NULL)
    pdf_error(p, PDF_IOError,
    	"Couldn't open embedded file '%s'!", src->private_data);

  src->private_data = (void *) fp;
}

int
pdf_data_source_file_fill(PDF *p, PDF_data_source *src)
{
  src->next_byte = src->buffer_start;
  src->bytes_available = 
  	fread(src->buffer_start, 1, FILE_BUFSIZE, (FILE *) (src->private_data));

    if (src->bytes_available == 0)
	return pdf_false;
    else
	return pdf_true;
}

void
pdf_data_source_file_terminate(PDF *p, PDF_data_source *src)
{
  p->free(p, (void *) src->buffer_start);
  fclose((FILE *) src->private_data);
}

/* copy the complete contents of src to the output */
void
pdf_copy(PDF *p, PDF_data_source *src)
{
    src->init(p, src);

    while (src->fill(p, src))
	(void) fwrite(src->next_byte, 1, src->bytes_available, p->fp);
    
    src->terminate(p, src);
}

#ifdef HAVE_LIBZ
/* zlib compression */

/* wrapper for p->malloc for use in zlib */
static voidpf
pdf_zlib_alloc(voidpf p, uInt items, uInt size)
{
    return (voidpf) ((PDF *)p)->malloc(p, items *size, "zlib");
}

#define COMPR_BUFLEN 1024		/* size of compression buffer */

void
pdf_compress(PDF *p, PDF_data_source *src)
{
    z_stream z;		/* zlib compression stream */
    int status;
    size_t count;
    char	compression_buffer[COMPR_BUFLEN];

    z.zalloc = (alloc_func) pdf_zlib_alloc;
    z.zfree = (free_func) p->free;
    z.opaque = (voidpf) p;

    if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK)
	pdf_error(p, PDF_SystemError, "Compression initialization error!");

    src->init(p, src);

    z.avail_in = 0;
    z.next_out = (Bytef *) compression_buffer;
    z.avail_out = COMPR_BUFLEN;

    for ( ; ; ) {
	if (z.avail_in == 0) {
	    if(!src->fill(p, src))
		break;
	    z.next_in  = src->next_byte;
	    z.avail_in = src->bytes_available;
	}

	if (deflate(&z, Z_NO_FLUSH) != Z_OK)
	    pdf_error(p, PDF_SystemError, "Compression error!");

	count = COMPR_BUFLEN - z.avail_out;
	if (count != 0) {
	    (void) fwrite(compression_buffer, 1, count, p->fp);
	    z.next_out = (Bytef *) compression_buffer;
	    z.avail_out = COMPR_BUFLEN;
	}
    }
    /* Finish the stream */
    for ( ; ;) {
	z.next_out = (Bytef *) compression_buffer;
	z.avail_out = COMPR_BUFLEN;

	status = deflate(&z, Z_FINISH);

	count = COMPR_BUFLEN - z.avail_out;
	if (count != 0)
	    (void) fwrite(compression_buffer, 1, count, p->fp);
	if (status == Z_STREAM_END) break;
	if (status != Z_OK)
	    pdf_error(p, PDF_SystemError, "Compression error (Z_FINISH)!");
    }

    if (deflateEnd(&z) != Z_OK)
	pdf_error(p, PDF_SystemError, "Compression error (deflateEnd)!");

    src->terminate(p, src);
}

#else	/* not HAVE_LIBZ */

void
pdf_compress(PDF *p, PDF_data_source *src)
{
    /* simply relay to the copy function if we can't do real compression */
    pdf_copy(p, src);
}
#endif	/* not HAVE_LIBZ */
