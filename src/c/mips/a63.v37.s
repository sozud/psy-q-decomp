	.set	noreorder
	.set	noat
	.globl	puts
	.text
	.word	0x0c017350
	.word	0x003796da
puts:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 63
