
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: encoder.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/*
 *            Save-To-Disk Routines (GIF)
 *
 * GIF and 'Graphics Interchange Format' are trademarks (tm) of Compuserve
 * Incorporated, an H&R Block Company.
 *
 *
 * The following routines perform the GIF encoding when the 's' key is pressed.
 * The routines refer to several variables that are declared elsewhere
 * [colors, xdots, ydots, and 'dacbox'], and rely on external routines to
 * actually read and write screen pixels [getcolor(x,y) and
 *  putcolor(x,y,color)].
 * (Writing pixels is just stuffed in here as a sort of visual status report,
 * and has nothing to do with any GIF function.)    They also rely on the
 * existence of an externally-defined 64K dataspace and they use the routines
 * 'toextra()' and 'cmpextra()' to deal with that dataspace (in the same manner
 * as 'memcpy()' and 'memcmp()' would).   Otherwise, they perform a generic
 * GIF-encoder function.
 *
 * Note that these routines use small string- and hash-tables, and "flush"
 * the GIF entries whenever the hash-table gets two-thirds full or the string
 * table gets full.   They also use the GIF encoding technique of limiting the
 * encoded string length to a specific size, "adding" a string to the hash
 * table at that point even if a matching string exists ("adding" is in
 * quotes, because if a matching string exists we can increment the code
 * counter but safely throw the duplicate string away, saving both string
 * space and a hash table entry).
 *
 * This results in relatively good speed and small data space, but at the
 * expense of compression efficiency (filesize). These trade-offs could be
 * adjusted by modifying the defined variables in the main header file.
 *
 */

/* static functions */
static int shftwrite(unsigned char *color, int numcolors);
static int inittable(void);
static int raster(unsigned int code);
static int extend_blk_len(int datalen);
static int put_extend_blk(int block_id, int block_len, char *block_data);
static int store_item_name(char *nameptr);
static void setup_save_info(struct fractal_info *save_info);

/* static variables */
static int numsaves = 0;    /* For adjusting 'save-to-disk' filenames */

static int lentest, lastentry, numentries, numrealentries;
static unsigned int nextentry;
static int clearcode, endcode;
static unsigned int hashcode;

static unsigned char blockcount;
static int startbits, codebits, bytecount, bitcount;

static char paletteBW[] = {            /* B&W palette */
      0,  0,  0, 63, 63, 63,
      };

static char paletteEGA[] = {            /* 16-color (EGA/CGA) pal */
      0,  0,  0,  0,  0, 42,  0, 42,  0,  0, 42, 42,
     42,  0,  0, 42,  0, 42, 42, 21,  0, 42, 42, 42,
     21, 21, 21, 21, 21, 63, 21, 63, 21, 21, 63, 63,
     63, 21, 21, 63, 21, 63, 63, 63, 21, 63, 63, 63,
     };

char s_cantwrite[]      = {"Can't write %s"};
char s_cantcreate[]     = {"Can't create %s"};

extern int major_method;
extern int minor_method;

