#if defined HEXTOOLSPLAY
#include <algorithm>
using std::min;
using std::max;

#include <wx/wx.h>

#include "hextools.h"
using ht::wxS;
using ht::dice;
using ht::mission2str;
using ht::limit;
using ht::coordinatesOutOfBounds;

#include "application.h"
#include "dirs.loop"
#include "selectedunits.loop"
#include "hexunits.loop"
#include "weathercondition.h"
#include "port.h"
#include "city.h"
#include "sideplayer.h"
#include "rulesvariant.h"
#include "rules.h"
#include "phase.h"
#include "aircombatresult.h"
#include "hexcontents.h"
#include "counter.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "groundunitsubtype.h"
#include "groundunit.h"
#include "airunittype.h"
#include "airunit.h"
#include "navalunittype.h"
#include "navalunit.h"
#include "facility.h"
#include "statusmarker.h"
#include "hexnote.h"
#include "hexcontentspane.h"
#include "dairunitbombingmission.h"
#include "dairunitcombat.h"
#include "daafire.h"
#include "dairunitoperation.h"
#include "daircombatdice.h"
#include "dairunitbombingtargets.h"

extern char msg[10000];
extern const char* _month_string[12];
extern const char* _turn_numeral[20];
extern Counter* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _current_player;
extern int _phasing_player;
extern int _current_phase;						// COMBAT_PHASE etc.
extern int _weather_condition[];				// W_MUD etc., per zone (frame.cpp)
extern int _year;
extern int _month;
extern int _turn;

extern Rules _rule_set;

extern AirMissionDlgXfer _air_mission_dialog_data;
extern AADlgXfer _aa_dialog_data;
extern TakeOffDlgXfer _airunit_takeoff_dialog_data;

// Variant A ACRTs
#if 0
extern const char air_crt_elim[17][13];
extern const char air_crt_thru[17][13];
extern const char air_crt_min_loss[17][13];
#endif

bool MapPane::Takeoff( bool staging )
{
	DAirUnitOperation dlg( this );

	int pp = ( _current_player == _phasing_player );
	int np = pp ? 0 : 1;

	// if all units are flying no need to takeoff
	Counter* cntr;
	ENUM_SELUNITS(cntr)
		if ( ! cntr->getInAir( ) )
			break;

	if ( ! cntr )
		return true;

	// old system: nonphasing can takeoff only in reaction phase
	if ( _rule_set.OnDemandAirMissions == FALSE && np && _current_phase != Phase::REACTION_PHASE )
	{
		wxMessageBox( wxT("NOTE:\n"
				"You are using traditional air mission rules, non-phasing\n"
				"player can only fly patrol attacks and air unit escape\n"
				"transfer missions\n") );
		//return false;
	}

	cntr = _selected_unit_list;

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( ! cntr )
		return false;  // bug catcher, shouldn't happen TODO: assert this

	char tmp[200];
	int cnt = 0;
	int non_F = 0;
	AirUnit* au = nullptr;

	for ( cntr = _selected_unit_list; cntr; cntr = cntr->getNext( ) )
	{
		if ( cntr->getCounterType( ) != Counter::Type::PLANE )
		{
			cntr->getUnitString( tmp );
			sprintf( msg, "%s can't fly!", tmp );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
			return false;
		}

		au = (AirUnit*)cntr;

		if ( au->getInAir( ) )  // it's already flying, no need to takeoff
			continue;

		if ( au->getType( ) != AirUnitType::F && au->getType( ) != AirUnitType::HF )
			++non_F;  // no. of non- fighter units

		// GT and GHT are not counted:
		if ( au->getType( ) != AirUnitType::GT && au->getType( ) != AirUnitType::GHT )
			++cnt;

		if ( ! au->isActive( ) )
		{
			au->getUnitString( tmp );
			sprintf( msg, "%s is inoperative!", tmp );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
			return false;
		}

		if ( au->getMP( ) == 0 )
		{  // no MPs left, stay in ground
			cntr->getUnitString( tmp );
			sprintf( msg, "%s has no movement points left!", tmp );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
			return false;
		}

		if ( au->getAborted( ) )
		{
			au->getUnitString( tmp );
			sprintf( msg, "%s is aborted!", tmp );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
			return false;
		}

		if ( au->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("Wrong side") );
			return false;
		}
	}

	// reaction phase: non-phasing player flies interceptors but no other
	// missions (react phase: must be old system)
	// Variant A all planes can takeoff in Reaction Phase

	if ( _current_phase == Phase::REACTION_PHASE && non_F )
	{
		wxMessageBox( wxT("Only fighters can takeoff in the reaction phase!") );
		return false;
	}

	// check af capacity (if using old system)
	if ( _rule_set.NewAFCapacitySystem == FALSE && GetCurrentAFCapacity( x, y ) < cnt )
	{
		sprintf( msg, "There is not enough airfield capacity here\n"
				"(%d required, %d available)\n\n"
				"Still takeoff?", cnt, GetCurrentAFCapacity( x, y ) );
		if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
			return false;
	}
	// if using old air system no takeoffs in expl phase
	if ( _rule_set.OnDemandAirMissions == FALSE && _current_phase == Phase::EXPLOITATION_PHASE && cnt )
	{
		if ( wxMessageBox( wxT("No takeoffs allowed in the exploitation phase when\n"
				"using traditional air mission rules\n\n"
				"Still takeoff?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
			return false;
	}
	// DAirUnitOperation:
	_airunit_takeoff_dialog_data.rdBomb = _airunit_takeoff_dialog_data.rdCAP = _airunit_takeoff_dialog_data.rdEsc = _airunit_takeoff_dialog_data.rdInt = _airunit_takeoff_dialog_data.rdTranfer = _airunit_takeoff_dialog_data.rdStage = 0;

	if ( dlg.ShowModal( ) != wxID_OK )
		return false;
	dlg.CloseDialog( );

	int mis;

	if ( _airunit_takeoff_dialog_data.rdBomb )
		mis = AirUnit::Mission::ANYBOMB;
	else if ( _airunit_takeoff_dialog_data.rdCAP )
		mis = AirUnit::Mission::CAP;
	else if ( _airunit_takeoff_dialog_data.rdEsc )
		mis = AirUnit::Mission::ESCORT;
	else if ( _airunit_takeoff_dialog_data.rdInt )
		mis = AirUnit::Mission::INTERCEPT;
	else if ( _airunit_takeoff_dialog_data.rdTranfer )
		mis = AirUnit::Mission::TRANSFER;
	else if ( _airunit_takeoff_dialog_data.rdStage )
		mis = AirUnit::Mission::STAGE;
	else
	{
		wxMessageBox( wxT("You must select mission type"), Application::NAME );
		return false;
	}

	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getInAir( ) )
			continue;

		au = static_cast<AirUnit*>( cntr );
		// warn if planes in naval patrol trying to do something else:
		if ( au->getNavalPatrol( ) )
		{
			au->getUnitString( tmp );
			if ( mis != AirUnit::Mission::ANYBOMB )
			{
				sprintf( msg, "%s is assigned to naval patrol, "
						"still takeoff with this mission?", tmp );
				if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
					return false;
			}
			else
			{
				sprintf( msg, "NOTE: %s is assigned to naval patrol", tmp );
				wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
			}
		}
		switch ( mis )
		{
			case AirUnit::Mission::STAGE:
				// too many stagings are monitored elsewhere, no need to whine here
				/*
				 if (au->GetStaged()) {
				 wxMessageBox("Planes may stage only once");
				 return FALSE;
				 }
				 */
				break;
		}
		if ( _airunit_takeoff_dialog_data.chkExtRange && mis == AirUnit::Mission::INTERCEPT )
		{
			wxMessageBox( wxT("Can't fly interception missions at extended range") );
			return false;
		}
		// NOTE: tactical bombing not allowed at night...
		if ( _airunit_takeoff_dialog_data.chkNight && au->getPrefix( ) != AirUnitType::Prefix::N && mis == AirUnit::Mission::ESCORT )
		{
			wxMessageBox( wxT("Can't fly escort missions at night") );
			return false;
		}
	}
	// all units were planes and able to takeoff, so takeoff
	for ( cntr = _selected_unit_list; cntr; cntr = cntr->getNext( ) )
	{
		if ( cntr->getInAir( ) )
			continue;
		au = (AirUnit*)cntr;
		switch ( mis )
		{
			case AirUnit::Mission::TRANSFER:
				cntr->setMP( cntr->getMaxMP( ) * 3 );
				break;
			case AirUnit::Mission::STAGE:  // rules affect staging range:
				cntr->setMP( ( cntr->getMaxMP( ) * ( _rule_set.StageRange / 50 ) ) / 2 );
				break;
			case AirUnit::Mission::INTERCEPT:
				cntr->setMP( cntr->getMaxMP( ) / 2 );
				break;
				// DAS in combat phase: 1/2 range:
			case AirUnit::Mission::ANYBOMB:
				if ( _rule_set.OnDemandAirMissions == TRUE && np && _current_phase == Phase::COMBAT_PHASE )
				{
					// v2.2: DAS range only halved on B's, T's and HT's
					if ( au->getType( ) == AirUnitType::B || au->getType( ) == AirUnitType::T || au->getType( ) == AirUnitType::HT )
						cntr->setMP( cntr->getMaxMP( ) / 2 );
				}
				break;
		}
		// jos ext range: mp *= 2, str /= 3
		if ( _airunit_takeoff_dialog_data.chkExtRange )
		{
			au->setMP( au->getMaxMP( ) * 2 );
			au->setExtRange( TRUE );
		}
		if ( _airunit_takeoff_dialog_data.chkNight )
			au->setNight( TRUE );

		// jettison?
		if ( _airunit_takeoff_dialog_data.chkJettison )
			( (AirUnit*)cntr )->setJettison( TRUE );
		else
			( (AirUnit*)cntr )->setJettison( FALSE );

		cntr->setMission( mis );
		cntr->setInAir( 1 );
		if ( ! staging )
			( (AirUnit*)cntr )->setHomeHex( x, y );
	}

	if ( _rule_set.NewAFCapacitySystem == FALSE )
	{
		if ( GetCurrentAFCapacity( x, y ) < cnt )  // never below zero...
			cnt = GetCurrentAFCapacity( x, y );
		sit_[y][x].setAFUsage( sit_[y][x].getAFUsage( ) + cnt );
	}
	contents_pane_->Refresh( );
	// clkmrch: full repaint must be allowed or takeoff dlg won't be erased
	if ( click_marching_mode_ == TRUE )
	{
		// TODO: what does this 2 do, vice setting it to 1?
		dont_redraw_map_ = 2;
		Refresh( );
	}

	return true;
}

