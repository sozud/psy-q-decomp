/* LIBAPI.LIB PATCH: 4.1 */
	.set	noreorder
	.set	noat
	.globl	_patch_pad
	.globl	EnablePAD
	.globl	DisablePAD
	.lcomm	store, 4
	.lcomm	ret_adr, 4
	.lcomm	_start_pad, 4
	.lcomm	_stop_pad, 4
	.text
EnablePAD:
	lw	$9, _start_pad
	nop
	jr	$9
	nop
DisablePAD:
	lw	$9, _stop_pad
	nop
	jr	$9
	nop
_patch_pad:
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
	addiu	$9, $0, 11
	addi	$3, $2, 2180
	.set	at
	sw	$3, _start_pad
	.set	noat
	addi	$3, $2, 2196
	.set	at
	sw	$3, _stop_pad
	.set	noat
1:
	sw	$0, 1428($2)
	addiu	$2, $2, 4
	addiu	$9, $9, -1
	bne	$9, $0, 1b
	nop
	jal	FlushCache
	nop
	lw	$31, store
	nop
	jr	$31
	nop
