
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: lsys.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int ispow2(long n);
static void lsys_doplus(long n);
static void lsys_doplus_pow2(long n);
static void lsys_dominus(long n);
static void lsys_dominus_pow2(long n);
static void lsys_doslash(long n);
static void lsys_dobslash(long n);
static void lsys_doat(long n);
static void lsys_dopipe(long n);
static void lsys_dopipe_pow2(long n);
static void lsys_dobang(long n);
static void lsys_dosizedm(long n);
static void lsys_dosizegf(long n);
static void lsys_dodrawd(long n);
static void lsys_dodrawm(long n);
static void lsys_dodrawg(long n);
static void lsys_dodrawf(long n);
static void lsys_dodrawc(long n);
static void lsys_dodrawgt(long n);
static void lsys_dodrawlt(long n);
static double getnumber(char **str);
static struct lsys_cmd *findsize
  (struct lsys_cmd *command, struct lsys_cmd **rules, int depth);
static int findscale
  (struct lsys_cmd *command, struct lsys_cmd **rules, int depth);
static struct lsys_cmd *drawLSys
  (struct lsys_cmd *command, struct lsys_cmd **rules, int depth);
static int readLSystemFile(char *str);
static void free_rules_mem(void);
static int save_rule(char *rule, char **saveptr);
static struct lsys_cmd *SizeTransform(char *s);
static struct lsys_cmd *DrawTransform(char *s);
static void free_lcmds(void);

/* static variables */
static char *ruleptrs[MAXRULES];
static char curcolor;
static char dmaxangle;
static char maxangle;
static char stackoflow;
static long coss[50];
static long lsys_Xmax;
static long lsys_Xmin;
static long lsys_Ymax;
static long lsys_Ymin;
static unsigned long realangle;

static struct lsys_cmd *rules2[MAXRULES];

static int ispow2(long n)
{
  return (n == (n & -n));
}

/*ARGSUSED*/
static void lsys_doplus(long n)
{
  if (reverse)
    {
    if (++angle == maxangle)
      angle = 0;
    }
  else
    {
    if (angle)
      angle--;
    else
      angle = dmaxangle;
    }
}

/* This is the same as lsys_doplus, except maxangle is a power of 2. */
/*ARGSUSED*/
static void lsys_doplus_pow2(long n)
{
  if (reverse)
    {
    angle++;
    angle &= dmaxangle;
    }
  else
    {
    angle--;
    angle &= dmaxangle;
    }
}

/*ARGSUSED*/
static void lsys_dominus(long n)
{
  if (reverse)
    {
    if (angle)
      angle--;
    else
      angle = dmaxangle;
    }
  else
    {
    if (++angle == maxangle)
      angle = 0;
    }
}

/*ARGSUSED*/
static void lsys_dominus_pow2(long n)
{
  if (reverse)
    {
    angle--;
    angle &= dmaxangle;
    }
  else
    {
    angle++;
    angle &= dmaxangle;
    }
}

static void lsys_doslash(long n)
{
  if (reverse)
    realangle -= n;
  else
    realangle += n;
}

static void lsys_dobslash(long n)
{
  if (reverse)
    realangle += n;
  else
    realangle -= n;
}

static void lsys_doat(long n)
{
  lsize = multiply(lsize, n, 19);
}

/*ARGSUSED*/
static void lsys_dopipe(long n)
{
  angle += maxangle / 2;
  angle %= maxangle;
}

/*ARGSUSED*/
static void lsys_dopipe_pow2(long n)
{
  angle += maxangle >> 1;
  angle &= dmaxangle;
}

/*ARGSUSED*/
static void lsys_dobang(long n)
{
  reverse = ! reverse;
}

/*ARGSUSED*/
static void lsys_dosizedm(long n)
{
  double angle = (double) realangle * ANGLE2DOUBLE;
  double s, c;
  long fixedsin, fixedcos;
  FPUsincos(&angle, &s, &c);
  fixedsin = (long) (s * FIXEDLT1);
  fixedcos = (long) (c * FIXEDLT1);

  xpos += multiply(multiply(lsize, aspect, 19), fixedcos, 29);
  ypos += multiply(lsize, fixedsin, 29);

  /* xpos+=lsize*aspect*cos(realangle*PI/180);  */
  /* ypos+=lsize*sin(realangle*PI/180);         */
  if (xpos>lsys_Xmax) lsys_Xmax=xpos;
  if (ypos>lsys_Ymax) lsys_Ymax=ypos;
  if (xpos<lsys_Xmin) lsys_Xmin=xpos;
  if (ypos<lsys_Ymin) lsys_Ymin=ypos;
}