void MapPane::Stage( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	int cnt = 0;
	Counter* cntr;
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getCounterType( ) != Counter::Type::PLANE || cntr->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("Only your own planes can stage") );
			return;
		}
		++cnt;
	}

	// in new system no af capacity is needed in staging
	if ( _rule_set.NewAFCapacitySystem == FALSE && GetCurrentAFCapacity( x, y ) < cnt )
	{
		sprintf( msg, "There is not enough airfield capacity here\n"
				"(%d required, %d available)\n\nStill stage?", cnt, GetCurrentAFCapacity( x, y ) );
		if ( wxMessageBox( wxS( msg ), Application::NAME, wxICON_QUESTION | wxYES_NO) != wxYES )
			return;
	}

	char s[200];
	ENUM_SELUNITS(cntr)
		if ( ( (AirUnit*)cntr )->getFlag( AirUnit::STAGED ) )
		{
			cntr->getUnitString( s );
			sprintf( msg, "The following unit has already staged:\n%s\n\nReally stage again?", s );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxICON_QUESTION | wxYES_NO) != wxYES )
				return;
		}
	ENUM_SELUNITS(cntr)
	{
		cntr->setInAir( FALSE );
		cntr->setMP( cntr->getMaxMP( ) );
	}
	if ( ! Takeoff( true ) )
		return;
	// TakeOff() decreases af capacity if rules require so
#if 0
	if ( ! RuleSet.NewAFCapacitySystem )
	  Sit[y][x].SetAFUsage( Sit[y][x].GetAFUsage() + cnt );
#endif
	ENUM_SELUNITS(cntr)
	{
		( (AirUnit*)cntr )->recordReplay( ht::Replay::Movement::STAGE );
		( (AirUnit*)cntr )->setFlag( AirUnit::STAGED );
		cntr->setMP( cntr->getMaxMP( ) );
	}
	wxMessageBox( wxT("Staging done, range is now back at maximum"), Application::NAME );
	contents_pane_->Refresh( );
}

void MapPane::CancelAirattack( )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) || ! sit_[y][x].isAirTarget( ) )
		return;

	if ( wxMessageBox( wxT("Do you really wish to cancel all air operations in the selected hex?"), Application::NAME, wxYES_NO ) != wxYES )
		return;

	sit_[y][x].setAirTarget( false );

	// clear mission from all selected air units
	Counter* cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE )
			cntr->setMission( 0 );
		cntr = cntr->getNext( );
	}

	// clear mission from all air units in selected hex
	cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE )
			cntr->setMission( 0 );
		cntr = cntr->getNext( );
	}

	contents_pane_->Refresh( );
	PaintHex( x, y );
}

static void init_mission_list( int* list )
{
	for ( int i = 0; i < AirUnit::Mission::LAST_MISSION; ++i )
		list[i] = -1;
}

// add to the list and return the number of missions in the list
static int add_to_mission_list( int mis, int* list )
{
	int i;
	for ( i = 0; i < AirUnit::Mission::LAST_MISSION; ++i )
	{
		if ( list[i] < 0 )
			break;
	}

	int cnt = i;
	for ( i = 0; i < AirUnit::Mission::LAST_MISSION; ++i )
	{
		if ( list[i] < 0 ) // doesn't exist, add
		{  
			list[i] = mis;
			return cnt + 1;
		}
		if ( list[i] == mis ) // exists, return their number
			return cnt;
	}

	return cnt;
}

void MapPane::AirCombat( )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( x < 0 || y < 0 )
		return;

	DAirUnitCombat dlg( this );

	int mission_list[AirUnit::Mission::LAST_MISSION + 1];
	int ax_l = 0, al_l = 0, fighters, bombers = 0, escorts = 0;  // for combat report
	int night_mission = 0, night_fighters = TRUE, idx, e_cnt = 0, i_cnt = 0;
	char s[100], b_s[5000], e_s[5000], i_s[5000], axk_s[5000], alk_s[5000];
	wxString ws[20];  // max this many missions in one hex -> ok???

	strcpy( msg, "" );
	strcpy( b_s, "" );
	strcpy( e_s, "" );
	strcpy( i_s, "" );
	strcpy( axk_s, "" );
	strcpy( alk_s, "" );

	Counter* cntr;
	ENUM_SELUNITS(cntr)
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("Wrong side"), Application::NAME );
			return;
		}

	Intercept( );  // set selected fighters to intercept mission

	int i = 0;
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)cntr )->getMission( ) == AirUnit::Mission::INTERCEPT )
			++i;

		if ( cntr->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)cntr )->getMission( ) == AirUnit::Mission::INTERCEPT && ( (AirUnit*)cntr )->getPrefix( ) != AirUnitType::Prefix::N )
			night_fighters = FALSE;
	}
	if ( ! i )  // no interceptors
		return;
	fighters = i;
	/*
	 - put all the different missions into the list
	 - show dlg of which missions in the list
	 - put in dlg only those bombers with the chosen mission and those
	 escorts which escort the chosen mission and the chosen interceptors
	 */
	i = 0;
	init_mission_list( mission_list );
	ENUM_HEXUNITS(x,y,cntr)
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getSide( ) != _current_player && cntr->getMission( ) < AirUnit::Mission::ESCORT && cntr->getMission( ) != AirUnit::Mission::NONE && ! ( (AirUnit*)cntr )->getFlag( AirUnit::FOUGHT ) )
			i = add_to_mission_list( cntr->getMission( ), mission_list );
	}
	if ( ! i )
	{  // no planes to intercept
		wxMessageBox( wxT("There are no planes that could be intercepted"), Application::NAME );
		return;
	}
	if ( i > 1 )
	{
		for ( int j = 0; j < i; ++j )
		{
			sprintf( msg, "Planes with '%s' mission", mission2str( mission_list[j] ) );
			ws[i] = wxS( msg );
#if 0
			tdlg.AddTargetTxt( msg );
#endif
		}
#if 0
		if ( tdlg.ShowModal( ) == FALSE )
			return;
#endif
		idx = wxGetSingleChoiceIndex( wxT("Select target"), Application::NAME, i, ws, this );
		if ( idx < 0 )
			return;
	}
	else
		idx = 0;
#if 0
	wxMessageBox( mission2str( mission_list[tdlg.GetTargetIdx( )] ) );
	UnitWin->DeselectAll( );
#endif
	if ( ! sit_[y][x].unit_list_ )
		return;

	for ( cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
		if ( cntr->getCounterType( ) == Counter::Type::PLANE )
		{  // only planes!
			if ( cntr->getSide( ) != _current_player )
			{
				// escorts: any general escort or escort dedicated for this mission
				if ( cntr->getMission( ) == AirUnit::Mission::ESCORT || cntr->getMission( ) == AirUnit::Mission::ESCORT + mission_list[idx] )
				{
					++escorts;
					dlg.AddEscort( (AirUnit*)cntr );
					cntr->getSymbolString( s );
					strcat( e_s, s );
					strcat( e_s, " " );
				}
				else if ( cntr->getMission( ) == mission_list[idx] )
				{
					// F's and HF's may jettison their bombs
					if ( ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F
						|| ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF ) && ( (AirUnit*)cntr )->getJettison( ) )
					{
						cntr->getSymbolString( s );
						// jettison: may or may not become escorts:
						if ( _rule_set.JettisonEsc == TRUE )
						{
							sprintf( msg, "%s jettisons its bombs and becomes an escort!", s );
							wxMessageBox( wxS( msg ), Application::NAME );
							( (AirUnit*)cntr )->setMission( AirUnit::Mission::ESCORT );
							++escorts;
							dlg.AddEscort( (AirUnit*)cntr );
							( (AirUnit*)cntr )->setJettisoned( 1 );
							cntr->getSymbolString( s );
							strcat( e_s, s );
							strcat( e_s, " " );
							++e_cnt;
							if ( e_cnt == 4 )
							{
								strcat( e_s, "\n" );
								e_cnt = 0;
							}
						}
						else
						{
							sprintf( msg, "%s jettisons its bombs (but stays in mission force)!", s );
							wxMessageBox( wxS( msg ), Application::NAME );
							++bombers;
							dlg.AddTarget( (AirUnit*)cntr );
							( (AirUnit*)cntr )->setJettisoned( 1 );
							cntr->getUnitString( s );  // includes mission type
							strcat( b_s, s );
							strcat( b_s, " " );
						}
					}
					else
					{
						if ( ( (AirUnit*)cntr )->getNight( ) )
							++night_mission;
						++bombers;
						dlg.AddTarget( (AirUnit*)cntr );
						cntr->getUnitString( s );  // includes mission type
						strcat( b_s, s );
						strcat( b_s, "\n" );  // newline because missions take spc
					}
				}
			}
#if 0
			else if ( cntr->GetMission() == AirUnit::Mission::INTERCEPT )
				dlg.AddInterceptor( static_cast<AirUnit*>( cntr ) );
#endif
		}
	if ( night_mission && escorts )
		wxMessageBox( wxT("NOTE: night mission has escorts, this is illegal, shame on you!"), Application::NAME );
	if ( night_mission && night_mission != bombers )
		wxMessageBox( wxT("NOTE: some bombers are flying day missions and some\n"
				"night missions, this is illegal!"), Application::NAME );
	if ( night_mission && !night_fighters )
	{
		wxMessageBox( wxT("Only night fighters may intercept night missions"), Application::NAME );
		return;
	}
	// add interceptors
	ENUM_SELUNITS(cntr)
	{
		dlg.AddInterceptor( (AirUnit*)cntr );
		cntr->getSymbolString( s );
		strcat( i_s, s );
		strcat( i_s, " " );
		++i_cnt;
		if ( i_cnt == 4 )
		{
			strcat( i_s, "\n" );
			i_cnt = 0;
		}
	}
	if ( dlg.ShowModal( ) != wxOK )
		return;

	// build fancy report:
	contents_pane_->deselectAll( );
	contents_pane_->clear( );
	for ( i = 0; i < dlg.num_killed_planes_; ++i )
	{
		if ( dlg.killed_plane_list_[i]->getSide( ) == SidePlayer::AXIS )
		{
			++ax_l;
			dlg.killed_plane_list_[i]->getSymbolString( s );
			strcat( axk_s, s );
			strcat( axk_s, " " );
		}
		else
		{
			++al_l;
			dlg.killed_plane_list_[i]->getSymbolString( s );
			strcat( alk_s, s );
			strcat( alk_s, " " );
		}
		DestroyUnit( dlg.killed_plane_list_[i] );
	}

	// add combat report to the hex
	sprintf( msg, "Turn: %s %s %d\n"
			"%d %s fighter intercepted:\n%s\n\n"
			"%d %s planes:\n%s\n%d escorts\n%s\n"
			"\n"
			"Axis lost %d planes:\n%s\n"
			"Allied lost %d planes:\n%s", _month_string[_month], _turn_numeral[_turn - 1], _year, fighters,
			_current_player == SidePlayer::AXIS ? "Axis" : "Allied", i_s, bombers, _current_player == SidePlayer::AXIS ? "Allied" : "Axis", b_s, escorts, e_s, ax_l, axk_s, al_l, alk_s );

	HexNote* combat_report = new HexNote( HexNote::AIRREPORT );
	combat_report->setCaption( "Aircombat rep" );
	combat_report->setText( msg );
	insertUnitToCurrentHex( combat_report );
}

