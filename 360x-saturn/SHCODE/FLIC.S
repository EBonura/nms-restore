;****************************************************************
;*    								*
;*   IRQ setup routines						*
;*               								*
;****************************************************************

	.align 4

	include "shcode\sh2.mac"
;	include "equ.h"
;	include "regs.h"
;	include "globals.i"

	section	.text

	.align 4

webheight equ   184

	xdef	webwidth
webwidth  dc.l	320


esi	equr	r4
edi	equr	r5
edx	equr	r6
ebx	equr	r7
eax	equr	r8

	global 	L_Fill_Area

	global 	read_pc_long
	global 	readpad
	global 	code_dots
	global 	code_webdecomp
	global 	webdecomp
	global 	waitvbl
	global 	init_irqs
	global 	init_vdp1
	global	start_draw
	global	clear_fb
	global	flip_fb
	global	see_back
	global	reverse_long
	global	reverse_word
	global	reverse_word2
	global	reverse_long2
	global	squareroot
	global	trans
	global	mask_flat_ball
	global	checkvbl

	global	ASM_BLIT_DM_IMAGE_MERGE
	global	ASM_BLIT_DM_IMAGE
	global	ASM_FAST_BLIT_DM_160BY24
	global	ASM_RUNLEN_FAST_BLIT_DM_160BY24
	global	ASM_DOTMAT_CLR_TO_DARK_BROWN
	global	FASTEST_ASM_DOTMAT_PRN_BLACK
	global	FASTEST_ASM_DOTMAT_PRN

	global	ASM_RUNLEN_FAST_BLIT_DM_160BY24_OVERLAY
	global	ASM_BLIT_DM_IMAGE_MASKED
	global	ASM_FASTEST_DOTMAT_PRN
	global	ASM_DOTMAT_PRN_640480



	xref	DMA_CpuMemCopy16

	xref	Screen_address
	xref	dot_table
	xref	screen
	xref	screenbuffer
	xref	rssData
	xref	hitData
	xref	chunk
	xref	dotnum
	xref	tenbuffer
	xref	tendata
	xref	lines
	xref	longtemp

;	xdef	memset

VRAM	equ	$25c00000

	.align 4
	lits



	.align 4
;****************************************************************
;*								*
;*   playweb bits						*
;*   r4 is not set up in the function				*
;****************************************************************
;;;;;;;;;;;;;
; Register usage:-
; r0 = temp
; r1 = temp
; r2 = temp
; r3 = temp
; r4 = most_frequent colour
; r5 = free
; r6 = const
; r7 = const
; r8 = source
; r9 = dest
; r10= olddest
; r11= x
; r12= y
; r13= const

code_webdecomp

	mov.l	r1,@-r15
	mov.l	r2,@-r15
	mov.l	r3,@-r15
	mov.l	r4,@-r15
	mov.l	r5,@-r15
	mov.l	r6,@-r15
	mov.l	r7,@-r15
	mov.l	r8,@-r15
	mov.l	r9,@-r15
	mov.l	r10,@-r15
	mov.l	r11,@-r15
	mov.l	r12,@-r15
	mov.l	r13,@-r15
	mov.l	r14,@-r15
	sts.l	pr,@-r15

	mov.l	r5,r8			; r8 = source

	mov.l	r4,r10
	mov	=webwidth,r0		; r6 = screen width
	mov	=320,r1			; r6 = screen width
	mov.l	r1,@r0	

web_loop:

;	mov.l	=VRAM,r10		; r10 = old dest
;	add	r12,r10
;	mov.l	=$6066000,r10

	mov	#1,r14
	mov.l	=code_cel_types,r13	; r13 = call address

	mov	#200/8,r12		;176/8,r12   ; r12 = y
web_yloop:
	mov.l	r12,@-r15
	mov	#320/8,r11		;#36,r11		 ;288/8	; r11 = x
web_xloop:
	mov	#24,r2
	mov.b	@r8+,r1
	extu.b	r1,r0  				; pipeline collision

	cmp/gt	r2,r0
	bt	get_out_error2
	and	#3,r0
	tst	r0,r0
	bf	get_out_error2
		    
	extu.b	r1,r0
 
	mov.l	@(r0,r13),r0
	jsr	@r0   				; r13 = call address
	mov	r10,r9				; r9 = dest

	dt	r11
	bf/s	web_xloop
	add	#8,r10				; r10 = old dest

