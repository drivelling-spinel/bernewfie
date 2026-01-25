# BERNEWFIE
## Hexen source port for DOS

![BERNEWFIE logo](/DOC/logo.png)   

BERNEWFIE is a source port of Hexen 1996 PC game by Raven Software from WATCOM to GCC that combines [original source](https://sourceforge.net/projects/heretic/files/) with functions taken from [MBF 2.04](https://archive.org/details/doom-mbf-204) maintenance release by \@Gerwin. DJGPP environment and Allegro library headers and binaries are a prerequisite to build it. Original SETUP.EXE from Hexen is used to configure game keyboard, mouse and joystick bindings (HEXEN.CFG) and can also be used for basic audio setup. ASETUP.EXE is used to fine tune Allegro for sound and music configuration and calibrate joystick (SETUP.CFG). 

### What's been added
- High resolution support activated with one of the below arguments   
  `-400p`    
  `-480p`    
  `-600p`    
  `-768p`    
  `-1024p`    
  `-1200p`    
  `-wxga`    
- mouse look and alternative flight controls activated with `-mlook` command line argument   
  (when flying hold mouse jump button and move mouse vertically to fly up and down)
- `-novert` command line argument
- up to 32 sound channels activated with `-maxsound` command line argument    
  (running SETUP.EXE caps to 8, but HEXEN.CFG can be edited by hand for values between 8 and 32)
- more fluid experience with quicksave/quickload with `-quick` command line argument:
  quickloading instead of respawning, less prompts and end game/suicide buttons switched off
- 10 more savegame slots available with high resolution options   
  (new slots have filenames like hexa*.*, hexb*.* etc in HEXNDATA)
- additional command line arguments to fine-tune \@Gerwin's video code
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
- `-extra` command line argument for an alternative way to provide PWADs to be loaded to the engine   
  (e.g. to `HEXENDK.EXE -extra FMMUS.WAD`)   
- `-noorb` command line to spend as few time as possible in the start screen 
  and jump to the title screen instead   
- Hexen Unofficial Update v1.2 by \@Boondorl ported from ZScript C
  (optionally included at compile time, e.g. in `BERNEW12.EXE`)   
- Renderer image aspect correction to compensate for square pixels (`-aspect 1`, `-aspect 2`) 
  and screen geometry (`-wide16x9`, `-wide16x10`)
- basic support for "POV hats" on joysticks (no remapping)
- fix to avoid crashes with Porkelator when facing the Wyvern
  (only included for builds that also contain 1.2 features - see above)
- joke mode with permanent flight activated with `-canfly`
- more visplanes (bumped to 960), also more vissprites (bumped to 256)


### What's removed
- Graphical startup (although Bernewfie tries to mimic it a little bit, 
  if you find that too awkward, use `-noorb` command line argument to skip)
- CD Audio support (including track changing cheat code)
- External driver support for "exotic" input devices 
- No idea if multiplayer works


### Code changes
Most of the code in BERNEWFIE is untouched Hexen code, except for:
- sound, video and system routines taken from MBF 2.04 (by Gerwin, CRVS)
- memory management, some math and column drawing assembly code also from MBF (by Lee Killough)


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
- player weapon sprites scaling code to, well, scale accordingly in higher resolutions
- game subsystems startup order to accommodate for changes to graphic mode initialization 


