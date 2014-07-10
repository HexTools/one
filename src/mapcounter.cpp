#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "selectedunits.loop"
#include "hexunits.loop"
#include "phase.h"
#include "supplystatus.h"
#include "disruptionstatus.h"
#include "sideplayer.h"

#include "hextools.h"
using ht::wxS;
using ht::coordinatesOutOfBounds;

#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "hexcontents.h"
#include "counter.h"
#include "groundunittype.h"
#include "groundunitsubtype.h"
#include "groundunit.h"
#include "airunitsubtype.h"
#include "airunittype.h"
#include "airunit.h"
#include "navalunitsubtype.h"
#include "navalunit.h"
#include "resourcemarker.h"
#include "hexnote.h"

#include "groundunitreplpool.h"
#include "airunitreplpool.h"

#include "hexcontentspane.h"

extern char msg[10000];
extern GroundUnit* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _current_player;
extern int _phasing_player;
extern int _current_phase;							// COMBAT_PHASE etc.
extern int _year;
extern int _month;
extern int _turn;

extern GroundUnitReplPool _repl_pool[2];		// AXIS and ALLIED
extern AirUnitReplPool _air_repl_pool[2];		// AXIS and ALLIED

//extern GroundUnitType UnitTypes[];											// unit types
extern GroundArmedForce _ground_armed_force[][GroundArmedForce::Allied::COUNT];	// unit sub types, per side
extern AirArmedForce _air_armed_force[][AirArmedForce::Allied::COUNT];			// unit sub types, per side
extern NavalArmedForce _naval_armed_force[][NavalArmedForce::Allied::COUNT];	// unit sub types, per side

// TODO: extract these #defines
#define SIDEMAP_SIZE 24
#define AIRSIDEMAP_SIZE 16
#define SHIPSIDEMAP_SIZE 15
extern int _unit_subtype_side_map[][2];
extern int _airunit_subtype_side_map[][2];
extern int _navalunit_subtype_side_map[][2];

void MapPane::CmAddUnit( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		//wxMessageBox( wxT("First select hex where you want to add units"), Application::NAME );
		return;
	}

	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::SEA
		|| hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::RESTRICTEDWATERS
		|| hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::ICINGSEA )
	{
		wxMessageBox( wxT("Your units can't float!"), Application::NAME );
		return;
	}

	GroundUnit* unit = new GroundUnit;
	unit->setSide( _current_player );
#if 0
	if ( _current_player == AXIS )
		unit->setAxis( );
	else
		unit->setAllied( );
#endif
	if ( ! unit->edit( ) ) // show editor dialog
	{
		delete unit;
		return;
	}

	// check if this was a valid unit...
	if ( ! unit->isValidUnit( ) )
	{
		wxMessageBox( wxT("Not a valid unit!") );
		delete unit;
		return;
	}

	unit->setHomeHex( selected_hex_point_.x, selected_hex_point_.y );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setOwner( _current_player );
	sit_[selected_hex_point_.y][selected_hex_point_.x].insertUnit( unit );
	contents_pane_->addUnit( unit );
	updateHexCache( selected_hex_point_.x, selected_hex_point_.y ); // TODO: PaintHex(..) calls updateHexCache(..)
	PaintHex( selected_hex_point_.x, selected_hex_point_.y );
	file_is_dirty_ = true;
}

void MapPane::addAirUnit( void )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::SEA
		|| hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::RESTRICTEDWATERS
		|| hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::ICINGSEA
		|| GetRawAFCapacity( selected_hex_point_.x, selected_hex_point_.y ) == 0 )
	{
		if ( wxMessageBox( wxT("There is no functional air base, still place planes here?"), Application::NAME, wxYES_NO ) != wxYES )
			return;
	}

	AirUnit* airunit = new AirUnit;
	airunit->setSide( _current_player );

	//if ( _current_player == AXIS )
	//	airunit->setAxis( );
	//else
	//	airunit->setAllied( );

	if ( ! airunit->edit( ) ) // show editor dialog
	{
		delete airunit;
		return;
	}

	airunit->setHomeHex( selected_hex_point_.x, selected_hex_point_.y );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setOwner( _current_player );
	sit_[selected_hex_point_.y][selected_hex_point_.x].insertUnit( airunit );
	contents_pane_->addUnit( airunit );
	PaintHex( selected_hex_point_.x, selected_hex_point_.y ); // calls updateHexCache(..)
	file_is_dirty_ = true;
}

