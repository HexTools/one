#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;

#include <wx/wx.h>

#include "hextools.h"
using ht::wxS;
using ht::spc2line;
using ht::line2spc;
using ht::strLen;
using ht::drawStr;
using ht::isOkStr;

#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "rulesvariant.h"
#include "rules.h"
#include "counter.h"
#include "navalunitsubtype.h"
#include "navalunittype.h"
#include "navalunit.h"
#include "dnavalunitship.h"
#include "dnavalunit.h"

extern wxFont fnt;
extern wxFont fnt2;

extern int _scn_version;

extern Rules _rule_set;

extern NavalArmedForce _naval_armed_force[][NavalArmedForce::Allied::COUNT];

extern ShipDlgXfer _ship_dialog_data;
extern FleetDlgXfer _fleet_dialog_data;

extern int _current_player;

extern wxBitmap* hbmWarShip;
extern wxBitmap* hbmCargoShip;
extern wxBitmap* hbmCruiser;
extern wxBitmap* hbmDestroyer;
extern wxBitmap* hbmAuxShip;
extern wxBitmap* hbmLC;
extern wxBitmap* hbmSubmarine;
extern wxBitmap* hbmCarrier;

extern wxWindow* _parent_dialog_window;

extern const char* _ship_types[];			// SHIPTYPES
extern const char* _ship_type_string[];		// SHIPTYPES
extern const char* _fleet_type[];		// FLEETTYPES
extern const char* _fleet_type_name[];	// FLEETTYPES

NavalUnit::NavalUnit( int t /* = FLEET_NTP */)
{
	counter_type_ = Counter::Type::SHIP;
	next_ = nullptr;
	type_ = t;
#if 0
	if ( RuleSet.OldNavalSystem )
		type_ = SHIP_NT;
	else
		type_ = FLEET_NTP;
#endif
	subtype_ = 0;
	setSide( _current_player );
	strcpy( name_, "Titanic" );
	name_[NavalUnit::MAX_NAME_LEN - 1] = '\0';
	aa_ = pri_ = sec_ = ter_ = current_damage_ = 0;
	strcpy( protection_, "1" );
	speed_ = maximum_damage_ = 1;
	torpedo_ = reloads_ = naval_unit_flags_ = 0;
}

int NavalUnit::getMP( void )
{
	if ( _rule_set.OldNavalSystem == TRUE )
		return 0;
	else
		return ( pri_ > 100 ? pri_ - 256 : pri_ );
}

int NavalUnit::getMaxMP( void )
{
	return ( _rule_set.OldNavalSystem == TRUE ? 0 : sec_ );
}

void NavalUnit::setMP( unsigned int i )
{
	if ( type_ >= NavalUnitType::FIRST_FLEET )  // silently ignore errors (called by RestoreAllMPs())
		pri_ = static_cast<unsigned char>( i );
#if 0
	else
		wxMessageBox( "Old style ships do not have MPs\n" );
#endif
}

void NavalUnit::setMaxMP( unsigned int i )
{
	if ( type_ >= NavalUnitType::FIRST_FLEET )
		sec_ = static_cast<unsigned char>( i );
	else
		wxMessageBox( wxT("Old style ships do not have MPs\n") );
}

