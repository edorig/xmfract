
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: three_d_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void three_d_options_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_options_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_options_deflts_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_options_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget three_d_options_dialog_shell = NULL;
static Widget three_d_options_dialog = NULL;

/*ARGSUSED*/
void create_three_d_options_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Widget label;
  Widget frame;
  Widget radio_box;
  Widget button;
  XmString str;
  char tempstr[64];
  Arg args[MAX_ARGS];
  int btnbase;
  int fracbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 4 - 1;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  three_d_opts.current.xrot = XROT;
  three_d_opts.current.yrot = YROT;
  three_d_opts.current.zrot = ZROT;
  three_d_opts.current.zviewer = ZVIEWER;
  three_d_opts.current.yshift = YSHIFT;
  three_d_opts.current.xshift = XSHIFT;
  three_d_opts.current.glassestype = glassestype;

  three_d_options_dialog_shell = XmCreateDialogShell
    (main_window, "Three-D Options", NULL, 0);

  XtAddCallback(three_d_options_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(three_d_options_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  str = XmStringCreateLocalized("Three Dimensional Options");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNdialogTitle, str); n++;
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;

  three_d_options_dialog = XmCreateForm
    (three_d_options_dialog_shell, "three_d_options_dialog", args, n);

  XtAddCallback(three_d_options_dialog, XmNmapCallback,
    map_dialog, NULL);

  XmStringFree(str);

  /*
   * X-axis rotation in degrees
   *
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("X-axis rotation in degrees)");
  XtSetArg(args[n], XmNmaximum, 359); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, three_d_opts.current.xrot); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;

  three_d_opts.parms_x_rot_scale = XmCreateScale
    (three_d_options_dialog, "x_axis_rotation_scale", args, n);

  XtManageChild(three_d_opts.parms_x_rot_scale);

  XmStringFree(str);

  /*
   * Y-axis rotation in degrees
   *
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Y-axis rotation in degrees");
  XtSetArg(args[n], XmNmaximum, 359); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, three_d_opts.current.yrot); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  three_d_opts.parms_y_rot_scale = XmCreateScale
    (three_d_options_dialog, "y_axis_rotation_scale", args, n);

  XtManageChild(three_d_opts.parms_y_rot_scale);

  XmStringFree(str);

  /*
   * Z-axis rotation in degrees
   *
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Z-axis rotation in degrees");
  XtSetArg(args[n], XmNmaximum, 359); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, three_d_opts.current.zrot); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, three_d_opts.parms_x_rot_scale); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;

  three_d_opts.parms_z_rot_scale = XmCreateScale
    (three_d_options_dialog, "z_axis_rotation_scale", args, n);

  XtManageChild(three_d_opts.parms_z_rot_scale);

  XmStringFree(str);

  /*
   * Perspective distance [1 - 999, 0 for no persp]
   *
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Perspective distance [1-999, 0 for none]");
  XtSetArg(args[n], XmNmaximum, 999); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, three_d_opts.current.zviewer); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, three_d_opts.parms_y_rot_scale); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  three_d_opts.parms_perspective_scale = XmCreateScale
    (three_d_options_dialog, "perspective_scale", args, n);

  XtManageChild(three_d_opts.parms_perspective_scale);

  XmStringFree(str);

  /*
   * X shift with perspective (positive = right)
   *
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized(
   "X shift with perspective (positive = right)");
  XtSetArg(args[n], XmNmaximum, 100); n++;
  XtSetArg(args[n], XmNminimum, -100); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, three_d_opts.current.xshift); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, three_d_opts.parms_z_rot_scale); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;

  three_d_opts.parms_x_shift_scale = XmCreateScale
    (three_d_options_dialog, "x_shift_perspective_scale", args, n);

  XtManageChild(three_d_opts.parms_x_shift_scale);

  XmStringFree(str);

  /*
   * Y shift with perspective (positive = up   )
   *
   * (scale)
   */

  n = 0;
  str = XmStringCreateLocalized(
    "Y shift with perspective (positive = up   )");
  XtSetArg(args[n], XmNmaximum, 100); n++;
  XtSetArg(args[n], XmNminimum, -100); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, three_d_opts.current.yshift); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, three_d_opts.parms_perspective_scale); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  three_d_opts.parms_y_shift_scale = XmCreateScale
    (three_d_options_dialog, "y_shift_perspective_scale", args, n);

  XtManageChild(three_d_opts.parms_y_shift_scale);

  XmStringFree(str);

  /*
   * Stereo (Red/Blue 3D)?
   * (radio button)
   *
   * No
   * Alternate
   * Superimpose
   * Photo
   *
   */

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, three_d_opts.parms_y_shift_scale); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  frame = XmCreateFrame
    (three_d_options_dialog, "stereo_radio_box_frame", args, n);

  n = 0;
  sprintf((char *) &tempstr, "Stereo (Red/Blue 3D)?");
  str = XmStringCreateLocalized((char *) &tempstr);
  XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
  XtSetArg(args[n], XmNchildVerticalAlignment,
    XmALIGNMENT_WIDGET_BOTTOM); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  label = XmCreateLabel(frame, "stereo_box_label", args, n);

  XtManageChild(label);
  XmStringFree(str);

  n = 0;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNradioBehavior, True); n++;
  XtSetArg(args[n], XmNisHomogeneous, False); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;

  radio_box = XmCreateRadioBox
    (frame, "stereo_radio_box", args, n);

  n = 0;
  str = XmStringCreateLocalized("No");
  if(three_d_opts.current.glassestype == 0)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  three_d_opts.parms_stereo_no_toggle = XmCreateToggleButton
    (radio_box, "no_stereo_toggle", args, n);

  XtManageChild(three_d_opts.parms_stereo_no_toggle);

  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Alternate");
  if(three_d_opts.current.glassestype == 1)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  three_d_opts.parms_stereo_alt_toggle = XmCreateToggleButton
    (radio_box, "alternate_stereo_toggle", args, n);

  XtManageChild(three_d_opts.parms_stereo_alt_toggle);

  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Superimpose");
  if(three_d_opts.current.glassestype == 2)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  three_d_opts.parms_stereo_super_toggle = XmCreateToggleButton
    (radio_box, "superimpose_stereo_toggle", args, n);

  XtManageChild(three_d_opts.parms_stereo_super_toggle);

  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Photo");
  if(three_d_opts.current.glassestype == 3)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  three_d_opts.parms_stereo_photo_toggle = XmCreateToggleButton
    (radio_box, "photo_stereo_toggle", args, n);

  XtManageChild(three_d_opts.parms_stereo_photo_toggle);

  XmStringFree(str);

  XtManageChild(radio_box);
  XtManageChild(frame);

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
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_options_dialog, "three_d_dialog_help_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_options_help_btn_cb, NULL );

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
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_options_dialog, "three_d_dialog_reset_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_options_reset_btn_cb, NULL);

  XmStringFree(str);
  XtManageChild(button);

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
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_options_dialog, "three_d_dialog_defaults_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_options_deflts_btn_cb, NULL);

  XmStringFree(str);
  XtManageChild(button);

  /*
   * Done
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Done");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_options_dialog, "three_d_dialog_done_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_options_done_btn_cb, NULL);

  XmStringFree(str);
  XtManageChild(button);

  XtVaSetValues(three_d_options_dialog,
    XmNdefaultButton,
    button,
    NULL);

  XtManageChild(three_d_options_dialog);
  XtManageChild(three_d_options_dialog_shell);
  XtPopup(three_d_options_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void three_d_options_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.xrot = XROT;
  three_d_opts.current.yrot = YROT;
  three_d_opts.current.zrot = ZROT;
  three_d_opts.current.zviewer = ZVIEWER;
  three_d_opts.current.xshift = XSHIFT;
  three_d_opts.current.yshift = YSHIFT;
  three_d_opts.current.glassestype = glassestype;

  XmScaleSetValue(three_d_opts.parms_x_rot_scale,
    three_d_opts.current.xrot);

  XmScaleSetValue(three_d_opts.parms_y_rot_scale,
    three_d_opts.current.yrot);

  XmScaleSetValue(three_d_opts.parms_z_rot_scale,
    three_d_opts.current.zrot);

  XmScaleSetValue(three_d_opts.parms_perspective_scale,
    three_d_opts.current.zviewer);

  XmScaleSetValue(three_d_opts.parms_x_shift_scale,
    three_d_opts.current.xshift);

  XmScaleSetValue(three_d_opts.parms_y_shift_scale,
    three_d_opts.current.yshift);

  if(three_d_opts.current.glassestype == 0)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 1)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 3)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 2)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, True, False);
    }
}

/*ARGSUSED*/
static void three_d_options_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleGetValue(three_d_opts.parms_x_rot_scale,
    &(three_d_opts.current.xrot));

  XmScaleGetValue(three_d_opts.parms_y_rot_scale,
    &(three_d_opts.current.yrot));

  XmScaleGetValue(three_d_opts.parms_z_rot_scale,
    &(three_d_opts.current.zrot));

  XmScaleGetValue(three_d_opts.parms_perspective_scale,
    &(three_d_opts.current.zviewer));

  XmScaleGetValue(three_d_opts.parms_x_shift_scale,
    &(three_d_opts.current.xshift));

  XmScaleGetValue(three_d_opts.parms_y_shift_scale,
    &(three_d_opts.current.yshift));

  if(XmToggleButtonGetState
    (three_d_opts.parms_stereo_no_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 0;
  else if(XmToggleButtonGetState
    (three_d_opts.parms_stereo_alt_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 1;
  else if(XmToggleButtonGetState
    (three_d_opts.parms_stereo_photo_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 3;
  else if(XmToggleButtonGetState
    (three_d_opts.parms_stereo_super_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 2;

  XROT = three_d_opts.current.xrot;
  YROT = three_d_opts.current.yrot;
  ZROT = three_d_opts.current.zrot;
  ZVIEWER = three_d_opts.current.zviewer;
  YSHIFT = three_d_opts.current.yshift;
  XSHIFT = three_d_opts.current.xshift;
  glassestype = three_d_opts.current.glassestype;

  XtUnmanageChild(three_d_options_dialog_shell);
  XtDestroyWidget(three_d_options_dialog_shell);
  three_d_options_dialog_shell = NULL;
}

/*ARGSUSED*/
static void three_d_options_deflts_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.xrot = three_d_opts.dflt.xrot;
  three_d_opts.current.yrot = three_d_opts.dflt.yrot;
  three_d_opts.current.zrot = three_d_opts.dflt.zrot;
  three_d_opts.current.zviewer = three_d_opts.dflt.zviewer;
  three_d_opts.current.xshift = three_d_opts.dflt.xshift;
  three_d_opts.current.yshift = three_d_opts.dflt.yshift;
  three_d_opts.current.glassestype = three_d_opts.dflt.glassestype;

  XmScaleSetValue(three_d_opts.parms_x_rot_scale,
    three_d_opts.current.xrot);

  XmScaleSetValue(three_d_opts.parms_y_rot_scale,
    three_d_opts.current.yrot);

  XmScaleSetValue(three_d_opts.parms_z_rot_scale,
    three_d_opts.current.zrot);

  XmScaleSetValue(three_d_opts.parms_perspective_scale,
    three_d_opts.current.zviewer);

  XmScaleSetValue(three_d_opts.parms_x_shift_scale,
    three_d_opts.current.xshift);

  XmScaleSetValue(three_d_opts.parms_y_shift_scale,
    three_d_opts.current.yshift);

  if(three_d_opts.current.glassestype == 0)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 1)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 3)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 2)
    {
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_no_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_alt_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_photo_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.parms_stereo_super_toggle, True, False);
    }
}

/*ARGSUSED*/
static void three_d_options_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELP3DPARMS;
  read_help(main_window, NULL, NULL);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: three_d_menu.c,v $
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
