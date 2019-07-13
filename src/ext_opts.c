
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: ext_opts.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void extended_dialog_defaults_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void extended_dialog_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void extended_dialog_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void extended_dialog_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void extended_dialog_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget extended_options_dialog_shell = NULL;
static Widget extended_options_dialog = NULL;

/*ARGSUSED*/
void create_extended_options_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[256];

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!extended_options_dialog_shell)
    {
    Widget textfield;
    Widget scale;
    Widget frame;
    Widget which;
    Widget form;
    Widget label;
    Widget button;
    XmString str;
    Arg args[MAX_ARGS];
    int fracbase;
    int btnbase;
    int n;

    btnbase = 20;
    fracbase = btnbase * 6 - 1;

    extended_options_dialog_shell = XmCreateDialogShell
      (main_window, "Extended Options", NULL, 0);

    XtAddCallback(extended_options_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(extended_options_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Extended Options");
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    extended_options_dialog = XmCreateForm
      (extended_options_dialog_shell, "extended_options_dialog", args, n);

    XtAddCallback(extended_options_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

    frame = XmCreateFrame
      (extended_options_dialog, "extended_options_scale_frame", args, n);

    which = frame;

    n = 0;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    form = XmCreateForm(frame, "extended_options_scale_form", args, n);

    /*
     * Look for finite attractor (0=no, 1=yes, -1=phase)
     *
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized(
      "Look for finite attractor (0=no, 1=yes, -1=phase)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNmaximum, 1); n++;
    XtSetArg(args[n], XmNminimum, -1); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "finite_attractor_scale", args, n);

    extended_opts.finite_attractor_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Potential Max Color (0 means off)
     *
     * (scale)
     *
     */

    str = XmStringCreateLocalized("Potential max color (0 means off)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNmaximum, 32767); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "potential_max_color_scale", args, n);

    extended_opts.potential_max_color_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Modulus
     *
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Modulus (bailout)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNmaximum, 32767); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "bailout_scale", args, n);

    extended_opts.bailout_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Distance Estimator (0=off, <0=edge, >0=on)
     *
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized(
      "Distance Estimator (0=off, <0=edge, >0=on)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNmaximum, 32767); n++;
    XtSetArg(args[n], XmNminimum, -32767); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "distance_estimator_scale", args, n);

    extended_opts.distance_estimator_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Width factor
     *
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Width Factor");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 32767); n++;
    XtSetArg(args[n], XmNminimum, -32767); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "width_factor_scale", args, n);

    extended_opts.width_factor_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Color cycling from color
     *
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Color cycling from color");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, colors - 2); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "color_cycling_from_scale", args, n);

    extended_opts.color_cycling_from_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * Color cycling to color
     *
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Color cycling to color");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, colors - 1); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "color_cycling_to_scale", args, n);

    extended_opts.color_cycling_to_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    /*
     * 16 bit values (0 means no)
     *
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("16 bit values (0 means no)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopWidget, scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNmaximum, 1); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "sixteen_bit_values_scale", args, n);

    extended_opts.sixteen_bit_values_scale = scale;

    XtManageChild(scale);

    XmStringFree(str);

    XtManageChild(form);
    XtManageChild(frame);

    /*
     * Slope
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "slope_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Slope");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "slope_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "slope_textfield", args, n);

    extended_opts.slope_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * Inversion radius or "auto" (0 means off)
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "inversion_radius_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized
      ("Inversion radius or 'auto' (0 means off)");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "inversion_radius_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "inversion_radius_textfield", args, n);

    extended_opts.inversion_radius_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * center X coordinate or "auto"
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "center_x_coord_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized
      ("Center X coordinate or 'auto')");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "center_x_coord_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "center_x_coord_textfield", args, n);

    extended_opts.center_x_coord_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * center Y coordinate or "auto"
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "center_y_coord_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized
      ("Center Y coordinate or 'auto')");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "center_y_coord_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "center_y_coord_textfield", args, n);

    extended_opts.center_y_coord_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * Periodicity checking (Yes|No|Show|nnn)
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, which); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "periodicity_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized
      ("Periodicity checking (Yes|No|Show|nnn)");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "periodicity_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "periodicity_textfield", args, n);

    extended_opts.periodicity_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * Symmetry (None|Xaxis|Yaxis|XYaxis|Origin|Pi)
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "symmetry_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized
      ("Symmetry (None|Xaxis|Yaxis|XYaxis|Origin|Pi)");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "symmetry_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "symmetry_textfield", args, n);

    extended_opts.symmetry_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * Random seed value (nnn)
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "rseed_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Random seed value (nnn)");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "rseed_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "rseed_textfield", args, n);

    extended_opts.rseed_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * Show dot color (Off|nnn)
     *
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (extended_options_dialog, "showdot_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Show dot color (Off|nnn)");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "showdot_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    textfield = XmCreateTextField
      (frame, "showdot_textfield", args, n);

    extended_opts.showdot_textfield = textfield;

    XtManageChild(textfield);
    XtManageChild(frame);

    /*
     * Help
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (extended_options_dialog, "extended_dialog_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      extended_dialog_help_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Apply
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Apply");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (extended_options_dialog, "extended_dialog_apply_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      extended_dialog_done_btn_cb, (XtPointer) "0");

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Reset
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Reset");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (extended_options_dialog, "extended_dialog_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      extended_dialog_reset_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Defaults
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Defaults");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (extended_options_dialog, "extended_dialog_defaults_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      extended_dialog_defaults_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Cancel
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Cancel");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (extended_options_dialog, "extended_dialog_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      extended_dialog_cancel_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Done
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Done");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 5); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    button = XmCreatePushButton
      (extended_options_dialog, "extended_dialog_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      extended_dialog_done_btn_cb, (XtPointer) "1");

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(extended_options_dialog,
      XmNdefaultButton,
      button,
      NULL);
    }

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
  extended_opts.current.periodicitycheck = periodicitycheck;
  extended_opts.current.symmetry = symmetry;
  extended_opts.current.rseed = rseed;
  extended_opts.current.showdot = showdot;

  XtVaSetValues(extended_opts.finite_attractor_scale,
    XmNvalue, (int)extended_opts.current.finattract,
    NULL);

  XtVaSetValues(extended_opts.potential_max_color_scale,
    XmNvalue, (int)extended_opts.current.potparam0,
    NULL);

  sprintf((char *) &tempstr, "%+.17g", extended_opts.current.potparam1);

  XtVaSetValues(extended_opts.slope_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  XtVaSetValues(extended_opts.bailout_scale,
    XmNvalue, (int)extended_opts.current.potparam2,
    NULL);

  XtVaSetValues(extended_opts.distance_estimator_scale,
    XmNvalue, (int)extended_opts.current.usr_distest,
    NULL);

  XtVaSetValues(extended_opts.width_factor_scale,
    XmNvalue, (int)extended_opts.current.distestwidth,
    NULL);

  if(extended_opts.current.inversion0 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.current.inversion0);

  XtVaSetValues(extended_opts.inversion_radius_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  if(extended_opts.current.inversion1 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.current.inversion1);

  XtVaSetValues(extended_opts.center_x_coord_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  if(extended_opts.current.inversion2 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.current.inversion2);

  XtVaSetValues(extended_opts.center_y_coord_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  XtVaSetValues(extended_opts.color_cycling_from_scale,
    XmNvalue, (int)extended_opts.current.rotate_lo,
    NULL);

  XtVaSetValues(extended_opts.color_cycling_to_scale,
    XmNvalue, (int)extended_opts.current.rotate_hi,
    NULL);

  XtVaSetValues(extended_opts.sixteen_bit_values_scale,
    XmNvalue, (int)extended_opts.current.pot16bit,
    NULL);

  if(extended_opts.current.periodicitycheck == -1)
    sprintf((char *) &tempstr, "%s", "Show");
  else if(extended_opts.current.periodicitycheck == 0)
    sprintf((char *) &tempstr, "%s", "No");
  else if(extended_opts.current.periodicitycheck == 1)
    sprintf((char *) &tempstr, "%s", "Yes");
  else
    sprintf((char *) &tempstr, "%+d",
    extended_opts.current.periodicitycheck);

  XtVaSetValues(extended_opts.periodicity_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  if(extended_opts.current.symmetry == XAXIS)
    sprintf((char *) &tempstr, "%s", "Xaxis");
  else if(extended_opts.current.symmetry == YAXIS)
    sprintf((char *) &tempstr, "%s", "Yaxis");
  else if(extended_opts.current.symmetry == XYAXIS)
    sprintf((char *) &tempstr, "%s", "XYaxis");
  else if(extended_opts.current.symmetry == ORIGIN)
    sprintf((char *) &tempstr, "%s", "Origin");
  else if(extended_opts.current.symmetry == PI_SYM)
    sprintf((char *) &tempstr, "%s", "PI");
  else if(extended_opts.current.symmetry == NOSYM)
    sprintf((char *) &tempstr, "%s", "None");
  else
    sprintf((char *) &tempstr, "%s", "Unknown");

  XtVaSetValues(extended_opts.symmetry_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  sprintf((char *) &tempstr, "%+d", extended_opts.current.rseed);

  XtVaSetValues(extended_opts.rseed_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  if(extended_opts.current.showdot == -1)
    sprintf((char *) &tempstr, "%s", "Off");
  else
    sprintf((char *) &tempstr, "%+d", extended_opts.current.showdot);

  XtVaSetValues(extended_opts.showdot_textfield,
    XmNvalue, (char *) &tempstr,
    NULL);

  XtManageChild(extended_options_dialog);
  XtManageChild(extended_options_dialog_shell);
  XtPopup(extended_options_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void extended_dialog_defaults_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[256];

  XmScaleSetValue(extended_opts.finite_attractor_scale,
    (int) extended_opts.dflt.finattract);

  XmScaleSetValue(extended_opts.potential_max_color_scale,
    (int) extended_opts.dflt.potparam0);

  XmScaleSetValue(extended_opts.bailout_scale,
    (int) extended_opts.dflt.potparam2);

  XmScaleSetValue(extended_opts.distance_estimator_scale,
    (int) extended_opts.dflt.usr_distest);

  XmScaleSetValue(extended_opts.width_factor_scale,
    (int) extended_opts.dflt.distestwidth);

  XmScaleSetValue(extended_opts.color_cycling_from_scale,
    (int) extended_opts.dflt.rotate_lo);

  XmScaleSetValue(extended_opts.color_cycling_to_scale,
    (int) extended_opts.dflt.rotate_hi);

  XmScaleSetValue(extended_opts.sixteen_bit_values_scale,
    (int) extended_opts.dflt.pot16bit);

  sprintf((char *) &tempstr, "%+.17g", extended_opts.dflt.potparam1);

  XmTextFieldSetString(extended_opts.slope_textfield, (char *) &tempstr);

  if(extended_opts.dflt.inversion0 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.dflt.inversion0);

  XmTextFieldSetString(extended_opts.inversion_radius_textfield,
    (char *) &tempstr);

  if(extended_opts.dflt.inversion1 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.dflt.inversion1);

  XmTextFieldSetString(extended_opts.center_x_coord_textfield,
    (char *) &tempstr);

  if(extended_opts.dflt.inversion2 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.dflt.inversion2);

  XmTextFieldSetString(extended_opts.center_y_coord_textfield,
    (char *) &tempstr);

  if(extended_opts.dflt.periodicitycheck == -1)
    sprintf((char *) &tempstr, "%s", "Show");
  else if(extended_opts.dflt.periodicitycheck == 0)
    sprintf((char *) &tempstr, "%s", "No");
  else if(extended_opts.dflt.periodicitycheck == 1)
    sprintf((char *) &tempstr, "%s", "Yes");
  else
    sprintf((char *) &tempstr, "%+d",
    extended_opts.dflt.periodicitycheck);

  XmTextFieldSetString(extended_opts.periodicity_textfield,
    (char *) &tempstr);

  if(extended_opts.dflt.symmetry == XAXIS)
    sprintf((char *) &tempstr, "%s", "Xaxis");
  else if(extended_opts.dflt.symmetry == YAXIS)
    sprintf((char *) &tempstr, "%s", "Yaxis");
  else if(extended_opts.dflt.symmetry == XYAXIS)
    sprintf((char *) &tempstr, "%s", "XYaxis");
  else if(extended_opts.dflt.symmetry == ORIGIN)
    sprintf((char *) &tempstr, "%s", "Origin");
  else if(extended_opts.dflt.symmetry == PI_SYM)
    sprintf((char *) &tempstr, "%s", "PI");
  else if(extended_opts.dflt.symmetry == NOSYM)
    sprintf((char *) &tempstr, "%s", "None");
  else
    sprintf((char *) &tempstr, "%s", "Unknown");

  XmTextFieldSetString(extended_opts.symmetry_textfield,
    (char *) &tempstr);

  sprintf((char *) &tempstr, "%+d", extended_opts.dflt.rseed);

  XmTextFieldSetString(extended_opts.rseed_textfield,
    (char *) &tempstr);

  if(extended_opts.dflt.showdot == -1)
    sprintf((char *) &tempstr, "%s", "Off");
  else
    sprintf((char *) &tempstr, "%d", extended_opts.dflt.showdot);

  XmTextFieldSetString(extended_opts.showdot_textfield,
    (char *) &tempstr);
}

/*ARGSUSED*/
static void extended_dialog_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[256];

  XmScaleSetValue(extended_opts.finite_attractor_scale,
    (int) extended_opts.current.finattract);

  XmScaleSetValue(extended_opts.potential_max_color_scale,
    (int) extended_opts.current.potparam0);

  XmScaleSetValue(extended_opts.bailout_scale,
    (int) extended_opts.current.potparam2);

  XmScaleSetValue(extended_opts.distance_estimator_scale,
    (int) extended_opts.current.usr_distest);

  XmScaleSetValue(extended_opts.width_factor_scale,
    (int) extended_opts.current.distestwidth);

  XmScaleSetValue(extended_opts.color_cycling_from_scale,
    (int) extended_opts.current.rotate_lo);

  XmScaleSetValue(extended_opts.color_cycling_to_scale,
    (int) extended_opts.current.rotate_hi);

  XmScaleSetValue(extended_opts.sixteen_bit_values_scale,
    (int) extended_opts.current.pot16bit);

  sprintf((char *) &tempstr, "%+.17g", extended_opts.current.potparam1);

  XmTextFieldSetString(extended_opts.slope_textfield, (char *) &tempstr);

  if(extended_opts.current.inversion0 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.current.inversion0);

  XmTextFieldSetString(extended_opts.inversion_radius_textfield,
    (char *) &tempstr);

  if(extended_opts.current.inversion1 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.current.inversion1);

  XmTextFieldSetString(extended_opts.center_x_coord_textfield,
    (char *) &tempstr);

  if(extended_opts.current.inversion2 == AUTOINVERT)
    sprintf((char *) &tempstr, "%s", "auto");
  else
    sprintf((char *) &tempstr, "%+.17g", extended_opts.current.inversion2);

  XmTextFieldSetString(extended_opts.center_y_coord_textfield,
    (char *) &tempstr);

  if(extended_opts.current.periodicitycheck == -1)
    sprintf((char *) &tempstr, "%s", "Show");
  else if(extended_opts.current.periodicitycheck == 0)
    sprintf((char *) &tempstr, "%s", "No");
  else if(extended_opts.current.periodicitycheck == 1)
    sprintf((char *) &tempstr, "%s", "Yes");
  else
    sprintf((char *) &tempstr, "%+d",
    extended_opts.current.periodicitycheck);

  XmTextFieldSetString(extended_opts.periodicity_textfield,
    (char *) &tempstr);

  if(extended_opts.current.symmetry == XAXIS)
    sprintf((char *) &tempstr, "%s", "Xaxis");
  else if(extended_opts.current.symmetry == YAXIS)
    sprintf((char *) &tempstr, "%s", "Yaxis");
  else if(extended_opts.current.symmetry == XYAXIS)
    sprintf((char *) &tempstr, "%s", "XYaxis");
  else if(extended_opts.current.symmetry == ORIGIN)
    sprintf((char *) &tempstr, "%s", "Origin");
  else if(extended_opts.current.symmetry == PI_SYM)
    sprintf((char *) &tempstr, "%s", "PI");
  else if(extended_opts.current.symmetry == NOSYM)
    sprintf((char *) &tempstr, "%s", "None");
  else
    sprintf((char *) &tempstr, "%s", "Unknown");

  XmTextFieldSetString(extended_opts.symmetry_textfield,
    (char *) &tempstr);

  sprintf((char *) &tempstr, "%+d", extended_opts.current.rseed);

  XmTextFieldSetString(extended_opts.rseed_textfield,
    (char *) &tempstr);

  if(extended_opts.current.showdot == -1)
    sprintf((char *) &tempstr, "%s", "Off");
  else
    sprintf((char *) &tempstr, "%+d", extended_opts.current.showdot);

  XmTextFieldSetString(extended_opts.showdot_textfield,
    (char *) &tempstr);
}

/*ARGSUSED*/
static void extended_dialog_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *txt;
  int tmpval;
  int changed;
  char *invalid;
  double validnum;
  char msg[128];

  int pop_down = atoi((char *)client_data);

  txt = XmTextFieldGetString(extended_opts.slope_textfield);

  invalid = (char *) NULL;

  validnum = strtod(txt, &invalid);

  if( invalid && *invalid )
    {
    sprintf((char *) &msg,
      "Cannot resolve floating point number \"%s\".", txt);
    caution(main_window, (char *) &msg, NULL);
    }

  extended_opts.current.potparam1 = validnum;

  txt = XmTextFieldGetString(extended_opts.inversion_radius_textfield);

  if((strstr(txt, "auto") != (char *) NULL) ||
     (strstr(txt, "Auto") != (char *) NULL) ||
     (strstr(txt, "AUTO") != (char *) NULL))
    {
    extended_opts.current.inversion0 = AUTOINVERT;
    }
  else
    {
    invalid = (char *) NULL;

    validnum = strtod(txt, &invalid);

    if( invalid && *invalid )
      {
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }

    extended_opts.current.inversion0 = validnum;
    }

  txt = XmTextFieldGetString(extended_opts.center_x_coord_textfield);

  if((strstr(txt, "auto") != (char *) NULL) ||
     (strstr(txt, "Auto") != (char *) NULL) ||
     (strstr(txt, "AUTO") != (char *) NULL))
    {
    extended_opts.current.inversion1 = AUTOINVERT;
    }
  else
    {
    invalid = (char *) NULL;

    validnum = strtod(txt, &invalid);

    if( invalid && *invalid )
      {
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }

    extended_opts.current.inversion1 = validnum;
    }

  txt = XmTextFieldGetString(extended_opts.center_y_coord_textfield);

  if((strstr(txt, "auto") != (char *) NULL) ||
     (strstr(txt, "Auto") != (char *) NULL) ||
     (strstr(txt, "AUTO") != (char *) NULL))
    {
    extended_opts.current.inversion2 = AUTOINVERT;
    }
  else
    {
    invalid = (char *) NULL;

    validnum = strtod(txt, &invalid);

    if( invalid && *invalid )
      {
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }

    extended_opts.current.inversion2 = validnum;
    }

  XmScaleGetValue(extended_opts.finite_attractor_scale, &tmpval);
  extended_opts.current.finattract = (int) tmpval;

  XmScaleGetValue(extended_opts.potential_max_color_scale, &tmpval);
  extended_opts.current.potparam0 = (double) tmpval;

  XmScaleGetValue(extended_opts.bailout_scale, &tmpval);
  extended_opts.current.potparam2 = (double) tmpval;

  XmScaleGetValue(extended_opts.distance_estimator_scale, &tmpval);
  extended_opts.current.usr_distest = (int) tmpval;

  XmScaleGetValue(extended_opts.width_factor_scale, &tmpval);
  extended_opts.current.distestwidth = (int) tmpval;

  XmScaleGetValue(extended_opts.color_cycling_from_scale, &tmpval);
  extended_opts.current.rotate_lo = (int) tmpval;

  XmScaleGetValue(extended_opts.color_cycling_to_scale, &tmpval);
  extended_opts.current.rotate_hi = (int) tmpval;

  XmScaleGetValue(extended_opts.sixteen_bit_values_scale, &tmpval);
  extended_opts.current.pot16bit = (int) tmpval;

  txt = XmTextFieldGetString(extended_opts.periodicity_textfield);

  if((strstr(txt, "show") != (char *) NULL) ||
     (strstr(txt, "Show") != (char *) NULL) ||
     (strstr(txt, "SHOW") != (char *) NULL))
    {
    extended_opts.current.periodicitycheck = -1;
    }
  else if((strstr(txt, "no") != (char *) NULL) ||
          (strstr(txt, "No") != (char *) NULL) ||
          (strstr(txt, "NO") != (char *) NULL))
    {
    extended_opts.current.periodicitycheck = 0;
    }
  else if((strstr(txt, "yes") != (char *) NULL) ||
          (strstr(txt, "Yes") != (char *) NULL) ||
          (strstr(txt, "YES") != (char *) NULL))
    {
    extended_opts.current.periodicitycheck = 1;
    }
  else
    {
    invalid = (char *) NULL;

    validnum = strtod(txt, &invalid);

    if( invalid && *invalid )
      {
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }

    extended_opts.current.periodicitycheck = validnum;
    }

  txt = XmTextFieldGetString(extended_opts.symmetry_textfield);

  if((strstr(txt, "xaxis") != (char *) NULL) ||
     (strstr(txt, "Xaxis") != (char *) NULL) ||
     (strstr(txt, "XAXIS") != (char *) NULL))
    {
    extended_opts.current.symmetry = XAXIS;
    }
  else if((strstr(txt, "yaxis") != (char *) NULL) ||
          (strstr(txt, "Yaxis") != (char *) NULL) ||
          (strstr(txt, "YAXIS") != (char *) NULL))
    {
    extended_opts.current.symmetry = YAXIS;
    }
  else if((strstr(txt, "xyaxis") != (char *) NULL) ||
          (strstr(txt, "Xyaxis") != (char *) NULL) ||
          (strstr(txt, "XYAXIS") != (char *) NULL))
    {
    extended_opts.current.symmetry = XYAXIS;
    }
  else if((strstr(txt, "origin") != (char *) NULL) ||
          (strstr(txt, "Origin") != (char *) NULL) ||
          (strstr(txt, "ORIGIN") != (char *) NULL))
    {
    extended_opts.current.symmetry = ORIGIN;
    }
  else if((strstr(txt, "pi") != (char *) NULL) ||
          (strstr(txt, "Pi") != (char *) NULL) ||
          (strstr(txt, "PI") != (char *) NULL))
    {
    extended_opts.current.symmetry = PI_SYM;
    }
  else if((strstr(txt, "none") != (char *) NULL) ||
          (strstr(txt, "None") != (char *) NULL) ||
          (strstr(txt, "NONE") != (char *) NULL))
    {
    extended_opts.current.symmetry = NOSYM;
    }
  else
    {
    sprintf((char *) &msg,
      "Cannot resolve symmetry \"%s\".", txt);
    caution(main_window, (char *) &msg, NULL);
    extended_opts.current.symmetry = NOSYM;
    }

  txt = XmTextFieldGetString(extended_opts.rseed_textfield);

  validnum = atoi(txt);

  extended_opts.current.rseed = validnum;

  txt = XmTextFieldGetString(extended_opts.showdot_textfield);

  if((strstr(txt, "off") != (char *) NULL) ||
     (strstr(txt, "Off") != (char *) NULL) ||
     (strstr(txt, "OFF") != (char *) NULL))
    {
    extended_opts.current.showdot = -1;
    }
  else
    {
    invalid = (char *) NULL;

    validnum = strtod(txt, &invalid);

    if( invalid && *invalid )
      {
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }

    extended_opts.current.showdot = validnum;
    }

  if(finattract != extended_opts.current.finattract                 ||
     potparam[0] != extended_opts.current.potparam0                 ||
     potparam[1] != extended_opts.current.potparam1                 ||
     potparam[2] != extended_opts.current.potparam2                 ||
     usr_distest != extended_opts.current.usr_distest               ||
     distestwidth != extended_opts.current.distestwidth             ||
     inversion[0] != extended_opts.current.inversion0               ||
     inversion[1] != extended_opts.current.inversion1               ||
     inversion[2] != extended_opts.current.inversion2               ||
     pot16bit != extended_opts.current.pot16bit                     ||
     usr_periodicitycheck != extended_opts.current.periodicitycheck ||
     symmetry != extended_opts.current.symmetry                     ||
     ((fractype == PLASMA || fractype == CELLULAR || fractype == DIFFUSION)
       && rseed != extended_opts.current.rseed))
    changed = 1;
  else
    changed = 0;

  finattract = extended_opts.current.finattract;
  potparam[0] = extended_opts.current.potparam0;
  potparam[1] = extended_opts.current.potparam1;
  potparam[2] = extended_opts.current.potparam2;
  usr_distest = extended_opts.current.usr_distest;
  distestwidth = extended_opts.current.distestwidth;
  inversion[0] = extended_opts.current.inversion0;
  inversion[1] = extended_opts.current.inversion1;
  inversion[2] = extended_opts.current.inversion2;
  rotate_lo = extended_opts.current.rotate_lo;
  rotate_hi = extended_opts.current.rotate_hi;
  pot16bit = extended_opts.current.pot16bit;
  usr_periodicitycheck = extended_opts.current.periodicitycheck;
  symmetry = extended_opts.current.symmetry;
  rseed = extended_opts.current.rseed;
  showdot = extended_opts.current.showdot;

  XtFree(txt);

  if(changed)
    {
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    }

  if(pop_down == 1)
    XtPopdown(extended_options_dialog_shell);
}

