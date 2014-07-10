#ifndef HEX_H
#define HEX_H

class HexType;
class City;

class Hex
{
	friend std::istream& operator>>( std::istream&, Hex& );
#if defined HEXTOOLSMAP
	friend std::ostream& operator<<( std::ostream&, Hex& );
#endif
public:
	// hex parts, used as flags for rivers, roads, etc.
	static const unsigned char NORTHWEST	= 0x01;
	static const unsigned char NORTHEAST	= 0x02;
	static const unsigned char EAST			= 0x04;
	static const unsigned char SOUTHEAST	= 0x08;
	static const unsigned char SOUTHWEST	= 0x10;
	static const unsigned char WEST			= 0x20;
	static const unsigned char LAST_DIR		= WEST;

	// hex vertices, used as flags for dams, etc.
	static const unsigned char NORTHWEST_CORNER	= 0x01;
	static const unsigned char NORTH_CORNER		= 0x02;
	static const unsigned char NORTHEAST_CORNER	= 0x04;
	static const unsigned char SOUTHEAST_CORNER	= 0x08;
	static const unsigned char SOUTH_CORNER		= 0x10;
	static const unsigned char SOUTHWEST_CORNER	= 0x20;

	static const unsigned char LOMASK	= 0x07;	// 00000111
	static const unsigned char HIMASK	= 0x38;	// 00111000
	static const unsigned char DATAMASK	= 0xc0;	// 11000000

	static const unsigned char SIZE_X	= 32;	// size of one hex in pixels
	static const unsigned char SIZE_Y	= 32;
	static const unsigned char HALF		= 16;
	static const unsigned char THIRD	= 10;	// close enough... (only used in GetHexPart)
	static const unsigned char TWOTHIRDS= 21;
	static const unsigned char THREEFOURTHS= 24;// 3/4 hexsize
	static const unsigned char QUARTER	= 8;	// hexsize/4

	static const unsigned short DEF_INF_HALVED 		= 0x001;	// infantry attacker halved
	static const unsigned short DEF_CM_HALVED		= 0x002;	// motorized attacker halved
	static const unsigned short DEF_GS_HALVED		= 0x004;	// ground support halved
	static const unsigned short DEF_ART_HALVED		= 0x008;	// artillery halved
	static const unsigned short DEF_MTN_HALVED		= 0x010;	// mountain units halved
	static const unsigned short DEF_ATT_QUARTERED	= 0x020;	// attacker quartered
	static const unsigned short DEF_NO_AEC			= 0x040;	// no AEC
	static const unsigned short DEF_FREEZES			= 0x080;	// freezes in winter weather -> flags=none
	static const unsigned short DEF_NO_EFFECT		= 0x100;	// no effects
	static const unsigned short DEF_SG_ART_DOUBLED	= 0x200;	// siege artillery doubled (forts & cities)
	static const unsigned short DEF_NO_RETREAT		= 0x400;	// defender ignores retreat (westwall)
	static const unsigned short DEF_PROHIBITED		= 0x800;	// attacking prohibited

	unsigned char terrain_;		// index to "TerrainTypes" table

private:
	// below are the hexsides and features bits
	// DATA corresponds to	bits 7..6	11000000
	// HI corresponds to	bits 5..3	00111000
	// LO corresponds to	bits 2..0	00000111
	// HI and LO are given in W-SW-SE order (hi-to-lo bit)

	unsigned char border_;		// 7: rail ferry SE, 6: limited stack. hex flag, HI: sea district id (3-5), LO: sea district id (0-2)
	unsigned char roads_;		// DATA: WZ (1..0 lo bits), HI: railroad, LO: road
	unsigned char rivers_;		// DATA: WZ (3..2 mid bits), HI: major river, LO: minor river
	unsigned char mountain_;		// 7: rare flag, 6: sea district id (6), HI: high mtn, LO: mtn
	unsigned char high_mountain_pass_;	// DATA: limited stacking W-SW, HI: karst, LO: high mtn pass
	unsigned char escarpment_;	// DATA: impass. esc. NE-NW, HI-LO: esc. W-SW-SE-E-NE-NW :: impass. esc. & esc. == fortified hexside
	unsigned char water_;		// DATA: port size (3..2 hi bits), HI: narrow strait, LO: sea (no ice)
	unsigned char wadi_;			// DATA: port size (1..0 lo bits), HI: salt desert, LO: wadi
	unsigned char ice_fortif_;	// 7: interior flag, 6..3: impass. esc. W-SW-SE-E, LO: glacier
	unsigned char misc_;			// 7..5: port symbol location, 4..2: lake/great river, 1..0: port attrib

	unsigned short hex_id_;	// hex id: 0..9999		==> 2 bytes
	unsigned short map_id_;	// map id: 0.99[A..Z]	==> 2 bytes

	unsigned char misc2_;		// 7: inland waters flag, 6: late ice flag, HI: standalone bridge, LO: pipeline
	unsigned char canals_;		// 7: seasonal river SE, 6: great wall flag, HI: minor canal, LO: canal
	unsigned char track_;		// DATA: WZ (5..4 hi bits), HI: clear ornament, LO: track
	unsigned char ornament_;		// 7: sea (ice) SE, 6: limited stacking SE, HI: forest ornament, LO: rough ornament :: LO & HI == woods ornament
	unsigned char int_mtn_RMY_;	// DATA: inland route bar W-SW, HI: waterway route, LO: interior mtn NW-NE-E
	unsigned char int_mtn_intr_AF_;	// DATA: intrinsic AF capacity, HI: intrinsic AF symbol loc, LO: interior mtn SE-SW-W
	unsigned char fordable_water_;// DATA: seasonal river W-SW, HI: fordable major river, LO: fordable great river
	unsigned char restricted_water_; // DATA: sea (ice) W-SW, 5: restricted waters, 4: route flag, 3: >>>AVAILABLE<<<, LO: restricted waters hexside
	unsigned char dbl_escarpment_;// DATA: dam vertices W-SW, HI: impass. double esc., LO: double esc.
	unsigned char border2_;		// DATA: rail ferry  W-SW, HI: sea district id (13-15), LO: sea district id (10-12)
	unsigned char inland_net_WMD_;	// 7: inland route bar SE, 6: wooded rough ornament SE, HI: inland shipping, LO: sea district id (7-9)
	unsigned char lake_sea_routes_;// DATA: wooded rough ornament W-SW, HI: lake/sea ferry, LO: shipping channel
	unsigned char filling_res_;		// DATA: >>>AVAILABLE<<<, HI: filling reservoir, LO: >>>AVAILABLE<<<

	unsigned short district_id_;	// land district id: 256..65536	==> 2 bytes

public:
	City* city_;

	Hex( );
	~Hex( );