int encoder()
{
  int i, ydot, xdot, color, outcolor1, outcolor2;
  int width;
  int rownum, rowlimit;
  unsigned int hashentry;
  unsigned char bitsperpixel, x;
  int entrynum;
  struct fractal_info save_info;
  char msg[128];

  i = ydot = xdot = color = outcolor1 = outcolor2 = 0;

  if(initbatch)            /* flush any impending keystrokes */
    while(keypressed())
      getakey();

  setup_save_info((struct fractal_info *) &save_info);

  if (colors==2)
    {
    bitsperpixel = 1;
    startbits = 3;
    }
  else
    {
    bitsperpixel = 8;
    startbits = 9;
    }

  clearcode = 1 << (startbits - 1);    /* set clear and end codes */
  endcode = clearcode+1;

  outcolor1 = 0;                /* use these colors to show progress */
  outcolor2 = 1;                /* (this has nothing to do with GIF) */
  if (colors > 2)
    {
    outcolor1 = 2;
    outcolor2 = 3;
    }

  if (((++numsaves) & 1) == 0)
    {            /* reverse the colors on alt saves */
    i = outcolor1;
    outcolor1 = outcolor2;
    outcolor2 = i;
    }

  outcolor1s = outcolor1;
  outcolor2s = outcolor2;

  if (gif87a_flag == 1)
    {
    if (fwrite("GIF87a",6,1,out) != 1)
      goto oops;  /* old GIF Signature */
    }
  else
    {
    if (fwrite("GIF89a",6,1,out) != 1)
      goto oops;  /* new GIF Signature */
    }

  width = xdots;
  rowlimit = ydots;

  if (save16bit)
    {
    /*
     * pot16bit info is stored as:
     * file:    double width rows, right side of row is low 8 bits
     * diskvid: ydots rows of colors followed by ydots rows of low 8 bits
     * decoder: returns (row of color info then row of low 8 bits) * ydots
     */
    rowlimit <<= 1;
    width <<= 1;
    }

  if (write2(&width,2,1,out) != 1)
    goto oops;  /* screen descriptor */

  if (write2(&ydots,2,1,out) != 1)
    goto oops;

  x = 128 + ((6-1)<<4) + (bitsperpixel-1);
  /* color resolution == 6 bits worth */

  if (write1(&x,1,1,out) != 1)
    goto oops;

  if (fputc(0,out) != 0)
    goto oops;    /* background color */

  i = 0;

  if (viewwindow                    /* less than full screen?  */
      && (viewxdots == 0 || viewydots == 0))    /* and we picked the dots? */
    i = ((double)sydots / (double)sxdots) * 64.0/screenaspect - 14.5;
  else /* must risk loss of precision if numbers low */
    i = (((double)ydots / (double)xdots) / finalaspectratio) * 64 - 14.5;

  if (i < 1)   i = 1;
  if (i > 255) i = 255;
  if (gif87a_flag) i = 0;    /* for some decoders which can't handle aspect */
  if (fputc(i,out) != i)
    goto oops;    /* pixel aspect ratio */

  if (colors > 2)
    {
    if (gotrealdac)
      {               /* got a DAC - must be a VGA */
      if (!shftwrite((unsigned char *)dacbox,256))
        goto oops;
      }
    else
      {            /* uh oh - better fake it */
      for (i = 0; i < 256; i += 16)
        if (!shftwrite((unsigned char *)paletteEGA,16))
          goto oops;
      }
    }

  if (colors == 2)
    {            /* write out the B&W palette */
    if (!shftwrite((unsigned char *)paletteBW,colors))
      goto oops;
    }

  if (fwrite(",",1,1,out) != 1)
    goto oops;  /* Image Descriptor */

  i = 0;

  if (write2(&i,2,1,out) != 1)
    goto oops;
  if (write2(&i,2,1,out) != 1)
    goto oops;
  if (write2(&width,2,1,out) != 1)
    goto oops;
  if (write2(&ydots,2,1,out) != 1)
    goto oops;
  if (write1(&i,1,1,out) != 1)
    goto oops;

  bitsperpixel = startbits - 1;        /* raster data starts here */

  if (write1(&bitsperpixel,1,1,out) != 1)
    goto oops;

  codebits = startbits;            /* start encoding */

  if (!raster(9999))
    goto oops;        /* initialize the raster routine */

  if (!inittable())
    goto oops;        /* initialize the LZW tables */

  for ( rownum = 0; rownum < ydots; rownum++)
    {  /* scan through the dots */
    for (ydot = rownum; ydot < rowlimit; ydot += ydots)
      {
      for (xdot = 0; xdot < xdots; xdot++)
        {
        if (save16bit == 0 || ydot < ydots)
          color = getcolor(xdot,ydot);
        else
          color = readdisk(xdot+sxoffs,ydot+syoffs);
        teststring[0] = ++lentest;
        teststring[lentest] = color;
        if (lentest == 1)
          {        /* root entry? */
          lastentry = color;
          continue;
          }
        if (lentest == 2)        /* init   the hash code */
          hashcode = 301 * (teststring[1]+1);
        hashcode *= (color + lentest);    /* update the hash code */
        hashentry = ++hashcode % MAXENTRY;
        for( i = 0; i < MAXENTRY; i++)
          {
          if (++hashentry >= MAXENTRY) hashentry = 0;
          if (cmpextra(strlocn[hashentry]+sizeof(int),
             (char *)teststring,lentest+1) == 0)
            break;
          if (strlocn[hashentry] == 0) i = MAXENTRY;
          }
        /* found an entry and string length isn't too bad */
        if (strlocn[hashentry] != 0 && lentest < MAXTEST-1-sizeof(int))
          {
          fromextra(strlocn[hashentry],(char *)&entrynum,sizeof(int));
          lastentry = entrynum;
          continue;
          }
        if (!raster(lastentry))
          goto oops;    /* write entry */
        numentries++;        /* act like you added one, anyway */
        if (strlocn[hashentry] == 0)
          {    /* add new string, if any */
          entrynum = numentries+endcode;
          strlocn[hashentry] = nextentry;
          toextra(nextentry, (char *)&entrynum,sizeof(int));
          toextra(nextentry+sizeof(int),
              (char *)teststring,lentest+1);
          nextentry += lentest+1+sizeof(int);
          numrealentries++;
          }
        teststring[0] = 1;        /* reset current entry */
        teststring[1] = color;
        lentest = 1;
        lastentry = color;

        if ((numentries+endcode) == (1<<codebits))
          codebits++;         /* use longer encoding */

        if ( numentries + endcode > 4093 ||    /* out of room? */
             numrealentries > (MAXENTRY*2)/3 ||
             nextentry > MAXSTRING-MAXTEST-1-2*sizeof(int))
          {
          if (!raster(lastentry))
            goto oops;    /* flush & restart */
          if (!inittable())
            goto oops;
          }
        }
        if(ydot == rownum)
        {
        if ((ydot & 4) == 0)
          {
          if (++outcolor1 >= colors) outcolor1 = 0;
          if (++outcolor2 >= colors) outcolor2 = 0;
          }
        for (i = 0; 250*i < xdots; i++)
          {/* display vert status bars */
           /*   (this is NOT GIF-related)    */
           /* PB Changed following code to xor color, so that
              image can be restored at end and resumed
              putcolor(      i,ydot,outcolor1);
              putcolor(xdots-1-i,ydot,outcolor2);
            */
          putcolor(i,ydot,getcolor(i,ydot)^outcolor1);
          putcolor(xdots-1-i,ydot,getcolor(xdots-1-i,ydot)^outcolor2);
          }
        last_colorbar = ydot;
        }
      if (keypressed())                     /* keyboard hit - bail out */
        ydot = rownum = 9999;
      }
    }

  if (!raster(lastentry))
    goto oops;    /* tidy up - dump the last code */

  if (!raster(endcode))
    goto oops;    /* finish the map */

  if (fputc(0,out) != 0)
    goto oops;    /* raster data ends here */

  if (gif87a_flag == 0)
    { /* store non-standard fractal info */
      /* loadfile.c has notes about extension block structure */

    if (ydot >= 9999)
      save_info.calc_status = IMG_PRMS_CHGD; /* partial save is not resumable */

    save_info.tot_extend_len = 0;

    if (resume_info != NULL && save_info.calc_status == IMG_INTR_RESM)
      {
      update_draw_menu(CONT_CALC);
      /* resume info block, 002 */
      save_info.tot_extend_len += extend_blk_len(resume_len);
      if (!put_extend_blk(2,resume_len,resume_info))goto oops;
      }
    if (save_info.fractal_type == FORMULA || save_info.fractal_type == FFORMULA)
      save_info.tot_extend_len += store_item_name(FormName);
    if (save_info.fractal_type == LSYSTEM)
      save_info.tot_extend_len += store_item_name(LName);
    if (save_info.fractal_type == IFS || save_info.fractal_type == IFS3D)
      save_info.tot_extend_len += store_item_name(IFSName);
    if (display3d <= 0 && rangeslen)
      {
      /* ranges block, 004 */
      save_info.tot_extend_len += extend_blk_len(rangeslen*2);
      fix_ranges(ranges,rangeslen,0);
      put_extend_blk(4,rangeslen*2,(char *)ranges);
      fix_ranges(ranges,rangeslen,0);
      }

    /* main and last block, 001 */
    save_info.tot_extend_len += extend_blk_len(FRACTAL_INFO_SIZE);
    decode_fractal_info(&save_info,0);
    if (!put_extend_blk(1,FRACTAL_INFO_SIZE,(char *)&save_info))
      {
      goto oops;
      }
    }

  if (fwrite(";",1,1,out) != 1)
    goto oops;          /* GIF Terminator */

  return ((ydot < 9999) ? 0 : 1);

oops:
  {
  fflush(out);
  sprintf((char *) &msg, "Error Writing to disk (Disk full?)");
  warning(main_window,(char *) &msg, NULL);
  return 1;
  }
}

