
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: rubber_band.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* static functions */
static void update_zoom_handles(void);
static void draw_zoom_rect(Position x, Position y, Position x2, Position y2);
static void check_points(Position *x, Position *y, Position *x2, Position *y2);
static void snap_rect_to_aspect(int vertical);
static void draw_stretch_marks(void);
static int ptr_in_box(int x, int y);
static int ptr_in_handle(int x, int y, zoombox_dir_e *which);

/* static variables */
static int mousedown = 0;

/*ARGSUSED*/
void rubber_band(
  Widget w,
  XEvent *event,
  String *args,
  int *argcnt)
{
  if(!((XButtonEvent *)event))
    return;

  if((zoomoff == 1) &&
     (!(editpal_cursor)) &&
     (!(curfractalspecific->flags & NOZOOM)))
    {
    if(verbose)
      {
      int ret;
      char msg[256];
      sprintf((char *) &msg, "Parameters for this image have changed since\nit was created. Xmfract cannot zoom this image\nuntil it is at least partially recalculated.");
      ret = confirm(main_window, (char *) &msg,
        OK_BTN, XmDIALOG_OK_BUTTON);
      }
    return;
    }
  else if(((int)strcmp((char *) &ZoomName, "GIFFILE")) == 0)
    {
    int ret;
    char msg[256];
    sprintf((char *) &msg, "The file \"%s\" has\nno fractal info, and cannot be zoomed.", readname);
    ret = confirm(main_window, (char *) &msg,
      OK_BTN, XmDIALOG_OK_BUTTON);
    return;
    }
  else if((curfractalspecific->flags & NOZOOM))
    {
    int ret;
    char msg[256];
    sprintf((char *) &msg, "The %s fractal type cannot be zoomed.", curfractalspecific->name);
    ret = confirm(main_window, (char *) &msg,
      OK_BTN, XmDIALOG_OK_BUTTON);
    return;
    }
  else if(editpal_cursor == 1)
    {
    return;
    }

  if(((int)strcmp(args[0], "dn") == 0) &&
     (((XButtonEvent *)event)->button == Button1) && !mousedown)
    {
    zoombox_dir_e which_handle;

    if(viewwindow)
      {
      if((event->xbutton.x < sxoffs) ||
         (event->xbutton.x > (sxoffs + xdots - 1)) ||
         (event->xbutton.y < syoffs) ||
         (event->xbutton.y > (syoffs + ydots - 1)))
        {
        return;
        }
      }

    rubber_band_data.last_x = rubber_band_data.start_x = event->xbutton.x;
    rubber_band_data.last_y = rubber_band_data.start_y = event->xbutton.y;

    /*
     * if the pointer is in one of the handles, assume we want to stretch.
     * else if the pointer is in the box, assume we want to pan.
     * if neither, then erase any old box, and start drawing a new box.
     */

    if(rubber_band_data.boxisactive == 1)
      {
      if(ptr_in_box
        (rubber_band_data.start_x,
         rubber_band_data.start_y))
        {
        if(rubber_band_data.panning == 0 && ptr_in_handle
          (rubber_band_data.start_x,
           rubber_band_data.start_y,
           &which_handle))
          {
          rubber_band_data.moving = 0;
          rubber_band_data.panning = 0;
          rubber_band_data.stretching = 1;
          }
        else
          {
          if(rubber_band_data.panning == 0)
            rubber_band_data.moving = 1;
          else
            rubber_band_data.moving = 0;
          rubber_band_data.stretching = 0;
          }
        }
      }
    else
      {
      change_xor_gc(1);
      rubber_band_data.moving = 0;
      rubber_band_data.panning = 0;
      rubber_band_data.stretching = 0;
      }

    if(rubber_band_data.stretching == 1)
      {
      switch(which_handle)
        {
        case NorthWest:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_top_left_corner));
          rubber_band_data.stretch_dir = NorthWest;
        break;

        case North:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_top_side));
          rubber_band_data.stretch_dir = North;
        break;

        case NorthEast:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_top_right_corner));
          rubber_band_data.stretch_dir = NorthEast;
        break;

        case East:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_right_side));
          rubber_band_data.stretch_dir = East;
        break;

        case SouthEast:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_bottom_right_corner));
          rubber_band_data.stretch_dir = SouthEast;
        break;

        case South:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_bottom_side));
          rubber_band_data.stretch_dir = South;
        break;

        case SouthWest:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_bottom_left_corner));
          rubber_band_data.stretch_dir = SouthWest;
        break;

        case West:
          XDefineCursor(display, window,
            XCreateFontCursor(display, XC_left_side));
          rubber_band_data.stretch_dir = West;
        break;

        case Center:
        break;
        }
      change_xor_gc(2);

      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);

      if(rubber_band_data.panning == 0)
        {
        update_zoom_handles();
        draw_stretch_marks(); /* :-) */
        }

      change_xor_gc(1);

      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);
      }
    else if(rubber_band_data.moving == 1 || rubber_band_data.panning == 1)
      {
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_fleur));
      change_xor_gc(2);

      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);

      if(rubber_band_data.panning == 0)
        {
        update_zoom_handles();
        draw_stretch_marks(); /* :-) */
        }

      change_xor_gc(1);

      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);
      }
    else
      {
      if(rubber_band_data.boxisactive == 1)
        {
        change_xor_gc(2);
        draw_zoom_rect
          (rubber_band_data.tl_x, rubber_band_data.tl_y,
           rubber_band_data.br_x, rubber_band_data.br_y);
        if(rubber_band_data.panning == 0)
          {
          update_zoom_handles();
          draw_stretch_marks();
          }
        }
      change_xor_gc(1);

      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_crosshair));
      }
    mousedown = 1;
    return;
    }
  else if((((int)strcmp(args[0], "mv")) == 0) && (mousedown))
    {
    if(rubber_band_data.moving == 1 || rubber_band_data.panning == 1)
      {
      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);

      if(rubber_band_data.last_x >= event->xbutton.x)
        {
        /* going west */
        rubber_band_data.tl_x -=
          rubber_band_data.last_x - event->xbutton.x;
        rubber_band_data.br_x -=
          rubber_band_data.last_x - event->xbutton.x;
        }
      else if(rubber_band_data.last_x < event->xbutton.x)
        {
        /* going east */
        rubber_band_data.tl_x +=
          event->xbutton.x - rubber_band_data.last_x;
        rubber_band_data.br_x +=
          event->xbutton.x - rubber_band_data.last_x;
        }
      if(rubber_band_data.last_y >= event->xbutton.y)
        {
        /* going up */
        rubber_band_data.tl_y -=
          rubber_band_data.last_y - event->xbutton.y;
        rubber_band_data.br_y -=
          rubber_band_data.last_y - event->xbutton.y;
        }
      else if(rubber_band_data.last_y < event->xbutton.y)
        {
        /* going down */
        rubber_band_data.tl_y +=
          event->xbutton.y - rubber_band_data.last_y;
        rubber_band_data.br_y +=
          event->xbutton.y - rubber_band_data.last_y;
        }

      rubber_band_data.last_x = event->xbutton.x;
      rubber_band_data.last_y = event->xbutton.y;

      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);
      }
    else if(rubber_band_data.stretching == 1)
      {
      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);

      switch(rubber_band_data.stretch_dir)
        {
        case NorthWest:
          rubber_band_data.tl_x +=
            (event->xbutton.x - rubber_band_data.last_x);
          rubber_band_data.tl_y +=
            (event->xbutton.y - rubber_band_data.last_y);
          rubber_band_data.last_x = event->xbutton.x;
          rubber_band_data.last_y = event->xbutton.y;
        break;

        case North:
          rubber_band_data.tl_y +=
            (event->xbutton.y - rubber_band_data.last_y);
          rubber_band_data.last_y = event->xbutton.y;
        break;

        case NorthEast:
          rubber_band_data.br_x +=
            (event->xbutton.x - rubber_band_data.last_x);
          rubber_band_data.tl_y +=
            (event->xbutton.y - rubber_band_data.last_y);
          rubber_band_data.last_x = event->xbutton.x;
          rubber_band_data.last_y = event->xbutton.y;
        break;

        case East:
          rubber_band_data.br_x +=
            (event->xbutton.x - rubber_band_data.last_x);
          rubber_band_data.last_x = event->xbutton.x;
        break;

        case SouthEast:
          rubber_band_data.br_x +=
            (event->xbutton.x - rubber_band_data.last_x);
          rubber_band_data.br_y +=
              (event->xbutton.y - rubber_band_data.last_y);
          rubber_band_data.last_x = event->xbutton.x;
          rubber_band_data.last_y = event->xbutton.y;
        break;

        case South:
          rubber_band_data.br_y +=
            (event->xbutton.y - rubber_band_data.last_y);
          rubber_band_data.last_y = event->xbutton.y;
        break;

        case SouthWest:
          rubber_band_data.tl_x +=
              (event->xbutton.x - rubber_band_data.last_x);
          rubber_band_data.br_y +=
            (event->xbutton.y - rubber_band_data.last_y);
          rubber_band_data.last_x = event->xbutton.x;
          rubber_band_data.last_y = event->xbutton.y;
        break;

        case West:
          rubber_band_data.tl_x +=
            (event->xbutton.x - rubber_band_data.last_x);
          rubber_band_data.last_x = event->xbutton.x;
        break;

        case Center:
        break;
        }

      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);
      }
    else
      {
      draw_zoom_rect
        (rubber_band_data.start_x, rubber_band_data.start_y,
         rubber_band_data.last_x, rubber_band_data.last_y);

      rubber_band_data.last_x = event->xbutton.x;
      rubber_band_data.last_y = event->xbutton.y;

      draw_zoom_rect
        (rubber_band_data.start_x, rubber_band_data.start_y,
         rubber_band_data.last_x, rubber_band_data.last_y);
      }
    return;
    }
  else if (((int)strcmp(args[0], "up") == 0) &&
           (((XButtonEvent *)event)->button == Button1) && mousedown)
    {
    rubber_band_data.last_x = event->xbutton.x;
    rubber_band_data.last_y = event->xbutton.y;

    if(rubber_band_data.moving == 1 ||
       rubber_band_data.stretching == 1 ||
       rubber_band_data.panning == 1)
      {
      change_xor_gc(1);

      check_points
        (&(rubber_band_data.tl_x),
         &(rubber_band_data.tl_y),
         &(rubber_band_data.br_x),
         &(rubber_band_data.br_y));

      draw_zoom_rect
        (rubber_band_data.tl_x, rubber_band_data.tl_y,
         rubber_band_data.br_x, rubber_band_data.br_y);

      }
    else
      {
      check_points
        (&(rubber_band_data.start_x),
         &(rubber_band_data.start_y),
         &(rubber_band_data.last_x),
         &(rubber_band_data.last_y));

      draw_zoom_rect
        (rubber_band_data.start_x, rubber_band_data.start_y,
         rubber_band_data.last_x, rubber_band_data.last_y);

      rubber_band_data.tl_x = rubber_band_data.start_x;
      rubber_band_data.tl_y = rubber_band_data.start_y;
      rubber_band_data.br_x = rubber_band_data.last_x;
      rubber_band_data.br_y = rubber_band_data.last_y;
      }

    rubber_band_data.width =
      rubber_band_data.br_x - rubber_band_data.tl_x;

    rubber_band_data.height =
      rubber_band_data.br_y - rubber_band_data.tl_y;

    /* minimum zoombox size is 25 x 25 */

    if((abs(rubber_band_data.width)) < 25 ||
       (abs(rubber_band_data.height)) < 25)
      {
      rubber_band_data.moving = 0;
      rubber_band_data.panning = 0;
      rubber_band_data.stretching = 0;
      rubber_band_data.boxisactive = 0;
      update_zoom_menu();
      XDefineCursor(display, window,
        XCreateFontCursor(display, XC_top_left_arrow));
      mousedown = 0;
      return;
      }

    if(rubber_band_data.snap_to_aspect)
      {
      /*
       * snap the lower right corner of the zoombox to
       * coordinates that make the box represent the
       * same aspect ratio as the parent window. Use
       * the longest of the horizontal or vertical sides
       * as the preferred minimum size. If they are equal,
       * use horizontal as the preferred minimum size.
       */
      if(rubber_band_data.width >= rubber_band_data.height)
        {
        snap_rect_to_aspect(0); /* horizontal preferred */
        }
      else if(rubber_band_data.width < rubber_band_data.height)
        {
        snap_rect_to_aspect(1); /* vertical preferred */
        }
      }

    change_xor_gc(2);

    draw_zoom_rect
      (rubber_band_data.tl_x, rubber_band_data.tl_y,
       rubber_band_data.br_x, rubber_band_data.br_y);

    if(rubber_band_data.panning == 0)
      {
      update_zoom_handles();
      draw_stretch_marks(); /* :-) */
      }

    XDefineCursor(display, window,
      XCreateFontCursor(display, XC_top_left_arrow));

    change_xor_gc(1);

    rubber_band_data.moving = 0;
    rubber_band_data.stretching = 0;
    rubber_band_data.boxisactive = 1;

    update_zoom_menu();

    mousedown = 0;

    return;
    }
  else
    return;
}

