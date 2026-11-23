// Destination is offset into screen

void screen_dump_line(char *source,int dest,int width)
{
	char *vga=(char *) screen_lin_addr;
	int x;
	int destmod=dest&0xffff;

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;						// distance to band change
		memcpy(vga+destmod,source,x);		// copy to band change
		currentband++;							// increment band
		setvesaband(currentband);				// set new band
		memcpy(vga,source+x,width-x);			// copy remainder
	}
	else memcpy(vga+destmod,source,width);
}	

void screen_set_line(long dest,long width,ubyte colour)
{
	char *vga=(char *) screen_lin_addr;
	long x;
	long destmod=dest&0xffff;

	if(width==0) { currentband=-1; return; }

	if((dest>>16)!=currentband)
	{
		currentband=dest>>16;
		setvesaband(currentband);
	}
	if(((dest+width)>>16)!=currentband)
	{
		x=65536-destmod;						// distance to band change
		memset(vga+destmod,colour,x);		// copy to band change
		currentband++;							// increment band
		setvesaband(currentband);				// set new band
		memset(vga,colour,width-x);			// copy remainder
	}
	else
		memset(vga+destmod,colour,width);
}	

; UNIVBE Version

EXTRN setBank:near
EXTRN _VBE20_setBankA:near

; call with ax = band
	public asm_set_band_		; for Paul from piss 'C'
asm_set_band_ proc near
	push	eax

	call	setBank

	pop	eax
	ret
asm_set_band_ endp

; call with ax = band
	public set_band_
set_band_ proc near
	pushad

	IFNDEF	SAFE_MODE
	mov	dx,3ceh				; straight to Cirrus
	mov	al,9
	shl	bl,4
	mov	ah,bl
	out	dx,ax
	ELSE

	movzx	eax,bx				; standard VESA (must set vesagran first)
	mov	edx,64
	mul	edx
	mov	ecx,vesagran
	div	ecx

	lea	esi,RRegs
	mov	[esi].Real_EAX,00004f05h
	mov	[esi].Real_EBX,00000000h
	mov	[esi].Real_EDX,eax

	lea	edi,RRegs
	mov	ax,300h
	mov	bx,10h
	mov	cx,0
	int	31h

	ENDIF

	popad
	ret
set_band_ endp

	public set_vga_address_
set_vga_address_ proc near
	pushad

	IFDEF	SAFE_MODE
	; ... VESA + UNIVBE

	lea	esi,RRegs					
	mov	[esi].Real_EAX,4f07h
	mov	[esi].Real_EBX,80h	; Wait VBI
	mov	[esi].Real_ECX,ecx	; Screen X
	mov	[esi].Real_EDX,edx	; Screen Y
	
	lea	edi,RRegs
	mov	ax,300h
	mov	bx,10h
	mov	cx,0
	int	31h

	Comment ~

	mov  dx,3dah ;port    ; say, where's the 6845?
;	add  dx,6            ; locate the status register
@@vrdly3:
	in   al,dx           ; loop until vertical retrace is off
	test al,8            ; ...
	jnz  @@vrdly3          ; ...
	
@@vrdly4:
	in   al,dx           ; now loop until it's on!
	test al,8            ; ...
	jz   @@vrdly4          ; ...
	popad
	ret

	pushad
	mov  dx,3dah ;port    ; say, where's the 6845?
;	add  dx,6            ; locate the status register
@@vrdly1:
	in   al,dx           ; loop until vertical retrace is off
	test al,8            ; ...
	jnz  @@vrdly1          ; ...
	mov	eax,[esi].Real_ECX
	mov	eax,ecx
	call	pan4_

@@vrdly2:
	in   al,dx           ; now loop until it's on!
	test al,8            ; ...
	jz   @@vrdly2          ; ...
	popad

	~
	
	ELSE

	Comment ~

;	mov	eax,[esi].Real_ECX
	mov	eax,[esi].Real_EAX
	movzx	eax,ah
	mov	return_eax,eax


	pushad
	mov  dx,3dah ;port    ; say, where's the 6845?
;	add  dx,6            ; locate the status register
@@vrdly1:
	in   al,dx           ; loop until vertical retrace is off
	test al,8            ; ...
	jnz  @@vrdly1          ; ...
	mov	eax,[esi].Real_ECX
	call	pan4_

@@vrdly2:
	in   al,dx           ; now loop until it's on!
	test al,8            ; ...
	jz   @@vrdly2          ; ...
	popad
	~

	push	ecx

	; Cirrus Logic Hardware version
	mov	eax,edx
	mov	ebx,160
	mul	ebx
	mov	ebx,eax
	mov	eax,ecx
	shr	eax,2
	add	ebx,eax

; Wait for display enable to be active (active low), to be sure that
; both halves of the start address will take place in one frame. We
; preload a few values here to save time after the DE has been
; detected.

	mov     cl,0Ch              ; CL := Start Address High register
	mov     ch,bh               ; CH := high byte of new address
	mov     bh,bl               ; BH := low byte of new address
	mov     bl,0Dh              ; BL := Start Address Low register
	mov     dx,03DAh            ; DX := video status port

@@WaitDEVGA:
	in      al,dx
	test    al,1
	jnz     @@WaitDEVGA         ; Wait for Display Enable

	cli

	mov     dx,03D4h            ; DX := CRTC I/O port (3D4h)
	mov     ax,bx
	out     dx,ax
	mov     ax,cx
	out     dx,ax


	rol	ebx,16
	mov	bh,bl
	and	bh,01h

	shl	bl,1
	and	bl,0ch
	or	bl,bh

	mov	dx,3d4h ;3ceh
	mov	al,1bh
	out	dx,al
	inc	dx
	in	al,dx
	and	al,0f2h
	or	al,bl
	out	dx,al
	sti

	pop	eax
	call	pan4_

; Now wait for the start of the vertical sync, to ensure that the old
; page will be invisible before anything is drawn on it.
@@Done:
	mov     dx,03DAh            ; DX := video status port
@@WaitStartVert:
	in      al,dx               ; Wait for start of vertical retrace
	test    al,8
	jz      @@WaitStartVert

	ENDIF

	popad
	mov	eax,return_eax
	ret
set_vga_address_ endp


