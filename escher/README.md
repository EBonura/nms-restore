# escher: a PS1 3D view for QD Up (new work)

QD Up's September 1996 source has no PS1 3D view in the game. (NMS did write a PS1 3D table viewer in spring and summer 1996: `GLENN/POOL.C`, commenced 29/04/96, loads `models/pmd/uktable2.pmd` and draws it with `RotPMD`; `GLENN/POOL.CPE` and `PPP.CPE` are builds of it. It has no game logic and was dropped for the 2D view. This crate is separate new work.) Every frame `Render` (MY3DS.C) draws the 2D top-down debug view (`DrawScreen`, GFX.C) and then calls `draw_3d` (STV3D.C), the PC version's calls into NMS's 3D engine Escher; on the PS1 every Escher render and projection call is an empty stub (ENGINE.C), and only the loader exists (ESCHER.S: `E_Init`, `E_AddMesh`, `E_Load`, `E_GetObjectPtr`). The PC renderer itself is lost: `PINDOS.EXE` in the archive is a DOS pinball game (Road King table editor), not QD Up, and holds no Escher code.

This crate is a renderer written for the restoration behind NMS's Escher interface. It exports the E_* functions ENGINE.C stubs, with the C signatures ESCHER.H and ENGINE.C give them, so `draw_3d` and the rest of NMS's camera, cue and ball code drive it unchanged. It is not NMS's Escher; where it has to guess how Escher behaved, the guess is written down below and in the code.

`qdup/build.sh` (or `qdup/disc.sh`) links it in place of ENGINE.C. It is a library crate that `psyq/` depends on, not a static library of its own: `psyq/build.sh` makes the one `libpsyq.a` that holds the layer, this renderer and each SDK crate once (a second static library would embed its own copy of every SDK crate it uses, and two copies of an SDK crate whose assembly is a global symbol do not link). The game keeps NMS's 2D view too: `E_SetView3D` (new for the PS1, not an Escher call) switches the render calls on and off, and the game picks the view on the choice screen after START (`qdup/new/viewchoice.c`).

## Data formats

