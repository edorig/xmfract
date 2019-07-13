
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: window_size.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void resize_main_window_option_menu_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void resize_main_window_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void resize_main_window_help_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void resize_main_window_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void resize_main_window_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void resize_main_window_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget resize_main_window_shell = NULL;
static Widget resize_main_window_dialog = NULL;
 Widget explicit_x_size_scale = NULL;
 Widget explicit_y_size_scale = NULL;

/*ARGSUSED*/
void window_size_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Widget resize_main_window_option_menu;
  Widget resize_main_window_option_toc;
  Widget frame;
  Widget button;
  Arg args[MAX_ARGS];
  XmString str;
  XmString titlestr;
  XmString micro;
  XmString tiny;
  XmString small;
  XmString large;
  XmString distiny;
  XmString dissmall;
  XmString dislarge;
  XmString xtratiny;
  XmString visstr;
  XmString bigstr;
  XmString rootstr;
  char tempstr[256];
  Window no_win;
  int i_dont_care;
  unsigned int u_dont_care;
  unsigned int width_return;
  unsigned int height_return;
  int scale_width_val;
  int scale_height_val;
  int win_too_small;
  int win_too_big;
  int fracbase;
  int btnbase;
  int n;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  win_too_big = 0;
  win_too_small = 0;

  btnbase  = 20;
  fracbase = btnbase * 5 - 1;

  resize_main_window_shell = XmCreateDialogShell
    (main_window, "Resize Main Window", NULL, 0);

  XtAddCallback(resize_main_window_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(resize_main_window_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  str = XmStringCreateLocalized("Resize Main Window");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNdialogTitle, str); n++;
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;

  resize_main_window_dialog = XmCreateForm
    (resize_main_window_shell, "resize_main_window_dialog", args, n);

  XtAddCallback(resize_main_window_dialog, XmNmapCallback,
    map_dialog, NULL);

  XmStringFree(str);

  /*
   * Explicit size x pixels (width)
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Explicit size x pixels (width)");
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNmaximum, 2048); n++;
  XtSetArg(args[n], XmNminimum, window_presets.min_width); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;

  explicit_x_size_scale = XmCreateScale
    (resize_main_window_dialog, "explicit_x_size_scale", args, n);

  XtManageChild(explicit_x_size_scale);

  XmStringFree(str);

  /*
   * Explicit size y pixels (height)
   * (scale)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Explicit size y pixels (height)");
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, explicit_x_size_scale); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNmaximum, 2048); n++;
  XtSetArg(args[n], XmNminimum, window_presets.min_height); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;

  explicit_y_size_scale = XmCreateScale
    (resize_main_window_dialog, "explicit_y_size_scale", args, n);

  XtManageChild(explicit_y_size_scale);

  XmStringFree(str);

  /* preset values option menu */

  titlestr = XmStringCreateLocalized("Preset window size values");

  micro = XmStringCreateLocalized
    ("Normal (4:3) aspect ratio: Micro.....160 x 120");
  tiny = XmStringCreateLocalized
    ("Normal (4:3) aspect ratio: Tiny......320 x 240");
  small = XmStringCreateLocalized
    ("Normal (4:3) aspect ratio: Default...640 x 480");
  large = XmStringCreateLocalized
    ("Normal (4:3) aspect ratio: Large.....1024 x 768");

  sprintf((char *) &tempstr,
    "Root Window (%d:%d) aspect ratio: Tiny......%d x %d",
     window_presets.aspect_width, window_presets.aspect_height,
     window_presets.dis_tiny_width, window_presets.dis_tiny_height);
  distiny = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr,
    "Root Window (%d:%d) aspect ratio: Small.....%d x %d",
     window_presets.aspect_width, window_presets.aspect_height,
     window_presets.dis_small_width, window_presets.dis_small_height);
  dissmall = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr,
    "Root Window (%d:%d) aspect ratio: Large.....%d x %d",
     window_presets.aspect_width, window_presets.aspect_height,
     window_presets.dis_large_width, window_presets.dis_large_height);
  dislarge = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr,
    "Largest image visible on this display: %4d x %4d",
       window_presets.vis_max_width, window_presets.vis_max_height);
  visstr = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr,
    "Smallest image allowed by xmfract:     %4d x %4d",
       window_presets.min_width, window_presets.min_height);
  xtratiny = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr,
    "Root window background image size:     %4d x %4d",
       window_presets.display_width, window_presets.display_height);
  rootstr = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr,
    "Largest image allowed by xmfract:      %4d x %4d",
       window_presets.max_width, window_presets.max_height);
  bigstr = XmStringCreateLocalized((char *) &tempstr);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, explicit_y_size_scale); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  frame = XmCreateFrame
    (resize_main_window_dialog,
    "resize_main_window_option_menu_frame", args, n);

  resize_main_window_option_menu = XmVaCreateSimpleOptionMenu
    (frame, "resize_main_window_option_menu",
     titlestr, (KeySym)NULL, 2, resize_main_window_option_menu_cb,
     XmVaSEPARATOR,
     XmVaPUSHBUTTON, micro, NULL, NULL, NULL,
     XmVaPUSHBUTTON, tiny,  NULL, NULL, NULL,
     XmVaPUSHBUTTON, small, NULL, NULL, NULL,
     XmVaPUSHBUTTON, large, NULL, NULL, NULL,
     XmVaSEPARATOR,
     XmVaPUSHBUTTON, distiny,  NULL, NULL, NULL,
     XmVaPUSHBUTTON, dissmall, NULL, NULL, NULL,
     XmVaPUSHBUTTON, dislarge, NULL, NULL, NULL,
     XmVaSEPARATOR,
     XmVaPUSHBUTTON, xtratiny, NULL, NULL, NULL,
     XmVaPUSHBUTTON, visstr,   NULL, NULL, NULL,
     XmVaPUSHBUTTON, rootstr,  NULL, NULL, NULL,
     XmVaPUSHBUTTON, bigstr,   NULL, NULL, NULL,
     NULL);

  /* get the tear-off control */

  if((resize_main_window_option_toc = XmGetTearOffControl
    (resize_main_window_option_menu)) != NULL)
    {
    XtVaSetValues(resize_main_window_option_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  XtManageChild(resize_main_window_option_menu);
  XtManageChild(frame);

  XmStringFree(titlestr);
  XmStringFree(micro);
  XmStringFree(tiny);
  XmStringFree(small);
  XmStringFree(large);
  XmStringFree(distiny);
  XmStringFree(dissmall);
  XmStringFree(dislarge);
  XmStringFree(xtratiny);
  XmStringFree(visstr);
  XmStringFree(rootstr);
  XmStringFree(bigstr);

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
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (resize_main_window_dialog, "resize_main_window_help_button", args, n);

  XtAddCallback(button, XmNactivateCallback,
    resize_main_window_help_button_cb, NULL);

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
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (resize_main_window_dialog, "resize_main_window_reset_button", args, n);

  XtAddCallback(button, XmNactivateCallback,
    resize_main_window_reset_button_cb, NULL);

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
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (resize_main_window_dialog, "resize_main_window_dflt_button", args, n);

  XtAddCallback(button, XmNactivateCallback,
    resize_main_window_dflt_button_cb, NULL);

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
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (resize_main_window_dialog, "resize_main_window_cancel_button", args, n);

  XtAddCallback(button, XmNactivateCallback,
    resize_main_window_cancel_button_cb, NULL);

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
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (resize_main_window_dialog, "resize_main_window_done_button", args, n);

  XtAddCallback(button, XmNactivateCallback,
    resize_main_window_done_button_cb, NULL);

  XtManageChild(button);
  XmStringFree(str);

  XtVaSetValues(resize_main_window_dialog,
    XmNdefaultButton,
    button,
    NULL);

  XGetGeometry(display,XtWindow(canvas),
    &no_win,
    &(i_dont_care), &(i_dont_care),
    &(width_return), &(height_return),
    &(u_dont_care), &(u_dont_care));

  if(width_return < window_presets.min_width)
    {
    scale_width_val = window_presets.min_width;
    win_too_small = 1;
    }
  else if(width_return > window_presets.max_width)
    {
    scale_width_val = window_presets.max_width;
    win_too_big = 1;
    }
  else
    {
    scale_width_val = width_return;
    }

  if(height_return < window_presets.min_height)
    {
    scale_height_val = window_presets.min_height;
    win_too_small = 1;
    }
  else if(height_return > window_presets.max_height)
    {
    scale_height_val = window_presets.max_height;
    win_too_big = 1;
    }
  else
    {
    scale_height_val = height_return;
    }

  if(win_too_big || win_too_small)
    {
    char msg[128];
    char smallbig[64];
    char smallestbiggest[64];
    int wval, hval;
    if(win_too_small)
      {
      sprintf((char *) smallbig, "small");
      sprintf((char *) smallestbiggest, "smallest");
      wval = window_presets.min_width;
      hval = window_presets.min_height;
      }
    else
      {
      sprintf((char *) smallbig, "large");
      sprintf((char *) smallestbiggest, "largest");
      wval = window_presets.max_width;
      hval = window_presets.max_height;
      }
    sprintf((char *) &msg,
      "The main window is too %s.\n\nThe %s canvas allowed by xmfract is %d x %d.\n\nThe main window will be resized so that the canvas is %d x %d.\n",
     (char *) &smallbig, (char *) &smallestbiggest, wval, hval, wval, hval);

    confirm(main_window, (char *) &msg,
      OK_BTN, XmDIALOG_OK_BUTTON);

    XmScaleSetValue(explicit_x_size_scale, scale_width_val);

    XmScaleSetValue(explicit_y_size_scale, scale_height_val);

    resize_main_window_done_button_cb(main_window, NULL, NULL);
    }

  XmScaleSetValue(explicit_x_size_scale, scale_width_val);
  XmScaleSetValue(explicit_y_size_scale, scale_height_val);

  XtManageChild(resize_main_window_dialog);
  XtManageChild(resize_main_window_shell);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void resize_main_window_option_menu_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int changed;
  int item_no;
  char tempstr[4];

  sprintf((char *) &tempstr, "%d", (char *)client_data);
  item_no = atoi((char *) &tempstr);
  changed = 0;

  switch(item_no)
    {
    case 0:
      XmScaleSetValue(explicit_x_size_scale, 160);
      XmScaleSetValue(explicit_y_size_scale, 120);
      changed = 1;
    break;

    case 1:
      XmScaleSetValue(explicit_x_size_scale, 320);
      XmScaleSetValue(explicit_y_size_scale, 240);
      changed = 1;
    break;

    case 2:
      XmScaleSetValue(explicit_x_size_scale, 640);
      XmScaleSetValue(explicit_y_size_scale, 480);
      changed = 1;
    break;

    case 3:
      XmScaleSetValue(explicit_x_size_scale, 1024);
      XmScaleSetValue(explicit_y_size_scale, 768);
      changed = 1;
    break;

    case 4:
      XmScaleSetValue(explicit_x_size_scale, window_presets.dis_tiny_width);
      XmScaleSetValue(explicit_y_size_scale, window_presets.dis_tiny_height);
      changed = 1;
    break;

    case 5:
      XmScaleSetValue(explicit_x_size_scale, window_presets.dis_small_width);
      XmScaleSetValue(explicit_y_size_scale, window_presets.dis_small_height);
      changed = 1;
    break;

    case 6:
      XmScaleSetValue(explicit_x_size_scale, window_presets.dis_large_width);
      XmScaleSetValue(explicit_y_size_scale, window_presets.dis_large_height);
      changed = 1;
    break;

    case 7:
      XmScaleSetValue(explicit_x_size_scale, window_presets.min_width);
      XmScaleSetValue(explicit_y_size_scale, window_presets.min_height);
      changed = 1;
    break;

    case 8:
      XmScaleSetValue(explicit_x_size_scale, window_presets.vis_max_width);
      XmScaleSetValue(explicit_y_size_scale, window_presets.vis_max_height);
      changed = 1;
    break;

    case 9:
      XmScaleSetValue(explicit_x_size_scale, window_presets.display_width);
      XmScaleSetValue(explicit_y_size_scale, window_presets.display_height);
      changed = 1;
    break;

    case 10:
      XmScaleSetValue(explicit_x_size_scale, window_presets.max_width);
      XmScaleSetValue(explicit_y_size_scale, window_presets.max_height);
      changed = 1;
    break;

    default:
      XmScaleSetValue(explicit_x_size_scale, 640);
      XmScaleSetValue(explicit_y_size_scale, 480);
      changed = 1;
    break;
    }

  if(changed)
    resize_main_window_done_button_cb(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void resize_main_window_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Window no_win;
  int i_dont_care;
  unsigned int u_dont_care;
  unsigned int width_return;
  unsigned int height_return;

  XGetGeometry(display,XtWindow(canvas),
    &no_win,
    &(i_dont_care), &(i_dont_care),
    &(width_return), &(height_return),
    &(u_dont_care), &(u_dont_care));

  XmScaleSetValue(explicit_x_size_scale,
    (int)(width_return));

  XmScaleSetValue(explicit_y_size_scale,
    (int)(height_return));
}

/*ARGSUSED*/
static void resize_main_window_help_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPXMFRACT;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void resize_main_window_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(explicit_x_size_scale, 640);
  XmScaleSetValue(explicit_y_size_scale, 480);
}

/*ARGSUSED*/
static void resize_main_window_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(resize_main_window_dialog));
}

