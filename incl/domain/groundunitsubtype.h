#if defined HEXTOOLSPLAY
#ifndef GROUNDARMEDFORCE_H
#define GROUNDARMEDFORCE_H

class GroundArmedForce
{
public:
	struct Allied
	{
		enum
		{
			SOV					=	0,	// soviet army
			GUARDS,				//	1	soviet guards
			NKVD,				//	2	soviet nkvd
			SOVNAVY,			//	3	soviet navy
			USA,				//	4
			USAAF,				//	5	us air forces
			USNAVY,				//	6
			CANADA,				//	7
			BRITISH,			//	8
			RAF,				//	9	royal air forces
			INDIAN,				//	10	indian army
			NZ,					//	11	new zealand
			FRENCH,				//	12	"metropolitan" french
			GREEK,				//	13
			POLISH,				//	14
			AL_ITA,				//	15	italians at allied side
			AL_TURK,			//	16	turkish
			AL_IRAQ,			//	17	iraq
			AL_TJ,				//	18	transjordan
			AL_IRAN,			//	19	iran
			AL_SWE,				//	20	swedish
			SOVW,				//	21	soviet winterized
			SOVF,				//	22	soviet foreign contingent
			SOVM,				//	23	soviet militia
			OSS,				//	24	usa OSS
			USCAN,				//	25	usa canadian
			USMC,				//	26	usa marines
			RNAVY,				//	27	royal navy
			RMC,				//	28	royal marines
			SAFR,				//	29	south africa
			INDSF,				//	30	indian state force
			BRCOL,				//	31	british colonial
			BREMP,				//	32	british empire/inter-allied
			AUST,				//	33	australian
			FR_AFR,				//	34	french african
			FR_COL,				//	35	french colonial
			FR_FOR,				//	36	french foreign legion
			OTHAL,				//	37	general allied
			BELG,				//	38	belgian
			BRAZ,				//	39	brazil
			CZ,					//	40	czechoslovakia
			DUTCH,				//	41	dutch
			NORW,				//	42	norwegian
			YUGO,				//	43	yugoslavian
			BUL,				//	44	bulgarian
			AL_FIN,				//	45	finnish in allied side
			AL_LITH,			//	46	lithuanian
			AL_PPL,				//	47	FWtBT peoples army
			AL_INTR,			//	48	FWtBT international
			AL_ASTU,			//	49	FWtBT asturian
			AL_BASQ,			//	50	FWtBT basque
			AL_CATA,			//	51	FWtBT catalan
			AL_SANT,			//	52	FWtBT santandero
			AL_ANAR,			//	53	FWtBT anarchist
			AL_POUM,			//	54	FWtBT POUM
			AL_SPA,				//	55	spanish
			AL_POR,				//	56	portugal
			AL_EST,				//	57	estonian
			AL_EGY,				//	58	egyptian
			FREEFR,				//	59	free french
			AL_SWI,				//	60	switzerland
			AL_DAN,				//	61	danish
			AL_LATV,			//	62	latvian
			AL_ETIO,			//	63	ethiopian
			AL_SAUDI,			//	64	saudi-arabian
			AL_WW1_RUS,			//	65	ww1: russian
			AL_WW1_ITA,			//	66
			AL_WW1_BEL,			//	67	ww1: belgian
			AL_WW1_BELFP,		//	68	ww1: belgian, force publique
			AL_WW1_RFC,			//	69	ww1: royal flying corps
			AL_WW1_RN,			//	70	ww1: royal navy
			AL_WW1_ANZAC,		//	71	ww1:
			AL_SPA_COL,			//	72	spanish colonial
			AL_WW1_INDP,		//	73	ww1: Indian Princely States
			AL_WW1_ITACOL,		//	74	ww1: italian contingents
			AL_WW1_ITACON,		//	75	ww1: italian colonial
			AL_WW1_DUT,			//	76	dutch
			AL_WW1_DUTC,		//	77	dutch colonial
			AL_WW1_COS,			//	78	cossack
			AL_WW1_RUSC,		//	79	russian contingents
			AL_WW1_RUSPG,		//	80	russian provisional goverment
			AL_WW1_WSAN,		//	81	western sanussi
			AL_WW1_ESAN,		//	82	eastern sanussi
			AL_FR_AF,			//	83	french air force
			AL_FR_NAV,			//	84	french navy
			AL_WW1_SPA_COL,		//	85	WWI spanish colonial have different color from WWII
			AL_BRIT_EMP,		//	86	british empire
			ALG_CHI_CENTRAL,	//	87	ww2pacific: china central army
			ALG_YUNAN,			//	88	ww2pacific:
			ALG_KWEICHOW,		//	89	ww2pacific:
			ALG_KWANGSI,		//	90	ww2pacific:
			ALG_SZECHWAN,		//	91	ww2pacific:
			ALG_KWANTUNG,		//	92	ww2pacific:
			ALG_HUNAN,			//	93	ww2pacific:
			ALG_MISCS,			//	94	ww2pacific: misc southern factions
			ALG_MACHEX,			//	95	ww2pacific: manchurian exiles
			ALG_SHANTUNG,		//	96	ww2pacific:
			ALG_SHANSI,			//	97	ww2pacific:
			ALG_HOPI,			//	98	ww2pacific:
			ALG_MISCN,			//	99	ww2pacific: misc northern factions
			ALG_MOSLEMS,		//	100	ww2pacific:
			ALG_COMMUNIST,		//	101	ww2pacific: chinese communists
			ALG_MULTINAT,		//	102	ww2pacific: multinational
			ALG_PHILIP,			//	103	ww2pacific: philippine
			AL_USAIR,			//	104	ww1: us aeronautical section
			AL_US_CA,			//	105	ww1: us central american
			AL_US_CS,			//	106	ww1: us controlled states
			AL_WW1CS,			//	107	czechoslovak
			AL_THAI,			//	108	thailand
			AL_SOV_PART,		//	109	soviet partisans
			COUNT				//	110
		};
	};

