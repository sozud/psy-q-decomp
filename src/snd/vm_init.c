#include "libsnd_i.h"


extern unsigned short _svm_okon1;
extern unsigned short _svm_okon2;
extern unsigned short _svm_okof1;
extern unsigned short _svm_okof2;
extern unsigned short _svm_orev1;
extern unsigned short _svm_orev2;

#if VERSION >= 37
static long _ss_spu_vm_rec[0x44];
#else
static long _ss_spu_vm_rec[0x24];
#endif

void _SsVmInit(char numVoices) {
    unsigned short i;
    unsigned short temp;
    unsigned int n;

    _spu_setInTransfer(0);
    _svm_damper = 0;
    SpuInitMalloc(32, (char*)&_ss_spu_vm_rec[2]);
    for (i = 0; i < NUM_VOICES * 8; i++) {
        ((short*)_svm_sreg_buf)[i] = 0;
    }
    for (i = 0; i < NUM_VOICES; i++) {
        _svm_sreg_dirty[i] = 0;
    }
    _svm_vab_count = 0;
    for (i = 0; i < NUM_VAB; i++) {
        _svm_vab_used[i] = 0;
    }

    n = numVoices;
    if (n >= NUM_VOICES) {
        _SsVmMaxVoice = NUM_VOICES;
    } else {
        _SsVmMaxVoice = n;
    }

    for (i = 0; i < _SsVmMaxVoice; i++) {
        temp = i * 8;
        _svm_voice[i].unk2 = 0x18;
        _svm_voice[i].seq_sep_no = -1;
        _svm_voice[i].vag_idx = 0xFF;
        _svm_voice[i].unk1b = 0;
        _svm_voice[i].unk04 = 0;
        _svm_voice[i].key_stat = 0;
        _svm_voice[i].fake_program = 0;
        _svm_voice[i].prog = 0;
        _svm_voice[i].tone = 0xFF;
        _svm_voice[i].voll1 = 0;
        _svm_voice[i].pan = 0x40;
        _svm_voice[i].auto_vol = 0;
        _svm_voice[i].unk1e = 0;
        _svm_voice[i].unk20 = 0;
        _svm_voice[i].unk22 = 0;
        _svm_voice[i].auto_pan = 0;
        _svm_voice[i].unk2a = 0;
        _svm_voice[i].unk2c = 0;
        _svm_voice[i].unk2e = 0;
        _svm_voice[i].start_pan = 0;
        _svm_voice[i].start_vol = 0;
        ((u16*)_svm_sreg)[temp + 3] = 0x200;
        ((u16*)_svm_sreg)[temp + 2] = 0x1000;
        ((u16*)_svm_sreg)[temp + 4] = 0x80FF;
        ((u16*)_svm_sreg)[temp + 0] = 0;
        ((u16*)_svm_sreg)[temp + 1] = 0;
        ((u16*)_svm_sreg)[temp + 5] = 0x4000;
        _svm_cur.voice = i;
        _SsVmKeyOffNow(1);
    }
    _svm_rattr.depth.left = 0x3FFF;
    _svm_rattr.depth.right = 0x3FFF;
    _svm_okon1 = 0;
    _svm_okon2 = 0;
    _svm_okof1 = 0;
    _svm_orev1 = 0;
    _svm_orev2 = 0;
    _svm_rattr.mask = 0;
    _svm_rattr.mode = 0;
    _svm_auto_kof_mode = 0;
    _svm_stereo_mono = 0;
    kMaxPrograms = 128;
    _SsVmFlush();
}
