/*
 * coff_pe.h
 * Copyright 1999, Yurij  Sysoev, All rights reserved
 * Copyright Rainer Schnitker 92,93
 */
#ifndef _COFF_PE
#define _COFF_PE

#include "license.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LDD_TYPES
#include "ld_types.h"
#endif

#ifdef unix
#define P(a) a __attribute__((packed))
#else
#define P(a) a
#endif

#define N_MAGIC(exec) ((exec).a_info & 0xffff)
#define OMAGIC 0x107
#define NMAGIC 0x108
#define ZMAGIC 0x10B

#define PAGE_SIZE	4096

/* load different sizes */
#define _N_HDROFF(x)    _n_hdroff
#define N_TXTADDR(x)    n_txtaddr
#define SEGMENT_SIZE    segment_size

#define N_DATADDR(x) (SEGMENT_SIZE + ((N_TXTADDR(x)+(x).a_text-1) & ~(SEGMENT_SIZE-1)))
#define N_BSSADDR(x) (N_DATADDR(x) + (x).a_data)

#define N_TXTOFF(x) (_N_HDROFF((x)) + sizeof (struct exec))
#define N_DATOFF(x) (N_TXTOFF(x) + (x).a_text)
#define N_TRELOFF(x) (N_DATOFF(x) + (x).a_data)
#define N_DRELOFF(x) (N_TRELOFF(x) + (x).a_trsize)
#define N_SYMOFF(x) (N_DRELOFF(x) + (x).a_drsize)
#define N_STROFF(x) (N_SYMOFF(x) + (x).a_syms)

struct exe_hdr {
    UINT16 signatur;
    UINT16 low;
    UINT16 high;
    UINT16 reloc;
    UINT16 hdr_para;
};
#define EXE_HDR struct  exe_hdr
#define EXE_HSZ sizeof(EXE_HDR)

struct stub_hdr2 {
    char sig[18];
    char next_hdr[4];
    char option[64];
};
#define STUB_HDR2 struct stub_hdr2
#define STUB_HDSZ sizeof(STUB_HDR2)

struct exec {
    UINT32 a_info;     /* Use macros N_MAGIC, etc for access */
    UINT32 a_text;     /* length of text, in bytes */
    UINT32 a_data;     /* length of data, in bytes */
    UINT32 a_bss;      /* length of uninitialized data area for file, in bytes */
    UINT32 a_syms;     /* length of symbol table data in file, in bytes */
    UINT32 a_entry;    /* start address */
    UINT32 a_trsize;   /* length of relocation info for text, in bytes */
    UINT32 a_drsize;   /* length of relocation info for data, in bytes */
};
#define GNUOUT      struct exec
#define GNUOUTSZ    sizeof(GNUOUT)

//NT specific
struct IMAGE_DATA_DIRECTORY
{
   UINT32   VirtualAddress;
   UINT32   Size;
};
#define IMAGE_DATA_DIRECTORY struct IMAGE_DATA_DIRECTORY

/*\
PE/COFF image header
WIN32 exe/dll have PE\0x00\0x00 additional signature above (DWORD)
\*/
struct filehdr {
    UINT16  f_magic; //0x14c:I386,
                     //0x16x:MIPS,
                     //0x184:ALPHA,
                     //0x1F0:PowerPC
    UINT16  f_nscns; //NumberOfSections
    UINT32  f_timdat;//time & date stamp
    UINT32  f_symptr;//file pointer to symtab
    UINT32  f_nsyms; //number of symtab entries
    UINT16  f_opthdr;//sizeof(optional hdr
    UINT16  f_flags;
};
#define FILEHDR struct filehdr
#define FILEHSZ sizeof(FILEHDR)

/* Bits for f_flags:
 *  F_RELFLG    relocation info stripped from file
 *  F_EXEC      file is executable (no unresolved external references)
 *  F_LNNO      line numbers stripped from file
 *  F_LSYMS     local symbols stripped from file
 *  F_AR32WR    file has byte ordering of an AR32WR machine (e.g. vax)
 */

#define F_RELFLG    (0x0001)
#define F_EXEC      (0x0002)
#define F_LNNO      (0x0004)
#define F_LSYMS     (0x0008)

struct  aouthdr{
    UINT16  magic;
    UINT16  vstamp;
    UINT32  tsize;    /* text size in bytes, padded to FW bdry*/
    UINT32  dsize;    /* initialized data "  "                */
    UINT32  bsize;    /* uninitialized data "   "             */
    UINT32  entry;    /* entry pt.                */
    UINT32  text_start;   /* base of text used for this file  */
    UINT32  data_start;   /* base of data used for this file  */
}; //Head of IMAGE_OPTIONAL_HEADER ( COFF & PE )
#define AOUTHDR struct aouthdr
#define AOUTHSZ sizeof(AOUTHDR)