/*ARGSUSED*/
static void lsys_dosizegf(long n)
{
  xpos += multiply(lsize, (long) coss[(int)angle], 29);
  ypos += multiply(lsize, (long) sins[(int)angle], 29);
  /* xpos+=lsize*coss[angle];                   */
  /* ypos+=lsize*sins[angle];                   */
  if (xpos>lsys_Xmax) lsys_Xmax=xpos;
  if (ypos>lsys_Ymax) lsys_Ymax=ypos;
  if (xpos<lsys_Xmin) lsys_Xmin=xpos;
  if (ypos<lsys_Ymin) lsys_Ymin=ypos;
}

/*ARGSUSED*/
static void lsys_dodrawd(long n)
{
  double angle = (double) realangle * ANGLE2DOUBLE;
  double s, c;
  long fixedsin, fixedcos;
  int lastx, lasty;
  FPUsincos(&angle, &s, &c);
  fixedsin = (long) (s * FIXEDLT1);
  fixedcos = (long) (c * FIXEDLT1);

  lastx=(int) (xpos >> 19);
  lasty=(int) (ypos >> 19);
  xpos += multiply(multiply(lsize, aspect, 19), fixedcos, 29);
  ypos += multiply(lsize, fixedsin, 29);
  /* xpos+=lsize*aspect*cos(realangle*PI/180);   */
  /* ypos+=lsize*sin(realangle*PI/180);          */
  draw_line(lastx,lasty,(int)(xpos >> 19),(int)(ypos>>19),curcolor);
}

/*ARGSUSED*/
static void lsys_dodrawm(long n)
{
  double angle = (double) realangle * ANGLE2DOUBLE;
  double s, c;
  long fixedsin, fixedcos;
  FPUsincos(&angle, &s, &c);
  fixedsin = (long) (s * FIXEDLT1);
  fixedcos = (long) (c * FIXEDLT1);

  /* xpos+=lsize*aspect*cos(realangle*PI/180);   */
  /* ypos+=lsize*sin(realangle*PI/180);          */
  xpos += multiply(multiply(lsize, aspect, 19), fixedcos, 29);
  ypos += multiply(lsize, fixedsin, 29);
}

/*ARGSUSED*/
static void lsys_dodrawg(long n)
{
  xpos += multiply(lsize, (long) coss[(int)angle], 29);
  ypos += multiply(lsize, (long) sins[(int)angle], 29);
  /* xpos+=lsize*coss[(int)angle];                    */
  /* ypos+=lsize*sins[(int)angle];                    */
}

/*ARGSUSED*/
static void lsys_dodrawf(long n)
{
  int lastx = (int) (xpos >> 19);
  int lasty = (int) (ypos >> 19);
  xpos += multiply(lsize, (long) coss[(int)angle], 29);
  ypos += multiply(lsize, (long) sins[(int)angle], 29);
  /* xpos+=lsize*coss[(int)angle];                    */
  /* ypos+=lsize*sins[(int)angle];                    */
  draw_line(lastx,lasty,(int)(xpos>>19),(int)(ypos>>19),curcolor);
}

static void lsys_dodrawc(long n)
{
  curcolor = ((int) n) % colors;
}

static void lsys_dodrawgt(long n)
{
  curcolor -= n;
  if ((curcolor &= colors-1) == 0)
    curcolor = colors-1;
}

static void lsys_dodrawlt(long n)
{
  curcolor += n;
  if ((curcolor &= colors-1) == 0)
    curcolor = 1;
}

static double getnumber(char **str)
{
  char numstr[30];
  float ret;
  int i,root,inverse;

  root=0;
  inverse=0;
  strcpy(numstr,"");
  (*str)++;
  switch (**str)
    {
    case 'q':
      root=1;
      (*str)++;
    break;
    case 'i':
      inverse=1;
      (*str)++;
    break;
    }
  switch (**str)
    {
    case 'q':
      root=1;
      (*str)++;
    break;
    case 'i':
      inverse=1;
      (*str)++;
    break;
    }
  i=0;
  while (**str<='9' && (**str>='0' || **str=='.'))
    {
    numstr[i++]= **str;
    (*str)++;
    }
  (*str)--;
  numstr[i]=0;
  ret=atof(numstr);
  if (root)
    ret=sqrt(ret);
  if (inverse)
    ret = 1/ret;
  return ret;
}

