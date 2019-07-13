
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: mpmath.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static unsigned long ExpFudged(long x, int Fudge);

/* static variables */
static _CMPLX BaseLog;
static _CMPLX ct2;
static double TwoPi;
static int DivideOverflow;

_CMPLX ComplexPower(_CMPLX xx, _CMPLX yy)
{
  _CMPLX z, cLog, t;
  double e2x, siny, cosy;

  FPUcplxlog(&xx, &cLog);
  FPUcplxmul(&cLog, &yy, &t);

  if(t.x < -690)
    e2x = 0;
  else
    e2x = exp(t.x);
  FPUsincos(&t.y, &siny, &cosy);
  z.x = e2x * cosy;
  z.y = e2x * siny;
  return(z);
}

 /* LogFlag == 1  -- standard log palettes
  * LogFlag == -1 -- old log palettes
  * LogFlag >  1  -- compress counts < LogFlag into color no. 1
  * LogFlag < -1  -- use quadratic palettes based on square roots && compress
  */

void SetupLogTable(void)
{
  float l, f, c, m;
  unsigned n, prev, limit, lf;

  if (LogFlag > -2)
    {
    lf = (LogFlag > 1) ? LogFlag : 0;
    if (lf >= maxit)
      lf = maxit - 1;
    Fg2Float((long)(maxit-lf), 0, m);
    fLog14(m, m);
    Fg2Float((long)(colors-(lf?2:1)), 0, c);
    fDiv(m, c, m);
    for (prev = 1; prev <= lf; prev++)
      LogTable[prev] = 1;
    for (n = (lf?2:1); n < colors; n++)
      {
      Fg2Float((long)n, 0, f);
      fMul16(f, m, f);
      fExp14(f, l);
      limit = Float2Fg(l, 0) + lf;
      if (limit > maxit || n == colors-1)
        limit = maxit;
      while (prev <= limit)
        LogTable[prev++] = n;
      }
    }
  else
    {
    if ((lf = 0 - LogFlag) >= maxit)
      lf = maxit - 1;
    Fg2Float((long)(maxit-lf), 0, m);
    fSqrt14(m, m);
    Fg2Float((long)(colors-2), 0, c);
    fDiv(m, c, m);
    for (prev = 1; prev <= lf; prev++)
      LogTable[prev] = 1;
    for (n = 2; n < colors; n++)
      {
      Fg2Float((long)n, 0, f);
      fMul16(f, m, f);
      fMul16(f, f, l);
      limit = Float2Fg(l, 0) + lf;
      if (limit > maxit || n == colors-1)
        limit = maxit;
      while (prev <= limit)
        LogTable[prev++] = n;
      }
    }
  LogTable[0] = 0;
  if (LogFlag != -1)
    for (n = 1; n < maxit; n++) /* spread top to incl unused colors */
      if (LogTable[n] > LogTable[n-1])
        LogTable[n] = LogTable[n-1]+1;
}

long ExpFloat14(long xx)
{
  static float fLogTwo = (float)0.6931472;
  int f;
  long Ans;

  f = 23 - (int)RegFloat2Fg(RegDivFloat(xx, *(long*)&fLogTwo), 0);
  Ans = ExpFudged(RegFloat2Fg(xx, 16), f);
  return(RegFg2Float(Ans, (char)f));
}

int ComplexNewtonSetup(void)
{
  threshold = .001;
  periodicitycheck = 0;
  if(param[0] != 0.0 || param[1] != 0.0 ||
     param[2] != 0.0 || param[3] != 0.0)
    {
    croot.x = param[2];
    croot.y = param[3];
    cdegree.x = param[0];
    cdegree.y = param[1];
    FPUcplxlog(&croot, &BaseLog);
    TwoPi = asin(1.0) * 4;
    }
  return(1);
}

int ComplexNewton(void)
{
  _CMPLX cd1;

  /*
   * new = ((cdegree-1) * old**cdegree) + croot
   *       ------------------------------------
   *            cdegree * old**(cdegree-1)
   */

  cd1.x = cdegree.x - 1.0;
  cd1.y = cdegree.y;

  temp = ComplexPower(old, cd1);
  FPUcplxmul(&temp, &old, &new);

  tmp.x = new.x - croot.x;
  tmp.y = new.y - croot.y;
  if((sqr(tmp.x) + sqr(tmp.y)) < threshold)
     return(1);

  FPUcplxmul(&new, &cd1, &tmp);
  tmp.x += croot.x;
  tmp.y += croot.y;

  FPUcplxmul(&temp, &cdegree, &ct2);
  FPUcplxdiv(&tmp, &ct2, &old);
  if(DivideOverflow)
    {
    DivideOverflow = 0;
    return(1);
    }
  new = old;
  return(0);
}

int ComplexBasin(void)
{
  _CMPLX cd1;
  double mod;

  /*
   * new = ((cdegree-1) * old**cdegree) + croot
   *       ------------------------------------
   *            cdegree * old**(cdegree-1)
   */

  cd1.x = cdegree.x - 1.0;
  cd1.y = cdegree.y;

  temp = ComplexPower(old, cd1);
  FPUcplxmul(&temp, &old, &new);

  tmp.x = new.x - croot.x;
  tmp.y = new.y - croot.y;
  if((sqr(tmp.x) + sqr(tmp.y)) < threshold)
    {
    if(fabs(old.y) < .01)
      old.y = 0.0;
    FPUcplxlog(&old, &temp);
    FPUcplxmul(&temp, &cdegree, &tmp);
    mod = tmp.y/TwoPi;
    color = (int)mod;
    if(fabs(mod - color) > 0.5)
      {
      if(mod < 0.0)
        color--;
      else
        color++;
      }
    color += 2;
    if(color < 0)
      color += 128;
    return(1);
    }

  FPUcplxmul(&new, &cd1, &tmp);
  tmp.x += croot.x;
  tmp.y += croot.y;

  FPUcplxmul(&temp, &cdegree, &ct2);
  FPUcplxdiv(&tmp, &ct2, &old);
  if(DivideOverflow)
    {
    DivideOverflow = 0;
    return(1);
    }
  new = old;
  return(0);
}

