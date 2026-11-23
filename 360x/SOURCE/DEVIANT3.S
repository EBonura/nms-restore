deviant3_170:
	sll	s0,s7,8
	or	s1,s0,s7
	sll	s0,s1,16
	jr	ra
	or	s0,s1

deviant3_85:
	sll	s0,s6,8
	or	s1,s0,s6
	sll	s0,s1,16
	jr	ra
	or	s0,s1

deviant3_0:
	sll	s0,s5,8
	or	s1,s0,s5
	sll	s0,s1,16
	jr	ra
	or	s0,s1

deviant3_1:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_2:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_3:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_4:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_5:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_6:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_7:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_8:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_9:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_10:
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_11:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_12:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_13:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_14:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_15:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_16:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_17:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_18:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_19:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_20:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_21:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_22:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_23:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_24:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_25:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_26:
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_27:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_28:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_29:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_30:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_31:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_32:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_33:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_34:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_35:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_36:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_37:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_38:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_39:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_40:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_41:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_42:
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_43:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_44:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_45:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_46:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_47:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_48:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_49:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_50:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_51:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_52:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_53:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_54:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_55:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_56:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_57:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_58:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_59:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_60:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_61:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_62:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_63:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s5
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_64:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_65:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_66:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_67:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_68:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_69:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_70:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_71:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_72:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_73:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_74:
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_75:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_76:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_77:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_78:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_79:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_80:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_81:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_82:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_83:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_84:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	

deviant3_86:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_87:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_88:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_89:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_90:
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_91:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_92:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_93:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_94:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_95:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_96:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_97:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_98:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_99:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_100:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_101:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_102:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_103:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_104:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_105:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_106:
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_107:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_108:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_109:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_110:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_111:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_112:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_113:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_114:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_115:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_116:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_117:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_118:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_119:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_120:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_121:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_122:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_123:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_124:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_125:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_126:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_127:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s6
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_128:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_129:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_130:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_131:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_132:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_133:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_134:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_135:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_136:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_137:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_138:
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_139:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_140:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_141:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_142:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_143:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_144:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_145:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_146:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_147:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_148:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_149:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_150:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_151:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_152:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_153:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_154:
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_155:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_156:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_157:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_158:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_159:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_160:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_161:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_162:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_163:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_164:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_165:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_166:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_167:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_168:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_169:
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_171:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s1,24
	jr	ra
	or	s0,s0,t0
	
deviant3_172:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_173:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_174:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_175:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s1,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_176:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_177:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_178:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_179:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_180:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_181:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_182:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_183:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_184:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_185:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_186:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_187:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_188:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_189:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_190:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_191:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s7
	sll	t0,s1,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_192:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_193:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_194:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_195:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_196:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_197:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_198:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_199:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_200:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_201:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_202:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_203:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_204:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_205:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_206:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_207:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s5,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_208:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_209:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_210:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_211:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_212:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_213:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_214:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_215:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_216:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_217:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_218:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_219:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_220:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_221:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_222:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_223:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s6,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_224:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_225:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_226:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_227:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_228:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_229:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_230:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_231:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_232:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_233:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_234:
	lbu	s1,0(a0)
	add	a0,a0,1
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_235:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s2,24
	jr	ra
	or	s0,s0,t0
	
deviant3_236:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_237:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_238:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_239:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s7,8
	or	s0,s0,t0
	sll	t0,s2,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_240:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_241:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_242:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_243:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s5,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_244:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_245:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_246:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_247:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s6,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_248:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_249:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_250:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	add	a0,a0,2
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_251:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s7,16
	or	s0,s0,t0
	sll	t0,s3,24
	jr	ra
	or	s0,s0,t0
	
deviant3_252:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s3,16
	or	s0,s0,t0
	sll	t0,s5,24
	jr	ra
	or	s0,s0,t0
	
deviant3_253:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s3,16
	or	s0,s0,t0
	sll	t0,s6,24
	jr	ra
	or	s0,s0,t0
	
deviant3_254:
	lbu	s1,0(a0)
	lbu	s2,1(a0)
	lbu	s3,2(a0)
	add	a0,a0,3
	or	s0,zero,s1
	sll	t0,s2,8
	or	s0,s0,t0
	sll	t0,s3,16
	or	s0,s0,t0
	sll	t0,s7,24
	jr	ra
	or	s0,s0,t0
	
deviant3_255:
	ulw	s0,0(a0)
	jr	ra
	add	a0,a0,4

