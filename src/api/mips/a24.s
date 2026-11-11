	.set	noreorder
	.set	noat
	.globl	ResetEntryInt
	.text
ResetEntryInt:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 24