/*ARGSUSED*/
static void update_zoom_handles()
{
  int v_margin, h_margin;
  int min_margin = 5;
  int max_margin = 15;

  v_margin = rubber_band_data.height / 5;

  if(v_margin < min_margin)
    {
    rubber_band_data.h_tl_y = rubber_band_data.tl_y + min_margin;
    rubber_band_data.h_tr_y = rubber_band_data.tl_y + min_margin;
    rubber_band_data.h_bl_y = rubber_band_data.br_y - min_margin;
    rubber_band_data.h_br_y = rubber_band_data.br_y - min_margin;
    }
  else if(v_margin > max_margin)
    {
    rubber_band_data.h_tl_y = rubber_band_data.tl_y + max_margin;
    rubber_band_data.h_tr_y = rubber_band_data.tl_y + max_margin;
    rubber_band_data.h_bl_y = rubber_band_data.br_y - max_margin;
    rubber_band_data.h_br_y = rubber_band_data.br_y - max_margin;
    }
  else
    {
    rubber_band_data.h_tl_y = rubber_band_data.tl_y + v_margin;
    rubber_band_data.h_tr_y = rubber_band_data.tl_y + v_margin;
    rubber_band_data.h_bl_y = rubber_band_data.br_y - v_margin;
    rubber_band_data.h_br_y = rubber_band_data.br_y - v_margin;
    }

  rubber_band_data.h_tl_x = rubber_band_data.tl_x;
  rubber_band_data.h_tr_x = rubber_band_data.br_x;
  rubber_band_data.h_bl_x = rubber_band_data.tl_x;
  rubber_band_data.h_br_x = rubber_band_data.br_x;

  h_margin = rubber_band_data.width / 5;

  if(h_margin < min_margin)
    {
    rubber_band_data.v_tl_x = rubber_band_data.tl_x + min_margin;
    rubber_band_data.v_tr_x = rubber_band_data.br_x - min_margin;
    rubber_band_data.v_bl_x = rubber_band_data.tl_x + min_margin;
    rubber_band_data.v_br_x = rubber_band_data.br_x - min_margin;
    }
  else if(h_margin > max_margin)
    {
    rubber_band_data.v_tl_x = rubber_band_data.tl_x + max_margin;
    rubber_band_data.v_tr_x = rubber_band_data.br_x - max_margin;
    rubber_band_data.v_bl_x = rubber_band_data.tl_x + max_margin;
    rubber_band_data.v_br_x = rubber_band_data.br_x - max_margin;
    }
  else
    {
    rubber_band_data.v_tl_x = rubber_band_data.tl_x + h_margin;
    rubber_band_data.v_tr_x = rubber_band_data.br_x - h_margin;
    rubber_band_data.v_bl_x = rubber_band_data.tl_x + h_margin;
    rubber_band_data.v_br_x = rubber_band_data.br_x - h_margin;
    }

  rubber_band_data.v_tl_y = rubber_band_data.tl_y;
  rubber_band_data.v_tr_y = rubber_band_data.tl_y;
  rubber_band_data.v_bl_y = rubber_band_data.br_y;
  rubber_band_data.v_br_y = rubber_band_data.br_y;
}

