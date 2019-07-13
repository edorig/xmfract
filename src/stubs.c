
#include "xmfract.h"
#include "fractype.h"
#include "macros.h"
#include "helpdefs.h"
#include "mpmath.h"
#include "globals.h"
#include "prototyp.h"

#ifndef NO_IDENT
#ident "$Id: stubs.c,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

/* more or less permanent stubs */

int intFormulaSetup() { return 1; }
int LongBifurcMay() { return 1; }
int LongBifurcLambdaTrig() { return 1; }
int LongBifurcAddTrigPi() { return 1; }
int LongBifurcSetTrigPi() { return 1; }
int LongBifurcStewartTrig() { return 1; }
int LongBifurcVerhulstTrig() { return 1; }
int JuliaFractal() { return 1; }
int LambdaFractal() { return 1; }
int longZpowerFractal() { return 1; }
int longCmplxZpowerFractal() { return 1; }
int TrigPlusSqrFractal() { return 1; }
int ScottTrigPlusSqrFractal() { return 1; }
int SkinnerTrigSubSqrFractal() { return 1; }
int long_julia_per_pixel() { return 1; }
int TrigPlusTrigFractal() { return 1; }
int ScottTrigPlusTrigFractal() { return 1; }
int SkinnerTrigSubTrigFractal() { return 1; }
int ZXTrigPlusZFractal() { return 1; }
int ScottZXTrigPlusZFractal() { return 1; }
int SkinnerZXTrigSubZFractal() { return 1; }
int LambdaTrigFractal() { return 1; }
int LambdaTrigFractal1() { return 1; }
int LambdaTrigFractal2() { return 1; }
int LambdaTrigfpFractal2() { return 1; }
int LongLambdaexponentFractal() { return 1; }
int LongPhoenixFractal() { return 1; }
int LongPhoenixPlusFractal() { return 1; }
int LongPhoenixMinusFractal() { return 1; }
int ManOWarFractal() { return 1; }
int SierpinskiFractal() { return 1; }
int Barnsley1Fractal() { return 1; }
int long_mandel_per_pixel() { return 1; }
int Barnsley2Fractal() { return 1; }
int SqrTrigFractal() { return 1; }
int UnityFractal() { return 1; }
int TrigZsqrdFractal() { return 1; }
int TrigXTrigFractal() { return 1; }
int Sqr1overTrigFractal() { return 1; }
int TrigPlusZsquaredFractal() { return 1; }
int LongTrigPlusExponentFractal() { return 1; }
int LPopcornFractal() { return 1; }
int MPCNewtonFractal() { return 1; }
int MPCjulia_per_pixel() { return 1; }
int SpiderFractal() { return 1; }
int Richard8Fractal() { return 1; }
int long_richard8_per_pixel() { return 1; }
int MarksMandelPwrFractal() { return 1; }
int marks_mandelpwr_per_pixel() { return 1; }
int LambdaTrigOrTrigFractal() { return 1; }
int JuliaTrigOrTrigFractal() { return 1; }
int MPCHalleyFractal() { return 1; }
int MPCHalley_per_pixel() { return 1; }
int long_phoenix_per_pixel() { return 1; }
int long_mandphoenix_per_pixel() { return 1; }
int TimsErrorFractal() { return 1; }
int MatchTPlusMode() { return 1; }
int CheckForTPlus() { return 1; }
/*ARGSUSED*/
void WriteTPlusBankedPixel(int col, int row, unsigned long color) { }

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: stubs.c,v $
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
 * Revision 0.101  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
