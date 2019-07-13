
#ifndef globals_h
#define globals_h

#include "xmfract.h"
#include "fractype.h"
#include "helpdefs.h"
#include "macros.h"
#include "mpmath.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: globals.h,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

extern Colormap colormap;

extern Dimension winheight;
extern Dimension winwidth;

extern Display *display;

extern FILE *fp;
extern FILE *out;

extern GC gc;
extern GC xgc;

extern MATH_TYPE MathType;

extern MATRIX mtrix;

extern Screen *screen;

extern VECTOR cross;
extern VECTOR view;

extern Visual *visual;

extern Widget canvas;
extern Widget draw_fractal_menu_btn1;
extern Widget explicit_x_size_scale;
extern Widget explicit_y_size_scale;
extern Widget main_window;
extern Widget menu_bar;
extern Widget misc_menu_btn[5];
extern Widget param_textfield[MAXPARAMS];
extern Widget toplevel;

extern Window window;

extern XColor cols[256];

extern XImage *image;

extern XtAppContext app;

extern short calc_status;

extern _CMPLX *floatparm;
extern _CMPLX *roots;
extern _CMPLX Coefficient;
extern _CMPLX attr[N_ATTR];
extern _CMPLX cdegree;
extern _CMPLX coefficient;
extern _CMPLX croot;
extern _CMPLX init;
extern _CMPLX initorbit;
extern _CMPLX new;
extern _CMPLX old;
extern _CMPLX parm2;
extern _CMPLX parm;
extern _CMPLX pwr;
extern _CMPLX saved;
extern _CMPLX temp;
extern _CMPLX tmp;

extern _LCMPLX lattr[N_ATTR];
extern _LCMPLX linit;
extern _LCMPLX linitorbit;
extern _LCMPLX lnew;
extern _LCMPLX lold;
extern _LCMPLX lparm2;
extern _LCMPLX lparm;

extern char *black_str;
extern char *bof60_str;
extern char *bof61_str;
extern char *curtrigndx_str[4];
extern char *epscr_str;
extern char *extraseg;
extern char *fractdir;
extern char *geometry;
extern char *helpdir;
extern char *imag_str;
extern char *iter_str;
extern char *juli3Doptions[4];
extern char *maxiter_str;
extern char *mult_str;
extern char *newtrigndx_str[4];
extern char *per_str;
extern char *real_str;
extern char *resume_info;
extern char *scrnfile;
extern char *star_str;
extern char *summ_str;
extern char *zmag_str;

extern char CommandFile[MAXPATHLEN];
extern char CommandName[80];
extern char FormFileName[MAXPATHLEN];
extern char FormName[80];
extern char Glasses1Map[MAXPATHLEN];
extern char GreyFile[MAXPATHLEN];
extern char IFSFileName[MAXPATHLEN];
extern char IFSName[80];
extern char LFileName[MAXPATHLEN];
extern char LName[80];
extern char MAP_name[MAXPATHLEN];
extern char PAR_name[MAXPATHLEN];
extern char ZoomName[MAXPATHLEN];
extern char angle;
extern char boxx[9216];
extern char boxy[4096];
extern char xmbusy;
extern char colorfile[MAXPATHLEN];
extern char counter;
extern char floatflag;
extern char initcorners;
extern char light_name[MAXPATHLEN];
extern char loaded;
extern char mapfilename[MAXPATHLEN];
extern char oldFormFileName[MAXPATHLEN];
extern char oldFormName[80];
extern char oldIFSFileName[MAXPATHLEN];
extern char oldIFSName[80];
extern char oldLFileName[MAXPATHLEN];
extern char oldLName[80];
extern char overwrite_flag;
extern char preview;
extern char ray_name[MAXPATHLEN];
extern char readname[MAXPATHLEN];
extern char restore_which[20][4];
extern char reverse;
extern char savename[MAXPATHLEN];
extern char showbox;
extern char stdcalcmode;
extern char useinitorbit;
extern char usr_floatflag;
extern char usr_stdcalcmode;

extern double *dx0;
extern double *dx1;
extern double *dy0;
extern double *dy1;

