
//**************************************************************************
//**
//** f_finale.c : Heretic 2 : Raven Software, Corp.
//**
//** $RCSfile: f_finale.c,v $
//** $Revision: 1.7 $
//** $Date: 96/01/05 23:33:26 $
//** $Author: bgokey $
//**
//**************************************************************************

// HEADER FILES ------------------------------------------------------------

#include "h2def.h"
#include "soundst.h"
#include "p_local.h"
#include <ctype.h>

// MACROS ------------------------------------------------------------------

#define	TEXTSPEED	3
#define	TEXTWAIT	250

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

static void TextWrite(void);
static void DrawPic(void);
static void InitializeFade(boolean fadeIn);
static void DeInitializeFade(void);
static void FadePic(void);
static char *GetFinaleText(int sequence);

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

extern boolean automapactive;
extern boolean viewactive;

// PUBLIC DATA DECLARATIONS ------------------------------------------------

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static int FinaleStage;
static int FinaleCount;
static int FinaleEndCount;
static int FinaleLumpNum;
static int FontABaseLump;
static char *FinaleText;

static fixed_t *Palette;
static fixed_t *PaletteDelta;
static byte *RealPalette;
static byte *MonochromePalette;

static int goffsx, goffsy;

// CODE --------------------------------------------------------------------

//===========================================================================
//
// F_StartFinale
//
//===========================================================================

void F_StartFinale (void)
{
	gameaction = ga_nothing;
	gamestate = GS_FINALE;
	viewactive = false;
	automapactive = false;
	P_ClearMessage(&players[consoleplayer]);

	FinaleStage = 0;
	FinaleCount = 0;
	FinaleText = GetFinaleText(0);
	FinaleEndCount = 70;
	FinaleLumpNum = W_GetNumForName("FINALE1");
	FontABaseLump = W_GetNumForName("FONTA_S")+1;
	InitializeFade(1);

//	S_ChangeMusic(mus_victor, true);
	S_StartSongName("hall", false); // don't loop the song
}

//===========================================================================
//
// F_Responder
//
//===========================================================================

boolean F_Responder(event_t *event)
{
	return false;
}

//===========================================================================
//
// F_Ticker
//
//===========================================================================

void F_Ticker (void)
{
	FinaleCount++;
	if(FinaleStage < 5 && FinaleCount >= FinaleEndCount)
	{
		FinaleCount = 0;
		FinaleStage++;
		switch(FinaleStage)
		{
			case 1: // Text 1
				FinaleEndCount = strlen(FinaleText)*TEXTSPEED+TEXTWAIT;
				break;
			case 2: // Pic 2, Text 2
				FinaleText = GetFinaleText(1);
				FinaleEndCount = strlen(FinaleText)*TEXTSPEED+TEXTWAIT;
				FinaleLumpNum = W_GetNumForName("FINALE2");
				S_StartSongName("orb", false);
				break;
			case 3: // Pic 2 -- Fade out
				FinaleEndCount = 70;
				DeInitializeFade();
				InitializeFade(0);
				break;
			case 4: // Pic 3 -- Fade in
				FinaleLumpNum = W_GetNumForName("FINALE3");
				FinaleEndCount = 71;
				DeInitializeFade();
				InitializeFade(1);
				S_StartSongName("chess", true);
				break;
			case 5: // Pic 3 , Text 3
				FinaleText = GetFinaleText(2);
				DeInitializeFade();
				break;
			default:
				 break;
		}
		return;
	}
	if(FinaleStage == 0 || FinaleStage == 3 || FinaleStage == 4)
	{
		FadePic();
	}
}

//===========================================================================
//
// TextWrite
//
//===========================================================================

static void TextWrite (void)
{
	int		count;
	char	*ch;
	int		c;
	int		cx, cy;
	patch_t *w;

	V_DrawRawScreen(W_CacheLumpNum(FinaleLumpNum, PU_CACHE));
	if(FinaleStage == 5)
	{ // Chess pic, draw the correct character graphic
		if(netgame)
		{
			V_DrawPatch(goffsx + 20, goffsy + 0, W_CacheLumpName("chessall", PU_CACHE));
		}
		else if(PlayerClass[consoleplayer])
		{
			V_DrawPatch(goffsx + 60, goffsy + 0, W_CacheLumpNum(W_GetNumForName("chessc")
				+PlayerClass[consoleplayer]-1, PU_CACHE));
		}
	}
	// Draw the actual text
	if(FinaleStage == 5)
	{
		cy = 135;
	}
	else
	{
		cy = 5;
	}
	cx = 20;
	ch = FinaleText;
	count = (FinaleCount-10)/TEXTSPEED;
	if (count < 0)
	{
		count = 0;
	}
	for(; count; count--)
	{
		c = *ch++;
		if(!c)
		{
			break;
		}
		if(c == '\n')
		{
			cx = 20;
			cy += 9;
			continue;
		}
		if(c < 32)
		{
			continue;
		}
		c = toupper(c);
		if(c == 32)
		{
			cx += 5;
			continue;
		}
		w = W_CacheLumpNum(FontABaseLump+c-33, PU_CACHE);
		if(cx+w->width > SCREENWIDTH)
		{
			break;
		}
		V_DrawPatch(goffsx + cx, goffsy + cy, w);
		cx += w->width;
	}
}

//===========================================================================
//
// InitializeFade
//
//===========================================================================

