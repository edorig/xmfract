
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: symmetry.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static int xsym_split(int xaxis_row, int xaxis_between);
static int ysym_split(int yaxis_col, int yaxis_between);

/* set up proper symmetrical plot functions */
void setsymmetry(int sym, int uselist)
{
  int i;
  int parmszero;
  int xaxis_row, yaxis_col;         /* pixel number for origin */
  int xaxis_between = 0;
  int yaxis_between = 0; /* if axis between 2 pixels, not on one */
  double ftemp;

  symmetry = 1;
  TranspSymmetry = sym;         /* for tp3d.c, MCP 6-1-90 */

  if(sym == NOPLOT && forcesymmetry == 999)
    {
    plot = noplot;
    return;
    }

  /* NOTE: 16-bit potential disables symmetry
   * also any decomp= option and any inversion not about the origin
   * also any rotation other than 180deg and any off-axis stretch
   */

  if ((potflag && pot16bit) || (invert && inversion[2] != 0.0) ||
         decomp[0] != 0 || xxmin!=xx3rd || yymin!=yy3rd)
    {
    return;
    }

  if(sym != XAXIS && sym != XAXIS_NOPARM && inversion[1] != 0.0 &&
            forcesymmetry == 999)
    {
    return;
    }

  if(forcesymmetry < 999)
    sym = forcesymmetry;
  else if(forcesymmetry == 1000)
    forcesymmetry = sym;  /* for backwards compatibility */
  else if(outside==REAL || outside==IMAG || outside==MULT || outside==SUM)
    return;

  parmszero = (parm.x == 0.0 && parm.y == 0.0 && useinitorbit != 1);

  switch (fractype)
    {
    case LMANLAMFNFN:      /* These need only P1 checked. */
    case FPMANLAMFNFN:     /* P2 is used for a switch value */
    case LMANFNFN:         /* These have NOPARM set in fractalp.c, */
    case FPMANFNFN:        /* but it only applies to P1. */
    case FPMANDELZPOWER:   /* or P2 is an exponent */
    case LMANDELZPOWER:
    case FPMANZTOZPLUSZPWR:
      break;
    default:   /* Check P2 for the rest */
      parmszero = (parmszero && parm2.x == 0.0 && parm2.y == 0.0);
    }

  xaxis_row = yaxis_col = -1;

  if (fabs(yymin+yymax) < fabs(yymin)+fabs(yymax)) /* axis is on screen */
    {
    ftemp = (0.0-yymax) / (yymin-yymax) * (ydots-1) + 0.25;
    xaxis_row = ftemp;
    xaxis_between = (ftemp - xaxis_row >= 0.5);
    if (uselist == 0 && (!xaxis_between || (xaxis_row+1)*2 != ydots))
      xaxis_row = -1; /* can't split screen, so dead center or not at all */
    }

  if (fabs(xxmin+xxmax) < fabs(xxmin)+fabs(xxmax)) /* axis is on screen */
    {
    ftemp = (0.0-xxmin) / (xxmax-xxmin) * (xdots-1) + 0.25;
    yaxis_col = ftemp;
    yaxis_between = (ftemp - yaxis_col >= 0.5);
    if (uselist == 0 && (!yaxis_between || (yaxis_col+1)*2 != xdots))
      yaxis_col = -1; /* can't split screen, so dead center or not at all */
    }

  switch(sym) /* symmetry switch */
    {
    case XAXIS_NOREAL: /* X-axis Symmetry (no real param) */
      if (parm.x != 0.0)
        break;
      goto xsym;

    case XAXIS_NOIMAG: /* X-axis Symmetry (no imag param) */
      if (parm.y != 0.0)
        break;
      goto xsym;

    case XAXIS_NOPARM: /* X-axis Symmetry  (no params)*/
      if (!parmszero)
        break;
      /*FALLTHRU*/
xsym:
    case XAXIS: /* X-axis Symmetry */
      if (xsym_split(xaxis_row,xaxis_between) == 0)
        {
        if(basin)
          plot = symplot2basin;
        else
          plot = symplot2;
        }
      break;

    case YAXIS_NOPARM: /* Y-axis Symmetry (No Parms)*/
      if (!parmszero)
        break;

    /*FALLTHRU*/
    case YAXIS: /* Y-axis Symmetry */
      if (ysym_split(yaxis_col,yaxis_between) == 0)
        plot = symplot2Y;
      break;

    case XYAXIS_NOPARM: /* X-axis AND Y-axis Symmetry (no parms)*/
      if(!parmszero)
        break;

    /*FALLTHRU*/
    case XYAXIS: /* X-axis AND Y-axis Symmetry */
      xsym_split(xaxis_row,xaxis_between);
      ysym_split(yaxis_col,yaxis_between);

      switch (worksym & 3)
        {
        case 1: /* just xaxis symmetry */
          if(basin)
            plot = symplot2basin;
          else
            plot = symplot2 ;
          break;

        case 2: /* just yaxis symmetry */
          if (basin) /* got no routine for this case */
            {
            ixstop = xxstop; /* fix what split should not have done */
            symmetry = 1;
            }
          else
            plot = symplot2Y;
          break;

        case 3: /* both axes */
          if(basin)
            plot = symplot4basin;
          else
            plot = symplot4 ;
        }
      break;

    case ORIGIN_NOPARM: /* Origin Symmetry (no parms)*/
      if (!parmszero)
        break;

    /*FALLTHRU*/
    case ORIGIN: /* Origin Symmetry */
originsym:
      if ( xsym_split(xaxis_row,xaxis_between) == 0 &&
           ysym_split(yaxis_col,yaxis_between) == 0)
        {
        plot = symplot2J;
        ixstop = xxstop; /* didn't want this changed */
        }
      else
        {
        iystop = yystop; /* in case first split worked */
        symmetry = 1;
        worksym = 0x30; /* let it recombine with others like it */
        }
      break;

    case PI_SYM_NOPARM:
      if (!parmszero)
        break;

    /*FALLTHRU*/
    case PI_SYM:              /* PI symmetry */
      if(fabs(xxmax - xxmin) < PI/4)
         break; /* no point in pi symmetry if values too close */

      if(invert && forcesymmetry == 999)
        goto originsym;
      plot = symPIplot ;
      symmetry = 0;
      if ( xsym_split(xaxis_row,xaxis_between) == 0 &&
           ysym_split(yaxis_col,yaxis_between) == 0)
        {
        if(parm.y == 0.0)
          plot = symPIplot4J; /* both axes */
        else
          plot = symPIplot2J; /* origin */
        }
      else
        {
        iystop = yystop; /* in case first split worked */
        worksym = 0x30;  /* don't mark pisym as ysym, just do it unmarked */
        }
      pixelpi = (PI/fabs(xxmax-xxmin))*xdots; /* PI in pixels */
      if ( (ixstop = xxstart+pixelpi-1 ) > xxstop)
        ixstop = xxstop;
      if (plot == symPIplot4J && ixstop > (i = (xxstart+xxstop)/2))
        ixstop = i;
      break;

    default: /* no symmetry */
      break;
    }
}

