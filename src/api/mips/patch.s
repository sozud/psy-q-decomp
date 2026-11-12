	.globl	_patch_pad
	.globl	EnablePAD
#if VERSION == 37
#define DisablePAD DesablePAD
#endif
	.globl	DisablePAD
	.lcomm	save_ra, 4
	.lcomm	unused, 4
	.lcomm	enable_fn, 4
	.lcomm	disable_fn, 4
	.text
	.ent	EnablePAD
EnablePAD:
	lw	$9, enable_fn
	j	$9
	.end	EnablePAD
	.ent	DisablePAD
DisablePAD:
	lw	$9, disable_fn
	j	$9
	.end	DisablePAD
	.ent	_patch_pad
_patch_pad:
	sw	$31, save_ra
	jal	EnterCriticalSection
	li	$10, 0xb0
	li	$9, 0x57
	jal	$10
	lw	$2, 0x16c($2)
	addi	$3, $2, 0x884
	sw	$3, enable_fn
	addi	$3, $2, 0x894
	li	$9, 11
	sw	$3, disable_fn
loop:
	addiu	$9, $9, -1
	sw	$0, 0x594($2)
	addiu	$2, $2, 4
	bne	$9, $0, loop
	jal	FlushCache
	lw	$31, save_ra
	j	$31
	.end	_patch_pad
