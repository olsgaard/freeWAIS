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
: This forces SH files to create target in same directory as SH file.
: This is so that make depend always knows where to find SH derivatives.
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac
echo "Extracting x/Imakefile (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >Imakefile <<!GROK!THIS!
BINDIR=$binexp
PRIVLIB=$installprivlib
PREFIX=$prefix
INSTALL=$install
CC=$cc
MCFLAGS=$ccflags
EXTRA_LDFLAGS=$ldflags
!GROK!THIS!
: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Imakefile <<'!NO!SUBS!'
############################ -*- Mode: Makefile -*- ###########################
## Imakefile -- 
## ITIID           : $ITI$ $Header $__Header$
## Author          : Jonny Goldman
## Created On      : Wed Mar  9 10:59:20 1994
## Last Modified By: Ulrich Pfeifer
## Last Modified On: Thu Sep 28 17:40:55 1995
## Update Count    : 51
## Status          : Unknown, Use with caution!
###############################################################################

#include "../config.h"

#ifdef MYCTYPE
   EXTRA_INCLUDES = -I../ctype $(MCFLAGS)
          ISOOBJS = ../ctype/ctype.o
#else
   EXTRA_INCLUDES = $(MCFLAGS)
#endif
          VERSION = 0.86

              TOP = ..

           SRCDIR = $(TOP)/x
           SUPDIR = $(TOP)/ir
            UIDIR = $(TOP)/ui

/*
 For a smaller client, but with no local search,
 use this line this instead of the latter:
IR_LIB        = ../bin/client.a
*/
#ifdef MYREGEXP
        REGEXPLIB = ../regexp/libregexp.a
#endif

     PROTOCOL_LIB = $(TOP)/ir/libwais.a

           LIBFTW = $(TOP)/lib/libftw.a

           UI_LIB = $(IR_LIB) $(PROTOCOL_LIB) $(REGEXPLIB) $(LIBFTW)


/*
 For the Motif version , add -lXm

  LOCAL_LIBRARIES = $(UI_LIB) -lXm $(XAWLIB) $(XTOOLLIB) $(XMULIB) $(XLIB)

*/


  LOCAL_LIBRARIES = $(UI_LIB) $(XAWLIB) $(XTOOLLIB) $(XMULIB) $(XLIB)


/*
 For the openwin version version , add -I/usr/openwin/include

         INCLUDES = -I$(SRCDIR) -I$(SUPDIR) -I$(UIDIR) -I/usr/openwin/include

*/


         INCLUDES = -I$(SRCDIR) -I$(SUPDIR) -I$(UIDIR) -I/usr/openwin/include

/* for alternate font in text popup, add -DUSE_ALTFONT to CFLAGS (somehow) */

/* this is R5 only! */

    EXTRA_DEFINES = -DUSE_ALTFONT $(CHARFLAGS) 

/*
 For the Motif version, add -DMOTIF and -D_NO_PROTO if your C compiler
 can't handle ANSI prototypes.  You may need to fiddle with the libraries
 to insure all symbols are defined (I did!).

      CDEBUGFLAGS = -g -Bstatic -DUSE_ALTFONT -DMOTIF -D_NO_PROTO

*/


             TOBJ = \
                    xwais.o \
                    display.o \
                    tcommands.o \
                    scommands.o 

             QOBJ = \
                    xwaisq.o \
                    qdisplay.o \
                    text.o \
                     qcommands.o \
                    filereq.o \
                    scommands.o \
                    catalog.o \
                    myAsciiSink.o \
                    myAsciiSrc.o \
                     $(TOP)/ui/question.o \
                    $(TOP)/ui/document.o \
                    $(TOP)/ui/wais-ui.o

             OBJS = MyList.o \
                         util.o \
                    $(TOP)/ui/source.o

       XWAIS_OBJS = $(TOBJ) $(OBJS) $(ISOOBJS) 
      XWAISQ_OBJS = $(QOBJ) $(OBJS) $(ISOOBJS) 

          SYSLIBS = -lm -lc

            XWAIS = xwais
           XWAISQ = xwaisq

             SRCS = \
                    MyList.c \
                    Viewport.c \
                    catalog.c \
                    display.c \
                    filereq.c \
                    myAsciiSink.c \
                    myAsciiSrc.c \
                    qcommands.c \
                    qdisplay.c \
                    scommands.c \
                    tcommands.c \
                    text.c \
                    util.c \
                    xwais.c \
                    xwaisq.c 

     XAPPLOADDIR = $(PRIVLIB)/X11/app-defaults

PROGRAMS = $(XWAIS) $(XWAISQ) Xwais.ad

AllTarget($(PROGRAMS))

NormalProgramTarget($(XWAIS),$(XWAIS_OBJS),$(UI_LIB),$(LOCAL_LIBRARIES),$(SYSLIBS))
NormalProgramTarget($(XWAISQ),$(XWAISQ_OBJS),$(UI_LIB),$(LOCAL_LIBRARIES),$(SYSLIBS))

/* relies on /bin/csh !!
DATE=`date +%y%m%d.%T`

install:: all
	((test -f $(XAPPLOADDIR)/Xwais) && \
	  mv $(XAPPLOADDIR)/Xwais $(XAPPLOADDIR)/Xwais.$(DATE))
*/

InstallAppDefaults(Xwais)
InstallNonExec(XwaisHELP,$(PRIVLIB))
InstallNonExec(XwaisqHELP,$(PRIVLIB))
InstallProgram($(XWAIS), $(BINDIR))
InstallProgram($(XWAISQ), $(BINDIR))

CppFileTarget(Xwais.ad,Xwais.raw,-DINSTALLROOT=$(PREFIX) -DBINDIR=$(BINDIR) -DXAPPLOADDIR=$(XAPPLOADDIR),Xwais.raw)

$(TOP)/ui/question.o $(TOP)/ui/document.o $(TOP)/ui/wais-ui.o $(TOP)/ui/source.o:
	cd ../ui; $(MAKE) $@ 

$(PROTOCOL_LIB):
	(cd $(TOP)/ir;make ${PROTOCOL_LIB})

clean::
	$(RM) Xwais Xwais.ad Makefile.bak *%

veryclean:: clean
	$(RM) xwais xwaisq

Makefile:: Imakefile

Imakefile: Imakefile.SH
	sh Imakefile.SH

DependTarget()

#ifdef MYREGEXP
$(REGEXPLIB):
	cd ../regexp; $(MAKE) 
#endif
!NO!SUBS!
chmod 755 Imakefile
$eunicefix Imakefile
