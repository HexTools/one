#if defined HEXTOOLSPLAY

#include "hextools.h"
using ht::wxS;

#include "application.h"
#include "hexunits.loop"
#include "sideplayer.h"
#include "weathercondition.h"
#include "phase.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "rules.h"

#include "mappane.h"
#include "counter.h"
#include "groundunit.h"
#include "airunittype.h"
#include "airunit.h"
#include "facility.h"
#include "hexcontents.h"
#include "hexcontentspane.h"

extern char msg[];		// 10000
extern int _phasing_player;
extern int _current_player;
extern int _current_phase;
extern int _show_combat_markers;

extern int _weather_condition[];	// W_ZONES	// W_MUD etc., per zone (frame.cpp)

extern Rules _rule_set;

// is it ok to end phase 'Phase' (global variable)
bool MapPane::okToEndThisPhase( void )
{
	int x, y;
	int awarn = 0;
	int gwarn = 0;
	Counter* cntr;
	AirUnit* au;
	char s[100], s2[100];

	contents_pane_->deselectAll( );
	contents_pane_->Refresh( );
	if ( _current_phase == Phase::MOVEMENT_PHASE )
	{
		if ( _show_combat_markers )
		{
			wxMessageBox( wxT("NOTE: your combat markers are still visible, you can hide\n"
					"them by using 'View->Show Combat Markers' command"), Application::NAME, wxOK | wxICON_INFORMATION );
		}
		for ( x = 0; x < world_x_size_; ++x )
		{
			for ( y = 0; y < world_y_size_; ++y )
			{
				cntr = sit_[y][x].unit_list_;
				while ( cntr )
				{
					if ( cntr->getCounterType( ) == Counter::Type::PLANE )
					{  // planes must have mission
						au = (AirUnit*)cntr;
						if ( au->getInAir( ) && au->getMission( ) == AirUnit::Mission::NONE )
						{
							au->getSymbolString( s );
							hex_[y][x].getHexIdStr( s2 );
							SelectHex( x, y );  // for user's convenience
							sprintf( msg, "%s is flying but has no mission at %s", s, s2 );
							wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION | wxOK );
							return false;
						}
					}
					cntr = cntr->getNext( );
				}  // end while cntr
			}  // end for y
		}  // end for x
	}

	// movement phase
	if ( _current_phase == Phase::COMBAT_PHASE )
	{
		for ( x = 0; x < world_x_size_; ++x )
		{
			for ( y = 0; y < world_y_size_; ++y )
			{
				if ( sit_[y][x].isAirTarget( ) && ! awarn )
				{
					// don't warn if pending DAS missions, they are natural
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) != AirUnit::Mission::BOMB_DAS )
						{
							SelectHex( x, y );  // for user's convenience
							wxMessageBox( wxT("Warning:\n"
									"there are still unresolved air missions that\n"
									"will be canceled when you end the combat phase"), Application::NAME,
									wxICON_INFORMATION | wxOK );
							awarn = TRUE;
							break;
						}
				}

				if ( sit_[y][x].isTarget( ) && ! gwarn )
				{
					SelectHex( x, y );  // for user's convenience
					wxMessageBox( wxT("Warning:\n"
							"there are still unresolved ground combats that\n"
							"will be canceled when you end the combat phase"), Application::NAME, wxICON_INFORMATION | wxOK );
					gwarn = TRUE;
				}
			}
		}
	}

	// expl phase
	if ( _current_phase == Phase::EXPLOITATION_PHASE )
	{
		// check if OWN planes are still flying (DAS may fly, rest must land)
		for ( x = 0; x < world_x_size_; ++x )
		{
			for ( y = 0; y < world_y_size_; ++y )
			{
				cntr = sit_[y][x].unit_list_;
				while ( cntr )
				{
					if ( cntr->getCounterType( ) == Counter::Type::PLANE )
					{  // planes must have mission
						au = (AirUnit*)cntr;
						if ( au->getInAir( ) && au->getSide( ) == _phasing_player && au->getMission( ) != AirUnit::Mission::BOMB_DAS
							&& au->getMission( ) != AirUnit::Mission::BOMB_HARASS && au->getMission( ) != AirUnit::Mission::NAVALPATROL )
						{
							au->getSymbolString( s );
							hex_[y][x].getHexIdStr( s2 );
							SelectHex( x, y );  // for user's convenience
							sprintf( msg, "%s is still flying at %s", s, s2 );
							wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION | wxOK);
							return false;
						}
					}
					cntr = cntr->getNext( );
				}
			}
		}

		// check if ENEMY planes are still flying (NONE may fly)
		for ( x = 0; x < world_x_size_; ++x )
		{
			for ( y = 0; y < world_y_size_; ++y )
			{
				cntr = sit_[y][x].unit_list_;
				while ( cntr )
				{
					if ( cntr->getCounterType( ) == Counter::Type::PLANE )
					{  // planes must have mission
						au = (AirUnit*)cntr;
						if ( au->getInAir( ) && au->getSide( ) != _phasing_player )
						{
							au->getSymbolString( s );
							hex_[y][x].getHexIdStr( s2 );
							SelectHex( x, y );  // for user's convenience
							sprintf( msg, "%s is still flying at %s", s, s2 );
							wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION | wxOK );
							return false;
						}
					}
					cntr = cntr->getNext( );
				}
			}
		}
	}

