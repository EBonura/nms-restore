// ----------------------------------------------------------------------------

/* escher 3d information */

txtstructptr mesh_table[] = {

  /*------------------------ POTTERS ROOM -------------------------*/

/*----------------*/
/* potters  us room */
	"tables\\potters\\potterus.esh",	/* escher 3d mesh */
	"tables\\potters\\potterus.emp",	/* escher texture mapping data */
	"tables\\potters\\bright.tab",	/* light source table */
	"tables\\potters\\ustable.bmp",	/* ball machanic bumper information */
	"tables\\potters",

/*----------------*/
/* potters uk room */

	"tables\\potters\\potteruk.esh", /* escher 3d mesh */
	"tables\\potters\\potteruk.emp", /* escher texture mapping data */
	"tables\\potters\\bright.tab",   /* light source table */
	"tables\\potters\\ukcolis.bmp",  /* ball machanic bumper information */	// RESTORE: was uktable.bmp, a broken export (pocket_y, pocket_radius "*****"); UKCOLIS.BMP is byte-identical to POTTERUK.BMP, and the US pair USTABLE.BMP == USCOLIS.BMP shows NMS's naming
	"tables\\potters",

/*----------------*/

	"tables\\potters\\pottrsn.esh", /* escher 3d mesh */
	"tables\\potters\\pottrsn.emp", /* escher texture mapping data */
	"tables\\potters\\bright.tab",   /* light source table */
	"tables\\potters\\snookcol.bmp",  /* ball machanic bumper information */
	"tables\\potters",

  /*------------------------ RACKERS ROOM -------------------------*/

/*----------------*/
/* rackers us room */
	"rackers\\club.esh",	/* escher 3d mesh */
	"rackers\\club.emp",	/* escher texture mapping data */
	"rackers\\bright.tab",	/* light source table */
	"rackers\\ustable.bmp",	/* ball machanic bumper information */
	"rackers",

/*----------------*/
/* rackers uk room */

	"rackers\\clubuk.esh", /* escher 3d mesh */
	"rackers\\clubuk.emp", /* escher texture mapping data */
	"rackers\\bright.tab",   /* light source table */
	"rackers\\uktable.bmp",  /* ball machanic bumper information */
	"rackers",

/*----------------*/

	"rackers\\clubsn.esh", /* escher 3d mesh */
	"rackers\\clubsn.emp", /* escher texture mapping data */
	"rackers\\bright.tab",   /* light source table */
	"rackers\\snookcol.bmp",  /* ball machanic bumper information */
	"rackers",

/*----------------*/

/*----------------*/
/* hall us room */

	"hall\\poolus.esh", /* escher 3d mesh */
	"hall\\poolus.emp", /* escher texture mapping data */
	"hall\\bright.tab",   /* light source table */
	"hall\\ustable.bmp",  /* ball machanic bumper information */
	"hall",

/*----------------*/
/* hall uk room */

	"hall\\pooluk.esh", /* escher 3d mesh */
	"hall\\pooluk.emp", /* escher texture mapping data */
	"hall\\bright.tab",   /* light source table */
	"hall\\uktable.bmp",  /* ball machanic bumper information */
	"hall",

/*----------------*/

	"hall\\poolhasn.esh", /* escher 3d mesh */
	"hall\\poolhasn.emp", /* escher texture mapping data */
	"hall\\bright.tab",   /* light source table */
	"hall\\snookcol.bmp",  /* ball machanic bumper information */
	"hall",


   /*------------------------- GERMAN ROOM ---------------------------*/

/*----------------*/
/* german us room */

	"german\\germanus.esh",	/* escher 3d mesh */
	"german\\germanus.emp",	/* escher texture mapping data */
	"german\\bright.tab",	/* light source table */
	"german\\ustable.bmp",	/* ball machanic bumper information */
	"german",
/*----------------*/

/*----------------*/
/* german uk room */

	"german\\germanuk.esh", /* escher 3d mesh */
	"german\\germanuk.emp", /* escher texture mapping data */
	"german\\bright.tab",   /* light source table */
	"german\\uktable.bmp",  /* ball machanic bumper information */
	"german",

/*----------------*/

	"german\\germansn.esh", /* escher 3d mesh */
	"german\\germansn.emp", /* escher texture mapping data */
	"german\\bright.tab",   /* light source table */
	"german\\snookcol.bmp",  /* ball machanic bumper information */
	"german",
   /*------------------------- POOL HALL ROOM ---------------------------*/

/*----------------*/
/* hall uk room */

	"snooker\\room.esh", /* escher 3d mesh */
	"snooker\\room.emp", /* escher texture mapping data */
	"snooker\\bright.tab",   /* light source table */
	"snooker\\snookcol.bmp",  /* ball machanic bumper information */
	"snooker",

   /*------------------------- MOON ROOM ---------------------------*/

	"moon\\moon.esh",	/* escher 3d mesh */
	"moon\\moon.emp",	/* escher texture mapping data */
	"moon\\bright.tab",	/* light source table */
	"moon\\hex.bmp",	/* ball machanic bumper information */
	"moon",

   /*------------------------- ICE RINK ROOM ---------------------------*/

	"ice\\iceberg.esh",	/* escher 3d mesh */
	"ice\\iceberg.emp",	/* escher texture mapping data */
	"ice\\bright.tab",	/* light source table */
	"ice\\ustable.bmp",	/* ball machanic bumper information */
	"ice",

   /*------------------------- BEACH ROOM ---------------------------*/

	"beach\\beach4.esh",	/* escher 3d mesh */
	"beach\\beach4.emp",	/* escher texture mapping data */
	"beach\\bright.tab",	/* light source table */
	"beach\\ustable.bmp",	/* ball machanic bumper information */
	"beach",

   /*------------------------- COFFIN ROOM ---------------------------*/

// RESTORE: the paths gain the "tables\" prefix the Potters rows carry (the
// archive keeps them in TABLES/CRYPT).  The last string is the folder the
// balls and cue mesh is loaded from (init_3d: <folder>\ballscue.esh); there
// is no ballscue.esh in TABLES/CRYPT, so the Crypt uses the shared
// BALLSCUE.ESH, as the Potters tables do.
	"tables\\crypt\\coffin2.esh",	/* escher 3d mesh */
	"tables\\crypt\\coffin2.emp",	/* escher texture mapping data */
	"tables\\crypt\\bright.tab",	/* light source table */
	"tables\\crypt\\coffcoli.bmp",	/* ball machanic bumper information */
	"tables\\potters",

   /*------------------------- SWAMP ROOM ---------------------------*/

	"swamp\\swamp.esh",	/* escher 3d mesh */
	"swamp\\swamp.emp",	/* escher texture mapping data */
	"swamp\\bright.tab",	/* light source table */
	"swamp\\ustable.bmp",	/* ball machanic bumper information */
	"swamp",

    /*------------------------- TOY ROOM ---------------------------*/

// RESTORE: the toy room's files are not in the archive under these names.
// The table icon's Square is TABLES/POTTERS/SQUARE.ESH and .EMP (97 objects
// each) with its collision file SQRCOLIS.BMP, and the folder's own
// BRIGHT.TAB; the balls and cue come from that folder's BALLSCUE.ESH.
	"tables\\potters\\square.esh",	/* escher 3d mesh */
	"tables\\potters\\square.emp",	/* escher texture mapping data */
	"tables\\potters\\bright.tab",	/* light source table */
	"tables\\potters\\sqrcolis.bmp",  /* ball machanic bumper information */
	"tables\\potters",
};

