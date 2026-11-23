	section	.rdata  
	section	.text   
	section	.data   
	section	.sdata  
	
	section	.sbss	; The bss sections
	section	.bss

	section	.text


	global	E_GetObjectPtr
	global	E_Init
	global	E_Load
	global	E_AddMesh
; RESTORE: the loader below reads through FILE_IO.C's FileOpen, FileRead
; and FileClose (the PC link, or the CD in a disc build); it called PCopen,
; PCread and PCclose
	global	FileOpen
	global	FileRead
	global	FileClose
; RESTORE: the loader's object list, mesh names, cosine table and the PC
; renderer's edge buffers are exported for the PS1 3D renderer written for
; the restoration (escher/, the 3D view; not NMS code).  Nothing here
; changes: these lines only make the labels visible to the linker.
	global	objects
	global	num_objects
	global	mesh_names
	global	mesh_name_ptr
	global	cosine
	global	edge_mem
	global	edge_start_mem

object_static		= 0
object_mobile		= 1
object_dummy		= 2
object_ball		= 3
object_mobile_compound	= 4

	; setup for VERTICES
	rsreset
vx	rb	4
vy	rb	4
vz	rb	4
sizeofvertex	rb	0
	
	; setup for LINE
	rsreset
vertex0	rb	4
vertex1	rb	4
plane0	rb	4
plane1	rb	4
orientation0	rb	4
orientation1	rb	4
start_u1	rb	4
start_u2	rb	4
start_v1	rb	4
start_v2	rb	4
end_u1	rb	4
end_u2	rb	4
end_v1	rb	4
end_v2	rb	4
sizeofline	rw	0

	; setup for PLANE
	rsreset
axis	rb	1
plane_type	rb	1
visible_side	rb	1
current_side	rb	1
material	rb	4
brightness	rb	4
tile_info	rb	4
gradient0	rb	4
gradient1	rb	4
plane_offset	rb	4
pradius	rb	4
centre_vertex	rb	4
ppriority	rb	4
vertex_start	rb	4
pnum_vertices	rb	4
sizeofplane	rw	0

	; setup for OBJECT
	rsreset
num_vertices	rb	4
first_extra_vertex	rb	4
num_lines	rb	4
num_planes	rb	4
radius	rb	4
priority	rb	4
num_plane_vertices	rb	4
first_sorted_plane	rb	4
last_sorted_plane	rb	4
source_pos	rb	sizeofvertex
modified_pos	rb	sizeofvertex
world_pos	rb	sizeofvertex
rotation	rb	sizeofvertex
source_vertex	rb	4
modified_vertex	rb	4
world_vertex	rb	4

lines	rb	4
planes	rb	4
plane_vertex	rb	4

object_name	rb	16
colour	rb	1
visible	rb	1
selected	rb	1
sort_type	rb	1
enabled	rb	1
object_type	rb	1

sizeofobject	rw	0

	; setup for EDGE
	rsreset
start_line	rb	4
end_line	rb	4
x_low	rb	4
x_high	rb	4
gradient_low	rb	4
gradient_high	rb	4
eplane0	rb	4
eplane1	rb	4
eu1	rb	4
eu2	rb	4
ev1	rb	4
ev2	rb	4
du1	rb	4
du2	rb	4
dv1	rb	4
dv2	rb	4
eorientation0	rb	1
eorientation1	rb	1
used	rb	1
pad	rb	1
previous	rb	4
next	rb	4
sizeofedge	rw	0

	; setup for EDGE_START
	rsreset
edge_address	rb	4
enext	rb	4
sizeofedgestart	rw	0

	; setup for PLANE_DATA
	rsreset
x0	rb	4
x1	rb	4
z	rb	4
line_num	rb	4
pad2	rb	1
mark	rb	1
pplane_type	rb	1
ppad	rb	1
start_u	rb	4
end_u	rb	4
start_v	rb	4
end_v	rb	4
materials	rb	4
pbrightness	rb	4
ptile_info	rb	4
dword_pad	rb	12
pobject_type	rb	1
byte_pad	rb	3
sizeofplanedata	rw	0

	; setup for BALL
	rsreset
bx	rb	4
by	rb	4
bz	rb	4
bradius	rb	4
sizeofball	rw	0

	; setup for ZONELINE
	rsreset
zleft	rb	4
zright	rb	4
znext	rb	4
zprevious	rb	4
sizeofzoneline	rw	0

	; setup for CHUNK
	rsreset
ram_address	rb	4
vga_address	rb	4
chunk_length	rb	4
cnext	rb	4
sizeofchunk	rw	0

	; setup for SORTORDER
	rsreset
