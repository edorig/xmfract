
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: fractals.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int Halleybailout(void);
static int longbailout(void);
static int TrigPlusSqrlongSetup(void);
static int TrigPlusSqrfpSetup(void);
static int TrigPlusSqrfpFractal(void);
static int ScottTrigPlusSqrfpFractal(void);
static int SkinnerTrigSubSqrfpFractal(void);
static int ScottTrigPlusTrigfpFractal(void);
static int SkinnerTrigSubTrigfpFractal(void);
static int ScottZXTrigPlusZfpFractal(void);
static int SkinnerZXTrigSubZfpFractal(void);
static int LambdaTrigfpFractal1(void);
static int lcpower(_LCMPLX *base, int exp, _LCMPLX *result, int bitshift);
static void FloatPreCalcMagnet2(void);
static void HComplexTrig0(_HCMPLX *h, _HCMPLX *out);

/* static variables */
static _CMPLX  T_Cm1;
static _CMPLX  T_Cm1Cm2;
static _CMPLX  T_Cm2;
static _CMPLX tmp2;
static _LCMPLX *longparm;
static _LCMPLX lcoefficient;
static _LCMPLX ltmp;
static double foldxinitx;
static double foldxinity;
static double foldyinitx;
static double foldyinity;
static double tmpexp;
static int Ap1deg;
static long FgHalf;
static long lxt;
static long lyt;
static long oldxinitx;
static long oldxinity;
static long oldyinity;

static int Halleybailout()
{
  if ( fabs(modulus(new)-modulus(old)) < parm2.x)
    return(1);
  old = new;
  return(0);
}

static int longbailout()
{
  ltempsqrx = lsqr(lnew.x); ltempsqry = lsqr(lnew.y);
  lmagnitud = ltempsqrx + ltempsqry;
  if (lmagnitud >= llimit || lmagnitud < 0 || labs(lnew.x) > llimit2
    || labs(lnew.y) > llimit2 || overflow)
    {
    overflow=0;
    return(1);
    }
  lold = lnew;
  return(0);
}

/* Mandelbrot Routine */
int MandelSetup()
{
  if (debugflag != 90 && ! invert && decomp[0] == 0 && rqlim <= 4.0
       && bitshift == 29 && potflag == 0
       && biomorph == -1 && inside > -59 && outside >= -1
       && useinitorbit != 1 && using_jiim == 0)
    calctype = calcmand; /* the normal case - use CALCMAND */
  else
    {
    /* special case: use the main processing loop */
    calctype = StandardFractal;
    longparm = &linit;
    }
  return(1);
}

int MandelfpSetup()
{
  c_exp = param[2];
  pwr.x = param[2] - 1.0;
  pwr.y = param[3];
  floatparm = &init;

  switch (fractype)
    {
    case MARKSMANDELFP:
      if(c_exp < 1)
        c_exp = 1;
      if(!(c_exp & 1))
        symmetry = XYAXIS_NOPARM;    /* odd exponents */
      if(c_exp & 1)
        symmetry = XAXIS_NOPARM;
    break;

    case MANDELFP:
      /*
       * floating point code could probably be altered to handle many of
       * the situations that otherwise are using StandardFractal().
       * calcmandfp() can currently handle invert, any rqlim, potflag
       * zmag, epsilon cross, and all the current outside options
       *                   Wes Loewer 11/03/91
       */
      if (debugflag != 90
          && !distest
          && decomp[0] == 0
          && biomorph == -1
          && (inside >= -1 || inside == -59 || inside == -100)
          /* uncomment this next line if more outside options are added */
          /* && outside >= -5 */
          && useinitorbit != 1
          && using_jiim == 0)
        {
        calctype = calcmandfp; /* the normal case - use calcmandfp */
        calcmandfpasmstart();
        }
      else
        {
        /* special case: use the main processing loop */
        calctype = StandardFractal;
        }
    break;

    case FPMANDELZPOWER:
      if((double)c_exp == param[2] && c_exp & 1) /* odd exponents */
        symmetry = XYAXIS_NOPARM;
      if(param[3] != 0)
        symmetry = NOSYM;
      if(param[3] == 0.0 && debugflag != 6000 && (double)c_exp == param[2])
        fractalspecific[fractype].orbitcalc = floatZpowerFractal;
      else
        fractalspecific[fractype].orbitcalc = floatCmplxZpowerFractal;
    break;

    case MAGNET1M:
    case MAGNET2M:
      attr[0].x = 1.0;        /* 1.0 + 0.0i always attracts */
      attr[0].y = 0.0;        /* - both MAGNET1 and MAGNET2 */
      attrperiod[0] = 1;
      attractors = 1;
    break;

    case SPIDERFP:
      if(periodicitycheck==1) /* if not user set */
        periodicitycheck=4;
    break;

    case TRIGPLUSTRIG:
      symmetry = XAXIS_NOPARM;
    break;

    /* Added to account for symmetry in manfn+exp and manfn+zsqrd */
    /*     JCO 2/29/92 */
    case FPMANTRIGPLUSEXP:
    case FPMANTRIGPLUSZSQRD:
      if(parm.y == 0.0)
        symmetry = XAXIS;
      else
        symmetry = NOSYM;

      if ((trigndx[0] == LOG) || (trigndx[0] == 14)) /* LOG or FLIP */
        symmetry = NOSYM;
    break;

    case QUATFP:
    case HYPERCMPLXFP:
      floatparm = &tmp;
      attractors = 0;
      periodicitycheck = 0;
    break;

    default:
    break;
    }
  return(1);
}

int MandellongSetup()
{
  FgHalf = fudge >> 1;
  c_exp = param[2];

  if(fractype==MARKSMANDEL && c_exp < 1)
    c_exp = 1;

  if(fractype==LMANDELZPOWER && c_exp < 1)
    c_exp = 1;

  if((fractype==MARKSMANDEL   && !(c_exp & 1)) ||
     (fractype==LMANDELZPOWER && c_exp & 1))
    symmetry = XYAXIS_NOPARM;    /* odd exponents */

  if((fractype==MARKSMANDEL && (c_exp & 1)) || fractype==LMANDELEXP)
    symmetry = XAXIS_NOPARM;

  if(fractype==SPIDER && periodicitycheck==1)
    periodicitycheck=4;

  longparm = &linit;

  if(fractype==LMANDELZPOWER)
    {
    if(param[3] == 0.0 && debugflag != 6000  && (double)c_exp == param[2])
      fractalspecific[fractype].orbitcalc = longZpowerFractal;
    else
      fractalspecific[fractype].orbitcalc = longCmplxZpowerFractal;

    if(param[3] != 0 || (double)c_exp != param[2] )
      symmetry = NOSYM;
    }

  /* Added to account for symmetry in manfn+exp and manfn+zsqrd */
  /*     JCO 2/29/92 */
  if((fractype==LMANTRIGPLUSEXP)||(fractype==LMANTRIGPLUSZSQRD))
    {
    if(parm.y == 0.0)
      symmetry = XAXIS;
    else
      symmetry = NOSYM;

    if ((trigndx[0] == LOG) || (trigndx[0] == 14)) /* LOG or FLIP */
      symmetry = NOSYM;
   }
  return(1);
}

int mandel_per_pixel()
{
  /* mandel */

  if(invert)
    {
    invertz2(&init);

    /* watch out for overflow */
    if(bitshift <= 24)
      {
      if (sqr(init.x)+sqr(init.y) >= 127)
        {
        init.x = 8;  /* value to bail out in one iteration */
        init.y = 8;
        }
      }

    if(bitshift >  24)
      {
      if (sqr(init.x)+sqr(init.y) >= 4)
        {
        init.x = 2;  /* value to bail out in one iteration */
        init.y = 2;
        }
      }

    /* convert to fudged longs */
    linit.x = init.x*fudge;
    linit.y = init.y*fudge;
    }
  else
    linit.x = lx0[col]+lx1[row];

  switch (fractype)
    {
    case MANDELLAMBDA:        /* Critical Value 0.5 + 0.0i  */
      lold.x = FgHalf;
      lold.y = 0;
    break;

    default:
      lold = linit;
    break;
    }

  /* alter init value */
  if(useinitorbit == 1)
    lold = linitorbit;
  else if(useinitorbit == 2)
    lold = linit;

  if(inside == -60 || inside == -61)
    {
    /* kludge to match "Beauty of Fractals" picture since we start
    Mandelbrot iteration with init rather than 0 */
    lold.x = lparm.x; /* initial pertubation of parameters set */
    lold.y = lparm.y;
    color = -1;
    }
  else
    {
    lold.x += lparm.x; /* initial pertubation of parameters set */
    lold.y += lparm.y;
    }

  ltmp = linit; /* for spider */
  ltempsqrx = multiply(lold.x, lold.x, bitshift);
  ltempsqry = multiply(lold.y, lold.y, bitshift);

  return(1); /* 1st iteration has been done */
}

int StandaloneSetup()
{
   /* effectively disable solid-guessing */
   timer(0,curfractalspecific->calctype);
   return(0);
}

