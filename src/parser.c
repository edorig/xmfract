
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: parser.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

void dStkAdd(void)
{
  Arg2->d.x += Arg1->d.x;
  Arg2->d.y += Arg1->d.y;
  Arg1--;
  Arg2--;
}

void dStkSub(void)
{
  Arg2->d.x -= Arg1->d.x;
  Arg2->d.y -= Arg1->d.y;
  Arg1--;
  Arg2--;
}

void dStkNeg(void)
{
  Arg1->d.x = -Arg1->d.x;
  Arg1->d.y = -Arg1->d.y;
}

void dStkMul(void)
{
  FPUcplxmul(&Arg2->d, &Arg1->d, &Arg2->d);
  Arg1--;
  Arg2--;
}

void dStkLT(void)
{
  Arg2->d.x = (double)(Arg2->d.x < Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkLTE(void)
{
  Arg2->d.x = (double)(Arg2->d.x <= Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkMod(void)
{
  Arg1->d.x = (Arg1->d.x * Arg1->d.x) + (Arg1->d.y * Arg1->d.y);
  Arg1->d.y = 0.0;
}

void dStkPwr(void)
{
  Arg2->d = ComplexPower(Arg2->d, Arg1->d);
  Arg1--;
  Arg2--;
}

void dStkDiv(void)
{
  FPUcplxdiv(&Arg2->d, &Arg1->d, &Arg2->d);
  Arg1--;
  Arg2--;
}

void dStkAbs(void)
{
  Arg1->d.x = fabs(Arg1->d.x);
  Arg1->d.y = fabs(Arg1->d.y);
}

void dStkReal(void)
{
  Arg1->d.y = 0.0;
}

void dStkImag(void)
{
  Arg1->d.x = Arg1->d.y;
  Arg1->d.y = 0.0;
}

void dStkSin(void)
{
  double sinx, cosx, sinhy, coshy;

  FPUsincos(&Arg1->d.x, &sinx, &cosx);
  FPUsinhcosh(&Arg1->d.y, &sinhy, &coshy);
  Arg1->d.x = sinx*coshy;
  Arg1->d.y = cosx*sinhy;
}

void dStkSinh(void)
{
  double siny, cosy, sinhx, coshx;

  FPUsincos(&Arg1->d.y, &siny, &cosy);
  FPUsinhcosh(&Arg1->d.x, &sinhx, &coshx);
  Arg1->d.x = sinhx*cosy;
  Arg1->d.y = coshx*siny;
}

void dStkSqr(void)
{
  xmv[4].a.d.x = Arg1->d.x * Arg1->d.x;
  xmv[4].a.d.y = Arg1->d.y * Arg1->d.y;
  Arg1->d.y = Arg1->d.x * Arg1->d.y * 2.0;
  Arg1->d.x = xmv[4].a.d.x - xmv[4].a.d.y;
  xmv[4].a.d.x += xmv[4].a.d.y;
  xmv[4].a.d.y = 0;
}

void dStkCosh(void)
{
  double siny, cosy, sinhx, coshx;

  FPUsincos(&Arg1->d.y, &siny, &cosy);
  FPUsinhcosh(&Arg1->d.x, &sinhx, &coshx);
  Arg1->d.x = coshx*cosy;
  Arg1->d.y = sinhx*siny;
}

void dStkCosXX(void)
{
  dStkCos();
  Arg1->d.y = -Arg1->d.y;
}

void dStkExp(void)
{
  FPUcplxexp(&Arg1->d, &Arg1->d);
}

void dStkLog(void)
{
  FPUcplxlog(&Arg1->d, &Arg1->d);
}

void dStkRecip(void)
{
  double mod;
  mod =Arg1->d.x * Arg1->d.x + Arg1->d.y * Arg1->d.y;
  if(mod <= DBL_MIN)
    return;
  Arg1->d.x =  Arg1->d.x/mod;
  Arg1->d.y = -Arg1->d.y/mod;
}

void StkIdent(void)
{
 /* do nothing - the function Z */
}

void dStkCos(void)
{
  double sinx, cosx, sinhy, coshy;

  FPUsincos(&Arg1->d.x, &sinx, &cosx);
  FPUsinhcosh(&Arg1->d.y, &sinhy, &coshy);
  Arg1->d.x = cosx*coshy;
  Arg1->d.y = -sinx*sinhy; /* TIW 04-25-91 sign */
}

void dStkTan(void)
{
  double sinx, cosx, sinhy, coshy, denom;
  Arg1->d.x *= 2;
  Arg1->d.y *= 2;
  FPUsincos(&Arg1->d.x, &sinx, &cosx);
  FPUsinhcosh(&Arg1->d.y, &sinhy, &coshy);
  denom = cosx + coshy;
  if(fabs(denom) <= DBL_MIN)
    return;
  Arg1->d.x = sinx/denom;
  Arg1->d.y = sinhy/denom;
}

void dStkTanh(void)
{
  double siny, cosy, sinhx, coshx, denom;
  Arg1->d.x *= 2;
  Arg1->d.y *= 2;
  FPUsincos(&Arg1->d.y, &siny, &cosy);
  FPUsinhcosh(&Arg1->d.x, &sinhx, &coshx);
  denom = coshx + cosy;
  if(fabs(denom) <= DBL_MIN)
    return;
  Arg1->d.x = sinhx/denom;
  Arg1->d.y = siny/denom;
}

void dStkCoTan(void)
{
  double sinx, cosx, sinhy, coshy, denom;
  Arg1->d.x *= 2;
  Arg1->d.y *= 2;
  FPUsincos(&Arg1->d.x, &sinx, &cosx);
  FPUsinhcosh(&Arg1->d.y, &sinhy, &coshy);
  denom = coshy - cosx;
  if(fabs(denom) <= DBL_MIN)
    return;
  Arg1->d.x = sinx/denom;
  Arg1->d.y = -sinhy/denom;
}

void dStkCoTanh(void)
{
  double siny, cosy, sinhx, coshx, denom;
  Arg1->d.x *= 2;
  Arg1->d.y *= 2;
  FPUsincos(&Arg1->d.y, &siny, &cosy);
  FPUsinhcosh(&Arg1->d.x, &sinhx, &coshx);
  denom = coshx - cosy;
  if(fabs(denom) <= DBL_MIN)
    return;
  Arg1->d.x = sinhx/denom;
  Arg1->d.y = -siny/denom;
}

void dStkFlip(void)
{
  double t;

  t = Arg1->d.x;
  Arg1->d.x = Arg1->d.y;
  Arg1->d.y = t;
}

void dStkConj(void)
{
  Arg1->d.y = -Arg1->d.y;
}

void dStkZero(void)
{
  Arg1->d.y = Arg1->d.x = 0.0;
}

void dStkGT(void)
{
  Arg2->d.x = (double)(Arg2->d.x > Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkGTE(void)
{
  Arg2->d.x = (double)(Arg2->d.x >= Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkEQ(void)
{
  Arg2->d.x = (double)(Arg2->d.x == Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkNE(void)
{
  Arg2->d.x = (double)(Arg2->d.x != Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkAND(void)
{
  Arg2->d.x = (double)(Arg2->d.x && Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkOR(void)
{
  Arg2->d.x = (double)(Arg2->d.x || Arg1->d.x);
  Arg2->d.y = 0.0;
  Arg1--;
  Arg2--;
}

void dStkSRand(void)
{
  Arg1->l.x = (long)(Arg1->d.x * (1L << bitshift));
  Arg1->l.y = (long)(Arg1->d.y * (1L << bitshift));
  SetRandFnct();
  dRandom();
  Arg1->d = xmv[7].a.d;
}

void FPUcplxexp(_CMPLX *x, _CMPLX *z)
{
  double e2x, siny, cosy;

  e2x = exp(x->x);
  FPUsincos(&x->y, &siny, &cosy);
  z->x = e2x * cosy;
  z->y = e2x * siny;
}

void StkClr(void)
{
  xms[0] = *Arg1;
  Arg1 = &xms[0];
  Arg2 = Arg1;
  Arg2--;
}

void EndInit(void)
{
  LastInitOp = OpPtr;
}

void StkSto(void)
{
  *Store[StoPtr++] = *Arg1;
}

void StkLod(void)
{
  Arg1++;
  Arg2++;
  *Arg1 = *Load[LodPtr++];
}

void NotAFnct(void) { }
void FnctNotFound(void) { }

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: parser.c,v $
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
