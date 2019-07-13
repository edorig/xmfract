
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: loadfile.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int find_fractal_info(char *gif_file, struct fractal_info *info);
static void load_ext_blk(char *loadptr, int loadlen);
static void skip_ext_blk(int *block_len, int *data_len);
static void backwardscompat(struct fractal_info *info);
static void set_if_old_bif(void);

/* static variables */
extern int num_fractal_types;
extern int major_method;
extern int minor_method;

static float fileaspectratio;

/*
 * loadfile.c - load an existing fractal image, control level
 */

int read_overlay()    /* read overlay/3D files, if reqr'd */
{
  struct fractal_info read_info;
  char oldfloatflag;
  char msg[110];

  showfile = 1;        /* for any abort exit, pretend done */
  initmode = -1;        /* no viewing mode set yet */
  oldfloatflag = usr_floatflag;
  loaded3d = 0;

  if(strchr(readname,'.') == NULL)
    strcat(readname,".gif");

  sprintf((char *) &ZoomName, "NOTGIF");

  if(find_fractal_info(readname,&read_info))
    { /* didn't find a useable file */
    sprintf(msg,"Sorry, %s isn't a file I can decode.",readname);
    inform(main_window, msg, NULL);
    return(-1);
    }

  /*
   * This tells the zoom code that the currently displayed file is
   * a GIF that does not have any fractal info, and cannot be zoomed.
   * It gets reset here before reading a gif file. It gets set to
   * GIFFILE when reading a gif that does not have fractal info.
   */

  maxit = read_info.iterations;
  fractype = read_info.fractal_type;
  if (fractype < 0 || fractype >= num_fractal_types)
    {
    int ret = 0;
    sprintf((char *) &msg,
      "Warning: %s has a bad fractal type; using 0", readname);
    ret = confirm(main_window, (char *) &msg,
      OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON);
    if(ret != 1)
      return(-1);
    fractype = 0;
    }
  curfractalspecific = &fractalspecific[fractype];
  xxmin    = read_info.xmin;
  xxmax    = read_info.xmax;
  yymin    = read_info.ymin;
  yymax    = read_info.ymax;
  param[0]    = read_info.creal;
  param[1]    = read_info.cimag;
  save_release = 1100; /* unless we find out better later on */
  invert = 0;

  if(read_info.version > 0)
    {
    param[2]      = read_info.parm3;
    roundfloatd(&param[2]);
    param[3]      = read_info.parm4;
    roundfloatd(&param[3]);
    potparam[0]   = read_info.potential[0];
    potparam[1]   = read_info.potential[1];
    potparam[2]   = read_info.potential[2];
    potflag       = (potparam[0] != 0.0);
    rflag         = read_info.rflag;
    rseed         = read_info.rseed;
    inside        = read_info.inside;
    LogFlag       = read_info.logmap;
    inversion[0]  = read_info.invert[0];
    inversion[1]  = read_info.invert[1];
    inversion[2]  = read_info.invert[2];
    if (inversion[0] != 0.0)
      invert = 3;
    decomp[0]     = read_info.decomp[0];
    decomp[1]     = read_info.decomp[1];
    usr_biomorph  = read_info.biomorph;
    forcesymmetry = read_info.symmetry;
    }

  if(read_info.version > 1)
    {
    save_release  = 1200;
    if (!display3d
        && (read_info.version <= 4 || read_info.flag3d > 0
        || (curfractalspecific->flags&PARMS3D) ))
      {
      int i;
      for (i = 0; i < 16; i++)
        init3d[i] = read_info.init3d[i];
      previewfactor   = read_info.previewfactor;
      xtrans          = read_info.xtrans;
      ytrans          = read_info.ytrans;
      red_crop_left   = read_info.red_crop_left;
      red_crop_right  = read_info.red_crop_right;
      blue_crop_left  = read_info.blue_crop_left;
      blue_crop_right = read_info.blue_crop_right;
      red_bright      = read_info.red_bright;
      blue_bright     = read_info.blue_bright;
      xadjust         = read_info.xadjust;
      eyeseparation   = read_info.eyeseparation;
      glassestype     = read_info.glassestype;
      }
    }

  if(read_info.version > 2)
    {
    save_release = 1300;
    outside      = read_info.outside;
    }

  calc_status = IMG_PRMS_CHGD;      /* defaults if version < 4 */
  update_draw_menu(NEW_IMAGE);
  xx3rd = xxmin;
  yy3rd = yymin;
  usr_distest = 0;
  calctime = 0;

  if(read_info.version > 3)
    {
    save_release         = 1400;
    xx3rd                = read_info.x3rd;
    yy3rd                = read_info.y3rd;
    calc_status          = read_info.calc_status;
    usr_stdcalcmode      = read_info.stdcalcmode;
    usr_distest          = read_info.distest;
    usr_floatflag        = read_info.floatflag;
    bailout              = read_info.bailout;
    calctime             = read_info.calctime;
    trigndx[0]           = read_info.trigndx[0];
    trigndx[1]           = read_info.trigndx[1];
    trigndx[2]           = read_info.trigndx[2];
    trigndx[3]           = read_info.trigndx[3];
    finattract           = read_info.finattract;
    initorbit.x          = read_info.initorbit[0];
    initorbit.y          = read_info.initorbit[1];
    useinitorbit         = read_info.useinitorbit;
    usr_periodicitycheck = read_info.periodicity;
    }

  pot16bit = 0;
  save_system = 0;

  if(read_info.version > 4)
    {
    pot16bit = read_info.pot16bit;

    if (pot16bit)
      filexdots >>= 1;

    fileaspectratio = read_info.faspectratio;

    if (fileaspectratio < 0.01)    /* fix files produced in early v14.1 */
      fileaspectratio = screenaspect;

    save_system  = read_info.system;
    save_release = read_info.release; /* from fmt 5 on we know real number */

    if (read_info.version == 5    /* except a few early fmt 5 cases: */
       && (save_release <= 0 || save_release >= 2000))
      {
      save_release = 1410;
      save_system = 0;
      }

    if (!display3d && read_info.flag3d > 0)
      {
      loaded3d       = 1;
      Ambient        = read_info.ambient;
      RANDOMIZE      = read_info.randomize;
      haze           = read_info.haze;
      transparent[0] = read_info.transparent[0];
      transparent[1] = read_info.transparent[1];
      }
    }

  rotate_lo = 0; rotate_hi = colors - 1;
  distestwidth = 71;
  if(read_info.version > 5)
    {
    rotate_lo    = read_info.rotate_lo;
    rotate_hi    = read_info.rotate_hi;
    distestwidth = read_info.distestwidth;
    }

  if(read_info.version > 6)
    {
    param[2] = read_info.dparm3;
    param[3] = read_info.dparm4;
    }

  if(read_info.version > 7)
    {
    fillcolor = read_info.fillcolor;
    }

  if(read_info.version > 8)
    {
    mxmaxfp   =  read_info.mxmaxfp;
    mxminfp   =  read_info.mxminfp;
    mymaxfp   =  read_info.mymaxfp;
    myminfp   =  read_info.myminfp;
    zdots     =  read_info.zdots;
    originfp  =  read_info.originfp;
    depthfp   =  read_info.depthfp;
    heightfp  =  read_info.heightfp;
    widthfp   =  read_info.widthfp;
    distfp    =  read_info.distfp;
    eyesfp    =  read_info.eyesfp;
    neworbittype = read_info.orbittype;
    juli3Dmode   = read_info.juli3Dmode;
    maxfn    =   read_info.maxfn;
    major_method = read_info.inversejulia >> 8;
    minor_method = read_info.inversejulia & 255;
    param[4] = read_info.dparm5;
    param[5] = read_info.dparm6;
    param[6] = read_info.dparm7;
    param[7] = read_info.dparm8;
    param[8] = read_info.dparm9;
    param[9] = read_info.dparm10;
    }

  if(read_info.version < 4)
    { /* pre-version 14.0? */
    backwardscompat(&read_info); /* translate obsolete types */
    if(LogFlag)
      LogFlag = 2;
    usr_floatflag = (curfractalspecific->isinteger) ? '0' : '1';
    }

  if (read_info.version < 5)
    { /* pre-version 15.0? */
    if (LogFlag == 2) /* logmap=old changed again in format 5! */
      LogFlag = -1;
    if (decomp[0] > 0 && decomp[1] > 0)
      bailout = decomp[1];
    }

  if(potflag) /* in version 15.x and 16.x logmap didn't work with pot */
    if(read_info.version == 6 || read_info.version == 7)
      LogFlag = 0;

  set_trig_pointers(-1);

  if(read_info.version < 9)
    { /* pre-version 18.0? */
    /* forcesymmetry==1000 means we want to force symmetry but don't
       know which symmetry yet, will find out in setsymmetry() */
    if(outside==REAL || outside==IMAG || outside==MULT || outside==SUM)
      if(forcesymmetry == 999)
        forcesymmetry = 1000;
    }

  if(save_release < 1725)
    { /* pre-version 17.25 */
    set_if_old_bif(); /* translate bifurcation types */
    }

  if (display3d)                  /* PB - a klooge till the meaning of */
    usr_floatflag = oldfloatflag; /*  floatflag in line3d is clarified */

  if (overlay3d)
    {
    initmode = 0;       /* use previous adapter mode for overlays */
    if (filexdots > xdots || fileydots > ydots)
      {
      static char msg[]={"Can't overlay with a larger image"};
      inform(main_window, msg, NULL);
      initmode = -1;
      return(-1);
      }
    }
  else
    {
    int olddisplay3d,oldfloatflag,i;
    olddisplay3d = display3d;
    oldfloatflag = floatflag;
    display3d = loaded3d;      /* for <tab> display during next */
    floatflag = usr_floatflag; /* ditto */
    i = 0;
    display3d = olddisplay3d;
    floatflag = oldfloatflag;
    if (i)
      {
      initmode = -1;
      return(-1);
      }
    }

  if (display3d)
    {
    calc_status = IMG_PRMS_CHGD;
    update_draw_menu(NEW_IMAGE);
    toggle_zoom_grab(ZOOM_OFF);
    fractype = PLASMA;
    curfractalspecific = &fractalspecific[PLASMA];
    param[0] = 0;
    }

  showfile = 0;
  return(0);
}

