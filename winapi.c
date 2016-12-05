/*  winapi.c
 *  Copyright 1999, Yurij  Sysoev, All rights reserved
 */
#include "license.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <conio.h>
#ifdef unix
#   include <unistd.h>
#else
#   include <io.h>
#endif

#include "w32types.h"
#include "dbgout.h"

/*** Environment ***/
ULONG 		__WindowsVersion =  0x80000003; /* TNT | Win32s */
ULONG		__WindowsBuild   =        1381; /* NT 4.0 */

LPSTR		__CommandLineA = "";
LPSTR		__EnvironmentA = "";

HANDLE      __HStdIn  = 0x6ffdf000;
HANDLE      __HStdOut = 0x6ffdf000;
HANDLE      __HStdErr = 0x6ffdf000;

/***
CHAR		__NativeCurrentDirectory[MAX_PATH];
CHAR 		__CurrentDirectoryA[MAX_PATH];
CHAR 		__WindowsDirectoryA[MAX_PATH];
CHAR 		__SystemDirectoryA[MAX_PATH];
***/

/* Errors internal */
#define     __WigwamErrStrSize  128
char        __WigwamErrStr[__WigwamErrStrSize];
int         __WigwamErr      =    0;

/* Heap */
/* MSVC crt_init ( VirtualAlloc() )*/
static void* guard_arena_64K = 0;


int dlink_error(char *s) {
    apilog("%s called, but not implemented yet\n", s);
    return 0;
}

/*** global ***/

DWORD WINAPI __GetLastError(VOID)
{
    dlink_error("GetLastError");
    return 2;
}

/*** Heap ***/
HANDLE WINAPI __HeapCreate(
    DWORD flOptions,	// heap allocation flag 
    DWORD dwInitialSize,	// initial heap size 
    DWORD dwMaximumSize 	// maximum heap size 
   )
{
    apilog("HeapCreate:\n flOptions %X,\n dwInitialSize %d,\n dwMaximumSize %d\n",
            flOptions, dwInitialSize, dwMaximumSize);
    return 0x0920000; //no comment
}

BOOL WINAPI __HeapDestroy(
	    HANDLE hHeap
	    )
{
    apilog("HeapDestroy: hHeap %X\n", hHeap);
    return 1; //nonzero
}

#define HEAP_ZERO_MEMORY    0x08

LPVOID WINAPI __HeapAlloc(
	  HANDLE hHeap,
	  DWORD dwFlags,
	  DWORD dwBytes
	  )
{
LPVOID ret;

    apilog("HeapAlloc: hHeap %X, dwFlags %X, dwBytes %X\n" , hHeap, dwFlags, dwBytes);
    if(dwFlags & HEAP_ZERO_MEMORY)
        ret = (void *)calloc(1, dwBytes);
    else
        ret = (void *)malloc(dwBytes);
    return ret;
}

BOOL WINAPI __HeapFree(
	 HANDLE hHeap,
	 DWORD dwFlags,
	 LPVOID lpMem
	 )
{
    apilog("HeapFree:\n hHeap %X,\n dwFlags %X,\n lpMem %X\n", hHeap, dwFlags, lpMem);
    free(lpMem);
    return 1;//nonzero
}

LPVOID WINAPI __VirtualAlloc(
	     LPVOID lpAddress,
	     DWORD dwSize,
	     DWORD flAllocationType,
	     DWORD flProtect
	     )
{
    apilog("VirtualAlloc:\n lpAddress %X,\n dwSize %X,\n flAllocationType %X,\n flProtect %X,\n", lpAddress, dwSize, flAllocationType, flProtect);
    if(lpAddress == (void *)0
    && dwSize    == 0x0400000) {
        //MS CRT allocated it from 0 to default image base (0x0400000) for memory guard
        guard_arena_64K = (void *)malloc(0x010000);
        return (void *)guard_arena_64K;
    }
    else if(lpAddress == guard_arena_64K
         && dwSize    == 0x010000) { 
            return guard_arena_64K;
    }
    else
        return (void *)0;
}