int mandelfp_per_pixel()
{

  if(invert)
    invertz2(&init);
  else
    init.x = dx0[col]+dx1[row];

  switch (fractype)
    {
    case MAGNET2M:
      FloatPreCalcMagnet2();

    /*FALLTHRU*/
    case MAGNET1M:         /* Crit Val Zero both, but neither   */
      old.x = old.y = 0.0; /* is of the form f(Z,C) = Z*g(Z)+C  */
    break;

    case MANDELLAMBDAFP:        /* Critical Value 0.5 + 0.0i  */
      old.x = 0.5;
      old.y = 0.0;
    break;

    default:
      old = init;
    break;
    }

  /* alter init value */

  if(useinitorbit == 1)
    old = initorbit;
  else if(useinitorbit == 2)
    old = init;

  if(inside == -60 || inside == -61)
    {
    /*
     * kludge to match "Beauty of Fractals" picture since we start
     * Mandelbrot iteration with init rather than 0
     */
    old.x = parm.x; /* initial pertubation of parameters set */
    old.y = parm.y;
    color = -1;
    }
  else
    {
    old.x += parm.x;
    old.y += parm.y;
    }

  tmp = init; /* for spider */
  tempsqrx = sqr(old.x);  /* precalculated value for regular Mandelbrot */
  tempsqry = sqr(old.y);

  return(1); /* 1st iteration has been done */
}

/* Julia Routine */
int JuliaSetup()
{
  if (debugflag != 90 && ! invert && decomp[0] == 0 && rqlim <= 4.0
     && bitshift == 29 && potflag == 0
     && biomorph == -1 && inside > -59 && outside >= -1
     && !finattract && using_jiim == 0)
    {
    calctype = calcmand; /* the normal case - use CALCMAND */
    }
  else
    {
    /* special case: use the main processing loop */
    calctype = StandardFractal;
    longparm = &lparm;
    get_julia_attractor (0.0, 0.0);   /* another attractor? */
    }
  return(1);
}

int julia_per_pixel()
{
  /* julia */

  if(invert)
    {
    /* invert */
    invertz2(&old);

    /* watch out for overflow */
    if(bitshift <= 24)
      if (sqr(old.x)+sqr(old.y) >= 127)
      {
      old.x = 8;    /* value to bail out in one iteration */
      old.y = 8;
      }

    if(bitshift >  24)
      if (sqr(old.x)+sqr(old.y) >= 4.0)
      {
      old.x = 2;    /* value to bail out in one iteration */
      old.y = 2;
      }

    /* convert to fudged longs */
    lold.x = old.x*fudge;
    lold.y = old.y*fudge;
    }
  else
    {
    lold.x = lx0[col]+lx1[row];
    lold.y = ly0[row]+ly1[col];
    }

  ltempsqrx = multiply(lold.x, lold.x, bitshift);
  ltempsqry = multiply(lold.y, lold.y, bitshift);
  ltmp = lold;

  return(0);
}

int JuliafpFractal()
{
  /* floating point version of classical Mandelbrot/Julia */
  /* note that fast >= 287 equiv in fracsuba.asm must be kept in step */

  new.x = tempsqrx - tempsqry + floatparm->x;
  new.y = 2.0 * old.x * old.y + floatparm->y;

  return(floatbailout());
}

int JuliafpSetup()
{
  c_exp = param[2];
  floatparm = &parm;
  if(fractype==COMPLEXMARKSJUL)
    {
    pwr.x = param[2] - 1.0;
    pwr.y = param[3];
    Coefficient = ComplexPower(*floatparm, pwr);
    }

  switch (fractype)
    {
    case JULIAFP:
      /*
       * floating point code could probably be altered to handle many of
       * the situations that otherwise are using StandardFractal().
       * calcmandfp() can currently handle invert, any rqlim, potflag
       * zmag, epsilon cross, and all the current outside options
       *     Wes Loewer 11/03/91
       */

      if (debugflag != 90
        && !distest
        && decomp[0] == 0
        && biomorph == -1
        && (inside >= -1 || inside == -59 || inside == -100)
        /* uncomment this next line if more outside options are added */
        /* && outside >= -5 */
        && useinitorbit != 1
        && !finattract
        && using_jiim == 0)
        {
        calctype = calcmandfp; /* the normal case - use calcmandfp */
        calcmandfpasmstart();
        }
      else
       {
       /* special case: use the main processing loop */
       calctype = StandardFractal;
       get_julia_attractor (0.0, 0.0);   /* another attractor? */
       }
    break;

    case FPJULIAZPOWER:
      if((c_exp & 1) || param[3] != 0.0 || (double)c_exp != param[2] )
        symmetry = NOSYM;
      if(param[3] == 0.0 && debugflag != 6000 && (double)c_exp == param[2])
        fractalspecific[fractype].orbitcalc = floatZpowerFractal;
      else
        fractalspecific[fractype].orbitcalc = floatCmplxZpowerFractal;
      get_julia_attractor (param[0], param[1]);    /* another attractor? */
    break;

    case MAGNET2J:
      FloatPreCalcMagnet2();

    /*FALLTHRU*/
    case MAGNET1J:
      attr[0].x = 1.0;        /* 1.0 + 0.0i always attracts */
      attr[0].y = 0.0;        /* - both MAGNET1 and MAGNET2 */
      attrperiod[0] = 1;
      attractors = 1;
      get_julia_attractor (0.0, 0.0);    /* another attractor? */
    break;

    case LAMBDAFP:
      get_julia_attractor (0.0, 0.0);    /* another attractor? */
      get_julia_attractor (0.5, 0.0);    /* another attractor? */
    break;

    case LAMBDAEXP:
      if(parm.y == 0.0)
        symmetry=XAXIS;
      get_julia_attractor (0.0, 0.0);   /* another attractor? */
    break;

    /* Added to account for symmetry in julfn+exp and julfn+zsqrd */
    /*     JCO 2/29/92 */
    case FPJULTRIGPLUSEXP:
    case FPJULTRIGPLUSZSQRD:
      if(parm.y == 0.0)
        symmetry = XAXIS;
      else
        symmetry = NOSYM;
      if ((trigndx[0] == LOG) || (trigndx[0] == 14)) /* LOG or FLIP */
        symmetry = NOSYM;
      get_julia_attractor (0.0, 0.0);    /* another attractor? */
    break;

    case HYPERCMPLXJFP:
      if(trigndx[0] != SQR)
        symmetry=NOSYM;

    /*FALLTHRU*/
    case QUATJULFP:
      attractors = 0;    /* attractors broken since code checks r,i not j,k */
      periodicitycheck = 0;
      if(param[4] != 0.0 || param[5] != 0)
        symmetry = NOSYM;
      break;
    default:
      get_julia_attractor (0.0, 0.0);    /* another attractor? */
    break;
    }

  return(1);
}

int juliafp_per_pixel()
{
  /* floating point julia */
  /* juliafp */
  if(invert)
    invertz2(&old);
  else
    {
    old.x = dx0[col]+dx1[row];
    old.y = dy0[row]+dy1[col];
    }
  tempsqrx = sqr(old.x);  /* precalculated value for regular Julia */
  tempsqry = sqr(old.y);
  tmp = old;
  return(0);
}

int floatbailout()
{
  if ((magnitude = (tempsqrx=sqr(new.x)) + (tempsqry=sqr(new.y))) >= rqlim)
    return(1);
  old = new;
  return(0);
}

int otherjuliafp_per_pixel()
{
  if(invert)
    invertz2(&old);
  else
    {
    old.x = dx0[col]+dx1[row];
    old.y = dy0[row]+dy1[col];
    }
  return(0);
}

int marksmandelfp_per_pixel()
{
  /* marksmandel */

  if(invert)
    invertz2(&init);
  else
    init.x = dx0[col]+dx1[row];

  if(useinitorbit == 1)
    old = initorbit;
  else
    old = init;

  old.x += parm.x;  /* initial pertubation of parameters set */
  old.y += parm.y;

  tempsqrx = sqr(old.x);
  tempsqry = sqr(old.y);

  if(c_exp > 3)
    cpower(&old,c_exp-1,&coefficient);
  else if(c_exp == 3)
    {
    coefficient.x = tempsqrx - tempsqry;
    coefficient.y = old.x * old.y * 2;
    }
  else if(c_exp == 2)
    coefficient = old;
  else if(c_exp < 2)
    {
    coefficient.x = 1.0;
    coefficient.y = 0.0;
    }
  return(1); /* 1st iteration has been done */
}

int othermandelfp_per_pixel()
{
  if(invert)
    invertz2(&init);
  else
    init.x = dx0[col]+dx1[row];

  if(useinitorbit == 1)
    old = initorbit;
  else
    old = init;

  old.x += parm.x;  /* initial pertubation of parameters set */
  old.y += parm.y;

  return(1); /* 1st iteration has been done */
}

int marks_mandelpwrfp_per_pixel()
{
  mandelfp_per_pixel();
  tmp = old;
  tmp.x -= 1;
  CMPLXpwr(old,tmp,tmp);
  return(1);
}

int otherrichard8fp_per_pixel()
{
  othermandelfp_per_pixel();
  CMPLXtrig1(*floatparm,tmp);
  CMPLXmult(tmp,parm2,tmp);
  return(1);
}

int phoenix_per_pixel()
{
  if(invert)
    invertz2(&old);
  else
    {
    old.x = dx0[col]+dx1[row];
    old.y = dy0[row]+dy1[col];
    }
  tempsqrx = sqr(old.x);  /* precalculated value */
  tempsqry = sqr(old.y);
  tmp2.x = 0; /* use tmp2 as the complex Y value */
  tmp2.y = 0;
  return(0);
}

int quaternionfp_per_pixel()
{
  old.x = 0;
  old.y = 0;
  floatparm->x = 0;
  floatparm->y = 0;
  qc  = dx0[col]+dx1[row];
  qci = dy0[row]+dy1[col];
  qcj = param[2];
  qck = param[3];
  return(0);
}

int Halley_per_pixel()
{
  if(invert)
    invertz2(&init);
  else
    init.x = dx0[col]+dx1[row];

  old = init;

  return(0); /* 1st iteration is not done */
}

