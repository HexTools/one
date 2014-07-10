#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "hextools.h"
using ht::wxS;
using ht::coordinatesOutOfBounds;

#include "application.h"
#include "selectedunits.loop"
#include "hexunits.loop"
#include "sideplayer.h"
#include "phase.h"
#include "stackingmode.h"
#include "disruptionstatus.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"

#include "rulesvariant.h"
#include "rules.h"
#include "frame.h"
#include "mappane.h"
#include "counter.h"
#include "armorantitank.h"
#include "groundunit.h"
#include "airunitsubtype.h"
#include "airunittype.h"
#include "airunit.h"
#include "navalunit.h"
#include "hexcontents.h"
#include "hexcontentspane.h"
#include "views.h"
#include "dview.h"
#include "dsearch.h"

DECLARE_APP(Application);

extern int _unit_editor_active;			// (frame.cpp)

extern char msg[];		// 10000
extern Counter* _selected_unit_list;
extern int _current_player;
extern int _phasing_player;
#if 0
extern int _current_phase;						// COMBAT_PHASE etc.
#endif
extern int _year;
extern int _month;

extern int _show_combat_markers;

extern Rules _rule_set;

extern ViewDlgXfer _view_dialog_data;

// very quick and dirty case-insensitive strstr
static char* stristr( const char* s1, const char* s2 )
{
	int len = strlen( s1 );

	char u1[100];
	for ( int i = 0; i < len; ++i )
		u1[i] = toupper( s1[i] );
	u1[len - 1] = '\0';

	len = strlen( s2 );

	char u2[100];
	for ( int i = 0; i < len; ++i )
		u2[i] = toupper( s2[i] );
	u2[len - 1] = '\0';

	return strstr( u1, u2 );  // better not dereference returned pointer...
}

// ret: 0=not found, !0=found
static int search_func( const char* s1, const char* s2, bool whole_string, bool case_sens )
{
	if ( whole_string )
	{
		if ( case_sens )
			return ! strcmp( s1, s2 );
		else
#ifdef __WXGTK__
			return ! strcasecmp( s1, s2 );
#else
			return ! stricmp( s1, s2 );
#endif
	}
	else // partial match ok
		return (int)( case_sens ? strstr( s1, s2 ) : stristr( s1, s2 ) );
}