void MapPane::Intercept( )
{
	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;
	if ( x < 0 || y < 0 )
		return;

	if ( ! _selected_unit_list )
	{
		wxMessageBox( wxT("First select interceptors you wish to use in the air combat!"), Application::NAME );
		return;
	}
	// any enemy planes to be intercepted?
	Counter* cntr;
	for ( cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getSide( ) != _current_player )
			break;

	if ( ! cntr )
	{
		wxMessageBox( wxT("There are no enemy planes to be intercepted here!"), Application::NAME );
		return;
	}
	// are all selected counter flying interceptors?
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getCounterType( ) != Counter::Type::PLANE )
		{
			wxMessageBox( wxT("Only planes can intercept enemy planes!"), Application::NAME );
			return;
		}
		if ( cntr->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("Select your own planes!"), Application::NAME );
			return;
		}
		if ( ( (AirUnit*)cntr )->getInAir( ) == FALSE )
		{
			wxMessageBox( wxT("All your planes must takeoff first!"), Application::NAME );
			return;
		}
		if ( ( (AirUnit*)cntr )->getFlag( AirUnit::FOUGHT ) )
		{
			char s[100];
			cntr->getUnitString( s );
			sprintf( msg, "The following unit has already fought in air combat:\n%s", s );
			wxMessageBox( wxS( msg ), Application::NAME );
			return;
		}
	}

	ENUM_SELUNITS(cntr)
	{
		cntr->setMission( AirUnit::Mission::INTERCEPT );
	}
	contents_pane_->Refresh( );
}

// assign mission to selected planes
void MapPane::AirMission( )
{
	// check side etc...
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	char s[100];
	Counter* cntr;
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("Wrong side") );
			return;
		}
		if ( cntr->getCounterType( ) != Counter::Type::PLANE )
		{
			wxMessageBox( wxT("Only planes can carry out air missions!") );
			return;
		}
		// aborted planes?
		if ( ! ( (AirUnit*)cntr )->getInAir( ) )
		{
			wxMessageBox( wxT("Planes must fly in order to bomb") );
			return;
		}
		// aborted planes?
		if ( ( (AirUnit*)cntr )->getAborted( ) )
		{
			wxMessageBox( wxT("Aborted plane can't bomb") );
			return;
		}
		// mission is set to AirUnit::Mission::BOMB at takeoff, cleared if returned
		if ( ( (AirUnit*)cntr )->getMission( ) == AirUnit::Mission::NONE )
		{
			cntr->getUnitString( s );
			sprintf( msg, "These planes are on their way home:\n%s\n\n"
						"This is probably because they have already completed their mission\n"
						"Are you sure you want to assign new mission to them?", s );
			if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO ) != wxYES )
				return;
		}
	}
	// interception is special kind of mission
	if ( _current_phase == Phase::REACTION_PHASE )
	{
		Intercept( );
		return;
	}
	cntr = _selected_unit_list;
	_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdGS = _air_mission_dialog_data.rdAirBase = _air_mission_dialog_data.rdRailLine = _air_mission_dialog_data.rdHarassment = _air_mission_dialog_data.rdNaval = _air_mission_dialog_data.rdDAS = _air_mission_dialog_data.rdPlanes = _air_mission_dialog_data.rdHarbour = _air_mission_dialog_data.rdTransport = _air_mission_dialog_data.rdAirDrop = 0;
	switch ( cntr->getMission( ) )
	{
		case AirUnit::Mission::BOMB_GS:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdGS = true;
			break;
		case AirUnit::Mission::BOMB_AF:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdAirBase = true;
			break;
		case AirUnit::Mission::BOMB_RR:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdRailLine = true;
			break;
		case AirUnit::Mission::BOMB_HARASS:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdHarassment = true;
			break;
		case AirUnit::Mission::BOMB_NAVAL:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdNaval = true;
			break;
		case AirUnit::Mission::BOMB_DAS:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdDAS = true;
			break;
		case AirUnit::Mission::BOMB_PLANES:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdPlanes = true;
			break;
		case AirUnit::Mission::BOMB_PORT:
			_air_mission_dialog_data.rdBombing = _air_mission_dialog_data.rdHarbour = true;
			break;
		case AirUnit::Mission::BOMB_CF: // TODO: what?
			_air_mission_dialog_data.rdBombing = true;
			_air_mission_dialog_data.rdCoastalDef = 1;
			break;
		case AirUnit::Mission::BOMB_OTHER: // TODO: what?
			_air_mission_dialog_data.rdBombing = true;
			_air_mission_dialog_data.rdTacOther = 1;
			break;
		case AirUnit::Mission::NAVALPATROL: // TODO: what?
			_air_mission_dialog_data.rdBombing = true;
			_air_mission_dialog_data.rdNavalPatrol = 1;
			break;
		case AirUnit::Mission::MINELAYING:
			_air_mission_dialog_data.rdMineLaying = _air_mission_dialog_data.rdTransport = true;
			break;
		case AirUnit::Mission::TRANSPORT:
			_air_mission_dialog_data.rdRegular = _air_mission_dialog_data.rdTransport = true;
			break;
		case AirUnit::Mission::AIRDROP:
			_air_mission_dialog_data.rdTransport = _air_mission_dialog_data.rdAirDrop = 1;
			break;
	}

	if ( cntr->getMission( ) == AirUnit::Mission::NAVALPATROL && _current_phase != Phase::INITIAL_PHASE )
		wxMessageBox( wxT("This air group has been assigned to naval patrol,\n"
						"this will cancel the naval patrol."), Application::NAME );

	DAirUnitBombingMission dlg( this );
	int mis = AirUnit::Mission::NONE;
	int rq_type = 0;
	if ( cntr->getMission( ) == AirUnit::Mission::ESCORT )
	{
		mis = GetEscortMission( );
	}
	else
	{
		// bombing/transport target
		if ( dlg.ShowModal( ) != wxID_OK )
			return;

		dlg.CloseDialog( );

		if ( _air_mission_dialog_data.rdBombing )
		{
			if ( _air_mission_dialog_data.rdGS )
				mis = AirUnit::Mission::BOMB_GS;
			if ( _air_mission_dialog_data.rdAirBase )
				mis = AirUnit::Mission::BOMB_AF;
			if ( _air_mission_dialog_data.rdRailLine )
				mis = AirUnit::Mission::BOMB_RR;
			if ( _air_mission_dialog_data.rdHarassment )
				mis = AirUnit::Mission::BOMB_HARASS;
			if ( _air_mission_dialog_data.rdNaval )
				mis = AirUnit::Mission::BOMB_NAVAL, rq_type = Counter::Type::SHIP;
			if ( _air_mission_dialog_data.rdDAS )
				mis = AirUnit::Mission::BOMB_DAS;
			if ( _air_mission_dialog_data.rdPlanes )
				mis = AirUnit::Mission::BOMB_PLANES, rq_type = Counter::Type::PLANE;
			if ( _air_mission_dialog_data.rdHarbour )
				mis = AirUnit::Mission::BOMB_PORT;
			if ( _air_mission_dialog_data.rdCoastalDef )
				mis = AirUnit::Mission::BOMB_CF;
			if ( _air_mission_dialog_data.rdNavalPatrol )
				mis = AirUnit::Mission::NAVALPATROL;
			if ( _air_mission_dialog_data.rdTacOther )
				mis = AirUnit::Mission::BOMB_OTHER;
		}
		if ( _air_mission_dialog_data.rdStratBomb )
		{
			if ( _air_mission_dialog_data.rdRailMarshal )
				mis = AirUnit::Mission::RAILMARSHAL;
			else if ( _air_mission_dialog_data.rdStratPort )
				mis = AirUnit::Mission::STRAT_PORT;
			else
				mis = AirUnit::Mission::STRAT_OTHER;
		}
		if ( _air_mission_dialog_data.rdTransport )
		{
			if ( _air_mission_dialog_data.rdAirDrop )
				mis = AirUnit::Mission::AIRDROP;
			else if ( _air_mission_dialog_data.rdMineLaying )
				mis = AirUnit::Mission::MINELAYING;
			else
				mis = AirUnit::Mission::TRANSPORT;
		}
	}
	if ( rq_type )
	{
		for ( cntr = sit_[selected_hex_point_.y][selected_hex_point_.x].unit_list_; cntr; cntr = cntr->getNext( ) )
		{
			if ( cntr->getCounterType( ) == rq_type && cntr->getSide( ) != _current_player )
				break;
		}
	}
	if ( ! cntr )
	{
		wxMessageBox( wxT("There are no targets in here!") );
		return;
	}
	// record missions for combat phase
	for ( cntr = _selected_unit_list; cntr; cntr = cntr->getNext( ) )
	{
		cntr->setMission( mis );
		if ( _air_mission_dialog_data.chkJettison )
			( (AirUnit*)cntr )->setJettison( TRUE );

		// if plane has already been ordered to jettison bombs DON'T REMOVE ORDER
#if 0
		else
			((AirUnit*)cntr)->SetJettison( FALSE );
#endif
	}
	// mark as target hex
	sit_[selected_hex_point_.y][selected_hex_point_.x].setAirTarget( true );
	paintCurrentHex( );
	contents_pane_->Refresh( );
