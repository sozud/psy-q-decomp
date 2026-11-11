#if VERSION <= 30

#include "libcd.h"

#undef CdlMAXFILE
#undef CdlMAXDIR
#define CdlMAXFILE 40
#define CdlMAXDIR 45

typedef union {
    s32 addr;
    struct {
        u8 unk0, unk1, unk2, unk3;
    } i;
} LBA;

typedef struct {
    s32 unk0;
    s32 unk4;
    LBA unk8;
    char unkC[0x20];
} CdlDIR;

#if VERSION == 26
typedef struct {
    u16 lo, hi;
} LBA16;

static int first = 1;
static int dir_max;
static int file_max;
static int current;
static int first_flag;
static int find_file;
static int find_dir;
static CdlFILE file[CdlMAXFILE];
static CdlDIR dire[CdlMAXDIR];
static char work1[128];
static char work2[128];
static u8 load_buf[0x800];

extern int CD_debug;

int printf(const char*, ...);
int strncmp(const char*, const char*, u32);
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);

static int CD_newmedia();
static int CD_searchdir();
static int CD_cachefile();
static int _cmp();
static int cd_read();
static void memcpy();
void getpos();

CdlFILE* CdSearchFile(CdlFILE* fp, char* name) {
    char buf[32];
    CdlFILE* f;
    int var_a0;
    int i;
    char* namePtr;
    char* bufPtr;

    if (first) {
        if (CD_newmedia() == 0) {
            return NULL;
        }
        first = 0;
    }
    if (name[0] != '\\') {
        return NULL;
    }
    strcpy(work1, name);
    buf[0] = 0;
    var_a0 = 1;
    namePtr = work1 + 1;
    for (i = 0; i < CdlMAXLEVEL; i++) {
        bufPtr = buf;
        while (*namePtr != '\\' && *namePtr != 0) {
            *bufPtr++ = *namePtr++;
        }
        if (*namePtr == 0) {
            break;
        }
        namePtr++;
        *bufPtr = 0;
        var_a0 = CD_searchdir(var_a0, buf);
        if (var_a0 == -1) {
            buf[0] = 0;
            break;
        }
    }
    if (i >= CdlMAXLEVEL) {
        if (CD_debug) {
            printf("CD_sarchfile: path level (%d) error\n", i);
        }
        return NULL;
    }
    if (buf[0] == 0) {
        if (CD_debug) {
            printf("CD_sarchfile: dir (%s) was not found\n", work1);
        }
        return NULL;
    }
    *bufPtr = 0;
    if (CD_cachefile(var_a0) == 0) {
        if (CD_debug) {
            printf("CD_sarchfile: disc error\n");
        }
        return NULL;
    }
    if (CD_debug) {
        printf("CD_sarchfile: searching %s...\n", buf);
    }
    for (i = 0; i < file_max; i++) {
        f = &file[i];
        if (_cmp(f->name, buf)) {
            find_file = i;
            if (CD_debug) {
                printf("CD_searchifile: %s found\n", buf);
            }
            *fp = *f;
            return f;
        }
    }
    if (CD_debug) {
        printf("CD_searchfile: %s not found\n", buf);
    }
    return NULL;
}

static int _cmp(const char* str1, const char* str2) {
    return strncmp(str1, str2, 0xC) == 0;
}

