	.set	noreorder
	.set	noat
	.globl	SysDeqIntRP
	.text
SysDeqIntRP:
	addiu	$10, $0, 192
	j	$10
	addiu	$9, $0, 3
