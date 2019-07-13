
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: calcfracinit.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static long fudgetolong(double d);
static double fudgetodouble(long l);
static int ratio_bad(double actual, double desired);
static void adjust_to_limits(double expand);
static void smallest_add(double *num);
static int combine_worklist(void);

/* static variables */
static int resume_offset;
static long delx2;
static long dely2;

void calcfracinit()
{
  int i;
  double ftemp;

  floatflag = usr_floatflag;
  if (calc_status == IMG_INTR_RESM)
    {
    if (curfractalspecific->isinteger)
      floatflag = '0';
    else
      floatflag = '1';
    update_draw_menu(CONT_CALC);
    }

  if (!curfractalspecific->isinteger &&
      curfractalspecific->tofloat == NOFRACTAL)
    {
    floatflag = '1';
    }

init_restart:

  /*
   * the following variables may be forced to a different setting due to
   * calc routine constraints;  usr_xxx is what the user last said is wanted,
   * xxx is what we actually do in the current situation
   */

  stdcalcmode      = usr_stdcalcmode;
  periodicitycheck = usr_periodicitycheck;
  distest      = usr_distest;
  biomorph     = usr_biomorph;

  potflag = 0;

  if (potparam[0] != 0.0 && colors >= 64 &&
     (curfractalspecific->calctype == StandardFractal ||
      curfractalspecific->calctype == calcmand        ||
      curfractalspecific->calctype == calcmandfp))
    {
    potflag = 1;
    distest = 0;
    }

  if (distest)
    floatflag = '1';  /* force floating point for dist est */

  /* ensure type matches floatflag */
  if (floatflag)
    {
    if (curfractalspecific->isinteger != 0 &&
        curfractalspecific->tofloat != NOFRACTAL)
      {
      fractype = curfractalspecific->tofloat;
      }
    }
  else
    {
    if (curfractalspecific->isinteger == 0 &&
        curfractalspecific->tofloat != NOFRACTAL)
      {
      fractype = curfractalspecific->tofloat;
      }
    }

  /* match Julibrot with integer mode of orbit */
  if(fractype == JULIBROTFP && fractalspecific[neworbittype].isinteger)
    {
    int i;
    if((i=fractalspecific[neworbittype].tofloat) != NOFRACTAL)
      neworbittype = i;
    else
      fractype = JULIBROT;
    }
  else if (fractype == JULIBROT && fractalspecific[neworbittype].isinteger==0)
    {
    int i;
    if((i=fractalspecific[neworbittype].tofloat) != NOFRACTAL)
      neworbittype = i;
    else
      fractype = JULIBROTFP;
    }

  curfractalspecific = &fractalspecific[fractype];

  integerfractal = curfractalspecific->isinteger;

  if (potflag && potparam[2] != 0.0)
    rqlim = potparam[2];
  else if (bailout) /* user input bailout */
    rqlim = bailout;
  else if (biomorph != -1) /* biomorph benefits from larger bailout */
    rqlim = 100;
  else
    rqlim = curfractalspecific->orbit_bailout;

  /* the bailout limit mustn't be too high here */
  if (integerfractal)
    if (rqlim > 127.0)
      rqlim = 127.0;

  /* ensure min < max and unrotated rectangle */

  if ((curfractalspecific->flags&NOROTATE) != 0)
    {
    if (xxmin > xxmax)
      {
      ftemp = xxmax; xxmax = xxmin;
      xxmin = ftemp;
      }
    if (yymin > yymax)
      {
      ftemp = yymax; yymax = yymin;
      yymin = ftemp;
      }
    xx3rd = xxmin; yy3rd = yymin;
    }

  /* set up bitshift for integer math */

  bitshift = FUDGEFACTOR2; /* by default, the smaller shift */

  if (integerfractal > 1)  /* use specific override from table */
    bitshift = integerfractal;

  if (integerfractal == 0) /* float? */
    if ((i = curfractalspecific->tofloat) != NOFRACTAL) /* -> int? */
      if (fractalspecific[i].isinteger > 1) /* specific shift? */
        bitshift = fractalspecific[i].isinteger;

  /* adust shift bits if.. */
  if (fractype == MANDEL || fractype == JULIA)
    {
    if (potflag == 0 &&
       (param[0] > -2.0 && param[0] < 2.0) &&
       (param[1] > -2.0 && param[1] < 2.0) &&
       !invert && biomorph == -1 && rqlim <= 4.0 &&
       (outside > -2 || outside < -5))
      {
      bitshift = FUDGEFACTOR;
      }
    }

  fudge = 1L << bitshift;

  l_at_rad = fudge/32768L;

  f_at_rad = 1.0/32768L;

  /* now setup arrays of real coordinates corresponding to each pixel */

  /* make sure all corners in valid range */
  adjust_to_limits(1.0);

  /* calculate stepsizes */
  delxx  = (xxmax - xx3rd) / dx_size;
  delyy  = (yymax - yy3rd) / dy_size;
  delxx2 = (xx3rd - xxmin) / dy_size;
  delyy2 = (yy3rd - yymin) / dx_size;

  /* fudgetolong fails w >10 digits in double */
  if(fractype != CELLULAR)
    {
    /* integer equivs for it all */
    creal = fudgetolong(param[0]);
    cimag = fudgetolong(param[1]);
    xmin  = fudgetolong(xxmin);
    xmax  = fudgetolong(xxmax);
    x3rd  = fudgetolong(xx3rd);
    ymin  = fudgetolong(yymin);
    ymax  = fudgetolong(yymax);
    y3rd  = fudgetolong(yy3rd);
    delx  = fudgetolong(delxx);
    dely  = fudgetolong(delyy);
    delx2 = fudgetolong(delxx2);
    dely2 = fudgetolong(delyy2);
    }

  /* skip this if plasma to avoid 3d problems */
  if (fractype != PLASMA)
    {
    if (integerfractal && !invert)
      {
      if ((delx  == 0 && delxx  != 0.0) ||
          (delx2 == 0 && delxx2 != 0.0) ||
          (dely  == 0 && delyy  != 0.0) ||
          (dely2 == 0 && delyy2 != 0.0))
        {
        goto expand_retry;
        }
      /* fill up the x, y grids */
      lx0[0] = xmin;
      ly0[0] = ymax;
      lx1[0] = ly1[0] = 0;

      for (i = 1; i < xdots; i++ )
        {
        lx0[i] = lx0[i-1] + delx;
        ly1[i] = ly1[i-1] - dely2;
        }

      for (i = 1; i < ydots; i++ )
        {
        ly0[i] = ly0[i-1] - dely;
        lx1[i] = lx1[i-1] + delx2;
        }

      /* past max res?  check corners within 10% of expected */

      if (ratio_bad((double)lx0[xdots-1]-xmin,(double)xmax-x3rd)     ||
          ratio_bad((double)ly0[ydots-1]-ymax,(double)y3rd-ymax)     ||
          ratio_bad((double)lx1[(ydots>>1)-1],((double)x3rd-xmin)/2) ||
          ratio_bad((double)ly1[(xdots>>1)-1],((double)ymin-y3rd)/2))
        {
expand_retry:
        if (integerfractal && curfractalspecific->tofloat != NOFRACTAL)
          floatflag = '1';
        else
          adjust_to_limits(2.0);

        if (calc_status == IMG_INTR_RESM)
          {
          calc_status = IMG_PRMS_CHGD;
          update_draw_menu(NEW_IMAGE);
          }
        goto init_restart;
        }

      /* re-set corners to match reality */
      xmax = lx0[xdots-1] + lx1[ydots-1];
      ymin = ly0[ydots-1] + ly1[xdots-1];
      x3rd = xmin + lx1[ydots-1];
      y3rd = ly0[ydots-1];
      xxmin = fudgetodouble(xmin);
      xxmax = fudgetodouble(xmax);
      xx3rd = fudgetodouble(x3rd);
      yymin = fudgetodouble(ymin);
      yymax = fudgetodouble(ymax);
      yy3rd = fudgetodouble(y3rd);
      }
    else
      {
      /* set up dx0 and dy0 analogs of lx0 and ly0 */
      /* put fractal parameters in doubles */
      /* fill up the x, y grids */

      dx0[0] = xxmin;
      dy0[0] = yymax;
      dx1[0] = dy1[0] = 0;

      for (i = 1; i < xdots; i++ )
        {
        dx0[i] = dx0[i-1] + delxx;
        dy1[i] = dy1[i-1] - delyy2;
        }

      for (i = 1; i < ydots; i++ )
        {
        dy0[i] = dy0[i-1] - delyy;
        dx1[i] = dx1[i-1] + delxx2;
        }

      if (ratio_bad(dx0[xdots-1]-xxmin,xxmax-xx3rd) ||
          ratio_bad(dy0[ydots-1]-yymax,yy3rd-yymax) ||
          ratio_bad(dx1[ydots-1],xx3rd-xxmin)       ||
          ratio_bad(dy1[xdots-1],yymin-yy3rd))
        {
        goto expand_retry;
        }

      /* re-set corners to match reality */
      xxmax = dx0[xdots-1] + dx1[ydots-1];
      yymin = dy0[ydots-1] + dy1[xdots-1];
      xx3rd = xxmin + dx1[ydots-1];
      yy3rd = dy0[ydots-1];
      }
    }

  /* for periodicity close-enough, and for unity: */
  /*     min(max(delx,delx2),max(dely,dely2)      */

  ddelmin = fabs(delxx);

  if (fabs(delxx2) > ddelmin)
    {
    ddelmin = fabs(delxx2);
    }

  if (fabs(delyy) > fabs(delyy2))
    {
    if (fabs(delyy) < ddelmin)
      ddelmin = fabs(delyy);
    }
  else
    {
    if (fabs(delyy2) < ddelmin)
      ddelmin = fabs(delyy2);
    }

  delmin = fudgetolong(ddelmin);

  /* calculate factors which plot real values to screen co-ords */
  /* calcfrac.c plot_orbit routines have comments about this    */

  ftemp = (0.0-delyy2) * delxx2 * dx_size * dy_size -
          (xxmax-xx3rd) * (yy3rd-yymax);

  plotmx1 = delxx2 * dx_size * dy_size / ftemp;
  plotmx2 = (yy3rd-yymax) * dx_size / ftemp;
  plotmy1 = (0.0-delyy2) * dx_size * dy_size / ftemp;
  plotmy2 = (xxmax-xx3rd) * dy_size / ftemp;

}