;	add	#-32,r10				; WHY!!!!
	mov.l	@r15+,r12

	mov	=320*7,r1	;=2016,r1 		;288*7
	dt	r12
	bf/s	web_yloop
	add	r1,r10				; r10 = old dest
	mov	#0,r14
	bra	no_error2
	nop

get_out_error2
	mov.l	@r15+,r12
no_error2
	mov.l	r14,r0
	lds.l	@r15+,pr
	mov.l	@r15+,r14
	mov.l	@r15+,r13
	mov.l	@r15+,r12
	mov.l	@r15+,r11
	mov.l	@r15+,r10
	mov.l	@r15+,r9
	mov.l	@r15+,r8
	mov.l	@r15+,r7
	mov.l	@r15+,r6
	mov.l	@r15+,r5
	mov.l	@r15+,r4
	mov.l	@r15+,r3
	mov.l	@r15+,r2
	mov.l	@r15+,r1
	rts
	nop

	lits

		.align 4
code_cel_types	dc.l	_code_zap		   ;0
		dc.l	_code_cel_deviant3	   ;1
		dc.l	_code_cel_delta_deviant3f  ;2
		dc.l	_code_cel_delta_deviant3f4 ;3
		dc.l	_code_cel_delta_deviant3f3 ;4
		dc.l	_code_cel_deviant7	   ;5
		dc.l	_code_cel_nochange	   ;6

    		.align	4

_code_cel_nochange

	rts
	nop

    		.align	4

_code_cel_cc
	sts.l	pr,r12

	mov	=webwidth,r1				; r6 = screen width
	mov.l	@r1,r6	
	mov	=jt_cc,r7			; r7 = vector table

	REPT 8
	mov.b	@r8+,r1
	mov	r0,r2
	extu.b	r1,r0
	shll2	r0
	mov.l	@(r0,r7),r1
	jsr	@r1
	mov	r2,r0
	add	r6,r9				; move to next line
	ENDR

	lds.l	r12,pr
	rts	     
	nop

	lits

	.align 4
;jt_cc
	include shcode\cc.asm
	
	lits

	.align	4
_code_zap
	mov.b	@r8+,r1				; r8 = source
	extu.b	r1,r1				; pipeline collision
	mov	r1,r0
	shll8	r1
	or	r1,r0
	shll8	r1
	or	r1,r0
	shll8	r1
	or	r1,r0

	mov	=webwidth,r6
	mov.l	@r6,r1	
	rept 8
		mov.l	r0,@r9
		mov.l	r0,@(4,r9)
		add	r1,r9
	endr

	rts
	nop

	lits

MDeviant34 MACRO
	local 	fred
	mov.b	@r8+,r4
	mov.b	@r8+,r7
	mov	r11,r0
	shlr	r10
	bf/s	fred
	extu.b	r4,r4
	mov	r12,r0
fred:
	shll2	r4
	mov.l	@(r0,r4),r4
	extu.b	r7,r7
	shll2	r7
	jsr	@r4	   			; r1 = call address
	mov.l	@(r0,r7),r7

	jsr     @r7	   			; r1 = call address
	mov.l	r0,@r9
	mov.l	r0,@(4,r9)

	add	r13,r9				; move to next line
	ENDM

	.align	4
_code_cel_deviant34
	mov.l	r10,@-r15
	mov.l	r11,@-r15
	mov.l	r13,@-r15
	sts.l	pr,@-r15
	
	mov	=jt_plane2,r11
	mov	=jt_deviant3,r12
	mov	=webwidth,r1
	mov.l	@r1,r13	

	mov.b	@r8+,r1				; r1 = col 0
	mov.b	@r8+,r2				; r2 = col 1
	mov.b	@r8+,r3				; r3 = col 2
	mov.b	@r8+,r5				; r5 = col 3
	mov.b	@r8+,r10			; r7 = mask

	extu.b	r1,r1
	extu.b	r2,r2
	extu.b	r3,r3
	extu.b	r5,r5
	
	swap.b	r1,r0
	swap.w	r0,r4
	or	r4,r1

	swap.b	r2,r0
	swap.w	r0,r4
	or	r4,r2

	swap.b	r3,r0
	swap.w	r0,r4
	or	r4,r3

	swap.b	r5,r0
	swap.w	r0,r4
	or	r4,r5

	REPT 8
	MDEVIANT34
	ENDR

	lds.l	@r15+,pr
	mov.l	@r15+,r13
	mov.l	@r15+,r11
	mov.l	@r15+,r10
	rts	     
	nop

	lits


