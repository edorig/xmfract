
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: coloredit.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 * Note that the order of the rgb values in the dacbox variable
 * is different than that of the cols structure. This can and
 * will be used against you wherever possible. Caveat utilitor.
 *
 * dacbox = rgb
 * cols   = rbg
 *
 * dacbox[n][0] = cols[n].red
 * dacbox[n][1] = cols[n].green
 * dacbox[n][2] = cols[n].blue
 */

/* static functions */
static int ptr_in_rect(int x, int y);
static void load_active_register(int color);
static void load_inactive_register(int color);
static void freeze_inactive_register(int color);
static void unfreeze_inactive_register(void);
static void swaprgb(int which);
static void swaprgbrange(int which);
static void copy_inactive_to_active(void);
static void copy_active_to_inactive(void);
static void range_smooth(int skip);
static void copy_range(void);
static void paste_range(int direction);
static void get_skip_val(Widget w);
static void invertrgb(int which);
static void set_gamma(void);
static void set_brightness(int which);
static void freestyle_setcolors(int lo, int hi);
static void freestyle_cleanup(void);
static void toggle_range_mode(void);
static void undo_last_color_edit(void);
static void redo_last_color_edit(void);

static void editpal_colorbox_cursor
  (Widget w, XtPointer client_data, XEvent *event);
static void editpal_place_cursor
  (Widget w, XtPointer client_data, XEvent *event);
static void editpal_pointer_motion
  (Widget w, XtPointer client_data, XEvent *event);
static void coloredit_edit_menu_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void coloredit_range_menu_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void editpal_help_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void redraw_primary_colorbox
  (Widget w, XtPointer client_data, XtPointer call_data);
static void redraw_secondary_colorbox
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_active_register
  (Widget w, XtPointer client_data, XtPointer call_data);
static void edit_active_color
  (Widget w, XtPointer client_data, XtPointer call_data);
static void editpal_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void skipval_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void skipval_help_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void skipval_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void skipval_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void skipval_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void cyclerange_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void cyclerange_help_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void cyclerange_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void cyclerange_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void cyclerange_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void gammaval_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void gammaval_help_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void gammaval_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void gammaval_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void gammaval_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_gamma_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void set_freestyle_values
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_freestyle_mode
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_freestyle_wrap
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_range_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_color_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_endcolor_hi_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_endcolor_lo_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_transparency_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_help_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void freestyle_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void edit_center_color
  (Widget w, XtPointer client_data, XtPointer call_data);
static void redraw_freestyle_colorbox
  (Widget w, XtPointer client_data, XtPointer call_data);
static void rand_map_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_randmap_mode
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_randmap_func
  (Widget w, XtPointer client_data, XtPointer call_data);
static void randmap_help_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void randmap_apply_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void randmap_reset_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void randmap_dflt_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void randmap_cancel_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void randmap_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void set_cycle_range
  (Widget w, XtPointer client_data, XtPointer call_data);
static void redo_color_edit_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget editpal_options_dialog = NULL;
static Widget randmap_shell = NULL;
static Widget skipval_shell = NULL;
static Widget gammaval_shell = NULL;
static Widget gammaval_scale = NULL;
static Widget coloredit_range_menu = NULL;
static Widget coloredit_edit_menu = NULL;
static Widget cyclerange_shell = NULL;
static Widget cycle_from_scale = NULL;
static Widget cycle_to_scale = NULL;
static Widget freestyle_shell = NULL;
static Widget freestyle_range_scale = NULL;
static Widget freestyle_color_scale = NULL;
static Widget freestyle_endcolor_hi_scale = NULL;
static Widget freestyle_endcolor_lo_scale = NULL;
static Widget freestyle_transp_scale = NULL;
static Widget freestyle_mode_on_toggle = NULL;
static Widget freestyle_mode_off_toggle = NULL;
static Widget freestyle_wrap_on_toggle = NULL;
static Widget freestyle_wrap_off_toggle = NULL;
static Widget freestyle_red_scale = NULL;
static Widget freestyle_green_scale = NULL;
static Widget freestyle_blue_scale = NULL;
static Widget freestyle_static_label = NULL;
static Widget freestyle_dynamic_label = NULL;
static Widget freestyle_register_colorbox = NULL;
static Widget randmap_mode_0_toggle = NULL;
static Widget randmap_mode_1_toggle = NULL;
static Widget randmap_mode_2_toggle = NULL;
static Widget randmap_mode_3_toggle = NULL;
static Widget randmap_mode_4_toggle = NULL;
static Widget randmap_mode_5_toggle = NULL;
static Widget randmap_mode_6_toggle = NULL;
static Widget randmap_mode_7_toggle = NULL;
static Widget randmap_func_1_toggle = NULL;
static Widget randmap_func_2_toggle = NULL;
static Widget randmap_func_3_toggle = NULL;
static Widget randmap_func_4_toggle = NULL;
static Widget randmap_func_5_toggle = NULL;
static Widget randmap_func_6_toggle = NULL;
static Widget randmap_func_7_toggle = NULL;
static Widget randmap_func_8_toggle = NULL;
static GC freestyle_colorbox_gc = NULL;
static int freestyle_shell_popped_up = 0;
static unsigned char merge_color[3];
static int randmap_mode;
static int randmap_func;

void
create_coloredit_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget coloredit_menu_button;
  Widget coloredit_menu_toc;
  Widget coloredit_menu_pane;
  Widget menu_button;
  XmString str;
  XmString accel;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  coloredit_menu_pane = XmCreatePulldownMenu
    (w, "coloredit_menu_pane", args, n);

  /* get the tear-off control */

  if((coloredit_menu_toc = XmGetTearOffControl
    (coloredit_menu_pane)) != NULL)
    {
    XtVaSetValues(coloredit_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Color' cascade button */

  n = 0;
  str = XmStringCreateLocalized("Color");
  XtSetArg(args[n], XmNsubMenuId, coloredit_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  coloredit_menu_button = XmCreateCascadeButton
    (w, "coloredit_menu_button", args, n);

  XtManageChild(coloredit_menu_button);
  XmStringFree(str);

  /*
   * create pushbuttons and/or cascade pushbuttons for the 'New Image' menu
   *
   * Edit color palette...
   * Save color palette to file...
   * Load color palette from file...
   * Randomize color palette...
   */

  n = 0;
  str = XmStringCreateLocalized("Edit color palette...");
  accel = XmStringCreateLocalized("[ e ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>e:"); n++;

  menu_button = XmCreatePushButton
    (coloredit_menu_pane, "coloredit_edit_btn", args, n);

  XtAddCallback(menu_button, XmNactivateCallback,
    editpal_cb, (XtPointer) NULL);

  XtManageChild(menu_button);

  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Save color palette to file...");
  accel = XmStringCreateLocalized("[ m ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>m:"); n++;

  menu_button = XmCreatePushButton
    (coloredit_menu_pane, "coloredit_save_btn", args, n);

  XtAddCallback(menu_button, XmNactivateCallback,
    save_palette_cb, (XtPointer) NULL);

  XtManageChild(menu_button);

  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Load color palette from file...");
  accel = XmStringCreateLocalized("[ l ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>l:"); n++;

  menu_button = XmCreatePushButton
    (coloredit_menu_pane, "coloredit_load_btn", args, n);

  XtAddCallback(menu_button, XmNactivateCallback,
    load_map_cb, (XtPointer) NULL);

  XtManageChild(menu_button);

  XmStringFree(str);
  XmStringFree(accel);

  n = 0;
  str = XmStringCreateLocalized("Randomize color palette...");
  XtSetArg(args[n], XmNlabelString, str); n++;

  menu_button = XmCreatePushButton
    (coloredit_menu_pane, "coloredit_rand_btn", args, n);

  XtAddCallback(menu_button, XmNactivateCallback,
    rand_map_cb, (XtPointer) NULL);

  XtManageChild(menu_button);
  XmStringFree(str);
}

/*ARGSUSED*/
void create_editpal_options_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Pixel bg;
  XGCValues values;
  Widget button;
  XmString str;
  char tempstr[128];
  int color_rows;
  int boxht;
  int fracbase;
  int btnbase;
  int done;
  int n;

  btnbase = 120;
  fracbase = btnbase * 6 - 1;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(rotate_stuff.running == 1)
    toggle_color_cycling();

  paledit.range_mode = 0;
  freestyle.wraparound = 0;
  paledit.freestyle_mode = 0;

  if(undo_max > 0)
    {
    paledit.undo_index = 0;
    paledit.redo_index = 0;
    memset(undobox, 0, (sizeof(unsigned char) * undo_max * 256 * 3));
    memcpy(undobox, dacbox, 768);
    }

  /*
   * paledit.edit_palette_dialog is set to NULL in initvars_restart().
   */

  if(!paledit.edit_palette_dialog)
    {
    Widget frame;
    Widget form;
    Widget primary_colorbox_static_label;
    Widget secondary_colorbox_static_label;
    Widget coloredit_edit_menu_toc;
    Widget coloredit_range_menu_toc;
    XmString actstr;
    XmString undostr;
    XmString inactstr;
    XmString rangestr;
    XmString smoothstr;
    XmString stripestr;
    XmString brightupstr;
    XmString brightdnstr;
    XmString copystr;
    XmString rangemodestr;
    XmString pastefwdstr;
    XmString pastebwdstr;
    XmString resetstr;
    XmString rngmodestr;
    XmString editstr;
    XmString copyactstr;
    XmString copyinactstr;
    XmString rgswapstr;
    XmString gbswapstr;
    XmString brswapstr;
    XmString invertstr;
    XmString gammastr;
    XmString undolaststr;
    XmString redolaststr;
    Arg args[MAX_ARGS];
    char title[128];

    sprintf((char *) &title, "Color Editor (%3d colors)   Freestyle mode %s",
      colors, (paledit.freestyle_mode == 1 ? "On" : "Off"));

    paledit.edit_palette_dialog = XmCreateDialogShell
      (main_window, "edit_palette_dialog_shell", NULL, 0);

    XtAddCallback(paledit.edit_palette_dialog, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(paledit.edit_palette_dialog, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized((char *) &title);
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    editpal_options_dialog = XmCreateForm
      (paledit.edit_palette_dialog, "editpal_options_dialog", args, n);

    XtAddCallback(editpal_options_dialog, XmNmapCallback,
      map_dialog, NULL);

    XtAddCallback(editpal_options_dialog, XmNunmapCallback,
      editpal_cancel_cb, (XtPointer) "1");

    XmStringFree(str);

    /*
     * Primary color register
     */

    /*
     * frame
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;

    paledit.primary_register_frame = XmCreateFrame
      (editpal_options_dialog, "primary_register_frame", args, n);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    paledit.primary_register_form = XmCreateForm
      (paledit.primary_register_frame, "primary_register_form", args, n);

    /*
     * Red
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("R");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;

    paledit.primary_red_scale = XmCreateScale
      (paledit.primary_register_form, "primary_red_scale", args, n);

    XtAddCallback(paledit.primary_red_scale, XmNvalueChangedCallback,
      edit_active_color, "0");

    XtAddCallback(paledit.primary_red_scale, XmNdragCallback,
      edit_active_color, "0");

    XtManageChild(paledit.primary_red_scale);
    XmStringFree(str);

    /*
     * Green
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("G");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, (btnbase * 2)); n++;

    paledit.primary_green_scale = XmCreateScale
      (paledit.primary_register_form, "primary_green_scale", args, n);

    XtAddCallback(paledit.primary_green_scale, XmNvalueChangedCallback,
      edit_active_color, "0");

    XtAddCallback(paledit.primary_green_scale, XmNdragCallback,
      edit_active_color, "0");

    XtManageChild(paledit.primary_green_scale);
    XmStringFree(str);

    /*
     * Blue
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("B");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition,
      (btnbase * 2) + ((btnbase * 2) - 1)); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

    paledit.primary_blue_scale = XmCreateScale
      (paledit.primary_register_form, "primary_blue_scale", args, n);

    XtAddCallback(paledit.primary_blue_scale, XmNvalueChangedCallback,
      edit_active_color, "0");

    XtAddCallback(paledit.primary_blue_scale, XmNdragCallback,
      edit_active_color, "0");

    XtManageChild(paledit.primary_blue_scale);
    XmStringFree(str);

    /*
     * Active
     * (toggle button)
     */

    actstr = XmStringCreateLocalized("Active");
    inactstr = XmStringCreateLocalized("Inactive");

    n = 0;
    XtSetArg(args[n], XmNlabelString, actstr); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.primary_red_scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;

    paledit.primary_active_button = XmCreateToggleButton
      (paledit.primary_register_form, "primary_active_button", args, n);

    XtAddCallback(paledit.primary_active_button, XmNvalueChangedCallback,
      toggle_active_register, "0");

    XtManageChild(paledit.primary_active_button);

    /*
     * Color index: nnn
     * (labels)
     */

    n = 0;
    str = XmStringCreateLocalized("Color index:");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.primary_active_button); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;

    primary_colorbox_static_label = XmCreateLabel
      (paledit.primary_register_form, "primary_colorbox_static_label", args, n);

    XtManageChild(primary_colorbox_static_label);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("111");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, primary_colorbox_static_label); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    paledit.primary_colorbox_label = XmCreateLabel
      (paledit.primary_register_form, "primary_colorbox_label", args, n);

    XtManageChild(paledit.primary_colorbox_label);
    XmStringFree(str);

    /*
     * primary colorbox
     */

    n = 0;
    XtSetArg(args[n], XmNwidth, 60); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNtopWidget, primary_colorbox_static_label); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    paledit.primary_active_colorbox = XtCreateManagedWidget
      ("primary_active_colorbox", xmDrawingAreaWidgetClass,
        paledit.primary_register_form, args, n);

    XtAddCallback(paledit.primary_active_colorbox, XmNexposeCallback,
      redraw_primary_colorbox, NULL);

    XtManageChild(paledit.primary_register_form);
    XtManageChild(paledit.primary_register_frame);

    /*
     * Secondary color register
     */

    /*
     * frame
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    paledit.secondary_register_frame = XmCreateFrame
      (editpal_options_dialog, "secondary_register_frame", args, n);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    paledit.secondary_register_form = XmCreateForm
      (paledit.secondary_register_frame, "secondary_register_form", args, n);

    /*
     * Red
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("R");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;

    paledit.secondary_red_scale = XmCreateScale
      (paledit.secondary_register_form, "secondary_red_scale", args, n);

    XtAddCallback(paledit.secondary_red_scale, XmNvalueChangedCallback,
      edit_active_color, "1");

    XtAddCallback(paledit.secondary_red_scale, XmNdragCallback,
      edit_active_color, "1");

    XtManageChild(paledit.secondary_red_scale);
    XmStringFree(str);

    /*
     * Green
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("G");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, (btnbase * 2)); n++;

    paledit.secondary_green_scale = XmCreateScale
      (paledit.secondary_register_form, "secondary_green_scale", args, n);

    XtAddCallback(paledit.secondary_green_scale, XmNvalueChangedCallback,
      edit_active_color, "1");

    XtAddCallback(paledit.secondary_green_scale, XmNdragCallback,
      edit_active_color, "1");

    XtManageChild(paledit.secondary_green_scale);
    XmStringFree(str);

    /*
     * Blue
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("B");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition,
      (btnbase * 2) + ((btnbase * 2) - 1)); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

    paledit.secondary_blue_scale = XmCreateScale
      (paledit.secondary_register_form, "secondary_blue_scale", args, n);

    XtAddCallback(paledit.secondary_blue_scale, XmNvalueChangedCallback,
      edit_active_color, "1");

    XtAddCallback(paledit.secondary_blue_scale, XmNdragCallback,
      edit_active_color, "1");

    XtManageChild(paledit.secondary_blue_scale);
    XmStringFree(str);

    /*
     * Inactive
     * (toggle button)
     */

    n = 0;
    XtSetArg(args[n], XmNlabelString, inactstr); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.secondary_red_scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;

    paledit.secondary_active_button = XmCreateToggleButton
      (paledit.secondary_register_form, "secondary_active_button", args, n);

    XtAddCallback(paledit.secondary_active_button, XmNvalueChangedCallback,
      toggle_active_register, "1");

    XtManageChild(paledit.secondary_active_button);

    /*
     * Color index: nnn
     * (labels)
     */

    n = 0;
    str = XmStringCreateLocalized("Color index:");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.secondary_active_button); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;

    secondary_colorbox_static_label = XmCreateLabel
      (paledit.secondary_register_form,
      "secondary_colorbox_static_label", args, n);

    XtManageChild(secondary_colorbox_static_label);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("222");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, secondary_colorbox_static_label); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    paledit.secondary_colorbox_label = XmCreateLabelGadget
      (paledit.secondary_register_form, "secondary_colorbox_label", args, n);

    XtManageChild(paledit.secondary_colorbox_label);
    XmStringFree(str);

    /*
     * secondary colorbox
     */

    n = 0;
    XtSetArg(args[n], XmNwidth, 60); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNtopWidget, secondary_colorbox_static_label); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    paledit.secondary_active_colorbox = XtCreateManagedWidget
      ("secondary_active_colorbox", xmDrawingAreaWidgetClass,
        paledit.secondary_register_form, args, n);

    XtAddCallback(paledit.secondary_active_colorbox, XmNexposeCallback,
      redraw_secondary_colorbox, NULL);

    XtManageChild(paledit.secondary_register_form);
    XtManageChild(paledit.secondary_register_frame);

    /*
     * Color palette
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.secondary_register_frame); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    paledit.palette_colorbox = XtCreateManagedWidget
      ("palette_colorbox", xmDrawingAreaWidgetClass,
        editpal_options_dialog, args, n);

    XtAddCallback(paledit.palette_colorbox, XmNexposeCallback,
      redraw_palette_colorbox, NULL);

    /*
     * strings for option menus
     * forced to the same length
     * for aesthetic purposes
     */

    /* titles */

    editstr = XmStringCreateLocalized
      ("Color editing ");

    rangestr = XmStringCreateLocalized
      ("Palette ranges");

    /* edit commands */

    undostr = XmStringCreateLocalized
      ("Undo all edits                                 ");

    undolaststr = XmStringCreateLocalized
      ("Undo last edit (    0 undo's available)        ");

    redolaststr = XmStringCreateLocalized
      ("Redo last undo (    0 redo's available)        ");

    copyinactstr = XmStringCreateLocalized
      ("Copy from inactive register to active register ");

    copyactstr = XmStringCreateLocalized
      ("Copy from active register to inactive register ");

    rgswapstr = XmStringCreateLocalized
      ("Swap values in Red and Green columns           ");

    gbswapstr = XmStringCreateLocalized
      ("Swap values in Green and Blue columns          ");

    brswapstr = XmStringCreateLocalized
      ("Swap values in Blue and Red columns            ");

    invertstr = XmStringCreateLocalized
      ("Invert Red, Green, and Blue values             ");

    gammastr = XmStringCreateLocalized
      ("Set gamma value for ranges and freestyle mode  ");

    /* range commands */

    resetstr = XmStringCreateLocalized
      ("Cancel range selection                         ");

    rngmodestr = XmStringCreateLocalized
      ("Range mode: inclusive. Toggle to exclusive mode");

    copystr = XmStringCreateLocalized
      ("Copy selected range to memory                  ");

    pastefwdstr = XmStringCreateLocalized
      ("Paste copied range to selected range (forward) ");

    pastebwdstr = XmStringCreateLocalized
      ("Paste copied range to selected range (reverse) ");

    smoothstr = XmStringCreateLocalized
      ("Create a smooth range of shades                ");

    stripestr = XmStringCreateLocalized
      ("Create a striped ranged of shades              ");

    brightupstr = XmStringCreateLocalized
      ("Increase the brightness of the selected range  ");

    brightdnstr = XmStringCreateLocalized
      ("Decrease the brightness of the selected range  ");

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.palette_colorbox); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    frame = XmCreateFrame
      (editpal_options_dialog,
      "coloredit_edit_menu_frame", args, n);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    form = XmCreateForm(frame, "coloredit_edit_menu_form", args, n);

    coloredit_edit_menu = XmVaCreateSimpleOptionMenu
      (form, "coloredit_edit_menu",
       editstr, (KeySym)NULL, 0, coloredit_edit_menu_cb,
       XmVaPUSHBUTTON, undostr,      NULL, NULL, NULL,
       XmVaPUSHBUTTON, undolaststr,  NULL, NULL, NULL,
       XmVaPUSHBUTTON, redolaststr,  NULL, NULL, NULL,
       XmVaPUSHBUTTON, copyinactstr, NULL, NULL, NULL,
       XmVaPUSHBUTTON, copyactstr,   NULL, NULL, NULL,
       XmVaPUSHBUTTON, rgswapstr,    NULL, NULL, NULL,
       XmVaPUSHBUTTON, gbswapstr,    NULL, NULL, NULL,
       XmVaPUSHBUTTON, brswapstr,    NULL, NULL, NULL,
       XmVaPUSHBUTTON, invertstr,    NULL, NULL, NULL,
       XmVaPUSHBUTTON, gammastr,     NULL, NULL, NULL,
       XmNleftAttachment, XmATTACH_FORM,
       XmNrightAttachment, XmATTACH_FORM,
       XmNtopAttachment, XmATTACH_FORM,
       XmNbottomAttachment, XmATTACH_FORM,
       NULL);

    /* get the tear-off control */

    if((coloredit_edit_menu_toc = XmGetTearOffControl
      (coloredit_edit_menu)) != NULL)
      {
      XtVaSetValues(coloredit_edit_menu_toc, XmNseparatorType,
        XmSINGLE_DASHED_LINE, NULL);
      }

    XtManageChild(coloredit_edit_menu);
    XtManageChild(form);
    XtManageChild(frame);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    frame = XmCreateFrame
      (editpal_options_dialog,
      "coloredit_range_menu_frame", args, n);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    form = XmCreateForm(frame, "coloredit_range_menu_form", args, n);

    coloredit_range_menu = XmVaCreateSimpleOptionMenu
      (form, "coloredit_range_menu",
       rangestr, (KeySym)NULL, 0, coloredit_range_menu_cb,
       XmVaPUSHBUTTON, resetstr,    NULL, NULL, NULL,
       XmVaPUSHBUTTON, rngmodestr,  NULL, NULL, NULL,
       XmVaPUSHBUTTON, copystr,     NULL, NULL, NULL,
       XmVaPUSHBUTTON, pastefwdstr, NULL, NULL, NULL,
       XmVaPUSHBUTTON, pastebwdstr, NULL, NULL, NULL,
       XmVaPUSHBUTTON, smoothstr,   NULL, NULL, NULL,
       XmVaPUSHBUTTON, stripestr,   NULL, NULL, NULL,
       XmVaPUSHBUTTON, rgswapstr,   NULL, NULL, NULL,
       XmVaPUSHBUTTON, gbswapstr,   NULL, NULL, NULL,
       XmVaPUSHBUTTON, brswapstr,   NULL, NULL, NULL,
       XmVaPUSHBUTTON, invertstr,   NULL, NULL, NULL,
       XmVaPUSHBUTTON, brightupstr, NULL, NULL, NULL,
       XmVaPUSHBUTTON, brightdnstr, NULL, NULL, NULL,
       XmNleftAttachment, XmATTACH_FORM,
       XmNrightAttachment, XmATTACH_FORM,
       XmNtopAttachment, XmATTACH_FORM,
       XmNbottomAttachment, XmATTACH_FORM,
       NULL);

    /* get the tear-off control */

    if((coloredit_range_menu_toc = XmGetTearOffControl
      (coloredit_range_menu)) != NULL)
      {
      XtVaSetValues(coloredit_range_menu_toc, XmNseparatorType,
        XmSINGLE_DASHED_LINE, NULL);
      }

    if(paledit.range_mode == 0)
      {
      str = XmStringCreateLocalized
        ("Range mode: inclusive. Toggle to exclusive mode");
      }
    else
      {
      str = XmStringCreateLocalized
        ("Range mode: exclusive. Toggle to inclusive mode");
      }

    button = XtNameToWidget(editpal_options_dialog,
      "*coloredit_range_menu.button_1");
    XtVaSetValues(button, XmNlabelString, str, NULL);
    XmStringFree(str);

    XtManageChild(coloredit_range_menu);
    XtManageChild(form);
    XtManageChild(frame);

    XmStringFree(actstr);
    XmStringFree(undostr);
    XmStringFree(inactstr);
    XmStringFree(rangestr);
    XmStringFree(smoothstr);
    XmStringFree(stripestr);
    XmStringFree(brightupstr);
    XmStringFree(brightdnstr);
    XmStringFree(copystr);
    XmStringFree(pastefwdstr);
    XmStringFree(pastebwdstr);
    XmStringFree(resetstr);
    XmStringFree(rngmodestr);
    XmStringFree(editstr);
    XmStringFree(copyactstr);
    XmStringFree(copyinactstr);
    XmStringFree(rgswapstr);
    XmStringFree(gbswapstr);
    XmStringFree(brswapstr);
    XmStringFree(invertstr);
    XmStringFree(gammastr);
    XmStringFree(undolaststr);
    XmStringFree(redolaststr);

    /*
     * Help
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (editpal_options_dialog, "editpal_options_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      editpal_help_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (editpal_options_dialog, "editpal_options_apply_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      editpal_cancel_cb, (XtPointer) "2");

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Cycling
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Cycling");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (editpal_options_dialog, "editpal_options_cyclerange_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      set_cycle_range, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Freestyle
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Freestyle");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (editpal_options_dialog, "editpal_options_freestyle_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      set_freestyle_values, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (editpal_options_dialog, "editpal_options_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      editpal_cancel_cb, (XtPointer) "1");

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    button = XmCreatePushButton
      (editpal_options_dialog, "editpal_options_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      editpal_cancel_cb, (XtPointer) "0");

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(editpal_options_dialog, XmNdefaultButton, button, NULL);
    }

  values.line_style = LineSolid;

  XChangeGC(display, xgc, GCLineStyle, &values);

  color_rows = (colors >= 16 ? colors / 16 : 1);

  boxht = ((color_rows * 21) + 2);

  XtVaSetValues(paledit.palette_colorbox, XmNheight, boxht, NULL);

  paledit.oldrect = -1;
  paledit.inactive_frozen = 0;
  paledit.pal_range.skip_val = 1;
  paledit.pal_range.start_index = 0;
  paledit.pal_range.stop_index = -1;
  paledit.active_color_register = 0;
  paledit.active_color_register_color = 0;
  paledit.inactive_color_register_color = 0;
  paledit.editpal_canvas_cursor.active = 0;

  for(n = 0; n < 256; n++)
    freestyle.active[n] = 0;

  ok_to_cycle = 1;

  XtVaGetValues(editpal_options_dialog, XmNbackground, &bg, NULL);

  XtVaSetValues(paledit.primary_red_scale,
    XmNvalue, 0,
    NULL);

  XtVaSetValues(paledit.primary_green_scale,
    XmNvalue, 0,
    NULL);

  XtVaSetValues(paledit.primary_blue_scale,
    XmNvalue, 0,
    NULL);

  XtVaSetValues(paledit.secondary_red_scale,
    XmNvalue, 0,
    NULL);

  XtVaSetValues(paledit.secondary_green_scale,
    XmNvalue, 0,
    NULL);

  XtVaSetValues(paledit.secondary_blue_scale,
    XmNvalue, 0,
    NULL);

  if(paledit.active_color_register == 0)
    {
    XtVaSetValues(paledit.primary_active_button,
      XmNset, True,
      XmNhighlightColor, bg,
      NULL);
    XtVaSetValues(paledit.secondary_active_button,
      XmNset, False,
      XmNhighlightColor, bg,
      NULL);
    }
  else
    {
    XtVaSetValues(paledit.primary_active_button,
      XmNset, False,
      XmNhighlightColor, bg,
      NULL);
    XtVaSetValues(paledit.secondary_active_button,
      XmNset, True,
      XmNhighlightColor, bg,
      NULL);
    }

  done = n = 0;

  if(paledit.pal_range.stop_index >= 0)
    {
    XtSetSensitive(coloredit_range_menu, True);

    while(done == 0)
      {
      sprintf((char *) &tempstr, "*coloredit_range_menu*button_%d", n++);
      button = XtNameToWidget(editpal_options_dialog, (char *) &tempstr);
      if(button != NULL)
        XtSetSensitive(button, True);
      else
        done = 1;
      }
    }
  else
    {
    XtSetSensitive(coloredit_range_menu, False);

    while(done == 0)
      {
      sprintf((char *) &tempstr, "*coloredit_range_menu*button_%d", n++);
      button = XtNameToWidget(editpal_options_dialog, (char *) &tempstr);
      if(button != NULL)
        XtSetSensitive(button, False);
      else
        done = 1;
      }
    }

  sprintf((char *) &tempstr,
    "Undo last edit (%5d undo's available)           ",
     paledit.undo_index);
  str = XmStringCreateLocalized((char *) &tempstr);
  button = XtNameToWidget(editpal_options_dialog,
    "*coloredit_edit_menu*button_1");
  if(button != NULL)
    XtVaSetValues(button, XmNlabelString, str, NULL);
  XmStringFree(str);

  sprintf((char *) &tempstr,
    "Redo last undo (%5d redo's available)           ",
     paledit.redo_index);
  str = XmStringCreateLocalized((char *) &tempstr);
  button = XtNameToWidget(editpal_options_dialog,
    "*coloredit_edit_menu*button_2");
  if(button != NULL)
    XtVaSetValues(button, XmNlabelString, str, NULL);
  XmStringFree(str);

  XtAddEventHandler(paledit.palette_colorbox, ButtonPressMask,
    False, (XtEventHandler) editpal_colorbox_cursor, NULL);

  XtAddEventHandler(paledit.palette_colorbox, PointerMotionMask,
    False, (XtEventHandler) editpal_pointer_motion, NULL);

  XtAddEventHandler(canvas, ButtonPressMask,
    False, (XtEventHandler) editpal_place_cursor, NULL);

  XtAddEventHandler(canvas, PointerMotionMask,
    False, (XtEventHandler) editpal_pointer_motion, NULL);

  XtManageChild(editpal_options_dialog);
  XtManageChild(paledit.edit_palette_dialog);
  XtPopup(paledit.edit_palette_dialog, XtGrabNone);

  paledit.popped_up = 1;

  if(paledit.primary_active_colorbox_gc)
    XFreeGC(display, paledit.primary_active_colorbox_gc);

  if(paledit.secondary_active_colorbox_gc)
    XFreeGC(display, paledit.secondary_active_colorbox_gc);

  if(paledit.palette_colorbox_gc)
    XFreeGC(display, paledit.palette_colorbox_gc);

  if(privatecolor)
    XSetWindowColormap(display,
      XtWindow(paledit.edit_palette_dialog), colormap);

  paledit.primary_active_colorbox_gc = XCreateGC(XtDisplay(canvas),
    XtWindow(paledit.primary_active_colorbox), 0, NULL);

  paledit.secondary_active_colorbox_gc = XCreateGC(XtDisplay(canvas),
    XtWindow(paledit.secondary_active_colorbox), 0, NULL);

  paledit.palette_colorbox_gc = XCreateGC(XtDisplay(canvas),
    XtWindow(paledit.palette_colorbox), 0, NULL);

  redraw_primary_colorbox(editpal_options_dialog, NULL, NULL);
  redraw_secondary_colorbox(editpal_options_dialog, NULL, NULL);
  redraw_palette_colorbox(editpal_options_dialog, NULL, NULL);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  toggle_zoom_grab(ZOOM_OFF);
}

/*ARGSUSED*/
static void coloredit_edit_menu_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[128];
  Widget button;
  XmString str;
  int item_no;
  int n;

  sprintf((char *) &tempstr, "%d", (char *)client_data);
  item_no = atoi((char *) &tempstr);

  switch(item_no)
    {
    case 0:
      if(mapdacbox)
        {
        memcpy((char *)dacbox,mapdacbox,768);
        spindac(0,0);
        colorstate = 0;
        }
      else if(((char *) &MAP_name) != NULL &&
             ((int)(strlen((char *) &MAP_name)) != 0))
        load_color_map(main_window, (char *) &MAP_name);
      else
        load_color_map(main_window, "default.map");

      writevideopalette();

      for(n = 0; n < colors; n++)
        {
        freestyle.dirty[n] = 0;
        paledit.palette_colorbox_rect[n].red = dacbox[n][0];
        paledit.palette_colorbox_rect[n].blue = dacbox[n][2];
        paledit.palette_colorbox_rect[n].green = dacbox[n][1];
        paledit.palette_colorbox_rect[n].intred = (int)cols[n].red / 256;
        paledit.palette_colorbox_rect[n].intblue = (int)cols[n].blue / 256;
        paledit.palette_colorbox_rect[n].intgreen = (int)cols[n].green / 256;
        }

      if(undo_max > 0)
        {
        paledit.undo_index = 0;
        paledit.redo_index = 0;
        memset(undobox, 0, (sizeof(unsigned char) * undo_max * 256 * 3));
        memcpy(undobox + (paledit.undo_index * 768), dacbox, 768);
        }

      sprintf((char *) &tempstr,
        "Undo last edit (%5d undo's available)           ",
         paledit.undo_index);
      str = XmStringCreateLocalized((char *) &tempstr);
      button = XtNameToWidget(editpal_options_dialog,
        "*coloredit_edit_menu*button_1");
      if(button != NULL)
        XtVaSetValues(button, XmNlabelString, str, NULL);
      XmStringFree(str);

      sprintf((char *) &tempstr,
        "Redo last undo (%5d redo's available)           ",
         paledit.redo_index);
      str = XmStringCreateLocalized((char *) &tempstr);
      button = XtNameToWidget(editpal_options_dialog,
        "*coloredit_edit_menu*button_2");
      if(button != NULL)
        XtVaSetValues(button, XmNlabelString, str, NULL);
      XmStringFree(str);
    break;

    case 1:
      undo_last_color_edit();
    break;

    case 2:
      redo_last_color_edit();
    break;

    case 3:
      copy_inactive_to_active();
    break;

    case 4:
      copy_active_to_inactive();
    break;

    case 5:
      swaprgb(0);
    break;

    case 6:
      swaprgb(1);
    break;

    case 7:
      swaprgb(2);
    break;

    case 8:
      invertrgb(0);
    break;

    case 9:
      set_gamma();
    break;
    }
}

/*ARGSUSED*/
static void coloredit_range_menu_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int item_no;
  char tempstr[4];

  sprintf((char *) &tempstr, "%d", (char *)client_data);
  item_no = atoi((char *) &tempstr);

  switch(item_no)
    {
    case 0:
      unfreeze_inactive_register();
    break;

    case 1:
      toggle_range_mode();
    break;

    case 2:
      copy_range();
    break;

    case 3:
      paste_range(0);
    break;

    case 4:
      paste_range(1);
    break;

    case 5:
      range_smooth(0);
    break;

    case 6:
      get_skip_val(paledit.edit_palette_dialog);
    break;

    case 7:
      swaprgbrange(0);
    break;

    case 8:
      swaprgbrange(1);
    break;

    case 9:
      swaprgbrange(2);
    break;

    case 10:
      invertrgb(1);
    break;

    case 11:
      set_brightness(1);
    break;

    case 12:
      set_brightness(0);
    break;
    }
}

/*ARGSUSED*/
static void editpal_help_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPEDITPAL;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void editpal_colorbox_cursor(
Widget w,
XtPointer client_data,
XEvent *event)
{
  XButtonPressedEvent *bevent = (XButtonPressedEvent *) event;
  int j, n, color;

  if(bevent->button != 1 && bevent->button != 2)
    return;

  color = ptr_in_rect(bevent->x, bevent->y);
  if(color >= 0 && color < colors)
    {
    if(bevent->button == 2)
      freeze_inactive_register(color);
    else
      {
      load_active_register(color);

      if(paledit.editpal_canvas_cursor.active)
        {
        XDrawArc(display, XtWindow(canvas), xgc,
          paledit.editpal_canvas_cursor.old_x - 6,
          paledit.editpal_canvas_cursor.old_y - 6,
          12, 12, 23040, 23040);
        paledit.editpal_canvas_cursor.active = 0;
        }

      if(paledit.freestyle_mode == 1)
        {
        if(freestyle.wraparound == 0)
          {
          freestyle.range_start =
            (color - freestyle.range < 0 ? 0 : color - freestyle.range);

          freestyle.range_stop =
            (color + freestyle.range > colors - 1 ?
             colors - 1 : color + freestyle.range);
          }
        else
          {
          freestyle.range_start =
            (color - freestyle.range < 0 ?
            (colors + (color - freestyle.range)) :
            color - freestyle.range);

          freestyle.range_stop =
            (color + freestyle.range > colors - 1 ?
            (freestyle.range - (colors - color)) :
            color + freestyle.range);
          }

        for(n = freestyle.range_start, j = 0;
          j < ((freestyle.range * 2) + 1); n++, j++)
          {
          if(freestyle.wraparound == 1)
            {
            if(n > (colors - 1))
              n = 0;
            }
          else
            {
            if(n > (colors - 1))
              break;
            }
          freestyle.dirty[n] = 0;
          paledit.palette_colorbox_rect[n].red = dacbox[n][0];
          paledit.palette_colorbox_rect[n].blue = dacbox[n][2];
          paledit.palette_colorbox_rect[n].green = dacbox[n][1];
          paledit.palette_colorbox_rect[n].intred = (int)cols[n].red / 256;
          paledit.palette_colorbox_rect[n].intblue = (int)cols[n].blue / 256;
          paledit.palette_colorbox_rect[n].intgreen = (int)cols[n].green / 256;
          }
        if(freestyle_shell_popped_up == 1)
          redraw_freestyle_colorbox(main_window, NULL, NULL);
        update_undo_index();
        }
      }
    }
}

/*ARGSUSED*/
static void redraw_primary_colorbox(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char num[4];
  XmString str;
  int color = 0;

  if(paledit.active_color_register == 0)
    color = paledit.active_color_register_color;
  else if(paledit.active_color_register == 1)
    color = paledit.inactive_color_register_color;

  sprintf((char *) &num, "%3d", color);
  str = XmStringCreateLocalized((char *) &num);
  XtVaSetValues(paledit.primary_colorbox_label,
    XmNlabelString, str,
    NULL);
  XmStringFree(str);

  XtVaSetValues(paledit.primary_active_colorbox,
    XmNx, 90,
    XmNy, 114,
    XmNwidth, 60,
    XmNheight, 25,
    NULL);

  XSetForeground(display, paledit.primary_active_colorbox_gc,
    BlackPixelOfScreen(screen));

  XDrawRectangle(display, XtWindow(paledit.primary_active_colorbox),
    paledit.primary_active_colorbox_gc, 0, 0, 59, 24);

  XSetForeground(display, paledit.primary_active_colorbox_gc, pixtab[color]);

  XFillRectangle(display, XtWindow(paledit.primary_active_colorbox),
    paledit.primary_active_colorbox_gc, 0, 0, 59, 24);

  XSetForeground(display, paledit.primary_active_colorbox_gc,
    WhitePixelOfScreen(screen));
}

/*ARGSUSED*/
static void redraw_secondary_colorbox(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int color = 0;

  XSetForeground(display, paledit.secondary_active_colorbox_gc,
    BlackPixelOfScreen(screen));

  XDrawRectangle(display, XtWindow(paledit.secondary_active_colorbox),
    paledit.secondary_active_colorbox_gc, 0, 0, 59, 24);

  if(paledit.active_color_register == 1)
    color = paledit.active_color_register_color;
  else if(paledit.active_color_register == 0)
    color = paledit.inactive_color_register_color;

  XSetForeground(display, paledit.secondary_active_colorbox_gc, pixtab[color]);

  XFillRectangle(display, XtWindow(paledit.secondary_active_colorbox),
    paledit.secondary_active_colorbox_gc, 0, 0, 59, 24);

  XSetForeground(display, paledit.secondary_active_colorbox_gc,
      WhitePixelOfScreen(screen));
}

