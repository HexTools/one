#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;
using std::endl;
#include <vector>

#include <wx/wx.h>

#include "hextools.h"
using ht::calculateOptimalAEC;
using ht::wxS;
using ht::limit;

#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "sideplayer.h"
#include "rulesvariant.h"
#include "rules.h"
#include "counter.h"
#include "hexcontents.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "armorantitank.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "airunit.h"
#include "navalunit.h"
#include "facility.h"
#include "hitmarker.h"
#include "hexnote.h"
#include "statusmarker.h"
#include "resourcemarker.h"

extern Rules _rule_set;

#if 0
extern UnitType UnitTypes[UNITTYPECOUNT];   // unit types
extern SubType SubTypes[2][SUBTYPECOUNT];     // unit sub types, per side

extern int ShowAxisAtt;  // does the map show att or def str
extern int ShowAlliedAtt;

extern GroundUnit* SelectedUnits;    // defined in hexcontentspane.cpp
extern wxPoint ptCurrentHex;       // defined in mappane.cpp
#endif

extern int _scn_version;  // file version number (mappane.cpp)

#define AEC_AECA 1
#define AEC_AECD 2
#define AEC_ATEC 3

// situation flags
#if 0
#define SF_TEMPORARY_AF		0x00 // there is a temporary airfield here
#define SF_PERMANENT_AF		0x01 // there is a permanent airfield here
#endif

HexContents::HexContents( )
{
	unit_list_ = nullptr;
	defensive_strength_ = 0;
	AEC_ = 0;
	DAS_ = 0;
	rail_break_ = 0;
	data1_ = 0;
	data2_ = 0;
	data3_ = 0;
	data4_ = 0;
	data5_ = 0;
	data6_ = 0;
	data7_ = 0;
	data8_ = 0;
	data9_ = 0;
	temp_data_ = 0;
	subtype_ = 0;
	units_in_hex_ = false;

	setGauge( SidePlayer::AXIS );					// initialize data2 for standard gauge rail
	setOwner( SidePlayer::ALLIED );					// initialize data1 for Allied side ownership
}

HexContents::~HexContents( )
{
}

void HexContents::clear( )
{
	Counter* nc;
	Counter* c = unit_list_;
	while ( c )
	{
		nc = c->getNext( );
		delete c;
		c = nc;
	}
	unit_list_ = nullptr;
	defensive_strength_ = 0;
	AEC_ = 0;
	DAS_ = 0;
	rail_break_ = 0;
	data1_ = 0;
	data2_ = 0;
	data3_ = 0;
	data4_ = 0;
	data5_ = 0;
	data6_ = 0;
	data7_ = 0;
	data8_ = 0;
	data9_ = 0;
	temp_data_ = 0;
	subtype_ = 0;
	units_in_hex_ = false;

	setGauge( SidePlayer::AXIS );
	setOwner( SidePlayer::ALLIED );
}

// return side (AXIS/ALLIED) based on counters in hex.
// return -1 if no valid counters (=combat units)
int HexContents::getCounterSide( )
{
	Counter* cntr = unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT )
			return cntr->getSide( );
		cntr = cntr->getNext( );
	}
	return -1;
}

#if 0
// is hex occupied by opponent side's partisans?
int HexContents::isOpponentPartisans( int owner )
{
	Counter* cntr = UnitPtr;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == CNTR_UNIT )
		{
			if ( cntr->getSide( ) == owner )
				return FALSE;
			if ( ( (GroundUnit*)cntr )->getType( ) == PARTISAN )
				return TRUE;
		}
		cntr = cntr->getNext( );
	}
	return FALSE;
}
#endif

// data1:n 2 lowest bits contain the country information
void HexContents::setOwner( int i, bool obstruct_if_changed )
{
	if ( i < 0 || i > 3 )
	{
		wxMessageBox( wxT("Illegal owner in HexContents::SetOwner()"), wxT("BUG"), wxOK );
		return;
	}

	int owner_changed = ( i != getOwner( ) );

	// is it ok to change owner (no units that would occupy enemy hex...)?
#if 0
	 while (cntr) {
	 if (cntr->Type() != CNTR_MISC &&
	 cntr->Type() != CNTR_HIT &&
	 cntr->Type() != CNTR_NOTE &&
	 cntr->Type() != CNTR_PTS &&
	 (cntr->Type() != CNTR_PLANE || cntr->GetInAir() == FALSE) &&
	 cntr->GetSide() != i)
	 {
	 //wxMessageBox(0, "Can't take ownership of enemy-occupied hex!", Application::NAME, wxOK);
	 if (cntr->Type() == CNTR_UNIT) {
	 return;
	 }
	 cntr = cntr->GetNext();
	 }
#endif

	data1_ &= 0xfc;   // 1111 1100
	data1_ |= (unsigned char)i;

	// make all notes and status markers owned by new owner
	Counter* cntr = unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::MISC || cntr->getCounterType( ) == Counter::Type::HIT
				|| cntr->getCounterType( ) == Counter::Type::NOTE || cntr->getCounterType( ) == Counter::Type::PTS
				|| cntr->getCounterType( ) == Counter::Type::RP )
			cntr->setSide( i );

		cntr = cntr->getNext( );
	}

	// ww1: captured rail hexes are obstructed
	if ( _rule_set.RailsObstructed == TRUE && owner_changed && obstruct_if_changed )
		setRailObstructed( true );

	// TODO: caller of this function needs to call updateHitMarkers on the location
	//UpdateCache(SelectedUnits);
}

