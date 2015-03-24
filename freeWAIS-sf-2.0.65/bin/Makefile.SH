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
CURRENT=bin
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

CTAGS = ctags
L = $manext
MANSRC = $installmansrc
MAKE = make
MKDEP = $mkdep \$(DPFLAGS) --
MV = $mv
RM = $rm -f
SCRIPTDIR = $installscript
SED = $sed

########################################################################
# Automatically generated parameters -- do not edit

USRINC = $usrinc
SCRIPTS =  check-sources getaddrs server_stats stats.awk wais-gif-display wais-html-display wais-jfif-display wais-jpeg-display wais-pict-display wais-ppm-display wais-tiff-display waisretrieve ws makedb catalog dictionary inverted_file mkfmt

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
# Created On      : Tue Mar  8 14:49:56 1994
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Wed Sep 20 13:20:29 1995
# Update Count    : 42
# Status          : Unknown, Use with caution!
##############################################################################

all::
	@echo "Nothing to make in \"bin\".";

makedb: makedb.SH
	/bin/sh makedb.SH

makedb.man: makedb
	pod2man makedb > makedb.man

install.man:: makedb.man
	$(INSTALL) -c -m 444 makedb.man $(MANSRC)/makedb.$(L)

deinstall.man::
	$(RM) $(MANSRC)/makedb.$(L)

catalog: catalog.SH
	/bin/sh catalog.SH

catalog.man: catalog
	pod2man catalog > catalog.man

install.man:: catalog.man
	$(INSTALL) -c -m 444 catalog.man $(MANSRC)/catalog.$(L)

deinstall.man::
	$(RM) $(MANSRC)/catalog.$(L)

dictionary: dictionary.SH
	/bin/sh dictionary.SH

dictionary.man: dictionary
	pod2man dictionary > dictionary.man

install.man:: dictionary.man
	$(INSTALL) -c -m 444 dictionary.man $(MANSRC)/dictionary.$(L)

deinstall.man::
	$(RM) $(MANSRC)/dictionary.$(L)

inverted_file: inverted_file.SH
	/bin/sh inverted_file.SH

inverted_file.man: inverted_file
	pod2man inverted_file > inverted_file.man

install.man:: inverted_file.man
	$(INSTALL) -c -m 444 inverted_file.man $(MANSRC)/inverted_file.$(L)

deinstall.man::
	$(RM) $(MANSRC)/inverted_file.$(L)

mkfmt: mkfmt.SH
	/bin/sh mkfmt.SH

mkfmt.man: mkfmt
	pod2man mkfmt > mkfmt.man

install.man:: mkfmt.man
	$(INSTALL) -c -m 444 mkfmt.man $(MANSRC)/mkfmt.$(L)

deinstall.man::
	$(RM) $(MANSRC)/mkfmt.$(L)

ws: ws.SH
	/bin/sh ws.SH

install:: $(SCRIPTS) $(LSCRIPTS)
	@for file in $(SCRIPTS) $(LSCRIPTS); do \
	case '${MFLAGS}' in *[i]*) set +e;; esac; \
	(set -x; $(INSTALL) -c -m 555 $$file $(SCRIPTDIR)); \
	done

deinstall::
	@for file in $(SCRIPTS) $(LSCRIPTS); do \
	case '${MFLAGS}' in *[i]*) set +e;; esac; \
	(set -x; $(RM) $(SCRIPTDIR)/$$file); \
	done

install.man::
	@for dir in  $(MANSRC); do \
	case '${MFLAGS}' in *[i]*) set +e;; esac; \
	(set -x; test -d $$dir || $(INSTALLDIR) $$dir); \
	done

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

clean: local_clean
realclean: local_realclean
clobber: local_clobber

local_clean::
	$(RM) core *~ *.o

local_realclean:: local_clean

local_clobber:: local_realclean
	$(RM) Makefile config.sh

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

