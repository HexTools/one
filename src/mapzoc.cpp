#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "dirs.loop"
#include "hexunits.loop"
#include "sideplayer.h"
#include "weathercondition.h"

#include "hextools.h"
using ht::coordinatesOutOfBounds;

#include "rulesvariant.h"
#include "rules.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "counter.h"
#include "groundunit.h"
#include "hexcontents.h"

extern Rules _rule_set;

#if 0
extern int _weather_condition[];	// W_ZONES	// W_MUD etc., per zone (frame.cpp)
#endif
extern int _current_player;

bool MapPane::isHexInEnemyZOC( int x, int y, int xdir )
{
	if ( _current_player == SidePlayer::AXIS )
		return IsAlliedZOC( x, y, xdir );
	else
		return IsAxisZOC( x, y, xdir );
}

bool MapPane::isHexInEnemyRZOC( int x, int y, int xdir )
{
	if ( _current_player == SidePlayer::AXIS )
		return IsAlliedReducedZOC( x, y, xdir );
	else
		return IsAxisReducedZOC( x, y, xdir );
}

bool MapPane::isHexInFriendlyZOC( int x, int y )
{
	if ( _current_player == SidePlayer::AXIS )
		return IsAxisZOC( x, y );
	else
		return IsAlliedZOC( x, y );
}

bool MapPane::isHexInFriendlyRZOC( int x, int y )
{
	if ( _current_player == SidePlayer::AXIS )
		return IsAxisReducedZOC( x, y );
	else
		return IsAlliedReducedZOC( x, y );
}

bool MapPane::IsZOCSource( int x, int y )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	// TODO: vet these weather zones, and/or write a method that checks
	//	all the zones that have ZOCs; Series 7A2 currently notes
	//	these exotic wx zones:  A, F, G, H, I, J, L, P, Q, S[ahel]
	//	write a private MapPane:: function to check these
	// in zone A,E & F ALL units have ZOCs (not pos AA and trucks):
#if 0
	if ( Hex[y][x].getWeatherZone( ) == ( 'A' - 'A' ) || Hex[y][x].getWeatherZone( ) == ( 'E' - 'A' )
		|| Hex[y][x].getWeatherZone( ) == ( 'F' - 'A' ) )  // indexing where zone A is #define'd 0
#endif
	if ( hexInExoticZone( x, y ) )
	{
		Counter* c;
		ENUM_HEXUNITS(x, y, c)
		{
			if ( c->getCounterType( ) != Counter::Type::UNIT )
				continue;
			GroundUnit* unit = (GroundUnit*)c;
			// TODO: vet all the unit types that don't have ZOCs
			if (	unit->getSize( ) >= GroundUnit::Size::REG_GROUP
					&& unit->getType( ) != GroundUnitType::WW2::POS_LT_AA
					&& unit->getType( ) != GroundUnitType::WW2::POS_HV_AA
					&& unit->getType( ) != GroundUnitType::WW2::TRUCK
					&& unit->getType( ) != GroundUnitType::WW2::FORTAREA )
				return true;
		}
		return false;
	}
	// in non-exotic weather zones, check for units with ZOC
	return sit_[y][x].isZOCSource( );
}

bool MapPane::IsReducedZOCSource( int x, int y )
{
	return sit_[y][x].isReducedZOCSource( );
}

// Variant A has Reduced ZOC hexsides
#if 0
bool MapPane::ReducedZOCHexside( int x, int y, int dir )
{
	if ( _rule_set.rules_variant_ == RulesVariant::A )
		if ( IsMajorRiver( x, y, dir ) || IsMountain( x, y, dir ) || IsNarrowStrait( x, y, dir ) || IsEscarpment( x, y, dir ) )
			return true;

	return false;
}
#endif

