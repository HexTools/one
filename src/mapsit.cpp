#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "dirs.loop"
#include "selectedunits.loop"
#include "hexunits.loop"
#include "sideplayer.h"
#include "weathercondition.h"
#include "phase.h"
#include "weatherclass.h"
#include "supplystatus.h"

#include "hextools.h"
using ht::wxS;
using ht::aec2str;
using ht::oppositeDir;
using ht::coordinatesOutOfBounds;

#include "rules.h"
#include "hextype.h"
#include "port.h"
#include "city.h"
#include "hex.h"
#include "mappane.h"
#include "counter.h"
#include "groundunit.h"
#include "airunit.h"
#include "statusmarker.h"
#include "hitmarker.h"
#include "hexnote.h"
#include "hexcontents.h"
#include "hexcontentspane.h"
#include "dhex.h"

extern Rules _rule_set;

extern char msg[];	// 10000
extern const wxString _weather_condition_string[];	// 5	// from mappane.cpp
extern const char** _weather_zone_string; //[ht::NUM_WZ];
extern HexType _hex_types[]; //[TERRAINTYPECOUNT];	// terrain types
extern Counter* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _phasing_player;
extern int _current_player;
extern int _current_phase;						// COMBAT_PHASE etc.
extern int _weather_condition[];	// W_ZONES		// W_MUD etc., per zone (frame.cpp)
extern int _rivers_lakes_frozen[];	// W_ZONES		// true if rivers/lakes are frozen (frame.cpp)

extern HexDlgXfer _hex_dialog_data;

// TODO: DTM -- could use the one from stdlib.h: char* itoa (int, char*, int)
// used only by CmViewHex(..)
static char* itoa( int v )
{
	static char s[50];
	sprintf( s, "%d", v );
	return s;
}

