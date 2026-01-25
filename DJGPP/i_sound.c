// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: i_sound.c,v 1.3 2000-08-12 21:29:34 fraggle Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// DESCRIPTION:
//      System interface for sound.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_sound.c,v 1.3 2000-08-12 21:29:34 fraggle Exp $";

#include <stdio.h>
#include <allegro.h>
#include <gppconio.h>  // gotoxy, textcolor GB 2014

#include "mmus2mid.h"   //jff 1/16/98 declarations for MUS->MIDI converter
#include "../h2def.h"


// Needed for calling the actual sound output.
#define SAMPLECOUNT             512

// Factor volume is increased before sending to allegro
#define VOLSCALE                16

static int snd_card;
static int mus_card;

extern sfxinfo_t S_sfx[];


//int default_snd_card;  // killough 10/98: add default_ versions
//int default_mus_card;

//int detect_voices; //jff 3/4/98 enables voice detection prior to install_sound
//jff 1/22/98 make these visible here to disable sound/music on install err

static SAMPLE *raw2SAMPLE(unsigned char *rawdata, int *len)
{
  SAMPLE *spl = malloc(sizeof(SAMPLE));
  *len = *len - 8;
  spl->bits = 8;

  // comment out this line for allegro pre-v3.12
//#if(ALLEGRO_VERSION>=3 && ALLEGRO_SUB_VERSION>=10)
//  spl->stereo = 0;
//#endif

  // killough 1/22/98: Get correct frequency
  spl->freq = (rawdata[3]<<8)+rawdata[2];
  spl->len = *len;
  spl->priority = 255;
  spl->loop_start = 0;
  spl->loop_end = *len;
  spl->param = -1;
  spl->data = rawdata + 8;
  
  _go32_dpmi_lock_data(rawdata+8, *len);   // killough 3/8/98: lock sound data
  return spl;
}

static SAMPLE *wav2SAMPLE(unsigned char *rawdata, int *len)
{
  SAMPLE *spl = malloc(sizeof(SAMPLE));
  int channels = *(short *)(rawdata+22);
  int i = 0;
  int byteoffs = 44;
  int bytelen = *(int *)(rawdata + byteoffs - 4);
  
  spl->bits = *(short *)(rawdata+34);
  spl->freq = *(int *)(rawdata+24);
  spl->priority = 255;
  spl->loop_start = 0;
  spl->param = -1;
  
  spl->data = rawdata + byteoffs;
  if(bytelen > *len - byteoffs) bytelen = *len - byteoffs;
  spl->len = bytelen >> (spl->bits >> 4);
  
  while(!strncmp("LIST", spl->data - 8, 4))
  {
    int listlen = *(int *)(spl->data - 4);
    byteoffs += listlen + 8;
    spl->data += listlen + 8;
    bytelen = *(int *)(spl->data - 4);
    if(bytelen > *len - byteoffs) bytelen = *len - byteoffs;
    spl->len = bytelen >> (spl->bits >> 4);
  }
  
  spl->loop_end = spl->len;
  // Allegro does this for some reason
  for(i = 0; i < spl->len && spl->bits == 16; i += 1) ((char *)spl->data)[1 + (i << 1)] ^= 0x80;
  *len = bytelen;
  _go32_dpmi_lock_data(spl->data, bytelen);   // killough 3/8/98: lock sound data
  return spl;
}


//
// This function loads the sound data from the WAD lump,
//  for single sound.
//
static void *getsfx(char *sfxname, int *len)
{
  unsigned char *sfx;
  int  i;
  int  size;
  int  sfxlump;
  int  format;

  // Get the sound data from the WAD, allocate lump
  //  in zone memory.

  // Now, there is a severe problem with the
  //  sound handling, in it is not (yet/anymore)
  //  gamemode aware. That means, sounds from
  //  DOOM II will be requested even with DOOM
  //  shareware.
  // The sound list is wired into sounds.c,
  //  which sets the external variable.
  // I do not do runtime patches to that
  //  variable. Instead, we will use a
  //  default sound for replacement.

  if ( W_CheckNumForName(sfxname) == -1 ) 
    sfxlump = W_GetNumForName("default");
  else
    sfxlump = W_GetNumForName(sfxname);

  size = W_LumpLength(sfxlump);
  sfx = memcpy(malloc(size), W_CacheLumpNum(sfxlump, PU_CACHE), size);
  
  format = size >= 44 && !strncmp("RIFF", sfx, 4);
  *len = size;
  
  if(format) return wav2SAMPLE(sfx,len); 
  return raw2SAMPLE(sfx,len);
}

// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//

void I_SetChannels()
{
  // no-op.
}


// jff 1/21/98 moved music volume down into MUSIC API with the rest

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
  return W_CheckNumForName(sfx->lumpname);
}

// Almost all of the sound code from this point on was
// rewritten by Lee Killough, based on Chi's rough initial
// version.

// killough 2/21/98: optionally use varying pitched sounds

#define pitched_sounds (1)
#define PITCH(x) (pitched_sounds ? ((x)*1000)/128 : 1000)

// This is the number of active sounds that these routines
// can handle at once, regardless of the mixer's ability
// (which we don't care about since allegro does the mixing)
// We set it to some ridiculously large number, to avoid
// any chances that these routines will stop the sounds.
// killough

#define NUM_CHANNELS 256

// "Channels" used to buffer requests. Distinct SAMPLEs
// must be used for each active sound, or else clipping
// will occur.

static SAMPLE channel[NUM_CHANNELS];

// This function adds a sound to the list of currently
// active sounds, which is maintained as a given number
// of internal channels. Returns a handle.

int I_StartSound(sfxinfo_t * snd, int   vol, int sep, int pitch, int pri)
{
  static int handle;

  // move up one slot, with wraparound
  if (++handle >= NUM_CHANNELS)
    handle = 0;

  // destroy anything still in the slot
  stop_sample(&channel[handle]);

  // Copy the sound's data into the sound sample slot
  memcpy(&channel[handle], snd->snd_ptr, sizeof(SAMPLE));

  // Start the sound
//  play_sample(&channel[handle],vol*VOLSCALE+VOLSCALE-1,256-sep,PITCH(pitch),0);
  play_sample(&channel[handle],vol*VOLSCALE+VOLSCALE-1,sep,PITCH(pitch),0); // GB 2017, stereo was reversed. sep=0..255

  // Reference for s_sound.c to use when calling functions below
  return handle;
}

// Stop the sound. Necessary to prevent runaway chainsaw,
// and to stop rocket launches when an explosion occurs.

void I_StopSound (int handle)
{
  stop_sample(channel+handle);
}

// Update the sound parameters. Used to control volume,
// pan, and pitch changes such as when a player turns.

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch)
{
//  adjust_sample(&channel[handle], vol*VOLSCALE+VOLSCALE-1, 256-sep, PITCH(pitch), 0);
  adjust_sample(&channel[handle], vol*VOLSCALE+VOLSCALE-1, sep, PITCH(pitch), 0); // GB 2017, stereo was reversed. sep=0..255

}

// We can pretend that any sound that we've associated a handle
// with is always playing.

int I_SoundIsPlaying(int handle)
{
  return 1;
}

// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the global
//  mixbuffer, clamping it to the allowed range,
//  and sets up everything for transferring the
//  contents of the mixbuffer to the (two)
//  hardware channels (left and right, that is).
//
//  allegro does this now

void I_UpdateSound( void )
{
}

// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime.
// It is called during Timer interrupt with SNDINTR.

void I_SubmitSound(void)
{
  //this should no longer be necessary because
  //allegro is doing all the sound mixing now
}

void I_ShutdownSound(void)
{
  remove_sound();
}

//int snd_card_new;
//int mus_card_new;
//char midi_desc[160];
//char digi_desc[160];

static char _secondaries[256];

