#include <iostream>

#include <wx/wx.h>

#include "hextools.h"
#include "weathercondition.h"
#include "hexsidetype.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"

#if defined HEXTOOLSMAP
extern int _map_version;			// from mappane.cpp, used for versioning in HT-m
#endif

static const char MAP_ID_ESCAPE = '^';

static const short THREE_BIT_MASK = 0x7; // 0000 0111
static const short FOUR_BIT_MASK = 0xf;  // 0000 1111
static const short FIVE_BIT_MASK = 0x1f; // 0001 1111

Hex::Hex( )
{
	terrain_ = HexType::CLEAR;
	city_ = nullptr;
	rivers_ = 0;
	roads_ = 0;
	water_ = 0;
	wadi_ = 0;
	mountain_ = 0;
	high_mountain_pass_ = 0;
	escarpment_ = 0;
	border_ = 0;
	ice_fortif_ = 0;
	misc_ = 0;
	misc2_ = 0;
	canals_ = 0;
	track_ = 0;
	ornament_ = 0;
	int_mtn_RMY_ = 0;
	int_mtn_intr_AF_ = 0;
	fordable_water_ = 0;
	restricted_water_ = 0x07; // set three LO bits to permit restricted waters hexsides by default
	dbl_escarpment_ = 0;
	border2_ = 0;
	inland_net_WMD_ = 0;
	lake_sea_routes_ = 0;
	filling_res_ = 0;
	map_id_ = 0xffff;
	hex_id_ = ht::DEFAULT_HEX_ID;
	roads_ = 192; // setWeatherZone( WZ_D );
	inland_net_WMD_ = 2; // setSeaDistrictID( 0x0100 );
	district_id_ = 0x0100;
}

Hex::~Hex( )
{
}

#if defined HEXTOOLSMAP

int Hex::getLowVolumeRailRoad( int side )
{
	// TODO: deprecate this version check eventually
	if ( _map_version < 12 )
	{
		// before version 12, restricted waters was
		//	indicated by *both* lake and sea hexsides
		// this clears out the road bits of pre-ver 12
		//	stored low-volume railroads -- they thus get
		//	auto-upgraded to high-volume railroads:
		if ( getRoad( side ) & getRailRoad( side ) )
			clrRoad( side );
	}
	return 0;
#if 0
	// was:
	// here, getRoad left-shifts its LO bits before returning
	return getRoad( side ) & getRailRoad( side );
#endif
}

bool Hex::hasRareHexsides( )
{
	if ( hasMountains( ) || hasHighMtns( ) || hasHighMtnPasses( ) || hasKarsts( ) || hasSaltDeserts( )
		|| hasGlaciers( ) || hasDoubleEscarpments( ) || hasImpassableDoubleEscarpments( )
		|| hasFillingReservoirs( ) )
		return true;
	return false;
}

bool Hex::hasInteriorHexsides( )
{
	// only need to check hasEsc or hasImpEsc here, because Fortified
	//	(and great wall for that matter) is *both* esc and imp esc
	if ( hasMountainInterior( ) || hasEscarpments( ) || hasImpassableEscarpments( ) )
		return true;
	return false;
}

// refresh hex flags
void Hex::updateHexFlags( void )
{
	updateRareHexsidesFlag( );
	updateInteriorHexsidesFlag( );
	updateRouteHexsidesFlag( );
	updateInlandWaterHexsidesFlag( );
}

void Hex::updateRareHexsidesFlag( )
{
	if ( hasRareHexsides( ) )
		setRareHexsidesFlag( );
	else
		clrRareHexsidesFlag( );
}

void Hex::updateInteriorHexsidesFlag( )
{
	if ( hasInteriorHexsides( ) )
		setInteriorHexsidesFlag( );
	else
		clrInteriorHexsidesFlag( );
}

void Hex::updateRouteHexsidesFlag( )
{
	if ( hasAnyRoutes( ) )
		setRoutesFlag( );
	else
		clrRoutesFlag( );
}

void Hex::updateInlandWaterHexsidesFlag( )
{
	if ( hasInlandWaters( ) )
		setInlandWaterHexsidesFlag( );
	else
		clrInlandWaterHexsidesFlag( );
}
#endif

