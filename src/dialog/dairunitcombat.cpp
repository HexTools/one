#if defined HEXTOOLSPLAY

// This file contains air combat implementation, air CRT, air combat
// die modifier calculation etc.

#include <algorithm>
using std::max;

#include <wx/wx.h>

#include "application.h"
#include "aircombatresult.h"
#include "sideplayer.h"
#include "rules.h"
#include "counter.h"
#include "hextools.h"
using ht::dice;
using ht::wxS;
using ht::crt2str;
#include "dairunitcombat.h"
#include "airunitsubtype.h"
#include "airunittype.h"
#include "airunit.h"
#include "daircombatdice.h"

extern Rules _rule_set;
extern int _year;	// 42 etc...
extern int _month;	// 0...11  (0=Jan, 11=Dec, used as on index)

static char msg[1000];

enum
{
	IDC_BTNOK = 1,
	IDC_BTNCANCEL,
	IDC_LBLINFO
};

BEGIN_EVENT_TABLE(DAirUnitCombat, wxDialog)
EVT_INIT_DIALOG(DAirUnitCombat::EvInitDialog)
EVT_SIZE(DAirUnitCombat::EvSize)
EVT_PAINT(DAirUnitCombat::Paint)
EVT_LEFT_DOWN(DAirUnitCombat::EvLButtonDown)
EVT_BUTTON(IDC_BTNCANCEL, DAirUnitCombat::CancelClicked)
EVT_BUTTON(IDC_BTNOK, DAirUnitCombat::OkClicked)
END_EVENT_TABLE()

#define GRID_X 30
#define GRID_Y 30

DAirUnitCombat::DAirUnitCombat( wxWindow* parent )
		:
				wxDialog( parent, -1, wxT("Air Combat"), wxDefaultPosition, wxSize( 400, 400 ),
						wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	btnOk = new wxButton( this, IDC_BTNOK, wxT("Ok") );
	btnCancel = new wxButton( this, IDC_BTNCANCEL, wxT("Cancel") );
	lblInfo = new wxStaticText( this, IDC_LBLINFO, wxT("") );

	num_targets_ = num_escorts_ = num_interceptors_ = 0;
	for ( int i = 0; i < MAX_PLANES; ++i )
	{
		escort_list_[i] = target_list_[i] = interceptor_list_[i] = NULL;
		int_target_[i] = int_target_[i * 2] = -1;
		esc_target_[i] = -1;
	}
	selected_interceptor_ = -1;
	num_killed_planes_ = 0;
	Center( );
	CalcGeometry( );
#if 0
	Refresh( );
#endif
}

DAirUnitCombat::~DAirUnitCombat( )
{
}

void DAirUnitCombat::SetupDialog( )
{
	dc_ = new wxClientDC;
	CalcGeometry( );
}

void DAirUnitCombat::CloseDialog( )
{
	delete dc_;
}

// return no of interceptor clicked (or -1 if none)
int DAirUnitCombat::get_i_idx( int x, int y )
{
	int rows, row, col, i;

	if ( ! ( x > interceptor_x_ && x < ( interceptor_x_ + interceptor_width_ ) && y > interceptor_y_ && y < ( interceptor_y_ + interceptor_height_ ) ) )
		return -1;

	rows = interceptor_height_ / GRID_Y;
	col = ( ( interceptor_x_ + interceptor_width_ ) - x ) / GRID_X;  // grows from right to left!!!
	row = ( y - interceptor_y_ ) / GRID_Y;       // grows from top to bottom
	i = col * rows + row;
	if ( i < num_interceptors_ )
		return i;

	return -1;
}

// return no of escort clicked (or -1 if none)
int DAirUnitCombat::get_e_idx( int x, int y )
{
	int rows, row, col, i;

	if ( ! ( x > escort_x_ && x < ( escort_x_ + escort_width_ ) && y > escort_y_ && y < ( escort_y_ + escort_height_ ) ) )
		return -1;

	rows = escort_height_ / GRID_Y;
	col = ( x - escort_x_ ) / GRID_X;       // grows from right to left!!!
	row = ( y - escort_y_ ) / GRID_Y;       // grows from top to bottom
	i = col * rows + row;
	if ( i < num_escorts_ )
		return i;

	return -1;
}