bool MapPane::IsAxisZOC( int x, int y, int xdir )
{
	int d;
	ENUM_DIRS( d )
	{
		if ( d == xdir )
			continue;
		if ( AxisZOCFrom( x, y, d ) )
			return true;
	}
#if 0
	 if (MapPane::getAdjHex(WEST, x, y, &new_x, &new_y, iXWorldSize, iYWorldSize))
	 if (!IsProhibited(x,y,WEST) &&
	 IsAxisOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (MapPane::getAdjHex(SOUTHEAST, x, y, &new_x, &new_y, iXWorldSize, iYWorldSize))
	 if (!IsProhibited(x,y,SOUTHEAST) &&
	 IsAxisOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (MapPane::getAdjHex(SOUTHWEST, x, y, &new_x, &new_y, iXWorldSize, iYWorldSize))
	 if (!IsProhibited(x,y,SOUTHWEST) &&
	 IsAxisOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (MapPane::getAdjHex(EAST, x, y, &new_x, &new_y, iXWorldSize, iYWorldSize))
	 if (!IsProhibited(x,y,EAST) &&
	 IsAxisOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (MapPane::getAdjHex(NORTHEAST, x, y, &new_x, &new_y, iXWorldSize, iYWorldSize))
	 if (!IsProhibited(x,y,NORTHEAST) &&
	 IsAxisOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (MapPane::getAdjHex(NORTHWEST, x, y, &new_x, &new_y, iXWorldSize, iYWorldSize))
	 if (!IsProhibited(x,y,NORTHWEST) &&
	 IsAxisOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
#endif

	return false;
}

bool MapPane::IsAxisReducedZOC( int x, int y, int xdir )
{
	int d;
	ENUM_DIRS( d )
	{
		if ( d == xdir )
			continue;
		if ( AxisReducedZOCFrom( x, y, d ) )
			return true;
	}
	return false;
#if 0
	 int new_x, new_y;

	 if (getAdjHex(WEST, x, y, &new_x, &new_y))
	 if (IsAxisOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(SOUTHEAST, x, y, &new_x, &new_y))
	 if (IsAxisOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(SOUTHWEST, x, y, &new_x, &new_y))
	 if (IsAxisOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(EAST, x, y, &new_x, &new_y))
	 if (IsAxisOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(NORTHEAST, x, y, &new_x, &new_y))
	 if (IsAxisOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(NORTHWEST, x, y, &new_x, &new_y))
	 if (IsAxisOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 return FALSE;
#endif
}

bool MapPane::AxisZOCFrom(int x, int y, int dir )
{
	// no adj coords in new_x, new_y
	int new_x, new_y;
	if ( ! ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ ) )
		return false;

	if ( ! IsAxisOwned( new_x, new_y ) )
		return false;

	if ( ! IsZOCSource( new_x, new_y ) )
		return false;

	// hex or hexside terrain prohibited
	if ( IsProhibited( x, y, dir ) )
		return false;

	return true;
#if 0
	int new_x, new_y;
	if (	ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ )
			&& ! IsProhibited( x, y, dir )
			&& IsAxisOwned( new_x, new_y ) )
	{
		// Rules Variant A:  two+ Reduced ZOCs in a hex is the same as a Standard ZOC
		if ( _rule_set.rules_variant_ == RulesVariant::A )
		{
			if ( IsZOCSource( new_x, new_y ) )
			{
				if ( ReducedZOCHexside( x, y, dir ) ) // we have a Standard ZOC but the hexside causes a Reduced ZOC
					*two_rzoc += rzoc;
				else if ( allreduced ) // there is a condition where all ZOCs are Reduced e.g. Mud Weather
					++(*two_rzoc);
				else
					return true;
			}
			else if ( IsReducedZOCSource( new_x, new_y ) && ! ReducedZOCHexside( x, y, dir ) )
			{
				*two_rzoc += rzoc;
			}
		}
		else if ( IsZOCSource( new_x, new_y ) )
		{
			return true;
		}
	}
	return false;
#endif
}

bool MapPane::AxisReducedZOCFrom( int x, int y, int dir )
{
	int new_x, new_y;

	if (	ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ )
			&& IsAxisOwned( new_x, new_y )
			&& IsReducedZOCSource( new_x, new_y ) )
#if 0
			&& ! ReducedZOCHexside( x, y, dir ) )
#endif
		return true;

	return false;
}

bool MapPane::IsAlliedZOC( int x, int y, int xdir /* = 0 */ )
{
	int d;
	ENUM_DIRS( d )
	{
		if ( d == xdir )
			continue;
		if ( AlliedZOCFrom( x, y, d ) )
			return true;
	}
#if 0
	 if (getAdjHex(WEST, x, y, &new_x, &new_y))
	 if (!IsProhibited(x,y,WEST) &&
	 IsAlliedOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(SOUTHEAST, x, y, &new_x, &new_y))
	 if (!IsProhibited(x,y,SOUTHEAST) &&
	 IsAlliedOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(SOUTHWEST, x, y, &new_x, &new_y))
	 if (!IsProhibited(x,y,SOUTHWEST) &&
	 IsAlliedOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(EAST, x, y, &new_x, &new_y))
	 if (!IsProhibited(x,y,EAST) &&
	 IsAlliedOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(NORTHEAST, x, y, &new_x, &new_y))
	 if (!IsProhibited(x,y,NORTHEAST) &&
	 IsAlliedOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(NORTHWEST, x, y, &new_x, &new_y))
	 if (!IsProhibited(x,y,NORTHWEST) &&
	 IsAlliedOwned(new_x, new_y) && IsZOCSource(new_x, new_y))
	 return TRUE;
#endif

	return false;
}

bool MapPane::IsAlliedReducedZOC( int x, int y, int xdir )
{
	int d;
	ENUM_DIRS( d )
	{
		if ( d == xdir )
			continue;
		if ( AlliedReducedZOCFrom( x, y, d ) )
			return true;
	}
	return false;
#if 0
	 int new_x, new_y;
	 if (getAdjHex(WEST, x, y, &new_x, &new_y))
	 if (IsAlliedOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(SOUTHEAST, x, y, &new_x, &new_y))
	 if (IsAlliedOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(SOUTHWEST, x, y, &new_x, &new_y))
	 if (IsAlliedOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(EAST, x, y, &new_x, &new_y))
	 if (IsAlliedOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(NORTHEAST, x, y, &new_x, &new_y))
	 if (IsAlliedOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 if (getAdjHex(NORTHWEST, x, y, &new_x, &new_y))
	 if (IsAlliedOwned(new_x, new_y) && IsReducedZOCSource(new_x, new_y))
	 return TRUE;
	 return FALSE;
#endif
}

bool MapPane::AlliedZOCFrom( int x, int y, int dir )
{
	// no adj coords in new_x, new_y
	int new_x, new_y;
	if ( ! ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ ) )
		return false;

	if ( ! IsAlliedOwned( new_x, new_y ) )
		return false;

	if ( ! IsZOCSource( new_x, new_y ) )
		return false;

	// hex or hexside terrain prohibited
	if ( IsProhibited( x, y, dir ) )
		return false;

	// Allied ZOC prohibited across int'l border in dir
	if ( NoAlliedZOCAcrossBorder( x, y, dir ) )
		return false;

	return true;

#if 0
	int new_x, new_y;

	if (	ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ )
			&& ! IsProhibited( x, y, dir )
			&& ! NoAlliedZOCAcrossBorder( x, y, dir )
			&& IsAlliedOwned( new_x, new_y ) )
	{
		// Rules Variant A:  two+ Reduced ZOCs in a hex is the same as a Standard ZOC
		if ( _rule_set.rules_variant_ == RulesVariant::A )
		{
			if ( IsZOCSource( new_x, new_y ) )
			{
				if ( ReducedZOCHexside( x, y, dir ) ) // we have a Standard ZOC but the hexside causes a Reduced ZOC
					*two_rzoc += rzoc;
				else if ( allreduced ) // there is a condition where all ZOCs are Reduced e.g. Mud Weather
					(*two_rzoc)++;
				else
					return true;
			}
			else if ( IsReducedZOCSource( new_x, new_y ) && ! ReducedZOCHexside( x, y, dir ) )
			{
				*two_rzoc += rzoc;
			}
		}
		else if ( IsZOCSource( new_x, new_y ) )
		{
			return true;
		}
	}
	return false;
#endif
}

bool MapPane::AlliedReducedZOCFrom( int x, int y, int dir )
{
	int new_x, new_y;

	if (	ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ )
			&& IsAlliedOwned( new_x, new_y )			// Allied-specific
			&& ! NoAlliedZOCAcrossBorder( x, y, dir )	// Allied-specific
			&& IsReducedZOCSource( new_x, new_y ) )
#if 0
			&& ! ReducedZOCHexside( x, y, dir ) )
#endif
		return true;

	return false;
}

bool MapPane::NoAlliedZOCAcrossBorder( int x, int y, int dir )
{
	if ( _rule_set.NoAlliedZOCAcrossBorders == TRUE )
		return IsIntlBorder( x, y, dir );

	return false;
}

#endif
