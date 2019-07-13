
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: zoom.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 * zoom.c - routines for zoombox manipulation and for panning
 */

void drawbox()
{
  struct coords tl,bl,tr,br; /* dot addr of topleft, botleft, etc */
  double tmpx,tmpy,dx,dy,rotcos,rotsin,ftemp1,ftemp2;
  double fxwidth,fxskew,fydepth,fyskew,fxadj;

  if (zwidth==0) /* no box to draw */
    {
    if (boxcount!=0) /* remove the old box from display */
      {
      boxcount = 0;
      }
    reset_zoom_corners();
    return;
    }

  ftemp1 = PI*zrotate/72; /* convert to radians */
  rotcos = cos(ftemp1);   /* sin & cos of rotation */
  rotsin = sin(ftemp1);

  /* do some calcs just once here to reduce fp work a bit */
  fxwidth = sxmax-sx3rd;
  fxskew  = sx3rd-sxmin;
  fydepth = sy3rd-symax;
  fyskew  = symin-sy3rd;
  fxadj   = zwidth*zskew;

  /* calc co-ords of topleft & botright corners of box */
  tmpx = zwidth/-2+fxadj; /* from zoombox center as origin, on xdots scale */
  tmpy = zdepth*finalaspectratio/2;

  dx = (rotcos*tmpx - rotsin*tmpy) - tmpx; /* delta x to rotate topleft */
  dy = tmpy - (rotsin*tmpx + rotcos*tmpy); /* delta y to rotate topleft */

  /* calc co-ords of topleft */
  ftemp1 = zbx + dx + fxadj;
  ftemp2 = zby + dy/finalaspectratio;

  tl.x   = ftemp1*(dx_size+PIXELROUND); /* screen co-ords */
  tl.y   = ftemp2*(dy_size+PIXELROUND);

  xxmin  = sxmin + ftemp1*fxwidth + ftemp2*fxskew; /* real co-ords */
  yymax  = symax + ftemp2*fydepth + ftemp1*fyskew;

  /* calc co-ords of bottom right */
  ftemp1 = zbx + zwidth - dx - fxadj;
  ftemp2 = zby - dy/finalaspectratio + zdepth;

  br.x   = ftemp1*(dx_size+PIXELROUND);
  br.y   = ftemp2*(dy_size+PIXELROUND);

  xxmax  = sxmin + ftemp1*fxwidth + ftemp2*fxskew;
  yymin  = symax + ftemp2*fydepth + ftemp1*fyskew;

  /* do the same for botleft & topright */
  tmpx = zwidth/-2 - fxadj;
  tmpy = 0.0-tmpy;

  dx = (rotcos*tmpx - rotsin*tmpy) - tmpx;
  dy = tmpy - (rotsin*tmpx + rotcos*tmpy);

  ftemp1 = zbx + dx - fxadj;
  ftemp2 = zby + dy/finalaspectratio + zdepth;

  bl.x   = ftemp1*(dx_size+PIXELROUND);
  bl.y   = ftemp2*(dy_size+PIXELROUND);

  xx3rd  = sxmin + ftemp1*fxwidth + ftemp2*fxskew;
  yy3rd  = symax + ftemp2*fydepth + ftemp1*fyskew;

  ftemp1 = zbx + zwidth - dx + fxadj;
  ftemp2 = zby - dy/finalaspectratio;

  tr.x   = ftemp1*(dx_size+PIXELROUND);
  tr.y   = ftemp2*(dy_size+PIXELROUND);

  if (boxcount!=0) /* remove the old box from display */
    {
    boxcount = 0;
    }
}

void zoomout() /* for ctl-enter, calc corners for zooming out */
{
  /*
   * (xxmin,yymax), etc, are already set to zoombox corners;
   * (sxmin,symax), etc, are still the screen's corners;
   * use the same logic as plot_orbit stuff to first calculate current screen
   * corners relative to the zoombox, as if the zoombox were a square with
   * upper left (0,0) and width/depth 1; ie calc the current screen corners
   * as if plotting them from the zoombox;
   * then extend these co-ords from current real screen corners to get
   * new actual corners
   */

  double savxxmin,savyymax,ftemp;

  ftemp = (yymin-yy3rd)*(xx3rd-xxmin) - (xxmax-xx3rd)*(yy3rd-yymax);

  plotmx1 = (xx3rd-xxmin) / ftemp; /* reuse the plotxxx vars is safe */
  plotmx2 = (yy3rd-yymax) / ftemp;
  plotmy1 = (yymin-yy3rd) / ftemp;
  plotmy2 = (xxmax-xx3rd) / ftemp;

  savxxmin = xxmin; savyymax = yymax;

  zmo_calc(sxmin-savxxmin,symax-savyymax,&xxmin,&yymax); /* new xxmin/xxmax */
  zmo_calc(sxmax-savxxmin,symin-savyymax,&xxmax,&yymin);
  zmo_calc(sx3rd-savxxmin,sy3rd-savyymax,&xx3rd,&yy3rd);
}

