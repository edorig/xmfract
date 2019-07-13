
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: three_d_params.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void three_d_params_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_params_dflt_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_params_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_params_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_params_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget three_d_params_dialog_shell = NULL;
static Widget three_d_params_dialog = NULL;

/*ARGSUSED*/
void get_3d_params(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[128];

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!three_d_params_dialog_shell)
    {
    Widget radio_box;
    Widget frame;
    Widget label;
    Widget button;
    Widget previous;
    XmString str;
    Arg args[MAX_ARGS];
    int btnbase;
    int fracbase;
    int n;

    char *preview_radio_box_items[] =
      {
      "Yes",
      "No",
      };

    char *show_box_radio_box_items[] =
      {
      "Yes",
      "No",
      };

    char *sphere_radio_box_items[] =
      {
      "Yes",
      "No",
      };

    char *stereo_radio_box_items[] =
      {
      "No",
      "Alternate",
      "Photo",
      "Superimpose",
      };

    char *ray_trace_radio_box_items[] =
      {
      "No",
      "DKB/POVRay",
      "VIVID",
      "RAYSHADE",
      "RAW",
      "ACROSPIN",
      "MTV",
      "DXF",
      };

    char *brief_radio_box_items[] =
      {
      "Yes",
      "No",
      };

    char *targa_radio_box_items[] =
      {
      "No",
      "Yes",
      };

    btnbase = 20;
    fracbase = btnbase * 5 - 1;

    three_d_params_dialog_shell = XmCreateDialogShell
      (main_window, "Select Three-D Parameters", NULL, 0);

    XtAddCallback(three_d_params_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(three_d_params_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Three Dimensional Options");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    three_d_params_dialog = XmCreateForm
      (three_d_params_dialog_shell, "three_d_params_dialog", args, n);

    XtAddCallback(three_d_params_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    /*
     * Preview Mode?
     * (radio box)
     *
     * Yes
     * No
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "preview_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Preview Mode?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "preview_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(frame,
      "preview_radio_box", args, n);

    three_d_opts.preview_yes_toggle =
      XtVaCreateManagedWidget("preview_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(preview_radio_box_items[0]),
        XmNset, False,
        NULL);

    three_d_opts.preview_no_toggle =
      XtVaCreateManagedWidget("preview_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(preview_radio_box_items[1]),
        XmNset, True,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    /*
     * Show Box?
     * (radio box)
     *
     * Yes
     * No
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "show_box_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Show Box?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "show_box_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(frame,
      "show_box_radio_box", args, n);

    three_d_opts.show_box_yes_toggle =
      XtVaCreateManagedWidget("show_box_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString, (XtArgVal)XmStringCreateLocalized(
          show_box_radio_box_items[0]),
        XmNset, False,
        NULL);

    three_d_opts.show_box_no_toggle =
      XtVaCreateManagedWidget("show_box_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString, (XtArgVal)XmStringCreateLocalized(
          show_box_radio_box_items[1]),
        XmNset, True,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    /*
     * Coarseness, preview/grid/ray (in y dir)
     * (scale)
     *
     */

    sprintf((char *) &tempstr, "Coarsness:");
    str = XmStringCreateLocalized((char *) &tempstr);

    three_d_opts.coarseness_scale =
      XtVaCreateManagedWidget("coarseness_scale",
        xmScaleWidgetClass,
        three_d_params_dialog,
        XmNtitleString, str,
        XmNorientation, XmHORIZONTAL,
        XmNvalue, 20,
        XmNmaximum, 500,
        XmNminimum, 1,
        XmNshowValue, True,
        XmNscaleMultiple, 1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, frame,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightPosition, fracbase - 1,
        NULL);

    XmStringFree(str);

    /*
     * Spherical Projection?
     * (radio box)
     *
     * Yes
     * No
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, three_d_opts.coarseness_scale); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "sphere_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Spherical Projection?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "sphere_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(frame,
      "sphere_radio_box", args, n);

    XtManageChild(radio_box);

    three_d_opts.sphere_yes_toggle =
      XtVaCreateManagedWidget("sphere_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(sphere_radio_box_items[0]),
        XmNset, False,
        NULL);

    three_d_opts.sphere_no_toggle =
      XtVaCreateManagedWidget("sphere_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(sphere_radio_box_items[1]),
        XmNset, True,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    /*
     * Stereo (R/B 3D)?
     * (radio box)
     *
     * No
     * Alternate
     * Superimpose
     * Photo
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "stereo_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Stereo (Red/Blue 3D)?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "stereo_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(frame,
      "stereo_radio_box", args, n);

    three_d_opts.stereo_no_toggle =
      XtVaCreateManagedWidget("stereo_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(stereo_radio_box_items[0]),
        XmNset, True,
        NULL);

    three_d_opts.stereo_alt_toggle =
      XtVaCreateManagedWidget("stereo_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(stereo_radio_box_items[1]),
        XmNset, False,
        NULL);

    three_d_opts.stereo_photo_toggle =
      XtVaCreateManagedWidget("stereo_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(stereo_radio_box_items[2]),
        XmNset, False,
        NULL);

    three_d_opts.stereo_super_toggle =
      XtVaCreateManagedWidget("stereo_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(stereo_radio_box_items[3]),
        XmNset, False,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    /*
     * Ray trace out?
     * (radio box)
     *
     * No
     * DKB/POVRay
     * VIVID
     * RAW
     * MTV
     * RAYSHADE
     * ACROSPIN
     * DXF
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "ray_trace_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Ray trace out?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "ray_trace_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(frame,
      "ray_trace_radio_box", args, n);

    three_d_opts.ray_trace_no_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[0]),
        XmNset, True,
        NULL);

    three_d_opts.ray_trace_dkb_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[1]),
        XmNset, False,
        NULL);

    three_d_opts.ray_trace_vivid_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[2]),
        XmNset, False,
        NULL);

    three_d_opts.ray_trace_shade_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[3]),
        XmNset, False,
        NULL);

    three_d_opts.ray_trace_raw_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[4]),
        XmNset, False,
        NULL);

    three_d_opts.ray_trace_acro_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[5]),
        XmNset, False,
        NULL);

    three_d_opts.ray_trace_mtv_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[6]),
        XmNset, False,
        NULL);

    three_d_opts.ray_trace_dxf_toggle =
      XtVaCreateManagedWidget("ray_trace_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(ray_trace_radio_box_items[7]),
        XmNset, False,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    previous = frame;

    /*
     * Brief output?
     * (radio box)
     *
     * Yes
     * No
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, previous); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "brief_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Brief output?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "brief_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(frame,
      "brief_radio_box", args, n);

    three_d_opts.brief_yes_toggle =
      XtVaCreateManagedWidget("brief_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(brief_radio_box_items[0]),
        XmNset, False,
        NULL);

    three_d_opts.brief_no_toggle =
      XtVaCreateManagedWidget("brief_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(brief_radio_box_items[1]),
        XmNset, True,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    /*
     * Targa output?
     * (radio box)
     *
     * No
     * Yes
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, previous); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "targa_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Targa output?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "targa_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    radio_box = XmCreateRadioBox(frame,
      "targa_radio_box", args, n);

    three_d_opts.targa_no_toggle =
      XtVaCreateManagedWidget("targa_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(targa_radio_box_items[0]),
        XmNset, True,
        NULL);

    three_d_opts.targa_yes_toggle =
      XtVaCreateManagedWidget("targa_radio_box_btn",
        xmToggleButtonWidgetClass, radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(targa_radio_box_items[1]),
        XmNset, False,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    /*
     * Output File Name
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    frame = XmCreateFrame
      (three_d_params_dialog, "show_box_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Output file name:");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "output_fname_textfield_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    three_d_opts.output_fname_textfield =
      XtVaCreateManagedWidget("output_fname_textfield",
        xmTextFieldWidgetClass, frame,
        XmNorientation, XmHORIZONTAL,
        NULL);

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
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (three_d_params_dialog, "three_d_params_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_params_help_btn_cb, NULL );

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
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (three_d_params_dialog, "three_d_params_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_params_reset_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (three_d_params_dialog, "three_d_params_dialog_dflt_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_params_dflt_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (three_d_params_dialog, "three_d_params_dialog_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_params_cancel_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    button = XmCreatePushButton
      (three_d_params_dialog, "three_d_params_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_params_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(three_d_params_dialog,
      XmNdefaultButton,
      button,
      NULL);

    XtManageChild(three_d_params_dialog);
    XtManageChild(three_d_params_dialog_shell);
    }

  three_d_opts.current.preview = preview;
  three_d_opts.current.showbox = showbox;
  three_d_opts.current.previewfactor = previewfactor;
  three_d_opts.current.sphere = SPHERE;
  three_d_opts.current.glassestype = glassestype;
  three_d_opts.current.ray = RAY;
  three_d_opts.current.brief = BRIEF;
  three_d_opts.current.ray_name = (char *) &ray_name;
  three_d_opts.current.targa_out = Targa_Out;

  sprintf((char *) &tempstr, "%s%s", (char *) &ray_name, ".ray");
  XmTextFieldSetString(three_d_opts.output_fname_textfield,
    (char *) &tempstr);

  if(three_d_opts.current.preview == 1)
    {
    XmToggleButtonSetState(three_d_opts.preview_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.preview_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.preview_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.preview_no_toggle, True, False);
    }

  if(three_d_opts.current.showbox == 1)
    {
    XmToggleButtonSetState(three_d_opts.show_box_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.show_box_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.show_box_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.show_box_no_toggle, True, False);
    }

  if(three_d_opts.current.sphere == 1)
    {
    XmToggleButtonSetState(three_d_opts.sphere_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.sphere_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.sphere_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.sphere_no_toggle, True, False);
    }

  if(three_d_opts.current.glassestype == 0)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 1)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 3)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 2)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, True, False);
    }

  if(three_d_opts.current.ray == 0)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 1)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 2)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 3)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 4)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 5)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 6)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 7)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, True, False);
    }
  if(three_d_opts.current.brief == 1)
    {
    XmToggleButtonSetState(three_d_opts.brief_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.brief_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.brief_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.brief_no_toggle, True, False);
    }

  if(three_d_opts.current.targa_out == 0)
    {
    XmToggleButtonSetState(three_d_opts.targa_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.targa_no_toggle, True, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.targa_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.targa_no_toggle, False, False);
    }
  XtManageChild(three_d_params_dialog);
  XtManageChild(three_d_params_dialog_shell);
  XtPopup(three_d_params_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void three_d_params_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(three_d_opts.current.preview == 1)
    {
    XmToggleButtonSetState(three_d_opts.preview_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.preview_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.preview_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.preview_no_toggle, True, False);
    }

  if(three_d_opts.current.showbox == 1)
    {
    XmToggleButtonSetState(three_d_opts.show_box_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.show_box_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.show_box_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.show_box_no_toggle, True, False);
    }

  if(three_d_opts.current.sphere == 1)
    {
    XmToggleButtonSetState(three_d_opts.sphere_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.sphere_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.sphere_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.sphere_no_toggle, True, False);
    }

  if(three_d_opts.current.glassestype == 0)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 1)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 3)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.current.glassestype == 2)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, True, False);
    }

  if(three_d_opts.current.ray == 0)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 1)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 2)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 3)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 4)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 5)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 6)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.current.ray == 7)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, True, False);
    }

  if(three_d_opts.current.brief == 1)
    {
    XmToggleButtonSetState(three_d_opts.brief_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.brief_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.brief_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.brief_no_toggle, True, False);
    }

  if(three_d_opts.current.targa_out == 0)
    {
    XmToggleButtonSetState(three_d_opts.targa_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.targa_no_toggle, True, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.targa_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.targa_no_toggle, False, False);
    }
}

/*ARGSUSED*/
static void three_d_params_dflt_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current = three_d_opts.dflt;

  if(three_d_opts.dflt.preview == 1)
    {
    XmToggleButtonSetState(three_d_opts.preview_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.preview_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.preview_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.preview_no_toggle, True, False);
    }

  if(three_d_opts.dflt.showbox == 1)
    {
    XmToggleButtonSetState(three_d_opts.show_box_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.show_box_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.show_box_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.show_box_no_toggle, True, False);
    }

  if(three_d_opts.dflt.sphere == 1)
    {
    XmToggleButtonSetState(three_d_opts.sphere_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.sphere_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.sphere_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.sphere_no_toggle, True, False);
    }

  if(three_d_opts.dflt.glassestype == 0)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.dflt.glassestype == 1)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.dflt.glassestype == 3)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, False, False);
    }
  else if(three_d_opts.dflt.glassestype == 2)
    {
    XmToggleButtonSetState(three_d_opts.stereo_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_alt_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_photo_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.stereo_super_toggle, True, False);
    }

  if(three_d_opts.dflt.ray == 0)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.dflt.ray == 1)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.dflt.ray == 2)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.dflt.ray == 3)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.dflt.ray == 4)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.dflt.ray == 5)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.dflt.ray == 6)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, False, False);
    }
  else if(three_d_opts.dflt.ray == 7)
    {
    XmToggleButtonSetState(three_d_opts.ray_trace_no_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dkb_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_vivid_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_shade_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_raw_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_acro_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_mtv_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.ray_trace_dxf_toggle, True, False);
    }

  if(three_d_opts.dflt.brief == 1)
    {
    XmToggleButtonSetState(three_d_opts.brief_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.brief_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.brief_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.brief_no_toggle, True, False);
    }

  if(three_d_opts.dflt.targa_out == 0)
    {
    XmToggleButtonSetState(three_d_opts.targa_yes_toggle, False, False);
    XmToggleButtonSetState(three_d_opts.targa_no_toggle, True, False);
    }
  else
    {
    XmToggleButtonSetState(three_d_opts.targa_yes_toggle, True, False);
    XmToggleButtonSetState(three_d_opts.targa_no_toggle, False, False);
    }

  XmScaleSetValue(three_d_opts.coarseness_scale,
    three_d_opts.current.previewfactor);
}

