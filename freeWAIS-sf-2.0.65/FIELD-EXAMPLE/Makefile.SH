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
CURRENT=FIELD-EXAMPLE
DIR=`echo $CURRENT/ | sed -e 's/\.\///g'`
echo "Extracting ${DIR}Makefile (with variable substitutions)"

DATE=`date`

$spitshell >Makefile <<!GROK!THIS!
########################################################################
# Makefile generated from Makefile.SH on $DATE

SHELL = /bin/sh
JMAKE = jmake
TOP = ..
CURRENT = $CURRENT
DIR = $DIR

########################################################################
# Parameters set by Configure -- edit config.sh if changes are needed

CTAGS = ctags
MAKE = make
MKDEP = $mkdep \$(DPFLAGS) --
MV = $mv
RM = $rm -f
SED = $sed

########################################################################
# Automatically generated parameters -- do not edit

WAISSEARCH_ARGS = $waissearch_args
USRINC = $usrinc

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
# Created On      : Fri Feb 25 13:30:41 1994
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Wed Jan 31 15:36:56 1996
# Update Count    : 137
# Status          : Unknown, Use with caution!
##############################################################################

DIFF = cmp
WAISINDEX = ../ir/waisindex
WAISQ = ../ui/waisq
WAISSEARCH = ../ui/waissearch
SWAIS = ../ui/swais
SRCG = TEST TEST.german
SRC = TEST
RESFILES = \
	               test.cat \
	               test.doc \
	               test.doc \
	               test.fn \
	               test.hl \
	               test.inv \
	               test.src \
	               test.dct \
	               test_field_au.inv \
	               test_field_ck.inv \
	               test_field_jt.inv \
	               test_field_names.inv \
	               test_field_py.inv \
	               test_field_ti.inv \
	               test_field_au.dct \
	               test_field_ck.dct \
	               test_field_py.dct \
	               test_field_ti.dct \
	               test_field_jt.dct \
	               test_field_ed.dct \
	               test_field_ed.inv \
	               testg_field_ed.dct \
	               testg_field_ed.inv \
	               testg.cat \
	               testg.doc \
	               testg.doc \
	               testg.fn \
	               testg.hl \
	               testg.inv \
	               testg.src \
	               testg.dct \
	               testg_field_au.inv \
	               testg_field_ck.inv \
	               testg_field_jt.inv \
	               testg_field_names.inv \
	               testg_field_py.inv \
	               testg_field_ti.inv \
	               testg_field_au.dct \
	               testg_field_ck.dct \
	               testg_field_py.dct \
	               testg_field_ti.dct \
	               testg_field_jt.dct

HELPFILES = \
	               test.index.lock \
	               test.query.lock \
	               test.update.lock \
	               testg.index.lock \
	               testg.query.lock \
	               testg.update.lock

all:: $(WAISINDEX) testg.doc test.doc test

test.doc: $(SRC) $(WAISINDEX)
	$(WAISINDEX) -t fields -d test $(SRC)

testg.doc: $(SRCG) $(WAISINDEX)
	$(WAISINDEX) -t fields -d testg $(SRCG)

local_clean::
	$(RM) $(HELPFILES)

local_realclean:: clean
	$(RM) $(RESFILES)
itest:
	$(SWAIS) -S . -s test.src

$(SWAIS):
	cd ../ui; $(MAKE) $(SWAIS)

$(WAISINDEX):
	cd ../ir; $(MAKE) $(WAISINDEX)

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

test:: PLAIN

PLAIN: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test Probabilistic Indexing > \
	PLAIN.is 2>&1 ;
	@-if $(DIFF) PLAIN.is PLAIN.should ; then \
	echo Test PLAIN is ok; \
	else \
	echo Test PLAIN failed; \
	fi;

init:: init-PLAIN

init-PLAIN: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test Probabilistic Indexing > \
	PLAIN.should 2>&1

local_clean::
	$(RM) PLAIN.is

test:: WEIGHT

WEIGHT: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test 'Probabilistic <* 3 Indexing' > \
	WEIGHT.is 2>&1 ;
	@-if $(DIFF) WEIGHT.is WEIGHT.should ; then \
	echo Test WEIGHT is ok; \
	else \
	echo Test WEIGHT failed; \
	fi;

init:: init-WEIGHT

init-WEIGHT: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test 'Probabilistic <* 3 Indexing' > \
	WEIGHT.should 2>&1

local_clean::
	$(RM) WEIGHT.is

test:: BOOLEAN

