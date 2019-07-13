
#ifndef xmfract_h
#define xmfract_h

#include <config.h>

#include <stdio.h>
#include <math.h>
#include <signal.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#include <ctype.h>

#ifdef HAVE_STRING_H
# include <string.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr (), *strrchr ();
#endif
#ifndef STDC_HEADERS
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif

#ifdef TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  ifdef HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif

#ifdef HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif

#include <sys/stat.h>
#include <sys/param.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/MainW.h>
#include <Xm/RepType.h>
#include <Xm/DrawingA.h>
#include <Xm/MenuShell.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/BulletinB.h>
#include <Xm/FileSB.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>
#include <Xm/Scale.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Mrm/MrmPublic.h>

#ifdef EDITRES_ENABLE
#include <X11/Xmu/Editres.h>
#endif

#ifndef HAVE_STRICMP
#  ifdef HAVE_STRCASECMP
#    define stricmp strcasecmp
#    define strnicmp strncasecmp
#  else
   int stricmp();
   int strnicmp();
#  endif
#endif

#ifdef __STDC__
#include <stdarg.h>
#define VA_START(args, lastarg) va_start(args, lastarg)
#else
#include <varargs.h>
#define VA_START(args, lastarg) va_start(args)
#endif

#include "keydefs.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"

#ifndef NO_IDENT
#ident "$Id: xmfract.h,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* version number for this release */
#define VERSION 1.4

/* max number of fractals */
#define MAXFRACTALS 200

/* max entries in par files */
#define MAXENTRIES 200

/* for cmdarg */
#define NONNUMERIC -32767

/* maximum number of Args */
#define MAX_ARGS 100

/* maximum single string length */
#define MAXTEST  100

/* path separator */
#define SLASHC  '/'

/* maximum number of parameters */
#define MAXPARAMS 10

/* for the GIF encoder */
#define MAXTEST   100   /* maximum single string length */
#define MAXSTRING 64000 /* total space reserved for strings */
                        /* maximum number of strings available */
#define MAXENTRY  5003  /* (a prime number is best for hashing) */

/* for saving parameters */
#define NICELINELEN 68
#define MAXLINELEN  72

/* default extension for saved images */
#define DEFAULTFRACTALTYPE ".gif"

/* default extension for saved images */
#define ALTERNATEFRACTALTYPE ".fra"

/* for determining parm value to update */
#define RGLRPARM 0
#define MOREPARM 1
#define TRIGPARM 2
#define BAILPARM 3
#define JIIMPARM 4

/* maximum number of parameters */
#define MAXJBPARAMS 16

/* maximum number of fractal types */
#define MAX_TYPES 100

/* maximum fractal type name length */
#define ITEMNAMELEN 40

/* number of trig functions */
#define NUMTRIGFN 17

/* reserve memory for jiim and 3d */
#define MEM_RESERVE 8192L

/* largest width of SaveRect/RestoreRect */
#define MAXRECT 1024

/* fudge factors */
#define FUDGEFACTOR 29
#define FUDGEFACTOR2 24

/* maximum number of items on the worklist */
#define MAXCALCWORK 12

/* maxxblk*maxyblk*2 <= 4096, the size of "prefix" */
#define maxyblk 7

/* each maxxblk is oversize by 2 for a "border" */
#define maxxblk 202

/* maximum number of pixels across or down the screen */
#define MAXPIXELS 2048

/* maximum depth of history saves */
#define MAXHISTORY 10

/* for Lsys rules */
#define MAXRULES 27

/* automatic inversion */
#define AUTOINVERT -123456.789

/* (pb: not sure if this is special or arbitrary) */
#define DEM_BAILOUT 535.5

/* maximum number of attributes */
#define N_ATTR 8

/* for GIF decoder */
#define OUT_OF_MEMORY -10
#define BAD_CODE_SIZE -20
#define READ_ERROR -1
#define WRITE_ERROR -2
#define OPEN_ERROR -3
#define CREATE_ERROR -4

#define MAX_CODES 4095

#define NOPE 0
#define YUP -1

/* for modal dialog button unmanagement */
#define OK_BTN       1
#define CANCEL_BTN   2
#define HELP_BTN     4

/*
 * for boundary trace method
 * (I have some ideas for the future with this. -Wes)
 */
#define bkcolor 0

/* only in "default.map" */
#define BLACK      0
#define GREENCOLOR 2
#define BROWNCOLOR 6
#define WHITE      7

/* Use these palette indices for red/blue */
#define PAL_BLUE    1
#define PAL_RED     2
#define PAL_MAGENTA 3

/* for Julia Inverse Method */
#define JIIM  0
#define ORBIT 1

/* for updating the create image menu */
#define NEW_IMAGE 0
#define CONT_CALC 1

/*
 * math definitions (normally in float.h)
 * that are missing on some systems.
 */

#ifndef FLT_MIN
# define FLT_MIN 1.17549435e-38
#endif

#ifndef FLT_MAX
# define FLT_MAX 3.40282347e+38
#endif

