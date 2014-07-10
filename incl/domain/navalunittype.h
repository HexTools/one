#ifndef NAVALUNITTYPE_H
#define NAVALUNITTYPE_H

class NavalUnitType
{
public:
	struct Ship
	{
		enum
		{
			V		=	0,	// Fleet Carrier
			VE,		//	1	Escort Carrier
			VL,		//	2	Light Carrier
			VS,		//	3	Seaplane Carrier
			B,		//	4	Old (WWI) Battleships
			BB,		//	5	Battleship
			BC,		//	6	Battlecruiser
			BD,		//	7	Coast Defense Battleship
			BP,		//	8	Pocket Battleship
			CA,		//	9	Heavy Cruiser
			CC,		//	10	Coast Defense Cruiser
			CD,		//	11	Heavy Destroyer
			CL,		//	12	Light Cruiser
			DA,		//	13	AA Cruiser
			DC,		//	14	Obsolete Light Cruiser
			DD,		//	15	Destroyer
			DE,		//	16	Destroyer Escort
			DT,		//	17	Heavy Torpedo Boat
			FF,		//	18	Frigate
			GB,		//	19	Gunboat
			LC,		//	20	Landing Craft
			MW,		//	21	Minewarfare
			NT,		//	22	Naval Transport
			RF,		//	23	River Flotilla
			TB,		//	24	Torpedo Boat
			SC,		//	25	Coastal Submarine
			SS,		//	26	Sea going Submarine
			SO,		//	27	Ocean Going Submarine
			COUNT	//	28
		};
	};

	struct Fleet
	{
		enum
		{
			NTP		=	50,	// naval transport point(s)
			LC,		//	51 // landing craft(s)
			TF,		//	52 // task force
			CG,		//	53 // carrier group
			SS,		//	54 // submarine fleet
			RF,		//	55 // river flotilla
			BB,		//	56
			BP,		//	57
			BD,		//	58
			B,		//	59
			BC,		//	60
			CV,		//	61
			CA,		//	62
			CL,		//	63
			CD,		//	64
			DD		=	75,
			COUNT	=	16
		};
	};
	static const unsigned short FIRST_FLEET	= Fleet::NTP;
	static const unsigned short LAST_FLEET	= Fleet::DD;
};
#endif
