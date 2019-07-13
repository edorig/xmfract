
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: saveparms.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void save_parms_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void record_color_file_toggle_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void save_parms_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void save_parms_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void get_mapfile_pointer(Widget w);
static void get_mapfile_pointer_name_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
#ifdef __STDC__
static void put_parm(char *parm, ...);
#else
static void put_parm();
#endif
static void put_parm_line(void);
static int getprec(double a, double b, double c);
static void put_float(int slash, double fnum, int prec);
static void put_filename(char *keyword, char *fname);
static int endswithslash(char *fl);
char *nbasename();

/* static variables */
static char *major_method_array[] = {"breadth","depth","walk","run"};
static char *minor_method_array[] = {"left","right"};

extern char s_real[];
extern char s_imag[];
extern char s_mult[];
extern char s_sum[];
extern char s_zmag[];
extern char s_bof60[];
extern char s_bof61[];
extern char s_maxiter[];
extern char s_epscross[];
extern char s_startrail[];
extern char s_normal[];
extern char s_period[];

static Widget save_parms_dialog_shell = NULL;
static Widget save_parms_dialog = NULL;

void
saveparms(
char *filename,
char *entryname,
int write)
{
  XmString str;
  char tempstr[64];
  Widget label;

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  if(!save_parms_dialog_shell)
    {
    Widget textfield;
    Widget radio_box;
    Widget rowcol;
    Widget button;
    Widget frame = NULL;
    Arg args[MAX_ARGS];
    int fracbase;
    int btnbase;
    int i, n;

    static char *record_color_radio_box_items[] =
      {
      "Yes",
      "No",
      "Point to color map file name...",
      };

    btnbase = 20;
    fracbase = btnbase * 3 - 1;

    save_parms_dialog_shell = XmCreateDialogShell
      (main_window, "Save Parameter Options", NULL, 0);

    XtAddCallback(save_parms_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(save_parms_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Save Parameter Options");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    save_parms_dialog = XmCreateForm
      (save_parms_dialog_shell, "save_parms_dialog", args, n);

    XtAddCallback(save_parms_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    /*
     * Parameter file name
     * (label)
     */

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    label = XmCreateLabel(save_parms_dialog,
      "file_name_label", args, n);

    XtManageChild(label);

    /*
     * Parameter file entry name
     * (label)
     */

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, label); n++;
    label = XmCreateLabel(save_parms_dialog,
      "entry_name_label", args, n);

    XtManageChild(label);

    /*
     * Comments
     * (textfields)
     */

    for(i = 0; i < 4; i++)
      {
      n = 0;
      XtSetArg(args[n], XmNleftPosition, 1); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
      XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      if(i == 0)
        {
        XtSetArg(args[n], XmNtopWidget, label); n++;
        }
      else
        {
        XtSetArg(args[n], XmNtopWidget, frame); n++;
        }
      XtSetArg(args[n], XmNchildHorizontalAlignment,
        XmALIGNMENT_BEGINNING); n++;
      sprintf((char *) &tempstr, "comment_frame_%d", i);
      frame = XmCreateFrame
        (save_parms_dialog, (char *) &tempstr, args, n);

      n = 0;
      sprintf((char *) &tempstr, "Comment #%d", i + 1);
      str = XmStringCreateLocalized((char *) &tempstr);
      XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
      XtSetArg(args[n], XmNchildVerticalAlignment,
        XmALIGNMENT_WIDGET_BOTTOM); n++;
      XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
      XtSetArg(args[n], XmNlabelString, str); n++;

      sprintf((char *) &tempstr, "comment_label_%d", i);
      label = XmCreateLabel(frame, (char *) &tempstr, args, n);

      XtManageChild(label);
      XmStringFree(str);

      sprintf((char *) &tempstr, "comment_textfield_%d", i);

      n = 0;
      XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
      XtSetArg(args[n], XmNshowValue, True); n++;
      textfield = XmCreateTextField
        (frame, (char *) &tempstr, args, n);

      switch(i)
        {
        case 0:
          saveprm.comment_1_textfield = textfield;
        break;

        case 1:
          saveprm.comment_2_textfield = textfield;
        break;

        case 2:
          saveprm.comment_3_textfield = textfield;
        break;

        case 3:
        default:
          saveprm.comment_4_textfield = textfield;
        break;
        }

      XtManageChild(textfield);
      XtManageChild(frame);
      }

    /*
     * Record colors?
     * (radio box)
     *
     * Yes
     * No
     * Point to color map file name...
     */

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;

    frame = XmCreateFrame
      (save_parms_dialog, "record_color_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Record current color palette?");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "record_color_radio_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;

    rowcol = XmCreateRowColumn
      (frame, "record_color_radio_box_rc", args, n);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    radio_box = XmCreateRadioBox(rowcol,
      "record_color_radio_box", args, n);

    saveprm.record_color_yes_toggle =
      XtVaCreateManagedWidget("record_color_yes_toggle",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(record_color_radio_box_items[0]),
        XmNset, True,
        NULL);

    saveprm.record_color_no_toggle =
      XtVaCreateManagedWidget("record_color_no_toggle",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(record_color_radio_box_items[1]),
        XmNset, False,
        NULL);

    saveprm.record_color_file_toggle =
      XtVaCreateManagedWidget("record_color_file_toggle",
        xmToggleButtonWidgetClass,
        radio_box,
        XmNlabelString,
          (XtArgVal)XmStringCreateLocalized(record_color_radio_box_items[2]),
        XmNset, False,
        NULL);

    XtAddCallback(saveprm.record_color_file_toggle, XmNvalueChangedCallback,
      record_color_file_toggle_cb, NULL);

    XtManageChild(radio_box);
    XtManageChild(rowcol);
    XtManageChild(frame);

    /*
     * Help
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 0); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;

    button = XmCreatePushButton
      (save_parms_dialog, "save_parms_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      save_parms_help_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Cancel
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Cancel");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;

    button = XmCreatePushButton
      (save_parms_dialog, "save_parms_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      save_parms_cancel_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Done
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Done");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;

    button = XmCreatePushButton
      (save_parms_dialog, "save_parms_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      save_parms_done_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(save_parms_dialog,
      XmNdefaultButton,
      button,
      NULL);
    }

  switch(write)
    {
    case 1:
      sprintf((char *) &saveprm.writeflag, "1");
    break;

    case 2:
      sprintf((char *) &saveprm.writeflag, "2");
    break;

    case 0:
    default:
      sprintf((char *) &saveprm.writeflag, "0");
    break;
    }

  sprintf((char *) &saveprm.parmfilename, "%s", filename);
  sprintf((char *) &saveprm.commandname, "%s", entryname);

  sprintf((char *) &tempstr, "Parameter file name: %s", filename);
  str = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr, "*file_name_label");
  label = XtNameToWidget(save_parms_dialog, (char *) &tempstr);
  if(label != NULL)
    {
    XtVaSetValues(label, XmNlabelString, str, NULL);
    }

  XmStringFree(str);

  sprintf((char *) &tempstr, "Parameter file entry name: %s", entryname);
  str = XmStringCreateLocalized((char *) &tempstr);

  sprintf((char *) &tempstr, "*entry_name_label");
  label = XtNameToWidget(save_parms_dialog, (char *) &tempstr);
  if(label != NULL)
    {
    XtVaSetValues(label, XmNlabelString, str, NULL);
    }

  XmStringFree(str);

  XtManageChild(save_parms_dialog);
  XtManageChild(save_parms_dialog_shell);
  XtPopup(save_parms_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
static void save_parms_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  helpmode = HELPSAVEREST;
  read_help(main_window, NULL, NULL);
}

/*ARGSUSED*/
static void record_color_file_toggle_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    get_mapfile_pointer(main_window);
}

/*ARGSUSED*/
static void save_parms_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int write = atoi((char *) &(saveprm.writeflag));
  rewind(fp);
  fseek(fp,0L,SEEK_END);
  fclose(fp);

  if(write == 1)
    unlink((char *) &PAR_name);

  XtPopdown(save_parms_dialog_shell);
}

/*ARGSUSED*/
static void save_parms_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int i,j,k;
  char *comment1;
  char *comment2;
  char *comment3;
  char *comment4;
  char openflag[3];
  char msg[128];
  char *sptr;
  char buf[81];
  double Xctr, Yctr, Magnification;
  int record_color = 0;

  comment1 = XmTextFieldGetString(saveprm.comment_1_textfield);
  comment2 = XmTextFieldGetString(saveprm.comment_2_textfield);
  comment3 = XmTextFieldGetString(saveprm.comment_3_textfield);
  comment4 = XmTextFieldGetString(saveprm.comment_4_textfield);

  if(XmToggleButtonGetState(saveprm.record_color_yes_toggle) == True)
    record_color = 1;
  else if(XmToggleButtonGetState(saveprm.record_color_no_toggle) == True)
    record_color = 0;
  else if(XmToggleButtonGetState(saveprm.record_color_file_toggle) == True)
    record_color = -1;

  if((int)strcmp(saveprm.writeflag, "1") == 0)
    /* create new parmfile */
    sprintf((char *) &openflag, "%s", "wb");
  else if((int)strcmp(saveprm.writeflag, "2") == 0)
    /* append existing file */
    sprintf((char *) &openflag, "%s", "ab");
  else
    {
    sprintf((char *) &buf, "%s", "Fatal error, writeflag undecipherable.");
    confirm(main_window, (char *) &buf,
      OK_BTN, XmDIALOG_OK_BUTTON);
    exit(1);
    }

  if((fp = fopen(saveprm.parmfilename, (char *) &openflag)) == (FILE *) NULL)
    {
    sprintf((char *) &msg, "Can't open command file %s.", saveprm.parmfilename);
    warning(main_window, (char *) &msg, NULL);
    return;
    }

  fprintf(fp, "%-19s{", saveprm.commandname);

  if(comment1 && *comment1)
    fprintf(fp, " ; %s", comment1);

  fputc('\n', fp);

  memset(buf, ' ', 23);
  buf[23] = 0;
  buf[21] = ';';

  if(comment2 && *comment2)
    fprintf(fp, "%s%s\n", (char *) &buf, comment2);

  if(comment3 && *comment3)
    fprintf(fp, "%s%s\n", (char *) &buf, comment3);

  if(comment4 && *comment4)
    fprintf(fp, "%s%s\n", (char *) &buf, comment4);

  saveprm.wbdata = &saveprm.wb_data;

  /* force first parm to start on new line */
  saveprm.wb_data.len = 0;

  if (display3d <= 0)
    {
    /* a fractal was generated */
    put_parm(" reset");
    if (save_release!=0)
      put_parm("=%d",save_release);

    if (*(sptr = curfractalspecific->name) == '*')
      ++sptr;

    put_parm( " type=%s",sptr);

    if (fractype == JULIBROT || fractype == JULIBROTFP)
      {
      put_parm(" julibrotfromto=%.15g/%.15g/%.15g/%.15g",
        mxmaxfp,mxminfp,mymaxfp,myminfp);
      /* these rarely change */
      if(originfp != 8 || heightfp != 7 || widthfp != 10 || distfp != 24
                       || depthfp != 8 || zdots != 128)
        {
        put_parm(" julibrot3d=%d/%g/%g/%g/%g/%g",
          zdots, originfp, depthfp, heightfp, widthfp,distfp);
        }
      if(eyesfp != 0)
        put_parm(" julibroteyes=%g",eyesfp);
      if(neworbittype != JULIA)
        {
        char *name;
        name = fractalspecific[neworbittype].name;
        if(*name=='*')
          name++;
        put_parm(" orbitname=%s",name);
        }
      if(juli3Dmode !=0)
        put_parm(" 3Dmode=%s",juli3Doptions[juli3Dmode]);
      }

    if (fractype == FORMULA || fractype == FFORMULA)
      put_parm( " formulafile=%s formulaname=%s",
		nbasename(FormFileName),FormName);

    if (fractype == LSYSTEM)
      put_parm( " lfile=%s lname=%s",
		nbasename(LFileName),LName);

    if (fractype == IFS || fractype == IFS3D)
      put_parm( " ifsfile=%s ifs=%s",
		nbasename(IFSFileName),IFSName);

    if (fractype == INVERSEJULIA || fractype == INVERSEJULIAFP)
      {
      extern int major_method, minor_method;
      put_parm( " miim=%s/%s", major_method_array[major_method],
        minor_method_array[minor_method]);
      }

    showtrig(buf);

    if (buf[0])
      put_parm(buf);

    if (usr_stdcalcmode != 'g')
      put_parm(" passes=%c",usr_stdcalcmode);

    if (usemag && cvtcentermag(&Xctr, &Yctr, &Magnification))
      {
      put_parm(" center-mag=");
      put_parm((delmin > 1000) ? "%g/%g/%g" : "%+20.17lf/%+20.17lf/%+20.17lf",
        Xctr,Yctr,Magnification);
      }
    else
      {
      int xdigits,ydigits;
      put_parm( " corners=");
      xdigits = getprec(xxmin,xxmax,xx3rd);
      ydigits = getprec(yymin,yymax,yy3rd);
      put_float(0,xxmin,xdigits);
      put_float(1,xxmax,xdigits);
      put_float(1,yymin,ydigits);
      put_float(1,yymax,ydigits);
      if (xx3rd != xxmin || yy3rd != yymin)
        {
        put_float(1,xx3rd,xdigits);
        put_float(1,yy3rd,ydigits);
        }
      }

    for (i = (MAXPARAMS-1); i >= 0; --i)
      if (param[i] != 0.0)
        break;
    if (i >= 0)
      {
      if (fractype == CELLULAR)
        put_parm(" params=%.1f",param[0]);
      else
        put_parm(" params=%lf",(double)param[0]);
      for (j = 1; j <= i; ++j)
        if (fractype == CELLULAR)
          put_parm("/%.1f",param[j]);
        else
          put_parm("/%lf",(double)param[j]);
      }

    if(useinitorbit == 2)
      put_parm( " initorbit=pixel");
    else if(useinitorbit == 1)
      put_parm( " initorbit=%.15g/%.15g",initorbit.x,initorbit.y);

    if (floatflag == '1')
      put_parm( " float=y");

    if (maxit != 150)
      put_parm(" maxiter=%d",maxit);

    if(bailout && (potflag == 0 || potparam[2] == 0.0))
      put_parm( " bailout=%d",bailout);

    if(fillcolor != -1)
      {
      put_parm(" fillcolor=");
      put_parm( "%d",fillcolor);
      }

    if (inside != 1)
      {
      put_parm(" inside=");
      if (inside == -1)
        put_parm( s_maxiter);
      else if (inside == -59)
        put_parm(s_zmag);
      else if (inside == -60)
        put_parm(s_bof60);
      else if (inside == -61)
        put_parm(s_bof61);
      else if (inside == -100)
        put_parm(s_epscross);
      else if (inside == -101)
        put_parm(s_startrail);
      else if (inside == -102)
        put_parm(s_period);
      else
        put_parm( "%d",inside);
      }

    if (outside != -1)
      {
      put_parm(" outside=");
      if (outside == -2)
        put_parm(s_real);
      else if (outside == -3)
        put_parm(s_imag);
      else if (outside == -4)
        put_parm(s_mult);
      else if (outside == -5)
        put_parm(s_sum);
      else
        put_parm( "%d",outside);
      }

    if(LogFlag)
      {
      put_parm( " logmap=");
      if(LogFlag == -1)
        put_parm( "old");
      else if(LogFlag == 1)
        put_parm( "yes");
      else
        put_parm( "%d", LogFlag);
      }

    if (potflag)
      {
      put_parm( " potential=%d/%g/%d",
        (int)potparam[0],potparam[1],(int)potparam[2]);
      if(pot16bit)
        put_parm( "/16bit");
      }

    if (invert)
      put_parm( " invert=%g/%g/%g",
        inversion[0], inversion[1], inversion[2]);

    if (decomp[0])
      put_parm( " decomp=%d", decomp[0]);

    if ((distest = usr_distest) != 0)
      put_parm( " distest=%d/%d", distest, distestwidth);

    if (usr_biomorph != -1)
      put_parm( " biomorph=%d", usr_biomorph);

    if (finattract)
      put_parm(" finattract=y");

    if (forcesymmetry != 999)
      {
      if(forcesymmetry == 1000)
        {
        inform(main_window, (char *) &msg, NULL);
        sprintf((char *) &msg,
          "Regenerate before save to get correct symmetry");
        }

      put_parm( " symmetry=");

      if (forcesymmetry==XAXIS)
        put_parm("xaxis");
      else if(forcesymmetry==YAXIS)
        put_parm("yaxis");
      else if(forcesymmetry==XYAXIS)
        put_parm("xyaxis");
      else if(forcesymmetry==ORIGIN)
        put_parm("origin");
      else if(forcesymmetry==PI_SYM)
        put_parm("pi");
      else
        put_parm("none");
      }

    if (periodicitycheck != 1)
      put_parm( " periodicity=%d",periodicitycheck);

    if (rflag)
      put_parm( " rseed=%d",rseed);

    if (rangeslen)
      {
      put_parm(" ranges=");
      i = 0;
      while (i < rangeslen)
        {
        if (i)
          put_parm("/");
        if (ranges[i] == -1)
          {
          put_parm("-%d/",ranges[++i]);
          ++i;
          }
        put_parm("%d",ranges[i++]);
        }
      }
    }

  if(display3d >= 1)
    {
    if(display3d == 2)
      put_parm( " 3d=overlay");
    else
      put_parm( " 3d=yes");

    if (loaded3d == 0)
      put_filename("filename",readname);

    if (SPHERE)
      {
      put_parm( " sphere=y");
      put_parm( " latitude=%d/%d", THETA1, THETA2);
      put_parm( " longitude=%d/%d", PHI1, PHI2);
      put_parm( " radius=%d", RADIUS);
      }

    put_parm( " scalexyz=%d/%d", XSCALE, YSCALE);
    put_parm( " roughness=%d", ROUGH);
    put_parm( " waterline=%d", WATERLINE);

    if (FILLTYPE)
      put_parm( " filltype=%d", FILLTYPE);

    if (transparent[0] || transparent[1])
      put_parm( " transparent=%d/%d", transparent[0],transparent[1]);

    if (preview)
      {
      put_parm( " preview=y");
      if (showbox)
        put_parm( " showbox=y");
      put_parm( " coarse=%d",previewfactor);
      }

    if (RAY)
      {
      put_parm( " ray=%d",RAY);
      if (BRIEF)
        put_parm(" brief=y");
      }

    if (FILLTYPE > 4)
      {
      put_parm( " lightsource=%d/%d/%d", XLIGHT, YLIGHT, ZLIGHT);
      if (LIGHTAVG)
        put_parm( " smoothing=%d", LIGHTAVG);
      }

    if (RANDOMIZE)
      put_parm( " randomize=%d",RANDOMIZE);

    if (Targa_Out)
      put_parm( " fullcolor=y");

    if (Ambient)
      put_parm( " ambient=%d",Ambient);

    if (haze)
      put_parm( " haze=%d",haze);
    }

  if (display3d)
    {
    /* universal 3d */
    if (!SPHERE || display3d < 0)
      put_parm( " rotation=%d/%d/%d", XROT, YROT, ZROT);

    put_parm( " perspective=%d", ZVIEWER);
    put_parm( " xyshift=%d/%d", XSHIFT, YSHIFT);

    if(xtrans || ytrans)
      put_parm( " xyadjust=%d/%d",xtrans,ytrans);

    if(glassestype)
      {
      put_parm( " stereo=%d",glassestype);
      put_parm( " interocular=%d",eyeseparation);
      put_parm( " converge=%d",xadjust);
      put_parm( " crop=%d/%d/%d/%d",
        red_crop_left,red_crop_right,blue_crop_left,blue_crop_right);
      put_parm( " bright=%d/%d",
        red_bright,blue_bright);
      }
    }

  if(viewwindow == 1)
    {
    put_parm(" viewwindows=%g/%g",viewreduction,finalaspectratio);

    if(viewcrop)
      put_parm("/yes");
    else
      put_parm("/no");

    put_parm("/%d/%d",viewxdots,viewydots);
    }

  if (record_color)
    {
    put_parm(" colors=");
    if (record_color == -1 && saveprm.mapfile_pointer_name[0] == '@')
      put_parm(saveprm.mapfile_pointer_name);
    else
      {
      int curc,scanc,force,diffmag;
      int delta,diff1[4][3],diff2[4][3];
      curc = force = diffmag = 0;
      memcpy(olddacbox,dacbox,768);
      while (1)
        {
        /* emit color in rgb 3 char encoded form */
        for (j = 0; j < 3; ++j)
          {
          dacbox[curc][j] = dacbox[curc][j] >> 2;
          if ((k = dacbox[curc][j]) < 10)
            k += '0';
          else if (k < 36)
            k += ('A' - 10);
          else
            k += ('_' - 36);
          buf[j] = k;
          }
        buf[3] = 0;
        put_parm(buf);
        if (++curc >= maxcolor)  /* quit if done last color */
           break;
/*
 * Next a P Branderhorst special, a tricky scan for smooth-shaded
 * ranges which can be written as <nn> to compress .par file entry.
 * Method used is to check net change in each color value over
 * spans of 2 to 5 color numbers.  First time for each span size
 * the value change is noted.  After first time the change is
 * checked against noted change.  First time it differs, a
 * a difference of 1 is tolerated and noted as an alternate
 * acceptable change.  When change is not one of the tolerated
 * values, loop exits.
 */
        if (force)
          {
          --force;
          continue;
          }

        scanc = curc;
        while (scanc < maxcolor)
          {
          /* scan while same diff to next */
          if ((i = scanc - curc) > 3) /* check spans up to 4 steps */
            i = 3;
          for (k = 0; k <= i; ++k)
            {
            for (j = 0; j < 3; ++j)
              { /* check pattern of chg per color */
              delta = (int)dacbox[scanc][j] - (int)dacbox[scanc-k-1][j];
              if (k == scanc - curc)
                diff1[k][j] = diff2[k][j] = delta;
              else
                {
                if (delta != diff1[k][j] && delta != diff2[k][j])
                  {
                  diffmag = abs(delta - diff1[k][j]);
                    if (diff1[k][j] != diff2[k][j] || diffmag != 1)
                  break;
                  diff2[k][j] = delta;
                  }
                }
              }
            if (j < 3) break; /* must've exited from inner loop above */
            }
          if (k <= i) break;   /* must've exited from inner loop above */
            ++scanc;
          }
        /* now scanc-1 is next color which must be written explicitly */
        if (scanc - curc > 2)
          {
          /* good, we have a shaded range */
          if (scanc != maxcolor)
            {
            if (diffmag < 3)
              {
              /* not a sharp slope change? */
              force = 2;       /* force more between ranges, to stop  */
              --scanc;         /* "drift" when load/store/load/store/ */
              }
            if (k)
              {
              /* more of the same */
              force += k;
              --scanc;
              }
            }
          if (--scanc - curc > 1)
            {
            put_parm("<%d>",scanc-curc);
            curc = scanc;
            }
          else
            {
            /* changed our mind */
            force = 0;
            }
          }
        }
      memcpy(dacbox,olddacbox,768);
      writevideopalette();
      }
    }

  if (rotate_lo != 0 || rotate_hi != (colors - 1))
    {
    put_parm( " cyclerange=%d/%d",rotate_lo,rotate_hi);
    }

  while (saveprm.wbdata->len)
    {
    put_parm_line();
    }

  fputs("  }", fp);
  fputc('\n', fp);

  fclose(fp);

  XtPopdown(save_parms_dialog_shell);

  if(verbose)
    {
    sprintf((char *) &msg,
      "Parameters saved in the parameter file\n\n\"%s\"\n\nas an entry named \"%s\"",
       saveprm.parmfilename, saveprm.commandname);
    inform(main_window, (char *) &msg, NULL);
    }
}

static void get_mapfile_pointer(Widget w)
{
  static Widget mapfile_pointer_fsb = NULL;

  if(!mapfile_pointer_fsb)
    {
    Arg args[MAX_ARGS];
    XmString str[2];
    XmString dirstr = XmStringCreateLocalized(fractdir);
    int n = 0;

    str[0] = XmStringCreateLocalized("Color Map File to Reference");
    str[1] = XmStringCreateLocalized("*.map");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNmustMatch, True); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNdialogTitle, str[0]); n++;
    XtSetArg(args[n], XmNdirectory, dirstr); n++;
    XtSetArg(args[n], XmNpattern, str[1]); n++;

    mapfile_pointer_fsb = XmCreateFileSelectionDialog(XtParent(w),
      "mapfile_pointer_fsb", args, n);

    XtAddCallback(mapfile_pointer_fsb, XmNokCallback,
      get_mapfile_pointer_name_cb, NULL);

    XtAddCallback(mapfile_pointer_fsb, XmNcancelCallback,
      load_map_cancel_cb, NULL);

    XtAddCallback(mapfile_pointer_fsb, XmNnoMatchCallback,
      fsb_no_match, NULL);

    XtAddCallback(mapfile_pointer_fsb, XmNmapCallback,
      map_dialog, NULL);

    XtAddCallback(XtParent(mapfile_pointer_fsb), XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(XtParent(mapfile_pointer_fsb), (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    XmStringFree(str[0]);
    XmStringFree(str[1]);
    XmStringFree(dirstr);
    }
  XtManageChild(mapfile_pointer_fsb);
  XtPopup(XtParent(mapfile_pointer_fsb), XtGrabNone);
}

/*ARGSUSED*/
static void get_mapfile_pointer_name_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  char *filename;

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);

  sprintf((char *) &saveprm.mapfile_pointer_name, "@%s", nbasename(filename));

  XtPopdown(XtParent(w));
}