#if defined HEXTOOLSPLAY
// Calculate cached values
// TODO: used anywhere but map?
#if 0
void Hex::updateCache( void )
{
	if ( hasMountains( ) || hasHighMtns( ) || hasHighMtnPasses( ) || hasKarsts( ) || hasNarrowStraits( ) || hasWadiSides( )
		|| hasSaltDesertSides( ) || hasGlacierSides( ) )
		setRareHexSides( TRUE );
	else
		setRareHexSides( FALSE );

	if ( hasRoads( ) || hasRailRoads( ) || hasLVRailRoads( ) )
		setRoadCache( TRUE );
	else
		setRoadCache( FALSE );

	if ( hasFortifiedSides( ) || hasEscarpments( ) || hasImpassableEscarpments( ) )
		setPrivateHexSides( TRUE );
	else
		setPrivateHexSides( FALSE );
}
#endif
#endif

const char* Hex::convertMapIdEncoding( unsigned short mapId )
{
	short num = mapId & 0x00ff; // old number stored in lower 8 bits of word
	char chr = mapId >> 8; // old letter stored in upper 8 bits of word

	wxString newMapIdStr;
	if ( num == 0 ) // old '0M', '0Z', '0a', '0v', and '0 ' encodings
	{
		if ( chr == ' ' ) // old default/null map ID encoding
			newMapIdStr = wxString( MAP_ID_ESCAPE, 4 );
		else
		{
			switch( chr )
			{
				case 'M': // old Madagascar encoding
					newMapIdStr = "M" + wxString( MAP_ID_ESCAPE, 2 ) + "a";
					break;
				case 'Z': // old Azores encoding
					newMapIdStr = "AZ" + wxString( MAP_ID_ESCAPE, 2 );
					break;
				case 'a': // old Madeira encoding
					newMapIdStr = "MA" + wxString( MAP_ID_ESCAPE, 2 );
					break;
				case 'v': // old Cape Verde encoding
					newMapIdStr = "CV" + wxString( MAP_ID_ESCAPE, 2 );
					break;
			}
		}
	}
	else if ( num == 9 && chr == 'z' ) // old Canaries encoding '9z'
	{
		newMapIdStr = "CA" + wxString( MAP_ID_ESCAPE, 2 );
	}
	else if ( chr == 'W' ) // handles 'nW'
	{
		newMapIdStr = "WW"; // 1st and 2nd positions
		char third[3];
		sprintf( third, "%d", num );
		newMapIdStr += third[0]; // 3rd position
		if ( third[1] != '\0') // possible 4th position
			newMapIdStr += third[1];
		newMapIdStr += MAP_ID_ESCAPE; // last position
	}
	else // handles 'nA', 'nB', or 'nC'
	{
		// + 32 ==> make upper-case A-C lower-case a-c; otherwise use escape character
		char last = ( chr == 'A' || chr == 'B' || chr == 'C' ) ? chr + 32 : MAP_ID_ESCAPE;
		char firstThree[4];
		sprintf( firstThree, "%d", num );
		newMapIdStr = firstThree[0]; // 1st position
		newMapIdStr += firstThree[1] == '\0' ? MAP_ID_ESCAPE : firstThree[1]; // 2nd
		newMapIdStr += firstThree[2] == '\0' ? MAP_ID_ESCAPE : firstThree[2]; // 3rd
		newMapIdStr += last; // last position 'a', 'b', 'c', or MAP_ID_ESCAPE
	}

	return newMapIdStr.mb_str( );
}

void Hex::decodeMapId( char* decoded_string, unsigned short mapId )
{
	short lastVal = mapId & THREE_BIT_MASK;
	short thirdVal = ( mapId >> 3 ) & FIVE_BIT_MASK;
	short secondVal = ( mapId >> 8 ) & FOUR_BIT_MASK;
	short firstVal = mapId >> 12;

	char firstPos;
	if ( firstVal < 7 )
	{
		firstPos = '0' + firstVal;
	}
	else // handle the alpha encodings
	{
		switch ( firstVal )
		{
			case 0x7:
				firstPos = 'E';
				break;
			case 0x8:
				firstPos = 'G';
				break;
			case 0x9:
				firstPos = 'S';
				break;
			case 0xa:
				firstPos = 'A';
				break;
			case 0xb:
				firstPos = 'C';
				break;
			case 0xc:
				firstPos = 'I';
				break;
			case 0xd:
				firstPos = 'M';
				break;
			case 0xe:
				firstPos = 'W';
				break;
			case FOUR_BIT_MASK:
			default:
				firstPos = MAP_ID_ESCAPE;
				break;
		}
	}

	char secondPos;
	if ( secondVal < 10 )
	{
		secondPos = '0' + secondVal;
	}
	else // handle the alpha encodings
	{
		switch ( secondVal )
		{
			case 0xa:
				secondPos = 'A';
				break;
			case 0xb:
				secondPos = 'V';
				break;
			case 0xc:
				secondPos = 'W';
				break;
			case 0xd:
				secondPos = 'Z';
				break;
			case 0xe:
				secondPos = 'S';
				break;
			case FOUR_BIT_MASK:
			default:
				secondPos = MAP_ID_ESCAPE;
				break;
		}
	}

	char thirdPos;
	char fourthPos;
	if ( thirdVal == FIVE_BIT_MASK )
	{
		thirdPos = MAP_ID_ESCAPE;
		fourthPos = '\0';
	}
	else if ( thirdVal < 10 ) // single-digit third position
	{
		thirdPos = thirdVal + '0';
		fourthPos = '\0';
	}
	else // double-digit third position
	{
		char thirdPosStr[3]; // two digits plus null terminator
		sprintf( thirdPosStr, "%d", thirdVal );
		thirdPos = thirdPosStr[0];
		fourthPos = thirdPosStr[1];
	}

	char lastPos = lastVal == THREE_BIT_MASK ? MAP_ID_ESCAPE : lastVal + 'a';

	decoded_string[0] = firstPos;
	decoded_string[1] = secondPos;
	decoded_string[2] = thirdPos;
	if ( fourthPos == '\0' )
	{
		decoded_string[3] = lastPos;
		decoded_string[4] = '\0';
	}
	else
	{
		decoded_string[3] = fourthPos;
		decoded_string[4] = lastPos;
	}
	decoded_string[5] = '\0';
}