#ifndef DBL_EPSILON
# define DBL_EPSILON 2.2204460492503131e-16
#endif

/* milliseconds for calibration */
#define SLEEPINIT 250

/* maximum entries in the video table */
#define MAXVIDEOMODES 2
#define MAXVIDEOTABLE 2

/* max rotation for newtons */
#define NEWTONDEGREELIMIT 100

/*
 * filter value for bifurcation fractals
 *
 * "Beauty of Fractals" recommends using 5000
 * (p.25), but that seems unnecessary.
 * User can override this value with a nonzero param1
 */
#define DEFAULTFILTER 1000

/* starting value for bifurcation population */
#define SEED 0.66

/* also for bifurcation */
#define BIG 100000.0

/*
 * for frothy basin types
 *
 * these points were determined empirically and verified experimentally
 * using the program WL-Plot, a plotting program which has a mode for
 * orbits of recursive relations.
 *
 */

#define CLOSE    1e-6
#define SQRT3    1.732050807568877193
#define FROTH_A  1.02871376822
#define FROTH_B1 (FROTH_A/2)
#define FROTH_M2 SQRT3
#define FROTH_B2 (-FROTH_A)
#define FROTH_M3 (-SQRT3)
#define FROTH_B3 (-FROTH_A)
#define X1MIN   -1.04368901270
#define X1MAX    1.33928675524
#define XMIDT   -0.339286755220
#define X2MAX1   0.96729063460
#define XMIDR    0.61508950585
#define X3MIN1  -0.22419724936
#define X2MIN2  -1.11508950586
#define XMIDL   -0.27580275066
#define X3MAX2   0.07639837810

#define FROTH_BITSHIFT  28

/* for cellular types */
#define BAD_T          1
#define BAD_MEM        2
#define STRING1        3
#define STRING2        4
#define TABLEK         5
#define TYPEKR         6
#define RULELENGTH     7
#define CELLULAR_DONE 10

/* for Julia Inverse Method */
#define MEM_RESERVE 8192L
#define MAXRECT     1024
#define CURSOR_SIZE 5

/* for lorenz attractors */
#define XLAMBDA  param[0]
#define XALPHA   param[1]
#define XBETA    param[2]
#define XGAMMA   param[3]
#define XOMEGA   param[4]
#define XDEGREE  param[5]

/* for toggling the zoom functions */
#define ZOOM_ON  1
#define ZOOM_OFF 0

/* 3D stuff - formerly in 3d.h */

/* allow two of three points to be off screen */
#define MAXOFFSCREEN  2

#define CMAX 4 /* maximum column (4 x 4 matrix) */
#define RMAX 4 /* maximum row    (4 x 4 matrix) */
#define DIM  3 /* number of dimensions */

/* A matrix is used to describe a transformation from one coordinate
system to another.  Multiple transformations may be concatenated by
multiplying their transformation matrices. */

typedef double MATRIX [RMAX] [CMAX];  /* matrix of doubles */
typedef int   IMATRIX [RMAX] [CMAX];  /* matrix of ints    */
typedef long  LMATRIX [RMAX] [CMAX];  /* matrix of longs   */

/* A vector is an array of three coordinates [x,y,z] representing magnitude
and direction. A fourth dimension is assumed to always have the value 1, but
is not in the data structure */

typedef double VECTOR [DIM];  /* vector of doubles */
typedef int   IVECTOR [DIM];  /* vector of ints    */
typedef long  LVECTOR [DIM];  /* vector of longs   */

#define SPHERE    init3d[0]     /* sphere? 1 = yes, 0 = no  */
#define ILLUMINE  (FILLTYPE>4)  /* illumination model       */

/* regular 3D */
#define XROT      init3d[1]  /* rotate x-axis 60 degrees */
#define YROT      init3d[2]  /* rotate y-axis 90 degrees */
#define ZROT      init3d[3]  /* rotate x-axis  0 degrees */
#define XSCALE    init3d[4]  /* scale x-axis, 90 percent */
#define YSCALE    init3d[5]  /* scale y-axis, 90 percent */

/* sphere 3D */
#define PHI1      init3d[1]  /* longitude start, 180     */
#define PHI2      init3d[2]  /* longitude end ,   0      */
#define THETA1    init3d[3]  /* latitude start,-90 degrees */
#define THETA2    init3d[4]  /* latitude stop,  90 degrees */
#define RADIUS    init3d[5]  /* should be user input */

/* common parameters */
#define ROUGH     init3d[6]  /* scale z-axis, 30 percent */
#define WATERLINE init3d[7]  /* water level          */
#define FILLTYPE  init3d[8]  /* fill type            */
#define ZVIEWER   init3d[9]  /* perspective view point   */
#define XSHIFT    init3d[10] /* x shift */
#define YSHIFT    init3d[11] /* y shift */
#define XLIGHT    init3d[12] /* x light vector coordinate */
#define YLIGHT    init3d[13] /* y light vector coordinate */
#define ZLIGHT    init3d[14] /* z light vector coordinate */
#define LIGHTAVG  init3d[15] /* number of points to average */