// search for stuff (0=hex, 1=city, 2=unit)
// NOTE: since 2.2.4 only 'what = -1' used
void MapPane::Search( int what )
{
	// search options are static so we remember them from call to call:
	static bool unit_id = false;
	static bool desc = false;
	static bool air = false;
	static bool hx = false;
	static bool city = false;
	static bool casesens = false;
	static bool matchwhole = false;

	static wxString prev_s;
	static int prev_what = -1;
	static int prev_y;
	static int prev_x;
	static int prev_line = 0;

	static DSearch* dlg = 0;  // may not be static or crash will occur
	// create dialog when we come here first time
	if ( ! dlg )
		dlg = new DSearch( this );

	// search again (what == -1)
	if ( what == -1 && prev_what < 0 )
		what = 1;

	wxString s;
	bool again;

	// new search
	if ( what != -1 )
	{
		dlg->SetSearchText( wxT("") );

		_unit_editor_active = true;  // oh woe
		if ( dlg->ShowModal( ) != wxID_OK )
		{
			_unit_editor_active = false;  // oh woe
			return;
		}
		_unit_editor_active = false;  // oh woe

		s = dlg->GetSearchText( );
		if ( s.Length( ) == 0 )
			return;  // no text entered

		dlg->AddHistory( s );

		unit_id = dlg->UnitIdSelected( );
		desc = dlg->UnitDescriptionSelected( );
		air = dlg->PlanesSelected( );
		city = dlg->CitiesSelected( );
		hx = dlg->HexesSelected( );
		matchwhole = dlg->WholeNameOnly( );
		casesens = dlg->CaseSensitive( );

		prev_y = 0;
		prev_x = 0;
		prev_line = 0;

		again = false;
	}
	else // repeat search
	{
		what = prev_what;
		s = prev_s;

		again = true;
	}

	char bf[200];

	bool found = false;
	bool found_unit = false;

	int line = 0;
	int y = 0;
	int x = 0;
	for ( y = prev_y; y < world_y_size_; ++y )
	{
		for ( x = prev_x; x < world_x_size_; ++x )
		{
			// hex IDs
			if ( hx )
			{
				hex_[y][x].getHexIdStr( bf );
				if ( search_func( bf, s.ToAscii( ), matchwhole, casesens ) )
				{
					found = true;
					goto end;
				}
			}
			// cities
			if ( city )
			{
				City* cptr = hex_[y][x].city_;
				while ( cptr )
				{

					if ( search_func( cptr->name_, s.ToAscii( ), matchwhole, casesens ) )
					{
						found = true;
						goto end;
					}
					cptr = cptr->getNext( );
				}
			}

			// units
			if ( unit_id || desc )
			{
				line = 0;
				// continue old search:
				if ( again && selected_hex_point_.x == x && selected_hex_point_.y == y && y == prev_y && x == prev_x )
				{
					Counter* c;
					while ( ( c = contents_pane_->getUnit( line ) ) != NULL )
					{
						if ( line <= prev_line )
						{
							++line;
							continue;
						}
						if ( c->getCounterType( ) == Counter::Type::UNIT )
						{
							if ( desc )
								( (GroundUnit*)c )->getUnitString( bf );
							else
								( (GroundUnit*)c )->getID( bf );

							if ( search_func( bf, s.ToAscii( ), matchwhole, casesens ) )
							{
								found = true;
								found_unit = true;
								goto end;
							}
						}
						else if ( c->getCounterType( ) == Counter::Type::SHIP )
						{
							( (NavalUnit*)c )->getName( bf );
							if ( search_func( bf, s.ToAscii( ), matchwhole, casesens ) )
							{
								found_unit = true;
								found = true;
								goto end;
							}
						}
						++line;
					}
				}
				else
				{
					Counter* c;
					ENUM_HEXUNITS(x, y, c)
					{
						if ( c->getCounterType( ) == Counter::Type::UNIT )
						{
							if ( desc )
								( (GroundUnit*)c )->getUnitString( bf );
							else
								( (GroundUnit*)c )->getID( bf );
							if ( search_func( bf, s.ToAscii( ), matchwhole, casesens ) )
							{
								found = true;
								found_unit = true;
								goto end;
							}
						}
						else if ( c->getCounterType( ) == Counter::Type::SHIP )
						{
							( (NavalUnit*)c )->getName( bf );
							if ( search_func( bf, s.ToAscii( ), matchwhole, casesens ) )
							{
								found = true;
								found_unit = true;
								goto end;
							}
						}
						++line;
					}
				}
			}

			// planes
			if ( air )
			{
				line = 0;
				// continue old search:
				if ( again && selected_hex_point_.x == x && selected_hex_point_.y == y && y == prev_y && x == prev_x )
				{
					Counter* c;
					while ( ( c = contents_pane_->getUnit( line ) ) != NULL )
					{
						if ( line <= prev_line )
						{
							++line;
							continue;
						}
						if ( c->getCounterType( ) == Counter::Type::PLANE )
						{
							strcpy( bf, ( (AirUnit*)c )->getName( ) );
							if ( search_func( bf, s.ToAscii( ), matchwhole, casesens ) )
							{
								found = true;
								found_unit = true;
								goto end;
							}
						}
						++line;
					}
				}
				else
				{
					Counter* c;
					ENUM_HEXUNITS(x, y, c)
					{
						if ( c->getCounterType( ) == Counter::Type::PLANE )
						{
							strcpy( bf, ( (AirUnit*)c )->getName( ) );
							if ( search_func( bf, s.ToAscii( ), matchwhole, casesens ) )
							{
								found = true;
								found_unit = true;
								goto end;
							}
						}
						++line;
					}
				}
			}

		}
		prev_x = 0;
	}

	end: if ( found )
	{
		SelectHex( x, y );
		Refresh( );
		// emulate mouseclick in the hex
#if 0
		 if (ptCurrentHex.x != x || ptCurrentHex.y != y)
		 {
		 wxMouseEvent e;
		 e.m_x = x;
		 e.m_y = y;
		 Hex2Screen(&(e.m_x), &(e.m_y));
		 e.m_x /= Scale;
		 e.m_y /= Scale;
		 ++e.m_x;
		 ++e.m_y;
		 EvLButtonDown(e);
		 EvLButtonUp(e);
		 }
#endif
		if ( found_unit )
			contents_pane_->selectUnit( line );

		dont_redraw_map_ = 0;
	}
	else
		wxMessageBox( wxT("Not found"), Application::NAME, wxICON_INFORMATION );

	prev_what = what;
	prev_s = s;
	prev_x = x;
	prev_y = y;
	prev_line = line;
}

void MapPane::ShowCombatMarkers( )
{
	_unit_editor_active = 1;

	wxString ans;
	if ( _show_combat_markers )
		ans = wxGetTextFromUser( wxT("Enter password that will be needed to re-enable combat markers (so\n"
				"that your opponent can't take a peek to your plans):"), Application::NAME, wxT(""), this );
	else
		ans = wxGetPasswordFromUser( wxT("Enter password"), Application::NAME, wxT(""), this );

	_unit_editor_active = 0;

	if ( ans == wxT("") )
		return;

	if ( ans.Len( ) >= 50 )
	{
		wxMessageBox( wxT("Too long password!") );
		return;
	}

	// trying to hide
	if ( _show_combat_markers )
	{
		strcpy( marker_password_, ans.ToAscii( ) );
		_show_combat_markers = FALSE;
	}
	// trying to re-enable
	else
	{
		if ( strcmp( marker_password_, ans.ToAscii( ) ) != 0 )
		{
			wxMessageBox( wxT("Invalid password"), Application::NAME, wxOK | wxICON_HAND );
			return;
		}
		_show_combat_markers = TRUE;
	}
#if 0
	char*  p = scramble_passwd( MarkerPasswd );
	sprintf( msg, "scrambled='%s', descrambled='%s'", p, descramble_passwd( p ) );
	wxMessageBox( msg );
#endif
	Refresh( );

	// TODO: DTM
	wxGetApp( ).frame_->SetupMenu( );
#if 0
	wxGetApp( ).SetupMenu( Phase );
#endif
}

