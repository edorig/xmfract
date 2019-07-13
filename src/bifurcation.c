
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: bifurcation.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* Original code by Phil Wilson, hacked around by Kev Allen.   */

/* static functions */
static void verhulst(void);
static void Bif_Period_Init(void);
static int Bif_Periodic(int time);

/* static variables */
static double Bif_closenuf;
static double Bif_savedpop;
static int *verhulst_array;
static int Bif_savedand;
static int Bif_savedinc;
static int outside_x;
static long lbeta;
static unsigned int half_time_check;

int Bifurcation(void)
{
  unsigned long array_size;
  int row, column;
  column = 0;
  if (resuming)
    {
    start_resume();
    get_resume(sizeof(int),&column,0);
    end_resume();
    }

  array_size =  (iystop + 1) * sizeof(int); /* should be iystop + 1 */

  if ((verhulst_array = (int *) XtMalloc(array_size)) == NULL)
    return(-1);

  for (row = 0; row <= iystop; row++) /* should be iystop */
    verhulst_array[row] = 0;

  mono = 0;

  if (colors == 2)
    mono = 1;

  if (mono)
    {
    if (inside)
      {
      outside_x = 0;
      inside = 1;
      }
    else
      outside_x = 1;
    }

  filter_cycles = (parm.x <= 0) ? DEFAULTFILTER : parm.x;

  half_time_check = FALSE;

  if (periodicitycheck && maxit < filter_cycles)
    {
    filter_cycles = (filter_cycles - maxit + 1) / 2;
    half_time_check = TRUE;
    }

  init.y = dy0[iystop];   /* bottom pixels */

  while (column <= ixstop)
    {
    if(check_key() != 0)
      {
      XtFree((char *)verhulst_array);
      alloc_resume(10,1);
      put_resume(sizeof(int),&column,0);
      return(-1);
      }

    Rate = dx0[column];

    verhulst();     /* calculate array once per column */

    for (row = iystop; row >= 0; row--) /* should be iystop & >=0 */
      {
      int color;
      color = verhulst_array[row];
      if(color && mono)
        color = inside;
      else if((!color) && mono)
        color = outside_x;
      else if (color>=colors)
        color = colors-1;

      verhulst_array[row] = 0;

      (*plot)(column,row,color); /* was row-1, but that's not right? */
      }
    column++;
    }
  XtFree((char *)verhulst_array);
  return(0);
}

static void verhulst()        /* P. F. Verhulst (1845) */
{
  unsigned int pixel_row, counter, errors;

  Population = (parm.y == 0 ) ? SEED : parm.y;

  errors = overflow = FALSE;

  for (counter=0 ; counter < filter_cycles ; counter++)
    {
    errors = (*(curfractalspecific->orbitcalc))();
    if (errors)
    return;
    }

  if (half_time_check) /* check for periodicity at half-time */
    {
    Bif_Period_Init();
    for (counter=0 ; counter < maxit ; counter++)
      {
      errors = (*(curfractalspecific->orbitcalc))();
      if (errors) return;
      if (periodicitycheck && Bif_Periodic(counter)) break;
      }

    if (counter >= maxit)   /* if not periodic, go the distance */
      {
      for (counter=0 ; counter < filter_cycles ; counter++)
        {
        errors = (*(curfractalspecific->orbitcalc))();
        if (errors) return;
        }
      }
    }

  if (periodicitycheck) Bif_Period_Init();

  for (counter=0 ; counter < maxit ; counter++)
    {
    errors = (*(curfractalspecific->orbitcalc))();
    if (errors) return;

    /* assign population value to Y coordinate in pixels */
    pixel_row = iystop - (int)((Population - init.y) / deltaY);

    /* if it's visible on the screen, save it in the column array */
    if (pixel_row <= iystop) /* JCO 6/6/92 */
    verhulst_array[ pixel_row ] ++;
    if (periodicitycheck && Bif_Periodic(counter))
      {
      if (pixel_row <= iystop) /* JCO 6/6/92 */
        verhulst_array[ pixel_row ] --;
      break;
      }
    }
}

static void Bif_Period_Init()
{
  Bif_savedinc = 1;
  Bif_savedand = 1;
  Bif_savedpop = -1.0;
  Bif_closenuf = deltaY / 8.0;
}

static int Bif_Periodic(int time)
{
  if ((time & Bif_savedand) == 0)    /* time to save a new value */
    {
    Bif_savedpop =  Population;
    if (--Bif_savedinc == 0)
      {
      Bif_savedand = (Bif_savedand << 1) + 1;
      Bif_savedinc = 4;
      }
    }
  else             /* check against an old save */
    {
    if (fabs(Bif_savedpop-Population) <= Bif_closenuf)
      return(1);
    }
  return(0);
}

/**********************************************************************/
/*                                                                    */
/* The following are Bifurcation "orbitcalc" routines...              */
/*                                                                    */
/**********************************************************************/

int BifurcLambda() /* Used by lyanupov */
{
  Population = Rate * Population * (1 - Population);
  return (fabs(Population) > BIG);
}

int BifurcVerhulstTrig()
{
  /*  Population = Pop + Rate * fn(Pop) * (1 - fn(Pop)) */
  tmp.x = Population;
  tmp.y = 0;
  CMPLXtrig0(tmp, tmp);
  Population += Rate * tmp.x * (1 - tmp.x);
  return (fabs(Population) > BIG);
}

int BifurcStewartTrig()
{
  /*  Population = (Rate * fn(Population) * fn(Population)) - 1.0 */
  tmp.x = Population;
  tmp.y = 0;
  CMPLXtrig0(tmp, tmp);
  Population = (Rate * tmp.x * tmp.x) - 1.0;
  return (fabs(Population) > BIG);
}

int BifurcSetTrigPi()
{
  tmp.x = Population * PI;
  tmp.y = 0;
  CMPLXtrig0(tmp, tmp);
  Population = Rate * tmp.x;
  return (fabs(Population) > BIG);
}

int BifurcAddTrigPi()
{
  tmp.x = Population * PI;
  tmp.y = 0;
  CMPLXtrig0(tmp, tmp);
  Population += Rate * tmp.x;
  return (fabs(Population) > BIG);
}

int BifurcLambdaTrig()
{
  /* Population = Rate * fn(Population) * (1 - fn(Population)) */
  tmp.x = Population;
  tmp.y = 0;
  CMPLXtrig0(tmp, tmp);
  Population = Rate * tmp.x * (1 - tmp.x);
  return (fabs(Population) > BIG);
}

int BifurcMay()
{
  /*
   * X = (lambda * X) / (1 + X)^lbeta, from R.May as described in Pickover,
   * Computers, Pattern, Chaos, and Beauty, page 153
   */
  tmp.x = 1.0 + Population;
  tmp.x = pow(tmp.x, -lbeta); /* pow in math.h included with mpmath.h */
  Population = (Rate * Population) * tmp.x;
  return (fabs(Population) > BIG);
}

int BifurcMaySetup()
{
 lbeta = (long)param[2];
 if(lbeta < 2)
   lbeta = 2;
 param[2] = (double)lbeta;

 timer(0,curfractalspecific->calctype);
 return(0);
}

/* Here Endeth the Generalised Bifurcation Fractal Engine            */
/* END Phil Wilson's Code (modified slightly by Kev Allen et. al. !) */

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: bifurcation.c,v $
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
 * Revision 0.101  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
