
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: type_param.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void param_defaults_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void param_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void param_corners_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void param_entry_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void param_file_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void param_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget type_specific_param_dialog_shell;
static Widget type_specific_param_dialog;
static double newparam[MAXPARAMS];
static int default_bailout = 0;
static int new_bailout = 0;
static int new_major_method;
static int new_minor_method;

/*ARGSUSED*/
void type_specific_params(Widget w)
{
  int n, i;
  int num_parms;
  int extra;
  Arg args[MAX_ARGS];
  Widget label;
  Widget button;
  Widget textfield;
  Widget frame = NULL;
  Widget which;
  XmString str;
  char widget_name[64];
  char name[MAXPARAMS][64];
  char value[MAXPARAMS][64];
  int numtrig;
  int btnbase;
  int fracbase;
  char *typename;
  char *tmpptr;
  char msg[128];
  char tempstr[64];

  num_parms = 0;

  btnbase = 20;
  fracbase = btnbase * 6 - 1;

  if(type_specific_param_dialog_shell != NULL)
    {
    if(XtIsManaged(type_specific_param_dialog_shell))
      XtUnmanageChild(type_specific_param_dialog_shell);
    XtDestroyWidget(type_specific_param_dialog_shell);
    type_specific_param_dialog_shell = NULL;
    }

  for(i = 0; i < MAXPARAMS; i++)
    {
    parmtype[i].type = -1;
    parmtype[i].parm = -1;
    }

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  curtype = fractype;

  if (curfractalspecific->name[0] == '*' &&
      (i = fractalspecific->tofloat) != NOFRACTAL &&
      fractalspecific[i].name[0] != '*')
    {
    fractype = curtype = i;
    }

  curfractalspecific = &fractalspecific[curtype];

  if(fractype == FORMULA || fractype == FFORMULA)
    {
    if((((int)strcmp((char *) &oldFormName, (char *) &FormName)) != 0) ||
       (((int)strcmp((char *) &oldFormFileName, (char *) &FormFileName)) != 0))
      {
      calc_status = IMG_PRMS_CHGD;
      update_draw_menu(NEW_IMAGE);
      toggle_zoom_grab(ZOOM_OFF);
      }
    }

  if(fractype == IFS || fractype == IFS3D)
    {
    if((((int)strcmp((char *) &oldIFSName, (char *) &IFSName)) != 0) ||
       (((int)strcmp((char *) &oldIFSFileName, (char *) &IFSFileName)) != 0))
      {
      calc_status = IMG_PRMS_CHGD;
      update_draw_menu(NEW_IMAGE);
      toggle_zoom_grab(ZOOM_OFF);
      }
    }

  if(fractype == LSYSTEM)
    {
    if((((int)strcmp((char *) &oldLName, (char *) &LName)) != 0) ||
      (((int)strcmp((char *) &oldLFileName, (char *) &LFileName)) != 0))
      {
      calc_status = IMG_PRMS_CHGD;
      update_draw_menu(NEW_IMAGE);
      toggle_zoom_grab(ZOOM_OFF);
      }
    }

  if(fractype != curtype)
    {
    if(!forwarding)
      set_default_parms();
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    toggle_zoom_grab(ZOOM_OFF);
    for(i = 0; i < MAXPARAMS; i++)
      newparam[i] = param[i];
      curparam[i] = param[i];
    oldtype = curtype;
    }
  else
    {
    for(i = 0; i < MAXPARAMS; i++)
      {
      newparam[i] = param[i];
      curparam[i] = param[i];
      }
    }

  if(!functionpreloaded)
    {
    if((fractype == BIFURCATION) || (fractype == LBIFURCATION))
      {
      set_trig_array(0,"ident");
      functionpreloaded = 1;
      }

    if((fractype == BIFSTEWART) || (fractype == LBIFSTEWART))
      {
      set_trig_array(0,"ident");
      functionpreloaded = 1;
      }

    if((fractype == BIFLAMBDA) || (fractype == LBIFLAMBDA))
      {
      set_trig_array(0,"ident");
      functionpreloaded = 1;
      }

    if((fractype == BIFEQSINPI) || (fractype == LBIFEQSINPI))
      {
      set_trig_array(0,"sin");
      functionpreloaded = 1;
      }

    if((fractype == BIFADSINPI) || (fractype == LBIFADSINPI))
      {
      set_trig_array(0,"sin");
      functionpreloaded = 1;
      }

    sprintf(newtrigndx_str[0], trigfn[trigndx[0]].name);
    }

  type_specific_param_dialog_shell = XmCreateDialogShell
    (main_window, "Select Parameters", NULL, 0);

  XtAddCallback(type_specific_param_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(type_specific_param_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  for(i = 0; i < 4; i++)
    {
    if( i < 4 && (int) strlen(curfractalspecific->param[i]) < 1)
      continue;
    sprintf((char *) &name[num_parms], "%s", curfractalspecific->param[i]);
    if(fractype != oldtype)
      sprintf((char *) &value[num_parms], "%+.17g", curparam[i]);
    else
      sprintf((char *) &value[num_parms], "%+.17g", curparam[i]);
    parmtype[num_parms].type = RGLRPARM;
    parmtype[num_parms].parm = i;
    num_parms++;
    }

  if(curfractalspecific->flags&MORE && !julibrot)
    {
    if((extra=find_extra_param(fractype))<-1)
      {
      sprintf((char *) &msg, "find_extra_param error.\n");
      fatal(main_window, (char *) &msg);
      exit(1);
      }
    else
      {
      for (i = 0; i < MAXPARAMS - 4; i++)
        {
        if (moreparams[extra].param[i][0] == 0)
          break;
        sprintf((char *) &name[num_parms], "%s", moreparams[extra].param[i]);
        if(fractype != oldtype)
          sprintf((char *) &value[num_parms], "%+.17g", curparam[i+4]);
        else
          sprintf((char *) &value[num_parms], "%+.17g", curparam[i+4]);
        parmtype[num_parms].type = MOREPARM;
        parmtype[num_parms].parm = i;
        num_parms++;
        }
      }
    }

  numtrig = (curfractalspecific->flags >> 6) & 7;

  if(curtype==FORMULA || curtype==FFORMULA)
    numtrig = maxfn;

  for (i = 0; i < numtrig; i++)
    {
    sprintf((char *) &name[num_parms],
      "%s %d %s", "Function number", i + 1, ":");
    if((fractype != oldtype) && (!functionpreloaded))
      {
      sprintf(curtrigndx_str[i], trigfn[trigndx[i]].name);
      sprintf(newtrigndx_str[i], trigfn[trigndx[i]].name);
      sprintf((char *) &value[num_parms], "%s", curtrigndx_str[i]);
      set_trig_array(i, curtrigndx_str[i]);
      }
    else
      {
      sprintf(newtrigndx_str[i], "%s", curtrigndx_str[i]);
      sprintf((char *) &value[num_parms], "%s", curtrigndx_str[i]);
      set_trig_array(i, curtrigndx_str[i]);
      }
    parmtype[num_parms].type = TRIGPARM;
    parmtype[num_parms].parm = i;
    num_parms++;
    }

  if (*(typename = curfractalspecific->name) == '*')
    ++typename;

  if (curtype == INVERSEJULIA || curtype == INVERSEJULIAFP)
    {
    sprintf((char *) &name[num_parms],
      "%s", "Breadth first, Depth first, Random Walk");
    sprintf((char *) &value[num_parms], "%s", "Breadth First");
    parmtype[num_parms].type = JIIMPARM;
    parmtype[num_parms].parm = 0;
    num_parms++;

    sprintf((char *) &name[num_parms], "%s", "Left first or Right first");
    sprintf((char *) &value[num_parms], "%s", "Left First");
    parmtype[num_parms].type = JIIMPARM;
    parmtype[num_parms].parm = 1;
    num_parms++;
    }

  if (curfractalspecific->orbit_bailout)
    {
    if (potparam[0] != 0.0 && potparam[2] != 0.0)
      {
      sprintf((char *) &name[num_parms], "%s",
        "Bailout: continuous potential (Y screen) value in use");
      }
    else
      {
      sprintf((char *) &name[num_parms],
        "%s", "Bailout value (0 means use default)");
      tmpptr = typename;
      i = curfractalspecific->orbit_bailout;
      sprintf((char *) &name[num_parms], "%s\n%s%s%s%d%s",
        (char *) &name[num_parms], "( ", tmpptr, " default is ", i, " )");
      sprintf((char *) &value[num_parms], "%d", new_bailout);
      parmtype[num_parms].type = BAILPARM;
      parmtype[num_parms].parm = 0;
      num_parms++;
      }
    }

  helpmode = curfractalspecific->helpformula;

  n = 0;
  str = XmStringCreateLocalized("Select Parameters");
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNdialogTitle, str); n++;
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;

  type_specific_param_dialog = XmCreateForm
    (type_specific_param_dialog_shell,
      "type_specific_param_dialog", args, n);

  XmStringFree(str);

  which = NULL;

  n = 0;
  if(which == NULL)
    {
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    }
  else
    {
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, which); n++;
    }
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  frame = XmCreateFrame(type_specific_param_dialog,
    "type_param_type_frame", args, n);

  n = 0;
  sprintf((char *) &tempstr, "Fractal type: %s", typename);
  str = XmStringCreateLocalized((char *) &tempstr);
  XtSetArg(args[n], XmNlabelString, str); n++;
  label = XmCreateLabel(frame,
    "type_param_type_label", args, n);

  XtManageChild(label);
  XtManageChild(frame);
  XmStringFree(str);

  which = frame;

  if(gotfrmfile && gotfrmname)
    {
    n = 0;
    if(which == NULL)
      {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNtopWidget, which); n++;
      }
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    frame = XmCreateFrame(type_specific_param_dialog,
      "type_param_frmfile_frame", args, n);

    n = 0;
    sprintf((char *) &tempstr, "Formula file: %s",
      (char *) &FormFileName);
    str = XmStringCreateLocalized((char *) &tempstr);
    XtSetArg(args[n], XmNlabelString, str); n++;
    label = XmCreateLabel(frame,
      "type_param_frmfile_label", args, n);

    XtManageChild(label);
    XtManageChild(frame);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    frame = XmCreateFrame(type_specific_param_dialog,
      "type_param_frmname_frame", args, n);

    n = 0;
    sprintf((char *) &tempstr, "Formula name: %s",
      (char *) &FormName);
    str = XmStringCreateLocalized((char *) &tempstr);
    XtSetArg(args[n], XmNlabelString, str); n++;
    label = XmCreateLabel(frame,
      "type_param_frmname_label", args, n);

    XtManageChild(label);
    XtManageChild(frame);
    XmStringFree(str);

    which = frame;
    }

  if(gotifsfile && gotifsname)
    {
    n = 0;
    if(which == NULL)
      {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNtopWidget, which); n++;
      }
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    frame = XmCreateFrame(type_specific_param_dialog,
      "type_param_ifsfile_frame", args, n);

    n = 0;
    sprintf((char *) &tempstr, "IFS file: %s",
      (char *) &IFSFileName);
    str = XmStringCreateLocalized((char *) &tempstr);
    XtSetArg(args[n], XmNlabelString, str); n++;
    label = XmCreateLabel(frame,
      "type_param_ifsfile_label", args, n);

    XtManageChild(label);
    XtManageChild(frame);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    frame = XmCreateFrame(type_specific_param_dialog,
      "type_param_ifsname_frame", args, n);

    n = 0;
    sprintf((char *) &tempstr, "IFS entry name: %s",
      (char *) &IFSName);
    str = XmStringCreateLocalized((char *) &tempstr);
    XtSetArg(args[n], XmNlabelString, str); n++;
    label = XmCreateLabel(frame,
      "type_param_ifsname_label", args, n);

    XtManageChild(label);
    XtManageChild(frame);
    XmStringFree(str);

    which = frame;
    }

  if(gotlsysfile && gotlsysname)
    {
    n = 0;
    if(which == NULL)
      {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
      }
    else
      {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNtopWidget, which); n++;
      }
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    frame = XmCreateFrame(type_specific_param_dialog,
      "type_param_lsysfile_frame", args, n);

    n = 0;
    sprintf((char *) &tempstr, "L-System file: %s",
      (char *) &LFileName);
    str = XmStringCreateLocalized((char *) &tempstr);
    XtSetArg(args[n], XmNlabelString, str); n++;
    label = XmCreateLabel(frame,
      "type_param_lsysfile_label", args, n);

    XtManageChild(label);
    XtManageChild(frame);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    frame = XmCreateFrame(type_specific_param_dialog,
      "type_param_lsysname_frame", args, n);

    n = 0;
    sprintf((char *) &tempstr, "L-System name: %s",
      (char *) &LName);
    str = XmStringCreateLocalized((char *) &tempstr);
    XtSetArg(args[n], XmNlabelString, str); n++;
    label = XmCreateLabel(frame,
      "type_param_lsysname_label", args, n);

    XtManageChild(label);
    XtManageChild(frame);
    XmStringFree(str);

    which = frame;
    }

  for(i = 0; i < num_parms; i++)
    {
    n = 0;
    if(i == 0)
      {
      if(which == NULL)
        {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, which); n++;
        }
      }
    else
      {
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNtopWidget, frame); n++;
      }
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    sprintf((char *) &tempstr, "type_param_frame_%d", i + 1);
    frame = XmCreateFrame
      (type_specific_param_dialog, (char *) &tempstr, args, n);

    n = 0;
    str = XmStringCreateLtoR((char *) &name[i], XmFONTLIST_DEFAULT_TAG);
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    sprintf((char *) &tempstr, "type_param_label_%d", i + 1);
    label = XmCreateLabel
      (frame, (char *) &tempstr, args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNeditable, True); n++;
    XtSetArg(args[n], XmNvalue, (char *) &value[i]); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, label); n++;
    sprintf((char *) &widget_name, "type_param_textfield_%d", i + 1);

    textfield = XmCreateTextField
      (frame, (char *) &widget_name, args, n);

    XtManageChild (textfield);
    XtManageChild(frame);
    which = frame;
    }

  /*
   * Return to %s file selection.
   * where %s is [ifs || formula || lsystem]
   * (pushbutton)
   *
   */

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_file_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    param_file_btn_cb, NULL);

  XtAddCallback(type_specific_param_dialog, XmNmapCallback,
    map_dialog, NULL);

  if(gotfrmfile && gotfrmname)
    {
    str = XmStringCreateLocalized
      ("Return to formula file name selection");
    XtVaSetValues(button, XmNlabelString, str, NULL);
    XtManageChild(button);
    XmStringFree(str);
    which = button;
    }

  if(gotifsfile && gotifsname)
    {
    str = XmStringCreateLocalized
      ("Return to IFS file name selection");
    XtVaSetValues(button, XmNlabelString, str, NULL);
    XtManageChild(button);
    XmStringFree(str);
    which = button;
    }

  if(gotlsysfile && gotlsysname)
    {
    str = XmStringCreateLocalized
      ("Return to L-System file name selection");
    XtVaSetValues(button, XmNlabelString, str, NULL);
    XtManageChild(button);
    XmStringFree(str);
    which = button;
    }

  /*
   * Return to %s entry name selection.
   * where %s is [ifs || formula || lsystem]
   * (pushbutton)
   *
   */

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_entry_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    param_entry_btn_cb, NULL);

  XtAddCallback(type_specific_param_dialog, XmNmapCallback,
    map_dialog, NULL);

  if(gotfrmfile && gotfrmname)
    {
    str = XmStringCreateLocalized
      ("Return to formula entry name selection");
    XtVaSetValues(button, XmNlabelString, str, NULL);
    XtManageChild(button);
    XmStringFree(str);
    which = button;
    }

  if(gotifsfile && gotifsname)
    {
    str = XmStringCreateLocalized
      ("Return to IFS entry name selection");
    XtVaSetValues(button, XmNlabelString, str, NULL);
    XtManageChild(button);
    XmStringFree(str);
    which = button;
    }

  if(gotlsysfile && gotlsysname)
    {
    str = XmStringCreateLocalized
      ("Return to L-System entry name selection");
    XtVaSetValues(button, XmNlabelString, str, NULL);
    XtManageChild(button);
    XmStringFree(str);
    which = button;
    }

  /*
   * Corner coordinates
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Corner coordinates");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_corners_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    param_corners_cb, NULL);

  XmStringFree(str);

  XtManageChild(button);

  XtAddCallback(type_specific_param_dialog, XmNmapCallback,
    map_dialog, NULL);

  which = button;

  /*
   * Help
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Help");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 1); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_help_btn", args, n);

  XtAddCallback(button, XmNactivateCallback, read_help, NULL);

  XtManageChild(button);
  XmStringFree(str);

  /*
   * Apply
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Apply");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_apply_btn", args, n);

  XtAddCallback(button, XmNactivateCallback, param_done_cb, (XtPointer) "0");

  XtManageChild(button);
  XmStringFree(str);

  /*
   * Reset
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Reset");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_reset_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    param_reset_btn_cb, NULL);

  XtManageChild(button);
  XmStringFree(str);

  /*
   * Defaults
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Defaults");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_defaults_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    param_defaults_cb, NULL);

  XtManageChild(button);
  XmStringFree(str);

  /*
   * Cancel
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Cancel");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_cancel_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    param_cancel_cb, NULL);

  XtManageChild(button);
  XmStringFree(str);

  /*
   * Done
   * (pushbutton)
   *
   */

  n = 0;
  str = XmStringCreateLocalized("Done");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, which); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, btnbase * 5); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (type_specific_param_dialog, "param_done_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    param_done_cb, (XtPointer) "1");

  XtManageChild(button);
  XmStringFree(str);

  XtVaSetValues(type_specific_param_dialog,
    XmNdefaultButton,
    button,
    NULL);

  XtManageChild(type_specific_param_dialog);
  XtManageChild(type_specific_param_dialog_shell);
  XtPopup(type_specific_param_dialog_shell, XtGrabNone);

  if(!forwarding && (curtype != fractype))
    param_defaults_cb(type_specific_param_dialog, NULL, NULL);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void param_defaults_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  int parm_type;
  int which;
  int changed;
  int Rglrparm;
  int Moreparm;
  int Bailparm;
  int Trigparm;
  int Jiimparm;
  char *txt;
  char tempstr[64];
  Widget textfield;

  changed = 0;

  functionpreloaded = 0;

  set_default_parms();

  switch(fractype)
    {
    case BIFURCATION:
    case LBIFURCATION:
    case BIFSTEWART:
    case LBIFSTEWART:
    case BIFLAMBDA:
    case LBIFLAMBDA:
      set_trig_array(0,"ident");
    break;

    case BIFEQSINPI:
    case LBIFEQSINPI:
    case BIFADSINPI:
    case LBIFADSINPI:
      set_trig_array(0,"sin");
    break;
    }

  Rglrparm = RGLRPARM;
  Moreparm = MOREPARM;
  Bailparm = BAILPARM;
  Trigparm = TRIGPARM;
  Jiimparm = JIIMPARM;

  for( i = 0; i < MAXPARAMS; i++)
    {
    if(parmtype[i].type == -1)
      {
      break;
      }
    parm_type = parmtype[i].type;
    which = parmtype[i].parm;
    sprintf((char *) &tempstr, "*type_param_textfield_%d", i + 1);
    textfield = XtNameToWidget
      (type_specific_param_dialog_shell, (char *) &tempstr);

    if(textfield && XtIsManaged(textfield))
      txt = XmTextFieldGetString(textfield);
    else
      continue;

    if(parm_type == Rglrparm)
      {
      sprintf((char *) &tempstr, "%+.17g", defparam[i]);
      XmTextFieldSetString(textfield, (char *) &tempstr);
      if((int)strcmp((char *) &tempstr, txt) != 0)
        changed = 1;
      }
    else if(parm_type == Moreparm)
      {
      sprintf((char *) &tempstr, "%+.17g", defparam[i]);
      XmTextFieldSetString(textfield, (char *) &tempstr);
      if((int)strcmp((char *) &tempstr, txt) != 0)
        changed = 1;
      }
    else if(parm_type == Bailparm)
      {
      sprintf((char *) &tempstr, "%d", default_bailout);
      XmTextFieldSetString(textfield, (char *) &tempstr);
      if((int)strcmp((char *) &tempstr, txt) != 0)
        changed = 1;
      }
    else if(parm_type == Trigparm)
      {
      sprintf((char *) &tempstr, "%s", curtrigndx_str[which]);
      XmTextFieldSetString(textfield, (char *) &tempstr);
      if((int)strcmp((char *) &tempstr, txt) != 0)
        changed = 1;
      }
    else if(parm_type == Jiimparm)
      {
      if(which == 0)
        {
        sprintf((char *) &tempstr, "%s", "Breadth First");
        XmTextFieldSetString(textfield, (char *) &tempstr);
        if((int)strcmp((char *) &tempstr, txt) != 0)
          changed = 1;
        }
      else
        {
        sprintf((char *) &tempstr, "%s", "Left First");
        XmTextFieldSetString(textfield, (char *) &tempstr);
        if((int)strcmp((char *) &tempstr, txt) != 0)
          changed = 1;
        }
      }
    }

  if(changed)
    {
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    toggle_zoom_grab(ZOOM_OFF);
    }
}

