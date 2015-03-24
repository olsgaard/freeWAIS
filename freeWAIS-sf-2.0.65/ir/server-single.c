/* WIDE AREA INFORMATION SERVER SOFTWARE	
   No guarantees or restrictions.  See the readme file for the full standard 
   disclaimer.
   5.29.90	Harry Morris, morris@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* this file is a server process for a unix machine that takes input from 
   standard in or from a socket and searches the local search engine on the 
   unix box.
   originally written by harry morris.
   modified by brewster kahle. 7/90
   6.xx.90	Brewster - initial implementation of stdio interface
   7.xx.90	Patrick Bray - support for headers and forking processes
   90.07.31	Ephraim - support for logging 

   91.03.03     Jonathan - set searchLog to log_out.
   91.05.23	Jonathan - added fork process for indexer.
                           Fixed version display so it exits.
   91.05.25     Jonathan - added setuid.
   
   Tue Jul  9 12:11:02 1991 -- Michael Haberler mah@wu-wien.ac.at

                Added semi-intelligent INFO database indexing (only done if
		any of the .src files is newer than INFO.dct)
		
		Locking against multiple concurrent INFO rebuilds if 
		running under inetd

		Use scandir() for directory operations

		Works under inetd as well as standalone. Here are my inetd.conf
   		entries (not the missing userid in the Ultrix inetd.conf!):

   hpux 7.0/800, Interactive/386 2.2.1:
	z3950 stream tcp nowait root /usr/local/etc/waisserver waisserver -s \
		-d /usr/logins/mah/wais-sources

   Ultrix 4.1:
	z3950 stream tcp nowait /usr/local/etc/waisserver waisserver -s \
		-d /usr/logins/mah/wais-sources

   Also, add the next line to /etc/services, and tickle your YP server:
	z3950           210/tcp         # wide area information server (wais)

 * $Log: server-single.c,v $
 * Revision 2.0.1.4  1995/10/20 17:09:38  pfeifer
 * patch40: Removed unconditional reference to sys_errlist.
 *
 * Revision 2.0.1.3  1995/10/04  17:22:42  pfeifer
 * patch20: Replaced call to ulimit() by getrlimit() as suggested by John
 * patch20: Fieber.
 *
 * Revision 2.0.1.3  1995/10/04  17:22:42  pfeifer
 * patch20: Replaced call to ulimit() by getrlimit() as suggested by John
 * patch20: Fieber.
 *
 * Revision 2.0.1.2  1995/09/15  09:47:16  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  12:52:11  pfeifer
 * patch4: Streamlined sys/file.h fcntl.h and sys/fcntl.h stuff.
 * patch4: Hope it still works.
 *
 * Revision 2.0  1995/09/08  08:01:13  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.12  1994/12/13  19:50:18  pfeifer
 * chip@chinacat.unicom.com (Chip Rosenthal) patches.
 * Excluding the merge of libinv and libwais
 *
 * Revision 1.10  1994/10/11  14:39:04  pfeifer
 * convex
 *
 * Revision 1.9  1994/10/06  13:28:31  pfeifer
 * Patch from Mariusz Niewczas
 *
 * Revision 1.8  1994/09/09  09:00:28  pfeifer
 * Fixed the MAX_OCCURANCES - STOP_WORD_FLAG bug
 *
 * Revision 1.7  1994/09/08  16:42:23  pfeifer
 * make BSD happy
 *
 * Revision 1.6  1994/09/06  16:54:42  pfeifer
 * Added #include <netinet/in.h>
 *
 * Revision 1.5  1994/08/08  07:32:50  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.3  1994/05/20  12:57:05  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:06:41  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.8  92/05/10  14:49:06  jonathan
 * Updated for release
 * 
 * Revision 1.7  92/03/05  07:08:23  shen
 * add two more dummy arguments to call to init_search_engine
 * 
 * Revision 1.6  92/02/16  12:32:42  jonathan
 * Removed all code refering to use_stdio, since it's not a valid choice in
 * this server.
 * 
 * Revision 1.5  92/02/12  13:42:04  jonathan
 * Changed server date.
 * 
 * Revision 1.4  92/02/12  13:41:18  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
*/

