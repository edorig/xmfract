
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: miscfrac.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static unsigned short rand16(void);
static void putpot(int x, int y, unsigned short color);
static unsigned short getpot(int x, int y);
static unsigned short adjust(int xa, int ya, int x, int y, int xb, int yb);
static int new_subD(int x1, int y1, int x2, int y2, int recur);
static void subDivide(int x1, int y1, int x2, int y2);

/* static variables */
typedef void (*PLOT)(int,int,int);

static unsigned short (*getpix)(int,int) =
  (unsigned short(*)(int,int))getcolor;

static int iparmx;
static int pcolors;
static int recur1 = 1;
static int recur_level = 0;
static int shiftvalue;

static unsigned short rand16()
{
  unsigned short value;
  value = rand15();
  value <<= 1;
  value += rand15()&1;
  if(value < 1)
    value = 1;
  return(value);
}

static void putpot(int x, int y, unsigned short color)
{
  if(color < 1)
    color = 1;

  putcolor(x, y, color >> 8 ? color >> 8 : 1);  /* don't write 0 */

  writedisk(x+sxoffs,y+syoffs,color >> 8);    /* upper 8 bits */
  writedisk(x+sxoffs,y+sydots+syoffs,color&255); /* lower 8 bits */
}

static unsigned short getpot(int x, int y)
{
  unsigned short color;

  color = (unsigned short)readdisk(x+sxoffs,y+syoffs);
  color = (color << 8) + (unsigned short) readdisk(x+sxoffs,y+sydots+syoffs);
  return(color);
}

int plasma()
{
  int i,k, n;
  unsigned short rnd[4];
  KeySym key;

  int OldPotFlag = 0;
  int OldPot16bit = 0;

  plasma_check = 0;

  if(colors < 4)
    {
    char plasmamsg[128];
    sprintf((char *) &plasmamsg, "Plasma clouds can currently only be\nrun in a 4-or-more-color video mode");
    inform(main_window, (char *) &plasmamsg, NULL);
    return(-1);
    }

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  iparmx = param[0] * 8;
  if (parm.x <= 0.0) iparmx = 16;
  if (parm.x >= 100) iparmx = 800;

  if ((!rflag) && param[2] == 1)
    --rseed;

  if (param[2] != 0 && param[2] != 1)
    rseed = param[2];

  /* max_plasma is used as a flag for potential */
  max_plasma = (unsigned short)param[3];

  if(max_plasma != 0)
    {
    if (pot_startdisk() >= 0)
      {
      max_plasma = (unsigned short)(1L << 16) -1;
      plot    = (PLOT)putpot;
      getpix =  getpot;
      OldPotFlag = potflag;
      OldPot16bit = pot16bit;
      }
    else
      {
      max_plasma = 0;    /* can't do potential (startdisk failed) */
      param[3]   = 0;
      plot    = putcolor;
      getpix  = (unsigned short (*)(int,int))getcolor;
      }
    }
  else
    {
    plot    = putcolor;
    getpix  = (unsigned short (*)(int,int))getcolor;
    }

  srand(rseed);

  if (!rflag)
    ++rseed;

  if (colors > 16)
    shiftvalue = 18;
  else
    {
    if (colors > 4)
      shiftvalue = 22;
    else
      {
      if (colors > 2)
        shiftvalue = 24;
      else
        shiftvalue = 25;
      }
    }

  if(max_plasma != 0)
    shiftvalue = 10;

  if(max_plasma == 0)
    {
    pcolors = min(colors, max_colors);

    for(n = 0; n < 4; n++)
      rnd[n] = 1+(((rand15()/pcolors)*(pcolors-1))>>(shiftvalue-11));
    }
  else
    {
    for(n = 0; n < 4; n++)
      rnd[n] = rand16();
    }

  plot(0, 0, rnd[0]);
  plot(xdots - 1, 0, rnd[1]);
  plot(xdots-1, ydots-1, rnd[2]);
  plot(0, ydots - 1, rnd[3]);

  recur_level = 0;

  if (param[1] == 0)
    subDivide(0,0,xdots-1,ydots-1);
  else
    {
    recur1 = i = k = 1;
    while(new_subD(0,0,xdots-1,ydots-1,i)==0)
      {
      k = k * 2;

      if(k > (int)max(xdots-1,ydots-1))
        break;

      if((key = getakey() != 0))
        {
        n = 1;
        goto done;
        }

      i++;
      }
    }

  if ((key = getakey() != 0))
    n = 0;
  else
    n = 1;

done:

  if(max_plasma != 0)
    {
    potflag = OldPotFlag;
    pot16bit = OldPot16bit;
    }

  plot    = putcolor;
  getpix  = (unsigned short (*)(int,int))getcolor;
  XDefineCursor(display, window, XCreateFontCursor(display, XC_top_left_arrow));
  return(n);
}

static unsigned short adjust(int xa,int ya,int x,int y,int xb,int yb)
{
  long pseudorandom;
  pseudorandom = ((long)iparmx)*((rand15()-16383));
  pseudorandom = pseudorandom * recur1;
  pseudorandom = pseudorandom >> shiftvalue;
  pseudorandom = (((long)getpix(xa,ya)+(long)getpix(xb,yb)+1)>>1)+pseudorandom;
  if(max_plasma == 0)
    {
    if (pseudorandom >= pcolors)
      pseudorandom = pcolors-1;
    }
  else if (pseudorandom >= (long) max_plasma)
    pseudorandom = (long)max_plasma;

  if(pseudorandom < 1)
    pseudorandom = 1;

  plot(x,y,(unsigned short)pseudorandom);

  return((unsigned short)pseudorandom);
}

