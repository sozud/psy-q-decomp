	.set	noreorder
	.set	noat
	.globl	setjmp
	.text
setjmp:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 19