#ifdef __STDC__
static void put_parm(char *parm, ...)
#else
static void put_parm(parm, va_alist)
     char *parm;
     va_dcl
#endif
{
  char *bufptr;
  va_list args;


  if (*parm == ' ' && saveprm.wbdata->len == 0)
    ++parm;

  bufptr = saveprm.wbdata->buf + saveprm.wbdata->len;

  VA_START(args, parm);
  vsprintf(bufptr,parm,args);
  va_end(args);

  while (*(bufptr++))
    ++saveprm.wbdata->len;

  while (saveprm.wbdata->len > 200)
    put_parm_line();
}

static void put_parm_line()
{
  int len,c;
  if ((len = saveprm.wbdata->len) > NICELINELEN)
    {
    len = NICELINELEN+1;
    while (--len != 0 && saveprm.wbdata->buf[len] != ' ')
      {
      /* just decrement len */
      }
    if (len == 0)
      {
      len = NICELINELEN-1;
      while (++len < MAXLINELEN
         && saveprm.wbdata->buf[len] && saveprm.wbdata->buf[len] != ' ')
        {
        /* just increment len */
        }
      }
    }
  c = saveprm.wbdata->buf[len];
  saveprm.wbdata->buf[len] = 0;
  fputs("  ",fp);
  fputs(saveprm.wbdata->buf,fp);
  if (c && c != ' ')
    fputc('\\',fp);
  fputc('\n',fp);
  if ((saveprm.wbdata->buf[len] = c) == ' ')
    ++len;
  saveprm.wbdata->len -= len;
  strcpy(saveprm.wbdata->buf,saveprm.wbdata->buf+len);
}

