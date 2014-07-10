#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "hextools.h"

#include "application.h"
#include "selectedunits.loop"
#include "gameunitset.h"
#include "weathercondition.h"
#include "phase.h"
#include "city.h"
#include "sideplayer.h"
#include "rulesvariant.h"
#include "rules.h"
#include "hexcontents.h"
#include "counter.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "groundunit.h"
#include "facility.h"
#include "hexcontentspane.h"

extern char msg[10000];
extern Counter* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _current_player;
extern int _phasing_player;
extern int _current_phase;						// COMBAT_PHASE etc.
extern int _weather_condition[];				// W_MUD etc., per zone (frame.cpp)

extern Rules _rule_set;

// engineers build various things (or in setup mode they are free)
void MapPane::addFacility( int type )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	Facility* facility;

	if ( type == Facility::CONTEST ) // contested hexes are special
	{
		facility = new Facility( type );
		facility->setSide( _current_player );
	}
	// balloon barrages are special:
	else if ( type == Facility::BALLOON )
	{
		facility = new Facility( type );
		facility->setSide( _current_player );
	}
	// mulberries are special:
	else if ( type == Facility::MULBERRY )
	{
		facility = new Facility( type );
		facility->setSide( _current_player );
	}
	else if ( _current_phase == Phase::SETUP_PHASE ) // setup mode, just place counters
	{
		if ( ! isHexSelected( ) )
		{
			SelHexErr( );
			return;
		}

		if (	hex_[y][x].terrain_ == HexType::SEA ||
				hex_[y][x].terrain_ == HexType::RESTRICTEDWATERS ||
				hex_[y][x].terrain_ == HexType::ICINGSEA )
		{
			wxMessageBox( wxT("Can't add counters here!"), Application::NAME );
			return;
		}

		facility = new Facility( type );
		facility->setSide( _current_player );
#if 0
		if ( Player == AXIS )
			facility->setAxis( );
		else
			facility->setAllied( );
#endif
		if ( ! facility->edit( ) ) // show editor dialog
		{
			delete facility;
			return;
		}
	}
	else // not in setup mode, start building using engineers
	{
		if ( _current_player != _phasing_player ) // phasing player only
		{
			SideErr( );
			return;
		}

		// check selection
		Counter* cntr = _selected_unit_list;  // engineer unit to make building
		if ( ! cntr )
		{
			wxMessageBox( wxT("Select unit with construction capabilities\n"
							"(or switch to setup mode where you can freely\n"
							" place counters)."), Application::NAME );
			return;
		}

		// TODO: something is wrong with this logic, there is no GroundUnit::getCounterType( ) function
		if ( cntr->getCounterType( ) != Counter::Type::UNIT || static_cast<GroundUnit*>( cntr )->getCounterType( ) != Counter::Type::UNIT )
		{
			wxMessageBox( wxT("Only engineers and construction units can build things!"), Application::NAME );
			return;
		}

		if ( ! ( (GroundUnit*)cntr )->isConstructionEngineer( ) )
		{
			wxMessageBox( wxT("You must select unit with construction capabilities"), Application::NAME );
			return;
		}

		// ok, now do building
		int mpreq;
		switch ( type )
		{
			case Facility::TEMPAF:
				mpreq = _rule_set.TempAFMP;
#if 0
				mpreq = 4;
#endif
				if ( _weather_condition[hex_[y][x].getWeatherZone( )] != WeatherCondition::CLEAR )
					mpreq *= 2;
				// check legality
				if ( cntr->getStratMove( ) )
				{
					wxMessageBox( wxT("Unit that have used strategic movement may not\nbe used to build anything"), Application::NAME, wxOK );
					return;
				}
				if ( cntr->getAdminMove( ) )
				{
					if ( wxMessageBox( wxT("Unit has used administrative movement, continue?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
						return;
				}
				if ( cntr->getMP( ) < mpreq )
				{
					sprintf( msg, "%d movement points needed to build temporary airfield", mpreq );
					wxMessageBox( ht::wxS( msg ), Application::NAME );
					return;
				}
				cntr->setMP( cntr->getMP( ) - mpreq );
				facility = new Facility( Facility::TEMPAF );
				wxMessageBox( wxT("NOTE: there must always be construction unit in this hex or temporary airfield is removed from the play"), Application::NAME );
				break;
			case Facility::PERMAF3:
				cntr->setMission( GroundUnit::Project::AIRFIELD );
				( (GroundUnit*)cntr )->setCompletedTurns( 0 );
				wxMessageBox( wxT("Construction of permanent airfield started. Remember to reduce one resource point."), Application::NAME );
				contents_pane_->Refresh( );
				return;
			case Facility::FORT:
				if ( hex_[y][x].city_ != NULL )
				{
					switch ( hex_[y][x].city_->type_ )
					{
						case City::Type::UNUSED_WESTWALL:
						case City::Type::WESTWALL_1:
						case City::Type::WESTWALL_2:
						case City::Type::WESTWALL_3:
						case City::Type::UNUSED_OUVRAGE:
						case City::Type::OUVRAGE:
						case City::Type::UNUSED_FORTRESS:
						case City::Type::FORTRESS:
							if ( wxMessageBox( wxT("This hex already contains fortification,\nreally build fort here?\n"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
								return;
					}
				}
				cntr->setMission( GroundUnit::Project::FORT );
				( (GroundUnit*)cntr )->setCompletedTurns( 0 );
				wxMessageBox( wxT("Construction of fort started. Remember to reduce one resource point."), Application::NAME );
				contents_pane_->Refresh( );
				return;
			default:
				wxMessageBox( wxT("bug: MapPane::addFacility( )"), Application::NAME );
				return;
		}
	}

	facility->setHomeHex( x, y );
	// TODO:  no Sit[x][y].SetOwner(..) here?
	sit_[y][x].insertUnit( facility );
	contents_pane_->addUnit( facility );
	updateHexCache( x, y ); // TODO: PaintHex(..) calls updateHexCache(..)
	PaintHex( x, y );
	file_is_dirty_ = true;
}

// ground unit damages airfield
void MapPane::HitAF( )
{
	// only phasing player can do this
#if 0
	if ( Player != RealPlayer )
	  return;
#endif
	if ( GetRawAFCapacity( selected_hex_point_.x, selected_hex_point_.y ) <= 0 )
	{
		wxMessageBox( wxT("There is no airfield here!") );
		return;
	}

	Counter* cntr = _selected_unit_list;
	if ( ! cntr )
	{
		wxMessageBox( wxT("Select unit to damage the airfield") );
		return;
	}

	if ( cntr->getNext( ) )
	{
		wxMessageBox( wxT("Select just one unit") );
		return;
	}

	if ( cntr->getCounterType( ) != Counter::Type::UNIT )
	{
		wxMessageBox( wxT("Only land units can damage airfields by unis MPs!") );
		return;
	}

	if ( cntr->getAdminMove( ) )
	{
		wxMessageBox( wxT("Unit has used administrative movement, it can't use MPs to damage airfield!") );
		return;
	}

	if ( cntr->getStratMove( ) )
	{
		wxMessageBox( wxT("Unit has used strategic movement, it can't use MPs to damage airfield!") );
		return;
	}

	if ( cntr->getMission( ) )
	{
		if ( wxMessageBox( wxT("Cancel construction orders?"), Application::NAME, wxYES_NO ) != wxYES )
			return;
		cntr->setMission( GroundUnit::Project::NONE );
	}

	if ( cntr->getMP( ) < 2 )
	{
		wxMessageBox( wxT("Not enough movement points, can't\ndamage airfield(2 needed)") );
		return;
	}

	if ( wxMessageBox( wxT("Really damage airfield? (Cost is 2 MP)"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
		return;

	cntr->recordReplay( ht::Replay::Movement::HITAF );
	cntr->setMP( cntr->getMP( ) - 2 );
	cntr->setTacticalMove( true );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setAFHits( sit_[selected_hex_point_.y][selected_hex_point_.x].getAFHits( ) + 1 );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	wxMessageBox( wxT("One point of damage done to the airfield!"), Application::NAME );
	contents_pane_->deselectAll( );
	contents_pane_->clear( );
	contents_pane_->addUnits( sit_[selected_hex_point_.y][selected_hex_point_.x].unit_list_ );
	contents_pane_->Refresh( );
}

// ground unit breaks rail
void MapPane::HitRail( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( ! IsRRInHex( selected_hex_point_.x, selected_hex_point_.y ) )
	{
		if ( _rule_set.RoadsCanBeBroken == TRUE )
		{
			if ( ! IsRoadInHex( selected_hex_point_.x, selected_hex_point_.y ) )
			{
				wxMessageBox( wxT("There is no road or railroad here!") );
				return;
			}
		}
		else  // breaking roads not allowed
		{
			if ( IsRoadInHex( selected_hex_point_.x, selected_hex_point_.y ) )
				wxMessageBox( wxT("There is no railroad here!\n"
						"However, there seems to be a road here. If you\n"
						"want to break it, you need to select rule\n"
						"'Options->Rules->Movement->Roads can be broken'") );
			else
				wxMessageBox( wxT("There is no railroad here!") );
			return;
		}
	}

	// in setup mode no unit is needed to break rail
	if ( _current_phase == Phase::SETUP_PHASE )
	{
		sit_[selected_hex_point_.y][selected_hex_point_.x].setRRHit( true );
		updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
		contents_pane_->deselectAll( );
		contents_pane_->clear( );
		contents_pane_->addUnits( sit_[selected_hex_point_.y][selected_hex_point_.x].unit_list_ );
		contents_pane_->Refresh( );
		return;
	}

	// non-setup phase: must be phasing player
	if ( _current_player != _phasing_player )
	{
		SideErr( );
		return;
	}

	Counter* cntr = _selected_unit_list;
	if ( ! cntr )
	{
		wxMessageBox( wxT("Select unit to break the railroad (or switch to setup mode)") );
		return;
	}

	if ( cntr->getNext( ) )
	{
		wxMessageBox( wxT("Select just one unit") );
		return;
	}

	if ( cntr->getCounterType( ) != Counter::Type::UNIT )
	{
		wxMessageBox( wxT("Only land units can break rail lines by using MPs!") );
		return;
	}

	if ( cntr->getAdminMove( ) )
	{
		wxMessageBox( wxT("Unit has used administrative movement, it can't use MPs to break rail!") );
		return;
	}

	if ( cntr->getStratMove( ) )
	{
		wxMessageBox( wxT("Unit has used strategic movement, it can't use MPs to break rail!") );
		return;
	}

	if ( cntr->getMission( ) )
	{
		if ( wxMessageBox( wxT("Cancel construction orders?"), Application::NAME, wxYES_NO ) != wxYES )
			return;
		cntr->setMission( GroundUnit::Project::NONE );
	}

	if ( sit_[selected_hex_point_.y][selected_hex_point_.x].getRRHit( ) )
	{
		wxMessageBox( wxT("Railroad is already broken here!") );
		return;
	}

	if ( cntr->getMP( ) < 2 )
	{
		wxMessageBox( wxT("Not enough movement points, can't break rail\n(2 MPs needed)") );
		return;
	}

	if ( wxMessageBox( wxT("Really break rail line? (Cost is 2 MP)"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
		return;

	cntr->setMP( cntr->getMP( ) - 2 );
	cntr->setTacticalMove( true );
	cntr->recordReplay( ht::Replay::Movement::HITRAIL );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setRRHit( true );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->deselectAll( );
	contents_pane_->clear( );
	contents_pane_->addUnits( sit_[selected_hex_point_.y][selected_hex_point_.x].unit_list_ );
	contents_pane_->Refresh( );
}

void MapPane::Repair( )
{
	// repairs can only be made in movement phase (and in setup mode)
	if ( _current_phase != Phase::MOVEMENT_PHASE && _current_phase != Phase::SETUP_PHASE )
		return;

	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// in setup mode, always fix RR hit (airfields are handled elsewhere)
	if ( _current_phase == Phase::SETUP_PHASE )
	{
		if ( sit_[y][x].getRRHit( ) )
		{
			if ( wxMessageBox( wxT("Magically fix railroad?"), Application::NAME, wxYES_NO ) == wxYES )
			{
				sit_[y][x].setRRHit( false );
				updateHitMarkers( x, y );
				contents_pane_->deselectAll( );
				contents_pane_->clear( );
				contents_pane_->addUnits( sit_[y][x].unit_list_ );
				contents_pane_->Refresh( );
				paintCurrentHex( );
				return;
			}
		}
		if ( sit_[y][x].getAFHits( ) )
		{
			if ( wxMessageBox( wxT("Magically fix airbase hit?"), Application::NAME, wxYES_NO ) == wxYES )
			{
				sit_[y][x].setAFHits( sit_[y][x].getAFHits( ) - 1 );
				updateHitMarkers( x, y );
				contents_pane_->deselectAll( );
				contents_pane_->clear( );
				contents_pane_->addUnits( sit_[y][x].unit_list_ );
				contents_pane_->Refresh( );
				paintCurrentHex( );
				return;
			}
		}
		if ( sit_[y][x].getPortHits( ) )
		{
			if ( wxMessageBox( wxT("Magically fix port hit?"), Application::NAME, wxYES_NO ) == wxYES )
			{
				sit_[y][x].setPortHits( sit_[y][x].getPortHits( ) - 1 );
				updateHitMarkers( x, y );
				contents_pane_->deselectAll( );
				contents_pane_->clear( );
				contents_pane_->addUnits( sit_[y][x].unit_list_ );
				contents_pane_->Refresh( );
				paintCurrentHex( );
				return;
			}
		}
		return;  // if there is nothing to repair
	}

	// MOVEMENT PHASE
	// only phasing player can repair
	if ( _current_player != _phasing_player )
	{
		wxMessageBox( wxT("Only phasing player can repair things") );
		return;
	}

	// check selection
	Counter* cntr = _selected_unit_list;
	if ( ! cntr )
	{
		wxMessageBox( wxT("Select unit with construction capabilities to make repairs") );
		return;
	}
	if ( cntr->getSide( ) != _current_player )
	{
		SideErr( );
		return;
	}
	if ( cntr->getCounterType( ) != Counter::Type::UNIT )
	{
		wxMessageBox( wxT("Only engineers and construction\nunits can repair things!") );
		return;
	}

	bool rr_eng = false;
	int count = 0;
	ENUM_SELUNITS(cntr)
	{
		if ( ! ( (GroundUnit*)cntr )->isConstructionEngineer( ) && ( (GroundUnit*)cntr )->getType( ) != GroundUnitType::WW2::RR_ENG )
		{
			wxMessageBox( wxT("You must select unit(s) with construction capabilities") );
			return;
		}
		if ( ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::RR_ENG )
			rr_eng = true;
		++count;
	}

	// cannot select more than 2 units
	if ( count > 2 )
	{
		wxMessageBox( wxT("Select just one or two (for quick construction) units") );
		return;
	}

	// check legality, strategic rail & admin move may not spend MPs for any other purpose
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getStratMove( ) )
		{
			wxMessageBox( wxT("Unit that have used strategic movement may not\nbe used to repair anything"), Application::NAME, wxOK );
			return;
		}
		if ( cntr->getAdminMove( ) )
		{
			if ( wxMessageBox( wxT("Unit(s) have used administrative movement, continue?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
				return;  // if user selects "No" (don't continue)
		}
	}

	// eligible engineer(s) selected, proceed...

	int base_port_repair_rate = 4;
	int base_rail_repair_rate = 4;
	int base_rail_clearance_rate = 2;
	int base_airfield_repair_rate = 2;

	int qc_divisor = 1;
	// quick construction rate for 2 units or 1 brigade
	if ( count == 2 || ( (GroundUnit*)_selected_unit_list )->getSize( ) == GroundUnit::Size::BRIGADE )
		qc_divisor = 2;

	int wx_factor = 1;
	// in poor weather (frost, mud, winter, snow) double required MPs
	if ( _weather_condition[hex_[y][x].getWeatherZone( )] > WeatherCondition::CLEAR ) wx_factor = 2;

	if ( sit_[y][x].getRRHit( ) )
	{
		// subtract 1 from the base MP cost for WW1 RR Eng units
		base_rail_repair_rate = ( ( rr_eng && ( _rule_set.Era & GameEra::WW1 ) ) ?
		base_rail_repair_rate - 1 :
		base_rail_repair_rate );

		int mp_halves = 2 * base_rail_repair_rate / qc_divisor * wx_factor;

		sprintf( msg, "Do you want to fix the damaged railroad?\n(Cost is %f MPs per engineer unit)", mp_halves / 2.0f );
		if ( wxMessageBox( ht::wxS( msg ), wxT("Repair Railroad"), wxYES_NO | wxICON_QUESTION) == wxYES )
		{
			ENUM_SELUNITS(cntr)
				if ( cntr->getMP( ) * 2 + cntr->isHalfMP( ) < mp_halves )
				{
					wxMessageBox( wxT("Insufficient MPs"), wxT("Error"), wxOK | wxICON_HAND );
					return;
				}

			ENUM_SELUNITS(cntr)
			{
				cntr->setMP( cntr->getMP( ) - mp_halves / 2 );

				if ( mp_halves % 2 )  // 1/2 MP remainder
				{
					if ( cntr->isHalfMP( ) )
						cntr->setHalfMP( FALSE );  // remove the 1/2MP
					else
					{
						cntr->setMP( cntr->getMP( ) - 1 );  // knock down full MP
						cntr->setHalfMP( TRUE );  // then add the 1/2 MP
					}
				}

				cntr->recordReplay( ht::Replay::Movement::FIXRAIL );
			}

			sit_[y][x].setRRHit( false );
			updateHitMarkers( x, y );
			contents_pane_->deselectAll( );
			contents_pane_->clear( );
			contents_pane_->addUnits( sit_[y][x].unit_list_ );
			contents_pane_->Refresh( );
			return;
		}
	}  // else-if, because rail obstruction cannot be cleared until rail hit is repaired
	else if ( _rule_set.RailsObstructed == TRUE && sit_[y][x].isRailObstructed( ) )
	{
		// subtract 1 from the base MP cost for WW1 RR Eng units
		base_rail_clearance_rate = ( ( rr_eng && ( _rule_set.Era & GameEra::WW1 ) ) ? base_rail_clearance_rate - 1 : base_rail_clearance_rate );

		int mp_halves = 2 * base_rail_clearance_rate / qc_divisor * wx_factor;

		sprintf( msg, "Do you want to clear the obstructed railroad?\n(Cost is %f MPs per engineer unit)", mp_halves / 2.0f );
		if ( wxMessageBox( ht::wxS( msg ), wxT("Clear Railroad"), wxYES_NO | wxICON_QUESTION) == wxYES )
		{
			ENUM_SELUNITS(cntr)
				if ( 2 * cntr->getMP( ) + cntr->isHalfMP( ) < mp_halves )
				{
					wxMessageBox( wxT("Insufficient MPs"), wxT("Error"), wxOK | wxICON_HAND) ;
					return;
				}

			ENUM_SELUNITS(cntr)
			{
				cntr->setMP( cntr->getMP( ) - mp_halves / 2 );

				if ( mp_halves % 2 )  // 1/2 MP remainder
				{
					if ( cntr->isHalfMP( ) )
						cntr->setHalfMP( FALSE );  // remove the 1/2MP
					else
					{
						cntr->setMP( cntr->getMP( ) - 1 );  // knock down full MP
						cntr->setHalfMP( TRUE );  // then add the 1/2 MP
					}
				}

				cntr->recordReplay( ht::Replay::Movement::CLEARRAIL );
			}

			sit_[y][x].setRailObstructed( false );
			updateHitMarkers( x, y );

			contents_pane_->deselectAll( );
			contents_pane_->clear( );
			contents_pane_->addUnits( sit_[y][x].unit_list_ );
			contents_pane_->Refresh( );
			return;
		}
	}

	if ( rr_eng )
	{
		wxMessageBox( wxT("Railroad engineers can repair only railroads") );
		return;
	}

	if ( sit_[y][x].getAFHits( ) )
	{
		int mp_halves = 2 * base_airfield_repair_rate / qc_divisor * wx_factor;

		sprintf( msg, "Do you want to fix the damaged airfield?\n(Cost is %f MPs per engineer unit)", mp_halves / 2.0f );
		if ( wxMessageBox( ht::wxS( msg ), wxT("Repair Airfield"), wxYES_NO | wxICON_QUESTION) == wxYES )
		{
			ENUM_SELUNITS(cntr)
				if ( 2 * cntr->getMP( ) + cntr->isHalfMP( ) < mp_halves )
				{
					wxMessageBox( wxT("Insufficient MPs"), wxT("Error"), wxOK | wxICON_HAND );
					return;
				}

			ENUM_SELUNITS(cntr)
			{
				cntr->setMP( cntr->getMP( ) - mp_halves / 2 );

				if ( mp_halves % 2 )  // 1/2 MP remainder
				{
					if ( cntr->isHalfMP( ) )
						cntr->setHalfMP( FALSE );  // remove the 1/2MP
					else
					{
						cntr->setMP( cntr->getMP( ) - 1 );  // knock down full MP
						cntr->setHalfMP( TRUE );  // then add the 1/2 MP
					}
				}

				cntr->recordReplay( ht::Replay::Movement::FIXAF );
			}
			sit_[y][x].setAFHits( sit_[y][x].getAFHits( ) - 1 );
			updateHitMarkers( x, y );
			contents_pane_->deselectAll( );
			contents_pane_->clear( );
			contents_pane_->addUnits( sit_[y][x].unit_list_ );
			contents_pane_->Refresh( );
			return;
		}
	}

	if ( sit_[y][x].getPortHits( ) )
	{
		int mp_halves = 2 * base_port_repair_rate / qc_divisor * wx_factor;

		sprintf( msg, "Do you want to fix the damaged port?\n(Cost is %f MPs per engineer unit)", mp_halves / 2.0f );
		if ( wxMessageBox( ht::wxS( msg ), wxT("Repair Port"), wxYES_NO | wxICON_QUESTION) == wxYES )
		{
			ENUM_SELUNITS(cntr)
				if ( 2 * cntr->getMP( ) + cntr->isHalfMP( ) < mp_halves )
				{
					wxMessageBox( wxT("Insufficient MPs"), wxT("Error"), wxOK | wxICON_HAND );
					return;
				}

			ENUM_SELUNITS(cntr)
			{
				cntr->setMP( cntr->getMP( ) - mp_halves / 2 );

				if ( mp_halves % 2 )  // 1/2 MP remainder
				{
					if ( cntr->isHalfMP( ) )
						cntr->setHalfMP( FALSE );  // remove the 1/2MP
					else
					{
						cntr->setMP( cntr->getMP( ) - 1 );  // knock down full MP
						cntr->setHalfMP( TRUE );  // then add the 1/2 MP
					}
				}

				cntr->recordReplay( ht::Replay::Movement::FIXPORT );
			}

			sit_[y][x].setPortHits( sit_[y][x].getPortHits( ) - 1 );
			updateHitMarkers( x, y );
			contents_pane_->deselectAll( );
			contents_pane_->clear( );
			contents_pane_->addUnits( sit_[y][x].unit_list_ );
			contents_pane_->Refresh( );
			return;
		}
	}
	wxMessageBox( wxT("Nothing to repair"), Application::NAME );
}

void MapPane::RestoreAFCapacity( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( sit_[y][x].getAFUsage( ) > 0 )
		sit_[y][x].setAFUsage( sit_[y][x].getAFUsage( ) - 1 );
}

// RR Eng regauges railroad
void MapPane::Regauge( void )
{
	// only phasing player can do this
	if ( _current_player != _phasing_player )
	{
		wxMessageBox( wxT("Only phasing player can regauge railroads") );
		return;
	}

	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( ! IsRRInHex( x, y ) )
	{
		wxMessageBox( wxT("There is no railroad here!") );
		return;
	}

	GroundUnit* rr_eng = NULL;
	GroundUnit* help_eng = NULL;

	// setup-mode: do immediately
	if ( _current_phase != Phase::SETUP_PHASE )
	{
		// is RR Eng chosen (and helper Cons)
		Counter* cntr = _selected_unit_list;
		while ( cntr )
		{
			if ( cntr->getCounterType( ) == Counter::Type::UNIT )
			{
				if ( ! rr_eng && ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::RR_ENG )
					rr_eng = (GroundUnit*)cntr;
				// helper: can be RR Eng or any construction capable unit
				else if ( ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::RR_ENG
						|| ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::CONS ) // TODO: this unit has the construction flag, no need to check it
					help_eng = (GroundUnit*)cntr;
				// TODO: DLT
				//|| ( (GroundUnit*)cntr )->isConstructionEngineer( ) ) && RuleSet.RulesVariant != ERV_A )
			}
			cntr = cntr->getNext( );
		}

		if ( ! rr_eng )
		{
			wxMessageBox( wxT("You must select railroad engineer unit(s) to regauge railroad") );
			return;
		}

		// if we have RR Eng / Cons, check legality:
		if ( rr_eng->getStratMove( ) || ( help_eng && help_eng->getStratMove( ) ) )
		{
			wxMessageBox( wxT("Unit(s) that have used strategic movement may not\nbe used to regauge railroad"), Application::NAME, wxOK );
			return;
		}
		if ( rr_eng->getAdminMove( ) || ( help_eng && help_eng->getAdminMove( ) ) )
		{
			wxMessageBox( wxT("Unit(s) have used administrative movement may not\nbe used to regauge railroad"), Application::NAME, wxOK );
			return;
		}

		// NOTE: MP required calculations in 1/2 MP increments!
		int hmp_required;
		// TODO: DLT
		if ( rr_eng->getSize( ) >= GroundUnit::Size::BRIGADE ) // || ( RuleSet.RulesVariant == ERV_A && rr_eng->isGerman( ) ) )
		{
			if ( help_eng )
			{
				wxMessageBox( wxT("RR engineer brigade may regauge with cost of 1/2 MPs\nNo helping engineers are used"), Application::NAME );
				help_eng = NULL;  // remove helper Cons from this selected units list
			}
			hmp_required = 1;  // QC 1/2 MP
		}
		else
			// RR regiment
			hmp_required = help_eng ? 1 : 2;  // QC 1/2 MP or regular 1 MP

		// TODO: DLT
		// Variant A RR engineers regauge rate per hex: German at 1/2 MP, Soviet at 2 MP
		//if ( RuleSet.RulesVariant == ERV_A && rr_eng->isSoviet( ) )
		//	hmp_required *= 2;

		// in poor weather (Frost, Mud, Winter, Snow) double required MPs
		if ( _weather_condition[hex_[y][x].getWeatherZone( )] > WeatherCondition::CLEAR )
			hmp_required *= 2;

		// NOTE: MP remaining calculations in 1/2 MP increments!

		// RR Eng remaining half MPs
		int hmp_remaining = rr_eng->getMP( ) * 2 + rr_eng->isHalfMP( );
		if ( hmp_required > hmp_remaining )
		{
			wxMessageBox( wxT("Railroad engineers do not have enough\nmovement points to regauge railroad") );
			return;
		}

		// Helper Cons remaining half MPs
		if ( help_eng )
		{
			hmp_remaining = help_eng->getMP( ) * 2 + help_eng->isHalfMP( );
			if ( hmp_required > hmp_remaining )
			{
				wxMessageBox( wxT("Helping engineers do not have enough\nmovement points to regauge railroad") );
				return;
			}
		}

		// adjust RR Eng MPs
		rr_eng->setMP( rr_eng->getMP( ) - hmp_required / 2 );
		if ( hmp_required % 2 )  // odd number of 1/2 MPs required
		{
			if ( rr_eng->isHalfMP( ) )  // just remove the existing
				rr_eng->setHalfMP( FALSE );  // avail. 1/2 MP
			else
			{
				rr_eng->setHalfMP( TRUE );  // add a 1/2 MP, and bump
				rr_eng->setMP( rr_eng->getMP( ) - 1 );  // full MPs down by 1
			}
		}
		/*
		 if ( hmp_required == 2 )
		 	 rr_eng->SetMP( rr_eng->GetMP() - 1 );
		 else if ( rr_eng->IsHalfMP() ) // mp == 1
		 	 rr_eng->SetHalfMP( 0 );
		 else
		 {
			 rr_eng->SetHalfMP( 1 ),
			 rr_eng->SetMP( rr_eng->GetMP() - 1 );
		 }
		 */

		// adjust helper Cons MPs
		if ( help_eng )
		{
			help_eng->setMP( help_eng->getMP( ) - hmp_required / 2 );
			if ( hmp_required % 2 )  // odd number of 1/2 MPs required
			{
				if ( help_eng->isHalfMP( ) )  // just remove the existing
					help_eng->setHalfMP( FALSE );  // avail. 1/2 MP
				else
				{
					help_eng->setHalfMP( TRUE );  // add a 1/2 MP, and bump
					help_eng->setMP( rr_eng->getMP( ) - 1 );  // full MPs down by 1
				}
			}
		}
		/*
		 if ( ! help_eng )
		 	 ;
		 else if ( hmp_required == 2 )
		 	 help_eng->SetMP( help_eng->GetMP() - 1 );
		 else if ( help_eng->IsHalfMP() )
		 	 help_eng->SetHalfMP( 0 );
		 else
		 {
			 help_eng->SetHalfMP( 1 )
			 help_eng->SetMP( help_eng->GetMP() - 1 );
		 }
		 */
	}

	// invert gauge
	sit_[y][x].setGauge( sit_[y][x].getGauge( ) == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );

	Refresh( );
	contents_pane_->Refresh( );
}

// 1=all, 2=visible
void MapPane::MagicGauge( int mode )
{
	int xmax, xmin = 0, ymax, ymin = 0, g;

	if ( mode == 1 )
	{
		if ( wxMessageBox( wxT("This will toggle gauge of ALL hexes!\nAre you sure you want to do it?"), Application::NAME, wxYES_NO ) != wxYES )
			return;

		g = sit_[0][0].getGauge( );  // get current gauge
		g = ( g == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );  // toggle it

		for ( int y = 0; y < world_y_size_; ++y )
			for ( int x = 0; x < world_x_size_; ++x )
				sit_[y][x].setGauge( g );
	}
	else if ( mode == 2 )
	{
		if ( wxMessageBox( wxT("This will toggle gauge of all currently visible hexes!\nAre you sure you want to do it?"), Application::NAME, wxYES_NO ) != wxYES )
			return;

		GetClientSize( &xmax, &ymax );

		screen2Hex( &xmin, &ymin );
		screen2Hex( &xmax, &ymax );

		ymax = ( ymax > world_y_size_ ? world_y_size_ : ymax );
		xmax = ( xmax > world_x_size_ ? world_x_size_ : xmax );

		g = sit_[ymin][xmin].getGauge( );  // get current gauge
		g = ( g == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );  // toggle it

		for ( int y = ymin; y < ymax; ++y )
			for ( int x = xmin; x < xmax; ++x )
				sit_[y][x].setGauge( g );
	}
	else
		wxMessageBox( wxT("Weird, I have no idea what to do") );  // TODO: assert this

	Refresh( );
}

// TODO: this logic appears to be toggle not clear
void MapPane::ClearObstructed( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( _current_phase == Phase::SETUP_PHASE )
	{
		if ( sit_[selected_hex_point_.y][selected_hex_point_.x].isRailObstructed( ) )
			sit_[selected_hex_point_.y][selected_hex_point_.x].setRailObstructed( false );
		else
			sit_[selected_hex_point_.y][selected_hex_point_.x].setRailObstructed( true );
		return;
	}

	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );

	wxMessageBox( wxT("TODO") ); // TODO what?
}

// 1=all, 2=visible
void MapPane::MagicClearance( int mode )
{
	if ( mode == 1 )
	{
		if ( wxMessageBox( wxT("This will clear obstruction of ALL rail hexes!\nAre you sure you want to do it?"), Application::NAME, wxYES_NO ) != wxYES )
			return;

		for ( int y = 0; y < world_y_size_; ++y )
			for ( int x = 0; x < world_x_size_; ++x )
			{
				sit_[y][x].setRailObstructed( false );
				updateHitMarkers( x, y );
			}
	}
	else if ( mode == 2 )
	{
		if ( wxMessageBox( wxT("This will clear obstruction of all currently visible rail hexes!\nAre you sure you want to do it?"), Application::NAME, wxYES_NO ) != wxYES )
			return;

		int xmax = 0;
		int xmin = 0;
		int ymax = 0;
		int ymin = 0;

		GetClientSize( &xmax, &ymax );

		screen2Hex( &xmin, &ymin );
		screen2Hex( &xmax, &ymax );

		ymax = ( ymax > world_y_size_ ? world_y_size_ : ymax );
		xmax = ( xmax > world_x_size_ ? world_x_size_ : xmax );

		for ( int y = ymin; y < ymax; ++y )
			for ( int x = xmin; x < xmax; ++x )
			{
				sit_[y][x].setRailObstructed( false );
				updateHitMarkers( x, y );
			}
	}
	else
		wxMessageBox( wxT("Weird, I have no idea what to do") );  // TODO: assert this

	Refresh( );
}

void MapPane::RepairBridge( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( IsBridge( x, y, Hex::WEST ) && IsBridgeBreak( x, y, Hex::WEST ) )
	{
		if ( wxMessageBox( wxT("Do you want repair bridge at WEST?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
			SetBridgeBreak( x, y, Hex::WEST, false );
	}
	if ( IsBridge( x, y, Hex::NORTHWEST ) && IsBridgeBreak( x, y, Hex::NORTHWEST ) )
	{
		if ( wxMessageBox( wxT("Do you want repair bridge at NORTHWEST?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
			SetBridgeBreak( x, y, Hex::NORTHWEST, false );
	}
	if ( IsBridge( x, y, Hex::SOUTHWEST ) && IsBridgeBreak( x, y, Hex::SOUTHWEST ) )
	{
		if ( wxMessageBox( wxT("Do you want repair bridge at SOUTHWEST?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
			SetBridgeBreak( x, y, Hex::SOUTHWEST, false );
	}
	if ( IsBridge( x, y, Hex::EAST ) && IsBridgeBreak( x, y, Hex::EAST ) )
	{
		if ( wxMessageBox( wxT("Do you want repair bridge at EAST?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
			SetBridgeBreak( x, y, Hex::EAST, false );
	}
	if ( IsBridge( x, y, Hex::NORTHEAST ) && IsBridgeBreak( x, y, Hex::NORTHEAST ) )
	{
		if ( wxMessageBox( wxT("Do you want repair bridge at NORTHEAST?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
			SetBridgeBreak( x, y, Hex::NORTHEAST, false );
	}
	if ( IsBridge( x, y, Hex::SOUTHEAST ) && IsBridgeBreak( x, y, Hex::SOUTHEAST ) )
	{
		if ( wxMessageBox( wxT("Do you want repair bridge at SOUTHEAST?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
			SetBridgeBreak( x, y, Hex::SOUTHEAST, false );
	}
	Refresh( );
}

void MapPane::BreakBridge( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( IsBridge( x, y, Hex::WEST ) && ! IsBridgeBreak( x, y, Hex::WEST ) )
	{
		if ( wxMessageBox( wxT("Do you want break bridge at WEST?"), Application::NAME, wxYES_NO | wxICON_QUESTION) == wxYES )
			SetBridgeBreak( x, y, Hex::WEST, true );
	}
	if ( IsBridge( x, y, Hex::NORTHWEST ) && ! IsBridgeBreak( x, y, Hex::NORTHWEST ) )
	{
		if ( wxMessageBox( wxT("Do you want break bridge at NORTHWEST?"), Application::NAME, wxYES_NO | wxICON_QUESTION) == wxYES )
			SetBridgeBreak( x, y, Hex::NORTHWEST, true );
	}
	if ( IsBridge( x, y, Hex::SOUTHWEST ) && ! IsBridgeBreak( x, y, Hex::SOUTHWEST ) )
	{
		if ( wxMessageBox( wxT("Do you want break bridge at SOUTHWEST?"), Application::NAME, wxYES_NO | wxICON_QUESTION) == wxYES )
			SetBridgeBreak( x, y, Hex::SOUTHWEST, true );
	}
	if ( IsBridge( x, y, Hex::EAST ) && ! IsBridgeBreak( x, y, Hex::EAST ) )
	{
		if ( wxMessageBox( wxT("Do you want break bridge at EAST?"), Application::NAME, wxYES_NO | wxICON_QUESTION) == wxYES )
			SetBridgeBreak( x, y, Hex::EAST, true );
	}
	if ( IsBridge( x, y, Hex::NORTHEAST ) && ! IsBridgeBreak( x, y, Hex::NORTHEAST ) )
	{
		if ( wxMessageBox( wxT("Do you want break bridge at NORTHEAST?"), Application::NAME, wxYES_NO | wxICON_QUESTION) == wxYES )
			SetBridgeBreak( x, y, Hex::NORTHEAST, true );
	}
	if ( IsBridge( x, y, Hex::SOUTHEAST ) && ! IsBridgeBreak( x, y, Hex::SOUTHEAST ) )
	{
		if ( wxMessageBox( wxT("Do you want break bridge at SOUTHEAST?"), Application::NAME, wxYES_NO | wxICON_QUESTION) == wxYES )
			SetBridgeBreak( x, y, Hex::SOUTHEAST, true );
	}
	Refresh( );
}

// TODO: logic appears to be toggle destruction, not destroy
void MapPane::DestroyPort( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( sit_[selected_hex_point_.y][selected_hex_point_.x].isPortDestroyed( ) )
		sit_[selected_hex_point_.y][selected_hex_point_.x].setPortDestroyed( false );
	else
		sit_[selected_hex_point_.y][selected_hex_point_.x].setPortDestroyed( true );

	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

// start rebuilding destroyed port
void MapPane::RepairPort( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( _current_player != _phasing_player )
	{
		wxMessageBox( wxT("Only phasing player can repair ports") );
		return;
	}

	// check selection
	Counter* cntr = _selected_unit_list;  // engineer unit to rebuild port
	if ( ! cntr )
	{
		wxMessageBox( wxT("Select a port construction unit"), Application::NAME );
		return;
	}

	if ( cntr->getCounterType( ) != Counter::Type::UNIT || ( (GroundUnit*)cntr )->getType( ) != GroundUnitType::WW2::PORT_CONS )
	{
		wxMessageBox( wxT("Only port construction units can rebuild destroyed ports!"), Application::NAME );
		return;
	}

	cntr->setMission( GroundUnit::Project::PORT );
	wxMessageBox( wxT("Port rebuilding started, please decrease your resource points"), Application::NAME );

	PaintHex( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
}

void MapPane::Fieldworks( )
{
	// fieldworks can only be made in movement phase (and in setup mode)
	if ( _current_phase != Phase::MOVEMENT_PHASE && _current_phase != Phase::SETUP_PHASE )
		return;

	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	// in setup mode always fix rr hit (airfields are handled elsewhere)
	if ( _current_phase == Phase::SETUP_PHASE )
		return;

	// only phasing player can build
	if ( _current_player != _phasing_player )
	{
		wxMessageBox( wxT("Only phasing player can build fieldworks") );
		return;
	}

	// check selection
	Counter* cntr = _selected_unit_list;
	if ( ! cntr )
	{
		wxMessageBox( wxT("Select unit with construction capabilities to build fieldworks") );
		return;
	}

	if ( cntr->getSide( ) != _current_player )
	{
		SideErr( );
		return;
	}

	if ( cntr->getCounterType( ) != Counter::Type::UNIT )
	{
		wxMessageBox( wxT("Only engineers and construction\nunits can build fieldworks!") );
		return;
	}

	int cnt = 0;
	ENUM_SELUNITS(cntr)
	{
		if ( ! ( (GroundUnit*)cntr )->isConstructionEngineer( ) && ( (GroundUnit*)cntr )->getType( ) != GroundUnitType::WW2::RR_ENG )
		{
			wxMessageBox( wxT("You must select unit(s) with construction capabilities") );
			return;
		}
		++cnt;
	}
	if ( cnt > 2 )
	{
		wxMessageBox( wxT("Select just one or two (for quick construction) units") );
		return;
	}

	// check legality
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getStratMove( ) )
		{
			wxMessageBox( wxT("Unit that have used strategic movement may not\nbe used to build anything"), Application::NAME, wxOK );
			return;
		}
		if ( cntr->getAdminMove( ) )
		{
			if ( wxMessageBox( wxT("Unit have used administrative movement, continue?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
				return;
		}
	}

	int mp_req = cnt == 1 ? 2 : 1;

	// in poor weather costs are doubled:
	if ( _weather_condition[hex_[selected_hex_point_.y][selected_hex_point_.x].getWeatherZone( )] != WeatherCondition::CLEAR )
		mp_req *= 2;

	ENUM_SELUNITS(cntr)
		if ( cntr->getMP( ) < mp_req )
		{
			wxMessageBox( wxT("Not enough movement points"), wxT("Error"), wxOK | wxICON_HAND );
			return;
		}

	ENUM_SELUNITS(cntr)
	{
		cntr->setMP( cntr->getMP( ) - mp_req );
		// TODO: make fieldworks replayable
		//cntr->RecordReplay(RPL_FIXRAIL);
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	Facility* facility = new Facility( Facility::FIELDWORKS );
	facility->setSide( _current_player );
	facility->setHomeHex( x, y );
	sit_[y][x].insertUnit( facility );
	contents_pane_->addUnit( facility );
	updateHexCache( x, y ); // TODO: PaintHex(..) calls updateHexCache(..)
	PaintHex( x, y );
	file_is_dirty_ = true;
}

#endif
