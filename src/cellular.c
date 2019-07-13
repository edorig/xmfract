
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: cellular.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 * standalone engine for "cellular"
 *
 * Originally coded by Ken Shirriff.
 * Modified beyond recognition by Jonathan Osuch.
 *   Original or'd the neighborhood, changed to sum the neighborhood
 *   Changed prompts and error messages
 *   Added CA types
 *   Set the palette to some standard? CA colors
 *   Changed *cell_array to near and used dstack so put_line and get_line
 *     could be used all the time
 *   Made space bar generate next screen
 *   Increased string/rule size to 16 digits and added CA types 9/20/92
 */

/* static functions */
static void abort_cellular(int err, int t);
static void set_Cellular_palette(void);

static unsigned char *cell_array[2];

short r, k_1, rule_digits;
int lstscreenflag;

static void abort_cellular(int err, int t)
{
  int i;
  char msg[128];
  switch (err)
    {
    case BAD_T:
      sprintf(msg,"Bad t=%d, aborting\n", t);
      inform(main_window, (char *) &msg, NULL);
    break;

    case BAD_MEM:
      sprintf(msg, "Insufficient free memory for calculation");
      warning(main_window, (char *) &msg, NULL);
    break;

    case STRING1:
      sprintf(msg, "String can be a maximum of 16 digits");
      inform(main_window, (char *) &msg, NULL);
    break;

    case STRING2:
      sprintf(msg, "Make string of 0's through %d%s's", k_1);
      inform(main_window, (char *) &msg, NULL);
    break;

    case TABLEK:
      sprintf(msg, "Make Rule with 0's through %d's", k_1);
      inform(main_window, (char *) &msg, NULL);
    break;
    case TYPEKR:
        inform(main_window,
         "Type must be 21, 31, 41, 51, 61,\n22, 32, 42, 23, 33, 24, 25, 26, 27", NULL);
    break;

    case RULELENGTH:
      sprintf(msg, "Rule must be %d digits long", rule_digits);
      inform(main_window, msg, NULL);
    break;

    case CELLULAR_DONE:
    break;
    }
  if(cell_array[0] != NULL)
    XtFree((char *)cell_array[0]);

  if(cell_array[1] != NULL)
    XtFree((char *)cell_array[1]);
}

