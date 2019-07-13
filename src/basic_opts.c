
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: basic_opts.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void toggle_passes_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_inside_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void toggle_outside_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void iteration_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void log_palette_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void biomorph_color_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void decomp_option_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void fill_color_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void orbit_delay_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void inside_color_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void outside_color_scale_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void basic_dialog_reset_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void basic_dialog_defaults_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void basic_dialog_done_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void basic_dialog_cancel_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void basic_dialog_help_btn_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
static void set_basic_opts_values(void);

/* static variables */
static Widget basic_options_dialog_shell = NULL;
static Widget basic_options_dialog = NULL;
static int oldhelpmode;

/*ARGSUSED*/
void create_basic_options_dialog(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XDefineCursor(display, window, XCreateFontCursor(display, XC_watch));

  oldhelpmode = helpmode;

  helpmode = HELPXOPTS;

  if(!basic_options_dialog_shell)
    {
    Widget frame;
    Widget rowcol;
    Widget scale;
    Widget radio_box;
    Widget label;
    Widget toggle;
    Widget button;
    XmString str;
    Arg args[MAX_ARGS];
    int fracbase;
    int btnbase;
    int n;

    btnbase = 20;
    fracbase = btnbase * 6 - 1;

    basic_options_dialog_shell = XmCreateDialogShell
      (main_window, "Basic Options", NULL, 0);

    XtAddCallback(basic_options_dialog_shell, XmNpopupCallback,
      map_dialog, NULL);

#ifdef EDITRES_ENABLE
    XtAddEventHandler(basic_options_dialog_shell, (EventMask) 0,
      True, _XEditResCheckMessages, NULL);
#endif

    n = 0;
    str = XmStringCreateLocalized("Basic Options");
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNhorizontalSpacing, 3); n++;
    XtSetArg(args[n], XmNverticalSpacing, 3); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNfractionBase, fracbase); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    basic_options_dialog = XmCreateForm
      (basic_options_dialog_shell, "basic_options_dialog", args, n);

    XtAddCallback(basic_options_dialog, XmNmapCallback,
      map_dialog, NULL);

    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNnumColumns, 3); n++;

    rowcol = XmCreateRowColumn
      (basic_options_dialog, "radio_box_rowcol", args, n);

    /*
     * Calculation Method
     * (radio buttons)
     * One pass mode
     * Two passes mode
     * Solid guessing mode
     * Boundary trace mode
     * Tessaral mode
     *
     */

    n = 0;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (rowcol, "passes_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Calculation Method");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "passes_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;

    radio_box = XmCreateRadioBox
      (frame, "passes_radio_box", args, n);

    XtManageChild(radio_box);

    n = 0;
    str = XmStringCreateLocalized("One pass mode");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "one_pass_toggle", args, n);

    basic_opts.one_pass_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_passes_cb, "1");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Two passes mode");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "two_pass_toggle", args, n);

    basic_opts.two_pass_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
       toggle_passes_cb, "2");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Solid guessing mode");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "solid_guess_toggle", args, n);

    basic_opts.guess_pass_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_passes_cb, "g");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Boundary trace mode");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "boundary_trace_toggle", args, n);

    basic_opts.boundary_trace_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_passes_cb, "b");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Tesseral mode");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "tesseral_mode_toggle", args, n);

    basic_opts.tessaral_pass_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_passes_cb, "t");

    XtManageChild(toggle);
    XmStringFree(str);

    XtManageChild(frame);

    /*
     * Inside color
     * (radio buttons)
     * Maxiter
     * Magnitude of Z
     * Beauty of Fractals, p. 60
     * Beauty of Fractals, p. 61
     * Epsilon Cross
     * Star Trail
     * Period
     * Enter a color (below)
     */

    n = 0;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (rowcol, "inside_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Inside Color");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "inside_box_label", args, n);

    XtManageChild(label);
    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;

    radio_box = XmCreateRadioBox
      (frame, "inside_radio_box", args, n);

    XtManageChild(radio_box);

    n = 0;
    str = XmStringCreateLocalized("Maxiter");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_maxiter_toggle", args, n);

    basic_opts.maxiter_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "-1");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Magnitude of Z");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_zmag_toggle", args, n);

    basic_opts.zmag_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "-59");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Beauty of Fractals, p. 60");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_bof60_toggle", args, n);

    basic_opts.b_of_sixty_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "-60");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Beauty of Fractals, p. 61");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_bof61_toggle", args, n);

    basic_opts.b_of_sixty_one_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "-61");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Epsilon Cross");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_epscr_toggle", args, n);

    basic_opts.epscr_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "-100");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Star Trail");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_star_toggle", args, n);

    basic_opts.star_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "-101");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Period");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_per_toggle", args, n);

    basic_opts.per_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "-102");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Enter a color (below)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "inside_nnn_toggle", args, n);

    basic_opts.inside_nnn_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_inside_cb, "0");

    XtManageChild(toggle);
    XmStringFree(str);

    XtManageChild(frame);

    /*
     * Outside color
     * (radio buttons)
     * Iterations
     * Iter + Real
     * Iter + Imaginary
     * Iter x Real / Imaginary
     * Sum of all of the above
     * Enter a color (below)
     */

    n = 0;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    frame = XmCreateFrame
      (rowcol, "outside_radio_box_frame", args, n);

    n = 0;
    str = XmStringCreateLocalized("Outside Color");
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment,
      XmALIGNMENT_WIDGET_BOTTOM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;

    label = XmCreateLabel
      (frame, "outside_box_label", args, n);

    XtManageChild(label);

    XmStringFree(str);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;

    radio_box = XmCreateRadioBox
      (frame, "outside_radio_box", args, n);

    XtManageChild(radio_box);

    n = 0;
    str = XmStringCreateLocalized("Iterations");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "outside_iter_toggle", args, n);

    basic_opts.iter_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_outside_cb, "-1");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Iter + Real");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "outside_real_toggle", args, n);

    basic_opts.real_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_outside_cb, "-2");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Iter + Imaginary");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "outside_imag_toggle", args, n);

    basic_opts.imag_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_outside_cb, "-3");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Iter x Real / Imaginary");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "outside_mult_toggle", args, n);

    basic_opts.mult_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_outside_cb, "-4");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Sum of all of the above");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "outside_summ_toggle", args, n);

    basic_opts.summ_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_outside_cb, "-5");

    XtManageChild(toggle);
    XmStringFree(str);

    n = 0;
    str = XmStringCreateLocalized("Enter a color (below)");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;

    toggle = XmCreateToggleButton
      (radio_box, "outside_maxiter_toggle", args, n);

    basic_opts.outside_nnn_toggle = toggle;

    XtAddCallback(toggle, XmNvalueChangedCallback,
      toggle_outside_cb, "0");

    XtManageChild(toggle);
    XmStringFree(str);

    XtManageChild(frame);

    XtManageChild(rowcol);

    n = 0;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment,
      XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNisHomogeneous, False); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;

    rowcol = XmCreateRowColumn
      (basic_options_dialog, "scale_rowcol", args, n);

    /*
     * Specific Inside Color
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Specific Inside Color");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "inside_color_scale", args, n);

    basic_opts.inside_nnn_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      inside_color_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      inside_color_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    /*
     * Calculation Iterations
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Calculation Iterations");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNmaximum, 32767); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "iteration_scale", args, n);

    basic_opts.iteration_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      iteration_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      iteration_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    /*
     * Log palette
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Log palette (0=no,1=yes,-1=old,+n=cmprsd,-n=sqrt)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, -255); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "log_palette_scale", args, n);

    basic_opts.log_palette_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      log_palette_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      log_palette_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    /*
     * Biomorph Color
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Biomorph Color");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, -1); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "biomorph_scale", args, n);

    basic_opts.biomorph_color_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      biomorph_color_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      biomorph_color_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    /*
     * Specific Outside Color
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Specific Outside Color");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "outside_color_scale", args, n);

    basic_opts.outside_nnn_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      outside_color_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      outside_color_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    /*
     * Decomposition
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Decomposition (2,4,8,..,256, 0=Off)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNmaximum, 256); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "decomposition_scale", args, n);

    basic_opts.decomp_option_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      decomp_option_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      decomp_option_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    /*
     * Fill Color
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized
      ("Fill color (0=no,1=yes,-1=old,+n=cmprsd,-n=sqrt)");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, -255); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "fill_color_scale", args, n);

    basic_opts.fill_color_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      fill_color_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      fill_color_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    /*
     * Orbit Delay
     * (scale)
     */

    n = 0;
    str = XmStringCreateLocalized("Orbit Delay");
    XtSetArg(args[n], XmNtitleString, str); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNmaximum, 10000); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;

    scale = XmCreateScale
      (rowcol, "orbit_delay_scale", args, n);

    basic_opts.orbit_delay_scale = scale;

    XtAddCallback(scale, XmNvalueChangedCallback,
      orbit_delay_scale_cb, NULL);

    XtAddCallback(scale, XmNdragCallback,
      orbit_delay_scale_cb, NULL);

    XtManageChild(scale);
    XmStringFree(str);

    XtManageChild(rowcol);

    /*
     * Help
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Help");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase - 1); n++;

    button = XmCreatePushButton
      (basic_options_dialog, "basic_dialog_help_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      basic_dialog_help_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Apply
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Apply");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (basic_options_dialog, "basic_dialog_apply_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      basic_dialog_done_btn_cb, (XtPointer) "0");

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Reset
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Reset");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 2); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 2 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (basic_options_dialog, "basic_dialog_reset_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      basic_dialog_reset_btn_cb, NULL);

    XtManageChild(button);
    XmStringFree(str);

    /*
     * Defaults
     * (pushbutton)
     */

    n = 0;
    str = XmStringCreateLocalized("Defaults");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, btnbase * 3); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 3 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (basic_options_dialog, "basic_dialog_defaults_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      basic_dialog_defaults_btn_cb, NULL);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 4); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, btnbase * 4 + (btnbase - 1)); n++;

    button = XmCreatePushButton
      (basic_options_dialog, "basic_dialog_cancel_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      basic_dialog_cancel_btn_cb, NULL);

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
    XtSetArg(args[n], XmNleftPosition, btnbase * 5); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, rowcol); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, fracbase - 1); n++;

    button = XmCreatePushButton
      (basic_options_dialog, "basic_dialog_done_btn", args, n);

    XtAddCallback(button, XmNactivateCallback,
      basic_dialog_done_btn_cb, (XtPointer) "1");

    XtManageChild(button);
    XmStringFree(str);

    XtVaSetValues(basic_options_dialog, XmNdefaultButton, button, NULL);

    XtManageChild(basic_options_dialog);
    XtManageChild(basic_options_dialog_shell);
    }

  basic_opts.current.stdcalcmode = usr_stdcalcmode;
  basic_opts.current.inside = inside;
  basic_opts.current.outside = outside;
  basic_opts.current.maxit = maxit;
  basic_opts.current.LogFlag = LogFlag;
  basic_opts.current.usr_biomorph = usr_biomorph;
  basic_opts.current.decomp = decomp[0];
  basic_opts.current.fillcolor = fillcolor;
  basic_opts.current.orbit_delay = orbit_delay;

  basic_opts.new = basic_opts.current;

  set_basic_opts_values();

  XtManageChild(basic_options_dialog);
  XtManageChild(basic_options_dialog_shell);
  XtPopup(basic_options_dialog_shell, XtGrabNone);

  XDefineCursor(display, window,
    XCreateFontCursor(display, XC_top_left_arrow));
}