static int xsym_split(int xaxis_row,int xaxis_between)
{
  int i;

  if ((worksym&0x11) == 0x10) /* already decided not sym */
    return(1);
  if ((worksym&1) != 0) /* already decided on sym */
    iystop = (yystart+yystop)/2;
  else /* new window, decide */
    {
    worksym |= 0x10;
    if (xaxis_row <= yystart || xaxis_row >= yystop)
      return(1); /* axis not in window */
    i = xaxis_row + (xaxis_row - yystart);
    if (xaxis_between)
      ++i;
    if (i > yystop) /* split into 2 pieces, bottom has the symmetry */
      {
      if (num_worklist >= MAXCALCWORK-1) /* no room to split */
        return(1);
      iystop = xaxis_row - (yystop - xaxis_row);
      if (!xaxis_between)
        --iystop;
      add_worklist(xxstart,xxstop,iystop+1,yystop,iystop+1,workpass,0);
      yystop = iystop;
      return(1); /* tell set_symmetry no sym for current window */
      }

    if (i < yystop) /* split into 2 pieces, top has the symmetry */
      {
      if (num_worklist >= MAXCALCWORK-1) /* no room to split */
        return(1);
      add_worklist(xxstart,xxstop,i+1,yystop,i+1,workpass,0);
      yystop = i;
      }
    iystop = xaxis_row;
    worksym |= 1;
    }
  symmetry = 0;
  return(0); /* tell set_symmetry its a go */
}

