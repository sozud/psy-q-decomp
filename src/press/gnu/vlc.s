/* LIBPRESS.LIB VLC: 4.1 4.3 4.4 4.6 */
	.set	noreorder
	.set	noat
#if VERSION >= 43
	.data
break_cnt:
	.word	0xffffff
#endif
	.text
#if VERSION < 43
break_cnt:
	.word	0xffffff
#endif
	.globl	DecDCTvlcSize
DecDCTvlcSize:
	la	$8,break_cnt
	addi	$1,$4,-1
	blez	$1,default
	lw	$2,0($8)
	sll	$1,$4,0x1
	jr	$31
	sw	$1,0($8)
default:
	lui	$1,0xff
	ori	$1,$1,0xffff
	jr	$31
	sw	$1,0($8)
	.globl	DecDCTvlc
DecDCTvlc:
	la	$8,break_cnt
	la	$6,CF_DVLC
	la	$7,CF2_DVLC
	bnez	$4,normal
	lw	$9,0($8)
	la	$8,save_regs
	lw	$4,0($8)
	lw	$5,4($8)
	lw	$2,8($8)
	lw	$3,12($8)
	lw	$12,16($8)
	lw	$13,20($8)
	lw	$15,24($8)
	lw	$24,28($8)
	lw	$25,32($8)
	add	$9,$9,$9
	b	loop
	add	$14,$5,$9
normal:
	add	$13,$0,$0
	add	$15,$0,$0
	add	$24,$0,$0
	add	$25,$0,$0
	add	$9,$9,$9
	add	$14,$5,$9
#if VERSION < 43
	lhu	$8,0($4)
	lhu	$9,2($4)
#else
	lw	$9,0($4)
#endif
	lhu	$12,4($4)
	lhu	$10,6($4)
	lhu	$2,8($4)
	lhu	$3,10($4)
	addi	$10,$10,-3
	bltz	$10,vlc1
	sll	$12,$12,0xa
	addi	$13,$0,1
vlc1:
	addi	$4,$4,12
	sll	$2,$2,0x10
	or	$2,$2,$3
	or	$3,$0,$0
#if VERSION < 43
	sh	$8,0($5)
	sh	$9,2($5)
#else
	sw	$9,0($5)
	andi	$9,$9,0xffff
	sll	$9,$9,0x2
	addiu	$9,$9,4
	add	$9,$9,$5
	la	$8,end_ptr
	sw	$9,0($8)
#endif
	addi	$5,$5,2
endblock:
	beqz	$13,normaldc
	srl	$8,$2,0x16
	xori	$1,$8,0x3ff
	beqz	$1,epilogue
	addi	$5,$5,2
	addi	$1,$13,-3
	bltz	$1,found
	addi	$1,$6,-1024
	addi	$1,$1,-1024
found:
	srl	$8,$2,0x18
	sll	$8,$8,0x2
	add	$8,$8,$1
	lhu	$9,0($8)
	lhu	$10,2($8)
	and	$8,$0,$0
	beqz	$10,zerolen
	sllv	$2,$2,$9
	addi	$1,$0,32
	sub	$1,$1,$10
	srlv	$8,$2,$1
	bltz	$2,pos_dc
	sllv	$2,$2,$10
	addi	$11,$0,-1
	srlv	$11,$11,$1
	sub	$8,$8,$11
pos_dc:
	add	$3,$3,$10
zerolen:
	add	$3,$3,$9
	andi	$1,$3,0x10
	beqz	$1,fresh0
	andi	$3,$3,0xf
	lhu	$9,0($4)
	addi	$4,$4,2
	sllv	$9,$9,$3
	or	$2,$2,$9
fresh0:
	addi	$1,$13,-2
	bgtz	$1,y_block
	add	$9,$25,$8
	beqz	$1,cb_block
	add	$9,$24,$8
	add	$9,$15,$8
	b	gotit
	add	$15,$15,$8
cb_block:
	b	gotit
	add	$24,$24,$8
y_block:
	add	$25,$25,$8
gotit:
	sll	$9,$9,0x2
	andi	$9,$9,0x3ff
	or	$9,$12,$9
	addi	$13,$13,1
	addi	$1,$13,-7
	bnez	$1,brkcheck
	sh	$9,0($5)
	b	brkcheck
	addi	$13,$13,-6
