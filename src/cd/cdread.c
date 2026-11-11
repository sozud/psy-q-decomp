#include "libcd.h"

#if VERSION == 40
int CdGetSector2(void*, int);
long CdDataCallback(void (*)());
void CdDataSync(int);

typedef struct {
    int num_sectors;
    u_long* buf;
    u_long* cur_buf;
    int mode;
    int size;
    int remaining;
    int start_time2;
    int start_time;
    int pos;
    CdlCB sync_callback;
    CdlCB ready_callback;
    void (*data_callback)();
    int read_mode;
    u_char* result;
} CD_READ_ATTR;

static CdlCB CD_cbread = NULL;
static volatile CD_READ_ATTR ReadAttr = {0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, 0};

static void cb_data(void);
static int cd_read_retry(int mode);

static void cb_read(u_char intr, u_char* result) {
    u_long buf[3];

    ReadAttr.result = result;
    if (intr == CdlDataReady) {
        if (ReadAttr.remaining > 0) {
            if (ReadAttr.size == 0x200) {
                if (ReadAttr.read_mode & 1) {
                    CdDataCallback(NULL);
                    CdGetSector2(buf, 3);
                    CdDataSync(0);
                    CdDataCallback(cb_data);
                } else {
                    CdGetSector(buf, 3);
                }
                if (CdPosToInt((CdlLOC*)buf) != ReadAttr.pos) {
                    puts("CdRead: sector error\n");
                    ReadAttr.remaining = -1;
                }
            }
            if (ReadAttr.read_mode & 1) {
                CdGetSector2((void*)ReadAttr.cur_buf, ReadAttr.size);
            } else {
                CdGetSector((void*)ReadAttr.cur_buf, ReadAttr.size);
                ReadAttr.cur_buf += ReadAttr.size;
                ReadAttr.remaining--;
                ReadAttr.pos++;
            }
        }
    } else {
        ReadAttr.remaining = -1;
    }

    ReadAttr.start_time2 = VSync(-1);
    if (ReadAttr.remaining < 0) {
        cd_read_retry(1);
    }
    if (VSync(-1) > ReadAttr.start_time + 1200) {
        ReadAttr.remaining = -1;
    }
    if (ReadAttr.remaining == 0 || VSync(-1) > ReadAttr.start_time + 1200) {
        CdSyncCallback(ReadAttr.sync_callback);
        CdReadyCallback(ReadAttr.ready_callback);
        if (ReadAttr.read_mode & 1) {
            CdDataCallback(ReadAttr.data_callback);
        }
        CdControlF(CdlPause, NULL);
        if (CD_cbread) {
            CD_cbread(
                ReadAttr.remaining == 0 ? CdlComplete : CdlDiskError, result);
        }
    }
}

static void cb_data(void) {
    ReadAttr.cur_buf += ReadAttr.size;
    ReadAttr.remaining--;
    ReadAttr.pos++;
    if (ReadAttr.remaining == 0) {
        CdSyncCallback(ReadAttr.sync_callback);
        CdReadyCallback(ReadAttr.ready_callback);
        if (ReadAttr.read_mode & 1) {
            CdDataCallback(ReadAttr.data_callback);
        }
        CdControlF(CdlPause, NULL);
        if (CD_cbread != NULL) {
            CD_cbread(CdlComplete, (u_char*)ReadAttr.result);
        }
    }
}

