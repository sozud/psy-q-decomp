	.set	noreorder
	.set	noat
	.globl	puts
	.text
puts:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 63