static int find_fractal_info(gif_file,info)
char *gif_file;
struct fractal_info *info;
{
  unsigned char gifstart[18];
  char temp1[81];
  int scan_extend, block_type, block_len, data_len;
  int fractinf_len;
  int hdr_offset;

  if((fp = fopen(gif_file,"rb"))==NULL)
    return(-1);

  fread(gifstart,18,1,fp);

  if ((strncmp((char *)gifstart,"GIF",3)!=0) &&
      (strncmp((char *)gifstart,"gif",3)!=0))
    { /* not GIF, maybe old .tga? */
    if(fread(info,FRACTAL_INFO_SIZE,1,fp)==1 &&
      strncmp(info->info_id,"Fractal",8)==0)
      {

      filetype = 1; /* Targa 16 */

      GET16(gifstart[14],fileydots);
      GET16(gifstart[12],filexdots);

      filecolors = info->colors;
      fileaspectratio = screenaspect;

      if(fileydots == info->ydots && filexdots == info->xdots)
        {
        fclose(fp);
        return(0);
        }
      }
    fclose(fp);
    return(-1);
    }

  filetype = 0; /* GIF */

  GET16(gifstart[6],filexdots);
  GET16(gifstart[8],fileydots);

  filecolors = 2 << (gifstart[10] & 7);
  fileaspectratio = 0; /* unknown */

