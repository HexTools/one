#include <iostream>

#include <algorithm>
using std::min;
using std::max;

#include <wx/wx.h>

// includes common to MAP/ and PLAY/maprenderer

#include "hextools.h"
using ht::wxS;

#include "intrinsicaf.h"
#include "port.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "hexinfopane.h"
#include "frame.h"
#if defined HEXTOOLSPLAY
#include "selectedunits.loop"
#include "hexunits.loop"
#include "application.h"
#include "armorantitank.h"
#include "sideplayer.h"
#endif
#include "mappane.h"

#if defined HEXTOOLSMAP
#include "terrainselector.h"
#include "dcity.h"
#endif

#if defined HEXTOOLSPLAY
#include "rules.h"
#include "phase.h"
#include "supplystatus.h"
#include "counter.h"

#include "groundunittype.h"
#include "groundunitsubtype.h"
#include "groundunit.h"
#include "airunitsubtype.h"
#include "airunit.h"
#include "navalunitsubtype.h"
#include "navalunit.h"
#include "hitmarker.h"
#include "hexcontents.h"
#include "hexcontentspane.h"

#include "view.h"
#include "views.h"
#include "dview.h"
#endif // HEXTOOLSPLAY includes

extern HexType _hex_types[];	// TERRAINTYPECOUNT

#if defined HEXTOOLSMAP
extern const char* _weather_zone_string[];
#endif

#if defined HEXTOOLSPLAY

extern Rules _rule_set;

extern GroundUnitType _ground_unit_type[GroundUnitType::WW2::UNITTYPECOUNT];	// unit types
extern GroundArmedForce _ground_armed_force[][GroundArmedForce::Allied::COUNT];	// unit sub types, per side
#if 0
extern AirArmedForce _air_armed_force[][AirArmedForce::Allied::COUNT];  		// unit sub types, per side
extern NavalArmedForce _naval_armed_force[][NavalArmedForce::Allied::COUNT];	// unit sub types, per side
#endif
extern int _current_phase;			// COMBAT_PHASE etc.
extern int _rivers_lakes_frozen[];		// [W_ZONES]  // true if rivers/lakes are frozen (frame.cpp)

extern Counter* _selected_unit_list;

extern int _gray_out_hexes;
extern int _show_MPs_on_map;
extern int _show_units;
extern int _show_combat_markers;
extern int _show_path;
#if 0
extern int _transparent_text;  // stack str text bgnd
#endif
extern int _show_corps_markers_on_map;

#endif // endif HEXTOOLSPLAY data

// fonts seem to differ from one platform to another:
//					sz,	family,		style,		weight
#if defined( __WXGTK__ )
wxFont big_font(	14,	wxDEFAULT,	wxNORMAL,	wxBOLD );
wxFont font(		10,	wxDEFAULT,	wxNORMAL,	wxNORMAL);
wxFont bfont(		10,	wxDEFAULT,	wxNORMAL,	wxBOLD );
wxFont small_font(	8,	wxDEFAULT,	wxNORMAL,	wxNORMAL);
#elif defined( __WXMSW__ )
wxFont xl_font(		14,	wxDEFAULT,	wxNORMAL,	wxNORMAL );
wxFont big_font(	10,	wxDEFAULT,	wxNORMAL,	wxNORMAL );
#ifdef HEXTOOLSPLAY
wxFont bfont( 		8,	wxSWISS,	wxNORMAL,	wxBOLD );
#endif
wxFont font(		8,	wxSWISS,	wxNORMAL,	wxNORMAL );
wxFont small_font(	6,	wxSWISS,	wxNORMAL,	wxNORMAL );
#else // MacOS probably needs some other fonts:
#error "Please define some fonts (see above how)!"
#endif

// global data specific to PLAY/maprenderer (counters, notes, reports, hex ownership roundels, paths)

#ifdef HEXTOOLSPLAY
extern wxBitmap* hbmAxis;
extern wxBitmap* hbmAllied;  // points to one of the above

#if 0
extern wxBitmap* hbmAxMine;
extern wxBitmap* hbmAlMine;
#endif
extern wxBitmap* hbmMulberry;
extern wxBitmap* hbmHexMask;  // hex graying
extern wxBitmap* hbmEntrenchent;
extern wxBitmap* hbmImpFort;
extern wxBitmap* hbmFieldWorks;
extern wxBitmap* hbmZepBase;
extern wxBitmap* hbmContested;
extern wxBitmap* hbmBalloons;
extern wxBitmap* hbmRP;
extern wxBitmap* hbmAlert;

// some toolbar bitmap ptr's from frame.cpp (for path icons)
extern wxBitmap* stagebmp;
extern wxBitmap* hitrailbmp;
extern wxBitmap* hitafbmp;
extern wxBitmap* repairbmp;
extern wxBitmap* regaugebmp;
extern wxBitmap* totrainbmp;
extern wxBitmap* fromtrainbmp;
extern wxBitmap* bombbmp;

// pens for path
extern wxPen* tac_pen;  // movement phase movement
extern wxPen* expl_pen;  // tactical exploitation movement
extern wxPen* combat_pen;  // advancing after combat

// draw corps markers 18X14 256 colors
// for static draw methods:
extern const int _counter_width;
extern const int _counter_height;

// used only in static draw_rpl(..) and MapPane::PaintPaths( )
static bool need_erase = false;
#endif

#ifdef HEXTOOLSPLAY

// private helpers for paint(..)
static void DrawCorpsCounter( wxDC* dc, int x, int y, wxColor fgnd, wxColor bgnd, wxColor txtcolor, int aec, int number )
{
	wxMemoryDC memdc, mem2dc;
	//wxColor fgnd = SubTypes[owner][subtype].
	//wxColor bgnd = SubTypes[owner][subtype].
	wxPen fgnd_pen( fgnd, 1, wxSOLID );
	wxPen bgnd_pen( bgnd, 1, wxSOLID );
	wxBrush bgnd_brush( bgnd, wxSOLID );
	wxBrush fgnd_brush( fgnd, wxSOLID );
	const int W = 15/*13*/, W_SYM = 13, H = 11;  // bitmap size
	wxBitmap bmp( W, H );
	char str[20];

	// counter background
	dc->SetPen( bgnd_pen );  // no border
	dc->SetBrush( bgnd_brush );
	dc->DrawRectangle( x + 1, y + 1, 26, 25 );
	// symbol
	switch ( aec )
	{
		case ArmorAntitank::Proportion::NONE:
			memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::INF].hbmBitmap );
			break;
		case ArmorAntitank::Proportion::PARTIAL:
			memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::MIXED].hbmBitmap );
			break;
		case ArmorAntitank::Proportion::HALF:
			memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::MECH].hbmBitmap );
			break;
		case ArmorAntitank::Proportion::FULL:
			memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::ARM].hbmBitmap );
			break;
	}
	mem2dc.SelectObject( bmp );  // mem2dc = counter color
	mem2dc.SetBrush( fgnd_brush );
	mem2dc.SetPen( fgnd_pen );
	mem2dc.DrawRectangle( 0, 0, W, H );
	dc->Blit( x + 7, y + 5, W_SYM, H, &mem2dc, 0, 0, wxXOR );
	dc->Blit( x + 7, y + 5, W_SYM, H, &memdc, 0, 0, wxAND );
	dc->Blit( x + 7, y + 5, W_SYM, H, &mem2dc, 0, 0, wxXOR );
	// flagpole:                      
	dc->SetPen( fgnd_pen );
	dc->DrawLine( x + 6, y + 5, x + 6, y + 25 );
	dc->DrawLine( x + 7, y + 16, x + 7, y + 25 );
	dc->SetTextBackground( bgnd );
	dc->SetTextForeground( txtcolor );
	// strength:
	sprintf( str, "%d", number );
	if ( number < 10 )
		ht::drawStr( dc, x + 12, y + 17, str );
	else if ( number < 100 )
		ht::drawStr( dc, x + 10, y + 17, str );
	else  // 100..999
		ht::drawStr( dc, x + 9, y + 17, str );
}

#if 0
static inline void flagpole( wxDC* dc, int x, int y )
{
	dc->DrawLine( x, y, x, y + 12 );
	dc->DrawLine( x + 1, y, x + 1, y + 12 );
}
#endif

// private helper for draw_tac(..), draw_expl(..), and draw_combat(..)
static void draw_rpl( wxDC* hdc, int* x, int* y, int dir, wxPen* pen )
{
	int x0 = *x;
	int y0 = *y;

	need_erase = true;
	switch ( dir )
	{
		case Hex::EAST:
			*x += Hex::SIZE_X;
			break;
		case Hex::SOUTHEAST:
			*x += Hex::HALF;
			*y += Hex::SIZE_Y;
			break;
		case Hex::NORTHEAST:
			*x += Hex::HALF;
			*y -= Hex::SIZE_Y;
			break;
		case Hex::WEST:
			*x -= Hex::SIZE_X;
			break;
		case Hex::SOUTHWEST:
			*x -= Hex::HALF;
			*y += Hex::SIZE_Y;
			break;
		case Hex::NORTHWEST:
			*x -= Hex::HALF;
			*y -= Hex::SIZE_Y;
			break;
	}
	hdc->SetPen( *pen );
	if ( x >= 0 && y >= 0 )
		hdc->DrawLine( x0, y0, *x, *y );
}

// private helpers for DrawPath(..)

// tactical movement replay
static void draw_tac( wxDC* hdc, int* x, int* y, int dir )
{
	draw_rpl( hdc, x, y, dir, tac_pen );
}

// exploitation movement replay
static void draw_expl( wxDC* hdc, int* x, int* y, int dir )
{
	draw_rpl( hdc, x, y, dir, expl_pen );
}

// exploitation movement replay
static void draw_combat( wxDC* hdc, int* x, int* y, int dir )
{
	draw_rpl( hdc, x, y, dir, combat_pen );
}

// private helper for initializeCCBitmap(..)
static void draw_arm( wxDC* hdc )
{
	hdc->DrawLine( _counter_width - 5, 4, 4, 4 );
	hdc->DrawLine( 4, 4, 4, _counter_height - 5 );
	hdc->DrawLine( 4, _counter_height - 5, _counter_width - 5, _counter_height - 5 );
	hdc->DrawLine( _counter_width - 5, _counter_height - 5, _counter_width - 5, 4 );
}

static void draw_inf( wxDC* hdc )
{
	hdc->DrawLine( 1, 1, _counter_width - 1, _counter_height - 1 );  // lines
	hdc->DrawLine( _counter_width, 0, 0, _counter_height - 1 );
}

static void draw_mxd( wxDC* hdc )
{
	// tank symbol
	hdc->DrawLine( _counter_width - 5, 4, 4, 4 );
	hdc->DrawLine( 4, 4, 4, _counter_height - 5 );
	hdc->DrawLine( 4, _counter_height - 5, _counter_width - 5, _counter_height - 5 );
	hdc->DrawLine( _counter_width - 5, _counter_height - 5, _counter_width - 5, 4 );
	// cross
	hdc->DrawLine( _counter_width - 6, 4, _counter_width - 1, 0 );
	hdc->DrawLine( 5, 4, 0, 0 );
	hdc->DrawLine( 5, _counter_height - 5, 0, _counter_height - 1 );
	hdc->DrawLine( _counter_width - 6, _counter_height - 5, _counter_width - 1, _counter_height - 1 );
}

static void draw_mec( wxDC* hdc )
{
	// lines
	hdc->DrawLine( 0, 0, _counter_width - 1, _counter_height - 1 );
	hdc->DrawLine( 0, _counter_height - 1, _counter_width - 1, 0 );
	// tank symbol
	hdc->DrawLine( _counter_width - 5, 4, 4, 4 );
	hdc->DrawLine( 4, 4, 4, _counter_height - 5 );
	hdc->DrawLine( 4, _counter_height - 5, _counter_width - 5, _counter_height - 5 );
	hdc->DrawLine( _counter_width - 5, _counter_height - 5, _counter_width - 5, 4 );
}

#endif

void MapPane::paint( wxPaintEvent& WXUNUSED(e) )
{
	wxPaintDC dc( this );
	PrepareDC( dc );					// must be called or automatic scrolling won't work
	//dc.SetOptimization( true );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	// do special painting for a "no game" phase
	if ( _current_phase == Phase::NO_GAME )
	{
		int x, y;
		GetClientSize( &x, &y );

		dc.SetBrush( wxBrush( wxColour( 255, 255, 255 ), wxSOLID ) );
		dc.DrawRectangle( 0, 0, x, y );
		x /= 2;
		y /= 2;

		// TODO: redesign an "infobox" image for HexTools
		//hdcMem.SelectObject( *hbmInfoBox );
		//dc.Blit( x - ( hbmInfoBox->GetWidth( ) / 2 ), y - ( hbmInfoBox->GetHeight( ) / 2 ), hbmInfoBox->GetWidth( ), hbmInfoBox->GetHeight( ), &hdcMem, 0, 0 );

		return;
	}

	// TODO: w/o this, a msg reference is to the global msg[10000] defined in mappane.cpp
	char msg[500];
#endif

	dc.SetUserScale( 1.0 / scale_, 1.0 / scale_ );

	int min_x = 0;
	int max_x = 0;
	int min_y = 0;
	int max_y = 0;

	wxRegionIterator upd( GetUpdateRegion( ) );
	while ( upd )
	{
		// Paint damaged area only, !!! coordinates in hexes, NOT pixels !!!
		int hexsize = (int)( Hex::SIZE_X / scale_ );

		min_x = ( upd.GetX( ) / hexsize ) - 1;
		max_x = ( ( upd.GetX( ) + upd.GetW( ) ) / hexsize ) + 2;  // because hexrows have 1/2 hex offset
		min_y = ( upd.GetY( ) / hexsize ) - 1;
		max_y = ( ( upd.GetY( ) + upd.GetH( ) ) / hexsize ) + 1;

		// above coordinates are unscrolled, convert them to scrolled ones:
		int vx, vy;
		CalcUnscrolledPosition( 0, 0, &vx, &vy );
		vx /= hexsize;
		vy /= hexsize;

		min_x += vx;
		max_x += vx;
		min_y += vy;
		max_y += vy;

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#if defined __WXMSW__
		// map edges may need to be painted if not covered with hexes (because
		// automatic background repainting is disabled in order to reduce
		// flickering. Silly GTK 1.2 always paints background (and flickers!),
		// so this is not needed there.
		if ( min_x == 0 )
		{
			dc.SetPen( *wxLIGHT_GREY_PEN );
			dc.SetBrush( *wxLIGHT_GREY_BRUSH );
			dc.DrawRectangle( 0, min_y * Hex::SIZE_Y, Hex::SIZE_X, ( ( max_y - min_y ) + 1 ) * Hex::SIZE_Y );
		}
		if ( max_x > world_x_size_ )  // big screen or zoomed mode
		{
			dc.SetPen( *wxLIGHT_GREY_PEN );
			dc.SetBrush( *wxLIGHT_GREY_BRUSH );
			dc.DrawRectangle( world_x_size_ * Hex::SIZE_X, min_y * Hex::SIZE_Y, max_x * Hex::SIZE_X, ( max_y - min_y ) * Hex::SIZE_Y );
		}
		if ( max_y > world_y_size_ )
		{
			dc.SetPen( *wxLIGHT_GREY_PEN );
			dc.SetBrush( *wxLIGHT_GREY_BRUSH );
			dc.DrawRectangle( min_x * Hex::SIZE_X, world_y_size_ * Hex::SIZE_Y, ( ( max_x - min_x ) + 1 ) * Hex::SIZE_X, ( max_y - world_y_size_ ) * Hex::SIZE_Y );
		}
#endif // __WXMSW__
#endif
		// limit painting area to the map:
        if ( min_x < 0 )
        	min_x = 0;
        //min_x = limit( min_x, 0, NoUpperBound );

        // TODO: vet this
        //if ( max_x > iXWorldSize )
        //	max_x = iXWorldSize;
        if ( max_x > world_x_size_ - 1 )
        	max_x = world_x_size_ - 1;
        //max_x = limit( max_x, NoLowerBound, iXWorldSize - 1 );

        if ( min_y < 0 )
        	min_y = 0;
        //min_y = limit( min_y, 0, NoUpperBound );

        if ( max_y > world_y_size_ - 1 )
        	max_y = world_y_size_ - 1;
        //max_y = limit( max_y, NoLowerBound, iYWorldSize - 1 );

		// offsets to be used for atolls, small islands, and major (partial hex) cities
		int island_yo = 0, island_xo = 0;
		int city_yo = 0, city_xo = 0;

		wxMemoryDC hdcMem, hdcMem2, hdcMem3, hdcMem4;
#if 0
		wxBitmap* bm;
#endif
		dc.SetTextBackground( wxColour( 255, 255, 255 ) );
		dc.SetFont( font );

		// hexside features & city names must be painted in adjacent hexes:
        int min_xx = max( min_x - 1, 0 );
		int max_xx = min( max_x + 1, world_x_size_ - 1 ); // , iXWorldSize );
        int min_yy = max( min_y - 1, 0 );
        int max_yy = min( max_y + 1, world_y_size_ - 1 );

#if defined DOUBLE_BUFFERING
		blit_h = upd.GetY() + upd.GetH();
		blit_w = upd.GetX() + upd.GetW();
		scr_x = upd.GetX();
		scr_y = upd.GetY();

		wxBitmap bmp( 1000, 800 );
		dc.SelectObject( bmp );
		dc.SetDeviceOrigin( min_x * 32, min_y * 32 ); // handle scrolling
#endif

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
		// kludge: don't need to redraw map, just gray hexes, etc.
		// TODO: so TakeOff(..) won't trigger a mappane redraw?
		if ( dont_redraw_map_ == 1 ) // not just TRUE/FALSE:  MapPane::Takeoff(..) sets this to 2
		{
			upd++;
			continue;
		}
#endif

		hdcMem2.SelectObject( *hbmPartialHexCity );
		hdcMem3.SelectObject( *hbmRestrictedWatersOverlay );
		hdcMem4.SelectObject( *hbmFullHexCity );

		// hex terrain, primary cities (defer primary city text), and restricted waters

		int x, y, x_pos, y_pos;
		int xo, yo, tx, ty;

		for ( y = max_y; y >= min_y; --y )
		{
			y_pos = y << 5;  // y << 5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_x; x < max_x; ++x )
			{
				x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );  // y & 1 == y % 2 ; << 4 == *16 == *Hex::HALF

				hdcMem.SelectObject( *_hex_types[HexType::HEXMASK].hbmBitmap );
				dc.Blit( x_pos, y_pos - 3, 32, 38, &hdcMem, 0, 0, wxOR );

				wxBitmap bmp;
				int terrain = hex_[y][x].terrain_;
				switch ( terrain )
				{
					case HexType::ATOLL:
					case HexType::SMALLISLAND:
						bmp = *_hex_types[HexType::SEA].hbmBitmap;
						break;
					default:
						bmp = *_hex_types[terrain].hbmBitmap;
						break;
				}
				hdcMem.SelectObject( bmp );
				dc.Blit( x_pos, y_pos - 3, 32, 38, &hdcMem, 0, 0, wxAND );

				City* cptr = hex_[y][x].city_;

				island_yo = 0, island_xo = 0;
				city_yo = 0, city_xo = 0;

				// blitting these city types now, deferring text drawing to later
				if ( cptr != nullptr && cptr->type_ == City::Type::FULLHEX )
					dc.Blit( x_pos, y_pos - 3, 32, 38, &hdcMem4, 0, 0, wxAND );
				else if ( cptr != nullptr && cptr->type_ == City::Type::PARTIALHEX )
				{
					ht::hexMajorCityPositionOffset( cptr->position_, &city_xo, &city_yo );
					dc.Blit( x_pos + city_xo, y_pos + city_yo, 25, 26, &hdcMem2, 0, 0, wxAND );
				}
				else if ( cptr != nullptr &&
					( cptr->type_ == City::Type::ATOLL_CUSTOM || cptr->type_ == City::Type::SMALL_ISLAND_CUSTOM ) )
				{
					// the above cities have clear underterrain, and so do not require the
					//	same treatment as for custom islands below (sea underterrain)
					ht::hexIslandPositionOffset( cptr->position_, &island_xo, &island_yo );
					// throw down mask, then invert it out
					hdcMem.SelectObject( *hbmIslandCustomMask );
					dc.Blit( x_pos + island_xo, y_pos + island_yo, 32, 38, &hdcMem, 0, 0, wxAND );
					dc.Blit( x_pos + island_xo, y_pos + island_yo, 32, 38, &hdcMem, 0, 0, wxOR_INVERT );
					// and the image
					hdcMem.SelectObject( cptr->type_ == City::Type::ATOLL_CUSTOM ? *_hex_types[HexType::ATOLL].hbmBitmap : *_hex_types[HexType::SMALLISLAND].hbmBitmap );
					dc.Blit( x_pos + island_xo, y_pos + island_yo, 32, 38, &hdcMem, 0, 0, wxAND );
				}

				// hdcMem3 is the restricted waters overlay
				if ( hex_[y][x].getRestrictedWaters( ) &&
						(	terrain == HexType::SEA || terrain == HexType::ICINGSEA ||
							terrain == HexType::SMALLISLAND || terrain == HexType::ATOLL )	)
					dc.Blit( x_pos, y_pos - 3, 32, 38, &hdcMem3, 0, 0, wxAND );
			}
		}
		hdcMem.SelectObject( *_hex_types[HexType::NONE].hbmBitmap );
		hdcMem2.SelectObject( *_hex_types[HexType::NONE].hbmBitmap );
		hdcMem3.SelectObject( *_hex_types[HexType::NONE].hbmBitmap );

		// ornamental hexsides (for smoothing blocky edges)
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y << 5 == y * 32 == y * Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
				if ( hex_[y][x].hasClearOrnaments( ) )
				{
					if ( hex_[y][x].getClearOrnament( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEClear );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSEDecoMask );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSEClear );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getClearOrnament( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWClear );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSWDecoMask );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSWClear );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getClearOrnament( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWClear );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmWDecoMask );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmWClear );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
					}
				}
				if ( hex_[y][x].hasRoughOrnaments( ) )
				{
					if ( hex_[y][x].getRoughOrnament( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSERough );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSEDecoMask );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSERough );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getRoughOrnament( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWRough );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSWDecoMask );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSWRough );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getRoughOrnament( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWRough );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmWDecoMask );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmWRough );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
					}
				}
				if ( hex_[y][x].hasWoodsOrnaments( ) )
				{
					if ( hex_[y][x].getWoodsOrnament( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEWoods );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSEDecoMask );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSEWoods );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getWoodsOrnament( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWWoods );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSWDecoMask );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSWWoods );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getWoodsOrnament( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWWoods );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmWDecoMask );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmWWoods );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
					}
				}
				if ( hex_[y][x].hasForestOrnaments( ) )
				{
					if ( hex_[y][x].getForestOrnament( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEForest );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSEDecoMask );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSEForest );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getForestOrnament( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWForest );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSWDecoMask );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSWForest );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getForestOrnament( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWForest );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmWDecoMask );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmWForest );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
					}
				}
				if ( hex_[y][x].hasWoodedRoughOrnaments( ) )
				{
					if ( hex_[y][x].getWoodedRoughOrnament( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEWoodedRough );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSEDecoMask );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSEWoodedRough );
						dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getWoodedRoughOrnament( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWWoodedRough );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmSWDecoMask );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmSWWoodedRough );
						dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].getWoodedRoughOrnament( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWWoodedRough );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbmWDecoMask );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbmWWoodedRough );
						dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxXOR );
					}
				}
			}
		}

		// draw minor rivers, seasonal rivers, canals, minor canals, and wadis
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

				// TODO: minor canals need frozen weather treatments in HT-p
				int ldistid = hex_[y][x].getLandDistrictID( );
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
				int country_id = sit_[y][x].getCountryID( );
