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
echo "Extracting bin/mkfmt (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >mkfmt <<!GROK!THIS!
#!$perl
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>mkfmt <<'!NO!SUBS!'
#                              -*- Mode: Perl -*- 
# 
# mkfmt -- 
# ITIID           : $ITI$ $Header $__Header$
# Author          : Ulrich Pfeifer
# Created On      : Wed Sep 13 13:43:30 1995
# Last Modified By: Ulrich Pfeifer
# Last Modified On: Wed Sep 20 12:34:03 1995
# Language        : Perl
# Update Count    : 69
# Status          : Unknown, Use with caution!
# 
# (C) Copyright 1995, Universität Dortmund, all rights reserved.
# 
# HISTORY
# 
# $$
# $Log: mkfmt.SH,v $
# Revision 2.0.1.1  1995/09/20 12:13:27  pfeifer
# patch14: Fixed references.
#
# Revision 2.0  1995/09/20  10:19:20  pfeifer
# Interacively generate format files (not really yet)
#
# Revision 1.1  1995/09/13  13:07:31  pfeifer
# Initial revision
#
# 

=head1 NAME

mkfmt - interactively generate format files

=head1 SYNOPSIS

B<mkfmt> I<database> I<test_input>

=head1 DESCRIPTION

I<mkfmt> will some day allow for interactive developement of format
files.  Its second parameter should be a sample input file. Currently
it parses the format file an gives more meaningfull error messages if
errors occur. Then the record-end-separator is used to cut the first
documents from the sample input.

Any help on this greatly appreciated.

=head1 EXAMPLES

        mkfmt FIELD_EXAMPLE/test FIELD_EXAMPLE/TEST

=head1 FILES

I<database>C<.fmt>, I<test_input>

=head1 SEE ALSO

L<perl> and L<waisindex>.

=head1 AUTHOR

Ulrich Pfeifer <pfeifer@ls6.informatik.uni-dortmund.de>

=cut

$opt_database = 'test';
$opt_source   = 'TEST';

use Getopt::Long;
&GetOptions(
            'database=s',
            'debug',
            'source=s',
) || die "Usage $0 ....\n";

$fmtfile = "$opt_database.fmt";

if (-e $fmtfile) {
    print STDERR "Reading $fmtfile ... \n";
    &read_fmt($fmtfile);
}

&test_sep($opt_source);

#print "record_separator=$record_separator\n";

exit(0);

sub yes_or_no {
    my($prompt, $default) = @_;
    my($input);
    do {
        print "$prompt [$default] (y/n) ";
        chomp($input = <STDIN>);
        $input = $default if $input eq '';
    } until ($input =~ /[yn]/);
    return $input;
}

sub test_sep {
    my($file) = @_;
    my($nth) = 0;
    my($more) = 'n';

    local($_);                  # love dynamic scoping

    open(SRC, "<$file") || die "could not open $file: $!";
    do {
        $nth++; @test_document = ();
        print "The document number $nth seems to be:\n\n";
        while (<SRC>) {
            if (/$record_separator/) {
                last;
            } else {
                push(@test_document, $_);
                print;
            }
        }
        print "\n";
    } while (($more = &yes_or_no("Read another one", $more)) eq 'y');
}