void MapPane::addNavalUnit( int WXUNUSED( type ) /* = SHIP_CL */ )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( sit_[selected_hex_point_.y][selected_hex_point_.x].getOwner( ) != _current_player )
	{
		wxMessageBox( wxT("Can't add ships to enemy owned hex!"), Application::NAME );
		return;
	}

	// TODO: IsPort(..) would allow placement of naval units in coastal cities without ports?
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ != HexType::SEA
		&& hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ != HexType::RESTRICTEDWATERS
		&& hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ != HexType::ICINGSEA && ! IsPort( selected_hex_point_.x, selected_hex_point_.y )
		&& ! IsCoastalHex( selected_hex_point_.x, selected_hex_point_.y ) )
		if ( wxMessageBox( wxT("There seems to be no sea/harbor here,\n"
				"are you sure you want to place ships here?"), Application::NAME, wxOK | wxCANCEL | wxICON_QUESTION ) != wxOK )
		{
			return;
		}

	NavalUnit* navunit = new NavalUnit( );
	navunit->setSide( _current_player );

	//if ( Player == AXIS	)
	//	navunit->setAxis( );
	//else
	//	navunit->setAllied( );

	if ( ! navunit->edit( ) ) // show editor dialog
	{
		delete navunit;
		return;
	}

	navunit->setHomeHex( selected_hex_point_.x, selected_hex_point_.y );
	// TODO:  no .SetOwner(..) here?
	sit_[selected_hex_point_.y][selected_hex_point_.x].insertUnit( navunit );
	contents_pane_->addUnit( navunit );
	updateHexCache( selected_hex_point_.x, selected_hex_point_.y ); // TODO: PaintHex(..) calls updateHexCache(..)
	PaintHex( selected_hex_point_.x, selected_hex_point_.y );
	file_is_dirty_ = true;
}

void MapPane::addResourceMarker( void )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::SEA
		|| hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::RESTRICTEDWATERS
		|| hex_[selected_hex_point_.y][selected_hex_point_.x].terrain_ == HexType::ICINGSEA )
	{
		wxMessageBox( wxT("Your resource points would sink!"), Application::NAME );
		return;
	}

	ResourceMarker* respt = new ResourceMarker;
	respt->setSide( _current_player );

	//if ( Player == AXIS	)
	//	respt->setAxis( );
	//else
	//	respt->setAllied( );

	if ( ! respt->edit( ) ) // show editor dialog
	{
		delete respt;
		return;
	}

	respt->setHomeHex( selected_hex_point_.x, selected_hex_point_.y );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setOwner( _current_player );
	sit_[selected_hex_point_.y][selected_hex_point_.x].insertUnit( respt );
	contents_pane_->addUnit( respt );
	updateHexCache( selected_hex_point_.x, selected_hex_point_.y ); // TODO: PaintHex(..) calls updateHexCache(..)
	PaintHex( selected_hex_point_.x, selected_hex_point_.y );
	file_is_dirty_ = true;
}

void MapPane::insertUnit( int x, int y, Counter* unit )
{
	if ( ! unit )
	{
		wxMessageBox( wxT("No unit!"), wxT("MapPane::insertUnit") );
		return;
	}

	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
	{
		sprintf( msg, "Invalid hex!\n\ncur.x=%d\ncur.y=%d", x, y );
		wxMessageBox( wxS( msg ), wxT("MapPane::insertUnit") );
		return;
	}

	sit_[y][x].insertUnit( unit );
	updateHexCache( x, y );
	PaintHex( x, y );
	file_is_dirty_ = true;
}

