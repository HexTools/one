#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;

#include <wx/wx.h>

#include "application.h"
#include "rulesvariant.h"
#include "rules.h"
#include "counter.h"
#include "facility.h"
#include "dfacility.h"
#include "hextools.h"
using ht::drawStr;

extern wxFont fnt;

extern MiscDlgXfer _misc_dialog_data;

extern int _current_player;

extern wxBitmap* hbmMulberry;
extern wxBitmap* hbmEntrenchent;
extern wxBitmap* hbmImpFort;
extern wxBitmap* hbmFieldWorks;
extern wxBitmap* hbmZepBase;
extern wxBitmap* hbmContested;
extern wxBitmap* hbmBalloons;

extern wxWindow* _parent_dialog_window;

Facility::Facility( int t /* = MISC_FORT */ )
{
	counter_type_ = Counter::Type::MISC;
	next_ = nullptr;
	type_ = static_cast<unsigned char>( t );
	setSide( _current_player );
}

bool Facility::edit( void )
{
	if ( getMiscType( ) == Facility::MULBERRY )
	{
		wxMessageBox( wxT("Mulberry does not have any mutable properties"), Application::NAME, wxOK );
		return true;
	}
	// init dialog
	_misc_dialog_data.rdFort = _misc_dialog_data.rdTempAF = _misc_dialog_data.rdPermAF = FALSE;
	_misc_dialog_data.rdPermAF6 = _misc_dialog_data.rdPermAF9 = _misc_dialog_data.rdPermAF12 = FALSE;
	switch ( type_ )
	{
		case Facility::FORT:
			_misc_dialog_data.rdFort = TRUE;
			break;
		case Facility::TEMPAF:
			_misc_dialog_data.rdTempAF = TRUE;
			break;
		case Facility::PERMAF3:
			_misc_dialog_data.rdPermAF = TRUE;
			break;
		case Facility::PERMAF6:
			_misc_dialog_data.rdPermAF6 = TRUE;
			break;
		case Facility::PERMAF9:
			_misc_dialog_data.rdPermAF9 = TRUE;
			break;
		case Facility::PERMAF12:
			_misc_dialog_data.rdPermAF12 = TRUE;
			break;
		case Facility::FIELDWORKS:
			_misc_dialog_data.rdFieldWorks = TRUE;
			break;
		case Facility::ENTRENCHMENT:
			_misc_dialog_data.rdEntr = TRUE;
			break;
		case Facility::IMPR_FORT:
			_misc_dialog_data.rdImpFort = TRUE;
			break;
	}

	DFacility dlg( _parent_dialog_window );

	if ( dlg.ShowModal( ) != wxID_OK )
		return false;
	dlg.CloseDialog( );

	// get results
	if ( _misc_dialog_data.rdFort == TRUE )
		type_ = Facility::FORT;
	else if ( _misc_dialog_data.rdTempAF == TRUE )
		type_ = Facility::TEMPAF;
	else if ( _misc_dialog_data.rdPermAF6 == TRUE )
		type_ = Facility::PERMAF6;
	else if ( _misc_dialog_data.rdPermAF9 == TRUE )
		type_ = Facility::PERMAF9;
	else if ( _misc_dialog_data.rdPermAF12 == TRUE )
		type_ = Facility::PERMAF12;
	else if ( _misc_dialog_data.rdFieldWorks )
		type_ = Facility::FIELDWORKS;
	else if ( _misc_dialog_data.rdEntr )
		type_ = Facility::ENTRENCHMENT;
	else if ( _misc_dialog_data.rdImpFort )
		type_ = Facility::IMPR_FORT;
	else
		type_ = Facility::PERMAF3;

	return true;
}

