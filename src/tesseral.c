
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: tesseral.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int tesschkcol(int x, int y1, int y2);
static int tesschkrow(int x1, int x2, int y);
static int tesscol(int x, int y1, int y2);
static int tessrow(int x1, int x2, int y);

int tesseral()
{
  register struct tess *tp;

  guessplot = (plot != putcolor && plot != symplot2);
  tp = (struct tess *)&dstack[0];
  tp->x1 = ixstart;                  /* set up initial box */
  tp->x2 = ixstop;
  tp->y1 = iystart;
  tp->y2 = iystop;

  if (workpass == 0)
    { /* not resuming */
    tp->top = tessrow(ixstart,ixstop,iystart);     /* Do top row */
    tp->bot = tessrow(ixstart,ixstop,iystop);      /* Do bottom row */
    tp->lft = tesscol(ixstart,iystart+1,iystop-1); /* Do left column */
    tp->rgt = tesscol(ixstop,iystart+1,iystop-1);  /* Do right column */
    if (check_key() != 0)
      { /* interrupt before we got properly rolling */
      add_worklist(xxstart,xxstop,yystart,yystop,yystart,0,worksym);
      return(-1);
      }
    }
  else
    { /* resuming, rebuild work stack */
    int i,mid,curx,cury,xsize,ysize;
    struct tess *tp2;
    tp->top = tp->bot = tp->lft = tp->rgt = -2;
    cury = yybegin & 0xfff;
    ysize = 1;
    i = (unsigned)yybegin >> 12;
    while (--i >= 0)
      ysize <<= 1;
    curx = workpass & 0xfff;
    xsize = 1;
    i = (unsigned)workpass >> 12;
    while (--i >= 0)
      xsize <<= 1;
    while (1)
      {
      tp2 = tp;
      if (tp->x2 - tp->x1 > tp->y2 - tp->y1)
        { /* next divide down middle */
        if (tp->x1 == curx && (tp->x2 - tp->x1 - 2) < xsize)
          break;
        mid = (tp->x1 + tp->x2) >> 1;         /* Find mid point */
        if (mid > curx)
          { /* stack right part */
          memcpy(++tp,tp2,sizeof(*tp));
          tp->x2 = mid;
          }
        tp2->x1 = mid;
        }
      else
        { /* next divide across */
        if (tp->y1 == cury && (tp->y2 - tp->y1 - 2) < ysize)
          break;
        mid = (tp->y1 + tp->y2) >> 1;         /* Find mid point */
        if (mid > cury)
          { /* stack bottom part */
          memcpy(++tp,tp2,sizeof(*tp));
          tp->y2 = mid;
          }
        tp2->y1 = mid;
        }
      }
    }

  got_status = 4; /* for tab_display */

  while (tp >= (struct tess *)&dstack[0])
    { /* do next box */
    curcol = tp->x1; /* for tab_display */
    currow = tp->y1;

    if (tp->top == -1 || tp->bot == -1 || tp->lft == -1 || tp->rgt == -1)
      goto tess_split;
    /* for any edge whose color is unknown, set it */
    if (tp->top == -2)
      tp->top = tesschkrow(tp->x1,tp->x2,tp->y1);
    if (tp->top == -1)
      goto tess_split;
    if (tp->bot == -2)
      tp->bot = tesschkrow(tp->x1,tp->x2,tp->y2);
    if (tp->bot != tp->top)
      goto tess_split;
    if (tp->lft == -2)
      tp->lft = tesschkcol(tp->x1,tp->y1,tp->y2);
    if (tp->lft != tp->top)
      goto tess_split;
    if (tp->rgt == -2)
      tp->rgt = tesschkcol(tp->x2,tp->y1,tp->y2);
    if (tp->rgt != tp->top)
      goto tess_split;

      {
      int mid,midcolor;
      if (tp->x2 - tp->x1 > tp->y2 - tp->y1)
        { /* divide down the middle */
        mid = (tp->x1 + tp->x2) >> 1;         /* Find mid point */
        midcolor = tesscol(mid, tp->y1+1, tp->y2-1); /* Do mid column */
        if (midcolor != tp->top)
          goto tess_split;
        }
      else
        {                   /* divide across the middle */
        mid = (tp->y1 + tp->y2) >> 1;         /* Find mid point */
        midcolor = tessrow(tp->x1+1, tp->x2-1, mid); /* Do mid row */
        if (midcolor != tp->top)
          goto tess_split;
        }
      }

      {  /* all 4 edges are the same color, fill in */
      int i,j;
      i = 0;
      if(fillcolor != 0)
        {
        if(fillcolor > 0)
          tp->top = fillcolor & (colors-1);
        if (guessplot || (j = tp->x2 - tp->x1 - 1) < 2)
          { /* paint dots */
          for (col = tp->x1 + 1; col < tp->x2; col++)
            for (row = tp->y1 + 1; row < tp->y2; row++)
              {
              (*plot)(col,row,tp->top);
              if (++i > 500)
                {
                if (check_key() != 0)
                  goto tess_end;
                i = 0;
                }
              }
          }
        else
          { /* use put_line for speed */
          memset(&dstack[MAXPIXELS],tp->top,j);
          for (row = tp->y1 + 1; row < tp->y2; row++)
            {
            put_line(row,tp->x1+1,tp->x2-1,&dstack[MAXPIXELS]);
            if (plot != putcolor) /* symmetry */
              if ((j = yystop-(row-yystart)) > iystop && j < ydots)
                put_line(j,tp->x1+1,tp->x2-1,&dstack[MAXPIXELS]);
              if (++i > 25)
                {
                if (check_key() != 0)
                  goto tess_end;
                i = 0;
                }
            }
          }
        }
      --tp;
      }

      continue;

tess_split:

      {  /* box not surrounded by same color, sub-divide */
      int mid,midcolor;
      struct tess *tp2;
      if (tp->x2 - tp->x1 > tp->y2 - tp->y1)
        { /* divide down the middle */
        mid = (tp->x1 + tp->x2) >> 1;         /* Find mid point */
        midcolor = tesscol(mid, tp->y1+1, tp->y2-1); /* Do mid column */
        if (midcolor == -3)
          goto tess_end;
        if (tp->x2 - mid > 1)
          { /* right part >= 1 column */
          if (tp->top == -1)
            tp->top = -2;
          if (tp->bot == -1)
            tp->bot = -2;
          tp2 = tp;
          if (mid - tp->x1 > 1)
            { /* left part >= 1 col, stack right */
            memcpy(++tp,tp2,sizeof(*tp));
            tp->x2 = mid;
            tp->rgt = midcolor;
            }
          tp2->x1 = mid;
          tp2->lft = midcolor;
          }
        else
          --tp;
        }
      else
        { /* divide across the middle */
        mid = (tp->y1 + tp->y2) >> 1;         /* Find mid point */
        midcolor = tessrow(tp->x1+1, tp->x2-1, mid); /* Do mid row */
        if (midcolor == -3)
          goto tess_end;
        if (tp->y2 - mid > 1)
          {    /* bottom part >= 1 column */
          if (tp->lft == -1)
            tp->lft = -2;
          if (tp->rgt == -1)
            tp->rgt = -2;
          tp2 = tp;
          if (mid - tp->y1 > 1)
            { /* top also >= 1 col, stack bottom */
            memcpy(++tp,tp2,sizeof(*tp));
            tp->y2 = mid;
            tp->bot = midcolor;
            }
          tp2->y1 = mid;
          tp2->top = midcolor;
          }
        else
          --tp;
        }
      }
    }

tess_end:

  if (tp >= (struct tess *)&dstack[0])
    { /* didn't complete */
    int i,xsize,ysize;
    xsize = ysize = 1;
    i = 2;
    while (tp->x2 - tp->x1 - 2 >= i)
      {
      i <<= 1;
      ++xsize;
      }
    i = 2;
    while (tp->y2 - tp->y1 - 2 >= i)
      {
      i <<= 1;
      ++ysize;
      }
    add_worklist(xxstart,xxstop,yystart,yystop,
      (ysize<<12)+tp->y1,(xsize<<12)+tp->x1,worksym);
    return(-1);
    }
  return(0);

} /* tesseral */

