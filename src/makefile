# INKHOKI makefile for LSI C-86 3.30c

LIBR=../libr
CFLAGS=-I$(LIBR)

all : ink.exe

clean :
	erase *.obj
	erase make.i
	erase ink.exe

package :
	lha a inkhoki.lzh makefile *.c *.h *.exe *.txt *.xx

ink.exe : \
  pe.obj pe0.obj pe0a.obj pe0b.obj linebox.obj circle.obj pe2.obj \
  pe3.obj pe4.obj pe6.obj box.obj input.obj files.obj palette.obj \
  output.obj skipdot.obj bitmap.obj plane16.obj stdsave.obj color.obj
	lcc -o $@ @${$#} -ltinymain.obj -lhayper.lib -L$(LIBR)
