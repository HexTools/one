#ifndef HEXSIDETYPE_H
#define HEXSIDETYPE_H

class HexsideType
{
public:
	enum
	{
		// changes to this need to be synchronized with the indices in hexterrain.defs
		MINORRIVER 				  = 134,
		MAJORRIVER,				//	135
		ROAD,					//	136
		RAILROAD,				//	137
		LOW_VOL_RR,				//	138
		SEA_SIDE,				//	139  // ALLWATER
		MTN_SIDE,				//	140
		HIGH_MTN_SIDE,			//	141
		HIGH_MTN_PASS,			//	142
		ESCARPMENT,				//	143
		IMP_ESCARPMENT,			//	144
		FORTIFIED,				//	145
		BORDER,					//	146
		IBORDER,				//	147
		GLACIER_SIDE,			//	148
		SALT_DESERT_SIDE,		//	149
		WADI,					//	150
		LAKE_SIDE,				//	151
		RWATER_SIDE,			//	152  // restricted water
		KARST,					//	153
		CANAL,					//	154
		MINOR_CANAL,			//	155
		TRACK,					//	156  // ww2pac
		CLEAR_HEXSIDE,			//	157  // decoration only
		GREAT_WALL,				//	158
		ROUGH_SIDE,				//	159  // decoration only
		FOREST_SIDE,			//	160  // decoration only
		WOODS_SIDE,				//	161	// decoration only
		SEASONALRIVER,			//	162
		GREATRIVERFORD,			//	163
		MAJORRIVERFORD,			//	164
		ICING_RWATER_SIDE,		//	165 // ice counterpart to 152
		ICING_SEA_SIDE,			//	166  // ice counterpart to 139
		STANDALONEBRIDGE,		//	167
		LIMITED_STACKING_SIDE,	//	168
		DBL_ESCARPMENT,			//	169
		IMP_DBL_ESCARPMENT,		//	170
		MOTORTRACK,				//	171
		RAILTRACK,				//	172
		SUBIBORDER,				//	173
		DEMARCATION,			//	174
		MTN_SIDE2,				//	175	// treated like decorative, but also needed for effective terrain
		LAKESEA_FERRY,			//	176
		SHIP_CHANNEL,			//	177
		INLAND_SHIPPING,		//	178
		WATERWAY,				//	179
		WATERWAY_BAR,			//	180
		NARROW_STRAITS,			//	181
		NARROW_STRAITS_FERRY,	//	182
		WOODEDROUGH_SIDE,		//	183  // decoration only
		RAIL_FERRY,				//	184
		FILLING_RESERVOIR_SIDE,	//	185
		NOTHING					//	186  // no hexside feature, for infobar
	};
};

#endif
