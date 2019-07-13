
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: authors_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void authors_dialog_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void disclaimer_dialog_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void rotate_labels(XtPointer client_data, XtIntervalId Id);

/* static variables */
static int finished = 0;
static int popped_up = 0;
static Widget disclaimer_dialog_shell = NULL;
static Widget disclaimer_dialog = NULL;
static Widget authors_dialog_shell = NULL;
static Widget authors_dialog = NULL;
static XtIntervalId autimeout;

static char *primaries[] =
  {
  "Bert Tyler     [73477,433]              <73477.433@compuserve.com>",
  "Timothy Wegner [71320,675]              <71320.675@compuserve.com>",
  "Jonathan Osuch [73277,1432]             <73277.1432@compuserve.com>",
  "Wesley Loewer                           <loewer@tenet.edu>",
   NULL
  };

static char *xmprimaries[] =
  {
  "Darryl House   <darryl@chc.bpss.eds.com>",
   NULL
  };

static char *xmcontribs[] =
  {
  "Thomas Winder  <tom@vlsivie.tuwien.ac.at> autoconf, many enhancements",
  "Noel Giffin    <noel@erich.triumf.ca> excellent suggestions, beta testing",
   NULL
  };

static char *contribs[] =
  {
  "Michael Abrash    360x480x256, 320x400x256 VGA video modes",
  "Joseph Albrecht   Tandy video, CGA video speedup",
  "Kevin Allen       kevina@microsoft.com Finite attractor, bifurcation engine",
  "Steve Bennett     restore-from-disk logic",
  "Rob Beyer         [71021,2074] Barnsley IFS, Lorenz fractals",
  "Francois Blais    [70700,446] Lyapunov Fractals, LYAPUNOV.MAP",
  "Dennis Bragg      [75300,2456] DXF Raytracing output option",
  "Pieter Branderhorst [72611,2257] Past main author, solid guessing, menus",
  "Juan J. Buhler    jbuhler@usina.org.ar Diffusion options, inverse Julia",
  "Mike Burkey       376x564x256, 400x564x256, and 832x612x256 VGA video modes",
  "Robin Bussell     Palette-editor 'freestyle' option",
  "John Bridges      [75300,2137] superVGA support, 360x480x256 mode",
  "Byte Magazine     Tweaked VGA Modes",
  "Brian Corbino     [71611,702] Tandy 1000 640x200x16 video mode",
  "Lee Crocker       [73407,2030] Fast Newton, Inversion, Decomposition..",
  "Monte Davis       [71450,3542] Documentation",
  "Paul De Leeuw     RDS (Random Dot Stereogram) Feature",
  "Chuck Ebbert      [76306,1226] cmprsd & sqrt logmap, fpu speedups",
  "Dan Farmer        [70703,1632] orbits enhancements",
  "Richard Finegold  [76701,153] 8/16/../256-Way Decomposition option",
  "Frank Fussenegger Mandelbrot speedups",
  "Mike Gelvin       [73337,520] Mandelbrot speedups",
  "MS-Kermit         Keyboard Routines",
  "Lawrence Gozum    [73437,2372] Tseng 640x400x256 Video Mode",
  "David Guenther    [70531,3525] Boundary Tracing algorithm",
  "Norman Hills      [71621,1352] Ranges option",
  "Richard Hughes    [70461,3272] 'inside=', 'outside=' coloring options",
  "Mike Kaufman      [kaufman@eecs.nwu.edu] mouse support, other features",
  "Wesley Loewer     fast Mandel/Julia/Lyapunov, boundary trace, frothybasin",
  "Adrian Mariano    [adrian@cam.cornell.edu] Diffusion & L-Systems",
  "Charles Marslett  [75300,1636] VESA video and IIT math chip support",
  "Joe McLain        [75066,1257] TARGA Support, color-map files",
  "Bob Montgomery    [73357,3140] (Author of VPIC) Fast text I/O routines",
  "Bret Mulvey       plasma clouds",
  "Roy Murphy        [76376,721] Lyapunov Fractals",
  "Ethan Nagel       [70022,2552] Palette editor, integrated help/doc system",
  "Yavuz Onder       yavuz@bnr.ca Postscript printer driver",
  "Jonathan Osuch    [73277,1432] IIT detect, register-compatible 8514/A code",
  "Mark Peterson     [70441,3353] Past main author, parser, julibrot",
  "PC Magazine       Sound Routines",
  "PC Tech Journal   CPU, FPU Detectors",
  "Marc Reinig       [72410,77] Lots of 3D options",
  "Kyle Powell       [76704,12] 8514/A Support",
  "Matt Saucier      [72371,3101] Printer Support",
  "Herb Savage       [75260,217] 'inside=bof60', 'inside=bof61' options",
  "Ken Shirriff      shirriff@sprite.berkeley.edu Quaternions, CA, Xfract port",
  "Lee Skinner       [75450,3631] Tetrate fractal types and more",
  "Dean Souleles     [75115,1671] Hercules Support",
  "Kurt Sowa         [73467,2013] Color Printer Support",
  "Hugh Steele       cyclerange feature",
  "John Swenson      [75300,2136] Postscript printer features",
  "Chris Taylor      Floating&Fixed-point algorithm speedups, Tesseral Option",
  "Scott Taylor      [72401,410] PostScript, Kam Torus, many fn types.",
  "Bill Townsend     Mandelbrot Speedups",
  "Paul Varner       [73237,441] Extended Memory support for Disk Video",
  "Dave Warker       Integer Mandelbrot Fractals concept",
  "Aaron Williams    Register-compatible 8514/A code",
  "Phil Wilson       [76247,3145] Distance Estimator, Bifurcation fractals",
  "Nicholas Wilt     Lsystem speedups",
  "Richard Wilton    Tweaked VGA Video modes",
   NULL
};

