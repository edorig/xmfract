
#ifndef macros_h
#define macros_h

#ifndef NO_IDENT
#ident "$Id: macros.h,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif

#define ABS(x) ((x)>0?(x):-(x))

#define ANGLE2DOUBLE (2*PI / 4294967296.)

#define CMPLXadd(arg1,arg2,out)\
        (out).x = (arg1).x + (arg2).x; (out).y = (arg1).y + (arg2).y

#define CMPLXmod(z) (sqr((z).x)+sqr((z).y))

#define CMPLXmult(arg1,arg2,out)\
        {\
        _CMPLX TmP;\
        TmP.x = (arg1).x*(arg2).x - (arg1).y*(arg2).y;\
        TmP.y = (arg1).x*(arg2).y + (arg1).y*(arg2).x;\
        (out) = TmP;\
        }

#define CMPLXmult1(arg1,arg2,out) Arg2->d = (arg1); Arg1->d = (arg2);\
        dStkMul(); Arg1++; Arg2++; (out) = Arg2->d

#define CMPLXpwr(arg1,arg2,out) (out)= ComplexPower((arg1), (arg2))

#define CMPLXrecip(arg,out)\
        { double denom; denom = sqr((arg).x) + sqr((arg).y);\
        if(denom==0.0) {(out).x = 1.0e10;(out).y = 1.0e10;}else\
        { (out).x =  (arg).x/denom;\
        (out).y = -(arg).y/denom;}}

#define CMPLXsqr_old(out)\
        (out).y = (old.x+old.x) * old.y;\
        (out).x = tempsqrx - tempsqry

#define CMPLXsub(arg1,arg2,out)\
        (out).x = (arg1).x - (arg2).x; (out).y = (arg1).y - (arg2).y

#define CMPLXtimesreal(arg,real,out)\
        (out).x = (arg).x*(real);\
        (out).y = (arg).y*(real)

#define CMPLXtrig0(arg,out) Arg1->d = (arg); dtrig0(); (out)=Arg1->d

#define CTL(x) ((x)&0x1f)

#define DIST1(z) (((z).x-1.0)*((z).x-1.0)+((z).y)*((z).y))

#define D_TO_L(x) ((long)((x)*(1L<<FROTH_BITSHIFT)))

#define END_OF_LINE(c) ((c) == '\n' || (c) == '\f')

#define FIXEDLT1 536870912.0

#define FIXEDMUL 524288L

#define FIXEDPT(x) ((long) (FIXEDMUL * (x)))

#define FLOATEXPBAILOUT()\
        if (fabs(old.y) >= 1.0e8) return(1);\
        if (fabs(old.x) >= 6.4e2) return(1);

#define FLOATHTRIGBAILOUT()\
        if (fabs(old.x) >= rqlim2) return(1);

#define FLOATTRIGBAILOUT()\
        if (fabs(old.y) >= rqlim2) return(1);

#define FLOATXYTRIGBAILOUT()\
        if (fabs(old.x) >= rqlim2 || fabs(old.y) >= rqlim2) return(1);

#define FOURPI (long)(4*PI*(double)(1L << 16))

#define GET16(c,i) (i) = (*(unsigned char *)&(c))+\
        ((*((unsigned char*)&(c)+1))<<8)

#define LCMPLXpwr(arg1,arg2,out) Arg2->l = (arg1); Arg1->l = (arg2);\
        lStkPwr(); Arg1++; Arg2++; (out) = Arg2->l

#define LCMPLXtrig0(arg,out) Arg1->l = (arg); ltrig0(); (out)=Arg1->l

#define LDIST1(z) (lsqr((((z).x)-fudge)) + lsqr(((z).y)))

#define LONGBAILOUT()\
        ltempsqrx = lsqr(lnew.x); ltempsqry = lsqr(lnew.y);\
        lmagnitud = ltempsqrx + ltempsqry;\
        if (lmagnitud >= llimit || lmagnitud < 0 || labs(lnew.x) > llimit2\
        || labs(lnew.y) > llimit2 || overflow)\
        { overflow=0;return(1);}\
        lold = lnew;