// display hex info message box
void MapPane::CmViewHex( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	HexContents hc = sit_[y][x]; // the hex contents, data from -Play
	Hex h = hex_[y][x]; // the hex, data from -Map

	int def;
	int stats;
	int dummy[6];
	char city[100];
	h.getHexStats( &def, &stats, dummy, city );

	char def_explain[255];
	def = GetMods( x, y, def_explain );

	char mapId[ht::MAX_MAP_ID_LEN];
	h.getMapIdStr( mapId ); // up to 5-length char*
	char hexId[ht::MAX_HEX_ID_LEN];
	h.getHexIdStr( hexId ); // 4-digit number char*

	wxString maphex( wxS( mapId ) );
	maphex.Replace( "^", "" );

	maphex += ":" + wxS( hexId );
	maphex += " (" + wxString::Format( wxT("%i"), x ) + ", " + wxString::Format( wxT("%i"), y ) + ")";

	strcpy( _hex_dialog_data.hexid, const_cast<char*>( maphex.mb_str() ) );

	strcpy( _hex_dialog_data.city, strlen( city ) == 0 ? "-" : city );

	// TODO: rip this out when above is confirmed
	//if ( strlen( city ) == 0 )
	//	strcpy( HexDlgData.city, "-" );
	//else
	//	strcpy( HexDlgData.city, city );

	int wthr = _weather_condition[h.getWeatherZone( )];
	switch ( wthr )
	{
		case WeatherCondition::FROST:
		case WeatherCondition::CLEAR:
			wthr = WeatherClass::CLEAR;
			break;
		case WeatherCondition::SNOW:
		case WeatherCondition::WINTER:
			wthr = WeatherClass::WINTER;
			break;
		case WeatherCondition::MUD:
			wthr = WeatherClass::MUD;
			break;
	}

	int* mp = _hex_types[h.terrain_].mp_[wthr];

	char tmp[100];
	switch ( hc.getOwner( ) )
	{
		case SidePlayer::AXIS:
			strcpy( tmp, "Axis" );
			break;
		case SidePlayer::ALLIED:
			strcpy( tmp, "Allied" );
			break;
		default:
			strcpy( tmp, "Neutral" );
	}

	// ownership has been changed in this turn?
	if ( hc.getOwner( ) != hc.getOrigOwner( ) )
	{
		strcat( tmp, " (but " );
		switch ( hc.getOrigOwner( ) )
		{
			case SidePlayer::AXIS:
				strcat( tmp, "Axis" );
				break;
			case SidePlayer::ALLIED:
				strcat( tmp, "Allied" );
				break;
			default:
				strcat( tmp, "Neutral" );
		}
		strcat( tmp, " owned this hex at the beginning of this turn)" );
	}
	strcpy( _hex_dialog_data.owner, tmp );

	strcpy( _hex_dialog_data.terrain_str, _hex_types[h.terrain_].name_ );

	sprintf( _hex_dialog_data.weather, "%s, %sice [WZ %s]",
			_weather_condition_string[_weather_condition[h.getWeatherZone( )]].c_str( ),
			_rivers_lakes_frozen[h.getWeatherZone( )] ? "" : "no ",
			_weather_zone_string[h.getWeatherZone( )] );
			//(char)h.getWeatherZone( ) + 'A' );

	// general info
	sprintf( msg,
			"******************************************************************"
			"\nHex: \t%s\n"
			"Terrain:\t%s\n"
			"Owner:\t%s\n\n"
			"Weather:\t%s, "
			"rivers and lakes are %s "
			"[WZ %s]\n\n",
			const_cast<char*>( maphex.mb_str() ),
			_hex_types[h.terrain_].name_,
			tmp,
			_weather_condition_string[_weather_condition[h.getWeatherZone( )]].c_str( ),
			_rivers_lakes_frozen[h.getWeatherZone( )] ? "frozen" : "not frozen",
			_weather_zone_string[h.getWeatherZone( )] );

	strcat( msg, city );
	if ( strlen( city ) > 0 )
		strcat( msg, "\n" );

	// movement info
	if ( IsRRInHex( x, y ) )
	{
		if ( hc.getGauge( ) == SidePlayer::AXIS )
		{
			strcpy( _hex_dialog_data.rail, "Narrow" );
			strcat( msg, "Rail gauge:  narrow\n" );
		}
		else
		{
			strcpy( _hex_dialog_data.rail, "Wide" );
			strcat( msg, "Rail gauge:  wide\n" );
		}
		if ( hc.getRRHit( ) )
		{
			strcat( _hex_dialog_data.rail, ", broken!!\n" );
			strcat( msg, "Railroad is broken!!\n" );
		}
		if ( hc.isRailObstructed( ) )
		{
			strcat( msg, "Railroad is obstructed!\n" );
		}
	}
	else
		strcpy( _hex_dialog_data.rail, "-" );

	if ( hc.getHarassment( ) == 1 )
	{
		sprintf( _hex_dialog_data.harr, "+1 MP" );
		strcat( msg, "1 harassment hit!!\n" );
	}
	else if ( hc.getHarassment( ) == 2 )
	{
		sprintf( _hex_dialog_data.harr, "+2 MPs" );
		strcat( msg, "2 harassment hits!!\n" );
	}
	else
		sprintf( _hex_dialog_data.harr, "-" );

	// combat effects
	strcat( msg, "\nCombat effects:\n" );
	if ( stats & Hex::DEF_NO_EFFECT )
		strcat( msg, "    -No effects\n" );
	else
	{
		if ( stats & Hex::DEF_ATT_QUARTERED )
			strcat( msg, "    -Attacker quartered\n" );
		if ( stats & Hex::DEF_INF_HALVED )
		{
			strcat( msg, "    -Attacker halved\n" );
			// jungles are special:
			if ( h.terrain_ == HexType::JUNGLE || h.terrain_ == HexType::JUNGLEROUGH || h.terrain_ == HexType::JUNGLESWAMP )
				strcat( msg, "    -Exception: Infantry attacker NOT halved\n" );
			if ( ! ( stats & Hex::DEF_MTN_HALVED ) )
				strcat( msg, "    -Exception: Mountain attacker NOT halved\n" );
		}
		if ( stats & Hex::DEF_CM_HALVED )
			strcat( msg, "    -C/M attacker halved\n" );
		if ( stats & Hex::DEF_ART_HALVED )
			strcat( msg, "    -Attacking artillery halved\n" );
		if ( stats & Hex::DEF_GS_HALVED )
			strcat( msg, "    -Ground support halved\n" );
		if ( stats & Hex::DEF_NO_AEC )
			strcat( msg, "    -No AEC\n" );
		if ( stats & Hex::DEF_FREEZES )
			strcat( msg, "    -Freezes in winter\n" );
	}
	sprintf( tmp, "Defense modifier:\t%d (%s)\n", def, def_explain );
	strcat( msg, tmp );
	if ( IsCoastalCliffs( x, y ) )
		strcat( msg, "Coastal cliffs, attacker making amphibious landing halved\n" );

	if ( GetRawAFCapacity( x, y ) > 0 || hc.getAFHits( ) > 0 )
	{
		sprintf( tmp, "\nAirfield, %d hits, Capacity: %d (%d left)", hc.getAFHits( ), GetRawAFCapacity( x, y ), GetCurrentAFCapacity( x, y ) );
		strcat( msg, tmp );
		sprintf( _hex_dialog_data.airfield, "%d hits, capacity: %d (%d left)", hc.getAFHits( ), GetRawAFCapacity( x, y ), GetCurrentAFCapacity( x, y ) );
	}
	else
		sprintf( _hex_dialog_data.airfield, "-" );

	if ( h.getPortSize( ) || hc.isMulberry( ) )
	{
		strcat( msg, "\n" );
		strcpy( _hex_dialog_data.port, "" );
		if ( h.getPortAttribute( ) == Port::Attribute::ARTIFICIAL )
		{
			strcat( msg, "Artificial " );
			strcat( _hex_dialog_data.port, "Artificial " );
		}
		else if ( h.getPortAttribute( ) == Port::Attribute::STRONG )
		{
			strcat( msg, "Natural " );
			strcat( _hex_dialog_data.port, "Natural  " );
		}
		switch ( h.getPortSize( ) )
		{
			case Port::Type::MINOR:
				strcat( msg, "Minor Port" );
				strcat( _hex_dialog_data.port, "Minor Port" );
				break;
			case Port::Type::STANDARD:
				strcat( msg, "Standard Port" );
				strcat( _hex_dialog_data.port, "Standard Port" );
				break;
			case Port::Type::MAJOR:
				strcat( msg, "Major Port" );
				strcat( _hex_dialog_data.port, "Major Port" );
				break;
			case Port::Type::GREAT:
				strcat( msg, "Great Port" );
				strcat( _hex_dialog_data.port, "Great Port" );
				break;
			case Port::Type::ANCHORAGE:
				strcat( msg, "Anchorage" );
				strcat( _hex_dialog_data.port, "Anchorage" );
				break;
			case Port::Type::MARGINAL:
				strcat( msg, "Marginal Port" );
				strcat( _hex_dialog_data.port, "Marginal Port" );
				break;
		}
		if ( hc.isMulberry( ) )
		{
			strcat( msg, "\nMulberry" );
			strcat( _hex_dialog_data.port, "mulberry" );
		}

		sprintf( tmp, "%d hits, capacity: %d (%d left)", hc.getPortHits( ), GetPortCapacity( x, y ), GetPortCapacity( x, y ) - hc.getPortUsage( ) );
		strcat( _hex_dialog_data.port, "\n" );
		strcat( _hex_dialog_data.port, tmp );
		strcat( msg, ", " );
		strcat( msg, tmp );
	}
	else
		strcpy( _hex_dialog_data.port, "-" );

	if ( hc.isPortDestroyed( ) )
		strcat( msg, "\nPort is destroyed!" );

	int i = GetCoastalDef( x, y );
	if ( i != 0 )
	{
		sprintf( tmp, "\n%d points coastal defenses, %d hits", i, hc.getCDHits( ) );
		strcat( msg, tmp );
	}

	strcat( msg, "\n\nMovement points:\n    -Normal:\t" );
	// weather conditions!!!
	strcat( msg, itoa( mp[MovementCategory::OTHER] ) );  // TODO: use stblib version of itoa(..)
	strcat( msg, "\n    -Mot/Art:\t" );

	if ( h.terrain_ == HexType::SWAMP && _rule_set.MP6Swamp == FALSE )
		strcat( msg, "1/2MP + 1" );
	else
		strcat( msg, itoa( mp[MovementCategory::MOT_ART] ) ); 	// TODO: use stblib version of itoa(..)

	strcat( msg, "\n    -Light:     \t" );
	strcat( msg, itoa( mp[MovementCategory::LIGHT] ) );	// TODO: use stblib version of itoa(..)
	strcat( msg, "\n    -Cavalry:\t" );
	strcat( msg, itoa( mp[MovementCategory::MP_CAV] ) );	// TODO: use stblib version of itoa(..)
	strcat( msg, "\n    -Mountain:\t" );
	strcat( msg, itoa( mp[MovementCategory::MP_MTN] ) );	// TODO: use stblib version of itoa(..)
	strcat( msg, "\n    -Ski:\t\t" );
	strcat( msg, itoa( mp[MovementCategory::MP_SKI] ) );	// TODO: use stblib version of itoa(..)
	// old:
	wxMessageBox( wxS( msg ), wxT("Hex info") );
	// new:
	//     DHexInfo hdlg(this);
	//     HexDlgData.terrain = hex[y][x].Terrain;
	//     hdlg.ShowModal();
}