static void InitializeFadeInternal(boolean fadeIn, byte * pal)
{

	unsigned i;

	Palette = Z_Malloc(768*sizeof(fixed_t), PU_STATIC, 0);
	PaletteDelta = Z_Malloc(768*sizeof(fixed_t), PU_STATIC, 0);
	RealPalette = Z_Malloc(768*sizeof(byte), PU_STATIC, 0);

	if(fadeIn)
	{
		memset(RealPalette, 0, 768*sizeof(byte));
		for(i = 0; i < 768; i++)
		{
			Palette[i] = 0;
                        PaletteDelta[i] = FixedDiv((*(pal+i))<<FRACBITS, 70*FRACUNIT);
		}
	}
	else
	{
		for(i = 0; i < 768; i++)
		{
                        RealPalette[i] = *(pal+i);
			Palette[i] = RealPalette[i]<<FRACBITS;
			PaletteDelta[i] = FixedDiv(Palette[i], -70*FRACUNIT);
		}
	}
	I_SetPalette(RealPalette);
}

static void InitializeMonochromeFade(boolean fadeIn, int color)
{
  int i = 0;
  MonochromePalette = Z_Malloc(768*sizeof(byte), PU_STATIC, 0);
  memset(MonochromePalette, 0, 768*sizeof(byte));
  for(i = 6 ; i < 768 ; i += 3)
    {
      MonochromePalette[i] = color >> 16;
      MonochromePalette[i+1] = color >> 8;
      MonochromePalette[i+2] = color;
    }
  InitializeFadeInternal(fadeIn, MonochromePalette);
}

static void InitializeFade(boolean fadeIn)
{
  InitializeFadeInternal(fadeIn, (byte *)W_CacheLumpName("playpal", PU_CACHE));
}

//===========================================================================
//
// DeInitializeFade
//
//===========================================================================

static void DeInitializeFade(void)
{
	Z_Free(Palette);
        Palette = 0;
	Z_Free(PaletteDelta);
        PaletteDelta = 0;
	Z_Free(RealPalette);
        RealPalette = 0;
        Z_Free(MonochromePalette);
        MonochromePalette = 0;
}

//===========================================================================
//
// FadePic
//
//===========================================================================

static void FadePic(void)
{
	unsigned i;

	for(i = 0; i < 768; i++)
	{
		Palette[i] += PaletteDelta[i];
		RealPalette[i] = Palette[i]>>FRACBITS;
	}
	I_SetPalette(RealPalette);
}

//===========================================================================
//
// DrawPic
//
//===========================================================================

static void DrawPic(void)
{
 
	V_DrawRawScreen(W_CacheLumpNum(FinaleLumpNum, PU_CACHE));
	if(FinaleStage == 4 || FinaleStage == 5)
	{ // Chess pic, draw the correct character graphic
		if(netgame)
		{
			V_DrawPatch(goffsx + 20, goffsy +0, W_CacheLumpName("chessall", PU_CACHE));
		}
		else if(PlayerClass[consoleplayer])
		{
			V_DrawPatch(goffsx + 60, goffsy +0, W_CacheLumpNum(W_GetNumForName("chessc")
				+PlayerClass[consoleplayer]-1, PU_CACHE));
		}
	}
}

//===========================================================================
//
// F_Drawer
//
//===========================================================================

void F_Drawer(void)
{
  goffsx = (SCREENWIDTH - LORESWIDTH) / 2;
  goffsy = (SCREENHEIGHT- LORESHEIGHT) / 2;

	switch(FinaleStage)
	{
		case 0: // Fade in initial finale screen
			DrawPic();
			break;
		case 1:
		case 2:
			TextWrite();
			break;
		case 3: // Fade screen out
			DrawPic();
			break;
		case 4: // Fade in chess screen
			DrawPic();
			break;
		case 5:
			TextWrite();
			break;
	}
	UpdateState |= I_FULLSCRN;	
}

//==========================================================================
//
// GetFinaleText
//
//==========================================================================

static char *GetFinaleText(int sequence)
{
	char *msgLumpName;
	int msgSize;
	int msgLump;
	static char *winMsgLumpNames[] =
	{
		"win1msg",
		"win2msg",
		"win3msg"
	};

	msgLumpName = winMsgLumpNames[sequence];
	msgLump = W_GetNumForName(msgLumpName);
	msgSize = W_LumpLength(msgLump);
	if(msgSize >= MAX_INTRMSN_MESSAGE_SIZE)
	{
		I_Error("Finale message too long (%s)", msgLumpName);
	}
	W_ReadLump(msgLump, ClusterMessage);
	ClusterMessage[msgSize] = 0; // Append terminator
	return ClusterMessage;
}

void F_FadeToBlackInit(int color)
{
  DeInitializeFade();
  InitializeMonochromeFade(0, color);
}


void F_DoFadeToBlack(int color)
{
  int fade=70;

  rest(1000);
  S_StartSound(NULL, SFX_STARTUP_TICK);
  rest(1000);
  S_StartSound(NULL, SFX_STARTUP_TICK);
  rest(1000);
  S_StartSound(NULL, SFX_STARTUP_TICK);

  while(fade-->0)
    {
      FadePic();
      rest(1000/35);
      if(fade % 16 == 0) S_StartSound(NULL, SFX_STARTUP_TICK);
    }
}

