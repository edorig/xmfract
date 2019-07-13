
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: util.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void getChar(unsigned char *dst, unsigned char **src, int dir);
static void getInt(short *dst, unsigned char **src, int dir);
static void getLong(long *dst, unsigned char **src, int dir);
static void getDouble(double *dst, unsigned char **src, int dir);
static void getFloat(float *dst, unsigned char **src, int dir);
static void trigdetails(char *buf);

/* static variables */
static unsigned char *dataPtr;
static int vidtbllen;
static int from_inverse = 0;
static struct videoinfo *vidtbl;
static int old_type = -1;
static char jmapname[MAXPATHLEN];

void fromextra(int fromoffset, char *toaddr, int tocount)
{
  memcpy(toaddr,(char *)(extraseg+fromoffset),tocount);
}

void toextra(int tooffset, char *fromaddr, int fromcount)
{
  memcpy((char *)(extraseg+tooffset),fromaddr,fromcount);
}

void updatesavename(char *filename) /* go to the next file name */
{
  char *save, *hold;
  char name[MAXPATHLEN],suffix[MAXPATHLEN];
  char *dotptr;

  strcpy(name,filename);
  suffix[0] = 0;
  if ((dotptr = strrchr(name,'.')) != NULL
    && dotptr > strrchr(name,SLASHC))
    {
    strcpy(suffix,dotptr);
    *dotptr = 0;
    }

  hold = name + strlen(name) - 1; /* start at the end */
  while(hold >= name && (*hold == ' ' || isdigit(*hold))) /* skip backwards */
    hold--;
  hold++;          /* recover first digit */
  while (*hold == '0')         /* skip leading zeros */
    hold++;
  save = hold;
  while (*save)
    {
    /* check for all nines */
    if (*save != '9')
      break;
    save++;
    }
  if (!*save)          /* if the whole thing is nines then back */
    save = hold - 1;   /* up one place. Note that this will eat */
                       /* your last letter if you go too far.   */
  else
    save = hold;
  sprintf(save,"%d",atoi(hold)+1); /* increment the number */
  strcpy(filename,name);
  strcat(filename,suffix);
}

int cmpextra(int cmpoffset, char *cmpaddr, int cmpcount)
{
  return memcmp((char *)(extraseg+cmpoffset),cmpaddr,cmpcount);
}

KeySym waitkeypressed(int wait_time)
{
  XEvent event;
  long start;
  long finish;
  char buf[128];
  KeySym ks;
  int len;
  int timed_out = 0;

  ks = 0;

  if(wait_time)
    {
    start = clock_ticks();

    while(!timed_out)
      {
      if(XCheckTypedWindowEvent(display,XtWindow(canvas),KeyRelease,&event))
        {
        XKeyEvent *key_event = (XKeyEvent *) &event;
        len = XLookupString(key_event,buf,128,&ks,(XComposeStatus *) NULL);
        if(ks == CTRL_LEFT || ks == CTRL_RIGHT)
          {
          ctrlkey = 0;
          ks = 0;
          }
        else if(ks == SHIFT_LEFT || ks == SHIFT_RIGHT)
          {
          shiftkey = 0;
          ks = 0;
          }
        else if(ks == XK_Alt_L || ks == XK_Alt_R)
          {
          altkey = 0;
          ks = 0;
          }
        else if(ks == XK_Meta_L || ks == XK_Meta_R)
          {
          metakey = 0;
          ks = 0;
          }
        }
      else if(XCheckTypedWindowEvent(display,XtWindow(canvas),KeyPress,&event))
        {
        XKeyEvent *key_event = (XKeyEvent *) &event;
        len = XLookupString(key_event,buf,128,&ks,(XComposeStatus *) NULL);

        if(ks == CTRL_LEFT || ks == CTRL_RIGHT)
          {
          ctrlkey = 1;
          ks = 0;
          }

        if(ks == SHIFT_LEFT || ks == SHIFT_RIGHT)
          {
          shiftkey = 1;
          ks = 0;
          }
        else if(ks == XK_Alt_L || ks == XK_Alt_R)
          {
          altkey = 1;
          ks = 0;
          }
        else if(ks == XK_Meta_L || ks == XK_Meta_R)
          {
          metakey = 1;
          ks = 0;
          }
        }
      finish = clock_ticks();
      if(finish - start > 5)
        timed_out = 1;
      }
    keybuffer = ks;
    return keypressed();
    }

  if(XCheckTypedWindowEvent(display,XtWindow(canvas),KeyRelease,&event))
    {
    XKeyEvent *key_event = (XKeyEvent *) &event;
    len = XLookupString(key_event,buf,128,&ks,(XComposeStatus *) NULL);
    if(ks == CTRL_LEFT || ks == CTRL_RIGHT)
      {
      ctrlkey = 0;
      ks = 0;
      }
    else if(ks == SHIFT_LEFT || ks == SHIFT_RIGHT)
      {
      shiftkey = 0;
      ks = 0;
      }
    else if(ks == XK_Alt_L || ks == XK_Alt_R)
      {
      altkey = 0;
      ks = 0;
      }
    else if(ks == XK_Meta_L || ks == XK_Meta_R)
      {
      metakey = 0;
      ks = 0;
      }
    }
  else if(XCheckTypedWindowEvent(display,XtWindow(canvas),KeyPress,&event))
    {
    XKeyEvent *key_event = (XKeyEvent *) &event;
    len = XLookupString(key_event,buf,128,&ks,(XComposeStatus *) NULL);

    if(ks == CTRL_LEFT || ks == CTRL_RIGHT)
      {
      ctrlkey = 1;
      ks = 0;
      }

    if(ks == SHIFT_LEFT || ks == SHIFT_RIGHT)
      {
      shiftkey = 1;
      ks = 0;
      }
    else if(ks == XK_Alt_L || ks == XK_Alt_R)
      {
      altkey = 1;
      ks = 0;
      }
    else if(ks == XK_Meta_L || ks == XK_Meta_R)
      {
      metakey = 1;
      ks = 0;
      }
    }
  keybuffer = ks;
  return keypressed();
}

KeySym check_key()
{
  waitkeypressed(0);
  if(keybuffer == XK_d || keybuffer == SPACE ||
     keybuffer == SPACE2 || jiiming)
    {
    keybuffer = 0;
    }
  return keybuffer;
}

KeySym keypressed()
{
  return keybuffer;
}

long clock_ticks()
{
#ifdef HAVE_GETTIMEOFDAY
  struct timeval tim;

#ifdef GETTIMEOFDAY_HAS_ONE_PARA
  gettimeofday(&tim);
#else
  struct timezone tzp;
  gettimeofday(&tim, &tzp);
#endif
  return tim.tv_sec*clk_ticks + tim.tv_usec*clk_ticks/1000000;
#else /* HAVE_GETTIMEOFDAY */
  return -1;
#endif
}

KeySym getakey()
{
  int ret = keybuffer;
  keybuffer = 0;
  return ret;
}

KeySym havekey()
{
  int ret = keybuffer;
  return ret;
}

/*ARGSUSED*/
void response(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  XtPopdown(XtParent(w));
}

/*ARGSUSED*/
void modal_response(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int *answer = (int *) client_data;

  XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;

  switch(cbs->reason)
    {
    case XmCR_OK:
      *answer = 1;
      break;
    case XmCR_CANCEL:
      *answer = 2;
      break;
    default:
      *answer = 0;
      return;
    }
}

/*ARGSUSED*/
void fsb_no_match(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  int ret;
  char *fname;
  char msg[128];

  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &fname);

  sprintf((char *) &msg, "Can't open \"%s\".\n", fname);

  ret = confirm(main_window, (char *) &msg,
    OK_BTN, XmDIALOG_OK_BUTTON);
}

/*ARGSUSED*/
void buzzer(int buzzertype)
{
  XBell(display, 100);
}

/* convert corners to center/mag */
int cvtcentermag(double *Xctr, double *Yctr, double *Magnification)
{
  double Width, Height, Radius, Ratio;

  Width = xxmax - xxmin;
  Height = yymax - yymin;
  Ratio = Height / Width;

  if(xx3rd != xxmin || yy3rd != yymin || Width < 0 ||
    (Width > 1e-8 && (Ratio <= 0.74 || Ratio >= 0.76)) ||
    Ratio < 0.66 || Ratio > 0.84)
    {
    return(0);
    }

  /* calculate center and magnification */

  Radius = Height / 2.0;
  *Xctr = xxmin + (Width / 2.0);
  *Yctr = yymin + Radius;
  *Magnification = 1.0 / Radius;

  return(1);
}

