/* LIBC.LIB A63: 4.1 4.3 4.4 4.6 */
	.set	noreorder
	.set	noat
	.globl	puts
	.text
#if   VERSION >= 43
	.word	0x25017350
	.word	0x0043529b
#else
	.word	0x19017350
	.word	0x00415cb3
#endif
puts:
	addiu	$10, $0, 176
	j	$10
	addiu	$9, $0, 63
