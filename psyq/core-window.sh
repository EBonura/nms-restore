# RESTORE: PLATFORM. The one parameter for the SDK scene core's window: the RAM a console program may use,
# from 0x80000000 up to the window (scene-system CORE-ABI.md section 0). The window and its 256-byte handoff
# block sit above it, to the top of the 2 MB. Sourced by 360x/build.sh and qdup/build.sh, which pass it
# to the link (psyq/psyq.ld RAM_SIZE) and to QD Up's heap (RAM.C _ramsize); PSYQ_RAM_SIZE overrides it.
# 0x1FE000: an 8,192-byte window, 0x801FDF00..0x801FFF00 (provisional until the core's first proof;
# 0x1FD800 for the 10,240-byte cap).
core_ram_size=${PSYQ_RAM_SIZE:-0x1FE000}
