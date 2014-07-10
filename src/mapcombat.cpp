#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

#include <algorithm>
using std::min;
using std::max;

#include <wx/wx.h>

#include "hextools.h"
using ht::wxS;
using ht::rollDiceN;
using ht::crt;
using ht::crt2str;
using ht::calculateOptimalAEC;
using ht::aeca2mod;
using ht::aecd2mod;
using ht::atec2mod;
using ht::oppositeDir;
using ht::dir2replayCombat;
using ht::coordinatesOutOfBounds;

#include "application.h"
#include "dirs.loop"
#include "selectedunits.loop"
#include "hexunits.loop"
#include "phase.h"
#include "weathercondition.h"
#include "weatherclass.h"
#include "dasmode.h"
#include "sideplayer.h"
#include "rulesvariant.h"
#include "rules.h"
#include "city.h"
#include "hexcontents.h"
#include "counter.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "armorantitank.h"
#include "groundunitsubtype.h"
#include "groundunit.h"
#include "airunit.h"
#include "navalunit.h"
#include "facility.h"
#include "statusmarker.h"
#include "hexnote.h"
#include "hexcontentspane.h"
#include "combatresult.h"
#include "dgroundcombatdice.h"

extern char msg[10000];
extern const char* _month_string[12];
extern const char* _turn_numeral[20];
extern Counter* _selected_unit_list;					// defined in HexContentsPane.cpp
extern int _current_player;
extern int _phasing_player;
extern int _current_phase;								// COMBAT_PHASE etc.
extern int _weather_condition[];						// WeatherCondition::MUD etc., per zone (frame.cpp)
extern int _rivers_lakes_frozen[];						// true if rivers/lakes are frozen (frame.cpp)
extern int _year;
extern int _month;
extern int _turn;
extern int _show_combat_markers;

extern Rules _rule_set;

extern HexType _hex_types[];//[TERRAINTYPECOUNT];  // terrain types

extern DieDlgXfer DieDlgData;

// show info about combat that will take place.
void MapPane::CombatInfo( int x, int y )
{
	ResolveCombat( true, x, y ); // boolean flag to just show combat info
}

// show ground combat dialog box and do the fighting
// return combat result (DE, etc) or -1 if canceled
int MapPane::ResolveCombat( bool just_show_combat_info, int x, int y, bool overrun )
{
	// prior to v2.2, tricky user could get combat reports, but no more
	if ( ! _show_combat_markers )
	{
		wxMessageBox( wxT("Combat markers are hidden, so this command is disabled.\n\n"
						"Select 'View->Show Combat Markers' to unhide combat markers."), Application::NAME );
		return -1;
	}

	// if given location is invalid, try to use currently-selected location
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
	{
		x = selected_hex_point_.x;
		y = selected_hex_point_.y;
		if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		{
			wxMessageBox( wxT( "Select target hex first!" ), Application::NAME );
			return -1;
		}
	}

	if ( ! sit_[y][x].isTarget( ) )
	{
		wxMessageBox( wxT("Select hex that is being attacked first!\n\n"
						"You need to order your units to attack before you can\n"
						"resolve combat. Do this by moving your units towards\n"
						"the desired target hex from any adjacent hex."), Application::NAME );
		return -1;
	}

	contents_pane_->deselectAll( );
	float def = CalcDefStr( x, y );
	float att = 0;

	float ngs = 0;
	float dngs = 0;
	int x2, y2;
	int d;
	ENUM_DIRS(d)
	{
		ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ );
		att += CalcAttStr( x2, y2, oppositeDir( d ) );
		ngs += CalcNgsStr( x2, y2, oppositeDir( d ) );  // Allow ships to attack in combat phase
	}

	// TODO: and, rough seas should halve NGS, true?

	float gs = CalcGS( x, y );
	float das = CalcDAS( x, y );

	// TODO: this should be for WINTER as well
	if ( _weather_condition[hex_[y][x].getWeatherZone( )] == WeatherCondition::MUD || _weather_condition[hex_[y][x].getWeatherZone( )] == WeatherCondition::SNOW )
	{
		gs /= 2;
		das /= 2;
	}

	float plus_att = static_cast<float>( GetAttSupport( x, y ) );
	float plus_def = static_cast<float>( GetDefSupport( x, y ) );

	// Get*Support(..) adds points-of-attack/defense status markers
	float tot_att = att + gs + ngs + plus_att;
	float tot_def = def + das + dngs + plus_def;

	/* don't divide by zero (note: 0 def -> no DAS allowed)
	 * TODO: believe this is an optional rule
	 * w/o the optional rule, DAS can contribute to 0-str
	 * units, and should use tot_def vice def: */
	float odds = def < 0.01 ? 9 : tot_att / tot_def;

	char mod_explain[500];

	// max the modifier to ensure auto DE when odds
	// are 9:1 due to zero defense strength
	int mod = def < 0.01 ? 99 : GetMods( x, y, mod_explain );

	// FitE/SE German first winter effects
	mod += GermanFirstWinterMod( x, y, mod_explain );
	mod += CalcWinterization( x, y, mod_explain );

	int aeca = CalcNominalAECA( x, y );  // weather modified later; need nominal AECA now
	int aecd = ModifyAEC( x, y, sit_[y][x].getAECD( _selected_unit_list ) );
	int atec = sit_[y][x].getATEC( _selected_unit_list  );  // ATEC works in any weather

	int i;
	char tmps[255];
	strcpy( tmps, "" );

	// apply AECD or ATEC to the net modifier
	if ( aeca == ArmorAntitank::Proportion::FULL || aeca == ArmorAntitank::Proportion::HALF )  // can use ATEC
	{
		// tricky: smaller value is better, but zero means no AEC...
		if ( atec && ( atec <= aecd || aecd == ArmorAntitank::Proportion::NONE ) )
		{
			i = atec2mod( atec );
			if ( i )
			{
				mod -= i;
				sprintf( tmps, ", ATEC: -%d", i );
			}
		}
		else  // TODO: if (aecd > AEC_NONE) [short-circuits superfluous call to aecd2mod] ?
		{
			i = aecd2mod( aecd );
			if ( i )
			{
				mod -= i;
				sprintf( tmps, ", AECD: -%d", i );
			}
		}
	}
	else  // TODO: if (aecd > AEC_NONE) [short-circuits superfluous call to aecd2mod] ?
	{
		i = aecd2mod( aecd );
		if ( i )
		{
			mod -= i;
			sprintf( tmps, ", AECD: -%d", i );
		}
	}
	strcat( mod_explain, tmps );

	/* modify AECA for weather and terrain importantly,
	 * this function call occurs after AECD or ATEC have been
	 * calculated, because although weather may defeat AECA,
	 * ATEC will still be calculated if the attacker is
	 * >capable< of AECA
	 */
	// TODO: if ( aeca > AEC_NONE ) [short-circuits when three is no AECA to modify] ?
	aeca = ModifyAEC( x, y, aeca );

	// apply AECA to the net modifier
	i = aeca2mod( aeca );
	if ( i )
	{
		mod += i;
		sprintf( tmps, ", AECA: +%d", i );
		strcat( mod_explain, tmps );
	}

	// apply combat engineering to the net modifier
	// TODO: make sure defenders can get this negative modifier, too
	i = CalcEngMod( x, y );
	if ( i )
	{
		mod += i;
		sprintf( tmps, ", Cmbt Engs: +%d", i );
		strcat( mod_explain, tmps );
	}

	// TODO: apply adverse terrain expertise to the net modifier
	// both positive for attacker, and negative for defender

	// check that there are not too many artillery unit attacking
	float art_re, norm_re;
	CalcAttArtREs( x, y, &art_re, &norm_re );

	// TODO: handle this for real, not just a warning
	if ( art_re > norm_re + 0.01 ) /* damn floating point numbers... */
	{
		sprintf( msg, "WARNING:\n"
				"There are %.1f artillery REs attacking but only %.1f non-artillery REs attacking,\n"
				"pre-calculated attack strength is incorrect (see rule 14B).",
				art_re, norm_re );
		wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
	}

	// "Combat info" command selected
	if ( just_show_combat_info )
	{
		sprintf( msg, "Attacker strength:\t%.2f\n"
				"Air support:\t%.2f\n"
				"NGS:\t\t%.2f\n"
				"Other support:\t%.2f\n"
				"Total:\t\t%.2f\n\n"
				"Defender strength:\t%.2f\n"
				"Air support:\t%.2f\n"
				"NGS:\t\t%.2f\n"
				"Other support:\t%.2f\n"
				"Total:\t\t%.2f\n\n"
				"Ratio: %.2f\n"
				"Die modifier: %s%d (%s)\n"
				"Possible results:\n1: %s\n2: %s\n3: %s\n4: %s\n5: %s\n6: %s",
				att,
				gs,
				ngs,
				plus_att,
				tot_att,
				def,
				das,
				dngs,
				plus_def,
				tot_def,
				odds,
				mod > 0 ? "+" : "", mod, mod_explain,
				crt2str( crt( odds, 1 + mod ) ), crt2str( crt( odds, 2 + mod ) ), crt2str( crt( odds, 3 + mod ) ), crt2str( crt( odds, 4 + mod ) ), crt2str( crt( odds, 5 + mod ) ), crt2str( crt( odds, 6 + mod ) ) );

		wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
		return -1;
	}

	if ( _current_player != _phasing_player )
	{
		wxMessageBox( wxT("Please switch back to phasing player before resolving combat"), Application::NAME );
		return -1;
	}

	// setup
	sprintf( DieDlgData.EditAtt, "%.2f", att );
	sprintf( DieDlgData.EditGS, "%.2f", gs );
	sprintf( DieDlgData.EditLR, "%.2f", plus_att );
	sprintf( DieDlgData.EditDef, "%.2f", def );
	sprintf( DieDlgData.EditDAS, "%.2f", das );
	sprintf( DieDlgData.EditMod, "%d", mod );
	sprintf( DieDlgData.EditOtherDef, "%.2f", plus_def );

	DGroundCombatDice dlg( this );
	if ( dlg.ShowModal( ) != wxID_OK )
		return -1;
	dlg.CloseDialog( );  // kludgemaster 5000

	int die;
	if ( DieDlgData.RdComputer )
		die = rollDiceN( 1 );
	else
	{
		die = atoi( DieDlgData.EditDie );
		if ( die < 1 || die > 6 )
		{
			wxMessageBox( wxT("Illegal die roll (must be between 1 and 6),\nplease try again!") );
			return -1;
		}
	}

	odds = dlg.GetOdds( );
	mod = dlg.GetMod( );

