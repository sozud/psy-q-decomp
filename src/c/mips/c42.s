	.set	noreorder
	.set	noat
	.globl	memcpy
	.text
memcpy:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 42
