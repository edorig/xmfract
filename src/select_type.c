
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: select_type.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void update_helpmode
  (Widget w, XtPointer client_data, XtPointer call_data);
static void select_type_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static int formfract;
static int ifsfract;
static int lsysfract;
static Widget type_selection_dialog;

/*ARGSUSED*/
void create_type_selection_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int n;
  int i;
  int j;
  char msg[128];
  XmString str[2];
  XmString list[MAX_TYPES];
  Arg args[MAX_ARGS];
  struct choice *choices[MAX_TYPES];

  if(!type_selection_dialog)
    {
    for( n = 0; n < MAX_TYPES; n++)
      {
      if( (choices[n] = (struct choice *)
           XtMalloc(sizeof(struct choice))) == (struct choice *) NULL)
        {
        sprintf((char *) &msg, "Sorry, out of memory.\n");
        fatal(main_window, (char *) &msg);
        exit(1);
        }
      list[n] = NULL;
      }

    i = -1;
    j = -1;

    while(fractalspecific[++i].name)
      {
      if (fractalspecific[i].name[0] == '*')
        {
        continue;
        }
      strcpy(choices[++j]->name, fractalspecific[i].name);
      choices[j]->name[14] = 0;
      choices[j]->num = i;
      }

    numtypes = j + 1;

    qsort(choices,numtypes,sizeof(char *),xmcompare);

    for(i = 0; i < numtypes; i++)
      {
      list[i] = XmStringCreateLocalized(choices[i]->name);
      /* find starting choice in sorted list */
      if(choices[i]->num == fractype ||
         choices[i]->num == fractalspecific[fractype].tofloat)
        {
        j = i;
        }
      }

    n = 0;
    str[0] = XmStringCreateLocalized("Select a Fractal Type");
    str[1] = XmStringCreateLocalized("Fractal Types");
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_SELECTION); n++;
    XtSetArg(args[n], XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;
    XtSetArg(args[n], XmNtextString, list[j]); n++;
    XtSetArg(args[n], XmNlistItems, list); n++;
    XtSetArg(args[n], XmNlistItemCount, numtypes); n++;
    XtSetArg(args[n], XmNlistLabelString, str[1]); n++;
    XtSetArg(args[n], XmNlistVisibleItemCount, 10); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNtextColumns, 40); n++;

    type_selection_dialog = XmCreateSelectionDialog
      (main_window, "type_selection_dialog", args, n);

    XtUnmanageChild(XmSelectionBoxGetChild(type_selection_dialog,
      XmDIALOG_APPLY_BUTTON));

    XtAddCallback(type_selection_dialog, XmNapplyCallback,
      select_type_ok_cb, (XtPointer) "0");

    XtAddCallback(type_selection_dialog, XmNokCallback,
      select_type_ok_cb, (XtPointer) "0");

    XtAddCallback(type_selection_dialog, XmNnoMatchCallback,
      select_type_err_cb, NULL);

    XtAddCallback(type_selection_dialog, XmNhelpCallback,
      update_helpmode, (XtPointer) "0");

    XtAddCallback(type_selection_dialog, XmNcancelCallback,
      select_type_cancel_cb, NULL);

    XtAddCallback(XmSelectionBoxGetChild
      (type_selection_dialog, XmDIALOG_TEXT),
      XmNvalueChangedCallback, update_helpmode,
      (XtPointer) "1");

    XtAddCallback(type_selection_dialog, XmNmapCallback,
      map_dialog, NULL);

    XtAddCallback(XtParent(type_selection_dialog), XmNpopupCallback,
      map_dialog, NULL);

    XtManageChild(type_selection_dialog);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(type_selection_dialog), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);

    for( n = 0; n < MAX_TYPES; n++)
      {
      XtFree((char *) choices[n]);
      XmStringFree(list[n]);
      }

    update_helpmode(XmSelectionBoxGetChild
      (type_selection_dialog, XmDIALOG_TEXT),
      (XtPointer) "1", NULL);
    }
  XtPopup(XtParent(type_selection_dialog), XtGrabNone);
}