#if 0
	if ( RuleSet.OnDemandAirMissions )
	{
		if ( Player == RealPlayer )
			wxMessageBox( "Non-phasing player should now fly interceptors", Application::NAME, wxOK | wxICON_INFORMATION );
		else
			wxMessageBox( "Phasing player should now fly interceptors", Application::NAME, wxOK | wxICON_INFORMATION );
	}
#endif
}

// return type of the escort mission
int MapPane::GetEscortMission( )
{
	Counter* cntr;
	DAirUnitCombat dlg( this );
	int x, y, i, j;
	int mission_list[AirUnit::Mission::LAST_MISSION + 1], idx;
	wxString ws[20];

	x = selected_hex_point_.x;
	y = selected_hex_point_.y;
	if ( x < 0 || y < 0 )
		return AirUnit::Mission::NONE;
	i = 0;
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE
			&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F || ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF ) )
			++i;
	}
	if ( ! i )  // no fighters
		return AirUnit::Mission::NONE;
	// - put all the different missions into the list
	// - show dlg in which missions in the list
	// - return the escort mission for the chosen mission
	init_mission_list( mission_list );
	ENUM_HEXUNITS(x,y,cntr)
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getSide( ) == _current_player && cntr->getMission( ) != AirUnit::Mission::NONE && cntr->getMission( ) < AirUnit::Mission::ESCORT )
			i = add_to_mission_list( cntr->getMission( ), mission_list );
	}

	if ( ! i )  // no planes to escort
		return AirUnit::Mission::NONE;

	if ( i > 1 )
	{
		for ( j = 0; j < i; ++j )
		{
			sprintf( msg, "Planes with '%s' mission", mission2str( mission_list[j] ) );
			ws[i] = wxS( msg );
			//tdlg.AddTargetTxt(msg);
		}
		//tdlg.ShowModal();
		idx = wxGetSingleChoiceIndex( wxT("Select target"), Application::NAME, i, ws, this );
		if ( idx < 0 )
			idx = 0;  // FIXME TODO "Cancel" not handled!
		return AirUnit::Mission::ESCORT + mission_list[idx];  //mission_list[tdlg.GetTargetIdx()];
	}
	return AirUnit::Mission::ESCORT + mission_list[0];
	//wxMessageBox(mission2str(mission_list[tdlg.GetTargetIdx()]));
}

void MapPane::AssignNavalPatrol( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) != Counter::Type::PLANE )
		{
			wxMessageBox( wxT("Only planes can fly naval patrol"), Application::NAME );
			break;
		}
		( (AirUnit*)c )->setNavalPatrol( TRUE );
	}

	contents_pane_->Refresh( );
}

// AA retults (from AATable(int aa, AirUnit* plane) in mappane2.cpp)
#define AA_MISS			AirCombatResult::MISS
#define AA_RETURN		AirCombatResult::RETURN
#define AA_ABORT		AirCombatResult::ABORT
#define AA_KILL			AirCombatResult::KILL

// corresponding string (from AA2Str())
#define AA_MISS_STR		"miss"
#define AA_RETURN_STR	"return"
#define AA_ABORT_STR	"abort"
#define AA_KILL_STR		"kill"

static const char* AA2Str( int aa_result )
{
	switch ( aa_result )
	{
		case AA_MISS:
			return AA_MISS_STR;
		case AA_RETURN:
			return AA_RETURN_STR;
		case AA_ABORT:
			return AA_ABORT_STR;
		case AA_KILL:
			return AA_KILL_STR;
		default:
			return "bug found!";
	}
}

// return AA result for given plane/die/mods
static int AATable( int aa_str, int np1, int np2, int mods )
{
	int n = np1 + np2;
	n += mods;
	n = max( n, 1 );
	n = min( n, 12 );
	if ( _rule_set.NewAAMods == TRUE )
	{  // GE table
		switch ( aa_str )
		{
			case 0:
				return AA_MISS;
			case 1:
				if ( n <= 2 ) return AA_ABORT;
				if ( n <= 3 ) return AA_RETURN;
				return AA_MISS;
			case 2:
				if ( n <= 3 ) return AA_ABORT;
				if ( n <= 4 ) return AA_RETURN;
				return AA_MISS;
			case 3:
			case 4:
				if ( n <= 3 ) return AA_ABORT;
				if ( n <= 5 ) return AA_RETURN;
				return AA_MISS;
			case 5:
			case 6:
				if ( n <= 4 ) return AA_ABORT;
				if ( n <= 5 ) return AA_RETURN;
				return AA_MISS;
			case 7:
			case 8:
			case 9:
				if ( n <= 4 ) return AA_ABORT;
				if ( n <= 6 ) return AA_RETURN;
				return AA_MISS;
			case 10:
			case 11:
			case 12:
				if ( n <= 1 ) return AA_KILL;
				if ( n <= 5 ) return AA_ABORT;
				if ( n <= 6 ) return AA_RETURN;
				return AA_MISS;
			case 13:
			case 14:
			case 15:
			case 16:
				if ( n <= 2 ) return AA_KILL;
				if ( n <= 5 ) return AA_ABORT;
				if ( n <= 7 ) return AA_RETURN;
				return AA_MISS;
			default:  // >=17
				if ( n <= 3 ) return AA_KILL;
				if ( n <= 6 ) return AA_ABORT;
				if ( n <= 7 ) return AA_RETURN;
				return AA_MISS;
		}
	}
	else
	{  // SE table
		switch ( aa_str )
		{
			case 0:
				return AA_MISS;
			case 1:
				if ( n <= 2 ) return AA_ABORT;
				if ( n <= 4 ) return AA_RETURN;
				return AA_MISS;
			case 2:
				if ( n <= 3 ) return AA_ABORT;
				if ( n <= 5 ) return AA_RETURN;
				return AA_MISS;
			case 3:
			case 4:
				if ( n <= 3 ) return AA_ABORT;
				if ( n <= 6 ) return AA_RETURN;
				return AA_MISS;
			case 5:
			case 6:
				if ( n <= 4 ) return AA_ABORT;
				if ( n <= 6 ) return AA_RETURN;
				return AA_MISS;
			case 7:
			case 8:
			case 9:
				if ( n <= 4 ) return AA_ABORT;
				if ( n <= 7 ) return AA_RETURN;
				return AA_MISS;
			case 10:
			case 11:
			case 12:
				if ( n <= 1 ) return AA_KILL;
				if ( n <= 5 ) return AA_ABORT;
				if ( n <= 7 ) return AA_RETURN;
				return AA_MISS;
			case 13:
			case 14:
			case 15:
			case 16:
				if ( n <= 2 ) return AA_KILL;
				if ( n <= 5 ) return AA_ABORT;
				if ( n <= 8 ) return AA_RETURN;
				return AA_MISS;
			default:  // >=17
				if ( n <= 3 ) return AA_KILL;
				if ( n <= 6 ) return AA_ABORT;
				if ( n <= 8 ) return AA_RETURN;
				return AA_MISS;
		}  // switch
	}  // else
}

// calc AA mods for given plane
static int AAMods( AirUnit* p )
{
	int t = p->getType( );
	int mis = p->getMission( );
	int code = p->getCode( );
	int mods = 0;

	// TODO: fix HB mods someday!!!

	if ( code == AirUnit::Code::V && p->getMission( ) == AirUnit::Mission::BOMB_NAVAL )
		--mods;  // -1 for torpedo bomber at naval unit

	// sf aa mods:
	if ( _rule_set.NewAAMods == TRUE )
	{
		if ( _year >= 44 && _month >= 4 )
			--mods;  // -1 after Apr I 44

		if ( ( t == AirUnitType::B || t == AirUnitType::HT || t == AirUnitType::T )
			&& ( mis == AirUnit::Mission::BOMB_HARASS || mis == AirUnit::Mission::BOMB_GS || mis == AirUnit::Mission::BOMB_DAS || mis == AirUnit::Mission::AIRDROP ) )
			--mods;  // -1 for B/T/HT on GS/DAS/harassment/airdrop

		if ( ( t == AirUnitType::B && code != AirUnit::Code::L )
			&& ( mis != AirUnit::Mission::BOMB_NAVAL && mis != AirUnit::Mission::BOMB_GS && mis != AirUnit::Mission::BOMB_DAS && mis != AirUnit::Mission::BOMB_HARASS
				&& mis != AirUnit::Mission::AIRDROP ) )
			++mods;  // +1 for B not in above missions (no +1 for code L though)

		if ( t == AirUnitType::HB )
			++mods;  // +1 for HB in any mission
	}
	else
	{  // fite/se aa mods:
		if ( t == AirUnitType::B || t == AirUnitType::HB ) // +1 to B/HB always !!!
			++mods;
		if ( t == AirUnitType::D && _year >= 43 ) // -1 to D on/after Jan I 43
			--mods;
	}
	// +1 against rocket or jet planes
	if ( p->getPrefix( ) == AirUnitType::Prefix::J || p->getPrefix( ) == AirUnitType::Prefix::R )
		++mods;

	// +1 for night missions
	if ( p->getNight( ) )
		++mods;

	// V (torpedo bomber) gets -1 against ships
	if ( p->getCode( ) == AirUnit::Code::V && p->getMission( ) == AirUnit::Mission::BOMB_NAVAL )
		--mods;

	return mods;
}

