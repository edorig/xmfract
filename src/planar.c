
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: planar.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void three_d_planar_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_planar_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_planar_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_planar_dflt_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void three_d_planar_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget three_d_planar_dialog_shell = NULL;
static Widget three_d_planar_dialog = NULL;

/*ARGSUSED*/
void select_3d_planar(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Arg args[MAX_ARGS];
  XmString str;
  Widget form;
  Widget frame;
  Widget label;
  Widget scale;
  Widget button;
  Widget left;
  Widget right;
  int last_pos;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 5 - 1;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  three_d_planar_dialog_shell = XmCreateDialogShell
    (main_window, "three_d_planar_dialog_shell", NULL, 0);

  XtAddCallback(three_d_planar_dialog_shell, XmNpopupCallback,
    map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(three_d_planar_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  if(SPHERE)
    str = XmStringCreateLocalized("Three Dimensional Sphere Parameters");
  else
    str = XmStringCreateLocalized("Three Dimensional Planar Parameters");
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogTitle, str); n++;

  three_d_planar_dialog = XmCreateForm
    (three_d_planar_dialog_shell, "three_d_planar_dialog", args, n);

  XtAddCallback(three_d_planar_dialog, XmNmapCallback,
    map_dialog, NULL);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

  frame = XmCreateFrame
    (three_d_planar_dialog, "planar_dialog_scale_frame", args, n);

  n = 0;
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;

  form = XmCreateForm(frame, "planar_dialog_scale_form", args, n);

  if(SPHERE)
    {
    str = XmStringCreateLocalized
      ("Sphere is on its side; North pole to right");
    }
  else
    {
    str = XmStringCreateLocalized
      ("Pre-rotation X axis is screen top; Y axis is left side");
    }

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 0); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  label = XmCreateLabel(form, "three_d_planar_sphere_hdr_1", args, n);

  XtManageChild(label);
  XmStringFree(str);

  if(SPHERE)
    {
    str = XmStringCreateLocalized
      ("Long. 180 is top, 0 is bottom; Lat. -90 is left, 90 is right");
    }
  else
    {
    str = XmStringCreateLocalized
      ("Pre-rotation Z axis is coming at you out of the screen!");
    }

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, label); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 0); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  label = XmCreateLabel(form, "three_d_planar_sphere_hdr_2", args, n);

  XtManageChild(label);
  XmStringFree(str);

  /*
   * if last_pos == 1 then widget goes on the left
   * and the top widget attachment = left
   * otherwise the widget goes on the right
   * and the top widget attachment = right
   */

  left = right = label;
  last_pos = 1;

  /*
   * if(SPHERE)
   *   Longitude start (degrees)
   *   Longitude stop  (degrees)
   *   Latitude start  (degrees)
   *   Latitude stop   (degrees)
   *   Radius scaling factor in pct
   * else
   *   if(!RAY)
   *     X-axis rotation in degrees
   *     Y-axis rotation in degrees
   *     Z-axis rotation in degrees
   *   X-axis scaling factor in pct
   *   Y-axis scaling factor in pct
   * Surface Roughness scaling factor in pct
   * Water Level (minimum color value)
   * if(!RAY)
   *   Perspective distance
   *   X shift with perspective (positive = right)
   *   Y shift with perspective (positive = up)
   *   Non-perspective X adjust (positive = right)
   *   Non-perspective Y adjust (positive = up)
   *   First transparent color
   *   Last transparent color
   * Randomize Colors (0 = no)
   */

  if(SPHERE)
    {
    /*
     * Longitude start (degrees)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Longitude start (degrees)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 359); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_long_start_scale", args, n);

    three_d_opts.planar_long_start_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Longitude stop  (degrees)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Longitude stop (degrees)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 180); n++;
    XtSetArg(args[n], XmNminimum, -180); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_long_stop_scale", args, n);

    three_d_opts.planar_long_stop_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Latitude start  (degrees)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Latitude start (degrees)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 180); n++;
    XtSetArg(args[n], XmNminimum, -180); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_lat_start_scale", args, n);

    three_d_opts.planar_lat_start_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Latitude stop  (degrees)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Latitude stop (degrees)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 180); n++;
    XtSetArg(args[n], XmNminimum, -180); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_lat_stop_scale", args, n);

    three_d_opts.planar_lat_stop_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Radius scaling factor in pct
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Radius scaling factor in pct");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 1000); n++;
    XtSetArg(args[n], XmNminimum, -1000); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_radius_pct_scale", args, n);

    three_d_opts.planar_radius_pct_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }
    }
  else /* !SPHERE */
    {
    if(!RAY)
      {
      /*
       * X-axis rotation in degrees
       * (scale)
       */

      n = 0;
      str = XmStringCreateLocalized("X-axis rotation in degrees");
      XtSetArg(args[n], XmNtitleString, str); n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
      if(last_pos == 1)
        {
        XtSetArg(args[n], XmNtopWidget, left); n++;
        XtSetArg(args[n], XmNleftPosition, 1); n++;
        XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, right); n++;
        XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
        XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
        }
      XtSetArg(args[n], XmNmaximum, 359); n++;
      XtSetArg(args[n], XmNminimum, -359); n++;
      XtSetArg(args[n], XmNscaleMultiple, 1); n++;
      XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
      XtSetArg(args[n], XmNshowValue, True); n++;

      scale = XmCreateScale
        (form, "planar_x_rot_scale", args, n);

      three_d_opts.planar_x_rot_scale = scale;

      XtManageChild(scale);
      XmStringFree(str);

      if(last_pos == 1)
        {
        left = scale;
        last_pos = 0;
        }
      else
        {
        right = scale;
        last_pos = 1;
        }

      /*
       * Y-axis rotation in degrees
       * (scale)
       */

      n = 0;
      str = XmStringCreateLocalized("Y-axis rotation in degrees");
      XtSetArg(args[n], XmNtitleString, str); n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
      if(last_pos == 1)
        {
        XtSetArg(args[n], XmNtopWidget, left); n++;
        XtSetArg(args[n], XmNleftPosition, 1); n++;
        XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, right); n++;
        XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
        XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
        }
      XtSetArg(args[n], XmNmaximum, 359); n++;
      XtSetArg(args[n], XmNminimum, -359); n++;
      XtSetArg(args[n], XmNscaleMultiple, 1); n++;
      XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
      XtSetArg(args[n], XmNshowValue, True); n++;

      scale = XmCreateScale
        (form, "planar_y_rot_scale", args, n);

      three_d_opts.planar_y_rot_scale = scale;

      XtManageChild(scale);
      XmStringFree(str);

      if(last_pos == 1)
        {
        left = scale;
        last_pos = 0;
        }
      else
        {
        right = scale;
        last_pos = 1;
        }

      /*
       * Z-axis rotation in degrees
       * (scale)
       */

      n = 0;
      str = XmStringCreateLocalized("Z-axis rotation in degrees");
      XtSetArg(args[n], XmNtitleString, str); n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
      if(last_pos == 1)
        {
        XtSetArg(args[n], XmNtopWidget, left); n++;
        XtSetArg(args[n], XmNleftPosition, 1); n++;
        XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, right); n++;
        XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
        XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
        }
      XtSetArg(args[n], XmNmaximum, 359); n++;
      XtSetArg(args[n], XmNminimum, -359); n++;
      XtSetArg(args[n], XmNscaleMultiple, 1); n++;
      XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
      XtSetArg(args[n], XmNshowValue, True); n++;

      scale = XmCreateScale
        (form, "planar_z_rot_scale", args, n);

      three_d_opts.planar_z_rot_scale = scale;

      XtManageChild(scale);
      XmStringFree(str);

      if(last_pos == 1)
        {
        left = scale;
        last_pos = 0;
        }
      else
        {
        right = scale;
        last_pos = 1;
        }
      }

    /*
     * X-axis scaling factor in pct
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("X-axis scaling factor in pct");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 1000); n++;
    XtSetArg(args[n], XmNminimum, -1000); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_x_scale_scale", args, n);

    three_d_opts.planar_x_scale_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Y-axis scaling factor in pct
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Y-axis scaling factor in pct");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 1000); n++;
    XtSetArg(args[n], XmNminimum, -1000); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_y_scale_scale", args, n);

    three_d_opts.planar_y_scale_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }
    }

  /*
   * Surface Roughness scaling factor in pct
   * (scale)
   */

  n = 0;
  str = XmStringCreateLocalized("Surface Roughness scaling factor in pct");
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  if(last_pos == 1)
    {
    XtSetArg(args[n], XmNtopWidget, left); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    }
  else
    {
    XtSetArg(args[n], XmNtopWidget, right); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    }
  XtSetArg(args[n], XmNmaximum, 1000); n++;
  XtSetArg(args[n], XmNminimum, -1000); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;

  scale = XmCreateScale
    (form, "planar_rough_scale", args, n);

  three_d_opts.planar_rough_scale = scale;

  XtManageChild(scale);
  XmStringFree(str);

  if(last_pos == 1)
    {
    left = scale;
    last_pos = 0;
    }
  else
    {
    right = scale;
    last_pos = 1;
    }

  n = 0;
  str = XmStringCreateLocalized("Water Level (min. color value)");
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  if(last_pos == 1)
    {
    XtSetArg(args[n], XmNtopWidget, left); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    }
  else
    {
    XtSetArg(args[n], XmNtopWidget, right); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    }
  XtSetArg(args[n], XmNmaximum, 255); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;

  scale = XmCreateScale
    (form, "planar_water_scale", args, n);

  three_d_opts.planar_water_scale = scale;

  XtManageChild(scale);
  XmStringFree(str);

  if(last_pos == 1)
    {
    left = scale;
    last_pos = 0;
    }
  else
    {
    right = scale;
    last_pos = 1;
    }

  if(!RAY)
    {
    /*
     * Perspective distance
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Perspective distance");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 999); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_perspective_scale", args, n);

    three_d_opts.planar_perspective_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * X shift with perspective (positive = right)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("X shift with perspective (positive = right)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, -100); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_x_shift_scale", args, n);

    three_d_opts.planar_x_shift_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Y shift with perspective (positive = up)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Y shift with perspective (positive = right)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, -100); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_y_shift_scale", args, n);

    three_d_opts.planar_y_shift_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Non-perspective X adjust (positive = right)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Non-perspective X adjust (positive = right)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, -100); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_x_adjust_scale", args, n);

    three_d_opts.planar_x_adjust_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Non-perspective Y adjust (positive = up)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Non-perspective Y adjust (positive = up)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, -100); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_y_adjust_scale", args, n);

    three_d_opts.planar_y_adjust_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * First transparent color
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("First transparent color");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_first_color_scale", args, n);

    three_d_opts.planar_first_color_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }

    /*
     * Last transparent color
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Last transparent color");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    if(last_pos == 1)
      {
      XtSetArg(args[n], XmNtopWidget, left); n++;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, right); n++;
      XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      }
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    scale = XmCreateScale
      (form, "planar_last_color_scale", args, n);

    three_d_opts.planar_last_color_scale = scale;

    XtManageChild(scale);
    XmStringFree(str);

    if(last_pos == 1)
      {
      left = scale;
      last_pos = 0;
      }
    else
      {
      right = scale;
      last_pos = 1;
      }
    }

  /*
   * Randomize Colors (0 = no)
   * (scale)
   */

  n = 0;
  str = XmStringCreateLocalized("Randomize Colors (0 = no)");
  XtSetArg(args[n], XmNtitleString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  if(last_pos == 1)
    {
    XtSetArg(args[n], XmNtopWidget, left); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    }
  else
    {
    XtSetArg(args[n], XmNtopWidget, right); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    }
  XtSetArg(args[n], XmNmaximum, 7); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNscaleMultiple, 1); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;

  scale = XmCreateScale
    (form, "planar_random_color_scale", args, n);

  three_d_opts.planar_random_color_scale = scale;

  XtManageChild(scale);
  XmStringFree(str);

  XtManageChild(form);
  XtManageChild(frame);

  /*
   * Help
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Help");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNleftPosition, 0); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_planar_dialog, "three_d_planar_help_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_planar_help_btn_cb, NULL );

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
  XtSetArg(args[n], XmNleftPosition, btnbase); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_planar_dialog, "three_d_planar_reset_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_planar_reset_btn_cb, NULL);

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
  XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_planar_dialog, "three_d_planar_dialog_dflt_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_planar_dflt_btn_cb, NULL);

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
  XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_planar_dialog, "three_d_planar_dialog_cancel_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_planar_cancel_btn_cb, NULL);

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
  XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;

  button = XmCreatePushButton
    (three_d_planar_dialog, "three_d_planar_done_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    three_d_planar_done_btn_cb, NULL);

  XtManageChild(button);
  XmStringFree(str);

  XtVaSetValues(three_d_planar_dialog,
    XmNdefaultButton,
    button,
    NULL);

  three_d_opts.current.phi1 = PHI1;
  three_d_opts.current.phi2 = PHI2;
  three_d_opts.current.theta1 = THETA1;
  three_d_opts.current.theta2 = THETA2;
  three_d_opts.current.radius = RADIUS;
  three_d_opts.current.xrot = XROT;
  three_d_opts.current.yrot = YROT;
  three_d_opts.current.zrot = ZROT;
  three_d_opts.current.xscale = XSCALE;
  three_d_opts.current.yscale = YSCALE;
  three_d_opts.current.rough = ROUGH;
  three_d_opts.current.waterline = WATERLINE;
  three_d_opts.current.zviewer = ZVIEWER;
  three_d_opts.current.xshift = XSHIFT;
  three_d_opts.current.yshift = YSHIFT;
  three_d_opts.current.xtrans = xtrans;
  three_d_opts.current.ytrans = ytrans;
  three_d_opts.current.transparent[0] = transparent[0];
  three_d_opts.current.transparent[1] = transparent[1];
  three_d_opts.current.rndcolor = RANDOMIZE;

  if(SPHERE)
    {
    XmScaleSetValue(three_d_opts.planar_long_start_scale,
      three_d_opts.current.phi1);
    XmScaleSetValue(three_d_opts.planar_long_stop_scale,
      three_d_opts.current.phi2);
    XmScaleSetValue(three_d_opts.planar_lat_start_scale,
      three_d_opts.current.theta1);
    XmScaleSetValue(three_d_opts.planar_lat_stop_scale,
      three_d_opts.current.theta2);
    XmScaleSetValue(three_d_opts.planar_radius_pct_scale,
      three_d_opts.current.radius);
    }
  else
    {
    if(!RAY)
      {
      XmScaleSetValue(three_d_opts.planar_x_rot_scale,
        three_d_opts.current.xrot);
      XmScaleSetValue(three_d_opts.planar_y_rot_scale,
        three_d_opts.current.yrot);
      XmScaleSetValue(three_d_opts.planar_z_rot_scale,
        three_d_opts.current.zrot);
      }
    XmScaleSetValue(three_d_opts.planar_x_scale_scale,
      three_d_opts.current.xscale);
    XmScaleSetValue(three_d_opts.planar_y_scale_scale,
      three_d_opts.current.yscale);
    }
  XmScaleSetValue(three_d_opts.planar_rough_scale,
    three_d_opts.current.rough);
  XmScaleSetValue(three_d_opts.planar_water_scale,
    three_d_opts.current.waterline);
  if(!RAY)
    {
    XmScaleSetValue(three_d_opts.planar_perspective_scale,
      three_d_opts.current.zviewer);
    XmScaleSetValue(three_d_opts.planar_x_shift_scale,
      three_d_opts.current.xshift);
    XmScaleSetValue(three_d_opts.planar_y_shift_scale,
      three_d_opts.current.yshift);
    XmScaleSetValue(three_d_opts.planar_x_adjust_scale,
      three_d_opts.current.xtrans);
    XmScaleSetValue(three_d_opts.planar_y_adjust_scale,
      three_d_opts.current.ytrans);
    XmScaleSetValue(three_d_opts.planar_first_color_scale,
      three_d_opts.current.transparent[0]);
    XmScaleSetValue(three_d_opts.planar_last_color_scale,
      three_d_opts.current.transparent[1]);
    }
  XmScaleSetValue(three_d_opts.planar_random_color_scale,
    three_d_opts.current.rndcolor);

  XtManageChild(three_d_planar_dialog);
  XtManageChild(three_d_planar_dialog_shell);
  XtPopup(three_d_planar_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void three_d_planar_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.phi1 = PHI1;
  three_d_opts.current.phi2 = PHI2;
  three_d_opts.current.theta1 = THETA1;
  three_d_opts.current.theta2 = THETA2;
  three_d_opts.current.radius = RADIUS;
  three_d_opts.current.xrot = XROT;
  three_d_opts.current.yrot = YROT;
  three_d_opts.current.zrot = ZROT;
  three_d_opts.current.xscale = XSCALE;
  three_d_opts.current.yscale = YSCALE;
  three_d_opts.current.rough = ROUGH;
  three_d_opts.current.waterline = WATERLINE;
  three_d_opts.current.zviewer = ZVIEWER;
  three_d_opts.current.xshift = XSHIFT;
  three_d_opts.current.yshift = YSHIFT;
  three_d_opts.current.xtrans = xtrans;
  three_d_opts.current.ytrans = ytrans;
  three_d_opts.current.transparent[0] = transparent[0];
  three_d_opts.current.transparent[1] = transparent[1];
  three_d_opts.current.rndcolor = RANDOMIZE;

  if(SPHERE)
    {
    XmScaleSetValue(three_d_opts.planar_long_start_scale,
      three_d_opts.current.phi1);

    XmScaleSetValue(three_d_opts.planar_long_stop_scale,
      three_d_opts.current.phi2);

    XmScaleSetValue(three_d_opts.planar_lat_start_scale,
      three_d_opts.current.theta1);

    XmScaleSetValue(three_d_opts.planar_lat_stop_scale,
      three_d_opts.current.theta2);

    XmScaleSetValue(three_d_opts.planar_radius_pct_scale,
      three_d_opts.current.radius);
    }
  else
    {
    if(!RAY)
      {
      XmScaleSetValue(three_d_opts.planar_x_rot_scale,
        three_d_opts.current.xrot);

      XmScaleSetValue(three_d_opts.planar_y_rot_scale,
        three_d_opts.current.yrot);

      XmScaleSetValue(three_d_opts.planar_z_rot_scale,
        three_d_opts.current.zrot);
      }

    XmScaleSetValue(three_d_opts.planar_x_scale_scale,
      three_d_opts.current.xscale);

    XmScaleSetValue(three_d_opts.planar_y_scale_scale,
      three_d_opts.current.yscale);

    XmScaleSetValue(three_d_opts.planar_rough_scale,
      three_d_opts.current.rough);
    }

  XmScaleSetValue(three_d_opts.planar_water_scale,
    three_d_opts.current.waterline);

  XmScaleSetValue(three_d_opts.planar_perspective_scale,
    three_d_opts.current.zviewer);

  if(!RAY)
    {
    XmScaleSetValue(three_d_opts.planar_x_shift_scale,
      three_d_opts.current.xshift);

    XmScaleSetValue(three_d_opts.planar_y_shift_scale,
      three_d_opts.current.yshift);

    XmScaleSetValue(three_d_opts.planar_x_adjust_scale,
      three_d_opts.current.xtrans);

    XmScaleSetValue(three_d_opts.planar_y_adjust_scale,
      three_d_opts.current.ytrans);

    XmScaleSetValue(three_d_opts.planar_first_color_scale,
      three_d_opts.current.transparent[0]);

    XmScaleSetValue(three_d_opts.planar_last_color_scale,
      three_d_opts.current.transparent[1]);
    }

  XmScaleSetValue(three_d_opts.planar_random_color_scale,
    three_d_opts.current.rndcolor);
}

/*ARGSUSED*/
static void three_d_planar_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  initvars_fractal();
  XtPopdown(three_d_planar_dialog_shell);
  XtDestroyWidget(three_d_planar_dialog_shell);
  three_d_planar_dialog_shell = NULL;
}

