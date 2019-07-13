
#include "xmfract.h"
#include "fractype.h"
#include "helpdefs.h"
#include "macros.h"
#include "mpmath.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: globals.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

Colormap colormap;

Dimension winheight;
Dimension winwidth;

Display *display;

FILE *fp;
FILE *out;

GC gc;
GC xgc;

MATH_TYPE MathType;

MATRIX mtrix;

Screen *screen;

VECTOR cross;
VECTOR view;

Visual *visual;

Widget canvas;
Widget draw_fractal_menu_btn1;
Widget explicit_x_size_scale;
Widget explicit_y_size_scale;
Widget main_window;
Widget menu_bar;
Widget misc_menu_btn[5];
Widget param_textfield[MAXPARAMS];
Widget toplevel;

Window window;

XColor cols[256];

XImage *image;

XtAppContext app;

short calc_status;

_CMPLX *floatparm;
_CMPLX *roots;
_CMPLX Coefficient;
_CMPLX attr[N_ATTR];
_CMPLX cdegree;
_CMPLX coefficient;
_CMPLX croot;
_CMPLX init;
_CMPLX initorbit;
_CMPLX new;
_CMPLX old;
_CMPLX parm2;
_CMPLX parm;
_CMPLX pwr;
_CMPLX saved;
_CMPLX temp;
_CMPLX tmp;

_LCMPLX lattr[N_ATTR];
_LCMPLX linit;
_LCMPLX linitorbit;
_LCMPLX lnew;
_LCMPLX lold;
_LCMPLX lparm2;
_LCMPLX lparm;

char *black_str;
char *bof60_str;
char *bof61_str;
char *curtrigndx_str[4];
char *epscr_str;
char *extraseg;
char *fractdir;
char *geometry;
char *helpdir;
char *imag_str;
char *iter_str;
char *juli3Doptions[4];
char *maxiter_str;
char *mult_str;
char *newtrigndx_str[4];
char *per_str;
char *real_str;
char *resume_info;
char *scrnfile;
char *star_str;
char *summ_str;
char *zmag_str;

char CommandFile[MAXPATHLEN];
char CommandName[80];
char FormFileName[MAXPATHLEN];
char FormName[80];
char Glasses1Map[MAXPATHLEN];
char GreyFile[MAXPATHLEN];
char IFSFileName[MAXPATHLEN];
char IFSName[80];
char LFileName[MAXPATHLEN];
char LName[80];
char MAP_name[MAXPATHLEN];
char PAR_name[MAXPATHLEN];
char ZoomName[MAXPATHLEN];
char angle;
char boxx[9216];
char boxy[4096];
char xmbusy;
char colorfile[MAXPATHLEN];
char counter;
char floatflag;
char initcorners;
char light_name[MAXPATHLEN];
char loaded;
char mapfilename[MAXPATHLEN];
char oldFormFileName[MAXPATHLEN];
char oldFormName[80];
char oldIFSFileName[MAXPATHLEN];
char oldIFSName[80];
char oldLFileName[MAXPATHLEN];
char oldLName[80];
char overwrite_flag;
char preview;
char ray_name[MAXPATHLEN];
char readname[MAXPATHLEN];
char restore_which[20][4];
char reverse;
char savename[MAXPATHLEN];
char showbox;
char stdcalcmode;
char useinitorbit;
char usr_floatflag;
char usr_stdcalcmode;

double *dx0;
double *dx1;
double *dy0;
double *dy1;

double Cx;
double Cy;
double Population;
double Rate;
double xma;
double aspect;
double xmb;
double xmc;
double closenuff;
double cosx;
double cosy;
double curparam[MAXPARAMS];
double xmd;
double ddelmin;
double defparam[MAXPARAMS];
double deltaX;
double deltaY;
double delxx2;
double delxx;
double delyy2;
double delyy;
double dem_delta;
double dem_toobig;
double dem_width;
double depthfp;
double dinitorbit[3];
double distfp;
double dt;
double dx;
double dx_size;
double dy;
double dy_size;
double dz;
double eyesfp;
double f_at_rad;
double f_radius;
double f_xcenter;
double f_ycenter;
double xmfg;
double floatmax;
double floatmin;
double heightfp;
double initx;
double inity;
double inversion[3];
double magnitude;
double mxmaxfp;
double mxminfp;
double mymaxfp;
double myminfp;
double orbit;
double originfp;
double param[MAXPARAMS];
double plotmx1;
double plotmx2;
double plotmy1;
double plotmy2;
double potparam[3];
double qc;
double qci;
double qcj;
double qck;
double rqlim2;
double rqlim;
double rqlim_save;
double sinx;
double siny;
double starfield_values[4];
double sx3rd;
double sxmax;
double sxmin;
double sy3rd;
double symax;
double symin;
double t2;
double tempsqrx;
double tempsqry;
double threshold;
double ttmax;
double ttmin;
double twopi;
double widthfp;
double xmxt;
double xx3rd;
double xxmax;
double xxmin;
double xmyt;
double yy3rd;
double yymax;
double yymin;
double zbx;
double zby;
double zdepth;
double zskew;
double zwidth;
double zzmax;
double zzmin;

