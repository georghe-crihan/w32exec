/* 
 *  loader.c
 *  Copyright 1999, Yurij  Sysoev, All rights reserved
 *
 *  based on kernel/peload.c
 *  Copyright   1996    Onno Hovers
 *
 *  Copyright	1995	Martin von Loewis
 *  Copyright	1994	Eric Youngdale & Erik Bos
 *
 *  Win32 Portable Executable loader
 */
#include "license.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef unix
#include <unistd.h>
#endif

#include <io.h>

#include "w32types.h"
#include "coff_pe.h"
#include "loader.h"
#include "dbgout.h"

typedef void (*FUNCS) (void);
#define FARPROC  FUNCS

FARPROC GetExportedAddress(char *s);

#define DOS16       0
#define DOS32       1
#define RSX32       2
#define WIN_IMG     3
#define COFF_ONLY   4

/*\
Determine image target type:
 = DOS16
 = DOS32
 = RSX32
 = WIN_IMG
 = COFF_ONLY
and go to begin
\*/
int do_image(int filehandle, ULONG* headoff){
/**/
EXE_HDR     exehdr;
STUB_HDR2   stubhdr;
int target = 0;

    read(filehandle, &exehdr, sizeof(struct exe_hdr));
        
    if (exehdr.signatur == 0x5a4d) {    /* MZ */
        /* EMX/RSX/GO32 ? */
        *headoff = ((UINT) exehdr.hdr_para) * 16;
        lseek(filehandle, *headoff, SEEK_SET);
        read(filehandle, &stubhdr, sizeof(stubhdr));
        if (memcmp(stubhdr.sig, "emx", 3) == 0) {
            *headoff = *(UINT *) stubhdr.next_hdr;
            target = RSX32;
        }
        else if (memcmp(stubhdr.sig, "go32", 4) == 0) {
            target = DOS32;
            *headoff = (UINT) exehdr.high * 512L;
            if (exehdr.low)
                *headoff += (UINT) exehdr.low - 512L;
        }
        else {
        /* Check for NewExe field */
        char ch;
            lseek(filehandle,0x18,SEEK_SET);
            read(filehandle, &ch, 1);
            if(ch != 0x40) {
                /* It's not WINDOWS file */
                *headoff = 0L;
                target = DOS16;
            }
            else {
                /* Get offset new-Exe */
                lseek(filehandle, 0x3c,SEEK_SET);
                read(filehandle, headoff, 2);
                target = WIN_IMG;
            }
        }
    } /* MZ */
    else {
        if (exehdr.signatur == 0x14C) { /* pure COFF */
            *headoff = 0L;
            target = COFF_ONLY;
        }
        else
            goto fail;
    }

    if (lseek(filehandle, *headoff, SEEK_SET) == -1L)
        goto fail;
    return target;

fail:
    *headoff = 0;
    lseek(filehandle, 0, SEEK_SET);
    return -1;
}

BOOL PE_Relocate(MODULE_INFO *pefile) {
/**/
IMAGE_RELOCATION   *relocblock;   
void               *page;
UINT                loadaddr;
int                 delta;
int                 count;   
int                 type;
int                 offset;
int                 i; 
   
    loadaddr   = (UINT)pefile->Image;
    delta      = loadaddr - pefile->BaseAddress;
    relocblock = pefile->Relocations;
	
    if(delta == 0)
        return TRUE;
    if(relocblock == NULL)
        return FALSE;

    /* do the base relocations for a file */
    dbglog("relocating ...\n");
    while(relocblock->VirtualAddress) {
        page  = (void *)((UINT)loadaddr + (UINT)relocblock->VirtualAddress);
        count = (relocblock->SymbolTableIndex - 8)/2;
			
        /* do the base relocations for a page */

        for(i=0; i<count; i++) {
            offset = relocblock->Type[i] & 0xFFF;
            type   = relocblock->Type[i] >> 12;
            switch(type) {
                case IMAGE_REL_BASED_ABSOLUTE: 
                /* a NOP */
                    break;
                case IMAGE_REL_BASED_HIGH:
                    *(UINT *)((UINT)page+offset) += HIWORD(delta);
                    break;
                case IMAGE_REL_BASED_LOW:
                    *(UINT *)((UINT)page+offset) += LOWORD(delta);
                    break;
                case IMAGE_REL_BASED_HIGHLOW:
                    *(UINT *)((UINT)page+offset) += delta;
                    break;
                default:
                    dbglog("Unknown fixup type\n");
                    break;
            }
        }
        relocblock = (IMAGE_RELOCATION *)((BYTE *)relocblock + 
                     relocblock->SymbolTableIndex);
    }
    return TRUE;		                    
}