/* shift IBM colors to GIF */
static int shftwrite(unsigned char *color,int numcolors)
{
  unsigned char thiscolor;
  int i,j;
  for (i = 0; i < numcolors; i++)
    for (j = 0; j < 3; j++)
      {
      thiscolor = color[3*i+j];
      if (fputc(thiscolor,out) != thiscolor) return(0);
      }
  return(1);
}

static int inittable()         /* routine to init tables */
{
  int i;

  if (!raster(clearcode)) return(0);    /* signal that table is initialized */

  numentries = 0;             /* initialize the table */
  numrealentries = 0;
  nextentry = 1;
  lentest = 0;
  codebits = startbits;

  toextra(0,"\0",1);                      /* clear the hash entries */
  for (i = 0; i < MAXENTRY; i++)
    strlocn[i] = 0;

  return(1);
}

static int raster(unsigned int code)    /* routine to block and output codes */
{
  unsigned int icode, i, j;

  if (code == 9999)
    {            /* special start-up signal */
    bytecount = 0;
    bitcount = 0;
    for (i = 0; i < 266; i++)
        block[i] = 0;
    return(1);
    }

  icode = code << bitcount;        /* update the bit string */
  block[bytecount  ] |= (icode & 255);
  block[bytecount+1] |= ((icode>>8) & 255);
  icode = (code>>8) << bitcount;
  block[bytecount+2] |= ((icode>>8) & 255);
  bitcount += codebits;
  while (bitcount >= 8)
    {         /* locate next starting point */
    bitcount -= 8;
    bytecount++;
    }

  if (bytecount > 250 || code == endcode)
    {    /* time to write a block */
    if (code == endcode)
      while (bitcount > 0)
        {        /* if EOF, find the real end */
        bitcount -= 8;
        bytecount++;
        }
    i = bytecount;
    blockcount = i;
    if (write1(&blockcount,1,1,out) != 1) return(0); /* write the block */
    if (fwrite(block,i,1,out) != 1) return(0);
    bytecount = 0;                /* now re-start the block */
    for (j = 0; j < 5; j++)         /* (may have leftover bits) */
      block[j] = block[j+i];
    for (j = 5; j < 266; j++)
      block[j] = 0;
    }
  return(1);
  }