// return no of target (=bomber) clicked (or -1 if none)
int DAirUnitCombat::get_t_idx( int x, int y )
{
	int rows, row, col, i;

	if ( ! ( x > target_x_ && x < ( target_x_ + target_width_ ) && y > target_y_ && y < ( target_y_ + target_height_ ) ) )
		return -1;

	rows = target_height_ / GRID_Y;
	col = ( x - target_x_ ) / GRID_X;       // grows from right to left!!!
	row = ( y - target_y_ ) / GRID_Y;       // grows from top to bottom
	i = col * rows + row;
	if ( i < num_targets_ )
		return i;

	return -1;
}

// return coordinates for interceptor 'i' (assume it fits)
void DAirUnitCombat::get_i_xy( int i, int* x, int* y )
{
	int rows, row, col;

	if ( i < 0 || i >= num_interceptors_ )
		return;

	rows = interceptor_height_ / GRID_Y;
	if ( ! rows )
		return;

	col = i / rows;
	row = i % rows;
	*x = ( ( interceptor_x_ + interceptor_width_ ) - col * GRID_X ) - GRID_X;  // grows from left to right!!!
	*y = interceptor_y_ + row * GRID_Y;              // grows from top to bottom
}

// return coordinates for escort 'i' (assume it fits)
void DAirUnitCombat::get_e_xy( int i, int* x, int* y )
{
	int rows, row, col;

	if ( i < 0 || i >= num_escorts_ )
		return;

	rows = escort_height_ / GRID_Y;

	if ( ! rows )
		return;

	col = i / rows;
	row = i % rows;
	*x = escort_x_ + col * GRID_X;              // grows from right to left
	*y = escort_y_ + row * GRID_Y;              // grows from top to bottom
}

// return coordinates for mission force plane 'i' (assume it fits)
void DAirUnitCombat::get_t_xy( int i, int* x, int* y )
{
	int rows, row, col;

	if ( i < 0 || i >= num_targets_ )
		return;

	rows = target_height_ / GRID_Y;
	if ( ! rows )
		return;

	col = i / rows;
	row = i % rows;
	*x = target_x_ + col * GRID_X;              // grows from right to left
	*y = target_y_ + row * GRID_Y;              // grows from top to bottom
}

void DAirUnitCombat::EvLButtonDown( wxMouseEvent &e )
{
	int x = e.m_x;
	int y = e.m_y;

	int i;
	if ( ( i = get_i_idx( x, y ) ) >= 0 )
		selected_interceptor_ = i;
	else if ( ( i = get_e_idx( x, y ) ) >= 0 )
	{
		if ( selected_interceptor_ >= 0 )
		{
			int_target_[selected_interceptor_] = i + MAX_PLANES;  // target escort
			Refresh( );
		}
	}
	else if ( ( i = get_t_idx( x, y ) ) >= 0 )
	{
		if ( selected_interceptor_ >= 0 )
		{
			int_target_[selected_interceptor_] = i;						// target bomber
			Refresh( );
		}
	}
}

#if 0
void draw_grid( wxDC* dc, int x0, int y0, int x1, int y1 )
{
	for ( int x = x0; x < x1; x += GRID_X )
		dc->DrawLine( x, y0, x, y1 );

	for ( int y = y0; y < y1; y += GRID_Y )
		dc->DrawLine( x0, y, x1, y );
}
#endif

