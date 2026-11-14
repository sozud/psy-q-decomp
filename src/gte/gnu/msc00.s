/* LIBGTE.LIB MSC00: 4.1 4.3 4.4 4.6 */
	.file	1 "msc00.asm"
	.set	noreorder
	.set	noat
	.globl	InitGeom
	.text
#if VERSION == 41
	.word	0x19097350
	.word	0x004162b3
#elif VERSION == 43
	.word	0x25097350
	.word	0x0043539b
#else
	.word	0x00097350
	.word	0x00460000
#endif
InitGeom:
	.set	at
	sw	$31, SAVERA
	.set	noat
	jal	_patch_gte
	nop
	lw	$31, SAVERA
	nop
	mfc0	$2, $12
	lui	$3, 0x4000
	or	$2, $2, $3
	mtc0	$2, $12
	nop
	addiu	$8, $0, 341
	ctc2	$8, $29
	nop
	addiu	$8, $0, 256
	ctc2	$8, $30
	nop
	addiu	$8, $0, 1000
	ctc2	$8, $26
	nop
	addiu	$8, $0, -4194
	ctc2	$8, $27
	nop
	lui	$8, 0x140
	ctc2	$8, $28
	nop
	ctc2	$0, $24
	ctc2	$0, $25
	nop
	j	$31
	.data
SAVERA:
	.byte	0x0