/*ARGSUSED*/
static void three_d_planar_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELP3DPARMS;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void three_d_planar_dflt_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  three_d_opts.current.phi1 = three_d_opts.dflt.phi1;
  three_d_opts.current.phi2 = three_d_opts.dflt.phi2;
  three_d_opts.current.theta1 = three_d_opts.dflt.theta1;
  three_d_opts.current.theta2 = three_d_opts.dflt.theta2;
  three_d_opts.current.radius = three_d_opts.dflt.radius;
  three_d_opts.current.xrot = three_d_opts.dflt.xrot;
  three_d_opts.current.yrot = three_d_opts.dflt.yrot;
  three_d_opts.current.zrot = three_d_opts.dflt.zrot;
  three_d_opts.current.xscale = three_d_opts.dflt.xscale;
  three_d_opts.current.yscale = three_d_opts.dflt.yscale;
  three_d_opts.current.rough = three_d_opts.dflt.rough;
  three_d_opts.current.waterline = three_d_opts.dflt.waterline;
  three_d_opts.current.zviewer = three_d_opts.dflt.zviewer;
  three_d_opts.current.xshift = three_d_opts.dflt.xshift;
  three_d_opts.current.yshift = three_d_opts.dflt.yshift;
  three_d_opts.current.xtrans = three_d_opts.dflt.xtrans;
  three_d_opts.current.ytrans = three_d_opts.dflt.ytrans;
  three_d_opts.current.transparent[0] = three_d_opts.dflt.transparent[0];
  three_d_opts.current.transparent[1] = three_d_opts.dflt.transparent[1];
  three_d_opts.current.rndcolor = three_d_opts.dflt.rndcolor;

  if(SPHERE)
    {
    XmScaleSetValue(three_d_opts.planar_long_start_scale,
      three_d_opts.current.phi1);

    XmScaleSetValue(three_d_opts.planar_long_stop_scale,
      three_d_opts.current.phi2);

    XmScaleSetValue(three_d_opts.planar_lat_start_scale,
      three_d_opts.current.theta1);

    XmScaleSetValue(three_d_opts.planar_lat_stop_scale,
      three_d_opts.current.theta2);

    XmScaleSetValue(three_d_opts.planar_radius_pct_scale,
      three_d_opts.current.radius);
    }
  else
    {
    if(!RAY)
      {
      XmScaleSetValue(three_d_opts.planar_x_rot_scale,
        three_d_opts.current.xrot);

      XmScaleSetValue(three_d_opts.planar_y_rot_scale,
        three_d_opts.current.yrot);

      XmScaleSetValue(three_d_opts.planar_z_rot_scale,
        three_d_opts.current.zrot);
      }

    XmScaleSetValue(three_d_opts.planar_x_scale_scale,
      three_d_opts.current.xscale);

    XmScaleSetValue(three_d_opts.planar_y_scale_scale,
      three_d_opts.current.yscale);
    }

  XmScaleSetValue(three_d_opts.planar_rough_scale,
    three_d_opts.current.rough);

  XmScaleSetValue(three_d_opts.planar_water_scale,
    three_d_opts.current.waterline);

  if(!RAY)
    {
    XmScaleSetValue(three_d_opts.planar_perspective_scale,
      three_d_opts.current.zviewer);

    XmScaleSetValue(three_d_opts.planar_x_shift_scale,
      three_d_opts.current.xshift);

    XmScaleSetValue(three_d_opts.planar_y_shift_scale,
      three_d_opts.current.yshift);

    XmScaleSetValue(three_d_opts.planar_x_adjust_scale,
      three_d_opts.current.xtrans);

    XmScaleSetValue(three_d_opts.planar_y_adjust_scale,
      three_d_opts.current.ytrans);

    XmScaleSetValue(three_d_opts.planar_first_color_scale,
      three_d_opts.current.transparent[0]);

    XmScaleSetValue(three_d_opts.planar_last_color_scale,
      three_d_opts.current.transparent[1]);
    }

  XmScaleSetValue(three_d_opts.planar_random_color_scale,
    three_d_opts.current.rndcolor);
}

