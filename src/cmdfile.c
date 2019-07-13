
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: cmdfile.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int next_command
  (char *cmdbuf, int maxlen, FILE *handle,
   char *linebuf, int *lineoffset, int mode);
static int next_line(FILE *handle, char *linebuf, int mode);
static void reset_ifs_defn(void);
static void initvars_3d(void);
static void argerror(char *badarg);
static int file_gets(char *buf, int maxlen, FILE *infile);
static int parse_colors(char *value);

/* global variables */
char s_iter[]        = "iter";
char s_real[]        = "real";
char s_mult[]        = "mult";
char s_sum[]         = "summ";
char s_imag[]        = "imag";
char s_zmag[]        = "zmag";
char s_bof60[]       = "bof60";
char s_bof61[]       = "bof61";
char s_maxiter[]     = "maxiter";
char s_epscross[]    = "epsiloncross";
char s_startrail[]   = "startrail";
char s_normal[]      = "normal";
char s_period[]      = "period";

char CommandComment1[57];   /* comments for command set */
char CommandComment2[57];
char CommandComment3[57];
char CommandComment4[57];

/* static variables */
static char gifmask[13];
static char initparams;
static int first_init;
static int init_rseed;

/* mode = 0 command line @filename         */
/*        1 xmfract.ini                    */
/*        2 <@> command after startup      */
/*        3 command line @filename/setname */

int cmdfile(FILE *handle,int mode)
{
  /* note that cmdfile could be open as text OR as binary */
  /* binary is used in @ command processing for reasonable speed */
  int i;
  int lineoffset = 0;
  int changeflag = 0; /* &1 fractal stuff chgd, &2 3d stuff chgd */
  char linebuf[513],cmdbuf[1001];
  gotfrmfile = 0;
  gotfrmname = 0;
  gotifsfile = 0;
  gotifsname = 0;
  gotlsysfile = 0;
  gotlsysname = 0;
  if (mode == 2 || mode == 3)
    {
    while ((i = getc(handle)) != '{' && i != EOF)
      {
      /* empty loop */
      }
    CommandComment1[0] = CommandComment2[0] = 0;
    CommandComment3[0] = CommandComment4[0] = 0;
    }
  linebuf[0] = 0;
  while (next_command(cmdbuf,1000,handle,linebuf,&lineoffset,mode) > 0)
    {
    if ((mode == 2 || mode == 3) && (int)strcmp(cmdbuf,"}") == 0)
      break;
    if ((i = cmdarg(cmdbuf,mode)) < 0)
      break;
    changeflag |= i;
    }
  fclose(handle);
  return changeflag;
}

static int next_command(char *cmdbuf,int maxlen,
          FILE *handle,char *linebuf,int *lineoffset,int mode)
{
  int cmdlen = 0;
  char *lineptr;
  lineptr = linebuf + *lineoffset;
  while(1)
    {
    while (*lineptr <= ' ' || *lineptr == ';')
      {
      if (cmdlen)
        {        /* space or ; marks end of command */
        cmdbuf[cmdlen] = 0;
        *lineoffset = lineptr - linebuf;
        return cmdlen;
        }
      while (*lineptr && *lineptr <= ' ')
        ++lineptr;        /* skip spaces and tabs */
      if (*lineptr == ';' || *lineptr == 0)
        {
        if (*lineptr == ';'
          && (mode == 2 || mode == 3)
          && (CommandComment1[0] == 0 || CommandComment2[0] == 0 ||
              CommandComment3[0] == 0 || CommandComment4[0] == 0))
          {
          /* save comment */
          while (*(++lineptr) && (*lineptr == ' ' || *lineptr == '\t')) { }
          if (*lineptr)
            {
            if ((int)(strlen(lineptr)) > 56)
              *(lineptr+56) = 0;
            if (CommandComment1[0] == 0)
              strcpy(CommandComment1,lineptr);
            else if (CommandComment2[0] == 0)
              strcpy(CommandComment2,lineptr);
            else if (CommandComment3[0] == 0)
              strcpy(CommandComment3,lineptr);
            else
              strcpy(CommandComment4,lineptr);
            }
          }
        if (next_line(handle,linebuf,mode) != 0)
          return(-1); /* eof */
        lineptr = linebuf; /* start new line */
        }
      }
    if (*lineptr == '\\'          /* continuation onto next line? */
      && *(lineptr+1) == 0)
      {
      if (next_line(handle,linebuf,mode) != 0)
        {
        argerror(cmdbuf);      /* missing continuation */
        return(-1);
        }
      lineptr = linebuf;
      while (*lineptr && *lineptr <= ' ')
        ++lineptr;        /* skip white space @ start next line */
      continue;        /* loop to check end of line again */
      }
    cmdbuf[cmdlen] = *(lineptr++);   /* copy character to command buffer */
    if (++cmdlen >= maxlen)
      {    /* command too long? */
      argerror(cmdbuf);
      return(-1);
      }
    }
}

static int next_line(FILE *handle,char *linebuf,int mode)
{
  int toolssection;
  char tmpbuf[10];
  toolssection = 0;
  while (file_gets(linebuf,512,handle) >= 0)
    {
    if (mode == 1 && linebuf[0] == '[')
      {     /* check for [fractint] */
      strncpy(tmpbuf,&linebuf[1],9);
      tmpbuf[9] = 0;
      toolssection = strncmp(tmpbuf,"fractint]",9);
      continue;              /* skip tools section heading */
      }
    if (toolssection == 0) return(0);
    }
  return(-1);
}

/*
 * cmdarg(string,mode) processes a single command-line/command-file argument
 *  return:
 *    -1 error, >= 0 ok
 *    if ok, return value:
 *  | 1 means fractal parm has been set
 *  | 2 means 3d parm has been set
 *  | 4 means 3d=yes specified
 *  | 8 means reset specified
 */

