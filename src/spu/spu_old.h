#ifndef SPU_OLD_H
#define SPU_OLD_H

#ifdef SPU_OLD_LOCAL_RXX
#define _spu_RXX header_spu_RXX
#endif
#include "libspu_i.h"
#ifdef SPU_OLD_LOCAL_RXX

#undef _spu_RXX
#ifndef SPU_OLD_DEFER_RXX
static union SpuUnion *_spu_RXX = (union SpuUnion *)0x1F801C00;
#endif
#endif
#define SPU_OLD_RXX_DEF static union SpuUnion *_spu_RXX = (union SpuUnion *)0x1F801C00

extern long _spu_fd;
extern long _spu_ioctl(long fd, long cmd, ...);

extern long _spu_trans_mode;
extern unsigned long _spu_keystat;
extern unsigned long _spu_keyon_bit;
extern unsigned long _spu_keyoff_bit;
extern SpuReverbAttr _spu_rev_attr;

void SpuNwSetVoiceAttr(SpuVoiceAttr *arg);
void SpuNwSetKey(long on_off, unsigned long voice_bit);
#endif