struct nt_hdr {
    UINT32  ImageBase;
    UINT32  SectionAlignment;
    UINT32  FileAlignment;
    UINT16  MajorOperatingSystemVersion;
    UINT16  MinorOperatingSystemVersion;
    UINT16  MajorImageVersion;
    UINT16  MinorImageVersion;
    UINT16  MajorSubsystemVersion;
    UINT16  MinorSubsystemVersion;
    UINT32  Reserved1;
    UINT32  SizeOfImage;
    UINT32  SizeOfHeaders;
    UINT32  CheckSum;
    UINT16  Subsystem;
    //!!!
    UINT16  DllCharacteristics;
    UINT32  SizeOfStackReserve;
    UINT32  SizeOfStackCommit;
    UINT32  SizeOfHeapReserve;
    UINT32  SizeOfHeapCommit;
    UINT32  LoaderFlags;
    UINT32  NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];

};  //Tail of IMAGE_OPTIONAL_HEADER (PE)
#define NT_HDR struct nt_hdr
#define NT_HDRSZ sizeof(NT_HDR)

/* These are indexes into the DataDirectory array (PE) */
#define IMAGE_DIRECTORY_ENTRY_EXPORT           0
#define IMAGE_DIRECTORY_ENTRY_IMPORT           1
#define IMAGE_DIRECTORY_ENTRY_RESOURCE         2
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION        3
#define IMAGE_DIRECTORY_ENTRY_SECURITY         4
#define IMAGE_DIRECTORY_ENTRY_BASERELOC        5
#define IMAGE_DIRECTORY_ENTRY_DEBUG            6
#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT        7
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR        8   /* Mips */
#define IMAGE_DIRECTORY_ENTRY_TLS              9
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG      10

struct scnhdr {
    char    s_name[8];/* section name         */
    UINT32  s_paddr;  /* virtual size when loaded */
    UINT32  s_vaddr;  /* virtual address      */
    UINT32  s_size;   /* raw size             */
    UINT32  s_scnptr; /* file ptr to raw data for section */
    UINT32  s_relptr; /* file ptr to relocation   */
    UINT32  s_lnnoptr;/* file ptr to line numbers */
    UINT16  s_nreloc; /* number of relocation entries */
    UINT16  s_nlnno;  /* number of line number entries*/
    UINT32  s_flags;  /* flags            */
};

#define SCNHDR struct scnhdr
#define SCNHDRSZ sizeof(SCNHDR)

/*
 * names of "special" sections
 */
#define _TEXT       ".text"
#define _DATA       ".data"
#define _BSS        ".bss"
#define _COMMENT    ".comment"
#define _LIB        ".lib"
#define _IMPORT     ".idata"
#define _EXPORT     ".edata"


/*
 * s_flags "type"
 */
#define STYP_TEXT    (0x0020)   /* section contains text only */
#define STYP_DATA    (0x0040)   /* section contains data only */
#define STYP_BSS     (0x0080)   /* section contains bss only */
/*
 * Win PE specific
 */
#define IMAGE_SCN_CNT_CODE                  STYP_TEXT
#define IMAGE_SCN_CNT_INITIALIZED_DATA      STYP_DATA
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA    STYP_BSS
#define IMAGE_SCN_MEM_DISCARDABLE           0x2000000
#define IMAGE_SCN_MEM_SHARED 	            0x10000000
#define IMAGE_SCN_MEM_EXECUTE	            0x20000000
#define IMAGE_SCN_MEM_READ	                0x40000000
#define IMAGE_SCN_MEM_WRITE	                0x80000000


/********************** COFF LINE NUMBERS **********************/

/* 1 line number entry for every "breakpointable" source line in a section.
 * Line numbers are grouped on a per function basis; first entry in a function
 * grouping will have l_lnno = 0 and in place of physical address will be the
 * symbol table index of the function name.
 */
struct external_lineno {
    union {
        UINT32 P(l_symndx); /* function name symbol index, iff l_lnno == 0 */
        UINT32 P(l_paddr);  /* (physical) address of line number */
    } l_addr;
    UINT16 l_lnno;        /* line number */
};

#define LINENO  struct external_lineno
#define LINESZ  sizeof(LINENO)


/********************** COFF SYMBOLS **********************/

#define E_SYMNMLEN  8   /* # characters in a symbol name    */
#define E_FILNMLEN  14  /* # characters in a file name      */
#define E_DIMNUM    4   /* # array dimensions in auxiliary entry */