/*ARGSUSED*/
static void draw_zoom_rect(
  Position x,
  Position y,
  Position x2,
  Position y2)
{
  check_points(&x, &y, &x2, &y2);
  XDrawRectangle(display, XtWindow(canvas), xgc,  x, y, x2 - x, y2 - y);
}

/*ARGSUSED*/
static void check_points(
  Position *x,
  Position *y,
  Position *x2,
  Position *y2)
{
  if(*x2 < *x){ Position tmp = *x; *x = *x2; *x2 = tmp;}
  if(*y2 < *y){ Position tmp = *y; *y = *y2; *y2 = tmp;}
}

/*ARGSUSED*/
static void snap_rect_to_aspect(int vertical)
{
  if(vertical)
    {
    rubber_band_data.br_x = rubber_band_data.tl_x +
      (int)(rubber_band_data.height / finalaspectratio);
    rubber_band_data.last_x = rubber_band_data.br_x;
    rubber_band_data.last_y = rubber_band_data.br_y;
    }
  else
    {
    rubber_band_data.br_y = rubber_band_data.tl_y +
      (int)(rubber_band_data.width * finalaspectratio);
    rubber_band_data.last_x = rubber_band_data.br_x;
    rubber_band_data.last_y = rubber_band_data.br_y;
    }

  rubber_band_data.width =
    rubber_band_data.br_x - rubber_band_data.tl_x;

  rubber_band_data.height =
    rubber_band_data.br_y - rubber_band_data.tl_y;
}

