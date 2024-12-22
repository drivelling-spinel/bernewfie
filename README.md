# BERNEWFIE
## Hexen source port for DOS

![BERNEWFIE screenshot](/DOC/hex_000.png)   
_Are you ready to die?_

BERNEWFIE is a source port of Hexen 1996 PC game by Raven Software from WATCOM to GCC that combines [original source](https://sourceforge.net/projects/heretic/files/) with functions taken from [MBF 2.04](https://archive.org/details/doom-mbf-204) maintenance release by Gerwin. DJGPP environment and Allegro library headers and binaries are a prerequisite to build it. Original SETUP.EXE from Hexen is used to configure game keyboard and mouse bindings (HEXEN.CFG) and ASETUP.EXE is used to configure Allegro for sound and music (SETUP.CFG). 

### What's been added
- High resolution support activated with one of the below arguments   
  `-400p`    
  `-480p`    
  `-600p`    
  `-768p`    
  `-1024p`    
- mouse look and alternative flight controls activated with `-mlook` command line argument   
  (when flying hold mouse jump button and move mouse vertically to fly up and down)
- `-novert` command line argument
- up to 32 sound channels activated with `-maxsound` command line argument    
  (running SETUP.EXE caps to 8, but HEXEN.CFG can be edited by hand for values between 8 and 32)
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
- `-extra` command line argument for a second way to provide PWADs to be loaded to the engine   
  (e.g. to `HEXENDK.EXE -extra PWADS\BERNEW.WAD`)   
- `-noorb` command line to spend as few time as possible in the start screen 
  and jump to the title screen instead   
- Hexen Unofficial Update v1.2 \@Boondorl ported from ZScript C
  (optionally included at compile time)   
- more visplanes (bumped to 960), also more vissprites (bumped to 256)

### BERNEWFIE in action

![Original Deathkings Mage demo running](/DOC/hexendk_007.png)   
_Original Deathkings Mage demo running_

![Deathkings as Warrior in in high resolution](/DOC/hexendk_005.png)   
_Deathkings as Warrior in in high resolution_

![Automap in high resolition](/DOC/hexendk_006.png)   
_Automap in high resolition_

![Additional savegame slots](/DOC/hex_001.png)   
_Additional savegame slots_

### What's removed
- Graphical startup (although Bernewfie tries to mimic it a little bit, 
  if you find that too awkward, use `-noorb` command line argument to skip)
- CD Audio support (including track changing cheat code)
- External driver support for "exotic" input devices 
- No idea if multiplayer works

### Code changes
Most of the code in BERNEWFIE is untouched Hexen code, except for:
- improved VESA and Allegro system routines by Gerwin
- zone memory management, some math and column drawing assembly code by Lee Killough


Also changes were made to:
- game startup to incorporate allegedly more Windows NT friendly MBF 2.04 code 
- sound handing to avoid crashes in main menu and to accomodate for MBF sound caching
- fullscreen HUD drawing to "scale" to higher resolutions
- menu, status bar, intermission and finale layout and messages to center in higher resolutions
- info, title, intermission and finale drawing procs to show correctly in higher resolutions
- automap to work correctly in higher resolutions
- screen refresh and screen border logic to accomodate for status bar only taking  
  a portion of the lower screen part in higher resolution
- visplanes structure to extened height from 8 to 16 bits for higher resolution screens
- sky scaling code to work correctly in higher resolutions and with mouse look
- player weapon sprites scaing code to show correctly in higher resolutions
- game subsystems startup order to accomodate for changes to graphic mode initialization 