// hits are NOT calculated
int HexContents::getAFCapacity( bool bad_weather /* = false */ )
{
	int c = 0;
	Counter* cntr = unit_list_;
	while ( cntr )
	{
		if ( cntr->getCounterType( ) == Counter::Type::MISC )
		{
			if ( ( (Facility*)cntr )->getMiscType( ) == Facility::PERMAF3 )
				c += 3;
			else if ( ( (Facility*)cntr )->getMiscType( ) == Facility::PERMAF6 )
				c += 6;
			else if ( ( (Facility*)cntr )->getMiscType( ) == Facility::PERMAF9 )
				c += 9;
			else if ( ( (Facility*)cntr )->getMiscType( ) == Facility::PERMAF12 )
				c += 12;
		}
		if ( cntr->getCounterType( ) == Counter::Type::MISC && ( (Facility*)cntr )->getMiscType( ) == Facility::TEMPAF )
			c += ( bad_weather ? _rule_set.TmpAFBadWeatherCap : _rule_set.TmpAFCap );

		cntr = cntr->getNext( );
	}
	return c;
}

void HexContents::setGauge( int i )
{
	switch ( i )
	{
		case SidePlayer::AXIS:
			data2_ |= 0x40;
			break;
		case SidePlayer::ALLIED:
			data2_ &= 0xbf;
			break;
		default:
			wxMessageBox( wxT("HexContents::SetGauge"), wxT("illegal value"), wxOK | wxICON_HAND );
			break;
	}
}

// data7 bits 4..6 = bridge break
void HexContents::setBridgeBreak( int dir, bool val )
{
	switch ( dir )
	{
		case Hex::SOUTHEAST:
			if ( val )
				data6_ |= 0x10;
			else
				data6_ &= ~0x10;
			break;
		case Hex::SOUTHWEST:
			if ( val )
				data6_ |= 0x20;
			else
				data6_ &= ~0x20;
			break;
		case Hex::WEST:
			if ( val )
				data6_ |= 0x40;
			else
				data6_ &= ~0x40;
			break;
		default:
			wxMessageBox( wxT("You have found a bug:\n"
					"Illegal bridge direction in HexContents::SetBridgeBreak()"), wxT("HexTools internal error"), wxOK );
			break;
	}
}

bool HexContents::getBridgeBreak( int dir )
{
	switch ( dir )
	{
		case Hex::SOUTHEAST:
			return ( data6_ & 0x10 );
		case Hex::SOUTHWEST:
			return ( data6_ & 0x20 );
		case Hex::WEST:
			return ( data6_ & 0x40 );
		default:
			wxMessageBox( wxT("You have found a bug:\n"
					"Illegal bridge direction in HexContents::GetBridgeBreak()"), wxT("HexTools internal error"), wxOK );
			break;
	}
	return false;
}

GroundUnit* HexContents::getFirstLandUnit( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT )
			return (GroundUnit*)c;
		c = c->getNext( );
	}
	return 0;
}

AirUnit* HexContents::getFirstAirUnit( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::PLANE )
			return (AirUnit*)c;
		c = c->getNext( );
	}
	return 0;
}

NavalUnit* HexContents::getFirstShip( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::SHIP )
			return (NavalUnit*)c;
		c = c->getNext( );
	}
	return 0;
}

bool HexContents::isMisc( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC )
			return true;
		c = c->getNext( );
	}
	return false;
}
#if 0
int HexContents::isTempAF( )
{
	Counter* c = UnitPtr;
	while ( c )
	{
		if ( c->getCounterType( ) == CNTR_MISC && ( (Facility*)c )->getMiscType( ) == Facility::TEMPAF )
			return TRUE;
		c = c->getNext( );
	}
	return FALSE;
}
#endif
bool HexContents::isPermAF( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC
			&& ( ( (Facility*)c )->getMiscType( ) == Facility::PERMAF3 || ( (Facility*)c )->getMiscType( ) == Facility::PERMAF6
				|| ( (Facility*)c )->getMiscType( ) == Facility::PERMAF9 || ( (Facility*)c )->getMiscType( ) == Facility::PERMAF12 ) )
			return true;
		c = c->getNext( );
	}
	return false;
}

int HexContents::fortLevel( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->getType( ) == GroundUnitType::WW2::FORTAREA )
			return _rule_set.FortifiedAreaLevel;
		c = c->getNext( );
	}
	return 0;
}

bool HexContents::isFort( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		// fort counters:
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::FORT )
			return true;

		// fortified area counters:
		if ( c->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)c )->getType( ) == GroundUnitType::WW2::FORTAREA )
			return true;

		c = c->getNext( );
	}
	return false;
}