int cellular()
{
  short start_row;
  short filled, notfilled;
  unsigned short cell_table[32];
  unsigned short init_string[16];
  unsigned short kr, k;
  unsigned long lnnmbr;
  int i;
  short t, t2;
  long randparam;
  double n;
  char buf[30];

  set_Cellular_palette();

  randparam = param[0];
  lnnmbr = param[3];
  kr = param[2];
  switch(kr)
    {
    case 21:
    case 31:
    case 41:
    case 51:
    case 61:
    case 22:
    case 32:
    case 42:
    case 23:
    case 33:
    case 24:
    case 25:
    case 26:
    case 27:
    break;

    default:
      abort_cellular(TYPEKR, 0);
      return -1;
    /*NOTREACHED*/
    break;
    }

  r = kr % 10; /* Number of nearest neighbors to sum */
  k = kr / 10; /* Number of different states, k=3 has states 0,1,2 */
  k_1 = k - 1; /* Highest state value, k=3 has highest state value of 2 */

  rule_digits = (r * 2 + 1) * k_1 + 1; /* Number of digits in the rule */

  if ((!rflag) && randparam == -1)
    --rseed;
  if (randparam != 0 && randparam != -1)
    {
    n = param[0];
    sprintf(buf,"%.16g",n); /* number of digits in initial string */
    t = strlen(buf);
    if (t>16 || t <= 0)
      {
      abort_cellular(STRING1, 0);
      return -1;
      }
    for (i=0;i<16;i++)
      init_string[i] = 0; /* zero the array */
    t2 = (short) ((16 - t)/2);
    for (i=0;i<t;i++)
      { /* center initial string in array */
      init_string[i+t2] = buf[i] - 48; /* change character to number */
      if (init_string[i+t2]>(unsigned short)k_1)
        {
        abort_cellular(STRING2, 0);
        return -1;
        }
      }
    }

  srand(rseed);
  if (!rflag)
    ++rseed;

  /* generate rule table from parameter 1 */
  /* gcc can't manage to convert a big double to an unsigned long properly. */

  if (param[1]>0x7fffffff)
    {
    n = (param[1]-0x7fffffff);
    n += 0x7fffffff;
    }
  else
    n = param[1];

  if (n == 0)
    { /* calculate a random rule */
    n = (double)(rand15()%(int)k);
    for (i=1;i<rule_digits;i++)
      {
      n *= 10;
      n += (double)(rand15()%(int)k);
      }
    param[1] = n;
    }

  sprintf(buf,"%.*g",rule_digits ,n);
  t = strlen(buf);

  if (rule_digits < t || t < 0)
    { /* leading 0s could make t smaller */
    abort_cellular(RULELENGTH, 0);
    return -1;
    }

  for (i=0;i<rule_digits;i++) /* zero the table */
    cell_table[i] = 0;

  for (i=0;i<t;i++)
    { /* reverse order */
    cell_table[i] = buf[t-i-1] - 48; /* change character to number */
    if (cell_table[i]>(unsigned short)k_1)
      {
      abort_cellular(TABLEK, 0);
      return -1;
      }
    }

  start_row = 0;
  cell_array[0] = (unsigned char *)XtMalloc(ixstop+1);
  cell_array[1] = (unsigned char *)XtMalloc(ixstop+1);
  if (cell_array[0]==NULL || cell_array[1]==NULL)
    {
    abort_cellular(BAD_MEM, 0);
    return(-1);
    }

  /* nxtscreenflag toggled by space bar in fractint.c, 1 for continuous */
  /* 0 to stop on next screen */

  filled = 0;
  notfilled = 1-filled;
  if (resuming && !nxtscreenflag && !lstscreenflag)
    {
    start_resume();
    get_resume(sizeof(int),&start_row,0);
    end_resume();
    get_line(start_row,0,ixstop,cell_array[filled]);
    }
  else if (nxtscreenflag && !lstscreenflag)
    {
    start_resume();
    end_resume();
    get_line(iystop,0,ixstop,cell_array[filled]);
    param[3] += iystop + 1;
    start_row = -1; /* after 1st iteration its = 0 */
    }
  else
    {
    if(rflag || randparam==0 || randparam==-1)
      {
      for (col=0;col<=ixstop;col++)
        {
        cell_array[filled][col] = (unsigned short)(rand15()%(int)k);
        }
      } /* end of if random */
    else
      {
      for (col=0;col<=ixstop;col++)
        { /* Clear from end to end */
        cell_array[filled][col] = 0;
        }
      i = 0;
      for (col=(ixstop-16)/2;col<(ixstop+16)/2;col++)
        { /* insert initial */
        cell_array[filled][col] = init_string[i++];    /* string */
        }
      } /* end of if not random */

    if (lnnmbr != 0)
      lstscreenflag = 1;
    else
      lstscreenflag = 0;
    put_line(start_row,0,ixstop,cell_array[filled]);
    }
  start_row++;

  /* This section calculates the starting line when it is not zero */
  /* This section can't be resumed since no screen output is generated */
  /* calculates the (lnnmbr - 1) generation */

  if (lstscreenflag)
    { /* line number != 0 & not resuming & not continuing */
    for (row = start_row; row < lnnmbr; row++)
      {
      if(rflag || randparam==0 || randparam==-1)
        {
        /* Use a random border */
        for (i=0;i<=r;i++)
          {
          cell_array[notfilled][i]=(unsigned short)(rand15()%(int)k);
          cell_array[notfilled][ixstop-i]=(unsigned short)(rand15()%(int)k);
          }
        }
      else
        {
        /* Use a zero border */
        for (i=0;i<=r;i++)
          {
          cell_array[notfilled][i]=0;
          cell_array[notfilled][ixstop-i]=0;
          }
        }

      t = 0; /* do first cell */
      for (i=0;i<=r+r;i++)
        t += cell_array[filled][i];
      if (t>rule_digits || t<0)
        {
        abort_cellular(BAD_T, t);
        return(-1);
        }
      cell_array[notfilled][r] = cell_table[t];

      /* use a rolling sum in t */
      for (col=r+1;col<ixstop-r;col++)
        { /* now do the rest */
        t = t + cell_array[filled][col+r] - cell_array[filled][col-r-1];
        if (t>rule_digits || t<0)
          {
          abort_cellular(BAD_T, t);
          return(-1);
          }
        cell_array[notfilled][col] = cell_table[t];
        }

      filled = notfilled;
      notfilled = 1-filled;
      if (check_key() != 0)
        {
        abort_cellular(CELLULAR_DONE, 0);
        alloc_resume(10,1);
        put_resume(sizeof(int),&row,0);
        return -1;
        }
      }
    start_row = 0;
    lstscreenflag = 0;
    }

  /* This section does all the work */

contloop:

  for (row = start_row; row <= iystop; row++)
    {
    if(rflag || randparam==0 || randparam==-1)
      {
      /* Use a random border */
      for (i=0;i<=r;i++)
        {
        cell_array[notfilled][i]=(unsigned short)(rand15()%(int)k);
        cell_array[notfilled][ixstop-i]=(unsigned short)(rand15()%(int)k);
        }
      }
    else
      {
      /* Use a zero border */
      for (i=0;i<=r;i++)
        {
        cell_array[notfilled][i]=0;
        cell_array[notfilled][ixstop-i]=0;
        }
      }
    t = 0; /* do first cell */
    for (i=0;i<=r+r;i++)
      t += cell_array[filled][i];
    if (t>rule_digits || t<0)
      {
      abort_cellular(BAD_T, t);
      return(-1);
      }
    cell_array[notfilled][r] = cell_table[t];

    /* use a rolling sum in t */
    for (col=r+1;col<ixstop-r;col++)
      { /* now do the rest */
      t = t + cell_array[filled][col+r] - cell_array[filled][col-r-1];
      if (t>rule_digits || t<0)
        {
        abort_cellular(BAD_T, t);
        return(-1);
        }
      cell_array[notfilled][col] = cell_table[t];
      }

    filled = notfilled;
    notfilled = 1-filled;
    put_line(row,0,ixstop,cell_array[filled]);
    if (check_key() != 0)
      {
      abort_cellular(CELLULAR_DONE, 0);
      alloc_resume(10,1);
      put_resume(sizeof(int),&row,0);
      return -1;
      }
    }
  if(nxtscreenflag)
    {
    param[3] += iystop + 1;
    start_row = -1; /* after 1st iteration its = 0 */
    goto contloop;
    }
  abort_cellular(CELLULAR_DONE, 0);
  return 1;
}

