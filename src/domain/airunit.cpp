#if defined HEXTOOLSPLAY

#include <iostream>

#include <wx/wx.h>

#include "rules.h"
#include "sideplayer.h"
#include "counter.h"
#include "airunitsubtype.h"
#include "airunittype.h"
#include "airunit.h"
#include "dairunit.h"
#include "hextools.h"
using ht::wxS;
using ht::mission2str;
using ht::spc2line;
using ht::line2spc;
using ht::strLen;
using ht::drawStr;
using ht::isOkStr;

extern wxFont fnt;
extern wxFont fnt2;

extern Rules _rule_set;
#if 0
extern int _year;
#endif

extern AirArmedForce _air_armed_force[2][AirArmedForce::Allied::COUNT];
extern AddAirUnitXfer _add_airunit_dialog_data;

extern wxWindow* _parent_dialog_window;

static char* get_code_str( int code )
{
	static char bf[10];
	int i = 0;

	switch ( code )
	{
		case AirUnit::Code::L:
			bf[i++] = 'L';
			break;
		case AirUnit::Code::T:
			bf[i++] = 'T';
			break;
		case AirUnit::Code::C:
			bf[i++] = 'C';
			break;
		case AirUnit::Code::F:
			bf[i++] = 'F';
			break;
		case AirUnit::Code::S:
			bf[i++] = 'S';
			break;
		case AirUnit::Code::M:
			bf[i++] = 'M';
			break;
		case AirUnit::Code::V:
			bf[i++] = 'V';
			break;
		case AirUnit::Code::B:
			bf[i++] = 'B';
			break;
		case AirUnit::Code::I:
			bf[i++] = 'I';
			break;
		case AirUnit::Code::X:
			bf[i++] = 'X';
			break;
		case AirUnit::Code::Z:
			bf[i++] = 'Z';
			break;
		case AirUnit::Code::E:
			bf[i++] = 'E';
			break;
		case AirUnit::Code::H:
			bf[i++] = 'H';
			break;
	}
	bf[i] = '\0';
	return bf;
}

static const char* get_code_text( int code )
{
	switch ( code )
	{
		case AirUnit::Code::L:
			return "low altitude";
		case AirUnit::Code::T:
			return "antitank capable";
		case AirUnit::Code::C:
			return "carrier capable";
		case AirUnit::Code::F:
			return "floatplane";
		case AirUnit::Code::S:
			return "anti-shipping capable";
		case AirUnit::Code::M:
			return "anti-shipping missiles";
		case AirUnit::Code::V:
			return "anti-shipping torpedoes";
		case AirUnit::Code::B:
			return "high-precision bombing";
		case AirUnit::Code::I:
			return "dedicated interceptor";
		case AirUnit::Code::X:
			return "fragile air units";
		case AirUnit::Code::Z:
			return "V1 carrier";
		case AirUnit::Code::E:
			return "experten";
		case AirUnit::Code::H:
			return "high altitude";
	}
	return "";
}

static char* get_type_str( int type, int prefix )
{
	static char bf[10];
	int i = 0;

	switch ( prefix )
	{
		case AirUnitType::Prefix::N:
			bf[i++] = 'N';
			break;
		case AirUnitType::Prefix::J:
			bf[i++] = 'J';
			break;
		case AirUnitType::Prefix::R:
			bf[i++] = 'R';
			break;
	}
	switch ( type )
	{
		case AirUnitType::F:
			bf[i++] = 'F';
			break;
		case AirUnitType::HF:
			bf[i++] = 'H';
			bf[i++] = 'F';
			break;
		case AirUnitType::B:
			bf[i++] = 'B';
			break;
		case AirUnitType::A:
			bf[i++] = 'A';
			break;
		case AirUnitType::D:
			bf[i++] = 'D';
			break;
		case AirUnitType::HB:
			bf[i++] = 'H';
			bf[i++] = 'B';
			break;
		case AirUnitType::T:
			bf[i++] = 'T';
			break;
		case AirUnitType::HT:
			bf[i++] = 'H';
			bf[i++] = 'T';
			break;
		case AirUnitType::GT:
			bf[i++] = 'G';
			bf[i++] = 'T';
			break;
		case AirUnitType::GHT:
			bf[i++] = 'G';
			bf[i++] = 'H';
			bf[i++] = 'T';
			break;
		case AirUnitType::R:
			bf[i++] = 'R';
			break;
		case AirUnitType::Z:
			bf[i++] = 'Z';
			break;
		case AirUnitType::O:
			bf[i++] = 'O';
			break;
	}
	bf[i] = '\0';
	return bf;
}