#endif
				if ( hex_[y][x].hasMinorCanals( ) )
				{
					if ( hex_[y][x].getMinorCanal( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSECanalMask );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xse = 0, yse = 0;
						int hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEBdrMinCanal;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEIBdrMinCanal;
						else
							bm = hbmSEMinorCanal;

						bm = ( hasSeHex && ldistid != hex_[yse][xse].getLandDistrictID( ) ) ? hbmSEIBdrMinCanal : hbmSEMinorCanal;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSeHex )
						{
							if ( ldistid != hex_[yse][xse].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSEIBdrMinCanal );
								hdcMem.SelectObject( *hbmSEBdrMinCanal );

								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSERBorder );
									hdcMem.SelectObject( *hbmSEBorder );

									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSEMinorCanal );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SE hex
					} // minor canal SE

					if ( hex_[y][x].getMinorCanal( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWCanalMask );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xsw = 0, ysw = 0;
						int hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWBdrMinCanal;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWIBdrMinCanal;
						else
							bm = hbmSWMinorCanal;

						bm = ( hasSwHex && ldistid != hex_[ysw][xsw].getLandDistrictID( ) ) ? hbmSWIBdrMinCanal : hbmSWMinorCanal;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSwHex )
						{
							if ( ldistid != hex_[ysw][xsw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSWIBdrMinCanal );
								hdcMem.SelectObject( *hbmSWBdrMinCanal );

								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSWRBorder );
									hdcMem.SelectObject( *hbmSWBorder );

									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSWMinorCanal );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SW hex
					} // minor canal SW

					if ( hex_[y][x].getMinorCanal( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWCanalMask );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxOR ); // not x - 3, y + 3 like std rivers?

						int xw = 0, yw = 0;
						int hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::WEST ) )
							bm = hbmWBdrMinCanal;
						else if ( hex[y][x].getInternalBorder( Hex::WEST ) )
							bm = hbmWIBdrMinCanal;
						else
							bm = hbmWMinorCanal;

						bm = ( hasWHex && ldistid != hex_[yw][xw].getLandDistrictID( ) ) ? hbmWIBdrMinCanal : hbmWMinorCanal;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND ); // not x - 3, y + 3 like std rivers?
#endif

						if ( hasWHex )
						{
							if ( ldistid != hex_[yw][xw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmWIBdrMinCanal );
								hdcMem.SelectObject( *hbmWBdrMinCanal );

								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
								{
									 // blit red border if sea zone IDs are different across hexside
									hdcMem3.SelectObject( *hbmWRBorder );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmWMinorCanal );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has W hex
					} // minor canal W
				}

				// TODO: canals need frozen weather treatments in HT-p
				if ( hex_[y][x].hasCanals( ) )
				{
					hdcMem.SelectObject( wxNullBitmap );
					if ( hex_[y][x].getCanal( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSECanalMask );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xse = 0, yse = 0;
						int hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );
#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEBdrCanal;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEIBdrCanal;
						else
							bm = hbmSECanal;

						bm = ( hasSeHex && ldistid != hex_[yse][xse].getLandDistrictID( ) ) ? hbmSEIBdrCanal : hbmSECanal;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSeHex )
						{
							if ( ldistid != hex_[yse][xse].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSEIBdrCanal );
								hdcMem.SelectObject( *hbmSEBdrCanal );

								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSERBorder );
									hdcMem.SelectObject( *hbmSEBorder );

									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSECanal );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SE hex
					} // canal SE

					if ( hex_[y][x].getCanal( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWCanalMask );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xsw = 0, ysw = 0;
						int hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWBdrCanal;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWIBdrCanal;
						else
							bm = hbmSWCanal;

						bm = ( hasSwHex && ldistid != hex_[ysw][xsw].getLandDistrictID( ) ) ? hbmSWIBdrCanal : hbmSWCanal;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSwHex )
						{
							if ( ldistid != hex_[ysw][xsw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSWIBdrCanal );
								hdcMem.SelectObject( *hbmSWBdrCanal );

								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSWRBorder );
									hdcMem.SelectObject( *hbmSWBorder );

									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSWCanal );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SW hex
					} // canal SW

					if ( hex_[y][x].getCanal( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWCanalMask );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxOR ); // not x - 3, y + 3 like std rivers?

						int xw = 0, yw = 0;
						int hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );
#if 0
						if ( hex[y][x].getInternationalBorder( Hex::WEST ) )
							bm = hbmWBdrCanal;
						else if ( hex[y][x].getInternalBorder( Hex::WEST ) )
							bm = hbmWIBdrCanal;
						else
							bm = hbmWCanal;

						bm = ( hasWHex && ldistid != hex_[yw][xw].getLandDistrictID( ) ) ? hbmWIBdrCanal : hbmWCanal;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND ); // not x - 3, y + 3 like std rivers?
#endif

						if ( hasWHex )
						{
							if ( ldistid != hex_[yw][xw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmWIBdrCanal );
								hdcMem.SelectObject( *hbmWBdrCanal );

								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
								{
									 // blit red border if sea zone IDs are different across hexside
									hdcMem3.SelectObject( *hbmWRBorder );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmWCanal );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has W hex
					} // canal W
				}

				if ( hex_[y][x].hasWadis( ) )
				{
					hdcMem.SelectObject( wxNullBitmap );
					if ( hex_[y][x].getWadi( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEWadiMask );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xse = 0, yse = 0;
						int hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );
#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEBdrWadi;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEIBdrWadi;
						else
							bm = hbmSEWadi;

						bm = ( hasSeHex && ldistid != hex_[yse][xse].getLandDistrictID( ) ) ? hbmSEIBdrWadi : hbmSEWadi;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSeHex )
						{
							if ( ldistid != hex_[yse][xse].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSEIBdrWadi );
								hdcMem.SelectObject( *hbmSEBdrWadi );

								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSERBorder );
									hdcMem.SelectObject( *hbmSEBorder );

									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSEWadi );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SE hex
					} // wadi SE

					if ( hex_[y][x].getWadi( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWWadiMask );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xsw = 0, ysw = 0;
						int hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWBdrWadi;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWIBdrWadi;
						else
							bm = hbmSWWadi;

						bm = ( hasSwHex && ldistid != hex_[ysw][xsw].getLandDistrictID( ) ) ? hbmSWIBdrWadi : hbmSWWadi;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSwHex )
						{
							if ( ldistid != hex_[ysw][xsw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSWIBdrWadi );
								hdcMem.SelectObject( *hbmSWBdrWadi );

								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSWRBorder );
									hdcMem.SelectObject( *hbmSWBorder );

									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSWWadi );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SW hex
					} // wadi SW

					if ( hex_[y][x].getWadi( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWWadiMask );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxOR );

						int xw = 0, yw = 0;
						int hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );
#if 0
						if ( hex[y][x].getInternationalBorder( Hex::WEST ) )
							bm = hbmWBdrWadi;
						else if ( hex[y][x].getInternalBorder( Hex::WEST ) )
							bm = hbmWIBdrWadi;
						else
							bm = hbmWWadi;

						bm = ( hasWHex && ldistid != hex_[yw][xw].getLandDistrictID( ) ) ? hbmWIBdrWadi : hbmWWadi;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasWHex )
						{
							if ( ldistid != hex_[yw][xw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmWIBdrWadi );
								hdcMem.SelectObject( *hbmWBdrWadi );

								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
								{
									 // blit red border if sea zone IDs are different across hexside
									hdcMem3.SelectObject( *hbmWRBorder );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmWWadi );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has W hex
					} // wadi W
				}

				// TODO: do seasonal rivers need frozen weather treatments in HT-p?
				if ( hex_[y][x].hasSeasonalRivers( ) )
				{
					hdcMem.SelectObject( wxNullBitmap );
					if ( hex_[y][x].getSeasonalRiver( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSESeasonalRivMask );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR_INVERT );

						int xse = 0, yse = 0;
						int hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );
#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEBdrSeasonRiver;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEIBdrSeasonRiver;
						else
							bm = hbmSESeasonalRiv;

						bm = ( hasSeHex && ldistid != hex_[yse][xse].getLandDistrictID( ) ) ? hbmSEIBdrSeasonRiver : hbmSESeasonalRiv;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSeHex )
						{
							if ( ldistid != hex_[yse][xse].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSEIBdrSeasonRiver );
								hdcMem.SelectObject( *hbmSEBdrSeasonRiver );

								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSERBorder );
									hdcMem.SelectObject( *hbmSEBorder );

									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSESeasonalRiv );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SE hex
					} // seasonal river SE

					if ( hex_[y][x].getSeasonalRiver( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWSeasonalRivMask );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR_INVERT );

						int xsw = 0, ysw = 0;
						int hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWBdrSeasonRiver;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWIBdrSeasonRiver;
						else
							bm = hbmSWSeasonalRiv;

						bm = ( hasSwHex && ldistid != hex_[ysw][xsw].getLandDistrictID( ) ) ? hbmSWIBdrSeasonRiver : hbmSWSeasonalRiv;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSwHex )
						{
							if ( ldistid != hex_[ysw][xsw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSWIBdrSeasonRiver );
								hdcMem.SelectObject( *hbmSWBdrSeasonRiver );

								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSWRBorder );
									hdcMem.SelectObject( *hbmSWBorder );

									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSWSeasonalRiv );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SW hex
					} // seasonal river SW

					if ( hex_[y][x].getSeasonalRiver( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWSeasonalRivMask );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxOR_INVERT ); // not y + 3 like std rivers?

						int xw = 0, yw = 0;
						int hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::WEST ) )
							bm = hbmWBdrSeasonRiver;
						else if ( hex[y][x].getInternalBorder( Hex::WEST ) )
							bm = hbmWIBdrSeasonRiver;
						else
							bm = hbmWSeasonalRiv;

						bm = ( hasWHex && ldistid != hex_[yw][xw].getLandDistrictID( ) ) ? hbmWIBdrSeasonRiver : hbmWSeasonalRiv;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND ); // not y + 3 like std rivers?
#endif

						if ( hasWHex )
						{
							if ( ldistid != hex_[yw][xw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmWIBdrSeasonRiver );
								hdcMem.SelectObject( *hbmWBdrSeasonRiver );

								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
								{
									 // blit red border if sea zone IDs are different across hexside
									hdcMem3.SelectObject( *hbmWRBorder );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmWSeasonalRiv );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has W hex
					} // wadi W
				}

				if ( hex_[y][x].hasMinorRivers( ) )
				{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
					int wz __attribute__ ((unused)) = hex_[y][x].getWeatherZone( );
#endif
					hdcMem.SelectObject( wxNullBitmap );
					if ( hex_[y][x].getMinorRiver( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEWadiMask );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xse = 0, yse = 0;
						int hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEBdrMinRiver;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHEAST ) )
							bm = hbmSEIBdrMinRiver;
						else
							bm = hbmSEMinRiv;
//						else if ( Frozen[wz] ) // TODO: refactor for new hexside geometry
//							bm = hbmSEMinRivIce;

						bm = ( hasSeHex && ldistid != hex_[yse][xse].getLandDistrictID( ) ) ? hbmSEIBdrMinRiver : hbmSEMinRiv;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSeHex )
						{
							if ( ldistid != hex_[yse][xse].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSEIBdrMinRiver );
								hdcMem.SelectObject( *hbmSEBdrMinRiver );

								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSERBorder );
									hdcMem.SelectObject( *hbmSEBorder );

									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSEMinRiv );
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SE hex
					} // minor river SE

					if ( hex_[y][x].getMinorRiver( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWWadiMask );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxOR );

						int xsw = 0, ysw = 0;
						int hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWBdrMinRiver;
						else if ( hex[y][x].getInternalBorder( Hex::SOUTHWEST ) )
							bm = hbmSWIBdrMinRiver;
						else
							bm = hbmSWMinRiv;
//						else if ( Frozen[wz] ) // TODO: refactor for new hexside geometry
//							bm = hbmSWMinRivIce;

						bm = ( hasSwHex && ldistid != hex_[ysw][xsw].getLandDistrictID( ) ) ? hbmSWIBdrMinRiver : hbmSWMinRiv;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasSwHex )
						{
							if ( ldistid != hex_[ysw][xsw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSWIBdrMinRiver );
								hdcMem.SelectObject( *hbmSWBdrMinRiver );

								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
								{
									// blit red border if sea zone IDs are different across hexside
									hdcMem2.SelectObject( *hbmSWRBorder );
									hdcMem.SelectObject( *hbmSWBorder );

									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmSWMinRiv );
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has SW hex
					} // minor river SW

					if ( hex_[y][x].getMinorRiver( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWWadiMask );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxOR );

						int xw = 0, yw = 0;
						int hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );

#if 0
						if ( hex[y][x].getInternationalBorder( Hex::WEST ) )
							bm = hbmWBdrMinRiver;
						else if ( hex[y][x].getInternalBorder( Hex::WEST ) )
							bm = hbmWIBdrMinRiver;
						else
							bm = hbmWMinRiv;
//						else if ( Frozen[wz] ) // TODO: refactor for new hexside geometry
//							bm = hbmWMinRivIce;

						bm = ( hasWHex && ldistid != hex_[yw][xw].getLandDistrictID( ) ) ? hbmWIBdrMinRiver : hbmWMinRiv;

						hdcMem.SelectObject( *bm );
						dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
