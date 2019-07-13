
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: light_params.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void light_src_dialog_dflt_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void light_src_dialog_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void light_src_dialog_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void light_src_dialog_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void light_src_dialog_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget light_src_params_dialog_shell = NULL;
static Widget light_src_params_dialog = NULL;

/*ARGSUSED*/
void get_light_params(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!light_src_params_dialog_shell)
    {
    Widget rowcol;
    Widget label;
    Widget scale;
    Widget radio_box;
    Widget button;
    XmString str;
    Arg args[MAX_ARGS];
    int fracbase;
    int btnbase;
    int n;

    static char *targa_overlay_radio_box_items[] =
      {
      "Yes",
      "No",
      };

    btnbase = 20;
    fracbase = btnbase * 5 - 1;

    light_src_params_dialog_shell = XmCreateDialogShell
      (main_window, "Light Source Parameters", NULL, 0);

    XtAddCallback(light_src_params_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(light_src_params_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Light Source Parameters");
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    light_src_params_dialog = XmCreateForm
      (light_src_params_dialog_shell,
      "light_src_params_dialog", args, n);

    XtAddCallback(light_src_params_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    /*
     * X value light vector
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("X value light vector");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 180); n++;
    XtSetArg(args[n], XmNminimum, -180); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "xlight_scale", args, n);

    three_d_opts.xlight_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Y value light vector
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Y value light vector");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 180); n++;
    XtSetArg(args[n], XmNminimum, -180); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "ylight_scale", args, n);

    three_d_opts.ylight_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Z value light vector
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Z value light vector");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 180); n++;
    XtSetArg(args[n], XmNminimum, -180); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "zlight_scale", args, n);

    three_d_opts.zlight_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Light Source Smoothing Factor
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Light Source Smoothing Factor");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 1000); n++;
    XtSetArg(args[n], XmNminimum, -1000); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "lightavg_scale", args, n);

    three_d_opts.lightavg_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Ambient
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Ambient");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 1000); n++;
    XtSetArg(args[n], XmNminimum, -1000); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "ambient_scale", args, n);

    three_d_opts.ambient_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Haze
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Haze");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 1000); n++;
    XtSetArg(args[n], XmNminimum, -1000); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "haze_scale", args, n);

    three_d_opts.haze_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Background color (red value)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Background color (red value)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "bg_red_scale", args, n);

    three_d_opts.bg_red_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Background color (green value)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Background color (green value)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "bg_green_scale", args, n);

    three_d_opts.bg_green_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Background color (blue value)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Background color (blue value)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (light_src_params_dialog, "bg_blue_scale", args, n);

    three_d_opts.bg_blue_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Overlay Targa File?
     * (radio box)
     *
     * Yes
     * No
     */

    n = 0;
    rowcol =
      XtVaCreateManagedWidget("targa_overlay_radio_box_rc",
      xmRowColumnWidgetClass,
      light_src_params_dialog,
      XmNtopWidget, scale,
      XmNtopAttachment, XmATTACH_WIDGET,
      XmNleftAttachment, XmATTACH_POSITION,
      XmNleftPosition, 1,
      XmNrightAttachment, XmATTACH_POSITION,
      XmNrightPosition, fracbase - 1,
      XmNorientation, XmHORIZONTAL,
      XmNpacking, XmPACK_TIGHT,
      XmNradioAlwaysOne, True,
      XmNradioBehavior, True,
      NULL);

    label =
      XtVaCreateManagedWidget("targa_overlay_radio_box_label",
      xmLabelWidgetClass, rowcol,
      XmNlabelString, (XtArgVal)XmStringCreateLocalized("Overlay Targa File?"),
      NULL);

    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(rowcol,
      "targa_overlay_radio_box", args, n);

    XtManageChild(radio_box);

    three_d_opts.targa_overlay_yes_toggle =
      XtVaCreateManagedWidget("targa_overlay_yes_toggle",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(targa_overlay_radio_box_items[0]),
        XmNset, False,
        NULL);

    three_d_opts.targa_overlay_no_toggle =
      XtVaCreateManagedWidget("targa_overlay_no_toggle",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(targa_overlay_radio_box_items[1]),
        XmNset, True,
        NULL);

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
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (light_src_params_dialog, "light_src_dialog_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      light_src_dialog_help_btn_cb, NULL );

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
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (light_src_params_dialog, "light_src_dialog_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      light_src_dialog_reset_btn_cb, NULL);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (light_src_params_dialog, "light_src_dialog_dflt_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      light_src_dialog_dflt_btn_cb, NULL);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (light_src_params_dialog, "light_src_dialog_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      light_src_dialog_cancel_btn_cb, NULL);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (light_src_params_dialog, "light_src_dialog_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      light_src_dialog_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(light_src_params_dialog,
      XmNdefaultButton,
      button,
      NULL);

    XtManageChild(light_src_params_dialog);
    }

  three_d_opts.current.xlight = XLIGHT;
  three_d_opts.current.ylight = YLIGHT;
  three_d_opts.current.zlight = ZLIGHT;
  three_d_opts.current.lightavg = LIGHTAVG;
  three_d_opts.current.ambient = Ambient;
  three_d_opts.current.haze = haze;
  three_d_opts.current.back_color[0] = (int)back_color[0];
  three_d_opts.current.back_color[1] = (int)back_color[1];
  three_d_opts.current.back_color[2] = (int)back_color[2];
  three_d_opts.current.targa_overlay = Targa_Overlay;

  XmScaleSetValue(three_d_opts.xlight_scale,
    three_d_opts.current.xlight);

  XmScaleSetValue(three_d_opts.ylight_scale,
    three_d_opts.current.ylight);

  XmScaleSetValue(three_d_opts.zlight_scale,
    three_d_opts.current.zlight);

  XmScaleSetValue(three_d_opts.lightavg_scale,
    three_d_opts.current.lightavg);

  XmScaleSetValue(three_d_opts.ambient_scale,
    three_d_opts.current.ambient);

  XmScaleSetValue(three_d_opts.haze_scale,
    three_d_opts.current.haze);

  XmScaleSetValue(three_d_opts.bg_red_scale,
    three_d_opts.current.back_color[0]);

  XmScaleSetValue(three_d_opts.bg_green_scale,
    three_d_opts.current.back_color[1]);

  XmScaleSetValue(three_d_opts.bg_blue_scale,
    three_d_opts.current.back_color[2]);

  if(Targa_Overlay)
    {
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_yes_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_yes_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_no_toggle, True, False);
    }

  XtManageChild(light_src_params_dialog);
  XtManageChild(light_src_params_dialog_shell);
  XtPopup(light_src_params_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void light_src_dialog_dflt_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.xlight = three_d_opts.dflt.xlight;
  three_d_opts.current.ylight = three_d_opts.dflt.ylight;
  three_d_opts.current.zlight = three_d_opts.dflt.zlight;
  three_d_opts.current.lightavg = three_d_opts.dflt.lightavg;
  three_d_opts.current.ambient = three_d_opts.dflt.ambient;
  three_d_opts.current.haze = three_d_opts.dflt.haze;
  three_d_opts.current.back_color[0] = three_d_opts.dflt.back_color[0];
  three_d_opts.current.back_color[1] = three_d_opts.dflt.back_color[1];
  three_d_opts.current.back_color[2] = three_d_opts.dflt.back_color[2];
  three_d_opts.current.targa_overlay = three_d_opts.dflt.targa_overlay;

  XmScaleSetValue(three_d_opts.xlight_scale,
    three_d_opts.current.xlight);

  XmScaleSetValue(three_d_opts.ylight_scale,
    three_d_opts.current.ylight);

  XmScaleSetValue(three_d_opts.zlight_scale,
    three_d_opts.current.zlight);

  XmScaleSetValue(three_d_opts.lightavg_scale,
    three_d_opts.current.lightavg);

  XmScaleSetValue(three_d_opts.ambient_scale,
    three_d_opts.current.ambient);

  XmScaleSetValue(three_d_opts.haze_scale,
    three_d_opts.current.haze);

  XmScaleSetValue(three_d_opts.bg_red_scale,
    three_d_opts.current.back_color[0]);

  XmScaleSetValue(three_d_opts.bg_green_scale,
    three_d_opts.current.back_color[1]);

  XmScaleSetValue(three_d_opts.bg_blue_scale,
    three_d_opts.current.back_color[2]);

  if(Targa_Overlay)
    {
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_yes_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_yes_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_no_toggle, True, False);
    }
}

/*ARGSUSED*/
static void light_src_dialog_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{

  three_d_opts.current.xlight = XLIGHT;
  three_d_opts.current.ylight = YLIGHT;
  three_d_opts.current.zlight = ZLIGHT;
  three_d_opts.current.lightavg = LIGHTAVG;
  three_d_opts.current.ambient = Ambient;
  three_d_opts.current.haze = haze;
  three_d_opts.current.back_color[0] = (int)back_color[0];
  three_d_opts.current.back_color[1] = (int)back_color[1];
  three_d_opts.current.back_color[2] = (int)back_color[2];
  three_d_opts.current.targa_overlay = Targa_Overlay;

  XmScaleSetValue(three_d_opts.xlight_scale,
    three_d_opts.current.xlight);

  XmScaleSetValue(three_d_opts.ylight_scale,
    three_d_opts.current.ylight);

  XmScaleSetValue(three_d_opts.zlight_scale,
    three_d_opts.current.zlight);

  XmScaleSetValue(three_d_opts.lightavg_scale,
    three_d_opts.current.lightavg);

  XmScaleSetValue(three_d_opts.ambient_scale,
    three_d_opts.current.ambient);

  XmScaleSetValue(three_d_opts.haze_scale,
    three_d_opts.current.haze);

  XmScaleSetValue(three_d_opts.bg_red_scale,
    three_d_opts.current.back_color[0]);

  XmScaleSetValue(three_d_opts.bg_green_scale,
    three_d_opts.current.back_color[1]);

  XmScaleSetValue(three_d_opts.bg_blue_scale,
    three_d_opts.current.back_color[2]);

  if(Targa_Overlay)
    {
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_yes_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_yes_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.targa_overlay_no_toggle, True, False);
    }
}

/*ARGSUSED*/
static void light_src_dialog_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleGetValue(three_d_opts.xlight_scale,
    &(three_d_opts.current.xlight));

  XmScaleGetValue(three_d_opts.ylight_scale,
    &(three_d_opts.current.ylight));

  XmScaleGetValue(three_d_opts.zlight_scale,
    &(three_d_opts.current.zlight));

  XmScaleGetValue(three_d_opts.lightavg_scale,
    &(three_d_opts.current.lightavg));

  XmScaleGetValue(three_d_opts.ambient_scale,
    &(three_d_opts.current.ambient));

  XmScaleGetValue(three_d_opts.haze_scale,
    &(three_d_opts.current.haze));

  XmScaleGetValue(three_d_opts.bg_red_scale,
    &(three_d_opts.current.back_color[0]));

  XmScaleGetValue(three_d_opts.bg_green_scale,
    &(three_d_opts.current.back_color[1]));

  XmScaleGetValue(three_d_opts.bg_blue_scale,
    &(three_d_opts.current.back_color[2]));

  if(XmToggleButtonGetState(three_d_opts.targa_overlay_yes_toggle) == True)
    three_d_opts.current.targa_overlay = 1;
  else
    three_d_opts.current.targa_overlay = 0;

  XLIGHT = three_d_opts.current.xlight;
  YLIGHT = three_d_opts.current.ylight;
  ZLIGHT = three_d_opts.current.zlight;
  LIGHTAVG = three_d_opts.current.lightavg;
  Ambient = three_d_opts.current.ambient;
  haze = three_d_opts.current.haze;
  back_color[0] = three_d_opts.current.back_color[0];
  back_color[1] = three_d_opts.current.back_color[1];
  back_color[2] = three_d_opts.current.back_color[2];
  Targa_Overlay = three_d_opts.current.targa_overlay;

  XtPopdown(light_src_params_dialog_shell);

  if(Targa_Overlay)
    check_tgafile(main_window, NULL, NULL);
  else
    display_file();
}

/*ARGSUSED*/
static void light_src_dialog_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELP3DLIGHT;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void light_src_dialog_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  initvars_fractal();
  XtPopdown(light_src_params_dialog_shell);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: light_params.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.7  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.6  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.5  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
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