BOOLEAN: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test "au=(pennekamp or fuhr) and processing" > \
	BOOLEAN.is 2>&1 ;
	@-if $(DIFF) BOOLEAN.is BOOLEAN.should ; then \
	echo Test BOOLEAN is ok; \
	else \
	echo Test BOOLEAN failed; \
	fi;

init:: init-BOOLEAN

init-BOOLEAN: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test "au=(pennekamp or fuhr) and processing" > \
	BOOLEAN.should 2>&1

local_clean::
	$(RM) BOOLEAN.is

test:: FIELD

FIELD: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test au=pfeifer > \
	FIELD.is 2>&1 ;
	@-if $(DIFF) FIELD.is FIELD.should ; then \
	echo Test FIELD is ok; \
	else \
	echo Test FIELD failed; \
	fi;

init:: init-FIELD

init-FIELD: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test au=pfeifer > \
	FIELD.should 2>&1

local_clean::
	$(RM) FIELD.is

test:: NUMERIC

NUMERIC: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test py==1995 > \
	NUMERIC.is 2>&1 ;
	@-if $(DIFF) NUMERIC.is NUMERIC.should ; then \
	echo Test NUMERIC is ok; \
	else \
	echo Test NUMERIC failed; \
	fi;

init:: init-NUMERIC

init-NUMERIC: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test py==1995 > \
	NUMERIC.should 2>&1

local_clean::
	$(RM) NUMERIC.is

test:: COMPLEX

COMPLEX: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test "py==1995 and (ti=(Retrieval freeWAIS) or au=pfeifer)" > \
	COMPLEX.is 2>&1 ;
	@-if $(DIFF) COMPLEX.is COMPLEX.should ; then \
	echo Test COMPLEX is ok; \
	else \
	echo Test COMPLEX failed; \
	fi;

init:: init-COMPLEX

init-COMPLEX: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test "py==1995 and (ti=(Retrieval freeWAIS) or au=pfeifer)" > \
	COMPLEX.should 2>&1

local_clean::
	$(RM) COMPLEX.is

test:: PARTIAL

PARTIAL: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test 'Pfeif*' > \
	PARTIAL.is 2>&1 ;
	@-if $(DIFF) PARTIAL.is PARTIAL.should ; then \
	echo Test PARTIAL is ok; \
	else \
	echo Test PARTIAL failed; \
	fi;

init:: init-PARTIAL

init-PARTIAL: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test 'Pfeif*' > \
	PARTIAL.should 2>&1

local_clean::
	$(RM) PARTIAL.is

test:: DATE

DATE: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test 'ed>19930101' > \
	DATE.is 2>&1 ;
	@-if $(DIFF) DATE.is DATE.should ; then \
	echo Test DATE is ok; \
	else \
	echo Test DATE failed; \
	fi;

init:: init-DATE

init-DATE: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test 'ed>19930101' > \
	DATE.should 2>&1

local_clean::
	$(RM) DATE.is

test:: LITERAL

LITERAL: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test "'Enhanced Retrieval'" > \
	LITERAL.is 2>&1 ;
	@-if $(DIFF) LITERAL.is LITERAL.should ; then \
	echo Test LITERAL is ok; \
	else \
	echo Test LITERAL failed; \
	fi;

init:: init-LITERAL

init-LITERAL: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test "'Enhanced Retrieval'" > \
	LITERAL.should 2>&1

local_clean::
	$(RM) LITERAL.is

test:: PROXIMITY

PROXIMITY: test.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test '(Searching w/2 Documents) or (Development pre/3 Linear)' > \
	PROXIMITY.is 2>&1 ;
	@-if $(DIFF) PROXIMITY.is PROXIMITY.should ; then \
	echo Test PROXIMITY is ok; \
	else \
	echo Test PROXIMITY failed; \
	fi;

init:: init-PROXIMITY

init-PROXIMITY: test.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d test '(Searching w/2 Documents) or (Development pre/3 Linear)' > \
	PROXIMITY.should 2>&1

local_clean::
	$(RM) PROXIMITY.is

test:: GERMAN

GERMAN: testg.doc
	@(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d testg "au=großjohann" > \
	GERMAN.is 2>&1 ;
	@-if $(DIFF) GERMAN.is GERMAN.should ; then \
	echo Test GERMAN is ok; \
	else \
	echo Test GERMAN failed; \
	fi;

init:: init-GERMAN

init-GERMAN: testg.doc
	(echo q;echo q) | \
	$(WAISSEARCH) $(WAISSEARCH_ARGS) -d testg "au=großjohann" > \
	GERMAN.should 2>&1

local_clean::
	$(RM) GERMAN.is

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

