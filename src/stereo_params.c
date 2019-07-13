
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: stereo_params.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void funny_glasses_dialog_dflt_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void funny_glasses_dialog_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void funny_glasses_dialog_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void funny_glasses_dialog_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void funny_glasses_dialog_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget funny_glasses_params_dialog_shell = NULL;
static Widget funny_glasses_params_dialog = NULL;

/*ARGSUSED*/
void get_funny_glasses_params(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!funny_glasses_params_dialog_shell)
    {
    Widget button;
    XmString str;
    Arg args[MAX_ARGS];
    char tempstr[64];
    int fracbase;
    int btnbase;
    int n;

    btnbase = 20;
    fracbase = btnbase * 5 - 1;

    funny_glasses_params_dialog_shell = XmCreateDialogShell
      (main_window, "Select Funny Glasses Parameters", NULL, 0);

    XtAddCallback(funny_glasses_params_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(funny_glasses_params_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Select Funny Glasses Parameters");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    funny_glasses_params_dialog = XmCreateForm
      (funny_glasses_params_dialog_shell,
      "funny_glasses_params_dialog", args, n);

    XtAddCallback(funny_glasses_params_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    /*
     * Interocular distance (% of screen)
     * (scale)
     */

    sprintf((char *) &tempstr, "Interocular distance (%% of screen)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.interocular_distance_scale =
      XtVaCreateManagedWidget("interocular_distance_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.eyeseparation,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Convergence adjust (positive = spread greater)
     * (scale)
     */

    sprintf((char *) &tempstr,
      "Convergence adjust (positive = spread greater)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.convergence_adjust_scale =
      XtVaCreateManagedWidget("convergence_adjust_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.xadjust,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, three_d_opts.interocular_distance_scale,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Left red image crop (% of screen)
     * (scale)
     */

    sprintf((char *) &tempstr,
      "Left red image crop (%% of screen)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.left_red_crop_scale =
      XtVaCreateManagedWidget("left_red_crop_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.red_crop_left,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, three_d_opts.convergence_adjust_scale,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Right red image crop (% of screen)
     * (scale)
     */

    sprintf((char *) &tempstr,
      "Right red image crop (%% of screen)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.right_red_crop_scale =
      XtVaCreateManagedWidget("right_red_crop_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.red_crop_right,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, three_d_opts.left_red_crop_scale,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Left blue image crop (% of screen)
     * (scale)
     */

    sprintf((char *) &tempstr,
      "Left blue image crop (%% of screen)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.left_blue_crop_scale =
      XtVaCreateManagedWidget("left_blue_crop_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.blue_crop_left,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, three_d_opts.right_red_crop_scale,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Right blue image crop (% of screen)
     * (scale)
     */

    sprintf((char *) &tempstr,
      "Right blue image crop (%% of screen)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.right_blue_crop_scale =
      XtVaCreateManagedWidget("right_blue_crop_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.blue_crop_right,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, three_d_opts.left_blue_crop_scale,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Red brightness factor (%)
     * (scale)
     */

    sprintf((char *) &tempstr,
      "Red brightness factor (%%)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.red_brightness_scale =
      XtVaCreateManagedWidget("red_brightness_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.red_bright,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, three_d_opts.right_blue_crop_scale,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Blue brightness factor (%)
     * (scale)
     */

    sprintf((char *) &tempstr,
      "Blue brightness factor (%%)");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.blue_brightness_scale =
      XtVaCreateManagedWidget("blue_brightness_scale",
        xmScaleWidgetClass,
        funny_glasses_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, three_d_opts.current.blue_bright,
        XmNmaximum, 100,
        XmNminimum, 0,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, three_d_opts.red_brightness_scale,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

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
    XtSetArg(args[n], XmNtopWidget,
      three_d_opts.blue_brightness_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (funny_glasses_params_dialog, "funny_glasses_dialog_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      funny_glasses_dialog_help_btn_cb, NULL );

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
    XtSetArg(args[n], XmNtopWidget,
      three_d_opts.blue_brightness_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (funny_glasses_params_dialog, "funny_glasses_dialog_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      funny_glasses_dialog_reset_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopWidget,
      three_d_opts.blue_brightness_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (funny_glasses_params_dialog, "funny_glasses_dialog_dflt_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      funny_glasses_dialog_dflt_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopWidget,
      three_d_opts.blue_brightness_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (funny_glasses_params_dialog,
        "funny_glasses_dialog_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      funny_glasses_dialog_cancel_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopWidget,
      three_d_opts.blue_brightness_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    button = XmCreatePushButton(funny_glasses_params_dialog,
      "funny_glasses_dialog_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      funny_glasses_dialog_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(funny_glasses_params_dialog,
      XmNdefaultButton,
      button,
      NULL);

    XtManageChild(funny_glasses_params_dialog);
    XtManageChild(funny_glasses_params_dialog_shell);
    }

  if(ZVIEWER == 0)
    ZVIEWER = 150;

  if(eyeseparation == 0)
    {
    if(fractype==IFS3D || fractype==LLORENZ3D || fractype==FPLORENZ3D)
      {
      eyeseparation = 2;
      xadjust = -2;
      }
    else
      {
      eyeseparation = 3;
      xadjust = 0;
      }
    }

  if(eyeseparation > 100)
    eyeseparation = 100;
  else if (eyeseparation < 0)
    eyeseparation = 0;

  if(xadjust > 100)
    xadjust = 100;
  else if (xadjust < 0)
    xadjust = 0;

  if(red_crop_left > 100)
    red_crop_left = 100;
  else if (red_crop_left < 0)
    red_crop_left = 0;

  if(red_crop_right > 100)
    red_crop_right = 100;
  else if (red_crop_right < 0)
    red_crop_right = 0;

  if(blue_crop_left > 100)
    blue_crop_left = 100;
  else if (blue_crop_left < 0)
    blue_crop_left = 0;

  if(blue_crop_right > 100)
    blue_crop_right = 100;
  else if (blue_crop_right < 0)
    blue_crop_right = 0;

  if(red_bright > 100)
    red_bright = 100;
  else if (red_bright < 0)
    red_bright = 0;

  if(blue_bright > 100)
    blue_bright = 100;
  else if (blue_bright < 0)
    blue_bright = 0;

  three_d_opts.current.eyeseparation = eyeseparation;
  three_d_opts.current.xadjust = xadjust;
  three_d_opts.current.red_crop_left = red_crop_left;
  three_d_opts.current.red_crop_right = red_crop_right;
  three_d_opts.current.blue_crop_left = blue_crop_left;
  three_d_opts.current.blue_crop_right = blue_crop_right;
  three_d_opts.current.red_bright = red_bright;
  three_d_opts.current.blue_bright = blue_bright;

  XmScaleSetValue(three_d_opts.interocular_distance_scale,
    three_d_opts.current.eyeseparation);

  XmScaleSetValue(three_d_opts.convergence_adjust_scale,
    three_d_opts.current.xadjust);

  XmScaleSetValue(three_d_opts.left_red_crop_scale,
    three_d_opts.current.red_crop_left);

  XmScaleSetValue(three_d_opts.right_red_crop_scale,
    three_d_opts.current.red_crop_right);

  XmScaleSetValue(three_d_opts.left_blue_crop_scale,
    three_d_opts.current.blue_crop_left);

  XmScaleSetValue(three_d_opts.right_blue_crop_scale,
    three_d_opts.current.blue_crop_right);

  XmScaleSetValue(three_d_opts.red_brightness_scale,
    three_d_opts.current.red_bright);

  XmScaleSetValue(three_d_opts.blue_brightness_scale,
    three_d_opts.current.blue_bright);

  XtManageChild(funny_glasses_params_dialog);
  XtManageChild(funny_glasses_params_dialog_shell);
  XtPopup(funny_glasses_params_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void funny_glasses_dialog_dflt_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.eyeseparation = three_d_opts.dflt.eyeseparation;
  three_d_opts.current.xadjust = three_d_opts.dflt.xadjust;
  three_d_opts.current.red_crop_left = three_d_opts.dflt.red_crop_left;
  three_d_opts.current.red_crop_right = three_d_opts.dflt.red_crop_right;
  three_d_opts.current.blue_crop_left = three_d_opts.dflt.blue_crop_left;
  three_d_opts.current.blue_crop_right = three_d_opts.dflt.blue_crop_right;
  three_d_opts.current.red_bright = three_d_opts.dflt.red_bright;
  three_d_opts.current.blue_bright = three_d_opts.dflt.blue_bright;

  XmScaleSetValue(three_d_opts.interocular_distance_scale,
    three_d_opts.current.eyeseparation);

  XmScaleSetValue(three_d_opts.convergence_adjust_scale,
    three_d_opts.current.xadjust);

  XmScaleSetValue(three_d_opts.left_red_crop_scale,
    three_d_opts.current.red_crop_left);

  XmScaleSetValue(three_d_opts.right_red_crop_scale,
    three_d_opts.current.red_crop_right);

  XmScaleSetValue(three_d_opts.left_blue_crop_scale,
    three_d_opts.current.blue_crop_left);

  XmScaleSetValue(three_d_opts.right_blue_crop_scale,
    three_d_opts.current.blue_crop_right);

  XmScaleSetValue(three_d_opts.red_brightness_scale,
    three_d_opts.current.red_bright);

  XmScaleSetValue(three_d_opts.blue_brightness_scale,
    three_d_opts.current.blue_bright);
}

/*ARGSUSED*/
static void funny_glasses_dialog_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.eyeseparation = eyeseparation;
  three_d_opts.current.xadjust = xadjust;
  three_d_opts.current.red_crop_left = red_crop_left;
  three_d_opts.current.red_crop_right = red_crop_right;
  three_d_opts.current.blue_crop_left = blue_crop_left;
  three_d_opts.current.blue_crop_right = blue_crop_right;
  three_d_opts.current.red_bright = red_bright;
  three_d_opts.current.blue_bright = blue_bright;

  XmScaleSetValue(three_d_opts.interocular_distance_scale,
    three_d_opts.current.eyeseparation);

  XmScaleSetValue(three_d_opts.convergence_adjust_scale,
    three_d_opts.current.xadjust);

  XmScaleSetValue(three_d_opts.left_red_crop_scale,
    three_d_opts.current.red_crop_left);

  XmScaleSetValue(three_d_opts.right_red_crop_scale,
    three_d_opts.current.red_crop_right);

  XmScaleSetValue(three_d_opts.left_blue_crop_scale,
    three_d_opts.current.blue_crop_left);

  XmScaleSetValue(three_d_opts.right_blue_crop_scale,
    three_d_opts.current.blue_crop_right);

  XmScaleSetValue(three_d_opts.red_brightness_scale,
    three_d_opts.current.red_bright);

  XmScaleSetValue(three_d_opts.blue_brightness_scale,
    three_d_opts.current.blue_bright);
}

/*ARGSUSED*/
static void funny_glasses_dialog_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleGetValue(three_d_opts.interocular_distance_scale,
    &(three_d_opts.current.eyeseparation));

  XmScaleGetValue(three_d_opts.convergence_adjust_scale,
    &(three_d_opts.current.xadjust));

  XmScaleGetValue(three_d_opts.left_red_crop_scale,
    &(three_d_opts.current.red_crop_left));

  XmScaleGetValue(three_d_opts.right_red_crop_scale,
    &(three_d_opts.current.red_crop_right));

  XmScaleGetValue(three_d_opts.left_blue_crop_scale,
    &(three_d_opts.current.blue_crop_left));

  XmScaleGetValue(three_d_opts.right_blue_crop_scale,
    &(three_d_opts.current.blue_crop_right));

  XmScaleGetValue(three_d_opts.red_brightness_scale,
    &(three_d_opts.current.red_bright));

  XmScaleGetValue(three_d_opts.blue_brightness_scale,
    &(three_d_opts.current.blue_bright));

  eyeseparation = three_d_opts.current.eyeseparation;
  xadjust = three_d_opts.current.xadjust;
  red_crop_left = three_d_opts.current.red_crop_left;
  red_crop_right = three_d_opts.current.red_crop_right;
  blue_crop_left = three_d_opts.current.blue_crop_left;
  blue_crop_right = three_d_opts.current.blue_crop_right;
  red_bright = three_d_opts.current.red_bright;
  blue_bright = three_d_opts.current.blue_bright;

  XtPopdown(funny_glasses_params_dialog_shell);

  check_mapfile(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void funny_glasses_dialog_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELP3DGLASSES;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void funny_glasses_dialog_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  initvars_fractal();
  XtPopdown(funny_glasses_params_dialog_shell);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: stereo_params.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.8  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.7  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.6  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
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