static struct lsys_cmd *findsize(
  struct lsys_cmd *command,
  struct lsys_cmd **rules,
  int depth)
{
  struct lsys_cmd **rulind;
  int tran;

  if (overflow)     /* integer math routines overflowed */
    return NULL;

  if (stackavail() < 400)
    { /* leave some margin for calling subrtns */
    stackoflow = 1;
    return NULL;
    }

  while (command->ch && command->ch !=']')
    {
    if (! (counter++))
      {
      /* let user know we're not dead */
      }
    tran=0;
    if (depth)
      {
      for(rulind=rules;*rulind;rulind++)
        if ((*rulind)->ch==command->ch)
          {
          tran=1;
          if (findsize((*rulind)+1,rules,depth-1) == NULL)
            return(NULL);
          }
      }
    if (!depth || !tran)
      {
      if (command->f)
        (*command->f)(command->n);
      else if (command->ch == '[')
        {
        char saveang,saverev;
        long savesize,savex,savey;
        unsigned long saverang;

        saveang=angle;
        saverev=reverse;
        savesize=lsize;
        saverang=realangle;
        savex=xpos;
        savey=ypos;
        if ((command=findsize(command+1,rules,depth)) == NULL)
          return(NULL);
        angle=saveang;
        reverse=saverev;
        lsize=savesize;
        realangle=saverang;
        xpos=savex;
        ypos=savey;
        }
      }
    command++;
    }
  return command;
}

static int findscale
  (struct lsys_cmd *command, struct lsys_cmd **rules, int depth)
{
  float horiz,vert;
  double xmin, xmax, ymin, ymax;
  double locsize;
  double locaspect;
  int i;
  struct lsys_cmd *fsret;
  locaspect=screenaspect*xdots/ydots;
  aspect = FIXEDPT(locaspect);
  for(i=0;i<maxangle;i++)
    {
    sins[i]=(long) ((sin(2*i*PI/maxangle)) * FIXEDLT1);
    coss[i]=(long) ((locaspect * cos(2*i*PI/maxangle)) * FIXEDLT1);
    }
  xpos=ypos=lsys_Xmin=lsys_Xmax=lsys_Ymax=
    lsys_Ymin=angle=reverse=realangle=counter=0;
  lsize=FIXEDPT(1);
  fsret = findsize(command,rules,depth);
  xmin = (double) lsys_Xmin / FIXEDMUL;
  xmax = (double) lsys_Xmax / FIXEDMUL;
  ymin = (double) lsys_Ymin / FIXEDMUL;
  ymax = (double) lsys_Ymax / FIXEDMUL;
  locsize = (double) lsize / FIXEDMUL;
  if (fsret == NULL)
    return 0;
  if (xmax == xmin)
    horiz = 1E37;
  else
    horiz = (xdots-10)/(xmax-xmin);
  if (ymax == ymin)
    vert = 1E37;
  else
    vert = (ydots-6) /(ymax-ymin);
  locsize = (vert<horiz) ? vert : horiz;

  if (horiz == 1E37)
    xpos = FIXEDPT(xdots/2);
  else
    xpos = FIXEDPT(-xmin*(locsize)+5+((xdots-10)-(locsize)*(xmax-xmin))/2);
  if (vert == 1E37)
    ypos = FIXEDPT(ydots/2);
  else
    ypos = FIXEDPT(-ymin*(locsize)+3+((ydots-6)-(locsize)*(ymax-ymin))/2);
  lsize = FIXEDPT(locsize);
  return 1;
}

static struct lsys_cmd *drawLSys(
  struct lsys_cmd *command,
  struct lsys_cmd **rules,
  int depth)
{
  struct lsys_cmd **rulind;
  int tran;

  if (overflow)     /* integer math routines overflowed */
    return NULL;

  if (stackavail() < 400)
    { /* leave some margin for calling subrtns */
    stackoflow = 1;
    return NULL;
    }

  while (command->ch && command->ch !=']')
    {
    if (!(counter++))
      {
      if (check_key() != 0)
        {
        counter--;
        return NULL;
        }
      }
    tran=0;
    if (depth)
      {
      for(rulind=rules;*rulind;rulind++)
        if ((*rulind)->ch == command->ch)
          {
          tran=1;
          if (drawLSys((*rulind)+1,rules,depth-1) == NULL)
            return NULL;
          }
      }
    if (!depth||!tran)
      {
      if (command->f)
        (*command->f)(command->n);
      else if (command->ch == '[')
        {
        char saveang,saverev,savecolor;
        long savesize,savex,savey;
        unsigned long saverang;

        saveang=angle;
        saverev=reverse;
        savesize=lsize;
        saverang=realangle;
        savex=xpos;
        savey=ypos;
        savecolor=curcolor;
        if ((command=drawLSys(command+1,rules,depth)) == NULL)
          return(NULL);
        angle=saveang;
        reverse=saverev;
        lsize=savesize;
        realangle=saverang;
        xpos=savex;
        ypos=savey;
        curcolor=savecolor;
        }
      }
    command++;
    }
  return command;
}

