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
CURRENT=ir
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

AR = ar rc
CC = $cc
CP = $cp
CTAGS = ctags
JCFLAGS = \$(CFLAGS) $optimize $ccflags $large
JLDFLAGS = \$(LDFLAGS) $optimize $ldflags
JLFLAGS = \$(LFLAGS)
JYFLAGS = \$(YFLAGS) $yaccflags
LEX = $lex
LIBS = $libs
MAKE = make
MKDEP = $mkdep \$(DPFLAGS) --
MV = $mv
PRIVLIB = $installprivlib
RANLIB = $ranlib
RM = $rm -f
SED = $sed
YACC = $yacc

########################################################################
# Automatically generated parameters -- do not edit

CFLAGS = $ctypeinc
ISOOBJS = $ctypeobj
LDFLAGS =  $socketlib -lm
REGLIB = $regexplib
BIN_EXP = $installbin
USRINC = $usrinc
SOURCES =  query_l.l query_y.y field_l.l field_y.y \$(SRCS) waisserver.c waisindex.c
OBJECTS =  query_l.o query_y.o field_l.o field_y.o \$(ISOOBJS) \$(OBJS) waisserver.o \$(ALLOBJS) waisindex.o \$(ALLOBJS)

########################################################################
# New suffixes and associated building rules -- edit with care

.c.o:
	\$(RM) \$@
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
# Created On      : Tue Mar  8 13:39:44 1994
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Mon Nov  6 09:21:59 1995
# Language        : Makefile
# Update Count    : 154
# Status          : Unknown, Use with caution!
#
# (C) Copyright 1995, Universität Dortmund, all rights reserved.
#
# HISTORY
#
##############################################################################

# $$
# $X-Log: Jmakefile,v $
# Revision 2.0.1.8  1995/10/20  17:08:15  pfeifer
# patch40: Sighash seems to be obsolete?
#
# Revision 2.0.1.7  1995/10/10  18:27:35  pfeifer
# patch33: Rule to compile test_query.
#
# Revision 2.0.1.7  1995/10/10  18:27:35  pfeifer
# patch33: Rule to compile test_query.
#
# Revision 2.0.1.6  1995/10/04  17:19:23  pfeifer
# patch20:
#
# Revision 2.0.1.5  1995/10/04  11:02:12  pfeifer
# Test with metaconfigs errnolist. Does not work. Assumes the name of
# the array to be sys_errnolist instead of sys_errlist.
#
# Revision 2.0.1.4  1995/09/29  11:10:06  pfeifer
# patch17: Added ../ctype/ctype.o to libwais.a. This will cause problems
# patch17: if one switches back to the sustems ctype without removing
# patch17: the library.
#
# Revision 2.0.1.3  1995/09/21  16:28:05  pfeifer
# patch16:
#
# Revision 2.0.1.2  1995/09/19  19:09:52  pfeifer
# patch13: Added Make-Directories for PRIVLIB.
#
# Revision 2.0.1.1  1995/09/08  19:06:20  pfeifer
# patch1: Added dependency to PRIVLIB and $socketlib. Not sure if
# patch1: $socketlib is used correctly?
#
# Revision 2.0.1.1  1995/09/08  19:06:20  pfeifer
# patch1: Added dependency to PRIVLIB and $socketlib. Not sure if
# patch1: $socketlib is used correctly?
#
# Revision 2.0  1995/09/08  07:58:03  pfeifer
# Metaconfig baseline
#
# Revision 1.2  1995/09/07  09:42:44  pfeifer
# Anhaengigkeiten zur Uebersetungszeit muessen noch entfernt werden
#
# Revision 1.1  1995/09/07  07:52:32  pfeifer
# Initial revision
#
# Revision 1.21  1995/03/15  16:39:10  pfeifer
# moved soundex to libwais
#

YFLAGS = -d -l

SRCS = \
	                   cutil.c \
	                   syslog.c \
	                   futil.c \
	                   ircfiles.c \
	                   irfileio.c \
	                   irfiles.c \
	                   irtfiles.c \
	                   irkeywords.c \
	                   stemmer.c \
	                   panic.c \
	                   sockets.c \
	                   transprt.c \
	                   ustubs.c \
	                   wmessage.c \
	                   wprot.c \
	                   wutil.c \
	                   zprot.c \
	                   zutil.c \
	                   ztype1.c \
	                   soundex.c \
	                   docid.c \
	                   list.c \
	                   lock.c \
	                   udping.c \
	                   ir.c \
	                   irretrvl.c \
	                   stoplist.c \
	                   irsearch.c \
	                   stemmer.c \
	                   synonym.c \
	                   trie.c \
	                   field_index.c \
	                   field_search.c \
	                   ui.c\
	                   sersrch.c \
	                   irhash.c \
	                   hash.c \
	                   irinv.c \
		    weight.c \
	                   boolean_op.c \
	                   hash.c \
	                   field_l.c \
	                   field_y.c \
	                   query_y.c \
	                   query_l.c

OBJS = \
	                   cutil.o \
	                   syslog.o \
	                   futil.o \
	                   ircfiles.o \
	                   irfileio.o \
	                   irfiles.o \
	                   irtfiles.o \
	                   irkeywords.o \
	                   stemmer.o \
	                   panic.o \
	                   sockets.o \
	                   transprt.o \
	                   ustubs.o \
	                   wmessage.o \
	                   wprot.o \
	                   wutil.o \
	                   zprot.o \
	                   zutil.o \
	                   ztype1.o \
	                   soundex.o \
	                   docid.o \
	                   list.o \
	                   lock.o \
	                   udping.o \
	                   ir.o \
	                   irretrvl.o \
	                   stoplist.o \
	                   irsearch.o \
	                   stemmer.o \
	                   synonym.o \
	                   trie.o \
	                   field_index.o \
	                   field_search.o \
	                   ui.o\
	                   sersrch.o \
	                   irhash.o \
	                   hash.o \
	                   irinv.o \
		    weight.o \
	                   boolean_op.o \
	                   hash.o \
	                   field_l.o \
	                   field_y.o \
	                   query_y.o \
	                   query_l.o

