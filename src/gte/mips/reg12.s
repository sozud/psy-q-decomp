	.set	noreorder
	.set	noat
	.globl	SetGeomOffset
	.text
SetGeomOffset:
	sll	$4, $4, 16
	sll	$5, $5, 16
	ctc2	$4, $24
	ctc2	$5, $25
	j	$31