void MapPane::ViewStack( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}
	//if ( Sit[ptCurrentHex.y][ptCurrentHex.x].GetUnitCount( ) == 0 )
	//{
	//  wxMessageBox( "No units", MSGHDR );
	//  return;
	//}

	float att = 0.0;
	float def = 0.0;
	float aa = 0.0;
	float re = 0.0;
	float art_re = 0.0;
	float norm_re = 0.0;

	int cnt = 0;

	Counter* c;
	// UnitPtr
	ENUM_HEXUNITS( selected_hex_point_.x, selected_hex_point_.y, c )
	{
		// attack pts are omitted because they represent attack str TO hex
		//if ( c->Type() == CNTR_PTS  && ( (StatusMarker*)c )->GetPtsType( ) == PTS_ATTACK )
		//    att += ( (StatusMarker*)c )->GetPts( );
		if ( c->getCounterType( ) != Counter::Type::UNIT )
			continue;
		else if ( c->getCounterType( ) == Counter::Type::PTS && static_cast<StatusMarker*>( c )->getPtsType( ) == StatusMarker::DEFENSE )
			def += static_cast<StatusMarker*>( c )->getPts( );
		else if ( c->getCounterType( ) == Counter::Type::PTS && static_cast<StatusMarker*>( c )->getPtsType( ) == StatusMarker::AA )
			aa += static_cast<StatusMarker*>( c )->getPts( );
		else if ( c->getCounterType( ) == Counter::Type::SHIP	)  // allow ships to attack in combat phase
			att += c->getRealAtt( -1 );
		else
		{
			att += c->getRealAtt( -1 );
			aa += c->getRealFlak( );
			re += c->getRE( );
		}
		++cnt;
	}

	// SelectedUnits
	ENUM_SELUNITS( c )
	{
		// attack pts are omitted because they represent attack str TO hex
		//if (c->Type() == CNTR_PTS  && ((StatusMarker*)c)->GetPtsType() == PTS_ATTACK)
		//    att += ((StatusMarker*)c)->GetPts();
		if ( c->getCounterType( ) != Counter::Type::UNIT )
			continue;
		else if ( c->getCounterType( ) == Counter::Type::PTS && ( (StatusMarker*)c )->getPtsType( ) == StatusMarker::DEFENSE )
			def += ( (StatusMarker*)c )->getPts( );
		else if ( c->getCounterType( ) == Counter::Type::PTS && ( (StatusMarker*)c )->getPtsType( ) == StatusMarker::AA )
			aa += ( (StatusMarker*)c )->getPts( );
		else if ( c->getCounterType( ) == Counter::Type::SHIP	)  // Allow ships to attack in combat phase
			att += c->getRealAtt( -1 );
		else
		{
			att += c->getRealAtt( -1 );
			aa += c->getRealFlak( );
			re += c->getRE( );
		}
		++cnt;
	}

	def = CalcDefStr( selected_hex_point_.x, selected_hex_point_.y );

	art_re = 0;
	norm_re = 0;
	// if more artillery than normal units extra arts will have total strength of 1
	// UnitPtr
	ENUM_HEXUNITS( selected_hex_point_.x, selected_hex_point_.y, c )
	{
		if ( c->getCounterType( ) != Counter::Type::UNIT )
			continue;
		GroundUnit* u = (GroundUnit*)c;
		if ( u->isArtillery( ) )
		{
			art_re += u->getRE( );
#if 0
			sprintf( msg, "%0.2f-%0.2f art REs=%0.1f", u->GetRealAtt(-1), u->GetRealDef(-1), u->GetRE() );
			wxMessageBox( wxS(msg), Application::NAME );
#endif
		}
		else if ( u->isCombatUnit( ) )
		{
			norm_re += u->getRE( );
#if 0
			sprintf( msg, "%0.2f-%0.2f combat REs=%0.1f", u->GetRealAtt(-1), u->GetRealDef(-1), u->GetRE() );
			wxMessageBox( wxS(msg), Application::NAME );
#endif
		}
	}