/* number of ifs functions in ifs array */
#define NUMIFS 64

/* number of ifs parameters */
#define IFSPARM 7

/* number of ifs 3D parameters */
#define IFS3DPARM 13

/* for ifs and jiim */
#define SinCosFudge 0x10000L

/* trigonometric indexes */
#define SIN  0
#define COS  1
#define SINH 2
#define COSH 3
#define EXP  4
#define LOG  5
#define SQR  6

/* bailout defines */
#define FTRIGBAILOUT 2500.0
#define LTRIGBAILOUT   64.0
#define FROTHBAILOUT    6.0
#define STDBAILOUT      4.0
#define NOBAILOUT       0.0

/* bitmask defines for fractalspecific flags */
#define  NOZOOM         1    /* zoombox not allowed at all */
#define  NOGUESS        2    /* solid guessing not allowed */
#define  NOTRACE        4    /* boundary tracing not allowed */
#define  NOROTATE       8    /* zoombox rotate/stretch not allowed */
#define  NORESUME      16    /* can't interrupt and resume */
#define  INFCALC       32    /* this type calculates forever */
#define  TRIG1         64    /* number of trig functions in formula*/
#define  TRIG2        128
#define  TRIG3        192
#define  TRIG4        256
#define  WINFRAC      512    /* supported in WinFrac */
#define  PARMS3D     1024    /* uses 3d parameters */
#define  OKJB        2048    /* works with Julibrot */
#define  MORE        4096    /* more than 4 parms */

/* defines for symmetry */
#define  NOSYM          0
#define  XAXIS_NOPARM  -1
#define  XAXIS          1
#define  YAXIS_NOPARM  -2
#define  YAXIS          2
#define  XYAXIS_NOPARM -3
#define  XYAXIS         3
#define  ORIGIN_NOPARM -4
#define  ORIGIN         4
#define  PI_SYM_NOPARM -5
#define  PI_SYM         5
#define  XAXIS_NOIMAG  -6
#define  XAXIS_NOREAL   6
#define  NOPLOT        99
#define  SETUP_SYM    100

/* defines for inside/outside */
#define ITER        -1
#define REAL        -2
#define IMAG        -3
#define MULT        -4
#define SUM         -5
#define ZMAG       -59
#define BOF60      -60
#define BOF61      -61
#define EPSCROSS  -100
#define STARTRAIL -101
#define PERIOD    -102

/* obviously... */
#ifndef PI
# define PI 3.14159265358979323846
#endif

/* for 3d, etc. */
#define RANDOM(x)  (rand()%(x))

/* for formulas */
#define dShiftx dx1[row]
#define dShifty dy1[col]
#define lShiftx lx1[row]
#define lShifty ly1[col]

/* for parsing formulas */
#define MAX_OPS 250
#define MAX_STORES 128
#define MAX_LOADS  200

/* definitions for zoombox directions */
typedef enum {
  Center=0,
  NorthWest,
  North,
  NorthEast,
  East,
  SouthEast,
  South,
  SouthWest,
  West
} zoombox_dir_e;

/* for calc_status */
#define IMG_PRMS_INIT -1
#define IMG_PRMS_CHGD  0
#define IMG_ACTV_CALC  1
#define IMG_INTR_RESM  2
#define IMG_INTR_NRES  3
#define IMG_COMPLETED  4

/*
 * to cut off diverging orbits
 * (pixels can't get this big)
 */
#define BAD_PIXEL  10000L

#define PIXELROUND 0.00001

#ifndef RAND_MAX
#define RAND_MAX (2^31 - 1)
#endif

 /*
  * symmetry values in fractalspecificstuff struct
  *
  *  0 = no symmetry
  * -1 = y-axis symmetry (If No Params)
  *  1 = y-axis symmetry
  * -2 = x-axis symmetry (No Parms)
  *  2 = x-axis symmetry
  * -3 = y-axis AND x-axis (No Parms)
  *  3 = y-axis AND x-axis symmetry
  * -4 = polar symmetry (No Parms)
  *  4 = polar symmetry
  *  5 = PI (sin/cos) symmetry
  *  6 = NEWTON (power) symmetry
  *
  */

struct fractalspecificstuff
{
  char  *name;             /* name of the fractal */
                           /* (leading "*" supresses name display) */
  char  *param[4];         /* name of the parameters */
  double paramvalue[4];    /* default parameter values */
  int   helptext;          /* helpdefs.h HT_xxxx, -1 for none */
  int   helpformula;       /* helpdefs.h HF_xxxx, -1 for none */
  int   flags;             /* constraints, bits defined below */
  float xmin;              /* default XMIN corner */
  float xmax;              /* default XMAX corner */
  float ymin;              /* default YMIN corner */
  float ymax;              /* default YMAX corner */
  int   isinteger;         /* 1 if integerfractal, 0 otherwise */
  int   tojulia;           /* mandel-to-julia switch */
  int   tomandel;          /* julia-to-mandel switch */
  int   tofloat;           /* integer-to-floating switch */
  int   symmetry;          /* applicable symmetry logic */
  int (*orbitcalc)();      /* function that calculates one orbit */
  int (*per_pixel)();      /* once-per-pixel init */
  int (*per_image)();      /* once-per-image setup */
  int (*calctype)();       /* name of main fractal function */
  int orbit_bailout;       /* usual bailout value for orbit calc */
};

