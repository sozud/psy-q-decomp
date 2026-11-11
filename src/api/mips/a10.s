	.set	noreorder
	.set	noat
	.globl	WaitEvent
	.text
WaitEvent:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 10
