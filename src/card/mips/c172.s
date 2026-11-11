	.set	noreorder
	.set	noat
	.globl	_card_load
	.text
_card_load:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 172
