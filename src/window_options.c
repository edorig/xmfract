
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: window_options.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void viewwin_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void window_dialog_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void viewwin_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void viewwin_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void viewwin_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget window_options_dialog_shell = NULL;

/*ARGSUSED*/
void create_window_options_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Widget window_options_dialog;
  Widget frame;
  Widget radio_box;
  Widget label;
  Widget button;
  XmString str;
  char tempstr[64];
  Arg args[MAX_ARGS];
  int n;
  int btnbase;
  int fracbase;

  btnbase = 20;
  fracbase = btnbase * 5 - 1;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  viewwindow = viewwin_opts.current.viewwindow;
  viewcrop = viewwin_opts.current.viewcrop;
  viewreduction = viewwin_opts.current.viewreduction;
  finalaspectratio = viewwin_opts.current.finalaspectratio;
  viewxdots = viewwin_opts.current.viewxdots;
  viewydots = viewwin_opts.current.viewydots;

  window_options_dialog_shell = XmCreateDialogShell
    (main_window, "View Window Options", NULL, 0);

  XtAddCallback(window_options_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(window_options_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  str = XmStringCreateLocalized("Window Options");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNdialogTitle, str); n++;
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;

  window_options_dialog = XmCreateForm
    (window_options_dialog_shell, "window_options_dialog", args, n);

  XtAddCallback(window_options_dialog, XmNmapCallback,
    map_dialog, NULL);

  XmStringFree(str);

  /*
   * Preview display? (no for full screen)
   * (radio button)
   *
   */

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  frame = XmCreateFrame
    (window_options_dialog, "preview_display_radio_box_frame", args, n);

  n = 0;
  str = XmStringCreateLocalized("Preview display? (no for full screen)");
  XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
  XtSetArg(args[n], XmNchildVerticalAlignment,
    XmALIGNMENT_WIDGET_BOTTOM); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  label = XmCreateLabel
    (frame, "preview_display_box_label", args, n);

  XtManageChild(label);
  XmStringFree(str);

  n = 0;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNradioBehavior, True); n++;
  XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

  radio_box = XmCreateRadioBox
    (frame, "preview_display_radio_box", args, n);

  n = 0;
  str = XmStringCreateLocalized("Yes, preview");
  if(viewwin_opts.current.viewwindow == 1)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  viewwin_opts.preview_yes_toggle = XmCreateToggleButton
    (radio_box, "yes_preview_display_toggle", args, n);

  XtManageChild(viewwin_opts.preview_yes_toggle);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("No preview");
  if(viewwin_opts.current.viewwindow == 0)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  viewwin_opts.preview_no_toggle = XmCreateToggleButton
    (radio_box, "no_preview_display_toggle", args, n);

  XtManageChild(viewwin_opts.preview_no_toggle);

  XmStringFree(str);

  XtManageChild(radio_box);
  XtManageChild(frame);

  /*
   * Crop starting coordinates to new aspect ratio?
   * (radio button)
   *
   */

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

  frame = XmCreateFrame
    (window_options_dialog, "crop_start_coord_radio_box_frame", args, n);

  n = 0;
  str = XmStringCreateLocalized("Crop starting coords to new aspect ratio?");
  XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
  XtSetArg(args[n], XmNchildVerticalAlignment,
    XmALIGNMENT_WIDGET_BOTTOM); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  label = XmCreateLabel
    (frame, "crop_start_coord_box_label", args, n);

  XtManageChild(label);
  XmStringFree(str);

  n = 0;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNradioBehavior, True); n++;
  XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

  radio_box = XmCreateRadioBox
    (frame, "crop_start_coord_radio_box", args, n);

  n = 0;
  str = XmStringCreateLocalized("Yes, crop");
  if(viewwin_opts.current.viewcrop == 1)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  viewwin_opts.crop_yes_toggle = XmCreateToggleButton
    (radio_box, "yes_crop_start_coord_toggle", args, n);

  XtManageChild(viewwin_opts.crop_yes_toggle);

  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("No crop");
  if(viewwin_opts.current.viewcrop == 0)
    {
    XtSetArg(args[n], XmNset, True); n++;
    }
  else
    {
    XtSetArg(args[n], XmNset, False); n++;
    }
  XtSetArg(args[n], XmNlabelString, str); n++;

  viewwin_opts.crop_no_toggle = XmCreateToggleButton
    (radio_box, "no_crop_start_coord_toggle", args, n);

  XtManageChild(viewwin_opts.crop_no_toggle);
  XmStringFree(str);

  XtManageChild(radio_box);
  XtManageChild(frame);

  /*
   * Auto window size reduction factor
   * (textfield)
   *
   */

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

  frame = XmCreateFrame
    (window_options_dialog, "auto_reduction_textfield_frame", args, n);

  n = 0;
  str = XmStringCreateLocalized("Auto window size reduction factor");
  XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
  XtSetArg(args[n], XmNchildVerticalAlignment,
    XmALIGNMENT_WIDGET_BOTTOM); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  label = XmCreateLabel
    (frame, "auto_reduction_textfield_label", args, n);

  XtManageChild(label);
  XmStringFree(str);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNeditable, True); n++;

  viewwin_opts.auto_reduction_textfield = XmCreateTextField
    (frame, "auto_reduction_textfield", args, n);

  XtManageChild(viewwin_opts.auto_reduction_textfield);

  XtManageChild(frame);

  sprintf((char *) &tempstr, "%f", viewwin_opts.current.viewreduction);

  XmTextFieldSetString(viewwin_opts.auto_reduction_textfield,
    (char *) &tempstr);

  /*
   * Final media overall aspect ratio, y/x
   * (textfield)
   *
   */

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

  frame = XmCreateFrame
    (window_options_dialog, "final_aspect_ratio_textfield_frame", args, n);

  n = 0;
  str = XmStringCreateLocalized("Final media overall aspect ratio, y/x");
  XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
  XtSetArg(args[n], XmNchildVerticalAlignment,
    XmALIGNMENT_WIDGET_BOTTOM); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  label = XmCreateLabel
    (frame, "final_aspect_ratio_textfield_label", args, n);

  XtManageChild(label);
  XmStringFree(str);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNeditable, True); n++;

  viewwin_opts.final_aspect_textfield = XmCreateTextField
    (frame, "final_aspect_ratio_textfield", args, n);

  XtManageChild(viewwin_opts.final_aspect_textfield);

  XtManageChild(frame);

  sprintf((char *) &tempstr, "%f", viewwin_opts.current.finalaspectratio);

  XmTextFieldSetString(viewwin_opts.final_aspect_textfield,
    (char *) &tempstr);

  /*
   * Explicit size x pixels (0 for auto size)
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Explicit size x pixels (0 for auto size)");
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNmaximum, 2048); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNvalue, viewwin_opts.current.viewxdots); n++;

  viewwin_opts.viewxdots_scale = XmCreateScale
    (window_options_dialog, "explicit_x_size_scale", args, n);

  XtManageChild(viewwin_opts.viewxdots_scale);

  XmStringFree(str);

  /*
   * y pixels (0 to base on aspect ratio)
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Y pixels (0 to base on aspect ratio)");
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, viewwin_opts.viewxdots_scale); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNmaximum, 2048); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNvalue, viewwin_opts.current.viewydots); n++;

  viewwin_opts.viewydots_scale = XmCreateScale
    (window_options_dialog, "explicit_y_size_scale", args, n);

  XtManageChild(viewwin_opts.viewydots_scale);

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
  XtSetArg(args[n], XmNtopWidget, viewwin_opts.viewydots_scale); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

  button = XmCreatePushButton
    (window_options_dialog, "window_dialog_help_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    window_dialog_help_btn_cb, NULL);

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
  XtSetArg(args[n], XmNtopWidget, viewwin_opts.viewydots_scale); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

  button = XmCreatePushButton
    (window_options_dialog, "window_dialog_reset_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    viewwin_reset_button_cb, NULL);

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
  XtSetArg(args[n], XmNtopWidget, viewwin_opts.viewydots_scale); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

  button = XmCreatePushButton
    (window_options_dialog, "window_dialog_dflts_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    viewwin_dflt_button_cb, NULL);

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
  XtSetArg(args[n], XmNtopWidget, viewwin_opts.viewydots_scale); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

  button = XmCreatePushButton
    (window_options_dialog, "window_dialog_cancel_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    viewwin_cancel_button_cb, NULL);

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
  XtSetArg(args[n], XmNtopWidget, viewwin_opts.viewydots_scale); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

  button = XmCreatePushButton
    (window_options_dialog, "window_dialog_done_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    viewwin_done_button_cb, NULL);

  XtManageChild(button);
  XmStringFree(str);

  XtVaSetValues(window_options_dialog,
    XmNdefaultButton,
    button,
    NULL);

  XtManageChild(window_options_dialog);
  XtManageChild(window_options_dialog_shell);
  XtPopup(window_options_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void viewwin_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[64];

  if(viewwin_opts.current.viewwindow == 1)
    {
    XmToggleButtonSetState
      (viewwin_opts.preview_yes_toggle, True, False);
    XmToggleButtonSetState
      (viewwin_opts.preview_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState
      (viewwin_opts.preview_yes_toggle, False, False);
    XmToggleButtonSetState
      (viewwin_opts.preview_no_toggle, True, False);
    }

  if(viewwin_opts.current.viewcrop == 1)
    {
    XmToggleButtonSetState
      (viewwin_opts.crop_yes_toggle, True, False);
    XmToggleButtonSetState
      (viewwin_opts.crop_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState
      (viewwin_opts.crop_yes_toggle, False, False);
    XmToggleButtonSetState
      (viewwin_opts.crop_no_toggle, True, False);
    }

  sprintf((char *) &tempstr, "%f", viewwin_opts.current.viewreduction);
  XmTextFieldSetString(viewwin_opts.auto_reduction_textfield,
    (char *) &tempstr);

  sprintf((char *) &tempstr, "%f", viewwin_opts.current.finalaspectratio);
  XmTextFieldSetString(viewwin_opts.final_aspect_textfield,
    (char *) &tempstr);

  XmScaleSetValue(viewwin_opts.viewxdots_scale,
    (int) viewwin_opts.current.viewxdots);

  XmScaleSetValue(viewwin_opts.viewydots_scale,
    (int) viewwin_opts.current.viewydots);
}

/*ARGSUSED*/
static void window_dialog_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPVIEW;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void viewwin_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *txt;
  char *invalid;
  double validnum;
  int tmp_viewwindow;
  int tmp_viewcrop;
  float tmp_viewreduction = 0.0;
  float tmp_finalaspectratio = 0.0;
  int tmp_viewxdots;
  int tmp_viewydots;
  Boolean success;
  Boolean changed;
  char msg[128];

  success = True;
  changed = False;

  if(XmToggleButtonGetState
    (viewwin_opts.preview_yes_toggle) == True)
    tmp_viewwindow = 1;
  else
    tmp_viewwindow = 0;

  if(XmToggleButtonGetState
    (viewwin_opts.crop_yes_toggle) == True)
    tmp_viewcrop = 1;
  else
    tmp_viewcrop = 0;

  txt = XmTextFieldGetString(viewwin_opts.auto_reduction_textfield);

  invalid = (char *) NULL;

  validnum = strtod(txt, &invalid);

  if(invalid && *invalid)
    {
    sprintf((char *) &msg,
      "Cannot resolve floating point number \"%s\".", txt);
    caution(main_window, (char *) &msg, NULL);
    success = False;
    }
  else
    {
    success = True;
    tmp_viewreduction = validnum;
    }

  txt = XmTextFieldGetString(viewwin_opts.final_aspect_textfield);

  invalid = (char *) NULL;

  validnum = strtod(txt, &invalid);

  if(invalid && *invalid)
    {
    sprintf((char *) &msg,
      "Cannot resolve floating point number \"%s\".", txt);
    caution(main_window, (char *) &msg, NULL);
    success = False;
    }
  else
    {
    success = True;
    tmp_finalaspectratio = validnum;
    }

  XmScaleGetValue(viewwin_opts.viewxdots_scale,
      &(tmp_viewxdots));

  XmScaleGetValue(viewwin_opts.viewydots_scale,
      &(tmp_viewydots));

  if(success)
    {
    if(viewwin_opts.current.viewwindow != tmp_viewwindow             ||
       viewwin_opts.current.viewcrop != tmp_viewcrop                 ||
       viewwin_opts.current.viewreduction != tmp_viewreduction       ||
       viewwin_opts.current.finalaspectratio != tmp_finalaspectratio ||
       viewwin_opts.current.viewxdots != tmp_viewxdots               ||
       viewwin_opts.current.viewydots != tmp_viewydots)
      {
      changed = True;
      }

    viewwindow = viewwin_opts.current.viewwindow = tmp_viewwindow;
    viewcrop = viewwin_opts.current.viewcrop = tmp_viewcrop;
    viewreduction = viewwin_opts.current.viewreduction = tmp_viewreduction;
    finalaspectratio = viewwin_opts.current.finalaspectratio =
      tmp_finalaspectratio;
    viewxdots = viewwin_opts.current.viewxdots = tmp_viewxdots;
    viewydots = viewwin_opts.current.viewydots = tmp_viewydots;

    if(changed)
      {
      oldx = oldy = 0;
      resize_window(main_window, NULL, NULL);
      calc_status = IMG_PRMS_INIT;
      update_draw_menu(NEW_IMAGE);
      }

    XtUnmanageChild(window_options_dialog_shell);
    XtDestroyWidget(window_options_dialog_shell);
    window_options_dialog_shell = NULL;
    }
}

