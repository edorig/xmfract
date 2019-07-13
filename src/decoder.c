
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: decoder.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static short get_next_code(void);

/* static variables */
static short curr_size; /* The current code size */
static short sizeofstring[MAX_CODES+1]; /* size of string list */

/*
 * used for seperating out codes
 */

static short navail_bytes;           /* # bytes left in block */
static short nbits_left;             /* # bits left in current byte */
static unsigned char byte_buff[257]; /* Current block, reuse shared mem */
static unsigned char *pbytes;        /* Pointer to next byte in block */

static short code_mask[13] =
  {
  0,
  0x0001, 0x0003,
  0x0007, 0x000F,
  0x001F, 0x003F,
  0x007F, 0x00FF,
  0x01FF, 0x03FF,
  0x07FF, 0x0FFF
  };

short decoder( short linewidth)
{
  unsigned char *sp;
  short code;
  short old_code;
  short ret;
  short c;
  short size;
  short i;
  short j;
  short fastloop;
  short bufcnt;            /* how many empty spaces left in buffer */
  short xskip;
  short slot;              /* Last read code */
  short newcodes;          /* First available code */
  unsigned char *bufptr;
  short yskip;
  short top_slot;          /* Highest code for current size */
  short clear;             /* Value for a clear code */
  short ending;            /* Value for a ending code */
  unsigned char out_value;

  /*
   * Initialize for decoding a new image...
   */

  if ((size = get_byte()) < 0)
    return(size);
  if (size < 2 || 9 < size)
    return(BAD_CODE_SIZE);

  curr_size = size + 1;
  top_slot = 1 << curr_size;
  clear = 1 << size;
  ending = clear + 1;
  slot = newcodes = ending + 1;
  navail_bytes = nbits_left = sizeofstring[slot] =
    xskip = yskip = old_code = 0;
  out_value = 0;
  for (i = 0; i < slot; i++)
    sizeofstring[i] = 0;

  /*
   * Initialize in case they forgot to put in a clear code.
   * (This shouldn't happen, but we'll try and decode it anyway...)
   */

  /*
   * Set up the stack pointer and decode buffer pointer
   */

  sp = dcstack;
  bufptr = dcdecoderline;
  bufcnt = linewidth;

  /*
   * This is the main loop.  For each code we get we pass through the
   * linked list of prefix codes, pushing the corresponding "character" for
   * each code onto the stack.  When the list reaches a single "character"
   * we push that on the stack too, and then start unstacking each
   * character for output in the correct order.  Special handling is
   * included for the clear code, and the whole thing ends when we get
   * an ending code.
   */

  while ((c = get_next_code()) != ending)
    {
    /*
     * If we had a file error, return without completing the decode
     */
    if (c < 0)
      return(0);

    /*
     * If the code is a clear code, reinitialize all necessary items.
     */

    if (c == clear)
      {
      curr_size = size + 1;
      slot = newcodes;
      sizeofstring[slot] = 0;
      top_slot = 1 << curr_size;

      /*
       * Continue reading codes until we get a non-clear code
       * (Another unlikely, but possible case...)
       */

      while ((c = get_next_code()) == clear)
        ;

      /*
       * If we get an ending code immediately after a clear code
       * (Yet another unlikely case), then break out of the loop.
       */

      if (c == ending)
        break;

      /*
       * Finally, if the code is beyond the range of already set codes,
       * (This one had better NOT happen...    I have no idea what will
       * result from this, but I doubt it will look good...) then set it
       * to color zero.
       */

      if (c >= slot)
        c = 0;

      old_code = out_value = c;

      /* And let us not forget to put the char into the buffer... */

      *sp++ = c;
      }
    else
      {
      /*
       * In this case, it's not a clear code or an ending code, so
       * it must be a code code...  So we can now decode the code into
       * a stack of character codes. (Clear as mud, right?)
       */

      code = c;

      /*
       * Here we go again with one of those off chances...  If, on the
       * off chance, the code we got is beyond the range of those already
       * set up (Another thing which had better NOT happen...) we trick
       * the decoder into thinking it actually got the next slot avail.
       */

      if (code >= slot)
        {
        if (code > slot)
          {
          ++bad_code_count;
          c = slot;
          }
        code = old_code;
        *sp++ = out_value;
        }

      /*
       * Here we scan back along the linked list of prefixes.  If they can
       * fit into the output buffer then transfer them direct.  ELSE push
       * them into the stack until we are down to enough characters that
       * they do fit.  Output the line then fall through to unstack the ones
       * that would not fit.
       */

      fastloop = NOPE;
      while (code >= newcodes)
        {
        j = i = sizeofstring[code];
        if (i > 0 && bufcnt - i > 0 && skipxdots == 0)
          {
          fastloop = YUP;

          do
            {
            *(bufptr + j) = dcsuffix[code];
            code = (short)dcprefix[code];
            } while(--j > 0);

          *bufptr = (unsigned char)code;
          bufptr += ++i;
          bufcnt -= i;
          if (bufcnt == 0) /* finished an input row? */
            {
            if (--yskip < 0)
              {
              if ((ret = (*outln)(dcdecoderline, bufptr - dcdecoderline)) < 0)
                return(ret);
              yskip = skipydots;
              }
            if (keypressed())
              return(-1);
            bufptr = dcdecoderline;
            bufcnt = linewidth;
            xskip = 0;
            }
          }
        else
          {
          *sp++ = dcsuffix[code];
          code = (short)dcprefix[code];
          }
        }

      /*
       * Push the last character on the stack, and set up the new
       * prefix and suffix, and if the required slot number is greater
       * than that allowed by the current bit size, increase the bit
       * size.  (NOTE - If we are all full, we *don't* save the new
       * suffix and prefix...  I'm not certain if this is correct...
       * it might be more proper to overwrite the last code...
       */

      if (fastloop == NOPE)
        *sp++ = (unsigned char)code;

      if (slot < top_slot)
        {
        sizeofstring[slot] = sizeofstring[old_code]+1;
        dcsuffix[slot] = out_value = (unsigned char)code;
        dcprefix[slot++] = old_code;
        old_code = c;
        }
      if (slot >= top_slot)
        if (curr_size < 12)
          {
          top_slot <<= 1;
          ++curr_size;
          }
      }
    while (sp > dcstack)
      {
      --sp;
      if (--xskip < 0)
        {
        xskip = skipxdots;
        *bufptr++ = *sp;
        }
      if (--bufcnt == 0) /* finished an input row? */
        {
        if (--yskip < 0)
          {
          if ((ret = (*outln)(dcdecoderline, bufptr - dcdecoderline)) < 0)
            return(ret);
          yskip = skipydots;
          }
        if (keypressed())
          return(-1);
        bufptr = dcdecoderline;
        bufcnt = linewidth;
        xskip = 0;
        }
      }
    }

  /*
   * PB note that if last line is incomplete, we're not going to try
   * to emit it;  original code did, but did so via out_line and therefore
   * couldn't have worked well in all cases...
   */

  return(0);
}


