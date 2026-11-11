	.set	noreorder
	.set	noat
	.globl	printf
	.text
printf:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 63