void MapPane::removeUnit( int x, int y, Counter* unit )
{
	if ( ! unit )
	{
		wxMessageBox( wxT("No unit!"), wxT("MapPane::removeUnit") );
		return;
	}

	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
	{
		sprintf( msg, "Invalid hex!\n\ncur.x=%d\ncur.y=%d", x, y );
		wxMessageBox( wxS( msg ), wxT("MapPane::removeUnit") );
		return;
	}

	sit_[y][x].removeUnit( unit );
	// TODO: no updateHexCache(..) as with removeUnitFromCurrentHex(..) call?
	//	or to PaintHex(..)?
	file_is_dirty_ = true;
}

void MapPane::insertUnitToCurrentHex( Counter* unit )
{
	if ( ! isHexSelected( ) )
	{
		sprintf( msg, "No hex selected!\n\ncur.x=%d\ncur.y=%d", selected_hex_point_.x, selected_hex_point_.y );
		wxMessageBox( wxS( msg ), wxT("MapPane::insertUnitToCurrentHex") );
		return;
	}

	// TODO: why not just a pass-through call to insertUnit(..)?
	// sort out updateHexCache(..) and paintCurrentHex(..) discrepancies below
	// insertUnit( ptCurrentHex.x, ptCurrentHex.y, unit );

	if ( ! unit )
	{
		wxMessageBox( wxT("No unit!"), ( wxT("MapPane::insertUnitToCurrentHex") ) );
		return;
	}

	sit_[selected_hex_point_.y][selected_hex_point_.x].insertUnit( unit );
	// TODO: no updateHexCache(..) or PaintHex(..)?
	file_is_dirty_ = true;
}

void MapPane::removeUnitFromCurrentHex( Counter* unit )
{
	if ( ! isHexSelected( ) )
	{
		sprintf( msg, "No hex selected!\n\ncur.x=%d\ncur.y=%d", selected_hex_point_.x, selected_hex_point_.y );
		wxMessageBox( wxS( msg ), wxT("MapPane::removeUnitFromCurrentHex") );
		return;
	}

	// TODO: why not just a pass-through call to removeUnit(..)?
	// sort out updateHexCache(..) discrepancy below
	// removeUnit( ptCurrentHex.x, ptCurrentHex.y, unit );

	if ( ! unit )
	{
		wxMessageBox( wxT("No unit!"), wxT("MapPane::removeUnitFromCurrentHex") );
		return;
	}

	sit_[selected_hex_point_.y][selected_hex_point_.x].removeUnit( unit );
	updateHexCache( selected_hex_point_.x, selected_hex_point_.y );
	// TODO: no need to paintCurrentHex(..)?
	file_is_dirty_ = true;
}

// remove, delete and mark to log
void MapPane::DestroyUnit( Counter* u, bool record_losses )
{
	char s[200];

	u->getUnitString( s );

	// TODO: or removeUnit( ptCurrentHex.x, ptCurrentHex.y, u ); ?
	removeUnitFromCurrentHex( u );

	u->setMission( 0 );
	if ( _current_phase == Phase::COMBAT_PHASE )
	{
		sprintf( msg, "Unit destroyed:\n%s", s );
		wxMessageBox( wxS( msg ), Application::NAME );
	}
	if ( u->getCounterType( ) == Counter::Type::UNIT )  // land units to repl pool
	{
		GroundUnit* unit = (GroundUnit*)u;
		// log losses
		if ( record_losses )
		{
			if ( unit->getIsolated( ) )
				isol_losses_[_year % 100][_month][_turn][u->getSide( )] += u->getAtt( );
			else
			{
				unisol_losses_[_year % 100][_month][_turn][u->getSide( )] += u->getAtt( );
				// Track Soviet Special Replacement Points
				if ( unit->isSoviet( ) )
				{
					if (	unit->isArtillery( )
							|| unit->getType( ) == GroundUnitType::WW2::LT_AA
							|| unit->getType( ) == GroundUnitType::WW2::HV_AA
							|| unit->getType( ) == GroundUnitType::WW2::AT )
						// SovSpecialArtRP[Year%100][Month][Turn] += u->GetAtt();
						unisol_losses_[ ( _year + 60 ) % 100][_month][_turn][SidePlayer::ALLIED] += u->getAtt( );
					else if ( IsCm( u ) )
						// SovSpecialArmRP[Year%100][Month][Turn] += u->GetAtt();
						unisol_losses_[ ( _year + 50 ) % 100][_month][_turn][SidePlayer::ALLIED] += u->getAtt( );
					else
						// SovSpecialInfRP[Year%100][Month][Turn] += u->GetAtt();
						unisol_losses_[ ( _year + 40 ) % 100][_month][_turn][SidePlayer::ALLIED] += u->getAtt( );
				}
			}
		}
		// move to repl pool
		unit->setSupply( SupplyStatus::REG );
		unit->setSupplyTurns( 0 );
		unit->setIsolated( false );
		unit->clearFlag( GroundUnit::IS_CADRE );
		_repl_pool[u->getSide( )].add( unit );
	}
	else if ( u->getCounterType( ) == Counter::Type::PLANE )  // planes have their own repl pool
	{
		AirUnit* airunit = static_cast<AirUnit*>( u );
		airunit->setMission( AirUnit::Mission::NONE );
		airunit->setInAir( FALSE );
		airunit->setStaged( FALSE );
		airunit->setActive( TRUE );
		airunit->clearFlag( AirUnit::FOUGHT );
		_air_repl_pool[u->getSide( )].add( airunit );
	}
	else
	{  // others are just deleted
		if ( u->getCounterType( ) == Counter::Type::SHIP )
			wxMessageBox( wxT("Note: there is no replacement pool for naval units.\n"
					"You need to keep track of naval lossess manually."), Application::NAME, wxOK );
		delete u;
	}
}

