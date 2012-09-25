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

/* p_util.c
 *
 * PDFlib utility routines
 *
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "p_intern.h"

/* Format floating point numbers in a PDF compatible way.
 * This must be used for all floating output since PDF doesn't
 * allow %g exponential format, and %f produces too many characters
 * in most cases. PDF spec says: "use four or five decimal places".
 */

/* Acrobat viewers change absolute values < 1/65536 to zero */
#define SMALLREAL	0.000015

/* Acrobat viewers have an upper limit on real numbers */
#define BIGREAL		32767

const char *
pdf_float(char *buf, float f)
{
    if (fabs(f) < SMALLREAL)
	return "0";			/* force very small numbers to zero   */

    sprintf(buf, "%.4g", f);		/* try %g first and then check output */

    if (strchr(buf, 'e')) {		/* this format is not PDF compatible  */
	if (fabs(f) < 1)
	    sprintf(buf, "%1.5f", f);	/* 5 decimal places for small numbers */
	else if (fabs(f) <= BIGREAL)
	    sprintf(buf, "%1.2f", f);	/* 2 decimal places for medium numbers*/
	else
	    sprintf(buf, "%1.0f", f);	/* skip decimal places for big numbers*/
    }

    return buf;
}

/* Unicode-related stuff */

/* strlen() for unicode strings, which are terminated by two zero bytes.
 * wstrlen() returns the number of bytes in the Unicode string,
 * not including the two terminating null bytes.
 */
static size_t
wstrlen(const char *s)
{
    size_t len;

    for(len = 0; 
	(unsigned char) (s[len++]) != 0 ||
	(unsigned char) (s[len++]) != 0; /* */ ) {
	/* */
    }

    return len-2;
}

/* This function returns the length in bytes for C and Unicode strings.
 * Note that unlike strlen() it returns the length _including_ the 
 * terminator, which may be one or two null bytes.
 */
size_t
pdf_strlen(const char *text)
{
    if (pdf_is_unicode(text))
	return wstrlen(text) + 2;
    else
	return strlen(text) + 1;
}

/* Allocate a PDFlib-local buffer and copy the string including
 * the terminating sentinel. If the string starts with the Unicode BOM
 * it is considered a Unicode string, and must be terminated by
 * two null bytes. Otherwise it is considered a plain C string and
 * must be terminated by a single null byte.
 * The caller is responsible for freeing the buffer.
 */
char *
pdf_strdup(PDF *p, const char *text)
{
    char *buf;
    size_t len;

    if (text == NULL)
	pdf_error(p, PDF_SystemError, "NULL string in pdf_strdup");
	
    if (pdf_is_unicode(text))
	len = wstrlen(text) + 2;
    else
	len = strlen(text) + 1;

    buf = (char *) p->malloc(p, len, "pdf_strdup");
    memcpy(buf, text, len);

    return buf;
}

/* ----------------- Resource handling ----------------------- */
char *
pdf_find_resource(PDF *p, const char *category, const char *resourcename)
{
    pdf_category *cat;
    pdf_res *res;
    
    /* Resources may have been added individually with PDF_set_parameter().
     * If so, we search them; if not, we have to load the resource file.
     */
    if (p->resources == NULL && !p->resourcefile_loaded)
	pdf_init_resources(p);

    for (cat = p->resources; cat != (pdf_category *) NULL; cat = cat->next) {
	if (!strcmp(cat->category, category)) {
	    for (res = cat->kids; res != (pdf_res *)NULL; res=res->next) {
		if (!strcmp(res->name, resourcename))
		    return res->filename;
	    }
	    if (p->resourcefile_loaded)	/* all hope is lost */
		return NULL;
	}
    }

    /* Up to now we may have searched the individually loaded resources,
     * but not the resource file. Since we haven't found the resource,
     * we now (very lazily) load the resource file and try again.
     */
    if (!p->resourcefile_loaded) {
	pdf_init_resources(p);
	return pdf_find_resource(p, category, resourcename);
    }

    pdf_error(p, PDF_SystemError, "Resource category '%s' not found", category);
    
    /* Satisfy the compiler */
    return NULL;
}