extern struct fractalspecificstuff fractalspecific[];

struct moreparms
{
  int type;                       /* index of the fractal name */
  char *param[MAXPARAMS-4];       /* name of the parameters */
  double paramvalue[MAXPARAMS-4]; /* default parameter values */
};

extern struct moreparms moreparams[];

struct choice
{
  char name[15];
  int  num;
};

/* history structure */
struct historystruct
{
  int fractype;           /* fractal type */
  double param[MAXPARAMS];/* parameters   */
  double xxmin;           /* top left */
  double yymax;           /* top left */
  double xxmax;           /* bottom right */
  double yymin;           /* bottom right */
  double xx3rd;           /* bottom left  */
  double yy3rd;           /* bottom left  */
};

/* work list entry for std escape time engines */
struct workliststuff
{
  int xxstart;    /* screen window for this entry */
  int xxstop;
  int yystart;
  int yystop;
  int yybegin;    /* start row within window, for 2pass/ssg resume */
  int sym;        /* if symmetry in window, prevents bad combines */
  int pass;       /* for 2pass and solid guessing */
};

struct fls
{
  void (*function)(void);
  union FracArg *operand;
};

struct videoinfo
{
  char name[26];
  char comment[26];
  int keynum;
  int videomodeax;
  int videomodebx;
  int videomodecx;
  int videomodedx;
  int dotmode;
  int xdots;
  int ydots;
  int colors;
};

struct affine
{
  /* weird order so a,b,e and c,d,f are vectors */
  double a;
  double b;
  double e;
  double c;
  double d;
  double f;
};

struct l_affine
{
  /* weird order so a,b,e and c,d,f are vectors */
  long a;
  long b;
  long e;
  long c;
  long d;
  long f;
};

struct long3dvtinf /* data used by 3d view transform subroutine */
{
  long ct;             /* iteration counter */
  long orbit[3];       /* interated function orbit value */
  long iview[3];       /* perspective viewer's coordinates */
  long viewvect[3];    /* orbit transformed for viewing */
  long viewvect1[3];   /* orbit transformed for viewing */
  long maxvals[3];
  long minvals[3];
  MATRIX doublemat;    /* transformation matrix */
  MATRIX doublemat1;   /* transformation matrix */
  long longmat[4][4];  /* long version of matrix */
  long longmat1[4][4]; /* long version of matrix */
  int row,col;         /* results */
  int row1,col1;
  struct l_affine cvt;
};

struct float3dvtinf /* data used by 3d view transform subroutine */
{
  long ct;             /* iteration counter */
  double orbit[3];     /* interated function orbit value */
  double viewvect[3];  /* orbit transformed for viewing */
  double viewvect1[3]; /* orbit transformed for viewing */
  double maxvals[3];
  double minvals[3];
  MATRIX doublemat;    /* transformation matrix */
  MATRIX doublemat1;   /* transformation matrix */
  int row,col;         /* results */
  int row1,col1;
  struct affine cvt;
};

enum stored_at_values
{
  NOWHERE,
  DISK,
  MEMORY
};

struct bopt
{
  int soundflag;
  char stdcalcmode;
  int usr_floatflag;
  int inside;
  int outside;
  int maxit;
  int LogFlag;
  int usr_biomorph;
  int decomp;
  int fillcolor;
  int orbit_delay;
};

struct basic_opts
{
  Widget one_pass_toggle;
  Widget two_pass_toggle;
  Widget three_pass_toggle;
  Widget guess_pass_toggle;
  Widget boundary_trace_toggle;
  Widget tessaral_pass_toggle;
  Widget maxiter_toggle;
  Widget zmag_toggle;
  Widget b_of_sixty_toggle;
  Widget b_of_sixty_one_toggle;
  Widget epscr_toggle;
  Widget star_toggle;
  Widget per_toggle;
  Widget inside_nnn_toggle;
  Widget inside_nnn_scale;
  Widget iter_toggle;
  Widget real_toggle;
  Widget imag_toggle;
  Widget mult_toggle;
  Widget summ_toggle;
  Widget outside_nnn_toggle;
  Widget outside_nnn_scale;
  Widget iteration_scale;
  Widget log_palette_scale;
  Widget biomorph_color_scale;
  Widget decomp_option_scale;
  Widget fill_color_scale;
  Widget orbit_delay_scale;
  struct bopt current;
  struct bopt dflt;
  struct bopt new;
};