int cmdarg(char *curarg,int mode) /* process a single argument */
{
  char variable[21];   /* variable name goes here   */
  char *value;         /* pointer to variable value */
  int valuelen;        /* length of value           */
  int numval;          /* numeric value of arg      */
  char charval;        /* first character of arg    */
  int yesnoval;        /* 0 if 'n', 1 if 'y', -1 if not */
  double ftemp;
  int i, j, k;
  char *argptr,*argptr2;
  int totparms;         /* # of / delimited parms    */
  int intparms;         /* # of / delimited ints     */
  int floatparms;       /* # of / delimited floats   */
  int intval[64];       /* pre-parsed integer parms  */
  double floatval[16];  /* pre-parsed floating parms */
  char tmpc;
  int lastarg;
  char *tempstr;
  extern int major_method, minor_method;

  argptr = curarg;

#if 0
  while (*argptr)
    { /* convert to lower case */
    if(mode != 1) /* except when reading the ini file */
      {
      if (*argptr >= 'A' && *argptr <= 'Z')
        *argptr += 'a' - 'A';
      if (*argptr == '=' && strncmp(curarg,"colors=",7) == 0)
        /* don't convert colors=value */
        break;
      }
    ++argptr;
    }
#endif

  if ((value = strchr(&curarg[1],'=')))
    {
    if ((j = (value++) - curarg) > 1 && curarg[j-1] == ':')
      --j; /* treat := same as = */
    }
  else
    value = curarg + (j = strlen(curarg));

  if (j > 20)
    goto badarg;    /* keyword too long */

  strncpy(variable,curarg,j); /* get the variable name  */

  variable[j] = 0;            /* truncate variable name */

  valuelen = strlen(value);   /* note value's length    */

  charval = value[0];         /* first letter of value  */

  yesnoval = -1;              /* note yes/no value      */

  if (charval == 'n') yesnoval = 0;
  if (charval == 'y') yesnoval = 1;

  argptr = value;
  numval = totparms = intparms = floatparms = 0;
  while (*argptr)
    {            /* count and pre-parse parms */
    long ll;
    lastarg = 0;
    if ((argptr2 = strchr(argptr,'/')) == NULL)
      {     /* find next '/' */
      argptr2 = argptr + strlen(argptr);
      *argptr2 = '/';
      lastarg = 1;
      }
    if (totparms == 0)
      numval = NONNUMERIC;
    i = -1;
    charval = *argptr;            /* first letter of value  */
    if (charval == 'n')
      yesnoval = 0;    /* allows only ONE y/n param */
    if (charval == 'y')
      yesnoval = 1;
    if (sscanf(argptr,"%c%c",(char *) &j,&tmpc) > 0    /* NULL entry */
      && (j == '/' || j == '=') && tmpc == '/')
      {
      j = 0;
      ++floatparms; ++intparms;
      if (totparms < 16) floatval[totparms] = j;
      if (totparms < 64) intval[totparms] = j;
      if (totparms == 0) numval = j;
      }
    else if (sscanf(argptr,"%ld%c",&ll,&tmpc) > 0       /* got an integer */
        && tmpc == '/')
      {        /* needs a long int, ll, here for lyapunov */
      ++floatparms; ++intparms;
      if (totparms < 16) floatval[totparms] = ll;
      if (totparms < 64) intval[totparms] = (int)ll;
      if (totparms == 0) numval = (int)ll;
      }
    else if (sscanf(argptr,"%lf%c",&ftemp,&tmpc) > 0  /* got a float */
         && tmpc == '/')
      {
      ++floatparms;
      if (totparms < 16) floatval[totparms] = ftemp;
      }
    ++totparms;
    argptr = argptr2;                 /* on to the next */
    if (lastarg)
      *argptr = 0;
    else
      ++argptr;
    }

  /* these commands are allowed only at startup */
  if (mode != 2 || debugflag==110)
    {
    if ((int)strcmp(variable,"batch") == 0 )
      {     /* batch=?      */
      if (yesnoval < 0) goto badarg;
      initbatch = yesnoval;
      return 3;
      }

    if ((int)strcmp(variable, "exitnoask") == 0)
      {
      if (yesnoval < 0) goto badarg;
      escape_exit = yesnoval;
      return 3;
      }

    if ((int)strcmp(variable, "creditscreen") == 0)
      {
      if (yesnoval < 0) goto badarg;
      creditscreen = yesnoval;
      return 3;
      }

    if ((int)strcmp(variable, "disclaimer") == 0)
      {
      if (yesnoval < 0) goto badarg;
      disclaimer = yesnoval;
      return 3;
      }

    if ((int)strcmp(variable, "verbose") == 0)
      {
      if (yesnoval < 0) goto badarg;
      verbose = yesnoval;
      return 3;
      }

    if ((int)strcmp(variable, "rate") == 0)
      {
      if((numval < 1) || (numval > 100))
        goto badarg;
      refresh_rate = numval;
      return 3;
      }

    if ((int)strcmp(variable, "forwarding") == 0)
      {
      if (yesnoval < 0) goto badarg;
        goto badarg;
      forwarding = yesnoval;
      return 3;
      }

    if ((int)strcmp(variable, "undo") == 0)
      {
      if((numval < 0) || (numval > 32768))
        goto badarg;
      undo_max = numval;
      return 3;
      }

    if ((int)strcmp(variable, "fractdir") == 0)
      {
      int ret;
      struct stat st;
      char msg[1028];
      if (valuelen > 63) goto badarg;
      if(fractdir != NULL)
        XtFree(fractdir);
      if((fractdir = XtMalloc(MAXPATHLEN)) == (char *) NULL)
        {
        fprintf(stderr, "Sorry, out of memory (fractdir).\n");
        exit(1);
        }
      sprintf(fractdir, "%s", value);
      /*
       * if FRACTDIR or XMFRACTDIR is set in the environment,
       * make that value override the value in any command file
       */
      tempstr = getenv("XMFRACTDIR");
      if(tempstr == NULL) tempstr = getenv("FRACTDIR");
      if(tempstr != NULL)
        sprintf(fractdir, "%s", tempstr);

      if(((ret = stat(fractdir, &st)) == -1) ||
        ((st.st_mode & S_IFMT) != S_IFDIR))
        {
        sprintf((char *) &msg,
          "The \"fractdir\" directory \"%s\"\ndoes not exist. Set the environment variable \"FRACTDIR\"\nand/or the initialization file option \"fractdir=\" to\na valid directory that contains the ancillary files.", fractdir);
        ret = confirm(main_window, (char *) &msg, OK_BTN, XmDIALOG_OK_BUTTON);
        }
      return 3;
      }

    if ((int)strcmp(variable, "helpdir") == 0)
      {
      if (valuelen > 63) goto badarg;
      if(helpdir != NULL)
        XtFree(helpdir);
      if((helpdir = XtMalloc(MAXPATHLEN)) == (char *) NULL)
        {
        fprintf(stderr, "Sorry, out of memory (helpdir).\n");
        exit(1);
        }
      sprintf(helpdir, "%s", value);
      return 3;
      }
    } /* end of commands allowed only at startup */

  if ((int)strcmp(variable,"reset") == 0)
    {
    initvars_fractal();

    /* PAR release unknown unless specified */
    if (numval>=0)
      save_release = numval;
    else
      goto badarg;
    return 9;
    }

  if ((int)strcmp(variable,"filename") == 0)
    {      /* filename=?     */
    if (charval == '.')
      {
      if (valuelen > 4)
        goto badarg;
      gifmask[0] = '*';
      gifmask[1] = 0;
      strcat(gifmask,value);
      return 0;
      }

    if (valuelen >= MAXPATHLEN)
      goto badarg;

    if (mode == 2 && display3d == 0) /* can't do this in @ command */
      goto badarg;

    strcpy(readname,value);
    showfile = 0;
    gotfile = 1;
    return 3;
    }

  if ((int)strcmp(variable,"map") == 0 )
    {         /* map=, set default colors */
    if (valuelen >= MAXPATHLEN)
      goto badarg;
    if ((SetColorPaletteName(value)) != 0)
      return 0;
    mapset = 1;
    strcpy(MAP_name,value);
    return 0;
    }

  if ((int)strcmp(variable,"colors") == 0)
    {       /* colors=, set current colors */
    if (parse_colors(value) < 0)
      goto badarg;
    return 0;
    }

  if ((int)strcmp(variable, "maxcolorres") == 0)
    { /* Change default color resolution */
    if (numval == 1 || numval == 4 || numval == 8 ||
        numval == 16 || numval == 24)
      {
      MaxColorRes = numval;
      return 0;
      }
    goto badarg;
    }

  /* keep this for backward compatibility */
  if ((int)strcmp(variable,"warn") == 0 )
    {         /* warn=? */
    if (yesnoval < 0)
      goto badarg;
    overwrite_flag = yesnoval ^ 1;
    return 0;
    }

  if ((int)strcmp(variable,"overwrite") == 0 )
    {    /* overwrite=? */
    if (yesnoval < 0)
      goto badarg;
    overwrite_flag = yesnoval;
    return 0;
    }

  if ((int)strcmp(variable,"gif87a") == 0 )
    {       /* gif87a=? */
    if (yesnoval < 0)
      goto badarg;
    gif87a_flag = yesnoval;
    return 0;
    }

  if ((int)strcmp(variable,"dither") == 0 )
    {       /* dither=? */
    if (yesnoval < 0)
      goto badarg;
    dither_flag = yesnoval;
    return 0;
    }

  if ((int)strcmp(variable,"savetime") == 0)
    {      /* savetime=? */
    initsavetime = numval;
    return 0;
    }

  if ((int)strcmp(variable,"autokey") == 0)
    {       /* autokey=? */
    if ((int)strcmp(value,"record")==0)
      slides=2;
    else if ((int)strcmp(value,"play")==0)
      slides=1;
    else
      goto badarg;
    return 0;
    }

  if((int)strcmp(variable, "solidcore") == 0)
    {
    SolidCore = yesnoval;
    return(0);
    }

  if((int)strcmp(variable, "transparent3d") == 0)
    {
    Transparent3D = yesnoval;
    return(0);
    }

  if((int)strcmp(variable, "corecolor") == 0)
    {
    if(floatparms != totparms || totparms != 3)
      goto badarg;
    CoreRed    = (int)floatval[0];
    CoreGreen = (int)floatval[1];
    CoreBlue    = (int)floatval[2];
    return(0);
    }

  if((int)strcmp(variable, "mdcorners") == 0)
    {
    if(floatparms != totparms || totparms < 2 || totparms > 4)
      goto badarg;
    zzmin = floatval[0];
    zzmax = floatval[1];
    if(totparms >= 3)
      ttmin = floatval[2];
    if(totparms == 4)
      ttmax = floatval[3];
    return(0);
    }

  if((int)strcmp(variable, "numframes") == 0)
    {
    NumFrames = numval;
    return(0);
    }

  if ((int)strcmp(variable,"type") == 0 )
    {         /* type=? */
    if (value[valuelen-1] == '*')
      value[--valuelen] = 0;
    /* kludge because type ifs3d has an asterisk in front */
    if((int)strcmp(value,"ifs3d")==0)
      value[3]=0;
    for (k = 0; fractalspecific[k].name != NULL; k++)
      if ((int)strcmp(value,fractalspecific[k].name) == 0)
        break;
    if (fractalspecific[k].name == NULL)
      goto badarg;
    curfractalspecific = &fractalspecific[fractype = k];
    if (initcorners == 0)
      {
      xx3rd = xxmin = curfractalspecific->xmin;
      xxmax         = curfractalspecific->xmax;
      yy3rd = yymin = curfractalspecific->ymin;
      yymax         = curfractalspecific->ymax;
      }
    if (initparams == 0)
      {
      for (k = 0; k < 4; ++k)
        {
        param[k] = curfractalspecific->paramvalue[k];
        if(fractype != CELLULAR) /* don't round cellular */
          roundfloatd(&param[k]);
        }
      if(curfractalspecific->flags&MORE)
        {
        int extra;
        if((extra=find_extra_param(fractype)) > -1)
          for(i=0;i<MAXPARAMS-4;i++) {
            param[i+4] = moreparams[extra].paramvalue[i];
          }
        }
      }
    return 1;
    }

  if ((int)strcmp(variable,"inside") == 0 )
    {       /* inside=? */
    if((int)strcmp(value,s_zmag)==0)
      inside = -59;
    else if((int)strcmp(value,s_bof60)==0)
      inside = -60;
    else if((int)strcmp(value,s_bof61)==0)
      inside = -61;
    else if(strncmp(value,s_epscross,3)==0)
      inside = -100;
    else if(strncmp(value,s_startrail,4)==0)
      inside = -101;
    else if(strncmp(value,s_period,3)==0)
      inside = -102;
    else if((int)strcmp(value,s_maxiter)==0)
      inside = -1;
    else if(numval == NONNUMERIC)
      goto badarg;
    else
      inside = numval;
    return 1;
    }

  if ((int)strcmp(variable,"fillcolor") == 0 )
    {       /* fillcolor */
    if((int)strcmp(value,s_normal)==0)
      fillcolor = -1;
    else if(numval == NONNUMERIC)
      goto badarg;
    else
      fillcolor = numval;
    return 1;
    }

  if ((int)strcmp(variable,"finattract") == 0 )
    {   /* finattract=? */
    if (yesnoval < 0)
      goto badarg;
    finattract = yesnoval;
    return 1;
    }

  if ((int)strcmp(variable,"function") == 0)
    {      /* function=?,? */
    k = 0;
    while (*value && k < 4)
      {
      if(set_trig_array(k++,value))
        goto badarg;
      if ((value = strchr(value,'/')) == NULL) break;
        ++value;
      }
    functionpreloaded = 1; /* for old bifs  JCO 7/5/92 */
    return 1;
    }

  if ((int)strcmp(variable,"outside") == 0 )
    {      /* outside=? */
    if((int)strcmp(value,s_iter)==0)
      outside = -1;
    else if((int)strcmp(value,s_real)==0)
      outside = -2;
    else if((int)strcmp(value,s_imag)==0)
      outside = -3;
    else if((int)strcmp(value,s_mult)==0)
      outside = -4;
    else if((int)strcmp(value,s_sum)==0)
      outside = -5;
    else if(numval == NONNUMERIC)
      goto badarg;
    else if(numval < -5 || numval > 255)
      goto badarg;
    else
      outside = numval;
    return 1;
    }

  if ((int)strcmp(variable,s_maxiter) == 0)
    {       /* maxiter=? */
    if (numval < 2)
      goto badarg;
    maxit = numval;
    return 1;
    }

  if ((int)strcmp(variable,"iterincr") == 0)        /* iterincr=? */
    return 0;

  if ((int)strcmp(variable,"passes") == 0)
    {        /* passes=? */
    if ( charval != '1' && charval != '2'
      && charval != 'g' && charval != 'b'
      && charval != 't')
      {
      goto badarg;
      }
    usr_stdcalcmode = charval;
    return 1;
    }

  if ((int)strcmp(variable,"makemig") == 0)
    {
    int ret, xmult, ymult;
    if (totparms < 2)
      goto badarg;
    xmult = intval[0];
    ymult = intval[1];
    if((ret = make_mig(xmult, ymult) == 0))
      return 1;
    else
      return -1;
    }

  if ((int)strcmp(variable,"cyclerange") == 0)
     {
     if (totparms < 2)
       intval[1] = 255;
     if (totparms < 1)
       intval[0] = 1;
     if (totparms != intparms ||
        intval[0] < 0 || intval[1] > 255 ||
        intval[0] > intval[1])
       {
       goto badarg;
       }
     if((intval[0] > (colors - 2)) || (intval[1] > (colors - 1)))
       goto badarg;
     rotate_lo = intval[0];
     rotate_hi = intval[1];
     return 0;
     }

  if ((int)strcmp(variable,"ranges") == 0)
    {
    int i,j,entries,prev;
    int tmpranges[128];
    if (totparms != intparms)
      goto badarg;
    entries = prev = i = 0;
    while (i < totparms)
      {
      if ((j = intval[i++]) < 0)
        { /* striping */
        if ((j = 0-j) < 1 || j >= 16384 || i >= totparms)
          goto badarg;
        tmpranges[entries++] = -1; /* {-1,width,limit} for striping */
        tmpranges[entries++] = j;
        j = intval[i++];
        }
      if (j < prev)
        goto badarg;
      tmpranges[entries++] = prev = j;
      }
    if (prev == 0)
      goto badarg;
    if ((ranges = (int *)XtMalloc(2L*entries)) == NULL)
      {
      static char msg[] = {"Insufficient memory for ranges="};
      warning(main_window, (char *) &msg, NULL);
      return(-1);
      }
    rangeslen = entries;
    for (i = 0; i < rangeslen; ++i)
      ranges[i] = tmpranges[i];
    return 1;
    }

  if ((int)strcmp(variable,"savename") == 0)
    {      /* savename=? */
    if (valuelen >= MAXPATHLEN)
      goto badarg;
    if (first_init || mode == 2)
      strcpy(savename,value);
    return 0;
    }

  if ((int)strcmp(variable,"potential") == 0)
    {     /* potential=? */
    k = 0;
    while (k < 3 && *value)
      {
      if(k==1)
        potparam[k] = atof(value);
      else
        potparam[k] = atoi(value);
      k++;
      if ((value = strchr(value,'/')) == NULL) k = 99;
        ++value;
      }
    pot16bit = 0;
    if (k < 99)
      {
      if ((int)strcmp(value,"16bit"))
        goto badarg;
      pot16bit = 1;
      }
    return 1;
    }

  if ((int)strcmp(variable,"params") == 0) {        /* params=?,? */
     if (totparms != floatparms || totparms > MAXPARAMS)
    goto badarg;
     for (k = 0; k < MAXPARAMS; ++k)
    param[k] = (k < totparms) ? floatval[k] : 0.0;
     initparams = 1;
     return 1;
     }

  if ((int)strcmp(variable, "miim") == 0)
    {        /* miim=?[/?[/?[/?]]] */
    k = 0;
    do
      {
      if ((int)isdigit(*value) ||
        *value == '.' || *value == '-' || *value == '+')
        {
        if (k >= 4)
          goto badarg;
        param[k++] = atof(value);
        }
      else if (strncmp(value, "breadth", 7) == 0)
        major_method = 0;
      else if (strncmp(value, "depth",   5) == 0)
        major_method = 1;
      else if (strncmp(value, "walk",    4) == 0)
        major_method = 2;
      else if (strncmp(value, "run",     3) == 0)
        major_method = 3;
      else if (strncmp(value, "left",    4) == 0)
        minor_method = 0;
      else if (strncmp(value, "right",   5) == 0)
        minor_method = 1;
      else
        goto badarg;
      value = strchr(value, '/');
      } while (value++);
    return 1;
    }

  if ((int)strcmp(variable,"initorbit") == 0)
    {     /* initorbit=?,? */
    if((int)strcmp(value,"pixel")==0)
      useinitorbit = 2;
    else
      {
      if (totparms != 2 || floatparms != 2)
        goto badarg;
      dinitorbit[0] = floatval[0];
      dinitorbit[1] = floatval[1];
      useinitorbit = 1;
      }
    return 1;
    }

  if ((int)strcmp(variable,"orbitname") == 0 )
    {         /* orbitname=? */
    if(check_orbit_name(value))
      goto badarg;
    return 1;
    }

  if ((int)strcmp(variable,"3dmode") == 0 )
    {         /* orbitname=? */
    int i,j;
    j = -1;
    for(i=0;i<4;i++)
      if((int)strcmp(value,juli3Doptions[i])==0)
        j = i;
    if(j < 0)
      goto badarg;
    else
      juli3Dmode = j;
    return 1;
    }

  if ((int)strcmp(variable,"julibrot3d") == 0)
    {       /* julibrot3d=?,?,?,? */
    if (floatparms != totparms)
      goto badarg;
    if(totparms > 0)
      zdots = floatval[0];
    if (totparms > 1)
      originfp = floatval[1];
    if (totparms > 2)
      depthfp = floatval[2];
    if (totparms > 3)
      heightfp = floatval[3];
    if (totparms > 4)
      widthfp = floatval[4];
    if (totparms > 5)
      distfp = floatval[5];
    return 1;
    }

  if ((int)strcmp(variable,"julibroteyes") == 0)
    {     /* julibroteyes=?,?,?,? */
    if (floatparms != totparms || totparms != 1)
      goto badarg;
    eyesfp =  floatval[0];
    return 1;
    }

  if ((int)strcmp(variable,"julibrotfromto") == 0)
    {   /* julibrotfromto=?,?,?,? */
    if (floatparms != totparms || totparms != 4)
      goto badarg;
    mxmaxfp = floatval[0];
    mxminfp = floatval[1];
    mymaxfp = floatval[2];
    myminfp = floatval[3];
    return 1;
    }

  if ((int)strcmp(variable,"corners") == 0)
    {       /* corners=?,?,?,? */
    if (fractype == CELLULAR)
      return 1; /* skip setting the corners */
    if (floatparms != totparms || (totparms != 4 && totparms != 6))
      goto badarg;
    usemag = 0;
    initcorners = 1;
    xx3rd = xxmin = floatval[0];
    xxmax =         floatval[1];
    yy3rd = yymin = floatval[2];
    yymax =         floatval[3];
    if (totparms == 6)
      {
      xx3rd =      floatval[4];
      yy3rd =      floatval[5];
      }
    return 1;
    }

  if ((int)strcmp(variable,"viewwindows") == 0)
    {  /* viewwindows=?,?,?,?,? */
    if (totparms > 5 || floatparms-intparms > 2 || intparms > 4)
      goto badarg;
    viewwindow = 1;
    viewreduction = 4.2;  /* reset default values */
    finalaspectratio = screenaspect;
    viewcrop = 1; /* yes */
    viewxdots = viewydots = 0;

    if((totparms > 0) && (floatval[0] > 0.001))
      viewreduction = floatval[0];
    if((totparms > 1) && (floatval[1] > 0.001))
      finalaspectratio = floatval[1];
    if((totparms > 2) && (yesnoval == 0))
      viewcrop = yesnoval;
    if((totparms > 3) && (intval[3] > 0))
      viewxdots = intval[3];
    if((totparms == 5) && (intval[4] > 0))
      viewydots = intval[4];
    return 1;
    }

  if ((int)strcmp(variable,"center-mag") == 0)
    {    /* center-mag=?,?,? */
    double Xctr, Yctr,Magnification,Ratio,Height, Width,Radius;
    if (totparms != floatparms ||
       (totparms != 0 && totparms != 3) ||
       (totparms == 3 && floatval[2] <= 0.0))
      {
      goto badarg;
      }
    usemag = 1;
    if (totparms == 0)
      return 0;
    initcorners = 1;
    Xctr = floatval[0];
    Yctr = floatval[1];
    Magnification = floatval[2];
    Radius = 1.0 / Magnification;
    Ratio = .75;    /* inverse aspect ratio of screen  */
    /* calculate bounds */
    Height = 2.0 * Radius;
    Width = Height / Ratio;
    yymax = Yctr + Radius;
    yy3rd = yymin = Yctr - Radius;
    xxmax = Xctr + Width / 2.0;
    xx3rd = xxmin = Xctr - Width / 2.0;
    return 1;
    }

  if ((int)strcmp(variable,"invert") == 0)
    {        /* invert=?,?,? */
    if (totparms != floatparms || (totparms != 1 && totparms != 3))
      goto badarg;
    invert = ((inversion[0] = floatval[0]) != 0.0) ? totparms : 0;
    if (totparms == 3)
      {
      inversion[1] = floatval[1];
      inversion[2] = floatval[2];
      }
    return 1;
    }

  if ((int)strcmp(variable,"ramvideo") == 0 )       /* ramvideo=?   */
     return 0; /* just ignore and return, for old time's sake */

  if ((int)strcmp(variable,"float") == 0 )
    {        /* float=? */
    if (yesnoval < 0)
      goto badarg;
    usr_floatflag = yesnoval;
    return 3;
    }

  if ((int)strcmp(variable,"biomorph") == 0 )
    {     /* biomorph=? */
    usr_biomorph = numval;
    return 1;
    }

  if ((int)strcmp(variable,"orbitsave") == 0 )
    {     /* orbitsave=? */
    if (yesnoval < 0)
      goto badarg;
    orbitsave = yesnoval;
    return 1;
    }

  if ((int)strcmp(variable,"bailout") == 0 )
    {      /* bailout=? */
    if (numval < 4 || numval > 32000)
      goto badarg;
    bailout = numval;
    return 1;
    }

  if ((int)strcmp(variable,"symmetry") == 0 )
    {     /* symmetry=? */
    if     ((int)strcmp(value,"xaxis" )==0)
      forcesymmetry = XAXIS;
    else if((int)strcmp(value,"yaxis" )==0)
      forcesymmetry = YAXIS;
    else if((int)strcmp(value,"xyaxis")==0)
      forcesymmetry = XYAXIS;
    else if((int)strcmp(value,"origin")==0)
      forcesymmetry = ORIGIN;
    else if((int)strcmp(value,"pi"    )==0)
      forcesymmetry = PI_SYM;
    else if((int)strcmp(value,"none"  )==0)
      forcesymmetry = NOSYM;
    else
      goto badarg;
    return 1;
    }

  if ((int)strcmp(variable,"sound") == 0 )
    {        /* sound=? */
    soundflag = 0;
     if (strncmp(value,"ye",2) == 0)
     {
     soundflag = -1;
     return(0);
     }
    if (charval == 'x')
      soundflag = 1;
    if (charval == 'y')
      soundflag = 2;
    if (charval == 'z')
      soundflag = 3;
    return 0;
    }

  if ((int)strcmp(variable,"periodicity") == 0 )
    {  /* periodicity=? */
    usr_periodicitycheck=1;
    if ((charval == 'n') || (numval == 0))
      usr_periodicitycheck=0;
    else if (charval == 'y')
      usr_periodicitycheck=1;
    else if (charval == 's')   /* 's' for 'show' */
      usr_periodicitycheck= -1;
    else if(numval == NONNUMERIC)
      goto badarg;
    else if(numval != 0)
      usr_periodicitycheck=numval;
    return 1;
    }

  if ((int)strcmp(variable,"logmap") == 0 )
    {       /* logmap=? */
    if (charval == 'y')
      LogFlag = 1;                /* palette is logarithmic */
    else if (charval == 'n')
      LogFlag = 0;
    else if (charval == 'o')
      LogFlag = -1;                /* old log palette */
    else
      LogFlag = numval;
    return 1;
    }

  if ((int)strcmp(variable,"debugflag") == 0 ||
      (int)strcmp(variable,"debug") == 0)
    {        /* internal use only */
    debugflag = numval;
    timerflag = debugflag & 1;        /* separate timer flag */
    debugflag -= timerflag;
    return 0;
    }

  if ((int)strcmp(variable, "rseed") == 0)
    {
    rseed = numval;
    rflag = 1;
    return 1;
    }

  if ((int)strcmp(variable, "orbitdelay") == 0)
    {
    orbit_delay = numval;
    return 0;
    }

  if ((int)strcmp(variable, "showdot") == 0)
    {
    showdot=numval;
    if(showdot<0)
      showdot=0;
    return 0;
    }

  if ((int)strcmp(variable, "decomp") == 0)
    {
    if (totparms != intparms || totparms < 1)
      goto badarg;
    decomp[0] = intval[0];
    decomp[1] = 0;
    if (totparms > 1) /* backward compatibility */
      bailout = decomp[1] = intval[1];
    return 1;
    }

  if ((int)strcmp(variable, "distest") == 0)
    {
    if (totparms != intparms || totparms < 1)
      goto badarg;
    usr_distest = intval[0];
    distestwidth = 71;
    if (totparms > 1)
      distestwidth = intval[1];
    return 1;
    }

  if ((int)strcmp(variable,"formulafile") == 0)
    {   /* formulafile=? */
    if (valuelen >= MAXPATHLEN)
      goto badarg;
    strcpy(FormFileName,value);
    gotfrmfile = 1;
    return 1;
    }

  if ((int)strcmp(variable,"formulaname") == 0)
    {   /* formulaname=? */
    if (valuelen > ITEMNAMELEN)
      goto badarg;
    strcpy(FormName,value);
    gotfrmname = 1;
    return 1;
    }

  if ((int)strcmp(variable,"lfile") == 0)
    {
    if (valuelen >= MAXPATHLEN)
      goto badarg;
    strcpy(LFileName,value);
    gotlsysfile = 1;
    return 1;
    }

  if ((int)strcmp(variable,"lname") == 0)
    {
    if (valuelen > ITEMNAMELEN)
      goto badarg;
    strcpy(LName,value);
    gotlsysname = 1;
    return 1;
    }

  if ((int)strcmp(variable,"ifsfile") == 0)
    {
    if (valuelen >= MAXPATHLEN)
      goto badarg;
    strcpy(IFSFileName,value);
    reset_ifs_defn();
    gotifsfile = 1;
    return 1;
    }

  if ((int)strcmp(variable,"ifs") == 0 ||
      (int)strcmp(variable,"ifs3d") == 0)
    {        /* ifs3d for old time's sake */
    if (valuelen > ITEMNAMELEN)
      goto badarg;
    strcpy(IFSName,value);
    reset_ifs_defn();
    gotifsname = 1;
    return 1;
    }

  if ((int)strcmp(variable,"parmfile") == 0)
    {
    if (valuelen >= MAXPATHLEN)
      goto badarg;
    strcpy(CommandFile,value);
    return 0;
    }

  if ((int)strcmp(variable,"stereo") == 0)
    {        /* stereo=? */
    if ((numval<0) || (numval>3))
      goto badarg;
    glassestype = numval;
    return 3;
    }

  if ((int)strcmp(variable,"rotation") == 0)
    {      /* rotation=?/?/? */
    if (totparms != 3 || intparms != 3)
      goto badarg;
    XROT = intval[0];
    YROT = intval[1];
    ZROT = intval[2];
    return 3;
    }

  if ((int)strcmp(variable,"perspective") == 0)
    {   /* perspective=? */
    if (numval == NONNUMERIC)
      goto badarg;
    ZVIEWER = numval;
    return 3;
    }

  if ((int)strcmp(variable,"xyshift") == 0)
    {       /* xyshift=?/?  */
    if (totparms != 2 || intparms != 2)
      goto badarg;
    XSHIFT = intval[0];
    YSHIFT = intval[1];
    return 3;
    }

  if ((int)strcmp(variable,"interocular") == 0)
    {   /* interocular=? */
    eyeseparation = numval;
    return 3;
    }

  if ((int)strcmp(variable,"converge") == 0)
    {      /* converg=? */
    xadjust = numval;
    return 3;
    }

  if ((int)strcmp(variable,"crop") == 0)
    {          /* crop=? */
    if (totparms != 4 || intparms != 4 ||
        intval[0] < 0 || intval[0] > 100 ||
        intval[1] < 0 || intval[1] > 100 ||
        intval[2] < 0 || intval[2] > 100 ||
        intval[3] < 0 || intval[3] > 100)
      {
      goto badarg;
      }
    red_crop_left   = intval[0];
    red_crop_right  = intval[1];
    blue_crop_left  = intval[2];
    blue_crop_right = intval[3];
    return 3;
    }

  if ((int)strcmp(variable,"bright") == 0)
    {        /* bright=? */
    if (totparms != 2 || intparms != 2)
      goto badarg;
    red_bright  = intval[0];
    blue_bright = intval[1];
    return 3;
    }

  if ((int)strcmp(variable,"xyadjust") == 0)
    {      /* trans=? */
    if (totparms != 2 || intparms != 2)
      goto badarg;
    xtrans = intval[0];
    ytrans = intval[1];
    return 3;
    }

  if ((int)strcmp(variable,"3d") == 0)
    {            /* 3d=?/?/..    */
    if((int)strcmp(value,"overlay")==0)
      {
      yesnoval=1;
      if(calc_status > IMG_PRMS_INIT) /* if no image, treat same as 3D=yes */
        overlay3d=1;
      }
    else if (yesnoval < 0)
      goto badarg;
    display3d = yesnoval;
    initvars_3d();
    return (display3d) ? 6 : 2;
    }

  if ((int)strcmp(variable,"sphere") == 0 )
    {       /* sphere=? */
    if (yesnoval < 0)
      goto badarg;
    SPHERE = yesnoval;
    return 2;
    }

  if ((int)strcmp(variable,"scalexyz") == 0)
    {      /* scalexyz=?/?/? */
    if (totparms < 2 || intparms != totparms)
      goto badarg;
    XSCALE = intval[0];
    YSCALE = intval[1];
    if (totparms > 2)
      ROUGH = intval[2];
    return 2;
    }

  /* "rough" is really scale z, but we add it here for convenience */

  if ((int)strcmp(variable,"roughness") == 0)
    {     /* roughness=?  */
    ROUGH = numval;
    return 2;
    }

  if ((int)strcmp(variable,"waterline") == 0)
    {     /* waterline=?  */
    if (numval<0)
      goto badarg;
    WATERLINE = numval;
    return 2;
    }

  if ((int)strcmp(variable,"filltype") == 0)
    {      /* filltype=?   */
    if (numval < -1 || numval > 6)
      goto badarg;
    FILLTYPE = numval;
    return 2;
    }

  if ((int)strcmp(variable,"lightsource") == 0)
    {   /* lightsource=?/?/? */
    if (totparms != 3 || intparms != 3)
      goto badarg;
    XLIGHT = intval[0];
    YLIGHT = intval[1];
    ZLIGHT = intval[2];
    return 2;
    }

  if ((int)strcmp(variable,"smoothing") == 0)
    {     /* smoothing=?  */
    if (numval<0)
      goto badarg;
    LIGHTAVG = numval;
    return 2;
    }

  if ((int)strcmp(variable,"latitude") == 0)
    {      /* latitude=?/? */
    if (totparms != 2 || intparms != 2)
      goto badarg;
    THETA1 = intval[0];
    THETA2 = intval[1];
    return 2;
    }

  if ((int)strcmp(variable,"longitude") == 0)
    {     /* longitude=?/? */
    if (totparms != 2 || intparms != 2)
      goto badarg;
    PHI1 = intval[0];
    PHI2 = intval[1];
    return 2;
    }

  if ((int)strcmp(variable,"radius") == 0)
    {        /* radius=? */
    if (numval < 0)
      goto badarg;
    RADIUS = numval;
    return 2;
    }

  if ((int)strcmp(variable,"transparent") == 0)
    {   /* transparent? */
    if (totparms != intparms || totparms < 1)
      goto badarg;
    transparent[1] = transparent[0] = intval[0];
    if (totparms > 1)
      transparent[1] = intval[1];
    return 2;
    }

  if ((int)strcmp(variable,"preview") == 0)
    {       /* preview? */
    if (yesnoval < 0)
      goto badarg;
    preview = yesnoval;
    return 2;
    }

  if ((int)strcmp(variable,"showbox") == 0)
    {       /* showbox? */
    if (yesnoval < 0)
      goto badarg;
    showbox = yesnoval;
    return 2;
    }

  if ((int)strcmp(variable,"coarse") == 0)
    {        /* coarse=? */
    if (numval < 3 || numval > 2000)
      goto badarg;
    previewfactor = numval;
    return 2;
    }

  if ((int)strcmp(variable,"randomize") == 0)
    {     /* RANDOMIZE=? */
    if (numval<0 || numval>7)
      goto badarg;
    RANDOMIZE = numval;
    return 2;
    }

  if ((int)strcmp(variable,"ambient") == 0)
    {       /* ambient=? */
    if (numval<0||numval>100)
      goto badarg;
    Ambient = numval;
    return 2;
    }

  if ((int)strcmp(variable,"haze") == 0)
    {          /* haze=? */
    if (numval<0||numval>100)
      goto badarg;
    haze = numval;
    return 2;
    }

  if ((int)strcmp(variable,"fullcolor") == 0)
    {     /* fullcolor=? */
    if (yesnoval < 0)
      goto badarg;
    Targa_Out = yesnoval;
    return 2;
    }

  if ((int)strcmp(variable,"targa_out") == 0)
    {     /* Targa Out? */
    if (yesnoval < 0)
      goto badarg;
    Targa_Out = yesnoval;
    return 2;
    }

  if ((int)strcmp(variable,"targa_overlay") == 0)
    { /* Targa Overlay? */
    if (yesnoval < 0)
      goto badarg;
    Targa_Overlay = yesnoval;
    return 2;
    }

  if ((int)strcmp(variable,"background") == 0)
    {    /* background=?/? */
    if (totparms != 3 || intparms != 3)
      goto badarg;
    for (i=0;i<3;i++)
      if (intval[i] & 0xff)
        goto badarg;
    back_color[0] = intval[0];
    back_color[1] = intval[1];
    back_color[2] = intval[2];
    return 2;
    }

  if ((int)strcmp(variable,"lightname") == 0)
    {     /* lightname=?   */
    if (valuelen >= MAXPATHLEN)
      goto badarg;
    if (first_init || mode == 2)
      strcpy(light_name,value);
    return 0;
    }

  if ((int)strcmp(variable,"ray") == 0)
    {           /* RAY=? */
    if (numval < 0 || numval > 6)
      goto badarg;
    RAY = numval;
    return 2;
    }

  if ((int)strcmp(variable,"brief") == 0)
    {         /* BRIEF? */
    if (yesnoval < 0)
      goto badarg;
    BRIEF = yesnoval;
    return 2;
    }

  if ((int)strcmp(variable,"release") == 0)
    {       /* release */
    if (numval < 0)
      goto badarg;
    save_release = numval;
    return 2;
    }

  /*
   * The following variables are listed here for compatibility purposes only.
   * The arguments are ignored - the program doesn't need them or cannot
   * currently deal with them.
   */

  if ((int)strcmp(variable, "adapter") == 0 )
    return 0;

  if ((int)strcmp(variable, "afi") == 0 )
    return 0;

  if ((int)strcmp(variable, "textsafe") == 0 )
    return 0;

  if ((int)strcmp(variable, "vesadetect") == 0 )
    return 0;

  if ((int)strcmp(variable, "fpu") == 0 )
    return 0;

  if ((int)strcmp(variable, "biospalette") == 0 )
    return 0;

  if ((int)strcmp(variable, "noiit") == 0 )
    return 0;

  if ((int)strcmp(variable, "387") == 0 )
    return 0;

  if ((int)strcmp(variable, "makedoc") == 0 )
    return 0;

  if ((int)strcmp(variable, "video") == 0 )
    return 0;

  if ((int)strcmp(variable, "tplus") == 0 )
    return 0;

  if ((int)strcmp(variable, "noninterlaced") == 0 )
    return 0;

  if ((int)strcmp(variable, "pixelzoom") == 0 )
    return 0;

  if ((int)strcmp(variable, "autokeyname") == 0 )
    return 0;

  if ((int)strcmp(variable, "textcolors") == 0 )
    return 0;

  if ((int)strcmp(variable, "askvideo") == 0 )
    return 0;

  if ((int)strcmp(variable, "printer") == 0 )
    return 0;

  if ((int)strcmp(variable, "printfile") == 0 )
    return 0;

  if ((int)strcmp(variable, "rleps") == 0 )
    return 0;

  if ((int)strcmp(variable, "colorps") == 0 )
    return 0;

  if ((int)strcmp(variable, "epsf") == 0 )
    return 0;

  if ((int)strcmp(variable, "title") == 0 )
    return 0;

  if ((int)strcmp(variable, "translate") == 0 )
    return 0;

  if ((int)strcmp(variable, "plotstyle") == 0 )
    return 0;

  if ((int)strcmp(variable, "halftone") == 0 )
    return 0;

  if ((int)strcmp(variable, "linefeed") == 0 )
    return 0;

  if ((int)strcmp(variable, "comport") == 0 )
    return 0;

  if ((int)strcmp(variable, "hertz") == 0 )
    return 0;

badarg:
  argerror(curarg);
  return(-1);
}