	// used for hexside kill functionality
#if defined HEXTOOLSMAP
	void clearHexsideData( void )
	{
		mountain_ = 0;
		high_mountain_pass_ = 0;
		ice_fortif_ = 0;
		escarpment_ = 0;
		wadi_ = 0;
		misc_ = 0;
		misc2_ = 0;
		water_ = 0;
		canals_ = 0;
		rivers_ = 0;
		roads_ = 0;
		track_ = 0;
		ornament_ = 0;
		int_mtn_RMY_ = 0;
		int_mtn_intr_AF_ = 0;
		fordable_water_ = 0;
		restricted_water_ = 0;
		dbl_escarpment_ = 0;
		inland_net_WMD_ = 0;
		border_ = 0;
		border2_ = 0;
		lake_sea_routes_ = 0;
		filling_res_ = 0;
	}
#endif
	void updateHexFlags( void );
	void updateRareHexsidesFlag( void );
	void updateInteriorHexsidesFlag( void );
	void updateRouteHexsidesFlag( void );
	void updateInlandWaterHexsidesFlag( void );

	int getMapIdWord( )
	{
		return map_id_;
	}
#if defined HEXTOOLSMAP
	void setMapIdWord( int i )
	{
		map_id_ = i;
	}
#endif
	void getMapIdStr( char* decoded_string )
	{
		Hex::decodeMapId( decoded_string, map_id_ );
	}
#if defined HEXTOOLSMAP
	void setMapIdStr( const char* cp )
	{
		map_id_ = Hex::encodeMapIdStr( cp );
	}
#endif
	static const char* convertMapIdEncoding( unsigned short mapId );
	void decodeMapId( char* decoded_string, unsigned short mapId );
	static unsigned short encodeMapIdStr( const char* cp );

	static bool validateMapIdStr( wxString* cp );
	static bool validateMapIdFirstPosition( char c );
	static bool validateMapIdSecondPosition( char c );
	static bool validateMapIdThirdPosition( char c, wxString* mapIdStr );
	static bool validateMapIdFourthPosition( char c );
	static bool validateMapIdLastPosition( char c );

	int getHexNumber( void ) // e.g. 211
	{
		return hex_id_;
	}
#if defined HEXTOOLSMAP
	void setHexNumber( int n )
	{
		hex_id_ = n < 0 ? 0 : n;
	}
#endif
	void getHexIdStr( char* cp ) // e.g. "0211"
	{
		sprintf( cp, "%04d", getHexNumber( ) );
	}
#if defined HEXTOOLSMAP
	void getHexId( char* cp ) // e.g. "11B:0211"
	{
		getMapIdStr( cp );
		cp[ strlen( cp ) ] = ':'; // replace map ID null terminator w/ colon
		getHexIdStr( &cp[ strlen( cp ) + 1 ] );
	}
#endif
/*
	// internal borders (LO)
	int hasInternalBorders( void )
	{
		return bBorder & LOMASK;
	}
	int getInternalBorder( int side, int x_max, int y_max )
	{
		return ( bBorder << 3 ) & HIMASK & side;
	}
*/
#if defined HEXTOOLSMAP
/*
	void toggleInternalBorder( int side )
	{
		bBorder ^= ( side >> 3 );
	}
	void setInternalBorder( int side )
	{
		bBorder |= ( side >> 3 );
	}
	void clrInternalBorder( int side )
	{
		bBorder &= ~( side >> 3 );
	}
*/
#endif
/*
	// international borders (HI)
	int hasInternationalBorders( void )
	{
		return bBorder & HIMASK;
	}
	int getInternationalBorder( int side )
	{
		return bBorder & HIMASK & side;
	}
*/
#if defined HEXTOOLSMAP
/*
	void toggleInternationalBorder( int side )
	{
		bBorder ^= ( side & HIMASK );
	}
	void setInternationalBorder( int side )
	{
		bBorder |= ( side & HIMASK );
	}
	void clrInternationalBorder( int side )
	{
		bBorder &= ~( side & HIMASK );
	}
*/
#endif
/*
	// internal sub-borders (LO)
	int hasInternalSubBorders( void )
	{
		return bBorder2 & LOMASK;
	}
	int getInternalSubBorder( int side )
	{
		return ( bBorder2 << 3 ) & HIMASK & side;
	}
*/
#if defined HEXTOOLSMAP
/*
	void toggleInternalSubBorder( int side )
	{
		bBorder2 ^= ( side >> 3 );
	}
	void setInternalSubBorder( int side )
	{
		bBorder2 |= ( side >> 3 );
	}
	void clrInternalSubBorder( int side )
	{
		bBorder2 &= ~( side >> 3 );
	}
*/
#endif
/*
	// demarkation lines (HI), e.g. Axis-Soviet start line
	int hasDemarkationLines( void )
	{
		return bBorder2 & HIMASK;
	}
	int getDemarkationLine( int side )
	{
		return bBorder2 & HIMASK & side;
	}
*/
#if defined HEXTOOLSMAP
/*
	void toggleDemarkationLine( int side )
	{
		bBorder2 ^= ( side & HIMASK );
	}
	void setDemarkationLine( int side )
	{
		bBorder2 |= ( side & HIMASK );
	}
	void clrDemarkationLine( int side )
	{
		bBorder2 &= ~( side & HIMASK );
	}
*/
#endif
#if defined HEXTOOLSMAP
/*
	void clrAllBorders( int side )
	{
		clrInternationalBorder( side );
		clrInternalBorder( side );
		clrInternalSubBorder( side );
		clrDemarkationLine( side );
	}
*/
#endif
	// SW (GUI calls this "South") vertex dam stored on bit 6
	// W (GUI calls this "Southwest") vertex dam stored on bit 7
	int hasDams( void )
	{
		return ( dbl_escarpment_ & DATAMASK ) >> 2;
	}
	int getDam( int v )
	{
		return ( dbl_escarpment_ & ( ( v << 2 ) & DATAMASK ) ) >> 2;
	}
#if defined HEXTOOLSMAP
	void setDam( int v )
	{
		dbl_escarpment_ |= ( ( v << 2 ) & DATAMASK );
	}
	void clrDam( int v )
	{
		dbl_escarpment_ &= ~( ( v << 2 ) & DATAMASK );
	}
#endif
	// see hexfeature.defs: ANCHORAGE through GREAT
#if defined HEXTOOLSMAP
	void setPortSize( int size )
	{
		wadi_ &= ~DATAMASK;  // clear bits 6-7
		wadi_ |= ( ( size & 0x3 ) << 6 ); // set 0-1 of size to 6-7 of bWadi
		water_ &= ~DATAMASK;  // clear bits 6-7
		water_ |= ( ( ( size >> 2 ) & 0x3 ) << 6 ); // set 2-3 of size to 6-7 of bWater
	}
#endif
	int getPortSize( void )
	{
		return ( ( wadi_ >> 6 ) | ( ( water_ >> 6 ) << 2 ) );
	}

#if defined HEXTOOLSMAP
	void setIntrinsicAFCapacity( int type )
	{
		int_mtn_intr_AF_ &= ~DATAMASK;
		int_mtn_intr_AF_ |= ( type << 6 );
	}
#endif
	int getIntrinsicAFCapacity( void )
	{
		return ( int_mtn_intr_AF_ >> 6 );
	}

#if defined HEXTOOLSMAP
	void setIntrinsicAFSymbolDir( int dir )
	{
		int v;
		switch ( dir )
		{
			case EAST:
				v = 1;
				break;
			case WEST:
				v = 2;
				break;
			case NORTHEAST:
				v = 3;
				break;
			case NORTHWEST:
				v = 4;
				break;
			case SOUTHEAST:
				v = 5;
				break;
			case SOUTHWEST:
				v = 6;
				break;
			default:
				v = 0;
				break;
		}
		int_mtn_intr_AF_ &= ~HIMASK;
		int_mtn_intr_AF_ |= ( ( v << 3 ) & HIMASK );
	}
#endif
	int getIntrinsicAFSymbolDir( void )
	{
		int v = ( int_mtn_intr_AF_ & HIMASK ) >> 3;
		switch ( v )
		{
			case 1:
				return EAST;
			case 2:
				return WEST;
			case 3:
				return NORTHEAST;
			case 4:
				return NORTHWEST;
			case 5:
				return SOUTHEAST;
			case 6:
				return SOUTHWEST;
			default:
				return 0;
		}
	}