void zmo_calc(double dx, double dy, double *newx, double *newy)
{
  /*
   * calc cur screen corner relative to zoombox, when zoombox co-ords
   * are taken as (0,0) topleft thru (1,1) bottom right
   */
  double tempx,tempy;

  tempx = dy * plotmx1 - dx * plotmx2;
  tempy = dx * plotmy1 - dy * plotmy2;

  /* calc new corner by extending from current screen corners */

  *newx = sxmin + tempx*(sxmax-sx3rd) + tempy*(sx3rd-sxmin);
  *newy = symax + tempy*(sy3rd-symax) + tempx*(symin-sy3rd);
}

void aspectratio_crop(float oldaspect,float newaspect)
{
  double ftemp,xmargin,ymargin;
  if (newaspect > oldaspect) /* new ratio is taller, crop x */
    {
    ftemp = (1.0 - oldaspect / newaspect) / 2;
    xmargin = (xxmax - xx3rd) * ftemp;
    ymargin = (yymin - yy3rd) * ftemp;
    xx3rd += xmargin;
    yy3rd += ymargin;
    }
   else                /* new ratio is wider, crop y */
    {
    ftemp = (1.0 - newaspect / oldaspect) / 2;
    xmargin = (xx3rd - xxmin) * ftemp;
    ymargin = (yy3rd - yymax) * ftemp;
    xx3rd -= xmargin;
    yy3rd -= ymargin;
    }

   xxmin += xmargin;
   yymax += ymargin;
   xxmax -= xmargin;
   yymin -= ymargin;
}

int check_pan() /* return 0 if can't, alignment requirement if can */
{
  int i,j;

  if (calc_status != IMG_INTR_RESM && calc_status != IMG_COMPLETED)
    {
    return(0); /* not resumable, not complete */
    }

  if ( curfractalspecific->calctype != StandardFractal
    && curfractalspecific->calctype != calcmand
    && curfractalspecific->calctype != calcmandfp
    && curfractalspecific->calctype != lyapunov)
    {
    return(0); /* not a worklist-driven type */
    }

  if (zwidth != 1.0 || zdepth != 1.0 || zskew != 0.0 || zrotate != 0.0)
    {
    return(0); /* not a full size unrotated unskewed zoombox */
    }

  /* can pan if we get this far */
  if (calc_status == IMG_COMPLETED)
    {
    return(1); /* image completed, align on any pixel */
    }

  if (potflag && pot16bit)
    {
    return(1); /* 1 pass forced so align on any pixel */
    }

  if (stdcalcmode == 'b')
    {
    return(1); /* btm, align on any pixel */
    }

  if (stdcalcmode == 't')
    {
    return(0); /* tesselate, can't do it */
    }

  if (stdcalcmode != 'g' || (curfractalspecific->flags&NOGUESS))
    {
    if (stdcalcmode == '2') /* align on even pixel for 2pass */
      {
      return(2);
      }
    return(1); /* assume 1pass */
    }

  /* solid guessing */
  start_resume();

  get_resume(sizeof(int),&num_worklist,sizeof(worklist),worklist,0);

  /* don't do end_resume! we're just looking */
  i = 9;
  for (j=0; j<num_worklist; ++j) /* find lowest pass in any pending window */
    {
    if (worklist[j].pass < i)
      {
      i = worklist[j].pass;
      }
    }

  j = ssg_blocksize(); /* worst-case alignment requirement */

  while (--i >= 0)
    {
    j = j>>1; /* reduce requirement */
    }

  return(j);
}