char _has_secondary_instrument(char inst)
{
  return _secondaries[(unsigned char) inst];
}


void I_InitSound(void)
{
  int lengths[NUMSFX];  // The actual lengths of all sound effects. -- killough
  int i;  // killough 10/98: eliminate snd_c since we use default_snd_card now

  snd_card=1;
  mus_card=1;

  // Secure and configure sound device first.
  fputs("I_InitSound: ", stdout);

  memset(_secondaries, 0, sizeof(_secondaries));

  // GB 2014: In practice, Allegro 3.12 legacy audio selection does not work...
  // Here is the manual conversion from 3.00 to 3.12 device IDs.
/*
  snd_card_new=snd_card;
  mus_card_new=mus_card;
  #if(ALLEGRO_VERSION>=3 && ALLEGRO_SUB_VERSION>=10)
    switch (snd_card) {
        case 1: snd_card_new=DIGI_SB;         break; 
        case 2: snd_card_new=DIGI_SB10;		  break;
        case 3: snd_card_new=DIGI_SB15;		  break;
        case 4: snd_card_new=DIGI_SB20;		  break;
        case 5: snd_card_new=DIGI_SBPRO;	  break;
        case 6: snd_card_new=DIGI_SB16;       break;
        case 7: snd_card_new=DIGI_WSS;        break;
    }					
    switch (mus_card) {
        case 1: mus_card_new=MIDI_ADLIB;      break;
        case 2: mus_card_new=MIDI_OPL2;       break;
        case 3: mus_card_new=MIDI_2XOPL2;     break;
        case 4: mus_card_new=MIDI_OPL3;       break;
        case 5: mus_card_new=MIDI_SB_OUT;     break;
        case 6: mus_card_new=MIDI_MPU;        break;
        case 8: mus_card_new=MIDI_DIGMID;     break;
        case 9: mus_card_new=MIDI_AWE32;      break;
    }
  #endif
*/
/*// GB 2016 Policy change: Use setup.exe + setup.cfg
  if (detect_voices && snd_card_new>=0 && mus_card_new>=0)
    {
      int mv;                          //jff 3/3/98 try it according to Allegro
      int dv = detect_digi_driver(snd_card_new); // detect the digital sound driver
      if (dv==0)
        snd_card_new=0;
      mv = detect_midi_driver(mus_card_new);     // detect the midi driver
      if (mv==-1)
        dv=mv=dv/2;          //note stealing driver, uses digital voices
      if (mv==0xffff)
        mv=-1;               //extern MPU-401 - unknown use default voices
      reserve_voices(dv,mv); // reserve the number of voices detected
    }                                  //jff 3/3/98 end of sound init changes
*/
//  if (install_sound(snd_card_new, mus_card_new, "none")!=0) //jff 1/18/98 autodect MIDI
    if (install_sound(DIGI_AUTODETECT , MIDI_AUTODETECT, "none")!=0) // GB 2016 Policy change: Use setup.exe + setup.cfg
    {
      printf("ALLEGRO SOUND INIT ERROR!\n%s\n", allegro_error); // killough 8/8/98
      //jff 1/22/98 on error, disable sound this invocation
      //in future - nice to detect if either sound or music might be ok
      //jff end disable sound this invocation
    }
  else //jff 1/22/98 don't register I_ShutdownSound if errored
    {
      //puts("configured audio device");  // killough 8/8/98
      //GB 2014, show sound driver:
      snd_card=1;
	  mus_card=1;
	  printf(" Sound: %s\n",digi_driver->desc);
    printf(" Music: %s\n",midi_driver->desc);
	  LOCK_VARIABLE(channel);  // killough 2/7/98: prevent VM swapping of sfx
      atexit(I_ShutdownSound); // killough

      if (!strcmp(digi_driver->name,"No sound")) {snd_card=0;} 
      if (!strcmp(midi_driver->name,"No sound")) {mus_card=0;}
    }

  // Initialize external data (all sounds) at start, keep static.
  fputs("I_InitSound: Load sound data",stdout); // killough 8/8/98 // GB 2014 added suffix

  for (i=1; i<NUMSFX; i++)
      S_sfx[i].snd_ptr = getsfx(S_sfx[i].lumpname, &lengths[i]);

  // Finished initialization.
  puts("\nI_InitSound: sound module ready");    // killough 8/8/98

  //rest(4000);
  //sleep(8); //uncomment for debugging
}

