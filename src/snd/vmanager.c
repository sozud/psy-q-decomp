// SPDX-License-Identifier: MIT
#include "common.h"

#define SEQ_UNK76_UNSIGNED

#define SsUtVibrateOn header_SsUtVibrateOn
#define SsUtVibrateOff header_SsUtVibrateOff
#define SpuVmSeKeyOn header_SpuVmSeKeyOn
#define SpuVmSeKeyOff header_SpuVmSeKeyOff
#define SpuVmSetSeqVol header_SpuVmSetSeqVol
#define SpuVmSeqKeyOff header_SpuVmSeqKeyOff
#define SsUtKeyOffV header_SsUtKeyOffV
#define SsUtSetDetVVol header_SsUtSetDetVVol
#define SsUtAllKeyOff header_SsUtAllKeyOff
#define _svm_sreg_buf header_svm_sreg_buf
#define _svm_sreg_dirty header_svm_sreg_dirty
#define _svm_voice header_svm_voice
#define D_80032F10 header_D_80032F10
#define SpuVmAlloc header_SpuVmAlloc
#define vmNoiseOn2 header_vmNoiseOn2
#define SeAutoVol header_SeAutoVol
#define SeAutoPan header_SeAutoPan
#include "libsnd_i.h"
#undef SsUtVibrateOn
#undef SsUtVibrateOff
#undef SpuVmSeKeyOn
#undef SpuVmSeKeyOff
#undef SpuVmSetSeqVol
#undef SpuVmSeqKeyOff
#undef SsUtKeyOffV
#undef SsUtSetDetVVol
#undef SsUtAllKeyOff
#undef _svm_sreg_buf
#undef _svm_sreg_dirty
#undef _svm_voice
#undef D_80032F10
#undef SpuVmAlloc
#undef vmNoiseOn2
#undef SeAutoVol
#undef SeAutoPan

#define NUM_SPU_CHANNELS 24

u16 _svm_okon1;
u16 _svm_okon2;
u16 _svm_okof1;
u16 _svm_okof2;

extern s16 _svm_vcf;
s16 _svm_orev1;
s16 _svm_orev2;
extern u8 _svm_auto_kof_mode;

static inline u16 get_field_0x1a() { return _svm_cur.field_0x1a; }

#if VERSION < 33

#include "vm26/vmstatic.c"
#include "vm26/vm_init.c"
#include "vm26/vm_noise.c"
#include "vm26/vm_pb.c"
#include "vm26/vm_f.c"
#include "vm26/vm_key.c"
#include "vm26/vm_seq.c"
#include "vm26/vm_prog.c"
#include "vm26/ut_key.c"
#include "vm26/ut_keyv.c"
#include "vm26/ut_pb.c"
#include "vm26/ut_cp.c"
#include "vm26/ut_cadsr.c"
#include "vm26/ut_vvol.c"
#include "vm26/ut_auto.c"
#include "vm26/ut_ako.c"
#else
#include "vm/vmstatic.c"
#include "vm/vm_init.c"
#include "vm/vm_noise.c"
#include "vm/vm_pb.c"
#include "vm/vm_f.c"
#include "vm/vm_key.c"
#include "vm/vm_seq.c"
#include "vm/vm_prog.c"
#include "vm/vm_vol.c"
#include "vm/ut_key.c"
#include "vm/ut_keyv.c"
#include "vm/ut_pb.c"
#include "vm/ut_cp.c"
#include "vm/ut_cadsr.c"
#include "vm/ut_vvol.c"
#include "vm/ut_auto.c"
#include "vm/ut_ako.c"
#endif