static long fudgetolong(double d)
{
  if ((d *= fudge) > 0)
    d += 0.5;
  else
    d -= 0.5;
  return (long)d;
}

static double fudgetodouble(long l)
{
  char buf[30];
  double d;
  sprintf(buf,"%.9g",(double)l / fudge);
  sscanf(buf,"%lf",&d);
  return d;
}

static int ratio_bad(double actual, double desired)
{
  double ftemp;
  if (desired != 0)
    if ((ftemp = actual / desired) < 0.95 || ftemp > 1.05)
      return(1);
  return(0);
}

static void adjust_to_limits(double expand)
{
  double cornerx[4],cornery[4];
  double lowx,highx,lowy,highy,limit,ftemp;
  double centerx,centery,adjx,adjy;
  int i;
  limit = 32767.99;
  if (bitshift >= 24) limit = 31.99;
  if (bitshift >= 29) limit = 3.99;
  centerx = (xxmin+xxmax)/2;
  centery = (yymin+yymax)/2;

  /* infinitely thin?, fix it */

  if (xxmin == centerx)
    {
    smallest_add(&xxmax);
    xxmin -= xxmax-centerx;
    }

  if (yymin == centery)
    {
    smallest_add(&yymax);
    yymin -= yymax-centery;
    }

  if (xx3rd == centerx)
    smallest_add(&xx3rd);

  if (yy3rd == centery)
    smallest_add(&yy3rd);

  /* setup array for easier manipulation */

  cornerx[0] = xxmin; cornerx[1] = xxmax;
  cornerx[2] = xx3rd; cornerx[3] = xxmin+(xxmax-xx3rd);
  cornery[0] = yymax; cornery[1] = yymin;
  cornery[2] = yy3rd; cornery[3] = yymin+(yymax-yy3rd);

  /* if caller wants image size adjusted, do that first */

  if (expand != 1.0)
    {
    for (i=0; i<4; ++i)
      {
      cornerx[i] = centerx + (cornerx[i]-centerx)*expand;
      cornery[i] = centery + (cornery[i]-centery)*expand;
      }
    }

  /* get min/max x/y values */

  lowx = highx = cornerx[0];
  lowy = highy = cornery[0];

  for (i=1; i<4; ++i)
    {
    if (cornerx[i] < lowx)
      lowx  = cornerx[i];
    if (cornerx[i] > highx)
      highx = cornerx[i];
    if (cornery[i] < lowy)
      lowy  = cornery[i];
    if (cornery[i] > highy)
      highy = cornery[i];
    }

  /* if image is too large, downsize it maintaining center */

  ftemp = highx-lowx;

  if (highy-lowy > ftemp)
    ftemp = highy-lowy;

  if ((ftemp = limit*2/ftemp) < 1.0)
    {
    for (i=0; i<4; ++i)
      {
      cornerx[i] = centerx + (cornerx[i]-centerx)*ftemp;
      cornery[i] = centery + (cornery[i]-centery)*ftemp;
      }
    }

  /* if any corner has x or y past limit, move the image */

  adjx = adjy = 0;

  for (i=0; i<4; ++i)
    {
    if (cornerx[i] > limit     && (ftemp = cornerx[i] - limit) > adjx)
      adjx = ftemp;
    if (cornerx[i] < 0.0-limit && (ftemp = cornerx[i] + limit) < adjx)
      adjx = ftemp;
    if (cornery[i] > limit     && (ftemp = cornery[i] - limit) > adjy)
      adjy = ftemp;
    if (cornery[i] < 0.0-limit && (ftemp = cornery[i] + limit) < adjy)
      adjy = ftemp;
    }

  if (calc_status == IMG_INTR_RESM &&
     (adjx != 0 || adjy != 0) && (zwidth == 1.0))
    {
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    }

  xxmin = cornerx[0] - adjx;
  xxmax = cornerx[1] - adjx;
  xx3rd = cornerx[2] - adjx;
  yymax = cornery[0] - adjy;
  yymin = cornery[1] - adjy;
  yy3rd = cornery[2] - adjy;

  /* make 3rd corner exact if very near other co-ords */

  adjust_corner();

}

