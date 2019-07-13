
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: julibrot_param.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void orbit_type_ok_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void orbit_param_done_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void orbit_param_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void select_julibrot_3d_parms(Widget w);
static void orbit_3d_param_done_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void orbit_3d_param_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget orbit_specific_param_dialog_shell = NULL;
static Widget julibrot_3d_parm_dialog_shell = NULL;
static Widget orbit_param[4];
static Widget orbit_mxmaxfp;
static Widget orbit_mymaxfp;
static Widget orbit_mxminfp;
static Widget orbit_myminfp;
static Widget orbit_zdots;
static Widget orbit_juli3Dmode;
static Widget orbit_eyesfp;
static Widget orbit_originfp;
static Widget orbit_depthfp;
static Widget orbit_heightfp;
static Widget orbit_widthfp;
static Widget orbit_distfp;
static int firstparm = 0;
static int orbit_index[MAXJBPARAMS];
static struct fractalspecificstuff *jborbit;

/*ARGSUSED*/
void select_julibrot_params(Widget w)
{
  int i, j, n;
  Arg args[MAX_ARGS];
  XmString list[MAX_TYPES];
  XmString str[2];
  Widget orbit_selection_dialog;
  struct choice *choices[MAX_TYPES];
  char msg[128];

  for( n = 0; n < MAX_TYPES; n++)
    {
    if( (choices[n] = (struct choice *)
         XtMalloc(sizeof(struct choice))) == (struct choice *) NULL)
      {
      sprintf((char *) &msg, "%s",
        "Sorry, out of memory (select jb parms) (choices).");
      fatal(main_window, (char *) &msg);
      exit(1);
      }
    list[n] = NULL;
    }

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  orbit_param[0] = NULL;
  orbit_param[1] = NULL;
  orbit_param[2] = NULL;
  orbit_param[3] = NULL;
  orbit_mxmaxfp = NULL;
  orbit_mymaxfp = NULL;
  orbit_mxminfp = NULL;
  orbit_myminfp = NULL;
  orbit_zdots = NULL;
  orbit_juli3Dmode = NULL;
  orbit_eyesfp = NULL;
  orbit_originfp = NULL;
  orbit_depthfp = NULL;
  orbit_heightfp = NULL;
  orbit_widthfp = NULL;
  orbit_distfp = NULL;

  i = -1;
  j = -1;

  while(fractalspecific[++i].name)
    {
    if (!((fractalspecific[i].flags & OKJB) &&
          *fractalspecific[i].name != '*'))
      {
      continue;
      }
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
    if(choices[i]->num == JULIA || choices[i]->num == JULIAFP)
      {
      j = i;
      }
    }

  n = 0;
  str[0] = XmStringCreateLocalized("Select Orbit Algorithm for Julibrot");
  str[1] = XmStringCreateLocalized("Select Orbit Type");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNdialogType, XmDIALOG_SELECTION); n++;
  XtSetArg(args[n], XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;
  XtSetArg(args[n], XmNtextString, list[j]); n++;
  XtSetArg(args[n], XmNlistItems, list); n++;
  XtSetArg(args[n], XmNlistItemCount, numtypes); n++;
  XtSetArg(args[n], XmNlistVisibleItemCount, 10); n++;
  XtSetArg(args[n], XmNmustMatch, True); n++;
  XtSetArg(args[n], XmNtextColumns, 40); n++;
  XtSetArg(args[n], XmNlistLabelString, str[0]); n++;
  XtSetArg(args[n], XmNdialogTitle, str[1]); n++;

  orbit_selection_dialog = XmCreateSelectionDialog
    (main_window, "orbit_selection_dialog", args, n);

  XtUnmanageChild (XmSelectionBoxGetChild
    (orbit_selection_dialog, XmDIALOG_APPLY_BUTTON));

  XtAddCallback(orbit_selection_dialog, XmNapplyCallback,
    orbit_type_ok_cb, NULL);

  XtAddCallback(orbit_selection_dialog, XmNokCallback,
    orbit_type_ok_cb, NULL);

  XtAddCallback(orbit_selection_dialog, XmNnoMatchCallback,
    select_type_err_cb, NULL);

  XtAddCallback(orbit_selection_dialog, XmNmapCallback, map_dialog, NULL);

  XtAddCallback(orbit_selection_dialog, XmNhelpCallback, read_help, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(XtParent(orbit_selection_dialog), (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  for( n = 0; n < MAX_TYPES; n++)
    {
    XtFree((char *) choices[n]);
    XmStringFree(list[n]);
    }

  XmStringFree(str[0]);
  XmStringFree(str[1]);

  helpmode = HT_JULIBROT;

  XtManageChild(orbit_selection_dialog);
  XtPopup(XtParent(orbit_selection_dialog), XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void orbit_type_ok_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  Arg args[MAX_ARGS];
  int i, n;
  int fracbase;
  int btnbase;
  int num_parms = 0;
  char tempstr[64];
  char name[MAXJBPARAMS][64];
  char value[MAXJBPARAMS][64];
  char msg[128];
  Widget button;
  Widget textfield;
  Widget orbit_specific_param_dialog;
  Widget label = NULL;
  Widget frame = NULL;
  XmString str[2];

  XmSelectionBoxCallbackStruct
    *cbs = (XmSelectionBoxCallbackStruct *)call_data;

  XtUnmanageChild(w);
  XtDestroyWidget(w);

  for(i = 0; i < MAXJBPARAMS; i++)
    orbit_index[i] = -1;

  btnbase = 20;
  fracbase = btnbase * 6 - 1;

  orbit_param[0] = NULL;
  orbit_param[1] = NULL;
  orbit_param[2] = NULL;
  orbit_param[3] = NULL;
  orbit_mxmaxfp = NULL;
  orbit_mymaxfp = NULL;
  orbit_mxminfp = NULL;
  orbit_myminfp = NULL;
  orbit_zdots = NULL;

  if(orbit_specific_param_dialog_shell != NULL)
    {
    if(XtIsManaged(orbit_specific_param_dialog_shell))
      XtUnmanageChild(orbit_specific_param_dialog_shell);
    XtDestroyWidget(orbit_specific_param_dialog_shell);
    orbit_specific_param_dialog_shell = NULL;
    }

  orbit_specific_param_dialog_shell = XmCreateDialogShell
    (main_window, "Select Parameters", NULL, 0);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(orbit_specific_param_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  str[0] = XmStringCreateLocalized("Select Parameters");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;
  XtSetArg(args[n], XmNdialogTitle, str[0]); n++;

  orbit_specific_param_dialog = XmCreateForm
    (orbit_specific_param_dialog_shell,
    "orbit_specific_param_dialog", args, n);

  XtAddCallback(orbit_specific_param_dialog, XmNmapCallback,
    map_dialog, NULL);

  XtAddCallback(orbit_specific_param_dialog, XmNhelpCallback, read_help, NULL);

  XmStringFree(str[0]);

  i = -1;
  while(fractalspecific[++i].name)
    {
    sprintf((char *) &tempstr, fractalspecific[i].name);
    str[1] = XmStringCreateLocalized((char *) &tempstr);
    if(XmStringCompare(str[1], cbs->value) == True )
      {
      neworbittype = i;
      jborbit = &fractalspecific[neworbittype];
      savespecific = curfractalspecific;
      curfractalspecific = jborbit;
      helpmode = curfractalspecific->helpformula;
      break;
      }
    XmStringFree(str[1]);
    }

  firstparm = 2;

  if(neworbittype == QUATJULFP || neworbittype == HYPERCMPLXJFP)
    {
    /* all parameters needed */
    firstparm = 0;
    }

  if(neworbittype == QUATFP || neworbittype == HYPERCMPLXFP)
    {
    /* no parameters needed */
    firstparm = 4;
    }

  num_parms = 0;

  for (i = firstparm; i < 4; i++)
    {
    if( i < 4 && (int) strlen(curfractalspecific->param[i]) < 1)
      {
      continue;
      }
    sprintf((char *) &name[num_parms], "%s", curfractalspecific->param[i]);
    sprintf((char *) &value[num_parms], "%+.17g", param[i]);
    orbit_index[num_parms] = i;
    num_parms++;
    }

  switch(neworbittype)
    {
    case QUATFP:
    case HYPERCMPLXFP:
      sprintf((char *) &name[num_parms], "%s", "From cj (3rd dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", mxmaxfp);
      orbit_index[num_parms] = 4;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "From ck (4th dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", mymaxfp);
      orbit_index[num_parms] = 5;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "To   cj (3rd dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", mxminfp);
      orbit_index[num_parms] = 6;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "To   ck (4th dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", myminfp);
      orbit_index[num_parms] = 7;
      num_parms++;
    break;

    case QUATJULFP:
    case HYPERCMPLXJFP:
      sprintf((char *) &name[num_parms], "%s", "From zj (3rd dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", mxmaxfp);
      orbit_index[num_parms] = 4;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "From zk (4th dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", mymaxfp);
      orbit_index[num_parms] = 5;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "To   zj (3rd dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", mxminfp);
      orbit_index[num_parms] = 6;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "To   zk (4th dimension)");
      sprintf((char *) &value[num_parms], "%+.17g", myminfp);
      orbit_index[num_parms] = 7;
      num_parms++;
    break;

    default:
      sprintf((char *) &name[num_parms], "%s", "From cx (real part)");
      sprintf((char *) &value[num_parms], "%+.17g", mxmaxfp);
      orbit_index[num_parms] = 4;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "From cy (imaginary part)");
      sprintf((char *) &value[num_parms], "%+.17g", mymaxfp);
      orbit_index[num_parms] = 5;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "To   cx (real part)");
      sprintf((char *) &value[num_parms], "%+.17g", mxminfp);
      orbit_index[num_parms] = 6;
      num_parms++;
      sprintf((char *) &name[num_parms], "%s", "To   cy (imaginary part)");
      sprintf((char *) &value[num_parms], "%+.17g", myminfp);
      orbit_index[num_parms] = 7;
      num_parms++;
    break;
    }

  sprintf((char *) &name[num_parms], "%s", "Number of z pixels");
  sprintf((char *) &value[num_parms], "%d", zdots);
  orbit_index[num_parms] = 8;
  num_parms++;

  for(i = 0; i < num_parms; i++)
    {
    if( i == 0 )
      {
      n = 0;
      str[1] = XmStringCreateLocalized("Julibrot Orbit Parameters");
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNlabelString, str[1]); n++;

      label = XmCreateLabel
        (orbit_specific_param_dialog, "orbit_param_orbit_header", args, n);

      XtManageChild(label);

      XmStringFree(str[1]);
      }

    n = 0;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    if( i == 0 )
      {
      XtSetArg(args[n], XmNtopWidget, label); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, frame); n++;
      }
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    sprintf((char *) &tempstr, "orbit_param_frame_%d", i + 1);
    frame = XmCreateFrame
      (orbit_specific_param_dialog, (char *) &tempstr, args, n);

    n = 0;
    str[1] = XmStringCreateLocalized((char *) &name[i]);
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str[1]); n++;

    sprintf((char *) &tempstr, "orbit_param_label_%d", i + 1);
    label = XmCreateLabel
      (frame, (char *) &tempstr, args, n);

    XtManageChild(label);
    XmStringFree(str[1]);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNvalue, (char *) &value[i]); n++;
    sprintf((char *) &tempstr, "orbit_param_textfield_%d", i + 1);

    textfield = XmCreateTextField
      (frame, (char *) &tempstr, args, n);

    XtManageChild (textfield);
    XtManageChild (frame);

    switch(orbit_index[i])
      {
      case 0:
        orbit_param[i] = textfield;
      break;

      case 1:
        orbit_param[i] = textfield;
      break;

      case 2:
        orbit_param[i] = textfield;
      break;

      case 3:
        orbit_param[i] = textfield;
      break;

      case 4:
        orbit_mxmaxfp = textfield;
      break;

      case 5:
        orbit_mymaxfp = textfield;
      break;

      case 6:
        orbit_mxminfp = textfield;
      break;

      case 7:
        orbit_myminfp = textfield;
      break;

      case 8:
        orbit_zdots = textfield;
      break;

      default:
        sprintf((char *) &msg,
          "Error - cannot determine parmtype \"%d\" for index \"%d\".",
           orbit_index[i], i);
        caution(main_window, (char *) &msg, NULL);
      break;
      }
    }

  /*
   * Apply
   * (pushbutton)
   *
   */

  n = 0;
  str[1] = XmStringCreateLocalized("Apply");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (orbit_specific_param_dialog, "orbit_param_apply_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    orbit_param_done_cb, (XtPointer) "0");

  XtManageChild(button);
  XmStringFree(str[1]);

  /*
   * Cancel
   * (pushbutton)
   *
   */

  n = 0;
  str[1] = XmStringCreateLocalized("Cancel");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (orbit_specific_param_dialog, "orbit_param_cancel_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    orbit_param_cancel_cb, NULL);

  XtManageChild(button);
  XmStringFree(str[1]);

  /*
   * Done
   * (pushbutton)
   *
   */

  n = 0;
  str[1] = XmStringCreateLocalized("Done");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 5 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (orbit_specific_param_dialog, "orbit_param_done_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    orbit_param_done_cb, (XtPointer) "1");

  XtManageChild(button);
  XmStringFree(str[1]);

  XtVaSetValues(orbit_specific_param_dialog,
    XmNdefaultButton,
    button,
    NULL);

  XtManageChild(orbit_specific_param_dialog);
  XtPopup(XtParent(orbit_specific_param_dialog), XtGrabNone);
}

/*ARGSUSED*/
static void orbit_param_done_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  char *txt;
  char *invalid;
  double validnum;
  char msg[128];
  int done = 1;

  int pop_down = atoi((char *)client_data);

  for(i = 0; i < 4 - firstparm; i++)
    {
    if(orbit_param[i] != NULL && XtIsManaged(orbit_param[i]))
      {
      txt = XmTextFieldGetString(orbit_param[i]);
      invalid = (char *) NULL;
      validnum = strtod(txt, &invalid);
      if( invalid && *invalid )
        {
        done = 0;
        sprintf((char *) &msg,
          "Cannot resolve floating point number \"%s\".", txt);
        caution(main_window, (char *) &msg, NULL);
        }
      param[i] = validnum;
      XtFree(txt);
      }
    }

  if(orbit_mxmaxfp != NULL && XtIsManaged(orbit_mxmaxfp))
    {
    txt = XmTextFieldGetString(orbit_mxmaxfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    mxmaxfp = validnum;
    XtFree(txt);
    }

  if(orbit_mymaxfp != NULL && XtIsManaged(orbit_mymaxfp))
    {
    txt = XmTextFieldGetString(orbit_mymaxfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    mymaxfp = validnum;
    XtFree(txt);
    }

  if(orbit_mxminfp != NULL && XtIsManaged(orbit_mxminfp))
    {
    txt = XmTextFieldGetString(orbit_mxminfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    mxminfp = validnum;
    XtFree(txt);
    }

  if(orbit_myminfp != NULL && XtIsManaged(orbit_myminfp))
    {
    txt = XmTextFieldGetString(orbit_myminfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    myminfp = validnum;
    XtFree(txt);
    }

  if(orbit_zdots != NULL && XtIsManaged(orbit_zdots))
    {
    txt = XmTextFieldGetString(orbit_zdots);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    zdots = validnum;
    XtFree(txt);
    }

  curfractalspecific = &fractalspecific[fractype];

  if(done)
    {
    if(pop_down == 1)
      {
      XtUnmanageChild(orbit_specific_param_dialog_shell);
      XtDestroyWidget(orbit_specific_param_dialog_shell);
      orbit_specific_param_dialog_shell = NULL;
      select_julibrot_3d_parms(main_window);
      }
    }
}

/*ARGSUSED*/
static void orbit_param_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(orbit_specific_param_dialog_shell);
  XtDestroyWidget(orbit_specific_param_dialog_shell);
  orbit_specific_param_dialog_shell = NULL;
}

/*ARGSUSED*/
static void select_julibrot_3d_parms(Widget w)
{
  Arg args[MAX_ARGS];
  int i, n;
  int num_parms = 0;
  int fracbase;
  int btnbase;
  char name[MAXJBPARAMS][64];
  char value[MAXJBPARAMS][64];
  char msg[128];
  char tempstr[64];
  Widget button;
  Widget textfield;
  Widget julibrot_3d_parm_dialog;
  Widget frame = NULL;
  Widget label = NULL;
  XmString str[2];

  for(i = 0; i < MAXJBPARAMS; i++)
    {
    orbit_index[i] = -1;
    }

  orbit_juli3Dmode = NULL;
  orbit_eyesfp = NULL;
  orbit_originfp = NULL;
  orbit_depthfp = NULL;
  orbit_heightfp = NULL;
  orbit_widthfp = NULL;
  orbit_distfp = NULL;

  btnbase = 20;
  fracbase = btnbase * 6 - 1;

  julibrot_3d_parm_dialog_shell = XmCreateDialogShell
    (main_window, "Julibrot 3D Parameters", NULL, 0);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(julibrot_3d_parm_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  str[0] = XmStringCreateLocalized("Select Julibrot 3D Parameters");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;
  XtSetArg(args[n], XmNdialogTitle, str[0]); n++;

  julibrot_3d_parm_dialog = XmCreateForm
    (julibrot_3d_parm_dialog_shell, "julibrot_3d_parm_dialog", args, n);

  XtAddCallback(julibrot_3d_parm_dialog, XmNmapCallback, map_dialog, NULL);

  XmStringFree(str[0]);

  num_parms = 0;

  sprintf((char *) &name[num_parms], "%s",
    "monocular, lefteye, righteye, red-blue");
  sprintf((char *) &value[num_parms], "%s", "monocular");
  orbit_index[num_parms] = 9;
  num_parms++;

  sprintf((char *) &name[num_parms], "%s", "Distance between eyes");
  sprintf((char *) &value[num_parms], "%+.17g", eyesfp);
  orbit_index[num_parms] = 10;
  num_parms++;

  sprintf((char *) &name[num_parms], "%s", "Location of z origin");
  sprintf((char *) &value[num_parms], "%+.17g", originfp);
  orbit_index[num_parms] = 11;
  num_parms++;

  sprintf((char *) &name[num_parms], "%s", "Depth of z");
  sprintf((char *) &value[num_parms], "%+.17g", depthfp);
  orbit_index[num_parms] = 12;
  num_parms++;

  sprintf((char *) &name[num_parms], "%s", "Screen height");
  sprintf((char *) &value[num_parms], "%+.17g", heightfp);
  orbit_index[num_parms] = 13;
  num_parms++;

  sprintf((char *) &name[num_parms], "%s", "Screen width");
  sprintf((char *) &value[num_parms], "%+.17g", widthfp);
  orbit_index[num_parms] = 14;
  num_parms++;

  sprintf((char *) &name[num_parms], "%s", "Distance to Screen");
  sprintf((char *) &value[num_parms], "%+.17g", distfp);
  orbit_index[num_parms] = 15;
  num_parms++;

  for(i = 0; i < num_parms; i++)
    {
    if( i == 0 )
      {
      n = 0;
      str[1] = XmStringCreateLocalized("Julibrot 3D Mode Parameters");
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNlabelString, str[1]); n++;

      label = XmCreateLabel
        (julibrot_3d_parm_dialog, "orbit_param_3d_header", args, n);

      XtManageChild(label);

      XmStringFree(str[1]);
      }

    n = 0;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    if( i == 0 )
      {
      XtSetArg(args[n], XmNtopWidget, label); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopWidget, frame); n++;
      }
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    sprintf((char *) &tempstr, "orbit_param_3d_frame_%d", i + 1);
    frame = XmCreateFrame
      (julibrot_3d_parm_dialog, (char *) &tempstr, args, n);

    n = 0;
    str[1] = XmStringCreateLocalized((char *) &name[i]);
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str[1]); n++;

    sprintf((char *) &tempstr, "orbit_param_3d_label_%d", i + 1);
    label = XmCreateLabel
      (frame, (char *) &tempstr, args, n);

    XtManageChild(label);
    XmStringFree(str[1]);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNvalue, (char *) &value[i]); n++;
    sprintf((char *) &tempstr, "orbit_3d_param_textfield_%d", i + 1);

    textfield = XmCreateTextField
      (frame, (char *) &tempstr, args, n);

    XtManageChild (textfield);
    XtManageChild (frame);

    switch(orbit_index[i])
      {
      case 9:
        orbit_juli3Dmode = textfield;
      break;

      case 10:
        orbit_eyesfp = textfield;
      break;

      case 11:
        orbit_originfp = textfield;
      break;

      case 12:
        orbit_depthfp = textfield;
      break;

      case 13:
        orbit_heightfp = textfield;
      break;

      case 14:
        orbit_widthfp = textfield;
      break;

      case 15:
        orbit_distfp = textfield;
      break;

      default:
        sprintf((char *) &msg,
          "Error - cannot determine parmtype \"%d\" for index \"%d\".",
           orbit_index[i], i);
        caution(main_window, (char *) &msg, NULL);
      break;
      }
    }

  /*
   * Apply
   * (pushbutton)
   *
   */

  n = 0;
  str[1] = XmStringCreateLocalized("Apply");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (julibrot_3d_parm_dialog, "orbit_3d_param_apply_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    orbit_3d_param_done_cb, (XtPointer) "0");

  XtManageChild(button);
  XmStringFree(str[1]);

  /*
   * Cancel
   * (pushbutton)
   *
   */

  n = 0;
  str[1] = XmStringCreateLocalized("Cancel");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (julibrot_3d_parm_dialog, "orbit_3d_param_cancel_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    orbit_3d_param_cancel_cb, NULL);

  XtManageChild(button);
  XmStringFree(str[1]);

  /*
   * Done
   * (pushbutton)
   *
   */

  n = 0;
  str[1] = XmStringCreateLocalized("Done");
  XtSetArg(args[n], XmNlabelString, str[1]); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 5 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (julibrot_3d_parm_dialog, "orbit_3d_param_done_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    orbit_3d_param_done_cb, (XtPointer) "1");

  XtManageChild(button);
  XmStringFree(str[1]);

  XtVaSetValues(julibrot_3d_parm_dialog,
    XmNdefaultButton,
    button,
    NULL);

  XtManageChild(julibrot_3d_parm_dialog);
}

/*ARGSUSED*/
static void orbit_3d_param_done_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *txt;
  char *invalid;
  double validnum;
  char msg[128];
  int done = 1;

  int pop_down = atoi((char *)client_data);

  if(orbit_juli3Dmode != NULL && XtIsManaged(orbit_juli3Dmode))
    {
    txt = XmTextFieldGetString(orbit_juli3Dmode);
    if((strstr(txt, "mono") != (char *) NULL) ||
       (strstr(txt, "Mono") != (char *) NULL) ||
       (strstr(txt, "MONO") != (char *) NULL))
      {
      juli3Dmode = 0;
      }
    else if((strstr(txt, "left") != (char *) NULL) ||
            (strstr(txt, "Left") != (char *) NULL) ||
            (strstr(txt, "LEFT") != (char *) NULL))
      {
      juli3Dmode = 1;
      }
    else if((strstr(txt, "righ") != (char *) NULL) ||
            (strstr(txt, "Righ") != (char *) NULL) ||
            (strstr(txt, "RIGH") != (char *) NULL))
      {
      juli3Dmode = 2;
      }
    else if((strstr(txt, "red") != (char *) NULL) ||
            (strstr(txt, "Red") != (char *) NULL) ||
            (strstr(txt, "RED") != (char *) NULL))
      {
      juli3Dmode = 3;
      }
    else
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve 3D mode \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    XtFree(txt);
    }

  if(orbit_eyesfp != NULL && XtIsManaged(orbit_eyesfp))
    {
    txt = XmTextFieldGetString(orbit_eyesfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    eyesfp = validnum;
    XtFree(txt);
    }

  if(orbit_originfp != NULL && XtIsManaged(orbit_originfp))
    {
    txt = XmTextFieldGetString(orbit_originfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    originfp = validnum;
    XtFree(txt);
    }

  if(orbit_depthfp != NULL && XtIsManaged(orbit_depthfp))
    {
    txt = XmTextFieldGetString(orbit_depthfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    depthfp = validnum;
    XtFree(txt);
    }

  if(orbit_heightfp != NULL && XtIsManaged(orbit_heightfp))
    {
    txt = XmTextFieldGetString(orbit_heightfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    heightfp = validnum;
    XtFree(txt);
    }

  if(orbit_widthfp != NULL && XtIsManaged(orbit_widthfp))
    {
    txt = XmTextFieldGetString(orbit_widthfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    widthfp = validnum;
    XtFree(txt);
    }

  if(orbit_distfp != NULL && XtIsManaged(orbit_distfp))
    {
    txt = XmTextFieldGetString(orbit_distfp);
    invalid = (char *) NULL;
    validnum = strtod(txt, &invalid);
    if( invalid && *invalid )
      {
      done = 0;
      sprintf((char *) &msg,
        "Cannot resolve floating point number \"%s\".", txt);
      caution(main_window, (char *) &msg, NULL);
      }
    distfp = validnum;
    XtFree(txt);
    }

  if(done)
    {
    if(pop_down == 1)
      {
      XtUnmanageChild(julibrot_3d_parm_dialog_shell);
      XtDestroyWidget(julibrot_3d_parm_dialog_shell);
      julibrot_3d_parm_dialog_shell = NULL;
      }
    }
}

/*ARGSUSED*/
static void orbit_3d_param_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(julibrot_3d_parm_dialog_shell);
  XtDestroyWidget(julibrot_3d_parm_dialog_shell);
  julibrot_3d_parm_dialog_shell = NULL;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: julibrot_param.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.14  1995/06/09  19:34:35  darryl
 * Fix bug where global variables were shadowed by local
 * variables of a different type.
 *
 * Revision 1.13  1995/05/18  22:55:48  darryl
 * Re-order buttons in julibrot parameter dialogs.
 *
 * Revision 1.12  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.11  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.10  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.9  1995/04/18  15:59:59  darryl
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
 * Revision 1.8  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.7  1995/03/28  01:09:14  darryl
 * Fix a potential crash problem where textfields were not
 * being initialized or retrieved correctly.
 *
 * Revision 1.6  1995/03/22  18:08:22  darryl
 * Prevent crashes when recursive calls are made to dialog
 * creation routines, enlarge file name and path variable
 * sizes to at least 256 bytes, don't pop down the parent
 * of a warning or caution dialog in the case of a list
 * or file selection dialog mis-match.
 *
 * Revision 1.5  1995/03/21  00:34:23  darryl
 * Add "Apply" buttons to allow values in a popped-up
 * dialog to be applied to the program without having
 * to press the "Done" button, which pops down the dialog.
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