void MapPane::peekAtSelectedPartisans( )
{
	if ( ! _selected_unit_list )
	{
		wxMessageBox( wxT("Select some partisans first"), Application::NAME );
		return;
	}
	strcpy( msg, "Partisan counters:\n" );

	int cnt = 0;

	Counter* cp;
	ENUM_SELUNITS( cp )
	{
		if ( cp->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cp )->getType( ) == GroundUnitType::WW2::PARTISAN )
		{
			int o_ax = _rule_set.HideAxPartisans;
			int o_al = _rule_set.HideAlPartisans;
			_rule_set.HideAxPartisans = FALSE;
			_rule_set.HideAlPartisans = FALSE;
			char bf[200];
			( (GroundUnit*)cp )->getUnitString( bf );
			strcat( msg, bf );
			strcat( msg, "\n" );
			_rule_set.HideAxPartisans = o_ax;
			_rule_set.HideAlPartisans = o_al;
			++cnt;
		}
	}

	if ( cnt == 0 )
		wxMessageBox( wxT("No partisans selected!"), Application::NAME );
	else
	{
		if ( wxMessageBox( wxT("Really peek these partisans?"), Application::NAME, wxYES_NO ) != wxYES )
			return;
		wxMessageBox( wxS( msg ), Application::NAME );
	}
}

void MapPane::HiliteWZ( int wz )
{
	int x2, y2;
	GetClientSize( &x2, &y2 );

	int x1 = 0;
	int y1 = 0;
	screen2Hex( &x1, &y1 );
	screen2Hex( &x2, &y2 );

	x1 = ( x1 < 0 ? 0 : x1 );
	y1 = ( y1 < 0 ? 0 : y1 );
	x2 = ( x2 >= world_x_size_ ? world_x_size_ - 1 : x2 );
	y2 = ( y2 >= world_y_size_ ? world_y_size_ - 1 : y2 );

	for ( int y = y1; y <= y2; ++y )
		for ( int x = x1; x <= x2; ++x )
			if ( hex_[y][x].getWeatherZone( ) == wz )
				HiLiteHex( x, y, true );
}

// this one is really used to hilite invalid stacks
bool MapPane::IsValidStack( int x, int y )
{
	// NONDIV_RE or NONDIV_UNIT
	int ndivmode = _rule_set.StackNDivType;

	// default, Regular stacking
	float maxdiv = _rule_set.StackDivs;
	float maxndiv = _rule_set.StackNDivs;
	float maxart = _rule_set.StackArt;
	bool artxxok = true;

	// exception, Mountain terrain / Reduced stacking
	if ( hex_[y][x].terrain_ == HexType::MOUNTAIN )
	{
		maxdiv = _rule_set.StackMDivs;
		maxndiv = _rule_set.StackMNDivs;
		maxart = _rule_set.StackMArt;
		artxxok = false;
	}
	// exception, Arctic geography / Limited stacking
	else if ( hex_[y][x].getWeatherZone( ) == 0 ) // TODO: proper Arctic logic
	{
		maxdiv = _rule_set.StackADivs;
		maxndiv = _rule_set.StackANDivs;
		maxart = _rule_set.StackAArt;
		artxxok = false;
	}

	float div = 0.0F; // TODO: why is the div count a float vice int?

	float ndiv = 0.0F;
	float art = 0.0F;
	int fsize = -1;

	GroundUnit* fu = NULL;
	Counter* c;

	ENUM_HEXUNITS(x, y, c)
	{
		if ( c->getCounterType( ) != Counter::Type::UNIT )
			continue;

		GroundUnit* u = static_cast<GroundUnit*>( c );
		if ( u->getOverstack( ) )
			continue;

		// ww1: fortification unit of any size allowed, so search for the biggest
		if (	_rule_set.StackWW1AllowOneFort == TRUE &&
				( u->getType( ) == GroundUnitType::WW2::FORT || u->getType( ) == GroundUnitType::WW2::PORT_FORT ) &&
				( u->getSize( ) > fsize ) )
			fsize = u->getSize( ), fu = u;

		// many types are not considered when calculating stacking limit:
		if ( u->getType( ) == GroundUnitType::WW2::PORT_FORT || u->getType( ) == GroundUnitType::WW2::POS_HV_AA || u->getType( ) == GroundUnitType::WW2::POS_LT_AA
			|| u->getType( ) == GroundUnitType::WW2::TRUCK || u->getType( ) == GroundUnitType::WW2::FORTAREA || u->getType( ) == GroundUnitType::WW2::TRANSPORT
			|| u->getType( ) == GroundUnitType::WW2::TANKFACT || u->getType( ) == GroundUnitType::WW2::ARTYFACT || u->getSize( ) == 0 || u->getSize( ) == GroundUnit::Size::HQ
			|| u->getSize( ) == GroundUnit::Size::ARMYHQ || u->getSize( ) == GroundUnit::Size::CORPSHQ )
			continue;  // special units are not counted
		else if ( u->getSize( ) >= GroundUnit::Size::DIVISIONAL && ! u->isCadre( ) )
		{
			if ( u->isArtillery( ) && artxxok && maxart != 0 )  // art XX
				maxart = 0;
			else
				++div;
		}
		else if ( u->isArtillery( ) )
		{
			art += ( ndivmode == StackingMode::NONDIV_RE ) ? u->getRE( ) : 1;
#if 0
			if ( ndivmode == STACK_RE )
				art += u->getRE( );
			else
				++art;
#endif
		}
		else if ( ndivmode == StackingMode::NONDIV_RE )
			ndiv += u->getRE( );
		else if ( u->getRE( ) > 0 )  // don't count 0-RE units
			++ndiv;
	}

	// ww1: reduce one fortress unit
	if ( _rule_set.StackWW1AllowOneFort == TRUE && fu != NULL && fsize != -1 )
	{
		if ( fsize >= GroundUnit::Size::DIVISIONAL )
			--div;
		else
		{
			ndiv -= ( ndivmode == StackingMode::NONDIV_RE ) ? fu->getRE( ) : 1;
#if 0
			if ( ndivmode == StackingMode::NONDIV_RE )
				ndiv -= fu->getRE( );
			else
				--ndiv;
#endif
		}
	}

	if ( div < maxdiv )
		ndiv -= ( maxdiv - div );

	if ( ndiv < maxndiv )
		art -= ( maxndiv - ndiv );

	if ( div > maxdiv ||
		 ndiv > maxndiv ||
		 art > maxart )
		return false;

	return true ;
}

