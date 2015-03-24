: Makefile.SH generated from Jmake.tmpl and Jmakefile [jmake 3.0 PL58]
: $X-Id: Jmake.tmpl,v 1.2 1995/09/25 12:41:11 pfeifer Exp $

case $CONFIG in
'')
	if test -f config.sh; then TOP=.;
	elif test -f ../config.sh; then TOP=..;
	elif test -f ../../config.sh; then TOP=../..;
	elif test -f ../../../config.sh; then TOP=../../..;
	elif test -f ../../../../config.sh; then TOP=../../../..;
	else
		echo "Can't find config.sh."; exit 1
	fi
	. $TOP/config.sh
	;;
esac
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac
CURRENT=ui
DIR=`echo $CURRENT/ | sed -e 's/\.\///g'`
echo "Extracting ${DIR}Makefile (with variable substitutions)"

INSTALL=`echo $install | sed -e 's,\./i,\$(TOP)/i,'`
INSTALLDIR=`echo $installdir | sed -e 's,\./i,\$(TOP)/i,'`
DATE=`date`

$spitshell >Makefile <<!GROK!THIS!
########################################################################
# Makefile generated from Makefile.SH on $DATE

SHELL = /bin/sh
JMAKE = jmake
TOP = ..
CURRENT = $CURRENT
DIR = $DIR
INSTALL = $INSTALL
INSTALLDIR = $INSTALLDIR

########################################################################
# Parameters set by Configure -- edit config.sh if changes are needed

CC = $cc
CPP = $cpp $cppminus $cppflags
CTAGS = ctags
JCFLAGS = \$(CFLAGS) $optimize $ccflags $large
JLDFLAGS = \$(LDFLAGS) $optimize $ldflags
LIBS = $libs
LN = $ln
MAKE = make
MKDEP = $mkdep \$(DPFLAGS) --
MV = $mv
PRIVLIB = $installprivlib
RM = $rm -f
SED = $sed

########################################################################
# Automatically generated parameters -- do not edit

CFLAGS = $ctypeinc -I\$(TOP)/ir
LDFLAGS = -lcurses -ltermcap -lm
ISOOBJS = $ctypeobj
REGLIB = $regexplib
BIN_EXP = $installbin
USRINC = $usrinc
SOURCES =   waisq.c  waissearch.c  swais.c
OBJECTS =   waisq.o \$(OBJS)  waissearch.o \$(ISOOBJS)  	            ../ir/libwais.a \$(REGLIB) ../lib/libftw.a  swais.o \$(OBJS)

########################################################################
# New suffixes and associated building rules -- edit with care

.c.o:
	\$(CC) -c \$(JCFLAGS) \$<

!GROK!THIS!
$spitshell >>Makefile <<'!NO!SUBS!'
########################################################################
# Jmake rules for building libraries, programs, scripts, and data files
# $X-Id: Jmake.rules,v 1.2 1995/09/25 12:41:11 pfeifer Exp $

########################################################################
# Force 'make depend' to be performed first -- do not edit

.FORCE_DEPEND::

all:: .FORCE_DEPEND

########################################################################
# Start of Jmakefile
########################### -*- Mode: Makefile -*- ###########################
# Imakefile --
# ITIID           : $ITI$ $X-Header $__Header$
# Author          : Ulrich Pfeifer
# Created On      : Tue Mar  8 14:48:35 1994
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Thu Jan 18 10:01:20 1996
# Update Count    : 101
# Status          : Unknown, Use with caution!
##############################################################################

OBJS = $(ISOOBJS) \
		    question.o wais-ui.o document.o \
		    util.o  source.o \
		    ../ir/libwais.a $(REGLIB) ../lib/libftw.a
SOURCES = question.c wais-ui.c document.c util.c  source.c

all:: waisq

local_realclean::
	$(RM) waisq

waisq:  waisq.o $(OBJS)
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@  waisq.o $(OBJS) $(JLDFLAGS) $(LIBS)

all:: waissearch

local_realclean::
	$(RM) waissearch

waissearch:  waissearch.o $(ISOOBJS)  	            ../ir/libwais.a $(REGLIB) ../lib/libftw.a
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@  waissearch.o $(ISOOBJS)  	            ../ir/libwais.a $(REGLIB) ../lib/libftw.a $(JLDFLAGS) $(LIBS)