/* set array of trig function indices according to "function=" command */
int set_trig_array(int k, char *name)
{
  char trigname[10];
  int i;
  char *slash;
  strncpy(trigname,name,6);
  trigname[6] = 0; /* safety first */

  if ((slash = strchr(trigname,'/')))
    *slash = 0;

  strlwr(trigname);

  for(i=0;i<numtrigfn;i++)
    {
    if((int)strcmp(trigname,trigfn[i].name)==0)
      {
      trigndx[k] = i;
      set_trig_pointers(k);
      break;
      }
    }
  return(0);
}

void set_trig_pointers(int which)
{
  /* set trig variable functions to avoid array lookup time */
   int i;
  switch(which)
    {
    case 0:
      dtrig0 = trigfn[trigndx[0]].dfunct;
      sprintf(curtrigndx_str[0], trigfn[trigndx[0]].name);
      break;
    case 1:
      dtrig1 = trigfn[trigndx[1]].dfunct;
      sprintf(curtrigndx_str[1], trigfn[trigndx[1]].name);
      break;
    case 2:
      dtrig2 = trigfn[trigndx[2]].dfunct;
      sprintf(curtrigndx_str[2], trigfn[trigndx[2]].name);
      break;
    case 3:
      dtrig3 = trigfn[trigndx[3]].dfunct;
      sprintf(curtrigndx_str[3], trigfn[trigndx[3]].name);
      break;
    default: /* do 'em all */
      for(i=0;i<4;i++)
        {
        set_trig_pointers(i);
        }
    break;
    }
}

char *strlwr(char *s)
{
  register char *sptr=s;
  while (*sptr != '\0')
    {
    if (isupper(*sptr))
      *sptr = tolower(*sptr);
    sptr++;
    }
  return s;
}

int find_file_item(char *filename,char *itemname,FILE **infile)
{
  char tmpname[41];
  char fullpathname[MAXPATHLEN];
  long notepoint;
  char buf[201];
  int c;
  char s_cantopen[] = {"Can't open %s"};
  findpath(filename, fullpathname);
  if ((*infile = fopen(fullpathname,"rt")) == NULL)
    {
    sprintf(buf,s_cantopen,fullpathname);
    caution(main_window, (char *) &buf, NULL);
    return(-1);
    }

  while (1)
    {
    while ((c = getc(*infile)) == ' ' || c == '\t' || c == '\n') { }
    if (c == EOF)
      break;
    if (c == ';')
      {
      while ((c = fgetc(*infile)) != '\n' && c != EOF) { }
      if (c == EOF)
        break;
      continue;
      }
    notepoint = ftell(*infile) - 1;
    ungetc(c,*infile);
    if (fscanf(*infile," %40[^ \n\t({]",tmpname) == EOF)
      break;
    while ((c = getc(*infile)) != EOF && c != '{' && c != '\n') { }
    if (c == EOF)
      break;
    if (c == '{')
      {
      if (stricmp(tmpname,itemname) == 0)
        {
        fseek(*infile,notepoint,SEEK_SET);
        return(0);
        }
      while ((c = getc(*infile)) != '}' && c != EOF) { }
      if (c == EOF)
        break;
      }
    }
  fclose(*infile);
  sprintf(buf,"'%s' definition not found",itemname);
  inform(main_window, (char *) &buf, NULL);
  return(-1);
}


void findpath(char *filename, char *fullpathname)
{
  /* If absolute path given or path starts with ./ use that one.
     If filename can be accessed use filename as given.
     Don't change path if fractdir is not given.  */
  if ((filename[0] == '/') ||
      ((filename[0] == '.') && (filename[1] == '/')) ||
      (access(filename, 0) == 0) ||
      (fractdir == NULL))
    {
    strcpy(fullpathname, filename);
    }
  else
    {
    /* prepend fractdir to filename */
    sprintf(fullpathname, "%s/%s", fractdir, filename);
    }
  return;
}

long stackavail()
{
#ifndef RLIM_INFINITY
  return 8192;
#else
  struct rlimit rlim;
  int ret;

  ret = getrlimit(RLIMIT_STACK, &rlim);
  if (ret == -1) {
    char *msg = "Can't get stacksize. Use 8192 instead\n";

    caution(main_window, (char *) &msg, NULL);
    return 8192;
  }

  return(rlim.rlim_cur);
#endif
}

int check_orbit_name(char *orbitname)
{
  int i, numtypes, bad;
  char *nameptr[MAXFRACTALS];
  int fractals[MAXFRACTALS];
  int last_val;

  numtypes = build_fractal_list(fractals, &last_val, nameptr);
  bad = 1;
  for(i=0;i<numtypes;i++)
    {
    if((int)strcmp(orbitname,nameptr[i]) == 0)
      {
      neworbittype = fractals[i];
      bad = 0;
      break;
      }
    }
  return(bad);
}

int build_fractal_list(int fractals[], int *last_val, char *nameptr[])
{
  int numfractals,i;
  extern int num_fractal_types;

  numfractals = 0;
  for (i = 0; i < num_fractal_types; i++)
    {
    int isinteger;
    isinteger = curfractalspecific->isinteger;
    if ((fractalspecific[i].flags & OKJB) && *fractalspecific[i].name != '*')
      {
      fractals[numfractals] = i;
      if (i == neworbittype || i == fractalspecific[neworbittype].tofloat)
        *last_val = numfractals;
      nameptr[numfractals] = fractalspecific[i].name;
      numfractals++;
      if (numfractals >= MAXFRACTALS)
        break;
      }
    }
  return (numfractals);
}

int check_writefile(char *name,char *ext)
{
  /* after v16 release, change encoder.c to also use this routine */
  char openfile[MAXPATHLEN];
  char opentype[20];
  int i;
nextname:
  strcpy(openfile,name);
  strcpy(opentype,ext);
  for (i = 0; i < (int)(strlen(openfile)); i++)
    if (openfile[i] == '.') {
      strcpy(opentype,&openfile[i]);
    openfile[i] = 0;
    }
  strcat(openfile,opentype);
  if (access(openfile,0) != 0) /* file doesn't exist */
    {
    strcpy(name,openfile);
    return 0;
    }
  /* file already exists */
  if (overwrite_flag == 0)
    {
    updatesavename(name);
    goto nextname;
    }
  return 1;
}

long normalize(char *ptr)
{
  return (long) ptr;
}

void notdiskmsg()
{
  char msg[128];

  sprintf((char *) &msg, "This may take a while... (Press ESC to abort)");

  if(main_window && XtIsManaged(main_window))
    inform(main_window, (char *) &msg, NULL);
  else
    fprintf(stderr, "%s\n", (char *) &msg);
}

void create_xor_gc()
{
  XGCValues values;

  if(xgc)
    XFreeGC(display, xgc);

  find_special_colors();

  XGetGCValues(display, gc, GCFunction | GCLineStyle, &values);

  values.function = GXxor;
  values.line_style = LineOnOffDash;

  xgc = XCreateGC(XtDisplay(canvas), XtWindow(canvas),
    GCFunction | GCLineStyle, &values);

  XSetForeground(display, xgc, cols[zoomcolor_index].pixel);

  XFlush(display);
}

void change_xor_gc(int do_line)
{
  XGCValues values;

  XGetGCValues(display, xgc,
    GCFunction | GCLineStyle,
    &values);

  values.function = GXxor;

  if(do_line == 1)
    {
    values.line_style = LineOnOffDash;
    XChangeGC(XtDisplay(canvas), xgc,
      GCFunction | GCLineStyle, &values);
    }
  else if(do_line == 2)
    {
    values.line_style = LineSolid;
    XChangeGC(XtDisplay(canvas), xgc,
      GCFunction | GCLineStyle, &values);
    }
  else
    {
    XChangeGC(XtDisplay(canvas), xgc,
      GCFunction, &values);
    }

  XFlush(display);
}

/*
 * The following routines are used for encoding/decoding gif images.
 * If we aren't on a PC, things are rough for decoding the fractal info
 * structure in the GIF file.  These routines look after converting the
 * MS_DOS format data into a form we can use.
 * If dir==0, we convert to MSDOS form.  Otherwise we convert from MSDOS.
 */

