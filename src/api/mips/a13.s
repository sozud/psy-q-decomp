	.set	noreorder
	.set	noat
	.globl	DisableEvent
	.text
DisableEvent:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 13
