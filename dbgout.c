/* 
 *  dgbout.c
 *  Copyright 1999, Yurij  Sysoev, All rights reserved
 */
#include "license.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

static int Dbg = 0;
static int Api = 0;
static char   outbuff[1024];
static FILE*  DbgOut = stderr;

void setDgbLog(int y_n) {
/**/

    Dbg = y_n;
}

void setApiLog(int y_n) {
/**/

    Api = y_n;
}

void setDbgOut(FILE *dbgfile) {
/**/

    DbgOut = dbgfile;
}

void dbglog(const char *fmt, ...) {
/**/

    if(Dbg) {
        va_list args;
    
        va_start(args, fmt);
        vsprintf(outbuff, fmt, args);
        va_end(args);
        fprintf( DbgOut, "DEBUG: %s", outbuff);
        fflush( DbgOut );
    }
}

void apilog(const char *fmt, ...) {
/**/

    if(Api) {
        va_list args;
    
        va_start(args, fmt);
        vsprintf(outbuff, fmt, args);
        va_end(args);
        fprintf(DbgOut, "API: %s", outbuff);
        fflush(DbgOut );
    }
}
#ifdef __cplusplus
}
#endif