void adjust_corner()
{
  /*
   * make edges very near vert/horiz exact, to ditch rounding errs and
   * to avoid problems when delta per axis makes too large a ratio
   */

  double ftemp,ftemp2;

  if( (ftemp=fabs(xx3rd-xxmin)) < (ftemp2=fabs(xxmax-xx3rd)) )
    {
    if (ftemp*10000 < ftemp2 && yy3rd != yymax)
      xx3rd = xxmin;
    }
  else if (ftemp2*10000 < ftemp && yy3rd != yymin)
    xx3rd = xxmax;

  if( (ftemp=fabs(yy3rd-yymin)) < (ftemp2=fabs(yymax-yy3rd)) )
    {
    if (ftemp*10000 < ftemp2 && xx3rd != xxmax)
      yy3rd = yymin;
    }
  else if (ftemp2*10000 < ftemp && xx3rd != xxmin)
    yy3rd = yymax;
}

static void smallest_add(double *num)
{
  *num += *num * 5.0e-16;
}

int add_worklist(int xfrom, int xto, int yfrom, int yto, int ybegin,
int pass, int sym)
{
  if (num_worklist >= MAXCALCWORK)
    return(-1);

  worklist[num_worklist].xxstart = xfrom;
  worklist[num_worklist].xxstop  = xto;
  worklist[num_worklist].yystart = yfrom;
  worklist[num_worklist].yystop  = yto;
  worklist[num_worklist].yybegin = ybegin;
  worklist[num_worklist].pass    = pass;
  worklist[num_worklist].sym     = sym;

  ++num_worklist;

  tidy_worklist();

  return(0);
}