bool NavalUnit::edit( void )   // show edit dialog-box, return: CANCEL->FALSE, OK->TRUE
{
	if ( type_ < NavalUnitType::Ship::COUNT ) 	// old naval system
	{
		DNavalUnitShip dlg( _parent_dialog_window );

		strcpy( _ship_dialog_data.editName, name_ );

		char str[50];
		sprintf( str, "%d", pri_ );
		strcpy( _ship_dialog_data.editPri, str );
		sprintf( str, "%d", sec_ );
		strcpy( _ship_dialog_data.editSec, str );
		sprintf( str, "%d", ter_ );
		strcpy( _ship_dialog_data.editThi, str );
		sprintf( str, "%d", aa_ );
		strcpy( _ship_dialog_data.editAA, str );
		sprintf( str, "%d", current_damage_ );
		strcpy( _ship_dialog_data.editHits, str );
		sprintf( str, "%d", maximum_damage_ );
		strcpy( _ship_dialog_data.editMaxHits, str );

		dlg.SetClass( _ship_type_string[type_] );
		dlg.SetNation( getSubType( ) );

		strcpy( _ship_dialog_data.editProt, protection_ );
		sprintf( _ship_dialog_data.editSpeed, "%d", speed_ );
		sprintf( _ship_dialog_data.editTorp, "%d", torpedo_ );
		sprintf( _ship_dialog_data.editReloads, "%d", reloads_ );

		_ship_dialog_data.chkRadar = naval_unit_flags_ & NavalUnit::NFLAG_RADAR ? TRUE : FALSE;

		if ( dlg.ShowModal( ) != wxID_OK )
			return false;
		dlg.CloseDialog( );

		// get results
		setSubType( dlg.GetNation( ) );
		type_ = dlg.GetClass( );
		strncpy( name_, _ship_dialog_data.editName, NavalUnit::MAX_NAME_LEN - 1 );
		pri_ = (unsigned char)atoi( _ship_dialog_data.editPri );
		sec_ = (unsigned char)atoi( _ship_dialog_data.editSec );
		ter_ = (unsigned char)atoi( _ship_dialog_data.editThi );
		aa_ = (unsigned char)atoi( _ship_dialog_data.editAA );
		current_damage_ = (unsigned char)atoi( _ship_dialog_data.editHits );
		maximum_damage_ = (unsigned char)atoi( _ship_dialog_data.editMaxHits );
		strcpy( protection_, _ship_dialog_data.editProt );
		speed_ = (unsigned char)atoi( _ship_dialog_data.editSpeed );
		torpedo_ = (unsigned char)atoi( _ship_dialog_data.editTorp );
		reloads_ = (unsigned char)atoi( _ship_dialog_data.editReloads );
		naval_unit_flags_ = ( _ship_dialog_data.chkRadar ? NFLAG_RADAR : 0);
	}
	else // new naval system
	{
		DNavalUnit dlg( _parent_dialog_window );
		dlg.SetType( type_ - NavalUnitType::FIRST_FLEET);
		dlg.SetSubType( getSubType( ) );

		sprintf( _fleet_dialog_data.editStr, "%d", getMaxHits( ) );
		sprintf( _fleet_dialog_data.editAA, "%d", getAA( ) );
		sprintf( _fleet_dialog_data.editHits, "%d", getHits( ) );
		sprintf( _fleet_dialog_data.editCurMP, "%d", getMP( ) );
		sprintf( _fleet_dialog_data.editMP, "%d", getMaxMP( ) );
		strcpy( _fleet_dialog_data.editName, name_ );

		if ( dlg.ShowModal( ) != wxID_OK )
			return false;
		dlg.CloseDialog( );

		type_ = dlg.GetType( ) + NavalUnitType::FIRST_FLEET;
		setSubType( dlg.GetSubType( ) );
		maximum_damage_ = atoi( _fleet_dialog_data.editStr );
		setAA( atoi( _fleet_dialog_data.editAA ) );
		setHits( atoi( _fleet_dialog_data.editHits ) );
		setMP( atoi( _fleet_dialog_data.editCurMP ) );
		setMaxMP( atoi( _fleet_dialog_data.editMP ) );
		strncpy( name_, _fleet_dialog_data.editName, NavalUnit::MAX_NAME_LEN - 1 );
	}

	return true;
}

void NavalUnit::setAttackDir( int dir )
{
	int i;

	switch ( dir )
	{
		case Hex::WEST:
			i = 2;
			break;
		case Hex::EAST:
			i = 4;
			break;
		case Hex::NORTHWEST:
			i = 6;
			break;
		case Hex::NORTHEAST:
			i = 8;
			break;
		case Hex::SOUTHWEST:
			i = 10;
			break;
		case Hex::SOUTHEAST:
			i = 12;
			break;
		default:
			i = 0;
			break;
	}
	naval_unit_flags_ = (unsigned char) ( naval_unit_flags_ & 0xf1 ) | i;
}