MDeviant3f MACRO
	local fred1,fred2
	swap.w	r1,r0				; opt1
	shlr	r10
	bf/s	fred1
	or	r1,r0

	mov.b	@r8+,r4
	extu.b	r4,r0				; r0 = mask
	shll2	r0
	mov.l	@(r0,r7),r4
	jsr	@r4	   			; r1 = call address
	nop
fred1:
 	move.l	r0,@r9

	swap.w	r1,r0				; opt1
	shlr	r10
	bf/s	fred2
	or	r1,r0

	mov.b	@r8+,r4
	extu.b	r4,r0				; r0 = mask
	shll2	r0
	mov.l	@(r0,r7),r4
	jsr	@r4	   			; r1 = call address
	nop
fred2:
	move.l	r0,@(4,r9)

	add	r12,r9
	ENDM
		                               
	.align	4
_code_cel_delta_deviant3f
	mov.l	r10,@-r15
	sts.l	pr,@-r15

	mov.l	=webwidth,r1
	mov.l	@r1,r12
	mov.l	=jt_deviant3,r7			; r7 = vector table

	mov.b	@r8+,r1				; r8 = source
	mov.b	@r8+,r2				; r8 = source
	mov.b	@r8+,r3				; r8 = source
	mov.b	@r8+,r0				; r8 = source
	mov.b	@r8+,r10			; r8 = source
	extu.b	r1,r1
	extu.b	r2,r2
	extu.b	r3,r3
	extu.b	r0,r0
	extu.b	r10,r10
	shll8	r10
	or	r0,r10				; r10 = word mask
	
	swap.b	r1,r0
	swap.w	r0,r4
	or	r4,r1

	swap.b	r2,r0
	swap.w	r0,r4
	or	r4,r2

	swap.b	r3,r0
	swap.w	r0,r4
	or	r4,r3

	REPT 8
	MDEVIANT3f
	ENDR

	lds.l	@r15+,pr
	mov.l	@r15+,r10
	rts	     
	nop

	lits
	

;****************************************************************
;*    								*
;*   The BIG ONE						*
;*               								*
;****************************************************************

MDeviant3f4 MACRO
	local fred1
	shlr	r10
	bf	fred1

	mov.b	@r8+,r4
	extu.b	r4,r0				; r0 = mask
	shll2	r0
	mov.l	@(r0,r7),r4
	jsr	@r4	   			; r1 = call address
	nop
fred1:
	move.l	r0,@r9
	add	r12,r9
	ENDM
	
	.align	4
_code_cel_delta_deviant3f4
	mov.l	r10,@-r15
	sts.l	pr,@-r15

	mov.l	=webwidth,r1
	mov.l	@r1,r12
	mov.l	=jt_deviant3,r7			; r7 = vector table

	mov.b	@r8+,r1				; r8 = source
	mov.b	@r8+,r2				; r8 = source
	mov.b	@r8+,r3				; r8 = source
	mov.b	@r8+,r0				; r8 = source
	mov.b	@r8+,r10			; r8 = source
	extu.b	r1,r1
	extu.b	r2,r2
	extu.b	r3,r3
	extu.b	r0,r0
	extu.b	r10,r10
	shll8	r10
	or	r0,r10				; r10 = word mask
	
	swap.b	r1,r0
	swap.w	r0,r4
	or	r4,r1

	swap.b	r2,r0
	swap.w	r0,r4
	or	r4,r2

	swap.b	r3,r0
	swap.w	r0,r4
	or	r4,r3

	push	r9
	swap.w	r1,r0				; opt1
	or	r1,r0
	rept	8
		MDeviant3f4
	endr
	pop	r9
	add	#4,r9

	swap.w	r1,r0				; opt1
	or	r1,r0
	rept	8
		MDeviant3f4
	endr

	lds.l	@r15+,pr
	mov.l	@r15+,r10
	rts	     
	nop

	lits
	
;****************************************************************
;*    								*
;*   The LITTLE ONE						*
;*               								*
;****************************************************************