#if defined HEXTOOLSMAP
static const unsigned int MAP_ID_STR_MAX_LEN = 5;
static const unsigned int MAP_ID_STR_MIN_LEN = 4;

unsigned short Hex::encodeMapIdStr( const char* cp )
{
	unsigned int length = strlen( cp );

	char firstPos = cp[0];
	char secondPos = cp[1];
	char thirdPos = cp[2];
	char fourthPos = length == MAP_ID_STR_MAX_LEN ? cp[3] : '\0';
	char lastPos = cp[length - 1];

	short firstVal;
	if ( firstPos >= '0' && firstPos <= '6' )
	{
		firstVal = firstPos - '0'; // zero-base the value by subtracting lowest numeral
	}
	else // handle non-numerals
	{
		switch ( firstPos )
		{
			case 'E':
				firstVal = 0x7;
				break;
			case 'G':
				firstVal = 0x8;
				break;
			case 'S':
				firstVal = 0x9;
				break;
			case 'A':
				firstVal = 0xa;
				break;
			case 'C':
				firstVal = 0xb;
				break;
			case 'I':
				firstVal = 0xc;
				break;
			case 'M':
				firstVal = 0xd;
				break;
			case 'W':
				firstVal = 0xe;
				break;
			case MAP_ID_ESCAPE:
			default:
				firstVal = FOUR_BIT_MASK;
				break;
		}
	}

	short secondVal;
	if ( secondPos >= '0' && secondPos <= '9' )
	{
		secondVal = secondPos - '0'; // zero-base the value be subtracting lowest numeral
	}
	else // handle non-numerals
	{
		switch ( secondPos )
		{
			case 'A':
				secondVal = 0xa;
				break;
			case 'V':
				secondVal = 0xb;
				break;
			case 'W':
				secondVal = 0xc;
				break;
			case 'Z':
				secondVal = 0xd;
				break;
			case 'S':
				secondVal = 0xe;
				break;
			case MAP_ID_ESCAPE:
			default:
				secondVal = FOUR_BIT_MASK;
				break;
		}
	}

	// right-justify single-digit third-position input
	if ( fourthPos == '\0' )
	{
		fourthPos = thirdPos;
		thirdPos = '0';
	}

	char thirdPosStr[3];
	thirdPosStr[0] = thirdPos;
	thirdPosStr[1] = fourthPos;
	thirdPosStr[2] = '\0';

	// set the thirdVal to 31 (all 5 bits set) when 3rd position is escaped,
	//	otherwise, convert the assembled thirdPosStr to an integral value
	short thirdVal = fourthPos == MAP_ID_ESCAPE ? FIVE_BIT_MASK : static_cast<short>( atoi( thirdPosStr ) );

	short lastVal = lastPos == MAP_ID_ESCAPE ? THREE_BIT_MASK : lastPos - 'a'; // zero-base by subtracting lowest letter

	unsigned short encodedMapId = lastVal & THREE_BIT_MASK;		// put 3 bits into 0000 0000 0000 0XXX mask
	encodedMapId |= ( thirdVal & FIVE_BIT_MASK ) << 3;	// put 5 bits into 0000 0000 XXXX X<-- mask then left-shift 3
	encodedMapId |= ( secondVal & FOUR_BIT_MASK ) << 8;	// put 4 bits into 0000 XXXX <--- ---- mask then left-shift 8
	encodedMapId |= ( firstVal & FOUR_BIT_MASK ) << 12;	// put 4 bits into XXXX <--- ---- ---- mask then left-shift 12

	return encodedMapId;
}

