#!/bin/sh
TMP=/var/tmp/pdfclock.$$.pdf

echo Content-type: application/pdf
echo

pdfclock -o $TMP
cat $TMP
rm $TMP
exit 0