struct eopt
{
  int finattract;
  double potparam0;
  double potparam1;
  double potparam2;
  int usr_distest;
  int distestwidth;
  double inversion0;
  double inversion1;
  double inversion2;
  int rotate_lo;
  int rotate_hi;
  int pot16bit;
  int periodicitycheck;
  int symmetry;
  int rseed;
  int showdot;
};

struct extended_opts
{
  Widget finite_attractor_scale;
  Widget potential_max_color_scale;
  Widget slope_textfield;
  Widget bailout_scale;
  Widget distance_estimator_scale;
  Widget width_factor_scale;
  Widget inversion_radius_textfield;
  Widget center_x_coord_textfield;
  Widget center_y_coord_textfield;
  Widget color_cycling_from_scale;
  Widget color_cycling_to_scale;
  Widget sixteen_bit_values_scale;
  Widget periodicity_textfield;
  Widget symmetry_textfield;
  Widget rseed_textfield;
  Widget showdot_textfield;
  struct eopt current;
  struct eopt dflt;
};

struct cnrval
{
  short usemag;
  double xxmin;
  double xxmax;
  double yymin;
  double yymax;
  double xx3rd;
  double yy3rd;
  double zzmin;
  double zzmax;
  double ttmin;
  double ttmax;
  double Xctr;
  double Yctr;
  double Mag;
};

struct corner_val
{
  struct cnrval current;
  struct cnrval dflt;
  struct cnrval new;
};

struct thrdopt
{
  char preview;
  char showbox;
  int previewfactor;
  char sphere;
  int glassestype;
  int ray;
  int brief;
  char *ray_name;
  int targa_out;
  int filltype;
  int phi1;
  int phi2;
  int theta1;
  int theta2;
  int radius;
  int xrot;
  int yrot;
  int zrot;
  int xscale;
  int yscale;
  int rough;
  int waterline;
  int zviewer;
  int xshift;
  int yshift;
  int xtrans;
  int ytrans;
  int transparent[2];
  int rndcolor;
  int eyeseparation;
  int xadjust;
  int red_crop_left;
  int red_crop_right;
  int blue_crop_left;
  int blue_crop_right;
  int red_bright;
  int blue_bright;
  int xlight;
  int ylight;
  int zlight;
  int lightavg;
  int ambient;
  int haze;
  int back_color[3];
  int targa_overlay;
};

struct three_d_opts
{
  Widget parms_x_rot_scale;
  Widget parms_y_rot_scale;
  Widget parms_z_rot_scale;
  Widget parms_perspective_scale;
  Widget parms_x_shift_scale;
  Widget parms_y_shift_scale;
  Widget parms_stereo_no_toggle;
  Widget parms_stereo_alt_toggle;
  Widget parms_stereo_photo_toggle;
  Widget parms_stereo_super_toggle;
  Widget preview_yes_toggle;
  Widget preview_no_toggle;
  Widget show_box_yes_toggle;
  Widget show_box_no_toggle;
  Widget sphere_yes_toggle;
  Widget sphere_no_toggle;
  Widget stereo_no_toggle;
  Widget stereo_alt_toggle;
  Widget stereo_photo_toggle;
  Widget stereo_super_toggle;
  Widget ray_trace_no_toggle;
  Widget ray_trace_dkb_toggle;
  Widget ray_trace_vivid_toggle;
  Widget ray_trace_shade_toggle;
  Widget ray_trace_raw_toggle;
  Widget ray_trace_acro_toggle;
  Widget ray_trace_mtv_toggle;
  Widget ray_trace_dxf_toggle;
  Widget brief_yes_toggle;
  Widget brief_no_toggle;
  Widget targa_yes_toggle;
  Widget targa_no_toggle;
  Widget coarseness_scale;
  Widget output_fname_textfield;
  Widget fill_type_surface_toggle;
  Widget fill_type_points_toggle;
  Widget fill_type_dots_toggle;
  Widget fill_type_sfc_clr_toggle;
  Widget fill_type_sfc_nclr_toggle;
  Widget fill_type_solid_toggle;
  Widget fill_type_light_toggle;
  Widget fill_type_light_b4_toggle;
  Widget fill_type_light_after_toggle;
  Widget planar_long_start_scale;
  Widget planar_long_stop_scale;
  Widget planar_lat_start_scale;
  Widget planar_lat_stop_scale;
  Widget planar_radius_pct_scale;
  Widget planar_x_rot_scale;
  Widget planar_y_rot_scale;
  Widget planar_z_rot_scale;
  Widget planar_x_scale_scale;
  Widget planar_y_scale_scale;
  Widget planar_rough_scale;
  Widget planar_water_scale;
  Widget planar_perspective_scale;
  Widget planar_x_shift_scale;
  Widget planar_y_shift_scale;
  Widget planar_x_adjust_scale;
  Widget planar_y_adjust_scale;
  Widget planar_first_color_scale;
  Widget planar_last_color_scale;
  Widget planar_random_color_scale;
  Widget interocular_distance_scale;
  Widget convergence_adjust_scale;
  Widget left_red_crop_scale;
  Widget right_red_crop_scale;
  Widget left_blue_crop_scale;
  Widget right_blue_crop_scale;
  Widget red_brightness_scale;
  Widget blue_brightness_scale;
  Widget xlight_scale;
  Widget ylight_scale;
  Widget zlight_scale;
  Widget lightavg_scale;
  Widget ambient_scale;
  Widget haze_scale;
  Widget bg_red_scale;
  Widget bg_green_scale;
  Widget bg_blue_scale;
  Widget targa_overlay_yes_toggle;
  Widget targa_overlay_no_toggle;
  struct thrdopt current;
  struct thrdopt dflt;
};