/*ARGSUSED*/
static void three_d_params_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELP3DMODE;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void three_d_params_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(XmToggleButtonGetState(three_d_opts.preview_yes_toggle) == True)
    three_d_opts.current.preview = preview = 1;
  else if(XmToggleButtonGetState(three_d_opts.preview_no_toggle) == True)
    three_d_opts.current.preview = preview = 0;

  if(XmToggleButtonGetState(three_d_opts.show_box_yes_toggle) == True)
    three_d_opts.current.showbox = showbox = 1;
  else if(XmToggleButtonGetState(three_d_opts.show_box_no_toggle) == True)
    three_d_opts.current.showbox = showbox = 0;

  if(XmToggleButtonGetState(three_d_opts.sphere_yes_toggle) == True)
    three_d_opts.current.sphere = SPHERE = 1;
  else if(XmToggleButtonGetState(three_d_opts.sphere_no_toggle) == True)
    three_d_opts.current.sphere = SPHERE = 0;

  if(XmToggleButtonGetState(three_d_opts.stereo_no_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 0;
  else if(XmToggleButtonGetState(three_d_opts.stereo_alt_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 1;
  else if(XmToggleButtonGetState(three_d_opts.stereo_photo_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 3;
  else if(XmToggleButtonGetState(three_d_opts.stereo_super_toggle) == True)
    three_d_opts.current.glassestype = glassestype = 2;

  if(XmToggleButtonGetState(three_d_opts.ray_trace_no_toggle) == True)
    three_d_opts.current.ray = RAY = 0;
  else if(XmToggleButtonGetState(three_d_opts.ray_trace_dkb_toggle) == True)
    three_d_opts.current.ray = RAY = 1;
  else if(XmToggleButtonGetState(three_d_opts.ray_trace_vivid_toggle) == True)
    three_d_opts.current.ray = RAY = 2;
  else if(XmToggleButtonGetState(three_d_opts.ray_trace_shade_toggle) == True)
    three_d_opts.current.ray = RAY = 5;
  else if(XmToggleButtonGetState(three_d_opts.ray_trace_raw_toggle) == True)
    three_d_opts.current.ray = RAY = 3;
  else if(XmToggleButtonGetState(three_d_opts.ray_trace_acro_toggle) == True)
    three_d_opts.current.ray = RAY = 6;
  else if(XmToggleButtonGetState(three_d_opts.ray_trace_mtv_toggle) == True)
    three_d_opts.current.ray = RAY = 4;
  else if(XmToggleButtonGetState(three_d_opts.ray_trace_dxf_toggle) == True)
    three_d_opts.current.ray = RAY = 7;

  if(XmToggleButtonGetState(three_d_opts.brief_yes_toggle) == True)
    three_d_opts.current.brief = BRIEF = 1;
  else if(XmToggleButtonGetState(three_d_opts.brief_no_toggle) == True)
    three_d_opts.current.brief = BRIEF = 0;

  if(XmToggleButtonGetState(three_d_opts.targa_yes_toggle) == True)
    three_d_opts.current.targa_out = Targa_Out = 1;
  else if(XmToggleButtonGetState(three_d_opts.targa_no_toggle) == True)
    three_d_opts.current.targa_out = Targa_Out = 0;

  XmScaleGetValue(three_d_opts.coarseness_scale,
    &(three_d_opts.current.previewfactor));
  previewfactor = three_d_opts.current.previewfactor;

  if(!gotfile)
    set_3d_defaults();
  else
    gotfile = 0;

  XtPopdown(three_d_params_dialog_shell);

  if(!RAY)
    select_3d_fill(main_window, NULL, NULL);
  else
    {
    if(glassestype)
      get_funny_glasses_params(main_window, NULL, NULL);
    else
      check_mapfile(main_window, NULL, NULL);
    }
}

/*ARGSUSED*/
static void three_d_params_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  initvars_fractal();
  XtPopdown(three_d_params_dialog_shell);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: three_d_params.c,v $
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
