
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: init_data.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void init_memsegs(void);

/* static variables */
void (*dtrig0)(void) = dStkSin;
void (*dtrig1)(void) = dStkSqr;
void (*dtrig2)(void) = dStkSinh;
void (*dtrig3)(void) = dStkCosh;

void
init_data(int argc, char **argv)
{
  int backing_store;
  XSetWindowAttributes winattrs;

  /*
   * initial or default values for globals
   */

  init_memsegs();
  initvars_restart();
  initvars_fractal();

  version = VERSION;

  strcpy(scrnfile, "xmfract.scrn");
  strcpy((char *) &light_name, "fract001.tga");
  strcpy((char *) &ray_name, "fract001");
  strcpy((char *) &savename, "fract001");
  strcpy((char *) &Glasses1Map, "glasses1.map");
  strcpy((char *) &GreyFile, "altern.map");
  strcpy((char *) &ZoomName, "NOTGIF");
  strcpy((char *) &CommandFile,"fractint.par");
  strcpy((char *) &PAR_name, "xmfract.par");
  strcpy((char *) &readname, "./");

  strcpy(maxiter_str, "Maxiter");
  strcpy(zmag_str, "Zmag");
  strcpy(bof60_str, "BoF60");
  strcpy(bof61_str, "BoF61");
  strcpy(epscr_str, "Epscr");
  strcpy(star_str, "Star");
  strcpy(per_str, "Per");
  strcpy(black_str, "1");
  strcpy(iter_str, "Iter");
  strcpy(real_str, "Real");
  strcpy(imag_str, "Imag");
  strcpy(mult_str, "Mult");
  strcpy(summ_str, "Summ");

  strcpy(juli3Doptions[0], "monocular");
  strcpy(juli3Doptions[1], "lefteye");
  strcpy(juli3Doptions[2], "righteye");
  strcpy(juli3Doptions[3], "red-blue");

  strcpy(curtrigndx_str[0], "sin");
  strcpy(curtrigndx_str[1], "sqr");
  strcpy(curtrigndx_str[2], "sinh");
  strcpy(curtrigndx_str[3], "cosh");

  strcpy(newtrigndx_str[0], "sin");
  strcpy(newtrigndx_str[1], "sqr");
  strcpy(newtrigndx_str[2], "sinh");
  strcpy(newtrigndx_str[3], "cosh");

  corner_val.current.xxmin = xxmin;
  corner_val.current.xxmax = xxmax;
  corner_val.current.yymin = yymin;
  corner_val.current.yymax = yymax;
  corner_val.current.xx3rd = xx3rd;
  corner_val.current.yy3rd = yy3rd;
  corner_val.current.zzmin = zzmin;
  corner_val.current.zzmax = zzmax;
  corner_val.current.ttmin = ttmin;
  corner_val.current.ttmax = ttmax;

  corner_val.dflt = corner_val.new = corner_val.current;

  basic_opts.current.usr_floatflag = usr_floatflag;
  basic_opts.current.stdcalcmode = usr_stdcalcmode;
  basic_opts.current.soundflag = 0;
  basic_opts.current.inside = inside;
  basic_opts.current.outside = outside;
  basic_opts.current.maxit = maxit;
  basic_opts.current.LogFlag = LogFlag;
  basic_opts.current.usr_biomorph = usr_biomorph;
  basic_opts.current.decomp = decomp[0];
  basic_opts.current.fillcolor = fillcolor;
  basic_opts.current.orbit_delay = 0;

  basic_opts.dflt = basic_opts.new = basic_opts.current;

  extended_opts.current.finattract = finattract;
  extended_opts.current.potparam0 = potparam[0];
  extended_opts.current.potparam1 = potparam[1];
  extended_opts.current.potparam2 = potparam[2];
  extended_opts.current.usr_distest = usr_distest;
  extended_opts.current.distestwidth = distestwidth;
  extended_opts.current.inversion0 = inversion[0];
  extended_opts.current.inversion1 = inversion[1];
  extended_opts.current.inversion2 = inversion[2];
  extended_opts.current.rotate_lo = rotate_lo;
  extended_opts.current.rotate_hi = rotate_hi;
  extended_opts.current.pot16bit = pot16bit;
  extended_opts.current.periodicitycheck = usr_periodicitycheck;
  extended_opts.current.symmetry = symmetry;
  extended_opts.current.rseed = rseed;
  extended_opts.current.showdot = showdot;

  extended_opts.dflt = extended_opts.current;

  three_d_opts.dflt.preview = preview;
  three_d_opts.dflt.showbox = showbox;
  three_d_opts.dflt.previewfactor = previewfactor;
  three_d_opts.dflt.sphere = SPHERE;
  three_d_opts.dflt.glassestype = glassestype;
  three_d_opts.dflt.ray = RAY;
  three_d_opts.dflt.brief = BRIEF;
  three_d_opts.dflt.ray_name = (char *) &ray_name;
  three_d_opts.dflt.targa_out = Targa_Out;
  three_d_opts.dflt.phi1 = PHI1;
  three_d_opts.dflt.phi2 = PHI2;
  three_d_opts.dflt.theta1 = THETA1;
  three_d_opts.dflt.theta2 = THETA2;
  three_d_opts.dflt.radius = RADIUS;
  three_d_opts.dflt.xrot = XROT;
  three_d_opts.dflt.yrot = YROT;
  three_d_opts.dflt.zrot = ZROT;
  three_d_opts.dflt.xscale = XSCALE;
  three_d_opts.dflt.yscale = YSCALE;
  three_d_opts.dflt.rough = ROUGH;
  three_d_opts.dflt.waterline = WATERLINE;
  three_d_opts.dflt.zviewer = ZVIEWER;
  three_d_opts.dflt.xshift = XSHIFT;
  three_d_opts.dflt.yshift = YSHIFT;
  three_d_opts.dflt.xtrans = xtrans;
  three_d_opts.dflt.ytrans = ytrans;
  three_d_opts.dflt.transparent[0] = transparent[0];
  three_d_opts.dflt.transparent[1] = transparent[1];
  three_d_opts.dflt.rndcolor = RANDOMIZE;
  three_d_opts.dflt.eyeseparation = eyeseparation;
  three_d_opts.dflt.xadjust = xadjust;
  three_d_opts.dflt.red_crop_left = red_crop_left;
  three_d_opts.dflt.red_crop_right = red_crop_right;
  three_d_opts.dflt.blue_crop_left = blue_crop_left;
  three_d_opts.dflt.blue_crop_right = blue_crop_right;
  three_d_opts.dflt.red_bright = red_bright;
  three_d_opts.dflt.blue_bright = blue_bright;

  three_d_opts.current = three_d_opts.dflt;

  starfield_opts.dflt.starfield_values[0] =
    starfield_values[0] = 30.0;

  starfield_opts.dflt.starfield_values[1] =
    starfield_values[1] = 100.0;

  starfield_opts.dflt.starfield_values[2] =
    starfield_values[2] = 5.0;

  starfield_opts.current = starfield_opts.dflt;

  rubber_band_data.moving = 0;
  rubber_band_data.panning = 0;
  rubber_band_data.stretching = 0;
  rubber_band_data.boxisactive = 0;
  rubber_band_data.snap_to_aspect = 1;

  rotate_stuff.running = 0;
  rotate_stuff.direction = -1;
  rotate_stuff.interval = 50;

  strcpy(trigfn[ 0].name, "sin");
  trigfn[ 0].lfunct = dStkSin;
  trigfn[ 0].dfunct = dStkSin;
  trigfn[ 0].mfunct = dStkSin;

  strcpy(trigfn[ 1].name, "cosxx");
  trigfn[ 1].lfunct = dStkCosXX;
  trigfn[ 1].dfunct = dStkCosXX;
  trigfn[ 1].mfunct = dStkCosXX;

  strcpy(trigfn[ 2].name, "sinh");
  trigfn[ 2].lfunct = dStkSinh;
  trigfn[ 2].dfunct = dStkSinh;
  trigfn[ 2].mfunct = dStkSinh;

  strcpy(trigfn[ 3].name, "cosh");
  trigfn[ 3].lfunct = dStkCosh;
  trigfn[ 3].dfunct = dStkCosh;
  trigfn[ 3].mfunct = dStkCosh;

  strcpy(trigfn[ 4].name, "exp");
  trigfn[ 4].lfunct = dStkExp;
  trigfn[ 4].dfunct = dStkExp;
  trigfn[ 4].mfunct = dStkExp;

  strcpy(trigfn[ 5].name, "log");
  trigfn[ 5].lfunct = dStkLog;
  trigfn[ 5].dfunct = dStkLog;
  trigfn[ 5].mfunct = dStkLog;

  strcpy(trigfn[ 6].name, "sqr");
  trigfn[ 6].lfunct = dStkSqr;
  trigfn[ 6].dfunct = dStkSqr;
  trigfn[ 6].mfunct = dStkSqr;

  strcpy(trigfn[ 7].name, "recip");
  trigfn[ 7].lfunct = dStkRecip;
  trigfn[ 7].dfunct = dStkRecip;
  trigfn[ 7].mfunct = dStkRecip;

  strcpy(trigfn[ 8].name, "ident");
  trigfn[ 8].lfunct = StkIdent;
  trigfn[ 8].dfunct = StkIdent;
  trigfn[ 8].mfunct = StkIdent;

  strcpy(trigfn[ 9].name, "cos");
  trigfn[ 9].lfunct = dStkCos;
  trigfn[ 9].dfunct = dStkCos;
  trigfn[ 9].mfunct = dStkCos;

  strcpy(trigfn[10].name, "tan");
  trigfn[10].lfunct = dStkTan;
  trigfn[10].dfunct = dStkTan;
  trigfn[10].mfunct = dStkTan;

  strcpy(trigfn[11].name, "tanh");
  trigfn[11].lfunct = dStkTanh;
  trigfn[11].dfunct = dStkTanh;
  trigfn[11].mfunct = dStkTanh;

  strcpy(trigfn[12].name, "cotan");
  trigfn[12].lfunct = dStkCoTan;
  trigfn[12].dfunct = dStkCoTan;
  trigfn[12].mfunct = dStkCoTan;

  strcpy(trigfn[13].name, "cotanh");
  trigfn[13].lfunct = dStkCoTanh;
  trigfn[13].dfunct = dStkCoTanh;
  trigfn[13].mfunct = dStkCoTanh;

  strcpy(trigfn[14].name, "flip");
  trigfn[14].lfunct = dStkFlip;
  trigfn[14].dfunct = dStkFlip;
  trigfn[14].mfunct = dStkFlip;

  strcpy(trigfn[15].name, "conj");
  trigfn[15].lfunct = dStkConj;
  trigfn[15].dfunct = dStkConj;
  trigfn[15].mfunct = dStkConj;

  strcpy(trigfn[16].name, "zero");
  trigfn[16].lfunct = dStkZero;
  trigfn[16].dfunct = dStkZero;
  trigfn[16].mfunct = dStkZero;

  trigndx[0] = SIN;
  trigndx[1] = SQR;
  trigndx[2] = SINH;
  trigndx[3] = COSH;

  update_zoom_menu();

  load_videotable(1);

  history = (struct historystruct *) XtMalloc((unsigned long)
            (MAXHISTORY * sizeof(*history)));

  historyptr = -1;
  historyflag = 0;
  history[0].fractype = -1;

  update_restore_menu();

  strcpy(videotable[0].name,    "xmfract mode             ");
  strcpy(videotable[0].comment, "                         ");

  videotable[0].keynum =      999;
  videotable[0].videomodeax =   0;
  videotable[0].videomodebx =   0;
  videotable[0].videomodecx =   0;
  videotable[0].videomodedx =   0;
  videotable[0].dotmode =      19;
  videotable[0].xdots =       640;
  videotable[0].ydots =       480;
  videotable[0].colors =   colors;

  strcpy(videotable[1].name,    "unused mode              ");
  strcpy(videotable[1].comment, "                         ");

  videotable[1].keynum =        0;
  videotable[1].videomodeax =   0;
  videotable[1].videomodebx =   0;
  videotable[1].videomodecx =   0;
  videotable[1].videomodedx =   0;
  videotable[1].dotmode =       0;
  videotable[1].xdots =         0;
  videotable[1].ydots =         0;
  videotable[1].colors =        0;

  switch(visual->class)
    {
    case DirectColor:
    case PseudoColor:
    case GrayScale:
      gotrealdac=1;
    break;

    case TrueColor:
    case StaticColor:
    case StaticGray:
    default:
      gotrealdac=0;
    break;
    }

  if(fractdir == NULL)
    {
    char *tempstr = getenv("XMFRACTDIR");

    if (tempstr == NULL) tempstr = getenv("FRACTDIR");

    if(tempstr != NULL)
      {
      if((fractdir = XtMalloc(MAXPATHLEN)) == (char *) NULL)
        {
        fprintf(stderr, "Sorry, out of memory (fractdir).\n");
        exit(1);
        }
      sprintf(fractdir, "%s", tempstr);
      }
    }

  if(fractdir == NULL)
    fractdir = getcwd(NULL, 64);

  if(fractdir == NULL)
    {
    fprintf(stderr, "Sorry, cannot determine current directory name.\n");
    exit(1);
    }

  cmdfiles(argc, argv);

  if(helpdir == NULL)
    {
    if(fractdir != NULL)
      {
      if((helpdir = XtMalloc(MAXPATHLEN)) == (char *) NULL)
        {
        fprintf(stderr, "Sorry, out of memory (helpdir).\n");
        exit(1);
        }
      if (((int)strlen(fractdir) + (int)strlen("/help")) >= MAXPATHLEN)
        {
        fprintf(stderr, "Sorry, path to long (helpdir).\n");
        exit(1);
        }
      sprintf(helpdir, "%s/help", fractdir);
      }
    }
  else if (helpdir[0] != '/')
    {
    if (helpdir[0] != '/')
      {
      char tempstr[MAXPATHLEN];

      if (((int)strlen(fractdir) + (int)strlen(helpdir) + 1) >= MAXPATHLEN)
        {
        fprintf(stderr, "Sorry, path to long (helpdir).\n");
        exit(1);
        }

      strcpy(tempstr, helpdir);
      sprintf(helpdir, "%s/%s", fractdir, tempstr);
      }
    }

  colors = cmapstuff();

  if(colors < 255)
    XtSetSensitive(misc_menu_btn[2], False);
  else
    XtSetSensitive(misc_menu_btn[2], True);

  maxcolor = colors;
  andcolor = colors - 1;

  rotate_hi = colors - 1;

  find_special_colors();

  extended_opts.dflt.rotate_hi = rotate_hi;
  extended_opts.current = extended_opts.dflt;

  winattrs.background_pixel = BlackPixelOfScreen(screen);
  winattrs.bit_gravity = StaticGravity;
  backing_store = DoesBackingStore(screen);

  if (backing_store)
    winattrs.backing_store = Always;
  else
    winattrs.backing_store = NotUseful;

  winattrs.background_pixel = BlackPixelOfScreen(screen);

  XChangeWindowAttributes(display, XtWindow(canvas),
        CWBackPixel | CWBitGravity | CWBackingStore, &winattrs);

  resize_window(main_window, NULL, NULL);

  XSync(display,False);

  if(!mapset)
    initdacbox();

  memcpy(olddacbox,dacbox,768);

  if(mapdacbox)
    {
    memcpy((char *)dacbox,mapdacbox,768);
    spindac(0,0);
    colorstate = 0;
    }

  if(undo_max > 0)
    {
    if((undobox = XtMalloc
      (sizeof(unsigned char) * undo_max * 768)) == (unsigned char *) NULL)
      {
      fprintf(stderr, "Sorry, out of memory (max undos %d).\n", undo_max);
      fprintf(stderr, "Try setting undos to a lower value.\n");
      exit(1);
      }
    }

  videotable[0].xdots = sxdots;
  videotable[0].ydots = sydots;
  videotable[0].colors = colors;
  videotable[0].dotmode = 19;

  viewwin_opts.dflt.viewwindow = 0;
  viewwin_opts.dflt.viewcrop = 1;
  viewwin_opts.dflt.viewreduction = 4.2;
  viewwin_opts.dflt.finalaspectratio = screenaspect;
  viewwin_opts.dflt.viewxdots = 0;
  viewwin_opts.dflt.viewydots = 0;

  viewwin_opts.current = viewwin_opts.dflt;

  xdots = videotable[0].xdots;
  ydots = videotable[0].ydots;

  dx_size = xdots - 1;
  dy_size = ydots - 1;

  sxoffs = 0;
  syoffs = 0;

  helpmode = HELPXMFRACT;

  /* set window refresh rate */
  refresh_timeout = (int) ((sxdots / refresh_rate) * sydots);

  /* init clk_ticks variable */
#ifdef HAVE_SYSCONF
  clk_ticks = sysconf(_SC_CLK_TCK);
#else
  clk_ticks = CLK_TCK;
#endif
}

