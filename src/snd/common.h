#ifndef COMMON_H
#define COMMON_H

#include "../types.h"

#define STATIC_PAD_CAT_(a, b) a##b
#define STATIC_PAD_CAT(a, b) STATIC_PAD_CAT_(a, b)
#define STATIC_PAD_BSS(size) static u8 STATIC_PAD_CAT(static_pad_bss_, __LINE__)[size]

#endif
