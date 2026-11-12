	.set	noreorder
	.set	noat
	.extern __unused1 0
	.globl	_patch_gte
	.extern	EnterCriticalSection 0
	.lcomm	Lsave, 4
	.text
_patch_gte:
	sw	$31, Lsave
	jal	EnterCriticalSection
	nop
	li	$10, 0xb0
	jal	$10
	li	$9, 0x56
	.set	reorder
	la	$10, Lpatch
	la	$9, Lpatch+56
	lw	$2, 0x18($2)
	.set	noreorder
Lcopy:
	lw	$3, 0($10)
	addiu	$10, $10, 4
	addiu	$2, $2, 4
	bne	$10, $9, Lcopy
	sw	$3, -4($2)
	jal	FlushCache
	nop
	jal	ExitCriticalSection
	nop
	lw	$31, Lsave
	nop
	j	$31
	nop
Lpatch:
	nop
	nop
	li	$26, 0x100
	lw	$26, 8($26)
	nop
	lw	$26, 0($26)
	nop
	addi	$26, $26, 8
	sw	$1, 4($26)
	sw	$2, 8($26)
	sw	$3, 12($26)
	sw	$31, 124($26)
	mfc0	$2, $13
	nop
