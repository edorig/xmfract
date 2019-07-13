
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: cmd_entry.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void command_selection_err_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void command_selection_ok_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void command_selection_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void command_selection_help_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/*ARGSUSED*/
void select_cmd_entry(Widget w, char *filename, int write)
{
  int numentries;
  long file_offset;
  long name_offset;
  int i, n;
  int ret;
  struct stat st;
  char buf[101];
  char openflag[3];
  static char writeflag[3];
  Arg args[MAX_ARGS];
  XmString list[MAXENTRIES];
  XmString str[2];
  Widget command_selection_dialog;
  char msg[128];

  gotfrmfile = 0;
  gotfrmname = 0;
  gotifsfile = 0;
  gotifsname = 0;
  gotlsysfile = 0;
  gotlsysname = 0;

  if(write)
    {
    ret = stat(filename, &st);

    if(ret == 0)
      {
      if(verbose)
        {
        sprintf((char *) &msg, "Append \"%s\"?", filename);
        ret = confirm(main_window, (char *) &msg,
          OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
        }
      else
        ret = 1;

      if(ret == 1)
        sprintf((char *) &openflag, "%s", "ab");
      }
    else
      {
      if(verbose)
        {
        sprintf((char *) &msg, "Create \"%s\"?", filename);
        ret = confirm(main_window, (char *) &msg,
          OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
        }
      else
        ret = 1;

      if(ret == 1)
        sprintf((char *) &openflag, "%s", "wb");
      }

    if(ret == 1)
      {
      if((fp = fopen(filename, (char *) &openflag)) == (FILE *) NULL)
        {
        sprintf((char *) &msg, "Can't open command file %s.", filename);
        caution(main_window, (char *) &msg, NULL);
        return;
        }
      if((int)strcmp(openflag, "ab") == 0)
        {
        write = 2;
        fclose(fp);
        sprintf((char *) &openflag, "%s", "rb");
        fp = fopen(filename, (char *) &openflag);
        }
      }
    else
      {
      return;
      }
    }
  else
    {
    sprintf((char *) &openflag, "%s", "rb");
    if((fp = fopen(filename, (char *) &openflag)) == (FILE *) NULL)
      {
      sprintf((char *) &msg, "Can't open command file %s.", filename);
      caution(main_window, (char *) &msg, NULL);
      return;
      }
    }

  /* paranoia, this should have already been done */
  sprintf((char *) &PAR_name, filename);

  numentries = 0;
  file_offset = -1;

  for(i = 0; i < numentries; i++)
    {
    list[i] = XmStringCreateLocalized("");
    }

  /* scan the file for entry names */
  while(1)
    {
    int c,len;
    do
      {
      ++file_offset;
      c = getc(fp);
      }
    while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
    if (c == ';')
      {
      do
        {
        ++file_offset;
        c = getc(fp);
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
       c = getc(fp);
       ++file_offset;
      }
    buf[len] = 0;
    while (c != '{' && c != '\n' && c != '\r' && c != EOF && c != '\032')
      {
      c = getc(fp);
      ++file_offset;
      }
    if (c == '{')
      {
      while (c != '}' && c != EOF && c != '\032')
        {
        c = getc(fp);
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

  if(!write)
    {
    if (numentries == 0)
      {
      sprintf((char *) &msg,
        "File \"%s\" doesn't contain any valid entries", filename);
      inform(main_window, (char *) &msg, NULL);
      fclose(fp);
      return;
      }
    }

  qsort((char **)choices,numentries,sizeof(char *),xmcompare);

  if(numentries > 0)
    {
    for(i = 0; i < numentries; i++)
      list[i] = XmStringCreateLocalized(choices[i]->name);
    }

  n = 0;
  str[0] = XmStringCreateLocalized("Par File Entries");
  str[1] = XmStringCreateLocalized("Select Command Set");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNdialogType, XmDIALOG_SELECTION); n++;
  XtSetArg(args[n], XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;
  if(numentries > 0)
    {
    if(!write)
      {
      XtSetArg(args[n], XmNtextString, list[0]); n++;
      }
    XtSetArg(args[n], XmNlistItems, list); n++;
    XtSetArg(args[n], XmNlistVisibleItemCount, 10); n++;
    }
  XtSetArg(args[n], XmNlistItemCount, numentries); n++;
  XtSetArg(args[n], XmNmustMatch, True); n++;
  XtSetArg(args[n], XmNtextColumns, 40); n++;
  XtSetArg(args[n], XmNlistLabelString, str[0]); n++;
  XtSetArg(args[n], XmNdialogTitle, str[1]); n++;

  command_selection_dialog = XmCreateSelectionDialog
    (main_window, "command_selection_dialog", args, n);

  XtUnmanageChild (XmSelectionBoxGetChild
    (command_selection_dialog, XmDIALOG_APPLY_BUTTON));

  switch(write)
    {
    case 1:
      sprintf((char *) &writeflag, "1");
    break;

    case 2:
      sprintf((char *) &writeflag, "2");
    break;

    case 0:
    default:
      sprintf((char *) &writeflag, "0");
    break;
    }

  XtAddCallback(command_selection_dialog, XmNapplyCallback,
    command_selection_ok_cb, (char *) &writeflag);

  XtAddCallback(command_selection_dialog, XmNokCallback,
    command_selection_ok_cb, (char *) &writeflag);

  XtAddCallback(command_selection_dialog, XmNnoMatchCallback,
      command_selection_err_cb, (char *) &writeflag);

  XtAddCallback(command_selection_dialog, XmNcancelCallback,
    command_selection_cancel_cb, (char *) &writeflag);

  XtAddCallback(command_selection_dialog, XmNhelpCallback,
    command_selection_help_cb, (char *) &writeflag);

  XtAddCallback(command_selection_dialog, XmNmapCallback, map_dialog, NULL);

  XtAddCallback(XtParent(command_selection_dialog), XmNpopupCallback,
    map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(XtParent(command_selection_dialog), (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  for(i = 0; i < numentries; i++)
    XmStringFree(list[i]);

  XmStringFree(str[0]);
  XmStringFree(str[1]);

  XtManageChild(command_selection_dialog);
  XtPopup(XtParent(command_selection_dialog), XtGrabNone);
}

/*ARGSUSED*/
static void command_selection_err_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  static Widget selection_error_dialog = NULL;
  Arg args[MAX_ARGS];
  XmString msg[4];
  XmString str;
  char *ename;
  int n;
  int len;

  int ret = 0;

  int write = atoi((char *) client_data);

  XmSelectionBoxCallbackStruct
    *cbs = (XmSelectionBoxCallbackStruct *)call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &ename);

  XtUnmanageChild(w);
  XtDestroyWidget(w);

  if(ename)
    len = (int)(strlen(ename));
  else
    len = 0;

  if(!write || len <= 0)
    {
    if(!selection_error_dialog)
      {
      n = 0;
      str = XmStringCreateLocalized("Selection error");
      XtSetArg (args[n], XmNnoResize, False); n++;
      XtSetArg (args[n], XmNdefaultPosition, False); n++;
      XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
      XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;
      XtSetArg (args[n], XmNdialogTitle, str); n++;

      selection_error_dialog = XmCreateWarningDialog
        (main_window, "selection_error_dialog", args, n);

      XtAddCallback(selection_error_dialog, XmNokCallback, response, NULL);
      XtAddCallback(selection_error_dialog, XmNcancelCallback, response, NULL);
      XtAddCallback(selection_error_dialog, XmNmapCallback, map_dialog, NULL);
      XtAddCallback(XtParent(selection_error_dialog),
        XmNpopupCallback, map_dialog, NULL);

#ifdef EDITRES_ENABLE
      XtAddEventHandler(XtParent(selection_error_dialog), (EventMask) 0,
        True, _XEditResCheckMessages, NULL);
#endif

      XtUnmanageChild (XmMessageBoxGetChild
        (selection_error_dialog, XmDIALOG_CANCEL_BUTTON));

      XmStringFree(str);
      }

    msg[1] = XmStringCreateLocalized("Command set ");
    msg[3] = XmStringCreateLocalized(" does not exist.");
    if(len)
      msg[2] = XmStringConcat(msg[1], cbs->value);
    else
      msg[2] = XmStringCreateLocalized("Command set <null>");
    msg[0] = XmStringConcat(msg[2], msg[3]);

    XtVaSetValues(selection_error_dialog,
      XmNmessageString, msg[0],
      NULL);

    XmStringFree(msg[0]);
    XmStringFree(msg[1]);
    XmStringFree(msg[2]);
    XmStringFree(msg[3]);

    XtManageChild (selection_error_dialog);
    }
  else
    {
    if(verbose)
      {
      char text[256];
      sprintf((char *) &text, "Create command set \"%s\"?", ename);
      ret = confirm(main_window, (char *) &text,
        OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
      }
    else
      ret = 1;

    if(ret == 1)
      {
      rewind(fp);
      fseek(fp,0L,SEEK_END);
      fclose(fp);
      saveparms((char *) &PAR_name, ename, write);
      }
    }
}

/*ARGSUSED*/
static void command_selection_ok_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i, ret, write;
  char *entryname;
  struct stat st;
  char msg[128];
  char tempstr[256];
  long entry_point = 0;

  XmSelectionBoxCallbackStruct
    *cbs = (XmSelectionBoxCallbackStruct *)call_data;

  write = atoi((char *) client_data);

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &entryname);

  for(i = 0; i < MAXENTRIES; i++)
    {
    if((int) strcmp(choices[i]->name, entryname) == 0)
      entry_point = choices[i]->point;
    }

  XtUnmanageChild(w);
  XtDestroyWidget(w);

  if(write)
    {
    sprintf((char *) &msg, "Cannot replace existing \"%s\".", entryname);
    inform(main_window, (char *) &msg, NULL);
    rewind(fp);
    fseek(fp,0L,SEEK_END);
    fclose(fp);
    }
  else
    {
    initvars_fractal();
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    toggle_zoom_grab(ZOOM_OFF);
    rewind(fp);
    fseek(fp,entry_point,SEEK_SET);
    ret = load_commands(fp);

    memcpy(olddacbox,dacbox,768);

    if((ret) < 0)
      {
      sprintf((char *) &msg, "Error loading entry \"%s\".", entryname);
      caution(main_window, (char *) &msg, NULL);
      rewind(fp);
      fseek(fp,0L,SEEK_END);
      fclose(fp);
      gotfrmfile = 0;
      gotfrmname = 0;
      gotifsfile = 0;
      gotifsname = 0;
      gotlsysfile = 0;
      gotlsysname = 0;
      functionpreloaded = 0;
      return;
      }
    else
      curtype = oldtype = fractype;

    if(colorpreloaded)
      {
      memcpy(dacbox,olddacbox,768);
      spindac(0,0);
      colorpreloaded = 0;
      }

    if((ret & 4) == 4)
      {
      if(gotfile)
        {
        if(fractdir && *fractdir)
          sprintf((char *) &tempstr, "%s/%s", fractdir, readname);
        else
          sprintf((char *) &tempstr, "%s", readname);
        if((ret = stat((char *) &tempstr, &st)) == 0)
          {
          sprintf(readname, (char *) &tempstr);
          display3d = 1;
          outln = line3d;
          read_overlay();
          }
        else
          load_transform_cb(main_window, NULL, NULL);
        }
      else
        load_transform_cb(main_window, NULL, NULL);
      }
    }
}

/*ARGSUSED*/
static void command_selection_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int write = atoi((char *) client_data);
  rewind(fp);
  fseek(fp,0L,SEEK_END);
  fclose(fp);

  if(write == 1)
    unlink((char *) &PAR_name);

  XtUnmanageChild(XtParent(w));
  XtDestroyWidget(XtParent(w));
}

/*ARGSUSED*/
static void command_selection_help_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPPARMFILE;
  read_help(main_window, NULL, NULL);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: cmd_entry.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.10  1995/06/02  06:09:35  darryl
 * Change default button in some dialogs from Cancel to Ok.
 *
 * Revision 1.9  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.8  1995/04/18  15:59:59  darryl
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
 * Revision 1.7  1995/03/31  18:47:10  darryl
 * Update parameter file processing so that 3D parameters
 * are handled in the same fashion as other entries.
 *
 * Revision 1.6  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
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
 * Revision 0.104  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.103  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
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
