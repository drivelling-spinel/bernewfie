#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../m_dflts.h"


void I_Error(const char *error, ...) __attribute__((format(printf,1,2)));// called by anything that can generate a terminal error

#ifdef DEFAULTCFG

//==========================================================================
//
// M_LoadDefaults
//
//==========================================================================

void M_LoadDefaultsEarly(char *fileName)
{
	int i;
	int len;
	FILE *f;
	char def[80];
	char strparm[100];
	char *newstring;
	int parm;
	boolean isstring;

	// Set everything to base values
	M_SetBaseValues();

	// Check for a custom config file
	i = M_CheckParm("-config");
	if(i && i < myargc-1)
	{
		strcpy(defaultfile, myargv[i+1]);
//		ST_Message("config file: %s\n", defaultfile);
	}
	else if(M_ParmExists("-cdrom"))
	{
		sprintf(defaultfile, "c:\\hexndata\\%s", fileName);
	}
	else
	{
		strcpy(defaultfile, fileName);
	}

	// Scan the config file
	f = fopen(defaultfile, "r");
	if(f)
	{
		while(!feof(f))
		{
			isstring = false;
			if(fscanf(f, "%79s %[^\n]\n", def, strparm) == 2)
			{
				if(strparm[0] == '"')
				{
					 // Get a string default
					 isstring = true;
					 len = strlen(strparm);
					 newstring = (char *)malloc(len);
					 if (newstring == NULL) I_Error("can't malloc newstring");
					 strparm[len-1] = 0;
					 strcpy(newstring, strparm+1);
				}
				else if(strparm[0] == '0' && strparm[1] == 'x')
				{
					sscanf(strparm+2, "%x", &parm);
				}
				else
				{
					sscanf(strparm, "%i", &parm);
				}
				for(i = 0; i < numdefaults; i++)
				{
					if(!strcmp(def, defaults[i].name))
					{
						if(!isstring)
						{
							*defaults[i].location = parm;
						}
						else
						{
							*defaults[i].location = (int)newstring;
						}
						break;
					}
				}
			}
		}

		fclose (f);
	}

	// Translate the key scancodes
	for(i = 0; i < numdefaults; i++)
	{
		if(defaults[i].scantranslate)
		{
			parm = *defaults[i].location;
			defaults[i].untranslated = parm;
			*defaults[i].location = I_ScanCode2DoomCode(parm);;
		}
	}
}

#endif