bool Hex::validateMapIdFirstPosition( char c )
{
	switch ( c )
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case 'E':
		case 'G':
		case 'S':
		case 'A':
		case 'C':
		case 'I':
		case 'M':
		case 'W':
		case MAP_ID_ESCAPE:
			return true;
	}
	return false;
}

bool Hex::validateMapIdSecondPosition( char c )
{
	switch ( c )
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'V':
		case 'W':
		case 'Z':
		case 'S':
		case MAP_ID_ESCAPE:
			return true;
	}
	return false;
}

bool Hex::validateMapIdThirdPosition( char c, wxString* mapIdStr )
{
	if ( mapIdStr->Len( ) == MAP_ID_STR_MAX_LEN ) // length 5
		switch ( c )
		{
			case '0':
			case '1':
			case '2':
				return true;
		}
	else // map ID string is length 4
		switch ( c )
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case MAP_ID_ESCAPE:
				return true;
		}
	return false;
}

bool Hex::validateMapIdFourthPosition( char c )
{
	switch ( c )
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
	}
	return false;
}

bool Hex::validateMapIdLastPosition( char c )
{
	switch ( c )
	{
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case MAP_ID_ESCAPE:
			return true;
	}
	return false;
}

bool Hex::validateMapIdStr( wxString* mapIdStr )
{
	unsigned int length = mapIdStr->Len( );

	const int FIRST_POS = 0;
	const int SECOND_POS = 1;
	const int THIRD_POS = 2;
	const int FOURTH_POS = 3;
	const int LAST_POS = length - 1;

	if ( length < MAP_ID_STR_MIN_LEN || length > MAP_ID_STR_MAX_LEN )
		return false;

	if ( ! validateMapIdFirstPosition( mapIdStr->mb_str()[FIRST_POS] ) )
		return false;

	if ( ! validateMapIdSecondPosition( mapIdStr->mb_str()[SECOND_POS] ) )
		return false;

	if ( ! validateMapIdThirdPosition( mapIdStr->mb_str()[THIRD_POS], mapIdStr ) )
		return false;

	if ( mapIdStr->Length( ) == MAP_ID_STR_MAX_LEN
		&& ! validateMapIdFourthPosition( mapIdStr->mb_str()[FOURTH_POS] ) )
		return false;

	if ( ! validateMapIdLastPosition( mapIdStr->mb_str()[LAST_POS] ) )
		return false;

	// at least one of the first three positions must be non-escaped
	if (	mapIdStr->mb_str()[FIRST_POS] == MAP_ID_ESCAPE
		&& mapIdStr->mb_str()[SECOND_POS] == MAP_ID_ESCAPE
		&& mapIdStr->mb_str()[THIRD_POS] == MAP_ID_ESCAPE )
		return false;

	// don't escape 1st position if 2nd position is not escaped
	if (	mapIdStr->mb_str()[FIRST_POS] == MAP_ID_ESCAPE
		&& mapIdStr->mb_str()[SECOND_POS] != MAP_ID_ESCAPE )
		return false;

	// don't escape 2nd position if 3rd position is not escaped
	if (	mapIdStr->mb_str()[SECOND_POS] == MAP_ID_ESCAPE
		&& mapIdStr->mb_str()[THIRD_POS] != MAP_ID_ESCAPE )
		return false;

	return true;
}
#endif

#if defined HEXTOOLSMAP
std::ostream& operator<<( std::ostream& o, Hex& h )
{
	o << (int)h.terrain_ << ' ';
	o << (int)h.border_ << ' ';
	o << (int)h.roads_ << ' ';
	o << (int)h.rivers_ << ' ';
	o << (int)h.mountain_ << ' ';
	o << (int)h.high_mountain_pass_ << ' ';
	o << (int)h.escarpment_ << ' ';
	o << (int)h.water_ << ' ';
	o << (int)h.wadi_ << ' ';
	o << (int)h.ice_fortif_ << ' ';
	o << (int)h.misc_ << ' ';
	o << (int)h.hex_id_ << ' ';
	o << (int)h.map_id_ << ' ';
	o << (int)h.misc2_ << ' ';
	o << (int)h.canals_ << ' ';
	o << (int)h.track_ << ' ';
	o << (int)h.ornament_ << ' ';
	o << (int)h.int_mtn_RMY_ << ' ';
	o << (int)h.int_mtn_intr_AF_ << ' ';
	o << (int)h.fordable_water_ << ' ';
	o << (int)h.restricted_water_ << ' ';
	o << (int)h.dbl_escarpment_ << ' ';
	o << (int)h.border2_ << ' ';
	o << (int)h.inland_net_WMD_ << ' ';
	o << (int)h.lake_sea_routes_ << ' ';
	o << (int)h.filling_res_ << ' ';
	o << (int)h.district_id_ << ' ';

	// save city (if there is one)
	if ( h.city_ != nullptr )
	{
		o << 'c' << ' ';	// this means "city present"
		o << *h.city_;
	}
	else
		o << 'n' << ' ';	// "no city"

	o << std::endl;
	return o;
}
;
#endif