BOOL WINAPI __VirtualFree(
	    LPVOID lpAddress,
	    DWORD dwSize,
	    DWORD dwFreeType
	    )
{
    apilog("VirtualFree : lpAddress %X,\n dwSize  %X,\ndwFreeType %X,\n", lpAddress, dwSize, dwFreeType);
    if(lpAddress == guard_arena_64K)
        free(guard_arena_64K);
    return 1; //TRUE
}

/*** Process ***/

VOID WINAPI __ExitProcess(
    UINT uExitCode 	// exit code for all threads  
   )
{
    apilog("ExitProcess with %X code \n", uExitCode);
    exit(uExitCode);
}

BOOL WINAPI __TerminateProcess(
    HANDLE hProcess,	// handle to the process 
    UINT uExitCode 	// exit code for the process  
   )
{
    dlink_error("TerminateProcess");
    return 0;
};

HANDLE WINAPI __GetCurrentProcess(VOID) {

    dlink_error("GetCurrentProcess");
    return 0; //process pseudo-handle
}

DWORD WINAPI __GetModuleFileNameA(
    HMODULE hModule,	   // handle to module to find filename for 
    LPTSTR FileNameBuffer, // pointer to buffer for module path 
    DWORD nSize 	   // size of buffer, in characters 
   )
{
static char* name = "self.exe";

    apilog("GetModuleFileName:\n hModule %X,\n FileNameBuffer %X,\n nSize %X\n",
            hModule, FileNameBuffer, nSize);

    memcpy(FileNameBuffer, name, strlen(name) + 1);
    return (strlen(name) + 1);
}

void* WINAPI __GetProcAddress(
	       HINSTANCE hModule,
	       LPSTR lpProcName
	       )
{
    dlink_error("GetProcAddress");
    return NULL;
}

HMODULE WINAPI __LoadLibraryA(
	     LPSTR lpLibFileName
	     )
{
    dlink_error("LoadLibraryA");
    return (HMODULE)NULL;
}

/*** Environment ***/

DWORD WINAPI __GetVersion(void) {
    return __WindowsVersion;
}

extern char **environ;

LPSTR WINAPI __GetEnvironmentStrings( VOID )
{
char **tmp;
int len, total = 0;
char *env_s, *dest;

    tmp = environ;
    /* size calculate */
    while(*tmp) {
        total += strlen(*tmp);
        total++; /* space reserved for separator ('\0') */
        tmp++;
    }
    total++; /* space reserved for end ('\0') */

    tmp = environ;
    dest = env_s = (char *)malloc(total);
    while(*tmp) {    
        len = strlen(*tmp);
        memcpy(dest, *tmp, len);
        dest += len;
       *dest = '\0';
        dest++;
        tmp++;
    }
    env_s[total - 1] = '\0';

    apilog("GetEnvironmentStrings\n");
    return env_s;
}

BOOL WINAPI __FreeEnvironmentStringsA(
    LPTSTR EnvBlock	// pointer to a block of environment strings
   )
{
    apilog("FreeEnvironmentStringsA: %s\n", EnvBlock);
    free(EnvBlock);
    return 1; //TRUE
}

LPWSTR WINAPI __GetEnvironmentStringsW( VOID )
{
    apilog("GetEnvironmentStringsW\n");
    return (void *)0;//not supported
}

BOOL WINAPI __FreeEnvironmentStringsW(
			LPWSTR lpEnvironmentBlock
			)
{
    apilog("FreeEnvironmentStringsW\n");
    return 1; //TRUE
}

typedef struct _STARTUPINFO { 
    DWORD   cb; 
    LPTSTR  lpReserved; 
    LPTSTR  lpDesktop; 
    LPTSTR  lpTitle; 
    DWORD   dwX; 
    DWORD   dwY; 
    DWORD   dwXSize; 
    DWORD   dwYSize; 
    DWORD   dwXCountChars; 
    DWORD   dwYCountChars; 
    DWORD   dwFillAttribute; 
    DWORD   dwFlags; 
    WORD    wShowWindow; 
    WORD    cbReserved2; 
    char   *lpReserved2; 
    HANDLE  hStdInput; 
    HANDLE  hStdOutput; 
    HANDLE  hStdError; 
} STARTUPINFO; 