void decode_fractal_info(info,dir)
  struct fractal_info *info;
  int dir;
{
  unsigned char *buf;
  unsigned char *bufPtr;
  int i;

  if (dir==1)
    {
    buf = (unsigned char *)XtMalloc(FRACTAL_INFO_SIZE);
    bufPtr = buf;
    memcpy((char *)buf,(char *)info,FRACTAL_INFO_SIZE);
    }
  else
    {
    buf = (unsigned char *)XtMalloc(sizeof(struct fractal_info));
    bufPtr = buf;
    memcpy((char *)buf,(char *)info,sizeof(struct fractal_info));
    }

  if (dir==1)
    {
    strncpy(info->info_id, (char *) bufPtr, 8);
    }
  else
    {
    strncpy((char *)bufPtr, (char *)info->info_id, 8);
    }
  bufPtr += 8;
  getInt(&info->iterations,&bufPtr,dir);
  getInt(&info->fractal_type,&bufPtr,dir);
  getDouble(&info->xmin,&bufPtr,dir);
  getDouble(&info->xmax,&bufPtr,dir);
  getDouble(&info->ymin,&bufPtr,dir);
  getDouble(&info->ymax,&bufPtr,dir);
  getDouble(&info->creal,&bufPtr,dir);
  getDouble(&info->cimag,&bufPtr,dir);
  getInt(&info->videomodeax,&bufPtr,dir);
  getInt(&info->videomodebx,&bufPtr,dir);
  getInt(&info->videomodecx,&bufPtr,dir);
  getInt(&info->videomodedx,&bufPtr,dir);
  getInt(&info->dotmode,&bufPtr,dir);
  getInt(&info->xdots,&bufPtr,dir);
  getInt(&info->ydots,&bufPtr,dir);
  getInt(&info->colors,&bufPtr,dir);
  getInt(&info->version,&bufPtr,dir);
  getFloat(&info->parm3,&bufPtr,dir);
  getFloat(&info->parm4,&bufPtr,dir);
  getFloat(&info->potential[0],&bufPtr,dir);
  getFloat(&info->potential[1],&bufPtr,dir);
  getFloat(&info->potential[2],&bufPtr,dir);
  getInt(&info->rseed,&bufPtr,dir);
  getInt(&info->rflag,&bufPtr,dir);
  getInt(&info->biomorph,&bufPtr,dir);
  getInt(&info->inside,&bufPtr,dir);
  getInt(&info->logmap,&bufPtr,dir);
  getFloat(&info->invert[0],&bufPtr,dir);
  getFloat(&info->invert[1],&bufPtr,dir);
  getFloat(&info->invert[2],&bufPtr,dir);
  getInt(&info->decomp[0],&bufPtr,dir);
  getInt(&info->decomp[1],&bufPtr,dir);
  getInt(&info->symmetry,&bufPtr,dir);
  for (i=0;i<16;i++)
    {
    getInt(&info->init3d[i],&bufPtr,dir);
    }
  getInt(&info->previewfactor,&bufPtr,dir);
  getInt(&info->xtrans,&bufPtr,dir);
  getInt(&info->ytrans,&bufPtr,dir);
  getInt(&info->red_crop_left,&bufPtr,dir);
  getInt(&info->red_crop_right,&bufPtr,dir);
  getInt(&info->blue_crop_left,&bufPtr,dir);
  getInt(&info->blue_crop_right,&bufPtr,dir);
  getInt(&info->red_bright,&bufPtr,dir);
  getInt(&info->blue_bright,&bufPtr,dir);
  getInt(&info->xadjust,&bufPtr,dir);
  getInt(&info->eyeseparation,&bufPtr,dir);
  getInt(&info->glassestype,&bufPtr,dir);
  getInt(&info->outside,&bufPtr,dir);
  getDouble(&info->x3rd,&bufPtr,dir);
  getDouble(&info->y3rd,&bufPtr,dir);
  getChar((unsigned char *)&info->stdcalcmode,(unsigned char **)&bufPtr,dir);
  getChar((unsigned char *)&info->useinitorbit,(unsigned char **)&bufPtr,dir);
  getInt((short *)&info->calc_status,&bufPtr,dir);
  getLong(&info->tot_extend_len,&bufPtr,dir);
  getInt(&info->distest,&bufPtr,dir);
  getInt(&info->floatflag,&bufPtr,dir);
  getInt(&info->bailout,&bufPtr,dir);
  getLong(&info->calctime,&bufPtr,dir);
  for (i=0;i<4;i++)
    {
    getChar((unsigned char *)&info->trigndx[i],(unsigned char **)&bufPtr,dir);
    }
  getInt(&info->finattract,&bufPtr,dir);
  getDouble(&info->initorbit[0],&bufPtr,dir);
  getDouble(&info->initorbit[1],&bufPtr,dir);
  getInt(&info->periodicity,&bufPtr,dir);
  getInt(&info->pot16bit,&bufPtr,dir);
  getFloat(&info->faspectratio,&bufPtr,dir);
  getInt(&info->system,&bufPtr,dir);
  getInt(&info->release,&bufPtr,dir);
  getInt(&info->flag3d,&bufPtr,dir);
  getInt(&info->transparent[0],&bufPtr,dir);
  getInt(&info->transparent[1],&bufPtr,dir);
  getInt(&info->ambient,&bufPtr,dir);
  getInt(&info->haze,&bufPtr,dir);
  getInt(&info->randomize,&bufPtr,dir);
  getInt(&info->rotate_lo,&bufPtr,dir);
  getInt(&info->rotate_hi,&bufPtr,dir);
  getInt(&info->distestwidth,&bufPtr,dir);
  getDouble(&info->dparm3,&bufPtr,dir);
  getDouble(&info->dparm4,&bufPtr,dir);
  getInt(&info->fillcolor,&bufPtr,dir);
  getDouble(&info->mxmaxfp,&bufPtr,dir);
  getDouble(&info->mxminfp,&bufPtr,dir);
  getDouble(&info->mymaxfp,&bufPtr,dir);
  getDouble(&info->myminfp,&bufPtr,dir);
  getInt(&info->zdots,&bufPtr,dir);
  getFloat(&info->originfp,&bufPtr,dir);
  getFloat(&info->depthfp,&bufPtr,dir);
  getFloat(&info->heightfp,&bufPtr,dir);
  getFloat(&info->widthfp,&bufPtr,dir);
  getFloat(&info->distfp,&bufPtr,dir);
  getFloat(&info->eyesfp,&bufPtr,dir);
  getInt(&info->orbittype,&bufPtr,dir);
  getInt(&info->juli3Dmode,&bufPtr,dir);
  getInt(&info->maxfn,&bufPtr,dir);
  getInt(&info->inversejulia,&bufPtr,dir);
  getDouble(&info->dparm5,&bufPtr,dir);
  getDouble(&info->dparm6,&bufPtr,dir);
  getDouble(&info->dparm7,&bufPtr,dir);
  getDouble(&info->dparm8,&bufPtr,dir);
  getDouble(&info->dparm9,&bufPtr,dir);
  getDouble(&info->dparm10,&bufPtr,dir);
  for (i=0;i<50;i++)
    {
    getInt(&info->future[i],&bufPtr,dir);
    }
  if (bufPtr-buf != FRACTAL_INFO_SIZE)
    {
    printf("Warning: loadfile miscount on fractal_info structure.\n");
    printf("Components add up to %d bytes, but FRACTAL_INFO_SIZE = %d\n",
        bufPtr-buf, FRACTAL_INFO_SIZE);
    }
  if (dir==0)
    {
    memcpy((char *)info,(char *)buf,FRACTAL_INFO_SIZE);
    }
  XtFree((char *) buf);
}

/*
 * This routine gets a char out of the buffer.
 * It updates the buffer pointer accordingly.
 */

static void getChar(unsigned char *dst, unsigned char **src, int dir)
{
  if (dir==1)
    {
    *dst = **src;
    }
  else
    {
    **src = *dst;
    }
  (*src)++;
}

/*
 * This routine gets an int out of the buffer.
 * It updates the buffer pointer accordingly.
 */

static void getInt(short *dst, unsigned char **src, int dir)
{
  if (dir==1)
    {
    *dst = (*src)[0] + ((((char *)(*src))[1])<<8);
    }
  else
    {
    (*src)[0] = (*dst)&0xff;
    (*src)[1] = ((*dst)&0xff00)>>8;
    }
  (*src) += 2; /* sizeof(int) in MS_DOS */
}

/*
 * This routine gets a long out of the buffer.
 * It updates the buffer pointer accordingly.
 */

static void getLong(long *dst, unsigned char **src, int dir)
{
  if (dir==1)
    {
    *dst = ((unsigned long)((*src)[0])) +
          (((unsigned long)((*src)[1]))<<8) +
          (((unsigned long)((*src)[2]))<<16) +
          (((long)(((char *)(*src))[3]))<<24);
    }
  else
    {
    (*src)[0] = (*dst)&0xff;
    (*src)[1] = ((*dst)&0xff00)>>8;
    (*src)[2] = ((*dst)&0xff0000)>>8;
    (*src)[3] = ((unsigned)(*dst)&0xff000000)>>8;
    }
  (*src) += 4; /* sizeof(long) in MS_DOS */
}

/*
 * This routine gets a double out of the buffer, or puts a double into the
 * buffer;
 * It updates the buffer pointer accordingly.
 */

