
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: diffusion.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

int diffusion()
{
  int xmax,ymax,xmin,ymin;    /* Current maximum coordinates */
  int border;  /* Distance between release point and fractal */
  int mode;    /* Determines diffusion type:  0 = central (classic) */
            /*                    1 = falling particles */
            /*                    2 = square cavity    */
  int i;
  double cosine,sine,angle;
  long lcosine,lsine;
  int x,y;
  float r, radius, f_tmp;

  x = y = 0;

  if (diskvideo)
    notdiskmsg();

  bitshift = 16;
  fudge = 1L << 16;

  border = param[0];
  mode = param[1];
  if (mode > 2)
    mode=0;

  if (border <= 0)
    border = 10;

  srand(rseed);

  if (!rflag)
    ++rseed;

  if (mode == 0)
    {
    xmax = xdots / 2 + border;  /* Initial box */
    xmin = xdots / 2 - border;
    ymax = ydots / 2 + border;
    ymin = ydots / 2 - border;
    }

  if (mode == 1)
    {
    xmax = xdots / 2 + border;  /* Initial box */
    xmin = xdots / 2 - border;
    ymin = ydots - 20;
    }

  if (mode == 2)
    {
    if (xdots>ydots)
      radius = ydots - 5;
    else
      radius = xdots - 5;
    }

  if (resuming) /* restore worklist, if we can't the above will stay in place */
    {
    start_resume();
    if (mode != 2)
      get_resume(sizeof(int),&xmax,sizeof(int),&xmin,sizeof(int),&ymax,
        sizeof(int),&ymin,0);
    else
      get_resume(sizeof(int),&xmax,sizeof(int),&xmin,sizeof(int),&ymax,
        sizeof(int),&radius,0);
    end_resume();
    }

  if (mode==0)
    putcolor(xdots / 2, ydots / 2,RANDOM(colors-1)+1);  /* Seed point */

  if (mode==1)
    for (i=0;i<=xdots;i++)
      putcolor(i,ydots-1,colors-1);

  if (mode==2)
    {
    if (xdots>ydots)
      {
      for (i=0;i<ydots;i++)
        {
        putcolor(xdots/2-ydots/2 , i , colors-1);
        putcolor(xdots/2+ydots/2 , i , colors-1);
        putcolor(xdots/2-ydots/2+i , 0 , colors-1);
        putcolor(xdots/2-ydots/2+i , ydots-1 , colors-1);
        }
      }
    else
      {
      for (i=0;i<xdots;i++)
        {
        putcolor(0 , ydots/2-xdots/2+i , colors-1);
        putcolor(xdots-1 , ydots/2-xdots/2+i , colors-1);
        putcolor(i , ydots/2-xdots/2 , colors-1);
        putcolor(i , ydots/2+xdots/2 , colors-1);
        }
      }
    }

  while (1)
    {
    /* Release new point on circle just inside the box */

    if (mode==0)
      {
      if (floatflag)
        {
        angle=2*(double)rand()/(RAND_MAX/PI);
        FPUsincos(&angle,&sine,&cosine);
        x = cosine*(xmax-xmin) + xdots;
        y = sine  *(ymax-ymin) + ydots;
        }
      else
        {
        SinCos086(multiply((long)rand15(),FOURPI,16),&lsine,&lcosine);
        x = (lcosine*(long)(xmax-xmin) >> 16) + xdots;
        y = (lsine  *(long)(ymax-ymin) >> 16) + ydots;
        }

      x = x >> 1; /* divide by 2 */
      y = y >> 1;
      }
    if (mode==1)
      {
      y=ymin;
      x=RANDOM(xmax-xmin) + (xdots-xmax+xmin)/2;
      }
    if (mode==2)
      {
      if (floatflag)
        {
        angle=2*(double)rand()/(RAND_MAX/PI);
        FPUsincos(&angle,&sine,&cosine);
        x = cosine*radius + xdots;
        y = sine  *radius + ydots;
        }
      else
        {
        SinCos086(multiply((long)rand15(),FOURPI,16),&lsine,&lcosine);
        x = (lcosine*(long)(radius) >> 16) + xdots;
        y = (lsine  *(long)(radius) >> 16) + ydots;
        }
      x = x >> 1;
      y = y >> 1;
      }
    /* Loop as long as the point (x,y) is surrounded by color 0 */
    /* on all eight sides                          */
    while((getcolor(x+1,y+1) == 0) && (getcolor(x+1,y) == 0) &&
       (getcolor(x+1,y-1) == 0) && (getcolor(x  ,y+1) == 0) &&
       (getcolor(x  ,y-1) == 0) && (getcolor(x-1,y+1) == 0) &&
       (getcolor(x-1,y) == 0) && (getcolor(x-1,y-1) == 0))
      {
      /* Erase moving point */
      if (show_orbit)
        putcolor(x,y,0);

      /* Make sure point is inside the box (if mode==0)*/
      if (mode==0){
        if (x==xmax)
          x--;
        else if (x==xmin)
          x++;
        if (y==ymax)
          y--;
        else if (y==ymin)
          y++;
        }

      if (mode==1)
        {
        if (x>xdots-2)
          x--;
        else if (x<1)
          x++;
        if (y<ymin)
          y++;
        }

      /* Take one random step */
      x += RANDOM(3) - 1;
      y += RANDOM(3) - 1;

      /* Check keyboard */
      if ((++plasma_check & 0x7f) == 1)
        if(check_key() != 0)
          {
          alloc_resume(20,1);
          if (mode!=2)
            put_resume(sizeof(int),&xmax,sizeof(int),&xmin, sizeof(int),&ymax,
               sizeof(int),&ymin,0);
          else
            put_resume(sizeof(int),&xmax,sizeof(int),&xmin, sizeof(int),&ymax,
               sizeof(int),&radius,0);

          plasma_check--;
          return 1;
          }

      /* Show the moving point */
      if (show_orbit)
        putcolor(x,y,RANDOM(colors-1)+1);

      }
    putcolor(x,y,RANDOM(colors-1)+1);

    /* Is point too close to the edge? */

    if (mode==0)
      {
      if (((x+border)>xmax) || ((x-border)<xmin)
         || ((y-border)<ymin) || ((y+border)>ymax))
        {
        /* Increase box size, but not past the edge of the screen */
        if (ymin != 1)
          {
          ymin--;
          ymax++;
          }
        if (xmin != 1)
          {
          xmin--;
          xmax++;
          }
        if ((ymin==1) || (xmin==1))
          return 0;
        }
      }
    if (mode==1)
      {
      if (y < ymin+5)
        ymin = y - 5;
      if (ymin<2)
        return 0;
      }
    if (mode==2)
      {
      if (abs(x-xdots/2)<5 && abs(y-ydots/2)<5)
        return 0;

      r = (x-xdots/2)*(x-xdots/2)+(y-ydots/2)*(y-ydots/2);
      fSqrt14(r,f_tmp);
      r = 2 * f_tmp;
      if (r < radius)
        radius = r;
      }
    }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: diffusion.c,v $
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
