	section	.rdata  
	section	.text   
	section	.data   
	section	.sdata  
	
	section	.sbss	; The bss sections
	section	.bss

	section	.text

	global	slSin
	global	slCos
	global	slMulFX
	global	slSquart



slSquart:	li	t1,16
	move	t0,zero
	move	t2,zero
@loop0	rol	a0,a0,2
	move	t2,a0
	and	t2,3
	sll	t0,t0,1
	add	t0,t0,1
	bgt	t0,t2,@loop1
	add	t1,t1,-1
	sub	t2,t2,t0
	bne	t1,zero,@loop0
	add	t0,t0,1
	srl	t0,t0,1
	jr	ra
	move	v0,t0
@loop1	bne	t1,zero,@loop0
	add	t0,t0,-1
	srl	t0,t0,1
	jr	ra
	move	v0,t0

slMulFX:	mult	a0,a1
	mflo	v0
	mfhi	v1
	nop
	nop
	srl	v0,v0,16		; lose unwanted data
	sll	v1,v1,16
	jr	ra
	or	v0,v0,v1

slSin:	move	t1,a0
	move	t2,t1
	sll	t1,t1,8
	srl	t2,t2,8
	or	t1,t1,t2
	and	t1,t1,$ffff		;swap.b t1,a0
	srl	t0,t1,2
	and	t0,t0,$38
	move	t2,t0
	la	t0,sin_index_tab
	add	t2,t0,t2
	lb	t0,5(t2)
	li	t1,$3ffe
	xor	a0,a0,t0
	lw	t0,(t2)
	and	a0,t1,a0
	add	t0,t0,a0
	lhu	t0,(t0)
	lb	t1,7(t2)
	lb	t2,6(t2)
	xor	t0,t0,t1
	jr	ra
	add	v0,t0,t2

slCos:	move	t1,a0
	move	t2,t1
	sll	t1,t1,8
	srl	t2,t2,8
	or	t1,t1,t2
	and	t1,t1,$ffff		;swap.b t1,a0
	srl	t0,t1,2
	and	t0,t0,$38
	move	t2,t0
	la	t0,cos_index_tab
	add	t2,t0,t2
	lb	t0,5(t2)
	li	t1,$3ffe
	xor	a0,a0,t0
	lw	t0,(t2)
	and	a0,t1,a0
	add	t0,t0,a0
	lhu	t0,(t0)
	lb	t1,7(t2)
	lb	t2,6(t2)
	xor	t0,t0,t1
	jr	ra
	add	v0,t0,t2

	section	.data

sin_index_tab	dw	sincostab+$40, $00000000
	dw	sincostab+$40, $00010000
cos_index_tab	dw	sincostab+$42, $0001ff00
	dw	sincostab+$42, $0000ff00
	dw	sincostab+$40, $ff010000
	dw	sincostab+$40, $ff000000
	dw	sincostab+$42, $ff00ff00
	dw	sincostab+$42, $ff01ff00
	dw	sincostab+$40, $00000000
	dw	sincostab+$42, $00010000

;sincostab	incbin	"sincos.tab"
sincostab	incbin	"sin_cos.tab"
