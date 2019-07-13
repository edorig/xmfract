
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: help.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void read_help_dialog_done_button_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void helps_menu_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void help_topic_index_dialog_ok_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void help_topic_index_dialog_err_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void help_topic_index_dialog_help_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void help_topic_index_dialog_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void form_topic_index_dialog_ok_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void form_topic_index_dialog_err_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void form_topic_index_dialog_help_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void form_topic_index_dialog_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void read_help_index(void);
static void formula_help(void);

/*ARGSUSED*/
void create_helps_menu(Widget w)
{
  int n;
  Arg args[MAX_ARGS];
  Widget helps_menu_button;
  Widget helps_menu_toc;
  Widget helps_menu_pane;
  Widget helps_menu_btn;
  XmString str;
  XmString accel;

  /* create a menu pane as a child of the menu bar */

  n = 0;
  helps_menu_pane = XmCreatePulldownMenu (w, "helps_menu_pane", args, n);

  /* get the tear-off control */

  if((helps_menu_toc = XmGetTearOffControl
    (helps_menu_pane)) != NULL)
    {
    XtVaSetValues(helps_menu_toc, XmNseparatorType,
      XmSINGLE_DASHED_LINE, NULL);
    }

  /* create 'Help' cascade button */

  n = 0;
  str = XmStringCreateLocalized("Help");
  XtSetArg(args[n], XmNsubMenuId, helps_menu_pane); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  helps_menu_button = XmCreateCascadeButton
    (w, "helps_menu_button", args, n);

  XtManageChild(helps_menu_button);
  XmStringFree(str);

  /*
   * create pushbuttons and/or cascade pushbuttons for the 'Help' menu
   *
   * About...
   * Authors...
   * Help Index
   * Chaos Theory
   * GNU Public License
   * Version information
   */

  n = 0;
  str = XmStringCreateLocalized("About...");
  XtSetArg(args[n], XmNlabelString, str); n++;

  helps_menu_btn = XmCreatePushButton
    (helps_menu_pane, "help_about_btn", args, n);

  XtAddCallback(helps_menu_btn, XmNactivateCallback,
    helps_menu_btn_cb, "0");

  XtManageChild(helps_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Authors...");
  accel = XmStringCreateLocalized("[ w ]");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNacceleratorText, accel); n++;
  XtSetArg(args[n], XmNaccelerator, "<Key>w:"); n++;

  helps_menu_btn = XmCreatePushButton
    (helps_menu_pane, "helps_authors_btn", args, n);

  XtAddCallback(helps_menu_btn, XmNactivateCallback,
    display_authors_window_cb, NULL);

  XtManageChild(helps_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Help Index");
  XtSetArg(args[n], XmNlabelString, str); n++;

  helps_menu_btn = XmCreatePushButton
    (helps_menu_pane, "helps_index_btn", args, n);

  XtAddCallback(helps_menu_btn, XmNactivateCallback,
    helps_menu_btn_cb, "1");

  XtManageChild(helps_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Chaos Theory");
  XtSetArg(args[n], XmNlabelString, str); n++;

  helps_menu_btn = XmCreatePushButton
    (helps_menu_pane, "helps_chaos_btn", args, n);

  XtAddCallback(helps_menu_btn, XmNactivateCallback,
    helps_menu_btn_cb, "2");

  XtManageChild(helps_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("GNU Public License");
  XtSetArg(args[n], XmNlabelString, str); n++;

  helps_menu_btn = XmCreatePushButton
    (helps_menu_pane, "helps_gnu_btn", args, n);

  XtAddCallback(helps_menu_btn, XmNactivateCallback,
    helps_menu_btn_cb, "3");

  XtManageChild(helps_menu_btn);
  XmStringFree(str);

  n = 0;
  str = XmStringCreateLocalized("Version information");
  XtSetArg(args[n], XmNlabelString, str); n++;

  helps_menu_btn = XmCreatePushButton
    (helps_menu_pane, "helps_version_btn", args, n);

  XtAddCallback(helps_menu_btn, XmNactivateCallback,
    display_disclaimer, NULL);

  XtManageChild(helps_menu_btn);
  XmStringFree(str);
}

/*ARGSUSED*/
void read_help(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Widget text_window;
  Widget help_window;
  Widget done_button;
  Widget read_help_dialog;
  Widget read_help_dialog_shell;
  XmString str;
  Arg args[MAX_ARGS];
  struct stat st;
  FILE *helpfp;
  char msg[128];
  char fname[256];
  char *help_text;
  int ret;
  int n;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  sprintf((char *) &fname, "%s/help.%d", helpdir, helpmode);

  ret = 0;

  if(((ret = stat(helpdir, &st)) == -1) ||
     ((st.st_mode & S_IFMT) != S_IFDIR))
    {
    sprintf((char *) &msg,
      "Help directory \"%s\" does not exist.", helpdir);
    caution(main_window, (char *) &msg, NULL);
    XDefineCursor(display, window,
      XCreateFontCursor(display, XC_top_left_arrow));
    return;
    }

  if(((ret = stat(fname, &st)) == -1) ||
     ((st.st_mode & S_IFMT) != S_IFREG) ||
     (!(helpfp = fopen(fname, "r"))))
    {
    if(ret == -1)
      {
      sprintf((char *) &msg, "Help file \"%s\" does not exist.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else if((st.st_mode & S_IFMT) == S_IFREG)
      {
      sprintf((char *) &msg, "Cannot open help file \"%s\".", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else
      {
      sprintf((char *) &msg, "\"%s\" is not a text file.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    }

  if(!(help_text = XtMalloc((unsigned) (st.st_size + 1))))
    {
    sprintf((char *) &msg, "Out of memory! Cannot display help.");
    warning(main_window, (char *) &msg, NULL);
    fclose(helpfp);
    XDefineCursor(display, window,
      XCreateFontCursor(display, XC_top_left_arrow));
    return;
    }

  if(!(fread(help_text, sizeof(char), st.st_size + 1, helpfp)))
    {
    sprintf((char *) &msg, "May not have read entire help file!.");
    warning(main_window, (char *) &msg, NULL);
    }

  help_text[st.st_size] = 0;

  n = 0;
  XtSetArg(args[n], XmNtraversalOn, True); n++;
  XtSetArg(args[n], XmNkeyboardFocusPolicy, XmEXPLICIT); n++;
  read_help_dialog_shell = XmCreateDialogShell
    (main_window, "read_help_dialog_shell", args, n);

  XtAddCallback(read_help_dialog_shell, XmNpopupCallback,
    map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(read_help_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  str = XmStringCreateLocalized("Help");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNtraversalOn, True); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogTitle, str); n++;

  read_help_dialog = XmCreateForm
    (read_help_dialog_shell, "read_help_dialog", args, n);

  XtAddCallback(read_help_dialog, XmNunmapCallback,
    read_help_dialog_done_button_cb, NULL);

  XmStringFree(str);

  n = 0;
  XtSetArg(args[n], XmNtraversalOn, True); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  help_window = XmCreateScrolledWindow
    (read_help_dialog, "help_window", args, n);

  n = 0;
  XtSetArg(args[n], XmNeditable, False); n++;
  XtSetArg(args[n], XmNtraversalOn, True); n++;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  XtSetArg(args[n], XmNcursorPositionVisible, True); n++;
  XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
  text_window = XmCreateText
    (help_window, "text_window", args, n);

  XmTextSetString(text_window, help_text);

  XtManageChild(text_window);
  XtManageChild(help_window);

  n = 0;
  str = XmStringCreateLocalized("Done");
  XtSetArg(args[n], XmNtopWidget, help_window); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;

  done_button = XmCreatePushButton
    (read_help_dialog, "read_help_dialog_done_button", args, n);

  XtManageChild(done_button);

  XtAddCallback(done_button, XmNactivateCallback,
    read_help_dialog_done_button_cb, NULL);

  XmStringFree(str);

  XtVaSetValues(read_help_dialog,
    XmNinitialFocus,
    help_window,
    XmNdefaultButton,
    done_button,
    NULL);

  XtManageChild(read_help_dialog);

  XtFree(help_text);
  fclose(helpfp);

  XtManageChild(read_help_dialog_shell);
  XtPopup(read_help_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  XmTextSetCursorPosition(text_window, 0);
}

/*ARGSUSED*/
static void helps_menu_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int which = atoi((char *) client_data);
  switch(which)
    {
    case 0:
      helpmode = HELPXMFRACT;
    break;

    case 1:
      helpmode = HELP_INDEX;
    break;

    case 2:
      helpmode = HELPCHAOS;
    break;

    case 3:
      helpmode = HELPGNU;
    break;

    default:
      helpmode = HELPXMFRACT;
    break;
    }

  if(helpmode == HELP_INDEX)
    read_help_index();
  else
    read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void read_help_index()
{
  char ch;
  int ret;
  int n, i, j, k;
  int numentries;
  int max_linelen;
  int skip_start;
  int skip_end;
  struct stat st;
  FILE *helpidxfp;
  char msg[128];
  char fname[256];
  char line[81];
  XmString str[2];
  Arg args[MAX_ARGS];
  char *clist[HELP_TOPIC_TOTAL];
  XmString list[HELP_TOPIC_TOTAL];
  Widget help_topic_index_dialog;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  numentries = HELP_TOPIC_TOTAL;
  skip_start = HELP_FORMULA_START;
  skip_end = HELP_FORMULA_END;

  sprintf((char *) &fname, "%s/help.%d", helpdir, helpmode);

  ret = 0;

  if(((ret = stat(fname, &st)) == -1) ||
     ((st.st_mode & S_IFMT) != S_IFREG) ||
     (!(helpidxfp = fopen(fname, "r"))))
    {
    if(ret == -1)
      {
      sprintf((char *) &msg, "Help file \"%s\" does not exist.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else if((st.st_mode & S_IFMT) == S_IFREG)
      {
      sprintf((char *) &msg, "Cannot open help file \"%s\".", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else
      {
      sprintf((char *) &msg, "\"%s\" is not a regular file.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    }

  k = -1;
  max_linelen = 0;

  for(i = 0; i < numentries; i++)
    {
    /* attempt to read past eof? */
    if(helpidxfp == (FILE *)NULL || feof(helpidxfp) || ferror(helpidxfp))
      {
      sprintf((char *) &msg, "Error - attempted read beyond EOF!");
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }

    j = -1;

    while((ch = getc(helpidxfp)) != EOF && !END_OF_LINE(ch))
      {
      if(++j > 81)
        {
        sprintf((char *) &msg, "Error - attempted read beyond EOF!");
        caution(main_window, (char *) &msg, NULL);
        XDefineCursor(display, window,
          XCreateFontCursor(display, XC_top_left_arrow));
        return;
        }
      line[j] = ch;
      }

    line[++j] = '\0';

    /* skip the 'specific formula' entries */
    if(i >= skip_start - 1 && i <= skip_end)
      continue;
    else
      ++k;

    if(max_linelen < j) max_linelen = j;

    if((clist[k] =
      (char *) XtMalloc((sizeof(char) * (j + 1)))) == (char *) NULL)
      {
      sprintf((char *) &msg, "Sorry - out of memory in read help index.");
        fatal(main_window, (char *) &msg);
        XDefineCursor(display, window,
          XCreateFontCursor(display, XC_top_left_arrow));
        return;
      }
    sprintf(clist[k], (char *) &line);
    }

  max_linelen += 5;

  fclose(helpidxfp);

  qsort((char **) clist, k, sizeof(char *), xmcompare);

  for(i = 0; i < k; i++)
    list[i] = XmStringCreateLocalized(clist[i]);

  n = 0;
  str[0] = XmStringCreateLocalized("Help Topic Index");
  str[1] = XmStringCreateLocalized("Select Help Topic");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogType, XmDIALOG_SELECTION); n++;
  XtSetArg(args[n], XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  if(numentries > 0)
    {
    XtSetArg(args[n], XmNtextString, list[1]); n++;
    XtSetArg(args[n], XmNlistItems, list); n++;
    XtSetArg(args[n], XmNlistVisibleItemCount, 20); n++;
    }
  XtSetArg(args[n], XmNlistItemCount, k); n++;
  XtSetArg(args[n], XmNmustMatch, True); n++;
  XtSetArg(args[n], XmNtextColumns, max_linelen); n++;
  XtSetArg(args[n], XmNlistLabelString, str[0]); n++;
  XtSetArg(args[n], XmNdialogTitle, str[1]); n++;

  help_topic_index_dialog = XmCreateSelectionDialog
    (main_window, "help_topic_index_dialog", args, n);

  XtUnmanageChild (XmSelectionBoxGetChild
    (help_topic_index_dialog, XmDIALOG_APPLY_BUTTON));

  XtAddCallback(help_topic_index_dialog, XmNapplyCallback,
    help_topic_index_dialog_ok_cb, NULL);

  XtAddCallback(help_topic_index_dialog, XmNokCallback,
    help_topic_index_dialog_ok_cb, NULL);

  XtAddCallback(help_topic_index_dialog, XmNnoMatchCallback,
    help_topic_index_dialog_err_cb, NULL);

  XtAddCallback(help_topic_index_dialog, XmNhelpCallback,
    help_topic_index_dialog_help_cb, NULL);

  XtAddCallback(help_topic_index_dialog, XmNcancelCallback,
    help_topic_index_dialog_cancel_cb, NULL);

  XtAddCallback(help_topic_index_dialog, XmNmapCallback, map_dialog, NULL);

  XtAddCallback(XtParent(help_topic_index_dialog), XmNpopupCallback,
    map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(XtParent(help_topic_index_dialog), (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  for(i = 0; i < k; i++)
    {
    XmStringFree(list[i]);
    XtFree(clist[i]);
    }

  XmStringFree(str[0]);
  XmStringFree(str[1]);

  XtManageChild(help_topic_index_dialog);
  XtPopup(XtParent(help_topic_index_dialog), XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void help_topic_index_dialog_ok_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char ch;
  int ret;
  int i, j;
  int numentries;
  struct stat st;
  FILE *helpidxfp;
  char msg[128];
  char fname[256];
  char line[81];
  XmString str;

  XmSelectionBoxCallbackStruct *cbs =
    (XmSelectionBoxCallbackStruct *)call_data;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  helpmode = HELP_INDEX;

  sprintf((char *) &fname, "%s/help.%d", helpdir, helpmode);

  helpmode = HELPMAIN;

  numentries = HELP_TOPIC_TOTAL;

  ret = 0;

  if(((ret = stat(fname, &st)) == -1) ||
     ((st.st_mode & S_IFMT) != S_IFREG) ||
     (!(helpidxfp = fopen(fname, "r"))))
    {
    if(ret == -1)
      {
      sprintf((char *) &msg, "Help file \"%s\" does not exist.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else if((st.st_mode & S_IFMT) == S_IFREG)
      {
      sprintf((char *) &msg, "Cannot open help file \"%s\".", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else
      {
      sprintf((char *) &msg, "\"%s\" is not a regular file.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    }

  for(i = 0; i < numentries; i++)
    {
    /* attempt to read past eof? */
    if(helpidxfp == (FILE *)NULL || feof(helpidxfp) || ferror(helpidxfp))
      {
      sprintf((char *) &msg, "Error - attempted read beyond EOF!");
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }

    j = -1;

    while((ch = getc(helpidxfp)) != EOF && !END_OF_LINE(ch))
      {
      if(++j > 81)
        {
        sprintf((char *) &msg, "Error - attempted read beyond EOF!");
        caution(main_window, (char *) &msg, NULL);
        XDefineCursor(display, window,
          XCreateFontCursor(display, XC_top_left_arrow));
        fclose(helpidxfp);
        return;
        }
      line[j] = ch;
      }

    line[++j] = '\0';

    str = XmStringCreateLocalized((char *) &line);

    if(XmStringCompare(str, cbs->value) == True)
      {
      helpmode = i + 1;
      XmStringFree(str);
      break;
      }
    XmStringFree(str);
    }

  fclose(helpidxfp);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  if(helpmode == HELPFRACTALS)
    formula_help();
  else
    read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void formula_help()
{
  char ch;
  int ret;
  int n, i, j, k;
  int numentries;
  int max_linelen;
  int skip_start;
  int skip_end;
  struct stat st;
  FILE *formidxfp;
  char msg[128];
  char fname[256];
  char line[81];
  XmString str[2];
  Arg args[MAX_ARGS];
  char *clist[HELP_TOPIC_TOTAL];
  XmString list[HELP_TOPIC_TOTAL];
  Widget form_topic_index_dialog;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  numentries = HELP_TOPIC_TOTAL;
  skip_start = HELP_FORMULA_START;
  skip_end = HELP_FORMULA_END;

  helpmode = HELP_INDEX;

  sprintf((char *) &fname, "%s/help.%d", helpdir, helpmode);

  ret = 0;

  if(((ret = stat(fname, &st)) == -1) ||
     ((st.st_mode & S_IFMT) != S_IFREG) ||
     (!(formidxfp = fopen(fname, "r"))))
    {
    if(ret == -1)
      {
      sprintf((char *) &msg, "Help file \"%s\" does not exist.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else if((st.st_mode & S_IFMT) == S_IFREG)
      {
      sprintf((char *) &msg, "Cannot open help file \"%s\".", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else
      {
      sprintf((char *) &msg, "\"%s\" is not a regular file.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    }

  k = -1;
  max_linelen = 0;

  for(i = 0; i < numentries; i++)
    {
    /* attempt to read past eof? */
    if(formidxfp == (FILE *)NULL || feof(formidxfp) || ferror(formidxfp))
      {
      sprintf((char *) &msg, "Error - attempted read beyond EOF!");
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }

    j = -1;

    while((ch = getc(formidxfp)) != EOF && !END_OF_LINE(ch))
      {
      if(++j > 81)
        {
        sprintf((char *) &msg, "Error - attempted read beyond EOF!");
        caution(main_window, (char *) &msg, NULL);
        XDefineCursor(display, window,
          XCreateFontCursor(display, XC_top_left_arrow));
        fclose(formidxfp);
        return;
        }
      line[j] = ch;
      }

    line[++j] = '\0';

    /* skip all but the 'specific formula' entries */
    if(i >= skip_start - 1 && i <= skip_end)
      ++k;
    else
      continue;

    if(max_linelen < j) max_linelen = j;

    if((clist[k] =
      (char *) XtMalloc((sizeof(char) * (j + 1)))) == (char *) NULL)
      {
      sprintf((char *) &msg, "Sorry - out of memory in read help index.");
        fatal(main_window, (char *) &msg);
        XDefineCursor(display, window,
          XCreateFontCursor(display, XC_top_left_arrow));
        return;
      }
    sprintf(clist[k], (char *) &line);
    }

  max_linelen += 5;

  fclose(formidxfp);

  qsort((char **) clist, k, sizeof(char *), xmcompare);

  for(i = 0; i < k; i++)
    list[i] = XmStringCreateLocalized(clist[i]);

  n = 0;
  str[0] = XmStringCreateLocalized("Select Specific Formula");
  str[1] = XmStringCreateLocalized("Fractal Formula Index");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogType, XmDIALOG_SELECTION); n++;
  XtSetArg(args[n], XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  if(numentries > 0)
    {
    XtSetArg(args[n], XmNtextString, list[0]); n++;
    XtSetArg(args[n], XmNlistItems, list); n++;
    XtSetArg(args[n], XmNlistVisibleItemCount, 20); n++;
    }
  XtSetArg(args[n], XmNlistItemCount, k); n++;
  XtSetArg(args[n], XmNmustMatch, True); n++;
  XtSetArg(args[n], XmNtextColumns, max_linelen); n++;
  XtSetArg(args[n], XmNlistLabelString, str[0]); n++;
  XtSetArg(args[n], XmNdialogTitle, str[1]); n++;

  form_topic_index_dialog = XmCreateSelectionDialog
    (main_window, "form_topic_index_dialog", args, n);

  XtUnmanageChild (XmSelectionBoxGetChild
    (form_topic_index_dialog, XmDIALOG_APPLY_BUTTON));

  XtAddCallback(form_topic_index_dialog, XmNapplyCallback,
    form_topic_index_dialog_ok_cb, NULL);

  XtAddCallback(form_topic_index_dialog, XmNokCallback,
    form_topic_index_dialog_ok_cb, NULL);

  XtAddCallback(form_topic_index_dialog, XmNnoMatchCallback,
    form_topic_index_dialog_err_cb, NULL);

  XtAddCallback(form_topic_index_dialog, XmNhelpCallback,
    form_topic_index_dialog_help_cb, NULL);

  XtAddCallback(form_topic_index_dialog, XmNcancelCallback,
    form_topic_index_dialog_cancel_cb, NULL);

  XtAddCallback(form_topic_index_dialog, XmNmapCallback, map_dialog, NULL);

  XtAddCallback(XtParent(form_topic_index_dialog), XmNpopupCallback,
    map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(XtParent(form_topic_index_dialog), (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  for(i = 0; i < k; i++)
    {
    XmStringFree(list[i]);
    XtFree(clist[i]);
    }

  XmStringFree(str[0]);
  XmStringFree(str[1]);

  XtManageChild(form_topic_index_dialog);
  XtPopup(XtParent(form_topic_index_dialog), XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void form_topic_index_dialog_ok_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char ch;
  int ret;
  int i, j;
  int numentries;
  struct stat st;
  FILE *formidxfp;
  char msg[128];
  char fname[256];
  char line[81];
  XmString str;

  XmSelectionBoxCallbackStruct *cbs =
    (XmSelectionBoxCallbackStruct *)call_data;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  helpmode = HELP_INDEX;

  sprintf((char *) &fname, "%s/help.%d", helpdir, helpmode);

  helpmode = HELPMAIN;

  numentries = HELP_TOPIC_TOTAL;

  ret = 0;

  if(((ret = stat(fname, &st)) == -1) ||
     ((st.st_mode & S_IFMT) != S_IFREG) ||
     (!(formidxfp = fopen(fname, "r"))))
    {
    if(ret == -1)
      {
      sprintf((char *) &msg, "Help file \"%s\" does not exist.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else if((st.st_mode & S_IFMT) == S_IFREG)
      {
      sprintf((char *) &msg, "Cannot open help file \"%s\".", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    else
      {
      sprintf((char *) &msg, "\"%s\" is not a regular file.", fname);
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }
    }

  for(i = 0; i < numentries; i++)
    {
    /* attempt to read past eof? */
    if(formidxfp == (FILE *)NULL || feof(formidxfp) || ferror(formidxfp))
      {
      sprintf((char *) &msg, "Error - attempted read beyond EOF!");
      caution(main_window, (char *) &msg, NULL);
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      return;
      }

    j = -1;

    while((ch = getc(formidxfp)) != EOF && !END_OF_LINE(ch))
      {
      if(++j > 81)
        {
        sprintf((char *) &msg, "Error - attempted read beyond EOF!");
        caution(main_window, (char *) &msg, NULL);
        XDefineCursor(display, window,
          XCreateFontCursor(display, XC_top_left_arrow));
        fclose(formidxfp);
        return;
        }
      line[j] = ch;
      }

    line[++j] = '\0';

    str = XmStringCreateLocalized((char *) &line);

    if(XmStringCompare(str, cbs->value) == True)
      {
      helpmode = i + 1;
      XmStringFree(str);
      break;
      }
    XmStringFree(str);
    }

  fclose(formidxfp);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));

  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void form_topic_index_dialog_err_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char msg[28];
  sprintf((char *) &msg, "Selected formula does not exist.");
  caution(main_window, (char *) &msg, NULL);
  return;
}

/*ARGSUSED*/
static void form_topic_index_dialog_help_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPMAIN;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void form_topic_index_dialog_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(XtParent(w));
  XtDestroyWidget(XtParent(w));
}

/*ARGSUSED*/
static void help_topic_index_dialog_err_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char msg[28];
  sprintf((char *) &msg, "Selected topic does not exist.");
  caution(main_window, (char *) &msg, NULL);
  return;
}

/*ARGSUSED*/
static void help_topic_index_dialog_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(XtParent(w));
  XtDestroyWidget(XtParent(w));
}

/*ARGSUSED*/
static void help_topic_index_dialog_help_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPMAIN;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void read_help_dialog_done_button_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(XtParent(w));
  XtDestroyWidget(XtParent(w));
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: help.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.13  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.12  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
 *
 * Revision 1.11  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.10  1995/04/11  15:37:45  darryl
 * Remove hard-coded tearOffModel resources to allow the
 * resource file to over-ride the fallback resource.
 *
 * Revision 1.9  1995/03/28  22:47:41  darryl
 * Fix the "Defaults" function in the type parameters dialog.
 * Add a "Reset" function to type parameters dialog.
 * Fix the help dialogs so that any number of dialogs may
 * be opened simultaneously.
 *
 * Revision 1.8  1995/03/26  22:28:18  darryl
 * Add buttons to the type selection dialogs for returning
 * to a file or name selection dialog if the type is
 * formula, lsystem, or ifs. Update the help dialog so that
 * only one may be displayed at a time. Display a clock
 * cursor while an image is being generated.
 *
 * Revision 1.7  1995/03/23  23:47:42  darryl
 * Move the rows and columns resources for the Text widget
 * to the resource file.
 *
 * Revision 1.6  1995/03/23  22:16:06  darryl
 * Create the ScrolledWindow and Text widgets separately
 * so that the keyboard traversal will work correctly,
 * enable PageUp, PageDown, and cursor keys.
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
 * Revision 1.2  1995/03/10  19:32:17  darryl
 * Generalize the help dialog, using the Form widget as a manager.
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.302  1995/01/31  18:14:53  darryl
 * Extend the edge of the 'Done' button to the edge of
 * the scroll bar in the help dialogs.
 *
 * Revision 0.301  1995/01/18  22:28:05  darryl
 * Expand error messages from the command-line parser.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.201  1994/12/05  20:46:40  darryl
 * Make the help dialogs full application modal again to
 * prevent crashes when multiple dialogs are displayed.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.115  1994/12/02  02:51:01  darryl
 * Make the help dialogs non-modal.
 *
 * Revision 0.114  1994/11/30  21:09:19  darryl
 * Set the XmNseparatorType resource for TearOffControls.
 *
 * Revision 0.113  1994/11/30  20:20:55  darryl
 * Add support for tear-off menus.
 *
 * Revision 0.112  1994/11/29  23:42:30  darryl
 * Make the main menu bar help file the default listing in
 * the index selection dialog.
 *
 * Revision 0.111  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.110  1994/11/28  18:16:19  darryl
 * Increase the life span of the index and formula dialogs.
 *
 * Revision 0.109  1994/11/28  17:08:18  darryl
 * Reorder help menu items.
 *
 * Revision 0.108  1994/11/27  00:05:39  darryl
 * Rename help menu items.
 *
 * Revision 0.107  1994/11/26  21:27:00  darryl
 * Expand help, adding specific formulas to a separate
 * dialog called from the main help index.
 *
 * Revision 0.106  1994/11/26  02:41:11  darryl
 * Minor widget alignment changes.
 *
 * Revision 0.105  1994/11/26  02:10:28  darryl
 * Add help index and help topic browser.
 *
 * Revision 0.104  1994/11/25  20:31:03  darryl
 * Expand main help menu.
 *
 * Revision 0.103  1994/11/25  17:11:55  darryl
 * Delete unused variables.
 *
 * Revision 0.102  1994/11/24  07:27:15  darryl
 * Initialize helpmode.
 *
 * Revision 0.101  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.100  1994/11/23  05:22:53  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
