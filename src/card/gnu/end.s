/* LIBCARD.LIB END: 4.1 */
	.set	noreorder
	.set	noat
	.globl	_ExitCard
	.lcomm	store, 16
	.text
_ExitCard:
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
	la	$10, _clr_vector
	la	$9, _c_vend
1:
	lw	$3, 0($10)
	nop
	sw	$3, 112($2)
	addiu	$10, $10, 4
	bne	$10, $9, 1b
	addiu	$2, $2, 4
	jal	FlushCache
	nop
	jal	ExitCriticalSection
	nop
	lw	$31, store
	nop
	jr	$31
	nop
_clr_vector:
	nop
	nop
	nop
_c_vend:
