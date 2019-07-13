
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: check_mapfile.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void check_mapfile_dialog_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void check_mapfile_dialog_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void check_mapfile_dialog_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget check_mapfile_dialog_shell = NULL;
static Widget check_mapfile_dialog = NULL;
static Widget check_mapfile_textfield = NULL;

/*ARGSUSED*/
void check_mapfile(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!check_mapfile_dialog_shell)
    {
    Widget button;
    Widget label;
    Widget form;
    Widget frame;
    XmString str;
    Arg args[MAX_ARGS];
    int n;
    int fracbase;
    int btnbase;

    btnbase = 20;
    fracbase = btnbase * 3 - 1;

    check_mapfile_dialog_shell = XmCreateDialogShell
      (main_window, "Check Color Map File", NULL, 0);

    XtAddCallback(check_mapfile_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(check_mapfile_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Check Color Map File");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    check_mapfile_dialog = XmCreateForm
      (check_mapfile_dialog_shell, "check_mapfile_dialog", args, n);

    XtAddCallback(check_mapfile_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    /*
     * Output File Name
     * (textfield)
     *
     */

    n = 0;
    XtSetArg(args[n], XmNtopPosition, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (check_mapfile_dialog, "check_mapfile_frame", args, n);

    n = 0;
    str = XmStringCreateLtoR
      ("Enter name of .map file to use, or '*' to\nuse palette from the image to be loaded.", XmFONTLIST_DEFAULT_TAG);
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "check_mapfile_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;

    check_mapfile_textfield = XmCreateTextField
      (frame, "check_mapfile_textfield", args, n);

    XtManageChild(check_mapfile_textfield);
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

    form = XmCreateForm(check_mapfile_dialog, "button_form", args, n);

    /*
     * Help
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (form, "check_mapfile_dialog_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      check_mapfile_dialog_help_btn_cb, NULL);

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
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (form, "check_mapfile_dialog_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      check_mapfile_dialog_cancel_btn_cb, NULL);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    button = XmCreatePushButton
      (form, "check_mapfile_dialog_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      check_mapfile_dialog_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtManageChild(form);

    XtVaSetValues(check_mapfile_dialog, XmNdefaultButton, button, NULL);

    XtManageChild(check_mapfile_dialog);
    }

  if(glassestype)
    {
    if(glassestype == 1)
      strcpy(MAP_name, "glasses1.map");
    else if(glassestype == 2)
      {
      if(FILLTYPE == -1)
        strcpy(MAP_name, "grid.map");
      else
        strcpy(MAP_name, "glasses2.map");
      }
    XmTextFieldSetString(check_mapfile_textfield, (char *) &MAP_name);
    }
  else if(mapset)
    XmTextFieldSetString(check_mapfile_textfield, (char *) &MAP_name);
  else
    XmTextFieldSetString(check_mapfile_textfield, "*");

  XtManageChild(check_mapfile_dialog_shell);
  XtPopup(check_mapfile_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void check_mapfile_dialog_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  int finished;
  char msg[128];
  static char *dflt_name = "*";

  char *curr_name = XmTextFieldGetString(check_mapfile_textfield);

  finished = 0;

  if(mapset)
    {
    if((int)(strcmp(curr_name, dflt_name)) != 0)
      {
      sprintf(curr_name, MAP_name);
      if((int)(strlen(curr_name)) > 3)
        {
        i = ValidateLuts(curr_name);
        if(i <= 0)
          finished = 1;
        }
      else
        {
        sprintf((char *) &msg, "Map file name is empty.");
        caution(main_window, (char *) &msg, NULL);
        }
      }
    else
      {
      mapset = 0;
      finished = 1;
      }
    }
  else
    {
    mapset = 0;
    finished = 1;
    }

  if(!(glassestype == 1 || glassestype == 2))
    {
    if(((int)strcmp(dflt_name, curr_name)) != 0)
      {
      memcpy(olddacbox,dacbox,768); /* save the DAC */
      i = ValidateLuts(curr_name);
      if(i <= 0)
        {
        sprintf(MAP_name, curr_name);
        finished = 1;
        mapset = 1;
        }
      }
    }
  else
    {
    if(glassestype == 1)
      strcpy(curr_name, Glasses1Map);
    else if(glassestype == 2)
      {
      if(FILLTYPE == -1)
        strcpy(curr_name,"grid.map");
      else
        strcpy(curr_name,"glasses2.map");
      }
    memcpy(olddacbox,dacbox,768); /* save the DAC */
    i = ValidateLuts(curr_name);
    if(i <= 0)
      {
      sprintf(MAP_name, curr_name);
      finished = 1;
      mapset = 1;
      }
    }

  XtFree(curr_name);

  if(finished)
    {
    XtPopdown(check_mapfile_dialog_shell);
    select_3d_planar(main_window, NULL, NULL);
    }
  else
    {
    buzzer(0);
    }
}

/*ARGSUSED*/
static void check_mapfile_dialog_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELP3DGLASSES;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void check_mapfile_dialog_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  initvars_fractal();
  XtPopdown(check_mapfile_dialog_shell);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: check_mapfile.c,v $
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
 * Revision 1.7  1995/03/30  02:43:15  darryl
 * Lint and optimization cleanup.
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