/*ARGSUSED*/
void redraw_palette_colorbox(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i, j, n;
  int current_x;
  int current_y;
  int index;
  int color_rows;
  XGCValues values;
  Widget inactive;
  Widget inactive_label;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;
  Widget active;
  Widget active_label;
  Widget active_red;
  Widget active_green;
  Widget active_blue;
  GC inactive_gc;
  GC active_gc;

  n = 0;
  current_x = 1;
  current_y = 1;

  XSetForeground(display, paledit.palette_colorbox_gc,
    BlackPixelOfScreen(screen));

  XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
    paledit.palette_colorbox_gc, 0, 0, 337, 337);

  color_rows = (colors >= 16 ? colors / 16 : 1);

  for(i = 0; i < color_rows; i++)
    {
    for(j = 0; j < 16; j++)
      {
      paledit.palette_colorbox_rect[n].x = current_x;
      paledit.palette_colorbox_rect[n].y = current_y;
      paledit.palette_colorbox_rect[n].width = 20;
      paledit.palette_colorbox_rect[n].height = 20;
      paledit.palette_colorbox_rect[n].color = pixtab[n];
      paledit.palette_colorbox_rect[n].red = dacbox[n][0];
      paledit.palette_colorbox_rect[n].blue = dacbox[n][2];
      paledit.palette_colorbox_rect[n].green = dacbox[n][1];
      paledit.palette_colorbox_rect[n].intred = (int)cols[n].red / 256;
      paledit.palette_colorbox_rect[n].intblue = (int)cols[n].blue / 256;
      paledit.palette_colorbox_rect[n].intgreen = (int)cols[n].green / 256;

      XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
        paledit.palette_colorbox_gc,
        paledit.palette_colorbox_rect[n].x,
        paledit.palette_colorbox_rect[n].y,
        paledit.palette_colorbox_rect[n].width,
        paledit.palette_colorbox_rect[n].height);

      n++;
      current_x += 21;
      }
    current_x = 1;
    current_y += 21;
    }

  for(n = 0; n < colors; n++)
    {
    XSetForeground(display, paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[n].color);

    XFillRectangle(display, XtWindow(paledit.palette_colorbox),
      paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[n].x + 1,
      paledit.palette_colorbox_rect[n].y + 1,
      paledit.palette_colorbox_rect[n].width - 1,
      paledit.palette_colorbox_rect[n].height - 1);
    }

  XSetForeground(display, paledit.palette_colorbox_gc,
      WhitePixelOfScreen(screen));

  if(paledit.pal_range.start_index >= 0)
    {
    index = paledit.pal_range.start_index;
    XGetGCValues(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

    values.line_style = LineOnOffDash;

    XChangeGC(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

    XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
      paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[index].x,
      paledit.palette_colorbox_rect[index].y,
      paledit.palette_colorbox_rect[index].width,
      paledit.palette_colorbox_rect[index].height);

    values.line_style = LineSolid;

    XChangeGC(display, paledit.palette_colorbox_gc, GCLineStyle, &values);
    }

  if(paledit.pal_range.stop_index >= 0)
    {
    index = paledit.pal_range.stop_index;
    XGetGCValues(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

    values.line_style = LineSolid;

    XChangeGC(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

    XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
      paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[index].x,
      paledit.palette_colorbox_rect[index].y,
      paledit.palette_colorbox_rect[index].width,
      paledit.palette_colorbox_rect[index].height);
    }

  if(paledit.active_color_register == 0)
    {
    active = paledit.primary_active_colorbox;
    active_label = paledit.primary_colorbox_label;
    active_gc = paledit.primary_active_colorbox_gc;
    active_red = paledit.primary_red_scale;
    active_green = paledit.primary_green_scale;
    active_blue = paledit.primary_blue_scale;
    }
  else
    {
    active = paledit.secondary_active_colorbox;
    active_label = paledit.secondary_colorbox_label;
    active_gc = paledit.secondary_active_colorbox_gc;
    active_red = paledit.secondary_red_scale;
    active_green = paledit.secondary_green_scale;
    active_blue = paledit.secondary_blue_scale;
    }

  XmScaleSetValue(active_red,
    paledit.palette_colorbox_rect
    [paledit.active_color_register_color].intred);
  XmScaleSetValue(active_green,
    paledit.palette_colorbox_rect
    [paledit.active_color_register_color].intgreen);
  XmScaleSetValue(active_blue,
    paledit.palette_colorbox_rect
    [paledit.active_color_register_color].intblue);

  if(paledit.active_color_register == 1)
    {
    inactive = paledit.primary_active_colorbox;
    inactive_label = paledit.primary_colorbox_label;
    inactive_gc = paledit.primary_active_colorbox_gc;
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive = paledit.secondary_active_colorbox;
    inactive_label = paledit.secondary_colorbox_label;
    inactive_gc = paledit.secondary_active_colorbox_gc;
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  XmScaleSetValue(inactive_red,
    paledit.palette_colorbox_rect
    [paledit.inactive_color_register_color].intred);
  XmScaleSetValue(inactive_green,
    paledit.palette_colorbox_rect
    [paledit.inactive_color_register_color].intgreen);
  XmScaleSetValue(inactive_blue,
    paledit.palette_colorbox_rect
    [paledit.inactive_color_register_color].intblue);

  if(freestyle_shell_popped_up == 1)
    redraw_freestyle_colorbox(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void toggle_active_register(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmString actstr;
  XmString inactstr;

  int which = atoi((char *) client_data);

  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;

  int wasactive = paledit.active_color_register;
  int old_active_color = paledit.active_color_register_color;
  int old_inactive_color = paledit.inactive_color_register_color;

  actstr = XmStringCreateLocalized("Active");
  inactstr = XmStringCreateLocalized("Inactive");

  if(which == 0)
    { /* primary */
    if(cbs->set == False && paledit.active_color_register == 0)
      {
      XmToggleButtonSetState(paledit.primary_active_button, False, False);
      XmToggleButtonSetState(paledit.secondary_active_button, True, False);
      paledit.active_color_register = 1;
      }
    else if(cbs->set == False && paledit.active_color_register == 1)
      {
      XmToggleButtonSetState(paledit.primary_active_button, False, False);
      XmToggleButtonSetState(paledit.secondary_active_button, True, False);
      paledit.active_color_register = 1;
      }
    else if(cbs->set == True && paledit.active_color_register == 0)
      {
      XmToggleButtonSetState(paledit.primary_active_button, True, False);
      XmToggleButtonSetState(paledit.secondary_active_button, False, False);
      paledit.active_color_register = 0;
      }
    else if(cbs->set == True && paledit.active_color_register == 1)
      {
      XmToggleButtonSetState(paledit.primary_active_button, True, False);
      XmToggleButtonSetState(paledit.secondary_active_button, False, False);
      paledit.active_color_register = 0;
      }
    }
  else
    { /* secondary */
    if(cbs->set == False && paledit.active_color_register == 1)
      {
      XmToggleButtonSetState(paledit.primary_active_button, True, False);
      XmToggleButtonSetState(paledit.secondary_active_button, False, False);
      paledit.active_color_register = 0;
      }
    else if(cbs->set == False && paledit.active_color_register == 0)
      {
      XmToggleButtonSetState(paledit.primary_active_button, True, False);
      XmToggleButtonSetState(paledit.secondary_active_button, False, False);
      paledit.active_color_register = 0;
      }
    else if(cbs->set == True && paledit.active_color_register == 0)
      {
      XmToggleButtonSetState(paledit.primary_active_button, False, False);
      XmToggleButtonSetState(paledit.secondary_active_button, True, False);
      paledit.active_color_register = 1;
      }
    else if(cbs->set == True && paledit.active_color_register == 1)
      {
      XmToggleButtonSetState(paledit.primary_active_button, False, False);
      XmToggleButtonSetState(paledit.secondary_active_button, True, False);
      paledit.active_color_register = 1;
      }
    }

  if(wasactive != paledit.active_color_register)
    {
    paledit.active_color_register_color = old_active_color;
    paledit.inactive_color_register_color = old_inactive_color;
    load_active_register(paledit.active_color_register_color);
    load_inactive_register(paledit.inactive_color_register_color);
    }

  if(paledit.active_color_register == 0)
    {
    XtVaSetValues(paledit.primary_active_button,
      XmNlabelString, actstr,
      NULL);
    XtVaSetValues(paledit.secondary_active_button,
      XmNlabelString, inactstr,
      NULL);
    }
  else
    {
    XtVaSetValues(paledit.primary_active_button,
      XmNlabelString, inactstr,
      NULL);
    XtVaSetValues(paledit.secondary_active_button,
      XmNlabelString, actstr,
      NULL);
    }

  XtVaSetValues(paledit.primary_active_colorbox,
    XmNx, 90,
    XmNy, 114,
    XmNwidth, 60,
    XmNheight, 25,
    NULL);

  XtVaSetValues(paledit.secondary_active_colorbox,
    XmNx, 90,
    XmNy, 114,
    XmNwidth, 60,
    XmNheight, 25,
    NULL);

  XmStringFree(actstr);
  XmStringFree(inactstr);
}

/*ARGSUSED*/
static void editpal_place_cursor(
Widget w,
XtPointer client_data,
XEvent *event)
{
  XButtonPressedEvent *bevent = (XButtonPressedEvent *)event;
  int j, n, color;

  if(bevent->button != 1 ||
    (calc_status != IMG_PRMS_CHGD && calc_status != IMG_COMPLETED))
    return;

  /*
   * Draw a circle with a bounding rect of 12 x 12 pixels,
   * erasing the old one first, if it exists.
   */

  if(paledit.editpal_canvas_cursor.active)
    {
    XDrawArc(display, XtWindow(canvas), xgc,
      paledit.editpal_canvas_cursor.old_x - 6,
      paledit.editpal_canvas_cursor.old_y - 6,
      12, 12, 23040, 23040);

    paledit.editpal_canvas_cursor.old_x = bevent->x;
    paledit.editpal_canvas_cursor.old_y = bevent->y;

    XDrawArc(display, XtWindow(canvas), xgc,
      bevent->x - 6,
      bevent->y - 6,
      12, 12, 23040, 23040);
    }
  else
    {
    paledit.editpal_canvas_cursor.active = 1;
    paledit.editpal_canvas_cursor.old_x = bevent->x;
    paledit.editpal_canvas_cursor.old_y = bevent->y;

    XDrawArc(display, XtWindow(canvas), xgc,
      bevent->x - 6,
      bevent->y - 6,
      12, 12, 23040, 23040);
    }

  color = getcolor(bevent->x, bevent->y);

  if(color >= 0 && color < colors)
    load_active_register(color);

  if(paledit.freestyle_mode == 1)
    {
    if(freestyle.wraparound == 0)
      {
      freestyle.range_start =
        (color - freestyle.range < 0 ? 0 : color - freestyle.range);

      freestyle.range_stop =
        (color + freestyle.range > colors - 1 ?
        colors - 1 : color + freestyle.range);
      }
    else
      {
      freestyle.range_start =
          (color - freestyle.range < 0 ?
          (colors + (color - freestyle.range)) :
          color - freestyle.range);

        freestyle.range_stop =
          (color + freestyle.range > colors - 1 ?
          (freestyle.range - (colors - color)) :
          color + freestyle.range);
      }

    for(n = freestyle.range_start, j = 0;
      j < ((freestyle.range * 2) + 1); n++, j++)
      {
      if(freestyle.wraparound == 1)
        if(n > (colors - 1))
          n = 0;
      freestyle.dirty[n] = 0;
      paledit.palette_colorbox_rect[n].red = dacbox[n][0];
      paledit.palette_colorbox_rect[n].blue = dacbox[n][2];
      paledit.palette_colorbox_rect[n].green = dacbox[n][1];
      paledit.palette_colorbox_rect[n].intred = (int)cols[n].red / 256;
      paledit.palette_colorbox_rect[n].intblue = (int)cols[n].blue / 256;
      paledit.palette_colorbox_rect[n].intgreen = (int)cols[n].green / 256;
      }
    }
}

static void load_active_register(int color)
{
  Widget active;
  Widget active_label;
  Widget active_red;
  Widget active_green;
  Widget active_blue;
  XGCValues values;
  GC active_gc;
  XmString str;
  char num[4];

  if(paledit.active_color_register == 0)
    {
    active = paledit.primary_active_colorbox;
    active_label = paledit.primary_colorbox_label;
    active_gc = paledit.primary_active_colorbox_gc;
    active_red = paledit.primary_red_scale;
    active_green = paledit.primary_green_scale;
    active_blue = paledit.primary_blue_scale;
    }
  else
    {
    active = paledit.secondary_active_colorbox;
    active_label = paledit.secondary_colorbox_label;
    active_gc = paledit.secondary_active_colorbox_gc;
    active_red = paledit.secondary_red_scale;
    active_green = paledit.secondary_green_scale;
    active_blue = paledit.secondary_blue_scale;
    }

  XSetForeground(display, active_gc, pixtab[color]);
    XFillRectangle(display, XtWindow(active),
    active_gc, 0, 0, 59, 24);

  if(paledit.palette_colorbox_rect[color].intred > 255)
    paledit.palette_colorbox_rect[color].intred = 255;

  if(paledit.palette_colorbox_rect[color].intblue > 255)
    paledit.palette_colorbox_rect[color].intblue = 255;

  if(paledit.palette_colorbox_rect[color].intgreen > 255)
    paledit.palette_colorbox_rect[color].intgreen = 255;

  XmScaleSetValue(active_red,
    paledit.palette_colorbox_rect[color].intred);

  XmScaleSetValue(active_green,
    paledit.palette_colorbox_rect[color].intgreen);

  XmScaleSetValue(active_blue,
    paledit.palette_colorbox_rect[color].intblue);

  sprintf((char *) &num, "%3d", color);
  str = XmStringCreateLocalized((char *) &num);
  XtVaSetValues(active_label,
    XmNlabelString, str,
    NULL);
  XmStringFree(str);

  XtVaSetValues(active,
    XmNx, 90,
    XmNy, 114,
    XmNwidth, 60,
    XmNheight, 25,
    NULL);

  paledit.active_color_register_color = color;

  if(paledit.pal_range.start_index >= 0)
    {
    XSetForeground(display, paledit.palette_colorbox_gc,
      BlackPixelOfScreen(screen));

    XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
      paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[paledit.pal_range.start_index].x,
      paledit.palette_colorbox_rect[paledit.pal_range.start_index].y,
      paledit.palette_colorbox_rect[paledit.pal_range.start_index].width,
      paledit.palette_colorbox_rect[paledit.pal_range.start_index].height);

    XSetForeground(display, paledit.palette_colorbox_gc,
      WhitePixelOfScreen(screen));
    }

  XSetForeground(display, paledit.palette_colorbox_gc,
    BlackPixelOfScreen(screen));

  XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
    paledit.palette_colorbox_gc,
    paledit.palette_colorbox_rect[color].x,
    paledit.palette_colorbox_rect[color].y,
    paledit.palette_colorbox_rect[color].width,
    paledit.palette_colorbox_rect[color].height);

  XSetForeground(display, paledit.palette_colorbox_gc,
    WhitePixelOfScreen(screen));

  XGetGCValues(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

  values.line_style = LineOnOffDash;

  XChangeGC(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

  XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
    paledit.palette_colorbox_gc,
    paledit.palette_colorbox_rect[color].x,
    paledit.palette_colorbox_rect[color].y,
    paledit.palette_colorbox_rect[color].width,
    paledit.palette_colorbox_rect[color].height);

  values.line_style = LineSolid;

  XChangeGC(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

  paledit.pal_range.start_index = color;
}

static void load_inactive_register(int color)
{
  Widget inactive;
  Widget inactive_label;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;
  GC inactive_gc;
  XmString str;
  char num[4];

  if(paledit.active_color_register == 1)
    {
    inactive = paledit.primary_active_colorbox;
    inactive_label = paledit.primary_colorbox_label;
    inactive_gc = paledit.primary_active_colorbox_gc;
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive = paledit.secondary_active_colorbox;
    inactive_label = paledit.secondary_colorbox_label;
    inactive_gc = paledit.secondary_active_colorbox_gc;
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  sprintf((char *) &num, "%3d", color);
  str = XmStringCreateLocalized((char *) &num);
  XtVaSetValues(inactive_label,
    XmNlabelString, str,
    NULL);
  XmStringFree(str);

  XtVaSetValues(inactive,
    XmNx, 90,
    XmNy, 114,
    XmNwidth, 60,
    XmNheight, 25,
    NULL);

  XSetForeground(display, inactive_gc, pixtab[color]);
    XFillRectangle(display, XtWindow(inactive),
    inactive_gc, 0, 0, 59, 24);

  XSetForeground(display, inactive_gc,
    BlackPixelOfScreen(screen));

  if(paledit.palette_colorbox_rect[color].intred > 255)
    paledit.palette_colorbox_rect[color].intred = 255;

  if(paledit.palette_colorbox_rect[color].intblue > 255)
    paledit.palette_colorbox_rect[color].intblue = 255;

  if(paledit.palette_colorbox_rect[color].intgreen > 255)
    paledit.palette_colorbox_rect[color].intgreen = 255;

  XmScaleSetValue(inactive_red,
    paledit.palette_colorbox_rect[color].intred);

  XmScaleSetValue(inactive_green,
    paledit.palette_colorbox_rect[color].intgreen);

  XmScaleSetValue(inactive_blue,
    paledit.palette_colorbox_rect[color].intblue);

  paledit.inactive_color_register_color = color;
}

static void freeze_inactive_register(int color)
{
  int n;
  int done;
  char tempstr[64];
  Widget button;
  Widget inactive;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;
  XGCValues values;
  GC inactive_gc;

  if(paledit.freestyle_mode == 1)
    {
    freestyle_cleanup();
    paledit.freestyle_mode = 0;
    freestyle.wraparound = 0;
    if(freestyle_shell_popped_up == 1)
      {
      XmToggleButtonSetState(freestyle_mode_on_toggle, False, False);
      XmToggleButtonSetState(freestyle_mode_off_toggle, True, False);
      XmToggleButtonSetState(freestyle_wrap_on_toggle, False, False);
      XmToggleButtonSetState(freestyle_wrap_off_toggle, True, False);
      }
    }

  if(paledit.active_color_register == 1)
    {
    inactive = paledit.primary_active_colorbox;
    inactive_gc = paledit.primary_active_colorbox_gc;
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive = paledit.secondary_active_colorbox;
    inactive_gc = paledit.secondary_active_colorbox_gc;
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  XSetForeground(display, inactive_gc, pixtab[color]);
    XFillRectangle(display, XtWindow(inactive),
    inactive_gc, 0, 0, 59, 24);

  if(paledit.palette_colorbox_rect[color].intred > 255)
    paledit.palette_colorbox_rect[color].intred = 255;

  if(paledit.palette_colorbox_rect[color].intblue > 255)
    paledit.palette_colorbox_rect[color].intblue = 255;

  if(paledit.palette_colorbox_rect[color].intgreen > 255)
    paledit.palette_colorbox_rect[color].intgreen = 255;

  XmScaleSetValue(inactive_red,
    paledit.palette_colorbox_rect[color].intred);

  XmScaleSetValue(inactive_green,
    paledit.palette_colorbox_rect[color].intgreen);

  XmScaleSetValue(inactive_blue,
    paledit.palette_colorbox_rect[color].intblue);

  paledit.inactive_color_register_color = color;

  if(paledit.pal_range.stop_index >= 0)
    {
    XSetForeground(display, paledit.palette_colorbox_gc,
      BlackPixelOfScreen(screen));

    XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
      paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].x,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].y,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].width,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].height);

    XSetForeground(display, paledit.palette_colorbox_gc,
      WhitePixelOfScreen(screen));
    }

  XSetForeground(display, paledit.palette_colorbox_gc,
    BlackPixelOfScreen(screen));

  XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
    paledit.palette_colorbox_gc,
    paledit.palette_colorbox_rect[color].x,
    paledit.palette_colorbox_rect[color].y,
    paledit.palette_colorbox_rect[color].width,
    paledit.palette_colorbox_rect[color].height);

  XSetForeground(display, paledit.palette_colorbox_gc,
    WhitePixelOfScreen(screen));

  XGetGCValues(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

  values.line_style = LineSolid;

  XChangeGC(display, paledit.palette_colorbox_gc, GCLineStyle, &values);

  XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
    paledit.palette_colorbox_gc,
    paledit.palette_colorbox_rect[color].x,
    paledit.palette_colorbox_rect[color].y,
    paledit.palette_colorbox_rect[color].width,
    paledit.palette_colorbox_rect[color].height);

  paledit.pal_range.stop_index = color;

  paledit.inactive_frozen = 1;

  XtSetSensitive(coloredit_range_menu, True);

  done = n = 0;

  while(done == 0)
    {
    sprintf((char *) &tempstr, "*coloredit_range_menu*button_%d", n++);
    button = XtNameToWidget(editpal_options_dialog, (char *) &tempstr);
    if(button != NULL)
      XtSetSensitive(button, True);
    else
      done = 1;
    }
}

static void unfreeze_inactive_register()
{
  int n;
  int done;
  Widget button;
  char tempstr[64];

  if(paledit.pal_range.stop_index >= 0)
    {
    XSetForeground(display, paledit.palette_colorbox_gc,
      BlackPixelOfScreen(screen));

    XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
      paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].x,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].y,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].width,
      paledit.palette_colorbox_rect[paledit.pal_range.stop_index].height);

    XSetForeground(display, paledit.palette_colorbox_gc,
      WhitePixelOfScreen(screen));
    }

  for(n = 0; n < 256; n++)
    {
    paledit.pal_range.saved[n].intred = 0;
    paledit.pal_range.saved[n].intgreen = 0;
    paledit.pal_range.saved[n].intblue = 0;
    paledit.pal_range.saved[n].red = 0;
    paledit.pal_range.saved[n].green = 0;
    paledit.pal_range.saved[n].blue = 0;
    paledit.pal_range.saved[n].color = 0;
    }

  paledit.pal_range.num_saved = 0;
  paledit.pal_range.stop_index = -1;
  paledit.inactive_frozen = 0;

  XtSetSensitive(coloredit_range_menu, False);

  done = n = 0;

  while(done == 0)
    {
    sprintf((char *) &tempstr, "*coloredit_range_menu*button_%d", n++);
    button = XtNameToWidget(editpal_options_dialog, (char *) &tempstr);
    if(button != NULL)
      XtSetSensitive(button, False);
    else
      done = 1;
    }
}

/*ARGSUSED*/
static void editpal_pointer_motion(
Widget w,
XtPointer client_data,
XEvent *event)
{
  int color = 0;
  Widget inactive;
  Widget inactive_label;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;
  GC inactive_gc;
  XGCValues values;
  XmString str;
  char num[4];

  XMotionEvent *mevent = (XMotionEvent *) event;

  if(paledit.inactive_frozen)
    {
    return;
    }

  if(paledit.active_color_register == 1)
    {
    inactive = paledit.primary_active_colorbox;
    inactive_label = paledit.primary_colorbox_label;
    inactive_gc = paledit.primary_active_colorbox_gc;
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive = paledit.secondary_active_colorbox;
    inactive_label = paledit.secondary_colorbox_label;
    inactive_gc = paledit.secondary_active_colorbox_gc;
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  if(mevent->window == XtWindow(canvas))
    color = getcolor(mevent->x, mevent->y);
  else if(mevent->window == XtWindow(paledit.palette_colorbox))
    color = ptr_in_rect(mevent->x, mevent->y);

  if(color != freestyle.oldcolor)
    freestyle.oldcolor = color;
  else
    return;

  if(color >= 0 && color < colors)
    {
    sprintf((char *) &num, "%3d", color);
    str = XmStringCreateLocalized((char *) &num);
    XtVaSetValues(inactive_label,
      XmNlabelString, str,
      NULL);
    XmStringFree(str);

    XtVaSetValues(inactive,
      XmNx, 90,
      XmNy, 114,
      XmNwidth, 60,
      XmNheight, 25,
      NULL);

    XSetForeground(display, inactive_gc, pixtab[color]);

    XFillRectangle(display, XtWindow(inactive),
      inactive_gc, 0, 0, 59, 24);

    XSetForeground(display, inactive_gc,
      BlackPixelOfScreen(screen));

    if(paledit.oldrect >= 0)
      {
      XSetForeground(display, paledit.palette_colorbox_gc,
        BlackPixelOfScreen(screen));

      XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
        paledit.palette_colorbox_gc,
        paledit.palette_colorbox_rect[paledit.oldrect].x,
        paledit.palette_colorbox_rect[paledit.oldrect].y,
        paledit.palette_colorbox_rect[paledit.oldrect].width,
        paledit.palette_colorbox_rect[paledit.oldrect].height);

      if(paledit.oldrect == paledit.active_color_register_color)
        {
        XSetForeground(display, paledit.palette_colorbox_gc,
          WhitePixelOfScreen(screen));

        XGetGCValues(display, paledit.palette_colorbox_gc,
          GCLineStyle, &values);

        values.line_style = LineOnOffDash;

        XChangeGC(display, paledit.palette_colorbox_gc,
          GCLineStyle, &values);

        XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
          paledit.palette_colorbox_gc,
          paledit.palette_colorbox_rect[paledit.oldrect].x,
          paledit.palette_colorbox_rect[paledit.oldrect].y,
          paledit.palette_colorbox_rect[paledit.oldrect].width,
          paledit.palette_colorbox_rect[paledit.oldrect].height);

        XSetForeground(display, paledit.palette_colorbox_gc,
          BlackPixelOfScreen(screen));

        values.line_style = LineSolid;

        XChangeGC(display, paledit.palette_colorbox_gc,
          GCLineStyle, &values);
        }
      }

    paledit.oldrect = color;

    if(color == paledit.active_color_register_color)
      {
      XGetGCValues(display, paledit.palette_colorbox_gc,
        GCLineStyle, &values);

      values.line_style = LineOnOffDash;

      XChangeGC(display, paledit.palette_colorbox_gc,
        GCLineStyle, &values);
      }

    XSetForeground(display, paledit.palette_colorbox_gc,
      WhitePixelOfScreen(screen));

    XDrawRectangle(display, XtWindow(paledit.palette_colorbox),
      paledit.palette_colorbox_gc,
      paledit.palette_colorbox_rect[color].x,
      paledit.palette_colorbox_rect[color].y,
      paledit.palette_colorbox_rect[color].width,
      paledit.palette_colorbox_rect[color].height);

    XSetForeground(display, paledit.palette_colorbox_gc,
      BlackPixelOfScreen(screen));

    if(color == paledit.active_color_register_color)
      {
      values.line_style = LineSolid;
      XChangeGC(display, paledit.palette_colorbox_gc,
        GCLineStyle, &values);
      }

    if(paledit.palette_colorbox_rect[color].intred > 255)
      paledit.palette_colorbox_rect[color].intred = 255;

    if(paledit.palette_colorbox_rect[color].intblue > 255)
      paledit.palette_colorbox_rect[color].intblue = 255;

    if(paledit.palette_colorbox_rect[color].intgreen > 255)
      paledit.palette_colorbox_rect[color].intgreen = 255;

    XmScaleSetValue(inactive_red,
      paledit.palette_colorbox_rect[color].intred);

    XmScaleSetValue(inactive_green,
      paledit.palette_colorbox_rect[color].intgreen);

    XmScaleSetValue(inactive_blue,
      paledit.palette_colorbox_rect[color].intblue);

    paledit.inactive_color_register_color = color;

    if(paledit.freestyle_mode == 1)
      {
      int j, n;
      int hi, lo;
      float top_mult, bot_mult;
      int temp_start, temp_stop;

      if(freestyle.wraparound == 0)
        {
        freestyle.range_start =
          (color - freestyle.range < 0 ? 0 : color - freestyle.range);

        freestyle.range_stop =
          (color + freestyle.range > colors - 1 ?
           colors - 1 : color + freestyle.range);
        }
      else
        {
        freestyle.range_start =
          (color - freestyle.range < 0 ?
          (colors + (color - freestyle.range)) :
          color - freestyle.range);

        freestyle.range_stop =
          (color + freestyle.range > colors - 1 ?
          (freestyle.range - (colors - color)) :
          color + freestyle.range);

        for(n = 0; n < colors; n++)
          freestyle.active[n] = 0;
        }

      if(freestyle.endcolor_hi == -1 || freestyle.endcolor_lo == -1)
        {
        if(freestyle.endcolor_lo == -1)
          lo = freestyle.range_start;
        else
          lo = freestyle.endcolor_lo;

        if(freestyle.endcolor_hi == -1)
          hi = freestyle.range_stop;
        else
          hi = freestyle.endcolor_hi;

        freestyle_setcolors(lo, hi);
        }

      for(n = freestyle.range_start, j = 0;
        j < ((freestyle.range * 2) + 1); n++, j++)
        {
        if(freestyle.wraparound == 1)
          {
          if(n > (colors - 1))
            n = 0;
          freestyle.active[n] = 1;
          }
        else
          {
          if(n > (colors - 1))
            break;
          }

        freestyle.dirty[n] = 1;

        merge_color[0] = (unsigned char)
          ((float)(((int)paledit.palette_colorbox_rect[n].red *
            freestyle.botmult)) + ((float)((int)freestyle.gamma_rect[j].red *
            freestyle.topmult)));
        merge_color[1] = (unsigned char)
          ((float)(((int)paledit.palette_colorbox_rect[n].green *
            freestyle.botmult)) + ((float)((int)freestyle.gamma_rect[j].green *
            freestyle.topmult)));
        merge_color[2] = (unsigned char)
          ((float)(((int)paledit.palette_colorbox_rect[n].blue *
            freestyle.botmult)) + ((float)((int)freestyle.gamma_rect[j].blue *
            freestyle.topmult)));

        dacbox[n][0] = merge_color[0];
        dacbox[n][1] = merge_color[1];
        dacbox[n][2] = merge_color[2];

        cols[n].red = dacbox[n][0] * 256;
        cols[n].blue = dacbox[n][2] * 256;
        cols[n].green = dacbox[n][1] * 256;

        XStoreColor(display, colormap, &cols[n]);

        paledit.palette_colorbox_rect[n].intred = (int)cols[n].red / 256;
        paledit.palette_colorbox_rect[n].intblue = (int)cols[n].blue / 256;
        paledit.palette_colorbox_rect[n].intgreen = (int)cols[n].green / 256;

        if(n == paledit.active_color_register_color)
          {
          if(paledit.active_color_register == 0)
            {
            XmScaleSetValue(paledit.primary_red_scale,
              paledit.palette_colorbox_rect[n].intred);
            XmScaleSetValue(paledit.primary_blue_scale,
              paledit.palette_colorbox_rect[n].intblue);
            XmScaleSetValue(paledit.primary_green_scale,
              paledit.palette_colorbox_rect[n].intgreen);
            }
          else
            {
            XmScaleSetValue(paledit.secondary_red_scale,
              paledit.palette_colorbox_rect[n].intred);
            XmScaleSetValue(paledit.secondary_blue_scale,
              paledit.palette_colorbox_rect[n].intblue);
            XmScaleSetValue(paledit.secondary_green_scale,
              paledit.palette_colorbox_rect[n].intgreen);
            }
          }
        }

      for(n = 0; n < colors; n++)
        {
        if(freestyle.wraparound == 0)
          {
          if(n < freestyle.range_start || n > freestyle.range_stop)
            {
            if(freestyle.dirty[n] == 1)
              {
              dacbox[n][0] = paledit.palette_colorbox_rect[n].red;
              dacbox[n][2] = paledit.palette_colorbox_rect[n].blue;
              dacbox[n][1] = paledit.palette_colorbox_rect[n].green;

              cols[n].red = dacbox[n][0] * 256;
              cols[n].blue = dacbox[n][2] * 256;
              cols[n].green = dacbox[n][1] * 256;

              XStoreColor(display, colormap, &cols[n]);

              paledit.palette_colorbox_rect[n].intred =
                (int)cols[n].red / 256;
              paledit.palette_colorbox_rect[n].intblue =
                (int)cols[n].blue / 256;
              paledit.palette_colorbox_rect[n].intgreen =
                (int)cols[n].green / 256;

              if(n == freestyle.color)
                {
                if(freestyle_shell_popped_up == 1)
                  redraw_freestyle_colorbox(main_window, NULL, NULL);
                }

              if(n == paledit.active_color_register_color)
                {
                if(paledit.active_color_register == 0)
                  {
                  XmScaleSetValue(paledit.primary_red_scale,
                    paledit.palette_colorbox_rect[n].intred);
                  XmScaleSetValue(paledit.primary_blue_scale,
                    paledit.palette_colorbox_rect[n].intblue);
                  XmScaleSetValue(paledit.primary_green_scale,
                    paledit.palette_colorbox_rect[n].intgreen);
                  }
                else
                  {
                  XmScaleSetValue(paledit.secondary_red_scale,
                    paledit.palette_colorbox_rect[n].intred);
                  XmScaleSetValue(paledit.secondary_blue_scale,
                    paledit.palette_colorbox_rect[n].intblue);
                  XmScaleSetValue(paledit.secondary_green_scale,
                    paledit.palette_colorbox_rect[n].intgreen);
                  }
                }
              freestyle.dirty[n] = 0;
              }
            }
          }
        else
          {
          if(freestyle.active[n] != 1)
            {
            if(freestyle.dirty[n] == 1)
              {
              dacbox[n][0] = paledit.palette_colorbox_rect[n].red;
              dacbox[n][2] = paledit.palette_colorbox_rect[n].blue;
              dacbox[n][1] = paledit.palette_colorbox_rect[n].green;

              cols[n].red = dacbox[n][0] * 256;
              cols[n].blue = dacbox[n][2] * 256;
              cols[n].green = dacbox[n][1] * 256;

              XStoreColor(display, colormap, &cols[n]);

              paledit.palette_colorbox_rect[n].intred =
                (int)cols[n].red / 256;
              paledit.palette_colorbox_rect[n].intblue =
                (int)cols[n].blue / 256;
              paledit.palette_colorbox_rect[n].intgreen =
                (int)cols[n].green / 256;

              if(n == freestyle.color)
                {
                if(freestyle_shell_popped_up == 1)
                  redraw_freestyle_colorbox(main_window, NULL, NULL);
                }

              if(n == paledit.active_color_register_color)
                {
                if(paledit.active_color_register == 0)
                  {
                  XmScaleSetValue(paledit.primary_red_scale,
                    paledit.palette_colorbox_rect[n].intred);
                  XmScaleSetValue(paledit.primary_blue_scale,
                    paledit.palette_colorbox_rect[n].intblue);
                  XmScaleSetValue(paledit.primary_green_scale,
                    paledit.palette_colorbox_rect[n].intgreen);
                  }
                else
                  {
                  XmScaleSetValue(paledit.secondary_red_scale,
                    paledit.palette_colorbox_rect[n].intred);
                  XmScaleSetValue(paledit.secondary_blue_scale,
                    paledit.palette_colorbox_rect[n].intblue);
                  XmScaleSetValue(paledit.secondary_green_scale,
                    paledit.palette_colorbox_rect[n].intgreen);
                  }
                }
              freestyle.dirty[n] = 0;
              }
            }
          }
        }
      }
    }
  else
    return;
}

