/* $Id: acconfig.h,v 1.2 1995/03/26 16:41:50 tom Exp $ */

/* div_t is in stdlib.h */
#undef HAVE_DIV_T

/* use EDITRES feature */
#undef EDITRES_ENABLE

/* gettimeofday only needs one parameter */
#undef GETTIMEOFDAY_HAS_ONE_PARA