static int CD_newmedia(void) {
    LBA16 root[2];
    CdlDIR* d;
    u8* ptr;
    u8* buf;
    int i;
    int n;

    if (cd_read(1, 0x10, (u_long*)load_buf) != 1) {
        if (CD_debug) {
            printf("CD_newmedia: Read error in cd_read( PVD )\n");
        }
        return 0;
    }
    if (strncmp((char*)&load_buf[1], "CD001", 5) != 0) {
        if (CD_debug) {
            printf("CD_newmedia: Disc format error in cd_read( PVD )\n");
        }
        return 0;
    }
    buf = load_buf;
    root[1] = *(LBA16*)&load_buf[0x9E];
    if (cd_read(1, *(int*)&load_buf[0x8C], (u_long*)load_buf) != 1) {
        if (CD_debug) {
            printf("CD_newmedia: Read error ( PT:%08x )\n", *(int*)&load_buf[0x8C]);
        }
        return 0;
    }
    if (CD_debug) {
        printf("CD_newmedia: sarching dir..\n");
    }
    d = dire;
    ptr = buf;
    i = 1;
    while (ptr < load_buf + 0x800 && i < CdlMAXDIR) {
        if (ptr[0] == 0) {
            break;
        }
        d->unk8.i = ((LBA*)&ptr[2])->i;
        d->unk4 = ptr[6];
        d->unk0 = i;
        n = ptr[0];
        memcpy(d->unkC, (char*)&ptr[8], n);
        d->unkC[n] = '\0';
        i++;
        if (ptr[0] % 2 == 1) {
            ptr += ptr[0] + 9;
        } else {
            ptr += ptr[0] + 8;
        }
        if (CD_debug) {
            printf("\t%08x,%04x,%04x,%s\n", d->unk8.addr, d->unk0, d->unk4,
                   d->unkC);
        }
        d++;
    }
    dir_max = i - 1;
    current = 0;
    if (CD_debug) {
        printf("CD_newmedia: %d dir entries found\n", i - 1);
    }
    return 1;
}

static int CD_searchdir(int arg0, char* arg1) {
    CdlDIR* d;
    int i;

    d = dire;
    for (i = 0; i < dir_max; i++, d++) {
        if (d->unk4 == arg0 && strcmp(arg1, d->unkC) == 0) {
            return i + 1;
        }
    }
    return -1;
}

static int CD_cachefile(int arg0) {
    int sp18;
    CdlFILE* f;
    u8* ptr;
    int i;
    int n;

    if (arg0 == current) {
        return 1;
    }
    if (cd_read(1, (dire - 1)[arg0].unk8.addr, (u_long*)load_buf) != 1) {
        if (CD_debug) {
            printf("CD_cachefile: dir not found\n");
        }
        return -1;
    }
    if (CD_debug) {
        printf("CD_cachefile: searching...\n");
    }
    i = 0;
    f = file;
    ptr = load_buf;
    while (ptr < load_buf + 0x800 && f < file + CdlMAXFILE) {
        if (ptr[0] == 0) {
            break;
        }
        ((LBA*)&sp18)->i = ((LBA*)&ptr[2])->i;
        ((LBA*)&f->size)->i = ((LBA*)&ptr[0xA])->i;
        getpos(sp18, &f->pos);
        n = ptr[0x20];
        memcpy(f->name, (char*)&ptr[0x21], n);
        f->name[n] = '\0';
        if (CD_debug) {
            printf("\t(%02x:%02x:%02x) %8d %s\n", f->pos.minute,
                   f->pos.second, f->pos.sector, f->size, f->name);
        }
        f++;
        ptr += ptr[0];
        i++;
    }
    current = arg0;
    file_max = i;
    if (CD_debug) {
        printf("CD_cachefile: %d files found\n", i);
    }
    return 1;
}

static void memcpy(char* dst, char* src, u32 size) {
    while (size--) {
        *dst++ = *src++;
    }
}

static int cd_read(int sectors, int arg1, u_long* buf) {
    CdlLOC p;
    int ret;
    int i;

    getpos(arg1, &p);
    i = 0;
    do {
        if (CD_debug) {
            printf("cd_read: (%02x:%02x:%02x), %d sector(s)...\n", p.minute,
                   p.second, p.sector, sectors);
        }
        CdControl(CdlSetloc, (u8*)&p, NULL);
        CdRead(sectors, buf, CdlModeSpeed);
        while ((ret = CdReadSync(1, NULL)) > 0) {
        }
    } while (++i < 20 && ret != 0);
    return ret == 0;
}

