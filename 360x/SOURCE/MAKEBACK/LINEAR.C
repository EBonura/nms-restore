/* Flags for combining with video modes during mode set */

#define	vbeDontClear	0x8000		/* Dont clear display memory		*/
#define	vbeLinearBuffer	0x4000		/* Enable linear framebuffer mode	*/

/* Flags for the mode attributes returned by VBE_getModeInfo. If
 * vbeMdNonBanked is set to 1 and vbeMdLinear is also set to 1, then only
 * the linear framebuffer mode is available.
 */

#define vbeMdAvailable	0x0001		/* Video mode is available			*/
#define	vbeMdTTYOutput	0x0004		/* TTY BIOS output is supported		*/
#define	vbeMdColorMode	0x0008		/* Mode is a color video mode		*/
#define	vbeMdGraphMode	0x0010		/* Mode is a graphics mode			*/
#define	vbeMdNonVGA		0x0020		/* Mode is not VGA compatible		*/
#define	vbeMdNonBanked	0x0040		/* Banked mode is not supported		*/
#define	vbeMdLinear		0x0080		/* Linear mode supported			*/

bool VBE_setVideoMode(int mode);

        Set the specified video mode, given the internal VBE mode number.
        DONT pass old style VBE mode numbers to this routine (ie: 0x101
        for 640x480x256), although these *may* still work, the VBE 2.0
        method is to search through the list of available video modes
        for the one that has the desired resolution and color depth. This
        will allow you code to work with all custom video resolutions
        provided by different OEM VBE drivers (like UniVBE/Pro). Have a
        look at the code in the HELLOVBE.C file that demonstrates how
        to start any video mode given a user specified resolution.

        To set a VBE 2.0 linear framebuffer mode, or the flag vbeLinearBuffer
        with the video mode.

        *DONT* attempt to set a linear framebuffer mode unless it is available.
        You can determine this by checking that the vbeMdLinear flag is set
        in the modeAtttibutes field of the VBE_modeInfo block for this video
        mode.

        Returns FALSE is the mode could not be set correctly.

ushort VBE_getLinearSelector(VBE_modeInfo *modeInfo);

        Utility function to create a selector to the linear framebuffer
        for the specified video mode. The selector is created only
        once (the linear framebuffer location *never* moves after bootup)
        and is cached for all other functions calls. The selector
        is always created with a limit of 8Mb, regardless of how much
        physical memory is really present.