int load_commands(FILE *infile)
{
   /* when called, file is open in binary mode, positioned at the */
   /* '(' or '{' following the desired parameter set's name       */
   int ret;
   initcorners = initparams = 0; /* reset flags for type= */
   ret = cmdfile(infile,2);
   return ret;
}

static void reset_ifs_defn()
{
  if (ifs_defn)
    {
    XtFree((char *)ifs_defn);
    ifs_defn = NULL;
    }
}

static void initvars_3d()       /* init vars affecting 3d */
{
  RAY     = 0;
  BRIEF   = 0;
  SPHERE = FALSE;
  preview = 0;
  showbox = 0;
  xadjust = 0;
  yadjust = 0;
  eyeseparation = 0;
  glassestype = 0;
  previewfactor = 20;
  red_crop_left   = 4;
  red_crop_right  = 0;
  blue_crop_left  = 0;
  blue_crop_right = 4;
  red_bright     = 80;
  blue_bright   = 100;
  transparent[0] = transparent[1] = 0; /* no min/max transparency */
  set_3d_defaults();
}

void set_3d_defaults()
{
  ROUGH     = 30;
  WATERLINE = 0;
  ZVIEWER   = 0;
  XSHIFT    = 0;
  YSHIFT    = 0;
  xtrans    = 0;
  ytrans    = 0;
  LIGHTAVG  = 0;
  Ambient   = 20;
  RANDOMIZE = 0;
  haze      = 0;
  back_color[0] = 51;
  back_color[1] = 153;
  back_color[2] = 200;
  if(SPHERE)
    {
    PHI1     = 180;
    PHI2     = 0;
    THETA1   = -90;
    THETA2   = 90;
    RADIUS   = 100;
    FILLTYPE = 2;
    XLIGHT   = 1;
    YLIGHT   = 1;
    ZLIGHT   = 1;
    }
  else
    {
    XROT  = 60;
    YROT  = 30;
    ZROT  = 0;
    XSCALE = 90;
    YSCALE = 90;
    FILLTYPE = 0;
    XLIGHT = 1;
    YLIGHT = -1;
    ZLIGHT = 1;
    }
}

