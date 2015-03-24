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
echo "Extracting bin/dictionary (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >dictionary <<!GROK!THIS!
#!$perl
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>dictionary <<'!NO!SUBS!'
#                              -*- Mode: Perl -*- 
# 
# dictionary -- 
# ITIID           : $ITI$ $Header $__Header$
# Author          : Ulrich Pfeifer
# Created On      : Sun Jun  4 15:06:56 1995
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Wed Sep 20 12:32:02 1995
# Language        : Perl
# Update Count    : 79
# Status          : Unknown, Use with caution!
# 
# (C) Copyright 1995, Universität Dortmund, all rights reserved.
# 
# HISTORY
# 
# $$
# $Log: dictionary.SH,v $
# Revision 2.0.1.1  1995/09/20 12:12:04  pfeifer
# patch14: Added author.
#
# Revision 2.0  1995/09/20  10:00:52  pfeifer
# Print the dictionary of a wais database.
#

=head1 NAME

dictionary - print the dictionary of a WAIS database.

=head1 SYNOPSIS

B<dictionary> I<database>

=head1 DESCRIPTION

I<dictionary> reads the dictionary of I<database> (I<database>C<.doc>)
and prints it in human readable form. You may use it to print the
global dioctionary or and field dictionary. See below.

=head1 EXAMPLES

        dictionary FIELD_EXAMPLE/test
        dictionary FIELD_EXAMPLE/test_field_ti

=head1 FILES

I<database>C<.doc>, I<database>C<.fn>, and I<database>C<.hl>.

=head1 SEE ALSO

L<perl> and L<waisindex>.

=head1 AUTHOR

Ulrich Pfeifer <pfeifer@ls6.informatik.uni-dortmund.de>

=cut

$db = $ARGV[0];

open(DCT, "<$db.dct") || die "Could not open $db.doc: $!\n";


read(DCT,$head,4);

($magic,$nib) =  unpack('SS', $head); 
print "$db.dct has $nib index blocks\n";

# skip the index blocks
seek(DCT,$nib*29,1) || die "could not seek $offset in $db.fn: $!\n";

$terms=0;
read(DCT,$entry,29);
while (length($entry)) {
    ($term,$ptr,$cont) = unpack('a21LL', $entry);
    write if $cont;
} continue {
    $terms++;
    read(DCT,$entry,29 );
}
close(DCT);

format STDOUT_TOP =
term                   occurances  pointer
.
format STDOUT =
@<<<<<<<<<<<<<<<<<<<<  @>>>>>>>>>  @>>>>>>
$term,$cont,$ptr
.

sub date {
    my($mtime) = @_;

    ($sec,$min,$hour,$mday,$mon,$year) = localtime($mtime);
    $moname = $moname[$mon];
    $timeyear = $year + 1900;

    sprintf "%2d:%02d %s %2d %5s %s",
    $hour, $min, $moname, $mday, $timeyear, $pname;
}
!NO!SUBS!
chmod 755 dictionary
$eunicefix dictionary
