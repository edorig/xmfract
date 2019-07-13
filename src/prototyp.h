
#ifndef prototyp_h
#define prototyp_h

#ifndef NO_IDENT
#ident "$Id: prototyp.h,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* basic_opts.c */
extern void create_basic_options_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);

/* bifurcation.c */
extern int Bifurcation(void);
extern int BifurcLambda(void);
extern int BifurcVerhulstTrig(void);
extern int BifurcStewartTrig(void);
extern int BifurcSetTrigPi(void);
extern int BifurcAddTrigPi(void);
extern int BifurcLambdaTrig(void);
extern int BifurcMay(void);
extern int BifurcMaySetup(void);

/* boundary_trace.c */
extern int bound_trace_main(void);

/* calcfracinit.c */
extern void calcfracinit(void);
extern void adjust_corner(void);

extern int add_worklist
  (int xfrom, int xto, int yfrom, int yto, int ybegin, int pass, int sym);

extern void tidy_worklist(void);

#ifdef __STDC__
  extern int put_resume(int len, ...);
  extern int get_resume(int len, ...);
#else
  extern int put_resume();
  extern int get_resume();
#endif

extern int alloc_resume(int alloclen, int version);
extern int start_resume(void);
extern void end_resume(void);
extern int ssg_blocksize(void);

/* calcfract.c */
extern int calcfract(void);
extern void invertz2(_CMPLX *z);

/* calcfroth.c */
extern int froth_setup(void);
extern int calcfroth(void);

/* calcmand.c */
extern int calcmand(void);

/* calcmandfp.c */
extern int calcmandfpasmstart(void);
extern int calcmandfp(void);
extern int calcmandfpasm(void);

/* caution.c */
extern void caution
  (Widget w, XtPointer client_data, XtPointer call_data);

/* cellular.c */
extern int cellular(void);
extern int CellularSetup(void);

/* check_mapfile.c */
extern void check_mapfile
  (Widget w, XtPointer client_data, XtPointer call_data);

/* check_tgafile.c */
extern void check_tgafile
  (Widget w, XtPointer client_data, XtPointer call_data);

/* cmdfile.c */
extern int cmdfile(FILE *handle, int mode);
extern int cmdarg(char *curarg, int mode);
extern int load_commands(FILE *infile);
extern void set_3d_defaults(void);
extern void initvars_fractal(void);
extern int cmdfiles(int argc, char **argv);
extern void initvars_restart(void);

/* corner_param.c */
extern void create_corner_param_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);

/* authors_menu.c */
extern void create_authors_menu(Widget w);
extern void display_disclaimer
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void display_authors_window_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* create_image.c */
extern void create_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void create_previous_image(int which);

/* image_info.c */
extern void create_current_image_info_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);

/* misc_menu.c */
extern void create_misc_menu(Widget w);

/* decoder.c */
extern short decoder(short linewidth);

/* decomposition.c */
extern void decomposition(void);

/* diffusion.c */
extern int diffusion(void);

/* draw_image.c */
extern void create_draw_fractal_menu(Widget w);

/* coloredit.c */
extern void create_coloredit_menu(Widget w);
extern void create_editpal_options_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void redraw_palette_colorbox
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void update_randmap_dialog(void);
extern void update_undo_index(void);

/* encoder.c */
extern int encoder(void);

/* ext_opts.c */
extern void create_extended_options_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);

/* fatal.c */
extern int fatal(Widget w, char *msg);

/* file_menu.c */
extern void saved_cmd_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_fname_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void load_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_image_file_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void load_image_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_transform_file_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void load_map_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_mapname_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void save_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_save_name_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void save_par_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_save_parameter_name_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_save_parameter_err_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void save_palette_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void get_save_palette_name_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void quit_prog_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void editpal_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void saved_cmd_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void save_image_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void save_par_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void save_palette_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void load_transform_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);
extern void create_file_menu(Widget w);
extern void delay_display_file(XtPointer client_data, XtIntervalId Id);

