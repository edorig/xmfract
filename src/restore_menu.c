
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: restore_menu.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void restore_previous_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget prev_img_menu_btn;
static Widget prev_img_menu_max_btn;
static Widget prev_img_menu_ptr_btn;
static Widget prev_img_submenu_btn[21];
static char prev_img_max_label[128];
static char prev_img_ptr_label[128];

/*ARGSUSED*/
void create_restore_menu(Widget w)
{
  int i, n;
  XmString str;
  XmString prev[20];
  Arg args[MAX_ARGS];
  char item_name[128];
  Widget prev_img_button;
  Widget prev_img_menu_toc;
  Widget prev_img_menu_pane;
  Widget prev_img_submenu_toc;
  Widget prev_img_submenu_pane;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  prev_img_menu_pane = XmCreatePulldownMenu(w, "prev_img_menu_pane", args, n);

  /* get the tear-off control */

  if((prev_img_menu_toc = XmGetTearOffControl
    (prev_img_menu_pane)) != NULL)
    {
    XtVaSetValues(prev_img_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Restore' cascade button */

  n = 0;
  str = XmStringCreateLocalized("Restore");
  XtSetArg(args[n], XmNsubMenuId, prev_img_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;
  prev_img_button = XmCreateCascadeButton(w, "prev_img_button", args, n);

  XtManageChild(prev_img_button);

  XmStringFree(str);

  prev[0] =  XmStringCreateLocalized("Current image minus 1");
  prev[1] =  XmStringCreateLocalized("Current image minus 2");
  prev[2] =  XmStringCreateLocalized("Current image minus 3");
  prev[3] =  XmStringCreateLocalized("Current image minus 4");
  prev[4] =  XmStringCreateLocalized("Current image minus 5");
  prev[5] =  XmStringCreateLocalized("Current image minus 6");
  prev[6] =  XmStringCreateLocalized("Current image minus 7");
  prev[7] =  XmStringCreateLocalized("Current image minus 8");
  prev[8] =  XmStringCreateLocalized("Current image minus 9");
  prev[9] =  XmStringCreateLocalized("Current image minus 10");
  prev[10] = XmStringCreateLocalized("Current image plus 1");
  prev[11] = XmStringCreateLocalized("Current image plus 2");
  prev[12] = XmStringCreateLocalized("Current image plus 3");
  prev[13] = XmStringCreateLocalized("Current image plus 4");
  prev[14] = XmStringCreateLocalized("Current image plus 5");
  prev[15] = XmStringCreateLocalized("Current image plus 6");
  prev[16] = XmStringCreateLocalized("Current image plus 7");
  prev[17] = XmStringCreateLocalized("Current image plus 8");
  prev[18] = XmStringCreateLocalized("Current image plus 9");
  prev[19] = XmStringCreateLocalized("Current image plus 10");

  n = 0;
  sprintf((char *) &prev_img_max_label, "%s %d %s",
    "There are", historymax, "saved images");
  str = XmStringCreateLocalized((char *) &prev_img_max_label);
  XtSetArg(args[n], XmNlabelString, str); n++;
  prev_img_menu_max_btn = XmCreatePushButton
      (prev_img_menu_pane, "prev_img_menu_max_btn", args, n);

  XtManageChild(prev_img_menu_max_btn);

  XmStringFree(str);

  n = 0;
  sprintf((char *) &prev_img_ptr_label, "%s %d",
    "Current image is number", historyptr);
  str = XmStringCreateLocalized((char *) &prev_img_ptr_label);
  XtSetArg(args[n], XmNlabelString, str); n++;
  prev_img_menu_ptr_btn = XmCreatePushButton
      (prev_img_menu_pane, "prev_img_menu_ptr_btn", args, n);

  XtManageChild(prev_img_menu_ptr_btn);

  XmStringFree(str);

  n = 0;
  prev_img_submenu_pane = XmCreatePulldownMenu(prev_img_button,
    "prev_img_submenu_pane", args, n);

  /* get the tear-off control */

  if((prev_img_submenu_toc = XmGetTearOffControl
    (prev_img_submenu_pane)) != NULL)
    {
    XtVaSetValues(prev_img_submenu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  n = 0;
  str = XmStringCreateLocalized("Restore which image ");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNsubMenuId, prev_img_submenu_pane); n++;
  prev_img_menu_btn = XmCreateCascadeButton
    (prev_img_menu_pane, "prev_img_menu_btn", args, n);

  XmStringFree(str);

  for(i = 0; i < 21; i++)
    {
    if(i < 10)
      {
      n = 0;
      sprintf((char *) &item_name, "%s%d", "prev_img_submenu_btn_", i);
      sprintf((char *) &restore_which[i], "%d", i);
      XtSetArg(args[n], XmNlabelString, prev[i]); n++;
      prev_img_submenu_btn[i] = XmCreatePushButton
        (prev_img_submenu_pane, (char *) &item_name, args, n);
      XtAddCallback(prev_img_submenu_btn[i], XmNactivateCallback,
        restore_previous_image_cb, (char *) &restore_which[i]);
      }
    else if(i == 10)
      {
      prev_img_submenu_btn[i] = XmCreateSeparator
        (prev_img_submenu_pane, "prev_img_submenu_sep", NULL, 0);
      }
    else
      {
      n = 0;
      sprintf((char *) &item_name, "%s%d", "prev_img_submenu_btn_", i - 1);
      sprintf((char *) &restore_which[i - 1], "%d", i - 1);
      XtSetArg(args[n], XmNlabelString, prev[i - 1]); n++;
      prev_img_submenu_btn[i] = XmCreatePushButton
        (prev_img_submenu_pane, (char *) &item_name, args, n);
      XtAddCallback(prev_img_submenu_btn[i], XmNactivateCallback,
        restore_previous_image_cb, (char *) &restore_which[i - 1]);
      }
    }

  XtManageChild(prev_img_menu_btn);
  XtManageChildren(prev_img_submenu_btn, 21);

  XmStringFree(prev[0]);
  XmStringFree(prev[1]);
  XmStringFree(prev[2]);
  XmStringFree(prev[3]);
  XmStringFree(prev[4]);
  XmStringFree(prev[5]);
  XmStringFree(prev[6]);
  XmStringFree(prev[7]);
  XmStringFree(prev[8]);
  XmStringFree(prev[9]);
  XmStringFree(prev[10]);
  XmStringFree(prev[11]);
  XmStringFree(prev[12]);
  XmStringFree(prev[13]);
  XmStringFree(prev[14]);
  XmStringFree(prev[15]);
  XmStringFree(prev[16]);
  XmStringFree(prev[17]);
  XmStringFree(prev[18]);
  XmStringFree(prev[19]);
}

/*ARGSUSED*/
static void restore_previous_image_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int which = atoi((char *) client_data);
  calc_status = IMG_PRMS_CHGD;
  update_draw_menu(NEW_IMAGE);
  create_previous_image(++which);
}

void update_restore_menu()
{
  int n;
  int have_prev;
  int have_next;
  int need_sep;
  XmString str;

  need_sep = 0;
  have_prev = 0;
  have_next = 0;

  for(n = 0; n < 10; n++)
    {
    if(historyptr > n)
      {
      have_prev = 1;
      XtManageChild(prev_img_submenu_btn[n]);
      }
    else
      XtUnmanageChild(prev_img_submenu_btn[n]);
    }

  for(n = 10; n < 21; n++)
    {
    if(n < 10)
      {
      if(historyptr >= 0 && historymax - historyptr > n - 10)
        have_next = 1;
      }
    else if(n == 10)
      {
      /* do nothing */
      }
    else
      {
      if(historyptr >= 0 && historymax - historyptr > n - 11)
        have_next = 1;
      }
    }

  if(have_prev && have_next)
    need_sep = 1;

  for(n = 10; n < 21; n++)
    {
    if(n < 10)
      {
      if(historyptr >= 0 && historymax - historyptr > n - 10)
        XtManageChild(prev_img_submenu_btn[n]);
      else
        XtUnmanageChild(prev_img_submenu_btn[n]);
      }
    else if(n == 10)
      {
      if(need_sep)
        XtManageChild(prev_img_submenu_btn[n]);
      else
        XtUnmanageChild(prev_img_submenu_btn[n]);
      }
    else
      {
      if(historyptr >= 0 && historymax - historyptr > n - 11)
        XtManageChild(prev_img_submenu_btn[n]);
      else
        XtUnmanageChild(prev_img_submenu_btn[n]);
      }
    }

  XtSetSensitive(prev_img_menu_btn, True);
  XtSetSensitive(prev_img_menu_ptr_btn, True);

  switch(historymax)
    {
    case 0:
      if(historyptr < 0)
        {
        sprintf((char *) &prev_img_max_label, "%s",
          "There are no saved images");
        XtSetSensitive(prev_img_menu_btn, False);
        XtSetSensitive(prev_img_menu_ptr_btn, False);
        }
      else
        {
        sprintf((char *) &prev_img_max_label, "%s",
          "There is 1 saved image");
        XtSetSensitive(prev_img_menu_btn, False);
        XtSetSensitive(prev_img_menu_ptr_btn, True);
        }
    break;

    case 10:
      sprintf((char *) &prev_img_max_label, "%s %d %s",
        "There are", historymax, "saved images");
    break;

    default:
      sprintf((char *) &prev_img_max_label, "%s %d %s",
        "There are", historymax + 1, "saved images");
    break;
    }

  str = XmStringCreateLocalized((char *) &prev_img_max_label);
  XtVaSetValues(prev_img_menu_max_btn, XmNlabelString, str, NULL);
  XmStringFree(str);

  sprintf((char *) &prev_img_ptr_label, "%s %d",
    "Current image is number", historyptr + 1);
  str = XmStringCreateLocalized((char *) &prev_img_ptr_label);
  XtVaSetValues(prev_img_menu_ptr_btn, XmNlabelString, str, NULL);
  XmStringFree(str);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: restore_menu.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.7  1995/05/10  22:14:04  darryl
 * Delete unused code and variables, fix problems
 * reported in beta test, enhance the file selection
 * dialogs and restore menu.
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
 *  -- a saved image was not drawn if the refresh rate was < 100.
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
