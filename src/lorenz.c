
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: lorenz.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 * This file contains two 3 dimensional orbit-type fractal
 * generators - IFS and LORENZ3D, along with code to generate
 * red/blue 3D images. Tim Wegner
 */

/*
 * BAD_PIXEL is used to cutoff orbits that are diverging. It might be better
 * to test the actual floating point prbit values, but this seems safe for now.
 */

/* static functions */
static double determinant(double mat[3 ][3 ]);
static int solve3x3(double mat[3 ][3 ], double vec[3 ], double ans[3 ]);
static int l_setup_convert_to_screen(struct l_affine *l_cvt);
static int orbit3dlongcalc(void);
static int orbit3dfloatcalc(void);
static int ifs3dfloat(void);
static int ifs2d(void);
static int ifs3dlong(void);
static void setupmatrix(MATRIX doublemat);
static int long3dviewtransf(struct long3dvtinf *inf);
static int float3dviewtransf(struct float3dvtinf *inf);
static FILE *open_orbitsave(void);
static void plothist(int x, int y, int color);
static _LCMPLX ComplexSqrtLong(long x, long y);
static _CMPLX PopFloat(void);
static _LCMPLX DeQueueLong(void);
static _LCMPLX PopLong(void);

/* static variables */
static char orbitsavename[]    = {"orbits.raw"};
static char orbitsave_format[] = {"%g %g %g 15\n"};

static double adt;
static double bdt;
static double cdt;
static double xdt;
static double ydt;
static double zdt;

static int run_length;
static int yyadjust1;

static long CxLong;
static long CyLong;
static long initorbitlong[3];
static long l_a;
static long l_b;
static long l_c;
static long l_d;
static long l_dt;
static long l_dx;
static long l_dy;
static long l_dz;
static long l_orbit;
static long l_sinx;
static long l_xdt;
static long l_ydt;
static long l_adt;
static long l_bdt;
static long l_cdt;
static long l_cosx;

static struct l_affine lcvt;

enum   {breadth_first, depth_first, random_walk, random_run} major_method;
enum   {left_first, right_first}                             minor_method;

/******************************************************************/
/*       zoom box conversion functions                            */
/******************************************************************/

static double determinant(mat) /* determinant of 3x3 matrix */
double mat[3][3];
{
  /* calculate determinant of 3x3 matrix */
  return(mat[0][0]*mat[1][1]*mat[2][2] +
   mat[0][2]*mat[1][0]*mat[2][1] +
   mat[0][1]*mat[1][2]*mat[2][0] -
   mat[2][0]*mat[1][1]*mat[0][2] -
   mat[1][0]*mat[0][1]*mat[2][2] -
   mat[0][0]*mat[1][2]*mat[2][1]);

}

static int solve3x3(mat,vec,ans)
double mat[3][3], vec[3], ans[3];
{
  /* solve 3x3 linear equation [mat][ans] = [vec] */
  double denom;
  double tmp[3][3];
  int i;
  denom = determinant(mat);
  if(fabs(denom) < DBL_EPSILON) /* test if can solve */
   return(-1);
  memcpy(tmp,mat,sizeof(double)*9);
  for(i=0;i<3;i++)
   {
   tmp[0][i] = vec[0];
   tmp[1][i] = vec[1];
   tmp[2][i] = vec[2];
   ans[i]  =  determinant(tmp)/denom;
   tmp[0][i] = mat[0][i];
   tmp[1][i] = mat[1][i];
   tmp[2][i] = mat[2][i];
   }
  return(0);
}

/*
 * Conversion of complex plane to screen coordinates for rotating zoom box.
 * Assume there is an affine transformation mapping complex zoom parallelogram
 * to rectangular screen. We know this map must map parallelogram corners to
 * screen corners, so we have following equations:
 *
 *    a*xxmin+b*yymax+e == 0        (upper left)
 *    c*xxmin+d*yymax+f == 0
 *
 *    a*xx3rd+b*yy3rd+e == 0        (lower left)
 *    c*xx3rd+d*yy3rd+f == ydots-1
 *
 *    a*xxmax+b*yymin+e == xdots-1  (lower right)
 *    c*xxmax+d*yymin+f == ydots-1
 *
 *    First we must solve for a,b,c,d,e,f - (which we do once per image),
 *    then we just apply the transformation to each orbit value.
 */

int setup_convert_to_screen(struct affine *scrn_cnvt)
{
  /*
  * we do this twice - rather than having six equations with six unknowns,
  * everything partitions to two sets of three equations with three
  * unknowns. Nice, because who wants to calculate a 6x6 determinant??
  */
  double mat[3][3];
  double vec[3];
  /*
  *  first these equations - solve for a,b,e
  *  a*xxmin+b*yymax+e == 0      (upper left)
  *  a*xx3rd+b*yy3rd+e == 0      (lower left)
  *  a*xxmax+b*yymin+e == xdots-1  (lower right)
  */
  mat[0][0] = xxmin;
  mat[0][1] = yymax;
  mat[0][2] = 1.0;
  mat[1][0] = xx3rd;
  mat[1][1] = yy3rd;
  mat[1][2] = 1.0;
  mat[2][0] = xxmax;
  mat[2][1] = yymin;
  mat[2][2] = 1.0;
  vec[0]    = 0.0;
  vec[1]    = 0.0;
  vec[2]    = (double)(xdots-1);

  if(solve3x3(mat,vec, &(scrn_cnvt->a)))
   return(-1);

  /*
   * now solve these:
   * c*xxmin+d*yymax+f == 0
   * c*xx3rd+d*yy3rd+f == ydots-1
   * c*xxmax+d*yymin+f == ydots-1
   * (mat[][] has not changed - only vec[])
   */

  vec[0]    = 0.0;
  vec[1]    = (double)(ydots-1);
  vec[2]    = (double)(ydots-1);

  if(solve3x3(mat,vec, &scrn_cnvt->c))
   return(-1);
  return(0);
}

static int l_setup_convert_to_screen(struct l_affine *l_cvt)
{
  struct affine cvt;

  /* MCP 7-7-91, This function should return a something! */
  if(setup_convert_to_screen(&cvt))
    return(-1);
  l_cvt->a = cvt.a*fudge; l_cvt->b = cvt.b*fudge; l_cvt->c = cvt.c*fudge;
  l_cvt->d = cvt.d*fudge; l_cvt->e = cvt.e*fudge; l_cvt->f = cvt.f*fudge;

  /* MCP 7-7-91 */
  return(0);
}

/******************************************************************/
/*   setup functions - put in fractalspecific[fractype].per_image */
/******************************************************************/

int orbit3dlongsetup()
{
  doconnect = 1;
  waste = 100;
  projection = 2;
  if (fractype==LHENON || fractype==KAM || fractype==KAM3D ||
      fractype==INVERSEJULIA)
    doconnect=0;
  if(fractype==LROSSLER)
    waste = 500;
  if(fractype==LLORENZ)
    projection = 1;

  initorbitlong[0] = fudge;  /* initial conditions */
  initorbitlong[1] = fudge;
  initorbitlong[2] = fudge;

  if(fractype==LHENON)
    {
    l_a =  param[0]*fudge;
    l_b =  param[1]*fudge;
    l_c =  param[2]*fudge;
    l_d =  param[3]*fudge;
    }
  else if(fractype==KAM || fractype==KAM3D)
    {
    xma   = param[0];    /* angle */
    if(param[1] <= 0.0)
      param[1] = .01;
    l_b =  param[1]*fudge;  /* stepsize */
    l_c =  param[2]*fudge;  /* stop */
    xmt = l_d =  param[3];     /* points per orbit */

    l_sinx = sin(xma)*fudge;
    l_cosx = cos(xma)*fudge;
    l_orbit = 0;
    initorbitlong[0] = initorbitlong[1] = initorbitlong[2] = 0;
    }
  else if (fractype == INVERSEJULIA)
    {
    _LCMPLX Sqrt;

    CxLong = param[0] * fudge;
    CyLong = param[1] * fudge;

    maxhits    = (int) param[2];
    run_length = (int) param[3];
    if (maxhits == 0)
      maxhits = 1;
    else if (maxhits >= colors)
      maxhits = colors - 1;

    setup_convert_to_screen(&cvt);
    /* Note: using bitshift of 21 for affine, 24 otherwise */

    lcvt.a = cvt.a * (1L << 21);
    lcvt.b = cvt.b * (1L << 21);
    lcvt.c = cvt.c * (1L << 21);
    lcvt.d = cvt.d * (1L << 21);
    lcvt.e = cvt.e * (1L << 21);
    lcvt.f = cvt.f * (1L << 21);

    Sqrt = ComplexSqrtLong(fudge - 4 * CxLong, -4 * CyLong);

    switch (major_method)
      {
      case breadth_first:
        if (Init_Queue((long)32*1024) == 0)
          { /* can't get queue memory: fall back to random walk */
          major_method = random_walk;
          goto lrwalk;
          }
        EnQueueLong((fudge + Sqrt.x) / 2,  Sqrt.y / 2);
        EnQueueLong((fudge - Sqrt.x) / 2, -Sqrt.y / 2);
      break;

      case depth_first:
        if (Init_Queue((long)32*1024) == 0)
          { /* can't get queue memory: fall back to random walk */
          major_method = random_walk;
          goto lrwalk;
          }
          switch (minor_method)
            {
            case left_first:
              PushLong((fudge + Sqrt.x) / 2,  Sqrt.y / 2);
              PushLong((fudge - Sqrt.x) / 2, -Sqrt.y / 2);
            break;

            case right_first:
              PushLong((fudge - Sqrt.x) / 2, -Sqrt.y / 2);
              PushLong((fudge + Sqrt.x) / 2,  Sqrt.y / 2);
            break;
            }
      break;

      case random_walk:
lrwalk:
        lnew.x = initorbitlong[0] = fudge + Sqrt.x / 2;
        lnew.y = initorbitlong[1] =         Sqrt.y / 2;
      break;

      case random_run:
        lnew.x = initorbitlong[0] = fudge + Sqrt.x / 2;
        lnew.y = initorbitlong[1] =         Sqrt.y / 2;
      break;
      }
    }
  else
    {
    l_dt = param[0]*fudge;
    l_a =  param[1]*fudge;
    l_b =  param[2]*fudge;
    l_c =  param[3]*fudge;
    }

  /* precalculations for speed */
  l_adt = multiply(l_a,l_dt,bitshift);
  l_bdt = multiply(l_b,l_dt,bitshift);
  l_cdt = multiply(l_c,l_dt,bitshift);
  return(1);
}