#endif

						if ( hasWHex )
						{
							if ( ldistid != hex_[yw][xw].getLandDistrictID( ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmWIBdrMinRiver );
								hdcMem.SelectObject( *hbmWBdrMinRiver );

								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
							}
							else // in HT-p, when district ID matches across the hexside
							{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
								{
									 // blit red border if sea zone IDs are different across hexside
									hdcMem3.SelectObject( *hbmWRBorder );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
								}
								else // same country, same district, just blit the feature
#endif
								{
									hdcMem.SelectObject( *hbmWMinRiv );
									dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
								}
							}
						} // has W hex
					} // minor river W
				}
			}
		}

		// draw major rivers
		for ( y = max_yy; y >= min_yy; y-- )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; x++ )
			{
				if ( hex_[y][x].hasMajorRivers( ) || hex_[y][x].hasFordableMajorRivers( ) )
				{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
					int wz = hex_[y][x].getWeatherZone( );
#endif
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( hex_[y][x].getMajorRiver( Hex::SOUTHEAST ) || hex_[y][x].getFordableMajorRiver( Hex::SOUTHEAST ) )
					{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						if ( _rivers_lakes_frozen[wz] ) // TODO: refactor for new hexside geometry
							dc.DrawBitmap( *hbmSEMajRivIce, x_pos + 16, y_pos + 29 );
#endif
						hdcMem.SelectObject( *hbmSEMajRivMask );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 12, &hdcMem, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmSEMajRiv );
						dc.Blit( x_pos + 16, y_pos + 27, 16, 12, &hdcMem, 0, 0, wxAND );
					}
					if ( hex_[y][x].getMajorRiver( Hex::SOUTHWEST ) || hex_[y][x].getFordableMajorRiver( Hex::SOUTHWEST ) )
					{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						if ( _rivers_lakes_frozen[wz] ) // TODO: refactor for new hexside geometry
							dc.DrawBitmap( *hbmSWMajRivIce, x_pos, y_pos + 29 );
#endif
						hdcMem.SelectObject( *hbmSWMajRivMask );
						dc.Blit( x_pos, y_pos + 27, 16, 12, &hdcMem, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmSWMajRiv );
						dc.Blit( x_pos, y_pos + 27, 16, 12, &hdcMem, 0, 0, wxAND );
					}
					if ( hex_[y][x].getMajorRiver( Hex::WEST ) || hex_[y][x].getFordableMajorRiver( Hex::WEST ) )
					{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						if ( _rivers_lakes_frozen[wz] ) // TODO: refactor for new hexside geometry
							dc.DrawBitmap( *hbmWMajRivIce, x_pos, y_pos + 29 );
#endif
						hdcMem.SelectObject( *hbmWMajRivMask );
						dc.Blit( x_pos - 3, y_pos + 4, 8, 24, &hdcMem, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmWMajRiv );
						dc.Blit( x_pos - 3, y_pos + 4, 8, 24, &hdcMem, 0, 0, wxAND );
					}
				}
			}
		}

		// water hexsides ( restricted water, sea/lake/great rivers )
		wxBitmap* hbm;
		wxBitmap* hbm2;
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				// TODO: possible zoom scale handling for Scale >= 5 (see old JET code below)
				// 			e.g. block water hexsides (faster but ugly)
				if ( hex_[y][x].hasAnyWater( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					if ( hex_[y][x].hasWaterSide( Hex::SOUTHEAST ) )
					{
						// TODO:  need frozen weather treatments in HT-p
						hbm = hex_[y][x].getIcingSeaSide( Hex::SOUTHEAST ) ? hbmSEIcingSea : hbmSEWater;
						hbm2 = hex_[y][x].hasOpenWater( Hex::SOUTHEAST ) ? hbmSEMask : hbmSELakeRiverMask;

						hdcMem.SelectObject( *hbm );
						dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );

						hdcMem.SelectObject( *hbm2 );
						dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );

						hdcMem.SelectObject( *hbm );
						dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
					}

					if ( hex_[y][x].hasWaterSide( Hex::SOUTHWEST ) )
					{
						// TODO:  need frozen weather treatments in HT-p
						hbm = hex_[y][x].getIcingSeaSide( Hex::SOUTHWEST ) ? hbmSWIcingSea : hbmSWWater ;
						hbm2 = hex_[y][x].hasOpenWater( Hex::SOUTHWEST ) ? hbmSWMask : hbmSWLakeRiverMask;

						hdcMem.SelectObject( *hbm );
						dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbm2 );
						dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbm );
						dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
					}
					if ( hex_[y][x].hasWaterSide( Hex::WEST ) )
					{
						// TODO:  need frozen weather treatments in HT-p
						hbm = hex_[y][x].getIcingSeaSide( Hex::WEST ) ? hbmWIcingSea : hbmWWater;
						hbm2 = hex_[y][x].hasOpenWater( Hex::WEST ) ? hbmWMask : hbmWLakeRiverMask;

						hdcMem.SelectObject( *hbm );
						dc.Blit( x_pos - 8, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
						hdcMem.SelectObject( *hbm2 );
						dc.Blit( x_pos - 8, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxAND );
						hdcMem.SelectObject( *hbm );
						dc.Blit( x_pos - 8, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
					}
				}
			}
		}

		// 2nd pass at water hexsides for restricted waters hexsides
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				int xw = 0, yw = 0;
				int hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );
				int xnw = 0, ynw = 0;
				int hasNwHex = ht::getAdjHexCoords( Hex::NORTHWEST, x, y, &xnw, &ynw, world_x_size_, world_y_size_ );
				int xne = 0, yne = 0;
				int hasNeHex = ht::getAdjHexCoords( Hex::NORTHEAST, x, y, &xne, &yne, world_x_size_, world_y_size_ );
				int xe = 0, ye = 0;
				int hasEHex = ht::getAdjHexCoords( Hex::EAST, x, y, &xe, &ye, world_x_size_, world_y_size_ );
				int xse = 0, yse = 0;
				int hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );
				int xsw = 0, ysw = 0;
				int hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

				// only checking for hexsides here; full restricted waters hexes were handled
				//	during hex terrain blitting (bottom/beginning of z-order)
				if ( hex_[y][x].hasAnyWater( ) ||
					( hasWHex && hex_[yw][xw].hasAnyWater( ) ) ||
					( hasNwHex && hex_[ynw][xnw].hasAnyWater( ) ) ||
					( hasNeHex && hex_[yne][xne].hasAnyWater( ) ) ||
					( hasEHex && hex_[ye][xe].hasAnyWater( ) ) ||
					( hasSeHex && hex_[yse][xse].hasAnyWater( ) )	)
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					if ( hex_[y][x].getRestrictedWaters( ) )
					{
						if ( hex_[y][x].hasOpenWater( Hex::SOUTHEAST ) && hex_[y][x].getRestrictedWaterSide( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSERestrMask );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].hasOpenWater( Hex::SOUTHWEST ) && hex_[y][x].getRestrictedWaterSide( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWRestrMask );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].hasOpenWater( Hex::WEST )&& hex_[y][x].getRestrictedWaterSide( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWRestrMask );
							dc.Blit( x_pos - 8, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxAND );
						}

						if ( hasNwHex && hex_[ynw][xnw].hasOpenWater( Hex::SOUTHEAST ) && hex_[ynw][xnw].getRestrictedWaterSide( Hex::SOUTHEAST )  )
						{
							hdcMem.SelectObject( *hbmNWRestrMask );
							dc.Blit( x_pos + 16 - Hex::SIZE_X / 2, y_pos + 24 - Hex::SIZE_Y, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hasNeHex && hex_[yne][xne].hasOpenWater( Hex::SOUTHWEST ) && hex_[yne][xne].getRestrictedWaterSide( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmNERestrMask );
							dc.Blit( x_pos + Hex::SIZE_X / 2, y_pos + 24 - Hex::SIZE_Y, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hasEHex && hex_[ye][xe].hasOpenWater( Hex::WEST ) && hex_[ye][xe].getRestrictedWaterSide( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmERestrMask );
							dc.Blit( x_pos - 8 + Hex::SIZE_X, y_pos + 4, 16, 24, &hdcMem, 0, 0, wxAND );
						}
					}

					// TODO: need to validate this short circuit logic
					// also need to check vertices, if hex is non-sea [or is that not possible here??]
					//if ( hex[y][x].terrain == SEA || hex[y][x].terrain == ICINGSEA )
						//continue;

					if (	hasNwHex &&	hex_[ynw][xnw].getRestrictedWaters( ) &&
							hasNeHex &&	hex_[yne][xne].getRestrictedWaters( ) &&
							hex_[yne][xne].hasOpenWater( Hex::WEST ) &&
							hex_[yne][xne].getRestrictedWaterSide( Hex::WEST )
						)
					{
						hdcMem.SelectObject( *hbmNRestrVertexMask );
						dc.Blit( x_pos + Hex::HALF - 1, y_pos - 4, 3, 3, &hdcMem, 0, 0, wxAND );
					}

					if (	hasSwHex &&	hex_[ysw][xsw].getRestrictedWaters( ) &&
							hasSeHex &&	hex_[yse][xse].getRestrictedWaters( ) &&
							hex_[yse][xse].hasOpenWater( Hex::WEST ) &&
							hex_[yse][xse].getRestrictedWaterSide( Hex::WEST )
						)
					{
						hdcMem.SelectObject( *hbmSRestrVertexMask );
						dc.Blit( x_pos + Hex::HALF - 1, y_pos + Hex::SIZE_Y + 1, 3, 3, &hdcMem, 0, 0, wxAND );
					}

					if (	hasWHex && hex_[yw][xw].getRestrictedWaters( ) &&
							hasNwHex &&	hex_[ynw][xnw].getRestrictedWaters( ) &&
							hex_[ynw][xnw].hasOpenWater( Hex::SOUTHWEST ) &&
							hex_[ynw][xnw].getRestrictedWaterSide( Hex::SOUTHWEST )
						)
					{
						hdcMem.SelectObject( *hbmNWRestrVertexMask );
						dc.Blit( x_pos, y_pos + 3, 3, 3, &hdcMem, 0, 0, wxAND );
					}

					if (	hasSwHex &&	hex_[ysw][xsw].getRestrictedWaters( ) &&
							hasWHex && hex_[yw][xw].getRestrictedWaters( ) &&
							hex_[yw][xw].hasOpenWater( Hex::SOUTHEAST ) &&
							hex_[yw][xw].getRestrictedWaterSide( Hex::SOUTHEAST )
						)
					{
						hdcMem.SelectObject( *hbmSWRestrVertexMask );
						dc.Blit( x_pos, y_pos + Hex::SIZE_Y - 6, 3, 3, &hdcMem, 0, 0, wxAND );
					}

					if (	hasEHex && hex_[ye][xe].getRestrictedWaters( ) &&
							hasNeHex &&	hex_[yne][xne].getRestrictedWaters( ) &&
							hex_[yne][xne].hasOpenWater( Hex::SOUTHEAST ) &&
							hex_[yne][xne].getRestrictedWaterSide( Hex::SOUTHEAST )
						)
					{
						hdcMem.SelectObject( *hbmNERestrVertexMask );
						dc.Blit( x_pos + Hex::SIZE_X - 2, y_pos + 3, 3, 3, &hdcMem, 0, 0, wxAND );
					}

					if (	hasSeHex &&	hex_[yse][xse].getRestrictedWaters( ) &&
							hasEHex && hex_[ye][xe].getRestrictedWaters( ) &&
							hex_[ye][xe].hasOpenWater( Hex::SOUTHWEST ) &&
							hex_[ye][xe].getRestrictedWaterSide( Hex::SOUTHWEST )
						)
					{
						hdcMem.SelectObject( *hbmSERestrVertexMask );
						dc.Blit( x_pos + Hex::SIZE_X - 2, y_pos + Hex::SIZE_Y - 6, 3, 3, &hdcMem, 0, 0, wxAND );
					}
				}
			}
		}

		// draw fords
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].hasFordableGreatRivers( ) || hex_[y][x].hasFordableMajorRivers( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( hex_[y][x].getFordableGreatRiver( Hex::SOUTHEAST ) || hex_[y][x].getFordableMajorRiver( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEFordMask );
						dc.Blit( x_pos + 18, y_pos + 26, 11, 12, &hdcMem, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmSEFord );
						dc.Blit( x_pos + 18, y_pos + 26, 11, 12, &hdcMem, 0, 0, wxAND );
					}
					if ( hex_[y][x].getFordableGreatRiver( Hex::SOUTHWEST ) || hex_[y][x].getFordableMajorRiver( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWFordMask ); // larger bitmap size here, due to covering wider SW great river breadth
						dc.Blit( x_pos + 3, y_pos + 26, 11, 12, &hdcMem, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmSWFord ); // larger bitmap size here, due to covering wider SW great river breadth
						dc.Blit( x_pos + 3, y_pos + 26, 11, 12, &hdcMem, 0, 0, wxAND );
					}
					if ( hex_[y][x].getFordableGreatRiver( Hex::WEST ) || hex_[y][x].getFordableMajorRiver( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWFordMask );
						dc.Blit( x_pos - 7, y_pos + 13, 14, 6, &hdcMem, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmWFord );
						dc.Blit( x_pos - 7, y_pos + 13, 14, 6, &hdcMem, 0, 0, wxAND );
					}
				}
			}
		}

		// rare hexsides
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
				if ( hex_[y][x].getRareHexsidesFlag( ) )
				{
					// mtn hexsides (on hexside proper, which is distinct from mtns on interior hexsides)
					if ( hex_[y][x].hasMountains( ) )
					{
						if ( hex_[y][x].getMountain( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEMountain );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSEMountainMask );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmSEMountain );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getMountain( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWMountain );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSWMountainMask );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmSWMountain );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getMountain( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWMountain );
							dc.Blit( x_pos - 7, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmWMountainMask );
							dc.Blit( x_pos - 7, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmWMountain );
							dc.Blit( x_pos - 7, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
						}
					}
					// high mtn hexsides
					if ( hex_[y][x].hasHighMtns( ) )
					{
						if ( hex_[y][x].getHighMtn( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEHighMtnMask );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSEHighMtn );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getHighMtn( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWHighMtnMask );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSWHighMtn );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getHighMtn( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWHighMtnMask );
							dc.Blit( x_pos - 7, y_pos + 4, 13, 24, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmWHighMtn );
							dc.Blit( x_pos - 7, y_pos + 4, 13, 24, &hdcMem, 0, 0, wxAND );
						}
					}
					if ( hex_[y][x].hasHighMtnPasses( ) )
					{
						if ( hex_[y][x].getHighMtnPass( Hex::SOUTHEAST ) )
							dc.DrawBitmap( *hbmSEHighMtnPass, x_pos + 16, y_pos + 25 );
						if ( hex_[y][x].getHighMtnPass( Hex::SOUTHWEST ) )
							dc.DrawBitmap( *hbmSWHighMtnPass, x_pos, y_pos + 25 );
						if ( hex_[y][x].getHighMtnPass( Hex::WEST ) )
							dc.DrawBitmap( *hbmWHighMtnPass, x_pos - 3, y_pos + 4 );
					}
					if ( hex_[y][x].hasKarsts( ) )
					{
						if ( hex_[y][x].getKarst( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEKarst );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getKarst( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWKarst );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getKarst( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWKarst );
							dc.Blit( x_pos - 4, y_pos + 4, 8, 24, &hdcMem, 0, 0, wxAND );
						}
					}
					if ( hex_[y][x].hasFillingReservoirs( ) )
					{
						if ( hex_[y][x].getFillingReservoir( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEFillingReservoir );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSELakeRiverMask );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmSEFillingReservoir );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getFillingReservoir( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWFillingReservoir );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSWLakeRiverMask );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmSWFillingReservoir );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getFillingReservoir( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWFillingReservoir );
							dc.Blit( x_pos - 8, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmWLakeRiverMask );
							dc.Blit( x_pos - 8, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmWFillingReservoir );
							dc.Blit( x_pos - 8, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
						}
					}
					if ( hex_[y][x].hasGlaciers( ) )
					{
						if ( hex_[y][x].getGlacier( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEGlacierMask );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSEGlacier );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getGlacier( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWGlacierMask );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSWGlacier );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getGlacier( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWGlacierMask );
							dc.Blit( x_pos - 4, y_pos + 4, 10, 24, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmWGlacier );
							dc.Blit( x_pos - 4, y_pos + 4, 10, 24, &hdcMem, 0, 0, wxAND );
						}
					}
					if ( hex_[y][x].hasSaltDeserts( ) )
					{
						if ( hex_[y][x].getSaltDesert( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSESaltDesert );
							dc.Blit(  x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSESaltDesertMask );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmSESaltDesert );
							dc.Blit( x_pos + 13, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getSaltDesert( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWSaltDesert );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSWSaltDesertMask );
							dc.Blit( x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmSWSaltDesert );
							dc.Blit(  x_pos - 3, y_pos + 21, 22, 22, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getSaltDesert( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWSaltDesert );
							dc.Blit(x_pos - 7, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmWSaltDesertMask );
							dc.Blit( x_pos - 7, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmWSaltDesert );
							dc.Blit(x_pos - 7, y_pos + 1, 16, 30, &hdcMem, 0, 0, wxXOR );
						}
					}
					// hexside proper, distinct from escarpments on interior hexsides
					if ( hex_[y][x].hasDoubleEscarpments( ) )
					{
						if ( hex_[y][x].getDoubleEscarpment( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSEDblEsc );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getDoubleEscarpment( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSWDblEsc );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getDoubleEscarpment( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos - 3, y_pos + 4, 7, 24, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmWDblEsc );
							dc.Blit( x_pos - 3, y_pos + 4, 7, 24, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos - 3, y_pos + 4, 7, 24, &hdcMem, 0, 0, wxXOR );
						}
					}
					// on hexside proper, distinct from impass. escarpments on interior hexsides
					if ( hex_[y][x].hasImpassableDoubleEscarpments( ) )
					{
						if ( hex_[y][x].getImpassableDoubleEscarpment( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEDblEsc );
							dc.Blit( x_pos + 16, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getImpassableDoubleEscarpment( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWDblEsc );
							dc.Blit( x_pos, y_pos + 24, 16, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getImpassableDoubleEscarpment( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWDblEsc );
							dc.Blit( x_pos - 3, y_pos + 4, 7, 24, &hdcMem, 0, 0, wxAND );
						}
					}
				}
			}  // for x
		}  // for y

		// z order is bottom up:  district/sea circle borders, international/zonal borders

		// dashed borders
		// (map-60:  gray for sea circle boundaries)
		// (map-47:  black for sea zone boundaries)
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].isWaterHex( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					int seabox_id = hex_[y][x].getSeaDistrictID( );
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
					int seazone_id = sit_[y][x].getSeazoneID( );
#endif
					int xse = 0, yse = 0;
					bool hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );

					if ( hasSeHex && hex_[yse][xse].terrain_ != HexType::NONE )
					{
						if ( seabox_id != hex_[yse][xse].getSeaDistrictID( ) )
						{
							// pick gray (HT-m/p) or black (HT-p-only)
							bool interzonal_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
							if ( seazone_id != sit_[yse][xse].getSeazoneID( ) )
								interzonal_border = true;
#endif
							hdcMem2.SelectObject( interzonal_border ? *hbmSEIBBorder : *hbmSEISBorder );
							hdcMem.SelectObject( *hbmSEISBorderMask );

							dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
							dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
						}
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						else // in HT-p when sea circle IDs are the same
						{
							if ( seazone_id != sit_[yse][xse].getSeazoneID( ) )
							{
								// blit red border if sea zone IDs are different across hexside
								hdcMem2.SelectObject( *hbmSERBorder );
								hdcMem.SelectObject( *hbmSEBorder );

								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
						}
#endif
					}

					int xsw = 0, ysw = 0;
					bool hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

					if ( hasSwHex && hex_[ysw][xsw].terrain_ != HexType::NONE )
					{
						if ( seabox_id != hex_[ysw][xsw].getSeaDistrictID( ) )
						{
							// pick gray (HT-m/p) or black (HT-p-only)
							bool interzonal_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
							if ( seazone_id != sit_[ysw][xsw].getSeazoneID( ) )
								interzonal_border = true;
#endif
							hdcMem2.SelectObject( interzonal_border ? *hbmSWIBBorder : *hbmSWISBorder );
							hdcMem.SelectObject( *hbmSWISBorderMask );

							dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
							dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
						}
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						else // in HT-p when sea circle IDs are the same
						{
							if ( seazone_id != sit_[ysw][xsw].getSeazoneID( ) )
							{
								// blit red border if sea zone IDs are different across hexside
								hdcMem2.SelectObject( *hbmSWRBorder );
								hdcMem.SelectObject( *hbmSWBorder );

								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
						}
#endif
					}

					int xw = 0, yw = 0;
					bool hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );

					if ( hasWHex && hex_[yw][xw].terrain_ != HexType::NONE )
					{
						if ( seabox_id != hex_[yw][xw].getSeaDistrictID( ) )
						{
							// pick gray (HT-m/p) or black (HT-p-only)
							bool interzonal_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
							if ( seazone_id != sit_[yw][xw].getSeazoneID( ) )
								interzonal_border = true;
#endif
							hdcMem2.SelectObject( interzonal_border ? *hbmWIBBorder : *hbmWISBorder );
							hdcMem.SelectObject( *hbmWISBorderMask );

							dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
							dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem, 0, 0, wxAND );
							dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem2, 0, 0, wxXOR );
						}
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						else // in HT-p when sea circle IDs are the same
						{
							if ( seazone_id != sit_[yw][xw].getSeazoneID( ) )
							{
								 // blit red border if sea zone IDs are different across hexside
								hdcMem3.SelectObject( *hbmWRBorder );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
							}
						}
#endif
					}
				}
			}
		}

		// solid borders
		// (map-60:  for district boundaries)
		// (map-47:  for international boundaries)
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].terrain_ != HexType::SEA && hex_[y][x].terrain_ != HexType::ICINGSEA && hex_[y][x].terrain_ != HexType::NONE )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					int ldistid = hex_[y][x].getLandDistrictID( );
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
					int country_id = sit_[y][x].getCountryID( );
#endif
					int xse = 0, yse = 0;
					bool hasSeHex = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xse, &yse, world_x_size_, world_y_size_ );

					if ( hasSeHex
							&& hex_[yse][xse].terrain_ != HexType::NONE
							&& hex_[yse][xse].terrain_ != HexType::SEA
							&& hex_[yse][xse].terrain_ != HexType::ICINGSEA
						)
					{
						if ( ldistid != hex_[yse][xse].getLandDistrictID( ) )
						{
							// if there is a minor river or wadi, the border has already been
							//	accounted for with a composite river-border bitmap
							if (	! hex_[y][x].getMinorCanal( Hex::SOUTHEAST ) &&
									! hex_[y][x].getCanal( Hex::SOUTHEAST ) &&
									! hex_[y][x].getWadi( Hex::SOUTHEAST ) &&
									! hex_[y][x].getSeasonalRiver( Hex::SOUTHEAST ) &&
									! hex_[y][x].getMinorRiver( Hex::SOUTHEAST ) &&
									! hex_[y][x].getStandaloneBridge( Hex::SOUTHEAST ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yse][xse].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSEIBorder );
								hdcMem.SelectObject( *hbmSEBorder );

								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
						}
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						else // in HT-p, when district ID is same across the hexside
						{
							if ( country_id != sit_[yse][xse].getCountryID( ) )
							{
								// blit red border if sea zone IDs are different across hexside
								hdcMem2.SelectObject( *hbmSERBorder );
								hdcMem.SelectObject( *hbmSEBorder );

								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
						}
#endif
					}

					int xsw = 0, ysw = 0;
					bool hasSwHex = ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xsw, &ysw, world_x_size_, world_y_size_ );

					if ( hasSwHex
							&& hex_[ysw][xsw].terrain_ != HexType::NONE
							&& hex_[ysw][xsw].terrain_ != HexType::SEA
							&& hex_[ysw][xsw].terrain_ != HexType::ICINGSEA
						)
					{
						if ( ldistid != hex_[ysw][xsw].getLandDistrictID( ) )
						{
							// if there is a minor river or wadi, the border has already been
							//	accounted for with a composite river-border bitmap
							if (	! hex_[y][x].getMinorCanal( Hex::SOUTHWEST ) &&
									! hex_[y][x].getCanal( Hex::SOUTHWEST ) &&
									! hex_[y][x].getWadi( Hex::SOUTHWEST ) &&
									! hex_[y][x].getSeasonalRiver( Hex::SOUTHWEST ) &&
									! hex_[y][x].getMinorRiver( Hex::SOUTHWEST ) &&
									! hex_[y][x].getStandaloneBridge( Hex::SOUTHWEST ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[ysw][xsw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem2.SelectObject( *hbmSWIBorder );
								hdcMem.SelectObject( *hbmSWBorder );

								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								if ( ! international_border )
									dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
						}
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						else // in HT-p, when district ID is same across the hexside
						{
							if ( country_id != sit_[ysw][xsw].getCountryID( ) )
							{
								// blit red border if sea zone IDs are different across hexside
								hdcMem2.SelectObject( *hbmSWRBorder );
								hdcMem.SelectObject( *hbmSWBorder );

								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem, 0, 0, wxAND );
								dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, wxXOR );
							}
						}
#endif
					}

					int xw = 0, yw = 0;
					bool hasWHex = ht::getAdjHexCoords( Hex::WEST, x, y, &xw, &yw, world_x_size_, world_y_size_ );

					if ( hasWHex
							&& hex_[yw][xw].terrain_ != HexType::NONE
							&& hex_[yw][xw].terrain_ != HexType::SEA
							&& hex_[yw][xw].terrain_ != HexType::ICINGSEA
						)
					{
						if ( ldistid != hex_[yw][xw].getLandDistrictID( ) )
						{
							// if there is a minor river or wadi, the border has already been
							//	accounted for with a composite river-border bitmap
							if (	! hex_[y][x].getMinorCanal( Hex::WEST ) &&
									! hex_[y][x].getCanal( Hex::WEST ) &&
									! hex_[y][x].getWadi( Hex::WEST ) &&
									! hex_[y][x].getSeasonalRiver( Hex::WEST ) &&
									! hex_[y][x].getMinorRiver( Hex::WEST ) &&
									! hex_[y][x].getStandaloneBridge( Hex::WEST ) )
							{
								// pick gray (HT-m/p) or black (HT-p-only)
								bool international_border = false;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
								if ( country_id != sit_[yw][xw].getCountryID( ) )
									international_border = true;
#endif
								hdcMem3.SelectObject( international_border ? *hbmWBorder : *hbmWIBorder );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
							}
						}
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
						else // in HT-p, when district ID is same across the hexside
						{
							if ( country_id != sit_[yw][xw].getCountryID( ) )
							{
								 // blit red border if sea zone IDs are different across hexside
								hdcMem3.SelectObject( *hbmWRBorder );
								dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0 );
							}
						}
#endif
					}
				}
			}
		}
#if 0
		// international borders
		hdcMem.SelectObject( *hbmSEBorder );
		hdcMem2.SelectObject( *hbmSWBorder );
		hdcMem3.SelectObject( *hbmWBorder );
