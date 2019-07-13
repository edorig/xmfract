
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: formula.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static char *FindFormula(char *Str);
static void parser_allocate(void);
static int BadFormula(void);
static unsigned long NewRandNum(void);
static int ParseStr(char *Str);
static void (*isfunct(char *Str, int Len))(void);
static int whichfn(char *s, int len);
static unsigned SkipWhiteSpace(char *Str);
static struct ConstArg *isconst(char *Str, int Len);
static void RandomSeed(void);
static void RecSortPrec(void);

/* static variables */
static char *Constants[] = {
  "pixel",        /* xmv[0] */
  "p1",           /* xmv[1] */
  "p2",           /* xmv[2] */
  "z",            /* xmv[3] */
  "LastSqr",      /* xmv[4] */
  "xy",           /* xmv[5] */
  "zt",           /* xmv[6] */
  "rand",         /* xmv[7] */
};

static char *ErrStrings[] =
{
  "Should be an Argument",
  "Should be an Operator",
  "')' needs a matching '('",
  "Need more ')'",
  "Undefined Operator",
  "Undefined Function",
  "More than one ','",
  "Table overflow"
};

struct SYMETRY SymStr[] =
{
  {"NOSYM",         0},
  {"XAXIS_NOPARM", -1},
  {"XAXIS",         1},
  {"YAXIS_NOPARM", -2},
  {"YAXIS",         2},
  {"XYAXIS_NOPARM",-3},
  {"XYAXIS",        3},
  {"ORIGIN_NOPARM",-4},
  {"ORIGIN",        4},
  {"PI_SYM_NOPARM",-5},
  {"PI_SYM",        5},
  {"NOPLOT",       99},
  {"",              0}
};

void (*StkSin)(void)    = dStkSin;
void (*StkSinh)(void)   = dStkSinh;
void (*StkSqr)(void)    = dStkSqr;
void (*StkCosh)(void)   = dStkCosh;
void (*StkCosXX)(void)  = dStkCosXX;
void (*StkExp)(void)    = dStkExp;
void (*StkLog)(void)    = dStkLog;
void (*StkCos)(void)    = dStkCos;
void (*StkTan)(void)    = dStkTan;
void (*StkTanh)(void)   = dStkTanh;
void (*StkCoTan)(void)  = dStkCoTan;
void (*StkCoTanh)(void) = dStkCoTanh;
void (*StkFlip)(void)   = dStkFlip;
void (*StkAdd)(void)    = dStkAdd;
void (*StkSub)(void)    = dStkSub;
void (*StkNeg)(void)    = dStkNeg;
void (*StkMul)(void)    = dStkMul;
void (*StkLT)(void)     = dStkLT;
void (*StkLTE)(void)    = dStkLTE;
void (*StkMod)(void)    = dStkMod;
void (*StkPwr)(void)    = dStkPwr;
void (*StkDiv)(void)    = dStkDiv;
void (*StkAbs)(void)    = dStkAbs;
void (*StkReal)(void)   = dStkReal;
void (*StkImag)(void)   = dStkImag;
void (*StkConj)(void)   = dStkConj;
void (*StkTrig0)(void)  = dStkSin;
void (*StkTrig1)(void)  = dStkSqr;
void (*StkTrig2)(void)  = dStkSinh;
void (*StkTrig3)(void)  = dStkCosh;
void (*StkGT)(void)     = dStkGT;
void (*StkGTE)(void)    = dStkGTE;
void (*StkEQ)(void)     = dStkEQ;
void (*StkNE)(void)     = dStkNE;
void (*StkAND)(void)    = dStkAND;
void (*StkOR)(void)     = dStkOR;
void (*StkSRand)(void)  = dStkSRand;

struct FNCT_LIST FnctList[] =
{
  {"sin",  &StkSin},
  {"sinh", &StkSinh},
  {"cos",  &StkCos},
  {"cosh", &StkCosh},
  {"sqr",  &StkSqr},
  {"log",  &StkLog},
  {"exp",  &StkExp},
  {"abs",  &StkAbs},
  {"conj", &StkConj},
  {"real", &StkReal},
  {"imag", &StkImag},
  {"fn1",  &StkTrig0},
  {"fn2",  &StkTrig1},
  {"fn3",  &StkTrig2},
  {"fn4",  &StkTrig3},
  {"flip", &StkFlip},
  {"tan",  &StkTan},
  {"tanh", &StkTanh},
  {"cotan",  &StkCoTan},
  {"cotanh", &StkCoTanh},
  {"cosxx", &StkCosXX},
  {"srand", &StkSRand}
};

