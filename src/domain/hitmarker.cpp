#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "rules.h"
#include "counter.h"
#include "hitmarker.h"
#include "hextools.h"
using ht::wxS;

extern wxFont fnt;

extern int _current_player;

extern wxBitmap* hbmSmallHit;	// tiny hit marker for hit counters
extern wxBitmap* hbmCDHit;
extern wxBitmap* hbmHarPlane;
extern wxBitmap* hbmAxMine;
extern wxBitmap* hbmAlMine;
extern wxBitmap* hbmDestroyed;
extern wxBitmap* hbmSmallPort;

HitMarker::HitMarker( int t /* = HIT_RR */)
{
	counter_type_ = Counter::Type::HIT;
	next_ = NULL;
	type_ = t;
	data_ = 1;
	setSide( _current_player );
}

// it is not possible to edit hit-markers directly!
bool HitMarker::edit( void )
{
	wxMessageBox( wxT("You can't directly edit this counter.\n"
					"Instead, use commands in Hex-menu in the\n"
					"setup mode to change hex properties"), Application::NAME, wxOK | wxICON_INFORMATION );
	return true;
}

void HitMarker::drawCounter( wxDC* hdc, int x, int y, bool WXUNUSED(border) /* = true */ )
{
	wxMemoryDC memdc;
	int szx, szy;
	wxPen hpen1( *wxBLACK, 1, wxSOLID );
	wxPen hpen2( *wxBLACK, 2, wxSOLID );
	char str[10];
	int x_pos, midx, midy;
	wxString ws;

	// draw counter (always black on white)
	hdc->SetPen( *wxBLACK_PEN );
	hdc->SetBrush( *wxWHITE_BRUSH );
	hdc->DrawRectangle( x, y, 28, 27 );
#if 0
	hdc.SetBkMode(TRANSPARENT);
#endif
	hdc->SetFont( fnt );

	//draw symbol
	switch ( getHitType( ) )
	{
		case HitMarker::RR:
		case HitMarker::AF:
			midx = x + 14, midy = y + 8;
			hdc->SetPen( hpen2 );
			hdc->DrawLine( midx - 5, midy + 3, midx + 4, midy - 3 );
			hdc->DrawLine( midx - 5, midy - 3, midx + 4, midy + 3 );
			hdc->DrawLine( midx, midy + 6, midx, midy - 6 );
			sprintf( str, "%d", getHitCount( ) );
			ws = wxS( str );
			hdc->GetTextExtent( ws, &szx, &szy );
			x_pos = x + 14 - ( szx / 2 );
			hdc->DrawText( ws, x_pos, y + 13 );
			break;
		case HitMarker::RR_OBSTRUCT:
			hdc->SetPen( hpen1 );
			hdc->DrawLine( x + 18, y + 3, x + 16, y + 4 );
			hdc->DrawLine( x + 16, y + 4, x + 11, y + 23 );
			hdc->DrawLine( x + 11, y + 23, x + 8, y + 24 );
			memdc.SelectObject( *hbmSmallHit );
			hdc->Blit( x + 10, y + 9, 8, 8, &memdc, 0, 0 );
			break;
		case HitMarker::PORT:
			memdc.SelectObject( *hbmSmallPort );
			hdc->Blit( x + 9, y + 3, 11, 12, &memdc, 0, 0 );
			sprintf( str, "%d", getHitCount( ) );
			ws = wxS( str );
			hdc->GetTextExtent( ws, &szx, &szy );
			x_pos = x + 10 - ( szx / 2 );
			hdc->DrawText( ws, x_pos, y + 13 );
			memdc.SelectObject( *hbmSmallHit );
			hdc->Blit( x + 13 + ( szx / 2 ), y + 16, 8, 8, &memdc, 0, 0 );
			break;
		case HitMarker::CD:
			memdc.SelectObject( *hbmCDHit );
			hdc->Blit( x + 7, y + 2, 15, 15, &memdc, 0, 0 );
			sprintf( str, "%d", getHitCount( ) );
			ws = wxS( str );
			hdc->GetTextExtent( ws, &szx, &szy );
			x_pos = x + 10 - ( szx / 2 );
			hdc->DrawText( ws, x_pos, y + 13 );
			memdc.SelectObject( *hbmSmallHit );
			hdc->Blit( x + 14 + ( szx / 2 ), y + 16, 8, 8, &memdc, 0, 0 );
			break;
		case HitMarker::AXMINE:
			memdc.SelectObject( *hbmAxMine );
			hdc->Blit( x + 7, y + 3, 14, 12, &memdc, 0, 0 );
			sprintf( str, "%d", getHitCount( ) );
			ws = wxS( str );
			hdc->GetTextExtent( ws, &szx, &szy );
			x_pos = x + 14 - ( szx / 2 );
			hdc->DrawText( ws, x_pos, y + 13 );
			break;
		case HitMarker::ALMINE:
			memdc.SelectObject( *hbmAlMine );
			hdc->Blit( x + 7, y + 3, 14, 12, &memdc, 0, 0 );
			sprintf( str, "%d", getHitCount( ) );
			ws = wxS( str );
			hdc->GetTextExtent( ws, &szx, &szy );
			x_pos = x + 14 - ( szx / 2 );
			hdc->SetTextForeground( wxColour( 128, 0, 0 ) );
			hdc->DrawText( ws, x_pos, y + 13 );
			break;
		case HitMarker::HARASSMENT:
			memdc.SelectObject( *hbmHarPlane );
			hdc->Blit( x + 7, y + 3, 14, 12, &memdc, 0, 0 );
			// write hit count
			sprintf( str, "%d", getHitCount( ) );
			ws = wxS( str );
			hdc->GetTextExtent( ws, &szx, &szy );
			x_pos = x + 10 - ( szx / 2 );
			hdc->DrawText( ws, x_pos, y + 13 );
			memdc.SelectObject( *hbmSmallHit );
			hdc->Blit( x + 13 + ( szx / 2 ), y + 16, 8, 8, &memdc, 0, 0 );
			break;
			// deprecated: att & def support ARE NOT HITS. they are status markers.
#if 0
			 case HitMarker::ATTSUPPORT:
			 memdc.SelectObject(hbmAttSupport);
			 BitBlt(dc, x+7,y+2, 15,15, memdc, 0, 0, SRCCOPY);
			 // write mine count
			 sprintf(str, "%d", GetHitCount());
			 sz = dc.GetTextExtent(str, strlen(str));
			 x_pos = x+14-(sz.cx/2);
			 dc.DrawText(x_pos,y+15, str);
			 break;
			 case HIT_DEFSUPPORT:
			 memdc.SelectObject(hbmDefSupport);
			 BitBlt(dc, x+7,y+2, 15,15, memdc, 0, 0, SRCCOPY);
			 // write mine count
			 sprintf(str, "%d", GetHitCount());
			 sz = dc.GetTextExtent(str, strlen(str));
			 x_pos = x+14-(sz.cx/2);
			 dc.DrawText(x_pos,y+15, str);
			 break;
#endif
		case HitMarker::PORTDEST:
			memdc.SelectObject( *hbmDestroyed );
			hdc->Blit( x + 5, y + 5, 18, 18, &memdc, 0, 0 );
			break;
	}
#if 0
	dc.SetBkMode(OPAQUE);
#endif
}

