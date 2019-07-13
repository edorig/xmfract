
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: zoom_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void zoom_box_color
  (Widget w, XtPointer client_data, XtPointer call_data);
static void zoomcolor_color_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void zoomcolor_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void zoomcolor_setcolor(void);

/* static variables */
static Widget zoomcolor_shell = NULL;
static Widget zoomcolor_color_scale = NULL;
static int zoomcolor_shell_popped_up = 0;

void
create_zoom_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget button;
  Widget zoom_button;
  Widget zoom_menu_btn;
  Widget zoom_menu_toc;
  Widget zoom_menu_pane;
  XmString str;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  zoom_menu_pane = XmCreatePulldownMenu (w, "zoom_menu_pane", args, n);

  /* get the tear-off control */

  if((zoom_menu_toc = XmGetTearOffControl
    (zoom_menu_pane)) != NULL)
    {
    XtVaSetValues(zoom_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Zoom' cascade button */

  n = 0;
  str = XmStringCreateLocalized("Zoom");
  XtSetArg(args[n], XmNsubMenuId, zoom_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  zoom_button = XmCreateCascadeButton(w, "zoom_button", args, n);

  XtManageChild(zoom_button);
  XmStringFree(str);

  /*
   * create pushbuttons and/or cascade pushbuttons for the 'Zoom' menu
   *
   * Turn [On] [Off] Aspect Ratio Snap
   * Pan (Full-Screen Zoom Box)
   * Zoom In (Create Image)
   * Zoom Out (Create Image)
   * Cancel (Erase Zoom Box)
   * Change zoom box color...
   */

  n = 0;
  str = XmStringCreateLocalized("Turn [On] [Off] Aspect Ratio Snap");
  XtSetArg(args[n], XmNlabelString, str); n++;

  zoom_menu_btn = XmCreatePushButton
    (zoom_menu_pane, "zoom_aspect_btn", args, n);

  XtAddCallback(zoom_menu_btn, XmNactivateCallback,
    zoom_aspect_cb, (XtPointer) NULL);

  XtManageChild(zoom_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Pan (Full-Screen Zoom Box)");
  XtSetArg(args[n], XmNlabelString, str); n++;

  zoom_menu_btn = XmCreatePushButton
    (zoom_menu_pane, "zoom_pan_btn", args, n);

  XtAddCallback(zoom_menu_btn, XmNactivateCallback,
    zoom_pan_cb, (XtPointer) NULL);

  XtManageChild(zoom_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Zoom In (Create Image)");
  XtSetArg(args[n], XmNlabelString, str); n++;

  zoom_menu_btn = XmCreatePushButton(zoom_menu_pane,
    "zoom_in_btn", args, n);

  XtAddCallback(zoom_menu_btn, XmNactivateCallback,
    zoom_in_cb, (XtPointer) NULL);

  XtManageChild(zoom_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Zoom Out (Create Image)");
  XtSetArg(args[n], XmNlabelString, str); n++;

  zoom_menu_btn = XmCreatePushButton
    (zoom_menu_pane, "zoom_out_btn", args, n);

  XtAddCallback(zoom_menu_btn, XmNactivateCallback,
    zoom_out_cb, (XtPointer) NULL);

  XtManageChild(zoom_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Cancel (Erase Zoom Box)");
  XtSetArg(args[n], XmNlabelString, str); n++;

  zoom_menu_btn = XmCreatePushButton
    (zoom_menu_pane, "zoom_cancel_btn", args, n);

  XtAddCallback(zoom_menu_btn, XmNactivateCallback,
    zoom_cancel_cb, (XtPointer) NULL);

  XtManageChild(zoom_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Change zoom box color...");
  XtSetArg(args[n], XmNlabelString, str); n++;

  zoom_menu_btn = XmCreatePushButton
    (zoom_menu_pane, "zoom_color_btn", args, n);

  XtAddCallback(zoom_menu_btn, XmNactivateCallback,
    zoom_box_color, (XtPointer) NULL);

  XtManageChild(zoom_menu_btn);
  XmStringFree(str);

  if(rubber_band_data.boxisactive != 1)
    {
    button = XtNameToWidget(menu_bar, "*zoom_aspect_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_in_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_out_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_cancel_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_color_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    }
  else if(rubber_band_data.moving || rubber_band_data.stretching)
    {
    button = XtNameToWidget(menu_bar, "*zoom_in_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_out_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    }
  else
    {
    button = XtNameToWidget(menu_bar, "*zoom_aspect_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_in_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_out_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_cancel_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_color_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    }

  if(rubber_band_data.snap_to_aspect == 0)
    str = XmStringCreateLocalized( "Turn On Aspect Ratio Snap");
  else
    str = XmStringCreateLocalized( "Turn Off Aspect Ratio Snap");

  button = XtNameToWidget(menu_bar, "*zoom_aspect_btn");
  if(button != NULL)
    XtVaSetValues(button, XmNlabelString, str, NULL);

  XmStringFree(str);
}

void
update_zoom_menu()
{
  Widget button;
  XmString str;

  if(rubber_band_data.boxisactive != 1)
    {
    button = XtNameToWidget(menu_bar, "*zoom_aspect_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_in_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_out_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_cancel_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_color_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    }
  else if(rubber_band_data.moving || rubber_band_data.stretching)
    {
    button = XtNameToWidget(menu_bar, "*zoom_in_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    button = XtNameToWidget(menu_bar, "*zoom_out_btn");
    if(button != NULL)
      XtSetSensitive(button, False);
    }
  else
    {
    button = XtNameToWidget(menu_bar, "*zoom_aspect_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_in_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_out_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_cancel_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    button = XtNameToWidget(menu_bar, "*zoom_color_btn");
    if(button != NULL)
      XtSetSensitive(button, True);
    }

  if(rubber_band_data.snap_to_aspect == 0)
    str = XmStringCreateLocalized( "Turn On Aspect Ratio Snap");
  else
    str = XmStringCreateLocalized( "Turn Off Aspect Ratio Snap");

  button = XtNameToWidget(menu_bar, "*zoom_aspect_btn");
  if(button != NULL)
    XtVaSetValues(button, XmNlabelString, str, NULL);

  XmStringFree(str);
}

/*ARGSUSED*/
static void zoom_box_color(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget zoomcolor_dialog = NULL;

  Widget label;
  Widget form;
  Widget button;
  Arg args[MAX_ARGS];
  XmString str;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 6 - 1;

  if(!zoomcolor_shell)
    {
    zoomcolor_shell = XmCreateDialogShell
      (main_window, "zoomcolor_shell", NULL, 0);

    XtAddCallback(zoomcolor_shell, XmNpopupCallback,  map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(zoomcolor_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Change zoom box color");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    zoomcolor_dialog = XmCreateForm
      (zoomcolor_shell, "zoomcolor_dialog", args, n);

    XtAddCallback(zoomcolor_dialog, XmNmapCallback, map_dialog, NULL);
    XmStringFree(str);

    /*
     * Sliding this scale will change the color of the zoom box.
     * Select a color that meets your particular requirements.
     * (label)
     */

    n = 0;
    str = XmStringCreateLtoR
      ("Sliding this scale will change the color of the zoom box.\nSelect a color that meets your particular requirements.", XmFONTLIST_DEFAULT_TAG);
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel(zoomcolor_dialog, "zoomcolor_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    /*
     * Zoom box color
     * (scale)
     */

    n = 0;
    XtSetArg(args[n], XmNmaximum, colors - 1); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, False); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, label); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    zoomcolor_color_scale =
      XmCreateScale (zoomcolor_dialog, "zoomcolor_color_scale", args, n);

    XtAddCallback(zoomcolor_color_scale, XmNvalueChangedCallback,
      zoomcolor_color_cb, NULL);

    XtAddCallback(zoomcolor_color_scale, XmNdragCallback,
      zoomcolor_color_cb, NULL);

    XtManageChild(zoomcolor_color_scale);

    /*
     * Done
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Done");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 0); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, zoomcolor_color_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 5 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (zoomcolor_dialog, "zoomcolor_apply_button", args, n);

    XtAddCallback(button, XmNactivateCallback,
      zoomcolor_done_button_cb, "1");

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(zoomcolor_dialog,
      XmNdefaultButton,
      button,
      NULL);
    }

  if(zoomcolor_index >= colors)
    zoomcolor_index = colors - 1;

  XtVaSetValues(zoomcolor_color_scale,
    XmNvalue, (int) zoomcolor_index, NULL);

  XtManageChild(zoomcolor_dialog);
  XtManageChild(zoomcolor_shell);
  XtPopup(zoomcolor_shell, XtGrabNone);

  zoomcolor_shell_popped_up = 1;
}

/*ARGSUSED*/
static void zoomcolor_color_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  zoomcolor_index = cbs->value;
  zoomcolor_setcolor();
}

static void zoomcolor_setcolor()
{
  if(rubber_band_data.boxisactive == 1)
    {
    change_xor_gc(2);
    XDrawRectangle(display, XtWindow(canvas), xgc,
      rubber_band_data.tl_x, rubber_band_data.tl_y,
      rubber_band_data.br_x - rubber_band_data.tl_x,
      rubber_band_data.br_y - rubber_band_data.tl_y);

    if(rubber_band_data.panning == 0)
      {
      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_tl_x, rubber_band_data.h_tl_y,
        rubber_band_data.h_tr_x, rubber_band_data.h_tr_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_bl_x, rubber_band_data.h_bl_y,
        rubber_band_data.h_br_x, rubber_band_data.h_br_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tl_x, rubber_band_data.v_tl_y,
        rubber_band_data.v_bl_x, rubber_band_data.v_bl_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tr_x, rubber_band_data.v_tr_y,
        rubber_band_data.v_br_x, rubber_band_data.v_br_y);
      }
    }

  XSetForeground(display, xgc, cols[zoomcolor_index].pixel);

  if(rubber_band_data.boxisactive == 1)
    {
    change_xor_gc(2);
    XDrawRectangle(display, XtWindow(canvas), xgc,
      rubber_band_data.tl_x, rubber_band_data.tl_y,
      rubber_band_data.br_x - rubber_band_data.tl_x,
      rubber_band_data.br_y - rubber_band_data.tl_y);

    if(rubber_band_data.panning == 0)
      {
      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_tl_x, rubber_band_data.h_tl_y,
        rubber_band_data.h_tr_x, rubber_band_data.h_tr_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.h_bl_x, rubber_band_data.h_bl_y,
        rubber_band_data.h_br_x, rubber_band_data.h_br_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tl_x, rubber_band_data.v_tl_y,
        rubber_band_data.v_bl_x, rubber_band_data.v_bl_y);

      XDrawLine(display, XtWindow(canvas), xgc,
        rubber_band_data.v_tr_x, rubber_band_data.v_tr_y,
        rubber_band_data.v_br_x, rubber_band_data.v_br_y);
      }
    }
}

/*ARGSUSED*/
static void zoomcolor_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(zoomcolor_shell);
  zoomcolor_shell_popped_up = 0;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: zoom_menu.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.10  1995/05/19  08:11:11  darryl
 * Add initialization for zoom box color index.
 *
 * Revision 1.9  1995/05/18  22:55:48  darryl
 * Change menu position of the zoom box color item.
 *
 * Revision 1.8  1995/05/16  18:02:13  darryl
 * Add zoom box color selector.
 *
 * Revision 1.7  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
 *
 * Revision 1.6  1995/04/18  15:59:59  darryl
 * Fix problems and incorporate enhancements from beta test.
 *  -- not able to zoom after loading a saved image.
 *  -- string parsing would fail because the tags were incorrect.
 *  -- CLK_TCK is determined from sysconf() if it is available.
 *  -- Julia sets were not calculated correctly after image save.
 *  -- some void pointers were not cast before promotion.
 *  -- Info About Image display says it was working on pass 2 of 1.
 *  -- an incomplete image could not be saved.
 *  -- a saved image was not drawn if the zoomcolor rate was < 100.
 *  -- added Refresh Rate menu item and command line support.
 *  -- added Parameter Forwarding menu item and command line support.
 *  -- added fallback resources.
 *  -- enhanced parameter handling.
 *  -- implemented the Fractint zoombox 'panning' capability.
 *  -- enhanced the LogMap and LogTable handling.
 *  -- various typograhic error fixes.
 *
 * Revision 1.5  1995/04/11  15:37:45  darryl
 * Remove hard-coded tearOffModel resources to allow the
 * resource file to over-ride the fallback resource.
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
 * Revision 0.102  1994/11/30  21:09:19  darryl
 * Set the XmNseparatorType resource for TearOffControls.
 *
 * Revision 0.101  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