// make cadre and mark to log
void MapPane::MakeCadre( Counter* c )
{
	if ( c->getCounterType( ) != Counter::Type::UNIT )
	{
		wxMessageBox( wxT("Can't make cadre (not a land unit!)") );
		return;
	}

	GroundUnit* u = static_cast<GroundUnit*>( c );
	if ( ! u->getFlag( GroundUnit::HAS_CADRE ) || u->getFlag( GroundUnit::IS_CADRE ) )
	{
		wxMessageBox( wxT("Can't make cadre") );
		return;
	}

	int str = u->getAtt( );

	char s[200];
	u->getUnitString( s );
	sprintf( msg, "Unit reduced to cadre:\n%s", s );
	wxMessageBox( wxS( msg ), Application::NAME );

	u->setFlag( GroundUnit::IS_CADRE );
	if ( u->getIsolated( ) )
		isol_losses_[_year % 100][_month][_turn][u->getSide( )] += str - u->getAtt( );
	else
		unisol_losses_[_year % 100][_month][_turn][u->getSide( )] += str - u->getAtt( );

	// Track Soviet Special Replacement Points
	if ( u->isSoviet( ) )
	{
		if ( u->isArtillery( ) || u->getType( ) == GroundUnitType::WW2::LT_AA || u->getType( ) == GroundUnitType::WW2::HV_AA || u->getType( ) == GroundUnitType::WW2::AT )
			//                SovSpecialArtRP[Year%100][Month][Turn] += str - u->GetAtt();
			unisol_losses_[ ( _year + 60 ) % 100][_month][_turn][SidePlayer::ALLIED] += str - u->getAtt( );
		else if ( IsCm( c ) )
			//                SovSpecialArmRP[Year%100][Month][Turn] += str - u->GetAtt();
			unisol_losses_[ ( _year + 50 ) % 100][_month][_turn][SidePlayer::ALLIED] += str - u->getAtt( );
		else
			//                SovSpecialInfRP[Year%100][Month][Turn] += str - u->GetAtt();
			unisol_losses_[ ( _year + 40 ) % 100][_month][_turn][SidePlayer::ALLIED] += str - u->getAtt( );
	}
}

// eliminate all units in hex
// TODO: not used anywhere, regardless address the naked delete
int MapPane::EliminateUnits( int x, int y )  // TODO: leave permanent airfields
{
	int c = 0;
	Counter* tmp;
	Counter* unit = sit_[y][x].unit_list_;
	while ( unit )
	{
		tmp = unit->getNext( );
		delete unit;
		unit = tmp;
		++c;
	}
	sit_[y][x].unit_list_ = nullptr;
	PaintHex( x, y );
	return c;
}