#if 0
	sprintf( msg, "art=%0.1f, combat=%0.1f", art_re, norm_re );
	wxMessageBox( wxS(msg), Application::NAME );
#endif
	int aeca = sit_[selected_hex_point_.y][selected_hex_point_.x].getAECA( _selected_unit_list );
	int aecd = sit_[selected_hex_point_.y][selected_hex_point_.x].getAECD( _selected_unit_list );
	int atec = sit_[selected_hex_point_.y][selected_hex_point_.x].getATEC( _selected_unit_list );

	aa += IntrinsicCityAA( selected_hex_point_.x, selected_hex_point_.y );
	aa += IntrinsicAirfieldAA( selected_hex_point_.x, selected_hex_point_.y );

	sprintf( msg, "Stack info:\n\n"
			"%d units, %.1f REs\n"
			"Attack strength:\t%.2f %s\n"
			"Defense strength:\t%.2f\n"
			"AA strength:\t%.2f\n\n"
			"AECA:\t%s\n"
			"AECD:\t%s\n"
			"ATEC:\t%s\n\n"
			"%s",
			cnt, re,
			att, ( art_re > norm_re ) ? " or less (see note below)" : "",
			def,
			aa,
			aec2str(aeca),
			aec2str(aecd),
			aec2str(atec),
			( art_re > norm_re ) ? "NOTE: more artillery RE's than normal RE's. Attack strength\n"
									"will be less than shown above unless more non-artillery units\n"
									"from other hexes participate the attack." : "" );

	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
}

// scan given list and selected units for any hit marker of given type
static HitMarker* scan_for_hit( Counter* c, int type )
{
	HitMarker* h = NULL;

	for ( ; c; c = c->getNext( ) )
		if ( c->getCounterType( ) == Counter::Type::HIT && ( (HitMarker*)c )->getHitType( ) == type )
		{
			h = (HitMarker*)c;
			break; // TODO: why not just return h here?
		}

	if ( h == NULL )  // not found in given list, try selected units
	{
		for ( c = _selected_unit_list; c; c = c->getNext( ) )
			if ( c->getCounterType( ) == Counter::Type::HIT && ( (HitMarker*)c )->getHitType( ) == type )
			{
				h = (HitMarker*)c;
				break; // TODO: why not just return h here?
			}
	}

	return h;
}