extern void load_transform_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void load_map_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* formula.c */
extern int RunForm(char *Name);
extern int fpFormulaSetup(void);
extern int form_per_pixel(void);
extern int Formula(void);
extern void dRandom(void);
extern void SetRandFnct(void);

/* fractal_types.c */
extern void create_fractal_types_menu(Widget w);

/* fractals.c */
extern int MandelSetup(void);
extern int MandelfpSetup(void);
extern int MandellongSetup(void);
extern int mandel_per_pixel(void);
extern int StandaloneSetup(void);
extern int mandelfp_per_pixel(void);
extern int JuliaSetup(void);
extern int julia_per_pixel(void);
extern int JuliafpFractal(void);
extern int JuliafpSetup(void);
extern int juliafp_per_pixel(void);
extern int floatbailout(void);
extern int otherjuliafp_per_pixel(void);
extern int marksmandelfp_per_pixel(void);
extern int othermandelfp_per_pixel(void);
extern int marks_mandelpwrfp_per_pixel(void);
extern int otherrichard8fp_per_pixel(void);
extern int phoenix_per_pixel(void);
extern int quaternionfp_per_pixel(void);
extern int Halley_per_pixel(void);
extern int mandphoenix_per_pixel(void);
extern int quaternionjulfp_per_pixel(void);
extern int JulialongSetup(void);
extern int TrigPlusTriglongSetup(void);
extern int TrigPlusTrigfpSetup(void);
extern int LambdaTrigOrTrigSetup(void);
extern int JuliaTrigOrTrigSetup(void);
extern int ManlamTrigOrTrigSetup(void);
extern int MandelTrigOrTrigSetup(void);
extern int ZXTrigPlusZSetup(void);
extern int LambdaTrigSetup(void);
extern int JuliafnPlusZsqrdSetup(void);
extern int SqrTrigSetup(void);
extern int FnXFnSetup(void);
extern int MandelTrigSetup(void);
extern int MarksJuliaSetup(void);
extern int MarksJuliafpSetup(void);
extern int SierpinskiSetup(void);
extern int SierpinskiFPSetup(void);
extern int HalleySetup(void);
extern int PhoenixSetup(void);
extern int MandPhoenixSetup(void);
extern int StandardSetup(void);
extern int demowalk(void);
extern int TrigPlusTrigfpFractal(void);
extern int ZXTrigPlusZfpFractal(void);
extern int LambdaTrigfpFractal(void);
extern int LambdaexponentFractal(void);
extern int PhoenixFractal(void);
extern int PhoenixPlusFractal(void);
extern int PhoenixMinusFractal(void);
extern void cpower(_CMPLX *base, int exp, _CMPLX *result);
extern int UnityfpFractal(void);
extern int complex_mult(_CMPLX arg1, _CMPLX arg2, _CMPLX *pz);
extern int LambdaFPFractal(void);
extern int ManOWarfpFractal(void);
extern int test(void);
extern int SqrTrigfpFractal(void);
extern int MarksLambdaFractal(void);
extern int marksmandel_per_pixel(void);
extern int UnitySetup(void);
extern int Mandel4Fractal(void);
extern int Mandel4fpFractal(void);
extern int Barnsley3Fractal(void);
extern int Barnsley3FPFractal(void);
extern int TrigZsqrdfpFractal(void);
extern int TrigXTrigfpFractal(void);
extern int Sqr1overTrigfpFractal(void);
extern int TrigPlusZsquaredfpFractal(void);
extern int floatZtozPluszpwrFractal(void);
extern int FloatTrigPlusExponentFractal(void);
extern int PopcornFractal(void);
extern int MarksCplxMand(void);
extern int MarksCplxMandperp(void);
extern int SierpinskiFPFractal(void);
extern int Barnsley1FPFractal(void);
extern int Barnsley2FPFractal(void);
extern int SpiderfpFractal(void);
extern int TetratefpFractal(void);
extern int Magnet1Fractal(void);
extern int Magnet2Fractal(void);
extern int Richard8fpFractal(void);
extern int MarksMandelPwrfpFractal(void);
extern int TimsErrorfpFractal(void);
extern int CirclefpFractal(void);
extern int LambdaTrigOrTrigfpFractal(void);
extern int JuliaTrigOrTrigfpFractal(void);
extern int HalleyFractal(void);
extern int QuaternionFPFractal(void);
extern int HyperComplexFPFractal(void);
extern int MarksLambdafpFractal(void);
extern int floatZpowerFractal(void);
extern int floatCmplxZpowerFractal(void);