int mandphoenix_per_pixel()
{
  if(invert)
    invertz2(&init);
  else
    init.x = dx0[col]+dx1[row];

  if(useinitorbit == 1)
    old = initorbit;
  else
    old = init;

  old.x += parm.x;  /* initial pertubation of parameters set */
  old.y += parm.y;
  tempsqrx = sqr(old.x);  /* precalculated value */
  tempsqry = sqr(old.y);
  tmp2.x = 0;
  tmp2.y = 0;
  return(1); /* 1st iteration has been done */
}

int quaternionjulfp_per_pixel()
{
  old.x = dx0[col]+dx1[row];
  old.y = dy0[row]+dy1[col];
  floatparm->x = param[4];
  floatparm->y = param[5];
  qc  = param[0];
  qci = param[1];
  qcj = param[2];
  qck = param[3];
  return(0);
}

int JulialongSetup()
{
  c_exp = param[2];
  longparm = &lparm;
  switch (fractype)
    {
    case LJULIAZPOWER:
      if((c_exp & 1) || param[3] != 0.0 || (double)c_exp != param[2])
        symmetry = NOSYM;
      else if(c_exp < 1)
        c_exp = 1;

      if(param[3] == 0.0 && debugflag != 6000 && (double)c_exp == param[2])
        fractalspecific[fractype].orbitcalc = longZpowerFractal;
      else
        fractalspecific[fractype].orbitcalc = longCmplxZpowerFractal;
    break;

    case LAMBDA:
      get_julia_attractor (0.0, 0.0);    /* another attractor? */
      get_julia_attractor (0.5, 0.0);    /* another attractor? */
    break;

    case MANDELTRIG:
      if(lparm.y == 0)
        symmetry = XAXIS;
    break;

    /* Added to account for symmetry in julfn+exp and julfn+zsqrd */
    /*     JCO 2/29/92 */
    case LJULTRIGPLUSEXP:
    case LJULTRIGPLUSZSQRD:
      if(parm.y == 0.0)
        symmetry = XAXIS;
      else
        symmetry = NOSYM;
      if ((trigndx[0] == LOG) || (trigndx[0] == 14)) /* LOG or FLIP */
        symmetry = NOSYM;
      get_julia_attractor (0.0, 0.0);    /* another attractor? */
    break;

    default:
      get_julia_attractor (0.0, 0.0);    /* another attractor? */
    break;
    }
  return(1);
}

static int TrigPlusSqrlongSetup()
{
  curfractalspecific->per_pixel =  julia_per_pixel;
  curfractalspecific->orbitcalc =  TrigPlusSqrFractal;
  if(lparm.x == fudge && lparm.y == 0L && lparm2.y == 0L && debugflag != 90)
    {
    if(lparm2.x == fudge)       /* Scott variant */
      curfractalspecific->orbitcalc =  ScottTrigPlusSqrFractal;
    else if(lparm2.x == -fudge)  /* Skinner variant */
      curfractalspecific->orbitcalc =  SkinnerTrigSubSqrFractal;
    }
  return(JulialongSetup());
}

static int TrigPlusSqrfpSetup()
{
  curfractalspecific->per_pixel =  juliafp_per_pixel;
  curfractalspecific->orbitcalc =  TrigPlusSqrfpFractal;
  if(parm.x == 1.0 && parm.y == 0.0 && parm2.y == 0.0 && debugflag != 90)
    {
    if(parm2.x == 1.0)    /* Scott variant */
      curfractalspecific->orbitcalc =  ScottTrigPlusSqrfpFractal;
    else if(parm2.x == -1.0)    /* Skinner variant */
      curfractalspecific->orbitcalc =  SkinnerTrigSubSqrfpFractal;
    }
  return(JuliafpSetup());
}

int TrigPlusTriglongSetup()
{
  FnPlusFnSym();
  if(trigndx[1] == SQR)
    return(TrigPlusSqrlongSetup());
  curfractalspecific->per_pixel =  long_julia_per_pixel;
  curfractalspecific->orbitcalc =  TrigPlusTrigFractal;
  if(lparm.x == fudge && lparm.y == 0L && lparm2.y == 0L && debugflag != 90)
    {
    if(lparm2.x == fudge)       /* Scott variant */
      curfractalspecific->orbitcalc =  ScottTrigPlusTrigFractal;
    else if(lparm2.x == -fudge)  /* Skinner variant */
      curfractalspecific->orbitcalc =  SkinnerTrigSubTrigFractal;
    }
  return(JulialongSetup());
}

int TrigPlusTrigfpSetup()
{
  FnPlusFnSym();
  if(trigndx[1] == SQR)
    return(TrigPlusSqrfpSetup());
  curfractalspecific->per_pixel =  otherjuliafp_per_pixel;
  curfractalspecific->orbitcalc =  TrigPlusTrigfpFractal;
  if(parm.x == 1.0 && parm.y == 0.0 && parm2.y == 0.0 && debugflag != 90)
    {
    if(parm2.x == 1.0)    /* Scott variant */
      curfractalspecific->orbitcalc =  ScottTrigPlusTrigfpFractal;
    else if(parm2.x == -1.0)    /* Skinner variant */
      curfractalspecific->orbitcalc =  SkinnerTrigSubTrigfpFractal;
    }
  return(JuliafpSetup());
}

int LambdaTrigOrTrigSetup()
{
  /* default symmetry is ORIGIN  JCO 2/29/92 (changed from PI_SYM) */
  longparm = &lparm; /* added to consolidate code 10/1/92 JCO */
  floatparm = &parm;
  if ((trigndx[0] == EXP) || (trigndx[1] == EXP))
    symmetry = NOSYM; /* JCO 1/9/93 */
  if ((trigndx[0] == LOG) || (trigndx[1] == LOG))
    symmetry = XAXIS;
  get_julia_attractor (0.0, 0.0);    /* an attractor? */
  return(1);
}

int JuliaTrigOrTrigSetup()
{
  /* default symmetry is XAXIS */
  longparm = &lparm; /* added to consolidate code 10/1/92 JCO */
  floatparm = &parm;
  if(parm.y != 0.0)
    symmetry = NOSYM;
  get_julia_attractor (0.0, 0.0);    /* an attractor? */
  return(1);
}

int ManlamTrigOrTrigSetup()
{
  /* psuedo */
  /* default symmetry is XAXIS */
  longparm = &linit; /* added to consolidate code 10/1/92 JCO */
  floatparm = &init;
  if (trigndx[0] == SQR)
    symmetry = NOSYM;
  if ((trigndx[0] == LOG) || (trigndx[1] == LOG))
    symmetry = NOSYM;
  return(1);
}

int MandelTrigOrTrigSetup()
{
  /* default symmetry is XAXIS_NOPARM */
  longparm = &linit; /* added to consolidate code 10/1/92 JCO */
  floatparm = &init;
  if ((trigndx[0] == 14) || (trigndx[1] == 14)) /* FLIP  JCO 5/28/92 */
    symmetry = NOSYM;
  return(1);
}

int ZXTrigPlusZSetup()
{
   if(param[1] == 0.0 && param[3] == 0.0)
     switch(trigndx[0])
       {
       case COS:   /* changed to two case statments and made any added */
       case COSH:  /* functions default to XAXIS symmetry. JCO 5/25/92 */
       case SQR:
       case 9:   /* 'real' cos */
         symmetry = XYAXIS;
       break;
       case 14:   /* FLIP  JCO 2/29/92 */
         symmetry = YAXIS;
       break;
       case LOG:
         symmetry = NOSYM;
       break;
       default:
         symmetry = XAXIS;
       break;
       }
     else
       switch(trigndx[0])
         {
         case COS:
         case COSH:
         case SQR:
         case 9:   /* 'real' cos */
           symmetry = ORIGIN;
         break;
         case 14:   /* FLIP  JCO 2/29/92 */
           symmetry = NOSYM;
         break;
         default:
           symmetry = XAXIS;
         break;
         }
  if(curfractalspecific->isinteger)
    {
    curfractalspecific->orbitcalc =  ZXTrigPlusZFractal;
    if(lparm.x == fudge && lparm.y == 0L && lparm2.y == 0L && debugflag != 90)
      {
      if(lparm2.x == fudge)       /* Scott variant */
        curfractalspecific->orbitcalc =  ScottZXTrigPlusZFractal;
      else if(lparm2.x == -fudge)  /* Skinner variant */
        curfractalspecific->orbitcalc =  SkinnerZXTrigSubZFractal;
      }
    return(JulialongSetup());
    }
  else
    {
    curfractalspecific->orbitcalc =  ZXTrigPlusZfpFractal;
    if(parm.x == 1.0 && parm.y == 0.0 && parm2.y == 0.0 && debugflag != 90)
      {
      if(parm2.x == 1.0)    /* Scott variant */
        curfractalspecific->orbitcalc =  ScottZXTrigPlusZfpFractal;
      else if(parm2.x == -1.0)    /* Skinner variant */
        curfractalspecific->orbitcalc =  SkinnerZXTrigSubZfpFractal;
      }
    }
  return(JuliafpSetup());
}