/*ARGSUSED*/
static void update_helpmode(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int n;
  int found;
  Widget textfield;
  char *curname = NULL;

  int which = atoi((char *)client_data);

  if(which == 1)
    textfield = w;
  else
    textfield = XmSelectionBoxGetChild
      (type_selection_dialog, XmDIALOG_TEXT);

  if((XtIsManaged(textfield)) &&
    (XtIsSubclass(textfield, xmTextFieldWidgetClass)))
    curname = XmTextFieldGetString(textfield);

  if(which == 0)
    {
    if(!curname || !*curname)
      {
      char msg[128];
      sprintf((char *) &msg, "No fractal type selected.");
      caution(main_window, (char *) &msg, NULL);
      }
    }

  n = -1;
  found = 0;
  while(fractalspecific[++n].name)
    {
    if(((int)strcmp(curname, fractalspecific[n].name)) == 0)
      {
      helpmode = fractalspecific[n].helpformula;
      found = 1;
      break;
      }
    }

  if(which == 0)
    {
    if(!found)
      {
      char msg[128];
      sprintf((char *) &msg, "Fractal type \"%s\" does not exist.", curname);
      caution(main_window, (char *) &msg, NULL);
      }
    }

  if(curname)
    XtFree(curname);

  if(which == 0 && found)
    read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
void select_type_err_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget type_selection_error_dialog = NULL;
  Arg args[MAX_ARGS];
  XmString msg[4];
  XmString str;
  int n;

  XmSelectionBoxCallbackStruct
    *cbs = (XmSelectionBoxCallbackStruct *)call_data;

  if(!type_selection_error_dialog)
    {
    n = 0;
    str = XmStringCreateLocalized("Selection error");
    XtSetArg (args[n], XmNnoResize, False); n++;
    XtSetArg (args[n], XmNdefaultPosition, False); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;
    XtSetArg (args[n], XmNdialogTitle, str); n++;

    type_selection_error_dialog = XmCreateWarningDialog
      (main_window, "type_selection_error_dialog", args, n);

    XtAddCallback(type_selection_error_dialog,
      XmNokCallback, response, NULL);

    XtAddCallback(type_selection_error_dialog,
      XmNcancelCallback, response, NULL);

    XtAddCallback(type_selection_error_dialog,
      XmNmapCallback, map_dialog, NULL);

    XtAddCallback(XtParent(type_selection_error_dialog),
      XmNpopupCallback, map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(type_selection_error_dialog), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XtUnmanageChild(XmMessageBoxGetChild
      (type_selection_error_dialog, XmDIALOG_CANCEL_BUTTON));

    XtUnmanageChild(XmMessageBoxGetChild
      (type_selection_error_dialog, XmDIALOG_HELP_BUTTON));

    XmStringFree(str);
    }

  msg[1] = XmStringCreateLocalized("Fractal type \"");
  msg[3] = XmStringCreateLocalized("\" does not exist.");
  msg[2] = XmStringConcat(msg[1], cbs->value);
  msg[0] = XmStringConcat(msg[2], msg[3]);

  XtVaSetValues(type_selection_error_dialog,
    XmNmessageString, msg[0],
    NULL);

  XmStringFree(msg[0]);
  XmStringFree(msg[1]);
  XmStringFree(msg[2]);
  XmStringFree(msg[3]);

  XtManageChild(type_selection_error_dialog);
  XtPopup(XtParent(type_selection_error_dialog), XtGrabNone);
}

/*ARGSUSED*/
void select_type_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*ARGSUSED*/
void select_type_ok_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[64];
  char msg[128];
  XmString name = NULL;
  int entry_point;
  int ret = 0;
  static int i = -1;
  static int n = 0;
  static char typename[80];

  if((entry_point = atoi((char *)client_data)) == 0)
    {
    XmSelectionBoxCallbackStruct
      *cbs = (XmSelectionBoxCallbackStruct *)call_data;

    XtPopdown(XtParent(w));

    curtype = fractype;

    functionpreloaded = 0;

    if(!forwarding)
      set_default_parms();

    n = 0;
    i = -1;
    while(fractalspecific[++i].name)
      {
      sprintf((char *) &tempstr, fractalspecific[i].name);
      name = XmStringCreateLocalized((char *) &tempstr);
      if(XmStringCompare(name, cbs->value) == True )
        {
        curfractalspecific = &fractalspecific[i];
        curtype = fractype = i;
        break;
        }
      }

    if(!forwarding)
      set_default_parms();

    if(((int)strcmp((char *) &typename, curfractalspecific->name)) != 0)
      {
      calc_status = IMG_PRMS_CHGD;
      update_draw_menu(NEW_IMAGE);
      toggle_zoom_grab(ZOOM_OFF);
      }

    sprintf((char *) &typename, curfractalspecific->name);

    corner_val.dflt.xxmin = curfractalspecific->xmin;
    corner_val.dflt.xxmax = curfractalspecific->xmax;
    corner_val.dflt.yymin = curfractalspecific->ymin;
    corner_val.dflt.yymax = curfractalspecific->ymax;

    corner_val.dflt.xx3rd = xxmin;
    corner_val.dflt.yy3rd = yymin;
    }

  if(fractype==JULIBROT || fractype==JULIBROTFP)
    julibrot = 1;
  else
    julibrot = 0;

  if(fractype==FORMULA || fractype==FFORMULA)
    formfract = 1;
  else
    {
    formfract = 0;
    gotfrmfile = 0;
    gotfrmname = 0;
    }

  if(fractype==LSYSTEM)
    lsysfract = 1;
  else
    {
    lsysfract = 0;
    gotlsysfile = 0;
    gotlsysname = 0;
    }

  if(fractype==IFS || fractype==IFS3D)
    ifsfract = 1;
  else
    {
    ifsfract = 0;
    gotifsfile = 0;
    gotifsname = 0;
    }

  for (i = 0; i < 4; i++)
    {
    defparam[i] = curfractalspecific->paramvalue[i];
    if((int)strlen(curfractalspecific->param[i]) >= 1)
      {
      n++;
      }
    }

  if(curfractalspecific->flags & MORE)
    {
    int extra;
    if((extra=find_extra_param(fractype)) > -1)
      {
      for(i=0;i<MAXPARAMS-4;i++)
        {
        defparam[i+4] = moreparams[extra].paramvalue[i];
        n++;
        }
      }
    }

  if(fractype==MANDEL || fractype==MANDELFP)
    hasinverse=1;
  else
    hasinverse=0;

  if(julibrot)
    select_julibrot_params(main_window);
  else if((n > 0) ||
    (curfractalspecific->flags & TRIG1) ||
    (curfractalspecific->flags & TRIG2) ||
    (curfractalspecific->flags & TRIG3) ||
    (curfractalspecific->flags & TRIG4))
    {
    if(formfract)
      {
      if(gotfrmfile && gotfrmname)
        {
        if((ret = RunForm((char *) &FormName)) == 0)
          type_specific_params(main_window);
        else
          {
          sprintf((char *) &msg, "Error loading entry \"%s\".",
            (char *) &FormName);
          caution(main_window, (char *) &msg, NULL);
          }
        }
      else
        select_formula_file(main_window, "1");
      }
    else if(lsysfract)
      {
      if(gotlsysfile && gotlsysname)
        {
        if((ret = LLoad()) == 0)
          type_specific_params(main_window);
        else
          {
          sprintf((char *) &msg, "Error loading entry \"%s\".",
            (char *) &FormName);
          caution(main_window, (char *) &msg, NULL);
          }
        }
      else
        select_lsystem_file(main_window, "1");
      }
    else if(ifsfract)
      {
      if(gotifsfile && gotifsname)
        {
        if((ret = ifsload()) == 0)
          type_specific_params(main_window);
        else
          {
          sprintf((char *) &msg, "Error loading entry \"%s\".",
            (char *) &FormName);
          caution(main_window, (char *) &msg, NULL);
          }
        }
      else
        select_ifs_file(main_window, "1");
      }
    else
      type_specific_params(main_window);
    }
  else
    {
    sprintf((char *) &tempstr, curfractalspecific->name);
    sprintf((char *) &msg,
      "Fractal type \"%s\" needs no parameters", (char *) &tempstr);
    inform(main_window, (char *) &msg, NULL);
    set_default_parms();
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    toggle_zoom_grab(ZOOM_OFF);
    }

  XmStringFree(name);
}

