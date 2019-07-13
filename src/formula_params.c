
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: formula_params.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void get_formula_name_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void update_formula_text
  (Widget w, XtPointer client_data, XtPointer call_data);
static void formula_selection_err_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void formula_selection_ok_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void select_formula_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void formula_selection_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static FILE *formfp;
static int numentries;
static int popped_up = 0;
static char fullfilename[256];
static Widget formula_file_fsb;
static Widget formula_selection_dialog;

/*ARGSUSED*/
void select_formula_file(Widget w, char *getname)
{
  if(!formula_file_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr;
    int n = 0;

    str[0] = XmStringCreateLocalized("Select Formula File");
    str[1] = XmStringCreateLocalized("*.frm");
    dirstr = XmStringCreateLocalized(fractdir);
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    formula_file_fsb = XmCreateFileSelectionDialog (main_window,
      "formula_file_fsb", args, n);

    XtAddCallback(formula_file_fsb, XmNokCallback,
      get_formula_name_cb, (XtPointer) getname);

    XtAddCallback(formula_file_fsb, XmNcancelCallback,
      select_formula_cancel_cb, NULL);

    XtAddCallback(formula_file_fsb, XmNnoMatchCallback, fsb_no_match, NULL);

    XtAddCallback(formula_file_fsb, XmNhelpCallback, read_help, NULL);

    XtAddCallback(formula_file_fsb, XmNmapCallback, map_dialog, NULL);

    XtAddCallback(XtParent(formula_file_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(formula_file_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    XtManageChild(formula_file_fsb);
    }
  helpmode = HT_FORMULA;
  XtPopup(XtParent(formula_file_fsb), XtGrabNone);
}

/*ARGSUSED*/
static void get_formula_name_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  int which = atoi((char *)client_data);

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  XtPopdown(XtParent(formula_file_fsb));

  if(which == 1)
    select_frm_entry(main_window, filename);
  else
    sprintf((char *) &FormFileName, filename);
}

/*ARGSUSED*/
void select_frm_entry(Widget w, char *filename)
{
  long file_offset;
  long name_offset;
  int i, n;
  int this_name;
  char buf[101];
  Arg args[MAX_ARGS];
  XmString list[MAXENTRIES];
  XmString str[3];
  Widget label;
  char *nbasename();
  char msg[128];

  if(popped_up != 0)
    {
    fclose(formfp);
    if(XtIsManaged(formula_selection_dialog))
      XtUnmanageChild(formula_selection_dialog);
    XtDestroyWidget(formula_selection_dialog);
    popped_up = 0;
    }

  if((formfp = fopen(filename, "rb")) == (FILE *) NULL)
    {
    sprintf((char *) &msg, "Can't open command file %s.", filename);
    caution(main_window, (char *) &msg, NULL);
    return;
    }

  numentries = 0;
  file_offset = -1;

  /* scan the file for entry names */
  while(1)
    {
    int c,len;
    do
      {
      ++file_offset;
      c = getc(formfp);
      }
    while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
    if (c == ';')
      {
      do
        {
        ++file_offset;
        c = getc(formfp);
        }
      while (c != '\n' && c != EOF && c != '\032');
      if (c == EOF || c == '\032')
        {
        break;
        }
      continue;
      }
    name_offset = file_offset;
    len = 0;
    while (c != ' ' && c != '\t' && c != '('
        && c != '{' && c != '\n' && c != '\r'
        && c != EOF && c != '\032')
      {
      if (len < 40)
        {
        buf[len++] = c;
        }
       c = getc(formfp);
       ++file_offset;
      }
    buf[len] = 0;
    while (c != '{' && c != '\n' && c != '\r' && c != EOF && c != '\032')
      {
      c = getc(formfp);
      ++file_offset;
      }
    if (c == '{')
      {
      while (c != '}' && c != EOF && c != '\032')
        {
        c = getc(formfp);
        ++file_offset;
        }
      if (c != '}')
        {
        break;
        }
      buf[ITEMNAMELEN] = 0;
      if (buf[0] != 0 && stricmp(buf,"comment") != 0)
        {
        strcpy(choices[numentries]->name,buf);
        choices[numentries]->point = name_offset;
        if (++numentries >= MAXENTRIES)
          {
          sprintf((char *) &msg,
            "Too many entries in file, first %d used",MAXENTRIES);
          inform(main_window, (char *) &msg, NULL);
          break;
          }
        }
      }
    else
      {
      if (c == EOF || c == '\032')
        {
        break;
        }
      }
    }

  if (numentries == 0)
    {
    sprintf((char *) &msg,
      "File \"%s\" doesn't contain any valid entries", filename);
    inform(main_window, (char *) &msg, NULL);
    fclose(formfp);
    return;
    }

  sprintf((char *) &fullfilename, filename);

  if(((int)strcmp((char *) &FormFileName, nbasename(filename))) != 0)
    {
    calc_status = IMG_PRMS_CHGD;
    toggle_zoom_grab(ZOOM_OFF);
    update_draw_menu(NEW_IMAGE);
    sprintf((char *) &oldFormFileName, filename);
    }

  sprintf((char *) &FormFileName, nbasename(filename));

  gotfrmfile = 1;

  qsort((char **)choices,numentries,sizeof(char *),xmcompare);

  for(i = 0; i < numentries; i++)
    list[i] = XmStringCreateLocalized(choices[i]->name);

  this_name = 0;

  if(gotfrmname)
    {
    for(i = 0; i < numentries; i++)
      {
      if(((int)strcasecmp((char *) &FormName, choices[i]->name)) == 0)
        {
        this_name = i;
        break;
        }
      }
    }

  n = 0;
  str[0] = XmStringCreateLocalized("Formula File Entries");
  str[1] = XmStringCreateLocalized("Select Formula Set");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNdialogType, XmDIALOG_SELECTION); n++;
  XtSetArg(args[n], XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;
  XtSetArg(args[n], XmNtextString, list[this_name]); n++;
  XtSetArg(args[n], XmNlistItems, list); n++;
  XtSetArg(args[n], XmNlistItemCount, numentries); n++;
  XtSetArg(args[n], XmNlistVisibleItemCount, 10); n++;
  XtSetArg(args[n], XmNmustMatch, True); n++;
  XtSetArg(args[n], XmNtextColumns, 40); n++;
  XtSetArg(args[n], XmNlistLabelString, str[0]); n++;
  XtSetArg(args[n], XmNdialogTitle, str[1]); n++;

  formula_selection_dialog = XmCreateSelectionDialog
    (main_window, "formula_selection_dialog", args, n);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(XtParent(formula_selection_dialog), (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  XtUnmanageChild (XmSelectionBoxGetChild
    (formula_selection_dialog, XmDIALOG_APPLY_BUTTON));

  XtAddCallback(formula_selection_dialog, XmNapplyCallback,
    formula_selection_ok_cb, NULL);

  XtAddCallback(formula_selection_dialog, XmNokCallback,
    formula_selection_ok_cb, NULL);

  XtAddCallback(formula_selection_dialog, XmNnoMatchCallback,
    formula_selection_err_cb, NULL);

  XtAddCallback(formula_selection_dialog, XmNcancelCallback,
    formula_selection_cancel_cb, NULL);

  XtAddCallback(formula_selection_dialog, XmNunmapCallback,
    formula_selection_cancel_cb, NULL);

  XtAddCallback(formula_selection_dialog, XmNhelpCallback, read_help, NULL);

  XtAddCallback(formula_selection_dialog, XmNmapCallback, map_dialog, NULL);

  XtAddCallback(XtParent(formula_selection_dialog), XmNpopupCallback,
    map_dialog, NULL);

  /*
   * Add a workarea for the formula definition
   */

  XtAddCallback(XmSelectionBoxGetChild
    (formula_selection_dialog, XmDIALOG_TEXT),
    XmNvalueChangedCallback, update_formula_text,
    (XtPointer) "1");

  n = 0;
  str[2] = XmStringCreateLocalized("Symmetry:");
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
  XtSetArg(args[n], XmNlabelString, str[2]); n++;
  label = XmCreateLabel(formula_selection_dialog, "formula_label", args, n);

  XtManageChild(label);
  XmStringFree(str[2]);

  for(i = 0; i < numentries; i++)
    XmStringFree(list[i]);

  XmStringFree(str[0]);
  XmStringFree(str[1]);

  helpmode = HT_FORMULA;

  XtManageChild(formula_selection_dialog);
  XtPopup(XtParent(formula_selection_dialog), XtGrabNone);

  popped_up = 1;

  update_formula_text(XmSelectionBoxGetChild
    (formula_selection_dialog, XmDIALOG_TEXT), NULL, NULL);
}

/*ARGSUSED*/
static void update_formula_text(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  int c;
  int len;
  int index1;
  int index2;
  int nxtlin;
  int foundfrm;
  int foundsym;
  int have_formstr;
  Widget label;
  Widget textfield;
  XmString str[4];
  char buf[1025];
  char formbuf[1025];
  char tempstr[256];
  char *curname = NULL;
  extern struct SYMETRY SymStr[];

  textfield = XmSelectionBoxGetChild(
    formula_selection_dialog, XmDIALOG_TEXT);

  if((XtIsManaged(textfield)) &&
    (XtIsSubclass(textfield, xmTextFieldWidgetClass)))
    curname = XmTextFieldGetString(textfield);

  for(i = 0, foundfrm = 0; i < numentries; i++)
    {
    if((int)(strcasecmp(choices[i]->name, curname)) == 0)
      {
      foundfrm = 1;
      break;
      }
    }

  if(foundfrm)
    {
    sprintf((char *) &tempstr, "%s%s\n\n", "Formula name: ", curname);
    str[0] = XmStringCreateLtoR((char *) &tempstr, XmFONTLIST_DEFAULT_TAG);

    foundsym = 0;
    have_formstr = 0;

    if((formfp = fopen(fullfilename, "rb")) == (FILE *) NULL)
      {
      sprintf((char *) &tempstr, "Can't open formula file %s.", fullfilename);
      caution(main_window, (char *) &tempstr, NULL);
      return;
      }

    while(buf[0] = 0, fscanf(formfp, "%1024[^ \n\t({]", buf) != EOF)
      {
      if(!stricmp(buf, curname))
        {
        while((c = getc(formfp)) != EOF)
          {
          if(c == '(')
            {
            buf[0] = 0;
            fscanf(formfp, "%1024[^)]", buf);
            for(i = 0; SymStr[i].s[0]; i++)
              {
              if(!stricmp(SymStr[i].s, buf))
                {
                foundsym = 1;
                break;
                }
              }
            }
          else if(c == '{')
            {
            if((fscanf(formfp, "%1024[^}]", buf) != EOF))
              {
              strcpy((char *) &formbuf, (char *) &buf);
              have_formstr = 1;
              break;
              }
            }
          }
        }
      if(have_formstr)
        break;
      fscanf(formfp, "%1024[ \n\t({]", buf);
      }

    fclose(formfp);

    if(foundsym)
      sprintf((char *) &tempstr, "%s%s\n\n", "Symmetry: ", SymStr[i].s);
    else
      sprintf((char *) &tempstr, "%s%s\n\n", "Symmetry: ", "None");

    str[1] = XmStringCreateLtoR((char *) &tempstr, XmFONTLIST_DEFAULT_TAG);

    if(have_formstr)
      {
      /*
       * Pad the end of each line to 80 chars to prevent
       * a disconcerting change of width in the selection
       * dialog when the label is changed. Height will change
       * depending on the number of lines in the formula
       * definition but that is acceptable.
       */
      len = 0;
      index1 = 0;
      index2 = 0;
      buf[0] = '\0';
      nxtlin = 0;

      while(index1 < (int)strlen(formbuf))
        {
        if(nxtlin == 0)
          {
          if(formbuf[index1] != ';')
            while(formbuf[++index1] == ' ');
          nxtlin = 1;
          }
        else
          {
          while(formbuf[index1] != '\n' && formbuf[index1] != '\r')
            {
            if(index1 >= (int)strlen(formbuf))
              break;
            buf[index2++] = formbuf[index1++];
            len++;
            }
          while(++len < 80)
            buf[index2++] = ' ';
          buf[index2++] = '\n';
          nxtlin = 0;
          }
        }

      buf[index2] = '\0';

      sprintf(formbuf, "%s\n\n%s", "Formula definition:", (char *) &buf);
      str[2] = XmStringCreateLtoR((char *) &formbuf, XmFONTLIST_DEFAULT_TAG);

      str[3] = XmStringConcat(str[0], str[1]);
      str[3] = XmStringConcat(str[3], str[2]);

      sprintf((char *) &tempstr, "%s", "*formula_label");
      label = XtNameToWidget(XtParent(formula_selection_dialog),
        (char *) &tempstr);
      if(label != NULL)
        {
        XtVaSetValues(label, XmNlabelString, str[3], NULL);
        XtManageChild(label);
        }
      XmStringFree(str[0]);
      XmStringFree(str[1]);
      XmStringFree(str[2]);
      XmStringFree(str[3]);
      }
    }

  if(curname)
    XtFree(curname);
}

/*ARGSUSED*/
static void formula_selection_err_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget formula_selection_error_dialog = NULL;
  Arg args[MAX_ARGS];
  XmString msg[4];
  XmString str;
  int n;

  XmSelectionBoxCallbackStruct
    *cbs = (XmSelectionBoxCallbackStruct *)call_data;

  fclose(formfp);

  XtUnmanageChild(w);
  XtDestroyWidget(w);

  popped_up = 0;

  if(!formula_selection_error_dialog)
    {
    n = 0;
    str = XmStringCreateLocalized("Selection error");
    XtSetArg (args[n], XmNnoResize, False); n++;
    XtSetArg (args[n], XmNdefaultPosition, False); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;
    XtSetArg (args[n], XmNdialogTitle, str); n++;

    formula_selection_error_dialog = XmCreateWarningDialog
      (main_window, "formula_selection_error_dialog", args, n);

    XtAddCallback(formula_selection_error_dialog,
      XmNokCallback, response, NULL);

    XtAddCallback(formula_selection_error_dialog,
      XmNcancelCallback, response, NULL);

    XtAddCallback(formula_selection_error_dialog,
      XmNmapCallback, map_dialog, NULL);

    XtAddCallback(XtParent(formula_selection_error_dialog),
      XmNpopupCallback, map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(formula_selection_error_dialog),
      (EventMask) 0, True, _XEditResCheckMessages, NULL);
#endif

    XtUnmanageChild(XmMessageBoxGetChild
      (formula_selection_error_dialog, XmDIALOG_CANCEL_BUTTON));

    XmStringFree(str);
    }

  msg[1] = XmStringCreateLocalized("Formula set ");
  msg[3] = XmStringCreateLocalized(" does not exist.");
  msg[2] = XmStringConcat(msg[1], cbs->value);
  msg[0] = XmStringConcat(msg[2], msg[3]);

  XtVaSetValues(formula_selection_error_dialog,
    XmNmessageString, msg[0],
    NULL);

  XmStringFree(msg[0]);
  XmStringFree(msg[1]);
  XmStringFree(msg[2]);
  XmStringFree(msg[3]);

  XtManageChild(formula_selection_error_dialog);
  XtPopup(XtParent(formula_selection_error_dialog), XtGrabNone);
}

/*ARGSUSED*/
static void formula_selection_ok_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int ret;
  char *entryname;
  char msg[128];

  XmSelectionBoxCallbackStruct
    *cbs = (XmSelectionBoxCallbackStruct *)call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &entryname);

  fclose(formfp);

  XtUnmanageChild(w);
  XtDestroyWidget(w);

  popped_up = 0;

  if(((int)strcmp((char *) &FormName, entryname)) != 0)
    {
    sprintf((char *) &oldFormName, "%s", entryname);
    calc_status = IMG_PRMS_CHGD;
    toggle_zoom_grab(ZOOM_OFF);
    update_draw_menu(NEW_IMAGE);
    }

  sprintf((char *) &FormName, "%s", entryname);

  ret = RunForm(entryname);

  if(ret != 0)
    {
    gotfrmfile = 0;
    gotfrmname = 0;
    sprintf((char *) &msg, "Error loading entry \"%s\".", entryname);
    caution(main_window, (char *) &msg, NULL);
    }
  else
    {
    gotfrmfile = 1;
    gotfrmname = 1;
    type_specific_params(main_window);
    }
}

/*ARGSUSED*/
static void select_formula_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  fclose(formfp);
  popped_up = 0;
  XtPopdown(XtParent(formula_file_fsb));
}

