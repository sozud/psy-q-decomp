	.set	noreorder
	.set	noat
	.globl	rand
	.text
rand:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 47
