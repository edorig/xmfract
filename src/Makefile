# Generated automatically from Makefile.in by configure.
# Makefile for xmfract
# Copyright (C) 1995 Thomas Winder

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# $Id: Makefile.in,v 1.2 1995/04/26 15:34:22 tom Exp $
# $Log: Makefile.in,v $
# Revision 1.2  1995/04/26  15:34:22  tom
# changed basename to nbasename to avoid problems with linux
#
# Revision 1.1.1.1  1995/04/25  10:26:49  root
# put under cvs control
#
# Revision 1.3  1995/03/26  16:45:01  tom
# install target
#
# Revision 1.2  1995/03/20  22:46:54  tom
# changed MOTIF handling
#
# Revision 1.1  1995/03/14  12:25:37  tom
# Initial revision
#
#
SHELL = /bin/sh

srcdir = .

prefix = /usr/local
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
libdir = $(prefix)/xmfract

INSTALL = /usr/bin/ginstall -c
INSTALL_PROGRAM = ${INSTALL}
CC = gcc
DEFS = -DHAVE_CONFIG_H
LIBS =    \
  -L/usr/lib -lXm  \
  -lXmu -lXt -lX11  ../lib/libxmfra.a -lieee -lm 
CFLAGS =  -g -O -I/usr/include 
LDFLAGS = 
STRIP = strip
RM = rm

SOURCES = \
authors_menu.c    globals.c         mpmath.c          stubs.c \
check_tgafile.c   help.c            newton.c          symmetry.c \
cmd_entry.c       ifs_params.c      oneortwopass.c    symplot.c \
cmdfile.c         ifsload.c         options_menu.c    tesseral.c \
coloredit.c       image_info.c      parser.c          testpt.c \
corner_param.c    inform.c          planar.c          three_d_lib.c \
create_image.c    init_data.c       plot3d.c          three_d_menu.c \
decoder.c         jiim.c            plot_orbit.c      three_d_params.c \
decomposition.c   julibrot.c        potential.c       timer.c \
diffusion.c       julibrot_param.c  reset_menu.c      tp3d.c \
draw_image.c      light_params.c    resize_window.c   type_param.c \
encoder.c         line3d.c          restore_menu.c    util.c \
ext_opts.c        loadfile.c        rotate.c          video.c \
fatal.c           loadmap.c         rubber_band.c     warning.c \
file_menu.c       lorenz.c          saveparms.c       window_options.c \
fill.c            lsys.c            savetodisk.c      window_size.c \
formula.c         lsys_params.c     select_type.c     worklist.c \
formula_params.c  lyapunov.c        solidguess.c      xmfract.c \
fractal_types.c   make_mig.c        standard.c        zoom.c \
fractalp.c        misc_menu.c       star_params.c     zoom_menu.c \
fractals.c        miscfrac.c        starfield.c       caution.c \
gifview.c         modal.c           stereo_params.c   calcfracinit.c \
bifurcation.c     basic_opts.c      attractor.c       boundary_trace.c \
calcfract.c       calcfroth.c       calcmand.c        calcmandfp.c \
cellular.c        check_mapfile.c


OBJECTS = \
authors_menu.o    globals.o         mpmath.o          stubs.o \
check_tgafile.o   help.o            newton.o          symmetry.o \
cmd_entry.o       ifs_params.o      oneortwopass.o    symplot.o \
cmdfile.o         ifsload.o         options_menu.o    tesseral.o \
coloredit.o       image_info.o      parser.o          testpt.o \
corner_param.o    inform.o          planar.o          three_d_lib.o \
create_image.o    init_data.o       plot3d.o          three_d_menu.o \
decoder.o         jiim.o            plot_orbit.o      three_d_params.o \
decomposition.o   julibrot.o        potential.o       timer.o \
diffusion.o       julibrot_param.o  reset_menu.o      tp3d.o \
draw_image.o      light_params.o    resize_window.o   type_param.o \
encoder.o         line3d.o          restore_menu.o    util.o \
ext_opts.o        loadfile.o        rotate.o          video.o \
fatal.o           loadmap.o         rubber_band.o     warning.o \
file_menu.o       lorenz.o          saveparms.o       window_options.o \
fill.o            lsys.o            savetodisk.o      window_size.o \
formula.o         lsys_params.o     select_type.o     worklist.o \
formula_params.o  lyapunov.o        solidguess.o      xmfract.o \
fractal_types.o   make_mig.o        standard.o        zoom.o \
fractalp.o        misc_menu.o       star_params.o     zoom_menu.o \
fractals.o        miscfrac.o        starfield.o       caution.o \
gifview.o         modal.o           stereo_params.o   calcfracinit.o \
bifurcation.o     basic_opts.o      attractor.o       boundary_trace.o \
calcfract.o       calcfroth.o       calcmand.o        calcmandfp.o \
cellular.o        check_mapfile.o

HEADERS = fractype.h  helpdefs.h  macros.h    prototyp.h \
          globals.h   keydefs.h   mpmath.h    xmfract.h


DISTFILES = Makefile.in $(SOURCES) 

PROGS = xmfract

LIBPROGS =

all: $(PROGS) $(LIBPROGS)

.SUFFIXES: .c .o

incl = -I.. -I$(srcdir) -I../lib -I$(srcdir)/../lib 


.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) $(incl) $(CFLAGS) $<

subdir = src
Makefile: ../config.status Makefile.in
	cd .. && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= ./config.status

installdirs:
	$(srcdir)/../mkinstalldirs $(bindir)

transform = s,x,x,
install: all installdirs
	for f in $(PROGS); do \
	  $(STRIP) $$f; \
	  $(INSTALL_PROGRAM) $$f \
	      $(bindir)/`echo $$f|sed '$(transform)'`; \
	done

uninstall:
	for f in $(PROGS); do \
	  $(RM) -f \
	      $(bindir)/`echo $$f|sed '$(transform)'`; \
	done

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

clean:
	$(RM) -f $(PROGS) $(LIBPROGS) *.o core

mostlyclean: clean

distclean: clean
	$(RM) -f Makefile

realclean: distclean
	$(RM) -f TAGS

distdir = ../`cat ../distname`/$(subdir)
dist: $(DISTFILES)
	for file in $(DISTFILES); do \
	  ln $$file $(distdir) \
	    || { echo copying $$file instead; cp -p $$file $(distdir);}; \
	done

# Linking rules.

link_command = $(CC) $(LDFLAGS) -o $@

xmfract: ../lib/libxmfra.a $(OBJECTS)
	$(link_command) $(OBJECTS) $(LIBS)

$(OBJECTS): $(HEADERS)

#.version: version.c
#	sed -e '/version_string/!d' \
#	    -e 's/[^"]*"\([^"]*\)".*/\1/' \
#	    -e q $(srcdir)/version.c \
#	  > $@-t
#	mv $@-t $@

# Make all .o files depend on these files even though there are a few
# unnecessary dependencies.
$(OBJECTS): ../config.h

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
