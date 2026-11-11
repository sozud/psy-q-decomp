	.set	noreorder
	.set	noat
	.globl	FlushCache
	.text
FlushCache:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 68
