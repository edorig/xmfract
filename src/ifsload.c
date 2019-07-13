
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: ifsload.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static variables */
static char insufficient_ifs_mem[]={"Insufficient memory for IFS"};
static float fstack[4096];

int ifsload()  /* read in IFS parameters */
{
  int i;
  FILE *ifsfile;
  char buf[201];
  char tempstr[128];
  char *bufptr;
  int ret,rowsize;

  if (ifs_defn)
    { /* release prior parms */
    XtFree((char *)ifs_defn);
    ifs_defn = NULL;
    }

  if((int)(strlen((char *) &IFSFileName) == 0))
    {
    sprintf(tempstr, "No IFS file specified in the saved image.\nSelect the ifs definition file containing the \"%s\" definition,\nthen re-load this saved image.",
(char *) &IFSName);
    ret = confirm(main_window, (char *) &tempstr,
      OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
    if(ret != 1)
      return(-1);
    else
      {
      select_ifs_file(main_window, "0");
      return(-1);
      }
    }

  ifs_type = 0;
  rowsize = IFSPARM;
  if (find_file_item(IFSFileName,IFSName,&ifsfile) < 0)
     return(-1);

  fgets(buf,200,ifsfile);
  strlwr(buf);
  bufptr = &buf[0];
  while (*bufptr)
    {
    if (strncmp(bufptr,"(3d)",4) == 0)
      {
      ifs_type = 1;
      rowsize = IFS3DPARM;
      }
    ++bufptr;
    }

  for (i = 0; i < (NUMIFS+1)*IFS3DPARM; ++i)
    fstack[i] = 0.0;

  i = ret = 0;

  while (fscanf(ifsfile," %f ",&fstack[i]))
    {
    if (++i >= NUMIFS*rowsize)
      {
      static char message[]={"IFS definition has too many lines"};
      caution(main_window, (char *) &message, NULL);
      ret = -1;
      break;
      }
    }

  if ((i % rowsize) != 0 || getc(ifsfile) != '}')
    {
    static char message[]={"invalid IFS definition"};
    caution(main_window, (char *) &message, NULL);
    ret = -1;
    }

  if (i == 0 && ret == 0)
    {
    static char message[]={"Empty IFS definition"};
    caution(main_window, (char *) &message, NULL);
    ret = -1;
    }

  fclose(ifsfile);

  if (ret == 0)
    {
    numaffine = i/rowsize;
    if ((ifs_defn = (float *)XtMalloc(
      (long)((NUMIFS+1)*IFS3DPARM*sizeof(float)))) == NULL)
      {
      warning(main_window, (char *) &insufficient_ifs_mem, NULL);
      ret = -1;
      }
    else
      for (i = 0; i < (NUMIFS+1)*IFS3DPARM; ++i)
        ifs_defn[i] = fstack[i];
    }
  return(ret);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: ifsload.c,v $
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
