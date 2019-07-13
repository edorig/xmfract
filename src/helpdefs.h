
#ifndef helpdefs_h
#define helpdefs_h

#ifndef NO_IDENT
#ident "$Id: helpdefs.h,v 1.400 1995/06/20 15:39:23 darryl Exp $"
#endif


/* labels */

#define HELP_INDEX          0
#define HELP3D              1
#define HELP3DFILL          2
#define HELP3DFRACT         3
#define HELP3DGLASSES       4
#define HELP3DLIGHT         5
#define HELP3DMODE          6
#define HELP3DOVLY          7
#define HELP3DPARMS         8
#define HELPCOLORMAP        9
#define HELPCOMMANDS       10
#define HELPCOORDS         11
#define HELPCYCLING        12
#define HELPFRACTALS       13
#define HELPLOADFILE       14
#define HELPMAIN           15
#define HELPMENU           16
#define HELPPARMFILE       17
#define HELPSAVEREST       18
#define HELPSTARFLD        19
#define HELPVIDSEL         20
#define HELPVIEW           21
#define HELPEDITPAL        22
#define HELPXOPTS          23
#define HELPYOPTS          24
#define HELPZOOM           25
#define HELP_JIIM          26
#define HELP_ORBITS        27

#define HELP_FORMULA_START 28

#define HF_BARNSJ1         28
#define HF_BARNSJ2         29
#define HF_BARNSJ3         30
#define HF_BARNSM1         31
#define HF_BARNSM2         32
#define HF_BARNSM3         33
#define HF_BIFEQSINPI      34
#define HF_BIFLAMBDA       35
#define HF_BIFMAY          36
#define HF_BIFPLUSSINPI    37
#define HF_BIFSTEWART      38
#define HF_BIFURCATION     39
#define HF_CELLULAR        40
#define HF_CIRCLE          41
#define HF_CMPLXMARKSJUL   42
#define HF_CMPLXMARKSMAND  43
#define HF_COMPLEXNEWT     44
#define HF_DIFFUS          45
#define HF_DYNAM           46
#define HF_FNPLUSFN        47
#define HF_FNPLUSFNPIX     48
#define HF_FNTIMESFN       49
#define HF_FNXZPLUSZ       50
#define HF_FNZTIMESZ       51
#define HF_FROTH           52
#define HF_GINGER          53
#define HF_HALLEY          54
#define HF_HENON           55
#define HF_HOPALONG        56
#define HF_HYPERC          57
#define HF_HYPERCJ         58
#define HF_ICON            59
#define HF_INVERSE         60
#define HF_JULFNPLUSEXP    61
#define HF_JULFNPLUSZSQRD  62
#define HF_JULIA           63
#define HF_JULIA4          64
#define HF_JULIAFNFN       65
#define HF_JULZPOWER       66
#define HF_JULZZPWR        67
#define HF_KAM             68
#define HF_LAMBDA          69
#define HF_LAMBDAFN        70
#define HF_LAMBDAFNFN      71
#define HF_LORENZ          72
#define HF_LORENZ3D1       73
#define HF_LORENZ3D3       74
#define HF_LORENZ3D4       75
#define HF_MAGJ1           76
#define HF_MAGJ2           77
#define HF_MAGM1           78
#define HF_MAGM2           79
#define HF_MANDEL          80
#define HF_MANDEL4         81
#define HF_MANDELCLOUD     82
#define HF_MANDELFNFN      83
#define HF_MANDFN          84
#define HF_MANDFNPLUSEXP   85
#define HF_MANDFNPLUSZSQRD 86
#define HF_MANDPHOENIX     87
#define HF_MANLAMFNFN      88
#define HF_MANOWAR         89
#define HF_MANOWARJ        90
#define HF_MANZPOWER       91
#define HF_MANZZPWR        92
#define HF_MARKSJULIA      93
#define HF_MARKSMAND       94
#define HF_MARKSMANDPWR    95
#define HF_MARTIN          96
#define HF_MLAMBDA         97
#define HF_NEWT            98
#define HF_NEWTBAS         99
#define HF_PHOENIX        100
#define HF_PICKOVER       101
#define HF_PLASMA         102
#define HF_POPCJUL        103
#define HF_POPCORN        104
#define HF_QUAT           105
#define HF_QUATJ          106
#define HF_ROSS           107
#define HF_SIER           108
#define HF_SPIDER         109
#define HF_SQRFN          110
#define HF_SQROVFN        111
#define HF_TEST           112
#define HF_TETRATE        113
#define HF_TIMSERR        114
#define HF_UNITY          115

#define HELP_FORMULA_END  115

#define HT_BARNS          116
#define HT_BIF            117
#define HT_CELLULAR       118
#define HT_CIRCLE         119
#define HT_DIFFUS         120
#define HT_DYNAM          121
#define HT_FNORFN         122
#define HT_FORMULA        123
#define HT_FROTH          124
#define HT_GINGER         125
#define HT_HALLEY         126
#define HT_HENON          127
#define HT_HYPERC         128
#define HT_ICON           129
#define HT_IFS            130
#define HT_INVERSE        131
#define HT_JULIA          132
#define HT_JULIBROT       133
#define HT_KAM            134
#define HT_LAMBDA         135
#define HT_LAMBDAFN       136
#define HT_LORENZ         137
#define HT_LSYS           138
#define HT_LYAPUNOV       139
#define HT_MAGNET         140
#define HT_MANDEL         141
#define HT_MANDELCLOUD    142
#define HT_MANDFN         143
#define HT_MANDJUL4       144
#define HT_MARKS          145
#define HT_MARTIN         146
#define HT_MLAMBDA        147
#define HT_NEWT           148
#define HT_NEWTBAS        149
#define HT_NEWTCMPLX      150
#define HT_PHOENIX        151
#define HT_PICK           152
#define HT_PICKMJ         153
#define HT_PLASMA         154
#define HT_POPCORN        155
#define HT_QUAT           156
#define HT_ROSS           157
#define HT_SCOTSKIN       158
#define HT_SIER           159
#define HT_TEST           160
#define HT_UNITY          161
#define HELPXMFRACT       162
#define HELPGNU           163
#define HELPCHAOS         164

#define HELP_TOPIC_TOTAL  164

#endif

/*******************************************************************
 * MODIFICATION HISTORY
 *
 * $Log: helpdefs.h,v $
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
 * Revision 0.105  1994/11/26  21:27:00  darryl
 * Expand help, adding specific formulas to a separate
 * dialog called from the main help index.
 *
 * Revision 0.104  1994/11/26  02:10:28  darryl
 * Add help index and help topic browser.
 *
 * Revision 0.103  1994/11/25  20:31:03  darryl
 * Expand main help menu.
 *
 * Revision 0.102  1994/11/24  02:15:29  darryl
 * Integrate on-line help. More widget alignment changes.
 *
 * Revision 0.101  1994/11/23  05:19:37  darryl
 * Initial implementation of help. Minor widget alignment
 * changes. Widget name changes for uniqueness.
 *
 * Revision 0.100  1994/11/18  05:31:10  darryl
 * Initial beta version.
 *
 * END MODIFICATION HISTORY
 *******************************************************************/
