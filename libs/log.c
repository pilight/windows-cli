/*
	Copyright (C) 2013 CurlyMo

	This file is part of the Raspberry Pi 433.92Mhz transceiver.

    Raspberry Pi 433.92Mhz transceiver is free software: you can redistribute
	it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.

    Raspberry Pi 433.92Mhz transceiver is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Raspberry Pi 433.92Mhz transceiver. If not, see
	<http://www.gnu.org/licenses/>
*/

//#include "mingw.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include "config.h"
#include "gc.h"
#include "log.h"

FILE *lf=NULL;

/* Enable log */
int filelog = 1;
int shelllog = 1;
int loglevel = LOG_INFO;
char logfile[1024] = LOG_FILE;

int log_gc(void) {
	if(lf != NULL) {
		if(fclose(lf) != 0)
			return 0;
	}
	return 1;
}

void logprintf(int prio, const char *format_str, ...) {
	va_list ap;

	if(filelog == 0 && shelllog == 0)
		return;

	if(loglevel >= prio) {
		if(lf == NULL && filelog > 0) {
			if((lf = fopen(logfile, "a+")) == NULL) {
				logprintf(LOG_WARNING, "could not open logfile %s", logfile);
			} else {
				gc_attach(log_gc);
			}
		}

		time_t current;
		char *currents;

		current=time(&current);
		currents=ctime(&current);

		if(shelllog == 1) {
			fprintf(stderr, "[%15.15s] %s: ",currents+4, progname);
			va_start(ap, format_str);

			if(prio==LOG_WARNING)
				fprintf(stderr, "WARNING: ");
			if(prio==LOG_ERR)
				fprintf(stderr, "ERROR: ");
			if(prio==LOG_INFO)
				fprintf(stderr, "INFO: ");
			if(prio==LOG_NOTICE)
				fprintf(stderr, "NOTICE: ");
			if(prio==LOG_DEBUG)
				fprintf(stderr, "DEBUG: ");
			vfprintf(stderr, format_str, ap);
			fputc('\n',stderr);
			fflush(stderr);
			va_end(ap);
		}
	}
}

void logperror(int prio, const char *s) {
	// int save_errno = errno;
	// if(logging == 0)
		// return;

	// if(s != NULL) {
		// logprintf(prio, "%s: %s", s, strerror(errno));
	// } else {
		// logprintf(prio, "%s", strerror(errno));
	// }
	// errno = save_errno;
}

void enable_file_log(void) {
	filelog = 1;
}

void disable_file_log(void) {
	filelog = 0;
}

void enable_shell_log(void) {
	shelllog = 1;
}

void disable_shell_log(void) {
	shelllog = 0;
}

void set_loglevel(int level) {
	loglevel = level;
}

int get_loglevel(void) {
	return loglevel;
}
