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
echo "Extracting config.h (with variable substitutions)"
sed <<!GROK!THIS! >config.h -e 's!^#undef!/\*#define!' -e 's!^#un-def!#undef!'
/*
 * This file was produced by running the config_h.SH script, which
 * gets its values from config.sh, which is generally produced by
 * running Configure.
 *
 * Feel free to modify any of this as the need arises.  Note, however,
 * that running config_h.SH again will wipe out any changes you've made.
 * For a more permanent change edit config.sh and rerun config_h.SH.
 *
 * \$Id: config_h.SH,v 2.0.1.8 1995/10/20 17:07:28 pfeifer Exp $
 */

/* Configuration time: $cf_time
 * Configured by: $cf_by
 * Target system: $myuname
 */

#ifndef _config_h_
#define _config_h_

/* BSD:
 *	This symbol, if defined, indicates that the program is running under
 *	a BSD system.
 */
#$d_bsd BSD		/**/

/* BYTEORDER:
 *	This symbol hold the hexadecimal constant defined in byteorder,
 *	i.e. 0x1234 or 0x4321, etc...
 */
#define BYTEORDER 0x$byteorder	/* large digits for MSB */

/* HAS_BCOPY:
 *	This symbol is defined if the bcopy() routine is available to
 *	copy blocks of memory.
 */
#$d_bcopy HAS_BCOPY	/**/

/* HAS_BZERO:
 *	This symbol is defined if the bzero() routine is available to
 *	set a memory block to 0.
 */
#$d_bzero HAS_BZERO	/**/

/* HASCONST:
 *	This symbol, if defined, indicates that this C compiler knows about
 *	the const type. There is no need to actually test for that symbol
 *	within your programs. The mere use of the "const" keyword will
 *	trigger the necessary tests.
 */
#$d_const HASCONST	/**/
#ifndef HASCONST
#define const
#endif

/* HAS_GETHOSTENT:
 *	This symbol, if defined, indicates that the gethostent routine is
 *	available to lookup host names in some data base or other.
 */
#$d_gethent HAS_GETHOSTENT		/**/

/* HAS_GETWD:
 *	This symbol, if defined, indicates that the getwd routine is
 *	available to get working directory. If not, you should
 *	probably use getcwd().
 */
#$d_getwd HAS_GETWD		/**/

/* HAS_MEMMOVE:
 *	This symbol, if defined, indicates that the memmove routine is available
 *	to copy potentially overlapping blocks of memory. This should be used
 *	only when HAS_SAFE_BCOPY is not defined. If neither is there, roll your
 *	own version.
 */
#$d_memmove HAS_MEMMOVE	/**/

/* HAS_SAFE_BCOPY:
 *	This symbol, if defined, indicates that the bcopy routine is available
 *	to copy potentially overlapping memory blocks. Otherwise you should
 *	probably use memmove() or memcpy(). If neither is defined, roll your
 *	own version.
 */
#$d_safebcpy HAS_SAFE_BCOPY	/**/

/* HAS_SAFE_MEMCPY:
 *	This symbol, if defined, indicates that the memcpy routine is available
 *	to copy potentially overlapping memory blocks. Otherwise you should
 *	probably use memmove() or memcpy(). If neither is defined, roll your
 *	own version.
 */
#$d_safemcpy HAS_SAFE_MEMCPY	/**/

/* HAS_SCANDIR:
 *	This symbol, if defined, indicates that the scandir routine is
 *	available to scan a directory.
 */
#$d_scandir HAS_SCANDIR	/**/

/* HAS_INDEX:
 *	This symbol is defined to indicate that the index()/rindex()
 *	functions are available for string searching.
 */
#$d_index HAS_INDEX	/**/

/* HAS_STRCSPN:
 *	This symbol, if defined, indicates that the strcspn routine is
 *	available to scan strings.
 */
#$d_strcspn HAS_STRCSPN		/**/