struct external_syment 
{
    union {
        char e_name[E_SYMNMLEN];
        struct {
            UINT32 P(e_zeroes);
            UINT32 P(e_offset);
        } e;
    } e;
    UINT32 P(e_value);
    UINT16  e_scnum;
    UINT16  e_type;
    BYTE  e_sclass;
    BYTE  e_numaux;
};

#define N_BTMASK    (0xf)
#define N_TMASK     (0x30)
#define N_BTSHFT    (4)
#define N_TSHIFT    (2)
  
union external_auxent {
    struct {
        UINT32 P(x_tagndx);         /* str, un, or enum tag indx */
        union {
            struct {
                UINT16  x_lnno;     /* declaration line number */
                UINT16  x_size;     /* str/union/array size */
            } x_lnsz;
            P(UINT32 x_fsize);      /* size of function */
        } x_misc;
        union {
            struct {                /* if ISFCN, tag, or .bb */
                UINT32 P(x_lnnoptr);/* ptr to fcn line # */
                UINT32 P(x_endndx); /* entry ndx past block end */
            } x_fcn;
            struct {                /* if ISARY, up to 4 dimen. */
                UINT16 x_dimen[E_DIMNUM];
            } x_ary;
        } x_fcnary;
        UINT16 x_tvndx;             /* tv index */
    } x_sym;

    union {
        char x_fname[E_FILNMLEN];
        struct {
            UINT32 P(x_zeroes);
            UINT32 P(x_offset);
        } x_n;
    } x_file;

    struct {
        UINT32 P(x_scnlen);       /* section length */
        UINT16 x_nreloc;          /* # relocation entries */
        UINT16 x_nlinno;          /* # line numbers */
    } x_scn;

    struct {
        UINT32 P(x_tvfill);       /* tv fill value */
        UINT16 x_tvlen;           /* length of .tv */
        UINT16 x_tvran[2];        /* tv range */
    } x_tv;                        /* info about .tv section
                                   (in auxent of symbol .tv)) */
};

#define SYMENT  struct external_syment
#define SYMESZ  sizeof(SYMENT)
#define AUXENT  union external_auxent
#define AUXESZ  sizeof(AUXENT)


#   define _ETEXT   "etext"


/* Relocatable symbols have number of the section in which they are defined,
   or one of the following: */

#define N_UNDEF ((short)0)  /* undefined symbol */
#define N_ABS   ((short)-1) /* value of symbol is absolute */
#define N_DEBUG ((short)-2) /* debugging symbol -- value is meaningless */
#define N_TV    ((short)-3) /* indicates symbol needs preload transfer vector */
#define P_TV    ((short)-4) /* indicates symbol needs postload transfer vector*/

/*
 * Type of a symbol, in low N bits of the word
 */
#define T_NULL      0
#define T_VOID      1   /* function argument (only used by compiler) */
#define T_CHAR      2   /* character        */
#define T_SHORT     3   /* short integer    */
#define T_INT       4   /* integer          */
#define T_LONG      5   /* long integer     */
#define T_FLOAT     6   /* floating point   */
#define T_DOUBLE    7   /* double word      */
#define T_STRUCT    8   /* structure        */
#define T_UNION     9   /* union            */
#define T_ENUM      10  /* enumeration      */
#define T_MOE       11  /* member of enumeration*/
#define T_UCHAR     12  /* unsigned character   */
#define T_USHORT    13  /* unsigned short   */
#define T_UINT      14  /* unsigned integer */
#define T_ULONG     15  /* unsigned long    */
#define T_LNGDBL    16  /* long double      */

/*
 * derived types, in n_type
*/
#define DT_NON      (0) /* no derived type */
#define DT_PTR      (1) /* pointer  */
#define DT_FCN      (2) /* function */
#define DT_ARY      (3) /* array    */

#define BTYPE(x)    ((x) & N_BTMASK)

#define ISPTR(x)    (((x) & N_TMASK) == (DT_PTR << N_BTSHFT))
#define ISFCN(x)    (((x) & N_TMASK) == (DT_FCN << N_BTSHFT))
#define ISARY(x)    (((x) & N_TMASK) == (DT_ARY << N_BTSHFT))
#define ISTAG(x)    ((x)==C_STRTAG||(x)==C_UNTAG||(x)==C_ENTAG)
#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

/********************** STORAGE CLASSES **********************/