int
find_extra_param(
int type)
{
  int i,ret,curtyp;
  ret = -1;
  i= -1;
  while((curtyp=moreparams[++i].type) != type && curtyp != -1);

  if(curtyp == type)
    ret = i;

  return(ret);
}

/* for qsort */

int
xmcompare(
const void * arg1,
const void * arg2)
{
  return(strncasecmp(*((char **)arg1),*((char **)arg2),40));
}

void
roundfloatd(
double *x)
{
  char buf[30];
  sprintf(buf,"%-10.7g",*x);
  *x = atof(buf);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: select_type.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.20  1995/06/09  19:34:35  darryl
 * Fix bug where global variables were shadowed by local
 * variables of a different type.
 *
 * Revision 1.19  1995/05/10  22:14:04  darryl
 * Delete unused code and variables, fix problems
 * reported in beta test, enhance the file selection
 * dialogs and restore menu.
 *
 * Revision 1.18  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.17  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
 *
 * Revision 1.16  1995/04/21  23:23:16  darryl
 * Fix bugs in client_data handling.
 *
 * Revision 1.15  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.14  1995/04/19  19:36:07  darryl
 * Fix bugs in parameter forwarding options.
 *
 * Revision 1.13  1995/04/18  15:59:59  darryl
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
 * Revision 1.12  1995/03/31  18:47:10  darryl
 * Update parameter file processing so that 3D parameters
 * are handled in the same fashion as other entries.
 *
 * Revision 1.11  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.10  1995/03/28  18:11:40  darryl
 * Fix trig function handling. Enhance center-mag button
 * management. Aesthetic changes in the parameter dialog.
 * Add frames around groups of scale widgets. Fix a bogus
 * message about color cell allocation if sharecolor.
 *
 * Revision 1.9  1995/03/27  20:21:18  darryl
 * Fix precedence of initialization file processing.
 * Fix parameter function initialization.
 *
 * Revision 1.8  1995/03/25  08:42:49  darryl
 * Fixes and enhancements -- make the parameter selection
 * understand that if a parameter file is loaded, then
 * the ifs, lsys, or formula file does not need to be
 * re-selected. Display the current fractal type and
 * any associated file and file entries at the top of
 * the parameters screen. Delete some unused code.
 *
 * Revision 1.7  1995/03/23  21:51:39  darryl
 * Aesthetic changes.
 *
 * Revision 1.6  1995/03/23  20:06:30  darryl
 * Modify help, allow full-path names in the ini file,
 * fix a bug where default trig paramaters were not
 * set correctly for some types.
 *
 * Revision 1.5  1995/03/20  21:30:38  darryl
 * Add an entry point for editing the current fractal type
 * parameters without having to traverse the type selection
 * process. Invoked with a menu item or the 'z' key.
 * The window resize accelerator (previously 'z') is now 'Z'.
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
 * Revision 0.106  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.105  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.104  1994/11/27  01:07:47  darryl
 * Fix bugs where resume was not being handled correctly.
 *
 * Revision 0.103  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.102  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.101  1994/11/23  05:21:08  darryl
 * Initial implementation of help. Minor widget alignment
 * changes. Widget name changes for uniqueness.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