sub read_fmt {
    my($file) = @_;

    open(FMT, "<$file") || die "Could not open $file; $!";
    &read_sep;
    &read_layout;
    $fields = $[;
    &skip;
    while (!eof FMT) {
        &read_region;
        &skip;
    }
    close FMT;
}

sub skip {
    s/^\s+//;
    while (!length($_)) {
        chomp($_ = <FMT>);
        print STDERR "$.: $_\n" if $opt_debug;
        s/^\s+//;
        last if eof FMT;
    } 
}

sub read_sep {
    &skip;
    if (s/^<record-end>//) {
        $record_separator = &read_regexp(1);
    } else {
        die "$.: <record-end> expected\n";
    }
}

sub read_regexp {
    my($required) = @_;
    my($regexp);

    &skip;
    if (s:^/(([^/]|\\/)*[^\\])/::) {
        $regexp = $1;
        $regexp =~ s/\\([A-Z])/chr(ord($1) - ord('A'))/e;
        return $1;
    } elsif ($required) {
        die "$.: regular expression expected looking at \"$_\"";
    } else {
        return '';
    }
}

sub read_layout {
    &skip;
    unless (s/^<layout>//) {
        die "$.: '<layout>' expected looking at \"$_\"";
    }
    $headline_definitions = $[;
    while (1) {
        &skip;
        if (s/^<headline>//) {
            &read_headline;
        }
        if (s/^<date>//) {
            &read_date;
        }
        if (s/^<end>//) {
            last;
        }
    }
}

sub read_int {
    &skip;
    if (s/^(\d+)//) {
        return $1;
    } else {
        die "$.: integer expected looking at \"$_\"";
    }
}

sub read_headline {
    $headline_begin[$headline_definitions] = &read_regexp(1);
    $headline_end[$headline_definitions] = &read_regexp(1);
    $headline_length[$headline_definitions] = &read_int;
    $headline_skip[$headline_definitions] = &read_regexp(0);
    $headline_definitions++;
}

sub d_m_y {
    my($type) = @_;
    my($result);

    &skip;
    unless (s/^(day|month|year)//) {
        die "$.: 'day', 'month', or 'year' expected looking at \"$_\"";
    } 
    $result = $1;
    if ($type eq 's') {
        &skip;
        unless (s/^string//) {
            die "$.: 'string' expected looking at \"$_\"";
        }
    }
    return $result;
}

sub read_date {
    $date_start = &read_regexp(1);
    $date_scan  = &read_regexp(1); # missused!!
    @scanargs = ($date_scan =~ /%\d*([sd]).*%\d*([sd]).*%\d*([sd])/);
    $date[0] = &d_m_y($scanargs[0]);
    $date[1] = &d_m_y($scanargs[1]);
    $date[2] = &d_m_y($scanargs[2]);
    $date_skip = &read_regexp(0);
}

sub read_region {
    &skip;
    unless (s/^<field>//) {
        die "$.: '<field>' expected looking at \"$_\"";
    }
    $region_start = &read_regexp(1);
    $region_skip  = &read_regexp(0);
    &skip;
    while (!/^(<numeric>|stemming|TEXT|SOUNDEX|PHONIX)/) {
        &read_field;
        &skip;
    }
    &read_options;
    &read_indexspecs;
    &skip;
    unless (s/^<end>//) {
        die "$.: '<end>' expected looking at \"$_\"";
    }
    &skip;
    $region_end   = &read_regexp(1);
}

sub read_options {
    &skip;
    while (1) {
        if (s/^<numeric>//) {
            &read_regexp(1);
            &read_regexp(0);
            &read_int;
        } elsif (s/^stemming//) {
        } else {
            last;
        }
    }
}

sub read_indexspecs {
    &read_indexspec(1);
    while (&read_indexspec(0)) {};
}
sub read_indexspec {
    my($required) = @_;

    &skip;
    if (s/^(TEXT|SOUNDEX|PHONIX)//) {
        &skip;
        unless (s/^(GLOBAL|LOCAL|BOTH)//) {
            die "$.: 'GLOBAL', 'LOCAL', or 'BOTH' expected looking at \"$_\"";
        } else {
            return(1);
        }
    } elsif ($required) {
        die "$.: 'TEXT', 'SOUNDEX', or 'PHONIX' expected looking at \"$_\"";
    } else {
        return 0;
    }
}

sub read_field {
    &skip;
    unless (s/^(\w+)//) {
        die "$.: field name  expected looking at \"$_\"";
    }
    &skip;
    if (s/^"[^\"]+"//) {
        $description = $1;
    }
}
!NO!SUBS!
chmod 755 mkfmt
$eunicefix mkfmt