bool HexContents::isMulberry( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::MULBERRY )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isFieldWork( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::FIELDWORKS )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isZepBase( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::ZEP_BASE )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isEntrenchment( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::ENTRENCHMENT )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isBalloonBarrage( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::BALLOON )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isContested( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::CONTEST )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isRP( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::RP )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isImpFort( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::MISC && ( (Facility*)c )->getMiscType( ) == Facility::IMPR_FORT )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isShip( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::SHIP )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isNote( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::NOTE )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isPts( )
{
	Counter* c = unit_list_;
	while ( c )
	{
		if ( c->getCounterType( ) == Counter::Type::PTS )
			return true;
		c = c->getNext( );
	}
	return false;
}

bool HexContents::isHit( int typ /* = 0 */ )
{
	Counter* c = unit_list_;
	if ( typ == 0 )
		while ( c )
		{
			if ( c->getCounterType( ) == Counter::Type::HIT )
				return true;
			c = c->getNext( );
		}
	else
		while ( c )
		{
			if ( c->getCounterType( ) == Counter::Type::HIT && ( (HitMarker*)c )->getHitType( ) == typ )
				return true;
			c = c->getNext( );
		}

	return false;
}

int HexContents::insertUnit( Counter* new_unit )
{
	Counter* cur_unit, *prev_unit;

	cur_unit = unit_list_;
	if ( cur_unit == NULL )  // empty list, add to the beginning
	{
		unit_list_ = new_unit;
		new_unit->setNext( NULL );
	}
	else  // list already has at least one unit
	{
		prev_unit = NULL;
		while ( TRUE )  // find a suitable place for the unit from the list
		{
			// TODO: simplify this logic
			if ( ( new_unit->getSize( ) > cur_unit->getSize( )  // the new unit is larger or (same size but stronger)
			|| ( new_unit->getSize( ) == cur_unit->getSize( )  // -> before
			&& new_unit->getAtt( ) > cur_unit->getAtt( ) ) )
				&& ! ( new_unit->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)new_unit )->getOverstack( ) ) )
			{
				if ( prev_unit == NULL )
					unit_list_ = new_unit;  // to the beginning of the list
				else
					prev_unit->setNext( new_unit );  // to the middle of the list
				new_unit->setNext( cur_unit );    // rest of the list after the new unit
				break;
			}
			if ( cur_unit->getNext( ) == NULL )
			{
				cur_unit->setNext( new_unit );  // the end of the list -> after this
				new_unit->setNext( NULL );
				break;
			}
			prev_unit = cur_unit;
			cur_unit = cur_unit->getNext( );  // go forward in the list
		}
	}
	return TRUE;
}

// insert list of units
int HexContents::insertUnits( Counter* units )
{
	Counter* next = units;
	while ( next != NULL )
	{
		insertUnit( next );
		next = next->getNext( );
	}
	return TRUE;
}

int HexContents::removeUnit( Counter* unit )
{
	Counter* next_unit;

	Counter* cur_unit = unit_list_;
	Counter* prev_unit = NULL;
	while ( TRUE )
	{
		if ( cur_unit == NULL )  // not found
			return FALSE;
		if ( cur_unit == unit )
		{  // found, remove
			next_unit = cur_unit->getNext( );
			if ( prev_unit )
				prev_unit->setNext( next_unit );  // removed from the middle of the list
			else
				unit_list_ = next_unit;  // removal from the beginning of the list
			cur_unit->setNext( NULL );
			break;
		}
		prev_unit = cur_unit;     // go forward in the list
		cur_unit = cur_unit->getNext( );
	}
	return TRUE;
}

int HexContents::getUnitCount( )
{
	int c = 0;
	Counter* unit = unit_list_;
	while ( unit )
	{
		++c;
		unit = unit->getNext( );
	}
	return c;
}

// count GroundUnits
// NOTE: some GroundUnits are not counted, because this is only used
//       for drawing nice counters to map
int HexContents::getLandUnitCount( )
{
	int c = 0;
	Counter* unit = unit_list_;
	while ( unit )
	{
		if ( unit->getCounterType( ) == Counter::Type::UNIT && ( (GroundUnit*)unit )->getType( ) != GroundUnitType::WW2::POS_HV_AA
			&& ( (GroundUnit*)unit )->getType( ) != GroundUnitType::WW2::POS_LT_AA && ( (GroundUnit*)unit )->getType( ) != GroundUnitType::WW2::TRUCK )
			++c;
		unit = unit->getNext( );
	}
	return c;
}

int HexContents::getNonLandUnitCount( )
{
	int c = 0;
	Counter* unit = unit_list_;
	while ( unit )
	{
		if ( unit->getCounterType( ) != Counter::Type::UNIT || ( (GroundUnit*)unit )->getType( ) == GroundUnitType::WW2::POS_HV_AA
			|| ( (GroundUnit*)unit )->getType( ) == GroundUnitType::WW2::POS_LT_AA )
			++c;
		unit = unit->getNext( );
	}
	return c;
}