normaldc:
	xori	$1,$8,0x1ff
	beqz	$1,epilogue
	addi	$5,$5,2
	sll	$2,$2,0xa
	addi	$3,$3,10
	andi	$1,$3,0x10
	beqz	$1,fresh1
	andi	$3,$3,0xf
	lhu	$9,0($4)
	addi	$4,$4,2
	sllv	$9,$9,$3
	or	$2,$2,$9
fresh1:
	or	$8,$12,$8
	sh	$8,0($5)
brkcheck:
	subu	$1,$5,$14
	bgez	$1,abort
	addi	$5,$5,2
loop:
	srl	$8,$2,0x13
	sll	$8,$8,0x3
	add	$8,$8,$6
	lw	$9,0($8)
	nop
	bnez	$9,gotfirst
	andi	$1,$9,0xff
	sll	$2,$2,0x8
	addi	$3,$3,8
	andi	$1,$3,0x10
	beqz	$1,fresh2
	andi	$3,$3,0xf
	lhu	$8,0($4)
	addi	$4,$4,2
	sllv	$8,$8,$3
	or	$2,$2,$8
fresh2:
	srl	$8,$2,0x17
	sll	$8,$8,0x2
	add	$8,$8,$7
	lw	$9,0($8)
	add	$11,$0,$0
	b	gotcode
	andi	$1,$9,0xff
gotfirst:
	lw	$11,4($8)
gotcode:
	sllv	$2,$2,$1
	add	$3,$3,$1
	andi	$1,$3,0x10
	beqz	$1,fresh3
	andi	$3,$3,0xf
	lhu	$8,0($4)
	addi	$4,$4,2
	sllv	$8,$8,$3
	or	$2,$2,$8
fresh3:
	srl	$9,$9,0x10
	xori	$1,$9,0x7c1f
	beqz	$1,escape
	xori	$1,$9,0xfe00
	beqz	$1,endblock
	sh	$9,0($5)
	beqz	$11,loop
	addi	$5,$5,2
	andi	$10,$11,0xffff
	xori	$1,$10,0x7c1f
	beqz	$1,escape
	xori	$1,$10,0xfe00
	beqz	$1,endblock
	sh	$10,0($5)
	srl	$10,$11,0x10
	beqz	$10,loop
	addi	$5,$5,2
	xori	$1,$10,0x7c1f
	beqz	$1,escape
	xori	$1,$10,0xfe00
	beqz	$1,endblock
	sh	$10,0($5)
	b	loop
	addi	$5,$5,2
escape:
	srl	$8,$2,0x10
	sh	$8,0($5)
	addi	$5,$5,2
	lhu	$8,0($4)
	addi	$4,$4,2
	sll	$2,$2,0x10
	sllv	$8,$8,$3
	b	loop
	or	$2,$2,$8
epilogue:
#if VERSION >= 43
	la	$8,end_ptr
	lw	$9,0($8)
#endif
	li	$8,0xfe00
#if VERSION < 43
	addi	$2,$0,64
#endif
fill:
#if VERSION >= 43
	subu	$1,$5,$9
	bgez	$1,endfill
	nop
#endif
	sh	$8,0($5)
#if VERSION >= 43
	b	fill
#endif
	addi	$5,$5,2
#if VERSION < 43
	bnez	$2,fill
	addi	$2,$2,-1
	mfc0	$9,$12
	nop
	lui	$1,0x2
	or	$9,$9,$1
	mtc0	$9,$12
#else
endfill:
#endif
	jr	$31
	add	$2,$0,$0
abort:
	la	$8,save_regs
	sw	$4,0($8)
	sw	$5,4($8)
	sw	$2,8($8)
	sw	$3,12($8)
	sw	$12,16($8)
	sw	$13,20($8)
#if VERSION < 43
	sw	$15,28($8)
	sw	$24,32($8)
	sw	$25,36($8)
#else
	sw	$15,24($8)
	sw	$24,28($8)
	sw	$25,32($8)
#endif
	jr	$31
	addi	$2,$0,1
	.data
save_regs:
#if VERSION < 43
	.space	40
#else
	.space	36
end_ptr:
	.word	0
#endif
