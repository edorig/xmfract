
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: solidguess.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int guessrow(int firstpass,int y,int blocksize);
static void plotblock(int buildrow, int x, int y, int color);

int solidguess()
{
  int i,x,y,xlim,ylim,blocksize;
  unsigned int *pfxp0,*pfxp1;
  unsigned int u;

  guessplot=(plot!=putcolor && plot!=symplot2 && plot!=symplot2J);

  /* check if guessing at bottom & right edges is ok */

  bottom_guess = (plot == symplot2 || (plot == putcolor && iystop+1 == ydots));

  right_guess = (plot == symplot2J || ((plot == putcolor ||
                 plot == symplot2) && ixstop+1 == xdots));

  i = maxblock = blocksize = ssg_blocksize();

  totpasses = 1;

  while ((i >>= 1) > 1) ++totpasses;

  /* ensure window top and left are on required boundary, treat window
   * as larger than it really is if necessary (this is the reason symplot
   * routines must check for > xdots/ydots before plotting sym points)
   */

  ixstart &= -1 - (maxblock-1);
  iystart = yybegin;
  iystart &= -1 - (maxblock-1);

  got_status = 1;

  if (workpass == 0) /* otherwise first pass already done */
    {
    /* first pass, calc every blocksize**2 pixel, quarter result & paint it */
    curpass = 1;
    if (iystart <= yystart) /* first time for this window, init it */
      {
      currow = 0;
      memset(&prefix[1][0][0],0,maxxblk*maxyblk*2); /* noskip flags off */
      reset_periodicity = 1;
      row=iystart;
      for(col=ixstart; col<=ixstop; col+=maxblock)
        { /* calc top row */
        if((*calctype)()== -1)
          {
          add_worklist(xxstart,xxstop,yystart,yystop,yybegin,0,worksym);
          goto exit_solidguess;
          }
        reset_periodicity = 0;
        }
      }
    else
      memset(&prefix[1][0][0],-1,maxxblk*maxyblk*2); /* noskip flags on */

    for(y=iystart; y<=iystop; y+=blocksize)
      {
      currow = y;
      i = 0;
      if(y+blocksize<=iystop)
        { /* calc the row below */
        row=y+blocksize;
        reset_periodicity = 1;
        for(col=ixstart; col<=ixstop; col+=maxblock)
          {
          if((i=(*calctype)()) == -1)
            break;
          reset_periodicity = 0;
          }
        }
      reset_periodicity = 1;
      if (i == -1 || guessrow(1,y,blocksize) != 0) /* interrupted? */
        {
        if (y < yystart)
          y = yystart;
        add_worklist(xxstart,xxstop,yystart,yystop,y,0,worksym);
        goto exit_solidguess;
        }
      }

    if (num_worklist) /* work list not empty, just do 1st pass */
      {
      add_worklist(xxstart,xxstop,yystart,yystop,yystart,1,worksym);
      goto exit_solidguess;
      }

    ++workpass;

    iystart = yystart & (-1 - (maxblock-1));

    /* calculate skip flags for skippable blocks */
    xlim=(ixstop+maxblock)/maxblock+1;
    ylim=((iystop+maxblock)/maxblock+15)/16+1;

    if(right_guess==0) /* no right edge guessing, zap border */
      for(y=0;y<=ylim;++y)
        prefix[1][y][xlim]= -1;

    if(bottom_guess==0) /* no bottom edge guessing, zap border */
      {
      i=(iystop+maxblock)/maxblock+1;
      y=i/16+1;
      i=1<<(i&15);
      for(x=0;x<=xlim;++x)
        prefix[1][y][x]|=i;
      }

    /* set each bit in prefix[0] to OR of it & surrounding 8 in prefix[1] */
    for(y=0;++y<ylim;)
      {
      pfxp0= &prefix[0][y][0];
      pfxp1= &prefix[1][y][0];
      for(x=0;++x<xlim;)
        {
        ++pfxp1;
        u= *(pfxp1-1)|*pfxp1|*(pfxp1+1);
        *(++pfxp0)=u|(u>>1)|(u<<1)
          |((*(pfxp1-(maxxblk+1))|*(pfxp1-maxxblk)|*(pfxp1-(maxxblk-1)))>>15)
          |((*(pfxp1+(maxxblk-1))|*(pfxp1+maxxblk)|*(pfxp1+(maxxblk+1)))<<15);
        }
      }
    }
  else /* first pass already done */
    memset(&prefix[0][0][0],-1,maxxblk*maxyblk*2); /* noskip flags on */

  /* remaining pass(es), halve blocksize & quarter each blocksize**2 */
  i = workpass;

  while (--i > 0) /* allow for already done passes */
    blocksize = blocksize>>1;

  reset_periodicity = 1;

  while((blocksize=blocksize>>1)>=2)
    {
    curpass = workpass + 1;
    for(y=iystart; y<=iystop; y+=blocksize)
      {
      currow = y;
      if(guessrow(0,y,blocksize)!=0)
        {
        if (y < yystart)
          y = yystart;

        add_worklist(xxstart,xxstop,yystart,yystop,y,workpass,worksym);
        goto exit_solidguess;
        }
      }

    ++workpass;
    if (num_worklist /* work list not empty, do one pass at a time */
      && blocksize>2) /* if 2, we just did last pass */
      {
      add_worklist(xxstart,xxstop,yystart,yystop,yystart,workpass,worksym);
      goto exit_solidguess;
      }
    iystart = yystart & (-1 - (maxblock-1));
    }

  exit_solidguess:
  return(0);
}