sort_value	rb	4
sobject_name	rb	16
sizeofsortorder	rw	0

	; setup for UVCOORD
	rsreset
ustart_u1	rb	4
ustart_u2	rb	4
ustart_v1	rb	4
ustart_v2	rb	4
uend_u1	rb	4
uend_u2	rb	4
uend_v1	rb	4
uend_v2	rb	4
sizeofuvcoord	rw	0

	; setup for LINE2D
	rsreset
lx1	rb	4
ly1	rb	4
lx2	rb	4
ly2	rb	4
lcolour	rb	4
lpriority	rb	4
sizeofline2d	rw	0

	; setup for BALLDATA
	rsreset
hilite_on_off	rb	4
shape_scale	rb	4
ball_colour	rb	4
x_8	rb	4
y_8	rb	4
radius_8	rb	4
top_8	rb	4
bottom_8	rb	4
btop	rb	4
height_8	rb	4
delta_table	rb	4
table_index	rb	4
line_width_8	rb	4
line_addr	rb	4
line_end_addr	rb	4
buffer_address	rb	4
left_8	rb	4
right_8	rb	4
last_left_8	rb	4
last_right_8	rb	4
delta_colour	rb	4
left_colour	rb	4
right_colour	rb	4
middle_colour	rb	4
hilite_left_8	rb	4
hilite_right_8	rb	4
last_line	rb	4
oric_radius_8	rb	4
active	rb	4
flag	rb	4
bbz	rb	4
bvisible	rb	4
y_position	rb	4
sizeofballdata	rw	0

	
	section	.data

cosine_filename	db	'cosine.tab',0
mem_heap	dw	0
mem_used	dw	0
edge_mem	dw	0
edge_start_mem	dw	0

total_planes	dw	0
plane_data_mem	dw	0

mesh_name_ptr	dw	0
file_handle	dw	0
objects	dw	0
objects_ptr	dw	0
num_objects	dw	0
total_objects	dw	0
temp	dw	0

cosine	dcw	4096,0
mesh_names	dcb	1024,0

	section	.text

;
; Compare 2 AsciiZ Strings pointed to by A0 and A1
; returns 1 if strings match
;------------------------------------------------------
; IN:  A0 = ptr to name1
;      A1 = ptr to name2
; OUT: V0 = 1 if match else 0
;------------------------------------------------------
StringCompare:	ori	v0,zero,1
	li	t0,$2a
	li	t1,$3f
@loop	lbu	a2,(a0)
	lbu	a3,(a1)
	beq	a2,t0,@done
	addiu	a0,a0,1
	beq	a2,t1,@loop
	addiu	a1,a1,1
	beq	a2,a3,@match
	nop
	j	@done
	move	v0,zero
@match	or	a2,a2,a3
	bne	a2,zero,@loop
	nop
@done	jr	ra
	nop
;------------------------------------------------------
; IN: A0 = ptr to name
;------------------------------------------------------
SelectObjectSub:	add	sp,sp,-12
	sw	ra,0(sp)
	sw	s0,4(sp)
	sw	s1,8(sp)
	move	s0,a0
	lw	v1,objects
	lw	s1,num_objects
	move	t2,zero
@search_loop	bgt	t2,s1,@done
	nop
	la	a1,object_name(v1)
	nop
	jal	StringCompare
	move	a0,s0
	sb	v0,selected(v1)
	ori	a0,zero,sizeofobject
	add	v1,v1,a0
	j	@search_loop
	add	t2,t2,1
@done	lw	ra,0(sp)
	lw	s0,4(sp)
	lw	s1,8(sp)
	jr	ra
	add	sp,sp,12
;--------------------------------------------------------------------------
; This routine takes an asciiz string and returns a ptr to the object of
; that name
;------------------------------------------------------
; IN:  A0 = asciiz name
; OUT: V0 = object ptr (null if object not found)
;------------------------------------------------------
E_GetObjectPtr:	add	sp,sp,-12
	sw	ra,0(sp)
	sw	s0,4(sp)
	sw	s1,8(sp)
	move	s0,a0
	lw	v1,objects
	lw	s1,num_objects
	move	t2,zero
@search_loop	bgt	t2,s1,@not_found
	move	v0,zero
	la	a1,object_name(v1)
	jal	StringCompare
	move	a0,s0
	bne	v0,zero,@found_object
	ori	a0,zero,sizeofobject
	add	v1,v1,a0
	j	@search_loop
	add	t2,t2,1