/* three_d_params.c */
extern void get_3d_params
  (Widget w, XtPointer client_data, XtPointer call_data);

/* stereo_params.c */
extern void get_funny_glasses_params
  (Widget w, XtPointer client_data, XtPointer call_data);

/* attractor.c */
extern void get_julia_attractor(double real, double imag);

/* light_params.c */
extern void get_light_params
  (Widget w, XtPointer client_data, XtPointer call_data);

/* star_params.c */
extern void get_starfield_params(Widget w);

/* gifview.c */
extern int get_byte(void);
extern int get_bytes(unsigned char *where, int how_many);
extern int gifview1(void);

/* ifsload.c */
extern int ifsload(void);

/* inform.c */
extern void inform
  (Widget w, XtPointer client_data, XtPointer call_data);

/* init_data.c */
extern void init_data(int argc, char **argv);

/* julibrot.c */
extern int JulibrotSetup(void);
extern int jb_per_pixel(void);
extern int jbfp_per_pixel(void);
extern int Std4dFractal(void);
extern int Std4dfpFractal(void);

/* jiim.c */
extern void circle(int radius, int color);
extern int QueueEmpty(void);
extern int QueueFull(void);
extern int QueueFullAlmost(void);
extern int Init_Queue(unsigned long request);
extern void Free_Queue(void);
extern int PushLong(long x, long y);
extern int PushFloat(float x, float y);
extern int EnQueueFloat(float x, float y);
extern int EnQueueLong(long x, long y);
extern void SaveRect(int x, int y, int width, int depth);
extern void RestoreRect(int x, int y, int width, int depth);
extern void do_jiim(void);
extern void Jiim(int which);

/* line3d.c */
extern int line3d(unsigned char *pixels, unsigned linelen);

extern void corners
  (MATRIX m, int show, double *pxmin, double *pymin,
   double *pzmin, double *pxmax, double *pymax, double *pzmax);

extern int offscreen(struct point pt);
extern int targa_color(int x, int y, int color);

/* loadfile.c */
extern int read_overlay(void);

/* loadmap.c */
extern void SetTgaColors(void);
extern int ValidateLuts(char *fn);
extern int SetColorPaletteName(char *fn);

/* lorenz.c */
extern int setup_convert_to_screen(struct affine *scrn_cnvt);
extern int orbit3dlongsetup(void);
extern int orbit3dfloatsetup(void);
extern int Minverse_julia_orbit(void);
extern int Linverse_julia_orbit(void);
extern int lorenz3dlongorbit(long *l_x, long *l_y, long *l_z);
extern int lorenz3d1floatorbit(double *x, double *y, double *z);
extern int lorenz3dfloatorbit(double *x, double *y, double *z);
extern int lorenz3d3floatorbit(double *x, double *y, double *z);
extern int lorenz3d4floatorbit(double *x, double *y, double *z);
extern int henonfloatorbit(double *x, double *y, double *z);
extern int henonlongorbit(long *l_x, long *l_y, long *l_z);
extern int rosslerfloatorbit(double *x, double *y, double *z);
extern int pickoverfloatorbit(double *x, double *y, double *z);
extern int gingerbreadfloatorbit(double *x, double *y, double *z);
extern int rosslerlongorbit(long *l_x, long *l_y, long *l_z);
extern int kamtorusfloatorbit(double *r, double *s, double *z);
extern int kamtoruslongorbit(long *r, long *s, long *z);
extern int hopalong2dfloatorbit(double *x, double *y, double *z);
extern int martin2dfloatorbit(double *x, double *y, double *z);
extern int mandelcloudfloat(double *x, double *y, double *z);
extern int dynamfloat(double *x, double *y, double *z);
extern int iconfloatorbit(double *x, double *y, double *z);
extern int inverse_julia_per_image(void);
extern int orbit2dfloat(void);
extern int orbit2dlong(void);
extern int dynam2dfloatsetup(void);
extern int dynam2dfloat(void);
extern int funny_glasses_call(int (*calc )());
extern int ifs(void);
extern int orbit3dfloat(void);
extern int orbit3dlong(void);
extern int ifs3d(void);
extern _CMPLX ComplexSqrtFloat(double x, double y);
extern _CMPLX DeQueueFloat(void);

