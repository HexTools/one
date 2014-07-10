#if defined HEXTOOLSPLAY

#include <algorithm>
using std::min;
using std::max;

#include <wx/wx.h>
#include <wx/msw/ole/uuid.h>

#include "application.h"
#include "dirs.loop"
#include "selectedunits.loop"
#include "hexunits.loop"
#include "gameunitset.h"
#include "sideplayer.h"
#include "weathercondition.h"
#include "phase.h"
#include "weatherclass.h"
#include "weatherzone.h"
#include "supplystatus.h"
#include "movementmode.h"
#include "disruptionstatus.h"
#include "hextools.h"
using ht::wxS;
using ht::dir2replayExploitation;
using ht::dir2replay;
using ht::coordinatesOutOfBounds;
#include "port.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "rulesvariant.h"
#include "rules.h"
#include "mappane.h"
#include "counter.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "airunittype.h"
#include "airunit.h"
#include "navalunittype.h"
#include "navalunit.h"
#include "hexnote.h"
#include "facility.h"
#include "hexcontents.h"
#include "hexcontentspane.h"
#include "combatresult.h"
#include "dunitmover.h"

extern const char* _month_string[12];
extern const char* _turn_numeral[20];

extern HexType _hex_types[]; //[TERRAINTYPECOUNT];	// terrain types

extern char msg[10000];
extern Counter* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _phasing_player;
extern int _current_player;
extern int _current_phase;						// COMBAT_PHASE etc.
extern int _year;
extern int _month;
extern int _turn;

extern int _weather_condition[];			// W_MUD etc., per zone (frame.cpp)
extern int _rivers_lakes_frozen[];			// true if rivers/lakes are frozen (frame.cpp)

extern Rules _rule_set;

extern int _use_point_and_click_move;
extern int _gray_out_hexes;

extern GroundUnitType _ground_unit_type[GroundUnitType::WW2::UNITTYPECOUNT];	// unit types

// display or hide unit mover dialog
void MapPane::CmMoveUnits( )
{
	if ( ! unit_mover_dialog_ )
	{
		unit_mover_dialog_ = new DUnitMover( this );
		if ( ! unit_mover_dialog_ )
			wxMessageBox( wxT("Failed to create the initial move-dialog!"), Application::NAME );
		else
			unit_mover_dialog_->Show( true );
	}
	else
	{
		if ( unit_mover_dialog_->IsShown( ) )
			unit_mover_dialog_->Show( true );  // TODO: DLT
		else
			unit_mover_dialog_->Show( true );
	}
}

// REs rail capacity used this turn
void MapPane::show_rail_usage( )
{
	sprintf( msg, "This turn rail usage: %.1f RE's", turn_REs_rail_used_ );
	wxMessageBox( ht::wxS( msg ), Application::NAME );
}

void MapPane::CmUnitNorthWest( )
{
	March( Hex::NORTHWEST );
}

void MapPane::CmUnitNorthEast( )
{
	March( Hex::NORTHEAST );
}

void MapPane::CmUnitEast( )
{
	March( Hex::EAST );
}

void MapPane::CmUnitSouthEast( )
{
	March( Hex::SOUTHEAST );
}

void MapPane::CmUnitSouthWest( )
{
	March( Hex::SOUTHWEST );
}

void MapPane::CmUnitWest( )
{
	March( Hex::WEST );
}

// move selected hex (dir=numpad)
void MapPane::MoveSelection( int dir )
{
	if ( _current_phase == Phase::NO_GAME )
		return;

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	switch ( dir )
	{
		case 8:  // up
			--y;
			break;
		case 2:  // down
			++y;
			break;
		case 4:  // left
			--x;
			break;
		case 6:  // right
			++x;
			break;
	}

	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return;

	contents_pane_->deselectAll( );  // new hex selected, get rid of old
	contents_pane_->clear( );  // selections.
	UnHiLiteHex( );
	if ( x == selected_hex_point_.x && y == selected_hex_point_.y ) // hex de-selected
	{
		selected_hex_point_.y = selected_hex_point_.x = -1;
		return;
	}
	centerHex( x, y );
	selected_hex_point_.x = x;
	selected_hex_point_.y = y;
	//UnitWin->AddAirUnits(Sit[y][x].AirUnitPtr);
	contents_pane_->addUnits( sit_[y][x].unit_list_ );
	HiLiteHex( x, y );
}