MDeviant3f3 MACRO
	local fred1
	shlr	r10
	bf	fred1

	mov.b	@r8+,r5
	extu.b	r5,r0				; r0 = mask
	shll2	r0
	mov.l	@(r0,r7),r5
fred1:
	jsr	@r5	   			; r1 = call address
	nop
	move.l	r0,@r9
	add	r12,r9
	ENDM
	
	.align	4
_code_cel_delta_deviant3f3
	mov.l	r10,@-r15
	sts.l	pr,@-r15

	mov.l	=webwidth,r1
	mov.l	@r1,r12
	mov.l	=jt_deviant3,r7			; r7 = vector table

	mov.b	@r8+,r1				; r8 = source
	mov.b	@r8+,r2				; r8 = source
	mov.b	@r8+,r3				; r8 = source
	mov.b	@r8+,r0				; r8 = source
	mov.b	@r8+,r10			; r8 = source
	extu.b	r1,r1
	extu.b	r2,r2
	extu.b	r3,r3
	extu.b	r0,r0
	extu.b	r10,r10
	shll8	r10
	or	r0,r10				; r10 = word mask
	
	swap.b	r1,r0
	swap.w	r0,r4
	or	r4,r1

	swap.b	r2,r0
	swap.w	r0,r4
	or	r4,r2

	swap.b	r3,r0
	swap.w	r0,r4
	or	r4,r3

	push	r9
	move.l	=deviant3_0,r5
	rept	8
		MDeviant3f3
	endr
	pop	r9
	add	#4,r9
	move.l	=deviant3_0,r5
	rept	8
		MDeviant3f3
	endr

	lds.l	@r15+,pr
	mov.l	@r15+,r10
	rts	     
	nop

	lits
	

MDeviant3 MACRO
	local fred1,fred2
	shlr	r10
	bf/s	fred1
	swap.w	r1,r0				; opt1

	mov.b	@r8+,r4
	mov.b	@r8+,r5
	extu.b	r4,r0				; r0 = mask
	shll2	r0
	mov.l	@(r0,r7),r4
	extu.b	r5,r0				; r0 = mask
	shll2	r0

	jsr	@r4	   			; r1 = call address
	mov.l	@(r0,r7),r5

	jsr	@r5	   			; r1 = call address
	mov.l	r0,@r9
	bra	fred2
	mov.l	r0,@(4,r9)
fred1:
	or	r1,r0
	move.l	r0,@r9
	move.l	r0,@(4,r9)
fred2:
	add	r12,r9
	ENDM
		                               
	.align	4
_code_cel_delta_deviant3
	mov.l	r10,@-r15
	sts.l	pr,@-r15

	mov.l	=webwidth,r1
	mov.l	@r1,r12
	mov.l	=jt_deviant3,r7			; r7 = vector table

	mov.b	@r8+,r1				; r8 = source
	mov.b	@r8+,r2				; r8 = source
	mov.b	@r8+,r3				; r8 = source
	mov.b	@r8+,r10			; r8 = source
	extu.b	r1,r1
	extu.b	r2,r2
	extu.b	r3,r3
	
	swap.b	r1,r0
	swap.w	r0,r4
	or	r4,r1

	swap.b	r2,r0
	swap.w	r0,r4
	or	r4,r2

	swap.b	r3,r0
	swap.w	r0,r4
	or	r4,r3

	REPT 8
	MDEVIANT3
	ENDR

	lds.l	@r15+,pr
	mov.l	@r15+,r10
	rts	     
	nop

	lits

;****************************************************************
;*    								*
;*   The REAL TWAT						*
;*               								*
;****************************************************************

MDEVIANT7	MACRO
	local   noload_dv7_0,noload_dv7_1
	sub	r0,r0
	shlr	r3
	rotcl	r0
	shlr	r2
	rotcl	r0
	shlr	r1
	rotcl	r0

	cmp/eq	#7,r0
	bf/s	noload_dv7_0
	mov.b	@(r0,r7),r4
	
	mov.b	@r8+,r4
noload_dv7_0:	

	sub	r0,r0
	shlr	r3
	rotcl	r0
	shlr	r2
	rotcl	r0
	shlr	r1
	rotcl	r0

	cmp/eq	#7,r0
	bf/s	noload_dv7_1
	mov.b	@(r0,r7),r5
	
	mov.b	@r8+,r5