struct svprm
{
  Widget comment_1_textfield;
  Widget comment_2_textfield;
  Widget comment_3_textfield;
  Widget comment_4_textfield;
  Widget record_color_yes_toggle;
  Widget record_color_no_toggle;
  Widget record_color_file_toggle;
  char writeflag[3];
  char mapfile_pointer_name[64];
  char commandname[64];
  char parmfilename[64];
  struct write_batch_data
    {
    int len;
    char buf[513];
    } wb_data, *wbdata;
};

struct strfld
{
  double starfield_values[4];
};

struct strfld_opts
{
  Widget starfield_density_scale;
  Widget starfield_clumpiness_scale;
  Widget starfield_dim_ratio_scale;
  struct strfld current;
  struct strfld dflt;
};

struct vwin
{
  int viewwindow;
  int viewcrop;
  int viewxdots;
  int viewydots;
  float viewreduction;
  float finalaspectratio;
};

struct viewwin_opts
{
  Widget crop_yes_toggle;
  Widget crop_no_toggle;
  Widget preview_yes_toggle;
  Widget preview_no_toggle;
  Widget auto_reduction_textfield;
  Widget final_aspect_textfield;
  Widget viewxdots_scale;
  Widget viewydots_scale;
  struct vwin current;
  struct vwin dflt;
};

struct window_presets
{
  int width_addition;
  int height_addition;
  int new_width_addition;
  int new_height_addition;
  int dis_large_width;
  int dis_large_height;
  int dis_small_width;
  int dis_small_height;
  int dis_tiny_width;
  int dis_tiny_height;
  int display_width;
  int display_height;
  int min_width;
  int min_height;
  int max_width;
  int max_height;
  int vis_min_width;
  int vis_min_height;
  int vis_max_width;
  int vis_max_height;
  int aspect_width;
  int aspect_height;
  int new_width;
  int new_height;
  int oversize;
  Window rootw;
  unsigned int width_return;
  unsigned int height_return;
  unsigned int border_width_return;
  unsigned int depth_return;
  unsigned int new_width_return;
  unsigned int new_height_return;
};

struct palette_rect
{
  int x;
  int y;
  int width;
  int height;
  int intred;
  int intgreen;
  int intblue;
  unsigned short red;
  unsigned short green;
  unsigned short blue;
  unsigned long color;
};

struct freestyle_stuff
{
  int center;
  int range_stop;
  int range_start;
  int wraparound;
  int range;
  int color;
  int endcolor_hi;
  int endcolor_lo;
  int gamma;
  int oldcolor;
  int transparency;
  float topmult;
  float botmult;
  int dirty[256];
  int active[256];
  struct palette_rect gamma_rect[256];
};

struct canvas_cursor
{
  int old_x;
  int old_y;
  int active;
};

struct palette_range
{
  int start_index;
  int stop_index;
  int num_saved;
  int skip_val;
  Widget skipval_scale;
  struct palette_rect saved[256];
};

struct editpal_stuff
{
  Widget edit_palette_dialog;
  Widget primary_register_frame;
  Widget primary_register_form;
  Widget primary_red_scale;
  Widget primary_green_scale;
  Widget primary_blue_scale;
  Widget primary_active_button;
  Widget primary_active_colorbox;
  Widget primary_colorbox_label;
  Widget secondary_register_frame;
  Widget secondary_register_form;
  Widget secondary_red_scale;
  Widget secondary_green_scale;
  Widget secondary_blue_scale;
  Widget secondary_active_button;
  Widget secondary_active_colorbox;
  Widget secondary_colorbox_label;
  Widget palette_colorbox;
  GC primary_active_colorbox_gc;
  GC secondary_active_colorbox_gc;
  GC palette_colorbox_gc;
  XGCValues xgc_values;
  int active_color_register;
  int active_color_register_color;
  int inactive_color_register_color;
  int oldrect;
  int popped_up;
  int range_mode;
  int freestyle_mode;
  int inactive_frozen;
  int undo_index;
  int redo_index;
  double gamma_val;
  struct palette_range pal_range;
  struct palette_rect palette_colorbox_rect[256];
  struct canvas_cursor editpal_canvas_cursor;
};

#define FP_SEG(x) 0
#define FP_OFF(x) ((char *)(x))
#define segread(x)

struct point
{
  int x;
  int y;
  int color;
};

struct f_point
{
  float x;
  float y;
  float color;
};