VOID WINAPI __GetStartupInfoA(
    STARTUPINFO *lpStartupInfo
    )
{
    lpStartupInfo->cb              = sizeof(STARTUPINFO);
    lpStartupInfo->lpReserved      = (void *)0;
    lpStartupInfo->lpDesktop       = "WinSta0\\Default"; 
    lpStartupInfo->lpTitle         = "";
    lpStartupInfo->dwX             = 0x00000000; 
    lpStartupInfo->dwY             = 0x00000000; 
    lpStartupInfo->dwXSize         = 0x00000000; 
    lpStartupInfo->dwYSize         = 0x00000000; 
    lpStartupInfo->dwXCountChars   = 0x00000000; 
    lpStartupInfo->dwYCountChars   = 0x00000000; 
    lpStartupInfo->dwFillAttribute = 0x00000000; 
    lpStartupInfo->dwFlags         = 0x00000001; 
    lpStartupInfo->wShowWindow     = 0x0001; 
    lpStartupInfo->cbReserved2     = 0x0000;
    lpStartupInfo->lpReserved2     = (void *)0;
    lpStartupInfo->hStdInput       = __HStdIn;
    lpStartupInfo->hStdOutput      = __HStdOut;
    lpStartupInfo->hStdError       = __HStdErr;

}

LPSTR WINAPI __GetCommandLineA( VOID )
{
    apilog("GetCommandLineA\n");
    return __CommandLineA;
}

/*** Unicode ***/
int WINAPI __MultiByteToWideChar(
    UINT CodePage,	// code page 
    DWORD dwFlags,	// character-type options 
    LPCSTR lpMultiByteStr,	// address of string to map 
    int cchMultiByte,	// number of characters in string 
    LPWSTR lpWideCharStr,	// address of wide-character buffer 
    int cchWideChar 	// size of buffer 
   )
{
    apilog("MultiByteToWideChar\n");
    return cchMultiByte;
}

int WINAPI __WideCharToMultiByte(
   UINT		CodePage,
   DWORD	dwFlags,
   LPWSTR	lpWideCharStr,
   int		cchWideChar,
   LPSTR	lpMultiByteStr,
   int		cchMultiByte,
   LPCSTR	lpDefaultChar,
   int	   *lpUsedDefaultChar
   )
{
    apilog("WideCharToMultiByte:\n CodePage %X,\n dwFlags %X,\n lpWideCharStr %X,\n cchWideChar %X,\n lpMultiByteStr %X,\n cchMultiByte %X,\n lpDefaultChar %X,\n *lpUsedDefaultChar %X\n",
            CodePage , dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cchMultiByte, lpDefaultChar, lpUsedDefaultChar);
    return 0;//cchWideChar;
}

#define MAX_DEFAULTCHAR			2
#define MAX_LEADBYTES			12

typedef struct __CPINFO
{ 
   UINT		MaxCharSize; 
   BYTE		DefaultChar[MAX_DEFAULTCHAR]; 
   BYTE		LeadByte[MAX_LEADBYTES]; 
} CPINFO;

BOOL WINAPI __GetCPInfo(
   UINT		CodePage,
   CPINFO  *lpCPInfo
   )
{
    apilog("GetCPInfo: CodePage %X, lpCPInfo %X\n", CodePage, lpCPInfo);

    lpCPInfo->MaxCharSize = 0x01;
    lpCPInfo->DefaultChar[0] = '?';
    lpCPInfo->DefaultChar[1] = '\0';
    lpCPInfo->LeadByte[0] = '\0';

    return 1; //TRUE
}

UINT WINAPI __GetACP(VOID)
{
    apilog("GetACP\n");
    return 0x04e3;
}