/* HAS_STRDUP:
 *	This symbol, if defined, indicates that the strdup routine is
 *	available to duplicate strings in memory. Otherwise, roll up
 *	your own...
 */
#$d_strdup HAS_STRDUP		/**/

/* HAS_SYS_ERRLIST:
 *	This symbol, if defined, indicates that the sys_errlist array is
 *	available to translate error numbers to strings. The extern int
 *	sys_nerr gives the size of that table.
 */
/* Strerror:
 *	This preprocessor symbol is defined as a macro if strerror() is
 *	not available to translate error numbers to strings but sys_errlist[]
 *	array is there.
 */
#$d_syserrlst HAS_SYS_ERRLIST	/**/
#define Strerror(e) $d_strerrm

/* HAS_SYSLOG:
 *	This symbol, if defined, indicates that the program can rely on the
 *	system providing syslog().  Otherwise, the syslog code provided by
 *	the package should be used.
 */
#$d_syslog HAS_SYSLOG	/**/

/* Time_t:
 *	This symbol holds the type returned by time(). It can be long,
 *	or time_t on BSD sites (in which case <sys/types.h> should be
 *	included).
 */
#define Time_t $timetype		/* Time type */

/* I_NDIR:
 *	This symbol, if defined, indicates that the program should include the
 *	system's version of ndir.h, rather than the one with this package.
 */
#$i_ndir I_NDIR		/**/

/* Signal_t:
 *	This symbol's value is either "void" or "int", corresponding to the
 *	appropriate return type of a signal handler.  Thus, you can declare
 *	a signal handler using "Signal_t (*handler)()", and define the
 *	handler using "Signal_t handler(sig)".
 */
#define Signal_t $signal_t	/* Signal handler's return type */

/* HASVOLATILE:
 *	This symbol, if defined, indicates that this C compiler knows about
 *	the volatile declaration.
 */
#$d_volatile	HASVOLATILE	/**/
#ifndef HASVOLATILE
#define volatile
#endif

/* HAS_VPRINTF:
 *	This symbol, if defined, indicates that the vprintf routine is available
 *	to printf with a pointer to an argument list.  If unavailable, you
 *	may need to write your own, probably in terms of _doprnt().
 */
#$d_vprintf HAS_VPRINTF	/**/

/* I_ARPA_INET:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <arpa/inet.h> to get inet_addr and friends declarations.
 */
#$i_arpainet	I_ARPA_INET		/**/

/* I_DIRENT:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <dirent.h>. Using this symbol also triggers the definition
 *	of the Direntry_t define which ends up being 'struct dirent' or
 *	'struct direct' depending on the availability of <dirent.h>.
 */
/* DIRNAMLEN:
 *	This symbol, if defined, indicates to the C program that the length
 *	of directory entry names is provided by a d_namlen field.  Otherwise
 *	you need to do strlen() on the d_name field.
 */
/* Direntry_t:
 *	This symbol is set to 'struct direct' or 'struct dirent' depending on
 *	whether dirent is available or not. You should use this pseudo type to
 *	portably declare your directory entries.
 */
#$i_dirent I_DIRENT		/**/
#$d_dirnamlen DIRNAMLEN	/**/
#define Direntry_t $direntrytype

/* I_FCNTL:
 *	This manifest constant tells the C program to include <fcntl.h>.
 */
#$i_fcntl I_FCNTL	/**/

/* I_MEMORY:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <memory.h>.
 */
#$i_memory I_MEMORY		/**/

/* I_NETINET_IN:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <netinet/in.h>. Otherwise, you may try <sys/in.h>.
 */
#$i_niin I_NETINET_IN	/**/

/* I_PWD:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <pwd.h>.
 */
#$i_pwd I_PWD		/**/

/* I_STDLIB:
 *	This symbol, if defined, indicates that <stdlib.h> exists and should
 *	be included.
 */
#$i_stdlib I_STDLIB		/**/