struct minmax
{
  int minx;
  int maxx;
};

struct Perspective
{
  long x, y, zx, zy;
};

struct Perspectivefp
{
  double x, y, zx, zy;
};

struct coords
{
  int x,y;
};

/* one of these per box to be done gets stacked */
struct tess
{
  int x1,x2,y1,y2;      /* left/right top/bottom x/y coords  */
  int top,bot,lft,rgt;  /* edge colors, -1 mixed, -2 unknown */
};

struct trig_funct_lst
{
  char *name;
  void (*lfunct)(void);
  void (*dfunct)(void);
  void (*mfunct)(void);
};

struct parmtype
{
  int type;
  int parm;
};

struct lsys_cmd
{
  char ch;
  void (*f)(long n);
  long n;
};

struct entryinfo
{
  char name[ITEMNAMELEN+1];
  long point;
};

struct rotate_stuff
{
  int running;
  int direction;
  int interval;
  int onestep;
  int fkey;
};

struct rubber_band_data
{
  Position start_x;
  Position start_y;
  Position last_x;
  Position last_y;
  Position tl_x;
  Position tl_y;
  Position br_x;
  Position br_y;
  Position h_tl_x;
  Position h_tl_y;
  Position h_tr_x;
  Position h_tr_y;
  Position h_bl_x;
  Position h_bl_y;
  Position h_br_x;
  Position h_br_y;
  Position v_tl_x;
  Position v_tl_y;
  Position v_tr_x;
  Position v_tr_y;
  Position v_bl_x;
  Position v_bl_y;
  Position v_br_x;
  Position v_br_y;
  int width;
  int height;
  int boxisactive;
  int moving;
  int panning;
  int stretching;
  int snap_to_aspect;
  zoombox_dir_e stretch_dir;
};

struct PEND_OP
{
  void (*f)(void);
  int p;
};

struct FNCT_LIST
{
  char *s;
  void (**ptr)(void);
};

struct SYMETRY
{
  char *s;
  int n;
};

typedef struct palett
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} Palettetype;

#define INFO_ID        "Fractal"
#define FRACTAL_INFO   struct fractal_info

/*
 * Note: because non-MSDOS machines store structures differently, we have
 * to do special processing of the fractal_info structure in loadfile.c.
 * Make sure changes to the structure here get reflected there.
 */

/* This value should be the MSDOS size, not the Unix size. */
#define FRACTAL_INFO_SIZE 502

struct fractal_info        /*  for saving data in GIF file     */
{
  char  info_id[8];        /* Unique identifier for info block */
  short iterations;
  short fractal_type;      /* 0=Mandelbrot 1=Julia 2= ... */
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double creal;
  double cimag;
  short videomodeax;
  short videomodebx;
  short videomodecx;
  short videomodedx;
  short dotmode;
  short xdots;
  short ydots;
  short colors;
  short version;           /* used to be 'future[0]' */
  float parm3;
  float parm4;
  float potential[3];
  short rseed;
  short rflag;
  short biomorph;
  short inside;
  short logmap;
  float invert[3];
  short decomp[2];
  short symmetry;

  /* version 2 stuff */

  short init3d[16];
  short previewfactor;
  short xtrans;
  short ytrans;
  short red_crop_left;
  short red_crop_right;
  short blue_crop_left;
  short blue_crop_right;
  short red_bright;
  short blue_bright;
  short xadjust;
  short eyeseparation;
  short glassestype;

  /* version 3 stuff, release 13 */

  short outside;

  /* version 4 stuff, release 14 */

  double x3rd;               /* 3rd corner */
  double y3rd;
  char stdcalcmode;          /* 1/2/g/b */
  char useinitorbit;         /* init Mandelbrot orbit flag */
  short calc_status;         /* resumable, finished, etc */
  long tot_extend_len;       /* total length of extension blocks in .gif file */
  short distest;
  short floatflag;
  short bailout;
  long calctime;
  unsigned char trigndx[4];  /* which trig functions selected */
  short finattract;
  double initorbit[2];       /* init Mandelbrot orbit values */
  short periodicity;         /* periodicity checking */

  /* version 5 stuff, release 15 */

  short pot16bit;            /* save 16 bit continuous potential info */
  float faspectratio;        /* finalaspectratio, y/x */
  short system;              /* 0 for dos, 1 for windows */
  short release;             /* release number, with 2 decimals implied */
  short flag3d;              /* stored only for now, for future use */
  short transparent[2];
  short ambient;
  short haze;
  short randomize;

  /* version 6 stuff, release 15.x */

  short rotate_lo;
  short rotate_hi;
  short distestwidth;

  /* version 7 stuff, release 16 */

  double dparm3;
  double dparm4;

  /* version 8 stuff, release 17 */

  short fillcolor;

  /* version 9 stuff, release 18 */