int NavalUnit::getAttackDir( void )
{
	int dir;

	int i = naval_unit_flags_ & 0xe;
	switch ( i )
	{
		case 2:
			dir = Hex::WEST;
			break;
		case 4:
			dir = Hex::EAST;
			break;
		case 6:
			dir = Hex::NORTHWEST;
			break;
		case 8:
			dir = Hex::NORTHEAST;
			break;
		case 10:
			dir = Hex::SOUTHWEST;
			break;
		case 12:
			dir = Hex::SOUTHEAST;
			break;
		default:
			dir = 0;
			break;
	}

	return dir;
}

float NavalUnit::getRealAtt( int WXUNUSED(defender) )
{
	return 0.0F;
}

float NavalUnit::getRealDef( int WXUNUSED(attacker) )
{
	return 0.0F;
}

void NavalUnit::drawCounter( wxDC* dc, int x, int y, bool border /* = true */ )
{
	wxColor bgnd = _naval_armed_force[getSide( )][getSubType( )].crBg;
	wxColor fgnd = _naval_armed_force[getSide( )][getSubType( )].crFg;
	wxColor bgndtop = *wxWHITE;
	dc->SetFont( fnt );
	// draw counter
#if 0
	dc->SetPen( *wxBLACK_PEN );
	dc->SetBrush( wxBrush( bgnd, wxSOLID ) );
	dc->DrawRectangle( x, y, 28, 27 );
#endif
	if ( border )
	{
		wxBrush bgnd_brush( bgnd, wxSOLID );
		dc->SetPen( *wxBLACK_PEN );  // always black border on counter
		dc->SetBrush( bgnd_brush );
		dc->DrawRectangle( x, y, 28, 27 );
	}
	else
	{
		wxPen bgnd_pen( bgnd, 1, wxSOLID );
		// EJW (new): top 2/3 is white
		wxBrush bgnd_brushtop( bgndtop, wxSOLID );
		dc->SetPen( bgnd_pen );      // no border
		dc->SetBrush( bgnd_brushtop );
		dc->DrawRectangle( x + 1, y + 1, 26, 18 );
		// EJW: bottom 1/3 is background color
		wxBrush bgnd_brush( bgnd, wxSOLID );
		dc->SetPen( bgnd_pen );      // no border
		dc->SetBrush( bgnd_brush );
		dc->DrawRectangle( x + 1, y + 18, 26, 9 );
	}

	char str[10];

	// AA in similar way in both types
	// EJW: AA drawn first, same color as symbol
	dc->SetTextForeground( fgnd );
	sprintf( str, "%d", aa_ );
	drawStr( dc, x + 2, y + 2, str );
#if 0
	dc->SetTextForeground( *wxBLACK );
#endif
	// draw symbol
	// old style (separate capital ships)
	if ( type_ < NavalUnitType::FIRST_FLEET )
	{
		wxBitmap bmp( 20, 20 );

#if 0
		dc.SetBkMode( OPAQUE );
#endif
		wxMemoryDC mem2dc;
		mem2dc.SelectObject( bmp );  // mem2dc = counter's color (16 bit)
		mem2dc.SetBrush( wxBrush( fgnd, wxSOLID ) );
		mem2dc.SetPen( wxPen( fgnd, 1, wxSOLID ) );
		mem2dc.DrawRectangle( 0, 0, 20, 20 );
#if 0
		mem2dc.FillRect( 0, 0, 20, 20, TBrush( TColor( 0, 255, 0 ) ) );
#endif
		wxMemoryDC memdc;
		switch ( getShipType( ) )
		{
			case NavalUnitType::Ship::V:
			case NavalUnitType::Ship::VE:
			case NavalUnitType::Ship::VL:
			case NavalUnitType::Ship::VS:
				memdc.SelectObject( *hbmCarrier );
				break;
			case NavalUnitType::Ship::B:
			case NavalUnitType::Ship::BB:
			case NavalUnitType::Ship::BC:
			case NavalUnitType::Ship::BD:
			case NavalUnitType::Ship::BP:
				memdc.SelectObject( *hbmWarShip );
				break;
			case NavalUnitType::Ship::CA:
			case NavalUnitType::Ship::CC:
			case NavalUnitType::Ship::CD:
			case NavalUnitType::Ship::CL:
				memdc.SelectObject( *hbmCruiser );
				break;
			case NavalUnitType::Ship::DA:
			case NavalUnitType::Ship::DC:
			case NavalUnitType::Ship::DD:
			case NavalUnitType::Ship::DE:
			case NavalUnitType::Ship::DT:
				memdc.SelectObject( *hbmDestroyer );
				break;
			case NavalUnitType::Ship::SC:
			case NavalUnitType::Ship::SS:
			case NavalUnitType::Ship::SO:
				memdc.SelectObject( *hbmSubmarine );
				break;
			case NavalUnitType::Ship::LC:
				memdc.SelectObject( *hbmLC );
				break;
			case NavalUnitType::Ship::NT:
				memdc.SelectObject( *hbmCargoShip );
				break;
			default:
				memdc.SelectObject( *hbmAuxShip );
				break;
		}
		dc->Blit( x + 7, y + 6, 17, 10, &mem2dc, 0, 0, wxXOR );  // don't draw borders
		dc->Blit( x + 7, y + 6, 17, 10, &memdc, 0, 0, wxAND );
		dc->Blit( x + 7, y + 6, 17, 10, &mem2dc, 0, 0, wxXOR );

		dc->SetTextBackground( bgnd );
		dc->SetTextForeground( fgnd );
		dc->SetFont( fnt );

		// write str
		if ( getShipType( ) != NavalUnitType::Ship::NT && getShipType( ) != NavalUnitType::Ship::LC )
		{
			sprintf( str, "%d-%d-%d", pri_, sec_, ter_ );
			int szx = strLen( str );
			int x_pos = x + 14 - ( szx / 2 );
			drawStr( dc, x_pos, y + 17, str );
		}
	}
	// new style task forces
	else
	{
		// name
		dc->SetFont( fnt2 );

#if 0
		sprintf( str, "%s", FleetTypes[GetShipType( ) - NavalUnitType::FIRST_FLEET] );
#endif
		sprintf( str, "%s", name_ );
		wxString ws = wxS( str );

		int szx, szy;
		dc->GetTextExtent( ws, &szx, &szy );

		dc->SetTextForeground( fgnd );
		dc->SetTextBackground( bgnd );

		int x_pos = x + 14 - ( szx / 2 );  // width=28 -> 14 = halfway
		dc->DrawText( ws, x_pos, y + 9 );
		// EJW: switch to white for the type and strength on background color
		dc->SetTextForeground( *wxWHITE );
		// str
		if ( type_ == NavalUnitType::Fleet::TF )
			sprintf( str, "%d", getStrength( ) );
		else if ( type_ == NavalUnitType::Fleet::CG )
			sprintf( str, "(%d)", getStrength( ) );
		else if ( type_ == NavalUnitType::Fleet::SS )
			sprintf( str, "S%d", getStrength( ) );
		else if ( type_ == NavalUnitType::Fleet::RF )
			sprintf( str, "RF%d", getStrength( ) );
		else if ( type_ == NavalUnitType::Fleet::LC || type_ == NavalUnitType::Fleet::NTP )
			sprintf( str, "C%d", getStrength( ) );
		else // SOS type:
			sprintf( str, "%s%d", _fleet_type[type_ - NavalUnitType::FIRST_FLEET], getStrength( ) );

		szx = strLen( str );
		x_pos = x + 14 - ( szx / 2 );
		drawStr( dc, x_pos, y + 19, str );
	}
}

