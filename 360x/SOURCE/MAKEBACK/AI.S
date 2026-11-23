declare:	obj_x
declare:	obj_y
declare:	obj_temp1
declare:	obj_temp2
declare:	obj_temp3
declare:	obj_temp4
declare:	obj_temp5
declare:	obj_temp6
declare:	obj_temp7
declare:	obj_temp8
declare:	obj_temp9
declare:	obj_temp10
declare:	obj_temp11
declare:	obj_temp12
declare:	obj_temp13
declare:	obj_temp14
declare:	obj_temp15
declare:	obj_temp16
declare:	obj_children
declare:	obj_angle

name:	ai_null
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; The New Man
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Set animation speed according to acceleration...

equate:	new_man_x	obj_temp1
equate:	new_man_y 	obj_temp2
equate:	new_man_dx	obj_temp3
equate:	new_man_dy	obj_temp4
equate: MAX_SCAN	16
equate: MANS_HEIGHT	8

name:	ai_new_man
	thread		ai_cycle

	movei		1,new_man_dx
	setanimright
@next
	stop
	movei		MAX_SCAN,new_man_dy
	movev		obj_x,new_man_x
	movev		obj_y,new_man_y
	addi		MANS_HEIGHT,new_man_y
	addv		new_man_dx,new_man_x
	find_floor	new_man_x,new_man_y,new_man_dy
	iflti		0,new_man_dy,@going_up
@going_down
	ifgei		MAX_SCAN-1,new_man_dy,@change_direction
	addv		new_man_dx,obj_x
	addv		new_man_dy,obj_y
	update_xy
	animspeed	4
	jmp		@next
@going_up
	iflei		-MAX_SCAN,new_man_dy,@change_direction
	addv		new_man_dx,obj_x
	addv		new_man_dy,obj_y
	update_xy
	animspeed	6
	jmp		@next
@change_direction
	neg		new_man_dx
	ifgti		0,new_man_dx,@face_right
	setanimleft
	jmp		@next
@face_right
	setanimright
	jmp		@next

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Mines
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; This is bollox...
;
; Lets do some AI for the mines
; Plan a trip and take it...
; T-Point finder
; Scan points on radius left and right for large gap...
; Find extremity
; Same idea as little man
; Perpendicular angles to craft
; 

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Surface Hugger
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

equate:	hug_leftx	obj_temp1
equate:	hug_rightx	obj_temp2
equate:	hug_width	obj_temp3
equate:	hug_dx		obj_temp4
equate:	hug_dy		obj_temp5

name:	ai_surface_hug
;@still
;	stop
;	inrange		256*256,@wait
;	jmp		@still

	movei		1,hug_dy
@again
	stop
	stop
	stop
	stop
	scan		obj_x,obj_y,hug_leftx,hug_rightx
	ifltv		hug_leftx,obj_x,@reverse_direction
	movev		hug_rightx,hug_width
	subv		hug_leftx,hug_width
	add_line	hug_leftx,obj_y,hug_width
	addv		hug_dy,obj_y

	subv		obj_x,hug_leftx	
	neg		hug_leftx
	subv		obj_x,hug_rightx	

	movei		1,hug_dx
	ifgtv		hug_leftx,hug_rightx,@lefty
	neg		hug_dx
@lefty
	addv		hug_dx,obj_x

	update_xy	
	jmp		@again
@reverse_direction
	neg		hug_dy
	addv		hug_dy,obj_y
	update_xy	
	jmp		@again

	movev			obj_x,hug_leftx
	addi			1,hug_leftx
	movev			obj_x,hug_rightx
	addi			1,hug_rightx
@scan_right
	stop
	point_collision		hug_rightx,obj_y,@right_wall
	movev			hug_rightx,hug_width
	subv			hug_leftx,hug_width
	add_line		hug_leftx,obj_y,hug_width
	addi			1,hug_rightx
	jmp			@scan_right
@right_wall

@scan_left
	stop
	point_collision		hug_leftx,obj_y,@left_wall
	movev			hug_rightx,hug_width
	subv			hug_leftx,hug_width
	add_line		hug_leftx,obj_y,hug_width
	subi			1,hug_leftx
	jmp			@scan_left
@left_wall

@scan_up_left


	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Lazer
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Waits for you to come into range
; Wavers about a bit
; Opens and shoots...

name:	ai_lazer
:spagetti_lazer
	on_hit		ai_lazer_hit
@still
	stop
	inrange		256*256,@wait
	jmp		@still
@wait
	animspeed	4
	movei		0,obj_temp1

	movernd		6,obj_temp2
	addi		2,obj_temp2
	setanimrange	obj_temp1,obj_temp2
	animplayforwardonce
	animplaybackonce
	inrange		128*128,@start
	jmp		@wait
@start
	animspeed	3
	setanimnormal
	animplayforwardonce
	
	movei		320,obj_temp1
@fire
	stop
	movev		obj_y,obj_temp2
	subi		2,obj_temp2
	add_line	obj_x,obj_temp2,obj_temp1
	addi		1,obj_temp2
	add_line	obj_x,obj_temp2,obj_temp1
	addi		1,obj_temp2
	add_line	obj_x,obj_temp2,obj_temp1
	addi		1,obj_temp2
	add_line	obj_x,obj_temp2,obj_temp1
	addi		1,obj_temp2
	add_line	obj_x,obj_temp2,obj_temp1
	addi		2,obj_temp2

	ifgti		0,obj_temp1,@fire

	animspeed	2
	animplaybackonce
	jmp		@still

name:	ai_lazer_hit
	animspeed	1
	animplaybackonce
	movei		32,obj_temp1
	pause		obj_temp1
	jmp		:spagetti_lazer

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Lazer
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Don't like this at all it looks shit
; Lets just try a straight line then

equate:	lazer_x		obj_temp1
equate:	lazer_y		obj_temp2

equate:	lazer_xf	obj_temp3
equate:	lazer_yf	obj_temp4

equate:	lazer_dx	obj_temp5
equate:	lazer_dy	obj_temp6

equate:	lazer_r1	obj_temp7
equate:	lazer_r2	obj_temp8

equate:	lazer_r1f	obj_temp9
equate:	lazer_r2f	obj_temp10

equate:	lazer_count	obj_temp11

name:	ai_shite_lazer
@wait
	stop
	inrange		256*256,@ready
	jmp		@wait

@ready
	animplayforwardonce

	movev		obj_x,lazer_x
	movev		obj_y,lazer_y
	shl		16,lazer_x,lazer_xf
	shl		16,lazer_y,lazer_yf

	movei		4,lazer_r1
	movei		1,lazer_r2

	shl		16,lazer_r1,lazer_r1f
	shl		16,lazer_r2,lazer_r2f
@expand
	stop
	shr		16,lazer_r1f,lazer_r1
	shr		16,lazer_r2f,lazer_r2
	add_polo	lazer_r1,lazer_r2,lazer_x,lazer_y
	addi		16384,lazer_r1f
	addi		8192+4096,lazer_r2f
	iflti		32,lazer_r1,@expand

	movei		128,lazer_count
	movei		32768,lazer_dx
	movei		0,lazer_dy
@loop
	stop	
	shr		16,lazer_xf,lazer_x
	shr		16,lazer_yf,lazer_y
	add_polo	lazer_r1,lazer_r2,lazer_x,lazer_y
	addv		lazer_dx,lazer_xf
	addv		lazer_dy,lazer_yf
	addi		4096,lazer_dx
	subi		1,lazer_count
	ifgti		0,lazer_count,@loop

	animplaybackonce
	jmp		@wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Soft rock
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_soft_rock
	on_hit		ai_rock_flash
	wait

