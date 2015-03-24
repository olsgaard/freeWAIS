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
CURRENT=lib
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
CTAGS = ctags
JCFLAGS = \$(CFLAGS) $optimize $ccflags $large
JLDFLAGS = \$(LDFLAGS) $optimize $ldflags
L = $manext
LIBS = $libs
MANSRC = $installmansrc
MAKE = make
MKDEP = $mkdep \$(DPFLAGS) --
MV = $mv
PRIVLIB = $installprivlib
RANLIB = $ranlib
RM = $rm -f
SED = $sed

########################################################################
# Automatically generated parameters -- do not edit

CFLAGS = -I. $ctypeinc
ISOOBJS = $ctypeobj
BIN_EXP = $installbin
MAN3 = `echo $installmansrc | $sed -e 's/man.\$/man3/'`
L = \$(MANSUFFIX)
USRINC = $usrinc
SOURCES =  \$(SRCS) trunc.c stringtoany.c ftwtest.c scantest.c
OBJECTS =  \$(OBJS) trunc.o stringtoany.o ftwtest.o libftw.a \$(ISOOBJS) scantest.o libftw.a \$(ISOOBJS)

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
# Created On      : Tue Mar  8 14:46:34 1994
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Tue Oct 31 18:28:38 1995
# Update Count    : 79
# Status          : Unknown, Use with caution!
##############################################################################

OBJS = alphasort.o \
	                   alloca.o \
	                   freedir.o \
	                   ftw.o \
	                   scandir.o

SRCS = alphasort.c \
	            alloca.c \
	                   freedir.c \
	                   ftw.c \
	                   scandir.c

all::	      trunc stringtoany ftwtest scantest

all:: libftw.a

local_realclean::
	$(RM) libftw.a

libftw.a: $(OBJS)
	$(RM) $@
	$(AR) $@ $(OBJS)
	$(RANLIB) $@

install::
	@for dir in  $(PRIVLIB) $(BIN_EXP); do \
	case '${MFLAGS}' in *[i]*) set +e;; esac; \
	(set -x; test -d $$dir || $(INSTALLDIR) $$dir); \
	done

install.man::
	@for dir in  $(MAN3); do \
	case '${MFLAGS}' in *[i]*) set +e;; esac; \
	(set -x; test -d $$dir || $(INSTALLDIR) $$dir); \
	done

install:: libftw.a
	$(INSTALL) -c -m 644 libftw.a $(PRIVLIB)
	$(RANLIB) $(PRIVLIB)/libftw.a
	chmod 444 $(PRIVLIB)/libftw.a

deinstall::
	$(RM) $(PRIVLIB)/libftw.a

all:: trunc

local_realclean::
	$(RM) trunc

trunc: trunc.o
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@ trunc.o $(JLDFLAGS) $(LIBS)

install:: trunc
	$(INSTALL) -c -s -m 555  trunc $(BIN_EXP)

deinstall::
	$(RM) $(BIN_EXP)/trunc

all:: stringtoany

local_realclean::
	$(RM) stringtoany

stringtoany: stringtoany.o
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@ stringtoany.o $(JLDFLAGS) $(LIBS)

install:: stringtoany
	$(INSTALL) -c -s -m 555  stringtoany $(BIN_EXP)

deinstall::
	$(RM) $(BIN_EXP)/stringtoany

all:: ftwtest

local_realclean::
	$(RM) ftwtest

ftwtest: ftwtest.o libftw.a $(ISOOBJS)
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@ ftwtest.o libftw.a $(ISOOBJS) $(JLDFLAGS) $(LIBS)

all:: scantest

local_realclean::
	$(RM) scantest

scantest: scantest.o libftw.a $(ISOOBJS)
	$(RM) $@
	if test -f $@; then $(MV) $@ $@~; else exit 0; fi
	$(CC) -o $@ scantest.o libftw.a $(ISOOBJS) $(JLDFLAGS) $(LIBS)

L=3

install.man:: ftw.man
	$(INSTALL) -c -m 444 ftw.man $(MAN3)/ftw.$(L)

deinstall.man::
	$(RM) $(MAN3)/ftw.$(L)

install.man:: scandir.man
	$(INSTALL) -c -m 444 scandir.man $(MAN3)/scandir.$(L)

deinstall.man::
	$(RM) $(MAN3)/scandir.$(L)

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

test:		ftwtest scantest
		ftwtest /tmp
		scantest /tmp

local_realclean:: clean
	$(RM)  $(LOCAL_LIBRARIES) trunc stringtoany ftwtest scantest

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