AirUnit::AirUnit( ) : Counter( )
{
	counter_type_ = Counter::Type::PLANE;
	next_ = nullptr;
	air_ = bombing_ = data1_ = data2_ = flags_ = mission_ = 0;
	MPs_left_ = 0;
	range_ = 0xff;	// 255 is invalid
	strcpy( name_, "" );
}

void AirUnit::setName( const char* s )
{
	strncpy( name_, s, AirUnit::MAX_MODEL_LEN );
	if ( strlen( s ) >= AirUnit::MAX_MODEL_LEN )
		name_[AirUnit::MAX_MODEL_LEN - 1] = '\0';
	spc2line( name_ );
}

bool AirUnit::edit( )
{
	static int t, st, c;
	static DAirUnit* dlg;
	if ( ! dlg )
		dlg = new DAirUnit( _parent_dialog_window );

	// setup dialog box
	dlg->SetPlayer( isAxis( ) ? SidePlayer::AXIS : SidePlayer::ALLIED ); // dlg Player

	// initialize dlg based on unit ( if valid, i.e. range < 255 )
	char msg[64];
	if ( getMaxMP( ) < 255 )
	{
		strcpy( _add_airunit_dialog_data.Type, getName( ) );
		sprintf( msg, "%d", getAtt( ) );
		strcpy( _add_airunit_dialog_data.Attack, msg );
		sprintf( msg, "%d", getDef( ) );
		strcpy( _add_airunit_dialog_data.Defence, msg );
		sprintf( msg, "%d", getStrat( ) );
		strcpy( _add_airunit_dialog_data.Strat, msg );
		sprintf( msg, "%d", getTac( ) );
		strcpy( _add_airunit_dialog_data.Tac, msg );
		sprintf( msg, "%d", getMaxMP( ) );
		strcpy( _add_airunit_dialog_data.Range, msg );
		sprintf( msg, "%d", getMP( ) );
		strcpy( _add_airunit_dialog_data.editMPLeft, msg );
#if 0
		AddAirUnitData.AirType.Select( GetType( ) );
#endif
		t = getType( );
		st = getSubType( );
		c = getCode( );
		dlg->SetType( t ); // dlg type
		dlg->SetSubType( st ); // dlg nat
		dlg->SetCode( c ); // dlg code
		_add_airunit_dialog_data.Aborted = getAborted( );
		_add_airunit_dialog_data.chkInop = ( isActive( ) ? FALSE : TRUE );
		_add_airunit_dialog_data.Night = _add_airunit_dialog_data.Jet = _add_airunit_dialog_data.Rocket = 0;
		_add_airunit_dialog_data.chkC = _add_airunit_dialog_data.chkL = 0;
		if ( getPrefix( ) == AirUnitType::Prefix::N )
			_add_airunit_dialog_data.Night = TRUE;
		else if ( getPrefix( ) == AirUnitType::Prefix::J )
			_add_airunit_dialog_data.Jet = TRUE;
		else if ( getPrefix( ) == AirUnitType::Prefix::R )
			_add_airunit_dialog_data.Rocket = TRUE;
		if ( getFlag( AirUnit::CODE_C ) )
			_add_airunit_dialog_data.chkC = TRUE;
		if ( getFlag( AirUnit::CODE_L ) )
			_add_airunit_dialog_data.chkL = TRUE;
		if ( getFlag( AirUnit::CODE_F ) )
			_add_airunit_dialog_data.chkF = TRUE;
	}
	else
	{
		dlg->SetType( t );
		dlg->SetSubType( st );
		dlg->SetCode( c );
	}

	if ( dlg->ShowModal( ) != wxID_OK )
		return false;
	dlg->CloseDialog( );

	// get results
	setName( _add_airunit_dialog_data.Type );
	setAtt( atoi( _add_airunit_dialog_data.Attack ) );
	setDef( atoi( _add_airunit_dialog_data.Defence ) );
	setStrat( atoi( _add_airunit_dialog_data.Strat ) );
	setTac( atoi( _add_airunit_dialog_data.Tac ) );
	setMaxMP( atoi( _add_airunit_dialog_data.Range ) );
	setMP( atoi( _add_airunit_dialog_data.editMPLeft ) );
	setAborted( _add_airunit_dialog_data.Aborted );
	setActive( _add_airunit_dialog_data.chkInop ? FALSE : TRUE );
	setType( t = dlg->GetType( ) );
	setSubType( st = dlg->GetSubType( ) );
	setCode( c = dlg->GetCode( ) );

	if ( _add_airunit_dialog_data.Night )
		setPrefix( AirUnitType::Prefix::N );
	else if ( _add_airunit_dialog_data.Jet )
		setPrefix( AirUnitType::Prefix::J );
	else if ( _add_airunit_dialog_data.Rocket )
		setPrefix( AirUnitType::Prefix::R );
	else
		setPrefix( AirUnitType::Prefix::NONE );

	if ( _add_airunit_dialog_data.chkL )
		setFlag( AirUnit::CODE_L );
	else
		clearFlag( AirUnit::CODE_L );

	if ( _add_airunit_dialog_data.chkC )
		setFlag( AirUnit::CODE_C );
	else
		clearFlag( AirUnit::CODE_C );

	if ( _add_airunit_dialog_data.chkF )
		setFlag( AirUnit::CODE_F );
	else
		clearFlag( AirUnit::CODE_F );

	return true;
}

