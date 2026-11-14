/* LIBAPI.LIB CHCLRPAD: 4.1 */
	.set	noreorder
	.set	noat
	.globl	_remove_ChgclrPAD
	.lcomm	store, 16
	.text
_remove_ChgclrPAD:
	.set	at
	sw	$31, store
	.set	noat
	jal	EnterCriticalSection
	nop
	addiu	$9, $0, 87
	addiu	$10, $0, 176
	jalr	$10
	nop
	addiu	$10, $0, 9
	lw	$2, 364($2)
	nop
	addi	$3, $2, 1580
1:
	sw	$0, 0($3)
	addiu	$3, $3, 4
	addiu	$10, $10, -1
	bne	$10, $0, 1b
	nop
	jal	FlushCache
	nop
	jal	ExitCriticalSection
	nop
	lw	$31, store
	nop
	jr	$31
	nop