std::istream& operator>>( std::istream& is, Hex& hex )
{
	int a, b, c, d, e, f, g, h, i, j, k, l, m;
	char ch;
	City* city;

	is >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l >> m;
	hex.terrain_ = (unsigned char)a;

#if defined HEXTOOLSMAP
	// starting with internal map version 12:
	// restricted waters are now handled by a single bit (per hex)
	//	and rendered via various hex and interior hexside overlays
	if ( _map_version < 12 )
	{
		if ( hex.terrain_ == HexType::RESTRICTEDWATERS || hex.terrain_ == HexType::ICINGRESTRICTEDWATERS )
		{
			hex.setRestrictedWaters( );
			hex.terrain_ = hex.terrain_ == HexType::RESTRICTEDWATERS ? HexType::SEA : HexType::ICINGSEA;
		}
	}
#endif
	hex.border_ = (unsigned char)b;
	hex.roads_ = (unsigned char)c;
	hex.rivers_ = (unsigned char)d;
	hex.mountain_ = (unsigned char)e;
	hex.high_mountain_pass_ = (unsigned char)f;
	hex.escarpment_ = (unsigned char)g;
	hex.water_ = (unsigned char)h;
	hex.wadi_ = (unsigned char)i;
	hex.ice_fortif_ = (unsigned char)j;
	hex.misc_ = (unsigned char)k;
	hex.hex_id_ = (unsigned short)l;
	hex.map_id_ = (unsigned short)m;

#if defined HEXTOOLSMAP
	// this handles the conversion of old "number-letter" map ID schema to
	//	new 4 or 5 position map ID schema
	if ( _map_version < 12 )
		hex.setMapIdStr( Hex::convertMapIdEncoding( hex.map_id_ ) );

	// ver 2.1(pre) & newer: load two more bytes
	if ( _map_version >= 9 )
	{
#endif
	is >> a >> b;
	hex.misc2_ = (unsigned char)a;
	hex.canals_ = (unsigned char)b;
#if defined HEXTOOLSMAP
		// ver. 2.1 & newer: ww2pac tracks
		if ( _map_version >= 10 )
		{
#endif
	is >> a;
	hex.track_ = (unsigned char)a;
#if defined HEXTOOLSMAP
			// old trails/tracks should be FWRs/motortracks now
			if ( _map_version < 12 )
			{
				if ( hex.getTrack( Hex::WEST ) )
					hex.setMotorTrack( Hex::WEST );
				if ( hex.getTrack( Hex::SOUTHWEST ) )
					hex.setMotorTrack( Hex::SOUTHWEST );
				if ( hex.getTrack( Hex::SOUTHEAST ) )
					hex.setMotorTrack( Hex::SOUTHEAST );
			}
		}
		// ver 2.1.1: decorative hexside:
		if ( _map_version >= 11 )
		{
#endif
	is >> a;
	hex.ornament_ = (unsigned char)a;
#if defined HEXTOOLSMAP
		}
		// ver 2.3.0: railtracks, motor tracks, RMYs,
		//	intrinsic airfields, fordable lakes / great rivers,
		//	fordable major rivers, restricted sea sides (& ice),
		//	dbl escarpments, and imp. dbl esc., dams,
		//	internal sub-borders, demarcation lines,
		//	filling reservoirs, and land district IDs
		if ( _map_version >= 12 )
		{
#endif
	is >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j;
	hex.int_mtn_RMY_ = (unsigned char)a;
	hex.int_mtn_intr_AF_ = (unsigned char)b;
	hex.fordable_water_ = (unsigned char)c;
	hex.restricted_water_ = (unsigned char)d;
	hex.dbl_escarpment_ = (unsigned char)e;
	hex.border2_ = (unsigned char)f;
	hex.inland_net_WMD_ = (unsigned char)g;
	hex.lake_sea_routes_ = (unsigned char)h;
	hex.filling_res_ = (unsigned char)i;
	hex.district_id_ = (unsigned short)j;
#if defined HEXTOOLSMAP
		}
	}
	// TODO: needed for -play version of Hex API?
	if ( hex.hasMountains( )
			|| hex.hasGlaciers( )
			|| hex.hasKarsts( )
			|| hex.hasHighMtns( )
			|| hex.hasHighMtnPasses( )
			|| hex.hasSaltDeserts( )
			|| hex.hasDoubleEscarpments( )
			|| hex.hasImpassableDoubleEscarpments( )
			|| hex.hasFillingReservoirs( ) )
		hex.setRareHexsidesFlag( );
#endif
	// read city, if there is one
	is >> ch;
	if ( ch == 'c' )
	{
		city = new City( );
		is >> *city;
		hex.city_ = city;
#if defined HEXTOOLSMAP
		if ( _map_version < 12 )
		{
			while ( city != nullptr )
			{
				// deprecating old coal, using new coal
				city->type_ = ( city->type_ == City::Type::ENERGY_RESOURCE ? City::Type::COAL_RESOURCE : city->type_ );

				// for illegal old city names, assign new default city name ";n..."
				if ( City::illegalCityName( city->name_ ) )
				{
					strcpy( city->name_, ";n" );
					strcat( city->name_, City::cityDefaultName( city->type_ ) );
				}
				city = city->getNext( );
			} // while city != null
		} // map_version < 12
#endif
	}
#if defined HEXTOOLSMAP
	// map-42: these terrain types now handled as custom overlays w/ city position
	if ( _map_version < 12 && ( hex.terrain_ == HexType::ATOLL || hex.terrain_ == HexType::SMALLISLAND ) )
	{
		City* currentList = hex.city_;
		hex.city_ = new City( ";n[isl]", hex.terrain_ == HexType::ATOLL ? City::Type::ATOLL_CUSTOM : City::Type::SMALL_ISLAND_CUSTOM );
		hex.city_->setNext( currentList );
	}

	// map-59:  reclaiming border storage for sea circle IDs
#if 0
	if ( map_version <= 12 )	// use this for -ALPHA ONE-TIME CONVERTER, only
#endif
	if ( _map_version < 12 )	// enable this for -OMEGA and future builds
		hex.setSeaDistrictID( 256 );
#endif
	return is;
}
;

