
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: image_info.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void current_image_info_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* static variables */
static Widget current_image_info_dialog_shell = NULL;
static Widget current_image_info_dialog = NULL;

/*ARGSUSED*/
void create_current_image_info_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmString str;
  char tempstr[256];
  int i, n;
  int labelbase;
  int fracbase;
  int numfn;
  int trigdone;
  char tmpbuf[20];
  char tmpbuf2[60];
  double Xctr, Yctr, Magnification;
  Widget rowcol;
  Widget frame;
  Widget button;
  Arg args[MAX_ARGS];

  int numlabels = 56;

  Widget label[] =
    {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL
    };

  static char sfractal_type[] =        {"Fractal type:"};
  static char s3D_transform[] =        {"3D Transform"};
  static char sformula_name[] =        {"Formula name:"};
  static char sformulas[] =            {"Formulas:"};
  static char slsystem_name[] =        {"L-System name:"};
  static char sifs_name[] =            {"IFS name:"};
  static char syou_are_cycling[] =     {"You are in color-cycling mode"};
  static char sruns_forever[] =        {"Note: this type runs forever."};
  static char ssolid_guessing[] =      {"Solid guessing"};
  static char sboundary_tracing[] =    {"Boundary tracing"};
  static char stesseral[] =            {"Tesseral"};
  static char scalculation_time[] =    {"Calculation time:"};
  static char scalculation_mode[] =    {"Calculation mode:"};
  static char scornersxy[] =           {"Corners:                  X                     Y"};
  static char stop_left[] =            {"top-left"};
  static char sbottom_right[] =        {"bottom-right"};
  static char scenter[] =              {"center:"};
  static char smag[] =                 {"mag:"};
  static char sbottom_left[] =         {"bottom-left"};
  static char sparams[] =              {"Parameters:"};
  static char siteration_maximum[] =   {"Iteration maximum: "};
  static char seffective_bailout[] =   {"Effective bailout: "};
  static char scurrent_rseed[] =       {"Current 'rseed': "};
  static char sinversion_radius[] =    {"Inversion radius: "};
  static char sxcenter[] =             {"xcenter:"};
  static char sycenter[] =             {"ycenter:"};
  static char scalcstatus[] =          {"Status:"};
  static char sparms_chgd[] =          {"parms changed since generated"};
  static char sstill_being[] =         {"still being generated"};
  static char sinter_resumable[] =     {"interrupted, resumable"};
  static char sinter_non_resumable[] = {"interrupted, non-resumable"};
  static char simage_completed[] =     {"image completed"};
  static char snone[] =                {"unknown"};
  static char snoninterruptable[] =    {"Note: this type cannot be resumed."};

  labelbase = 30;
  fracbase = labelbase * 3 - 1;

  trigdone = 0;

  if(current_image_info_dialog_shell != NULL)
    current_image_info_done_btn_cb(NULL, NULL, NULL);

  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  current_image_info_dialog_shell = XmCreateDialogShell
    (main_window, "Information About Current Image", NULL, 0);

  XtAddCallback(current_image_info_dialog_shell, XmNpopupCallback,
    map_dialog, NULL);

#ifdef EDITRES_ENABLE
  XtAddEventHandler(current_image_info_dialog_shell, (EventMask) 0,
    True, _XEditResCheckMessages, NULL);