void initvars_fractal()     /* init vars affecting calculation */
{
  int i;
  usr_periodicitycheck = 1; /* turn on periodicity      */
  inside = 1;               /* inside color = blue      */
  fillcolor = -1;           /* no special fill color */
  usr_biomorph = -1;        /* turn off biomorph flag */
  outside = -1;             /* outside color = -1 (not used) */
  maxit = 127;              /* initial maxiter      */
  usr_stdcalcmode = 'g';    /* initial solid-guessing */
  usr_floatflag = '1';        /* turn on the float flag */
  finattract = 0;           /* disable finite attractor logic */
  fractype = 0;             /* initial type Set flag  */
  curfractalspecific = &fractalspecific[0];
  initcorners = initparams = 0;
  bailout = 0;              /* no user-entered bailout */
  useinitorbit = 0;
  for (i = 0; i < MAXPARAMS; i++) param[i] = 0.0; /* initial parm values */
  for (i = 0; i < 3; i++) potparam[i]    = 0.0;   /* initial pot values */
  for (i = 0; i < 3; i++) inversion[i] = 0.0;     /* initial invert values */
  dinitorbit[0] = dinitorbit[1] = 0.0;            /* initial orbit values */
  invert = 0;
  decomp[0] = decomp[1] = 0;
  usr_distest = 0;
  distestwidth = 71;
  forcesymmetry = 999;                  /* symmetry not forced */
  xx3rd = xxmin = -2.5; xxmax = 1.5;    /* initial corner values  */
  yy3rd = yymin = -1.5; yymax = 1.5;    /* initial corner values  */
  pot16bit = potflag = 0;
  LogFlag = 0;                          /* logarithmic palette */
  set_trig_array(0,"sin");              /* trigfn defaults */
  set_trig_array(1,"sqr");
  set_trig_array(2,"sinh");
  set_trig_array(3,"cosh");
  if (rangeslen)
    {
    XtFree((char *)ranges);
    rangeslen = 0;
    }
  usemag = 0; /* use corners, not center-mag */

  colorstate = colorpreloaded = 0;
  rotate_lo = 0;
  rotate_hi = colors - 1;

  extended_opts.dflt.rotate_hi = rotate_hi;
  extended_opts.current = extended_opts.dflt;

  display3d = 0;            /* 3D display is off       */
  overlay3d = 0;            /* 3D overlay is off       */

  functionpreloaded = 0; /* for old bifs  JCO 7/5/92 */
  mxminfp = -.83;
  myminfp = -.25;
  mxmaxfp = -.83;
  mymaxfp =  .25;
  originfp = 8;
  heightfp = 7;
  widthfp = 10;
  distfp = 24;
  eyesfp = 2.5;
  depthfp = 8;
  lastorbittype = -1;
  neworbittype = JULIA;
  zdots = 128;
  initvars_3d();

  /*
   * This tells the zoom code that the currently displayed file is
   * a GIF that does not have any fractal info, and cannot be zoomed.
   * It gets reset here before and/or after a fractal type is selected.
   * It gets set to GIFFILE when reading a gif that doesn't have fractal info.
   */

  sprintf((char *) &ZoomName, "NOTGFILE");
}