/*ARGSUSED*/
void zoom_cancel_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  erase_current_zoombox();
  rubber_band_data.snap_to_aspect = 1;
  rubber_band_data.boxisactive = 0;
  rubber_band_data.moving = 0;
  rubber_band_data.panning = 0;
  rubber_band_data.stretching = 0;
  update_zoom_menu();
  XPutImage(display, XtWindow(canvas), gc, image, 0, 0, 0, 0, sxdots, sydots);
}

/*ARGSUSED*/
void erase_current_zoombox()
{
  /*
   * Draw once to clear the previous box.
   */

  if(rubber_band_data.boxisactive == 1)
    {
    change_xor_gc(2);

    draw_zoom_rect
      (rubber_band_data.tl_x, rubber_band_data.tl_y,
       rubber_band_data.br_x, rubber_band_data.br_y);

    if(rubber_band_data.panning == 0)
      {
      update_zoom_handles();
      draw_stretch_marks();
      }
    }

  rubber_band_data.boxisactive = 0;

  change_xor_gc(1);
}

/*ARGSUSED*/
static void draw_stretch_marks()
{
  /*
   * Draw four lines, 2 horizontal and 2 vertical,
   * indicating a margin in which the pointer must
   * be placed to stretch the box in the desired
   * direction.
   */

  /* top horizontal line */
  XDrawLine(display, XtWindow(canvas), xgc,
    rubber_band_data.h_tl_x, rubber_band_data.h_tl_y,
    rubber_band_data.h_tr_x, rubber_band_data.h_tr_y);

  /* bottom horizontal line */
  XDrawLine(display, XtWindow(canvas), xgc,
    rubber_band_data.h_bl_x, rubber_band_data.h_bl_y,
    rubber_band_data.h_br_x, rubber_band_data.h_br_y);

  /* left vertical line */
  XDrawLine(display, XtWindow(canvas), xgc,
    rubber_band_data.v_tl_x, rubber_band_data.v_tl_y,
    rubber_band_data.v_bl_x, rubber_band_data.v_bl_y);

  /* right vertical line */
  XDrawLine(display, XtWindow(canvas), xgc,
    rubber_band_data.v_tr_x, rubber_band_data.v_tr_y,
    rubber_band_data.v_br_x, rubber_band_data.v_br_y);
}

