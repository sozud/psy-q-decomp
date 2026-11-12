	.set	noreorder
	.set	noat
	.globl	ChangeTh
	.text
ChangeTh:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 16