// build string like "7F5 Me109E 1" and "4B5 Ju88A 3-5"
void AirUnit::getSymbolString( char* str )
{
	strcpy( str, "" );
	strcat( str, getName( ) );

	char tmp[10];
	sprintf( tmp, " %d", getAtt( ) );
	strcat( str, tmp );
	if ( getPrefix( ) == AirUnitType::Prefix::N )
		strcat( str, "N" );
	else if ( getPrefix( ) == AirUnitType::Prefix::J )
		strcat( str, "J" );
	else if ( getPrefix( ) == AirUnitType::Prefix::R )
		strcat( str, "R" );

	switch ( getType( ) )
	{
		case AirUnitType::F:
			strcat( str, "F" );
			break;
		case AirUnitType::HF:
			strcat( str, "HF" );
			break;
		case AirUnitType::B:
			strcat( str, "B" );
			break;
		case AirUnitType::A:
			strcat( str, "A" );
			break;
		case AirUnitType::D:
			strcat( str, "D" );
			break;
		case AirUnitType::HB:
			strcat( str, "HB" );
			break;
		case AirUnitType::T:
			strcat( str, "T" );
			break;
		case AirUnitType::HT:
			strcat( str, "HT" );
			break;
		case AirUnitType::GT:
			strcat( str, "GT" );
			break;
		case AirUnitType::GHT:
			strcat( str, "GHT" );
			break;
		case AirUnitType::R:
			strcat( str, "R" );
			break;
		case AirUnitType::Z:
			strcat( str, "Z" );
			break;
		case AirUnitType::O:
			strcat( str, "O" );
			break;
		default:
			strcat( str, "?" );
			break;
	}
	sprintf( tmp, "%d", getDef( ) );
	strcat( str, tmp );
	strcat( str, " " );

	sprintf( tmp, "%d", getTac( ) );
	strcat( str, tmp );
	if ( getStrat( ) > 0 )
	{
		strcat( str, "-" );
		sprintf( tmp, "%d ", getStrat( ) );
		strcat( str, tmp );
	}
	strcat( str, " " );
	// code
	if ( getCode( ) != AirUnit::Code::C && getFlag( AirUnit::CODE_C ) )
		strcat( str, "C" );
	if ( getCode( ) != AirUnit::Code::L && getFlag( AirUnit::CODE_L ) )
		strcat( str, "L" );
	sprintf( tmp, "%s", get_code_str( getCode( ) ) );
	strcat( str, tmp );

	for ( int i = 0; i < 10; ++i )  // make sure string is at least 14 chars
		strcat( str, " " );
}

