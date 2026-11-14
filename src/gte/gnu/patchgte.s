/* LIBGTE.LIB PATCHGTE: 4.1 */
	.file	1 "patchgte.asm"
	.lcomm	store, 4
	.globl	_patch_gte
	.text
	.ent	_patch_gte
_patch_gte:
	sw	$31, store
	jal	EnterCriticalSection
	li	$9, 0x56
	li	$10, 0xb0
	jal	$10
	lw	$2, 0x18($2)
	addiu	$2, $2, 40
	move	$15, $2
	la	$10, oldbug
	la	$9, oldbug_end
1:
	lw	$3, 0($10)
	lw	$11, 0($2)
	addiu	$10, $10, 4
	addiu	$2, $2, 4
	bne	$3, $11, 3f
	bne	$10, $9, 1b
	move	$2, $15
	la	$10, newnonbug
	la	$9, newnonbug_end
2:
	lw	$3, 0($10)
	sw	$3, 0($2)
	addiu	$10, $10, 4
	addiu	$2, $2, 4
	bne	$10, $9, 2b
3:
	jal	FlushCache
	jal	ExitCriticalSection
	lw	$31, store
	j	$31
	.end	_patch_gte

	.set	noreorder
	.set	noat
	.ent	oldbug
oldbug:
	sw	$1, 4($26)
	sw	$2, 8($26)
	sw	$3, 12($26)
	sw	$31, 124($26)
	mfc0	$3, $14
	nop
oldbug_end:
	.end	oldbug

	.ent	newnonbug
newnonbug:
	sw	$1, 4($26)
	sw	$2, 8($26)
	mfc0	$2, $13
	sw	$3, 12($26)
	mfc0	$3, $14
	sw	$31, 124($26)
newnonbug_end:
	.end	newnonbug