/*ARGSUSED*/
void param_done_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i, n;
  int parm_type;
  int which;
  int changed;
  char *txt;
  char *invalid;
  double validnum;
  int Rglrparm;
  int Moreparm;
  int Bailparm;
  int Trigparm;
  int Jiimparm;
  char msg[128];
  char tempstr[64];
  Widget textfield;
  extern int major_method;
  extern int minor_method;

  int pop_down = atoi((char *)client_data);

  Rglrparm = RGLRPARM;
  Moreparm = MOREPARM;
  Bailparm = BAILPARM;
  Trigparm = TRIGPARM;
  Jiimparm = JIIMPARM;

  changed = 0;

  for(i = 0; i < MAXPARAMS; i++)
    {
    if(parmtype[i].type == -1)
      {
      break;
      }
    parm_type = parmtype[i].type;
    which = parmtype[i].parm;
    sprintf((char *) &tempstr, "*type_param_textfield_%d", i + 1);
    textfield = XtNameToWidget
      (type_specific_param_dialog_shell, (char *) &tempstr);

    txt = XmTextFieldGetString(textfield);

    if(parm_type == Rglrparm)
      {
      invalid = (char *) NULL;
      validnum = strtod(txt, &invalid);

      if( invalid && *invalid )
        {
        sprintf((char *) &msg,
          "Cannot resolve floating point number \"%s\".", txt);
        caution(main_window, (char *) &msg, NULL);
        return;
        }
      if(param[which] != validnum)
        changed = 1;
      param[which] = validnum;
      newparam[which] = validnum;
      }
    else if(parm_type == Moreparm)
      {
      invalid = (char *) NULL;
      validnum = strtod(txt, &invalid);

      if( invalid && *invalid )
        {
        sprintf((char *) &msg,
          "Cannot resolve floating point number \"%s\".", txt);
        caution(main_window, (char *) &msg, NULL);
        return;
        }
      if(param[which + 4] != validnum)
        changed = 1;
      param[which + 4] = validnum;
      newparam[which + 4] = validnum;
      curparam[which + 4] = validnum;
      }
    else if(parm_type == Bailparm)
      {
      invalid = (char *) NULL;
      validnum = strtod(txt, &invalid);

      if( invalid && *invalid )
        {
        sprintf((char *) &msg,
          "Cannot resolve integer \"%s\".", txt);
        caution(main_window, (char *) &msg, NULL);
        return;
        }
      if (potparam[0] != 0.0 && potparam[2] != 0.0)
        continue;
      else
        {
        new_bailout = validnum;
        if(bailout != new_bailout)
          changed = 1;
        bailout = new_bailout;
        }
      }
    else if(parm_type == Trigparm)
      {
      int trig_found = 0;
      for(n = 0; n < numtrigfn; n++)
        if((int)(strcmp(txt, trigfn[n].name)) == 0)
          trig_found = 1;
      if(trig_found)
        {
        if((int)strcmp(curtrigndx_str[which], txt) != 0)
          changed = 1;
        sprintf(curtrigndx_str[which], txt);
        sprintf(newtrigndx_str[which], txt);
        set_trig_array(which, txt);
        functionpreloaded = 1;
        }
      else
        {
        sprintf((char *) &msg,
          "Cannot resolve trig function name \"%s\".", txt);
        caution(main_window, (char *) &msg, NULL);
        return;
        }
      }
    else if(parm_type == Jiimparm)
      {
      new_major_method = major_method;
      new_minor_method = minor_method;
      if(which == 0)
        {
        if((strstr(txt, "bread") != (char *) NULL) ||
           (strstr(txt, "Bread") != (char *) NULL) ||
           (strstr(txt, "BREAD") != (char *) NULL))
          {
          major_method = 0;
          }
        else if ((strstr(txt, "depth") != (char *) NULL) ||
                 (strstr(txt, "Depth") != (char *) NULL) ||
                 (strstr(txt, "DEPTH") != (char *) NULL))
          {
          major_method = 1;
          }
        else if ((strstr(txt, "rand")  != (char *) NULL) ||
                 (strstr(txt, "Rand")  != (char *) NULL) ||
                 (strstr(txt, "RAND")  != (char *) NULL))
          {
          major_method = 2;
          }
        else
          major_method = 1;
        }
      else if(which == 1)
        {
        if((strstr(txt, "left") != (char *) NULL) ||
           (strstr(txt, "Left") != (char *) NULL) ||
           (strstr(txt, "LEFT") != (char *) NULL))
          {
          minor_method = 0;
          }
        else if ((strstr(txt, "right") != (char *) NULL) ||
                 (strstr(txt, "Right") != (char *) NULL) ||
                 (strstr(txt, "RIGHT") != (char *) NULL))
          {
          minor_method = 1;
          }
        else
          minor_method = 0;
        }
      if(new_major_method != major_method ||
         new_minor_method != minor_method)
        changed = 1;
      }
    if(curparam[i] != newparam[i])
      changed = 1;
    }

  if(changed)
    {
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    toggle_zoom_grab(ZOOM_OFF);
    }

  if(pop_down == 1)
    {
    XtUnmanageChild(type_specific_param_dialog_shell);
    XtDestroyWidget(type_specific_param_dialog_shell);
    type_specific_param_dialog_shell = NULL;
    }
}

