/* PSoXide Psy-Q compatibility layer: the dev kit's PC link (files on the
   developer's PC) and its debug monitor. psyq/src/pcdrv.rs, over the BREAK
   0x101..0x107 convention the PSoXide emulator serves with --pcdrv. */
#ifndef PSYQ_LIBSN_H
#define PSYQ_LIBSN_H
int  PCinit(void);
int  PCopen(char *name, int mode, int attrib);   /* handle, -1 on failure */
int  PCcreat(char *name, int attrib, int unused);
int  PClseek(int fd, int offset, int whence);    /* whence 0 set, 1 current, 2 end */
int  PCread(int fd, char *buf, int len);         /* bytes read, -1 on failure */
int  PCwrite(int fd, char *buf, int len);
int  PCclose(int fd);
/* pollhost() is a macro, as the 1996 build shows (a BREAK in line, no call):
   360X has a variable of the same name and writes `if (pollhost==YES)
   pollhost();`. Without a debugger there is nothing to poll. */
void psyq_pollhost(void);
#define pollhost() psyq_pollhost()
#endif