static int cd_status(void) {
    u_char result[8];

    CdControlB(CdlNop, NULL, result);
    return result[0];
}

void getpos(int i, CdlLOC* p) {
    int min;
    int sec;
    int mh, ml, sh, sl;

    i += 150;
    min = i / (75 * 60);
    mh = min / 10;
    ml = min % 10;
    i -= min * (75 * 60);
    sec = i / 75;
    sh = sec / 10;
    sl = sec % 10;
    i -= sec * 75;
    p->minute = ml + (mh << 4);
    p->second = sl + (sh << 4);
    p->sector = i % 10 + ((i / 10) << 4);
}

#else
static int first = 1;
static CdlFILE file[CdlMAXFILE];
static CdlDIR dire[CdlMAXDIR];
static u8 load_buf[0x800];
static int current;

extern int CD_debug;

int printf(const char*, ...);
int strncmp(const char*, const char*, u32);
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);

static int CD_newmedia();
static int CD_searchdir();
static int CD_cachefile();
static int _cmp();
static int cd_read();
static void memcpy();

CdlFILE* CdSearchFile(CdlFILE* fp, char* name) {
    char buf[32];
    int var_a0;
    int i;
    char* namePtr;
    char* bufPtr;

    if (first) {
        if (CD_newmedia() == 0) {
            return NULL;
        }
        first = 0;
    }
    var_a0 = 1;
    if (name[0] != '\\') {
        return NULL;
    }
    buf[0] = 0;
    namePtr = name;
    for (i = 0; i < CdlMAXLEVEL; i++) {
        bufPtr = &buf;
        while (*namePtr != '\\' && *namePtr != 0) {
            *bufPtr++ = *namePtr++;
        }
        if (*namePtr == 0) {
            goto out;
        }
        namePtr++;
        *bufPtr = 0;
        var_a0 = CD_searchdir(var_a0, buf);
        if (var_a0 == -1) {
            buf[0] = 0;
            break;
        }
    }
out:
    if (i >= CdlMAXLEVEL) {
        if (CD_debug > 0) {
            printf("%s: path level (%d) error\n", name, i);
        }
        return NULL;
    }
    if (buf[0] == 0) {
        if (CD_debug > 0) {
            printf("%s: dir was not found\n", name);
        }
        return NULL;
    }
    *bufPtr = 0;
    if (CD_cachefile(var_a0) == 0) {
        if (CD_debug > 0) {
            printf("CdSearchFile: disc error\n");
        }
        return NULL;
    }
    if (CD_debug > 1) {
        printf("CdSearchFile: searching %s...\n", buf);
    }
    for (i = 0; i < CdlMAXFILE && file[i].name[0] != 0; i++) {
        if (_cmp(file[i].name, buf)) {
            if (CD_debug > 1) {
                printf("%s:  found\n", buf);
            }
            *fp = file[i];
            return &file[i];
        }
    }
    if (CD_debug > 0) {
        printf("%s: not found\n", buf);
    }
    return NULL;
}

static int _cmp(const char* str1, const char* str2) {
    return strncmp(str1, str2, 0xC) == 0;
}