#if defined __WXMSW__
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex[y][x].hasInternationalBorders( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					// if there is a minor river or wadi, the border has already been
					//	accounted for with a composite river-border bitmap
					int hasAdj;
					int xxx, yyy;
					bool seaBorder;
					int rop; // logical rendering operator
					if ( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) )
					{
						if (	!hex[y][x].getMinorCanal( Hex::SOUTHEAST ) &&
								!hex[y][x].getCanal( Hex::SOUTHEAST ) &&
								!hex[y][x].getWadi( Hex::SOUTHEAST ) &&
								!hex[y][x].getSeasonalRiver( Hex::SOUTHEAST ) &&
								!hex[y][x].getMinorRiver( Hex::SOUTHEAST ) &&
								!hex[y][x].getStandaloneBridge( Hex::SOUTHEAST ) )
						{
							hasAdj = getAdjHex( Hex::SOUTHEAST, x, y, &xxx, &yyy );
							seaBorder = hex[y][x].terrain == SEA ||
										hex[y][x].terrain == ICINGSEA ||
										( hasAdj && hex[yyy][xxx].terrain == SEA ) ||
										( hasAdj && hex[yyy][xxx].terrain == ICINGSEA ) ||
										hex[y][x].getSeaSide( Hex::SOUTHEAST ) ||
										hex[y][x].getIcingSeaSide( Hex::SOUTHEAST );
							rop = seaBorder && hex[y][x].getDemarkationLine( Hex::SOUTHEAST ) ? wxOR_INVERT : wxAND;
							dc.Blit( x_pos + 16, y_pos + 27, 16, 10, &hdcMem, 0, 0, rop );
						}
					}
					if ( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) )
					{
						if (	!hex[y][x].getMinorCanal( Hex::SOUTHWEST ) &&
								!hex[y][x].getCanal( Hex::SOUTHWEST ) &&
								!hex[y][x].getWadi( Hex::SOUTHWEST ) &&
								!hex[y][x].getSeasonalRiver( Hex::SOUTHWEST ) &&
								!hex[y][x].getMinorRiver( Hex::SOUTHWEST ) &&
								!hex[y][x].getStandaloneBridge( Hex::SOUTHWEST ) )
						{
							hasAdj = getAdjHex( Hex::SOUTHWEST, x, y, &xxx, &yyy );
							seaBorder = hex[y][x].terrain == SEA ||
										hex[y][x].terrain == ICINGSEA ||
										( hasAdj && hex[yyy][xxx].terrain == SEA ) ||
										( hasAdj && hex[yyy][xxx].terrain == ICINGSEA ) ||
										hex[y][x].getSeaSide( Hex::SOUTHWEST ) ||
										hex[y][x].getIcingSeaSide( Hex::SOUTHWEST );
							rop = seaBorder && hex[y][x].getDemarkationLine( Hex::SOUTHWEST ) ? wxOR_INVERT : wxAND;
							dc.Blit( x_pos, y_pos + 27, 16, 10, &hdcMem2, 0, 0, rop );
						}
					}
					if ( hex[y][x].getInternationalBorder( Hex::WEST ) )
					{
						if (	!hex[y][x].getMinorCanal( Hex::WEST ) &&
								!hex[y][x].getCanal( Hex::WEST ) &&
								!hex[y][x].getWadi( Hex::WEST ) &&
								!hex[y][x].getSeasonalRiver( Hex::WEST ) &&
								!hex[y][x].getMinorRiver( Hex::WEST ) &&
								!hex[y][x].getStandaloneBridge( Hex::WEST ) )
						{
							hasAdj = getAdjHex( Hex::WEST, x, y, &xxx, &yyy );
							seaBorder = hex[y][x].terrain == SEA ||
										hex[y][x].terrain == ICINGSEA ||
										( hasAdj && hex[yyy][xxx].terrain == SEA ) ||
										( hasAdj && hex[yyy][xxx].terrain == ICINGSEA ) ||
										hex[y][x].getSeaSide( Hex::WEST ) ||
										hex[y][x].getIcingSeaSide( Hex::WEST );
							rop = seaBorder && hex[y][x].getDemarkationLine( Hex::WEST ) ? wxOR_INVERT : wxAND;
							dc.Blit( x_pos - 2, y_pos + 4, 4, 24, &hdcMem3, 0, 0, rop );
						}
					}
				}
			}
		}
#else
		dc.SetLogicalFunction( wxAND );
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex[y][x].hasInternationalBorders( ) )
				{
					x_pos= ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) )
					{
						if ( !hex[y][x].getMinorRiver( Hex::SOUTHEAST ) )
						dc.DrawBitmap( *hbmSEBorder, x_pos + 16, y_pos + 29 );
					}
					if ( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) )
					{
						if ( !hex[y][x].getMinorRiver( Hex::SOUTHWEST ) )
						dc.DrawBitmap( *hbmSWBorder, x_pos, y_pos + 29 );
					}
					if ( hex[y][x].getInternationalBorder( Hex::WEST ) )
					{
						if ( !hex[y][x].getMinorRiver( Hex::WEST ) )
						dc.DrawBitmap( *hbmWBorder, x_pos - 3, y_pos + 2 );
					}
				}
			}
		}
		dc.SetLogicalFunction( wxCOPY );
#endif
#endif

#if 0
		// demarcation line
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex[y][x].hasDemarkationLines( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					int xxx, yyy;
					bool seaBorder;
					if ( hex[y][x].getDemarkationLine( Hex::SOUTHEAST ) )
					{
						int hasAdj = getAdjHex( Hex::SOUTHEAST, x, y, &xxx, &yyy );
						seaBorder = hex[y][x].terrain == SEA ||
									hex[y][x].terrain == ICINGSEA ||
									( hasAdj && hex[yyy][xxx].terrain == SEA ) ||
									( hasAdj && hex[yyy][xxx].terrain == ICINGSEA ) ||
									hex[y][x].getSeaSide( Hex::SOUTHEAST ) ||
									hex[y][x].getIcingSeaSide( Hex::SOUTHEAST );
						if ( ! seaBorder )
						{
							dc.SetPen( hex[y][x].getInternationalBorder( Hex::SOUTHEAST ) ? *hpenDemarcateBorder : *hpenDemarcateIBorder );
							dc.DrawLine( x_pos + Hex::SIZE_X - 1, y_pos + Hex::SIZE_Y - 4, x_pos + Hex::HALF - 1, y_pos + Hex::SIZE_Y + 4 );
						}
					}
					if ( hex[y][x].getDemarkationLine( Hex::SOUTHWEST ) )
					{
						int hasAdj = getAdjHex( Hex::SOUTHWEST, x, y, &xxx, &yyy );
						seaBorder = hex[y][x].terrain == SEA ||
									hex[y][x].terrain == ICINGSEA ||
									( hasAdj && hex[yyy][xxx].terrain == SEA ) ||
									( hasAdj && hex[yyy][xxx].terrain == ICINGSEA ) ||
									hex[y][x].getSeaSide( Hex::SOUTHWEST ) ||
									hex[y][x].getIcingSeaSide( Hex::SOUTHWEST );
						if ( ! seaBorder )
						{
							dc.SetPen( hex[y][x].getInternationalBorder( Hex::SOUTHWEST ) ? *hpenDemarcateBorder : *hpenDemarcateIBorder );
							dc.DrawLine( x_pos + Hex::HALF - 1, y_pos + Hex::SIZE_Y + 4, x_pos - 1, y_pos + Hex::SIZE_Y - 4 );
						}
					}
					if ( hex[y][x].getDemarkationLine( Hex::WEST ) )
					{
						int hasAdj = getAdjHex( Hex::WEST, x, y, &xxx, &yyy );
						seaBorder = hex[y][x].terrain == SEA ||
									hex[y][x].terrain == ICINGSEA ||
									( hasAdj && hex[yyy][xxx].terrain == SEA ) ||
									( hasAdj && hex[yyy][xxx].terrain == ICINGSEA ) ||
									hex[y][x].getSeaSide( Hex::WEST ) ||
									hex[y][x].getIcingSeaSide( Hex::WEST );
						if ( ! seaBorder )
						{
							dc.SetPen( hex[y][x].getInternationalBorder( Hex::WEST ) ? *hpenDemarcateBorder : *hpenDemarcateIBorder );
							dc.DrawLine( x_pos - 1, y_pos + Hex::SIZE_Y - 4, x_pos - 1, y_pos + 4 );
						}
					}
				}
			}
		}
#endif
		// waterway route
		dc.SetPen( *hpenWaterwayRoute );
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].hasWaterwayRoutes( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( hex_[y][x].getWaterwayRoute( Hex::WEST ) )
						dc.DrawLine( x_pos, y_pos + 4, x_pos - 1, y_pos + Hex::SIZE_Y - 4 );
					if ( hex_[y][x].getWaterwayRoute( Hex::SOUTHWEST ) )
						dc.DrawLine( x_pos, y_pos + Hex::SIZE_Y - 4, x_pos + Hex::HALF - 1, y_pos + Hex::SIZE_Y + 3 );
					if ( hex_[y][x].getWaterwayRoute( Hex::SOUTHEAST ) )
						dc.DrawLine( x_pos + Hex::HALF + 1, y_pos + Hex::SIZE_Y + 3, x_pos + Hex::SIZE_X - 1, y_pos + Hex::SIZE_Y - 4 );
				}
			}
		}

		// inland port access route
		dc.SetPen( *hpenPortRoute );
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].hasInlandShippingChannels( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( hex_[y][x].getInlandShippingChannel( Hex::WEST ) )
						dc.DrawLine( x_pos, y_pos + 4, x_pos, y_pos + Hex::SIZE_Y - 4 );
					if ( hex_[y][x].getInlandShippingChannel( Hex::SOUTHWEST ) )
						dc.DrawLine( x_pos + 2, y_pos + Hex::SIZE_Y - 4, x_pos + Hex::HALF - 1, y_pos + Hex::SIZE_Y + 3 );
					if ( hex_[y][x].getInlandShippingChannel( Hex::SOUTHEAST ) )
						dc.DrawLine( x_pos + Hex::HALF + 2, y_pos + Hex::SIZE_Y + 3, x_pos + Hex::SIZE_X - 1, y_pos + Hex::SIZE_Y - 4 );
				}
			}
		}

		// bars to waterway movement
		dc.SetPen( *hpenBar );
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].hasInlandRouteBars( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( hex_[y][x].getInlandRouteBar( Hex::WEST ) )
					{
						dc.DrawLine( x_pos - 3, y_pos + 8, x_pos + 2, y_pos + 8 );
						dc.DrawLine( x_pos - 4, y_pos + 23, x_pos + 1, y_pos + 23 );
					}
					if ( hex_[y][x].getInlandRouteBar( Hex::SOUTHWEST ) )
					{
						dc.DrawLine( x_pos + 2, y_pos + Hex::SIZE_Y - 1, x_pos + 4, y_pos + Hex::SIZE_Y - 5 );
						dc.DrawLine( x_pos + 12, y_pos + Hex::SIZE_Y + 4, x_pos + 14, y_pos + Hex::SIZE_Y );
					}
					if ( hex_[y][x].getInlandRouteBar( Hex::SOUTHEAST ) )
					{
						dc.DrawLine( x_pos + 18, y_pos + Hex::SIZE_Y, x_pos + 20, y_pos + Hex::SIZE_Y + 4 );
						dc.DrawLine( x_pos + 28, y_pos + Hex::SIZE_Y - 5, x_pos + 30, y_pos + Hex::SIZE_Y - 1 );
					}
				}
			}
		}
		dc.SetBrush( wxNullBrush );

		// TODO: consider omitting routes when zoom Scale >= 6
		// draw routes
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				// x*Hex::SIZE_X + Hex::HALF on odd-numbered hexrows

				// if there is anything to render
				if ( hex_[y][x].hasAnyRoutes( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					int xxx, yyy; // used to check adjacent hexes

					// Hex::SOUTHEAST
					if ( hex_[y][x].getShippingChannel( Hex::SOUTHEAST ) )
					{
						dc.SetPen( *hpenShippingChannel );
						dc.DrawLine( x_pos + Hex::HALF + 1, y_pos + Hex::HALF, x_pos + Hex::SIZE_X, y_pos + ( Hex::SIZE_Y + Hex::HALF ) - 1 );
						dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF + 1, x_pos + Hex::SIZE_X - 1, y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
					}
					if ( hex_[y][x].getNarrowStrait( Hex::SOUTHEAST )  )
					{
						dc.SetPen( *wxBLACK_PEN );
						dc.DrawLine( x_pos + 23, y_pos + 25, x_pos + 29, y_pos + 34 );
						dc.DrawLine( x_pos + 19, y_pos + 28, x_pos + 25, y_pos + 37 );
						if ( hex_[y][x].getRoad( Hex::SOUTHEAST ) ) // quicker way to check for nrw. str. ferry
						{
							dc.SetPen( *hpenRoad );
							dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos + Hex::SIZE_X - Hex::HALF / 2 - 2, y_pos + Hex::SIZE_Y - 2 );
							dc.DrawLine( x_pos + Hex::SIZE_X - Hex::HALF / 2 + 2, y_pos + Hex::SIZE_Y + 2, x_pos + Hex::SIZE_X, y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
						}
					}
					else
					{
#ifdef HEXTOOLSMAP
						// TODO: deprecating; empty call here in order to allow
						// the gettor to perform the version upgrade (upgrades
						// low-vol to high-vol rails)
						hex_[y][x].getLowVolumeRailRoad( Hex::SOUTHEAST );
#endif
						wxPen* routePen = hpenRoad; // initialize to something

						bool draw = false;
						if ( hex_[y][x].getRailTrack( Hex::SOUTHEAST ) )  // track & rail
							routePen = hpenRailTrack, draw = true;
						else if ( hex_[y][x].getMotorTrack( Hex::SOUTHEAST ) )  // track & road
							routePen = hpenMotorTrack, draw = true;
						else if ( hex_[y][x].getRailRoad( Hex::SOUTHEAST ) )
							routePen = hpenRailRoad, draw = true;
						else if ( hex_[y][x].getRoad( Hex::SOUTHEAST ) )
							routePen = hpenRoad, draw = true;
						else if ( hex_[y][x].getTrack( Hex::SOUTHEAST ) )
							routePen = hpenTrack, draw = true;
						else if ( hex_[y][x].getRailFerry( Hex::SOUTHEAST ) )
							routePen = hpenRailFerry, draw = true;

						if ( draw )
						{
							if ( hex_[y][x].city_ != nullptr && ( hex_[y][x].city_->type_ == City::Type::PARTIALHEX || hex_[y][x].city_->type_ == City::Type::FULLHEX ) )
							{
								dc.SetPen( *hpenPriCityBoulevard );
								dc.DrawLine( x_pos + Hex::HALF + 1, y_pos + Hex::HALF + 1, x_pos + 3 * Hex::HALF / 2 - 1, y_pos + Hex::SIZE_Y - 1 );
							}

							if ( ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xxx, &yyy, world_x_size_, world_y_size_ ) && hex_[yyy][xxx].city_ != nullptr && ( hex_[yyy][xxx].city_->type_ == City::Type::PARTIALHEX || hex_[yyy][xxx].city_->type_ == City::Type::FULLHEX ) )
							{
								dc.SetPen( *hpenPriCityBoulevard );
								dc.DrawLine( x_pos + 3 * Hex::HALF / 2 + 1, y_pos + Hex::SIZE_Y + 1, x_pos + Hex::SIZE_X - 1, y_pos + ( Hex::SIZE_Y + Hex::HALF ) - 1 );
							}

							dc.SetPen( *routePen );
							dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos + Hex::SIZE_X, y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
						}
					}

					// Hex::SOUTHWEST
					if ( hex_[y][x].getShippingChannel( Hex::SOUTHWEST ) )
					{
						dc.SetPen( *hpenShippingChannel );
						dc.DrawLine( x_pos + Hex::HALF - 1, y_pos + Hex::HALF, x_pos, y_pos + ( Hex::SIZE_Y + Hex::HALF ) - 1 );
						dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF + 1, x_pos + 1 , y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
					}
					if ( hex_[y][x].getNarrowStrait( Hex::SOUTHWEST ) )
					{
						dc.SetPen( *wxBLACK_PEN );
						dc.DrawLine( x_pos + 9, y_pos + 25, x_pos + 3, y_pos + 34 );
						dc.DrawLine( x_pos + 13, y_pos + 28, x_pos + 7, y_pos + 37 );
						if ( hex_[y][x].getRoad( Hex::SOUTHWEST ) ) // quicker way to check for nrw. str. ferry
						{
							dc.SetPen( *hpenRoad );
							dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos + Hex::HALF / 2 + 2, y_pos + Hex::SIZE_Y - 2 );
							dc.DrawLine( x_pos + Hex::HALF / 2 - 2, y_pos + Hex::SIZE_Y + 2, x_pos, y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
						}
					}
					else
					{
#ifdef HEXTOOLSMAP
						// TODO: deprecating; empty call here in order to allow
						// the gettor to perform the version upgrade (upgrades
						// low-vol to high-vol rails)
						hex_[y][x].getLowVolumeRailRoad( Hex::SOUTHWEST );
#endif
						wxPen* routePen = hpenRoad; // initalize to something

						bool draw = false;
						if ( hex_[y][x].getRailTrack( Hex::SOUTHWEST ) )  // track & rail
							routePen = hpenRailTrack, draw = true;
						else if ( hex_[y][x].getMotorTrack( Hex::SOUTHWEST ) )  // track & road
							routePen = hpenMotorTrack, draw = true;
						else if ( hex_[y][x].getRailRoad( Hex::SOUTHWEST ) )
							routePen = hpenRailRoad, draw = true;
						else if ( hex_[y][x].getRoad( Hex::SOUTHWEST ) )
							routePen = hpenRoad, draw = true;
						else if ( hex_[y][x].getTrack( Hex::SOUTHWEST ) )
							routePen = hpenTrack, draw = true;
						else if ( hex_[y][x].getRailFerry( Hex::SOUTHWEST ) )
							routePen = hpenRailFerry, draw = true;

						if ( draw )
						{
							if ( hex_[y][x].city_ != nullptr && ( hex_[y][x].city_->type_ == City::Type::PARTIALHEX || hex_[y][x].city_->type_ == City::Type::FULLHEX ) )
							{
								dc.SetPen( *hpenPriCityBoulevard );
								dc.DrawLine( x_pos + Hex::HALF - 1, y_pos + Hex::HALF + 1, x_pos + Hex::HALF / 2 + 1, y_pos + Hex::SIZE_Y - 1);
							}

							if ( ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &xxx, &yyy, world_x_size_, world_y_size_ ) && hex_[yyy][xxx].city_ != nullptr && ( hex_[yyy][xxx].city_->type_ == City::Type::PARTIALHEX || hex_[yyy][xxx].city_->type_ == City::Type::FULLHEX ) )
							{
								dc.SetPen( *hpenPriCityBoulevard );
								dc.DrawLine( x_pos + Hex::HALF / 2 - 1, y_pos + Hex::SIZE_Y + 1, x_pos + 1, y_pos + ( Hex::SIZE_Y + Hex::HALF ) - 1);
							}

							dc.SetPen( *routePen );
							dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos, y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
						}
					}

					// Hex::WEST
					if ( hex_[y][x].getShippingChannel( Hex::WEST ) )
					{
						dc.SetPen( *hpenShippingChannel );
						dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF - 1, x_pos - Hex::HALF, y_pos + Hex::HALF - 1 );
						dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF + 1, x_pos - Hex::HALF, y_pos + Hex::HALF + 1 );
					}
					else if ( hex_[y][x].getNarrowStrait( Hex::WEST ) )
					{
						dc.SetPen( *wxBLACK_PEN );
						dc.DrawLine( x_pos + 4, y_pos + 13, x_pos - 5, y_pos + 13 );
						dc.DrawLine( x_pos + 4, y_pos + 19, x_pos - 5, y_pos + 19 );
						if ( hex_[y][x].getRoad( Hex::WEST ) ) // quicker way to check for nrw. str. ferry
						{
							dc.SetPen( *hpenRoad );
							dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos + 4, y_pos + Hex::HALF);
							dc.DrawLine( x_pos - 4, y_pos + Hex::HALF, x_pos - Hex::HALF, y_pos + Hex::HALF);
						}
					}
					else
					{
#ifdef HEXTOOLSMAP
						// TODO: deprecating; empty call here in order to allow
						// the gettor to perform the version upgrade (upgrades
						// low-vol to high-vol rails)
						hex_[y][x].getLowVolumeRailRoad( Hex::WEST );
#endif
						wxPen* routePen = hpenRoad; // initalize to something

						bool draw = false;
						if ( hex_[y][x].getRailTrack( Hex::WEST ) )  // track & rail
							routePen = hpenRailTrack, draw = true;
						else if ( hex_[y][x].getMotorTrack( Hex::WEST ) )  // track & road
							routePen = hpenMotorTrack, draw = true;
						else if ( hex_[y][x].getRailRoad( Hex::WEST ) )
							routePen = hpenRailRoad, draw = true;
						else if ( hex_[y][x].getRoad( Hex::WEST ) )
							routePen = hpenRoad, draw = true;
						else if ( hex_[y][x].getTrack( Hex::WEST ) )
							routePen = hpenTrack, draw = true;
						else if ( hex_[y][x].getRailFerry( Hex::WEST ) )
							routePen = hpenRailFerry, draw = true;

						if ( draw )
						{
							if ( hex_[y][x].city_ != nullptr && ( hex_[y][x].city_->type_ == City::Type::PARTIALHEX || hex_[y][x].city_->type_ == City::Type::FULLHEX ) )
							{
								dc.SetPen( *hpenPriCityBoulevard );
								dc.DrawLine( x_pos + Hex::HALF - 1, y_pos + Hex::HALF, x_pos + 1, y_pos + Hex::HALF );
							}

							if ( ht::getAdjHexCoords( Hex::WEST, x, y, &xxx, &yyy, world_x_size_, world_y_size_ ) && hex_[yyy][xxx].city_ != nullptr && ( hex_[yyy][xxx].city_->type_ == City::Type::PARTIALHEX || hex_[yyy][xxx].city_->type_ == City::Type::FULLHEX ) )
							{
								dc.SetPen( *hpenPriCityBoulevard );
								dc.DrawLine( x_pos - 1, y_pos + Hex::HALF, x_pos - Hex::HALF + 1, y_pos + Hex::HALF );
							}

							dc.SetPen( *routePen );
							dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos - Hex::HALF, y_pos + Hex::HALF );
						}
					}
				}
			}
		}

		// draw standalone bridges
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].hasStandaloneBridge( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( hex_[y][x].getStandaloneBridge( Hex::SOUTHEAST ) )
					{
						hdcMem.SelectObject( *hbmSEStandaloneBridge );
						dc.Blit( x_pos + 19, y_pos + 28, 10, 9, &hdcMem, 0, 0, wxAND );
					}
					if ( hex_[y][x].getStandaloneBridge( Hex::SOUTHWEST ) )
					{
						hdcMem.SelectObject( *hbmSWStandaloneBridge );
						dc.Blit( x_pos + 3, y_pos + 28, 10, 9, &hdcMem, 0, 0, wxAND );
					}
					if ( hex_[y][x].getStandaloneBridge( Hex::WEST ) )
					{
						hdcMem.SelectObject( *hbmWStandaloneBridge );
						dc.Blit( x_pos - 4, y_pos + 12, 8, 7, &hdcMem, 0, 0, wxAND );
					}
				}
			}
		}

