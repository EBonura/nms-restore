	section	Code_32

	global	Decompress
;	global	Decompress288
;	global	Decompress320
	global	UpdateYline

	xdef		Decompress288

	global	copy_deviant3tab2cache

 	xref	buffer

ulw	macro
;	lwr	\1,\2
;	lwl	\1,3+\2

 	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	lbu	s4,3(a0)
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s3,16
	or	s0,s0,t0
	sll	t0,s4,24
	or	s0,s0,t0
	
	endm


copy_deviant3tab2cache:
	la	a0,dev3_vect
	li	a1,$1f800000
	li	a2,256
@loop	lw	v0,(a0)
	add	a0,a0,4
	sw	v0,(a1)
	sub	a2,a2,1
	bne	a2,zero,@loop
	add	a1,a1,4
	jr	ra
	nop

;
; Usage		Decompress ();

Decompress:
	addiu	sp,sp,-$108
	sw	ra,$84(sp)
	sw	fp,$90(sp)
	move	fp,sp

	opt at-
	sw	at,$00(fp)
	opt at+
	sw	v0,$04(fp)
	sw	v1,$08(fp)
	sw	a0,$0c(fp)
	sw	a1,$10(fp)
	sw	a2,$14(fp)
	sw	a3,$18(fp)
	sw	t0,$1c(fp)
	sw	t1,$20(fp)
	sw	t2,$24(fp)
	sw	t3,$28(fp)
	sw	t4,$2c(fp)
	sw	t5,$30(fp)
	sw	t6,$34(fp)
	sw	t7,$38(fp)
	sw	s0,$3c(fp)
	sw	s1,$40(fp)
	sw	s2,$44(fp)
	sw	s3,$48(fp)
	sw	s4,$4c(fp)
	sw	s5,$50(fp)
	sw	s6,$54(fp)
	sw	s7,$58(fp)
	sw	t8,$5c(fp)
	sw	t9,$60(fp)

	la	a2,jmp_table
	add	a0,a0,4
	
	move	a3,a1
;	ori	v0,zero,32/8		;176/8

@loop1	ori	v1,zero,640/8		;288/8

@loop2	lbu	s0,(a0)
	addiu	a0,a0,1

	add	s1,a2,s0
	lw	s0,(s1)
	addiu	v1,v1,-1
	jal	s0
	move	a1,a3
	
	bne	v1,zero,@loop2
	addiu	a3,a3,8

;	addiu	v0,v0,-1
;	bne	v0,zero,@loop1
;	addiu	a3,a3,640*7		;288*7

	opt at-
	lw	at,$00(fp)
	opt at+
	lw	v0,$04(fp)
	lw	v1,$08(fp)
	lw	a0,$0c(fp)
	lw	a1,$10(fp)
	lw	a2,$14(fp)
	lw	a3,$18(fp)
	lw	t0,$1c(fp)
	lw	t1,$20(fp)
	lw	t2,$24(fp)
	lw	t3,$28(fp)
	lw	t4,$2c(fp)
	lw	t5,$30(fp)
	lw	t6,$34(fp)
	lw	t7,$38(fp)
	lw	s0,$3c(fp)
	lw	s1,$40(fp)
	lw	s2,$44(fp)
	lw	s3,$48(fp)
	lw	s4,$4c(fp)
	lw	s5,$50(fp)
	lw	s6,$54(fp)
	lw	s7,$58(fp)
	lw	t8,$5c(fp)
	lw	t9,$60(fp)

exit	move	sp,fp
	lw	fp,$90(sp)
	lw	ra,$84(sp)
	addiu	sp,sp,$108

nochange:
	jr	ra
	nop

	section	Data_32

jmp_table:
	dw	zap		; 0
;	dw	deviant1
;	dw	deldeviant1
	dw	deviant3	; 4
	dw	deldeviant3f	; 8
	dw	deldeviant3fw	; 12
	dw	deldeviant3f4	; 16
	dw	deldeviant3f3	; 20
	dw	deldeviant3f2	; 24
;	dw	nochange

	section	Code_32