name:	ai_rock_flash
	movei		4,obj_temp1
	movei		1,obj_temp2

@loop1
	movev		obj_temp1,obj_temp3
	visible_off
	pause		obj_temp3
	visible_on
	movev		obj_temp2,obj_temp3
	pause		obj_temp3

	addi		1,obj_temp2
	iflti		4,obj_temp2,@loop1


@loop2
	movev		obj_temp1,obj_temp3
	visible_off
	pause		obj_temp3
	visible_on
	movev		obj_temp2,obj_temp3
	pause		obj_temp3

	subi		1,obj_temp1
	ifnei		0,obj_temp1,@loop2

	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI 4 BUG
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

equate:	bugger_x	obj_temp1
equate:	bugger_y	obj_temp2
equate:	bugger_dx	obj_temp3
equate:	bugger_dy	obj_temp4
equate:	bugger_ddx	obj_temp5
equate:	bugger_ddy	obj_temp6

equate:	temp_x		obj_temp1
equate:	temp_y		obj_temp2
equate:	temp_x1		obj_temp3
equate:	temp_x2		obj_temp4
equate:	temp_y1		obj_temp5
equate:	temp_y2		obj_temp6
equate:	temp_xdx	obj_temp7
equate:	temp_ydy	obj_temp8
equate:	temp_count	obj_temp9

name:	ai_bugger

	; Find Floor

	call	@find_floor

@again
	movei		32,obj_temp1
	pause		obj_temp1

	; On Floor

	movei		0,obj_temp1
	movei		5,obj_temp2
	setanimrange	obj_temp1,obj_temp2

	movei		8,obj_temp3
@faster	
	animspeedv	obj_temp3
	set_frame	obj_temp1
	animplayforwardonce
	subi		1,obj_temp3
	ifgei		3,obj_temp3,@faster

	; Take Off

	movei		6,obj_temp1
	movei		23,obj_temp2
	set_frame	obj_temp1
	setanimrange	obj_temp1,obj_temp2
	animplayforwardonce

	movei		24,obj_temp1
	movei		29,obj_temp2
	set_frame	obj_temp1
	setanimrange	obj_temp1,obj_temp2
	thread		ai_cycle

	shl		16,obj_x,bugger_x
	shl		16,obj_y,bugger_y
	movei		0,bugger_dx
	movei		-65536,bugger_dy
	movei		0,bugger_ddx
	movei		4096,bugger_ddy
@loop
	stop
	shr		16,bugger_x,obj_x
	shr		16,bugger_y,obj_y
	update_xy
	addv		bugger_dx,bugger_x
	addv		bugger_dy,bugger_y
	addv		bugger_ddx,bugger_dx
	addv		bugger_ddy,bugger_dy
	iflti		0,bugger_dy,@loop
	point_collision	obj_x,obj_y,@done
	jmp		@loop
@done
	thread		ai_none
	jmp		@again

@find_floor
	stop
	get_sprites_coords	temp_x1,temp_y1,temp_x2,temp_y2

	output_var		0,temp_x1
	output_var		1,temp_y1
	output_var		2,temp_x2
	output_var		3,temp_y2

	movev			temp_x2,temp_xdx
	subv			temp_x1,temp_xdx
	movev			temp_y2,temp_ydy
	subv			temp_y1,temp_ydy

	output_var		4,temp_xdx
	output_var		5,temp_ydy

	divi			2,temp_xdx,temp_xdx

	movei			0,temp_count
@down_a_bit
	stop
	addi			1,temp_count
	output_var		6,temp_count
	output_var		7,temp_y
	movev			obj_x,temp_x
	movev			obj_y,temp_y

	addv			temp_xdx,temp_x
	addv			temp_ydy,temp_y
	
	point_collision		temp_x,temp_y,@bad

	addi			1,obj_y
	update_xy
	jmp			@down_a_bit
@bad
	return			; got it


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI 4 Character type thingies (Crashes)
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

equate:	lem_x		obj_temp1
equate:	lem_y		obj_temp2

equate:	lem_x1		obj_temp1
equate:	lem_x2		obj_temp2
equate:	lem_y1		obj_temp3
equate:	lem_y2		obj_temp4

equate:	lem_xdx		obj_temp5
equate:	lem_ydy		obj_temp6

equate:	lem_count	obj_temp7
equate:	dy		obj_temp7
equate:	lem_temp	obj_temp8

equate:	lem_x_move	obj_temp9

equate: tx		obj_temp10
equate: ty		obj_temp11

equate: px		obj_temp12
equate: py		obj_temp13

equate: fdy		obj_temp15

equate: MAN_ANIM_DELAY	1
equate: MAN_DELAY	32

	wait

name:	ai_lem

	on_hit	ai_survrs_die
;	setanimnormal
	setanimright

;	animspeed	MAN_ANIM_DELAY

	call			@find_floor

; -----------------------------------------

	movei			1,lem_x_move
	movei			20,obj_temp16


@lets_go

	addv			lem_x_move,obj_x ;move horizontally
	frameinc
	movei			0,lem_count	 ;reset counter
	movev			obj_x,lem_x	 ;get co-ordinates of man
	movev			obj_y,lem_y
	addv			lem_xdx,lem_x	 ;add width/2 to x
	addv			lem_ydy,lem_y	 ;add height to man


	call			:find_dy

@do_move

	;add_particle		lem_x,lem_y,0,0,0,0

	;;; dy is now our dist up or down

	iflti			8,dy,@not_big_down

	;climb or drop is too big so wait and wave,reverse direction

	subv			lem_x_move,obj_x
	neg			lem_x_move

	;output_var		0,lem_x_move
	output_var		0,dy
	stop

	movei			MAN_DELAY,lem_temp
	pause			lem_temp

	call	@find_floor
	setanimnormal
	animplayforwardonce

	call			:set_direction

	jmp			@lets_go
	
@not_big_down

	ifgti			-8,dy,@not_big_up

	;climb or drop is too big so wait and wave,reverse direction
	subv			lem_x_move,obj_x
	neg			lem_x_move

	;output_var		0,lem_x_move
	output_var		0,dy
	stop

	movei			MAN_DELAY,lem_temp
	pause			lem_temp

	call	@find_floor
	setanimnormal
	animplayforwardonce

	call			:set_direction

	jmp			@lets_go

@not_big_up

	;ifgei			6,dy,:steep_climb

	addv			dy,obj_y
	update_xy

	movei			4,lem_temp
	pause			lem_temp

	call	@find_floor

	jmp			@lets_go


; -----------------------------------------
;routine to set  direction
; -----------------------------------------

:set_direction

	ifeqi			-1,lem_x_move,@set_left
	setanimright
	get_ret		obj_temp16
	call		@find_floor
	set_ret		obj_temp16

	jmp		@l2
	
	@set_left
	setanimleft
	get_ret		obj_temp16
	call		@find_floor
	set_ret		obj_temp16

	@l2

	return


; -----------------------------------------
;routine to move man by dy in steps n
; -----------------------------------------

:steep_climb
	
	;do some climbing frames later
	
	;distance to climb

	;movev			dy,lem_temp
	@get_increment
	 iflei			-1,dy,@negative
	 movei			32768,fdy
	 jmp			@climb_loop

	@negative
	  movei			-32768,fdy
	 
	  shl 			16,dy,lem_temp
	  shl			16,obj_y,ty
	  		
	  ;movev		obj_y,ty
	
     @climb_loop

	  stop
	  addv			fdy,obj_y
	  subi			32768,lem_temp

	  shr			16,ty,obj_y
	  update_xy
	  frameinc

	 ifgei			0,lem_temp,@climb_loop

	return


