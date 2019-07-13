
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: oneortwopass.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int StandardCalc(int passnum);

int OneOrTwoPass()
{
  int i;
  totpasses = 1;

  if (stdcalcmode == '2')
    totpasses = 2;

  if (stdcalcmode == '2' && workpass == 0) /* do 1st pass of two */
    {
    curpass = 1;
    if (StandardCalc(1) == -1)
      {
      add_worklist(xxstart,xxstop,yystart,yystop,row,0,worksym);
      return(-1);
      }
    if (num_worklist > 0) /* worklist not empty, defer 2nd pass */
      {
      add_worklist(xxstart,xxstop,yystart,yystop,yystart,1,worksym);
      return(0);
      }
    workpass = 1;
    yybegin = yystart;
    }
  /* second or only pass */
  if (StandardCalc(2) == -1)
    {
    if(totpasses == 1)
      curpass = 1;
    else
      curpass = 2;
    i = yystop;
    if (iystop != yystop) /* must be due to symmetry */
      i -= row - iystart;
    add_worklist(xxstart,xxstop,row,i,row,workpass,worksym);
    return(-1);
    }
  return(0);
}

static int StandardCalc(int passnum)
{
  got_status = 0;
  row = yybegin;

  while (row <= iystop)
    {
    currow = row;
    reset_periodicity = 1;
    col = ixstart;

    while (col <= ixstop)
      {
      if(showdot>0)
        (*plot) (col, row, showdot&(colors-1));

      /* on 2nd pass of two, skip even pts */
      if (passnum == 1 || stdcalcmode == '1' || (row&1) != 0 || (col&1) != 0)
        {
        /* StandardFractal(), calcmand() or calcmandfp() */
        if ((*calctype)() == -1)
          return(-1); /* interrupted */

        reset_periodicity = 0;

        if (passnum == 1) /* first pass, copy pixel and bump col */
          {
          if ((row&1) == 0 && row < iystop)
            {
            (*plot)(col,row+1,color);

            if ((col&1) == 0 && col < ixstop)
              {
              (*plot)(col+1,row+1,color);
              }
            }
          if ((col&1) == 0 && col < ixstop)
            {
            (*plot)(++col,row,color);
            }
          }
        }
      ++col;
      }
    if (passnum == 1 && (row&1) == 0)
      {
      ++row;
      }
    ++row;
    }
  return(0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: oneortwopass.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.5  1995/04/18  15:59:59  darryl
 * Fix problems and incorporate enhancements from beta test.
 *  -- not able to zoom after loading a saved image.
 *  -- string parsing would fail because the tags were incorrect.
 *  -- CLK_TCK is determined from sysconf() if it is available.
 *  -- Julia sets were not calculated correctly after image save.
 *  -- some void pointers were not cast before promotion.
 *  -- Info About Image display says it was working on pass 2 of 1.
 *  -- an incomplete image could not be saved.
 *  -- a saved image was not drawn if the refresh rate was < 100.
 *  -- added Refresh Rate menu item and command line support.
 *  -- added Parameter Forwarding menu item and command line support.
 *  -- added fallback resources.
 *  -- enhanced parameter handling.
 *  -- implemented the Fractint zoombox 'panning' capability.
 *  -- enhanced the LogMap and LogTable handling.
 *  -- various typograhic error fixes.
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
