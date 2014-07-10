#include <wx/wx.h>

#include "hextools.h"
using ht::wxS;
using ht::coordinatesOutOfBounds;

#include "application.h"
#include "hextype.h"
#if defined HEXTOOLSPLAY
#include "selectedunits.loop"
#include "hexunits.loop"
#include "sideplayer.h"
#endif

#include "city.h"
#include "hex.h"
#include "mappane.h"

#if defined HEXTOOLSPLAY
#include "counter.h"
#include "groundunit.h"
#include "airunit.h"
#include "hexnote.h"
#include "hexcontents.h"
#include "hexcontentspane.h"
#include "groundunitreplpool.h"
#include "airunitreplpool.h"
#include "dreplacementpool.h"
#include "daircombatdice.h"
#endif

DECLARE_APP(Application);

#if defined HEXTOOLSPLAY
extern char msg[];		// 10000
extern Counter* _selected_unit_list;			// defined in HexContentsPane.cpp
extern int _current_player;
extern int _year;
extern int _month;
extern int _turn;

extern GroundUnitReplPool _repl_pool[];	// AXIS and ALLIED
extern AirUnitReplPool _air_repl_pool[];	// AXIS and ALLIED

extern int _cache_axis_attack_strengths;
extern int _cache_allied_attack_strengths;
extern int _show_path;
#endif

// both HT-m and HT-p
// private helper for SelectHex(..) and MoveSelection(..)
// center given hex to screen
void MapPane::centerHex( int x, int y )
{
// scroller setup:
// MUST NOT BE CHANGED FROM 32 OR ZOOMED MODES WON'T WORK PROPERLY!
#define SCROLLSTEPS_PER_HEX 32 // HexTools-play says 32 for zoomability, was 8

	// to pixel coordinates:
	x = ( x * Hex::SIZE_X ) + ( y % 2 ) * ( Hex::SIZE_X / 2 );
	y = y * Hex::SIZE_Y;

	// subst 1/2 win size (=center)
	int w, h;
	GetClientSize( &w, &h );

// TODO: investigate this discrepancy
#if defined HEXTOOLSMAP
	x -= static_cast<int>( ( w - SCROLLSTEPS_PER_HEX ) / 2 );
	y -= static_cast<int>( ( h - SCROLLSTEPS_PER_HEX ) / 2 );
#elif defined HEXTOOLSPLAY
	x -= static_cast<int>( ( ( ( w / 2 ) - 16 ) * scale_ ) );
	y -= static_cast<int>( ( ( ( h / 2 ) - 16 ) * scale_ ) );
#endif

	// convert to scroll units
	x = ( x * SCROLLSTEPS_PER_HEX ) / Hex::SIZE_X;
	y = ( y * SCROLLSTEPS_PER_HEX ) / Hex::SIZE_Y;

	x = ht::limit( x, 0, ht::NoUpperBound );
	y = ht::limit( y, 0, ht::NoUpperBound );

	setScrollbarRange( x, y );
	Refresh( false ); // don't erase background
}

// zoom map, dir: -1=in, 1=out, 0=restore normal (100%)
void MapPane::zoom( int dir )
{
#if defined HEXTOOLSMAP
	int w, h;
	GetClientSize( &w, &h );

	// capture center hex x,y (at current zoom Scale)
	int x = w / 2;
	int y = h / 2;

	// get the hex coordinates before changing Scale
	screen2Hex( &x, &y );
#endif

	float ns = 1.0;

	if ( dir == 0 )
		setZoomScale( 1 );
	else if ( dir < 0 )
	{
		if ( scale_ == 0.5 )
			return;
		else if ( scale_ == 1.0 )
			ns = 0.5;
		else if ( scale_ == 1.5 )
			ns = 1.0;
		else if ( scale_ == 2.0 )
			ns = 1.5;
		else if ( scale_ == 4.0 )
			ns = 2.0;
		else if ( scale_ == 8.0 )
			ns = 4.0;
		else if ( scale_ == 16.0 )
			ns = 8.0;
	}
	else
	{
		if ( scale_ == 0.5 )
			ns = 1.0;
		else if ( scale_ == 1.0 )
			ns = 1.5;
		else if ( scale_ == 1.5 )
			ns = 2.0;
		// there is a bug in wxgtk: crashes on zoom level > 2
#ifdef __WXGTK__
		else ns = 2.0;
#else
		else if ( scale_ == 2.0 )
			ns = 4.0;
		else if ( scale_ == 4.0 )
			ns = 8.0;
		else if ( scale_ == 8.0 )
			ns = 16.0;
		else if ( scale_ == 16.0 )
			return;
#endif
	}

	setZoomScale( ns );
#if defined HEXTOOLSMAP
	setScrollbarRange( );
#endif
	Refresh( false );
#if defined HEXTOOLSMAP
	centerHex( x, y );
	Refresh( true ); // erase background
#endif
}

