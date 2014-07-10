#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "hextools.h"
using ht::wxS;

#include "application.h"
#include "frame.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "counter.h"
#include "phase.h"
#include "rules.h"
#include "mappane.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "airunit.h"
#include "navalunit.h"
#include "statusmarker.h"
#include "hexcontentspane.h"

#define LINES	MAXUNITS
#define LINEHEIGHT 27   // height of the text line in pixels was: 24
#define LINEWIDTH 231   // width of the text line (and the window) in pixels
#define STEPSPERLINE 2  // scroller steps per line

extern Rules _rule_set;

extern int _current_phase;              // COMBAT_PHASE etc.
extern int _current_player;
extern int _phasing_player;
extern int _show_combat_markers;  // show attack orders?
extern int _show_path;
//extern UnitType UnitTypes[UNITTYPECOUNT];      // unit types

//extern int ResourcesLow;	// from loadsave.cpp

extern wxFont font;		// maprenderer.cpp
extern wxFont bfont;	// maprenderer.cpp (bold font)

extern Counter* _selected_unit_list;	// list of the selected units;

extern wxBitmap* hbmUnitBgnd;
extern wxBitmap* hbmHexContentsPaneBgnd;

static char msg[5000];
static char tmp[100];

// Build a response table for all messages/commands handled
// by the application.

BEGIN_EVENT_TABLE(HexContentsPane, wxScrolledWindow)
EVT_PAINT(HexContentsPane::paint)
EVT_MOTION(HexContentsPane::EvMouseMove)
EVT_LEFT_DOWN(HexContentsPane::EvLButtonDown)
EVT_RIGHT_DOWN(HexContentsPane::EvRButtonDown)
EVT_LEFT_DCLICK(HexContentsPane::EvLButtonDblClk)
END_EVENT_TABLE()

HexContentsPane::HexContentsPane( wxWindow* p, int i, const wxPoint& pt, const wxSize& sz, long WXUNUSED(l) )
		: wxScrolledWindow( p, i, pt, sz )
{
	hbmHexContentsPaneBgnd = new wxBitmap( wxT("graphics/images/uwbgnd.bmp"), wxBITMAP_TYPE_BMP );

	SetBackgroundColour( wxColour( 240, 240, 240 ) );  // (*wxWHITE);
	map_ = nullptr;
	num_units_ = 0;
	hpenDark = new wxPen( wxColour( 150, 150, 150 ), 1, wxSOLID );
	hpenLight = new wxPen( wxColour( 230, 230, 230 ), 1, wxSOLID );

	if ( ! hpenDark || ! hpenLight )
	{
		wxMessageBox( wxT("UnitWindow: pen creation failed!"), wxT("ERROR") );
		exit( -1 );
	}

	for ( int j = 0; j < MAXUNITS; ++j )
	{
		unit_[j] = 0;
		unitline_is_selected_[j] = FALSE;
	}

	SetScrollbars(	0,						// x step
					1,						// y step
					0,						// x range
					MAXUNITS * LINEHEIGHT,	// y range
					0, 0, TRUE );
}

HexContentsPane::~HexContentsPane( )
{
}

// a white box and vertical edges
void HexContentsPane::DrawBox( wxDC* dc, int x1, int y1, int x2, int y2 )
{
	dc->SetPen( *hpenDark );
	dc->DrawLine( x1, y1, x1, y2 );
	dc->SetPen( *hpenLight );
	dc->DrawLine( x2, y1, x2, y2 );
	dc->SetBrush( *wxWHITE_BRUSH );
	dc->SetPen( *wxWHITE_PEN );
	dc->DrawRectangle( x1 + 1, y1, x2 - 1, y2 );
}

// a white box
void DrawBoxNoBorder( wxDC* dc, int x1, int y1, int x2, int y2 )
{
	dc->SetBrush( *wxWHITE_BRUSH );
	dc->SetPen( *wxWHITE_PEN );
	dc->DrawRectangle( x1 + 1, y1, x2 - 1, y2 );
}