dev3_vect:
	dw	deviant3_0
	dw	deviant3_1
	dw	deviant3_2
	dw	deviant3_3
	dw	deviant3_4
	dw	deviant3_5
	dw	deviant3_6
	dw	deviant3_7
	dw	deviant3_8
	dw	deviant3_9
	dw	deviant3_10
	dw	deviant3_11
	dw	deviant3_12
	dw	deviant3_13
	dw	deviant3_14
	dw	deviant3_15
	dw	deviant3_16
	dw	deviant3_17
	dw	deviant3_18
	dw	deviant3_19
	dw	deviant3_20
	dw	deviant3_21
	dw	deviant3_22
	dw	deviant3_23
	dw	deviant3_24
	dw	deviant3_25
	dw	deviant3_26
	dw	deviant3_27
	dw	deviant3_28
	dw	deviant3_29
	dw	deviant3_30
	dw	deviant3_31
	dw	deviant3_32
	dw	deviant3_33
	dw	deviant3_34
	dw	deviant3_35
	dw	deviant3_36
	dw	deviant3_37
	dw	deviant3_38
	dw	deviant3_39
	dw	deviant3_40
	dw	deviant3_41
	dw	deviant3_42
	dw	deviant3_43
	dw	deviant3_44
	dw	deviant3_45
	dw	deviant3_46
	dw	deviant3_47
	dw	deviant3_48
	dw	deviant3_49
	dw	deviant3_50
	dw	deviant3_51
	dw	deviant3_52
	dw	deviant3_53
	dw	deviant3_54
	dw	deviant3_55
	dw	deviant3_56
	dw	deviant3_57
	dw	deviant3_58
	dw	deviant3_59
	dw	deviant3_60
	dw	deviant3_61
	dw	deviant3_62
	dw	deviant3_63
	dw	deviant3_64
	dw	deviant3_65
	dw	deviant3_66
	dw	deviant3_67
	dw	deviant3_68
	dw	deviant3_69
	dw	deviant3_70
	dw	deviant3_71
	dw	deviant3_72
	dw	deviant3_73
	dw	deviant3_74
	dw	deviant3_75
	dw	deviant3_76
	dw	deviant3_77
	dw	deviant3_78
	dw	deviant3_79
	dw	deviant3_80
	dw	deviant3_81
	dw	deviant3_82
	dw	deviant3_83
	dw	deviant3_84
	dw	deviant3_85
	dw	deviant3_86
	dw	deviant3_87
	dw	deviant3_88
	dw	deviant3_89
	dw	deviant3_90
	dw	deviant3_91
	dw	deviant3_92
	dw	deviant3_93
	dw	deviant3_94
	dw	deviant3_95
	dw	deviant3_96
	dw	deviant3_97
	dw	deviant3_98
	dw	deviant3_99
	dw	deviant3_100
	dw	deviant3_101
	dw	deviant3_102
	dw	deviant3_103
	dw	deviant3_104
	dw	deviant3_105
	dw	deviant3_106
	dw	deviant3_107
	dw	deviant3_108
	dw	deviant3_109
	dw	deviant3_110
	dw	deviant3_111
	dw	deviant3_112
	dw	deviant3_113
	dw	deviant3_114
	dw	deviant3_115
	dw	deviant3_116
	dw	deviant3_117
	dw	deviant3_118
	dw	deviant3_119
	dw	deviant3_120
	dw	deviant3_121
	dw	deviant3_122
	dw	deviant3_123
	dw	deviant3_124
	dw	deviant3_125
	dw	deviant3_126
	dw	deviant3_127
	dw	deviant3_128
	dw	deviant3_129
	dw	deviant3_130
	dw	deviant3_131
	dw	deviant3_132
	dw	deviant3_133
	dw	deviant3_134
	dw	deviant3_135
	dw	deviant3_136
	dw	deviant3_137
	dw	deviant3_138
	dw	deviant3_139
	dw	deviant3_140
	dw	deviant3_141
	dw	deviant3_142
	dw	deviant3_143
	dw	deviant3_144
	dw	deviant3_145
	dw	deviant3_146
	dw	deviant3_147
	dw	deviant3_148
	dw	deviant3_149
	dw	deviant3_150
	dw	deviant3_151
	dw	deviant3_152
	dw	deviant3_153
	dw	deviant3_154
	dw	deviant3_155
	dw	deviant3_156
	dw	deviant3_157
	dw	deviant3_158
	dw	deviant3_159
	dw	deviant3_160
	dw	deviant3_161
	dw	deviant3_162
	dw	deviant3_163
	dw	deviant3_164
	dw	deviant3_165
	dw	deviant3_166
	dw	deviant3_167
	dw	deviant3_168
	dw	deviant3_169
	dw	deviant3_170
	dw	deviant3_171
	dw	deviant3_172
	dw	deviant3_173
	dw	deviant3_174
	dw	deviant3_175
	dw	deviant3_176
	dw	deviant3_177
	dw	deviant3_178
	dw	deviant3_179
	dw	deviant3_180
	dw	deviant3_181
	dw	deviant3_182
	dw	deviant3_183
	dw	deviant3_184
	dw	deviant3_185
	dw	deviant3_186
	dw	deviant3_187
	dw	deviant3_188
	dw	deviant3_189
	dw	deviant3_190
	dw	deviant3_191
	dw	deviant3_192
	dw	deviant3_193
	dw	deviant3_194
	dw	deviant3_195
	dw	deviant3_196
	dw	deviant3_197
	dw	deviant3_198
	dw	deviant3_199
	dw	deviant3_200
	dw	deviant3_201
	dw	deviant3_202
	dw	deviant3_203
	dw	deviant3_204
	dw	deviant3_205
	dw	deviant3_206
	dw	deviant3_207
	dw	deviant3_208
	dw	deviant3_209
	dw	deviant3_210
	dw	deviant3_211
	dw	deviant3_212
	dw	deviant3_213
	dw	deviant3_214
	dw	deviant3_215
	dw	deviant3_216
	dw	deviant3_217
	dw	deviant3_218
	dw	deviant3_219
	dw	deviant3_220
	dw	deviant3_221
	dw	deviant3_222
	dw	deviant3_223
	dw	deviant3_224
	dw	deviant3_225
	dw	deviant3_226
	dw	deviant3_227
	dw	deviant3_228
	dw	deviant3_229
	dw	deviant3_230
	dw	deviant3_231
	dw	deviant3_232
	dw	deviant3_233
	dw	deviant3_234
	dw	deviant3_235
	dw	deviant3_236
	dw	deviant3_237
	dw	deviant3_238
	dw	deviant3_239
	dw	deviant3_240
	dw	deviant3_241
	dw	deviant3_242
	dw	deviant3_243
	dw	deviant3_244
	dw	deviant3_245
	dw	deviant3_246
	dw	deviant3_247
	dw	deviant3_248
	dw	deviant3_249
	dw	deviant3_250
	dw	deviant3_251
	dw	deviant3_252
	dw	deviant3_253
	dw	deviant3_254
	dw	deviant3_255

