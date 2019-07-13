
#ifndef mpmath_h
#define mpmath_h

#ifndef NO_IDENT
#ident "$Id: mpmath.h,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

struct DHyperComplex
{
  double x,y;
  double z,t;
};

struct LHyperComplex
{
  long x,y;
  long z,t;
};

struct DComplex
{
  double x,y;
};

struct LComplex
{
  long x,y;
};

typedef struct DComplex      _CMPLX;
typedef struct LComplex      _LCMPLX;
typedef struct DHyperComplex _HCMPLX;
typedef struct LHyperComplex _LHCMPLX;

struct MP
{
  double val;
};

struct MPC
{
  struct MP x, y;
};

extern struct MP MPTrigTable[2][4][256], InvHalfPi, HalfPi, InvLn2, Ln2;

extern int MPaccuracy, MPOverflow;

/* Mark Peterson's expanded floating point operators. */

/* function pointer support added by Tim Wegner 12/07/89 */
extern int        (*pMPcmp)(struct MP, struct MP);
extern struct MP *(*pMPmul)(struct MP, struct MP);
extern struct MP *(*pMPdiv)(struct MP, struct MP);
extern struct MP *(*pMPadd)(struct MP, struct MP);
extern struct MP *(*pMPsub)(struct MP, struct MP);
extern struct MP *(*pd2MP)(double);
extern double    *(*pMP2d)(struct MP);


/*** Formula Declarations ***/
typedef enum { D_MATH, M_MATH, L_MATH } MATH_TYPE;

#define fDiv(x, y, z) (void)((*(long*)&z) = RegDivFloat(*(long*)&x, *(long*)&y))

#define fMul16(x, y, z) (void)((*(long*)&z) = r16Mul(*(long*)&x, *(long*)&y))

#define fShift(x, Shift, z) (void)((*(long*)&z) = \
   RegSftFloat(*(long*)&x, Shift))

#define Fg2Float(x, f, z) (void)((*(long*)&z) = RegFg2Float(x, f))

#define Float2Fg(x, f) RegFloat2Fg(*(long*)&x, f)

#define fLog14(x, z) (void)((*(long*)&z) = \
        RegFg2Float(LogFloat14(*(long*)&x), 16))

#define fExp14(x, z) (void)((*(long*)&z) = ExpFloat14(*(long*)&x));

#define fSqrt14(x, z) fLog14(x, z); fShift(z, -1, z); fExp14(z, z)

/*
 * the following are declared 4 dimensional as an experimnent
 * changing declarations to _CMPLX and _LCMPLX restores the code to 2D
 */

union FracArg
{
  _CMPLX     d;
  struct MPC m;
  _LCMPLX    l;
};

struct ConstArg
{
  char *s;
  int len;
  union FracArg a;
};

extern union FracArg *Arg1,*Arg2;

extern void lStkSin(void),lStkCos(void),lStkSinh(void),lStkCosh(void),
            lStkLog(void),lStkExp(void),lStkSqr(void);

extern void dStkSin(void),dStkCos(void),dStkSinh(void),dStkCosh(void),
            dStkLog(void),dStkExp(void),dStkSqr(void);

extern void (*ltrig0)(void);
extern void (*ltrig1)(void);
extern void (*ltrig2)(void);
extern void (*ltrig3)(void);
extern void (*dtrig0)(void);
extern void (*dtrig1)(void);
extern void (*dtrig2)(void);
extern void (*dtrig3)(void);

/* -------------------------------------------------------------------- */
/*   The following #defines allow the complex transcendental functions  */
/*   in parser.c to be used here thus avoiding duplicated code.   */
/* -------------------------------------------------------------------- */

#define  CMPLXtrig0(arg,out) Arg1->d = (arg); dtrig0(); (out)=Arg1->d
#define  CMPLXtrig1(arg,out) Arg1->d = (arg); dtrig1(); (out)=Arg1->d
#define  CMPLXtrig2(arg,out) Arg1->d = (arg); dtrig2(); (out)=Arg1->d
#define  CMPLXtrig3(arg,out) Arg1->d = (arg); dtrig3(); (out)=Arg1->d

#define CMPLXsin(arg,out)    Arg1->d = (arg); dStkSin();  (out) = Arg1->d
#define CMPLXcos(arg,out)    Arg1->d = (arg); dStkCos();  (out) = Arg1->d
#define CMPLXsinh(arg,out)   Arg1->d = (arg); dStkSinh(); (out) = Arg1->d
#define CMPLXcosh(arg,out)   Arg1->d = (arg); dStkCosh(); (out) = Arg1->d
#define CMPLXlog(arg,out)    Arg1->d = (arg); dStkLog();  (out) = Arg1->d
#define CMPLXexp(arg,out)    FPUcplxexp(&(arg), &(out))

#define CMPLXsqr(arg,out) \
   (out).x = sqr((arg).x) - sqr((arg).y);\
   (out).y = ((arg).x+(arg).x) * (arg).y

#define CMPLXsqr_old(out) \
   (out).y = (old.x+old.x) * old.y;\
   (out).x = tempsqrx - tempsqry

#define CMPLXpwr(arg1,arg2,out) (out)= ComplexPower((arg1), (arg2))

#define CMPLXmult1(arg1,arg2,out) \
   Arg2->d = (arg1); Arg1->d = (arg2);\
   dStkMul(); Arg1++; Arg2++; (out) = Arg2->d

#define CMPLXmult(arg1,arg2,out) \
  {\
     _CMPLX TmP;\
     TmP.x = (arg1).x*(arg2).x - (arg1).y*(arg2).y;\
     TmP.y = (arg1).x*(arg2).y + (arg1).y*(arg2).x;\
     (out) = TmP;\
   }

#define CMPLXadd(arg1,arg2,out) \
    (out).x = (arg1).x + (arg2).x; (out).y = (arg1).y + (arg2).y

#define CMPLXsub(arg1,arg2,out) \
    (out).x = (arg1).x - (arg2).x; (out).y = (arg1).y - (arg2).y

#define CMPLXtimesreal(arg,real,out) \
    (out).x = (arg).x*(real);\
    (out).y = (arg).y*(real)

#define CMPLXrecip(arg,out) \
   { double denom; denom = sqr((arg).x) + sqr((arg).y);\
     if(denom==0.0) {(out).x = 1.0e10;(out).y = 1.0e10;}else\
   { (out).x =  (arg).x/denom;\
     (out).y = -(arg).y/denom;}}

#endif

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: mpmath.h,v $
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