static int ptr_in_rect(int x, int y)
{
  int i, j;
  int gotx, goty;

  for(i = 0, gotx = 0; i < 16; i++)
    {
    if(x > paledit.palette_colorbox_rect[i].x + 1 &&
       x < paledit.palette_colorbox_rect[i].x +
           paledit.palette_colorbox_rect[i].width - 1)
      {
      gotx = 1;
      break;
      }
    }

  for(j = 0, goty = 0; j < 16; j++)
    {
    if(y > paledit.palette_colorbox_rect[j * 16].y + 1 &&
       y < paledit.palette_colorbox_rect[j * 16].y +
           paledit.palette_colorbox_rect[j * 16].height - 1)
      {
      goty = 1;
      break;
      }
    }

  return(gotx && goty ? (j * 16) + i : -1);
}

/*ARGSUSED*/
static void edit_active_color(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int which = atoi((char *) client_data);
  XmScaleCallbackStruct *scs = (XmScaleCallbackStruct *)call_data;

  if(which != paledit.active_color_register)
    return;
  else
    {
    if(which == 0)
      {
      if(w == paledit.primary_red_scale)
        dacbox[paledit.active_color_register_color][0] =
          (unsigned char) scs->value;
      else if(w == paledit.primary_blue_scale)
        dacbox[paledit.active_color_register_color][2] =
          (unsigned char) scs->value;
      else if(w == paledit.primary_green_scale)
        dacbox[paledit.active_color_register_color][1] =
          (unsigned char) scs->value;

      cols[paledit.active_color_register_color].red =
        dacbox[paledit.active_color_register_color][0] * 256;
      cols[paledit.active_color_register_color].blue =
        dacbox[paledit.active_color_register_color][2] * 256;
      cols[paledit.active_color_register_color].green =
        dacbox[paledit.active_color_register_color][1] * 256;

      XStoreColor(display, colormap,
        &cols[paledit.active_color_register_color]);

      change_xor_gc(0);

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].red =
        dacbox[paledit.active_color_register_color][0];

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].blue =
        dacbox[paledit.active_color_register_color][2];

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].green =
        dacbox[paledit.active_color_register_color][1];

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].intred =
        (int)cols[paledit.active_color_register_color].red / 256;

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].intblue =
        (int)cols[paledit.active_color_register_color].blue / 256;

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].intgreen =
        (int)cols[paledit.active_color_register_color].green / 256;
      }
    else
      {
      if(w == paledit.secondary_red_scale)
        dacbox[paledit.active_color_register_color][0] =
          (unsigned char) scs->value;
      else if(w == paledit.secondary_blue_scale)
        dacbox[paledit.active_color_register_color][2] =
          (unsigned char) scs->value;
      else if(w == paledit.secondary_green_scale)
        dacbox[paledit.active_color_register_color][1] =
          (unsigned char) scs->value;

      cols[paledit.active_color_register_color].red =
        dacbox[paledit.active_color_register_color][0] * 256;
      cols[paledit.active_color_register_color].blue =
        dacbox[paledit.active_color_register_color][2] * 256;
      cols[paledit.active_color_register_color].green =
        dacbox[paledit.active_color_register_color][1] * 256;

      XStoreColor(display, colormap,
        &cols[paledit.active_color_register_color]);

      change_xor_gc(0);

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].red =
        dacbox[paledit.active_color_register_color][0];

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].blue =
        dacbox[paledit.active_color_register_color][2];

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].green =
        dacbox[paledit.active_color_register_color][1];

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].intred =
        (int)cols[paledit.active_color_register_color].red / 256;

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].intgreen =
        (int)cols[paledit.active_color_register_color].green / 256;

      paledit.palette_colorbox_rect
        [paledit.active_color_register_color].intblue =
        (int)cols[paledit.active_color_register_color].blue / 256;
      }

    if(paledit.active_color_register_color == freestyle.color)
      {
      if(freestyle_shell_popped_up == 1)
        redraw_freestyle_colorbox(main_window, NULL, NULL);
      }
    if(scs->reason == XmCR_VALUE_CHANGED)
      update_undo_index();
    }
}

/*ARGSUSED*/
static void editpal_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Widget button;
  XmString str;
  char tempstr[128];

  int do_reset_color = atoi((char *)client_data);

  if(do_reset_color == 2 || do_reset_color == 0)
    {
    if(mapdacbox)
      {
      memcpy((char *)mapdacbox,dacbox,768);
      spindac(0,0);
      colorstate = 0;
      }
    }

  if(do_reset_color == 1)
    {
    if(mapdacbox)
      {
      memcpy((char *)dacbox,mapdacbox,768);
      spindac(0,0);
      colorstate = 0;
      }
    else if(((char *) &MAP_name) != NULL &&
           ((int)(strlen((char *) &MAP_name)) != 0))
      load_color_map(main_window, (char *) &MAP_name);
    else
      load_color_map(main_window, "default.map");
    }

  create_xor_gc();

  if(do_reset_color != 2)
    {
    editpal_cursor = 0;
    paledit.oldrect = -1;
    paledit.popped_up = 0;
    paledit.freestyle_mode = 0;
    freestyle.wraparound = 0;
    paledit.editpal_canvas_cursor.active = 0;
    ok_to_cycle = 1;

    XtRemoveEventHandler(paledit.palette_colorbox, PointerMotionMask,
      False, (XtEventHandler) editpal_pointer_motion, NULL);

    XtRemoveEventHandler(canvas, ButtonPressMask,
      False, (XtEventHandler) editpal_place_cursor, NULL);

    XtRemoveEventHandler(canvas, PointerMotionMask,
      False, (XtEventHandler) editpal_pointer_motion, NULL);

    if(paledit.editpal_canvas_cursor.active)
      XDrawArc(display, XtWindow(canvas), xgc,
        paledit.editpal_canvas_cursor.old_x - 6,
        paledit.editpal_canvas_cursor.old_y - 6,
        12, 12, 23040, 23040);

    XChangeGC(display, xgc, GCLineStyle, &paledit.xgc_values);

    if(skipval_shell != NULL)
      XtPopdown(skipval_shell);

    if(cyclerange_shell != NULL)
      XtPopdown(cyclerange_shell);

    if(freestyle_shell != NULL)
      XtPopdown(freestyle_shell);

    if(gammaval_shell != NULL)
      XtPopdown(gammaval_shell);

    freestyle_shell_popped_up = 0;

    XtPopdown(paledit.edit_palette_dialog);

    toggle_zoom_grab(ZOOM_ON);
    }

  paledit.undo_index = 0;
  paledit.redo_index = 0;

  if(undo_max > 0)
    {
    memset(undobox, 0, (sizeof(unsigned char) * undo_max * 256 * 3));
    memcpy(undobox, dacbox, 768);
    }

  sprintf((char *) &tempstr,
    "Undo last edit (%5d undo's available)           ",
     paledit.undo_index);
  str = XmStringCreateLocalized((char *) &tempstr);
  button = XtNameToWidget(editpal_options_dialog,
    "*coloredit_edit_menu*button_1");
  if(button != NULL)
    XtVaSetValues(button, XmNlabelString, str, NULL);
  XmStringFree(str);

  sprintf((char *) &tempstr,
    "Redo last undo (%5d redo's available)           ",
     paledit.redo_index);
  str = XmStringCreateLocalized((char *) &tempstr);
  button = XtNameToWidget(editpal_options_dialog,
    "*coloredit_edit_menu*button_2");
  if(button != NULL)
    XtVaSetValues(button, XmNlabelString, str, NULL);
  XmStringFree(str);
}

static void swaprgb(int which)
{
  Widget active_red;
  Widget active_green;
  Widget active_blue;
  int from_val;
  int to_val;
  int i;

  if(paledit.active_color_register == 0)
    {
    active_red = paledit.primary_red_scale;
    active_green = paledit.primary_green_scale;
    active_blue = paledit.primary_blue_scale;
    }
  else
    {
    active_red = paledit.secondary_red_scale;
    active_green = paledit.secondary_green_scale;
    active_blue = paledit.secondary_blue_scale;
    }

  switch(which)
    {
    case 0:
      XmScaleGetValue(active_red, &from_val);
      XmScaleGetValue(active_green, &to_val);
      dacbox[paledit.active_color_register_color][0] =
        (unsigned char) to_val;
      dacbox[paledit.active_color_register_color][1] =
        (unsigned char) from_val;
      XmScaleSetValue(active_red, to_val);
      XmScaleSetValue(active_green, from_val);
    break;

    case 1:
      XmScaleGetValue(active_green, &from_val);
      XmScaleGetValue(active_blue, &to_val);
      dacbox[paledit.active_color_register_color][1] =
        (unsigned char) to_val;
      dacbox[paledit.active_color_register_color][2] =
        (unsigned char) from_val;
      XmScaleSetValue(active_green, to_val);
      XmScaleSetValue(active_blue, from_val);
    break;

    case 2:
      XmScaleGetValue(active_blue, &from_val);
      XmScaleGetValue(active_red, &to_val);
      dacbox[paledit.active_color_register_color][2] =
        (unsigned char) to_val;
      dacbox[paledit.active_color_register_color][0] =
        (unsigned char) from_val;
      XmScaleSetValue(active_blue, to_val);
      XmScaleSetValue(active_red, from_val);
    break;
    }

  cols[paledit.active_color_register_color].red =
    dacbox[paledit.active_color_register_color][0] * 256;
  cols[paledit.active_color_register_color].blue =
    dacbox[paledit.active_color_register_color][2] * 256;
  cols[paledit.active_color_register_color].green =
    dacbox[paledit.active_color_register_color][1] * 256;

  XStoreColor(display, colormap,
    &cols[paledit.active_color_register_color]);

  i = paledit.active_color_register_color;

  paledit.palette_colorbox_rect[i].red = dacbox[i][0];
  paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
  paledit.palette_colorbox_rect[i].green = dacbox[i][1];
  paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
  paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
  paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;

  change_xor_gc(0);
  update_undo_index();
}

/*ARGSUSED*/
static void swaprgbrange(int which)
{
  int start, stop, num, i, j;
  unsigned char rval, bval, gval;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;
  Widget active_red;
  Widget active_green;
  Widget active_blue;

  if(paledit.range_mode == 0)
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    else
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    num = ABS(stop - start);
    }
  else
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    else
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    num = (colors - ABS(stop - start));
    }

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));
  XDefineCursor(display, XtWindow(paledit.edit_palette_dialog),
    XCreateFontCursor(display, XC_watch));

  if(paledit.active_color_register == 0)
    {
    active_red = paledit.primary_red_scale;
    active_green = paledit.primary_green_scale;
    active_blue = paledit.primary_blue_scale;
    }
  else
    {
    active_red = paledit.secondary_red_scale;
    active_green = paledit.secondary_green_scale;
    active_blue = paledit.secondary_blue_scale;
    }

  if(paledit.active_color_register == 1)
    {
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  if(paledit.range_mode == 0)
    {
    for(i = start, j = 0; j <= num; i++, j++)
      {
      rval = dacbox[i][0];
      gval = dacbox[i][1];
      bval = dacbox[i][2];

      switch(which)
        {
        case 0: /* swap red and green values */
          dacbox[i][0] = gval;
          dacbox[i][1] = rval;
        break;

        case 1: /* swap green and blue values */
          dacbox[i][1] = bval;
          dacbox[i][2] = gval;
        break;

        case 2: /* swap red and blue values */
          dacbox[i][2] = rval;
          dacbox[i][0] = bval;
        break;
        }

      cols[i].red = dacbox[i][0] * 256;
      cols[i].blue = dacbox[i][2] * 256;
      cols[i].green = dacbox[i][1] * 256;

      XStoreColor(display, colormap, &cols[i]);

      paledit.palette_colorbox_rect[i].red = dacbox[i][0];
      paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
      paledit.palette_colorbox_rect[i].green = dacbox[i][1];
      paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
      paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
      paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;

      if(i == paledit.active_color_register_color)
        {
        XmScaleSetValue(active_red,
          paledit.palette_colorbox_rect[i].intred);
        XmScaleSetValue(active_green,
          paledit.palette_colorbox_rect[i].intgreen);
        XmScaleSetValue(active_blue,
          paledit.palette_colorbox_rect[i].intblue);
        }
      else if(i == paledit.inactive_color_register_color)
        {
        XmScaleSetValue(inactive_red,
          paledit.palette_colorbox_rect[i].intred);
        XmScaleSetValue(inactive_green,
          paledit.palette_colorbox_rect[i].intgreen);
        XmScaleSetValue(inactive_blue,
          paledit.palette_colorbox_rect[i].intblue);
        }

      if(i == freestyle.color)
        {
        if(freestyle_shell_popped_up == 1)
          redraw_freestyle_colorbox(main_window, NULL, NULL);
        }
      }
    }
  else
    {
    for(i = start + 1, j = 0; j < (num - 1); i++)
      {
      if(i > (colors - 1))
        i = 0;

      if((i >= start) && (i <= stop))
        continue;
      else
        j++;

      rval = dacbox[i][0];
      bval = dacbox[i][1];
      gval = dacbox[i][2];

      switch(which)
        {
        case 0:
          dacbox[i][0] = gval;
          dacbox[i][2] = rval;
        break;

        case 1:
          dacbox[i][1] = gval;
          dacbox[i][2] = bval;
        break;

        case 2:
          dacbox[i][1] = rval;
          dacbox[i][0] = bval;
        break;
        }

      cols[i].red = dacbox[i][0] * 256;
      cols[i].blue = dacbox[i][2] * 256;
      cols[i].green = dacbox[i][1] * 256;

      XStoreColor(display, colormap, &cols[i]);

      paledit.palette_colorbox_rect[i].red = dacbox[i][0];
      paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
      paledit.palette_colorbox_rect[i].green = dacbox[i][1];
      paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
      paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
      paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;

      if(i == freestyle.color)
        {
        if(freestyle_shell_popped_up == 1)
          redraw_freestyle_colorbox(main_window, NULL, NULL);
        }
      }
    }

  change_xor_gc(0);
  update_undo_index();

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  XDefineCursor(display, XtWindow(paledit.edit_palette_dialog),
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void invertrgb(int which)
{
  int i, j;
  int start, stop, num;
  int rval, bval, gval;
  Widget active_red;
  Widget active_green;
  Widget active_blue;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;

  if(paledit.range_mode == 0)
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    else
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    num = ABS(stop - start);
    }
  else
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    else
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    num = (colors - ABS(stop - start));
    }

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));
  XDefineCursor(display, XtWindow(paledit.edit_palette_dialog),
    XCreateFontCursor(display, XC_watch));

  if(paledit.active_color_register == 0)
    {
    active_red = paledit.primary_red_scale;
    active_green = paledit.primary_green_scale;
    active_blue = paledit.primary_blue_scale;
    }
  else
    {
    active_red = paledit.secondary_red_scale;
    active_green = paledit.secondary_green_scale;
    active_blue = paledit.secondary_blue_scale;
    }

  if(paledit.active_color_register == 1)
    {
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  if(which == 1)
    {
    if(paledit.range_mode == 0)
      {
      for(i = start, j = 0; j <= num; i++, j++)
        {
        rval = (255 - ((int)dacbox[i][0]));
        bval = (255 - ((int)dacbox[i][1]));
        gval = (255 - ((int)dacbox[i][2]));

        dacbox[i][0] = (unsigned char) rval;
        dacbox[i][1] = (unsigned char) bval;
        dacbox[i][2] = (unsigned char) gval;

        cols[i].red = dacbox[i][0] * 256;
        cols[i].blue = dacbox[i][2] * 256;
        cols[i].green = dacbox[i][1] * 256;

        XStoreColor(display, colormap, &cols[i]);

        paledit.palette_colorbox_rect[i].red = dacbox[i][0];
        paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
        paledit.palette_colorbox_rect[i].green = dacbox[i][1];
        paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
        paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
        paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;

        if(i == paledit.active_color_register_color)
          {
          XmScaleSetValue(active_red,
            paledit.palette_colorbox_rect[i].intred);
          XmScaleSetValue(active_green,
            paledit.palette_colorbox_rect[i].intgreen);
          XmScaleSetValue(active_blue,
            paledit.palette_colorbox_rect[i].intblue);
          }
        else if(i == paledit.inactive_color_register_color)
          {
          XmScaleSetValue(inactive_red,
            paledit.palette_colorbox_rect[i].intred);
          XmScaleSetValue(inactive_green,
            paledit.palette_colorbox_rect[i].intgreen);
          XmScaleSetValue(inactive_blue,
            paledit.palette_colorbox_rect[i].intblue);
          }
        }
      }
    else
      {
      for(i = start + 1, j = 0; j < (num - 1); i++)
        {
        if(i > (colors - 1))
          i = 0;

        if((i >= start) && (i <= stop))
          continue;
        else
          j++;

        rval = (255 - ((int)dacbox[i][0]));
        gval = (255 - ((int)dacbox[i][1]));
        bval = (255 - ((int)dacbox[i][2]));

        dacbox[i][0] = (unsigned char) gval;
        dacbox[i][1] = (unsigned char) bval;
        dacbox[i][2] = (unsigned char) rval;

        cols[i].red = dacbox[i][0] * 256;
        cols[i].blue = dacbox[i][2] * 256;
        cols[i].green = dacbox[i][1] * 256;

        XStoreColor(display, colormap, &cols[i]);

        paledit.palette_colorbox_rect[i].red = dacbox[i][0];
        paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
        paledit.palette_colorbox_rect[i].green = dacbox[i][1];
        paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
        paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
        paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;
        }
      }
    }
  else
    {
    XmScaleGetValue(active_red, &rval);
    XmScaleGetValue(active_green, &gval);
    XmScaleGetValue(active_blue, &bval);

    i = (255 - rval);
    dacbox[paledit.active_color_register_color][0] = (unsigned char) i;
    XmScaleSetValue(active_red, i);

    i = (255 - gval);
    dacbox[paledit.active_color_register_color][1] = (unsigned char) i;
    XmScaleSetValue(active_green, i);

    i = (255 - bval);
    dacbox[paledit.active_color_register_color][2] = (unsigned char) i;
    XmScaleSetValue(active_blue, i);

    i = paledit.active_color_register_color;

    cols[i].red = dacbox[i][0] * 256;
    cols[i].blue = dacbox[i][2] * 256;
    cols[i].green = dacbox[i][1] * 256;

    XStoreColor(display, colormap, &cols[i]);

    paledit.palette_colorbox_rect[i].red = dacbox[i][0];
    paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
    paledit.palette_colorbox_rect[i].green = dacbox[i][1];
    paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
    paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
    paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;
    }

  if(freestyle_shell_popped_up == 1)
    redraw_freestyle_colorbox(main_window, NULL, NULL);

  change_xor_gc(0);
  update_undo_index();

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  XDefineCursor(display, XtWindow(paledit.edit_palette_dialog),
    XCreateFontCursor(display, XC_top_left_arrow));
}