noload_dv7_1:	
	shll8	r4
	extu.b	r5,r5
	or	r5,r4

	mov.w	r4,@r9
	add	#2,r9
	
	ENDM

_code_cel_deviant7
	mov.l	r10,@-r15
	sts.l	pr,@-r15

	mov.l	=webwidth,r1
	mov.l	@r1,r12

	mov.l	r8,r7				; r7 = look up table
	add	#7,r8

	mov	#8,r6				; y count
loop_dv7:
	mov.b	@r8+,r1				; planes
	mov.b	@r8+,r2
	mov.b	@r8+,r3

	rept	4
	MDEVIANT7
	endr
	sub	#8,r9
	add	r12,r9

	dt	r6
	bf	loop_dv7

	lds.l	@r15+,pr
	mov.l	@r15+,r10
	rts	     
	nop

	lits

		      
	.align	4
_code_cel_deviant3
	sts.l	pr,@-r15

	mov.l	=webwidth,r1
	mov.l	@r1,r12
	mov.l	=jt_deviant3,r7			; r7 = vector table

	mov.b	@r8+,r1				; r8 = source
	mov.b	@r8+,r2				; r8 = source
	mov.b	@r8+,r3				; r8 = source

	extu.b	r1,r1
	extu.b	r2,r2
	extu.b	r3,r3
	
	swap.b	r1,r0
	swap.w	r0,r4
	or	r4,r1

	swap.b	r2,r0
	swap.w	r0,r4
	or	r4,r2

	swap.b	r3,r0
	swap.w	r0,r4
	or	r4,r3

	REPT 8
	mov.b	@r8+,r4				
	mov.b	@r8+,r5
	extu.b	r4,r0				; r0 = mask
	shll2	r0
	mov.l	@(r0,r7),r4
	extu.b	r5,r0				; r0 = mask
	shll2	r0

	jsr	@r4	   			; r1 = call address
	mov.l	@(r0,r7),r5

	jsr	@r5	   			; r1 = call address
	mov.l	r0,@r9
	mov.l	r0,@(4,r9)

	add	r12,r9				; move to next line
	ENDR

	lds.l	@r15+,pr
	rts	     
	nop	                                       
		                             
	lits

	.align 4
	include shcode\deviant3.asm
	
	lits

; r5 is free

	.align	4
_code_cel_deviant1
	sts.l	pr,@-r15

	mov.l	=webwidth,r1
	mov.l	@r1,r6
	mov.l	=vect_table,r7			; r7 = vector table

	mov.b	@r8+,r4				; r8 = source
	;mov	#0,r4
	extu.b	r4,r0
	shll8	r4
	or	r0,r4

	REPT 8
	mov.b	@r8+,r1				; 
	extu.b	r1,r0				; pipeline collision
	shll2	r0
	mov.l	@(r0,r7),r1
	jsr	@r1	   			; r1 = call address
	add	#8,r9				; pre-decrement system
	add	r6,r9				; move to next line
	ENDR

	lds.l	@r15+,pr
	rts	     
	nop

	lits

MDeviant1 MACRO
	local fred1
	shlr	r5
	bf/s	fred1
	mov	#0,r0
	mov.b	@r8+,r1				; 
	extu.b	r1,r0				; r0 = mask
	shll2	r0
fred1:
	mov.l	@(r0,r7),r1
	jsr	@r1	   			; r1 = call address
	add	#8,r9				; pre-decrement system

	add	r6,r9				; move to next line
	ENDM
	
; r5 is used

	.align	4
_code_cel_delta_deviant1
	sts.l	pr,@-r15

	mov.l	=vect_table,r7			; r7 = vector table

	mov.b	@r8+,r4				; r8 = source
	;mov	#0,r4
	mov.b	@r8+,r5
	mov.l	=webwidth,r1
	mov.l	@r1,r6
	extu.b	r4,r0
	shll8	r4
	or	r0,r4

	REPT 8
	MDEVIANT1
	ENDR

	lds.l	@r15+,pr
	rts	     
	nop
	
	lits

	.align 4
;vect_table
	include shcode\deviant1.asm
	lits

	.align 4
jt_plane2
;	include plane2.asm

	lits

	.align 4

	lits

