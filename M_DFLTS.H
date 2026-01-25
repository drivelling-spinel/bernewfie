//**************************************************************************
//**
//** m_deflts.h :
//**
//**************************************************************************

#ifndef __MDEFLTS_H__
#define __MDEFLTS_H__

#include "xddefs.h"

/*
==============================================================================

							DEFAULTS

==============================================================================
*/

// Keyboard routines, By Lee Killough
// Based only a little bit on Chi's v0.2 code
int I_ScanCode2DoomCode (int a);

extern int key_right, key_left, key_up, key_down;
extern int key_strafeleft, key_straferight, key_jump;
extern int key_fire, key_use, key_strafe, key_speed;
extern int key_flyup, key_flydown, key_flycenter;
extern int key_lookup, key_lookdown, key_lookcenter;
extern int key_invleft, key_invright, key_useartifact;

extern int mousebfire;
extern int mousebstrafe;
extern int mousebforward;
extern int mousebjump;

extern int joybfire;
extern int joybstrafe;
extern int joybuse;
extern int joybspeed;
extern int joybjump;

extern boolean messageson;

extern  int     viewwidth, viewheight;

extern char *chat_macros[10];

typedef struct
{
	char    *name;
	int     *location;
	int     defaultvalue;
	int     scantranslate;      // PC scan code hack
	int     untranslated;       // lousy hack
} default_t;

#ifndef __NeXT__
extern int snd_Channels;
extern int snd_MusicDevice, // current music card # (index to dmxCodes)
	snd_SfxDevice; // current sfx card # (index to dmxCodes)
#endif


#ifdef DEFAULTCFG

extern int mouseSensitivity;

#ifndef __NeXT__
extern int snd_DesiredMusicDevice, snd_DesiredSfxDevice;

extern int     snd_SBport, snd_SBirq, snd_SBdma;       // sound blaster variables
extern int     snd_Mport;                              // midi variables
#endif

extern default_t defaults[];

extern int numdefaults;
extern char defaultfile[128];


// Set everything to base values
void M_SetBaseValues(void);


// definitions copied from main header file

extern	int		myargc;
extern	char	**myargv;

int M_CheckParm(char *check);
// returns the position of the given parameter in the arg list (0 if not found)
boolean M_ParmExists(char *check);


#endif

#endif