static void set_basic_opts_values()
{
  switch(basic_opts.new.stdcalcmode)
    {
    case '1':
      XtVaSetValues(basic_opts.one_pass_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.two_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.guess_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.boundary_trace_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.tessaral_pass_toggle, XmNset, False, NULL);
    break;

    case '2':
      XtVaSetValues(basic_opts.one_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.two_pass_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.guess_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.boundary_trace_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.tessaral_pass_toggle, XmNset, False, NULL);
    break;

    case '3':
      XtVaSetValues(basic_opts.one_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.two_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.guess_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.boundary_trace_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.tessaral_pass_toggle, XmNset, False, NULL);
    break;

    case 'g':
      XtVaSetValues(basic_opts.one_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.two_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.guess_pass_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.boundary_trace_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.tessaral_pass_toggle, XmNset, False, NULL);
    break;

    case 'b':
      XtVaSetValues(basic_opts.one_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.two_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.guess_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.boundary_trace_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.tessaral_pass_toggle, XmNset, False, NULL);
    break;

    case 't':
      XtVaSetValues(basic_opts.one_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.two_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.guess_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.boundary_trace_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.tessaral_pass_toggle, XmNset, True, NULL);
    break;

    default:
      XtVaSetValues(basic_opts.one_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.two_pass_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.guess_pass_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.boundary_trace_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.tessaral_pass_toggle, XmNset, False, NULL);
    break;
    }

  switch(basic_opts.new.inside)
    {
    case -1:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -59:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -60:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -61:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -100:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -101:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -102:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    default:
      XtVaSetValues(basic_opts.maxiter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.zmag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.b_of_sixty_one_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.epscr_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.star_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.per_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.inside_nnn_toggle, XmNset, True, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, True);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue,
        basic_opts.new.inside, NULL);
    break;
    }

  switch(basic_opts.new.outside)
    {
    case -1:
      XtVaSetValues(basic_opts.iter_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.real_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.imag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.mult_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.summ_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -2:
      XtVaSetValues(basic_opts.iter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.real_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.imag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.mult_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.summ_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -3:
      XtVaSetValues(basic_opts.iter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.real_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.imag_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.mult_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.summ_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -4:
      XtVaSetValues(basic_opts.iter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.real_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.imag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.mult_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.summ_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -5:
      XtVaSetValues(basic_opts.iter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.real_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.imag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.mult_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.summ_toggle, XmNset, True, NULL);
      XtVaSetValues(basic_opts.outside_nnn_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    default:
      XtVaSetValues(basic_opts.iter_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.real_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.imag_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.mult_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.summ_toggle, XmNset, False, NULL);
      XtVaSetValues(basic_opts.outside_nnn_toggle, XmNset, True, NULL);
      XtSetSensitive(basic_opts.outside_nnn_scale, True);
      XtVaSetValues(basic_opts.outside_nnn_scale, XmNvalue,
        basic_opts.new.outside, NULL);
    break;
    }

  XtVaSetValues(basic_opts.iteration_scale, XmNvalue,
    basic_opts.new.maxit, NULL);

  XtVaSetValues(basic_opts.log_palette_scale, XmNvalue,
    basic_opts.new.LogFlag, NULL);

  XtVaSetValues(basic_opts.biomorph_color_scale, XmNvalue,
    basic_opts.new.usr_biomorph, NULL);

  XtVaSetValues(basic_opts.decomp_option_scale, XmNvalue,
    basic_opts.new.decomp, NULL);

  XtVaSetValues(basic_opts.fill_color_scale, XmNvalue,
    basic_opts.new.fillcolor, NULL);

  XtVaSetValues(basic_opts.orbit_delay_scale, XmNvalue,
    basic_opts.new.orbit_delay, NULL);

  XtVaSetValues(basic_opts.orbit_delay_scale, XmNvalue,
    basic_opts.new.orbit_delay, NULL);
}

/*ARGSUSED*/
static void toggle_passes_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmToggleButtonCallbackStruct *cbs =
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    basic_opts.new.stdcalcmode = (char) (*((char *)(client_data)));
}

/*ARGSUSED*/
static void toggle_inside_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  basic_opts.new.inside = atoi((char *) client_data);

  switch(basic_opts.new.inside)
    {
    case -1:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, False, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -59:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, False, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -60:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, False, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -61:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, False, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -100:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, False, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -101:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, False, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    case -102:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, False, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue, 0, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, False);
    break;

    default:
      XmToggleButtonSetState
        (basic_opts.maxiter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.zmag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.b_of_sixty_one_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.epscr_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.star_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.per_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.inside_nnn_toggle, True, False);
      XtVaSetValues(basic_opts.inside_nnn_scale, XmNvalue,
        basic_opts.new.inside, NULL);
      XtSetSensitive(basic_opts.inside_nnn_scale, True);
    break;
    }
}

/*ARGSUSED*/
static void inside_color_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  basic_opts.new.inside = cbs->value;
}