zap	lbu	t9,(a0)
	addiu	a0,a0,1
	sll	t7,t9,8
	or	t1,t7,t9
	sll	t7,t1,16
	or	t7,t7,t1
	move	t1,t7

	sw	t7,00(a1)
	sw	t7,04(a1)

	sw	t7,640(a1)
	sw	t7,644(a1)
	
	sw	t7,1280(a1)
	sw	t7,1284(a1)
	
	sw	t7,1920(a1)
	sw	t7,1924(a1)
	
	sw	t7,2560(a1)
	sw	t7,2564(a1)
	
	sw	t7,3200(a1)
	sw	t7,3204(a1)
	
	sw	t7,3840(a1)
	sw	t7,3844(a1)
	
	sw	t7,4480(a1)
	jr	ra
	sw	t7,4484(a1)

deviant3:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	addiu	a0,a0,3

	ori	t5,zero,8
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect

@outer	lbu	s0,0(a0)		;mask byte
	lbu	s1,1(a0)		;mask byte
	addiu	a0,a0,2
	sll	s0,s0,2
	sll	s1,s1,2

	add	s0,s0,t6
	add	s1,s1,t6
	lw	t3,(s0)
	lw	t4,(s1)
	jal	t3
	add	t5,t5,-1
	sw	s0,(a1)
	jal	t4
	nop
	sw	s0,4(a1)

	bne	t5,zero,@outer
	add	a1,a1,640		;288

	jr	t7
	nop


deldeviant3f:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)
	lbu	t6,4(a0)
	addiu	a0,a0,5

	sll	s4,s5,8
	or	s1,s4,s5
	sll	s4,s1,16
	or	s4,s1

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect

@outer	
	and	s1,t5,t8
	beq	s1,zero,@over1
	move	s0,s4

	lbu	s0,0(a0)		;mask byte
	addiu	a0,a0,1
	sll	s0,s0,2

	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3
@over1
	srl	t8,t8,1
	sw	s0,(a1)

	and	s1,t5,t8
	beq	s1,zero,@over2
	move	s0,s4

	lbu	s0,0(a0)		;mask byte
	addiu	a0,a0,1
	sll	s0,s0,2

	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3
@over2
	srl	t8,t8,1
	sw	s0,4(a1)
	bne	t8,zero,@outer
	add	a1,a1,640		;288

	jr	t7
	nop


; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t0 = temp
; t4 = temp

deldeviant3fw:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	addiu	a0,a0,4

	sll	s4,s5,8			; long mf0
	or	s1,s4,s5
	sll	s4,s1,16
	or	s4,s1

	ori	t8,zero,$80
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect

@outer	
	move	t1,s4
	and	s1,t5,t8
	beq	s1,zero,@over1
	move	s0,s4

	lbu	s0,0(a0)		;mask byte
	lbu	t0,1(a0)		;mask byte
	addiu	a0,a0,2

	sll	s0,s0,2
	sll	t0,t0,2
	add	s0,s0,t6
	add	t0,t0,t6

	lw	t3,(s0)
	lw	t4,(t0)
	jal	t3
	nop
	move	t1,s0			; saved
	jal	t4
	nop
@over1
	srl	t8,t8,1
	sw	t1,0(a1)
	sw	s0,4(a1)
	bne	t8,zero,@outer
	add	a1,a1,640		;288

	jr	t7
	nop

; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t4 = temp

deldeviant3f2:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	lbu	t6,4(a0)		;byte mask
	addiu	a0,a0,5

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
	move	t1,a1			;save dest address
;------------------------------
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect
	ori	t4,zero,8		;y strip count

@nexty	and	s1,t5,t8
	bne	s1,zero,@use1
	nop
	ulw	s0,(a0)
	addiu	a0,a0,4
	sw	s0,(a1)
	add	a1,a1,4
	srl	t8,t8,1
	and	s1,t5,t8
	bne	s1,zero,@use2
	nop
	ulw	s0,(a0)
	addiu	a0,a0,4
	j	@over2
	sw	s0,(a1)