// att and def str
float HexContents::getRawDefStr( bool supported )
{
	float s = 0.0F;

	Counter* unit = unit_list_;
	while ( unit )
	{
		if ( unit->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* gunit = static_cast<GroundUnit*>( unit );

			// overstacked units do not defend
			if ( gunit->getOverstack( ) )
			{
				unit = unit->getNext( );
				continue;
			}

			// hex is unsupported, non-self-supported halved
			if ( ! supported )
			{
				// TODO: shouldn't this check isSelfSupported( ) ??
				if ( gunit->isSupported( ) )
					s += unit->getRealDef( );
				else
					s += unit->getRealDef( ) / 2;
			}
			else // hex is supported, each unit at full strength
				s += unit->getRealDef( );
		}
		unit = unit->getNext( );
	}

	return s;
}

float HexContents::getRawAttStr( bool supported )
{
	float s = 0.0F;

	Counter* unit = unit_list_;
	while ( unit )
	{
		if ( unit->getCounterType( ) == Counter::Type::UNIT )
		{
			GroundUnit* gunit = static_cast<GroundUnit*>( unit );

			if ( gunit->getOverstack( ) )	// overstacked units do not defend
			{
				unit = unit->getNext( );
				continue;
			}

			// TODO: investigate what is going on with the -1 argument passed to the GroundUnit function
			// hex is unsupported, non-self-supported halved
			if ( ! supported )
			{
				if ( gunit->isSelfSupported( ) )
					s += unit->getRealAtt( -1 );
				else
					s += unit->getRealAtt( -1 ) / 2;
			}
			else // hex is supported, each unit at full strength
				s += unit->getRealAtt( -1 );
		}
		unit = unit->getNext( );
	}

	return s;
}

bool HexContents::isZOCSource( )
{
	Counter* unit = unit_list_;
	while ( unit )
	{
		if ( unit->getCounterType( ) == Counter::Type::UNIT && ! ( (GroundUnit*)unit )->getOverstack( ) )
		{
			if ( ((GroundUnit*)unit )->hasNormalZOC( ) )
				return true;
		}
		unit = unit->getNext( );
	}
	return false;
}

bool HexContents::isReducedZOCSource( )
{
	Counter* unit = unit_list_;
	while ( unit )
	{
		if ( unit->getCounterType( ) == Counter::Type::UNIT && ! ( (GroundUnit*)unit )->getOverstack( ) )
		{
			GroundUnit* u = (GroundUnit*)unit;
			if ( u->hasReducedZOC( ) )
				return true;
		}
		unit = unit->getNext( );
	}
	return false;
}

// calculate armor effects (not considering weather)
//	aec_type input is 1=AECA, 2=AECD, 3=ATEC, other=error
// from calculateOptimalAEC(..):
// returns:  0=None, 1=Full, 2=Half, 5=Partial (e.g. 1/7 or 1/10)
int HexContents::getAEC( Counter* ext_units, const int aec_type )
{
	float full_re = 0.0f;
	float half_re = 0.0f;
	float neut_re = 0.0f;
	float norm_re = 0.0f;

	Counter* cntr;
	for ( cntr = unit_list_; cntr != NULL; cntr = cntr->getNext( ) )
	{
		if ( cntr->getCounterType( ) != Counter::Type::UNIT )
			continue;

		// TODO: Series rules will deprecate HQs
		if ( cntr->getSize( ) == GroundUnit::Size::HQ )	// HQ == neutral
			continue;

		GroundUnit* unit = (GroundUnit*)cntr;

		if ( unit->getOverstack( ) )
			continue;

		if (	unit->getType( ) == GroundUnitType::WW2::POS_HV_AA
				|| unit->getType( ) == GroundUnitType::WW2::POS_LT_AA
				|| unit->getType( ) == GroundUnitType::WW2::TRUCK )
			continue;

		float re = cntr->getRE( );

		int aec; // this is the proportion from the UIC
		switch ( aec_type )
		{
			case AEC_AECA:
				aec = cntr->getAECA( );
				break;
			case AEC_AECD:
				aec = cntr->getAECD( );
				break;
			case AEC_ATEC:
				aec = cntr->getATEC( );
				break;
			default:
				wxMessageBox( wxT("HexContents::GetAEC: unknown aec_type"), wxT("Bug found"), wxOK );
				aec = 0;
				break;
		}

		switch ( aec )
		{
			case ArmorAntitank::Proportion::FULL:
				full_re += re;
				break;
			case ArmorAntitank::Proportion::HALF:
				half_re += re;
				break;
			case ArmorAntitank::Proportion::THIRD:
				full_re += re / 3;
				norm_re += ( re * 2 ) / 3;
				break;
			case ArmorAntitank::Proportion::NEUT:
				neut_re += re;
				break;
			case ArmorAntitank::Proportion::NONE:
				norm_re += re;
				break;
		}
	}

	for ( cntr = ext_units; cntr != NULL; cntr = cntr->getNext( ) )
	{
		if ( cntr->getCounterType( ) != Counter::Type::UNIT )
			continue;

		// TODO: Series rules will deprecate HQs
		if ( cntr->getSize( ) == GroundUnit::Size::HQ )  // HQ == neutral
			continue;

		GroundUnit* unit = (GroundUnit*)cntr;

		if ( unit->getOverstack( ) )
			continue;

		if (	unit->getType( ) == GroundUnitType::WW2::POS_HV_AA
				|| unit->getType( ) == GroundUnitType::WW2::POS_LT_AA
				|| unit->getType( ) == GroundUnitType::WW2::TRUCK )
			continue;

		float re = cntr->getRE( );

		int aec = 0;
		switch ( aec_type )
		{
			case AEC_AECA:
				aec = cntr->getAECA( );
				break;
			case AEC_AECD:
				aec = cntr->getAECD( );
				break;
			case AEC_ATEC:
				aec = cntr->getATEC( );
				break;
			default:
				wxMessageBox( wxT("HexContents::GetAEC: unknown aec_type"), wxT("Bug found"), wxOK );
				break;
		}

		switch ( aec )
		{
			case ArmorAntitank::Proportion::FULL:
				full_re += re;
				break;
			case ArmorAntitank::Proportion::HALF:
				half_re += re;
				break;
			case ArmorAntitank::Proportion::THIRD:
				full_re += re / 3;
				norm_re += ( re * 2 ) / 3;
				break;
			case ArmorAntitank::Proportion::NEUT:
				neut_re += re;
				break;
			case ArmorAntitank::Proportion::NONE:
				norm_re += re;
				break;
		}
	}

	return calculateOptimalAEC( full_re, half_re, neut_re, norm_re );
}

