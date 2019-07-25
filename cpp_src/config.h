#ifndef __CONFIG_H
#define __CONFIG_H


#define redis_fstat fastat
#define redis_stat stat


#define HAVE_PROC_STAT 1
#define HAVE_PROC_MAPS 1
#define HAVE_PROC_SMAPS 1

#define HAVE_EPOLL 1

#define aof_fsync fdatasync

#include <linux/version.h>
#include <features.h>

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if (LINUX_VERSION_CODE >= 0x020611 && __GLIBC_PREREQ(2, 6))
#define HAVE_SYNC_FILE_RANGE 1
#endif
#else
#if (LINUX_VERSION_CODE >= 0x020611)
#define HAVE_SYNC_FILE_RANGE 1
#endif
#endif

#ifdef HAVE_SYNC_FILE_RANGE
#define rdb_fsync_range(fd,off,size) sync_file_range(fd,off,size,SYNC_FILE_RANGE_WAIT_BEFORE|SYNC_FILE_RANGE_WRITE)
#else
#define rdb_fsync_range(fd,off,size) fsync(fd)
#endif


#if (defined __linux || defined __APPLE__)
#define USE_SETPROCTITLE
#define INIT_SETPROCTITLE_REPLACEMENT
void spt_init(int argc, char *argv[]);
void setproctitle(const char *fmt, ...);
#endif

#include <sys/types.h>

#if !defined(BYTE_ORDER) || \
    (BYTE_ORDER != BIG_ENDIAN && BYTE_ORDER != LITTLE_ENDIAN)
	/* you must determine what the correct bit order is for
	 * your compiler - the next line is an intentional error
	 * which will force your compiles to bomb until you fix
	 * the above macros.
	 */
#error "Undefined or invalid BYTE_ORDER"
#endif

#if (__i386 || __amd64) && __GNUC__
#define GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if (GNUC_VERSION >= 40100) || defined(__clang__)
#define HAVE_ATOMIC
#endif
#endif

#endif