float *ifs_defn;

float finalaspectratio;
float screenaspect;
float version;
float viewreduction;

int (*calctype)();
int (*dotread)(int, int);
int (*outln)(unsigned char *,unsigned);

int *ranges;
int *save_orbit;

int Ambient;
int AntiAliasing;
int AplusOne;
int BRIEF;
int CO;
int CO_MAX;
int Distribution;
int LastInitOp;
int LodPtr;
int LogFlag;
int MaxColorRes;
int Offset;
int OpPtr;
int XMP;
int RANDOMIZE;
int RAY;
int RO;
int Real_V;
int Shadowing;
int ShiftBack;
int Slope;
int SolidCore;
int StoPtr;
int TPlusFlag;
int T_Safe;
int T_header_24;
int Targa_Out;
int Targa_Overlay;
int TranspSymmetry;
int Transparent3D;
int abort_process;
int altkey;
int andcolor;
int attractors;
int attrperiod[N_ATTR];
int bad_check;
int bad_code_count;
int bad_value;
int bailout;
int basin;
int bbase;
int biomorph;
int bitshift;
int bitshiftless1;
int blue_bright;
int blue_crop_left;
int blue_crop_right;
int bottom_guess;
int boxcount;
int c_exp;
int col;
int color;
int color_bright;
int color_dark;
int colorpreloaded;
int colors;
int colorstate;
int creditscreen;
int ctrlkey;
int curcol;
int curpass;
int currow;
int curtype;
int debugflag;
int decomp[2];
int degree;
int disclaimer;
int disk16bit;
int diskflag;
int diskisactive;
int diskvideo;
int display3d;
int distest;
int distestwidth;
int dither_flag;
int doconnect;
int doredraw;
int editpal_cursor;
int xmerror;
int escape_exit;
int euler;
int eyeseparation;
int filecolors;
int filetype;
int filexdots;
int fileydots;
int fillcolor;
int finattract;
int forcesymmetry;
int forwarding;
int fractype;
int functionpreloaded;
int gif87a_flag;
int glassestype;
int got_status;
int gotfile;
int gotfrmfile;
int gotfrmname;
int gotifsfile;
int gotifsname;
int gotlsysfile;
int gotlsysname;
int gotrealdac;
int guessplot;
int halfblock;
int hasinverse;
int haze;
int helpmode;
int histocycle;
int historyflag;
int historymax;
int historyptr;
int ifs_type;
int init3d[20];
int initbatch;
int initmode;
int inside;
int inside_color;
int integerfractal;
int invert;
int ipixtab[256];
int ixstart;
int ixstop;
int iystart;
int iystop;
int jiiming;
int juli3Dmode;
int julibrot;
int kbdcount;
int keybuf;
int keybuffer;
int last_colorbar;
int lastorbittype;
int loaded3d;
int lstscreenflag;
int mapset;
int max_colors;
int max_kbdcount;
int maxblock;
int maxcolor;
int maxhits;
int maxit;
int metakey;
int mono;
int neworbittype;
int num_worklist;
int numaffine;
int numaffine;
int numtrigfn;
int numtypes;
int nxtscreenflag;
int ok_to_cycle;
int oldcolor;
int oldtype;
int oldx;
int oldy;
int orbit_color;
int orbit_delay;
int orbit_ptr;
int orbitsave;
int outcolor1s;
int outcolor2s;
int outside;
int overflow;
int overlay3d;
int paren;
int periodicitycheck;
int persp;
int pixelpi;
int plasma_check;
int pot16bit;
int potflag;
int previewfactor;
int privatecolor;
int projection;
int randshell_popped_up;
int rangeslen;
int realcolor;
int realtime;
int red_bright;
int red_crop_left;
int red_crop_right;
int refresh_rate;
int refresh_timeout;
int release;
int resave_flag;
int reset_periodicity;
int resized_by_user;
int resize_mode;
int resume_len;
int resuming;
int rflag;
int right_guess;
int root;
int rotate_hi;
int rotate_lo;
int row;
int rowcount;
int rseed;
int save16bit;
int save_release;
int save_system;
int screen_number;
int set_defaults;
int sharecolor;
int shiftkey;
int show_orbit;
int showdot;
int showfile;
int skipxdots;
int skipydots;
int slides;
int soundflag;
int started_resaves;
int sxdots;
int sxoffs;
int sydots;
int symmetry;
int syoffs;
int xmt;
int tabmode;
int timerflag;
int totpasses;
int tpdepth;
int tptime;
int transparent[2];
int undo_max;
int usepixtab;
int using_jiim;
int usr_biomorph;
int usr_distest;
int usr_periodicitycheck;
int usr_randmap_func;
int usr_randmap_mode;
int verbose;
int viewcrop;
int viewwindow;
int viewxdots;
int viewydots;
int waste;
int whichimage;
int workpass;
int worksym;
int xadjust;
int xbase;
int xdots;
int xlastcolor;
int xshift1;
int xshift;
int xtrans;
int xxadjust1;
int xxadjust;
int xxstart;
int xxstop;
int yadjust;
int ybase;
int ydots;
int yshift1;
int yshift;
int ytrans;
int yyadjust;
int yybegin;
int yystart;
int yystop;
int zdots;
int zoomcolor_index;
int zoomoff;
int zrotate;