void MapPane::FireAA( void )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( ! sit_[y][x].isAirTarget( ) )
	{
		// TODO: for readability, inline out a message box function call here
		wxMessageBox( wxT("There are no attacking planes here!\n\nNote that you must assign missions to\nplanes before you can fire AA against\nthem (mission affects die modifier)."), Application::NAME );
		return;
	}

	bool das = false;

	// if hex is attacked, check if there are any planes on DAS mission
	if ( IsHexAttacked( x, y ) && CalcDASAA( x, y ) )
	{
		Counter* cntr;
		ENUM_HEXUNITS( x, y, cntr )
			if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) == AirUnit::Mission::BOMB_DAS )
				das = true;
	}

	// if planes on DAS mission, ask whether to fire them
	if ( das && wxMessageBox( wxT("Fire AA against planes flying DAS?\n\n"
								"Note: AA against DAS is fired by attacking units,\n"
								"AA against any other mission is fired by defending units."), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
		das = false;

	int raw_str = ( das ? CalcDASAA( x, y ) : CalcAA( x, y ) );

	// normal AA (not DAS)
	if ( raw_str <= 0 )
	{
		// TODO: for readability, inline out a message box function call here
		wxMessageBox( wxT("There are no antiaircraft guns to fire!"), Application::NAME );
		return;
	}

	// AA can be fired against several missions
	//if ( Sit[y][x].HasFiredAA( ) )
	//{
	//	wxMessageBox("You have already fired AA here!", Application::NAME);
	//	return;
	//}

	int hv_str = CalcHvyAA( x, y );  // no need in DAS (HB can't fly DAS mission)

	int hexowner = sit_[y][x].getOwner( );

	contents_pane_->deselectAll( );

	DAAFire dlg( this );

	for ( int ii = 0; ii < 2; ++ii )
	{
		// two passes:
		//	1st pass: hex units
		//	2nd pass: selected units
		// TODO: after the deselectAll( ) call above, will there ever be a 2nd pass?!?
		Counter* cntr = ( ii ? _selected_unit_list : sit_[y][x].unit_list_ );

		while ( cntr )
		{
			Counter* next = cntr->getNext( );  // in case cntr is destroyed

			// don't fire on escorts (or CAP, or transfer!?!...)
			if ( cntr->getCounterType( ) == Counter::Type::PLANE
				&& ( cntr->getSide( ) != hexowner || cntr->getMission( ) == AirUnit::Mission::BOMB_DAS )
				&& cntr->getMission( ) >= AirUnit::Mission::FIRST_AA && cntr->getMission( ) <= AirUnit::Mission::LAST_AA )
			{
				if ( das && cntr->getMission( ) != AirUnit::Mission::BOMB_DAS )
					goto end;

				if ( ! das && cntr->getMission( ) == AirUnit::Mission::BOMB_DAS )
					goto end;

				AirUnit* plane = (AirUnit*)cntr;
				int mods = AAMods( plane );

				char s[200];
				plane->getUnitString( s );
				sprintf( _aa_dialog_data.lblTarget, "%s", s );
				sprintf( _aa_dialog_data.editStr, "%d", plane->getType( ) == AirUnitType::HB ? hv_str : raw_str );
				sprintf( _aa_dialog_data.editMod, "%d", mods );

				if ( dlg.ShowModal( ) != wxID_OK )
					return;
				dlg.CloseDialog( );

				int str = atoi( _aa_dialog_data.editStr );
				mods = atoi( _aa_dialog_data.editMod );
				int die1 = _aa_dialog_data.rdManu ? limit( atoi( _aa_dialog_data.editDie ), 1, 6 ) : dice( );
				int die2 = _aa_dialog_data.rdManu ? limit( atoi( _aa_dialog_data.editDie2 ), 1, 6 ) : dice( );
				int res = AATable( str, die1, die2, mods );

				if ( mods )
				{
					sprintf( msg, "Firing %d pts AA (die modifier %s%d) against:\n%s\n\n"
									"Result: %d+%d(+%d) = %d --> %s", str, mods >= 0 ? "+" : "", mods, s,
									die1, die2, mods, die1 + die2 + mods, AA2Str( res ) );
				}
				else
				{
					sprintf( msg, "Firing %d pts AA (die modifier %s%d) against:\n%s\n\n"
									"Result: %d+%d = %d --> %s", str, mods >= 0 ? "+" : "", mods, s,
									die1, die2, die1 + die2, AA2Str( res ) );
				}
				wxMessageBox( wxS( msg ), Application::NAME );

				switch ( res )
				{
					case AA_MISS:
						break;
					case AA_ABORT:
						plane->setAborted( TRUE );
						plane->setMission( AirUnit::Mission::NONE );
						plane->setMP( plane->getMaxMP( ) );
						if ( _rule_set.AbortedPlanesToPool == TRUE )
						{
							contents_pane_->removeUnit( plane );
							DestroyUnit( plane );
							plane->getUnitString( s );
							sprintf( msg, "Aborted plane was removed to air replacement pool:\n%s", s );
							wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
						}
						break;
					case AA_RETURN:
						plane->setMission( AirUnit::Mission::NONE );
						plane->setMP( plane->getMaxMP( ) );
						break;
					case AA_KILL:
						contents_pane_->removeUnit( plane );
						plane->getUnitString( s );
						sprintf( msg, "The following plane was shot down:\n%s", s );
						wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
						DestroyUnit( plane );
						break;
					default:
						wxMessageBox( wxT("MapPane::FireAA: bug found!") );
						break;
				}
			}
			end:
			cntr = next;
		}
	}
	contents_pane_->Refresh( );

	sit_[y][x].setFiredAA( true );
}

bool MapPane::IsAxisOwnedAtStart( int x, int y )
{
	return ( sit_[y][x].getOrigOwner( ) == SidePlayer::AXIS );
}

bool MapPane::IsAlliedOwnedAtStart( int x, int y )
{
	return ( sit_[y][x].getOrigOwner( ) == SidePlayer::ALLIED );
}

// Add Allied and Axis intrinsic AA for Cities, Fortresses and Airfields
int MapPane::IntrinsicCityAA( int x, int y )
{
	int aa = 0;
	City* city;
	if ( IsAxisOwned( x, y ) )
	{
		if ( IsAxisOwnedAtStart( x, y ) )
		{
			city = hex_[y][x].city_;
			while ( city )
			{
				switch ( city->type_ )
				{
					case City::Type::MINOR:
						aa += _rule_set.AxisReferenceCityAA;
						break;
					case City::Type::MAJOR:
						aa += _rule_set.AxisDotCityAA;
						break;
					case City::Type::PARTIALHEX:
						aa += _rule_set.AxisPartialCityAA;
						break;
					case City::Type::FULLHEX:
						aa += _rule_set.AxisFullCityAA;
						break;
					// TODO: need to discriminate -map fortress here
					case City::Type::UNUSED_FORTRESS:
						aa += _rule_set.AxisUnImprovedFortressAA;
						break;
					case City::Type::FORTRESS:
						aa += _rule_set.AxisImprovedFortressAA;
						break;
					case City::Type::UNUSED_WESTWALL:
					case City::Type::WESTWALL_1:
					case City::Type::WESTWALL_2:
					case City::Type::WESTWALL_3:
					case City::Type::UNUSED_OUVRAGE:
					case City::Type::OUVRAGE:
						break;
				}
				city = city->getNext( );
			}
		}
	}
	else if ( IsAlliedOwned( x, y ) )
	{
		if ( IsAlliedOwnedAtStart( x, y ) )
		{
			city = hex_[y][x].city_;
			while ( city )
			{
				switch ( hex_[y][x].city_->type_ )
				{
					case City::Type::MINOR:
						aa += _rule_set.AlliedReferenceCityAA;
						break;
					case City::Type::MAJOR:
						aa += _rule_set.AlliedDotCityAA;
						break;
					case City::Type::PARTIALHEX:
						aa += _rule_set.AlliedPartialCityAA;
						break;
					case City::Type::FULLHEX:
						aa += _rule_set.AlliedFullCityAA;
						break;
					// TODO: need to discriminate here -map fortresses
					case City::Type::UNUSED_FORTRESS:
						aa += _rule_set.AlliedUnImprovedFortressAA;
						break;
					case City::Type::FORTRESS:
						aa += _rule_set.AlliedImprovedFortressAA;
						break;
					case City::Type::UNUSED_WESTWALL:
					case City::Type::WESTWALL_1:
					case City::Type::WESTWALL_2:
					case City::Type::WESTWALL_3:
					case City::Type::UNUSED_OUVRAGE:
					case City::Type::OUVRAGE:
						break;
				}
				city = city->getNext( );
			}
		}
	}
	return aa;
}

int MapPane::IntrinsicAirfieldAA( int x, int y )
{
	int aa = 0;
	if ( IsAxisOwned( x, y ) )
	{
		if ( IsAxisOwnedAtStart( x, y ) )
		{
			if ( sit_[y][x].isPermAF( ) )
			{
				int numaf = sit_[y][x].getAFCapacity( 0 ) / 3;
				if ( numaf > 0 )
					aa += numaf * _rule_set.AxisAirfieldAA;
			}
		}
	}
	else if ( IsAlliedOwned( x, y ) )
	{
		if ( IsAlliedOwnedAtStart( x, y ) )
		{
			if ( sit_[y][x].isPermAF( ) )
			{
				int numaf = sit_[y][x].getAFCapacity( 0 ) / 3;
				if ( numaf > 0 )
					aa += numaf * _rule_set.AlliedAirfieldAA;
			}
		}
	}
	return aa;
}

// calc ONLY hvy AA
int MapPane::CalcHvyAA( int x, int y )
{
	float aa = 0;

	Counter* c;
	ENUM_HEXUNITS(x, y, c)
		if ( c->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* u = static_cast<GroundUnit*>( c );
			if ( u->getType( ) == GroundUnitType::WW2::POS_HV_AA || u->getType( ) == GroundUnitType::WW2::HV_AA )
				aa += c->getRealFlak( );
		}

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
		ENUM_SELUNITS(c)
			if ( c->getCounterType( ) == Counter::Type::UNIT )
			{
				GroundUnit* u = (GroundUnit*)c;
				if ( u->getType( ) == GroundUnitType::WW2::POS_HV_AA || u->getType( ) == GroundUnitType::WW2::HV_AA )
					aa += c->getRealFlak( );
			}

	// AA status markers (considered to be heavy)
	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) == Counter::Type::PTS && static_cast<StatusMarker*>( c )->getPtsType( ) == StatusMarker::AA )
			aa += static_cast<StatusMarker*>( c )->getPts( );
	}

	// Add Allied and Axis intrinsic AA for Cities, Fortresses and Airfields
	aa += IntrinsicCityAA( x, y );

	// airfield AA is light AA
	// aa += IntrinsicAirfieldAA(x,y);

	return static_cast<int>( aa );
}