static void argerror(char *badarg)  /* oops. couldn't decode this */
{
   static char argerrmsg1[]={"\
Oops. I couldn't understand the argument:\n  "};
   static char argerrmsg2[]={"\n\n\
(see the Startup Help screens or documentation for a complete\n\
 argument list with descriptions)"};
  char msg[300];
  if(!(XtIsManaged(main_window)))
    {
    if ((int)(strlen(badarg)) > 70)
      badarg[70] = 0;
    if (first_init)
      {
      /* this is 1st call to cmdfiles */
      sprintf(msg,"%s%s%s",argerrmsg1,badarg,argerrmsg2);
      }
    else
      {
      sprintf(msg,"%s%s",argerrmsg1,badarg);
      }
    fprintf(stderr, "%s", msg);
    }
  else
    {
    sprintf(msg,"%s%s",argerrmsg1,badarg);
    caution(main_window, (char *) &msg, NULL);
    }
}

static int file_gets(char *buf,int maxlen,FILE *infile)
{
  int len,c;
  /* similar to 'fgets', but file may be in either text or binary mode */
  /* returns -1 at eof, length of string otherwise */
  if (feof(infile))
    return -1;
  len = 0;
  while (len < maxlen)
    {
    if ((c = getc(infile)) == EOF || c == '\032')
      {
      if (len)
        break;
      return -1;
      }
    if (c == '\n')
      break;             /* linefeed is end of line */
    if (c != '\r')
      buf[len++] = c;    /* ignore c/r */
    }
  buf[len] = '\0';
  return len;
}