static void copy_inactive_to_active()
{
  Widget active;
  Widget active_red;
  Widget active_green;
  Widget active_blue;
  GC active_gc;
  int color;
  int oldcolor;

  if(paledit.active_color_register == 0)
    {
    active = paledit.primary_active_colorbox;
    active_gc = paledit.primary_active_colorbox_gc;
    active_red = paledit.primary_red_scale;
    active_green = paledit.primary_green_scale;
    active_blue = paledit.primary_blue_scale;
    }
  else
    {
    active = paledit.secondary_active_colorbox;
    active_gc = paledit.secondary_active_colorbox_gc;
    active_red = paledit.secondary_red_scale;
    active_green = paledit.secondary_green_scale;
    active_blue = paledit.secondary_blue_scale;
    }

  oldcolor = paledit.active_color_register_color;
  color = paledit.inactive_color_register_color;

  XSetForeground(display, active_gc, pixtab[color]);
    XFillRectangle(display, XtWindow(active),
    active_gc, 0, 0, 59, 24);

  XmScaleSetValue(active_red,
    paledit.palette_colorbox_rect[color].intred);

  XmScaleSetValue(active_green,
    paledit.palette_colorbox_rect[color].intgreen);

  XmScaleSetValue(active_blue,
    paledit.palette_colorbox_rect[color].intblue);

  dacbox[oldcolor][0] = dacbox[color][0];
  dacbox[oldcolor][1] = dacbox[color][1];
  dacbox[oldcolor][2] = dacbox[color][2];

  paledit.palette_colorbox_rect[oldcolor].red = dacbox[color][0];
  paledit.palette_colorbox_rect[oldcolor].green = dacbox[color][2];
  paledit.palette_colorbox_rect[oldcolor].blue = dacbox[color][1];

  cols[oldcolor].red = dacbox[oldcolor][0] * 256;
  cols[oldcolor].blue = dacbox[oldcolor][2] * 256;
  cols[oldcolor].green = dacbox[oldcolor][1] * 256;

  XStoreColor(display, colormap, &cols[oldcolor]);

  change_xor_gc(0);
  update_undo_index();

  paledit.palette_colorbox_rect[oldcolor].intred =
    (int)cols[color].red / 256;
  paledit.palette_colorbox_rect[oldcolor].intblue =
    (int)cols[color].blue / 256;
  paledit.palette_colorbox_rect[oldcolor].intgreen =
    (int)cols[color].green / 256;
}

static void copy_active_to_inactive()
{
  Widget inactive;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;
  GC inactive_gc;

  if(paledit.active_color_register == 1)
    {
    inactive = paledit.primary_active_colorbox;
    inactive_gc = paledit.primary_active_colorbox_gc;
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive = paledit.secondary_active_colorbox;
    inactive_gc = paledit.secondary_active_colorbox_gc;
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  oldcolor = paledit.inactive_color_register_color;
  color = paledit.active_color_register_color;

  XSetForeground(display, inactive_gc, pixtab[color]);
    XFillRectangle(display, XtWindow(inactive),
    inactive_gc, 0, 0, 59, 24);

  XmScaleSetValue(inactive_red,
    paledit.palette_colorbox_rect[color].intred);

  XmScaleSetValue(inactive_green,
    paledit.palette_colorbox_rect[color].intgreen);

  XmScaleSetValue(inactive_blue,
    paledit.palette_colorbox_rect[color].intblue);

  dacbox[oldcolor][0] = dacbox[color][0];
  dacbox[oldcolor][1] = dacbox[color][1];
  dacbox[oldcolor][2] = dacbox[color][2];

  paledit.palette_colorbox_rect[oldcolor].red = dacbox[color][0];
  paledit.palette_colorbox_rect[oldcolor].green = dacbox[color][2];
  paledit.palette_colorbox_rect[oldcolor].blue = dacbox[color][1];

  cols[oldcolor].red = dacbox[oldcolor][0] * 256;
  cols[oldcolor].blue = dacbox[oldcolor][2] * 256;
  cols[oldcolor].green = dacbox[oldcolor][1] * 256;

  XStoreColor(display, colormap, &cols[oldcolor]);

  change_xor_gc(0);
  update_undo_index();

  paledit.palette_colorbox_rect[oldcolor].intred =
    (int)cols[color].red / 256;
  paledit.palette_colorbox_rect[oldcolor].intblue =
    (int)cols[color].blue / 256;
  paledit.palette_colorbox_rect[oldcolor].intgreen =
    (int)cols[color].green / 256;
}

static void range_smooth(int skip)
{
  int num, start, stop;
  double rm, gm, bm;
  int n, x;

  if(paledit.range_mode == 0)
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    else
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    num = ABS(stop - start);
    }
  else
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    else
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    num = (colors - ABS(stop - start));
    }

    rm = (double)((int)paledit.palette_colorbox_rect[stop].red -
                  (int)paledit.palette_colorbox_rect[start].red) / num;
    gm = (double)((int)paledit.palette_colorbox_rect[stop].green -
                  (int)paledit.palette_colorbox_rect[start].green) / num;
    bm = (double)((int)paledit.palette_colorbox_rect[stop].blue -
                  (int)paledit.palette_colorbox_rect[start].blue) / num;

  if(paledit.range_mode == 0)
    {
    for(n = start, x = 0; n < stop; n += (1 + skip), x += (1 + skip))
      {
      if(n > stop)
        continue;

      if(paledit.gamma_val == 1.0)
        {
        paledit.palette_colorbox_rect[n].red =
          (paledit.palette_colorbox_rect[start].red ==
           paledit.palette_colorbox_rect[stop].red) ?
           paledit.palette_colorbox_rect[start].red :
           (int)paledit.palette_colorbox_rect[start].red +
           (int)(rm * x);

        paledit.palette_colorbox_rect[n].green =
          (paledit.palette_colorbox_rect[start].green ==
           paledit.palette_colorbox_rect[stop].green) ?
           paledit.palette_colorbox_rect[start].green :
           (int)paledit.palette_colorbox_rect[start].green +
           (int)(gm * x);

        paledit.palette_colorbox_rect[n].blue =
          (paledit.palette_colorbox_rect[start].blue ==
           paledit.palette_colorbox_rect[stop].blue) ?
           paledit.palette_colorbox_rect[start].blue :
           (int)paledit.palette_colorbox_rect[start].blue +
           (int)(bm * x);
        }
      else
        {
        paledit.palette_colorbox_rect[n].red =
          (paledit.palette_colorbox_rect[start].red ==
           paledit.palette_colorbox_rect[stop].red) ?
           paledit.palette_colorbox_rect[start].red :
           (unsigned char)(paledit.palette_colorbox_rect[start].red +
           (unsigned char)(pow(x/(double)(num - 1),
           (double) paledit.gamma_val) * num * rm));

        paledit.palette_colorbox_rect[n].green =
          (paledit.palette_colorbox_rect[start].green ==
           paledit.palette_colorbox_rect[stop].green) ?
           paledit.palette_colorbox_rect[start].green :
           (unsigned char)(paledit.palette_colorbox_rect[start].green +
           (unsigned char)(pow(x/(double)(num - 1),
           (double) paledit.gamma_val) * num * gm));

        paledit.palette_colorbox_rect[n].blue =
          (paledit.palette_colorbox_rect[start].blue ==
           paledit.palette_colorbox_rect[stop].blue) ?
           paledit.palette_colorbox_rect[start].blue :
           (unsigned char)(paledit.palette_colorbox_rect[start].blue +
           (unsigned char)(pow(x/(double)(num - 1),
           (double) paledit.gamma_val) * num * bm));
        }

      dacbox[n][0] = paledit.palette_colorbox_rect[n].red;
      dacbox[n][2] = paledit.palette_colorbox_rect[n].blue;
      dacbox[n][1] = paledit.palette_colorbox_rect[n].green;

      cols[n].red = dacbox[n][0] * 256;
      cols[n].blue = dacbox[n][2] * 256;
      cols[n].green = dacbox[n][1] * 256;

      XStoreColor(display, colormap, &cols[n]);

      paledit.palette_colorbox_rect[n].intred = (int)cols[n].red / 256;
      paledit.palette_colorbox_rect[n].intblue = (int)cols[n].blue / 256;
      paledit.palette_colorbox_rect[n].intgreen = (int)cols[n].green / 256;
      }
    }
  else
    {
    for(n = start + 1, x = 0; x < num; n += (1 + skip))
      {
      if(n > (colors - 1))
        n = 0;

      if((n >= start) && (n < stop))
        continue;
      else
        x += (1 + skip);

      if(paledit.gamma_val == 1.0)
        {
        paledit.palette_colorbox_rect[n].red =
          (paledit.palette_colorbox_rect[start].red ==
           paledit.palette_colorbox_rect[stop].red) ?
           paledit.palette_colorbox_rect[start].red :
           (int)paledit.palette_colorbox_rect[start].red +
           (int)(rm * x);

        paledit.palette_colorbox_rect[n].green =
          (paledit.palette_colorbox_rect[start].green ==
           paledit.palette_colorbox_rect[stop].green) ?
           paledit.palette_colorbox_rect[start].green :
           (int)paledit.palette_colorbox_rect[start].green +
           (int)(gm * x);

        paledit.palette_colorbox_rect[n].blue =
          (paledit.palette_colorbox_rect[start].blue ==
           paledit.palette_colorbox_rect[stop].blue) ?
           paledit.palette_colorbox_rect[start].blue :
           (int)paledit.palette_colorbox_rect[start].blue +
           (int)(bm * x);
        }
      else
        {
        paledit.palette_colorbox_rect[n].red =
          (paledit.palette_colorbox_rect[start].red ==
           paledit.palette_colorbox_rect[stop].red) ?
           paledit.palette_colorbox_rect[start].red :
           (unsigned char)(paledit.palette_colorbox_rect[start].red +
           (unsigned char)(pow(x/(double)(num - 1),
           (double) paledit.gamma_val) * num * rm));

        paledit.palette_colorbox_rect[n].green =
          (paledit.palette_colorbox_rect[start].green ==
           paledit.palette_colorbox_rect[stop].green) ?
           paledit.palette_colorbox_rect[start].green :
           (unsigned char)(paledit.palette_colorbox_rect[start].green +
           (unsigned char)(pow(x/(double)(num - 1),
           (double) paledit.gamma_val) * num * gm));

        paledit.palette_colorbox_rect[n].blue =
          (paledit.palette_colorbox_rect[start].blue ==
           paledit.palette_colorbox_rect[stop].blue) ?
           paledit.palette_colorbox_rect[start].blue :
           (unsigned char)(paledit.palette_colorbox_rect[start].blue +
           (unsigned char)(pow(x/(double)(num - 1),
           (double) paledit.gamma_val) * num * bm));
        }

      dacbox[n][0] = paledit.palette_colorbox_rect[n].red;
      dacbox[n][2] = paledit.palette_colorbox_rect[n].blue;
      dacbox[n][1] = paledit.palette_colorbox_rect[n].green;

      cols[n].red = dacbox[n][0] * 256;
      cols[n].blue = dacbox[n][2] * 256;
      cols[n].green = dacbox[n][1] * 256;

      XStoreColor(display, colormap, &cols[n]);

      paledit.palette_colorbox_rect[n].intred = (int)cols[n].red / 256;
      paledit.palette_colorbox_rect[n].intblue = (int)cols[n].blue / 256;
      paledit.palette_colorbox_rect[n].intgreen = (int)cols[n].green / 256;
      }
    }
  if(freestyle_shell_popped_up == 1)
    redraw_freestyle_colorbox(main_window, NULL, NULL);
  update_undo_index();
}

static void copy_range()
{
  int x, y, z;
  int num, start, stop;

  if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
    {
    start = paledit.pal_range.start_index;
    stop = paledit.pal_range.stop_index;
    }
  else
    {
    stop = paledit.pal_range.start_index;
    start = paledit.pal_range.stop_index;
    }

  num = ABS(stop - start);

  paledit.pal_range.num_saved = num + 1;

  for(x = start, y = 0, z = 0;
      z < paledit.pal_range.num_saved && z <= num;
      x++, y++, z++)
    {
    paledit.pal_range.saved[y] = paledit.palette_colorbox_rect[x];
    }
}

static void paste_range(int direction)
{
  int x, y, z;
  int num, start, stop;

  if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
    {
    start = paledit.pal_range.start_index;
    stop = paledit.pal_range.stop_index;
    }
  else
    {
    stop = paledit.pal_range.start_index;
    start = paledit.pal_range.stop_index;
    }

  num = ABS(stop - start);

  if(direction == 0)
    {
    for(x = start, y = 0, z = 0;
        z < paledit.pal_range.num_saved && z <= num;
        x++, y++, z++)
      {
      paledit.palette_colorbox_rect[x].intred =
        paledit.pal_range.saved[y].intred;
      paledit.palette_colorbox_rect[x].intgreen =
        paledit.pal_range.saved[y].intgreen;
      paledit.palette_colorbox_rect[x].intblue =
        paledit.pal_range.saved[y].intblue;
      paledit.palette_colorbox_rect[x].red =
        paledit.pal_range.saved[y].red;
      paledit.palette_colorbox_rect[x].green =
        paledit.pal_range.saved[y].green;
      paledit.palette_colorbox_rect[x].blue =
        paledit.pal_range.saved[y].blue;
      paledit.palette_colorbox_rect[x].color =
        paledit.pal_range.saved[y].color;

      dacbox[x][0] = paledit.palette_colorbox_rect[x].red;
      dacbox[x][2] = paledit.palette_colorbox_rect[x].blue;
      dacbox[x][1] = paledit.palette_colorbox_rect[x].green;
      }
    }
  else /* reverse */
    {
    for(x = stop, y = 0, z = 0;
        z < paledit.pal_range.num_saved && z <= num;
        x--, y++, z++)
      {
      paledit.palette_colorbox_rect[x].intred =
        paledit.pal_range.saved[y].intred;
      paledit.palette_colorbox_rect[x].intgreen =
        paledit.pal_range.saved[y].intgreen;
      paledit.palette_colorbox_rect[x].intblue =
        paledit.pal_range.saved[y].intblue;
      paledit.palette_colorbox_rect[x].red =
        paledit.pal_range.saved[y].red;
      paledit.palette_colorbox_rect[x].green =
        paledit.pal_range.saved[y].green;
      paledit.palette_colorbox_rect[x].blue =
        paledit.pal_range.saved[y].blue;
      paledit.palette_colorbox_rect[x].color =
        paledit.pal_range.saved[y].color;

      dacbox[x][0] = paledit.palette_colorbox_rect[x].red;
      dacbox[x][2] = paledit.palette_colorbox_rect[x].blue;
      dacbox[x][1] = paledit.palette_colorbox_rect[x].green;
      }
    }
  writevideopalette();
  change_xor_gc(0);
  update_undo_index();
}

/*ARGSUSED*/
static void get_skip_val(Widget w)
{
  static Widget skipval_dialog = NULL;
  Widget skipval_dflt_button;
  Widget skipval_done_button;
  Widget skipval_help_button;
  Widget skipval_cancel_button;
  Widget skipval_reset_button;
  Arg args[MAX_ARGS];
  XmString str;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 5 - 1;

  if(!skipval_shell)
    {
    skipval_shell = XmCreateDialogShell
      (main_window, "Select stripe width", NULL, 0);

    XtAddCallback(skipval_shell, XmNpopupCallback,  map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(skipval_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Select stripe width");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    skipval_dialog = XmCreateForm
      (skipval_shell, "skipval_dialog", args, n);

    XtAddCallback(skipval_dialog, XmNmapCallback, map_dialog, NULL);
    XmStringFree(str);

    /*
     * Number of colors to skip between stripes
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Number of colors to skip between stripes");
    XtSetArg(args[n], XmNmaximum, ((colors / 2) - 1)); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    paledit.pal_range.skipval_scale =
      XmCreateScale (skipval_dialog, "skipval_scale", args, n);

    XtManageChild(paledit.pal_range.skipval_scale);
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
    XtSetArg(args[n], XmNleftPosition, 0); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.pal_range.skipval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    skipval_reset_button = XmCreatePushButton
      (skipval_dialog, "skipval_reset_button", args, n);

    XtAddCallback(skipval_reset_button, XmNactivateCallback,
      skipval_reset_button_cb, NULL);

    XtManageChild(skipval_reset_button);
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
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.pal_range.skipval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    skipval_help_button = XmCreatePushButton
      (skipval_dialog, "skipval_help_button", args, n);

    XtAddCallback(skipval_help_button, XmNactivateCallback,
      skipval_help_button_cb, NULL);

    XtManageChild(skipval_help_button);
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
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.pal_range.skipval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    skipval_done_button = XmCreatePushButton
      (skipval_dialog, "skipval_done_button", args, n);

    XtAddCallback(skipval_done_button, XmNactivateCallback,
      skipval_done_button_cb, NULL);

    XtManageChild(skipval_done_button);
    XmStringFree(str);

    /*
     * Default
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Default");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.pal_range.skipval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    skipval_dflt_button = XmCreatePushButton
      (skipval_dialog, "skipval_dflt_button", args, n);

    XtAddCallback(skipval_dflt_button, XmNactivateCallback,
      skipval_dflt_button_cb, NULL);

    XtManageChild(skipval_dflt_button);
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
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, paledit.pal_range.skipval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    skipval_cancel_button = XmCreatePushButton
      (skipval_dialog, "skipval_cancel_button", args, n);

    XtAddCallback(skipval_cancel_button, XmNactivateCallback,
      skipval_cancel_button_cb, NULL);

    XtManageChild(skipval_cancel_button);
    XmStringFree(str);

    XtVaSetValues(skipval_dialog,
      XmNdefaultButton,
      skipval_done_button,
      NULL);
    }


  XtVaSetValues(paledit.pal_range.skipval_scale,
    XmNvalue, paledit.pal_range.skip_val,
    NULL);

  XtManageChild(skipval_dialog);
  XtManageChild(skipval_shell);
  XtPopup(skipval_shell, XtGrabNone);
}

/*ARGSUSED*/
static void skipval_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(paledit.pal_range.skipval_scale,
    paledit.pal_range.skip_val);
}

/*ARGSUSED*/
static void skipval_help_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPEDITPAL;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void skipval_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleGetValue(paledit.pal_range.skipval_scale,
    &(paledit.pal_range.skip_val));

  XtPopdown(skipval_shell);

  range_smooth(paledit.pal_range.skip_val);
}

/*ARGSUSED*/
static void skipval_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(paledit.pal_range.skipval_scale, 1);
}

/*ARGSUSED*/
static void skipval_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(skipval_shell != NULL)
    XtPopdown(skipval_shell);
}

