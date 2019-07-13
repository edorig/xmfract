
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: boundary_trace.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void step_col_row(void);

/* static variables */
enum direction {Boundary_North,Boundary_East,Boundary_South,Boundary_West};
enum direction going_to;
static int trail_col;
static int trail_row;

int  bound_trace_main()
{
  enum direction coming_from;
  unsigned int match_found, continue_loop;
  int trail_color, fillcolor_used, last_fillcolor_used;
  int max_putline_length;
  int right, left, length;
  char msg[128];

  if (inside == 0 || outside == 0)
    {
    sprintf((char *) &msg, "%s",
      "Boundary tracing cannot be used with inside=0 or outside=0.");
    inform(main_window, (char *) &msg, NULL);
    return(-1);
    }
  if (colors < 16)
    {
    sprintf((char *) &msg, "%s",
      "Boundary tracing cannot be used with < 16 colors.");
    inform(main_window, (char *) &msg, NULL);
    return(-1);
    }

  last_fillcolor_used = -999;
  got_status = 2;
  max_putline_length = 0; /* reset max_putline_length */
  for (currow = iystart; currow <= iystop; currow++)
    {
    reset_periodicity = 1; /* reset for a new row */
    color = bkcolor;
    for (curcol = ixstart; curcol <= ixstop; curcol++)
      {
      if (getcolor(curcol, currow) != bkcolor)
        continue;

      trail_color = color;
      row = currow;
      col = curcol;
      if ((*calctype)()== -1) /* color, row, col are global */
        {
        if (iystop != yystop)  /* DG */
          iystop = yystop - (currow - yystart); /* allow for sym */
        add_worklist(xxstart,xxstop,currow,iystop,currow,0,worksym);
        return -1;
        }
      reset_periodicity = 0; /* normal periodicity checking */

      /*
       * This next line may cause a few more pixels to be calculated,
       * but at the savings of quite a bit of overhead
       */
      if (color != trail_color)  /* DG */
        continue;

      /* sweep clockwise to trace outline */
      trail_row = currow;
      trail_col = curcol;
      trail_color = color;
      fillcolor_used = fillcolor > 0 ? fillcolor : trail_color;
      coming_from = Boundary_West;
      going_to = Boundary_East;
      match_found = 0;
      continue_loop = TRUE;
      do
        {
        step_col_row();
        if (row >= currow
          && col >= ixstart
          && col <= ixstop
          && row <= iystop)
          {
          /* the order of operations in this next line is critical */
          if ((color = getcolor(col, row)) == bkcolor && (*calctype)()== -1)
            /* color, row, col are global for (*calctype)() */
            {
            if (iystop != yystop)  /* DG */
              iystop = yystop - (currow - yystart); /* allow for sym */
            add_worklist(xxstart,xxstop,currow,iystop,currow,0,worksym);
            return -1;
            }
          else if (color == trail_color)
            {
            if (match_found < 4) /* to keep it from overflowing */
              match_found++;
            trail_row = row;
            trail_col = col;
            advance_match();
            }
          else
            {
            advance_no_match();
            continue_loop = going_to != coming_from || match_found;
            }
          }
        else
          {
          advance_no_match();
          continue_loop = going_to != coming_from || match_found;
          }
        } while (continue_loop && (col != curcol || row != currow));

      if (match_found <= 3)  /* DG */
        { /* no hole */
        color = bkcolor;
        reset_periodicity = 1;
        continue;
        }

      /*
       * Fill in region by looping around again, filling lines to the left
       * whenever going_to is Boundary_South or Boundary_West
       */
      trail_row = currow;
      trail_col = curcol;
      coming_from = Boundary_West;
      going_to = Boundary_East;
      do
        {
        match_found = FALSE;
        do
          {
          step_col_row();
          if (row >= currow
            && col >= ixstart
            && col <= ixstop
            && row <= iystop
            && getcolor(col,row) == trail_color)
            /* getcolor() must be last */
            {
            if (going_to == Boundary_South
            || (going_to == Boundary_West && coming_from != Boundary_East))
              { /* fill a row, but only once */
              right = col;
              while (--right >= ixstart &&
                (color = getcolor(right,row)) == trail_color)
                ; /* do nothing */
              if (color == bkcolor) /* check last color */
                {
                left = right;
                while (getcolor(--left,row) == bkcolor)
                  /* Should NOT be possible for left < ixstart */
                  ; /* do nothing */
                left++; /* one pixel too far */
                if (right == left) /* only one hole */
                  (*plot)(left,row,fillcolor_used);
                else
                  { /* fill the line to the left */
                  length=right-left+1;
                  if (fillcolor_used != last_fillcolor_used
                    || length > max_putline_length)
                    { /* only reset dstack if necessary */
                    memset(dstack,fillcolor_used,length);
                    last_fillcolor_used = fillcolor_used;
                    max_putline_length = length;
                    }
                  put_line(row,left,right,dstack);
                  /* here's where all the symmetry goes */
                  if (plot == putcolor)
                    kbdcount -= length >> 4; /* seems a reasonable value */
                  else if (plot == symplot2) /* X-axis symmetry */
                    {
                    put_line(yystop-(row-yystart),left,right,dstack);
                    kbdcount -= length >> 3;
                    }
                  else if (plot == symplot2Y) /* Y-axis symmetry */
                    {
                    put_line(row,xxstop-(right-xxstart),
                      xxstop-(left-xxstart),dstack);
                    kbdcount -= length >> 3;
                    }
                  else if (plot == symplot2J)  /* Origin symmetry */
                    {
                    put_line(yystop-(row-yystart),
                      xxstop-(right-xxstart),xxstop-(left-xxstart),dstack);
                    kbdcount -= length >> 3;
                    }
                  else if (plot == symplot4) /* X-axis and Y-axis symmetry */
                    {
                    put_line(yystop-(row-yystart),left,right,dstack);
                    put_line(row,xxstop-(right-xxstart),
                      xxstop-(left-xxstart),dstack);
                    put_line(yystop-(row-yystart),
                      xxstop-(right-xxstart),xxstop-(left-xxstart),dstack);
                    kbdcount -= length >> 2;
                    }
                  else  /* cheap and easy way out */
                    {
                    int c;
                    for (c = left; c <= right; c++)  /* DG */
                      (*plot)(c,row,fillcolor_used);
                    kbdcount -= length >> 1;
                    }
                  }
                } /* end of fill line */

              if(--kbdcount<=0)
                {
                if(check_key() != 0)
                  {
                  if (iystop != yystop)
                    iystop = yystop - (currow - yystart); /* allow for sym */
                  add_worklist(xxstart,xxstop,currow,iystop,currow,0,worksym);
                  return(-1);
                  }
                kbdcount=max_kbdcount;
                }
              }
            trail_row = row;
            trail_col = col;
            advance_match();
            match_found = TRUE;
            }
          else
            advance_no_match();
          } while (!match_found && going_to != coming_from);

        if (!match_found)
          { /* next one has to be a match */
          step_col_row();
          trail_row = row;
          trail_col = col;
          advance_match();
          }
        } while (trail_col != curcol || trail_row != currow);
      reset_periodicity = 1; /* reset after a trace/fill */
      color = bkcolor;
      }
    }
  return 0;
}

/*******************************************************************/

/* take one step in the direction of going_to */
static void step_col_row()
{
  switch (going_to)
    {
    case Boundary_North:
      col = trail_col;
      row = trail_row - 1;
    break;

    case Boundary_East:
      col = trail_col + 1;
      row = trail_row;
    break;

    case Boundary_South:
      col = trail_col;
      row = trail_row + 1;
    break;

    case Boundary_West:
      col = trail_col - 1;
      row = trail_row;
    break;
  }
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: boundary_trace.c,v $
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
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
