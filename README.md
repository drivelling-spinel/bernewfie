# BERNEWFIE
## Hexen source port hack for DOS

![BERNEWFIE screenshot](/DOC/hex_000.png)   
_Are you ready to die?_

BERNEWFIE is a quick and dirty and source port of Hexen 1996 PC game by Raven Software from WATCOM to GCC that combines [original source](https://sourceforge.net/projects/heretic/files/) with functions taken from [MBF 2.04](https://archive.org/details/doom-mbf-204) maintenance release by Gerwin. DJGPP environment and Allegro library headers and binaries are a prerequisite to build it. Original SETUP.EXE from Hexen is used to configure game keyboard and mouse bindings (HEXEN.CFG) and SETUP.EXE from MBF 2.04 package is used to configure Allegro for sound and music (SETUP.CFG). If MBF_D2GM.IBK is found in the same directory it will be loaded during OPL music card initialization. Finally, BERNEWFIE requires CWSDMPI.EXE to be launched in "pure" DOS; it can also be found in MBF 2.04 package.

### What's been added
- 640x400 resolution activated with `-hires` command line argument   
  (pillarboxed 640x480 will be activated if 640x400 is not supported by the installed video card)
- mouse look and alternative flight controls activated with `-mlook` command line argument   
  (when flying hold mouse jump button and move mouse vertically to fly up and down)
- `-novert` command line argument
- up to 128 sound channels activated with `-maxsound` command line argument    
  (SETUP.EXE limits this to 8, so HEXEN.CFG needs to be edited by hand for values between 8 and 128)
- twice more visplanes 
- more fluid experience with quicksave/quickload with `-quick` command line argument:
  quickloading instead of respawning, less prompts and end game/suicide buttons switched off
- 10 more savegame slots available with `-hires` command line argument   
  (new slots have filenames like hexa*.*, hexb*.* etc in HEXNDATA)
- additional command line arguments to fine-tune Gerwin's VESA code
  (see [readme](DOC/MBFUP204.TXT) for details):
  `-show_fps`    
  `-use_vsync`    
  `-page_flip`    
  `-safe`    
  `-asmp6`    
  `-nolfb`    
  `-nopm`    
- support for WAV sound effect lumps and 16-bit samples    
  (in case a SoundBulb-like mod for Hexen arrives some day)
- optional assets WAD with portions of status bar that are normally off-screen in lower resolutions,
  that is explicitly loaded with either `-file BERNEW.WAD` _or_
- `-extra` command line argument for a second way to provide PWADs to be loaded to the engine   
  (e.g. to `HEXENDK.EXE -extra WADS\BERNEW.WAD`)

### BERNEWFIE in action

![Original Deathkings Mage demo running](/DOC/hexendk_007.png)   
_Original Deathkings Mage demo running_

![Status bar in high resolution with Deathkings and BERNEW.WAD loaded](/DOC/hexendk_005.png)   
_Status bar in high resolution with Deathkings and BERNEW.WAD loaded_

![Automap in high resolition](/DOC/hexendk_006.png)   
_Automap in high resolition_

![Additional savegame slots](/DOC/hex_001.png)   
_Additional savegame slots_

Also check out a recording of live gameplay: <https://www.youtube.com/watch?v=trRCKFNDD6Q>

### What's removed
- Graphical startup
- CD Audio support (including track changing cheat code)
- External driver support for "exotic" input devices 

### What does not work
- Hard to tell at the moment as lots of bugs are to be expected.
  If you run into any, and are on a board I frequent, please let me know.
- Finale screen has not been tested
- No idea if multiplayer works
- Patch scaling is not implemented and full screen HUD geometry is not correct

### Code changes
Most of the code in BERNEWFIE is untouched Hexen code, except for:
- improved VESA and Allegro system routines by Gerwin
- zone memory management, some math and column drawing assembly code by Lee Killough


Also changes were made to:
- game startup to incorporate allegedly more Windows NT friendly MBF 2.04 code 
- sound handing to avoid crashes in main menu and to accomodate for MBF sound caching
- fullscreen HUD drawing to "scale" to higher resolutions
- menu, status bar and  finale layout and messages to center in higher resolutions
- info, title and finale drawing procs to show correctly in higher resolutions
- automap to work correctly in higher resolutions
- screen refresh and screen border logic to accomodate for status bar only taking  
  a portion of the lower screen part in higher resolution
- visplanes structure to extened heigh from 8 to 16 bits for higher resolution screens
- sky scaling code to work correctly in higher resolutions and with mouse look
- player weapon sprites scaing code to show correctly in higher resolutions
- game subsystems startup order to accomodate for changes to graphic mode initialization 

