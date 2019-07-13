
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#include "xmfract.xbm"

#ifndef NO_IDENT
#ident "$Id: xmfract.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static String fallback_resources[] =
{
#include "Xmfract.ad.h"
NULL
};

void
main(int argc, char **argv)
{
  int n;
  long xmversion;
  XtActionsRec acts;
  Pixmap icon_pixmap = None;
  Arg arg;

  String xlate =
    "<Btn1Down>:   rubber_band(dn) ManagerGadgetArm() \n\
     <Btn1Up>:     rubber_band(up) ManagerGadgetActivate() \n\
     <Btn1Motion>: rubber_band(mv) ManagerGadgetButtonMotion()";

  /*
   * Because we make calls that are not included in Motif 1.1,
   * ensure that the version of Motif that this machine is
   * running is Motif 1.2 or later and greater.
   */

  if((xmversion = XmVersion) < 1002)
    {
    fprintf(stderr, "\n%s requires Motif version 1.2 or greater.\n", argv[0]);
    fprintf(stderr, "This machine is running Motif version %d.%d.%d\n\n",
      XmVERSION, XmREVISION, XmUPDATE_LEVEL);
    exit(1);
    }

  XtSetLanguageProc(NULL, NULL, NULL);

  n = 0;
  toplevel = XtVaAppInitialize
    (&app, "Xmfract", NULL, 0, &argc, argv,
    fallback_resources,
    XmNminWidth, 80,
    XmNminHeight, 60,
    NULL);

  /*
   * Install the resource converter for the tear-off menu resource
   */

  XmRepTypeInstallTearOffModelConverter(); /* whew! */

  /*
   * Install a signal handler for floating point math exceptions
   */

  {
#ifdef _POSIX_VERSION
  struct sigaction act;

  /* this is a pointer to a function which returns RETSIGTYPE - oh well ... */
  act.sa_handler = (RETSIGTYPE (*)()) fpe_handler;

  /* block signal from occuring twice - shouldn't happen but who
     knows */
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGFPE);
# ifdef SA_RESTART
  /* Make sure that the handler is in place after leaving the
     handler. Some systems do this as default, others - like linux -
     need this flag */
  act.sa_flags = SA_RESTART;
# else
  act.sa_flags = 0;
# endif
  (void) sigaction(SIGFPE, &act, (struct sigaction *)NULL);
#else
# ifdef SV_INTERRUPT
  struct sigvec vec;

  vec.sv_handler = (RETSIGTYPE (*)()) fpe_handler;
  vec.sv_mask = sigmask(SIGFPE);
  vec.sv_flags = 0;
  (void) sigvec(SIG_FPE, &vec, (struct sigvec *)NULL);

# else
  (void) signal(SIGFPE, fpe_handler);
# endif
#endif  /* _POSIX_VERSION */
  }


  /*
   * Create a main window
   */

  main_window = XmCreateMainWindow (toplevel, "main_window", NULL, 0);

  XtManageChild(main_window);

  /* Create menu bar in main window */

  menu_bar = XmCreateMenuBar (main_window, "menu_bar", NULL, 0);

  XtManageChild (menu_bar);

  /* Create menus */

  create_file_menu(menu_bar);
  create_options_menu(menu_bar);
  create_fractal_types_menu(menu_bar);
  create_draw_fractal_menu(menu_bar);
  create_zoom_menu(menu_bar);
  create_coloredit_menu(menu_bar);
  create_reset_menu(menu_bar);
  create_misc_menu(menu_bar);
  create_restore_menu(menu_bar);
  create_helps_menu(menu_bar);

  update_draw_menu(NEW_IMAGE);

  /*
   * Create a drawing surface
   */

  acts.string = "rubber_band";
  acts.proc = (XtActionProc) rubber_band;

  XtAppAddActions(app, &acts, 1);

  canvas = XtVaCreateManagedWidget
    ("canvas", xmDrawingAreaWidgetClass, main_window,
    XmNtranslations, XtParseTranslationTable(xlate),
    XmNwidth, 640,
    XmNheight, 480,
    NULL);

  XtAddCallback(canvas, XmNinputCallback, (XtCallbackProc) rubber_band, NULL);

  /*
   * Add event handlers
   */

  XtAddEventHandler(canvas,
    KeyPressMask|KeyReleaseMask|KeymapStateMask,
    True, (XtEventHandler) kbd_event, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(toplevel, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);

  XtAddEventHandler(main_window, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  XtRealizeWidget(toplevel);

  window = XtWindow(toplevel);
  gc = XCreateGC(XtDisplay(canvas), XtWindow(canvas), 0, NULL);
  display = XtDisplay(toplevel);
  screen_number = XDefaultScreen(display);
  screen = XScreenOfDisplay(display, screen_number);
  visual = XDefaultVisual(display, screen_number);

  /* set icon bitmap */
  XtSetArg(arg, XtNiconPixmap, &icon_pixmap);
  XtGetValues(toplevel, &arg, 1);
  if(icon_pixmap == None)
    {
    arg.value =
      (XtArgVal)XCreateBitmapFromData(display,
        RootWindowOfScreen(XtScreen(toplevel)),
        xmfract_bits, xmfract_width, xmfract_height);
    XtSetValues (toplevel, &arg, 1);
    }

  XCreateFontCursor(display, XC_top_left_arrow);

  /*
   * Ensure that resizes are handled.
   */

  XtAddCallback(canvas, XmNresizeCallback, resize_window, NULL);

  /*
   * Change the cursor to a watch cursor while initializing
   */

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_watch));

  /*
   * initialize all this stuff...
   */

  init_data(argc, argv);

 /*
  * Initialize some window size structure members
  */

  get_winsize_defaults();

  /*
   * Ensure the main window is visible before attempting to display
   * the credits screen and/or disclaimer notices.
   */

  force_update(main_window);

  /*
   * if not specifically prevented, display the credits screen
   */

  if(creditscreen != 0)
    {
    XtAppAddTimeOut(app, 250,
      (XtTimerCallbackProc) delay_display_authors_window, NULL);
    }

  /*
   * if not specifically prevented, display the disclaimer screen
   */

  if(disclaimer != 0)
    {
    XtAppAddTimeOut(app, 250,
      (XtTimerCallbackProc) delay_display_disclaimer, NULL);
    }

  /*
   * change the cursor back to an arrow
   */

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  /*
   * liftoff...
   */

  XtAppMainLoop(app);
}

