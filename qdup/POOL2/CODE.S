	section	.rdata  
	section	.text   
	section	.data   
	section	.sdata  
	
	section	.sbss	; The bss sections
	section	.bss



	section	.text

	global	Rts
	global	rle2raw

	global	get_ram_available
	global	get_ram_start

	global	_stacksize
	global	_ramsize

	global	VblCallback
	global	wait_for_phils_VBL
	global	vblcount
	global	get_moment
	global	mul64


;
; Convert Big-Endian To Little-Endian
;
; In: A0 = Start Address
;     A1 = Number Of Entries
;     A2 = Type (0 = Short, 1 = Long )
ByteSwap:	bne	a2,zero,@swaplong
	nop
@swapshort	lh	v0,(a0)
	addiu	a1,a1,-1
	and	v1,v0,$ff00
	and	v0,v0,$00ff
	sll	v0,v0,8
	srl	v1,v1,8
	or	v0,v0,v1
	sh	v0,(a0)
	bne	a1,zero,@swapshort
	addiu	a0,a0,2
	jr	ra
	nop
@swaplong	lw	t0,(a0)
	addiu	a1,a1,-1
	sll	t1,t0,24
	sll	t2,t0,8
	and	t2,t2,$00ff0000
	srl	t3,t0,8
	and	t3,t3,$0000ff00
	srl	t4,t0,24
	or	t0,t1,t2
	or	t0,t0,t3
	or	t0,t0,t4
	sw	t0,(a0)
	bne	a1,zero,@swaplong
	addiu	a0,a0,4
	jr	ra
	nop

get_ram_available:	la	v0,sectend(.bss)
	sll	v0,v0,3
	srl	v0,v0,3

	sub	a0,a0,8
	subu	a0,a0,a1
	ori	a1,a0,$80000000
	sw	a1,ram_end
	jr	ra
	subu	v0,a0,v0	;return available ram

VblCallback:	lw	v0,vblcount
	nop
	addiu	v0,v0,1
	sw 	v0,vblcount
	nop
	jr	ra
	nop


wait_for_phils_VBL:
	lw	v0,vblcount
@wait	lw	v1,vblcount
	nop
	beq	v1,v0,@wait
	nop
	sw	zero,vblcount
	jr	ra
	nop

get_ram_start:
	lw	a1,ram_end
	la	v0,sectend(.bss)
	addu	a2,a0,v0
	bleu	a2,a1,@exit
	nop
	move	v0,zero
@exit	jr	ra
	nop

	
rle2raw:
	lw	a2,(a0)		;src_len=*(u_long*)source++
	addiu	a0,a0,4
@outer	beq	a2,zero,@exit	;while(src_len != 0)
	lb	a3,(a0)		;temp = *source++
	addiu	a2,a2,-2	;src_len -= 2
	bge	a3,zero,@count	;if(temp>0)
	addiu	a0,a0,1
	lbu	v0,(a0)		;col = *source++
	addiu	a0,a0,1
@in1	sb	v0,(a1)
	addiu	a3,a3,1
	ble	a3,zero,@in1
	addiu	a1,a1,1
	j	@outer
	nop
@count	sub	a2,a2,a3
@co1	lbu	v0,(a0)
	addiu	a3,a3,-1
	sb	v0,(a1)
	addiu	a1,a1,1
	bge	a3,zero,@co1
	addiu	a0,a0,1
	j	@outer
	nop
@exit	jr	ra
	nop

get_moment:	mult	a1,a1
	mflo	v0
	mfhi	v1
	nop
	nop
	srl	v0,v0,24		; lose unwanted data
	sll	v1,v1,8
	or	v0,v0,v1
	mult	v0,a0
	mflo	v0
	mfhi	v1
	nop
	nop
	srl	v0,v0,24		; lose unwanted data
	sll	v1,v1,8
	or	v0,v0,v1
	li	v1,5
	div	v0,v0,v1
	mflo	v0
	nop
	nop
	jr	ra
	sll	v0,v0,1


mul64:	mult	a0,a1
	mflo	v0
	mfhi	v1
	nop
	nop
	srl	v0,v0,24		; lose unwanted data
	sll	v1,v1,8
	jr	ra
	or	v0,v0,v1



ram_end	dw	0



