#include <wx/wx.h>

#include "armoreffectsvalue.h"

void ArmorEffectsValue::set_by_char( char c )
{
	switch ( c )
	{
		case ArmorEffectsValue::NO:
			set_none( );		break;
		case ArmorEffectsValue::FU:
			set_full( );		break;
		case ArmorEffectsValue::HA:
			set_half( );		break;
		case ArmorEffectsValue::NE:
			set_neutral( );	break;
		case ArmorEffectsValue::TH:
			set_third( );		break;
		default: // any other
			set_none( );
			wxMessageBox( wxT( "Xv0B: bogus armor/antitank effect value setting " ) + wxT( c ) );
			break;
	}
}

char ArmorEffectsValue::char_value( )
{
	char c = 'x';
	switch ( value_ )
	{
		case ArmorEffectsValue::AECNONE:
			c = ArmorEffectsValue::NO;	break;
		case ArmorEffectsValue::FULL:
			c = ArmorEffectsValue::FU;	break;
		case ArmorEffectsValue::HALF:
			c = ArmorEffectsValue::HA;	break;
		case ArmorEffectsValue::NEUT:
			c = ArmorEffectsValue::NE;	break;
		case ArmorEffectsValue::THIRD:
			c = ArmorEffectsValue::TH;	break;
	}
	return c;
}

ArmorEffectsValue::ArmorEffectsValue( )
{
	value_ = ArmorEffectsValue::AECNONE;
}

ArmorEffectsValue::~ArmorEffectsValue( )
{
	// TODO Auto-generated destructor stub
}