// add AECA and non-AECA REs to given variables
// TODO: same as GetAECA
void HexContents::addAECA_RE( float* full_re, float* half_re, float* neut_re, float* norm_re, int dir )
{
	Counter* cntr = unit_list_;
	while ( cntr != NULL )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && cntr->getAttackDir( ) == dir )
		{
			GroundUnit* unit = (GroundUnit*)cntr;
			if ( unit->getType( ) == GroundUnitType::WW2::POS_HV_AA || unit->getType( ) == GroundUnitType::WW2::POS_LT_AA || unit->getType( ) == GroundUnitType::WW2::TRUCK ) /* TODO: check this, should just be AEC_NEUT flag */
				continue;

			float re = unit->getRE( );
			switch ( unit->getAECA( ) )
			{
				case ArmorAntitank::Proportion::FULL:
					*full_re += re;
					break;
				case ArmorAntitank::Proportion::HALF:
					*half_re += re;
					break;
				case ArmorAntitank::Proportion::THIRD:
					*full_re += re / 3;
					*norm_re += re * 2 / 3;
					break;
				case ArmorAntitank::Proportion::NEUT:
					*neut_re += re;
					break;
				case ArmorAntitank::Proportion::NONE:
					*norm_re += re;
					break;
			}
		}
		cntr = cntr->getNext( );
	}
}

bool HexContents::commandoUnitAttacking( int dir )
{
	Counter* cntr = unit_list_;
	while ( cntr != NULL )
	{
		GroundUnit* unit = (GroundUnit*)cntr;
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && cntr->getAttackDir( ) == dir
			&& ( unit->getType( ) == GroundUnitType::WW2::CMDO || unit->getType( ) == GroundUnitType::WW2::MARINE_CMDO || unit->getType( ) == GroundUnitType::WW2::MECH_CMDO
				|| unit->getType( ) == GroundUnitType::WW2::PARA_CMDO ) )
			return true;
		cntr = cntr->getNext( );
	}
	return false;
}
void HexContents::addEng_RE( float* eng_re, float* norm_re, int dir )
{
	Counter* cntr = unit_list_;
	while ( cntr != NULL )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && cntr->getAttackDir( ) == dir )
		{
			/* do not count artillery, i.e in neither the numerator
			 * nor the denominator of the engineer proportion */
			GroundUnit* unit = (GroundUnit*)cntr;
			if ( unit->getType( ) == GroundUnitType::WW2::POS_HV_AA // TODO: true for AEC/ATEC
			|| unit->getType( ) == GroundUnitType::WW2::POS_LT_AA // but true for engineering?
			|| unit->getType( ) == GroundUnitType::WW2::TRUCK // TODO: check this
			|| unit->isArtillery( ) )
				continue;

			if ( unit->isAssaultEngineer( ) )
				*eng_re += unit->getRE( ) * 2;
			else if ( unit->isCombatEngineer( ) )
				*eng_re += unit->getRE( );
			else
				*norm_re += unit->getRE( );
		}
		cntr = cntr->getNext( );
	}
}

/**
 * call this function only for an attack across a fortified hexside
 */