static int extend_blk_len(int datalen)
  {
  return(datalen + (datalen+254)/255 + 15);
  /*       data   +    1.per.block   + 14 for id + 1 for null at end  */
  }

static int put_extend_blk(int block_id,int block_len,char *block_data)
  {
  int i,j;
  char header[15];
  strcpy(header,"!\377\013fractint");
  sprintf(&header[11],"%03u",block_id);
  if (fwrite(header,14,1,out) != 1) return(0);
  i = (block_len + 254) / 255;
  while (--i >= 0)
    {
    block_len -= (j = min(block_len,255));
    if (fputc(j,out) != j) return(0);
    while (--j >= 0)
      fputc(*(block_data++),out);
    }
  if (fputc(0,out) != 0) return(0);
  return(1);
  }

static int store_item_name(char *nameptr)
  {
  char tmpname[40];
  strcpy(tmpname,nameptr);
  /* formula/lsys/ifs info block, 003 */
  put_extend_blk(3,40,tmpname);
  return(extend_blk_len(40));
}

static void setup_save_info(struct fractal_info *save_info)
{
  int i;

  if(fractype != FORMULA && fractype != FFORMULA)
    maxfn = 0;
  /* set save parameters in save structure */
  strcpy(save_info->info_id, INFO_ID);
  save_info->version          = 9; /* file version, independant of system */
  save_info->iterations      = maxit;
  save_info->fractal_type    = fractype;
  save_info->xmin          = xxmin;
  save_info->xmax          = xxmax;
  save_info->ymin          = yymin;
  save_info->ymax          = yymax;
  save_info->creal          = param[0];
  save_info->cimag          = param[1];
  save_info->videomodeax     = videotable[0].videomodeax;
  save_info->videomodebx     = videotable[0].videomodebx;
  save_info->videomodecx     = videotable[0].videomodecx;
  save_info->videomodedx     = videotable[0].videomodedx;
  save_info->dotmode          = videotable[0].dotmode % 100;
  save_info->xdots          = videotable[0].xdots;
  save_info->ydots          = videotable[0].ydots;
  save_info->colors          = videotable[0].colors;
  save_info->parm3          = 0; /* pre version==7 fields */
  save_info->parm4          = 0;
  save_info->dparm3          = param[2];
  save_info->dparm4          = param[3];
  save_info->dparm5          = param[4];
  save_info->dparm6          = param[5];
  save_info->dparm7          = param[6];
  save_info->dparm8          = param[7];
  save_info->dparm9          = param[8];
  save_info->dparm10         = param[9];
  save_info->fillcolor          = fillcolor;
  save_info->potential[0]    = potparam[0];
  save_info->potential[1]    = potparam[1];
  save_info->potential[2]    = potparam[2];
  save_info->rflag          = rflag;
  save_info->rseed          = rseed;
  save_info->inside          = inside;
  save_info->logmap          = LogFlag;
  save_info->invert[0]       = inversion[0];
  save_info->invert[1]       = inversion[1];
  save_info->invert[2]       = inversion[2];
  save_info->decomp[0]       = decomp[0];
  save_info->biomorph          = usr_biomorph;
  save_info->symmetry          = (short)forcesymmetry;
  for (i = 0; i < 16; i++)
    save_info->init3d[i] = init3d[i];
  save_info->previewfactor   = previewfactor;
  save_info->xtrans          = xtrans;
  save_info->ytrans          = ytrans;
  save_info->red_crop_left   = red_crop_left;
  save_info->red_crop_right  = red_crop_right;
  save_info->blue_crop_left  = blue_crop_left;
  save_info->blue_crop_right = blue_crop_right;
  save_info->red_bright      = red_bright;
  save_info->blue_bright     = blue_bright;
  save_info->xadjust          = xadjust;
  save_info->eyeseparation   = eyeseparation;
  save_info->glassestype     = glassestype;
  save_info->outside          = outside;
  save_info->x3rd          = xx3rd;
  save_info->y3rd          = yy3rd;
  save_info->calc_status     = calc_status;
  save_info->stdcalcmode     = stdcalcmode;
  save_info->distest          = distest;
  save_info->floatflag       = floatflag;
  save_info->bailout          = bailout;
  save_info->calctime          = calctime;
  save_info->trigndx[0]      = trigndx[0];
  save_info->trigndx[1]      = trigndx[1];
  save_info->trigndx[2]      = trigndx[2];
  save_info->trigndx[3]      = trigndx[3];
  save_info->finattract      = finattract;
  save_info->initorbit[0]    = initorbit.x;
  save_info->initorbit[1]    = initorbit.y;
  save_info->useinitorbit    = useinitorbit;
  save_info->periodicity     = periodicitycheck;
  save_info->pot16bit          = disk16bit;
  save_info->faspectratio    = finalaspectratio;
  save_info->system          = save_system;
  save_info->release          = save_release;
  save_info->flag3d          = display3d;
  save_info->ambient          = Ambient;
  save_info->randomize       = RANDOMIZE;
  save_info->haze          = haze;
  save_info->transparent[0]  = transparent[0];
  save_info->transparent[1]  = transparent[1];
  save_info->rotate_lo       = rotate_lo;
  save_info->rotate_hi       = rotate_hi;
  save_info->distestwidth    = distestwidth;
  save_info->mxmaxfp         = mxmaxfp;
  save_info->mxminfp         = mxminfp;
  save_info->mymaxfp         = mymaxfp;
  save_info->myminfp         = myminfp;
  save_info->zdots           = zdots;
  save_info->originfp        = originfp;
  save_info->depthfp         = depthfp;
  save_info->heightfp        = heightfp;
  save_info->widthfp         = widthfp;
  save_info->distfp          = distfp;
  save_info->eyesfp          = eyesfp;
  save_info->orbittype       = neworbittype;
  save_info->juli3Dmode      = juli3Dmode;
  save_info->maxfn           = maxfn;
  save_info->inversejulia    = (major_method << 8) + minor_method; /* MVS */
  for (i = 0; i < sizeof(save_info->future)/sizeof(short); i++)
    save_info->future[i] = 0;
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: encoder.c,v $
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
 * Revision 0.104  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.103  1994/11/29  07:30:29  darryl
 * Make the 'Create Image' button label dynamic. Depending on
 * the value of calc_status, it reads 'Create Image' or
 * 'Continue calculation'. The accelerator remains the same.
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