#define SERVER_DATE "Sun May 10 1992"

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/server-single.c,v 2.0.1.4 1995/10/20 17:09:38 pfeifer Exp $";
#endif

#define INFO_DICT    "INFO.dct"
#define LOCKFILE    "/tmp/INFO.lock" /* while re-indexing INFO */
#define NAPTIME     1		     /* seconds */
#define MAXNAPTIME  60		/* wait up to a minute for indexer to finish */

#include "server.h"
#include "sockets.h"

#include <sys/stat.h>

#ifdef I_SYS_FILE
#include <sys/file.h>
#else /* not I_SYS_FILE */
#ifdef I_FCNTL
#include <fcntl.h>
#else /* not I_FCNTL */
#include <sys/fcntl.h>
#endif /* not I_FCNTL */
#endif /* not I_SYS_FILE */

#ifdef SYSV			
#define SIGCHLD SIGCLD
#endif

#include <signal.h>

#include "panic.h"
#include "ustubs.h"
#include "transprt.h"
#include "wmessage.h"
#include "ir.h"
#include "wprot.h"
#include "cutil.h"
#include "futil.h"
#include "irext.h"

/* to create the INFO index */
#include "irtfiles.h"
#include "irfiles.h"
#include "irhash.h"

#ifdef I_SYS_RESOURCE
#include <sys/resource.h>
#endif

/* for address decoding */

#ifdef I_SYS_SOCKET
#include <sys/socket.h>
#endif

#ifdef I_NETINET_IN
#include <netinet/in.h>
#endif

#ifdef I_ARPA_INET
#include <arpa/inet.h>
#endif

#include <setjmp.h>

#ifdef I_SYS_SELECT
#include <sys/select.h>
#endif

static jmp_buf jbuf;
static long bufferSize = BUFSZ; /* how much we are using
                                   (we get one of these per process) */

static FILE *current_client;
static struct itimerval new, old;

/*---------------------------------------------------------------------------*/
#define TIMEOUT_LENGTH 5 /* 5 second timeout. 
			    Since this is only in the handle - client code,
			    I don't want it to wait too long.  
			    The bytes should come quickly
			  */
#define IDLE_TIME "5 seconds"

long
 handle_client(in,out, index_directory)
FILE* in;
FILE* out;
char *index_directory;
{ 
  char buf[BUFSZ];		/* contains the message and header */
  char *bufPtr ;		/* points at the begining of the z3950 */
  long size;			/* bytes in the z3950 message */
  WAISMessage header;		/* for storing the header */
  long i;
  long bytesLeft;
  long nextChar;
  int  jmpres;

  new.it_interval.tv_sec = 0;
  new.it_interval.tv_usec = 0;
  new.it_value.tv_sec = TIMEOUT_LENGTH;
  new.it_value.tv_usec = 0;

  if ((jmpres = setjmp(jbuf)) != 0) {
    if (jmpres == 2) return EOF; /* because we're here by timeout */
  } else {
    /* try to read the header */
    for (i = 0; i < HEADER_LENGTH; i++)
      { 
	setitimer(ITIMER_REAL, &new, NULL);
	nextChar = fgetc(in);
	if (nextChar == EOF)	/* my connection exited, so will I */
	  { 
	    setitimer(ITIMER_REAL, &old, NULL);
	    return EOF;
	  }
	else
	  buf[i] = (char)nextChar;
      }
    setitimer(ITIMER_REAL, &old, NULL);
    /* parse the header */
    readWAISPacketHeader(buf,&header);

    /* make sure we have the right version.  
       If we dont, we dont know what to do. */
    if (header.hdr_vers > HEADER_VERSION)
      panic("Incompatable header versions (Current version: %d, supplied version: %d.", 
	    HEADER_VERSION, header.hdr_vers) ;

    /* determine the size of the z3950 message */
    {
      char length_array[11];
      strncpy(length_array, header.msg_len, 10);
      length_array[10] = '\0';
      size = atol(length_array);
    }

    /* set bufPtr to start the z3950 message */
    bufPtr = buf + HEADER_LENGTH ;

    /* read the z3950 message */
    for (i = 0; i < size ; i++) {
      setitimer(ITIMER_REAL, &new, NULL);
      if ((buf[i + HEADER_LENGTH] = (char)fgetc(in)) == EOF)
	return -1;
    }

    setitimer(ITIMER_REAL, &old, NULL);
    rewind(in);

    /* decode the z3950 if necessary */
    transportDecode((long)header.encoding,bufPtr,&size);
     
    /* XXX handle compression options */

    /* process it the z3950 */
    bytesLeft = bufferSize;

    size = interpret_buffer(bufPtr,size,bufPtr,bytesLeft,
			    &bufferSize,(long)header.hdr_vers,
			    index_directory); 

    /* re-encode the message if necessary */
    transportCode((long)header.encoding,bufPtr,&size); 

    /* XXX handle compression options */

    /* write the new header */
    writeWAISPacketHeader(buf,size,
			  (long)header.msg_type,header.server,
			  (long)header.compression,(long)header.encoding,
			  (long)header.hdr_vers);

    /* write the whole response to the output file */
    for (i = 0; i < size + HEADER_LENGTH; i++)
      fputc(buf[i],out) ;

    fflush(out);		/* flush any file buffers */
    rewind(out);

    return 0;
  }
}

