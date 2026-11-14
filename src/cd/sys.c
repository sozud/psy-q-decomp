#if VERSION <= 30
#include "libcd.h"

#define btoi(b) ((b) / 16 * 10 + (b) % 16)
#define itob(i) ((i) / 10 * 16 + (i) % 10)

extern CdlCB CD_cbready;
extern CdlCB CD_cbsync;
extern CdlCB CD_cbread;
extern int CD_debug;
extern int CD_isopen;
extern char* CD_comstr[];
extern char* CD_intstr[];

int CD_init(void);
int CD_sync(int, u_char*);
int CD_ready(int, u_char*);
int CD_readsync(int, u_char*);
int CD_cw(u8, u8*, u8*, s32);
int CD_vol(CdlATV*);
int CD_read(void*, int);
#if VERSION == 26
int CD_readm(u_long*, int, int, int);
#else
int CD_readm(u_long*, int, int);
#endif
long CdDataCallback(void (*)());
void data_ready_callback(void);
void StCdInterrupt(void);

#if VERSION == 26
int CdReset(int mode) { return CD_init(); }

int CdSetDebug(int level) {
    int old = CD_debug;
    CD_debug = level;
    return old;
}

char* CdComstr(u8 com) {
    if (com > 0x1B) {
        return "none";
    }
    return CD_comstr[com];
}

char* CdIntstr(u8 intr) {
    if (intr > 6) {
        return "none";
    }
    return CD_intstr[intr];
}

int CdSync(int mode, u_char* result) { return CD_sync(mode, result); }

int CdReady(int mode, u_char* result) { return CD_ready(mode, result); }

int CdReadSync(int mode, u_char* result) { return CD_readsync(mode, result); }

#else
int CdReset(int mode) { return CD_init(); }

int CdSync(int mode, u_char* result) { return CD_sync(mode, result); }

int CdReady(int mode, u_char* result) { return CD_ready(mode, result); }

int CdReadSync(int mode, u_char* result) { return CD_readsync(mode, result); }

int CdSetDebug(int level) {
    int old = CD_debug;
    CD_debug = level;
    return old;
}

char* CdComstr(u8 com) {
    if (com > 0x1B) {
        return "none";
    }
    return CD_comstr[com];
}

char* CdIntstr(u8 intr) {
    if (intr > 6) {
        return "none";
    }
    return CD_intstr[intr];
}

#endif
CdlCB CdSyncCallback(CdlCB func) {
    CdlCB old = CD_cbsync;
    CD_cbsync = func;
    return old;
}

CdlCB CdReadyCallback(CdlCB func) {
    CdlCB old = CD_cbready;
    CD_cbready = func;
    return old;
}

CdlCB CdReadCallback(CdlCB func) {
    CdlCB old = CD_cbread;
    CD_cbread = func;
    return old;
}

static int setloc[] = {
    0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
};

#if VERSION == 26
static CdlLOC CurrentPos;

static int cd_setloc(u8 com, u8* param, u_char* result) {
    CdlCB old = CdSyncCallback(NULL);

    if (param != NULL && setloc[com] != 0) {
        if (CD_cw(CdlSetloc, param, result, 0)) {
            CdSyncCallback(old);
            return -1;
        }
        CurrentPos.minute = param[0];
        CurrentPos.second = param[1];
        CurrentPos.sector = param[2];
    }
    if (param != NULL && com == CdlSetloc) {
        CurrentPos.minute = param[0];
        CurrentPos.second = param[1];
        CurrentPos.sector = param[2];
    }
    CdSyncCallback(old);
    return 0;
}

int CdControl(u8 com, u8* param, u_char* result) {
    if (cd_setloc(com, param, result)) {
        return 0;
    }
    return CD_cw(com, param, result, 0) == 0;
}

int CdControlF(u8 com, u8* param) {
    if (cd_setloc(com, param, NULL)) {
        return 0;
    }
    return CD_cw(com, param, NULL, 1) == 0;
}