int orbit3dfloatsetup()
{
  doconnect = 1;
  waste = 100;
  projection = 2;

  if(fractype==FPHENON || fractype==FPPICKOVER || fractype==FPGINGERBREAD
     || fractype == KAMFP || fractype == KAM3DFP
     || fractype == FPHOPALONG || fractype == INVERSEJULIAFP)
    doconnect=0;
  if(fractype==FPLORENZ3D1 || fractype==FPLORENZ3D3 ||
     fractype==FPLORENZ3D4)
    waste = 750;
  if(fractype==FPROSSLER)
    waste = 500;
  if(fractype==FPLORENZ)
    projection = 1; /* plot x and z */

  dinitorbit[0] = 1;  /* initial conditions */
  dinitorbit[1] = 1;
  dinitorbit[2] = 1;
  if(fractype==FPGINGERBREAD)
    {
    dinitorbit[0] = param[0];  /* initial conditions */
    dinitorbit[1] = param[1];
    }

  if(fractype==ICON || fractype==ICON3D)        /* DMF */
    {
    dinitorbit[0] = 0.01;  /* initial conditions */
    dinitorbit[1] = 0.003;
    doconnect = 0;
    waste = 2000;
    }

  if(fractype==FPHENON || fractype==FPPICKOVER)
    {
    xma =  param[0];
    xmb =  param[1];
    xmc =  param[2];
    xmd =  param[3];
    }
  else if(fractype==ICON || fractype==ICON3D)        /* DMF */
    {
    dinitorbit[0] = 0.01;  /* initial conditions */
    dinitorbit[1] = 0.003;
    doconnect = 0;
    waste = 2000;
    /* Initialize parameters */
    xma  =   param[0];
    xmb  =   param[1];
    xmc  =   param[2];
    xmd  =   param[3];
    }
  else if(fractype==KAMFP || fractype==KAM3DFP)
    {
    xma = param[0];        /* angle */
    if(param[1] <= 0.0)
      param[1] = .01;
    xmb =  param[1];  /* stepsize */
    xmc =  param[2];  /* stop */
    xmt = l_d =  param[3];     /* points per orbit */
    sinx = sin(xma);
    cosx = cos(xma);
    orbit = 0;
    dinitorbit[0] = dinitorbit[1] = dinitorbit[2] = 0;
    }
  else if(fractype==FPHOPALONG || fractype==FPMARTIN)
    {
    dinitorbit[0] = 0;  /* initial conditions */
    dinitorbit[1] = 0;
    dinitorbit[2] = 0;
    doconnect = 0;
    xma =  param[0];
    xmb =  param[1];
    xmc =  param[2];
    xmd =  param[3];
    }
  else if (fractype == INVERSEJULIAFP)
    {
    _CMPLX Sqrt;

    Cx = param[0];
    Cy = param[1];

    maxhits    = (int) param[2];
    run_length = (int) param[3];
    if (maxhits == 0)
      maxhits = 1;
    else if (maxhits >= colors)
      maxhits = colors - 1;

    setup_convert_to_screen(&cvt);

    /* find fixed points: guaranteed to be in the set */
    Sqrt = ComplexSqrtFloat(1 - 4 * Cx, -4 * Cy);
    switch ((int) major_method)
      {
      case breadth_first:
        if (Init_Queue((long)32*1024) == 0)
          { /* can't get queue memory: fall back to random walk */
          major_method = random_walk;
          goto rwalk;
          }
        EnQueueFloat((1 + Sqrt.x) / 2,  Sqrt.y / 2);
        EnQueueFloat((1 - Sqrt.x) / 2, -Sqrt.y / 2);
      break;

      case depth_first:      /* depth first (choose direction) */
        if (Init_Queue((long)32*1024) == 0)
          { /* can't get queue memory: fall back to random walk */
          major_method = random_walk;
          goto rwalk;
          }
        switch (minor_method)
          {
          case left_first:
            PushFloat((1 + Sqrt.x) / 2,  Sqrt.y / 2);
            PushFloat((1 - Sqrt.x) / 2, -Sqrt.y / 2);
          break;

         case right_first:
           PushFloat((1 - Sqrt.x) / 2, -Sqrt.y / 2);
           PushFloat((1 + Sqrt.x) / 2,  Sqrt.y / 2);
         break;
         }
      break;

      case random_walk:
rwalk:
        new.x = dinitorbit[0] = 1 + Sqrt.x / 2;
        new.y = dinitorbit[1] = Sqrt.y / 2;
      break;

      case random_run:  /* random run, choose intervals */
        major_method = random_run;
        new.x = dinitorbit[0] = 1 + Sqrt.x / 2;
        new.y = dinitorbit[1] = Sqrt.y / 2;
      break;
      }
    }
  else
    {
    dt = param[0];
    xma =  param[1];
    xmb =  param[2];
    xmc =  param[3];
    }

  /* precalculations for speed */
  adt = xma*dt;
  bdt = xmb*dt;
  cdt = xmc*dt;

  return(1);
}

/******************************************************************/
/*   orbit functions - put in fractalspecific[fractype].orbitcalc */
/******************************************************************/

/* Julia sets by inverse iterations added by Juan J. Buhler 4/3/92 */
/* Integrated with Lorenz by Tim Wegner 7/20/92 */
/* Add Modified Inverse Iteration Method, 11/92 by Michael Snyder  */

int
Minverse_julia_orbit()
{
  static int   random_dir = 0, random_len = 0;
  int    newrow, newcol;
  int    color,  leftright;

  /*
   * First, compute new point
   */

  switch (major_method)
    {
    case breadth_first:
      if (QueueEmpty())
        return -1;
      new = DeQueueFloat();
    break;

    case depth_first:
      if (QueueEmpty())
        return -1;
      new = PopFloat();
    break;

    case random_walk:
    break;

    case random_run:
    break;
    }

  /*
   * Next, find its pixel position
   */
  newcol = cvt.a * new.x + cvt.b * new.y + cvt.e;
  newrow = cvt.c * new.x + cvt.d * new.y + cvt.f;

  /*
   * Now find the next point(s), and flip a coin to choose one.
   */

  new       = ComplexSqrtFloat(new.x - Cx, new.y - Cy);
  leftright = (RANDOM(2)) ? 1 : -1;

  if (newcol < 1 || newcol >= xdots || newrow < 1 || newrow >= ydots)
    {
    /*
     * MIIM must skip points that are off the screen boundary,
     * since it cannot read their color.
     */
    switch (major_method)
      {
      case breadth_first:
        EnQueueFloat(leftright * new.x, leftright * new.y);
        return 1;

      case depth_first:
        PushFloat   (leftright * new.x, leftright * new.y);
        return 1;

      case random_run:
      case random_walk:
      break;
      }
    }

  /*
   * Read the pixel's color:
   * For MIIM, if color >= maxhits, discard the point
   *           else put the point's children onto the queue
   */
  color  = getcolor(newcol, newrow);
  switch (major_method)
    {
    case breadth_first:
      if (color < maxhits)
        {
        putcolor(newcol, newrow, color+1);
        EnQueueFloat( new.x,  new.y);
        EnQueueFloat(-new.x, -new.y);
        }
    break;

    case depth_first:
      if (color < maxhits)
        {
        putcolor(newcol, newrow, color+1);
        if (minor_method == left_first)
          {
          if (QueueFullAlmost())
            PushFloat(-new.x, -new.y);
          else
            {
            PushFloat( new.x,  new.y);
            PushFloat(-new.x, -new.y);
            }
          }
        else
          {
          if (QueueFullAlmost())
            PushFloat( new.x,  new.y);
          else
            {
            PushFloat(-new.x, -new.y);
            PushFloat( new.x,  new.y);
            }
          }
        }
    break;

    case random_run:
      if (random_len-- == 0)
        {
        random_len = RANDOM(run_length);
        random_dir = RANDOM(3);
        }
      switch (random_dir)
        {
        case 0:  /* left */
        break;

        case 1:  /* right */
          new.x = -new.x;
          new.y = -new.y;
        break;

        case 2:  /* random direction */
          new.x = leftright * new.x;
          new.y = leftright * new.y;
        break;
        }
      if (color < colors-1)
        putcolor(newcol, newrow, color+1);
    break;

    case random_walk:
      if (color < colors-1)
        putcolor(newcol, newrow, color+1);
      new.x = leftright * new.x;
      new.y = leftright * new.y;
    break;
    }
  return 1;
}

