;******************************************************************
;
;
; Scroll Screen Update Routines for 360X
;
;
;******************************************************************


	include	'shcode\sh.mac'

	opt	c+
	
	section	.text

	xdef	shCopyBlock
	xdef	shUpdateTopEdge
	xdef	shUpdateBottomEdge
	xdef	shUpdateLeftEdge
	xdef	shUpdateRightEdge

	xref	ScrollX
	xref	ScrollY
	xref	LastScrollX
	xref	LastScrollY
	xref	MapCellWidth
	xref	MapCellHeight


;******************************************************************
; memory defines
;******************************************************************

VDP2_VRAM_A0   	equ	$25e00000
VDP2_VRAM_A1   	equ	$25e20000
VDP2_VRAM_B0   	equ	$25e40000
VDP2_VRAM_B1   	equ	$25e60000
MapData	       	equ	$002e1004

BLOCKHEIGHT	equ	16
BLOCKWIDTH	equ	16

;******************************************************************
;		     r4		 r5
;void shCopyBlock(Uint8 *src,Uint8 *dest)
;

	;r0	=	pixel data
	;r1	=	y counter
	;r2	=	x counter
	;r3	=	NOT USED
	;r4	=	source
	;r5	=	destination
	;r6	=	offset to next line

shCopyBlock:

	mov.l	=$1f0,r6	;offset to next line
	mov	#15,r1		;y count

@y_loop:
	mov	#16,r2		;x count
	nop			;align memory load to lw boundary

@x_loop:
	mov.l	@r4+,r0		;get 4 pixels of data into r0
	sub	#4,r2		;decrement x count
	mov.l	r0,@r5		;move pixel data into vram
	cmp/pl	r2		;is x count above 0
	bt/s	@x_loop		;if yes then loop and increment dest register
	add	#4,r5

	add.l	r6,r5		;offset dest to next line (1024-16)
	cmp/pl	r1		; if y count above 0 then carry on looping
	bt/s	@y_loop 	
	sub	#1,r1		;sub from y count in delayed branch

	rts
	nop

	littab


;******************************************************************
;******************************************************************
;				r4	      r5
;void	shUpdateTopEdge(Uint32 mapindex,Uint32 vramindex)
; 
	;r3	=	loop counter
	;r4	=	parameter for shcopyblock
	;r5	=	      "	     "	 "
	;r9	=	*LowRam
	;r10	=	*MapData
	;r11	=	*shCopyBlock
	;r12	=	*VDP2_VRAM
	
shUpdateTopEdge:
	PUSH	r9
	PUSH	r10
	PUSH	r11
	PUSH	r12
	PUSH	pr

@setup:
	move.l	=MapData,r10
	move.l	=shCopyBlock,r11
	move.l	=VDP2_VRAM_A0,r12
	move.l	=$00200000,r9
	xor.l	r3,r3
	
@copy_loop:
	


	POP	pr
	POP	r12
	POP	r11
	POP	r10
	rts
	POP	r9

	littab



;******************************************************************
;******************************************************************
;		        		r4	      r5
;void	shUpdateBottomEdge(Uint32 mapindex,Uint32 vramindex)
; 

shUpdateBottomEdge:
	PUSH	pr

	POP	pr
	rts
	nop
	littab


;******************************************************************
;******************************************************************
;				r4	      r5
;void	shUpdateLeftEdge(Uint32 mapindex,Uint32 vramindex)
; 

shUpdateLeftEdge:
	PUSH	pr

	POP	pr
	rts
	nop
	littab


;******************************************************************
;******************************************************************
;				r4	      r5
;void	shUpdateRightEdge(Uint32 mapindex,Uint32 vramindex)
; 

shUpdateRightEdge:
	PUSH	pr

	POP	pr
	rts
	nop
	littab

;******************************************************************
;******************************************************************