int CdControlB(u8 com, u8* param, u_char* result) {
    if (cd_setloc(com, param, result)) {
        return 0;
    }
    if (CD_cw(com, param, result, 0)) {
        return 0;
    }
    return CD_sync(CdlSync, result) == CdlComplete;
}

int CdGetSector(void* madr, int size) { return CD_read(madr, size) == 0; }

int CdRead(int sectors, u_long* buf, int mode) {
    u_char param[4];
    int size;

    switch (mode & 0x30) {
    case 0:
        size = 0x200;
        break;
    case 0x20:
        size = 0x249;
        break;
    default:
        size = 0x246;
        break;
    }
    if (CD_cw(CdlSetloc, (u8*)&CurrentPos, NULL, 0) == 0) {
        param[0] = mode;
        if (CD_cw(CdlSetmode, param, NULL, 0) == 0) {
            CdIntToPos(CdPosToInt(&CurrentPos) + sectors, &CurrentPos);
            return CD_readm(buf, sectors, size, 0) == 0;
        }
    }
}

int CdGetStatus(u_char* status) {
    u_char result[8];
    int ret;

    ret = CdControlB(CdlNop, NULL, result);
    *status = result[0];
    return ret;
}

int CdSetFilter(u_char file, u_char chan) {
    u_char param[4];

    param[0] = file;
    param[1] = chan;
    return CdControlB(0x0D, param, NULL);
}

int CdMix(CdlATV* vol) {
    CD_vol(vol);
    return 1;
}

int CdGetPos(CdlLOC* p) {
    u_char result[8];
    int ret;

    ret = CdControl(0x11, NULL, result);
    p->minute = result[5];
    p->second = result[6];
    p->sector = result[7];
    p->track = result[0];
    return ret;
}

int CdGetHeader(u_char* p) {
    u_char result[8];
    int ret;

    ret = CdControl(0x10, NULL, result);
    p[0] = result[0];
    p[1] = result[1];
    p[2] = result[2];
    return ret;
}

int CdSetMode(u_char mode) {
    u_char param[4];

    param[0] = mode;
    return CdControl(CdlSetmode, param, NULL);
}
#else
static int cd_cw(u8 com, u8* param, u_char* result, int arg3) {
    CdlCB old = CD_cbsync;
    int count = 4;

    while (count--) {
        CD_cbsync = NULL;
        if (CD_isopen) {
            CD_cw(CdlNop, NULL, NULL, 0);
            continue;
        }
        if (param != NULL && setloc[com] != 0 &&
            CD_cw(CdlSetloc, param, result, 0)) {
            continue;
        }
        CD_cbsync = old;
        if (!CD_cw(com, param, result, arg3)) {
            return 0;
        }
    }
    CD_cbsync = old;
    return -1;
}

int CdControl(u8 com, u8* param, u_char* result) {
    return cd_cw(com, param, result, 0) == 0;
}

int CdControlF(u8 com, u8* param) { return cd_cw(com, param, NULL, 1) == 0; }

int CdControlB(u8 com, u8* param, u_char* result) {
    if (cd_cw(com, param, result, 0)) {
        return 0;
    }
    return CD_sync(CdlSync, result) == CdlComplete;
}

int CdGetSector(void* madr, int size) { return CD_read(madr, size) == 0; }

int CdRead(int sectors, u_long* buf, int mode) {
    u_char result[8];
    u_char param[4];
    int size;
    int count = 4;

    switch (mode & 0x30) {
    case 0:
        size = 0x200;
        break;
    case 0x20:
        size = 0x249;
        break;
    default:
        size = 0x246;
        break;
    }
    param[0] = mode;
    while (count--) {
        if (CD_isopen) {
            CD_cw(CdlNop, NULL, NULL, 0);
            continue;
        }
        if (CD_cw(CdlSetmode, param, NULL, 0)) {
            continue;
        }
        if (!CD_readm(buf, sectors, size)) {
            return 1;
        }
    }
    return 0;
}

int CdMix(CdlATV* vol) {
    CD_vol(vol);
    return 1;
}