/*ARGSUSED*/
static void extended_dialog_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(extended_options_dialog_shell);
}

/*ARGSUSED*/
static void extended_dialog_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPYOPTS;
  read_help(main_window, NULL, NULL);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: ext_opts.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.16  1995/06/03  20:18:36  darryl
 * Allow zooming and/or panning after changes in basic or
 * extended options.
 *
 * Revision 1.15  1995/05/30  17:13:03  darryl
 * Fix typo in scale widget label.
 *
 * Revision 1.14  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.13  1995/05/08  16:36:45  darryl
 * Update cycle ranges.
 *
 * Revision 1.12  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.11  1995/04/21  18:31:23  darryl
 * Update client_data handling.
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
 * Revision 1.9  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.8  1995/03/28  18:11:40  darryl
 * Fix trig function handling. Enhance center-mag button
 * management. Aesthetic changes in the parameter dialog.
 * Add frames around groups of scale widgets. Fix a bogus
 * message about color cell allocation if sharecolor.
 *
 * Revision 1.7  1995/03/25  20:27:49  darryl
 * Add cancel buttons for several dialogs. Make the corner
 * values dialog title dynamic, reflecting the current mode.
 *
 * Revision 1.6  1995/03/22  18:08:22  darryl
 * Prevent crashes when recursive calls are made to dialog
 * creation routines, enlarge file name and path variable
 * sizes to at least 256 bytes, don't pop down the parent
 * of a warning or caution dialog in the case of a list
 * or file selection dialog mis-match.
 *
 * Revision 1.5  1995/03/21  00:34:23  darryl
 * Add "Apply" buttons to allow values in a popped-up
 * dialog to be applied to the program without having
 * to press the "Done" button, which pops down the dialog.
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
 * Revision 0.106  1994/12/02  02:25:20  darryl
 * Optimization and lint cleanup.
 *
 * Revision 0.105  1994/12/01  20:35:03  darryl
 * Add periodicity checking, symmetry, rseed, and showdot to
 * the extended options dialog so they do not have to be
 * set only on the command line or in a parameter file.
 *
 * Revision 0.104  1994/11/29  06:15:07  darryl
 * More bulletproofing the zoom box, and default rotate_lo to
 * two so the lake is not cycled, and fix a bug where the
 * scale multiple on the upper and lower cycle range scales
 * was not set to one.
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
