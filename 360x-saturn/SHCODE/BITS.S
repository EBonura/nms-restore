	.align 4

	include "shcode\sh2.mac"

	section	.text

	.align 4

;webheight equ   184

	xdef	webwidth
;webwidth  dc.l	320


	global 	read_pc_long
	global	reverse_long

	xref	Screen_address
	xref	screen
	xref	screenbuffer
	xdef	memset

VRAM	equ	$25c00000

	.align 4
	lits

	
;R4 = address  R5 = Number of longs to switch
reverse_long

rloop:	mov.b	@r4,r0
	extu.b	r0,r0
	add	#1,r4
	mov.b	@r4,r1
	extu.b	r1,r1
	add	#1,r4
	mov.b	@r4,r2
	extu.b	r2,r2
	add	#1,r4
	mov.b	@r4,r3
	extu.b	r3,r3
	mov.b	r0,@r4
	sub	#1,r4
	mov.b	r1,@r4
	sub	#1,r4
	mov.b	r2,@r4
	sub	#1,r4
	mov.b	r3,@r4
	sub	#1,r5
	tst	r5,r5
	bf/s	rloop
	add	#4,r4
	
	swap.b	r1,r1	
	swap.w	r2,r2	
	swap.b	r3,r3	
	swap.w	r3,r3	
	add	r1,r0
	add	r2,r0
	rts
	add	r3,r0

	lits
	.align 4
	
;R4 = address  R5 = Number of words to switch
reverse_word

rwloop:	mov.b	@r4,r0
	extu.b	r0,r0
	add	#1,r4
	mov.b	@r4,r1
	extu.b	r1,r1
	mov.b	r0,@r4
	sub	#1,r4
	mov.b	r1,@r4

	sub	#1,r5
	tst	r5,r5
	bf/s	rwloop
	add	#2,r4
	
	swap.b	r1,r1
	rts
	add	r1,r0	

	lits
	.align 4
	
reverse_long2
	swap.b	r4,r4
	swap.w	r4,r4
	swap.b	r4,r4
	rts
	mov.l	r4,r0

	lits
	.align 4

reverse_word2
	swap.b	r4,r4
	rts
	mov.l	r4,r0

	lits


