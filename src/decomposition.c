
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: decomposition.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

void decomposition()
{
  static double cos45     = 0.70710678118654750;   /* cos 45     degrees */
  static double sin45     = 0.70710678118654750;   /* sin 45     degrees */
  static double cos22_5   = 0.92387953251128670;   /* cos 22.5   degrees */
  static double sin22_5   = 0.38268343236508980;   /* sin 22.5   degrees */
  static double cos11_25  = 0.98078528040323040;   /* cos 11.25  degrees */
  static double sin11_25  = 0.19509032201612820;   /* sin 11.25  degrees */
  static double cos5_625  = 0.99518472667219690;   /* cos 5.625  degrees */
  static double sin5_625  = 0.09801714032956060;   /* sin 5.625  degrees */
  static double tan22_5   = 0.41421356237309500;   /* tan 22.5   degrees */
  static double tan11_25  = 0.19891236737965800;   /* tan 11.25  degrees */
  static double tan5_625  = 0.09849140335716425;   /* tan 5.625  degrees */
  static double tan2_8125 = 0.04912684976946725;   /* tan 2.8125 degrees */
  static double tan1_4063 = 0.02454862210892544;   /* tan 1.4063 degrees */

  static long lcos45     ; /* cos 45     degrees */
  static long lsin45     ; /* sin 45     degrees */
  static long lcos22_5   ; /* cos 22.5   degrees */
  static long lsin22_5   ; /* sin 22.5   degrees */
  static long lcos11_25  ; /* cos 11.25  degrees */
  static long lsin11_25  ; /* sin 11.25  degrees */
  static long lcos5_625  ; /* cos 5.625  degrees */
  static long lsin5_625  ; /* sin 5.625  degrees */
  static long ltan22_5   ; /* tan 22.5   degrees */
  static long ltan11_25  ; /* tan 11.25  degrees */
  static long ltan5_625  ; /* tan 5.625  degrees */
  static long ltan2_8125 ; /* tan 2.8125 degrees */
  static long ltan1_4063 ; /* tan 1.4063 degrees */

  static reset_fudge = -1;
  int temp = 0;
  int i;

  _LCMPLX lalt;
  _CMPLX alt;

  color = 0;

  if (integerfractal) /* the only case */
    {
    if (reset_fudge != fudge)
      {
      reset_fudge = fudge;
      lcos45     = cos45     *fudge;
      lsin45     = sin45     *fudge;
      lcos22_5   = cos22_5     *fudge;
      lsin22_5   = sin22_5     *fudge;
      lcos11_25  = cos11_25     *fudge;
      lsin11_25  = sin11_25     *fudge;
      lcos5_625  = cos5_625     *fudge;
      lsin5_625  = sin5_625     *fudge;
      ltan22_5   = tan22_5     *fudge;
      ltan11_25  = tan11_25     *fudge;
      ltan5_625  = tan5_625     *fudge;
      ltan2_8125 = tan2_8125  *fudge;
      ltan1_4063 = tan1_4063  *fudge;
      }

    if (lnew.y < 0)
      {
      temp = 2;
      lnew.y = -lnew.y;
      }
    if (lnew.x < 0)
      {
      ++temp;
      lnew.x = -lnew.x;
      }

    if (decomp[0] >= 8)
      {
      temp <<= 1;
      if (lnew.x < lnew.y)
        {
        ++temp;
        lalt.x = lnew.x; /* just */
        lnew.x = lnew.y; /* swap */
        lnew.y = lalt.x; /* them */
        }

      if (decomp[0] >= 16)
        {
        temp <<= 1;
        if (multiply(lnew.x,ltan22_5,bitshift) < lnew.y)
          {
          ++temp;
          lalt = lnew;
          lnew.x = multiply(lalt.x,lcos45,bitshift) +
          multiply(lalt.y,lsin45,bitshift);
          lnew.y = multiply(lalt.x,lsin45,bitshift) -
          multiply(lalt.y,lcos45,bitshift);
          }

        if (decomp[0] >= 32)
          {
          temp <<= 1;
          if (multiply(lnew.x,ltan11_25,bitshift) < lnew.y)
            {
            ++temp;
            lalt = lnew;
            lnew.x = multiply(lalt.x,lcos22_5,bitshift) +
              multiply(lalt.y,lsin22_5,bitshift);
            lnew.y = multiply(lalt.x,lsin22_5,bitshift) -
              multiply(lalt.y,lcos22_5,bitshift);
            }

          if (decomp[0] >= 64)
            {
            temp <<= 1;
            if (multiply(lnew.x,ltan5_625,bitshift) < lnew.y)
              {
              ++temp;
              lalt = lnew;
              lnew.x = multiply(lalt.x,lcos11_25,bitshift) +
              multiply(lalt.y,lsin11_25,bitshift);
              lnew.y = multiply(lalt.x,lsin11_25,bitshift) -
              multiply(lalt.y,lcos11_25,bitshift);
              }

            if (decomp[0] >= 128)
              {
              temp <<= 1;
              if (multiply(lnew.x,ltan2_8125,bitshift) < lnew.y)
              {
              ++temp;
              lalt = lnew;
              lnew.x = multiply(lalt.x,lcos5_625,bitshift) +
                multiply(lalt.y,lsin5_625,bitshift);
              lnew.y = multiply(lalt.x,lsin5_625,bitshift) -
                multiply(lalt.y,lcos5_625,bitshift);
              }

              if (decomp[0] == 256)
                {
                temp <<= 1;
                if (multiply(lnew.x,ltan1_4063,bitshift) < lnew.y)
                  if ((lnew.x*ltan1_4063 < lnew.y))
                    ++temp;
                }
              }
            }
          }
        }
      }
    }
  else /* double case */
    {
    if (new.y < 0)
      {
      temp = 2;
      new.y = -new.y;
      }
    if (new.x < 0)
      {
      ++temp;
      new.x = -new.x;
      }
    if (decomp[0] >= 8)
      {
      temp <<= 1;
      if (new.x < new.y)
        {
        ++temp;
        alt.x = new.x; /* just */
        new.x = new.y; /* swap */
        new.y = alt.x; /* them */
        }
      if (decomp[0] >= 16)
        {
        temp <<= 1;
        if (new.x*tan22_5 < new.y)
          {
          ++temp;
          alt = new;
          new.x = alt.x*cos45 + alt.y*sin45;
          new.y = alt.x*sin45 - alt.y*cos45;
          }

        if (decomp[0] >= 32)
          {
          temp <<= 1;
          if (new.x*tan11_25 < new.y)
            {
            ++temp;
            alt = new;
            new.x = alt.x*cos22_5 + alt.y*sin22_5;
            new.y = alt.x*sin22_5 - alt.y*cos22_5;
            }

          if (decomp[0] >= 64)
            {
            temp <<= 1;
            if (new.x*tan5_625 < new.y)
              {
              ++temp;
              alt = new;
              new.x = alt.x*cos11_25 + alt.y*sin11_25;
              new.y = alt.x*sin11_25 - alt.y*cos11_25;
              }

            if (decomp[0] >= 128)
              {
              temp <<= 1;
              if (new.x*tan2_8125 < new.y)
                {
                ++temp;
                alt = new;
                new.x = alt.x*cos5_625 + alt.y*sin5_625;
                new.y = alt.x*sin5_625 - alt.y*cos5_625;
                }

              if (decomp[0] == 256)
                {
                temp <<= 1;
                if ((new.x*tan1_4063 < new.y))
                  ++temp;
                }
              }
            }
          }
        }
      }
    }
  for (i = 1; temp > 0; ++i)
   {
   if (temp & 1)
     color = (1 << i) - 1 - color;
   temp >>= 1;
   }

  if (decomp[0] == 2)
    color &= 1;
  if (colors > decomp[0])
    color++;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: decomposition.c,v $
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