int parse_colors(char *value)
{
  int i,j,k;
  if (*value == '@')
    {
    if ((int)(strlen(value)) >= MAXPATHLEN || ValidateLuts(&value[1]) != 0)
      goto badcolor;
    if (display3d)
      {
      mapset = 1;
      strcpy(MAP_name,&value[1]);
      }
    else
      {
      strcpy(colorfile,&value[1]);
      colorstate = 2;
      }
    }
  else
    {
    int smooth;
    i = smooth = 0;
    while (*value)
      {
      if (i >= 256)
        goto badcolor;
      if (*value == '<')
        {
        if (i == 0 || smooth
          || (smooth = atoi(value+1)) < 2
          || (value = strchr(value,'>')) == NULL)
          {
          goto badcolor;
          }
        i += smooth;
        ++value;
        }
      else
        {
        for (j = 0; j < 3; ++j)
          {
          if ((k = *(value++)) < '0')
            goto badcolor;
          else if (k <= '9')
            k -= '0';
          else if (k < 'A')
            goto badcolor;
          else if (k <= 'Z')
            k -= ('A'-10);
          else if (k < '_' || k > 'z')
            goto badcolor;
          else
            k -= ('_'-36);

          dacbox[i][j] = k;

          if (smooth)
            {
            int start,spread,cnum;
            int tmpval;
            start = i - (spread = smooth + 1);
            cnum = 0;
            if ((k - (int)dacbox[start][j]) == 0)
              {
              while (++cnum < spread)
                dacbox[start+cnum][j] = k;
              }
            else
              {
              while (++cnum < spread)
                {
                tmpval = cnum * dacbox[i][j]
                         + (i - (start + cnum))
                         * dacbox[start][j]
                         + spread / 2;
                tmpval = tmpval / spread;
                dacbox[start+cnum][j] = tmpval;
                }
              }
            }
          }
        smooth = 0;
        ++i;
        }
      }

    if (smooth)
      goto badcolor;

    while (i < 256)
      {
      /* zap unset entries */
      dacbox[i][0] = dacbox[i][1] = dacbox[i][2] = 40;
      ++i;
      }

    for(i = 0; i < 256; i++)
      {
      dacbox[i][0] = dacbox[i][0] << 2;
      dacbox[i][1] = dacbox[i][1] << 2;
      dacbox[i][2] = dacbox[i][2] << 2;
      }

    colorstate = 1;
    }

  memcpy((char *)mapdacbox,dacbox,768);
  spindac(0,0);
  colorpreloaded = 1;

  return(0);

badcolor:

  return(-1);
}