void MapPane::AddNote( bool alert )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	HexNote* note = new HexNote( alert ? HexNote::ALERT : HexNote::TEXT );
	// TODO:  ???? note->setSide( Player );

	if ( ! note->edit( ) ) // show editor dialog
	{
		delete note;
		return;
	}

	// TODO:  ???? note->setHomeHex( ptCurrentHex.x, ptCurrentHex.y )
	// TODO:  ???? Sit[ptCurrentHex.y][ptCurrentHex.x].setOwner( Player );
	// TODO:  ???? Sit[ptCurrentHex.y][ptCurrentHex.x].insertUnit( note );
	insertUnitToCurrentHex( note );
	sit_[selected_hex_point_.y][selected_hex_point_.x].updateCaches( 0, _selected_unit_list );
	paintCurrentHex( );
	contents_pane_->addUnit( note );
	contents_pane_->Refresh( );
	// TODO:  updateHexCache(..) here??
	// TODO:  ???? PaintHex(..) here??
	file_is_dirty_ = true;
}

void MapPane::RemoveNote( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}
	wxMessageBox( wxT("You shouldn't be here (use 'Remove Unit' command instead)!") );
}

void MapPane::addStatusMarker( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	StatusMarker* statusmarker = new StatusMarker( );
	statusmarker->setSide( _current_player );

	if ( ! statusmarker->edit( ) ) // show editor dialog
	{
		delete statusmarker;
		return;
	}

	// TODO:  ???? statusmarker->setHomeHex( ptCurrentHex.x, ptCurrentHex.y )
	// TODO:  ???? Sit[ptCurrentHex.y][ptCurrentHex.x].setOwner( Player );
	// TODO:  ???? Sit[ptCurrentHex.y][ptCurrentHex.x].insertUnit( statusmarker );
	insertUnitToCurrentHex( statusmarker );
	paintCurrentHex( );
	contents_pane_->addUnit( statusmarker );
	contents_pane_->Refresh( );
	// TODO:  updateHexCache(..)??
	// TODO:  or Sit[ptCurrentHex.y][ptCurrentHex.x].updateCaches( 0, SelectedUnits );
	// TODO:  ???? PaintHex(..)
	file_is_dirty_ = true;
}

void MapPane::ChangeOwner( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}
#if 0
	ENUM_HEXUNITS(ptCurrentHex.x, ptCurrentHex.y, cntr)
	if ( cntr->Type() != CNTR_MISC && cntr->Type() != CNTR_NOTE && cntr->Type() != CNTR_HIT && cntr->Type() != CNTR_PTS )
	{
		wxMessageBox("Can't change owner of the occupied hex!", MSGHDR, wxOK);
		return;
	}
	ENUM_SELUNITS( cntr )
	if ( cntr->Type() != CNTR_MISC && cntr->Type() != CNTR_NOTE && cntr->Type() != CNTR_HIT && cntr->Type() != CNTR_PTS )
	{
		wxMessageBox("Can't change owner of the occupied hex!", MSGHDR, wxOK);
		return;
	}
#endif
	int newside = sit_[selected_hex_point_.y][selected_hex_point_.x].getOwner( ) == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS;
	sit_[selected_hex_point_.y][selected_hex_point_.x].setOwner( newside );

	// there may be airfields, hit markers, notes etc. in the hex,
	// therefore change their owner too
	Counter* cntr;
	ENUM_HEXUNITS(selected_hex_point_.x, selected_hex_point_.y, cntr)
		if ( cntr->getCounterType( ) != Counter::Type::UNIT && cntr->getCounterType( ) != Counter::Type::PLANE && cntr->getCounterType( ) != Counter::Type::SHIP )
			cntr->setSide( newside );

	ENUM_SELUNITS(cntr)
		if ( cntr->getCounterType( ) != Counter::Type::UNIT && cntr->getCounterType( ) != Counter::Type::PLANE && cntr->getCounterType( ) != Counter::Type::SHIP )
			cntr->setSide( newside );

	paintCurrentHex( );
}

void MapPane::SetOwner( int owner )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	if ( owner != SidePlayer::AXIS && owner != SidePlayer::ALLIED && owner != 2 /* NEUTRAL */ )
	{
		wxMessageBox( wxT("MapPane::SetOwner: illegal side (bug!)") );
		return;
	}

	Counter* cntr = _selected_unit_list;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) != Counter::Type::MISC )
		{
			wxMessageBox( wxT("Can't change owner of the occupied hex!"), Application::NAME, wxOK );
			return;
		}
		cntr = cntr->getNext( );
	}

	sit_[selected_hex_point_.y][selected_hex_point_.x].setOwner( owner );
	paintCurrentHex( );
}

// set ownership of currently visible hexes
void MapPane::SetVisibleOwner( int owner )
{
	int xmax, ymax = 0;
	GetClientSize( &xmax, &ymax );

	int xmin = 0, ymin = 0;
	screen2Hex( &xmin, &ymin );
	screen2Hex( &xmax, &ymax );

	if ( ymax > world_y_size_ )
		ymax = world_y_size_;

	if ( xmax > world_x_size_ )
		xmax = world_x_size_;

	for ( int y = ymin; y < ymax; ++y )
		for ( int x = xmin; x < xmax; ++x )
			sit_[y][x].setOwner( owner );

	Refresh( );
}