@use1	lbu	s0,(a0)
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
	sw	s0,(a1)			;store word
	add	a1,a1,4
@over1	srl	t8,t8,1			;shift down mask
	and	s1,t5,t8
	bne	s1,zero,@use2
	nop
	ulw	s0,(a0)
	addiu	a0,a0,4
	j	@over2
	sw	s0,(a1)
@use2	lbu	s0,(a0)
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
	sw	s0,(a1)			;store word
@over2
	srl	t8,t8,1
	sub	t4,t4,1
	bne	t4,zero,@nexty
	add	a1,a1,640-4
;------------------------------
	jr	t7
	nop

; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t4 = temp

deldeviant3f3:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	lbu	t6,4(a0)		;byte mask
	addiu	a0,a0,5

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
	move	t1,a1			;save dest address
;------------------------------
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect
	ori	t4,zero,8		;y strip count
	lw	t3,(t6)			;default jt address
@outer1	
	and	s1,t5,t8
	beq	s1,zero,@over1
	addiu	t4,t4,-1		;dec y strip count

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop

@over1
	jal	t3			;call table
	srl	t8,t8,1			;shift down mask
	sw	s0,(a1)			;store word
	bne	t4,zero,@outer1		;finished stip
	add	a1,a1,640		
;------------------------------
	addiu	a1,t1,4	       		;restore dest addr+4
	lw	t3,(t6)			;default jt address
	ori	t4,zero,8
@outer2
	and	s1,t5,t8
	beq	s1,zero,@over2
	addiu	t4,t4,-1

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2
	add	s0,s0,t6
	lw	t3,(s0)
	nop
@over2
	jal	t3			;call table
	srl	t8,t8,1
	sw	s0,(a1)
	bne	t4,zero,@outer2
	add	a1,a1,640		
;------------------------------
	jr	t7
	nop

; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t4 = temp

deldeviant3f4:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	lbu	t6,4(a0)		;byte mask
	addiu	a0,a0,5

	sll	s4,s5,8			; long mf0
	or	s1,s4,s5
	sll	s4,s1,16
	or	s4,s1

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
	move	t1,a1			;save dest address
;------------------------------
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect
	move	s0,s4
	ori	t4,zero,8		;y strip count
@outer1	
	and	s1,t5,t8
	beq	s1,zero,@over1
	addiu	t4,t4,-1		;dec y strip count

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
@over1
	srl	t8,t8,1			;shift down mask
	sw	s0,(a1)			;store word
	bne	t4,zero,@outer1		;finished stip
	add	a1,a1,640
;------------------------------
	addiu	a1,t1,4	       		;restore dest addr+4
	move	s0,s4
	ori	t4,zero,8
@outer2
	and	s1,t5,t8
	beq	s1,zero,@over2
	addiu	t4,t4,-1

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
@over2
	srl	t8,t8,1
	sw	s0,(a1)
	bne	t4,zero,@outer2
	add	a1,a1,640
;------------------------------
	jr	t7
	nop





Decompress288:
	addiu	sp,sp,-$108
	sw	ra,$84(sp)
	sw	fp,$90(sp)
	move	fp,sp

	opt at-
	sw	at,$00(fp)
	opt at+
	sw	v0,$04(fp)
	sw	v1,$08(fp)
	sw	a0,$0c(fp)
	sw	a1,$10(fp)
	sw	a2,$14(fp)
	sw	a3,$18(fp)
	sw	t0,$1c(fp)
	sw	t1,$20(fp)
	sw	t2,$24(fp)
	sw	t3,$28(fp)
	sw	t4,$2c(fp)
	sw	t5,$30(fp)
	sw	t6,$34(fp)
	sw	t7,$38(fp)
	sw	s0,$3c(fp)
	sw	s1,$40(fp)
	sw	s2,$44(fp)
	sw	s3,$48(fp)
	sw	s4,$4c(fp)
	sw	s5,$50(fp)
	sw	s6,$54(fp)
	sw	s7,$58(fp)
	sw	t8,$5c(fp)
	sw	t9,$60(fp)

	la	a2,jmp_table288
	add	a0,a0,4
	
	move	a3,a1
	ori	v0,zero,184/8

