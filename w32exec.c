/* 
 *  w32exec.c
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
#include <io.h>
#ifdef unix
#include <unistd.h>
#endif

#include "w32types.h"
#include "coff_pe.h"
#include "loader.h"
#include "dbgout.h"

int main(int argc, char **argv) {
/**/
MODULE_INFO*    DllInfo;

    printf("PE loader. (c) 1999, Yurij  Sysoev, All rights reserved\n\n");

    if(argc < 2) {
		printf("\tusage: %s file_name.exe\n", argv[0]);
        return -1;
    }



   /* Get the value of the LIB environment variable. */
    if(getenv( "DBG_LOG" ))
        setDgbLog(TRUE);
    
    if(getenv( "API_LOG" ))
        setApiLog(TRUE);

    DllInfo = LoadCoffFile(DllInfo, argv[1]);

    /*** RUN */
    {
    typedef int (*PEXE_ENTRY)(void *, int, int);
    PEXE_ENTRY      proc;

    proc = (PEXE_ENTRY)((UINT)DllInfo->AoutHdr->entry + (UINT)DllInfo->Image);
    proc((void *)DllInfo, 0, 0 );
    }
    /***/



    return 0;
}

#ifdef __cplusplus
}
#endif