void Facility::drawSymbol( wxDC* hdc, int lft, int top, int rgt, int bot )
{
	wxPen hpen( *wxBLACK, 2, wxSOLID );
	wxBrush bgbr( *wxWHITE, wxSOLID ), oldbr;
	wxMemoryDC memdc;

	// Draw background box, and switch to foreground GDI objects
	hdc->SetPen( *wxBLACK_PEN );
	hdc->SetBrush( bgbr );  // background brush
#if 0
	Rectangle( hdc, lft, top, rgt, bot );       // Draw rectangle
#endif
	// draw:
	hdc->SetPen( hpen );
	int mid = ( lft + rgt ) / 2 - 1;
	int x = lft, y = top;
	switch ( type_ )
	{
		case Facility::FIELDWORKS:
			memdc.SelectObject( *hbmFieldWorks );
			hdc->Blit( x + 2, y + 1, 19, 19, &memdc, 0, 0 );
			break;
		case Facility::ENTRENCHMENT:
			memdc.SelectObject( *hbmEntrenchent );
			hdc->Blit( x + 2, y + 1, 19, 19, &memdc, 0, 0 );
			break;
		case Facility::IMPR_FORT:
			memdc.SelectObject( *hbmImpFort );
			hdc->Blit( x + 2, y + 1, 19, 19, &memdc, 0, 0 );
			break;
		case Facility::ZEP_BASE:
			memdc.SelectObject( *hbmZepBase );
			hdc->Blit( x + 2, y + 1, 19, 19, &memdc, 0, 0 );
			break;
		case Facility::FORT:
			hdc->DrawLine( mid, top + 2, rgt - 7, top + 6 );
			hdc->DrawLine( rgt - 7, top + 6, rgt - 7, bot - 7 );
			hdc->DrawLine( rgt - 7, bot - 7, mid, bot - 3 );
			hdc->DrawLine( mid, bot - 3, lft + 6, bot - 7 );
			hdc->DrawLine( lft + 6, bot - 7, lft + 6, top + 6 );
			hdc->DrawLine( lft + 6, top + 6, mid, top + 2 );
			break;
		case Facility::PERMAF3:
		case Facility::PERMAF6:
		case Facility::PERMAF9:
		case Facility::PERMAF12:
			hdc->DrawLine( rgt - 6, top + 5, rgt - 6, bot - 1 );
			hdc->DrawLine( rgt - 5, top + 6, lft + 8, bot - 1 );
			hdc->DrawLine( rgt - 4, bot - 3, lft + 7, bot - 3 );
			// draw box for capacity
			hdc->SetPen( *wxBLACK_PEN );
			hdc->SetBrush( *wxWHITE_BRUSH );
			hdc->SetFont( fnt );
			hdc->DrawRectangle( x, y, 11, 11 );
			hdc->SetTextForeground( *wxBLACK );
			hdc->SetTextBackground( *wxWHITE );

			if ( type_ == Facility::PERMAF3 )
				drawStr( hdc, x + 4, y + 2, const_cast<char*>( "3" ) );
			else if ( type_ == Facility::PERMAF6 )
				drawStr( hdc, x + 4, y + 2, const_cast<char*>( "6" ) );
			else if ( type_ == Facility::PERMAF9 )
				drawStr( hdc, x + 4, y + 2, const_cast<char*>( "9" ) );
			else if ( type_ == Facility::PERMAF12 )
				drawStr( hdc, x + 2, y + 2, const_cast<char*>( "12" ) );
			break;
		case Facility::TEMPAF:
			hdc->DrawLine( rgt - 8, top + 3, rgt - 8, bot - 3 );
			hdc->DrawLine( rgt - 6, top + 5, lft + 6, bot - 3 );
			break;
		case Facility::MULBERRY:
			memdc.SelectObject( *hbmMulberry );
			hdc->Blit( x + 2, y + 4, 19, 16, &memdc, 0, 0 );
			break;
		case Facility::BALLOON:
			memdc.SelectObject( *hbmBalloons );
			hdc->Blit( x + 2, y + 1, 19, 19, &memdc, 0, 0 );
			break;
		case Facility::CONTEST:
			memdc.SelectObject( *hbmContested );
			hdc->Blit( x + 2, y + 1, 19, 19, &memdc, 0, 0 );
			break;
	}
}

void Facility::drawCounter( wxDC *hdc, int x, int y, bool WXUNUSED(border) /* = true */ )
{
	hdc->SetPen( *wxBLACK_PEN );  // always black border on counter
	hdc->SetBrush( *wxWHITE_BRUSH );
	hdc->DrawRectangle( x, y, 28, 27 );
	int lft = x + 2;
	int rgt = x + 26;
	int top = y + 3;
	int bot = y + 21;
	drawSymbol( hdc, lft, top, rgt, bot );
}

void Facility::getSymbolString( char *s )
{
	switch ( type_ )
	{
		case Facility::FORT:
			sprintf( s, "Fort            " );
			break;
		case Facility::TEMPAF:
			sprintf( s, "Tmp. Airfield   " );
			break;
		case Facility::PERMAF3:
			sprintf( s, "Airfield 3 cap  " );
			break;
		case Facility::PERMAF6:
			sprintf( s, "Airfield 6 cap  " );
			break;
		case Facility::PERMAF9:
			sprintf( s, "Airfield 9 cap  " );
			break;
		case Facility::PERMAF12:
			sprintf( s, "Airfield 12cap  " );
			break;
		case Facility::MULBERRY:
			sprintf( s, "Mulberry        " );
			break;
		case Facility::FIELDWORKS:
			sprintf( s, "Fieldworks      " );
			break;
		case Facility::ENTRENCHMENT:
			sprintf( s, "Entrenchment    " );
			break;
		case Facility::IMPR_FORT:
			sprintf( s, "Improved Fort   " );
			break;
		case Facility::ZEP_BASE:
			sprintf( s, "Zeppelin base   " );
			break;
		case Facility::CONTEST:
			sprintf( s, "Contested Hex   " );
			break;
		case Facility::BALLOON:
			sprintf( s, "Balloon Barrage " );
			break;
		default:
			sprintf( s, "ufo base!       " );
			break;
	}
}

void Facility::getUnitString( char *s, bool WXUNUSED(verbose) )
{
	getSymbolString( s );
}

// loading
std::istream& operator>>( std::istream& is, Facility& facility )
{
	int a;
	is >> a;
	facility.type_ = a;

	return is;
}

// saving
std::ostream& operator<<( std::ostream& os, Facility& facility )
{
	os << static_cast<int>( facility.type_ ) << ' ';

	return os;
}

#endif