/* The UPR file handling routines are based on the publicly available
 * utility makepsres from Adobe Systems.
 */
static void
StripComments(char *buf)
{
    register char *ch = buf;

    while (pdf_true) {
	while (*ch != '%' && *ch != '\0')
	    ch++;
	if (*ch == '\0')
	    break;
	if (ch == buf || *(ch-1) != '\\') {
	    *ch = '\0';
	    break;
	}
	ch++;
    }

    /* ch points to '\0' right now */

    if (ch == buf)
	return;
    ch--;

    while (ch > buf && (*ch == ' ' || *ch == '\t' || *ch == '\n')) {
	*ch = '\0';
	ch--;
    }

    if (ch == buf && (*ch == ' ' || *ch == '\t' || *ch == '\n'))
	*ch = '\0';
}

/* Caller must free returned line */

#define BUFSIZE 1024

static char *
GetWholeLine(PDF *p, FILE *fp)
{
    char *line;
    char buf[BUFSIZE];
    size_t len, oldlen;

    while (pdf_true) {
	if (fgets(buf, BUFSIZE, fp) == NULL)
	    return NULL;
	StripComments(buf);	/* Strip comments and blank lines */
	if (buf[0] != '\0')
	    break;
    }

    StripComments(buf);
    
    len = strlen(buf); 
    line = (char *) p->malloc(p, len+1, "GetWholeLine in UPR parser");
    strcpy(line, buf);

    if (line[len-1] == '\\') {	/* Continued... */
	line[len-1] = '\0';
	oldlen = len-1;
	while (pdf_true) {
	    if (fgets(buf, BUFSIZE, fp) == NULL)
		return line;

	    StripComments(buf);
	    if (buf[0] == '\0') return line;

	    len = strlen(buf);
	    line = (char *) p->realloc(p, line, oldlen+len+1,
			     "GetWholeLine in UPR parser");
	    strcat(line, buf);

	    oldlen += len;
	    if (line[oldlen-1] != '\\')
		break;
	    line[oldlen-1] = '\0';
	    oldlen--;
	}
    }
    return line;
}

void
pdf_add_resource(PDF *p, const char *category, const char *resource,
		const char *filename, const char *prefix)
{
    pdf_category *cat, *lastcat = NULL;
    pdf_res *res, *lastres = NULL;

    /* All other (unknown) categories raise an error */
    if (strcmp("FontOutline", category) &&
	strcmp("FontAFM", category) &&
	strcmp("FontTT", category) &&
	strcmp("FontPFM", category)) {
	    pdf_error(p, PDF_ValueError, "Unknown resource category");
	    return;
    }

    /* make sure the resource file is loaded before adding user-supplied
     * resources.
     */
    /* Nope: Evgeny wants to dynamically add resources without ever
     * installing a static resource configuration file...
     */
    /*
    pdf_init_resources(p);
    */

    /* find start of this category's resource list, if the category exists */
    for (cat = p->resources; cat != (pdf_category *) NULL; cat = cat->next) {
	lastcat = cat;
	if (!strcmp(cat->category, category))
	    break;
    }

    if (cat == NULL) {
	cat = (pdf_category *) p->malloc(p, 
			sizeof(pdf_category), "pdf_add_resource");
	cat->category = pdf_strdup(p, category);
	cat->kids = NULL;
	cat->next = NULL;

	if (lastcat) {
	    lastcat->next = cat;
	}else {
	    p->resources = cat;
	}
    }

    if (prefix == NULL) {
	prefix = "";
	if (filename[0] == '.' && filename[1] == '/') filename+=2;
    } else {
	prefix++; /* Skip over leading / */
	if (prefix[0] == '.' && prefix[1] == '/') prefix += 2;
    }

    /* Go to the end of the resource list */
    for (res = cat->kids; res != (pdf_res *) NULL; res = res->next)
	lastres = res;

    res = (pdf_res *) p->malloc(p, sizeof(pdf_res), "pdf_add_resource");

    if (lastres)
	lastres->next = res;
    else
	cat->kids = res;

    res->next = NULL;
    res->name = pdf_strdup(p, resource);
    res->filename = (char *) p->malloc(p, strlen(filename) + strlen(prefix)+2,
		"pdf_add_resource");

    if (prefix != NULL && prefix[0] != '\0') {
	strcpy (res->filename, prefix);
	strcat (res->filename, PATHSEP);
	strcat (res->filename, filename);
    } else
	strcpy (res->filename, filename);
}

