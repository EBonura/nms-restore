
	include	'shcode\sh.mac'

	opt	c+

	section	.text

	xdef	shRle2raw

	;r0=free
	;r1=src_len
	;r2=temp
	;r3=col

;;			r4	      r5
;;void	Rle2raw(Uint8 *source,Uint8 *dest)
;;

shRle2raw:
	mov.l	@r4+,r1		;r1=src_len

@main_loop:
	
	mov.b	@r4+,r0		;temp=*src++
	cmp/pl	r0		;if(temp>0)
	bt	@copy_bytes


@copy_color:
	sub	#2,r1		;src_len-=2
	mov.b	@r4+,r6		;r6=*src++
	neg	r0,r0		;temp=1-temp
	extu.b	r6,r6
	add	#1,r0

	@color_loop:
		mov.b	r6,@r5	;*dest=col(r6)
		sub	#1,r0	;temp--
		tst	r0,r0
		bf/s	@color_loop
		add	#1,r5	;dest++
	
		bra	@done
		nop

@copy_bytes:
	add	#1,r0		;temp++
	sub	r0,r1		;src_len-=temp
	sub	#1,r1		;...-1

	@copy_loop:
		mov.b	@r4+,r6
		sub	#1,r0
		extu.b	r6,r6
		
	    ;	shll2	r6
	    ;	shll2	r6

	    ;	mov.l	r6,r7
	    ;	swap.b	r6,r6
	    ;	or	r7,r6

		mov.b	r6,@r5
		
		tst	r0,r0
		bf/s	@copy_loop
		add	#1,r5


@done:
	cmp/pl	r1
	bt	@main_loop


	rts
	nop

	littab

	
	end