All little-endian 32-bit ints unless said. World units: 256000 to the metre (the game's `FIX24_TO_WORLD`), y up. Angles: 4096 to a turn.

**ESH** (mesh; read by ESCHER.S `E_Load`, layouts in ESCHER.H): `int num_objects`, then per object `char name[16]`, `int radius`, `VERTEX source_pos`, `int num_vertices`, `VERTEX vertex[num_vertices]` (object-local), `int first_extra_vertex`, `int num_lines`, `LINE line[num_lines]` (56 bytes: `vertex[2]`, `plane[2]` (-1 none), `orientation[2]`, `start_u[2]`, `start_v[2]`, `end_u[2]`, `end_v[2]`), `int num_planes`, `PLANE plane[num_planes]` (48 bytes: `char axis` (0 x, 1 y, 2 z), `plane_type` (0 plain, 1 texture, 2 chrome, 3 tiled), `visible_side` (2: seen from the + side of `axis`, 1: from the -), `current_side`; `int colour` (palette index in all four bytes), `brightness` (0..31), `tile_info`, `gradient[2]`, `offset`, `radius`, `centre_vertex`, `priority`, `vertex_start`, `num_vertices`), `int num_plane_vertices`, `short plane_vertex[]`. POTTERUS.ESH: 80 objects (room, table), BALLSCUE.ESH: 46 (balls' spots, stripes, numbers, the cue, dummies).

A plane's `plane_vertex` run lists its vertices but not always around it (160 of the Potters table's 702 planes are out of order); the outline comes from chaining the LINEs that border the plane. Every outline closes into one loop with exactly the plane's vertex count except `cuespot`'s (its list adds the disc's centre). 71 outlines are concave (the number glyphs, the fruit machines' sides, the door window, and the stripes' near-collinear bands).

**EMP** (map data; `E_LoadMapData`, same base name as the ESH): `int num_objects`, then per object `char name[16]`, `int num_lines`, `num_planes`, `priority`; `int colour[num_planes]`, `brightness[num_planes]`, `tile_info[num_planes]`; `int uv[num_lines][8]`; `char plane_type[num_planes]`. Found from the files: POTTERUS.EMP's 80 records match POTTERUS.ESH object for object (all planes plain, colour and baked brightness per plane, the object priorities NMS's painter sorts by). BALLSCUE.EMP (May 1996) predates BALLSCUE.ESH (July): its colours are running debug numbers and some type bytes are ASCII; the game sets the balls' colours itself (GetBallPtrs). The cue's colours are in other files: `POTTRUS3.ESH` (9 May) and `SQUARE.ESH` (29 Apr) hold a `cue` object with the same counts and byte-identical line records as BALLSCUE.ESH's, and `POTTRUS3.EMP` and `SQUARE.EMP` give 28 per-plane colours (brightness 31), which the 3D build applies (STV3D.C). Caveat: POTTRUS3.EMP predates POTTBLUE.PAL (27 May). `POTTERUK.ESH`/`.EMP` (the UK table, played in UK pool) load the same way as the US table.

**Tables**: BRIGHT.TAB, 32 rows of 256 palette indices (row = brightness); POTTBLUE.PAL, the room palette (VGA 6-bit RGB); POOLPAL.TAB (BLUEPAL.TAB for Potters), 11 x 256 indices, block 10 (`poolptr[2560 + c]`) the ball colour ramps: 16 entries a colour, 0..7 the colour shading into the felt, 8..15 the colour to white; COSINE.TAB, 4096 cosines in 16.16 (E_Init loads it).

`tools/qdup_esh_view.py` reads all of this and renders a z-buffered reference still.

## How it draws

- Colour: flat per plane, `palette[BRIGHT.TAB[brightness][colour]]`, as Escher's PC planes were coloured (no lighting is computed: the brightness is baked into the EMP).
- Transform: on the GTE. Each object's rotation and translation go into RT/TR (world units / 128, so a 12 m room fits the GTE's 16-bit inputs and depth), vertices three at a time through RTPT. `E_RotateObject` turns a mesh with MVMVA.
- Order: Escher was a painter, and `draw_3d` orders its render calls for it (room and far cushions, then each ball far to near, then near cushions and the cue). Each E_RenderToBuffer call here takes the next band of the game's own ordering table (`cdl->ot`, 4096 slots), so the GPU draws the calls in the order the game made them; inside a band the slots run by object priority (higher first), then by depth. E_DrawBall takes one slot of its own.
- Clipping: a polygon the GTE flagged (behind the camera, saturated) or one too big for the GPU is clipped in camera space against the near plane and a guard band, then projected on the CPU; crossings are computed from each edge's ends in one order and kept vertices use the GTE's projection, so neighbours meet without cracks. Others go to the GPU as the GTE projected them, after a screen-area facing test.
- Balls: `E_DrawBall` is a 64x64 4-bit disc texture (shade steps toward a highlight) with one CLUT per ball colour from the pool table ramps; shadows use a flat CLUT.
- 2D lines (`E_Draw2DLine`, the head string, tracking lines, camera cross) go into the same band, at the near end of their sort value's rank.
- Memory: E_Init's two edge buffers (152000 + 16000 bytes inside the game's 512 KB model memory) were the PC span renderer's and are unused on the PS1; this renderer keeps its plane table, index pool, pool table, vertex scratch, line queue and two 62 KB primitive buffers (one per display page) there.
- VRAM: the disc texture at (640,0), 16x64 halfwords, the CLUTs at (640,64..83); the game uses 0..319 x 0..511 (two 320x256 pages), the 2D ball sprite and CLUT at (320,0) and (320,511), the debug font at (960,256).
- 32-bit integer maths only; no floating point anywhere in the crate.

## Readings of Escher (guesses, marked in the code)