/* I_STRING:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <string.h> (USG systems) instead of <strings.h> (BSD systems).
 */
#$i_string I_STRING		/**/

/* I_SYS_DIR:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/dir.h>.
 */
#$i_sysdir I_SYS_DIR		/**/

/* I_SYS_FILE:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/file.h> to get definition of R_OK and friends.
 */
#$i_sysfile I_SYS_FILE		/**/

/* I_SYS_NDIR:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/ndir.h>.
 */
#$i_sysndir I_SYS_NDIR	/**/

/* I_SYS_PARAM:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/param.h>.
 */
#$i_sysparam I_SYS_PARAM		/**/

/* I_SYS_RESOURCE:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/resource.h>.
 */
#$i_sysresrc I_SYS_RESOURCE		/**/

/* I_SYS_SELECT:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/select.h> in order to get definition of struct timeval.
 */
#$i_sysselct I_SYS_SELECT	/**/

/* I_SYS_SOCKET:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/socket.h> before performing socket calls.
 */
#$i_syssock I_SYS_SOCKET		/**/

/* I_SYS_TYPES:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/types.h>.
 */
#$i_systypes	I_SYS_TYPES		/**/

/* I_TERMIO:
 *	This symbol, if defined, indicates that the program should include
 *	<termio.h> rather than <sgtty.h>.  There are also differences in
 *	the ioctl() calls that depend on the value of this symbol.
 */
/* I_TERMIOS:
 *	This symbol, if defined, indicates that the program should include
 *	the POSIX termios.h rather than sgtty.h or termio.h.
 *	There are also differences in the ioctl() calls that depend on the
 *	value of this symbol.
 */
#$i_termio I_TERMIO		/**/
#$i_termios I_TERMIOS		/**/

/* I_TIME:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <time.h>.
 */
/* I_SYS_TIME:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <sys/time.h>.
 */
#$i_time I_TIME		/**/
#$i_systime I_SYS_TIME		/**/

/* I_UNISTD:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <unistd.h>.
 */
#$i_unistd I_UNISTD		/**/

/* I_STDARG:
 *	This symbol, if defined, indicates that <stdarg.h> exists and should
 *	be included.
 */
/* I_VARARGS:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <varargs.h>.
 */
#$i_stdarg I_STDARG		/**/
#$i_varargs I_VARARGS	/**/

/* INTSIZE:
 *	This symbol contains the size of an int, so that the C preprocessor
 *	can make decisions based on it.
 */
#define INTSIZE $intsize		/**/

/* LONGSIZE:
 *	This symbol contains the size of a long, so that the C preprocessor
 *	can make decisions based on it.
 */
#define LONGSIZE $longsize		/**/

/* CAN_PROTOTYPE:
 *	If defined, this macro indicates that the C compiler can handle
 *	function prototypes.
 */
/* _:
 *	This macro is used to declare function parameters for folks who want
 *	to make declarations with prototypes using a different style than
 *	the above macros.  Use double parentheses.  For example:
 *
 *		int main _((int argc, char *argv[]));
 */
#$prototype	CAN_PROTOTYPE	/**/
#ifdef CAN_PROTOTYPE
#define	_(args) args
#else
#define	_(args) ()
#endif

/* Uid_t:
 *	This symbol holds the type used to declare user ids in the kernel.
 *	It can be int, ushort, uid_t, etc... It may be necessary to include
 *	<sys/types.h> to get any typedef'ed information.
 */
#define Uid_t $uidtype		/* UID type */

/* VOIDFLAGS:
 *	This symbol indicates how much support of the void type is given by this
 *	compiler.  What various bits mean:
 *
 *	    1 = supports declaration of void
 *	    2 = supports arrays of pointers to functions returning void
 *	    4 = supports comparisons between pointers to void functions and
 *		    addresses of void functions
 *	    8 = suports declaration of generic void pointers
 *
 *	The package designer should define VOIDUSED to indicate the requirements
 *	of the package.  This can be done either by #defining VOIDUSED before
 *	including config.h, or by defining defvoidused in Myinit.U.  If the
 *	latter approach is taken, only those flags will be tested.  If the
 *	level of void support necessary is not present, defines void to int.
 */