void MapPane::RmCMarkers( )
{
	if ( wxMessageBox( wxT("Really remove ALL air and ground combat reports?"), Application::NAME, wxYES_NO ) != wxYES )
		return;

	Counter* del_list[20];  // max 20 reports/hex

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
		{
			// find all units to be deleted (the can't be deleted here
			// or ENUM_HEXUNITS will fail):
			int i = 0;
			Counter* c;
			ENUM_HEXUNITS(x, y, c)
			{
				if ( c->getCounterType( ) == Counter::Type::NOTE
					&& ( ( (HexNote*)c )->getNoteType( ) == HexNote::GNDREPORT
						|| ( (HexNote*)c )->getNoteType( ) == HexNote::AIRREPORT
						|| ( (HexNote*)c )->getNoteType( ) == HexNote::BOMBREPORT
						|| ( (HexNote*)c )->getNoteType( ) == HexNote::DIEROLLREPORT
						|| ( (HexNote*)c )->getNoteType( ) == HexNote::NAVREPORT ) )
				{
					del_list[i++] = c;
				}
			}
			// delete found reports:
			for ( int j = 0; j < i; ++j )
			{
				if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
					contents_pane_->removeUnit( del_list[j] );
				removeUnit( x, y, del_list[j] );
				delete del_list[j];
			}
		}
	Refresh( );
	contents_pane_->Refresh( );
}

// destroy/cadre attacker unit(s) to satisfy combat losses
// NOTE: record_losses==0 if moving to repl.pool
void MapPane::DestroySelUnits( bool record_losses /* = true */ )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	Counter* cntr = _selected_unit_list;
	while ( cntr )
	{
		Counter* next = cntr->getNext( );

		contents_pane_->deselectUnit( cntr );

		if ( cntr->getCounterType( ) == Counter::Type::HIT )
		{
			wxMessageBox( wxT("You can't destroy hit markers. Markers will \ndisappear when you repair the damage."), Application::NAME );
			cntr = next;
			continue;
		}

		char s[200];
		cntr->getUnitString( s );
		if ( record_losses )
		{
			if ( ( (GroundUnit*)cntr )->getFlag( GroundUnit::HAS_CADRE ) && ( (GroundUnit*)cntr )->getFlag( GroundUnit::IS_CADRE ) == 0 )
				sprintf( msg, "Really reduce to cadre this unit:\n%s", s );
			else
				sprintf( msg, "Really destroy and move to replacement pool this unit:\n%s", s );
		}
		else
			sprintf( msg, "Really move to replacement pool (without recording losses):\n%s", s );

		if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
		{
			cntr = next;
			continue;
		}

		if ( ! record_losses )
		{
			contents_pane_->removeUnit( cntr );
			DestroyUnit( cntr, false ); // don't record losses
		}
		else if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getFlag( GroundUnit::HAS_CADRE )
			&& ( (GroundUnit*)cntr )->getFlag( GroundUnit::IS_CADRE ) == 0 )
			MakeCadre( cntr );
		else
		{
			contents_pane_->removeUnit( cntr );
			DestroyUnit( cntr, record_losses );
		}
		cntr = next;
	}
	PaintHex( x, y );
	contents_pane_->Refresh( );
}

void MapPane::SetDisruption( int disr )
{
	if ( disr == DisruptionStatus::UNDISRUPT_ALL )
	{
		if ( wxMessageBox( wxT("Really remove disruption from ALL units?"), Application::NAME, wxYES_NO ) != wxYES )
			return;

		for ( int y = 0; y < world_y_size_; ++y )
			for ( int x = 0; x < world_x_size_; ++x )
			{
				Counter* c;
				ENUM_HEXUNITS(x, y, c)
				{
					if ( c->getCounterType( ) == Counter::Type::UNIT )
						( (GroundUnit*)c )->setDisruption( DisruptionStatus::NOT_DISRUPTED );
				}
			}

		rePaint( );
		return;
	}

	if ( ! _selected_unit_list )
	{
		wxMessageBox( wxT("Select units first!"), Application::NAME );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT )
			( (GroundUnit*)c )->setDisruption( disr );
	}

	contents_pane_->Refresh( );
}

