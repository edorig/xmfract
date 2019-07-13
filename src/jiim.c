
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: jiim.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void SetAspect(double aspect);
static void c_putcolor(int x, int y, int color);
static int c_getcolor(int x, int y);
static void circleplot(int x, int y, int color);
static void plot8(int x, int y, int color);
static void fillrect(int x, int y, int width, int depth, int color);
static void ClearQueue(void);
static Boolean MemoryAlloc(long size);
static int do_fractal_routines(double cr, double ci, int (*func )());
static void Cursor_Hide(void);
static void Cursor_Show(void);
static void mem_init(void *block, unsigned size);
static void getrow(int x, int y, int width, char *buff);
static void putrow(int x, int y, int width, char *buff);
static void clip_put_line(int row, int start, int stop, unsigned char *pixels);
static void clip_get_line(int row, int start, int stop, unsigned char *pixels);

/* static variables */
static FILE *file;
static int stored_at;
static int show_numbers =0; /* toggle for display of coords */
static char *memory = NULL; /* pointer to saved rectangle */
static int windows = 0;     /* windows management system */
static int xc, yc;          /* corners of the window */
static int xd, yd;          /* dots in the window    */
static double xcjul = BIG;
static double ycjul = BIG;
static int lookatmouse;
static unsigned mem_avail;
static char *mem_block;
static int OKtoMIIM;
static int SecretExperimentalMode;
static float luckyx = 0;
static float luckyy = 0;
static int cursx;
static int cursy;
static oldrootx = 0;
static oldrooty = 0;
static int oldmyx = 0;
static int oldmyy = 0;
static int warned = 0;

static void SetAspect(double aspect)
{
  xAspect = 0;
  yAspect = 0;
  aspect = fabs(aspect);
  if (aspect != 1.0)
    if (aspect > 1.0)
      yAspect = 65536.0 / aspect;
    else
      xAspect = 65536.0 * aspect;
}

static void c_putcolor(int x, int y, int color)
{
  /* avoid writing outside window */
  if ( x < xc || y < yc || x >= xc + xd || y >= yc + yd )
    return;
  if(y >= sydots - show_numbers) /* avoid overwriting coords */
    return;
  if(windows == 2) /* avoid overwriting fractal */
    if (0 <= x && x < xdots && 0 <= y && y < ydots)
      return;
  putcolor(x, y, color);
}

static int c_getcolor(int x, int y)
{
  /* avoid reading outside window */
  if ( x < xc || y < yc || x >= xc + xd || y >= yc + yd )
    return 1000;
  if(y >= sydots - show_numbers) /* avoid overreading coords */
    return 1000;
  if(windows == 2) /* avoid overreading fractal */
    if (0 <= x && x < xdots && 0 <= y && y < ydots)
      return 1000;
  return getcolor(x, y);
}

static void circleplot(int x, int y, int color)
{
  if (xAspect == 0)
    if (yAspect == 0)
      c_putcolor(x+xbase, y+ybase,color);
    else
      c_putcolor(x+xbase, (short)(ybase + (((long) y *
        (long) yAspect) >> 16)),color);
  else
    c_putcolor((int)(xbase + (((long) x *
      (long) xAspect) >> 16)), y+ybase, color);
}

static void plot8(int x, int y, int color)
{
  circleplot(x,y,color);
  circleplot(-x,y,color);
  circleplot(x,-y,color);
  circleplot(-x,-y,color);
  circleplot(y,x,color);
  circleplot(-y,x,color);
  circleplot(y,-x,color);
  circleplot(-y,-x,color);
}

void circle(int radius, int color)
{
  int x,y,sum;

  x = 0;
  y = radius << 1;
  sum = 0;

  while (x <= y)
    {
    if ( !(x & 1) )   /* plot if x is even */
      plot8( x >> 1, (y+1) >> 1, color);
    sum += (x << 1) + 1;
    x++;
    if (sum > 0)
      {
      sum -= (y << 1) - 1;
      y--;
      }
    }
}

static void fillrect(int x, int y, int width, int depth, int color)
{
  /* fast version of fillrect */
  if(hasinverse == 0)
    return;
  memset(dstack, color % colors, width);
  while (depth-- > 0)
    {
    if(keypressed()) /* we could do this less often when in fast modes */
      return;
    putrow(x, y++, width, (char *) &dstack);
    }
}

int QueueEmpty()        /* True if NO points remain in queue */
{
  return (ListFront == ListBack);
}

int QueueFull()        /* True if room for NO more points in queue */
{
  return (((ListFront + 1) % ListSize) == ListBack);
}

int QueueFullAlmost()    /* True if room for ONE more point in queue */
{
  return (((ListFront + 2) % ListSize) == ListBack);
}

static void ClearQueue()
{
  ListFront = ListBack = lsize = lmax = 0;
}

int Init_Queue(unsigned long request)
{
  for (ListSize = request; ListSize > 1024; ListSize -= 512)
    switch (common_startdisk(ListSize * 8, 1, 256))
      {
      case 0:                        /* success */
        ListFront = ListBack = 0;
        lsize = lmax = 0;
        return 1;
      case -1:
        continue;            /* try smaller queue size */
      case -2:
        ListSize = 0;               /* cancelled by user      */
        return 0;
      }

  /* failed to get memory for MIIM Queue */
  ListSize = 0;
  return 0;
}