@found_object	move	v0,v1
@not_found	lw	ra,0(sp)
	lw	s0,4(sp)
	lw	s1,8(sp)
	jr	ra
	add	sp,sp,12
;--------------------------------------------------------------------------
; This routine initialises the memory heap with the size passed
;
; IN: A0 = heap address
;     A1 = length
;--------------------------------------------------------------------------
E_Init:	add	sp,sp,-4
	sw	ra,(sp)
	add	a0,a0,3
	and	a0,a0,$fffffffc		; long word align
	sub	a1,a1,8
	sw	a0,mem_heap			; point heap here
	sw	a1,(a0)			; write first heaer
	add	a0,a0,a1
	li	a2,-1
	sw	a2,(a0)
	add	a0,a0,4			; move to end of block

	li	a0,2000*sizeofedge
	jal	AllocMem
	nop
	sw	v0,edge_mem

	li	a0,2000*sizeofedgestart
	jal	AllocMem
	nop
	sw	v0,edge_start_mem

	la	a0,cosine_filename
	jal	OpenFile
	nop
	la	a0,cosine
	jal	ReadFile
	li	a1,4096*4

	jal	CloseFile
	nop

	la	a0,mesh_names
	sw	a0,mesh_name_ptr
	lw	ra,(sp)
	sw	zero,total_objects
	add	sp,sp,4
	jr	ra
	nop
;--------------------------------------------------------------------------
; This routine adds the mesh name into a list and reads the number of objects
; in the mesh and adds it to the total object count.
;--------------------------------------------------------------------------
; IN:  A0 = pointer to asciiz filename
;--------------------------------------------------------------------------
E_AddMesh:	add	sp,sp,-4
	lw	a1,mesh_name_ptr
	sw	ra,(sp)
	move	t0,a0
	move	t1,a1
@name_copy_loop	lbu	v0,(t0)
	add	t0,t0,1
	sb	v0,(t1)
	bne	v0,zero,@name_copy_loop
	add	t1,t1,1
	add	a1,a1,64
	sw	a1,mesh_name_ptr

	jal	OpenFile
	nop

	la	a0,temp
	jal	ReadFile
	li	a1,4

	jal	CloseFile
	nop

	lw	a0,temp
	lw	a1,total_objects
	nop
	add	a0,a0,a1
	sw	a0,total_objects
	nop

@exit	lw	ra,(sp)
	nop
	jr	ra
	add	sp,sp,4
;--------------------------------------------------------------------------
; This routine loads the meshes into memory, allocating chunks as it goes
; along
;--------------------------------------------------------------------------
E_Load:	add	sp,sp,-12
	sw	ra,0(sp)
	sw	s7,4(sp)
	sw	s6,8(sp)
	lw	a0,total_objects
	li	a1,sizeofobject

; allocate space for object structures

	mul	a0,a0,a1
	jal	AllocMem
	nop
	sw	v0,objects
	sw	v0,objects_ptr
	lw	s6,mesh_name_ptr
	la	s7,mesh_names

; now loop through the mesh list and load in the meshes

@mesh_loop	beq	s7,s6,@done_all_meshes
	nop

; open the mesh file

	jal	OpenFile
	move	a0,s7

; Read the number of objects

	la	a0,num_objects
	jal	ReadFile
	li	a1,4

; now loop through and read in objects

 	lw	t0,objects_ptr
	sw	zero,temp
@object_loop	ori	t1,zero,1
	li	t2,$10000000
	li	t3,object_static

	sb	zero,selected(t0)
	sb	t1,visible(t0)
	sw	t2,priority(t0)
	sb	zero,enabled(t0)
	sb	t3,object_type(t0)

; set the object to 0 rotation
	
	sw	zero,rotation+vx(t0)
	sw	zero,rotation+vy(t0)
	sw	zero,rotation+vz(t0)

; Read object name

	la	a0,object_name(t0)
	jal	ReadFile
	ori	a1,zero,16

; radius

	la	a0,radius(t0)
	jal	ReadFile
	ori	a1,zero,4

; start position

	la	a0,source_pos(t0)
	jal	ReadFile
	ori	a1,zero,sizeofvertex

; read vertices, start with num vertices

	la	a0,num_vertices(t0)
	jal	ReadFile
	ori	a1,zero,4

; now allocate space for source, modified and world vertex lists

	lw	a0,num_vertices(t0)
	li	t1,sizeofvertex
	add	a0,a0,3
	mul	a0,a0,t1

	jal	AllocMem
	move	t1,a0
	sw	v0,source_vertex(t0)

	jal	AllocMem
	move	a0,t1
	sw	v0,modified_vertex(t0)

	jal	AllocMem
	move	a0,t1
	sw	v0,world_vertex(t0)