void I_LoadSoundBank(void *bank)
{
  // GB 2014
  // Allegro load_ibk: Reads in a .IBK patch set file, for use by the Adlib driver.
  // Returns non-zero on error. 
  // int load_ibk(char *filename, int drums)
  if (mus_card>0)
  {
     int i = 0;
     load_op2_extra(bank, _secondaries);
  } 
}

#ifdef DEFAULTCFG

extern int snd_Channels;
extern int snd_DesiredMusicDevice, snd_DesiredSfxDevice;
extern snd_SBport, snd_SBirq, snd_SBdma;       
extern int snd_Mport;                              


void I_GenerateAllegroCfg(char * fname)
{
  static char template[] = 
"[sound]\n"
"digi_card = %d\n"
"midi_card = %d\n"
"digi_volume = -1\n"
"midi_volume = -1\n"
"digi_voices = %d\n"
"midi_voices = -1\n"
"flip_pan = 0\n"
"sb_port = %x\n"
"sb_dma = -1\n"
"sb_irq = %d\n"
"sb_freq = 11111\n"
"fm_port = -1\n"
"mpu_port = %x\n"
"ibk_file =  \n"
"ibk_drum_file = \n"  
"patches = \n\n";
  static char asetup[sizeof(template) * 2];

  static char digi_lookup[] = {DIGI_NONE,
			       DIGI_NONE,
			       DIGI_NONE,
			       DIGI_SB,
			       DIGI_AUTODETECT,
			       DIGI_GUSPNP,
			       DIGI_NONE,
			       DIGI_NONE,
			       DIGI_NONE,
			       DIGI_NONE},
	      midi_lookup[] = {MIDI_NONE,
			       MIDI_NONE,
			       MIDI_ADLIB,
			       MIDI_ADLIB,
			       MIDI_AUTODETECT,
			       MIDI_GUS,
			       MIDI_MPU,
			       MIDI_MPU,
			       MIDI_MPU,
			       MIDI_AWE32,
			       MIDI_NONE    
};
  char digi = (snd_DesiredSfxDevice >= 0
      && snd_DesiredSfxDevice < sizeof(digi_lookup) / sizeof(digi_lookup[0])) ?
	digi_lookup[snd_DesiredSfxDevice] : DIGI_AUTODETECT;

  memset(asetup, 0, sizeof(asetup));
  sprintf(asetup, template, digi,
    (snd_DesiredMusicDevice >= 0
      && snd_DesiredMusicDevice < sizeof(midi_lookup) / sizeof(midi_lookup[0])) ?
	midi_lookup[snd_DesiredMusicDevice] : MIDI_AUTODETECT,
    digi != DIGI_NONE ? snd_Channels : 0,
    snd_SBport,
//    because of the hiccups with SB16 we leave it to liballeg to assign DMA
//    snd_SBdma,
    snd_SBirq,
    snd_Mport
  );
  set_config_data(asetup, strlen(asetup) + 1);
}
#endif


boolean I_IsMusicCardOPL(void)
{
  return midi_card > 0 && midi_card < MIDI_SB_OUT;
}

///
// MUSIC API.
//

// This is the number of active musics that these routines
// can handle at once, regardless of the mixer's ability
// (which we don't care about since allegro does the mixing)
// We set it to 1 to allow just one music at a time for now.

#define NUM_MIDICHAN 1

// mididata is used to buffer the current music.

static MIDI mididata;

void I_ShutdownMusic(void)
{
  stop_midi();          //jff 1/16/98 shut down midi
}

