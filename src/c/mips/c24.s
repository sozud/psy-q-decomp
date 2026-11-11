	.set	noreorder
	.set	noat
	.globl	strncmp
	.text
strncmp:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 24
