
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: gifview.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 *
 * This GIF decoder is designed for use with the xmfract program.
 * This decoder code lacks full generality in the following respects:
 * supports non-interlaced GIF files only, and calmly ignores any
 * local color maps and non-xmfract-specific extension blocks.
 *
 * GIF and 'Graphics Interchange Format' are trademarks (tm) of
 * Compuserve, Incorporated, an H&R Block Company.
 *
 *                                            Tim Wegner
 */

/* static functions */
static void close_file(void);
static int out_line_migs(unsigned char *pixels, unsigned linelen);
static int out_line_dither(unsigned char *pixels, unsigned linelen);

/* static variables */
static FILE *fpin;
static char *ditherbuf;
static unsigned int gifview_image_left;
static unsigned int gifview_image_top;
static unsigned int gifview_image_twidth;

int get_byte()
{
  return (getc(fpin)); /* EOF is -1, as desired */
}

int get_bytes(unsigned char *where,int how_many)
{
  return (fread((char *)where,1,how_many,fpin)); /* EOF is -1, as desired */
}

/* Main entry decoder */

int gifview1()
{
  unsigned char buffer[16];
  unsigned top, left, width, finished;
  char temp1[81];

  int status;
  int i, j, k, planes;

  status = 0;

  /* initialize the row count for write-lines */
  rowcount = 0;

  /* zero out the full write-line */
  for (width = 0; width < MAXPIXELS+1; width++) decoderline[width] = 0x0;

  /* Open the file */
  strcpy(temp1,readname);
  if (strchr(temp1,'.') == NULL)
    {
    strcat(temp1,DEFAULTFRACTALTYPE);
    if ((fpin = fopen(temp1,"rb")) != NULL)
      fclose(fpin);
    else
      {
      strcpy(temp1,readname);
      strcat(temp1,ALTERNATEFRACTALTYPE);
      }
    }
  if ((fpin = fopen(temp1, "rb")) == NULL)
    return (-1);

  /* Get the screen description */
  for (i = 0; i < 13; i++)
    {
    int tmp;

    buffer[i] = tmp = get_byte();
    if (tmp < 0)
      {
      close_file();
      return(-1);
      }
    }

  /* use updated GIF specs? */
  if((int)strncmp((char *) &buffer,"GIF87a",3 > 0) ||
                buffer[3] < '0' || buffer[3] > '9' ||
                buffer[4] < '0' || buffer[4] > '9' ||
                buffer[5] < 'A' || buffer[5] > 'z' )
    {
    close_file();
    return(-1);
    }

  width  = buffer[6] | (buffer[7] << 8);
  height = buffer[8] | (buffer[9] << 8);
  planes = (buffer[10] & 0x0F) + 1;
  gifview_image_twidth = width;

  if((buffer[10] & 0x80)==0)     /* color map (better be!) */
    {
    close_file();
    return(-1);
    }
  numcolors = 1 << planes;

  if (dither_flag && numcolors>2 && colors==2 && outln==out_line)
    outln = out_line_dither;

  if((verbose) && ((numcolors > colors) && (!display3d)))
    {
    char msg[1024];
    int ret;
    sprintf((char *) &msg, "There are more colors in this image than xmfract has currently\nallocated. Some colors may be distorted from the original image.\nIn addition, applying the palette editor against this image may\ncause strange and interesting things to happen.\n\nColors in this image: %d\nColors allocated:     %d\n\nContinue?\n", numcolors, colors);
    ret = confirm(main_window, (char *) &msg,
      OK_BTN | CANCEL_BTN, XmDIALOG_OK_BUTTON);
    if(ret != 1)
      return(-1);
    }

  for (i = 0; i < numcolors; i++)
    {
    for (j = 0; j < 3; j++)
      {
      if ((k = get_byte()) < 0)
        {
        close_file();
        return(-1);
        }
      if(!display3d || (glassestype != 1 && glassestype != 2))
      dacbox[i][j] = k;
      }
    }
  colorstate = 1; /* colors aren't default and not a known .map file */

  /* don't read if glasses */
  if (display3d && mapset && glassestype!=1 && glassestype != 2)
    {
    ValidateLuts(MAP_name);    /* read the palette file */
    spindac(0,0); /* load it, but don't spin */
    }
  if (dacbox[0][0] != 255)
    spindac(0,0);      /* update the DAC */

  memcpy((char *)mapdacbox,dacbox,768);

  /* Now display one or more GIF objects */
  finished = 0;
  while (!finished)
    {
    switch (get_byte())
      {
      case ';':
      /* End of the GIF dataset */

        finished = 1;
        status = 0;
      break;

      case '!':                               /* GIF Extension Block */
        get_byte();             /* read (and ignore) the ID */
          while ((i = get_byte()) > 0)     /* get the data length */
            for (j = 0; j < i; j++)
              get_byte();     /* flush the data */
      break;

      case ',':
      /*
       * Start of an image object. Read the image description.
       */

      for (i = 0; i < 9; i++)
        {
        int tmp;

        buffer[i] = tmp = get_byte();
        if (tmp < 0)
          {
          status = -1;
          break;
          }
        }
      if(status < 0)
        {
        finished = 1;
        break;
        }

      left   = buffer[0] | (buffer[1] << 8);
      top    = buffer[2] | (buffer[3] << 8);
      width  = buffer[4] | (buffer[5] << 8);
      height = buffer[6] | (buffer[7] << 8);

      if (pot16bit)
        width >>= 1;

      /* adjustments for handling MIGs */
      gifview_image_top  = top;
      if (skipxdots > 0)
        gifview_image_top /= (skipydots+1);
      gifview_image_left = left;
      if (skipydots > 0)
        gifview_image_left /= (skipxdots+1);
      if (outln==out_line &&
         (width != gifview_image_twidth || top != 0))
        {
        /* we're using normal decoding and we have a MIG */
        outln = out_line_migs;
        }

      /* Skip local color palette */
      if((buffer[8] & 0x80)==0x80)
        {      /* local map? */
        int numcolors;    /* make this local */
        planes = (buffer[8] & 0x0F) + 1;
        numcolors = 1 << planes;
        /* skip local map */
        for (i = 0; i < numcolors; i++)
          {
          for (j = 0; j < 3; j++)
            {
            if ((k = get_byte()) < 0)
              {
              close_file();
              return(-1);
              }
            }
          }
        }

      /* initialize the row count for write-lines */
      rowcount = 0;

      /* zero out the full write-line */
      {
      int i;
      for (i = 0; i < MAXPIXELS+1; i++)
        decoderline[i] = 0x0;
      }

      if (calc_status == IMG_ACTV_CALC) /* should never be so, but make sure */
        {
        calc_status = IMG_PRMS_CHGD;
        update_draw_menu(NEW_IMAGE);
        }
      xmbusy = 1;    /* for slideshow CALCWAIT */
      status = timer(1,NULL,width); /* call decoder(width) via timer */
      xmbusy = 0;    /* for slideshow CALCWAIT */
      if (calc_status == IMG_ACTV_CALC) /* e.g., set by line3d */
        {
        calctime = timer_interval; /* note how long it took */
        if (keypressed() != 0)
          {
          calc_status = IMG_INTR_NRES; /* interrupted, not resumable */
          update_draw_menu(NEW_IMAGE);
          finished = 1;
          }
        if(display3d)
          {
          calc_status = IMG_COMPLETED;
          }
        }
      /* Hey! the decoder doesn't read the last (0-length) block!! */
      if (get_byte() != 0)
        {
        status = -1;
        finished = 1;
        }
      break;

      default:
        status = -1;
        finished = 1;
      break;
      }
    }
  close_file();

  if (ditherbuf != NULL)
    { /* we're done, free dither memory */
    XtFree(ditherbuf);
    ditherbuf = NULL;
    }

  return(status);
}

