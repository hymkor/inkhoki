.SUFFIXES: .obj .a86 .p86 .c .y .l .lib

.l.lib:
	libr -o $@ $<

all : manual.doc hayper.lib

clear :
	erase _*.lib
	erase *.obj
	erase make.i

manual.doc : manual.xx
	xtr -e $# >$@

hayper.lib : \
	_sprite.lib _vramoff.lib _beta.lib _mice.lib _graph.lib \
	_keyb.lib _beep.lib _ems.lib _others.lib _garbage.lib \
	_console.lib
	if exist $@ del $@
	oar qc $@ @${$#}

_graph.lib	: graph.l
	libr -o $@ $<
_sprite.lib	: sprite.l
	libr -o $@ $<
_vramoff.lib	: vramoff.l
	libr -o $@ $<
_beta.lib	: beta.l
	libr -o $@ $<
_bmptobg.obj	: bmptobg.c
	libr -o $@ $<
_keyb.lib	: scnkey.l
	libr -o $@ $<
_mice.lib	: mice.l
	libr -o $@ $<
_beep.lib	: beep.l
	libr -o $@ $<
_ems.lib	: ems.l
	libr -o $@ $<
_garbage.lib	: garbages.l
	libr -o $@ $<
_console.lib	: console.l
	libr -o $@ $<
_others.lib	: gputat.obj _bmptobg.obj vramcopy.obj
	oar ruc $@ $#