void HitMarker::getSymbolString( char* s )
{
	switch ( type_ )
	{
		case HitMarker::AF:
			if ( getHitCount( ) == 1 )
				sprintf( s, "1 Airfield hit        " );
			else
				sprintf( s, "%d Airfield hits      ", getHitCount( ) );
			break;
		case HitMarker::RR:
			sprintf( s, "Rail hit          " );
			break;
		case HitMarker::RR_OBSTRUCT:
			sprintf( s, "Rail obstruction       " );
			break;
		case HitMarker::PORT:
			sprintf( s, "%d Port hits    ", getHitCount( ) );
			break;
		case HitMarker::CD:
			sprintf( s, "%d CD hits      ", getHitCount( ) );
			break;
		case HitMarker::HARASSMENT:
			if ( getHitCount( ) == 1 )
				sprintf( s, "1 Harassment hit " );
			else
				sprintf( s, "%d Harassment hits ", getHitCount( ) );
			break;
		case HitMarker::AXMINE:
			if ( getHitCount( ) == 1 )
				sprintf( s, "1 Axis mine     " );
			else
				sprintf( s, "%d Axis mines   ", getHitCount( ) );
			break;
		case HitMarker::ALMINE:
			if ( getHitCount( ) == 1 )
				sprintf( s, "1 Allied mine    " );
			else
				sprintf( s, "%d Allied mines  ", getHitCount( ) );
			break;
		case HitMarker::PORTDEST:
			sprintf( s, "Port destruction" );
			break;
#if 0
		// deprecated
		case HitMarker::ATTSUPPORT:
			sprintf( s, "%d attack pts    ", getHitCount( ) );
			break;
		case HitMarker::DEFSUPPORT:
			sprintf( s, "%d defense pts   ", getHitCount( ) );
			break;
#endif
	}
}

void HitMarker::getUnitString( char* s, bool WXUNUSED(verbose) /* = false */ )
{
	switch ( type_ )
	{
		case HitMarker::AF:
			if ( getHitCount( ) == 1 )
				sprintf( s, "1 hit of damage on the airfield" );
			else
				sprintf( s, "%d hits of damage on the airfield", getHitCount( ) );
			break;
		case HitMarker::RR:
			sprintf( s, "railroad broken" );
			break;
		case HitMarker::RR_OBSTRUCT:
			sprintf( s, "railroad obstructed" );
			break;
		case HitMarker::PORT:
			sprintf( s, "%d hits of damage on the port", getHitCount( ) );
			break;
		case HitMarker::CD:
			sprintf( s, "%d hits on the coastal defenses", getHitCount( ) );
			break;
		case HitMarker::HARASSMENT:
			sprintf( s, "%d harassment hits", getHitCount( ) );
			break;
		case HitMarker::AXMINE:
			if ( getHitCount( ) == 1 )
				sprintf( s, "1 axis mine point" );
			else
				sprintf( s, "%d axis mine points", getHitCount( ) );
			break;
		case HitMarker::ALMINE:
			if ( getHitCount( ) == 1 )
				sprintf( s, "1 allied mine point" );
			else
				sprintf( s, "%d allied mine points", getHitCount( ) );
			break;
		case HitMarker::PORTDEST:
			sprintf( s, "Port destroyed" );
			break;
#if 0
		// deprecated
		case HitMarker::ATTSUPPORT:
			sprintf( s, "%d points general attack support", getHitCount( ) );
			break;
		case HitMarker::DEFSUPPORT:
			sprintf( s, "%d points general defensive support", getHitCount( ) );
			break;
#endif
	}
}

// no loading/saving

#endif