//check stacking limits
// TODO: outdated, doesn't work!!!! (then again this is not used...)
//  IsValidStack(..) (below) is actually used!
void MapPane::CheckStacks( )
{
	int div, ndiv, art;
	int maxdiv, maxndiv, maxart, ndivmode, i;
	Counter* c;
	GroundUnit* u;
	char s[200];

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
		{
			div = ndiv = art = 0;
			// what limit to use?
			ndivmode = _rule_set.StackNDivType;
			if ( hex_[y][x].getWeatherZone( ) == 0 )
			{
				maxdiv = _rule_set.StackADivs;
				maxndiv = _rule_set.StackANDivs;
				maxart = _rule_set.StackAArt;
			}
			else if ( hex_[y][x].terrain_ == HexType::MOUNTAIN )
			{
				maxdiv = _rule_set.StackMDivs;
				maxndiv = _rule_set.StackMNDivs;
				maxart = _rule_set.StackMArt;
			}
			else
			{
				maxdiv = _rule_set.StackDivs;
				maxndiv = _rule_set.StackNDivs;
				maxart = _rule_set.StackArt;
			}
			ENUM_HEXUNITS(x, y, c)
			{
				if ( c->getCounterType( ) != Counter::Type::UNIT )
					continue;

				u = (GroundUnit*)c;
				if ( u->getOverstack( ) )  // overstacked units are not counted
					continue;
				if ( u->getType( ) == GroundUnitType::WW2::PORT_FORT || u->getType( ) == GroundUnitType::WW2::POS_HV_AA || u->getType( ) == GroundUnitType::WW2::POS_LT_AA || u->getType( ) == GroundUnitType::WW2::TRUCK || u->getSize( ) == GroundUnit::Size::NO_SIZE || u->getSize( ) == GroundUnit::Size::HQ )
					continue;  // special units are not counted
				if ( u->isArtillery( ) )
				{
					if ( ndivmode == StackingMode::NONDIV_RE )
						art += (int)( u->getRE( ) );
					else
						++art;
				}
				else if ( u->getSize( ) >= GroundUnit::Size::DIVISIONAL )
					++div;
				else if ( ndivmode == StackingMode::NONDIV_RE )
					ndiv += (int)( u->getRE( ) );
				else if ( u->getRE( ) > 0 )  // don't count 0-RE units
					++ndiv;
			}
			strcpy( msg, "" );
			i = 0;
			if ( div < maxdiv )
				maxndiv += ( maxdiv - div);
			if ( ndiv < maxndiv )
				maxart += ( maxndiv - ndiv);
			if ( div > maxdiv )
			{
				sprintf( s, "Too many divisional units (%d allowed, %d present).\n", maxdiv, div );
				strcat( msg, s );
				++i;
			}
			if ( ndiv > maxndiv )
			{
				sprintf( s, "Too many nondivisional units (%d%s allowed, %d%s present).\n", maxndiv,
						ndivmode == StackingMode::NONDIV_RE ? " REs" : "", ndiv, ndivmode == StackingMode::NONDIV_RE ? " REs" : "" );
				strcat( msg, s );
				++i;
			}
			if ( art > maxart )
			{
				sprintf( s, "Too many artillery units (%d%s allowed, %d%s present).\n", maxart,
						ndivmode == StackingMode::NONDIV_RE ? " REs" : "", art, ndivmode == StackingMode::NONDIV_RE ? " REs" : "" );
				strcat( msg, s );
				++i;
			}
			if ( i )
			{
				selected_hex_point_.x = x;
				selected_hex_point_.y = y;
				HiLiteHex( x, y, true );

				char id[20];
				hex_[y][x].getHexIdStr( id );

				sprintf( s, "at hex %s\n\n(Select Cancel to end checking)", id );
				strcat( msg, s );
				if ( wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxCANCEL | wxICON_QUESTION) == wxCANCEL )
					return;

				UnHiLiteHex( );
			}
		}
}

