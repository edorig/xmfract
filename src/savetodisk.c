
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: savetodisk.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* global variables */
extern char s_cantwrite[];
extern char s_cantcreate[];

/* static variables */
static int timedsave;

int savetodisk(char *filename)
  {
  char tmpmsg[41]; /* before openfile in case of overrun */
  char openfile[256], openfiletype[256];
  char tmpfile[256];
  int newfile;
  int i, j, outcolor1, outcolor2, interrupted;

restart:

  save16bit = disk16bit;

  if (gif87a_flag) /* not storing non-standard fractal info */
    save16bit = 0;

  strcpy(openfile,filename);        /* decode and open the filename */

  strcpy(openfiletype,DEFAULTFRACTALTYPE);/* determine the file extension */

  if (save16bit)
    strcpy(openfiletype,".pot");

  for (i = 0; i < (int)strlen(openfile); i++)
    if (openfile[i] == '.')
    {
    strcpy(openfiletype,&openfile[i]);
    openfile[i] = 0;
    }
  /* The string openfiletype now contains the string that is at the right of the leftmost '.' character. This is wrong: the file extension is the string at the right of the rightmost '.' . There will be no problem if there is no '.' in the directory name. To mitigate this bug openfiletype is dimensioned at the same size as openfile. */ 
  if (resave_flag != 1)
    updatesavename(filename); /* for next time */


  strcat(openfile,openfiletype);

  strcpy(tmpfile,openfile);

  if (access(openfile,0) != 0) /* file doesn't exist */
    newfile = 1;
  else
    { /* file already exists */
    if (overwrite_flag == 0)
      {
      if (resave_flag == 0)
        goto restart;
      if (started_resaves == 0)
        {
        /* first save of a savetime set */
        updatesavename(filename);
        goto restart;
        }
      }
    if (access(openfile,2) != 0)
      {
      sprintf((char *) &tmpmsg,s_cantwrite,openfile);
      warning(main_window, (char *) &tmpmsg, NULL);
      return -1;
      }
    newfile = 0;
    i = strlen(tmpfile);
    while (--i >= 0 && tmpfile[i] != SLASHC)
      tmpfile[i] = '\0';
    strcat(tmpfile,"fractint.tmp");
    }

  started_resaves = (resave_flag == 1) ? 1 : 0;
  if (resave_flag == 2) /* final save of savetime set? */
    resave_flag = 0;

  if ((out=fopen(tmpfile,"wb")) == NULL)
    {
    sprintf((char *) &tmpmsg,s_cantcreate,tmpfile);
    warning(main_window, (char *) &tmpmsg, NULL);
    return -1;
    }

  xmbusy = 1;

  if (debugflag != 200)
    interrupted = encoder();
  else
    interrupted = timer(2,NULL);    /* invoke encoder() via timer */

  xmbusy = 0;

  fclose(out);

  if (interrupted)
    {
    char buf[200];
    sprintf(buf,"Save of %s interrupted.\nPress Cancel to ",openfile);
    if (newfile)
      strcat(buf,"delete the file,\nPress OK to keep the partial image.");
    else
      strcat(buf,"retain the original file,\nPress OK to replace original with new partial image.");
    interrupted = 1;
    if ((confirm(main_window, buf,
      OK_BTN | CANCEL_BTN, XmDIALOG_CANCEL_BUTTON)) < 0)
      {
      interrupted = -1;
      unlink(tmpfile);
      }
    }

  if (newfile == 0 && interrupted >= 0)
    { /* replace the real file */
    unlink(openfile);        /* success assumed since we checked */
    rename(tmpfile,openfile);    /* earlier with access            */
    }

    outcolor1 = outcolor1s;
    outcolor2 = outcolor2s;
    for (j = 0; j <= last_colorbar; j++)
      {
      if ((j & 4) == 0)
        {
        if (++outcolor1 >= colors) outcolor1 = 0;
        if (++outcolor2 >= colors) outcolor2 = 0;
        }
      for (i = 0; 250*i < xdots; i++)
        { /* clear vert status bars */
        putcolor(i,j,getcolor(i,j)^outcolor1);
        putcolor(xdots-1-i,j,getcolor(xdots-1-i,j)^outcolor2);
        }
      }

  if (interrupted)
    {
    sprintf((char *) &tmpmsg," *interrupted* save ");
    warning(main_window, (char *) &tmpmsg, NULL);
    /* if batch mode, set error level */
    if (initbatch == 2 || initbatch == 5) initbatch = 3;
    return -1;
    }
  if (timedsave == 0)
    {
    buzzer(0);
    sprintf(readname, "%s", openfile);
    if (initbatch == 0 || verbose)
      {
      sprintf((char *) &tmpmsg," File saved as %s ",openfile);
      inform(main_window, (char *) &tmpmsg, NULL);
      }
    }
  return 0;
  }

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: savetodisk.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
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
 * Revision 0.301  1994/12/21  21:47:52  darryl
 * Add verbosity when saving parameters.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.101  1994/11/24  03:53:13  darryl
 * Generalize modal dialog with variable dialog types,
 * number of buttons, and default button types.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
