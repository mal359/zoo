#ifndef LINT
static char bsdid[]="@(#) bsd.c 2.3 88/01/10 14:45:19";
#endif /* LINT */

/* machine.c for 4.3BSD. */

/*
The contents of this file are hereby released to the public domain.

                                    -- Rahul Dhesi  1987/07/23
*/

/* 
WARNING:  This file assumes that ZOOFILE is a standard buffered
file.  It will have to be modified if ZOOFILE is changed to
be an unbuffered file descriptor or to any other kind of file.
*/

#ifdef UNBUF_IO
/*
Function tell() returns the current seek position for a file 
descriptor.  4.3BSD on VAX-11/785 has an undocumented tell() function
but it may not exist on all implementations, so we code one here
to be on the safe side.  It is needed for unbuffered I/O only.
*/
long lseek PARMS ((int, long, int));
long tell (fd)
int fd;
{ return (lseek (fd, 0L, SEEK_CUR)); }
#endif

long ftell();

/****************
Function fixfname() converts the supplied filename to a syntax
legal for the host system.  It is used during extraction.
*/

char *fixfname(fname)
char *fname;
{
   return fname; /* default is no-op */
}

/****************
Date and time functions are standard UNIX-style functions.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

/* Standard UNIX-compatible time routines */
#include "nixtime.i"

/* Standard UNIX-specific file attribute routines */
#include "nixmode.i"

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(linux)
#include <unistd.h>
#else
#ifndef SEEK_CUR
# define  SEEK_CUR    1
#endif
#endif

/* Function isadir() returns 1 if the supplied handle is a directory, 
else it returns 0.  
*/

int isadir (f)
ZOOFILE f;
{
   struct stat buf;           /* buffer to hold file information */
   if (fstat (fileno (f), &buf) == -1) {
      return (0);             /* inaccessible -- assume not dir */
   } else {
      if (buf.st_mode & S_IFDIR)
         return (1);
      else
         return (0);
   }
}

/* Function gettz() returns the offset from GMT in seconds */
long gettz(t)
long t;
{
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
   return -localtime(&t)->tm_gmtoff;
#else
#define SEC_IN_DAY	(24L * 60L * 60L)
#define INV_VALUE		(SEC_IN_DAY + 1L)
	static long retval = INV_VALUE;	     /* cache, init to impossible value */
   time_t lt;
   struct tm *tm;
	if (retval != INV_VALUE)				 /* if have cached value, return it */
		return retval;
		
	time(&lt);
	tm = localtime(&lt);
#if defined(__sun)
	retval = 0;
#else
	retval = -tm->tm_gmtoff;
#endif
	return retval;
#endif
}

/* Truncate a file. */
int zootrunc(f) FILE *f;
{
	extern off_t lseek();
	off_t seekpos;
	int fd = fileno(f);
	seekpos = lseek(fd, 0L, SEEK_CUR);
	if (seekpos >= 0)
		return ftruncate(fd, seekpos);
	return 0;
}