#endif

  n = 0;
  str = XmStringCreateLocalized("Info About Current Image");
  XtSetArg(args[n], XmNfractionBase, fracbase); n++;
  XtSetArg(args[n], XmNnoResize, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNchildHorizontalAlignment,
    XmALIGNMENT_BEGINNING); n++;
  XtSetArg(args[n], XmNdialogTitle, str); n++;

  current_image_info_dialog = XmCreateForm
    (current_image_info_dialog_shell, "current_image_info_dialog", args, n);

  XtAddCallback(current_image_info_dialog, XmNmapCallback,
    map_dialog, NULL);

  XmStringFree(str);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;

  frame = XmCreateFrame
    (current_image_info_dialog, "current_image_info_frame", args, n);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  XtSetArg(args[n], XmNchildHorizontalAlignment,
    XmALIGNMENT_BEGINNING); n++;

  rowcol = XmCreateRowColumn(frame, "current_image_info_rowcol", args, n);

  n = 0;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
  for(i = 0; i < numlabels; i++)
    {
    sprintf((char *) &tempstr, "label_%d", i);
    label[i] = XmCreateLabel(rowcol, (char *) &tempstr, args, n);
    }

  n = 0;
  str = XmStringCreateLocalized("Done");
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

  button = XmCreatePushButton
    (current_image_info_dialog, "current_image_info_done_btn", args, n);

  XtAddCallback(button, XmNactivateCallback,
    current_image_info_done_btn_cb, NULL);

  XmStringFree(str);

  n = 0;

  /* fractal type */

  sprintf((char *) &tempstr, "%s %s", (char *) &sfractal_type,
    display3d > 0 ? (char *) &s3D_transform :
    curfractalspecific->name[0] == '*' ? &curfractalspecific->name[1] :
    curfractalspecific->name);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  /* formula name */

  if(fractype == FORMULA || fractype == FFORMULA)
    {
    sprintf((char *) &tempstr, "%s %s",
      (char *) &sformula_name, (char *) &FormName);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;

   /* formulas */

    numfn = maxfn;

    if (numfn > 0)
      {
      strcpy((char *) &tmpbuf2,trigfn[trigndx[0]].name);
      i = 0;
      while(++i < numfn)
        {
        sprintf(tmpbuf,"/%s",trigfn[trigndx[i]].name);
        strcat((char *) &tmpbuf2,tmpbuf);
        }

      sprintf((char *) &tempstr, "%s %s",
        (char *) &sformulas, (char *) &tmpbuf2);

      str = XmStringCreateLocalized((char *) &tempstr);
      XtVaSetValues(label[n],
        XmNlabelString, str,
        NULL);
      XmStringFree(str);
      trigdone = 1;
      n++;
      }
    }

  /* L-system name */

  if(fractype == LSYSTEM)
    {
    sprintf((char *) &tempstr, "%s %s",
      (char *) &slsystem_name, (char *) &LName);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  /* IFS parameter entry name */

  if(fractype == IFS || fractype == IFS3D)
    {
    sprintf((char *) &tempstr, "%s %s",
      (char *) &sifs_name, (char *) &IFSName);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  if(!trigdone)
    {
    if(fractype==JULIBROT || fractype==JULIBROTFP)
      numfn = (fractalspecific[neworbittype].flags >> 6) & 7;
    else
      numfn = (curfractalspecific->flags >> 6) & 7;

    if (numfn > 0)
      {
      strcpy((char *) &tmpbuf2,trigfn[trigndx[0]].name);
      i = 0;
      while(++i < numfn)
        {
        sprintf(tmpbuf,"/%s",trigfn[trigndx[i]].name);
        strcat((char *) &tmpbuf2,tmpbuf);
        }

      sprintf((char *) &tempstr, "%s %s",
        (char *) &sformulas, (char *) &tmpbuf2);

      str = XmStringCreateLocalized((char *) &tempstr);
      XtVaSetValues(label[n],
        XmNlabelString, str,
        NULL);
      XmStringFree(str);
      n++;
      }
    trigdone = 1;
    }

  /* calculation status */

  switch (calc_status)
    {
    case IMG_PRMS_CHGD:
      sprintf((char *) &tmpbuf2, "%s", (char *) &sparms_chgd);
    break;

    case IMG_ACTV_CALC:
      sprintf((char *) &tmpbuf2, "%s", (char *) &sstill_being);
    break;

    case IMG_INTR_RESM:
      sprintf((char *) &tmpbuf2, "%s", (char *) &sinter_resumable);
    break;

    case IMG_INTR_NRES:
      sprintf((char *) &tmpbuf2, "%s", (char *) &sinter_non_resumable);
    break;

    case IMG_COMPLETED:
      sprintf((char *) &tmpbuf2, "%s", (char *) &simage_completed);
    break;

    default:
      sprintf((char *) &tmpbuf2, "%s", (char *) &snone);
    break;
    }

  sprintf((char *) &tempstr, "%s %s",
    (char *) &scalcstatus, (char *) &tmpbuf2);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  /* color cycling */

  if(rotate_stuff.running == 1)
    {
    str = XmStringCreateLocalized((char *) &syou_are_cycling);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  /* type runs forever */

  if(curfractalspecific->flags&INFCALC)
    {
    str = XmStringCreateLocalized((char *) &sruns_forever);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  /* non-interruptable */

  if((calc_status == IMG_ACTV_CALC || calc_status == IMG_INTR_RESM) &&
      (curfractalspecific->flags&NORESUME))
    {
    str = XmStringCreateLocalized((char *) &snoninterruptable);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  /* calculation progress */

  if(got_status >= 0 &&
    (calc_status == IMG_ACTV_CALC || calc_status == IMG_INTR_RESM))
    {
    switch (got_status)
      {
      case 0:
        sprintf((char *) &tmpbuf2,"%d Pass Mode",totpasses);
        break;
      case 1:
        sprintf((char *) &tmpbuf2,"%s", (char *) &ssolid_guessing);
        break;
      case 2:
        sprintf((char *) &tmpbuf2,"%s", (char *) &sboundary_tracing);
        break;
      case 3:
        sprintf((char *) &tmpbuf2,"Processing row %d (of %d) of input image",
          currow,fileydots);
        break;
      case 4:
        sprintf((char *) &tmpbuf2,"%s", (char *) &stesseral);
        break;
      }

    sprintf((char *) &tempstr,"%s %s",
      (char *) &scalculation_mode, (char *) &tmpbuf2);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;

    if (got_status != 3)
      {
      sprintf((char *) &tempstr,"Working on block (y,x) [%d,%d]...[%d,%d], ",
        yystart, xxstart, yystop, xxstop);

      str = XmStringCreateLocalized((char *) &tempstr);

      XtVaSetValues(label[n],
        XmNlabelString, str,
        NULL);
      XmStringFree(str);
      n++;

      if (got_status == 2 || got_status == 4)
        {
        sprintf((char *) &tempstr, "at (row,col) [%d,%d]",currow,curcol);
        str = XmStringCreateLocalized((char *) &tempstr);
        }
      else
        {
        sprintf((char *) &tempstr, "pass %d of %d at row %d",
          curpass, totpasses, currow);
        str = XmStringCreateLocalized((char *) &tempstr);
        }

      XtVaSetValues(label[n],
        XmNlabelString, str,
        NULL);
      XmStringFree(str);
      n++;
      }
    }

  /* calculation time */

  sprintf((char *) &tempstr,"%s %03ld:%02ld:%02ld.%02ld",
    (char *) &scalculation_time, calctime/360000,
    (calctime%360000)/6000, (calctime%6000)/100, calctime%100);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  /* video mode */

  sprintf((char *) &tempstr,"Video: %dx%dx%d %s %s",
          videotable[0].xdots, videotable[0].ydots, videotable[0].colors,
          videotable[0].name, videotable[0].comment);
  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  /* corner values */

  str = XmStringCreateLocalized((char *) &scornersxy);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "  %s     %+20.16f  %+20.16f",
     (char *) &stop_left, xxmin, yymax);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "  %s %+20.16f  %+20.16f",
     (char *) &sbottom_right, xxmax, yymin);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  /* make bottom left exact if very near exact */
  adjust_corner();

  if (cvtcentermag(&Xctr, &Yctr, &Magnification))
    {
    sprintf((char *) &tempstr, "  %s      %+20.16f  %+20.16f",
      (char *) &scenter, Xctr, Yctr);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;

    sprintf((char *) &tempstr, "  %s         %+20.16f",
      (char *) &smag, Magnification);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }
  else if(xxmin != xx3rd || yymin != yy3rd)
    {
    sprintf((char *) &tempstr, "  %s  %+20.16f  %+20.16f",
      (char *) &sbottom_left, xx3rd, yy3rd);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  str = XmStringCreateLocalized((char *) &sparams);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "  %3d: %+12.9f", 1, param[0]);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "  %3d: %+12.9f", 2, param[1]);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "  %3d: %+12.9f", 3, param[2]);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "  %3d: %+12.9f", 4, param[3]);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "%s %d", (char *) &siteration_maximum, maxit);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  sprintf((char *) &tempstr, "%s %+.16f", (char *) &seffective_bailout, rqlim);

  str = XmStringCreateLocalized((char *) &tempstr);
  XtVaSetValues(label[n],
    XmNlabelString, str,
    NULL);
  XmStringFree(str);
  n++;

  if(fractype == PLASMA)
    {
    sprintf((char *) &tempstr, "%s %d", (char *) &scurrent_rseed, rseed);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  if(invert)
    {
    sprintf((char *) &tempstr,
      "%s %+12.9f", (char *) &sinversion_radius, f_radius);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;

    sprintf((char *) &tempstr, "%s %+12.9f", (char *) &sxcenter, f_xcenter);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;

    sprintf((char *) &tempstr, "%s %+12.9f", (char *) &sycenter, f_ycenter);

    str = XmStringCreateLocalized((char *) &tempstr);
    XtVaSetValues(label[n],
      XmNlabelString, str,
      NULL);
    XmStringFree(str);
    n++;
    }

  for(i = 0; i < n; i++)
    XtManageChild(label[i]);

  XtManageChild(rowcol);
  XtManageChild(frame);
  XtManageChild(button);

  XtVaSetValues(current_image_info_dialog,
    XmNdefaultButton,
    button,
    NULL);

  XtManageChild(current_image_info_dialog);
  XtManageChild(current_image_info_dialog_shell);
  XtPopup(current_image_info_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

/*ARGSUSED*/
void current_image_info_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(current_image_info_dialog_shell);
  XtDestroyWidget(current_image_info_dialog_shell);
  current_image_info_dialog_shell = NULL;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: image_info.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.7  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.6  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
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
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.301  1994/12/27  15:53:30  darryl
 * Fix bug where formulas were not displayed correctly.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.103  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
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