void tidy_worklist() /* combine mergeable entries, resort */
{
  int i,j;
  struct workliststuff tempwork;

  while ((i=combine_worklist()))
    {
    /* merged two, delete the gone one */
    while (++i < num_worklist)
      worklist[i-1] = worklist[i];
    --num_worklist;
    }

  for (i=0; i<num_worklist; ++i)
    {
    for (j=i+1; j<num_worklist; ++j)
      {
      if (worklist[j].pass < worklist[i].pass       ||
         (worklist[j].pass == worklist[i].pass      &&
         (worklist[j].yystart < worklist[i].yystart ||
         (worklist[j].yystart == worklist[i].yystart &&
         worklist[j].xxstart <  worklist[i].xxstart))))
        {
        /* dumb sort, swap 2 entries to correct order */
        tempwork = worklist[i];
        worklist[i] = worklist[j];
        worklist[j] = tempwork;
        }
      }
    }
}

/* look for 2 entries which can freely merge */
static int combine_worklist()
{
  int i,j;

  for (i=0; i<num_worklist; ++i)
    {
    if (worklist[i].yystart == worklist[i].yybegin)
      {
      for (j=i+1; j<num_worklist; ++j)
        {
        if (worklist[j].sym == worklist[i].sym
          && worklist[j].yystart == worklist[j].yybegin
          && worklist[i].pass == worklist[j].pass)
          {
          if ( worklist[i].xxstart == worklist[j].xxstart
            && worklist[i].xxstop  == worklist[j].xxstop)
            {
            if (worklist[i].yystop+1 == worklist[j].yystart)
              {
              worklist[i].yystop = worklist[j].yystop;
              return(j);
              }
            if (worklist[j].yystop+1 == worklist[i].yystart)
              {
              worklist[i].yystart = worklist[j].yystart;
              worklist[i].yybegin = worklist[j].yybegin;
              return(j);
              }
            }
          if ( worklist[i].yystart == worklist[j].yystart
            && worklist[i].yystop  == worklist[j].yystop)
            {
            if (worklist[i].xxstop+1 == worklist[j].xxstart)
              {
              worklist[i].xxstop = worklist[j].xxstop;
              return(j);
              }
            if (worklist[j].xxstop+1 == worklist[i].xxstart)
              {
              worklist[i].xxstart = worklist[j].xxstart;
              return(j);
              }
            }
          }
        }
      }
    }
  return(0); /* nothing combined */
}