; -----------------------------------------
;routine to find dy from lem_x,lem_y
; -----------------------------------------

:find_dy

; check down first

@go_down
	
	;if we are already in contact with the floor we need to move up
	point_collision		lem_x,lem_y,@go_up

	;above floor so continue search down
	addi			1,lem_count
	addi			1,lem_y
	
	;limit the length of the search
	iflti			16,lem_count,@go_down
	


@go_up
	;limit the length of the search
	iflei			-16,lem_count,@end_find_dy
	
	subi			1,lem_count
	subi			1,lem_y
	
	;if in contact with floor continue to search up
	point_collision		lem_x,lem_y,@go_up


@end_find_dy
	
	return

; -----------------------------------------

; -----------------------------------------

;
; routine carried out first to align man to landscape
;

@find_floor

	;stop
	get_sprites_coords	lem_x1,lem_y1,lem_x2,lem_y2

	;work out bottom middle of sprite

	movev			lem_x2,lem_xdx
	subv			lem_x1,lem_xdx
	movev			lem_y2,lem_ydy
	subv			lem_y1,lem_ydy

	divi			2,lem_xdx,lem_xdx ;width /2 

	movei			0,lem_count ;reset counter

@up_a_bit
	stop
	addi			1,lem_count

	movev			obj_x,lem_x
	movev			obj_y,lem_y

	addv			lem_xdx,lem_x
	addv			lem_ydy,lem_y	;offset collision Cx point 
						;to bottom of mans feet
	point_collision		lem_x,lem_y,@bad

	;stop
	return			; feet above floor so go back

@bad
	subi			1,obj_y
	update_xy
	jmp			@up_a_bit



;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

;
;	iflti			-1,lem_x_move,@going_left
;	setanimright
;	jmp			@done_direction
;@going_left
;	setanimleft
;
;@done_direction
;

;	movev			obj_x,tx
;	movev			obj_y,ty
;
;	shl			16,tx,tx
;	shl			16,ty,ty
;
;	addv			lem_x_move,tx    ;move horizontally
;	movei			0,lem_count	 ;reset counter
;
;	shl			16,obj_x,lem_x	 ;get co-ordinates of man
;	shl			16,obj_x,lem_y
;
;;	movev			obj_x,lem_x	 ;get co-ordinates of man
;;	movev			obj_y,lem_y
;
;	shl			16,lem_xdx,lem_xdx
;	shl			16,lem_ydy,lem_ydy
;
;	addv			lem_xdx,lem_x	 ;add width/2 to x
;	addv			lem_ydy,lem_y	 ;add height to man
;
;@go_down
;	stop
;	output_var		6,lem_count
;	shr			16,lem_x,px
;	shr			16,lem_y,py
;	
;	add_particle		px,py,0,0,0,0
;
;	point_collision		px,py,@go_up
;	
;	addi			1,lem_count
;	addi			32768,lem_y
;	iflti			16,lem_count,@go_down
;
;@go_up
;	stop
;	iflei			-16,lem_count,@avoid
;	subi			1,lem_count
;	subi			32768,lem_y
;	subi			1,py
;
;	add_particle		px,py,0,0,0,0
;	
;	point_collision		px,py,@go_up
;
;
;@avoid
;	; lem_count is now our dist up or down...
;
;	output_var		0,lem_count
;
;	iflti			8,lem_count,@not_big_down
;
;	subv			lem_x_move,lem_x
;	neg			lem_x_move
;
;	output_var		0,lem_count
;	output_var		1,lem_count
;	output_var		2,lem_count
;	output_var		3,lem_count
;
;	movei			32,lem_temp
;	pause			lem_temp
;
;	animplayforwardonce
;
;	setanimnormal
;
;	jmp			@lets_go
;	
;@not_big_down
;	ifgti			-8,lem_count,@not_big_up
;	subv			lem_x_move,lem_x
;	neg			lem_x_move
;
;	output_var		4,lem_count
;	output_var		5,lem_count
;	output_var		6,lem_count
;	output_var		7,lem_count
;
;	movei			32,lem_temp
;	pause			lem_temp
;
;	animplayforwardonce
;	setanimnormal
;
;	jmp			@lets_go
;
;@not_big_up
;
;	addv			lem_count,lem_y
;
;	shr			16,lem_x,obj_x
;	shr			16,lem_y,obj_y
;
;	update_xy
;
;	movei			4,lem_temp
;	pause			lem_temp
;
;	jmp			@lets_go
;

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Flock
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
equate:	flock_x		obj_temp1
equate:	flock_y		obj_temp2
equate:	flock_dx	obj_temp3
equate:	flock_dy	obj_temp4
equate:	flock_ddx	obj_temp5
equate:	flock_ddy	obj_temp6
equate: flock_steps	obj_temp7
equate: flock_to_id	obj_temp8
equate: flock_vel	obj_temp9

name:	ai_flock
;	thread		ai_cycle
	on_hit		ai_spermicide

	next_node	flock_dx,flock_dy,flock_steps
	ifeqi		-1,flock_dx,@not_leader

	adopt_inrange_child	128*128

	; I am the leader

	stop
@leader_loop
	call		@make_dds
	shl		16,obj_x,flock_x
	shl		16,obj_y,flock_y
@leader_move
	stop
	addv		flock_dx,flock_x
	addv		flock_dy,flock_y
	addv		flock_ddx,flock_dx
	addv		flock_ddy,flock_dy
	shr		16,flock_x,obj_x
	shr		16,flock_y,obj_y
	update_xy

	subi		1,flock_steps
	ifgti		0,flock_steps,@leader_move

	next_node	flock_dx,flock_dy,flock_steps
	jmp		@leader_loop	

	; Someone else is leader

@not_leader
	stop

	get_parent_id	flock_to_id
	get_angle_to_id	flock_to_id

;	movei		256+512,obj_angle

	divi		16,obj_angle,flock_vel
	addi		32,flock_vel

	iflti		64,flock_vel,@not_too_big
	subi		64,flock_vel
@not_too_big
	ifgei		0,flock_vel,@not_too_small
	addi		64,flock_vel
@not_too_small
	movei		63,flock_dx
	subv		flock_vel,flock_dx

	set_frame	flock_dx

	movernd		32768,flock_vel
	addi		40000,flock_vel
	
	set_vel		flock_vel,flock_dx,flock_dy
	shl		16,obj_x,flock_x
	shl		16,obj_y,flock_y
	movernd		32,flock_steps
	addi		48,flock_steps

	call		@make_dds
@fly_baby
	stop
	addv		flock_dx,flock_x
	addv		flock_dy,flock_y
	addv		flock_ddx,flock_dx
	addv		flock_ddy,flock_dy
	shr		16,flock_x,obj_x
	shr		16,flock_y,obj_y
	update_xy
	subi		1,flock_steps
	ifgti		0,flock_steps,@fly_baby

	ifeqi		0,obj_children,@not_leader

	jmp		@leader_loop

	; This bodges the inertia
	; add half of its distance to its velocity
	; divide its velocity by its steps
	; This is the deceleration