static int ptr_in_box(int x, int y)
{
  if( x < rubber_band_data.tl_x ||
      x > rubber_band_data.br_x ||
      y < rubber_band_data.tl_y ||
      y > rubber_band_data.br_y ||
      rubber_band_data.stretching == 1)
    {
    return 0;
    }
  else
    return 1;
}

static int ptr_in_handle(int x, int y, zoombox_dir_e *which)
{
  /*
   * handle boxes are numbered from 1 to 8
   * clockwise from the top left handle
   */

  if( y < rubber_band_data.h_tl_y )
    {
    /* ptr is in top row of handles */
    if( x <= rubber_band_data.v_tl_x )
      {
      /* ptr is in box 1 */
      *which = NorthWest;
      return 1;
      }
    else if( x <= rubber_band_data.v_tr_x )
      {
      /* ptr is in box 2 */
      *which = North;
      return 1;
      }
    else
      {
      /* ptr must be in box 3 */
      *which = NorthEast;
      return 1;
      }
    }
  else if( y < rubber_band_data.h_bl_y )
    {
    /* ptr is in middle row of boxes */
    if( x <= rubber_band_data.v_tl_x )
      {
      /* ptr is in box 8 */
      *which = SouthWest;
      return 1;
      }
    else if( x <= rubber_band_data.v_tr_x )
      {
      /* ptr is in center, not in handle */
      *which = Center;
      return 0;
      }
    else
      {
      /* ptr must be in box 4 */
      *which = East;
      return 1;
      }
    }
  else if( y < rubber_band_data.br_y )
    {
    /* ptr is in bottom row of boxes */
    if( x <= rubber_band_data.v_tl_x )
      {
      /* ptr is in box 7 */
      *which = SouthWest;
      return 1;
      }
    else if( x <= rubber_band_data.v_tr_x )
      {
      /* ptr is in box 6 */
      *which = South;
      return 1;
      }
    else
      {
      /* ptr must be in box 5 */
      *which = SouthEast;
      return 1;
      }
    }
  return 0;
}