/* lsys.c */
extern int Lsystem(void);
extern int LLoad(void);

/* lyapunov.c */
extern int lyapunov(void);
extern int lya_setup(void);

/* make_mig.c */
extern int make_mig(int xmult, int ymult);

/* miscfrac.c */
extern int plasma(void);
extern int popcorn(void);

/* modal.c */
extern int confirm(Widget w, char *msg, int buttons, unsigned char dflt);

/* mpmath.c */
extern _CMPLX ComplexPower(_CMPLX xx, _CMPLX yy);
extern void SetupLogTable(void);
extern long ExpFloat14(long xx);
extern int ComplexNewtonSetup(void);
extern int ComplexNewton(void);
extern int ComplexBasin(void);
extern int GausianNumber(int Probability, int Range);
extern long divide(long x, long y, int n);
extern long multiply(long x, long y, int n);
extern float tofloat(long x, int n);
extern void FPUcplxmul(_CMPLX *x, _CMPLX *y, _CMPLX *z);
extern void FPUcplxdiv(_CMPLX *x, _CMPLX *y, _CMPLX *z);
extern void FPUsincos(double *Angle, double *Sin, double *Cos);
extern void FPUsinhcosh(double *Angle, double *Sinh, double *Cosh);
extern void FPUcplxlog(_CMPLX *x, _CMPLX *z);
extern void SinCos086(long x, long *sinx, long *cosx);
extern long r16Mul(long x, long y);
extern long LogFloat14(unsigned long x);
extern long RegDivFloat(long x, long y);
extern long RegFg2Float(long x, int FudgeFact);
extern long RegFloat2Fg(long x, int Fudge);
extern long RegSftFloat(long x, int Shift);

/* newton.c */
extern int NewtonSetup(void);
extern int NewtonFractal2(void);

/* oneortwopass.c */
extern int OneOrTwoPass(void);

/* options_menu.c */
extern void create_options_menu(Widget w);

/* parser.c */
extern void dStkAdd(void);
extern void dStkSub(void);
extern void dStkNeg(void);
extern void dStkMul(void);
extern void dStkLT(void);
extern void dStkLTE(void);
extern void dStkMod(void);
extern void dStkPwr(void);
extern void dStkDiv(void);
extern void dStkAbs(void);
extern void dStkReal(void);
extern void dStkImag(void);
extern void dStkSin(void);
extern void dStkSinh(void);
extern void dStkSqr(void);
extern void dStkCosh(void);
extern void dStkCosXX(void);
extern void dStkExp(void);
extern void dStkLog(void);
extern void dStkRecip(void);
extern void StkIdent(void);
extern void dStkCos(void);
extern void dStkTan(void);
extern void dStkTanh(void);
extern void dStkCoTan(void);
extern void dStkCoTanh(void);
extern void dStkFlip(void);
extern void dStkConj(void);
extern void dStkZero(void);
extern void dStkGT(void);
extern void dStkGTE(void);
extern void dStkEQ(void);
extern void dStkNE(void);
extern void dStkAND(void);
extern void dStkOR(void);
extern void dStkSRand(void);
extern void FPUcplxexp(_CMPLX *x, _CMPLX *z);
extern void StkClr(void);
extern void EndInit(void);
extern void StkSto(void);
extern void StkLod(void);
extern void NotAFnct(void);
extern void FnctNotFound(void);