void move_row(int fromrow,int torow,int col)
/* move a row on the screen */
{
  int startcol,endcol,tocol;
  memset(dstack,0,xdots); /* use dstack as a temp for the row; clear it */
  if (fromrow >= 0 && fromrow < ydots)
    {
    tocol = startcol = 0;
    endcol = xdots-1;
    if (col < 0)
      {
      tocol -= col;
      endcol += col;
      }

    if (col > 0)
      {
      startcol += col;
      }
    get_line(fromrow,startcol,endcol,(unsigned char *)&dstack[tocol]);
    }
  put_line(torow,0,xdots-1,(unsigned char *)dstack);
}

int init_pan_or_recalc(int do_zoomout)
/* decide to recalc, or to chg worklist & pan */
{
  int i,j,row,col,y,alignmask,listfull;
  char msg[128];

  if (zwidth == 0.0)
    {
    return(0); /* no zoombox, leave calc_status as is */
    }

  /* got a zoombox */
  if ((alignmask=check_pan()-1) < 0)
    {
    calc_status = IMG_PRMS_CHGD; /* can't pan, trigger recalc */
    update_draw_menu(NEW_IMAGE);
    return(0);
    }

  if (zbx == 0.0 && zby == 0.0)
    /* box is full screen, leave calc_status as is */
    {
    zoom_cancel_cb(main_window, NULL, NULL);
    return(0);
    }

  col = zbx*(dx_size+PIXELROUND); /* calc dest col,row of topleft pixel */
  row = zby*(dy_size+PIXELROUND);

  if (do_zoomout)   /* invert row and col */
    {
    row = 0-row;
    col = 0-col;
    }

  if ((row&alignmask) != 0 || (col&alignmask) != 0)
    {
    /* not on useable pixel alignment, trigger recalc */
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    return(0);
    }

  /* pan */
  num_worklist = 0;
  if (calc_status == IMG_INTR_RESM)
    {
    start_resume();
    get_resume(sizeof(int),&num_worklist,sizeof(worklist),worklist,0);
    } /* don't do end_resume! we might still change our mind */

  /* adjust existing worklist entries */
  for (i=0; i<num_worklist; ++i)
    {
    worklist[i].yystart -= row;
    worklist[i].yystop  -= row;
    worklist[i].yybegin -= row;
    worklist[i].xxstart -= col;
    worklist[i].xxstop  -= col;
    }

  /* add worklist entries for the new edges */
  listfull = i = 0;
  j = ydots-1;

  if (row < 0)
    {
    listfull |= add_worklist(0,xdots-1,0,0-row-1,0,0,0);
    i = 0 - row;
    }

  if (row > 0)
    {
    listfull |= add_worklist(0,xdots-1,ydots-row,ydots-1,ydots-row,0,0);
    j = ydots - row - 1;
    }

  if (col < 0)
    listfull |= add_worklist(0,0-col-1,i,j,i,0,0);

  if (col > 0)
    listfull |= add_worklist(xdots-col,xdots-1,i,j,i,0,0);

  if (listfull != 0)
    {
    sprintf((char *) &msg, "%s",
"Tables full, can't pan current_image.\n\
Cancel resumes old image, continue pans and calculates a new one.");
    if((confirm(main_window, (char *) &msg,
      OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON)))
      {
      zwidth = 0; /* cancel the zoombox */
      drawbox();
      }
    else
      calc_status = IMG_PRMS_CHGD; /* trigger recalc */
      update_draw_menu(NEW_IMAGE);
    return(0);
    }

  /* now we're committed */
  calc_status = IMG_INTR_RESM;
  update_draw_menu(CONT_CALC);

  if (row > 0) /* move image up */
    {
    for (y=0; y<ydots; ++y)
      {
      move_row(y+row,y,col);
      }
    }
  else     /* move image down */
    {
    for (y=ydots; --y>=0;)
      {
      move_row(y+row,y,col);
      }
    }

  fix_worklist(); /* fixup any out of bounds worklist entries */

  alloc_resume(sizeof(worklist)+10,1); /* post the new worklist */

  put_resume(sizeof(int),&num_worklist,sizeof(worklist),worklist,0);

  return(0);
}

void restart_window(int wknum)
/* force a worklist entry to restart */
{
  int yfrom,yto,xfrom,xto;
  if ((yfrom = worklist[wknum].yystart) < 0)
    {
    yfrom = 0;
    }

  if ((xfrom = worklist[wknum].xxstart) < 0)
    {
    xfrom = 0;
    }

  if ((yto = worklist[wknum].yystop) >= ydots)
    {
    yto = ydots - 1;
    }

  if ((xto = worklist[wknum].xxstop) >= xdots)
    {
    xto = xdots - 1;
    }

  memset(dstack,0,xdots); /* use dstack as a temp for the row; clear it */

  while (yfrom <= yto)
    {
    put_line(yfrom++,xfrom,xto,(unsigned char *)dstack);
    }

  worklist[wknum].sym = worklist[wknum].pass = 0;
  worklist[wknum].yybegin = worklist[wknum].yystart;
}

