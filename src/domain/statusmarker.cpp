#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;

#include <wx/wx.h>

#include "rules.h"
#include "counter.h"
#include "statusmarker.h"
#include "dstatusmarker.h"
#include "hextools.h"
using ht::wxS;
using ht::spc2line;
using ht::line2spc;
using ht::isOkStr;

extern int _scn_version;

extern wxFont fnt;
extern PtsDlgXfer _points_dialog_data;
extern wxWindow* _parent_dialog_window;
#if 0
StatusMarker::StatusMarker( )
{
	counter_type_ = Counter::Type::PTS;
	next_ = NULL;
	points_ = 1;
	type_ = PTS_STATUS;
	// limit Status Point Captions to 20 chars to maintain backward compatibility of SCN file
	setCaption( "" );
}
#endif
StatusMarker::StatusMarker( int t /* = PTS_STATUS */ )
{
	counter_type_ = Counter::Type::PTS;
	next_ = NULL;
	points_ = 1;
	type_ = t;
	setCaption( "" );
}

bool StatusMarker::edit( void )
{
	DStatusMarker dlg( _parent_dialog_window );

	char bfr[50];
	sprintf( bfr, "%d", points_ );
	strcpy( _points_dialog_data.editPoints, bfr );
	strcpy( _points_dialog_data.editTxt, caption_ );
	dlg.SetType( type_ );

	if ( dlg.ShowModal( ) != wxID_OK )
		return false;
	dlg.CloseDialog( );

	// get results
	points_ = atoi( _points_dialog_data.editPoints );
	setCaption( _points_dialog_data.editTxt );
	type_ = dlg.GetType( );

	return true;
}

void StatusMarker::drawCounter( wxDC* dc, int x, int y, bool WXUNUSED(border) /* = true */ )
{
	wxColor* bgnd = const_cast<wxColour*>( wxWHITE );
	int szx, szy;
	wxString ws;

	// draw counter
	dc->SetPen( *wxBLACK_PEN );  // always black border on counter
	dc->SetBrush( wxBrush( *bgnd, wxSOLID ) );
	dc->DrawRectangle( x, y, 28, 27 );

	//draw symbol
	int top = y, lft = x;
	dc->DrawEllipse( lft + 2, top + 2, 24, 24 );

	// write strength
	char str[10];
	int x_pos, y_pos;
#if 0
	dc.SetBkMode( TRANSPARENT );
#endif
	dc->SetFont( fnt );
	dc->SetTextForeground( *wxBLACK );
	sprintf( str, "%d", getPts( ) );
	ws = wxS( str );
	dc->GetTextExtent( ws, &szx, &szy );
	x_pos = x + 14 - ( szx / 2 );
	y_pos = y + 13 - ( szy / 2 );
	dc->DrawText( ws, x_pos, y_pos );
#if 0
	dc.SetBkMode( OPAQUE );
#endif
}

void StatusMarker::getSymbolString( char* s )
{
	sprintf( s, "%s                     ", caption_ );
}

void StatusMarker::getUnitString( char* s, bool verbose /* = false */ )
{
	sprintf( s, "%d pts status marker: %s", points_, caption_ );
	if ( verbose )
	{
		switch ( type_ )
		{
			case StatusMarker::ATTACK:
				strcat( s, "\nAdded to attack strength" );
				break;
			case StatusMarker::DEFENSE:
				strcat( s, "\nAdded to defense strength" );
				break;
			case StatusMarker::AA:
				strcat( s, "\nAdded to AA strength" );
				break;
			case StatusMarker::CD:
				strcat( s, "\nAdded to CD strength" );
				break;
			case StatusMarker::DIE:
				strcat( s, "\nAdded to die roll modifier" );
				break;
		}
	}
}

// loading
std::istream& operator>>( std::istream& is, StatusMarker& sm )
{
	int a;
	is >> a;
	sm.points_ = a;

	int b = StatusMarker::STATUS;			// default (vanilla status marker) for .scn v2 and earlier
	if ( _scn_version >= 3 )	// starting with .scn v3, varieties of status markers
		is >> b;
	sm.type_ = b;

	char c[StatusMarker::STATUS_CAPTION_LEN + 1];	// was 50
	is >> c;

	line2spc( c );
	strncpy( sm.caption_, c, StatusMarker::STATUS_CAPTION_LEN );

	return is;
}

// saving
std::ostream& operator<<( std::ostream& os, StatusMarker& sm )
{
	spc2line( sm.caption_ );

	os << static_cast<int>( sm.points_ ) << ' ';

	os << static_cast<int>( sm.type_ ) << ' ';

	if ( isOkStr( sm.caption_ ) )
		os << sm.caption_ << ' ';
	else
		os << "-" << ' ';

	line2spc( sm.caption_ );

	return os;
}

#endif
