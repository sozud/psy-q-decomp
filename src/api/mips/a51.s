	.set	noreorder
	.set	noat
	.globl	lseek
	.text
lseek:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 51
