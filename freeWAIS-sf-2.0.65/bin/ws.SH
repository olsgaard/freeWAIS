#                              -*- Mode: Perl -*- 
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
echo "Extracting bin/ws (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >ws <<!GROK!THIS!
#!$perl
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>ws <<'!NO!SUBS!'

# WIDE AREA INFORMATION SERVER SOFTWARE:
#   No guarantees or restrictions.  See the readme file for the full standard
#   disclaimer.
#
# A quick shell interface to WAIS:
#   
#      ws <dbname> word ....
#  eg. ws wais-docs  what is wais
#
#Created by Randal L. Schwartz, 
# Stonehenge Consulting Services, Portland, Oregon
# distributed without restriction.

$src = shift;
open(SRC,"$ENV{'HOME'}/wais-sources/$src.src") ||
 open(SRC,"$ENV{'HOME'}/wais-sources/$src") ||
 open(SRC,"$ENV{'WAISCOMMONSOURCEDIR'}/$src.src") ||
 open(SRC,"$ENV{'WAISCOMMONSOURCEDIR'}/$src") ||
 die "Cannot open $1.src: $!";
# print <SRC>;
while(<SRC>) {
	/:ip-name "(.*)"/ && ($ipname = $1);
	/:database-name "(.*)"/ && ($databasename = $1);
	/:tcp-port "(.*)"/ && ($tcpport = $1);
	/:tcp-port (\d+)/ && ($tcpport = $1);
}
close(SRC);
print "waissearch ", 
	"-h ", $ipname,
	" -d ", $databasename,
	" -p ", $tcpport, " ",
	@ARGV, "\n";

exec "waissearch",
	"-h", $ipname,
	"-d", $databasename,
	"-p", $tcpport,
	@ARGV;
!NO!SUBS!
chmod 755 ws
$eunicefix ws