void MapPane::StatusMsg( const char* s )
{
	( (wxFrame*) ( wxGetApp( ).GetTopWindow( ) ) )->SetStatusText( wxString::FromAscii( s ) );
}

// adjust scroll bar thumb sizes
// if no new position is given  ( x < 0, y < 0 ) use current
// TODO: figure out what this magic +3 does
// set scrollbar ranges; y+3 seems to fit nicely
void MapPane::setScrollbarRange( int x, int y )
{
// scroller setup:
// MUST NOT BE CHANGED FROM 32 OR ZOOMED MODES WON'T WORK PROPERLY!
#define SCROLLSTEPS_PER_HEX 32 // HexTools-play says 32 for zoomability, was 8

	if ( x < 0 || y < 0 )
		GetViewStart( &x, &y );

	x = static_cast<int>( x / scale_ );
	y = static_cast<int>( y / scale_ );
	SetScrollbars(	Hex::SIZE_X / SCROLLSTEPS_PER_HEX,		// x step
					Hex::SIZE_Y / SCROLLSTEPS_PER_HEX,		// y step
					static_cast<int>( ( world_x_size_ * SCROLLSTEPS_PER_HEX ) / scale_ ),		// x range
					static_cast<int>( ( world_y_size_ * SCROLLSTEPS_PER_HEX + 3 ) / scale_ ),	// y range
					x,
					y,
					true ); // noRefresh
}

#if defined HEXTOOLSPLAY

// Add more info to die roll
void MapPane::DieRoll( int numdie, int diemax )
{
	DAirCombatDice ddlg( this );
	ddlg.SetHelpText( const_cast<char*>( "Die Roll: " ) );
	ddlg.SetNoOfDies( numdie );
	ddlg.SetDieMax( diemax );

	if ( ddlg.ShowModal( ) == wxID_OK )
	{
		ddlg.CloseDialog( );
		// log the die roll results

		char rep[1000];
		strcpy( rep, ddlg.GetInfo( ) );
		strcat( rep, ddlg.WasAuto( ) ? "\nComputer generated" : "\nManual Die Roll" );

		sprintf( msg, "\nFirst D%d = %d", diemax, ddlg.GetDie1( ) );
		strcat( rep, msg );
		if ( numdie > 1 )
		{
			sprintf( msg, "\nSecond D%d = %d", diemax, ddlg.GetDie2( ) );
			strcat( rep, msg );
			sprintf( msg, "\nTotal 2D%d  = %d", diemax, ddlg.GetDie1( ) + ddlg.GetDie2( ) );
			strcat( rep, msg );
		}

		HexNote* dieroll_report = new HexNote( HexNote::DIEROLLREPORT );
		if ( ! dieroll_report )
		{
			wxMessageBox( wxT("Panic: memory allocation failed, please exit and buy more memory"), Application::NAME, wxICON_HAND );
			return;
		}

		dieroll_report->setCaption( "Die Roll report" );
		dieroll_report->setText( rep );
		insertUnitToCurrentHex( dieroll_report );
		sit_[selected_hex_point_.y][selected_hex_point_.x].updateCaches( 0, _selected_unit_list );
		paintCurrentHex( );
		contents_pane_->addUnit( dieroll_report );
		contents_pane_->Refresh( );
		file_is_dirty_ = true;
	}
	else
	{
		ddlg.CloseDialog( );
	}
}