static int tesschkcol(int x,int y1,int y2)
{
  int i;
  i = getcolor(x,++y1);
  while (--y2 > y1)
    if (getcolor(x,y2) != i) return -1;
  return i;
}

static int tesschkrow(int x1,int x2,int y)
{
  int i;
  i = getcolor(x1,y);
  while (x2 > x1)
    {
    if (getcolor(x2,y) != i) return -1;
    --x2;
    }
  return i;
}

static int tesscol(int x,int y1,int y2)
{
  int colcolor,i;
  col = x;
  row = y1;
  reset_periodicity = 1;
  colcolor = (*calctype)();
  reset_periodicity = 0;
  while (++row <= y2)
    { /* generate the column */
    if ((i = (*calctype)()) < 0)
      return -3;
    if (i != colcolor)
      colcolor = -1;
    }
  return colcolor;
}

static int tessrow(int x1,int x2,int y)
{
  int rowcolor,i;
  row = y;
  col = x1;
  reset_periodicity = 1;
  rowcolor = (*calctype)();
  reset_periodicity = 0;
  while (++col <= x2)
    { /* generate the row */
    if ((i = (*calctype)()) < 0)
      return -3;
    if (i != rowcolor)
      rowcolor = -1;
  }
  return rowcolor;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: tesseral.c,v $
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