#endif
int CdRead2(long mode) {
    u_char param[4];

    param[0] = mode;
    CdControl(CdlSetmode, param, NULL);
    if (mode & CdlModeStream) {
        CdDataCallback(data_ready_callback);
        CdReadyCallback(StCdInterrupt);
        return CdControl(CdlReadS, NULL, NULL);
    }
    return CdControl(CdlReadN, NULL, NULL);
}

CdlLOC* CdIntToPos(int i, CdlLOC* p) {
    i += 150;
    p->sector = itob(i % 75);
    p->second = itob(i / 75 % 60);
    p->minute = itob(i / 75 / 60);
    return p;
}

int CdPosToInt(CdlLOC* p) {
    return (btoi(p->minute) * 60 + btoi(p->second)) * 75 + btoi(p->sector) -
           150;
}
#else
#include "libcd.h"

extern CdlCB CD_cbready;
extern CdlCB CD_cbsync;
#if VERSION >= 37
extern u_char CD_status;
#else
extern int CD_status;
#endif
extern unsigned char CD_mode;
extern unsigned char CD_com;
extern unsigned char CD_pos[];
extern int CD_debug;
extern char* CD_comstr[];
extern char* CD_intstr[];

int CD_initintr(void);
int CD_init(void);
int CD_initvol(void);
void CD_flush(void);
int CD_sync(int, u_char*);
int CD_ready(int, u_char*);
int CD_cw(u8, u8*, u8*, s32);
void CD_vol(CdlATV*);
int CD_getsector(void*, int);
void CD_datasync(int);
long DMACallback(int, void (*)());

#if VERSION >= 41
#if !defined(PIECE) || PIECE == 3
inline int CdStatus(void) { return CD_status; }
#else
extern inline int CdStatus(void) { return CD_status; }
#endif
#else
inline char CdStatus(void) { return CD_status; }
#endif

#if VERSION != 33
#if !defined(PIECE) || PIECE == 4
int CdMode(void) { return CD_mode; }
#endif
#endif

#if !defined(PIECE) || PIECE == 5
int CdLastCom(void) { return CD_com; }
#endif

#if VERSION != 33
#if !defined(PIECE) || PIECE == 6
unsigned char* CdLastPos(void) { return CD_pos; }
#endif
#endif

#if !defined(PIECE) || PIECE == 7
int CdReset(int mode) {
    if (mode == CdlModeAP) {
        CD_initintr();
        return 1;
    }
    if (CD_init()) {
        return 0;
    }
    if (mode == CdlModeDA && CD_initvol()) {
        return 0;
    }
    return 1;
}
#endif

#if !defined(PIECE) || PIECE == 8
void CdFlush(void) { CD_flush(); }
#endif

#if !defined(PIECE) || PIECE == 9
s32 CdSetDebug(s32 arg0) {
    s32 temp_v0 = CD_debug;
    CD_debug = arg0;
    return temp_v0;
}
#endif

#if !defined(PIECE) || PIECE == 10
char* CdComstr(unsigned char com) {
    if (com > 0x1b) {
        return "none";
    }
    return CD_comstr[com];
}
#endif

#if !defined(PIECE) || PIECE == 11
char* CdIntstr(u8 intr) {
    if (intr > 6) {
        return "none";
    }
    return CD_intstr[intr];
}
#endif

#if !defined(PIECE) || PIECE == 12
int CdSync(int mode, u_char* result) { return CD_sync(mode, result); }
#endif

#if !defined(PIECE) || PIECE == 13
int CdReady(int mode, u_char* result) { return CD_ready(mode, result); }
#endif

#if !defined(PIECE) || PIECE == 14
CdlCB CdSyncCallback(CdlCB func) {
    CdlCB old = CD_cbsync;
    CD_cbsync = func;
    return old;
}
#endif

#if !defined(PIECE) || PIECE == 15
CdlCB CdReadyCallback(CdlCB func) {
    CdlCB old = CD_cbready;
    CD_cbready = func;
    return old;
}
#endif

#if VERSION == 33
extern CdlCB CD_cbread;

CdlCB CdReadCallback(CdlCB func) {
    CdlCB old = CD_cbread;
    CD_cbread = func;
    return old;
}
#endif