void Free_Queue()
{
  enddisk();
  ListFront = ListBack = ListSize = lsize = lmax = 0;
}

int PushLong(long x, long y)
{
  if (((ListFront + 1) % ListSize) != ListBack)
    {
    if (ToMemDisk(8*ListFront, sizeof(x), &x) &&
    ToMemDisk(8*ListFront +sizeof(x), sizeof(y), &y))
      {
      ListFront = (ListFront + 1) % ListSize;
      if (++lsize > lmax)
        {
        lmax   = lsize;
        luckyx = x;
        luckyy = y;
        }
      return 1;
      }
    }
  return 0;            /* fail */
}

int PushFloat(float x, float y)
{
  if (((ListFront + 1) % ListSize) != ListBack)
    {
    if (ToMemDisk(8*ListFront, sizeof(x), &x) &&
    ToMemDisk(8*ListFront +sizeof(x), sizeof(y), &y))
      {
      ListFront = (ListFront + 1) % ListSize;
      if (++lsize > lmax)
        {
        lmax   = lsize;
        luckyx = x;
        luckyy = y;
        }
      return 1;
      }
    }
  return 0;            /* fail */
}

int EnQueueFloat(float x, float y)
{
  return PushFloat(x, y);
}

int EnQueueLong(long x, long y)
{
  return PushLong(x, y);
}

static Boolean MemoryAlloc(long size)
{
  char *temp;

  if (debugflag == 420)
    return(FALSE);
  temp = (char *)XtMalloc(MEM_RESERVE);   /* minimum free space */

  if (temp == NULL)
    {
    stored_at = NOWHERE;
    return (FALSE);   /* can't do it */
    }

  memory = (char *)XtMalloc( size );
  XtFree(temp);

  if ( memory == NULL )
    {
    stored_at = NOWHERE;
    return (FALSE);
    }
  else
    {
    stored_at = MEMORY;
    return (TRUE);
    }
}

void SaveRect(int x, int y, int width, int depth)
{
  char buff[MAXRECT];
  int  yoff;
  if(hasinverse == 0)
     return;
  /* first, do any de-allocationg */

  switch( stored_at )
    {
    case NOWHERE:
    break;

    case DISK:
    break;

    case MEMORY:
      if (memory != NULL)
        {
        XtFree(memory);
        }
      memory = NULL;
    break;
    }

  /* allocate space and store the rect */

  memset(dstack, color_dark, width);
  if ( MemoryAlloc( (long)width*(long)depth) )
    {
    char *ptr = memory;
    char *bufptr = buff; /* MSC needs this indirection to get it right */

    Cursor_Hide();
    for (yoff=0; yoff<depth; yoff++)
      {
      getrow(x, y+yoff, width, buff);
      putrow(x, y+yoff, width, (char *) &dstack);
      movedata(FP_SEG(bufptr), FP_OFF(bufptr), FP_SEG(ptr), FP_OFF(ptr), width);
      ptr = (char *)normalize(ptr+width);
      }
    Cursor_Show();
    }
  else /* to disk */
    {
    stored_at = DISK;

    if ( file == NULL )
      {
      file = fopen(scrnfile, "w+b");
      if (file == NULL)
        {
        stored_at = NOWHERE;
        buzzer(3);
        return ;
        }
      }
    rewind(file);
    Cursor_Hide();
    for (yoff=0; yoff<depth; yoff++)
      {
      getrow(x, y+yoff, width, buff);
      putrow(x, y+yoff, width, (char *) &dstack);
      if ( fwrite(buff, width, 1, file) != 1 )
        {
        buzzer(3);
        break;
        }
      }
    Cursor_Show();
    }
}

void RestoreRect(int x, int y, int width, int depth)
{
  char buff[MAXRECT];
  int  yoff;
  if(hasinverse == 0)
     return;

  switch ( stored_at )
    {
    case DISK:
      rewind(file);
      Cursor_Hide();
      for (yoff=0; yoff<depth; yoff++)
        {
        if ( fread(buff, width, 1, file) != 1 )
          {
          buzzer(3);
          break;
          }
        putrow(x, y+yoff, width, buff);
        }
      Cursor_Show();
    break;

    case MEMORY:
      {
      char *ptr = memory;
      char *bufptr = buff; /* MSC needs this indirection to get it right */

      Cursor_Hide();
      for (yoff=0; yoff<depth; yoff++)
        {
        movedata(FP_SEG(ptr), FP_OFF(ptr), FP_SEG(bufptr),
          FP_OFF(bufptr), width);
        putrow(x, y+yoff, width, buff);
        ptr = (char *)normalize(ptr+width);
        }
      Cursor_Show();
      break;
      }

    case NOWHERE:
    break;
    } /* switch */
}

/*
 * the following macros and function call the setup, per_pixel, and orbit
 * routines and calculate an orbit at row 0 column 0. Have to save and
 * restore the first elements of dx0 ... dy1 as well as row and col
 */

