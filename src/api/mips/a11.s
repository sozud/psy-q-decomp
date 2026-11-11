	.set	noreorder
	.set	noat
	.globl	TestEvent
	.text
TestEvent:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 11