// AA against DAS mission
int MapPane::CalcDASAA( int x, int y )
{
	float aa = 0;
	int hexes = 0;

	int d;
	ENUM_DIRS(d)
	{
		int x2, y2;
		ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ );

		int atkdir = ht::getAdjHexPart( d );
		bool aa_present = false;

		Counter* c;
		ENUM_HEXUNITS(x2, y2, c)
			if ( c->getCounterType( ) == Counter::Type::UNIT && c->getAttackDir( ) == atkdir )
			{
				aa += c->getRealFlak( );
				aa_present = true;
			}

		if ( aa_present )
			++hexes;
	}

	// TODO: need div zero protection
	return static_cast<int>( aa / hexes );
}

int MapPane::CalcAA( int x, int y )
{
	float aa = 0;

	Counter* c = sit_[y][x].unit_list_;
	while ( c )
	{
		aa += c->getRealFlak( );
		c = c->getNext( );
	}

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
	{
		c = _selected_unit_list;
		while ( c )
		{
			aa += c->getRealFlak( );
			c = c->getNext( );
		}
	}

	// aa status markers
	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) == Counter::Type::PTS && ( (StatusMarker*)c )->getPtsType( ) == StatusMarker::AA )
			aa += ( (StatusMarker*)c )->getPts( );
	}

	// Add Allied and Axis intrinsic AA for Cities, Fortresses and Airfields
	aa += IntrinsicCityAA( x, y );
	aa += IntrinsicAirfieldAA( x, y );

	return static_cast<int>( aa );
}

#define BOMB_MISS 1
#define BOMB_HIT  2

static int BombingTable( int str, int die )
{
	switch ( str )
	{
		case 0:
			return BOMB_MISS;  // 0
		case 1:
			if ( die <= 5 ) return BOMB_MISS;
			return BOMB_HIT;  // 1
		case 2:
		case 3:
		case 4:
			if ( die <= 4 ) return BOMB_MISS;
			return BOMB_HIT;  // 2-4
		case 5:
		case 6:
		case 7:
		case 8:
			if ( die <= 3 ) return BOMB_MISS;
			return BOMB_HIT;  // 5-8
		case 9:
		case 10:
		case 11:
		case 12:
			if ( die <= 2 ) return BOMB_MISS;
			return BOMB_HIT;  // 9-12
		case 13:
		case 14:
		case 15:
		case 16:
			if ( die <= 1 ) return BOMB_MISS;
			return BOMB_HIT;  // 13-16
		default:
			if ( die <= 0 ) return BOMB_MISS;
			return BOMB_HIT;  // 17+
	}
}

static int BombingDieNeededToHit( int str )
{
	for ( int i = 1; i <= 6; ++i )
		if ( BombingTable( str, i ) == BOMB_HIT )
			return i;

	return 7;	// TODO:  what magic number is this?
}