	struct Axis
	{
		enum
		{
			GER				=	0,  // wehrmacht
			SS,				//	1	SS
			SA,				//	2	SA
			LW,				//	3	luftwaffe
			FIN,			//	4	finnish
			RUM,			//	5	rumanian
			ITA,			//	6	italian
			RSI,			//	7	RSI italian
			HUN,			//	8	hungarian
			SLO,			//	9	slovakia
			AX_TURK,		//	10	turkish
			AX_IRAQ,		//	11	iraq
			AX_TJ,			//	12	transjordan
			AX_IRAN,		//	13	iran
			AX_SWE,			//	14	swedish
			AX_EST,			//	15	estonian
			AX_EAST,		//	16	eastern troops
			BRAND,			//	17	brandenburger
			SSPOL,			//	18	ss-police
			KM,				//	19	kriegsmarine
			OKW,			//	20	german OKW forces
			HY,				//	21	hitler youth
			AX_FOR,			//	22	german special and foreign contingents
			CCNN,			//	23	italian CCNN "blackshirts"
			ALB,			//	24	albanian
			AX_YUGO,		//	25	yugoslavian
			AX_BUL,			//	26	bulgarian
			AX_SOV,			//	27	FtF: soviet army
			AX_GUARDS,		//	28	FtF: soviet guards
			AX_NKVD,		//	29	FtF: soviet nkvd
			AX_SOVNAVY,		//	30	FtF: soviet navy
			AX_LITH,		//	31	lithuanian
			AX_NATI,		//	32	FWtBT nationalist
			AX_COLO,		//	33	FWtBT colonianl
			AX_FALA,		//	34	FWtBT falangist
			AX_REQU,		//	35	FWtBT requete
			AX_MXD,			//	36	FWtBT mixed
			AX_SPA,			//	37	spanish
			AX_POR,			//	38	portugal
			AX_EGY,			//	39	egyptian
			AXFRENCH,		//	40	(vichy) french
			AXFR_AFR,		//	41	(vichy) french
			AXFR_COL,		//	42	(vichy) french
			AXFR_FOR,		//	43	(vichy) french
			AX_SWI,			//	44	switzerland
			AX_GERWINT,		//	45	wehrmacht winterized
			AX_DAN,			//	46	danish
			FINVOL,			//	47	finnish foreign volunteer (aww)
			AX_LATV,		//	48	latvian
			AX_ETIO,		//	49	ethiopian
			AX_SAUDI,		//	50	saudi-arabian
			AX_NORW,		//	51	norwegians
			AX_SOVW,		//	52	soviet winterized
			AX_WW1_AH,		//	53	ww1: austro-hungarian
			AX_WW1_BAV,		//	54	ww1: bavarian
			AX_WW1_AUS,		//	55	ww1: austrian
			AX_WW1_HUN,		//	56	ww1: hungarian
			AX_WW1_PRUS,	//	57	ww1:
			AX_WW1_SAX,		//	58	ww1: saxon
			AX_WW1_WURT,	//	59	ww1: Wurttemburg
			AX_WW1_GCOL,	//	60	ww1: german colonial
			AX_WW1_IRE,		//	61	ww1: irish rebels
			AX_WW1_GERC,	//	62	ww1: german contingents
			AX_WW1_BER,		//	63	berber rebels
			AX_WW1_GERAF,	//	64	ww1: german air force
			AX_WW1_GERNA,	//	65	ww1: german navy
			AX_JP_ARMY,		//	66	ww2pacific: japanese army
			AX_IJN,			//	67	ww2pacific: jap.navy ground forces
			AX_MANCHU,		//	68	ww2pacific: manchuko puppet governemt
			AX_FORMOSA,		//	69	ww2pacific: formosan pup.
			AX_MONGOL,		//	70	ww2pacific: Mongolian pup.
			AX_CHINA,		//	71	ww2pacific: chinese pup.
			AX_WRUS,		//	72	ww2pacific: white russians pup.
			AX_EHOPI,		//	73	ww2pacific: east hopi pup.
			AX_JEHOL,		//	74	ww2pacific: Jehol
			AX_WW1_DUT,		//	75	ww1
			AX_WW1_DUTC,	//	76	ww1
			AX_AFR_NAT,		//	77	african natives
			AX_BOER,		//	78	south afr. boer
			AX_MEX,			//	79	mexico
			AX_WW1_SAN,		//	80	western sanussi
			COUNT			//	81
		};
	};

	char name_[32];		// four letter abbreviation, etc "Ger", "Sov", "Fin"
	char longname_[42];	// full name, etc "German", "Soviet", "Finnish"
	wxColour crFg;		// symbol color
	wxColour crText;	// text color
	wxColour crBg;		// counter background
	wxBitmap* hbm[4];	// bitmaps for corps counters
	int games_;			// OR'ed games where this subtype appears

	GroundArmedForce( )
	{
		games_ = 0;
	}
};

#endif
#endif