void I_InitMusic(void)
{
  atexit(I_ShutdownMusic); //jff 1/16/98 enable atexit routine for shutdown
}

// jff 1/18/98 changed interface to make mididata destroyable

void I_PlaySong(int handle, int looping)
{
  if (handle>=0)
    play_midi(&mididata,looping);       // start registered midi playing
}

void I_SetMusicVolume(int volume)
{
  // Internal state variable.
  snd_MusicVolume = volume;
  // Now set volume on output device.

  //jff 01/17/98 - add VOLSCALE-1 to get most out of volume
  set_volume(-1,snd_MusicVolume*VOLSCALE+VOLSCALE-1);   // jff 1/18/98
}

void I_PauseSong (int handle)
{
  if (handle>=0)
    midi_pause();       // jff 1/16/98 pause midi playing
}

void I_ResumeSong (int handle)
{
  if (handle>=0)
    midi_resume();      // jff 1/16/98 resume midi playing
}

void I_StopSong(int handle)
{
  if (handle>=0)
    stop_midi();        // jff 1/16/98 stop midi playing
}

void I_UnRegisterSong(int handle)
{
}

// jff 1/16/98 created to convert data to MIDI ala Allegro

int I_RegisterSong(void *data)
{
  int handle, err;

  //jff 1/21/98 just stop any midi currently playing
  stop_midi();

  // convert the MUS lump data to a MIDI structure
  //jff 1/17/98 make divisions 89, compression allowed
  if    //jff 02/08/98 add native midi support
    (
     (err=MidiToMIDI(data, &mididata)) &&       // try midi first
     (err=mmus2mid(data, &mididata, 89, 1, I_IsMusicCardOPL()))  // now try mus
     )
    {
      handle=-1;
      cprintf("Error loading midi: %d",err);
    }
  else
    {
      handle=0;
      lock_midi(&mididata);     // data must be locked for Allegro
    }
  //jff 02/08/98 add native midi support:
  return handle;                        // 0 if successful, -1 otherwise
}

// Is the song playing?
int I_QrySongPlaying(int handle)
{
  return 0;
}

//----------------------------------------------------------------------------
//
// GB 2015: Renamed dprintf to dmprintf, for DJGPP 2.05 compatibility
//
// $Log: i_sound.c,v $
// Revision 1.3  2000-08-12 21:29:34  fraggle
// change license header
//
// Revision 1.2  2000/08/01 20:23:05  fraggle
// stereo fix for allegro v3.12
//
// Revision 1.1.1.1  2000/07/29 13:20:39  fraggle
// imported sources
//
// Revision 1.15  1998/05/03  22:32:33  killough
// beautification, use new headers/decls
//
// Revision 1.14  1998/03/09  07:11:29  killough
// Lock sound sample data
//
// Revision 1.13  1998/03/05  00:58:46  jim
// fixed autodetect not allowed in allegro detect routines
//
// Revision 1.12  1998/03/04  11:51:37  jim
// Detect voices in sound init
//
// Revision 1.11  1998/03/02  11:30:09  killough
// Make missing sound lumps non-fatal
//
// Revision 1.10  1998/02/23  04:26:44  killough
// Add variable pitched sound support
//
// Revision 1.9  1998/02/09  02:59:51  killough
// Add sound sample locks
//
// Revision 1.8  1998/02/08  15:15:51  jim
// Added native midi support
//
// Revision 1.7  1998/01/26  19:23:27  phares
// First rev with no ^Ms
//
// Revision 1.6  1998/01/23  02:43:07  jim
// Fixed failure to not register I_ShutdownSound with atexit on install_sound error
//
// Revision 1.4  1998/01/23  00:29:12  killough
// Fix SSG reload by using frequency stored in lump
//
// Revision 1.3  1998/01/22  05:55:12  killough
// Removed dead past changes, changed destroy_sample to stop_sample
//
// Revision 1.2  1998/01/21  16:56:18  jim
// Music fixed, defaults for cards added
//
// Revision 1.1.1.1  1998/01/19  14:02:57  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
