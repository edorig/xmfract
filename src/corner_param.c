
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: corner_param.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void corners_type_parm_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void corners_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void corners_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void corners_defaults_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void corners_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void corners_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void corners_cmag_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget corner_param_dialog_shell = NULL;
static Widget corner_param_dialog = NULL;

struct crnr_name
{
  char *base;
  char *label;
  double value;
  int cmag;
  int transp3d;
  char valstr[24];
};

static struct crnr_name crnr_names[] =
{
  {"center_x_", "Center X Coordinate", 0, 1, 0, ""},
  {"center_y_", "Center Y Coordinate", 0, 1, 0, ""},
  {"magnification_", "Magnification", 0, 1, 0, ""},
  {"top_left_x_", "Top Left X Coordinate", 0, 0, 0, ""},
  {"top_left_y_", "Top Left Y Coordinate", 0, 0, 0, ""},
  {"top_left_z_", "Top Left Z Coordinate", 0, 0, 1, ""},
  {"bottom_right_x_", "Bottom Right X Coordinate", 0, 0, 0, ""},
  {"bottom_right_y_", "Bottom Right Y Coordinate", 0, 0, 0, ""},
  {"bottom_right_z_", "Bottom Right Z Coordinate", 0, 0, 1, ""},
  {"time_step_from_", "Time Step From:", 0, 0, 1, ""},
  {"time_step_to_", "Time Step To:", 0, 0, 1, ""},
  {"bottom_left_x_", "Bottom Left X Coordinate", 0, 0, 0, ""},
  {"bottom_left_y_", "Bottom Left Y Coordinate", 0, 0, 0, ""},
  { NULL, NULL, 0, 0, 0, "" }
};