; now read vertex list into source_vertex

	lw	a0,num_vertices(t0)
	li	t1,sizeofvertex
	mul	a1,a0,t1
 	lw	a0,source_vertex(t0)
	jal	ReadFile
	nop

; now read first extra vertex

	la	a0,first_extra_vertex(t0)
	jal	ReadFile
	ori	a1,zero,4

; Read lines

	la	a0,num_lines(t0)
	jal	ReadFile
	ori	a1,zero,4

	lw	a0,num_lines(t0)
	li	t1,sizeofline
	mul	a0,t1,a0

	jal	AllocMem
	move	a1,a0
	sw	v0,lines(t0)
	jal	ReadFile
	move	a0,v0

; Read planes

	la	a0,num_planes(t0)
	jal	ReadFile
	ori	a1,zero,4

	lw	a0,num_planes(t0)
	li	t1,sizeofplane
	sw	a0,total_planes
	mul	a0,t1,a0

	jal	AllocMem
	move	a1,a0
	sw	v0,planes(t0)
	jal	ReadFile
	move	a0,v0

; Read plane-vertex translation table

	la	a0,num_plane_vertices(t0)
	jal	ReadFile
	ori	a1,zero,4

	lw	a0,num_plane_vertices(t0)
	nop
	sll	a0,a0,1

	jal	AllocMem
	move	a1,a0
	sw	v0,plane_vertex(t0)
	jal	ReadFile
	move	a0,v0
 	
; and now loop back for more perhaps

	add	t0,t0,sizeofobject
	lw	a0,temp
	sw	t0,objects_ptr
	addiu	a0,a0,1
	lw	a1,num_objects
	sw	a0,temp
	bne	a0,a1,@object_loop
	nop

; we have done this mesh. now close the file and loop back for next one

	jal	CloseFile
	nop

	j	@mesh_loop
	add	s7,s7,64

; now allocate mem for plane_data table

@done_all_meshes	lw	a0,total_planes
	nop
	add	a0,a0,1024
	li	t1,sizeofplanedata
	mul	a0,a0,t1

	jal	AllocMem
	nop
	sw	v0,plane_data_mem

; Place all objects at their default position

	jal	PlaceObjects

; now set num_objects to total_objects

	lw	v0,total_objects
	lw	ra,0(sp)
	sw	v0,num_objects

	lw	s7,4(sp)
	lw	s6,8(sp)		; RESTORE: was in the jr's delay slot, and sp
	jr	ra			; was never given back its 12 bytes: the caller came
	add	sp,sp,12		; back with sp 12 low (a -O2 init_3d returned to 0x61)
;--------------------------------------------------------------------------
; This routine places all the objects to their default positions by copying
; source_pos into modified_pos and copying the source_vertex list into
; modified_vertex
;--------------------------------------------------------------------------
PlaceObjects:	add	sp,sp,-12
	sw	ra,0(sp)
	lw	a0,objects
	move	t7,zero			; RESTORE: was "move zero,t7" (operands swapped), so the
					; object count started from whatever t7 held
@object_loop	lw	t6,total_objects
	nop
	bge	t7,t6,@done_objects
	nop
	la	t0,source_pos(a0)
	la	t1,modified_pos(a0)

; Copy the objects position

	lw	a1,vx(t0)
	lw	a2,vy(t0)
	lw	a3,vz(t0)

	sw	a1,vx(t1)
	sw	a2,vy(t1)
	sw	a3,vz(t1)

; Now go through the source_vertex list and copy it into modified_vertex

	lw	v1,num_vertices(a0)
	nop
	beq	v1,zero,@done_vertices
	nop
	lw	t0,source_vertex(a0)
	lw	t1,modified_vertex(a0)

@vertex_loop	lw	a1,vx(t0)
	lw	a2,vy(t0)
	lw	a3,vz(t0)

	sw	a1,vx(t1)
	sw	a2,vy(t1)
	sw	a3,vz(t1)
	
	li	a1,sizeofvertex
	add	t0,t0,a1

	sub	v1,v1,1
	bne	v1,zero,@vertex_loop
	add	t1,t1,a1

@done_vertices	add	t7,t7,1
	j	@object_loop
	add	a0,a0,sizeofobject

@done_objects	lw	ra,0(sp)
	add	sp,sp,12
	jr	ra
	nop