static int cd_read_retry(int mode) {
    u_char param;

    CdSyncCallback(NULL);
    CdReadyCallback(NULL);
    if (ReadAttr.read_mode & 1) {
        CdDataCallback(NULL);
    }
    if (CdStatus() & CdlStatShellOpen) {
        if ((VSync(-1) & 0x3F) == 0) {
            puts("CdRead: Shell open...\n");
        }
        CdControlF(CdlNop, NULL);
        ReadAttr.start_time = VSync(-1);
        ReadAttr.remaining = -1;
        return ReadAttr.remaining;
    }
    if (mode) {
        puts("CdRead: retry...\n");
        CdControl(CdlPause, NULL, NULL);
        if (CdControl(CdlSetloc, CdLastPos(), NULL) == 0) {
            ReadAttr.remaining = -1;
            return ReadAttr.remaining;
        }
    }
    CdFlush();
    param = ReadAttr.mode;
    if (param != CdMode() || mode) {
        if (CdControl(CdlSetmode, &param, NULL) == 0) {
            ReadAttr.remaining = -1;
            return ReadAttr.remaining;
        }
    }
    ReadAttr.pos = CdPosToInt((CdlLOC*)CdLastPos());
    CdReadyCallback(cb_read);
    if (ReadAttr.read_mode & 1) {
        CdDataCallback(cb_data);
    }
    ReadAttr.cur_buf = ReadAttr.buf;
    CdControlF(CdlReadN, NULL);
    ReadAttr.remaining = ReadAttr.num_sectors;
    ReadAttr.start_time2 = VSync(-1);
    return ReadAttr.remaining;
}

void CdReadBreak(void) {
    if (ReadAttr.read_mode & 1) {
        CdDataSync(0);
    }
    ReadAttr.remaining = 0;
    CdSyncCallback(ReadAttr.sync_callback);
    CdReadyCallback(ReadAttr.ready_callback);
    if (ReadAttr.read_mode & 1) {
        CdDataCallback(ReadAttr.data_callback);
    }
    CdControlF(CdlPause, NULL);
}

int CdRead(int sectors, u_long* buf, int mode) {
    ReadAttr.mode = mode;
    switch (ReadAttr.mode & (CdlModeSize0 | CdlModeSize1)) {
    case 0:
        ReadAttr.size = 0x200;
        break;
    case CdlModeSize1:
        ReadAttr.size = 0x249;
        break;
    default:
        ReadAttr.size = 0x246;
        break;
    }
    ReadAttr.mode |= CdlModeSize1;
    ReadAttr.buf = buf;
    ReadAttr.num_sectors = sectors;
    ReadAttr.sync_callback = CdSyncCallback(NULL);
    ReadAttr.ready_callback = CdReadyCallback(NULL);
    if (ReadAttr.read_mode & 1) {
        ReadAttr.data_callback = (void (*)())CdDataCallback(NULL);
    }
    ReadAttr.start_time = VSync(-1);
    if (CdStatus() & (CdlStatPlay | CdlStatSeek | CdlStatRead)) {
        CdControlB(CdlPause, NULL, NULL);
    }
    return cd_read_retry(0) > 0;
}

int CdReadSync(int mode, u_char* result) {
    int ret;

    do {
        if (VSync(-1) > ReadAttr.start_time + 1200) {
            ret = -1;
        } else if (ReadAttr.remaining < 0 ||
                   VSync(-1) > ReadAttr.start_time2 + 60) {
            cd_read_retry(1);
            ret = ReadAttr.num_sectors;
        } else {
            ret = ReadAttr.remaining;
        }
    } while (mode == 0 && ret > 0);

    CdReady(1, result);
    return ret;
}

CdlCB CdReadCallback(CdlCB func) {
    CdlCB prev = CD_cbread;
    CD_cbread = func;
    return prev;
}

int CdReadMode(int mode) {
    int prev = ReadAttr.read_mode;
    ReadAttr.read_mode = mode;
    return prev;
}

#else

typedef struct {
    int sectors;
    u_long* unk4;
    u_long* unk8;
    int mode;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
    int unk20;
    CdlCB sync;
    CdlCB ready;
} cdreadStruct;

static CdlCB CD_ReadCallbackFunc = NULL;
static volatile cdreadStruct cdread = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, NULL,
};

static int cd_read_retry(int retry);