/*ARGSUSED*/
static void set_brightness(int which)
{
  int num, i, j;
  int start, stop;
  int rval, bval, gval;
  Widget inactive_red;
  Widget inactive_green;
  Widget inactive_blue;
  Widget active_red;
  Widget active_green;
  Widget active_blue;

  if(paledit.range_mode == 0)
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    else
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    num = ABS(stop - start);
    }
  else
    {
    if(paledit.pal_range.start_index <= paledit.pal_range.stop_index)
      {
      stop = paledit.pal_range.start_index;
      start = paledit.pal_range.stop_index;
      }
    else
      {
      start = paledit.pal_range.start_index;
      stop = paledit.pal_range.stop_index;
      }
    num = (colors - ABS(stop - start));
    }

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));
  XDefineCursor(display, XtWindow(paledit.edit_palette_dialog),
    XCreateFontCursor(display, XC_watch));

  if(paledit.active_color_register == 0)
    {
    active_red = paledit.primary_red_scale;
    active_green = paledit.primary_green_scale;
    active_blue = paledit.primary_blue_scale;
    }
  else
    {
    active_red = paledit.secondary_red_scale;
    active_green = paledit.secondary_green_scale;
    active_blue = paledit.secondary_blue_scale;
    }

  if(paledit.active_color_register == 1)
    {
    inactive_red = paledit.primary_red_scale;
    inactive_green = paledit.primary_green_scale;
    inactive_blue = paledit.primary_blue_scale;
    }
  else
    {
    inactive_red = paledit.secondary_red_scale;
    inactive_green = paledit.secondary_green_scale;
    inactive_blue = paledit.secondary_blue_scale;
    }

  if(paledit.range_mode == 0)
    {
    for(i = start; i <= stop; i++)
      {
      if(which == 1)
        {
        rval = (int)dacbox[i][0];
        gval = (int)dacbox[i][1];
        bval = (int)dacbox[i][2];

        if((rval += 5) <= 255)
          dacbox[i][0] = (unsigned char) rval;
        else
          dacbox[i][0] = 0xff;

        if((gval += 5) <= 255)
          dacbox[i][1] = (unsigned char) gval;
        else
          dacbox[i][1] = 0xff;

        if((bval += 5) <= 255)
          dacbox[i][2] = (unsigned char) bval;
        else
          dacbox[i][2] = 0xff;
        }
      else
        {
        rval = (int)dacbox[i][0];
        gval = (int)dacbox[i][1];
        bval = (int)dacbox[i][2];

        if((rval -= 5) >= 0)
          dacbox[i][0] = (unsigned char) rval;
        else
          dacbox[i][0] = 0x0;

        if((gval -= 5) >= 0)
          dacbox[i][1] = (unsigned char) gval;
        else
          dacbox[i][1] = 0x0;

        if((bval -= 5) >= 0)
          dacbox[i][2] = (unsigned char) bval;
        else
          dacbox[i][2] = 0x0;
        }
      }

    for(i = start; i <= stop; i++)
      {
      cols[i].red = dacbox[i][0] * 256;
      cols[i].blue = dacbox[i][2] * 256;
      cols[i].green = dacbox[i][1] * 256;

      XStoreColor(display, colormap, &cols[i]);

      paledit.palette_colorbox_rect[i].red = dacbox[i][0];
      paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
      paledit.palette_colorbox_rect[i].green = dacbox[i][1];
      paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
      paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
      paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;

      if(i == paledit.active_color_register_color)
        {
        XmScaleSetValue(active_red,
          paledit.palette_colorbox_rect[i].intred);
        XmScaleSetValue(active_green,
          paledit.palette_colorbox_rect[i].intgreen);
        XmScaleSetValue(active_blue,
          paledit.palette_colorbox_rect[i].intblue);
        }
      else if(i == paledit.inactive_color_register_color)
        {
        XmScaleSetValue(inactive_red,
          paledit.palette_colorbox_rect[i].intred);
        XmScaleSetValue(inactive_green,
          paledit.palette_colorbox_rect[i].intgreen);
        XmScaleSetValue(inactive_blue,
          paledit.palette_colorbox_rect[i].intblue);
        }

      if(i == freestyle.color)
        {
        if(freestyle_shell_popped_up == 1)
          redraw_freestyle_colorbox(main_window, NULL, NULL);
        }
      }
    }
  else
    {
    for(i = start + 1, j = 0; j < (num - 1); i++)
      {
      if(i > (colors - 1))
        i = 0;

      if((i >= start) && (i <= stop))
        continue;
      else
        j++;

      if(which == 1)
        {
        rval = (int)dacbox[i][0];
        gval = (int)dacbox[i][1];
        bval = (int)dacbox[i][2];

        if((rval += 5) <= 255)
          dacbox[i][0] = (unsigned char) rval;
        else
          dacbox[i][0] = 0xff;

        if((gval += 5) <= 255)
          dacbox[i][1] = (unsigned char) gval;
        else
          dacbox[i][1] = 0xff;

        if((bval += 5) <= 255)
          dacbox[i][2] = (unsigned char) bval;
        else
          dacbox[i][2] = 0xff;
        }
      else
        {
        rval = (int)dacbox[i][0];
        gval = (int)dacbox[i][1];
        bval = (int)dacbox[i][2];

        if((rval -= 5) >= 0)
          dacbox[i][0] = (unsigned char) rval;
        else
          dacbox[i][0] = 0x0;

        if((gval -= 5) >= 0)
          dacbox[i][1] = (unsigned char) gval;
        else
          dacbox[i][1] = 0x0;

        if((bval -= 5) >= 0)
          dacbox[i][2] = (unsigned char) bval;
        else
          dacbox[i][2] = 0x0;
        }
      }

    for(i = start + 1, j = 0; j < (num - 1); i++)
      {
      if(i > (colors - 1))
        i = 0;

      if((i >= start) && (i <= stop))
        continue;
      else
        j++;

      cols[i].red = dacbox[i][0] * 256;
      cols[i].blue = dacbox[i][2] * 256;
      cols[i].green = dacbox[i][1] * 256;

      XStoreColor(display, colormap, &cols[i]);

      paledit.palette_colorbox_rect[i].red = dacbox[i][0];
      paledit.palette_colorbox_rect[i].blue = dacbox[i][2];
      paledit.palette_colorbox_rect[i].green = dacbox[i][1];
      paledit.palette_colorbox_rect[i].intred = (int)cols[i].red / 256;
      paledit.palette_colorbox_rect[i].intblue = (int)cols[i].blue / 256;
      paledit.palette_colorbox_rect[i].intgreen = (int)cols[i].green / 256;

      if(i == paledit.active_color_register_color)
        {
        XmScaleSetValue(active_red,
          paledit.palette_colorbox_rect[i].intred);
        XmScaleSetValue(active_green,
          paledit.palette_colorbox_rect[i].intgreen);
        XmScaleSetValue(active_blue,
          paledit.palette_colorbox_rect[i].intblue);
        }
      else if(i == paledit.inactive_color_register_color)
        {
        XmScaleSetValue(inactive_red,
          paledit.palette_colorbox_rect[i].intred);
        XmScaleSetValue(inactive_green,
          paledit.palette_colorbox_rect[i].intgreen);
        XmScaleSetValue(inactive_blue,
          paledit.palette_colorbox_rect[i].intblue);
        }

      if(i == freestyle.color)
        {
        if(freestyle_shell_popped_up == 1)
          redraw_freestyle_colorbox(main_window, NULL, NULL);
        }
      }
    }

  change_xor_gc(0);
  update_undo_index();

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  XDefineCursor(display, XtWindow(paledit.edit_palette_dialog),
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void toggle_range_mode()
{
  XmString str;
  Widget button;

  if(paledit.range_mode == 1)
    {
    paledit.range_mode = 0;
    str = XmStringCreateLocalized
      ("Range mode: inclusive. Toggle to exclusive mode");
    }
  else
    {
    paledit.range_mode = 1;
    str = XmStringCreateLocalized
      ("Range mode: exclusive. Toggle to inclusive mode");
    }

  button = XtNameToWidget(editpal_options_dialog,
    "*coloredit_range_menu.button_1");
  XtVaSetValues(button, XmNlabelString, str, NULL);
  XmStringFree(str);
}

/*ARGSUSED*/
static void set_cycle_range(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget cyclerange_dialog = NULL;
  Widget cyclerange_dflt_button;
  Widget cyclerange_done_button;
  Widget cyclerange_help_button;
  Widget cyclerange_cancel_button;
  Widget cyclerange_reset_button;
  Arg args[MAX_ARGS];
  XmString str;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 5 - 1;

  if(!cyclerange_shell)
    {
    cyclerange_shell = XmCreateDialogShell
      (main_window, "cyclerange_shell", NULL, 0);

    XtAddCallback(cyclerange_shell, XmNpopupCallback,  map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(cyclerange_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Select color cycling range");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    cyclerange_dialog = XmCreateForm
      (cyclerange_shell, "cyclerange_dialog", args, n);

    XtAddCallback(cyclerange_dialog, XmNmapCallback, map_dialog, NULL);
    XmStringFree(str);

    /*
     * Color cycling from color (0 ... 254)
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Color cycling from color");
    XtSetArg(args[n], XmNmaximum, colors - 2); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    cycle_from_scale =
      XmCreateScale (cyclerange_dialog, "cycle_from_scale", args, n);

    XtManageChild(cycle_from_scale);
    XmStringFree(str);

    /*
     * Color cycling to color
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Color cycling to color)");
    XtSetArg(args[n], XmNmaximum, colors - 1); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, cycle_from_scale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    cycle_to_scale =
      XmCreateScale (cyclerange_dialog, "cycle_to_scale", args, n);

    XtManageChild(cycle_to_scale);
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
    XtSetArg(args[n], XmNtopWidget, cycle_to_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    cyclerange_help_button = XmCreatePushButton
      (cyclerange_dialog, "cyclerange_help_button", args, n);

    XtAddCallback(cyclerange_help_button, XmNactivateCallback,
      cyclerange_help_button_cb, NULL);

    XtManageChild(cyclerange_help_button);
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
    XtSetArg(args[n], XmNtopWidget, cycle_to_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    cyclerange_reset_button = XmCreatePushButton
      (cyclerange_dialog, "cyclerange_reset_button", args, n);

    XtAddCallback(cyclerange_reset_button, XmNactivateCallback,
      cyclerange_reset_button_cb, NULL);

    XtManageChild(cyclerange_reset_button);
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
    XtSetArg(args[n], XmNtopWidget, cycle_to_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    cyclerange_dflt_button = XmCreatePushButton
      (cyclerange_dialog, "cyclerange_dflt_button", args, n);

    XtAddCallback(cyclerange_dflt_button, XmNactivateCallback,
      cyclerange_dflt_button_cb, NULL);

    XtManageChild(cyclerange_dflt_button);
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
    XtSetArg(args[n], XmNtopWidget, cycle_to_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    cyclerange_cancel_button = XmCreatePushButton
      (cyclerange_dialog, "cyclerange_cancel_button", args, n);

    XtAddCallback(cyclerange_cancel_button, XmNactivateCallback,
      cyclerange_cancel_button_cb, NULL);

    XtManageChild(cyclerange_cancel_button);
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
    XtSetArg(args[n], XmNtopWidget, cycle_to_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    cyclerange_done_button = XmCreatePushButton
      (cyclerange_dialog, "cyclerange_done_button", args, n);

    XtAddCallback(cyclerange_done_button, XmNactivateCallback,
      cyclerange_done_button_cb, NULL);

    XtManageChild(cyclerange_done_button);
    XmStringFree(str);

    XtVaSetValues(cyclerange_dialog,
      XmNdefaultButton,
      cyclerange_done_button,
      NULL);
    }

  XtVaSetValues(cycle_from_scale,
    XmNvalue, (int)extended_opts.current.rotate_lo,
    NULL);

  XtVaSetValues(cycle_to_scale,
    XmNvalue, (int)extended_opts.current.rotate_hi,
    NULL);

  XtManageChild(cyclerange_dialog);
  XtManageChild(cyclerange_shell);
  XtPopup(cyclerange_shell, XtGrabNone);
}

/*ARGSUSED*/
static void cyclerange_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(cycle_from_scale,
    (int)extended_opts.current.rotate_lo);

  XmScaleSetValue(cycle_to_scale,
    (int)extended_opts.current.rotate_hi);
}

/*ARGSUSED*/
static void cyclerange_help_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPEDITPAL;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void cyclerange_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int tmpval;

  XmScaleGetValue(cycle_from_scale, &(tmpval));

  extended_opts.current.rotate_lo = tmpval;

  XmScaleGetValue(cycle_to_scale, &(tmpval));

  extended_opts.current.rotate_hi = tmpval;

  rotate_lo = extended_opts.current.rotate_lo;
  rotate_hi = extended_opts.current.rotate_hi;

  XtPopdown(cyclerange_shell);
}

/*ARGSUSED*/
static void cyclerange_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(cycle_from_scale, (int) extended_opts.dflt.rotate_lo);
  XmScaleSetValue(cycle_to_scale, (int) extended_opts.dflt.rotate_hi);
}

/*ARGSUSED*/
static void cyclerange_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(cyclerange_shell != NULL)
    XtPopdown(cyclerange_shell);
}

/*ARGSUSED*/
static void set_freestyle_values(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget freestyle_dialog = NULL;

  Widget form;
  Widget label;
  Widget button;
  Widget radio_box;
  Widget modeframe;
  Widget wrapframe;
  Widget colorframe;
  Arg args[MAX_ARGS];
  XmString str;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 6 - 1;

  if(!freestyle_shell)
    {
    freestyle_shell = XmCreateDialogShell
      (main_window, "freestyle_shell", NULL, 0);

    XtAddCallback(freestyle_shell, XmNpopupCallback,  map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(freestyle_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Freestyle Mode");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    freestyle_dialog = XmCreateForm
      (freestyle_shell, "freestyle_dialog", args, n);

    XtAddCallback(freestyle_dialog, XmNmapCallback, map_dialog, NULL);
    XmStringFree(str);

    /*
     * Freestyle mode
     * (radio buttons)
     * On
     * Off
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    modeframe = XmCreateFrame
      (freestyle_dialog, "freestyle_mode_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Color Editor Freestyle Mode");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (modeframe, "freestyle_mode_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

    radio_box = XmCreateRadioBox
      (modeframe, "freestyle_radio_box", args, n);

    XtManageChild(radio_box);

    n = 0;
    str = XmStringCreateLocalized("On");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    freestyle_mode_on_toggle = XmCreateToggleButton
      (radio_box, "freestyle_mode_on_toggle", args, n);

    XtAddCallback(freestyle_mode_on_toggle, XmNvalueChangedCallback,
      toggle_freestyle_mode, "1");

    XtManageChild(freestyle_mode_on_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Off");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    freestyle_mode_off_toggle = XmCreateToggleButton
      (radio_box, "freestyle_mode_off_toggle", args, n);

    XtAddCallback(freestyle_mode_off_toggle, XmNvalueChangedCallback,
       toggle_freestyle_mode, "0");

    XtManageChild(freestyle_mode_off_toggle);
    XmStringFree(str);

    XtManageChild(modeframe);

    /*
     * Freestyle wraparound mode
     * (radio buttons)
     * On
     * Off
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, modeframe); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    wrapframe = XmCreateFrame
      (freestyle_dialog, "freestyle_wrap_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Freestyle Wraparound Mode");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (wrapframe, "freestyle_wrap_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

    radio_box = XmCreateRadioBox
      (wrapframe, "freestyle_radio_box", args, n);

    XtManageChild(radio_box);

    n = 0;
    str = XmStringCreateLocalized("On");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    freestyle_wrap_on_toggle = XmCreateToggleButton
      (radio_box, "freestyle_wrap_on_toggle", args, n);

    XtAddCallback(freestyle_wrap_on_toggle, XmNvalueChangedCallback,
      toggle_freestyle_wrap, "1");

    XtManageChild(freestyle_wrap_on_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Off");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    freestyle_wrap_off_toggle = XmCreateToggleButton
      (radio_box, "freestyle_wrap_off_toggle", args, n);

    XtAddCallback(freestyle_wrap_off_toggle, XmNvalueChangedCallback,
       toggle_freestyle_wrap, "0");

    XtManageChild(freestyle_wrap_off_toggle);
    XmStringFree(str);

    XtManageChild(wrapframe);

    /*
     * Center color register
     */

    /*
     * frame
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    colorframe = XmCreateFrame(freestyle_dialog,
      "freestyle_color_register_frame", args, n);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    form = XmCreateForm(colorframe, "freestyle_color_register_form", args, n);

    /*
     * Red
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("R");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;

    freestyle_red_scale =
      XmCreateScale(form, "freestyle_red_scale", args, n);

    XtAddCallback(freestyle_red_scale, XmNvalueChangedCallback,
      edit_center_color, NULL);

    XtAddCallback(freestyle_red_scale, XmNdragCallback,
      edit_center_color, NULL);

    XtManageChild(freestyle_red_scale);
    XmStringFree(str);

    /*
     * Green
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("G");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, (btnbase * 2)); n++;

    freestyle_green_scale =
      XmCreateScale(form, "freestyle_green_scale", args, n);

    XtAddCallback(freestyle_green_scale, XmNvalueChangedCallback,
      edit_center_color, NULL);

    XtAddCallback(freestyle_green_scale, XmNdragCallback,
      edit_center_color, NULL);

    XtManageChild(freestyle_green_scale);
    XmStringFree(str);

    /*
     * Blue
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("B");
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition,
      (btnbase * 2) + ((btnbase * 2) - 1)); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

    freestyle_blue_scale = XmCreateScale
      (form, "freestyle_blue_scale", args, n);

    XtAddCallback(freestyle_blue_scale, XmNvalueChangedCallback,
      edit_center_color, NULL);

    XtAddCallback(freestyle_blue_scale, XmNdragCallback,
      edit_center_color, NULL);

    XtManageChild(freestyle_blue_scale);
    XmStringFree(str);

    /*
     * Color index: nnn
     * (label)
     */

    n = 0;
    str = XmStringCreateLocalized("Color index:");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_blue_scale); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;

    freestyle_static_label = XmCreateLabel
      (form, "freestyle_register_static_label", args, n);

    XtManageChild(freestyle_static_label);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("222");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_static_label); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    freestyle_dynamic_label = XmCreateLabelGadget
      (form, "freestyle_dynamic_label", args, n);

    XtManageChild(freestyle_dynamic_label);
    XmStringFree(str);

    /*
     * Color register colorbox
     */

    n = 0;
    XtSetArg(args[n], XmNwidth, 60); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_static_label); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    freestyle_register_colorbox = XtCreateManagedWidget
      ("freestyle_register_colorbox", xmDrawingAreaWidgetClass,
        form, args, n);

    XtAddCallback(freestyle_register_colorbox, XmNexposeCallback,
      redraw_freestyle_colorbox, NULL);

    XtManageChild(form);

    /*
     * Palette index to use as center color
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Palette index to use as center color");
    XtSetArg(args[n], XmNmaximum, colors - 1); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, wrapframe); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    freestyle_color_scale =
      XmCreateScale (freestyle_dialog, "freestyle_color_scale", args, n);

    XtAddCallback(freestyle_color_scale, XmNvalueChangedCallback,
      freestyle_color_cb, NULL);

    XtAddCallback(freestyle_color_scale, XmNdragCallback,
      freestyle_color_cb, NULL);

    XtManageChild(freestyle_color_scale);
    XmStringFree(str);

    XtVaSetValues(colorframe,
      XmNbottomAttachment, XmATTACH_WIDGET,
      XmNbottomWidget, freestyle_color_scale,
      NULL);

    XtManageChild(colorframe);

    /*
     * Left (low) endpoint color for transform (-1 = auto)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Left (low) endpoint color for transform (-1 = auto)");
    XtSetArg(args[n], XmNmaximum, colors - 1); n++;
    XtSetArg(args[n], XmNminimum, -1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_color_scale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    freestyle_endcolor_lo_scale =
      XmCreateScale (freestyle_dialog, "freestyle_endcolor_lo_scale", args, n);

    XtAddCallback(freestyle_endcolor_lo_scale, XmNvalueChangedCallback,
      freestyle_endcolor_lo_cb, NULL);

    XtAddCallback(freestyle_endcolor_lo_scale, XmNdragCallback,
      freestyle_endcolor_lo_cb, NULL);

    XtManageChild(freestyle_endcolor_lo_scale);
    XmStringFree(str);

    /*
     * Right (high) endpoint color for transform (-1 = auto)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Right (high) endpoint color for transform (-1 = auto)");
    XtSetArg(args[n], XmNmaximum, colors - 1); n++;
    XtSetArg(args[n], XmNminimum, -1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_endcolor_lo_scale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    freestyle_endcolor_hi_scale =
      XmCreateScale (freestyle_dialog, "freestyle_endcolor_hi_scale", args, n);

    XtAddCallback(freestyle_endcolor_hi_scale, XmNvalueChangedCallback,
      freestyle_endcolor_hi_cb, NULL);

    XtAddCallback(freestyle_endcolor_hi_scale, XmNdragCallback,
      freestyle_endcolor_hi_cb, NULL);

    XtManageChild(freestyle_endcolor_hi_scale);
    XmStringFree(str);

    /*
     * Range of colors from center
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Range of colors (from center)");
    XtSetArg(args[n], XmNmaximum, ((colors / 2) - 1)); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_endcolor_hi_scale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    freestyle_range_scale =
      XmCreateScale (freestyle_dialog, "freestyle_range_scale", args, n);

    XtAddCallback(freestyle_range_scale, XmNvalueChangedCallback,
      freestyle_range_cb, NULL);

    XtAddCallback(freestyle_range_scale, XmNdragCallback,
      freestyle_range_cb, NULL);

    XtManageChild(freestyle_range_scale);
    XmStringFree(str);

    /*
     * Transparency of the overlayed colors (%)
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Transparency of the overlayed colors (%)");
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_range_scale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    freestyle_transp_scale =
      XmCreateScale (freestyle_dialog, "freestyle_transp_scale", args, n);

    XtAddCallback(freestyle_transp_scale, XmNvalueChangedCallback,
      freestyle_transparency_cb, NULL);

    XtAddCallback(freestyle_transp_scale, XmNdragCallback,
      freestyle_transparency_cb, NULL);

    XtManageChild(freestyle_transp_scale);
    XmStringFree(str);

    /*
     * Help
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 0); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_transp_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (freestyle_dialog, "freestyle_help_button", args, n);

    XtAddCallback(button, XmNactivateCallback,
      freestyle_help_button_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_transp_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (freestyle_dialog, "freestyle_apply_button", args, n);

    XtAddCallback(button, XmNactivateCallback,
      freestyle_done_button_cb, "0");

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_transp_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (freestyle_dialog, "freestyle_reset_button", args, n);

    XtAddCallback(button, XmNactivateCallback,
      freestyle_reset_button_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_transp_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (freestyle_dialog, "freestyle_dflt_button", args, n);

    XtAddCallback(button, XmNactivateCallback,
      freestyle_dflt_button_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_transp_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (freestyle_dialog, "freestyle_cancel_button", args, n);

    XtAddCallback(button, XmNactivateCallback,
      freestyle_cancel_button_cb, NULL);

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
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, freestyle_transp_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 5 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (freestyle_dialog, "freestyle_done_button", args, n);

    XtAddCallback(button, XmNactivateCallback,
      freestyle_done_button_cb, "1");

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(freestyle_dialog,
      XmNdefaultButton,
      button,
      NULL);
    }

  if(paledit.freestyle_mode == 0)
    {
    XtVaSetValues(freestyle_mode_on_toggle, XmNset, False, NULL);
    XtVaSetValues(freestyle_mode_off_toggle, XmNset, True, NULL);
    }
  else
    {
    XtVaSetValues(freestyle_mode_on_toggle, XmNset, True, NULL);
    XtVaSetValues(freestyle_mode_off_toggle, XmNset, False, NULL);
    }

  if(freestyle.wraparound == 0)
    {
    XtVaSetValues(freestyle_wrap_on_toggle, XmNset, False, NULL);
    XtVaSetValues(freestyle_wrap_off_toggle, XmNset, True, NULL);
    }
  else
    {
    XtVaSetValues(freestyle_wrap_on_toggle, XmNset, True, NULL);
    XtVaSetValues(freestyle_wrap_off_toggle, XmNset, False, NULL);
    }

  if(freestyle.color < 0)
    freestyle.color = colors - 1;

  if((int)((colors / 2) - 1) < freestyle.range)
    freestyle.range = (int)((colors / 2) - 1);

  XtVaSetValues(freestyle_range_scale,
    XmNvalue, (int) freestyle.range, NULL);

  XtVaSetValues(freestyle_color_scale,
    XmNvalue, (int) freestyle.color, NULL);

  XtVaSetValues(freestyle_endcolor_hi_scale,
    XmNvalue, (int) freestyle.endcolor_hi, NULL);

  XtVaSetValues(freestyle_endcolor_lo_scale,
    XmNvalue, (int) freestyle.endcolor_lo, NULL);

  XtVaSetValues(freestyle_transp_scale,
    XmNvalue, (int) freestyle.transparency, NULL);

  XtManageChild(freestyle_dialog);
  XtManageChild(freestyle_shell);
  XtPopup(freestyle_shell, XtGrabNone);

  if(freestyle_colorbox_gc)
    XFreeGC(display, freestyle_colorbox_gc);

  freestyle_colorbox_gc = XCreateGC(XtDisplay(canvas),
    XtWindow(freestyle_register_colorbox), 0, NULL);

  if(privatecolor)
    XSetWindowColormap(display, XtWindow(freestyle_shell), colormap);

  freestyle_shell_popped_up = 1;
}

/*ARGSUSED*/
static void redraw_freestyle_colorbox(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char num[4];
  XmString str;
  int color = 0;

  color = freestyle.color;

  sprintf((char *) &num, "%3d", color);
  str = XmStringCreateLocalized((char *) &num);
  XtVaSetValues(freestyle_dynamic_label,
    XmNlabelString, str,
    NULL);
  XmStringFree(str);

  XtVaSetValues(freestyle_register_colorbox,
    XmNx, 90,
    XmNy, 114,
    XmNwidth, 60,
    XmNheight, 25,
    NULL);

  XSetForeground(display, freestyle_colorbox_gc,
    BlackPixelOfScreen(screen));

  XDrawRectangle(display, XtWindow(freestyle_register_colorbox),
    freestyle_colorbox_gc, 0, 0, 59, 24);

  XSetForeground(display, freestyle_colorbox_gc, pixtab[color]);

  XFillRectangle(display, XtWindow(freestyle_register_colorbox),
    freestyle_colorbox_gc, 0, 0, 59, 24);

  XSetForeground(display, freestyle_colorbox_gc,
    WhitePixelOfScreen(screen));

  XmScaleSetValue(freestyle_red_scale,
    paledit.palette_colorbox_rect[color].intred);
  XmScaleSetValue(freestyle_green_scale,
    paledit.palette_colorbox_rect[color].intgreen);
  XmScaleSetValue(freestyle_blue_scale,
    paledit.palette_colorbox_rect[color].intblue);

  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void edit_center_color(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *scs = (XmScaleCallbackStruct *)call_data;

  if(w == freestyle_red_scale)
    dacbox[freestyle.color][0] = (unsigned char) scs->value;
  else if(w == freestyle_blue_scale)
    dacbox[freestyle.color][2] = (unsigned char) scs->value;
  else if(w == freestyle_green_scale)
    dacbox[freestyle.color][1] = (unsigned char) scs->value;

  cols[freestyle.color].red = dacbox[freestyle.color][0] * 256;
  cols[freestyle.color].blue = dacbox[freestyle.color][2] * 256;
  cols[freestyle.color].green = dacbox[freestyle.color][1] * 256;

  XStoreColor(display, colormap, &cols[freestyle.color]);

  paledit.palette_colorbox_rect[freestyle.color].red =
    dacbox[freestyle.color][0];

  paledit.palette_colorbox_rect[freestyle.color].blue =
    dacbox[freestyle.color][2];

  paledit.palette_colorbox_rect[freestyle.color].green =
    dacbox[freestyle.color][1];

  paledit.palette_colorbox_rect[freestyle.color].intred =
    (int)cols[freestyle.color].red / 256;

  paledit.palette_colorbox_rect[freestyle.color].intblue =
    (int)cols[freestyle.color].blue / 256;

  paledit.palette_colorbox_rect[freestyle.color].intgreen =
    (int)cols[freestyle.color].green / 256;

  if(freestyle.color == paledit.active_color_register_color)
    {
    if(paledit.active_color_register == 0)
      {
      XmScaleSetValue(paledit.primary_red_scale,
        paledit.palette_colorbox_rect[freestyle.color].intred);
      XmScaleSetValue(paledit.primary_blue_scale,
        paledit.palette_colorbox_rect[freestyle.color].intblue);
      XmScaleSetValue(paledit.primary_green_scale,
        paledit.palette_colorbox_rect[freestyle.color].intgreen);
      }
    else
      {
      XmScaleSetValue(paledit.secondary_red_scale,
        paledit.palette_colorbox_rect[freestyle.color].intred);
      XmScaleSetValue(paledit.secondary_blue_scale,
        paledit.palette_colorbox_rect[freestyle.color].intblue);
      XmScaleSetValue(paledit.secondary_green_scale,
        paledit.palette_colorbox_rect[freestyle.color].intgreen);
      }
    }
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
  if(scs->reason == XmCR_VALUE_CHANGED)
    update_undo_index();
}

/*ARGSUSED*/
static void freestyle_color_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  freestyle.color = cbs->value;
  redraw_freestyle_colorbox(main_window, NULL, NULL);
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void freestyle_endcolor_hi_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  freestyle.endcolor_hi = (int)(cbs->value);
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void freestyle_endcolor_lo_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  freestyle.endcolor_lo = (int)(cbs->value);
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void freestyle_range_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  freestyle.range = cbs->value;
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void freestyle_transparency_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  freestyle.transparency = cbs->value;
  freestyle.botmult = ((float)freestyle.transparency / 100.0);
  freestyle.topmult = ((float)(100.0 - (float)freestyle.transparency) / 100.0);
}

/*ARGSUSED*/
static void toggle_freestyle_mode(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmString str;
  char tempstr[128];
  static int was = 0;

  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    paledit.freestyle_mode = atoi((char *) client_data);

  if(was == paledit.freestyle_mode)
    return;

  was = paledit.freestyle_mode;

  sprintf((char *) &tempstr, "Color Editor (%3d colors)   Freestyle mode %s",
    colors, (paledit.freestyle_mode == 1 ? "On" : "Off"));
  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(editpal_options_dialog, XmNdialogTitle, str, NULL);
  XmStringFree(str);

  if(paledit.freestyle_mode == 0)
    {
    if(freestyle.wraparound == 1)
      {
      XmToggleButtonSetState(freestyle_wrap_on_toggle, False, False);
      XmToggleButtonSetState(freestyle_wrap_off_toggle, True, False);
      freestyle.wraparound = 0;
      }
    freestyle_cleanup();
    }
  else
    {
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
    ok_to_cycle = 0;
    }
}

/*ARGSUSED*/
static void toggle_freestyle_wrap(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmString str;
  char tempstr[128];
  static int was = 0;

  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    freestyle.wraparound = atoi((char *) client_data);

  if(was == freestyle.wraparound)
    return;

  was = freestyle.wraparound;

  if(freestyle.wraparound == 0)
    freestyle_cleanup();
  else
    {
    if(paledit.freestyle_mode == 0)
      {
      XmToggleButtonSetState(freestyle_mode_on_toggle, True, False);
      XmToggleButtonSetState(freestyle_mode_off_toggle, False, False);
      paledit.freestyle_mode = 1;
      sprintf((char *) &tempstr,
        "Color Editor (%3d colors)   Freestyle mode %s",
        colors, (paledit.freestyle_mode == 1 ? "On" : "Off"));
      str = XmStringCreateLocalized((char *) &tempstr);
      XtVaSetValues(editpal_options_dialog, XmNdialogTitle, str, NULL);
      XmStringFree(str);
      }
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
    ok_to_cycle = 0;
    }
}

static void freestyle_setcolors(int lo, int hi)
{
  int n, j;
  double hi_rm, hi_gm, hi_bm;
  double lo_rm, lo_gm, lo_bm;
  unsigned char hi_red, hi_blue, hi_green;
  unsigned char lo_red, lo_blue, lo_green;

  if(paledit.inactive_frozen == 1)
    unfreeze_inactive_register();

  if(lo == -1)
    lo = 0;

  if(hi == -1)
    hi = 0;

  freestyle.center = freestyle.range;

  freestyle.gamma_rect[freestyle.center].red =
    paledit.palette_colorbox_rect[freestyle.color].red;

  freestyle.gamma_rect[freestyle.center].blue =
    paledit.palette_colorbox_rect[freestyle.color].blue;

  freestyle.gamma_rect[freestyle.center].green =
    paledit.palette_colorbox_rect[freestyle.color].green;

  hi_red = paledit.palette_colorbox_rect[hi].red;
  hi_blue = paledit.palette_colorbox_rect[hi].blue;
  hi_green = paledit.palette_colorbox_rect[hi].green;

  lo_red = paledit.palette_colorbox_rect[lo].red;
  lo_blue = paledit.palette_colorbox_rect[lo].blue;
  lo_green = paledit.palette_colorbox_rect[lo].green;

  hi_rm = (double)((int)hi_red -
    (int)freestyle.gamma_rect[freestyle.center].red) /
    (freestyle.range + 2);

  hi_bm = (double)((int) hi_blue -
    (int)freestyle.gamma_rect[freestyle.center].blue) /
    (freestyle.range + 2);

  hi_gm = (double)((int) hi_green -
    (int)freestyle.gamma_rect[freestyle.center].green) /
    (freestyle.range + 2);

  lo_rm = (double)((int)lo_red -
    (int)freestyle.gamma_rect[freestyle.center].red) /
    (freestyle.range + 2);

  lo_bm = (double)((int) lo_blue -
    (int)freestyle.gamma_rect[freestyle.center].blue) /
    (freestyle.range + 2);

  lo_gm = (double)((int) lo_green -
    (int)freestyle.gamma_rect[freestyle.center].green) /
    (freestyle.range + 2);

  for(n = freestyle.range, j = 0; n < (freestyle.range * 2 + 1); n++, j++)
    {
    if(paledit.gamma_val == 1.0)
      {
      freestyle.gamma_rect[n].red =
        freestyle.gamma_rect[freestyle.center].red == hi_red ?
        freestyle.gamma_rect[freestyle.center].red :
        (int)freestyle.gamma_rect[freestyle.center].red +
        (int)(hi_rm * j);

      freestyle.gamma_rect[n].green =
        freestyle.gamma_rect[freestyle.center].green == hi_green ?
        freestyle.gamma_rect[freestyle.center].green :
        (int)freestyle.gamma_rect[freestyle.center].green +
        (int)(hi_gm * j);

      freestyle.gamma_rect[n].blue =
        freestyle.gamma_rect[freestyle.center].blue == hi_blue ?
        freestyle.gamma_rect[freestyle.center].blue :
        (int)freestyle.gamma_rect[freestyle.center].blue +
        (int)(hi_bm * j);
      }
    else
      {
      freestyle.gamma_rect[n].red =
        freestyle.gamma_rect[freestyle.center].red == hi_red ?
        freestyle.gamma_rect[freestyle.center].red :
        (unsigned char)(freestyle.gamma_rect[freestyle.center].red +
        (unsigned char)(pow(j/(double)(freestyle.range + 1),
        (double) paledit.gamma_val) * (freestyle.range + 2) * hi_rm));

      freestyle.gamma_rect[n].green =
        freestyle.gamma_rect[freestyle.center].green == hi_green ?
        freestyle.gamma_rect[freestyle.center].green :
        (unsigned char)(freestyle.gamma_rect[freestyle.center].green +
        (unsigned char)(pow(j/(double)(freestyle.range + 1),
        (double) paledit.gamma_val) * (freestyle.range + 2) * hi_gm));

      freestyle.gamma_rect[n].blue =
        freestyle.gamma_rect[freestyle.center].blue == hi_blue ?
        freestyle.gamma_rect[freestyle.center].blue :
        (unsigned char)(freestyle.gamma_rect[freestyle.center].blue +
        (unsigned char)(pow(j/(double)(freestyle.range + 1),
        (double) paledit.gamma_val) * (freestyle.range + 2) * hi_bm));
      }
    }

  for(n = freestyle.range - 1, j = 1; n >= 0; n--, j++)
    {
    if(paledit.gamma_val == 1.0)
      {
      freestyle.gamma_rect[n].red =
        freestyle.gamma_rect[freestyle.center].red == lo_red ?
        freestyle.gamma_rect[freestyle.center].red :
        (int)freestyle.gamma_rect[freestyle.center].red +
        (int)(lo_rm * j);

      freestyle.gamma_rect[n].green =
        freestyle.gamma_rect[freestyle.center].green == lo_green ?
        freestyle.gamma_rect[freestyle.center].green :
        (int)freestyle.gamma_rect[freestyle.center].green +
        (int)(lo_gm * j);

      freestyle.gamma_rect[n].blue =
        freestyle.gamma_rect[freestyle.center].blue == lo_blue ?
        freestyle.gamma_rect[freestyle.center].blue :
        (int)freestyle.gamma_rect[freestyle.center].blue +
        (int)(lo_bm * j);
      }
    else
      {
      freestyle.gamma_rect[n].red =
        freestyle.gamma_rect[freestyle.center].red == lo_red ?
        freestyle.gamma_rect[freestyle.center].red :
        (unsigned char)(freestyle.gamma_rect[freestyle.center].red +
        (unsigned char)(pow(j/(double)(freestyle.range + 1),
        (double) paledit.gamma_val) * (freestyle.range + 2) * lo_rm));

      freestyle.gamma_rect[n].green =
        freestyle.gamma_rect[freestyle.center].green == lo_green ?
        freestyle.gamma_rect[freestyle.center].green :
        (unsigned char)(freestyle.gamma_rect[freestyle.center].green +
        (unsigned char)(pow(j/(double)(freestyle.range + 1),
        (double) paledit.gamma_val) * (freestyle.range + 2) * lo_gm));

      freestyle.gamma_rect[n].blue =
        freestyle.gamma_rect[freestyle.center].blue == lo_blue ?
        freestyle.gamma_rect[freestyle.center].blue :
        (unsigned char)(freestyle.gamma_rect[freestyle.center].blue +
        (unsigned char)(pow(j/(double)(freestyle.range + 1),
        (double) paledit.gamma_val) * (freestyle.range + 2) * lo_bm));
      }
    }
}

static void freestyle_cleanup()
{
  int n;

  for(n = 0; n < colors; n++)
    {
    if(freestyle.dirty[n] == 1)
      {
      dacbox[n][0] = paledit.palette_colorbox_rect[n].red;
      dacbox[n][2] = paledit.palette_colorbox_rect[n].blue;
      dacbox[n][1] = paledit.palette_colorbox_rect[n].green;
      cols[n].red = dacbox[n][0] * 256;
      cols[n].blue = dacbox[n][2] * 256;
      cols[n].green = dacbox[n][1] * 256;
      XStoreColor(display, colormap, &cols[n]);
      paledit.palette_colorbox_rect[n].intred =
        (int)cols[n].red / 256;
      paledit.palette_colorbox_rect[n].intblue =
        (int)cols[n].blue / 256;
      paledit.palette_colorbox_rect[n].intgreen =
        (int)cols[n].green / 256;
      freestyle.dirty[n] = 0;
      }
   }

  if(paledit.freestyle_mode == 0)
    ok_to_cycle = 1;
}

/*ARGSUSED*/
static void freestyle_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(freestyle_range_scale, freestyle.range);
  XmScaleSetValue(freestyle_color_scale, freestyle.color);
  XmScaleSetValue(freestyle_endcolor_hi_scale, freestyle.endcolor_hi);
  XmScaleSetValue(freestyle_endcolor_lo_scale, freestyle.endcolor_lo);
  XmScaleSetValue(freestyle_transp_scale, freestyle.transparency);
  redraw_freestyle_colorbox(main_window, NULL, NULL);
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
  else
    freestyle_cleanup();
}

/*ARGSUSED*/
static void freestyle_help_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPEDITPAL;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void freestyle_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int tmpval;
  int pop_down = atoi((char *) client_data);

  XmScaleGetValue(freestyle_range_scale, &(tmpval));
  freestyle.range = tmpval;

  XmScaleGetValue(freestyle_color_scale, &(tmpval));
  freestyle.color = tmpval;

  XmScaleGetValue(freestyle_endcolor_hi_scale, &(tmpval));
  freestyle.endcolor_hi = tmpval;

  XmScaleGetValue(freestyle_endcolor_lo_scale, &(tmpval));
  freestyle.endcolor_lo = tmpval;

  XmScaleGetValue(freestyle_transp_scale, &(tmpval));
  freestyle.transparency = tmpval;

  if(pop_down == 1)
    {
    XtPopdown(freestyle_shell);
    freestyle_shell_popped_up = 0;
    }
  else if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void freestyle_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(freestyle_range_scale, 6);
  XmScaleSetValue(freestyle_color_scale, colors - 1);
  XmScaleSetValue(freestyle_endcolor_hi_scale, 0);
  XmScaleSetValue(freestyle_endcolor_lo_scale, 0);
  XmScaleSetValue(freestyle_transp_scale, 50);
  freestyle.range = 6;
  freestyle.color = colors - 1;
  freestyle.transparency = 50;
  redraw_freestyle_colorbox(main_window, NULL, NULL);
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void freestyle_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  freestyle_cleanup();
  paledit.freestyle_mode = 0;
  freestyle.wraparound = 0;

  if(freestyle_shell != NULL)
    XtPopdown(freestyle_shell);

  freestyle_shell_popped_up = 0;
}

