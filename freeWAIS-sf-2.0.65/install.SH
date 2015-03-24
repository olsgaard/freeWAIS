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
echo "Extracting install (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >install <<!GROK!THIS!
$startsh
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>install <<'!NO!SUBS!'

#
# install - install a program, script, or datafile
# This comes from X11R5; it is not part of GNU.
#
# $XConsortium: install.sh,v 1.2 89/12/18 14:47:22 jim Exp $
#
# This script is compatible with the BSD install script, but was written
# from scratch.
#


# set DOITPROG to echo to test this script

# Don't use :- since 4.3BSD and earlier shells don't like it.
doit="${DOITPROG-}"


# put in absolute paths if you don't have them in your path; or use env. vars.

mvprog="${MVPROG-mv}"
cpprog="${CPPROG-cp}"
chmodprog="${CHMODPROG-chmod}"
chownprog="${CHOWNPROG-chown}"
chgrpprog="${CHGRPPROG-chgrp}"
stripprog="${STRIPPROG-strip}"
rmprog="${RMPROG-rm}"

instcmd="$cpprog"
chmodcmd=""
chowncmd=""
chgrpcmd=""
stripcmd=""
rmcmd="$rmprog -f"
mvcmd="$mvprog"
src=""
dst=""

while [ x"$1" != x ]; do
    case $1 in
	-c) instcmd="$cpprog"
	    shift
	    continue;;

	-m) chmodcmd="$chmodprog $2"
	    shift
	    shift
	    continue;;

	-o) chowncmd="$chownprog $2"
	    shift
	    shift
	    continue;;

	-g) chgrpcmd="$chgrpprog $2"
	    shift
	    shift
	    continue;;

	-s) stripcmd="$stripprog"
	    shift
	    continue;;

	*)  if [ x"$src" = x ]
	    then
		src=$1
	    else
		dst=$1
	    fi
	    shift
	    continue;;
    esac
done

if [ x"$src" = x ]
then
	echo "install:  no input file specified"
	exit 1
fi

if [ x"$dst" = x ]
then
	echo "install:  no destination specified"
	exit 1
fi


# If destination is a directory, append the input filename; if your system
# does not like double slashes in filenames, you may need to add some logic

if [ -d $dst ]
then
	dst="$dst"/`basename $src`
fi

# Make a temp file name in the proper directory.

dstdir=`dirname $dst`
dsttmp=$dstdir/#inst.$$#

# Move or copy the file name to the temp name

$doit $instcmd $src $dsttmp

# and set any options; do chmod last to preserve setuid bits

if [ x"$chowncmd" != x ]; then $doit $chowncmd $dsttmp; fi
if [ x"$chgrpcmd" != x ]; then $doit $chgrpcmd $dsttmp; fi
if [ x"$stripcmd" != x ]; then $doit $stripcmd $dsttmp; fi
if [ x"$chmodcmd" != x ]; then $doit $chmodcmd $dsttmp; fi

# Now rename the file to the real destination.

$doit $rmcmd $dst
$doit $mvcmd $dsttmp $dst


exit 0
!NO!SUBS!
chmod 755 install
$eunicefix install