#if 0
	// Variant A if commando involved in attack then a chance of surprise attack
	if ( _rule_set.rules_variant_ == RulesVariant::A && CommandoUnitAttacking( x, y ) )
	{
		int commando_roll = rollDiceN( 1 );
		if ( commando_roll >= 5 )
		{
			mod += 1;
			sprintf( msg, "Commando Surprise Successful +1: (die roll = %d)", commando_roll );
		}
		else
			sprintf( msg, "No Commando Surprise: (die roll = %d)", commando_roll );

		wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
	}
#endif

	int result = crt( odds, die + mod );
	sprintf( msg, "Die roll: %d + %d\nCombat result: %s", die, mod, crt2str( result ) );
	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );

	// add combat report to the hex
	sprintf( msg, "Turn: %s %s %d\n"
			"%s attacked with %.2f points__(Units: %.2f, GS: %.2f, other: %.2f)\n"
			"%s defended with %.2f points__(Units: %.2f, DAS: %.2f, other: %.2f)\n\n"
			"Odds: %.2f, die modifier %d\n\n"
			"Die roll: %d\n"
			"Result: %s",
			_month_string[_month], _turn_numeral[_turn - 1], _year,
			_current_player == SidePlayer::AXIS ? "Axis" : "Allied", att + gs + plus_att, att, gs, plus_att,
			_current_player == SidePlayer::AXIS ? "Allied" : "Axis", def + das + plus_def, def, das, plus_def,
			odds, mod,
			die,
			crt2str( result ) );

	HexNote* combat_report = new HexNote( HexNote::GNDREPORT );
	combat_report->setCaption( overrun ? "Overrun report" : "Combat report" );
	combat_report->setText( msg );

	insertUnit( x, y, combat_report );

	CommitLosses( result );  // and retreat (if necessary)

	// remove GS and DAS missions from planes
	Counter* cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE )
		{
			AirUnit* plane = (AirUnit*)cntr;
			if ( plane->getMission( ) == AirUnit::Mission::BOMB_GS )
				plane->setMission( AirUnit::Mission::NONE );
			else if ( plane->getMission( ) == AirUnit::Mission::BOMB_DAS )
			{
				plane->setWasDAS( TRUE );  // TODO: what does this do?
				plane->setMission( AirUnit::Mission::NONE );
			}
		}
		cntr = cntr->getNext( );
	}

	// any other pending missions?
	// TODO: like what?  figure this out and/or explain it better
	cntr = sit_[y][x].unit_list_;
	sit_[y][x].setAirTarget( false );
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)cntr )->getMission( ) != AirUnit::Mission::NONE )
			sit_[y][x].setAirTarget( true );

		cntr = cntr->getNext( );
	}

	// TODO: this seems like common/routine code that can be re-used
	PaintHex( x, y );
	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
	{
		contents_pane_->clear( );
		contents_pane_->addUnits( sit_[y][x].unit_list_ );
		sit_[y][x].setOwner( _current_player );
	}

	sit_[y][x].setTarget( false );

	return result;
}

