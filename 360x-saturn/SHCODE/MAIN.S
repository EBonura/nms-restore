;;
;;
;; Start up code to clear bss for SGL
;;
;;


	opt	c+

	section	.text
	section	.ctors
	section	.dtors
	section	.data
	section	.bss

	xdef	main
	xref	ss_main

	section	.text


main:
	mov.l	=sectend(.bss)-sect(.bss),r0	; clear bss
	mov.l	=sect(.bss),r1			; get address of bss
	mov.l	#0,r2				; zero

	cmp/eq	#0,r0				; is bss zero length?
	bt	@done				; yes then exit to main

@clear_loop:
	mov.b	r2,@r1
	dt	r0
	bf/s	@clear_loop
	add.l	#1,r1


@done:
	move.l	=ss_main,r0
	jsr	@r0
	nop