#ifdef HEXTOOLSMAP
		// draw lake/sea ferry actual routes (only in -map, won't display this in -play)
		dc.SetPen( *hpenLakeSeaFerryOnIce );
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;
			for ( x = min_xx; x < max_xx; ++x )
			{
				x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
				if ( hex_[y][x].getLakeSeaFerry( Hex::SOUTHEAST ) )
					dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos + Hex::SIZE_X, y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
				if ( hex_[y][x].getLakeSeaFerry( Hex::SOUTHWEST ) )
					dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos, y_pos + ( Hex::SIZE_Y + Hex::HALF ) );
				if ( hex_[y][x].getLakeSeaFerry( Hex::WEST ) )
					dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::HALF, x_pos - Hex::HALF, y_pos + Hex::HALF );
			}
		}
#endif
		// draw lake/sea ferries (notional route, not the actual hexside pathing)
		dc.SetPen( *hpenLakeSeaFerry );
		for ( y = max_yy; y >= min_yy; --y )
		{
			for ( x = min_xx; x < max_xx; ++x )
			{
				// don't walk a lake/sea ferry route if no terminal
				//	or if it has a terminal but not a leaf node
				if ( ! hex_[y][x].hasFerryTerminal( ) || get_lake_sea_ferry_count( x, y ) != 1 )
					continue;

				int y1 = y;
				int x1 = x;
				int y1_pos = y1 << 5;
				int x1_pos = ( x1 << 5 ) + ( ( y1 & 1 ) << 4 );

				int dir;
				int fromDir;
				int y2, x2;
				bool found = false;

				// in-hex dirs, Hex::SOUTHEAST, Hex::SOUTHWEST, Hex::WEST
				for ( dir = Hex::SOUTHEAST; dir <= Hex::WEST; dir <<= 1 )
					if (	hex_[y1][x1].getLakeSeaFerry( dir ) &&
							ht::getAdjHexCoords( dir, x1, y1, &x2, &y2, world_x_size_, world_y_size_ ) )
					{
						found = true;
						break;
					}

				// adjacent-hex dirs, Hex::NORTHWEST, Hex::NORTHEAST, Hex::EAST
				if ( ! found )
				{
					for ( dir = Hex::NORTHWEST; dir <= Hex::EAST; dir <<= 1 )
						if (	ht::getAdjHexCoords( dir, x1, y1, &x2, &y2, world_x_size_, world_y_size_ ) &&
								hex_[y2][x2].getLakeSeaFerry( ht::getAdjHexPart( dir ) ) )
						{
							found = true;
							break;
						}
				}

				// sanity check lakeSeaFerryTerminal mechanism in case it fails
				//	and there isn't actually a ferry terminal in the hex
				if ( ! found )
					continue;

				// in ferry route walking checks, exclude the dir from which the ferry
				//	entered the hex being checked
				fromDir = ht::getAdjHexPart( dir );

				// walk ferry hexsides to the next terminal
				while ( ! hex_[y2][x2].hasFerryTerminal( ) )
				{
					found = false;
					y1 = y2;
					x1 = x2;

					// 8, 16, 32
					for ( dir = Hex::SOUTHEAST; dir <= Hex::WEST; dir <<= 1 )
						if (	dir != fromDir &&
								hex_[y1][x1].getLakeSeaFerry( dir ) &&
								ht::getAdjHexCoords( dir, x1, y1, &x2, &y2, world_x_size_, world_y_size_ ) )
						{
							found = true;
							break;
						}

					// adjacent-hex dirs, Hex::NORTHWEST, Hex::NORTHEAST, Hex::EAST
					if ( ! found )
					{
						// 1, 2, 4
						for ( dir = Hex::NORTHWEST; dir <= Hex::EAST; dir <<= 1 )
							if (	dir != fromDir &&
									ht::getAdjHexCoords( dir, x1, y1, &x2, &y2, world_x_size_, world_y_size_ ) &&
									hex_[y2][x2].getLakeSeaFerry( ht::getAdjHexPart( dir ) ) )
							{
								found = true;
								break;
							}
					}

					if ( ! found )
						break;

					fromDir = ht::getAdjHexPart( dir );
				}

				// when a ferry route does not terminate at a ferry terminal
				if ( ! found )
					continue;

				// this relies upon the bottom to top, left to right rendering order
				//	and keeps a simple terminal-to-terminal ferry route from double rendering
				// exception:  ferry count check allows branch ferries to be north of a central ferry,
				//	which would otherwise fail to render due to the coordinate checks
				if (	( y2 > y || ( y2 == y && x2 < x ) ) &&
						get_lake_sea_ferry_count( x2, y2 ) == 1 )
					continue;

				int y2_pos = y2 << 5;
				int x2_pos = ( x2 << 5 ) + ( ( y2 & 1 ) << 4 );

				// locate ferry terminal 1

				City* cptr = hex_[y][x].city_; // note: y,x not y1,x1
				while ( cptr != nullptr && cptr->type_ != City::Type::FERRY_TERMINAL )
					cptr = cptr->getNext( );

				int ferry_x1, ferry_y1;
				ht::hexOtherCityTypePositionOffset( cptr->position_, &ferry_x1, &ferry_y1 );

				ferry_x1 += 15;		ferry_y1 += 15;

				// locate ferry terminal 2
				cptr = hex_[y2][x2].city_;
				while ( cptr != nullptr && cptr->type_ != City::Type::FERRY_TERMINAL )
					cptr = cptr->getNext( );

				int ferry_x2, ferry_y2;
				ht::hexOtherCityTypePositionOffset( cptr->position_, &ferry_x2, &ferry_y2 );

				ferry_x2 += 15;		ferry_y2 += 15;

				dc.DrawLine( x1_pos + ferry_x1, y1_pos + ferry_y1, x2_pos + ferry_x2, y2_pos + ferry_y2 );
			}
		}

		// fortified hexside must be drawn in the separate sweep, otherwise
		// they might get covered by mountain hexside etc.
		// ww2pac great wall hexsides are drawn here
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].getInteriorHexsidesFlag( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					// cannot render both fortified and great walls in same hex
					//	there is only one bit to register that status per hex
					if ( hex_[y][x].hasFortifiedSides( ) )
					{
						if ( hex_[y][x].getFortifiedSide( Hex::NORTHWEST ) )
						{
							hdcMem.SelectObject( *hbmNWFort );
							dc.Blit( x_pos + 5, y_pos + 2, 12, 8, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getFortifiedSide( Hex::NORTHEAST ) )
						{
							hdcMem.SelectObject( *hbmNEFort );
							dc.Blit( x_pos + 16, y_pos + 2, 12, 8, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getFortifiedSide( Hex::EAST ) )
						{
							hdcMem.SelectObject( *hbmEFort );
							dc.Blit( x_pos + 26, y_pos + 10, 4, 12, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getFortifiedSide( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEFort );
							dc.Blit( x_pos + 16, y_pos + 22, 12, 8, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getFortifiedSide( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWFort );
							dc.Blit( x_pos + 5, y_pos + 22, 12, 8, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getFortifiedSide( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWFort );
							dc.Blit( x_pos + 3, y_pos + 10, 4, 12, &hdcMem, 0, 0, wxAND );
						}
					}
					else if ( hex_[y][x].hasGreatWalls( ) )  // note: *either* fortified *or* great walls
					{
						if ( hex_[y][x].getGreatWall( Hex::NORTHWEST ) )
						{
							hdcMem.SelectObject( *hbmNWGWall );
							dc.Blit( x_pos + 3, y_pos + 1, 14, 10, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getGreatWall( Hex::NORTHEAST ) )
						{
							hdcMem.SelectObject( *hbmNEGWall );
							dc.Blit( x_pos + 15, y_pos + 1, 14, 10, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getGreatWall( Hex::EAST ) )
						{
							hdcMem.SelectObject( *hbmEGWall );
							dc.Blit( x_pos + 26, y_pos + 11, 4, 15, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getGreatWall( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEGWall );
							dc.Blit( x_pos + 15, y_pos + 19, 14, 14, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getGreatWall( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWGWall );
							dc.Blit( x_pos + 3, y_pos + 19, 14, 14, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getGreatWall( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWGWall );
							dc.Blit( x_pos + 2, y_pos + 11, 4, 15, &hdcMem, 0, 0, wxAND );
						}
					}
					if ( hex_[y][x].hasEscarpments( ) )  // below, check to make sure not imp. esc.
					{
						if ( hex_[y][x].getEscarpment( Hex::NORTHWEST ) && !hex_[y][x].getImpassableEscarpment( Hex::NORTHWEST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 5, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmNWEsc );
							dc.Blit( x_pos + 5, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 5, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getEscarpment( Hex::NORTHEAST ) && !hex_[y][x].getImpassableEscarpment( Hex::NORTHEAST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 16, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmNEEsc );
							dc.Blit( x_pos + 16, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 16, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getEscarpment( Hex::EAST ) && !hex_[y][x].getImpassableEscarpment( Hex::EAST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 27, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmEEsc );
							dc.Blit( x_pos + 27, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 27, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getEscarpment( Hex::SOUTHEAST ) && !hex_[y][x].getImpassableEscarpment( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 16, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSEEsc );
							dc.Blit( x_pos + 16, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 16, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getEscarpment( Hex::SOUTHWEST ) && !hex_[y][x].getImpassableEscarpment( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 5, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmSWEsc );
							dc.Blit( x_pos + 5, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 5, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxXOR );
						}
						if ( hex_[y][x].getEscarpment( Hex::WEST ) && !hex_[y][x].getImpassableEscarpment( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 2, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxXOR );
							hdcMem.SelectObject( *hbmWEsc );
							dc.Blit( x_pos + 2, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmEscColor );
							dc.Blit( x_pos + 2, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxXOR );
						}
					}
					if ( hex_[y][x].hasImpassableEscarpments( ) )  // below check to make sure not esc.
					{
						if ( hex_[y][x].getImpassableEscarpment( Hex::NORTHWEST ) && !hex_[y][x].getEscarpment( Hex::NORTHWEST ) )
						{
							hdcMem.SelectObject( *hbmNWEsc );
							dc.Blit( x_pos + 5, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getImpassableEscarpment( Hex::NORTHEAST ) && !hex_[y][x].getEscarpment( Hex::NORTHEAST ) )
						{
							hdcMem.SelectObject( *hbmNEEsc );
							dc.Blit( x_pos + 16, y_pos - 1, 12, 11, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getImpassableEscarpment( Hex::EAST ) && !hex_[y][x].getEscarpment( Hex::EAST ) )
						{
							hdcMem.SelectObject( *hbmEEsc );
							dc.Blit( x_pos + 27, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getImpassableEscarpment( Hex::SOUTHEAST ) && !hex_[y][x].getEscarpment( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEEsc );
							dc.Blit( x_pos + 16, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getImpassableEscarpment( Hex::SOUTHWEST ) && !hex_[y][x].getEscarpment( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWEsc );
							dc.Blit( x_pos + 5, y_pos + 23, 12, 11, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getImpassableEscarpment( Hex::WEST ) && ! hex_[y][x].getEscarpment( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWEsc );
							dc.Blit( x_pos + 2, y_pos + 9, 4, 16, &hdcMem, 0, 0, wxAND );
						}
					}
					if ( hex_[y][x].hasMountainInterior( ) )
					{
						if ( hex_[y][x].getMountainInterior( Hex::NORTHWEST ) )
						{
							hdcMem.SelectObject( *hbmNWMtnInteriorMask );
							dc.Blit( x_pos + 3, y_pos, 16, 12, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmNWMtnInterior );
							dc.Blit( x_pos + 3, y_pos, 16, 12, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getMountainInterior( Hex::NORTHEAST ) )
						{
							hdcMem.SelectObject( *hbmNEMtnInteriorMask );
							dc.Blit( x_pos + 13, y_pos, 16, 12, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmNEMtnInterior );
							dc.Blit( x_pos + 13, y_pos, 16, 12, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getMountainInterior( Hex::EAST ) )
						{
							hdcMem.SelectObject( *hbmEMtnInteriorMask );
							dc.Blit( x_pos + 23, y_pos + 5, 7, 22, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmEMtnInterior );
							dc.Blit( x_pos + 23, y_pos + 5, 7, 22, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getMountainInterior( Hex::SOUTHEAST ) )
						{
							hdcMem.SelectObject( *hbmSEMtnInteriorMask );
							dc.Blit( x_pos + 14, y_pos + 21, 16, 12, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSEMtnInterior );
							dc.Blit( x_pos + 14, y_pos + 21, 16, 12, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getMountainInterior( Hex::SOUTHWEST ) )
						{
							hdcMem.SelectObject( *hbmSWMtnInteriorMask );
							dc.Blit( x_pos + 3, y_pos + 21, 16, 12, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSWMtnInterior );
							dc.Blit( x_pos + 3, y_pos + 21, 16, 12, &hdcMem, 0, 0, wxAND );
						}
						if ( hex_[y][x].getMountainInterior( Hex::WEST ) )
						{
							hdcMem.SelectObject( *hbmWMtnInteriorMask );
							dc.Blit( x_pos + 2, y_pos + 5, 7, 22, &hdcMem, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmWMtnInterior );
							dc.Blit( x_pos + 2, y_pos + 5, 7, 22, &hdcMem, 0, 0, wxAND );
						}
					}
				}
			}
		}

		dc.SetTextBackground( wxColor( 255, 255, 255 ) );
		dc.SetTextForeground( wxColor( 0, 0, 0 ) );

		// cities
		hdcMem2.SelectObject( *hbmMinorCity ); // reference
		hdcMem3.SelectObject( *hbmMediumCity ); // dot/medium
		hdcMem4.SelectObject( *hbmResCtrMask );
		for ( y = max_y; y >= min_y; --y )
		{
			y_pos = y << 5;  // y << 5 == y * 32 == y * Hex::SIZE_Y
			for ( x = min_x; x < max_x; ++x )
			{
				bool primaryCity = false;

				City* cptr = hex_[y][x].city_;
				while ( cptr != nullptr )  // there is another city in the hex
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					ht::hexOtherCityTypePositionOffset( cptr->position_, &xo, &yo );
					// coming out of this switch, xo/yo are direction-specific
					//	settings referenced during blitting of symbols

					bool fortressMask = false;
					// list of underterrain over which
					//	fortresses must first be background masked:
					switch ( hex_[y][x].terrain_ )
					{
						case HexType::MOUNTAIN:
						case HexType::WOODEDROUGH:
						case HexType::FOREST:
						case HexType::WOODS:
						case HexType::INTIRRIGATION:
						case HexType::SEA:
						case HexType::ICINGSEA:
							fortressMask = true;
							break;
					}

					bool resCtr = false;
					bool fortification = false;
					bool secondaryCity = false;
					bool noSymbol = false;  // used by BIG_TEXT (Large Map Text) and MAPTEXT (default Small Map Text)
					bool virtualPrimaryCity = false;  // used only by BIG_TEXT city type for major-city-like alignment

					wxFont thisFont = small_font;

					// NOTE: +15,+15 is the UL corner of the 2x2 hex center
					int y_sym = 15 + yo;
					int x_sym = 15 + xo;

					// blit symbols and set font and symbol for text measurement
					switch ( cptr->type_ )
					{
						case City::Type::PT_CITY: // town
							y_sym -= 1;
							x_sym -= 1;
							hdcMem.SelectObject( *hbmPtCity );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 4, 4, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::MINOR: // reference/minor city
							secondaryCity = true;
							y_sym -= 5;
							x_sym -= 5;
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem3, 0, 0, wxAND );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem3, 0, 0, wxOR_INVERT );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem2, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmMinorCity ); // just for measurement
							thisFont = font;
							break;
						case City::Type::MAJOR: // dot/medium city
							secondaryCity = true;
							y_sym -= 5;
							x_sym -= 5;
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem3, 0, 0, wxAND );
							hdcMem.SelectObject( *hbmMediumCity ); // just for measurement
							thisFont = font;
							break;
						case City::Type::PARTIALHEX: // major city
							primaryCity = true; // primary cities get laid down w/ hex terrain 25x26
							y_sym = 2 + city_yo;
							x_sym = 2 + city_xo;
							hdcMem.SelectObject( *hbmPartialHexCity ); // selected for text measurement
							thisFont = big_font;
							break;
						case City::Type::FULLHEX: // great city
							primaryCity = true; // primary cities get laid down w/ hex terrain 32x38
							y_sym = -3;
							x_sym = 0;
							hdcMem.SelectObject( *hbmFullHexCity ); // selected for text measurement
							thisFont = big_font;
							break;
						case City::Type::ATOLL_CUSTOM:
						case City::Type::SMALL_ISLAND_CUSTOM:
							// TODO: vet relative location of text for custom islands
							y_sym -= 5;
							x_sym -= 5;
							hdcMem.SelectObject( *hbmPartialHexCity ); // just for measurement
							break;
						case City::Type::REF_PT:
							hdcMem.SelectObject( *hbmRefPt );
							y_sym -= 3;
							x_sym -= 2;
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 7, 7, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::FORTRESS:
						case City::Type::WW1_NEW_FORTRESS:
							fortification = true;
							y_sym -= 7;
							x_sym -= 7;
							if ( primaryCity || fortressMask )
							{
								hdcMem4.SelectObject( *hbmFortMask );
								dc.Blit( x_pos + x_sym - 1, y_pos + y_sym - 1, 18, 18, &hdcMem4, 0, 0, wxOR );
							}
							hdcMem.SelectObject( *hbmFortress );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 16, 16, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::WW1_OLD_FORTRESS:
							fortification = true;
							y_sym -= 7;
							x_sym -= 7;
							if ( primaryCity || fortressMask )
							{
								hdcMem4.SelectObject( *hbmFortMask );
								dc.Blit( x_pos + x_sym - 1, y_pos + y_sym - 1, 18, 18, &hdcMem4, 0, 0, wxOR );
							}
							hdcMem.SelectObject( *hbmWW1OFort );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 16, 16, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::WW1_GREAT_FORTRESS:
							fortification = true;
							y_sym -= 7;
							x_sym -= 7;
							if ( primaryCity || fortressMask )
							{
								hdcMem4.SelectObject( *hbmFortMask );
								dc.Blit( x_pos + x_sym - 1, y_pos + y_sym - 1, 18, 18, &hdcMem4, 0, 0, wxOR );
							}
							hdcMem.SelectObject( *hbmWW1GFort );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 16, 16, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::WESTWALL_1:
						case City::Type::WESTWALL_2:
						case City::Type::WESTWALL_3:
							fortification = true;
							y_sym -= 7;
							x_sym -= 7;
							hdcMem4.SelectObject( *hbmFortMask );
							dc.Blit( x_pos + x_sym - 1, y_pos + y_sym - 1, 18, 18, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmWestWall );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 16, 16, &hdcMem, 0, 0, wxAND );

							dc.SetFont( small_font );  // 6-pt font numeral
							dc.SetTextForeground( *wxBLACK );
							// zoom level seems to affect vertical alignment:
							char level[1];
							itoa( cptr->type_ - 7 , level, 10 ); // convert city index to numeral
							dc.DrawText( level, x_pos + x_sym + 6, y_pos + y_sym + 3 );

							break;
						case City::Type::OUVRAGE:
							fortification = true;
							y_sym -= 7;
							x_sym -= 7;
							if ( primaryCity || fortressMask )
							{
								hdcMem4.SelectObject( *hbmFortMask );
								dc.Blit( x_pos + x_sym - 1, y_pos + y_sym - 1, 18, 18, &hdcMem4, 0, 0, wxOR );
							}
							hdcMem.SelectObject( *hbmOuvrage );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 16, 16, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::VOLKSDEUTSCH:
							y_sym -= 2;
							x_sym -= 2;
			                hdcMem.SelectObject( *hbmVolksDeutsch );
			                dc.Blit( x_pos + x_sym, y_pos + y_sym, 6, 6, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::OASIS:
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmOasisMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmOasis );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::SHIPYARD:
							y_sym -= 5;
							x_sym -= 5;
							hdcMem.SelectObject( *hbmShipyard );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, ( primaryCity ? wxCOPY : wxAND ) );
							break;
						case City::Type::RAILYARD:
							y_sym -= 5;
							x_sym -= 5;
							hdcMem.SelectObject( *hbmRMY3 );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 7, &hdcMem, 0, 0, ( primaryCity ? wxCOPY : wxAND ) );
							break;
						case City::Type::WATERWAY_DOCK:
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmWMDMask );
							dc.Blit( x_pos + x_sym - 1, y_pos + y_sym - 1, 13, 7, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmWMD3 );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 5, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::FERRY_TERMINAL:
						    dc.SetPen( wxPen( wxColor( 255, 255, 255 ), 1 ) );
						    dc.DrawRectangle( x_pos + x_sym - 1, y_pos + y_sym - 1, 3, 3 );
							break;
						case City::Type::MAJOR_NAVAL_BASE:
							y_sym -= 5;
							x_sym -= 5;
							if ( primaryCity )
							{
								hdcMem4.SelectObject( *hbmAirNavalBaseMask );
								dc.Blit( x_pos + x_sym, y_pos + y_sym, 9, 9, &hdcMem4, 0, 0, wxOR );
							}
							hdcMem.SelectObject( *hbmMajorNavalBase );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 9, 9, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::MINOR_NAVAL_BASE:
							y_sym -= 5;
							x_sym -= 5;
							if ( primaryCity )
							{
								hdcMem4.SelectObject( *hbmAirNavalBaseMask );
								dc.Blit( x_pos + x_sym, y_pos + y_sym, 9, 9, &hdcMem4, 0, 0, wxOR );
							}
							hdcMem.SelectObject( *hbmMinorNavalBase );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 9, 9, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::HYDRO_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmHydroRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::ORE_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmOreRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::MINERAL_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmMineralRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::COAL_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmCoalRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::NATGAS_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmNaturalGasRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::OIL_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmOilRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::OILSHALE_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmOilShaleRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::SYNTHOIL_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSynthOilRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::PEAT_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmPeatRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::CEMENT_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmCementRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::RUBBER_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmRubberRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::SYNTHRUBBER_RESOURCE:
							resCtr = true;
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmResCtrMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmSynthRubberRes );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 12, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::BIG_TEXT:  // Map Text (Large)
							noSymbol = true;
							virtualPrimaryCity = true;
							ht::hexMajorCityPositionOffset( cptr->position_, &city_xo, &city_yo );
							y_sym = 2 + city_yo;
							x_sym = 2 + city_xo;
							hdcMem.SelectObject( *hbmPartialHexCity ); // so that text aligns as if a major (partial hex) city
							thisFont = big_font;
							break;
						case City::Type::TRIBALAREA:
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmTribalMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmTribalArea );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::BERBERAREA:
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmBerberMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmBerberArea );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::INDIANAREA:
							y_sym -= 5;
							x_sym -= 5;
							hdcMem4.SelectObject( *hbmIndianMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmIndianArea );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 12, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::CAMELAREA:
							y_sym -= 5;
							x_sym -= 6;
							hdcMem4.SelectObject( *hbmCamelMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 14, 11, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmCamelArea );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 14, 11, &hdcMem, 0, 0, wxAND );
							break;
						case City::Type::TSETSEAREA:
							y_sym -= 2;
							x_sym -= 4;
							hdcMem4.SelectObject( *hbmTsetseMask );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 6, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmTsetseArea );
							dc.Blit( x_pos + x_sym, y_pos + y_sym, 11, 6, &hdcMem, 0, 0, wxAND );
							break;
						default: // City::Type::MAPTEXT // Map Text (Small)
							noSymbol = true;
							y_sym -= 1;
							x_sym -= 1;
							hdcMem.SelectObject( *hbmPtCity ); // so that text aligns as if town
							break;
					}

					wxString text( cptr->name_ );

					// if the name is not the default ";n..." (the escape code for non-displayable)
					if ( text.length() > 0 && text.Left( 2 ) != ";n" )
					{
						dc.SetFont( thisFont );

						// to be used for measurement purposes
						wxString mText( cptr->name_ );
						mText = City::scrubCityNameStyleAndRotationEncodings( mText );

						// measure for text positioning offset
						int th, tw;
						dc.GetTextExtent( mText, &tw, &th );

						int sym_width, sym_height;
						hdcMem.GetSize( &sym_width, &sym_height );

						ty = sym_height / 2;
						tx = sym_width / 2;

						switch ( cptr->text_position_ )
						{
							case 7: // NW
								ty -= -2 + th * 7 / 4;
								tx -= -4 + tw;
								break;
							case 8: // N
								ty -= -2 + th * 7 / 4;
								tx -= tw / 2;
								break;
							case 9: // NE
								ty -= -2 + th * 7 / 4;
								tx -= 2;
								//tx -= noSymbol ? -16 + tw : 2;
								break;
							case 4: // W
								ty -= th / 2;
								// primaryCity special case to deal with wide city graphics
								tx -= tw + ( primaryCity || virtualPrimaryCity ? 16 : 8 );
								break;
							case 5: // C
								// special cases for tall symbols / short font combination
								ty -= th / ( noSymbol ? 2 : 1 ) + ( fortification ? 8 : 0 ) + ( secondaryCity ? 4 : 0 );
								tx -= tw / 2;
								break;
							case 6: // E
								ty -= th / 2;
								tx += primaryCity || virtualPrimaryCity ? 16 : 8;
								//tx += noSymbol ? ( primaryCity ? 8 : 16 ) - tw : ( primaryCity ? 16 : 8 );
								break;
							case 1: // SW
								ty += -2 + th * 3 / 4;
								tx -= -4 + tw;
								break;
							case 3: // SE
								ty += -2 + th * 3 / 4;
								tx -= 3;
								//tx -= noSymbol ? -16 + tw : 3;
								break;
							case 2: // S
							default: // default
								ty += -2 + th * 3 / 4;
								tx -= tw / 2;
								break;
						}

						if ( resCtr && primaryCity )
						{
							hdcMem.SelectObject( *hbmResCtrTextMask );
							dc.Blit( x_pos + x_sym + tx - 1, y_pos + y_sym + ty + 1, 9, 8, &hdcMem, 0, 0, wxOR );
						}

						text.Replace( wxString("_"), wxString(" "), true );

						if ( text.length( ) > 0 && text.GetChar(0) == ';' )
						{
							int pad = 1;
							if ( text.size() > 1 )
							{
								// capital-letter style codes mean white text
								//	all capital letters precede the '[' in ASCII order
								dc.SetTextForeground( text.GetChar(1) < '[' ? *wxWHITE : *wxBLACK );

								pad = 2;
								switch ( text.GetChar(1) )
								{
									case 'b': // bold
									case 'B': // and white
										thisFont.SetWeight( wxFONTWEIGHT_BOLD );
										break;
									case 'i': // italics
									case 'I': // and white
										thisFont.SetStyle( wxFONTSTYLE_ITALIC );
										break;
									case 'c': // bold and italics
									case 'C': // and white
										thisFont.SetWeight( wxFONTWEIGHT_BOLD );
										thisFont.SetStyle( wxFONTSTYLE_ITALIC );
										break;
									case 'x': // bold 16-pt
									case 'X': // and white
										thisFont = xl_font;
										thisFont.SetWeight( wxFONTWEIGHT_BOLD );
										break;
									case 'W': // white was already handled, no further action
									case 'n': // shouldn't be possible
										break;
									default:
										pad = 1;
										break;
								}
							}
							text = text.Mid( pad ); // don't render the style code
						}
						else
							dc.SetTextForeground( *wxBLACK );

						int angle = 0;
						double rads = 0;
						double angb = 0;
						double angc = 0;
						double lind = 0;
						if ( text.length() > 2 && text.GetChar( text.length() - 2 ) == ';' )
						{
							switch ( text.GetChar( text.length() - 1 ) )
							{
								case '2':
									angle = -27;
									rads = -0.47124; // -27° in radians
									ty = sym_height / 2;
									tx = sym_width / 2;
									switch ( cptr->text_position_ )
									{
										case 7: // NW
											ty -= 19;
											tx -=  1;
											break;
										case 8: // N
											ty -= 14 - lround( ( tw / 2 ) * sin( rads ) );
											tx -= -6 + lround( ( tw / 2 ) * cos( rads ) );
											break;
										case 9: // NE
											ty -= noSymbol ? 12 - lround( ( tw ) * sin( rads ) ) : 17;
											tx -= noSymbol ? -16 + lround( ( tw ) * cos( rads ) ) : -5;
											break;
										case 4: // W
											ty -= 18;
											tx -=  4;
											break;
										case 5: // C
											ty -= 5 - lround( ( tw / 2 ) * sin( rads ) );
											tx -= 1 + lround( ( tw / 2 ) * cos( rads ) );
											break;
											// 1.5708 rad = 90°
										case 6: // E
											ty -= noSymbol ? 20 - lround( th * sin( rads + 1.5708 ) ) - lround( ( tw ) * sin( rads ) ) : 10 - lround( th * sin( rads + 1.5708 ) );
											tx -= noSymbol ? -12 - lround( th * cos( rads + 1.5708 ) ) + lround( ( tw ) * cos( rads ) ) : 16 - lround( th * sin( rads + 1.5708 ) );
											break;
										case 1: // SW
											ty -=  7 - lround( th * sin( rads + 1.5708 ) );
											tx -= 17 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 3: // SE
											ty -= noSymbol ? -1 - lround( th * sin( rads + 1.5708 ) ) - lround( ( tw ) * sin( rads ) ) : 5 - lround( th * sin( rads + 1.5708 ) );
											tx -= noSymbol ? -2 - lround( th * cos( rads + 1.5708 ) ) + lround( ( tw ) * cos( rads ) ) : 13 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 2: // S
										default: // default
											ty -= -4 - lround( ( tw / 2 ) * sin( rads ) );
											tx -=  4 + lround( ( tw / 2 ) * cos( rads ) );
											break;
									}
									break;
								case '3':
									angle = 30;
									rads = 0.47124; // 27° in radians
									angb = atan( th / tw );  // angle between upper right corner and lower right corner of the text box,  in radians
									angc = angb + rads; // desired angle plus angle above
									lind = sqrt( pow( th, 2 ) + pow( tw, 2 ) );
									ty = sym_height / 2;
									tx = sym_width / 2;
									switch ( cptr->text_position_ )
									{
										case 7: // NW
											ty -= 13;
											tx -= 15;
											break;
										case 8: // N
											ty -= 17 - lround( ( lind / 2 ) * sin( angc ) );
											tx -=  7 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 9: // NE
											ty -= noSymbol ? 20 - lround( ( tw ) * sin( rads ) ) : 15;
											tx -= noSymbol ?  0 + lround( ( tw ) * cos( rads ) ) :  9;
											break;
										case 4: // W
											ty -= 10;
											tx -= 15;
											break;
										case 5: // C
											ty -= 4 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= 0 + lround( ( lind / 2 ) * cos( angc ) );
											break;
											// 1.5708 rad = 90°
										case 6: // E
											ty -= noSymbol ?   9 - lround( th * sin( rads + 1.5708 ) ) - lround( ( tw ) * sin( rads ) ) : 0 - lround( th * sin( rads + 1.5708 ) );
											tx -= noSymbol ? -14 - lround( th * cos( rads + 1.5708 ) ) + lround( ( tw ) * cos( rads ) ) : 2 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 1: // SW
											ty -= -3 - lround( th * sin( rads + 1.5708 ) );
											tx -=  2 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 3: // SE
											ty -= noSymbol ?   7 - lround( th * sin( rads + 1.5708 ) ) - lround( ( tw ) * sin( rads ) ) : -1 - lround( th * sin( rads + 1.5708 ) );
											tx -= noSymbol ? -14 - lround( th * cos( rads + 1.5708 ) ) + lround( ( tw ) * cos( rads ) ) : -2 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 2: // S
										default: // default
											ty -= -6 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -3 + lround( ( lind / 2 ) * cos( angc ) );
											break;
									}
									break;
								case '5':
									angle = -60; // 60 used instead of 54 to compensate for implied angle of C text
									rads = -0.9425; // 54° in radians
									angb = atan( th / tw );  // angle between upper right corner and lower right corner of the text box,  in radians
									angc = angb + rads; // desired angle plus angle above
									lind = sqrt( pow( th, 2 ) + pow( tw, 2 ) );
									ty = sym_height / 2;
									tx = sym_width / 2;
									switch ( cptr->text_position_ )
									{
										case 7: // NW
											ty -= 16;
											tx -= 6;
											break;
										case 8: // N
											ty -=  11 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -10 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 9: // NE
											ty -= 18;
											tx -= -1;
											break;
										case 4: // W
											ty -= 9;
											tx -= 6;
											break;
										case 5: // C
											ty -=  4 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -6 + lround( ( lind / 2 ) * cos( angc ) );
											break;
											// 1.5708 rad = 90°
										case 6: // E
											ty -= 21 - lround( th * sin( rads + 1.5708 ) );
											tx -=  3 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 1: // SW
											ty -= noSymbol ? -12 - lround( ( tw ) * sin( rads ) ) :  9 - lround( ( tw ) * sin( rads ) );
											tx -= noSymbol ? -10 + lround( ( tw ) * cos( rads ) ) : 12 - lround( ( tw ) * cos( rads ) );
											break;
										case 3: // SE
											ty -= noSymbol ?  -2 - lround( th * sin( rads + 1.5708 ) ) - lround( ( tw ) * sin( rads ) ) : 20 - lround( th * sin( rads + 1.5708 ) );
											tx -= noSymbol ? -12 - lround( th * cos( rads + 1.5708 ) ) + lround( ( tw ) * cos( rads ) ) : -2 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 2: // S
										default: // default
											ty -= noSymbol ?  -9 - lround( ( tw ) * sin( rads ) ) : -3 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= noSymbol ? -14 + lround( ( tw ) * cos( rads ) ) : -3 + lround( ( lind / 2 ) * cos( angc ) );
											break;
									}
									break;
								case '6':
									angle = 60; // 60 used instead of 54 to compensate for implied angle of C text
									rads = 0.9425; // 54° in radians
									angb = atan( th / tw );  // angle between upper right corner and lower right corner of the text box,  in radians
									angc = angb + rads; // desired angle plus angle above
									lind = sqrt( pow( th, 2 ) + pow( tw, 2 ) );
									ty = sym_height / 2;
									tx = sym_width / 2;
									switch ( cptr->text_position_ )
									{
										case 7: // NW
											ty -= noSymbol ? 16 - lround( ( tw ) * sin( rads ) ) : -1;
											tx -= noSymbol ?  3 + lround( ( tw ) * cos( rads ) ) : 15;
											break;
										case 8: // N
											ty -= noSymbol ? 15 - lround( ( tw ) * sin( rads ) ) : 8 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= noSymbol ? -3 + lround( ( tw ) * cos( rads ) ) : 5 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 9: // NE
											ty -= noSymbol ? 12 - lround( ( tw ) * sin( rads ) ) : -9;
											tx -= noSymbol ? -7 + lround( ( tw ) * cos( rads ) ) :  1;
											break;
										case 4: // W
											ty -= -4;
											tx -= 17;
											break;
										case 5: // C
											ty -= 4 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= 4 + lround( ( lind / 2 ) * cos( angc ) );
											break;
											// 1.5708 rad = 90°
										case 6: // E
											ty -= -7 - lround( th * sin( rads + 1.5708 ) );
											tx -= -1 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 1: // SW
											ty -= -5 - lround( th * sin( rads + 1.5708 ) );
											tx -= 10 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 3: // SE
											ty -= -9 - lround( th * sin( rads + 1.5708 ) );
											tx -=  0 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 2: // S
										default: // default
											ty -= -3 - lround( ( lind / 2 ) * sin( angc ) );
											tx -=  1 + lround( ( lind / 2 ) * cos( angc ) );
											break;
									}
									break;
								case '8': // for W/E hexsides
									angle = -90;
									rads = -1.5708; // 90° in radians
									angb = atan( th / tw );  // angle between upper right corner and lower right corner of the text box,  in radians
									angc = angb + rads; // desired angle plus angle above
									lind = sqrt( pow( th, 2 ) + pow( tw, 2 ) );
									ty = sym_height / 2;
									tx = sym_width / 2;
									switch ( cptr->text_position_ )
									{
										case 7: // NW
											ty -= 12;
											tx -= 11;
											break;
										case 8: // N
											ty -= -1 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= 11 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 9: // NE
											ty -= 12;
											tx -=  -21;
											break;
										case 4: // W
											ty -= -1 - lround( ( lind / 2 ) * sin( angc ) );
											tx -=  4 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 5: // C
											ty -= -1 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -4 + lround( ( lind / 2 ) * cos( angc ) );
											break;
											// 1.5708 rad = 90°
										case 6: // E
											ty -=  -1 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -14 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 1: // SW
											ty -= 8;
											tx -=  11;
											break;
										case 3: // SE
											ty -= 8;
											tx -= -21;
											break;
										case 2: // S
										default: // default
											ty -=  -1 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -21 + lround( ( lind / 2 ) * cos( angc ) );
											break;
									}
									break;
								case '9': // for W/E hexsides
									angle = 90;
									rads = 1.5708; // 90° in radians
									angb = atan( th / tw );  // angle between upper right corner and lower right corner of the text box, in radians
									angc = angb + rads; // desired angle plus angle above
									lind = sqrt( pow( th, 2 ) + pow( tw, 2 ) );
									ty = sym_height / 2;
									tx = sym_width / 2;
									switch ( cptr->text_position_ )
									{
										case 7: // NW
											ty -= -12;
											tx -= 20;
											break;
										case 8: // N
											ty -=  1 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= 20 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 9: // NE
											ty -= -8;
											tx -=  -12;
											break;
										case 4: // W
											ty -=  0 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= 12 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 5: // C
											ty -= 0 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= 4 + lround( ( lind / 2 ) * cos( angc ) );
											break;
											// 1.5708 rad = 90°
										case 6: // E
											ty -=  0 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -5 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 1: // SW
											ty -= -12;
											tx -=  20;
											break;
										case 3: // SE
											ty -= -8;
											tx -= -12;
											break;
										case 2: // S
										default: // default
											ty -=   1 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= -12 + lround( ( lind / 2 ) * cos( angc ) );
											break;
									}
									break;
								case 'u': // 180° = upside down
									angle = 180;
									rads = -3.1416; // 180° in radians = Pi
									angb = atan( th / tw );  // angle between upper right corner and lower right corner of the text box,  in radians
									angc = angb + rads; // desired angle plus angle above
									lind = sqrt( pow( th, 2 ) + pow( tw, 2 ) );
									ty = sym_height / 2;
									tx = sym_width / 2;
									switch ( cptr->text_position_ )
									{
										case 7: // NW
											ty -= 4;
											tx -= -11;
											break;
										case 8: // N
											ty -= 4 - lround( ( lind / 2 ) * sin( angc ) );
											tx -= 1 + lround( ( lind / 2 ) * cos( angc ) );
											break;
										case 9: // NE
											ty -= 4;
											tx -=  -14;
											break;
										case 4: // W
											ty -= -4;
											tx -= -11;
											break;
										case 5: // C
											ty -= -4 - lround( ( lind / 2 ) * sin( angc ) );
											tx -=  1 + lround( ( lind / 2 ) * cos( angc ) );
											break;
											// 1.5708 rad = 90°
										case 6: // E
											ty -= -13 - lround( th * sin( rads + 1.5708 ) );
											tx -= -14 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 1: // SW
											ty -= -24 - lround( th * sin( rads + 1.5708 ) );
											tx -=  -5 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 3: // SE
											ty -= -24 - lround( th * sin( rads + 1.5708 ) );
											tx -= -11 - lround( th * cos( rads + 1.5708 ) );
											break;
										case 2: // S
										default: // default
											ty -= -15 - lround( ( lind / 2 ) * sin( angc ) );
											tx -=   1 + lround( ( lind / 2 ) * cos( angc ) );
											break;
									}
									break;
								default:
									break;
							}
							text = text.BeforeLast(wxChar(';')); // don't render the rotation code
						}

						dc.SetFont( thisFont ); // must set the font again, to register any style changes
						dc.DrawRotatedText( text, x_pos + x_sym + tx, y_pos + y_sym + ty, angle );

						// text boxing around VD/tribal sites
						if ( cptr->type_ == City::Type::VOLKSDEUTSCH )
						{
							wxCoord left( x_pos + x_sym + tx - 2 );
							wxCoord right( x_pos + x_sym + tx + tw );
							wxCoord top( y_pos + y_sym + ty );
							wxCoord bottom( y_pos + y_sym + ty + th - 1 );
							dc.SetPen( *wxBLACK );
							dc.DrawLine( left, top, right, top );
							dc.DrawLine( right, top, right, bottom );
							dc.DrawLine( right, bottom, left, bottom );
							dc.DrawLine( left, bottom, left, top );
						}

					} //draw text

					cptr = cptr->getNext( );

				} // while cptr != nullptr
			}  // for x
		}  // for y
		hdcMem.SelectObject( wxNullBitmap );
		hdcMem2.SelectObject( wxNullBitmap );
		hdcMem3.SelectObject( wxNullBitmap );
		hdcMem4.SelectObject( wxNullBitmap );

		//  ports
		int p, pxo, pyo;
		hdcMem2.SelectObject( *hbmMinorPort );
		hdcMem3.SelectObject( *hbmMajorPort );
		for ( y = max_y; y >= min_y; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_x; x < max_x; ++x )
			{
				if ( ( p = hex_[y][x].getPortSize( ) ) != 0 )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );  // y&1 == y%2 and <<4 == *16 == *Hex::HALF

					ht::hexPortSymbolPositionOffset( hex_[y][x].getPortSymbolDir( ), &pxo, &pyo );

					if ( hex_[y][x].getPortAttribute( ) == Port::Attribute::STRONG )
					{
						hdcMem4.SelectObject( *hbmPortIndicatorMask );
						dc.Blit( x_pos + 10 + pxo, y_pos - 1 + pyo, 11, 4, &hdcMem4, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmStrongPort );
						dc.Blit( x_pos + 10 + pxo, y_pos + pyo - 1, 11, 4, &hdcMem, 0, 0, wxAND );
					}
					else if ( hex_[y][x].getPortAttribute( ) == Port::Attribute::ARTIFICIAL )
					{
						hdcMem4.SelectObject( *hbmPortIndicatorMask );
						dc.Blit( x_pos + 10 + pxo, y_pos + 17 + pyo, 11, 3, &hdcMem4, 0, 0, wxOR );
						hdcMem.SelectObject( *hbmArtificialPort );
						dc.Blit( x_pos + 10 + pxo, y_pos + 16 + pyo, 11, 3, &hdcMem, 0, 0, wxAND );
					}
					switch ( p )
					{
						case Port::Type::MINOR:
							hdcMem4.SelectObject( *hbmRoundPortMask );
							dc.Blit( x_pos + 9 + pxo, y_pos + 3 + pyo, 13, 14, &hdcMem4, 0, 0, wxOR );
							// uses hdcMem2, assigned above
							dc.Blit( x_pos + 10 + pxo, y_pos + 4 + pyo, 11, 12, &hdcMem2, 0, 0, wxAND );
							break;
						case Port::Type::STANDARD:
							hdcMem4.SelectObject( *hbmSquarePortMask );
							dc.Blit( x_pos + 10 + pxo, y_pos + 4 + pyo, 11, 12, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmStandardPort );
							dc.Blit( x_pos + 10 + pxo, y_pos + 4 + pyo, 11, 12, &hdcMem, 0, 0, wxAND );
							break;
						case Port::Type::MAJOR:
							// uses hdcMem3, assigned above
							dc.Blit( x_pos + 10 + pxo, y_pos + 4 + pyo, 11, 12, &hdcMem3, 0, 0, wxCOPY );
							break;
						case Port::Type::GREAT:
							hdcMem.SelectObject( *hbmGreatPort );
							dc.Blit( x_pos + 10 + pxo, y_pos + 4 + pyo, 11, 12, &hdcMem, 0, 0, wxCOPY );
							break;
						case Port::Type::MARGINAL:
							hdcMem4.SelectObject( *hbmRoundPortMask );
							dc.Blit( x_pos + 9 + pxo, y_pos + 3 + pyo, 13, 14, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmMarginalPort );
							dc.Blit( x_pos + 10 + pxo, y_pos + 4 + pyo, 11, 12, &hdcMem, 0, 0, wxAND );
							break;
						case Port::Type::ANCHORAGE:
							hdcMem4.SelectObject( *hbmRoundPortMask );
							dc.Blit( x_pos + 9 + pxo, y_pos + 3 + pyo, 13, 14, &hdcMem4, 0, 0, wxOR );
							hdcMem.SelectObject( *hbmAnchorage );
							dc.Blit( x_pos + 10 + pxo, y_pos + 4 + pyo, 11, 12, &hdcMem, 0, 0, wxAND );
							break;
					}
				}  // if port
			}  // for x ports
		}  // for y ports

		//  intrinsic airfields
		int af, axo, ayo;
		hdcMem2.SelectObject( *hbmAirNavalBaseMask );
		for ( y = max_y; y >= min_y; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_x; x < max_x; ++x )
			{
				if ( ( af = hex_[y][x].getIntrinsicAFCapacity( ) ) != 0 )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );  // y&1 == y%2 and <<4 == *16 == *Hex::HALF
					bool no_viewable_AF = true;
					switch ( af )
					{
						case IntrinsicAF::Cap::ONE:
							hdcMem.SelectObject( *hbmAirfield1 );
							no_viewable_AF = false;
							break;
						case IntrinsicAF::Cap::THREE:
							hdcMem.SelectObject( *hbmAirfield3 );
							no_viewable_AF = false;
							break;
					}
					if ( no_viewable_AF )
						continue;
					ht::hexPortSymbolPositionOffset( hex_[y][x].getIntrinsicAFSymbolDir( ), &axo, &ayo );
					dc.Blit( x_pos + 11 + axo, y_pos + 5 + ayo, 9, 9, &hdcMem2, 0, 0, wxOR );
					dc.Blit( x_pos + 11 + axo, y_pos + 5 + ayo, 9, 9, &hdcMem, 0, 0, wxAND );
				}  // if intrinsic AF
			}
		}
		hdcMem.SelectObject( wxNullBitmap );
		hdcMem2.SelectObject( wxNullBitmap );

		// dams
		for ( y = max_yy; y >= min_yy; --y )
		{
			y_pos = y << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( hex_[y][x].hasDams( ) )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );  // y&1 == y%2 and <<4 == *16 == *Hex::HALF
					dc.SetPen( *wxGREY_PEN );
					// TODO:  a real graphic for an intact dam, SW and S corners
					if ( hex_[y][x].getDam( Hex::SOUTHWEST_CORNER ) )
					{
						// check the three abutting hexsides for a lake / great river
						bool bigDam = hex_[y][x].getLakeSide( Hex::WEST ) || hex_[y][x].getLakeSide( Hex::SOUTHWEST ) ? true : false;
						if ( ! bigDam )
						{
							int xxx, yyy;
							int hasAdj = ht::getAdjHexCoords( Hex::WEST, x, y, &xxx, &yyy, world_x_size_, world_y_size_ );
							bigDam = hasAdj ? hex_[yyy][xxx].getLakeSide( Hex::SOUTHEAST ) : false;
						}
						if ( bigDam )
							dc.DrawCircle( x_pos, y_pos + Hex::SIZE_Y - 3, 6 );
						dc.DrawCircle( x_pos, y_pos + Hex::SIZE_Y - 3, 4 );
						dc.DrawCircle( x_pos, y_pos + Hex::SIZE_Y - 3, 2 );
					}
					if ( hex_[y][x].getDam( Hex::SOUTH_CORNER ) )
					{
						// check the three abutting hexsides for a lake / great river
						// note:  hexside abutment differs for a Hex::SOUTHWEST vice SOUTH vertex
						bool bigDam = hex_[y][x].getLakeSide( Hex::SOUTHWEST ) || hex_[y][x].getLakeSide( Hex::SOUTHEAST ) ? true : false;
						if ( ! bigDam )
						{
							int xxx, yyy;
							int hasAdj = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &xxx, &yyy, world_x_size_, world_y_size_ );
							bigDam = hasAdj ? hex_[yyy][xxx].getLakeSide( Hex::WEST ) : false;
						}
						if ( bigDam )
							dc.DrawCircle( x_pos + Hex::HALF, y_pos + Hex::SIZE_Y + 3, 6 );
						dc.DrawCircle( x_pos + Hex::HALF, y_pos + Hex::SIZE_Y + 3, 4 );
						dc.DrawCircle( x_pos + Hex::HALF, y_pos + Hex::SIZE_Y + 3, 2 );
					}
				}  // if dams
			}
		}

		//  limited stacking indicator
		hdcMem.SelectObject( *hbmLtdStack );
		for ( y = max_y; y >= min_y; --y )
			for ( x = min_x; x < max_x; ++x )
				if ( hex_[y][x].getLimitedStackingHex( ) )
				{
					y_pos = y << 5;  // !! y << 5 == y * 32 == y * Hex::SIZE_Y
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );  // y & 1 == y % 2 and << 4 == *16 == *Hex::HALF
					dc.Blit( x_pos + 9, y_pos + 8, 5, 5, &hdcMem, 0, 0, wxAND );
				}