BOOL PE_FixupImports(MODULE_INFO *pefile) {
/**/
IMAGE_IMPORT_DESCRIPTOR    *imports;
void                       *loadptr;
IMAGE_IMPORT_BY_NAME       *name; 
UINT                       *importlist;
UINT                       *thunklist;
int                         i;

    loadptr = (void *)pefile->Image;
    imports =         pefile->Imports;
    i = 0;

    dbglog("importing ...\n");

    while(imports->Name) {
        dbglog("Imports from module %s:\n",(char *)((UINT)loadptr + (UINT)imports->Name));

        /***
        pimpfile=__ImageLoadModule((int)loadptr +  pimports->Name);
      
        if(!pimpfile)   
            return FALSE;
        ***/

        importlist  = (UINT *)((UINT)loadptr + (UINT)imports->Characteristics);
        thunklist   = (UINT *)((UINT)loadptr + (UINT)imports->FirstThunk);
           
        while(*(thunklist)) {   
            /* try thunklist first */      
            if( ( *(thunklist) >= 0x80000000) && 
                ( *(thunklist) <  0x80010000) )
            {
                /* import by ordinal */
                dbglog("import by ordinal: %d\n", *((int *)thunklist) & 0xFFFF);

                /***
                (LPVOID) *thunklist = __ImageGetExportedAddress(
                                     (LPVOID) pimpfile->LoadAddress, 
                                     pimpfile->Exports, 
                                     (LPCSTR) (*thunklist & 0xFFFF));
                ***/
            }
            else if( ((UINT)loadptr + *thunklist >  (UINT)loadptr)
                   &&((UINT)loadptr + *thunklist <  (UINT)loadptr + pefile->LoadSize) )
            {
                /* import by name */
                name = (IMAGE_IMPORT_BY_NAME *) ((UINT)loadptr + *thunklist);
                dbglog("import by name: %s\n", (char *)&(name->Name));
                (FARPROC) *thunklist = GetExportedAddress((char *)&(name->Name));

                /***
                (LPVOID) *thunklist = __ImageGetExportedAddress( (LPVOID) pimpfile->LoadAddress, pimpfile->Exports, (LPSTR) &(pname->Name));
                ***/
            }
         
            /*
            * this file has been cursed by "BIND", try the inofficial
            * importlist 
            */
          
            else if( ( *importlist >= 0x80000000)
                   &&( *importlist <  0x80010000) )
            {
                /* import by ordinal */             
                dbglog("bind used, import by ordinal: %d\n", (int)(*importlist & 0xFFFF));
                /***
                (LPVOID) *thunklist = __ImageGetExportedAddress((LPVOID) pimpfile->LoadAddress, pimpfile->Exports, (LPCSTR) (*importlist & 0xFFFF));
                ***/                                                                               
            }
            else if( ( (UINT)loadptr + *importlist >  (UINT)loadptr)
                   &&( (UINT)loadptr + *importlist <  (UINT)loadptr + pefile->LoadSize) )
            {
                /* import by name */
                name = (IMAGE_IMPORT_BY_NAME *) ((UINT)loadptr + *importlist);
                dbglog("bind used, import by name: %s\n",
                     (char *) &(name->Name));
                /***         
                (LPVOID) *thunklist = __ImageGetExportedAddress((LPVOID) pimpfile->LoadAddress, pimpfile->Exports,(LPSTR) &(pname->Name));
                ***/
            }      
            else {
            /* last resort */
            dbglog("bind used, no name/ordinal: %X\n",(int) *((int *)thunklist));
            /***
            *thunklist = *thunklist+pimpfile->LoadAddress-
                          pimpfile->BaseAddress;
            **/
            }
            thunklist++;
            importlist++;
        }
        i++;
        imports++;
    }
    return TRUE;
}