#if !defined(PIECE) || PIECE == 16
static int setloc[] = {
    0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
};

#if VERSION == 33
int CD_shell(void);

static inline cd_cw(u8 com, u8* param, u_char* result, s32 arg3) {
    CdlCB old = CD_cbsync;
    int count = 4;

    while (count--) {
        if (com == CdlGetlocL && !(CdStatus() & CdlStatRead)) {
            CD_shell();
            continue;
        }
        if (com != CdlNop) {
            CD_cbsync = NULL;
            CD_shell();
            if (param != NULL && setloc[com] != 0 &&
                CD_cw(CdlSetloc, param, result, 0)) {
                continue;
            }
            CD_cbsync = old;
        }
        if (!CD_cw(com, param, result, arg3)) {
            return 0;
        }
    }
    CD_cbsync = old;
    return -1;
}
#else
static inline cd_cw(u8 com, u8* param, u_char* result, s32 arg3) {
    CdlCB old = CD_cbsync;
    int count = 4;

    while (count--) {
        CD_cbsync = NULL;
        if (com != CdlNop && (CdStatus() & CdlStatShellOpen)) {
            CD_cw(CdlNop, NULL, NULL, 0);
        }
        if (param == NULL || setloc[com] == 0 ||
            !CD_cw(CdlSetloc, param, result, 0)) {
            CD_cbsync = old;
            if (!CD_cw(com, param, result, arg3)) {
                return 0;
            }
        }
    }
    CD_cbsync = old;
    return -1;
}
#endif

int CdControl(u8 com, u8* param, u_char* result) {
    return cd_cw(com, param, result, 0) == 0;
}

int CdControlF(u8 com, u8* param) { return cd_cw(com, param, NULL, 1) == 0; }

int CdControlB(u8 com, u8* param, u_char* result) {
    if (cd_cw(com, param, result, 0)) {
        return 0;
    }
    return CD_sync(CdlSync, result) == CdlComplete;
}
#endif

#if !defined(PIECE) || PIECE == 20
int CdMix(CdlATV* vol) {
    CD_vol(vol);
    return 1;
}
#endif

#if !defined(PIECE) || PIECE == 21
int CdGetSector(void* madr, int size) { return CD_getsector(madr, size) == 0; }
#endif

#if VERSION >= 40
int CD_getsector2(void*, int);

#if !defined(PIECE) || PIECE == 22
int CdGetSector2(void* madr, int size) {
    return CD_getsector2(madr, size) == 0;
}
#endif
#endif

#if !defined(PIECE) || PIECE == 23
long CdDataCallback(void (*func)()) { return DMACallback(3, func); }
#endif

#if !defined(PIECE) || PIECE == 24
void CdDataSync(int mode) { CD_datasync(mode); }
#endif

#if VERSION == 33
int CD_readsync(int, u_char*);
int CD_readm(u_long*, int, int);

int CdReadSync(int mode, u_char* result) { return CD_readsync(mode, result); }

int CdRead(int sectors, u_long* buf, int mode) {
    int count = 4;

    while (count--) {
        if (!CD_readm(buf, sectors, mode)) {
            return 1;
        }
    }
    return 0;
}
#endif

#if !defined(PIECE) || PIECE == 2
CdlLOC* CdIntToPos(int i, CdlLOC* p) {
    inline int ENCODE_BCD(int n) { return ((n / 10) << 4) + (n % 10); }

    i += 150;
    p->sector = ENCODE_BCD(i % 75);
    p->second = ENCODE_BCD(i / 75 % 60);
    p->minute = ENCODE_BCD(i / 75 / 60);
    return p;
}
#endif

#if !defined(PIECE) || PIECE == 1
int CdPosToInt(CdlLOC* p) {
#define DECODE_BCD(x) (((x) >> 4) * 10 + ((x) & 0xF))
#if VERSION >= 41
    return (DECODE_BCD(p->minute) * 60 + DECODE_BCD(p->second)) * 75 +
           DECODE_BCD(p->sector) - 150;
#else
    u8 sector = p->sector;
    u8 second = p->second;
    u8 minute = p->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
#endif
}
#endif
#endif