void HexContents::addNeut_RE( float* neut_re, float* norm_re, int dir )
{
	Counter* cntr = unit_list_;
	while ( cntr != NULL )
	{
		if ( cntr->getCounterType( ) == Counter::Type::UNIT && cntr->getAttackDir( ) == dir )
		{
			/* exclude positional AA from all AEC/ATEC calculations
			 * this supersedes the fortified hexside rule [11I2] */
			GroundUnit* unit = (GroundUnit*)cntr;
			if (	unit->getType( ) == GroundUnitType::WW2::POS_HV_AA
					|| unit->getType( ) == GroundUnitType::WW2::POS_LT_AA
					|| unit->getType( ) == GroundUnitType::WW2::TRUCK ) /* TODO: check this, should be non-capable */
				continue;  // don't count above types in any part of AEC calculation

			/* the half-capable neutral election is not allowed here --
			 * only artillery are neutral across a fortified hexside */
			if ( unit->isArtillery( ) )
				*neut_re += unit->getRE( );
			else
				*norm_re += unit->getRE( );
		}
		cntr = cntr->getNext( );
	}
}

int HexContents::getAECA( Counter* ext_units )
{
	return getAEC( ext_units, AEC_AECA );
}

int HexContents::getAECD( Counter* ext_units )
{
	return getAEC( ext_units, AEC_AECD );
}

int HexContents::getATEC( Counter* ext_units )
{
	return getAEC( ext_units, AEC_ATEC );
}

bool HexContents::update_hex_situation( Counter* c )
{
	bool units_in_hex = false;

	while ( c != nullptr )
	{
		if ( c->getCounterType( ) == Counter::Type::UNIT )
			units_in_hex = true;
		else if ( c->getCounterType( ) == Counter::Type::RP )
			units_in_hex = true;
		else if ( c->getCounterType( ) == Counter::Type::PLANE )
		{
			setAirUnits( true );
			setPlaneOwner( c->isAxis( ) ? SidePlayer::AXIS : SidePlayer::ALLIED );
		}
		else if ( c->getCounterType( ) == Counter::Type::NOTE && static_cast<HexNote*>( c )->getNoteType( ) == HexNote::ALERT )
			setAlert( true );

		c = c->getNext( );
	}

	return units_in_hex;
}

// update cache-data of this hex, ext_units is list of selected units
void HexContents::updateCaches( int str, Counter* ext_units )
{
	// figure out what should be drawn in this hex
	defensive_strength_ = str;
	AEC_ = static_cast<unsigned char>( getAECA( ext_units ) );
	setAirUnits( false );
	setAlert( false );

	units_in_hex_ = update_hex_situation( unit_list_ ) ? true : false;

	units_in_hex_ = update_hex_situation( ext_units ) ? true : units_in_hex_;

#if 0
	Counter* unit = unit_list_;
	while ( unit != nullptr )
	{
		if ( update_hex_situation( unit ) )
			units_in_hex_ = true;
		unit = unit->getNext( );
	}

	unit = ext_units;
	while ( unit != nullptr )
	{
		if ( update_hex_situation( unit ) )
			units_in_hex_ = true;
		unit = unit->getNext( );
	}
#endif

	// determine what color should be used to draw corps marker
	if ( unit_list_ )
	{
		// use any ground unit's subtype for corps marker color
		bool set = false;
		Counter* cp = unit_list_;
		while ( cp )
		{
			if ( cp->getCounterType( ) == Counter::Type::UNIT )
			{
				setSubType( ( (GroundUnit*)cp )->getSubType( ) );
				set = true;
				break;
			}
			cp = cp->getNext( );
		}
		// if no ground unit present, use any air unit's subtype
		if ( ! set )
		{
			if ( unit_list_->getCounterType( ) == Counter::Type::PLANE )
			{
				setSubType( static_cast<AirUnit*>( unit_list_ )->getSubType( ) );
				set = true;
			}
		}
		if ( ! set )
			setSubType( 0 );
	}
	else if ( ext_units ) // e.g. when all counters in hex have been selected
	{
		if ( ext_units->getCounterType( ) == Counter::Type::UNIT )
			setSubType( ( (GroundUnit*)ext_units )->getSubType( ) );
		else if ( ext_units->getCounterType( ) == Counter::Type::PLANE )
			setSubType( ( (AirUnit*)ext_units )->getSubType( ) );
		else
			setSubType( 0 );
	}

	// make all notes and status markers owned by new owner
	Counter* unit = unit_list_;
	while ( unit )
	{
		if (	unit->getCounterType( ) == Counter::Type::MISC
				|| unit->getCounterType( ) == Counter::Type::HIT
				|| unit->getCounterType( ) == Counter::Type::PTS    // status marker
				|| unit->getCounterType( ) == Counter::Type::RP
				|| unit->getCounterType( ) == Counter::Type::NOTE )
			unit->setSide( getOwner( ) );

		unit = unit->getNext( );
	}
}

char msgx[1000];  		// tmp buffer for user messages
char tmpx[100];
int linenum = 0;

void check_count( int n )
{
	if ( n < 0 || n > 100 )
	{
		sprintf( msgx, "Line %d Invalid %s Count %d", linenum, tmpx, n );
		wxMessageBox( wxS( msgx ), wxT("Load Problem") );
	}
}

void bad_counter( void )
{
	wxMessageBox( wxT("Out of memory"), wxT("Fatal error"), wxICON_HAND );
	exit( -1 );
}