/* worklist.c */
extern void perform_worklist(void);
extern void free_workarea(void);

/* plot3d.c */
extern void plot_setup(void);

/* plot_orbit.c */
extern void iplot_orbit(long ix, long iy, int color);
extern void plot_orbit(double real, double imag, int color);
extern void scrub_orbit(void);

/* potential.c */
extern int potential(double mag, int iterations);

/* read_help.c */
extern void create_helps_menu(Widget w);
extern void read_help(Widget w, XtPointer client_data, XtPointer call_data);

/* reset_menu.c */
extern void create_reset_menu(Widget w);

extern void clear_image_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* resize_window.c */
extern void resize_window
  (Widget w, XtPointer client_data, XtPointer call_data);

/* restore_menu.c */
extern void create_restore_menu(Widget w);
extern void update_restore_menu(void);

/* rotate.c */
extern void rotate(XtPointer client_data, XtIntervalId Id);
extern int save_palette(char *fname);
extern void cycle_speed_increase(void);
extern void cycle_speed_decrease(void);
extern void cycle_step_forward(void);
extern void cycle_step_backward(void);
extern void toggle_color_cycling(void);
extern void randomize_cycle_palette(int points, int modifier);

/* rubber_band.c */
extern void rubber_band
  (Widget w, XEvent *event, String *args, int *argcnt);

extern void zoom_cancel_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void erase_current_zoombox(void);

extern void zoom_in_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void zoom_out_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void zoom_aspect_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void zoom_pan_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

/* saveparms.c */
extern void saveparms(char *filename, char *entryname, int write);

/* savetodisk.c */
extern int savetodisk(char *filename);

/* fill.c */
extern void select_3d_fill
  (Widget w, XtPointer client_data, XtPointer call_data);

/* planar.c */
extern void select_3d_planar
  (Widget w, XtPointer client_data, XtPointer call_data);

/* cmd_entry.c */
extern void select_cmd_entry(Widget w, char *filename, int write);

/* formula_params.c */
extern void select_formula_file(Widget w, char *getname);
extern void select_frm_entry(Widget w, char *filename);

/* ifs_params.c */
extern void select_ifs_file(Widget w, char *getname);
extern void select_ifs_entry(Widget w, char *filename);

/* julibrot_param.c */
extern void select_julibrot_params(Widget w);

/* lsys_params.c */
extern void select_lsystem_file(Widget w, char *getname);
extern void select_lsys_entry(Widget w, char *filename);

/* select_type.c */
extern void create_type_selection_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void select_type_err_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void select_type_ok_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern int find_extra_param(int type);
extern int xmcompare(const void *arg1, const void *arg2);
extern void roundfloatd(double *x);

/* window_size.c */
extern void window_size_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void check_geometry(XtPointer client_data, XtIntervalId Id);

/* solidguess.c */
extern int solidguess(void);

/* standard.c */
extern int StandardFractal(void);

/* starfield.c */
extern int starfield(void);

