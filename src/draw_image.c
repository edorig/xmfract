
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: draw_image.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void set_refresh_rate
  (Widget w, XtPointer client_data, XtPointer call_data);
static void refresh_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void refresh_help_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void refresh_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void refresh_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void refresh_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_resize_mode
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget refresh_rate_shell = NULL;
static Widget refresh_rate_scale = NULL;
static Widget draw_fractal_submenu_btn1 = NULL;
static Widget draw_fractal_submenu_btn2 = NULL;

void
create_draw_fractal_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget draw_fractal_button;
  Widget draw_fractal_menu_toc;
  Widget draw_fractal_menu_pane;
  Widget draw_fractal_submenu_toc;
  Widget draw_fractal_submenu_pane;
  Widget menu_button;
  XmString str;
  XmString accel;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  draw_fractal_menu_pane = XmCreatePulldownMenu
    (w, "draw_fractal_menu_pane", args, n);

  /* get the tear-off control */

  if((draw_fractal_menu_toc = XmGetTearOffControl
    (draw_fractal_menu_pane)) != NULL)
    {
    XtVaSetValues(draw_fractal_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Image' cascade button */

  n = 0;
  str = XmStringCreateLocalized("Image");
  XtSetArg(args[n], XmNsubMenuId, draw_fractal_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  draw_fractal_button = XmCreateCascadeButton
    (w, "draw_fractal_button", args, n);

  XtManageChild(draw_fractal_button);
  XmStringFree(str);

  /*
   * create pushbuttons and/or cascade pushbuttons for the 'New Image' menu
   *
   * Create Image
   * 3D transform from file...
   * Set image refresh rate...
   * Window resize mode ->
   *         Clear window when resized
   *         Do not clear when resized
   */

  n = 0;
  str = XmStringCreateLocalized("Create Image");
  accel = XmStringCreateLocalized("[ d ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>d:"); n++;

  draw_fractal_menu_btn1 = XmCreatePushButton
    (draw_fractal_menu_pane, "draw_fractal_menu_btn1", args, n);

  XtAddCallback(draw_fractal_menu_btn1, XmNactivateCallback,
    create_image_cb, (XtPointer) NULL);

  XtManageChild(draw_fractal_menu_btn1);

  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("3d transform from file...");
  accel = XmStringCreateLocalized("[ 3 ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>3:"); n++;

  menu_button = XmCreatePushButton
    (draw_fractal_menu_pane, "three_d_transform_btn", args, n);

  XtAddCallback(menu_button, XmNactivateCallback,
    load_transform_cb, (XtPointer) NULL);

  XtManageChild(menu_button);
  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Set image refresh rate...");
  accel = XmStringCreateLocalized("[ R ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "Shift<Key>r:"); n++;

  menu_button = XmCreatePushButton
    (draw_fractal_menu_pane, "refresh_rate_menu_btn", args, n);

  XtAddCallback(menu_button, XmNactivateCallback,
    set_refresh_rate, (XtPointer) NULL);

  XtManageChild(menu_button);
  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  draw_fractal_submenu_pane = XmCreatePulldownMenu
   (draw_fractal_button, "draw_fractal_submenu_pane", args, n);

  /* get the tear-off control */

  if((draw_fractal_submenu_toc = XmGetTearOffControl
    (draw_fractal_submenu_pane)) != NULL)
    {
    XtVaSetValues(draw_fractal_submenu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  n = 0;
  str = XmStringCreateLocalized("Window resize mode");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNsubMenuId, draw_fractal_submenu_pane); n++;

  menu_button = XmCreateCascadeButton
    (draw_fractal_menu_pane, "menu_button", args, n);

  XtManageChild(menu_button);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Clear window when resized");
  XtSetArg(args[n], XmNlabelString, str); n++;
  draw_fractal_submenu_btn1 = XmCreateToggleButton
    (draw_fractal_submenu_pane, "draw_fractal_submenu_btn1", args, n);

  XtAddCallback(draw_fractal_submenu_btn1, XmNvalueChangedCallback,
    toggle_resize_mode, (XtPointer) "0");

  XtManageChild(draw_fractal_submenu_btn1);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Do not clear when resized");
  XtSetArg(args[n], XmNlabelString, str); n++;
  draw_fractal_submenu_btn2 = XmCreateToggleButton
    (draw_fractal_submenu_pane, "draw_fractal_submenu_btn2", args, n);

  XtAddCallback(draw_fractal_submenu_btn2, XmNvalueChangedCallback,
    toggle_resize_mode, (XtPointer) "1");

  XtManageChild(draw_fractal_submenu_btn2);
  XmStringFree(str);

  if(resize_mode == 1)
    {
    XtVaSetValues(draw_fractal_submenu_btn1, XmNset, False, NULL);
    XtVaSetValues(draw_fractal_submenu_btn2, XmNset, True, NULL);
    }
  else
    {
    XtVaSetValues(draw_fractal_submenu_btn1, XmNset, True, NULL);
    XtVaSetValues(draw_fractal_submenu_btn2, XmNset, False, NULL);
    }

}

/*ARGSUSED*/
static void set_refresh_rate(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget refresh_rate_dialog = NULL;
  Widget refresh_dflt_button;
  Widget refresh_done_button;
  Widget refresh_help_button;
  Widget refresh_cancel_button;
  Widget refresh_reset_button;
  Arg args[MAX_ARGS];
  XmString str;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 5 - 1;

  if(!refresh_rate_shell)
    {
    refresh_rate_shell = XmCreateDialogShell
      (main_window, "refresh_rate_shell", NULL, 0);

    XtAddCallback(refresh_rate_shell, XmNpopupCallback,  map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(refresh_rate_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Set Window Refresh Rate");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    refresh_rate_dialog = XmCreateForm
      (refresh_rate_shell, "refresh_rate_dialog", args, n);

    XtAddCallback(refresh_rate_dialog, XmNmapCallback, map_dialog, NULL);

    XmStringFree(str);

    /*
     * 10% <--------  Slower     Faster  --------> 100%
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized
      ("1% <--------  Slower     Faster  --------> 100%");
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    refresh_rate_scale = XmCreateScale
      (refresh_rate_dialog, "refresh_rate_scale", args, n);

    XtManageChild(refresh_rate_scale);

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
    XtSetArg(args[n], XmNleftPosition, 0); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, refresh_rate_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    refresh_help_button = XmCreatePushButton
      (refresh_rate_dialog, "refresh_help_button", args, n);

    XtAddCallback(refresh_help_button, XmNactivateCallback,
      refresh_help_button_cb, NULL);

    XtManageChild(refresh_help_button);

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
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, refresh_rate_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    refresh_reset_button = XmCreatePushButton
      (refresh_rate_dialog, "refresh_reset_button", args, n);

    XtAddCallback(refresh_reset_button, XmNactivateCallback,
      refresh_reset_button_cb, NULL);

    XtManageChild(refresh_reset_button);

    XmStringFree(str);

    /*
     * Default
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Default");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, refresh_rate_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    refresh_dflt_button = XmCreatePushButton
      (refresh_rate_dialog, "refresh_dflt_button", args, n);

    XtAddCallback(refresh_dflt_button, XmNactivateCallback,
      refresh_dflt_button_cb, NULL);

    XtManageChild(refresh_dflt_button);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, refresh_rate_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    refresh_cancel_button = XmCreatePushButton
      (refresh_rate_dialog, "refresh_cancel_button", args, n);

    XtAddCallback(refresh_cancel_button, XmNactivateCallback,
      refresh_cancel_button_cb, NULL);

    XtManageChild(refresh_cancel_button);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, refresh_rate_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    refresh_done_button = XmCreatePushButton
      (refresh_rate_dialog, "refresh_done_button", args, n);

    XtAddCallback(refresh_done_button, XmNactivateCallback,
      refresh_done_button_cb, NULL);

    XtManageChild(refresh_done_button);
    XmStringFree(str);

    XtVaSetValues(refresh_rate_dialog,
      XmNdefaultButton,
      refresh_done_button,
      NULL);
    }

  XtVaSetValues(refresh_rate_scale, XmNvalue, refresh_rate, NULL);

  XtManageChild(refresh_rate_dialog);
  XtManageChild(refresh_rate_shell);
  XtPopup(refresh_rate_shell, XtGrabNone);
}

/*ARGSUSED*/
static void refresh_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(refresh_rate_scale, refresh_rate);
}

/*ARGSUSED*/
static void refresh_help_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPLOADFILE;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void refresh_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleGetValue(refresh_rate_scale, &(refresh_rate));
  refresh_timeout = (int) ((sxdots / refresh_rate) * sydots);
  XtPopdown(refresh_rate_shell);
}

/*ARGSUSED*/
static void refresh_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(refresh_rate_scale, 100);
}

/*ARGSUSED*/
static void refresh_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(refresh_rate_shell != NULL)
    XtPopdown(refresh_rate_shell);
}

/*ARGSUSED*/
static void toggle_resize_mode(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int which = atoi((char *)client_data);

  if(which == 1)
    {
    resize_mode = 1;
    XtVaSetValues(draw_fractal_submenu_btn1, XmNset, False, NULL);
    XtVaSetValues(draw_fractal_submenu_btn2, XmNset, True, NULL);
    }
  else
    {
    resize_mode = 0;
    XtVaSetValues(draw_fractal_submenu_btn1, XmNset, True, NULL);
    XtVaSetValues(draw_fractal_submenu_btn2, XmNset, False, NULL);
    }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: draw_image.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.11  1995/06/13  17:51:35  darryl
 * Add window resize mode function.
 *
 * Revision 1.10  1995/05/17  16:28:45  darryl
 * Add an accelerator ('R') for the Refresh Rate menu item.
 *
 * Revision 1.9  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.8  1995/05/03  05:48:27  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.7  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
 *
 * Revision 1.6  1995/04/11  15:37:45  darryl
 * Remove hard-coded tearOffModel resources to allow the
 * resource file to over-ride the fallback resource.
 *
 * Revision 1.5  1995/04/10  22:12:57  darryl
 * Add rate=nnn and menu support for setting the screen refresh
 * rate during image calculation. Fix a bug where the Julia
 * inverse mode was broken after an image save. Add fallback
 * resources that take effect if a resource file is not found.
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
 * Revision 0.103  1994/11/30  21:09:19  darryl
 * Set the XmNseparatorType resource for TearOffControls.
 *
 * Revision 0.102  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.101  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