void MapPane::ViewReplPool( )
{
	DReplacementPool dlg( this );
	if ( dlg.ShowModal( ) != wxID_OK )
		return;

	GroundUnit* c = (GroundUnit*)dlg.GetSelCntr( );
	if ( ! c )
		return;

	if ( ! isHexSelected( ) )
	{
		wxMessageBox( wxT("You haven't selected hex, can't restore unit") );
		return;
	}

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	if ( IsEnemyOccupied( x, y ) )
	{
		wxMessageBox( wxT("You can't rebuild units to enemy occupied hex") );
		return;
	}

	sit_[y][x].setOwner( _current_player );
	insertUnitToCurrentHex( c ); // also sets dirty to true

	// 'reset' unit:
	c->setHomeHex( x, y );
	c->clearReplay( );
	c->setRailRem( 0 );
	c->setTacticalMove( false );
	c->setStratMove( false );
	c->setAdminMove( FALSE );
	c->setHalfMP( FALSE );

	sit_[y][x].setOwner( c->getSide( ) );
	_repl_pool[_current_player].remove( c );
	paintCurrentHex( );
	contents_pane_->addUnit( c );
	contents_pane_->Refresh( );
}

void MapPane::ViewAirReplPool( )
{
	DReplacementPool dlg( this );
	dlg.SetAirMode( TRUE );

	if ( dlg.ShowModal( ) != wxID_OK )
		return;

	AirUnit* c = (AirUnit*)dlg.GetSelCntr( );
	if ( ! c )
		return;

	if ( ! isHexSelected( ) )
	{
		wxMessageBox( wxT("You haven't selected hex, can't restore unit") );
		return;
	}

	insertUnitToCurrentHex( c ); // also sets dirty to true
	_air_repl_pool[_current_player].remove( c );
	paintCurrentHex( );

	contents_pane_->addUnit( c );
	contents_pane_->Refresh( );
}

void MapPane::Statistics( int side )
{
	int gcnt = 0, gatt = 0, gdef = 0, gaa = 0;
	float re = 0, art_re = 0, cm_re = 0, cm_a = 0, cm_d = 0;
	int art_a = 0, art_d = 0;
	int acnt = 0, aatt = 0, adef = 0, atac = 0, astr = 0;

	// any phase:
	for ( int x = 0; x < world_x_size_; ++x )
		for ( int y = 0; y < world_y_size_; ++y )
		{
			if ( hex_[y][x].terrain_ == HexType::NONE )  // don't count units in 'none' hexes,
				continue;// because they are not in the game
			Counter* cntr = sit_[y][x].unit_list_;
			while ( cntr )
			{
				if ( cntr->getSide( ) != side )
				{
					cntr = cntr->getNext( );
					continue;
				}
				switch ( cntr->getCounterType( ) )
				{
					case Counter::Type::UNIT:
					{
						GroundUnit* up = (GroundUnit*)cntr;
						++gcnt;
						gatt += up->getAtt( );
						gdef += up->getDef( );
						gaa += up->getFlak( );
						re += up->getRE( );
						if ( up->isSupplementalMotorized( ) )
							cm_re += up->getRE( ), cm_a += up->getAtt( ), cm_d += up->getDef( );
						else if ( up->isArtillery( ) )
							art_re += up->getRE( ), art_a += up->getAtt( ), art_d += up->getDef( );
						break;
					}
					case Counter::Type::PLANE:
					{
						AirUnit* au = (AirUnit*)cntr;
						++acnt;
						aatt += au->getAtt( );
						adef += au->getDef( );
						atac += au->getTac( );
						astr += au->getStrat( );
						break;
					}
					default:
						break;
				}
				cntr = cntr->getNext( );
			}  // while cntr
		}  // for y

	if ( side == SidePlayer::AXIS )
		sprintf( msg, "Axis Statistics:\n\n" );
	else if ( side == SidePlayer::ALLIED )
		sprintf( msg, "Allied Statistics:\n\n" );

	char tmp[100];
	sprintf( tmp, "Number of ground units: %d\n", gcnt );
	strcat( msg, tmp );
	sprintf( tmp, "Total attack strength: %d\n", gatt );
	strcat( msg, tmp );
	sprintf( tmp, "Total defense strength: %d\n", gdef );
	strcat( msg, tmp );
	sprintf( tmp, "Total AA strength: %d\n", gaa );
	strcat( msg, tmp );
	sprintf( tmp, "Total RE's: %.0f\n", re );
	strcat( msg, tmp );
	sprintf( tmp, "c/m RE's: %.0f (att: %.0f, def: %.0f)\n", cm_re, cm_a, cm_d );
	strcat( msg, tmp );
	sprintf( tmp, "Artillery RE's: %.0f (att: %d, def: %d)\n", art_re, art_a, art_d );
	strcat( msg, tmp );
	strcat( msg, "\n" );
	sprintf( tmp, "Number of air groups: %d\n", acnt );
	strcat( msg, tmp );
	sprintf( tmp, "Total attack strength: %d\n", aatt );
	strcat( msg, tmp );
	sprintf( tmp, "Total defense strength: %d\n", adef );
	strcat( msg, tmp );
	sprintf( tmp, "Total tactical bombing strength: %d\n", atac );
	strcat( msg, tmp );
	sprintf( tmp, "Total strategic bombing strength: %d\n", astr );
	strcat( msg, tmp );
	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
}