// set ownership of ALL hexes
void MapPane::SetAllOwner( int owner )
{
	if ( wxMessageBox( wxT("This will change ownership of ALL possible hexes!\nAre you sure??"), Application::NAME, wxYES_NO | wxICON_HAND ) != wxYES )
		return;

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			sit_[y][x].setOwner( owner );

	Refresh( );
}

void MapPane::ChangeOrigOwner( int what )
{
	int x, y;

	if ( what == 0 )
	{  // just selected hex
		if ( ! isHexSelected( ) )
		{
			wxMessageBox( wxT("Select hex first"), Application::NAME );
			return;
		}
		sit_[selected_hex_point_.y][selected_hex_point_.x].setOrigOwner( sit_[selected_hex_point_.y][selected_hex_point_.x].getOwner( ) );
		paintCurrentHex( );
	}
	else if ( what == 1 )
	{  // whole map
		if ( wxMessageBox( wxT("Really set current owner to be old owner for ALL hexes?"), Application::NAME, wxYES_NO ) != wxYES )
			return;

		for ( y = 0; y < world_y_size_; ++y )
			for ( x = 0; x < world_x_size_; ++x )
				sit_[y][x].setOrigOwner( sit_[y][x].getOwner( ) );

		Refresh( );
	}
	else
	{  // visible hexes
		if ( wxMessageBox( wxT("Really set current owner to be old owner for visible hexes?"), Application::NAME, wxYES_NO ) != wxYES )
			return;

		int xmax, xmin = 0, ymax, ymin = 0;

		GetClientSize( &xmax, &ymax );
		screen2Hex( &xmin, &ymin );
		screen2Hex( &xmax, &ymax );
		if ( ymax > world_y_size_ )
			ymax = world_y_size_;

		if ( xmax > world_x_size_ )
			xmax = world_x_size_;

		for ( y = ymin; y < ymax; ++y )
			for ( x = xmin; x < xmax; ++x )
				sit_[y][x].setOrigOwner( sit_[y][x].getOwner( ) );

		Refresh( );
	}
}

// modify/insert/remove hit markers (counters) in the hex
void MapPane::updateHitMarkers( int x, int y )
{
	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
	{
		contents_pane_->deselectAll( );
		contents_pane_->clear( );
	}

	// airfield hits:
	HitMarker* hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::AF );
	if ( sit_[y][x].getAFHits( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::AF );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( sit_[y][x].getAFHits( ) );
	}
	else
	{  // no hits, delete any counter
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}

	// RR hit:
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::RR );
	if ( sit_[y][x].getRRHit( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::RR );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( 1 );
	}
	else
	{  // no hits, delete any counter
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}

	// RR obstruction:
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::RR_OBSTRUCT );
	if ( sit_[y][x].isRailObstructed( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::RR_OBSTRUCT );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( 1 );
	}
	else  // no hits, delete any counter
	{
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}

	// port hits
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::PORT );
	if ( sit_[y][x].getPortHits( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::PORT );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( sit_[y][x].getPortHits( ) );
	}
	else
	{  // no hits, delete any counter
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}
#if 0
	// Axis mines
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::AXMINE );
	if ( sit_[y][x].getAxisMines( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::AXMINE );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( sit_[y][x].getAxisMines( ) );
	}
	else
	{  // no hits, delete any counter
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}

	// Allied mines
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::ALMINE );
	if ( sit_[y][x].getAlliedMines( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::ALMINE );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( sit_[y][x].getAlliedMines( ) );
	}
	else
	{  // no hits, delete any counter
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}
#endif
	// harassment
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::HARASSMENT );
	if ( sit_[y][x].getHarassment( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::HARASSMENT );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( sit_[y][x].getHarassment( ) );
	}
	else
	{  // no hits, delete any counter
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}

	// port destruction
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::PORTDEST );
	if ( sit_[y][x].isPortDestroyed( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::PORTDEST );
			insertUnit( x, y, hit );
		}
	}
	else
	{  // no hits, delete any counter
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}

	// CD hits
	hit = scan_for_hit( sit_[y][x].unit_list_, HitMarker::CD );
	if ( sit_[y][x].getCDHits( ) )
	{
		if ( ! hit )
		{
			hit = new HitMarker( HitMarker::CD );
			insertUnit( x, y, hit );
		}
		hit->setHitCount( sit_[y][x].getCDHits( ) );
	}
	else  // no hits, delete any counter
	{
		if ( hit )
		{
			removeUnit( x, y, hit );
			delete hit;
		}
	}

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
		contents_pane_->addUnits( sit_[y][x].unit_list_ );
}

bool MapPane::IsAxisOwned( int x, int y )
{
	return ( sit_[y][x].getOwner( ) == SidePlayer::AXIS );
}