int cmdfiles(int argc,char **argv)
{
  int     i;
  char    curarg[141];
  char    tempstr[256];
  char    tempstring[256];
  char    *sptr;
  char    *homedir;
  FILE    *initfile;

  for (i = 1; i < argc; i++)
    {     /* cycle through args */
    if ((int)strcmp(argv[i],"-private")==0)
      privatecolor = 1;
    else if ((int)strcmp(argv[i],"-share")==0)
      sharecolor = 1;
    else if ((int)strcmp(argv[i],"-fullscreen")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-disk")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-onroot")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-fast")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-simple")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-slowdisplay")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-sync")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-fixcolors")==0)
      /* ignore */;
    else if ((int)strcmp(argv[i],"-geometry")==0 && i+1<argc)
      {
      geometry = argv[i+1];
      i++;
      }
    else
      {
      strcpy(curarg,argv[i]);
      if (curarg[0] == ';')             /* start of comments? */
        break;
      if (curarg[0] != '@')
        {           /* simple command? */
        if (strchr(curarg,'=') == NULL)
          { /* not xxx=yyy, so check for gif */
          strcpy(tempstring,curarg);
          if (strchr(curarg,'.') == NULL)
            strcat(tempstring,".gif");
          if ((initfile = fopen(tempstring,"rb")))
            {
            fread(tempstring,6,1,initfile);
            if ( tempstring[0] == 'G'
              && tempstring[1] == 'I'
              && tempstring[2] == 'F'
              && tempstring[3] >= '8' && tempstring[3] <= '9'
              && tempstring[4] >= '0' && tempstring[4] <= '9')
              {
              strcpy(readname,curarg);
              curarg[0] = showfile = 0;
              }
            fclose(initfile);
            }
          }
        if (curarg[0])
          cmdarg(curarg,0);        /* process simple command */
        }
      else if ((sptr = strchr(curarg,'/')))
        { /* @filename/setname? */
        *sptr = 0;
        strcpy(CommandFile,&curarg[1]);
        strcpy(CommandName,sptr+1);
        find_file_item(CommandFile,CommandName,&initfile);
        cmdfile(initfile,3);
        }
      else
        {                /* @filename */
        if ((initfile = fopen(&curarg[1],"r")) == NULL)
          argerror(curarg);
        cmdfile(initfile,0);
        }
      }
    }

  /* try fractdir/xmfract.ini */
  if (fractdir != NULL)
    {
    sprintf(tempstr, "%s/xmfract.ini", fractdir);
    if ((initfile = fopen(tempstr, "r")) != NULL)
      cmdfile(initfile,1);        /* process it */
    }

  homedir = getenv("HOME");
  if(homedir != NULL)
    {
    sprintf((char *) &tempstr, "%s/%s", homedir, ".xmfractrc");
    if((initfile = fopen(tempstr,"r")) != NULL)
      cmdfile(initfile,1);        /* process it */

    sprintf((char *) &tempstr, "%s/%s", homedir, "xmfract.ini");
    if((initfile = fopen(tempstr,"r")) != NULL)
      cmdfile(initfile,1);        /* process it */
    }

  /* look for ./xmfract.ini - no need for findpath etc. */
  if ((initfile = fopen("xmfract.ini" ,"r")) != NULL)
    cmdfile(initfile,1);        /* process it */

  if (first_init == 0)
    {
    initmode = -1; /* don't set video when <ins> key used */
    showfile = 1;  /* nor startup image file            */
    }

  if(debugflag != 110)
    first_init = 0;
  return(0);
}