/*ARGSUSED*/
static void set_gamma()
{
  static Widget gammaval_dialog = NULL;
  Widget gammaval_dflt_button;
  Widget gammaval_done_button;
  Widget gammaval_help_button;
  Widget gammaval_cancel_button;
  Widget gammaval_reset_button;
  Arg args[MAX_ARGS];
  XmString str;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 5 - 1;

  if(!gammaval_shell)
    {
    gammaval_shell = XmCreateDialogShell
      (main_window, "gammaval_shell", NULL, 0);

    XtAddCallback(gammaval_shell, XmNpopupCallback,  map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(gammaval_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Set gamma value");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    gammaval_dialog = XmCreateForm
      (gammaval_shell, "gammaval_dialog", args, n);

    XtAddCallback(gammaval_dialog, XmNmapCallback, map_dialog, NULL);
    XmStringFree(str);

    /*
     * Gamma value
     * (scale)
     *
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Gamma value");
    XtSetArg(args[n], XmNmaximum, 4000); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNdecimalPoints, 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    gammaval_scale =
      XmCreateScale (gammaval_dialog, "gammaval_scale", args, n);

    XtAddCallback(gammaval_scale, XmNvalueChangedCallback,
      freestyle_gamma_cb, NULL);

    XtAddCallback(gammaval_scale, XmNdragCallback,
      freestyle_gamma_cb, NULL);

    XtManageChild(gammaval_scale);
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
    XtSetArg(args[n], XmNtopWidget, gammaval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    gammaval_help_button = XmCreatePushButton
      (gammaval_dialog, "gammaval_help_button", args, n);

    XtAddCallback(gammaval_help_button, XmNactivateCallback,
      gammaval_help_button_cb, NULL);

    XtManageChild(gammaval_help_button);
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
    XtSetArg(args[n], XmNtopWidget, gammaval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    gammaval_reset_button = XmCreatePushButton
      (gammaval_dialog, "gammaval_reset_button", args, n);

    XtAddCallback(gammaval_reset_button, XmNactivateCallback,
      gammaval_reset_button_cb, NULL);

    XtManageChild(gammaval_reset_button);
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
    XtSetArg(args[n], XmNtopWidget, gammaval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    gammaval_dflt_button = XmCreatePushButton
      (gammaval_dialog, "gammaval_dflt_button", args, n);

    XtAddCallback(gammaval_dflt_button, XmNactivateCallback,
      gammaval_dflt_button_cb, NULL);

    XtManageChild(gammaval_dflt_button);
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
    XtSetArg(args[n], XmNtopWidget, gammaval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    gammaval_cancel_button = XmCreatePushButton
      (gammaval_dialog, "gammaval_cancel_button", args, n);

    XtAddCallback(gammaval_cancel_button, XmNactivateCallback,
      gammaval_cancel_button_cb, NULL);

    XtManageChild(gammaval_cancel_button);
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
    XtSetArg(args[n], XmNtopWidget, gammaval_scale); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    gammaval_done_button = XmCreatePushButton
      (gammaval_dialog, "gammaval_done_button", args, n);

    XtAddCallback(gammaval_done_button, XmNactivateCallback,
      gammaval_done_button_cb, NULL);

    XtManageChild(gammaval_done_button);
    XmStringFree(str);

    XtVaSetValues(gammaval_dialog,
      XmNdefaultButton, gammaval_done_button,
      NULL);
    }

  XtVaSetValues(gammaval_scale,
    XmNvalue, (int)freestyle.gamma,
    NULL);

  XtManageChild(gammaval_dialog);
  XtManageChild(gammaval_shell);
  XtPopup(gammaval_shell, XtGrabNone);
}

/*ARGSUSED*/
static void gammaval_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(gammaval_scale,
    (int)freestyle.gamma);
}

/*ARGSUSED*/
static void gammaval_help_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPEDITPAL;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void gammaval_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  double normalized;
  XmScaleGetValue(gammaval_scale, &freestyle.gamma);
  normalized = (double)(freestyle.gamma / 1000.0);
  if(normalized == 0.0)
    normalized = 0.001;
  paledit.gamma_val = (double)(1./normalized);
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
  XtPopdown(gammaval_shell);
}

/*ARGSUSED*/
static void gammaval_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleSetValue(gammaval_scale, 1000);
  freestyle.gamma = 1000;
  paledit.gamma_val = 1.0;
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void gammaval_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(gammaval_shell != NULL)
    XtPopdown(gammaval_shell);
}

/*ARGSUSED*/
static void freestyle_gamma_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  double normalized;
  freestyle.gamma = cbs->value;
  normalized = (double)(freestyle.gamma / 1000.0);
  if(normalized == 0.0)
    normalized = 0.001;
  paledit.gamma_val = (double)(1./normalized);
  if(paledit.freestyle_mode == 1)
    freestyle_setcolors(freestyle.endcolor_lo, freestyle.endcolor_hi);
}

