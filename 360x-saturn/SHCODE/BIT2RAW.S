
	include	'shcode\sh.mac'

	opt	c+

	section	.text

	xdef	shBit2raw

;;				r4	     r5
;;	void shBit2raw(Uint8 *source,Uint8 *dest)
;;

	;r0=free
	;r1=destlen
	;r2=mask
	;r3=src_cols
	;r4=source
	;r5=dest
	;r6=DATA

shBit2raw:
	mov.l	@r4+,r1		;destlen -> r1

	mov.l	r4,r3
	mov.l	r1,r0
	sub	#1,r0
	shlr2	r0
	shlr	r0
	add	#1,r0

	add	r0,r3		;src_cols=source+((destlen-1)>>3)+1
		
@main_loop:

	mov.b	#$80,r0		;mask = 0x80
	extu.b	r0,r2		;r2=mask

		@sub_loop:
			mov.b	@r4,r6		;*source
			and	r2,r6		;(*source & mask)
		
			tst	r6,r6
			bt	@is_zero
		
		@not_zero:
			mov.b	@r3+,r0		;*dest=*src_cols
			extu.b	r0,r0
			
		     ;	shll2	r0
		     ;	shll2	r0

		     ;	mov.l	r0,r7
		     ;	swap.b	r0,r0
		     ;	or	r7,r0

			bra	@end_subloop
			mov.b	r0,@r5

		@is_zero:
			mov.b	r6,@r5		;*dest=0
						;above test failed so r6 WILL be zero
		
		@end_subloop:
			shlr	r2		;mask>>=1
			
			tst	r2,r2
			bf/s	@sub_loop
			add	#1,r5		;dest++

	sub	#8,r1		;destlen -= 8
       	add	#1,r4		;source++

	cmp/pl	r1
	bt	@main_loop	

	rts
	nop

	littab

	end
	
