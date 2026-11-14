#ifndef LIBCD_LOCAL_H
#define LIBCD_LOCAL_H

#include "../types.h"

typedef struct {
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;

typedef struct {
    u_char val0;
    u_char val1;
    u_char val2;
    u_char val3;
} CdlATV;

typedef struct {
    CdlLOC pos;
    u_long size;
    char name[16];
} CdlFILE;

typedef struct {
    u16 id;
    u16 type;
    u16 secCount;
    u16 nSectors;
    u_long frameCount;
    u_long frameSize;
    u16 width;
    u16 height;
    u_long dummy1;
    u_long dummy2;
    CdlLOC loc;
} StHEADER;

typedef void (*CdlCB)(u_char, u_char*);

#define CdlNop 0x01
#define CdlSetloc 0x02
#define CdlReadN 0x06
#define CdlPause 0x09
#define CdlSetmode 0x0E
#define CdlGetlocL 0x10
#define CdlReadS 0x1B
#define CdlSync 0x00
#define CdlModeStream 0x100
#define CdlModeSize1 0x20
#define CdlModeSize0 0x10
#define CdlModeRept 0x04
#define CdlModeAP 0x02
#define CdlModeDA 0x01
#define CdlModeSpeed 0x80
#define CdlModeSpeedNormal 0
#define CdlMAXFILE 64
#define CdlMAXDIR 128
#define CdlMAXLEVEL 8
#define CdlStatPlay 0x80
#define CdlStatSeek 0x40
#define CdlStatRead 0x20
#define CdlStatShellOpen 0x10
#define CdlStatIdError 0x08
#define CdlStatSeekError 0x04
#define CdlStatError 0x01
#define CdlNoIntr 0x00
#define CdlDataReady 0x01
#define CdlComplete 0x02
#define CdlAcknowledge 0x03
#define CdlDataEnd 0x04
#define CdlDiskError 0x05
#define CdlReset 0x0A
#define CdlDemute 0x0C

#if VERSION >= 41
int CdStatus(void);
#else
char CdStatus(void);
#endif
int CdMode(void);
void CdFlush(void);
int CdControl(u_char, u_char*, u_char*);
int CdControlB(u_char, u_char*, u_char*);
int CdControlF(u_char, u_char*);
int CdGetSector(void*, int);
CdlLOC* CdIntToPos(int, CdlLOC*);
int CdPosToInt(CdlLOC*);
u_char* CdLastPos(void);
CdlCB CdSyncCallback(CdlCB);
CdlCB CdReadyCallback(CdlCB);
int CdReady(int, u_char*);
int CdRead(int, u_long*, int);
int CdReadSync(int, u_char*);
int VSync(int);
int puts(const char*);

#endif
