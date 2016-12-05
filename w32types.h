/*
 * w32types.h
 * Copyright 1999, Yurij  Sysoev, All rights reserved
 */
#ifndef _W32_TYPES
#define _W32_TYPES

#include "license.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void            VOID;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int    DWORD;
typedef unsigned int    BOOL;
typedef char            CHAR;
typedef int             INT;
typedef long            LONG;
typedef unsigned int    UINT;
typedef unsigned long   ULONG;

/****** Additional basic types **************************************/
#ifdef unix
#define __stdcall       __attribute__((stdcall))
#endif

#define FAR
#define PASCAL          __stdcall
#define CDECL

#define WINAPI          FAR PASCAL
#define CALLBACK        FAR PASCAL

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif //FALSE

typedef UINT            WPARAM;
typedef LONG            LPARAM;
typedef LONG            LRESULT;

typedef unsigned int    HANDLE;
#define DECLARE_HANDLE(name)    typedef UINT name

DECLARE_HANDLE(HMODULE);
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HLOCAL);
DECLARE_HANDLE(HGLOBAL);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HTASK);

#if !defined(NULL)
#define NULL    0L
#endif //NULL

typedef void *          LPVOID;
typedef WORD *          LPWORD;
typedef DWORD*          LPDWORD;
typedef char *          LPSTR;
typedef const char FAR* LPCSTR;
typedef void *          LPLOGPALETTE;
typedef void *          LPMSG;
typedef WORD *          LPTSTR;
typedef WORD *          LPWSTR;

/* MAKE/HI/LO macros */
#define MAKEWORD(a,b)	     ( (WORD)     (((BYTE)(a)) | (((WORD)    ((BYTE)(b))) << 8 )) )
#define MAKELONG(a,b)        ( (LONG)     (((WORD)(a)) | (((LONG)    ((WORD)(b))) << 16)) )
#define MAKELONGLONG(a, b)   ( (LONGLONG) (((LONG)(a)) | (((LONGLONG)((LONG)(a))) << 32)) )

#define LOLONG(a)   ((LONG)(a))
#define HILONG(a)   ((LONG)(((ULONGLONG)(a) >> 32) & 0xFFFFFFFF))
#define LOWORD(a)   ((WORD)(a))
#define HIWORD(a)	((WORD)(((DWORD)(a) >> 16) & 0xFFFF)) 
#define LOBYTE(a)	((BYTE)(a))
#define HIBYTE(a)	((BYTE)(((WORD )(a) >> 8) & 0xFF))

#ifdef __cplusplus
}
#endif

#endif //_W32_TYPES

 