// toggle unit's attack supply status on/off
void MapPane::ToggleAttackSup( )
{
	Counter* c;
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) != Counter::Type::UNIT )
			continue;

		GroundUnit* u = (GroundUnit*)c;
		if ( u->getSide( ) == _phasing_player && _current_player == _phasing_player )
			u->setAttackSupply( ! u->getAttackSupply( ) );
	}
	contents_pane_->Refresh( );
}

void MapPane::FlipAirCounter( )
{
	Counter* c;

	if ( !_selected_unit_list )
	{
		SelUnitErr( );
		return;
	}
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) != Counter::Type::PLANE )
		{
			wxMessageBox( wxT("Only air groups can be made active/deactive") );
			break;
		}
		if ( c->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("You can only active/deactive your own air groups") );
			break;
		}
		( (AirUnit*)c )->setActive( ( (AirUnit*)c )->isActive( ) ? FALSE : TRUE );
	}
	contents_pane_->Refresh( );
}

void MapPane::CancelOrder( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}
	Counter* c;
	ENUM_SELUNITS(c)
	{
		// TODO: DLT logic check
		if ( c->getCounterType( ) == Counter::Type::PLANE )
			( (AirUnit*)c )->setNavalPatrol( FALSE );
		else
		{
			if ( c->getCounterType( ) == Counter::Type::SHIP )
				( (NavalUnit*)c )->setAttackDir( 0 );
			else
			{
				if ( c->getCounterType( ) != Counter::Type::UNIT )
					continue;
				GroundUnit* u = (GroundUnit*)c;
				u->setAttackDir( 0 );
				u->setMission( 0 );
			}
		}
	}
	contents_pane_->Refresh( );
}

void MapPane::ToggleDidPA( void )
{
	if ( ! _selected_unit_list )
	{
		wxMessageBox( wxT("Select some air units first"), Application::NAME );
		return;
	}

	// check everything
	Counter* cntr;
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE )
		{
			AirUnit* a = (AirUnit*)cntr;
			if ( a->getType( ) != AirUnitType::F && a->getType( ) != AirUnitType::HF )
				wxMessageBox( wxT("Setting 'has patrol attacked' only makes sense for F's or HF's"), Application::NAME );
			else
				a->setDidPA( ! a->getDidPA( ) );
		}
	}
	contents_pane_->Refresh( );
}

// set RE sizes to default values
void MapPane::SetAllREs( void )
{
	if ( wxMessageBox( wxT("This will set RE sizes of all units to the values\n"
			"defined in Options->Rules->General. If you have\n"
			"defined custom RE sizes for any units you will\n"
			"lose that information.\n"
			"\n"
			"Do you really want to set RE sizes of ALL units\n"
			"to their default values?"), Application::NAME, wxYES_NO | wxICON_QUESTION  ) != wxYES )
		return;

	Counter* c;
	ENUM_SELUNITS(c)
		if ( c->getCounterType( ) == Counter::Type::UNIT )
			( (GroundUnit*)c )->setRE( ( (GroundUnit*)c )->getDefaultRE( ) );

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			ENUM_HEXUNITS(x, y, c)
				if ( c->getCounterType( ) == Counter::Type::UNIT )
					( (GroundUnit*)c )->setRE( ( (GroundUnit*)c )->getDefaultRE( ) );
}