/*ARGSUSED*/
void zoom_in_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  erase_current_zoombox();
  rubber_band_data.moving = 0;
  rubber_band_data.panning = 0;
  rubber_band_data.stretching = 0;
  zrotate = 0;
  zskew = 0;
  rubber_band_data.boxisactive = 0;
  rubber_band_data.snap_to_aspect = 1;
  zbx = (MIN(rubber_band_data.tl_x,rubber_band_data.br_x)-sxoffs)/dx_size;
  zby = (MIN(rubber_band_data.tl_y,rubber_band_data.br_y)-syoffs)/dy_size;
  zwidth = ABS(rubber_band_data.br_x-rubber_band_data.tl_x)/dx_size;
  zdepth = zwidth;
  drawbox();
  init_pan_or_recalc(0);
  create_image_cb(main_window, NULL, NULL);
  update_zoom_menu();
}

/*ARGSUSED*/
void zoom_out_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  rubber_band_data.moving = 0;
  rubber_band_data.panning = 0;
  rubber_band_data.stretching = 0;
  erase_current_zoombox();
  zrotate = 0;
  zskew = 0;
  rubber_band_data.boxisactive = 0;
  rubber_band_data.snap_to_aspect = 1;
  zbx = (MIN(rubber_band_data.tl_x,rubber_band_data.br_x)-sxoffs)/dx_size;
  zby = (MIN(rubber_band_data.tl_y,rubber_band_data.br_y)-syoffs)/dy_size;
  zwidth = ABS(rubber_band_data.br_x-rubber_band_data.tl_x)/dx_size;
  zdepth = zwidth;
  drawbox();
  init_pan_or_recalc(1);
  zoomout();
  create_image_cb(main_window, NULL, NULL);
  update_zoom_menu();
}

/*ARGSUSED*/
void zoom_aspect_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  if(rubber_band_data.snap_to_aspect == 1)
    rubber_band_data.snap_to_aspect = 0;
  else
    rubber_band_data.snap_to_aspect = 1;

  update_zoom_menu();
}