#if defined HEXTOOLSMAP
		if ( weather_zone_mode_ )  // weather zones
		{
			dc.SetFont( big_font );
			for ( y = max_y; y >= min_y; --y )
			{
				y_pos = y << 5;
				for ( x = min_x; x < max_x; ++x )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );  // y&1 == y%2 and <<4 == *16 == *Hex::HALF
					// using a contrasting cycle of colors
					switch ( hex_[y][x].getWeatherZone( ) % 8 )
					{
						case 0: // purple
							dc.SetTextForeground( wxColour( 69, 0, 138 ) );	break;
						case 1: // light blue
							dc.SetTextForeground( wxColour( 0, 204, 255 ) ); break;
						case 2: // yellow
							dc.SetTextForeground( wxColour( 255, 204, 0 ) ); break;
						case 3: // red
							dc.SetTextForeground( wxColour( 255, 0, 0 ) ); break;
						case 4: // violet
							dc.SetTextForeground( wxColour( 128, 0, 255 ) ); break;
						case 5: // green
							dc.SetTextForeground( wxColour( 70, 220, 70 ) ); break;
						case 6: // dark blue
							dc.SetTextForeground( wxColour( 24, 0, 255 ) ); break;
						case 7: // orange
							dc.SetTextForeground( wxColour( 255, 128, 0 ) ); break;
						default:
							dc.SetTextForeground( *wxBLACK ); break;
					}
					if ( hex_[y][x].getMapIdWord( ) < 0xffff )
						dc.DrawText( _weather_zone_string[hex_[y][x].getWeatherZone( )], x_pos + 16, y_pos + 16 );
						//dc.DrawText( wz_codes[hex[y][x].getWeatherZone( )], x_pos + 16, y_pos + 16 );
				}
			}
			dc.SetTextForeground( *wxBLACK );
		}
		else if ( mapleaf_mode_ || show_hexid_ )
		{
			dc.SetFont( font );
			for ( y = max_y; y >= min_y; --y )
			{
				y_pos = y << 5;
				for ( x = min_x; x < max_x; ++x )
				{
					if ( hex_[y][x].getMapIdWord( ) != 0xffff ) // hex has a non-null map ID
					{
						x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

						char msg6[ht::MAX_MAP_ID_LEN];
						hex_[y][x].getMapIdStr( msg6 );

						// color code the map ID
						int mapsheet_idx = mapsheet_index( msg6 );
						if ( mapsheet_idx == -1 )
							dc.SetTextForeground( *wxBLACK );
						else switch ( mapsheet_idx % 8 )
						{
							case 0: // purple
								dc.SetTextForeground( wxColour( 69, 0, 138 ) );	break;
							case 1: // light blue
								dc.SetTextForeground( wxColour( 0, 204, 255 ) ); break;
							case 2: // yellow
								dc.SetTextForeground( wxColour( 255, 204, 0 ) ); break;
							case 3: // red
								dc.SetTextForeground( wxColour( 255, 0, 0 ) ); break;
							case 4: // violet
								dc.SetTextForeground( wxColour( 128, 0, 255 ) ); break;
							case 5: // green
								dc.SetTextForeground( wxColour( 70, 220, 70 ) ); break;
							case 6: // dark blue
								dc.SetTextForeground( wxColour( 24, 0, 255 ) ); break;
							case 7: // orange
								dc.SetTextForeground( wxColour( 255, 128, 0 ) ); break;
							default:
								// TODO: assert design error here
								dc.SetTextForeground( *wxBLACK ); break;
						}

						wxString mapIdStr( msg6 );
						mapIdStr.Replace( "^", "" );
						strcpy( msg6, mapIdStr.mb_str() );

						dc.SetFont( small_font );
						int th, tw;
						dc.GetTextExtent( msg6, &tw, &th );
						dc.DrawText( msg6, x_pos + 4 + ( 25 - tw ) / 2, y_pos + 5 );

						if ( show_hexid_ )
						{
							// no color for the hex ID
							dc.SetTextForeground( *wxBLACK );
							hex_[y][x].getHexIdStr( msg6 );
							dc.SetFont( font );
							dc.DrawText( msg6, x_pos + 4, y_pos + 17 );
						}
					}
				}
			}
		}