int
Linverse_julia_orbit()
{
  static int   random_dir = 0, random_len = 0;
  int    newrow, newcol;
  int    color;

  /*
   * First, compute new point
   */
  switch (major_method)
    {
    case breadth_first:
      if (QueueEmpty())
        return -1;
      lnew = DeQueueLong();
    break;

    case depth_first:
      if (QueueEmpty())
        return -1;
      lnew = PopLong();
    break;

    case random_walk:
      lnew = ComplexSqrtLong(lnew.x - CxLong, lnew.y - CyLong);
      if (RANDOM(2))
        {
        lnew.x = -lnew.x;
        lnew.y = -lnew.y;
        }
    break;

    case random_run:
      lnew = ComplexSqrtLong(lnew.x - CxLong, lnew.y - CyLong);
      if (random_len == 0)
        {
        random_len = RANDOM(run_length);
        random_dir = RANDOM(3);
        }
      switch (random_dir)
        {
        case 0:  /* left */
        break;

        case 1:  /* right */
          lnew.x = -lnew.x;
          lnew.y = -lnew.y;
        break;

        case 2:  /* random direction */
          if (RANDOM(2))
            {
            lnew.x = -lnew.x;
            lnew.y = -lnew.y;
            }
        break;
        }
    }

  /*
   * Next, find its pixel position
   *
   * Note: had to use a bitshift of 21 for this operation because
   * otherwise the values of lcvt were truncated.  Used bitshift
   * of 24 otherwise, for increased precision.
   */

  newcol = (multiply(lcvt.a, lnew.x >> (bitshift - 21), 21) +
    multiply(lcvt.b, lnew.y >> (bitshift - 21), 21) + lcvt.e) >> 21;
  newrow = (multiply(lcvt.c, lnew.x >> (bitshift - 21), 21) +
    multiply(lcvt.d, lnew.y >> (bitshift - 21), 21) + lcvt.f) >> 21;

  if (newcol < 1 || newcol >= xdots || newrow < 1 || newrow >= ydots)
    {
    /*
     * MIIM must skip points that are off the screen boundary,
     * since it cannot read their color.
     */
    if (RANDOM(2))
      color =  1;
    else
      color = -1;
    switch (major_method)
      {
      case breadth_first:
        lnew = ComplexSqrtLong(lnew.x - CxLong, lnew.y - CyLong);
        EnQueueLong(color * lnew.x, color * lnew.y);
      break;

      case depth_first:
        lnew = ComplexSqrtLong(lnew.x - CxLong, lnew.y - CyLong);
        PushLong(color * lnew.x, color * lnew.y);
      break;

      case random_run:
        random_len--;

      /*FALLTHRU*/
      case random_walk:
        break;
      }
    return 1;
    }

  /*
   * Read the pixel's color:
   * For MIIM, if color >= maxhits, discard the point
   *           else put the point's children onto the queue
   */
  color  = getcolor(newcol, newrow);
  switch (major_method)
    {
    case breadth_first:
      if (color < maxhits)
        {
        putcolor(newcol, newrow, color+1);
        lnew = ComplexSqrtLong(lnew.x - CxLong, lnew.y - CyLong);
        EnQueueLong( lnew.x,  lnew.y);
        EnQueueLong(-lnew.x, -lnew.y);
        }
    break;

    case depth_first:
      if (color < maxhits)
        {
        putcolor(newcol, newrow, color+1);
        lnew = ComplexSqrtLong(lnew.x - CxLong, lnew.y - CyLong);
        if (minor_method == left_first)
          {
            if (QueueFullAlmost())
              PushLong(-lnew.x, -lnew.y);
            else
              {
              PushLong( lnew.x,  lnew.y);
              PushLong(-lnew.x, -lnew.y);
              }
            }
          else
            {
            if (QueueFullAlmost())
              PushLong( lnew.x,  lnew.y);
            else
              {
              PushLong(-lnew.x, -lnew.y);
              PushLong( lnew.x,  lnew.y);
              }
            }
          }
    break;

    case random_run:
      random_len--;
    /*FALLTHRU*/
    case random_walk:
      if (color < colors-1)
        putcolor(newcol, newrow, color+1);
    break;
    }
  return 1;
}

int lorenz3dlongorbit(long *l_x, long *l_y, long *l_z)
{
  l_xdt = multiply(*l_x,l_dt,bitshift);
  l_ydt = multiply(*l_y,l_dt,bitshift);
  l_dx  = -multiply(l_adt,*l_x,bitshift) + multiply(l_adt,*l_y,bitshift);
  l_dy  =  multiply(l_bdt,*l_x,bitshift) -l_ydt -multiply(*l_z,l_xdt,bitshift);
  l_dz  = -multiply(l_cdt,*l_z,bitshift) + multiply(*l_x,l_ydt,bitshift);

  *l_x += l_dx;
  *l_y += l_dy;
  *l_z += l_dz;
  return(0);
}

int lorenz3d1floatorbit(double *x, double *y, double *z)
{
  double norm;

  xdt = (*x)*dt;
  ydt = (*y)*dt;
  zdt = (*z)*dt;

  /* 1-lobe Lorenz */
  norm = sqrt((*x)*(*x)+(*y)*(*y));
  dx   = (-adt-dt)*(*x) + (adt-bdt)*(*y) + (dt-adt)*norm + ydt*(*z);
  dy   = (bdt-adt)*(*x) - (adt+dt)*(*y) + (bdt+adt)*norm - xdt*(*z) -
  norm*zdt;
  dz   = (ydt/2) - cdt*(*z);

  *x += dx;
  *y += dy;
  *z += dz;
  return(0);
}

int lorenz3dfloatorbit(double *x, double *y, double *z)
{
  xdt = (*x)*dt;
  ydt = (*y)*dt;
  zdt = (*z)*dt;

  /* 2-lobe Lorenz (the original) */
  dx  = -adt*(*x) + adt*(*y);
  dy  =  bdt*(*x) - ydt - (*z)*xdt;
  dz  = -cdt*(*z) + (*x)*ydt;

  *x += dx;
  *y += dy;
  *z += dz;
  return(0);
}

int lorenz3d3floatorbit(double *x, double *y, double *z)
{
  double norm;

  xdt = (*x)*dt;
  ydt = (*y)*dt;
  zdt = (*z)*dt;

  /* 3-lobe Lorenz */
  norm = sqrt((*x)*(*x)+(*y)*(*y));
  dx = (-(adt+dt)*(*x) + (adt-bdt+zdt)*(*y)) / 3 +
    ((dt-adt)*((*x)*(*x)-(*y)*(*y)) +
    2*(bdt+adt-zdt)*(*x)*(*y))/(3*norm);
  dy = ((bdt-adt-zdt)*(*x) - (adt+dt)*(*y)) / 3 +
    (2*(adt-dt)*(*x)*(*y) +
    (bdt+adt-zdt)*((*x)*(*x)-(*y)*(*y)))/(3*norm);
  dz = (3*xdt*(*x)*(*y)-ydt*(*y)*(*y))/2 - cdt*(*z);

  *x += dx;
  *y += dy;
  *z += dz;
  return(0);
}

int lorenz3d4floatorbit(double *x, double *y, double *z)
{
  xdt = (*x)*dt;
  ydt = (*y)*dt;
  zdt = (*z)*dt;

  /* 4-lobe Lorenz */
  dx = (-adt*(*x)*(*x)*(*x) + (2*adt+bdt-zdt)*(*x)*(*x)*(*y) +
       (adt-2*dt)*(*x)*(*y)*(*y) + (zdt-bdt)*(*y)*(*y)*(*y)) /
       (2 * ((*x)*(*x)+(*y)*(*y)));
  dy = ((bdt-zdt)*(*x)*(*x)*(*x) + (adt-2*dt)*(*x)*(*x)*(*y) +
       (-2*adt-bdt+zdt)*(*x)*(*y)*(*y) - adt*(*y)*(*y)*(*y)) /
       (2 * ((*x)*(*x)+(*y)*(*y)));
  dz = (2*xdt*(*x)*(*x)*(*y) - 2*xdt*(*y)*(*y)*(*y) - cdt*(*z));

  *x += dx;
  *y += dy;
  *z += dz;
  return(0);
}

/*ARGSUSED*/
int henonfloatorbit(double *x, double *y, double *z)
{
  double newx,newy;
  newx  = 1 + *y - xma*(*x)*(*x);
  newy  = xmb*(*x);
  *x = newx;
  *y = newy;
  return(0);
}

/*ARGSUSED*/
int henonlongorbit(long *l_x, long *l_y, long *l_z)
{
  long newx,newy;
  newx = multiply(*l_x,*l_x,bitshift);
  newx = multiply(newx,l_a,bitshift);
  newx  = fudge + *l_y - newx;
  newy  = multiply(l_b,*l_x,bitshift);
  *l_x = newx;
  *l_y = newy;
  return(0);
}

int rosslerfloatorbit(double *x, double *y, double *z)
{
  xdt = (*x)*dt;
  ydt = (*y)*dt;

  dx = -ydt - (*z)*dt;
  dy = xdt + (*y)*adt;
  dz = bdt + (*z)*xdt - (*z)*cdt;

  *x += dx;
  *y += dy;
  *z += dz;
  return(0);
}

int pickoverfloatorbit(double *x, double *y, double *z)
{
  double newx,newy,newz;
  newx = sin(xma*(*y)) - (*z)*cos(xmb*(*x));
  newy = (*z)*sin(xmc*(*x)) - cos(xmd*(*y));
  newz = sin(*x);
  *x = newx;
  *y = newy;
  *z = newz;
  return(0);
}