void delay_display_authors_window(XtPointer client_data, XtIntervalId Id)
  {
  display_authors_window_cb(main_window, NULL, NULL);
  }

void delay_display_disclaimer(XtPointer client_data, XtIntervalId Id)
  {
  display_disclaimer(main_window, NULL, NULL);
  }

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: xmfract.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.8  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
 *
 * Revision 1.7  1995/04/18  15:59:59  darryl
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
 * Revision 1.6  1995/04/10  22:12:57  darryl
 * Add rate=nnn and menu support for setting the screen refresh
 * rate during image calculation. Fix a bug where the Julia
 * inverse mode was broken after an image save. Add fallback
 * resources that take effect if a resource file is not found.
 *
 * Revision 1.5  1995/03/29  21:19:37  tom
 * Add icon.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.2.1.1  1995/03/13  15:11:50  tom
 * changed signal handling
 *
 * Revision 1.2  1995/03/10  19:32:17  darryl
 * Add signal handler definition.
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.303  1995/02/04  22:41:42  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.302  1995/01/30  23:23:13  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.301  1995/01/17  00:03:30  darryl
 * Add a disclaimer dialog.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.204  1994/12/09  00:22:52  darryl
 * Change fg and bg of xgc if a new color map.
 *
 * Revision 0.203  1994/12/07  23:31:52  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.202  1994/12/07  17:30:43  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.201  1994/12/07  00:31:30  darryl
 * Change calls to rubber banding functions from a passive
 * pointer grab to translations from the canvas widget.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.106  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.105  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.104  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.103  1994/11/25  18:18:01  darryl
 * Move the event handlers for the rubber band functions
 * from the initialization section to the image creation
 * section so they can be installed and removed more easily.
 * Prevents event processing errors caused by delays in
 * posting events from the X server that would occasionally
 * make strange and interesting things to happen to the zoom box.
 *
 * Revision 0.102  1994/11/23  05:21:08  darryl
 * Initial implementation of help. Minor widget alignment
 * changes. Widget name changes for uniqueness.
 *
 * Revision 0.101  1994/11/21  22:15:32  darryl
 * Enforce an 80 x 60 minimum window size.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
