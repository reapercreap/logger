#ifndef RP_LOGGER_H
#define RP_LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include <math.h>

#include <pthread.h>

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define LOGGER_APPEND "a"
#define LOGGER_WRITE "w"

#define LOGGER_INFO 0
#define LOGGER_ERROR 1
#define LOGGER_WARNING 2
#define LOGGER_CRITICAL 3
#define LOGGER_PANIC 4

typedef struct LoggerConfig {
    BOOL Timestamp;
    const char *timeformat_str;
    const char *fmtInfo;
    const char *fmtError;
    const char *fmtWarning;
    const char *fmtCritical;
    const char *fmtPanic;
} LoggerConfig;

/*     BOOL Timestamp;
    const char *timeformat_str;
    */

typedef struct _Logger {
    const char *file;
    FILE *fp;
    LoggerConfig *config;
    pthread_mutex_t l_mtx;
} Logger;

/** Description: Creates a Logger
 * 
 * Returns:     pointer to a Logger structure
*/
Logger *Logger_Create(LoggerConfig *conf);

/** Description: Destroys the logger structure
 * 
 * Returns:         nothing
 * In:      logger: pointer to logfile structure
*/
void Logger_Destroy(Logger *logger);

/** Description: Sets the logfile to write to
 * 
 * In:      logger: Logger strucure
 *          file: filename to log to
*/
void Logger_setLogfile(Logger *logger, const char *file);

/** Description: Disable Timestamp in Logger
 * 
 * Returns:         nothing
 * In:      logger: Logger structure
*/
void Logger_enableTimestamp(Logger *logger);

/** Description: Disable Timestamp in Logger
 * 
 * Returns:         nothing
 * In:      logger: Logger structure
*/
void Logger_disableTimestamp(Logger *logger);

/** Description: Logs a string in printf format string style with variable arguments like 
 * 
 * Returns:         nothing
 * 
 * In:      logger: pointer to Logger structure
 *          fmt:    printf like format string 
 *          ...:    variable arguments like printf's
*/
void Logger_Log(Logger *logger, int kind,  char *fmt, ...);

void Logger_Info(Logger *logger, char *fmt, ...);

void Logger_Error(Logger *logger, char *fmt, ...);

void Logger_Warning(Logger *logger, char *fmt, ...);

void Logger_Critical(Logger *logger, char *fmt, ...);

void Logger_Panic(Logger *logger, char *fmt, ...);

#endif