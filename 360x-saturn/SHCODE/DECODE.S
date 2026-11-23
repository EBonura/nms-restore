;
; Decode SSF data routine
;

	include	'shcode\sh.mac'

	opt	c+
	section	.text
	xdef	shDecodeSpriteFrame
	
	xref	shipoffx,shipoffy
	xref	GetSlot,Slots

	

;			    r4           r5         r6
;Sint16	shDecodeSpriteFrame(Uint16 tframe,Uint8 *src,Uint8 *dst)


shDecodeSpriteFrame:

	PUSH	pr		;save return address
	PUSH	r8
	PUSH	r9
	PUSH	r10
	PUSH	r11

	move.l	r6,r8		;save destination address
	move.l	r5,r1  		

	mov.b	@r5,r0		;get first byte
	shll8	r0		;<<8
	add	#1,r1
	mov.b	@r1,r0		;get second byte
				;now have numframes


	sub	#2,r0		;numframes -2

	cmp/hi	r4,r0		;see if frame requested actually exists
	bt	@get_offset
	move.l	#0,r4		;frame requested doesn't exist 
				;so get frame 0 instead

				

				;(src+4+(frames*4))	frame no in r4
@get_offset:
	move.l	r5,r1		;get source address
	add	#4,r1		;src + 4
	
	shll2	r4		;frames * 4

	add	r4,r1		;r1 now points to location of offset
		  		



	move.b	@r1+,r0		;read in the offset value
	shll8	r0		;one byte at a time
				;Have to do this as data is
				;not bound to longwords

	;xor.l	r3,r3
	move.b	@r1+,r3
	extu.b	r3,r3
	or	r3,r0
	shll8	r0

	;xor.l	r3,r3
	move.b	@r1+,r3
	extu.b	r3,r3
	or	r3,r0
	shll8	r0

	;xor.l	r3,r3
	move.b	@r1+,r3
	extu.b	r3,r3
	or	r3,r0

				
	add	r5,r0		;r5 has src
				;src+offset -> r0


	mov.b	@r0+,r1		;get offset x (Sint16)
	move	=shipoffx,r3
	shll8	r1
	mov.b	@r0+,r2
	or	r2,r1
	move.w	r1,@r3		;save offset data


	mov.b	@r0+,r1		;get offset y (Sint16)
	move	=shipoffy,r3
	shll8	r1
	mov.b	@r0+,r2
	or	r2,r1
	move.w	r1,@r3		;save offset data

	add	#2,r0		;skip past width data !

	
	mov.b	@r0+,r1		;get height (Uint16)
	move	=height,r3
	shll8	r1
	mov.b	@r0+,r2
	or	r2,r1
	move.w	r1,@r3

	mov.b	@r0+,r1		;get slotsize (Uint16)
	;move.l	=slotsize,r3	;get the data area ready
	shll8	r1
	mov.b	@r0+,r2
	or	r2,r1
	;move.w	r1,@r3		;r1 has slot size


	shll2	r1		;slotsize * 4
	
	move.l	=soffset,r3	;save our current location in the 
	move.l	r0,@r3		;image data

       


	;;;MAY RECODE THE GET_SLOT ROUTINES IN ASSEMBLY!!	
	move.l	=GetSlot,r3	;get a free slot in the buffer
	add	r1,r3		;Add the slot size to the array pointer
	move.l	=Slots,r4	;which holds the address of our functions
	move.l	@r1,r0		;in vdp1_2.c
	move	@r3,r3		;
	jsr	@r3		;Call GetSlotSize
	nop

	move.l	#-1,r3		;have we got a free slot?
	cmp/eq	r1,r3
	bt	@Done		;no slots found so exit

	move.l	r0,r7		;this is an offset! save it!
	
	move	r1,r0
	shlr16	r0		;get clear area from top word

	move.l	r8,r6		;get destination address
	;move	=$ffffffff,r2	;move endcode data into r2 R3 ALREADY IS -1!
	
	move.l	r6,r5		;dst->r5
	add	r7,r5		;add offset to the slot to r5
	add	r0,r5		;r5 += clear area length

	;move	#10,r3		;test data

	even

	;
	; CLEARS SLOT USING LONGS
	;
@clear_loop
	move.l	r3,@-r5		;move end code into buffer
	cmp/pz	r0		;have we finished
	bt/s	@clear_loop
	sub	#4,r0		;clear area -=1



	;Slot now ready for image data

	extu.w	r1,r1		;gets rid of the top word(!) r1=delta


	;now get the height and the width of the sprite
	;move.l	=width,r4	;not using width!
	;move.w	@r4,r4

	
	move.l	=height,r9
	move.w	@r9,r9

	move.l	=soffset,r10	;source data
	move	@r10,r10



	add	r7,r8		;add slot offset to GFXBuffer address
				;r8 now has exact address where data has to
				;be written to and r7 is now free
				

	move	r1,r7		;delta into r7






	;r0 = free!
	;r1 = free!
	;r2 = free!
	;r3 = free!
	;r4 = free!
	;r5 = free!
	;r6 = free!
	;r7 = (frame delta?)
	;r8 = gfx buffer
	;r9 = height
	;r10= src data pointer
	;r11= for dx

@y_loop:
	xor.l	r0,r0
	mov.b 	@r10+,r4	;get dx into r4
	shll8	r4
	mov.b	@r10+,r2
	extu.b	r2,r2
	or	r2,r4
	move	r4,r11		;dx now saved r11

	cmp/eq	r0,r4		;if dx as already zero then 
	bt	@no_dxloop	;don't bother with any clearing
	
	;;;;;;fill dx pixels in GFXBuffer with 0 

	move	r8,r0		;get gfx buffer address
	add	r4,r0	

	xor.l	r2,r2

	;
	; NOT CONVINCED THAT MEMSET IS EFFICIENT WAY OF SETTING DX PIXELS
	; AS THERE ARE SO FEW OF THEM
	;

	add	#1,r4
@dx_loop:
	mov.b	r2,@-r0		;move 0 into buffer
	sub	#1,r4		;subtract counter
	cmp/eq	r2,r4		;is dx = 0 ?
	bf	@dx_loop


@no_dxloop:
	;;;now copy the actual line data
	mov.b	@r10+,r6	;get lx into r6
	shll8	r6
	mov.b	@r10+,r5
	extu.b	r5,r5
	or	r5,r6		

				;NOTE!!!!!!!
				;r11 = dx

				;r0 has already got the correct
				;offset into GFXBuffer!


@copy_loop:
	move	r8,r4		;destination into r4
	move	r10,r5		;source into r5
				;lx already in r6
	move.l	=memcpy,r0	;do memcopy
	jsr	@r0
	nop

	add	r6,r10		;add lx to source!
	move	r7,r2		;delta to r2
	sub	r11,r2		;delta-dx
				;So:
	add	r2,r8		;dest += (delta-dx)
	xor.l	r2,r2
	

	;;;;;Done line copy

   	sub	#1,r9
   	cmp/eq	r9,r2
   	bf	@y_loop
	


@Done:		   
	POP	r11
	POP	r10
	POP	r9
	POP	r8
	POP	pr

	rts
	nop

	littab

soffset		ds.l	1
gbuffer		ds.l	1
src		ds.l	1
dst		ds.l	1
height		ds.w	1


	end







	if 0

@copy_loop:
	mov.b	@r10+,r1	;move 1 byte into buffer
	mov.b	r1,@r0
	add	#1,r0
	sub	#1,r4		;sub lx
	cmp/eq	r4,r2
	bf	@copy_loop

	endif