static s32 CD_newmedia(void) {
    LBA pathTableLBA;
    u8* pathTablePtr;
    s32 i;

    if (cd_read(1, 0x10, (u_long*)load_buf) != 1) {
        if (CD_debug > 0) {
            printf("CD_newmedia: Read error in cd_read(PVD)\n");
        }
        return 0;
    }
    if (strncmp((char*)&load_buf[1], "CD001", 5) != 0) {
        if (CD_debug > 0) {
            printf("CD_newmedia: Disc format error in cd_read(PVD)\n");
        }
        return 0;
    }
    (&pathTableLBA)->i = ((LBA*)(&load_buf[0x8C]))->i;
    if (cd_read(1, pathTableLBA.addr, (u_long*)load_buf) != 1) {
        if (CD_debug > 0) {
            printf("CD_newmedia: Read error (PT:%08x)\n", pathTableLBA.addr);
        }
        return 0;
    }
    if (CD_debug > 1) {
        printf("CD_newmedia: sarching dir..\n");
    }
    i = 0;
    pathTablePtr = load_buf;
    while (i < CdlMAXDIR && pathTablePtr < load_buf + 0x800 &&
           pathTablePtr[0] != 0) {
        dire[i].unk8.i = ((LBA*)&pathTablePtr[2])->i;
        dire[i].unk4 = pathTablePtr[6];
        dire[i].unk0 = i + 1;
        memcpy(dire[i].unkC, (char*)&pathTablePtr[8], pathTablePtr[0]);
        dire[i].unkC[pathTablePtr[0]] = '\0';
        pathTablePtr += 8 + pathTablePtr[0] + pathTablePtr[0] % 2;
        if (CD_debug > 1) {
            printf("\t%08x,%04x,%04x,%s\n", dire[i].unk8.addr, dire[i].unk0,
                   dire[i].unk4, dire[i].unkC);
        }
        i++;
    }
    if (i < CdlMAXDIR) {
        dire[i].unk4 = 0;
    }
    current = 0;
    if (CD_debug > 1) {
        printf("CD_newmedia: %d dir entries found\n", i);
    }
    return 1;
}

static s32 CD_searchdir(s32 arg0, char* arg1) {
    s32 i;

    for (i = 0; i < CdlMAXDIR && dire[i].unk4 != 0; i++) {
        if (dire[i].unk4 == arg0 && strcmp(arg1, dire[i].unkC) == 0) {
            return i + 1;
        }
    }
    return -1;
}

static s32 CD_cachefile(s32 arg0) {
    LBA sp18;
    u8* ptr;
    s32 i;
    short* namePtr;

    if (arg0 == current) {
        return 1;
    }
    if (cd_read(1, (dire - 1)[arg0].unk8.addr, (u_long*)load_buf) != 1) {
        if (CD_debug > 0) {
            printf("CD_cachefile: dir not found\n");
        }
        return -1;
    }
    if (CD_debug > 1) {
        printf("CD_cachefile: searching...\n");
    }
    i = 0;
    ptr = load_buf;
    while (i < CdlMAXFILE && ptr < load_buf + 0x800 && ptr[0] != 0) {
        sp18.i = ((LBA*)&ptr[2])->i;
        CdIntToPos(sp18.addr, &file[i].pos);
        ((LBA*)&file[i].size)->i = ((LBA*)&ptr[0xA])->i;
        switch (i) {
        case 0:
            namePtr = file[i].name;
            strcpy(namePtr, ".");
            break;
        case 1:
            namePtr = file[i].name;
            strcpy(namePtr, "..");
            break;
        default:
            memcpy(file[i].name, (char*)&ptr[0x21], ptr[0x20]);
            file[i].name[ptr[0x20]] = '\0';
            break;
        }
        if (CD_debug > 1) {
            printf("\t(%02x:%02x:%02x) %8d %s\n", file[i].pos.minute,
                   file[i].pos.second, file[i].pos.sector, file[i].size,
                   file[i].name);
        }
        ptr += ptr[0];
        i++;
    }
    current = arg0;
    if (i < CdlMAXFILE) {
        file[i].name[0] = '\0';
    }
    if (CD_debug > 1) {
        printf("CD_cachefile: %d files found\n", i);
    }
    return 1;
}

static void memcpy(char* dst, char* src, u32 size) {
    while (size--) {
        *dst++ = *src++;
    }
}

static int cd_read(int sectors, int arg1, u_long* buf) {
    CdlLOC p;
    int ret;

    CdControl(CdlSetloc, (u8*)CdIntToPos(arg1, &p), NULL);
    CdRead(sectors, buf, CdlModeSpeed);
    while ((ret = CdReadSync(1, NULL)) > 0) {
    }
    return ret == 0;
}
#endif
#else
#include "libcd.h"