/* page 149 "Science of Fractal Images" */
/*ARGSUSED*/
int gingerbreadfloatorbit(double *x, double *y, double *z)
{
  double newx;
  newx = 1 - (*y) + fabs(*x);
  *y = *x;
  *x = newx;
  return(0);
}

int rosslerlongorbit(long *l_x, long *l_y, long *l_z)
{
  l_xdt = multiply(*l_x,l_dt,bitshift);
  l_ydt = multiply(*l_y,l_dt,bitshift);

  l_dx  = -l_ydt - multiply(*l_z,l_dt,bitshift);
  l_dy  =  l_xdt + multiply(*l_y,l_adt,bitshift);
  l_dz  =  l_bdt + multiply(*l_z,l_xdt,bitshift)
     - multiply(*l_z,l_cdt,bitshift);

  *l_x += l_dx;
  *l_y += l_dy;
  *l_z += l_dz;

  return(0);
}

/* OSTEP  = Orbit Step (and inner orbit value) */
/* NTURNS = Outside Orbit */
/* TURN2  = Points per orbit */
/* a    = Angle */

int kamtorusfloatorbit(double *r, double *s, double *z)
{
  double srr;
  if(xmt++ >= l_d)
    {
    orbit += xmb;
    (*r) = (*s) = orbit/3;
    xmt = 0;
    *z = orbit;
    if(orbit > xmc)
      return(1);
    }
  srr = (*s)-(*r)*(*r);
  (*s)=(*r)*sinx+srr*cosx;
  (*r)=(*r)*cosx-srr*sinx;
  return(0);
}

int kamtoruslongorbit(long *r, long *s, long *z)
{
  long srr;
  if(xmt++ >= l_d)
    {
    l_orbit += l_b;
    (*r) = (*s) = l_orbit/3;
    xmt = 0;
    *z = l_orbit;
    if(l_orbit > l_c)
      return(1);
    }
  srr = (*s)-multiply((*r),(*r),bitshift);
  (*s)=multiply((*r),l_sinx,bitshift)+multiply(srr,l_cosx,bitshift);
  (*r)=multiply((*r),l_cosx,bitshift)-multiply(srr,l_sinx,bitshift);
  return(0);
}

/*ARGSUSED*/
int hopalong2dfloatorbit(double *x, double *y, double *z)
{
  double tmp;
  tmp = *y - sign(*x)*sqrt(fabs(xmb*(*x)-xmc));
  *y = xma - *x;
  *x = tmp;
  return(0);
}

/*ARGSUSED*/
int martin2dfloatorbit(double *x, double *y, double *z)
{
  double tmp;
  tmp = *y - sin(*x);
  *y = xma - *x;
  *x = tmp;
  return(0);
}

/*ARGSUSED*/
int mandelcloudfloat(double *x, double *y, double *z)
{
  double newx,newy,x2,y2;
  x2 = (*x)*(*x);
  y2 = (*y)*(*y);
  if (x2+y2>2)
    return 1;
  newx = x2-y2+xma;
  newy = 2*(*x)*(*y)+xmb;
  *x = newx;
  *y = newy;
  return(0);
}

/*ARGSUSED*/
int dynamfloat(double *x, double *y, double *z)
{
  _CMPLX cp,tmp;
  double newx,newy;
  cp.x = xmb* *x;
  cp.y = 0;
  CMPLXtrig0(cp, tmp);
  newy = *y + dt*sin(*x + xma*tmp.x);
  if (euler)
    {
    *y = newy;
    }

  cp.x = xmb* *y;
  cp.y = 0;
  CMPLXtrig0(cp, tmp);
  newx = *x - dt*sin(*y + xma*tmp.x);
  *x = newx;
  *y = newy;
  return(0);
}

int iconfloatorbit(double *x, double *y, double *z)
{
  double oldx, oldy, zzbar, zreal, zimag, za, zb, zn, p;
  int i;
  int tmp;

  tmp = (int)(param[5] - 2);

  oldx = *x;
  oldy = *y;

  zzbar = oldx * oldx + oldy * oldy;
  zreal = oldx;
  zimag = oldy;

  for(i=1; i <= tmp; i++)
    {
    za = zreal * oldx - zimag * oldy;
    zb = zimag * oldx + zreal * oldy;
    zreal = za;
    zimag = zb;
    }
  zn = oldx * zreal - oldy * zimag;
  p = XLAMBDA + XALPHA * zzbar + XBETA * zn;
  *x = p * oldx + XGAMMA * zreal - XOMEGA * oldy;
  *y = p * oldy - XGAMMA * zimag + XOMEGA * oldx;

  *z = zzbar;
  return(0);
}

/**********************************************************************/
/*   Main fractal engines - put in fractalspecific[fractype].calctype */
/**********************************************************************/

int inverse_julia_per_image()
{
  int color = 0;

  if (resuming)            /* can't resume */
    return -1;

  while (color >= 0)       /* generate points */
    {
    if (check_key() != 0)
      {
      Free_Queue();
      return -1;
      }
    color = curfractalspecific->orbitcalc();
    old = new;
    }
  Free_Queue();
  return 0;
}

int orbit2dfloat()
{
  FILE *fp;
  double x,y,z;
  int color,col,row;
  int count;
  int oldrow, oldcol;
  struct affine cvt;
  int ret;
  double *p0 = (double *)0;
  double *p1 = (double *)0;
  double *p2 = (double *)0;

  fp = open_orbitsave();
  /* setup affine screen coord conversion */
  setup_convert_to_screen(&cvt);

  /* set up projection scheme */
  if(projection==0)
    {
    p0 = &z;
    p1 = &x;
    p2 = &y;
    }
  else if(projection==1)
    {
    p0 = &x;
    p1 = &z;
    p2 = &y;
    }
  else if(projection==2)
    {
    p0 = &x;
    p1 = &y;
    p2 = &z;
    }
  count = 0;
  if(inside > 0)
    color = inside;
  if(color >= colors)
    color = 1;
  oldcol = oldrow = -1;
  x = dinitorbit[0];
  y = dinitorbit[1];
  z = dinitorbit[2];

  if (resuming)
    {
    start_resume();
    get_resume(sizeof(count),&count,sizeof(color),&color,
        sizeof(oldrow),&oldrow,sizeof(oldcol),&oldcol,
        sizeof(x),&x,sizeof(y),&y,sizeof(z),&z,
        sizeof(xmt),&xmt,sizeof(orbit),&orbit,
        0);
    end_resume();
    }

  ret = 0;
  while(1)
    {
    if(check_key() != 0)
      {
      alloc_resume(100,1);
      put_resume(sizeof(count),&count,sizeof(color),&color,
          sizeof(oldrow),&oldrow,sizeof(oldcol),&oldcol,
          sizeof(x),&x,sizeof(y),&y,sizeof(z),&z,
          sizeof(xmt),&xmt,sizeof(orbit),&orbit,
          0);
      ret = -1;
      break;
      }

    if (++count > 1000)
      {        /* time to switch colors? */
      count = 0;
      if (++color >= colors)   /* another color to switch to? */
        color = 1;  /* (don't use the background color) */
      }

    col = cvt.a*x + cvt.b*y + cvt.e;
    row = cvt.c*x + cvt.d*y + cvt.f;
    if ( col >= 0 && col < xdots && row >= 0 && row < ydots )
      {
      if(fractype!=ICON)
        {
        if(oldcol != -1 && doconnect)
          draw_line(col,row,oldcol,oldrow,color&(colors-1));
        else
          (*plot)(col,row,color&(colors-1));
        }
      else
        {
        /* should this be using plothist()? */
        color = getcolor(col,row)+1;
        if( color < colors ) /* color sticks on last value */
          (*plot)(col,row,color);
        }

      oldcol = col;
      oldrow = row;
      }
    else if((long)abs(row) + (long)abs(col) > BAD_PIXEL) /* sanity check */
      return(ret);
    else
      oldrow = oldcol = -1;

    if(curfractalspecific->orbitcalc(p0, p1, p2))
      break;
    if(fp)
      fprintf(fp,orbitsave_format,*p0,*p1,0.0);
    }
  if(fp)
    fclose(fp);
  return(ret);
}