	int hasWaterwayRoutes( void )
	{
		return int_mtn_RMY_ & HIMASK;
	}
	int getWaterwayRoute( int side )
	{
		return int_mtn_RMY_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setWaterwayRoute( int side )
	{
		int_mtn_RMY_ |= ( side & HIMASK );
	}
	void clrWaterwayRoute( int side )
	{
		int_mtn_RMY_ &= ~( side & HIMASK );
	}
#endif
	// bars to inland routes are uppermost bit of inland_net_WMD_ and upper 2 bits of bIntMtnRMY
	int hasInlandRouteBars( void )
	{
		// xx000000 --> 00xx0000 & 00110000 plus y0000000 --> 0000y000 & 00001000 is 00xxy000
		return ( ( int_mtn_RMY_ >> 2 ) & 0x30 ) | ( ( inland_net_WMD_ >> 4 ) & 0x08 );
	}
	int getInlandRouteBar( int side )
	{
		return ( ( ( int_mtn_RMY_ >> 2 ) & 0x30 ) | ( ( inland_net_WMD_ >> 4 ) & 0x08 ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setInlandRouteBar( int side )
	{
		//00abc000 --> c0000000 & 10000000 sets c0000000
		inland_net_WMD_ |= ( ( side << 4 ) & 0x80 );
		//00abc000 --> abc00000 & 11000000 sets ab000000
		int_mtn_RMY_ |= ( ( side << 2 ) & DATAMASK );
	}
	void clrInlandRouteBar( int side )
	{
		inland_net_WMD_ &= ~( ( side << 4 ) & 0x80 );
		int_mtn_RMY_ &= ~( ( side << 2 ) & DATAMASK );
	}
#endif
	int hasInlandShippingChannels( void )
	{
		return inland_net_WMD_ & HIMASK;
	}
	int getInlandShippingChannel( int side )
	{
		return inland_net_WMD_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setInlandShippingChannel( int side )
	{
		inland_net_WMD_ |= ( side & HIMASK );
	}
	void clrInlandShippingChannel( int side )
	{
		inland_net_WMD_ &= ~( side & HIMASK );
	}
#endif

#if defined HEXTOOLSMAP
	void setPortAttribute( int type )
	{
		misc_ &= ~0x03;	// clear bits 0-1
		misc_ |= ( type & 0x03 );
	}
#endif
	int getPortAttribute( void )
	{
		return ( misc_ & 0x03 );
	}

#if defined HEXTOOLSMAP
	void setPortSymbolDir( int dir )
	{
		unsigned char v;
		switch ( dir )
		{
			case EAST:
				v = 1;
				break;
			case WEST:
				v = 2;
				break;
			case NORTHEAST:
				v = 3;
				break;
			case NORTHWEST:
				v = 4;
				break;
			case SOUTHEAST:
				v = 5;
				break;
			case SOUTHWEST:
				v = 6;
				break;
			default:
				v = 0;
				break;
		}
		misc_ &= ~0xe0;	// clear upper 3 bits
		misc_ |= ( v << 5 ); // stuff int v into bits 5-7 of bMisc
	}
#endif
	int getPortSymbolDir( void )
	{
		int v = misc_ >> 5;
		switch ( v )
		{
			case 1:
				return EAST;
			case 2:
				return WEST;
			case 3:
				return NORTHEAST;
			case 4:
				return NORTHWEST;
			case 5:
				return SOUTHEAST;
			case 6:
				return SOUTHWEST;
			default:
				return 0;
		}
	}

	// 8 == SE; 16 == SW; 32 == W
	// side = 8, 16 or 32 or 00111000
	// Usage: setRoad(WEST); if (Hex.setRoad(SOUTHEAST)) ...

	// minor rivers (LO)
	int hasMinorRivers( void )
	{
		return rivers_ & LOMASK;
	}
	int getMinorRiver( int side )
	{
		return ( rivers_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setMinorRiver( int side )
	{
		rivers_ |= ( side >> 3 );
	}
	void clrMinorRiver( int side )
	{
		rivers_ &= ~( side >> 3 );
	}
#endif
	// major rivers (HI)
	int hasMajorRivers( void )
	{
		return rivers_ & HIMASK;
	}
	int getMajorRiver( int side )
	{
		return rivers_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setMajorRiver( int side )
	{
		rivers_ |= ( side & HIMASK );
	}
	void clrMajorRiver( int side )
	{
		rivers_ &= ~( side & HIMASK );
	}
#endif
	// fordable great rivers / lakes (LO)
	int hasFordableGreatRivers( void )
	{
		return fordable_water_ & LOMASK;
	}
	int getFordableGreatRiver( int side )
	{
		return ( fordable_water_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setFordableGreatRiver( int side )
	{
		fordable_water_ |= ( side >> 3 );
	}
	void clrFordableGreatRiver( int side )
	{
		fordable_water_ &= ~( side >> 3 );
	}
#endif
	// fordable major rivers (HI)
	int hasFordableMajorRivers( void )
	{
		return fordable_water_ & HIMASK;
	}
	int getFordableMajorRiver( int side )
	{
		return fordable_water_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setFordableMajorRiver( int side )
	{
		fordable_water_ |= ( side & HIMASK );
	}
	void clrFordableMajorRiver( int side )
	{
		fordable_water_ &= ~( side & HIMASK );
	}
#endif
	// limited stacking indicators are bit 6 of bOrnament and upper 2 bits of bHighMtnPass
	int hasLimitedStackingSide( void )
	{
		// xx000000 --> 00xx0000 & 00110000 plus 0y000000 --> 0000y000 & 00001000 is 00xxy000
		return ( ( ( high_mountain_pass_ >> 2 ) & 0x30 ) | ( ( ornament_ >> 3 ) & 0x08 ) );
	}
	int getLimitedStacking( int side )
	{
		return ( ( ( high_mountain_pass_ >> 2 ) & 0x30 ) | ( ( ornament_ >> 3 ) & 0x08 ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setLimitedStacking( int side )
	{
		//00abc000 --> 0c000000 & 01000000 sets 0c000000
		ornament_ |= ( ( side << 3 ) & 0x40 );
		//00abc000 --> abc00000 & 11000000 sets ab000000
		high_mountain_pass_ |= ( ( side << 2 ) & DATAMASK );
	}
	void clrLimitedStacking( int side )
	{
		ornament_ &= ~( ( side << 3 ) & 0x40 );
		high_mountain_pass_ &= ~( ( side << 2 ) & DATAMASK );
	}
#endif
	// seasonal rivers are uppermost bit of bCanals and upper 2 bits of fordable_water_
	int hasSeasonalRivers( void )
	{
		// xx000000 --> 00xx0000 & 00110000 plus y0000000 --> 0000y000 & 00001000 is 00xxy000
		return ( ( ( fordable_water_ >> 2 ) & 0x30 ) | ( ( canals_ >> 4 ) & 0x08 ) );
	}
	int getSeasonalRiver( int side )
	{
		return ( ( ( fordable_water_ >> 2 ) & 0x30 ) | ( ( canals_ >> 4 ) & 0x08 ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setSeasonalRiver( int side )
	{
		//00abc000 --> c0000000 & 10000000 sets c0000000
		canals_ |= ( ( side << 4 ) & 0x80 );
		//00abc000 --> abc00000 & 11000000 sets ab000000
		fordable_water_ |= ( ( side << 2 ) & DATAMASK );
	}
	void clrSeasonalRiver( int side )
	{
		canals_ &= ~( ( side << 4 ) & 0x80 );
		fordable_water_ &= ~( ( side << 2 ) & DATAMASK );
	}
#endif
	// canals (LO)
	int hasCanals( void )
	{
		return canals_ & LOMASK;
	}
	int getCanal( int side )
	{
		return ( canals_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setCanal( int side )
	{
		canals_ |= ( side >> 3 );
	}
	void clrCanal( int side )
	{
		canals_ &= ~( side >> 3 );
	}
#endif
	// minor canals (HI)
	int hasMinorCanals( void )
	{
		return canals_ & HIMASK;
	}
	int getMinorCanal( int side )
	{
		return canals_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setMinorCanal( int side )
	{
		canals_ |= ( side & HIMASK );
	}
	void clrMinorCanal( int side )
	{
		canals_ &= ~( side & HIMASK );
	}
#endif
	int hasInlandWaters( void )
	{
		return hasMinorRivers( )
				| hasMajorRivers( ) | hasLakeSides( )
				| hasFordableMajorRivers( ) | hasFordableGreatRivers( )
				| hasCanals( ) | hasMinorCanals( );
	}

	int hasInlandWater( int side )
	{
		return getMinorRiver( side )
				| getMajorRiver( side ) | getLakeSide( side )
				| getFordableMajorRiver( side ) | getFordableGreatRiver( side )
				| getCanal( side ) | getMinorCanal( side );
	}

	// mountain   (LO)
	int hasMountains( void )
	{
		return mountain_ & LOMASK;
	}
	int getMountain( int side )
	{
		return ( mountain_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setMountain( int side )
	{
		mountain_ |= ( side >> 3 );
	}
	void clrMountain( int side )
	{
		mountain_ &= ~( side >> 3 );
	}
#endif
	// high mountain  (HI)
	int hasHighMtns( void )
	{
		return mountain_ & HIMASK;
	}
	int getHighMtn( int side )
	{
		return mountain_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setHighMtn( int side )
	{
		mountain_ |= ( side & HIMASK );
	}
	void clrHighMtn( int side )
	{
		mountain_ &= ~( side & HIMASK );
	}
#endif
	// high mtn pass  (LO)
	int hasHighMtnPasses( void )
	{
		return high_mountain_pass_ & LOMASK;
	}
	int getHighMtnPass( int side )
	{
		return ( high_mountain_pass_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setHighMtnPass( int side )
	{
		high_mountain_pass_ |= ( side >> 3 );
	}
	void clrHighMtnPass( int side )
	{
		high_mountain_pass_ &= ~( side >> 3 );
	}
#endif
	// karst	(HI)
	int hasKarsts( void )
	{
		return high_mountain_pass_ & HIMASK;
	}
	int getKarst( int side )
	{
		return high_mountain_pass_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setKarst( int side )
	{
		high_mountain_pass_ |= ( side & HIMASK );
	}
	void clrKarst( int side )
	{
		high_mountain_pass_ &= ~( side & HIMASK );
	}
#endif
	// filling reservoir	(HI)
	int hasFillingReservoirs( void )
	{
		return filling_res_ & HIMASK;
	}
	int getFillingReservoir( int side )
	{
		return filling_res_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setFillingReservoir( int side )
	{
		filling_res_ |= ( side & HIMASK );
	}
	void clrFillingReservoir( int side )
	{
		filling_res_ &= ~( side & HIMASK );
	}
#endif
	// TODO: these Water functions are not used anywhere
/* returns
	 0 000 no water
	 1 001 sea = bWater: LO								00000111 << 3
	 2 010 lake = bMisc: 4..2							00011100 << 1
	 3 011 restricted sea = bRestricted: LO				00000111 << 3
	 4 100 icing sea = bRestricted: DATA | bOrnament: 7 11000000 >> 2 | 10000000 >> 4
	 6 110 restricted icing sea = bRestricted: HI		00111000
*/
/*
	// TODO: included fordable great river here?
	int getWater( int side )
	{
		if ( getSeaSide( side ) )					return 0x01;
		if ( getLakeSide( side ) )					return 0x02;
		if ( getIcingSeaSide( side ) )				return 0x04;
		return 0;
	}
*/
#if defined HEXTOOLSMAP
/*
	// TODO: included fordable great river here?
	void setWater( int side, int type )
	{
		clrWater( side );
		if		( type == 0 )	return;
		else if ( type & 0x01 )	setSeaSide( side );
		else if	( type & 0x02 )	setLakeSide( side );
		else if ( type & 0x04 )	setIcingSeaSide( side );
	}
	// TODO: included fordable great river here?
	void clrWater( int side )
	{
		clrSeaSide( side );
		clrLakeSide( side );
		clrIcingSeaSide( side );
	}
*/
#endif
#if defined HEXTOOLSMAP
	void clrAllOrnaments( int side )
	{
		track_ &= ~( side & HIMASK );
		ornament_ &= ~( side >> 3 );
		ornament_ &= ~( side & HIMASK );
		inland_net_WMD_ &= ~( ( side << 3 ) & 0x40 );
		lake_sea_routes_ &= ~( ( side << 2 ) & DATAMASK );
	}
#endif
	// clear (HI)
	int hasClearOrnaments( void )
	{
		return track_ & HIMASK;
	}
	int getClearOrnament( int side )
	{
		return track_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setClearOrnament( int side )
	{
		track_ |= ( side & HIMASK );
	}
	void clrClearOrnament( int side )
	{
		track_ &= ~( side & HIMASK );
	}
#endif
	// rough sides
	int hasRoughOrnaments( void )
	{
		return ornament_ & LOMASK;
	}
	int getRoughOrnament( int side )	// rough and NOT forest
	{
		return ( ( ornament_ & ( side >> 3 ) ) << 3 ) && ! ( ornament_ & ( side & HIMASK ) );
	}
#if defined HEXTOOLSMAP
	void setRoughOrnament( int side )
	{
		ornament_ |= ( side >> 3 );
	}
	void clrRoughOrnament( int side )
	{
		ornament_ &= ~( side >> 3 );
	}
#endif
	// woods side: both rough & forest
	int hasWoodsOrnaments( void )
	{
		return ( ornament_ << 3 ) & ornament_ & HIMASK;
	}
	int getWoodsOrnament( int side )
	{
		return ( ornament_ << 3 ) & ornament_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setWoodsOrnament( int side )
	{
		ornament_ |= ( side >> 3 );
		ornament_ |= ( side & HIMASK );
	}
	void clrWoodsOrnament( int side )
	{
		ornament_ &= ~( side >> 3 );
		ornament_ &= ~( side & HIMASK );
	}
#endif
	// forest sides
	int hasForestOrnaments( void )
	{
		return ornament_ & HIMASK;
	}
	int getForestOrnament( int side )	// forest and NOT rough
	{
		return ( ornament_ & ( side & HIMASK ) ) && ( ! ( ornament_ & ( side >> 3 ) ) << 3 );
	}
#if defined HEXTOOLSMAP
	void setForestOrnament( int side )
	{
		ornament_ |= ( side & HIMASK );
	}
	void clrForestOrnament( int side )
	{
		ornament_ &= ~( side & HIMASK );
	}
#endif
	// wooded rough sides
	// wooded rough decorations are bit 6 of inland_net_WMD_ and upper 2 bits of bLakeSeaRoutes
	int hasWoodedRoughOrnaments( void )
	{
		// xx000000 --> 00xx0000 & 00110000 plus 0y000000 --> 0000y000 & 00001000 is 00xxy000
		return ( ( ( lake_sea_routes_ >> 2 ) & 0x30 ) | ( ( inland_net_WMD_ >> 3 ) & 0x08 ) );
	}
	int getWoodedRoughOrnament( int side )
	{
		return ( ( ( lake_sea_routes_ >> 2 ) & 0x30 ) | ( ( inland_net_WMD_ >> 3 ) & 0x08 ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setWoodedRoughOrnament( int side )
	{
		//00abc000 --> 0c000000 & 01000000 sets 0c000000
		inland_net_WMD_ |= ( ( side << 3 ) & 0x40 );
		//00abc000 --> abc00000 & 11000000 sets ab000000
		lake_sea_routes_ |= ( ( side << 2 ) & DATAMASK );
	}
	void clrWoodedRoughOrnament( int side )
	{
		inland_net_WMD_ &= ~( ( side << 3 ) & 0x40 );
		lake_sea_routes_ &= ~( ( side << 2 ) & DATAMASK );
	}
#endif
	bool isWaterHex( void )
	{
		return ( terrain_ == HexType::SEA || terrain_ == HexType::ICINGSEA || terrain_ == HexType::ATOLL || terrain_ == HexType::SMALLISLAND );
	}

	int hasAnyOpenWater( void )
	{
		return ( water_ & LOMASK )															// sea
				| ( ( ( restricted_water_ >> 2 ) & 0x30 ) | ( ( ornament_ >> 4 ) & 0x08 ) );	// icing sea
	}

	int hasOpenWater( int side )
	{
		return ( getSeaSide( side ) || getIcingSeaSide( side ) );
	}

	int hasAnyWater( void )
	{
		return ( water_ & LOMASK )					// sea
				| ( misc_ & 0x1c ) 					// lake/great river
				| ( ( ( restricted_water_ >> 2 ) & 0x30 ) | ( ( ornament_ >> 4 ) & 0x08 ) // icing sea
				| ( fordable_water_ & LOMASK )		// fordable great river
				);
	}

	int hasWaterSide( int side )
	{
		return ( getSeaSide( side )
				|| getLakeSide( side )
				|| getIcingSeaSide( side )
				|| getFordableGreatRiver( side )
				);
	}

	int hasIcingSeaSides( void )
	{
		return ( ( restricted_water_ >> 2 ) & 0x30 ) | ( ( ornament_ >> 4 ) & 0x08 );
	}
	// icing sea is uppermost bit of bOrnament and upper 2 bits of restricted_water_
	int getIcingSeaSide( int side )
	{
		return ( ( ( restricted_water_ >> 2 ) & 0x30 ) | ( ( ornament_ >> 4 ) & 0x08 ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setIcingSeaSide( int side )
	{
		//00abc000 --> c0000000 & 10000000 sets c0000000
		ornament_ |= ( ( side << 4 ) & 0x80 );
		//00abc000 --> abc00000 & 11000000 sets ab000000
		restricted_water_ |= ( ( side << 2 ) & DATAMASK );
	}
	void clrIcingSeaSide( int side )
	{
		ornament_ &= ~( ( side << 4 ) & 0x80 );
		restricted_water_ &= ~( ( side << 2 ) & DATAMASK );
	}
#endif
	// restricted water (LO: restricted_water_)
	int hasRestrictedWaterSides( void )
	{
		return ( restricted_water_ << 3 ) & HIMASK;
	}
	int getRestrictedWaterSide( int side )
	{
		return ( restricted_water_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setRestrictedWaterSide( int side )
	{
		restricted_water_ |= ( side >> 3 );
	}
	void clrRestrictedWaterSide( int side )
	{
		restricted_water_ &= ~( side >> 3 );
	}
#endif
	// sea (LO: bWater)
	int hasSeaSides( void )
	{
		return ( water_ << 3 ) & HIMASK;
	}
	int getSeaSide( int side )
	{
		return ( water_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setSeaSide( int side )
	{
		water_ |= ( side >> 3 );
	}
	void clrSeaSide( int side )
	{
		water_ &= ~( side >> 3 );
	}
#endif
	// lake		bMisc: 2-4
	int hasLakeSides( void )
	{
		return misc_ & 0x1c; // was 0x38
	}
	int getLakeSide( int side )
	{
		return ( misc_ << 1 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setLakeSide( int side )
	{
		misc_ |= ( ( side & HIMASK ) >> 1 );  // bits 00xx x000 --> 000x xx00
	}
	void clrLakeSide( int side )
	{
		misc_ &= ~( ( side & HIMASK ) >> 1 );
	}
#endif
	// double escarpments (LO)
	int hasDoubleEscarpments( void )
	{
		return dbl_escarpment_ & LOMASK;
	}
	int getDoubleEscarpment( int side )
	{
		return ( dbl_escarpment_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setDoubleEscarpment( int side )
	{
		dbl_escarpment_ |= ( side >> 3 );
	}
	void clrDoubleEscarpment( int side )
	{
		dbl_escarpment_ &= ~( side >> 3 );
	}
#endif
	// impassable double escarpments (HI)
	int hasImpassableDoubleEscarpments( void )
	{
		return dbl_escarpment_ & HIMASK;
	}
	int getImpassableDoubleEscarpment( int side )
	{
		return dbl_escarpment_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setImpassableDoubleEscarpment( int side )
	{
		dbl_escarpment_ |= ( side & HIMASK );
	}
	void clrImpassableDoubleEscarpment( int side )
	{
		dbl_escarpment_ &= ~( side & HIMASK );
	}
#endif
	// wadi      (LO)
	int hasWadis( void )
	{
		return wadi_ & LOMASK;
	}
	int getWadi( int side )
	{
		return ( wadi_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setWadi( int side )
	{
		wadi_ |= ( side >> 3 );
	}
	void clrWadi( int side )
	{
		wadi_ &= ~( side >> 3 );
	}
#endif
	// salt desert  (HI)
	int hasSaltDeserts( void )
	{
		return wadi_ & HIMASK;
	}
	int getSaltDesert( int side )
	{
		return wadi_ & ( side & HIMASK );
	}
#if defined HEXTOOLSMAP
	void setSaltDesert( int side )
	{
		wadi_ |= ( side & HIMASK );
	}
	void clrSaltDesert( int side )
	{
		wadi_ &= ~( side & HIMASK );
	}
#endif
	// glacier  (LO)
	int hasGlaciers( void )
	{
		return ice_fortif_ & LOMASK;
	}
	int getGlacier( int side )
	{
		return ( ice_fortif_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setGlacier( int side )
	{
		ice_fortif_ |= ( side >> 3 );
	}
	void clrGlacier( int side )
	{
		ice_fortif_ &= ~( side >> 3 );
	}
#endif
#if defined HEXTOOLSMAP
	void clrAllRoutes( int side )
	{
		water_ &= ~( side & HIMASK ); // narrow straits, narrow straits ferries
		track_ &= ~( ( side >> 3 ) & LOMASK ); // tracks, motortracks, railtracks
		roads_ &= ~( ( side >> 3 ) & LOMASK ); // roads, motortracks, narrow straits ferries
		roads_ &= ~( side & HIMASK ); // railroads, railtracks
		misc2_ &= ~( side & HIMASK ); // standalone bridges
		lake_sea_routes_ &= ~( ( side >> 3 ) & LOMASK ); // shipping channels
		border_ &= ~( ( side << 4 ) & 0x80 ); // rail ferry SE
		border2_ &= ~( ( side << 2 ) & DATAMASK ); // rail ferries SW-W
	}
#endif
	int hasAnyRoutes( void )
	{
		// bWater upper 3 bits checks narrow straits, narrow straits ferries
		// bRoads lower 6 bits checks railroads, roads, motortracks, rail tracks, nrw. str. ferries
		// bTrack lower 3 bits checks tracks, motortracks, rail tracks
		// lake_sea_routes_ lower 3 bits checks shipping channels
		// bBorder upper bit & border2_ upper 2 bits checks rail ferries
		return ( ( water_ & HIMASK ) | ( roads_ & 0x3f ) | ( track_ & LOMASK ) |
				( lake_sea_routes_ & LOMASK ) | ( border_ & 0x80 ) | ( border2_ & DATAMASK ) );
	}

	// routes (all 10 are mutex except lake/sea ferries; note each settor clears all routes before, and sets routes flag after)

	// tracks	(bTracks LO)
	int hasTracks( void )
	{
		return track_ & LOMASK;
	}
	int getTrack( int side )
	{
		return ( track_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setTrack( int side )
	{
		clrAllRoutes( side );
		track_ |= ( ( side >> 3 ) & LOMASK );
		setRoutesFlag( );
	}
	void clrTrack( int side )
	{
		track_ &= ~( ( side >> 3 ) & LOMASK );
	}
#endif
	// motorable tracks	(bTracks LO & bRoads LO)
	int hasMotorTracks( void )
	{
		return hasTracks( ) & hasRoads( );
	}
	int getMotorTrack( int side )
	{
		return getTrack( side ) & getRoad( side );
	}
#if defined HEXTOOLSMAP
	void setMotorTrack( int side )
	{
		clrAllRoutes( side );
		track_ |= ( ( side >> 3 ) & LOMASK );
		roads_ |= ( ( side >> 3 ) & LOMASK );
		setRoutesFlag( );
	}
	void clrMotorTrack( int side )
	{
		track_ &= ~( ( side >> 3 ) & LOMASK );
		roads_ &= ~( ( side >> 3 ) & LOMASK );
	}
#endif
	// roads  (bRoads LO)
	int hasRoads( void )
	{
		return roads_ & LOMASK;
	}
	int getRoad( int side )
	{
		return ( roads_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setRoad( int side )
	{
		clrAllRoutes( side );
		roads_ |= ( side >> 3 );
		setRoutesFlag( );
	}
	void clrRoad( int side )
	{
		roads_ &= ~( ( side >> 3 ) & LOMASK );
	}
#endif
	// standalone bridge (HI)
	int hasStandaloneBridge( void )
	{
		return misc2_ & HIMASK;
	}
	int getStandaloneBridge( int side )
	{
		return misc2_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setStandaloneBridge( int side )
	{
		clrAllRoutes( side );
		misc2_ |= ( side & HIMASK );
		setRoutesFlag( );
	}
	void clrStandaloneBridge( int side )
	{
		misc2_ &= ~( side & HIMASK );
	}
#endif
	// railtracks	(bTrack LO & bRoads HI)
	int hasRailTracks( void )
	{
		return ( hasTracks( ) << 3 ) & hasRailRoads( );
	}
	int getRailTrack( int side )
	{
		// here, getTrack left-shifts its LO bits before returning
		return getTrack( side ) & getRailRoad( side );
	}
#if defined HEXTOOLSMAP
	void setRailTrack( int side )
	{
		clrAllRoutes( side );
		track_ |= ( ( side >> 3 ) & LOMASK );
		roads_ |= ( side & HIMASK );
		setRoutesFlag( );
	}
	void clrRailTrack( int side )
	{
		track_ &= ~( ( side >> 3 ) & LOMASK );
		roads_ &= ~( side & HIMASK );
	}
#endif
	// railroads	(bRoads HI)
	int hasRailRoads( void )
	{
		return roads_ & HIMASK;
	}
	int getRailRoad( int side )
	{
		return roads_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setRailRoad( int side )
	{
		clrAllRoutes( side );
		roads_ |= ( side & HIMASK );
		setRoutesFlag( );
	}
	void clrRailRoad( int side )
	{
		roads_ &= ~( side & HIMASK );
	}
#endif
	int getLowVolumeRailRoad( int side );

	// shipping channel (LO)
	int hasShippingChannels( void )
	{
		return lake_sea_routes_ & LOMASK;
	}
	int getShippingChannel( int side )
	{
		return ( lake_sea_routes_ << 3 ) & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setShippingChannel( int side )
	{
		clrAllRoutes( side );
		lake_sea_routes_ |= ( side >> 3 );
		setRoutesFlag( );
	}
	void clrShippingChannel( int side )
	{
		lake_sea_routes_ &= ~( ( side >> 3 ) & LOMASK );
	}
#endif
	// lake/sea ferries (HI)
	int hasLakeSeaFerries( void )
	{
		return lake_sea_routes_ & HIMASK;
	}
	int getLakeSeaFerry( int side )
	{
		return lake_sea_routes_ & HIMASK & side;
	}
#if defined HEXTOOLSMAP
	void setLakeSeaFerry( int side )
	{
		// note: no longer mutex with other transportation routes
		lake_sea_routes_ |= ( side & HIMASK );
	}
	void clrLakeSeaFerry( int side )
	{
		lake_sea_routes_ &= ~( side & HIMASK );
	}
#endif
	bool hasFerryTerminal( )
	{
		return ! city_ ? false : city_->hasFerryTerminal( );
	}

	// narrow straits ferries (bWater HI & bRoads LO)
	int hasNarrowStraitsFerries( void )
	{
		return hasNarrowStraits( ) & hasRoads( );
	}
	int getNarrowStraitsFerry( int side )
	{
		return getNarrowStrait( side ) & getRoad( side );
	}
#if defined HEXTOOLSMAP
	void setNarrowStraitsFerry( int side )
	{
		clrAllRoutes( side );
		water_ |= ( side & HIMASK );
		roads_ |= ( ( side >> 3 ) & LOMASK );
		setRoutesFlag( );
	}
	void clrNarrowStraitsFerry( int side )
	{
		water_ &= ~( side & HIMASK );
		roads_ &= ~( ( side >> 3 ) & LOMASK );
	}
#endif
	// narrow straits  (HI)
	int hasNarrowStraits( void )
	{
		return water_ & HIMASK;
	}
	int getNarrowStrait( int side )
	{
		return water_ & ( side & HIMASK );
	}
#if defined HEXTOOLSMAP
	void setNarrowStrait( int side )
	{
		clrAllRoutes( side );
		water_ |= ( side & HIMASK );
		setRoutesFlag( );
	}
	void clrNarrowStrait( int side )
	{
		water_ &= ~( side & HIMASK );
	}
#endif
	// rail ferries are uppermost bit of bBorder (SE) and upper 2 bits of bBorder2 (W-SW)
	int hasRailFerries( void )
	{
		// xx000000 --> 00xx0000 & 00110000 plus y0000000 --> 0000y000 & 00001000 is 00xxy000
		return ( ( ( border2_ >> 2 ) & 0x30 ) | ( ( border_ >> 4 ) & 0x08 ) );
	}
	int getRailFerry( int side )
	{
		return ( ( ( border2_ >> 2 ) & 0x30 ) | ( ( border_ >> 4 ) & 0x08 ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setRailFerry( int side )
	{
		clrAllRoutes( side );
		//00abc000 --> c0000000 & 10000000 sets c0000000
		border_ |= ( ( side << 4 ) & 0x80 );
		//00abc000 --> abc00000 & 11000000 sets ab000000
		border2_ |= ( ( side << 2 ) & DATAMASK );
		setRoutesFlag( );
	}
	void clrRailFerry( int side )
	{
		border_ &= ~( ( side << 4 ) & 0x80 );
		border2_ &= ~( ( side << 2 ) & DATAMASK );
	}
#endif
	// Private hexsides (hexsides which belong only to hexes)
	// escarpment   (bits 0...5)
	int hasEscarpments( void )
	{
		return escarpment_ & 0x3f;	// 00111111
	}
	int getEscarpment( int side )
	{
		return escarpment_ & side;
	}
#if defined HEXTOOLSMAP
	void setEscarpment( int side )
	{
		escarpment_ |= ( side & 0x3f );
	}
	void clrEscarpment( int side )
	{
		escarpment_ &= ~( side & 0x3f );
	}
#endif
	// interior mountain hexside	( bits 0-2:bIntMtnRMY(LO) | bits 3-5:int_mtn_intr_AF_(LO) )
	int hasMountainInterior( void )
	{
		return ( ( int_mtn_intr_AF_ & LOMASK ) << 3 ) | ( int_mtn_RMY_ & LOMASK );
	}
	int getMountainInterior( int side )
	{
		return ( ( ( int_mtn_intr_AF_ & LOMASK ) << 3 ) | ( int_mtn_RMY_ & LOMASK ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setMountainInterior( int side )
	{
		int_mtn_RMY_ |= ( side & LOMASK );
		int_mtn_intr_AF_ |= ( ( side & HIMASK ) >> 3 );
	}
	void clrMountainInterior( int side )
	{
		int_mtn_RMY_ &= ~( side & LOMASK );
		int_mtn_intr_AF_ &= ~( ( side & HIMASK ) >> 3 );
	}
#endif
	// impassable escarpment  (bits 7...8(0-1) + (3...6)(2-5)
	int hasImpassableEscarpments( void )
	{   // 						1100 0000  			+			0111 1000 -> 6 bits
		return ( ( escarpment_ & DATAMASK ) >> 6 ) | ( ( ice_fortif_ & 0x78 ) >> 1 );
	}
	int getImpassableEscarpment( int side )
	{
		return ( ( ( escarpment_ & DATAMASK ) >> 6 ) | ( ( ice_fortif_ & 0x78 ) >> 1 ) ) & side;
	}
#if defined HEXTOOLSMAP
	void setImpassableEscarpment( int side )
	{
		escarpment_ |= ( ( side & 0x3 ) << 6 );	// 1100 0000
		ice_fortif_ |= ( ( side & 0x3c ) << 1 );	// 0111 1000
	}
	void clrImpassableEscarpment( int side )
	{
		escarpment_ &= ~( ( side & 0x3 ) << 6 );
		ice_fortif_ &= ~( ( side & 0x3c ) << 1 );
	}
#endif
	// fortified (bits 0...5 & 7...8+4...7 (thus, Escarpment & ImpEscarpment)
	int hasFortifiedSides( void )
	{
		return getGreatWallFlag( ) ? FALSE : ( hasEscarpments( ) & hasImpassableEscarpments( ) );
	}
	int getFortifiedSide( int side )
	{
		return getGreatWallFlag( ) ? FALSE : ( getEscarpment( side ) & getImpassableEscarpment( side ) );
	}
#if defined HEXTOOLSMAP
	void setFortifiedSide( int side )
	{
		clrGreatWallFlag( );	// IMPORTANT:  this enforces all/none great wall or fortified w/i a hex
		setEscarpment( side );
		setImpassableEscarpment( side );
	}
	void clrFortifiedSide( int side )
	{
		// no need to explicitly reset the great wall flag here
		//	but also don't think it would cause trouble to do so
		clrEscarpment( side );
		clrImpassableEscarpment( side );
	}
#endif
	int hasGreatWalls( void )
	{
		return getGreatWallFlag( ) ? ( hasEscarpments( ) & hasImpassableEscarpments( ) ) : FALSE;
	}
	int getGreatWall( int side )
	{
		return getGreatWallFlag( ) ? ( getEscarpment( side ) & getImpassableEscarpment( side ) ) : FALSE;
	}
#if defined HEXTOOLSMAP
	void setGreatWall( int side )
	{
		setGreatWallFlag( );
		setEscarpment( side );
		setImpassableEscarpment( side );
	}
	void clrGreatWall( int side )
	{
		// IMPORTANT:  do not reset the great wall flag here
		// 	because the flag is used for all hexsides in the hex
		//	and there may be other hexsides the user wishes to
		//	retain as great wall
		clrEscarpment( side );
		clrImpassableEscarpment( side );
	}
#endif
	// great wall: fortified + great wall bit
	int getGreatWallFlag( void )
	{
		return canals_ & 0x40;
	}
#if defined HEXTOOLSMAP
	void setGreatWallFlag( void )
	{
		canals_ |= 0x40;
	}
	void clrGreatWallFlag( void )
	{
		canals_ &= ~0x40;
	}
#endif
	// other data, stored in unused bits of hexside bytes
	//
	// weather-zone, 6 bits (low 2b into bRoads DATA, mid 2b into bRivers DATA, hi 2b into bTrack DATA)
	int getWeatherZone( void )
	{
		return ( ( ( rivers_ & DATAMASK ) >> 4 ) | ( roads_ >> 6 ) ) | ( ( track_ & DATAMASK ) >> 2 );
	}
#if defined HEXTOOLSMAP
	void setWeatherZone( int i )
	{
		i &= 0x3f;  // max. 64 weather zones, clear 2 upper bits
		roads_ = ( roads_ & ~DATAMASK ) | ( i << 6 );
		rivers_ = ( rivers_ & ~DATAMASK ) | ( ( i << 4 ) & DATAMASK );
		track_ = ( track_ & ~DATAMASK ) | ( ( i << 2 ) & DATAMASK );
	}
#endif
	// rare hexside flag, 1 bit, bMountain bit 7
	int getRareHexsidesFlag( void )
	{
		return mountain_ & 0x80;	// 10000000
	}
#if defined HEXTOOLSMAP
	void setRareHexsidesFlag( void )
	{
		mountain_ |= 0x80;
	}
	void clrRareHexsidesFlag( void )
	{
		mountain_ &= ~0x80;
	}
#endif
	// inland waters hex flag, 1 bit, bMisc2 bit 7
	int getInlandWaterHexsidesFlag( void )
	{
		return misc2_ & 0x80;	// 10000000
	}
#if defined HEXTOOLSMAP
	void setInlandWaterHexsidesFlag( void )
	{
		misc2_ |= 0x80;
	}
	void clrInlandWaterHexsidesFlag( void )
	{
		misc2_ &= ~0x80;
	}
#endif
	// Private hex-side cache, 1 bit, bIceFort bit 7
	int getInteriorHexsidesFlag( void )
	{
		return ice_fortif_ & 0x80;	// 1000 0000
	}
#if defined HEXTOOLSMAP
	void setInteriorHexsidesFlag( )
	{
		ice_fortif_ |= 0x80;
	}
	void clrInteriorHexsidesFlag( )
	{
		ice_fortif_ &= ~0x80;
	}
#endif
	// route cache, 1 bit, bRestrictedWater bit 4
	int getRoutesFlag( void )
	{
		// TODO: consider ramifications of existing was bMountain & 0x40, i.e. bit 6;
		//	neither this function nor the bitwise byte check below is used anywhere
		return restricted_water_ & 0x10;	// 0001 0000 == bit 4
	}
#if defined HEXTOOLSMAP
	void setRoutesFlag( void )
	{
		restricted_water_ |= 0x10;
	}
	void clrRoutesFlag( void )
	{
		restricted_water_ &= ~0x10;
	}
#endif
	// late icing, 1 bit, bMisc2 bit 6
	int getLateSeaIce( void )
	{
		return misc2_ & 0x40;	// 01000000
	}
#if defined HEXTOOLSMAP
	void toggleLateSeaIce( void )
	{
		misc2_ ^= 0x40;
	}
	void setLateSeaIce( void )
	{
		misc2_ |= 0x40;
	}
	void clrLateSeaIce( void )
	{
		misc2_ &= ~0x40;
	}
#endif
	// restricted waters, 1 bit, restricted_water_ bit 5
	int getRestrictedWaters( void )
	{
		return restricted_water_ & 0x20;	// 0010 0000 == bit 5
	}
#if defined HEXTOOLSMAP
	void toggleRestrictedWaters( void )
	{
		restricted_water_ ^= 0x20;

		// TODO: during 2.3.0 alpha testing, need to clear the old restricted waters hexside data stored
		//restricted_water_ &= ~0x1f; // xxx00000
	}
	void setRestrictedWaters( void )
	{
		restricted_water_ |= 0x20;
	}
	void clrRestrictedWaters( void )
	{
		restricted_water_ &= ~0x20;
	}
#endif
	// hex limited stacking indicator, 1 bit, bBorder bit 6
	int getLimitedStackingHex( void )
	{
		return border_ & 0x40;
	}
#if defined HEXTOOLSMAP
	void toggleLimitedStackingHex( void )
	{
		border_ ^= 0x40;
	}
	void setLimitedStackingHex( void )
	{
		border_ |= 0x40;
	}
	void clrLimitedStackingHex( void )
	{
		border_ &= ~0x40;
	}
#endif
	// terrestrial district IDs
	int getLandDistrictID( void )
	{
		return district_id_;
	}
#if defined HEXTOOLSMAP
	void setLandDistrictID( int n )
	{
		district_id_ = n < 0 ? 0 : n;
	}
#endif
	// maritime district IDs
	int getSeaDistrictID( void )
	{
		return (	( border_ & 0x3f ) | 					// 5-0
					( mountain_ & 0x40 ) | 					// 6
					( ( inland_net_WMD_ & LOMASK ) << 7 ) |	// 7-9
					( ( border2_ & 0x3f ) << 10 )		);	// 10-15
	}
#if defined HEXTOOLSMAP
	void setSeaDistrictID( int n )
	{
		n = n < 0 ? -n : n;
		border_ = 		( border_ & ~0x3f )			| ( n & 0x3f ); 			// bits 0-5 of n into lower 6 bits of bBorder
		mountain_ =		( mountain_ & ~0x40 )		| ( n & 0x40 );				// bit 6 of n into bit 6 of bMountain
		inland_net_WMD_ =	( inland_net_WMD_ & ~LOMASK )	| ( ( n >> 7 ) & LOMASK );	// bits 7-9 of n into lower 3 bits of inland_net_WMD_
		border2_ =		( border2_ & ~0x3f )		| ( ( n >> 10 ) & 0x3f );	// bits 10-15 of n into lower 6 bits of bBorder2
	}
#endif

#if defined HEXTOOLSPLAY
	// TODO: consider moving this to another place, perhaps with HexContents:: version of the fx
	int getAFCapacity( void );
	void getHexStats( int* def, int* stats, int* mp, char* city );
#endif
protected:
	bool hasRareHexsides( );
	bool hasInteriorHexsides( );
	bool hasLandRoutes( );
};

#endif
