	.set	noreorder
	.set	noat
	.globl	HookEntryInt
	.text
HookEntryInt:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 25
