	.set	noreorder
	.set	noat
	.globl	InitGeom
	.extern __unused2 0
	.extern	_patch_gte 0
	.text
	.word	0x0c097350
	.word	0x0037aeda
InitGeom:
	.set	at
	sw	$31, Ld0
	.set	noat
	jal	_patch_gte
	nop
	lw	$31, Ld0
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
Ld0:
	.byte	0x0
