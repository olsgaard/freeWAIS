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
echo "Extracting bin/catalog (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >catalog <<!GROK!THIS!
#!$perl
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>catalog <<'!NO!SUBS!'
#                              -*- Mode: Perl -*- 
# 
# catalog -- 
# ITIID           : $ITI$ $Header $__Header$
# Author          : Ulrich Pfeifer
# Created On      : Sun Jun  4 15:06:56 1995
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Wed Sep 20 12:30:42 1995
# Language        : Perl
# Update Count    : 63
# Status          : Unknown, Use with caution!
# 
# (C) Copyright 1995, Universität Dortmund, all rights reserved.
# 
# HISTORY
# 
# $$
# $Log: catalog.SH,v $
# Revision 2.0.1.1  1995/09/20 12:11:42  pfeifer
# patch14: Added author.
#
# Revision 2.0  1995/09/20  09:49:55  pfeifer
# Print a catalog of a wais database
#

=head1 NAME

catalog - print the catalog of a WAIS database

=head1 SYNOPSIS

B<catalog> I<database>

=head1 DESCRIPTION

I<catalog> reads the document table I<database>C<.doc>, the filename
table I<database>C<.fn>, and the headline table I<database>C<.hl> and
prints a catalog file similar to I<database>C<.cat>. Usefull if you
supresses the generation of I<database>C<.cat> with the B<-nocat>
option of I<waisindex>.

=head1 FILES

I<database>C<.doc>, I<database>C<.fn>, and I<database>C<.hl>.

=head1 SEE ALSO

L<perl> and L<waisindex>.

=head1 AUTHOR

Ulrich Pfeifer <pfeifer@ls6.informatik.uni-dortmund.de>

=cut

@moname = (Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec);

$db = $ARGV[0];

open(DOC, "<$db.doc") || die "Could not open $db.doc: $!\n";
open(FN, "$db.fn") || die "Could not open $db.fn: $!\n";
open(HL, "$db.hl") || die "Could not open $db.hl: $!\n";

read(DOC,$head,2);
print "$head\n";

if (unpack('S', $head) != 0) {
    die "Unvalid document table $db.doc";
}

read(DOC,$entry,25);
$ndocs=0;
while (length($entry)) {
    ($fh, $fl,
     $hh, $hl,
     $start,
     $end,
     $terms,
     $lh, $ll,
     $date) = unpack('CSCSLLLCSL', $entry);
    $fileo = $fh * 0x10000 + $fl;
    $heado = $hh * 0x10000 + $hl;
    $lines = $lh * 0x10000 + $ll;
    next unless $fileo;
    ($fname,$type) = get_fn($fileo) if $fileo;
    $headline = get_hl($heado);
    write;
} continue {
    $ndocs++;
    read(DOC,$entry,25);
}

close(DOC);
close(FN);
close(HL);

sub get_fn {
    my($offset) = @_;
    my($file, $mtime, $type, $block);

    unless ($fn{$offset}) {
        seek(FN,$offset,0) || die "could not seek $offset in $db.fn: $!\n";
        read(FN, $block, 255);
        $file = &gets($block);
        ($mtime) = unpack('I', $block);
        $type = &gets(substr($block,4));
        $fn{$offset} = $file;
        $ty{$offset} = $type;
    }
    return($fn{$offset}, $ty{$offset});
}

sub get_hl {
    my($offset) = @_;
    my($headline);

    seek(HL,$offset,0) || die "could not seek $offset in $db.fn: $!\n";
    read(HL, $block, 255);
    $headline = &gets($block);
}

sub gets {
    my($s);

    ($s) = ($_[0] =~ /([^\0]*)\0/);
    $_[0] = $';
    $s;
}

format STDOUT_TOP =
docno  start  end    terms  lines  date
  type filename
  headline
.
format STDOUT =
@>>>>> @>>>>> @>>>>> @>>>>> @>>>>> @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$ndocs, $start, $end, $terms, $lines, &date($date)
  @<<<<<< @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$type,$fname
  @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$headline

.

sub date {
    my($wtime) = @_;

    #($sec,$min,$hour,$mday,$mon,$year) = localtime($mtime);
    ($year, $mon, $mday) = ($wtime =~ /(..)(..)(..)/);
    $moname = $moname[$mon];
    $timeyear = $year + 1900;

    sprintf "%s %2d %5s", $moname, $mday, $timeyear;
}
!NO!SUBS!
chmod 755 catalog
$eunicefix catalog