typedef union {
    s32 addr;
    struct {
        u8 unk0, unk1, unk2, unk3;
    } i;
} LBA;

typedef struct {
    s32 unk0;
    s32 unk4;
    LBA unk8;
    char unkC[0x20];
} CdlDIR;

static CdlFILE file[CdlMAXFILE];
static CdlDIR dire[CdlMAXDIR];
static u8 load_buf[0x800];

extern int CD_nopen;
extern int CD_debug;

static int current = 0;

int printf(const char*, ...);
int strncmp(const char*, const char*, u32);
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);

static int CD_newmedia();
static int CD_searchdir();
static int CD_cachefile();
static int _cmp();
static int cd_read();
#if VERSION == 40
void* memcpy();
#else
static void memcpy();
#endif

CdlFILE* CdSearchFile(CdlFILE* fp, char* name) {
    char buf[32];
    int var_a0;
    int i;
    char* namePtr;
    char* bufPtr;
    static int pre_nopen = -1;

    if (pre_nopen != CD_nopen) {
        if (CD_newmedia() == 0) {
            return NULL;
        }
        pre_nopen = CD_nopen;
    }
    var_a0 = 1;
    if (name[0] != '\\') {
        return NULL;
    }
    buf[0] = 0;
    namePtr = name;
    for (i = 0; i < CdlMAXLEVEL; i++) {
        bufPtr = &buf;
        while (*namePtr != '\\') {
            if (*namePtr == 0) {
                goto out;
            }
            *bufPtr++ = *namePtr++;
        }
        if (*namePtr == 0) {
            goto out;
        }
        namePtr++;
        *bufPtr = 0;
        var_a0 = CD_searchdir(var_a0, buf);
        if (var_a0 == -1) {
            buf[0] = 0;
            break;
        }
    }
out:
    if (i >= CdlMAXLEVEL) {
        if (CD_debug > 0) {
            printf("%s: path level (%d) error\n", name, i);
        }
        return NULL;
    }
    if (buf[0] == 0) {
        if (CD_debug > 0) {
            printf("%s: dir was not found\n", name);
        }
        return NULL;
    }
    *bufPtr = 0;
    if (CD_cachefile(var_a0) == 0) {
        if (CD_debug > 0) {
            printf("CdSearchFile: disc error\n");
        }
        return NULL;
    }
    if (CD_debug > 1) {
        printf("CdSearchFile: searching %s...\n", buf);
    }
    for (i = 0; i < CdlMAXFILE; i++) {
        if (file[i].name[0] == 0) {
            break;
        }
        if (_cmp(file[i].name, buf)) {
            if (CD_debug > 1) {
                printf("%s:  found\n", buf);
            }
            *fp = file[i];
            return &file[i];
        }
    }
    if (CD_debug > 0) {
        printf("%s: not found\n", buf);
    }
    return NULL;
}

static int _cmp(const char* str1, const char* str2) {
    return strncmp(str1, str2, 0xC) == 0;
}

