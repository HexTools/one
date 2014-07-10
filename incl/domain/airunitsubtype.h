#if defined HEXTOOLSPLAY
#ifndef AIRARMEDFORCE_H
#define AIRARMEDFORCE_H

class AirArmedForce
{
public:
	struct Allied
	{
		enum
		{
			AIR_SOV			=	0,
			AIR_GUARDS,		//	1
			AIR_USAAF,		//	2
			AIR_USNAVY,		//	3
			AIR_CANADA,		//	4	royal canadian air force
			AIR_RAF,		//	5
			AIR_NZ,			//	6	royal new zealander air force
			AIR_FRENCH,		//	7
			AIR_GREEK,		//	8
			AIR_POL,		//	9	polish
			AIR_FAA,		//	10	british fleet air arm
			AIR_AUS,		//	11	royal australian air force
			AIR_SAFR,		//	12	south african (SAAF)
			AIR_BELG,		//	13
			AIR_DUTCH,		//	14
			AIR_CZ,			//	15	czechoslovakian
			AIR_NORW,		//	16
			AIR_ALTRK,		//	17	turkish
			AIR_PPL,		//	18	fwtbt: people's army
			AIR_YUGO,		//	19	yugoslav
			AIR_ALSPA,		//	20	spanish
			AIR_ALPOR,		//	21	portuguese
			AIR_ALIRAN,		//	22	iran
			AIR_ALIRAQ,		//	23	iraq
			AIR_ALEGY,		//	24	egypt
			AIR_ALITA,		//	25	italian
			AIR_ALFIN,		//	26	finnish
			AIR_ALSWE,		//	27	swedish
			AIR_ALEST,		//	28	estonia
			AIR_ALLITH,		//	29	lithuania
			AIR_ALBUL,		//	30	bulgaria
			AIR_FREEFR,		//	31	free french
			AIR_ALSWI,		//	32	swiss
			AIR_ALDAN,		//	33	danish
			AIR_ALLATV,		//	34	latvian
			AIR_ALETIO,		//	35	ethiopian
			AIR_ALSAUD,		//	36	saudi arab
			AIR_RFC,		//	37	Royal Flying Corps
			AIR_CHINA,		//	38
			AIR_USAIR,		//	39
			AIR_WW1DUT,		//	40
			AIR_WW1ITA,		//	41
			AIR_WW1BEL,		//	42
			AIR_WW1RUS,		//	43
			AIR_WW1BRITNAV,	//	44
			AIR_WW1FRNAV,	//	45
			AIR_WW1ITANAV,	//	46
			AIR_WW1USNAV,	//	47
			COUNT			//	48
		};
	};

	struct Axis
	{
		enum
		{
			AIR_LW			=	0,
			AIR_ITA,		//	1
			AIR_RUM,		//	2
			AIR_HUN,		//	3
			AIR_FIN,		//	4
			AIR_AXTRK,		//	5	turkish
			AIR_NATI,		//	6	fwtbt: nationalist
			AIR_AXYG,		//	7	yugoslav
			AIR_AXBUL,		//	8	bulgarian
			AIR_AXSPA,		//	9	spanish
			AIR_AXPOR,		//	10	portuguese
			AIR_AXIRAN,		//	11	iran
			AIR_AXIRAQ,		//	12	iraq
			AIR_AXEGY,		//	13	egypt
			AIR_AXSOV,		//	14	soviet in axis side
			AIR_AXGUARD,	//	15	soviet guards in axis side
			AIR_AXSWE,		//	16	swedish
			AIR_AXEST,		//	17	estonia
			AIR_AXLITH,		//	18	lithuania
			AIR_AXVICH,		//	19	vichy france
			AIR_AXSWI,		//	20	swiss
			AIR_AXDAN,		//	21	danish
			AIR_AXLATV,		//	22	latvia
			AIR_AXETIO,		//	23	ethiopia
			AIR_AXSAUD,		//	24	saudi
			AIR_AXNORW,		//	25	norwegian
			AIR_WW1_GER,	//	26
			AIR_WW1_AH,		//	27	aust/hung
			AIR_SLO,		//	28	slovaks
			AIR_IJA,		//	29	ww2pacific: japanese army
			AIR_IJN,		//	30	ww2pacific: japanese navy
			AIR_AXWW1DUT,	//	31
			AIR_WW1AXITA,	//	32
			AIR_WW1GERNAV,	//	33
			AIR_WW1AXITANAV,//	34
			COUNT			//	35
		};
	};

	char name_[32];		// Name, "Luftwaffe", "Soviet", "USAAF", ...
	wxColour crFg;		// main counter foreground
	wxColour crFg2;		// 2nd counter foreground (usually crFg2==crFc)
	wxColour crBg;		// counter background
	wxBitmap* hbm[2];	// bitmap for counters (operative & inoperative)
	int games_;			// OR'ed games where this subtype appears
};

#endif
#endif
