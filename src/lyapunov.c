
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: lyapunov.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int lyapunov_cycles_in_c(int filter_cycles, double a, double b);

/* static variables */
int lyaLength, lyaSeedOK;
int lyaRxy[34];

int lyapunov()
{
  double a, b;

  if (check_key() != 0)
    return -1;
  overflow=FALSE;
  if (param[1]==1)
    Population = (1.0+rand())/(2.0+RAND_MAX);
  else if (param[1]==0)
    {
    if (fabs(Population)>BIG || Population==0 || Population==1)
      Population = (1.0+rand())/(2.0+RAND_MAX);
    }
  else
    Population = param[1];
  (*plot)(col, row, 1);
  if (invert)
    {
    invertz2(&init);
    a = init.y;
    b = init.x;
    }
  else
    {
    a = dy0[row]+dy1[col];
    b = dx0[col]+dx1[row];
    }
  color=lyapunov_cycles_in_c(filter_cycles, a, b);
  if (inside>0 && color==0)
    color = inside;
  else if (color>=colors)
    color = colors-1;
  (*plot)(col, row, color);
  return color;
}

  /*
   * This routine sets up the sequence for forcing the Rate parameter
   * to vary between the two values.  It fills the array lyaRxy[] and
   * sets lyaLength to the length of the sequence.
   *
   * The sequence is coded in the bit pattern in an integer.
   * Briefly, the sequence starts with an A the leading zero bits
   * are ignored and the remaining bit sequence is decoded.  The
   * sequence ends with a B.  Not all possible sequences can be
   * represented in this manner, but every possible sequence is
   * either represented as itself, as a rotation of one of the
   * representable sequences, or as the inverse of a representable
   * sequence (swapping 0s and 1s in the array.)  Sequences that
   * are the rotation and/or inverses of another sequence will generate
   * the same lyapunov exponents.
   *
   * A few examples follow:
   *     number    sequence
   *       0    ab
   *       1    aab
   *       2    aabb
   *       3    aaab
   *       4    aabbb
   *       5    aabab
   *       6    aaabb (this is a duplicate of 4, a rotated inverse)
   *       7    aaaab
   *       8    aabbbb    etc.
   */

int lya_setup()
{
  long i;
  int t;

  if ((filter_cycles=param[2])==0)
    filter_cycles=maxit/2;
  lyaSeedOK = param[1]>0 && param[1]<=1 && debugflag!=90;
  lyaLength = 1;

  i = param[0];
  lyaRxy[0] = 1;
  for (t=31; t>=0; t--)
    if (i & (1<<t))
      break;
  for (; t>=0; t--)
    lyaRxy[lyaLength++] = (i & (1<<t)) != 0;
  lyaRxy[lyaLength++] = 0;
  if (save_release<1732)        /* swap axes prior to 1732 */
    for (t=lyaLength; t>=0; t--)
      lyaRxy[t] = !lyaRxy[t];
  if (save_release<1731)
    {        /* ignore inside=, stdcalcmode */
    stdcalcmode='1';
    if (inside==1) inside = 0;
    }
  if (inside<0)
    {
    /*
    static char msg[]=
    {"Sorry, inside options other than inside=nnn are not supported by the lyapunov"};
    inform(main_window, (char *) &msg, NULL);
    */
    inside=1;
  }
  return 1;
}

static int lyapunov_cycles_in_c(int filter_cycles, double a, double b)
{
  int color, count, i, lnadjust;
  double lyap, total, temp;
  /* e10=22026.4657948  e-10=0.0000453999297625 */

  lyap = total = temp = 0.0;
  color = count = i = lnadjust = 0;

  for (i=0; i<filter_cycles; i++)
    {
    for (count=0; count<lyaLength; count++)
      {
      Rate = lyaRxy[count] ? a : b;
      if (curfractalspecific->orbitcalc())
        {
        overflow = TRUE;
        goto jumpout;
        }
      }
    }
  total = 1.0;
  lnadjust = 0;
  for (i=0; i < maxit/2; i++)
    {
    for (count = 0; count < lyaLength; count++)
      {
      Rate = lyaRxy[count] ? a : b;
      if (curfractalspecific->orbitcalc())
        {
        overflow = TRUE;
        goto jumpout;
        }
      temp = fabs(Rate-2.0*Rate*Population);
      if ((total *= (temp))==0)
        {
        overflow = TRUE;
        goto jumpout;
        }
      }
    while (total > 22026.4657948)
      {
      total *= 0.0000453999297625;
      lnadjust += 10;
      }
    while (total < 0.0000453999297625)
      {
      total *= 22026.4657948;
      lnadjust -= 10;
      }
    }

jumpout:
  if (overflow || total <= 0 || (temp = log(total) + lnadjust) > 0)
    color = 0;
  else
    {
    if (LogFlag)
      lyap = -temp/((double) lyaLength*i);
    else
      lyap = 1 - exp(temp/((double) lyaLength*i));
    color = 1 + (int)(lyap * (colors-1));
    }
  return color;
}

/******************* standalone engine for "lyapunov" *********************/
/*** Roy Murphy [76376,721]                                             ***/
/*** revision history:                                                  ***/
/*** initial version: Winter '91                                        ***/
/***    Fall '92 integration of Nicholas Wilt's ASM speedups            ***/
/***    Jan 93' integration with calcfrac() yielding boundary tracing,  ***/
/***    tesseral, and solid guessing, and inversion, inside=nnn         ***/
/*** save_release behavior:                                             ***/
/***    1730 & prior: ignores inside=, calcmode='1', (a,b)->(x,y)       ***/
/***    1731: other calcmodes and inside=nnn                            ***/
/***    1732: the infamous axis swap: (b,a)->(x,y),                     ***/
/***        the order parameter becomes a long int                      ***/
/**************************************************************************/

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: lyapunov.c,v $
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
