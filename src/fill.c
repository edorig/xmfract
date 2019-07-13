
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: fill.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void three_d_fill_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_fill_dflt_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_fill_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_fill_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_fill_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget three_d_fill_dialog_shell = NULL;
static Widget three_d_fill_dialog = NULL;

/*ARGSUSED*/
void select_3d_fill(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!three_d_fill_dialog_shell)
    {
    Widget form;
    Widget frame;
    Widget label;
    Widget button;
    Widget radio_box;
    XmString str;
    Arg args[MAX_ARGS];
    int btnbase;
    int fracbase;
    int n;

    static char *three_d_fill_radio_box_items[] =
      {
      "Make a surface grid",
      "Just draw the points",
      "Connect the dots (wire frame)",
      "Surface fill (colors interpolated)",
      "Surface fill (colors not interpolated)",
      "Solid fill (bars up from \"ground\")",
      "Light source",
      "Light source before transformation",
      "Light source after transformation",
      };

    btnbase = 20;
    fracbase = btnbase * 5 - 1;

    three_d_fill_dialog_shell = XmCreateDialogShell
      (main_window, "Select Three-D Fill Type", NULL, 0);

    XtAddCallback(three_d_fill_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(three_d_fill_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Three Dimensional Fill Type");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    three_d_fill_dialog = XmCreateForm
      (three_d_fill_dialog_shell, "three_d_fill_dialog", args, n);

    XtAddCallback(three_d_fill_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    /*
     * Three-D Fill Type
     * (radio buttons)
     *
     * Make a surface grid
     * Just draw the points
     * Connect the dots (wire frame)
     * Surface fill (colors interpolated)
     * Surface fill (colors not interpolated)
     * Solid fill (bars up from "ground")
     * if(SPHERE)
     *   Light source
     * else
     *   Light source before transformation
     *   Light source after transformation
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    frame = XmCreateFrame
      (three_d_fill_dialog, "three_d_fill_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Three-D FIll Mode:");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "three_d_fill_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

    radio_box = XmCreateRadioBox(frame, "three_d_fill_radio_box", args, n);

    three_d_opts.fill_type_surface_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[0]),
        XmNset, False,
        NULL);

    three_d_opts.fill_type_points_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[1]),
        XmNset, True,
        NULL);

    three_d_opts.fill_type_dots_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[2]),
        XmNset, False,
        NULL);

    three_d_opts.fill_type_sfc_clr_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[3]),
        XmNset, False,
        NULL);

    three_d_opts.fill_type_sfc_nclr_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[4]),
        XmNset, False,
        NULL);

    three_d_opts.fill_type_solid_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[5]),
        XmNset, False,
        NULL);

    three_d_opts.fill_type_light_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[6]),
        XmNset, False,
        NULL);

    three_d_opts.fill_type_light_b4_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[7]),
        XmNset, False,
        NULL);

    three_d_opts.fill_type_light_after_toggle =
      XtVaCreateManagedWidget("three_d_fill_radio_box_btn",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(three_d_fill_radio_box_items[8]),
        XmNset, False,
        NULL);

    XtManageChild(radio_box);
    XtManageChild(frame);

    n = 0;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

    form = XmCreateForm(three_d_fill_dialog, "button_form", args, n);

    /*
     * Help
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftPosition, 0); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton(form, "three_d_fill_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_fill_help_btn_cb, NULL );

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton(form, "three_d_fill_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_fill_reset_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton(form, "three_d_fill_dflt_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_fill_dflt_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton(form, "three_d_fill_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_fill_cancel_btn_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (form, "three_d_fill_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      three_d_fill_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtManageChild(form);

    XtVaSetValues(three_d_fill_dialog,
      XmNdefaultButton,
      button,
      NULL);
    }

  three_d_opts.current.filltype = FILLTYPE;

  if(SPHERE)
    {
    XtUnmanageChild(three_d_opts.fill_type_light_b4_toggle);
    XtUnmanageChild(three_d_opts.fill_type_light_after_toggle);
    XtManageChild(three_d_opts.fill_type_light_toggle);
    }
  else
    {
    XtManageChild(three_d_opts.fill_type_light_b4_toggle);
    XtManageChild(three_d_opts.fill_type_light_after_toggle);
    XtUnmanageChild(three_d_opts.fill_type_light_toggle);
    }

  if(SPHERE)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, True, False);
    }

  if(three_d_opts.current.filltype == -1)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 0)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState(three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 1)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 2)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 3)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 4)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, True, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 5)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, True, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 6)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, True, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 7)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, True, False);
      }
    }
  XtManageChild(three_d_fill_dialog);
  XtManageChild(three_d_fill_dialog_shell);
  XtPopup(three_d_fill_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void three_d_fill_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(three_d_opts.current.filltype == -1)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 0)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 1)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 2)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 3)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, True, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 4)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, True, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 5)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, True, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 6)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, True, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, False, False);
      }
    }
  else if(three_d_opts.current.filltype == 7)
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_surface_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_points_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_dots_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_clr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_sfc_nclr_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_solid_toggle, False, False);
    if(SPHERE)
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_toggle, False, False);
    else
      {
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_b4_toggle, False, False);
      XmToggleButtonSetState
        (three_d_opts.fill_type_light_after_toggle, True, False);
      }
    }
}

/*ARGSUSED*/
static void three_d_fill_dflt_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.filltype = 1;
  XmToggleButtonSetState(three_d_opts.fill_type_surface_toggle, False, False);
  XmToggleButtonSetState(three_d_opts.fill_type_points_toggle, True, False);
  XmToggleButtonSetState(three_d_opts.fill_type_dots_toggle, False, False);
  XmToggleButtonSetState(three_d_opts.fill_type_sfc_clr_toggle, False, False);
  XmToggleButtonSetState(three_d_opts.fill_type_sfc_nclr_toggle, False, False);
  XmToggleButtonSetState(three_d_opts.fill_type_solid_toggle, False, False);
  if(SPHERE)
    XmToggleButtonSetState
      (three_d_opts.fill_type_light_toggle, False, False);
  else
    {
    XmToggleButtonSetState
      (three_d_opts.fill_type_light_b4_toggle, False, False);
    XmToggleButtonSetState
      (three_d_opts.fill_type_light_after_toggle, False, False);
    }
}

/*ARGSUSED*/
static void three_d_fill_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(XmToggleButtonGetState
    (three_d_opts.fill_type_surface_toggle) == True)
    three_d_opts.current.filltype = FILLTYPE = -1;
  else if(XmToggleButtonGetState
    (three_d_opts.fill_type_points_toggle) == True)
    three_d_opts.current.filltype = FILLTYPE = 0;
  else if(XmToggleButtonGetState
    (three_d_opts.fill_type_dots_toggle) == True)
    three_d_opts.current.filltype = FILLTYPE = 1;
  else if(XmToggleButtonGetState
    (three_d_opts.fill_type_sfc_clr_toggle) == True)
    three_d_opts.current.filltype = FILLTYPE = 2;
  else if(XmToggleButtonGetState
    (three_d_opts.fill_type_sfc_nclr_toggle) == True)
    three_d_opts.current.filltype = FILLTYPE = 3;
  else if(XmToggleButtonGetState
    (three_d_opts.fill_type_solid_toggle) == True)
    three_d_opts.current.filltype = FILLTYPE = 4;
  if(SPHERE)
    {
    if(XmToggleButtonGetState
    (three_d_opts.fill_type_light_toggle) == True)
      three_d_opts.current.filltype = FILLTYPE = 5;
    }
  else
    {
    if(XmToggleButtonGetState
      (three_d_opts.fill_type_light_b4_toggle) == True)
      three_d_opts.current.filltype = FILLTYPE = 6;
    else if(XmToggleButtonGetState
      (three_d_opts.fill_type_light_after_toggle) == True)
      three_d_opts.current.filltype = FILLTYPE = 7;
    }

  XtPopdown(three_d_fill_dialog_shell);

  if(glassestype)
    get_funny_glasses_params(main_window, NULL, NULL);
  else
    check_mapfile(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void three_d_fill_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELP3DFILL;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void three_d_fill_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  initvars_fractal();
  XtPopdown(three_d_fill_dialog_shell);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: fill.c,v $
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
