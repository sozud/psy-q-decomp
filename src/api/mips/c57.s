	.set	noreorder
	.set	noat
	.globl	InitHeap
	.text
InitHeap:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 57
