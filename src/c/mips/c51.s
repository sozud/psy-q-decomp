	.set	noreorder
	.set	noat
	.globl	malloc
	.text
malloc:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 51