int LambdaTrigSetup()
{
  int isinteger;
  if((isinteger = curfractalspecific->isinteger))
    curfractalspecific->orbitcalc =  LambdaTrigFractal;
  else
    curfractalspecific->orbitcalc =  LambdaTrigfpFractal;
  switch(trigndx[0])
    {
    case SIN:
    case COS:
    case 9:   /* 'real' cos, added this and default for additional functions */
      symmetry = PI_SYM;
      if(isinteger)
        curfractalspecific->orbitcalc =  LambdaTrigFractal1;
      else
        curfractalspecific->orbitcalc =  LambdaTrigfpFractal1;
    break;
    case SINH:
    case COSH:
      symmetry = ORIGIN;
      if(isinteger)
        curfractalspecific->orbitcalc =  LambdaTrigFractal2;
      else
        curfractalspecific->orbitcalc =  LambdaTrigfpFractal2;
    break;
    case SQR:
      symmetry = ORIGIN;
    break;
    case EXP:
      if(isinteger)
        curfractalspecific->orbitcalc =  LongLambdaexponentFractal;
      else
        curfractalspecific->orbitcalc =  LambdaexponentFractal;
      symmetry = NOSYM; /* JCO 1/9/93 */
    break;
    case LOG:
      symmetry = NOSYM;
    break;
    default:   /* default for additional functions */
      symmetry = ORIGIN;  /* JCO 5/8/92 */
    break;
    }
  get_julia_attractor (0.0, 0.0);    /* an attractor? */
  if(isinteger)
    return(JulialongSetup());
  else
    return(JuliafpSetup());
}

int JuliafnPlusZsqrdSetup()
{
  switch(trigndx[0]) /* fix sqr symmetry & add additional functions */
    {
    case COS: /* cosxx */
    case COSH:
    case SQR:
    case 9:   /* 'real' cos */
    case 10:  /* tan */
    case 11:  /* tanh */
      symmetry = ORIGIN;
    /* default is for NOSYM symmetry */
    }
  if(curfractalspecific->isinteger)
    return(JulialongSetup());
  else
    return(JuliafpSetup());
}

int SqrTrigSetup()
{
  switch(trigndx[0]) /* fix sqr symmetry & add additional functions */
    {
    case SIN:
    case COS: /* cosxx */
    case 9:   /* 'real' cos */
      symmetry = PI_SYM;
    /* default is for XAXIS symmetry */
    }
  if(curfractalspecific->isinteger)
    return(JulialongSetup());
  else
    return(JuliafpSetup());
}

int FnXFnSetup()
{
  static char fnxfn[7][7] =
    {/* fn2 ->sin     cos    sinh    cosh  exp    log    sqr */
     /* fn1 */
     /* sin */ {PI_SYM,YAXIS, XYAXIS,XYAXIS,XAXIS, NOSYM, XYAXIS},
     /* cos */ {YAXIS, PI_SYM,XYAXIS,XYAXIS,XAXIS, NOSYM, XYAXIS},
     /* sinh*/ {XYAXIS,XYAXIS,XYAXIS,XYAXIS,XAXIS, NOSYM, XYAXIS},
     /* cosh*/ {XYAXIS,XYAXIS,XYAXIS,XYAXIS,XAXIS, NOSYM, XYAXIS},
     /* exp */ {XAXIS, XAXIS, XAXIS, XAXIS, XAXIS, NOSYM, XYAXIS},
     /* log */ {NOSYM, NOSYM, NOSYM, NOSYM, NOSYM, XAXIS, NOSYM},
     /* sqr */ {XYAXIS,XYAXIS,XYAXIS,XYAXIS,XYAXIS,NOSYM, XYAXIS},
     };

  if(trigndx[0] < 7 && trigndx[1] < 7)  /* bounds of array JCO 5/22/92*/
    symmetry = fnxfn[trigndx[0]][trigndx[1]];  /* JCO 5/22/92 */
  /* defaults to XAXIS symmetry JCO 5/22/92 */
  else
    {  /* added to complete the symmetry JCO 5/22/92 */
    if (trigndx[0]==LOG || trigndx[1] ==LOG) symmetry = NOSYM;
    if (trigndx[0]==9 || trigndx[1] ==9)
      { /* 'real' cos */
      if (trigndx[0]==SIN || trigndx[1] ==SIN) symmetry = PI_SYM;
      if (trigndx[0]==COS || trigndx[1] ==COS) symmetry = PI_SYM;
      }
    if (trigndx[0]==9 && trigndx[1] ==9) symmetry = PI_SYM;
    }
  if(curfractalspecific->isinteger)
    return(JulialongSetup());
  else
    return(JuliafpSetup());
}

int MandelTrigSetup()
{
  int isinteger;
  if((isinteger = curfractalspecific->isinteger))
    curfractalspecific->orbitcalc =  LambdaTrigFractal;
  else
    curfractalspecific->orbitcalc =  LambdaTrigfpFractal;
  symmetry = XYAXIS_NOPARM;
  switch(trigndx[0])
    {
    case SIN:
    case COS:
      if(isinteger)
        curfractalspecific->orbitcalc =  LambdaTrigFractal1;
      else
        curfractalspecific->orbitcalc =  LambdaTrigfpFractal1;
    break;
    case SINH:
    case COSH:
      if(isinteger)
        curfractalspecific->orbitcalc =  LambdaTrigFractal2;
      else
        curfractalspecific->orbitcalc =  LambdaTrigfpFractal2;
    break;
    case EXP:
      symmetry = XAXIS_NOPARM;
      if(isinteger)
        curfractalspecific->orbitcalc =  LongLambdaexponentFractal;
      else
        curfractalspecific->orbitcalc =  LambdaexponentFractal;
    break;
    case LOG:
      symmetry = XAXIS_NOPARM;
    break;
    default:   /* added for additional functions, JCO 5/25/92 */
      symmetry = XYAXIS_NOPARM;
    break;
    }
  if(isinteger)
    return(MandellongSetup());
  else
    return(MandelfpSetup());
}

int MarksJuliaSetup()
{
  c_exp = param[2];
  longparm = &lparm;
  lold = *longparm;
  if(c_exp > 2)
    lcpower(&lold,c_exp,&lcoefficient,bitshift);
  else if(c_exp == 2)
    {
    lcoefficient.x = multiply(lold.x,lold.x,bitshift) -
                     multiply(lold.y,lold.y,bitshift);
    lcoefficient.y = multiply(lold.x,lold.y,bitshiftless1);
    }
  else if(c_exp < 2)
    lcoefficient = lold;
  get_julia_attractor (0.0, 0.0);    /* an attractor? */
  return(1);
}

int MarksJuliafpSetup()
{
  c_exp = param[2];
  floatparm = &parm;
  old = *floatparm;
  if(c_exp > 2)
    cpower(&old,c_exp,&coefficient);
  else if(c_exp == 2)
    {
    coefficient.x = sqr(old.x) - sqr(old.y);
    coefficient.y = old.x * old.y * 2;
    }
  else if(c_exp < 2)
    coefficient = old;
  get_julia_attractor (0.0, 0.0);    /* an attractor? */
  return(1);
}

int SierpinskiSetup()
{
  /* sierpinski */
  periodicitycheck = 0;        /* disable periodicity checks */
  ltmp.x = 1;
  ltmp.x = ltmp.x << bitshift; /* ltmp.x = 1 */
  ltmp.y = ltmp.x >> 1;            /* ltmp.y = .5 */
  return(1);
}

int SierpinskiFPSetup()
{
  /* sierpinski */
  periodicitycheck = 0;        /* disable periodicity checks */
  tmp.x = 1;
  tmp.y = 0.5;
  return(1);
}

int HalleySetup()
{
  /* Halley */
  periodicitycheck=0;

  if(usr_floatflag)
    fractype = HALLEY; /* float on */
  else
    fractype = MPHALLEY;

  curfractalspecific = &fractalspecific[fractype];

  degree = (int)parm.x;
  if(degree < 2)
    degree = 2;
  param[0] = (double)degree;

  /*  precalculated values */
  AplusOne = degree + 1; /* a+1 */
  Ap1deg = AplusOne * degree;

  if(degree % 2)
    symmetry = XAXIS;   /* odd */
  else
    symmetry = XYAXIS; /* even */
  return(1);
}

int PhoenixSetup()
{
  longparm = &lparm; /* added to consolidate code 10/1/92 JCO */
  floatparm = &parm;
  degree = (int)parm2.x;
  if(degree < 2 && degree > -3) degree = 0;
    param[2] = (double)degree;
  if(degree == 0)
    {
    if(usr_floatflag)
      curfractalspecific->orbitcalc =  PhoenixFractal;
    else
      curfractalspecific->orbitcalc =  LongPhoenixFractal;
    }
  if(degree >= 2)
    {
    degree = degree - 1;
    if(usr_floatflag)
      curfractalspecific->orbitcalc =  PhoenixPlusFractal;
    else
      curfractalspecific->orbitcalc =  LongPhoenixPlusFractal;
    }
  if(degree <= -3)
    {
    degree = abs(degree) - 2;
    if(degree % 2)
      symmetry = XYAXIS; /* odd */
    else
      symmetry = XAXIS; /* even */
    if(usr_floatflag)
      curfractalspecific->orbitcalc =  PhoenixMinusFractal;
    else
      curfractalspecific->orbitcalc =  LongPhoenixMinusFractal;
    }
  return(1);
}

int MandPhoenixSetup()
{
  longparm = &linit; /* added to consolidate code 10/1/92 JCO */
  floatparm = &init;
  degree = (int)parm2.x;
  if(degree < 2 && degree > -3) degree = 0;
    param[2] = (double)degree;
  if(degree == 0)
    {
    if(usr_floatflag)
      curfractalspecific->orbitcalc =  PhoenixFractal;
    else
      curfractalspecific->orbitcalc =  LongPhoenixFractal;
    }
  if(degree >= 2)
    {
    degree = degree - 1;
    if(usr_floatflag)
      curfractalspecific->orbitcalc =  PhoenixPlusFractal;
    else
      curfractalspecific->orbitcalc =  LongPhoenixPlusFractal;
    }
  if(degree <= -3)
    {
    degree = abs(degree) - 2;
    if(usr_floatflag)
      curfractalspecific->orbitcalc =  PhoenixMinusFractal;
    else
      curfractalspecific->orbitcalc =  LongPhoenixMinusFractal;
    }
  return(1);
}

