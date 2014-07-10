#if defined HEXTOOLSPLAY
#ifndef NAVALARMEDFORCE_H
#define NAVALARMEDFORCE_H

class NavalArmedForce
{
public:
	struct Allied
	{
		enum
		{
			NAV_SOV			=	0, // soviet
			NAV_RN,			//	1  royal navy
			NAV_USA,		//	2  us navy
			NAV_FRA,		//	3  french
			NAV_AUS,		//	4  australia
			NAV_BEL,		//	5  belgian
			NAV_DUT,		//	6  dutch
			NAV_NOR,		//	7  norwegian
			NAV_ALPOR,		//	8  portg
			NAV_ALSPA,		//	9  spanish
			NAV_PPLS,		//	10 fwtbt: ppls army
			NAV_CANADA,		//	11
			NAV_NZ,			//	12 new zealand
			NAV_GREEK,		//	13
			NAV_POLISH,		//	14
			NAV_AL_ITA,		//	15 italians at allied side
			NAV_AL_TURK,	//	16 turkish
			NAV_AL_IRAQ,	//	17 iraq
			NAV_AL_IRAN,	//	18 iran
			NAV_AL_SWE,		//	19 swedish
			NAV_SAFR,		//	20 south africa
			NAV_BRAZ,		//	21 brazil
			NAV_CZ,			//	22 czechoslovakia
			NAV_YUGO,		//	23 yugoslavian
			NAV_BUL,		//	24 bulgarian
			NAV_AL_FIN,		//	25 finnish in allied side
			NAV_AL_LITH,	//	26 lithuanian
			NAV_AL_EST,		//	27 estonian
			NAV_AL_EGY,		//	28 egyptian
			NAV_AL_BASQ,	//	29 fwtbt: basque
			NAV_AL_CATA,	//	30 fwtbt: catalan
			NAV_ALDAN,		//	31 danish
			NAV_ALLATV,		//	32 latvian
			NAV_ALETIO,		//	33 ethiopian
			NAV_ALSAUD,		//	34 saudi
			NAV_CHINA,		//	35
			NAV_KWAN,		//	36 kwantung
			NAV_MSF,		//	37 chinese misc southern factions
			NAV_MNF,		//	38 chinese misc northern factions
			NAV_SHAN,		//	39 shantung
			COUNT			//	40
		};
	};

	struct Axis
	{
		enum
		{
			NAV_KM			=	0,  // kriegsmarine
			NAV_FIN,		//	1	finnish
			NAV_ITA,		//	2	italian
			NAV_AXPOR,		//	3	portg
			NAV_AXSPA,		//	4	spanish
			NAV_NATI,		//	5	fwtbt: nationalist
			NAV_RUM,		//	6	rumanian
			NAV_HUN,		//	7	hungarian
			NAV_SLO,		//	8	slovakia
			NAV_AX_TURK,	//	9	turkish
			NAV_AX_IRAQ,	//	10	iraq
			NAV_AX_IRAN,	//	11	iran
			NAV_AX_SWE,		//	12	swedish
			NAV_AX_EST,		//	13	estonian
			NAV_ALB,		//	14	albanian
			NAV_AX_YUGO,	//	15	yugoslavian
			NAV_AX_BUL,		//	16	bulgarian
			NAV_AX_SOVNAVY,	//	17	FtF: soviet navy
			NAV_AX_LITH,	//	18	lithuanian
			NAV_AX_EGY,		//	19	egyptian
			NAV_AXVICH,		//	20	vichy france
			NAV_AXDAN,		//	21	danish
			NAV_AXLATV,		//	22	latvian
			NAV_AXETIO,		//	23	ethiopian
			NAV_AXSAUD,		//	24	saudi
			NAV_AXNORW,		//	25	norwegian
			NAV_IJN,		//	26
			NAV_IJA,		//	27
			COUNT			//	28
		};
	};

	char name_[32];	// Name, "Kriegsmarine", "Soviet", "Royal Navy", ...
	wxColour crFg;	// counter foreground
	wxColour crBg;	// counter background
	wxBitmap* hbm;	// bitmap for counter
	int games_;		// OR'ed games where this subtype appears
};

#endif
#endif