int orbit2dlong()
{
  FILE *fp;
  long x,y,z;
  int color,col,row;
  int count;
  int oldrow, oldcol;
  struct l_affine cvt;
  int ret,start;
  long *p0 = (long *)0;
  long *p1 = (long *)0;
  long *p2 = (long *)0;

  start = 1;

  fp = open_orbitsave();

  /* setup affine screen coord conversion */
  l_setup_convert_to_screen(&cvt);

  /* set up projection scheme */
  if(projection==0)
    {
    p0 = &z;
    p1 = &x;
    p2 = &y;
    }
  else if(projection==1)
    {
    p0 = &x;
    p1 = &z;
    p2 = &y;
    }
  else if(projection==2)
    {
    p0 = &x;
    p1 = &y;
    p2 = &z;
    }

  count = 0;
  if(inside > 0)
    color = inside;
  if(color >= colors)
    color = 1;
  oldcol = oldrow = -1;
  x = initorbitlong[0];
  y = initorbitlong[1];
  z = initorbitlong[2];

  if (resuming)
    {
    start_resume();
    get_resume(sizeof(count),&count,sizeof(color),&color,
        sizeof(oldrow),&oldrow,sizeof(oldcol),&oldcol,
        sizeof(x),&x,sizeof(y),&y,sizeof(z),&z,
        sizeof(xmt),&xmt,sizeof(l_orbit),&l_orbit,
        0);
    end_resume();
    }

  ret = 0;
  while(1)
    {
    if(check_key() != 0)
      {
      alloc_resume(100,1);
      put_resume(sizeof(count),&count,sizeof(color),&color,
          sizeof(oldrow),&oldrow,sizeof(oldcol),&oldcol,
          sizeof(x),&x,sizeof(y),&y,sizeof(z),&z,
          sizeof(xmt),&xmt,sizeof(l_orbit),&l_orbit,
          0);
      ret = -1;
      break;
      }
    if (++count > 1000)
      {        /* time to switch colors? */
      count = 0;
      if (++color >= colors)   /* another color to switch to? */
        color = 1;  /* (don't use the background color) */
      }

    col = (multiply(cvt.a,x,bitshift) +
           multiply(cvt.b,y,bitshift) + cvt.e) >> bitshift;
    row = (multiply(cvt.c,x,bitshift) +
           multiply(cvt.d,y,bitshift) + cvt.f) >> bitshift;
    if(overflow)
      {
      overflow = 0;
      return(ret);
      }
    if ( col >= 0 && col < xdots && row >= 0 && row < ydots )
      {
      if(oldcol != -1 && doconnect)
        draw_line(col,row,oldcol,oldrow,color&(colors-1));
      else if(!start)
        (*plot)(col,row,color&(colors-1));
      oldcol = col;
      oldrow = row;
      start = 0;
      }
    else if((long)abs(row) + (long)abs(col) > BAD_PIXEL) /* sanity check */
      return(ret);
    else
      oldrow = oldcol = -1;

    /* Calculate the next point */
      if(curfractalspecific->orbitcalc(p0, p1, p2))
        break;
      if(fp)
        fprintf(fp,orbitsave_format,(double)*p0/fudge,(double)*p1/fudge,0.0);
    }

  if(fp)
    fclose(fp);
  return(ret);
}

static int orbit3dlongcalc()
{
  FILE *fp;
  unsigned count;
  int oldcol,oldrow;
  int oldcol1,oldrow1;
  struct long3dvtinf inf;
  unsigned long maxct;
  int color;
  int ret;

  /* setup affine screen coord conversion */
  l_setup_convert_to_screen(&inf.cvt);

  oldcol1 = oldrow1 = oldcol = oldrow = -1;
  color = 2;
  if(color >= colors)
     color = 1;

  inf.orbit[0] = initorbitlong[0];
  inf.orbit[1] = initorbitlong[1];
  inf.orbit[2] = initorbitlong[2];

  if(diskvideo)                /* this would KILL a disk drive! */
     notdiskmsg();

  fp = open_orbitsave();

  /* make maxct a function of screen size               */
  maxct = maxit*40L;
  count = inf.ct = 0L;
  ret = 0;
  while(inf.ct++ < maxct) /* loop until keypress or maxit */
    {
    /* calc goes here */
    if (++count > 1000)
      {        /* time to switch colors? */
      count = 0;
      if (++color >= colors)   /* another color to switch to? */
        color = 1;        /* (don't use the background color) */
      }
    if(check_key() != 0)
      {
      ret = -1;
      break;
      }

    curfractalspecific->orbitcalc(&inf.orbit[0],&inf.orbit[1],&inf.orbit[2]);
    if(fp)
      fprintf(fp,orbitsave_format,(double)inf.orbit[0]/fudge,
        (double)inf.orbit[1]/fudge,(double)inf.orbit[2]/fudge);
    if (long3dviewtransf(&inf))
      {
      /* plot if inside window */
      if (inf.col >= 0)
        {
        if(realtime)
          whichimage=1;
        if(oldcol != -1 && doconnect)
          draw_line(inf.col,inf.row,oldcol,oldrow,color&(colors-1));
        else
          (*plot)(inf.col,inf.row,color&(colors-1));
        }
      else if (inf.col == -2)
        return(ret);
      oldcol = inf.col;
      oldrow = inf.row;
      if(realtime)
        {
        whichimage=2;
          /* plot if inside window */
        if (inf.col1 >= 0)
          {
          if(oldcol1 != -1 && doconnect)
            draw_line(inf.col1,inf.row1,oldcol1,oldrow1,color&(colors-1));
           else
             (*plot)(inf.col1,inf.row1,color&(colors-1));
           }
        else if (inf.col1 == -2)
          return(ret);
        oldcol1 = inf.col1;
        oldrow1 = inf.row1;
        }
      }
    }
  if(fp)
    fclose(fp);
  return(ret);
}

static int orbit3dfloatcalc()
{
  FILE *fp;
  unsigned count;
  int oldcol,oldrow;
  int oldcol1,oldrow1;
  unsigned long maxct;
  int color;
  int ret;
  struct float3dvtinf inf;

  /* setup affine screen coord conversion */
  setup_convert_to_screen(&inf.cvt);

  oldcol = oldrow = -1;
  oldcol1 = oldrow1 = -1;
  color = 2;
  if(color >= colors)
     color = 1;
  inf.orbit[0] = dinitorbit[0];
  inf.orbit[1] = dinitorbit[1];
  inf.orbit[2] = dinitorbit[2];

  if(diskvideo)                /* this would KILL a disk drive! */
     notdiskmsg();

  fp = open_orbitsave();

  maxct = maxit*40L;
  count = inf.ct = 0L;
  ret = 0;
  while(inf.ct++ < maxct) /* loop until keypress or maxit */
    {
    /* calc goes here */
    if (++count > 1000)
      {        /* time to switch colors? */
      count = 0;
      if (++color >= colors)   /* another color to switch to? */
        color = 1;        /* (don't use the background color) */
      }

    if(check_key() != 0)
      {
      ret = -1;
      break;
      }

    curfractalspecific->orbitcalc(&inf.orbit[0],&inf.orbit[1],&inf.orbit[2]);
    if(fp)
      fprintf(fp,orbitsave_format,inf.orbit[0],inf.orbit[1],inf.orbit[2]);
    if (float3dviewtransf(&inf))
      {
      /* plot if inside window */
      if (inf.col >= 0)
        {
        if(realtime)
          whichimage=1;
        if(oldcol != -1 && doconnect)
          draw_line(inf.col,inf.row,oldcol,oldrow,color&(colors-1));
        else
          (*plot)(inf.col,inf.row,color&(colors-1));
        }
      else if (inf.col == -2)
        return(ret);
      oldcol = inf.col;
      oldrow = inf.row;
      if(realtime)
        {
        whichimage=2;
        /* plot if inside window */
        if (inf.col1 >= 0)
          {
          if(oldcol1 != -1 && doconnect)
            draw_line(inf.col1,inf.row1,oldcol1,oldrow1,color&(colors-1));
          else
            (*plot)(inf.col1,inf.row1,color&(colors-1));
          }
        else if (inf.col1 == -2)
          return(ret);
        oldcol1 = inf.col1;
        oldrow1 = inf.row1;
        }
      }
    }
  if(fp)
    fclose(fp);
  return(ret);
}

int dynam2dfloatsetup()
{
  doconnect = 0;
  euler = 0;
  xmd = param[0]; /* number of intervals */
  if (xmd<0)
    {
    xmd = -xmd;
    doconnect = 1;
    }
  else if (xmd==0)
    {
    xmd = 1;
    }
  if (fractype==DYNAMICFP)
    {
    xma = param[2]; /* parameter */
    xmb = param[3]; /* parameter */
    dt = param[1]; /* step size */
    if (dt<0)
      {
      dt = -dt;
      euler = 1;
      }
    if (dt==0)
      dt = 0.01;
    }
  if (outside == -5)
    {
    plot = plothist;
    }
  return(1);
}

/*
 * This is the routine called to perform a time-discrete dynamical
 * system image.
 * The starting positions are taken by stepping across the image in steps
 * of parameter1 pixels.  maxit differential equation steps are taken, with
 * a step size of parameter2.
 */

int dynam2dfloat()
{
  FILE *fp;
  double x,y;
  int color,col,row;
  int count;
  int oldrow, oldcol;
  double *p0,*p1;
  struct affine cvt;
  int ret;
  int xstep, ystep; /* The starting position step number */
  double xpixel, ypixel; /* Our pixel position on the screen */

  fp = open_orbitsave();
  /* setup affine screen coord conversion */
  setup_convert_to_screen(&cvt);

  p0 = &x;
  p1 = &y;


  count = 0;
  if(inside > 0)
     color = inside;
  if(color >= colors)
     color = 1;
  oldcol = oldrow = -1;

  xstep = -1;
  ystep = 0;

  if (resuming)
    {
    start_resume();
    get_resume(sizeof(count),&count, sizeof(color),&color,
    sizeof(oldrow),&oldrow, sizeof(oldcol),&oldcol,
    sizeof(x),&x, sizeof(y), &y, sizeof(xstep), &xstep,
    sizeof(ystep), &ystep, 0);
    end_resume();
    }

  ret = 0;
  while(1)
    {
    if(check_key() != 0)
      {
      alloc_resume(100,1);
      put_resume(sizeof(count),&count, sizeof(color),&color,
        sizeof(oldrow),&oldrow, sizeof(oldcol),&oldcol,
        sizeof(x),&x, sizeof(y), &y, sizeof(xstep), &xstep,
        sizeof(ystep), &ystep, 0);
      ret = -1;
      break;
      }
    xstep ++;
    if (xstep>=xmd)
      {
      xstep = 0;
      ystep ++;
      if (ystep>xmd)
        {
        ret = -1;
        break;
        }
      }
    xpixel = dx_size*(xstep+.5)/xmd;
    ypixel = dy_size*(ystep+.5)/xmd;
    x = (xxmin+delxx*xpixel) + (delxx2*ypixel);
    y = (yymax-delyy*ypixel) + (-delyy2*xpixel);
    if (fractype==MANDELCLOUD)
      {
      xma = x;
      xmb = y;
      }
    oldcol = -1;

    if (++color >= colors)   /* another color to switch to? */
      color = 1;  /* (don't use the background color) */

    for (count=0;count<maxit;count++)
      {
      col = cvt.a*x + cvt.b*y + cvt.e;
      row = cvt.c*x + cvt.d*y + cvt.f;
      if ( col >= 0 && col < xdots && row >= 0 && row < ydots )
        {
        if (count>=orbit_delay)
          {
          if(oldcol != -1 && doconnect)
            draw_line(col,row,oldcol,oldrow,color&(colors-1));
          else if(count > 0 || fractype != MANDELCLOUD)
            (*plot)(col,row,color&(colors-1));
          }
        oldcol = col;
        oldrow = row;
        }
      else if((long)abs(row) + (long)abs(col) > BAD_PIXEL) /* sanity check */
        return(ret);
      else
        oldrow = oldcol = -1;

      if(curfractalspecific->orbitcalc(p0, p1, NULL))
        break;
      if(fp)
        fprintf(fp,orbitsave_format,*p0,*p1,0.0);
      }
    }
  if(fp)
    fclose(fp);
  return(ret);
}