#ifndef VOIDUSED
#define VOIDUSED $defvoidused
#endif
#define VOIDFLAGS $voidflags
#if (VOIDFLAGS & VOIDUSED) != VOIDUSED
#define void int		/* is void to be avoided? */
#define M_VOID			/* Xenix strikes again */
#endif

/* HAS_ALLOCA:
 *	This symbol, if defined, indicates that the alloca routine is available
 *	to alloca files.
 */
/* I_ALLOCA:
 *	This symbol, if defined, indicates to the C program that it should
 *	include <alloca.h>.
 */
#$d_alloca HAS_ALLOCA	/**/
#$i_alloca I_ALLOCA	/**/

/* HAS_ALPHASORT:
 *	This symbol, if defined, indicates that the alphasort routine is available
 *	to alphasort files.  Otherwise you should do the unlink().
 */
#$d_alphasort HAS_ALPHASORT	/**/

/* CACHE_SYN:
 *     Use the shared memory support?
 */
#$d_cache_syn CACHE_SYN /**/

/* COMPILER_VERSION:
 *     The compiler version as given by the -v option
 */
#define COMPILER_VERSION $d_compiler_version 

/* UNCOMPRESSOR:
 *     Path to the unix uncomressing utility (usually zcat). UNZIPPER
 *     points to the gnu uncompressor gzcat.
 */
/* UNZIPPER:
 *     Path to the GNU uncomressing utility (usually gzcat).
 */
#define UNCOMPRESSOR $d_uncompressor
#define UNZIPPER $d_unzipper

/* EXTERN_SYS_ERRLIST:
 *     Do we need a extern declaration of sys_errlist?
 */
#$d_extern_sys_errlist EXTERN_SYS_ERRLIST /**/

/* HAS_GETPWUID:
 *	This symbol, if defined, indicates that the getpwuid routine is
 *	available.
 */
#$d_getpwuid HAS_GETPWUID	/**/

/* HUGE_HEADLINES:
 *     Compile with option to run local searches?
 *     Additional args for waissearch when compiled without HUGE_HEADLINES.
 */
#$d_huge_headlines HUGE_HEADLINES /**/

/* LOCAL_SEARCH:
 *     Compile with option to run local searches?
 *     Additional args for waissearch when compiled without LOCAL_SEARCH.
 */
#$d_local_search LOCAL_SEARCH /**/

/* MYCTYPE:
 *     Use the included ctype if user wants to. Also defines LCHARS
 *     and UCHARS by querying the user for character lists. CTYPEVAR
 *     is defined if the name of the ctype array is _ctype_.
 */
#$d_myctype MYCTYPE /**/
#define LCHARS $lchars
#define UCHARS $uchars
#$d_ctypevar CTYPEVAR /**/

/* MYREGEXP:
 *     Use the included regexps?
 */
#$d_myregexp MYREGEXP /**/

/* PROXIMITY:
 *     Use proximity instead of string search
 */
#$d_proximity PROXIMITY /**/

/* HAS_REMOVE:
 *	This symbol, if defined, indicates that the remove routine is available
 *	to remove files.  Otherwise you should do the unlink().
 */
#$d_remove HAS_REMOVE	/**/

/* DO_NOT_TELL_ABOUT_ME:
 *     Disable the UDp packet sending to maintainer.
 */
#$d_udping DO_NOT_TELL_ABOUT_ME /**/

/* URLDOCID:
 *     Use modified URL handling?
 */
#$d_urldocid URLDOCID /**/

/* VERSION_STRING:
 *     Version string announced to the world
 */
#define VERSION_STRING $d_version_string 

#endif
!GROK!THIS!