/*---------------------------------------------------------------------------*/

#ifndef ISC
static void breakKey _AP((long s1,long s2,struct sigcontext* s3,char* s4));
#endif

static void
breakKey (s1,s2,s3,s4)
long s1;
long s2;
struct sigcontext *s3;
char *s4;
{
  waislog(WLOG_HIGH, WLOG_ERROR, "got a ^c");
  exit (-1);
}

/*---------------------------------------------------------------------------*/

void
childhandler(sig, code, scp, addr)
long sig, code;
struct sigcontext *scp;
char *addr;
{
  wait(NULL);			/* give the kid a decent burial */
  signal (SIGCHLD, childhandler);  /* Dave Judd - IRIX requires resetting signal */
}

/*---------------------------------------------------------------------------*/

void
server_alarmhandler(sig, code, scp, addr)
long sig, code;
struct sigcontext *scp;
char *addr;
{
  setitimer(ITIMER_REAL, &old, NULL);
  if(current_client != NULL) {
    waislog(WLOG_HIGH, WLOG_CLOSE,
	    "Client idle longer %s during Read - Closing client connection.", IDLE_TIME);
    longjmp(jbuf, 2);
  }
  else  {
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "Bogus timer signal.  What's the deal?");
  }
}

/*---------------------------------------------------------------------------*/

void
seghandler(sig, code, scp, addr)
long sig, code;
struct sigcontext *scp;
char *addr;
{
  waislog(WLOG_HIGH, WLOG_CLOSE,
	  "Segmentation violation.  Bummer. Closing server and exiting.");
  exit(0);
}

/*---------------------------------------------------------------------------*/

void
bushandler(sig, code, scp, addr)
long sig, code;
struct sigcontext *scp;
char *addr;
{
  waislog(WLOG_HIGH, WLOG_CLOSE,
	  "Bus error.  Bummer. Closing server and exiting.");
  exit(0);
}

/*---------------------------------------------------------------------------*/

#ifdef I_PWD
#include <pwd.h>
#endif

int finduid(name)
char *name;
{
  struct passwd *pwent;

  if ((pwent = getpwnam(name)) == NULL) {
    return -1;
  }

  return(pwent->pw_uid);
}

static  char *index_dir = NULL;
static  time_t info_change_time;
static  int indexing_needed = 0;
static  char *info_dict = INFO_DICT;

extern int alphasort();

/* selecttion function for scandir()
 * trigger on ".src" extension, regular file, and != "INFO.src"
 * Indexing is needed if any of the .src files is younger than 
 * INFO.dct
 */
