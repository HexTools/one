#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "sideplayer.h"
#include "phase.h"
#include "counter.h"
#include "dreplacementpool.h"
#include "groundunitreplpool.h"
#include "airunitreplpool.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "airunit.h"

enum
{
	IDC_SCROLLBAR1 = 1,
	IDC_STATICFRAME7,
	IDC_BTNREBUILD,
	IDC_BTNREMOVE,
	IDC_LBLINFO,
	IDC_BTNADD,
	IDC_BTNEDIT
};

BEGIN_EVENT_TABLE(DReplacementPool, wxDialog)
EVT_BUTTON(IDC_BTNREBUILD, DReplacementPool::RebuildClicked)
EVT_BUTTON(IDC_BTNREMOVE, DReplacementPool::RemoveClicked)
EVT_BUTTON(IDC_BTNADD, DReplacementPool::AddClicked)
EVT_BUTTON(IDC_BTNEDIT, DReplacementPool::EditClicked)
EVT_COMMAND_SCROLL(IDC_SCROLLBAR1, DReplacementPool::EvVScroll)
EVT_LEFT_DOWN(DReplacementPool::EvLButtonDown)
EVT_INIT_DIALOG(DReplacementPool::EvInitDialog)
EVT_PAINT(DReplacementPool::Paint)
END_EVENT_TABLE()

extern int _current_player, _current_phase;

extern GroundUnitReplPool _repl_pool[];  // AXIS and ALLIED
extern AirUnitReplPool _air_repl_pool[];  // AXIS and ALLIED

static int prev_yzero = 0;