  if (gifstart[12])
    { /* calc reasonably close value from gif header */
    fileaspectratio = (64.0 / ((double)(gifstart[12]) + 15.0))
                    * (double)fileydots / (double)filexdots;
    if ( fileaspectratio > screenaspect-0.03
      && fileaspectratio < screenaspect+0.03)
      fileaspectratio = screenaspect;
    }
  else
    if (fileydots * 4 == filexdots * 3) /* assume the common square pixels */
      fileaspectratio = screenaspect;

  if (resume_info != NULL)
    { /* free the prior area if there is one */
    XtFree(resume_info);
    resume_info = NULL;
    }

  if (rangeslen)
    { /* free prior ranges */
    XtFree((char *)ranges);
    rangeslen = 0;
    }

/* Format of .gif extension blocks is:
   1 byte    '!', extension block identifier
   1 byte    extension block number, 255
   1 byte    length of id, 11
  11 bytes   alpha id, "fractintnnn" with fractint, nnn is secondary id
    n * {
   1 byte    length of block info in bytes
   x bytes   block info
    }
   1 byte    0, extension terminator
   To scan extension blocks, we first look in file at length of fractal_info
   (the main extension block) from end of file, looking for a literal known
   to be at start of our block info.  Then we scan forward a bit, in case
   the file is from an earlier fractint vsn with shorter fractal_info.
   If fractal_info is found and is from vsn>=14, it includes the total length
   of all extension blocks; we then scan them all first to last to load
   any optional ones which are present.
   Defined extension blocks:
  fractint001    header, always present
  fractint002    resume info for interrupted resumable image
  fractint003    additional formula type info
  fractint004    ranges info
  */