static int do_fractal_routines(double cr, double ci, int (*func)())
{
  int ret;
  int old_row, old_col;
  double old_dx0, old_dx1, old_dy0, old_dy1;
  old_dx0 = *dx0; old_dx1 = *dx1;
  old_dy0 = *dy0; old_dy1 = *dy1;
  old_row = row;  old_col = col;
  row = col = 0;
  *dx0 = cr; *dy0 = ci; *dx1 = *dy1 = 0.0;
  ret = func();
  *dx0 = old_dx0; *dx1 = old_dx1;
  *dy0 = old_dy0; *dy1 = old_dy1;
  row = old_row;  col = old_col;
  return(ret);
}

void do_jiim()
{
  static int frommandel = 0;
  static double jxxmin, jxxmax, jyymin, jyymax, jxx3rd, jyy3rd;
  char msg[512];
  int junkkey;
  int ret;

  if(viewwindow && !warned)
    {
    sprintf((char *) &msg, "Toggling to or from Mandelbrot/Julia displays\nwhile in preview mode is not recommended. The\ndisplay will probably not behave as expected.\n\nPress OK to continue.\n\nPress Cancel to cancel this operation.");
    ret = confirm(main_window, (char *) &msg,
          OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
    warned = 1;
    if(ret != 1)
      return;
    }

  jiiming = 1;

  junkkey = getakey();

  if (curfractalspecific->tojulia != NOFRACTAL
   && param[0] == 0.0 && param[1] == 0.0)
   {
   /* switch to corresponding Julia set */
   if(jiiming)
     {
     if(fractype==MANDEL || fractype==MANDELFP)
       hasinverse=1;
     else
       hasinverse=0;
     Jiim(JIIM);
     }

   fractype = curfractalspecific->tojulia;

   curfractalspecific = &fractalspecific[fractype];

   if(xcjul == BIG || ycjul == BIG)
     {
     param[0] = (xxmax + xxmin) / 2;
     param[1] = (yymax + yymin) / 2;
     }
   else
     {
     param[0] = xcjul;
     param[1] = ycjul;
     xcjul = ycjul = BIG;
     }
   jxxmin = sxmin; jxxmax = sxmax;
   jyymax = symax; jyymin = symin;
   jxx3rd = sx3rd; jyy3rd = sy3rd;
   frommandel = 1;
   xxmin = curfractalspecific->xmin;
   xxmax = curfractalspecific->xmax;
   yymin = curfractalspecific->ymin;
   yymax = curfractalspecific->ymax;
   xx3rd = xxmin;
   yy3rd = yymin;
   if (usr_distest == 0 && usr_biomorph != -1 && bitshift != 29)
     {
     xxmin *= 3.0;
     xxmax *= 3.0;
     yymin *= 3.0;
     yymax *= 3.0;
     xx3rd *= 3.0;
     yy3rd *= 3.0;
     }
   calc_status = IMG_PRMS_CHGD;
   update_draw_menu(NEW_IMAGE);
   jiiming = 0;
   create_image_cb(main_window, NULL, NULL);
   }
  else if (curfractalspecific->tomandel != NOFRACTAL)
   {
   /* switch to corresponding Mandel set */
   fractype = curfractalspecific->tomandel;
   curfractalspecific = &fractalspecific[fractype];
   if (frommandel)
     {
     frommandel = 0;
     xxmin = jxxmin;  xxmax = jxxmax;
     yymin = jyymin;  yymax = jyymax;
     xx3rd = jxx3rd;  yy3rd = jyy3rd;
     }
  else
     {
     double ccreal,ccimag;
     ccreal = (curfractalspecific->xmax - curfractalspecific->xmin) / 2;
     ccimag = (curfractalspecific->ymax - curfractalspecific->ymin) / 2;
     xxmin = xx3rd = param[0] - ccreal;
     xxmax = param[0] + ccreal;
     yymin = yy3rd = param[1] - ccimag;
     yymax = param[1] + ccimag;
     }
   param[0] = 0;
   param[1] = 0;
   calc_status = IMG_PRMS_CHGD;
   update_draw_menu(NEW_IMAGE);
   jiiming = 0;
   create_image_cb(main_window, NULL, NULL);
   }
  else
   {
   /* can't switch */
   buzzer(2);
   }
  jiiming = 0;
}

void Jiim(int which)
{
  struct affine cvt;
  int exact = 0;
  int oldhelpmode;
  int count = 0;            /* coloring julia */
  static int mode = 0;      /* point, circle, ... */
  int       oldlookatmouse = lookatmouse;
  double cr, ci, r;
  int xfactor, yfactor;             /* aspect ratio          */

  int xoff, yoff;                   /* center of the window  */
  int x, y;
  int still;
  int kbdchar = 0;
  int xcrsr,ycrsr;     /* coords of the cursor / offsets to move it  */
  int iter;
  int color;
  int oldzoomoff;
  float zoom;
  int oldsxoffs, oldsyoffs;
  int savehasinverse;
  int (*oldcalctype)();
  int o_fractype;
  int old_x, old_y;
  double aspect;
  int junkkey;
  static int randir = 0;
  static int rancnt = 0;
  static _CMPLX SaveC = {-3000.0, -3000.0};
  int actively_computing = 1;
  int first_time = 1;
  int cntr = 0;
  int skip = 0;

  /* must use standard fractal and have a float variant */
  if(fractalspecific[fractype].calctype != StandardFractal ||
    (fractalspecific[fractype].isinteger &&
    fractalspecific[fractype].tofloat == NOFRACTAL))
    {
    return;
    }

  oldhelpmode = helpmode;

  /* fixes a bug where Jiim wouldn't work after image save */
  calcfracinit();

  oldzoomoff = zoomoff;
  zoomoff = 1;

  if(which == JIIM)
    helpmode = HELP_JIIM;
  else
    {
    helpmode = HELP_ORBITS;
    hasinverse = 1;
    }

  if(fractalspecific[fractype].isinteger)
    o_fractype = fractalspecific[fractype].tofloat;
  else
    o_fractype = fractype;

  oldsxoffs = sxoffs;
  oldsyoffs = syoffs;

  oldcalctype = calctype;

  show_numbers = 0;
  using_jiim = 1;

  mem_init(strlocn, 10*1024);

  aspect = ((double)xdots*3)/((double)ydots*4);  /* assumes 4:3 */

  actively_computing = 1;
  SetAspect(aspect);
  lookatmouse = 3;

/*
 * MIIM code:
 * Grab memory for Queue/Stack before SaveRect gets it.
 */

  OKtoMIIM  = 0;
  if (which == JIIM && debugflag != 300)
    OKtoMIIM = Init_Queue((long)8*1024); /* Queue Set-up Successful? */

  maxhits = 1;
  if (which == ORBIT)
    plot = c_putcolor;             /* for line with clipping */

/*
 * end MIIM code.
 */

  if(sxoffs != 0 || syoffs != 0) /* we're in view windows */
    {
    savehasinverse = hasinverse;
    hasinverse = 1;
    SaveRect(0,0,xdots,ydots);
    sxoffs = 0;
    syoffs = 0;
    RestoreRect(0,0,xdots,ydots);
    hasinverse = savehasinverse;
    }

  if(which == ORBIT)
    {
    cr = (xxmax + xxmin) / 2.0;
    ci = (yymax + yymin) / 2.0;
    do_fractal_routines(cr, ci,PER_IMAGE);
    }
  else
    color = color_bright;

  oldhelpmode = helpmode;

  if(which == JIIM)
    helpmode = HELP_JIIM;
  else
    helpmode = HELP_ORBITS;

  if(xdots == sxdots || ydots == sydots ||
     sxdots-xdots < sxdots/3 ||
     sydots-ydots < sydots/3 ||
     xdots >= MAXRECT )
    {
    /*
     * this mode puts orbit/julia in an overlapping
     * window 1/3 the size of the physical screen
     */
    windows = 0; /* full screen or large view window */
    xd = sxdots / 3;
    yd = sydots / 3;
    xc = xd * 2;
    yc = yd * 2;
    xoff = xd * 5 / 2;
    yoff = yd * 5 / 2;
    }
  else if(xdots > sxdots/3 && ydots > sydots/3)
    {
    /* Julia/orbit and fractal don't overlap */
    windows = 1;
    xd = sxdots-xdots;
    yd = sydots-ydots;
    xc = xdots;
    yc = ydots;
    xoff = xc + xd/2;
    yoff = yc + yd/2;
    }
  else
    {
    /* Julia/orbit takes whole screen */
    windows = 2;
    xd = sxdots;
    yd = sydots;
    xc = 0;
    yc = 0;
    xoff = xd/2;
    yoff = yd/2;
    }

  xfactor = xd/5.33;
  yfactor = -yd/4;

  if(windows == 0)
    SaveRect(xc,yc,xd,yd);
  else if(windows == 2)  /* leave the fractal */
    {
    fillrect(xdots, yc, xd-xdots, yd, color_dark);
    fillrect(xc   , ydots, xdots, yd-ydots, color_dark);
    }
  else  /* blank whole window */
    fillrect(xc, yc, xd, yd, color_dark);

  setup_convert_to_screen(&cvt);

  /* reuse last location if inside window */
  xcrsr = cvt.a*SaveC.x + cvt.b*SaveC.y + cvt.e + .5;
  ycrsr = cvt.c*SaveC.x + cvt.d*SaveC.y + cvt.f + .5;

  if(xcrsr < 0 || xcrsr >= xdots ||
     ycrsr < 0 || ycrsr >= ydots)
    {
    cr = (xxmax + xxmin) / 2.0;
    ci = (yymax + yymin) / 2.0;
    }
  else
    {
    cr = SaveC.x;
    ci = SaveC.y;
    }

  old_x = old_y = -1;

  xcrsr = cvt.a*cr + cvt.b*ci + cvt.e + .5;
  ycrsr = cvt.c*cr + cvt.d*ci + cvt.f + .5;

  XWarpPointer(display, XtWindow(canvas), XtWindow(canvas),
    0, 0, winwidth, winheight, xcrsr, ycrsr);
  Cursor_Show();
  color = color_bright;

  iter = 1;
  still = 1;
  zoom = 1;

  editpal_cursor = 1;

  while (still)
    {
    int dxcrsr, dycrsr;
    Window rootwindow, childwindow;
    int rootx, rooty;
    int myx, myy;
    int timed_out;
    int tuneval;
    unsigned int mask;

    timed_out = 0;
    tuneval = 20;

    if(++cntr > 100)
      {
      XQueryPointer(display, window, &rootwindow, &childwindow,
        &rootx, &rooty, &myx, &myy, &mask);
      if(first_time)
        {
        oldrootx = rootx;
        oldrooty = rooty;
        oldmyx = myx;
        oldmyy = myy;
        }
      else
        {
        if(oldrootx != rootx || oldrooty != rooty ||
           oldmyx != myx || oldmyy != myy)
          {
          timed_out = 1;
          }
        }
      cursx = myx;
      cursy = myy-window_presets.height_addition;
      oldrootx = rootx;
      oldrooty = rooty;
      oldmyx = myx;
      oldmyy = myy;
      cntr = 0;
      }
    if((waitkeypressed(0) != 0) || timed_out || first_time)
      {
      if(first_time)
        {
        first_time = 0;
        junkkey = getakey();
        }
      while(keypressed() || timed_out)
        {
        if(timed_out)
          {
          xcrsr = cursx;
          ycrsr = cursy;
          timed_out = 0;
          kbdchar = 0;
          break;
          }
        else
          {
          kbdchar = getakey();
          if(skip == 0)
            {
            skip = 1;
            }
          else
            {
            kbdchar = 0;
            skip = 0;
            break;
            }
          }

        dxcrsr = dycrsr = 0;
        xcrsr = cursx;
        ycrsr = cursy;
        xcjul = BIG;
        ycjul = BIG;
        switch (kbdchar)
          {
          case PAGE_UP:
          case PAGE_UP2:
            dxcrsr = 1;
            dycrsr = -1;
          break;

          case PAGE_DOWN:
          case PAGE_DOWN2:
            dxcrsr = 1;
            dycrsr = 1;
          break;

          case HOME:
          case HOME2:
            dxcrsr = -1;
            dycrsr = -1;
          break;

          case END:
          case END2:
            dxcrsr = -1;
            dycrsr = 1;
          break;

          case UP_ARROW:
              dycrsr = -1;
          break;

          case DOWN_ARROW:
              dycrsr = 1;
          break;

          case LEFT_ARROW:
              dxcrsr = -1;
          break;

          case RIGHT_ARROW:
              dxcrsr = 1;
          break;

          case XMKEY_z:
          case XMKEY_Z:
            zoom = 1.0;
          break;

          case LESS_THAN:
            zoom /= 1.15;
          break;

          case GREATER_THAN:
            zoom *= 1.15;
          break;

          case SPACE:
            xcjul = cr;
            ycjul = ci;
            getakey();
            goto finish;
          break;

          case ESC:
            xcjul = cr;
            ycjul = ci;
            getakey();
            goto finish;
          break;

          case XMKEY_C:   /* circle toggle */
            mode = mode ^ 1;
          break;

          case XMKEY_L:
            mode = mode ^ 2;
          break;

          case XMKEY_h:   /* hide fractal toggle */
          case XMKEY_H:   /* hide fractal toggle */
            if(windows == 2)
              windows = 3;
            else if(windows == 3 && xd == sxdots)
              {
              RestoreRect(0, 0, xdots, ydots);
              windows = 2;
              }
          break;

          case XMKEY_0:
            if (which == JIIM)
              {
              SecretExperimentalMode = 0;
              break;
              }
          case XMKEY_1:
            if (which == JIIM)
              {
              SecretExperimentalMode = 1;
              break;
              }
          case XMKEY_2:
            if (which == JIIM)
              {
              SecretExperimentalMode = 2;
              break;
              }
          case XMKEY_4:
            if (which == JIIM)
              {
              SecretExperimentalMode = 4;
              break;
              }
          case XMKEY_5:
            if (which == JIIM)
              {
              SecretExperimentalMode = 5;
              break;
              }
          case XMKEY_6:
            if (which == JIIM)
              {
              SecretExperimentalMode = 6;
              break;
              }
          case XMKEY_7:
            if (which == JIIM)
              {
              SecretExperimentalMode = 7;
              break;
              }
          case XMKEY_8:
            if (which == JIIM)
              {
              SecretExperimentalMode = 8;
              break;
              }
          case XMKEY_9:
            if (which == JIIM)
              {
              SecretExperimentalMode = 9;
              break;
              }

          case ENTER:
          case RETURN:
          break;

          default:
            exact=0;
            still = 0;
            xcrsr = cursx;
            ycrsr = cursy;
          }  /* switch */

        if(kbdchar == XMKEY_s || kbdchar == XMKEY_S)
          goto finish;

        if(dxcrsr > 0 || dycrsr > 0)
          exact = 0;

        if (kbdchar == ENTER || kbdchar == RETURN)
          {
          /* We want to use the position of the cursor */
          exact=0;
          xcrsr = cursx;
          ycrsr = cursy;
          }

        xcrsr += dxcrsr;
        ycrsr += dycrsr;

        /* keep cursor in logical screen */
        if(xcrsr >= xdots)
          xcrsr = xdots -1, exact = 0;
        if(ycrsr >= ydots)
          ycrsr = ydots -1, exact = 0;
        if(xcrsr < 0)
          xcrsr = 0, exact = 0;
        if(ycrsr < 0)
          ycrsr = 0, exact = 0;

        XWarpPointer(display, XtWindow(canvas), XtWindow(canvas),
                     0, 0, winwidth, winheight, xcrsr, ycrsr);
        }  /* end while (keypressed) */

      if(exact == 0)
        {
        if(integerfractal)
          {
          cr = lx0[xcrsr]+lx1[ycrsr];  /* supports rotated zoom boxes! */
          ci = ly0[ycrsr]+ly1[xcrsr];
          cr /= (1L<<bitshift);
          ci /= (1L<<bitshift);
          }
        else
          {
          cr = dx0[xcrsr]+dx1[ycrsr];
          ci = dy0[ycrsr]+dy1[xcrsr];
          }
        }

      actively_computing = 1;
      if(show_numbers) /* write coordinates on screen */
        {
        char str[80];
        sprintf(str,"x=%16.14f y=%16.14f   ",cr,ci);
        str[40] = 0;
        if(windows == 0)
          {
          Cursor_Hide();
          actively_computing = 1;
          Cursor_Show();
          }
        }

      iter = 1;
      old.x = old.y = 0;
      SaveC.x = init.x =  cr;
      SaveC.y = init.y =  ci;
      old_x = old_y = -1;
/*
 * MIIM code:
 * compute fixed points and use them as starting points of JIIM
 */
      if (which == JIIM && OKtoMIIM)
        {
        _CMPLX f1, f2, Sqrt;    /* Fixed points of Julia */
        Sqrt = ComplexSqrtFloat(1 - 4 * cr, -4 * ci);
        f1.x = (1 + Sqrt.x) / 2;
        f2.x = (1 - Sqrt.x) / 2;
        f1.y =  Sqrt.y / 2;
        f2.y = -Sqrt.y / 2;
        ClearQueue();
        maxhits = 1;
        EnQueueFloat(f1.x, f1.y);
        EnQueueFloat(f2.x, f2.y);
        }
/*
 * End MIIM code.
 */
      if(which == ORBIT)
        do_fractal_routines(cr, ci,PER_PIXEL);
      /* move window if bumped */
      if(windows==0 && xcrsr>xc && xcrsr < xc+xd && ycrsr>yc && ycrsr < yc+yd)
        {
        RestoreRect(xc,yc,xd,yd);
        if (xc == xd*2)
          xc = 2;
        else
          xc = xd*2;
        xoff = xc + xd /  2;
        SaveRect(xc,yc,xd,yd);
        }

      if(windows == 2)
        {
        fillrect(xdots, yc, xd-xdots, yd-show_numbers, color_dark);
        fillrect(xc   , ydots, xdots, yd-ydots-show_numbers, color_dark);
        }
      else
        fillrect(xc, yc, xd, yd, color_dark);

      } /* end if (waitkeypressed) */

    if(which == JIIM)
      {
      if(hasinverse == 0)
        continue;
/*
 * MIIM code:
 * If we have MIIM queue allocated, then use MIIM method.
 */
    if (OKtoMIIM)
      {
      if (QueueEmpty())
        {
        if (maxhits < colors - 1 && maxhits < 5 &&
          (luckyx != 0.0 || luckyy != 0.0))
          {
          int i;

          lsize  = lmax   = 0;
          old.x  = new.x  = luckyx;
          old.y  = new.y  = luckyy;
          luckyx = luckyy = 0.0;
          for (i=0; i<199; i++)
            {
            old = ComplexSqrtFloat(old.x - cr, old.y - ci);
            new = ComplexSqrtFloat(new.x - cr, new.y - ci);
            EnQueueFloat( new.x,  new.y);
            EnQueueFloat(-old.x, -old.y);
            }
          maxhits++;
          }
        else
          continue;        /* loop while (still) */
        }

      old = DeQueueFloat();

      x = old.x * xfactor * zoom + xoff;
      y = old.y * yfactor * zoom + yoff;
      color = c_getcolor(x, y);
      if (color < maxhits)
        {
        c_putcolor(x, y, color + 1);
        new = ComplexSqrtFloat(old.x - cr, old.y - ci);
        EnQueueFloat( new.x,  new.y);
        EnQueueFloat(-new.x, -new.y);
        }
      }
    else
      {
/*
 * end Msnyder code, commence if not MIIM code.
 */
      old.x -= cr;
      old.y -= ci;
      r = old.x*old.x + old.y*old.y;
      if(r > 10.0)
        {
        old.x = old.y = 0.0; /* avoids math error */
        iter = 1;
        }
      iter++;
      color = ((count++)>>5)&(colors-1); /* chg color every 32 pts */

      if(color==0)
        color = 1;

      r = sqrt(old.x*old.x + old.y*old.y);
      new.x = sqrt(fabs((r + old.x)/2));

      if (old.y < 0)
        new.x = -new.x;

      new.y = sqrt(fabs((r - old.x)/2));


      switch (SecretExperimentalMode)
        {
        case 0:            /* unmodified random walk */
        default:
          if (rand() % 2)
            {
            new.x = -new.x;
            new.y = -new.y;
            }
          x = new.x * xfactor * zoom + xoff;
          y = new.y * yfactor * zoom + yoff;
        break;

        case 1:              /* always go one direction */
          if (SaveC.y < 0)
            {
            new.x = -new.x;
            new.y = -new.y;
            }
          x = new.x * xfactor * zoom + xoff;
          y = new.y * yfactor * zoom + yoff;
        break;

        case 2:            /* go one dir, draw the other */
          if (SaveC.y < 0)
            {
            new.x = -new.x;
            new.y = -new.y;
            }
          x = -new.x * xfactor * zoom + xoff;
          y = -new.y * yfactor * zoom + yoff;
        break;

        case 4:            /* go negative if max color */
          x = new.x * xfactor * zoom + xoff;
          y = new.y * yfactor * zoom + yoff;
          if (c_getcolor(x, y) == colors - 1)
            {
            new.x = -new.x;
            new.y = -new.y;
            x = new.x * xfactor * zoom + xoff;
            y = new.y * yfactor * zoom + yoff;
            }
        break;

        case 5:            /* go positive if max color */
          new.x = -new.x;
          new.y = -new.y;
          x = new.x * xfactor * zoom + xoff;
          y = new.y * yfactor * zoom + yoff;
          if (c_getcolor(x, y) == colors - 1)
            {
            x = new.x * xfactor * zoom + xoff;
            y = new.y * yfactor * zoom + yoff;
            }
        break;

        case 7:
          if (SaveC.y < 0)
            {
            new.x = -new.x;
            new.y = -new.y;
            }
          x = -new.x * xfactor * zoom + xoff;
          y = -new.y * yfactor * zoom + yoff;
          if(iter > 10)
            {
            if(mode == 0)            /* pixels  */
              c_putcolor(x, y, color);
            else if (mode & 1)            /* circles */
              {
              xbase = x;
              ybase = y;
              circle((int)(zoom*(xd >> 1)/iter),color);
              }
            if ((mode & 2) && x > 0 && y > 0 && old_x > 0 && old_y > 0)
              {
              draw_line(x, y, old_x, old_y, color);
              }
            old_x = x;
            old_y = y;
            }
          x = new.x * xfactor * zoom + xoff;
          y = new.y * yfactor * zoom + yoff;
        break;

        case 8:            /* go in long zig zags */
          if (rancnt >= 300)
            rancnt = -300;
          if (rancnt < 0)
            {
            new.x = -new.x;
            new.y = -new.y;
            }
          x = new.x * xfactor * zoom + xoff;
          y = new.y * yfactor * zoom + yoff;
        break;

        case 9:            /* "random run" */
          switch (randir)
            {
            case 0:             /* go random direction for a while */
            if (rand() % 2)
              {
              new.x = -new.x;
              new.y = -new.y;
              }
            if (++rancnt > 1024)
              {
              rancnt = 0;
              if (rand() % 2)
                randir =  1;
              else
                randir = -1;
              }
            break;

            case 1:             /* now go negative dir for a while */
              new.x = -new.x;
              new.y = -new.y;
              /* fall through */
            case -1:            /* now go positive dir for a while */
              if (++rancnt > 512)
                randir = rancnt = 0;
            break;
            }
          x = new.x * xfactor * zoom + xoff;
          y = new.y * yfactor * zoom + yoff;
        break;
        } /* end switch SecretMode */
      } /* end if not MIIM */
    } /* end if JIIM */
  else /* orbits */
    {
    if(iter < maxit)
      {
      color = iter&(colors-1);
      x = (old.x - init.x) * xfactor * 3 * zoom + xoff;
      y = (old.y - init.y) * yfactor * 3 * zoom + yoff;
      if(do_fractal_routines(cr, ci,ORBITCALC))
        iter = maxit;
      else
        iter++;
      }
    else
      {
      x = y = -1;
      actively_computing = 0;
      }
    }

  if(which == ORBIT || iter > 10)
    {
    if(mode == 0)            /* pixels  */
      c_putcolor(x, y, color);
    else if (mode & 1)            /* circles */
      {
      xbase = x;
      ybase = y;
      circle((int)(zoom*(xd >> 1)/iter),color);
      }

    if ((mode & 2) && x > 0 && y > 0 && old_x > 0 && old_y > 0)
      {
      draw_line(x, y, old_x, old_y, color);
      }
    old_x = x;
    old_y = y;
    }
  old = new;
  } /* end while(still) */
finish:

/*
 * Msnyder code:
 * XtFree MIIM queue
 */

  Free_Queue();
/*
 * end Msnyder code.
 */

  if(kbdchar != XMKEY_s && kbdchar != XMKEY_S && kbdchar != ESC)
    {
    Cursor_Hide();
    if(windows == 0)
      RestoreRect(xc,yc,xd,yd);
    else if(windows >= 2 )
      {
      if(windows == 2)
        {
        fillrect(xdots, yc, xd-xdots, yd, color_dark);
        fillrect(xc   , ydots, xdots, yd-ydots, color_dark);
        }
      else
        fillrect(xc, yc, xd, yd, color_dark);
      if(windows == 3 && xd == sxdots) /* unhide */
        {
        RestoreRect(0, 0, xdots, ydots);
        windows = 2;
        }
      Cursor_Hide();
      savehasinverse = hasinverse;
      hasinverse = 1;
      SaveRect(0,0,xdots,ydots);
      sxoffs = oldsxoffs;
      syoffs = oldsyoffs;
      RestoreRect(0,0,xdots,ydots);
      hasinverse = savehasinverse;
      }
    }
  Cursor_Hide();
  editpal_cursor = 0;
  if (memory)            /* done with memory, free it */
    {
    XtFree(memory);
    memory = NULL;
    }

  lookatmouse = oldlookatmouse;
  using_jiim = 0;
  calctype = oldcalctype;

  helpmode = oldhelpmode;
  if(kbdchar == XMKEY_s || kbdchar == XMKEY_S)
    {
    viewwindow = viewxdots = viewydots = 0;
    viewreduction = 4.2;
    viewcrop = 1;
    finalaspectratio = screenaspect;
    xdots = sxdots;
    ydots = sydots;
    dx_size = xdots - 1;
    dy_size = ydots - 1;
    sxoffs = 0;
    syoffs = 0;
    }

  if (file != NULL)
    {
    fclose(file);
    file = NULL;
    remove(scrnfile);
    }
  show_numbers = 0;
  zoomoff = oldzoomoff;
}

static void Cursor_Hide(void)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_top_left_arrow));
}