/*ARGSUSED*/
void create_corner_param_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Arg args[MAX_ARGS];
  Widget frame;
  Widget textfield;
  Widget label;
  Widget button;
  Widget which;
  Widget previous;
  XmString str;
  char tempstr[256];
  double Xctr;
  double Yctr;
  double Mag;
  int cmag;
  int transp3d;
  int fracbase;
  int btnbase;
  int i, n;

  btnbase = 20;
  fracbase = btnbase * 6 - 1;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  usemag = (short) atoi(client_data);

  if(!corner_param_dialog_shell)
    {
    corner_param_dialog_shell = XmCreateDialogShell
      (main_window, "corner_values", NULL, 0);

    XtAddCallback(corner_param_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(corner_param_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    corner_param_dialog = XmCreateForm
      (corner_param_dialog_shell, "corner_param_dialog", args, n);

    XtAddCallback(corner_param_dialog, XmNmapCallback,
      map_dialog, NULL);

    for(i = 0; crnr_names[i].base != NULL; i++)
      {
      n = 0;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
      XtSetArg(args[n], XmNchildHorizontalAlignment,
        XmALIGNMENT_BEGINNING); n++;
      sprintf((char *) &tempstr, "%s%s", crnr_names[i].base, "frame");
      frame = XmCreateFrame
        (corner_param_dialog, (char *) &tempstr, args, n);

      n = 0;
      str = XmStringCreateLocalized(crnr_names[i].label);
      XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
      XtSetArg(args[n], XmNchildVerticalAlignment,
        XmALIGNMENT_WIDGET_BOTTOM); n++;
      XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
      XtSetArg(args[n], XmNlabelString, str); n++;

      sprintf((char *) &tempstr, "%s%s", crnr_names[i].base, "label");
      label = XmCreateLabel
        (frame, (char *) &tempstr, args, n);

      XtManageChild(label);
      XmStringFree(str);

      n = 0;
      XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
      XtSetArg(args[n], XmNshowValue, True); n++;

      sprintf((char *) &tempstr, "%s%s", crnr_names[i].base, "textfield");
      textfield = XmCreateTextField
        (frame, (char *) &tempstr, args, n);

      XtManageChild(textfield);
      }

    n = 0;
    str = XmStringCreateLocalized("Toggle to Corner Coordinates Mode");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    button = XmCreatePushButton
      (corner_param_dialog, "corners_cmag_btn", args, n);

    XtAddCallback(button, XmNactivateCallback, corners_cmag_btn_cb, NULL);

    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Toggle to Center Magnification Mode");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    button = XmCreatePushButton
      (corner_param_dialog, "corners_crnr_btn", args, n);

    XtAddCallback(button, XmNactivateCallback, corners_cmag_btn_cb, NULL);

    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Return to Parameters");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    button = XmCreatePushButton
      (corner_param_dialog, "corners_type_parm_btn", args, n);

    XtAddCallback(button, XmNactivateCallback, corners_type_parm_btn_cb, NULL);

    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton(corner_param_dialog,
      "corners_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback, corners_help_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Apply");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton(corner_param_dialog,
      "corners_apply_btn", args, n);

    XtAddCallback(button, XmNactivateCallback, corners_done_btn_cb,
      (XtPointer) "0");

    XtManageChild(button);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Reset");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton(corner_param_dialog,
      "corners_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      corners_reset_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Defaults");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton(corner_param_dialog,
      "corners_defaults_btn", args, n);

    XtAddCallback(button, XmNactivateCallback, corners_defaults_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Cancel");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton(corner_param_dialog,
      "corners_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      corners_cancel_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Done");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (corner_param_dialog, "corners_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      corners_done_btn_cb, (XtPointer) "1");

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(corner_param_dialog,
      XmNdefaultButton,
      button,
      NULL);
    }

  transp3d = (Transparent3D && fractalspecific[fractype].orbitcalc == Formula);

  cmag = (!transp3d && usemag && cvtcentermag(&Xctr, &Yctr, &Mag));

  corner_val.current.usemag = usemag;
  corner_val.current.xxmin = xxmin;
  corner_val.current.xxmax = xxmax;
  corner_val.current.yymin = yymin;
  corner_val.current.yymax = yymax;
  corner_val.current.xx3rd = xx3rd;
  corner_val.current.yy3rd = yy3rd;
  corner_val.current.zzmin = zzmin;
  corner_val.current.zzmax = zzmax;
  corner_val.current.ttmin = ttmin;
  corner_val.current.ttmax = ttmax;
  corner_val.current.Xctr = Xctr;
  corner_val.current.Yctr = Yctr;
  corner_val.current.Mag = Mag;

  corner_val.new.usemag = usemag;
  corner_val.new.xxmin = xxmin;
  corner_val.new.xxmax = xxmax;
  corner_val.new.yymin = yymin;
  corner_val.new.yymax = yymax;
  corner_val.new.xx3rd = xx3rd;
  corner_val.new.yy3rd = yy3rd;
  corner_val.new.zzmin = zzmin;
  corner_val.new.zzmax = zzmax;
  corner_val.new.ttmin = ttmin;
  corner_val.new.ttmax = ttmax;
  corner_val.new.Xctr = Xctr;
  corner_val.new.Yctr = Yctr;
  corner_val.new.Mag = Mag;

  for(i = 0; crnr_names[i].base != NULL; i++)
    {
    sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "frame");
    which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
    if(which != NULL)
      {
      if(XtIsManaged(which))
        XtUnmanageChild(which);
      }
    }

  sprintf((char *) &tempstr, "*%s", "corners_cmag_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_crnr_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_type_parm_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_reset_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_defaults_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_done_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_help_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_done_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_apply_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
  if(which != NULL)
    XtUnmanageChild(which);

  i = -1;
  crnr_names[++i].value = corner_val.current.Xctr;
  crnr_names[++i].value = corner_val.current.Yctr;
  crnr_names[++i].value = corner_val.current.Mag;
  crnr_names[++i].value = corner_val.current.xxmin;
  crnr_names[++i].value = corner_val.current.yymax;
  crnr_names[++i].value = corner_val.current.zzmin;
  crnr_names[++i].value = corner_val.current.xxmax;
  crnr_names[++i].value = corner_val.current.yymin;
  crnr_names[++i].value = corner_val.current.zzmin;
  crnr_names[++i].value = corner_val.current.ttmin;
  crnr_names[++i].value = corner_val.current.ttmax;
  crnr_names[++i].value = corner_val.current.xx3rd;
  crnr_names[++i].value = corner_val.current.yy3rd;

  for(i = 0, which = NULL, previous = NULL; crnr_names[i].base != NULL; i++)
    {
    sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "frame");
    which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
    if(which != NULL)
      {
      sprintf((char *) &(crnr_names[i].valstr), "%+.17g", crnr_names[i].value);
      if(cmag && crnr_names[i].cmag)
        {
        if(previous == NULL)
          {
          XtVaSetValues(which,
            XmNtopAttachment, XmATTACH_POSITION,
            XmNtopPosition, 1,
            NULL);
          }
        else
          {
          XtVaSetValues(which,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, previous,
            NULL);
          }
        XtManageChild(which);
        sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "textfield");
        textfield = XtNameToWidget(which, (char *) &tempstr);
        XmTextFieldSetString(textfield, (char *) &(crnr_names[i].valstr));
        previous = which;
        }
      else if(!cmag && !crnr_names[i].cmag && !crnr_names[i].transp3d)
        {
        if(previous == NULL)
          {
          XtVaSetValues(which,
            XmNtopAttachment, XmATTACH_POSITION,
            XmNtopPosition, 1,
            NULL);
          }
        else
          {
          XtVaSetValues(which,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, previous,
            NULL);
          }
        XtManageChild(which);
        sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "textfield");
        textfield = XtNameToWidget(which, (char *) &tempstr);
        XmTextFieldSetString(textfield, (char *) &(crnr_names[i].valstr));
        previous = which;
        }
      else if(transp3d && crnr_names[i].transp3d)
        {
        if(previous == NULL)
          {
          XtVaSetValues(which,
            XmNtopAttachment, XmATTACH_POSITION,
            XmNtopPosition, 1,
            NULL);
          }
        else
          {
          XtVaSetValues(which,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, previous,
            NULL);
          }
        XtManageChild(which);
        sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "textfield");
        textfield = XtNameToWidget(which, (char *) &tempstr);
        XmTextFieldSetString(textfield, (char *) &(crnr_names[i].valstr));
        previous = which;
        }
      }
    }

  if(!transp3d && cvtcentermag(&Xctr, &Yctr, &Mag))
    {
    if(usemag)
      {
      sprintf((char *) &tempstr, "*%s", "corners_cmag_btn");
      which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
      }
    else
      {
      sprintf((char *) &tempstr, "*%s", "corners_crnr_btn");
      which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
      }

    XtVaSetValues(which,
      XmNtopAttachment, XmATTACH_WIDGET,
      XmNtopWidget, previous,
      NULL);
    XtManageChild(which);
    previous = which;
    }

  sprintf((char *) &tempstr, "*%s", "corners_type_parm_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);

  XtVaSetValues(which,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, previous,
    NULL);
  XtManageChild(which);

  previous = which;

  sprintf((char *) &tempstr, "*%s", "corners_reset_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);

  XtVaSetValues(which,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, previous,
    XmNbottomAttachment, XmATTACH_FORM,
    NULL);
  XtManageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_defaults_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);

  XtVaSetValues(which,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, previous,
    XmNbottomAttachment, XmATTACH_FORM,
    NULL);
  XtManageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_done_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);

  XtVaSetValues(which,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, previous,
    XmNbottomAttachment, XmATTACH_FORM,
    NULL);
  XtManageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_help_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);

  XtVaSetValues(which,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, previous,
    XmNbottomAttachment, XmATTACH_FORM,
    NULL);
  XtManageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_done_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);

  XtVaSetValues(which,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, previous,
    XmNbottomAttachment, XmATTACH_FORM,
    NULL);
  XtManageChild(which);

  sprintf((char *) &tempstr, "*%s", "corners_apply_btn");
  which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);

  XtVaSetValues(which,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, previous,
    XmNbottomAttachment, XmATTACH_FORM,
    NULL);
  XtManageChild(which);

  if(usemag)
    str = XmStringCreateLocalized("Center Magnification");
  else
    str = XmStringCreateLocalized("Corner Coordinates");

  XtVaSetValues(corner_param_dialog,
    XmNdialogTitle, str, NULL);

  XmStringFree(str);

  XtManageChild(corner_param_dialog);
  XtManageChild(corner_param_dialog_shell);
  XtPopup(corner_param_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void corners_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(corner_param_dialog);
  XtUnmanageChild(corner_param_dialog_shell);
}

