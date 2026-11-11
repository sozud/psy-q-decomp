	.set	noreorder
	.set	noat
	.globl	ReturnFromException
	.text
ReturnFromException:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 23