@make_dds
	movev		flock_dx,flock_ddx	       
	movev		flock_dy,flock_ddy
	divv		flock_steps,flock_ddx,flock_ddx
	divv		flock_steps,flock_ddy,flock_ddy
	neg		flock_ddx
	neg		flock_ddy

	divi		2,flock_dx,flock_vel
	addv		flock_vel,flock_dx
	divi		2,flock_dy,flock_vel
	addv		flock_vel,flock_dy
	return


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Sperm
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Need to sort out abdication for pather
; Needs to inherit parents path
; Could react to the death of a member
equate:	sperm_x		obj_temp1
equate:	sperm_y		obj_temp2
equate:	sperm_dx	obj_temp3
equate:	sperm_dy	obj_temp4
equate:	sperm_ddx	obj_temp5
equate:	sperm_ddy	obj_temp6
equate: sperm_steps	obj_temp7
equate: sperm_to_id	obj_temp8
equate: sperm_vel	obj_temp9

name:	ai_sperm
;	thread		ai_cycle
	on_hit		ai_spermicide

	next_node	sperm_dx,sperm_dy,sperm_steps
	ifeqi		-1,sperm_dx,@not_leader

	adopt_inrange_child	128*128

	; I am the leader

	stop
@leader_loop
;	queen_order	sperm_dx
;	queen_order	sperm_dy

	shl		16,obj_x,sperm_x
	shl		16,obj_y,sperm_y
@leader_move
	stop
	addv		sperm_dx,sperm_x
	addv		sperm_dy,sperm_y
	addv		sperm_ddx,sperm_dx
	addv		sperm_ddy,sperm_dy
	shr		16,sperm_x,obj_x
	shr		16,sperm_y,obj_y
	update_xy

	subi		1,sperm_steps
	ifgti		0,sperm_steps,@leader_move

	next_node	sperm_dx,sperm_dy,sperm_steps
	jmp		@leader_loop	

	; Someone else is leader

@not_leader
	stop

;	shl		16,obj_x,sperm_x
;	shl		16,obj_y,sperm_y
;	stop
;@kid_move
;	stop
;	addv		sperm_dx,sperm_x
;	addv		sperm_dy,sperm_y
;	shr		16,sperm_x,obj_x
;	shr		16,sperm_y,obj_y
;	update_xy
;	jmp		@kid_move
	
	get_queen_id	sperm_to_id
	get_angle_to_id	sperm_to_id


; --------------------------------------------------------------------------------
	divi		16,obj_angle,sperm_vel
	addi		32,sperm_vel

	iflti		64,sperm_vel,@not_too_big
	subi		64,sperm_vel
@not_too_big
	ifgei		0,sperm_vel,@not_too_small
	addi		64,sperm_vel
@not_too_small
	movei		63,sperm_dx
	subv		sperm_vel,sperm_dx

	set_frame	sperm_dx
; --------------------------------------------------------------------------------

	movernd		20000,sperm_vel
	addi		65536,sperm_vel
	
	set_vel		sperm_vel,sperm_dx,sperm_dy
	shl		16,obj_x,sperm_x
	shl		16,obj_y,sperm_y
	movernd		32,sperm_steps
	addi		40,sperm_steps
@fly_baby
	stop
	addv		sperm_dx,sperm_x
	addv		sperm_dy,sperm_y
	addv		sperm_ddx,sperm_dx
	addv		sperm_ddy,sperm_dy
	shr		16,sperm_x,obj_x
	shr		16,sperm_y,obj_y
	update_xy
	subi		1,sperm_steps
	ifgti		0,sperm_steps,@fly_baby

	ifeqi		0,obj_children,@not_leader

	jmp		@leader_loop

@make_dds
	movev		sperm_dx,sperm_ddx	       
	movev		sperm_dy,sperm_ddy
	divv		sperm_steps,sperm_ddx,sperm_ddx
	divv		sperm_steps,sperm_ddy,sperm_ddy
	neg		sperm_ddx
	neg		sperm_ddy

	divi		2,sperm_dx,sperm_vel
	addv		sperm_vel,sperm_dx
	divi		2,sperm_dy,sperm_vel
	addv		sperm_vel,sperm_dy
	return


name:	ai_spermicide
	shootable_off
	call		:expanding_polo
	die
	wait



;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Tonsl1
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_tonsl1
	on_hit	play_death_frames
	setanimnormal
	jmp	:spagetti_ai_pingpong
	wait

name:	play_death_frames
	setanimdeath
	jmp	:spagetti_ai_cycle

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Flea
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

equate:	flea_x		obj_temp1
equate:	flea_y		obj_temp2
equate:	flea_x_search	obj_temp3
equate:	flea_y_search	obj_temp4
equate:	flea_dx		obj_temp5
equate:	flea_dy		obj_temp6
equate:	flea_ddx	obj_temp7
equate:	flea_ddy	obj_temp8
equate:	flea_temp	obj_temp9

name:	ai_flea
	; move flea off landscape
@move_up
	stop
	subi		1,obj_y
	update_xy
	bg_collide	@move_up
	stop
	
	thread		ai_cycle
	
	shl		16,obj_x,flea_x
	shl		16,obj_y,flea_y

	movev		flea_x,flea_x_search
	movev		flea_y,flea_y_search

	movernd		8192,flea_dx
	addi		16384,flea_dx
	movernd		2,flea_temp
	index_array	flea_temp,:plus_or_minus,flea_temp
	mulv		flea_temp,flea_dx,flea_dx	

	movernd		8192,flea_dy
	addi		16384,flea_dy
	movernd		2,flea_temp
	index_array	flea_temp,:plus_or_minus,flea_temp
	mulv		flea_temp,flea_dy,flea_dy

	divi		64,flea_dx,flea_ddx
	divi		64,flea_dy,flea_ddy

	movei		0,flea_temp
@search
	stop
	addv		flea_dx,flea_x_search	
	addv		flea_dy,flea_y_search	
	addv		flea_ddx,flea_dx
	addv		flea_ddy,flea_dy
	shr		16,flea_x_search,obj_x
	shr		16,flea_y_search,obj_y
	update_xy
	bg_collide	@collided
	addi		1,flea_temp
	jmp		@search
@collided


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Polo
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
equate:	polo_r1		obj_temp1
equate:	polo_r2		obj_temp2
equate:	party_dx	obj_temp3
equate:	party_dy	obj_temp4
equate:	party_ddx	obj_temp5
equate:	party_ddy	obj_temp6
equate:	party_0		obj_temp7

name: 	throw_particles
	movei		0,party_0
	movei		0,party_ddx
	movei		0,party_ddy
	movei		64000,party_dx
	movei		64000,party_dy
@infinite
	movernd		32000,party_dx
	shl		2,party_dx,party_dx
	subi		64000,party_dx
	movernd		32000,party_dy
	shl		2,party_dy,party_dy
	subi		64000,party_dy
	shr		7,party_dx,party_ddx
	shr		7,party_dy,party_ddy

	add_particle	obj_x,obj_y,party_dx,party_dy,party_ddx,party_ddy
	stop
	jmp		@infinite
		
; Could be better

name:	ai_polo
	shootable_off
	thread		throw_particles
	thread		ai_none
	call		:expanding_polo
	shootable_on
	wait

:expanding_polo
	movei		16,polo_r1
	movei		12,polo_r2
@loop1
	stop
	addi		3,polo_r1
	addi		1,polo_r2
	add_polo	polo_r1,polo_r2,obj_x,obj_y
	iflti		48,polo_r1,@loop1
@loop2
	stop
	addi		1,polo_r1
	addi		3,polo_r2
	add_polo	polo_r1,polo_r2,obj_x,obj_y
	ifltv		polo_r1,polo_r2,@loop2

	return

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Dissolve
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_dissolve
	get_ret	obj_temp1
	set_ret	obj_temp1

	movei	24,obj_temp3
	movei	12,obj_temp4
@next
	movev	obj_temp3,obj_temp1
	movev	obj_temp4,obj_temp2
	visible_on
	pause	obj_temp1
	visible_off
	pause	obj_temp2
	subi	2,obj_temp3
	subi	1,obj_temp4
	ifgti	0,obj_temp4,@next
	die
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Path
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