// a longer string
void AirUnit::getUnitString( char* str, bool WXUNUSED(verbose) )
{
	int side = ( isAxis( ) ? 0 : 1 );   // 0=AXIS, 1=ALLIED
	strcpy( str, "" );

	strcat( str, _air_armed_force[side][getSubType( )].name_ );
	strcat( str, " " );

	char bfr[255];
	getSymbolString( bfr );
	strcat( str, bfr );
	strcat( str, get_code_text( getCode( ) ) );
	strcat( str, " " );

	if ( getAborted( ) )
		strcat( str, "(aborted) " );
	if ( getInAir( ) )
		strcat( str, "flying " );
	if ( getNight( ) )
		strcat( str, "night " );
	if ( getInAir( ) && getMission( ) == AirUnit::Mission::NONE )
		strcat( str, "back to home" );
	else
		strcat( str, mission2str( getMission( ) ) );
	if ( getExtRange( ) )
		strcat( str, " at extended range" );
	if ( getNavalPatrol( ) )
		strcat( str, " (assigned to naval patrol)" );
}

void AirUnit::drawCounter( wxDC* hdc, int x, int y, bool WXUNUSED(border) )
{
	wxColor bgnd = _air_armed_force[getSide( )][getSubType( )].crBg;
	wxColor fgnd = _air_armed_force[getSide( )][getSubType( )].crFg;
	wxColor fgnd2 = _air_armed_force[getSide( )][getSubType( )].crFg2;

	hdc->SetTextForeground( fgnd );
	hdc->SetTextBackground( bgnd );
	hdc->SetFont( fnt2 );
#if 0
	hdc->SetPen( *wxBLACK_PEN );  // always black border on counter
	hdc->SetBrush( wxBrush( bgnd, wxSOLID ) );
	hdc->DrawRectangle( x, y, 28, 27 );
#endif
	wxBrush bgnd_brush( bgnd, wxSOLID );

	--x;  // no border

#if 0
	if ( border )
	{
		hdc->SetPen( *wxBLACK_PEN );  // always black border on counter
		hdc->SetBrush( bgnd_brush );
		hdc->DrawRectangle( x, y, 28, 27 );
	}
	else
	{
#endif
	wxPen bgnd_pen( bgnd, 1, wxSOLID );
	hdc->SetPen( bgnd_pen );      // no border
	hdc->SetBrush( bgnd_brush );
	hdc->DrawRectangle( x + 1, y + 1, 26, 25 );
#if 0
	}
#endif

	char str[20];
	char tmp[10];
	int szx, szy;
	int x_pos;

	// operative
	if ( isActive( ) )
	{
		// type ("NB")
		sprintf( str, "%s", get_type_str( getType( ), getPrefix( ) ) );
		szx = strLen( str );
		if ( getAtt( ) < 10 )
			x_pos = x + 14 - ( szx / 2 );  // width=28 -> 14 = halfway
		else
			x_pos = x + 15 - ( szx / 2 );  // width=28 -> 14 = halfway
		drawStr( hdc, x_pos, y + 2, str );
		// att str:
		sprintf( str, "%d", getAtt( ) );
		drawStr( hdc, x + 2, y + 2, str );
		// def str:
		sprintf( str, "%d", getDef( ) );
		if ( getDef( ) < 10 )
			drawStr( hdc, x + 22, y + 2, str );
		else
			drawStr( hdc, x + 19, y + 2, str );

		// name ("Ju88A")
		sprintf( str, "%s", getName( ) );
		line2spc( str );
		hdc->GetTextExtent( wxS( str ), &szx, &szy );
		x_pos = x + 14 - ( szx / 2 );  // width=28 -> 14 = halfway

#if 0
		dc.DrawText(x_pos,y+8, str);
		TRect text_rect;
		text_rect.top = y+8;
		text_rect.bottom = y+18;
		text_rect.left = x;
		text_rect.right = x+28;
		dc.ExtTextOut(x_pos, y+8, ETO_CLIPPED, &text_rect, str, strlen(str));
#endif
		// TODO: clipping (as above)!

		hdc->SetTextBackground( bgnd );
		hdc->SetTextForeground( fgnd );
// text is written to slightly different places in gtk&msw
#ifdef __WXGTK__
		hdc->DrawText( wxS( str ), x_pos, y + 9 );
#else
		hdc->DrawText( wxS( str ), x_pos, y + 8 );
#endif
		// bottom line ("2-1 C 12")
		// bombing ("3-5")
		strcpy( str, "" );
		// only tactical str ("x")
		if ( getStrat( ) == 0 )
		{
			if ( 1 /* GetTac() != 0 */ )
			{  // in newer games '0' is printed too
				sprintf( str, "%d", getTac( ) );
				if ( getCode( ) )	// if code letter, move str 1 pixel left to make space
					drawStr( hdc, x + 2, y + 18, str );
				else
					drawStr( hdc, x + 3, y + 18, str );
			}
		}
		else  // both tactical and str ("x-y")
		{
			sprintf( str, "%d-", getTac( ) );
			if ( getCode( ) )	// if code letter, move str 1 pixel left to make space
				drawStr( hdc, x + 1, y + 18, str );
			else
				drawStr( hdc, x + 2, y + 18, str );
			szx = strLen( str );
			sprintf( str, "%d", getStrat( ) );
			if ( getCode( ) )	// if code letter, move str 1 pixel left to make space
				drawStr( hdc, x + szx - 1, y + 18, str );
			else
				drawStr( hdc, x + szx, y + 18, str );
		}
		// code
		strcpy( str, "" );
		if ( getCode( ) != AirUnit::Code::C && getFlag( AirUnit::CODE_C ) )
			strcat( str, "C" );
		if ( getCode( ) != AirUnit::Code::L && getFlag( AirUnit::CODE_L ) )
			strcat( str, "L" );
		if ( getCode( ) != AirUnit::Code::F && getFlag( AirUnit::CODE_F ) )
			strcat( str, "F" );
		sprintf( tmp, "%s", get_code_str( getCode( ) ) );
		strcat( str, tmp );
		szx = strLen( str );
		x_pos = x + 14 - ( szx / 2 );  // width=28 -> 14 = halfway
#if 0
		hdc->DrawText(str, x_pos,y+15);
#endif
		drawStr( hdc, x_pos, y + 18, str );

		// range
		sprintf( str, "%d", getMaxMP( ) );
		szx = strLen( str );
		if ( getCode( ) )	// if code letter, move str 1 pixel right to make space
			x_pos = x + 28 - ( szx );
		else
			x_pos = x + 28 - ( szx + 1 );
		drawStr( hdc, x_pos, y + 18, str );
		// if patrol attacked show it:
		if ( _rule_set.OnDemandAirMissions == FALSE && getDidPA( ) )
		{
			hdc->SetTextForeground( wxColour( 0, 0, 0 ) );
			hdc->SetTextBackground( *wxWHITE );
			hdc->SetPen( *wxBLACK_PEN );  	// border
			hdc->SetBrush( *wxWHITE_BRUSH );	// white marker
			hdc->DrawRectangle( x, y, 14, 10 );
			drawStr( hdc, x + 1, y + 1, const_cast<char*>( "PA" ) );
		}
	}
	else  // inoperative
	{
		// name ("Ju88A")
		hdc->SetFont( fnt2 );
		sprintf( str, "%s", getName( ) );
		hdc->GetTextExtent( wxS( str ), &szx, &szy );
		x_pos = x + 14 - ( szx / 2 );  // width=28 -> 14 = halfway
		hdc->DrawText( wxS( str ), x_pos, y + 6 );
		hdc->SetFont( fnt );
		hdc->DrawText( wxS( "Inop." ), x + 5, y + 14 );
	}
	// draw crosses if aborted
	if ( getFlag( AirUnit::ABORTED ) )
	{
		hdc->SetPen( wxPen( fgnd, 2, wxSOLID ) );
		hdc->DrawLine( x, y, x + 27, y + 27 );
		hdc->DrawLine( x, y + 27, x + 27, y );
	}
	hdc->SetTextForeground( *wxBLACK );
}