UINT WINAPI __GetOEMCP(VOID)
{
    apilog("GetOEMCP\n");
    return 866;
}

/*** file ***/

UINT WINAPI __SetHandleCount(
	       UINT uNumber
	       )
{
    apilog("SetHandleCount: Number %d\n", uNumber);
    return uNumber;
}

DWORD WINAPI __GetFileType(
	    HANDLE hFile
	    )
{
    apilog("GetFileType: hFile %X\n", hFile);
    return 0x02;
}

HANDLE WINAPI __GetStdHandle(
	     DWORD nStdHandle
	     )
{
    apilog("GetStdHandle: %X\n", nStdHandle);
    switch(nStdHandle) {
        case 0xFFFFFFF4:
            return 0x0B;

        case 0xFFFFFFF5:
            return 0x07;

        case 0xFFFFFFF6:
            return 0x03;
    }
    return (HANDLE) -1;
}

/*
int __FsAccessFlags(DWORD faccess)
{
    int retval = 0;

    switch(faccess)
    {
        case GENERIC_READ:
            retval = O_RDONLY;
            break;

        case GENERIC_WRITE:
            retval = O_WRONLY;
            break;

        case (GENERIC_READ | GENERIC_WRITE):
            retval = O_RDWR;
            break;
    }

    return retval;
}

int __FsCreationFlags(DWORD fcreate)
{
    int retval = 0;

    switch(fcreate)
    {
        case CREATE_NEW:
            retval = O_CREAT | O_EXCL;
            break;

        case CREATE_ALWAYS:
            retval = O_CREAT | O_TRUNC;
            break;

        case OPEN_EXISTING:
            retval = 0;
            break;

        case OPEN_ALWAYS:
            retval = O_CREAT;
            break;

        case TRUNCATE_EXISTING:
            retval = O_TRUNC;
            break;
    }
    return retval;
}

*/