equate:	path_x		obj_temp1
equate:	path_y		obj_temp2
equate:	path_dx		obj_temp3
equate:	path_dy		obj_temp4
equate: path_steps	obj_temp5
equate: path_x1		obj_temp6
equate: path_y1		obj_temp7
equate: path_x2		obj_temp8
equate: path_y2		obj_temp9
equate: path_0		obj_temp10

name:	ai_path
	on_hit		ai_spiral
	movei		0,path_0
	
	thread		ai_cycle
@loop
	stop
	animspeed	1
	movei		100,path_steps
	pause		path_steps
	animspeed	4

	next_node	path_dx,path_dy,path_steps

	shl		16,obj_x,path_x
	shl		16,obj_y,path_y
@move
	stop
	addv		path_dx,path_x
	addv		path_dy,path_y
	shr		16,path_x,obj_x
	shr		16,path_y,obj_y
	update_xy

	subi		1,path_steps
	ifgti		0,path_steps,@move

	jmp		@loop
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Test Pixel Collision (it now works)
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

equate:	test_x		obj_temp1
equate:	test_y		obj_temp2
equate:	test_dx		obj_temp3
equate:	test_dy		obj_temp4
equate:	test_ddx	obj_temp5
equate:	test_ddy	obj_temp6


name:	ai_test
	movei		0,test_ddx
	movei		0,test_ddy

@again
	movei		16-1,test_dy
@next_y
	stop
	movei		16-1,test_dx
@next_x
	stop
	movev		obj_x,test_x
	movev		obj_y,test_y

	addv		test_dx,test_x
	addv		test_dy,test_y

	point_collision	test_x,test_y,@bad

	subi		16,test_x

	add_particle	test_x,test_y,test_ddx,test_ddy,test_ddx,test_ddy
@bad
	
	subi		1,test_dx
	ifgti		-16,test_dx,@next_x

	subi		1,test_dy
	ifgti		-16,test_dy,@next_y

	jmp		@again
	
	
;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Floppy
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_floppy

	thread	ai_pingpong
	on_hit	ai_floppy_sparks
	wait
name:	ai_floppy_sparks
	shootable_off
	
	add_child	NAME_XB_FLOP1,ai_flop_exp
	
	movei		32,obj_temp1
	pause		obj_temp1
	shootable_on
	wait

name:	ai_flop_exp
	animspeed	2
	animplayforwardonce
	die
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Spiral Explosion
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Would be good if pod had spin as it fell...

equate:	pods_x		obj_temp5
equate:	pods_y		obj_temp6
equate:	pods_dx		obj_temp7
equate:	pods_dy		obj_temp8
equate:	pods_ddx	obj_temp9
equate:	pods_ddy	obj_temp10

name:	ai_pod
@restart
	thread		ai_none
	movei		0,obj_temp1
	visible_on
@wait
	stop
	inrange		128*128,@ready
	jmp		@wait

@ready
	movei		1,obj_temp1
	thread		ai_flash
@flash
	stop
	ifeqi		2,obj_temp1,@wait_to_be_dropped
	inrange		160*160,@flash
	jmp		@restart
@wait_to_be_dropped
	visible_on
	thread		ai_none
	stop
	ifeqi		2,obj_temp1,@wait_to_be_dropped
;	movei		0,pods_dx
;	movei		16384,pods_dy
;	movei		0,pods_ddx
;	movei		0,pods_ddy
	shl		16,obj_x,pods_x
	shl		16,obj_y,pods_y
@fall
	stop
	addv		pods_dx,pods_x
	addv		pods_dy,pods_y
	addv		pods_ddx,pods_dx
	addv		pods_ddy,pods_dy
	shr		16,pods_x,obj_x
	shr		16,pods_y,obj_y
	update_xy
	bg_collide	@restore
	jmp		@fall
@restore
	subv		pods_dx,pods_x
	subv		pods_dy,pods_y
	shr		16,pods_x,obj_x
	shr		16,pods_y,obj_y
	update_xy
	jmp		@restart	

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Spiral Explosion
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Lots and lots of fun...
; By tweaking variables anything can happen...

equate:	starfsh_command		obj_temp1

name:	ai_starfsh
	on_hit		ai_spiral
	thread		ai_cycle

	adopt_inrange_child	64*64

	set_angle	obj_temp3

	ifeqi		0,obj_children,@star_child
@start
	movei		0,starfsh_command
@hang
	stop
	inrange		256*256,@ready
	jmp		@hang
@ready
	movei		1,starfsh_command
	stop
	jmp		@start

@star_child
	stop
	get_queens	starfsh_command
	ifeqi		0,starfsh_command,@star_child

	addi		1,obj_y
	update_xy
	jmp		@star_child

equate:	spiral_iter	obj_temp1
equate:	spiral_vel	obj_temp2
equate:	spiral_dangle	obj_temp3
equate:	spiral_spawn	obj_temp4
equate:	spiral_dx	obj_temp5
equate:	spiral_dy	obj_temp6
equate:	spiral_ddx	obj_temp7
equate:	spiral_ddy	obj_temp8
equate:	SPIRAL_ITS	64
equate:	SPIRAL_ICOUNT	4*2
equate:	SPIRAL_ADD_A	64/2
equate:	SPIRAL_ADD_B	(16+1)
equate:	SPIRAL_RND	32767
equate:	SPIRAL_CONST	66666
equate:	SPIRAL_SHIFT	6

name:	ai_spiral
	get_player_id	obj_temp1
	get_angle_to_id	obj_temp1
	movei		0,obj_temp1
	set_angle	obj_temp1

	add_child	NAME_NXF,ai_explode
	stop

	movei		SPIRAL_ITS,obj_temp1
@next
	stop
	movei		SPIRAL_ADD_A,spiral_dangle

	movernd		SPIRAL_RND,spiral_vel
	addi		SPIRAL_CONST,spiral_vel

	movei		SPIRAL_ICOUNT,spiral_spawn
@spawn
	add_to_angle	spiral_dangle
	set_vel		spiral_vel,spiral_dx,spiral_dy
	shr		SPIRAL_SHIFT,spiral_dx,spiral_ddx
	shr		SPIRAL_SHIFT,spiral_dy,spiral_ddy
	neg		spiral_ddx
	neg		spiral_ddy
	add_particle	obj_x,obj_y,spiral_dx,spiral_dy,spiral_ddx,spiral_ddy

	subi		1,spiral_spawn
	ifgti		0,spiral_spawn,@spawn

	movei		SPIRAL_ADD_B,spiral_dangle
	add_to_angle	spiral_dangle

	subi		1,spiral_iter
	ifgti		0,spiral_iter,@next
	
	die
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Swarm
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Example parent child system
; Example queen order
; Need take_order

equate:	swarm_x		obj_temp1
equate:	swarm_y		obj_temp2
equate:	swarm_dx	obj_temp3
equate:	swarm_dy	obj_temp4
equate:	swarm_ddx	obj_temp5
equate:	swarm_ddy	obj_temp6
equate:	swarm_counter	obj_temp7
equate:	swarm_temp	obj_temp8
equate:	swarm_temp2	obj_temp9
equate: swarm_speed	obj_temp10

name:	ai_swarm

	bg_collide	:spagetti_ai_flash

	on_hit		ai_remove

	thread		ai_tad_anim

	movei		0,swarm_ddx
	movei		0,swarm_ddy

	adopt_inrange_child	256*256
	stop
 	get_queen_id	swarm_temp
	get_angle_to_id	swarm_temp