/*ARGSUSED*/
static void rand_map_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget randmap_dialog = NULL;
  Widget randmap_apply_button;
  Widget randmap_dflt_button;
  Widget randmap_done_button;
  Widget randmap_help_button;
  Widget randmap_cancel_button;
  Widget randmap_reset_button;
  Widget segframe;
  Widget modeframe;
  Widget radio_box;
  Widget label;
  Arg args[MAX_ARGS];
  XmString str;
  int fracbase;
  int btnbase;
  int n;

  btnbase = 20;
  fracbase = btnbase * 6 - 1;

  if(!randmap_shell)
    {
    randmap_shell = XmCreateDialogShell
      (main_window, "Randomize color palette", NULL, 0);

    XtAddCallback(randmap_shell, XmNpopupCallback,  map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(randmap_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Randomize color palette");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;

    randmap_dialog = XmCreateForm
      (randmap_shell, "randmap_dialog", args, n);

    XtAddCallback(randmap_dialog, XmNmapCallback, map_dialog, NULL);
    XmStringFree(str);

    /*
     * Randomization mode
     * (radio buttons)
     * Block mode (darker end first)
     * Reverse mode (brighter end first)
     * Alternate mode (darker end first)
     * Alternate mode (brighter end first)
     * Blend mode (darker end first)
     * Blend mode (brighter end first)
     * Smooth mode (darker end first)
     * Smooth mode (brighter end first)
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase / 2 - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    modeframe = XmCreateFrame
      (randmap_dialog, "randmap_mode_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Palette randomization mode");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (modeframe, "randmap_mode_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

    radio_box = XmCreateRadioBox
      (modeframe, "randmap_mode_radio_box", args, n);

    XtManageChild(radio_box);

    n = 0;
    str = XmStringCreateLocalized("Block mode (darker end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_0_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_0_toggle", args, n);

    XtAddCallback(randmap_mode_0_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "0");

    XtManageChild(randmap_mode_0_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Reverse mode (brighter end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_1_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_1_toggle", args, n);

    XtAddCallback(randmap_mode_1_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "1");

    XtManageChild(randmap_mode_1_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Alternate mode (darker end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_2_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_2_toggle", args, n);

    XtAddCallback(randmap_mode_2_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "2");

    XtManageChild(randmap_mode_2_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Alternate mode (brighter end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_3_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_3_toggle", args, n);

    XtAddCallback(randmap_mode_3_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "3");

    XtManageChild(randmap_mode_3_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Blend mode (darker end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_4_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_4_toggle", args, n);

    XtAddCallback(randmap_mode_4_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "4");

    XtManageChild(randmap_mode_4_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Blend mode (brighter end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_5_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_5_toggle", args, n);

    XtAddCallback(randmap_mode_5_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "5");

    XtManageChild(randmap_mode_5_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Smooth mode (darker end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_6_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_6_toggle", args, n);

    XtAddCallback(randmap_mode_6_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "6");

    XtManageChild(randmap_mode_6_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Smooth mode (brighter end first)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_mode_7_toggle = XmCreateToggleButton
      (radio_box, "randmap_mode_7_toggle", args, n);

    XtAddCallback(randmap_mode_7_toggle, XmNvalueChangedCallback,
      toggle_randmap_mode, "7");

    XtManageChild(randmap_mode_7_toggle);
    XmStringFree(str);

    /*
     * Randomization function
     * (radio buttons)
     * One segment
     * Two segments
     * 16-color segments
     * 8-color segments
     * Random segments (largest)
     * Random segments (large)
     * Random segments (small)
     * Random segments (smallest)
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, fracbase / 2 + 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    segframe = XmCreateFrame
      (randmap_dialog, "randmap_func_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Palette randomization function");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (segframe, "randmap_func_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;

    radio_box = XmCreateRadioBox
      (segframe, "randmap_func_radio_box", args, n);

    XtManageChild(radio_box);

    n = 0;
    str = XmStringCreateLocalized("One segment");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_1_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_1_toggle", args, n);

    XtAddCallback(randmap_func_1_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "1");

    XtManageChild(randmap_func_1_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Two segments");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_2_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_2_toggle", args, n);

    XtAddCallback(randmap_func_2_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "2");

    XtManageChild(randmap_func_2_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("16-color segments");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_3_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_3_toggle", args, n);

    XtAddCallback(randmap_func_3_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "3");

    XtManageChild(randmap_func_3_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("8-color segments");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_4_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_4_toggle", args, n);

    XtAddCallback(randmap_func_4_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "4");

    XtManageChild(randmap_func_4_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Random segments (largest)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_5_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_5_toggle", args, n);

    XtAddCallback(randmap_func_5_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "5");

    XtManageChild(randmap_func_5_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Random segments (large)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_6_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_6_toggle", args, n);

    XtAddCallback(randmap_func_6_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "6");

    XtManageChild(randmap_func_6_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Random segments (small)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_7_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_7_toggle", args, n);

    XtAddCallback(randmap_func_7_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "7");

    XtManageChild(randmap_func_7_toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Random segments (smallest)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    randmap_func_8_toggle = XmCreateToggleButton
      (radio_box, "randmap_func_8_toggle", args, n);

    XtAddCallback(randmap_func_8_toggle, XmNvalueChangedCallback,
      toggle_randmap_func, "8");

    XtManageChild(randmap_func_8_toggle);
    XmStringFree(str);

    XtManageChild(segframe);

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
    XtSetArg(args[n], XmNtopWidget, segframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    randmap_help_button = XmCreatePushButton
      (randmap_dialog, "randmap_help_button", args, n);

    XtAddCallback(randmap_help_button, XmNactivateCallback,
      randmap_help_button_cb, NULL);

    XtManageChild(randmap_help_button);
    XmStringFree(str);

    /*
     * Apply
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Apply");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, segframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    randmap_apply_button = XmCreatePushButton
      (randmap_dialog, "randmap_apply_button", args, n);

    XtAddCallback(randmap_apply_button, XmNactivateCallback,
      randmap_apply_button_cb, NULL);

    XtManageChild(randmap_apply_button);
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
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, segframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    randmap_reset_button = XmCreatePushButton
      (randmap_dialog, "randmap_reset_button", args, n);

    XtAddCallback(randmap_reset_button, XmNactivateCallback,
      randmap_reset_button_cb, NULL);

    XtManageChild(randmap_reset_button);
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
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, segframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    randmap_dflt_button = XmCreatePushButton
      (randmap_dialog, "randmap_dflt_button", args, n);

    XtAddCallback(randmap_dflt_button, XmNactivateCallback,
      randmap_dflt_button_cb, NULL);

    XtManageChild(randmap_dflt_button);
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
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, segframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    randmap_cancel_button = XmCreatePushButton
      (randmap_dialog, "randmap_cancel_button", args, n);

    XtAddCallback(randmap_cancel_button, XmNactivateCallback,
      randmap_cancel_button_cb, NULL);

    XtManageChild(randmap_cancel_button);
    XmStringFree(str);

    /*
     * Close
     * (pushbutton)
     *
     */

    n = 0;
    str = XmStringCreateLocalized("Close");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 5); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, segframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    randmap_done_button = XmCreatePushButton
      (randmap_dialog, "randmap_done_button", args, n);

    XtAddCallback(randmap_done_button, XmNactivateCallback,
      randmap_done_button_cb, NULL);

    XtManageChild(randmap_done_button);
    XmStringFree(str);

    XtVaSetValues(randmap_dialog,
      XmNdefaultButton,
      randmap_done_button,
      NULL);

    XtVaSetValues(modeframe,
      XmNbottomAttachment,
      XmATTACH_WIDGET,
      XmNbottomWidget,
      randmap_done_button,
      NULL);

    XtManageChild(modeframe);

    randmap_mode = usr_randmap_mode = 5;
    randmap_func = usr_randmap_func = 4;

    XmToggleButtonSetState(randmap_mode_0_toggle, False, False);
    XmToggleButtonSetState(randmap_mode_1_toggle, False, False);
    XmToggleButtonSetState(randmap_mode_2_toggle, False, False);
    XmToggleButtonSetState(randmap_mode_3_toggle, False, False);
    XmToggleButtonSetState(randmap_mode_4_toggle, False, False);
    XmToggleButtonSetState(randmap_mode_5_toggle, True,  False);
    XmToggleButtonSetState(randmap_mode_6_toggle, False, False);
    XmToggleButtonSetState(randmap_mode_7_toggle, False, False);

    XmToggleButtonSetState(randmap_func_1_toggle, False, False);
    XmToggleButtonSetState(randmap_func_2_toggle, False, False);
    XmToggleButtonSetState(randmap_func_3_toggle, False, False);
    XmToggleButtonSetState(randmap_func_4_toggle, True,  False);
    XmToggleButtonSetState(randmap_func_5_toggle, False, False);
    XmToggleButtonSetState(randmap_func_6_toggle, False, False);
    XmToggleButtonSetState(randmap_func_7_toggle, False, False);
    XmToggleButtonSetState(randmap_func_8_toggle, False, False);
    }

  XtManageChild(randmap_dialog);
  XtManageChild(randmap_shell);
  XtPopup(randmap_shell, XtGrabNone);

  randshell_popped_up = 1;

  randmap_reset_button_cb(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void toggle_randmap_mode(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    usr_randmap_mode = atoi((char *) client_data);
}

/*ARGSUSED*/
static void toggle_randmap_func(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    usr_randmap_func = atoi((char *) client_data);
}

/*ARGSUSED*/
static void randmap_reset_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmToggleButtonSetState(randmap_mode_0_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_1_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_2_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_3_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_4_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_5_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_6_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_7_toggle, False, False);

  XmToggleButtonSetState(randmap_func_1_toggle, False, False);
  XmToggleButtonSetState(randmap_func_2_toggle, False, False);
  XmToggleButtonSetState(randmap_func_3_toggle, False, False);
  XmToggleButtonSetState(randmap_func_4_toggle, False, False);
  XmToggleButtonSetState(randmap_func_5_toggle, False, False);
  XmToggleButtonSetState(randmap_func_6_toggle, False, False);
  XmToggleButtonSetState(randmap_func_7_toggle, False, False);
  XmToggleButtonSetState(randmap_func_8_toggle, False, False);

  switch(usr_randmap_mode)
    {
    case 0:
      XmToggleButtonSetState(randmap_mode_0_toggle, True, False);
    break;
    case 1:
      XmToggleButtonSetState(randmap_mode_1_toggle, True, False);
    break;
    case 2:
      XmToggleButtonSetState(randmap_mode_2_toggle, True, False);
    break;
    case 3:
      XmToggleButtonSetState(randmap_mode_3_toggle, True, False);
    break;
    case 4:
      XmToggleButtonSetState(randmap_mode_4_toggle, True, False);
    break;
    case 5:
      XmToggleButtonSetState(randmap_mode_5_toggle, True, False);
    break;
    case 6:
      XmToggleButtonSetState(randmap_mode_6_toggle, True, False);
    break;
    case 7:
      XmToggleButtonSetState(randmap_mode_7_toggle, True, False);
    break;
    }

  switch(usr_randmap_func)
    {
    case 1:
      XmToggleButtonSetState(randmap_func_1_toggle, True, False);
    break;
    case 2:
      XmToggleButtonSetState(randmap_func_2_toggle, True, False);
    break;
    case 3:
      XmToggleButtonSetState(randmap_func_3_toggle, True, False);
    break;
    case 4:
      XmToggleButtonSetState(randmap_func_4_toggle, True, False);
    break;
    case 5:
      XmToggleButtonSetState(randmap_func_5_toggle, True, False);
    break;
    case 6:
      XmToggleButtonSetState(randmap_func_6_toggle, True, False);
    break;
    case 7:
      XmToggleButtonSetState(randmap_func_7_toggle, True, False);
    break;
    case 8:
      XmToggleButtonSetState(randmap_func_8_toggle, True, False);
    break;
    }
}

/*ARGSUSED*/
static void randmap_help_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPLOADFILE;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void randmap_apply_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int tmpmode;
  int tmpfunc;

  tmpmode = usr_randmap_mode;
  tmpfunc = usr_randmap_func;

  if(XmToggleButtonGetState(randmap_mode_0_toggle) == True)
    tmpmode = 0;
  else if(XmToggleButtonGetState(randmap_mode_1_toggle) == True)
    tmpmode = 1;
  else if(XmToggleButtonGetState(randmap_mode_2_toggle) == True)
    tmpmode = 2;
  else if(XmToggleButtonGetState(randmap_mode_3_toggle) == True)
    tmpmode = 3;
  else if(XmToggleButtonGetState(randmap_mode_4_toggle) == True)
    tmpmode = 4;
  else if(XmToggleButtonGetState(randmap_mode_5_toggle) == True)
    tmpmode = 5;
  else if(XmToggleButtonGetState(randmap_mode_6_toggle) == True)
    tmpmode = 6;
  else if(XmToggleButtonGetState(randmap_mode_7_toggle) == True)
    tmpmode = 7;

  if(XmToggleButtonGetState(randmap_func_1_toggle) == True)
    tmpfunc = 1;
  else if(XmToggleButtonGetState(randmap_func_2_toggle) == True)
    tmpfunc = 2;
  else if(XmToggleButtonGetState(randmap_func_3_toggle) == True)
    tmpfunc = 3;
  else if(XmToggleButtonGetState(randmap_func_4_toggle) == True)
    tmpfunc = 4;
  else if(XmToggleButtonGetState(randmap_func_5_toggle) == True)
    tmpfunc = 5;
  else if(XmToggleButtonGetState(randmap_func_6_toggle) == True)
    tmpfunc = 6;
  else if(XmToggleButtonGetState(randmap_func_7_toggle) == True)
    tmpfunc = 7;
  else if(XmToggleButtonGetState(randmap_func_8_toggle) == True)
    tmpfunc = 8;

  randomize_cycle_palette(tmpfunc, tmpmode);
}

/*ARGSUSED*/
static void randmap_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  randmap_func = usr_randmap_func;
  randmap_mode = usr_randmap_mode;
  XtPopdown(randmap_shell);
  randshell_popped_up = 0;
}

/*ARGSUSED*/
static void randmap_dflt_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmToggleButtonSetState(randmap_mode_0_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_1_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_2_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_3_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_4_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_5_toggle, True,  False);
  XmToggleButtonSetState(randmap_mode_6_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_7_toggle, False, False);

  XmToggleButtonSetState(randmap_func_1_toggle, False, False);
  XmToggleButtonSetState(randmap_func_2_toggle, False, False);
  XmToggleButtonSetState(randmap_func_3_toggle, False, False);
  XmToggleButtonSetState(randmap_func_4_toggle, True, False);
  XmToggleButtonSetState(randmap_func_5_toggle, False, False);
  XmToggleButtonSetState(randmap_func_6_toggle, False, False);
  XmToggleButtonSetState(randmap_func_7_toggle, False, False);
  XmToggleButtonSetState(randmap_func_8_toggle, False, False);

  usr_randmap_mode = 5;
  usr_randmap_func = 4;
}

/*ARGSUSED*/
static void randmap_cancel_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(randmap_shell != NULL)
    XtPopdown(randmap_shell);
  randshell_popped_up = 0;
}

void update_randmap_dialog()
{
  XmToggleButtonSetState(randmap_mode_0_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_1_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_2_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_3_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_4_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_5_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_6_toggle, False, False);
  XmToggleButtonSetState(randmap_mode_7_toggle, False, False);

  if(shiftkey && metakey)
    XmToggleButtonSetState(randmap_mode_5_toggle, True, False);
  else if(shiftkey && ctrlkey)
    XmToggleButtonSetState(randmap_mode_4_toggle, True, False);
  else if(metakey)
    XmToggleButtonSetState(randmap_mode_3_toggle, True, False);
  else if(ctrlkey)
    XmToggleButtonSetState(randmap_mode_2_toggle, True, False);
  else if(shiftkey)
    XmToggleButtonSetState(randmap_mode_1_toggle, True, False);
  else
    {
    switch(usr_randmap_mode)
      {
      case 0:
        XmToggleButtonSetState(randmap_mode_0_toggle, True, False);
      break;
      case 1:
        XmToggleButtonSetState(randmap_mode_1_toggle, True, False);
      break;
      case 2:
        XmToggleButtonSetState(randmap_mode_2_toggle, True, False);
      break;
      case 3:
        XmToggleButtonSetState(randmap_mode_3_toggle, True, False);
      break;
      case 4:
        XmToggleButtonSetState(randmap_mode_4_toggle, True, False);
      break;
      case 5:
        XmToggleButtonSetState(randmap_mode_5_toggle, True, False);
      break;
      case 6:
        XmToggleButtonSetState(randmap_mode_6_toggle, True, False);
      break;
      case 7:
        XmToggleButtonSetState(randmap_mode_7_toggle, True, False);
      break;
      }
    }
}

void update_undo_index()
{
  XmString str;
  char tempstr[128];
  Widget button;

  static int warned = 0;

  if(undo_max > 0)
    {
    if(++paledit.undo_index > undo_max)
      {
      if(!warned)
        {
        char msg[] = "Sorry - Out of memory for palette editor undo.\nFurther color edits will not be undo-able.\nUse the Apply or Done button to reset the undo memory.\nThis message will not be repeated.";
        warned = confirm(main_window, (char *) &msg,
          OK_BTN, XmDIALOG_OK_BUTTON);
        }
      return;
      }

    paledit.redo_index = 0;

    memcpy(undobox + (paledit.undo_index * 768), dacbox, 768);

    sprintf((char *) &tempstr,
      "Undo last edit (%5d undo's available)           ",
       paledit.undo_index);
    str = XmStringCreateLocalized((char *) &tempstr);
    button = XtNameToWidget(editpal_options_dialog,
      "*coloredit_edit_menu*button_1");
    if(button != NULL)
      XtVaSetValues(button, XmNlabelString, str, NULL);
    XmStringFree(str);

    sprintf((char *) &tempstr,
      "Redo last undo (%5d redo's available)           ",
       paledit.redo_index);
    str = XmStringCreateLocalized((char *) &tempstr);
    button = XtNameToWidget(editpal_options_dialog,
      "*coloredit_edit_menu*button_2");
    if(button != NULL)
      XtVaSetValues(button, XmNlabelString, str, NULL);
    XmStringFree(str);
    }
  else
    return;
}

static void undo_last_color_edit()
{
  XmString str;
  char tempstr[128];
  Widget button;

  if(undo_max > 0)
    {
    if((paledit.undo_index - 1) >= 0)
      {
      ++paledit.redo_index;
      --paledit.undo_index;
      memcpy(dacbox, undobox + (paledit.undo_index * 768), 768);
      }

    sprintf((char *) &tempstr,
      "Undo last edit (%5d undo's available)           ",
       paledit.undo_index);
    str = XmStringCreateLocalized((char *) &tempstr);
    button = XtNameToWidget(editpal_options_dialog,
      "*coloredit_edit_menu*button_1");
    if(button != NULL)
      XtVaSetValues(button, XmNlabelString, str, NULL);
    XmStringFree(str);

    sprintf((char *) &tempstr,
      "Redo last undo (%5d redo's available)           ",
       paledit.redo_index);
    str = XmStringCreateLocalized((char *) &tempstr);
    button = XtNameToWidget(editpal_options_dialog,
      "*coloredit_edit_menu*button_2");
    if(button != NULL)
      XtVaSetValues(button, XmNlabelString, str, NULL);
    XmStringFree(str);

    writevideopalette();
    redraw_palette_colorbox(editpal_options_dialog, NULL, NULL);
    }
  else
    return;
}

static void redo_last_color_edit()
{
  XmString str;
  char tempstr[128];
  Widget button;

  if(undo_max > 0)
    {
    if((paledit.redo_index - 1) >= 0)
      {
      ++paledit.undo_index;
      --paledit.redo_index;
      memcpy(dacbox, undobox + (paledit.undo_index * 768), 768);
      }

    sprintf((char *) &tempstr,
      "Undo last edit (%5d undo's available)           ",
       paledit.undo_index);
    str = XmStringCreateLocalized((char *) &tempstr);
    button = XtNameToWidget(editpal_options_dialog,
      "*coloredit_edit_menu*button_1");
    if(button != NULL)
      XtVaSetValues(button, XmNlabelString, str, NULL);
    XmStringFree(str);

    sprintf((char *) &tempstr,
      "Redo last undo (%5d redo's available)           ",
       paledit.redo_index);
    str = XmStringCreateLocalized((char *) &tempstr);
    button = XtNameToWidget(editpal_options_dialog,
      "*coloredit_edit_menu*button_2");
    if(button != NULL)
      XtVaSetValues(button, XmNlabelString, str, NULL);
    XmStringFree(str);

    writevideopalette();
    redraw_palette_colorbox(editpal_options_dialog, NULL, NULL);
    }
  else
    return;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: coloredit.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.47  1995/06/15  21:36:28  darryl
 * Fix typo's in undo labels and reset the map buffer if
 * the Apply button is pressed.
 *
 * Revision 1.46  1995/06/15  21:18:16  darryl
 * Check the reason field of the dragCallbacks before calling
 * update_undo_index() to reduce the number of generated
 * undo commands available.
 *
 * Revision 1.45  1995/06/14  16:54:54  darryl
 * Add one more character to the undo and redo labels to
 * allow for up to 32768 possible undo commands.
 *
 * Revision 1.44  1995/06/14  01:30:07  darryl
 * Add color editor undo-last-command and undo-last-redo and
 * command-line and ini file support for max undo commands.
 *
 * Revision 1.43  1995/06/12  18:12:30  darryl
 * Fix bug where color cycling would remain active when the
 * wraparound mode was toggled on.
 *
 * Revision 1.42  1995/06/12  16:07:19  darryl
 * Disable color cycling when palette editor freestyle
 * mode is active. Re-enable when exiting freestyle mode.
 *
 * Revision 1.41  1995/06/07  19:20:09  darryl
 * Fix problem where the MIN macro was not guarded from
 * previous declarations and that the palette randomization
 * Done and Apply buttons were handled incorrectly.
 *
 * Revision 1.40  1995/05/24  16:28:59  darryl
 * Update help modes.
 *
 * Revision 1.39  1995/05/24  03:45:08  darryl
 * Add the Smooth modes to the palette randomization routines.
 *
 * Revision 1.38  1995/05/23  20:03:00  darryl
 * Fix mode selection in the palette randomization dialog.
 *
 * Revision 1.37  1995/05/23  18:50:55  darryl
 * Fix color editor and palette randomization problems
 * reported in beta testing.
 *
 * Revision 1.36  1995/05/22  06:57:06  darryl
 * Minor bug fixes in color alignment.
 *
 * Revision 1.35  1995/05/21  12:18:59  darryl
 * Add menu item and dialog for palette randomization.
 *
 * Revision 1.34  1995/05/21  09:54:04  darryl
 * Add exclusive range operations and fix storage problems.
 *
 * Revision 1.33  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.32  1995/05/19  08:11:11  darryl
 * Add freestyle wraparound feature.
 *
 * Revision 1.31  1995/05/18  22:55:48  darryl
 * Fix freestyle and ranges conflicts. Fix problem in
 * brighter/darker range selection and processing.
 *
 * Revision 1.30  1995/05/17  16:28:45  darryl
 * Re-order the buttons in the Set Gamma Value dialog.
 *
 * Revision 1.29  1995/05/17  04:20:13  darryl
 * Fix a problem where the center color edits in the
 * freestyle mode were not updating correctly.
 *
 * Revision 1.28  1995/05/17  02:05:36  darryl
 * Re-order the buttons in the cycle range dialog.
 *
 * Revision 1.27  1995/05/16  21:14:04  darryl
 * Update the saved mapdacbox if the Done or Apply
 * button is pressed in the color editor.
 *
 * Revision 1.26  1995/05/16  18:02:13  darryl
 * Add drag callbacks to Scale widgets.
 *
 * Revision 1.25  1995/05/15  19:30:32  darryl
 * Ensure that freestyle color range is less than (colors / 2).
 *
 * Revision 1.24  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.23  1995/05/08  19:44:43  darryl
 * Fix bugs reported in beta test.
 *
 * Revision 1.22  1995/05/08  16:36:45  darryl
 * Update cycle ranges.
 *
 * Revision 1.21  1995/05/04  17:55:21  darryl
 * Fix bug where color editor was not aware of color map
 * changes if the map was loaded from a file or randomized.
 *
 * Revision 1.20  1995/05/04  15:59:34  darryl
 * Enhance the color editor freestyle option.
 *
 * Revision 1.19  1995/05/03  21:29:57  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.18  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
 *
 * Revision 1.17  1995/05/03  01:28:39  darryl
 * Fix bug where undo was incomplete.
 *
 * Revision 1.16  1995/05/02  23:12:55  darryl
 * Enable freestyle option, fix various bugs in the color editor.
 *
 * Revision 1.15  1995/04/30  12:57:19  darryl
 * Color editor enhancements, place hooks for freestyle option.
 *
 * Revision 1.14  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.13  1995/04/27  17:53:47  darryl
 * Change basename to nbasename, change %x to %d in client_data
 * translations, delete unused header file inclusions.
 *
 * Revision 1.12  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
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
 * Revision 1.9  1995/04/03  15:49:33  darryl
 * Add Brighter and Darker buttons to the Range menu.
 * Fix a problem where the color editor cancel button
 * would reset the colors to the default color map
 * after a gif image was loaded. Fix a potential
 * problem where the skipval dialog was not being
 * destroyed when the color editor was popped down.
 *
 * Revision 1.8  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.7  1995/03/27  18:00:56  darryl
 * Fix a bug where the Done button would cancel any edits.
 *
 * Revision 1.6  1995/03/21  17:26:03  darryl
 * Reposition the scales so that they are evenly distributed.
 *
 * Revision 1.5  1995/03/20  18:34:04  darryl
 * Solidify widget heirarchy.
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
 * Revision 0.301  1994/12/21  19:23:20  darryl
 * Color editor enhancements - add a color index indicator
 * and make the active and inactive color register labels
 * dynamic, reflecting the state of the register.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.201  1994/12/09  17:03:53  darryl
 * Create a new xgc when the palette editor exits.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.110  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.109  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.108  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.107  1994/11/29  02:08:16  darryl
 * Reverse the operation of the zoomoff variable so
 * that a non-zero value means that zooming is disabled.
 *
 * Revision 0.106  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.105  1994/11/25  18:18:01  darryl
 * Move the event handlers for the rubber band functions
 * from the initialization section to the image creation
 * section so they can be installed and removed more easily.
 * Prevents event processing errors caused by delays in
 * posting events from the X server that would occasionally
 * make strange and interesting things to happen to the zoom box.
 *
 * Revision 0.104  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.103  1994/11/21  00:43:35  darryl
 * Minor aesthetic changes in widget placement.
 *
 * Revision 0.102  1994/11/19  03:57:23  darryl
 * Aesthetic changes for widget alignment.
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
