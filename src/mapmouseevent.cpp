#include <wx/wx.h>

#include "hextools.h"

#if defined HEXTOOLSMAP
#include "terrainselector.h"
#endif

#if defined HEXTOOLSPLAY
#include "application.h"
#include "sideplayer.h"
#include "dirs.loop"
#include "selectedunits.loop"
#endif

#include "hexsidetype.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "hexinfopane.h"
#include "mappane.h"
#include "frame.h"

#if defined HEXTOOLSPLAY
using ht::dir2replay;
using ht::coordinatesOutOfBounds;
#include "rules.h"
#include "counter.h"
#include "hexcontents.h"
#include "hexcontentspane.h"
#include "phase.h"
#include "movementmode.h"
#include "airunit.h"

#include "view.h"
#include "views.h"
#include "dview.h"

DECLARE_APP(Application);
#endif

extern HexType _hex_types[]; //[TERRAINTYPECOUNT];		// terrain types

#if defined HEXTOOLSPLAY

extern ViewDlgXfer _view_dialog_data;
extern Counter* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _teleport_mode_on;
extern Rules _rule_set;
extern int _current_phase;
extern int _phasing_player;
extern int _current_player;
extern int _gray_out_hexes;
extern const char* _weather_zone_string[];
extern const wxString _weather_condition_string[];	// 5// from mappane.cpp
extern int _weather_condition[];	// W_ZONES	// W_MUD etc., per zone (frame.cpp)
extern int _rivers_lakes_frozen[];	// W_ZONES	// true if rivers/lakes are frozen (frame.cpp)
extern int _show_combat_markers;
extern int _use_point_and_click_move;
extern int _quick_scroll_enabled;
extern int _quick_scroll_mirrored;
extern int _quick_scroll_velocity;

void MapPane::map_district_to_country( int district_id, int country_id )
{
	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			if ( hex_[y][x].getLandDistrictID( ) == district_id )
				sit_[y][x].setCountryID( country_id );
}

void MapPane::map_seabox_to_seazone( int seabox_id, int seazone_id )
{
	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			if ( hex_[y][x].getSeaDistrictID( ) == seabox_id )
				sit_[y][x].setSeazoneID( seazone_id );
}
#endif

// paint terrain in HT-m; scroll map, point'n'click movement in HT-p
void MapPane::EvLButtonDown( wxMouseEvent& e )
{
	wxPoint point( e.m_x, e.m_y );
	point.x = static_cast<int>( point.x * scale_ );
	point.y = static_cast<int>( point.y * scale_ );

//#if defined HEXTOOLSMAP
//#el
#if defined HEXTOOLSPLAY
	left_button_down_ = TRUE;
	if ( _current_phase == Phase::NO_GAME )
		return;
#endif
	int x, y, x2, y2;
	x = x2 = point.x;
	y = y2 = point.y;

	screen2Hex( &x, &y );

	if ( ht::coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return;

#if defined HEXTOOLSMAP
	selected_hex_point_.x = x;
	selected_hex_point_.y = y;

 	bool has_water = (	hex_[y][x].terrain_ == HexType::SEA ||
						hex_[y][x].terrain_ == HexType::ICINGSEA ||
						hex_[y][x].terrain_ == HexType::ATOLL ||
						hex_[y][x].terrain_ == HexType::SMALLISLAND );

	if ( ! has_water && hex_[y][x].hasAnyWater( ) ) // SE, SW, and W sides
		has_water = true;
	if ( ! has_water )
	{
		int xnw, ynw;
		int hasNWAdj = ht::getAdjHexCoords( Hex::NORTHWEST, x, y, &xnw, &ynw, world_x_size_, world_y_size_ );
		if ( hasNWAdj &&
				(	hex_[ynw][xnw].hasWaterSide( Hex::SOUTHEAST )		// NW side
					|| hex_[ynw][xnw].hasWaterSide( Hex::SOUTHWEST ) ) ) // NW vertex
			has_water = true;
	}
	if ( ! has_water )
	{
		int xne, yne;
		int hasNEAdj = ht::getAdjHexCoords( Hex::NORTHEAST, x, y, &xne, &yne, world_x_size_, world_y_size_ );
		if ( hasNEAdj &&
				(	hex_[yne][xne].hasWaterSide( Hex::SOUTHWEST )		// NE side
					|| hex_[yne][xne].hasWaterSide( Hex::WEST )			// N vertex
					|| hex_[yne][xne].hasWaterSide( Hex::SOUTHEAST ) ) )	// NE vertex
			has_water = true;
	}
	if ( ! has_water )
	{
		int xe, ye;
		int hasEAdj = ht::getAdjHexCoords( Hex::EAST, x, y, &xe, &ye, world_x_size_, world_y_size_ );
		if ( hasEAdj &&
				(	hex_[ye][xe].hasWaterSide( Hex::WEST )				// E side
					|| hex_[ye][xe].hasWaterSide( Hex::SOUTHWEST ) ) )	// SE vertex
			has_water = true;
	}
	if ( ! has_water )
	{
		int xse, yse;
		int hasSEAdj = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );
		if ( hasSEAdj && hex_[yse][xse].hasWaterSide( Hex::WEST ) )		// S vertex
			has_water = true;
	}
	if ( ! has_water )
	{
		int xw, yw;
		int hasWAdj = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );
		if ( hasWAdj && hex_[yw][xw].hasWaterSide( Hex::SOUTHEAST ) )	// SW vertex
			has_water = true;
	}

	if ( weather_zone_mode_ && selected_weather_zone_ > -1 )
		hex_[y][x].setWeatherZone( selected_weather_zone_ );
	else if ( mapleaf_mode_ && current_mapid_[0] != '\0' )
		hex_[y][x].setMapIdStr( current_mapid_ );
	else if ( sea_district_mode_ && selected_sea_district_ > 255 )
	{
		if ( has_water && hex_[y][x].terrain_ != HexType::NONE )
			hex_[y][x].setSeaDistrictID( selected_sea_district_ );
	}
	else if ( land_district_mode_ && selected_land_district_ > 255 )
	{
		if ( hex_[y][x].terrain_ != HexType::NONE )
			hex_[y][x].setLandDistrictID( selected_land_district_ );
	}
	else
	{
		// set hex terrain
		if ( parent_->terrain_selector_->getSelectedType( ) < HexType::TERRAINTYPECOUNT )
		{
			if ( parent_->terrain_selector_->getSelectedType( ) != hex_[y][x].terrain_ )
			{
				if ( parent_->terrain_selector_->getSelectedType( ) == HexType::HEXTYPEKILL )
				{
					if ( hex_[y][x].city_ != nullptr )
					{
						delete hex_[y][x].city_;
						hex_[y][x].city_ = nullptr;
					}
					hex_[y][x].clearHexsideData( );
					hex_[y][x].terrain_ = HexType::NONE;
					hex_[y][x].setMapIdWord( 0xffff );
					hex_[y][x].setHexNumber( ht::DEFAULT_HEX_ID );
				}
				else
				{
					hex_[y][x].terrain_ = parent_->terrain_selector_->getSelectedType( );
				}

				// TODO: consider making this part of the "updateFlags" pattern?
				//	i.e. a updateSeaIceFlag(..) method
				if ( hex_[y][x].getLateSeaIce( )
					&& ! hex_ices( hex_[y][x].terrain_ )
					&& ! hexsides_ice( hex_[y][x] ) )
				{
					hex_[y][x].clrLateSeaIce( );
				}
			}
		}
		else	// set hexside feature
		{
			// x2,y2 are the original screen coords (before translation to hex coords)
			int side = get_hex_part( x2, y2 );
			set_hex_side( x, y, side );

			// if any interior hexside, don't set feature in adjacent hex
			if ( parent_->terrain_selector_->getSelectedType( ) == HexsideType::MTN_SIDE2
				|| parent_->terrain_selector_->getSelectedType( ) == HexsideType::IMP_ESCARPMENT
				|| parent_->terrain_selector_->getSelectedType( ) == HexsideType::ESCARPMENT
				|| parent_->terrain_selector_->getSelectedType( ) == HexsideType::FORTIFIED
				|| parent_->terrain_selector_->getSelectedType( ) == HexsideType::GREAT_WALL )
			{
				;
			}
			else if ( ht::getAdjHexCoords( side, x, y, &x2, &y2, world_x_size_, world_y_size_ ) ) // otherwise, set same feature in adjacent hex, too
			{
				// now x2,y2 are the hex coords of the side-adjacent hex
				set_hex_side( x2, y2, ht::getAdjHexPart( side ) );
			}
		}
	}

	hex2Screen( &x, &y );

	x = static_cast<int>( x / scale_ );
	y = static_cast<int>( y / scale_ );

	wxRect rect;
	rect.x = x - static_cast<int>( ( Hex::SIZE_X << 1 ) / scale_ );
	rect.y = y - static_cast<int>( ( Hex::SIZE_Y << 1 ) / scale_ );
	rect.width = static_cast<int>( ( Hex::SIZE_X << 2 ) / scale_ );
	rect.height = static_cast<int>( ( Hex::SIZE_Y << 2 ) / scale_ );

	Refresh( false, &rect );
	file_is_dirty_ = true;