/*ARGSUSED*/
static void param_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  param_reset_btn_cb(type_specific_param_dialog, NULL, NULL);
  param_done_cb(type_specific_param_dialog, (XtPointer) "0", NULL);
  XtUnmanageChild(type_specific_param_dialog_shell);
  XtDestroyWidget(type_specific_param_dialog_shell);
  type_specific_param_dialog_shell = NULL;
}

/*ARGSUSED*/
static void param_corners_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char tempstr[64];
  param_done_cb(type_specific_param_dialog, (XtPointer) "1", NULL);
  sprintf((char *) &tempstr, "%d", usemag);
  create_corner_param_dialog(main_window,
    (XtPointer)((char *) &tempstr), NULL);
}

void set_default_parms()
{
  int i;

  if(display3d || showfile)
    {
    showfile = 0;
    display3d = 0;
    }

  xxmin = curfractalspecific->xmin;
  xxmax = curfractalspecific->xmax;
  yymin = curfractalspecific->ymin;
  yymax = curfractalspecific->ymax;
  xx3rd = xxmin;
  yy3rd = yymin;

  if (viewcrop && finalaspectratio != screenaspect)
    aspectratio_crop(screenaspect,finalaspectratio);

  for (i = 0; i < 4; i++)
    {
    param[i] = curfractalspecific->paramvalue[i];
    defparam[i] = curfractalspecific->paramvalue[i];
    curparam[i] = curfractalspecific->paramvalue[i];
    if (fractype != CELLULAR) /* don't round cellular */
      {
      roundfloatd(&param[i]);
      roundfloatd(&curparam[i]);
      roundfloatd(&defparam[i]);
      }
    }

  if(curfractalspecific->flags&MORE)
    {
    int extra;
    if((extra=find_extra_param(fractype)) > -1)
      {
      for(i=0;i<MAXPARAMS-4;i++)
        {
        param[i+4] = moreparams[extra].paramvalue[i];
        defparam[i+4] = moreparams[extra].paramvalue[i];
        curparam[i+4] = moreparams[extra].paramvalue[i];
        }
      }
    }

  if (curfractalspecific->orbit_bailout)
    default_bailout = 0;

  if(!functionpreloaded)
    {
    set_trig_array(0,"sin");
    set_trig_array(1,"sqr");
    set_trig_array(2,"sinh");
    set_trig_array(3,"cosh");
    set_trig_pointers(-1);
    }

  corner_val.dflt.xxmin = curfractalspecific->xmin;
  corner_val.dflt.xxmax = curfractalspecific->xmax;
  corner_val.dflt.yymin = curfractalspecific->ymin;
  corner_val.dflt.yymax = curfractalspecific->ymax;
  corner_val.dflt.xx3rd = curfractalspecific->xmin;
  corner_val.dflt.yy3rd = curfractalspecific->ymin;
}