// 1=armor, 2=art, 3=cdr, 4=all planes, 5=in air planes, 6=illegal stacks
// 7=c/m units, ...
void MapPane::HiliteStuff( int what )
{
	// interceptor sources
	if ( what == 15 && coordinatesOutOfBounds( selected_hex_point_.x, selected_hex_point_.y, world_x_size_, world_y_size_ ) )
	{
		wxMessageBox( wxT("Select hex first. Hexes containing interceptors\nthat can reach selected hex will be highlighted."), Application::NAME );
		return;
	}

	int x2, y2;
	GetClientSize( &x2, &y2 );

	int x1 = 0;
	int y1 = 0;
	screen2Hex( &x1, &y1 );
	screen2Hex( &x2, &y2 );

	x1 = ( x1 < 0 ? 0 : x1 );
	y1 = ( y1 < 0 ? 0 : y1 );
	x2 = ( x2 >= world_x_size_ ? world_x_size_ - 1 : x2 );
	y2 = ( y2 >= world_y_size_ ? world_y_size_ - 1 : y2 );

	Counter* cntr;
	for ( int y = y1; y <= y2; ++y )
		for ( int x = x1; x <= x2; ++x )
		{
			int i = FALSE;
			switch ( what )
			{
				case 1:  // armor
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getAECA( ) == ArmorAntitank::Proportion::HALF || cntr->getAECA( ) == ArmorAntitank::Proportion::FULL )
						{
							i = TRUE;
							break;
						}
					break;
				case 2:  // artillery
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->isArtillery( ) )
						{
							i = TRUE;
							break;
						}
					break;
				case 3:  // cadres
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->isCadre( ) )
						{
							i = TRUE;
							break;
						}
					break;
				case 4:  // all planes
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::PLANE)
						{
							i = TRUE;
							break;
						}
					break;
				case 5:  // flying planes
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)cntr )->getInAir( ) )
						{
							i = TRUE;
							break;
						}
					break;
				case 6:  // illegal stacks
					if ( ! IsValidStack( x, y ) )
						i = TRUE;
					break;
				case 7:  // c/m
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->isSupplementalMotorized( ) )
						{
							i = TRUE;
							break;
						}
					break;
				case 8:  // cons
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->isConstructionEngineer( ) )
						{
							i = TRUE;
							break;
						}
					break;
				case 9:  // rreng
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::RR_ENG)
						{
							i = TRUE;
							break;
						}
					break;
				case 10:  // aborted planes
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)cntr )->getAborted( ) )
						{
							i = TRUE;
							break;
						}
					break;
				case 11:  // trucks
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getType( ) == GroundUnitType::WW2::TRUCK )
						{
							i = TRUE;
							break;
						}
					break;
				case 12:  // air bases
					if ( GetRawAFCapacity( x, y ) > 0 )
						i = TRUE;
					break;
				case 13:  // inop planes
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::PLANE && ( (AirUnit*)cntr )->isActive( ) == FALSE )
						{
							i = TRUE;
							break;
						}
					break;
				case 14:  // disrupted units
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)cntr )->getDisruption( ) != DisruptionStatus::NOT_DISRUPTED )
						{
							i = TRUE;
							break;
						}
					break;
				case 15:  // enemy air bases with interceptors that can reach here
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::PLANE )
						{
							if ( cntr->getSide( ) != _current_player
								&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F
									|| ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF )
								&& ! ( (AirUnit*)cntr )->getAborted( ) && ( (AirUnit*)cntr )->isActive( )
								&& ! ( (AirUnit*)cntr )->getInAir( ) )
							{
								if ( ht::DistBetweenHexes( selected_hex_point_.x, selected_hex_point_.y, x, y ) <= ( (AirUnit*)cntr )->getMaxMP( ) / 2 )
								{
									i = 1;
								}
							}
						}
					break;
				case 16:  // enemy air bases with patrol attackers that can reach here
					int max_mp;
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::PLANE )
						{
							if ( cntr->getSide( ) != _current_player
								&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F
									|| ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF )
								&& ! ( (AirUnit*)cntr )->getAborted( ) && ( (AirUnit*)cntr )->isActive( )
								&& ! ( (AirUnit*)cntr )->getInAir( ) && ! ( (AirUnit*)cntr )->getDidPA( ) )
							{
								if ( cntr->getSide( ) == SidePlayer::ALLIED && ( (AirUnit*)cntr )->getSubType( ) == AirArmedForce::Allied::AIR_SOV )
									max_mp = 2;
								// guards unit have 1/2 range after I/10/42
								else if ( cntr->getSide( ) == SidePlayer::ALLIED && ( (AirUnit*)cntr )->getSubType( ) == AirArmedForce::Allied::AIR_GUARDS
									&& _year <= 42 && _month < 9 )
									max_mp = 2;
								else
									max_mp = ( (AirUnit*)cntr )->getMaxMP( ) / 2;

								if ( ht::DistBetweenHexes( selected_hex_point_.x, selected_hex_point_.y, x, y ) <= max_mp )
									i = 1;
							}
						}
					break;
				case 17:  // hits
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getCounterType( ) == Counter::Type::HIT )
						{
							i = TRUE;
							break;
						}
					break;
#if 0
					 case 18:
					 ENUM_HEXUNITS(x, y, cntr)
					 if (cntr->GetSide() == Player)
					 {
					 if (cntr->Type() == CNTR_UNIT)
					 {
					 if (	((GroundUnit*)cntr)->GetMP() == ((GroundUnit*)cntr)->GetMaxMP() &&
					 ((GroundUnit*)cntr)->GetMP() > 0 )
					 {
					 i = TRUE;
					 break;
					 }
					 else if ( RuleSet.rules_variant_ == ERV_A &&
					 Phase == EXPLOITATION_PHASE &&
					 !((GroundUnit*)cntr)->IsSupplementalMotorized() &&
					 ((GroundUnit*)cntr)->GetType() != CAV &&
					 ((GroundUnit*)cntr)->GetType() != HVCAV &&
					 ((GroundUnit*)cntr)->GetType() != PRTSNCAV &&
					 ((GroundUnit*)cntr)->GetMP() == ((GroundUnit*)cntr)->GetMaxMP()/2 &&
					 ((GroundUnit*)cntr)->GetMP() > 0 )
					 {
					 i = TRUE;
					 break;
					 }
					 }
					 else if (cntr->Type() == CNTR_PLANE)
					 {
					 if (	((AirUnit*)cntr)->GetMP() == ((AirUnit*)cntr)->GetMaxMP() &&
					 ((AirUnit*)cntr)->GetMP() > 0 &&
					 ((AirUnit*)cntr)->IsActive())
					 {
					 i = TRUE;
					 break;
					 }
					 }
					 }
					 break;