#elif defined HEXTOOLSPLAY
	if ( GetActiveViewCount( ) )
	{
		int nview = -1;

		if ( view_text_dialog_ && view_text_dialog_->IsShown( ) )
			nview = _view_dialog_data.CurrentView;
		else
			nview = FirstActiveView( );
		if ( nview >= 0 )
		{
			if ( e.m_controlDown )
			{
				if ( ! DeletePointFromView( nview, x, y ) )
					AddPointToView( nview, x, y );
				Refresh( false );
				return;
			}
			else if ( view_text_dialog_ && view_text_dialog_->IsShown( ) )
			{
				for ( nview = 0; nview < GetViewCount( ); ++nview )
				{
					if ( ViewIsActive( nview ) && HexInView( nview, x, y ) )
					{
						if ( _view_dialog_data.CurrentView != nview )
						{
							_view_dialog_data.CurrentView = nview;
							strncpy( _view_dialog_data.editCaption, views_->getViewName( nview ), HexNote::MAX_CAPTION_LEN - 1 );
							strncpy( _view_dialog_data.editText, views_->getViewText( nview ), HexNote::MAX_TEXT_LEN - 1 );
							strcpy( _view_dialog_data.editRed, ht::inttoa( views_->getRed( nview ) ) );
							strcpy( _view_dialog_data.editGreen, ht::inttoa( views_->getGreen( nview ) ) );
							strcpy( _view_dialog_data.editBlue, ht::inttoa( views_->getBlue( nview ) ) );
							view_text_dialog_->InitControls( );
							if ( view_text_dialog_->IsShown( ) )
								view_text_dialog_->Show( );
						}
						break;
					}
				}
			}
		}
	} // end if ( GetActiveViewCount( ) )

	qscroll_mouse_x0_ = point.x;
	qscroll_mouse_y0_ = point.y;

	int sx, sy;
	CalcUnscrolledPosition( 0, 0, &sx, &sy );

	// scroller setup:
	// MUST NOT BE CHANGED FROM 32 OR ZOOMED MODES WON'T WORK PROPERLY!
#define SCROLLSTEPS_PER_HEX 32 // HexTools-play says 32 for zoomability, was 8

	qscroll_scroller_x0_ = sx / ( 32 / SCROLLSTEPS_PER_HEX );
	qscroll_scroller_y0_ = sy / ( 32 / SCROLLSTEPS_PER_HEX );

	if ( _quick_scroll_enabled )
		StatusMsg( "Keep left button down and move the mouse to scroll the map" );

	if ( country_mode_ && selected_country_ >= 8 )
	{
		if ( hex_[y][x].terrain_ != HexType::NONE && hex_[y][x].getLandDistrictID( ) != 256 ) // != NONE district
		{
			int search_district_id = hex_[y][x].getLandDistrictID( );
			map_district_to_country( search_district_id, selected_country_ );
			Refresh( );
		}
		return;
	}

	if ( sea_zone_mode_ && selected_sea_zone_ >= 32 )
	{
		if ( hex_[y][x].terrain_ != HexType::NONE && hex_[y][x].getSeaDistrictID( ) != 256 ) // != NONE sea circle
		{
			int search_seabox_id = hex_[y][x].getSeaDistrictID( );
			map_seabox_to_seazone( search_seabox_id, selected_sea_zone_ );
			Refresh( );
		}
		return;
	}

	int old_x = selected_hex_point_.x;
	int old_y = selected_hex_point_.y;

	// updateHexCache(..) checks coordinate bounds
	//if ( ! coordinatesOutOfBounds( old_x, old_y, world_x_size_, world_y_size_ ) )
	updateHexCache( old_x, old_y );

	// move units to clicked hex if needed:
	int teleported = FALSE;
	if ( _teleport_mode_on == FALSE && _use_point_and_click_move && _selected_unit_list && ! sit_[y][x].isGrayed( )
		&& ( _current_phase == Phase::MOVEMENT_PHASE || _current_phase == Phase::REACT_MOVE_PHASE
#if 0
			|| ( Phase == COMBAT_PHASE && SelectedUnits->Type() == CNTR_PLANE )
#endif
			|| _current_phase == Phase::EXPLOITATION_PHASE )
		&& ( _rule_set.NonPhasMove == TRUE || _current_player == _phasing_player || _selected_unit_list->getCounterType( ) == Counter::Type::PLANE )
		&& ( x != old_x || y != old_y ) )
	{
		click_target_x_ = x;
		click_target_y_ = y;
		_teleport_mode_on = FALSE;
		teleported = 123;  // = point'n'click movement
		click_marching_mode_ = TRUE;
		contents_pane_->clearDeselected( );
		// TODO: DTM
		_teleport_mode_on = FALSE;
#if 0
		wxGetApp( ).CancelTeleport( );
#endif
		wxGetApp( ).frame_->getToolbar( )->ToggleTool( Frame::MenuEvent::TELEPORT, 0 );

		char* path = movement_path( x, y );

		bool ok_to_move = false;
		Counter* c = nullptr;
		ENUM_SELUNITS(c)
		{
			// allow non-phasing planes
			if ( c->getSide( ) != _phasing_player && ( c->getCounterType( ) != Counter::Type::PLANE ) )
			{
				teleported = FALSE;				// cancel
				contents_pane_->deselectAll( );	// new hex selected,
				contents_pane_->clear( );		// get rid of old selections
				break;
			}
			ok_to_move = true;
		}

		if ( ! path || ! *path )
			ok_to_move = false;

		if ( ok_to_move ) // yes they were, now move
		{
			// note: path is in reverse order
			char* cp = path;
			while ( *cp )
				++cp;
			--cp;

			int ox = selected_hex_point_.x;
			int oy = selected_hex_point_.y;

			erase_movement_path( );
			click_marching_mode_ = TRUE;

			// travel backwards
			while ( cp >= path && click_marching_mode_ == TRUE )
				March( ht::replay2dir( --(*cp) ) );  // NOTE: March( ) alters current hex!

			showSelUnitMovementArea( );
			click_marching_mode_ = FALSE;
			updateHexCache( ox, oy );  // TODO:  does not work, original hex remains
			PaintHex( ox, oy );  // <==  unpainted because DontDrawMap == 1
			return;  //   Hard to fix because no direct paint call
		}
		else
		{
			teleported = FALSE;
			contents_pane_->deselectAll( );	// new hex selected,
			contents_pane_->clear( );		// get rid of old selections
		}
	}
	else if ( _teleport_mode_on == FALSE ) // just click, select new hex (or deselect old if same clicked again):
	{
		contents_pane_->deselectAll( );	// new hex selected,
		contents_pane_->clear( );		// get rid of old selections
	}
	else  // magical teleport: let selected units move to the new hex
	{
		contents_pane_->clearDeselected( );
		_teleport_mode_on = FALSE;
		teleported = TRUE;
		// TODO: DTM
		//wxGetApp().CancelTeleport();
		wxGetApp( ).frame_->getToolbar( )->ToggleTool( Frame::MenuEvent::TELEPORT, false );
	}

	UnHiLiteHex( );
	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )  // hex de-selected
	{
		if ( teleported == TRUE )  // teleport back to old hex
		{
			contents_pane_->deselectAll( );
			contents_pane_->clear( );
		}
		selected_hex_point_.y = selected_hex_point_.x = -1;
		return;
	}
	selected_hex_point_.x = x;
	selected_hex_point_.y = y;

	// units teleported away from the old hex, repaint it
	if ( _selected_unit_list && old_x >= 0 && teleported )
	{
		PaintHex( old_x, old_y );
		sit_[y][x].setOwner( _selected_unit_list->getSide( ) );
	}

	// add units to unit window
	contents_pane_->addUnits( sit_[y][x].unit_list_ );
	PaintHex( x, y );
	HiLiteHex( x, y );
	if ( teleported == TRUE && ::_gray_out_hexes )  // needs full repaint!
		Refresh( false );