int CellularSetup(void)
{
  if (!resuming)
    nxtscreenflag = 0; /* initialize flag */
  timer(0,curfractalspecific->calctype);
  return(0);
}

static void set_Cellular_palette()
{
  static Palettetype Red    = { 42,  0,  0 };
  static Palettetype Green  = { 10, 35, 10 };
  static Palettetype Blue   = { 13, 12, 29 };
  static Palettetype Yellow = { 60, 58, 18 };
  static Palettetype Brown  = { 42, 21,  0 };

  if (mapdacbox)
    return;        /* map= specified */

  dacbox[0][0] = 0 ;
  dacbox[0][1] = 0 ;
  dacbox[0][2] = 0 ;

  dacbox[1][0] = Red.red;
  dacbox[1][1] = Red.green;
  dacbox[1][2] = Red.blue;

  dacbox[2][0] = Green.red;
  dacbox[2][1] = Green.green;
  dacbox[2][2] = Green.blue;

  dacbox[3][0] = Blue.red;
  dacbox[3][1] = Blue.green;
  dacbox[3][2] = Blue.blue;

  dacbox[4][0] = Yellow.red;
  dacbox[4][1] = Yellow.green;
  dacbox[4][2] = Yellow.blue;

  dacbox[5][0] = Brown.red;
  dacbox[5][1] = Brown.green;
  dacbox[5][2] = Brown.blue;

  SetTgaColors();
  spindac(0,0);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: cellular.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.6  1995/05/15  19:30:32  darryl
 * Remove trailing spaces from source code lines.
 *
 * Revision 1.5  1995/05/11  15:19:01  darryl
 * Revision sync from Tom's branch.
 *
 * Revision 1.4.1.1  1995/05/11  10:42:14  tom
 * minor fixes and cleanups
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
 * Revision 0.102  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.101  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