// short description
void NavalUnit::getSymbolString( char* str )
{
	// ShipTypes[..] for old style, FleetTypes[..] for new
	sprintf(	str,
				"%s %s          ",
				type_ < NavalUnitType::FIRST_FLEET ? _ship_types[type_] : _fleet_type[type_ - NavalUnitType::FIRST_FLEET],
				name_ );
}

// longer description
void NavalUnit::getUnitString( char* str, bool WXUNUSED(verbose) /* = false */ )
{
	// old style
	if ( type_ < NavalUnitType::FIRST_FLEET )
		sprintf(	str,
					"%s %s '%s' %d-%d-%d  AA=%d  %d of %d hits left     ",
					_naval_armed_force[getSide( )][getSubType( )].name_,
					_ship_type_string[type_],
					name_,
					pri_,
					sec_,
					ter_,
					aa_,
					current_damage_,
					maximum_damage_ );
	else
		sprintf(	str,
					"%s %s '%s'  strength=%d  AA=%d  %d hits     ",
					_naval_armed_force[getSide( )][getSubType( )].name_,
					_fleet_type_name[type_ - NavalUnitType::FIRST_FLEET],
					name_,
					maximum_damage_,
					aa_,
					current_damage_ );

}

// loading
std::istream &operator>>( std::istream &is, NavalUnit& navalunit )
{
	int flg;
	is >> flg;
	navalunit.flags_ = flg;

	int a;
	is >> a;
	if ( _scn_version < 5 )
		switch ( a )
		{
			case 0:
				navalunit.type_ = NavalUnitType::Ship::NT;
				break;
			case 1:
				navalunit.type_ = NavalUnitType::Ship::DD;
				break;
			case 2:
				navalunit.type_ = NavalUnitType::Ship::CL;
				break;
			case 3:
				navalunit.type_ = NavalUnitType::Ship::CA;
				break;
			case 4:
				navalunit.type_ = NavalUnitType::Ship::BB;
				break;
			case 6:
				navalunit.type_ = NavalUnitType::Fleet::NTP;
				break;
			case 7:
				navalunit.type_ = NavalUnitType::Fleet::LC;
				break;
			case 8:
				navalunit.type_ = NavalUnitType::Fleet::TF;
				break;
			case 9:
				navalunit.type_ = NavalUnitType::Fleet::CG;
				break;
		}
	else
		navalunit.type_ = a;

	int b;
	is >> b;
	navalunit.pri_ = b;

	int c;
	is >> c;
	navalunit.sec_ = c;

	int d;
	is >> d;
	navalunit.ter_ = d;

	char str[50];
	is >> str;
	strncpy( navalunit.name_, str, NavalUnit::MAX_NAME_LEN - 1 );
	line2spc( navalunit.name_ );

	int e;
	is >> e;
	navalunit.aa_ = e;

	int f;
	is >> f;
	navalunit.current_damage_ = f;

	int g;
	is >> g;
	navalunit.maximum_damage_ = g;

	int h;
	is >> h;
	navalunit.subtype_ = h;

	std::string path;
	is >> path;
	if ( path.compare( ht::Replay::NONE ) == 0 )
		navalunit.clearReplay( );
	else
		navalunit.setReplay( const_cast<char*>( path.c_str( ) ) );

	if ( _scn_version >= 6 )
	{
		char pr[10];
		is >> pr;
		strncpy( navalunit.protection_, pr, 4 );

		int i;
		is >> i;
		navalunit.speed_ = i;

		int j;
		is >> j;
		navalunit.torpedo_ = j;

		int k;
		is >> k;
		navalunit.reloads_ = k;

		int l;
		is >> l;
		navalunit.naval_unit_flags_ = l;
	}

	return is;
}

