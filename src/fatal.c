
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: fatal.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*ARGSUSED*/
int fatal(Widget w, char *msg)
{
  static Widget fatal_dialog = NULL;
  static int answer;
  XtAppContext context;
  XmString text;

  if(!fatal_dialog)
    {
    XmString ok;
    fatal_dialog = XmCreateWarningDialog(main_window, "fatal_dialog", NULL, 0);
    ok = XmStringCreateLocalized("Ok");
    XtVaSetValues(fatal_dialog,
      XmNnoResize, False,
      XmNdefaultPosition, False,
      XmNdialogStyle, XmDIALOG_MODELESS,
      XmNdialogTitle, XmStringCreateLocalized("Bummer"),
      XmNokLabelString, ok,
      NULL);

    XtUnmanageChild (XmMessageBoxGetChild
      (fatal_dialog, XmDIALOG_CANCEL_BUTTON));

    XtAddCallback(fatal_dialog, XmNokCallback, modal_response, &answer);
    XtAddCallback(fatal_dialog, XmNcancelCallback, modal_response, &answer);
    XtAddCallback(fatal_dialog, XmNhelpCallback, read_help, NULL);
    XtAddCallback(fatal_dialog, XmNmapCallback, map_dialog, NULL);

    XtAddCallback(XtParent(fatal_dialog), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(fatal_dialog), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(ok);
    XtManageChild(fatal_dialog);
    }

  answer = 0;

  text = XmStringCreateLtoR(msg, XmFONTLIST_DEFAULT_TAG);

  XtVaSetValues(fatal_dialog,
    XmNmessageString, text,
    NULL);

  XmStringFree(text);

  XtManageChild(fatal_dialog);
  XtPopup(XtParent(fatal_dialog), XtGrabNone);

  context = XtWidgetToApplicationContext(fatal_dialog);

  while(answer == 0 || XtAppPending(context))
    {
    XtAppProcessEvent(context, XtIMAll);
    }

  XtPopdown(XtParent(fatal_dialog));

  return(answer);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: fatal.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.5  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
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
 * Revision 0.102  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.101  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