- Rotation `(x, y, z)` = Ry(y) Rx(x) Rz(z), Rx raising -z toward +y: a camera pitched by x sits above its target (PutCamera), a cue raised by x lifts its butt (PositionCue).
- Projection: `E_SetZoom(zoom * 65536 / 320)` read as a focal length of the zoom value times half the screen width (564 pixels for the game's 1128 at 320 wide). With it the game's fixed cameras frame what they are for (the overhead camera puts the 2.6 m table across 295 of 320 pixels, the corner and computer cameras show all of it); with the full width they each show part of the table. The PC's 320x200 picture (centre 160,145, pixel aspect 54613) is mapped onto the PS1's 320x256.
- E_GetScreenPos answers in the PC picture's sixteenths of a pixel, E_DrawBall and E_Draw2DLine take eighths (x) as STV3D.C uses them.
- E_EaseCamera(n): 1/n of the way per E_UpdateCamera, snapping the last few units.
- E_Draw2DLine's colour c: the base of ball-colour ramp c (9: the shadow ramp, the dark line on the felt).
- Planes with brightness 0 (no EMP record) take BRIGHT.TAB's dark row; the cue ball's aiming spot (`cuespot`, whose EMP record does not match its mesh) is drawn black.

## Measured (retail emulator, CD build, route log)

The emulator's vblank (route tick) here is 571,236 CPU cycles (59.3 Hz); frame rates below are per vblank. Window counts are display flips (`display_start_changed`).

- Aiming, cue still: 2.38 vblanks per frame (24.9 frames/s at 59.3 Hz, 21.0 at 50 Hz), the same cadence as the 2D build. With the render calls switched off (same build) also 2.38: the renderer fits in the loop's existing slack.
- Turning the aim (Left held): 2.35 to 2.50 vblanks per frame.
- Cue ball rolling to the rack: 3.0 vblanks per frame; balls rolling after the break: 2.4 to 3.7. The break's collisions: 25 to 50 vblanks per frame for about 4 seconds, bound by the physics (`update_all_balls` runs up to 32 steps per game loop to catch up; FRAMES on the overlay); the 2D build shows the same stretch as slow motion because each physics step there draws a 2D frame. With FIXBALL.C and FIXMATH.C compiled at -O2 (the qdup-play branch) the longest gap between displayed frames in the break is 6 route ticks in the 3D build (73 before) and 5 in the 2D build (17 before), same physics.
- Computer turn (R2 at the title, a restoration switch since removed): 2.9 to 4.2 while it aims and its shot rolls.
- Cost: the renderer's code takes about 464,000 cycles a frame while aiming (0.81 vblank; the scene is about 110 primitives, 33 objects, 535 vertices, 1328 ordering-table slots), out of a 2.38-vblank loop that also spends 162,000 cycles in NMS's ResetObjectRotation (-O0) and 161,000 in FntFlush waiting for the GPU.

RAM (2 MB): static data ends at 0x80117808 (2D build 0x8010FD98: +31,344 bytes, the renderer's code 27 KB and statics 3.6 KB); the heap peaks at 782,928 bytes (+8 KB brightness table) in game; the stack's low-water mark is the same as the 2D build's (15,400 bytes). 153,728 bytes stay free between them (2D build: 193,264).

The frames and route logs behind these figures (reference still against the PS1 frame, title, aiming, a break, a computer turn, the L2 2D view, a high-detail experiment) are not in this repository; `tools/qdup_run.py` and `tools/qdup_esh_view.py` make them again.

## Open problems

- The break no longer freezes the picture (above); the shot loop is unchanged.
- Detail level: the game starts at low detail (`detail_level` 0: a black box round the table). The room (high detail) renders, checked with a temporary build: 2.38 vblanks per frame while aiming, as low detail, 3.5 while the balls roll after a break. No PS1 button reaches the PC's F9 toggle.
- The aiming spot (`cuespot`) is drawn black. Both older EMPs give its two planes colour 8 (blue in POTTBLUE.PAL), but their cuespot mesh has 2 planes and 32 lines against BALLSCUE.ESH's 1 and 16, so the evidence is weak and it is left as it was.
- Not checked on a console.