/*ARGSUSED*/
static void toggle_outside_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  basic_opts.new.outside = atoi((char *) client_data);

  switch(basic_opts.new.outside)
    {
    case -1:
      XmToggleButtonSetState
        (basic_opts.iter_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.real_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.imag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.mult_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.summ_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.outside_nnn_toggle, False, False);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -2:
      XmToggleButtonSetState
        (basic_opts.iter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.real_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.imag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.mult_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.summ_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.outside_nnn_toggle, False, False);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -3:
      XmToggleButtonSetState
        (basic_opts.iter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.real_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.imag_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.mult_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.summ_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.outside_nnn_toggle, False, False);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -4:
      XmToggleButtonSetState
        (basic_opts.iter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.real_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.imag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.mult_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.summ_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.outside_nnn_toggle, False, False);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    case -5:
      XmToggleButtonSetState
        (basic_opts.iter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.real_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.imag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.mult_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.summ_toggle, True, False);
      XmToggleButtonSetState
        (basic_opts.outside_nnn_toggle, False, False);
      XtSetSensitive(basic_opts.outside_nnn_scale, False);
    break;

    default:
      XmToggleButtonSetState
        (basic_opts.iter_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.real_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.imag_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.mult_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.summ_toggle, False, False);
      XmToggleButtonSetState
        (basic_opts.outside_nnn_toggle, True, False);
      XtVaSetValues(basic_opts.outside_nnn_scale, XmNvalue,
        basic_opts.new.outside, NULL);
      XtSetSensitive(basic_opts.outside_nnn_scale, True);
    break;
    }
}