#if defined HEXTOOLSPLAY

#include "rules.h"
extern Rules _rule_set;
extern int _year;

// TODO: consider moving this to another place, perhaps with HexContents:: version of the fx
int Hex::getAFCapacity( void )
{
	if ( _rule_set.WW1AirBaseCapacity == FALSE )
	{
		// TODO: well, there are the exceptions, e.g. for floatplanes in coastal hexes
		if ( city_ == nullptr )      // no city -> no airfield
			return 0;
		switch ( city_->type_ )
		{
			// TODO: refactor for intrinsic airfields, and no intrinsic city cap
			case City::Type::MINOR:
				return 1;
			case City::Type::MAJOR:
				return 3;
			case City::Type::PARTIALHEX:
			case City::Type::FULLHEX:
				return 6;
			case City::Type::UNUSED_FORTRESS:
			case City::Type::FORTRESS:
				return 1;
			default:
				return 0;
		}
	}
	else // WW1 intrinsic airbase capacity
	{
		int cap = 0;

		// ww1: most hexes have intrinsic capacity:
		if ( terrain_ != HexType::MOUNTAIN && terrain_ != HexType::FOREST && terrain_ != HexType::SWAMP && terrain_ != HexType::WOODEDSWAMP
			&& terrain_ != HexType::GLACIER && terrain_ != HexType::SEA && terrain_ != HexType::LAKE && terrain_ != HexType::RESTRICTEDWATERS
			&& terrain_ != HexType::SALTLAKE && terrain_ != HexType::SALTMARSH && terrain_ != HexType::ICINGSEA )
		{
			if ( _year >= 12 && _year <= 15 )
				cap = 1;
			else if ( _year >= 16 && _year <= 17 )
				cap = 2;
			else if ( _year >= 18 )
				cap = 3;
		}
		// hexes with fortresses/major cities/resources has cap. of 1
		for ( City* city = city_; city; city = city->getNext( ) )
			if (	city->type_ == City::Type::PARTIALHEX
					|| city->type_ == City::Type::FULLHEX
					|| ( city->type_ >= City::Type::FORTRESS && city->type_ <= City::Type::OUVRAGE )
					|| city->type_ == City::Type::WW1_OLD_FORTRESS
					|| city->type_ == City::Type::WW1_NEW_FORTRESS
					|| city->type_ == City::Type::WW1_GREAT_FORTRESS
					|| city->type_ == City::Type::ORE_RESOURCE
					|| city->type_ == City::Type::ENERGY_RESOURCE )
				cap = 1;  // not "+= 1", just "= 1"

		return cap;
	}
}

