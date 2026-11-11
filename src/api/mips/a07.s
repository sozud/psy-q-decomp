	.set	noreorder
	.set	noat
	.globl	DeliverEvent
	.text
DeliverEvent:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 7