txtstructptr room_palettes[] = {

/* blue baize */

		 "potters\\pottblue.pal" ,
		 "rackers\\clubblu.pal" ,
		 "hall\\poolblue.pal" ,
		 "german\\gerblue.pal" ,
		 "snooker\\grrr.pal" ,
		 "moon\\grrr.pal" ,
		 "ice\\iceberg.pal" ,
		 "beach\\beach.pal" ,
		 "crypt\\grrr.pal" ,
		 "swamp\\swamp.pal" ,
// RESTORE: no toy palette or table is in the archive ("toy\\toys.pal" and
// "toy\\poolpal.tab"); the Square takes the Potters set, the files that sit
// beside SQUARE.ESH, for each baize colour.
		 "potters\\pottblue.pal" ,

/* green baize */


		 "potters\\pottgren.pal",
		 "rackers\\rackgren.pal",
		 "hall\\poolgren.pal" ,
		 "german\\gergren.pal",
		 "snooker\\grrr.pal" ,

		 "moon\\grrr.pal",
		 "ice\\iceberg.pal" ,
		 "beach\\beach.pal" ,
		 "crypt\\grrr.pal" ,
		 "swamp\\swamp.pal" ,
		 "potters\\pottgren.pal" ,

/* red baize */
								   
		 "potters\\pottred.pal",
		 "rackers\\clubred.pal",
		 "hall\\poolred.pal" ,
		 "german\\gerred.pal" ,
		 "snooker\\grrr.pal" ,

		 "moon\\grrr.pal",
		 "ice\\iceberg.pal" ,
		 "beach\\beach.pal" ,
		 "crypt\\grrr.pal" ,
		 "swamp\\swamp.pal" ,
		 "potters\\pottred.pal" ,
};

