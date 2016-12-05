/* 
 *  loader.h
 *  Copyright 1999, Yurij  Sysoev, All rights reserved
 *  Win32 Portable Executable loader
 */

#ifdef __cplusplus
extern "C" {
#endif

struct ModuleInfo {
    struct ModuleInfo *Next;
    char       *ModuleName;
    char       *ModuleFullName;
    ULONG       Flags;
    BYTE       *Image;
    FILEHDR    *CoffOfset;
    AOUTHDR    *AoutHdr;
    NT_HDR     *NtHdr;
    char       *StringTable;
    SCNHDR     *Sections;
    IMAGE_IMPORT_DESCRIPTOR    *Imports;
    IMAGE_EXPORT_DIRECTORY     *Exports;
    IMAGE_RELOCATION           *Relocations;
    UINT        BaseAddress;
    UINT        Delta;
    UINT        LoadSize;
    HMODULE     HModule;
    UINT        Usage;
};
#define MODULE_INFO struct ModuleInfo
#define MODULE_INFOSZ sizeof(MODULE_INFO)

MODULE_INFO* LoadCoffFile(MODULE_INFO* DllInfo, char* FileName);

#ifdef __cplusplus
}
#endif