/*ARGSUSED*/
static void viewwin_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[64];

  if(viewwin_opts.dflt.viewwindow == 1)
    {
    XmToggleButtonSetState
      (viewwin_opts.preview_yes_toggle, True, False);
    XmToggleButtonSetState
      (viewwin_opts.preview_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState
      (viewwin_opts.preview_yes_toggle, False, False);
    XmToggleButtonSetState
      (viewwin_opts.preview_no_toggle, True, False);
    }

  if(viewwin_opts.dflt.viewcrop == 1)
    {
    XmToggleButtonSetState
      (viewwin_opts.crop_yes_toggle, True, False);
    XmToggleButtonSetState
      (viewwin_opts.crop_no_toggle, False, False);
    }
  else
    {
    XmToggleButtonSetState
      (viewwin_opts.crop_yes_toggle, False, False);
    XmToggleButtonSetState
      (viewwin_opts.crop_no_toggle, True, False);
    }

  sprintf((char *) &tempstr, "%f", viewwin_opts.dflt.viewreduction);
  XmTextFieldSetString(viewwin_opts.auto_reduction_textfield,
    (char *) &tempstr);

  sprintf((char *) &tempstr, "%f", viewwin_opts.dflt.finalaspectratio);
  XmTextFieldSetString(viewwin_opts.final_aspect_textfield,
    (char *) &tempstr);

  XmScaleSetValue(viewwin_opts.viewxdots_scale,
    (int) viewwin_opts.dflt.viewxdots);

  XmScaleSetValue(viewwin_opts.viewydots_scale,
    (int) viewwin_opts.dflt.viewydots);
}

/*ARGSUSED*/
static void viewwin_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(window_options_dialog_shell);
  XtDestroyWidget(window_options_dialog_shell);
  window_options_dialog_shell = NULL;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: window_options.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.9  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.8  1995/05/09  21:03:55  darryl
 * Fix bugs reported in beta test.
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
 * Revision 0.104  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.103  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
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