@swarm_question
	movei		0,swarm_dx
	movei		0,swarm_dy
	movei		0,swarm_speed

	ifeqi		0,obj_children,@swarm_child

@swarm_follow
	stop
	get_vector	swarm_dx,swarm_dy
	addv		swarm_dx,obj_x
	addv		swarm_dy,obj_y
	update_xy
	jmp		@swarm_follow

	; I'm a parent

;	queen_order	swarm_ddx
;	queen_order	swarm_ddy

	stop
	jmp		@swarm_make_vel
	; I'm a child
@swarm_child
	stop
	
	movernd		256,swarm_speed
	addi		128,swarm_speed
	movei		256,swarm_speed
	set_vel		swarm_speed,swarm_dx,swarm_dy

;	get_queens	swarm_ddx
;	get_queens	swarm_ddy

@swarm_make_vel
;	movernd		128,swarm_dx
;	addi		64,swarm_dx
;	mulv		swarm_ddx,swarm_dx,swarm_dx

;	movernd		128,swarm_dy
;	addi		64,swarm_dy
;	mulv		swarm_ddy,swarm_dy,swarm_dy

	shl		8,obj_x,swarm_x
	shl		8,obj_y,swarm_y

	movei		64,swarm_counter
@swarm_move
	stop
	ifgei		16,swarm_counter,@swarm_skip
;	ifnei		15,swarm_counter,@no_fetch
	get_queen_id	swarm_temp
	get_rotation_to_id	swarm_temp,swarm_ddx
@no_fetch
	add_to_angle	swarm_ddx
	add_to_angle	swarm_ddx
	set_vel		swarm_speed,swarm_dx,swarm_dy
@swarm_skip

	addv		swarm_dx,swarm_x
	addv		swarm_dy,swarm_y
	shr		8,swarm_x,obj_x
	shr		8,swarm_y,obj_y
	update_xy
	subi		1,swarm_counter
	ifnei		0,swarm_counter,@swarm_move

;	get_queen_id	swarm_temp
;	get_rotation_to_id	swarm_temp,swarm_ddx
;	movei		32,swarm_counter
;@swarm_turn
;	stop
;	add_to_angle	swarm_ddx
;	subi		1,swarm_counter
;	ifnei		0,swarm_counter,@swarm_turn
;	pause		swarm_counter

	jmp		@swarm_question

; Causes crash!!!
; Not enough checking
; Need an AND function...

name:	ai_tad_anim
	movei		0,swarm_temp2
	movei		0,swarm_ddy
@loop
	get_angle	swarm_temp
	shr		2,swarm_temp,swarm_temp
	addi		16,swarm_temp
;	iflti		64,swarm_temp,@no_problemo	
	iflei		64,swarm_temp,@no_problemo	; Attempted fix
	subi		64,swarm_temp
@no_problemo
	neg		swarm_temp
	addi		64,swarm_temp

	addi		64,swarm_temp2
	iflti		64*3,swarm_temp2,@ok
	movei		0,swarm_temp2
@ok
	addv		swarm_temp2,swarm_temp

	set_frame	swarm_temp
	stop
	jmp		@loop

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Tad
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; These need to point in the correct direction...
; Example of scanning ahead 
       	
equate:	tad_x		obj_temp1
equate:	tad_y		obj_temp2
equate:	tad_x_furthur	obj_temp3
equate:	tad_y_furthur	obj_temp4
equate:	tad_dx		obj_temp5
equate:	tad_dy		obj_temp6
equate:	tad_ddx		obj_temp7
equate:	tad_ddy		obj_temp8
equate:	tad_search_dist	obj_temp9
equate:	tad_counter	obj_temp10
equate: TAD_AHEAD_DIST	8
equate: TAD_MIN_SPEED	192
equate: TAD_RND_SPEED	128

name: 	ai_tad
	update_xy

	bg_collide	:spagetti_ai_flash

	on_hit		ai_tad_hit

	thread		ai_cycle

	shl		8,obj_x,tad_x
	shl		8,obj_y,tad_y
	
	; get direction

	movei		TAD_AHEAD_DIST,tad_search_dist
@tad_start
	subi		1,tad_search_dist
	movev		tad_x,tad_x_furthur
	movev		tad_y,tad_y_furthur
	shr		8,tad_x,obj_x
	shr		8,tad_y,obj_y
	update_xy
@tad_wait
	stop
	inrange		512*512,@tad_go
	jmp		@tad_wait
@tad_go

	movernd		2,tad_ddx
	index_array	tad_ddx,:plus_or_minus,tad_ddx
	movernd		TAD_MIN_SPEED,tad_dx
	addi		TAD_RND_SPEED,tad_dx
	mulv		tad_ddx,tad_dx,tad_dx

	movernd		2,tad_ddy
	index_array	tad_ddy,:plus_or_minus,tad_ddy
	movernd		TAD_MIN_SPEED,tad_dy
	addi		TAD_RND_SPEED,tad_dy
	mulv		tad_ddy,tad_dy,tad_dy

	neg		tad_ddx
	neg		tad_ddy

	movev		tad_search_dist,tad_counter
@tad_scan
	stop
	addv		tad_dx,tad_x_furthur
	addv		tad_dy,tad_y_furthur
	shr		8,tad_x_furthur,obj_x
	shr		8,tad_y_furthur,obj_y
	update_xy
	bg_collide	:tad_start
	shr		8,tad_x,obj_x
	shr		8,tad_y,obj_y
	update_xy
	subi		1,tad_counter
	ifgti		0,tad_counter,@tad_scan

	movernd		64,tad_counter
	addi		48,tad_counter
@tad_move
	stop
	addv		tad_dx,tad_x
	addv		tad_dy,tad_y
	addv		tad_dx,tad_x_furthur
	addv		tad_dy,tad_y_furthur
	addv		tad_ddx,tad_dx
	addv		tad_ddy,tad_dy

	shr		8,tad_x_furthur,obj_x
	shr		8,tad_y_furthur,obj_y
	update_xy
	bg_collide	:tad_start

	shr		8,tad_x,obj_x
	shr		8,tad_y,obj_y
	update_xy

	subi		1,tad_counter
	ifnei		0,tad_counter,@tad_move

	movei		TAD_AHEAD_DIST,tad_search_dist
	jmp		@tad_start

:plus_or_minus	
	array	2,-1,1


name:	ai_tad_hit
	animspeed	0
@loop
     	setanimnormal
	animplayforwardonce
	jmp	@loop

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Star
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name: ai_star
@loop
	setanimleft
	animplayforwardonce

	setanimright
	animplayforwardonce

	setanimup
	animplayforwardonce

	setanimdown
	animplayforwardonce

	setanimnormal
	animplayforwardonce
	
	setanimwounded
	animplayforwardonce

	setanimdeath
	animplayforwardonce

	jmp	@loop	

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Do Nothing
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name: ai_none
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Flash
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_flash
:spagetti_ai_flash
@loop
	visible_on
	stop
	stop
	stop
	stop
	visible_off
	stop
	stop
	jmp		@loop

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Cycle Animation
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name: ai_cycle
:spagetti_ai_cycle
@cycle_loop
	stop
	frameinc
	jmp	@cycle_loop

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Ping Pong Animation
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name: ai_pingpong
:spagetti_ai_pingpong
:pingpong_loop
	animplayforwardonce
	animplaybackonce
	jmp	:pingpong_loop

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Explode
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name: ai_explode
	shootable_off
	set_priority	0
	animspeed	2
	animplayforwardonce
	die
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Exploding Death
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name: ai_death
	add_child	NAME_EXPLOSION,ai_explode
	die
	wait

