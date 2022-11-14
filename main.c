#include "Logger.h"

void *log_tester(void *logger) {
    int i = 0;
    while(TRUE) {
        printf("i is %d\n", i);
        Logger_Log((Logger*) logger, LOGGER_INFO, "teststring %d", i++);
        sleep(10);
    }
}

int main(void) {
    LoggerConfig conf;
    conf.fmtError = "Error";
    conf.fmtInfo = "Info";
    conf.fmtCritical = "Critical";
    conf.fmtPanic = "Panic";
    conf.fmtWarning = "Warning";

    Logger *log = Logger_Create(NULL);
    Logger_setLogfile(log, "test.log");
    
    int i;
    pthread_t threads[10];
    for(i = 0; i < 10; i++) {
        printf("creating thread %d\n", i+1);
        pthread_create(&threads[i], NULL, log_tester, log);
        sleep(1);
        Logger_enableTimestamp(log);
    }
    printf("Waiting 3 Seconds");
    sleep(30);
    Logger_setLogfile(log, "newfile.log");
    sleep(30);

}
