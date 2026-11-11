#ifndef KERNEL_H
#define KERNEL_H

#define DescMask 0xff000000
#define DescTH DescMask
#define DescHW 0xf0000000
#define DescEV 0xf1000000
#define DescRC 0xf2000000
#define DescUEV 0xf3000000
#define DescSW 0xf4000000

#define HwVBLANK (DescHW | 0x01)
#define HwGPU (DescHW | 0x02)
#define HwCdRom (DescHW | 0x03)
#define HwDMAC (DescHW | 0x04)
#define HwRTC0 (DescHW | 0x05)
#define HwRTC1 (DescHW | 0x06)
#define HwRTC2 (DescHW | 0x07)
#define HwCNTL (DescHW | 0x08)
#define HwSPU (DescHW | 0x09)
#define HwPIO (DescHW | 0x0a)
#define HwSIO (DescHW | 0x0b)

#define HwCPU (DescHW | 0x10)
#define HwCARD (DescHW | 0x11)
#define HwCARD_0 (DescHW | 0x12)
#define HwCARD_1 (DescHW | 0x13)
#define SwCARD (DescSW | 0x01)
#define SwMATH (DescSW | 0x02)

#define RCntCNT0 (DescRC | 0x00)
#define RCntCNT1 (DescRC | 0x01)
#define RCntCNT2 (DescRC | 0x02)
#define RCntCNT3 (DescRC | 0x03)

#define RCntMdINTR 0x1000
#define RCntMdNOINTR 0x2000
#define RCntMdSC 0x0001
#define RCntMdSP 0x0000
#define RCntMdFR 0x0000
#define RCntMdGATE 0x0010

#define EvSpCZ 0x0001
#define EvSpINT 0x0002
#define EvSpIOE 0x0004
#define EvSpCLOSE 0x0008
#define EvSpACK 0x0010
#define EvSpCOMP 0x0020
#define EvSpDR 0x0040
#define EvSpDE 0x0080
#define EvSpTIMOUT 0x0100
#define EvSpUNKNOWN 0x0200
#define EvSpIOER 0x0400
#define EvSpIOEW 0x0800
#define EvSpTRAP 0x1000
#define EvSpNEW 0x2000
#define EvSpSYSCALL 0x4000
#define EvSpERROR 0x8000
#define EvSpPERROR 0x8001
#define EvSpEDOM 0x0301
#define EvSpERANGE 0x0302

#define EvMdINTR 0x1000
#define EvMdNOINTR 0x2000

#define EvStUNUSED 0x0000
#define EvStWAIT 0x1000
#define EvStACTIVE 0x2000
#define EvStALREADY 0x4000

#define TcbMdRT 0x1000
#define TcbMdPRI 0x2000

#define TcbStUNUSED 0x1000
#define TcbStACTIVE 0x4000

struct DIRENTRY {
     char name[20];
     long attr;
     long size;
     struct DIRENTRY* next;
     long head;
     char system[4];
};

void EnterCriticalSection(void);
void ExitCriticalSection(void);

long _card_info(long chan);
long _card_clear(long chan);
long _card_load(long chan);
void InitCARD(long val);
long StartCARD(void);

#endif