void MapPane::ResolveBombing( void )
{
	int x, y, str, res, mission, h, ah, x2, y2, i, np, hits;
	int night, mod = 0;
	float fstr = 0, f;
	Counter* cntr;
	AirUnit* au;
	NavalUnit* ship, *sunken_ship = 0;
	Counter* target = NULL;
	char s[200], rep[1000];
	DAirCombatDice ddlg( this );
	DAirUnitBombingTargets dlg( this );
	HexNote* bombing_report;

	if ( !isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}
	x = selected_hex_point_.x;
	y = selected_hex_point_.y;
	// new system: select mission
	//if (RuleSet.OnDemandAirMissions)
	//  AirMission();
	if ( !sit_[y][x].isAirTarget( ) )
	{
		wxMessageBox( wxT("There are no planes with assigned bombing mission here!\n\n"
				"You must assign mission to your planes\n"
				"before you can resolve one!"), Application::NAME );
		return;
	}
	//if (CalcAA(x, y) > 0 && !Sit[y][x].HasFiredAA()) {
	//	wxMessageBox("You must fire AA before bombing can take place!", Application::NAME);
	//  return;
	//}
	if ( !_selected_unit_list )
	{
		wxMessageBox( wxT("Select bombers that will bomb at this time.\n"
				"NOTE: bombers strength is summed and only one\n"
				"            attack is made."), Application::NAME );
		return;
	}
	cntr = _selected_unit_list;
	// check validity (only own planes must be selected)
	while ( cntr )
	{
		if ( cntr->getCounterType( ) != Counter::Type::PLANE || cntr->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("Please select only your own planes"), Application::NAME );
			return;
		}
		cntr = cntr->getNext( );
	}
	night = ( (AirUnit*)_selected_unit_list )->getNight( );
	mission = _selected_unit_list->getMission( );
	ENUM_SELUNITS (cntr)
	{
		if ( cntr->getMission( ) != mission )
		{
			wxMessageBox( wxT("All planes must have same mission"), Application::NAME );
			return;
		}
		au = (AirUnit*)cntr;
		if ( au->getNight( ) != night )
		{
			wxMessageBox( wxT("You can't combine planes with day mission and planes\n"
					"with night mission"), Application::NAME );
			return;
		}
		// strategic bombing
		if ( mission == AirUnit::Mission::RAILMARSHAL || mission == AirUnit::Mission::STRAT_PORT || mission == AirUnit::Mission::STRAT_OTHER)
		{
			f = au->getStrat( );
			if ( night && au->getPrefix( ) != AirUnitType::Prefix::N )
				f /= 2;
			if ( au->getExtRange( ) )
				f /= 3;
			fstr += f;
		}
		// tactical bombing

		else
		{
			// F's attacking airbase -> +1
			if ( mission == AirUnit::Mission::BOMB_PLANES
				&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F || ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF ) )
			{
				f = au->getTac( ) + 1;
				if ( night && au->getPrefix( ) != AirUnitType::Prefix::N )
					f /= 2;
				if ( au->getExtRange( ) )
					f /= 3;
				fstr += f;
				wxMessageBox( wxT("Fighters strafing airbase: +1 added to bombing strength"), Application::NAME, wxICON_INFORMATION );
			}
			else	// normal tactical
			{
				f = au->getTac( );
				// F that has jettisoned its bombs: reduce by 2/3
				if ( ( au->getType( ) == AirUnitType::F || au->getType( ) == AirUnitType::HF ) && au->getJettisoned( ) )
					f /= 3;
				if ( night && au->getPrefix( ) != AirUnitType::Prefix::N )
					f /= 2;
				if ( au->getExtRange( ) ) f /= 3;
				fstr += f;
			}
		}
	}
	// bombing str halved in bad weather:
	if ( _weather_condition[hex_[y][x].getWeatherZone( )] == WeatherCondition::MUD || _weather_condition[hex_[y][x].getWeatherZone( )] == WeatherCondition::SNOW
		|| _weather_condition[hex_[y][x].getWeatherZone( )] == WeatherCondition::WINTER )
		fstr /= 2;
	str = (int)fstr;
	if ( night && ( mission >= AirUnit::Mission::BOMB_GS && mission <= AirUnit::Mission::BOMB_PORT ) )
	{
		if ( wxMessageBox( wxT("Tactical bombing at night is against rules, still bomb?"), Application::NAME,
				wxYES_NO | wxICON_QUESTION) != wxYES ) return;
	}
	switch ( mission )
	{
		case AirUnit::Mission::BOMB_HARASS:
			break;  // anything goes
		case AirUnit::Mission::BOMB_AF:
			if ( GetRawAFCapacity( x, y ) <= 0 )
				wxMessageBox( wxT("NOTE: there seems to be no airbase here..."), Application::NAME );
			break;
		case AirUnit::Mission::BOMB_RR:
			if ( !IsRRInHex( x, y ) ) wxMessageBox( wxT("NOTE: there seems to be no railroad here..."), Application::NAME );
			break;
		case AirUnit::Mission::NAVALPATROL:
		case AirUnit::Mission::BOMB_NAVAL:
			i = 0;
			for ( cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
				if ( cntr->getCounterType( ) == Counter::Type::SHIP && cntr->getSide( ) != _current_player )
				{
					dlg.AddTarget( cntr ), ++i;
				}
			if ( !i )
			{
				wxMessageBox( wxT("No targets"), Application::NAME );
				return;
			}
			if ( dlg.ShowModal( ) == FALSE ) return;
			dlg.CloseDialog( );  // oh woe
			target = dlg.GetTarget( );
			break;
		case AirUnit::Mission::BOMB_PLANES:
			i = 0;
			for ( cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
				if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getSide( ) != _current_player )
					dlg.AddTarget( cntr ), ++i;
			if ( ! i )
			{
				wxMessageBox( wxT("No targets, you should cancel this mission"), Application::NAME );
				return;
			}
			if ( dlg.ShowModal( ) == FALSE ) 
				return;
			dlg.CloseDialog( );  // oh woe
			target = dlg.GetTarget( );
			break;
		case AirUnit::Mission::STRAT_PORT:
		case AirUnit::Mission::BOMB_PORT:
			if ( !hex_[y][x].getPortSize( ) )
			{
				if ( wxMessageBox( wxT("There is no port here to bomb, bomb anyway?"), Application::NAME,
						wxYES_NO | wxICON_QUESTION) != wxYES ) return;
			}
			break;
		case AirUnit::Mission::BOMB_OTHER:
			break;
		case AirUnit::Mission::BOMB_CF:
			if ( ! GetCoastalDef( x, y ) )
			{
				if ( wxMessageBox( wxT("There are no coastal defenses here to bomb, bomb anyway?"), Application::NAME,
						wxYES_NO | wxICON_QUESTION) != wxYES ) return;
			}
			break;
		case AirUnit::Mission::RAILMARSHAL:
			break;  // TODO: check target availability:
		case AirUnit::Mission::STRAT_OTHER:
			break;
		case AirUnit::Mission::MINELAYING:
			if ( hex_[y][x].terrain_ != HexType::SEA && hex_[y][x].terrain_ != HexType::RESTRICTEDWATERS && hex_[y][x].terrain_ != HexType::ICINGSEA )
			{
				if ( wxMessageBox( wxT("It doesn't make much sense to mine non-sea hex, mine anyway?"), Application::NAME,
						wxYES_NO | wxICON_QUESTION) != wxYES ) return;
			}
			break;
		case AirUnit::Mission::TRANSPORT:
			break;
		case AirUnit::Mission::AIRDROP:
			if ( hex_[y][x].terrain_ == HexType::SEA || hex_[y][x].terrain_ == HexType::RESTRICTEDWATERS || hex_[y][x].terrain_ == HexType::ICINGSEA )
			{
				if ( wxMessageBox( wxT("Really drop cargo to the sea?"), Application::NAME, wxYES_NO | wxICON_QUESTION) != wxYES )
					return;
			}
			break;
		default:
			wxMessageBox( wxT("Select planes with bombing mission!"), Application::NAME );
			return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), str );
	wxString ans = wxGetTextFromUser( wxT("Bombing strength:"), Application::NAME, ws, this );
	if ( ans == wxT("") ) return;
	str = strtol( ans.ToAscii( ), 0, 0 );

	// harassment is different (no die rolling):
	if ( mission == AirUnit::Mission::BOMB_HARASS)
	{
		if ( str < 2 )
			h = ah = 0;
		else if ( str < 4 )
			h = 1, ah = 0;
		else if ( str < 10 )
			h = 2, ah = 0;
		else if ( str < 20 )
			h = 2, ah = 1;
		else
			h = 2, ah = 2;
		sit_[y][x].setHarrasment( sit_[y][x].getHarassment( ) + h );
		if ( ah )
		{  // >2 hits adjacent hexes are affected too
			ht::getAdjHexCoords( Hex::WEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
			if ( ! coordinatesOutOfBounds( x2, y2, world_x_size_, world_y_size_ ) )
			{
				sit_[y2][x2].setHarrasment( sit_[y2][x2].getHarassment( ) + ah );
				updateHitMarkers( x2, y2 );
			}
			ht::getAdjHexCoords( Hex::NORTHWEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
			if ( ! coordinatesOutOfBounds( x2, y2, world_x_size_, world_y_size_ ) )
			{
				sit_[y2][x2].setHarrasment( sit_[y2][x2].getHarassment( ) + ah );
				updateHitMarkers( x2, y2 );
			}
			ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
			if ( ! coordinatesOutOfBounds( x2, y2, world_x_size_, world_y_size_ ) )
			{
				sit_[y2][x2].setHarrasment( sit_[y2][x2].getHarassment( ) + ah );
				updateHitMarkers( x2, y2 );
			}
			ht::getAdjHexCoords( Hex::EAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
			if ( ! coordinatesOutOfBounds( x2, y2, world_x_size_, world_y_size_ ) )
			{
				sit_[y2][x2].setHarrasment( sit_[y2][x2].getHarassment( ) + ah );
				updateHitMarkers( x2, y2 );
			}
			ht::getAdjHexCoords( Hex::NORTHEAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
			if ( ! coordinatesOutOfBounds( x2, y2, world_x_size_, world_y_size_ ) )
			{
				sit_[y2][x2].setHarrasment( sit_[y2][x2].getHarassment( ) + ah );
				updateHitMarkers( x2, y2 );
			}
			ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
			if ( ! coordinatesOutOfBounds( x2, y2, world_x_size_, world_y_size_ ) )
			{
				sit_[y2][x2].setHarrasment( sit_[y2][x2].getHarassment( ) + ah );
				updateHitMarkers( x2, y2 );
			}
			Refresh( );
		}
		sprintf( msg, "%d harassment points delivered to the target hex\n"
				"%d harassment points delivered to all adjacent hexes", h, ah );
		wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );
		goto loppu;
	}

	// eval die modifiers
	// dive bombers:
	ENUM_SELUNITS(cntr)
		if ( ( (AirUnit*)cntr )->getType( ) != AirUnitType::D )
			break;
	if ( !cntr )
	{  // all planes were dive bombers
		wxMessageBox( wxT("Dive bombers: HexTools will add +1 to die roll result"), Application::NAME, wxICON_INFORMATION );
		++mod;
	}
	// anti-ship and torpedo bombers:
	ENUM_SELUNITS(cntr)
		if ( mission != AirUnit::Mission::BOMB_NAVAL
			|| ( ( (AirUnit*)cntr )->getCode( ) != AirUnit::Code::V && ( (AirUnit*)cntr )->getCode( ) != AirUnit::Code::S ) )
			break;
	if ( !cntr )
	{  // all planes were S/V
		wxMessageBox( wxT("Anti-shipping bombers: HexTools will add +1 to die roll result"), Application::NAME, wxICON_INFORMATION );
		++mod;
	}

	// die roll
	sprintf( msg, "Bomb with %d pts, %s%d die mod. Needs %d to hit.", str, mod >= 0 ? "+" : "", mod,
			BombingDieNeededToHit( str ) - mod );
	ddlg.SetHelpText( msg );
	ddlg.SetNoOfDies( 1 );
	ddlg.SetDieMax( 6 );
	if ( ddlg.ShowModal( ) != wxID_OK ) 
		return;
	ddlg.CloseDialog( );  // ...
	np = ddlg.GetDie1( );
	if ( mod )
	{
		np += mod;
		sprintf( msg, "Die modifier +%d turns result to %d", mod, np );
		wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION);
	}
	res = BombingTable( str, np );
	sprintf( msg, "%s\n\n(die roll was %d)", res == BOMB_HIT ? "*** BLAM ***" : "missed!", np );
	wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );

	// place report here
	bombing_report = new HexNote( HexNote::BOMBREPORT );
	if ( ! bombing_report )
	{
		wxMessageBox( wxT("Panic: memory allocation failed, please exit and buy more memory"), Application::NAME, wxICON_HAND );
		return;
	}
	insertUnitToCurrentHex( bombing_report );
	// report:
	bombing_report->setCaption( "Bombing report" );
	strcpy( rep, "" );
	switch ( mission )
	{
		case AirUnit::Mission::BOMB_AF:
			strcpy( rep, "Target: air base.\n" );
			break;
		case AirUnit::Mission::BOMB_RR:
			strcpy( rep, "Target: railroad.\n" );
			break;
		case AirUnit::Mission::NAVALPATROL:
		case AirUnit::Mission::BOMB_NAVAL:
			strcpy( rep, "Target: naval units.\n" );
			break;
		case AirUnit::Mission::BOMB_PLANES:
			strcpy( rep, "Target: planes on the ground.\n" );
			break;
		case AirUnit::Mission::BOMB_CF:
			strcpy( rep, "Target: coastal fortifications.\n" );
			break;
		case AirUnit::Mission::STRAT_PORT:
			strcpy( rep, "Target: port (strategic bombing).\n" );
			break;
		case AirUnit::Mission::BOMB_PORT:
			strcpy( rep, "Target: port (tactical bombing).\n" );
			break;
		case AirUnit::Mission::BOMB_OTHER:
			strcpy( rep, "Target: other tactical target.\n" );
			break;
		case AirUnit::Mission::TRANSPORT:
		case AirUnit::Mission::AIRDROP:
		case AirUnit::Mission::MINELAYING:
			strcpy( rep, "n/a\n" );
			break;
		case AirUnit::Mission::RAILMARSHAL:
			strcpy( rep, "Target: rail marshaling yard.\n" );
			break;
		case AirUnit::Mission::STRAT_OTHER:
			strcpy( rep, "Target: other strategic target.\n" );
			break;
		default:
			break;

	}
	// report stuff end

	strcat( rep, "Planes participated:\n" );
	ENUM_SELUNITS (cntr)
	{
		cntr->getUnitString( s );
		strcat( rep, s );
		strcat( rep, "\n" );
	}
	sprintf( msg, "\nResult: %d+%d=%d  ", np - mod, mod, np );
	strcat( rep, msg );

	if ( res == BOMB_HIT)
	{
		switch ( mission )
		{
			case AirUnit::Mission::BOMB_AF:
				wxMessageBox( wxT("One hit damage done to the airfield"), Application::NAME, wxICON_INFORMATION );
				strcat( rep, "hit to the airfield!" );
				sit_[y][x].setAFHits( sit_[y][x].getAFHits( ) + 1 );
				break;
			case AirUnit::Mission::BOMB_RR:
				wxMessageBox( wxT("Railroad is now broken here"), Application::NAME, wxICON_INFORMATION );
				strcat( rep, "hit to the rail line!" );
				sit_[y][x].setRRHit( true );
				break;
			case AirUnit::Mission::NAVALPATROL:
			case AirUnit::Mission::BOMB_NAVAL:
				ship = static_cast<NavalUnit*>( target );
				// old style ships: hits are REMOVED:
				if ( ship->getShipType( ) < NavalUnitType::FIRST_FLEET )
				{
					target->getUnitString( s );
					sprintf( msg, "'%s' is hit", s );
					strcat( rep, msg );
					ship->setHits( ship->getHits( ) - 1 );
					if ( ship->getHits( ) <= 0 )
					{
						sunken_ship = ship;  //DestroyUnit(target);
						strcat( rep, " and sunk!" );
					}
					else
					{
						strcat( rep, "!" );
						sprintf( msg, "%s has now only %d hits left!", s, ship->getHits( ) );
						wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );
					}
				}
				// task forces: hits are ADDED

				else
				{
					target->getUnitString( s );
					sprintf( msg, "'%s' is hit", s );
					strcat( rep, msg );
					ship->setHits( ship->getHits( ) + 1 );
					if ( ship->getHits( ) >= ship->getMaxHits( ) )
					{
						strcat( rep, " and sunk!" );
						sunken_ship = ship;  //DestroyUnit(target);
					}
					else
					{
						strcat( rep, "!" );
						sprintf( msg, "%s now has %d hits!", s, ship->getHits( ) );
						wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );
					}
				}
				break;
			case AirUnit::Mission::BOMB_PLANES:
				target->getUnitString( s );
				sprintf( msg, "%s is hit", s );
				strcat( rep, msg );
				if ( target->getAborted( ) )
				{
					DestroyUnit( target );
					strcat( rep, " and is destroyed!" );
				}
				else
				{
					sprintf( msg, "%s is now aborted", s );
					wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );
					strcat( rep, " and is now aborted!" );
					target->setAborted( TRUE );
					if ( _rule_set.AbortedPlanesToPool == TRUE )
					{
						DestroyUnit( target );
						sprintf( msg, "Note: %s was moved to air replacement pool", s );
						wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );
					}
				}
				strcat( rep, " Airbase is hit too!" );
				wxMessageBox( wxT("One hit damage done to the airfield"), Application::NAME, wxICON_INFORMATION );
				sit_[y][x].setAFHits( sit_[y][x].getAFHits( ) + 1 );
				break;
			case AirUnit::Mission::BOMB_CF:
				strcat( rep, "hit!" );
				wxMessageBox( wxT("One hit damage done to coastal fortifications"), Application::NAME, wxICON_INFORMATION );
				sit_[y][x].setCDHits( sit_[y][x].getCDHits( ) + 1 );
				break;
			case AirUnit::Mission::STRAT_PORT:
			case AirUnit::Mission::BOMB_PORT:
				strcat( rep, "hit!" );
				// port type affects hits
				if ( hex_[y][x].getPortAttribute( ) == Port::Attribute::STRONG )
				{
					wxMessageBox( wxT("Strong natural port: please apply hits manually") );
					hits = 0;
				}
				else if ( hex_[y][x].getPortAttribute( ) == Port::Attribute::ARTIFICIAL )
					hits = 2;
				else
					hits = 1;
				i = sit_[y][x].getPortHits( ) + hits;
				if ( GetPortRawCapacity( x, y ) - i <= -2 )
				{
					wxMessageBox( wxT("Port has reached it's maximum damage"), Application::NAME, wxICON_INFORMATION );
					i = GetPortRawCapacity( x, y ) + 2;
				}
				sit_[y][x].setPortHits( i );
				sprintf( msg, "%d hits damage done to the port", hits );
				wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );
				break;
			case AirUnit::Mission::BOMB_OTHER:
				strcat( rep, "hit!" );
				wxMessageBox( wxT("Tactical bombing hit delivered to the target hex!\n\n"
						"This result is not handled automatically, you need\n"
						"to apply any special effects manually."), Application::NAME, wxICON_INFORMATION );
				break;
			case AirUnit::Mission::TRANSPORT:
				wxMessageBox( wxT("Transport not implemented!\nUse setup mode"), Application::NAME );
				break;
			case AirUnit::Mission::AIRDROP:
				wxMessageBox( wxT("Air drop not implemented!\nUse setup mode"), Application::NAME );
				break;
			case AirUnit::Mission::MINELAYING:
				wxMessageBox( wxT("Mine laying not implemented!\nUse setup mode"), Application::NAME );
				break;
			case AirUnit::Mission::RAILMARSHAL:
				strcat( rep, "hit!" );
				wxMessageBox( wxT("Strategic bombing hit delivered to the rail marshaling yard,\n"
						"enemy rail capacity in this rail net must be temporarily\n"
						"decreased by one in next player turn."), Application::NAME, wxICON_INFORMATION );
				if ( !sit_[y][x].getRRHit( ) )
				{
					wxMessageBox( wxT("Railroad is now broken here"), Application::NAME, wxICON_INFORMATION );
					sit_[y][x].setRRHit( true );
				}
				break;
			case AirUnit::Mission::STRAT_OTHER:
				strcat( rep, "hit!" );
				wxMessageBox( wxT("Strategic bombing hit delivered to the target hex!"), Application::NAME, wxICON_INFORMATION );
				break;
			default:
				break;
		}
	}
	else
		strcat( rep, "Missed!" );
	bombing_report->setText( rep );

	loppu:
	ENUM_SELUNITS (cntr)
	{
		cntr->recordReplay( ht::Replay::Movement::BOMB );
		if ( ( (AirUnit*)cntr )->getMission( ) == AirUnit::Mission::BOMB_DAS )
			( (AirUnit*)cntr )->setWasDAS( 1 );  // never here...
		cntr->setMission( AirUnit::Mission::NONE );
		if ( ( (AirUnit*)cntr )->getExtRange( ) )
			cntr->setMP( cntr->getMaxMP( ) * 2 );
		else
			cntr->setMP( cntr->getMaxMP( ) );
	}
	contents_pane_->deselectAll( );
	contents_pane_->clear( );
	if ( sunken_ship )
		DestroyUnit( sunken_ship );

	// hit markers are not saved (does AddUnits)
	// must be called after any of the above hit settors
	updateHitMarkers( x, y );

	//UnitWin->AddUnits(Sit[y][x].UnitPtr);
	// is this hex air target any more?
	for ( cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getSide( ) == _current_player && cntr->getMission( ) )
			break;

	if ( ! cntr )
		sit_[y][x].setAirTarget( false );

	PaintHex( x, y );
}

