	.set	noreorder
	.set	noat
	.globl	SysEnqIntRP
	.text
SysEnqIntRP:
	addiu	$10, $0, 192
	j	$10
	addiu	$9, $0, 2