/*ARGSUSED*/
static void corners_type_parm_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  corners_done_btn_cb(main_window, (XtPointer) "1", NULL);
  type_specific_params(main_window);
}

/*ARGSUSED*/
static void corners_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  char tempstr[64];
  Widget which;
  Widget textfield;

  corner_val.new = corner_val.current;

  i = -1;
  crnr_names[++i].value = corner_val.new.Xctr;
  crnr_names[++i].value = corner_val.new.Yctr;
  crnr_names[++i].value = corner_val.new.Mag;
  crnr_names[++i].value = corner_val.new.xxmin;
  crnr_names[++i].value = corner_val.new.yymax;
  crnr_names[++i].value = corner_val.new.zzmin;
  crnr_names[++i].value = corner_val.new.xxmax;
  crnr_names[++i].value = corner_val.new.yymin;
  crnr_names[++i].value = corner_val.new.zzmin;
  crnr_names[++i].value = corner_val.new.ttmin;
  crnr_names[++i].value = corner_val.new.ttmax;
  crnr_names[++i].value = corner_val.new.xx3rd;
  crnr_names[++i].value = corner_val.new.yy3rd;

  for(i = 0; crnr_names[i].base != NULL; i++)
    {
    sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "frame");
    which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
    if(which != NULL)
      {
      sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "textfield");
      textfield = XtNameToWidget(which, (char *) &tempstr);
      sprintf((char *) &(crnr_names[i].valstr), "%+.17g", crnr_names[i].value);
      if(XtIsManaged(textfield))
        XmTextFieldSetString(textfield, (char *) &(crnr_names[i].valstr));
      }
    }
}