int StandardSetup()
{
  if(fractype==UNITYFP)
    periodicitycheck=0;
  return(1);
}

int demowalk()
{
  float stepsize;            /* average stepsize */
  int xwalk, ywalk;            /* current position */
  int xstep, ystep;            /* current step */
  int steps;                /* number of steps */
  int color;                /* color to draw this step */
  float temp, tempadjust;        /* temporary variables */

  if (param[0] != 999)
    {            /* if 999, do a Mandelbrot instead */

    srand(rseed);            /* seed the random number generator */
    if (!rflag) ++rseed;
    tempadjust = RAND_MAX >> 2;        /* adjustment factor */

    xwalk = xdots / 2;            /* start in the center of the image */
    ywalk = ydots / 2;

    stepsize = min(xdots, ydots)     /* calculate average stepsize */
               * (param[0]/100.0);    /* as a percentage of the image */

    color = max(0, min(colors, param[1]));  /* set the initial color */

    for (steps = 0; steps < maxit; steps++)
      { /* take maxit steps */
      if (waitkeypressed(0))        /* abort if told to do so */
        return(0);
      temp = rand();            /* calculate the next xstep */
      xstep = ((temp/tempadjust) - 2.0) * stepsize;
      xstep = min(xwalk + xstep, xdots - 1);
      xstep = max(0, xstep);
      temp = rand();            /* calculate the next ystep */
      ystep = ((temp/tempadjust) - 2.0) * stepsize;
      ystep = min(ywalk + ystep, ydots - 1);
      ystep = max(0, ystep);
      if (param[1] == 0.0)        /* rotate the colors? */
        if (++color >= colors)    /* rotate the colors, avoiding */
          color = 1;        /* the background color 0 */
      /* the draw_line function is borrowed from the 3D routines */
      draw_line(xwalk, ywalk,xstep,ystep,color);
      /* or, we could be on a pogo stick and just displaying
         where we landed...
      putcolor(xstep, ystep, color);
       */

      xwalk = xstep;            /* remember where we were */
      ywalk = ystep;
      }
    return(1);                          /* we're done */

    }
  else
    { /* a simple Mandelbrot routine */
    /* the following routine determines the X and Y values of
       each pixel coordinate and calculates a simple mandelbrot
       fractal with them - slowly, but surely */
    int ix, iy;
    for (iy = 0; iy < ydots; iy++)
      {
      for (ix = 0; ix < xdots; ix++)
        {
        int iter;
        double x, y, newx, newy, tempxx, tempxy, tempyy;
        /* first, obtain the X and Y coordinate values of this pixel */
        x = dx0[ix]+dx1[iy];
        y = dy0[iy]+dy1[ix];
        /* now initialize the temporary values */
        tempxx = tempyy = tempxy = 0.0;
        if (waitkeypressed(0))        /* abort if told to do so */
          return(0);
        /* the inner iteration loop */
        for (iter = 1; iter < maxit; iter++)
          {
          /* calculate the X and Y values of Z(iter) */
          newx = tempxx - tempyy + x;
          newy = tempxy + tempxy + y;
          /* calculate the temporary values */
          tempxx = newx * newx;
          tempyy = newy * newy;
          tempxy = newx * newy;
          /* are we done yet? */
          if (tempxx + tempyy > 4.0) break;
          }
        /* color in the pixel */
        putcolor(ix, iy, iter & (colors - 1));
        }
      }
    return(1);                          /* we're done */
    }
}

static int TrigPlusSqrfpFractal()
{
  /* { z=pixel: z=(p1,p2)*trig(z)+(p3,p4)*sqr(z), |z|<BAILOUT } */

  CMPLXtrig0(old,tmp);       /* tmp = trig(old)                     */
  CMPLXmult(parm,tmp,new);   /* new = parm*trig(old)                */
  CMPLXsqr_old(tmp);         /* tmp = sqr(old)                      */
  CMPLXmult(parm2,tmp,tmp2); /* tmp = parm2*sqr(old)                */
  CMPLXadd(new,tmp2,new);    /* new = parm*trig(old)+parm2*sqr(old) */

  return(floatbailout());
}

static int ScottTrigPlusSqrfpFractal() /* float version */
{
  /* { z=pixel: z=sin(z)+sqr(z), |z|<BAILOUT } */

  CMPLXtrig0(old,new);    /* new = trig(old)          */
  CMPLXsqr_old(tmp);      /* tmp = sqr(old)           */
  CMPLXadd(new,tmp,new);  /* new = trig(old)+sqr(old) */

  return(floatbailout());
}

static int SkinnerTrigSubSqrfpFractal()
{
  /* { z=pixel: z=sin(z)-sqr(z), |z|<BAILOUT } */

  CMPLXtrig0(old,new);       /* new = trig(old)          */
  CMPLXsqr_old(tmp);         /* old = sqr(old)           */
  CMPLXsub(new,tmp,new);     /* new = trig(old)-sqr(old) */

  return(floatbailout());
}

int TrigPlusTrigfpFractal()
{
  /* z = trig0(z)*p1+trig1(z)*p2 */

  CMPLXtrig0(old,tmp);
  CMPLXmult(parm,tmp,tmp);
  CMPLXtrig1(old,old);
  CMPLXmult(parm2,old,old);
  CMPLXadd(tmp,old,new);

  return(floatbailout());
}

static int ScottTrigPlusTrigfpFractal()
{
  /* z = trig0(z)+trig1(z) */

  CMPLXtrig0(old,tmp);
  CMPLXtrig1(old,tmp2);
  CMPLXadd(tmp,tmp2,new);

  return(floatbailout());
}

static int SkinnerTrigSubTrigfpFractal()
{
  /* z = trig0(z)-trig1(z) */

  CMPLXtrig0(old,tmp);
  CMPLXtrig1(old,tmp2);
  CMPLXsub(tmp,tmp2,new);

  return(floatbailout());
}

int ZXTrigPlusZfpFractal()
{
  /* z = (p1*z*trig(z))+p2*z */

  CMPLXtrig0(old,tmp);      /* tmp  = trig(old)             */
  CMPLXmult(parm,tmp,tmp);  /* tmp  = p1*trig(old)          */
  CMPLXmult(old,tmp,tmp2);  /* tmp2 = p1*old*trig(old)      */
  CMPLXmult(parm2,old,tmp); /* tmp  = p2*old                */
  CMPLXadd(tmp2,tmp,new);   /* new  = p1*trig(old) + p2*old */

  return(floatbailout());
}

static int ScottZXTrigPlusZfpFractal()
{
  /* z = (z*trig(z))+z */

  CMPLXtrig0(old,tmp);      /* tmp  = trig(old)       */
  CMPLXmult(old,tmp,new);   /* new  = old*trig(old)   */
  CMPLXadd(new,old,new);    /* new  = trig(old) + old */

  return(floatbailout());
}

static int SkinnerZXTrigSubZfpFractal()
{
  /* z = (z*trig(z))-z */

  CMPLXtrig0(old,tmp);     /* tmp  = trig(old)        */
  CMPLXmult(old,tmp,new);   /* new  = old*trig(old)   */
  CMPLXsub(new,old,new);    /* new  = trig(old) - old */

  return(floatbailout());
}

int LambdaTrigfpFractal()
{
  FLOATXYTRIGBAILOUT();
  CMPLXtrig0(old,tmp);             /* tmp = trig(old)           */
  CMPLXmult(*floatparm,tmp,new);   /* new = longparm*trig(old)  */
  old = new;

  return(0);
}

static int LambdaTrigfpFractal1()
{
  FLOATTRIGBAILOUT();              /* sin,cos                   */
  CMPLXtrig0(old,tmp);             /* tmp = trig(old)           */
  CMPLXmult(*floatparm,tmp,new);   /* new = longparm*trig(old)  */
  old = new;

  return(0);
}

int LambdaexponentFractal()
{
  /* found this in  "Science of Fractal Images" */
  FLOATEXPBAILOUT();
  FPUsincos  (&old.y,&siny,&cosy);

  if (old.x >= rqlim && cosy >= 0.0) return(1);
  tmpexp = exp(old.x);
  tmp.x = tmpexp*cosy;
  tmp.y = tmpexp*siny;

  /*multiply by lamda */
  new.x = floatparm->x*tmp.x - floatparm->y*tmp.y;
  new.y = floatparm->y*tmp.x + floatparm->x*tmp.y;
  old = new;

  return(0);
}

int PhoenixFractal()
{
  /* z(n+1) = z(n)^2 + p + qy(n),  y(n+1) = z(n) */
  tmp.x = old.x * old.y;
  new.x = tempsqrx - tempsqry + floatparm->x + (floatparm->y * tmp2.x);
  new.y = (tmp.x + tmp.x) + (floatparm->y * tmp2.y);
  tmp2 = old; /* set tmp2 to Y value */

  return(floatbailout());
}

int PhoenixPlusFractal()
{
  /* z(n+1) = z(n)^(degree-1) * (z(n) + p) + qy(n),  y(n+1) = z(n) */
  int i;
  _CMPLX oldplus, newminus;

  oldplus = old;
  tmp = old;
  for(i = 1; i < degree; i++)
    {
    /* degree >= 2, degree=degree-1 in setup */
     FPUcplxmul(&old, &tmp, &tmp); /* = old^(degree-1) */
    }

  oldplus.x += floatparm->x;
  FPUcplxmul(&tmp, &oldplus, &newminus);
  new.x = newminus.x + (floatparm->y * tmp2.x);
  new.y = newminus.y + (floatparm->y * tmp2.y);
  tmp2 = old; /* set tmp2 to Y value */

  return(floatbailout());
}