static int getprec(double a,double b,double c)
{
  double diff,temp;
  int digits;
  double highv = 1.0E20;
  if ((diff = fabs(a - b)) == 0.0)
    diff = highv;
  if ((temp = fabs(a - c)) == 0.0)
    temp = highv;
  if (temp < diff)
    diff = temp;
  if ((temp = fabs(b - c)) == 0.0)
    temp = highv;
  if (temp < diff)
    diff = temp;
  digits = 7;
  if(debugflag >= 700 && debugflag < 720 )
    digits =  debugflag - 700;
  while (diff < 1.0 && digits < 17)
    {
    diff *= 10;
    ++digits;
    }
  return(digits);
}

static void put_float(int slash,double fnum,int prec)
{
  char buf[40];
  char *bptr, *dptr;

  bptr = buf;

  if (slash)
    *(bptr++) = '/';

  sprintf(bptr,"%1.*g",prec,(double)fnum);

  if ((dptr = strchr(bptr,'.')))
    {
    ++dptr;
    bptr = buf + strlen(buf);
    while (--bptr > dptr && *bptr == '0')
    *bptr = 0;
    }

  put_parm(buf);
}

static void put_filename(char *keyword,char *fname)
{
  char *p;
  if (*fname && !endswithslash(fname))
    {
    if ((p = strrchr(fname, SLASHC)))
      if (*(fname = p+1) == 0) return;
        put_parm(" %s=%s",keyword,fname);
    }
}