/*ARGSUSED*/
static void corners_defaults_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  char tempstr[64];
  Widget which;
  Widget textfield;

  corner_val.new = corner_val.dflt;

  i = -1;
  crnr_names[++i].value = corner_val.new.Xctr;
  crnr_names[++i].value = corner_val.new.Yctr;
  crnr_names[++i].value = corner_val.new.Mag;
  crnr_names[++i].value = corner_val.new.xxmin;
  crnr_names[++i].value = corner_val.new.yymax;
  crnr_names[++i].value = corner_val.new.zzmin;
  crnr_names[++i].value = corner_val.new.xxmax;
  crnr_names[++i].value = corner_val.new.yymin;
  crnr_names[++i].value = corner_val.new.zzmin;
  crnr_names[++i].value = corner_val.new.ttmin;
  crnr_names[++i].value = corner_val.new.ttmax;
  crnr_names[++i].value = corner_val.new.xx3rd;
  crnr_names[++i].value = corner_val.new.yy3rd;

  for(i = 0; crnr_names[i].base != NULL; i++)
    {
    sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "frame");
    which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
    if(which != NULL)
      {
      sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "textfield");
      textfield = XtNameToWidget(which, (char *) &tempstr);
      sprintf((char *) &(crnr_names[i].valstr), "%+.17g", crnr_names[i].value);
      if(XtIsManaged(textfield))
        XmTextFieldSetString(textfield, (char *) &(crnr_names[i].valstr));
      }
    }
}