@loop1	ori	v1,zero,288/8

@loop2	lbu	s0,(a0)
	addiu	a0,a0,1

	andi	s1,s0,3
	bne	s1,zero,@load_error
	nop
	bgtu	s0,24,@load_error
	nop

	add	s1,a2,s0
	lw	s0,(s1)
	addiu	v1,v1,-1
	jal	s0

	move	a1,a3
	
	bne	v1,zero,@loop2
	addiu	a3,a3,8

	addiu	v0,v0,-1
	bne	v0,zero,@loop1
	addiu	a3,a3,288*7
	j	@no_error
	move	s0,zero

@load_error
	ori	s0,zero,-1
@no_error
	sw	s0,$04(fp)

	opt at-
	lw	at,$00(fp)
	opt at+
	lw	v0,$04(fp)
	lw	v1,$08(fp)
	lw	a0,$0c(fp)
	lw	a1,$10(fp)
	lw	a2,$14(fp)
	lw	a3,$18(fp)
	lw	t0,$1c(fp)
	lw	t1,$20(fp)
	lw	t2,$24(fp)
	lw	t3,$28(fp)
	lw	t4,$2c(fp)
	lw	t5,$30(fp)
	lw	t6,$34(fp)
	lw	t7,$38(fp)
	lw	s0,$3c(fp)
	lw	s1,$40(fp)
	lw	s2,$44(fp)
	lw	s3,$48(fp)
	lw	s4,$4c(fp)
	lw	s5,$50(fp)
	lw	s6,$54(fp)
	lw	s7,$58(fp)
	lw	t8,$5c(fp)
	lw	t9,$60(fp)

	move	sp,fp
	lw	fp,$90(sp)
	lw	ra,$84(sp)
	addiu	sp,sp,$108
	jr	ra
	nop

	section	Data_32

jmp_table288:
	dw	zap288
;	dw	deviant1288
;	dw	deldeviant1288
	dw	deviant3288
	dw	deldeviant3f288
	dw	deldeviant3fw288
	dw	deldeviant3f4288
	dw	deldeviant3f3288
	dw	deldeviant3f2288
;	dw	nochange288

	section	Code_32

zap288	lbu	t9,(a0)
	addiu	a0,a0,1
	sll	t7,t9,8
	or	t1,t7,t9
	sll	t7,t1,16
	or	t7,t7,t1
	move	t1,t7

	sw	t7,00(a1)
	sw	t7,04(a1)

	sw	t7,288(a1)
	sw	t7,292(a1)
	
	sw	t7,576(a1)
	sw	t7,580(a1)
	
	sw	t7,864(a1)
	sw	t7,868(a1)
	
	sw	t7,1152(a1)
	sw	t7,1156(a1)
	
	sw	t7,1440(a1)
	sw	t7,1444(a1)
	
	sw	t7,1728(a1)
	sw	t7,1732(a1)
	
	sw	t7,2016(a1)
	jr	ra
	sw	t7,2020(a1)

deviant3288:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	addiu	a0,a0,3

	ori	t5,zero,8
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect

@outer	lbu	s0,0(a0)		;mask byte
	lbu	s1,1(a0)		;mask byte
	addiu	a0,a0,2
	sll	s0,s0,2
	sll	s1,s1,2

	add	s0,s0,t6
	add	s1,s1,t6
	lw	t3,(s0)
	lw	t4,(s1)
	jal	t3
	add	t5,t5,-1
	sw	s0,(a1)
	jal	t4
	nop
	sw	s0,4(a1)

	bne	t5,zero,@outer
	add	a1,a1,288

	jr	t7
	nop


deldeviant3f288:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)
	lbu	t6,4(a0)
	addiu	a0,a0,5

	sll	s4,s5,8
	or	s1,s4,s5
	sll	s4,s1,16
	or	s4,s1

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect

