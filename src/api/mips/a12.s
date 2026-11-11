	.set	noreorder
	.set	noat
	.globl	EnableEvent
	.text
EnableEvent:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 12