/*ARGSUSED*/
static void param_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i;
  int parm_type;
  int which;
  int Rglrparm;
  int Moreparm;
  int Bailparm;
  int Trigparm;
  int Jiimparm;
  char tempstr[64];
  Widget textfield;
  extern int major_method;
  extern int minor_method;

  Rglrparm = RGLRPARM;
  Moreparm = MOREPARM;
  Bailparm = BAILPARM;
  Trigparm = TRIGPARM;
  Jiimparm = JIIMPARM;

  for( i = 0; i < MAXPARAMS; i++)
    {
    if(parmtype[i].type == -1)
      {
      break;
      }
    parm_type = parmtype[i].type;
    which = parmtype[i].parm;
    sprintf((char *) &tempstr, "*type_param_textfield_%d", i + 1);
    textfield = XtNameToWidget
      (type_specific_param_dialog_shell, (char *) &tempstr);

    if(parm_type == Rglrparm)
      {
      if(textfield && XtIsManaged(textfield))
        {
        sprintf((char *) &tempstr, "%+.17g", newparam[i]);
        XmTextFieldSetString(textfield, (char *) &tempstr);
        }
      }
    else if(parm_type == Moreparm)
      {
      if(textfield && XtIsManaged(textfield))
        {
        sprintf((char *) &tempstr, "%+.17g", newparam[i]);
        XmTextFieldSetString(textfield, (char *) &tempstr);
        }
      }
    else if(parm_type == Bailparm)
      {
      if(textfield && XtIsManaged(textfield))
        {
        sprintf((char *) &tempstr, "%d", new_bailout);
        XmTextFieldSetString(textfield, (char *) &tempstr);
        }
      }
    else if(parm_type == Trigparm)
      {
      if(textfield && XtIsManaged(textfield))
        {
        sprintf((char *) &tempstr, "%s", newtrigndx_str[which]);
        XmTextFieldSetString(textfield, (char *) &tempstr);
        }
      }
    else if(parm_type == Jiimparm)
      {
      if(which == 0)
        {
        if(textfield && XtIsManaged(textfield))
          {
          switch(major_method)
            {
            case 1:
              sprintf((char *) &tempstr, "%s", "Depth First");
              XmTextFieldSetString(textfield, (char *) &tempstr);
            break;

            case 2:
              sprintf((char *) &tempstr, "%s", "Random Walk");
              XmTextFieldSetString(textfield, (char *) &tempstr);
            break;

            case 0:
            default:
              sprintf((char *) &tempstr, "%s", "Breadth First");
              XmTextFieldSetString(textfield, (char *) &tempstr);
            break;
            }
          }
        }
      else
        {
        if(textfield && XtIsManaged(textfield))
          {
          switch(minor_method)
            {
            case 1:
              sprintf((char *) &tempstr, "%s", "Right First");
              XmTextFieldSetString(textfield, (char *) &tempstr);
            break;

            case 0:
            default:
              sprintf((char *) &tempstr, "%s", "Left First");
              XmTextFieldSetString(textfield, (char *) &tempstr);
            break;
            }
          }
        }
      }
    }
}