static char *FormStr;
static int ExpectingArg;
static int InitLodPtr;
static int InitOpPtr;
static int InitStoPtr;
static int Randomized;
static int SetRandom;
static int SyntaxErr;
static unsigned ErrPtr;
static unsigned InitN;
static unsigned LastOp;
static unsigned NextOp;
static unsigned long RandNum;

static char *FindFormula(char *Str)
{
  char *FormulaStr = (char *)0;
  char StrBuff[201];      /* PB, to match a safety fix in parser */
  /* MCP, changed to an automatic variable */
  char fullfilename[100]; /* BDT Full file name */
  char tempstr[256];
  unsigned Done;
  int c;
  int ret;
  unsigned n;
  FILE *File;

  if((int)(strlen((char *) &FormFileName) == 0))
    {
    sprintf(tempstr, "No formula file specified in the saved image.\nSelect the formula file containing the \"%s\" formula,\nthen re-load this saved image.",
(char *) &FormName);
    ret = confirm(main_window, (char *) &tempstr,
      OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
    if(ret != 1)
      return((char *)NULL);
    else
      {
      select_formula_file(main_window, "0");
      return((char *)NULL);
      }
    }

  findpath(FormFileName, fullfilename);  /* BDT get full path name */

  symmetry = 0;
  if((File = fopen(fullfilename, "rt")) != NULL)
    { /* BDT use variable files */
    while(StrBuff[0]=0, fscanf(File, "%200[^ \n\t({]", StrBuff) != EOF)
      {
      if(!stricmp(StrBuff, Str) || !Str[0])
        {
        while((c = getc(File)) != EOF)
          {
          if(c == '(')
            {
            StrBuff[0]=0; /* TIW 04-22-91 */
            fscanf(File, "%200[^)]", StrBuff);
            for(n = 0; SymStr[n].s[0]; n++)
              {
              if(!stricmp(SymStr[n].s, StrBuff))
                {
                symmetry = SymStr[n].n;
                break;
                }
              }
            if(!SymStr[n].s[0])
              {
              sprintf(fullfilename,"Undefined symmetry:\n  %.76s", StrBuff);
              caution(main_window, (char *) &fullfilename, NULL);
              FormulaStr = (char *)0;  /* PB 910511 */
            fclose(File);
            return(FormulaStr);
            }
          }
        else if(c == '{')
          break;
        }

      /* MCP 4-9-91, Strip the comments inside the formula.  Might
         as well allow unlimited formula lengths while
         we're at it.
       */

      FormulaStr = boxx;
      n = Done = 0;
      while(!Done)
        {
        switch(c = getc(File))
          {
          static char message[]={"Unexpected EOF:  missing a '}'"};
          case EOF:
UnexpectedEOF:
            caution(main_window, (char *) &message, NULL);
            FormulaStr = (char *)0;
            fclose(File);
            return(FormulaStr);
          case '}':
            FormulaStr[n++] = 0;
            Done = 1;
            break;
          case ';':
            while((c = getc(File)) != '\n') {
              if(c == EOF)
                goto UnexpectedEOF;
              }
            FormulaStr[n++] = ',';
                 break;
              case ' ':                     /* Also strip out the
                                                  white spaces */

              case '\t':
                 break;
              case '\n':
                 FormulaStr[n++] = ',';
                 break;
              default:
                 FormulaStr[n++] = c;
              }
              if (n >= 8192) { /* PB 4-9-91, added safety test */
                 static char message[]={"Definition too large, missing a '}'?"};
                 caution(main_window, (char *) &message, NULL);
                 FormulaStr = (char *)0;
                 fclose(File);
                 return(FormulaStr);
              }
           }
           fclose(File);
           return(FormulaStr);
        }

        StrBuff[0]=0;  /* TIW 04-22-91 */
        fscanf(File, "%200[ \n\t({]", StrBuff);
        if(StrBuff[strcspn(StrBuff, "({")]) {
skipcomments:
           fscanf(File, "%200[^}]", StrBuff);
           if (getc(File)!= '}') goto skipcomments;
        }
     }
     sprintf(fullfilename, "Formula \"%s\" not found", Str);
     caution(main_window, (char *) &fullfilename, NULL);
     FormulaStr = (char *)0;       /* PB 910511 */
     fclose(File);
     return(FormulaStr);
  }
  sprintf(fullfilename, "Unable to open %s", FormFileName);
  caution(main_window, (char *) &fullfilename, NULL);
  return((char *)0);            /* PB 910511 */
}

int RunForm(char *Name)
{
  /*  returns 1 if an error occurred  */
  /*  CAE changed fn 12 July 1993 to fix problem when formula not found  */

  /*  first set the pointers so they point to a fn which always returns 1  */
  curfractalspecific->per_pixel = BadFormula;
  curfractalspecific->orbitcalc = BadFormula;

  if (FormName[0] == 0 )
    {
    return 1;  /*  and don't reset the pointers  */
    }

  parser_allocate();  /*  ParseStr() will test if this alloc worked  */

  if((FormStr = FindFormula(Name)) != NULL )
    {
    /*  formula was found  */
    if (ParseStr(FormStr))
      {
      /*  parse failed, don't change fn pointers  */
      return 1;
      }
    else
      {
      /*  parse succeeded so set the pointers back to good functions  */
      curfractalspecific->per_pixel = form_per_pixel;
      curfractalspecific->orbitcalc = Formula;
      return 0;
      }
    }
  else
    {
    /*  formula not found, leave pointers set to BadFormula  */
    return 1;                    /* PB, msg moved to FindFormula */
    }
}

static void parser_allocate(void)
{
  /* CAE fp changed below for v18 */
  /* Note that we will waste about 6k here for pfls */
  /* Somewhat more memory is now allocated than in v17 here */
  /* however Store and Load were reduced in size to help make up for it */

  unsigned int f_size,Store_size,Load_size,v_size, p_size;

  free_workarea();

  f_size = sizeof(void(**)(void)) * MAX_OPS;

  Store_size = sizeof(union FracArg *) * MAX_STORES;

  Load_size = sizeof(union FracArg *) * MAX_LOADS;

  v_size = sizeof(struct ConstArg) * MAX_ARGS;

  p_size = sizeof(struct fls *) * MAX_OPS;

  typespecific_workarea =
    XtMalloc((long)(f_size+Load_size+Store_size+v_size+p_size));

  xmf = (void(**)(void))typespecific_workarea;

  Store = (union FracArg **)(xmf + MAX_OPS);

  Load = (union FracArg **)(Store + MAX_STORES);

  xmv = (struct ConstArg *)(Load + MAX_LOADS);

  pfls = (struct fls *)(xmv + MAX_ARGS);
}

static int BadFormula()
{
  /*
   * this is called when a formula is bad, instead of calling
   * the normal functions which will produce undefined results
   */

  return 1;
}

int fpFormulaSetup(void)
{
  int ret;

  MathType = D_MATH;
  ret = RunForm(FormName);
  if(ret)
    {
    abort_process = 1;
    }
  return(ret);
}

int form_per_pixel(void)
{
  if (FormName[0] == 0)
    {
    return(1);
    }

  overflow = LodPtr = StoPtr = OpPtr = 0;

  Arg1 = &xms[0];
  Arg2 = Arg1;
  Arg2--;

  if(Transparent3D)
    {
    TranspPerPixel(MathType, &xmv[5].a, &xmv[6].a);
    xmv[0].a = xmv[5].a;
    }
  else
    {
    switch(MathType)
      {
      case D_MATH:
      case L_MATH:
      case M_MATH:
        xmv[5].a.d.x = (xmv[0].a.d.x = dx0[col]+dShiftx);
        xmv[5].a.d.x = (xmv[0].a.d.y = dy0[row]+dShifty);
      break;
      }
    }

  if(LastInitOp)
    {
    LastInitOp = LastOp;
    }

  while(OpPtr < LastInitOp)
    {
    xmf[OpPtr++]();
    }

  InitLodPtr = LodPtr;
  InitStoPtr = StoPtr;
  InitOpPtr = OpPtr;

  if(overflow)
    {
    return(0);
    }
  else
    {
    return(1);
    }
}

int Formula(void)
{
  if(FormName[0] == 0 || overflow)
    {
    return(1);
    }

  LodPtr = InitLodPtr;
  StoPtr = InitStoPtr;
  OpPtr = InitOpPtr;

  /* Set the random number, MCP 11-21-91 */
  if(SetRandom || Randomized)
    {
    switch(MathType)
      {
      case D_MATH:
      case L_MATH:
      case M_MATH:
        dRandom();
      break;
      }
    }

  Arg1 = &xms[0];
  Arg2 = Arg1-1;

  while(OpPtr < LastOp)
    {
    xmf[OpPtr++]();
    }

  switch(MathType)
    {
    case D_MATH:
    case L_MATH:
    case M_MATH:
      old = new = xmv[3].a.d;
      return(Arg1->d.x == 0.0);
    /*NOTREACHED*/
    break;
    }
  return(1);
}

void dRandom(void)
{
  long x, y;

  /*
   * Use the same algorithm as for fixed math so that
   * they will generate the same fractals when the
   * srand() function is used.
   */

  x = NewRandNum() >> (32 - bitshift);
  y = NewRandNum() >> (32 - bitshift);
  xmv[7].a.d.x = ((double)x / (1L << bitshift));
  xmv[7].a.d.y = ((double)y / (1L << bitshift));
}

static unsigned long NewRandNum(void)
{
  return(RandNum = ((RandNum << 15) + rand15()) ^ RandNum);
}

static int ParseStr(char *Str)
{
  struct ConstArg *c;
  int ModFlag = 999, Len, Equals = 0, Mod[20], mdstk = 0;
  int NewStatement;
  struct ERROR { int n, s; } *e;
  unsigned n;
  char msg[128];

  SetRandom = Randomized = 0;

  e = (struct ERROR *)XtMalloc(sizeof(struct ERROR) * 100L);

  /* PB 910417 changed "o" to be a temporary alloc, during ParseStr only */
  xmo = (struct PEND_OP *)XtMalloc(sizeof(struct PEND_OP) * (long)MAX_OPS);

  if(!e || !xmo || !typespecific_workarea)
   {
   sprintf((char *) &msg,
     "Insufficient memory to run fractal type 'formula'");
   warning(main_window, (char *) &msg, NULL);
   return(1);
   }

  switch(MathType)
   {
   case D_MATH:
   case L_MATH:
   case M_MATH:
     StkAdd = dStkAdd;
     StkSub = dStkSub;
     StkNeg = dStkNeg;
     StkMul = dStkMul;
     StkSin = dStkSin;
     StkSinh = dStkSinh;
     StkLT = dStkLT;
     StkLTE = dStkLTE;
     StkMod = dStkMod;
     StkSqr = dStkSqr;
     StkCos = dStkCos;
     StkCosh = dStkCosh;
     StkLog = dStkLog;
     StkExp = dStkExp;
     StkPwr = dStkPwr;
     StkDiv = dStkDiv;
     StkAbs = dStkAbs;
     StkReal = dStkReal;
     StkImag = dStkImag;
     StkConj = dStkConj;
     StkTrig0 = dtrig0;
     StkTrig1 = dtrig1;
     StkTrig2 = dtrig2;
     StkTrig3 = dtrig3;
     StkFlip = dStkFlip;
     StkTan = dStkTan;
     StkTanh = dStkTanh;
     StkCoTan = dStkCoTan;
     StkCoTanh = dStkCoTanh;
     StkCosXX = dStkCosXX;
     StkGT  = dStkGT;
     StkGTE = dStkGTE;
     StkEQ  = dStkEQ;
     StkNE  = dStkNE;
     StkAND = dStkAND;
     StkOR  = dStkOR;
     StkSRand = dStkSRand;
   break;
   }

  maxfn = 0;   /* TIW 03-30-91 */
  for(vsp = 0; vsp < sizeof(Constants) / sizeof(char*); vsp++)
    {
    xmv[vsp].s = Constants[vsp];
    xmv[vsp].len = strlen(Constants[vsp]);
    }

  xmv[6].a.d.x = xmv[6].a.d.y = 0.0;
  xmv[7].a = xmv[6].a;

  switch(MathType)
    {
    case D_MATH:
    case L_MATH:
    case M_MATH:
      xmv[1].a.d.x = param[0];
      xmv[1].a.d.y = param[1];
      xmv[2].a.d.x = param[2];
      xmv[2].a.d.y = param[3];
    break;
    }

  LastInitOp = ErrPtr = paren = OpPtr = LodPtr = StoPtr = posp = 0;
  NewStatement = 1;
  SyntaxErr = -1;
  ExpectingArg = 1;

  for(n = 0; Str[n]; n++)
    {
    if(!Str[n])
      {
      break;
      }
    InitN = n;
    switch(Str[n])
      {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
      break;

      case '(':
        paren++;
        if(!ExpectingArg)
          {
          SyntaxErr = 1;
          }
      break;

      case ')':
        if(paren)
          {
          paren--;
          }
        else
          {
          SyntaxErr = 2;
          }
        if(ExpectingArg)
          {
          e[ErrPtr].n = InitN;
          e[ErrPtr++].s = 0;
          }
      break;

      case '|':
        if(Str[n+1] == '|')
          {
          if(ExpectingArg)
            {
            SyntaxErr = 0;
            }
          ExpectingArg = 1;
          n++;
          xmo[posp].f = StkOR;
          xmo[posp++].p = 7 - (paren + Equals)*15;
          }
        else if(ModFlag == paren-1)
          {
          if(ExpectingArg)
            {
            SyntaxErr = 0;
            }
          paren--;
          ModFlag = Mod[--mdstk];
          }
        else
          {
          if(!ExpectingArg)
            {
            SyntaxErr = 1;
            }
          Mod[mdstk++] = ModFlag;
          xmo[posp].f = StkMod;
          xmo[posp++].p = 2 - (paren + Equals)*15;
          ModFlag = paren++;
          }
      break;

      case ',':
      case ';':
        if(paren)
          {
          e[ErrPtr].n = InitN;
          e[ErrPtr++].s = 3;
          }
        if(!ExpectingArg)
          {
          NewStatement = 1;
          ExpectingArg = 1;
          xmo[posp].f = (void(*)(void))0;
          xmo[posp++].p = 15;
          xmo[posp].f = StkClr;
          xmo[posp++].p = -30000;
          Equals = paren = 0;
          }
        else if(!NewStatement)
          {
          SyntaxErr = 0;
          }
      break;

      case ':':
        if(paren)
          {
          e[ErrPtr].n = InitN;
          e[ErrPtr++].s = 3;
          }
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        else
          {
          ExpectingArg = 1;
          }
        xmo[posp].f = (void(*)(void))0;
        xmo[posp++].p = 15;
        xmo[posp].f = EndInit;
        xmo[posp++].p = -30000;
        Equals = paren = 0;
        LastInitOp = 10000;
        NewStatement = 1;
      break;

      case '+':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        xmo[posp].f = StkAdd;
        xmo[posp++].p = 4 - (paren + Equals)*15;
      break;

      case '-':
        if(ExpectingArg)
          {
          xmo[posp].f = StkNeg;
          xmo[posp++].p = 2 - (paren + Equals)*15;
          }
        else
          {
          xmo[posp].f = StkSub;
          xmo[posp++].p = 4 - (paren + Equals)*15;
          ExpectingArg = 1;
          }
      break;

      case '&':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        if(Str[n+1] == '&')
          {
          n++;
          xmo[posp].f = StkAND;
          xmo[posp++].p = 7 - (paren + Equals)*15;
          }
        else
          {
          SyntaxErr = 4;
          }
      break;

      case '!':
        if(Str[n+1] == '=')
          {
          if(ExpectingArg)
            {
            SyntaxErr = 0;
            }
          ExpectingArg = 1;
          n++;
          xmo[posp].f = StkNE;
          xmo[posp++].p = 6 - (paren + Equals)*15;
          }
        else
          {
          SyntaxErr = 4;
          }
      break;

      case '<':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        if(Str[n+1] == '=')
          {
          n++;
          xmo[posp].f = StkLTE;
          }
        else
          {
          xmo[posp].f = StkLT;
          }
        xmo[posp++].p = 6 - (paren + Equals)*15;
      break;

      case '>':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        if(Str[n+1] == '=')
          {
          n++;
          xmo[posp].f = StkGTE;
          }
        else
          {
          xmo[posp].f = StkGT;
          }
        xmo[posp++].p = 6 - (paren + Equals)*15;
      break;

      case '*':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        xmo[posp].f = StkMul;
        xmo[posp++].p = 3 - (paren + Equals)*15;
      break;
      case '/':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        xmo[posp].f = StkDiv;
        xmo[posp++].p = 3 - (paren + Equals)*15;
      break;

      case '^':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        xmo[posp].f = StkPwr;
        xmo[posp++].p = 2 - (paren + Equals)*15;
      break;

      case '=':
        if(ExpectingArg)
          {
          SyntaxErr = 0;
          }
        ExpectingArg = 1;
        if(Str[n+1] == '=')
          {
          n++;
          xmo[posp].f = StkEQ;
          xmo[posp++].p = 6 - (paren + Equals)*15;
          }
        else
          {
          xmo[posp-1].f = StkSto;
          xmo[posp-1].p = 5 - (paren + Equals)*15;
          Store[StoPtr++] = Load[--LodPtr];
          Equals++;
          }
      break;

      default:
        if(isalnum(Str[n]) || Str[n] == '.')
          {
          while(isalnum(Str[n+1]) || Str[n+1] == '.')
            {
            n++;
            }
          if(!ExpectingArg)
            {
            SyntaxErr = 1;
            }
          NewStatement = ExpectingArg = 0;
          Len = (n+1)-InitN;
          xmo[posp].f = isfunct(&Str[InitN], Len);
          if(xmo[posp].f != NotAFnct)
            {
            if(xmo[posp].f == FnctNotFound)
              {
              e[ErrPtr].n = InitN;
              e[ErrPtr++].s = 5;
              }
            else
              {
              xmo[posp++].p = 1 - (paren + Equals)*15;
              }
            ExpectingArg = 1;
            }
          else
            {
            c = isconst(&Str[InitN], Len);
            Load[LodPtr++] = &(c->a);
            xmo[posp].f = StkLod;
            xmo[posp++].p = 1 - (paren + Equals)*15;
            n = InitN + c->len - 1;
            if(vsp >= MAX_ARGS-1)
              { /* PB 910417 safety test */
              e[ErrPtr].n = InitN;
              e[ErrPtr++].s = 7;
              break;
              }
            }
          }
        else
          {
          if(ExpectingArg)
            {
            SyntaxErr = 0;
            }
          ExpectingArg = 1;
          e[ErrPtr].n = InitN;
          e[ErrPtr++].s = 4;
          }
      break;
      }
    if(SyntaxErr >= 0)
      {
      e[ErrPtr].n = InitN;
      e[ErrPtr++].s = SyntaxErr;
      SyntaxErr = -1;
      }
    if(posp >= MAX_OPS-1)
      { /* PB 901103 added safety test here */
      e[ErrPtr].n = InitN;
      e[ErrPtr++].s = 7;
      break;
      }
    if(ErrPtr > 50)         /* PB 910417 safety test */
      {
      break;
      }
    }

  xmo[posp].f = (void(*)(void))0;
  xmo[posp++].p = 16;

  if(paren > 0)
    {
    e[ErrPtr].n = n;
    e[ErrPtr++].s = 3;
    }

  if (ErrPtr)
    {
    int i, j, k, m;
    char msgbuf[700];  /* PB replaced printf loop by build msgbuf & warning */
    /* warning defined to have max 9 lines, show at most first 3 errors */
    msgbuf[0] = 0;
    for(n = 0; n < ErrPtr && n < 3; n++)
      {
      if (n)
        strcat(msgbuf,"\n");
      sprintf(&msgbuf[strlen(msgbuf)], "Error(%d):  %s\n  ", e[n].s,
         ErrStrings[e[n].s]);
      j = 24;
      if ((i = e[n].n - j) < 0)
        {
        j = e[n].n;
        i = 0;
        }
      else
        {
        strcat(msgbuf,"...");
        j += 3;
        }
      k = strlen(msgbuf);
      m = i + 66;
      while (i < m && Str[i])
        {
        if ((msgbuf[k] = Str[i]) == '\n' || msgbuf[k] == '\t')
          {
          msgbuf[k] = ' ';
          }
        ++i;
        ++k;
        }
      if (Str[i])
        {
        msgbuf[k++] = '.';
        msgbuf[k++] = '.';
        msgbuf[k++] = '.';
        }
      msgbuf[k++] = '\n';
      while (--j >= -2)
        msgbuf[k++] = ' ';
      msgbuf[k++] = '^';
      msgbuf[k] = 0;
      }
    caution(main_window, (char *)&msgbuf, NULL);
    }

  if(!ErrPtr)
    {
    NextOp = 0;
    LastOp = posp;
    while(NextOp < posp)
      {
      if(xmo[NextOp].f)
        {
        RecSortPrec();
        }
      else
        {
        NextOp++;
        LastOp--;
        }
      }
    }
  else
    {
    posp = 0;
    }

  XtFree((char *)xmo);
  XtFree((char *)e);
  /* PB 910417 free all arrays if error */
  if (ErrPtr)
    {
    free_workarea();
    }
  return(ErrPtr);
}

static void (*isfunct(char *Str, int Len))(void)
{
  unsigned n;
  int functnum;    /* TIW 04-22-91 */

  n = SkipWhiteSpace(&Str[Len]);
  if(Str[Len+n] == '(')
    {
    for(n = 0; n < sizeof(FnctList) / sizeof(struct FNCT_LIST); n++)
      {
      if(strlen(FnctList[n].s) == Len)
        {
        if(!strnicmp(FnctList[n].s, Str, Len))
          {
          /* count function variables */
          if((functnum = whichfn(Str, Len)) != 0)
            if(functnum > maxfn)
              maxfn = functnum;
            return(*FnctList[n].ptr);
          }
        }
      }
    return(FnctNotFound);
    }
  return(NotAFnct);
}

static int whichfn(char *s, int len)
{
  int out;
  if(len != 3)
    out = 0;
  else if(strnicmp(s,"fn",2))
    out = 0;
  else
    out = atoi(s+2);
  if(out < 1 || out > 4)
    out = 0;
  return(out);
}

static unsigned SkipWhiteSpace(char *Str)
{
  unsigned n, Done;

  for(Done = n = 0; !Done; n++)
    {
    switch(Str[n])
      {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
         break;
      default:
         Done = 1;
      }
    }
  return(n - 1);
}

static struct ConstArg *isconst(char *Str, int Len)
{
  _CMPLX z;
  unsigned n, j;

  for(n = 0; n < vsp; n++)
    {
    if(xmv[n].len == Len)
      {
      if(!strnicmp(xmv[n].s, Str, Len))
        {
        if(n == 7)        /* The formula uses 'rand'. */
          RandomSeed();
        return(&xmv[n]);
        }
      }
    }

  xmv[vsp].s = Str;
  xmv[vsp].len = Len;
  xmv[vsp].a.d.x = xmv[vsp].a.d.y = 0.0;

  if(isdigit(Str[0]) || Str[0] == '.')
    {
    if(xmo[posp-1].f == StkNeg)
      {
      posp--;
      Str = Str - 1;
      InitN--;
      xmv[vsp].len++;
      }

    for(n = 1; isdigit(Str[n]) || Str[n] == '.'; n++);

    if(Str[n] == ',')
      {
      j = n + SkipWhiteSpace(&Str[n+1]) + 1;
      if(isdigit(Str[j]) || (Str[j] == '-' && isdigit(Str[j+1])))
        {
        z.y = atof(&Str[j]);
        for(; isdigit(Str[j]) || Str[j] == '.' || Str[j] == '-'; j++);
          xmv[vsp].len = j;
        }
      else
        z.y = 0.0;
      }
    else
      z.y = 0.0;
    z.x = atof(Str);
    switch(MathType)
      {
      case D_MATH:
      case L_MATH:
      case M_MATH:
         xmv[vsp].a.d = z;
         break;
      }
    xmv[vsp].s = Str;
    }
  return(&xmv[vsp++]);
}

void SetRandFnct(void)
{
  unsigned Seed;

  if(!SetRandom)
    RandNum = Arg1->l.x ^ Arg1->l.y;

  Seed = (unsigned)RandNum ^ (unsigned)(RandNum >> 16);
  srand(Seed);
  SetRandom = 1;

  /* Clear out the seed */
  NewRandNum();
  NewRandNum();
  NewRandNum();
}

static void RandomSeed(void)
{
  time_t ltime;

  /* Use the current time to randomize the random number sequence. */
  time(&ltime);
  srand((unsigned int)ltime);

  NewRandNum();
  NewRandNum();
  NewRandNum();
  Randomized = 1;
}

static void RecSortPrec(void)
{
  int ThisOp = NextOp++;

  while(xmo[ThisOp].p > xmo[NextOp].p && NextOp < posp)
    RecSortPrec();
  xmf[OpPtr++] = xmo[ThisOp].f;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: formula.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.6  1995/06/02  06:09:35  darryl
 * Change default button in some dialogs from Cancel to Ok.
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
 * Revision 0.101  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
