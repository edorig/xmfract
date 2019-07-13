dnl additional macros for autoconfig
dnl $Id: aclocal.m4,v 1.1 1995/03/14 17:24:32 tom Exp $

AC_DEFUN(AC_PATH_MOTIF,
[AC_REQUIRE_CPP()dnl Set CPP; we run AC_PATH_MOTIF_DIRECT conditionally.
# If we find MOTIF, set shell vars motif_includes and motif_libraries to the
# paths, otherwise set no_motif=yes.
# Uses ac_ vars as temps to allow command line to override cache and checks.
# --without-motif overrides everything else, but does not touch the cache.
AC_MSG_CHECKING(for MOTIF)

AC_ARG_WITH(motif, [  --with-motif=\"motif_includes=DIR motif_libraries=DIR\"],
ac_motif_arg=$withval)
if test "x$ac_motif_arg" != x; then
  eval "$ac_motif_arg"
fi

if test "x$with_motif" = xno; then
  no_motif=yes
else
  if test "x$motif_includes" != xNONE && test "x$motif_libraries" != xNONE; then
    no_motif=
  else
AC_CACHE_VAL(ac_cv_path_motif,
[# One or both of the vars are not set, and there is no cached value.
no_motif=yes
AC_PATH_MOTIF_XMKMF
if test "$no_motif" = yes; then
AC_PATH_MOTIF_DIRECT
fi
if test "$no_motif" = yes; then
  ac_cv_path_motif="no_motif=yes"
else
  ac_cv_path_motif="no_motif= ac_motif_includes=$ac_motif_includes ac_motif_libraries=$ac_motif_libraries"
fi])dnl
  fi
  eval "$ac_cv_path_motif"
fi # $with_motif != no

if test "$no_motif" = yes; then
  AC_MSG_RESULT(no)
else
  test "x$motif_includes" = xNONE && motif_includes=$ac_motif_includes
  test "x$motif_libraries" = xNONE && motif_libraries=$ac_motif_libraries
  ac_cv_path_motif="no_motif= ac_motif_includes=$motif_includes ac_motif_libraries=$motif_libraries"
  AC_MSG_RESULT([libraries $motif_libraries, headers $motif_includes])

  if test "x$motif_libraries" != x; then
    MOTIF_LIBS="-L$motif_libraries"
  fi
  if test "x$motif_includes" != x; then
    MOTIF_CFLAGS="-I$motif_includes"
  fi

  AC_SUBST(MOTIF_LIBS)
  AC_SUBST(MOTIF_CFLAGS)
fi
])

dnl Internal subroutine of AC_PATH_MOTIF.
dnl Set ac_motif_includes, ac_motif_libraries, and no_motif (initially yes).
AC_DEFUN(AC_PATH_MOTIF_XMKMF,
[rm -fr conftestdir
if mkdir conftestdir; then
  cd conftestdir
  # Make sure to not put "make" in the Imakefile rules, since we grep it out.
  cat > Imakefile <<'EOF'
acfindx:
	@echo 'ac_im_incroot="${INCROOT}"; ac_im_usrlibdir="${USRLIBDIR}"; ac_im_libdir="${LIBDIR}"'
EOF
  if (xmkmf) >/dev/null 2>/dev/null && test -f Makefile; then
    no_motif=
    # GNU make sometimes prints "make[1]: Entering...", which would confuse us.
    eval `make acfindx 2>/dev/null | grep -v make`
    # Open Windows xmkmf reportedly sets LIBDIR instead of USRLIBDIR.
    if test ! -f $ac_im_usrlibdir/libXm.a && test -f $ac_im_libdir/libXm.a
    then
      ac_im_usrlibdir=$ac_im_libdir
    fi
    # Screen out bogus values from the imake configuration.
    case "$ac_im_incroot" in
	/usr/include) ;;
	*) test -f "$ac_im_incroot/Xm/Xm.h" && ac_motif_includes="$ac_im_incroot" ;;
    esac
    case "$ac_im_usrlibdir" in
	/usr/lib | /lib) ;;
	*) test -d "$ac_im_usrlibdir" && ac_motif_libraries="$ac_im_usrlibdir" ;;
    esac
  fi
  cd ..
  rm -fr conftestdir