  memset(info,0,FRACTAL_INFO_SIZE);
  fractinf_len = FRACTAL_INFO_SIZE + (FRACTAL_INFO_SIZE+254)/255;
  fseek(fp,(long)(-1-fractinf_len),SEEK_END);
  fread(info,1,FRACTAL_INFO_SIZE,fp);
  if ((int)strcmp(INFO_ID,info->info_id) == 0)
    {
    decode_fractal_info(info,1);
    hdr_offset = -1-fractinf_len;
    }
  else
    {
    /* didn't work 1st try, maybe an older vsn, maybe junk at eof, scan: */
    int offset,i;
    char tmpbuf[110];
    hdr_offset = 0;
    offset = 80; /* don't even check last 80 bytes of file for id */
    while (offset < fractinf_len+513)
      { /* allow 512 garbage at eof */
      offset += 100; /* go back 100 bytes at a time */
      fseek(fp,(long)(0-offset),SEEK_END);
      fread(tmpbuf,1,110,fp); /* read 10 extra for string compare */
      for (i = 0; i < 100; ++i)
        {
        if (!(int)strcmp(INFO_ID,&tmpbuf[i]))
          { /* found header? */
          strcpy(info->info_id,INFO_ID);
          fseek(fp,(long)(hdr_offset=i-offset),SEEK_END);
          fread(info,1,FRACTAL_INFO_SIZE,fp);
          decode_fractal_info(info,1);
          offset = 10000; /* force exit from outer loop */
          break;
          }
        }
      }
    }