// Undo movement of all selected units
void MapPane::CmUnitHome( )
{
	// only phasing player can do this
	//if (Player != RealPlayer)
	//	return;

	Counter* unit;
	ENUM_SELUNITS(unit)
		if ( unit->getSide( ) != _current_player )  // only own units!
		{
			SideErr( );
			return;
		}

	if ( ! _selected_unit_list
		|| ( wxMessageBox( wxT("Really undo all movements of selected units?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES ) )
		return;

	//UnitWin->MarkAllUnselected();   // caused just problems -> removed 2.2.2

	int cur_x = selected_hex_point_.x;
	int cur_y = selected_hex_point_.y;

	char tmps[20];
	int i = 0;
	unit = _selected_unit_list;
	while ( unit != NULL )
	{
		Counter* next = unit->getNext( );
		contents_pane_->deselectUnit( unit );  // from sel list to sit-list
		int x, y;
		unit->getHomeHex( &x, &y );
		if ( x != selected_hex_point_.x || y != selected_hex_point_.y )  // (unit->GetMP() != unit->GetMaxMP())
		{
			unit->getHomeHex( &x, &y );

			//if ( x < 0 || y < 0 || x >= iXWorldSize || y >= iYWorldSize )
			if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
			{
				wxMessageBox( wxT("Illegal home position!\nScenario-file may be corrupted") );
				return;
			}

			// check if unit was ordered to attack
			int tmp;
			if ( unit->getCounterType( ) == Counter::Type::UNIT && ( tmp = unit->getAttackDir( ) ) != 0 )
			{
				char str[300];
				unit->getUnitString( str );
				sprintf( msg, "The following unit has been ordered to attack enemy forces:\n\n%s\n\nDo you want to cancel this order?", str );
				if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_INFORMATION ) != wxYES )
					goto end;

				unit->setAttackDir( 0 );  // cancel attack orders

				int tmp_x, tmp_y;
				ht::getAdjHexCoords( tmp, cur_x, cur_y, &tmp_x, &tmp_y, world_x_size_, world_y_size_ );
				if ( ! IsHexAttacked( tmp_x, tmp_y ) )
				{
					wxMessageBox( wxT("NOTE:\nno forces were left to attack target hex,\nattack canceled"), Application::NAME, wxOK | wxICON_INFORMATION );
					sit_[tmp_y][tmp_x].setTarget( false );
				}
			}
			// check if air operation is canceled
			if ( unit->getCounterType( ) == Counter::Type::PLANE && unit->getMission( ) != AirUnit::Mission::NONE )
			{
				char str[300];
				unit->getUnitString( str );
				sprintf( msg, "The following air group has been ordered to fight enemy forces:\n\n%s\n\nDo you want to cancel this order?", str );
				if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_INFORMATION) != wxYES )
					goto end;

				unit->setMission( AirUnit::Mission::NONE );
				if ( ! IsHexAirAttacked( cur_x, cur_y ) )
					sit_[cur_y][cur_x].setAirTarget( false );
			}

			// if plane && staged && airbase cap. needed to stage => reduce
			// usage of staging airfield too:
			if ( unit->getCounterType( ) == Counter::Type::PLANE && static_cast<AirUnit*>( unit )->getStaged( )
				&& ( _rule_set.NewAFCapacitySystem == FALSE ) )
			{
				// travel backwards path to find staging hex:
				int loop = 1;
				int rx = cur_x;
				int ry = cur_y;
				while ( loop )
				{
					// in exploitation phase stop undoing when going back to
					// movement phase actions:
					int nx, ny;
					int r = unit->getReplay( i++ );
					if ( _current_phase == Phase::EXPLOITATION_PHASE && r == ht::Replay::Movement::END )
						loop = 0;
					switch ( r )
					{
						case ht::Replay::END:
							wxMessageBox( wxT("Air group has staged, but I don't know where!\nPlease adjust airbase usage manually!") );
							loop = 0;
							break;
						case ht::Replay::Movement::STAGE:
							sit_[ry][rx].setAFUsage( sit_[y][x].getAFUsage( ) - 1 );
							hex_[ry][rx].getHexIdStr( tmps );
							sprintf( msg, "Note: air unit staged at %s, airbase usage\nin that hex has been restored too.", tmps );
							wxMessageBox( wxS( msg ) );
							loop = 0;
							break;
						case ht::Replay::Exploit::EAST:
						case ht::Replay::Combat::EAST:
						case ht::Replay::Movement::EAST:
							ht::getAdjHexCoords( Hex::WEST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Exploit::SOUTHEAST:
						case ht::Replay::Combat::SOUTHEAST:
						case ht::Replay::Movement::SOUTHEAST:
							ht::getAdjHexCoords( Hex::NORTHWEST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Exploit::NORTHEAST:
						case ht::Replay::Combat::NORTHEAST:
						case ht::Replay::Movement::NORTHEAST:
							ht::getAdjHexCoords( Hex::SOUTHWEST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Exploit::WEST:
						case ht::Replay::Combat::WEST:
						case ht::Replay::Movement::WEST:
							ht::getAdjHexCoords( Hex::EAST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Exploit::SOUTHWEST:
						case ht::Replay::Combat::SOUTHWEST:
						case ht::Replay::Movement::SOUTHWEST:
							ht::getAdjHexCoords( Hex::NORTHEAST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Exploit::NORTHWEST:
						case ht::Replay::Combat::NORTHWEST:
						case ht::Replay::Movement::NORTHWEST:
							ht::getAdjHexCoords( Hex::SOUTHEAST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
					}  // switch unit replay
					rx = nx;
					ry = ny;
				}  // while
			}
			// end of staging undo

			// old air system: land planes in movement phase
			// new air system: never land planes (hard to know when should...)
			if ( _rule_set.OnDemandAirMissions == FALSE && _current_phase != Phase::EXPLOITATION_PHASE )  // land planes except in exp phase
				unit->setInAir( 0 );
			if ( unit->getCounterType( ) == Counter::Type::PLANE )
			{
				// Undo aircraft movment - no longer inoperative
				( (AirUnit*)unit )->setActive( TRUE );
				( (AirUnit*)unit )->setInAir( TRUE );
				( (AirUnit*)unit )->setExtRange( FALSE );
				if ( unit->getMission( ) == AirUnit::Mission::INTERCEPT )  // no work, need rethink
					unit->setMP( unit->getMaxMP( ) / 2 );
				else
					unit->setMP( unit->getMaxMP( ) );
			}

			// undo rail breaks, airbase hits and port hits
			// hit selected here
			if ( unit->getCounterType( ) == Counter::Type::UNIT )
			{
				int rx = cur_x;
				int ry = cur_y;
				int loop = 1;
				while ( loop && ! coordinatesOutOfBounds( rx, ry, world_x_size_, world_y_size_ ) )
				{
					// in exploitation phase stop undoing when going back to
					// movement phase actions:
					int nx, ny;
					int r = unit->getReplay( i++ );

					if ( _current_phase == Phase::EXPLOITATION_PHASE && r == ht::Replay::Movement::END )
						loop = 0;

					switch ( r )
					{
						case ht::Replay::END:
							loop = 0;
							break;
						case ht::Replay::Movement::HITRAIL:
							hex_[ry][rx].getHexIdStr( tmps );
							if ( ! sit_[ry][rx].getRRHit( ) )
								sprintf( msg, "Note: unit broke rail at %s, but it has been fixed\nby someone. MPs of the unit that fixed it ought to\nbe adjusted accordingly.", tmps );
							else
								sprintf( msg, "Note: unit broke rail at %s, it's now unbroken", tmps );
							sit_[ry][rx].setRRHit( false );
							updateHitMarkers( rx, ry );
							PaintHex( rx, ry );  // hit marker removed -> may affect map
							wxMessageBox( wxS( msg ) );
							nx = rx;
							ny = ry;  // stay in the same hex
							break;
						case ht::Replay::Movement::HITAF:
							hex_[ry][rx].getHexIdStr( tmps );
							sprintf( msg, "Note: unit damaged airbase at %s, that damage is now undone.", tmps );
							sit_[ry][rx].setAFHits( sit_[ry][rx].getAFHits( ) - 1 );
							updateHitMarkers( rx, ry );
							PaintHex( rx, ry );  // hit marker removed -> may affect map
							wxMessageBox( wxS( msg ) );
							nx = rx;
							ny = ry;  // stay in the same hex
							break;
						case ht::Replay::Movement::FIXRAIL:
							hex_[ry][rx].getHexIdStr( tmps );
							if ( ! sit_[ry][rx].getRRHit( ) )
								sprintf( msg, "Note: unit repaired railroad at %s. It's\nnow broken again.", tmps );
							else
								sprintf( msg, "Note: unit repaired railroad at %s, but it's\nbeen broken again by someone. MPs of that unit\nought to be adjusted accordingly.", tmps );
							sit_[ry][rx].setRRHit( true );
							updateHitMarkers( rx, ry );
							PaintHex( rx, ry );  // hit marker added -> may affect map
							wxMessageBox( wxS( msg ) );
							nx = rx;
							ny = ry;  // stay in the same hex
							break;
						case ht::Replay::Movement::CLEARRAIL:
							hex_[ry][rx].getHexIdStr( tmps );
							if ( ! sit_[ry][rx].isRailObstructed( ) )
								sprintf( msg, "Note: unit cleared railroad at %s. It's\nnow obstructed again.", tmps );
							else
								sprintf( msg, "Note: unit cleared railroad at %s, but it's\nbeen obstructed again somehow. MPs of that unit\nought to be adjusted accordingly.", tmps );
							sit_[ry][rx].setRailObstructed( true );
							updateHitMarkers( rx, ry );
							PaintHex( rx, ry );  // hit marker added -> may affect map
							wxMessageBox( wxS( msg ) );
							nx = rx;
							ny = ry;  // stay in the same hex
							break;
						case ht::Replay::Movement::FIXAF:
							hex_[ry][rx].getHexIdStr( tmps );
							sprintf( msg, "Note: unit repaired airbase at %s. It's now damaged again.", tmps );
							sit_[ry][rx].setAFHits( sit_[ry][rx].getAFHits( ) + 1 );
							updateHitMarkers( rx, ry );
							PaintHex( rx, ry );  // hit marker added -> may affect map
							wxMessageBox( wxS( msg ) );
							nx = rx;
							ny = ry;  // stay in the same hex
							break;
						case ht::Replay::Admin::EAST:
						case ht::Replay::Exploit::EAST:
						case ht::Replay::Combat::EAST:
						case ht::Replay::Movement::EAST:
							ht::getAdjHexCoords( Hex::WEST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Admin::SOUTHEAST:
						case ht::Replay::Exploit::SOUTHEAST:
						case ht::Replay::Combat::SOUTHEAST:
						case ht::Replay::Movement::SOUTHEAST:
							ht::getAdjHexCoords( Hex::NORTHWEST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Admin::NORTHEAST:
						case ht::Replay::Exploit::NORTHEAST:
						case ht::Replay::Combat::NORTHEAST:
						case ht::Replay::Movement::NORTHEAST:
							ht::getAdjHexCoords( Hex::SOUTHWEST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Admin::WEST:
						case ht::Replay::Exploit::WEST:
						case ht::Replay::Combat::WEST:
						case ht::Replay::Movement::WEST:
							ht::getAdjHexCoords( Hex::EAST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Admin::SOUTHWEST:
						case ht::Replay::Exploit::SOUTHWEST:
						case ht::Replay::Combat::SOUTHWEST:
						case ht::Replay::Movement::SOUTHWEST:
							ht::getAdjHexCoords( Hex::NORTHEAST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						case ht::Replay::Admin::NORTHWEST:
						case ht::Replay::Exploit::NORTHWEST:
						case ht::Replay::Combat::NORTHWEST:
						case ht::Replay::Movement::NORTHWEST:
							ht::getAdjHexCoords( Hex::SOUTHEAST, rx, ry, &nx, &ny, world_x_size_, world_y_size_ );
							break;
						default:
							break;  // ignore rest, they can't be undone
					}  // switch unit replay
					rx = nx;
					ry = ny;
				}  // while IsValidHex
			}  // end of undo rail breaks, airbase hits and port hits

			unit->setAdminMove( 0 );
			unit->setTacticalMove( false );
			// restore rail capacity if this unit has used any:
			//TODO!
			unit->setStratMove( false );
			unit->setTrain( 0 );
			unit->setAttackDir( 0 );  // land units: clear attack orders
			unit->setMission( AirUnit::Mission::NONE );  // air units: clear mission

			if ( _current_phase == Phase::EXPLOITATION_PHASE )
				unit->clearExplReplay( );
			else
				unit->clearReplay( );

			unit->setHalfMP( FALSE );
			unit->setRailRem( 0 );
			sit_[selected_hex_point_.y][selected_hex_point_.x].removeUnit( unit );
			sit_[y][x].insertUnit( unit );
			// decrease af usage:
			if ( unit->getCounterType( ) == Counter::Type::PLANE )
			{
				( (AirUnit*)unit )->setStaged( FALSE );
				if ( _rule_set.NewAFCapacitySystem == FALSE && _current_phase != Phase::EXPLOITATION_PHASE )
					sit_[y][x].setAFUsage( sit_[y][x].getAFUsage( ) - 1 );
			}
			updateHexCache( x, y );
			PaintHex( x, y );
			HiLiteHex( x, y, true );
			if ( unit->getCounterType( ) == Counter::Type::UNIT )
			{
				GroundUnit* u = (GroundUnit*)unit;
				int mvmt_div = 1;
#if 0
				// TODO: DLT
				if ( RuleSet.rules_variant_ == ERV_A )
				{
					if ( Phase == EXPLOITATION_PHASE )  // exploitation phase
					{
						if ( u->getType( ) == COASTART )
							unit->setMP( 0 );
						else if ( u->isGerman( ) && u->getType( ) == STC )
							// German Static units cannot move in exploitation
							unit->setMP( 0 );
						else if ( ( u->getSupplyTurns( ) >= 3 ) && u->getIsolated( ) )
							// isolated and U-2 or worse = no exploitation movement
							u->setMP( 0 );
						else if ( u->getSupplyTurns( ) >= 5 )
						{
							// U-3 not isolated MP = 1, U-4 (or worse) or isloated MP = 0
							if ( ( u->getSupplyTurns( ) >= 7 ) || u->getIsolated( ) )
								u->setMP( 0 );
							else
								u->setMP( 1 );
						}
						else
						{
							int divisor = 1;
							if ( u->getSupplyTurns( ) >= 1 )
							{
								// U-1 (isolated or not) movement halved
								divisor *= 2;
							}
							if ( u->isSupplementalMotorized( )
								|| u->getType( ) == CAV
								|| u->getType( ) == HVCAV
								|| u->getType( ) == PRTSNCAV)
							{
								// for motorized or cavalry, full movement in exploitation
								divisor *= 1;
							}
							else
							{
								// all other unit type get half movement in exploitation
								divisor *= 2;
							}
							u->setMP( u->getMaxMP( ) / divisor );
						}
					}
					else  // normal movement phase
					{
						if ( u->getSupplyTurns( ) >= 7 )
						{
							// U-4 and isolated MP=1, U-5 and isloated MP=0, not isolated MP halved
							if ( u->getIsolated( ) )
								if ( u->getSupplyTurns( ) >= 9 )
									u->setMP( 0 );
								else
									u->setMP( 1 );
							else
								u->setMP( u->getMaxMP( ) / 2 );
						}
						else
						{
							if ( u->getSupplyTurns( ) >= 1 )
								// U-1 and above, movement factors halved
								u->setMP( u->getMaxMP( ) / 2 );
							else
								u->setMP( u->getMaxMP( ) );
						}
					}
				}
				else
#endif
				if ( u->getSupply( ) == SupplyStatus::REG )  // TODO: DTM (did some earlier refactoring here, make sure merge clean with DLT work above)
				{  // only for WW1 games			// disrupt = 1, bad disrupt = 2
					if ( ( _rule_set.Era & GameEra::WW1 ) && u->getDisruption( ) > DisruptionStatus::NOT_DISRUPTED )
						mvmt_div = 2;
					else
						mvmt_div = 1;
				}
				else if ( u->getSupplyTurns( ) >= 3 )  // U2 or worse
					mvmt_div = 2;
				else if ( u->isSupplementalMotorized( ) )  // U1 c/m halved too
					mvmt_div = 2;
				else
					mvmt_div = 1;  // U1 non-c/m not halved

				int mp_halves = ( u->getMaxMP( ) / mvmt_div ) * 2;
				if ( mp_halves % 2 )  // odd number of halves
					u->setHalfMP( TRUE );

				u->setMP( mp_halves / 2 );  // int division truncates to just the correct whole number
			}
		}  // if (x,y) not current hex
		end:
		unit = next;
	}

	PaintHex( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->clear( );
	contents_pane_->addUnits( sit_[selected_hex_point_.y][selected_hex_point_.x].unit_list_ );
	contents_pane_->Refresh( );
	updateHexCache( selected_hex_point_.x, selected_hex_point_.y );
}

// load selected units to train
void MapPane::ToTrain( )
{
	Counter* cntr = _selected_unit_list;
	if ( ! cntr )
	{
		SelUnitErr( );
		return;
	}

	if ( cntr->getSide( ) != _current_player )
	{
		SideErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( ! IsRRInHex( x, y ) )
	{
		wxMessageBox( wxT("There is no railroad here!") );
		return;
	}

	if ( sit_[y][x].getRRHit( ) )
	{
		wxMessageBox( wxT("Railroad is broken here, can't board train") );
		return;
	}

	if ( sit_[y][x].isRailObstructed( ) )
	{
		wxMessageBox( wxT("Railroad is obstructed here, can't board train") );
		return;
	}

	if ( _rule_set.NoGaugeChk == FALSE && sit_[y][x].getGauge( ) != _current_player )
	{
		if ( wxMessageBox( wxT("Wrong rail gauge, board anyway?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
			return;
	}

	// check all units
	cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) != Counter::Type::UNIT )  // CNTR_RP
		{
			wxMessageBox( wxT("Only land units can board train") );
			return;
		}
		if ( cntr->getStratMove( ) )
		{
			wxMessageBox( wxT("Unit is using strategic movement, it is already in train!") );
			return;
		}
		if ( _rule_set.ToTrain1MP == TRUE && cntr->getMP( ) < 1 )
		{
			wxMessageBox( wxT("Not enough movement points to board train (1 MP needed)") );
			return;
		}
		cntr = cntr->getNext( );
	}

	// all units were ok to move
	float re = 0.0F;
	cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( ! ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getFlag( GroundUnit::RAILONLY ) ) && cntr->getTrain( ) == 0 )
		{
			if ( _rule_set.ToTrain1MP == TRUE )
				cntr->setMP( cntr->getMP( ) - 1 );
			cntr->setTrain( 1 );
			cntr->recordReplay( ht::Replay::Movement::TOTRAIN );
			// calculate REs
			float r = ( (GroundUnit*)cntr )->getRE( );
			// c/m and cav are double
			if ( ( (GroundUnit*)cntr )->isSupplementalMotorized( ) || ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::CAV || ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::HVCAV )
				r *= 2;
			// low-vol rr: double it:
#if 0
			if ( IsLVRailRoad( x, y, direction ) )
				r *= 2;
#endif
			re += r;
		}
		cntr = cntr->getNext( );
	}
	sprintf( msg, "%.1f RE's of rail capacity consumed", re );
	turn_REs_rail_used_ += re;
	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
	contents_pane_->Refresh( );
	showSelUnitMovementArea( );
}

// unload selected units from train
void MapPane::FromTrain( )
{
	Counter* cntr = _selected_unit_list;
	if ( ! cntr )
	{
		SelUnitErr( );
		return;
	}

	if ( cntr->getSide( ) != _current_player )
	{
		SideErr( );
		return;
	}

	while ( cntr )
	{
		cntr->recordReplay( ht::Replay::Movement::FRTRAIN );
		cntr->setTrain( 0 );
		cntr->setRailRem( 0 );
		cntr = cntr->getNext( );
	}

	contents_pane_->Refresh( );
	showSelUnitMovementArea( );
}

// port capacity table
static int port_capacity( int type )
{
	switch ( type )
	{
		case Port::Type::MINOR:
			return 3;
		case Port::Type::STANDARD:
			return 6;
		case Port::Type::MAJOR:
			return 12;
		case Port::Type::GREAT:
			return 24;
	}
	return 0;
}

// load selected units to ship
void MapPane::ToShip( )
{
	Counter* cntr = _selected_unit_list;

	if ( ! cntr )
	{
		SelUnitErr( );
		return;
	}

	if ( cntr->getSide( ) != _current_player )
	{
		SideErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	int port = hex_[y][x].getPortSize( );
	if ( port == 0 && sit_[y][x].isMulberry( ) )
		port = 1;

	bool friendly = ( sit_[y][x].getOwner( ) == _current_player );

	bool beach = false;
	if ( port == 0 )
	{
		if ( wxMessageBox( wxT("There is no port here, use beach instead?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
			return;
		beach = true;
	}
	else  // port present
	{
		if ( sit_[selected_hex_point_.y][selected_hex_point_.x].isPortDestroyed( ) )
		{
			wxMessageBox( wxT("There is a port here, but it's destroyed. Using beach instead.") );
			beach = true;
		}
		else if ( wxMessageBox( wxT("There is a port here, do you wish to use it to board ship?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
		{
			if ( wxMessageBox( wxT("Use beach instead?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
				return;
			beach = true;
		}
	}

	float re = 0.0F;

	// check all units
	cntr = _selected_unit_list;
	while ( cntr )
	{
		// TODO: allow supply pts by ship, too
		if ( cntr->getCounterType( ) != Counter::Type::UNIT && cntr->getCounterType( ) != Counter::Type::RP )
		{
			wxMessageBox( wxT("Only land units and RPs can board ship") );
			return;
		}
		if ( cntr->getStratMove( ) )
		{
			wxMessageBox( wxT("Unit is using strategic movement, it can't board ship!") );
			return;
		}
		// calculate REs
		re += ( (GroundUnit*)cntr )->getRE( );  // TODO: should double this for HE
		cntr = cntr->getNext( );
	}

	if ( ! beach && re > port_capacity( port ) - ( sit_[y][x].getPortUsage( ) + sit_[y][x].getPortHits( ) ) )
	{
		wxMessageBox( wxT("Not enough port capacity, can't board ship!"), Application::NAME, wxOK );
		return;
	}

	// all units were ok to move
	int mp;
	cntr = _selected_unit_list;
	while ( cntr )
	{
		cntr->setShip( 1 );
		// MP calculation
		mp = 1;
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->isSupplementalMotorized( ) )
			mp *= 2;
		if ( beach )
			mp *= 2;
		if ( cntr->getMP( ) <= mp )
			cntr->setMP( 0 );
		else
			cntr->setMP( cntr->getMP( ) - mp );
		cntr = cntr->getNext( );
	}
	if ( beach )
		re = 0;

	// ship MP:
	int smp;
	if ( ! beach )
		smp = 30;  // port
	else if ( friendly )
		smp = 60;  // friendly-owned beach
	else
		smp = 90;  // enemy-owned beach

	if ( beach )
		sprintf( msg, "Unit(s) boarded ship using beach.\nAffected ships should pay %d MP.\n", smp );
	else
		sprintf( msg, "Unit(s) boarded ship using port.\n%.1f REs of port capacity consumed.\nAffected ships should pay %d MP.\n", re, smp );

	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );

	sit_[y][x].setPortUsage( static_cast<int>( sit_[y][x].getPortUsage( ) + re ) );

	contents_pane_->Refresh( );
}

// unload selected units from ship
void MapPane::FromShip( )
{
	Counter* cntr = _selected_unit_list;

	if ( ! cntr )
	{
		SelUnitErr( );
		return;
	}

	if ( cntr->getSide( ) != _current_player )
	{
		SideErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	int portsize = hex_[y][x].getPortSize( );
	bool friendly = ( sit_[y][x].getOwner( ) == _current_player );

	bool beach = false;
	if ( portsize == 0 )
	{
		if ( wxMessageBox( wxT("There is no port here, use beach instead?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
			return;
		beach = true;
	}
	else // a port is available
	{
		if ( sit_[selected_hex_point_.y][selected_hex_point_.x].isPortDestroyed( ) )
		{
			wxMessageBox( wxT("There is a port here, but it's destroyed. Using beach instead.") );
			beach = true;
		}
		else if ( wxMessageBox( wxT("There is a port here, do you wish to use it to leave ship?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
		{
			if ( wxMessageBox( wxT("Use beach instead?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
				return;
			beach = true;
		}
	}

	// check all units
	float re = 0.0F;
	cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) != Counter::Type::UNIT )
		{
			wxMessageBox( wxT("Only land units can be unloaded from the ship") );
			return;
		}

		// calculate REs
		float r = ( (GroundUnit*)cntr )->getRE( );
		// no one pays double when leaving ship:
		//if ( ( (GroundUnit*)cntr )->HasHE( ) )
		//	r *= 2;
		re += r;

		cntr = cntr->getNext( );
	}

	if ( beach )
		re = 0.0F;
	else if ( re > port_capacity( portsize ) - ( sit_[y][x].getPortUsage( ) + sit_[y][x].getPortHits( ) ) )
	{
		wxMessageBox( wxT("Not enough port capacity, can't debark from ship!"), Application::NAME, wxOK );
		// TODO: give opportunity to debark at beach instead??  if so, set beach = true, and re = 0.0F
		return;
	}

	// consume port capacity (if any)
	if ( re > 0.0F )
		sit_[y][x].setPortUsage( static_cast<int>( sit_[y][x].getPortUsage( ) + re ) );

	// all units were ok to move
	cntr = _selected_unit_list;
	while ( cntr )
	{
		// MP calculation
		int mp = 1;

		if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->isSupplementalMotorized( ) )
			mp *= 2;

		if ( beach )
			mp *= 2;

		int remainingMPs = cntr->getMP( ) - mp;

		cntr->setMP( remainingMPs <= 0 ? 0 : remainingMPs );

		// remove selected unit from ship
		cntr->setShip( 0 );

		cntr = cntr->getNext( );
	}

	// ship MPs:
	int shipMPs;
	if ( ! beach )
		shipMPs = 30;  // port
	else if ( friendly )
		shipMPs = 60;  // friendly owned beach
	else
		shipMPs = 90;  // enemy owned beach

	if ( ! beach )
		sprintf( msg, "Unit(s) left ship using port.\n%.1f REs of port capacity consumed.\nAffected ships should pay %d MP.\n", re, shipMPs );
	else
	{
		if ( friendly )
			sprintf( msg, "Unit(s) left ship using friendly beach.\nAffected ships should pay %d MP.\n", shipMPs );
		else
			sprintf( msg, "Unit(s) made amphibious assault to enemy owned beach.\nAffected ships should pay %d MP.\n", shipMPs );
	}

	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );

	// TODO:  is this correct?  if so, why?  perhaps only for an unopposed amphibious assault where landed units are not disrupted?
	sit_[y][x].setOwner( _current_player );

	contents_pane_->Refresh( );
}

void MapPane::ToOverstack( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT )
		{
			if ( c->getSide( ) == _current_player )
			{
				if ( ( (GroundUnit*)c )->getAttackDir( ) || ( (GroundUnit*)c )->getMission( ) )
				{
					char s[100];
					c->getUnitString( s );
					sprintf( msg, "Cancel orders of the following unit?\n%s", s );
					if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
						continue;
				}
				( (GroundUnit*)c )->setOverstack( true );
				( (GroundUnit*)c )->setAttackDir( 0 );
				( (GroundUnit*)c )->setMission( 0 );
			}
			else
			{
				SideErr( );
				return;
			}
		}
	}

	contents_pane_->Refresh( );
}

void MapPane::FromOverstack( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT )
		{
			if ( ( (GroundUnit*)c )->getOverstack( ) && c->getMP( ) < 2 )
			{
				char s[200];
				c->getUnitString( s );
				sprintf( msg, "The following unit doesn't have enough MPs to leave overstack:\n%s", s );
				wxMessageBox( wxS( msg ), Application::NAME );
				break;
			}
			if ( ( (GroundUnit*)c )->getOverstack( ) )
			{
				( (GroundUnit*)c )->setOverstack( false );
				c->setMP( c->getMP( ) - 2 );
			}
		}
	}

	contents_pane_->Refresh( );
}

// MP quick edit
void MapPane::MPPlus( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
		c->setMP( c->getMP( ) + 1 );

	contents_pane_->Refresh( );
	showSelUnitMovementArea( );
}

// MP quick edit
void MapPane::MPMinus( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
		c->setMP( c->getMP( ) - 1 );

	contents_pane_->Refresh( );
	showSelUnitMovementArea( );
}

// MP quick edit
void MapPane::MPMinus30( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
		if ( c->getCounterType( ) != Counter::Type::SHIP )
		{
			wxMessageBox( wxT("This command can only be applied to naval units") );
			return;
		}

	ENUM_SELUNITS(c)
		c->setMP( c->getMP( ) - 30 );

	contents_pane_->Refresh( );
}

// erase above graying
void MapPane::clearSelUnitMovementArea( )
{
	bool needs_repaint = false;
	for ( int yy = 0; yy < world_y_size_; ++yy )
		for ( int xx = 0; xx < world_x_size_; ++xx )
		{
			if ( sit_[yy][xx].isGrayed( ) )
			{
				sit_[yy][xx].setGrayed( false );
				needs_repaint = true;
			}
			sit_[yy][xx].setTmp( 0 );
		}

	if ( needs_repaint )
		Refresh( false );
}

// public
// gray out hexes that are out of reach of selected unit(s)
// use distance field method to calculate reachable hexes.
// TACTICAL MOVEMENT
void MapPane::showSelUnitMovementArea( bool lazy_repaint )
{
	// if neither graying nor clicking: do nothing
	// if just clicking: need to calc distance field!
	if ( ! ::_gray_out_hexes && ! _use_point_and_click_move )
		return;

	if ( _current_phase == Phase::INITIAL_PHASE )
	{
		showSelUnitSupplyRange( lazy_repaint );
		return;
	}

	if ( _current_phase != Phase::MOVEMENT_PHASE && _current_phase != Phase::REACT_MOVE_PHASE && _current_phase != Phase::EXPLOITATION_PHASE && _current_phase != Phase::COMBAT_PHASE )
		return;

	if ( ! isHexSelected( ) || ! _selected_unit_list )
		return;

	Counter* cntr = _selected_unit_list;

	// only planes may move in combat phase (if on-demand rules)
	if ( _current_phase == Phase::COMBAT_PHASE && cntr->getCounterType( ) != Counter::Type::PLANE )
		return;

	if ( cntr->getSide( ) != _phasing_player && _rule_set.NonPhasMove == FALSE && cntr->getCounterType( ) != Counter::Type::PLANE )
		return;

	StatusMsg( "Click target hex to move units. Click home hex or grayed hex to cancel." );
	if ( cntr->getCounterType( ) == Counter::Type::PLANE )
	{
		showSelUnitMovementAreaAir( lazy_repaint );
		return;
	}
	if ( cntr->getCounterType( ) == Counter::Type::SHIP )
	{
		showSelUnitMovementAreaShip( lazy_repaint );
		return;
	}
	if ( move_mode_ == MovementMode::STRATEGIC )
	{
		if ( ! AllSelUnitUsedMoveMode( MovementMode::STRATEGIC ) )
			return;  // TODO: all to gray
		showSelUnitMovementAreaStrat( );
		return;
	}
	if ( cntr->getTrain( ) )
	{
		if ( ! AllSelUnitUsedMoveMode( MovementMode::TACTICAL ) )
			return;  // TODO: all to gray
		showSelUnitMovementAreaTrain( lazy_repaint );
		return;
	}
	if ( move_mode_ == MovementMode::ADMIN )
	{
		if ( ! AllSelUnitUsedMoveMode( MovementMode::ADMIN ) )
			return;  // TODO: all to gray
		showSelUnitMovementAreaAdmin( lazy_repaint );
		return;
	}
#if 0
	 if ( MoveMode == MovementMode::OPERATIONAL )
	 {
		 if ( ! AllSelUnitUsedMoveMode( MovementMode::OPERATIONAL ) )
			 return; // TODO: all to gray
		 ShowSelUnitMovementAreaOperational(lazy_repaint);
		 return;
	 }
#endif
	if ( ! AllSelUnitUsedMoveMode( MovementMode::TACTICAL ) )
		return;  // TODO: all to gray
	// here starts ShowSelUnitMovementAreaTactical

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// find minimum MPs:
	// NOTE: We use counter with smallest MP amount in our calculations.
	//       Sometimes this produces erroneous results, i.e. if there
	//       are pz and jager units in swamp we use jager's which
	//       actually can move much further than pz's. Only solution
	//       would be to calculate own distance fields to all units
	//       and draw own path to all of them. Too much trouble.
	Counter* calc_cntr = 0;
	int max_mp = 256;
	for ( Counter* c = cntr; c; c = c->getNext( ) )
	{
		if ( c->getMP( ) < max_mp )
		{
			max_mp = c->getMP( );
			calc_cntr = c;
		}
	}
	cntr = calc_cntr;

	// quickly erase old distance field data (needed when moving units):
	int max_radius = cntr->getMaxMP( ) + 1;  // for erasing
	for ( int yy = -max_radius; yy <= max_radius; ++yy )
		for ( int xx = -max_radius; xx <= max_radius; ++xx )
			if ( ! coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
				sit_[y + yy][x + xx].setTmp( 0 );

	// recalculate new distance field
	max_radius = max_mp;
	sit_[y][x].setTmp( 0x8000 );  // set starting hex
	int radius;
	for ( radius = 0; radius <= max_radius; ++radius )
	{
		for ( int yy = -radius; yy <= radius; ++yy )
		{
			for ( int xx = -radius; xx <= radius; ++xx )
			{
				if ( coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
					continue;

				// hex was updated last time: calc MP costs to enter neighbors:
				if ( ( sit_[y + yy][x + xx].getTmp( ) & 0xc000 ) == 0x8000 )
				{
					int dir;
					ENUM_DIRS(dir)
						fillTmp( cntr, x + xx, y + yy, dir, 0x8000 );
					// set both 'this' and 'next' masks to avoid
					// unnecessary recalculations:
					sit_[y + yy][x + xx].setTmp( sit_[y + yy][x + xx].getTmp( ) | 0xc000 );
				}
			}
		}
	}

	// show reachable hexes
	for ( int yy = 0; yy < world_y_size_; ++yy )
		for ( int xx = 0; xx < world_x_size_; ++xx )
			if ( xx == x && yy == y )
				sit_[yy][xx].setGrayed( false );
			else if ( sit_[yy][xx].getTmp( ) && ( sit_[yy][xx].getTmp( ) & 0x3fff ) <= max_mp )
				sit_[yy][xx].setGrayed( false );
			else
				sit_[yy][xx].setGrayed( true );

	// TODO: there should be a maprenderer.cpp function to handle this housekeeping
	// don't repaint anything, just gray out more hexes
	if ( lazy_repaint )
	{
		wxRect r;
		r.x = x - radius + 3;
		r.y = y - radius + 3;
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

// Restore all MPs to maximum and set home hex
//		(for undo) to unit's
// If "expl" then restore only C/M units ( expl = TRUE at the
//		beginning of the exploitation phase)
// Pass Phase as parameter to RestoreAllMPs
void MapPane::RestoreAllMPs( int currphase )
{
	for ( int y = 0; y < world_y_size_; ++y )
	{
		for ( int x = 0; x < world_x_size_; ++x )
		{
			Counter* cntr = sit_[y][x].unit_list_;
			while ( cntr )
			{
				cntr->setAdminMove( 0 );
				cntr->setTacticalMove( false );
				cntr->setTrain( 0 );
				cntr->setHomeHex( x, y );
				if ( currphase == Phase::MOVEMENT_PHASE )
					cntr->setStratMove( false );
				cntr->recordReplay( ht::Replay::Movement::END );

				turn_REs_rail_used_ = 0;
				if ( cntr->getCounterType( ) == Counter::Type::PLANE )
					( (AirUnit*)cntr )->setDidPA( FALSE );
				switch ( currphase )
				{
					case Phase::MOVEMENT_PHASE:
						// normal movement phase
						if ( cntr->getCounterType( ) == Counter::Type::UNIT )
						{
							GroundUnit* u = (GroundUnit*)cntr;
							u->setOverrunFail( false );
							if ( u->getSupply( ) == SupplyStatus::NONE )
							{  // unit is out of supply
								if ( u->isSupplementalMotorized( ) )
									cntr->setMP( cntr->getMaxMP( ) / 2 );  // c/m unit MPs halved
								else if ( u->getSupplyTurns( ) > 2 )
									cntr->setMP( cntr->getMaxMP( ) / 2 );  // others halved if >=U2
								else
									cntr->setMP( cntr->getMaxMP( ) );  // non c/m U1 not halved
							}
							else if ( u->getDisruption( ) != DisruptionStatus::NOT_DISRUPTED )
								cntr->setMP( cntr->getMaxMP( ) / 2 );  // disrupted -> halved MPs
							else
								cntr->setMP( cntr->getMaxMP( ) );
						}
						else
							cntr->setMP( cntr->getMaxMP( ) );
						break;
					case Phase::REACT_MOVE_PHASE:
						break;
					case Phase::EXPLOITATION_PHASE:
						if ( cntr->getCounterType( ) == Counter::Type::PLANE )
						{
							// extended range units flying back home (trad.rules):
							// give double MPs:
							if ( ( (AirUnit*)cntr )->getExtRange( ) )
								cntr->setMP( cntr->getMaxMP( ) * 2 );
							// new rules & DAS mission: & 'B': only  MPs
							else if ( _rule_set.OnDemandAirMissions == TRUE && ( (AirUnit*)cntr )->getWasDAS( )
								&& ( (AirUnit*)cntr )->getType( ) == AirUnitType::B )
								cntr->setMP( cntr->getMaxMP( ) / 2 );
							else
								cntr->setMP( cntr->getMaxMP( ) );
							( (AirUnit*)cntr )->setWasDAS( 0 );
						}
						else if ( cntr->getCounterType( ) == Counter::Type::UNIT ) // in expl, not possible to spend 1/2 MPs, no need to track them
						{
							GroundUnit* u = (GroundUnit*)cntr;
							if ( u->isSupplementalMotorized( ) )
							{
								// c/m units always get full MPs for exploitation phase
								// TODO: Cavalry class units get exploit ability in 2010+ series games
								if ( u->getSupply( ) == SupplyStatus::NONE )  // Ux c/m unit always
									cntr->setMP( cntr->getMaxMP( ) / 2 );  // has MPs halved
								else
									cntr->setMP( cntr->getMaxMP( ) );
							}
							// ww1 rules: some other types get MPs too:
							else if ( _rule_set.WW1ExploitMPs == TRUE )
							{
								// TODO: water only: full MPs
								// cav: 1/2
								if ( u->getType( ) == GroundUnitType::WW2::CAV || u->getType( ) == GroundUnitType::WW2::HVCAV || u->getType( ) == GroundUnitType::WW2::PRTSNCAV )
									cntr->setMP( cntr->getMaxMP( ) / 2 );
								// infantry 1/3
								else if ( u->isWW1Infantry( ) )
									cntr->setMP( cntr->getMaxMP( ) / 3 );
								// others: 1/5
								else
									cntr->setMP( cntr->getMaxMP( ) / 5 );
							}
							else
								cntr->setMP( 0 );
						}
						else
							cntr->setMP( 0 );
						break;
					default:
						break;
				}  // end switch
				   // TODO: DLT begin check for merge
#if 0
				}
				else
				{
				 // use divisors and halves to account for
				 // 1/2 MP remainders when divisor > 1
				 int mvmt_div = 1;
				 if ( cntr->Type() == CNTR_UNIT )
				 {
				 GroundUnit* u = (GroundUnit*)cntr;
				 u->SetOverrunFail( 0 );
				 if ( u->GetSupply() == SUPPLY_OUT )
				 { // unit is out of supply
				 if ( u->IsSupplementalMotorized() ) // c/m unit MPs halved if U1 or worse
				 mvmt_div = 2;
				 else if ( u->GetSupplyTurns() >= 3 ) // others halved if U2 or worse
				 mvmt_div = 2;
				 else // non-c/m U1 MPs not halved
				 mvmt_div = 1;
				 } // only for WW1 games			 // disrupt = 1, bad disrupt = 2
				 else if ( (RuleSet.Era & UNITS_WW1) && u->GetDisruption()> NOT_DISRUPTED )
				 mvmt_div = 2;
				 else
				 mvmt_div = 1;
				 }
				 else
				 mvmt_div = 1;

				 int mp_halves = ( ((GroundUnit*)cntr)->GetMaxMP() / mvmt_div ) * 2;
				 if ( mp_halves % 2 ) // odd number of halves
				 cntr->SetHalfMP( TRUE );

				 cntr->SetMP( mp_halves / 2 ); // int division truncates to just the correct whole number
				 }
#endif
				// TODO: DLT end check for merge
				cntr = cntr->getNext( );
			}  // while cntr
		}  // for x
	}  // for y
}

void MapPane::MoveErr( const char* str )
{
	wxMessageBox( wxS( str ), wxT("Movement failed"), wxOK );
	click_marching_mode_ = FALSE;
}

// Variant A allows Reaction Phase
bool MapPane::ReactionCombatWithin( int x, int y, int minhexes, int maxhexes )
{
	for ( int dy = y - maxhexes; dy <= y + maxhexes; ++dy )
		for ( int dx = x - maxhexes - 1; dx <= x + maxhexes + 1; ++dx )
			// TODO: use MapPane::IsValidHex(..) here?
			if ( dx >= 0 && dy >= 0 && dx < world_x_size_ && dy < world_y_size_ )  // check if in map
			{
				int dist = ht::DistBetweenHexes( dx, dy, x, y );
				if ( dist >= minhexes && dist <= maxhexes )
				{
					Counter* cntr;
					ENUM_HEXUNITS(dx, dy, cntr)
					{
						if (	cntr->getCounterType( ) == Counter::Type::NOTE &&
								static_cast<HexNote*>( cntr )->getNoteType( ) == HexNote::GNDREPORT &&
								static_cast<HexNote*>( cntr )->isReactionCombat( )	)
							return true;
					}
				}
			}

	return false;
}

bool MapPane::EnemyUnitWithin( int x, int y, int minhexes, int maxhexes )
{
	for ( int dy = y - maxhexes; dy <= y + maxhexes; ++dy )
		for ( int dx = x - maxhexes - 1; dx <= x + maxhexes + 1; ++dx )
			// TODO: test with this...
			// if ( ! coordinatesOutOfBounds( dx, dy, iXWorldSize, iYWorldSize ) )
			if ( dx >= 0 && dy >= 0 && dx < world_x_size_ && dy < world_y_size_ )  // check if in map
			{
				int dist = ht::DistBetweenHexes( dx, dy, x, y );
				if ( dist >= minhexes && dist <= maxhexes )
				{
					if ( IsEnemyCombatUnits( dx, dy ) )
						return true;
				}
			}

	return false;
}

// any units that can defend themselves
bool MapPane::IsEnemyCombatUnits( int x, int y )
{
	if ( sit_[y][x].getOwner( ) == _current_player )
		return false;

	Counter* cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* u = (GroundUnit*)cntr;
			if ( u->getAtt( ) || u->getDef( ) )
				return true;
		}
		cntr = cntr->getNext( );
	}

	return false;
}

// Variant A module 108 Naval Movement
int MapPane::calculateNavalHexCost( Counter* cp, int x, int y, int dir )
{
	//int ice = Frozen[Hex[y][x].getWeatherZone( )];
	//int w = Weather[Hex[y][x].GetWeatherZone()]; // TODO: weather!!!

	if ( cp->getCounterType( ) != Counter::Type::SHIP )
		return 99;

	int new_x, new_y;
	if ( ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ ) == 0 )
		return 0;

	// TODO: IsPort(..) returns city not ports
	if (	hex_[new_y][new_x].terrain_ != HexType::SEA
			&& ! IsCoastalHex( new_x, new_y )
			&& ! IsRivers( new_x, new_y )
			&& ! IsPort( new_x, new_y )
			&& hex_[new_y][new_x].terrain_ != HexType::ATOLL
			&& hex_[new_y][new_x].terrain_ != HexType::SMALLISLAND
			&& hex_[new_y][new_x].terrain_ != HexType::RESTRICTEDWATERS
			&& hex_[new_y][new_x].terrain_ != HexType::ICINGSEA )
		return 98;
#if 0
	// TODO: DLT Variant A Naval Movement
	if ( RuleSet.rules_variant_ == ERV_A)
	{
		// Variant A River Flotilla movement
		NavalUnit* ship = (NavalUnit*)cp;  // already checked for CNTR_SHIP above
		if ( ship->getShipType( ) == FLEET_RF ) // cp->getCounterType( ) == CNTR_SHIP &&
		{
			if ( hex[new_y][new_x].terrain == SEA && !IsCoastalHex( new_x, new_y ) )
				return 97;
			else
				mps = 1 + calculateZOCCost( cp, x, y, new_x, new_y );
		}
		else
		{
			mps = IsRivers( new_x, new_y ) ? 2 : 1;
		}
	}
	else
	{
#endif

	int mps = IsCoastalHex( new_x, new_y ) ? 2 : 1;

	return mps;
}

// private helper for showSelUnitMovementAreaShip(..)
// fill ship movement tmp data for given target hex (for MP distance field)
void MapPane::fillTmpShip( Counter* cntr, int x, int y, int dir, int mask )
{
	int x2, y2;
	if ( ! ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
		return;

	int m0 = sit_[y][x].getTmp( ) & 0x3fff;  // discard 2 upper bits
	int m = sit_[y2][x2].getTmp( ) & 0x3fff;

	bool uncalculated = ( ( sit_[y2][x2].getTmp( ) & 0xc000 ) ? false : true );

	int mp = ( IsEnemyOccupied( x2, y2 ) ) ? 100 : calculateNavalHexCost( cntr, x, y, dir );

	if ( uncalculated || ( m0 + mp ) < m )
		sit_[y2][x2].setTmp( m0 + ( mp | mask ) );
}

// private helper for showSelUnitMovementArea(..)
void MapPane::showSelUnitMovementAreaShip( bool lazy_repaint )
{
	int x, y, xx, yy, dir, max_mp, radius = 0; //, x2, y2;
	int max_radius; //, this_mask, next_mask;
	Counter* cntr, *c, *calc_cntr = NULL;

	cntr = _selected_unit_list;
	if ( cntr->getSide( ) != _current_player )
		return;
	x = selected_hex_point_.x;
	y = selected_hex_point_.y;
	// find minimum MPs:
	max_mp = 999;
	for ( c = cntr; c; c = c->getNext( ) )
	{
		if ( c->getCounterType( ) != Counter::Type::SHIP )
			max_mp = 0;
		else if ( c->getMP( ) < max_mp )
		{
			max_mp = c->getMP( );
			calc_cntr = c;
		}
	}
	cntr = calc_cntr;
	// recalculate new distance field
	max_radius = cntr->getMaxMP( ) + 1;  // for erasing
	// quickly erase old distance field data (needed when moving units):
	for ( yy = -max_radius; yy <= max_radius; ++yy )
		for ( xx = -max_radius; xx <= max_radius; ++xx )
			if ( ! coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
				sit_[y + yy][x + xx].setTmp( 0 );
	// recalculate new distance field
	max_radius = max_mp;
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
						fillTmpShip( cntr, x + xx, y + yy, dir, 0x8000 );
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

// private helper for showSelUnitMovementAreaAdmin(..)
// fill tmp data for given target hex (for MP distance field)
void MapPane::fillTmpAdmin( Counter* cntr, int x, int y, int dir, int mask )
{
	int x2, y2;
	if ( ! ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
		return;

	int m0 = sit_[y][x].getTmp( ) & 0x3fff;  // discard 2 uppermost bits
	int m = sit_[y2][x2].getTmp( ) & 0x3fff;

	int mp = 0;
	if ( IsEnemyOccupied( x2, y2 ) || sit_[y2][x2].getOwner( ) != _current_player || sit_[y2][x2].getOrigOwner( ) != _current_player
		|| isHexInEnemyZOC( x2, y2 ) )  // TODO: DLT
		mp = 30;
	else if ( _rule_set.OnlyClearAdmin == TRUE
		&& ( ( hex_[y2][x2].terrain_ != HexType::CLEAR || IsMinorRiver( x, y, dir ) )
			&& ! ( IsRoad( x, y, dir ) || IsLVRailRoad( x, y, dir ) || IsRailRoad( x, y, dir ) ) ) )
		mp = 30;
	else if ( IsAdjEnemies( x2, y2 ) )
		mp = 30;
	else
		//mp = CalcMP(cntr, x, y, dir);
		mp = calculateHexCost( cntr, x, y, dir, 0 );  // TODO: DLT

	bool uncalculated = ( ( sit_[y2][x2].getTmp( ) & 0xc000 ) ? false : true );

	// if already calculated AND from-data plus mp is less than to-data
	if ( uncalculated || ( m0 + mp ) < m )
		sit_[y2][x2].setTmp( m0 + ( mp | mask ) );
}

// private helper for showSelUnitMovementArea(..)
void MapPane::showSelUnitMovementAreaAdmin( bool lazy_repaint )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// find minimum MPs:
	int max_mp = 99;
	Counter* cntr = _selected_unit_list;
	Counter* calc_cntr = 0;
	for ( Counter* c = cntr; c; c = c->getNext( ) )
	{
		if ( c->getMP( ) < max_mp )
		{
			max_mp = c->getMP( );
			calc_cntr = c;
		}
	}
	cntr = calc_cntr;

	max_mp *= 2;  // admin!
	if ( cntr->isHalfMP( ) ) 
		++max_mp;

	int max_radius = cntr->getMaxMP( ) * 2 + 1;  // for erasing

	// quickly erase old distance field data (needed when moving units):
	for ( int yy = -max_radius; yy <= max_radius; ++yy )
		for ( int xx = -max_radius; xx <= max_radius; ++xx )
			if ( ! coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
				sit_[y + yy][x + xx].setTmp( 0 );

	// recalculate new distance field
	sit_[y][x].setTmp( 0x8000 );  // set starting hex
	max_radius = max_mp;
	int radius;
	for ( radius = 0; radius <= max_radius; ++radius )
	{
		for ( int yy = -radius; yy <= radius; ++yy )
		{
			for ( int xx = -radius; xx <= radius; ++xx )
			{
				if ( coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
					continue;
				// hex was updated last time: calc mp costs to enter neighbors:
				if ( ( sit_[y + yy][x + xx].getTmp( ) & 0xc000 ) == 0x8000 )
				{
					int dir;
					ENUM_DIRS(dir)
						fillTmpAdmin( cntr, x + xx, y + yy, dir, 0x8000 );
					// set both 'this' and 'next' masks to avoid
					// unnecessary recalculations:
					sit_[y + yy][x + xx].setTmp( sit_[y + yy][x + xx].getTmp( ) | 0xc000 );
				}
			}
		}
	}

	// show reachable hexes
	for ( int yy = 0; yy < world_y_size_; ++yy )
		for ( int xx = 0; xx < world_x_size_; ++xx )
			if ( xx == x && yy == y )
				sit_[yy][xx].setGrayed( false );
			else if ( sit_[yy][xx].getTmp( ) && ( sit_[yy][xx].getTmp( ) & 0x3fff ) <= max_mp )
				sit_[yy][xx].setGrayed( false );
			else
				sit_[yy][xx].setGrayed( true );

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
		if ( max_radius <= gray_out_admin_radius_ )
			dont_redraw_map_ = 1;
		gray_out_admin_radius_ = max_radius;
		gray_out_radius_ = gray_out_rail_radius_ =
		//		GrayingRadiusOperational =
		gray_out_strat_radius_ = 0;
		Refresh( false, nullptr );
	}
}

// private helper for showSelUnitMovementArea(..)
// fill tmp data for given target hex (for MP distance field)
void MapPane::fillTmp( Counter* cntr, int x, int y, int dir, int mask )
{
	int x2, y2;
	if ( ! ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
		return;

	int m0 = sit_[y][x].getTmp( ) & 0x3fff;  // discard 2 uppermost bits
	int m = sit_[y2][x2].getTmp( ) & 0x3fff;

	bool uncalculated = ( ( sit_[y2][x2].getTmp( ) & 0xc000 ) ? false : true );

	// TODO: Total War overrun combat should wire into this formula?
	// treat an enemy-occupied to-hex as 15 MP cost
	int mp = ( IsEnemyOccupied( x2, y2 ) ? 15 : calculateHexCost( cntr, x, y, dir, 0 ) );  // TODO: DLT

	// if already calculated AND from-data plus mp is less than to-data
	if ( uncalculated || ( m0 + mp ) < m )
		sit_[y2][x2].setTmp( m0 + ( mp | mask ) );
}

// return true if all selected units have used movemode 'mode'
bool MapPane::AllSelUnitUsedMoveMode( int mode )
{
	bool tac = false;
	bool adm = false;
	bool strat = false;

	Counter* cntr = nullptr;
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getTacticalMove( ) )
			tac = true;

		if ( cntr->getAdminMove( ) )
			adm = true;

		if ( cntr->getStratMove( ) )
			strat = true;
	}

	switch ( mode )
	{
		case MovementMode::TACTICAL:
			return ( adm || strat ) ? false : true;
		case MovementMode::ADMIN:
			return ( tac || strat ) ? false : true;
		case MovementMode::STRATEGIC:
			return ( adm || tac ) ? false : true;
		default:
			return false;
	}
}

// private helper for showSelUnitMovementArea(..)
void MapPane::showSelUnitMovementAreaTrain( bool lazy_repaint )
{
	Counter* cntr = _selected_unit_list;
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// find minimum MPs:
	int max_mp = 99;
	for ( Counter* c = cntr; c; c = c->getNext( ) )
	{
		int tmp = c->getMP( ) * c->getRailHexes( ) + c->getRailRem( );
		if ( tmp < max_mp )
			max_mp = tmp;
	}
	int max_radius = max_mp + 1;
	// quickly erase old distance field data (needed when moving units):
	for ( int yy = -max_radius; yy <= max_radius; ++yy )
		for ( int xx = -max_radius; xx <= max_radius; ++xx )
			if ( ! coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
				sit_[y + yy][x + xx].setTmp( 0 );

	// recalculate new distance field
	sit_[y][x].setTmp( 0x8000 );  // set starting hex
	max_radius = max_mp;
	int radius;
	for ( radius = 0; radius <= max_radius; ++radius )
	{
		for ( int yy = -radius; yy <= radius; ++yy )
		{
			for ( int xx = -radius; xx <= radius; ++xx )
			{
				if ( coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
					continue;
				// hex was updated last time: calc mp costs to enter neighbors:
				if ( ( sit_[y + yy][x + xx].getTmp( ) & 0xc000 ) == 0x8000 )
				{
					int tmp = sit_[y + yy][x + xx].getTmp( ) & 0x3fff;
					int dir;
					ENUM_DIRS(dir)
					{
						int x2, y2;
						if ( ht::getAdjHexCoords( dir, x + xx, y + yy, &x2, &y2, world_x_size_, world_y_size_ ) )
						{
							if ( sit_[y2][x2].getTmp( ) == 0 || ( sit_[y2][x2].getTmp( ) & 0x3fff ) >= 99 )
							{
								if ( IsRailRoad( x + xx, y + yy, dir ) && ! sit_[y2][x2].getRRHit( ) && ! sit_[y2][x2].isRailObstructed( )
									&& sit_[y2][x2].getOwner( ) == _current_player && sit_[y2][x2].getOrigOwner( ) == _current_player &&
								// gauge:
									( _rule_set.NoGaugeChk == TRUE || sit_[y2][x2].getGauge( ) == _current_player ) )
									sit_[y2][x2].setTmp( ( tmp + 1 ) | 0x8000 );
								else
									sit_[y2][x2].setTmp( 99 | 0xc000 );
							}
						}
					}
					// set both 'this' and 'next' masks to avoid
					sit_[y + yy][x + xx].setTmp( sit_[y + yy][x + xx].getTmp( ) | 0xc000 );
				}
			}
		}
	}

	// show reachable hexes
	for ( int yy = 0; yy < world_y_size_; ++yy )
		for ( int xx = 0; xx < world_x_size_; ++xx )
			if ( xx == x && yy == y )
				sit_[yy][xx].setGrayed( false );
			else if ( sit_[yy][xx].getTmp( ) && ( sit_[yy][xx].getTmp( ) & 0x3fff ) <= max_mp )
				sit_[yy][xx].setGrayed( false );
			else
				sit_[yy][xx].setGrayed( true );

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
		if ( max_radius <= gray_out_rail_radius_ )
			dont_redraw_map_ = 1;
		gray_out_rail_radius_ = max_radius;
		gray_out_radius_ = gray_out_admin_radius_ = gray_out_strat_radius_ = 0;
		Refresh( false, nullptr );
	}
}

// private helper for showSelUnitMovementArea(..)
void MapPane::showSelUnitMovementAreaStrat( bool lazy_repaint )
{
	Counter* cntr = _selected_unit_list;
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// find minimum MPs:
	int max_mp = 256;
	for ( Counter* c = cntr; c; c = c->getNext( ) )
	{
		int tmp = c->getMP( );
		if ( tmp < max_mp ) max_mp = tmp;
	}

	// quickly erase old distance field data (needed when moving units):
	int max_radius = max_mp + 1;
	for ( int yy = -max_radius; yy <= max_radius; ++yy )
		for ( int xx = -max_radius; xx <= max_radius; ++xx )
			if ( ! coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
				sit_[y + yy][x + xx].setTmp( 0 );

	// recalculate new distance field
	sit_[y][x].setTmp( 0x8000 );  // set starting hex
	max_radius = max_mp;
	int radius;
	for ( radius = 0; radius <= max_radius; ++radius )
	{
		for ( int yy = -radius; yy <= radius; ++yy )
		{
			for ( int xx = -radius; xx <= radius; ++xx )
			{
				if ( coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
					continue;

				// hex was updated last time: calc mp costs to enter neighbors:
				if ( ( sit_[y + yy][x + xx].getTmp( ) & 0xc000 ) == 0x8000 )
				{
					int tmp = sit_[y + yy][x + xx].getTmp( ) & 0x3fff;
					int dir;
					ENUM_DIRS(dir)
					{
						int x2, y2;
						if ( ht::getAdjHexCoords( dir, x + xx, y + yy, &x2, &y2, world_x_size_, world_y_size_ ) )
						{
							if ( sit_[y2][x2].getTmp( ) == 0 || ( sit_[y2][x2].getTmp( ) & 0x3fff ) >= 256 )
							{
								if ( IsRailRoad( x + xx, y + yy, dir ) && ! sit_[y2][x2].getRRHit( ) && ! sit_[y2][x2].isRailObstructed( )
									&& sit_[y2][x2].getOrigOwner( ) == _current_player &&
								// gauge:
									( _rule_set.NoGaugeChk == TRUE || sit_[y2][x2].getGauge( ) == _current_player ) )
									sit_[y2][x2].setTmp( ( tmp + 1 ) | 0x8000 );
								else
									sit_[y2][x2].setTmp( 256 | 0xc000 );
							}
						}
					}
					// set both 'this' and 'next' masks to avoid
					sit_[y + yy][x + xx].setTmp( sit_[y + yy][x + xx].getTmp( ) | 0xc000 );
				}
			}
		}
	}

	// show reachable hexes
	for ( int yy = 0; yy < world_y_size_; ++yy )
		for ( int xx = 0; xx < world_x_size_; ++xx )
			if ( xx == x && yy == y )
				sit_[yy][xx].setGrayed( false );
			else if ( sit_[yy][xx].getTmp( ) && ( sit_[yy][xx].getTmp( ) & 0x3fff ) <= max_mp )
				sit_[yy][xx].setGrayed( false );
			else
				sit_[yy][xx].setGrayed( true );

	// don't repaint anything, just gray out more hexes
	if ( lazy_repaint )
	{
		wxRect r;
		r.x = x - radius + 3;
		r.y = y - radius + 3;
		hex2Screen( & ( r.x ), & ( r.y ) );
		r.width = r.height = ( radius + 3 ) * 2 * Hex::SIZE_X;
		dont_redraw_map_ = 1;
		Refresh( false, &r );
	}
	else
	{
		if ( max_radius <= gray_out_strat_radius_ )
			dont_redraw_map_ = 1;
		gray_out_strat_radius_ = max_radius;
		gray_out_radius_ = gray_out_admin_radius_ =
		//		GrayingRadiusOperational =
		gray_out_rail_radius_ = 0;
		Refresh( false, nullptr );
	}
}

// private helper for showSelUnitMovementArea(..) and Land(..)
void MapPane::showSelUnitMovementAreaAir( bool lazy_repaint )
{
	Counter* cntr = _selected_unit_list;
	if ( cntr->getSide( ) != _current_player )
		return;

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// find minimum MPs:
	Counter* calc_cntr = 0;
	int max_mp = 999;
	for ( Counter* c = cntr; c; c = c->getNext( ) )
	{
		if ( ( (AirUnit*)c )->isActive( ) == FALSE )
			max_mp = 0;
		else if ( c->getMP( ) < max_mp )
		{
			max_mp = c->getMP( );
			calc_cntr = c;
		}
	}
	cntr = calc_cntr;

	int max_radius = 100;
	// quickly erase old distance field data (needed when moving units):
	for ( int yy = -max_radius; yy <= max_radius; ++yy )
		for ( int xx = -max_radius; xx <= max_radius; ++xx )
			if ( ! coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
				sit_[y + yy][x + xx].setTmp( 0 );

	// recalculate new distance field
	max_radius = max_mp;
	int this_mask = 0x8000;
	int next_mask = 0x4000;
	sit_[y][x].setTmp( this_mask );  // set starting hex
	int radius;
	for ( radius = 0; radius <= max_radius; ++radius )
	{
		for ( int yy = -radius; yy <= radius; ++yy )
		{
			for ( int xx = -radius; xx <= radius; ++xx )
			{
				if ( coordinatesOutOfBounds( x + xx, y + yy, world_x_size_, world_y_size_ ) )
					continue;

				// hex was updated last time: calc mp costs to enter neighbors:
				if ( ( sit_[y + yy][x + xx].getTmp( ) & 0xc000 ) == this_mask )
				{
					int dir;
					ENUM_DIRS(dir)
					{
						int x2, y2;
						if ( ht::getAdjHexCoords( dir, x + xx, y + yy, &x2, &y2, world_x_size_, world_y_size_ ) && ( ( sit_[y2][x2].getTmp( ) & 0xc000 ) == 0 ) )
							sit_[y2][x2].setTmp( ( radius + 1 ) | next_mask );
					}
					// set both 'this' and 'next' masks to avoid
					// unnecessary recalculations:
					sit_[y + yy][x + xx].setTmp( sit_[y + yy][x + xx].getTmp( ) | 0xc000 );
				}
			}
		}
		int _t = this_mask;
		this_mask = next_mask;
		next_mask = _t;
	}

	// show reachable hexes
	for ( int yy = 0; yy < world_y_size_; ++yy )
		for ( int xx = 0; xx < world_x_size_; ++xx )
			if ( xx == x && yy == y )
				sit_[yy][xx].setGrayed( false );
			else if ( sit_[yy][xx].getTmp( ) && ( sit_[yy][xx].getTmp( ) & 0x3fff ) <= max_mp )
				sit_[yy][xx].setGrayed( false );
			else
				sit_[yy][xx].setGrayed( true );

	// don't repaint anything, just gray out more hexes
	if ( lazy_repaint )
	{
		wxRect r;
		r.x = x - radius + 3;
		r.y = y - radius + 3;
		hex2Screen( & ( r.x ), & ( r.y ) );
		r.width = r.height = ( radius + 3 ) * 2 * Hex::SIZE_X;
		dont_redraw_map_ = 1;
		Refresh( false, &r );
	}
	else
	{
		if ( max_radius <= gray_out_admin_radius_ )
			dont_redraw_map_ = 1;
		gray_out_admin_radius_ = max_radius;
		gray_out_radius_ = gray_out_rail_radius_ =
		//		GrayingRadiusOperational =
		gray_out_strat_radius_ = 0;
		Refresh( false, nullptr );
	}
}

static int GetLowOddsOverrunMPs( GroundUnit* unit, int odds )
{
	int mps;

	if ( unit->isSupplementalMotorized( ) && unit->isGerman( ) )
	{
		if ( odds >= 12 )
			mps = 1;
		else if ( odds >= 10 )
			mps = 2;
		else if ( odds >= 9 )
			mps = 3;
		else if ( odds >= 8 )
			mps = 4;
		else if ( odds >= 7 )
			mps = 5;
		else if ( odds >= 6 )
			mps = 7;
		else
			mps = 9;
	}
	else
	{
		if ( odds >= 12 )
			mps = 2;
		else if ( odds >= 10 )
			mps = 3;
		else if ( odds >= 9 )
			mps = 4;
		else if ( odds >= 8 )
			mps = 5;
		else if ( odds >= 7 )
			mps = 7;
		else
			mps = 9;
	}
	return mps;
}

#define MOVE_DELAY 50  // delay between move steps when clickmarching[ms]

// General unit TACTICAL mover function, move selected units to "direction"
void MapPane::March( int direction )
{
	// what kind of movement is actually wanted?
	switch ( CanMoveStack( direction ) )
	{
		case 0:
			return;
		case 1:
			break;  // tactical
		case 2:
			AdminMove( direction );
			return;
		case 3:
			StrategicMove( direction );
			return;
		case 4:
			TrainMove( direction );
			return;
		case 6:
			AirMove( direction );
			return;
		case 5:
		case 7:
			ShipMove( direction );
			return;
		case 8:
			break;  // combat phase: attack
	}

	int new_x, new_y;
	if ( ! ht::getAdjHexCoords( direction, selected_hex_point_.x, selected_hex_point_.y, &new_x, &new_y, world_x_size_, world_y_size_ ) )
	{
		wxMessageBox( wxT("Please stay on the map!"), Application::NAME, wxOK );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;  // save typing...
	//har = Sit[y][x].GetHarrasment();

	// calculate MPs:
	char str[128];
	int partisans = 1;
	int asked_combat_phase = FALSE;
	int i = 0;
	int new_mps[32];
	int min_mps = 999;
	int mps;
	int has_zoc = 0;

	Counter* unit = _selected_unit_list;
	while ( unit ) // Weather[Sit[y][x].WeatherZone]
	{
		if ( ! IsPartisanUnit( unit ) )
			partisans = 0;

		if ( _current_phase != Phase::COMBAT_PHASE && unit->getAdminMove( ) )
		{
			unit->getUnitString( str );
			sprintf( msg, "The following unit has used administrative movement\nand thus is unable to use tactical movement:\n\n%s", str );
			wxMessageBox( wxS( msg ) );
			return;
		}
		if ( unit->getMission( ) )
		{
			unit->getUnitString( str );
			sprintf( msg, "Cancel construction work?\n%s", str );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_INFORMATION ) != wxYES )
				return;
			unit->setMission( 0 );
		}
		int tmp;
		if ( ( tmp = unit->getAttackDir( ) ) != 0 )
		{
			unit->getUnitString( str );
			sprintf( msg, "The following unit has been ordered to attack enemy forces:\n\n%s\n\nDo you want to cancel this order?", str );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_INFORMATION ) != wxYES )
				return;
			unit->setAttackDir( 0 );  // cancel attack orders
			contents_pane_->Refresh( );
			int tmp_x, tmp_y;
			ht::getAdjHexCoords( tmp, x, y, &tmp_x, &tmp_y, world_x_size_, world_y_size_ );
			if ( ! IsHexAttacked( tmp_x, tmp_y ) )
			{
				wxMessageBox( wxT("NOTE:\nno forces were left to attack target hex,\nattack canceled"), Application::NAME, wxOK | wxICON_INFORMATION );
				sit_[tmp_y][tmp_x].setTarget( false );
			}
		}
		if ( _current_phase == Phase::COMBAT_PHASE )
		{  // in combat phase no MPs required
		   // ask whether to move in combat phase:
			if ( 	! asked_combat_phase
					&& ! IsEnemyOccupied( new_x, new_y )
					&& wxMessageBox( wxT("Combat phase: movement is covered by special rules\n"
						"that are not handled by HexTools. If you move you should\n"
						"adjust MPs manually.\n\n"
						"Really move?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
				mps = 9999;
			else
				mps = 0;
			asked_combat_phase = TRUE;
		}
		else
			mps = calculateHexCost( unit, x, y, direction, 0 );  // + har;
		//sprintf(str, "Hex requires %d MP", mps);
		//wxMessageBox(wxS(str), Application::NAME);
		/* TODO: this was wrong, vet with new code hereafter
		 ( Hex[y][x].getWeatherZone( ) == 0 // A
		 || Hex[y][x].getWeatherZone( ) == 'A' - 'E'
		 || Hex[y][x].getWeatherZone( ) == 'A' - 'F' ) */
		if ( 	( (GroundUnit*)unit )->hasNormalZOC( )
				|| ( (GroundUnit*)unit )->hasReducedZOC( )
				|| ( hexInExoticZone( x, y ) && unit->getSize( ) >= GroundUnit::Size::REGIMENT ) )
			has_zoc = 1;
		new_mps[i] = unit->getMP( ) - mps;
		if ( new_mps[i] < 0 )  // movement failed, check for attack
		{
			if ( ! IsEnemyOccupied( new_x, new_y ) )
			{
				if ( mps > 100 )
				{
					wxMessageBox( wxT("Prohibited direction") );
					return;
				}
				else  // it's always possible to move one hex
				{
					int hx, hy;
					unit->getHomeHex( &hx, &hy );
					if ( hx == x && hy == y && unit->getMP( ) == unit->getMaxMP( ) )
						new_mps[i] = 0;  // allow to move, cost is all MPs
					else
					{
						sprintf( msg, "Not enough MPs (%d required, %d left)\n\nHINT: if you still want to move, use 'MP+'\nbutton to increase unit's MPs", mps, unit->getMP( ) );
						wxMessageBox( wxS( msg ), Application::NAME, wxOK );
						return;
					}
				}
			}
		}
		// reduce ZOC costs paid for overrun:
		new_mps[i] += ReduceOverrunZOC( unit, x, y, direction );
		// record lowest MPs
		if ( new_mps[i] < min_mps )
			min_mps = new_mps[i];
		++i;
		unit = unit->getNext( );
	}  // closing brace for while (unit) calculate MPs loop

	int odds;
	Counter* cp;
	float as, ds;
	int did_overrun = 0;

	// overrun/attack:
	if ( IsEnemyOccupied( new_x, new_y ) )
	{
		as = CalcAllSelAttStr( direction );
		ds = CalcDefStr( new_x, new_y );
		if ( ds == 0.0 )
			odds = 1000;
		else
			odds = (int) ( as / ds);  // rounds down

		int overrun_odds = 200, o_odds;
		// low odds overruns:
		if ( _rule_set.TEM40Overruns == TRUE )
		{
			//int ger_cm = 1; // German c/m pay less

			i = 0;  // new_mps[] index
			ENUM_SELUNITS( cp )
			{
				// German c/m
				if ( ( (GroundUnit*)cp )->isGerman( ) && ( (GroundUnit*)cp )->isSupplementalMotorized( ) )
				{
					switch ( new_mps[i] )
					{
						case 0:
							o_odds = 100;
							break;
						case 1:
							o_odds = 12;
							break;
						case 2:
							o_odds = 10;
							break;
						case 3:
							o_odds = 9;
							break;
						case 4:
							o_odds = 8;
							break;
						case 5:
							o_odds = 7;
							break;
						case 6:
							o_odds = 7;
							break;
						case 7:
							o_odds = 6;
							break;
						case 8:
							o_odds = 6;
							break;
						case 9:
							o_odds = 5;
							break;
						case 10:
							o_odds = 5;
							break;
						default:
							o_odds = 100;
							break;
					}
				}
				else  // other than German c/m pay more:
				{
					switch ( new_mps[i] )
					{
						case 0:
							o_odds = 100;
							break;
						case 1:
							o_odds = 100;
							break;
						case 2:
							o_odds = 12;
							break;
						case 3:
							o_odds = 10;
							break;
						case 4:
							o_odds = 9;
							break;
						case 5:
							o_odds = 8;
							break;
						case 6:
							o_odds = 8;
							break;
						case 7:
							o_odds = 7;
							break;
						case 8:
							o_odds = 7;
							break;
						case 9:
							o_odds = 6;
							break;
						case 10:
							o_odds = 6;
							break;
						default:
							o_odds = 100;
							break;
					}
				}
				if ( o_odds < overrun_odds )
					overrun_odds = o_odds;
				++i;
			}
			//sprintf(msg, "overrun odds: %d:1", overrun_odds);
			//wxMessageBox(msg);
		}
		else
			// not TEM40 overrun rules
			overrun_odds = 10;  // normal rules: 10:1 needed for overrun

		if ( odds < overrun_odds || _current_phase == Phase::COMBAT_PHASE )
		{
			if ( _current_phase == Phase::COMBAT_PHASE || _current_phase == Phase::REACT_COMB_PHASE )
				sprintf( msg, "Do you want to designate these units to attack?\n(Your stack has strength of %.2f points)", as );
			else if ( _current_phase == Phase::MOVEMENT_PHASE || _current_phase == Phase::REACT_MOVE_PHASE )
				sprintf( msg, "Odds %d:1 is not sufficient for the overrun,\n(you need at least %.2f points attack strength.)\n\nDo you want to designate these units to attack?\n(Your stack has strength of %.2f points)", odds, overrun_odds * ds, as );
			else
			{  // Phase == EXPLOITATION_PHASE
				sprintf( msg, "Odds %d:1 is not sufficient for the overrun.\nYou need at least %.2f points attack strength,\nyour stack has strength of %.2f points.", odds, overrun_odds * ds, as );
				wxMessageBox( wxS( msg ), wxT("Can't go there!"), wxOK | wxICON_INFORMATION );
				return;
			}
			if ( wxMessageBox( wxS( msg ), wxT("Can't go there!"), wxYES_NO | wxICON_QUESTION ) == wxYES )
			{
				sit_[new_y][new_x].setTarget( true );
				unit = _selected_unit_list;
				while ( unit )
				{
					unit->setAttackDir( direction );
					unit = unit->getNext( );
				}
				Refresh( );
				contents_pane_->Refresh( );
			}
			return;
		}  // if (odds < overrun_odds || Phase == COMBAT_PHASE )
		else if ( IsGroundedEnemyPlanes( new_x, new_y ) )
		{
			wxMessageBox( wxT("There are enemy planes in the target hex. Please check for\nair unit escape and move/destroy enemy planes before entering\nthe hex."), Application::NAME, wxOK );
			return;
		}
		else if ( odds == 1000 )
		{  // no enemies that can defend themselves
			if ( wxMessageBox( wxT("Destroy enemy non-combat units?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
			{
				i = DestroyHexUnits( new_x, new_y );
				sprintf( msg, "Destroyed %d enemy non-combat units", i );
				wxMessageBox( wxS( msg ), Application::NAME );
			}
		}  // else if (odds == 1000)
		else
		{  // enemy combat units, but able to do overrun
			unit = _selected_unit_list;
			i = 0;
			while ( unit )
			{
				if ( _rule_set.TEM40Overruns == TRUE )
					mps = GetLowOddsOverrunMPs( (GroundUnit*)unit, odds );
				else  // "normal" overrun rules
				{
					// TODO: DLT
					// if ( RuleSet.RulesVariant == ERV_A ) mps = ( odds < 12 ? 3 : ( odds < 15 ? 2 : 1 ) ); else
					mps = ( odds < 12 ? 3 : 2 );
					if ( ( (GroundUnit*)unit )->isSupplementalMotorized( ) && ( (GroundUnit*)unit )->isGerman( ) )
						--mps;
				}
#if 0
				sprintf(str, "Overrun requires %d MP", mps);
				wxMessageBox(wxS(str), Application::NAME);
				// return MPs paid for ZOC if ZOC came from destination hex only
				// DONE ALREADY
				new_mps[i] += ReduceOverrunZOC(unit, x, y, direction);
				sprintf(msg, "MPs required = %d ", mps);
				wxMessageBox(wxS(msg), Application::NAME);
#endif
				new_mps[i] -= mps;
				if ( new_mps[i] < 0 )
				{
					unit->getUnitString( msg );
					sprintf( str, "This unit has not enough MPs for overrun (%d required, %d left, odds %d:1)\n"
							"%s\n\n"
							"Do you want to designate these units to attack?\n"
							"(Your stack has strength of %.2f points)", unit->getMP( ) - new_mps[i], unit->getMP( ),
							odds, msg, as );

					if ( wxMessageBox( wxS( str ), wxT("Can't go there!"), wxYES_NO | wxICON_QUESTION) == wxYES )
					{
						sit_[new_y][new_x].setTarget( true );
						unit = _selected_unit_list;
						while ( unit )
						{
							unit->setAttackDir( direction );
							unit = unit->getNext( );
						}
						Refresh( );
						contents_pane_->Refresh( );
					}
					return;
				}
				unit = unit->getNext( );
				++i;
			} // while( unit )

			if ( odds >= 10 )
				sprintf( msg, "Do you want to make an overrun?\n\nOdds: %d:1", odds );
			else // low odds overrun
				sprintf( msg, "Do you want to try to make a low-odds overrun?\n\nOdds: %d:1", odds );
			if ( wxMessageBox( wxS( msg ), wxT("Overrun"), wxYES_NO ) != wxYES )
				return;

			if ( odds >= 10 )  // normal overrun
			{
				i = DestroyHexUnits( new_x, new_y );
				// Add combat note for normal overrun
				// add combat report to the hex
				HexNote* combat_report;
				sprintf( msg, "Turn: %s %s %d\n"
						"%s attacked with %.2f points\n"
						"%s defended with %.2f points\n\n"
						"Odds: %d\n\n"
						"Result: %s", _month_string[_month], _turn_numeral[_turn - 1], _year, _current_player == SidePlayer::AXIS ? "Axis" : "Allied", as,
						_current_player == SidePlayer::AXIS ? "Allied" : "Axis", ds,
						// TODO: DLT Extreme CRT
						odds, "DE" );
				combat_report = new HexNote( HexNote::GNDREPORT );
				combat_report->setCaption( "Overrun report" );
				combat_report->setText( msg );
				insertUnit( new_x, new_y, combat_report );
#if 0
				 sprintf(msg, "Crushed %d enemy units!\n\n"
				 "MPs of involved units have been reduced, you may enter\n"
				 "the hex with 'Move->Advance' command without paying\n"
				 "any more MPs.",  i);
				 wxMessageBox(wxS(msg), Application::NAME);
#endif
				did_overrun = 1;
				// need to set attack dir so that units can advance
				ENUM_SELUNITS(cp)
				{
					( (GroundUnit*)cp )->setAttackDir( direction );
				}
			}
			else  // low odds overrun
			{
				// prepare target hex for combat:
				sit_[new_y][new_x].setTarget( true );
				ENUM_SELUNITS(cp)
					( (GroundUnit*)cp )->setAttackDir( direction );

				int r = ResolveCombat( false, new_x, new_y, true );
				if ( r < 0 ) // 'Cancel' button was hit
					return;

				if ( r != CombatResult::DE ) // failed
				{
					wxMessageBox( wxT("Result was not DE => overrun failed, unit may not\nmove any more or attack in the combat phase.\n"), Application::NAME );
					ENUM_SELUNITS(cp)
					{
						( (GroundUnit*)cp )->setMP( 0 );
						( (GroundUnit*)cp )->setOverrunFail( true );
						( (GroundUnit*)cp )->setAttackDir( 0 );
					}
					return;
				}
				// success (DE), see if target hex is empty
				// reduce MPs
				i = 0;
				// new_mps calculated above
				wxMessageBox( wxT("Low-odds overrun succeeded!\n\nMPs of involved units have been reduced, you may enter\nthe hex with 'Move->Advance' command without paying\nany more MPs."), Application::NAME );
				did_overrun = 1;
				// kludge #9912: ResolveCombat() deselects all units, so we should
				// re-select them here, or no MPs will be reduced.
				cp = sit_[y][x].unit_list_;
				while ( cp )
				{
					if ( cp->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cp )->getAttackDir( ) == direction )
					{
						contents_pane_->selectUnit( cp );
						cp = sit_[y][x].unit_list_;  // needs to be reset after selection
					}
					else
						cp = cp->getNext( );
				}
			}  // if (odds >= 10)
		}  // closing brace for if (odds < overrun_odds || Phase == COMBAT_PHASE) - else clause
	}  // closing brace for if (IsEnemyOccupied(new_x, new_y)) overrun/attack block

	//if (IsEnemyShipsInPort)... // TODO: not implemented (yet)

	// all units were able to move, now reduce their MPs
	unit = _selected_unit_list;
	i = 0;
	while ( unit )
	{
#if 0
		// rewritten below
		if ( did_overrun )  // don't record path if overrun was made; record it at 'Advance'
			;	// TODO: suspicious semicolon
		else if ( _current_phase == Phase::EXPLOITATION_PHASE )
			unit->recordReplay( dir2replayExploitation( direction ), unit->getMP( ) - new_mps[i] );
		else
			unit->recordReplay( dir2replay( direction ), unit->getMP( ) - new_mps[i] );
#endif
		// don't record overrun as movement path
		if ( ! did_overrun )
		{
			if ( _current_phase == Phase::EXPLOITATION_PHASE )
				unit->recordReplay( dir2replayExploitation( direction ), unit->getMP( ) - new_mps[i] );
			else
				unit->recordReplay( dir2replay( direction ), unit->getMP( ) - new_mps[i] );
		}

		unit->setMP( new_mps[i] );
		unit->setTacticalMove( true );
		unit = unit->getNext( );
		++i;
	}

	sit_[new_y][new_x].setSupply( SupplyStatus::REG );
	// grab ownership of the hex (ZOCs changes owner too), if moving units
	// are NOT partisans
	if ( ! partisans )
	{
		sit_[new_y][new_x].setOwner( _current_player, true );  // true: obstruct rail if ownership changes
		updateHitMarkers( new_x, new_y );

		if ( has_zoc )
		{
			for ( int dir = 1; dir <= 32; dir <<= 1 )
			{
				int tmpx, tmpy;
				if ( ht::getAdjHexCoords( dir, new_x, new_y, &tmpx, &tmpy, world_x_size_, world_y_size_ ) )
					if ( ! isHexInEnemyZOC( tmpx, tmpy )
						&& ! isHexInEnemyRZOC( tmpx, tmpy )
						&& ! IsEnemyUnits( tmpx, tmpy ) )
					{
						sit_[tmpy][tmpx].setOwner( _current_player, true );  // true: obstruct rail if ownership changes
						updateHitMarkers( tmpx, tmpy );
						sit_[tmpy][tmpx].setSupply( SupplyStatus::REG );
						//PaintHex(tmpx, tmpy);
					}
			}
		}
	}

	if ( did_overrun )
	{
#if 0
		if ( _rule_set.rules_variant_ == RulesVariant::A )
			Advance( true ); // boolean flag for overrun
#endif
		updateHexCache( new_x, new_y );
		contents_pane_->Refresh( );
		return;
	}

	// normal movement (target hex entered), move units
	UnHiLiteHex( );
	contents_pane_->clear( );
	contents_pane_->addUnits( _selected_unit_list );
	contents_pane_->markAllSelected( );  // select selected units
	contents_pane_->addUnits( sit_[new_y][new_x].unit_list_ );  // non-selected mode
	contents_pane_->Refresh( false );
	int old_x = selected_hex_point_.x;
	int old_y = selected_hex_point_.y;
	selected_hex_point_.x = new_x;
	selected_hex_point_.y = new_y;
	updateHexCache( old_x, old_y );
	PaintHex( old_x, old_y );
	updateHexCache( new_x, new_y );
	UnHiLiteHex( );
	HiLiteHex( selected_hex_point_.x, selected_hex_point_.y );

	if ( click_marching_mode_ == TRUE )
		wxMilliSleep( MOVE_DELAY );  // milliseconds
	else
		showSelUnitMovementArea( true );

	file_is_dirty_ = true;
}

// get attack str of selected units
// NOTE: units do not need to have attack orders (vrt. CalcSelAttStr())
float MapPane::CalcAllSelAttStr( int dir )
{
	float str = 0;
	Counter* unit = _selected_unit_list;
	while ( unit )
	{
		if ( unit->getCounterType( ) == Counter::Type::UNIT )
			str += CalcUnitAttStr( (GroundUnit*)unit, selected_hex_point_.x, selected_hex_point_.y, dir, IsHexSupported( selected_hex_point_.x, selected_hex_point_.y, dir, true ) );

		unit = unit->getNext( );
	}
	return str;
}

// remove, delete and mark to log
// NOTE: don't destroy flying planes!
int MapPane::DestroyHexUnits( int x, int y )
{
	Counter* new_unitlist = NULL;
	int c = 0;

	Counter* unit = sit_[y][x].unit_list_;
	while ( unit )
	{
		Counter* tmp = unit->getNext( );

		// destroy everything except permanent airfields
		if ( unit->getCounterType( ) == Counter::Type::MISC
			&& (	static_cast<Facility*>( unit )->getMiscType( ) == Facility::PERMAF3 ||
					static_cast<Facility*>( unit )->getMiscType( ) == Facility::PERMAF6 ||
					static_cast<Facility*>( unit )->getMiscType( ) == Facility::PERMAF9 ||
					static_cast<Facility*>( unit )->getMiscType( ) == Facility::PERMAF12 ) )
		{
			unit->setNext( new_unitlist );
			new_unitlist = unit;
		}
		else if ( unit->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)unit )->getInAir( ) )
		{
			unit->setNext( new_unitlist );
			new_unitlist = unit;
		}
		// TODO: if a temp AF was destroyed and no more air facilities remain, AF hits should also be removed
		else if ( unit->getCounterType( ) == Counter::Type::HIT || unit->getCounterType( ) == Counter::Type::NOTE )
		{
			unit->setNext( new_unitlist );
			new_unitlist = unit;
		}
		else
		{
			++c;
			DestroyUnit( unit );  // log losses
		}
		unit = tmp;
	}
	sit_[y][x].unit_list_ = new_unitlist;
	PaintHex( x, y );
	return c;
}

bool MapPane::IsGroundedEnemyPlanes( int x, int y )
{
	if ( sit_[y][x].getOwner( ) == _current_player )
		return false;

	Counter* c;
	ENUM_HEXUNITS(x, y, c)
		if ( c->getCounterType( ) == Counter::Type::PLANE && c->getInAir( ) == FALSE )
			return true;

	if ( isCurrentHex( x, y ) )
		ENUM_SELUNITS(c)
			if ( c->getCounterType( ) == Counter::Type::PLANE && c->getInAir( ) == FALSE )
				return true;

	return false;
}

bool MapPane::IsEnemyUnits( int x, int y )
{
	if ( sit_[y][x].getOwner( ) == _current_player )
		return false;

	if ( sit_[y][x].getUnitCount( ) == 0 )
		return false;

	// if there are some units check if they are combat units (that
	// are able to own hexes):
	Counter* cp;
	ENUM_HEXUNITS(x,y,cp)
	{
		if ( cp->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* up = (GroundUnit*)cp;
			if ( up->getType( ) == GroundUnitType::WW2::POS_HV_AA || up->getType( ) == GroundUnitType::WW2::POS_LT_AA )
				continue;
			return true;
		}
	}

	return false;
}

void MapPane::AirMove( int direction )
{
	int new_x, new_y;
	ht::getAdjHexCoords( direction, selected_hex_point_.x, selected_hex_point_.y, &new_x, &new_y, world_x_size_, world_y_size_ );

	Counter* cntr;
	for ( cntr = _selected_unit_list; cntr; cntr = cntr->getNext( ) )
	{
		if ( cntr->getCounterType( ) != Counter::Type::PLANE )
		{  // sanity check: should've been
			wxMessageBox( wxT("Only planes can fly!") );  // checked in CanMoveStack()
			return;
		}
#if 0
		if ( ((AirUnit*)cntr)->GetMission() != AM_NONE )
		{
			cntr->GetUnitString( s );
			sprintf( msg, "Do you want to cancel mission of:\n%s", s );
			if ( wxMessageBox( msg, Application::NAME, wxICON_QUESTION | wxYES_NO ) != wxYES )
		    	return;
		}
#endif
	}

	for ( cntr = _selected_unit_list; cntr; cntr = cntr->getNext( ) )
		if ( cntr->getMP( ) == 0 )
			return;

	if ( ! Takeoff( ) )  // check for takeoff?
		return; // not all planes were able to takeoff, can't fly stack

	// check for canceled missions
#if 0
	 for (cntr = SelectedUnits; cntr; cntr = cntr->GetNext()) {
	 if (cntr->GetMission() != AM_NONE) {
	 if (wxMessageBox("Do you want to cancel the air operation?",
	 "Check this out", wxICON_QUESTION|wxYES_NO) != IDNO) {
	 cntr->SetMission(AM_NONE);
	 cur_x = ptCurrentHex.x;
	 cur_y = ptCurrentHex.y;
	 if (!IsHexAirAttacked(cur_x, cur_y))
	 Sit[cur_y][cur_x].SetAirTarget(0);
	 }
	 else
	 return;
	 }
	 }
#endif

	for ( cntr = _selected_unit_list; cntr; cntr = cntr->getNext( ) )
	{
		cntr->setMP( cntr->getMP( ) - 1 );
		cntr->recordReplay( _current_phase == Phase::EXPLOITATION_PHASE ? dir2replayExploitation( direction ) : dir2replay( direction ) );
		//cntr->SetMission(AM_NONE);
	}

	contents_pane_->clear( );
	contents_pane_->addUnits( _selected_unit_list );
	contents_pane_->markAllSelected( );  // moving stack is selected
	contents_pane_->addUnits( sit_[new_y][new_x].unit_list_ );  // non-selected units

	UnHiLiteHex( );

	int old_x = selected_hex_point_.x;
	int old_y = selected_hex_point_.y;
	selected_hex_point_.x = new_x;
	selected_hex_point_.y = new_y;
	updateHexCache( old_x, old_y );
	updateHexCache( new_x, new_y );

	UnHiLiteHex( );

	HiLiteHex( selected_hex_point_.x, selected_hex_point_.y );

	contents_pane_->Refresh( false );
}

// make all possible checks to see if it really is possible to move
// selected stack of units.
// return:
// 0 = can't move
// 1 = land units, tactical
// 2 = land units, administrative
// 3 = land units, strategic
// 4 = land units in train
// 5 = land units in ship
// 6 = planes
// 7 = ships
// 8 = land unit attack (combat phase)
int MapPane::CanMoveStack( int dir )
{
	int pp = ( _current_player == _phasing_player );  // pp nonzero if phasing player is active
	int np = ( _current_player != _phasing_player );  // np nonzero if non-phasing player is active

	Counter* cntr = _selected_unit_list;
	if ( ! cntr )  // no (own) units, can't move
		return 0;

	if ( cntr->getSide( ) != _current_player )
	{  // can't move enemy units
		wxMessageBox( wxT("Wrong side") );
		return 0;
	}

	int planes = 0, units = 0, ships = 0;

	// combat phase: no moving, but can attack
	if ( _current_phase == Phase::COMBAT_PHASE && pp )
	{
		ENUM_SELUNITS(cntr)
		{
			// on-demand missions: can fly GS/DAS in combat phase
			if ( _rule_set.OnDemandAirMissions == TRUE && cntr->getCounterType( ) == Counter::Type::PLANE )
				++planes;
			else if ( cntr->getCounterType( ) == Counter::Type::UNIT )  // not land unit, no movement
				++units;
			else if ( cntr->getCounterType( ) == Counter::Type::SHIP )  // Allow ships to attack in combat phase
				++ships;
			else
				return 0;
		}
		if ( planes && units )
			return 0;
		if ( units && ships )  // Allow ships to attack in combat phase
			return 0;
		if ( planes && ships )
			return 0;
		//if (units)
		//  return 8;
		if ( planes )
			return 6;
		//if (ships) // Allow ships to attack in combat phase
		//	return 7;
	}

	// initial phase: only lanes may move
	if ( _current_phase == Phase::INITIAL_PHASE )
	{		// no moving, but can attack
		ENUM_SELUNITS( cntr )
			if ( cntr->getCounterType( ) != Counter::Type::PLANE || cntr->getSide( ) != _current_player )
				return 0;
		return 6;
	}

	if ( _current_phase == Phase::NO_GAME || _current_phase == Phase::SETUP_PHASE || _current_phase == Phase::INITIAL_PHASE )
		return 0;
	if ( ! isHexSelected( ) )  // shouldn't happen... // TODO: DTM (then assert that it shouldn't)
		return 0;

	cntr = _selected_unit_list;
	int type = cntr->getCounterType( );
	cntr = cntr->getNext( );
	while ( cntr )
	{
		// can't move if any counter is different type
		if ( cntr->getCounterType( ) != type )
		{
			// exception:  ground units and naval units (assumes embarked)
			if ( ( cntr->getCounterType( ) == Counter::Type::SHIP && type == Counter::Type::UNIT )
				|| ( cntr->getCounterType( ) == Counter::Type::UNIT && type == Counter::Type::SHIP ) )
			{
				cntr = cntr->getNext( );
				continue;
			}
			MoveErr( "You can only move counters of same type in one stack" );
			return 0; // can't move
		}
		cntr = cntr->getNext( );
	}

	ENUM_SELUNITS( cntr )
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && static_cast<GroundUnit*>( cntr )->getOverstack( ) )
		{
			wxMessageBox( wxT("Units in overstack may not move!"), Application::NAME );
			return 0;
		}

	cntr = _selected_unit_list;
	// can't move misc counters
	if ( cntr->getCounterType( ) == Counter::Type::MISC )
		return 0;

	if ( cntr->getCounterType( ) == Counter::Type::PLANE )
	{
		// traditional air rules
		if ( _rule_set.OnDemandAirMissions == FALSE )
		{
			if ( _current_phase == Phase::REACTION_PHASE && np )
			{
				// in reaction phase only non-ph fighter can fly
				ENUM_SELUNITS( cntr )
				{
					if ( static_cast<AirUnit*>( cntr )->getType( ) != AirUnitType::F && ( (AirUnit*)cntr )->getType( ) != AirUnitType::HF )
					{
						MoveErr( "Only fighters can fly in the reaction phase!" );
						return 0;  // non-fighter plane(s) selected, no way
					}
					return 6;
				}
			}
			// DAS planes can be flown by non-ph player in exploitation phase
			if ( _current_phase == Phase::EXPLOITATION_PHASE && np )
				return 6;  // only planes flying DAS should be in air...
			// no flying in below phases or by non-phasing player
			//if ( Phase == COMBAT_PHASE || Phase == INITIAL_PHASE || np )
			//  return 0;
			return 6;  // now we have phasing player and allowed phase
		}
		else // on-demand missions
		{
			return 6;  // both players can fly at any time
		}
	}
	cntr = _selected_unit_list;
	if ( _current_phase == Phase::REACTION_PHASE )  // Only planes (handled above) can move in
		return 0;  // reaction phase.

	// ships:
	if ( cntr->getCounterType( ) == Counter::Type::SHIP )
	{
		if ( _current_phase == Phase::COMBAT_PHASE )
			return 0;
		if ( _rule_set.SeparateNavalPhases == TRUE &&
				( ( _current_phase < Phase::NAVAL_PHASE_1 || _current_phase > Phase::NAVAL_PHASE_5 ) && ( _current_phase < Phase::NAVAL_PHASE_6 || _current_phase > Phase::NAVAL_PHASE_10 ) ) )
			return 0;
		return 7;
	}

	if ( cntr->getCounterType( ) != Counter::Type::UNIT )
		return 0;

	// ok, we have only land units selected!

	// non-phasing player: depends on rule
	if ( ( np && _rule_set.NonPhasMove == FALSE ) && ( np && _current_phase != Phase::REACT_MOVE_PHASE && _current_phase != Phase::REACT_COMB_PHASE ) )
	{
		wxMessageBox( wxT("Non phasing units are not allowed to move. You\ncan change this by selecting the appropriate\nrule (Options->Rules->Movement).") );
		return 0;
	}

	// land units can't in general move in combat phase, but non-phasing
	// unit can (reaction...)
#if 0
	if ( ! np && Phase == COMBAT_PHASE )
		return 0;
	if ( Phase == COMBAT_PHASE )
	{
		if (wxMessageBox("Combat phase: movement is covered by special rules\n"
	                 "that are not handled by HexTools. If you move you should\n"
	                 "adjust MPs manually.\n\n"
	                 "Really move?", Application::NAME, wxYES_NO|wxICON_QUESTION ) != wxYES )
			return 0;
	}
#endif
	// first:if target hex is enemy occupied then use tactical movement
	// because then player will be prompted if he wishes to attack
	int nx, ny;
	ht::getAdjHexCoords( dir, selected_hex_point_.x, selected_hex_point_.y, &nx, &ny, world_x_size_, world_y_size_ );
	if ( IsEnemyOccupied( nx, ny ) )
		return 1;

	// are units perhaps in train?
	int intrain = 0;
	int onfoot = 0;
	int inship = 0;

	while ( cntr )
	{
		if ( cntr->getTrain( ) || ( (GroundUnit*)cntr )->getFlag( GroundUnit::RAILONLY ) )
			++intrain;
		else if ( cntr->getShip( ) )
			++inship;
		else
			++onfoot;
		cntr = cntr->getNext( );
	}

	if ( ( intrain && onfoot ) || ( intrain && inship ) || ( inship && onfoot ) )
	{
		MoveErr( "All units must use same transportation method" );
		return 0;
	}

	if ( move_mode_ == MovementMode::STRATEGIC && _current_phase != Phase::EXPLOITATION_PHASE )
	{
		char str[255];
		cntr = _selected_unit_list;
		while ( cntr )
		{
			if ( cntr->getTacticalMove( ) )
			{
				cntr->getUnitString( str );
				sprintf( msg, "Following unit(s) has used tactical movement\nand thus is unable to use strategic movement:\n\n%s", str );
				MoveErr( msg );
				return 0;
			}
			if ( cntr->getAdminMove( ) )
			{
				cntr->getUnitString( str );
				sprintf( msg, "Following unit(s) has used administrative movement\nand thus is unable to use strategic movement:\n\n%s", str );
				MoveErr( msg );
				return 0;
			}
			cntr = cntr->getNext( );
		}
		return 3;
	}

	if ( _current_phase == Phase::EXPLOITATION_PHASE )
	{
		cntr = _selected_unit_list;
		while ( cntr )
		{
			if ( cntr->getStratMove( ) )
			{
				wxMessageBox( wxT("Unit that have used strategic movement may not\nmove in the exploitation phase"), Application::NAME, wxOK );
				return 0;
			}
			cntr = cntr->getNext( );
		}
	}

	if ( intrain )
		return 4;

	if ( inship )
		return 5;

	if ( move_mode_ == MovementMode::ADMIN && _current_phase != Phase::EXPLOITATION_PHASE )
	{
		char str[255];
		cntr = _selected_unit_list;
		while ( cntr )
		{
			if ( cntr->getTacticalMove( ) )
			{
				cntr->getUnitString( str );
				sprintf( msg, "Following unit(s) has used tactical movement\nand thus is unable to use administrative movement:\n\n%s", str );
				wxMessageBox( wxS( msg ) );
				return 0;
			}
			if ( cntr->getStratMove( ) )
			{
				cntr->getUnitString( str );
				sprintf( msg, "Following unit(s) has used strategic movement\nand thus is unable to use administrative movement:\n\n%s", str );
				wxMessageBox( wxS( msg ) );
				return 0;
			}
			cntr = cntr->getNext( );
		}
		return 2;
	}

	// tactical movement wanted, check if it's possible
	char str[255];
	cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( cntr->getAdminMove( ) )
		{
			cntr->getUnitString( str );
			sprintf( msg, "Following unit(s) has used administrative movement\nand thus is unable to use tactical movement:\n\n%s", str );
			wxMessageBox( wxS( msg ) );
			return 0;
		}
		if ( cntr->getStratMove( ) )
		{
			cntr->getUnitString( str );
			sprintf( msg, "Following unit(s) has used strategic movement\nand thus is unable to use tactical movement:\n\n%s", str );
			wxMessageBox( wxS( msg ) );
			return 0;
		}
		cntr = cntr->getNext( );
	}
	return 1;
}

// administrative movement
void MapPane::AdminMove( int direction )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// TODO: shouldn't we check for false (new coords out of bounds) here??
	int new_x, new_y = 0;
	ht::getAdjHexCoords( direction, x, y, &new_x, &new_y, world_x_size_, world_y_size_ );

	Counter* unit = _selected_unit_list;
	if ( ! unit || ! IsOwnUnit( unit ) )
		return;

	bool road = IsRoad( x, y, direction )
			 || IsRailRoad( x, y, direction )
			 || IsLVRailRoad( x, y, direction );

	// can't go to non-clear hex (except by road)
	if ( ! road )
	{
		// FitE/SE: only clear terrain allowed
		if (	_rule_set.OnlyClearAdmin == TRUE
			 || hex_[new_y][new_x].getWeatherZone( ) == WeatherZone::F
			 || hex_[new_y][new_x].getWeatherZone( ) == WeatherZone::G )
		{
			if ( hex_[new_y][new_x].terrain_ != HexType::CLEAR )  // no road, only clear ok
			{
				const char* q;
				if ( _rule_set.OnlyClearAdmin == TRUE )
					q = "Administrative movement: Only clear terrain is allowed!\n(FitE style, see 'Options->Rules->Movement').\n\nStill move?";
				else
					q = "Administrative movement: Only clear terrain is allowed!\n(Weather zones F and G).\n\nStill move?";
				if ( wxMessageBox( wxS( q ), Application::NAME, wxYES_NO ) != wxYES )
					return;
			}
			else if ( IsNonClearHexSide( x, y, direction ) )  // also, no non-clear hexsides
			{
				const char* q;
				if ( _rule_set.OnlyClearAdmin == TRUE )
					q = "Administrative movement: Can't cross non-clear hexsides\n(FitE style, see 'Options->Rules->Movement'). Still move?";
				else
					q = "Administrative movement: Can't cross non-clear hexsides\n(Weather zones F and G).Still move?";
				if ( wxMessageBox( wxS( q ), Application::NAME, wxYES_NO ) != wxYES )
					return;
			}
		}
		else // SF+: can go anywhere but mountains
		{
			// TODO: make sure there is no weather zone logic here
			// w/o road, can't go to mountain
			if ( hex_[new_y][new_x].terrain_ == HexType::MOUNTAIN )
			{
				MoveErr( "Administrative movement: Can't move to mountain hex" );
				return;
			}
			else if ( IsMountain( x, y, direction ) )
			{
				MoveErr( "Administrative movement: Can't cross mountain hexside" );
				return;
			}
			else if ( IsMajorRiver( x, y, direction ) )
			{
				MoveErr( "Administrative movement: Can't cross major river" );
				return;
			}
			else if ( IsProhibited( x, y, direction ) )
			{
				MoveErr( "Administrative movement: prohibited hexside" );
				return;
			}
		}
	}

	// can't start from enemy adjacent hex
	if ( IsAdjEnemies( x, y ) )
	{
		MoveErr( "Administrative movement: can't start from enemy adjacent hex" );
		return;
	}

	// can't enter enemy owned hex
	if ( sit_[new_y][new_x].getOwner( ) != _current_player )
	{
		MoveErr( "Administrative movement: can't enter enemy owned hex" );
		return;
	}

	// can't enter hex that was enemy owned at the beginning of the turn
	if ( sit_[new_y][new_x].getOrigOwner( ) != _current_player )
	{
		MoveErr( "Administrative movement: can't enter hex that\nwas enemy owned at the beginning of the turn." );
		return;
	}

	// can't move adjacent to enemy units
	if ( IsAdjEnemies( new_x, new_y ) )
	{
		MoveErr( "Administrative movement: can't move adjacent to enemy units" );
		return;
	}

	// ok to admin move, calculate MPs...
	int mp_harassment = sit_[y][x].getHarassment( );  // 0..2
	int new_mps[32];
	int new_halfmp[32];
	int i = 0;
	unit = _selected_unit_list;
	while ( unit )
	{
		if ( unit->getMission( ) )
		{
			char str[255];
			unit->getUnitString( str );
			sprintf( msg, "Cancel construction work?\n%s", str );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_INFORMATION ) != wxYES )
				return;
			unit->setMission( 0 );
		}

		if (	unit->getCounterType( ) == Counter::Type::UNIT &&  // sanity check
				( ( (GroundUnit*)unit )->getSupply( ) == SupplyStatus::NONE ) &&
				_rule_set.AllowUxAdminMove == FALSE )
		{
			char s[100];
			unit->getUnitString( s );
			sprintf( msg, "The following unit is out of supply, still use administrative movement?\n%s", s );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO ) != wxYES )
				return;
		}

		// add harassment, too; it's added in CalcMP, so it's doubled
		// that's fine because it's halved below. 2*=1
		int mps = calculateHexCost( unit, x, y, direction, 0 ) + mp_harassment;  // admin: half cost
#if 0
		sprintf( msg, "before: mps=%d, rem=%d,  %d/2 req", unit->GetMP(), unit->IsHalfMP(), mps );
		wxMessageBox( msg, "bdg" );
#endif
		new_mps[i] = unit->getMP( ) - mps / 2;

		if ( mps % 2 )  // remainder of 1/2 MP
		{
			if ( unit->isHalfMP( ) )
				new_halfmp[i] = 0;  // just remove the existing 1/2 MP
			else
			{
				--new_mps[i];  // knock down by full 1 MP
				new_halfmp[i] = 1;  // then add back a 1/2 MP
			}
		}
		else
			// no 1/2 MP remainder, keep whatever 1/2 MP the unit already has
			new_halfmp[i] = unit->isHalfMP( );

		if ( new_mps[i] < 0 )  // movement failed
		{
			//FlashWindow(-1);  // TODO
			return;
		}
#if 0
		sprintf( msg, "after: mps=%d, rem=%d", new_mps[i], new_halfmp[i] );
		wxMessageBox( msg, "bdg" );
#endif
		unit = unit->getNext( );
		++i;
	}

	// all units were able to move, now reduce their MPs
	i = 0;
	unit = _selected_unit_list;
	while ( unit )
	{
		unit->recordReplay( dir2replay( direction ) );
		unit->setMP( new_mps[i] );
		unit->setHalfMP( new_halfmp[i] );
		unit->setAdminMove( 1 );
		unit = unit->getNext( );
		++i;
	}

	contents_pane_->clear( );
	contents_pane_->addUnits( _selected_unit_list );
	contents_pane_->markAllSelected( );  // select selected units
	contents_pane_->addUnits( sit_[new_y][new_x].unit_list_ );  // non-selected mode
	contents_pane_->Refresh( false );

	UnHiLiteHex( );
	int old_x = selected_hex_point_.x;
	int old_y = selected_hex_point_.y;
	selected_hex_point_.x = new_x;
	selected_hex_point_.y = new_y;
	updateHexCache( old_x, old_y );
	updateHexCache( new_x, new_y );
	UnHiLiteHex( );
	HiLiteHex( selected_hex_point_.x, selected_hex_point_.y );

	if ( click_marching_mode_ == TRUE )
		wxMilliSleep( MOVE_DELAY );  // milliseconds
	else
		showSelUnitMovementArea( true );

	file_is_dirty_ = true;
}

void MapPane::StrategicMove( int direction )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	int new_x, new_y;
	ht::getAdjHexCoords( direction, x, y, &new_x, &new_y, world_x_size_, world_y_size_ );
	if ( ! IsRailRoad( x, y, direction ) )
	{
		MoveErr( "Strategic movement: no railroad" );
		return;
	}

	if ( sit_[new_y][new_x].getGauge( ) != _current_player )
	{
		if ( wxMessageBox( wxT("Strategic movement: wrong rail gauge, move anyway?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
			return;
	}

	if ( sit_[new_y][new_x].getRRHit( ) ) // rail is broken
	{
		MoveErr( "Strategic movement: railroad is broken" );
		return;
	}

	if ( sit_[new_y][new_x].isRailObstructed( ) ) // rail is obstructed
	{
		MoveErr( "Strategic movement: railroad is obstructed" );
		return;
	}

	if ( IsBridgeBreak( x, y, direction ) ) // bridge is broken
	{
		MoveErr( "Strategic movement: bridge is broken" );
		return;
	}

	if ( sit_[new_y][new_x].getOwner( ) != _current_player || IsEnemyOccupied( new_x, new_y ) )
	{
		MoveErr( "Strategic movement: can't enter enemy hex" );
		return;
	}

	// can't enter enemy zoc
	// (TODO: can enter if occupied by own unit that doesn't move at all)
	if ( isHexInEnemyZOC( new_x, new_y ) && ! IsOwnOccupied( new_x, new_y ) )
	{
		MoveErr( "Strategic movement: can't enter enemy ZOC" );
		return;
	}

	char str[255];

	// TODO: check for enemy zoc in dest
	// are all units able to move?
	int mp_req = 1 + ( sit_[y][x].getHarassment( ) * 20 );

	Counter* cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( cntr->getMission( ) )
		{
			cntr->getUnitString( str );
			sprintf( msg, "Cancel construction work?\n%s", str );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_INFORMATION ) != wxYES )
				return;
			cntr->setMission( 0 );
		}
		if ( cntr->getMP( ) < mp_req )
		{
			//FlashWindow(1);  TODO
			return;
		}
		cntr = cntr->getNext( );
	}

	// if unit is starting to move, set its MPs to 200 (200 rail hexes)
	int hx, hy;
	cntr = _selected_unit_list;
	while ( cntr )
	{
		cntr->getHomeHex( &hx, &hy );
		if ( x == hx && y == hy )
		{
			cntr->setMP( 200 );
			cntr->clearReplay( );
		}
		cntr->recordReplay( dir2replay( direction ) );
		cntr->setMP( cntr->getMP( ) - mp_req );  // reduce MPs
		// rail usage:
		if ( ! cntr->getStratMove( ) )
		{
			// calculate REs
			if ( cntr->getCounterType( ) != Counter::Type::UNIT )  // bugcheck
				wxMessageBox( wxT("bug #1143") );
			else
			{  // must be UNIT
				float re = ( (GroundUnit*)cntr )->getRE( );
				if ( ( (GroundUnit*)cntr )->isSupplementalMotorized( ) ||  // c/m and cav are double
					( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::CAV || ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::HVCAV )
					re *= 2;
				// TODO: implement the new LV RR treatment:
				// low-vol rr: double it:
				if ( IsLVRailRoad( x, y, direction ) )
					re *= 2;
				turn_REs_rail_used_ += re;
			}
			cntr->setStratMove( true );
		}
		cntr = cntr->getNext( );
	}
	contents_pane_->clear( );
	contents_pane_->addUnits( _selected_unit_list );
	contents_pane_->markAllSelected( );  // select selected units
	contents_pane_->addUnits( sit_[new_y][new_x].unit_list_ );  // non-selected mode
	contents_pane_->Refresh( false );
	UnHiLiteHex( );
	int old_x = selected_hex_point_.x;
	int old_y = selected_hex_point_.y;
	selected_hex_point_.x = new_x;
	selected_hex_point_.y = new_y;
	updateHexCache( old_x, old_y );
	updateHexCache( new_x, new_y );
	UnHiLiteHex( );
	HiLiteHex( selected_hex_point_.x, selected_hex_point_.y );

	if ( click_marching_mode_ == TRUE )
		wxMilliSleep( MOVE_DELAY );  // milliseconds
	else
		showSelUnitMovementArea( true );

	file_is_dirty_ = true;
}

// units are in train (same way in tactical and admin move)
void MapPane::TrainMove( int direction )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	int new_x, new_y;
	ht::getAdjHexCoords( direction, x, y, &new_x, &new_y, world_x_size_, world_y_size_ );

	if ( ! IsRailRoad( x, y, direction ) ) // no railroad, can't go there
	{
		MoveErr( "Train movement: no railroad" );
		return;
	}

	if ( _rule_set.NoGaugeChk == FALSE && sit_[new_y][new_x].getGauge( ) != _current_player )
	{
		if ( wxMessageBox( wxT("Train movement: wrong rail gauge, move anyway?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
			return;
	}

	if ( sit_[new_y][new_x].getRRHit( ) ) // rail is broken
	{
		MoveErr( "Train movement: railroad is broken" );
		return;
	}
	if ( sit_[new_y][new_x].isRailObstructed( ) ) // rail is obstructed
	{
		MoveErr( "Train movement: railroad is obstructed" );
		return;
	}
	if ( IsBridgeBreak( x, y, direction ) ) // bridge is broken
	{
		MoveErr( "Train movement: bridge is broken" );
		return;
	}
	if ( sit_[new_y][new_x].getOwner( ) != _current_player || IsEnemyOccupied( new_x, new_y ) )
	{
		MoveErr( "Train movement: can't move to enemy owned hex" );
		return;
	}
	if ( sit_[new_y][new_x].getOrigOwner( ) != _current_player )
	{
		MoveErr( "Train movement: can't move to hex that was enemy\nowned at the beginning of the turn." );
		return;
	}
	// can't enter enemy zoc (TODO: can enter if occupied by own unit that
	// doesn't move at all)
	if ( isHexInEnemyZOC( new_x, new_y ) && ! IsOwnOccupied( new_x, new_y ) )
	{
		MoveErr( "Train movement: can't enter enemy ZOC\n(unless the hex is occupied by a friendly\n)unit that does not move at all)." );
		return;
	}

	int har = sit_[y][x].getHarassment( );  // 0..2

	// check ZOCs (it's occupied by own units)
	int zoc = 0;
	if ( isHexInEnemyZOC( x, y ) )
	{
		// see if there are any unit's that haven't moved:
		Counter* c;
		int ok = 0;
		ENUM_HEXUNITS(x, y, c)
		{
			if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->getMP( ) == ( (GroundUnit*)c )->getMaxMP( ) )
				++ok;
		}

		if ( ! ok )
		{
			if ( wxMessageBox( wxT("Target hex is in the enemy ZOC,\n"
					"it should be occupied by own unit that does not\n"
					"move at all or entering the hex with operative\n"
					"rail movement is prohibited.\n\n"
					"Still move?"), Application::NAME, wxYES_NO ) != wxYES )
				return;
		}
		//if (IsEnemyZOC(new_x, new_y, 0))
		//    zoc = 3;  // cost to move from zoc to zoc
		//else
		//    zoc = 2;  // cost to leave zoc
	}
	else
		zoc = 0;
	har += zoc;  // let's kludge!

	int lvrr = 0;

#if 0
	// TODO: DLT Variant A low vol RR is double movement
	if ( ( RuleSet.rules_variant_ == ERV_A ) && ( IsLVRailRoad( x, y, direction ) ) )
		lvrr = 1;
#endif

	// are all units able to move?
	char str[255];

	Counter* cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( cntr->getMission( ) )
		{
			cntr->getUnitString( str );
			sprintf( msg, "Cancel construction work?\n%s", str );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_INFORMATION ) != wxYES )
				return;
			cntr->setMission( 0 );
		}
		if ( har && cntr->getMP( ) < har )
		{
			sprintf( msg, "Not enough MPs (%d required, %d left)", har, cntr->getMP( ) );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK );
			return;
		}
#if 0
		sprintf( msg, "(%d (%d) required, %d left) %d rrem", har+1, lvrr, cntr->GetMP( ), cntr->GetRailRem( ) );
		wxMessageBox( wxS(msg), Application::NAME, wxOK );
#endif
		if ( cntr->getRailRem( ) == 0 && cntr->getMP( ) == har )
		{
			sprintf( msg, "Not enough MPs (%d required, %d left)", har + 1, cntr->getMP( ) );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK );
			return;
		}
		cntr = cntr->getNext( );
	}

	cntr = _selected_unit_list;
	while ( cntr )
	{
#if 0
		cntr->SetTactMove( 1 );
#endif
		cntr->recordReplay( dir2replay( direction ) );
		cntr->setMP( cntr->getMP( ) - har );  // harassment hits
		if ( cntr->getRailRem( ) == 0 )
		{
			cntr->setRailRem( cntr->getRailHexes( ) - 1 );
			cntr->setMP( cntr->getMP( ) - 1 );
		}
		else
			cntr->setRailRem( cntr->getRailRem( ) - 1 );

		// Variant A low vol RR is double movement
		if ( lvrr )
		{
			if ( cntr->getRailRem( ) == 0 )
			{
				cntr->setRailRem( cntr->getRailHexes( ) - 1 );
				cntr->setMP( cntr->getMP( ) - 1 );
			}
		}
		else
			cntr->setRailRem( cntr->getRailRem( ) - 1 );
		// TODO: DLT (is that an inner else or outer else?)

		cntr = cntr->getNext( );
	}
	contents_pane_->clear( );
	contents_pane_->addUnits( _selected_unit_list );
	contents_pane_->markAllSelected( );  // select selected units
	contents_pane_->addUnits( sit_[new_y][new_x].unit_list_ );  // non-selected mode
	contents_pane_->Refresh( false );
	UnHiLiteHex( );
	int old_x = selected_hex_point_.x;
	int old_y = selected_hex_point_.y;
	selected_hex_point_.x = new_x;
	selected_hex_point_.y = new_y;
	updateHexCache( old_x, old_y );
	updateHexCache( new_x, new_y );
	UnHiLiteHex( );
	HiLiteHex( selected_hex_point_.x, selected_hex_point_.y );

	if ( click_marching_mode_ == TRUE )
		wxMilliSleep( MOVE_DELAY );  // milliseconds
	else
		showSelUnitMovementArea( true );

	file_is_dirty_ = true;
}

void MapPane::ShipMove( int direction )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	int new_x, new_y;
	ht::getAdjHexCoords( direction, x, y, &new_x, &new_y, world_x_size_, world_y_size_ );

	// TODO: IsPort(..) returns city not ports
	if (	hex_[new_y][new_x].terrain_ != HexType::SEA
			&& ! IsCoastalHex( new_x, new_y )
			&& ! IsRivers( new_x, new_y )
			&& ! IsPort( new_x, new_y )
			&& hex_[new_y][new_x].terrain_ != HexType::ATOLL
			&& hex_[new_y][new_x].terrain_ != HexType::SMALLISLAND
			&& hex_[new_y][new_x].terrain_ != HexType::RESTRICTEDWATERS
			&& hex_[new_y][new_x].terrain_ != HexType::ICINGSEA )
	{
		if ( wxMessageBox( wxT("There's seems to be no water for ships to sail!\n\nStill move"), Application::NAME, wxYES_NO ) != wxYES )
			return;
	}

	// Variant A River Flotilla movement
	Counter* cntr = _selected_unit_list;
	if (	cntr->getCounterType( ) == Counter::Type::SHIP &&
			static_cast<NavalUnit*>( cntr )->getShipType( ) == NavalUnitType::Fleet::RF &&
			hex_[new_y][new_x].terrain_ == HexType::SEA &&
			! IsCoastalHex( new_x, new_y ) )
	{
		if ( wxMessageBox( wxT("River flotillas are not allowed to move in full sea hexes\n\nStill move"), Application::NAME, wxYES_NO ) != wxYES )
			return;
	}

	int side = cntr->getSide( );

	int mp_req = ( IsCoastalHex( new_x, new_y ) ? 2 : 1 );

	// enough MPs? (old system: unlimited MPs)
	if ( _rule_set.OldNavalSystem == FALSE ) // TODO: DLT || RuleSet.RulesVariant == ERV_A )
	{
		ENUM_SELUNITS(cntr)
			if ( cntr->getCounterType( ) == Counter::Type::SHIP && cntr->getMP( ) < mp_req )
			{
				MoveErr( "Ship movement: not enough MPs" );
				return;
			}
		// MPs left, now move
		ENUM_SELUNITS(cntr)
			if ( cntr->getCounterType( ) == Counter::Type::SHIP )
				cntr->setMP( cntr->getMP( ) - mp_req );  // reduce MPs
	}
	sit_[y][x].setOwner( side );
	contents_pane_->clear( );
	contents_pane_->addUnits( _selected_unit_list );
	contents_pane_->markAllSelected( );  // select selected units
	contents_pane_->addUnits( sit_[new_y][new_x].unit_list_ );  // non-selected mode
	contents_pane_->Refresh( false );
	UnHiLiteHex( );
	int old_x = selected_hex_point_.x;
	int old_y = selected_hex_point_.y;
	selected_hex_point_.x = new_x;
	selected_hex_point_.y = new_y;
	updateHexCache( old_x, old_y );
	updateHexCache( new_x, new_y );
	UnHiLiteHex( );
	HiLiteHex( selected_hex_point_.x, selected_hex_point_.y );

	if ( click_marching_mode_ == TRUE )
		wxMilliSleep( MOVE_DELAY );  // in ms
	else
		showSelUnitMovementArea( true );

	file_is_dirty_ = true;
}

// if doing overrun: if moving ZOC-to-ZOC but only ZOC source is
//	the destination hex, then negate ZOC MP cost
// 2.2.3: fixed to handle reduced ZOCs correctly
int MapPane::ReduceOverrunZOC( Counter* cp, int x, int y, int dir )
{
	if ( cp->getCounterType( ) != Counter::Type::UNIT )
		return 99;

	int new_x, new_y;
	if ( ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ ) == 0 )
		return 0;

	// target hex not occupied by enemy
	if ( ! IsEnemyOccupied( new_x, new_y ) )
		return 0;

	// TODO: vet this ILO below conditional out-commented:
	// source hex not in enemy R/ZOC other than (perhaps) from target dir/hex
	if (	! isHexInEnemyZOC( x, y, dir )
		 && ! isHexInEnemyRZOC( x, y, dir ) )
		return 0;
#if 0
	// are we moving from ZOC to ZOC?
	// TODO: DLT (try to simplify logic)
	if ( ( isHexInEnemyZOC( x, y ) && isHexInEnemyZOC( new_x, new_y ) )
		|| ( isHexInEnemyZOC( x, y ) && IsZOCSource( new_x, new_y ) ) )
		;	// ZOC = 1;	// TODO: suspicious semicolon
	else if ( ( isHexInEnemyRZOC( x, y ) && isHexInEnemyRZOC( new_x, new_y ) )
		|| ( isHexInEnemyRZOC( x, y ) && IsReducedZOCSource( new_x, new_y ) ) )
		;	// rZOC = 1;	// TODO: suspicious semicolon
	else
		return 0;  // target doesn't have ZOC-capable units -> no compensation
#endif
	GroundUnit* unit = static_cast<GroundUnit*>( cp );

	// Modify ZOC MP reduction for overrun
	return calculateZOCCost( unit, x, y, new_x, new_y, 0 ) - calculateZOCCost( unit, x, y, new_x, new_y, dir );

#if 0
	 //int d, x2, y2, zoc = 0, rzoc = 0, mp_return;
	 //int leave_zoc = 0, dir1, dir2, rzoc2rzoc = 0;

	 // in addition, are we leaving ZOC of some other unit(s)
	 if (dir == 1)         // previous dir
	 dir1 = LAST_DIR;
	 else
	 dir1 = dir / 2;
	 if (dir == LAST_DIR)	// next dir
	 dir2 = 1;
	 else
	 dir2 = dir * 2;

	 ENUM_DIRS(d)
	 {
	 if (d == dir) // ignore target units now
	 continue;
	 // if other units cast zoc to both current and target hex -> no mp comp.
	 if (MapPane::getAdjHex(d, x, y, &x2, &y2, iXWorldSize, iYWorldSize))
	 {
	 if (IsZOCSource(x2, y2) && IsEnemyOccupied(x2, y2))
	 {
	 if (d == dir1 || d == dir2)
	 return 0;	// other units cause zoc2zoc, no compensation
	 else
	 ++leave_zoc;
	 }
	 // does somebody cast reduced zoc to both source and target hexes?
	 if (IsReducedZOCSource(x2, y2) && IsEnemyOccupied(x2, y2))
	 {
	 if (d == dir1 || d == dir2)
	 ++rzoc2rzoc;	// other units cause zoc2zoc, no compensation
	 }
	 }
	 }

	 // not even leaving zoc, only zoc came from units to be overrun, thus
	 // compensate any mp cost caused by zoc movement
	 if (leave_zoc == 0) {
	 if (Player == AXIS
	 && unit->IsGerman()
	 && cp->IsCm())
	 {
	 if (RuleSet.ReducedZOCs)
	 mp_return = 1;  // German C/M units pay less -> reduce less
	 else
	 {
	 MapPane::getAdjHex(dir, x, y, &x2, &y2, iXWorldSize, iYWorldSize);
	 if (IsZOCSource(new_x, new_y))
	 mp_return = 2;  // German C/M units pay less -> reduce less
	 else
	 mp_return = 1;
	 }
	 }
	 else
	 {
	 if (RuleSet.ReducedZOCs)
	 mp_return = 1;  // normal cost (reduced ZOC's)
	 else
	 {
	 MapPane::getAdjHex(dir, x, y, &x2, &y2, iXWorldSize, iYWorldSize);
	 if (IsZOCSource(new_x, new_y))
	 mp_return = 3;  // normal zoc2zoc cost
	 else  // just reduced zoc source
	 mp_return = 1;
	 }
	 }
	 // if we are moving from reduced zoc to reduced zoc we must still
	 // charge MPs for that
	 if (rzoc2rzoc)
	 --mp_return;
	 return mp_return;
	 }

	 // in addition of units to be overrun, we are leaving zoc of some other
	 // unit(s), thus compensate only 1 MP (difference between zoc2zoc and
	 // leave-zoc costs.
	 // 2.2.3: must figure out if we are leaving full or reduced ZOC:
	 if (leave_zoc)     // leaving full zoc
	 {
	 // NOTE: it costs more to leave full ZOC than to move from rzoc to rzoc
	 return 1;
	 }
	 else // leaving reduced zoc
	 {
	 // leaving reduced zoc costs nothing, so compensate all:
	 MapPane::getAdjHex(dir, x, y, &x2, &y2, iXWorldSize, iYWorldSize);
	 if (IsZOCSource(new_x, new_y))
	 return 3;  // cost from zoc to zoc
	 else
	 return 1;  // cost from rzoc to rzoc
	 }
#endif
}

int MapPane::hexsideWeatherZone( int x, int y, int dir )
{
	int zoneThisSide = hex_[y][x].getWeatherZone( );

	int x2, y2;
	ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ );

	int zoneOtherSide = hex_[y2][x2].getWeatherZone( );

	// TODO: correct this for P1 and P2 zone exceptions (north of A)
	return zoneThisSide <= zoneOtherSide ? zoneThisSide : zoneOtherSide;
}

int MapPane::calculateHexsideCost( Counter* cp, int x, int y, int dir )
{
	if ( cp->getCounterType( ) != Counter::Type::UNIT )
		return 99;

	GroundUnit* unit = (GroundUnit*)cp;

	// TODO: believe this should simply check ( unit->getType( ) == SKI )
	int ski = unit->getType( ) == GroundUnitType::WW2::SKI; // ( _ground_unit_type[unit->getType( )].type_ == SKI );  // ski unit?
	// TODO: believe this should also check unit->isCombatMotClass( )
	//int mot = unit->isSupplementalMotorized( );
	int mot = ( unit->isCombatMotClass( ) || unit->isSupplementalMotorized( ) );
	int art = unit->isArtillery( );

	int ice = _rivers_lakes_frozen[hex_[y][x].getWeatherZone( )];
	//int w = Weather[Hex[y][x].GetWeatherZone( )]; // TODO: weather!!!

	int mps = 0;

	if ( IsMinorCanal( x, y, dir ) )
		mps += ( ice ? minor_canal_frozen_MP_cost_ : minor_canal_MP_cost_ );
	else if ( IsCanal( x, y, dir ) || IsMinorRiver( x, y, dir ) )
		mps += ( ice ? minor_river_frozen_MP_cost_ : minor_river_MP_cost_ );
	else if ( IsMajorRiver( x, y, dir ) )
		mps += ( ice ? ( ski ? minor_river_frozen_MP_cost_ : major_river_frozen_MP_cost_ ) : major_river_MP_cost_ );

	//if ( IsMountain( x, y, dir ) ) mps += 3; // TODO: removed to calling method

	if ( IsHighMtn( x, y, dir ) )
	{
		if ( _weather_condition[hexsideWeatherZone( x, y, dir )] == WeatherCondition::CLEAR )
		{
			if ( unit->getType( ) == GroundUnitType::WW2::HIGH_MOUNTAIN )
				mps += 4;
			else
				mps = 99;
		}
		else // other-than-Clear weather
		{
			mps = 99;
		}
	}

	if ( IsHighMtnPass( x, y, dir ) || IsKarst( x, y, dir ) )
	{
		if ( _weather_condition[hexsideWeatherZone( x, y, dir )] == WeatherCondition::CLEAR )
		{
			if ( mot || art )
				mps = 99;
			else
				mps += 4;
		}
		else // other-than-Clear weather
		{
			if ( unit->getType( ) == GroundUnitType::WW2::HIGH_MOUNTAIN )
				mps += 4;
			else
				mps = 99;
		}
	}

	if ( IsNarrowStrait( x, y, dir ) )
		mps += ice ? ( ski ? 0 : 1 ) : 2;
	else if ( !ice && IsWater( x, y, dir ) )
		mps = 199;
	else if ( ice && IsLakeSide( x, y, dir ) )
	{	// frozen lake hexside, ignored in SE
		if ( _rule_set.IgnoreFrozenLakeHexsides == FALSE )
			mps += ( mot || art ) ? 3 : ( ski ? 1 : 2 );
	}
#if 0
	else if ( !RuleSet.IgnoreFrozenLakeHexsides && ice && IsLakeSide( x, y, dir ) && ( mot || art ) )
		mps += 3;  // ice+mot=+3
	else if ( !RuleSet.IgnoreFrozenLakeHexsides && ice && IsLakeSide( x, y, dir ) && ski )
		mps += 1;  // ice+ski=+1
	else if ( !RuleSet.IgnoreFrozenLakeHexsides && ice && IsLakeSide( x, y, dir ) )
		mps += 2;  // ice+oth=+2
	else if ( RuleSet.IgnoreFrozenLakeHexsides && ice && IsLakeSide( x, y, dir ) )
		mps += 0;  //
#endif
	else if ( ice && ( IsSeaSide( x, y, dir ) || IsRWaterSide( x, y, dir ) ) )
		mps += ( mot || art ) ? 99 : ( ski ? 2 : 3 );
#if 0
	else if ( ice && ( IsSeaSide( x, y, dir ) || IsRWaterSide( x, y, dir ) ) && ski )
		mps += 2;  // ice+ski=+2
	else if ( ice && ( IsSeaSide( x, y, dir ) || IsRWaterSide( x, y, dir ) ) )
		mps += 3;  // ice+oth=+3
#endif
	if ( IsSaltDesert( x, y, dir ) )
		mps = 299;

	if ( IsGlacier( x, y, dir ) )
		mps = 399;

	mps += ( ( IsWadi( x, y, dir ) || IsEscarpment( x, y, dir ) ) ? 1 : 0 );

	if ( IsImpEscarpment( x, y, dir ) )
	{
		if ( use_WW1_hexsides_ && unit->isMtn( ) )
			mps += 4;
		else
			mps = 499;
	}

	// when only clear hexes allowed in admin move:
	// if there is any penalty on the hexside, i.e. it's not clear, then disallow
	if ( _rule_set.OnlyClearAdmin == TRUE && move_mode_ == MovementMode::ADMIN && mps > 0 )
		mps = 69;

	return mps;
}

// Modify ZOC MP reduction for overrun
int MapPane::calculateZOCCost( Counter* cp, int x, int y, int new_x, int new_y, int xdir )
{
	// Variant A River Flotilla movement
	if ( cp->getCounterType( ) == Counter::Type::SHIP )
	{
		if ( ( (NavalUnit*)cp )->getShipType( ) != NavalUnitType::Fleet::RF )
			return 99;
	}
	else if ( cp->getCounterType( ) != Counter::Type::UNIT )
		return 99;

	int d = 0;

	// check ZOCs
	if ( ! IsPartisanUnit( cp ) && isHexInEnemyZOC( x, y, xdir ) )
	{  // this hex is in enemy zoc
		if ( isHexInEnemyZOC( new_x, new_y ) || isHexInEnemyRZOC( new_x, new_y ) || IsEnemyOccupied( new_x, new_y ) )  // this is for overrun MP calculation
		{
			// we are going from a hex in a FULL ZOC to on that is either a Full or Reduced ZOC or is occupied by an enemy unit
			if ( _rule_set.ReducedZOCs == TRUE )
			{
				// Add Rules option to specify Reduced ZOC MP Costs
				d = _rule_set.RZOC2ZOCCost;  // cost to move from zoc to zoc (reduced ZOC'cs)
			}
			else
			{
				d = _rule_set.ZOC2ZOCCost;  // cost to move from zoc to zoc
			}
		}
		else
		{
			if ( _rule_set.ReducedZOCs == TRUE )
			{
				// Add Rules option to specify Reduced ZOC MP Costs
				d = _rule_set.RZOCLeaveCost;  // cost to leave zoc (reduced ZOC'cs)
			}
			else
			{
				d = _rule_set.ZOCLeaveCost;  // cost to leave zoc
			}
		}

		if ( _rule_set.ReducedZOCs == FALSE && _rule_set.GermanCMZOCRule == TRUE && _current_player == SidePlayer::AXIS && IsCm( cp ) && IsGermanUnit( cp ) )
			--d;  // German c/m units pay less

		if ( d < 0 )
			d = 0;

		return d;
	}
	// check _reduced_ ZOC's ONLY if no normal zoc present
	else if ( isHexInEnemyRZOC( x, y, xdir ) )  // leaving enemy reduced zoc?
	{
		if ( isHexInEnemyZOC( new_x, new_y ) || isHexInEnemyRZOC( new_x, new_y ) || IsEnemyOccupied( new_x, new_y ) )  // this is for overrun MP calculation
		{
			// Add Rules option to specify Reduced ZOC MP Costs
			d = _rule_set.RZOC2ZOCCost;  // cost to move from zoc to zoc (reduced ZOC'cs)
		}
		else
		{
			// Add Rules option to specify Reduced ZOC MP Costs
			d = _rule_set.RZOCLeaveCost;  // cost to leave zoc (reduced ZOC'cs)
		}
		return d;
	}
	// this hex is NOT in enemy zoc
	else if ( ! isHexInEnemyZOC( x, y, xdir ) && ( isHexInEnemyZOC( new_x, new_y ) || isHexInEnemyRZOC( new_x, new_y ) ) )
	{
		// rule for FtF: entering ZOC costs MPs
		if ( IsCm( cp ) )
			return _rule_set.ZOCEnterMot;
		else
			return _rule_set.ZOCEnterNormal;
	}
	return 0;
}

// calculate unit's MPs from hex to direction dir
int MapPane::calculateHexCost( Counter* cp, int x, int y, int dir, int show )
{
	if ( cp->getCounterType( ) != Counter::Type::UNIT )
		return 30;

	int new_x, new_y;
	if ( ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ ) == 0 )
		return 30;

	// no MP calculation:
	if ( _rule_set.DisableMPCalc == TRUE )
		return 0;

	int wx;
	switch ( _weather_condition[hex_[new_y][new_x].getWeatherZone( )] )
	{
		case WeatherCondition::MUD:
			wx = WeatherClass::MUD;
			break;
		case WeatherCondition::WINTER:
		case WeatherCondition::SNOW:
			wx = WeatherClass::WINTER;
			break;
		case WeatherCondition::CLEAR:
		case WeatherCondition::FROST:
		default:
			wx = WeatherClass::CLEAR;
			break;
	}

	// motorized mountain is motorized, NOT mountain
	GroundUnit* unit = static_cast<GroundUnit*>( cp );
	int movement_category;
	if ( unit->isSupplementalMotorized( ) )
		movement_category = MovementCategory::MOT_ART;
	else if ( unit->isMtn( ) )
		movement_category = MovementCategory::MP_MTN;
	else
		movement_category = _ground_unit_type[unit->getType( )].movement_category_.value( );

	int mps;
	// swamps/mtn may have different cost based on rules
	if ( _rule_set.MP6Swamp == FALSE && ( unit->isSupplementalMotorized( ) || unit->isArtillery( ) ) && ( hex_[new_y][new_x].terrain_ == HexType::SWAMP
				|| hex_[new_y][new_x].terrain_ == HexType::FOREST
				|| hex_[new_y][new_x].terrain_ == HexType::MOUNTAIN ) )
	{
		mps = unit->getMaxMP( ) / 2 + 1;
	}
	else
	{
		// Partisans are special: they may use mountain/lt.inf/cavalry
		// movement points if player wants. Here we assume that player
		// always wants to pay as little as possible:
		if ( IsPartisanUnit( cp ) )
		{
			mps = _hex_types[hex_[new_y][new_x].terrain_].mp_[wx][MovementCategory::OTHER];
			mps = min( mps, _hex_types[hex_[new_y][new_x].terrain_].mp_[wx][MovementCategory::MP_CAV] );
			mps = min( mps, _hex_types[hex_[new_y][new_x].terrain_].mp_[wx][MovementCategory::LIGHT] );
			mps = min( mps, _hex_types[hex_[new_y][new_x].terrain_].mp_[wx][MovementCategory::MP_MTN] );
		}
		else
			mps = _hex_types[hex_[new_y][new_x].terrain_].mp_[wx][movement_category];
	}

	if ( show == 1 )
	{
		sprintf( msg, "MPs required (hex types) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}

	// special case for a mtn hexside:  it overrides the hex terrain
	//	on the other side of the indicated hexside
	if ( IsMountain( x, y, dir ) )
	{
		// reset the hex MP cost calculation, doesn't matter what hex terrain
		//	on other side actually is, treat it as mountain
		mps = _hex_types[HexType::MOUNTAIN].mp_[wx][movement_category];

		// this logic is from the end of the calculateHexsideCost(..) function:
		if ( _rule_set.OnlyClearAdmin == TRUE && move_mode_ == MovementMode::ADMIN )
			mps = 69;
	}
	else
	{
		mps += calculateHexsideCost( unit, x, y, dir );  // hexside effects
	}

	if ( show == 1 )
	{
		sprintf( msg, "MPs required (hex side) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}

	// roads ( lv rr = both ) NOTE: road + water = narrow strait = major river
	bool road = false;
	if (	IsBridge( x, y, dir )
		 && ! IsBridgeBreak( x, y, dir ) )
	{
		// 2.2.3: paved roads option:
		if ( _rule_set.PavedRoads == TRUE && hex_[new_y][new_x].terrain_ != HexType::MOUNTAIN )
			mps = 1;  //  even during bad weather
		else
			mps = _hex_types[HexType::CLEAR].mp_[wx][movement_category];  //UnitTypes[unit->GetType()].bMoveType];
		road = true;
	}
	else if (	IsRailRoad( x, y, dir ) 		// broken railroads are always roads
				|| IsLVRailRoad( x, y, dir )	// " "
				|| (	IsRoad( x, y, dir )		// but roads could be broken...
					 && ! IsNarrowStrait( x, y, dir )	// but not across narrow straits
					 && (	_rule_set.RoadsCanBeBroken == FALSE 	// can't be broken by rule setting
						 || ! sit_[new_y][new_x].getRRHit( ) ) ) )	// if can be broken, but are not broken
	{
		// paved roads option:
		if ( _rule_set.PavedRoads == TRUE && hex_[new_y][new_x].terrain_ != HexType::MOUNTAIN )
			mps = 1;  //  even during bad weather
		else
			mps = _hex_types[HexType::CLEAR].mp_[wx][movement_category];  // UnitTypes[unit->GetType()].bMoveType];
		road = true;
	}

	if ( show == 1 )
	{
		sprintf( msg, "MPs required (after check roads) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}

	// trail: reduce 1 mp (but not to zero!)
	if ( IsTrail( x, y, dir ) && mps > 0 )
		--mps;

	// in arctic MPs are doubled (except when on the road)
	// TODO: find (or write) a function to adjudicate "arctic" ( i.e. == A, or >= P1 )
	if (	! road
		 && ( hex_[y][x].getWeatherZone( ) == WeatherZone::A
		   || hex_[y][x].getWeatherZone( ) >= WeatherZone::P1 ) )
		mps *= 2;

	// Modify ZOC MP reduction for overrun
	if ( show == 2 )
	{
		sprintf( msg, "ZOC requires %d MP", calculateZOCCost( unit, x, y, new_x, new_y ) );
		wxMessageBox( wxS( msg ), Application::NAME );
	}
	mps += calculateZOCCost( unit, x, y, new_x, new_y );
	if ( show == 1 )
	{
		sprintf( msg, "MPs required (after check ZOC) = %d ", mps );
		wxMessageBox( wxS( msg ), Application::NAME );
	}
#if 0
	 // check ZOCs
	 if (unit->GetType() != PARTISAN && IsEnemyZOC(x, y))  // this hex is in enemy zoc
	 {
	 if (IsEnemyZOC(new_x, new_y) || IsEnemyReducedZOC(new_x, new_y) ||
	 IsEnemyOccupied(new_x, new_y))
	 { // this is for overrun MP calculation
	 int d;
	 if (RuleSet.ReducedZOCs) {
	 d = 1;  // cost to move from zoc to zoc (reduced ZOC'cs)
	 } else {
	 d = RuleSet.ZOC2ZOCCost; // cost to move from zoc to zoc
	 //d = 3;
	 }
	 } else {
	 if (RuleSet.ReducedZOCs) {
	 d = 0;  // cost to leave zoc (reduced ZOC'cs)
	 } else {
	 d = RuleSet.ZOCLeaveCost; // cost to leave zoc
	 //d = 2;
	 }
	 }
	 if (!RuleSet.ReducedZOCs
	 && RuleSet.GermanCMZOCRule
	 && Player == AXIS
	 && unit->IsGerman()
	 && (cp->IsCm())
	 --d;  // German C/M units pay less
	 if (d < 0)
	 d = 0;
	 mps += d;
	 }
	 // check _reduced_ ZOCs ONLY if no normal ZOC present
	 else if (IsEnemyReducedZOC(x, y))  // leaving enemy reduced ZOC?
	 {
	 if (IsEnemyZOC(new_x, new_y) || IsEnemyReducedZOC(new_x, new_y) ||
	 IsEnemyOccupied(new_x, new_y))  // this is for overrun MP calculation
	 d = 1;  // cost to move from zoc to zoc (reduced ZOCs)
	 else
	 d = 0;  // cost to leave zoc (reduced ZOCs)
	 if (Player == AXIS
	 && unit->IsGerman()
	 && cp->IsCm())
	 --d;  // german C/M units pay less
	 mps += d;
	 }
	 else if (!IsEnemyZOC(x, y)&&  // this hex is NOT in enemy ZOC
	 IsEnemyZOC(new_x, new_y) || IsEnemyReducedZOC(new_x, new_y))
	 {
	 // rule for FtF: entering ZOC costs MPs
	 if (cp->IsCm())
	 mps += RuleSet.ZOCEnterMot;
	 else
	 mps += RuleSet.ZOCEnterNormal;
	 }
#endif

	// harassment hits:
	mps += sit_[y][x].getHarassment( );  // 0..2

	return mps;
}

#endif