// reaction phase
	if ( _current_phase == Phase::REACTION_PHASE )
	{
		for ( x = 0; x < world_x_size_; ++x )
			for ( y = 0; y < world_y_size_; ++y )
			{
				cntr = sit_[y][x].unit_list_;
				while ( cntr )
				{
					if ( cntr->getCounterType( ) == Counter::Type::PLANE )
					{  // planes must have mission
						au = (AirUnit*)cntr;
						if ( au->getSide( ) == _current_player && au->getInAir( ) && au->getMission( ) != AirUnit::Mission::BOMB_DAS )
						{
							au->getSymbolString( s );
							SelectHex( x, y );  // for user's convenience
							hex_[y][x].getHexIdStr( s2 );
							sprintf( msg, "%s is still flying at %s", s, s2 );
							wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION | wxOK);
							return false;
						}
					}
					cntr = cntr->getNext( );
				}
			}
	}

	return true;
}

// do any phase-specific things
// NOTE: all units are de-selected when this is called.
void MapPane::initializeThePhase( )
{
	int own, cnt, warned, aug, t;
	Counter* unit;
	AirUnit* au;
	GroundUnit* up;
	char s2[100];

	// any phase:
	for ( int x = 0; x < world_x_size_; ++x )
	{
		for ( int y = 0; y < world_y_size_; ++y )
		{
			Counter* cntr = sit_[y][x].unit_list_;
			cnt = 0;
			warned = 0;
			while ( cntr )
			{
				switch ( cntr->getCounterType( ) )
				{
					case Counter::Type::UNIT:
						cntr->setRailRem( 0 );
						cntr->setHalfMP( FALSE );
						break;
					case Counter::Type::PLANE:
						au = (AirUnit*)cntr;
						au->setFired( FALSE );
						// flying or inactive planes and GT and GHT are not counted:
						if ( 	au->isActive( )
								&& ! au->getInAir( )
								&& au->getType( ) != AirUnitType::GT
								&& au->getType( ) != AirUnitType::GHT )
							++cnt;
						if ( _rule_set.NewAFCapacitySystem == TRUE && hex_[y][x].terrain_ != HexType::NONE &&  // no whining if not in map
							cnt > GetRawAFCapacity( x, y ) && ! warned )
						{
							hex_[y][x].getHexIdStr( s2 );
							if ( s2[0] != '0' )
							{
								SelectHex( x, y );
								sprintf( msg, "Number of active air groups exceed airfield capacity "
												"in hex %s!\nPlease inactivate extra air groups.", s2 );
								wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
								warned = TRUE;
							}
						}
						break;
				}
				cntr = cntr->getNext( );
			}  // end while
		}  // end for int y
	}  // end for int x

	switch ( _current_phase )
	{
		case Phase::INITIAL_PHASE:
			updateAllSupply( );  // automatic at initial phase start
			_show_combat_markers = TRUE;
			strcpy( marker_password_, "" );
			for ( int x = 0; x < world_x_size_; ++x )
			{
				for ( int y = 0; y < world_y_size_; ++y )
				{
					sit_[y][x].setFiredAA( false );
					sit_[y][x].setAFUsage( 0 );
					sit_[y][x].setPortUsage( 0 );
					if ( sit_[y][x].getOwner( ) != _current_player )
					{  // Remove harassment pts from
						sit_[y][x].setHarrasment( 0 );  // enemy hexes.
						updateHitMarkers( x, y );
					}
					if ( sit_[y][x].getOwner( ) == _current_player && sit_[y][x].getCDHits( ) )
					{
						sit_[y][x].setCDHits( 0 );  // Remove CD hits from own hexes.
						updateHitMarkers( x, y );
						contents_pane_->deselectAll( );
						contents_pane_->clear( );
						contents_pane_->addUnits( sit_[y][x].unit_list_ );
						contents_pane_->Refresh( );
						hex_[y][x].getHexIdStr( s2 );
						sprintf( msg, "Hits removed from coastal defenses at %s", s2 );
						wxMessageBox( wxS( msg ) );
					}
					sit_[y][x].setOrigOwner( sit_[y][x].getOwner( ) );  // remember who owner the hex in the beginning of the turn
					Counter* cntr = sit_[y][x].unit_list_;
					while ( cntr )
					{
						own = ( cntr->getSide( ) == _current_player );
						switch ( cntr->getCounterType( ) )
						{
							case Counter::Type::UNIT:
								up = static_cast<GroundUnit*>( cntr );
								up->setAttackSupply( false );  // attack supply is now used:
								up->clearSupplyTurnFlag( );  // new turn, forget old value
								// if unit ended last player turn in regular supply then it's
								// supply turn counter is zeroed.
								//	if (up->GetSupply() == SUPPLY_REG)
								//	    up->SetSupplyTurns(0);
								// if unit ended prev turn out of supply, then increment it's
								// supply counter. otherwise we'll see wrong color in "Ux"
								// marker.
								//	else
								//	    up->UpdateSupplyTurns();
								// is fort construction ready? (at friendly initial phase)
								if ( own && cntr->getMission( ) == GroundUnit::Project::FORT )
								{
									// how many turns needed?
									t = ( hex_[y][x].terrain_ == HexType::CLEAR || hex_[y][x].terrain_ == HexType::ROUGH ? 1 : 2 );
									if ( _weather_condition[hex_[y][x].getWeatherZone( )] != WeatherCondition::CLEAR )
										t *= 2;

									if ( up->getCompletedTurns( ) >= ( t - 1 ) )
									{
										unit = new Facility( Facility::FORT );
										unit->setSide( _current_player );
										sit_[y][x].insertUnit( unit );
										updateHexCache( x, y );
										cntr->setMission( GroundUnit::Project::NONE );
										up->setCompletedTurns( 0 );
										hex_[y][x].getHexIdStr( s2 );
										SelectHex( x, y );
										sprintf( msg, "Fort construction ready at %s", s2 );
										wxMessageBox( wxS( msg ), Application::NAME );
									}
									else
										up->setCompletedTurns( up->getCompletedTurns( ) + 1 );
								}
								// is AF construction ready? (at friendly initial phase)
								if ( own && cntr->getMission( ) == GroundUnit::Project::AIRFIELD )
								{
									// how many turns needed?
									t = ( hex_[y][x].terrain_ == HexType::CLEAR || hex_[y][x].terrain_ == HexType::ROUGH ? 1 : 2 );
									if ( _weather_condition[hex_[y][x].getWeatherZone( )] != WeatherCondition::CLEAR )
										t *= 2;
									if ( up->getCompletedTurns( ) >= ( t - 1 ) )
									{
										aug = 0;
										ENUM_HEXUNITS( x, y, unit )
										{
											if ( unit->getCounterType( ) == Counter::Type::MISC )
											{
												if ( ( (Facility*)unit )->getMiscType( ) == Facility::PERMAF3 )
												{
													( (Facility*)unit )->setMiscType( Facility::PERMAF6 );
													++aug;
												}
												else if ( ( (Facility*)unit )->getMiscType( ) == Facility::PERMAF6 )
												{
													( (Facility*)unit )->setMiscType( Facility::PERMAF9 );
													++aug;
												}
												else if ( ( (Facility*)unit )->getMiscType( ) == Facility::PERMAF9 )
												{
													( (Facility*)unit )->setMiscType( Facility::PERMAF12 );
													++aug;
												}
												else if ( ( (Facility*)unit )->getMiscType( ) == Facility::PERMAF12 )
												{
													// can't augment any more
													++aug;
												}
											}
										}
										if ( aug )
										{
											SelectHex( x, y );
											hex_[y][x].getHexIdStr( s2 );
											sprintf( msg, "Airfield capacity increased at %s", s2 );
											wxMessageBox( wxS( msg ), Application::NAME );
											updateHexCache( x, y );
											cntr->setMission( GroundUnit::Project::NONE );
											up->setCompletedTurns( 0 );
										}
										else
										{
											SelectHex( x, y );
											unit = new Facility( Facility::PERMAF3 );
											unit->setSide( _current_player );
											sit_[y][x].insertUnit( unit );
											updateHexCache( x, y );
											cntr->setMission( GroundUnit::Project::NONE );
											up->setCompletedTurns( 0 );
											hex_[y][x].getHexIdStr( s2 );
											sprintf( msg, "Airfield construction ready at %s", s2 );
											wxMessageBox( wxS( msg ), Application::NAME );
										}  // end if-else aug
									}
									else
										up->setCompletedTurns( up->getCompletedTurns( ) + 1 );
								}
								// is port repaired?
								if ( own && cntr->getMission( ) == GroundUnit::Project::PORT )
								{
									sit_[y][x].setPortDestroyed( false );
									updateHitMarkers( x, y );
									contents_pane_->deselectAll( );
									contents_pane_->clear( );
									contents_pane_->addUnits( sit_[selected_hex_point_.y][selected_hex_point_.x].unit_list_ );
									contents_pane_->Refresh( );
									cntr->setMission( GroundUnit::Project::NONE );
									hex_[y][x].getHexIdStr( s2 );
									sprintf( msg, "Port at %s is now repaired", s2 );
									wxMessageBox( wxS( msg ), Application::NAME );
								}
								if ( own )
									cntr->clearReplay( );
								break;  // end case CNTR_UNIT
							case Counter::Type::PLANE:
								au = (AirUnit*)cntr;
								au->setFired( FALSE );
								au->setStaged( FALSE );
								au->clearFlag( AirUnit::FOUGHT );
								au->clearFlag( AirUnit::JETTISON );
								au->setActive( TRUE );
								au->setNight( FALSE );
								au->setExtRange( FALSE );
#if 0
								au->SetNavalPatrol( FALSE );  let them be until the end of the world
#endif
								if ( own )
									au->clearReplay( );
								break;
							case Counter::Type::SHIP:
								if ( own )
									cntr->clearReplay( );
							default:
								break;
						}  // end switch cntr type
						cntr = cntr->getNext( );
					}  // end while cntr
				}  // end for int y
			}  // end for int x
			break;  // end case INITIAL PHASE
		case Phase::NAVAL_PHASE_1:
		case Phase::NAVAL_PHASE_2:
		case Phase::NAVAL_PHASE_3:
		case Phase::NAVAL_PHASE_4:
		case Phase::NAVAL_PHASE_5:
		case Phase::NAVAL_PHASE_6:
		case Phase::NAVAL_PHASE_7:
		case Phase::NAVAL_PHASE_8:
		case Phase::NAVAL_PHASE_9:
		case Phase::NAVAL_PHASE_10:
			for ( int x = 0; x < world_x_size_; ++x )
				for ( int y = 0; y < world_y_size_; ++y )
				{
					for ( Counter* cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
					{
						// ships get 30 MPs
						if ( cntr->getCounterType( ) == Counter::Type::SHIP )
						{
							if ( cntr->getMP( ) >= 0 )
								cntr->setMP( 30 );
							else
								cntr->setMP( cntr->getMP( ) + 30 );
						}
					}
				}
			break;
		case Phase::MOVEMENT_PHASE:
			break;
		case Phase::COMBAT_PHASE:
			for ( int x = 0; x < world_x_size_; ++x )
				for ( int y = 0; y < world_y_size_; ++y )
				{
					for ( Counter* cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
					{
						// set homehex for Undo
						if ( cntr->getCounterType( ) == Counter::Type::UNIT )
							( (GroundUnit*)cntr )->setHomeHex( x, y );
						// escort missions are considered done at this time
						if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) == AirUnit::Mission::ESCORT )
							cntr->setMission( AirUnit::Mission::NONE );
					}
				}
			break;
		case Phase::EXPLOITATION_PHASE:
			for ( int x = 0; x < world_x_size_; ++x )
				for ( int y = 0; y < world_y_size_; ++y )
				{
					sit_[y][x].setAirTarget( false );
					sit_[y][x].setTarget( false );
					for ( Counter* cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
					{
						// escort missions are considered done at this time
						if ( cntr->getCounterType( ) == Counter::Type::UNIT )
							cntr->setAttackDir( 0 );
						else if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) != AirUnit::Mission::BOMB_DAS )  // missions (not DAS!)
							cntr->setMission( AirUnit::Mission::NONE );  // cancel all pending
					}
				}
			break;
		default:
			break;
	}  // end switch Phase
#if 0
	if ( har_rem )
		wxMessageBox( "NOTE: harassment hits were removed from non-phasing player hexes" );
#endif
	contents_pane_->Refresh( );
}

#endif
