#ifndef _LOG_H_
#define _LOG_H_

#ifndef _WIN32
#include <syslog.h>
#else
#define	LOG_EMERG	0
#define	LOG_ALERT	1
#define	LOG_CRIT	2
#define	LOG_ERR		3
#define	LOG_WARNING	4
#define	LOG_NOTICE	5
#define	LOG_INFO	6
#define	LOG_DEBUG	7
#endif
#include "config.h"

void logprintf(int prio, const char *format_str, ...);
void logperror(int prio, const char *s);
void enable_file_log(void);
void disable_file_log(void);
void enable_shell_log(void);
void disable_shell_log(void);
void set_logfile(char *file);
void set_loglevel(int level);
int get_loglevel(void);

#endif
