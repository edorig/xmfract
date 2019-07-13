
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: star_params.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void starfield_params_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void star_params_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void starfield_params_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void starfield_params_dflt_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void starfield_params_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget starfield_params_dialog_shell = NULL;
static Widget starfield_params_dialog = NULL;

/*ARGSUSED*/
void get_starfield_params(Widget w)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!starfield_params_dialog_shell)
    {
    Widget button;
    Arg args[MAX_ARGS];
    XmString str;
    char tempstr[64];
    int fracbase;
    int btnbase;
    int n;

    btnbase = 20;
    fracbase = btnbase * 6 - 1;

    starfield_params_dialog_shell = XmCreateDialogShell
      (main_window, "Starfield Parameters", NULL, 0);

    XtAddCallback(starfield_params_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(starfield_params_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Starfield Parameters");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    starfield_params_dialog = XmCreateForm
      (starfield_params_dialog_shell, "starfield_params_dialog", args, n);

    XtAddCallback(starfield_params_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    /*
     * Star Density in Pixels per Star
     * (scale)
     */

    sprintf((char *) &tempstr, "Star Density in Pixels per Star");
    str = XmStringCreateLocalized((char *) &tempstr);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNvalue,
      (int)(starfield_opts.current.starfield_values[0])); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    starfield_opts.starfield_density_scale = XmCreateScale
      (starfield_params_dialog, "starfield_density_scale", args, n);

    XtManageChild(starfield_opts.starfield_density_scale);

    XmStringFree(str);

    /*
     * Percent Clumpiness
     * (scale)
     */

    sprintf((char *) &tempstr, "Percent Clumpiness");
    str = XmStringCreateLocalized((char *) &tempstr);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNvalue,
      (int)(starfield_opts.current.starfield_values[1])); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_density_scale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    starfield_opts.starfield_clumpiness_scale = XmCreateScale
      (starfield_params_dialog, "starfield_clumpiness_scale", args, n);

    XtManageChild(starfield_opts.starfield_clumpiness_scale);

    XmStringFree(str);

    /*
     * Ratio of Dim stars to Bright
     * (scale)
     */

    sprintf((char *) &tempstr, "Ratio of Dim stars to Bright");
    str = XmStringCreateLocalized((char *) &tempstr);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNvalue,
      (int)(starfield_opts.current.starfield_values[2])); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_clumpiness_scale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    starfield_opts.starfield_dim_ratio_scale = XmCreateScale
      (starfield_params_dialog, "starfield_dim_ratio_scale", args, n);

    XtManageChild(starfield_opts.starfield_dim_ratio_scale);

    XmStringFree(str);

    /*
     * Help
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_dim_ratio_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (starfield_params_dialog, "starfield_params_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      starfield_params_help_btn_cb, NULL );

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Apply
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Apply");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_dim_ratio_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (starfield_params_dialog, "starfield_params_apply_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      starfield_params_done_btn_cb, (XtPointer) "0");

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Reset
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Reset");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_dim_ratio_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (starfield_params_dialog, "starfield_params_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      starfield_params_reset_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Defaults
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Defaults");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_dim_ratio_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (starfield_params_dialog, "starfield_params_dialog_dflt_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      starfield_params_dflt_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Cancel
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Cancel");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_dim_ratio_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (starfield_params_dialog, "starfield_params_dialog_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      star_params_cancel_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Done
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Done");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 5); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,
      starfield_opts.starfield_dim_ratio_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 5 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (starfield_params_dialog, "starfield_params_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      starfield_params_done_btn_cb, (XtPointer) "1");

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(starfield_params_dialog,
      XmNdefaultButton,
      button,
      NULL);

    XtManageChild(starfield_params_dialog);
    XtManageChild(starfield_params_dialog_shell);
    }

  starfield_opts.current.starfield_values[0] = starfield_values[0];
  starfield_opts.current.starfield_values[1] = starfield_values[1];
  starfield_opts.current.starfield_values[2] = starfield_values[2];

  XmScaleSetValue(starfield_opts.starfield_density_scale,
    (int)(starfield_opts.current.starfield_values[0]));

  XmScaleSetValue(starfield_opts.starfield_clumpiness_scale,
    (int)(starfield_opts.current.starfield_values[1]));

  XmScaleSetValue(starfield_opts.starfield_dim_ratio_scale,
    (int)(starfield_opts.current.starfield_values[2]));

  XtManageChild(starfield_params_dialog);
  XtManageChild(starfield_params_dialog_shell);
  XtPopup(starfield_params_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void starfield_params_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  starfield_opts.current.starfield_values[0] = starfield_values[0];
  starfield_opts.current.starfield_values[1] = starfield_values[1];
  starfield_opts.current.starfield_values[2] = starfield_values[2];

  XmScaleSetValue(starfield_opts.starfield_density_scale,
    (int)(starfield_opts.current.starfield_values[0]));

  XmScaleSetValue(starfield_opts.starfield_clumpiness_scale,
    (int)(starfield_opts.current.starfield_values[1]));

  XmScaleSetValue(starfield_opts.starfield_dim_ratio_scale,
    (int)(starfield_opts.current.starfield_values[2]));
}

/*ARGSUSED*/
static void star_params_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(starfield_params_dialog_shell);
}

/*ARGSUSED*/
static void starfield_params_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPSTARFLD;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void starfield_params_dflt_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  starfield_opts.current.starfield_values[0] =
    starfield_opts.dflt.starfield_values[0];
  starfield_opts.current.starfield_values[1] =
    starfield_opts.dflt.starfield_values[1];
  starfield_opts.current.starfield_values[2] =
    starfield_opts.dflt.starfield_values[2];

  XmScaleSetValue(starfield_opts.starfield_density_scale,
    (int)(starfield_opts.current.starfield_values[0]));

  XmScaleSetValue(starfield_opts.starfield_clumpiness_scale,
    (int)(starfield_opts.current.starfield_values[1]));

  XmScaleSetValue(starfield_opts.starfield_dim_ratio_scale,
    (int)(starfield_opts.current.starfield_values[2]));
}

/*ARGSUSED*/
static void starfield_params_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int tmp;

  XmScaleGetValue(starfield_opts.starfield_density_scale, &tmp);
  starfield_opts.current.starfield_values[0] = (double) tmp;

  XmScaleGetValue(starfield_opts.starfield_clumpiness_scale, &tmp);
  starfield_opts.current.starfield_values[1] = (double) tmp;

  XmScaleGetValue(starfield_opts.starfield_dim_ratio_scale, &tmp);
  starfield_opts.current.starfield_values[2] = (double) tmp;

  starfield_values[0] = starfield_opts.current.starfield_values[0];
  starfield_values[1] = starfield_opts.current.starfield_values[1];
  starfield_values[2] = starfield_opts.current.starfield_values[2];

  XtPopdown(starfield_params_dialog_shell);

  starfield();
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: star_params.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.10  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.9  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.8  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.7  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.6  1995/03/21  00:34:23  darryl
 * Add "Apply" buttons to allow values in a popped-up
 * dialog to be applied to the program without having
 * to press the "Done" button, which pops down the dialog.
 *
 * Revision 1.5  1995/03/20  19:09:05  darryl
 * Lint and optimization cleanup.
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
 * Revision 0.103  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.102  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.101  1994/11/18  17:09:57  darryl
 * Aesthetic changes - widget and label placement. Minor bug
 * fixes when saving parameters and drawing a zoom box.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