static int guessrow(int firstpass,int y,int blocksize)
{
  int x;
  int i;
  int j;
  int color;
  int xplushalf;
  int xplusblock;
  int ylessblock;
  int ylesshalf;
  int yplushalf;
  int yplusblock;
  /* cxy is the color of pixel at (x,y) */
  /* where c22 is the topleft corner of */
  /* the block being handled in current */
  /* iteration                          */
  int c21;
  int c31;
  int c41 = 0;
  int c12;
  int c22;
  int c32;
  int c42 = 0;
  int c13;
  int c23;
  int c33;
  int c24;
  int c44 = 0;
  int guessed23;
  int guessed32;
  int guessed33;
  int guessed12;
  int guessed13;
  int prev11;
  int fix21;
  int fix31;
  unsigned int *pfxptr;
  unsigned int pfxmask;
  KeySym key = 0;

  halfblock=blocksize>>1;
  i=y/maxblock;
  pfxptr= &prefix[firstpass][(i>>4)+1][ixstart/maxblock];
  pfxmask=1<<(i&15);
  ylesshalf=y-halfblock;
  ylessblock=y-blocksize; /* constants, for speed */
  yplushalf=y+halfblock;
  yplusblock=y+blocksize;
  prev11= -1;
  c24=c12=c13=c22=getcolor(ixstart,y);
  c31=c21=getcolor(ixstart,(y>0)?ylesshalf:0);

  if(yplusblock<=iystop)
    c24=getcolor(ixstart,yplusblock);
  else if(bottom_guess==0)
    c24= -1;

  guessed12=guessed13=0;

  for(x=ixstart; x<=ixstop;)  /* increment at end, or when doing continue */
    {
    if((x&(maxblock-1))==0)  /* time for skip flag stuff */
      {
      ++pfxptr;
      if(firstpass==0 && (*pfxptr&pfxmask)==0)  /* check for fast skip */
        {
        /* next useful in testing to make skips visible
         *
         * if(halfblock==1)
         *   {
         *   (*plot)(x+1,y,0); (*plot)(x,y+1,0); (*plot)(x+1,y+1,0);
         *   }
         */
        x+=maxblock;
        prev11=c31=c21=c24=c12=c13=c22;
        guessed12=guessed13=0;
        continue;
        }
      }

    if(firstpass)  /* 1st pass, paint topleft corner */
      plotblock(0,x,y,c22);

    /* setup variables */
    xplusblock=(xplushalf=x+halfblock)+halfblock;
    if(xplushalf>ixstop)
      {
      if(right_guess==0)
        c31= -1;
      }
    else if(y>0)
      c31=getcolor(xplushalf,ylesshalf);

    if(xplusblock<=ixstop)
      {
      if(yplusblock<=iystop)
        c44=getcolor(xplusblock,yplusblock);
      c41=getcolor(xplusblock,(y>0)?ylesshalf:0);
      c42=getcolor(xplusblock,y);
      }
    else if(right_guess==0)
      c41=c42=c44= -1;

    if(yplusblock>iystop)
      c44=(bottom_guess)?c42:-1;

    /* guess or calc the remaining 3 quarters of current block */
    guessed23=guessed32=guessed33=1;

    c23=c32=c33=c22;

    if(yplushalf>iystop)
      {
      if(bottom_guess==0)
        c23=c33= -1;
      guessed23=guessed33= -1;
      }

    if(xplushalf>ixstop)
      {
      if(right_guess==0)
        c32=c33= -1;
      guessed32=guessed33= -1;
      }

    while(1) /* go around till none of 23,32,33 change anymore */
      {
      if(guessed33>0
         && (c33!=c44 || c33!=c42 || c33!=c24 || c33!=c32 || c33!=c23))
        {
        calcadot(c33,xplushalf,yplushalf);
        guessed33=0;
        }

      if(guessed32>0
          && (c32!=c33 || c32!=c42 || c32!=c31 || c32!=c21
          || c32!=c41 || c32!=c23))
        {
        calcadot(c32,xplushalf,y);
        guessed32=0;
        continue;
        }

      if(guessed23>0
         && (c23!=c33 || c23!=c24 || c23!=c13 || c23!=c12 || c23!=c32))
        {
        calcadot(c23,x,yplushalf);
        guessed23=0;
        continue;
        }
      break;
      }

    if(firstpass) /* note whether any of block's contents were calculated */
      if(guessed23==0 || guessed32==0 || guessed33==0)
       *pfxptr|=pfxmask;

    if(halfblock>1) /* not last pass, check if something to display */
      if(firstpass)    /* display guessed corners, fill in block */
        {
        if(guessplot)
          {
          if(guessed23>0)
            (*plot)(x,yplushalf,c23);
          if(guessed32>0)
            (*plot)(xplushalf,y,c32);
          if(guessed33>0)
            (*plot)(xplushalf,yplushalf,c33);
          }
        plotblock(1,x,yplushalf,c23);
        plotblock(0,xplushalf,y,c32);
        plotblock(1,xplushalf,yplushalf,c33);
        }
      else  /* repaint changed blocks */
        {
       if(c23!=c22)
          plotblock(-1,x,yplushalf,c23);
       if(c32!=c22)
          plotblock(-1,xplushalf,y,c32);
       if(c33!=c22)
          plotblock(-1,xplushalf,yplushalf,c33);
         }

    /* check if some calcs in this block mean earlier guesses need fixing */
    fix21=((c22!=c12 || c22!=c32)
       && c21==c22 && c21==c31 && c21==prev11
       && y>0
       && (x==ixstart || c21==getcolor(x-halfblock,ylessblock))
       && (xplushalf>ixstop || c21==getcolor(xplushalf,ylessblock))
       && c21==getcolor(x,ylessblock));

    fix31=(c22!=c32
       && c31==c22 && c31==c42 && c31==c21 && c31==c41
       && y>0 && xplushalf<=ixstop
       && c31==getcolor(xplushalf,ylessblock)
       && (xplusblock>ixstop || c31==getcolor(xplusblock,ylessblock))
       && c31==getcolor(x,ylessblock));

    prev11=c31; /* for next time around */

    if(fix21)
      {
      calcadot(c21,x,ylesshalf);
      if(halfblock>1 && c21!=c22)
        plotblock(-1,x,ylesshalf,c21);
      }

    if(fix31)
      {
      calcadot(c31,xplushalf,ylesshalf);
      if(halfblock>1 && c31!=c22)
        plotblock(-1,xplushalf,ylesshalf,c31);
      }

    if(c23!=c22)
      {
      if(guessed12)
        {
        calcadot(c12,x-halfblock,y);
        if(halfblock>1 && c12!=c22)
          plotblock(-1,x-halfblock,y,c12);
        }

      if(guessed13)
        {
        calcadot(c13,x-halfblock,yplushalf);
        if(halfblock>1 && c13!=c22)
          plotblock(-1,x-halfblock,yplushalf,c13);
        }
      }

    c22=c42;
    c24=c44;
    c13=c33;
    c31=c21=c41;
    c12=c32;
    guessed12=guessed32;
    guessed13=guessed33;
    x+=blocksize;
    } /* end x loop */

  if(firstpass==0 || guessplot) return 0;

  /* paint rows the fast way */
  for(i=0;i<halfblock;++i)
    {
    if((j=y+i)<=iystop)
      put_line(j,xxstart,ixstop,&dstack[xxstart]);
    if((j=y+i+halfblock)<=iystop)
      put_line(j,xxstart,ixstop,&dstack[xxstart+MAXPIXELS]);
    if((key = waitkeypressed(0) != 0))
      {
      if(key == 0x1 || key == XK_d ||
         key == SPACE || key == SPACE2 || jiiming)
        getakey();
      else
        return -1;
      }
    }

  if(plot!=putcolor)  /* symmetry, just vertical & origin the fast way */
    {
    if(plot==symplot2J) /* origin sym, reverse lines */
      for(i=(ixstop+xxstart+1)/2;--i>=xxstart;)
        {
        color=dstack[i];
        dstack[i]=dstack[j=ixstop-(i-xxstart)];
        dstack[j]=color;
        j+=MAXPIXELS;
        color=dstack[i+MAXPIXELS];
        dstack[i+MAXPIXELS]=dstack[j];
        dstack[j]=color;
        }
    for(i=0;i<halfblock;++i)
      {
      if((j=yystop-(y+i-yystart))>iystop && j<ydots)
        put_line(j,xxstart,ixstop,&dstack[xxstart]);
      if((j=yystop-(y+i+halfblock-yystart))>iystop && j<ydots)
        put_line(j,xxstart,ixstop,&dstack[xxstart+MAXPIXELS]);
      if((key = waitkeypressed(0) !=0))
        {
        if(key == 0x1 || key == XK_d || key == SPACE ||
           key == SPACE2 || jiiming)
          getakey();
        else
          return -1;
        }
      }
    }
  return 0;
}

static void plotblock(int buildrow,int x,int y,int color)
{
   int i,xlim,ylim;

   if((xlim=x+halfblock)>ixstop)
     xlim=ixstop+1;

   if(buildrow>=0 && guessplot==0) /* save it for later put_line */
     {
     if(buildrow==0)
       for(i=x;i<xlim;++i)
         dstack[i]=color;
     else
       for(i=x;i<xlim;++i)
         dstack[i+MAXPIXELS]=color;
     if (x>=xxstart) /* when x reduced for alignment, paint those dots too */
       return; /* the usual case */
     }

   /* paint it */
   if((ylim=y+halfblock)>iystop)
     {
     if(y>iystop)
       return;
     ylim=iystop+1;
     }

   for(i=x;++i<xlim;)
     (*plot)(i,y,color); /* skip 1st dot on 1st row */

   while(++y<ylim)
     for(i=x;i<xlim;++i)
       (*plot)(i,y,color);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: solidguess.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.6  1995/05/03  21:29:57  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.5  1995/03/21  17:26:03  darryl
 * Discard a key event if it is 0x1.
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
 * Revision 0.101  1994/11/19  02:53:05  darryl
 * Move plotblock() to solidguess.c and declare it static.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