std::istream& operator>>( std::istream& is, HexContents& sit )
{
	int b;
	is >> b;
	sit.rail_break_ =	static_cast<unsigned char>( b );

	is >> b;
	sit.DAS_ =			static_cast<unsigned char>( b );

	is >> b;
	sit.defensive_strength_ = static_cast<unsigned char>( b );

	is >> b;
	sit.data1_ =		static_cast<unsigned char>( b );

	is >> b;
	sit.data2_ =		static_cast<unsigned char>( b );

	is >> b;
	sit.data3_ =		static_cast<unsigned char>( b );

	is >> b;
	sit.data4_ =		static_cast<unsigned char>( b );

	is >> b;
	sit.data5_ =		static_cast<unsigned char>( b );

	is >> b;
	sit.data6_ =		static_cast<unsigned char>( b );

	is >> b;
	sit.data7_ =		static_cast<unsigned char>( b );

	b = 8;	// 8: default country ID NONE
	if ( _scn_version >= 2 && _scn_version < 12 )
	{
		int dummy;		// JET versions 2 through 9 did have a data8_
		is >> dummy;	// just consume the old data, use default b = 8
	}
	if ( _scn_version >= 12 )
		is >> b;
	sit.data8_ =		static_cast<unsigned char>( b );

	b = 32;	// 32: default seazone ID NONE
	if ( _scn_version >= 12 )
		is >> b;
	sit.data9_ =		static_cast<unsigned char>( b );

	// clear current unit_list_ before in-processing Counters from .scn file
	Counter* cp = sit.unit_list_;
	while ( cp )
	{
		Counter* cnp = cp->getNext( );
		delete cp;
		cp = cnp;
	}

	Counter** cdp = &( sit.unit_list_ );

	// increment for diagnostic reporting from check_cnt(..)
	++linenum;

	int cnt;

	strcpy( tmpx, "GroundUnit" );
	is >> cnt;
	check_count( cnt );

	for ( int i = 0; i < cnt; ++i )
	{
		GroundUnit* gu = new GroundUnit( );
		if ( gu == nullptr )
			bad_counter( );
		is >> *gu;
		*cdp = gu;
		cdp = &( gu->next_ );
	}

	strcpy( tmpx, "AirUnit" );
	is >> cnt;
	check_count( cnt );

	for ( int i = 0; i < cnt; ++i )
	{
		AirUnit* au = new AirUnit( );
		if ( au == nullptr )
			bad_counter( );
		is >> *au;
		*cdp = au;
		cdp = &( au->next_ );
	}

	strcpy( tmpx, "StatusMarker" );
	is >> cnt;
	check_count( cnt );

	for ( int i = 0; i < cnt; ++i )
	{
		StatusMarker* sm = new StatusMarker( );
		if ( sm == nullptr )
			bad_counter( );
		is >> *sm;
		*cdp = sm;
		cdp = &( sm->next_ );
	}

	strcpy( tmpx, "Resource Marker" );
	is >> cnt;
	check_count( cnt );

	for ( int i = 0; i < cnt; ++i )
	{
		ResourceMarker* rm = new ResourceMarker( );
		if ( rm == nullptr )
			bad_counter( );
		is >> *rm;
		*cdp = rm;
		cdp = &( rm->next_ );
	}

	strcpy( tmpx, "Facility" );
	is >> cnt;
	check_count( cnt );

	for ( int i = 0; i < cnt; ++i )
	{
		Facility* fac = new Facility( );
		if ( fac == nullptr )
		{
			wxMessageBox( wxT("Out of memory"), wxT("Fatal error"), wxICON_HAND );
			exit( -1 );
		}
		is >> *fac;
		*cdp = fac;
		cdp = &( fac->next_ );
	}

	strcpy( tmpx, "NavalUnit" );
	is >> cnt;
	check_count( cnt );

	for ( int i = 0; i < cnt; ++i )
	{
		NavalUnit* nu = new NavalUnit( );
		if ( nu == nullptr )
			bad_counter( );
		is >> *nu;
		*cdp = nu;
		cdp = &( nu->next_ );
	}

	strcpy( tmpx, "HexNote" );
	is >> cnt;
	check_count( cnt );

	for ( int i = 0; i < cnt; ++i )
	{
		HexNote* hn = new HexNote( );
		if ( hn == nullptr )
			bad_counter( );
		is >> *hn;
		*cdp = hn;
		cdp = &( hn->next_ );
	}

	return is;
}