MODULE_INFO* LoadCoffFile(MODULE_INFO* DllInfo, char* FileName) {
/**/
int     fhandle;
int     file_size;
int     target = 0;
ULONG   headoff;
FILEHDR file_hdr;
AOUTHDR aout_hdr;
NT_HDR  nt_hdr;
int     i;
int     dirsize;
UINT    dirrva;

    if ((fhandle = open(FileName, O_RDONLY | O_BINARY)) == -1)
        return NULL;
    file_size = lseek(fhandle, 0L, SEEK_END);
    lseek(fhandle, 0L, SEEK_SET);
        /* skip exe-header, return correct offset in file */
    headoff = 0;
    target = do_image(fhandle, &headoff);

    switch(target) {
        case DOS16:
            goto fail;
        case DOS32:
            break; /* ! */
        case RSX32:
        case WIN_IMG:
        {
        UINT32 pe_sig;
            read(fhandle, &pe_sig, sizeof(UINT32));
            if (pe_sig != 0x00004550)
                goto fail; /* not PE file */
            headoff = headoff + sizeof(UINT32);
        }
            break;
        case COFF_ONLY:
            break; /* ! */
        default:
            goto fail;
    }

    DllInfo = (MODULE_INFO *)malloc(MODULE_INFOSZ);
    if(!DllInfo)
        goto fail;

    lseek(fhandle, headoff, SEEK_SET);
    read(fhandle, &file_hdr, FILEHSZ);
    if(file_hdr.f_magic != 0x14C)  {// COFF header 
        free(DllInfo);
        goto fail;
    }

    read(fhandle, &aout_hdr, AOUTHSZ);

    if(target == WIN_IMG)
        read(fhandle, &nt_hdr, NT_HDRSZ);

    if(target == WIN_IMG) {
        DllInfo->BaseAddress = nt_hdr.ImageBase;
        DllInfo->LoadSize = nt_hdr.SizeOfImage;
    }
    else {
        DllInfo->BaseAddress = aout_hdr.text_start;
        DllInfo->LoadSize = file_size - headoff;
    }

    DllInfo->Image = (void *)malloc(DllInfo->LoadSize);
    if(!DllInfo->Image) {
        free(DllInfo);
        goto fail;
    }

    dbglog("loading %s based at 0x%X from address 0x%X\n",
            FileName, (UINT)DllInfo->BaseAddress,
            (UINT)DllInfo->Image );

    /* load file headers */
    if(target == WIN_IMG) {
        lseek(fhandle, 0L, SEEK_SET);                     
        read(fhandle, DllInfo->Image, headoff + FILEHSZ + file_hdr.f_opthdr + file_hdr.f_nscns * SCNHDRSZ);
        DllInfo->CoffOfset = (FILEHDR *)( (BYTE *)(DllInfo->Image) + headoff );
        DllInfo->NtHdr = (NT_HDR *)( (BYTE *)(DllInfo->CoffOfset) + FILEHSZ + AOUTHSZ);
    }
    else {
        lseek(fhandle, headoff, SEEK_SET);
        read(fhandle, DllInfo->Image, FILEHSZ + file_hdr.f_opthdr + file_hdr.f_nscns * SCNHDRSZ);
        DllInfo->CoffOfset = (FILEHDR *)( (BYTE *)(DllInfo->Image) );
        DllInfo->NtHdr = (NT_HDR *)(NULL);
    }

    DllInfo->AoutHdr = (AOUTHDR *)( (BYTE *)(DllInfo->CoffOfset) + FILEHSZ);
    DllInfo->Sections = (SCNHDR *)((BYTE *)(DllInfo->AoutHdr) + DllInfo->CoffOfset->f_opthdr);

    /* load sections - !now for NT PE image only! */
    for(i=0; i < file_hdr.f_nscns; i++) {
    int     flags;
    UINT   rawdatasize;
    UINT   virtualsize;
    void   *section;
    long    diff;

        dbglog("Section %d Name           = %s\n", i,
              DllInfo->Sections[i].s_name);
        dbglog("Section %d VirtualAddress = %X\n", i,
               (UINT) DllInfo->Sections[i].s_vaddr);
        dbglog("Section %d SizeOfRawData  = %X\n", i,
               (UINT) DllInfo->Sections[i].s_size);
        dbglog("Section %d VirtualSize    = %X\n", i,
               (UINT) DllInfo->Sections[i].s_paddr);
        dbglog("Section %d file pointer   = %X\n", i,
                DllInfo->Sections[i].s_scnptr);

        section=(void *)((UINT)DllInfo->Image + (UINT)DllInfo->Sections[i].s_vaddr);

        /* in original -- commit memory for this section (does nothing but bookkeeping) */

        /*
         * The SizeOfRawData is the size of data that has to be loaded 
         * into memory. 
         * If the VirtualSize is greater than the SizeOfRawData
         * the rest is BSS. 
         * Suprisingly, some apps have a VirtualSize == 0
         */
                             
        flags       = DllInfo->Sections[i].s_flags;      
        rawdatasize = DllInfo->Sections[i].s_size;
        virtualsize = DllInfo->Sections[i].s_paddr;
      
        /* load this section */
        if((flags & IMAGE_SCN_CNT_UNINITIALIZED_DATA) == 0) { //.bss
            if(nt_hdr.FileAlignment >= PAGE_SIZE) {
                /* if we have 4 K filealign we can just mmap everything */
                ;
            }
            else {
                dbglog("LoadCoffFile: reading in section %d\n", i);
                if(lseek(fhandle, DllInfo->Sections[i].s_scnptr, SEEK_SET) == -1) {
                    dbglog("LoadCoffFile: could not seek\n");
                    goto free_and_fail;
                }
                if(read(fhandle, section, rawdatasize) == -1) {
                    printf("LoadCoffFile: could not read\n");
                    goto free_and_fail;
                }
            }
            //diff = ROUNDUP(virtualsize, PAGE_SIZE) - ROUNDUP(rawdatasize, PAGE_SIZE);
            diff = virtualsize - rawdatasize;
            if(diff > 0) {
                dbglog("The rest of this section is loaded as bss\n");
                dbglog("zeroing out bss\n");
                memset((void *)((UINT)section + rawdatasize), 0, diff);
            }
        }
        else {
            virtualsize = ROUNDUP(max(virtualsize, rawdatasize), PAGE_SIZE);
            dbglog("zeroing out bss\n");
            memset(section, 0, DllInfo->Sections[i].s_size);
        }
        /* end load for section */
    }/* for end - end sections load */

    /* get interesting RVAs */
    dirsize = nt_hdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    dirrva  = nt_hdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if(dirsize)
        DllInfo->Exports = (IMAGE_EXPORT_DIRECTORY *)((UINT)DllInfo->Image + dirrva);
    else
        dbglog("no exports\n");
   
    dirsize=nt_hdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
    dirrva =nt_hdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(dirsize)
        DllInfo->Imports = (IMAGE_IMPORT_DESCRIPTOR *)((UINT)DllInfo->Image + dirrva);
    else
        dbglog("no imports\n");
    
    dirsize = nt_hdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
    dirrva  = nt_hdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    if(dirsize)
        DllInfo->Relocations = (IMAGE_RELOCATION *)((UINT)DllInfo->Image + dirrva);
    else
        dbglog("no relocations\n");

    close(fhandle);

    PE_Relocate(DllInfo);

    PE_FixupImports(DllInfo);

    return DllInfo;

free_and_fail:
    free(DllInfo->Image);
    free(DllInfo);
fail:
    close(fhandle);
    DllInfo = NULL;
    return DllInfo;
}

#ifdef __cplusplus
}
#endif