long *lx0;
long *lx1;
long *ly0;
long *ly1;

long ListBack;
long ListFront;
long ListSize;
long calctime;
long cimag;
long con;
long creal;
long delmin;
long delx;
long dely;
long dist;
long djx;
long djy;
long eyes;
long fudge;
long jx;
long jy;
long l16triglim;
long l_at_rad;
long lclosenuff;
long llimit2;
long llimit;
long lmagnitud;
long lmax;
long lsize;
long lt2;
long ltempsqrx;
long ltempsqry;
long mx;
long mxmin;
long my;
long mymin;
long sins[50];
long timer_interval;
long width;
long x3rd;
long xmax;
long xmin;
long xoffset;
long xpixel;
long xpos;
long y3rd;
long ymax;
long ymin;
long yoffset;
long ypixel;
long ypos;

short maxfn;
short usemag;

struct ConstArg *xmv;
struct PEND_OP *xmo;
struct affine cvt;
struct basic_opts basic_opts;
struct corner_val corner_val;
struct editpal_stuff paledit;
struct freestyle_stuff freestyle;
struct entryinfo *choices[MAXENTRIES];
struct extended_opts extended_opts;
struct fls *pfls;
struct fractalspecificstuff *curfractalspecific;
struct fractalspecificstuff *savespecific;
struct historystruct *history;
struct parmtype parmtype[MAXPARAMS];
struct point oldlast;
struct point p1;
struct point p2;
struct point p3;
struct rotate_stuff rotate_stuff;
struct rubber_band_data rubber_band_data;
struct strfld_opts starfield_opts;
struct svprm saveprm;
struct three_d_opts three_d_opts;
struct trig_funct_lst trigfn[NUMTRIGFN];
struct videoinfo videotable[2];
struct viewwin_opts viewwin_opts;
struct window_presets window_presets;
struct workliststuff worklist[MAXCALCWORK];

union FracArg **Load;
union FracArg **Store;
union FracArg *Arg1;
union FracArg *Arg2;
union FracArg xms[20];

unsigned CoreBlue;
unsigned CoreGreen;
unsigned CoreRed;
unsigned NumFrames;
unsigned height;
unsigned initsavetime;
unsigned numcolors;
unsigned posp;
unsigned vsp;
unsigned xAspect;
unsigned yAspect;

unsigned char *LogTable;
unsigned char *mapdacbox;
unsigned char *pixbuf;
unsigned char *undobox;

unsigned char back_color[3];
unsigned char block[266];
unsigned char dacbox[256][3];
unsigned char dcdecoderline[MAXPIXELS+1];
unsigned char dcstack[MAX_CODES + 1];
unsigned char dcsuffix[MAX_CODES + 1];
unsigned char decoderline[MAXPIXELS+1];
unsigned char dstack[4096];
unsigned char olddacbox[256][3];
unsigned char suffix[4096];
unsigned char teststring[MAXTEST];
unsigned char trigndx[4];

unsigned int colsize;
unsigned int filter_cycles;
unsigned int prefix[2][maxyblk][maxxblk];
unsigned int rowsize;
unsigned int strlocn[10240];

unsigned long interval;
unsigned long lm;
unsigned long pixtab[256];

unsigned short dcprefix[MAX_CODES + 1];
unsigned short max_plasma;

int clk_ticks;