// advance after combat
void MapPane::Advance( bool WXUNUSED(isoverrrun) )
{
	if ( _current_player != _phasing_player )  // TODO: check this
		return;

	if ( ! _selected_unit_list )
	{
		wxMessageBox( wxT("First select units that you wish to advance.") );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	Counter* next = NULL;
	Counter* cntr = _selected_unit_list;
	while ( cntr )
	{
		//int dir;
		next = cntr->getNext( );
		if ( cntr->getCounterType( ) != Counter::Type::UNIT )
		{
			cntr = next;
			continue;
		}
		else if ( cntr->getAttackDir( ) != 0 )
		{
			int dir = cntr->getAttackDir( );

			int nx, ny;
			ht::getAdjHexCoords( dir, x, y, &nx, &ny, world_x_size_, world_y_size_ );
			if ( IsEnemyOccupied( nx, ny ) ) // target hex not yet conquered
			{
				wxMessageBox( wxT("Target hex is still enemy occupied!") );
				cntr = next;
				continue;
			}

			// RR unit can only advance via RR
			// TODO: analyze whether to check RR obstruction here
			// TODO: also should check whether RR hit in either from- or to-hex !?
			if ( ( ( (GroundUnit*)cntr )->getFlag( GroundUnit::RAILONLY ) )
				&& ( ! IsRailRoad( selected_hex_point_.x, selected_hex_point_.y, dir ) || sit_[y][x].getRRHit( ) ) )
			{
				wxMessageBox( wxT("Rail-only units can only advance via railroad") );
				cntr = next;
				continue;
			}

			cntr->recordReplay( dir2replayCombat( dir ) );

			if ( _current_phase == Phase::REACT_COMB_PHASE )
				sit_[ny][nx].setOwner( _current_player == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS, true );
			else
				sit_[ny][nx].setOwner( _current_player, true );
			updateHitMarkers( nx, ny );	// for rail obstruction sync

			contents_pane_->deselectUnit( cntr );
			contents_pane_->removeUnit( cntr );

			removeUnit( x, y, cntr );
			insertUnit( nx, ny, cntr );

			//if ( isoverrun )
			//	UnitWin->SelectUnit( cntr );
			contents_pane_->Refresh( );
			cntr->setAttackDir( 0 );

			// FIXME
			// TODO: this appears to override above setOwner when in Reaction Combat Phase
			sit_[ny][nx].setOwner( _current_player, true );
			updateHitMarkers( nx, ny );	// for rail obstruction sync

			PaintHex( nx, ny );
			cntr = next;
		}
	}
	PaintHex( x, y );
}

void MapPane::CmCancelattack( )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) || ! sit_[y][x].isTarget( ) )
		return;

	if ( wxMessageBox( wxT("Do you really wish to cancel this attack?"), Application::NAME, wxYES_NO ) != wxYES )
		return;

	int x2, y2;

	ht::getAdjHexCoords( Hex::WEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
	CancelAttackOrders( x2, y2, Hex::EAST );

	ht::getAdjHexCoords( Hex::NORTHWEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
	CancelAttackOrders( x2, y2, Hex::SOUTHEAST );

	ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
	CancelAttackOrders( x2, y2, Hex::NORTHEAST );

	ht::getAdjHexCoords( Hex::EAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
	CancelAttackOrders( x2, y2, Hex::WEST );

	ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
	CancelAttackOrders( x2, y2, Hex::NORTHWEST );

	ht::getAdjHexCoords( Hex::NORTHEAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
	CancelAttackOrders( x2, y2, Hex::SOUTHWEST );

	sit_[y][x].setTarget( false );
	PaintHex( x, y );
}

// cancel orders for units at 'x','y' attacking 'dir'
void MapPane::CancelAttackOrders( int x, int y, int dir )
{
	Counter* cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && cntr->getAttackDir( ) == dir )
			cntr->setAttackDir( 0 );
		cntr = cntr->getNext( );
	}
}

// return possible retreat direction for given _enemy_ unit
int MapPane::GetRetreatDir( Counter* c )
{
	int ok_dirs[6] = { 0, 0, 0, 0, 0, 0 };  // 0 = can't go, 1 = ZOC, 2 = ok
	int dir_rq = 0;  // 0, 1 or 2: best available value in ok_dirs

	for ( int i = 0; i < 6; ++i )
	{
		int dir = 1 << i;
		int x, y;
		ht::getAdjHexCoords( dir, selected_hex_point_.x, selected_hex_point_.y, &x, &y, world_x_size_, world_y_size_ );
		bool own = ( _current_phase == Phase::REACT_COMB_PHASE ) ? IsEnemyOccupied( x, y ) : IsOwnOccupied( x, y );
		bool enemy = ( _current_phase == Phase::REACT_COMB_PHASE ) ? IsOwnOccupied( x, y ) : IsEnemyOccupied( x, y );

		if ( ! own )  // not enemy owned, can go but check further...
		{
			// I (DTM) think this is a defect, should be x, y, dir, correct?
			//	see same defect in following else if block
			if ( IsProhibited( selected_hex_point_.x, selected_hex_point_.y, dir ) )
				continue;
			else if (	c->getCounterType( ) == Counter::Type::UNIT
					 && static_cast<GroundUnit*>( c )->getFlag( GroundUnit::RAILONLY )
					 && ( ! IsRailRoad( selected_hex_point_.x, selected_hex_point_.y, dir )
					   || sit_[y][x].getRRHit( )
					   || sit_[y][x].isRailObstructed( ) ) )
				continue; // RR-only unit w/o a RR retreat route, or RR retreat route is broken/obstructed
			else
			{
				dir_rq = max( dir_rq, 1 );  // others may retreat anywhere
				ok_dirs[i] = 1;
				int own_zoc = ( _current_phase == Phase::REACT_COMB_PHASE ) ? isHexInEnemyZOC( x, y ) : isHexInFriendlyZOC( x, y );

				if ( ! own_zoc ) // no enemies nor zoc, good place to go
				{
					++ok_dirs[i]; // i.e. 2
					dir_rq = 2;
				}
				else if ( ( c->getCounterType( ) == Counter::Type::UNIT ) // ZOC: only German c/m may retreat there
						&& ( (GroundUnit*)c )->isGerman( )
						&& ( (GroundUnit*)c )->isSupplementalMotorized( )
						&& enemy )
				{
					++ok_dirs[i]; // i.e. 2
					dir_rq = 2;
				}
			}
		}
	}

	if ( dir_rq == 0 )  // nowhere to retreat
		return 0;

	// return random dir from available dirs
	while ( true )
		for ( int i = 0; i < 6; ++i )
			if ( ok_dirs[i] == dir_rq && ( rand( ) % 100 ) < 10 )
				return 1 << i;

	// should be impossible to reach here
	return 0;
}

// retreat from the current hex
void MapPane::Retreat( )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	Counter* cntr = sit_[y][x].unit_list_;

	if ( _rule_set.AutoDR == FALSE )
	{
		if ( cntr )
			wxMessageBox( wxT("Please retreat any remaining units") );
		return;
	}

	while ( cntr )
	{
		switch ( cntr->getCounterType( ) )
		{
			case Counter::Type::MISC:				// perm airfields remain
				if (	static_cast<Facility*>( cntr )->getMiscType( ) == Facility::PERMAF3 ||
						static_cast<Facility*>( cntr )->getMiscType( ) == Facility::PERMAF6 ||
						static_cast<Facility*>( cntr )->getMiscType( ) == Facility::PERMAF9 ||
						static_cast<Facility*>( cntr )->getMiscType( ) == Facility::PERMAF12 )
					break;
				DestroyUnit( cntr );
				break;
			case Counter::Type::PTS:				// save status markers
			case Counter::Type::HIT:				// save hits
			case Counter::Type::NOTE:				// save notes
				break;
			case Counter::Type::PLANE:			// planes stay (they must be flying DAS)
				if ( cntr->getSide( ) == _current_player )	// own planes
					break;
				// TODO: shoudn't there be a call to DestroyUnit( cntr ) here?
				break;
			case Counter::Type::UNIT:				// units retreat
			{	// this explicit block encloses these case-specific declarations:
				char s[100];
				int dir = GetRetreatDir( cntr );

				// if nowhere to retreat, or a 0-MA unit
				if ( dir == 0 || static_cast<GroundUnit*>( cntr )->getMaxMP( ) == 0 )
				{
					cntr->getUnitString( s );
					sprintf( msg, "The following unit can't retreat and is eliminated:\n%s", s );
					wxMessageBox( wxS( msg ), Application::NAME );
					DestroyUnit( cntr );
					break;
				}

				cntr->getUnitString( s );
				sprintf( msg, "The following unit retreats to %s\n%s", ht::dir2Str( dir ), s );
				wxMessageBox( wxS( msg ), Application::NAME );

				// cancel any construction orders
				cntr->setMission( GroundUnit::Project::NONE );

				// TODO: or removeUnit( ptCurrentHex.x, ptCurrentHex.y, cntr ); ?
				removeUnitFromCurrentHex( cntr );

				// get the new coords in the accepted direction of retreat
				int nx, ny;
				ht::getAdjHexCoords( dir, x, y, &nx, &ny, world_x_size_, world_y_size_ );

				// retreat to ZOC:
				GroundUnit* u = (GroundUnit*)cntr;
				if ( ( _current_phase != Phase::REACT_COMB_PHASE && isHexInFriendlyZOC( nx, ny ) ) || ( _current_phase == Phase::REACT_COMB_PHASE && isHexInEnemyZOC( nx, ny ) ) )
				{
					// TODO: check whether intrinsically c/m units (i.e. those w/o supplemental mot. symbol) get treated correctly
					//		e.g. could use IsCm( u ); ??
					if ( u->isGerman( ) && u->isSupplementalMotorized( ) && IsEnemyOccupied( nx, ny ) )
						wxMessageBox( wxT("German C/M unit retreats to friendly occupied hex\nand is not affected by the ZOC!"), Application::NAME );
					else
					{	// unit retreats into ZOC and is eliminated, to include being cadre'd
						wxMessageBox( wxT("Unit retreats to ZOC!"), Application::NAME );
						if ( u->getFlag( GroundUnit::HAS_CADRE ) && ! u->getFlag( GroundUnit::IS_CADRE ) )
							MakeCadre( u );
						else
						{
							DestroyUnit( u );
							break;
						}
					}
				}

				sit_[ny][nx].setOwner( cntr->getSide( ), true );
				updateHitMarkers( nx, ny );	// for rail obstruction sync

				insertUnit( nx, ny, cntr );
				break;
			}
			default:					// destroy anything else
				DestroyUnit( cntr );
				break;
		}
		cntr = cntr->getNext( );
	}
}

const int DH_MAX_UNITS = 20;

// helper func for CommitHalfLosses()
void MapPane::DeleteMaskedUnits( GroundUnit* u[DH_MAX_UNITS], int mask )
{
	for ( int i = 0; i < DH_MAX_UNITS; ++i )
		if ( mask & ( 1 << i ) )
		{
			if ( ! u[i] )
			{
				wxMessageBox( wxT("You have found a bug in MapPane::DeleteMaskedUnits\n"
									"Please report this bug to author") );
				return;
			}
			if ( u[i]->getFlag( GroundUnit::HAS_CADRE ) && ! u[i]->getFlag( GroundUnit::IS_CADRE ) )
				MakeCadre( u[i] );
			else
				DestroyUnit( u[i] );
		}
}

// delete half of units in list (def)
// warning: this algorithm is a bit complicated
void MapPane::CommitHalfLosses( Counter* cntr )
{
	GroundUnit* c[DH_MAX_UNITS];
	for ( int i = 0; i < DH_MAX_UNITS; ++i )
		c[i] = 0;

	// calc total str and fill 'c' table with units
	int tot = 0;
	int cnt = 0;
	Counter* cp = cntr;
	while ( cp )
	{
		if ( cp->getCounterType( ) == Counter::Type::UNIT )
		{
			tot += cp->getDef( );  // raw str
			c[cnt] = (GroundUnit*)cp;
			++cnt;
		}
		cp = cp->getNext( );
	}
	tot = ( tot + 1 ) / 2;  // 'sum' pts (rounded up) needs to be deleted
	if ( cnt == 0 )  // no units to delete
		return;
	if ( cnt == 1 )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getFlag( GroundUnit::HAS_CADRE )
			&& ( (GroundUnit*)cntr )->getFlag( GroundUnit::IS_CADRE ) == FALSE )
			MakeCadre( cntr );
		else
			DestroyUnit( cntr );
		return;
	}

	// below: at least 2 units present
	int mask = 0;
	int best_mask = 0;
	int max_mask = ( 1 << cnt ) - 1;
	int best = 1000000;
	while ( mask < max_mask )
	{
		mask += 1;  // mask tells which units are counted now
		int sum = 0;
		for ( int i = 0; i < cnt; ++i )
			if ( mask & ( 1 << i ) ) 
				sum += c[i]->getDef( ); // add units whose corresponding bit is on

		if ( sum == tot )
		{  // found optimal combination, delete units
			DeleteMaskedUnits( c, mask );
			return;
		}
		if ( sum > tot && sum < best )
		{  // found best combination, remember it
			best = sum;
			best_mask = mask;
		}
	}
	// no optimal combination found, get second best
	DeleteMaskedUnits( c, best_mask );
}