/* stubs.c */
extern int intFormulaSetup(void);
extern int LongBifurcMay(void);
extern int LongBifurcLambdaTrig(void);
extern int LongBifurcAddTrigPi(void);
extern int LongBifurcSetTrigPi(void);
extern int LongBifurcStewartTrig(void);
extern int LongBifurcVerhulstTrig(void);
extern int JuliaFractal(void);
extern int LambdaFractal(void);
extern int longZpowerFractal(void);
extern int longCmplxZpowerFractal(void);
extern int TrigPlusSqrFractal(void);
extern int ScottTrigPlusSqrFractal(void);
extern int SkinnerTrigSubSqrFractal(void);
extern int long_julia_per_pixel(void);
extern int TrigPlusTrigFractal(void);
extern int ScottTrigPlusTrigFractal(void);
extern int SkinnerTrigSubTrigFractal(void);
extern int ZXTrigPlusZFractal(void);
extern int ScottZXTrigPlusZFractal(void);
extern int SkinnerZXTrigSubZFractal(void);
extern int LambdaTrigFractal(void);
extern int LambdaTrigFractal1(void);
extern int LambdaTrigFractal2(void);
extern int LambdaTrigfpFractal2(void);
extern int LongLambdaexponentFractal(void);
extern int LongPhoenixFractal(void);
extern int LongPhoenixPlusFractal(void);
extern int LongPhoenixMinusFractal(void);
extern int ManOWarFractal(void);
extern int SierpinskiFractal(void);
extern int Barnsley1Fractal(void);
extern int long_mandel_per_pixel(void);
extern int Barnsley2Fractal(void);
extern int SqrTrigFractal(void);
extern int UnityFractal(void);
extern int TrigZsqrdFractal(void);
extern int TrigXTrigFractal(void);
extern int Sqr1overTrigFractal(void);
extern int TrigPlusZsquaredFractal(void);
extern int LongTrigPlusExponentFractal(void);
extern int LPopcornFractal(void);
extern int MPCNewtonFractal(void);
extern int MPCjulia_per_pixel(void);
extern int SpiderFractal(void);
extern int Richard8Fractal(void);
extern int long_richard8_per_pixel(void);
extern int MarksMandelPwrFractal(void);
extern int marks_mandelpwr_per_pixel(void);
extern int LambdaTrigOrTrigFractal(void);
extern int JuliaTrigOrTrigFractal(void);
extern int MPCHalleyFractal(void);
extern int MPCHalley_per_pixel(void);
extern int long_phoenix_per_pixel(void);
extern int long_mandphoenix_per_pixel(void);
extern int TimsErrorFractal(void);
extern int MatchTPlusMode(void);
extern int CheckForTPlus(void);
extern void WriteTPlusBankedPixel(int col, int row, unsigned long color);

/* symmetry.c */
extern void setsymmetry(int sym, int uselist);
extern int FnPlusFnSym(void);

/* symplot.c */
extern void noplot(int x, int y, int color);
extern void symplot2basin(int x, int y, int color);
extern void symplot4basin(int x, int y, int color);
extern int autologmap(void);
extern void symPIplot(int x, int y, int color);
extern void symPIplot2J(int x, int y, int color);
extern void symPIplot4J(int x, int y, int color);
extern void symplot2(int x, int y, int color);
extern void symplot2Y(int x, int y, int color);
extern void symplot2J(int x, int y, int color);
extern void symplot4(int x, int y, int color);

/* tesseral.c */
extern int tesseral(void);

/* testpt.c */
extern int teststart(void);
extern void testend(void);

extern int testpt
  (double initreal, double initimag, double parm1,
   double parm2, int maxit, int inside);

/* three_d_lib.c */
extern void identity(MATRIX m);
extern void mat_mul(MATRIX mat1, MATRIX mat2, MATRIX mat3);
extern void scale(double sx, double sy, double sz, MATRIX m);
extern void xrot(double theta, MATRIX m);
extern void yrot(double theta, MATRIX m);
extern void zrot(double theta, MATRIX m);
extern void trans(double tx, double ty, double tz, MATRIX m);
extern int cross_product(VECTOR v, VECTOR w, VECTOR cross);
extern int normalize_vector(VECTOR v);
extern int vmult(VECTOR s, MATRIX m, VECTOR t);
extern void mult_vec_c(VECTOR s);
extern int perspective(VECTOR v);

extern int longvmultpersp(LVECTOR s, LMATRIX m, LVECTOR t0,
  LVECTOR t, LVECTOR lview, int bitshift);

