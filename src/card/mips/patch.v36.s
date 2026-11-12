	.set	noreorder
	.set	noat
	.lcomm	Lb0, 4
	.globl	_patch_card
	.globl	_patch_card2
	.lcomm	Lb4, 4
	.lcomm	Lb8, 8
	.extern __unused6 0; .extern __unused7 0; .extern __unused8 0; .extern __unused9 0; .extern __unused10 0
	.extern	EnterCriticalSection 0
	.extern __unused12 0
	.extern	FlushCache 0
	.text
Lt0:
	la	$2, Lt14
	j	$2
	nop
	nop
Lt14:
	lw	$2, 4212($3)
	nop
	andi	$2, $2, 0x80
	beq	$2, $0, Lt50
	nop
Lt28:
	lw	$2, 4164($3)
	nop
	andi	$2, $2, 0x80
	bne	$2, $0, Lt28
	nop
	lw	$2, Lb4
	nop
	j	$2
	nop
Lt50:
	j	$31
	nop
_patch_card:
	.set	at
	sw	$31, Lb0
	.set	noat
	jal	EnterCriticalSection
	nop
	addiu	$10, $0, 176
	jal	$10
	addiu	$9, $0, 86
	lw	$2, 24($2)
	nop
	lw	$3, 112($2)
	nop
	andi	$9, $3, 0xffff
	lw	$3, 116($2)
	sll	$9, $9, 16
	andi	$10, $3, 0xffff
	.set	reorder
	addu	$3, $9, $10
	addiu	$2, $3, 40
	la	$10, Lt0
	la	$9, Lt14
	.set	noreorder
Ltac:
	lw	$3, 0($10)
	addiu	$10, $10, 4
	addiu	$2, $2, 4
	bne	$10, $9, Ltac
	sw	$3, -4($2)
	.set	reorder
	.set	at
	sw	$2, Lb4
	.set	noat
	jal	FlushCache
	.set	noreorder
	lw	$31, Lb0
	nop
	j	$31
	nop
Lte0:
	la	$8, Lte0
	jal	$8
	nop
	nop
Ltf4:
	nop
	addiu	$8, $0, 200
Ltfc:
	addiu	$8, $8, -1
	bne	$8, $0, Ltfc
	nop
	j	$31
	nop
_patch_card2:
	.set	at
	sw	$31, Lb0
	.set	noat
	jal	EnterCriticalSection
	nop
	addiu	$10, $0, 176
	jal	$10
	addiu	$9, $0, 87
	.set	reorder
	lw	$2, 364($2)
	lw	$3, 2504($2)
	la	$10, Lte0
	la	$9, Ltf4
	.set	noreorder
Lt144:
	lw	$3, 2504($2)
	lw	$8, 0($10)
	addiu	$10, $10, 4
	sw	$3, -4($10)
	addiu	$2, $2, 4
	bne	$10, $9, Lt144
	sw	$8, 2500($2)
	jal	FlushCache
	nop
	lw	$31, Lb0
	nop
	j	$31
	nop
	lui	$14, 0x32
	addiu	$14, $14, -1
Lt184:
	bne	$14, $0, Lt184
	addiu	$14, $14, -1
	j	$31
	nop
