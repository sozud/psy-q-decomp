	.set	noreorder
	.set	noat
	.globl	SetGeomScreen
	.text
SetGeomScreen:
	ctc2	$4, $26
	j	$31