/*ARGSUSED*/
void display_authors_window_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(authors_dialog_shell != NULL)
    if(popped_up == 1)
      return;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!authors_dialog_shell)
    {
    char tempstr[64];
    Widget frame;
    Widget form;
    Widget button;
    Widget label;
    Arg args[MAX_ARGS];
    XmString str;
    int fracbase;
    int btnbase;
    int i, n;

    btnbase = 20;
    fracbase = btnbase * 4 - 1;

    authors_dialog_shell = XmCreateDialogShell
      (main_window, "Stone Soup Group", NULL, 0);

    XtAddCallback(authors_dialog_shell, XmNpopupCallback, map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(authors_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Stone Soup Group");
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    authors_dialog = XmCreateForm
      (authors_dialog_shell, "authors_dialog", args, n);

    XtAddCallback(authors_dialog, XmNmapCallback, map_dialog, NULL);

    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (authors_dialog, "primary_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Fractint Primary Authors");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "primary_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;

    form = XmCreateForm(frame, "primary_form", args, n);

    for(i = 0; primaries[i] != NULL; i++)
      {
      n = 0;
      str = XmStringCreateLocalized(primaries[i]);
      XtSetArg(args[n], XmNlabelString, str); n++;
      if( i == 0 )
        {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, label); n++;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        }
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;

      sprintf((char *) &tempstr, "primaries_label_%d", i);

      label = XmCreateLabel(form, (char *) &tempstr, args, n);

      XtManageChild(label);
      XmStringFree(str);
      }

    XtManageChild(form);
    XtManageChild(frame);

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (authors_dialog, "xmprimary_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Xmfract Primary Authors");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "xmprimary_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;

    form = XmCreateForm(frame, "xmprimary_form", args, n);

    for(i = 0; xmprimaries[i] != NULL; i++)
      {
      n = 0;
      str = XmStringCreateLocalized(xmprimaries[i]);
      XtSetArg(args[n], XmNlabelString, str); n++;
      if( i == 0 )
        {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, label); n++;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        }
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;

      sprintf((char *) &tempstr, "xmprimaries_label_%d", i);

      label = XmCreateLabel(form, (char *) &tempstr, args, n);

      XtManageChild(label);
      XmStringFree(str);
      }

    XtManageChild(form);
    XtManageChild(frame);

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (authors_dialog, "xmcontrib_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Xmfract Contributing Authors");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "xmcontrib_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;

    form = XmCreateForm(frame, "xmcontrib_form", args, n);

    for(i = 0; xmcontribs[i] != NULL; i++)
      {
      n = 0;
      str = XmStringCreateLocalized(xmcontribs[i]);
      XtSetArg(args[n], XmNlabelString, str); n++;
      if( i == 0 )
        {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, label); n++;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        }
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;

      sprintf((char *) &tempstr, "xmcontribs_label_%d", i);

      label = XmCreateLabel(form, (char *) &tempstr, args, n);

      XtManageChild(label);
      XmStringFree(str);
      }

    XtManageChild(form);
    XtManageChild(frame);

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (authors_dialog, "contrib_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Fractint Contributing Authors");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "contrib_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;

    form = XmCreateForm(frame, "contrib_form", args, n);

    for(i = 0; i < 10; i++)
      {
      str = XmStringCreateLocalized(contribs[i]);
      XtSetArg(args[n], XmNlabelString, str); n++;
      if( i == 0 )
        {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, label); n++;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        }
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
      sprintf((char *) &tempstr, "contrib_label_%d", i);
      label = XmCreateLabel(form, (char *) &tempstr, args, n);
      XtManageChild(label);
      XmStringFree(str);
      }

    XtManageChild(form);
    XtManageChild(frame);

    n = 0;
    str = XmStringCreateLocalized("Dismiss authors window");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (authors_dialog, "authors_dialog_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      authors_dialog_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(authors_dialog, XmNdefaultButton, button, NULL);

    XtManageChild(authors_dialog);
    XtManageChild(authors_dialog_shell);
    }

  finished = 0;
  popped_up = 1;
  XtManageChild(authors_dialog);
  XtManageChild(authors_dialog_shell);
  XtPopup(authors_dialog_shell, XtGrabNone);
  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
  autimeout = XtAppAddTimeOut
    (app, 1000, (XtTimerCallbackProc) rotate_labels, NULL);
}

