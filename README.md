# BERNEWFIE
## Hexen source port hack for DOS

![BERNEWFIE screenshot](/DOC/hex_000.png)

BERNEWFIE is a quick and dirty and source port of Hexen 1996 PC game by Raven Software from WATCOM to GCC that combines [original source](https://sourceforge.net/projects/heretic/files/) with functions taken from [MBF 2.04](https://archive.org/details/doom-mbf-204) maintenance release by Gerwin. DJGPP environment and Allegro library headers and binaries are a prerequisite to build it. Original SETUP.EXE from Hexen is used to configure game keyboard and mouse bindings (HEXEN.CFG) and SETUP.EXE from MBF 2.04 package is used to configure Allegro for sound and music (SETUP.CFG). If MBF_D2GM.IBK is found in the same directory it will be loaded during OPL music card initialization. Finally, BERNEWFIE requires CWSDMPI.EXE to be launched in "pure" DOS; it can also be found in MBF 2.04 package.

### What's been added
- 640x400 resolution activated with `-hires` command line argument
  (pillarboxed 640x480 will be activated if 640x400 is not supported by the installed video card)
- mouse look and alternative flight controls activated with `-mlook` command line argument
  (when flying hold mouse jump button and move mouse vertically to fly up and down)
- `-novert` command line argument
- up to 128 sound channels
  (SETUP.EXE limits this to 8, so HEXEN.CFG needs to be edited by hand)
- more visplanes 
- additional command line arguments to fine-tune Gerwin's VESA code
  (see [readme](DOC/MBFUP204.TXT) for details):
  `-show_fps` 
  `-use_vsync` 
  `-page_flip` 
  `-safe` 
  `-asmp6` 
  `-nolfb` 
  `-nopm` 

### What's removed
- Graphical startup
- CD Audio support (including track changing cheat code)
- External driver support for "exotic" input devices

### What does not work
- Hard to tell at the moment but finale screens and minimap have never been tested yet
- Demos go out of sync
- No idea if multiplayer works
- Patch scaling is not implemented and status bar is showing outright weird
  It is strongly recommended to play with fullscreen HUD as on the screenshot

### Code changes
Most of the code in BERNEWFIE is untouched Hexen code, except for:
- improved VESA and Allegro system routines by Gerwin
- zone memory management, some math and column drawing assembly code by Lee Killough


Also changes were made to:
- game startup to incorporate allegedly more Windows NT friendly MBF 2.04 code 
- sound handing to avoid crashes in main menu and to accomodate for MBF sound caching
- status bar / HUD drawing to "scale" to higher resolutions
- menu layout to center in higher resolutions
- info and title drawing procs to show correctly in higher resolutions
- visplanes structure to extened heigh from 8 to 16 bits for higher resolution screens
- sky scaling code to work correctly in higher resolutions
- player weapon sprites scaing code to show correctly in higher resolutions
- game subsystems startup order to accomodate for changes to graphic mode initialization 