// Variant A combat results
// delete quarter of units in list (def)
// warning: this algorithm is a bit complicated
void MapPane::CommitQuarterLosses( Counter* cntr )
{
	int tot = 0, cnt = 0, sum, mask, max_mask, best = 1000000, i, best_mask;
	GroundUnit* c[DH_MAX_UNITS];
	Counter* cp;

	for ( i = 0; i < DH_MAX_UNITS; ++i )
		c[i] = 0;

	// calc total str and fill 'c' table with units
	cp = cntr;
	while ( cp )
	{
		if ( cp->getCounterType( ) == Counter::Type::UNIT )
		{
			tot += cp->getDef( );  // raw str
			c[cnt] = (GroundUnit*)cp;
			++cnt;
		}
		cp = cp->getNext( );
	}
	tot = ( tot + 3 ) / 4;  // 'sum' pts (rounded up) needs to be deleted ie one quarter
	if ( cnt == 0 )
	{  // no units to delete
		return;
	}
	if ( cnt == 1 )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getFlag( GroundUnit::HAS_CADRE )
			&& ( (GroundUnit*)cntr )->getFlag( GroundUnit::IS_CADRE ) == FALSE )
		{
			MakeCadre( cntr );
		}
		else
		{
			DestroyUnit( cntr );
		}
		return;
	}
	// below: at least 2 units present
	mask = 0;
	best_mask = 0;
	max_mask = ( 1 << cnt ) - 1;
	while ( mask < max_mask )
	{
		mask += 1;  // mask tells which units are counted nowt
		sum = 0;
		for ( i = 0; i < cnt; ++i )
			if ( mask & ( 1 << i ) ) // sum the corresponding bit
				sum += c[i]->getDef( );

		if ( sum == tot ) // found optimal combination, delete units
		{
			DeleteMaskedUnits( c, mask );
			return;
		}
		if ( sum > tot && sum < best ) // found best combination, remember it
		{
			best = sum;
			best_mask = mask;
		}
	}
	// no optimal combination found, get second best
	DeleteMaskedUnits( c, best_mask );
}

// current hex was attacked with result 'result'
void MapPane::CommitLosses( int result )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// political police: no retreat (advanced rule: DE forces retreat always)
	bool defender_has_political_police = false;

	Counter* cntr;
	ENUM_HEXUNITS( x, y, cntr )
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::PLTCL_POL )
			defender_has_political_police = true;

	if ( defender_has_political_police && result == CombatResult::DR )
	{
		wxMessageBox( wxT("Political police motivate defenders: DR becomes EX") );
		result = CombatResult::EX;
	}

	float losses = 0.0F;

	Counter* next;

	cntr = sit_[y][x].unit_list_;

	switch ( result )
	{
		case CombatResult::AS:
		case CombatResult::AR:
			return;
		case CombatResult::AH:
			wxMessageBox( wxT("Destroy half of attacking units to satisfy losses"), Application::NAME );
			return;
		case CombatResult::AD:  // ww1
		case CombatResult::AE:
			wxMessageBox( wxT("Destroy all attacking units to satisfy losses"), Application::NAME );
			return;
		case CombatResult::HX:
		case CombatResult::EX:
		case CombatResult::DD:  // ww1
		case CombatResult::DE:
			if ( _rule_set.AutoDE == FALSE )
				wxMessageBox( wxT("Remove defending units to satisfy losses"), Application::NAME );
			while ( cntr )
			{
				next = cntr->getNext( );
				if ( _current_phase != Phase::REACT_COMB_PHASE && cntr->getSide( ) == _current_player ) // own planes
				{
					cntr = next;
					continue;
				}
				if ( _current_phase == Phase::REACT_COMB_PHASE && cntr->getSide( ) != _current_player ) // own planes
				{
					cntr = next;
					continue;
				}
				if ( cntr->getCounterType( ) == Counter::Type::UNIT )
				{
					GroundUnit* u = static_cast<GroundUnit*>( cntr );
					losses += u->getDef( );  // raw def str (to EX/HX)
					if ( _rule_set.AutoDE == TRUE )
					{
						if ( u->getFlag( GroundUnit::HAS_CADRE ) && ! u->getFlag( GroundUnit::IS_CADRE ) )
							MakeCadre( u );
						else
							DestroyUnit( u );
					}
				}
				cntr = next;
			}
			if ( result == CombatResult::EX )
			{
				sprintf( msg, "Exchange: remove units to satisfy losses of %d points", (int)losses );
				wxMessageBox( wxS( msg ), Application::NAME );
			}
			else if ( result == CombatResult::HX )
			{
				sprintf( msg, "Half Exchange: remove units to satisfy attacker losses of %d points", (int) ( losses + 1 ) / 2 );
				wxMessageBox( wxS( msg ), Application::NAME );
			}
			break;
		case CombatResult::DH:
			if ( _rule_set.AutoDH == FALSE )
			{
				if ( cntr )
					wxMessageBox( wxT("Please destroy half of defending units\nand retreat remaining ones") );
				return;
			}
			CommitHalfLosses( cntr );
			break;
		case CombatResult::DR:
			break;
		case CombatResult::AQ:
			wxMessageBox( wxT("Destroy one quarter of attacking units to satisfy losses"), Application::NAME );
			return;
		case CombatResult::AL:
		case CombatResult::DL:
		case CombatResult::AX:
		case CombatResult::DX:
		case CombatResult::DXM:
		case CombatResult::BX:
		case CombatResult::BXM:
			wxMessageBox( wxT("Sorry, this WW1 combat result is not yet implemented,\nplease apply it's effects manually."), Application::NAME );
			return;
		default:
			wxMessageBox( wxT("Non-standard combat result: please apply effects manually!") );
			return;
	}
	if ( defender_has_political_police && result != CombatResult::DE )
		wxMessageBox( wxT("Political police present: defenders do not retreat") );
	else
		Retreat( );
}

int MapPane::GetAttSupport( int x, int y )
{
	int pts = 0;

	Counter* c;
	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) == Counter::Type::PTS && static_cast<StatusMarker*>( c )->getPtsType( ) == StatusMarker::ATTACK )
			pts += static_cast<StatusMarker*>( c )->getPts( );
	}

	return pts;
}

int MapPane::GetDefSupport( int x, int y )
{
	int pts = 0;

	Counter* c;
	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) == Counter::Type::PTS && static_cast<StatusMarker*>( c )->getPtsType( ) == StatusMarker::DEFENSE )
			pts += static_cast<StatusMarker*>( c )->getPts( );
	}

	return pts;
}

// att support: any combat strength not represented by HexTools
void MapPane::SetAttSupport( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	StatusMarker* statusmarker = new StatusMarker( );
	statusmarker->setSide( _current_player );
	statusmarker->setPtsType( StatusMarker::ATTACK );
	statusmarker->edit( );
	insertUnitToCurrentHex( statusmarker ); // also sets dirty true
	paintCurrentHex( );
	contents_pane_->addUnit( statusmarker );
	contents_pane_->Refresh( );
}

// def support: any combat strength not represented by HexTools
void MapPane::SetDefSupport( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	StatusMarker* statusmarker = new StatusMarker( );
	statusmarker->setSide( _current_player );
	statusmarker->setPtsType( StatusMarker::DEFENSE );
	statusmarker->edit( );
	insertUnitToCurrentHex( statusmarker ); // also sets dirty true
	paintCurrentHex( );
	contents_pane_->addUnit( statusmarker );
	contents_pane_->Refresh( );
}

// FitE/SE winterization in winter 41/42
int MapPane::CalcWinterization( int dx, int dy, char* explain )
{
	// no winterization when not attacking in SNOW or WINTER weather
	if (	_weather_condition[hex_[dy][dx].getWeatherZone( )] == WeatherCondition::CLEAR
			|| _weather_condition[hex_[dy][dx].getWeatherZone( )] == WeatherCondition::MUD
			|| _weather_condition[hex_[dy][dx].getWeatherZone( )] == WeatherCondition::FROST )
		return 0;

	// no winterization except in the winter of 1941-42
	if ( _year < 41 || _year > 42 )
		return 0;

	if ( _year == 41 && _month < 10 )  // that's the previous winter
		return 0;

	if ( _year == 42 && _month > 5 )  // that's the following winter
		return 0;

	// no winterization when attacking a city or fortress
	City* city = hex_[dy][dx].city_;
	while ( city )
	{
		if (	city->type_ == City::Type::MINOR ||
				city->type_ == City::Type::MAJOR ||
				city->type_ == City::Type::PARTIALHEX ||
				city->type_ == City::Type::FULLHEX ||
				( city->type_ >= City::Type::FORTRESS && city->type_ <= City::Type::OUVRAGE ) )
			return 0;
		city = city->getNext( );
	}

	float att_tot_re = 0, att_w_re = 0;

	int dir;
	ENUM_DIRS(dir)
	{
		int ax, ay;
		ht::getAdjHexCoords( dir, dx, dy, &ax, &ay, world_x_size_, world_y_size_ );

		Counter* c;
		ENUM_HEXUNITS(ax, ay, c)
		{
			if ( c->getCounterType( ) == Counter::Type::UNIT )
			{
				GroundUnit* unit = (GroundUnit*)c;
				if ( unit->getAttackDir( ) == ht::getAdjHexPart( dir ) )
				{
					att_tot_re += unit->getRE( );
					att_w_re += unit->isWinterized( ) ? unit->getRE( ) : 0;
				}
			}
		}
	}

	float def_tot_re = 0, def_w_re = 0;

	// any Soviet defenders (first winter modifier only applies to east front)
	Counter* c;
	ENUM_HEXUNITS(dx, dy, c)
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* unit = (GroundUnit*)c;
			if (	unit->getType( ) != GroundUnitType::WW2::POS_HV_AA
					&& unit->getType( ) != GroundUnitType::WW2::POS_LT_AA
					&& unit->getType( ) != GroundUnitType::WW2::TRUCK
					&& unit->getType( ) != GroundUnitType::WW2::TRANSPORT
					&& unit->getType( ) != GroundUnitType::WW2::APC
					&& unit->getType( ) != GroundUnitType::WW2::LVT )
			{
				def_tot_re += unit->getRE( );
				def_w_re += unit->isWinterized( ) ? unit->getRE( ) : 0;
			}
		}
	}

	// ratios
	int mod = 0;
	float att_r = att_w_re / att_tot_re;
	float def_r = def_w_re / def_tot_re;
	// calc winterization
	if ( att_r >= 0.5 )
	{
		if ( def_r < 0.2 )
			mod = 2;
		else if ( def_r < 0.5 )
			mod = 1;
		else
			mod = 0;  // both sides over half
	}
	else if ( att_r >= 0.2 )
	{
		mod = ( def_r < 0.2 ) ? 1 : 0; // def 1/5 or more => no bonus
	}
	else
	{
		mod = 0;
	}
	char mod_explain[60];
	sprintf( mod_explain, ", Winterization: +%d", mod );
	strcat( explain, mod_explain );
	return mod;
}