/*ARGSUSED*/
static void outside_color_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  basic_opts.new.outside = cbs->value;
}

/*ARGSUSED*/
static void iteration_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  basic_opts.new.maxit = cbs->value;
}

/*ARGSUSED*/
static void log_palette_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  basic_opts.new.LogFlag = cbs->value;
}

/*ARGSUSED*/
static void biomorph_color_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  basic_opts.new.usr_biomorph = cbs->value;
}

/*ARGSUSED*/
static void decomp_option_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  int val = cbs->value;

  if(val >= 0 && val < 2)
    basic_opts.new.decomp = 0;
  else if(val >= 2 && val < 4)
    basic_opts.new.decomp = 2;
  else if(val >= 4 && val < 8)
    basic_opts.new.decomp = 4;
  else if(val >= 8 && val < 16)
    basic_opts.new.decomp = 8;
  else if(val >= 16 && val < 32)
    basic_opts.new.decomp = 16;
  else if(val >= 32 && val < 64)
    basic_opts.new.decomp = 32;
  else if(val >= 64 && val < 128)
    basic_opts.new.decomp = 64;
  else if(val >= 128 && val < 256)
    basic_opts.new.decomp = 128;
  else
    basic_opts.new.decomp = 256;
}

/*ARGSUSED*/
static void fill_color_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  basic_opts.new.fillcolor = cbs->value;
}

