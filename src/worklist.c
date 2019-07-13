
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: worklist.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static unsigned long savedmask;

void perform_worklist()
{
  int i;
  long tmplong; /* this temp must be signed */
  KeySym key;

  if (potflag && pot16bit)
    {
    int tmpcalcmode = stdcalcmode;

    stdcalcmode = '1'; /* force 1 pass */
    if (resuming == 0)
      {
      if (pot_startdisk() < 0)
        {
        pot16bit = 0;   /* startdisk failed or cancelled */
        stdcalcmode = tmpcalcmode;  /* maybe we can carry on??? */
        }
      }
    }

  if (stdcalcmode == 'b' && (curfractalspecific->flags & NOTRACE))
    stdcalcmode = '1';

  if (stdcalcmode == 'g' && (curfractalspecific->flags & NOGUESS))
    stdcalcmode = '1';

  /* default setup a new worklist */
  num_worklist = 1;
  worklist[0].xxstart = 0;
  worklist[0].yystart = worklist[0].yybegin = 0;
  worklist[0].xxstop = xdots - 1;
  worklist[0].yystop = ydots - 1;
  worklist[0].pass = worklist[0].sym = 0;

  if (resuming) /* restore worklist, if we can't the above will stay in place */
    {
    start_resume();
    get_resume(sizeof(int),&num_worklist,sizeof(worklist),worklist,0);
    end_resume();
    }

  if (distest) /* setup stuff for distance estimator */
    {
    double ftemp,ftemp2;
    dem_delta = sqr(delxx) + sqr(delyy2);
    if ((ftemp = sqr(delyy) + sqr(delxx2)) > dem_delta)
      dem_delta = ftemp;
    if (distestwidth == 0)
      distestwidth = 71;
    ftemp = distestwidth;
    dem_delta *= sqr(ftemp)/10000; /* multiply by thickness desired */
    dem_width = ( sqrt( sqr(xxmax-xxmin) + sqr(xx3rd-xxmin) ) * ydots/xdots
        + sqrt( sqr(yymax-yymin) + sqr(yy3rd-yymin) ) ) / distest;
    ftemp = (rqlim < DEM_BAILOUT) ? DEM_BAILOUT : rqlim;
    ftemp += 3; /* bailout plus just a bit */
    ftemp2 = log(ftemp);
    dem_toobig = sqr(ftemp) * sqr(ftemp2) * 4 / dem_delta;
    }

  while (num_worklist > 0)
    {
    calctype = curfractalspecific->calctype; /* per_image can override */
    symmetry = curfractalspecific->symmetry; /*   calctype & symmetry  */
    plot = putcolor; /* defaults when setsymmetry not called or does nothing */

    /* pull top entry off worklist */
    ixstart = xxstart = worklist[0].xxstart;
    ixstop  = xxstop  = worklist[0].xxstop;
    iystart = yystart = worklist[0].yystart;
    iystop  = yystop  = worklist[0].yystop;
    yybegin  = worklist[0].yybegin;
    workpass = worklist[0].pass;
    worksym  = worklist[0].sym;
    --num_worklist;
    for (i=0; i<num_worklist; ++i)
      worklist[i] = worklist[i+1];

    calc_status = IMG_ACTV_CALC; /* mark as in-progress */
    update_draw_menu(NEW_IMAGE);

    curfractalspecific->per_image();

    if(abort_process)
      {
      abort_process = 0;
      clear_image_cb(main_window, NULL, NULL);
      return;
      }

    /* some common initialization for escape-time pixel level routines */
    closenuff = delmin >> abs(periodicitycheck); /* for periodicity checking */
    closenuff /= fudge;
    lclosenuff = closenuff * fudge;   /* "close enough" value */
    kbdcount=max_kbdcount;
    /* savedmask is for calcmand's periodicity checking */
    savedmask = 0xC0000000; /* top 2 bits on */
    tmplong = (delmin >> abs(periodicitycheck)) | 1;
    while (tmplong > 0) /* while top bit not on */
      {
      tmplong <<= 1;
      savedmask = (savedmask >> 1) | 0x80000000;
      }

    setsymmetry(symmetry,1);

    /* added for testing autologmap() */
    if (!(resuming)&&(abs(LogFlag) ==2))
      {  /* calculate round screen edges to work out best start for logmap */
      LogFlag = ( autologmap() * (LogFlag / abs(LogFlag)));
      SetupLogTable();
      }

    /* call the appropriate escape-time engine */
    switch (stdcalcmode)
      {
      case 't':
        tesseral();
        break;
      case 'b':
        bound_trace_main();
        break;
      case 'g':
        solidguess();
        break;
      default:
        OneOrTwoPass();
      }

    if ((key = getakey()) != 0) /* interrupted? */
      break;
    }

  if (num_worklist > 0)
    {  /* interrupted, resumable */
    alloc_resume(sizeof(worklist)+10,1);
    put_resume(sizeof(int),&num_worklist,sizeof(worklist),worklist,0);
    calc_status = IMG_INTR_RESM;
    update_draw_menu(CONT_CALC);
    }
  else
    {
    calc_status = IMG_COMPLETED;
    update_draw_menu(NEW_IMAGE);
    }
}

void free_workarea()
{
   if(typespecific_workarea)
     {
     XtFree(typespecific_workarea);
     typespecific_workarea = NULL;
     }
   Store = (union FracArg **)0;
   Load = (union FracArg **)0;
   xmv = (struct ConstArg *)0;
   xmf = (void(**)(void))0;
   pfls = (struct fls *)0;

}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: worklist.c,v $
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
 * Revision 0.102  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.101  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
