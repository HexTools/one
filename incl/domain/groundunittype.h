#if defined HEXTOOLSPLAY
#ifndef GROUNDUNITTYPE_H
#define GROUNDUNITTYPE_H

class MovementCategory;
class ArmorEffectsValue;
class GroundUnit;

// unit types, these are stored in array in MapPane.cpp
class GroundUnitType
{
public:
	static const unsigned short HAS_HE 			= 0x0001;	// hvy equip
	static const unsigned short HAS_HA 			= 0x0002;	// hvy arms
	static const unsigned short RAIL_ONLY		= 0x0004;	// move via RR only
	static const unsigned short ENGINEER		= 0x0008;	// combat engineer
	static const unsigned short CONSTRUCTION	= 0x0010;	// construction capable
	static const unsigned short WINTERIZED		= 0x0020;	// intrinsically winterized
	static const unsigned short AMPHIBIOUS		= 0x0040;	// amphibiously-trained
	static const unsigned short AIRDROP			= 0x0080;	// air-droppable
	static const unsigned short COMMANDO		= 0x0100;
	static const unsigned short LIGHT_INF		= 0x0200;
	static const unsigned short MOUNTAIN		= 0x0400;
	static const unsigned short SKI				= 0x0800;
	static const unsigned short CAMEL			= 0x1000;

	void setHE( void )
	{
		flags_ |= HAS_HE;
	}
	void clearHE( void )
	{
		flags_ &= ~HAS_HE;
	}
	void setHA( void )
	{
		flags_ |= HAS_HA;
	}
	void clearHA( void )
	{
		flags_ &= ~HAS_HA;
	}
	void setRailOnly( void )
	{
		flags_ |= RAIL_ONLY;
	}
	void clearRailOnly( void )
	{
		flags_ &= ~RAIL_ONLY;
	}
	void setEng( void )
	{
		flags_ |= ENGINEER;
	}
	void clearEng( void )
	{
		flags_ &= ~ENGINEER;
	}
	void setCons( void )
	{
		flags_ |= CONSTRUCTION;
	}
	void clearCons( void )
	{
		flags_ &= ~CONSTRUCTION;
	}
	void setWinterized( void )
	{
		flags_ |= WINTERIZED;
	}
	void clearWinterized( void )
	{
		flags_ &= ~WINTERIZED;
	}
	void setAmphibious( void )
	{
		flags_ |= AMPHIBIOUS;
	}
	void clearAmphibious( void )
	{
		flags_ &= ~AMPHIBIOUS;
	}
	void setAirdroppable( void )
	{
		flags_ |= AIRDROP;
	}
	void clearAirdroppable( void )
	{
		flags_ &= ~AIRDROP;
	}
	void setCommando( void )
	{
		flags_ |= COMMANDO;
	}
	void clearCommando( void )
	{
		flags_ &= ~COMMANDO;
	}
	void setLightInf( void )
	{
		flags_ |= LIGHT_INF;
	}
	void clearLightInf( void )
	{
		flags_ &= ~LIGHT_INF;
	}
	void setMountain( void )
	{
		flags_ |= MOUNTAIN;
	}
	void clearMountain( void )
	{
		flags_ &= ~MOUNTAIN;
	}
	void setSki( void )
	{
		flags_ |= SKI;
	}
	void clearSki( void )
	{
		flags_ &= ~SKI;
	}
	void setCamel( void )
	{
		flags_ |= CAMEL;
	}
	void clearCamel( void )
	{
		flags_ &= ~CAMEL;
	}

private:
	friend std::istream& operator>>( std::istream&, GroundUnit& );
	friend std::ostream& operator<<( std::ostream&, GroundUnit& );
public:
	struct WW2Corps // Corps Counter types; infantry, mixed, mechanized, tank
	{
		enum
		{
			INF		=	0,
			MXD,	//	1
			MEC,	//	2
			TNK		//	3
		};
	};
	struct WW2
	{
		enum
		{
			AIR_LNDG		=	0,
			AMPH_ARM,		//	1
			AMPH_ASLT_ENG,	//	2
			ARM,			//	3
			ART,			//	4
			ASLT_ENG,		//	5
			ASLT_ENG_TANK,	//	6
			ASLT_GUN,		//	7
			AT,				//	8
			BDR,			//	9
			BICYCLE,		//	10
			CAV,			//	11
			CMDO,			//	12
			CONS,			//	13
			ENG,			//	14
			ENG_TANK,		//	15
			FL_TANK,		//	16
			FORT,			//	17     // fortress
			HIGH_MOUNTAIN,	//	18   // !! HIGH_MTN is terraintype !!
			HV_AA,			//	19    // combat aa
			INF,			//	20
			LR_SIEGE_ART,	//	21
			LT_AA,			//	22     // combat aa
			LT_ARM,			//	23
			LT_INF,			//	24
			MARINE,			//	25
			MARINE_CMDO,	//	26
			MECH,			//	27
			MECH_CMDO,		//	28
			MG,				//	29
			MIXED,			//	30
			MTN,			//	31     // mountain
			MORTAR,			//	32
			MOTORCYCLE,		//	33
			NVL_TRPS,		//	34
			PARA,			//	35
			PARA_CMDO,		//	36
			PARA_INF,		//	37
			PARTISAN,		//	38
			PLTCL_POL,		//	39
			PL_CONS,		//	40
			PNTIVE_FORT,	//	41
			POL,			//	42     // police
			PORT_CONS,		//	43
			PORT_FORT,		//	44
			POS_HV_AA,		//	45     // positional (0-combat strength) heavy AA
			POS_LT_AA,		//	46     // positional (0-combat strength) light AA
			PUNITIVE,		//	47
			REPL,			//	48
			RES,			//	49	   // reserve
			RKT_ART,		//	50
			RR_ENG,			//	51
			SEC,			//	52
			SIEGE_ART,		//	53
			SKI,			//	54
			STC,			//	55	// static
			STURMPZ,		//	56
			TRNG,			//	57	// training
			TRUCK,			//	58
			V_WEAPONS,		//	59
			COASTAL,		//	60  // new in 1.0.2
			FORTAREA,		//	61  // new in 1.1.0
			SUPPGRP,		//	62  // new in 1.1.0 (support groups for WitD)
			TRANSPORT,		//	63  // new in 1.1.1 (movement counter)
			APC,			//	64  // new in 1.1.1 (movement counter)
			LVT,			//	65  // new in 1.1.1 (movement counter)
			RECON,			//	66  // new in 1.1.3 (just like lt armor)
			ALPINI,			//	67	// ww1 (new 2.1):
			ARMCAR,			//	68
			BERS,			//	69
			CHSSR,			//	70
			COASTART,		//	71
			FLAME,			//	72
			FLDART,			//	73
			FOOTART,		//	74
			GAR,			//	75 // garrison = static
			GASENG,			//	76
			GASFOOTART,		//	77
			GASHVART,		//	78
			GASHVMORT,		//	79
			HVART,			//	80
			HVCAV,			//	81
			HVMORT,			//	82
			HVSIEGEART,		//	83
			LTMORT,			//	84
			LTRIFLE,		//	85
			MARCH,			//	86
			MARRIFLE,		//	87
			MECHRIFLE,		//	88
			WW1_MG,			//	89 // rifle-mg
			MTDRIFLE,		//	90
			PRTSNCAV,		//	91
			PRTSNRIFLE,		//	92
			RIFLE,			//	93
			SIEGEENG,		//	94
			SHRPSHTR,		//	95
			SIEGEMORT,		//	96
			SNOWSHOE,		//	97
			STOSS,			//	98
			STOSSMG,		//	99
			STURM,			//	100
			HORS_ART,		//	101 // WOR horse art
			TANKFACT,		//	102 // tank factory
			ARTYFACT,		//	103 // artillery factory
			LT_ART,			//	104 // light artillery, special ATEC Soviets in Variant A
			UNITTYPECOUNT	//	105  // size of the UnitTypes[UNITTYPECOUNT] table (mappane.cpp)
		};
	};
	struct WW1
	{
		enum
		{

		};
	};

