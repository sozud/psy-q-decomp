#include "libsnd_i.h"

s32 _svm_brr_start_addr[16]; /* number: 10 */
s16 _svm_stereo_mono;        /* number: 11 */
SpuReverbAttr _svm_rattr;    /* number: 12 */
u8 _svm_vab_used[16];        /* number: 13 */
u8 spuVmMaxVoice;            /* number: 14 */
u16 _svm_vab_count;          /* number: 15 */
s16 kMaxPrograms;            /* number: 16 */
struct struct_svm _svm_cur;  /* number: 17 */
s16 _svm_damper;             /* number: 18 */
s16 _svm_vcf;                /* number: 19 */
u8 _svm_auto_kof_mode;       /* number: 20 */
VabHdr *_svm_vab_vh[16];     /* number: 21 */
ProgAtr *_svm_vab_pg[16];    /* number: 22 */
VagAtr *_svm_vab_tn[16];     /* number: 23 */
s32 _svm_vab_start[16];      /* number: 24 */
s32 _svm_vab_total[16];      /* number: 25 */
VabHdr *_svm_vh;             /* number: 26 */
ProgAtr* _svm_pg;            /* number: 27 */
VagAtr *_svm_tn;             /* number: 28 */
void* _svm_vg;               /* number: 29 */
