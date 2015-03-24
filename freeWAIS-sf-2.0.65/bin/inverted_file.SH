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
echo "Extracting bin/inverted_file (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >inverted_file <<!GROK!THIS!
#!$perl
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>inverted_file <<'!NO!SUBS!'
#                              -*- Mode: Perl -*- 
# 
# catalog -- 
# ITIID           : $ITI$ $Header $__Header$
# Author          : Ulrich Pfeifer
# Created On      : Sun Jun  4 15:06:56 1995
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Wed Sep 20 12:33:13 1995
# Language        : Perl
# Update Count    : 205
# Status          : Unknown, Use with caution!
# 
# (C) Copyright 1995, Universität Dortmund, all rights reserved.
# 
# HISTORY
# 
# $$
# $Log: inverted_file.SH,v $
# Revision 2.0.1.1  1995/09/20 12:12:27  pfeifer
# patch14: Fixed references.
#
# Revision 2.0  1995/09/20  10:18:08  pfeifer
# Print the inverted file or a single list.
#
# 

=head1 NAME

inverted_file - print the inverted file of a WAIS database

=head1 SYNOPSIS

B<inverted_file> I<database>

B<inverted_file> I<database> I<offset>

=head1 DESCRIPTION

I<inverted_file> reads the inverted file I<database>C<.inv> of a wais
database and prints it in human readable form. If a second argument
I<offset> (as printed by I<dictionary> is given only the list at this
offset is printed.

=head1 FILES

I<database>C<.inv>

=head1 EXAMPLES

        B<inverted_file> FIELD-EXAMPLE/test

        B<inverted_file> FIELD-EXAMPLE/test_field_ck

        B<inverted_file> FIELD-EXAMPLE/test_field_ck 21867

=head1 SEE ALSO

L<perl>, L<waisindex>, and L<dictionary>.

=head1 AUTHOR

Ulrich Pfeifer <pfeifer@ls6.informatik.uni-dortmund.de>

=cut

$db = $ARGV[0];

open(INV, "<$db.inv") || die "Could not open $db.inv: $!\n";

if ($#ARGV > $[) {
    seek(INV,$ARGV[1],0);
    &entry;
} else {
    read(INV,$head,4);
    ($nib) =  unpack('L', $head); 
    print "$db.inv contains $nib terms\n";
    $terms=0;

    while (!eof(INV)) {
        &header();
        &entry();
        $terms++;
    }
}

sub entry {
    my($np, $i);
    $np = &header_rest();
    for($i=0;$i<$np;$i++) {
        &posting($np);
    }
}

sub header {
    my($entry, $df, $x, $y, $next_1, $size_1, $postings,
       $occ, $term);
    read(INV,$entry,13);

    ($df, $x, $y, $next_1, $size_1, $occ) = unpack('CCCLSL', $entry);
    #^^^  00  00  0000     ^^^^^^^  ^^^^

    die "Dictionary flag not valid" if $df != 123;

    read(INV,$term,$size_1-13); chomp($term);
    printf ("%-20s %6d\n", $term, $occ);
}

sub header_rest {
    my($full,$postings,$size_2);

    read(INV,$entry,9);
    ($full,$postings,$size_2,) = unpack('aLL', $entry);
    #^^^^^ ^^^^^^^    0000  ^^^^^^^ 
    #print "$size_2)\n";
    die "invalid full flag: $full" if $full ne 'E';
    die if $x;
    die if $y;
    die if $next_1;
    return($postings);
}

sub posting {
    my($ints) = @_;
    my($docid, $size_3, $weight, $ch, $cl, $docid, $weight, $charpos);

    read(INV,$entry,15);
    ($docid,$size_3,$weight, $ch,$cl) = unpack('LLfSC', $entry);
    $charpos = ($ch<<8) + $cl;
    printf ("\t\t(%6d, %7.5f) %d", $docid,$weight, $charpos);
    read(INV,$entry,$size_3-3);
    while(length($entry)) {
        printf (", %d", &readCompressedInteger(*entry));
    }
    print "\n";
}

sub readCompressedInteger {
    local (*buf) = @_;
    my ($number, $byte);

    # this initialisation is just for tuning: most frequent case is
    # 0<=n<=127
    ($byte, $buf) = unpack("C1 a*", $buf);
    return($byte) if (($byte & 128)==0);
    $number = $byte&127;

    do {			# get one byte from buf at first
	($byte, $buf) = unpack("C1 a*", $buf);
	$number <<= 7;
	$number += ($byte & 127); # 127 = 7F in hexadecimal   
    } until (($byte & 128) == 0); # until the most significant  
    # bit of byte equals to 0
    $number;
}                        

!NO!SUBS!
chmod 755 inverted_file
$eunicefix inverted_file