int PhoenixMinusFractal()
{
  /* z(n+1) = z(n)^(degree-2) * (z(n)^2 + p) + qy(n),  y(n+1) = z(n) */
  int i;
  _CMPLX oldsqr, newminus;

  FPUcplxmul(&old, &old, &oldsqr);
  tmp = old;
  for(i = 1; i < degree; i++)
    {
    /* degree >= 3, degree=degree-2 in setup */
    FPUcplxmul(&old, &tmp, &tmp); /* = old^(degree-2) */
    }

  oldsqr.x += floatparm->x;
  FPUcplxmul(&tmp, &oldsqr, &newminus);
  new.x = newminus.x + (floatparm->y * tmp2.x);
  new.y = newminus.y + (floatparm->y * tmp2.y);
  tmp2 = old; /* set tmp2 to Y value */

  return(floatbailout());
}

void cpower(_CMPLX *base, int exp, _CMPLX *result)
{
  if (exp<0)
    {
    cpower(base,-exp,result);
    CMPLXrecip(*result,*result);
    return;
    }

  xmxt = base->x;
  xmyt = base->y;

  if (exp & 1)
    {
    result->x = xmxt;
    result->y = xmyt;
    }
  else
    {
    result->x = 1.0;
    result->y = 0.0;
    }

  exp >>= 1;
  while (exp)
    {
    t2 = xmxt * xmxt - xmyt * xmyt;
    xmyt = 2 * xmxt * xmyt;
    xmxt = t2;

    if (exp & 1)
      {
      t2 = xmxt * result->x - xmyt * result->y;
      result->y = result->y * xmxt + xmyt * result->x;
      result->x = t2;
      }
    exp >>= 1;
    }
}

static int lcpower(_LCMPLX *base, int exp, _LCMPLX *result, int bitshift)
{
  overflow = 0;
  lxt = base->x;   lyt = base->y;

  if (exp & 1)
    {
    result->x = lxt;
    result->y = lyt;
    }
  else
    {
    result->x = 1L<<bitshift;
    result->y = 0L;
    }

  exp >>= 1;
  while (exp)
    {
    lt2 = multiply(lxt, lxt, bitshift) - multiply(lyt,lyt,bitshift);
    lyt = multiply(lxt,lyt,bitshiftless1);
    if(overflow)
      {
      return(overflow);
      }
    lxt = lt2;

    if (exp & 1)
      {
      lt2 = multiply(lxt,result->x, bitshift) -
            multiply(lyt,result->y,bitshift);

      result->y = multiply(result->y,lxt,bitshift) +
                  multiply(lyt,result->x,bitshift);

      result->x = lt2;
      }

    exp >>= 1;
    }

  if(result->x == 0 && result->y == 0)
    {
    overflow = 1;
    }

  return(overflow);
}

int UnityfpFractal()
{
  /* brought to you by Mark Peterson - you won't find this in any fractal
     books unless they saw it here first - Mark invented it! */

  new.x = sqr(old.x) + sqr(old.y);
  if((new.x > 2.0) || (fabs(new.x - 1.0) < ddelmin))
    return(1);
  old.y = (2.0 - new.x)* old.x;
  old.x = (2.0 - new.x)* old.y;
  new=old;  /* TW added this line */
  return(0);
}

int complex_mult(arg1,arg2,pz)
_CMPLX arg1,arg2,*pz;
{
  pz->x = arg1.x*arg2.x - arg1.y*arg2.y;
  pz->y = arg1.x*arg2.y+arg1.y*arg2.x;

  return(0);
}

int LambdaFPFractal()
{
  /* variation of classical Mandelbrot/Julia */
  /* note that fast >= 287 equiv in fracsuba.asm must be kept in step */

  tempsqrx = old.x - tempsqrx + tempsqry;
  tempsqry = -(old.y * old.x);
  tempsqry += tempsqry + old.y;

  new.x = floatparm->x * tempsqrx - floatparm->y * tempsqry;
  new.y = floatparm->x * tempsqry + floatparm->y * tempsqrx;

  return(floatbailout());
}

int ManOWarfpFractal()
{
  /* From Art Matrix via Lee Skinner */
  /* note that fast >= 287 equiv in fracsuba.asm must be kept in step */
  new.x = tempsqrx - tempsqry + tmp.x + floatparm->x;
  new.y = 2.0 * old.x * old.y + tmp.y + floatparm->y;
  tmp = old;
  return(floatbailout());
}

int test()
{
  int passes,startrow,startpass,numpasses;
  startrow = startpass = 0;
  if (resuming)
    {
    start_resume();
    get_resume(sizeof(int),&startrow,sizeof(int),&startpass,0);
    end_resume();
    }
  if(teststart()) /* assume it was stand-alone, doesn't want passes logic */
    return(0);
  numpasses = (stdcalcmode == '1') ? 0 : 1;
  for (passes=startpass; passes <= numpasses ; passes++)
    {
    for (row = startrow; row <= iystop; row=row+1+numpasses)
      {
      register int col;
      for (col = 0; col <= ixstop; col++)  /* look at each point on screen */
        {
        register color;
        init.y = dy0[row]+dy1[col];
        init.x = dx0[col]+dx1[row];
        if(check_key() != 0)
          {
          testend();
          alloc_resume(20,1);
          put_resume(sizeof(int),&row,sizeof(int),&passes,0);
          return(-1);
          }
        color = testpt(init.x,init.y,parm.x,parm.y,maxit,inside);
        if (color >= colors) /* avoid trouble if color is 0 */
          if (colors < 16)
            color &= andcolor;
          else
            color = ((color-1) % andcolor) + 1; /* skip color zero */
        (*plot)(col,row,color);
        if(numpasses && (passes == 0))
           (*plot)(col,row+1,color);
        }
      }
    startrow = passes + 1;
    }
  testend();
  return(0);
}

int SqrTrigfpFractal()
{
  /* SZSB(XYAXIS) { z=pixel, TEST=(p1+3): z=sin(z)*sin(z), |z|<TEST} */
  CMPLXtrig0(old,tmp);
  CMPLXsqr(tmp,new);
  return(floatbailout());
}

int MarksLambdaFractal()
{
  /* Mark Peterson's variation of "lambda" function */

  /* Z1 = (C^(exp-1) * Z**2) + C */
  ltmp.x = ltempsqrx - ltempsqry;
  ltmp.y = multiply(lold.x ,lold.y ,bitshiftless1);

  lnew.x = multiply(lcoefficient.x, ltmp.x, bitshift)
    - multiply(lcoefficient.y, ltmp.y, bitshift) + longparm->x;
  lnew.y = multiply(lcoefficient.x, ltmp.y, bitshift)
    + multiply(lcoefficient.y, ltmp.x, bitshift) + longparm->y;

  return(longbailout());
}

int marksmandel_per_pixel()
{
  /* marksmandel */

  if(invert)
    {
    invertz2(&init);

    /* watch out for overflow */
    if(sqr(init.x)+sqr(init.y) >= 127)
      {
      init.x = 8;  /* value to bail out in one iteration */
      init.y = 8;
      }

    /* convert to fudged longs */
    linit.x = init.x*fudge;
    linit.y = init.y*fudge;
    }
  else
    linit.x = lx0[col]+lx1[row];

  if(useinitorbit == 1)
    lold = linitorbit;
  else
    lold = linit;

  lold.x += lparm.x;    /* initial pertubation of parameters set */
  lold.y += lparm.y;

  if(c_exp > 3)
    lcpower(&lold,c_exp-1,&lcoefficient,bitshift);
  else if(c_exp == 3)
    {
    lcoefficient.x = multiply(lold.x, lold.x, bitshift)
     - multiply(lold.y, lold.y, bitshift);
    lcoefficient.y = multiply(lold.x, lold.y, bitshiftless1);
    }
  else if(c_exp == 2)
    lcoefficient = lold;
  else if(c_exp < 2)
    {
    lcoefficient.x = 1L << bitshift;
    lcoefficient.y = 0L;
    }

  ltempsqrx = multiply(lold.x, lold.x, bitshift);
  ltempsqry = multiply(lold.y, lold.y, bitshift);
  return(1); /* 1st iteration has been done */
}

int UnitySetup()
{
  periodicitycheck = 0;
  return(1);
}

int Mandel4Fractal()
{
  /*
   * By writing this code, Bert has left behind the excuse "don't
   * know what a fractal is, just know how to make'em go fast".
   * Bert is hereby declared a bonafide fractal expert! Supposedly
   * this routine calculates the Mandelbrot/Julia set based on the
   * polynomial z**4 + lambda, but I wouldn't know -- can't follow
   * all that integer math speedup stuff - Tim
   */

  /* first, compute (x + iy)**2 */
  lnew.x  = ltempsqrx - ltempsqry;
  lnew.y = multiply(lold.x, lold.y, bitshiftless1);
  if (longbailout()) return(1);

  /* then, compute ((x + iy)**2)**2 + lambda */
  lnew.x  = ltempsqrx - ltempsqry + longparm->x;
  lnew.y = multiply(lold.x, lold.y, bitshiftless1) + longparm->y;
  return(longbailout());
}

