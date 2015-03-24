#!/bin/csh -f

# test to see if xwais can be made on this system

# check for xmkmf

if (! $?CC) set CC=cc
if (! $?CFLAGS) set CFLAGS="-g -I. -I../ui -I../ir -I../lib -I/usr/local/include"
if (! $?TOP) set TOP=../
if (! $?MAKE) set MAKE="make -k"

set EXTRA_INCLUDES="$CFLAGS -I$TOP/x -I$TOP/ui"
set my_name = xmkmf

foreach dir ($path)
  if (-f $dir/$my_name) then
    set my_dir=$dir
  endif
end

if ($?my_dir) then
 echo "Making xwais..."
 xmkmf
 echo $MAKE CC=$CC "EXTRA_INCLUDES=$EXTRA_INCLUDES" TOP=$TOP all
 $MAKE CC=$CC "EXTRA_INCLUDES=$EXTRA_INCLUDES" TOP=$TOP all
else
 echo "Couldn't find xmkmf, you probably can't make X programs here."
 
endif