#ifdef __STDC__
int put_resume(int len, ...)
#else
int put_resume(len, va_alist)
     int len;
     va_dcl
#endif
{
  va_list arg_marker;  /* variable arg list */
  char *source_ptr;

  if (resume_info == NULL)
    return(-1);

  VA_START(arg_marker,len);

  while (len)
    {
    source_ptr = va_arg(arg_marker,char *);
    memcpy(resume_info+resume_len,source_ptr,len);
    resume_len += len;
    len = va_arg(arg_marker,int);
    }
  va_end(arg_marker);
  return(0);
}

#ifdef __STDC__
int get_resume(int len, ...)
#else
int get_resume(len, va_alist)
     int len;
     va_dcl
#endif
{
  va_list arg_marker;  /* variable arg list */
  char *dest_ptr;

  if (resume_info == NULL)
    return(-1);

  VA_START(arg_marker,len);

  while (len)
   {
   dest_ptr = va_arg(arg_marker,char *);
   memcpy(dest_ptr,resume_info+resume_offset,len);
   resume_offset += len;
   len = va_arg(arg_marker,int);
   }
   va_end(arg_marker);
   return(0);
}

int alloc_resume(int alloclen, int version)
{
  char msg[256];

  if (resume_info != NULL) /* free the prior area if there is one */
    XtFree(resume_info);

  if ((resume_info = XtMalloc((long)alloclen))== NULL)
    {
    sprintf((char *) &msg, "%s",
"Warning - insufficient free memory to save status.\nYou will not be able to resume calculating this image.");
    warning(main_window, (char *) &msg, NULL);
    calc_status = IMG_INTR_NRES;
    update_draw_menu(NEW_IMAGE);
    return(-1);
    }
  resume_len = 0;
  put_resume(sizeof(int),&version,0);
  calc_status = IMG_INTR_RESM;
  update_draw_menu(CONT_CALC);
  return(0);
}

int start_resume()
{
  int version;
  if (resume_info == NULL)
    return(-1);
  resume_offset = 0;
  get_resume(sizeof(int),&version,0);
  return(version);
}

void end_resume()
{
  if (resume_info != NULL) /* free the prior area if there is one */
  {
  XtFree(resume_info);
  resume_info = NULL;
  }
}

int ssg_blocksize() /* used by solidguessing and by zoom panning */
{
  int blocksize,i;

  /* blocksize 4 if <300 rows, 8 if 300-599, 16 if 600-1199, 32 if >=1200 */

  blocksize=4;
  i=300;

  while(i<=ydots)
    {
    blocksize+=blocksize;
    i+=i;
    }

  /* increase blocksize if prefix array not big enough */
  while(blocksize*(maxxblk-2)<xdots || blocksize*(maxyblk-2)*16<ydots)
    blocksize+=blocksize;

  /* fix bug in prefix array calculation */
  if(blocksize < 32)
    blocksize+=blocksize;

  return(blocksize);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: calcfracinit.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.6  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
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
 * Revision 1.1.1.2  1995/03/14  15:32:18  tom
 * changed #if to #ifdef
 *
 * Revision 1.1.1.1  1995/03/11  03:04:33  tom
 * renamed dysize to dy_size and dxsixe to dx_size
 * changed varargs handling
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
 * Revision 0.101  1994/11/18  17:09:57  darryl
 * Aesthetic changes - widget and label placement. Minor bug
 * fixes when saving parameters and drawing a zoom box.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