std::ostream& operator<<( std::ostream& o, HexContents& sit )
{
	o << static_cast<int>( sit.rail_break_ ) << ' ';
	o << static_cast<int>( sit.DAS_ ) << ' ';
	o << static_cast<int>( sit.defensive_strength_ ) << ' ';
	o << static_cast<int>( sit.data1_ ) << ' ';
	o << static_cast<int>( sit.data2_ ) << ' ';
	o << static_cast<int>( sit.data3_ ) << ' ';
	o << static_cast<int>( sit.data4_ ) << ' ';
	o << static_cast<int>( sit.data5_ ) << ' ';
	o << static_cast<int>( sit.data6_ ) << ' ';
	o << static_cast<int>( sit.data7_ ) << ' ';
	o << static_cast<int>( sit.data8_ ) << ' ';
	o << static_cast<int>( sit.data9_ ) << ' ';

	int num_gu = 0; // GroundUnit
	std::vector<Counter*> gu;

	int num_au = 0; // AirUnit
	std::vector<Counter*> au;

	int num_sm = 0; // StatusMarker
	std::vector<Counter*> sm;

	int num_rm = 0; // ResourceMarker
	std::vector<Counter*> rm;

	int num_fac = 0; // Facility
	std::vector<Counter*> fac;

	int num_nu = 0; // NavalUnit
	std::vector<Counter*> nu;

	int num_hn = 0; // HexNote
	std::vector<Counter*> hn;

	// populate counter-type-specific lists and count each
	Counter* cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		switch ( cp->getCounterType( ) )
		{
			case Counter::Type::UNIT:
				gu.push_back( cp );		++num_gu;		break;
			case Counter::Type::PLANE:
				au.push_back( cp );		++num_au;		break;
			case Counter::Type::PTS:
				sm.push_back( cp );		++num_sm;		break;
			case Counter::Type::RP:
				rm.push_back( cp );		++num_rm;		break;
			case Counter::Type::MISC:
				fac.push_back( cp );	++num_fac;		break;
			case Counter::Type::SHIP:
				nu.push_back( cp );		++num_nu;		break;
			case Counter::Type::NOTE:
				hn.push_back( cp );		++num_hn;		break;
			case Counter::Type::HIT:	// not saved
			case Counter::Type::NAT:	// not yet implemented
			default:
				break;
		}
		cp = cp->getNext( );
	}

	o << num_gu << ' ';
	for ( int i = 0; i < num_gu; ++i )
		o << *( static_cast<GroundUnit*>( gu[i] ) ) << ' ';

	o << num_au << ' ';
	for ( int i = 0; i < num_au; ++i )
		o << *( static_cast<AirUnit*>( au[i] ) ) << ' ';

	o << num_sm << ' ';
	for ( int i = 0; i < num_sm; ++i )
		o << *( static_cast<StatusMarker*>( sm[i] ) ) << ' ';

	o << num_rm << ' ';
	for ( int i = 0; i < num_rm; ++i )
		o << *( static_cast<ResourceMarker*>( rm[i] ) ) << ' ';

	o << num_fac << ' ';
	for ( int i = 0; i < num_fac; ++i )
		o << *( static_cast<Facility*>( fac[i] ) ) << ' ';

	o << num_nu << ' ';
	for ( int i = 0; i < num_nu; ++i )
		o << *( static_cast<NavalUnit*>( nu[i] ) ) << ' ';

	o << num_hn << ' ';
	for ( int i = 0; i < num_hn; ++i )
		o << *( static_cast<HexNote*>( hn[i] ) ) << ' ';

	o << endl;
	return o;

#if 0
	// store ground unit count //
	int i = 0;
	Counter* cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_UNIT )
			++i;
		cp = cp->getNext( );
	}
	o << i << ' ';

	// store ground units //
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_UNIT )
			o << *(GroundUnit*)cp << ' ';
		cp = cp->getNext( );
	}

	// store air unit count //
	i = 0;
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_PLANE )
			++i;
		cp = cp->getNext( );
	}
	o << i << ' ';

	// store air units //
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_PLANE )
			o << *(AirUnit*)cp << ' ';
		cp = cp->getNext( );
	}

	// store status marker count //
	i = 0;
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_PTS )
			++i;
		cp = cp->getNext( );
	}
	o << i << ' ';

	// store status markers //
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_PTS )
			o << *(StatusMarker*)cp << ' ';
		cp = cp->getNext( );
	}

	// store resource point marker count //
	i = 0;
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_RP )
			++i;
		cp = cp->getNext( );
	}
	o << i << ' ';

	// store resource point markers //
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_RP )
			o << *(ResourceMarker*)cp << ' ';
		cp = cp->getNext( );
	}

	// store miscellaneous marker count //
	i = 0;
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_MISC )
			++i;
		cp = cp->getNext( );
	}
	o << i << ' ';

	// store miscellaneous markers //
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_MISC )
			o << *(Facility*)cp << ' ';
		cp = cp->getNext( );
	}

	// store naval unit count //
	i = 0;
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_SHIP )
			++i;
		cp = cp->getNext( );
	}
	o << i << ' ';

	// store naval units //
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_SHIP )
			o << *(NavalUnit*)cp << ' ';
		cp = cp->getNext( );
	}

	// store note count //
	i = 0;
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_NOTE )
			++i;
		cp = cp->getNext( );
	}
	o << i << ' ';

	// store notes //
	cp = sit.unit_list_;
	while ( cp != nullptr )
	{
		if ( cp->getCounterType( ) == CNTR_NOTE )
			o << *(HexNote*)cp << ' ';
		cp = cp->getNext( );
	}

	o << endl;
	return o;
#endif
}

#endif