static s32 CD_newmedia(void) {
    LBA pathTableLBA;
    u8* pathTablePtr;
    s32 i;

    if (cd_read(1, 0x10, (u_long*)load_buf) != 1) {
        if (CD_debug > 0) {
            printf("CD_newmedia: Read error in cd_read(PVD)\n");
        }
        return 0;
    }
    if (strncmp((char*)&load_buf[1], "CD001", 5) != 0) {
        if (CD_debug > 0) {
            printf("CD_newmedia: Disc format error in cd_read(PVD)\n");
        }
        return 0;
    }
    (&pathTableLBA)->i = ((LBA*)(&load_buf[0x8C]))->i;
    if (cd_read(1, pathTableLBA.addr, (u_long*)load_buf) != 1) {
        if (CD_debug > 0) {
            printf("CD_newmedia: Read error (PT:%08x)\n", pathTableLBA.addr);
        }
        return 0;
    }
    if (CD_debug > 1) {
        printf("CD_newmedia: sarching dir..\n");
    }
    do {
        i = 0;
        pathTablePtr = load_buf;
        while (pathTablePtr < load_buf + 0x800) {
            if (pathTablePtr[0] == 0) {
                break;
            }
            dire[i].unk8.i = ((LBA*)&pathTablePtr[2])->i;
            dire[i].unk4 = pathTablePtr[6];
            dire[i].unk0 = i + 1;
            memcpy(dire[i].unkC, (char*)&pathTablePtr[8], pathTablePtr[0]);
            dire[i].unkC[pathTablePtr[0]] = '\0';
            pathTablePtr += 8 + pathTablePtr[0] + pathTablePtr[0] % 2;
            if (CD_debug > 1) {
                printf("\t%08x,%04x,%04x,%s\n", dire[i].unk8.addr,
                       dire[i].unk0, dire[i].unk4, dire[i].unkC);
            }
            if (++i >= CdlMAXDIR) {
                break;
            }
        }
        if (i < CdlMAXDIR) {
            dire[i].unk4 = 0;
        }
    } while (0);

    current = 0;
    if (CD_debug > 1) {
        printf("CD_newmedia: %d dir entries found\n", i);
    }
    return 1;
}

static s32 CD_searchdir(s32 arg0, char* arg1) {
    s32 i;

    for (i = 0; i < CdlMAXDIR; i++) {
        if (dire[i].unk4 == 0) {
            return -1;
        }
        if (dire[i].unk4 != arg0) {
            continue;
        }
        if (strcmp(arg1, dire[i].unkC) == 0) {
            return i + 1;
        }
    }
    return -1;
}

static s32 CD_cachefile(s32 arg0) {
    LBA sp18;
    u8* ptr;
    s32 i;
    short* namePtr;

    if (arg0 == current) {
        return 1;
    }
    if (cd_read(1, (dire - 1)[arg0].unk8.addr, (u_long*)load_buf) != 1) {
        if (CD_debug > 0) {
            printf("CD_cachefile: dir not found\n");
        }
        return -1;
    }
    if (CD_debug > 1) {
        printf("CD_cachefile: searching...\n");
    }
    i = 0;
    ptr = load_buf;
    while (ptr < load_buf + 0x800) {
        if (ptr[0] == 0) {
            break;
        }
        sp18.i = ((LBA*)&ptr[2])->i;
        CdIntToPos(sp18.addr, &file[i].pos);
        ((LBA*)&file[i].size)->i = ((LBA*)&ptr[0xA])->i;
        switch (i) {
        case 0:
            namePtr = file[i].name;
            strcpy(namePtr, ".");
            break;
        case 1:
            namePtr = file[i].name;
            strcpy(namePtr, "..");
            break;
        default:
            memcpy(file[i].name, (char*)&ptr[0x21], ptr[0x20]);
            file[i].name[ptr[0x20]] = '\0';
            break;
        }
        if (CD_debug > 1) {
            printf("\t(%02x:%02x:%02x) %8d %s\n", file[i].pos.minute,
                   file[i].pos.second, file[i].pos.sector, file[i].size,
                   file[i].name);
        }
        ptr += ptr[0];
        if (++i >= CdlMAXFILE) {
            break;
        }
    }
    current = arg0;
    if (i < CdlMAXFILE) {
        file[i].name[0] = '\0';
    }
    if (CD_debug > 1) {
        printf("CD_cachefile: %d files found\n", i);
    }
    return 1;
}

static int cd_read(int sectors, int arg1, u_long* buf) {
    CdlLOC p;

    CdIntToPos(arg1, &p);
    CdControl(CdlSetloc, (u8*)&p, NULL);
    CdRead(sectors, buf, CdlModeSpeed | CdlModeSpeedNormal);
    return CdReadSync(CdlSync, NULL) == 0;
}

#if VERSION != 40
static void memcpy(char* dst, char* src, u32 size) {
    while (size--) {
        *dst++ = *src++;
    }
}
#endif
#endif