/*ARGSUSED*/
static void resize_main_window_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int ret;
  int new_x_pos;
  int new_y_pos;
  XWindowChanges values;
  char msg[512];

  XmScaleGetValue(explicit_x_size_scale, &(window_presets.new_width));
  XmScaleGetValue(explicit_y_size_scale, &(window_presets.new_height));

  window_presets.oversize = 0;

  if((window_presets.new_width > window_presets.vis_max_width) ||
     (window_presets.new_height > window_presets.vis_max_height))
    {
    if(verbose)
      {
      sprintf((char *) &msg,
"New window size may be too large for this screen.\nMaximum values for a fully visible image are:\n\nWidth:  %4d\nHeight: %4d\n\nThese values are based on the current\nmenu bar height of %d pixels.\n\nResize anyway?\n",
      window_presets.vis_max_width, window_presets.vis_max_height,
      window_presets.height_addition);
      ret = confirm(main_window, (char *) &msg,
        OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
      }
    else
      ret = 1;

    if(ret != 1)
      return;
    else
      window_presets.oversize = 1;
    }

  values.width = window_presets.new_width + window_presets.width_addition;
  values.height = window_presets.new_height + window_presets.height_addition;

  if(!(window_presets.oversize))
    {
    new_x_pos = window_presets.vis_max_width - values.width;
    new_y_pos = window_presets.vis_max_height - values.height;

    new_x_pos /= 2;
    new_y_pos /= 2;
    }
  else
    {
    new_x_pos = 0;
    new_y_pos = 0;
    }

  values.x = new_x_pos;
  values.y = new_y_pos;

  XGetGeometry(display,XtWindow(canvas),
    &(window_presets.rootw),
    &(window_presets.width_addition), &(window_presets.height_addition),
    &(window_presets.width_return), &(window_presets.height_return),
    &(window_presets.border_width_return), &(window_presets.depth_return));

  XtPopdown(XtParent(resize_main_window_dialog));

  XReconfigureWMWindow(display, window, screen_number,
    CWX|CWY|CWWidth|CWHeight, &values);

  XtAppAddTimeOut(app, 250, (XtTimerCallbackProc) check_geometry, NULL);

  calc_status = IMG_PRMS_CHGD;
  update_draw_menu(NEW_IMAGE);
  toggle_zoom_grab(ZOOM_OFF);
}