all:: swais

local_realclean::
	$(RM) swais

swais:  swais.o $(OBJS)
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@  swais.o $(OBJS) $(JLDFLAGS) $(LIBS)

install:: waisq
	$(INSTALL) -c -s -m 555  waisq $(BIN_EXP)

deinstall::
	$(RM) $(BIN_EXP)/waisq

install::
	$(RM) $(BIN_EXP)/waisping
	$(LN) $(BIN_EXP)/waisq $(BIN_EXP)/waisping

install:: waissearch
	$(INSTALL) -c -s -m 555  waissearch $(BIN_EXP)

deinstall::
	$(RM) $(BIN_EXP)/waissearch

install:: swais
	$(INSTALL) -c -s -m 555  swais $(BIN_EXP)

deinstall::
	$(RM) $(BIN_EXP)/swais

wais.el:: wais.el.raw wais.el.raw
	$(RM) $@
	$(CPP) -DINSTALLROOT=\"$(BIN_EXP)\" <wais.el.raw | \
	sed -e '/^#  *[0-9][0-9]*  *.*$$/d' >$@
	chmod a+x $@

local_realclean::
	$(RM) wais.el

install::
	@for dir in  $(PRIVLIB)/emacs $(PRIVLIB)/emacs/lisp; do \
	case '${MFLAGS}' in *[i]*) set +e;; esac; \
	(set -x; test -d $$dir || $(INSTALLDIR) $$dir); \
	done

install:: wais.el
	$(INSTALL) -c -m 444 wais.el  $(PRIVLIB)/emacs/lisp

deinstall::
	$(RM)  $(PRIVLIB)/emacs/lisp/wais.el

depend:: ../mkdep

../mkdep:
	@echo "You have to run Configure in $(TOP) first."; exit 1

depend::
	($(SED) '/^# DO NOT DELETE/q' Makefile && \
	($(MKDEP) $(SOURCES); echo 'foo: bar') | \
	$(SED) -e 's:/usr/include[^ ]*::g; s:$(USRINC)[^ ]*::g; ' \
	-e '/: / b print' -e 'H; d; n; : print' -e 'x; s/\\\n//g' \
	-e 's/  */ /g; s/ :/:/;' -e '/: *$$/d' \
	) > Makefile.new
	cp Makefile Makefile.bak
	cp Makefile.new Makefile
	$(RM) Makefile.new

########################################################################
# Common rules for all Makefiles -- do not edit

emptyrule::

clean: local_clean
realclean: local_realclean
clobber: local_clobber

local_clean::
	$(RM) core *~ *.o

local_realclean:: local_clean

local_clobber:: local_realclean
	$(RM) Makefile config.sh

Makefile.SH: Jmakefile
	-@if test -f $(TOP)/.package; then \
	if test -f Makefile.SH; then \
	echo "	$(RM) Makefile.SH~; $(MV) Makefile.SH Makefile.SH~"; \
	$(RM) Makefile.SH~; $(MV) Makefile.SH Makefile.SH~; \
	fi; \
	echo "	$(JMAKE) -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT)" ; \
	$(JMAKE) -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT) ; \
	else touch $@; exit 0; fi

Makefile: Makefile.SH
	/bin/sh Makefile.SH

tags::
	$(CTAGS) -w *.[ch]
	$(CTAGS) -xw *.[ch] > tags

local_clobber::
	$(RM) tags

########################################################################
# Empty rules for directories with no sub-directories -- do not edit

install::
	@echo "install in $(CURRENT) done."

deinstall::
	@echo "deinstall in $(CURRENT) done."

install.man::
	@echo "install.man in $(CURRENT) done."

deinstall.man::
	@echo "deinstall.man in $(CURRENT) done."

Makefiles::

Makefiles.SH::

########################################################################
# Dependencies generated by make depend
# DO NOT DELETE THIS LINE -- make depend relies on it

# Put nothing here or make depend will gobble it up
.FORCE_DEPEND::
	@echo "You must run 'make depend' in $(TOP) first."; exit 1
!NO!SUBS!
chmod 644 Makefile
$eunicefix Makefile

