	.set	noreorder
	.set	noat
	.globl	EnterCriticalSection
	.text
EnterCriticalSection:
	addiu	$4, $0, 1
	syscall
	j	$31