void DAirUnitCombat::Paint( wxPaintEvent& )
{
	wxPaintDC dc( this );
	int i, x, y, x2, y2, szx, szy;
	wxString wxs;

	PrepareDC( dc );
#if 0
	dc.SelectObject( TBrush( TColor( 255, 0 ,0 ) ) );
	dc.Rectangle( i_x, i_y, i_x + i_w, i_y + i_h );
	draw_grid( &dc, i_x, i_y, i_x + i_w, i_y + i_h );

	dc.Rectangle( e_x, e_y, e_x + e_w, e_y + e_h );
	draw_grid( &dc, e_x, e_y, e_x + e_w, e_y + e_h );

	dc.Rectangle( t_x, t_y, t_x + t_w, t_y + t_h );
	draw_grid( &dc, t_x, t_y, t_x + t_w, t_y + t_h );
	dc.SetBkMode( TRANSPARENT );
#endif
	wxs.Printf( wxT("%s"), "Interceptors" );
	dc.GetTextExtent( wxs, &szx, &szy );
	dc.DrawText( wxs, interceptor_x_ + interceptor_width_ - szx, interceptor_y_ - 20 );
	wxs.Printf( wxT("%s"), "Escorts" );
	dc.DrawText( wxs, escort_x_, escort_y_ - 20 );
	wxs.Printf( wxT("%s"), "Mission force" );
	dc.DrawText( wxs, target_x_, target_y_ - 20 );
#if 0
	dc.SetBkMode( OPAQUE );
#endif
	// render counters
	for ( i = 0; i < num_targets_; ++i )
	{
		get_t_xy( i, &x, &y );
		target_list_[i]->drawCounter( &dc, x + 2, y + 2, 1 );
	}
	for ( i = 0; i < num_interceptors_; ++i )
	{
		get_i_xy( i, &x, &y );
		interceptor_list_[i]->drawCounter( &dc, x + 2, y + 2, 1 );
	}
	if ( num_escorts_ == 0 )
	{
#if 0
		dc.SetBkMode( TRANSPARENT );
#endif
		sprintf( msg, "(no escorts)" );
		dc.DrawText( wxS( msg ), escort_x_, escort_y_ );
#if 0
		dc.SetBkMode( OPAQUE );
#endif
	}
	else
		for ( i = 0; i < num_escorts_; ++i )
		{
			get_e_xy( i, &x, &y );
			escort_list_[i]->drawCounter( &dc, x + 2, y + 2, 1 );
		}
	// draw attack indicators
	dc.SetPen( wxPen( wxColor( 255, 0, 0 ), 2, wxSOLID ) );
	for ( i = 0; i < num_interceptors_; ++i )
		if ( int_target_[i] >= 0 )
		{
			get_i_xy( i, &x, &y );
			if ( int_target_[i] < MAX_PLANES )
				get_t_xy( int_target_[i], &x2, &y2 );
			else
				get_e_xy( int_target_[i] - MAX_PLANES, &x2, &y2 );
			dc.DrawLine( x + 18, y + 14, x2 + 6, y2 + 14 );
			dc.DrawLine( x2 + 6, y2 + 14, x2 + 14, y2 + 14 );
			dc.DrawLine( x2 + 14, y2 + 14, x2 + 10, y2 + 10 );
			dc.DrawLine( x2 + 14, y2 + 14, x2 + 10, y2 + 18 );
		}
}

void DAirUnitCombat::AddTarget( AirUnit* au )
{
	if ( num_targets_ >= MAX_PLANES )
	{
		wxMessageBox( wxT("Too many planes!"), wxT("Internal error!") );
		return;
	}
	target_list_[num_targets_++] = au;
}

void DAirUnitCombat::AddEscort( AirUnit* au )
{
	if ( num_escorts_ >= MAX_PLANES )
	{
		wxMessageBox( wxT("Too many planes!"), wxT("Internal error!") );
		return;
	}
	escort_list_[num_escorts_++] = au;
}

void DAirUnitCombat::AddInterceptor( AirUnit* au )
{
	if ( num_interceptors_ >= MAX_PLANES )
	{
		wxMessageBox( wxT("Too many planes!"), wxT("Internal error!") );
		return;
	}
	interceptor_list_[num_interceptors_++] = au;
}

// set the value 'val' to closest value divisible by 'grid'
static inline int align( int val, int grid )
{
	val += grid / 2;
	return ( val / grid ) * grid;
}

void DAirUnitCombat::CalcGeometry( void )
{
	int szx, szy;

	GetClientSize( &szx, &szy );

	// left
	interceptor_x_ = align( GRID_X, GRID_X );
	interceptor_y_ = align( GRID_Y, GRID_Y );
	interceptor_width_ = align( ( szx / 2 ) - 3 * GRID_X, GRID_X );

	// upper right
	target_x_ = interceptor_x_ + interceptor_width_ + GRID_X;
	target_y_ = interceptor_y_;
	target_width_ = align( szx / 2, GRID_X );
	target_height_ = align( ( szy - 2 * GRID_Y ) / 2, GRID_Y ) - GRID_Y;

	// lower right
	escort_x_ = target_x_;
	escort_y_ = target_y_ + target_height_ + GRID_X;
	escort_width_ = target_width_;
	escort_height_ = target_height_;

	interceptor_height_ = escort_height_ + target_height_ + GRID_Y;  //align(size.cy-50, GRID_Y);

	lblInfo->Move( 20, szy - 50 );
	btnOk->Move( szx / 2 - 80, szy - 30 );
	btnCancel->Move( szx / 2 + 10, szy - 30 );
}

void DAirUnitCombat::EvSize( wxSizeEvent& )
{
	CalcGeometry( );
	Refresh( );
}

void DAirUnitCombat::EvInitDialog( wxInitDialogEvent& )
{
	// nothing to do
}

