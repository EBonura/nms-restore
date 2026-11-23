//! A PS1 renderer for QD Up's 3D pool table, behind NMS's Escher interface.
//!
//! New work, written for the restoration: NMS never had a PS1 3D view. In
//! the September 1996 source every Escher render and projection call on the
//! PS1 is an empty stub (ENGINE.C) and the game shows a 2D top-down debug
//! view; the PC's Escher renderer itself does not survive (PINDOS.EXE in the
//! archive is a DOS pinball game, not QD Up). This crate implements the
//! E_* functions ENGINE.C stubs, with their C signatures, from how STV3D.C
//! and the rest of the game call them and from the data NMS's loader
//! (ESCHER.S: E_Init, E_AddMesh, E_Load, E_GetObjectPtr) reads. Its
//! readings of Escher (the rotation order, the projection, the easing, the
//! line colours) are marked where they are made.
//!
//! - Vertices go through the GTE (RTPS for the scene, MVMVA for the meshes
//!   E_RotateObject turns); polygons are flat, coloured as the PC's Escher
//!   coloured them, palette[BRIGHT.TAB[brightness][colour]].
//! - Each render call is a band of the game's ordering table, in the order
//!   draw_3d makes them, sorted inside by object priority then depth.
//! - Polygons that cross the near plane or reach past the guard band are
//!   clipped in camera space; the rest go to the GPU as the GTE projected
//!   them.
//! - The balls are E_DrawBall's shaded discs, as textured quads.
//! - Memory: E_Init's edge buffers (the PC span renderer's, unused on the
//!   PS1), so the renderer adds no RAM beyond its code and a few statics.
//! - 32-bit integer maths only, no floating point.
#![no_std]
#![cfg_attr(target_arch = "mips", feature(asm_experimental_arch))]
#![allow(non_snake_case, non_upper_case_globals, clippy::missing_safety_doc)]

mod api;
mod balls;
mod camera;
mod fixed;
mod lines;
mod mesh;
mod nms;
mod pool;
mod render;
mod select;
