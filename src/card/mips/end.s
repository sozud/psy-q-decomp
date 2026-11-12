	.set	noreorder
	.set	noat
	.globl	_ExitCard
	.lcomm	Lb0, 16
	.extern	EnterCriticalSection 0
	.extern __unused4 0; .extern __unused5 0; .extern __unused6 0
	.extern	FlushCache 0
	.extern	ExitCriticalSection 0
	.text
_ExitCard:
	.set	at
	sw	$31, Lb0
	.set	noat
	jal	EnterCriticalSection
	nop
	addiu	$10, $0, 176
	jal	$10
	addiu	$9, $0, 86
	.set	reorder
	lw	$2, 24($2)
	la	$10, Lt68
	la	$9, Lt74
	.set	noreorder
Lt30:
	lw	$3, 0($10)
	addiu	$10, $10, 4
	addiu	$2, $2, 4
	bne	$10, $9, Lt30
	sw	$3, 108($2)
	jal	FlushCache
	nop
	jal	ExitCriticalSection
	nop
	lw	$31, Lb0
	nop
	j	$31
	nop
Lt68:
	nop
	nop
	nop
Lt74:
