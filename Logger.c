#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include <math.h>

#include <pthread.h>

#include "Logger.h"


LoggerConfig default_config = {TRUE, "%Y:%m:%d %H:%M:%S", "[INFO] ", "[ERROR] ", "[WARNING] ", "[CRITICAL]", "[PANIC]"};



/** Creates a Logger
 * Returns:     pointer to a Logger structure
*/
Logger *Logger_Create(LoggerConfig *config) {
    Logger *logger = (Logger *) malloc(sizeof(*logger));

    if(config == NULL) {
        logger->config = &default_config;
    } else {
        logger->config = config;
    }

    logger->file = NULL;
    logger->fp = NULL;

   /*  logger->Timestamp = TRUE;
     */
    pthread_mutex_init(&logger->l_mtx, NULL);

    return logger;
}

/** Destroys the logger structure
 * Returns:         nothing
 * In:      logger: pointer to logfile structure
*/
void Logger_Destroy(Logger *logger) {
    pthread_mutex_lock(&logger->l_mtx);
    if(logger == NULL) {
        return;
    }
    if(logger->fp != NULL) {
        fclose(logger->fp);
    }
    pthread_mutex_unlock(&logger->l_mtx);
    pthread_mutex_destroy(&logger->l_mtx);
    free(logger);
}

/** Sets the logfile to write to
 * In:      logger: Logger strucure
 *          file: filename to log to
*/
void Logger_setLogfile(Logger *logger, const char *file) {
    pthread_mutex_lock(&logger->l_mtx);
    if(logger->fp != NULL) {
        fclose(logger->fp);
    }
    logger->file = file;

    if(access(logger->file, F_OK) != 0) {
        logger->fp = fopen(logger->file, LOGGER_WRITE);
    } else {
        logger->fp = fopen(logger->file, LOGGER_APPEND);
    }

    pthread_mutex_unlock(&logger->l_mtx);
}

/** Disable Timestamp in Logger
 * Returns:         nothing
 * In:      logger: Logger structure
*/
void Logger_enableTimestamp(Logger *logger) {
    pthread_mutex_lock(&logger->l_mtx);
    logger->config->Timestamp = TRUE;
    pthread_mutex_unlock(&logger->l_mtx);
}
/** Disable Timestamp in Logger
 * Returns:         nothing
 * In:      logger: Logger structure
*/
void Logger_disableTimestamp(Logger *logger) {
    pthread_mutex_lock(&logger->l_mtx);
    logger->config->Timestamp = FALSE;
    pthread_mutex_unlock(&logger->l_mtx);
}

/** gets the time and calcualtes milliseconds: only used internally
 * Returns:     nothing
 * In:      tv: pointer to struct timeval
 *          millisec: pointer to in storing milliseconds
*/
void logger_get_time(struct timeval *tv, int *millisec) {
    gettimeofday(tv, NULL);
    if(millisec == NULL) {
        return;
    }

    *millisec = lrint(tv->tv_usec/1000.0);
    if(*millisec >= 1000) {
        *millisec = *millisec - 1000;
        tv->tv_sec++;
    }

}

/**  Logs a timestamp to the logfile: used internally
 * Returns:         nothing
 * In:      logger: pointer to a logging structure
 *          tv:     pointer to struct timeval
 *          milli:  milliseconds as integer
*/
void logger_log_timestamp(Logger *logger, struct timeval *tv, int milli) {
    struct tm *timeinfo;
    char tfmt_buf[26];

    timeinfo = localtime(&tv->tv_sec);

    strftime(tfmt_buf, 26, logger->config->timeformat_str, timeinfo);

    fprintf(logger->fp, "%s.%03d: ", tfmt_buf, milli);
}

/**  Logs a string in printf format string style with variable arguments like printf
 * Returns:         nothing
 * In:      logger: pointer to Logger structure
 *          fmt:    printf like format string 
 *          ...:    variable arguments like printf's
*/
void Logger_Log(Logger *logger, int kind,  char *fmt, ...) {

    struct timeval tv;
    int millisec;

    logger_get_time(&tv, &millisec);
    
    pthread_mutex_lock(&logger->l_mtx);
    if(logger->config->Timestamp) {
        logger_log_timestamp(logger, &tv, millisec);
    }
    
    switch(kind){
        case LOGGER_INFO:
            fprintf(logger->fp, "%s: ", logger->config->fmtInfo); break;
        case LOGGER_ERROR:
            fprintf(logger->fp, "%s: ", logger->config->fmtError); break;
        case LOGGER_WARNING:
            fprintf(logger->fp, "%s: ", logger->config->fmtWarning); break;
        case LOGGER_CRITICAL:
            fprintf(logger->fp, "%s: ", logger->config->fmtCritical); break;
        case LOGGER_PANIC:
            fprintf(logger->fp, "%s: ", logger->config->fmtPanic); break;
        default:
            fprintf(logger->fp, "%s: ", logger->config->fmtInfo); break;
            
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(logger->fp, fmt, args); fputc('\n', logger->fp);
    va_end(args);
    fflush(logger->fp);
    pthread_mutex_unlock(&logger->l_mtx);
}

void Logger_Info(Logger *logger, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Logger_Log(logger, LOGGER_INFO, fmt, args);
    va_end(args);
}

void Logger_Error(Logger *logger, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Logger_Log(logger, LOGGER_ERROR, fmt, args);
    va_end(args);
}

void Logger_Warning(Logger *logger, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Logger_Log(logger, LOGGER_WARNING, fmt, args);
    va_end(args);
}

void Logger_Critical(Logger *logger, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Logger_Log(logger, LOGGER_CRITICAL, fmt, args);
    va_end(args);
}

void Logger_Panic(Logger *logger, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Logger_Log(logger, LOGGER_PANIC, fmt, args);
    va_end(args);
}


