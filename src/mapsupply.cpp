#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "dirs.loop"
#include "sideplayer.h"
#include "phase.h"
#include "weatherzone.h"

#include "hextools.h"
using ht::wxS;
using ht::coordinatesOutOfBounds;

#include "rules.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "counter.h"
#include "groundunit.h"
#include "hexcontents.h"

extern char msg[];		// 10000
extern Rules _rule_set;
extern Counter* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _current_player;
extern int _current_phase;						// COMBAT_PHASE etc.
extern int _rivers_lakes_frozen[];	// W_ZONES		// true if rivers/lakes are frozen (frame.cpp)
extern int _month;

// Show Supply Range
// gray out hexes that are out of supply of selected unit(s)
// use distance field method to calculate reachable hexes.
void MapPane::showSelUnitSupplyRange( bool lazy_repaint )
{
	int x, y, xx, yy, dir, max_mp, radius = 0;
	int max_radius;
	Counter* cntr, *c, *calc_cntr = 0;

	// if neither graying nor clicking: do nothing
	// if just clicking: need to calc distance field!
	//if (!::GrayHexes && !ClickToMove)
	//	return;
	//if (!ShowSupplyRange)
	//	return;
	if ( ! isHexSelected( ) || ! _selected_unit_list )
		return;
	if ( _current_phase != Phase::INITIAL_PHASE )
		return;

	cntr = _selected_unit_list;
	if ( cntr->getCounterType( ) != Counter::Type::UNIT )
		return;
#if 0
	if ( cntr->GetSide() != RealPlayer && ! RuleSet.NonPhasMove && cntr->Type() != Counter::Type::PLANE )
		return;
#endif
	StatusMsg( "Showing supply range for selected units." );

	x = selected_hex_point_.x;
	y = selected_hex_point_.y;
	// find minimum MPs:
	// NOTE: We use counter with smallest MP amount in our calculations.
	//       Sometimes this produces erraneous results, ie. if there
	//       are pz and jager units in swamp we use jager's which
	//       actually can move much further than pz's. Only solution
	//       would be to calculate own distance fields to all units
	//       and draw own path to all of them. Too much trouble.
	max_mp = 256;
	for ( c = cntr; c; c = c->getNext( ) )
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->getSupplyRange( ) < max_mp )
		{
			max_mp = ( (GroundUnit*)c )->getSupplyRange( );  // we multiply by 2 as we calculate supply range in 1/2 hexes
			calc_cntr = c;
		}
	}

	cntr = calc_cntr;
	//sprintf(supplymsg, "%(%d,%d) supply range = %d (%s)", x, y, ((GroundUnit*)cntr)->GetSupplyRange(), ((GroundUnit*)cntr)->GetID());
	max_radius = ( (GroundUnit*)cntr )->getSupplyRange( ) + 2;  // for erasing
	// quickly erase old distance field data (needed when moving units):
	for ( yy = -max_radius; yy <= max_radius; ++yy )
		for ( xx = -max_radius; xx <= max_radius; ++xx )
			if ( ! coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
				sit_[y + yy][x + xx].setTmp( 0 );
	// recalculate new distance field
	max_radius = max_mp;

#if 0
	sprintf( msg, "Max SR = %d ", max_mp );
	wxMessageBox( wxS( msg ), Application::NAME );
#endif
	sit_[y][x].setTmp( 0x8000 );  // set starting hex
	for ( radius = 0; radius <= max_radius; ++radius )
	{
		for ( yy = -radius; yy <= radius; ++yy )
		{
			for ( xx = -radius; xx <= radius; ++xx )
			{
				if ( coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
					continue;
				// hex was updated last time: calc mp costs to enter neighbors:
				if ( ( sit_[y + yy][x + xx].getTmp( ) & 0xc000 ) == 0x8000 )
				{
					ENUM_DIRS(dir)
						fillTmpSupply( cntr, x + xx, y + yy, dir, 0x8000 );
					// set both 'this' and 'next' masks to avoid
					// unnecessary recalculations:
					sit_[y + yy][x + xx].setTmp( sit_[y + yy][x + xx].getTmp( ) | 0xc000 );
				}
			}
		}
	}

	// show reachable hexes
	for ( yy = 0; yy < world_y_size_; ++yy )
		for ( xx = 0; xx < world_x_size_; ++xx )
			if ( xx == x && yy == y )
				sit_[yy][xx].setGrayed( false );
			else if ( sit_[yy][xx].getTmp( ) && ( sit_[yy][xx].getTmp( ) & 0x3fff ) <= max_mp )
				sit_[yy][xx].setGrayed( false );
			else
				sit_[yy][xx].setGrayed( true );

#if 0
	ofstream os;
	os.open( "d:/FillTmpSupply.txt" );
	if ( os )
		os << supplymsg << endl;
	else
		wxMessageBox( wxT( "cannot open FillTmpSupply.txt" ), Application::NAME );
#endif

	// don't repaint anything, just gray out more hexes
	if ( lazy_repaint )
	{
		wxRect r;
		r.x = x - ( radius + 3 );
		r.y = y - ( radius + 3 );
		hex2Screen( & ( r.x ), & ( r.y ) );
		r.width = r.height = ( radius + 3 ) * 2 * Hex::SIZE_X;
		dont_redraw_map_ = 1;
		Refresh( false, &r );
	}
	else
	{
		if ( max_radius <= gray_out_radius_ )
			dont_redraw_map_ = 1;
		gray_out_radius_ = max_radius;
		gray_out_rail_radius_ = gray_out_admin_radius_ =
		//		GrayingRadiusOperational =
		gray_out_strat_radius_ = 0;
		Refresh( false, nullptr );
	}
}

float MapPane::TotalOwnREs( int x, int y )
{
	float re = 0.0F;

	if ( sit_[y][x].getOwner( ) == _current_player )
	{
		Counter* cntr = sit_[y][x].unit_list_;
		while ( cntr )
		{
			if ( cntr->getCounterType( ) == Counter::Type::UNIT )
				re += ( (GroundUnit*)cntr )->getRE( );

			cntr = cntr->getNext( );
		}
	}

	return re;
}

float MapPane::TotalEnemyREs( int x, int y )
{
	float re = 0.0F;

	if ( sit_[y][x].getOwner( ) != _current_player )
	{
		Counter* cntr = sit_[y][x].unit_list_;
		while ( cntr )
		{
			if ( cntr->getCounterType( ) == Counter::Type::UNIT )
				re += ( (GroundUnit*)cntr )->getRE( );

			cntr = cntr->getNext( );
		}
	}

	return re;
}

// Show Supply Range
// calc supply line cost to cross hexside. Note this is calculated in half MPs
int MapPane::calculateHexsideSupplyLineCost( Counter* cp, int x, int y, int dir )
{
	int ice = _rivers_lakes_frozen[hex_[y][x].getWeatherZone( )];
	//int w = Weather[Hex[y][x].GetWeatherZone()]; // TODO: weather!!!

	if ( cp->getCounterType( ) != Counter::Type::UNIT )
		return 90;

	if ( IsHighMtnPass( x, y, dir ) )
		return 98;

	//GroundUnit* unit = (GroundUnit*)cp;

	//int ski = ( UnitTypes[unit->getType( )].bType == SKI );  // ski unit?
	//int mot = unit->isSupplementalMotorized( );
	//int art = unit->isArtillery( );

	int mps = 0;
	if ( ! ice )
	{
		if ( IsMinorCanal( x, y, dir ) || IsCanal( x, y, dir ) || IsMinorRiver( x, y, dir ) )
			mps += 1;
		else if ( IsMajorRiver( x, y, dir ) )
			mps += 2;
	}

	if ( IsMountain( x, y, dir ) || IsKarst( x, y, dir ) )
		mps += 6;

	if ( IsNarrowStrait( x, y, dir ) )
		mps += ice ? 2 : 6;

	else if ( ! ice && IsWater( x, y, dir ) )
		return 97;

	else if ( ice && IsLakeSide( x, y, dir ) ) // frozen lake hexside
		mps += 1;

	else if ( ice && ( IsSeaSide( x, y, dir ) || IsRWaterSide( x, y, dir ) ) ) // frozen sea hexside
		mps += 2;

	if ( IsWadi( x, y, dir ) )
		mps += 1;

	if ( IsSaltDesert( x, y, dir ) )
		return 96;

	if ( IsGlacier( x, y, dir ) )
		return 95;

	if ( IsEscarpment( x, y, dir ) )
		mps += 1;

	if ( IsImpEscarpment( x, y, dir ) )
		return 94;

	return mps;
}

// calc supply line cost to enter hex; calculated in half-MPs
int MapPane::calculateHexSupplyLineCost( Counter* cp, int x, int y, int dir, bool show )
{
	if ( cp->getCounterType( ) != Counter::Type::UNIT )
		return 80;

	int new_x, new_y;
	if ( ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ ) == 0 )
		return 81;
#if 0
	int wt;
	switch ( Weather[hex[new_y][new_x].getWeatherZone( )] )
	{
		case W_MUD:
			wt = MP_MUD;
			break;
		case W_WINTER:
		case W_SNOW:
			wt = MP_WINTER;
			break;
		case W_CLEAR:
		case W_FROST:
		default:
			wt = MP_CLEAR;
			break;
	}
#endif
	int ice = _rivers_lakes_frozen[hex_[new_y][new_x].getWeatherZone( )];

	int mps;
	switch ( hex_[new_y][new_x].terrain_ )
	{
		case HexType::CLEAR:
		case HexType::ROUGH:
		case HexType::WOODS:
		case HexType::LAKE:  // intermittent lake
		case HexType::WOODEDLAKE:
		case HexType::SWAMPYTUNDRA:  // perma-frosted tundra
		case HexType::ROCKYTUNDRA:
		case HexType::RAVINES:
		case HexType::STONYDESERT:
			mps = 2;
			break;
		case HexType::MOUNTAIN:
		case HexType::FOREST:
		case HexType::WOODEDROUGH:
		case HexType::BOCAGE:
		case HexType::SAND:
		case HexType::CANALS:
		case HexType::JUNGLE:
		case HexType::JUNGLEROUGH:
		case HexType::JUNGLEMTN:
		case HexType::JUNGLESWAMP:
		case HexType::INTIRRIGATION:  // intensive irrgation
		case HexType::EXTREMERAVINES:  // extreme ravines/loess
		case HexType::HEAVYBUSH:
			mps = 6;
			break;
		case HexType::SWAMP:
		case HexType::WOODEDSWAMP:
			mps = ( ice ? 2 : 6 );
			break;
		case HexType::SEASONALSWAMP:
			// TODO if Jan == 0, Jul == 6,
			// then Jul-Oct is > 5 && < 10
			mps = ( _month > 5 && _month < 11 ) ? 6 : 2;
			break;
		case HexType::RESTRICTEDWATERS:
		case HexType::ICINGSEA:  // sea/lake subject to freezing
			if ( ice )
				mps = 2;
			else
				return 82;
			break;
		case HexType::SALTDESERT:
		case HexType::SALTLAKE:
		case HexType::SALTMARSH:
		default:
			return 83;
	}

	if ( show )
	{
		sprintf( msg, "MPs required (hex types) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}

	GroundUnit* unit = static_cast<GroundUnit*>( cp );
	mps += calculateHexsideSupplyLineCost( unit, x, y, dir );  // hex-side effects
	if ( show )
	{
		sprintf( msg, "MPs required (hex side) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}

	bool road = false;
	// roads ( lv rr == both ) NOTE: road + water == narrow strait == major river
	if (	IsBridge( x, y, dir )
		 && ! IsBridgeBreak( x, y, dir ) )
	{
			mps = 2;  // 1 MP, even during bad weather
			road = true;
	}
	else if (	IsRailRoad( x, y, dir ) 		// broken railroads are always roads
				|| IsLVRailRoad( x, y, dir )	// " "
				|| (	IsRoad( x, y, dir )		// but roads could be broken...
					 && ! IsNarrowStrait( x, y, dir )	// but not across narrow straits
					 && (	_rule_set.RoadsCanBeBroken == FALSE 	// can't be broken by rule setting
						 || ! sit_[new_y][new_x].getRRHit( ) ) ) )	// if can be broken, but are not broken
	{
		mps = 2;  // 1 MP, even during bad weather
		road = true;
	}

	if ( show )
	{
		sprintf( msg, "MPs required (after check roads) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}

#if 0
	 // trail: reduce 1 mp (but not to zero!)
	 if ( IsTrail( x, y, dir ) && mps > 0 )
		 --mps;
#endif

	// in arctic MPs are doubled (except when on the road)
	// TODO: find (or write) a function to adjudicate "arctic" ( i.e. == A, or >= P1 )
	if (	! road
		 && ( hex_[new_y][new_x].getWeatherZone( ) == WeatherZone::A
		   || hex_[new_y][new_x].getWeatherZone( ) >= WeatherZone::P1 ) )
		mps *= 2;

	// check ZOCs and occupied hexes
	if ( unit->getSide( ) == _current_player )
	{
		if ( IsEnemyOccupied( new_x, new_y ) )
			return 84;
		else if ( isHexInEnemyZOC( new_x, new_y ) && ( ! IsOwnOccupied( new_x, new_y ) || TotalOwnREs( new_x, new_y ) < 1.0F ) )
		// this hex is in enemy zoc and has less than 1 RE of own forces in the hex
			return 85;
		if ( isHexInEnemyRZOC( new_x, new_y ) && ! IsOwnOccupied( new_x, new_y ) )
		// extra 1/2 MP to for supply to move through an enemy reduced ZOC with no friendly forces present
			mps += 1;
	}
	else
	{
		if ( IsOwnOccupied( new_x, new_y ) )
			return 86;
		else if ( isHexInFriendlyZOC( new_x, new_y )
				&& ( ! IsEnemyOccupied( new_x, new_y ) || TotalEnemyREs( new_x, new_y ) < 1.0F ) )
		// this hex is in enemy zoc and has less than 1 RE of own forces in the hex
			return 100 + (int) ( TotalEnemyREs( new_x, new_y ) * 10 );
		if ( isHexInFriendlyRZOC( new_x, new_y ) && ! IsEnemyOccupied( new_x, new_y ) )
		// extra 1/2 MP to for supply to move through an enemy reduced ZOC with no friendly forces present
			mps += 1;
	}

	if ( show )
	{
		sprintf( msg, "MPs required (after check ZOC) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}

	// harassment hits:
	mps += sit_[new_y][new_x].getHarassment( );  // 0..2

	return mps;
}

// private helper for showSelUnitSupplyRange(..)
// Show Supply Range
// fill tmp data for given target hex (for MP distance field)
void MapPane::fillTmpSupply( Counter* cntr, int x, int y, int dir, int mask )
{
	int x2, y2;
	if ( ! ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
		return;

	int m0 = sit_[y][x].getTmp( ) & 0x3fff;  // discard 2 upper bits
	int m = sit_[y2][x2].getTmp( ) & 0x3fff;

	bool uncalculated = ( ( sit_[y2][x2].getTmp( ) & 0xc000 ) ? false : true );

	int mp;
	if ( ( cntr->getSide( ) == _current_player && IsEnemyOccupied( x2, y2 ) )
		|| ( cntr->getSide( ) != _current_player && IsOwnOccupied( x2, y2 ) ) )
		mp = 15;
	else
		mp = calculateHexSupplyLineCost( cntr, x, y, dir, false ); // false = don't show

	//sprintf (supplymsg, "%s\n(%d,%d) supply mp = %d (dir=%d, x2=%d, y2=%d uncalc=%d)", supplymsg, x, y, mp, dir, x2, y2, uncalculated);

	if ( uncalculated || ( m0 + mp ) < m )  // if already calculated AND from-data plus mp is less than to-data
		sit_[y2][x2].setTmp( m0 + ( mp | mask ) );
}

#endif