int Mandel4fpFractal()
{
  /* first, compute (x + iy)**2 */
  new.x  = tempsqrx - tempsqry;
  new.y = old.x*old.y*2;
  if (floatbailout()) return(1);

  /* then, compute ((x + iy)**2)**2 + lambda */
  new.x  = tempsqrx - tempsqry + floatparm->x;
  new.y =  old.x*old.y*2 + floatparm->y;
  return(floatbailout());
}

int Barnsley3Fractal()
{
  /* An unnamed Mandelbrot/Julia function from "Fractals
  Everywhere" by Michael Barnsley, p. 292, example 4.1 */

  /* calculate intermediate products */
  oldxinitx   = multiply(lold.x, lold.x, bitshift);
  oldyinity   = multiply(lold.y, lold.y, bitshift);
  oldxinity   = multiply(lold.x, lold.y, bitshift);

  /* orbit calculation */
  if(lold.x > 0)
    {
    lnew.x = oldxinitx   - oldyinity - fudge;
    lnew.y = oldxinity << 1;
    }
  else
    {
    lnew.x = oldxinitx - oldyinity - fudge
       + multiply(longparm->x,lold.x,bitshift);
    lnew.y = oldxinity <<1;

    /*
     * This term added by Tim Wegner to make dependent on the
     * imaginary part of the parameter. (Otherwise Mandelbrot
     * is uninteresting.
     */
    lnew.y += multiply(longparm->y,lold.x,bitshift);
    }
  return(longbailout());
}

int Barnsley3FPFractal()
{
  /*
   * An unnamed Mandelbrot/Julia function from "Fractals
   * Everywhere" by Michael Barnsley, p. 292, example 4.1
   */

  /* calculate intermediate products */
  foldxinitx  = old.x * old.x;
  foldyinity  = old.y * old.y;
  foldxinity  = old.x * old.y;

  /* orbit calculation */
  if(old.x > 0)
    {
    new.x = foldxinitx - foldyinity - 1.0;
    new.y = foldxinity * 2;
    }
  else
    {
    new.x = foldxinitx - foldyinity -1.0 + floatparm->x * old.x;
    new.y = foldxinity * 2;

    /* This term added by Tim Wegner to make dependent on the
     * imaginary part of the parameter. (Otherwise Mandelbrot
     * is uninteresting.
     */
    new.y += floatparm->y * old.x;
    }
  return(floatbailout());
}

int TrigZsqrdfpFractal()
{
  /* { z=pixel: z=trig(z*z), |z|<TEST } */
  CMPLXsqr_old(tmp);
  CMPLXtrig0(tmp,new);
  return(floatbailout());
}

int TrigXTrigfpFractal()
{
  /* z = trig0(z)*trig1(z) */
  CMPLXtrig0(old,tmp);
  CMPLXtrig1(old,old);
  CMPLXmult(tmp,old,new);
  return(floatbailout());
}

int Sqr1overTrigfpFractal()
{
  /* z = sqr(1/trig(z)) */
  CMPLXtrig0(old,old);
  CMPLXrecip(old,old);
  CMPLXsqr(old,new);
  return(floatbailout());
}

int TrigPlusZsquaredfpFractal()
{
  /* From Scientific American, July 1989 */
  /* A Biomorph              */
  /* z(n+1) = trig(z(n))+z(n)**2+C   */

  CMPLXtrig0(old,new);
  new.x += tempsqrx - tempsqry + floatparm->x;
  new.y += 2.0 * old.x * old.y + floatparm->y;
  return(floatbailout());
}

int floatZtozPluszpwrFractal()
{
  cpower(&old,(int)param[2],&new);
  old = ComplexPower(old,old);
  new.x = new.x + old.x +floatparm->x;
  new.y = new.y + old.y +floatparm->y;
  return(floatbailout());
}

int FloatTrigPlusExponentFractal()
{
  /* another Scientific American biomorph type */
  /* z(n+1) = e**z(n) + trig(z(n)) + C */

  if (fabs(old.x) >= 6.4e2)
    return(1); /* DOMAIN errors */
  tmpexp = exp(old.x);
  FPUsincos  (&old.y,&siny,&cosy);
  CMPLXtrig0(old,new);

  /*new =   trig(old) + e**old + C  */
  new.x += tmpexp*cosy + floatparm->x;
  new.y += tmpexp*siny + floatparm->y;
  return(floatbailout());
}

int PopcornFractal()
{
  tmp = old;
  tmp.x *= 3.0;
  tmp.y *= 3.0;
  FPUsincos(&tmp.x,&sinx,&cosx);
  FPUsincos(&tmp.y,&siny,&cosy);
  tmp.x = sinx/cosx + old.x;
  tmp.y = siny/cosy + old.y;
  FPUsincos(&tmp.x,&sinx,&cosx);
  FPUsincos(&tmp.y,&siny,&cosy);
  new.x = old.x - parm.x*siny;
  new.y = old.y - parm.x*sinx;
  if(plot == noplot)
    {
    plot_orbit(new.x,new.y,1+row%colors);
    old = new;
    }
  else
    /* FLOATBAILOUT(); */
    /* PB The above line was weird, not what it seems to be!  But, bracketing
     * it or always doing it (either of which seem more likely to be what
     * was intended) changes the image for the worse, so I'm not touching it.
     * Same applies to int form in next routine.
     */

    /* PB later: recoded inline, still leaving it weird */

  tempsqrx = sqr(new.x);
  tempsqry = sqr(new.y);
  if((magnitude = tempsqrx + tempsqry) >= rqlim) return(1);
    old = new;
  return(0);
}

int MarksCplxMand(void)
{
  tmp.x = tempsqrx - tempsqry;
  tmp.y = 2*old.x*old.y;
  FPUcplxmul(&tmp, &Coefficient, &new);
  new.x += floatparm->x;
  new.y += floatparm->y;
  return(floatbailout());
}

int MarksCplxMandperp(void)
{
  if(invert)
    invertz2(&init);
  else
    init.x = dx0[col]+dx1[row];
  old.x = init.x + parm.x; /* initial pertubation of parameters set */
  old.y = init.y + parm.y;
  tempsqrx = sqr(old.x);  /* precalculated value */
  tempsqry = sqr(old.y);
  Coefficient = ComplexPower(init, pwr);
  return(1);
}

int SierpinskiFPFractal()
{
  /*
   * Following code translated from basic - see "Fractals
   * Everywhere" by Michael Barnsley, p. 251, Program 7.1.1
   */

  new.x = old.x + old.x;
  new.y = old.y + old.y;
  if(old.y > .5)
    new.y = new.y - 1;
  else if (old.x > .5)
    new.x = new.x - 1;

  /* end barnsley code */
  return(floatbailout());
}

int Barnsley1FPFractal()
{
  /*
   * Barnsley's Mandelbrot type M1 from "Fractals
   * Everywhere" by Michael Barnsley, p. 322
   */

  /* calculate intermediate products */
  foldxinitx = old.x * floatparm->x;
  foldyinity = old.y * floatparm->y;
  foldxinity = old.x * floatparm->y;
  foldyinitx = old.y * floatparm->x;
  /* orbit calculation */
  if(old.x >= 0)
    {
    new.x = (foldxinitx - floatparm->x - foldyinity);
    new.y = (foldyinitx - floatparm->y + foldxinity);
    }
  else
    {
    new.x = (foldxinitx + floatparm->x - foldyinity);
    new.y = (foldyinitx + floatparm->y + foldxinity);
    }
  return(floatbailout());
}

int Barnsley2FPFractal()
{
  /*
   * An unnamed Mandelbrot/Julia function from "Fractals
   * Everywhere" by Michael Barnsley, p. 331, example 4.2
   */

  /* calculate intermediate products */
  foldxinitx = old.x * floatparm->x;
  foldyinity = old.y * floatparm->y;
  foldxinity = old.x * floatparm->y;
  foldyinitx = old.y * floatparm->x;

  /* orbit calculation */
  if(foldxinity + foldyinitx >= 0)
    {
    new.x = foldxinitx - floatparm->x - foldyinity;
    new.y = foldyinitx - floatparm->y + foldxinity;
    }
  else
    {
    new.x = foldxinitx + floatparm->x - foldyinity;
    new.y = foldyinitx + floatparm->y + foldxinity;
    }
  return(floatbailout());
}

int SpiderfpFractal(void)
{
  /* Spider(XAXIS) { c=z=pixel: z=z*z+c; c=c/2+z, |z|<=4 } */
  new.x = tempsqrx - tempsqry + tmp.x;
  new.y = 2 * old.x * old.y + tmp.y;
  tmp.x = tmp.x/2 + new.x;
  tmp.y = tmp.y/2 + new.y;
  return(floatbailout());
}

int TetratefpFractal()
{
  /* Tetrate(XAXIS) { c=z=pixel: z=c^z, |z|<=(P1+3) } */
  new = ComplexPower(*floatparm,old);
  return(floatbailout());
}

int Magnet1Fractal()    /*    Z = ((Z**2 + C - 1)/(2Z + C - 2))**2    */
{           /*  In "Beauty of Fractals", code by Kev Allen. */
  _CMPLX top, bot, tmp;
  double div;

  top.x = tempsqrx - tempsqry + floatparm->x - 1; /* top = Z**2+C-1 */
  top.y = old.x * old.y;
  top.y = top.y + top.y + floatparm->y;

  bot.x = old.x + old.x + floatparm->x - 2;       /* bot = 2*Z+C-2  */
  bot.y = old.y + old.y + floatparm->y;

  div = bot.x*bot.x + bot.y*bot.y;            /* tmp = top/bot  */
  if (div < FLT_MIN) return(1);
  tmp.x = (top.x*bot.x + top.y*bot.y)/div;
  tmp.y = (top.y*bot.x - top.x*bot.y)/div;

  new.x = (tmp.x + tmp.y) * (tmp.x - tmp.y);      /* Z = tmp**2     */
  new.y = tmp.x * tmp.y;
  new.y += new.y;

  return(floatbailout());
}

