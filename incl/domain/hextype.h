#ifndef HEXTYPE_H
#define HEXTYPE_H

/* hexterrain.defs in HexTools Play defines these for
 * indexing the mp[][] array:
 *
 * #define MP_CLEAR		0  // MPs in clear weather
 * #define MP_MUD		1  //         mud
 * #define MP_WINTER	2  //         winter
 *
 * #define MP_OTHER		0  // MPs for normal units
 * #define MP_CM_ART	1  //          motorized/artillery
 * #define MP_LIGHT		2  //          light inf
 * #define MP_CAV		3  //          cavalry
 * #define MP_MTN		4  //          mountain
 * #define MP_SKI		5  //          ski
 * #define MP_CAMEL		6  //		   camel
 */

// These are stored in a table, which is indexed by above-mentioned
// "enum" values. This is done for simplicity and speed. Hex has an index.
class HexType
{
public:
	enum
	{
		NONE,	// 				  = 0
		CLEAR,	//					1
		ROUGH,	//					2
		WOODS,	//					3
		WOODEDROUGH,	//			4
		FOREST,	//					5
		MOUNTAIN,		//			6
		SWAMP,	//					7
		SEA,	//					8   // sea/lake NOT subject to freezing
		LAKE,	//					9   // intermittent lake
		WOODEDLAKE,		//			10
		BOCAGE,	//					11
		CANALS,	//					12
		GLACIER,		//			13
		SWAMPYTUNDRA,	//			14
		RAVINES,		//			15
		RESTRICTEDWATERS,	//		16
		SALTDESERT,		//			17
		SALTLAKE,		//			18
		SALTMARSH,		//			19
		SAND,	//					20
		STONYDESERT,	//			21
		WOODEDSWAMP,	//			22
		JUNGLE,	//					23
		JUNGLEROUGH,	//			24
		JUNGLEMTN,		//			25
		JUNGLESWAMP,	//			26
		FILLINGRESERVOIR,	//		27
		ATOLL,	//					28
		SMALLISLAND,	//			29
		INTIRRIGATION,	//			30 // intensive irrgation
		EXTREMERAVINES,	//			31
		ICINGSEA,		//			32 // sea/lake subject to freezing
		SEASONALSWAMP,	//			33
		HEAVYBUSH,		//			34
		ROCKYTUNDRA,	//			35
		SPECIALSWAMP,	//			36
		SPECIALSEASONALSWAMP,	//	37
		ICINGRESTRICTEDWATERS,	//	38
		HIGHMOUNTAIN,	//			39
		// INSERT NEW TERRAIN ABOVE THIS LINE,
		// INCREMENT THE INDEX COMMENTS BELOW
		HEXTYPEKILL,	//			40
		HEXMASK,		//			41
		TERRAINTYPECOUNT		//	42
	};

	int mp_[ht::NUM_WEATHER_CLASSES][ht::NUM_MVMT_EFFECTS_COLUMNS];	// table for MPs
	int drm_[ht::NUM_WEATHER_CLASSES];	// defensive DRM: 0, -1, -2
	unsigned int defense_flags_;	// flags like, "attacker halved", see below
	const char* name_;				// "clear" etc.
	wxBitmap* hbmBitmap;			// bitmap handle for wx	(CLEAR)
	// Note: HexTools Map does not use the weather-affected hex terrain bitmaps
#if defined HEXTOOLSPLAY
	wxBitmap* hbmMudBitmap;
	wxBitmap* hbmSnowBitmap;
	wxBitmap* hbmFrostBitmap;
#endif

	HexType( );
	~HexType( );
};

#endif
