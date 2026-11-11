	.set	noreorder
	.set	noat
	.globl	ExitCriticalSection
	.text
ExitCriticalSection:
	addiu	$4, $0, 2
	syscall
	j	$31
