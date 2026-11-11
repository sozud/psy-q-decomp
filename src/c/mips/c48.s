	.set	noreorder
	.set	noat
	.globl	srand
	.text
srand:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 48
