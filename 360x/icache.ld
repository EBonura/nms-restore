/* RESTORE: PLATFORM. 360X's I-cache placement on the disc build, checked at link time (360x/build.sh adds this
 * script to the shipping scene build's link, not to a test disc's: docs: 360x/scenes.txt `region`).
 *
 * The R3000's I-cache is 4 KB, direct mapped, 16-byte lines: two pieces of code whose addresses are equal modulo
 * 4 KB evict each other. Print_The_Map (SCROLLER.C, in the MISSION image) draws the map with one call to
 * GsSortFastSprite a tile; GsSortFastSprite and the layer's sort_sprt it calls are the layer's hot functions,
 * which psyq.ld pins from I-cache line 252 (`.text.layer_hot`, 0xfc0 past a 4 KB boundary, through line 24).
 * When the map loop's lines share sets with them, every tile evicts both: over the x360-mission window
 * (gate frontend, polls 2850..3400) the refill stalls go from 4.6M to 8.6M cycles (region offset 0x7c0 against
 * 0x840) and 10.6M (0x880). The region starts at a fixed place modulo 4 KB (scenes.txt `region 0x1000 OFFSET`)
 * and layer_hot is pinned, so only the region offset or MISSION's code ahead of Print_The_Map can move one
 * against the other; then the link stops here instead of the game slowing down. Print_The_Map's extent is its
 * start to Do_Scroll_Download's, the function after it in SCROLLER.C (one .text section, source order).
 */
__x360_map_lo = Print_The_Map & ~15;
__x360_map_hi = (Do_Scroll_Download + 15) & ~15;
__x360_hot_lo = __layer_hot_start & ~15;
__x360_hot_hi = (__layer_hot_end + 15) & ~15;
ASSERT(((__x360_map_lo - __x360_hot_lo) & 0xfff) >= __x360_hot_hi - __x360_hot_lo
       && ((__x360_hot_lo - __x360_map_lo) & 0xfff) >= __x360_map_hi - __x360_map_lo,
       "360x/icache.ld: Print_The_Map shares I-cache lines with the layer's hot functions: choose another region offset (360x/scenes.txt)")