static int ysym_split(int yaxis_col,int yaxis_between)
{
  int i;

  if ((worksym&0x22) == 0x20) /* already decided not sym */
    return(1);
  if ((worksym&2) != 0) /* already decided on sym */
    ixstop = (xxstart+xxstop)/2;
  else /* new window, decide */
    {
    worksym |= 0x20;
    if (yaxis_col <= xxstart || yaxis_col >= xxstop)
      return(1); /* axis not in window */
    i = yaxis_col + (yaxis_col - xxstart);
    if (yaxis_between)
      ++i;
    if (i > xxstop) /* split into 2 pieces, right has the symmetry */
      {
      if (num_worklist >= MAXCALCWORK-1) /* no room to split */
        return(1);
      ixstop = yaxis_col - (xxstop - yaxis_col);
      if (!yaxis_between)
        --ixstop;
      add_worklist(ixstop+1,xxstop,yystart,yystop,yystart,workpass,0);
      xxstop = ixstop;
      return(1); /* tell set_symmetry no sym for current window */
      }
    if (i < xxstop) /* split into 2 pieces, left has the symmetry */
      {
      if (num_worklist >= MAXCALCWORK-1) /* no room to split */
        return(1);
      add_worklist(i+1,xxstop,yystart,yystop,yystart,workpass,0);
      xxstop = i;
      }
    ixstop = yaxis_col;
    worksym |= 2;
    }
  symmetry = 0;
  return(0); /* tell set_symmetry its a go */
}

int FnPlusFnSym()
{
  static char fnplusfn[7][7] =
    {
    /* fn2 ->sin     cos    sinh    cosh   exp    log    sqr  */
    /* fn1 */
    /* sin */ {PI_SYM,XAXIS, XYAXIS, XAXIS, XAXIS, XAXIS, XAXIS},
    /* cos */ {XAXIS, PI_SYM,XAXIS,  XYAXIS,XAXIS, XAXIS, XAXIS},
    /* sinh*/ {XYAXIS,XAXIS, XYAXIS, XAXIS, XAXIS, XAXIS, XAXIS},
    /* cosh*/ {XAXIS, XYAXIS,XAXIS,  XYAXIS,XAXIS, XAXIS, XAXIS},
    /* exp */ {XAXIS, XYAXIS,XAXIS,  XAXIS, XYAXIS,XAXIS, XAXIS},
    /* log */ {XAXIS, XAXIS, XAXIS,  XAXIS, XAXIS, XAXIS, XAXIS},
    /* sqr */ {XAXIS, XAXIS, XAXIS,  XAXIS, XAXIS, XAXIS, XYAXIS}
    };

  if(parm.y == 0.0 && parm2.y == 0.0)
    {
    /* bounds of array JCO 5/6/92*/
    if(trigndx[0] < 7 && trigndx[1] < 7)
      symmetry = fnplusfn[trigndx[0]][trigndx[1]];  /* JCO 5/6/92 */
    }
  else
    /* defaults to XAXIS symmetry JCO 5/6/92 */
    symmetry = NOSYM;

  return(0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: symmetry.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
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