/*
 *  get_next_code()
 * - gets the next code from the GIF file.  Returns the code, or else
 * a negative number in case of file errors...
 */

static short get_next_code()
{
  static unsigned char b1;                /* Current byte */
  static unsigned short ret_code;

  if (nbits_left == 0)
    {
    if (navail_bytes <= 0)
      {
      /*
       * Out of bytes in current block, so read next block
       */
      pbytes = byte_buff;
      if ((navail_bytes = get_byte()) < 0)
        return(navail_bytes);
      else
        if (navail_bytes)
          get_bytes(byte_buff,navail_bytes);
      }
    b1 = *pbytes++;
    nbits_left = 8;
    --navail_bytes;
    }

  ret_code = b1 >> (8 - nbits_left);
  while (curr_size > nbits_left)
    {
    if (navail_bytes <= 0)
      {
      /*
       * Out of bytes in current block, so read next block
       */
      pbytes = byte_buff;
      if ((navail_bytes = get_byte()) < 0)
        return(navail_bytes);
      else
        if (navail_bytes)
          get_bytes(byte_buff,navail_bytes);
      }
    b1 = *pbytes++;
    ret_code |= b1 << nbits_left;
    nbits_left += 8;
    --navail_bytes;
    }
  nbits_left -= curr_size;
  return(ret_code & code_mask[curr_size]);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: decoder.c,v $
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