name: ai_remove
	die
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Crystal
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

equate: particle_x	obj_temp1
equate: particle_y	obj_temp2
equate: particle_dx	obj_temp3
equate: particle_dy	obj_temp4
equate: particle_ddx	obj_temp5
equate: particle_ddy	obj_temp6
equate: crystal_count	obj_temp7
equate: particle_iter 	obj_temp8
equate: particle_id 	obj_temp10

name: ai_test_ref
	movei		32,particle_ddx	
	movei		16,particle_ddy
	movei		24,particle_dx	
	movei		20,particle_dy
	thread		ai_cycle
@next_c
	stop
;	get_ref_point	0,particle_x,particle_y
;	add_polo	particle_dx,particle_dy,particle_x,particle_y	
;	get_ref_point	1,particle_x,particle_y
;	add_polo	particle_ddx,particle_ddy,particle_x,particle_y	
;	get_ref_point	2,particle_x,particle_y
;	add_polo	particle_dx,particle_dy,particle_x,particle_y	
;	get_ref_point	3,particle_x,particle_y
;	add_polo	particle_dx,particle_dy,particle_x,particle_y	
	jmp		@next_c

	movei		4,particle_iter
@next_i
	movernd		32767,particle_dx
	addi		-16384,particle_dx
	addv		particle_dx,particle_dx
	movernd		32767,particle_dy
	addi		-16384,particle_dy
	addv		particle_dy,particle_dy
     	add_particle	particle_x,particle_y,particle_dx,particle_dy,particle_ddx,particle_ddy
	subi		1,particle_iter
	ifnei		0,particle_iter,@next_i
	
	jmp		@next_c


name: ai_crystal
	wait
	movei		0,particle_ddx	
	movei		0,particle_ddy
;	on_hit		ai_blow_crystal
	on_hit		ai_stream_crystal
	visible_on
	thread		ai_cycle
:crystal_animate
	wait
	stop
;	animplayforwardonce
;	animplaybackonce
	jmp		:crystal_animate

:ai_add_part
	movernd		32767,particle_iter
	addi		99999,particle_iter
	set_vel		particle_iter,particle_dx,particle_dy
	add_particle	particle_x,particle_y,particle_dx,particle_dy,particle_ddx,particle_ddy
	return

name: ai_stream_crystal
	animspeed	0
	movei		64,crystal_count
@find_safe_area
	stop
	subi		1,crystal_count
	iflei		0,crystal_count,@finished
	movernd		16,particle_x
	addi		-8,particle_x
	addv		obj_x,particle_x
	movernd		16,particle_y
	addi		-8,particle_y
	addv		obj_y,particle_y
	point_collision	particle_x,particle_y,@find_safe_area

	get_player_id	particle_id
	get_angle_to_id particle_id
@next_particle
	call		:ai_add_part
	stop
	movei		-2,particle_id
	add_to_angle	particle_id
	call		:ai_add_part
	stop
	movei		4,particle_id
	add_to_angle	particle_id
	call		:ai_add_part
	stop
	movei		-2,particle_id
	add_to_angle	particle_id

	ifeqi		0,crystal_count,@finished	
	stop
	stop
	subi		1,crystal_count
	jmp		@next_particle
		
@finished
	animspeed	4
	stop
	jmp		:crystal_animate	

name: ai_blow_crystal
	shootable_off
	movei		32,crystal_count
:next_crystals_particle
	stop
	movei		4,particle_iter
:next_iter
	movernd		16,particle_x
	addi		-8,particle_x
	addv		obj_x,particle_x
	movernd		16,particle_y
	addi		-8,particle_y
	addv		obj_y,particle_y
		
	movernd		32767,particle_dx
	addi		-16384,particle_dx
	addv		particle_dx,particle_dx
	movernd		32767,particle_dy
	addi		-16384,particle_dy
	addv		particle_dy,particle_dy
     	add_particle	particle_x,particle_y,particle_dx,particle_dy,particle_ddx,particle_ddy
	subi		1,particle_iter
	ifnei		0,particle_iter,:next_iter
	
	subi		1,crystal_count
	ifnei		0,crystal_count,:next_crystals_particle
	shootable_on
	jmp		:crystal_animate	


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Wind Tunnel
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name: ai_wind_tunnel
	movei		64,particle_ddx
	movei		0,particle_ddy
:dont_do_it
	stop
	inrange		256*256,:unwind
	jmp		:dont_do_it
:unwind
	animplayforwardonce
:wind_loop
	stop
	movernd		32,particle_x
	addi		-16,particle_x
	addv		obj_x,particle_x
	movernd		32,particle_y
	addi		-16,particle_y
	addv		obj_y,particle_y
	
	movernd		32767,particle_dx
	addi		-65536-32768-16384,particle_dx
	movernd		32767,particle_dy
	addi		-16384,particle_dy

	add_particle	particle_x,particle_y,particle_dx,particle_dy,particle_ddx,particle_ddy
	inrange		480*480,:wind_loop
	animplaybackonce
	jmp		:dont_do_it

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Asteroid
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Next Add a sprite when shot
; Plus animate frames on a thread

equate:	ast_facing	obj_temp1
equate:	ast_x		obj_temp2
equate:	ast_y		obj_temp3
equate:	ast_dx		obj_temp4
equate:	ast_dy		obj_temp5
equate:	ast_temp	obj_temp6

name: ai_asteroid
:ast_dummy
	thread		ai_cycle
	on_hit		ai_asteroid_break
	movernd		8,ast_facing
	index_array	ast_facing,:bounce_x,ast_temp
	shl		8,ast_temp,ast_dx
	index_array	ast_facing,:bounce_y,ast_temp
	shl		8,ast_temp,ast_dy
	shl		8,obj_x,ast_x
	shl		8,obj_y,ast_y
:move_ast
	stop
	addv		ast_dx,ast_x
	addv		ast_dy,ast_y
	shr		8,ast_x,obj_x
	shr		8,ast_y,obj_y
	update_xy
	bg_collide	:ast_change_dir
	jmp		:move_ast
:ast_change_dir
	subv		ast_dx,ast_x
	subv		ast_dy,ast_y
	shr		8,ast_x,obj_x
	shr		8,ast_y,obj_y
	addi		1,ast_facing
	ifnei		8,ast_facing,:ast_do_it
	movei		0,ast_facing
:ast_do_it
	index_array	ast_facing,:bounce_x,ast_temp
	shl		8,ast_temp,ast_dx
	movernd		64,ast_temp
	addv		ast_temp,ast_dx
	index_array	ast_facing,:bounce_y,ast_temp
	shl		8,ast_temp,ast_dy
	movernd		64,ast_temp
	addv		ast_temp,ast_dy
	jmp		:move_ast

name:	ai_asteroid_break
	on_hit		ai_none
	add_child	NAME_RCK2,ai_asteroid
	add_child	NAME_RCK2,ai_asteroid
	add_child	NAME_RCK2,ai_asteroid
	thread		ai_none
	die
	wait

:bounce_x
	array	       	8,1,1,-1,-1, -1,-1, 1,1
:bounce_y
	array		8,1,-1,1,-1, 1, 1, -1,1

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Bug
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Next Add a sprite when shot
; Plus animate frames on a thread

equate:	bug_flood_val	obj_temp1
equate:	bug_dx		obj_temp2
equate:	bug_dy		obj_temp3

name: ai_bug
:bug_wait
	stop
	get_flood_val	bug_flood_val
	ifeqi		-1,bug_flood_val,:bug_wait
	iflti		8,bug_flood_val,:bug_wait