// modify AEC
int MapPane::ModifyAEC( int x, int y, int aec )
{
	int w = _weather_condition[hex_[y][x].getWeatherZone( )];

	if ( w == WeatherCondition::MUD )
		aec = 0;

	switch ( hex_[y][x].terrain_ )
	{  // TODO: vet this list for new terrain
		case HexType::MOUNTAIN:
		case HexType::FOREST:
		case HexType::SWAMP:
		case HexType::WOODEDSWAMP:
		case HexType::CANALS:
		case HexType::BOCAGE:
		case HexType::RAVINES:
			aec = 0;
			break;
	}

	// city/fortress effect
	if ( hex_[y][x].city_ != NULL )
	{
		switch ( hex_[y][x].city_->type_ )
		{  // TODO: vet this list for new terrain
			case City::Type::MINOR:  // town
				break;
			case City::Type::MAJOR:  // dot city
			case City::Type::PARTIALHEX:
			case City::Type::FULLHEX:
			case City::Type::UNUSED_WESTWALL:
			case City::Type::WESTWALL_1:
			case City::Type::WESTWALL_2:
			case City::Type::WESTWALL_3:
			case City::Type::UNUSED_OUVRAGE:
			case City::Type::OUVRAGE:
			case City::Type::UNUSED_FORTRESS:
			case City::Type::FORTRESS:
				aec = 0;
				break;
		}
	}

	// fort counter
	if ( sit_[y][x].isFort( ) )
		aec = 0;

	// reduced AEC in winter
	if ( w == WeatherCondition::SNOW || w == WeatherCondition::WINTER )
	{
		// snow: there may be no AEC at all or reduced AEC
		if ( _rule_set.SnowReducedAEC == FALSE )
			aec = 0;
		else if ( aec == ArmorAntitank::Proportion::FULL || aec == ArmorAntitank::Proportion::HALF )
			aec = ArmorAntitank::Proportion::PARTIAL;
		else
			aec = 0;
	}
	return aec;
}

// calc AECA for attack on hex x, y
// NOTE: no units may be selected in any other hex than in x,y
int MapPane::CalcNominalAECA( int x, int y )
{
	if ( ! sit_[y][x].isTarget( ) )
	{
		wxMessageBox( wxT("MapPane::CalcNominalAECA: not a target hex") );
		return 0;
	}

	float full_re = 0.0;
	float half_re = 0.0;
	float neut_re = 0.0;
	float norm_re = 0.0;

	int d, x2, y2;
	ENUM_DIRS(d)
	{
		int od = oppositeDir( d );
		ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ );
		if ( IsHexsideFortified( x2, y2, od ) )
			sit_[y2][x2].addNeut_RE( &neut_re, &norm_re, od );
		else
			sit_[y2][x2].addAECA_RE( &full_re, &half_re, &neut_re, &norm_re, od );
	}

	return calculateOptimalAEC( full_re, half_re, neut_re, norm_re );
}

// Variant A if commando involved in attack then a chance of surprise attack
// check if a command unit capable of surprise attacks is attacking the hex
int MapPane::CommandoUnitAttacking( int x, int y )
{
	int d, x2, y2;
	ENUM_DIRS(d)
	{
		ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ );
		if ( sit_[y2][x2].commandoUnitAttacking( oppositeDir( d ) ) )
			return 1;
	}
	return 0;
}

// calculate engineer modifier when attacking hex x,y
// TODO: also treat engineers defending in a qualified hex
int MapPane::CalcEngMod( int x, int y )
{
	// TODO: also ok for engineering: fortified area units,
	// and port fortification units

	// can use eng modifier at all?
	int ok = 0;
	if ( sit_[y][x].isFort( ) )  // fort counter: yes
		ok = 1;
	else  // if (Sit[y][x].IsHexsideFortified() == FALSE)
	{
		// no fort, scan for correct kind of city or fortification feature
		City* cp = hex_[y][x].city_;
		while ( cp != NULL )
		{
			switch ( hex_[y][x].city_->type_ )
			{
				case City::Type::MINOR:  // reference city
				case City::Type::MAJOR:  // dot city
				case City::Type::MAPTEXT:
				case City::Type::PT_CITY:
				case City::Type::REF_PT:
				case City::Type::BIG_TEXT:
				case City::Type::ORE_RESOURCE:
				case City::Type::ENERGY_RESOURCE:
				case City::Type::OASIS:
					break;
					// TODO: review this; should be full- or partial-hex city, or
					// 	any line on the FEC with the DRM -1 */
					// 2.2.3: I assume engineers do get +1 bonus even if there is no -1
#if 0
				case PARTIALHEX :	// west: -0 -> no eng mod
					if ( RuleSet.WestCityMod )
						ok = 0;
					else
						ok = 1;
					break;
#endif
				default:  // other types are major cities or fortresses: bonus
					ok = 1;
					break;
			}
			if ( ok )  // no need to search through the rest of the city collection
				break;
			cp = cp->getNext( );
		}
	}

	if ( ! ok )
		return 0;

	float eng_re = 0, norm_re = 0;
	int d, x2, y2;
	ENUM_DIRS(d)
	{
		ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ );
		sit_[y2][x2].addEng_RE( &eng_re, &norm_re, oppositeDir( d ) );
	}

	if ( eng_re * _rule_set.EngProportion >= norm_re )
		return 1;

	return 0;
}

// defense modifier for the hex
int MapPane::GetMods( int x, int y, char* explain )
{
	// weather of the target hex affect hex basic die modifier
	int weather = ( _weather_condition[hex_[y][x].getWeatherZone( )] );

	int m;
	// handle swamp -1/-2 die modifier here
	if ( hex_[y][x].terrain_ == HexType::SWAMP && _rule_set.NewSwamp == FALSE )
		m = -2;
	else
	{
		switch ( weather )
		{
			case WeatherCondition::SNOW:
			case WeatherCondition::WINTER:
				m = _hex_types[hex_[y][x].terrain_].drm_[WeatherClass::WINTER];
				break;
			case WeatherCondition::MUD:
				m = _hex_types[hex_[y][x].terrain_].drm_[WeatherClass::MUD];
				break;
			case WeatherCondition::FROST:
			case WeatherCondition::CLEAR:
			default:
				m = _hex_types[hex_[y][x].terrain_].drm_[WeatherClass::CLEAR];
				break;
		}
	}

	// city/fort effect
	int fort = 0;
	if ( hex_[y][x].city_ != NULL )
	{
		switch ( hex_[y][x].city_->type_ )
		{
			case City::Type::MINOR:
				break;
			case City::Type::MAJOR:
				break;
			case City::Type::PARTIALHEX:
				if ( _rule_set.WestCityMod == FALSE )
					m = -1;
				break;
			case City::Type::FULLHEX:
				if ( _rule_set.WestCityMod == FALSE )
					m = -2;
				else
					m = -1;
				break;
			case City::Type::UNUSED_WESTWALL:
				fort = 1;
				break;
			case City::Type::WESTWALL_1:
				fort = 1;
				break;
			case City::Type::WESTWALL_2:
				fort = 2;
				break;
			case City::Type::WESTWALL_3:
				fort = 3;
				break;
			case City::Type::UNUSED_OUVRAGE:
				fort = 1;
				break;
			case City::Type::OUVRAGE:
				fort = 1;
				break;
			case City::Type::UNUSED_FORTRESS:
				fort = 1;
				break;
			case City::Type::FORTRESS:
				fort = 1;
				break;
		}
	}

	// weather of the target hex affect hex basic die modifier
	//weather = (Weather[Hex[y][x].GetWeatherZone()]);
	if ( _rule_set.FreezingAllowed == TRUE )
	{
		switch ( hex_[y][x].terrain_ )
		{
			case HexType::SWAMP:  // swamp freezes in winter
				if ( weather == WeatherCondition::WINTER || weather == WeatherCondition::SNOW || weather == WeatherCondition::FROST )
					m = 0;
				break;
			case HexType::WOODEDSWAMP:  // swamp freezes in winter
				if ( weather == WeatherCondition::WINTER || weather == WeatherCondition::SNOW || weather == WeatherCondition::FROST )
					m = -1;
				break;
			case HexType::SWAMPYTUNDRA:  // tundra freezes
				if ( weather == WeatherCondition::WINTER || weather == WeatherCondition::SNOW || weather == WeatherCondition::FROST )
					m = 0;
				break;
		}
	}

	if ( explain )
		sprintf( explain, "Hex: %d", m );

	// general weather effect (if enabled)
	int wm = 0;
	if ( _rule_set.WeatherDieMod == TRUE )
	{
		if ( weather == WeatherCondition::WINTER || weather == WeatherCondition::SNOW )
			wm = 1;
		else if ( weather == WeatherCondition::MUD )
			wm = 2;
	}
	m -= wm;

	if ( wm && explain )
	{
		char tmps[255];
		sprintf( tmps, ", Weather: %d", -wm );
		strcat( explain, tmps );
	}

	// fort counter?
	if ( sit_[y][x].isFort( ) )
	{
		// TODO: ww1 fortifications!
		if ( fort )
		{  // fort in hex that has fortress -> it's unimproved,
		   //m += fort;	// die mod. is always -1
		   //m -= 1;
			fort = 1;
		}
		else
		{
			// no fortress, it's just -1
			fort = 1;
		}
	}
	m -= fort;
	if ( explain && fort )
	{
		char tmps[255];
		sprintf( tmps, ", Fort: %d", -fort );
		strcat( explain, tmps );
	}

	// status marker(s)
	int i = 0;
	Counter* c;
	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) == Counter::Type::PTS && ( (StatusMarker*)c )->getPtsType( ) == StatusMarker::DIE )
			i += ( (StatusMarker*)c )->getPts( );
	}
	m += i;
	if ( explain && i )
	{
		char tmps[255];
		sprintf( tmps, ", Status markers: %s%d", i > 0 ? "+" : "", i );
		strcat( explain, tmps );
	}
	return m;
}