static void getDouble(double *dst, unsigned char **src, int dir)
{
  int e;
  double f;
  int i;
  if (dir==1)
    {
    for (i=0;i<8;i++)
      {
      if ((*src)[i] != 0)
        {
        break;
        }
      }
    if (i==8)
      {
      *dst = 0;
      }
    else
      {
      e = (((*src)[7]&0x7f)<<4) + ((int)((*src)[6]&0xf0)>>4) - 1023;
      f = (double) (1 + (int)((*src)[6]&0x0f)/16.0
                      + (int)(*src)[5]/4096.0
                      + (int)(*src)[4]/1048576.0
                      + (int)(*src)[3]/268435456.0
                      + (int)(*src)[2]/68719476736.0
                      + (int)(*src)[1]/17592186044416.0
                      + (int)(*src)[0]/4503599627370496.0);
      f *= pow(2.,(double)e);
      if ((*src)[7]&0x80)
        {
        f = -f;
        }
      *dst = f;
      }
    }
  else
    {
    if (*dst==0)
      {
      memset((char *)(*src), 0, 8);
      }
    else
      {
      int s=0;
      f = *dst;
      if (f<0)
        {
        s = 0x80;
        f = -f;
        }
      e = log(f)/log(2.);
      f = f/pow(2.,(double)e) - 1;
      if (f<0)
        {
        e--;
        f = (f+1)*2-1;
        }
      else if (f>=1)
        {
        e++;
        f = (f+1)/2-1;
        }
      e += 1023;
      (*src)[7] = s | ((e&0x7f0)>>4);
      f *= 16;
      (*src)[6] = ((e&0x0f)<<4) | (((int)f)&0x0f);
      f = (f-(int)f)*256;
      (*src)[5] = (((int)f)&0xff);
      f = (f-(int)f)*256;
      (*src)[4] = (((int)f)&0xff);
      f = (f-(int)f)*256;
      (*src)[3] = (((int)f)&0xff);
      f = (f-(int)f)*256;
      (*src)[2] = (((int)f)&0xff);
      f = (f-(int)f)*256;
      (*src)[1] = (((int)f)&0xff);
      f = (f-(int)f)*256;
      (*src)[0] = (((int)f)&0xff);
      }
    }
  *src += 8; /* sizeof(double) in MSDOS */
}

/*
 * This routine gets a float out of the buffer.
 * It updates the buffer pointer accordingly.
 */

static void getFloat(float *dst, unsigned char **src, int dir)
{
  int e;
  double f;
  int i;
  if (dir==1)
    {
    for (i=0;i<4;i++)
      {
      if ((*src)[i] != 0)
        {
        break;
        }
      }
    if (i==4)
      {
      *dst = 0;
      }
    else
      {
      e = (((((int)(*src)[3]&0x7f)<<1)| (((int)(*src)[2]&0x80)>>7)) - 127);
      f = 1 + ((int)(*src)[2]&0x7f)/128 +
              (int)(*src)[1]/32768 + (int)(*src)[0]/8388608;
      f *= pow(2.,(double)e);
      if ((*src)[3]&0x80)
        {
        f = -f;
        }
      *dst = f;
      }
    }
  else
    {
    if (*dst==0)
      {
      memset((char *)(*src), 0, 4);
      }
    else
      {
      int s=0;
      f = *dst;
      if (f<0)
        {
        s = 0x80;
        f = -f;
        }
      e = log(f)/log(2.);
      f = f/pow(2.,(double)e) - 1;
      if (f<0)
        {
        e--;
        f = (f+1)*2-1;
        }
      else if (f>=1)
        {
        e++;
        f = (f+1)/2-1;
        }
      e += 127;
      (*src)[3] = s | ((e&0xf7)>>1);
      f *= 128;
      (*src)[2] = ((e&0x01)<<7) | (((int)f)&0x7f);
      f = (f-(int)f)*256;
      (*src)[1] = (((int)f)&0xff);
      f = (f-(int)f)*256;
      (*src)[0] = (((int)f)&0xff);
      }
    }
  *src += 4; /* sizeof(float) in MSDOS */
}


/*
 * Fix up the ranges data.
 */

void fix_ranges(int *ranges, int num, int dir)
{
  unsigned char *buf;
  unsigned char *bufPtr;
  int i;

  if (dir==1)
    {
    buf = (unsigned char *)XtMalloc(num*2);
    bufPtr = buf;
    memcpy((char *)buf, (char *)ranges, num*2);
    }
  else
    {
    buf = (unsigned char *)XtMalloc(num*sizeof(int));
    bufPtr = buf;
    memcpy((char *)buf, (char *)ranges, num*sizeof(int));
    }
  for (i=0;i<num;i++)
    getInt((short *)&ranges[i],(unsigned char **)bufPtr, dir);

  XtFree((char *)buf);
}

void
display_file()
{
  XGCValues values;

  if(showfile == 0)
    {
    XDefineCursor(display, window,
      XCreateFontCursor(display, XC_watch));
    clear_window();
    ok_to_cycle = 0;
    sxmin = xxmin;
    sxmax = xxmax;
    sx3rd = xx3rd;
    symax = yymax;
    symin = yymin;
    sy3rd = yy3rd;
    funny_glasses_call(gifview1);
    XDefineCursor(display, window,
      XCreateFontCursor(display, XC_top_left_arrow));
    zoomoff = 0;
    ok_to_cycle = 1;

    /* initialize the history file */

    if (history[0].fractype == -1)
      {
      int i;
      histocycle = 0;
      for (i = 0; i < MAXHISTORY; i++)
        {
        int j;
        historymax = -1;
        history[i].xxmax = xxmax;
        history[i].xxmin = xxmin;
        history[i].yymax = yymax;
        history[i].yymin = yymin;
        history[i].xx3rd = xx3rd;
        history[i].yy3rd = yy3rd;

        for(j=0;j<MAXPARAMS;j++)
          {
          history[i].param[j] = param[j];
          }

        history[i].fractype = fractype;
        }
      }

    if(historyflag == 0 &&
      ((calc_status < IMG_ACTV_CALC || calc_status > IMG_INTR_NRES) ||
      (rubber_band_data.panning == 1)))
      {
      int j;

      rubber_band_data.panning = 0;

      if(++historyptr == MAXHISTORY)
        {
        historymax = MAXHISTORY;
        historyptr = 0;
        histocycle = 1;
        }
      else if(historymax == MAXHISTORY)
        {
        if(historyptr >= MAXHISTORY)
          historyptr = 0;
        }
      else
        {
        historyptr = ++historymax;
        if(historyptr >= MAXHISTORY)
          {
          historymax = MAXHISTORY;
          historyptr = 0;
          histocycle = 1;
          }
        }

      history[historyptr].xxmax = xxmax;
      history[historyptr].xxmin = xxmin;
      history[historyptr].yymax = yymax;
      history[historyptr].yymin = yymin;
      history[historyptr].xx3rd = xx3rd;
      history[historyptr].yy3rd = yy3rd;

      for(j=0;j<MAXPARAMS;j++)
        history[historyptr].param[j] = param[j];

      history[historyptr].fractype = fractype;
      }

    update_restore_menu();

    buzzer(0);
    return;
    }
}

void showtrig(char *buf) /* return display form of active trig functions */
{
  char tmpbuf[30];
  *buf = 0; /* null string if none */
  trigdetails(tmpbuf);
  if (tmpbuf[0])
    sprintf(buf," function=%s",tmpbuf);
}

static void trigdetails(char *buf)
{
  int i, numfn;
  char tmpbuf[20];
  if(fractype==JULIBROT || fractype==JULIBROTFP)
    numfn = (fractalspecific[neworbittype].flags >> 6) & 7;
  else
    numfn = (curfractalspecific->flags >> 6) & 7;
  if(curfractalspecific == &fractalspecific[FORMULA] ||
    curfractalspecific == &fractalspecific[FFORMULA]  )
    {
    numfn = maxfn;
    }
  *buf = 0; /* null string if none */
  if (numfn>0)
    {
    strcpy(buf,trigfn[trigndx[0]].name);
    i = 0;
    while(++i < numfn)
      {
      sprintf(tmpbuf,"/%s",trigfn[trigndx[i]].name);
      strcat(buf,tmpbuf);
      }
    }
}

/*ARGSUSED*/
void map_dialog(
Widget dialog,
XtPointer client_data,
XtPointer call_data)
{
  Position main_x, main_y;
  Position new_x, new_y;
  Dimension main_w, main_h;
  Dimension dlog_w, dlog_h;
  int scrn_width;
  int scrn_height;

  scrn_width = DisplayWidth(display, screen_number);
  scrn_height = DisplayHeight(display, screen_number);

  XtVaGetValues(main_window,
    XmNx, &main_x,
    XmNy, &main_y,
    XmNwidth, &main_w,
    XmNheight, &main_h,
    NULL);

  XtVaGetValues(dialog,
    XmNwidth, &dlog_w,
    XmNheight, &dlog_h,
    NULL);

  if((int)main_w >= (int)scrn_width ||
     (int)main_h >= (int)scrn_height)
    {
    new_x = (int)((int)(scrn_width - dlog_w) / 2);
    new_y = (int)((int)(scrn_height - dlog_h) / 2);
    }
  else
    {
    new_x = (int)((int)(main_x + ((int)(main_w - dlog_w)) / 2));
    new_y = (int)((int)(main_y + ((int)(main_h - dlog_h)) / 2));
    }

  XtVaSetValues(dialog,
    XmNx, new_x,
    XmNy, new_y,
    NULL);
}