#include "weatherclass.h"
#include "hextype.h"

extern HexType _hex_types[];//[TERRAINTYPECOUNT];
extern int _weather_condition[];//WeatherZone::COUNT];  // W_MUD etc., per zone (frame.cpp)

// returns hex's defense stats: def = 0..-4, stats = flags
// takes care of cities and weather
void Hex::getHexStats( int* def, int* stats, int* mp, char* city )
{
	*def = 0;
	*stats = 0;
	HexType* ht = &( terrain_ == HexType::NONE ? _hex_types[HexType::CLEAR] : _hex_types[terrain_] );

	// check city effects
	strcpy( city, "" );
	int citycnt = 0;
	City* c = city_;
	while ( c )
	{
		if ( citycnt )
			strcat( city, "\n" );

		++citycnt;

		switch ( c->type_ )
		{
			case City::Type::PT_CITY:
				strcat( city, "Point city: " );
				break;
			case City::Type::REF_PT:
				strcat( city, "Reference point: " );
				break;
			case City::Type::MINOR:
				strcat( city, "Reference city: " );
				break;
			case City::Type::MAJOR:
				strcat( city, "Dot city: " );
				*stats |= Hex::DEF_NO_AEC;
				break;
			case City::Type::PARTIALHEX:
				strcat( city, "Partial hex city: " );
				*def = -1;
				*stats |= Hex::DEF_INF_HALVED | Hex::DEF_CM_HALVED | Hex::DEF_GS_HALVED | Hex::DEF_NO_AEC | Hex::DEF_MTN_HALVED;
				break;
			case City::Type::FULLHEX:
				strcat( city, "Full hex city: " );
				*def = -2;
				*stats |= Hex::DEF_INF_HALVED | Hex::DEF_CM_HALVED | Hex::DEF_GS_HALVED | Hex::DEF_NO_AEC | Hex::DEF_MTN_HALVED;
				break;
			case City::Type::FORTRESS:
				*stats |= Hex::DEF_INF_HALVED | Hex::DEF_CM_HALVED | Hex::DEF_GS_HALVED | Hex::DEF_SG_ART_DOUBLED | Hex::DEF_MTN_HALVED;
				// no break here, intentionally using fall-through logic
			case City::Type::UNUSED_FORTRESS:
				strcat( city, "Fortress: " );
				*def = -1;
				*stats |= DEF_NO_AEC;
				break;
			case City::Type::UNUSED_WESTWALL:
				strcat( city, "Westwall fort" );
				*def = -1;
				*stats |= DEF_NO_AEC;
				break;
			case City::Type::WESTWALL_1:
				strcat( city, "Westwall fort" );
				*def = -1;
				*stats |= Hex::DEF_NO_AEC | Hex::DEF_NO_RETREAT;
				break;
			case City::Type::WESTWALL_2:
				strcat( city, "Westwall fort" );
				*def = -2;
				*stats |= Hex::DEF_NO_AEC | Hex::DEF_NO_RETREAT;
				break;
			case City::Type::WESTWALL_3:
				strcat( city, "Westwall fort - " );
				*def = -3;
				*stats |= Hex::DEF_NO_AEC | Hex::DEF_NO_RETREAT;
				break;
			case City::Type::UNUSED_OUVRAGE:
				strcat( city, "Ouvrage: " );
				*def = -1;
				*stats |= Hex::DEF_NO_AEC;
				break;
			case City::Type::OUVRAGE:
				strcat( city, "Ouvrage: " );
				*def = -1;
				*stats |= Hex::DEF_INF_HALVED | Hex::DEF_CM_HALVED | Hex::DEF_GS_HALVED | Hex::DEF_NO_AEC | Hex::DEF_NO_RETREAT | Hex::DEF_MTN_HALVED;
				break;
			case City::Type::ORE_RESOURCE:
				strcat( city, "Iron RC: " );
				break;
			case City::Type::ENERGY_RESOURCE:
				strcat( city, "Coal RC: " );
				break;
			case City::Type::OASIS:
				strcat( city, "Oasis: " );
				break;
			case City::Type::WW1_OLD_FORTRESS:
				strcat( city, "Old fortress: " );
				*def = -1;
				*stats |= DEF_NO_AEC;
				break;
			case City::Type::WW1_NEW_FORTRESS:
				strcat( city, "New fortress: " );
				*def = -1;
				*stats |= DEF_NO_AEC;
				break;
			case City::Type::WW1_GREAT_FORTRESS:
				strcat( city, "Great fortress: " );
				*def = -1;
				*stats |= Hex::DEF_NO_AEC;
				break;
			case City::Type::MINERAL_RESOURCE:
				strcat( city, "Mineral RC: " );
				break;
			case City::Type::SPECIAL_RESOURCE:
				strcat( city, "Special RC: " );
				break;
			case City::Type::HYDRO_RESOURCE:
				strcat( city, "Hydroelectric RC: " );
				break;
			case City::Type::COAL_RESOURCE:
				strcat( city, "Coal RC: " );
				break;
			case City::Type::NATGAS_RESOURCE:
				strcat( city, "Natural Gas RC: " );
				break;
			case City::Type::OIL_RESOURCE:
				strcat( city, "Oil RC: " );
				break;
			case City::Type::OILSHALE_RESOURCE:
				strcat( city, "Oil Shale RC: " );
				break;
			case City::Type::SYNTHOIL_RESOURCE:
				strcat( city, "Synthetic Oil RC: " );
				break;
			case City::Type::PEAT_RESOURCE:
				strcat( city, "Peat RC: " );
				break;
			case City::Type::CEMENT_RESOURCE:
				strcat( city, "Cement RC: " );
				break;
			case City::Type::RUBBER_RESOURCE:
				strcat( city, "Rubber RC: " );
				break;
			case City::Type::SYNTHRUBBER_RESOURCE:
				strcat( city, "Synthetic Rubber RC: " );
				break;
			case City::Type::SHIPYARD:
				strcat( city, "Shipyard: " );
				break;
			case City::Type::VOLKSDEUTSCH:
				strcat( city, "Volksdeutsch: " );
				break;
			case City::Type::RAILYARD:
				strcat( city, "Railyard: " );
				break;
			case City::Type::WATERWAY_DOCK:
				strcat( city, "Waterway Dock: " );
				break;
			case City::Type::MAJOR_NAVAL_BASE:
				strcat( city, "Major Naval Base: " );
				break;
			case City::Type::MINOR_NAVAL_BASE:
				strcat( city, "Minor Naval Bsse: " );
				break;
			case City::Type::FERRY_TERMINAL:
				strcat( city, "Ferry Terminal: " );
				break;
			case City::Type::ATOLL_CUSTOM:
				strcat( city, "Atoll: " );
				break;
			case City::Type::SMALL_ISLAND_CUSTOM:
				strcat( city, "Small Island: " );
				break;
			case City::Type::BIG_TEXT:
				strcat( city, "Large Map Text: " );
				break;
			default:	// MAPTEXT	13
				strcat( city, "Small Map Text: " );
				break;
		}
		if ( c->name_ )
		{
			// convert to wxS, scrub the name of any control codes, convert back to chars
			wxString cityname( c->name_ );
			cityname = City::scrubCityNameStyleAndRotationEncodings( cityname );
			const char* oneCityName = cityname.ToAscii( );

			strcat( city, oneCityName );
		}
		c = c->getNext( );
	}

	// TEC DRMs for Mud and Winter
	int weather = _weather_condition[getWeatherZone( )];
	switch ( weather )
	{
		case WeatherCondition::FROST:
		case WeatherCondition::CLEAR:
			weather = WeatherClass::CLEAR;
			break;
		case WeatherCondition::SNOW:
		case WeatherCondition::WINTER:
			weather = WeatherClass::WINTER;
			break;
		case WeatherCondition::MUD:
			weather = WeatherClass::MUD;
			break;
		default:
			weather = WeatherClass::CLEAR;
			break;
	}
	*def += ht->drm_[weather];

	// hex's normal attributes TODO: vet this commented logic
#if 0
	if ( ( ht->iDefFlags & Hex::DEF_FREEZES ) /* || it has been winter for a couple of turns */ )
		stats = 0;
	else
#endif
	*stats |= ( ht->defense_flags_ );
	if ( ( *stats & Hex::DEF_NO_EFFECT ) != *stats )	// there are some effects after all!
		*stats &= ~Hex::DEF_NO_EFFECT;				// so remove flag telling otherwise

	// get correct MPs
	for ( int i = 0; i < ht::NUM_MVMT_EFFECTS_COLUMNS; ++i )
		mp[i] = ht->mp_[weather][i];
}

#endif