;--------------------------------------------------------------------------
; This routine allocates a block of memory from the heap
;--------------------------------------------------------------------------
; IN:  A0 = size required
; OUT: V0 = address of mem (0 if fail)
;--------------------------------------------------------------------------
AllocMem:	lw	v1,mem_used
	add	a0,a0,3
	and	a0,a0,$fffffffc
	add	v1,v1,a0
	lw	v0,mem_heap
	sw	v1,mem_used
	move	v1,v0
	add	v1,v1,a0
	sw	v1,mem_heap
	jr	ra
	nop
;--------------------------------------------------------------------------
; IN:  A0 = ptr to filename
; updates variable file_handle
;--------------------------------------------------------------------------
; RESTORE: OpenFile, ReadFile and CloseFile call FILE_IO.C's FileOpen, FileRead
; and FileClose (NMS called PCopen, PCread and PCclose).  Those are C, so the
; frames below keep the 16-byte argument area the o32 calling convention lets
; a C function store its arguments in (FileOpen storing a0 there overwrote
; ra), and save t0-t9 around the call: E_Load keeps pointers in t0..t3
; across ReadFile, which PCread left alone and C code need not.
OpenFile:	add	sp,sp,-68
	sw	ra,16(sp)
	sw	a1,20(sp)
	sw	a2,24(sp)
	sw	t0,28(sp)
	sw	t1,32(sp)
	sw	t2,36(sp)
	sw	t3,40(sp)
	sw	t4,44(sp)
	sw	t5,48(sp)
	sw	t6,52(sp)
	sw	t7,56(sp)
	sw	t8,60(sp)
	sw	t9,64(sp)
	move	a1,zero
	jal	FileOpen		; RESTORE: was PCopen
	move	a2,zero
	sw	v0,file_handle
	lw	t0,28(sp)
	lw	t1,32(sp)
	lw	t2,36(sp)
	lw	t3,40(sp)
	lw	t4,44(sp)
	lw	t5,48(sp)
	lw	t6,52(sp)
	lw	t7,56(sp)
	lw	t8,60(sp)
	lw	t9,64(sp)
	lw	ra,16(sp)
	lw	a1,20(sp)
	lw	a2,24(sp)
	jr	ra
	add	sp,sp,68
;--------------------------------------------------------------------------
; IN:  A0 = ptr to filename
;      A1 = length
;--------------------------------------------------------------------------
ReadFile:	add	sp,sp,-68		; RESTORE: was -8, ra and a2 at 0 and 4
	sw	ra,16(sp)
	beq	a1,zero,@nolen
	sw	a2,20(sp)
	sw	t0,28(sp)
	sw	t1,32(sp)
	sw	t2,36(sp)
	sw	t3,40(sp)
	sw	t4,44(sp)
	sw	t5,48(sp)
	sw	t6,52(sp)
	sw	t7,56(sp)
	sw	t8,60(sp)
	sw	t9,64(sp)
	move	a2,a1
	move	a1,a0
	lw	a0,file_handle
	jal	FileRead		; RESTORE: was PCread
	nop
	lw	t0,28(sp)
	lw	t1,32(sp)
	lw	t2,36(sp)
	lw	t3,40(sp)
	lw	t4,44(sp)
	lw	t5,48(sp)
	lw	t6,52(sp)
	lw	t7,56(sp)
	lw	t8,60(sp)
	lw	t9,64(sp)
@nolen	lw	ra,16(sp)
	lw	a2,20(sp)
	jr	ra
	add	sp,sp,68
;--------------------------------------------------------------------------
CloseFile:	add	sp,sp,-68		; RESTORE: was -8, ra and a0 at 0 and 4
	sw	ra,16(sp)
	sw	a0,20(sp)
	sw	t0,28(sp)
	sw	t1,32(sp)
	sw	t2,36(sp)
	sw	t3,40(sp)
	sw	t4,44(sp)
	sw	t5,48(sp)
	sw	t6,52(sp)
	sw	t7,56(sp)
	sw	t8,60(sp)
	sw	t9,64(sp)
	lw	a0,file_handle
	jal	FileClose		; RESTORE: was PCclose
	nop
	lw	t0,28(sp)
	lw	t1,32(sp)
	lw	t2,36(sp)
	lw	t3,40(sp)
	lw	t4,44(sp)
	lw	t5,48(sp)
	lw	t6,52(sp)
	lw	t7,56(sp)
	lw	t8,60(sp)
	lw	t9,64(sp)
@nolen	lw	ra,16(sp)
	lw	a0,20(sp)
	jr	ra
	add	sp,sp,68