int image_load_error()
{
  char tempstr[256];
  int ret;

  sprintf((char *) &tempstr,
"The image size does not match the screen size.\n\nWindow size = %4d x %4d\nImage size  = %4d x %4d\n\nResize the window for this image?", xdots, ydots, filexdots, fileydots);

  ret = confirm(main_window, tempstr,
    OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);

  return(ret);
}

int resize_for_new_image(int new_width, int new_height)
{
  int i;
  int ret;
  int new_x_pos;
  int new_y_pos;
  int bigger;
  int gcd = 1;
#ifdef HAVE_DIV_T
  div_t res_1;
  div_t res_2;
#else
  long res_1;
  long res_2;
#endif
  XWindowChanges values;
  char msg[512];

  window_presets.new_width = new_width;
  window_presets.new_height = new_height;

  XGetGeometry(display,XtWindow(canvas),
    &(window_presets.rootw),
    &(window_presets.width_addition), &(window_presets.height_addition),
    &(window_presets.width_return), &(window_presets.height_return),
    &(window_presets.border_width_return), &(window_presets.depth_return));

  window_presets.oversize = 0;

  window_presets.display_width = DisplayWidth(display, screen_number);
  window_presets.display_height = DisplayHeight(display, screen_number);

  if(window_presets.display_width != window_presets.display_height)
    bigger = min(window_presets.display_width, window_presets.display_height);
  else
    bigger = window_presets.display_width;

  if(bigger == window_presets.display_width)
    {
    for(i = 1; i <= window_presets.display_width; i++)
      {
#ifdef HAVE_DIV_T
      res_1 = div(window_presets.display_width,i);
      if(res_1.rem == 0)
        {
        res_2 = div(window_presets.display_height,i);
        if(res_2.rem == 0)
          gcd = i;
        }
#else
      res_1 = window_presets.display_width % i;
      if(res_1 == 0)
        {
        res_2 = window_presets.display_height % i;
        if(res_2 == 0)
          gcd = i;
        }
#endif
      }
    }
  else
    {
    for(i = 1; i <= window_presets.display_height; i++)
      {
#ifdef HAVE_DIV_T
      res_1 = div(window_presets.display_height,i);
      if(res_1.rem == 0)
        {
        res_2 = div(window_presets.display_width,i);
        if(res_2.rem == 0)
          gcd = i;
        }
#else
      res_1 = window_presets.display_height % i;
      if(res_1 == 0)
        {
        res_2 = window_presets.display_width % i;
        if(res_2 == 0)
          gcd = i;
        }
#endif
      }
    }

  window_presets.aspect_width = window_presets.display_width / gcd;
  window_presets.aspect_height = window_presets.display_height / gcd;

  window_presets.vis_max_width =
    window_presets.display_width - window_presets.width_addition;
  window_presets.vis_max_height =
    window_presets.display_height - window_presets.height_addition;

  if((window_presets.new_width > window_presets.vis_max_width) ||
     (window_presets.new_height > window_presets.vis_max_height))
    {
    if(verbose)
      {
      sprintf((char *) &msg,
"New window size may be too large for this screen.\nMaximum values for a fully visible image are:\n\nWidth:  %4d\nHeight: %4d\n\nThese values are based on the current\nmenu bar height of %d pixels.\n\nResize anyway?\n",
      window_presets.vis_max_width, window_presets.vis_max_height,
      window_presets.height_addition);
      ret = confirm(main_window, (char *) &msg,
        OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
      }
    else
      ret = 1;

    if(ret != 1)
      return 1;
    else
      window_presets.oversize = 1;
    }

  values.width = window_presets.new_width + window_presets.width_addition;
  values.height = window_presets.new_height + window_presets.height_addition;

  if(!(window_presets.oversize))
    {
    new_x_pos = window_presets.vis_max_width - values.width;
    new_y_pos = window_presets.vis_max_height - values.height;

    new_x_pos /= 2;
    new_y_pos /= 2;
    }
  else
    {
    new_x_pos = 0;
    new_y_pos = 0;
    }

  values.x = new_x_pos;
  values.y = new_y_pos;

  XReconfigureWMWindow(display, window, screen_number,
    CWX|CWY|CWWidth|CWHeight, &values);

  XtAppAddTimeOut(app, 250, (XtTimerCallbackProc) check_geometry, NULL);

  sxdots = new_width;
  sydots = new_height;
  videotable[0].xdots = sxdots;
  videotable[0].ydots = sydots;
  xdots = sxdots;
  ydots = sydots;
  dx_size = xdots - 1;
  dy_size = ydots - 1;
  oldx = sxdots;
  oldy = sydots;
  winwidth = sxdots;
  winheight = sydots;
  screenaspect = sydots/(float)sxdots;
  finalaspectratio = screenaspect;

  if (pixbuf != NULL)
    XtFree((char *) pixbuf);

  pixbuf = (unsigned char *) XtMalloc(winwidth *sizeof(unsigned char));

  if (image != NULL)
    {
    XtFree(image->data);
    XFree((void *) image);
    }
  /* Borrowed from http://pficheux.free.fr/articles/lmf/xlib-images/ */
  image = XCreateImage(display,
    visual,
    XDefaultDepth(display, screen_number),
		       ZPixmap, 0, NULL, sxdots, sydots, 8*sizeof(unsigned int), 0);

  if (image == NULL)
    {
    sprintf(msg, "%s", "XCreateImage failed.");
    fatal(main_window, (char *) &msg);
    exit(-1);
    }

  image->data = XtMalloc(image->bytes_per_line * image->height);

  if (image->data==NULL)
    {
    sprintf(msg,"Malloc failed: (Ximage data: size:%d\n",
    image->bytes_per_line * image->height);
    fatal(main_window, (char *) &msg);
    exit(-1);
    }

  XSync(display,False);
  resized_by_user = 2;
  return 0;
}

void get_winsize_defaults()
{
  int i;
  int gcd = 0;
  int done;
  int bigger;
#ifdef HAVE_DIV_T
  div_t res_1;
  div_t res_2;
#else
  long res_1;
  long res_2;
#endif
  long usr_mask;
  XSizeHints size_hints;

  XGetWMNormalHints(display, XtWindow(canvas),
    &size_hints, &usr_mask);

  if(size_hints.base_width)
    window_presets.min_width = size_hints.base_width;
  else
    window_presets.min_width = size_hints.min_width;

  if(size_hints.base_height)
    window_presets.min_height = size_hints.base_height;
  else
    window_presets.min_height = size_hints.min_height;

  /*
   * well, that doesn't work with mwm on Solaris because Sun, for some
   * incomprehensible reason, runs mwm under OpenLook - a window manager
   * managing a window manager... the returns from XGetWMNormalHints
   * are bogus in that situation. Running mwm from a naked Xterm will
   * solve the problem but most people will follow the Sun doco and
   * run mwm like this so I'll just hard code the minimum limits...
   */

  window_presets.min_width = 80;
  window_presets.min_height = 60;

  window_presets.max_width = 2048;
  window_presets.max_height = 2048;

  size_hints.min_width = window_presets.min_width;
  size_hints.min_height = window_presets.min_height;

  size_hints.flags |= PMinSize;

  XSetWMNormalHints(display, XtWindow(canvas), &size_hints);

  XGetGeometry(display, XtWindow(canvas),
    &(window_presets.rootw),
    &(window_presets.width_addition), &(window_presets.height_addition),
    &(window_presets.width_return), &(window_presets.height_return),
    &(window_presets.border_width_return), &(window_presets.depth_return));

  window_presets.display_width = DisplayWidth(display, screen_number);
  window_presets.display_height = DisplayHeight(display, screen_number);

  window_presets.vis_max_width =
    window_presets.display_width - window_presets.width_addition;
  window_presets.vis_max_height =
    window_presets.display_height - window_presets.height_addition;

  if(window_presets.display_width != window_presets.display_height)
    bigger =
      min(window_presets.display_width, window_presets.display_height);
  else
    bigger = window_presets.display_width;

  if(bigger == window_presets.display_width)
    {
    for(i = 1; i <= window_presets.display_width; i++)
      {
#ifdef HAVE_DIV_T
      res_1 = div(window_presets.display_width,i);
      if(res_1.rem == 0)
        {
        res_2 = div(window_presets.display_height,i);
        if(res_2.rem == 0)
          gcd = i;
        }
#else
      res_1 = window_presets.display_width % i;
      if(res_1 == 0)
        {
        res_2 = window_presets.display_height % i;
        if(res_2 == 0)
          gcd = i;
        }
#endif
      }
    }
  else
    {
    for(i = 1; i <= window_presets.display_height; i++)
      {
#ifdef HAVE_DIV_T
      res_1 = div(window_presets.display_height,i);
      if(res_1.rem == 0)
        {
        res_2 = div(window_presets.display_width,i);
        if(res_2.rem == 0)
          gcd = i;
        }
#else
      res_1 = window_presets.display_height % i;
      if(res_1 == 0)
        {
        res_2 = window_presets.display_width % i;
        if(res_2 == 0)
          gcd = i;
        }
#endif
      }
    }

  window_presets.aspect_width = window_presets.display_width / gcd;
  window_presets.aspect_height = window_presets.display_height / gcd;

  window_presets.dis_tiny_width = window_presets.aspect_width;
  window_presets.dis_tiny_height = window_presets.aspect_height;

  done = 0;

  while(!done)
    {
    if(bigger == window_presets.display_width)
      {
      if(window_presets.dis_tiny_width < window_presets.min_width)
        {
        window_presets.dis_tiny_width += window_presets.aspect_width;
        window_presets.dis_tiny_height += window_presets.aspect_height;
        }
      else if(window_presets.dis_tiny_height < window_presets.min_height)
        {
        window_presets.dis_tiny_width += window_presets.aspect_width;
        window_presets.dis_tiny_height += window_presets.aspect_height;
        }
      else
        done = 1;
      }
    else
      {
      if(window_presets.dis_tiny_height < window_presets.min_height)
        {
        window_presets.dis_tiny_height += window_presets.aspect_height;
        window_presets.dis_tiny_width += window_presets.aspect_width;
        }
      else if(window_presets.dis_tiny_width < window_presets.min_width)
        {
        window_presets.dis_tiny_width += window_presets.aspect_width;
        window_presets.dis_tiny_height += window_presets.aspect_height;
        }
      else
        done = 1;
      }
    }

  window_presets.dis_small_width = window_presets.aspect_width;
  window_presets.dis_small_height = window_presets.aspect_height;

  done = 0;

  while(!done)
    {
    if(bigger == window_presets.display_width)
      {
      if(window_presets.dis_small_width < (window_presets.vis_max_width / 2))
        {
        window_presets.dis_small_width += window_presets.aspect_width;
        window_presets.dis_small_height += window_presets.aspect_height;
        }
      else
        done = 1;
      }
    else
      {
      if(window_presets.dis_small_height < (window_presets.vis_max_height / 2))
        {
        window_presets.dis_small_height += window_presets.aspect_height;
        window_presets.dis_small_width += window_presets.aspect_width;
        }
      else
        done = 1;
      }
    }

  window_presets.dis_large_width = window_presets.aspect_width;
  window_presets.dis_large_height = window_presets.aspect_height;

  done = 0;

  while(!done)
    {
    if(bigger == window_presets.display_width)
      {
      if((window_presets.dis_large_width + window_presets.aspect_width) <
          window_presets.vis_max_width)
        {
        if((window_presets.dis_large_height + window_presets.aspect_height) <
            window_presets.vis_max_height)
          {
          window_presets.dis_large_width += window_presets.aspect_width;
          window_presets.dis_large_height += window_presets.aspect_height;
          }
        else
          done = 1;
        }
      else
        done = 1;
      }
    else
      {
      if((window_presets.dis_large_height + window_presets.aspect_height) <
          window_presets.vis_max_height)
        {
        if((window_presets.dis_large_width + window_presets.aspect_width) <
            window_presets.vis_max_width)
          {
          window_presets.dis_large_height += window_presets.aspect_height;
          window_presets.dis_large_width += window_presets.aspect_width;
          }
        else
          done = 1;
        }
      else
        done = 1;
      }
    }
}

int startdisk()
{
  if (!diskisactive)
    return(0);
  return (common_startdisk(sxdots,sydots,colors));
}

/*ARGSUSED*/
int targa_startdisk(FILE *targafp,int overhead)
{
  int i;
  fp = targafp;
  i = common_startdisk(sxdots*3,sydots,colors);
  return (i);
}

/*ARGSUSED*/
int common_startdisk(int newrowsize, int newcolsize, int colors)
{
  long memorysize;

  if (diskflag)
    enddisk();
  memorysize = (long)(newcolsize) * newrowsize;
  diskflag = 1;
  rowsize = newrowsize;
  colsize = newcolsize;

  if (dataPtr != NULL)
    XtFree((char *) dataPtr);

  dataPtr = (unsigned char *)XtMalloc(memorysize);

  memset(dataPtr, 0, memorysize);

  return(0);
}

void enddisk()
{
  diskflag = rowsize = disk16bit = 0;
  fp = NULL;
}

int readdisk(int col, int row)
{
  if (row>=colsize || col>=rowsize)
    return 0;

  return dataPtr[row*rowsize+col];
}

int FromMemDisk(long offset, int size, void *dest)
{
  memcpy(dest, (void *) (dataPtr+offset), size);
  return 1;
}

void targa_readdisk(unsigned int col, unsigned int row,
            unsigned char *red, unsigned char *green, unsigned char *blue)
{
  col *= 3;
  *blue  = readdisk(col,row);
  *green = readdisk(++col,row);
  *red   = readdisk(col+1,row);
}

void writedisk(int col, int row, int color)
{
  if (row>=colsize || col>=rowsize)
    return;

  dataPtr[row*rowsize+col] = color;
  if (Shadowing)
    {
     unsigned Mask;
     Mask = (1 << AntiAliasing) - 1;
     if(!(col & Mask) && !(row & Mask))
       ShadowPutColor(col, row, color);
     }
}

int ToMemDisk(long offset, int size, void *src)
{
   memcpy((void *) (dataPtr+offset), src, size);
   return 1;
}

void targa_writedisk(unsigned int col, unsigned int row,
            unsigned char red, unsigned char green, unsigned char blue)
{
  writedisk(col*=3,row,blue);
  writedisk(++col, row,green);
  writedisk(col+1, row,red);
}

int pot_startdisk()
{
  int i;
  i = common_startdisk(sxdots,sydots<<1,colors);
  disk16bit = 1;
  return (i);
}

/*ARGSUSED*/
void load_videotable(int options)
{
  int i;
  int keyents;
  struct videoinfo *vident;

  vidtbl = (struct videoinfo *)
            XtMalloc(sizeof(struct videoinfo) * MAXVIDEOTABLE);

  vidtbllen = 0;
  vident = vidtbl;

  videotable[0].xdots = sxdots;
  videotable[0].ydots = sydots;
  videotable[0].colors = colors;
  videotable[0].dotmode = 19;

  for (i = 0; i < MAXVIDEOTABLE; ++i)
    {
    if (videotable[i].xdots)
      {
      memcpy((char *)vident, (char *)&videotable[i], sizeof(*vident));
      ++vident;
      ++vidtbllen;
      }
    }

  keyents = 0;

  memset((char *)videotable, 0, sizeof(*vidtbl) * MAXVIDEOTABLE);

  for (i = 0; i < vidtbllen; ++i)
    {
    if (vidtbl[i].keynum > 0)
      {
      memcpy((char *)&videotable[keyents], (char *)&vidtbl[i], sizeof(*vidtbl));      if (++keyents >= MAXVIDEOTABLE)
        {
        break;
        }
      }
    }

}

/*ARGSUSED*/
void kbd_event(
Widget w,
XtPointer client_data,
XEvent *event,
Boolean *dispatch)
{
  XKeyEvent *key_event = (XKeyEvent *)event;
  char buf[128];
  KeySym ks;
  int len;

  switch(key_event->type)
    {
    case KeyRelease:
      {
      XLookupString(key_event,buf,128,&ks,(XComposeStatus *) NULL);
      keybuf = buf[0];
      switch (ks)
        {
        case CTRL_LEFT:
        case CTRL_RIGHT:
          ctrlkey = 0;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;

        case SHIFT_LEFT:
        case SHIFT_RIGHT:
          shiftkey = 0;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;

        case XK_Meta_L:
        case XK_Meta_R:
          metakey = 0;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;

        case XK_Alt_L:
        case XK_Alt_R:
          altkey = 0;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;
        }
      }
    break;

    case KeyPress:
      {
      len = XLookupString(key_event,buf,128,&ks,(XComposeStatus *) NULL);

      switch(ks)
        {
        case CTRL_LEFT:
        case CTRL_RIGHT:
          ctrlkey = 1;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;

        case SHIFT_LEFT:
        case SHIFT_RIGHT:
          shiftkey = 1;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;

        case XK_Meta_L:
        case XK_Meta_R:
          metakey = 1;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;

        case XK_Alt_L:
        case XK_Alt_R:
          altkey = 1;
          if(randshell_popped_up == 1)
            update_randmap_dialog();
        break;

        case EXCLAM:
        break;

        case UP_ARROW:
          if(ok_to_cycle == 1)
            cycle_speed_increase();
        break;

        case DOWN_ARROW:
          if(ok_to_cycle == 1)
            cycle_speed_decrease();
        break;

        case XMKEY_PLUS:
        case XMKEY_ADD:
        case RIGHT_ARROW:
          if(ok_to_cycle == 1)
            cycle_step_forward();
        break;

        case XMKEY_MINUS:
        case XMKEY_SUBTRACT:
        case LEFT_ARROW:
          if(ok_to_cycle == 1)
            cycle_step_backward();
        break;

        case XMKEY_c:
        case XMKEY_C:
          if(ok_to_cycle)
            toggle_color_cycling();
        break;

        case ENTER:
        case RETURN:
          if(ok_to_cycle == 1)
            randomize_cycle_palette(0, 0);
        break;

        case XMKEY_F1:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 1;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(1, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(1, 4);
            else if(metakey)
              randomize_cycle_palette(1, 3);
            else if(ctrlkey)
              randomize_cycle_palette(1, 2);
            else if(shiftkey)
              randomize_cycle_palette(1, 1);
            else
              randomize_cycle_palette(1, usr_randmap_mode);
            }
        break;

        case XMKEY_F2:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 2;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(2, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(2, 4);
            else if(metakey)
              randomize_cycle_palette(2, 3);
            else if(ctrlkey)
              randomize_cycle_palette(2, 2);
            else if(shiftkey)
              randomize_cycle_palette(2, 1);
            else
              randomize_cycle_palette(2, usr_randmap_mode);
            }
        break;

        case XMKEY_F3:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 3;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(3, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(3, 4);
            else if(metakey)
              randomize_cycle_palette(3, 3);
            else if(ctrlkey)
              randomize_cycle_palette(3, 2);
            else if(shiftkey)
              randomize_cycle_palette(3, 1);
            else
              randomize_cycle_palette(3, usr_randmap_mode);
            }
        break;

        case XMKEY_F4:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 4;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(4, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(4, 4);
            else if(metakey)
              randomize_cycle_palette(4, 3);
            else if(ctrlkey)
              randomize_cycle_palette(4, 2);
            else if(shiftkey)
              randomize_cycle_palette(4, 1);
            else
              randomize_cycle_palette(4, usr_randmap_mode);
            }
        break;

        case XMKEY_F5:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 5;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(5, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(5, 4);
            else if(metakey)
              randomize_cycle_palette(5, 3);
            else if(ctrlkey)
              randomize_cycle_palette(5, 2);
            else if(shiftkey)
              randomize_cycle_palette(5, 1);
            else
              randomize_cycle_palette(5, usr_randmap_mode);
            }
        break;

        case XMKEY_F6:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 6;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(6, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(6, 4);
            else if(metakey)
              randomize_cycle_palette(6, 3);
            else if(ctrlkey)
              randomize_cycle_palette(6, 2);
            else if(shiftkey)
              randomize_cycle_palette(6, 1);
            else
              randomize_cycle_palette(6, usr_randmap_mode);
            }
        break;

        case XMKEY_F7:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 7;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(7, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(7, 4);
            else if(metakey)
              randomize_cycle_palette(7, 3);
            else if(ctrlkey)
              randomize_cycle_palette(7, 2);
            else if(shiftkey)
              randomize_cycle_palette(7, 1);
            else
              randomize_cycle_palette(7, usr_randmap_mode);
            }
        break;

        case XMKEY_F8:
          if(rotate_stuff.running == 1)
            rotate_stuff.fkey = 8;
          else if(ok_to_cycle == 1)
            {
            if(shiftkey && metakey)
              randomize_cycle_palette(8, 5);
            else if(shiftkey && ctrlkey)
              randomize_cycle_palette(8, 4);
            else if(metakey)
              randomize_cycle_palette(8, 3);
            else if(ctrlkey)
              randomize_cycle_palette(8, 2);
            else if(shiftkey)
              randomize_cycle_palette(8, 1);
            else
              randomize_cycle_palette(8, usr_randmap_mode);
            }
        break;

        case SPACE:
          if(calc_status >= IMG_INTR_RESM)
            {
            if(from_inverse)
              {
              memcpy(dacbox,olddacbox,768);
              spindac(0,0);
              from_inverse = 0;
              }
            if(fractype == CELLULAR)
              {
              if(nxtscreenflag == 0)
                {
                nxtscreenflag = 1;
                calc_status = IMG_INTR_RESM;
                update_draw_menu(CONT_CALC);
                create_image_cb(main_window, NULL, NULL);
                nxtscreenflag = 0;
                }
              }
            else
              {
              keybuffer = 0;
              do_jiim();
              }
            }
        break;

        case XMKEY_o:
          if(calc_status == IMG_COMPLETED)
            {
            if(from_inverse)
              {
              buzzer(0);
              break;
              }
            Jiim(ORBIT);
            }
        break;

        case XMKEY_j:
        case XMKEY_J:
          if(calc_status == IMG_COMPLETED)
            {
            if(fractype==JULIA || fractype==JULIAFP ||
               fractype==INVERSEJULIA || fractype==INVERSEJULIAFP)
              {
              jiiming = 1;
              sprintf((char *)&jmapname, "inversej.map");
              if(fractype==JULIA || fractype==JULIAFP)
                {
                if(ValidateLuts((char *)&jmapname) != 0)
                  {
                  break;
                  }
                spindac(0,0);
                old_type=fractype;
                fractype=INVERSEJULIA;
                from_inverse = 1;
                }
              else if(fractype==INVERSEJULIA || fractype==INVERSEJULIAFP)
                {
                if(mapset)
                  {
                  if(ValidateLuts((char *)&MAP_name) != 0)
                    {
                    if(ValidateLuts((char *)&mapfilename) != 0)
                      {
                      break;
                      }
                    }
                  }
                else
                  memcpy(dacbox,olddacbox,768);
                spindac(0,0);
                if(old_type != -1)
                  fractype=old_type;
                else
                  fractype=JULIA;
                from_inverse = 0;
                }
              curfractalspecific = &fractalspecific[fractype];
              calc_status = IMG_PRMS_CHGD;
              update_draw_menu(NEW_IMAGE);
              create_image_cb(main_window, NULL, NULL);
              jiiming = 0;
              }
            }
        break;
        }
      break;
      }
    }
  return;
}

/*ARGSUSED*/
void toggle_zoom_grab(int which)
{
  Widget button = XtNameToWidget(menu_bar, "*zoom_pan_btn");

  if(which == ZOOM_ON)
    {
    if((!(curfractalspecific->flags & NOZOOM)) &&
      (calc_status == IMG_COMPLETED || calc_status == IMG_INTR_RESM))
      {
      zoomoff = 0;
      if(button != NULL)
        XtSetSensitive(button, True);
      }
    }
  else
    {
    XFlush(display);
    XSync(display, False);
    if(!zoomoff) zoomoff = 1;
    if(button != NULL)
      XtSetSensitive(button, False);
    }
}

void update_draw_menu(int cont)
{
  XmString str;
  Widget button;

  if(cont)
    str = XmStringCreateLocalized("Continue Calculation");
  else
    str = XmStringCreateLocalized("Create Image");

  button = XtNameToWidget(menu_bar, "*draw_fractal_menu_btn1");
  if(button != NULL)
    XtVaSetValues(button, XmNlabelString, str, NULL);
  XmStringFree(str);

  if(paledit.popped_up == 1 || calc_status > 0)
    ok_to_cycle = 1;
}

/*
 * This routine from the "Motif Programming Manual" (Second Edition), by Dan
 * Heller and Paula M. Ferguson, (c) 1991, 1994 O'Reilly & Associates, Inc.,
 * pp. 750-751. It ensures that a window's contents are visible before
 * returning. This is necessary in this instance so that we may position
 * other dialogs relative to the position of the main window, and the main
 * window must be visible before the map_dialog routine can understand where
 * the window is mapped.
 */

void
force_update(Widget w)
{
  Display *dpy;
  XEvent event;
  XWindowAttributes xwa;
  Widget dshell, tshell;
  Window dwindow, twindow;

  XtAppContext cxt = XtWidgetToApplicationContext(w);

  /* Locate the shell we are interested in */
  for(dshell = w; !XtIsShell(dshell); dshell = XtParent(dshell));

  /* Locate it's primary window's shell (which may be the same) */
  for(tshell = dshell; !XtIsTopLevelShell(tshell); tshell = XtParent(tshell));

  /*
   * If the dialog shell (or it's primary shell window) is not realized,
   * don't bother... nothing can possibly happen.
   */

  if(XtIsRealized(dshell) && XtIsRealized(tshell))
    {
    dpy = XtDisplay(tshell);
    dwindow = XtWindow(dshell);
    twindow = XtWindow(tshell);

    /* Wait for the dialog to be mapped. It's guaranteed to become so. */
    while(XGetWindowAttributes(dpy, dwindow, &xwa) &&
          xwa.map_state != IsViewable)
      {
      /* ... if the primary is (or becomes) unviewable or unmapped,
       * it's probably iconic, and nothing will happen.
       */
      if(XGetWindowAttributes(dpy, twindow, &xwa) &&
         xwa.map_state != IsViewable)
        {
        break;
        }

      /* We are guaranteed there will be an event of some kind. */
      XtAppNextEvent(cxt, &event);
      XtDispatchEvent(&event);
      }
    }

  /* The next XSync() will get an expose event. */
  XmUpdateDisplay(tshell);
}

RETSIGTYPE fpe_handler(int signum)
{
    overflow = 1;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: util.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.36  1995/06/13  17:51:35  darryl
 * Add window resize mode function.
 *
 * Revision 1.35  1995/06/12  16:07:19  darryl
 * Disable color cycling when palette editor freestyle
 * mode is active. Re-enable when exiting freestyle mode.
 *
 * Revision 1.34  1995/06/07  19:20:09  darryl
 * Fix problem where the MIN macro was not guarded from
 * previous declarations and that the palette randomization
 * Done and Apply buttons were handled incorrectly.
 *
 * Revision 1.33  1995/06/05  16:30:14  tom
 * File handling enhancements. Remove DOS-specific code.
 *
 * Revision 1.32  1995/06/02  06:09:35  darryl
 * Change default button in some dialogs from Cancel to Ok.
 *
 * Revision 1.31  1995/06/02  05:42:01  darryl
 * Add restored images from file to restore history.
 *
 * Revision 1.30  1995/05/26  19:05:18  darryl
 * Fix default color cycling options.
 *
 * Revision 1.29  1995/05/23  18:50:55  darryl
 * Fix color editor and palette randomization problems
 * reported in beta testing.
 *
 * Revision 1.28  1995/05/21  09:54:04  darryl
 * Add keyboard processing for more modifier keys.
 *
 * Revision 1.27  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.25.1.1  1995/06/05  14:11:05  tom
 * changed filename handling
 *
 * Revision 1.26  1995/05/19  08:11:11  darryl
 * Add initialization for zoom box color index.
 *
 * Revision 1.25  1995/05/15  19:30:32  darryl
 * Change indexing method for xgc foreground color selection.
 *
 * Revision 1.24  1995/05/12  18:11:34  darryl
 * Add default pushbuttons to all dialogs. Add a warning
 * message to the Mandelbrot/Julia toggle function when
 * in preview mode. Add warnings to the create image
 * function in preview mode if the window is too small.
 *
 * Revision 1.23  1995/05/08  19:44:43  darryl
 * Fix bugs reported in beta test.
 *
 * Revision 1.22  1995/05/02  23:12:55  darryl
 * Enable freestyle option, fix various bugs in the color editor.
 *
 * Revision 1.21  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.20  1995/04/27  19:07:43  darryl
 * Change zoombox color initialization yet again.
 *
 * Revision 1.19  1995/04/26  20:31:53  darryl
 * Fix bugs reported in beta testing:
 *   -- solid-guessing would occasionally leave portions
 *      of the image in coarse pixels.
 *   -- Re-align more void pointers before type-casting.
 *   -- Change zoombox color on loading a map or gif file.
 *   -- Add panned images to restore history.
 *
 * Revision 1.18  1995/04/21  21:54:14  darryl
 * Fix bug where xgc was invisible when a saved image was displayed.
 *
 * Revision 1.17  1995/04/21  18:31:23  darryl
 * Update client_data handling.
 *
 * Revision 1.16  1995/04/20  16:42:16  darryl
 * Style cleanup -- change tabs to spaces.
 *
 * Revision 1.15  1995/04/18  15:59:59  darryl
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
 * Revision 1.14  1995/04/10  22:12:57  darryl
 * Add rate=nnn and menu support for setting the screen refresh
 * rate during image calculation. Fix a bug where the Julia
 * inverse mode was broken after an image save. Add fallback
 * resources that take effect if a resource file is not found.
 *
 * Revision 1.13  1995/04/09  03:13:08  darryl
 * Fix bug where zoom corners and calc_status were set
 * incorrectly after loading a saved image.
 *
 * Revision 1.12  1995/04/08  19:14:32  tom
 * Enhance timer functions. Uses sysconf(3) to
 * determine CLK_TCK if the function is available.
 *
 * Revision 1.11  1995/03/31  18:47:10  darryl
 * Update parameter file processing so that 3D parameters
 * are handled in the same fashion as other entries.
 *
 * Revision 1.10  1995/03/31  06:49:22  darryl
 * Fix window size preset values.
 *
 * Revision 1.9  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.8  1995/03/28  18:04:20  tom
 * Enhance initialization file handling. Revise stackavail()
 * so that it uses getrlimit if available. Enhance configuration
 * files to check for getrlimit and associated header files.
 *
 * Revision 1.7  1995/03/26  18:46:28  tom
 * Check for XMFRACTDIR then FRACTDIR in the environment,
 * add checks for gettimeofday parameters, add install
 * target and defaults to the configuration files.
 *
 * Revision 1.6  1995/03/23  20:06:30  darryl
 * Modify help, allow full-path names in the ini file,
 * fix a bug where default trig paramaters were not
 * set correctly for some types.
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
 * Revision 1.2.1.4  1995/03/14  15:33:35  tom
 * changed #if to #ifdef
 *
 * Revision 1.2.1.3  1995/03/13  18:42:42  tom
 * removed strnicmp
 *
 * Revision 1.2.1.2  1995/03/13  14:53:36  tom
 * changed return type of fpe_handler
 *
 * Revision 1.2.1.1  1995/03/11  02:50:09  tom
 * removed stricmp - inserted in own file
 * renamed dysize to dy_size and dxsixe to dx_size
 * added HAVE_DIV_T
 * added HAVE_GETTIMEOFDAY (not completely finished)
 *
 * Revision 1.2  1995/03/10  19:32:17  darryl
 * Add signal handler definition.
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.305  1995/02/28  18:49:20  darryl
 * Modifications for Linux.
 *
 * Revision 0.304  1995/01/30  23:23:13  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.303  1995/01/17  00:03:30  darryl
 * Add a disclaimer dialog.
 *
 * Revision 0.302  1995/01/11  23:21:33  darryl
 * Use the previous method of loading the xgc fg and bg colors.
 *
 * Revision 0.301  1994/12/22  19:29:07  darryl
 * Speed up decoder calls.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.206  1994/12/10  01:05:03  darryl
 * More attempts to fortify the rubber-banding functions.
 *
 * Revision 0.205  1994/12/09  16:58:26  darryl
 * Fix bug where the zoombox colors could be reversed.
 *
 * Revision 0.204  1994/12/09  00:22:52  darryl
 * Change fg and bg of xgc if a new color map.
 *
 * Revision 0.203  1994/12/08  19:27:17  darryl
 * Ensure that the xgc contains the special colors as the
 * foreground and background when the color map changes.
 *
 * Revision 0.202  1994/12/07  00:31:30  darryl
 * Change calls to rubber banding functions from a passive
 * pointer grab to translations from the canvas widget.
 *
 * Revision 0.201  1994/12/05  16:59:27  darryl
 * Fix bug where the draw image button was being unmanaged.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.111  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.110  1994/11/30  00:00:09  darryl
 * Set the sensitivity of the 'Flip Image' menu item
 * depending on the state of the image creation (calc_status)
 * and the fractal type that is currently selected.
 *
 * Revision 0.109  1994/11/29  21:44:52  darryl
 * Fix typo in toggle_zoom_grab() that caused too many
 * parameters to be passed to XtGrabButton().
 *
 * Revision 0.108  1994/11/29  07:34:52  darryl
 * Correct capitalization of menu item entries.
 *
 * Revision 0.107  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.106  1994/11/29  04:17:04  darryl
 * Beef up the zoom box event handlers. Ungrab and re-grab
 * the buttons when something else is going on in the canvas
 * window, and discard pointer events that happened during
 * an image creation.
 *
 * Revision 0.105  1994/11/29  02:08:16  darryl
 * Reverse the operation of the zoomoff variable so
 * that a non-zero value means that zooming is disabled.
 *
 * Revision 0.104  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.103  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.102  1994/11/21  18:21:16  darryl
 * Change instances of XtIsRealized to XtIsManaged.
 *
 * Revision 0.101  1994/11/19  02:53:05  darryl
 * Move plotblock() to solidguess.c and declare it static.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