txtstructptr poolpal_tab[] = {
/* blue baize */

	   "potters\\bluepal.tab" ,
	   "rackers\\bluepal.tab" ,
	   "hall\\bluepal.tab"  ,
	   "german\\bluepal.tab"  ,
	   "snooker\\poolpal.tab"     ,

	   "moon\\poolpal.tab"    ,
	   "ice\\poolpal.tab"     ,
	   "beach\\poolpal.tab"   ,
	   "crypt\\poolpal.tab"   ,
	   "swamp\\grenpal.tab"   ,
	   "potters\\bluepal.tab" ,	// RESTORE: see room_palettes (no toy\\poolpal.tab)

/* red baize */

	   "potters\\grenpal.tab" ,
	   "rackers\\grenpal.tab" ,
	   "hall\\greenpal.tab"  ,
	   "german\\grenpal.tab"  ,
	   "snooker\\poolpal.tab"     ,

	   "moon\\poolpal.tab"    ,
	   "ice\\poolpal.tab"     ,
	   "beach\\poolpal.tab"   ,
	   "crypt\\poolpal.tab"   ,
	   "swamp\\grenpal.tab"   ,
	   "potters\\grenpal.tab" ,
	  			  
/* green baize */

	   "potters\\redpal.tab"  ,
	   "rackers\\redpal.tab"  ,
	   "hall\\redpal.tab"   , 
	   "german\\redpal.tab"   , 
	   "snooker\\poolpal.tab"     ,

	   "moon\\poolpal.tab"    ,
	   "ice\\poolpal.tab"     ,
	   "beach\\poolpal.tab"   ,
	   "crypt\\poolpal.tab"   ,
	   "swamp\\grenpal.tab"   ,
	   "potters\\redpal.tab"  ,
};
/* ------------------------------------ */
/* ball racking positions for all rooms */
long ball_spot_positions[] = {	// RESTORE: 8.24 (FIX24), were doubles

		 FIX24(0.62),FIX24(0.42),FIX24(0.33),   /* potters */
		 FIX24(0.62),FIX24(0.42),FIX24(0.33),   /* rackers */
		 FIX24(0.62),FIX24(0.42),FIX24(0.33),   /* pool hall */
		 FIX24(0.62),FIX24(0.42),FIX24(0.33),   /* arena */
		 FIX24(1.00),FIX24(0.42),FIX24(0.33),   /* snooker hall */

		 FIX24(0.32),             /* moon */
		 FIX24(0.58),             /* icerink */
		 FIX24(0.62),             /* beach */
		 FIX24(0.92),             /* crypt */
		 FIX24(0.62),             /* swamp */
		 FIX24(0.42),             /* toy */
};
/* -------------------------------------- */
/* white ball spot position for all rooms */
long whiteball_spot_positions[] = {
       
      FIX24(-0.850),FIX24(-0.650),FIX24(-0.580),  /* potters */
      FIX24(-0.850),FIX24(-0.650),FIX24(-0.580),  /* rackers */
      FIX24(-0.850),FIX24(-0.650),FIX24(-0.580),  /* pool hall */
      FIX24(-0.850),FIX24(-0.650),FIX24(-0.580),  /* arena */
      FIX24(-1.250),FIX24(-0.650),FIX24(-0.580),  /* snooker hall */

		FIX24(-0.450),			        /* moon */
		FIX24(-0.550),					  /* icerink */
		FIX24(-0.850),					  /* beach */
		FIX24(-1.0050),					  /* crypt */
		FIX24(-0.650),					  /* swamp */
		FIX24(-0.550),					  /* toy */
};
/* ------------------------------------ */

