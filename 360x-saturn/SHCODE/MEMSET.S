;;============================================================
;;============================================================
;;	Pats memset functions
;;============================================================
;;============================================================

	opt	c+
	section	.text
	
	xdef	memset
	xdef	word_memset
	xdef	long_memset


;;============================================================
;;
;;	r4	target address
;;	r5	value (assume to be byte)
;;	r6	write length in bytes
;;
;;============================================================

memset:
	xor.l	r0,r0
	sub	#1,r6

@copy_loop:
	move.b	r5,@r4
	add	#1,r4
	cmp/eq	r6,r0
	bf/s	@copy_loop
	sub	#1,r6
	
	rts
	nop

	littab


;;============================================================
;;
;;	r4	target address
;;	r5	value (assume to be word)
;;	r6	write length in bytes
;;
;;============================================================

word_memset:
	xor.l	r0,r0
	sub	#2,r6

@copy_loop:
	move.w	r5,@r4
	add	#2,r4
	cmp/eq	r6,r0
	bf/s	@copy_loop
	sub	#2,r6
	
	rts
	nop

	littab


;;============================================================
;;
;;	r4	target address
;;	r5	value (assume to be long)
;;	r6	write length in bytes
;;
;;============================================================

long_memset:
	xor.l	r0,r0
	sub	#4,r6

@copy_loop:
	move.l	r5,@r4
	add	#4,r4
	cmp/eq	r6,r0
	bf/s	@copy_loop
	sub	#4,r6
	
	rts
	nop

	littab
