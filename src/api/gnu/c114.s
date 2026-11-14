/* LIBAPI.LIB C114: 4.1 4.3 4.4 4.6 */
	.set	noreorder
	.set	noat
	.globl	_96_remove
	.text
#if   VERSION >= 43
	.word	0x25007350
	.word	0x0043529b
#else
	.word	0x19007350
	.word	0x00415bb3
#endif
_96_remove:
	addiu	$10, $0, 160
	j	$10
	addiu	$9, $0, 114