int Magnet2Fractal()  /* Z = ((Z**3 + 3(C-1)Z + (C-1)(C-2)  ) /  */
            /*       (3Z**2 + 3(C-2)Z + (C-1)(C-2)+1) )**2  */
{         /*   In "Beauty of Fractals", code by Kev Allen.  */
  _CMPLX top, bot, tmp;
  double div;

  top.x = old.x * (tempsqrx-tempsqry-tempsqry-tempsqry + T_Cm1.x)
      - old.y * T_Cm1.y + T_Cm1Cm2.x;
  top.y = old.y * (tempsqrx+tempsqrx+tempsqrx-tempsqry + T_Cm1.x)
      + old.x * T_Cm1.y + T_Cm1Cm2.y;

  bot.x = tempsqrx - tempsqry;
  bot.x = bot.x + bot.x + bot.x
      + old.x * T_Cm2.x - old.y * T_Cm2.y
      + T_Cm1Cm2.x + 1.0;
  bot.y = old.x * old.y;
  bot.y += bot.y;
  bot.y = bot.y + bot.y + bot.y
      + old.x * T_Cm2.y + old.y * T_Cm2.x
      + T_Cm1Cm2.y;

  div = bot.x*bot.x + bot.y*bot.y;            /* tmp = top/bot  */
  if (div < FLT_MIN) return(1);
  tmp.x = (top.x*bot.x + top.y*bot.y)/div;
  tmp.y = (top.y*bot.x - top.x*bot.y)/div;

  new.x = (tmp.x + tmp.y) * (tmp.x - tmp.y);      /* Z = tmp**2     */
  new.y = tmp.x * tmp.y;
  new.y += new.y;

  return(floatbailout());
}

int Richard8fpFractal()
{
  /*  Richard8 {c = z = pixel: z=sin(z)+sin(pixel),|z|<=50} */
  CMPLXtrig0(old,new);
  new.x += tmp.x;
  new.y += tmp.y;
  return(floatbailout());
}

int MarksMandelPwrfpFractal()
{
  CMPLXtrig0(old,new);
  CMPLXmult(tmp,new,new);
  new.x += floatparm->x;
  new.y += floatparm->y;
  return(floatbailout());
}

int TimsErrorfpFractal()
{
  CMPLXtrig0(old,new);
  new.x = new.x * tmp.x - new.y * tmp.y;
  new.y = new.x * tmp.y - new.y * tmp.x;
  new.x += floatparm->x;
  new.y += floatparm->y;
  return(floatbailout());
}

int CirclefpFractal()
{
  int i;
  i = param[0]*(tempsqrx+tempsqry);
  color = i&(colors-1);
  return(1);
}

int LambdaTrigOrTrigfpFractal()
{
  /* z = trig0(z)*p1 if mod(old) < p2.x and
         trig1(z)*p1 if mod(old) >= p2.x */
  if (CMPLXmod(old) < parm2.x)
    {
    CMPLXtrig0(old,old);
    FPUcplxmul(floatparm,&old,&new);
    }
  else
    {
    CMPLXtrig1(old,old);
    FPUcplxmul(floatparm,&old,&new);
    }
  return(floatbailout());
}

int JuliaTrigOrTrigfpFractal()
{
   /* z = trig0(z)+p1 if mod(old) < p2.x and
          trig1(z)+p1 if mod(old) >= p2.x */
   if (CMPLXmod(old) < parm2.x){
     CMPLXtrig0(old,old);
     CMPLXadd(*floatparm,old,new);}
   else{
     CMPLXtrig1(old,old);
     CMPLXadd(*floatparm,old,new);}
   return(floatbailout());
}

int HalleyFractal()
{
  /*  X(X^a - 1) = 0, Halley Map */
  /*  a = parm.x = degree, relaxation coeff. = parm.y, epsilon = parm2.x  */

  int ihal;
  _CMPLX XtoAlessOne, XtoA, XtoAplusOne; /* a-1, a, a+1 */
  _CMPLX FX, F1prime, F2prime, Halnumer1, Halnumer2;

  XtoAlessOne = old;
  for(ihal=2; ihal<degree; ihal++)
    {
    FPUcplxmul(&old, &XtoAlessOne, &XtoAlessOne);
    }
  FPUcplxmul(&old, &XtoAlessOne, &XtoA);
  FPUcplxmul(&old, &XtoA, &XtoAplusOne);

  CMPLXsub(XtoAplusOne, old, FX);     /* FX = X^(a+1) - X  = F */
  F2prime.x = Ap1deg * XtoAlessOne.x; /* Ap1deg in setup */
  F2prime.y = Ap1deg * XtoAlessOne.y; /* F" */

  F1prime.x = AplusOne * XtoA.x - 1.0;
  F1prime.y = AplusOne * XtoA.y;           /*  F'  */

  FPUcplxmul(&F2prime, &FX, &Halnumer1);   /*  F * F"  */

  FPUcplxdiv(&FX, &Halnumer2, &Halnumer2);
  CMPLXsub(F1prime, Halnumer1, Halnumer2); /*  F' - F"F/2F'  */
  FPUcplxdiv(&FX, &Halnumer2, &Halnumer2);

  new.x = old.x - (parm.y * Halnumer2.x);  /* parm.y is relaxation coef. */
  new.y = old.y - (parm.y * Halnumer2.y);

  return(Halleybailout());
}

int QuaternionFPFractal()
{
  double a0,a1,a2,a3,n0,n1,n2,n3;
  a0 = old.x;
  a1 = old.y;
  a2 = floatparm->x;
  a3 = floatparm->y;

  n0 = a0*a0-a1*a1-a2*a2-a3*a3 + qc;
  n1 = 2*a0*a1 + qci;
  n2 = 2*a0*a2 + qcj;
  n3 = 2*a0*a3 + qck;
  /* Check bailout */
  magnitude = a0*a0+a1*a1+a2*a2+a3*a3;
  if (magnitude>rqlim)
    {
    return 1;
    }
  old.x = n0;
  old.y = n1;
  floatparm->x = n2;
  floatparm->y = n3;
  return(0);
}

int HyperComplexFPFractal()
{
  _HCMPLX hold, hnew;
  hold.x = old.x;
  hold.y = old.y;
  hold.z = floatparm->x;
  hold.t = floatparm->y;

  HComplexTrig0(&hold,&hnew);

  hnew.x += qc;
  hnew.y += qci;
  hnew.z += qcj;
  hnew.t += qck;

  old.x = hnew.x;
  old.y = hnew.y;
  floatparm->x = hnew.z;
  floatparm->y = hnew.t;

  /* Check bailout */
  magnitude = sqr(old.x)+sqr(old.y)+sqr(floatparm->x)+sqr(floatparm->y);
  if (magnitude>rqlim)
    {
    return 1;
    }
  return(0);
}

int MarksLambdafpFractal()
{
  /* Mark Peterson's variation of "lambda" function */

  /* Z1 = (C^(exp-1) * Z**2) + C */
  tmp.x = tempsqrx - tempsqry;
  tmp.y = old.x * old.y *2;

  new.x = coefficient.x * tmp.x - coefficient.y * tmp.y + floatparm->x;
  new.y = coefficient.x * tmp.y + coefficient.y * tmp.x + floatparm->y;

  return(floatbailout());
}

static void FloatPreCalcMagnet2()
{
  T_Cm1.x = floatparm->x - 1.0;   T_Cm1.y = floatparm->y;
  T_Cm2.x = floatparm->x - 2.0;   T_Cm2.y = floatparm->y;
  T_Cm1Cm2.x = (T_Cm1.x * T_Cm2.x) - (T_Cm1.y * T_Cm2.y);
  T_Cm1Cm2.y = (T_Cm1.x * T_Cm2.y) + (T_Cm1.y * T_Cm2.x);
  T_Cm1.x += T_Cm1.x + T_Cm1.x;   T_Cm1.y += T_Cm1.y + T_Cm1.y;
  T_Cm2.x += T_Cm2.x + T_Cm2.x;   T_Cm2.y += T_Cm2.y + T_Cm2.y;
}

int floatZpowerFractal()
{
  cpower(&old,c_exp,&new);
  new.x += floatparm->x;
  new.y += floatparm->y;
  return(floatbailout());
}

int floatCmplxZpowerFractal()
{
  new = ComplexPower(old, parm2);
  new.x += floatparm->x;
  new.y += floatparm->y;
  return(floatbailout());
}

/* extends the unary function f to *h1 */
static void HComplexTrig0(_HCMPLX *h, _HCMPLX *out)
{
  /* This is the whole beauty of Hypercomplex numbers - *ANY* unary
   * complex valued function of a complex variable can easily
   * be generalized to hypercomplex numbers
   */

  _CMPLX a,b, resulta,resultb;

  /* convert to duplex form */
  a.x = h->x - h->t;
  a.y = h->y + h->z;
  b.x = h->x + h->t;
  b.y = h->y - h->z;

  /* apply function to each part */
  CMPLXtrig0(a,resulta);
  CMPLXtrig0(b,resultb);

  /* convert back */
  out->x =  (resulta.x + resultb.x)/2;
  out->y =  (resulta.y + resultb.y)/2;
  out->z =  (resulta.y - resultb.y)/2;
  out->t =  (resultb.x - resulta.x)/2;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: fractals.c,v $
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