#endif
				case 18:  // Highlight unmoved units
					ENUM_HEXUNITS(x, y, cntr)
						if ( cntr->getSide( ) == _current_player )
						{
							if ( cntr->getCounterType( ) == Counter::Type::UNIT )
							{
								GroundUnit* unit = (GroundUnit*)cntr;
								if ( unit->getMP( ) == unit->getMaxMP( ) && unit->getMP( ) > 0 )
								{
									i = TRUE;
									break;
								}
#if 0
								else if ( _rule_set.rules_variant_ == RulesVariant::A && _current_phase == Phase::EXPLOITATION_PHASE
									&& ! unit->isSupplementalMotorized( ) && unit->getType( ) != GroundUnitType::WW2::CAV
									&& unit->getType( ) != GroundUnitType::WW2::HVCAV && unit->getType( ) != GroundUnitType::WW2::PRTSNCAV
									&& unit->getMP( ) == unit->getMaxMP( ) / 2 && unit->getMP( ) > 0 )
								{
									i = TRUE;
									break;
								}
#endif
							}
#if 0
							 else if (cntr->Type() == CNTR_PLANE)
							 {
							 if (((AirUnit*)cntr)->GetMP() == ((AirUnit*)cntr)->GetMaxMP()
							 && ((AirUnit*)cntr)->GetMP() > 0
							 && ((AirUnit*)cntr)->IsActive())
							 {
							 i = TRUE;
							 break;
							 }
							 }
#endif
						}
					break;
				case 19:  // Normal Gauge Rail
					if ( IsRRInHex( x, y ) && sit_[y][x].getGauge( ) == SidePlayer::AXIS )
						i = TRUE;
					break;
				case 20:  // Wide Gauge Rail
					if ( IsRRInHex( x, y ) && sit_[y][x].getGauge( ) == SidePlayer::ALLIED )
						i = TRUE;
					break;
				case 21:  // Own Normal Gauge Rail
					if ( IsRRInHex( x, y ) && sit_[y][x].getGauge( ) == SidePlayer::AXIS && sit_[y][x].getOwner( ) == _phasing_player )
						i = TRUE;
					break;
				case 22:  // Own Wide Gauge Rail
					if ( IsRRInHex( x, y ) && sit_[y][x].getGauge( ) == SidePlayer::ALLIED && sit_[y][x].getOwner( ) == _phasing_player )
						i = TRUE;
					break;
				case 23:  // Highlight Enemy Full or Reduced ZOCs
					if ( isHexInEnemyRZOC( x, y ) || isHexInEnemyZOC( x, y ) )
						i = TRUE;
					break;
				case 24:  // Full Enemy ZOCs
					if ( isHexInEnemyZOC( x, y ) )
						i = _rule_set.ReducedZOCs == FALSE;
					break;
				case 25:  // Reduced Enemy ZOCs
					if ( isHexInEnemyRZOC( x, y ) || ( isHexInEnemyZOC( x, y ) && _rule_set.ReducedZOCs == TRUE ) )
						i = TRUE;
					break;

			}
			if ( i )
				HiLiteHex( x, y, true );
		}
}

// private helper, for HiliteArea(..)
void MapPane::HiLiteHexLight( int x, int y )  // highlight hex
{
	if ( ! map_DC_ )
	{
		map_DC_ = new wxClientDC( this );
		if ( ! map_DC_ )
		{
			wxMessageBox( wxT("Panic: failed to allocate DC!"), wxT("FATAL ERROR"), 0 );
			return;
		}
	}
	map_DC_->SetUserScale( 1.0 / scale_, 1.0 / scale_ );
	int x2 = x;
	int y2 = y;
	hex2Screen( &x2, &y2 );

	wxPen pen( wxColour( 255, 80, 80 ), 2, wxSOLID );
	map_DC_->SetPen( pen );
	//    HiliteX = x;
	//	  HiliteY = y;
	map_DC_->DrawLine( x2, y2 + 3, x2 + Hex::HALF, y2 - 3 );
	map_DC_->DrawLine( x2 + Hex::HALF, y2 - 3, x2 + Hex::SIZE_X, y2 + 3 );
	map_DC_->DrawLine( x2 + Hex::SIZE_X, y2 + 3, x2 + Hex::SIZE_X, y2 + Hex::SIZE_Y - 3 );
	map_DC_->DrawLine( x2 + Hex::SIZE_X, y2 + Hex::SIZE_Y - 3, x2 + Hex::HALF, y2 + Hex::SIZE_Y + 3 );
	map_DC_->DrawLine( x2 + Hex::HALF, y2 + Hex::SIZE_Y + 3, x2, y2 + Hex::SIZE_Y - 3 );
	map_DC_->DrawLine( x2, y2 + Hex::SIZE_Y - 3, x2, y2 + 3 );
}