void initvars_restart()
{
  AntiAliasing = 0;
  CommandComment1[0] = 0;
  CommandComment2[0] = 0;
  CommandComment3[0] = 0;
  CommandComment4[0] = 0;
  CommandName[0] = 0;
  CoreBlue  = 128;
  CoreGreen = 128;
  CoreRed   = 128;
  FormName[0] = 0;
  IFSName[0] = 0;
  LName[0] = 0;
  LogFlag = 0;
  MathType = D_MATH;
  MaxColorRes = 8;
  NumFrames = 1;
  XMP = 250;
  Real_V = 0;
  Shadowing = 0;
  SolidCore = 1;
  TPlusFlag = 1;
  T_header_24 = 18;
  Targa_Out = 0;
  TranspSymmetry = 0;
  Transparent3D = 0;
  abort_process = 0;
  altkey = 0;
  bad_check = -3000;
  bad_value = -10000;
  bailout = 0;
  bitshift = 16;
  calc_status = IMG_PRMS_INIT;
  cdegree.x = 3.0;
  cdegree.y = 0.0;
  color_dark = 0;
  colors = 256;
  creditscreen = 1;
  croot.x = 1.0;
  croot.y = 0.0;
  ctrlkey = 0;
  curtype = MANDELFP;
  debugflag = 0;
  disclaimer = 1;
  disk16bit = 0;
  diskflag = 0;
  diskisactive = 0;
  diskvideo = 0;
  distest = 0;
  dither_flag = 0;
  doconnect = 1;
  doredraw = 0;
  dotread = readvideo;
  dotwrite = writevideo;
  dx0 = mkptr(extraseg,0);
  dy1 = (dx1 = (dy0 = dx0 + MAXPIXELS) + MAXPIXELS) + MAXPIXELS;
  escape_exit = 0;
  euler = 0;
  xmf = (void(**)(void))0;
  floatflag = '1';
  floatmax = FLT_MAX;
  floatmin = FLT_MIN;
  forcesymmetry = 999;
  forwarding = 0;
  fp = (FILE *)NULL;
  freestyle.range = 6;
  freestyle.color = -1;
  freestyle.endcolor_hi = 0;
  freestyle.endcolor_lo = 0;
  freestyle.gamma = 1000;
  freestyle.oldcolor = -1;
  freestyle.transparency = 50;
  freestyle.topmult = .5;
  freestyle.botmult = .5;
  gif87a_flag = 0;
  gotfile = 0;
  hasinverse = 0;
  helpmode = -1;
  ifs_defn = NULL;
  init_rseed = (int)time(NULL);
  initbatch = 0;
  initmode = -1;
  initsavetime = 0;
  jiiming = 0;
  juli3Dmode = 0;
  julibrot = 0;
  lineread = readvideoline;
  linewrite = writevideoline;
  loaded = 0;
  lstscreenflag = 0;
  lx0 = (long *) dx0;
  ly1 = (lx1 = (ly0 = lx0 + MAXPIXELS) + MAXPIXELS) + MAXPIXELS;
  magnitude = 0;
  mapdacbox = (unsigned char *)NULL;
  mapset = 0;
  max_colors = 256;
  undo_max = 8192;
  maxfn = 0;
  metakey = 0;
  numtrigfn = NUMTRIGFN;
  nxtscreenflag = 1;
  ok_to_cycle = 0;
  oldlast.color = 0;
  oldlast.x = 0;
  oldlast.y = 0;
  oldtype = MANDELFP;
  oldx = -1;
  oldy = -1;
  orbit_delay = 0;
  outln = out_line;
  overwrite_flag = 0;
  paledit.popped_up = 0;
  paledit.freestyle_mode = 0;
  paledit.edit_palette_dialog = NULL;
  paledit.primary_active_colorbox_gc = NULL;
  paledit.secondary_active_colorbox_gc = NULL;
  paledit.palette_colorbox_gc = NULL;
  paledit.gamma_val = 1.0;
  paledit.oldrect = -1;
  paledit.inactive_frozen = 0;
  paledit.pal_range.skip_val = 1;
  paledit.pal_range.start_index = 0;
  paledit.pal_range.stop_index = -1;
  paledit.active_color_register = 0;
  paledit.active_color_register_color = 0;
  paledit.inactive_color_register_color = 0;
  paledit.editpal_canvas_cursor.active = 0;
  periodicitycheck = 1;
  projection = 2;
  randshell_popped_up = 0;
  refresh_rate = 100;
  reset_ifs_defn();
  resized_by_user = 0;
  resize_mode = 0;
  resume_info = (char *) NULL;
  rflag = 0;
  rowsize = 0;
  rseed = init_rseed;
  save16bit=0;
  save_release = release = 1822;
  set_defaults = 0;
  shiftkey = 0;
  showfile = 1;
  soundflag = 0;
  strcpy(readname,"./");
  timerflag = 0;
  tpdepth = tptime = 0;
  ttmax = 0.0;
  ttmin = 0.0;
  twopi = PI * 2.0;
  usepixtab = 0;
  usr_floatflag = '1';
  usr_periodicitycheck = 1;
  usr_randmap_mode = 5;
  usr_randmap_func = 4;
  verbose = 1;
  viewcrop = 1;
  viewreduction = 4.2;
  viewwindow = 0;
  viewxdots = viewydots = 0;
  waste = 100;
  xlastcolor = -1;
  zoomcolor_index = 1;
  zoomoff = 1;
  zzmax = 1.5;
  zzmin = 0.0;
  free_workarea();
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: cmdfile.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.27  1995/06/15  15:36:25  darryl
 * Enhance zoom error message when attempting to zoom after
 * reading a GIF that does not contain fractal info.
 *
 * Revision 1.26  1995/06/14  01:30:07  darryl
 * Add color editor undo-last-command and undo-last-redo and
 * command-line and ini file support for max undo commands.
 *
 * Revision 1.25  1995/06/13  17:51:35  darryl
 * Add window resize mode function.
 *
 * Revision 1.24  1995/06/12  16:07:19  darryl
 * Disable color cycling when palette editor freestyle
 * mode is active. Re-enable when exiting freestyle mode.
 *
 * Revision 1.23  1995/06/08  18:30:45  darryl
 * Enhance mig creation routines.
 *
 * Revision 1.22  1995/06/08  00:01:06  darryl
 * Don't exit when making a mig.
 *
 * Revision 1.21  1995/06/05  16:30:14  tom
 * File handling enhancements. Remove DOS-specific code.
 *
 * Revision 1.20  1995/05/24  16:28:59  darryl
 * Set default color cycle range back to 0/colors-1.
 *
 * Revision 1.19  1995/05/23  18:50:55  darryl
 * Fix color editor and palette randomization problems
 * reported in beta testing.
 *
 * Revision 1.18  1995/05/21  09:54:04  darryl
 * Fix rate= option and initialize some modifier keys.
 *
 * Revision 1.17  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.16.1.1  1995/06/05  13:28:33  tom
 * changed filepath handling
 *
 * Revision 1.16  1995/05/19  08:11:11  darryl
 * Add initialization for zoom box color index.
 *
 * Revision 1.15  1995/05/08  19:44:43  darryl
 * Fix bugs reported in beta test.
 *
 * Revision 1.14  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.13  1995/04/18  15:59:59  darryl
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
 * Revision 1.12  1995/04/10  22:12:57  darryl
 * Add rate=nnn and menu support for setting the screen refresh
 * rate during image calculation. Fix a bug where the Julia
 * inverse mode was broken after an image save. Add fallback
 * resources that take effect if a resource file is not found.
 *
 * Revision 1.11  1995/03/31  17:07:59  darryl
 * Fix color loading so that the mapdacbox is loaded when
 * a colors= line is found in the parameter file.
 *
 * Revision 1.10  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.9  1995/03/28  18:04:20  tom
 * Enhance initialization file handling. Revise stackavail()
 * so that it uses getrlimit if available. Enhance configuration
 * files to check for getrlimit and associated header files.
 *
 * Revision 1.8  1995/03/27  20:21:18  darryl
 * Fix precedence of initialization file processing.
 * Fix parameter function initialization.
 *
 * Revision 1.7  1995/03/27  18:49:22  darryl
 * Add processing for a command file named .xmfractrc in
 * the user's home directory.
 *
 * Revision 1.6  1995/03/25  08:42:49  darryl
 * Fixes and enhancements -- make the parameter selection
 * understand that if a parameter file is loaded, then
 * the ifs, lsys, or formula file does not need to be
 * re-selected. Display the current fractal type and
 * any associated file and file entries at the top of
 * the parameters screen. Delete some unused code.
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
 * Revision 1.1.1.1  1995/03/14  18:07:27  tom
 * init usepixtab to 0
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.305  1995/01/30  19:54:51  darryl
 * Enhance information messages concerning shared color.
 *
 * Revision 0.304  1995/01/18  22:28:05  darryl
 * Expand error messages from the command-line parser.
 *
 * Revision 0.303  1995/01/17  19:07:18  darryl
 * Add processing for obsolete command-line or parameter
 * file commands and options to prevent spurious error
 * messages from the argument processor.
 *
 * Revision 0.302  1995/01/17  00:03:30  darryl
 * Add a disclaimer dialog.
 *
 * Revision 0.301  1994/12/30  23:54:54  darryl
 * Delete unused global variables.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.109  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.108  1994/11/29  16:41:23  darryl
 * Move references to the initialization file from "sstools.ini"
 * to "xmfract.ini" to prevent confusion between DOS and UNIX
 * versions of the programs.
 *
 * Revision 0.107  1994/11/29  06:15:07  darryl
 * More bulletproofing the zoom box, and default rotate_lo to
 * two so the lake is not cycled, and fix a bug where the
 * scale multiple on the upper and lower cycle range scales
 * was not set to one.
 *
 * Revision 0.106  1994/11/29  02:08:16  darryl
 * Reverse the operation of the zoomoff variable so
 * that a non-zero value means that zooming is disabled.
 *
 * Revision 0.105  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.104  1994/11/23  05:19:37  darryl
 * Initial implementation of help. Minor widget alignment
 * changes. Widget name changes for uniqueness.
 *
 * Revision 0.103  1994/11/21  18:21:16  darryl
 * Change instances of XtIsRealized to XtIsManaged.
 *
 * Revision 0.102  1994/11/19  01:58:15  darryl
 * Change default maxit to 127 and default.map 128'th color
 * to a dark blue so that the program performs more like
 * the original when users follow examples in the book.
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