HANDLE WINAPI __CreateFileA(
    LPCSTR lpFileName,
/* generic access rights 
#define GENERIC_READ                    0x80000000
#define GENERIC_WRITE                   0x40000000
*/
    DWORD  dwDesiredAccess,
    DWORD  dwShareMode,
    void  *lpSecurityAttributes,
/* flags passed to CreateFile
#define CREATE_NEW                      1
#define CREATE_ALWAYS                   2
#define OPEN_EXISTING                   3
#define OPEN_ALWAYS                     4
#define TRUNCATE_EXISTING               5
 */
    DWORD  dwCreationDisposition,
    DWORD  dwFlagsAndAttributes,
    HANDLE hTemplateFile
    )
{
    apilog("CreateFileA:\n lpFileName %s,\n dwDesiredAccess %X,\n dwShareMode %X,\n lpSecurityAttributes %X,\n dwCreationDisposition %X,\n dwFlagsAndAttributes %X,\n hTemplateFile %X\n",
            lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
            dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    return 10; //open(lpFileName, , );
}

BOOL WINAPI __SetEndOfFile(
	     HANDLE hFile
	     )
{
    dlink_error("SetEndOfFile");
    return FALSE;
}

BOOL WINAPI __ReadFile(
	 HANDLE hFile,
	 LPVOID lpBuffer,
	 DWORD nNumberOfBytesToRead,
	 DWORD *lpNumberOfBytesRead,
	 void  *lpOverlapped
	 )
{
    dlink_error("ReadFile");
    return FALSE;
}

BOOL WINAPI __WriteFile(
	  HANDLE hFile,
	  void  *lpBuffer,
	  DWORD  nNumberOfBytesToWrite,
	  DWORD *lpNumberOfBytesWritten,
	  void  *lpOverlapped
	  )
{
    apilog("WriteFile:\n hFile %X,\n lpBuffer %X,\n nNumberOfBytesToWrite %d,\n lpNumberOfBytesWritten %X,\n lpOverlapped %X\n",
            hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

    if(hFile == 0x07) { //stdout
        //Allways worked
        *lpNumberOfBytesWritten = fwrite(lpBuffer, sizeof(char), nNumberOfBytesToWrite, stdout);
        //Stranger effect under NT: hidden output!
        //*lpNumberOfBytesWritten = write( 1, lpBuffer, nNumberOfBytesToWrite);
    }
    return 1; //TRUE
}

BOOL WINAPI __FlushFileBuffers(HANDLE hFile)
{
    dlink_error("FlushFileBuffers");
    return FALSE;
}

DWORD WINAPI __SetFilePointer(
	       HANDLE hFile,
	       long   MoveTo,
	       DWORD *lpDistanceToMoveHigh,
	       DWORD  dwMoveMethod
	       )
{
    apilog("SetFilePointer:\n hFile: %X,\n MoveTo: %d,\n dwMoveMethod: %d\n",
            hFile, MoveTo, dwMoveMethod);
    return 0; //previos position
}

BOOL WINAPI __SetStdHandle(
	     DWORD nStdHandle,
	     HANDLE hHandle
	     )
{
    dlink_error("SetStdHandle");
    return FALSE;
}

BOOL WINAPI __CloseHandle(
	    HANDLE hObject
	    )
{
    apilog("CloseHandle : %X\n", hObject);
    return TRUE;
}

/*** console ***/
BOOL WINAPI __SetConsoleMode(
    HANDLE hHandle,
    DWORD dwMode
    )
{
    apilog("SetConsoleMode: hHandle %X, dwMode: %X\n", hHandle, dwMode);
    return 1; //TRUE
}

#define KEY_EVENT			0x01
#define MOUSE_EVENT			0x02

typedef struct __KEY_EVENT_RECORD {
    BOOL bKeyDown;             
    WORD wRepeatCount;         
    WORD wVirtualKeyCode;      
    WORD wVirtualScanCode; 
    union { 
        //WCHAR UnicodeChar;
        short UnicodeChar;
        char  AsciiChar;
    } uChar;
    DWORD dwControlKeyState;   
} KEY_EVENT_RECORD; 
 
typedef struct __MOUSE_EVENT_RECORD {
    DWORD dwMousePosition;
    DWORD dwButtonState;
    DWORD dwControlKeyState;
    DWORD dwEventFlags;
} MOUSE_EVENT_RECORD; 
 
typedef struct __INPUT_RECORD {
    WORD EventType;
    union {
        KEY_EVENT_RECORD KeyEvent;
        MOUSE_EVENT_RECORD MouseEvent;
    } Event;
} INPUT_RECORD;

BOOL WINAPI __ReadConsoleInputA(
    HANDLE hConsoleInput,
    INPUT_RECORD *lpBuffer,
    DWORD nLength,
    DWORD *lpNumberOfEventsRead
    )
{
char ch;

    apilog("ReadConsoleInputA:\n hConsoleInput %X,\n lpBuffer %X,\n nLength %d,\n lpNumberOfEventsRead %X\n",
            hConsoleInput, lpBuffer, nLength, lpNumberOfEventsRead);

    ch = getch();
    
   *lpNumberOfEventsRead = 1;

    lpBuffer->EventType = KEY_EVENT;
    lpBuffer->Event.KeyEvent.bKeyDown = 1;
    lpBuffer->Event.KeyEvent.wRepeatCount = 1;
    lpBuffer->Event.KeyEvent.wVirtualKeyCode = 0x51;
    lpBuffer->Event.KeyEvent.wVirtualScanCode = 0x10;
    lpBuffer->Event.KeyEvent.uChar.AsciiChar = ch;
    lpBuffer->Event.KeyEvent.dwControlKeyState = 0x20;

    printf("bKeyDown %X,\n", lpBuffer->Event.KeyEvent.bKeyDown);
    printf("wRepeatCount %X,\n", lpBuffer->Event.KeyEvent.wRepeatCount);
    printf("wVirtualKeyCode %X,\n", lpBuffer->Event.KeyEvent.wVirtualKeyCode);
    printf("wVirtualScanCode %X,\n", lpBuffer->Event.KeyEvent.wVirtualScanCode);
    printf("ch %c,\n", (unsigned char)lpBuffer->Event.KeyEvent.uChar.AsciiChar);
    printf("dwControlKeyState %X,\n", lpBuffer->Event.KeyEvent.dwControlKeyState);

    return 1; //TRUE
}

BOOL WINAPI __GetConsoleMode(
    HANDLE hConsole,
    DWORD *lpMode
    )
{
    apilog("GetConsoleMode: hConsole %X\n", hConsole);
   *lpMode = 0; 
    return 1; //TRUE
}

BOOL WINAPI __PeekConsoleInputA(
    HANDLE hConsoleInput,
    /* PINPUT_RECORD */ void *lpBuffer,
    DWORD nLength,
    DWORD *lpNumberOfEventsRead
    )
{
    dlink_error("PeekConsoleInputA");
    return FALSE;
}

BOOL WINAPI __GetNumberOfConsoleInputEvents(
    HANDLE hConsoleInput,
    DWORD *lpNumberOfEvents
    )
{
    dlink_error("GetNumberOfConsoleInputEvents");
    return FALSE;
}

BOOL WINAPI __WriteConsoleA(
    HANDLE hConsoleOutput,
    void  *lpBuffer,
    DWORD  nNumberOfCharsToWrite,
    DWORD *lpNumberOfCharsWritten,
    void  *lpReserved
    )
{
    dlink_error("WriteConsoleA");
    return FALSE;
}

/*** exeptions ***/

/*
 * The exception frame, used for registering exception handlers
 * Win32 cares only about this, but compilers generally emit
 * larger exception frames for their own use.
 *
typedef struct __EXCEPTION_FRAME
{
  struct __EXCEPTION_FRAME *Prev;
  PEXCEPTION_HANDLER       Handler;
} EXCEPTION_FRAME;

typedef struct __EXCEPTION_FRAME *PEXCEPTION_FRAME;
 *
 *  this undocumented function is called when an exception
 *  handler wants all the frames to be unwound. RtlUnwind
 *  calls all exception handlers with the EH_UNWIND or
 *  EH_EXIT_UNWIND flags set in the exception record
 *
 *  This prototype assumes RtlUnwind takes the same
 *  parameters as OS/2 2.0 DosUnwindException
 *  Disassembling RtlUnwind shows this is true, except for
 *  the TargetEIP parameter, which is unused. There is
 *  a fourth parameter, that is used as the eax in the
 *  context.
 *

BOOL  WINAPI RtlUnwind( PEXCEPTION_FRAME pestframe,
                        LPVOID unusedEIP,
                        PEXCEPTION_RECORD pexcrec,
                        DWORD contextEAX );

BOOL __RtlUnwind( 
            PEXCEPTION_FRAME pestframe,
            LPVOID unusedEIP,
            PEXCEPTION_RECORD pexcrec,
            DWORD contextEAX ) {

    RtlUnwind(pestframe, unusedEIP, pexcrec, contextEAX);
}
***/

long __UnhandledExceptionFilter(
    /* STRUCT _EXCEPTION_POINTERS */void *ExceptionInfo	// address of exception info 
   )
{
    //UnhandledExceptionFilter(ExceptionInfo);
    apilog("Unhandled exception, programm terminated\n");
//#define EXCEPTION_EXECUTE_HANDLER        1
//#define EXCEPTION_CONTINUE_SEARCH        0
//#define EXCEPTION_CONTINUE_EXECUTION    -1
    exit(-1);
    return 1;
}

typedef void (*FUNCS) (void);
#define FARPROC  FUNCS

struct exp_entry {
    char  *name;
    FUNCS f_ptr;
};

#define EXP_ENTRY struct exp_entry
#define EXP_ENTRYSZ sizeof(EXP_ENTRY)

EXP_ENTRY exp_table[] = {
    {
    "HeapCreate", (FUNCS) __HeapCreate
    },
    {
    "GetVersion", (FUNCS) __GetVersion
    },
    {
    "ExitProcess", (FUNCS) __ExitProcess
    },
    {
    "TerminateProcess", (FUNCS) __TerminateProcess
    },
    {
    "GetCurrentProcess", (FUNCS) __GetCurrentProcess
    },
    {
    "RtlUnwind", (FUNCS) dlink_error
    },
    {
    "UnhandledExceptionFilter", (FUNCS) __UnhandledExceptionFilter
    },
    {
    "GetModuleFileNameA", (FUNCS) __GetModuleFileNameA
    },
    {
    "FreeEnvironmentStringsA", (FUNCS) __FreeEnvironmentStringsA
    },
    {
    "MultiByteToWideChar", (FUNCS) __MultiByteToWideChar
    },
    {
    "GetEnvironmentStrings", (FUNCS) __GetEnvironmentStrings
    },
    {
    "FreeEnvironmentStringsW", (FUNCS) __FreeEnvironmentStringsW
    },
    {
    "GetEnvironmentStringsW", (FUNCS) __GetEnvironmentStringsW
    },
    {
    "WideCharToMultiByte", (FUNCS) __WideCharToMultiByte
    },
    {
    "GetCPInfo", (FUNCS) __GetCPInfo
    },
    {
    "GetACP", (FUNCS) __GetACP
    },
    {
    "GetOEMCP", (FUNCS) __GetOEMCP
    },
    {
    "SetHandleCount", (FUNCS) __SetHandleCount
    },
    {
    "GetFileType", (FUNCS) __GetFileType
    },
    {
    "GetStdHandle", (FUNCS) __GetStdHandle
    },
    {
    "GetStartupInfoA", (FUNCS) __GetStartupInfoA
    },
    {
    "HeapDestroy", (FUNCS) __HeapDestroy
    },
    {
    "GetCommandLineA", (FUNCS) __GetCommandLineA
    },
    {
    "VirtualFree", (FUNCS) __VirtualFree
    },
    {
    "WriteFile", (FUNCS) __WriteFile
    },
    {
    "HeapAlloc", (FUNCS) __HeapAlloc
    },
    {
    "HeapFree", (FUNCS) __HeapFree
    },
    {
    "VirtualAlloc", (FUNCS) __VirtualAlloc
    },
    {
    "GetProcAddress", (FUNCS) __GetProcAddress
    },
    {
    "LoadLibraryA", (FUNCS) __LoadLibraryA
    },
    {
    "GetLastError", (FUNCS) __GetLastError
    },
    {
    "FlushFileBuffers", (FUNCS) __FlushFileBuffers
    },
    {
    "SetFilePointer", (FUNCS) __SetFilePointer
    },
    {
    "SetStdHandle", (FUNCS) __SetStdHandle
    },
    {
    "CloseHandle", (FUNCS) __CloseHandle
    },
    {
    "SetConsoleMode", (FUNCS) __SetConsoleMode
    },
    {
    "ReadConsoleInputA", (FUNCS) __ReadConsoleInputA
    },
    {
    "GetConsoleMode", (FUNCS) __GetConsoleMode
    },
    {
    "PeekConsoleInputA", (FUNCS) __PeekConsoleInputA
    },
    {
    "GetNumberOfConsoleInputEvents", (FUNCS) __GetNumberOfConsoleInputEvents
    },
    {
    "WriteConsoleA", (FUNCS) __WriteConsoleA
    },
    {
    "CreateFileA", (FUNCS) __CreateFileA
    },
    {
    "SetEndOfFile", (FUNCS) __SetEndOfFile
    },
    {
    "ReadFile", (FUNCS) __ReadFile
    },
    0
};

FARPROC GetExportedAddress(char *s) {
int k;
char len = strlen(s);

    for(k = 0; exp_table[k].name; k++) {
    char *str = exp_table[k].name;
        
        if(memcmp(s, str, len) == 0) {
            return exp_table[k].f_ptr;
            break;
        }
    }
    printf("%s not found!\n", s);
    return (void *) 0;

}

#ifdef __cplusplus
}
#endif