static int new_subD (int x1,int y1,int x2,int y2, int recur)
{
  int x,y;
  int nx1;
  int nx;
  int ny1, ny;
  long i, v;
  KeySym key;

  struct sub {
     unsigned char t; /* top of stack */
     int v[16]; /* subdivided value */
     unsigned char r[16];  /* recursion level */
  };

  static struct sub subx, suby;

  recur1 = 320L >> recur;
  suby.t = 2;
  ny   = suby.v[0] = y2;
  ny1 = suby.v[2] = y1;
  suby.r[0] = suby.r[2] = 0;
  suby.r[1] = 1;
  y = suby.v[1] = (ny1 + ny) >> 1;

  while (suby.t >= 1)
    {
    if ((++plasma_check & 0x0f) == 1)
      {
      if((key = getakey() != 0))
        {
        plasma_check--;
        return(1);
        }
      }
    while (suby.r[suby.t-1] < (unsigned char) recur)
      {
      /*     1.  Create new entry at top of the stack  */
      /*     2.  Copy old top value to new top value.  */
      /*            This is largest y value.           */
      /*     3.  Smallest y is now old mid point       */
      /*     4.  Set new mid point recursion level     */
      /*     5.  New mid point value is average        */
      /*            of largest and smallest            */

      suby.t++;
      ny1  = suby.v[suby.t] = suby.v[suby.t-1];
      ny   = suby.v[suby.t-2];
      suby.r[suby.t] = suby.r[suby.t-1];
      y    = suby.v[suby.t-1]   = (ny1 + ny) >> 1;
      suby.r[suby.t-1]   = (int)max(suby.r[suby.t], suby.r[suby.t-2])+1;
      }
    subx.t = 2;
    nx  = subx.v[0] = x2;
    nx1 = subx.v[2] = x1;
    subx.r[0] = subx.r[2] = 0;
    subx.r[1] = 1;
    x = subx.v[1] = (nx1 + nx) >> 1;

    while (subx.t >= 1)
      {
      while (subx.r[subx.t-1] < (unsigned char) recur)
        {
        subx.t++; /* move the top ofthe stack up 1 */
        nx1  = subx.v[subx.t] = subx.v[subx.t-1];
        nx   = subx.v[subx.t-2];
        subx.r[subx.t] = subx.r[subx.t-1];
        x    = subx.v[subx.t-1]   = (nx1 + nx) >> 1;
        subx.r[subx.t-1]   = (int)max(subx.r[subx.t], subx.r[subx.t-2])+1;
        }

      if ((i = getpix(nx, y)) == 0)
        i = adjust(nx,ny1,nx,y ,nx,ny);

      v = i;

      if ((i = getpix(x, ny)) == 0)
        i = adjust(nx1,ny,x ,ny,nx,ny);

      v += i;

      if(getpix(x,y) == 0)
        {
        if ((i = getpix(x, ny1)) == 0)
          i = adjust(nx1,ny1,x ,ny1,nx,ny1);

        v += i;

        if ((i = getpix(nx1, y)) == 0)
          i = adjust(nx1,ny1,nx1,y ,nx1,ny);

        v += i;

        plot(x,y,(unsigned short)((v + 2) >> 2));
        }

      if (subx.r[subx.t-1] == recur)
        subx.t = subx.t - 2;
      }

    if (suby.r[suby.t-1] == recur)
      suby.t = suby.t - 2;
    }

  return(0);
}

static void subDivide(int x1,int y1,int x2,int y2)
{
  int x,y;
  long v,i;

  if ((++plasma_check & 0x7f) == 1)
    {
    if(check_key() != 0)
      {
      plasma_check--;
      return;
      }
    }

  if(x2 - x1 < 2 && y2 - y1 < 2)
    return;

  recur_level++;

  recur1 = 320L >> recur_level;

  x = (x1 + x2) >> 1;
  y = (y1 + y2) >> 1;

  if((v = getpix(x ,y1)) == 0)
    v = adjust(x1, y1, x, y1, x2, y1);

  i = v;

  if((v = getpix(x2, y)) == 0)
    v = adjust(x2, y1, x2, y, x2, y2);

  i += v;

  if((v = getpix(x, y2)) == 0)
    v = adjust(x1, y2, x, y2, x2, y2);

  i += v;

  if((v = getpix(x1, y)) == 0)
    v = adjust(x1, y1, x1, y, x1, y2);

  i += v;

  if(getpix(x, y) == 0)
    plot(x, y, (unsigned short)((i + 2) >> 2));

  subDivide(x1, y1, x, y);
  subDivide(x, y1, x2, y);
  subDivide(x, y, x2, y2);
  subDivide(x1, y, x, y2);

  recur_level--;
}

int popcorn()   /* subset of std engine */
{
  int start_row;
  start_row = 0;
  if (resuming)
    {
    start_resume();
    get_resume(sizeof(int),&start_row,0);
    end_resume();
    }
  kbdcount=max_kbdcount;
  plot = noplot;
  tempsqrx = ltempsqrx = 0; /* PB added this to cover weird BAILOUTs */
  for (row = start_row; row <= iystop; row++)
    {
    reset_periodicity = 1;
    for (col = 0; col <= ixstop; col++)
      {
      if (StandardFractal() == -1) /* interrupted */
        {
        alloc_resume(10,1);
        put_resume(sizeof(int),&row,0);
        return(-1);
        }
      reset_periodicity = 0;
      }
    }
  calc_status = IMG_COMPLETED;
  update_draw_menu(NEW_IMAGE);
  return(0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: miscfrac.c,v $
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