// loading
std::istream& operator>>( std::istream& is, AirUnit& unit )
{
	char id[32];
	is >> id;
	line2spc( id );
	strncpy( unit.name_, id, 10 );

	int x;
	is >> x;
	if ( x )
		unit.setAxis( );
	else
		unit.setAllied( );

	int t;
	is >> t;
	unit.setSubType( t );

	int a;
	is >> a;
	unit.air_ = a;

	int b;
	is >> b;
	unit.bombing_ = b;

	int c;
	is >> c;
	unit.range_ = c;

	int mp;
	is >> mp;
	unit.MPs_left_ = mp;

	int d;
	is >> d;
	unit.data1_ = d;

	int e;
	is >> e;
	unit.data2_ = e;

	int f;
	is >> f;
	unit.flags_ = f;

	int hx;
	is >> hx;
	unit.homex_ = hx;

	int hy;
	is >> hy;
	unit.homey_ = hy;

	int mis;
	is >> mis;
	unit.mission_ = mis;

	std::string path;
	is >> path;
	if ( path.compare( ht::Replay::NONE ) == 0 )
		unit.clearReplay( );
	else
		unit.setReplay( const_cast<char*>( path.c_str( ) ) );

	return is;
}

// saving
std::ostream& operator<<( std::ostream& os, AirUnit& unit )
{
	if ( isOkStr( unit.name_ ) )
	{
		spc2line( unit.name_ );
		os << unit.name_ << ' ';
	}
	else
		os << "???" << ' ';

	os << (int)unit.isAxis( ) << ' ';
	os << (int)unit.getSubType( ) << ' ';
	os << (int)unit.air_ << ' ';
	os << (int)unit.bombing_ << ' ';
	os << (int)unit.range_ << ' ';
	os << (int)unit.MPs_left_ << ' ';
	os << (int)unit.data1_ << ' ';
	os << (int)unit.data2_ << ' ';
	os << (int)unit.flags_ << ' ';
	os << (int)unit.homex_ << ' ';
	os << (int)unit.homey_ << ' ';
	os << (int)unit.mission_ << ' ';
	if ( unit.replay_string_ && unit.replay_string_[0] != ht::Replay::END )
		os << unit.replay_string_ << ' ';
	else
		os << ht::Replay::NONE << ' ';

	return os;
}