fi
])

dnl Internal subroutine of AC_PATH_MOTIF.
dnl Set ac_motif_includes, ac_motif_libraries, and no_motif (initially yes).
AC_DEFUN(AC_PATH_MOTIF_DIRECT,
[test -z "$motif_direct_test_library" && motif_direct_test_library=Xm
test -z "$motif_direct_test_function" && motif_direct_test_function=XmCreateMainWindow
test -z "$motif_direct_test_include" && motif_direct_test_include=X11/Xm.h
AC_TRY_CPP([#include <$motif_direct_test_include>],
[no_motif= ac_motif_includes=],
[  for ac_dir in               \
    /usr/X11R6/include        \
    /usr/X11R5/include        \
    /usr/X11R4/include        \
                              \
    /usr/include/X11R6        \
    /usr/include/X11R5        \
    /usr/include/X11R4        \
                              \
    /usr/local/X11R6/include  \
    /usr/local/X11R5/include  \
    /usr/local/X11R4/include  \
                              \
    /usr/local/include/X11R6  \
    /usr/local/include/X11R5  \
    /usr/local/include/X11R4  \
                              \
    /usr/X11/include          \
    /usr/include/X11          \
    /usr/local/X11/include    \
    /usr/local/include/X11    \
                              \
    /usr/X386/include         \
    /usr/x386/include         \
    /usr/XFree86/include/X11  \
                              \
    /usr/include              \
    /usr/local/include        \
    /usr/unsupported/include  \
    /usr/athena/include       \
    /usr/local/x11r5/include  \
    /usr/lpp/Xamples/include  \
                              \
    /usr/dt/include           \
    /usr/openwin/include      \
    /usr/openwin/share/include \
    /opt/SUNWmotif/include   \
    ; \
  do
    if test -r "$ac_dir/$motif_direct_test_include"; then
      no_motif= ac_motif_includes=$ac_dir
      break
    fi
  done])

# Check for the libraries.
# See if we find them without any special options.
# Don't add to $LIBS permanently.
ac_save_LIBS="$LIBS"
LIBS="-l$motif_direct_test_library $LIBS"
AC_TRY_LINK(, [${motif_direct_test_function}()],
[LIBS="$ac_save_LIBS" no_motif= ac_motif_libraries=],
[LIBS="$ac_save_LIBS"
# First see if replacing the include by lib works.
for ac_dir in `echo "$ac_motif_includes" | sed s/include/lib/` \
    /usr/X11R6/lib        \
    /usr/X11R5/lib        \
    /usr/X11R4/lib        \
                          \
    /usr/lib/X11R6        \
    /usr/lib/X11R5        \
    /usr/lib/X11R4        \
                          \
    /usr/local/X11R6/lib  \
    /usr/local/X11R5/lib  \
    /usr/local/X11R4/lib  \
                          \
    /usr/local/lib/X11R6  \
    /usr/local/lib/X11R5  \
    /usr/local/lib/X11R4  \
                          \
    /usr/X11/lib          \
    /usr/lib/X11          \
    /usr/local/X11/lib    \
    /usr/local/lib/X11    \
                          \
    /usr/X386/lib         \
    /usr/x386/lib         \
    /usr/XFree86/lib/X11  \
                          \
    /usr/lib              \
    /usr/local/lib        \
    /usr/unsupported/lib  \
    /usr/athena/lib       \
    /usr/local/x11r5/lib  \
    /usr/lpp/Xamples/lib  \
                          \
    /usr/dt/lib           \
    /usr/openwin/lib      \
    /usr/openwin/share/lib \
    /opt/SUNWmotif/lib    \
    ; \
do
  for ac_extension in a so sl; do
    if test -r $ac_dir/lib${motif_direct_test_library}.$ac_extension; then
      no_motif= ac_motif_libraries=$ac_dir
      break 2
    fi
  done
done])])