// FitE/SE: Axis first winter effect?
int MapPane::GermanFirstWinterMod( int dx, int dy, char* explain )
{
	// no winterization when not attacking in SNOW or WINTER weather
	if (	_weather_condition[hex_[dy][dx].getWeatherZone( )] == WeatherCondition::CLEAR
			|| _weather_condition[hex_[dy][dx].getWeatherZone( )] == WeatherCondition::MUD
			|| _weather_condition[hex_[dy][dx].getWeatherZone( )] == WeatherCondition::FROST )
		return 0;

	// no winterization except in the winter of 1941-42
	if ( _year < 41 || _year > 42 )
		return 0;

	if ( _year == 41 && _month < 10 )  // that's the previous winter
		return 0;

	if ( _year == 42 && _month > 5 )  // that's the following winter
		return 0;

	// any German attackers?
	int ger = 0;
	int dir;
	ENUM_DIRS(dir)
	{
		int ax, ay;
		ht::getAdjHexCoords( dir, dx, dy, &ax, &ay, world_x_size_, world_y_size_ );
		Counter* c;
		ENUM_HEXUNITS(ax, ay, c)
		{
			// TODO: re-vet this logic for Total War
			if (	c->getCounterType( ) == Counter::Type::UNIT
					&& ( (GroundUnit*)c )->getSubType( ) != GroundArmedForce::Axis::FIN
					&& ( (GroundUnit*)c )->getAttackDir( ) == ht::getAdjHexPart( dir ) )
				++ger;
		}
	}
	if ( ! ger )
		return 0;

	// any Soviet defenders?
	int sov = 0;
	Counter* c;
	ENUM_HEXUNITS(dx, dy, c)
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->isSoviet( ) )
			++sov;
	}
	if ( sov )
	{
		strcat( explain, ", First winter: -1" );
		return -1;
	}
	return 0;
}

// is hex supported (so that all unsupported units in it are supported too)
// 'dir' = attack dir, -1=any or none
// include_all_sel: if true, check all selected units.
//		needed while moving and checking for overrun strengths
bool MapPane::IsHexSupported( int x, int y, int dir, bool include_all_sel )
{
	Counter* c;
	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) != Counter::Type::UNIT )
			continue;

		GroundUnit* u = (GroundUnit*)c;

		// dir==-1 if not given
		if ( dir >= 0 && u->getAttackDir( ) != dir )
			continue;

		// divisions and corps support everybody
		// (if they are not self-supported or unsupported)
		if (	u->getSize( ) >= GroundUnit::Size::DIVISIONAL
				&& ! u->isCadre( )
				&& u->getFlag( GroundUnit::SUPPORTED ) == 0  // does this mean self-supported?
				&& u->getFlag( GroundUnit::UNSUPPORTED ) == 0 )
			return true;

		// artillery or HQ supports everybody
		if (	u->getType( ) == GroundUnitType::WW2::ART
				|| u->getType( ) == GroundUnitType::WW2::LR_SIEGE_ART
				|| u->getType( ) == GroundUnitType::WW2::MORTAR
				|| u->getType( ) == GroundUnitType::WW2::RKT_ART
				|| u->getType( ) == GroundUnitType::WW2::SIEGE_ART
				|| u->getSize( ) == GroundUnit::Size::HQ
				// ww1 artillery types:
				|| u->getType( ) == GroundUnitType::WW2::COASTART
				|| u->getType( ) == GroundUnitType::WW2::FLDART
				|| u->getType( ) == GroundUnitType::WW2::FOOTART
				|| u->getType( ) == GroundUnitType::WW2::HVART
				|| u->getType( ) == GroundUnitType::WW2::HVMORT
				|| u->getType( ) == GroundUnitType::WW2::HVSIEGEART
				|| u->getType( ) == GroundUnitType::WW2::LTMORT
				|| u->getType( ) == GroundUnitType::WW2::SIEGEMORT
				// ww2pac:
				|| u->getType( ) == GroundUnitType::WW2::HORS_ART )
			return true;
	}

	if ( isCurrentHex( x, y ) )
	{
		ENUM_SELUNITS(c)
		{
			if ( c->getCounterType( ) != Counter::Type::UNIT )
				continue;
			GroundUnit* u = (GroundUnit*)c;

			// dir==-1 if not given
			if ( ! include_all_sel && dir >= 0 && u->getAttackDir( ) != dir )
				continue;

			// divisions, corps and armies support everybody (if they are not self-supported)
			if ( u->getSize( ) >= GroundUnit::Size::DIVISIONAL && u->getFlag( GroundUnit::SUPPORTED ) == 0 && u->getFlag( GroundUnit::UNSUPPORTED ) == 0 )
				return true;

			// artillery & HQs supports everybody
			if ( u->getType( ) == GroundUnitType::WW2::ART || u->getType( ) == GroundUnitType::WW2::LR_SIEGE_ART || u->getType( ) == GroundUnitType::WW2::MORTAR
				|| u->getType( ) == GroundUnitType::WW2::RKT_ART || u->getType( ) == GroundUnitType::WW2::SIEGE_ART || u->getSize( ) == GroundUnit::Size::HQ
				|| u->getSize( ) == GroundUnit::Size::CORPSHQ || u->getSize( ) == GroundUnit::Size::ARMYHQ
			// ww1 artillery:
				|| u->getType( ) == GroundUnitType::WW2::FLDART || u->getType( ) == GroundUnitType::WW2::FOOTART || u->getType( ) == GroundUnitType::WW2::HVART
				|| u->getType( ) == GroundUnitType::WW2::HVMORT || u->getType( ) == GroundUnitType::WW2::LTMORT )  // siege art not included???
				return true;
		}
	}

	return false;
}

// get defense str(*1) of selected units
float MapPane::CalcSelDefStr( bool sup )
{
	float str = 0;
	Counter* unit = _selected_unit_list;

	while ( unit )
	{
		if ( sup )
			str += unit->getRealDef( -1 );
		else
		{
			if ( ( (GroundUnit*)unit )->isSelfSupported( ) )
				str += unit->getRealDef( -1 );
			else
				str += ( ( (float)unit->getRealDef( -1 ) ) / 2);
		}
		unit = unit->getNext( );
	}
	return str;
}

