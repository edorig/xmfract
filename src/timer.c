
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: timer.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static long timer_start;

#ifdef __STDC__
int timer(int timertype, int(*subrtn)(), ...)
#else
int timer(timertype, subrtn, va_alist)
     int timertype;
     int(*subrtn)();
     va_dcl
#endif
{
  va_list arg_marker;
  char *timestring;
  time_t ltime;
  FILE *fp = NULL;
  int out = 0;
  int i;
  int do_bench;

  VA_START(arg_marker,subrtn);

  do_bench = timerflag; /* record time? */

  if (timertype == 2)   /* encoder, record time only if debug=200 */
    do_bench = (debugflag == 200);

  if(do_bench)
    fp=fopen("bench","a");

  timer_start = clock_ticks();

  switch(timertype)
    {
    case 0:
      out = (*subrtn)();
    break;

    case 1:
      i = va_arg(arg_marker,int);
      va_end(arg_marker);
      out = decoder(i);       /* not indirect, safer with overlays */
    break;

    case 2:
      out = encoder();        /* not indirect, safer with overlays */
    break;
    }

  timer_interval = (clock_ticks() - timer_start) /
    ((clk_ticks/100) < 1 ? 1 : (clk_ticks/100));

  if(do_bench)
    {
    time(&ltime);
    timestring = ctime(&ltime);
    timestring[24] = 0; /*clobber newline in time string */
    switch(timertype)
      {
      case 1:
        fprintf(fp,"decode ");
      break;

      case 2:
        fprintf(fp,"encode ");
      break;
      }

    fprintf(fp,"%s type=%s resolution = %dx%d maxiter=%d",
      timestring,
      curfractalspecific->name,
      xdots,
      ydots,
      maxit);

    fprintf(fp," time= %ld.%02ld secs\n",timer_interval/100,timer_interval%100);

    if(fp != NULL)
      fclose(fp);
    }

  return(out);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: timer.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.7  1995/04/08  19:12:06  tom
 * Enhance timer functions. Uses sysconf(3) to
 * determine CLK_TCK if the function is available.
 *
 * Revision 1.6  1995/03/30  02:32:37  darryl
 * Rearrange the order of the buttons at the bottom of all
 * dialogs so that the order is consistent throughout the
 * program. Add a workarea child to the entry selection
 * dialogs for formula, ifs. and lsystem that displays
 * the entry in the workarea. Lint and optimization cleanup.
 *
 * Revision 1.5  1995/03/20  18:34:04  darryl
 * Add a check for CLK_TCK < 100 to avoid divide-by-zero.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.2  1995/03/14  15:11:21  tom
 * changed #if to #ifdef
 *
 * Revision 1.1.1.1  1995/03/11  03:25:30  tom
 * changed varargs handling
 *
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