#endif
}

void MapPane::EvLButtonDblClk( wxMouseEvent& e )
{
	wxPoint point( e.m_x, e.m_y );
	point.x = (int)( point.x * scale_ );
	point.y = (int)( point.y * scale_ );
#if defined HEXTOOLSPLAY
	if ( _current_phase == Phase::NO_GAME )
		return;
#endif

	int x = point.x;
	int y = point.y;
	screen2Hex( &x, &y );

	if ( ht::coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return;
	//if ( x < 0 || y < 0 || x >= world_x_size_ || y >= world_y_size_ )

	// left button doubleclick not used for anything in HT-m

#if defined HEXTOOLSPLAY
	// if hex has changed
	if ( selected_hex_point_.x != x || selected_hex_point_.y != y )
	{
		UnHiLiteHex( );
		contents_pane_->deselectAll( );
		contents_pane_->clear( );
		contents_pane_->addUnits( sit_[y][x].unit_list_ );
	}
	selected_hex_point_.x = x;
	selected_hex_point_.y = y;
	HiLiteHex( x, y );
	CmViewHex( );
#endif
}

void MapPane::EvRButtonDown( wxMouseEvent& e )
{
	wxPoint point( e.m_x, e.m_y );
	point.x = static_cast<int>( point.x * scale_ );
	point.y = static_cast<int>( point.y * scale_ );

#if defined HEXTOOLSPLAY
	if ( _current_phase == Phase::NO_GAME )
		return;
#endif
	// calculate and store location of the selected hex
	int x = point.x;
	int y = point.y;
	screen2Hex( &x, &y );

	// TODO: use ht::coordinatesOutOfBounds( 0, 0, world_x_size_, world_y_size_ )
	if ( x < 0 || y < 0 || x >= world_x_size_ || y >= world_y_size_ )
		return;

#if defined HEXTOOLSPLAY
	// if hex has changed, update UnitWin
	if ( selected_hex_point_.x != x || selected_hex_point_.y != y )
	{
		UnHiLiteHex( );
		contents_pane_->deselectAll( );
		contents_pane_->clear( );
		contents_pane_->addUnits( sit_[y][x].unit_list_ );
	}
#endif
	selected_hex_point_.x = x;
	selected_hex_point_.y = y;
#if defined HEXTOOLSPLAY
	HiLiteHex( x, y );
#endif
	wxPoint pt;
	pt.x = point.x;
	pt.y = point.y;

#if defined HEXTOOLSMAP
	wxMenu menu;

	menu.Append( MapPane::RMBMenuEvent::HEX_OPTIONSADDCITY, "Add City..." );
	if ( hex_[y][x].city_ != nullptr )
	{
		menu.Append( MapPane::RMBMenuEvent::EDITCITY, "Edit City..." );
		menu.Append( MapPane::RMBMenuEvent::HEX_OPTIONSREMOVE_CITY, "Remove City" );
	}
	menu.Append( MapPane::RMBMenuEvent::TOGGLE_RESTR_WATERS, "Toggle Restricted Waters" );

	if ( hex_ices( hex_[y][x].terrain_ ) || hexsides_ice( hex_[y][x] ) )
		menu.Append( MapPane::RMBMenuEvent::TOGGLE_LATE_SEAICE, "Toggle Late Sea Ice" );

	menu.Append( MapPane::RMBMenuEvent::LIMSTACK_INDICATOR, "Toggle Limited Stacking" );
	menu.AppendSeparator( );
	menu.Append( MapPane::RMBMenuEvent::COMMANDSEXTEND_MAP, "Extend map..." );
	menu.Append( MapPane::RMBMenuEvent::SHRINKMAP, "Shrink map..." );
	menu.Append( MapPane::RMBMenuEvent::INSERTMAPHERE, "Insert map here..." );
	menu.Append( MapPane::RMBMenuEvent::HEX_OPTIONSINFO, "Map info..." );
	menu.AppendSeparator( );
	menu.Append( MapPane::RMBMenuEvent::MAPLEAFMODE, ( mapleaf_mode_ ? "[CURRENT] Map sheet mode" : "Map sheet mode" ) );
	menu.Append( MapPane::RMBMenuEvent::HEX_OPTIONSSHOW_WZ, ( weather_zone_mode_ ? "[CURRENT] Weather zone mode" : "Weather zone mode" ) );
	menu.Append( MapPane::RMBMenuEvent::SEADISTRICTMODE, ( sea_district_mode_ ? "[CURRENT] Sea circle mode" : "Sea circle mode" ) );
	if ( sea_district_mode_ )
		menu.Append( MapPane::RMBMenuEvent::CLEARSEACIRCLES, "---> Clear ALL sea circles" );
	menu.Append( MapPane::RMBMenuEvent::LANDDISTRICTMODE, ( land_district_mode_ ? "[CURRENT] Land district mode" : "Land district mode" ) );
	if ( land_district_mode_ )
		menu.Append( MapPane::RMBMenuEvent::CLEARLANDDISTRICTS, "---> Clear ALL land districts" );
	if ( weather_zone_mode_ || mapleaf_mode_ || sea_district_mode_ || land_district_mode_ )
		menu.Append( MapPane::RMBMenuEvent::HEX_OPTIONSHIDE_WZ, "End current mode" );
	menu.AppendSeparator( );
	menu.Append( MapPane::RMBMenuEvent::SHOWHEXID, "Show hex IDs" );
	menu.Append( MapPane::RMBMenuEvent::HIDEHEXID, "Hide hex IDs" );
	menu.Append( MapPane::RMBMenuEvent::CALC_HEXID, "Recalc hex IDs" );
	menu.Append( MapPane::RMBMenuEvent::MAPSHEETORIENTATION, "Change Mapsheet Orientation..." );
	menu.Append( MapPane::RMBMenuEvent::LISTMAPSHEETS, "List Mapsheets..." );
	menu.AppendSeparator( );

	// ports in submenu (crashes application when menu is shown!!!):
	wxMenu portmenu;
	portmenu.Append( MapPane::RMBMenuEvent::NOPORT, "No port" );
	portmenu.Append( MapPane::RMBMenuEvent::ANCHORAGE, "Anchorage" );
	portmenu.Append( MapPane::RMBMenuEvent::MARGINALPORT, "Marginal port" );
	portmenu.Append( MapPane::RMBMenuEvent::MINORPORT, "Minor port" );
	portmenu.Append( MapPane::RMBMenuEvent::STANDARDPORT, "Standard port" );
	portmenu.Append( MapPane::RMBMenuEvent::MAJORPORT, "Major port" );
	portmenu.Append( MapPane::RMBMenuEvent::GREATPORT, "Great port" );
	portmenu.Append( MapPane::RMBMenuEvent::STRONGPORT, "Strong" );
	portmenu.Append( MapPane::RMBMenuEvent::ARTIFICIALPORT, "Artificial" );
	int portMenuID = 1234;	// TODO: vet this magic number
	menu.Append( portMenuID, "Port", &portmenu );
	menu.AppendSeparator( );

	wxMenu dirmenu;
	dirmenu.Append( MapPane::RMBMenuEvent::PORT_CENTER, "Center" );
	dirmenu.Append( MapPane::RMBMenuEvent::PORT_NW, "North-West" );
	dirmenu.Append( MapPane::RMBMenuEvent::PORT_NE, "North-East" );
	dirmenu.Append( MapPane::RMBMenuEvent::PORT_E, "East" );
	dirmenu.Append( MapPane::RMBMenuEvent::PORT_W, "West" );
	dirmenu.Append( MapPane::RMBMenuEvent::PORT_SW, "South-West" );
	dirmenu.Append( MapPane::RMBMenuEvent::PORT_SE, "South-East" );
	int portSymbolLocMenuID = 1235;	// TODO: vet this magic number
	portmenu.Append( portSymbolLocMenuID, "Symbol location", &dirmenu );

	// Intrinsic AFs in submenu (crashes application when menu is shown!!!):
	wxMenu intrAFmenu;
	intrAFmenu.Append( MapPane::RMBMenuEvent::NO_INTRAF, "No Intrinsic AF" );
	intrAFmenu.Append( MapPane::RMBMenuEvent::INTRAF1, "Intrinsic AF 1-cap" );
	intrAFmenu.Append( MapPane::RMBMenuEvent::INTRAF3, "Intrinsic AF 3-cap" );
	int intrAFMenuID = 3456;	// TODO: vet this magic number
	menu.Append( intrAFMenuID, "Intrinsic AF", &intrAFmenu );

	wxMenu dir2menu;
	dir2menu.Append( MapPane::RMBMenuEvent::AF_CENTER, "Center" );
	dir2menu.Append( MapPane::RMBMenuEvent::AF_NW, "North-West" );
	dir2menu.Append( MapPane::RMBMenuEvent::AF_NE, "North-East" );
	dir2menu.Append( MapPane::RMBMenuEvent::AF_E, "East" );
	dir2menu.Append( MapPane::RMBMenuEvent::AF_W, "West" );
	dir2menu.Append( MapPane::RMBMenuEvent::AF_SW, "South-West" );
	dir2menu.Append( MapPane::RMBMenuEvent::AF_SE, "South-East" );
	int afSymbolLocMenuID = 3457;	// TODO: vet this magic number
	intrAFmenu.Append( afSymbolLocMenuID, "Symbol location", &dir2menu );

	wxMenu damMenu;
	damMenu.Append( MapPane::RMBMenuEvent::NO_DAM, "No Dam" );
	int damMenuID = 4567;	// TODO: vet this magic number
	if ( hex_[y][x].hasInlandWaters( ) || hex_[y][x].hasDams( ) )
	{
		menu.AppendSeparator( );
		menu.Append( damMenuID, "Dam", &damMenu );

		wxMenu vertexMenu;
		bool damnable = false;
		if ( hex_[y][x].hasInlandWater( Hex::WEST ) || hex_[y][x].hasInlandWater( Hex::SOUTHWEST ) )
		{
			vertexMenu.Append( MapPane::RMBMenuEvent::DAM_SOUTHWEST, "Southwest" );
			damnable = true;
		}
		if ( hex_[y][x].hasInlandWater( Hex::SOUTHWEST ) || hex_[y][x].hasInlandWater( Hex::SOUTHEAST ) )
		{
			vertexMenu.Append( MapPane::RMBMenuEvent::DAM_SOUTH, "South" );
			damnable = true;
		}
		int damVertexMenuID = 4568;
		if ( damnable )
			damMenu.Append( damVertexMenuID, "Dam location", &vertexMenu );
	}

	PopupMenu( &menu, static_cast<int>( pt.x / scale_ ), static_cast<int>( pt.y / scale_ ) );

	// detach submenu or crash may happen (c++ objects are deleted while
	// still attached together):
	portmenu.Remove( portSymbolLocMenuID );
	intrAFmenu.Remove( afSymbolLocMenuID );
	menu.Remove( portMenuID );
	menu.Remove( intrAFMenuID );
	file_is_dirty_ = true;
#elif defined HEXTOOLSPLAY
	wxMenu hmenu;

	hmenu.Append( Frame::MenuEvent::HEXINFO, wxT("&Hex Info...") );
	hmenu.Append( Frame::MenuEvent::STACKINFO, wxT("&Stack Info...") );
	hmenu.Append( Frame::MenuEvent::ADDNOTE, wxT("Add &Note...") );

	int sep_rq = 1;
	if ( sit_[y][x].isTarget( ) )
	{
		if ( _show_combat_markers )
		{
			if ( sep_rq )
				hmenu.AppendSeparator( );
			hmenu.Append( Frame::MenuEvent::HEXCOMBATINFO, wxT("&Combat Info...") );
			if ( sit_[y][x].getOwner( ) != _current_player )  // can cancel only own attacks
				hmenu.Append( Frame::MenuEvent::CANCELATTACK, wxT("C&ancel Combat") );
		}
		if ( _current_phase == Phase::COMBAT_PHASE || _current_phase == Phase::REACT_COMB_PHASE )
			hmenu.Append( Frame::MenuEvent::COMBAT, wxT("&Resolve Combat...") );
		sep_rq = 1;
	}
	if ( sit_[y][x].isAirTarget( ) )
	{
		if ( sep_rq )
			hmenu.AppendSeparator( );
		sep_rq = 0;
		// can cancel only own air attacks
		if ( _rule_set.OnDemandAirMissions == FALSE && sit_[y][x].getOwner( ) != _current_player )
		{
			hmenu.Append( Frame::MenuEvent::CANCELAIR, wxT("Cancel Air A&ttack") );
			sep_rq = 1;
		}
		if ( _current_phase == Phase::REACTION_PHASE )  // TODO: only if there are interceptors
		{
			if ( air_combat_here( x, y ) )
			{
				hmenu.Append( Frame::MenuEvent::RESOLVEAIRCOMBAT, wxT("R&esolve Air Combat...") );
				sep_rq = 1;
			}
		}
		if ( _current_phase == Phase::COMBAT_PHASE )
		{
			if ( CalcAA( x, y ) )
			{
				hmenu.Append( Frame::MenuEvent::FIREAA, wxT("&Fire AA...") );
				if ( sit_[y][x].hasFiredAA( ) )
					hmenu.Enable( Frame::MenuEvent::FIREAA, false );
#if 0
				if ( ! Sit[y][x].hasFiredAA( ) )
					hmenu.Append( Frame::MenuEvent::FIREAA, wxT("&Fire AA...") );
				else
				{
					hmenu.Append( Frame::MenuEvent::FIREAA, wxT("&Fire AA...") );
					hmenu.Enable( Frame::MenuEvent::FIREAA, false );
				}
#endif
			}
			hmenu.Append( Frame::MenuEvent::RESOLVEBOMBING, wxT("Resolve Air Attac&k...") );
			sep_rq = 1;
		}
	}
	if ( _current_phase == Phase::SETUP_PHASE )
	{
		if ( sep_rq )
			hmenu.AppendSeparator( );
		if ( !IsEnemyOccupied( x, y ) )
			hmenu.Append( Frame::MenuEvent::COMMANDSCHANGEOWNER, wxT("Change o&wner") );
		if ( IsRRInHex( x, y ) )
			hmenu.Append( Frame::MenuEvent::HEXREGAUGE, wxT("Change rail &gauge") );
		if ( _rule_set.RailsObstructed == TRUE && IsRRInHex( x, y ) && sit_[y][x].isRailObstructed( ) )
			hmenu.Append( Frame::MenuEvent::CLEAROBSTRUCTED, wxT("Clear rail obstruction") );
		if ( GetCurrentAFCapacity( x, y ) < GetRawAFCapacity( x, y ) )
			hmenu.Append( Frame::MenuEvent::RESTOREAFCAP, wxT("Rest&ore AF capacity") );
	}
	if ( sit_[y][x].getOwner( ) == _current_player )
	{
		if ( _current_phase == Phase::INITIAL_PHASE )
		{
			if ( sep_rq )
				hmenu.AppendSeparator( );
			sep_rq = 0;
			hmenu.Append( Frame::MenuEvent::HEXCREATEUNIT, wxT("A&dd Unit...") );
			hmenu.Append( Frame::MenuEvent::HEXCREATEAIRUNIT, wxT("Add Air Group...") );
		}
		if ( sit_[y][x].unit_list_ )
		{
			if ( sep_rq )
				hmenu.AppendSeparator( );
			sep_rq = 0;
			hmenu.Append( Frame::MenuEvent::COMMANDSSELECTALL, wxT("Select all") );
		}
		if ( _selected_unit_list )
		{
			if ( sep_rq )
			{
				hmenu.AppendSeparator( );
#if 0
				sep_rq = 0;
#endif
			}
			hmenu.Append( Frame::MenuEvent::COMMANDSDESELECTALL, wxT("Deselect all") );
		}
	}
	PopupMenu( &hmenu, (int)( pt.x / scale_ ), (int)( pt.y / scale_ ) );
#endif
}

void MapPane::EvMouseMove( wxMouseEvent& e )
{
	HexInfoPane* hexinfopane;

#if defined HEXTOOLSMAP
	hexinfopane = parent_->info_pane_;
#elif defined HEXTOOLSPLAY
	if ( _current_phase == Phase::NO_GAME )
		return;
	hexinfopane = info_pane_;
#endif

	wxPoint point( e.m_x, e.m_y );
	point.x = static_cast<int>( point.x * scale_ );
	point.y = static_cast<int>( point.y * scale_ );

	// x and y in hexes, not pixels
	int x = point.x;
	int y = point.y;
	screen2Hex( &x, &y );

	if ( ht::coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return;

#if defined HEXTOOLSPLAY
	if ( e.m_leftDown == 0 )
		left_button_down_ = FALSE;

	// QuickScroll(tm):
	if ( _quick_scroll_enabled && left_button_down_ == TRUE )
	{
		if ( _quick_scroll_mirrored )
			wxScrolledWindow::Scroll( ( qscroll_scroller_x0_ + ( qscroll_mouse_x0_ - point.x ) * _quick_scroll_velocity ), ( qscroll_scroller_y0_ + ( qscroll_mouse_y0_ - point.y ) * _quick_scroll_velocity ) );
		else
			wxScrolledWindow::Scroll( ( qscroll_scroller_x0_ + ( point.x - qscroll_mouse_x0_ ) * _quick_scroll_velocity ), ( qscroll_scroller_y0_ + ( point.y - qscroll_mouse_y0_ ) * _quick_scroll_velocity ) );
	}
#endif

	int x2 = point.x;
	int y2 = point.y;

	// used for hexside discrimination
	int i = get_hex_part( x2, y2 );

	// determine which hex has the hexside storage
	//	(it may be other than x, y)
	int xa, ya;
	Hex* checkHex = nullptr;
	if ( i & ( Hex::SOUTHEAST | Hex::SOUTHWEST | Hex::WEST ) )
		checkHex = &hex_[y][x];
	else if ( ht::getAdjHexCoords( i, x, y, &xa, &ya, world_x_size_, world_y_size_ ) )
	{
		checkHex = &hex_[ya][xa];
		i = ht::getAdjHexPart( i );
	}
#if 0
	bool demarcated = false;
#endif
	// this block of code decorates the char* terrain name with any sea ice prevailing.
	//	the four possible sources are:  icing sea hex, icing restricted sea hex,
	//	icing sea hexside (W, SW, or SE), icing restricted sea hexside (W, SW, or SE)
	bool lim = ( hex_[y][x].getLimitedStackingHex( ) );
	bool ice = ( hex_ices( hex_[y][x].terrain_ ) || hexsides_ice( hex_[y][x] ) );
	bool lateice = hex_[y][x].getLateSeaIce( );

	// hex terrain
	// for the unmarked field of the hex info pane:
	wxString hexterrain;
	hexterrain += _hex_types[hex_[y][x].terrain_].name_;
	if ( lim || ice )	hexterrain += " [";
	if ( lim )			hexterrain += "limited";
	if ( lim && ice )	hexterrain += "; ";
	if ( ice && lateice ) hexterrain += "late ";
	if ( ice )			hexterrain += "sea ice";
	if ( lim || ice )	hexterrain += "]";

	bool waterway = ( checkHex && checkHex->getWaterwayRoute( i ) );

	bool limitedStackingHexside = ( checkHex && checkHex->getLimitedStacking( i ) );

	// hexside terrain (rivers etc.)
	// for the Hexside: field of the hex info pane:
	int hexside = 0;

	if ( checkHex && checkHex->getSeasonalRiver( i ) )
		hexside = HexsideType::SEASONALRIVER;
	else if ( checkHex && checkHex->getMinorCanal( i ) )
		hexside = HexsideType::MINOR_CANAL;
	else if ( checkHex && checkHex->getCanal( i ) )
		hexside = HexsideType::CANAL;
	else if ( checkHex && checkHex->getMinorRiver( i ) )
		hexside = HexsideType::MINORRIVER;
	else if ( checkHex && checkHex->getFordableMajorRiver( i ) )
		hexside = HexsideType::MAJORRIVERFORD;
	else if ( checkHex && checkHex->getMajorRiver( i ) )
		hexside = HexsideType::MAJORRIVER;
	else if ( checkHex && checkHex->getFordableGreatRiver( i ) )
		hexside = HexsideType::GREATRIVERFORD;
	else if ( checkHex && checkHex->getLakeSide( i ) ) // i.e. great river
		hexside = HexsideType::LAKE_SIDE;
	else if ( checkHex && checkHex->getSeaSide( i ) && ! checkHex->getRestrictedWaterSide( i ) )
		hexside = HexsideType::SEA_SIDE;
	else if ( checkHex && checkHex->getIcingSeaSide( i ) && ! checkHex->getRestrictedWaterSide( i ) )
		hexside = HexsideType::ICING_SEA_SIDE;
	else if ( checkHex && checkHex->getSeaSide( i ) && checkHex->getRestrictedWaterSide( i ) )
		hexside = HexsideType::RWATER_SIDE;
	else if ( checkHex && checkHex->getIcingSeaSide( i ) && checkHex->getRestrictedWaterSide( i ) )
		hexside = HexsideType::ICING_RWATER_SIDE;
	else if ( checkHex && checkHex->getMountain( i ) )
		hexside = HexsideType::MTN_SIDE;
	else if ( checkHex && checkHex->getHighMtn( i ) )
		hexside = HexsideType::HIGH_MTN_SIDE;
	else if ( checkHex && checkHex->getHighMtnPass( i ) )
		hexside = HexsideType::HIGH_MTN_PASS;
	else if ( checkHex && checkHex->getKarst( i ) )
		hexside = HexsideType::KARST;
	else if ( checkHex && checkHex->getWadi( i ) )
		hexside = HexsideType::WADI;
	else if ( checkHex && checkHex->getGlacier( i ) )
		hexside = HexsideType::GLACIER_SIDE;
	else if ( checkHex && checkHex->getFillingReservoir( i ) )
		hexside = HexsideType::FILLING_RESERVOIR_SIDE;
	else if ( checkHex && checkHex->getSaltDesert( i ) )
		hexside = HexsideType::SALT_DESERT_SIDE;
	else if ( checkHex && checkHex->getDoubleEscarpment( i ) )
		hexside = HexsideType::DBL_ESCARPMENT;
	else if ( checkHex && checkHex->getImpassableDoubleEscarpment( i ) )
		hexside = HexsideType::IMP_DBL_ESCARPMENT;

	// borders are not mutex with other hexside terrain
	//	only want to report borders if nothing else more
	//	significant on hexside
	// if no other terrain, want to check these borders in
	//	top-to-bottom order, also note: skip demarkation
	//	for infopane purposes (still rendered in mappane)
	// TODO: refactor for districts and countries
#if 0
	else if ( checkHex && checkHex->getInternationalBorder( i ) )
	{
		hexside = HexsideType::BORDER;
		demarcated = checkHex->getDemarkationLine( i );
	}
	else if ( checkHex && checkHex->getInternalBorder( i ) )
	{
		hexside = HexsideType::IBORDER;
		demarcated = checkHex->getDemarkationLine( i );
	}
	else if ( checkHex && checkHex->getInternalSubBorder( i ) )
		hexside = HexsideType::SUBIBORDER;
#endif

	// for the Road: field of the hex info pane:
	int landRoute = 0;
	if ( checkHex && checkHex->getRailTrack( i ) )
		landRoute = HexsideType::RAILTRACK;
	else if ( checkHex && checkHex->getMotorTrack( i ) )
		landRoute = HexsideType::MOTORTRACK;
	else if ( checkHex && checkHex->getRailRoad( i ) )
		landRoute = HexsideType::RAILROAD;
	else if ( checkHex && checkHex->getRoad( i ) )
		landRoute = HexsideType::ROAD;
	else if ( checkHex && checkHex->getStandaloneBridge( i ) )
		landRoute = HexsideType::STANDALONEBRIDGE;
	else if ( checkHex && checkHex->getTrack( i ) )
		landRoute = HexsideType::TRACK;
	else if ( checkHex && checkHex->getRailFerry( i ) )
		landRoute = HexsideType::RAIL_FERRY;
	else if ( checkHex && checkHex->getNarrowStrait( i ) )
		landRoute = HexsideType::NARROW_STRAITS;
	else if ( checkHex && checkHex->getNarrowStraitsFerry( i ) )
		landRoute = HexsideType::NARROW_STRAITS_FERRY;
	else if ( checkHex && checkHex->getLakeSeaFerry( i ) )
		landRoute = HexsideType::LAKESEA_FERRY;
	else if ( checkHex && checkHex->getShippingChannel( i ) )
		landRoute = HexsideType::SHIP_CHANNEL;

#if defined HEXTOOLSPLAY
#if 0
	if ( IsLVRailRoad( x, y, i ) )	// order *does* matter
		landRoute = ( sit_[y][x].getGauge( ) == AXIS ) ? "L.vol RR (std)" : "L.vol RR (brd)";
	else if ( IsRoad( x, y, i ) && ! IsNarrowStrait( x, y, i ) )
		landRoute = "Road";
	else if ( IsTrail( x, y, i ) )
		landRoute = "Trail";
	else if ( IsRailRoad( x, y, i ) )
		landRoute = ( sit_[y][x].getGauge( ) == AXIS ) ? "Railroad (std)" : "Railroad (brd)";
	else
		landRoute = "";
#endif
#endif

	// resetting this for 6-way interior hexside checks
	i = get_hex_part( x2, y2 );
	checkHex = &hex_[y][x];

	// for the Other: field of the hex info pane:
	int interiorHexside = 0;

	if ( checkHex && checkHex->getFortifiedSide( i ) )
		interiorHexside = HexsideType::FORTIFIED;
	else if ( checkHex && checkHex->getGreatWall( i ) )
		interiorHexside = HexsideType::GREAT_WALL;
	else if ( checkHex && checkHex->getImpassableEscarpment( i ) )
		interiorHexside = HexsideType::IMP_ESCARPMENT;
	else if ( checkHex && checkHex->getEscarpment( i ) )
		interiorHexside = HexsideType::ESCARPMENT;
	else if ( checkHex && checkHex->getMountainInterior( i ) )
		interiorHexside = HexsideType::MTN_SIDE2;

	// for the locator field of the hex info pane:
	char local_map_id[ht::MAX_MAP_ID_LEN];
	hex_[y][x].getMapIdStr( local_map_id );
	char local_hex_id[ht::MAX_HEX_ID_LEN];
	hex_[y][x].getHexIdStr( local_hex_id );

	// check for vertex features (dams)
	int v = get_hex_vertex( x2, y2 );

	checkHex = nullptr;
	if ( v & ( Hex::SOUTHWEST_CORNER | Hex::SOUTH_CORNER ) ) // vertex stored in same hex
		checkHex = &hex_[y][x];
	else if ( ht::getCornerAdjHexCoords( v, x, y, &xa, &ya, world_x_size_, world_y_size_ ) )
	{
		checkHex = &hex_[ya][xa];
		// SW, N, SE use SW storage; (else) NW, NE, S use S storage:
		v = ( v & ( Hex::SOUTHWEST_CORNER | Hex::NORTH_CORNER | Hex::SOUTHEAST_CORNER ) ) ? Hex::SOUTHWEST_CORNER : Hex::SOUTH_CORNER;
	}

	bool dam = ( checkHex && checkHex->getDam( v ) ) ? true : false;

	wxString citynames;

	City* city = hex_[y][x].city_;
	bool first_city = true;
	while ( city != nullptr )
	{
		wxString cityname( city->name_ );
		cityname = City::scrubCityNameStyleAndRotationEncodings( cityname );
		citynames = cityname + ( first_city == false ? "; " : "" ) + citynames;
		first_city = false;
		city = city->getNext( );
	}

	int scId = hex_[y][x].getSeaDistrictID( );
	wxString sc = parent_->getSeaCircleDistrict( scId < 256 ? 256 : scId );

	int ldId = hex_[y][x].getLandDistrictID( );
	wxString ld = parent_->getLandDistrict( ldId < 256 ? 256 : ldId );

#if defined HEXTOOLSMAP
	wxString sz("");
	wxString cc("");

	char wz[3];
	wz[0] = '\0';
	char ws[100];
	ws[0] = '\0';
#elif defined HEXTOOLSPLAY

	// trim sea circle and district strings by the prefix Country_
	wxString trimmed_sc = sc.AfterFirst( '_' ); // will be empty if no underscore
	sc = trimmed_sc != "" ? trimmed_sc : sc;
	wxString trimmed_ld = ld.AfterFirst( '_' ); // will be empty if no underscore
	ld = trimmed_ld != "" ? trimmed_ld : ld;

	int szId = sit_[y][x].getSeazoneID( );
	wxString sz = parent_->getSeaZone( szId );

	int cId = sit_[y][x].getCountryID( );
	wxString cc = parent_->getCountry( cId );

	// weather zone
	// for the Weather: field of the hex info pane (in brackets)
	int wz_index = hex_[y][x].getWeatherZone( );
	char wz[3]; // 2-digit WZ codes, plus null terminator
	sprintf( wz, "%s", _weather_zone_string[wz_index] );

	// weather condition
	// for the Weather: field of the hex info pane (left of the brackets)
	int wthr = _weather_condition[wz_index];
	char ws[100]; // TODO: 20 chars is probably enough here:  "WXWXWX, -- ICE [WZ]/"
	sprintf( ws, "%s, %s", _weather_condition_string[wthr].c_str( ), _rivers_lakes_frozen[wz_index] ? "ice" : "no ice" );
#endif

	hexinfopane->displayHexInfo(
		x,
		y,
		local_map_id,
		local_hex_id,
		hexterrain,
		hexside,
		landRoute,
		interiorHexside,
		citynames,
		limitedStackingHexside,
		waterway,
		dam,
		sc,
		ld,
		sz,
		cc,
		wz,
		ws
	);
#if defined HEXTOOLSPLAY
	draw_movement_path( x, y );
#endif

#if defined HEXTOOLSMAP
	// left btn down, paint map
	if ( e.m_leftDown )
	{
		wxRect rect;
		if ( weather_zone_mode_ && selected_weather_zone_ > -1 )
			hex_[y][x].setWeatherZone( selected_weather_zone_ );
		else if ( mapleaf_mode_ && current_mapid_[0] != '\0' )
			hex_[y][x].setMapIdStr( current_mapid_ );
		else if ( sea_district_mode_ && selected_sea_district_ > 255 )
		{
			if ( hex_[y][x].terrain_ == HexType::SEA || hex_[y][x].terrain_ == HexType::ICINGSEA )
				hex_[y][x].setSeaDistrictID( selected_sea_district_ );
		}
		else if ( land_district_mode_ && selected_land_district_ > 255 )
		{
			if ( hex_[y][x].terrain_ != HexType::NONE )
				hex_[y][x].setLandDistrictID( selected_land_district_ );
		}
		else if ( parent_->terrain_selector_->getSelectedType( ) < HexType::TERRAINTYPECOUNT
				&& parent_->terrain_selector_->getSelectedType( ) != hex_[y][x].terrain_ )
		{
			if ( parent_->terrain_selector_->getSelectedType( ) == HexType::HEXTYPEKILL )
			{
					if ( hex_[y][x].city_ != nullptr )
				{
					delete hex_[y][x].city_;
						hex_[y][x].city_ = nullptr;
				}
				hex_[y][x].clearHexsideData( );
				hex_[y][x].terrain_ = HexType::NONE;
				hex_[y][x].setMapIdWord( 0xffff );
				hex_[y][x].setHexNumber( ht::DEFAULT_HEX_ID );
			}
			else
			{
				hex_[y][x].terrain_ = parent_->terrain_selector_->getSelectedType( );
			}
		}

		hex2Screen( &x, &y );
		rect.x = static_cast<int>( x / scale_ );
		rect.y = static_cast<int>( ( y - 3 ) / scale_ );
		rect.width = static_cast<int>( Hex::SIZE_X / scale_ );
		rect.height = static_cast<int>( ( Hex::SIZE_Y + 6 ) / scale_ );
		Refresh( false, &rect );
		file_is_dirty_ = true;
	}
#endif
}

// private helper for EvMouseMove(..)
// returns selected part of the hex
// where point resides,			 nw ne
// x and y in screen pixels		w HEX e
//                               sw se
int MapPane::get_hex_part( int x, int y )
{
	int x2 = x;
	int y2 = y;
	screen2Hex( &x2, &y2 );  // x2 and y2 of the left/top part of the hex
	hex2Screen( &x2, &y2 );

	int dx = x - x2;
	int dy = y - y2;
	if ( dx < Hex::HALF )  // left side of the hex
	{
		if ( dy < Hex::THIRD )
			return Hex::NORTHWEST;	// top
		if ( dy > Hex::TWOTHIRDS )
			return Hex::SOUTHWEST;	// bottom
		return Hex::WEST;			// middle
	}
	else  // right side of the hex
	{
		if ( dy < Hex::THIRD )
			return Hex::NORTHEAST;	// top
		if ( dy > Hex::TWOTHIRDS )
			return Hex::SOUTHEAST;	// bottom
		return Hex::EAST;			// middle
	}
	}

// private helper for EvMouseMove(..)
// returns vertex of the hex where point resides,     nw ne
// x and y in screen pixels                        	 w HEX e
int MapPane::get_hex_vertex( int x, int y ) 		  //  sw se
	{
	int x2 = x;
	int y2 = y;
	screen2Hex( &x2, &y2 );  // x2 and y2 hex of upper left corner
	hex2Screen( &x2, &y2 );

	int dx = x - x2;
	int dy = y - y2;

	if ( dy < Hex::HALF )	// top half of hex
		{
		if ( dx < Hex::THIRD ) return Hex::NORTHWEST_CORNER;	// left
		if ( dx > Hex::TWOTHIRDS ) return Hex::NORTHEAST_CORNER;	// right
		return Hex::NORTH_CORNER;								// middle
		}
	else				// bottom half of hex
		{
		if ( dx < Hex::THIRD ) return Hex::SOUTHWEST_CORNER;	// left
		if ( dx > Hex::TWOTHIRDS ) return Hex::SOUTHEAST_CORNER;	// right
		return Hex::SOUTH_CORNER;								// middle
	}
#if 0
	if ( dy <= TWOTHIRDSHEX )
		return 0;

	if ( dx < QUARTERHEX )
		return Hex::WEST;

	if ( dx < THREEFOURTHSHEX )
		return Hex::SOUTHWEST;

	return 0;
#endif
		}

#if defined HEXTOOLSPLAY

void MapPane::EvLButtonUp( wxMouseEvent& WXUNUSED(e) )
{
	left_button_down_ = FALSE;
	StatusMsg( "" );
}

// private helper for drawMovementPath(..) and eraseMovementPath(..)
void MapPane::draw_path( wxDC* dc, int x0, int y0, int x1, int y1 )
{
	int x, y, dir, x2, y2, mp, nx = 0, ny = 0, sx, sy, ex, ey, prev_mp, hmp, dir2;
	Counter* MovingCntr = _selected_unit_list;

	if ( ! MovingCntr )  // TODO: bug actually!
		return;

	dc->SetUserScale( 1.0 / scale_, 1.0 / scale_ );  // zooming

	if ( x0 == x1 && y0 == y1 )  // nothing to draw
		return;

	// start drawing from movement endpoint
	x = x1;
	y = y1;

	mp = prev_mp = 999;
	do
	{
		ENUM_DIRS(dir)
		{
			if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
			{
				dir2 = ht::getAdjHexPart( dir );
				// in train;
				if ( MovingCntr->getTrain( ) || move_mode_ == MovementMode::STRATEGIC )
				{
					if ( IsRailRoad( x2, y2, dir2 ) && ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
						nx = x2, ny = y2, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
				}
				// on foot (or air):
				else
				{
					if ( MovingCntr->getCounterType( ) == Counter::Type::SHIP )
					{
						hmp = 1;
						if ( ( ( ( sit_[y][x].getTmp( ) & 0x3fff ) - ( sit_[y2][x2].getTmp( ) & 0x3fff ) ) == hmp )
							&& ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
							nx = x2, ny = y2, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
					}
					else if ( MovingCntr->getCounterType( ) == Counter::Type::PLANE )
					{
						hmp = 1;
						if ( ( ( ( sit_[y][x].getTmp( ) & 0x3fff ) - ( sit_[y2][x2].getTmp( ) & 0x3fff ) ) == hmp )
							&& ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
							nx = x2, ny = y2, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
					}
					else
					{
						hmp = calculateHexCost( MovingCntr, x2, y2, dir2, 0 );  // TODO: DLT
						if ( ( ( ( sit_[y][x].getTmp( ) & 0x3fff ) - ( sit_[y2][x2].getTmp( ) & 0x3fff ) ) == hmp )
							&& ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
							nx = x2, ny = y2, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
					}
				}
			}
		}
		if ( mp >= prev_mp )  // sanity check: are we lost
			break;
		prev_mp = mp;
		sx = x;
		sy = y;
		ex = nx, ey = ny;
		hex2Screen( &sx, &sy );
		hex2Screen( &ex, &ey );
		dc->DrawLine( sx + 16, sy + 16, ex + 16, ey + 16 );
		x = nx, y = ny;
	}
	while ( x != x0 || y != y0 );
}

// private helper for EvMouseMove(..)
// Draw movement path from ptCurrentHex to x1,y2. Take advantage of
// distance field filled by ShowSelUnitMovementArea.
void MapPane::draw_movement_path( int x1, int y1 )
{
	wxWindowDC dc( this );
	int x0, y0, do_draw;
	static wxPen path_pen( *wxBLACK, 4, wxSOLID );
	static wxCursor cursor( wxCURSOR_HAND );  //(wxCURSOR_BULLSEYE);

	if ( ! _use_point_and_click_move )
		return;
	x0 = selected_hex_point_.x;
	y0 = selected_hex_point_.y;
	dc.SetLogicalFunction( wxINVERT );
	dc.SetPen( path_pen );
	do_draw = ( sit_[y1][x1].getTmp( ) && !sit_[y1][x1].isGrayed( ) );
	if ( x0 == x1 && y0 == y1 )
		do_draw = 0;
	// erase old:
	if ( path_drawn_ == TRUE )
	{
		if ( x0 == x1 && y0 == y1 )	// mouse over start hex
		{
			draw_path( &dc, x0, y0, previous_x_, previous_y_ );
			path_drawn_ = FALSE;
		}
		// won't work....
		//else if (Teleport) // pending teleport, erase old line
		//{
		//	DoDrawPath(&dc, x0, y0, prev_x1, prev_y1);
		//	MovementPathDrawn = 0;
		//	SetCursor(cursor);
		//	return;
		//}
		else if ( do_draw && previous_x_ == x1 && previous_y_ == y1 )
			do_draw = 123;
		else
		{
			draw_path( &dc, x0, y0, previous_x_, previous_y_ );
			path_drawn_ = FALSE;
		}
	}

	if ( do_draw == 0 )
		SetCursor( *wxSTANDARD_CURSOR );
	else if ( do_draw != 123 ) // keep cross cursor visible
	{
		x0 = selected_hex_point_.x;
		y0 = selected_hex_point_.y;
		draw_path( &dc, x0, y0, x1, y1 );
		path_drawn_ = TRUE;
		previous_x_ = x1;
		previous_y_ = y1;
		SetCursor( cursor );  //*wxCROSS_CURSOR);
	}
}

// private helper for EvLButtonDown(..)
void MapPane::erase_movement_path( )
{
	wxWindowDC dc( this );

	static wxPen path_pen( *wxBLACK, 4, wxSOLID );

	if ( ! _use_point_and_click_move || path_drawn_ == FALSE )
		return;

	int x0 = selected_hex_point_.x;
	int y0 = selected_hex_point_.y;
	dc.SetLogicalFunction( wxINVERT );
	dc.SetPen( path_pen );
	// erase old:
	draw_path( &dc, x0, y0, previous_x_, previous_y_ );
	path_drawn_ = FALSE;
}

char* MapPane::movement_path( int x1, int y1 )
{
	Counter* MovingCntr = _selected_unit_list;
	if ( ! MovingCntr )  // bug actually!
		return const_cast<char*>( "" );

	int x0 = selected_hex_point_.x;
	int y0 = selected_hex_point_.y;

	if ( x0 == x1 && y0 == y1 )  // nothing to draw
		return const_cast<char*>( "" );

	static char path[1000];  // 1000 hexes ought to be enough for everyone
	int x = x1;
	int y = y1;  // start drawing from movement endpoint
	int mp = 0;
	int prev_mp = 999;
	int nx = 0;
	int ny = 0; //, sx, sy, ex, ey;
	int ndir = 0;

	int i = 0;
	do
	{
		int dir;
		ENUM_DIRS(dir)
		{
			int x2, y2;
			if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
			{
				int dir2 = ht::getAdjHexPart( dir );
				// sailing;
				if ( MovingCntr->getCounterType( ) == Counter::Type::SHIP )
				{
					if ( ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
						nx = x2, ny = y2, ndir = dir, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
				}
				// flying;
				else if ( MovingCntr->getCounterType( ) == Counter::Type::PLANE )
				{
					if ( ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
						nx = x2, ny = y2, ndir = dir, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
				}
				// in train (tactical or strategic);
				else if ( MovingCntr->getTrain( ) || move_mode_ == MovementMode::STRATEGIC )
				{
					if ( IsRailRoad( x2, y2, dir2 ) && ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
						nx = x2, ny = y2, ndir = dir, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
				}
				// on foot:
				else
				{
					int hmp = calculateHexCost( MovingCntr, x2, y2, dir2, 0 );  // TODO: DLT
					if ( ( ( ( sit_[y][x].getTmp( ) & 0x3fff ) - ( sit_[y2][x2].getTmp( ) & 0x3fff ) ) == hmp )
						&& ( sit_[y2][x2].getTmp( ) & 0x3fff ) < mp )
						nx = x2, ny = y2, ndir = dir, mp = sit_[y2][x2].getTmp( ) & 0x3fff;
				}
			}
		}
		if ( mp >= prev_mp )  // sanity check: are we lost
			break;

		prev_mp = mp;

		path[i++] = dir2replay( ht::getAdjHexPart( ndir ) );
		x = nx;
		y = ny;
	}
	while ( x != x0 || y != y0 );

	path[i] = '\0';

	return path;
}

bool MapPane::air_combat_here( int x, int y )
{
	if ( ! sit_[y][x].isAirTarget( ) )
		return false;

	Counter* cntr;

	// TODO: why not ENUM_HEXUNITS(x, y, cntr) here?
	for ( cntr = sit_[y][x].unit_list_; cntr; cntr = cntr->getNext( ) )
		if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) == AirUnit::Mission::INTERCEPT )
			return true;

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
		ENUM_SELUNITS(cntr)
			if ( cntr->getCounterType( ) == Counter::Type::PLANE && cntr->getMission( ) == AirUnit::Mission::INTERCEPT )
				return true;

	return false;
}

#endif
