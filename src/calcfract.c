
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: calcfract.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void init_misc(void);

int calcfract()
{
  char msg[128];

  /* default to no known finite attractors */
  attractors = 0;

  display3d = 0;
  basin = 0;

  init_misc();

  /* following delta values useful only for types with rotation disabled */
  /* currently used only by bifurcation */
  if (integerfractal)
    {
    distest = 0;
    deltaX = (double)lx0[  1] / fudge - xxmin;
    deltaY = yymax - (double)ly0[  1] / fudge;
    }
  else
    {
    deltaX = dx0[1] - xxmin;
    deltaY = yymax - dy0[1];
    }

  parm.x   = param[0];
  parm.y   = param[1];
  parm2.x  = param[2];
  parm2.y  = param[3];

  if (LogFlag && colors < 16)
    {
      sprintf((char *) &msg, "%s", "Need at least 16 colors to use logmap");
      inform(main_window, (char *) &msg, NULL);
      LogFlag = 0;
    }

  if (LogFlag || rangeslen)
    {
    if(!(LogTable =
      (unsigned char *)XtMalloc((sizeof(unsigned char)) * (long)maxit + 1)))
      {
      sprintf((char *) &msg, "%s",
        "Insufficient memory for logmap/ranges with this maxiter");
      fatal(main_window, (char *) &msg);
      exit(1);
      }
    else if (rangeslen)
      {
      int i,k,l,m,numval,flip,altern;
      i = k = l = 0;
      while (i < rangeslen)
        {
        m = flip = 0;
        altern = 32767;
        if ((numval = ranges[i++]) < 0)
          {
          altern = ranges[i++];    /* sub-range iterations */
          numval = ranges[i++];
          }
        if (numval > maxit || i >= rangeslen)
          numval = maxit;
        while (l <= numval)
          {
          LogTable[l++] = k + flip;
          if (++m >= altern)
            {
            flip ^= 1;        /* Alternate colors */
            m = 0;
            }
          }
        ++k;
        if (altern != 32767)
          ++k;
        }
      }
    else
      {
      SetupLogTable();
      }
    }

  /* calcmand magnitude limit */
  lm = 4L << bitshift;

  /* ORBIT stuff */
  save_orbit = (int *)((double *)dx0 + 4*MAXPIXELS);
  show_orbit = 0;
  orbit_ptr = 0;
  orbit_color = 15;

  if(colors < 16)
    orbit_color = 1;

  if(inversion[0] != 0.0)
    {
    f_radius  = inversion[0];
    f_xcenter = inversion[1];
    f_ycenter = inversion[2];

    /* auto calc radius 1/6 screen */
    if (inversion[0] == AUTOINVERT)
      inversion[0] = f_radius =
        min(fabs(xxmax - xxmin), fabs(yymax - yymin)) / 6.0;

    /* xcenter not already set */
    if (invert < 2 || inversion[1] == AUTOINVERT)
      {
      inversion[1] = f_xcenter = (xxmin + xxmax) / 2.0;
      if (fabs(f_xcenter) < fabs(xxmax-xxmin) / 100)
        inversion[1] = f_xcenter = 0.0;
      }

    /* ycenter not already set */
    if (invert < 3 || inversion[2] == AUTOINVERT)
      {
      inversion[2] = f_ycenter = (yymin + yymax) / 2.0;
      if (fabs(f_ycenter) < fabs(yymax-yymin) / 100)
        inversion[2] = f_ycenter = 0.0;
      }

    /* so values will not be changed if we come back */
    invert = 3;
    }

  /* for periodicity checking */
  closenuff = delmin >> abs(periodicitycheck);
  closenuff /= fudge;
  rqlim_save = rqlim;
  rqlim2 = sqrt(rqlim);

  /* for integer routines (lambda) */
  if (integerfractal)
    {
    lparm.x = parm.x * fudge;             /* real portion of Lambda */
    lparm.y = parm.y * fudge;             /* imaginary portion of Lambda */
    lparm2.x = parm2.x * fudge;           /* real portion of Lambda2 */
    lparm2.y = parm2.y * fudge;           /* imaginary portion of Lambda2 */
    llimit = rqlim * fudge;               /* stop if magnitude exceeds this */
    if (llimit <= 0) llimit = 0x7fffffff; /* klooge for integer math */
    llimit2 = rqlim2 * fudge;             /* stop if magnitude exceeds this */
    lclosenuff = closenuff * fudge;       /* "close enough" value */
    l16triglim = 8L<<16;                  /* domain limit of fast trig funcs */
    linitorbit.x = initorbit.x * fudge;
    linitorbit.y = initorbit.y * fudge;
    }

  resuming = (calc_status == IMG_INTR_RESM);

  /* free resume_info memory if any is hanging around */
  if (!resuming)
    {
    end_resume();
    if (resave_flag)
      {
      updatesavename(savename);
      resave_flag = started_resaves = 0;
      }
    calctime = 0;
    }

  if (curfractalspecific->calctype != StandardFractal &&
      curfractalspecific->calctype != calcmand        &&
      curfractalspecific->calctype != calcmandfp      &&
      curfractalspecific->calctype != lyapunov        &&
      curfractalspecific->calctype != calcfroth)
    {
    calctype = curfractalspecific->calctype; /* per_image can override */
    symmetry = curfractalspecific->symmetry; /*   calctype & symmetry  */
    plot = putcolor; /* defaults when setsymmetry not called or does nothing */

    iystart = ixstart = yystart = xxstart = yybegin = 0;

    iystop = yystop = ydots -1;
    ixstop = xxstop = xdots -1;

    calc_status = IMG_ACTV_CALC; /* mark as in-progress */
    update_draw_menu(NEW_IMAGE);
    distest = 0; /* only standard escape time engine supports distest */

    /* per_image routine is run here */
    if (curfractalspecific->per_image()) /* not a stand-alone */
      {
      /* next two lines in case periodicity changed */
      closenuff = delmin >> abs(periodicitycheck); /* periodicity checking */
      closenuff /= fudge;
      lclosenuff = closenuff * fudge;    /* "close enough" value */
      setsymmetry(symmetry,0);
      timer(0,calctype); /* non-standard fractal engine */
      }
    if (havekey() != 0)
      {
      if (calc_status == IMG_ACTV_CALC) /* calctype didn't set this itself, */
        {
        /* so mark it interrupted, non-resumable */
        calc_status = IMG_INTR_NRES;
        update_draw_menu(NEW_IMAGE);
        }
      }
    else
      {
      calc_status = IMG_COMPLETED; /* no key, so assume it completed */
      update_draw_menu(NEW_IMAGE);
      }
    }
  else
    timer(0,(int (*)()) perform_worklist);

  calctime += timer_interval;

  if(LogTable)
    {
    XtFree((char *) LogTable);
    LogTable = NULL;
    }

  if(typespecific_workarea)
    free_workarea();

  return((calc_status == IMG_COMPLETED) ? 0 : -1);
}

void invertz2(_CMPLX *z)
{
  z->x = dx0[col]+dx1[row];
  z->y = dy0[row]+dy1[col];

  /* Normalize values to center of circle */
  z->x -= f_xcenter;
  z->y -= f_ycenter;

  /* Get old radius */
  tempsqrx = sqr(z->x) + sqr(z->y);

  if(fabs(tempsqrx) > FLT_MIN)
    tempsqrx = f_radius / tempsqrx;
  else
    /* a big number, but not TOO big */
    tempsqrx = FLT_MAX;

  /* Perform inversion */
  z->x *= tempsqrx;
  z->y *= tempsqrx;

  /* Renormalize */
  z->x += f_xcenter;
  z->y += f_ycenter;
}

static void init_misc()
{
  static struct ConstArg xmv1[5];
  static union FracArg argfirst;
  static union FracArg argsecond;

  if(!xmv) xmv = xmv1;
  Arg1 = &argfirst;
  Arg2 = &argsecond;
  xmfg = (double)(1L << bitshift);
  bitshiftless1 = bitshift-1;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: calcfract.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.6  1995/06/09  22:54:11  darryl
 * Change the atomic size of the LogTable to unsigned char.
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
 * Revision 0.103  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.102  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.101  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