ALLOBJS = libwais.a $(REGLIB) ../lib/libftw.a

query_l.o: query_y.h

waisserver: query_l.c

query_l.c: query_l.l
	$(LEX) $(JLFLAGS) query_l.l
	$(SED) -e 's/yy\(.\)/query\1/g' < lex.yy.c > query_l.c
	$(RM) lex.yy.c

local_realclean::
	$(RM) query_l.c

waisserver: query_y.c

query_y.c: query_y.y
	$(YACC) $(JYFLAGS) query_y.y
	$(SED) -e 's/yy\(.\)/query\1/g' < y.tab.c > query_y.c
	$(SED) -e 's/yy\(.\)/query\1/g' < y.tab.h > query_y.h
	$(RM) y.tab.c y.tab.h

local_realclean::
	$(RM) query_y.c

query_y.h: query_y.c
	@if test -f y.tab.h; then \
	echo "	$(SED) -e 's/yy\(.\)/query\1/g' < y.tab.h > query_y.h"; \
	$(SED) -e 's/yy\(.\)/query\1/g' < y.tab.h > query_y.h; \
	echo "	$(RM) y.tab.h"; \
	$(RM) y.tab.h; \
	elif test -f query_y.h; then \
	echo "	$(CP) query_y.h query_y.ht"; \
	$(CP) query_y.h query_y.ht; \
	echo "	$(SED) -e 's/yy\(.\)/query\1/g' < query_y.ht > query_y.h"; \
	$(SED) -e 's/yy\(.\)/query\1/g' < query_y.ht > query_y.h; \
	echo "	$(RM) query_y.ht"; \
	$(RM) query_y.ht; \
	else \
	exit 0; \
	fi

local_realclean::
	$(RM) query_y.h

field_l.o: field_y.h

waisindex: field_l.c

field_l.c: field_l.l
	$(LEX) $(JLFLAGS) field_l.l
	$(SED) -e 's/yy\(.\)/field\1/g' < lex.yy.c > field_l.c
	$(RM) lex.yy.c

local_realclean::
	$(RM) field_l.c

waisindex: field_y.c

field_y.c: field_y.y
	$(YACC) $(JYFLAGS) field_y.y
	$(SED) -e 's/yy\(.\)/field\1/g' < y.tab.c > field_y.c
	$(SED) -e 's/yy\(.\)/field\1/g' < y.tab.h > field_y.h
	$(RM) y.tab.c y.tab.h

local_realclean::
	$(RM) field_y.c

field_y.h: field_y.c
	@if test -f y.tab.h; then \
	echo "	$(SED) -e 's/yy\(.\)/field\1/g' < y.tab.h > field_y.h"; \
	$(SED) -e 's/yy\(.\)/field\1/g' < y.tab.h > field_y.h; \
	echo "	$(RM) y.tab.h"; \
	$(RM) y.tab.h; \
	elif test -f field_y.h; then \
	echo "	$(CP) field_y.h field_y.ht"; \
	$(CP) field_y.h field_y.ht; \
	echo "	$(SED) -e 's/yy\(.\)/field\1/g' < field_y.ht > field_y.h"; \
	$(SED) -e 's/yy\(.\)/field\1/g' < field_y.ht > field_y.h; \
	echo "	$(RM) field_y.ht"; \
	$(RM) field_y.ht; \
	else \
	exit 0; \
	fi

local_realclean::
	$(RM) field_y.h

all:: libwais.a

local_realclean::
	$(RM) libwais.a

libwais.a: $(ISOOBJS) $(OBJS)
	$(RM) $@
	$(AR) $@ $(ISOOBJS) $(OBJS)
	$(RANLIB) $@

install:: libwais.a
	$(INSTALL) -c -m 644 libwais.a $(PRIVLIB)
	$(RANLIB) $(PRIVLIB)/libwais.a
	chmod 444 $(PRIVLIB)/libwais.a

deinstall::
	$(RM) $(PRIVLIB)/libwais.a

install::
	@for dir in  $(PRIVLIB); do \
	case '${MFLAGS}' in *[i]*) set +e;; esac; \
	(set -x; test -d $$dir || $(INSTALLDIR) $$dir); \
	done

all:: waisserver

local_realclean::
	$(RM) waisserver

waisserver: waisserver.o $(ALLOBJS)
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@ waisserver.o $(ALLOBJS) $(JLDFLAGS) $(LIBS)

all:: waisindex

local_realclean::
	$(RM) waisindex

waisindex: waisindex.o $(ALLOBJS)
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@ waisindex.o $(ALLOBJS) $(JLDFLAGS) $(LIBS)

install:: waisserver
	$(INSTALL) -c -s -m 555  waisserver $(BIN_EXP)

deinstall::
	$(RM) $(BIN_EXP)/waisserver

install:: waisindex
	$(INSTALL) -c -s -m 555  waisindex $(BIN_EXP)

deinstall::
	$(RM) $(BIN_EXP)/waisindex

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

test_query: query_y.c libwais.a
	$(CC) -o $@ query_y.c -DTEST_QUERY  $(JLDFLAGS) libwais.a

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