/* this function's only purpose is to manage funnyglasses related */
/* stuff so the code is not duplicated for ifs3d() and lorenz3d() */
int funny_glasses_call(int (*calc)())
{
  int status;
  status = 0;
  if(glassestype)
     whichimage = 1;
  else
     whichimage = 0;
  plot_setup();
  plot = standardplot;
  status = calc();
  if(realtime && glassestype != 3)
    {
    realtime = 0;
    return(status);
    }
  if(glassestype && status == 0 && display3d)
    {
    if(glassestype==3) /* photographer's mode */
      {
      static char firstready[]={"First (Left Eye) image is complete"};
      confirm(main_window, (char *) &firstready,
        OK_BTN, XmDIALOG_OK_BUTTON);
      clear_window();
      }
    whichimage = 2;
    plot_setup();
    plot = standardplot;
    /* is there a better way to clear the graphics screen ? */
    if((status = calc()))
      return(status);
    if(glassestype==3) /* photographer's mode */
      { /* dos version */
      static char secondready[]={"Second image (right eye) is ready"};
      confirm(main_window, (char *) &secondready,
        OK_BTN, XmDIALOG_OK_BUTTON);
      }
    }
  return(status);
}

/* double version - mainly for testing */
static int ifs3dfloat()
{
  int color_method;
  FILE *fp;
  unsigned long maxct;
  int color;

  double newx,newy,newz,r,sum;

  int k;
  int ret;

  struct float3dvtinf inf;

  float *ffptr;

  /* setup affine screen coord conversion */
  setup_convert_to_screen(&inf.cvt);
  srand(1);
  color_method = param[0];
  if(diskvideo)                /* this would KILL a disk drive! */
     notdiskmsg();

  inf.orbit[0] = 0;
  inf.orbit[1] = 0;
  inf.orbit[2] = 0;

  fp = open_orbitsave();

  maxct = maxit*40L;
  inf.ct = 0L;
  ret = 0;
  while(inf.ct++ < maxct) /* loop until keypress or maxit */
    {
    if( check_key() != 0 )  /* keypress bails out */
      {
      ret = -1;
      break;
      }
    r = rand15();   /* generate fudged random number between 0 and 1 */
    r /= 32767;

    /* pick which iterated function to execute, weighted by probability */
    sum = ifs_defn[12]; /* [0][12] */
    k = 0;
    while ( sum < r)
      {
      k++;
      sum += ifs_defn[k*IFS3DPARM+12];
      if (ifs_defn[(k+1)*IFS3DPARM+12] == 0)
        break; /* for safety */
      }

    /* calculate image of last point under selected iterated function */
    ffptr = ifs_defn + k*IFS3DPARM; /* point to first parm in row */
    newx = *ffptr * inf.orbit[0] +
      *(ffptr+1) * inf.orbit[1] +
      *(ffptr+2) * inf.orbit[2] + *(ffptr+9);
    newy = *(ffptr+3) * inf.orbit[0] +
      *(ffptr+4) * inf.orbit[1] +
      *(ffptr+5) * inf.orbit[2] + *(ffptr+10);
    newz = *(ffptr+6) * inf.orbit[0] +
      *(ffptr+7) * inf.orbit[1] +
      *(ffptr+8) * inf.orbit[2] + *(ffptr+11);

    inf.orbit[0] = newx;
    inf.orbit[1] = newy;
    inf.orbit[2] = newz;
    if(fp)
      fprintf(fp,orbitsave_format,newx,newy,newz);
    if (float3dviewtransf(&inf))
      {
      /* plot if inside window */
      if (inf.col >= 0)
        {
        if(realtime)
          whichimage=1;
        if(color_method)
          color = (k&(colors-1))+1;
        else
          color = getcolor(inf.col,inf.row)+1;
        if( color < colors ) /* color sticks on last value */
          (*plot)(inf.col,inf.row,color);
        }
      else if (inf.col == -2)
        return(ret);
      if(realtime)
        {
        whichimage=2;
        /* plot if inside window */
        if (inf.col1 >= 0)
          {
          if(color_method)
            color = (k&(colors-1))+1;
          else
            color = getcolor(inf.col1,inf.row1)+1;
          if( color < colors ) /* color sticks on last value */
            (*plot)(inf.col1,inf.row1,color);
          }
        else if (inf.col1 == -2)
          return(ret);
        }
      }
    } /* end while */
  if(fp)
    fclose(fp);
  return(ret);
}

int ifs()      /* front-end for ifs2d and ifs3d */
{
  if (ifs_defn == NULL && ifsload() < 0)
     return(-1);
  if(diskvideo)                /* this would KILL a disk drive! */
     notdiskmsg();
  return((ifs_type == 0) ? ifs2d() : ifs3d());
}


/* IFS logic shamelessly converted to integer math */
static int ifs2d()
{
  int color_method;
  FILE *fp;
  unsigned long maxct,ct;
  int col;
  int row;
  int color;
  int ret;
  long *localifs;
  long *lfptr;
  long x,y,newx,newy,r,sum, tempr;

  int i,j,k;
  struct l_affine cvt;
  /* setup affine screen coord conversion */
  l_setup_convert_to_screen(&cvt);

  srand(1);
  color_method = param[0];
  if((localifs=(long *)XtMalloc((long)numaffine*IFSPARM*sizeof(long)))==NULL)
    {
    return(-1);
    }

  for (i = 0; i < numaffine; i++)    /* fill in the local IFS array */
    for (j = 0; j < IFSPARM; j++)
      localifs[i*IFSPARM+j] = ifs_defn[i*IFSPARM+j] * fudge;

  tempr = fudge / 32767;   /* find the proper rand() fudge */

  fp = open_orbitsave();

  /* make maxct a function of screen size     */
  /* 1k times maxit at EGA resolution seems about right */
  maxct = (float)maxit*(1024.0*xdots*ydots)/(640.0*350.0);
  ct = 0L;
  x = y = 0;
  ret = 0;
  while(ct++ < maxct) /* loop until keypress or maxit */
    {
    if( check_key() != 0 )  /* keypress bails out */
      {
      ret = -1;
      break;
      }
    r = rand15();   /* generate fudged random number between 0 and 1 */
    r *= tempr;

    /* pick which iterated function to execute, weighted by probability */
    sum = localifs[6];  /* [0][6] */
    k = 0;
    while ( sum < r && k < numaffine-1) /* fixed bug of error if sum < 1 */
      sum += localifs[++k*IFSPARM+6];
    /* calculate image of last point under selected iterated function */
    lfptr = localifs + k*IFSPARM; /* point to first parm in row */
    newx = multiply(lfptr[0],x,bitshift) +
           multiply(lfptr[1],y,bitshift) + lfptr[4];
    newy = multiply(lfptr[2],x,bitshift) +
           multiply(lfptr[3],y,bitshift) + lfptr[5];
    x = newx;
    y = newy;
    if(fp)
      fprintf(fp,orbitsave_format,(double)newx/fudge,(double)newy/fudge,0.0);

    /* plot if inside window */
    col = (multiply(cvt.a,x,bitshift) +
           multiply(cvt.b,y,bitshift) + cvt.e) >> bitshift;
    row = (multiply(cvt.c,x,bitshift) +
           multiply(cvt.d,y,bitshift) + cvt.f) >> bitshift;
    if ( col >= 0 && col < xdots && row >= 0 && row < ydots )
      {
      /* color is count of hits on this pixel */
      if(color_method)
        color = (k&(colors-1))+1;
      else
        color = getcolor(col,row)+1;
      if( color < colors ) /* color sticks on last value */
        (*plot)(col,row,color);
      }
    else if((long)abs(row) + (long)abs(col) > BAD_PIXEL) /* sanity check */
      return(ret);
    }
  if(fp)
    fclose(fp);
  XtFree((char *) localifs);
  return(ret);
}