extern double Cx;
extern double Cy;
extern double Population;
extern double Rate;
extern double xma;
extern double aspect;
extern double xmb;
extern double xmc;
extern double closenuff;
extern double cosx;
extern double cosy;
extern double curparam[MAXPARAMS];
extern double xmd;
extern double ddelmin;
extern double defparam[MAXPARAMS];
extern double deltaX;
extern double deltaY;
extern double delxx2;
extern double delxx;
extern double delyy2;
extern double delyy;
extern double dem_delta;
extern double dem_toobig;
extern double dem_width;
extern double depthfp;
extern double dinitorbit[3];
extern double distfp;
extern double dt;
extern double dx;
extern double dx_size;
extern double dy;
extern double dy_size;
extern double dz;
extern double eyesfp;
extern double f_at_rad;
extern double f_radius;
extern double f_xcenter;
extern double f_ycenter;
extern double xmfg;
extern double floatmax;
extern double floatmin;
extern double heightfp;
extern double initx;
extern double inity;
extern double inversion[3];
extern double magnitude;
extern double mxmaxfp;
extern double mxminfp;
extern double mymaxfp;
extern double myminfp;
extern double orbit;
extern double originfp;
extern double param[MAXPARAMS];
extern double plotmx1;
extern double plotmx2;
extern double plotmy1;
extern double plotmy2;
extern double potparam[3];
extern double qc;
extern double qci;
extern double qcj;
extern double qck;
extern double rqlim2;
extern double rqlim;
extern double rqlim_save;
extern double sinx;
extern double siny;
extern double starfield_values[4];
extern double sx3rd;
extern double sxmax;
extern double sxmin;
extern double sy3rd;
extern double symax;
extern double symin;
extern double t2;
extern double tempsqrx;
extern double tempsqry;
extern double threshold;
extern double ttmax;
extern double ttmin;
extern double twopi;
extern double widthfp;
extern double xmxt;
extern double xx3rd;
extern double xxmax;
extern double xxmin;
extern double xmyt;
extern double yy3rd;
extern double yymax;
extern double yymin;
extern double zbx;
extern double zby;
extern double zdepth;
extern double zskew;
extern double zwidth;
extern double zzmax;
extern double zzmin;

extern float *ifs_defn;

extern float finalaspectratio;
extern float screenaspect;
extern float version;
extern float viewreduction;

extern int (*calctype)();
extern int (*dotread)(int, int);
extern int (*outln)(unsigned char *,unsigned);

extern int *ranges;
extern int *save_orbit;