@outer	
	and	s1,t5,t8
	beq	s1,zero,@over1
	move	s0,s4

	lbu	s0,0(a0)		;mask byte
	addiu	a0,a0,1
	sll	s0,s0,2

	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3
@over1
	srl	t8,t8,1
	sw	s0,(a1)

	and	s1,t5,t8
	beq	s1,zero,@over2
	move	s0,s4

	lbu	s0,0(a0)		;mask byte
	addiu	a0,a0,1
	sll	s0,s0,2

	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3
@over2
	srl	t8,t8,1
	sw	s0,4(a1)
	bne	t8,zero,@outer
	add	a1,a1,288

	jr	t7
	nop


; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t0 = temp
; t4 = temp

deldeviant3fw288:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	addiu	a0,a0,4

	sll	s4,s5,8			; long mf0
	or	s1,s4,s5
	sll	s4,s1,16
	or	s4,s1

	ori	t8,zero,$80
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect

@outer	
	move	t1,s4
	and	s1,t5,t8
	beq	s1,zero,@over1
	move	s0,s4

	lbu	s0,0(a0)		;mask byte
	lbu	t0,1(a0)		;mask byte
	addiu	a0,a0,2

	sll	s0,s0,2
	sll	t0,t0,2
	add	s0,s0,t6
	add	t0,t0,t6

	lw	t3,(s0)
	lw	t4,(t0)
	jal	t3
	nop
	move	t1,s0			; saved
	jal	t4
	nop
@over1
	srl	t8,t8,1
	sw	t1,0(a1)
	sw	s0,4(a1)
	bne	t8,zero,@outer
	add	a1,a1,288

	jr	t7
	nop

; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t4 = temp

deldeviant3f2288:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	lbu	t6,4(a0)		;byte mask
	addiu	a0,a0,5

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
	move	t1,a1			;save dest address
;------------------------------
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect
	ori	t4,zero,8		;y strip count

@nexty	and	s1,t5,t8
	bne	s1,zero,@use1
	nop
	ulw	s0,(a0)
	addiu	a0,a0,4
	sw	s0,(a1)
	add	a1,a1,4
	srl	t8,t8,1
	and	s1,t5,t8
	bne	s1,zero,@use2
	nop
	ulw	s0,(a0)
	addiu	a0,a0,4
	j	@over2
	sw	s0,(a1)
@use1	lbu	s0,(a0)
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
	sw	s0,(a1)			;store word
	add	a1,a1,4
@over1	srl	t8,t8,1			;shift down mask
	and	s1,t5,t8
	bne	s1,zero,@use2
	nop
	ulw	s0,(a0)
	addiu	a0,a0,4
	j	@over2
	sw	s0,(a1)
@use2	lbu	s0,(a0)
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
	sw	s0,(a1)			;store word
@over2
	srl	t8,t8,1
	sub	t4,t4,1
	bne	t4,zero,@nexty
	add	a1,a1,288-4
;------------------------------
	jr	t7
	nop

; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t4 = temp

deldeviant3f3288:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	lbu	t6,4(a0)		;byte mask
	addiu	a0,a0,5

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
	move	t1,a1			;save dest address
;------------------------------
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect
	ori	t4,zero,8		;y strip count
	lw	t3,(t6)			;default jt address
@outer1	
	and	s1,t5,t8
	beq	s1,zero,@over1
	addiu	t4,t4,-1		;dec y strip count

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop

@over1
	jal	t3			;call table
	srl	t8,t8,1			;shift down mask
	sw	s0,(a1)			;store word
	bne	t4,zero,@outer1		;finished stip
	add	a1,a1,288
;------------------------------
	addiu	a1,t1,4	       		;restore dest addr+4
	lw	t3,(t6)			;default jt address
	ori	t4,zero,8
@outer2
	and	s1,t5,t8
	beq	s1,zero,@over2
	addiu	t4,t4,-1

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2
	add	s0,s0,t6
	lw	t3,(s0)
	nop
@over2
	jal	t3			;call table
	srl	t8,t8,1
	sw	s0,(a1)
	bne	t4,zero,@outer2
	add	a1,a1,288		
