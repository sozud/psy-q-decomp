	.set	noreorder
	.set	noat
	.globl	_card_write
	.text
_card_write:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 78