extern int longpersp(LVECTOR lv, LVECTOR lview, int bitshift);
extern int longvmult(LVECTOR s, LMATRIX m, LVECTOR t, int bitshift);

/* three_d_menu.c */
extern void create_three_d_options_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);

/* timer.c */
#ifdef __STDC__
  extern int timer(int timertype, int (*subrtn )(), ...);
#else
  extern int timer();
#endif

/* tp3d.c */
extern void TranspPerPixel
  (int MathType, union FracArg *xy, union FracArg *zt);

extern void ShadowPutColor(unsigned xdot, unsigned ydot, unsigned color);

/* type_param.c */
extern void type_specific_params(Widget w);

extern void param_done_cb
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void set_default_parms(void);

/* util.c */
extern void fromextra(int fromoffset, char *toaddr, int tocount);
extern void toextra(int tooffset, char *fromaddr, int fromcount);
extern void updatesavename(char *filename);
extern int cmpextra(int cmpoffset, char *cmpaddr, int cmpcount);
extern KeySym waitkeypressed(int wait_time);
extern KeySym check_key(void);
extern KeySym keypressed(void);
extern long clock_ticks(void);
extern KeySym getakey(void);
extern KeySym havekey(void);

extern void response
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void modal_response
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void fsb_no_match
  (Widget w, XtPointer client_data, XtPointer call_data);

extern void buzzer(int buzzertype);

extern int cvtcentermag
  (double *Xctr, double *Yctr, double *Magnification);

extern int set_trig_array(int k, char *name);
extern void set_trig_pointers(int which);
extern char *strlwr(char *s);

extern int find_file_item
  (char *filename, char *itemname, FILE **infile);

extern void findpath(char *filename, char *fullpathname);
extern long stackavail(void);

extern int check_orbit_name(char *orbitname);

extern int build_fractal_list
  (int fractals[], int *last_val, char *nameptr[]);

extern int check_writefile(char *name, char *ext);
extern long normalize(char *ptr);
extern void notdiskmsg(void);
extern void create_xor_gc(void);
extern void change_xor_gc(int do_line);

extern void decode_fractal_info(struct fractal_info *info, int dir);
extern void fix_ranges(int *ranges, int num, int dir);
extern void display_file(void);

extern int makepath
  (char *template, char *drive, char *dir, char *fname, char *ext);

extern int splitpath
  (char *template, char *drive, char *dir, char *fname, char *ext);

extern void showtrig(char *buf);

extern void map_dialog
  (Widget dialog, XtPointer client_data, XtPointer call_data);

extern int image_load_error(void);
extern int resize_for_new_image(int new_width, int new_height);
extern void get_winsize_defaults(void);
extern int startdisk(void);
extern int targa_startdisk(FILE *targafp, int overhead);
extern int common_startdisk(int newrowsize, int newcolsize, int colors);
extern void enddisk(void);
extern int readdisk(int col, int row);
extern int FromMemDisk(long offset, int size, void *dest);

extern void targa_readdisk
  (unsigned int col, unsigned int row, unsigned char *red,
   unsigned char *green, unsigned char *blue);

extern void writedisk(int col, int row, int color);
extern int ToMemDisk(long offset, int size, void *src);

extern void targa_writedisk
  (unsigned int col, unsigned int row, unsigned char red,
   unsigned char green, unsigned char blue);

extern int pot_startdisk(void);

extern void kbd_event
  (Widget w, XtPointer client_data, XEvent *event, Boolean *dispatch);

extern void toggle_zoom_grab(int which);
extern void update_draw_menu(int cont);
extern void force_update(Widget w);
extern void fpe_handler(int signum);

/* video.c */
extern int getcolor(int xdot, int ydot);
extern void putcolor(int xdot, int ydot, int color);
extern void writevideo(int x, int y, int color);
extern void clear_window(void);

extern void put_line
  (int row, int startcol, int stopcol, unsigned char *pixels);

