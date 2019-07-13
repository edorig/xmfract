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

# $Id: Makefile.in,v 1.1 1995/03/26 16:44:54 tom Exp $
# $Log: Makefile.in,v $
# Revision 1.1  1995/03/26  16:44:54  tom
# Initial revision
#
#

SHELL = /bin/sh

srcdir = .

prefix = /usr/local
libdir = $(prefix)/xmfract
helpdir = $(libdir)/help
INSTALL = /usr/bin/ginstall -c
INSTALL_DATA = ${INSTALL} -m 644
RM = rm

FILES = help.*

DISTFILES = Makefile.in $(FILES)

all:

.SUFFIXES:

subdir = help
Makefile: ../config.status Makefile.in
	cd .. && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= ./config.status

installdirs:
	$(srcdir)/../mkinstalldirs $(helpdir)

install: installdirs
	for f in $(FILES) ; do \
	  $(INSTALL_DATA) $(srcdir)/$$f $(helpdir) ;\
	done

uninstall:
	for f in $(FILES) ; do \
	  $(RM) -f $(helpdir)/$$f ;\
	done

TAGS:

check:

clean:

mostlyclean: clean

distclean: clean
	$(RM) -f Makefile

realclean: distclean
	$(RM) -f TAGS

distdir = ../`cat ../distname`/$(subdir)
dist:
	for file in $(DISTFILES); do \
	  ln $$file $(distdir) \
	    || { echo copying $$file instead; cp -p $$file $(distdir);}; \
	done

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