extern int Ambient;
extern int AntiAliasing;
extern int AplusOne;
extern int BRIEF;
extern int CO;
extern int CO_MAX;
extern int Distribution;
extern int LastInitOp;
extern int LodPtr;
extern int LogFlag;
extern int MaxColorRes;
extern int Offset;
extern int OpPtr;
extern int XMP;
extern int RANDOMIZE;
extern int RAY;
extern int RO;
extern int Real_V;
extern int Shadowing;
extern int ShiftBack;
extern int Slope;
extern int SolidCore;
extern int StoPtr;
extern int TPlusFlag;
extern int T_Safe;
extern int T_header_24;
extern int Targa_Out;
extern int Targa_Overlay;
extern int TranspSymmetry;
extern int Transparent3D;
extern int abort_process;
extern int altkey;
extern int andcolor;
extern int attractors;
extern int attrperiod[N_ATTR];
extern int bad_check;
extern int bad_code_count;
extern int bad_value;
extern int bailout;
extern int basin;
extern int bbase;
extern int biomorph;
extern int bitshift;
extern int bitshiftless1;
extern int blue_bright;
extern int blue_crop_left;
extern int blue_crop_right;
extern int bottom_guess;
extern int boxcount;
extern int c_exp;
extern int col;
extern int color;
extern int color_bright;
extern int color_dark;
extern int colorpreloaded;
extern int colors;
extern int colorstate;
extern int creditscreen;
extern int ctrlkey;
extern int curcol;
extern int curpass;
extern int currow;
extern int curtype;
extern int debugflag;
extern int decomp[2];
extern int degree;
extern int disclaimer;
extern int disk16bit;
extern int diskflag;
extern int diskisactive;
extern int diskvideo;
extern int display3d;
extern int distest;
extern int distestwidth;
extern int dither_flag;
extern int doconnect;
extern int doredraw;
extern int editpal_cursor;
extern int xmerror;
extern int escape_exit;
extern int euler;
extern int eyeseparation;
extern int filecolors;
extern int filetype;
extern int filexdots;
extern int fileydots;
extern int fillcolor;
extern int finattract;
extern int forcesymmetry;
extern int forwarding;
extern int fractype;
extern int functionpreloaded;
extern int gif87a_flag;
extern int glassestype;
extern int got_status;
extern int gotfile;
extern int gotfrmfile;
extern int gotfrmname;
extern int gotifsfile;
extern int gotifsname;
extern int gotlsysfile;
extern int gotlsysname;
extern int gotrealdac;
extern int guessplot;
extern int halfblock;
extern int hasinverse;
extern int haze;
extern int helpmode;
extern int histocycle;
extern int historyflag;
extern int historymax;
extern int historyptr;
extern int ifs_type;
extern int init3d[20];
extern int initbatch;
extern int initmode;
extern int inside;
extern int inside_color;
extern int integerfractal;
extern int invert;
extern int ipixtab[256];
extern int ixstart;
extern int ixstop;
extern int iystart;
extern int iystop;
extern int jiiming;
extern int juli3Dmode;
extern int julibrot;
extern int kbdcount;
extern int keybuf;
extern int keybuffer;
extern int last_colorbar;
extern int lastorbittype;
extern int loaded3d;
extern int lstscreenflag;
extern int mapset;
extern int max_colors;
extern int max_kbdcount;
extern int maxblock;
extern int maxcolor;
extern int maxhits;
extern int maxit;
extern int metakey;
extern int mono;
extern int neworbittype;
extern int num_worklist;
extern int numaffine;
extern int numaffine;
extern int numtrigfn;
extern int numtypes;
extern int nxtscreenflag;
extern int ok_to_cycle;
extern int oldcolor;
extern int oldtype;
extern int oldx;
extern int oldy;
extern int orbit_color;
extern int orbit_delay;
extern int orbit_ptr;
extern int orbitsave;
extern int outcolor1s;
extern int outcolor2s;
extern int outside;
extern int overflow;
extern int overlay3d;
extern int paren;
extern int periodicitycheck;
extern int persp;
extern int pixelpi;
extern int plasma_check;
extern int pot16bit;
extern int potflag;
extern int previewfactor;
extern int privatecolor;
extern int projection;
extern int rangeslen;
extern int randshell_popped_up;
extern int realcolor;
extern int realtime;
extern int red_bright;
extern int red_crop_left;
extern int red_crop_right;
extern int refresh_rate;
extern int refresh_timeout;
extern int release;
extern int resave_flag;
extern int reset_periodicity;
extern int resized_by_user;
extern int resize_mode;
extern int resume_len;
extern int resuming;
extern int rflag;
extern int right_guess;
extern int root;
extern int rotate_hi;
extern int rotate_lo;
extern int row;
extern int rowcount;
extern int rseed;
extern int save16bit;
extern int save_release;
extern int save_system;
extern int screen_number;
extern int set_defaults;
extern int sharecolor;
extern int shiftkey;
extern int show_orbit;
extern int showdot;
extern int showfile;
extern int skipxdots;
extern int skipydots;
extern int slides;
extern int soundflag;
extern int started_resaves;
extern int sxdots;
extern int sxoffs;
extern int sydots;
extern int symmetry;
extern int syoffs;
extern int xmt;
extern int tabmode;
extern int timerflag;
extern int totpasses;
extern int tpdepth;
extern int tptime;
extern int transparent[2];
extern int undo_max;
extern int usepixtab;
extern int using_jiim;
extern int usr_biomorph;
extern int usr_distest;
extern int usr_periodicitycheck;
extern int usr_randmap_func;
extern int usr_randmap_mode;
extern int verbose;
extern int viewcrop;
extern int viewwindow;
extern int viewxdots;
extern int viewydots;
extern int waste;
extern int whichimage;
extern int workpass;
extern int worksym;
extern int xadjust;
extern int xbase;
extern int xdots;
extern int xlastcolor;
extern int xshift1;
extern int xshift;
extern int xtrans;
extern int xxadjust1;
extern int xxadjust;
extern int xxstart;
extern int xxstop;
extern int yadjust;
extern int ybase;
extern int ydots;
extern int yshift1;
extern int yshift;
extern int ytrans;
extern int yyadjust;
extern int yybegin;
extern int yystart;
extern int yystop;
extern int zdots;
extern int zoomcolor_index;
extern int zoomoff;
extern int zrotate;

extern long *lx0;
extern long *lx1;
extern long *ly0;
extern long *ly1;

extern long ListBack;
extern long ListFront;
extern long ListSize;
extern long calctime;
extern long cimag;
extern long con;
extern long creal;
extern long delmin;
extern long delx;
extern long dely;
extern long dist;
extern long djx;
extern long djy;
extern long eyes;
extern long fudge;
extern long jx;
extern long jy;
extern long l16triglim;
extern long l_at_rad;
extern long lclosenuff;
extern long llimit2;
extern long llimit;
extern long lmagnitud;
extern long lmax;
extern long lsize;
extern long lt2;
extern long ltempsqrx;
extern long ltempsqry;
extern long mx;
extern long mxmin;
extern long my;
extern long mymin;
extern long sins[50];
extern long timer_interval;
extern long width;
extern long x3rd;
extern long xmax;
extern long xmin;
extern long xoffset;
extern long xpixel;
extern long xpos;
extern long y3rd;
extern long ymax;
extern long ymin;
extern long yoffset;
extern long ypixel;
extern long ypos;

extern short maxfn;
extern short usemag;