// hilite 'range' hexes around x,y
void MapPane::HiliteArea( int x, int y, int range )
{
	int x1 = x - range;
	int x2 = x + range;
	int y1 = y - range;
	int y2 = y + range;

	x1 = ( x1 < 0 ? 0 : x1 );
	y1 = ( y1 < 0 ? 0 : y1 );
	x2 = ( x2 >= world_x_size_ ? world_x_size_ - 1 : x2 );
	y2 = ( y2 >= world_y_size_ ? world_y_size_ - 1 : y2 );

	for ( int yy = y1; yy <= y2; ++yy )	
	{
		int fix0, fix1;

		int d = ( yy - y );
		if ( d < 0 )
			d = -d;
		fix0 = d / 2;

		fix1 = ( d + 1 ) / 2;
		d -= range;
		if ( d < 0 )
			d = -d;

		// if y is even, then the even lines go 1 hex to the right
		if ( ! ( y % 2 ) && yy % 2 )
			++fix0, --fix1;

		for ( int xx = x - d - fix0; xx <= x + d + fix1; ++xx )
			HiLiteHexLight( xx, yy );
	}
}

// hilite zones around specific units
// this works in different way than HiliteStuff, that's why it has own function
// 1..4 = interception zones
// 5    = patrol attack zones
void MapPane::HiliteZones( int what )
{
	int x2, y2;
	GetClientSize( &x2, &y2 );

	int x1 = 0;
	int y1 = 0;
	screen2Hex( &x1, &y1 );
	screen2Hex( &x2, &y2 );

	// since zone sources can be outside visible region we must enlarge
	// analyzed area. we do this by guessing nice number of hexes:
	const int extra_range = 15;
	x1 -= extra_range;
	x2 += extra_range;
	y1 -= extra_range;
	y2 += extra_range;

	x1 = ( x1 < 0 ? 0 : x1 );
	y1 = ( y1 < 0 ? 0 : y1 );
	x2 = ( x2 >= world_x_size_ ? world_x_size_ - 1 : x2 );
	y2 = ( y2 >= world_y_size_ ? world_y_size_ - 1 : y2 );

	int str = 0;
	switch ( what )
	{
		case 1:
			str = 0;
			break;
		case 2:
			str = 4;
			break;
		case 3:
			str = 6;
			break;
		case 4:
			str = 8;
			break;
	}

	Counter* cntr;

	// 1st sweep: hilite zones
	for ( int y = y1; y <= y2; ++y )
	{
		for ( int x = x1; x <= x2; ++x )
		{
			switch ( what )
			{
				case 1:  // intercp.
				case 2:  // intercp.
				case 3:  // intercp.
				case 4:  // intercp.
					ENUM_HEXUNITS(x, y, cntr)
					{
						// active enemy interceptors?
						if ( cntr->getCounterType( ) == Counter::Type::PLANE )
						{
							if ( cntr->getSide( ) != _current_player
								&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F
									|| ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF )
								&& ! ( (AirUnit*)cntr )->getAborted( ) && ( (AirUnit*)cntr )->isActive( )
								&& ( (AirUnit*)cntr )->getAtt( ) > str && ! ( (AirUnit*)cntr )->getInAir( ) )
								HiliteArea( x, y, ( (AirUnit*)cntr )->getMaxMP( ) / 2 );
						}
					}
					break;
				case 5:  // patrol attack.
					ENUM_HEXUNITS(x, y, cntr)
					{
						// active enemy interceptors?
						if ( cntr->getCounterType( ) == Counter::Type::PLANE )
						{
							if ( cntr->getSide( ) != _current_player
								&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F
									|| ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF )
								&& ! ( (AirUnit*)cntr )->getAborted( ) && ( (AirUnit*)cntr )->isActive( )
								&& ! ( (AirUnit*)cntr )->getInAir( ) && ! ( (AirUnit*)cntr )->getDidPA( ) )
							{
								// soviets have shorter pa range:
								if ( cntr->getSide( ) == SidePlayer::ALLIED && ( (AirUnit*)cntr )->getSubType( ) == AirArmedForce::Allied::AIR_SOV )
									HiliteArea( x, y, 2 );
								// guards unit have 1/2 range after I/10/42
								else if ( cntr->getSide( ) == SidePlayer::ALLIED && ( (AirUnit*)cntr )->getSubType( ) == AirArmedForce::Allied::AIR_GUARDS
									&& _year <= 42 && _month < 9 )
									HiliteArea( x, y, 2 );
								else
									HiliteArea( x, y, ( (AirUnit*)cntr )->getMaxMP( ) / 2 );
							}
						}
					}
					break;
			}  // switch
		}  // for x
	}  // for y

	// 2nd sweep: hilite sources
	for ( int y = y1; y <= y2; ++y )
	{
		for ( int x = x1; x <= x2; ++x )
		{
			switch ( what )
			{
				case 1:  // intercp.
				case 2:  // intercp.
				case 3:  // intercp.
				case 4:  // intercp.
					ENUM_HEXUNITS(x, y, cntr)
					{
						// active enemy interceptors?
						if ( cntr->getCounterType( ) == Counter::Type::PLANE )
						{
							if ( cntr->getSide( ) != _current_player
								&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F
									|| ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF )
								&& ! ( (AirUnit*)cntr )->getAborted( ) && ( (AirUnit*)cntr )->isActive( )
								&& ( (AirUnit*)cntr )->getAtt( ) > str && ! ( (AirUnit*)cntr )->getInAir( ) )
								HiLiteHex( x, y, true );
						}
					}
					break;
				case 5:  // patrol attack.
					ENUM_HEXUNITS(x, y, cntr)
					{
						// active enemy interceptors?
						if ( cntr->getCounterType( ) == Counter::Type::PLANE )
						{
							if ( cntr->getSide( ) != _current_player
								&& ( ( (AirUnit*)cntr )->getType( ) == AirUnitType::F
									|| ( (AirUnit*)cntr )->getType( ) == AirUnitType::HF )
								&& ! ( (AirUnit*)cntr )->getAborted( ) && ( (AirUnit*)cntr )->isActive( )
								&& ! ( (AirUnit*)cntr )->getInAir( ) && ! ( (AirUnit*)cntr )->getDidPA( ) )
								HiLiteHex( x, y, true );
						}
					}
					break;
			}  // switch
		}  // for x
	}  // for y
}

