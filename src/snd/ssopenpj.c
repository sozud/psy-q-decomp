#include "libsnd_i.h"

short SsSepOpenJ(unsigned long* addr, short vab_id, short seq_cnt) {
#if VERSION >= 41
    s32 bit;
    short i;
    short flag;
    int len;
    char var_a3;
#else
    short bit;
    short i;
    short flag;
    int len;
    unsigned char var_a3;
#endif
    unsigned char *p = (unsigned char *)addr;

    flag = 0;
    if (_snd_openflag == -1) {
        printf("Can't Open Sequence data any more\n\n");
        return -1;
    }
    bit = 0;
    var_a3 = 0;
    do {
        if ((_snd_openflag & (1 << bit)) == 0U) {
            flag = bit;
            var_a3 = 1;
        }
        bit++;
#if VERSION >= 41
        if (bit >= 32) {
            printf("Can't Open Sequence data any more\n\n");
            return -1;
        }
#endif
    } while (var_a3 == 0);
    _snd_openflag |= 1 << flag;
    for (i = 0; i < seq_cnt; i++) {
        len = _SsInitSoundSep(flag, i, vab_id, (unsigned long *)p);
#if VERSION >= 41
        if (len == -1) {
#else
        var_a3 = 1;
        if (len == -var_a3) {
#endif
            return -1;
        }
        p += len;
    }
    return flag;
}