void DAirUnitCombat::CancelClicked( wxCommandEvent& )
{
	if ( wxMessageBox( wxT("Are you sure you want to abort this air combat now?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) != wxYES )
		return;
	EndModal( wxCANCEL );
}

int DAirUnitCombat::IsEscortAttacked( int n )
{
	for ( int i = 0; i < num_interceptors_; ++i )
		if ( int_target_[i] == ( n + MAX_PLANES ) ) 
			return TRUE;

	return FALSE;
}

int DAirUnitCombat::IsInterceptorAttacked( int n )
{
	for ( int i = 0; i < num_escorts_; ++i )
		if ( esc_target_[i] == n ) 
			return TRUE;

	return FALSE;
}

int DAirUnitCombat::IsAttackingBombers( int n )
{
	if ( int_target_[n] < MAX_PLANES )
		return TRUE;

	return FALSE;
}

static const char* AirCrt2Str( int res )
{
	switch ( res )
	{
		case AirCombatResult::KILL:
			return "shot down";
		case AirCombatResult::ABORT:
			return "aborted";
		case AirCombatResult::RETURN:
			return "returned";
		case AirCombatResult::MISS:
			return "missed";
		case AirCombatResult::AUTOMISS:
			return "didn't fire at";
		default:
			return "bugged";
	}
}

void DAirUnitCombat::GetDiffAndMods( AirUnit* att, AirUnit* def, int* diff, int* mods )
{
	int a = att->getAtt( );
	int d = def->getDef( );

	// ext range: -2 to str (but not below 1
	if ( att->getExtRange( ) )
		a = max( 1, a - 2 );

	if ( def->getExtRange( ) )
		d = max( 1, d - 2 );

	// day planes may not fire at night
	if ( att->getNight( ) && att->getPrefix( ) != AirUnitType::Prefix::N )
		a = 0;

	// fighter in bombing mission: str -= 2
	if ( ( att->getType( ) == AirUnitType::F || att->getType( ) == AirUnitType::HF ) && att->getMission( ) >= AirUnit::Mission::FIRST_BOMBING
		&& att->getMission( ) <= AirUnit::Mission::LAST_BOMBING && ! att->getJettison( ) )
		a -= 2;

	if ( a < 0 )
		a = 0;

	if ( ( def->getType( ) == AirUnitType::F || def->getType( ) == AirUnitType::HF ) && def->getMission( ) >= AirUnit::Mission::FIRST_BOMBING
		&& def->getMission( ) <= AirUnit::Mission::LAST_BOMBING && ! att->getJettison( ) && _rule_set.BombingFBothReduced == TRUE )
		d -= 2;

	if ( d < 0 )
		d = 0;

	if ( a == 0 )    	// if a==0 plane may not fire
		*diff = -99;
	else
		*diff = a - d;

	*mods = GetAirMods( att, def );
}

// initiate combats
void DAirUnitCombat::OkClicked( wxCommandEvent& )
{
	int i, j, k, min, cnt, min_j;
	char s[255];

	for ( i = 0; i < num_interceptors_; ++i )
		if ( int_target_[i] < 0 )
		{
			wxMessageBox( wxT("You must select target for every interceptor\n"
					"before you can continue"), Application::NAME, wxICON_INFORMATION );
			return;
		}

	// assign remaining escorts to attack interceptors
	for ( i = 0; i < num_escorts_; ++i )
		if ( !IsEscortAttacked( i ) )
		{
			// free escort, find target for it, prefer bypassers!
			// primary targets are unattacked bypassers
			for ( j = 0; j < num_interceptors_; j++ )
				if ( !IsInterceptorAttacked( j ) && IsAttackingBombers( j ) ) esc_target_[i] = j;
			// secondary targets are other bypassers
			// now first one is attacked, find one that has fewest attackers!
			min = 100;
			if ( esc_target_[i] < 0 )  // unattacked not found
			{
				min = 200;
				min_j = -1;
				for ( j = 0; j < num_interceptors_; ++j )
					if ( IsAttackingBombers( j ) )
					{
						// count how many escorts are already attacking this plane:
						cnt = 0;
						for ( k = 0; k < num_escorts_; ++k )
							if ( esc_target_[k] == j ) 
								cnt++;

						if ( cnt < min ) 
							min = cnt, min_j = j;
					}
				esc_target_[i] = min_j;
			}
			// if no bypassers then attack any interceptor
			if ( esc_target_[i] < 0 )
			{
				// even assignment:
				min = 200;
				min_j = -1;
				for ( j = 0; j < num_interceptors_; ++j )
				{
					// count how many escorts are already attacking this plane:
					cnt = 0;
					for ( k = 0; k < num_escorts_; ++k )
						if ( esc_target_[k] == j + MAX_PLANES )
							cnt++;

					if ( cnt < min ) 
						min = cnt, min_j = j;
				}
				esc_target_[i] = min_j + MAX_PLANES;
#if 0
				//random assignment:
				j = 0;
				while ( 1 )
				{
					if ( rand( ) % 100 < 5 ) // 5% chance
					{
						EscTargets[i] = j + MAX_PLANES; // MAX_PLANES -> interceptor
						break;
					}
					j = ( j + 1 ) % Ints;
				}
#endif
			}
			// TODO: let user decide if excess escorts should attack bypassers or
			//       interceptors that are fighting escorts!
		}

	// tell user how planes
	sprintf( msg, "Unattacked escorts were assigned following way:\n" );
	j = 0;
	for ( i = 0; i < num_escorts_; ++i )
	{
		if ( !IsEscortAttacked( i ) )
		{
			if ( esc_target_[i] >= MAX_PLANES )
				sprintf( s, "%s fires %s #%d (attacking escorts)\n", escort_list_[i]->getName( ),
						interceptor_list_[esc_target_[i] - MAX_PLANES]->getName( ), esc_target_[i] - MAX_PLANES);
			else
				sprintf( s, "%s fires %s #%d (trying to bypass escorts)\n", escort_list_[i]->getName( ),
						interceptor_list_[esc_target_[i]]->getName( ), esc_target_[i] );
			strcat( msg, s );
			++j;
		}

	}
	if ( j )  // some unassigned escorts
		wxMessageBox( wxS( msg ), wxT("Info") );

	// first escorts attack bypassing interceptors
	for ( i = 0; i < num_escorts_; ++i )
		if ( esc_target_[i] >= 0 && esc_target_[i] < MAX_PLANES && CanBeAttacked( interceptor_list_[esc_target_[i]] ) )
		{
			FightAirCombat( escort_list_[i], interceptor_list_[esc_target_[i]], TRUE );
			if ( interceptor_list_[esc_target_[i]]->getMission( ) == AirUnit::Mission::NONE )
				int_target_[esc_target_[i]] = -1;       // hit, won't fire at the bombers
		}
	// then interceptors attack escorts
	for ( i = 0; i < num_interceptors_; ++i )
		if ( int_target_[i] >= 0 && int_target_[i] >= MAX_PLANES && CanBeAttacked( escort_list_[int_target_[i] - MAX_PLANES] ) )
		{
			FightAirCombat( interceptor_list_[i], escort_list_[int_target_[i] - MAX_PLANES] );
			int_target_[i] = -1;
		}

	// then extra escorts attack interceptors that are attacking other escorts
	for ( i = 0; i < num_escorts_; ++i )
		if ( esc_target_[i] >= MAX_PLANES && CanBeAttacked( interceptor_list_[esc_target_[i] - MAX_PLANES] ) )
		{
			esc_target_[i] -= MAX_PLANES;
			FightAirCombat( escort_list_[i], interceptor_list_[esc_target_[i]], TRUE );
			if ( interceptor_list_[esc_target_[i]]->getMission( ) == AirUnit::Mission::NONE )
				int_target_[esc_target_[i]] = -1;       // hit, won't fire at the bombers
		}

	// last interceptors attack bombers
	for ( i = 0; i < num_interceptors_; ++i )
		if ( interceptor_list_[i]->getMission( ) == AirUnit::Mission::INTERCEPT &&  // not returned/aborted
			int_target_[i] >= 0 && int_target_[i] < MAX_PLANES && CanBeAttacked( target_list_[int_target_[i]] ) )
		{
			FightAirCombat( interceptor_list_[i], target_list_[int_target_[i]] );
			int_target_[i] = -1;
		}

	// these planes can't participate in any future air combat in this phase
	for ( i = 0; i < num_interceptors_; ++i )
	{
		interceptor_list_[i]->setFlag( AirUnit::FOUGHT );
		interceptor_list_[i]->setMP( interceptor_list_[i]->getMaxMP( ) / 2 );
	}
	for ( i = 0; i < num_escorts_; ++i )
	{
		escort_list_[i]->setFlag( AirUnit::FOUGHT );
		if ( escort_list_[i]->getExtRange( ) )
			escort_list_[i]->setMP( escort_list_[i]->getMaxMP( ) * 2 );
		else
			escort_list_[i]->setMP( escort_list_[i]->getMaxMP( ) );
	}
	for ( i = 0; i < num_targets_; ++i )
	{
		target_list_[i]->setFlag( AirUnit::FOUGHT );
		if ( target_list_[i]->getExtRange( ) )
			target_list_[i]->setMP( target_list_[i]->getMaxMP( ) * 2 );
		else
			target_list_[i]->setMP( target_list_[i]->getMaxMP( ) );
	}
	EndModal( wxOK );
}

// * sf: unit can receive only one result, so:
//   has unit already been damaged/returned in air combat?
// * fite/se: unit can always be attacked
bool DAirUnitCombat::CanBeAttacked( AirUnit* au )
{
	for ( int i = 0; i < num_killed_planes_; ++i )
		if ( au == killed_plane_list_[i] )	  // shot down already
			return false;

	if ( _rule_set.EndAirCombatAtFirstResult == FALSE )  // fite rules: let effects compound
		return true;

	if ( au->getAborted( ) )            	  // aborted
		return false;

	if ( au->getMission( ) == AirUnit::Mission::NONE ) 	  // returned
		return false;

	return true;
}

void DAirUnitCombat::KillPlane( AirUnit* au, bool only_aborted )
{
	for ( int i = 0; i < num_killed_planes_; ++i )	// if units is already in killed list
		if ( au == killed_plane_list_[i] )	// then don't add it again
			return;

	char s[100];
	au->getUnitString( s );

	char killmsg[200];
	if ( only_aborted )
		sprintf( killmsg, "Aborted plane was removed to air replacement pool:\n%s", s );
	else
		sprintf( killmsg, "The following plane was shot down:\n%s", s );
	wxMessageBox( wxS( killmsg ), Application::NAME, wxICON_INFORMATION );

	killed_plane_list_[num_killed_planes_++] = au;
}

void DAirUnitCombat::ApplyResult( AirUnit* au, int res )
{
	switch ( res )
	{
		case AirCombatResult::MISS:
			break;
		case AirCombatResult::ABORT:
			au->setMission( AirUnit::Mission::NONE );
			if ( au->getAborted( ) )
				KillPlane( au );  // aborted twice == killed
			else
			{
				au->setAborted( TRUE );
				if ( _rule_set.AbortedPlanesToPool == TRUE )
					KillPlane( au, true );
			}
			break;
		case AirCombatResult::RETURN:
			if ( au->getMission( ) == AirUnit::Mission::NONE )
			{
				// house rule: two return equals to abort:
#if 0
				 au->SetAborted( true );
				 if ( RuleSet.AbortedPlanesToPool )
					 KillPlane( au );
#endif
			}
			else
				au->setMission( AirUnit::Mission::NONE );
			break;
		case AirCombatResult::KILL:
			KillPlane( au );
			break;
	}
	// TODO: below causes bug: if one int attacks two escorts and there
	// are no bypassers then second esc is not allowed to fire (because
	// int is thought to be returning to home).
	if ( au->getMission( ) == AirUnit::Mission::INTERCEPT )
		au->setMission( AirUnit::Mission::NONE );
}

static void trunc_str( char* s ) // remove trailing spaces
{
	char* cp = s;
	while ( *s )
		if ( *s++ != ' ' )
			cp = s;

	*cp = 0;
}

// no_def_shoot!=0 -> def won't fire (=a bypassing interceptor)
int DAirUnitCombat::FightAirCombat( AirUnit* att, AirUnit* def, int no_def_shoot )
{
	int res1, diff1, mods1, diff2, mods2, n1, n2;
	int res2 = -1;
	int n3 = 0;
	int n4 = 0;
	int def_fired;
	char s1[100], s2[100];

	def_fired = def->hasFired( );

	att->getUnitString( s1 );
	def->getUnitString( s2 );
	GetDiffAndMods( att, def, &diff1, &mods1 );
	GetDiffAndMods( def, att, &diff2, &mods2 );
	sprintf( msg, "%s\nvs\n%s\n\nDiff %s%d, die modifier %s%d\n"
			"Let computer to do die rolling?", s1, s2, diff1 >= 0 ? "+" : "", diff1, mods1 >= 0 ? "+" : "", mods1 );
	if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO ) == wxYES )
	{
		n1 = dice( );
		n2 = dice( );
		res1 = AirCrt( att, def, n1, n2 );
		if ( ! no_def_shoot )
		{
			n3 = dice( );
			n4 = dice( );
			res2 = AirCrt( def, att, n3, n4 );
		}
	}
	else
	{
		att->getSymbolString( s1 );
		def->getSymbolString( s2 );
		trunc_str( s1 );
		trunc_str( s2 );
		DAirCombatDice dlg( this );
		sprintf( msg, "%s fires %s", s1, s2 );
		dlg.SetHelpText( msg );
		if ( dlg.ShowModal( ) != wxID_OK ) 
			return 0;
		dlg.CloseDialog( );  // oh woe
		n1 = dlg.GetDie1( );
		n2 = dlg.GetDie2( );
		res1 = AirCrt( att, def, n1, n2 );
		if ( ! no_def_shoot )
		{
			sprintf( msg, "%s fires %s", s2, s1 );
			dlg.SetHelpText( msg );
			if ( dlg.ShowModal( ) == FALSE )
				return 0;
			dlg.CloseDialog( );  // oh woe
			n3 = dlg.GetDie1( );
			n4 = dlg.GetDie2( );
			res2 = AirCrt( def, att, n3, n4 );
		}
	}
	att->setFired( TRUE );
	def->setFired( TRUE );
	att->getSymbolString( s1 );
	def->getSymbolString( s2 );
	trunc_str( s1 );
	trunc_str( s2 );
	if ( ! no_def_shoot && ! def_fired ) 	// normal way:
	{
		sprintf( msg, "%s  %s  %s\t(die roll %d+%d(%s%d)=%d, diff %s%d)\n"
				"%s  %s  %s\t(die roll %d+%d(%s%d)=%d, diff %s%d)", s1, AirCrt2Str( res1 ), s2, n1, n2,
				mods1 >= 0 ? "+" : "", mods1, n1 + n2 + mods1, diff1 >= 0 ? "+" : "", diff1, s2, AirCrt2Str( res2 ), s1,
				n3, n4, mods2 >= 0 ? "+" : "", mods2, n3 + n4 + mods2, diff2 >= 0 ? "+" : "", diff2 );
	}
	else
	{
		if ( no_def_shoot )
			sprintf( msg, "%s  %s  %s\t(die roll %d+%d(%s%d)=%d, diff %s%d)\n"
					"%s was trying to bypass and didn't fire", s1, AirCrt2Str( res1 ), s2, n1, n2,
					mods1 >= 0 ? "+" : "", mods1, n1 + n2 + mods1, diff1 >= 0 ? "+" : "", diff1, s2 );
		else
			// def_fired
			sprintf( msg, "%s  %s  %s\t(die roll %d+%d(%s%d)=%d, diff %s%d)\n"
					"%s has already fired and didn't fire at this combat", s1, AirCrt2Str( res1 ), s2, n1, n2,
					mods1 >= 0 ? "+" : "", mods1, n1 + n2 + mods1, diff1 >= 0 ? "+" : "", diff1, s2 );
	}
	wxMessageBox( wxS( msg ), Application::NAME, wxICON_INFORMATION );

	if ( ! no_def_shoot || res1 != AirCombatResult::MISS )  // missed bypasser: no ApplyResult
		ApplyResult( def, res1 );
	if ( !no_def_shoot ) 
		ApplyResult( att, res2 );

	return 0;
}

static int rawAirCrt( int diff, int die )
{
	const char air_crt[13][10] = // x = die, y = differential
	{
		// 1      2      3      4      5      6      7      8      9      10+
		{ AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	// -5
		{ AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	// -4
		{ AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	// -3
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	// -2
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	// -1
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	//  0
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	// +1
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS, AirCombatResult::MISS },	// +2
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS },	// +3
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS, AirCombatResult::MISS },	// +4
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::RETURN, AirCombatResult::MISS },	// +5
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::RETURN, AirCombatResult::MISS },	// +6
		{ AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::KILL, AirCombatResult::ABORT, AirCombatResult::ABORT, AirCombatResult::MISS }		// +7
	};

	if ( diff < -50 )
		return AirCombatResult::AUTOMISS;

	if ( diff < -5 )
		diff = -5;

	if ( diff > 7 )
		diff = 7;

	if ( die < 1 )
		die = 1;

	if ( die > 10 )
		die = 10;