// defense strength of the hex
float MapPane::CalcDefStr( int x, int y )
{
	// art limit checking:
	float art_re = 0, norm_re = 0, art_str = 0, tot_art_str = 0, max_def;
	int sel[100], i, sel_i = 0;

	float str = 0;
	bool sup = IsHexSupported( x, y );
	if ( isCurrentHex( x, y ) )
		str = CalcSelDefStr( sup );

	str += sit_[y][x].getRawDefStr( sup );

	// if more artillery than normal units extra arts will have tot. str of 1
	GroundUnit* u = NULL;
	Counter* c;
	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) != Counter::Type::UNIT )
			continue;

		u = (GroundUnit*)c;
		if ( u->isArtillery( ) )
		{
			art_re += u->getRE( );
			tot_art_str += u->getRealDef( );  // calculate str of all artillery units
		}
		else if ( u->isCombatUnit( ) )
		{
			norm_re += u->getRE( );
		}
	}

	GroundUnit* sel_u;

	// Too many art units, strongest ones will defend at full str.
	if ( art_re > norm_re )
	{
		for ( i = 0; i < 100; ++i )
			sel[i] = FALSE;

		art_re = 0;
		while ( art_re < norm_re )
		{
			i = -1;
			max_def = -1;
			sel_u = NULL;
			ENUM_HEXUNITS(x, y, c)
			{
				++i;
				if ( c->getCounterType( ) != Counter::Type::UNIT )
					continue;

				u = (GroundUnit*)c;
				if (	! sel[i]
						&& u->isArtillery( )
						&& u->getRE( ) <= ( norm_re - art_re )
						&& u->getRealDef( ) > max_def )
				{
					sel_u = u;
					sel_i = i;
					max_def = u->getRealDef( );
				}
			}
			if ( ! sel_u )
			{
				break;
			}
			art_re += sel_u->getRE( );
			art_str += sel_u->getRealDef( );
			sel[sel_i] = TRUE;
		}
		str -= tot_art_str;
		str += art_str + ( u->isSoviet( ) ? 0 : 1 );
	}
	if ( norm_re > 0 && str == 0 )
	{
		str = 0.01;  // nominal def str to trigger overrun MP cost
	}

#if 0
	sprintf(msg, "%.1f pts of artillery %.1f pts defend ok, %.1f pts defend with one -> tot. %.1f",
	        tot_art_str, art_str, tot_art_str - art_str, art_str+1);
	wxMessageBox(msg);
#endif
	return str;
}

// return strength of the unit from x,y attacking direction "dir", 'sup'
// tells whether this hex is supported
float MapPane::CalcUnitAttStr( GroundUnit* unit, int x, int y, int dir, bool sup )
{
	int nx, ny;
	ht::getAdjHexCoords( dir, x, y, &nx, &ny, world_x_size_, world_y_size_ );
	// Variant A add German/Finnish CEV
	//int defender = -1;
	// MajorityDefenderSide(nx,ny);
	float att = unit->getRealAtt( );
	//df = HexTypes[Hex[ny][nx].Terrain].iDefFlags;
	// weather of the target hex
	int weather = ( _weather_condition[hex_[y][x].getWeatherZone( )] );
	int ice = _rivers_lakes_frozen[hex_[y][x].getWeatherZone( )];
	switch ( hex_[ny][nx].terrain_ )
	{
		case HexType::MOUNTAIN:  // non-mountain halved (TODO: mtn art...)
			if ( unit->getType( ) != GroundUnitType::WW2::MTN && unit->getType( ) != GroundUnitType::WW2::HIGH_MOUNTAIN && ! unit->isMtn( ) )
				att /= 2;
			break;
		case HexType::FOREST:  // non-artillery C/M halved
			if ( unit->isSupplementalMotorized( ) && ! unit->isArtillery( ) )
				att /= 2;
			break;
		case HexType::SWAMP:  // mot/art halved (not in winter)
		case HexType::WOODEDSWAMP:
			if ( weather == WeatherCondition::FROST || weather == WeatherCondition::SNOW || weather == WeatherCondition::WINTER )
				break;
			if ( unit->isSupplementalMotorized( ) || unit->isArtillery( ) )
				att /= 2;
			break;
		case HexType::LAKE:  // atkr (excp art) halved (not in winter)
		case HexType::WOODEDLAKE:
			if ( weather == WeatherCondition::FROST || weather == WeatherCondition::SNOW || weather == WeatherCondition::WINTER )
				break;
			if ( ! unit->isArtillery( ) )
				att /= 2;
			break;
		case HexType::SALTLAKE:  // in winter too
			if ( ! unit->isArtillery( ) )
				att /= 2;
			break;
		case HexType::BOCAGE:  // mot atkr (excp art) halved
			if ( unit->isSupplementalMotorized( ) && !unit->isArtillery( ) )
				att /= 2;
			break;
		case HexType::JUNGLE:  // attacker (excp. inf & mtn) halved, mot prohib.
		case HexType::JUNGLEROUGH:
		case HexType::JUNGLESWAMP:
			if ( unit->isSupplementalMotorized( ) )
				att = 0;
			if ( unit->getType( ) != GroundUnitType::WW2::MTN && unit->getType( ) != GroundUnitType::WW2::INF )
				att /= 2;
			break;
		case HexType::JUNGLEMTN:  // attacker (excp. mtn) halved, mot prohib.
			if ( unit->isSupplementalMotorized( ) )
				att = 0;
			if ( unit->getType( ) != GroundUnitType::WW2::MTN )
				att /= 2;
			break;
		case HexType::INTIRRIGATION:  // attacker halved
			if ( unit->isSupplementalMotorized( ) )
				att = 0;
			att /= 2;
			break;
		case HexType::EXTREMERAVINES:  // attacker (excp. mtn) halved, mot prohib.
			if ( unit->isSupplementalMotorized( ) )
				att = 0;
			if ( unit->getType( ) != GroundUnitType::WW2::MTN )
				att /= 2;
			break;
	}

	// cities:
	int improved_fortress = 0;
	if ( hex_[ny][nx].city_ != NULL )
	{
		switch ( hex_[ny][nx].city_->type_ )
		{
			case City::Type::MINOR:
			case City::Type::MAJOR:
				break;
			case City::Type::PARTIALHEX:
			case City::Type::FULLHEX:
				if ( unit->isSiegeArtillery( ) || unit->isAssaultEngineer( ) )
					att *= 2;
				else if ( ! unit->isArtillery( ) && ! unit->isCombatEngineer( ) )
					att /= 2;
				break;
			case City::Type::UNUSED_WESTWALL:
			case City::Type::WESTWALL_1:
			case City::Type::WESTWALL_2:
			case City::Type::WESTWALL_3:
			case City::Type::UNUSED_OUVRAGE:
			case City::Type::OUVRAGE:
			case City::Type::UNUSED_FORTRESS:  // -1, no aec
				break;
			// TODO: -map also check that fortress here is improved
			case City::Type::FORTRESS:  // att halved (excpt art,eng), gs halved, sg art dbl
				improved_fortress = 1;
				break;
		}
	}

	// fort counter in improved fortress hex: it's unimproved fortress!
	Counter* c;
	ENUM_HEXUNITS(nx,ny, c)
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::FORT )
		{
			improved_fortress = 0;
			break;
		}
	// port fortifications are improved fortresses:
	ENUM_HEXUNITS(nx,ny, c)
		if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->getType( ) == GroundUnitType::WW2::PORT_FORT )
		{
			improved_fortress = 1;
			break;
		}

	if ( improved_fortress )
	{
		if ( unit->isSiegeArtillery( ) || unit->isAssaultEngineer( ) )
			att *= 2;
		else if ( ! unit->isArtillery( ) && ! unit->isCombatEngineer( ) )
			att /= 2;
	}

	//hexside effects (att=0 means attacking prohibited):
	int art = unit->isArtillery( );
	//int mot = unit->IsSupplementalMotorized();

	// WW1ModernArtTactics: artillery is sometimes halved (note: Jan=0)
	if ( unit->isArtillery( ) && _rule_set.WW1ModernArtTactics == TRUE )
	{
		// From Oct I 17 on: Austria-Hungary and Germany.
		if ( ( unit->isGerman( )
			|| ( unit->getSide( ) == SidePlayer::AXIS
				&& ( unit->getSubType( ) == GroundArmedForce::Axis::AX_WW1_AH || unit->getSubType( ) == GroundArmedForce::Axis::AX_WW1_AUS
					|| unit->getSubType( ) == GroundArmedForce::Axis::AX_WW1_HUN ) ) ) && ( _year <= 17 && _month < 9 ) )
			att /= 2;
		// From Jul I 18 on: America, Britain, and France.
		else if ( ( unit->getSide( ) == SidePlayer::ALLIED
			&& ( unit->getSubType( ) == GroundArmedForce::Allied::AL_WW1_RFC || unit->getSubType( ) == GroundArmedForce::Allied::AL_WW1_RN || unit->getSubType( ) == GroundArmedForce::Allied::BRITISH
				|| unit->getSubType( ) == GroundArmedForce::Allied::FRENCH /* || USA */) ) && ( _year <= 18 && _month < 6 ) )
			att /= 2;
		// From Feb I 19 on: All other countries
		else if ( _year <= 19 && _month < 1 )
			att /= 2;
	}

	// Add option that border rivers do not affect combat
#if 0
	sprintf( msg, "ignore=%d, isintlborder=%d, isminorriver=%d, ismajorriver=%d, iscanal=%d", RuleSet.BorderRiversIgnored, IsIntlBorder( x, y, dir ), IsMinorRiver( x, y, dir ), IsMajorRiver( x, y, dir ), IsCanal( x, y, dir ) );
	wxMessageBox( msg, Application::NAME );