extern void writevideoline(int y, int x, int lastx, unsigned char *pixels);
extern int readvideo(int x, int y);
extern void readvideoline(int y, int x, int lastx, unsigned char *pixels);
extern void spindac(int dir, int inc);
extern int writevideopalette(void);
extern void initdacbox(void);
extern int cmapstuff(void);
extern void draw_line(int X1, int Y1, int X2, int Y2, int color);
extern void find_special_colors(void);
extern int ShadowVideo(int on);
extern int out_line(unsigned char *pixels, unsigned linelen);

extern void get_line
  (int row, int startcol, int stopcol, unsigned char *pixels);

extern void flip_image(int flip_direction);
extern void load_videotable(int options);
extern void load_color_map(Widget w, char *filename);


/* warning.c */
extern void warning
  (Widget w, XtPointer client_data, XtPointer call_data);

/* window_options.c */
extern void create_window_options_dialog
  (Widget w, XtPointer client_data, XtPointer call_data);

/* xmfract.c */
extern void main(int argc, char **argv);
extern void delay_display_authors_window
  (XtPointer client_data, XtIntervalId Id);
extern void delay_display_disclaimer
  (XtPointer client_data, XtIntervalId Id);

/* zoom.c */
extern void drawbox(void);
extern void zoomout(void);
extern void zmo_calc(double dx, double dy, double *newx, double *newy);
extern void aspectratio_crop(float oldaspect, float newaspect);
extern int check_pan(void);
extern void move_row(int fromrow, int torow, int col);
extern int init_pan_or_recalc(int do_zoomout);
extern void restart_window(int wknum);
extern void fix_worklist(void);
extern void reset_zoom_corners(void);
extern void clear_zoombox(void);

/* zoom_menu.c */
extern void create_zoom_menu(Widget w);
extern void update_zoom_menu(void);

#endif

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: prototyp.h,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.14  1995/06/14  01:30:07  darryl
 * Add color editor undo-last-command and undo-last-redo and
 * command-line and ini file support for max undo commands.
 *
 * Revision 1.13  1995/06/08  00:01:06  darryl
 * Fix mig handling.
 *
 * Revision 1.12  1995/05/23  18:50:55  darryl
 * Fix color editor and palette randomization problems
 * reported in beta testing.
 *
 * Revision 1.11  1995/05/21  09:54:04  darryl
 * Change arguments to palette randomization routines.
 *
 * Revision 1.10  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.9  1995/05/04  17:55:21  darryl
 * Fix bug where color editor was not aware of color map
 * changes if the map was loaded from a file or randomized.
 *
 * Revision 1.8  1995/05/03  04:54:20  darryl
 * Re-arrange main menu bar, rename some menus, move others.
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
 * Revision 1.6  1995/03/26  22:28:18  darryl
 * Add buttons to the type selection dialogs for returning
 * to a file or name selection dialog if the type is
 * formula, lsystem, or ifs. Update the help dialog so that
 * only one may be displayed at a time. Display a clock
 * cursor while an image is being generated.
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
 * Revision 1.2.1.3  1995/03/14  15:22:11  tom
 * changed #if to #ifdef
 *
 * Revision 1.2.1.2  1995/03/13  18:42:26  tom
 * changed type of calc_status
 *
 * Revision 1.2.1.1  1995/03/11  03:18:21  tom
 * change varargs handling
 * remove stricmp prototyp
 *
 * Revision 1.2  1995/03/10  19:32:17  darryl
 * Add signal handler definition.
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.301  1995/01/17  00:03:30  darryl
 * Add a disclaimer dialog.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.201  1994/12/07  00:31:30  darryl
 * Change calls to rubber banding functions from a passive
 * pointer grab to translations from the canvas widget.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.106  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.105  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.104  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.103  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.102  1994/11/23  05:19:37  darryl
 * Initial implementation of help. Minor widget alignment
 * changes. Widget name changes for uniqueness.
 *
 * Revision 0.101  1994/11/19  02:53:05  darryl
 * Move plotblock() to solidguess.c and declare it static.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