static void init_memsegs()
{
  int i;

  if((extraseg = XtMalloc(0x18000)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (extraseg).\n");
    exit(1);
    }

  if((scrnfile = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (scrnfile).\n");
    exit(1);
    }

  if((maxiter_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (maxiter_str).\n");
    exit(1);
    }

  if((zmag_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (zmag_str).\n");
    exit(1);
    }

  if((bof60_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (bof60_str).\n");
    exit(1);
    }

  if((bof61_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (bof61_str).\n");
    exit(1);
    }

  if((epscr_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (epscr_str).\n");
    exit(1);
    }

  if((star_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (star_str).\n");
    exit(1);
    }

  if((per_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (per_str).\n");
    exit(1);
    }

  if((black_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (black_str).\n");
    exit(1);
    }

  if((iter_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (iter_str).\n");
    exit(1);
    }


  if((real_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (real_str).\n");
    exit(1);
    }

  if((imag_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (imag_str).\n");
    exit(1);
    }

  if((mult_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (mult_str).\n");
    exit(1);
    }

  if((summ_str = XtMalloc(MAXPATHLEN)) == (char *) NULL)
    {
    fprintf(stderr, "Sorry, out of memory (summ_str).\n");
    exit(1);
    }

  for(i = 0; i < NUMTRIGFN; i++)
    {
    if((trigfn[i].name = (char *)XtMalloc(MAXPATHLEN)) == (char *) NULL)
      {
      fprintf(stderr, "Sorry, out of memory (trigfn).\n");
      exit(1);
      }
    trigfn[i].name[0] = 0;
    }

  for(i = 0; i < 4; i++)
    {
    if((juli3Doptions[i] = (char *)XtMalloc(MAXPATHLEN)) == (char *) NULL)
      {
      fprintf(stderr, "Sorry, out of memory (juli3Doptions).\n");
      exit(1);
      }
    }

  for(i = 0; i < 4; i++)
    {
    if((curtrigndx_str[i] = (char *)XtMalloc(MAXPATHLEN)) == (char *) NULL)
      {
      fprintf(stderr, "Sorry, out of memory (curtrigndx_str).\n");
      exit(1);
      }
    }

  for(i = 0; i < 4; i++)
    {
    if((newtrigndx_str[i] = (char *)XtMalloc(MAXPATHLEN)) == (char *) NULL)
      {
      fprintf(stderr, "Sorry, out of memory (curtrigndx_str).\n");
      exit(1);
      }
    }

  for(i = 0; i < MAXENTRIES; i++)
    {
    if((choices[i] =
        (struct entryinfo *) XtMalloc(sizeof(struct entryinfo))) ==
        (struct entryinfo *) NULL)
      {
      fprintf(stderr, "Sorry, out of memory (choices_struct).\n");
      exit(1);
      }
    choices[i]->name[0] = 0;
    }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: init_data.c,v $
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
 * Revision 1.18  1995/06/05  16:30:14  tom
 * File handling enhancements. Remove DOS-specific code.
 *
 * Revision 1.17  1995/05/21  09:54:04  darryl
 * Fix rate= option and initialize some modifier keys.
 *
 * Revision 1.16.1.1  1995/06/05  14:07:55  tom
 * changed filename handling
 *
 * Revision 1.16  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.15  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.14  1995/04/18  15:59:59  darryl
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
 * Revision 1.13  1995/04/08  19:12:06  tom
 * Enhance timer functions. Uses sysconf(3) to
 * determine CLK_TCK if the function is available.
 *
 * Revision 1.12  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.11  1995/03/28  22:47:41  darryl
 * Fix the "Defaults" function in the type parameters dialog.
 * Add a "Reset" function to type parameters dialog.
 * Fix the help dialogs so that any number of dialogs may
 * be opened simultaneously.
 *
 * Revision 1.10  1995/03/28  18:11:40  darryl
 * Fix trig function handling. Enhance center-mag button
 * management. Aesthetic changes in the parameter dialog.
 * Add frames around groups of scale widgets. Fix a bogus
 * message about color cell allocation if sharecolor.
 *
 * Revision 1.9  1995/03/26  18:46:28  tom
 * Check for XMFRACTDIR then FRACTDIR in the environment,
 * add checks for gettimeofday parameters, add install
 * target and defaults to the configuration files.
 *
 * Revision 1.8  1995/03/25  08:42:49  darryl
 * Fixes and enhancements -- make the parameter selection
 * understand that if a parameter file is loaded, then
 * the ifs, lsys, or formula file does not need to be
 * re-selected. Display the current fractal type and
 * any associated file and file entries at the top of
 * the parameters screen. Delete some unused code.
 *
 * Revision 1.7  1995/03/24  17:03:23  tom
 * If no helpdir is specified use <fractdir>/help,
 * if the specified helpdir doesn't start with a '/',
 * use <fractdir>/<helpdir> else leave it alone.
 *
 * Revision 1.6  1995/03/23  20:06:30  darryl
 * Modify help, allow full-path names in the ini file,
 * fix a bug where default trig paramaters were not
 * set correctly for some types.
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
 * Revision 1.1.1.1  1995/03/11  03:12:58  tom
 * renamed dysize to dy_size and dxsixe to dx_size
 * (init_memsegs): init trigfn and choices
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.303  1995/02/28  18:49:20  darryl
 * Modifications for Linux.
 *
 * Revision 0.302  1995/01/30  20:55:14  darryl
 * Ensure that the background of the screen is black
 * when color map is shared with other clients.
 *
 * Revision 0.301  1995/01/17  00:03:30  darryl
 * Add a disclaimer dialog.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.104  1994/12/01  20:35:03  darryl
 * Add periodicity checking, symmetry, rseed, and showdot to
 * the extended options dialog so they do not have to be
 * set only on the command line or in a parameter file.
 *
 * Revision 0.103  1994/11/24  07:27:15  darryl
 * Initialize helpmode.
 *
 * Revision 0.102  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
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