// this is "coastal defenses summary" chart
int MapPane::GetCoastalDef( int x, int y )
{
	int str = 0;

	// ports give CD
	// NOTE: SF: in Mediterranean/Baltic seas ports give one pt less CD than
	//       in Atlantic. 1 pts CD status marker needed.
	if ( _rule_set.EnableCDCalc == TRUE )
	{
		switch ( hex_[y][x].getPortSize( ) )
		{
			case Port::Type::MINOR:
			case Port::Type::STANDARD:
				str += 1;
				break;
			case Port::Type::MAJOR:
			case Port::Type::GREAT:
				str += 2;
				break;
			default:
				break;
		}

		// TODO: -map also check fortress is improved
		if ( hex_[y][x].city_ && hex_[y][x].city_->type_ == City::Type::FORTRESS )
			str += 4;
	}

	Counter* c;
	ENUM_HEXUNITS(x, y, c)
	{
		// status markers
		if ( c->getCounterType( ) == Counter::Type::PTS && ( (StatusMarker*)c )->getPtsType( ) == StatusMarker::CD )
			str += ( (StatusMarker*)c )->getPts( );
		// port fort unit == 2
		if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->getType( ) == GroundUnitType::WW2::PORT_FORT )
			str += 2;
		// 0.5 RE LR Siege Art == 1
		else if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->getType( ) == GroundUnitType::WW2::LR_SIEGE_ART )
			str += (int) ( ( (GroundUnit*)c )->getRE( ) * 2);
		// 1.0 RE Italian Pos AA == 1
		else if ( c->getCounterType( ) == Counter::Type::UNIT
			&& ( ( (GroundUnit*)c )->getType( ) == GroundUnitType::WW2::POS_HV_AA || ( (GroundUnit*)c )->getType( ) == GroundUnitType::WW2::POS_LT_AA )
			&& ( (GroundUnit*)c )->getSubType( ) == GroundArmedForce::Axis::ITA )
			str += ( (GroundUnit*)c )->getFlak( ) / 2;  // 1 pts = 0.5 RE ???
		// fort counter == 1
		else if ( c->getCounterType( ) == Counter::Type::MISC && static_cast<Facility*>( c )->getMiscType( ) == Facility::FORT )
			str += 1;
	}
	return str;
}

// calculate total airfield capacity (less hits)
int MapPane::GetRawAFCapacity( int x, int y )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return 0;

	// TODO: this is the Hex:: function
	int a = hex_[y][x].getAFCapacity( );

	// weather may affect temporary airfields:
	bool bad_wthr = false;
	if ( _weather_condition[hex_[y][x].getWeatherZone( )] == WeatherCondition::MUD || _weather_condition[hex_[y][x].getWeatherZone( )] == WeatherCondition::SNOW )
		bad_wthr = true;

	// TODO: this is the HexContents:: function
	int b = sit_[y][x].getAFCapacity( bad_wthr );

	int c = sit_[y][x].getAFHits( );

	int cap = a + b - c;

	// TODO: call to limit(..)
	if ( cap < 0 )
		cap = 0;
	else if ( cap > 12 )
		cap = 12;

	return cap;
}

// calculate currently available airfield capacity (less usage)
int MapPane::GetCurrentAFCapacity( int x, int y )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return 0;

	int a = GetRawAFCapacity( x, y );
	int u = sit_[y][x].getAFUsage( );
	return max( a - u, 0 );
}

void MapPane::Land( )
{
	Counter* cntr;
	AirUnit* au;
	int warned = 0;

	if ( !_selected_unit_list )
	{
		SelUnitErr( );
		return;
	}
	// check for airfield
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("Can't land enemy planes!"), Application::NAME );
			return;
		}
	}
	if ( sit_[selected_hex_point_.y][selected_hex_point_.x].getOwner( ) != _current_player )
	{
		wxMessageBox( wxT("Can't land to enemy owned hex!"), Application::NAME );
		return;
	}
	if ( GetRawAFCapacity( selected_hex_point_.x, selected_hex_point_.y ) == 0 )
	{
		if ( wxMessageBox( wxT("There is no functioning airbase here, still land?"), Application::NAME, wxYES_NO ) != wxYES )
			return;
	}
	// land planes and zero mps
	ENUM_SELUNITS(cntr)
	{
		if ( cntr->getCounterType( ) != Counter::Type::PLANE )
			continue;

		au = (AirUnit*)cntr;
		if ( au->getInAir( ) )
		{
			if ( au->getNight( ) && au->getPrefix( ) != AirUnitType::Prefix::N && !warned )
			{
				wxMessageBox( wxT("NOTE: day air units flying night mission,\n"
						"you should check for crash landing!"), Application::NAME );
				warned = 1;
			}
			au->setInAir( 0 );
			if ( au->getMission( ) == AirUnit::Mission::STAGE)
			{  // may fly mission after stage
				au->setMP( au->getMaxMP( ) );
				au->setStaged( TRUE );
			}
			else
			{  // any other mission: inactivate
				au->setMP( 0 );
				au->setActive( FALSE );
				cntr->setMission( AirUnit::Mission::NONE );
			}
		}
	}
	showSelUnitMovementAreaAir( );
	contents_pane_->Refresh( );
}

// switch Fighters from CAP to other mission
void MapPane::CAPToEscort( )
{
	if ( ! _selected_unit_list )
	{
		wxMessageBox( wxT("Select some planes flying CAP first"), Application::NAME );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)c )->getMission( ) == AirUnit::Mission::CAP )
			( (AirUnit*)c )->setMission( AirUnit::Mission::ESCORT );
		else
			wxMessageBox( wxT("Select planes flying CAP!"), Application::NAME );
	}

	contents_pane_->Refresh( );
}

// switch Fighters from CAP to other mission
void MapPane::CAPToInterception( )
{
	if ( ! _selected_unit_list )
	{
		wxMessageBox( wxT("Select some planes flying CAP first"), Application::NAME );
		return;
	}

	Counter* c;
	ENUM_SELUNITS(c)
	{
		if ( c->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)c )->getMission( ) == AirUnit::Mission::CAP )
			( (AirUnit*)c )->setMission( AirUnit::Mission::INTERCEPT );
		else
			wxMessageBox( wxT("Select planes flying CAP!"), Application::NAME );
	}

	contents_pane_->Refresh( );
}

#endif