  if (hdr_offset)
    { /* we found INFO_ID */

    if (info->version >= 4)
      {
      /* first reload main extension block, reasons:
         might be over 255 chars, and thus earlier load might be bad
         find exact endpoint, so scan back to start of ext blks works
       */
      fseek(fp,(long)(hdr_offset-15),SEEK_END);
      scan_extend = 1;
      while (scan_extend)
        {
        if (fgetc(fp) != '!' /* if not what we expect just give up */
             || fread(temp1,1,13,fp) != 13
             || strncmp(&temp1[2],"fractint",8))
          break;
        temp1[13] = 0;
        block_type = atoi(&temp1[10]); /* e.g. "fractint002" */
        switch (block_type)
          {
          case 1: /* "fractint001", the main extension block */
            if (scan_extend == 2)
              { /* we've been here before, done now */
              scan_extend = 0;
              break;
              }
            load_ext_blk((char *)info,FRACTAL_INFO_SIZE);
            decode_fractal_info(info,1);
            scan_extend = 2;
            /* now we know total extension len, back up to first block */
            fseek(fp,0L-info->tot_extend_len,SEEK_CUR);
          break;

          case 2: /* resume info */
            skip_ext_blk(&block_len,&data_len); /* once to get lengths */
            if ((resume_info = XtMalloc((unsigned)data_len)) == NULL)
              info->calc_status = IMG_INTR_NRES; /* not resumable after all */
            else
              {
              fseek(fp,(long)(0-block_len),SEEK_CUR);
              load_ext_blk(resume_info,data_len);
              resume_len = data_len;
              }
          break;

          case 3: /* formula info */
            {
            char *nameptr;
            char tmpname[ITEMNAMELEN+1];
            load_ext_blk(tmpname,40);
            switch (info->fractal_type)
              {
              case LSYSTEM:
                nameptr = LName;
              break;

              case IFS:
              case IFS3D:
                nameptr = IFSName;
              break;

              default:
                nameptr = FormName;
              break;
              }
            tmpname[ITEMNAMELEN] = 0;
            strcpy(nameptr,tmpname);
            /* perhaps in future add more here, check block_len for
               backward compatibility */
            }
          break;
          case 4: /* ranges info */
            skip_ext_blk(&block_len,&data_len); /* once to get lengths */
            if ((ranges = (int *)XtMalloc((unsigned)data_len)))
              {
              fseek(fp,(long)(0-block_len),SEEK_CUR);
              load_ext_blk((char *)ranges,data_len);
              rangeslen = data_len/2;
              fix_ranges(ranges,rangeslen,1);
              }
          break;
          default:
            skip_ext_blk(&block_len,&data_len);
          }
        }
      }

    fclose(fp);
    fileaspectratio = screenaspect; /* if not >= v15, this is correct */
    return(0);
    }

  strcpy(info->info_id, "GIFFILE");
  info->iterations = 150;
  info->fractal_type = PLASMA;
  info->xmin = -1;
  info->xmax = 1;
  info->ymin = -1;
  info->ymax = 1;
  info->x3rd = -1;
  info->y3rd = -1;
  info->creal = 0;
  info->cimag = 0;
  info->videomodeax=255;
  info->videomodebx=255;
  info->videomodecx=255;
  info->videomodedx=255;
  info->dotmode = 0;
  info->xdots = filexdots;
  info->ydots = fileydots;
  info->colors = filecolors;
  info->version = 0; /* this forces lots more init at calling end too */

  /*
   * This tells the zoom code that the currently displayed file is
   * a GIF that does not have any fractal info, and cannot be zoomed.
   * It is reset at the top of this function and in initvars_fractal()
   */

  strcpy((char *) &ZoomName, "GIFFILE");

  /* zero means we won */
  fclose(fp);
  return(0);
  }

static void load_ext_blk(char *loadptr,int loadlen)
  {
  int len;
  while ((len = fgetc(fp)) > 0)
    {
    while (--len >= 0)
      {
      if (--loadlen >= 0)
        *(loadptr++) = fgetc(fp);
      else
        fgetc(fp); /* discard excess characters */
      }
    }
  }

static void skip_ext_blk(int *block_len, int *data_len)
{
  int len;
  *data_len = 0;
  *block_len = 1;
  while ((len = fgetc(fp)) > 0)
    {
    fseek(fp,(long)len,SEEK_CUR);
    *data_len += len;
    *block_len += len + 1;
    }
}