;------------------------------
	jr	t7
	nop

; s0 = Temp
; s1 = Temp
; s2 = Temp
; s3 = Temp
; s4 = Long Most Freq 0
; s5 = Byte Most Freq 0
; s6 = Byte Most Freq 1
; s7 = Byte Most Freq 2
; t7 = Return Address
; t6 = Vector
; t5 = Mask
; t4 = temp

deldeviant3f4288:
	move	t7,ra
	lbu	s5,0(a0)		;most freq colour
	lbu	s6,1(a0)		;most freq colour
	lbu	s7,2(a0)		;most freq colour
	lbu	t5,3(a0)		;byte mask
	lbu	t6,4(a0)		;byte mask
	addiu	a0,a0,5

	sll	s4,s5,8			; long mf0
	or	s1,s4,s5
	sll	s4,s1,16
	or	s4,s1

	sll	t6,t6,8
	or	t5,t5,t6		;word mask

	ori	t8,zero,$8000
	move	t1,a1			;save dest address
;------------------------------
;	la	t6,dev3_vect
	li	t6,$1f800000	;dev3_vect
	move	s0,s4
	ori	t4,zero,8		;y strip count
@outer1	
	and	s1,t5,t8
	beq	s1,zero,@over1
	addiu	t4,t4,-1		;dec y strip count

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2			
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
@over1
	srl	t8,t8,1			;shift down mask
	sw	s0,(a1)			;store word
	bne	t4,zero,@outer1		;finished stip
	add	a1,a1,288
;------------------------------
	addiu	a1,t1,4	       		;restore dest addr+4
	move	s0,s4
	ori	t4,zero,8
@outer2
	and	s1,t5,t8
	beq	s1,zero,@over2
	addiu	t4,t4,-1

	lbu	s0,0(a0)		;get entry of table
	addiu	a0,a0,1
	sll	s0,s0,2
	add	s0,s0,t6
	lw	t3,(s0)
	nop
	jal	t3			;call table
	nop
@over2
	srl	t8,t8,1
	sw	s0,(a1)
	bne	t4,zero,@outer2
	add	a1,a1,288
;------------------------------
	jr	t7
	nop

	include	"deviant3.s"
;dev_vect:
;	include	"dev1.s"
;

;call with (xpos,ypos,address source,address destination);

UpdateYline:
	add	sp,sp,-$20

	sw	t0,00(sp)
	sw	t1,04(sp)
	sw	t2,08(sp)
	sw	t3,12(sp)
	sw	t4,16(sp)
	sw	t5,20(sp)
	sw	t6,24(sp)
	sw	t7,28(sp)

	sll	v0,a1,6
	sll	v1,v0,1
	add	v0,v0,v1	;v0 = y*192
	sll	v1,v0,2		;v1 = y*768
	add	a1,v0,v1	;a1 = y*960
	add	a1,a1,a0	;a1 = (y*960)+x
	add	a1,a1,a2	;a1 = source addr;
	li	a0,240		;y counter
@loop	lw	t0,00(a1)
	lw	t1,04(a1)
	lw	t2,08(a1)
	lw	t3,12(a1)
	lw	t4,16(a1)
	lw	t5,20(a1)
	lw	t6,24(a1)
	lw	t7,28(a1)
	sw	t0,00(a3)
	sw	t1,04(a3)
	sw	t2,08(a3)
	sw	t3,12(a3)
	sw	t4,16(a3)
	sw	t5,20(a3)
	sw	t6,24(a3)
	sw	t7,28(a3)
	add	a1,a1,960
	add	a0,a0,-1
	bne	a0,zero,@loop
	add	a3,a3,32

	lw	t0,00(sp)
	lw	t1,04(sp)
	lw	t2,08(sp)
	lw	t3,12(sp)
	lw	t4,16(sp)
	lw	t5,20(sp)
	lw	t6,24(sp)
	lw	t7,28(sp)
	add	sp,sp,$20

	jr	ra
	nop