// losses
// TODO: per nationality
void MapPane::ViewLosses( int /*side*/ )  //both sides at the same time
{
	int ax_l = 0, ax_il = 0, al_l = 0, al_il = 0;
	int sov_infrp = 0, sov_armrp = 0, sov_artrp = 0;  // Track Soviet Special Replacement Points
	char tmp[1000];

	for ( int y = _year - 9; y < _year + 1; ++y )
		for ( int m = 0; m < 12; ++m )
			for ( int t = 0; t < 2; ++t )
			{
				ax_l += unisol_losses_[y][m][t][SidePlayer::AXIS];
				ax_il += isol_losses_[y][m][t][SidePlayer::AXIS];
				al_l += unisol_losses_[y][m][t][SidePlayer::ALLIED];
				al_il += isol_losses_[y][m][t][SidePlayer::ALLIED];
			}

	// Track Soviet Special Replacement Points
	for ( int y = _year - 9; y < _year + 1; ++y )
		for ( int m = 0; m < 12; ++m )
			for ( int t = 0; t < 2; ++t )
			{
				//                sov_infrp += SovSpecialInfRP[y][m][t];
				//                sov_armrp += SovSpecialArmRP[y][m][t];
				//                sov_artrp += SovSpecialArtRP[y][m][t];
				sov_infrp += unisol_losses_[ ( y + 40 ) % 100][m][t][SidePlayer::ALLIED];
				sov_armrp += unisol_losses_[ ( y + 50 ) % 100][m][t][SidePlayer::ALLIED];
				sov_artrp += unisol_losses_[ ( y + 60 ) % 100][m][t][SidePlayer::ALLIED];
			}

	sprintf( msg, "Axis this turn losses:\nNon-isolated: %d\nIsolated: %d\n\n"
			"Axis total losses:\nNon-isolated: %d\nIsolated: %d\n\n\n",
			unisol_losses_[_year][_month][_turn][SidePlayer::AXIS],
			isol_losses_[_year][_month][_turn][SidePlayer::AXIS],
			ax_l,
			ax_il );

	sprintf( tmp, "Allied this turn losses:\nNon-isolated: %d\nIsolated: %d\n\n"
			"Allied total losses:\nNon-isolated: %d\nIsolated: %d"
			"Soviet this turn losses generating special RP:\nInfantry: %d\nArmour: %d\nArtillery: %d\n\n\n"
			"Soviet total losses generating special RP:\nInfantry: %d\nArmour: %d\nArtillery: %d\n\n\n",
			unisol_losses_[_year][_month][_turn][SidePlayer::ALLIED],
			isol_losses_[_year][_month][_turn][SidePlayer::ALLIED],
			al_l,
			al_il,
			unisol_losses_[ ( _year + 40 ) % 100][_month][_turn][SidePlayer::ALLIED],
			unisol_losses_[ ( _year + 50 ) % 100][_month][_turn][SidePlayer::ALLIED],
			unisol_losses_[ ( _year + 60 ) % 100][_month][_turn][SidePlayer::ALLIED],
			sov_infrp,
			sov_armrp,
			sov_artrp );
	// Track Soviet Special Replacement Points
	//            SovSpecialInfRP[Year][Month][Turn],
	//            SovSpecialArmRP[Year][Month][Turn],
	//            SovSpecialArtRP[Year][Month][Turn],

	strcat( msg, tmp );
	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
}