/* switch obsolete fractal types to new generalizations */
static void backwardscompat(struct fractal_info *info)
{
  switch(fractype)
    {
    case LAMBDASINE:
      fractype = LAMBDATRIGFP;
      trigndx[0] = SIN;
    break;

    case LAMBDACOS  :
      fractype = LAMBDATRIGFP;
      trigndx[0] = COS;
    break;

    case LAMBDAEXP  :
      fractype = LAMBDATRIGFP;
      trigndx[0] = EXP;
    break;

    case MANDELSINE  :
      fractype = MANDELTRIGFP;
      trigndx[0] = SIN;
    break;

    case MANDELCOS  :
      fractype = MANDELTRIGFP;
      trigndx[0] = COS;
    break;

    case MANDELEXP  :
      fractype = MANDELTRIGFP;
      trigndx[0] = EXP;
    break;

    case MANDELSINH  :
      fractype = MANDELTRIGFP;
      trigndx[0] = SINH;
    break;

    case LAMBDASINH  :
      fractype = LAMBDATRIGFP;
      trigndx[0] = SINH;
    break;

    case MANDELCOSH  :
      fractype = MANDELTRIGFP;
      trigndx[0] = COSH;
    break;

    case LAMBDACOSH  :
      fractype = LAMBDATRIGFP;
      trigndx[0] = COSH;
    break;

    case LMANDELSINE  :
      fractype = MANDELTRIG;
      trigndx[0] = SIN;
    break;

    case LLAMBDASINE  :
      fractype = LAMBDATRIG;
      trigndx[0] = SIN;
    break;

    case LMANDELCOS  :
      fractype = MANDELTRIG;
      trigndx[0] = COS;
    break;

    case LLAMBDACOS  :
      fractype = LAMBDATRIG;
      trigndx[0] = COS;
    break;

    case LMANDELSINH  :
      fractype = MANDELTRIG;
      trigndx[0] = SINH;
    break;

    case LLAMBDASINH  :
      fractype = LAMBDATRIG;
      trigndx[0] = SINH;
    break;

    case LMANDELCOSH  :
      fractype = MANDELTRIG;
      trigndx[0] = COSH;
    break;

    case LLAMBDACOSH  :
      fractype = LAMBDATRIG;
      trigndx[0] = COSH;
    break;

    case LMANDELEXP  :
      fractype = MANDELTRIG;
      trigndx[0] = EXP;
    break;

    case LLAMBDAEXP  :
      fractype = LAMBDATRIG;
      trigndx[0] = EXP;
    break;

    case DEMM   :
      fractype = MANDELFP;
      usr_distest = (info->ydots - 1) * 2;
    break;

    case DEMJ   :
      fractype = JULIAFP;
      usr_distest = (info->ydots - 1) * 2;
    break;

    case MANDELLAMBDA :
      useinitorbit = 2;
    break;
    }
  curfractalspecific = &fractalspecific[fractype];
}

/* switch old bifurcation fractal types to new generalizations */
static void set_if_old_bif(void)
{
/* set functions if not set already, may need to check 'functionpreloaded'
   before calling this routine.  JCO 7/5/92 */

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
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: loadfile.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.9  1995/06/15  15:36:25  darryl
 * Enhance zoom error message when attempting to zoom after
 * reading a GIF that does not contain fractal info.
 *
 * Revision 1.8  1995/05/26  19:05:18  darryl
 * Fix default color cycling options.
 *
 * Revision 1.7  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.6  1995/04/18  15:59:59  darryl
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
 * Revision 1.5  1995/03/31  18:47:10  darryl
 * Update parameter file processing so that 3D parameters
 * are handled in the same fashion as other entries.
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
 * Revision 0.104  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.103  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
 *
 * Revision 0.102  1994/11/29  06:15:07  darryl
 * More bulletproofing the zoom box, and default rotate_lo to
 * two so the lake is not cycled, and fix a bug where the
 * scale multiple on the upper and lower cycle range scales
 * was not set to one.
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
