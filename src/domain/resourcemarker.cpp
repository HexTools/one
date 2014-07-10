#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;

#include <wx/wx.h>

#include "rules.h"
#include "counter.h"
#include "resourcemarker.h"
#include "dresourcemarker.h"
#include "hextools.h"

using ht::wxS;

extern int _current_player;
extern wxFont fnt;
extern RpDlgXfer RpDlgData;
extern wxWindow* _parent_dialog_window;

ResourceMarker::ResourceMarker( )
{
	counter_type_ = Counter::Type::RP;
	next_ = NULL;
	num_points_ = 1;
	setSide( _current_player );
}

bool ResourceMarker::edit( void )
{
	DResourceMarker dlg( _parent_dialog_window );

	char bfr[50];
	sprintf( bfr, "%d", num_points_ );
	strcpy( RpDlgData.editPts, bfr );

	if ( dlg.ShowModal( ) != wxID_OK )
		return false;
	dlg.CloseRpDialog( );

	// get results
	num_points_ = strtol( RpDlgData.editPts, 0, 0 );

	return true;
}

void ResourceMarker::drawCounter( wxDC* hdc, int x, int y, bool WXUNUSED(border) /* = true */ )
{
	wxColor bgnd( 255, 255, 255 ), fgnd( 0, 0, 0 );
	int szx, szy;

	// draw counter
	hdc->SetPen( *wxBLACK_PEN );  // always black border on counter
	hdc->SetBrush( wxBrush( bgnd, wxSOLID ) );
	hdc->DrawRectangle( x, y, 28, 27 );

	//draw symbol
	int mid = x + 14, top = y, bot = y + 17, lft = x + 3;
	hdc->DrawEllipse( lft + 5, top + 3, 11, 11 );
	hdc->DrawLine( mid - 1, top + 4, mid - 1, bot - 4 );
	hdc->DrawLine( mid - 2, top + 4, mid - 2, bot - 4 );
	hdc->DrawLine( mid - 3, top + 5, mid - 3, bot - 5 );
	hdc->DrawLine( mid - 4, top + 5, mid - 4, bot - 5 );
	hdc->DrawLine( mid - 5, top + 6, mid - 5, bot - 6 );
	hdc->DrawLine( mid - 6, top + 7, mid - 6, bot - 7 );

	// write strength
	char str[10];
	int x_pos;
#if 0
	dc.SetBkMode( TRANSPARENT );
#endif
	hdc->SetFont( fnt );
	sprintf( str, "%d", getPts( ) );
	hdc->GetTextExtent( wxS( str ), &szx, &szy );
	x_pos = x + 14 - ( szx / 2 );
	hdc->DrawText( wxS( str ), x_pos, y + 13 );
#if 0
	dc.SetBkMode( OPAQUE );
#endif
}

void ResourceMarker::getSymbolString( char* s )
{
	sprintf( s, "%d resource pts        ", num_points_ );
}

void ResourceMarker::getUnitString( char* s, bool WXUNUSED(verbose) /* = false */ )
{
	sprintf( s, "%d resource pts        ", num_points_ );
}

// loading
std::istream &operator>>( std::istream &is, ResourceMarker& respt )
{
	int a;
	is >> a;
	respt.num_points_ = a;

	return is;
}

// saving
std::ostream &operator<<( std::ostream& os, ResourceMarker& respt )
{
	os << static_cast<int>( respt.num_points_ ) << ' ';

	return os;
}

#endif