extern struct ConstArg *xmv;
extern struct PEND_OP *xmo;
extern struct affine cvt;
extern struct basic_opts basic_opts;
extern struct corner_val corner_val;
extern struct editpal_stuff paledit;
extern struct freestyle_stuff freestyle;
extern struct entryinfo *choices[MAXENTRIES];
extern struct extended_opts extended_opts;
extern struct fls *pfls;
extern struct fractalspecificstuff *curfractalspecific;
extern struct fractalspecificstuff *savespecific;
extern struct historystruct *history;
extern struct parmtype parmtype[MAXPARAMS];
extern struct point oldlast;
extern struct point p1;
extern struct point p2;
extern struct point p3;
extern struct rotate_stuff rotate_stuff;
extern struct rubber_band_data rubber_band_data;
extern struct strfld_opts starfield_opts;
extern struct svprm saveprm;
extern struct three_d_opts three_d_opts;
extern struct trig_funct_lst trigfn[NUMTRIGFN];
extern struct videoinfo videotable[2];
extern struct viewwin_opts viewwin_opts;
extern struct window_presets window_presets;
extern struct workliststuff worklist[MAXCALCWORK];

extern union FracArg **Load;
extern union FracArg **Store;
extern union FracArg *Arg1;
extern union FracArg *Arg2;
extern union FracArg xms[20];

extern unsigned CoreBlue;
extern unsigned CoreGreen;
extern unsigned CoreRed;
extern unsigned NumFrames;
extern unsigned height;
extern unsigned initsavetime;
extern unsigned numcolors;
extern unsigned posp;
extern unsigned vsp;
extern unsigned xAspect;
extern unsigned yAspect;

extern unsigned char *LogTable;
extern unsigned char *mapdacbox;
extern unsigned char *pixbuf;
extern unsigned char *undobox;

extern unsigned char back_color[3];
extern unsigned char block[266];
extern unsigned char dacbox[256][3];
extern unsigned char dcdecoderline[MAXPIXELS+1];
extern unsigned char dcstack[MAX_CODES + 1];
extern unsigned char dcsuffix[MAX_CODES + 1];
extern unsigned char decoderline[MAXPIXELS+1];
extern unsigned char dstack[4096];
extern unsigned char olddacbox[256][3];
extern unsigned char suffix[4096];
extern unsigned char teststring[MAXTEST];
extern unsigned char trigndx[4];

extern unsigned int colsize;
extern unsigned int filter_cycles;
extern unsigned int prefix[2][maxyblk][maxxblk];
extern unsigned int rowsize;
extern unsigned int strlocn[10240];

extern unsigned long interval;
extern unsigned long lm;
extern unsigned long pixtab[256];

extern unsigned short dcprefix[MAX_CODES + 1];
extern unsigned short max_plasma;

extern int clk_ticks;

extern void (**xmf)(void);
extern void (*StkAND)(void);
extern void (*StkAbs)(void);
extern void (*StkAdd)(void);
extern void (*StkCoTan)(void);
extern void (*StkCoTanh)(void);
extern void (*StkConj)(void);
extern void (*StkCos)(void);
extern void (*StkCosXX)(void);
extern void (*StkCosh)(void);
extern void (*StkDiv)(void);
extern void (*StkEQ)(void);
extern void (*StkExp)(void);
extern void (*StkFlip)(void);
extern void (*StkGT)(void);
extern void (*StkGTE)(void);
extern void (*StkImag)(void);
extern void (*StkLT)(void);
extern void (*StkLTE)(void);
extern void (*StkLog)(void);
extern void (*StkMod)(void);
extern void (*StkMul)(void);
extern void (*StkNE)(void);
extern void (*StkNeg)(void);
extern void (*StkOR)(void);
extern void (*StkPwr)(void);
extern void (*StkReal)(void);
extern void (*StkSRand)(void);
extern void (*StkSin)(void);
extern void (*StkSinh)(void);
extern void (*StkSqr)(void);
extern void (*StkSub)(void);
extern void (*StkTan)(void);
extern void (*StkTanh)(void);
extern void (*StkTrig0)(void);
extern void (*StkTrig1)(void);
extern void (*StkTrig2)(void);
extern void (*StkTrig3)(void);

extern void (*dotwrite)(int, int, int);

extern void (*lineread)();
extern void (*linewrite)(int, int, int, unsigned char *);
extern void (*plot)(int,int,int);
extern void (*standardplot)(int,int,int);

extern void *typespecific_workarea;

#endif

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: globals.h,v $
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
 * Revision 1.12.1.1  1995/06/05  14:07:12  tom
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
 * Revision 1.1.1.2  1995/03/13  18:42:00  tom
 * changed type of calc_status
 *
 * Revision 1.1.1.1  1995/03/11  03:52:29  tom
 * change dxsize and dysize
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