// saving
std::ostream &operator<<( std::ostream &os, NavalUnit& navalunit )
{
	os << (int)navalunit.flags_ << ' ';

	os << (int)navalunit.type_ << ' ';

	os << (int)navalunit.pri_ << ' ';

	os << (int)navalunit.sec_ << ' ';

	os << (int)navalunit.ter_ << ' ';

	spc2line( navalunit.name_ );
	if ( isOkStr( navalunit.name_ ) )
		os << navalunit.name_ << ' ';
	else
		os << "-" << ' ';
	line2spc( navalunit.name_ );

	os << (int)navalunit.aa_ << ' ';

	os << (int)navalunit.current_damage_ << ' ';

	os << (int)navalunit.maximum_damage_ << ' ';

	os << (int)navalunit.subtype_ << ' ';

	if ( navalunit.replay_string_ && navalunit.replay_string_[0] != ht::Replay::END )
		os << navalunit.replay_string_ << ' ';
	else
		os << ht::Replay::NONE << ' ';

	spc2line( navalunit.protection_ );
	if ( ! isOkStr( navalunit.protection_ ) )
		strcpy( navalunit.protection_, "0" );
	os << navalunit.protection_ << ' ';
	line2spc( navalunit.protection_ );

	os << (int)navalunit.speed_ << ' ';

	os << (int)navalunit.torpedo_ << ' ';

	os << (int)navalunit.reloads_ << ' ';

	os << (int)navalunit.naval_unit_flags_ << ' ';

	return os;
}

#endif