/*ARGSUSED*/
void check_geometry(XtPointer client_data, XtIntervalId Id)
{
  XWindowChanges values;
  int new_x_pos;
  int new_y_pos;
  static int size_retries = 0;

  XGetGeometry(display,XtWindow(canvas),
    &(window_presets.rootw),
    &(window_presets.new_width_addition), &(window_presets.new_height_addition),
    &(window_presets.new_width_return), &(window_presets.new_height_return),
    &(window_presets.border_width_return), &(window_presets.depth_return));

  window_presets.height_addition = window_presets.new_height_addition;
  window_presets.width_addition = window_presets.new_width_addition;

  if(window_presets.new_width_return != window_presets.new_width ||
     window_presets.new_height_return != window_presets.new_height)
    {
    values.width = window_presets.new_width +
      window_presets.new_width_addition;
    values.height = window_presets.new_height +
      window_presets.new_height_addition;

    if(!(window_presets.oversize))
      {
      new_x_pos = window_presets.vis_max_width - values.width;
      new_y_pos = window_presets.vis_max_height - values.height;

      new_x_pos /= 2;
      new_y_pos /= 2;
      }
    else
      {
      new_x_pos = 0;
      new_y_pos = 0;
      }

    values.x = new_x_pos;
    values.y = new_y_pos;

    if(++size_retries > 10)
      {
      char msg[256];
      sprintf((char *) &msg, "I've tried 10 times to accomodate your\nrequested window size of %d x %d.\n\nThe closest I could come was %d x %d.\n\nThis server cannot resize the main window\nto the requested size. I'm giving up now,\nand the program will exit.\n\nBetter luck next time.\n",
        window_presets.new_width, window_presets.new_height,
        window_presets.new_width_return, window_presets.new_height_return);
      fatal(main_window, (char *) &msg);
      exit(1);
      }

    XReconfigureWMWindow(display, window, screen_number,
    CWX|CWY|CWWidth|CWHeight, &values);

    XtAppAddTimeOut(app, 250, (XtTimerCallbackProc) check_geometry, NULL);
    }
  else
    {
    size_retries = 0;
    }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: window_size.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.16  1995/06/13  17:51:35  darryl
 * Add window resize mode function.
 *
 * Revision 1.15  1995/06/02  06:09:35  darryl
 * Change default button in some dialogs from Cancel to Ok.
 *
 * Revision 1.14  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.13  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.12  1995/04/27  17:53:47  darryl
 * Change basename to nbasename, change %x to %d in client_data
 * translations, delete unused header file inclusions.
 *
 * Revision 1.11  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
 *
 * Revision 1.10  1995/04/19  19:36:07  darryl
 * Fix bugs in parameter forwarding options.
 *
 * Revision 1.9  1995/04/18  15:59:59  darryl
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
 * Revision 1.8  1995/03/31  06:49:22  darryl
 * Fix window size preset values.
 *
 * Revision 1.7  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.6  1995/03/22  18:08:22  darryl
 * Prevent crashes when recursive calls are made to dialog
 * creation routines, enlarge file name and path variable
 * sizes to at least 256 bytes, don't pop down the parent
 * of a warning or caution dialog in the case of a list
 * or file selection dialog mis-match.
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
 * Revision 0.301  1995/02/04  19:44:04  darryl
 * Automatically resize if the user selects an option from
 * the pop-up menu, rather than having to select an option,
 * then press the 'Done' button in the dialog. This feature
 * should probably be user-configurable.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
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
 * Revision 0.106  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.105  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.104  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.103  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.102  1994/11/21  22:15:32  darryl
 * Enforce an 80 x 60 minimum window size.
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