static void close_file()
{
  fclose(fpin);
  fpin = NULL;
}

/* routine for MIGS that generates partial output lines */

static int out_line_migs(unsigned char *pixels, unsigned linelen)
{
  int row, startcol, stopcol;

  row = gifview_image_top + rowcount;
  startcol = gifview_image_left;
  stopcol = startcol+linelen-1;
  put_line(row, startcol, stopcol, pixels);
  rowcount++;
  return 0;
}

static int out_line_dither(pixels, linelen)
unsigned char *pixels;
unsigned linelen;
{
  int i,nexterr,brt,err;
  if(ditherbuf == NULL)
     ditherbuf = (char *)XtMalloc(linelen+1);
  memset( ditherbuf, 0, linelen+1);

  nexterr = (rand15()&0x1f)-16;
  for (i=0;i<linelen;i++)
    {
    brt = (int)((int) dacbox[pixels[i]][0]*5+dacbox[pixels[i]][1]*9 +
                (int) dacbox[pixels[i]][2]*2) >> 4;
                /* brightness from 0 to 63 */
    brt += nexterr;
    if (brt>32)
      {
      pixels[i] = 1;
      err = brt-63;
      }
    else
      {
      pixels[i] = 0;
      err = brt;
      }
    nexterr = ditherbuf[i+1]+err/3;
    ditherbuf[i] = err/3;
    ditherbuf[i+1] = err/3;
    }
  return out_line(pixels, linelen);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: gifview.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.11  1995/06/08  00:01:06  darryl
 * Fix mig handling.
 *
 * Revision 1.10  1995/06/02  05:42:01  darryl
 * Add restored images from file to restore history.
 *
 * Revision 1.9  1995/05/08  19:44:43  darryl
 * Fix bugs reported in beta test.
 *
 * Revision 1.8  1995/04/18  15:59:59  darryl
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
 * Revision 1.7  1995/04/09  03:13:08  darryl
 * Fix bug where zoom corners and calc_status were set
 * incorrectly after loading a saved image.
 *
 * Revision 1.6  1995/04/08  19:02:44  tom
 * Change calc_status to IMG_COMPLETED when a gif image
 * file is successfully loaded.
 *
 * Revision 1.5  1995/04/03  15:49:33  darryl
 * Add Brighter and Darker buttons to the Range menu.
 * Fix a problem where the color editor cancel button
 * would reset the colors to the default color map
 * after a gif image was loaded. Fix a potential
 * problem where the skipval dialog was not being
 * destroyed when the color editor was popped down.
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
 * Revision 0.103  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.102  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
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