static int readLSystemFile(char *str)
{
  int c;
  int ret;
  char **rulind;
  int err=0;
  int linenum,check=0;
  char my_inline[161],fixed[161],*word;
  FILE *infile;
  char msgbuf[481]; /* enough for 6 full lines */
  char tempstr[128];

  if((int)(strlen((char *) &LFileName) == 0))
    {
    sprintf(tempstr, "No L-System file specified in the saved image.\nSelect the L-System definition file containing the \"%s\" definition,\nthen re-load this saved image.", (char *) &LName);
    ret = confirm(main_window, (char *) &tempstr,
      OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
    if(ret != 1)
      return(-1);
    else
      {
      select_lsystem_file(main_window, "0");
      return(-1);
      }
    }

  if (find_file_item(LFileName,str,&infile) < 0)
    return -1;
  while ((c = fgetc(infile)) != '{')
    if (c == EOF) return -1;

  maxangle=0;
  for(linenum=0;linenum<MAXRULES;++linenum) ruleptrs[linenum]=NULL;
  rulind= &ruleptrs[1];
  msgbuf[0]=linenum=0;

  while(fgets(my_inline,160,infile))  /* Max line length 160 chars */
    {
    linenum++;
    if ((word = strchr(my_inline,';'))) /* strip comment */
      *word = 0;
    strlwr(my_inline);

    if ((int)strspn(my_inline," \t\n") < (int)strlen(my_inline))
      /* not a blank line */
      {
      word=strtok(my_inline," =\t\n");
      if (!(int)strcmp(word,"axiom"))
        {
        save_rule(strtok(NULL," \t\n"),&ruleptrs[0]);
        check=1;
        }
      else if (!(int)strcmp(word,"angle"))
        {
        maxangle=atoi(strtok(NULL," \t\n"));
        dmaxangle = maxangle - 1;
        check=1;
        }
    else if (!(int)strcmp(word,"}"))
       break;
    else if (strlen(word)==1)
      {
      char *tok;
      tok = strtok(NULL, " \t\n");
      strcpy(fixed, word);
      if (tok != NULL)
        {     /* Some strcat's die if they cat with NULL */
        strcat(fixed, tok);
        }
      save_rule(fixed, rulind++);
      check=1;
      }
    else
      if (err<6)
        {
        sprintf(&msgbuf[strlen(msgbuf)],
          "Syntax error line %d: %s\n",linenum,word);
        ++err;
        }
      if (check)
        {
        check=0;
        if((word=strtok(NULL," \t\n")))
          if (err<6)
          {
          sprintf(&msgbuf[strlen(msgbuf)],
            "Extra text after command line %d: %s\n",linenum,word);
          ++err;
          }
        }
      }
    }
  fclose(infile);
  if (!ruleptrs[0] && err<6)
    {
    strcat(msgbuf,"Error:  no axiom\n");
    ++err;
    }
  if ((maxangle<3||maxangle>50) && err<6)
    {
    strcat(msgbuf,"Error:  illegal or missing angle\n");
    ++err;
    }
  if (err)
    {
    msgbuf[strlen(msgbuf)-1]=0; /* strip trailing \n */
    caution(main_window, (char *)&msgbuf, NULL);
    return -1;
    }
  *rulind=NULL;
  return 0;
}

int Lsystem()
{
  int order;
  char **rulesc;
  struct lsys_cmd **sc;

  if ( (!loaded) && LLoad())
    return -1;

  overflow = 0;        /* reset integer math overflow flag */

  order=param[0];
  if (order<=0)
    order=0;
  stackoflow = 0;

  sc = rules2;
  for (rulesc = ruleptrs; *rulesc; rulesc++)
    *sc++ = SizeTransform(*rulesc);
  *sc = NULL;

  if (findscale(rules2[0], &rules2[1], order))
    {
    realangle = angle = reverse = 0;

    free_lcmds();
    sc = rules2;
    for (rulesc = ruleptrs; *rulesc; rulesc++)
      *sc++ = DrawTransform(*rulesc);
    *sc = NULL;

    /* !! HOW ABOUT A BETTER WAY OF PICKING THE DEFAULT DRAWING COLOR */
    if ((curcolor=15) > colors)
      curcolor=colors-1;
    drawLSys(rules2[0], &rules2[1], order);
    }
  if (stackoflow)
    {
    static char msg[]={"insufficient memory, try a lower order"};
    warning(main_window, (char *)&msg, NULL);
    }
  if (overflow)
    {
    static char msg[]={"Internal math routines failed, try a lower order"};
    caution(main_window, (char *)&msg, NULL);
    }
  free_rules_mem();
  free_lcmds();
  loaded=0;
  return 0;
}

int LLoad()
{
  char i;
  if (readLSystemFile(LName))
    { /* error occurred */
    free_rules_mem();
    loaded=0;
    return -1;
    }
  for(i=0;i<maxangle;i++)
    {
    sins[(int)i]=(long) ((sin(2*i*PI/maxangle)) * FIXEDLT1);
    coss[(int)i]=(long) (((double) aspect /
                   (double) FIXEDMUL * cos(2*i*PI/maxangle)) * FIXEDLT1);
    }
  loaded=1;
  return 0;
}

static void free_rules_mem()
{
  int i;
  for(i=0;i<MAXRULES;++i)
    if(ruleptrs[i]) XtFree(ruleptrs[i]);
}

static int save_rule(char *rule,char **saveptr)
{
  int i;
  char *tmpfar;
  i=strlen(rule)+1;
  if((tmpfar=XtMalloc((long)i))==NULL)
    {
    stackoflow = 1;
    return -1;
    }
  *saveptr=tmpfar;
  while(--i>=0)
    *(tmpfar++)= *(rule++);
  return 0;
}

static struct lsys_cmd *SizeTransform(char *s)
{
  struct lsys_cmd *ret;
  struct lsys_cmd *doub;
  int maxval = 10;
  int n = 0;
  void (*f)(long);
  long num;

  void (*plus)(long) = (ispow2(maxangle)) ? lsys_doplus_pow2 : lsys_doplus;
  void (*minus)(long) = (ispow2(maxangle)) ? lsys_dominus_pow2 : lsys_dominus;
  void (*pipe)(long) = (ispow2(maxangle)) ? lsys_dopipe_pow2 : lsys_dopipe;

  void (*slash)(long) =  lsys_doslash;
  void (*bslash)(long) = lsys_dobslash;
  void (*at)(long) =     lsys_doat;
  void (*dogf)(long) =   lsys_dosizegf;

  ret = (struct lsys_cmd *) XtMalloc((long) maxval * sizeof(struct lsys_cmd));
  if (ret == NULL)
    {
    stackoflow = 1;
    return NULL;
    }
  while (*s)
    {
    f = NULL;
    num = 0;
    ret[n].ch = *s;
    switch (*s)
      {
      case '+':
        f = plus;
      break;

      case '-':
        f = minus;
      break;

      case '/':
        f = slash;
        num = (long) (getnumber(&s) * 11930465L);
      break;

      case '\\':
        f = bslash;
        num = (long) (getnumber(&s) * 11930465L);
      break;

      case '@':
        f = at;
        num = FIXEDPT(getnumber(&s));
      break;

      case '|':
        f = pipe;
      break;

      case '!':
        f = lsys_dobang;
      break;

      case 'd':
      case 'm':
        f = lsys_dosizedm;
      break;

      case 'g':
      case 'f':
        f = dogf;
      break;

      case '[':
        num = 1;
      break;

      case ']':
        num = 2;
      break;

      default:
        num = 3;
      break;
      }
    ret[n].f = f;
    ret[n].n = num;
    if (++n == maxval)
      {
      doub = (struct lsys_cmd *) XtMalloc
        ((long) maxval*2*sizeof(struct lsys_cmd));
      if (doub == NULL)
        {
        XtFree((char *) ret);
        stackoflow = 1;
        return NULL;
        }
      memcpy(doub, ret, maxval*sizeof(struct lsys_cmd));
      XtFree((char *) ret);
      ret = doub;
      maxval <<= 1;
      }
    s++;
    }
  ret[n].ch = 0;
  ret[n].f = NULL;
  ret[n].n = 0;
  n++;

  doub = (struct lsys_cmd *) XtMalloc((long) n*sizeof(struct lsys_cmd));
  if (doub == NULL)
    {
    XtFree((char *) ret);
    stackoflow = 1;
    return NULL;
    }
  memcpy(doub, ret, n*sizeof(struct lsys_cmd));
  XtFree((char *) ret);
  return doub;
}

static struct lsys_cmd *DrawTransform(char *s)
{
  struct lsys_cmd *ret;
  struct lsys_cmd *doub;
  int maxval = 10;
  int n = 0;
  void (*f)(long);
  long num;

  void (*plus)(long) = (ispow2(maxangle)) ? lsys_doplus_pow2 : lsys_doplus;
  void (*minus)(long) = (ispow2(maxangle)) ? lsys_dominus_pow2 : lsys_dominus;
  void (*pipe)(long) = (ispow2(maxangle)) ? lsys_dopipe_pow2 : lsys_dopipe;

  void (*slash)(long) =  lsys_doslash;
  void (*bslash)(long) = lsys_dobslash;
  void (*at)(long) =     lsys_doat;
  void (*drawg)(long) =  lsys_dodrawg;

  ret = (struct lsys_cmd *) XtMalloc((long) maxval * sizeof(struct lsys_cmd));

  if (ret == NULL)
    {
    stackoflow = 1;
    return NULL;
    }

  while (*s)
    {
    f = NULL;
    num = 0;
    ret[n].ch = *s;
    switch (*s)
      {
      case '+':
        f = plus;
      break;

      case '-':
        f = minus;
      break;

      case '/':
        f = slash;
        num = (long) (getnumber(&s) * 11930465L);
      break;

      case '\\':
        f = bslash;
        num = (long) (getnumber(&s) * 11930465L);
      break;

      case '@':
        f = at;
        num = FIXEDPT(getnumber(&s));
      break;

      case '|':
        f = pipe;
      break;

      case '!':
        f = lsys_dobang;
      break;

      case 'd':
        f = lsys_dodrawd;
      break;

      case 'm':
        f = lsys_dodrawm;
      break;

      case 'g':
        f = drawg;
      break;

      case 'f':
        f = lsys_dodrawf;
      break;

      case 'c':
        f = lsys_dodrawc;
        num = getnumber(&s);
      break;

      case '<':
        f = lsys_dodrawlt;
        num = getnumber(&s);
      break;

      case '>':
        f = lsys_dodrawgt;
        num = getnumber(&s);
      break;

      case '[':
        num = 1;
      break;

      case ']':
        num = 2;
      break;

      default:
        num = 3;
      break;
      }
    ret[n].f = f;
    ret[n].n = num;
    if (++n == maxval)
      {
      doub = (struct lsys_cmd *)
        XtMalloc((long) maxval*2*sizeof(struct lsys_cmd));
      if (doub == NULL)
        {
        XtFree((char *) ret);
        stackoflow = 1;
        return NULL;
        }
      memcpy(doub, ret, maxval*sizeof(struct lsys_cmd));
      XtFree((char *) ret);
      ret = doub;
      maxval <<= 1;
      }
    s++;
    }
  ret[n].ch = 0;
  ret[n].f = NULL;
  ret[n].n = 0;
  n++;

  doub = (struct lsys_cmd *)
    XtMalloc((long) n*sizeof(struct lsys_cmd));
  if (doub == NULL)
    {
    XtFree((char *) ret);
    stackoflow = 1;
    return NULL;
    }
  memcpy(doub, ret, n*sizeof(struct lsys_cmd));
  XtFree((char *) ret);
  return doub;
}

static void free_lcmds()
{
  struct lsys_cmd **sc = rules2;

  while (*sc)
    XtFree((char *) *sc++);
}

/*
 * Some notes to Adrian from PB, made when I integrated with v15:
 *   printfs changed to work with new user interface
 *   bug at end of readLSystemFile, the line which said rulind=0 fixed
 *     to say *rulind=0
 *   the calloc was not worthwhile, it was just for a 54 byte area, cheaper
 *     to keep it as a static;    but there was a static 201 char buffer I
 *     changed to not be static
 *   use of strdup was a nono, caused problems running out of space cause
 *     the memory allocated each time was never freed; I've changed to
 *     free when done
 */

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: lsys.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.7  1995/06/02  06:09:35  darryl
 * Change default button in some dialogs from Cancel to Ok.
 *
 * Revision 1.6  1995/05/15  19:30:32  darryl
 * Change warning message referring to integer math.
 *
 * Revision 1.5  1995/04/18  15:59:59  darryl
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
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.1  1995/03/11  03:16:15  tom
 * change inline to my_inline
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
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
