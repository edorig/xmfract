
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: modal.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*ARGSUSED*/
int confirm(
  Widget w,
  char *msg,
  int buttons,
  unsigned char dflt)
{
  static Widget modal_dialog = NULL;
  static int answer;
  XtAppContext context;
  XmString text;
  XmString ok;

  answer = 0;

  if((CANCEL_BTN & buttons))
    {
    modal_dialog = XmCreateQuestionDialog
    (main_window, "modal_dialog", NULL, 0);
    }
  else
    {
    modal_dialog = XmCreateInformationDialog
    (main_window, "modal_dialog", NULL, 0);
    }

  ok = XmStringCreateLocalized("Ok");

  XtVaSetValues(modal_dialog,
    XmNnoResize, False,
    XmNdefaultPosition, False,
    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
    XmNdialogTitle, XmStringCreateLocalized("Confirm"),
    XmNdefaultButtonType, dflt,
    XmNokLabelString, ok,
    NULL);

  XtAddCallback(modal_dialog, XmNokCallback, modal_response, &answer);
  XtAddCallback(modal_dialog, XmNcancelCallback, modal_response, &answer);
  XtAddCallback(modal_dialog, XmNhelpCallback, read_help, NULL);
  XtAddCallback(modal_dialog, XmNmapCallback, map_dialog, NULL);

  if(!(OK_BTN & buttons))
    XtUnmanageChild(XmMessageBoxGetChild
    (modal_dialog, XmDIALOG_OK_BUTTON));

  if(!(CANCEL_BTN & buttons))
    XtUnmanageChild(XmMessageBoxGetChild
    (modal_dialog, XmDIALOG_CANCEL_BUTTON));

  if(!(HELP_BTN & buttons))
    XtUnmanageChild(XmMessageBoxGetChild
    (modal_dialog, XmDIALOG_HELP_BUTTON));

  XtAddCallback(XtParent(modal_dialog), XmNpopupCallback,
    map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(XtParent(modal_dialog), (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  XmStringFree(ok);

  text = XmStringCreateLtoR(msg, XmFONTLIST_DEFAULT_TAG);

  XtVaSetValues(modal_dialog,
    XmNmessageString, text,
    NULL);

  XmStringFree(text);

  XtManageChild(modal_dialog);
  XtPopup(XtParent(modal_dialog), XtGrabNone);

  context = XtWidgetToApplicationContext(modal_dialog);

  while(answer == 0 || XtAppPending(context))
    {
    XtAppProcessEvent(context, XtIMAll);
    }

  XtPopdown(XtParent(modal_dialog));
  XtUnmanageChild(XtParent(modal_dialog));
  XtDestroyWidget(XtParent(modal_dialog));

  return(answer);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: modal.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.6  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.5  1995/04/18  15:59:59  darryl
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
