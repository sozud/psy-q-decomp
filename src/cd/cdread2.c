#include "../types.h"

#define CdlSetmode 0x0E
#define CdlReadS 0x1B
#define CdlModeSize1 0x20
#define CdlModeStream 0x100

#if VERSION == 40
extern s32 StMode;
#else
s32 StMode;
#endif
void data_ready_callback(void);
void StCdInterrupt(void);
int CdControl(u8, u8*, u8*);
long CdDataCallback(void (*)());
void* CdReadyCallback(void (*)());

int printf(const char*, ...);

#if VERSION != 33
static inline void StCdInterrupt2(u8 intr, u8* result) {
    StCdInterrupt();
}
#endif

int CdRead2(long mode) {
    u8 param = mode;

    CdControl(CdlSetmode, &param, 0);
    if (mode & CdlModeStream) {
        if (mode & CdlModeSize1) {
            StMode = 0;
        } else {
            StMode = 1;
#if VERSION == 33
            printf("waring:old type streaming mode in CdRead2\n");
#endif
        }
        CdDataCallback(data_ready_callback);
#if VERSION == 33
        CdReadyCallback(StCdInterrupt);
#else
        CdReadyCallback(StCdInterrupt2);
#endif
    }
    return CdControl(CdlReadS, 0, 0);
}