static int endswithslash(char *fl)
{
  int len;
  len = strlen(fl);
  if(len)
    if(fl[--len] == SLASHC)
      return(1);
  return(0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: saveparms.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.13  1995/06/07  19:20:09  darryl
 * Fix problem where the MIN macro was not guarded from
 * previous declarations and that the palette randomization
 * Done and Apply buttons were handled incorrectly.
 *
 * Revision 1.12  1995/06/05  16:30:14  tom
 * File handling enhancements. Remove DOS-specific code.
 *
 * Revision 1.10.1.1  1995/06/05  14:10:19  tom
 * changed filename handling
 *
 * Revision 1.10  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.9  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.8  1995/04/27  17:53:47  darryl
 * Change basename to nbasename, change %x to %d in client_data
 * translations, delete unused header file inclusions.
 *
 * Revision 1.7  1995/04/18  15:59:59  darryl
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
 * Revision 1.6  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.5  1995/03/20  19:09:05  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.2  1995/03/14  15:05:27  tom
 * changed #if to #ifdef
 *
 * Revision 1.1.1.1  1995/03/11  03:23:45  tom
 * changed varargs handling
 * changed basename prototype
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.301  1994/12/21  21:47:52  darryl
 * Add verbosity when saving parameters.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.105  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.104  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
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