void MapPane::analyzeGroundCombatReports( )
{
	int s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0;
	double sum = 0, hsum = 0;
	int cnt = 0;

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
		{
			Counter* c;
			ENUM_HEXUNITS(x,y,c)
			{
				if ( c->getCounterType( ) == Counter::Type::NOTE && ( (HexNote*)c )->getNoteType( ) == HexNote::GNDREPORT )
				{
					char* cp = ( (HexNote*)c )->getNoteText( );
					if ( ! cp )	// should never happen
						continue;
					cp = strstr( cp, "Die roll:" );
					if ( ! cp )	// should never happen
						continue;
					cp += 9;
					int roll = strtol( cp, 0, 10 );
					switch ( roll )
					{
						case 1:
							++s1;
							break;
						case 2:
							++s2;
							break;
						case 3:
							++s3;
							break;
						case 4:
							++s4;
							break;
						case 5:
							++s5;
							break;
						case 6:
							++s6;
							break;
						default:
							sprintf( msg, "Illegal die roll detected: %d", roll );
							wxMessageBox( wxS( msg ), Application::NAME );
							break;
					}
					sum += roll;
					hsum += pow( ( static_cast<double>( roll ) ) - 3.5, 2 );
					++cnt;
				}
			}
		}  // for x

	if ( cnt == 0 )
	{
		wxMessageBox( wxT("No combat reports found, nothing to calculate"), Application::NAME );
		return;
	}

	double avg = sum / (double)cnt;
	sprintf( msg, "%d combat die rolls.\n"
			"Average: %.3lf (should be ~3.5)\n"
			"1's: %d\n"
			"2's: %d\n"
			"3's: %d\n"
			"4's: %d\n"
			"5's: %d\n"
			"6's: %d\n"
			"Total %d die rolls",
			cnt, avg, s1, s2, s3, s4, s5, s6, cnt );

	wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_INFORMATION );
}

void MapPane::paintCurrentHex( void )
{
	if ( selected_hex_point_.x < 0 )
	{
		wxMessageBox( wxT("No hex selected!"), wxT("MapPane::paintCurrentHex") );
		return;
	}

	PaintHex( selected_hex_point_.x, selected_hex_point_.y );
}

void MapPane::cancel( )						// deselect units in HexContentsPane
{
	if ( ! isHexSelected( ) || ! _selected_unit_list )
		return;

	contents_pane_->deselectAll( );
}

void MapPane::updateHexCache( int x, int y )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return;

	// calculate stack's strengths
	bool att = false;

	if ( sit_[y][x].getOwner( ) == SidePlayer::AXIS && _cache_axis_attack_strengths )
		att = true;
	else if ( sit_[y][x].getOwner( ) == SidePlayer::ALLIED && _cache_allied_attack_strengths )
		att = true;

	float strength = 0;
	Counter* c;
	if ( att )
	{
		ENUM_HEXUNITS(x, y, c)
		{
			if ( c->getCounterType( ) != Counter::Type::UNIT )
				continue;
			strength += c->getRealAtt( -1 );
		}
		if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
			ENUM_SELUNITS(c)
			{
				if ( c->getCounterType( ) != Counter::Type::UNIT )
					continue;
				strength += c->getRealAtt( -1 );
			}
	}
	else // def
		strength = CalcDefStr( x, y );

	if ( x == selected_hex_point_.x && y == selected_hex_point_.y )
		sit_[y][x].updateCaches( (int)strength, _selected_unit_list );
	else
		sit_[y][x].updateCaches( (int)strength, nullptr );
}