bool MapPane::IsAlliedOwned( int x, int y )
{
	return ( sit_[y][x].getOwner( ) == SidePlayer::ALLIED );
}

bool MapPane::IsEnemyOccupied( int x, int y )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	if ( sit_[y][x].getOwner( ) == _current_player )
		return false;

	Counter* cntr;
	ENUM_HEXUNITS(x, y, cntr)
		if ( cntr->getSide( ) != _current_player )
		{
			//  In movement phase hex is not enemy occupied if there are only
			// partisans. This makes it impossible to order units to attack
			// such hex! That's why in combat phase same hex is considered to
			// be enemy occupied; it is possible to order attack in combat phase.
			if ( cntr->getCounterType( ) == Counter::Type::UNIT
				&& ( ( (GroundUnit*)cntr )->getType( ) != GroundUnitType::WW2::PARTISAN || _current_phase == Phase::COMBAT_PHASE ) )
#if 0
			cntr->Type() != CNTR_PTS &&
			cntr->Type() != CNTR_SHIP &&
			cntr->Type() != CNTR_NOTE &&
#endif
				return true;

			// landed planes make hex enemy occupied
			if ( cntr->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)cntr )->getInAir( ) == FALSE )
				return true;
		}

	return false;
}

bool MapPane::IsAdjEnemies( int x, int y )
{
	int x2, y2;

	int d;
	ENUM_DIRS(d)
		if ( ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
			if ( IsEnemyOccupied( x2, y2 ) )
				return true;

	return false;

#if 0
	if ( ht::getAdjHexCoords( WEST, x, y, &x2, &y2, iXWorldSize, iYWorldSize ) )
		if ( IsEnemyOccupied( x2, y2 ) )
			return true;

	if ( ht::getAdjHexCoords( NORTHWEST, x, y, &x2, &y2, iXWorldSize, iYWorldSize ) )
		if ( IsEnemyOccupied( x2, y2 ) )
			return true;

	if ( ht::getAdjHexCoords( SOUTHWEST, x, y, &x2, &y2, iXWorldSize, iYWorldSize ) )
		if ( IsEnemyOccupied( x2, y2 ) )
			return true;

	if ( ht::getAdjHexCoords( EAST, x, y, &x2, &y2, iXWorldSize, iYWorldSize ) )
		if ( IsEnemyOccupied( x2, y2 ) )
			return true;

	if ( ht::getAdjHexCoords( NORTHEAST, x, y, &x2, &y2, iXWorldSize, iYWorldSize ) )
		if ( IsEnemyOccupied( x2, y2 ) )
			return true;

	if ( ht::getAdjHexCoords( SOUTHEAST, x, y, &x2, &y2, iXWorldSize, iYWorldSize ) )
		if ( IsEnemyOccupied( x2, y2 ) )
			return true;

	return false;
#endif
}

bool MapPane::IsOwnOccupied( int x, int y )
{
	if ( sit_[y][x].getOwner( ) == _current_player && sit_[y][x].getUnitCount( ) != 0 )
		return true;

	return false;
}

// TODO: this function is only used in the sense that the # of units is non-zero,
//	therefore it could be refactored to return true/false (true when the first
//	attacking unit is found in the specified direction)
int MapPane::GetNoOfAttackingUnits( int x, int y, int dir )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return 0;

	int c = 0;

	Counter* cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && cntr->getAttackDir( ) == dir )
			++c;
		cntr = cntr->getNext( );
	}

	return c;
}

bool MapPane::IsHexAttacked( int x, int y )
{
	// owned by phasing player -> captured
	switch ( _current_phase )
	{
		case Phase::REACTION_PHASE:
			if ( sit_[y][x].getOwner( ) == ( _phasing_player == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS ) )
				return false;
			break;
		default:
			if ( sit_[y][x].getOwner( ) == _phasing_player )
				return false;
			break;
	}

	int d;
	ENUM_DIRS(d)
	{
		int x2, y2;
		ht::getAdjHexCoords( d, x, y, &x2, &y2, world_x_size_, world_y_size_ );

		if ( GetNoOfAttackingUnits( x2, y2, oppositeDir( d ) ) != 0 )
			return true;
	}

	return false;
}

bool MapPane::IsHexAirAttacked( int x, int y )
{
	Counter* cntr = sit_[y][x].unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) != AirUnit::Mission::NONE )
			return true;
		cntr = cntr->getNext( );
	}

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
	{
		cntr = _selected_unit_list;
		while ( cntr )
		{
			if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) != AirUnit::Mission::NONE )
				return true;
			cntr = cntr->getNext( );
		}
	}

	return false;
}

void MapPane::changeHexSupply( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int s = ( sit_[selected_hex_point_.y][selected_hex_point_.x].getSupply( ) == SupplyStatus::NONE ? SupplyStatus::REG : SupplyStatus::NONE );

	sit_[selected_hex_point_.y][selected_hex_point_.x].setSupply( s );

	paintCurrentHex( );
}