// set ZOCs to default values
void MapPane::SetAllZOCs( void )
{
	if ( wxMessageBox( wxT("This will set ZOC settings of all units to the value\n"
			"that has been defined in Options->Rules->General.\n"
			"If you have defined custom ZOC settings for any units\n"
			"you will lose that information.\n"
			"\n"
			"Do you really want to set ZOC's of ALL units\n"
			"to their default values?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
		return;

	Counter* c;
	ENUM_SELUNITS(c)
		if ( c->getCounterType( ) == Counter::Type::UNIT )
			( (GroundUnit*)c )->setDefaultZOC( );

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			ENUM_HEXUNITS(x, y, c)
				if ( c->getCounterType( ) == Counter::Type::UNIT )
					( (GroundUnit*)c )->setDefaultZOC( );
}

// toggle side of selected units
void MapPane::SetUnitSide( )
{
	if ( ! isHexSelected( ) || ! _selected_unit_list )
	{
		wxMessageBox( wxT("Select some units first"), Application::NAME );
		return;
	}

	int new_nat, old_nat, old_side, new_side;
	char s[100];

	Counter* c;
	ENUM_SELUNITS(c)
	{
		switch ( c->getCounterType( ) )
		{
			case Counter::Type::UNIT:  // GND
				old_side = c->getSide( );
				new_side = ( old_side == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );
				( (GroundUnit*)c )->setSide( new_side );

				old_nat = ( (GroundUnit*)c )->getSubType( );
				new_nat = -1;
				for ( int i = 0; i < SIDEMAP_SIZE; ++i )
					if ( _unit_subtype_side_map[i][old_side] == old_nat )
						new_nat = _unit_subtype_side_map[i][new_side];

				if ( new_nat < 0 )
				{
					sprintf( msg, "There is no corresponding %s nationality/group for %s",
							new_side == SidePlayer::AXIS ? "axis" : "allied", _ground_armed_force[old_side][old_nat].longname_ );
					wxMessageBox( wxS( msg ), Application::NAME );
					continue;
				}
				( (GroundUnit*)c )->setSubType( new_nat );

				sit_[selected_hex_point_.y][selected_hex_point_.x].setOwner( new_side );
				c->getUnitString( s );
				sprintf( msg, "%s  is now %s unit", s, new_side == SidePlayer::AXIS ? "axis" : "allied" );
				wxMessageBox( wxS( msg ), Application::NAME );
				break;
			case Counter::Type::PLANE:  // AIR
				old_side = c->getSide( );
				new_side = ( old_side == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );
				( (AirUnit*)c )->setSide( new_side );

				old_nat = ( (AirUnit*)c )->getSubType( );
				new_nat = -1;
				for ( int i = 0; i < AIRSIDEMAP_SIZE; ++i )
					if ( _airunit_subtype_side_map[i][old_side] == old_nat )
						new_nat = _airunit_subtype_side_map[i][new_side];

				if ( new_nat < 0 )
				{
					sprintf( msg, "There is no corresponding %s nationality/group for %s",
							new_side == SidePlayer::AXIS ? "axis" : "allied", _air_armed_force[old_side][old_nat].name_ );
					wxMessageBox( wxS( msg ), Application::NAME );
					continue;
				}
				( (AirUnit*)c )->setSubType( new_nat );

				// don't switch hex ownership if the air unit is in flight
				if ( ! ( ( (AirUnit*)c )->getInAir( )) )
					sit_[selected_hex_point_.y][selected_hex_point_.x].setOwner( new_side );
				c->getUnitString( s );
				sprintf( msg, "%s  is now %s unit", s, new_side == SidePlayer::AXIS ? "axis" : "allied" );
				wxMessageBox( wxS( msg ), Application::NAME );
				break;
			case Counter::Type::SHIP:  // SHIP
				old_side = c->getSide( );
				new_side = ( old_side == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );
				( (NavalUnit*)c )->setSide( new_side );

				old_nat = ( (NavalUnit*)c )->getSubType( );
				new_nat = -1;
				for ( int i = 0; i < SHIPSIDEMAP_SIZE; ++i )
					if ( _navalunit_subtype_side_map[i][old_side] == old_nat ) new_nat = _navalunit_subtype_side_map[i][new_side];
				if ( new_nat < 0 )
				{
					sprintf( msg, "There is no corresponding %s nationality/group for %s",
							new_side == SidePlayer::AXIS ? "axis" : "allied", _naval_armed_force[old_side][old_nat].name_ );
					wxMessageBox( wxS( msg ), Application::NAME );
					continue;
				}
				( (NavalUnit*)c )->setSubType( new_nat );

				c->getUnitString( s );
				sprintf( msg, "%s  is now %s unit", s, new_side == SidePlayer::AXIS ? "axis" : "allied" );
				wxMessageBox( wxS( msg ), Application::NAME );
				break;
		}  // switch
	}
	paintCurrentHex( );
}

// set side of all units belonging to subtype
void MapPane::SetNationSide( int new_side )
{
	int old_side = ( new_side == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );

	wxString ws[SIDEMAP_SIZE];
	for ( int i = 0; i < SIDEMAP_SIZE; ++i )
		ws[i] = wxString::FromAscii( _ground_armed_force[old_side][_unit_subtype_side_map[i][old_side]].longname_ );

	int idx = wxGetSingleChoiceIndex( wxT("Select nationality"), Application::NAME, SIDEMAP_SIZE, ws, this );
	if ( idx < 0 )
		return;

	int old_nat = _unit_subtype_side_map[idx][old_side];
	int new_nat = -1;
	for ( int i = 0; i < SIDEMAP_SIZE; ++i )
		if ( _unit_subtype_side_map[i][old_side] == old_nat )
			new_nat = _unit_subtype_side_map[i][new_side];

	//new_nat = sidemap[old_nat][new_side];
	if ( new_nat < 0 )
	{
		wxMessageBox( wxT("Bug: no corresponding nationality found"), Application::NAME );
		return;
	}
#if 0
	sprintf( msg, "new subtype='%s'", SubTypes[new_side][new_nat].achFullName );
	wxMessageBox( msg );
#endif
	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
		{
			Counter* c;
			ENUM_HEXUNITS(x, y, c)
			{
				// LAND
				if ( c->getCounterType( ) == Counter::Type::UNIT && c->getSide( ) == old_side
					&& ( (GroundUnit*)c )->getSubType( ) == old_nat )
				{
					( (GroundUnit*)c )->setSubType( new_nat );
					( (GroundUnit*)c )->setSide( new_side );
					sit_[y][x].setOwner( new_nat );
				}
				// AIR
				// FIXME: different old_nat
				else if ( c->getCounterType( ) == Counter::Type::PLANE && c->getSide( ) == old_side
					&& ( (AirUnit*)c )->getSubType( ) == old_nat )
				{
					( (AirUnit*)c )->setSubType( new_nat );
					( (AirUnit*)c )->setSide( new_side );
					if ( ! ( ( (AirUnit*)c )->getInAir( ) ) )
						sit_[y][x].setOwner( new_nat );
				}
				// SHIP
				// ...
			}
		}
	Refresh( );
}

void MapPane::CmUnitView( )
{
	char unitmsg[10240];
	strcpy( unitmsg, "" );

	char tmp[1000];
	Counter* unit = _selected_unit_list;
	while ( unit != nullptr )
	{
		unit->getUnitString( tmp, true ); // verbose on
		strcat( unitmsg, tmp );
		strcat( unitmsg, "\n" );
		unit = unit->getNext( );
	}
	wxMessageBox( wxS( unitmsg ), wxT("Selected counters:") );
}

bool MapPane::IsOwnUnit( Counter* unit )
{
	if ( _current_player == SidePlayer::ALLIED && unit->isAxis( ) )
		return false;

	if ( _current_player == SidePlayer::AXIS && unit->isAllied( ) )
		return false;

	return true;
}

// Variant A River Flotilla movement
bool MapPane::IsPartisanUnit( Counter* cp )
{
	if ( cp->getCounterType( ) == Counter::Type::UNIT && static_cast<GroundUnit*>( cp )->getType( ) == GroundUnitType::WW2::PARTISAN )
		return true;

	return false;
}

bool MapPane::IsGermanUnit( Counter* cp )
{
	if ( cp->getCounterType( ) == Counter::Type::UNIT && static_cast<GroundUnit*>( cp )->isGerman( ) )
		return true;

	return false;
}

bool MapPane::IsCm( Counter* cp )
{
	if ( cp->getCounterType( ) != Counter::Type::UNIT )
		return false;

	GroundUnit* unit = static_cast<GroundUnit*>( cp );

	// UC_COMBATMOT class 			||	// UF_MOTORIZED flag
	if ( unit->isCombatMotClass( )  || unit->isSupplementalMotorized( ) )
		return true;

	return false;
}

#endif