#if 0
	sprintf( msg, "Crt: odds=%f:  row=%d, die=%d", r, row, die );
	wxMessageBox( msg, Application::NAME );
#endif
	return air_crt[diff + 5][die - 1];	// die=0..9
}

// air combat resolution table, return MISS, RETURN, ABORT or KILL
// note: die = _two_ die rolls
// from second front
int DAirUnitCombat::AirCrt( AirUnit* att, AirUnit* def, int noppa1, int noppa2 )
{
	int die, diff, mods;

	GetDiffAndMods( att, def, &diff, &mods );

	if ( noppa1 <= 0 )
	{
		noppa1 = dice( );
		noppa2 = dice( );
	}
	die = noppa1 + noppa2 + mods;
	return rawAirCrt( diff, die );
}

// air combat DRM
int DAirUnitCombat::GetAirMods( AirUnit* att, AirUnit* def )
{
	int mod = 0;

	int atk_type = att->getType( );
	int def_type = def->getType( );

	// air unit type HF is a special case, handle it first:
	if ( _rule_set.HFIsF == TRUE && def_type == AirUnitType::HF)
		def_type = AirUnitType::F;

	// modifier for type F versus non type F
	if ( atk_type == AirUnitType::F )
	{
		switch ( def_type )
		{
			case AirUnitType::HF: // see above HFIsF flag
			case AirUnitType::B:
			case AirUnitType::T:
			case AirUnitType::HT:
				--mod;
				break;
		}

		// the following four mutex special cases are from 1937-1941 (WoR)
		if ( att->getSubType( ) == AirArmedForce::Axis::AIR_IJN
			&& def->getSubType( ) == AirArmedForce::Allied::AIR_CHINA )
			mod -= 2;
#if 0
		// excluded due to suspicion that it is handled by the broad -1 for any non-Guards Soviet (see below)
		if ( att->getSubType( ) == AirArmedForce::Axis::AIR_IJN
			&& def->getSubType( ) == AirArmedForce::Allied::AIR_SOV )
			--mod;
#endif
		if ( att->getSubType( ) == AirArmedForce::Axis::AIR_IJA
			&& def->getSubType( ) == AirArmedForce::Allied::AIR_CHINA )
			--mod;

		if ( _year < 1941 )
		{
			if ( att->getSubType( ) == AirArmedForce::Allied::AIR_USAAF
				&& def->getSubType( ) == AirArmedForce::Axis::AIR_IJA )
				--mod;
		}

		if (	( _year == 1942 && _month < 6 )		// Jan-Jun 42
			 || ( _year == 1941 && _month == 11 ) )	// Dec 41
		{
			if ( att->getSubType( ) == AirArmedForce::Axis::AIR_IJN
				&& def->getSubType( ) == AirArmedForce::Allied::AIR_CHINA )
				mod -= 2;

			if ( att->getSubType( ) == AirArmedForce::Axis::AIR_IJA
				&& def->getSide( ) == SidePlayer::ALLIED )
				--mod;
		}

		// TODO:  vet the time frame of this type F mod against Soviet non-Guards air units
		if ( def->getSubType( ) == AirArmedForce::Allied::AIR_SOV )
			--mod;
	}

	// heavy fighters get -1 often too
	else if ( atk_type == AirUnitType::HF )
	{
		switch ( def_type )
		{
			case AirUnitType::B:
			case AirUnitType::HB: // HF get -1 against HB too
			case AirUnitType::T:
			case AirUnitType::HT:
				--mod;
				break;
		}
	}

	// air unit type HF is a special case, handle it first:
	if ( _rule_set.HFIsF == TRUE && atk_type == AirUnitType::HF)
		atk_type = AirUnitType::F;

	// adverse modifiers non type F versus type F
	if ( def_type == AirUnitType::F )
	{
		switch ( atk_type )
		{
			case AirUnitType::HF: // see above HFIsF flag
			case AirUnitType::B:
			case AirUnitType::D:
			case AirUnitType::T:
			case AirUnitType::HT:
				++mod;
				break;
		}

		// TODO:  vet the time frame of this type F adverse mod for Soviet non-Guards air units
		if ( att->getSubType( ) == AirArmedForce::Allied::AIR_SOV )
			++mod;
	}

	int atk_prefix = att->getPrefix( );
	int def_prefix = def->getPrefix( );

	// any jet/rocket type versus non-jet/rocket type
	if ( atk_prefix == AirUnitType::Prefix::J || atk_prefix == AirUnitType::Prefix::R )
	{
		if ( def_prefix != AirUnitType::Prefix::J && def_prefix != AirUnitType::Prefix::R )
			--mod;
	}

	// any non-jet/rocket type versus jet/rocket type
	if ( atk_prefix != AirUnitType::Prefix::J && atk_prefix != AirUnitType::Prefix::R )
	{
		if ( def_prefix == AirUnitType::Prefix::J || def_prefix == AirUnitType::Prefix::R )
			++mod;
	}

	int atk_code = att->getCode( );
	int def_code = def->getCode( );

	// code E vs non code E (Elite)
	if (	atk_code == AirUnit::Code::E
		 && def_code != AirUnit::Code::E )
		--mod;

	if (	def_code == AirUnit::Code::E
		 && atk_code != AirUnit::Code::E )
		++mod;

	return mod;
}

#endif