// TODO: DLT "views" functions

void MapPane::ShowViewText( )
{
	if ( ! view_text_dialog_ )
	{
		view_text_dialog_ = new DView( this );
		if ( view_text_dialog_ == nullptr )
			wxMessageBox( wxT("Failed to create the Show View Text dialog!"), Application::NAME );
		else
		{
			_view_dialog_data.CurrentView = 0;
			strncpy( _view_dialog_data.editCaption, views_->getViewName( 0 ), HexNote::MAX_CAPTION_LEN - 1 );
			strncpy( _view_dialog_data.editText, views_->getViewText( 0 ), HexNote::MAX_TEXT_LEN - 1 );
			strcpy( _view_dialog_data.editRed, ht::inttoa( views_->getRed( 0 ) ) );
			strcpy( _view_dialog_data.editGreen, ht::inttoa( views_->getGreen( 0 ) ) );
			strcpy( _view_dialog_data.editBlue, ht::inttoa( views_->getBlue( 0 ) ) );
			view_text_dialog_->Show( true );
		}
	}
	else
	{
		// TODO: this conditional is useless, fix it or remove it
		if ( view_text_dialog_->IsShown( ) )
			view_text_dialog_->Show( true );
		else
			view_text_dialog_->Show( true );
	}
}

void MapPane::ShowAllViews( bool show )
{
	for ( int nview = 0; nview < GetViewCount( ); ++nview )
		views_->setViewActive( nview, show && ViewHexCount( nview ) > 0 );

	Refresh( false );
}

void MapPane::ToggleShowView( int nview )
{
	if ( nview < GetViewCount( ) )
	{
		if ( views_->viewIsActive( nview ) )
			views_->setViewActive( nview, false );
		else
		{
			views_->setViewActive( nview, true );
#if 0
			 int pt;
			 for (pt = 0; pt < ViewHexCount(nview); ++pt)
			 HiLiteHexColour(GetViewX(nview,pt), GetViewY(nview,pt), GetViewRed(nview), GetViewGreen(nview), GetViewBlue(nview), TRUE);
			 //for (pt = 0; pt < views->HexCount(nview); ++pt)
			 //	HiLiteHexColour(views->GetViewX(nview,pt), views->GetViewY(nview,pt), views->GetRed(nview), views->GetGreen(nview), views->GetBlue(nview), TRUE);
			 //HiLiteHex(views->GetViewX(nview,pt), views->GetViewY(nview,pt), true);
#endif
		}
		Refresh( false );
	}
}

// PAPI (Frame), but Frame::CmEditView( ) not used anywhere
void MapPane::EditView( int nview )
{
	views_->editView( nview );
	Refresh( false );
}

// PAPI (Frame), from Frame::SetupMenu( )
int MapPane::GetViewCount( )
{
	return views_->getViewCount( );
}

// PAPI (Frame)
int MapPane::GetActiveViewCount( )
{
	return views_->getActiveViewCount( );
}

// PAPI (Frame)
bool MapPane::ViewIsActive( int nview )
{
	return views_->viewIsActive( nview );
}

// PAPI (Frame), from Frame::SetupMenu( )
char* MapPane::GetViewName( int nview )
{
	return views_->getViewName( nview );
}

// PAPI (Frame), from Frame::CmAllViewsOn( )
int MapPane::ViewHexCount( int nview )
{
	return views_->hexCount( nview );
}

void MapPane::UpdateViewData( )
{
	views_->setViewName( _view_dialog_data.CurrentView, _view_dialog_data.editCaption );
	views_->setViewText( _view_dialog_data.CurrentView, _view_dialog_data.editText );
	views_->setViewColor( _view_dialog_data.CurrentView, _view_dialog_data.editRed, _view_dialog_data.editGreen, _view_dialog_data.editBlue );
	Refresh( false );
}

bool MapPane::HexInView( int nview, int x, int y )
{
	return views_->hexInView( nview, x, y );
}

int MapPane::FirstActiveView( )
{
	return views_->firstActiveView( );
}

int MapPane::GetViewX( int nview, int pt )
{
	return views_->getViewX( nview, pt );
}

int MapPane::GetViewY( int nview, int pt )
{
	return views_->getViewY( nview, pt );
}

int MapPane::GetViewRed( int nview )
{
	return views_->getRed( nview );
}

int MapPane::GetViewGreen( int nview )
{
	return views_->getGreen( nview );
}

int MapPane::GetViewBlue( int nview )
{
	return views_->getBlue( nview );
}

bool MapPane::DeletePointFromView( int nview, int x, int y )
{
	return views_->deletePointFromView( nview, x, y );
}

void MapPane::AddPointToView( int nview, int x, int y )
{
	views_->addPointToView( nview, x, y );
}

void MapPane::ClearAllViews( )
{
	views_->clearAllViews( );
}

 // TODO: this is not used
bool MapPane::CreateNewView( int nview )
{
	return views_->createNewView( nview );
}

#endif
