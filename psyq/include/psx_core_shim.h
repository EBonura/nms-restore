/* RESTORE: PLATFORM. The seam between the layer and the SDK's resident scene core
   (psyq/src/core_shim.rs). Not a Psy-Q library: NMS's C does not call these; the layer does, and
   the wrapped scene entries (tools/scenes.py) call PsyqSceneEnter. Today the seam is implemented
   over the layer's own polled reader; when the core lands each becomes a call into the core's
   jump table (scene-system CORE-ABI.md section 3). Return codes: 0 success, else the core's stage
   code (0x13 read failed, 0x14 checksum, 0x21 table, 0x22 no row, 0x24 bounds, 0x31 drive held). */
#ifndef _PSX_CORE_SHIM_H
#define _PSX_CORE_SHIM_H

typedef unsigned int u_int32_t_core;

/* psx_core_read: blocking read of `sectors` sectors from `lba` into `dst` (word aligned), with the
   retries, ending paused; hook(i) after sector i is in RAM (may be 0). */
u_int32_t_core core_read(u_int32_t_core lba, u_int32_t_core sectors, unsigned long *dst, void (*hook)(u_int32_t_core));
/* psx_core_pause: the drive settled and paused. */
u_int32_t_core core_pause(void);
/* psx_core_client_attach / _detach: a stream client (the film) holds the drive meanwhile. */
u_int32_t_core core_client_attach(void);
void core_client_detach(void);
/* Not in the core's ABI yet (asked for): load the scene image of table row `scene` into its fixed
   range in the running program, check its word FNV, flush the I-cache, record the switch. */
u_int32_t_core core_load_image(u_int32_t_core scene);
/* The handoff block's current scene (0: the program, before any image). */
u_int32_t_core core_current_scene(void);

/* The layer's policy over core_load_image: load `scene` unless it is the current one; LOAD FAILED
   when it cannot be loaded. What every wrapped entry calls first. */
void PsyqSceneEnter(u_int32_t_core scene);

#endif