/*ARGSUSED*/
static void corners_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  int changed;
  double validnum;
  char tempstr[64];
  char msg[256];
  char *invalid;
  char *txt;
  Widget which;
  Widget textfield;

  int pop_down = atoi((char *)client_data);

  for(i = 0; crnr_names[i].base != NULL; i++)
    {
    sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "frame");
    which = XtNameToWidget(corner_param_dialog_shell, (char *) &tempstr);
    if(which != NULL)
      {
      sprintf((char *) &tempstr, "*%s%s", crnr_names[i].base, "textfield");
      textfield = XtNameToWidget(which, (char *) &tempstr);
      sprintf((char *) &(crnr_names[i].valstr), "%+.17g", crnr_names[i].value);
      if(XtIsManaged(textfield))
        {
        txt = XmTextFieldGetString(textfield);
        validnum = crnr_names[i].value = strtod(txt, &invalid);
        if(invalid && *invalid)
          {
          sprintf((char *) &msg,
            "Cannot resolve floating point number \"%s\".", txt);
          caution(main_window, (char *) &msg, NULL);
          return;
          }
        }
      }
    }

  i = -1;
  if(usemag)
    {
    corner_val.new.Xctr = crnr_names[++i].value;
    corner_val.new.Yctr = crnr_names[++i].value;
    corner_val.new.Mag = crnr_names[++i].value;
    }
  else
    i += 3;
  corner_val.new.xxmin = crnr_names[++i].value;
  corner_val.new.yymax = crnr_names[++i].value;
  corner_val.new.zzmin = crnr_names[++i].value;
  corner_val.new.xxmax = crnr_names[++i].value;
  corner_val.new.yymin = crnr_names[++i].value;
  corner_val.new.zzmin = crnr_names[++i].value;
  corner_val.new.ttmin = crnr_names[++i].value;
  corner_val.new.ttmax = crnr_names[++i].value;
  corner_val.new.xx3rd = crnr_names[++i].value;
  corner_val.new.yy3rd = crnr_names[++i].value;

  if(corner_val.current.Xctr != corner_val.new.Xctr   ||
     corner_val.current.Yctr != corner_val.new.Yctr   ||
     corner_val.current.Mag != corner_val.new.Mag     ||
     corner_val.current.xxmin != corner_val.new.xxmin ||
     corner_val.current.yymax != corner_val.new.yymax ||
     corner_val.current.zzmin != corner_val.new.zzmin ||
     corner_val.current.xxmax != corner_val.new.xxmax ||
     corner_val.current.yymin != corner_val.new.yymin ||
     corner_val.current.zzmin != corner_val.new.zzmin ||
     corner_val.current.ttmin != corner_val.new.ttmin ||
     corner_val.current.ttmax != corner_val.new.ttmax ||
     corner_val.current.xx3rd != corner_val.new.xx3rd ||
     corner_val.current.yy3rd != corner_val.new.yy3rd)
    {
    changed = 1;
    }
  else
    {
    changed = 0;
    }

  corner_val.current.Xctr = corner_val.new.Xctr;
  corner_val.current.Yctr = corner_val.new.Yctr;
  corner_val.current.Mag = corner_val.new.Mag;
  corner_val.current.xxmin = corner_val.new.xxmin;
  corner_val.current.yymax = corner_val.new.yymax;
  corner_val.current.zzmin = corner_val.new.zzmin;
  corner_val.current.xxmax = corner_val.new.xxmax;
  corner_val.current.yymin = corner_val.new.yymin;
  corner_val.current.zzmin = corner_val.new.zzmin;
  corner_val.current.ttmin = corner_val.new.ttmin;
  corner_val.current.ttmax = corner_val.new.ttmax;
  corner_val.current.xx3rd = corner_val.new.xx3rd;
  corner_val.current.yy3rd = corner_val.new.yy3rd;

  xxmin = corner_val.current.xxmin;
  xxmax = corner_val.current.xxmax;
  yymin = corner_val.current.yymin;
  yymax = corner_val.current.yymax;
  xx3rd = corner_val.current.xx3rd;
  yy3rd = corner_val.current.yy3rd;
  zzmin = corner_val.current.zzmin;
  zzmax = corner_val.current.zzmax;
  ttmin = corner_val.current.ttmin;
  ttmax = corner_val.current.ttmax;

  if(changed)
    {
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    toggle_zoom_grab(ZOOM_OFF);
    }

  if(pop_down == 1)
    {
    XtUnmanageChild(corner_param_dialog);
    XtUnmanageChild(corner_param_dialog_shell);
    }
}

/*ARGSUSED*/
static void corners_cmag_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[64];

  XtUnmanageChild(corner_param_dialog);
  XtUnmanageChild(corner_param_dialog_shell);

  switch( (int) usemag)
    {
    case 0:
      usemag = 1;
    break;

    case 1:
      usemag = 0;
    break;
    }

  sprintf((char *) &tempstr, "%d", usemag);

  create_corner_param_dialog(main_window, (XtPointer) tempstr, NULL);
}

/*ARGSUSED*/
static void corners_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  read_help(main_window, NULL, NULL);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: corner_param.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.16  1995/05/15  19:30:32  darryl
 * Delete over-the-shoulder textfield edit checks.
 *
 * Revision 1.15  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.14  1995/05/09  21:03:55  darryl
 * Fix bugs reported in beta test.
 *
 * Revision 1.13  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.12  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.11  1995/04/18  15:59:59  darryl
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
 * Revision 1.10  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.9  1995/03/28  18:11:40  darryl
 * Fix trig function handling. Enhance center-mag button
 * management. Aesthetic changes in the parameter dialog.
 * Add frames around groups of scale widgets. Fix a bogus
 * message about color cell allocation if sharecolor.
 *
 * Revision 1.8  1995/03/25  20:27:49  darryl
 * Add cancel buttons for several dialogs. Make the corner
 * values dialog title dynamic, reflecting the current mode.
 *
 * Revision 1.7  1995/03/23  20:06:30  darryl
 * Modify help, allow full-path names in the ini file,
 * fix a bug where default trig paramaters were not
 * set correctly for some types.
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
 * Revision 0.105  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.104  1994/11/25  17:11:55  darryl
 * Fix bug in toggle between corner and center-mag modes.
 *
 * Revision 0.103  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.102  1994/11/21  18:21:16  darryl
 * Change instances of XtIsRealized to XtIsManaged.
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