/*ARGSUSED*/
static void rotate_labels(XtPointer client_data, XtIntervalId Id)
{
  int i;
  Widget label;
  XmString str;
  char tempstr[64];
  static int j = 0;

  /*
   * doesn't really cycle, just scrolls to the end and starts over
   */

  for(i = 0, j = (j - 9 < 0 ? 0 : j - 9); i < 10; i++)
    {
    if(contribs[++j] == NULL) j = 0;
    str = XmStringCreateLocalized(contribs[j]);
    sprintf((char *) &tempstr, "*contrib_label_%d", i);
    label = XtNameToWidget(authors_dialog_shell, (char *) &tempstr);
    if(label != NULL)
      XtVaSetValues(label, XmNlabelString, str, NULL);
    XmStringFree(str);
    }

  if(!finished)
    autimeout = XtAppAddTimeOut
      (app, 1000, (XtTimerCallbackProc) rotate_labels, NULL);
}

/*ARGSUSED*/
static void authors_dialog_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(authors_dialog_shell);
  XtRemoveTimeOut(autimeout);
  popped_up = 0;
  finished = 1;
}

/*ARGSUSED*/
void display_disclaimer(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!disclaimer_dialog_shell)
    {
    int n;
    Widget label;
    Widget frame;
    XmString str;
    Widget button;
    XmString dstr[6];
    char tmpstr[512];
    Arg args[MAX_ARGS];
    int fracbase;
    int btnbase;

    btnbase = 20;
    fracbase = btnbase * 4 - 1;

    disclaimer_dialog_shell = XmCreateDialogShell
      (main_window, "disclaimer_dialog_shell", NULL, 0);

    XtAddCallback(disclaimer_dialog_shell, XmNpopupCallback, map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(disclaimer_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Disclaimer");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    disclaimer_dialog = XmCreateForm
      (disclaimer_dialog_shell, "disclaimer_dialog", args, n);

    XtAddCallback(disclaimer_dialog, XmNmapCallback, map_dialog, NULL);

    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNtopPosition, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (disclaimer_dialog, "disclaimer_frame", args, n);

    sprintf((char *) &tmpstr, "\nxmfract version %.1f - a freeware fractal generation program.\n\nCopyright (C) 1995 Darryl House and the Stone Soup Group.\n\n", version);

    dstr[1] = XmStringCreateLtoR((char *) &tmpstr, XmFONTLIST_DEFAULT_TAG);

    dstr[2] = XmStringCreateLtoR("This program is free software; you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation; either version 2 of the License, or\n(at your option) any later version.\n\n", XmFONTLIST_DEFAULT_TAG);

    dstr[3] = XmStringCreateLtoR("This program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\nSelect \"GNU Public License\" from the \"Help\" menu to view the license.\n\n", XmFONTLIST_DEFAULT_TAG);

    dstr[4] = XmStringCreateLtoR("Some source code for this program is covered by additional copyrights.\nSee the copyright notices in the source files for more information.\n\n", XmFONTLIST_DEFAULT_TAG);

    dstr[5] = XmStringCreateLtoR("You should have received a machine-readable and/or hard copy of the\nGNU General Public License along with this program; if not, write to the\nFree Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n", XmFONTLIST_DEFAULT_TAG);

    dstr[0] = XmStringConcat(dstr[1], dstr[2]);
    dstr[0] = XmStringConcat(dstr[0], dstr[3]);
    dstr[0] = XmStringConcat(dstr[0], dstr[4]);
    dstr[0] = XmStringConcat(dstr[0], dstr[5]);

    n = 0;
    XtSetArg(args[n], XmNlabelString, dstr[0]); n++;
    label = XmCreateLabel(frame, "copyright_label", args, n);

    XtManageChild(label);
    XtManageChild(frame);

    XmStringFree(dstr[0]);
    XmStringFree(dstr[1]);
    XmStringFree(dstr[2]);
    XmStringFree(dstr[3]);
    XmStringFree(dstr[4]);
    XmStringFree(dstr[5]);

    n = 0;
    str = XmStringCreateLocalized("Acknowledged");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (disclaimer_dialog, "disclaimer_dialog_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      disclaimer_dialog_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(disclaimer_dialog, XmNdefaultButton, button, NULL);

    XtManageChild(disclaimer_dialog);
    XtManageChild(disclaimer_dialog_shell);
    }

  XtManageChild(disclaimer_dialog);
  XtManageChild(disclaimer_dialog_shell);
  XtPopup(disclaimer_dialog_shell, XtGrabNone);
  XDefineCursor(display, window, XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
void disclaimer_dialog_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(disclaimer_dialog_shell);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: authors_menu.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.11  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.10  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
 *
 * Revision 1.9  1995/05/03  01:58:14  darryl
 * Add Noel Giffin to the contributing authors menu.
 *
 * Revision 1.8  1995/05/03  01:28:39  darryl
 * Fix bug where timeout was not being removed properly.
 *
 * Revision 1.7  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.6  1995/04/11  15:37:45  darryl
 * Remove hard-coded tearOffModel resources to allow the
 * resource file to over-ride the fallback resource.
 *
 * Revision 1.5  1995/03/26  22:28:18  darryl
 * Add buttons to the type selection dialogs for returning
 * to a file or name selection dialog if the type is
 * formula, lsystem, or ifs. Update the help dialog so that
 * only one may be displayed at a time. Display a clock
 * cursor while an image is being generated.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.2  1995/03/10  19:32:17  darryl
 * Fix widget heirarchy problem in disclaimer dialog.
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.306  1995/02/08  20:42:59  darryl
 * Fix typo in disclaimer message.
 *
 * Revision 0.305  1995/02/08  16:54:53  darryl
 * Update the copyright notice.
 *
 * Revision 0.304  1995/01/24  22:24:12  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.303  1995/01/17  19:38:14  darryl
 * Change the format of the disclaimer message.
 *
 * Revision 0.302  1995/01/17  17:15:48  darryl
 * Lint and optimization cleanup.
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
 * Revision 0.107  1994/11/30  21:09:19  darryl
 * Set the XmNseparatorType resource for TearOffControls.
 *
 * Revision 0.106  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.105  1994/11/30  02:21:38  darryl
 * Change the accelerator letter from 'a' to 'w'
 * to resolve a conflict with the starfield menu.
 *
 * Revision 0.104  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.103  1994/11/26  02:39:59  darryl
 * Expand authors dialog.
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