static int
srcfiles(e)
	Direntry_t *e;
{
	struct stat sb;
	char *lastdot = strrchr(e->d_name,'.');
	int candidate;

	candidate =	lastdot && 
	      (stat(merge_pathnames(e->d_name,index_dir), &sb) >= 0) && 
	      ((sb.st_mode & S_IFMT) == S_IFREG) &&
	      !strcmp(lastdot,source_ext) && 
	      strcmp(e->d_name,info_dict); /* whew */

        if (candidate) {
	    indexing_needed |= (sb.st_mtime > info_change_time);
	    return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/time.h>

typedef struct _client_connection {
  FILE *file;
  long buffersize;
  long pid;
  long line;
} client_connection, *Client_connection;

void
main(argc,argv)
int argc;
char* argv[];
{ 
  fd_set fds;
  FILE *file;
  client_connection clients[200];
  int numclients, i;
  long socket;
  char *next_argument = next_arg(&argc, &argv), *command_name;
  long tcp_port = 210;		/* tcp_port to use */
  /* char *wais_log_file_name = NULL; */	/* name of file for error output */
  int child_proc;		/* for the child process id */
  char *uid_name = "root";	/* user id so setuid if root */
  int uid = 0;			/* if not specified, leave as root. */
  long cm_mem_percent = 0;  	/* default */
  int child,lockfd;
  struct stat statbuf;
  Direntry_t **list;
  int n_files,fd;    
  int naptime = 0;
  extern int errno;
  extern char *sys_errlist[];
  static long current_id = 1, current_log_line = 0;
  dataopsrec	dataops;  

  command_name = next_argument;

/* dgg -- must duplicate mods to irbuild.c, here is mini-build of INFO.src */
  dataops.separator_function= NULL;
  dataops.header_function= NULL;
  dataops.date_function= NULL;
  dataops.finish_header_function= NULL;
  dataops.type= "WSRC";
  dataops.wordDelimiter= wordbreak_notalnum;
  dataops.addseparatorwords= false;
  dataops.extraheaderweight= true;
  dataops.repeat_weight= 1;
  dataops.minwordlen= 2;
  stop_list_file("\0");
  gDelimiters[0]= '\0';  
  wordDelimiter= wordbreak_notalnum;   
/* dgg -- end new inits */

  getitimer(ITIMER_REAL, &old);
  for(i = 0; i < 200; i++)
    clients[i].file = NULL;

  if (argc == 0){
    printf("Usage: %s [-p [port_number]] [-d directory] [-u user] [-v] [-cmmem percent]\n",
	   command_name);
    printf(" -p [port] listen to the port.  If the port is supplied, then\n");
    printf("    that tcp_port number is used.  If it is not supplied \n");
    printf("    then the Z39.50 port (210) is used.\n");
    printf(" -d directory: means to use the directory as the source of databases.\n");
    printf("    Defaults to the current directory.\n");
    printf(" -e [file]: set log output to file, or /dev/null if not specified.\n");
    printf(" -u user: if started as root, setuid to user after startup.\n");
    printf(" -cmmem number: percentage of CM memory to use (CM code only).\n");
    printf(" -v prints the version.\n");
    exit(1);
  }
  if(NULL == (next_argument = next_arg(&argc, &argv))){
    printf("No arguments specified\n");
    exit(0);
  }
  while((next_argument != NULL) &&
	('-' == next_argument[0])){
    /* then we have an argument to process */
    if (0 == strcmp("-p", next_argument)){
      char *peek_argument = peek_arg(&argc, &argv);
      if ((NULL != peek_argument) && /* if we are not out of args */
	  ('-' != peek_argument[0])){ { /* and the next isn't an option... */
	    /* get the port number */
	    tcp_port = atoi(next_arg(&argc, &argv));
	  }			/* end if (explicit tcp_port) */
				    }
    }				/* end if (-p) */

    else if (0 == strcmp("-e", next_argument)) {
      char *peek_argument = peek_arg(&argc, &argv);
      wais_log_file_name = "/dev/null"; /* default to /dev/null */
      if ((peek_argument != NULL) &&
	  ('-' != peek_argument[0])) {
	wais_log_file_name = next_arg(&argc, &argv);
      }				/* end if (explicit log file) */
    }				/* end if (-e) */
    else if (0 == strcmp("-d", next_argument)) {
      index_dir = next_arg(&argc, &argv);
    }
    else if (0 == strcmp("-v", next_argument)) {
      printf("%s: %s, %s\n", command_name, VERSION_STRING, SERVER_DATE);
    }
    else if (0 == strcmp("-u", next_argument)) {
      uid_name = next_arg(&argc, &argv);
      if((uid = finduid(uid_name)) < 0)
	panic("Couldn't find user %s.", uid_name);
    }
    else if(0 == strcmp("-cmmem", next_argument)){
      if(NULL == (next_argument = next_arg(&argc, &argv)))
	panic("Expected a number (1-100) for percentage of memory to use");
      cm_mem_percent = atol(next_argument);
      if(cm_mem_percent < 1)
	panic("The -cmmem argument should not be less than 1 and less than 100");
      if(cm_mem_percent > 100)
	panic("Warning: The -cmmem parameter was %ld%%. It should be between 1-100.", cm_mem_percent);
    }
    else{
      panic("Don't recognize the %s option", next_argument);
    }
    next_argument = next_arg(&argc, &argv);
  }				/* end while (more arguments) */

  if (wais_log_file_name == NULL) {
    wais_log_file_name = "/dev/null";
    waislogfile = stderr;
  }
  else waislogfile = NULL;
  
  if(0 != init_search_engine(index_dir, false, true, cm_mem_percent,0,0))
    panic("unable to initialize search engine");

#ifdef AUTO_INDEX

  index_dir = index_dir ? index_dir : ".";  
  info_dict = s_strdup(merge_pathnames(info_dict,index_dir));
  
  /* remember timestamp on INFO.dct if rebuilding needed 
   * If it doesnt exist, it's assumed to be *very* old, to force
   * re-indexing
   */
  info_change_time = (stat(info_dict,&statbuf) == -1) ? 0 : statbuf.st_mtime;
  
  /* compare with candidates */

  if ((n_files = scandir(index_dir, &list, srcfiles, alphasort)) < 0) {
    waislog(WLOG_HIGH, WLOG_ERROR, 
	    "Error: reading directory %s, %s", 
	    index_dir, Strerror(errno));
    exit(1);
  }
  
  /* ok. we know if we need indexing, 
   * and have all the filenames. 
   */
  
  if ((info_change_time == 0) || indexing_needed) {

    /* Time to re-index,
     * aquire the lock 
     */
    waislog(WLOG_MEDIUM, WLOG_INDEX,
	    "re-indexing needed, info_change_time=%d",info_change_time); 

    if (( fd = open(LOCKFILE, O_WRONLY|O_CREAT|O_EXCL,0666)) == -1) {
	  
      /* already locked by somebody else
       * spin  till she finishes
       */
      while (!(stat(LOCKFILE,&statbuf) == -1)) {
	sleep(NAPTIME);
	naptime += NAPTIME;
	waislog(WLOG_MEDIUM, WLOG_INFO,
		"INFO locked, waiting since %d seconds", naptime);
	if (naptime  > MAXNAPTIME)  {
	  waislog(WLOG_HIGH, WLOG_ERROR,
		  "Warning - lockfile %s wont go away after %d seconds, exiting", 
		  LOCKFILE, naptime);
	  exit(1);		/* XXX be more perseverant */
	}
      }
      /* lockfile went away, assume INFO.* build finished
       * so just use it
       */
    } else {			/* we aquired the lock, so rebuild database  */
	  
      if (!(child = fork())) {
	database *db;
	Direntry_t **s = list;
	char filename[MAX_FILENAME_LEN];
	      
	waislog(WLOG_MEDIUM, WLOG_INDEX,
		"Creating INFO database, pid=%d",getpid());
	db = openDatabase(merge_pathnames("INFO",	index_dir),
			  true, /* maybe this should append XXX */
			  false);
	db->the_word_memory_hashtable =
	  init_word_memory_hashtable(1L<<16, 100000, db->the_word_memory_hashtable);
	      
	while (*s) {		/* index it */
	  strncpy(filename, index_dir, MAX_FILENAME_LEN);
	  if(index_dir[strlen(index_dir) -1] != '/')
	    strncat(filename, "/", MAX_FILENAME_LEN);
	  strncat(filename, (*s)->d_name, MAX_FILENAME_LEN);
	  waislog(WLOG_MEDIUM, WLOG_INDEX,
		  "Indexing %s", filename);
	  index_text_file(filename, &dataops, db, true, false);
	  s++;
	}
	freedir(list);		/* array of filenames */
	      
	if(!probe_file(source_filename(filename, db)))
	  write_src_structure(source_filename(filename, db),
			      "INFO", "WSRC", NULL, 0, true, tcp_port);
	finished_add_word(db);
	closeDatabase(db);
	if (unlink(LOCKFILE))
	  panic("Indexer: cant unlink lockfile!\n");
	waislog(WLOG_MEDIUM, WLOG_INDEX,
		"Indexer pid=%d done", getpid());
	      
	exit(0);		/* indexing child */

      }  else if (child == -1) {
	waislog(WLOG_HIGH, WLOG_ERROR,
		"Unable to fork for indexer.");
	exit(1);
      }
      /* wait for child process */
      else while (wait(0) != child) ; /* do nothing */
    }
  }

#endif				/* AUTO_INDEX */

  waislog(WLOG_MEDIUM, WLOG_INFO, "Running server %s", VERSION_STRING);

  signal(SIGINT, breakKey);

  signal(SIGCHLD, childhandler); /* XXX dont really need this any more */
  signal(SIGALRM, server_alarmhandler);

  signal(SIGSEGV, seghandler);

  signal(SIGBUS, bushandler);

  open_server(tcp_port,&socket,BUFSZ);

#ifdef SECURE_SERVER
    /* if root, setuid to user specified id. */
    if (uid > 0 && getuid() == 0)  {
      waislog(WLOG_MEDIUM, WLOG_INFO,
	      "Setting uid to %s.", uid_name);
      if ( 0 > setuid(uid)) {
	waislog(WLOG_HIGH, WLOG_ERROR,
		"Unable to setuid to %s!  Exiting.", uid_name);
	exit(-1);
      }
    }
#endif
    while (TRUE) {		/* be a server for several connections */
      int active, width;
      struct rlimit rlp;
      
      getrlimit(RLIMIT_NOFILE,&rlp);
      width = rlp.rlim_cur;

      FD_ZERO(&fds);
      FD_SET(socket, &fds);
      wais_pid = 0;
      for (i = 0; i < 200; i++) {
	if (clients[i].file != NULL)
	  FD_SET(fileno(clients[i].file), &fds);
      }
      if((active = select(width, &fds, NULL, NULL, NULL)) < 1) {
	perror ("Select: ");
	waislog(WLOG_HIGH, WLOG_ERROR, 
		"select returned an error!");
      }
      else {
	/* this is a connection on the socket. */
	if (FD_ISSET(socket, &fds)) {
	  for(i = 0; i < 200; i++) {
	    if (clients[i].file == NULL) {
	      current_log_line++;
	      accept_client_connection(socket, &clients[i].file);
	      current_client = clients[i].file;
	      clients[i].buffersize = BUFSZ;
	      wais_pid = clients[i].pid = current_id++;
	      log_line = clients[i].line = 0;
	      if (handle_client(current_client, current_client, index_dir) == -1)  {
		close_client_connection(current_client);
		current_client = clients[i].file = NULL;
		waislog(WLOG_MEDIUM, WLOG_CLOSE,
			"Done handling client");
		log_line = current_log_line;
	      }
	      else {
		setitimer(ITIMER_REAL, &old, NULL);
		clients[i].buffersize = bufferSize;
		clients[i].line = log_line;
		log_line = current_log_line;
		wais_pid = 0;
	      }

	      break;
	    }
	  }
	}
	/* this is for an established connection */

	for(i = 0; i < 200; i++) {
	  if(clients[i].file != NULL && FD_ISSET(fileno(clients[i].file), &fds)) {
	    current_client = clients[i].file;
	    bufferSize = clients[i].buffersize;
	    wais_pid = clients[i].pid;
	    log_line = clients[i].line;
	    if (handle_client(current_client, current_client, index_dir) == -1) {
	      close_client_connection(current_client);
	      current_client = clients[i].file = NULL;
	      waislog(WLOG_MEDIUM, WLOG_CLOSE,
		      "Done handling client");
	      wais_pid = 0;
	      log_line = current_log_line;
	    }
	    else setitimer(ITIMER_REAL, &old, NULL);
	    clients[i].line = log_line;
	  }
	}
      }
    }
}	

/*---------------------------------------------------------------------------*/