void fix_worklist() /* fix out of bounds and symmetry related stuff */
{
  int i,j,k;
  struct workliststuff *wk;
  for (i=0; i<num_worklist; ++i)
    {
    wk = &worklist[i];
    if ( wk->yystart >= ydots || wk->yystop < 0 ||
         wk->xxstart >= xdots || wk->xxstop < 0)
      { /* offscreen, delete */
      for (j=i+1; j<num_worklist; ++j)
        {
        worklist[j-1] = worklist[j];
        }
      --num_worklist;
      --i;
      continue;
      }
    if (wk->yystart < 0) /* partly off top edge */
      {
      if ((wk->sym&1) == 0) /* no sym, easy */
        {
        wk->yystart = 0;
        }
      else
        { /* xaxis symmetry */
        if ((j = wk->yystop + wk->yystart) > 0
            && num_worklist < MAXCALCWORK)
          { /* split the sym part */
          worklist[num_worklist] = worklist[i];
          worklist[num_worklist].yystart = 0;
          worklist[num_worklist++].yystop = j;
          wk->yystart = j+1;
          }
        else
          wk->yystart = 0;
        restart_window(i); /* restart the no-longer sym part */
        }
      }

    if (wk->yystop >= ydots) /* partly off bottom edge */
      {
      j = ydots-1;
      if ((wk->sym&1) != 0)   /* uses xaxis symmetry */
        {
        if ((k = wk->yystart + (wk->yystop - j)) < j)
          {
          if (num_worklist >= MAXCALCWORK) /* no room to split */
            {
            restart_window(i);
            }
          else   /* split it */
            {
            worklist[num_worklist] = worklist[i];
            worklist[num_worklist].yystart = k;
            worklist[num_worklist++].yystop = j;
            j = k-1;
            }
          }
        wk->sym &= -1 - 1;
        }
      wk->yystop = j;
      }

    if (wk->xxstart < 0) /* partly off left edge */
      {
      if ((wk->sym&2) == 0) /* no sym, easy */
        {
        wk->xxstart = 0;
        }
      else
        { /* yaxis symmetry */
        if ((j = wk->xxstop + wk->xxstart) > 0
            && num_worklist < MAXCALCWORK)
          { /* split the sym part */
          worklist[num_worklist] = worklist[i];
          worklist[num_worklist].xxstart = 0;
          worklist[num_worklist++].xxstop = j;
          wk->xxstart = j+1;
          }
        else
          wk->xxstart = 0;
        restart_window(i); /* restart the no-longer sym part */
        }
      }

    if (wk->xxstop >= xdots)   /* partly off right edge */
      {
      j = xdots-1;
      if ((wk->sym&2) != 0)   /* uses xaxis symmetry */
        {
        if ((k = wk->xxstart + (wk->xxstop - j)) < j)
          {
          if (num_worklist >= MAXCALCWORK) /* no room to split */
            {
            restart_window(i);
            }
          else
            { /* split it */
            worklist[num_worklist] = worklist[i];
            worklist[num_worklist].xxstart = k;
            worklist[num_worklist++].xxstop = j;
            j = k-1;
            }
          }
        wk->sym &= -1 - 2;
        }
      wk->xxstop = j;
      }

    if (wk->yybegin < wk->yystart)
      {
      wk->yybegin = wk->yystart;
      }

    if (wk->yybegin > wk->yystop)
      {
      wk->yybegin = wk->yystop;
      }
    }
  tidy_worklist(); /* combine where possible, re-sort */
}

void reset_zoom_corners()
{
  xxmin = sxmin;
  xxmax = sxmax;
  xx3rd = sx3rd;
  yymax = symax;
  yymin = symin;
  yy3rd = sy3rd;
}

void clear_zoombox()
{
   zwidth = 0;
   reset_zoom_corners();
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: zoom.c,v $
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
 * Revision 1.1.1.1  1995/03/11  03:46:34  tom
 * changed dxsize and dysize
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
 * Revision 0.101  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
