#ifndef SND_DEFS_H
#define SND_DEFS_H

#include "../types.h"

typedef void (*SndSsMarkCallbackProc)(short seq_no, short sep_no, short data);

struct SeqStruct {
#if VERSION <= 30
    u8 unk3C;
#else
    u8 unk0;
#endif
    u8 pad1[3];
    u8* read_pos;
    u8* next_sep_pos; /* 0x8 */
    u8* loop_pos;     /* 0xC */
    u8 unk10;
    u8 unk11;
    u8 channel;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 panpot[16];
    u8 unk27;
    u8 unk28;
    u8 unk29;
    u8 unk2a;
    u8 unk2b;
    u8 programs[16];
#if VERSION > 30
    u8 unk3C;
    u8 pad3D;
#endif
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    s16 unk46;
    s16 unk48;
    s16 unk4a;
    s16 unk4c;
    s16 vol[16];
    s16 unk6E;
    s16 unk70;
    s16 unk72;
    u16 unk74;
#if VERSION == 35 || VERSION <= 30 || defined(SEQ_UNK76_UNSIGNED)
    u16 unk76;
#else
    s16 unk76;
#endif
    s16 unk78;
    s16 unk7A;
    s32 unk7c;
    u32 unk80;
    s32 unk84;
    s32 delta_value;
    s32 unk8c;
    s32 unk90;
#if VERSION == 35 || VERSION == 33 || VERSION <= 30
    u32 unk94;
    u32 unk98;
#else
    s32 unk94;
    s32 unk98;
#endif
    s32 unk9C;
#if VERSION == 35 || VERSION == 33 || VERSION <= 30
    u32 unkA0;
#else
    s32 unkA0;
#endif
    u32 unkA4;
#if VERSION > 30
    s16 padA6;
    s16 padaa;
#endif
};

#endif