	GroundUnitType( )
	{
		flags_ = 0;
		hbmBitmap = 0;
	}
	~GroundUnitType( )
	{
	}

	std::string name_;						// e.g. "Infantry" or "Heavy AA", "Panzer"
	std::string shortname_;					// e.g. "Inf", "Hv AA", "Pz"
	MovementCategory movement_category_;	// MP_OTHER, MP_CM_ART, etc.
	std::string class_;						// unit class
	ArmorEffectsValue AECA_value_;			// FULL, HALF, NEUTRAL, THIRD, NONE
	ArmorEffectsValue AECD_value_;			//    -""-
	ArmorEffectsValue ATEC_value_;			//    -""-
	unsigned short flags_;					// 16 flags, see below
	wxBitmap* hbmBitmap;					// handle of bitmap of this unit

	// unit classes
	static std::string COMBAT_MOTORIZED_CLASS;
	static std::string CAVALRY_CLASS;
	static std::string ARTILLERY_CLASS;
	static std::string NON_MOTORIZED_CLASS;

	// unit type attributes
	static std::string HEAVY_EQUIP_ATTRIB;
	static std::string HEAVY_ARMS_ATTRIB;
	static std::string RAIL_ONLY_ATTRIB;
	static std::string COMBAT_ENGINGEER_ATTRIB;
	static std::string CONSTRUCTION_ATTRIB;
	static std::string WINTERIZED_ATTRIB;
	static std::string AMPHIBIOUS_ATTRIB;
	static std::string AIRDROP_ATTRIB;
	static std::string COMMANDO_ATTRIB;
	static std::string LIGHT_INFANTRY_ATTRIB;
	static std::string MOUNTAIN_ATTRIB;
	static std::string SKI_ATTRIB;
	static std::string CAMEL_ATTRIB;
};

#endif
#endif
