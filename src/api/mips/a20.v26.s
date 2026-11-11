	.set	noreorder
	.set	noat
	.globl	StopPAD
	.text
StopPAD:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 20
