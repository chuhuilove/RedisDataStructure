#include"serverlog.h"


void serverLog(int level, const char *fmt, ...) {
    va_list ap;
    char msg[LOG_MAX_LEN];


    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    serverLogRaw(level,msg,"");
}

void serverLogRaw(int level, const char *msg,char * logfile) {
    const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmode = (level & LL_RAW);

	//
    int log_to_stdout = logfile[0] == '\0';

    level &= 0xff; /* clear flags */

    fp = log_to_stdout ? stdout : fopen(logfile,"a");
    if (!fp) return;

    if (rawmode) {
        fprintf(fp,"%s",msg);
    } else {
        int off;
        struct timeval tv;
        int role_char='M';
    

        gettimeofday(&tv,NULL);
        struct tm tm;
        nolocks_localtime(&tm,tv.tv_sec,server.timezone,server.daylight_active);
        off = strftime(buf,sizeof(buf),"%d %b %Y %H:%M:%S.",&tm);
        snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
		
        fprintf(fp,"%d:%c %s %c %s\n",(int)getpid(),role_char, buf,c[level],msg);
    }
    fflush(fp);

    if (!log_to_stdout) fclose(fp);
    syslog(syslogLevelMap[level], "%s", msg);
}


