;
; Collision check with background
;


	opt	c+
	section	.text

	xdef	SHcollision_at

	xref	collmap
	xref	MapWidth

;  r0			 r4    r5
; flag SHcollision_at(long x,long y)
;

TRUE	equ	1
FALSE	equ	0

SHcollision_at:
	
	move.l	=MapWidth,r0		;get required globals
	move.l	=collmap,r1
	move.w	@r0,r0			;get the mapwidth
	move.l	@r1,r1			;get the address!

	mul.l	r5,r0			;y*MapWidth -> macl



					;setup 'bit'
	move.l	#7,r3
	move.l	r4,r2			;x into r2
	and	r3,r2			;result in r2
	sub	r2,r3			;7-(x&7)

	move.l	macl,r6			;setup 'mask'
	add	r4,r6
	shlr2	r6
	shlr	r6
	add	r1,r6
	
	move.b	@r6,r0			

	shll	r3			;complete 'bit' here to unstall pipeline
	and	r3,r0
	

	cmp/eq	#0,r0
	bf	@true

@false
	move.l	#FALSE,r0
	bra	@done
	nop				;;delay slot

@true	move.l	#TRUE,r0
	
@done:
	rts
	nop

	littab

