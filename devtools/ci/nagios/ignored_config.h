/* include/ignored_config.h.  Generated from ignored_config.h.in by configure.  */
/* include/ignored_config.h.in.  Generated from configure.ac by autoheader.  */

/* group name to run nagios */
#define DEFAULT_NAGIOS_GROUP "nagios"

/* user name to run nagios */
#define DEFAULT_NAGIOS_USER "nagios"

/* System has RLIMIT_PROC */
#define DETECT_RLIMIT_PROBLEM 1

/* defined if user enabled NERD (radio dispatcher) */
/* #undef ENABLE_NERD */

/* Define to the type of elements in the array argument to 'getgroups'.
   Usually this is either 'int' or 'gid_t'. */
#define GETGROUPS_T gid_t

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define if system has C99 compatible vsnprintf */
#define HAVE_C99_VSNPRINTF 1

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the declaration of 'tzname', and to 0 if you don't.
   */
/* #undef HAVE_DECL_TZNAME */

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Which loader library should we use? libtdl or dl? */
#define HAVE_DLFCN_H /**/

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define if your gd library has gdImageCreateTrueColor */
#define HAVE_GDIMAGECREATETRUECOLOR 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the <grp.h> header file. */
#define HAVE_GRP_H 1

/* Define to 1 if you have the 'initgroups' function. */
#define HAVE_INITGROUPS 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Have the krb5.h header file */
/* #undef HAVE_KRB5_H */

/* Define to 1 if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Which loader library should we use? libtdl or dl? */
/* #undef HAVE_LTDL_H */

/* Define to 1 if you have the <math.h> header file. */
#define HAVE_MATH_H 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define to 1 if you have the <regex.h> header file. */
#define HAVE_REGEX_H 1

/* Define to 1 if you have the 'setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the 'sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if you have the <socket.h> header file. */
/* #undef HAVE_SOCKET_H */

/* Have SSL support */
#define HAVE_SSL 1

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the 'strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the 'strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the 'strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if 'tm_zone' is a member of 'struct tm'. */
#define HAVE_STRUCT_TM_TM_ZONE 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/ipc.h> header file. */
#define HAVE_SYS_IPC_H 1

/* Define to 1 if you have the <sys/loadavg.h> header file. */
/* #undef HAVE_SYS_LOADAVG_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/msg.h> header file. */
#define HAVE_SYS_MSG_H 1

/* Define to 1 if you have the <poll.h> header file. */
#define HAVE_POLL_H 1

/* Define to 1 if you have the <sys/prctl.h> header file. */
#define HAVE_SYS_PRCTL_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/un.h> header file. */
#define HAVE_SYS_UN_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if your 'struct tm' has 'tm_zone'. Deprecated, use
   'HAVE_STRUCT_TM_TM_ZONE' instead. */
#define HAVE_TM_ZONE 1

/* Define to 1 if you don't have 'tm_zone' but do have the external array
   'tzname'. */
/* #undef HAVE_TZNAME */

/* Define to 1 if you have the <uio.h> header file. */
/* #undef HAVE_UIO_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the 'unsetenv' function. */
#define HAVE_UNSETENV 1

/* Whether va_copy() is available */
#define HAVE_VA_COPY 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Whether __va_copy() is available */
/* #undef HAVE___VA_COPY */

/* iobroker uses epoll method */
#define IOBROKER_USES_EPOLL 1

/* iobroker uses poll method */
/* #undef IOBROKER_USES_POLL */

/* iobroker uses select method */
/* #undef IOBROKER_USES_SELECT */

/* defined if va_list fails to compile */
/* #undef NEED_VA_LIST */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* typedef for socket size */
#define SOCKET_SIZE_TYPE size_t

/* Define to 1 if all of the C89 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define to 1 if your <sys/time.h> declares 'struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* traceroute command to use */
#define TRACEROUTE_COMMAND ""

/* defined to bring in the event broker objects */
#define USE_EVENT_BROKER /**/

/* compile histogram CGI */
#define USE_HISTOGRAM /**/

/* enables use of nanosleep (instead of sleep) */
#define USE_NANOSLEEP /**/

/* Use DH parameters for SSL/TLS */
/* #undef USE_SSL_DH */

/* defined if the user chose to include status map */
#define USE_STATUSMAP /**/

/* statuswrl CGI enabled by default, unless users chooses not to use it */
#define USE_STATUSWRL /**/

/* compile trends CGI */
#define USE_TRENDS /**/

/* Define to empty if 'const' does not conform to ANSI C. */
/* #undef const */

/* Define as 'int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to 'int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define as a signed integer type capable of holding a process identifier. */
/* #undef pid_t */

/* Define as 'unsigned int' if <stddef.h> doesn't define. */
/* #undef size_t */

/* Define as 'int' if <sys/types.h> doesn't define. */
/* #undef uid_t */
