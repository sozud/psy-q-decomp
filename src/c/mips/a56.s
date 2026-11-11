	.set	noreorder
	.set	noat
	.globl	exit
	.text
exit:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 56