static int ifs3dlong()
{
  int color_method;
  FILE *fp;
  unsigned long maxct;
  int color;
  int ret;

  long *localifs;
  long *lfptr;
  long newx,newy,newz,r,sum, tempr;

  int i,j,k;

  struct long3dvtinf inf;
  srand(1);
  color_method = param[0];
  if((localifs=(long *)XtMalloc((long)numaffine*IFS3DPARM*sizeof(long)))==NULL)
    {
    return(-1);
    }

  /* setup affine screen coord conversion */
  l_setup_convert_to_screen(&inf.cvt);

  for (i = 0; i < numaffine; i++)    /* fill in the local IFS array */
    for (j = 0; j < IFS3DPARM; j++)
      localifs[i*IFS3DPARM+j] = ifs_defn[i*IFS3DPARM+j] * fudge;

  tempr = fudge / 32767;   /* find the proper rand() fudge */

  inf.orbit[0] = 0;
  inf.orbit[1] = 0;
  inf.orbit[2] = 0;

  fp = open_orbitsave();

  maxct = maxit*40L;
  inf.ct = 0L;
  ret = 0;
  while(inf.ct++ < maxct) /* loop until keypress or maxit */
    {
    if( check_key() != 0 )  /* keypress bails out */
      {
      ret = -1;
      break;
      }
    r = rand15();   /* generate fudged random number between 0 and 1 */
    r *= tempr;

    /* pick which iterated function to execute, weighted by probability */
    sum = localifs[12];  /* [0][12] */
    k = 0;
    while ( sum < r && ++k < numaffine*IFS3DPARM)
      sum += localifs[k*IFS3DPARM+12];

    /* calculate image of last point under selected iterated function */
    lfptr = localifs + k*IFS3DPARM; /* point to first parm in row */

    /* calculate image of last point under selected iterated function */
    newx = multiply(lfptr[0], inf.orbit[0], bitshift) +
           multiply(lfptr[1], inf.orbit[1], bitshift) +
           multiply(lfptr[2], inf.orbit[2], bitshift) + lfptr[9];
    newy = multiply(lfptr[3], inf.orbit[0], bitshift) +
           multiply(lfptr[4], inf.orbit[1], bitshift) +
           multiply(lfptr[5], inf.orbit[2], bitshift) + lfptr[10];
    newz = multiply(lfptr[6], inf.orbit[0], bitshift) +
           multiply(lfptr[7], inf.orbit[1], bitshift) +
           multiply(lfptr[8], inf.orbit[2], bitshift) + lfptr[11];

    inf.orbit[0] = newx;
    inf.orbit[1] = newy;
    inf.orbit[2] = newz;
    if(fp)
      fprintf(fp,orbitsave_format,(double)newx/fudge,
        (double)newy/fudge,(double)newz/fudge);

    if (long3dviewtransf(&inf))
      {
      if((long)abs(inf.row) + (long)abs(inf.col) > BAD_PIXEL) /* sanity check */
        return(ret);
      /* plot if inside window */
      if (inf.col >= 0)
        {
        if(realtime)
        whichimage=1;
        if(color_method)
          color = (k&(colors-1))+1;
        else
          color = getcolor(inf.col,inf.row)+1;
        if( color < colors ) /* color sticks on last value */
          (*plot)(inf.col,inf.row,color);
        }
      if(realtime)
        {
        whichimage=2;
        /* plot if inside window */
        if (inf.col1 >= 0)
          {
          if(color_method)
            color = (k&(colors-1))+1;
          else
            color = getcolor(inf.col1,inf.row1)+1;
          if( color < colors ) /* color sticks on last value */
            (*plot)(inf.col1,inf.row1,color);
          }
        }
      }
    }
  if(fp)
    fclose(fp);
  XtFree((char *) localifs);
  return(ret);
}

static void setupmatrix(MATRIX doublemat)
{
  /* build transformation matrix */
  identity (doublemat);

  /* apply rotations - uses the same rotation variables as line3d.c */
  xrot ((double)XROT / 57.29577,doublemat);
  yrot ((double)YROT / 57.29577,doublemat);
  zrot ((double)ZROT / 57.29577,doublemat);

  /* apply scale */

  /*
   * scale((double)XSCALE/100.0,(double)YSCALE/100.0,
   *   (double)ROUGH/100.0,doublemat);
   */
}

int orbit3dfloat()
{
  display3d = -1;
  if(0 < glassestype && glassestype < 3)
    realtime = 1;
  else
    realtime = 0;
  return(funny_glasses_call(orbit3dfloatcalc));
}

int orbit3dlong()
{
  display3d = -1;
  if(0 < glassestype && glassestype < 3)
    realtime = 1;
  else
    realtime = 0;
  return(funny_glasses_call(orbit3dlongcalc));
}

int ifs3d()
{
  display3d = -1;

  if(0 < glassestype && glassestype < 3)
    realtime = 1;
  else
    realtime = 0;
  if(floatflag)
    return(funny_glasses_call(ifs3dfloat)); /* double version of ifs3d */
  else
    return(funny_glasses_call(ifs3dlong));  /* long version of ifs3d   */
}

static int long3dviewtransf(struct long3dvtinf *inf)
{
  int i,j;
  double tmpx, tmpy, tmpz;
  long tmp;

  if (inf->ct == 1)  /* initialize on first call */
    {
    for(i=0;i<3;i++)
      {
      inf->minvals[i] =  1L << 30;
      inf->maxvals[i] = -inf->minvals[i];
      }
    setupmatrix(inf->doublemat);
    if(realtime)
      setupmatrix(inf->doublemat1);
    /* copy xform matrix to long for for fixed point math */
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        {
        inf->longmat[i][j] = inf->doublemat[i][j] * fudge;
        if(realtime)
          inf->longmat1[i][j] = inf->doublemat1[i][j] * fudge;
        }
    }

  /* 3D VIEWING TRANSFORM */
  longvmult(inf->orbit,inf->longmat,inf->viewvect,bitshift);
  if(realtime)
    longvmult(inf->orbit,inf->longmat1,inf->viewvect1,bitshift);

  if(inf->ct <= waste) /* waste this many points to find minz and maxz */
    {
    /* find minz and maxz */
    for(i=0;i<3;i++)
      if ((tmp = inf->viewvect[i]) < inf->minvals[i])
        inf->minvals[i] = tmp;
      else if (tmp > inf->maxvals[i])
        inf->maxvals[i] = tmp;

    if(inf->ct == waste) /* time to work it out */
      {
      inf->iview[0] = inf->iview[1] = 0L; /* center viewer on origin */

      /* z value of user's eye - should be more negative than extreme
      negative part of image */
      inf->iview[2] =
        (long)((inf->minvals[2]-inf->maxvals[2])*(double)ZVIEWER/100.0);

      /* center image on origin */
      tmpx = (-inf->minvals[0]-inf->maxvals[0])/(2.0*fudge); /* center x */
      tmpy = (-inf->minvals[1]-inf->maxvals[1])/(2.0*fudge); /* center y */

      /* apply perspective shift */
      tmpx += ((double)xshift*(xxmax-xxmin))/(xdots);
      tmpy += ((double)yshift*(yymax-yymin))/(ydots);
      tmpz = -((double)inf->maxvals[2]) / fudge;
      trans(tmpx,tmpy,tmpz,inf->doublemat);

      if(realtime)
        {
        /* center image on origin */
        tmpx = (-inf->minvals[0]-inf->maxvals[0])/(2.0*fudge); /* center x */
        tmpy = (-inf->minvals[1]-inf->maxvals[1])/(2.0*fudge); /* center y */

        tmpx += ((double)xshift1*(xxmax-xxmin))/(xdots);
        tmpy += ((double)yshift1*(yymax-yymin))/(ydots);
        tmpz = -((double)inf->maxvals[2]) / fudge;
        trans(tmpx,tmpy,tmpz,inf->doublemat1);
        }
      for(i=0;i<3;i++)
        view[i] = (double)inf->iview[i] / fudge;

      /* copy xform matrix to long for for fixed point math */
      for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
          {
          inf->longmat[i][j] = inf->doublemat[i][j] * fudge;
          if(realtime)
            inf->longmat1[i][j] = inf->doublemat1[i][j] * fudge;
          }
      }
    return(0);
    }

  /* inf->ct > waste */
  /* apply perspective if requested */
  if(ZVIEWER)
    {
    if(debugflag==22 || ZVIEWER < 100) /* use float for small persp */
      {
      /* use float perspective calc */
      VECTOR tmpv;
      for(i=0;i<3;i++)
        tmpv[i] = (double)inf->viewvect[i] / fudge;
      perspective(tmpv);
      for(i=0;i<3;i++)
        inf->viewvect[i] = tmpv[i]*fudge;
      if(realtime)
        {
        for(i=0;i<3;i++)
          tmpv[i] = (double)inf->viewvect1[i] / fudge;
        perspective(tmpv);
        for(i=0;i<3;i++)
          inf->viewvect1[i] = tmpv[i]*fudge;
        }
      }
    else
      {
      longpersp(inf->viewvect,inf->iview,bitshift);
      if(realtime)
        longpersp(inf->viewvect1,inf->iview,bitshift);
      }
    }

  /* work out the screen positions */
  inf->row = ((multiply(inf->cvt.c,inf->viewvect[0],bitshift) +
    multiply(inf->cvt.d,inf->viewvect[1],bitshift) +
    inf->cvt.f) >> bitshift) + yyadjust;
  inf->col = ((multiply(inf->cvt.a,inf->viewvect[0],bitshift) +
    multiply(inf->cvt.b,inf->viewvect[1],bitshift) +
    inf->cvt.e) >> bitshift) + xxadjust;
  if (inf->col < 0 || inf->col >= xdots || inf->row < 0 || inf->row >= ydots)
    {
    if((long)abs(inf->col)+(long)abs(inf->row) > BAD_PIXEL)
      inf->col= inf->row = -2;
    else
      inf->col= inf->row = -1;
    }
  if(realtime)
    {
    inf->row1 = ((multiply(inf->cvt.c,inf->viewvect1[0],bitshift) +
      multiply(inf->cvt.d,inf->viewvect1[1],bitshift) +
      inf->cvt.f) >> bitshift) + yyadjust1;
    inf->col1 = ((multiply(inf->cvt.a,inf->viewvect1[0],bitshift) +
      multiply(inf->cvt.b,inf->viewvect1[1],bitshift) +
      inf->cvt.e) >> bitshift) + xxadjust1;
    if (inf->col1 < 0 || inf->col1 >= xdots || inf->row1 < 0 ||
        inf->row1 >= ydots)
      {
      if((long)abs(inf->col1)+(long)abs(inf->row1) > BAD_PIXEL)
        inf->col1= inf->row1 = -2;
      else
        inf->col1= inf->row1 = -1;
      }
    }
  return(1);
}