#endif
	if ( _rule_set.BorderRiversIgnored == TRUE && IsIntlBorder( x, y, dir ) )
		att /= 1;
	else
	{
		if ( ! ice && IsMinorRiver( x, y, dir ) && ! art )  // frozen: no eff
			att /= 2;
		if ( ! ice && IsCanal( x, y, dir ) && ! art )  // frozen: no eff
			att /= 2;
		if ( ice && IsMajorRiver( x, y, dir ) && ! art )  // frozen: halved
			att /= 2;
		else if ( ! ice && IsMajorRiver( x, y, dir ) && ! art )
			att /= 4;
		else if ( ! ice && IsMajorRiver( x, y, dir ) && art && use_WW1_hexsides_ )
			att /= 2;
	}
	if ( IsMountain( x, y, dir ) && unit->getType( ) != GroundUnitType::WW2::MTN && unit->getType( ) != GroundUnitType::WW2::HIGH_MOUNTAIN )
		att /= 2;
	if ( IsHighMtn( x, y, dir ) )
		att = 0;
	if ( IsHighMtnPass( x, y, dir ) )
		att = 0;
	if ( IsKarst( x, y, dir ) )
		att = 0;
	if ( ice && IsNarrowStrait( x, y, dir ) && ! art )  // frozen: halved
		att /= 2;
	else if ( ! ice && IsNarrowStrait( x, y, dir ) && ! art )
		att /= 4;
	else if ( _rule_set.IgnoreFrozenLakeHexsides == TRUE && ice && IsLakeSide( x, y, dir ) && ! art )
		att /= 1;
	else if ( _rule_set.IgnoreFrozenLakeHexsides == FALSE && ice && IsLakeSide( x, y, dir ) && ! art )  // frozen lake: quartered (exc art)
		att /= 4;
	else if ( ice && ( IsSeaSide( x, y, dir ) || IsRWaterSide( x, y, dir ) ) )  // frozen sea: all quartered
		att /= 4;
	else if ( ! ice && IsWater( x, y, dir ) )  // non-frozen lake/sea
		att = 0;
	if ( IsSaltDesert( x, y, dir ) )
		att = 0;
	if ( IsGlacier( x, y, dir ) )
		att = 0;
	// escarpment up: attacker halved (except art)
	int dir2 = ht::getAdjHexPart( dir );
	// TODO: DLT logic check, regarding: fort = escarpment + impassable escarpment
	if ( ! hex_[ny][nx].getFortifiedSide( dir2 ) )
	{
		if ( hex_[ny][nx].getEscarpment( dir2 ) && ! art )
			att /= 2;
		if ( IsImpEscarpment( x, y, dir ) )
			att = 0;
	}
	//if ( IsHexsideFortified( x, y, dir ) ) 	;  // fort doesn't affect here

	// check for supporting arms:
	if ( ! sup && ! unit->isSupported( ) )
		att /= 2;

	// lack of attack supply halves unit
	// 2.2.3: only required in combat phase, not for overruns!
	if ( _current_phase == Phase::COMBAT_PHASE && _rule_set.AttackSup == TRUE && ! unit->getAttackSupply( ) )
		att /= 2;

	// disruption is handled in GetRealAtt()
#if 0
	char s[100];
	unit->GetSymbolString( s );
	sprintf( msg, "%s: dir=%d, att = %f", s, dir, att );
	wxMessageBox( msg, Application::NAME );
#endif
	return att;
}

// get attack str of selected units
// NOTE: units must have been ordered to attack
float MapPane::CalcSelAttStr( int dir )
{
	float str = 0;
	Counter* unit = _selected_unit_list;

	while ( unit )
	{
		if ( unit->getCounterType( ) == Counter::Type::UNIT && unit->getAttackDir( ) == dir )
			str += CalcUnitAttStr( (GroundUnit*)unit, selected_hex_point_.x, selected_hex_point_.y, dir, IsHexSupported( selected_hex_point_.x, selected_hex_point_.y, dir ) );
		unit = unit->getNext( );
	}
	return str;
}

float MapPane::CalcAttStr( int x, int y, int dir )
{
	bool sup = IsHexSupported( x, y, dir );

	float str = 0;
	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
		str = CalcSelAttStr( dir );

	Counter* cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* u = (GroundUnit*)cntr;
			if ( u->getAttackDir( ) == dir )
				str += CalcUnitAttStr( u, x, y, dir, sup );
		}
		cntr = cntr->getNext( );
	}
	return str;
}

// Allow ships to attack in combat phase
float MapPane::CalcShipAttStr( NavalUnit* ship, int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(dir) )
{
	int defender = -1;  // MajorityDefenderSide(nx,ny);
	return ship->getRealAtt( defender );
}

// Allow ships to attack in combat phase
float MapPane::CalcSelNgsStr( int WXUNUSED(dir) )
{
	float str = 0;
#if 0
	Counter* cntr = SelectedUnits;

	while (cntr)
	{
		if (cntr->Type() == CNTR_SHIP && cntr->GetAttackDir() == dir)
			str += CalcShipAttStr((MyShip*)cntr, ptCurrentHex.x, ptCurrentHex.y, dir);
		cntr = cntr->GetNext();
	}
#endif
	return str;
}

// Allow ships to attack in combat phase
float MapPane::CalcNgsStr( int x, int y, int dir )
{
	float str = 0;
	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
		str = CalcSelNgsStr( dir );

	Counter* cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::SHIP )
		{
			NavalUnit* ship = (NavalUnit*)cntr;
			if ( ship->getAttackDir( ) == dir )
				str += CalcShipAttStr( ship, x, y, dir );
		}
		cntr = cntr->getNext( );
	}
	return str;
}

// calculate ground support
float MapPane::CalcGS( int x, int y )
{
	Counter* cntr;
	AirUnit* au;
	float gs = 0;

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
	{
		cntr = _selected_unit_list;
		while ( cntr )
		{
			if ( cntr->getCounterType( ) == Counter::Type::PLANE )
			{
				au = (AirUnit*)cntr;
				if ( au->getMission( ) == AirUnit::Mission::BOMB_GS )
					gs += au->getTac( );
			}
			cntr = cntr->getNext( );
		}
	}
	cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE )
		{
			au = (AirUnit*)cntr;
			if ( au->getMission( ) == AirUnit::Mission::BOMB_GS )
			{
				if ( au->getExtRange( ) )  // extended range -> 1/3 str
					gs += ( (float)au->getTac( ) ) / 3;
				else
					gs += au->getTac( );
			}
		}
		cntr = cntr->getNext( );
	}
	if ( _rule_set.GSHalved == TRUE )
	{
		switch ( hex_[y][x].terrain_ )
		{
			case HexType::MOUNTAIN:
			case HexType::FOREST:
			case HexType::SWAMP:
			case HexType::WOODEDSWAMP:
				gs /= 2;
				break;
			default:
				if ( hex_[y][x].city_ && ( hex_[y][x].city_->type_ == City::Type::PARTIALHEX || hex_[y][x].city_->type_ == City::Type::FULLHEX ) )
					gs /= 2;
				break;
		}

	}
	return gs;
}

// calculate defensive air support
float MapPane::CalcDAS( int x, int y )
{
	Counter* cntr;
	AirUnit* au;
	float das = 0;

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
	{
		ENUM_SELUNITS(cntr)
		{
			if ( cntr->getCounterType( ) == Counter::Type::PLANE )
			{
				au = (AirUnit*)cntr;
				if ( au->getMission( ) == AirUnit::Mission::BOMB_DAS )
				{
					if ( au->getExtRange( ) )  // extended range -> 1/3 str
						das += ( (float)au->getTac( ) ) / 3;
					else
						das += au->getTac( );
				}
			}
		}
	}
	ENUM_HEXUNITS (x, y, cntr)
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE )
		{
			au = (AirUnit*)cntr;
			if ( au->getMission( ) == AirUnit::Mission::BOMB_DAS )
			{
				if ( au->getExtRange( ) )  // extended range -> 1/3 str
					das += ( (float)au->getTac( ) ) / 3;
				else
					das += au->getTac( );
			}
		}
	}
	switch ( _rule_set.DASMode )
	{
		case DASMode::FULL_STR_MOD:
			switch ( hex_[y][x].terrain_ )
			{
				case HexType::MOUNTAIN:
				case HexType::FOREST:
				case HexType::SWAMP:
				case HexType::WOODEDSWAMP:
					das /= 2;
					break;
			}
			break;
		case DASMode::FULL_STR:
			break;
		case DASMode::HALF_STR:
			das /= 2;
			break;
	}
	return das;
}

// calculate artillery/non-artillery REs attacking (add to *args)
void MapPane::AddAttArtREs( int x, int y, int dir, float* art_re, float* norm_re )
{
	Counter* cntr;
	ENUM_HEXUNITS(x, y, cntr)
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* u = (GroundUnit*)cntr;
			if ( u->getAttackDir( ) == dir )
			{
				if ( u->isArtillery( ) )
					*art_re += u->getRE( );
				else if ( u->isCombatUnit( ) )
					*norm_re += u->getRE( );
			}
		}
	}
}

// calculate attacking artillery/non-artillery RE's
void MapPane::CalcAttArtREs( int x, int y, float* art_re, float* norm_re )
{
	*art_re = *norm_re = 0;

	int d, x2, y2;
	ENUM_DIRS(d)
	{
		ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ );
		AddAttArtREs( x2, y2, oppositeDir( d ), art_re, norm_re );
	}
}

#endif