/*ARGSUSED*/
static void formula_selection_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  fclose(formfp);
  popped_up = 0;
  XtUnmanageChild(w);
  XtDestroyWidget(w);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: formula_params.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.15  1995/05/03  21:29:57  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.14  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.13  1995/04/27  17:53:47  darryl
 * Change basename to nbasename, change %x to %d in client_data
 * translations, delete unused header file inclusions.
 *
 * Revision 1.12  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.11  1995/04/18  16:11:29  darryl
 * Fix problems and incorporate enhancements from beta test.
 * -- not able to zoom after loading a saved image.
 * -- string parsing would fail because the tags were incorrect.
 * -- CLK_TCK is determined from sysconf() if it is available.
 * -- Julia sets were not calculated correctly after image save.
 * -- some void pointers were not cast before promotion.
 * -- Info About Image display says it was working on pass 2 of 1.
 * -- an incomplete image could not be saved.
 * -- a saved image was not drawn if the refresh rate was < 100.
 * -- added Refresh Rate menu item and command line support.
 * -- added Parameter Forwarding menu item and command line support.
 * -- added fallback resources.
 * -- enhanced parameter handling.
 * -- implemented the Fractint zoombox 'panning' capability.
 * -- enhanced the LogMap and LogTable handling.
 * -- various typograhic error fixes.
 *
 * Revision 1.10  1995/03/31  00:38:13  darryl
 * Sync branch with top node.
 *
 * Revision 1.9.1.2  1995/03/31  00:26:55  darryl
 * Fix bugs in string parsing and XmString allocation.
 * Ensure that only one selection dialog of a given type
 * is available at any one time.
 *
 * Revision 1.9.1.1  1995/03/30  10:53:02  tom
 * changed str index
 *
 * Revision 1.9  1995/03/30  09:32:51  darryl
 * Revise the string parsing for selection dialog work areas.
 *
 * Revision 1.8  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.7  1995/03/27  01:53:25  darryl
 * Display previously selected name in list selection
 * dialogs, if appropriate.
 *
 * Revision 1.6  1995/03/26  22:28:18  darryl
 * Add buttons to the type selection dialogs for returning
 * to a file or name selection dialog if the type is
 * formula, lsystem, or ifs. Update the help dialog so that
 * only one may be displayed at a time. Display a clock
 * cursor while an image is being generated.
 *
 * Revision 1.5  1995/03/25  08:42:49  darryl
 * Fixes and enhancements -- make the parameter selection
 * understand that if a parameter file is loaded, then
 * the ifs, lsys, or formula file does not need to be
 * re-selected. Display the current fractal type and
 * any associated file and file entries at the top of
 * the parameters screen. Delete some unused code.
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