static int float3dviewtransf(struct float3dvtinf *inf)
{
  int i;
  double tmpx, tmpy, tmpz;
  double tmp;

  if (inf->ct == 1)  /* initialize on first call */
    {
    for(i=0;i<3;i++)
      {
      inf->minvals[i] =  100000.0; /* impossible value */
      inf->maxvals[i] = -100000.0;
      }
    setupmatrix(inf->doublemat);
    if(realtime)
      setupmatrix(inf->doublemat1);
    }

  /* 3D VIEWING TRANSFORM */
  vmult(inf->orbit,inf->doublemat,inf->viewvect );
  if(realtime)
    vmult(inf->orbit,inf->doublemat1,inf->viewvect1);

  if(inf->ct <= waste) /* waste this many points to find minz and maxz */
    {
    /* find minz and maxz */
    for(i=0;i<3;i++)
      if ((tmp = inf->viewvect[i]) < inf->minvals[i])
        inf->minvals[i] = tmp;
      else if (tmp > inf->maxvals[i])
        inf->maxvals[i] = tmp;
    if(inf->ct == waste) /* time to work it out */
      {
      view[0] = view[1] = 0; /* center on origin */
      /*
       * z value of user's eye - should be more negative than extreme
       * negative part of image
       */
      view[2] = (inf->minvals[2]-inf->maxvals[2])*(double)ZVIEWER/100.0;

      /* center image on origin */
      tmpx = (-inf->minvals[0]-inf->maxvals[0])/(2.0); /* center x */
      tmpy = (-inf->minvals[1]-inf->maxvals[1])/(2.0); /* center y */

      /* apply perspective shift */
      tmpx += ((double)xshift*(xxmax-xxmin))/(xdots);
      tmpy += ((double)yshift*(yymax-yymin))/(ydots);
      tmpz = -(inf->maxvals[2]);
      trans(tmpx,tmpy,tmpz,inf->doublemat);

      if(realtime)
        {
        /* center image on origin */
        tmpx = (-inf->minvals[0]-inf->maxvals[0])/(2.0); /* center x */
        tmpy = (-inf->minvals[1]-inf->maxvals[1])/(2.0); /* center y */

        tmpx += ((double)xshift1*(xxmax-xxmin))/(xdots);
        tmpy += ((double)yshift1*(yymax-yymin))/(ydots);
        tmpz = -(inf->maxvals[2]);
        trans(tmpx,tmpy,tmpz,inf->doublemat1);
        }
      }
    return(0);
    }

  /* inf->ct > waste */
  /* apply perspective if requested */
  if(ZVIEWER)
    {
    perspective(inf->viewvect);
    if(realtime)
      perspective(inf->viewvect1);
    }
  inf->row = inf->cvt.c*inf->viewvect[0] + inf->cvt.d*inf->viewvect[1]
     + inf->cvt.f + yyadjust;
  inf->col = inf->cvt.a*inf->viewvect[0] + inf->cvt.b*inf->viewvect[1]
     + inf->cvt.e + xxadjust;
  if (inf->col < 0 || inf->col >= xdots || inf->row < 0 || inf->row >= ydots)
    {
    if((long)abs(inf->col)+(long)abs(inf->row) > BAD_PIXEL)
      inf->col= inf->row = -2;
    else
      inf->col= inf->row = -1;
    }
  if(realtime)
    {
    inf->row1 = inf->cvt.c*inf->viewvect1[0] + inf->cvt.d*inf->viewvect1[1]
      + inf->cvt.f + yyadjust1;
    inf->col1 = inf->cvt.a*inf->viewvect1[0] + inf->cvt.b*inf->viewvect1[1]
      + inf->cvt.e + xxadjust1;
    if (inf->col1 < 0 || inf->col1 >= xdots || inf->row1 < 0 ||
        inf->row1 >= ydots)
      {
      if((long)abs(inf->col1)+(long)abs(inf->row1) > BAD_PIXEL)
        inf->col1= inf->row1 = -2;
      else
        inf->col1= inf->row1 = -1;
      }
    }
  return(1);
}

static FILE *open_orbitsave()
{
  FILE *fp;
  if (orbitsave && (fp = fopen(orbitsavename,"w")))
    {
    fprintf(fp,"pointlist x y z color\n");
    return fp;
    }
  return NULL;
}

/* Plot a histogram by incrementing the pixel each time it it touched */
static void plothist(x, y, color)
int x,y,color;
{
  color = getcolor(x,y)+1;
  if (color<colors)
    putcolor(x,y,color);
}

static _LCMPLX ComplexSqrtLong(long x, long y)
{
  double    mag, theta;
  long      maglong, thetalong;
  _LCMPLX    result;

  mag       = sqrt(sqrt(((double) multiply(x,x,bitshift))/fudge +
            ((double) multiply(y,y,bitshift))/ fudge));
  maglong   = mag * fudge;
  theta     = atan2((double) y/fudge, (double) x/fudge)/2;
  thetalong = theta * SinCosFudge;

  SinCos086(thetalong, &result.y, &result.x);

  result.x  = multiply(result.x << (bitshift - 16), maglong, bitshift);
  result.y  = multiply(result.y << (bitshift - 16), maglong, bitshift);
  return result;
}

_CMPLX
ComplexSqrtFloat(double x, double y)
{
  double mag   = sqrt(sqrt(x*x + y*y));
  double theta = atan2(y, x) / 2;
  _CMPLX  result;

  FPUsincos(&theta, &result.y, &result.x);
  result.x *= mag;
  result.y *= mag;
  return result;
}

_CMPLX
DeQueueFloat()
{
  _CMPLX out;
  float outx, outy;

  if (ListBack != ListFront)
    {
    if (FromMemDisk(8*ListBack, sizeof(outx), &outx) &&
      FromMemDisk(8*ListBack +sizeof(outx), sizeof(outy), &outy))
      {
      ListBack = (ListBack + 1) % ListSize;
      out.x = outx;
      out.y = outy;
      lsize--;
      }
    return out;
    }
  out.x = 0;
  out.y = 0;
  return out;
}

static _CMPLX PopFloat()
{
  _CMPLX pop;
  float  popx, popy;

  if (!QueueEmpty())
    {
    ListFront--;
    if (ListFront < 0)
    ListFront = ListSize - 1;
    if (FromMemDisk(8*ListFront, sizeof(popx), &popx) &&
    FromMemDisk(8*ListFront +sizeof(popx), sizeof(popy), &popy))
      {
      pop.x = popx;
      pop.y = popy;
      --lsize;
      }
    return pop;
    }
  pop.x = 0;
  pop.y = 0;
  return pop;
}

static _LCMPLX DeQueueLong()
{
  _LCMPLX out;
  out.x = 0;
  out.y = 0;

  if (ListBack != ListFront)
    {
    if (FromMemDisk(8*ListBack, sizeof(out.x), &out.x) &&
      FromMemDisk(8*ListBack +sizeof(out.x), sizeof(out.y), &out.y))
      {
      ListBack = (ListBack + 1) % ListSize;
      lsize--;
      }
    return out;
    }
  out.x = 0;
  out.y = 0;
  return out;
}

static _LCMPLX PopLong()
{
  _LCMPLX pop;

  if (!QueueEmpty())
    {
    ListFront--;
    if (ListFront < 0)
      ListFront = ListSize - 1;
    if (FromMemDisk(8*ListFront, sizeof(pop.x), &pop.x) &&
        FromMemDisk(8*ListFront +sizeof(pop.x), sizeof(pop.y), &pop.y))
      --lsize;
    return pop;
    }
  pop.x = 0;
  pop.y = 0;
  return pop;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: lorenz.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.5  1995/04/18  15:59:59  darryl
 * Fix problems and incorporate enhancements from beta test.
 *  -- not able to zoom after loading a saved image.
 *  -- string parsing would fail because the tags were incorrect.
 *  -- CLK_TCK is determined from sysconf() if it is available.
 *  -- Julia sets were not calculated correctly after image save.
 *  -- some void pointers were not cast before promotion.
 *  -- Info About Image display says it was working on pass 2 of 1.
 *  -- an incomplete image could not be saved.
 *  -- a saved image was not drawn if the refresh rate was < 100.
 *  -- added Refresh Rate menu item and command line support.
 *  -- added Parameter Forwarding menu item and command line support.
 *  -- added fallback resources.
 *  -- enhanced parameter handling.
 *  -- implemented the Fractint zoombox 'panning' capability.
 *  -- enhanced the LogMap and LogTable handling.
 *  -- various typograhic error fixes.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.1  1995/03/11  03:44:45  tom
 * changed dxsize and dysize
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.301  1995/01/10  00:40:37  darryl
 * Add comments, delete references to obsolete code.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.103  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.102  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
 *
 * Revision 0.101  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