:bug_fly
	stop
	get_flood_val	bug_flood_val
	ifeqi		-1,bug_flood_val,:bug_wait

	get_vector	bug_dx,bug_dy
	addv		bug_dx,obj_x
	addv		bug_dy,obj_y
	update_xy
	get_flood_val	bug_flood_val
	ifeqi		-1,bug_flood_val,:bug_exception
	frameinc
:back_in
	jmp		:bug_fly
:bug_exception
	subv		bug_dx,obj_x
	update_xy
	get_flood_val	bug_flood_val
	jmp		:back_in

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Survrs
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
; Flow:-
; Runs left or right for a random amount of time...
; Waves a bit then picks another direction
; If he reaches a wall he runs the other way...
; If he is stuck he just waves...

equate:	man_min_x	obj_temp1
equate:	man_max_x	obj_temp2
equate:	man_timer	obj_temp3
equate:	man_dummy	obj_temp4
equate:	man_guess	obj_temp5
equate:	man_x8		obj_temp6
equate: pod_x		obj_temp7
equate: pod_y		obj_temp8
equate: man_temp	obj_temp9
equate: MAN_RUN_CONST	32
equate: MAN_RUN_RND	96
equate: MAN_SPEED	160
equate: MAN_CHILL_DELAY 16

name: ai_cycle_man
@cycle_loop
	movei		0,path_0
	stop
	frameinc

	get_sprites_coords path_x1,path_y1,path_x2,path_y2

	jmp	@cycle_loop


name:	ai_survrs_die
	thread		ai_blow
	movei		0,particle_ddx	
	movei		2048,particle_ddy
:keep_particleing
	stop
	movei		10,particle_iter
:next_piter
	movernd		16,particle_x
	addi		-8,particle_x
	addv		obj_x,particle_x
	movernd		16,particle_y
	addi		-8,particle_y
	addv		obj_y,particle_y
		
	movernd		32767,particle_dx
	addi		-16384,particle_dx
	muli		6,particle_dx,particle_dx
	movernd		32767,particle_dy
	addi		-16384,particle_dy
	addv		particle_dy,particle_dy
	muli		6,particle_dy,particle_dy
	add_particle	particle_x,particle_y,particle_dx,particle_dy,particle_ddx,particle_ddy
	subi		1,particle_iter
	ifnei		0,particle_iter,:next_piter
	jmp	 	:keep_particleing
	wait
	
name:	ai_blow
	setanimwounded
	animplayforwardonce
	setanimdeath
	animplayforwardonce
	die
	wait

name:	ai_survrs

	on_hit	ai_survrs_die
	setanimleft
	bg_collide	:man_stuck
	setanimright
	bg_collide	:man_stuck
	  
	movernd		MAN_RUN_RND,man_timer
	addi		MAN_RUN_CONST,man_timer

	movev		obj_x,man_min_x
	movev		obj_x,man_max_x

	shl		8,obj_x,man_x8

	setanimnormal

	output_var	0,man_min_x
	output_var	1,man_max_x


	thread		ai_cycle
:man_hang
	inrange		128*128,:man_turn_left
	stop
	jmp		:man_hang
	
; ----------------------------------------------------
:man_turn_left
	thread		ai_none
	update_xy
	setanimleft
:go_left
	stop
	subi		1,man_timer
	iflei		0,man_timer,:man_wave_or_jump
	frameinc
	subi		MAN_SPEED,man_x8
	shr		8,man_x8,obj_x
	update_xy
	bg_collide	:assess_left
	iflev		obj_x,man_min_x,:go_left
	movev		obj_x,man_min_x
	jmp		:go_left
:assess_left
	iflev		obj_x,man_min_x,:go_left
	addi		MAN_SPEED,man_x8
	shr		8,man_x8,obj_x
	update_xy
	jmp		:man_turn_right
; ----------------------------------------------------
:man_wave_or_jump
	setanimnormal
;*****************************************************
@haha
	stop
	get_pod_id	man_dummy
	get_ids_var	man_dummy,obj_x,pod_x
	get_ids_var	man_dummy,obj_y,pod_y
	subv		obj_y,pod_y
	ifgti		0,pod_y,@positive
@negative
	neg		pod_y
@positive
	ifgti		32,pod_y,@haha
	
	get_ids_var	man_dummy,obj_temp2,man_temp
	addi		1,man_temp
	set_ids_var	man_temp,man_dummy,obj_temp2

	movernd		MAN_RUN_RND,man_timer
	addi		MAN_RUN_CONST,man_timer
	
	ifgtv		pod_x,obj_x,@man_turn_left
	jmp		@man_turn_right

	ifgtv		pod_x,obj_x,@to_the_left
@to_the_right
	addi		1,obj_x
	update_xy
	stop
	jmp		@haha	
@to_the_left
	subi		1,obj_x
	update_xy
	stop
	jmp		@haha	
;*****************************************************
	movei		MAN_CHILL_DELAY,man_dummy
	pause		man_dummy

	movernd		2,man_dummy
	addi		1,man_dummy
:wave_again
	movernd		2,man_guess
	setanimup
	ifeqi		0,man_guess,:man_jump
	setanimnormal
:man_jump
	animplayforwardonce
	subi		1,man_dummy
	ifnei		0,man_dummy,:wave_again

	inrange		256*256,:man_near
	jmp		:man_wave_or_jump
:man_near

	movernd		MAN_RUN_RND,man_timer
	addi		MAN_RUN_CONST,man_timer
	
	setanimnormal
	movei		MAN_CHILL_DELAY,man_dummy
	pause		man_dummy

	movernd		2,man_dummy
	ifeqi		0,man_dummy,:man_turn_left
; -----------------------------------------------------
:man_turn_right
	setanimright
:go_right
	subi		1,man_timer
	iflei		0,man_timer,:man_wave_or_jump
	stop
	frameinc
	addi		MAN_SPEED,man_x8
	shr		8,man_x8,obj_x
	update_xy
	bg_collide	:assess_right
	ifgev		obj_x,man_max_x,:go_right
	movev		obj_x,man_max_x
	jmp		:go_right
:assess_right
	ifgev		obj_x,man_max_x,:go_right
	subi		MAN_SPEED,man_x8
	shr		8,man_x8,obj_x
	update_xy
	jmp		:man_turn_left
; -----------------------------------------------------
:man_stuck
	output_var	0,obj_x
	output_var	1,obj_y


	movernd		MAN_CHILL_DELAY,man_dummy
	pause		man_dummy
	setanimnormal
	animplayforwardonce
	jmp	:man_stuck

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Star Fish
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_starfish_die
	setanimwounded
	animplayforwardonce
	setanimdeath
	animplayforwardonce
	die
	wait

name:	ai_starfish
	on_hit	ai_death
:starfish_anim
	setanimnormal
	animplayforwardonce
	jmp	:starfish_anim

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Mussel
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_mussel_die
	shootable_off
	thread	ai_none
	setanimwounded
	animplayforwardonce
	setanimdeath
	animplayforwardonce
	thread	ai_pingpong
	wait
	die
	wait

name:	ai_mussel
	on_hit	ai_mussel_die
	thread	ai_pingpong
	wait

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; AI Tentacle
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

name:	ai_tentacle
	on_hit	ai_tentacle_hit
:tentacle_loop
	movernd	16,obj_temp1
	addi	32,obj_temp1
	pause	obj_temp1
	setanimnormal
	animplayforwardonce
	animplaybackonce
	jmp	:tentacle_loop

name:	ai_tentacle_hit
	animplaybackonce
	jmp	:tentacle_loop



;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; End of file
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
