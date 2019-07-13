
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: file_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static Widget saved_cmd_fsb = NULL;
static Widget load_image_fsb = NULL;
static Widget load_transform_fsb = NULL;
static Widget load_map_fsb = NULL;
static Widget save_image_fsb = NULL;
static Widget save_par_fsb = NULL;
static Widget save_palette_fsb = NULL;

/*ARGSUSED*/
void create_file_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget file_button;
  Widget file_menu_toc;
  Widget file_menu_pane;
  Widget file_menu_btn;
  XmString str;
  XmString accel;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  file_menu_pane = XmCreatePulldownMenu(w, "file_menu_pane", args, n);

  /* get the tear-off control */

  if((file_menu_toc = XmGetTearOffControl
    (file_menu_pane)) != NULL)
    {
    XtVaSetValues(file_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'File' cascade button */

  n = 0;
  str = XmStringCreateLocalized("File");
  XtSetArg(args[n], XmNsubMenuId, file_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  file_button = XmCreateCascadeButton(w, "file_button", args, n);

  XtManageChild(file_button);
  XmStringFree(str);

  /*
   * create pushbuttons and/or cascade pushbuttons for the 'File' menu
   *
   * Load saved parameter set...
   * Save parameter set to file...
   * Load image from file...
   * Save image to file...
   * Quit xmfract
   */

  n = 0;
  str = XmStringCreateLocalized("Load saved parameter set...");
  accel = XmStringCreateLocalized("[ @ ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "Shift<Key>2:"); n++;

  file_menu_btn = XmCreatePushButton
    (file_menu_pane, "load_saved_par_btn", args, n);

  XtAddCallback(file_menu_btn, XmNactivateCallback,
    saved_cmd_cb, (XtPointer) NULL);

  XtManageChild(file_menu_btn);
  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Save parameter set to file...");
  accel = XmStringCreateLocalized("[ b ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>b:"); n++;

  file_menu_btn = XmCreatePushButton
    (file_menu_pane, "save_par_btn", args, n);

  XtAddCallback(file_menu_btn, XmNactivateCallback,
    save_par_cb, (XtPointer) NULL);

  XtManageChild(file_menu_btn);
  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Load image from file...");
  accel = XmStringCreateLocalized("[ r ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>r:"); n++;

  file_menu_btn = XmCreatePushButton
    (file_menu_pane, "load_image_btn", args, n);

  XtAddCallback(file_menu_btn, XmNactivateCallback,
    load_image_cb, (XtPointer) NULL);

  XtManageChild(file_menu_btn);
  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Save image to file...");
  accel = XmStringCreateLocalized("[ s ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>s:"); n++;

  file_menu_btn = XmCreatePushButton(file_menu_pane,
    "save_image_btn", args, n);

  XtAddCallback(file_menu_btn, XmNactivateCallback,
    save_image_cb, (XtPointer) NULL);

  XtManageChild(file_menu_btn);
  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Quit xmfract");
  accel = XmStringCreateLocalized("[ q ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>q:"); n++;

  file_menu_btn = XmCreatePushButton
    (file_menu_pane, "quit_btn", args, n);

  XtAddCallback(file_menu_btn, XmNactivateCallback,
    quit_prog_cb, (XtPointer) NULL);

  XtManageChild(file_menu_btn);
  XmStringFree(str);
  XmStringFree(accel);
}

/*ARGSUSED*/
void saved_cmd_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(!saved_cmd_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr;
    int n = 0;

    str[0] = XmStringCreateLocalized("Load Saved Command Set");
    str[1] = XmStringCreateLocalized("*.par");
    dirstr = XmStringCreateLocalized(fractdir);
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    saved_cmd_fsb = XmCreateFileSelectionDialog(main_window,
      "saved_cmd_fsb", args, n);
    XtAddCallback(saved_cmd_fsb, XmNokCallback,
      get_fname_cb, NULL);
    XtAddCallback(saved_cmd_fsb, XmNcancelCallback,
      saved_cmd_cancel_cb, NULL);
    XtAddCallback(saved_cmd_fsb, XmNhelpCallback, read_help, NULL);
    XtAddCallback(saved_cmd_fsb, XmNnoMatchCallback, fsb_no_match, NULL);
    XtAddCallback(saved_cmd_fsb, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(saved_cmd_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(saved_cmd_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  helpmode = HELPPARMFILE;
  XtManageChild(saved_cmd_fsb);
  XtPopup(XtParent(saved_cmd_fsb), XtGrabNone);
}

/*ARGSUSED*/
void get_fname_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;
  int read = 0;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);
  XtPopdown(XtParent(saved_cmd_fsb));
  select_cmd_entry(main_window, filename, read);
}

/*ARGSUSED*/
void load_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(!load_image_fsb)
    {
    XmString str[2];
    XmString dirstr;
    Arg args[MAX_ARGS];
    int n = 0;

    str[0] = XmStringCreateLocalized("Load Saved Image");
    str[1] = XmStringCreateLocalized("*.gif");
    dirstr = XmStringCreateLocalized(fractdir);
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    load_image_fsb = XmCreateFileSelectionDialog(main_window,
      "load_image_fsb", args, n);

    XtAddCallback(load_image_fsb, XmNokCallback,
      get_image_file_cb, NULL);
    XtAddCallback(load_image_fsb, XmNcancelCallback,
      load_image_cancel_cb, NULL);
    XtAddCallback(load_image_fsb, XmNhelpCallback, read_help, NULL);
    XtAddCallback(load_image_fsb, XmNnoMatchCallback, fsb_no_match, NULL);
    XtAddCallback(load_image_fsb, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(load_image_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(load_image_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  helpmode = HELPSAVEREST;
  XtManageChild(load_image_fsb);
  XtPopup(XtParent(load_image_fsb), XtGrabNone);
}

/*ARGSUSED*/
void get_image_file_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;
  int ret;
  int err;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(load_image_fsb));

  sprintf(readname, "%s", filename);

  display3d = 0; /* forget 3d */
  overlay3d = 0; /* forget overlays */
  if(viewwindow)
    {
    viewwindow = 0; /* forget preview */
    viewcrop = 0;
    viewreduction = 4.2;
    finalaspectratio = screenaspect;
    viewxdots = viewydots = 0;
    viewwin_opts.current.viewwindow = viewwindow;
    viewwin_opts.current.viewcrop = viewcrop;
    viewwin_opts.current.viewreduction = viewreduction;
    viewwin_opts.current.finalaspectratio = finalaspectratio;
    viewwin_opts.current.viewxdots = viewxdots;
    viewwin_opts.current.viewydots = viewydots;
    sxoffs = syoffs = 0;
    xdots = sxdots;
    ydots = sydots;
    dx_size = xdots - 1;
    dy_size = ydots - 1;
    }

  outln = out_line;

  ret = read_overlay();

  if((ret == 0) && ((filexdots != xdots) || (fileydots != ydots)))
    {
    if(verbose)
      err = image_load_error();
    else
      err = 1;

    if(err != 1)
      return;
    else
      ret = resize_for_new_image(filexdots, fileydots);
    }

  XSync(display, False);

  if(ret == 0)
    XtAppAddTimeOut(app, 250, (XtTimerCallbackProc) delay_display_file, NULL);
}

/*ARGSUSED*/
void delay_display_file( XtPointer client_data, XtIntervalId Id)
{
  int old_refresh_rate = refresh_rate;
  refresh_rate = 100;
  display_file();
  refresh_rate = old_refresh_rate;
}

/*ARGSUSED*/
void load_image_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(load_image_fsb));
}

/*ARGSUSED*/
void load_transform_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(!load_transform_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr;
    int n = 0;

    dirstr = XmStringCreateLocalized(fractdir);

    str[0] = XmStringCreateLocalized("Load File for Transform");
    str[1] = XmStringCreateLocalized("*.gif");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    load_transform_fsb = XmCreateFileSelectionDialog(main_window,
      "load_transform_fsb", args, n);

    XtAddCallback(load_transform_fsb, XmNokCallback,
      get_transform_file_cb, NULL);
    XtAddCallback(load_transform_fsb, XmNcancelCallback,
      load_transform_cancel_cb, NULL);
    XtAddCallback(load_transform_fsb, XmNhelpCallback, read_help, NULL);
    XtAddCallback(load_transform_fsb, XmNnoMatchCallback, fsb_no_match, NULL);
    XtAddCallback(load_transform_fsb, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(load_transform_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(load_transform_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  helpmode = HELP3D;
  XtManageChild(load_transform_fsb);
  XtPopup(XtParent(load_transform_fsb), XtGrabNone);
}

/*ARGSUSED*/
void get_transform_file_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(load_transform_fsb));

  sprintf(readname, "%s", filename);

  display3d = 1;
  outln = line3d;
  if((read_overlay()) == 0)
    get_3d_params(main_window, NULL, NULL);
  else
    return;
}

/*ARGSUSED*/
void load_map_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(!load_map_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr;
    int n = 0;

    dirstr = XmStringCreateLocalized(fractdir);

    str[0] = XmStringCreateLocalized("Load Color Map File");
    str[1] = XmStringCreateLocalized("*.map");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    load_map_fsb = XmCreateFileSelectionDialog(main_window,
      "load_map_fsb", args, n);

    XtAddCallback(load_map_fsb, XmNokCallback,
      get_mapname_cb, NULL);
    XtAddCallback(load_map_fsb, XmNcancelCallback,
      load_map_cancel_cb, NULL);
    XtAddCallback(load_map_fsb, XmNnoMatchCallback, fsb_no_match, NULL);
    XtAddCallback(load_map_fsb, XmNhelpCallback, read_help, NULL);
    XtAddCallback(load_map_fsb, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(load_map_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(load_map_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  helpmode = HELPCOLORMAP;
  XtManageChild(load_map_fsb);
  XtPopup(XtParent(load_map_fsb), XtGrabNone);
}

/*ARGSUSED*/
void get_mapname_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(load_map_fsb));

  load_color_map(main_window, filename);
}

/*ARGSUSED*/
void save_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(calc_status == IMG_PRMS_INIT)
    {
    char msg[128];
    sprintf((char *) &msg, "No image to save!");
    inform(main_window, (char *) &msg, NULL);
    helpmode = HELPSAVEREST;
    return;
    }

  if(calc_status == IMG_PRMS_CHGD)
    {
    int ret = 0;
    char msg[128];
    helpmode = HELPSAVEREST;
    sprintf((char *) &msg,
    "Parameters for this image have changed since it was created.\nPress OK to continue saving, or Cancel to cancel the save.");
    ret = confirm(main_window, (char *) &msg,
      OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
    if(ret != 1)
      return;
    }

  if(!save_image_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr;
    int n = 0;

    dirstr = XmStringCreateLocalized(fractdir);

    str[0] = XmStringCreateLocalized("Save Image");
    str[1] = XmStringCreateLocalized("*.gif");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    save_image_fsb = XmCreateFileSelectionDialog(main_window,
      "save_image_fsb", args, n);

    XtAddCallback(save_image_fsb, XmNokCallback,
      get_save_name_cb, NULL);
    XtAddCallback(save_image_fsb, XmNcancelCallback,
      save_image_cancel_cb, NULL);
    XtAddCallback(save_image_fsb, XmNhelpCallback, read_help, NULL);
    XtAddCallback(save_image_fsb, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(save_image_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(save_image_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  helpmode = HELPSAVEREST;
  XtManageChild(save_image_fsb);
  XtPopup(XtParent(save_image_fsb), XtGrabNone);
}

/*ARGSUSED*/
void get_save_name_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;
  struct stat st;
  char msg[256];
  int old_overwrite_flag;
  int ret;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(save_image_fsb));

  display3d = 0; /* forget 3d */
  overlay3d = 0; /* forget overlays */

  old_overwrite_flag = overwrite_flag;

  ret = stat(filename, &st);

  if(ret == 0)
    {
    ret = (st.st_mode & S_IFMT) == S_IFREG;
    if(!ret)
      {
      sprintf((char *) &msg, "Cannot overwrite \"%s\".", filename);
      ret = confirm(main_window, (char *) &msg,
        OK_BTN | HELP_BTN, XmDIALOG_OK_BUTTON);
      XtPopdown(XtParent(save_image_fsb));
      return;
      }
    if(verbose || (overwrite_flag == 0))
      {
      sprintf((char *) &msg, "Overwrite \"%s\"?", filename);
      ret = confirm(main_window, (char *) &msg,
        OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
      }
    else
      ret = 1;
    }
  else
    ret = 1;

  if(ret == 1)
    {
    overwrite_flag = 1;
    savetodisk(filename);
    overwrite_flag = old_overwrite_flag;
    }
}

/*ARGSUSED*/
void save_par_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(!save_par_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr;
    int n = 0;

    dirstr = XmStringCreateLocalized(fractdir);

    str[0] = XmStringCreateLocalized("Save Parameters");
    str[1] = XmStringCreateLocalized("*.par");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, True); n++;
    XtSetArg(args[n], XmNmustMatch, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    save_par_fsb = XmCreateFileSelectionDialog(main_window,
      "save_par_fsb", args, n);

    XtAddCallback(save_par_fsb, XmNokCallback,
      get_save_parameter_name_cb, NULL);
    XtAddCallback(save_par_fsb, XmNnoMatchCallback,
      get_save_parameter_err_cb, NULL);
    XtAddCallback(save_par_fsb, XmNcancelCallback,
      save_par_cancel_cb, NULL);
    XtAddCallback(save_par_fsb, XmNhelpCallback, read_help, NULL);
    XtAddCallback(save_par_fsb, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(save_par_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(save_par_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  helpmode = HELPPARMFILE;
  XtManageChild(save_par_fsb);
  XtPopup(XtParent(save_par_fsb), XtGrabNone);
}

/*ARGSUSED*/
void get_save_parameter_name_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  struct stat st;
  char msg[256];
  char *filename;
  int write = 1;
  int ret;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(save_par_fsb));

  sprintf((char *) &PAR_name, filename);

  ret = stat(filename, &st);

  if(ret == 0)
    {
    ret = (st.st_mode & S_IFMT) == S_IFREG;
    if(!ret)
      {
      sprintf((char *) &msg, "\"%s\"%s", filename, "is not a regular file.");
      ret = confirm(main_window, (char *) &msg,
        OK_BTN, XmDIALOG_OK_BUTTON);
      XtPopdown(XtParent(save_par_fsb));
      return;
      }
    }

  select_cmd_entry(main_window, (char *) &PAR_name, write);
}

/*ARGSUSED*/
void get_save_parameter_err_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char msg[256];
  char *filename;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(save_par_fsb));

  sprintf((char *) &msg, "\"%s\"%s", filename, "is not a parameter file.");

  caution(main_window, (char *) &msg, NULL);
}

/*ARGSUSED*/
void save_palette_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(!save_palette_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr;
    int n = 0;

    dirstr = XmStringCreateLocalized(fractdir);

    str[0] = XmStringCreateLocalized("Save Palette");
    str[1] = XmStringCreateLocalized("*.map");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    save_palette_fsb = XmCreateFileSelectionDialog(main_window,
      "save_palette_fsb", args, n);

    XtAddCallback(save_palette_fsb, XmNokCallback,
      get_save_palette_name_cb, NULL);
    XtAddCallback(save_palette_fsb, XmNcancelCallback,
      save_palette_cancel_cb, NULL);
    XtAddCallback(save_palette_fsb, XmNhelpCallback, read_help, NULL);
    XtAddCallback(save_palette_fsb, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(save_palette_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(save_palette_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  helpmode = HELPCOLORMAP;
  XtManageChild(save_palette_fsb);
  XtPopup(XtParent(save_palette_fsb), XtGrabNone);
}

/*ARGSUSED*/
void get_save_palette_name_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;
  struct stat st;
  char msg[256];
  int old_overwrite_flag;
  int ret;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(save_palette_fsb));

  old_overwrite_flag = overwrite_flag;

  ret = stat(filename, &st);

  if(ret == 0)
    {
    ret = (st.st_mode & S_IFMT) == S_IFREG;
    if(!ret)
      {
      sprintf((char *) &msg, "Cannot overwrite \"%s\".", filename);
      ret = confirm(main_window, (char *) &msg,
        OK_BTN, XmDIALOG_OK_BUTTON);
      XtPopdown(XtParent(save_image_fsb));
      return;
      }
    if(verbose || (overwrite_flag == 0))
      {
      sprintf((char *) &msg, "Overwrite \"%s\"?", filename);
      ret = confirm(main_window, (char *) &msg,
        OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
      }
    else
      ret = 1;
    }
  else
    ret = 1;

  if(ret == 1)
    {
    overwrite_flag = 1;
    save_palette(filename);
    overwrite_flag = old_overwrite_flag;
    }
}

/*ARGSUSED*/
void quit_prog_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char msg[256];
  int ret;

  if(escape_exit != 0)
    ret = 1;
  else
    {
    sprintf((char *) &msg, "Really quit xmfract?");
    ret = confirm(main_window, (char *) &msg,
      OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
    }

  if(ret == 1)
    exit(0);
}

/*ARGSUSED*/
void editpal_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XSetWindowAttributes winattrs;
  int backing_store;
  char msg[128];
  int ret;

  if(editpal_cursor != 0)
    return;

  if(colors < 16)
    {
    sprintf((char *) &msg, "Xmfract has currently allocated %d colors.\n\nUsing the palette editor on this palette which\ncontains less than 16 colors is not recommended.\n\nContinue?\n", colors);
    ret = confirm(main_window, (char *) &msg,
      OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
    if(ret != 1)
      return;
    }
  else if(sharecolor)
    {
    sprintf((char *) &msg, "Xmfract has been invoked with the \"-share\"\ncommand-line option. Colors that are shared\nare not privately allocated by this program\nand cannot be edited.\n");
    inform(main_window, (char *) &msg, NULL);
    return;
    }
  else if(!xgc)
    {
    sprintf((char *) &msg, "Xmfract cannot switch graphics contexts.\nPerhaps no color map has been loaded?\n");
    inform(main_window, (char *) &msg, NULL);
    return;
    }

  create_editpal_options_dialog(main_window, NULL, NULL);

  winattrs.background_pixel = BlackPixelOfScreen(screen);
  winattrs.bit_gravity = StaticGravity;
  backing_store = DoesBackingStore(screen);

  if (backing_store)
    winattrs.backing_store = Always;
  else
    winattrs.backing_store = NotUseful;

  winattrs.background_pixel = pixtab[0];

  XChangeWindowAttributes(display, XtWindow(paledit.edit_palette_dialog),
        CWBackPixel | CWBitGravity | CWBackingStore, &winattrs);

  editpal_cursor = 1;
}

/*ARGSUSED*/
void load_map_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*ARGSUSED*/
void saved_cmd_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*ARGSUSED*/
void save_image_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*ARGSUSED*/
void save_par_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*ARGSUSED*/
void save_palette_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*ARGSUSED*/
void load_transform_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: file_menu.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.12  1995/06/01  22:17:03  darryl
 * Fix problem where preview mode was not reset when
 * restoring a saved image from a file.
 *
 * Revision 1.11  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
 *
 * Revision 1.10  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.9  1995/04/25  04:06:37  darryl
 * Fix bug in cancel callbacks.
 *
 * Revision 1.8  1995/04/18  15:59:59  darryl
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
 * Revision 1.7  1995/04/11  15:37:45  darryl
 * Remove hard-coded tearOffModel resources to allow the
 * resource file to over-ride the fallback resource.
 *
 * Revision 1.6  1995/03/31  20:51:53  darryl
 * Fix 3d transform callback.
 *
 * Revision 1.5  1995/03/27  22:54:08  darryl
 * Add a safety check so that if the xgc is null the color
 * editor cannot be invoked. Change a button name for uniqueness.
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
 * Revision 0.302  1995/01/30  19:54:51  darryl
 * Enhance information messages concerning shared color.
 *
 * Revision 0.301  1995/01/04  17:38:04  darryl
 * Make references to the program name lower case.
 *
 * Revision 0.300  1994/12/20  17:41:14  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.201  1994/12/16  18:58:09  darryl
 * Fix typo in widget name.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.108  1994/11/30  21:09:19  darryl
 * Set the XmNseparatorType resource for TearOffControls.
 *
 * Revision 0.107  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.106  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.105  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.104  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.103  1994/11/24  07:27:15  darryl
 * Initialize helpmode.
 *
 * Revision 0.102  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.101  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
