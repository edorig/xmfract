
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: inform.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*ARGSUSED*/
void inform(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget inform_dialog = NULL;

  XmString msg = XmStringCreateLtoR((char *)client_data,
    XmFONTLIST_DEFAULT_TAG);

  if(!inform_dialog)
    {
    Arg args[MAX_ARGS];
    XmString str;
    int n = 0;

    str = XmStringCreateLocalized("Notice");
    XtSetArg (args[n], XmNnoResize, False); n++;
    XtSetArg (args[n], XmNdefaultPosition, False); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;
    XtSetArg (args[n], XmNdialogTitle, str); n++;

    inform_dialog = XmCreateInformationDialog
      (main_window, "inform_dialog", args, n);

    XtAddCallback(inform_dialog, XmNokCallback, response, NULL);
    XtAddCallback(inform_dialog, XmNcancelCallback, response, NULL);
    XtAddCallback(inform_dialog, XmNhelpCallback, read_help, NULL);
    XtAddCallback(inform_dialog, XmNmapCallback, map_dialog, NULL);
    XtAddCallback(XtParent(inform_dialog), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(inform_dialog), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XtUnmanageChild (XmMessageBoxGetChild
      (inform_dialog, XmDIALOG_CANCEL_BUTTON));

    XmStringFree(str);
    }

  XtVaSetValues(inform_dialog, XmNmessageString, msg, NULL);

  XmStringFree(msg);

  XtManageChild(inform_dialog);
  XtPopup(XtParent(inform_dialog), XtGrabNone);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: inform.c,v $
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
