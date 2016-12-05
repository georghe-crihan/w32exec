/* 
 *  dgbout.h
 *  Copyright 1999, Yurij  Sysoev, All rights reserved
 */
#include "license.h"

#ifdef __cplusplus
extern "C" {
#endif

void setDgbLog(int y_n);
void setApiLog(int y_n);
void setDbgOut(FILE *dbgfile);
void dbglog(const char *fmt, ...);
void apilog(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

