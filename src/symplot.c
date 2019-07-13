
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: symplot.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* Do nothing plot!!! */
/*ARGSUSED*/
void noplot(int x,int y,int color)
{
}

/* Symmetry plot for X Axis Symmetry - Striped Newtbasin version */

void symplot2basin(x, y, color)
int x, y, color;
{
  int i,stripe;

  putcolor(x, y, color);

  if(basin==2 && color > 8)
    stripe=8;
  else
    stripe = 0;

  if ((i=yystop-(y-yystart)) > iystop && i < ydots)
    {
    color -= stripe;            /* reconstruct unstriped color */
    color = (degree+1-color)%degree+1;  /* symmetrical color */
    color += stripe;            /* add stripe */
    putcolor(x, i,color);
    }
}

/* Symmetry plot for Both Axis Symmetry  - Newtbasin version */

void symplot4basin(x, y, color)
int x, y, color;
{
  int i,j,color1,stripe;

  if(color == 0) /* assumed to be "inside" color */
    {
    symplot4(x, y, color);
    return;
    }
  if(basin==2 && color > 8)
    stripe = 8;
  else
    stripe = 0;

  color -= stripe;       /* reconstruct unstriped color */
  color1 = degree/2+degree+2 - color;

  if(color < degree/2+2)
    color1 = degree/2+2 - color;
  else
    color1 = degree/2+degree+2 - color;

  j = xxstop-(x-xxstart);
  putcolor(x, y, color+stripe);

  if (j < xdots)
    putcolor(    j, y, color1+stripe);

  if ((i=yystop-(y-yystart)) > iystop && i < ydots)
    {
    putcolor(x, i, stripe + (degree+1 - color)%degree+1);
    if (j < xdots)
      putcolor(j, i, stripe + (degree+1 - color1)%degree+1);
    }
}

/* calculate round screen edges to avoid wasted colours in logmap */
int autologmap(void)
{
  int mincolour,lag;

  mincolour=32767;
  row=iystart;
  reset_periodicity = 0;

  for (col=ixstart;col<ixstop;col++) /* top row */
    {
    color=(*calctype)();

    if (realcolor < mincolour)
      mincolour=realcolor;

    if ( col >=32 )
      (*plot)(col-32,row,0);
    }

  /* these lines tidy up for BTM etc */

  for (lag=32;lag>0;lag--)
    (*plot)(col-lag,row,0);

  col=ixstop;

  for (row=iystart;row<iystop;row++) /* right  side */
    {
    color=(*calctype)();
    if (realcolor < mincolour)
      mincolour=realcolor;
    if ( row >=32 )
      (*plot)(col,row-32,0);
    }

  for (lag=32;lag>0;lag--)
    (*plot)(col,row-lag,0);

  col=ixstart;

  for (row=iystart;row<iystop;row++) /* left  side */
    {
    color=(*calctype)();
    if (realcolor < mincolour)
      mincolour=realcolor;
    if ( row >=32 )
      (*plot)(col,row-32,0);
    }

  for (lag=32;lag>0;lag--)
    (*plot)(col,row-lag,0);

  row=iystop;

  for (col=ixstart;col<ixstop;col++) /* bottom row */
    {
    color=(*calctype)();
    if (realcolor < mincolour)
      mincolour=realcolor;
    if ( col >=32 )
      (*plot)(col-32,row,0);
    }

  for (lag=32;lag>0;lag--)
    (*plot)(col-lag,row,0);

  /* insure autologmap not called again */

  if (mincolour==2)
    resuming=1;

  return mincolour;
}

/* Symmetry plot for period PI */

void symPIplot(x, y, color)
int x, y, color;
{
  while(x <= xxstop)
    {
    putcolor(x, y, color);
    x += pixelpi;
    }
}

/* Symmetry plot for period PI plus Origin Symmetry */

void symPIplot2J(x, y, color)
int x, y, color;
{
  int i,j;

  while(x <= xxstop)
    {
    putcolor(x, y, color);
    if ((i=yystop-(y-yystart)) > iystop && i < ydots &&
        (j=xxstop-(x-xxstart)) < xdots)
      {
      putcolor(j, i, color);
      }
    x += pixelpi;
    }
}

/* Symmetry plot for period PI plus Both Axis Symmetry */

void symPIplot4J(x, y, color)
int x, y, color;
{
  int i,j;
  while(x <= (xxstart+xxstop)/2)
    {
    j = xxstop-(x-xxstart);
    putcolor(x, y, color);
    if (j < xdots)
      putcolor(j , y , color);
    if ((i=yystop-(y-yystart)) > iystop && i < ydots)
      {
      putcolor(x , i , color);
      if (j < xdots)
        putcolor(j , i , color);
      }
    x += pixelpi;
    }
}

/* Symmetry plot for X Axis Symmetry */

void symplot2(x, y, color)
int x, y, color;
{
  int i;
  putcolor(x, y, color);
  if ((i=yystop-(y-yystart)) > iystop && i < ydots)
    putcolor(x, i, color);
}

/* Symmetry plot for Y Axis Symmetry */

void symplot2Y(x, y, color)
int x, y, color;
{
  int i;
  putcolor(x, y, color);
  if ((i=xxstop-(x-xxstart)) < xdots)
    putcolor(i, y, color);
}

/* Symmetry plot for Origin Symmetry */
void symplot2J(x, y, color)
int x, y, color;
{
  int i,j;
  putcolor(x, y, color);
  if ((i=yystop-(y-yystart)) > iystop && i < ydots &&
      (j=xxstop-(x-xxstart)) < xdots)
    {
    putcolor(j, i, color);
    }
}

/* Symmetry plot for Both Axis Symmetry */

void symplot4(x, y, color)
int x, y, color;
{
  int i,j;
  j = xxstop-(x-xxstart);
  putcolor(x, y, color);
  if (j < xdots)
    putcolor(    j , y, color);

  if ((i=yystop-(y-yystart)) > iystop && i < ydots)
    {
    putcolor(    x , i, color);
    if (j < xdots)
      putcolor(j , i, color);
    }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: symplot.c,v $
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
