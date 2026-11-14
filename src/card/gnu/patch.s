/* LIBCARD.LIB PATCH: 4.1 4.3 4.4 4.6 */
	.set	noreorder
	.set	noat
	.globl	_patch_card
	.globl	_patch_card2
	.globl	_copy_memcard_patch
	.lcomm	store, 16
	.text
_patch_wait_start:
	lhu	$15, 10($3)
	lui	$8, 0
	or	$24, $15, $2
	ori	$25, $24, 0x12
	sh	$25, 10($3)
_patch_wait_end:
	addiu	$8, $0, 40
1:
	addiu	$8, $8, -1
	bne	$8, $0, 1b
	nop
	jr	$31
	nop
_check_dsr:
	lw	$2, 4212($3)
	nop
	andi	$2, $2, 0x80
	beq	$2, $0, 3f
	nop
2:
	lw	$2, 4164($3)
	nop
	andi	$2, $2, 0x80
	bne	$2, $0, 2b
	nop
	lw	$2, 0xdffc
	nop
	jr	$2
	nop
3:
	jr	$31
	nop
_check_dsr_end:
_dsr_start:
	lui	$2, 0xa001		/* 0xa000dfac */
	addiu	$2, $2, -8276
	jr	$2
	nop
	nop
_dsr_end:
_wait_start:
	lui	$8, 0xa001		/* 0xa000df80 */
	addiu	$8, $8, -8320
	jalr	$8
	nop
	nop
_wait_end:
_patch_card:
	.set	at
	sw	$31, store
	.set	noat
	jal	EnterCriticalSection
	nop
	addiu	$9, $0, 86
	addiu	$10, $0, 176
	jalr	$10
	nop
	lw	$2, 24($2)
	nop
	lw	$3, 112($2)
	nop
	andi	$9, $3, 0xffff
	sll	$9, $9, 16
	lw	$3, 116($2)
	nop
	andi	$10, $3, 0xffff
	addu	$3, $9, $10
	addiu	$2, $3, 40
	la	$10, _dsr_start
	la	$9, _dsr_end
1:
	lw	$3, 0($10)
	nop
	sw	$3, 0($2)
	addiu	$10, $10, 4
	bne	$10, $9, 1b
	addiu	$2, $2, 4
	lui	$1, 1
	jal	FlushCache
	sw	$2, -8196($1)
	lw	$31, store
	nop
	jr	$31
	nop
_patch_card2:
	.set	at
	sw	$31, store
	.set	noat
	jal	EnterCriticalSection
	nop
	addiu	$9, $0, 87
	addiu	$10, $0, 176
	jalr	$10
	nop
	lw	$2, 364($2)
	nop
	lw	$3, 2504($2)
	la	$10, _wait_start
	la	$9, _wait_end
1:
	lw	$8, 0($10)
	nop
	sw	$8, 2504($2)
	addiu	$10, $10, 4
	bne	$10, $9, 1b
	addiu	$2, $2, 4
	jal	FlushCache
	nop
	lw	$31, store
	nop
	jr	$31
	nop
_copy_memcard_patch:
	li	$2, 0xdf80
	la	$10, _patch_wait_start
	la	$9, _check_dsr_end
1:
	lw	$3, 0($10)
	nop
	sw	$3, 0($2)
	addiu	$10, $10, 4
	bne	$10, $9, 1b
	addiu	$2, $2, 4
	jr	$31
	nop