/*
 * Generate a gaussian distributed number.
 * The right half of the distribution is folded onto the lower half.
 * That is, the curve slopes up to the peak and then drops to 0.
 * The larger slope is, the smaller the standard deviation.
 * The values vary from 0+offset to range+offset, with the peak
 * at range+offset.
 * To make this more complicated, you only have a
 * 1 in Distribution*(1-Probability/Range*con)+1 chance of getting a
 * Gaussian; otherwise you just get offset.
 */

int GausianNumber(int Probability, int Range)
{
  int n, r;
  long Accum = 0, p;

  p = divide((long)Probability << 16, (long)Range << 16, 16);
  p = multiply(p, con, 16);
  p = multiply((long)Distribution << 16, p, 16);
  if(!(rand15() % (Distribution - (int)(p >> 16) + 1)))
    {
    for(n = 0; n < Slope; n++)
      Accum += rand15();
    Accum /= Slope;
    r = (int)(multiply((long)Range << 15, Accum, 15) >> 14);
    r = r - Range;
    if(r < 0)
      r = -r;
    return(Range - r + Offset);
    }
  return(Offset);
}

/*
 * 32-bit integer divide routine with an 'n'-bit shift.
 * Overflow condition returns 0x7fffh with overflow = 1;
 *
 * z = divide(x,y,n);
 * z = x / y;
 */

long divide(long x, long y, int n)
{
  return (long) ( ((float)x)/ ((float)y)*(float)(1<<n));
}

/*
 * 32 bit integer multiply with n bit shift.
 * Note that we fake integer multiplication with floating point
 * multiplication.
 */

long multiply(long x, long y, int n)
{
  register long l;
  l = ((float)x)* ((float)y)/(float)(1<<n);
  if (l==0x7fffffff)
    overflow = 1;
  return l;
}

float tofloat(long x, int n)
{
  return (float)x/(float)(1<<n);
}

void FPUcplxmul(_CMPLX *x, _CMPLX *y, _CMPLX *z)
{
  double tx;
  tx = x->x * y->x - x->y * y->y;
  z->y = x->x * y->y + x->y * y->x;
  z->x = tx;
}

void FPUcplxdiv(_CMPLX *x, _CMPLX *y, _CMPLX *z)
{
  double mod,tx,yxmod,yymod;
  mod = y->x * y->x + y->y * y->y;
  if (mod==0)
    {
    DivideOverflow++;
    }
  yxmod = y->x/mod;
  yymod = - y->y/mod;
  tx = x->x * yxmod - x->y * yymod;
  z->y = x->x * yymod + x->y * yxmod;
  z->x = tx;
}

void FPUsincos(double *Angle, double *Sin, double *Cos)
{
  *Sin = sin(*Angle);
  *Cos = cos(*Angle);
}

void FPUsinhcosh(double *Angle, double *Sinh, double *Cosh)
{
  *Sinh = sinh(*Angle);
  *Cosh = cosh(*Angle);
}

void FPUcplxlog(_CMPLX *x, _CMPLX *z)
{
  double mod,zx,zy;
  mod = sqrt(x->x*x->x + x->y*x->y);
  zx = log(mod);
  zy = atan2(x->y,x->x);

  z->x = zx;
  z->y = zy;
}

/* Integer Routines */
void SinCos086(long x, long *sinx, long *cosx)
{
  double a;
  a = x/(double)(1<<16);
  *sinx = (long) (sin(a)*(double)(1<<16));
  *cosx = (long) (cos(a)*(double)(1<<16));
}

/*
 * Input is a 16 bit offset number.  Output is shifted by Fudge.
 */

static unsigned long ExpFudged(long x, int Fudge)
{
  return (long) (exp((double)x/(double)(1<<16))*(double)(1<<Fudge));
}

/* This multiplies two e/m numbers and returns an e/m number. */
long r16Mul(long x, long y)
{
  float f;
  f = em2float(x)*em2float(y);
  return float2em(f);
}

/* This takes an exp/mant number and returns a shift-16 number */
long LogFloat14(unsigned long x)
{
  return log((double)em2float(x))*(1<<16);
}

/* This divides two e/m numbers and returns an e/m number. */
long RegDivFloat(long x, long y)
{
  float f;
  f = em2float(x)/em2float(y);
  return float2em(f);
}

/*
 * This routine on the IBM converts shifted integer x,FudgeFact to
 * the 4 byte number: exp,mant,mant,mant
 * Instead of using exp/mant format, we'll just use floats.
 * Note: If sizeof(float) != sizeof(long), we're hosed.
 */

long RegFg2Float(long x, int FudgeFact)
{
  float f;
  long l;
  f = x/(float)(1<<FudgeFact);
  l = float2em(f);
  return l;
}

/*
 * This converts em to shifted integer format.
 */

long RegFloat2Fg(long x, int Fudge)
{
  return em2float(x)*(float)(1<<Fudge);
}

long RegSftFloat(long x, int Shift)
{
  float f;
  f = em2float(x);
  if (Shift>0)
    f *= (1<<Shift);
  else
    f /= (1<<Shift);
  return float2em(f);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: mpmath.c,v $
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