/*ARGSUSED*/
static void three_d_planar_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(SPHERE)
    {
    XmScaleGetValue(three_d_opts.planar_long_start_scale,
      &(three_d_opts.current.phi1));

    XmScaleGetValue(three_d_opts.planar_long_stop_scale,
      &(three_d_opts.current.phi2));

    XmScaleGetValue(three_d_opts.planar_lat_start_scale,
      &(three_d_opts.current.theta1));

    XmScaleGetValue(three_d_opts.planar_lat_stop_scale,
      &(three_d_opts.current.theta2));

    XmScaleGetValue(three_d_opts.planar_radius_pct_scale,
      &(three_d_opts.current.radius));
    }
  else
    {
    if(!RAY)
      {
      XmScaleGetValue(three_d_opts.planar_x_rot_scale,
        &(three_d_opts.current.xrot));

      XmScaleGetValue(three_d_opts.planar_y_rot_scale,
        &(three_d_opts.current.yrot));

      XmScaleGetValue(three_d_opts.planar_z_rot_scale,
        &(three_d_opts.current.zrot));
      }

    XmScaleGetValue(three_d_opts.planar_x_scale_scale,
      &(three_d_opts.current.xscale));

    XmScaleGetValue(three_d_opts.planar_y_scale_scale,
      &(three_d_opts.current.yscale));
    }

  XmScaleGetValue(three_d_opts.planar_rough_scale,
    &(three_d_opts.current.rough));

  XmScaleGetValue(three_d_opts.planar_water_scale,
    &(three_d_opts.current.waterline));

  if(!RAY)
    {
    XmScaleGetValue(three_d_opts.planar_perspective_scale,
      &(three_d_opts.current.zviewer));

    XmScaleGetValue(three_d_opts.planar_x_shift_scale,
      &(three_d_opts.current.xshift));

    XmScaleGetValue(three_d_opts.planar_y_shift_scale,
      &(three_d_opts.current.yshift));

    XmScaleGetValue(three_d_opts.planar_x_adjust_scale,
      &(three_d_opts.current.xtrans));

    XmScaleGetValue(three_d_opts.planar_y_adjust_scale,
      &(three_d_opts.current.ytrans));

    XmScaleGetValue(three_d_opts.planar_first_color_scale,
      &(three_d_opts.current.transparent[0]));

    XmScaleGetValue(three_d_opts.planar_last_color_scale,
      &(three_d_opts.current.transparent[1]));
    }

  XmScaleGetValue(three_d_opts.planar_random_color_scale,
    &(three_d_opts.current.rndcolor));

  PHI1 = three_d_opts.current.phi1;
  PHI2 = three_d_opts.current.phi2;
  THETA1 = three_d_opts.current.theta1;
  THETA2 = three_d_opts.current.theta2;
  RADIUS = three_d_opts.current.radius;
  XROT = three_d_opts.current.xrot;
  YROT = three_d_opts.current.yrot;
  ZROT = three_d_opts.current.zrot;
  XSCALE = three_d_opts.current.xscale;
  YSCALE = three_d_opts.current.yscale;
  ROUGH = three_d_opts.current.rough;
  WATERLINE = three_d_opts.current.waterline;
  ZVIEWER = three_d_opts.current.zviewer;
  XSHIFT = three_d_opts.current.xshift;
  YSHIFT = three_d_opts.current.yshift;
  xtrans = three_d_opts.current.xtrans;
  ytrans = three_d_opts.current.ytrans;
  transparent[0] = three_d_opts.current.transparent[0];
  transparent[1] = three_d_opts.current.transparent[1];
  RANDOMIZE = three_d_opts.current.rndcolor;

  XtPopdown(three_d_planar_dialog_shell);
  XtDestroyWidget(three_d_planar_dialog_shell);
  three_d_planar_dialog_shell = NULL;

  if((Targa_Out || ILLUMINE || RAY))
    get_light_params(main_window, NULL, NULL);
  else
    display_file();
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: planar.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.9  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.8  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.7  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.6  1995/03/28  18:11:40  darryl
 * Fix trig function handling. Enhance center-mag button
 * management. Aesthetic changes in the parameter dialog.
 * Add frames around groups of scale widgets. Fix a bogus
 * message about color cell allocation if sharecolor.
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
