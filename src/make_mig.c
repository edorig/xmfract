
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: make_mig.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 * make_mig() takes a collection of individual GIF images (all
 * presumably the same resolution and all presumably generated
 * by xmfract and its "divide and conquer" algorithm) and builds
 * a single multiple-image GIF out of them.
 */

int make_mig(int xmult, int ymult)
{
  short xstep, ystep;
  short xres, yres;
  short allxres, allyres, xtot, ytot;
  short xloc, yloc;
  short ichar;
  unsigned short allitbl, itbl;
  int i, errorflag, inputerrorflag;
  char gifin[MAXPATHLEN], gifout[MAXPATHLEN];
  unsigned char buf[32768];
  unsigned char *temp;
  FILE *out, *in;
  char msg[512];

  out = (FILE *) NULL;

  inputerrorflag = errorflag = allxres = allyres = xtot = ytot = 0;
  allitbl = itbl = 0;

  if((fractdir != NULL))
    sprintf(gifout, "%s/fractmig.gif", fractdir);
  else
    sprintf(gifout,"fractmig.gif");

  temp = &buf[0];            /* a safe place for our temp data */

  gif87a_flag = 1;            /* for now, force this */

  /* process each input image, one at a time */
  for (ystep = 0; ystep < ymult; ystep++)
    {
    for (xstep = 0; xstep < xmult; xstep++)
      {
      if (xstep == 0 && ystep == 0) /* first time through? */
        {
        /* attempt to create the output file */
        if ((out = fopen(gifout,"wb")) == NULL)
          {
          sprintf((char *) &msg, "Cannot create output file %s!\n", gifout);
          caution(main_window, (char *) &msg, NULL);
          return(-1);
          }
        }

      if((fractdir != NULL))
        {
        sprintf(gifin, "%s/frmig_%c%c.gif",
          fractdir, PAR_KEY(xstep), PAR_KEY(ystep));
        }
      else
        sprintf(gifin, "frmig_%c%c.gif", PAR_KEY(xstep), PAR_KEY(ystep));

      if ((in = fopen(gifin,"rb")) == NULL)
        {
        sprintf((char *) &msg, "Can't open file %s!\n", gifin);
        caution(main_window, (char *) &msg, NULL);
        return(-1);
        }

      sprintf((char *) &msg, "Generating multi-image GIF file %s using\n%d X and %d Y components\n", gifout, xmult, ymult);
      inform(main_window, (char *) &msg, NULL);

      inputerrorflag = 0;

      /* (read, but only copy this if it's the first time through) */
      if (fread(temp,13,1,in) != 1)    /* read the header and LDS */
        inputerrorflag = 1;

      GET16(temp[6], xres);
      GET16(temp[8], yres);

      if (xstep == 0 && ystep == 0)
        {                       /* first time through? */
        allxres = xres;         /* save the "master" resolution */
        allyres = yres;
        xtot = xres * xmult;    /* adjust the image size */
        ytot = yres * ymult;

        temp[6] = xtot;
        temp[7] = xtot >> 8;

        temp[8] = ytot;
        temp[9] = ytot >> 8;

        if (gif87a_flag)
          {
          temp[3] = '8';
          temp[4] = '7';
          temp[5] = 'a';
          }
        if (fwrite(temp,13,1,out) != 1)    /* write out the header */
          errorflag = 1;
        }                                  /* end of first-time-through */

      ichar = temp[10] & 0x07;    /* find the color table size */
      itbl = 1 << (++ichar);
      ichar = temp[10] & 0x80;    /* is there a global color table? */
      if (xstep == 0 && ystep == 0)    /* first time through? */
        allitbl = itbl;        /* save the color table size */
      if (ichar != 0)
        {        /* yup */
                 /* (read, but only copy this if it's the first time through) */
        if(fread(temp,3*itbl,1,in) != 1)    /* read the global color table */
          inputerrorflag = 2;
        if (xstep == 0 && ystep == 0)    /* first time through? */
          if (fwrite(temp,3*itbl,1,out) != 1)    /* write out the GCT */
            errorflag = 2;
        }

      if (xres != allxres || yres != allyres || itbl != allitbl)
        {
        /* Oops - our pieces don't match */
        sprintf((char *) &msg, "File %s doesn't have the same resolution as its predecessors!\n", gifin);
        caution(main_window, (char *) &msg, NULL);
        return(-1);
        }

      for (;;)
        {            /* process each information block */
        if (fread(temp,1,1,in) != 1)    /* read the block identifier */
          inputerrorflag = 3;

        if (temp[0] == 0x2c)
          {        /* image descriptor block */
          if (fread(&temp[1],9,1,in) != 1)    /* read the Image Descriptor */
            inputerrorflag = 4;

          GET16(temp[1], xloc);
          GET16(temp[3], yloc);

          xloc += (xstep * xres);    /* adjust the locations */
          yloc += (ystep * yres);

          temp[1] = xloc;
          temp[2] = xloc >> 8;

          temp[3] = yloc;
          temp[4] = yloc >> 8;

          if (fwrite(temp,10,1,out) != 1) /* write out the Image Descriptor */
            errorflag = 4;

          ichar = temp[9] & 0x80;    /* is there a local color table? */
          if (ichar != 0)
            {        /* yup */
            if (fread(temp,3*itbl,1,in) != 1) /* read the local color table */
              inputerrorflag = 5;
            if (fwrite(temp,3*itbl,1,out) != 1)    /* write out the LCT */
              errorflag = 5;
            }

          if (fread(temp,1,1,in) != 1)    /* LZH table size */
            inputerrorflag = 6;
          if (fwrite(temp,1,1,out) != 1)
            errorflag = 6;
          while (1)
            {
            if (errorflag != 0 || inputerrorflag != 0)
              /* oops - did something go wrong? */
              break;
            if (fread(temp,1,1,in) != 1)    /* block size */
              inputerrorflag = 7;
            if (fwrite(temp,1,1,out) != 1)
              errorflag = 7;
            if ((i = temp[0]) == 0)
              break;
            if (fread(temp,i,1,in) != 1)    /* LZH data block */
              inputerrorflag = 8;
            if (fwrite(temp,i,1,out) != 1)
              errorflag = 8;
            }
          }

        if (temp[0] == 0x21)
          {        /* extension block */
          /* (read, but only copy this if it's the last time through) */
          if (fread(&temp[2],1,1,in) != 1)    /* read the block type */
            inputerrorflag = 9;
          if ((!gif87a_flag) && xstep == xmult-1 && ystep == ymult-1)
            if (fwrite(temp,2,1,out) != 1)
              errorflag = 9;
          while (1)
            {
            if (errorflag != 0 || inputerrorflag != 0)
              /* oops - did something go wrong? */
              break;
            if (fread(temp,1,1,in) != 1)    /* block size */
              inputerrorflag = 10;
            if ((!gif87a_flag) && xstep == xmult-1 && ystep == ymult-1)
              if (fwrite(temp,1,1,out) != 1)
                errorflag = 10;
            if ((i = temp[0]) == 0)
              break;
            if (fread(temp,i,1,in) != 1)    /* data block */
              inputerrorflag = 11;
            if ((!gif87a_flag) && xstep == xmult-1 && ystep == ymult-1)
              if (fwrite(temp,i,1,out) != 1)
                errorflag = 11;
            }
          }

        if (temp[0] == 0x3b)
          {        /* end-of-stream indicator */
          break;            /* done with this file */
          }

        if (errorflag != 0 || inputerrorflag != 0)
          /* oops - did something go wrong? */
          break;
        }

      fclose(in);            /* done with an input GIF */

      if (errorflag != 0 || inputerrorflag != 0)
        /* oops - did something go wrong? */
        break;
      }

    if (errorflag != 0 || inputerrorflag != 0)
      /* oops - did something go wrong? */
      break;
    }

  temp[0] = 0x3b;            /* end-of-stream indicator */

  if (fwrite(temp,1,1,out) != 1)
    errorflag = 12;

  fclose(out);            /* done with the output GIF */

  if (inputerrorflag != 0) /* uh-oh - something failed */
    {
    sprintf((char *) &msg, "Process failed = early EOF on input file %s\n", gifin);
    caution(main_window, (char *) &msg, NULL);
    }

  if (errorflag != 0) /* uh-oh - something failed */
    {
    sprintf((char *) &msg, "Process failed = out of disk space?\n");
    caution(main_window, (char *) &msg, NULL);
    }

  /* tell the world we're done */
  if (errorflag == 0 && inputerrorflag == 0)
    {
    sprintf((char *) &msg, "File %s has been created\n", gifout);
    inform(main_window, (char *) &msg, NULL);
    }
  return 0;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: make_mig.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.8  1995/06/08  21:38:15  darryl
 * Fix odd screen size handling in the mig maker.
 *
 * Revision 1.7  1995/06/08  18:30:45  darryl
 * Enhance mig creation routines.
 *
 * Revision 1.6  1995/06/08  00:01:06  darryl
 * Fix mig handling.
 *
 * Revision 1.5  1995/05/03  21:29:57  darryl
 * Lint and optimization cleanup.
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
 * Revision 0.301  1995/01/10  00:40:37  darryl
 * Add comments, delete references to obsolete code.
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