static void Cursor_Show(void)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_circle));
}

static void mem_init(void *block, unsigned size)
{
  mem_block = (char *)block;
  mem_avail = size;
}

static void getrow(int x, int y, int width, char *buff)
{
  clip_get_line(y, x, x+width-1, (unsigned char *)buff);
}

static void putrow(int x, int y, int width, char *buff)
{
  clip_put_line(y, x, x+width-1, (unsigned char *)buff);
}

static void clip_put_line(int row, int start, int stop, unsigned char *pixels)
{
  if ( row < 0 || row >= sydots || start > sxdots || stop < 0 )
    return;

  if ( start < 0 )
    {
    pixels += -start;
    start = 0;
    }

  if ( stop >= sxdots )
    stop = sxdots - 1;

  if ( start > stop )
    return;

  put_line(row, start, stop, pixels);
}

static void clip_get_line(int row, int start, int stop, unsigned char *pixels)
{
  if ( row < 0 || row >= sydots || start > sxdots || stop < 0 )
    return;

  if ( start < 0 )
    {
    pixels += -start;
    start = 0;
    }

  if ( stop >= sxdots )
    stop = sxdots - 1;

  if ( start > stop )
    return;

  get_line(row, start, stop, pixels);
}

/*
 * JIIM.C
 *
 * Generates Inverse Julia in real time, lets move a cursor which determines
 * the J-set.
 *
 *  The J-set is generated in a fixed-size window, a third of the screen.
 *
 * The routines used to set/move the cursor and to save/restore the
 * window were "borrowed" from editpal.c (TW - now we *use* the editpal code)
 *     (if you don't know how to write good code, look for someone who does)
 *
 *    JJB  [jbuhler@gidef.edu.ar]
 *    TIW  Tim Wegner
 *    MS   Michael Snyder
 *    KS   Ken Shirriff
 * Revision History:
 *
 *        7-28-92       JJB  Initial release out of editpal.c
 *        7-29-92       JJB  Added SaveRect() & RestoreRect() - now the
 *                           screen is restored after leaving.
 *        7-30-92       JJB  Now, if the cursor goes into the window, the
 *                           window is moved to the other side of the screen.
 *                           Worked from the first time!
 *        10-09-92      TIW  A major rewrite that merged cut routines duplicated
 *                           in EDITPAL.C and added orbits feature.
 *        11-02-92      KS   Made cursor blink
 *        11-18-92      MS   Altered Inverse Julia to use MIIM method.
 *        11-25-92      MS   Modified MIIM support routines to better be
 *                           shared with stand-alone inverse julia in
 *                           LORENZ.C, and to use DISKVID for swap space.
 *        05-05-93      TIW  Boy this change file really got out of date.
 *                           Added orbits capability, various commands, some
 *                           of Dan Farmer's ideas like circles and lines
 *                           connecting orbits points.
 */

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: jiim.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.7  1995/05/15  19:30:32  darryl
 * Warn user only once about using jiim with preview mode.
 *
 * Revision 1.6  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.5  1995/04/10  22:12:57  darryl
 * Add rate=nnn and menu support for setting the screen refresh
 * rate during image calculation. Fix a bug where the Julia
 * inverse mode was broken after an image save. Add fallback
 * resources that take effect if a resource file is not found.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.1  1995/03/11  03:44:04  tom
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
 * Revision 0.105  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.104  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.103  1994/11/29  02:08:16  darryl
 * Reverse the operation of the zoomoff variable so
 * that a non-zero value means that zooming is disabled.
 *
 * Revision 0.102  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.101  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