/*ARGSUSED*/
static void orbit_delay_scale_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  basic_opts.new.orbit_delay = cbs->value;
}

/*ARGSUSED*/
static void basic_dialog_reset_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  basic_opts.new = basic_opts.current;
  set_basic_opts_values();
}

/*ARGSUSED*/
static void basic_dialog_defaults_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  basic_opts.new = basic_opts.dflt;
  set_basic_opts_values();
}

/*ARGSUSED*/
static void basic_dialog_cancel_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  basic_opts.new = basic_opts.current;
  set_basic_opts_values();
  XtPopdown(basic_options_dialog_shell);
}

/*ARGSUSED*/
static void basic_dialog_done_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int changed = 0;
  int pop_down = atoi((char *)(client_data));

  if(usr_stdcalcmode != basic_opts.new.stdcalcmode ||
     inside != basic_opts.new.inside               ||
     outside != basic_opts.new.outside             ||
     maxit != basic_opts.new.maxit                 ||
     LogFlag != basic_opts.new.LogFlag             ||
     usr_biomorph != basic_opts.new.usr_biomorph   ||
     decomp[0] != basic_opts.new.decomp            ||
     fillcolor != basic_opts.new.fillcolor         ||
     orbit_delay != basic_opts.new.orbit_delay)
    changed = 1;
  else
    changed = 0;

  helpmode = oldhelpmode;
  usr_stdcalcmode = basic_opts.new.stdcalcmode;
  inside = basic_opts.new.inside;
  outside = basic_opts.new.outside;
  maxit = basic_opts.new.maxit;
  LogFlag = basic_opts.new.LogFlag;
  usr_biomorph = basic_opts.new.usr_biomorph;
  decomp[0] = basic_opts.new.decomp;
  fillcolor = basic_opts.new.fillcolor;
  orbit_delay = basic_opts.new.orbit_delay;

  basic_opts.current = basic_opts.new;

  if(changed)
    {
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    }

  if(pop_down == 1)
    XtPopdown(basic_options_dialog_shell);
}

/*ARGSUSED*/
static void basic_dialog_help_btn_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  read_help(main_window, NULL, NULL);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: basic_opts.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.16  1995/06/03  20:18:36  darryl
 * Allow zooming and/or panning after changes in basic or
 * extended options.
 *
 * Revision 1.15  1995/06/02  00:42:22  darryl
 * Fix orbit delay and show orbits modes.
 *
 * Revision 1.14  1995/05/16  18:02:13  darryl
 * Add drag callbacks to Scale widgets.
 *
 * Revision 1.13  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.12  1995/05/09  21:03:55  darryl
 * Fix bugs reported in beta test.
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
 * Revision 1.7  1995/03/25  20:27:49  darryl
 * Add cancel buttons for several dialogs. Make the corner
 * values dialog title dynamic, reflecting the current mode.
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
 * Revision 1.1.1.1  1995/03/11  14:35:47  tom
 * add ifdef EDITRES_ENABLE
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