#if 0
int AirUnit::experienceRating( void )
{
	if ( getSide( ) == AXIS )
	{
		switch ( getSubType( ) )
		{
			case AIR_LW:
				switch ( _year )
				{
					case 39:
						return FULL_EXP;
						break;
					case 40:
						return FULL_EXP;
						break;
					case 41:
						return FULL_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_ITA:
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return FULL_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return NO_EXP;
						break;
				}
				break;
			case AIR_RUM:
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return NO_EXP;
						break;
				}
				break;
			case AIR_HUN:
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return ONE_SEVENTH_EXP;
						break;
					case 44:
						return NO_EXP;
						break;
					case 45:
						return NO_EXP;
						break;
				}
				break;
			case AIR_FIN:
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return FULL_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;
			case AIR_AXTRK:  // turkish
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return NO_EXP;
						break;
					case 43:
						return NO_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;
			case AIR_AXBUL:  // bulgarian
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return ONE_SEVENTH_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;
			case AIR_AXSPA:  // spanish
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_AXPOR:  // portugese
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return ONE_SEVENTH_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_AXEGY:  // egypt
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return NO_EXP;
						break;
					case 43:
						return NO_EXP;
						break;
					case 44:
						return NO_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;

#define AIR_AXSOV    14 // soviet in axis side
#define AIR_AXGUARD  15 // soviet guards in axis side

			case AIR_AXSWE:  // swedish
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_AXEST:  // estonia
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return NO_EXP;
						break;
					case 43:
						return NO_EXP;
						break;
					case 44:
						return NO_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;
			case AIR_AXVICH:  // vichy france
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_AXSWI:  // swiss
				switch ( _year )
				{
					case 39:
						return ONE_SEVENTH_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_AXDAN:  // danish
				switch ( _year )
				{
					case 39:
						return ONE_SEVENTH_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;

#define AIR_AXETIO   23 // ethiopia
#define AIR_AXSAUD   24 // saudi

			case AIR_AXNORW:  // norwegian
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;

#define AIR_WW1_GER  26
#define AIR_WW1_AH   27 // aust/hung

			case AIR_SLO:  // slovaks
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return ONE_SEVENTH_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return NO_EXP;
						break;
				}
				break;

#define AIR_IJA      29 // ww2pac: Jp army
#define AIR_IJN      30 // ww2pac: Jp navy
#define AIR_AXWW1DUT 31
#define AIR_WW1AXITA 32
#define AIR_WW1GERNAV   33
#define AIR_WW1AXITANAV 34

		}
		return NO_EXP;
	}
	else
	{
		switch ( getSubType( ) )
		{
			case AIR_SOV:
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_GUARDS:
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_USAAF:
			case AIR_USNAVY:
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return ONE_SEVENTH_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_CANADA:  // royal canadian air force
			case AIR_RAF:
			case AIR_NZ:  // royal new zealander air force
			case AIR_FAA:  // british fleet air arm
			case AIR_AUS:  // royal australian air force
			case AIR_SAFR:  // south afrocan (SAAF)
			case AIR_FREEFR:  // free french
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return FULL_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_FRENCH:
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_GREEK:
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_POL:   // Polish
				switch ( _year )
				{
					case 39:
						return ONE_SEVENTH_EXP;
						break;
					case 40:
						return ONE_SEVENTH_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_BELG:
				switch ( _year )
				{
					case 39:
						return ONE_SEVENTH_EXP;
						break;
					case 40:
						return ONE_SEVENTH_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_DUTCH:
				switch ( _year )
				{
					case 39:
						return ONE_SEVENTH_EXP;
						break;
					case 40:
						return ONE_SEVENTH_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;

			case AIR_CZ:  // Czechoslovakian

			case AIR_NORW:
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_ALTRK:  // Turkish
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return NO_EXP;
						break;
					case 43:
						return NO_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;

			case AIR_PPL: // FWtBT: People's Army
			case AIR_YUGO: // Yugoslavian

			case AIR_ALSPA:  // Spanish
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;
			case AIR_ALPOR:  // Portuguese
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return ONE_SEVENTH_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return ONE_SEVENTH_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;

			case AIR_ALIRAN: // Iranian
			case AIR_ALIRAQ: // Iraqi

			case AIR_ALEGY:  // Egyptian
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return NO_EXP;
						break;
					case 43:
						return NO_EXP;
						break;
					case 44:
						return NO_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;
			case AIR_ALITA:  // Italian
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return FULL_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return NO_EXP;
						break;
				}
				break;
			case AIR_ALFIN:  // Finnish
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return FULL_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;
			case AIR_ALSWE:  // Swedish
				switch ( _year )
				{
					case 39:
						return HALF_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return FULL_EXP;
						break;
					case 43:
						return FULL_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_ALEST:  // Estonian
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return NO_EXP;
						break;
					case 43:
						return NO_EXP;
						break;
					case 44:
						return NO_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;

			case AIR_ALLITH: // Lithuanian

			case AIR_ALBUL:  // Bulgaria
				switch ( _year )
				{
					case 39:
						return NO_EXP;
						break;
					case 40:
						return NO_EXP;
						break;
					case 41:
						return NO_EXP;
						break;
					case 42:
						return ONE_SEVENTH_EXP;
						break;
					case 43:
						return ONE_SEVENTH_EXP;
						break;
					case 44:
						return ONE_SEVENTH_EXP;
						break;
					case 45:
						return ONE_SEVENTH_EXP;
						break;
				}
				break;
			case AIR_ALSWI:  // Swiss
				switch ( _year )
				{
					case 39:
						return ONE_SEVENTH_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return FULL_EXP;
						break;
					case 45:
						return FULL_EXP;
						break;
				}
				break;
			case AIR_ALDAN:  // Danish
				switch ( _year )
				{
					case 39:
						return ONE_SEVENTH_EXP;
						break;
					case 40:
						return HALF_EXP;
						break;
					case 41:
						return HALF_EXP;
						break;
					case 42:
						return HALF_EXP;
						break;
					case 43:
						return HALF_EXP;
						break;
					case 44:
						return HALF_EXP;
						break;
					case 45:
						return HALF_EXP;
						break;
				}
				break;

			case AIR_ALLATV: // Latvian
			case AIR_ALETIO: // Ethiopian
			case AIR_ALSAUD: // Saudi Arabian
			case AIR_RFC: // Royal Flying Corps
			case AIR_CHINA:
			case AIR_USAIR:
			case AIR_WW1DUT:
			case AIR_WW1ITA:
			case AIR_WW1BEL:
			case AIR_WW1RUS:
			case AIR_WW1BRITNAV:
			case AIR_WW1FRNAV:
			case AIR_WW1ITANAV:
			case AIR_WW1USNAV:

		}
		return NO_EXP;
	}
}
#endif

#endif
