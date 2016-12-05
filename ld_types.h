/*
 * ld_types.h
 * Copyright 1999, Yurij  Sysoev, All rights reserved
 */
#include "license.h"

#ifndef _LDD_TYPES
#define _LDD_TYPES

#ifdef __cplusplus
extern "C" {
#endif

typedef short           INT16;
typedef unsigned short  UINT16;
typedef int             INT32;
typedef unsigned int    UINT32;
typedef long            LONG32;
typedef unsigned long   ULONG32;

/* round macros */
#define ROUNDUP(a,b)	((((a)+(b)-1)/(b))*(b))
#define ROUNDDOWN(a,b)	(((a)/(b))*(b))
#define FIELD_OFFSET(type,fld)	((LONG)&(((type *)0)->fld))

#ifdef __cplusplus
}
#endif

#endif // _LDD_TYPES