/* This used to be defined as -1, but now n_sclass is unsigned.  */
#define C_EFCN      0xff/* physical end of function */
#define C_NULL      0
#define C_AUTO      1   /* automatic variable   */
#define C_EXT       2   /* external symbol      */
#define C_STAT      3   /* static               */
#define C_REG       4   /* register variable    */
#define C_EXTDEF    5   /* external definition  */
#define C_LABEL     6   /* label                */
#define C_ULABEL    7   /* undefined label      */
#define C_MOS       8   /* member of structure  */
#define C_ARG       9   /* function argument    */
#define C_STRTAG    10  /* structure tag        */
#define C_MOU       11  /* member of union      */
#define C_UNTAG     12  /* union tag            */
#define C_TPDEF     13  /* type definition      */
#define C_USTATIC   14  /* undefined static     */
#define C_ENTAG     15  /* enumeration tag      */
#define C_MOE       16  /* member of enumeration*/
#define C_REGPARM   17  /* register parameter   */
#define C_FIELD     18  /* bit field            */
#define C_AUTOARG   19  /* auto argument        */
#define C_LASTENT   20  /* dummy entry (end of block) */
#define C_BLOCK     100 /* ".bb" or ".eb"       */
#define C_FCN       101 /* ".bf" or ".ef"       */
#define C_EOS       102 /* end of structure     */
#define C_FILE      103 /* file name            */
#define C_LINE      104 /* line # reformatted as symbol table entry */
#define C_ALIAS     105 /* duplicate tag        */
#define C_HIDDEN    106 /* ext symbol in dmert public lib */

/********************** RELOCATION DIRECTIVES **********************/

struct external_reloc {
    UINT32   P(r_vaddr);   /* section relative offset */
    UINT32   P(r_symndx);  /* [ndx] in symbol array*/
    UINT16    r_type;
};
#define RELOC struct external_reloc
#define RELSZ sizeof(RELOC)

#define RELOC_REL32     20  /* 32-bit PC-relative address */
#define RELOC_ADDR32    6   /* 32-bit absolute address */

#define DEFAULT_DATA_SECTION_ALIGNMENT 4
#define DEFAULT_BSS_SECTION_ALIGNMENT  4
#define DEFAULT_TEXT_SECTION_ALIGNMENT 4
/* For new sections we havn't heard of before */
#define DEFAULT_SECTION_ALIGNMENT      4

/*
 * Import module directory stuff
 */
typedef struct __IMAGE_IMPORT_DESCRIPTOR 
{
    UINT32   Characteristics;
    UINT32   TimeDateStamp;
    UINT32   ForwarderChain;
    UINT32   Name;
    void   *FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;

typedef struct __IMAGE_IMPORT_BY_NAME
{
    UINT16    Hint;
    char    Name[1];
} IMAGE_IMPORT_BY_NAME;

/*
 * Export module directory stuff
 */

typedef struct __IMAGE_EXPORT_DIRECTORY
{
  UINT32     Characteristics;
  UINT32     TimeDateStamp;
  UINT16     MajorVersion;
  UINT16     MinorVersion;
  UINT32     Name;
  UINT32     Base;
  UINT32     NumberOfFunctions;
  UINT32     NumberOfNames;
  UINT32    *AddressOfFunctions;
  UINT32    *AddressOfNames;
  UINT32    *AddressOfNameOrdinals;
/*  UCHAR ModuleName[1]; */
} IMAGE_EXPORT_DIRECTORY;

typedef struct __IMAGE_BASE_RELOCATION
{
    UINT32   VirtualAddress;       /* virtual address of page */
    UINT32   SymbolTableIndex;     /* actually the size of this block */
    UINT16   Type[1];              /* actually the relocations */
} IMAGE_RELOCATION;

typedef struct __IMAGE_BASE_RELOCATION *PIMAGE_BASE_RELOCATION;

#define IMAGE_REL_BASED_ABSOLUTE                0
#define IMAGE_REL_BASED_HIGH                    1
#define IMAGE_REL_BASED_LOW                     2
#define IMAGE_REL_BASED_HIGHLOW                 3
#define IMAGE_REL_BASED_HIGHADJ                 4
#define IMAGE_REL_BASED_MIPS_JMPADDR            5

/*
**
** DPMI program layout:
**
**  DPMI 0.9 :   fixed stack
**       never ending heap
**
**  old djgpp style
**  |--------------------------------------------------------------
**  |   |    code    | stack    |  data/bss  | heap -> ...
**  |--------------------------------------------------------------
**  0   ^4K          ^0x400000
**
**  djgpp style 1.11
**  |--------------------------------------------------------------
**  |  code |  data/bss  | stack  | heap -> ...
**  |--------------------------------------------------------------
**  0       ^ 4 Kb align ^
**
**
**  DPMI 1.0 :   address room = 64 MegaBytes
**
**  |--------------------------------------------...-----------------------|
**  |     |  code   |  |  data/bss  |  heap ->        <- stack  | mappings |
**  |     |         |  |            |                           | DOS 1MB  |
**  |--------------------------------------------...-----------------------|
**  0                               ^60    64
*/

#ifdef __cplusplus
}
#endif

#endif //_COFF_PE