void
pdf_cleanup_resources(PDF *p)
{
    pdf_category *cat, *lastcat;
    pdf_res *res, *lastres;

    for (cat = p->resources; cat != (pdf_category *) NULL; /* */) {
	for (res = cat->kids; res != (pdf_res *) NULL; /* */) {
	    lastres = res;
	    res = lastres->next;
	    p->free(p, lastres->name);
	    p->free(p, lastres->filename);
	    p->free(p, lastres);
	}
	lastcat = cat;
	cat = lastcat->next;
	p->free(p, lastcat->category);
	p->free(p, lastcat);
    }
}

void
pdf_init_resources(PDF *p)
{
    char	*uprfilename = NULL;
    char	*resourceFile;
    FILE	*fp;
    char	buf[BUFSIZE];
    char	*line;
    char	*category;
    char	*prefix = NULL;

    if (p->resourcefile_loaded)
	return;
    else
	p->resourcefile_loaded = pdf_true;

#ifndef MAC
    uprfilename = getenv(RESOURCEFILE);		/* user-supplied res file */
#endif
    if (uprfilename == NULL)
	uprfilename = p->resourcefilename;	/* client-supplied res file */

    if (uprfilename == NULL)			/* default resource file */
	uprfilename = pdf_strdup(p, DEFAULTRESOURCEFILE);

    if ((fp = fopen(uprfilename, "r")) == NULL)
	pdf_error(p, PDF_IOError, "Resource configuration file '%s' not found",
		uprfilename);

    while (pdf_true) {		/* Skip over list of categories */
	if (fgets(buf, BUFSIZE, fp) == NULL) {
	    fclose(fp);
	    return;
	}
	if (buf[0] == '.')
	    break;
    }

    while (pdf_true) {	/* process categories */
	line = GetWholeLine(p, fp);

	if (line == NULL)
	    break;
      
	if (line[0] == '/') {		/* Handle optional directory prefix */
	    prefix = line;
	    continue;
	}

	category = line;

	while (pdf_true) {	/* process lines for this category */

	    line = GetWholeLine(p, fp);
	    if (line == NULL) {
		if (prefix != NULL)
		    p->free(p, prefix);
		p->free(p, category);
		fclose(fp);
		return;
	    }

	    if (line[0] == '.') {
		p->free(p, category);
		p->free(p, line);
		break;
	    }

	    resourceFile = line;
	    while (pdf_true) {
		if ((resourceFile = strchr(resourceFile, '=')) != NULL) {
		    if (resourceFile != line && *(resourceFile-1) != '\\') {
			*resourceFile++ = '\0';

			if (*resourceFile == '=') {
			    resourceFile++;		/* skip '=' character */
			    pdf_add_resource(p, category, line, 
				    resourceFile, NULL);
			}else {
			    pdf_add_resource(p, category, line, 
				    resourceFile, prefix);
			}

			break;
		    }
		    resourceFile++;
		} else
		    break;		/* bogus resource line */
	    }

	    p->free(p, line);
	}	/* process lines for this category */
    }		/* process categories */

    if (prefix != NULL)
	p->free(p, prefix);
    fclose(fp);
}
