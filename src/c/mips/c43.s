	.set	noreorder
	.set	noat
	.globl	memset
	.text
memset:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 43