void MapPane::HiLiteHex( int x, int y, bool is_red )  // highlight hex
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

	if ( is_red )
		map_DC_->SetPen( *hpenHiliteRed );
	else
	{
		map_DC_->SetPen( *hpenHiliteBlack );
		// kludge #2232: save coordinates for Paint()
		highlighted_hex_x_ = x;
		highlighted_hex_y_ = y;
	}
	map_DC_->DrawLine( x2, y2 + 3, x2 + Hex::HALF, y2 - 3 );
	map_DC_->DrawLine( x2 + Hex::HALF, y2 - 3, x2 + Hex::SIZE_X, y2 + 3 );
	map_DC_->DrawLine( x2 + Hex::SIZE_X, y2 + 3, x2 + Hex::SIZE_X, y2 + Hex::SIZE_Y - 3 );
	map_DC_->DrawLine( x2 + Hex::SIZE_X, y2 + Hex::SIZE_Y - 3, x2 + Hex::HALF, y2 + Hex::SIZE_Y + 3 );
	map_DC_->DrawLine( x2 + Hex::HALF, y2 + Hex::SIZE_Y + 3, x2, y2 + Hex::SIZE_Y - 3 );
	map_DC_->DrawLine( x2, y2 + Hex::SIZE_Y - 3, x2, y2 + 3 );
}

// simply repaint
void MapPane::UnHiLiteHex( void )
{
	wxClientDC hdc( this );
	hdc.SetUserScale( 1.0 / scale_, 1.0 / scale_ );

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;
	hex2Screen( &x, &y );

	// overwrite hilite with white, this is needed in map borders and in
	// NONE hexes
	hdc.SetPen( *hpenHiliteWhite );
	hdc.DrawLine( x, y + 3, x + Hex::HALF, y - 3 );
	hdc.DrawLine( x + Hex::HALF, y - 3, x + Hex::SIZE_X, y + 3 );
	hdc.DrawLine( x + Hex::SIZE_X, y + 3, x + Hex::SIZE_X, y + Hex::SIZE_Y - 3 );
	hdc.DrawLine( x + Hex::SIZE_X, y + Hex::SIZE_Y - 3, x + Hex::HALF, y + Hex::SIZE_Y + 3 );
	hdc.DrawLine( x + Hex::HALF, y + Hex::SIZE_Y + 3, x, y + Hex::SIZE_Y - 3 );
	hdc.DrawLine( x, y + Hex::SIZE_Y - 3, x, y + 3 );

	// re-paint rect in order to remove hilite
	x = (int) ( x / scale_ );
	y = (int) ( y / scale_ );

	wxRect rect;
	rect.x = x - Hex::SIZE_X;
	rect.width = 3 * Hex::SIZE_X;
	rect.y = y - Hex::SIZE_Y;
	rect.height = 3 * Hex::SIZE_Y;
	highlighted_hex_x_ = highlighted_hex_y_ = -1;
	Refresh( false, &rect );
}

void MapPane::SelectHex( int x, int y )
{
	// new hex selected, get rid of old selections
	contents_pane_->deselectAll( );
	contents_pane_->clear( );
	UnHiLiteHex( );

	centerHex( x, y );

	selected_hex_point_.x = x;
	selected_hex_point_.y = y;

	//UnitWin->AddAirUnits(Sit[y][x].AirUnitPtr);
	contents_pane_->addUnits( sit_[y][x].unit_list_ );

	HiLiteHex( x, y );
}

// re-draw one hex
void MapPane::PaintHex( int x, int y )
{
	bool cur_hex = ( selected_hex_point_.x == x && selected_hex_point_.y == y );

	updateHexCache( x, y );

	int x2 = x;
	int y2 = y;
	hex2Screen( &x2, &y2 );

	wxRect rect;
	rect.y = y2 - 1;  //Hex::SIZE_Y;
	rect.height = ( Hex::SIZE_Y + 2 );
	rect.x = x2 - 1;  //Hex::SIZE_X;
	rect.width = ( Hex::SIZE_Y + 2 );

	Refresh( false, &rect );

	if ( ! drawing_path_ && _show_path && cur_hex )
		PaintPaths( );
}

#if defined HEXTOOLSDEBUG
void MapPane::dump_cur_hex_units( void )
{
	strcpy( msg, "" );

	Counter* unit = sit_[selected_hex_point_.y][selected_hex_point_.x].UnitPtr;
	char tmp[100];
	int i = 0;
	while ( unit )
	{
		unit->getUnitString( tmp );
		strcat( msg, tmp );
		strcat( msg, " -->\n" );
		unit = unit->getNext( );
		++i;
	}

	strcat( msg, "NULL" );
	sprintf( tmp, "%d units in selected hex", i );
	wxMessageBox( wxS( msg ), wxS( tmp ) );
}
#endif // HEXTOOLSDEBUG

#endif // HEXTOOLSPLAY