static void cb_read(u_char arg0, u_char* result) {
    int pos[3];

    if (arg0 == 1) {
        if (cdread.unk14 > 0) {
            if (cdread.unk10 == 0x200) {
                CdGetSector(pos, 3);
                if (CdPosToInt((CdlLOC*)pos) != cdread.unk20) {
                    puts("CdRead: sector error\n");
                    cdread.unk14 = -1;
                }
            }
            CdGetSector((void*)cdread.unk8, cdread.unk10);
            cdread.unk8 += cdread.unk10;
            cdread.unk14--;
            cdread.unk20++;
        }
    } else {
        cdread.unk14 = -1;
    }
    cdread.unk18 = VSync(-1);
    if (cdread.unk14 < 0) {
        cd_read_retry(true);
    }
    if (VSync(-1) > cdread.unk1C + 1200) {
        cdread.unk14 = -1;
    }
    if (cdread.unk14 && VSync(-1) <= cdread.unk1C + 1200) {
        return;
    }
    CdSyncCallback(cdread.sync);
    CdReadyCallback(cdread.ready);
    CdControl(CdlPause, NULL, NULL);
    if (CD_ReadCallbackFunc != NULL) {
        CD_ReadCallbackFunc(cdread.unk14 == 0 ? CdlComplete : CdlDiskError,
                            result);
    }
}

static int cd_read_retry(int retry) {
    char mode;
    int mode2;

    CdSyncCallback(NULL);
    CdReadyCallback(NULL);
    if (CdStatus() & CdlStatShellOpen) {
        if ((VSync(-1) % 0x40) == 0) {
            puts("CdRead: Shell open...\n");
        }
        CdControlF(CdlNop, NULL);
        cdread.unk1C = VSync(-1);
        cdread.unk14 = -1;
        return cdread.unk14;
    }
    if (retry) {
        puts("CdRead: retry...\n");
        CdControl(CdlPause, NULL, NULL);
        if (!CdControl(CdlSetloc, CdLastPos(), NULL)) {
            return cdread.unk14 = -1;
        }
    }
    CdFlush();
    mode2 = cdread.mode;
    mode = mode2;
    if ((char)mode2 != CdMode() || retry) {
        if (!CdControl(CdlSetmode, (u8*)&mode, NULL)) {
            cdread.unk14 = -1;
            return cdread.unk14;
        }
    }
    cdread.unk20 = CdPosToInt((CdlLOC*)CdLastPos());
    CdReadyCallback(&cb_read);
    cdread.unk8 = cdread.unk4;
    CdControlF(CdlReadN, NULL);
    cdread.unk14 = cdread.sectors;
    cdread.unk18 = VSync(-1);
    return cdread.unk14;
}

#if VERSION == 37
void CdReadBreak(void) {
    cdread.unk14 = 0;
    CdSyncCallback(cdread.sync);
    CdReadyCallback(cdread.ready);
    CdControl(CdlPause, NULL, NULL);
}
#endif

int CdRead(int sectors, u_long* buf, int mode) {
    cdread.mode = mode;
    switch (cdread.mode & (CdlModeSize0 | CdlModeSize1)) {
    case 0:
        cdread.unk10 = 0x200;
        break;
    case CdlModeSize1:
        cdread.unk10 = 0x249;
        break;
    default:
        cdread.unk10 = 0x246;
        break;
    }
    cdread.mode |= CdlModeSize1;
    cdread.unk4 = buf;
    cdread.sectors = sectors;
    cdread.sync = CdSyncCallback(NULL);
    cdread.ready = CdReadyCallback(NULL);
    cdread.unk1C = VSync(-1);
    if (CdStatus() & (CdlStatPlay | CdlStatSeek | CdlStatRead)) {
        CdControlB(CdlPause, NULL, NULL);
    }
    return cd_read_retry(false) > 0;
}

int CdReadSync(int mode, u_char* result) {
    int var_s0;

    while (true) {
        var_s0 = -1;
        if (VSync(-1) <= cdread.unk1C + 1200) {
            if (cdread.unk14 < 0 || VSync(-1) > cdread.unk18 + 60) {
                cd_read_retry(true);
                var_s0 = cdread.sectors;
            } else {
                var_s0 = cdread.unk14;
            }
        }
        if (mode != 0 || var_s0 <= 0) {
            CdReady(1, result);
            return var_s0;
        }
    }
}

CdlCB CdReadCallback(CdlCB func) {
    CdlCB prevFunc = CD_ReadCallbackFunc;
    CD_ReadCallbackFunc = func;
    return prevFunc;
}
#endif