DReplacementPool::DReplacementPool( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Replacement Pool"), wxDefaultPosition )
{
	const int W = 380, H = 400;
// windows seems to calculate titlebat to height...
#ifdef __WXMSW__
#define MSW_H_FIX 30
#else
#define MSW_H_FIX 0
#endif
	lblInfo = new wxStaticText( this, IDC_LBLINFO, wxT(""), wxPoint( 10, H - 60 ), wxDefaultSize, 0 );
	lblInfo2 = new wxStaticText( this, IDC_LBLINFO, wxT(""), wxPoint( 10, H - 45 ), wxDefaultSize, 0 );

	Scroller = new wxScrollBar( this, IDC_SCROLLBAR1, wxPoint( W - 20, 1 ), wxSize( -1, 360 ), wxSB_VERTICAL );

	btnRebuild = new wxButton( this, IDC_BTNREBUILD, wxT("Rebuild"), wxPoint( 10, H - 30 ), wxSize( 60, -1 ), 0 );
	btnRemove = new wxButton( this, IDC_BTNREMOVE, wxT("Delete"), wxPoint( 80, H - 30 ), wxSize( 60, -1 ), 0 );
	btnAdd = new wxButton( this, IDC_BTNADD, wxT("Add Unit"), wxPoint( 150, H - 30 ), wxSize( 60, -1 ), 0 );
#if 0
	btnRebuild = new wxButton( this, IDC_BTNREBUILD, wxT("Rebuild Unit"), wxPoint(10,H-30), wxSize(80,-1), 0 );
	btnRemove = new wxButton( this, IDC_BTNREMOVE, wxT("Delete Unit"), wxPoint(100,H-30), wxSize(80,-1), 0 );
	btnAdd = new wxButton( this, IDC_BTNADD, wxT("Add Unit..."), wxPoint(190,H-30), wxSize(80,-1), 0 );
#endif
	// Allow editing of units in the Replacement Pool
	btnEdit = new wxButton( this, IDC_BTNEDIT, wxT("Edit Unit"), wxPoint( 220, H - 30 ), wxSize( 60, -1 ), 0 );

	new wxButton( this, wxID_CANCEL, wxT("Close"), wxPoint( 280, H - 30 ), wxSize( 80, -1 ), 0 );
	wxWindow::SetSize( W, H + MSW_H_FIX);
	Center( );

	y0_ = prev_yzero;
	selected_index_ = -1;
	selected_counter_ = NULL;
	air_mode_ = FALSE;
}

DReplacementPool::~DReplacementPool( )
{
}

void DReplacementPool::SetScrollBarRange( )
{
	int p = Scroller->GetThumbPosition( );

	int cnt;
	if ( air_mode_ )
		cnt = _air_repl_pool[_current_player].count( ) / 10;
	else
		cnt = _repl_pool[_current_player].count( ) / 10;

	++cnt;

	if ( cnt < 10 ) 
		cnt = 10;

	Scroller->SetScrollbar( p, 10,          // visible rows
							cnt,         // total rows
							5,   	// page size
							false );
#if 0
	Scroller->SetScrollbar( 0, 100, 1000, 100 );
#endif
}

void DReplacementPool::EvInitDialog( wxInitDialogEvent& WXUNUSED(e) )
{
	if ( _current_player == SidePlayer::AXIS )
	{
		if ( air_mode_ )
			SetTitle( wxT("Axis air replacement pool") );
		else
			SetTitle( wxT("Axis replacement pool") );
	}
	else if ( _current_player == SidePlayer::ALLIED )
	{
		if ( air_mode_ )
			SetTitle( wxT("Allied air replacement pool") );
		else
			SetTitle( wxT("Allied replacement pool") );
	}
	else
	{
		if ( air_mode_ )
			SetTitle( wxT("Neutral air replacement pool") );
		else
			SetTitle( wxT("Neutral replacement pool") );
	}
	Scroller->SetThumbPosition( 0 );
	SetScrollBarRange( );
	y0_ = 0;
	if ( _current_phase != Phase::INITIAL_PHASE && _current_phase != Phase::SETUP_PHASE )
	{
#if 0
		btnRebuild->EnableWindow( false );
		btnRemove->EnableWindow( false );
#endif
	}
}

void DReplacementPool::CloseDialog( )
{
	// nothing to do
}

void DReplacementPool::EvVScroll( wxScrollEvent& WXUNUSED(e) )
{
	prev_yzero = y0_ = Scroller->GetThumbPosition( );

	wxRect r;
	r.x = 0;
	r.y = 0;
	r.width = 350;
	r.height = 320;

	Refresh( true, &r );  	// lazy man's way, blinks a lot
}

void DReplacementPool::Paint( wxPaintEvent& WXUNUSED(e) )
{
	Redraw( );
}

#define UNTS_PER_LINE 10
#define YZERO 14
#define XZERO 20
void DReplacementPool::Redraw( )
{
	wxPaintDC dc( this );

	int cnt;
	if ( air_mode_ )
		cnt = _air_repl_pool[_current_player].count( );
	else
		cnt = _repl_pool[_current_player].count( );

	char s[500];

	for ( int i = 0; i < cnt; ++i )
	{
		Counter* c;
		if ( air_mode_ )
			c = _air_repl_pool[_current_player].get( i );
		else
			c = _repl_pool[_current_player].get( i );

		if ( ! c )
		{	// sanity check
			sprintf( s, "corrupted replacement pool %d entry: %d/%d", _current_player, i, cnt );
			wxMessageBox( wxString::FromAscii( s ), wxString::FromAscii( "HexTools Defect" ) );
			break;
		}
		int x = XZERO + ( ( i % UNTS_PER_LINE ) * 30 );
		int y = ( YZERO + ( i / UNTS_PER_LINE ) * 30 ) - ( ( y0_ ) * 30 );
		if ( y >= YZERO && y < 300 )
		{
			if ( i == selected_index_ )
			{
				dc.SetBrush( *wxBLACK_BRUSH );
				dc.DrawRectangle( x - 2, y - 2, 32, 31 );
			}
			c->drawCounter( &dc, x, y );
		}
	}
}

void DReplacementPool::RebuildClicked( wxCommandEvent& WXUNUSED(e) )
{
	if ( selected_index_ < 0 )
	{
		wxMessageBox( wxT("Select unit to rebuild first!"), Application::NAME );
#if 0
		// dltx print replacement pool to a file

		int i;
		Counter* c;
		char s[1000];
		ofstream os;
		os.open("d:/ReplPool.txt");
		if ( ! os )
		{
			sprintf(s, "Couldn't open file \"%s\"!\n\nSave aborted.", "d:/ReplPool.txt");
			wxMessageBox(wxS(s), Application::NAME);
			return;
		}
		os << (int)ReplPool[Player].Count() << endl;
		for ( i = 0; i < ReplPool[Player].Count(); ++i )
		{
			c = ReplPool[Player].Get( i );
			os << c->GetAtt( );
			os << "-";
			os << c->GetDef( );
			os << "-";
			os << c->GetMaxMP( );
			os << " ";
			c->GetUnitString( s );
			os << s;
			os << endl;
		}

		// dltx print replacement pool to a file
#endif
		return;
	}

	if ( air_mode_ )
		selected_counter_ = _air_repl_pool[_current_player].get( selected_index_ );
	else
		selected_counter_ = _repl_pool[_current_player].get( selected_index_ );
	EndModal( wxID_OK );
}

void DReplacementPool::RemoveClicked( wxCommandEvent& WXUNUSED(e) )
{
	if ( selected_index_ < 0 )
		return;

	if ( wxMessageBox( wxT("This will permanently remove selected unit, are you sure?"), Application::NAME, wxYES_NO ) != wxYES )
		return;

	if ( air_mode_ )
		_air_repl_pool[_current_player].remove( selected_index_ );
	else
		_repl_pool[_current_player].remove( selected_index_ );

	selected_index_ = -1;
	SetScrollBarRange( );
	Refresh( );
}

void DReplacementPool::EditClicked( wxCommandEvent& WXUNUSED(e) )
{
	if ( selected_index_ < 0 )
		return;

	if ( air_mode_ )
		_air_repl_pool[_current_player].get( selected_index_ )->edit( );
	else
		_repl_pool[_current_player].get( selected_index_ )->edit( );

	Refresh( );
}

void DReplacementPool::EvLButtonDown( wxMouseEvent& e )
{
	char s[500];

	int x = e.m_x;
	int y = e.m_y;

	int line = ( ( y - YZERO ) / 30 ) + Scroller->GetThumbPosition( );
	int xpos = ( x - XZERO ) / 30;
	int idx = line*  UNTS_PER_LINE + xpos;

	if ( idx >= 0 || idx < _repl_pool[_current_player].count( ) )
		selected_index_ = idx;
	else
		selected_index_ = -1;

	Counter* u;
	if ( air_mode_ )
	{
		u = _air_repl_pool[_current_player].get( idx );
#if 0
		yy = AirReplPool[Player].getYear( idx );
		mm = AirReplPool[Player].getMonth( idx );
		tt = AirReplPool[Player].getTurn( idx );
#endif
	}
	else
	{
		u = _repl_pool[_current_player].get( idx );
#if 0
		yy = ReplPool[Player].getYear( idx );
		mm = ReplPool[Player].getMonth( idx );
		tt = ReplPool[Player].getTurn( idx );
#endif
	}

	if ( u )
		u->getUnitString( s );
	else
		strcpy( s, "" );

	lblInfo->SetLabel( wxString::FromAscii( s ) );
#if 0
	sprintf( s, "Destroyed: %s %s %d", months[mm], turns[tt], yy );
	lblInfo2->SetLabel( s );
#endif
	wxRect r;
	r.x = 0;
	r.y = 0;
	r.width = 350;
	r.height = 370;

	Refresh( true, &r );  	// lazy man's way, blinks a lot
}

void DReplacementPool::AddClicked( wxCommandEvent& WXUNUSED(e) )
{
	if ( air_mode_ )
	{
		AirUnit* au = new AirUnit( );
		au->setSide( _current_player );
		if ( ! au->edit( ) )
			delete au;
		else
			_air_repl_pool[_current_player].add( au );
	}
	else
	{
		GroundUnit* u = new GroundUnit( );
		u->setSide( _current_player );
		if ( ! u->edit( ) )
			delete u;
		else
			_repl_pool[_current_player].add( u );
	}
	SetScrollBarRange( );
	Refresh( );
}

#endif