#elif defined HEXTOOLSPLAY
		// hex ownership flags before units
		if ( show_owner_ && scale_ < 5 )  // paint ownership
		{
			hdcMem.SelectObject( *hbmAxis );
			hdcMem2.SelectObject( *hbmAllied );
			for ( y = max_y; y >= min_y; --y )
			{
				y_pos = y << 5;
				for ( x = min_x; x < max_x; ++x )
				{
					if (	hex_[y][x].terrain_ == HexType::SEA
							|| hex_[y][x].terrain_ == HexType::RESTRICTEDWATERS
							|| hex_[y][x].terrain_ == HexType::ICINGSEA )
						continue;

					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					if ( sit_[y][x].getOwner( ) == SidePlayer::AXIS )
					{
						if ( sit_[y][x].getOrigOwner( ) != SidePlayer::AXIS )  // show it has been just captured
						{
							dc.Blit( x_pos + 21, y_pos + 21, 10, 10, &hdcMem2, 0, 0, wxCOPY );
							dc.Blit( x_pos + 17, y_pos + 17, 10, 10, &hdcMem, 0, 0, wxCOPY );
						}
						else
							dc.Blit( x_pos + 21, y_pos + 21, 10, 10, &hdcMem, 0, 0, wxCOPY );
					}
					else if ( sit_[y][x].getOwner( ) == SidePlayer::ALLIED )
					{
						if ( sit_[y][x].getOrigOwner( ) != SidePlayer::ALLIED )  // show it has been just captured
						{
							dc.Blit( x_pos + 21, y_pos + 21, 10, 10, &hdcMem, 0, 0, wxCOPY );
							dc.Blit( x_pos + 17, y_pos + 17, 10, 10, &hdcMem2, 0, 0, wxCOPY );
						}
						else
							dc.Blit( x_pos + 21, y_pos + 21, 10, 10, &hdcMem2, 0, 0, wxCOPY );
					}
				}  // for x
			}  // for y
		}  // if show ownership and Scale < 5

		// bridge break reside between hexes, so draw them before units so
		// that they appear beneath units
		hdcMem.SelectObject( *hbmHit );
		for ( y = max_yy; y >= min_yy; --y )
		{
			for ( x = min_xx; x < max_xx; ++x )
			{
				if ( sit_[y][x].isBridgeBreak( ) )
				{
					y_pos = y << 5;
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
					if ( sit_[y][x].getBridgeBreak( Hex::WEST ) )
						dc.Blit( x_pos - 3, y_pos + 12, 10, 10, &hdcMem, 0, 0 );
					if ( sit_[y][x].getBridgeBreak( Hex::SOUTHWEST ) )
						dc.Blit( x_pos + 4, y_pos + 28, 10, 10, &hdcMem, 0, 0 );
					if ( sit_[y][x].getBridgeBreak( Hex::SOUTHEAST ) )
						dc.Blit( x_pos + 19, y_pos + 28, 10, 10, &hdcMem, 0, 0 );
				}
			}
		}

		if ( GetActiveViewCount( ) )
		{
			int nview, pt, viewx, viewy;
			for ( nview = 0; nview < GetViewCount( ); nview++ )
			{
				if ( ViewIsActive( nview ) )
				{
					hpenHiliteRed->SetColour( GetViewRed( nview ), GetViewGreen( nview ), GetViewBlue( nview ) );
					dc.SetPen( *hpenHiliteRed );
					for ( pt = 0; pt < ViewHexCount( nview ); pt++ )
					{
						viewx = GetViewX( nview, pt );
						viewy = GetViewY( nview, pt );
						if ( viewy >= min_y && viewy <= max_y && viewx >= min_x && viewx <= max_x )
						{
							y_pos = viewy << 5;  // !! y<<5 == y*32 == y*Hex::SIZE_Y
							x_pos = ( viewx << 5 ) + ( ( viewy & 1 ) << 4 );
							dc.DrawLine( x_pos, y_pos + 3, x_pos + Hex::HALF, y_pos - 3 );
							dc.DrawLine( x_pos + Hex::HALF, y_pos - 3, x_pos + Hex::SIZE_X, y_pos + 3 );
							dc.DrawLine( x_pos + Hex::SIZE_X, y_pos + 3, x_pos + Hex::SIZE_X, y_pos + Hex::SIZE_Y - 3 );
							dc.DrawLine( x_pos + Hex::SIZE_X, y_pos + Hex::SIZE_Y - 3, x_pos + Hex::HALF, y_pos + Hex::SIZE_Y + 3 );
							dc.DrawLine( x_pos + Hex::HALF, y_pos + Hex::SIZE_Y + 3, x_pos, y_pos + Hex::SIZE_Y - 3 );
							dc.DrawLine( x_pos, y_pos + Hex::SIZE_Y - 3, x_pos, y_pos + 3 );
						}
					}
				}
			}
			hpenHiliteRed->SetColour( *wxRED );
		}

		if ( _show_units )
		{
			dc.SetFont( bfont );
			dc.SetBrush( *wxWHITE_BRUSH );

			//SetBkMode(dc, OPAQUE);  // stack strengths opaque
			for ( y = max_y; y >= min_y; --y )
			{
				y_pos = y << 5;
				for ( x = min_x; x < max_x; ++x )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					bool curhex = ( selected_hex_point_.y == y && selected_hex_point_.x == x );

					if ( sit_[y][x].units_in_hex_ || ( curhex && _selected_unit_list ) )
					{
						Counter* cntr;

						int side = sit_[y][x].getCounterSide( );
						if ( side == -1 ) // no ground units in unit_list_
						{
							if ( curhex && _selected_unit_list )
							{
								ENUM_SELUNITS(cntr)
									if ( cntr->getCounterType( ) == Counter::Type::UNIT )
									{
										side = cntr->getSide( );
										break;
									}
							}
							else
								side = sit_[y][x].getOwner( );
						}

						// NOTE: if hex is occupied by enemy partisans we must
						//       use enemy 'side' here to get correct colors:
						//if ( sit_[y][x].IsOpponentPartisans( side ) ) // enemy partisans: toggle owner
						//	side = ( side == ALLIED ? AXIS : ALLIED );

						int cnt = sit_[y][x].getLandUnitCount( ); // ground units, pos AA, trucks
						if ( curhex && _selected_unit_list )
							ENUM_SELUNITS(cntr)
								if ( cntr->getCounterType( ) == Counter::Type::UNIT ) // just getting count on first pass
									++cnt;

						GroundUnit* gu = sit_[y][x].getFirstLandUnit( );
						if ( ! gu && curhex && _selected_unit_list )
							ENUM_SELUNITS(cntr)
								if ( cntr->getCounterType( ) == Counter::Type::UNIT )
								{
									gu = static_cast<GroundUnit*>( cntr );
									break;
								}

						if ( gu && cnt > 1 && _show_corps_markers_on_map )
						{
							int subtype = sit_[y][x].getSubType( );
							DrawCorpsCounter(	&dc,
												x_pos + 2,
												y_pos + 2,
												_ground_armed_force[side][subtype].crFg,
												_ground_armed_force[side][subtype].crBg,
												_ground_armed_force[side][subtype].crText,
												sit_[y][x].AEC_,
												sit_[y][x].defensive_strength_ );

							if ( sit_[y][x].getNonLandUnitCount( ) > 0 )
							{
								dc.SetPen( *wxBLACK_PEN );
								dc.DrawLine( x_pos + 29, y_pos + 5, x_pos + 29, y_pos + 28 );
								dc.DrawLine( x_pos + 28, y_pos + 5, x_pos + 28, y_pos + 28 );
								dc.DrawLine( x_pos + 5, y_pos + 28, x_pos + 30, y_pos + 28 );
							}
						}
						else if ( gu /* && cnt == 1 || no XXXs */ ) // just draw first ground unit in unit_list_
						{
							gu->drawCounter( &dc, x_pos + 2, y_pos + 2, false ); // no border
							if ( sit_[y][x].getUnitCount( ) > 1 )
							{
								dc.SetPen( *wxBLACK_PEN );
								dc.DrawLine( x_pos + 29, y_pos + 5, x_pos + 29, y_pos + 28 );
								dc.DrawLine( x_pos + 28, y_pos + 5, x_pos + 28, y_pos + 28 );
								dc.DrawLine( x_pos + 5, y_pos + 28, x_pos + 29, y_pos + 28 );
							}
						}
					}
					else  // no ground units, paint air units
					{
						// vet this, removed:
#if 0
						if ( ! sit_[y][x].unit_list_ )
							; // TODO: suspicious semicolon
#endif
						if ( sit_[y][x].unit_list_ && sit_[y][x].isAirUnits( ) )
						{
							AirUnit* au = sit_[y][x].getFirstAirUnit( );
							if ( au )
							{
								au->drawCounter( &dc, x_pos + 2, y_pos + 2, 0 );
								if ( sit_[y][x].getUnitCount( ) > 1 )
								{
									dc.SetPen( *wxBLACK_PEN );
									dc.DrawLine( x_pos + 29, y_pos + 5, x_pos + 29, y_pos + 28 );
									dc.DrawLine( x_pos + 28, y_pos + 5, x_pos + 28, y_pos + 28 );
									dc.DrawLine( x_pos + 5, y_pos + 27, x_pos + 29, y_pos + 27 );
									dc.DrawLine( x_pos + 5, y_pos + 28, x_pos + 29, y_pos + 28 );
								}
							}
						}
						else if ( sit_[y][x].unit_list_ && IsShips( x, y ) )
						{
							NavalUnit* nu = sit_[y][x].getFirstShip( );
							if ( nu )
							{
								nu->drawCounter( &dc, x_pos + 2, y_pos + 2, 0 );
								if ( sit_[y][x].getUnitCount( ) > 1 )
								{
									dc.SetPen( *wxBLACK_PEN );
									dc.DrawLine( x_pos + 29, y_pos + 5, x_pos + 29, y_pos + 28 );
									dc.DrawLine( x_pos + 28, y_pos + 5, x_pos + 28, y_pos + 28 );
									dc.DrawLine( x_pos + 5, y_pos + 27, x_pos + 29, y_pos + 27 );
									dc.DrawLine( x_pos + 5, y_pos + 28, x_pos + 29, y_pos + 28 );
								}
							}
						}
						else if ( sit_[y][x].unit_list_ ) // no ground, air units or ships
						{	// NOTE: these counters have different counter and map symbols
							if ( sit_[y][x].isNote( ) ) // CNTR_NOTE (notes / alerts)
							{
								hdcMem.SelectObject( *hbmMapNote );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
								if ( sit_[y][x].isAlert( ) ) // CNTR_NOTE w/ alert set
								{
									hdcMem.SelectObject( *hbmAlert );
									dc.Blit( x_pos + 18, y_pos + 16, 14, 14, &hdcMem, 0, 0 );
								}
							}
							else if ( sit_[y][x].isPts( ) ) // CNTR_PTS (status markers)
							{
								hdcMem.SelectObject( *hbmMapSP );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( sit_[y][x].isRP( ) ) // CNTR_RP (resource points)
							{
								hdcMem.SelectObject( *hbmRP );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							// CNTR_MISC:
							else if ( sit_[y][x].isFort( ) ) // forts drawn for hexcontents pane, via Facility::drawSymbol(..)
							{
								hdcMem.SelectObject( *hbmFort );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( sit_[y][x].isPermAF( ) ) // airfields drawn for hexcontents pane, via Facility::drawSymbol(..)
							{
								hdcMem.SelectObject( *hbmPermAF );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							// TODO:  temp AFs not blitted here? (it is drawn for hexcontents pane, via Facility::drawSymbol(..)
							else if ( sit_[y][x].isMulberry( ) ) // mulberries
							{
								hdcMem.SelectObject( *hbmFort );  // bgnd
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
								hdcMem.SelectObject( *hbmMulberry );
								dc.Blit( x_pos + 8, y_pos + 7, 17, 16, &hdcMem, 1, 0 );
								//dc.Blit( x_pos + 7, y_pos + 7, 17, 12, &hdcMem, 1, 1 );
							}
							else if ( sit_[y][x].isFieldWork( ) ) // fieldworks
							{
								hdcMem.SelectObject( *hbmFieldWorks );  // bgnd
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( sit_[y][x].isEntrenchment( ) ) // entrenchments
							{
								hdcMem.SelectObject( *hbmEntrenchent );  // bgnd
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( sit_[y][x].isImpFort( ) ) // improved forts
							{
								hdcMem.SelectObject( *hbmImpFort );  // bgnd
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( sit_[y][x].isBalloonBarrage( ) ) // balloon barrages
							{
								hdcMem.SelectObject( *hbmBalloons );  // bgnd
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( sit_[y][x].isZepBase( ) ) // zeppelin bases
							{
								hdcMem.SelectObject( *hbmZepBase );  // bgnd
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( sit_[y][x].isContested( ) ) // contested hexes
							{
								hdcMem.SelectObject( *hbmContested );  // bgnd
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
#if 0
							else if ( sit_[y][x].getAxisMines( ) )
							{
								hdcMem.SelectObject( *hbmAxMine );
								// AND does transparent bitblt for 2-color bitmap
								dc.Blit( x_pos + 9, y_pos + 3, 14, 12, &hdcMem, 0, 0, wxAND );
								// write mine count
								//dc.SetBkMode(TRANSPARENT);
								sprintf( msg, "%d", sit_[y][x].getAxisMines( ) );
								dc.DrawText( ht::wxS( msg ), x_pos + 16, y_pos + 15 );  // TODO: pos
								//dc.SetBkMode(OPAQUE);
							}
							else if ( sit_[y][x].getAlliedMines( ) )
							{
								hdcMem.SelectObject( *hbmAlMine );
								dc.Blit( x_pos + 9, y_pos + 3, 14, 12, &hdcMem, 0, 0, wxXOR );
								hdcMem.SelectObject( *hbmAxMine );
								dc.Blit( x_pos + 9, y_pos + 3, 14, 12, &hdcMem, 0, 0, wxAND );
								hdcMem.SelectObject( *hbmAlMine );
								dc.Blit( x_pos + 9, y_pos + 3, 14, 12, &hdcMem, 0, 0, wxXOR );
								// write mine count
								//dc.SetBkMode(TRANSPARENT);
								sprintf( msg, "%d", sit_[y][x].getAlliedMines( ) );
								dc.SetTextForeground( wxColour( 128, 0, 0 ) );
								dc.DrawText( msg, x_pos + 16, y_pos + 15 );  // TODO: pos
								//dc.SetBkMode(OPAQUE);
							}
#endif
							// CNTR_HIT:
							else if ( sit_[y][x].isHit( HitMarker::HARASSMENT ) ) // harassment bombing hit
							{
								hdcMem.SelectObject( *hbmMapHitLite );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							else if ( _rule_set.RailsObstructed == TRUE && sit_[y][x].isHit( HitMarker::RR_OBSTRUCT ) ) // RR obstruction
							{
								hdcMem.SelectObject( *hbmMapHitLite );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
							// rest of CNTR_HIT markers (AF, RR, port, CD, port destruction)
							else if ( sit_[y][x].isHit( ) )
							{
								hdcMem.SelectObject( sit_[y][x].getHarassment( ) || sit_[y][x].isRailObstructed( ) ? *hbmMapHitLite : *hbmMapHit );
								dc.Blit( x_pos + 7, y_pos + 6, 19, 19, &hdcMem, 0, 0 );
							}
						}
					}
				}  // for x
			}  // for y

			// target markers, harassment hits, hex supply/isolation markings
			//	these are painted outside the above ground/air/naval unit loop,
			hdcMem.SelectObject( *hbmTarget );
			hdcMem2.SelectObject( *hbmAirTarget );

			// still inside the if ( show_units ) conditional here
			for ( y = max_y; y >= min_y; y-- )
			{
				y_pos = y << 5;
				for ( x = min_x; x < max_x; x++ )
				{
					x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );

					if ( _show_combat_markers && sit_[y][x].isTarget( ) )
						dc.Blit( x_pos + 20, y_pos, 12, 12, &hdcMem, 0, 0, wxCOPY );

					if ( sit_[y][x].isAirTarget( ) )
						dc.Blit( x_pos + 8, y_pos, 12, 12, &hdcMem2, 0, 0, wxCOPY );

					// draw harassment hit count text in the hex
					int h = sit_[y][x].getHarassment( );
					if ( h != 0 ) // ! sea &&
					{
						sprintf( msg, "%d", h );
						dc.DrawText( msg, x_pos + 6, y_pos );
					}

					// U and S marks
					if ( sit_[y][x].getSupply( ) == SupplyStatus::NONE )
					{
						hdcMem3.SelectObject( sit_[y][x].getOwner( ) == SidePlayer::AXIS ? *hbmAxNoSupply : *hbmNoSupply );
						dc.Blit( x_pos + 2, y_pos + 20, 10, 9, &hdcMem3, 0, 0, wxCOPY );
					}
					else if ( sit_[y][x].getSupply( ) == SupplyStatus::SPC )
					{
						// TODO: axis special supply marker
						hdcMem3.SelectObject( sit_[y][x].getOwner( ) == SidePlayer::AXIS ? *hbmAxSpcSupply : *hbmSpcSupply );
						dc.Blit( x_pos + 2, y_pos + 20, 10, 9, &hdcMem3, 0, 0, wxCOPY );
					}

					// I mark
					if ( sit_[y][x].getIsolated( ) )
					{
						hdcMem3.SelectObject( sit_[y][x].getOwner( ) == SidePlayer::AXIS ? *hbmAxIsolated : *hbmIsolated );
						dc.Blit( x_pos + 11, y_pos + 20, 6, 9, &hdcMem3, 0, 0, wxCOPY );
					}
				}  // for x
			}  // for y
		}  // if ! HideUnits
#endif
#if defined DOUBLE_BUFFERING
		// blit double buffered image to screen
		scr_dc.Blit(scr_x, scr_y, blit_w, blit_h, &dc, scr_x, scr_y);
#endif
		hdcMem.SelectObject( wxNullBitmap );
		hdcMem2.SelectObject( wxNullBitmap );
		hdcMem3.SelectObject( wxNullBitmap );
		hdcMem4.SelectObject( wxNullBitmap );

		++upd;
	}  // while upd

#if defined HEXTOOLSPLAY
	// TODO: whether DontRedrawMap is set to 1 or 2, i.e. DontRedrawMap > 0
	//	see also line 492 in paint(..)
	if ( dont_redraw_map_ )
	{
		dont_redraw_map_ = 0;
		// then repaint at 1 hex radius from current hex
		int x2 = selected_hex_point_.x;
		int y2 = selected_hex_point_.y;
		hex2Screen( &x2, &y2 );

		wxRect rect;
		rect.y = y2 - ( Hex::SIZE_Y + 1 );
		rect.height = ( Hex::SIZE_Y * 3 ) + 2;
		rect.x = x2 - ( Hex::SIZE_X + 1 );
		rect.width = ( Hex::SIZE_X * 3 ) + 2;

		Refresh( false, &rect );
	}

	// kludge #4452 for wxWindows: no immediate paint call so hilite must
	// be done after paint

	if ( highlighted_hex_x_ >= 0 && highlighted_hex_y_ >= 0 )
		HiLiteHex( highlighted_hex_x_, highlighted_hex_y_ );

	grayHexes( &dc, min_x, min_y, max_x, max_y );

	// path display (if selected):
	if ( _show_path )
		DrawPaths( );
#endif

}

#if defined HEXTOOLSPLAY

// paint menu command: re-calculate caches, then repaint
void MapPane::rePaint( void )
{
	for ( int x = 0; x < world_x_size_; ++x )
		for ( int y = 0; y < world_y_size_; ++y )
		{
			updateHexCache( x, y );
			sit_[y][x].setTarget( IsHexAttacked( x, y ) );
			sit_[y][x].setAirTarget( IsHexAirAttacked( x, y ) );
			updateHitMarkers( x, y );
		}

	Refresh( );
}

// public (from HexContentsPane::selectUnit(..), deselectUnit(..), and deselectAll(..))
void MapPane::PaintPaths( )
{
	wxClientDC dc( this );
	dc.SetUserScale( 1.0 / scale_, 1.0 / scale_ );

	drawing_path_ = TRUE;

	Counter* c;
	// draw path of units
	if ( _show_path && isHexSelected( ) )
	{
		// FIXME TODO if several units are selected and some are deselected
		// then their paths are not erased
		if ( _selected_unit_list )  // !Sit[ptCurrentHex.y][ptCurrentHex.x].UnitPtr)
		{
			if ( need_erase )
			{
				ENUM_HEXUNITS( selected_hex_point_.x, selected_hex_point_.y, c )
				{
					if ( c->getPathIsShown( ) )
					{
						ErasePath( &dc, selected_hex_point_.x, selected_hex_point_.y, c, true );
						c->setPathIsShown( FALSE );
					}
				}
			}
			DrawPaths( );
		}
		else
		{
			if ( need_erase )
			{
				ENUM_HEXUNITS( selected_hex_point_.x, selected_hex_point_.y, c )
				{
					if ( c->getPathIsShown( ) )
					{
						ErasePath( &dc, selected_hex_point_.x, selected_hex_point_.y, c, true );
						c->setPathIsShown( FALSE );
					}
				}
			}
			need_erase = false;
		}
	}
	ErasePath( &dc, 0, 0, 0 );
	drawing_path_ = FALSE;
}

/*
 ErasePaths works with delay, DrawPaths immediately. That's why DrawPaths
 is called from paint() to ensure paths are draw AFTER old paths are
 erased. This is also why ErasePaths has UnitPtr check (to ensure one
 paint()).
 */

// private helper for DrawPaths(..)
// draw path unit has traveled, unit 'c' starting from hex x,y
void MapPane::DrawPath( wxDC* hdc, int x, int y, Counter* c )
{
	int sx = x, sy = y, i = 0;

	hex2Screen( &sx, &sy );
	sx += 16;  // half hex
	sy += 16;  // half hex

	c->setPathIsShown( TRUE );
	// NOTE: draw path backward -> draw to opposite direction
	while ( 1 )
	{
		switch ( c->getReplay( i++ ) )
		{
			case ht::Replay::END:
				return;
			case ht::Replay::Movement::EAST:
				draw_tac( hdc, &sx, &sy, Hex::WEST );
				break;
			case ht::Replay::Movement::SOUTHEAST:
				draw_tac( hdc, &sx, &sy, Hex::NORTHWEST );
				break;
			case ht::Replay::Movement::NORTHEAST:
				draw_tac( hdc, &sx, &sy, Hex::SOUTHWEST );
				break;
			case ht::Replay::Movement::WEST:
				draw_tac( hdc, &sx, &sy, Hex::EAST );
				break;
			case ht::Replay::Movement::SOUTHWEST:
				draw_tac( hdc, &sx, &sy, Hex::NORTHEAST );
				break;
			case ht::Replay::Movement::NORTHWEST:
				draw_tac( hdc, &sx, &sy, Hex::SOUTHEAST );
				break;
			case ht::Replay::Exploit::EAST:
				draw_expl( hdc, &sx, &sy, Hex::WEST );
				break;
			case ht::Replay::Exploit::SOUTHEAST:
				draw_expl( hdc, &sx, &sy, Hex::NORTHWEST );
				break;
			case ht::Replay::Exploit::NORTHEAST:
				draw_expl( hdc, &sx, &sy, Hex::SOUTHWEST );
				break;
			case ht::Replay::Exploit::WEST:
				draw_expl( hdc, &sx, &sy, Hex::EAST );
				break;
			case ht::Replay::Exploit::SOUTHWEST:
				draw_expl( hdc, &sx, &sy, Hex::NORTHEAST );
				break;
			case ht::Replay::Exploit::NORTHWEST:
				draw_expl( hdc, &sx, &sy, Hex::SOUTHEAST );
				break;
			case ht::Replay::Combat::EAST:
				draw_combat( hdc, &sx, &sy, Hex::WEST );
				break;
			case ht::Replay::Combat::SOUTHEAST:
				draw_combat( hdc, &sx, &sy, Hex::NORTHWEST );
				break;
			case ht::Replay::Combat::NORTHEAST:
				draw_combat( hdc, &sx, &sy, Hex::SOUTHWEST );
				break;
			case ht::Replay::Combat::WEST:
				draw_combat( hdc, &sx, &sy, Hex::EAST );
				break;
			case ht::Replay::Combat::SOUTHWEST:
				draw_combat( hdc, &sx, &sy, Hex::NORTHEAST );
				break;
			case ht::Replay::Combat::NORTHWEST:
				draw_combat( hdc, &sx, &sy, Hex::SOUTHEAST );
				break;
			case ht::Replay::Movement::STAGE:
				hdc->DrawBitmap( *stagebmp, sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::HITRAIL:
				hdc->DrawBitmap( *hitrailbmp, sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::HITAF:
				hdc->DrawBitmap( *hitafbmp, sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::CLEARRAIL:
			case ht::Replay::Movement::FIXRAIL:
			case ht::Replay::Movement::FIXAF:
			case ht::Replay::Movement::FIXPORT:
				hdc->DrawBitmap( *repairbmp, sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::REGAUGE:
				hdc->DrawBitmap( *regaugebmp, sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::HITPORT:
				hdc->DrawText( wxT("Port*"), sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::TOTRAIN:
				hdc->DrawBitmap( *totrainbmp, sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::FRTRAIN:
				hdc->DrawBitmap( *fromtrainbmp, sx - 10, sy - 10 );
				break;
			case ht::Replay::Movement::BOMB:
				hdc->DrawBitmap( *bombbmp, sx - 10, sy - 10 );
				break;
			default:
				break;  // ignore
		}
	}
}

// private helper for PaintPaths(..)
// clear path from the screen
// draw path backward -> draw to opposite direction
void MapPane::ErasePath( wxDC* /*hdc*/, int x, int y, Counter* c, bool hold /* = false */ )
{
	wxRect r;
	int nx, ny, i = 0;
	static int max_x = -1, max_y = -1, min_x = 100000000, min_y = 100000000;

	// if 'hold' then search for min/max coordinates of the are that
	// needs repaint. after all counters have been walked though we
	// will be called with hold=false. then we repaint
	if ( ! hold )
	{
		++max_x;
		++max_y;
		--min_x;
		--min_y;

		hex2Screen( &min_x, &min_y );
		hex2Screen( &max_x, &max_y );

		r.x = min_x;
		r.y = min_y;
		r.width = max_x - min_x;
		r.height = max_y - min_y;

		Refresh( false, &r );

		max_x = -1;
		max_y = -1;
		min_x = 100000000;
		min_y = 100000000;

		return;  // just draw!
	}

	// hold is true, search min/max hexes
	while ( 1 )
	{
		max_x = max( max_x, x );
		max_y = max( max_y, y );
		min_x = min( min_x, x );
		min_y = min( min_y, y );

		switch ( c->getReplay( i++ ) )
		{
			case ht::Replay::END:
				goto end;
			case ht::Replay::Exploit::EAST:
			case ht::Replay::Combat::EAST:
			case ht::Replay::Admin::EAST:
			case ht::Replay::Movement::EAST:
				ht::getAdjHexCoords( Hex::WEST, x, y, &nx, &ny, world_x_size_, world_y_size_ );
				break;
			case ht::Replay::Exploit::SOUTHEAST:
			case ht::Replay::Combat::SOUTHEAST:
			case ht::Replay::Admin::SOUTHEAST:
			case ht::Replay::Movement::SOUTHEAST:
				ht::getAdjHexCoords( Hex::NORTHWEST, x, y, &nx, &ny, world_x_size_, world_y_size_ );
				break;
			case ht::Replay::Exploit::NORTHEAST:
			case ht::Replay::Combat::NORTHEAST:
			case ht::Replay::Admin::NORTHEAST:
			case ht::Replay::Movement::NORTHEAST:
				ht::getAdjHexCoords( Hex::SOUTHWEST, x, y, &nx, &ny, world_x_size_, world_y_size_ );
				break;
			case ht::Replay::Exploit::WEST:
			case ht::Replay::Combat::WEST:
			case ht::Replay::Admin::WEST:
			case ht::Replay::Movement::WEST:
				ht::getAdjHexCoords( Hex::EAST, x, y, &nx, &ny, world_x_size_, world_y_size_ );
				break;
			case ht::Replay::Exploit::SOUTHWEST:
			case ht::Replay::Combat::SOUTHWEST:
			case ht::Replay::Admin::SOUTHWEST:
			case ht::Replay::Movement::SOUTHWEST:
				ht::getAdjHexCoords( Hex::NORTHEAST, x, y, &nx, &ny, world_x_size_, world_y_size_ );
				break;
			case ht::Replay::Exploit::NORTHWEST:
			case ht::Replay::Combat::NORTHWEST:
			case ht::Replay::Admin::NORTHWEST:
			case ht::Replay::Movement::NORTHWEST:
				ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &nx, &ny, world_x_size_, world_y_size_ );
				break;
			default:
				nx = x;
				ny = y;
				break;
		}
		x = nx;
		y = ny;
	}
	end: return;
}

// private helper for paint (stays in maprenderer.cpp)
// gray out hexes:
void MapPane::grayHexes( wxDC* dc, int min_x, int min_y, int max_x, int max_y )
{
	if ( ! ::_gray_out_hexes )
		return;

	dc->SetFont( *wxSWISS_FONT );

	wxMemoryDC hdcMem;
	// TODO: vet this hex mask bitmap for new hex geometry
	hdcMem.SelectObject( *hbmHexMask );
	//hdcMem.SelectObject( *HexTypes[HEXMASK].hbmBitmap );

    int y_pos, x_pos, p;

	for ( int y = max_y; y >= min_y; --y )
	{
		for ( int x = min_x; x < max_x; ++x )
		{
			// TODO: should continue if hex_[y][x].terrain_ == NONE ??

			p = sit_[y][x].getTmp( ) & 0x3fff;
			if ( _show_MPs_on_map && p > 0 && p < 100 )
			{
				wxString wxs;
				wxs.Printf( wxT("%d"), sit_[y][x].getTmp( ) & 0x3fff );
			    int szx, szy;
				dc->GetTextExtent( wxs, &szx, &szy );
				dc->SetBrush( *wxWHITE_BRUSH );
				y_pos = y << 5;
				x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
				dc->DrawRectangle( x_pos - 1, y_pos, szx + 2, szy );
				dc->DrawText( wxs, x_pos, y_pos );
			}

			bool grey_this_hex = sit_[y][x].isGrayed( );

			if ( ! grey_this_hex )
			{
				if ( sea_zone_mode_ && hex_[y][x].isWaterHex( ) )
				{
					if ( sit_[y][x].getSeazoneID( ) == 32 ) // NONE sea zone ID
						grey_this_hex = true;
				}
				else if ( country_mode_ && hex_[y][x].terrain_ != HexType::NONE && ! hex_[y][x].isWaterHex( ) )
				{
					if ( sit_[y][x].getCountryID( ) == 8 )	// NONE country ID
						grey_this_hex = true;
				}
			}

			if ( grey_this_hex )
			{
				y_pos = y << 5;
				x_pos = ( x << 5 ) + ( ( y & 1 ) << 4 );
				// TODO: vet this for the new hex geometry
				dc->Blit( x_pos, y_pos - 2, 32, 35, &hdcMem, 0, 0, wxAND );
				//dc->Blit( x_pos, y_pos - 3, 32, 38, &hdcMem, 0, 0, wxOR );
			}
		}
	}
}

// private helper for paint(..) and PaintPaths( )
// draw paths for selected units, erase for non-selected
void MapPane::DrawPaths( )
{
	wxClientDC dc( this );
	dc.SetUserScale( 1.0 / scale_, 1.0 / scale_ );

	// draw path of units
	if ( _show_path && isHexSelected( ) )
	{
		Counter* c;
		ENUM_SELUNITS(c)
			DrawPath( &dc, selected_hex_point_.x, selected_hex_point_.y, c );
	}
}

bool MapPane::IsShips( int x, int y )
{
	if ( sit_[y][x].isShip( ) )
		return true;

	Counter* c;
	if ( isCurrentHex( x, y ) )
		ENUM_SELUNITS(c)
			if ( c->getCounterType( ) == Counter::Type::SHIP )
				return true;

	return false;
}

// return ship side
int MapPane::GetShipSide( int x, int y )
{
	Counter* c;
	ENUM_HEXUNITS(x, y, c)
		if ( c->getCounterType( ) == Counter::Type::SHIP )
			return c->getSide( );

	if ( isCurrentHex( x, y ) )
	{
		ENUM_SELUNITS(c)
			if ( c->getCounterType( ) == Counter::Type::SHIP )
				return c->getSide( );
	}

	return 0;
}

// return ship nationality
int MapPane::GetShipNat( int x, int y )
{
	Counter* c;
	ENUM_HEXUNITS(x, y, c)
		if ( c->getCounterType( ) == Counter::Type::SHIP )
			return ( (NavalUnit*)c )->getSubType( );

	if ( isCurrentHex( x, y ) )
	{
		ENUM_SELUNITS(c)
			if ( c->getCounterType( ) == Counter::Type::SHIP )
				return ( (NavalUnit*)c )->getSubType( );
	}

	return 0;
}

// private helper for paint(..)
// initialize XXX counter bitmap
void MapPane::initializeCCBitmap( wxBitmap** hbm, wxColour fgnd, wxColour bgnd, int what )
{
	wxMemoryDC hdc;
	wxPen fg_pen( fgnd, 1, wxSOLID );
	wxBrush bg_brush( wxColour( 0, 0, 0 ), wxSOLID );

	if ( ( *hbm = new wxBitmap( _counter_width, _counter_height ) ) == nullptr )
		wxMessageBox( wxT("Failed to create cc bitmap!"), Application::NAME );

	// sprintf (msg, "created bitmap: depth=%d", (*hbm)->GetDepth());
	//wxMessageBox(msg);

	bg_brush.SetColour( bgnd );
	hdc.SelectObject( **hbm );
	hdc.SetBrush( bg_brush );
	hdc.SetPen( *wxBLACK_PEN );
	hdc.DrawRectangle( 0, 0, _counter_width, _counter_height );  //draw_counter(hdc);
	hdc.SetPen( fg_pen );
	switch ( what )
	{
		case GroundUnitType::WW2Corps::TNK:
			draw_arm( &hdc );
			break;
		case GroundUnitType::WW2Corps::INF:
			draw_inf( &hdc );
			break;
		case GroundUnitType::WW2Corps::MXD:
			draw_mxd( &hdc );
			break;
		case GroundUnitType::WW2Corps::MEC:
			draw_mec( &hdc );
			break;
	}
	hdc.SelectObject( wxNullBitmap );
}

#endif