#define LONGEXPBAILOUT()\
        if (labs(lold.y) >= (1000L<<bitshift)) return(1);\
        if (labs(lold.x) >= (8L<<bitshift)) return(1);

#define LONGHTRIGBAILOUT()\
        if(labs(lold.x) >= llimit2 || overflow) { overflow=0;return(1);}

#define LONGTRIGBAILOUT()\
        if(labs(lold.y) >= llimit2 || overflow) { overflow=0;return(1);}

#define LONGXYTRIGBAILOUT()\
        if(labs(lold.x) >= llimit2 || labs(lold.y) >= llimit2 || overflow)\
        { overflow=0;return(1);}

#define LTRIGARG(X)\
        if(labs((X)) > l16triglim)\
        {\
        double tmp;\
        tmp = (X);\
        tmp /= fudge;\
        tmp = fmod(tmp,twopi);\
        tmp *= fudge;\
        (X) = tmp;\
        }

#ifdef MIN
# undef MIN
#endif

#define MIN(x,y) ((x)<(y)?(x):(y))

#define MPdistance(z1,z2)  (*pMPadd(pMPsqr(*pMPsub((z1).x,(z2).x)),\
        pMPsqr(*pMPsub((z1).y,(z2).y))))

#define ORBITCALC fractalspecific[o_fractype].orbitcalc

#define PAR_KEY(x) ( x < 10 ? '0' + x : 'a' - 10 + x)

#define PER_IMAGE fractalspecific[o_fractype].per_image

#define PER_PIXEL fractalspecific[o_fractype].per_pixel

#define SIGN(x) ((x)>0?1:-1)

#define TRIG16CHECK(X)\
        if(labs((X)) > l16triglim || overflow) { overflow=0;return(1);}

#define advance_match() coming_from=((going_to=(going_to-1)&0x03)-1)&0x03

#define advance_no_match() going_to=(going_to+1)&0x03

#define calcadot(c,x,y) { col=x; row=y; if((c=(*calctype)())== -1) return -1; }

#define conjugate(pz) ((pz)->y = 0.0 - (pz)->y)

#define distance(z1,z2) (sqr((z1).x-(z2).x)+sqr((z1).y-(z2).y))

#define dot_product(v1,v2) ((v1)[0]*(v2)[0]+(v1)[1]*(v2)[1]+(v1)[2]*(v2)[2])

#define em2float(l) (*(float *)&(l))

#define float2em(f) (*(long *)&(f))

#define lsqr(x) (multiply((x),(x),bitshift))

#define max(a,b) ((a)>(b)?(a):(b))

#define min(a,b) ((a)<(b)?(a):(b))

#define mkptr(seg,off) (void *)(seg+off)

#define modulus(z) (sqr((z).x)+sqr((z).y))

#define movedata(s_seg,s_off,d_seg,d_off,len) memcpy(d_off,s_off,len)

#define nonalpha(c) ((c)<32 || (c)>127)

#define pMPsqr(z) (*pMPmul((z),(z)))

#define rand15() (rand()&0x7FFF)

#define sign(x) ((x)>=0?1:-1)

#define sqr(x) ((x)*(x))

#define write1(ptr,len,n,stream) (fputc(*(ptr),stream),1)

#define write2(ptr,len,n,stream) (fputc((*(ptr))&255,stream),\
        fputc((*(ptr))>>8,stream),1)

#endif

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: macros.h,v $
 * Revision 1.400  1995/06/20  15:39:23  darryl
 * Revision sync - release 1.4
 *
 * Revision 1.5  1995/06/07  19:20:09  darryl
 * Fix problem where the MIN macro was not guarded from
 * previous declarations and that the palette randomization
 * Done and Apply buttons were handled incorrectly.
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
 * Revision 0.103  1994/11/29  00:54:22  darryl
 * Lint and optimization cleanup.
 *
 * Revision 0.102  1994/11/28  20:21:53  darryl
 * Fix bug where some non-standard fractal types were not
 * interrupted when escape key was pressed. Move more macros
 * and definitions to header files.
 *
 * Revision 0.101  1994/11/26  02:10:28  darryl
 * Add help index and help topic browser.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