void (**xmf)(void);
void (*StkAND)(void);
void (*StkAbs)(void);
void (*StkAdd)(void);
void (*StkCoTan)(void);
void (*StkCoTanh)(void);
void (*StkConj)(void);
void (*StkCos)(void);
void (*StkCosXX)(void);
void (*StkCosh)(void);
void (*StkDiv)(void);
void (*StkEQ)(void);
void (*StkExp)(void);
void (*StkFlip)(void);
void (*StkGT)(void);
void (*StkGTE)(void);
void (*StkImag)(void);
void (*StkLT)(void);
void (*StkLTE)(void);
void (*StkLog)(void);
void (*StkMod)(void);
void (*StkMul)(void);
void (*StkNE)(void);
void (*StkNeg)(void);
void (*StkOR)(void);
void (*StkPwr)(void);
void (*StkReal)(void);
void (*StkSRand)(void);
void (*StkSin)(void);
void (*StkSinh)(void);
void (*StkSqr)(void);
void (*StkSub)(void);
void (*StkTan)(void);
void (*StkTanh)(void);
void (*StkTrig0)(void);
void (*StkTrig1)(void);
void (*StkTrig2)(void);
void (*StkTrig3)(void);

void (*dotwrite)(int, int, int);

void (*lineread)();
void (*linewrite)(int, int, int, unsigned char *);
void (*plot)(int,int,int);
void (*standardplot)(int,int,int);

void *typespecific_workarea;

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: globals.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.20  1995/06/15  15:36:25  darryl
 * Enhance zoom error message when attempting to zoom after
 * reading a GIF that does not contain fractal info.
 *
 * Revision 1.19  1995/06/14  01:30:07  darryl
 * Add color editor undo-last-command and undo-last-redo and
 * command-line and ini file support for max undo commands.
 *
 * Revision 1.18  1995/06/13  17:51:35  darryl
 * Add window resize mode function.
 *
 * Revision 1.17  1995/06/09  22:54:11  darryl
 * Change the atomic size of the LogTable to unsigned char.
 *
 * Revision 1.16  1995/06/05  16:30:14  tom
 * File handling enhancements. Remove DOS-specific code.
 *
 * Revision 1.15  1995/05/23  18:50:55  darryl
 * Fix color editor and palette randomization problems
 * reported in beta testing.
 *
 * Revision 1.14  1995/05/21  09:54:04  darryl
 * Add modifier keys.
 *
 * Revision 1.13  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.12.1.1  1995/06/05  14:06:56  tom
 * changed filename handling
 *
 * Revision 1.12  1995/05/19  08:11:11  darryl
 * Add freestyle wraparound feature.
 *
 * Revision 1.11  1995/05/10  22:14:04  darryl
 * Delete unused code and variables, fix problems
 * reported in beta test, enhance the file selection
 * dialogs and restore menu.
 *
 * Revision 1.10  1995/04/18  15:59:59  darryl
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
 * Revision 1.9  1995/04/10  22:12:57  darryl
 * Add rate=nnn and menu support for setting the screen refresh
 * rate during image calculation. Fix a bug where the Julia
 * inverse mode was broken after an image save. Add fallback
 * resources that take effect if a resource file is not found.
 *
 * Revision 1.8  1995/04/08  19:12:06  tom
 * Enhance timer functions. Uses sysconf(3) to
 * determine CLK_TCK if the function is available.
 *
 * Revision 1.7  1995/04/03  15:49:33  darryl
 * Add Brighter and Darker buttons to the Range menu.
 * Fix a problem where the color editor cancel button
 * would reset the colors to the default color map
 * after a gif image was loaded. Fix a potential
 * problem where the skipval dialog was not being
 * destroyed when the color editor was popped down.
 *
 * Revision 1.6  1995/03/28  22:47:41  darryl
 * Fix the "Defaults" function in the type parameters dialog.
 * Add a "Reset" function to type parameters dialog.
 * Fix the help dialogs so that any number of dialogs may
 * be opened simultaneously.
 *
 * Revision 1.5  1995/03/25  08:42:49  darryl
 * Fixes and enhancements -- make the parameter selection
 * understand that if a parameter file is loaded, then
 * the ifs, lsys, or formula file does not need to be
 * re-selected. Display the current fractal type and
 * any associated file and file entries at the top of
 * the parameters screen. Delete some unused code.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.2  1995/03/13  18:40:52  tom
 * changed type of calc_status
 *
 * Revision 1.1.1.1  1995/03/11  03:09:21  tom
 * renamed dysize to dy_size and dxsixe to dx_size
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.302  1995/01/17  00:03:30  darryl
 * Add a disclaimer dialog.
 *
 * Revision 0.301  1994/12/30  23:54:54  darryl
 * Delete unused global variables.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.103  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.102  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
 *
 * Revision 0.101  1994/11/23  05:19:37  darryl
 * Initial implementation of help. Minor widget alignment
 * changes. Widget name changes for uniqueness.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