/*ARGSUSED*/
void zoom_pan_cb(
Widget w,
XtPointer client_data,
XtPointer call_data)
{
  erase_current_zoombox();
  rubber_band_data.moving = 0;
  rubber_band_data.panning = 1;
  rubber_band_data.stretching = 0;
  rubber_band_data.boxisactive = 1;
  rubber_band_data.snap_to_aspect = 0;
  update_zoom_menu();
  change_xor_gc(2);

  if(viewwindow)
    {
    rubber_band_data.tl_x = rubber_band_data.start_x = sxoffs;
    rubber_band_data.tl_y = rubber_band_data.start_y = syoffs;
    rubber_band_data.br_x = rubber_band_data.last_x = sxoffs + xdots - 1;
    rubber_band_data.br_y = rubber_band_data.last_y = syoffs + ydots - 1;
    }
  else
    {
    rubber_band_data.tl_x = rubber_band_data.start_x = 0;
    rubber_band_data.tl_y = rubber_band_data.start_y = 0;
    rubber_band_data.br_x = rubber_band_data.last_x = xdots - 1;
    rubber_band_data.br_y = rubber_band_data.last_y = ydots - 1;
    }

  rubber_band_data.width =
    rubber_band_data.br_x - rubber_band_data.tl_x;

  rubber_band_data.height =
    rubber_band_data.br_y - rubber_band_data.tl_y;

  draw_zoom_rect
    (rubber_band_data.tl_x, rubber_band_data.tl_y,
     rubber_band_data.br_x, rubber_band_data.br_y);
}

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: rubber_band.c,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.13  1995/06/15  22:34:35  darryl
 * Put quotes around file names in messages.
 *
 * Revision 1.12  1995/06/15  15:36:25  darryl
 * Enhance zoom error message when attempting to zoom after
 * reading a GIF that does not contain fractal info.
 *
 * Revision 1.11  1995/05/20  03:17:09  darryl
 * Color editor and view windows enhancements.
 *
 * Revision 1.10  1995/05/03  21:29:57  darryl
 * Lint and optimization cleanup.
 *
 * Revision 1.9  1995/04/30  11:41:08  darryl
 * Revise zoom authorization handling, color editor enhancements.
 *
 * Revision 1.8  1995/04/21  21:14:17  darryl
 * Remove unnecessary XFlush and XSync calls.
 *
 * Revision 1.7  1995/04/20  16:42:16  darryl
 * Style cleanup -- change tabs to spaces.
 *
 * Revision 1.6  1995/04/18  15:59:59  darryl
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
 * Revision 1.5  1995/04/01  00:12:51  darryl
 * Remove the warning message from the rubber band function.
 * Placed there for debugging then never deleted.
 *
 * Revision 1.4  1995/03/20  04:01:03  darryl
 * Revision sync. Change all BulletinBoard dialogs to Forms.
 * Delete all specific widget positioning and sizing resources.
 * Change popup menus to option menus. Add Undo command to
 * the color editor. Change usepixtab to 1 if privatecolor.
 *
 * Revision 1.1.1.3  1995/03/14  15:35:01  tom
 * use enumeration type for zoombox_dir
 *
 * Revision 1.1.1.2  1995/03/13  18:39:16  tom
 * (rubber_band): moved location of check for button1
 *
 * Revision 1.1.1.1  1995/03/11  03:45:41  tom
 * changed dxsize and dysize
 *
 * Revision 1.1  1995/03/03  22:56:36  darryl
 * Linux and HPUX enhancements, bug fixes. New basic options
 * dialog, 'Flip' submenu created differently. EditRes protocol
 * event handlers conditionally included. xmconfig.h generalized.
 *
 * Revision 0.305  1995/02/04  22:41:42  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.304  1995/02/04  20:22:38  darryl
 * Lint and style cleanup.
 *
 * Revision 0.303  1995/02/04  19:44:04  darryl
 * Prevent crashes when the color cycler speed is changed
 * with the cursor keys. Check to ensure that the event
 * that is passed to the rubber band function is non-null.
 *
 * Revision 0.302  1994/12/22  19:29:07  darryl
 * Remove redundant event processing.
 *
 * Revision 0.301  1994/12/20  20:06:02  darryl
 * Add 'Refresh' to the 'Reset' menu.
 *
 * Revision 0.300  1994/12/16  18:58:09  darryl
 * Revision sync - beta3 version.
 *
 * Revision 0.205  1994/12/10  01:05:03  darryl
 * More attempts to fortify the rubber-banding functions.
 *
 * Revision 0.204  1994/12/09  16:58:26  darryl
 * Fix bug where the zoombox colors could be reversed.
 *
 * Revision 0.203  1994/12/08  19:27:17  darryl
 * Ensure that the xgc contains the special colors as the
 * foreground and background when the color map changes.
 *
 * Revision 0.202  1994/12/07  17:30:43  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.201  1994/12/07  00:31:30  darryl
 * Change calls to rubber banding functions from a passive
 * pointer grab to translations from the canvas widget.
 *
 * Revision 0.200  1994/12/02  02:54:24  darryl
 * Revision sync - beta2 version.
 *
 * Revision 0.105  1994/11/30  00:58:52  darryl
 * Remove magic numbers from calc_status, use values
 * defined in the main header file.
 *
 * Revision 0.104  1994/11/29  02:08:16  darryl
 * Reverse the operation of the zoomoff variable so
 * that a non-zero value means that zooming is disabled.
 *
 * Revision 0.103  1994/11/27  01:07:47  darryl
 * Fix bugs where resume was not being handled correctly.
 *
 * Revision 0.102  1994/11/19  02:53:05  darryl
 * Attempt to fortify the rubber-banding to account for
 * delayed event processing by the X server.
 *
 * Revision 0.101  1994/11/18  17:09:57  darryl
 * Aesthetic changes - widget and label placement. Minor bug
 * fixes when saving parameters and drawing a zoom box.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