  double mxmaxfp;
  double mxminfp;
  double mymaxfp;
  double myminfp;
  short zdots;
  float originfp;
  float depthfp;
  float heightfp;
  float widthfp;
  float distfp;
  float eyesfp;
  short orbittype;
  short juli3Dmode;
  short maxfn;
  short inversejulia;
  double dparm5;
  double dparm6;
  double dparm7;
  double dparm8;
  double dparm9;
  double dparm10;
  short future[50];            /* for stuff we haven't thought of yet */
};

#endif /* xmfract_h */

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: xmfract.h,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.21  1995/06/14  01:30:07  darryl
 * Add color editor undo-last-command and undo-last-redo and
 * command-line and ini file support for max undo commands.
 *
 * Revision 1.20  1995/06/12  16:07:19  darryl
 * Disable color cycling when palette editor freestyle
 * mode is active. Re-enable when exiting freestyle mode.
 *
 * Revision 1.19  1995/06/05  16:30:14  tom
 * File handling enhancements. Remove DOS-specific code.
 *
 * Revision 1.18  1995/05/21  09:54:04  darryl
 * Change palette editor and color cycling structures.
 *
 * Revision 1.17  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.16.1.1  1995/06/05  14:11:18  tom
 * changed filename handling
 *
 * Revision 1.16  1995/05/19  08:11:11  darryl
 * Add freestyle wraparound feature.
 *
 * Revision 1.15  1995/05/16  18:02:13  darryl
 * Increment program version to 1.4 prior to release.
 *
 * Revision 1.14  1995/05/02  23:12:55  darryl
 * Enable freestyle option, fix various bugs in the color editor.
 *
 * Revision 1.13  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.12  1995/04/18  15:59:59  darryl
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
 * Revision 1.11  1995/04/08  19:02:44  tom
 * Change the order of definitions for portability.
 *
 * Revision 1.10  1995/04/04  00:05:05  darryl
 * Set MAX_STORES to an even number to force alignment.
 *
 * Revision 1.9  1995/03/31  06:49:22  darryl
 * Fix window size preset values.
 *
 * Revision 1.8  1995/03/28  18:04:20  tom
 * Enhance initialization file handling. Revise stackavail()
 * so that it uses getrlimit if available. Enhance configuration
 * files to check for getrlimit and associated header files.
 *
 * Revision 1.7  1995/03/25  08:42:49  darryl
 * Fixes and enhancements -- make the parameter selection
 * understand that if a parameter file is loaded, then
 * the ifs, lsys, or formula file does not need to be
 * re-selected. Display the current fractal type and
 * any associated file and file entries at the top of
 * the parameters screen. Delete some unused code.
 *
 * Revision 1.6  1995/03/23  22:16:06  darryl
 * Add ScrolledW.h inclusion.
 *
 * Revision 1.5  1995/03/22  18:08:22  darryl
 * Prevent crashes when recursive calls are made to dialog
 * creation routines, enlarge file name and path variable
 * sizes to at least 256 bytes, don't pop down the parent
 * of a warning or caution dialog in the case of a list
 * or file selection dialog mis-match.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.3.1.5  1995/03/14  15:34:24  tom
 * changed #if to #ifdef
 *
 * Revision 1.3.1.4  1995/03/14  09:31:55  tom
 * enumeration changes
 *
 * Revision 1.3.1.3  1995/03/13  18:43:30  tom
 * added enum type calc_status_e
 * added enum type zoombox_dir_e
 * new strnicmp define
 *
 * Revision 1.3.1.2  1995/03/13  14:54:05  tom
 * portability changes
 *
 * Revision 1.3.1.1  1995/03/11  03:30:10  tom
 * changed varargs handling
 * new <string.h> and <time.h> include
 * define strcasecmp for stricmp if available
 * conditionalize Editres.h include with EDITRES_ENABLE
 *
 * Revision 1.3  1995/03/10  19:35:58  darryl
 * Add signal handler definition.
 *
 * Revision 1.2  1995/03/04  01:06:42  darryl
 * Increment the release version to 1.2
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.303  1995/02/28  18:57:59  darryl
 * Linux modifications.
 *
 * Revision 0.302  1995/01/17  00:03:30  darryl
 * Add a disclaimer dialog.
 *
 * Revision 0.301  1994/12/21  19:23:20  darryl
 * Color editor enhancements - add a color index indicator
 * and make the active and inactive color register labels
 * dynamic, reflecting the state of the register.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.202  1994/12/13  17:08:17  darryl
 * Move some definitions from the Makefile to xmcomfig.h for gcc.
 *
 * Revision 0.201  1994/12/07  00:31:30  darryl
 * Change calls to rubber banding functions from a passive
 * pointer grab to translations from the canvas widget.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.109  1994/12/01  20:35:03  darryl
 * Add periodicity checking, symmetry, rseed, and showdot to
 * the extended options dialog so they do not have to be
 * set only on the command line or in a parameter file.
 *
 * Revision 0.108  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.107  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.106  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.105  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.104  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.103  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
 *
 * Revision 0.102  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.101  1994/11/23  05:21:08  darryl
 * Initial implementation of help. Minor widget alignment
 * changes. Widget name changes for uniqueness.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
