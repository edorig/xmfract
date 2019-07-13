
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: starfield.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

int starfield(void)
{
  int c;
  KeySym key;

  xmbusy = 1;

  Distribution = 30;
  Slope = 25;

  if (starfield_values[0] <   1.0) starfield_values[0] =   1.0;
  if (starfield_values[0] > 100.0) starfield_values[0] = 100.0;
  if (starfield_values[1] <   1.0) starfield_values[1] =   1.0;
  if (starfield_values[1] > 100.0) starfield_values[1] = 100.0;
  if (starfield_values[2] <   1.0) starfield_values[2] =   1.0;
  if (starfield_values[2] > 100.0) starfield_values[2] = 100.0;

  Distribution = (int)(starfield_values[0]);
  con  = (long)(((starfield_values[1]) / 100.0) * (1L << 16));
  Slope = (int)(starfield_values[2]);

  if (ValidateLuts(GreyFile) != 0)
    {
    static char msg[]={"Unable to load ALTERN.MAP"};
    caution(main_window, (char *) &msg, NULL);
    xmbusy = 0;
    return(-1);
    }

  spindac(0,0);     /* load it, but don't spin */

  for(row = 0; row < ydots; row++)
    {
    for(col = 0; col < xdots; col++)
      {
      if((key = waitkeypressed(0) != 0))
        {
        buzzer(1);
        xmbusy = 0;
        return(1);
        }
      c = getcolor(col, row);
      if(c == inside)
        c = colors-1;
      putcolor(col, row, GausianNumber(c, colors));
      }
    }
  buzzer(0);
  xmbusy = 0;
  return(0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: starfield.c,v $
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
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