void HexContentsPane::RedrawTexts( )
{
	wxClientDC dc( this );
	PrepareDC( dc );  // needed for scrolling
	RedrawGrid( &dc );

	// paint ground units
	int y_ofs = 0;
	char testmsg[1024];
	int restore_black = 0;
	for ( int i = 0; i < num_units_; ++i )
	{
		if ( unit_[i] != NULL)
		{
			// Counter::DrawCounter may change font:
			dc.SetFont( font );

			if ( unitline_is_selected_[i] == TRUE )
			{
				// highlight background:
				dc.DrawBitmap( *hbmUnitBgnd, 27, i * LINEHEIGHT + 1 + y_ofs, FALSE );
			}
			else
			{
				dc.SetBrush( *wxWHITE_BRUSH );
				dc.SetPen( *wxWHITE_PEN );
				dc.DrawRectangle( 27, i * LINEHEIGHT + 1 + y_ofs, 118, LINEHEIGHT - 1 );
			}
			unit_[i]->getSymbolString( testmsg );

			// text:
			if ( unit_[i]->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)unit_[i] )->getOverstack( ) )
				dc.SetTextForeground( *wxLIGHT_GREY );
			// SELECTED LINE TEXT COLOR:
			else if ( unitline_is_selected_[i] == TRUE )
			{
				dc.SetTextForeground( *wxWHITE );
				restore_black = 1;
			}
			else
				dc.SetTextForeground( *wxBLACK );

			dc.SetFont( font );
			dc.DrawText( wxS( testmsg ), 30, i * LINEHEIGHT + 7 + y_ofs );
			if ( restore_black )
			{
				dc.SetTextForeground( *wxBLACK );
				restore_black = 0;
			}

			// movement points:
			int mp = unit_[i]->getMP( );
			if ( unit_[i]->getCounterType( ) == Counter::Type::RP ) // resource point
				sprintf( testmsg, unit_[i]->getShip( ) ? "S" : "-" );
			else if ( mp < 0 && unit_[i]->getCounterType( ) != Counter::Type::SHIP )
				sprintf( testmsg, "-" );
			else if ( unit_[i]->getCounterType( ) == Counter::Type::PLANE && ! ( (AirUnit*)unit_[i] )->isActive( ) )
				sprintf( testmsg, "-" );
			else
			{
				sprintf( testmsg, "%d", mp );
				if ( unit_[i]->isHalfMP( ) )
					strcat( testmsg, "" );
				if ( unit_[i]->getTrain( ) || ( unit_[i]->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)unit_[i] )->getFlag( GroundUnit::RAILONLY ) ) )
					strcat( testmsg, "T" );
				else if ( unit_[i]->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)unit_[i] )->getOverstack( ) )
					strcat( testmsg, "o" );
				else if ( unit_[i]->getShip( ) )
					strcat( testmsg, "S" );
				else if ( unit_[i]->getAdminMove( ) )
					strcat( testmsg, "a" );
				else if ( unit_[i]->getStratMove( ) )
					strcat( testmsg, "s" );
				else if ( unit_[i]->getCounterType( ) == Counter::Type::PLANE && unit_[i]->getInAir( ) )
				{
					if ( ( (AirUnit*)unit_[i] )->getExtRange( ) )
						strcat( testmsg, "e" );
					else if ( ( (AirUnit*)unit_[i] )->getNight( ) )
						strcat( testmsg, "n" );
					else
						strcat( testmsg, "f" );
				}
				else if ( unit_[i]->getCounterType( ) == Counter::Type::PLANE && unit_[i]->getAborted( ) ) strcat( testmsg, "*" );
			}
			dc.SetFont( bfont );
			dc.DrawText( wxS( testmsg ), 146 + strlen( testmsg ) < 3 ? 3 : 0, i * LINEHEIGHT + 7 + y_ofs );

			// orders text:
			if ( unit_[i]->getCounterType( ) == Counter::Type::PTS ) // status marker
			{
				if ( ( (StatusMarker*)unit_[i] )->getPtsType( ) == StatusMarker::ATTACK )
					dc.DrawText( wxT("AT"), 176, i * LINEHEIGHT + 7 );
				else if ( ( (StatusMarker*)unit_[i] )->getPtsType( ) == StatusMarker::DEFENSE )
					dc.DrawText( wxT("DF"), 176, i * LINEHEIGHT + 7 );
				else if ( ( (StatusMarker*)unit_[i] )->getPtsType( ) == StatusMarker::AA )
					dc.DrawText( wxT("AA"), 176, i * LINEHEIGHT + 7 );
				else if ( ( (StatusMarker*)unit_[i] )->getPtsType( ) == StatusMarker::CD )
					dc.DrawText( wxT("CD"), 176, i * LINEHEIGHT + 7 );
				else if ( ( (StatusMarker*)unit_[i] )->getPtsType( ) == StatusMarker::DIE )
					dc.DrawText( wxT("DR"), 176, i * LINEHEIGHT + 7 );
			}
			else if ( unit_[i]->getCounterType( ) == Counter::Type::UNIT && unit_[i]->getMission( ) == GroundUnit::Project::FORT )
				dc.DrawText( wxT("fo"), 176, i * LINEHEIGHT + 7 );
			else if ( unit_[i]->getCounterType( ) == Counter::Type::UNIT && unit_[i]->getMission( ) == GroundUnit::Project::AIRFIELD )
				dc.DrawText( wxT("af"), 176, i * LINEHEIGHT + 7 );
			else if ( unit_[i]->getCounterType( ) == Counter::Type::UNIT && unit_[i]->getMission( ) == GroundUnit::Project::PORT )
				dc.DrawText( wxT("po"), 176, i * LINEHEIGHT + 7 );
			else if ( unit_[i]->getCounterType( ) == Counter::Type::PLANE ) // air unit
			{
				if ( unit_[i]->getMission( ) > AirUnit::Mission::ESCORT )
					dc.DrawText( wxT("Es"), 176, i * LINEHEIGHT + 7 );
				else if ( ! unit_[i]->getInAir( ) && ( (AirUnit*)unit_[i] )->getNavalPatrol( ) ) // naval patrol, but not in air
					dc.DrawText( wxT("NP"), 176, i * LINEHEIGHT + 7 );
				else
					//if (CombatMarkersVisible)
					switch ( unit_[i]->getMission( ) )
					{
						case AirUnit::Mission::BOMB_GS:
							dc.DrawText( wxT("gs"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_AF:
							dc.DrawText( wxT("af"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_RR:
							dc.DrawText( wxT("rr"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_HARASS:
							dc.DrawText( wxT("ha"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_NAVAL:
							dc.DrawText( wxT("na"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_DAS:
							dc.DrawText( wxT("ds"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_PLANES:
							dc.DrawText( wxT("pl"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_PORT:
							dc.DrawText( wxT("po"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_CF:
							dc.DrawText( wxT("cf"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::BOMB_OTHER:
							dc.DrawText( wxT("gb"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::TRANSPORT:
							dc.DrawText( wxT("tr"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::AIRDROP:
							dc.DrawText( wxT("dr"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::INTERCEPT:
							dc.DrawText( wxT("in"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::NAVALPATROL:
							dc.DrawText( wxT("np"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::RAILMARSHAL:
							dc.DrawText( wxT("rm"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::STRAT_PORT:
							dc.DrawText( wxT("sp"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::STRAT_OTHER:
							dc.DrawText( wxT("so"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::ANYBOMB:
							dc.DrawText( wxT("b "), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::CAP:
							dc.DrawText( wxT("ca"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::ESCORT:
							dc.DrawText( wxT("e "), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::TRANSFER:
							dc.DrawText( wxT("tf"), 176, i * LINEHEIGHT + 7 );
							break;
						case AirUnit::Mission::STAGE:
							dc.DrawText( wxT("st"), 176, i * LINEHEIGHT + 7 );
							break;
					}
			}
			else if ( _show_combat_markers )
			{
				switch ( unit_[i]->getAttackDir( ) )
				{
					case Hex::WEST:
						dc.DrawText( wxT("W"), 177, i * LINEHEIGHT + 7 );
						break;
					case Hex::NORTHWEST:
						dc.DrawText( wxT("NW"), 175, i * LINEHEIGHT + 7 );
						break;
					case Hex::SOUTHWEST:
						dc.DrawText( wxT("SW"), 175, i * LINEHEIGHT + 7 );
						break;
					case Hex::EAST:
						dc.DrawText( wxT("E"), 177, i * LINEHEIGHT + 7 );
						break;
					case Hex::NORTHEAST:
						dc.DrawText( wxT("NE"), 175, i * LINEHEIGHT + 7 );
						break;
					case Hex::SOUTHEAST:
						dc.DrawText( wxT("SE"), 175, i * LINEHEIGHT + 7 );
						break;
				}
			}
		}
	}
}

void HexContentsPane::RedrawGrid( wxDC* dc )
{
	dc->SetPen( *hpenDark );
	int y_ofs = 0;
	for ( int line = 0; line <= LINES; ++line )
		dc->DrawLine( 0, line * LINEHEIGHT + y_ofs, LINEWIDTH, line * LINEHEIGHT + y_ofs );
	// vertical lines for MPs and attack status
	dc->DrawLine( 145, 0, 145, LINEHEIGHT * MAXUNITS );  //sz.GetHeight());
	dc->DrawLine( 173, 0, 173, LINEHEIGHT * MAXUNITS );  //sz.GetHeight());
}

void HexContentsPane::paint( wxPaintEvent& WXUNUSED(e) )
{
	wxPaintDC dc( this );
	PrepareDC( dc );

	if ( hbmHexContentsPaneBgnd )
		dc.DrawBitmap( *hbmHexContentsPaneBgnd, 0, 0 );

	int max = num_units_;
	if ( num_units_ > MAXUNITS )
	{
		wxMessageBox( wxT("Too many units, not all are shown"), Application::NAME );
		max = MAXUNITS;
	}
	RedrawTexts( );

	// paint counters
	dc.SetFont( font );
	for ( int i = 0; i < max; ++i )
		if ( unit_[i] != nullptr )
			unit_[i]->drawCounter( &dc, -1, i * LINEHEIGHT + 1, false ); // no border
}

// edit LineSelected unit(s)
void HexContentsPane::editUnit( )
{
	Counter* unit = _selected_unit_list;

	while ( unit )
	{
		if ( unit->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("You can't edit enemy units!"), Application::NAME );
			return;
		}
		unit->edit( );
		Refresh( );
		unit = unit->getNext( );
	}
	map_->paintCurrentHex( );
}

// delete LineSelected unit(s)
void HexContentsPane::deleteUnit( )
{
	if ( getSelCount( ) == 0 )
		return;

	strcpy( msg, "" );
	int i = 0;
	Counter* unit = _selected_unit_list;
	while ( unit )
	{
		if ( unit->getSide( ) != _current_player )
		{
			wxMessageBox( wxT("You can't delete enemy units!"), Application::NAME );
			return;
		}
		unit->getUnitString( tmp );
		strcat( msg, tmp );
		strcat( msg, "\n" );
		unit = unit->getNext( );
		++i;
	}

	strcat( msg, "\nThese units will be permanently removed from the play!" );
	if ( i == 1 )
		sprintf( tmp, "Really delete this fine unit?!" );
	else
		sprintf( tmp, "Really delete these %d units?!", i );

	if ( wxMessageBox( wxS( msg ), wxString::FromAscii( tmp ), wxYES_NO ) != wxYES )
		return;

	Counter* next;
	while ( _selected_unit_list )
	{
		removeUnit( _selected_unit_list );        // remove unit from the window
		next = _selected_unit_list->getNext( );  // get next one in the list
		if ( _selected_unit_list->getCounterType( ) == Counter::Type::HIT )
			wxMessageBox( wxT("NOTE: hits won't disappear even if you remove the hit counter.\n"
								"To actually remove hits use Hex-menu in the setup mode.") );
		delete _selected_unit_list;             // goodbye for this one!
		_selected_unit_list = next;
	}
	_selected_unit_list = NULL;

	Refresh( );
	map_->paintCurrentHex( );
}

// count LineSelected units
int HexContentsPane::getSelCount( )
{
	int i = 0;
	Counter* unit = _selected_unit_list;
	while ( unit )
	{
		unit = unit->getNext( );
		++i;
	}
	return i;
}

// add one unit in the first free slot of the window
void HexContentsPane::addUnit( Counter* unit )
{
	int i = 0;
	while ( unit_[i] != NULL )  // put in the first empty place
	{
		++i;
		if ( i == MAXUNITS )
		{
			wxMessageBox( wxT("Absolute stacking limit reached!"), Application::NAME );
			return;
		}
	}
	unit_[i] = unit;
	num_units_++;
	Refresh( );
}

// Add list of units to the unitwindow
void HexContentsPane::addUnits( Counter* units )
{
	if ( ! units )  // no units to add
		return;

	int line = 0;
	Counter* next = units;
	do
	{
		while ( unit_[line] != NULL )  // find the first empty place
			if ( ++line == MAXUNITS )
			{
				wxMessageBox( wxT("Absolute stacking limit reached\nPlease report to the author"), Application::NAME );
				return;
			}
		unit_[line] = next;
		++num_units_;
		next = next->getNext( );
	}
	while ( next != NULL );
	Refresh( );
}

// remove one unit
void HexContentsPane::removeUnit( Counter* unit )
{
	int i = 0;
	while ( unit_[i] != unit )
	{
		++i;
		if ( unit_[i] == NULL || i == MAXUNITS )
		{
			wxMessageBox( wxT("HexContentsPane::RemoveUnit: Unit not fount"), Application::NAME );
			return;
		}
	}
	while ( i < MAXUNITS - 1 )
	{
		unit_[i] = unit_[i + 1];
		unitline_is_selected_[i] = unitline_is_selected_[i + 1];
		++i;
	}
	unit_[MAXUNITS - 1] = NULL;
	unitline_is_selected_[MAXUNITS - 1] = FALSE;
	--num_units_;
//	::InvalidateRect(HWindow, NULL, FALSE);
}

// empty window
void HexContentsPane::clear( void )
{
	for ( int i = 0; i < MAXUNITS; ++i )
	{
		unit_[i] = NULL;
		unitline_is_selected_[i] = FALSE;
	}
	num_units_ = 0;
	Scroll( 0, 0 );
	Refresh( );
}

void HexContentsPane::clearDeselected( void )
{
	for ( int i = 0; i < MAXUNITS; ++i )
	{
		if ( unit_[i] && unitline_is_selected_[i] == FALSE )
		{
			removeUnit( unit_[i] );
			--i;	// there's new unit at pos 'i' now!
		}
	}
	Refresh( );
}

// add to the beginning of the list
void HexContentsPane::insertToSelectedList( Counter* unit )
{
	map_->removeUnitFromCurrentHex( unit );
	unit->setNext( _selected_unit_list );
	_selected_unit_list = unit;
}

// remove from the list
void HexContentsPane::removeFromSelectedList( Counter* unit )
{
	Counter* curr = _selected_unit_list;

	if ( ! curr ) // no units currently selected
	{
		char s2[100];
		unit->getUnitString( s2 );

		char s[300];
		sprintf( s, "Internal error:\nUnit '%s' is not in the selected list!", s2 );

		wxMessageBox( wxS( s ), Application::NAME );
		return;
	}

	if ( curr == unit ) // at the beginning of the list
	{
		_selected_unit_list = curr->getNext( );
		curr->setNext( NULL );
	}
	else
	{
		Counter* prev = curr;
		curr = curr->getNext( );
		while ( curr )
		{
			Counter* next = curr->getNext( );
			if ( curr == unit )
			{
				prev->setNext( next );
				curr->setNext( NULL );
				break;
			}
			prev = curr;
			curr = next;
		}
	}

	if ( ! curr ) // iterated through selected units and did not find unit
		wxMessageBox( wxT("Internal error:\nNo unit to remove from the LineSelected list!"), Application::NAME );
	else
		map_->insertUnitToCurrentHex( unit );
}

void HexContentsPane::selectUnit( int line, bool skip_graying )
{
	if ( line < 0 || line >= MAXUNITS || unit_[line] == NULL || unitline_is_selected_[line] == TRUE )
		return;

	unitline_is_selected_[line] = TRUE;
	insertToSelectedList( unit_[line] );

	//Refresh(FALSE);
	RedrawTexts( );

	if ( _show_path )
		map_->PaintPaths( );

	if ( ! skip_graying )
		map_->showSelUnitMovementArea( );
}

void HexContentsPane::deselectUnit( int which_line, bool skip_graying, bool skip_path_erasing )
{
	if ( which_line < 0 || which_line >= MAXUNITS || unit_[which_line] == NULL || unitline_is_selected_[which_line] == FALSE )
		return;

	unitline_is_selected_[which_line] = FALSE;
	int selected_count = 0;
	for ( int line = 0; line < MAXUNITS; ++line )
		if ( unitline_is_selected_[line] == TRUE )
			++selected_count;

	removeFromSelectedList( unit_[which_line] );

	//Refresh( false );
	RedrawTexts( );

	if ( ! skip_path_erasing && _show_path )
		map_->PaintPaths( );

	if ( selected_count == 0 )
		map_->clearSelUnitMovementArea( );

	if ( ! skip_graying )
		map_->showSelUnitMovementArea( );
}

void HexContentsPane::selectUnit( Counter* unit, bool skip_graying )
{
	int sel_cnt = 0;
	for ( int line = 0; line < MAXUNITS; ++line )
	{
		if ( unitline_is_selected_[line] == TRUE )
			++sel_cnt;

		if ( unit_[line] == unit && unitline_is_selected_[line] == FALSE )
			selectUnit( line );
	}

	//Refresh(FALSE);
	RedrawTexts( );

	if ( _show_path )
		map_->PaintPaths( );

	if ( ! skip_graying )
		map_->showSelUnitMovementArea( );
}

void HexContentsPane::deselectUnit( Counter* unit, bool skip_graying )
{
	int sel_cnt = 0;
	for ( int line = 0; line < MAXUNITS; ++line )
	{
		if ( unitline_is_selected_[line] == TRUE )
			++sel_cnt;

		if ( unit_[line] == unit && unitline_is_selected_[line] == TRUE )
			deselectUnit( line );
	}

	if ( sel_cnt == 0 )
		map_->clearSelUnitMovementArea( );

	if ( ! skip_graying )
		map_->showSelUnitMovementArea( );
}

void HexContentsPane::selectAll( void )
{
	for ( int line = 0; line < MAXUNITS; ++line )
		if ( unitline_is_selected_[line] == FALSE && unit_[line] && unit_[line]->getSide( ) == _current_player )
			selectUnit( line, true );

	map_->showSelUnitMovementArea( );
}

void HexContentsPane::deselectAll( void )
{
	for ( int line = 0; line < MAXUNITS; ++line )
		if ( unitline_is_selected_[line] == TRUE )
			deselectUnit( line, true, true );

	map_->clearSelUnitMovementArea( );

	if ( _show_path )
		map_->PaintPaths( );
}

void HexContentsPane::markAllSelected( void )
{
	for ( int line = 0; line < MAXUNITS; ++line )
		if ( unit_[line] != NULL )
			unitline_is_selected_[line] = TRUE;
}

void HexContentsPane::markAllUnselected( void )
{
	for ( int line = 0; line < MAXUNITS; ++line )
		if ( unit_[line] != NULL )
			unitline_is_selected_[line] = FALSE;
}

/////////////////////////////////////////////////////////////////////////
// Windows messages
bool select_mode = true;

void HexContentsPane::EvLButtonDown( wxMouseEvent &e )
{
	wxPoint point( e.m_x, e.m_y );

	int rx, ry;
	CalcUnscrolledPosition( point.x, point.y, &rx, &ry );

	int line = ry / LINEHEIGHT;

	if ( line < 0 || line >= LINES - 1 )
		return;

	if ( unit_[line] != NULL )
	{
		if ( unitline_is_selected_[line] == TRUE )
			deselectUnit( line ), select_mode = false;
		else
			selectUnit( line ), select_mode = true;
	}
}

void HexContentsPane::EvMouseMove( wxMouseEvent &e )
{
	if ( e.m_leftDown )
	{
		int rx, ry;
		CalcUnscrolledPosition( e.m_x, e.m_y, &rx, &ry );

		int line = ry / LINEHEIGHT;

		if ( line < 0 || line >= LINES - 1 )
			return;

		if ( select_mode )
		{
			if ( unit_[line] != NULL && unitline_is_selected_[line] == FALSE )
				selectUnit( line );
			return;
		}

		// select_mode == false, i.e. in de-select mode here
		if ( unit_[line] != NULL && unitline_is_selected_[line] == TRUE )
			deselectUnit( line );
	}
}

void HexContentsPane::EvLButtonDblClk( wxMouseEvent &e )
{
	wxPoint point( e.m_x, e.m_y );

	int rx, ry;
	CalcUnscrolledPosition( point.x, point.y, &rx, &ry );

	int line = ry / LINEHEIGHT;

	// TODO: no line check here?
	//if ( line < 0 || line >= LINES - 1 )
	//	return;

	if ( unit_[line] == NULL )
		return;

	// notes are special, always edit
	if ( unit_[line]->getCounterType( ) == Counter::Type::NOTE )
	{
		unit_[line]->edit( );
		return;
	}

	char clickmsg[1000];
	if ( _current_phase == Phase::SETUP_PHASE || _current_phase == Phase::INITIAL_PHASE )
	{
		if ( unit_[line]->getCounterType( ) != Counter::Type::MISC && unit_[line]->getSide( ) != _current_player )
		{
			unit_[line]->getUnitString( clickmsg, true );
			wxMessageBox( wxS( clickmsg ), wxString::FromAscii( "Unit info" ) );
			return;
		}
		unit_[line]->edit( );
		Refresh( );
		map_->paintCurrentHex( );
	}
	else
	{
		unit_[line]->getUnitString( clickmsg, true );
		wxMessageBox( wxS( clickmsg ), wxString::FromAscii( "Unit info" ) );
	}
}

void HexContentsPane::EvRButtonDown( wxMouseEvent &e )
{
	if ( _current_phase == Phase::NO_GAME )
		return;

	int rx, ry;
	CalcUnscrolledPosition( e.m_x, e.m_y, &rx, &ry );

	int line = ry / LINEHEIGHT;

	// TODO: no line check here?
	//if ( line < 0 || line >= LINES - 1 )
	//	return;

	if ( unit_[line] == NULL )
		return;

	Counter* c = unit_[line];
	deselectAll( );
	selectUnit( line );

	// build menu
	wxMenu hmenu;
	hmenu.Append( Frame::MenuEvent::COMMANDSSELECTALL, wxT("Select All") );
	hmenu.Append( Frame::MenuEvent::COMMANDSDESELECTALL, wxT("Unselect All") );
	hmenu.AppendSeparator( );
	hmenu.Append( Frame::MenuEvent::UNITVIEW, wxT("Unit Info...") );
	hmenu.Append( Frame::MenuEvent::HEXEDIT_UNIT, wxT("Edit Unit...") );
	hmenu.Append( Frame::MenuEvent::DESTROYUNIT, wxT("Destroy To Replacement Pool...") );
	hmenu.Append( Frame::MenuEvent::MOVETOREPLPOOL, wxT("Move To Replacement Pool...") );
	hmenu.Append( Frame::MenuEvent::HEXDELETEUNIT, wxT("Remove From Play...") );

	if ( c->getCounterType( ) == Counter::Type::PLANE )
	{
		hmenu.AppendSeparator( );
		if ( ( (AirUnit*)c )->getInAir( ) == FALSE )
			hmenu.Append( Frame::MenuEvent::ACTIVATEAIR, wxT("Toggle Air Group Operative Status") );
		else
			hmenu.Append( Frame::MenuEvent::LAND, wxT("Land") );
	}

	bool sep_rq = true;

	if ( c->getCounterType( ) == Counter::Type::UNIT )
	{
		// attack supply
		if ( _rule_set.AttackSup == TRUE && _current_phase >= Phase::NAVAL_PHASE_1 && _current_phase <= Phase::COMBAT_PHASE && c->getSide( ) == _phasing_player )
		{
			hmenu.AppendSeparator( );
			hmenu.Append( Frame::MenuEvent::ATTACKSUP, wxT("Toggle Attack Supply") );
		}

		// overstack
		hmenu.AppendSeparator( );
		hmenu.Append( Frame::MenuEvent::TOOVERSTACK, wxT("Enter Overstack") );
		hmenu.Append( Frame::MenuEvent::FROVERSTACK, wxT("Leave Overstack") );

		if ( _current_phase == Phase::MOVEMENT_PHASE )
		{
			// to/from train
			hmenu.AppendSeparator( );
			hmenu.Append( Frame::MenuEvent::UNITTOTRAIN, wxT("&Board Train") );
			hmenu.Append( Frame::MenuEvent::UNITFROMTRAIN, wxT("&Leave Train") );
			// break stuff
			hmenu.AppendSeparator( );
			hmenu.Append( Frame::MenuEvent::HEXHITRAIL, wxT("&Break Rail") );
			hmenu.Append( Frame::MenuEvent::HEXHITAF, wxT("&Damage Airfield") );
		}

		// construction units
		if ( ( (GroundUnit*)c )->isConstructionEngineer( ) )
		{
			if ( _current_phase == Phase::MOVEMENT_PHASE || _current_phase == Phase::SETUP_PHASE )
			{
				hmenu.AppendSeparator( );
				hmenu.Append( Frame::MenuEvent::HEXCREATETEMPAF, wxT("Build Temporary Airfield...") );
				sep_rq = false;
			}
			if ( _current_phase == Phase::INITIAL_PHASE || _current_phase == Phase::SETUP_PHASE )
			{
				if ( sep_rq )
					hmenu.AppendSeparator( );
				hmenu.Append( Frame::MenuEvent::HEXCREATEFORT, wxT("Build Fort...") );
				hmenu.Append( Frame::MenuEvent::HEXCREATEPERMAF, wxT("Build/Augment Airfield...") );
				hmenu.Append( Frame::MenuEvent::REPAIRPORT, wxT("Rebuild Port") );
			}
		}

		// cancel order
		if ( ( (GroundUnit*)c )->getMission( ) || ( (GroundUnit*)c )->getAttackDir( ) )
		{
			hmenu.AppendSeparator( );
			hmenu.Append( Frame::MenuEvent::CANCELORDER, wxT("Cancel Orders") );
		}
	}
	else if ( c->getCounterType( ) == Counter::Type::SHIP )
	{
		if ( ( (NavalUnit*)c )->getMission( ) || ( (NavalUnit*)c )->getAttackDir( ) )
		{
			hmenu.AppendSeparator( );
			hmenu.Append( Frame::MenuEvent::CANCELORDER, wxT("Cancel Orders") );
		}
	}

	// show menu
	PopupMenu( &hmenu, e.m_x, e.m_y );
}

#endif