/*ARGSUSED*/
static void param_entry_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(type_specific_param_dialog_shell);
  XtDestroyWidget(type_specific_param_dialog_shell);
  type_specific_param_dialog_shell = NULL;

  if(gotfrmfile)
    {
    select_frm_entry(main_window, (char *) &FormFileName);
    }

  if(gotifsfile)
    {
    select_ifs_entry(main_window, (char *) &IFSFileName);
    }

  if(gotlsysfile)
    {
    select_lsys_entry(main_window, (char *) &LFileName);
    }
}

/*ARGSUSED*/
static void param_file_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtUnmanageChild(type_specific_param_dialog_shell);
  XtDestroyWidget(type_specific_param_dialog_shell);
  type_specific_param_dialog_shell = NULL;

  if(gotfrmfile)
    select_formula_file(main_window, "1");

  if(gotifsfile)
    select_ifs_file(main_window, "1");

  if(gotlsysfile)
    select_lsystem_file(main_window, "1");
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: type_param.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.26  1995/06/05  16:30:14  tom
 * File handling enhancements. Remove DOS-specific code.
 *
 * Revision 1.25.1.1  1995/06/05  14:10:35  tom
 * changed filename handling
 *
 * Revision 1.25  1995/05/17  03:22:17  darryl
 * Fix a problem where 3D transforms were not cleared after
 * selecting a different fractal type.
 *
 * Revision 1.24  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.23  1995/05/10  22:14:04  darryl
 * Delete unused code and variables, fix problems
 * reported in beta test, enhance the file selection
 * dialogs and restore menu.
 *
 * Revision 1.22  1995/05/09  21:03:55  darryl
 * Fix bugs reported in beta test.
 *
 * Revision 1.21  1995/05/03  21:29:57  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.20  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.19  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.18  1995/04/19  22:16:52  darryl
 * Fix a bug in the parameter selection textfield handling.
 *
 * Revision 1.17  1995/04/19  19:36:07  darryl
 * Fix bugs in parameter forwarding options.
 *
 * Revision 1.16  1995/04/18  15:59:59  darryl
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
 * Revision 1.15  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.14  1995/03/28  22:47:41  darryl
 * Fix the "Defaults" function in the type parameters dialog.
 * Add a "Reset" function to type parameters dialog.
 * Fix the help dialogs so that any number of dialogs may
 * be opened simultaneously.
 *
 * Revision 1.13  1995/03/28  18:11:40  darryl
 * Fix trig function handling. Enhance center-mag button
 * management. Aesthetic changes in the parameter dialog.
 * Add frames around groups of scale widgets. Fix a bogus
 * message about color cell allocation if sharecolor.
 *
 * Revision 1.12  1995/03/28  01:09:14  darryl
 * Fix a potential crash problem where textfields were not
 * being initialized or retrieved correctly.
 *
 * Revision 1.11  1995/03/27  22:54:08  darryl
 * Add a safety check so that if the xgc is null the color
 * editor cannot be invoked. Change a button name for uniqueness.
 *
 * Revision 1.10  1995/03/27  20:21:18  darryl
 * Fix precedence of initialization file processing.
 * Fix parameter function initialization.
 *
 * Revision 1.9  1995/03/26  22:28:18  darryl
 * Add buttons to the type selection dialogs for returning
 * to a file or name selection dialog if the type is
 * formula, lsystem, or ifs. Update the help dialog so that
 * only one may be displayed at a time. Display a clock
 * cursor while an image is being generated.
 *
 * Revision 1.8  1995/03/25  20:27:49  darryl
 * Add cancel buttons for several dialogs. Make the corner
 * values dialog title dynamic, reflecting the current mode.
 *
 * Revision 1.7  1995/03/25  08:42:49  darryl
 * Fixes and enhancements -- make the parameter selection
 * understand that if a parameter file is loaded, then
 * the ifs, lsys, or formula file does not need to be
 * re-selected. Display the current fractal type and
 * any associated file and file entries at the top of
 * the parameters screen. Delete some unused code.
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
 * Revision 0.107  1994/11/29  05:23:45  darryl
 * Fix bug where XtDestroyWidget would cause a crash because
 * not all of the children were initialized to non-null values.
 *
 * Revision 0.106  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.105  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.104  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.103  1994/11/22  01:13:14  darryl
 * Button alignment.
 *
 * Revision 0.102  1994/11/21  18:21:16  darryl
 * Change instances of XtIsRealized to XtIsManaged.
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
