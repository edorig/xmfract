
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: loadmap.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static long *tga32;
static unsigned *tga16;

void SetTgaColors()
{
  unsigned r, g, b, index;

  if (tga16 != NULL)
    for( index = 0; index < 256; index++ )
      {
      r = dacbox[index][0];
      g = dacbox[index][1];
      b = dacbox[index][2];
      tga16[index] = ((r&248)<<7) | ((g&248)<<2) | (b>>3);
      tga32[index] = ((long)r<<16) | (g<<8) | b;
      }
}

int ValidateLuts( char * fn )
{
  FILE * f;
  unsigned r, g, b, index;
  char line[160];
  char temp[81];

  strcpy (temp,fn);

  if (strchr(temp,'.') == NULL) /* Did name have an extension? */
    strcat(temp,".map");  /* No? Then add .map */

  findpath( temp, line);          /* search the dos path */

  f = fopen( line, "r" );

  if (f == NULL)
    {
    sprintf(line,"Could not load color map %s",fn);
    caution(main_window, (char *)&line, NULL);
    return 1;
    }

  for( index = 0; index < 256; index++ )
    {
    if (fgets(line,100,f) == NULL)
      break;

    sscanf( line, "%u %u %u", &r, &g, &b );

    /** load global dac values **/
    dacbox[index][0] = (r%256);
    dacbox[index][1] = (g%256);
    dacbox[index][2] = (b%256);
    }
  fclose( f );

  while (index < 256)
    { /* zap unset entries */
    dacbox[index][0] = dacbox[index][2] = dacbox[index][1] = 40;
    ++index;
    }
  SetTgaColors();
  create_xor_gc();
  colorstate = 2;
  strcpy(colorfile,fn);
  return 0;
}

int SetColorPaletteName( char * fn )
{
  if( ValidateLuts( fn ) != 0)
    return 1;

  sprintf((char *)&mapfilename, "%s", fn);

  if(mapdacbox == NULL &&
    ((mapdacbox = (unsigned char *)XtMalloc(768L)) == NULL))
    {
    static char msg[]={"Insufficient memory for color map."};
    warning(main_window, (char *)&msg, NULL);
    return 1;
    }

  memcpy((char *)mapdacbox,(char *)dacbox,768);
  /* PB, 900829, removed atexit(RestoreMap) stuff, goodbye covers it */
  return 0;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: loadmap.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.201  1994/12/08  19:27:17  darryl
 * Ensure that the xgc contains the special colors as the
 * foreground and background when the color map changes.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