void MapPane::updateAllSupply( )
{
#if 0
	if ( wxMessageBox(	"This will update supply status of ALL units (both\n"
						"your and enemy) based on supply status of the hex\n"
						"they occupy. This should be done only once per player\n"
						"turn. Proceed?", Application::NAME, wxICON_QUESTION|wxYES_NO) != wxYES )
		return;
#endif
	contents_pane_->deselectAll( );
	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
		{
			int hs = sit_[y][x].getSupply( );
			Counter* c;
			ENUM_HEXUNITS(x, y, c)
			{
				if ( c->getCounterType( ) == Counter::Type::UNIT )
				{
					GroundUnit* u = (GroundUnit*)c;
					switch ( hs )
					{
						case SupplyStatus::REG:
							u->setSupply( SupplyStatus::REG );
							u->updateSupplyTurns( );
							break;
						case SupplyStatus::SPC:
							u->setSupply( SupplyStatus::SPC );
							u->updateSupplyTurns( );
							break;
						case SupplyStatus::NONE:
							u->setSupply( SupplyStatus::NONE );
							u->updateSupplyTurns( );
							break;
					}
				}
			}
		}

	rePaint( );
	contents_pane_->Refresh( );
}

void MapPane::changeHexSpecialSupply( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	int s = ( sit_[selected_hex_point_.y][selected_hex_point_.x].getSupply( ) == SupplyStatus::SPC ? SupplyStatus::REG : SupplyStatus::SPC );

	sit_[selected_hex_point_.y][selected_hex_point_.x].setSupply( s );

	paintCurrentHex( );
}

void MapPane::changeHexIsolation( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	bool i = ( sit_[selected_hex_point_.y][selected_hex_point_.x].getIsolated( ) ? false : true );

	sit_[selected_hex_point_.y][selected_hex_point_.x].setIsolated( i );

	int s = sit_[selected_hex_point_.y][selected_hex_point_.x].getSupply( );

	// isolated is unsupplied too (not in combat phase)
	if ( _current_phase != Phase::COMBAT_PHASE && s == SupplyStatus::REG )
		sit_[selected_hex_point_.y][selected_hex_point_.x].setSupply( SupplyStatus::NONE );

	paintCurrentHex( );
}

void MapPane::updateAllIsolation( )
{
	contents_pane_->deselectAll( );
	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
		{
			Counter* c;
			ENUM_HEXUNITS(x, y, c)
			{
				if ( c->getCounterType( ) == Counter::Type::UNIT )
					((GroundUnit*)c)->setIsolated( sit_[y][x].getIsolated( ) );
			}
		}
	rePaint( );

	contents_pane_->Refresh( );
}

void MapPane::SetAFHits( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getAFHits( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter # of airfield hits"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setAFHits( v );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

void MapPane::SetAFUsage( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getAFUsage( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter airfield usage"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setAFUsage( v );
}

void MapPane::SetPortHits( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getPortHits( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter # of port hits"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setPortHits( v );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

void MapPane::SetPortUsage( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getPortUsage( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter port usage"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setPortUsage( v );
}

void MapPane::SetCDHits( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getCDHits( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter # of coastal defenses hits"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setCDHits( v );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

void MapPane::SetHarassment( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getHarassment( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter # of harassment hits"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setHarrasment( v );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

#if 0
void MapPane::SetAxMines( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getAxisMines( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter # of axis mines"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setAxisMines( v );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

void MapPane::SetAlMines( )
{
	if ( ! isHexSelected( ) )
	{
		SelHexErr( );
		return;
	}

	wxString ws;
	ws.Printf( wxT("%d"), sit_[selected_hex_point_.y][selected_hex_point_.x].getAlliedMines( ) );
	wxString ans = wxGetTextFromUser( wxT("Enter # of allied mines"), Application::NAME, ws, this );
	if ( ans == wxT("") )
		return;

	int v = strtol( ans.ToAscii( ), 0, 0 );
	sit_[selected_hex_point_.y][selected_hex_point_.x].setAlliedMines( v );
	updateHitMarkers( selected_hex_point_.x, selected_hex_point_.y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

void MapPane::LayMines( )
{
	if ( ! _selected_unit_list )
	{
		SelUnitErr( );
		return;
	}

	if ( _selected_unit_list->getCounterType( ) != CNTR_SHIP )
	{
		wxMessageBox( wxT("Only ships can lay mines") );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// TODO: must be taskforce!
	// TODO: check MP
	_selected_unit_list->setMP( _selected_unit_list->getMP( ) - 5 );

	if ( _current_player == AXIS )
		sit_[y][x].setAxisMines( sit_[y][x].getAxisMines( ) + 1 );
	else
		sit_[y][x].setAlliedMines( sit_[y][x].getAlliedMines( ) + 1 );

	updateHitMarkers( x, y );
	contents_pane_->Refresh( );
	paintCurrentHex( );
}

void MapPane::SweepMines( )
{
	wxMessageBox( wxT("Mine sweeping not yet implemented, use setup mode instead.") );
}
#endif

#endif
