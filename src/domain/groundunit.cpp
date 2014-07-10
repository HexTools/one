#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;

#include <wx/wx.h>

#include "hextools.h"

#include "application.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"

#include "hextools.h"
using ht::wxS;
using ht::aec2str;
using ht::spc2line;
using ht::line2spc;
using ht::strLen;
using ht::drawStr;
using ht::isOkStr;
using ht::inttoa;
using ht::limit;

#include "rulesvariant.h"
#include "rules.h"
#include "sideplayer.h"
#include "alliedcoalition.h"
#include "phase.h"
#include "zoc.h"
#include "supplystatus.h"
#include "disruptionstatus.h"
#include "counter.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "armorantitank.h"
#include "groundunittype.h"
#include "groundunitsubtype.h"
#include "groundunit.h"
#include "dgroundunit.h"

extern int _scn_version;

extern int _current_phase;  // COMBAT_PHASE etc.

extern int _year;
extern int _month;
#if 0
extern int Turn;
#endif
extern int _phasing_player;
#if 0
extern int Turns;
extern const char* months[12];
extern const char* turns[20];
extern const char* players[3];
extern void LogDebugString( wxString descr, wxString logdata );
#endif
extern Rules _rule_set;

extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)

extern GroundUnitType _ground_unit_type[GroundUnitType::WW2::UNITTYPECOUNT];
extern GroundArmedForce _ground_armed_force[2][GroundArmedForce::Allied::COUNT];

extern UnitDlgXfer _unit_dialog_data;

extern int _phasing_player;  // phasing player, application.cpp
extern int _year;  // application.cpp
extern int _month;  // application.cpp
extern int _show_combat_markers;

// defined, loaded and destroyed in the mappane.cpp
extern wxBitmap* hbmCompany;
extern wxBitmap* hbmBatallion;
extern wxBitmap* hbmBrigade;
extern wxBitmap* hbmRegiment;
extern wxBitmap* hbmDivisional;
extern wxBitmap* hbmDivision;
extern wxBitmap* hbmCorps;
extern wxBitmap* hbmArmy;
extern wxBitmap* hbmCadre;
extern wxBitmap* hbmDG;
extern wxBitmap* hbm1RE;  // for transports/APC's
extern wxBitmap* hbm2RE;
extern wxBitmap* hbm3RE;
extern wxBitmap* hbmMtnMask;
extern wxBitmap* hbmBatallionGrp;
extern wxBitmap* hbmBrigadeGrp;
extern wxBitmap* hbmRegimentGrp;
#if 0
extern wxBitmap* hbmDieRollReport;

extern wxBitmap* hbmFont[127];  // digits for numbers in counters, mappane.cpp
#endif

extern wxWindow* _parent_dialog_window;

// fonts seem to differ from one platform to another:
// X11/GTK:
#if defined( __WXGTK__ )
wxFont fnt(12, wxDEFAULT, wxNORMAL, wxNORMAL);
#if 0
wxFont fnt2(8, wxSWISS, wxNORMAL, wxNORMAL);
#endif
wxFont fnt2(8, wxROMAN, wxNORMAL, wxNORMAL);
// Windows:
#elif defined (__WXMSW__)
wxFont fnt( 7, wxDEFAULT, wxNORMAL, wxNORMAL );
wxFont fnt2( 6, wxSWISS, wxNORMAL, wxNORMAL );
// MacOS probably needs some other fonts:
#else
#error "Please define some fonts!"
#endif

// values MUST be initialized before using (e.g. loaded from file).
GroundUnit::GroundUnit( )
		: Counter( )
{
	counter_type_ = Counter::Type::UNIT;
	attack_strength_ = 0;
	defense_strength_ = 0;
	cadre_strength_ = 0;
	unit_flags_ = 0;
	// TODO: what about default for WW1?
	type_ = GroundUnitType::WW2::INF;
	subtype_ = 0;
	next_ = nullptr;
	data1_ = data2_ = data3_ = data4_ = data5_ = data6_ = 0;
	setSize( GroundUnit::Size::INVALID_SIZE );
	current_MPs_ = 0;
	maximum_MPs_ = 0;
	bmpCounter = 0;
	image_ok_ = false;
	allow_cache_ = true;
}

GroundUnit::~GroundUnit( )
{
}

bool GroundUnit::isGerman( )
{
	if ( getSide( ) == SidePlayer::AXIS
		&& ( getSubType( ) == GroundArmedForce::Axis::GER || getSubType( ) == GroundArmedForce::Axis::LW || getSubType( ) == GroundArmedForce::Axis::SA || getSubType( ) == GroundArmedForce::Axis::BRAND
			|| getSubType( ) == GroundArmedForce::Axis::KM || getSubType( ) == GroundArmedForce::Axis::OKW || getSubType( ) == GroundArmedForce::Axis::HY || getSubType( ) == GroundArmedForce::Axis::AX_FOR
			|| getSubType( ) == GroundArmedForce::Axis::AX_EAST || getSubType( ) == GroundArmedForce::Axis::SS || getSubType( ) == GroundArmedForce::Axis::SSPOL || getSubType( ) == GroundArmedForce::Axis::AX_WW1_BAV
			||  // ww1
			getSubType( ) == GroundArmedForce::Axis::AX_WW1_PRUS ||  // ww1
			getSubType( ) == GroundArmedForce::Axis::AX_WW1_SAX ||  // ww1
			getSubType( ) == GroundArmedForce::Axis::AX_WW1_WURT ||  // ww1
			getSubType( ) == GroundArmedForce::Axis::AX_WW1_GCOL ||  // ww1
			getSubType( ) == GroundArmedForce::Axis::AX_WW1_GERAF ||  // ww1
			getSubType( ) == GroundArmedForce::Axis::AX_WW1_GERNA  // ww1
		) )
		return true;
	return false;
}

// FWtBT: is Loyalist
bool GroundUnit::isLoyalist( )
{
	if ( getSide( ) == SidePlayer::ALLIED
		&& ( getSubType( ) == GroundArmedForce::Allied::AL_PPL || getSubType( ) == GroundArmedForce::Allied::AL_INTR || getSubType( ) == GroundArmedForce::Allied::AL_ASTU || getSubType( ) == GroundArmedForce::Allied::AL_BASQ
			|| getSubType( ) == GroundArmedForce::Allied::AL_CATA || getSubType( ) == GroundArmedForce::Allied::AL_SANT || getSubType( ) == GroundArmedForce::Allied::AL_ANAR
			|| getSubType( ) == GroundArmedForce::Allied::AL_POUM ) )
		return true;
	return false;
}

bool GroundUnit::isSoviet( )
{
	if ( getSide( ) == SidePlayer::ALLIED
		&& ( getSubType( ) == GroundArmedForce::Allied::SOV || getSubType( ) == GroundArmedForce::Allied::GUARDS || getSubType( ) == GroundArmedForce::Allied::NKVD || getSubType( ) == GroundArmedForce::Allied::SOVNAVY
			|| getSubType( ) == GroundArmedForce::Allied::SOVW || getSubType( ) == GroundArmedForce::Allied::SOVM || getSubType( ) == GroundArmedForce::Allied::SOVF ) )
		return true;
	return false;
}

bool GroundUnit::isItalian( )
{
	if ( getSide( ) == SidePlayer::AXIS && ( getSubType( ) == GroundArmedForce::Axis::ITA || getSubType( ) == GroundArmedForce::Axis::RSI || getSubType( ) == GroundArmedForce::Axis::CCNN ) )
		return true;
	return false;
}

bool GroundUnit::isBritish( )
{
	if ( getSide( ) == SidePlayer::ALLIED
		&& ( getSubType( ) == GroundArmedForce::Allied::CANADA || getSubType( ) == GroundArmedForce::Allied::BRITISH || getSubType( ) == GroundArmedForce::Allied::RAF || getSubType( ) == GroundArmedForce::Allied::INDIAN
			|| getSubType( ) == GroundArmedForce::Allied::NZ || getSubType( ) == GroundArmedForce::Allied::RNAVY || getSubType( ) == GroundArmedForce::Allied::RMC || getSubType( ) == GroundArmedForce::Allied::SAFR
			|| getSubType( ) == GroundArmedForce::Allied::INDSF || getSubType( ) == GroundArmedForce::Allied::BRCOL || getSubType( ) == GroundArmedForce::Allied::BREMP || getSubType( ) == GroundArmedForce::Allied::AUST
			|| getSubType( ) == GroundArmedForce::Allied::FREEFR ) )
		return true;
	return false;
}

bool GroundUnit::isUS( )
{
	if ( getSide( ) == SidePlayer::ALLIED
		&& ( getSubType( ) == GroundArmedForce::Allied::USA || getSubType( ) == GroundArmedForce::Allied::USAAF || getSubType( ) == GroundArmedForce::Allied::USNAVY || getSubType( ) == GroundArmedForce::Allied::OSS
			|| getSubType( ) == GroundArmedForce::Allied::USCAN || getSubType( ) == GroundArmedForce::Allied::USMC ) )
		return true;
	return false;
}

bool GroundUnit::isWW1Infantry( )
{
	// any unit with 8 MPs (or more??)
	if ( getMaxMP( ) >= 8 )
		return true;

	// Aslt Eng with 6 or more MPs
	if ( ( getType( ) == GroundUnitType::WW2::ASLT_ENG || getType( ) == GroundUnitType::WW2::AMPH_ASLT_ENG ) && getMaxMP( ) >= 6 )
	// inf types:
		if (	getType( ) == GroundUnitType::WW2::INF ||
				getType( ) == GroundUnitType::WW2::LT_INF ||
				getType( ) == GroundUnitType::WW2::MTN ||
				getType( ) == GroundUnitType::WW2::MARINE ||
				getType( ) == GroundUnitType::WW2::MARINE_CMDO ||
				getType( ) == GroundUnitType::WW2::MG ||
				getType( ) == GroundUnitType::WW2::NVL_TRPS ||
				getType( ) == GroundUnitType::WW2::PARA_INF	||
				getType( ) == GroundUnitType::WW2::PARA ||
				getType( ) == GroundUnitType::WW2::PARA_CMDO ||
				getType( ) == GroundUnitType::WW2::ALPINI ||
#if 0
			GetType() == GroundUnitType::WW2::BERS ||
			GetType() == GroundUnitType::WW2::CHSSR ||
#endif
				getType( ) == GroundUnitType::WW2::STOSS ||
				getType( ) == GroundUnitType::WW2::STOSSMG ||
				getType( ) == GroundUnitType::WW2::STURM	)  // ???
			return true;
	return false;
}

// FitE/SE winterization during first winter
bool GroundUnit::isWinterized( )
{
	// ski and mountain units are always winterized
	if ( isMtn( ) )
		return true;

	if ( getSide( ) == SidePlayer::ALLIED )
	{
		if ( getSubType( ) == GroundArmedForce::Allied::GUARDS || getSubType( ) == GroundArmedForce::Allied::NKVD || getSubType( ) == GroundArmedForce::Allied::SOVNAVY || getSubType( ) == GroundArmedForce::Allied::SOVW )
			return true;
		return false;
	}
	else  // axis
	{
		if ( getSubType( ) == GroundArmedForce::Axis::FIN || getSubType( ) == GroundArmedForce::Axis::LW || getSubType( ) == GroundArmedForce::Axis::SS || getSubType( ) == GroundArmedForce::Axis::SSPOL
			|| getSubType( ) == GroundArmedForce::Axis::AX_GERWINT )
			return true;
		return false;
	}
}

// This is in half hexes
int GroundUnit::getSupplyRange( void )
{
	int sr = 0;

	if ( isGerman( ) )
		switch ( _year )
		{
			case 41:
				sr = 8 * 2;
				break;
			default:
				sr = 6 * 2;
				break;
		}
	else if ( isItalian( ) )
		switch ( _year )
		{
			case 43:
			case 44:
			case 45:
				sr = 4 * 2;
				break;
			default:
				sr = 6 * 2;
				break;
		}
	else if ( isBritish( ) )
		switch ( _year )
		{
			case 43:
			case 44:
			case 45:
				sr = 8 * 2;
				break;
			default:
				sr = 6 * 2;
				break;
		}
	else if ( isUS( ) )
		switch ( _year )
		{
			case 41:
				sr = 6 * 2;
				break;
			case 42:
				sr = 8 * 2;
				break;
			case 43:
			case 44:
			case 45:
				sr = 10 * 2;
				break;
			default:
				sr = 4 * 2;
				break;
		}
	else if ( isSoviet( ) )
		switch ( _year )
		{
			case 43:
			case 44:
				sr = 6 * 2;
				break;
			case 45:
				sr = 8 * 2;
				break;
			default:
				sr = 4 * 2;
				break;
		}
	else
		sr = 6 * 2;

	return sr;
}

int GroundUnit::getAtt( void )
{
	return getFlag( GroundUnit::IS_CADRE ) ? getCdrAtt( ) : attack_strength_; // cadre is 40% of XX
}

int GroundUnit::getDef( void )
{
	return getFlag( GroundUnit::IS_CADRE ) ? getCdrDef( ) : defense_strength_; // cadre is 40% of XX
}

float GroundUnit::getRealAtt( void )
{
	float fa = getAtt( );

	if ( getOverstack( ) )
		return 0;

	if ( getType( ) == GroundUnitType::WW2::PARTISAN )
	{
		if ( getSide( ) == SidePlayer::AXIS && _rule_set.HideAxPartisans == TRUE )
			return 0;
		if ( getSide( ) == SidePlayer::ALLIED && _rule_set.HideAlPartisans == TRUE )
			return 0;
	}

#if 0
	if ( _rule_set.rules_variant_ == RulesVariant::A )
	{
		if ( getType( ) == GroundUnitType::WW2::COASTART )
			fa /= 4;

		int DefendingSide = _rule_set.AlliedCombatSide;
		if ( defender >= 0 )
			DefendingSide = defender;

		switch ( DefendingSide )
		// 0 = Other, 1 = British, 2 = French, 3 = Vichy French, 4 = Soviet, 5 = USA, 6 = Greeks
		{
			case AlliedCoalition::OTHER:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						if ( isGerman( ) )
							fa *= 3;
						break;
					case 41:
						if ( isGerman( ) )
							fa *= 3;
						break;
					case 42:
						if ( isGerman( ) )
							fa *= 2.5;
						break;
					case 43:
						if ( isGerman( ) )
							fa *= 2.5;
						break;
					case 44:
						if ( isGerman( ) )
							fa *= 2.5;
						break;
					case 45:
						if ( isGerman( ) )
							fa *= 2;
						break;
				}
				break;
			case AlliedCoalition::BRITISH:
				switch ( _year )
				{
					case 39:
						if ( isGerman( ) )
							fa *= 2;
						break;
					case 40:
						if ( isGerman( ) )
							fa *= 2;
						break;
					case 41:
						if ( isGerman( ) )
							fa *= 1.5;
						break;
					case 42:
						if ( isGerman( ) )
							fa *= 1.3;
						break;
					case 43:
						if ( isGerman( ) )
							fa *= 1.3;
						break;
					case 44:
						if ( isGerman( ) )
							fa *= 1.3;
						break;
					case 45:
						if ( isGerman( ) )
							fa *= 1.2;
						break;
				}
				break;
			case AlliedCoalition::FRENCH:
				switch ( _year )
				{
					case 39:
						if ( isGerman( ) )
							fa *= 2;
						break;
					case 40:
						if ( isGerman( ) )
							fa *= 2;
						break;
					case 41:
						break;
					case 42:
						break;
					case 43:
						break;
					case 44:
						break;
					case 45:
						break;
				}
				break;
			case AlliedCoalition::VICHY:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						break;
					case 41:
						if ( isGerman( ) )
							fa *= 3;
						break;
					case 42:
						if ( isGerman( ) )
							fa *= 3;
						break;
					case 43:
						break;
					case 44:
						break;
					case 45:
						break;
				}
				break;
			case AlliedCoalition::SOVIET:
				switch ( _year )
				{
					case 39:
						if ( isGerman( ) )
							fa *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fa *= 2;
						break;
					case 40:
						if ( isGerman( ) )
							fa *= 2.75;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fa *= 2;
						break;
					case 41:
						if ( isGerman( ) )
							fa *= 3;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fa *= 2;
						break;
					case 42:
						if ( isGerman( ) )
							fa *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fa *= 2;
						break;
					case 43:
						if ( isGerman( ) )
							fa *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fa *= 2;
						break;
					case 44:
						if ( isGerman( ) )
							fa *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fa *= 2;
						break;
					case 45:
						if ( isGerman( ) )
							fa *= 2;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fa *= 1.5;
						break;
				}
				break;
			case AlliedCoalition::USA:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						break;
					case 41:
						break;
					case 42:
						if ( isGerman( ) )
							fa *= 1.5;
						break;
					case 43:
						if ( isGerman( ) )
							fa *= 1.3;
						break;
					case 44:
						if ( isGerman( ) )
							fa *= 1.2;
						break;
					case 45:
						if ( isGerman( ) )
							fa *= 1.1;
						break;
				}
				break;
			case AlliedCoalition::GREEKS:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						if ( isGerman( ) )
							fa *= 2;
						break;
					case 41:
						if ( isGerman( ) )
							fa *= 1.7;
						break;
					case 42:
						break;
					case 43:
						break;
					case 44:
						break;
					case 45:
						break;
				}
				break;
		}
	}
#endif

	// chk supply
	if ( getSupply( ) == SupplyStatus::NONE )
	{
#if 0
		if ( _rule_set.rules_variant_ == RulesVariant::A )
		{
			if ( getIsolated( ) )
			{
				if ( getSupplyTurns( ) >= 5 )  // U3 isolated or worse, zero attack
					fa = 0;
				else if ( getSupplyTurns( ) >= 3 )  // U2 isolated or worse, attack quartered
					fa /= 4;
				else
					// U1 isolated attack halved
					fa /= 2;
			}
			else
			{
				if ( getSupplyTurns( ) >= 7 )  // U4 or worse, zero attack
					fa = 0;
				else if ( getSupplyTurns( ) >= 3 )  // U2 or worse, attack quartered
					fa /= 4;
				else
					// U1 attack halved
					fa /= 2;
			}
		}
		else
#endif
		if ( getSupplyTurns( ) >= 3 )  // U2 or worse, halved anyway
		{
			fa /= 2;
		}
		else
		{
			if ( _rule_set.OnlyIsolU1Halved == FALSE )  // old sys: U1 halved always
				fa /= 2;
			else if ( getIsolated( ) )  // new sys: U1 halved if isolated
				fa /= 2;
		}
	}
	// chk disruption
	if ( getDisruption( ) == DisruptionStatus::DISRUPTED )
		fa /= 2;
	else if ( getDisruption( ) == DisruptionStatus::BADLY_DISRUPTED )
		fa = 0;

	return fa;
}

float GroundUnit::getRealDef( void )
{
	if ( getOverstack( ) )
		return 0;

	if ( getType( ) == GroundUnitType::WW2::PARTISAN )
	{
		if ( getSide( ) == SidePlayer::AXIS && _rule_set.HideAxPartisans == TRUE )
			return 100;
		if ( getSide( ) == SidePlayer::ALLIED && _rule_set.HideAlPartisans == TRUE )
			return 100;
	}

	float fd = (float)getDef( );

#if 0
	if ( _rule_set.rules_variant_ == RulesVariant::A )
	{
		if ( getType( ) == GroundUnitType::WW2::COASTART )
			fd = (float)getMaxMP( ) / 4;

		int AttackingSide = _rule_set.AlliedCombatSide;
		if ( attacker >= 0 )
			AttackingSide = attacker;

		switch ( AttackingSide )
		// 0 = Other, 1 = British, 2 = French, 3 = Vichy French, 4 = Soviet, 5 = USA, 6 = Greeks
		{
			case AlliedCoalition::OTHER:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						if ( isGerman( ) )
							fd *= 3;
						break;
					case 41:
						if ( isGerman( ) )
							fd *= 3;
						break;
					case 42:
						if ( isGerman( ) )
							fd *= 2.5;
						break;
					case 43:
						if ( isGerman( ) )
							fd *= 2.5;
						break;
					case 44:
						if ( isGerman( ) )
							fd *= 2.5;
						break;
					case 45:
						if ( isGerman( ) )
							fd *= 2;
						break;
				}
				break;
			case AlliedCoalition::BRITISH:
				switch ( _year )
				{
					case 39:
						if ( isGerman( ) )
							fd *= 2;
						break;
					case 40:
						if ( isGerman( ) )
							fd *= 2;
						break;
					case 41:
						if ( isGerman( ) )
							fd *= 1.5;
						break;
					case 42:
						if ( isGerman( ) )
							fd *= 1.3;
						break;
					case 43:
						if ( isGerman( ) )
							fd *= 1.3;
						break;
					case 44:
						if ( isGerman( ) )
							fd *= 1.3;
						break;
					case 45:
						if ( isGerman( ) )
							fd *= 1.2;
						break;
				}
				break;
			case AlliedCoalition::FRENCH:
				switch ( _year )
				{
					case 39:
						if ( isGerman( ) )
							fd *= 2;
						break;
					case 40:
						if ( isGerman( ) )
							fd *= 2;
						break;
					case 41:
						break;
					case 42:
						break;
					case 43:
						break;
					case 44:
						break;
					case 45:
						break;
				}
				break;
			case AlliedCoalition::VICHY:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						break;
					case 41:
						if ( isGerman( ) )
							fd *= 3;
						break;
					case 42:
						if ( isGerman( ) )
							fd *= 3;
						break;
					case 43:
						break;
					case 44:
						break;
					case 45:
						break;
				}
				break;
			case AlliedCoalition::SOVIET:
				switch ( _year )
				{
					case 39:
						if ( isGerman( ) )
							fd *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fd *= 2;
						break;
					case 40:
						if ( isGerman( ) )
							fd *= 2.75;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fd *= 2;
						break;
					case 41:
						if ( isGerman( ) )
							fd *= 3;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fd *= 2;
						break;
					case 42:
						if ( isGerman( ) )
							fd *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fd *= 2;
						break;
					case 43:
						if ( isGerman( ) )
							fd *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fd *= 2;
						break;
					case 44:
						if ( isGerman( ) )
							fd *= 2.5;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fd *= 2;
						break;
					case 45:
						if ( isGerman( ) )
							fd *= 2;
						else if ( getSide( ) == SidePlayer::AXIS && getSubType( ) == GroundArmedForce::Axis::FIN )
							fd *= 1.5;
						break;
				}
				break;
			case AlliedCoalition::USA:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						break;
					case 41:
						break;
					case 42:
						if ( isGerman( ) )
							fd *= 1.5;
						break;
					case 43:
						if ( isGerman( ) )
							fd *= 1.3;
						break;
					case 44:
						if ( isGerman( ) )
							fd *= 1.2;
						break;
					case 45:
						if ( isGerman( ) )
							fd *= 1.1;
						break;
				}
				break;
			case AlliedCoalition::GREEKS:
				switch ( _year )
				{
					case 39:
						break;
					case 40:
						if ( isGerman( ) )
							fd *= 2;
						break;
					case 41:
						if ( isGerman( ) )
							fd *= 1.7;
						break;
					case 42:
						break;
					case 43:
						break;
					case 44:
						break;
					case 45:
						break;
				}
				break;
		}
	}
#endif
	// chk supply
	if ( getSupply( ) == SupplyStatus::NONE )
	{
#if 0
		if ( _rule_set.rules_variant_ == RulesVariant::A )
		{
			if ( getIsolated( ) )
			{
				if ( getSupplyTurns( ) >= 7 )  // U4 isolated or worse, defense quartered
					fd /= 4;
				else if ( getSupplyTurns( ) >= 3 )  // U2 isolated or worse, defense halved
					fd /= 2;
			}
			else
			{
				if ( getSupplyTurns( ) >= 5 )  // U3 or worse, defense halved
					fd /= 2;
			}
		}
		else
#endif
		if ( getSupplyTurns( ) >= 3 )  // U2 or worse, halved
			fd /= 2;
	}

	// chk disruption
	if ( getDisruption( ) == DisruptionStatus::DISRUPTED )
		fd /= 2;
	else if ( getDisruption( ) == DisruptionStatus::BADLY_DISRUPTED )
		fd /= 4;

	return fd;
}

const wxString hidden_partisans_no_edit =
	wxT(	"You can't edit a partisan unit while partisans are hidden!\n"
			"Unhide partisans via menu View | Show Allied/Axis Partisans..." );

bool GroundUnit::edit( )
{
	if ( getType( ) == GroundUnitType::WW2::PARTISAN )
	{
		if (	( getSide( ) == SidePlayer::AXIS && _rule_set.HideAxPartisans == TRUE ) ||
				( getSide( ) == SidePlayer::ALLIED && _rule_set.HideAlPartisans == TRUE )	)
		{
			wxMessageBox( hidden_partisans_no_edit, Application::NAME );
			return false;
		}
	}

	// thus dialog must be created *after* above test!
	static DGroundUnit* dlg;
	if ( ! dlg )
		dlg = new DGroundUnit( _parent_dialog_window );

	// setup dialog box
	int player = isAxis( ) ? SidePlayer::AXIS : SidePlayer::ALLIED;

	// static cache
	static char t[100];
	static char st[100];

	// if valid unit copy unit's stats to dialog.
	// NOTE: for newly created unit, size == NO_SIZE. in that case
	// leave dialog untouched (values from previously edited unit).

	if ( getSize( ) != GroundUnit::Size::INVALID_SIZE ) // edit exiting unit
	{
		// TODO:  DTM try this to set a new unit custom RE combobox to default size for
		//	unit's give echelon "getSize( )" above:
#if 0
		_unit_dialog_data.cmbRE = get_default_2re( getSize( ) );
#endif
		_unit_dialog_data.chkHasCadre = ( getFlag( GroundUnit::HAS_CADRE ) ? TRUE : FALSE );
		_unit_dialog_data.chkIsCadre = ( getFlag( GroundUnit::IS_CADRE ) ? TRUE : FALSE );
		_unit_dialog_data.chkMotorized = ( isSupplementalMotorized( ) ? TRUE : FALSE );
		_unit_dialog_data.chkRailOnly = ( getFlag( GroundUnit::RAILONLY ) ? TRUE : FALSE );
		_unit_dialog_data.chkWaterOnly = ( getFlag( GroundUnit::WATERONLY ) ? TRUE : FALSE );
		_unit_dialog_data.chkSupported = ( getFlag( GroundUnit::SUPPORTED ) ? TRUE : FALSE );
		_unit_dialog_data.chkUnsupported = ( getFlag( GroundUnit::UNSUPPORTED ) ? TRUE : FALSE );
		_unit_dialog_data.chkTwoREOfATEC = ( getFlag( GroundUnit::TWO_RE_ATEC ) ? TRUE : FALSE );
		_unit_dialog_data.chkThreeREOfATEC = ( getFlag( GroundUnit::THREE_RE_ATEC ) ? TRUE : FALSE );
		_unit_dialog_data.chkMountain = ( getFlag( GroundUnit::MOUNTAIN ) ? TRUE : FALSE );
		_unit_dialog_data.chkReserve = ( getFlag( GroundUnit::RESERVE ) ? TRUE : FALSE );
		_unit_dialog_data.chkTraining = ( getFlag( GroundUnit::TRAINING ) ? TRUE : FALSE );
		_unit_dialog_data.chkHasZOC = ( getFlag( GroundUnit::HAS_ZOC ) ? TRUE : FALSE );
		_unit_dialog_data.chkCadreUnsupported = ( getFlag( GroundUnit::CDR_UNSUP ) ? TRUE : FALSE );
		strcpy( _unit_dialog_data.editAtt, inttoa( getRawAtt( ) ) );
		strcpy( _unit_dialog_data.editDef, inttoa( getRawDef( ) ) );
		strcpy( _unit_dialog_data.editFlak, inttoa( getRawFlak( ) ) );
		strcpy( _unit_dialog_data.editMP, inttoa( maximum_MPs_ ) );
		strcpy( _unit_dialog_data.editMPLeft, inttoa( getMP( ) ) );
		strcpy( _unit_dialog_data.editID, id_ );
		strcpy( _unit_dialog_data.editCdrAtt, inttoa( getCdrAtt( ) ) );
		strcpy( _unit_dialog_data.editCdrDef, inttoa( getCdrDef( ) ) );
		strcpy( _unit_dialog_data.editCdrFlak, inttoa( getCdrFlak( ) ) );
		strcpy( _unit_dialog_data.editCdrMP, inttoa( getCdrMaxMP( ) ) );
		strcpy( _unit_dialog_data.zocstr, getZOCString( ) );

		line2spc( _unit_dialog_data.editID );
#if 0
		_unit_dialog_data.NoSize	= FALSE;
		_unit_dialog_data.rdHQ		= FALSE;
		_unit_dialog_data.rdCorpsHQ	= FALSE;
		_unit_dialog_data.rdArmyHQ	= FALSE;
		_unit_dialog_data.Company	= FALSE;
		_unit_dialog_data.Batallion	= FALSE;
		_unit_dialog_data.Regiment	= FALSE;
		_unit_dialog_data.Brigade	= FALSE;
		_unit_dialog_data.Divisional= FALSE;
		_unit_dialog_data.Division	= FALSE;
		_unit_dialog_data.Corps		= FALSE;
		switch ( GetSize( ) )
		{
			case NO_SIZE:   _unit_dialog_data.NoSize	= TRUE; break;
			case HQ:        _unit_dialog_data.rdHQ		= TRUE; break;
			case CORPSHQ:   _unit_dialog_data.rdCorpsHQ	= TRUE; break;
			case ARMYHQ:    _unit_dialog_data.rdArmyHQ	= TRUE; break;
			case COMPANY:   _unit_dialog_data.Company	= TRUE; break;
			case BATALLION: _unit_dialog_data.Batallion	= TRUE; break;
			case REGIMENT:  _unit_dialog_data.Regiment	= TRUE; break;
			case BRIGADE:   _unit_dialog_data.Brigade	= TRUE; break;
			case DIVISIONAL:_unit_dialog_data.Divisional= TRUE; break;
			case DIVISION:  _unit_dialog_data.Division	= TRUE; break;
			case CORPS:     _unit_dialog_data.Corps		= TRUE; break;
		}
#endif
		_unit_dialog_data.listSize = getSize( );

		strcpy( dlg->type_name_, _ground_unit_type[getType( )].name_.c_str( ) );
		strcpy( dlg->subtype_name_, _ground_armed_force[player][getSubType( )].longname_ );
		dlg->nationality_ = getSubType( );
		strcpy( t, _ground_unit_type[getType( )].name_.c_str( ) );
		strcpy( st, _ground_armed_force[player][getSubType( )].longname_ );
		// supply
		sprintf( _unit_dialog_data.editTurns, "%d", getSupplyTurns( ) );
		_unit_dialog_data.rdRegSupply = FALSE;
		_unit_dialog_data.rdSpcSupply = FALSE;
		_unit_dialog_data.rdNoSupply = FALSE;
		switch ( getSupply( ) )
		{
			case SupplyStatus::REG:
				_unit_dialog_data.rdRegSupply = TRUE;
				break;
			case SupplyStatus::SPC:
				_unit_dialog_data.rdSpcSupply = TRUE;
				break;
			case SupplyStatus::NONE:
				_unit_dialog_data.rdNoSupply = TRUE;
				break;
		}

		_unit_dialog_data.chkIsolated = getIsolated( ) ? TRUE : FALSE;

		if ( getType( ) == GroundUnitType::WW2::TRANSPORT )
			dlg->SetRE2( getCustomRE2( ) );
		else
			dlg->SetRE2( static_cast<int>( getRE( ) * 2.0F ) );

		_unit_dialog_data.Disrupted = getDisruption( ) == DisruptionStatus::DISRUPTED;
		_unit_dialog_data.BadDisrupted = getDisruption( ) == DisruptionStatus::BADLY_DISRUPTED;
	}
	else if ( t[0] != '\0' && st[0] != '\0' ) // creating new unit
	{
		// but not the first time, because the cached unit type
		//	and armed force strings will not have ever been set
		strcpy( dlg->type_name_, t );
		strcpy( dlg->subtype_name_, st );
	}

	// go! //
	if ( dlg->ShowModal( ) != wxID_OK )
	{
		_unit_editor_active = false;  // TODO: check this
		return false;
	}
	_unit_editor_active = false;  // TODO: check this, why not just fire this before the modal check?

	image_ok_ = false;  // may need to re-draw bitmap

	dlg->CloseDialog( );  // hack: transfer results from controls

	setType( dlg->GetType( ) );
	setSubType( dlg->GetSubType( ) );
	setAtt( atoi( _unit_dialog_data.editAtt ) );
	setDef( atoi( _unit_dialog_data.editDef ) );
	setFlak( atoi( _unit_dialog_data.editFlak ) );
	setMaxMP( atoi( _unit_dialog_data.editMP ) );
	setMP( atoi( _unit_dialog_data.editMPLeft ) );
	setID( _unit_dialog_data.editID );
	if ( strlen( id_ ) == 0 )
		strcpy( id_, " " );
	setCdrAtt( atoi( _unit_dialog_data.editCdrAtt ) );
	setCdrDef( atoi( _unit_dialog_data.editCdrDef ) );
	setCdrFlak( atoi( _unit_dialog_data.editCdrFlak ) );
	setCdrMaxMP( atoi( _unit_dialog_data.editCdrMP ) );
	setZOCByStr( _unit_dialog_data.zocstr );

	setSize( _unit_dialog_data.listSize );
#if 0
	if ( _unit_dialog_data.NoSize )
		SetSize( NO_SIZE );
	else if ( _unit_dialog_data.Batallion )
		SetSize( BATALLION );
	else if ( _unit_dialog_data.Brigade )
		SetSize( BRIGADE );
	else if ( _unit_dialog_data.Regiment )
		SetSize( REGIMENT );
	else if ( _unit_dialog_data.Divisional )
		SetSize( DIVISIONAL );
	else if ( _unit_dialog_data.Division )
		SetSize( DIVISION );
	else if ( _unit_dialog_data.Company )
		SetSize( COMPANY );
	else if ( _unit_dialog_data.Corps )
		SetSize( CORPS );
	else if ( _unit_dialog_data.rdHQ )
		SetSize( HQ );
	else if ( _unit_dialog_data.rdCorpsHQ )
		SetSize( CORPSHQ );
	else if ( _unit_dialog_data.rdArmyHQ )
		SetSize( ARMYHQ );
#endif
	// flags
	// TODO: should C/m class units really have the UF_MOTORIZED flag set?
	if ( _unit_dialog_data.chkMotorized ||
		 _ground_unit_type[getType( )].class_ == GroundUnitType::COMBAT_MOTORIZED_CLASS )
		setSupplementalMotorized( true );
	else
		setSupplementalMotorized( false );

	if ( getType( ) == GroundUnitType::WW2::TRUCK )  // truck: cadre means depleted
		setFlag( GroundUnit::HAS_CADRE );
	else if ( _unit_dialog_data.chkHasCadre )
#if 0
	if (	( GetSize() == DIVISION || GetSize() == CORPS ) &&
			( UnitDlgData.chkHasCadre || ( GetDef() > 4 && GetAtt() > 4 ) ) // str > 4 XXs always have cadre
		)
#endif
		setFlag( GroundUnit::HAS_CADRE );
	else
		clearFlag( GroundUnit::HAS_CADRE );

	if ( getFlag( GroundUnit::HAS_CADRE ) && _unit_dialog_data.chkIsCadre )
		setFlag( GroundUnit::IS_CADRE );
	else
		clearFlag( GroundUnit::IS_CADRE );

	if ( _unit_dialog_data.chkRailOnly )
		setFlag( GroundUnit::RAILONLY );
	else
		clearFlag( GroundUnit::RAILONLY );

	if ( _unit_dialog_data.chkWaterOnly )
		setFlag( GroundUnit::WATERONLY );
	else
		clearFlag( GroundUnit::WATERONLY );

	if ( _unit_dialog_data.chkSupported )
		setFlag( GroundUnit::SUPPORTED );
	else
		clearFlag( GroundUnit::SUPPORTED );

	if ( _unit_dialog_data.chkUnsupported )
		setFlag( GroundUnit::UNSUPPORTED );
	else
		clearFlag( GroundUnit::UNSUPPORTED );

	if ( _unit_dialog_data.chkTwoREOfATEC )
		setFlag( GroundUnit::TWO_RE_ATEC );
	else
		clearFlag( GroundUnit::TWO_RE_ATEC );

	if ( _unit_dialog_data.chkThreeREOfATEC )
		setFlag( GroundUnit::THREE_RE_ATEC );
	else
		clearFlag( GroundUnit::THREE_RE_ATEC );

	if ( _unit_dialog_data.chkMountain || getType( ) == GroundUnitType::WW2::HIGH_MOUNTAIN || getType( ) == GroundUnitType::WW2::MTN )
		setFlag( GroundUnit::MOUNTAIN );
	else
		clearFlag( GroundUnit::MOUNTAIN );

	if ( _unit_dialog_data.chkTraining || getType( ) == GroundUnitType::WW2::TRNG )
		setFlag( GroundUnit::TRAINING );
	else
		clearFlag( GroundUnit::TRAINING );

	if ( _unit_dialog_data.chkReserve || getType( ) == GroundUnitType::WW2::RES )
		setFlag( GroundUnit::RESERVE );
	else
		clearFlag( GroundUnit::RESERVE );

#if 0
	if ( UnitDlgData.chkHasZOC || GetSize() >= GroundUnit::Size::DIVISIONAL )
		SetFlag( GroundUnit::HAS_ZOC );
	else
		ClearFlag( GroundUnit::HAS_ZOC );
#endif

	if ( _unit_dialog_data.chkCadreUnsupported && getSize( ) >= GroundUnit::Size::DIVISIONAL )
		setFlag( GroundUnit::CDR_UNSUP );
	else
		clearFlag( GroundUnit::CDR_UNSUP );

	if ( _unit_dialog_data.Disrupted )
		setDisruption( DisruptionStatus::DISRUPTED );
	else if ( _unit_dialog_data.BadDisrupted )
		setDisruption( DisruptionStatus::BADLY_DISRUPTED );
	else
		setDisruption( DisruptionStatus::NOT_DISRUPTED );

	// supply
	setSupplyTurns( atoi( _unit_dialog_data.editTurns ) );
	if ( _unit_dialog_data.rdRegSupply )
		setSupply( SupplyStatus::REG );
	else if ( _unit_dialog_data.rdSpcSupply )
		setSupply( SupplyStatus::SPC );
	else if ( _unit_dialog_data.rdNoSupply )
		setSupply( SupplyStatus::NONE );
	else
		setSupply( SupplyStatus::REG ); /* no supply status selected */

	setIsolated( _unit_dialog_data.chkIsolated > 0 ? true : false );

	// remember type of this unit till next time
	strcpy( t, _ground_unit_type[getType( )].name_.c_str( ) );
	strcpy( st, _ground_armed_force[player][getSubType( )].longname_ );
	// HQ is special
	if ( getSize( ) == GroundUnit::Size::HQ || getSize( ) == GroundUnit::Size::CORPSHQ || getSize( ) == GroundUnit::Size::ARMYHQ )
	{
		if ( getAtt( ) > 0 || getDef( ) > 0 )
			wxMessageBox( wxT("Note: HQ has combat strength of zero"), Application::NAME, wxOK );
		setAtt( 0 );
		setDef( 0 );
	}

	// positional AA is special
	if ( getType( ) == GroundUnitType::WW2::POS_LT_AA || getType( ) == GroundUnitType::WW2::POS_HV_AA )
	{
		if ( getAtt( ) > 0 || getDef( ) > 0 )
			wxMessageBox( wxT("Note: positional AA has combat strength of zero"), Application::NAME, wxOK );
		setAtt( 0 );
		setDef( 0 );
	}

	// truck is special
	if ( getType( ) == GroundUnitType::WW2::TRUCK )
	{
		if ( getAtt( ) > 0 || getDef( ) > 0 || getFlak( ) > 0 )
			wxMessageBox( wxT("Note: truck has combat strength of zero"), Application::NAME, wxOK );
		setAtt( 0 );
		setDef( 0 );
		setFlak( 0 );
		setSize( GroundUnit::Size::DIVISION ); /* has cadre (cadre==depleted) */
	}

	// transport is special
	if ( getType( ) == GroundUnitType::WW2::TRANSPORT )
	{
		if ( getAtt( ) > 0 || getDef( ) > 0 || getFlak( ) > 0 )
			wxMessageBox( wxT("Note: transport has combat strength of zero"), Application::NAME, wxOK );

		setAtt( 0 );
		setDef( 0 );
		setFlak( 0 );

		if ( getSize( ) != GroundUnit::Size::NO_SIZE )
			wxMessageBox( wxT("Note: transport has zero size"), Application::NAME, wxOK );

		if ( dlg->GetRE2( ) == 0 )
		{
			wxMessageBox( wxT("Note: no capacity selected, using 1 RE"), Application::NAME, wxOK );
			setRE( 1.0F );
		}
		else if ( dlg->GetRE2( ) == 2 )
			setRE( 1.0F );
		else if ( dlg->GetRE2( ) == 4 )
			setRE( 2.0F );
		else if ( dlg->GetRE2( ) == 6 )
			setRE( 3.0F );
		else
		{
			wxMessageBox( wxT("Allowed capacities are 1, 2 or 3 REs"), Application::NAME, wxOK );
			setRE( 1.0F );
		}
		setSize( GroundUnit::Size::NO_SIZE );
	}

	// fortified are is special
	if ( getType( ) == GroundUnitType::WW2::FORTAREA )
	{
		if ( ! getFlag( GroundUnit::SUPPORTED ) )
		{
			wxMessageBox( wxT("Note: fortified area is always self supported"), Application::NAME, wxOK );
			setFlag( GroundUnit::SUPPORTED );
		}
		if ( getRE( ) )
		{
			wxMessageBox( wxT("Note: fortified area has no size"), Application::NAME, wxOK );
			setRE( 0.0F );
		}
	}

	// set RE size (only used if appropriate rule is selected)
	if ( getType( ) != GroundUnitType::WW2::TRANSPORT )
		setRE( ( static_cast<float>( dlg->GetRE2( ) ) ) / 2.0F );

	return true;
}

void GroundUnit::setID( char* str )
{
	strncpy( id_, str, GroundUnit::MAX_ID_LEN );
	if ( strlen( str ) >= GroundUnit::MAX_ID_LEN )
		id_[GroundUnit::MAX_ID_LEN - 1] = '\0';
	spc2line( id_ );
}

void GroundUnit::setMission( int i )
{
	data4_ &= 0x3f;
	i &= 0x03;
	data4_ |= (unsigned char)( i << 6 );
}

float GroundUnit::getDefaultRE( void )
{
	switch ( getSize( ) )
	{
#if 0
		 case NO_SIZE   : return  0;
		 case ARMYHQ    :
		 case CORPSHQ   :
		 case HQ        : return ((float)(RuleSet.HQRE2))/2;
		 case COMPANY   : return ((float)(RuleSet.CompanyRE2))/2;
		 case BATT_GROUP:
		 case BATALLION : return ((float)(RuleSet.BattalionRE2))/2;
		 case REG_GROUP :
		 case REGIMENT  : return ((float)(RuleSet.RegimentRE2))/2;
		 case BRIG_GROUP:
		 case BRIGADE   : return ((float)(RuleSet.BrigadeRE2))/2;
		 case DIVISIONAL: return ((float)(RuleSet.DivGrpRE2))/2;
		 case DIVISION  : return ((float)(RuleSet.DivisionRE2))/2;
		 case CORPS     : return ((float)(RuleSet.CorpsRE2))/2;
		 case ARMY      : return ((float)(RuleSet.ArmyRE2))/2;
#endif
		case GroundUnit::Size::NO_SIZE:
			return 0.0F;
		case GroundUnit::Size::ARMYHQ:
		case GroundUnit::Size::CORPSHQ:
		case GroundUnit::Size::HQ:
			return ( (float) ( _rule_set.HQRE2 ) ) / 2;
		case GroundUnit::Size::COMPANY:
			return ( (float) ( _rule_set.CompanyRE2 ) ) / 2;
		case GroundUnit::Size::BATT_GROUP:
		case GroundUnit::Size::BATALLION:
			return ( (float) ( _rule_set.BattalionRE2 ) ) / 2;  // 1/2 RE
		case GroundUnit::Size::REG_GROUP:
		case GroundUnit::Size::REGIMENT:
			return ( (float) ( _rule_set.RegimentRE2 ) ) / 2;
		case GroundUnit::Size::BRIG_GROUP:
		case GroundUnit::Size::BRIGADE:
			return ( (float) ( _rule_set.BrigadeRE2 ) ) / 2;
		case GroundUnit::Size::DIVISIONAL:
			return ( (float) ( _rule_set.DivGrpRE2 ) ) / 2;
		case GroundUnit::Size::DIVISION:
			return ( (float) ( _rule_set.DivisionRE2 ) ) / 2;
		case GroundUnit::Size::CORPS:
			return ( (float) ( _rule_set.CorpsRE2 ) ) / 2;
		case GroundUnit::Size::ARMY:
			return ( (float) ( _rule_set.ArmyRE2 ) ) / 2;
	}
	return 0.0F;
}

int GroundUnit::getCustomRE2( )
{
	int re2 = ( ( data6_ >> 2 ) & 0xf );
	if ( getFlag( GroundUnit::RE_HBIT ) )
		re2 |= 0x10;
	return re2;
}

float GroundUnit::getRE( void )
{
	if ( getType( ) == GroundUnitType::WW2::PARTISAN )
	{
		if ( getSide( ) == SidePlayer::AXIS && _rule_set.HideAxPartisans == TRUE )
			return 0.0F;
		if ( getSide( ) == SidePlayer::ALLIED && _rule_set.HideAlPartisans == TRUE )
			return 0.0F;
	}
	// positional AA is special
	if ( getType( ) == GroundUnitType::WW2::POS_LT_AA || getType( ) == GroundUnitType::WW2::POS_HV_AA )
		return ( ( (float)getFlak( ) ) / 2 );

	if ( _rule_set.AllowCustomREs == TRUE )
		return ( (float)getCustomRE2( ) ) / 2;
	else
	{
		// small unit cadre size is always 50% of normal REs
		if ( getSize( ) < GroundUnit::Size::DIVISIONAL && isCadre( ) )
			return getDefaultRE( ) / 2;
		// normal (divisional) cadre size is customizable
		else if ( getSize( ) >= GroundUnit::Size::DIVISIONAL && isCadre( ) )
			return ( (float) ( _rule_set.CadreRE2 ) ) / 2;
		else
			return getDefaultRE( );
	}
}

// data6, bits 2..5
void GroundUnit::setRE( float f )
{
	int re = (int)( f * 2 );

	if ( re < 0 )
		re = 0;
	else if ( re > 31 )
		re = 31;

	// highest bit stored elsewhere
	if ( re > 15 )
	{
		setFlag( GroundUnit::RE_HBIT );
		re -= 16;
	}
	else
		clearFlag( GroundUnit::RE_HBIT );

	data6_ &= 0xc3;  // 1100 0011
	data6_ |= (unsigned char) ( re << 2);
}

static int zocflags( int u0, int u2 )
{
	int f = 0;
	if ( u0 == ZOC::REDUCED )
		f |= GroundUnit::HAS_RZOC;
	else if ( u0 == ZOC::NORMAL )
		f |= GroundUnit::HAS_ZOC;
	if ( u2 == ZOC::REDUCED )
		f |= GroundUnit::HAS_U2RZOC;
	else if ( u2 == ZOC::NORMAL )
		f |= GroundUnit::HAS_U2ZOC;
	return f;
}

// sets ZOC to default set by rules
void GroundUnit::setDefaultZOC( void )
{
	clearFlag( GroundUnit::HAS_ZOC );
	clearFlag( GroundUnit::HAS_RZOC );
	clearFlag( GroundUnit::HAS_CZOC );
	clearFlag( GroundUnit::HAS_CRZOC );
	clearFlag( GroundUnit::HAS_U2ZOC );
	clearFlag( GroundUnit::HAS_U2RZOC );
	clearFlag( GroundUnit::HAS_U2CZOC );
	clearFlag( GroundUnit::HAS_U2CRZOC );

	switch ( getSize( ) )
	{
		case GroundUnit::Size::NO_SIZE:
		case GroundUnit::Size::ARMYHQ:
		case GroundUnit::Size::CORPSHQ:
		case GroundUnit::Size::HQ:
		case GroundUnit::Size::COMPANY:
			return;  // these never have ZOC
		case GroundUnit::Size::BATT_GROUP:
		case GroundUnit::Size::BATALLION:
			setFlag( zocflags( _rule_set.BattalionZOC, _rule_set.BattalionU2ZOC ) );
			return;
		case GroundUnit::Size::REG_GROUP:
		case GroundUnit::Size::REGIMENT:
			setFlag( zocflags( _rule_set.RegimentZOC, _rule_set.RegimentU2ZOC ) );
			return;
		case GroundUnit::Size::BRIG_GROUP:
		case GroundUnit::Size::BRIGADE:
			setFlag( zocflags( _rule_set.BrigadeZOC, _rule_set.BrigadeU2ZOC ) );
			return;
		case GroundUnit::Size::DIVISIONAL:
			setFlag( zocflags( _rule_set.DivGrpZOC, _rule_set.DivGrpU2ZOC ) );
			setFlag( zocflags( _rule_set.CadreZOC, _rule_set.CadreU2ZOC ) );
			return;
		case GroundUnit::Size::DIVISION:
			setFlag( zocflags( _rule_set.DivisionZOC, _rule_set.DivisionU2ZOC ) );
			setFlag( zocflags( _rule_set.CadreZOC, _rule_set.CadreU2ZOC ) );
			return;
		case GroundUnit::Size::CORPS:
		case GroundUnit::Size::ARMY:
			setFlag( zocflags( _rule_set.CorpsZOC, _rule_set.CorpsU2ZOC ) );
			setFlag( zocflags( _rule_set.CadreZOC, _rule_set.CadreU2ZOC ) );
			return;
	}
}

// i.e. "F-/R-"
char* GroundUnit::getZOCString( )
{
	static char bf[10];

	strcpy( bf, "" );  // dwFlags
	if ( _rule_set.AllowCustomZOCs == TRUE )
	{
		if ( getFlag( GroundUnit::HAS_ZOC ) )
			strcat( bf, "F" );
		else if ( getFlag( GroundUnit::HAS_RZOC ) )
			strcat( bf, "R" );
		else
			strcat( bf, "-" );

		if ( getFlag( GroundUnit::HAS_CZOC ) )
			strcat( bf, "F" );
		else if ( getFlag( GroundUnit::HAS_CRZOC ) )
			strcat( bf, "R" );
		else
			strcat( bf, "-" );
		strcat( bf, "/" );

		if ( getFlag( GroundUnit::HAS_U2ZOC ) )
			strcat( bf, "F" );
		else if ( getFlag( GroundUnit::HAS_U2RZOC ) )
			strcat( bf, "R" );
		else
			strcat( bf, "-" );

		if ( getFlag( GroundUnit::HAS_U2CZOC ) )
			strcat( bf, "F" );
		else if ( getFlag( GroundUnit::HAS_U2CRZOC ) )
			strcat( bf, "R" );
		else
			strcat( bf, "-" );
	}
	else
	{
		switch ( getSize( ) )
		{
			case GroundUnit::Size::REG_GROUP:
			case GroundUnit::Size::REGIMENT:
				if ( _rule_set.RegimentZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.RegimentZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreZOC == ZOC::REDUCED ? "R" : "-" );

				strcat( bf, "/" );

				if ( _rule_set.RegimentU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.RegimentU2ZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreU2ZOC == ZOC::REDUCED ? "R" : "-" );
				break;
			case GroundUnit::Size::BRIG_GROUP:
			case GroundUnit::Size::BRIGADE:
				if ( _rule_set.BrigadeZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.BrigadeZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreZOC == ZOC::REDUCED ? "R" : "-" );

				strcat( bf, "/" );

				if ( _rule_set.BrigadeU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.BrigadeU2ZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreU2ZOC == ZOC::REDUCED ? "R" : "-" );
				break;
			case GroundUnit::Size::DIVISIONAL:
				if ( _rule_set.DivGrpZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.DivGrpZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreZOC == ZOC::REDUCED ? "R" : "-" );

				strcat( bf, "/" );

				if ( _rule_set.DivGrpU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.DivGrpU2ZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreU2ZOC == ZOC::REDUCED ? "R" : "-" );
				break;
			case GroundUnit::Size::DIVISION:
				if ( _rule_set.DivisionZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.DivisionZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreZOC == ZOC::REDUCED ? "R" : "-" );

				strcat( bf, "/" );

				if ( _rule_set.DivisionU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.DivisionU2ZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreU2ZOC == ZOC::REDUCED ? "R" : "-" );
				break;
			case GroundUnit::Size::ARMY:
			case GroundUnit::Size::CORPS:
				if ( _rule_set.CorpsZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.CorpsZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreZOC == ZOC::REDUCED ? "R" : "-" );

				strcat( bf, "/" );

				if ( _rule_set.CorpsU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );
				else if ( _rule_set.CorpsU2ZOC == ZOC::REDUCED )
					strcat( bf, "R" );
				else
					strcat( bf, "-" );

				if ( _rule_set.CadreU2ZOC == ZOC::NORMAL )
					strcat( bf, "F" );

				strcat( bf, _rule_set.CadreU2ZOC == ZOC::REDUCED ? "R" : "-" );
				break;
		}
	}
	return bf;
}

void GroundUnit::setZOCByStr( char* zocstr )
{
	clearFlag( GroundUnit::HAS_ZOC );
	clearFlag( GroundUnit::HAS_RZOC );
	clearFlag( GroundUnit::HAS_CZOC );
	clearFlag( GroundUnit::HAS_CRZOC );
	clearFlag( GroundUnit::HAS_U2ZOC );
	clearFlag( GroundUnit::HAS_U2RZOC );
	clearFlag( GroundUnit::HAS_U2CZOC );
	clearFlag( GroundUnit::HAS_U2CRZOC );
	if ( ! zocstr || strlen( zocstr ) < 5 )  // dwFlags
		return;
	// full str:
	if ( zocstr[0] == 'F' )
		setFlag( GroundUnit::HAS_ZOC );
	else if ( zocstr[0] == 'R' )
		setFlag( GroundUnit::HAS_RZOC );
	// cadre:
	if ( zocstr[1] == 'F' )
		setFlag( GroundUnit::HAS_CZOC );
	else if ( zocstr[1] == 'R' )
		setFlag( GroundUnit::HAS_CRZOC );
	// full str:
	if ( zocstr[3] == 'F' )
		setFlag( GroundUnit::HAS_U2ZOC );
	else if ( zocstr[3] == 'R' )
		setFlag( GroundUnit::HAS_U2RZOC );
	// cadre:
	if ( zocstr[4] == 'F' )
		setFlag( GroundUnit::HAS_U2CZOC );
	else if ( zocstr[4] == 'R' )
		setFlag( GroundUnit::HAS_U2CRZOC );
}

bool GroundUnit::hasNormalZOC( )
{
	// disrupted: no ZOC
	if ( getDisruption( ) == DisruptionStatus::DISRUPTED || getDisruption( ) == DisruptionStatus::BADLY_DISRUPTED )
		return false;

	// if custom ZOCs are enabled (rules dialog):
	if ( _rule_set.AllowCustomZOCs == TRUE )
	{
		// U2 or worse
		if ( getSupply( ) == SupplyStatus::NONE && getSupplyTurns( ) >= 3 )
			return isCadre( ) ? getFlag( GroundUnit::HAS_U2CZOC ) : getFlag( GroundUnit::HAS_U2ZOC );
		else  // U1 or better
			// TODO: shouldn't this return here, instead of setting an int r
			//	that isn't referenced hereafter?
			return isCadre( ) ? getFlag( GroundUnit::HAS_CZOC ) : getFlag( GroundUnit::HAS_ZOC );
	}

	// TODO: shouldn't we return true when these rule settings
	//	are ZOC::REDUCED, or greater?
	// standard ZOCs, get them from rule settings:
	// U2 or worse:
	if ( getSupply( ) == SupplyStatus::NONE && getSupplyTurns( ) >= 3 )
	{
		if ( isCadre( ) )
			return ( _rule_set.CadreU2ZOC == ZOC::NORMAL );
		switch ( getSize( ) )
		{
			case GroundUnit::Size::REG_GROUP:
			case GroundUnit::Size::REGIMENT:
				return ( _rule_set.RegimentU2ZOC == ZOC::NORMAL );
			case GroundUnit::Size::BRIG_GROUP:
			case GroundUnit::Size::BRIGADE:
				return ( _rule_set.BrigadeU2ZOC == ZOC::NORMAL );
			case GroundUnit::Size::DIVISIONAL:
				return ( _rule_set.DivGrpU2ZOC == ZOC::NORMAL );
			case GroundUnit::Size::DIVISION:
				return ( _rule_set.DivisionU2ZOC == ZOC::NORMAL );
			case GroundUnit::Size::ARMY:  // TODO: own setting
			case GroundUnit::Size::CORPS:
				return ( _rule_set.CorpsU2ZOC == ZOC::NORMAL );
			default:
				return false;
		}
	}

	// TODO: shouldn't we return true when these rule settings
	//	are ZOC::REDUCED, or greater?
	// in supply or U1
	if ( isCadre( ) )
		return ( _rule_set.CadreZOC == ZOC::NORMAL );
	switch ( getSize( ) )
	{
		case GroundUnit::Size::REG_GROUP:
		case GroundUnit::Size::REGIMENT:
			return ( _rule_set.RegimentZOC == ZOC::NORMAL );
		case GroundUnit::Size::BRIG_GROUP:
		case GroundUnit::Size::BRIGADE:
			return ( _rule_set.BrigadeZOC == ZOC::NORMAL );
		case GroundUnit::Size::DIVISIONAL:
			return ( _rule_set.DivGrpZOC == ZOC::NORMAL );
		case GroundUnit::Size::DIVISION:
			return ( _rule_set.DivisionZOC == ZOC::NORMAL );
		case GroundUnit::Size::ARMY:  // TODO: own setting
		case GroundUnit::Size::CORPS:
			return ( _rule_set.CorpsZOC == ZOC::NORMAL );
	}

	return false;
}

bool GroundUnit::hasReducedZOC( )
{
	// if custom ZOCs are enabled (rules dlg): (TODO) dwFlags
	if ( _rule_set.AllowCustomZOCs == TRUE )
	{
		// U2 or worse:
		if ( getSupply( ) == SupplyStatus::NONE && getSupplyTurns( ) >= 3 )
			return getFlag( isCadre( ) ? GroundUnit::HAS_U2CRZOC : GroundUnit::HAS_U2RZOC );
		// U1 or better
		else
			return getFlag( isCadre( ) ? GroundUnit::HAS_CRZOC : GroundUnit::HAS_RZOC );
	}

	// U2 or worse:
	if ( getSupply( ) == SupplyStatus::NONE && getSupplyTurns( ) >= 3 )
	{
		if ( isCadre( ) )
			return ( _rule_set.CadreU2ZOC == ZOC::REDUCED );
		switch ( getSize( ) )
		{
			case GroundUnit::Size::REGIMENT:
				return ( _rule_set.RegimentU2ZOC == ZOC::REDUCED );
			case GroundUnit::Size::BRIGADE:
				return ( _rule_set.BrigadeU2ZOC == ZOC::REDUCED );
			case GroundUnit::Size::DIVISIONAL:
				return ( _rule_set.DivGrpU2ZOC == ZOC::REDUCED );
			case GroundUnit::Size::DIVISION:
				return ( _rule_set.DivisionU2ZOC == ZOC::REDUCED );
			case GroundUnit::Size::CORPS:
				return ( _rule_set.CorpsU2ZOC == ZOC::REDUCED );
			default:
				return false;
		}
	}

	// in supply or U1
	if ( isCadre( ) )
		return ( _rule_set.CadreZOC == ZOC::REDUCED );
	switch ( getSize( ) )
	{
		case GroundUnit::Size::REGIMENT:
			return ( _rule_set.RegimentZOC == ZOC::REDUCED );
		case GroundUnit::Size::BRIGADE:
			return ( _rule_set.BrigadeZOC == ZOC::REDUCED );
		case GroundUnit::Size::DIVISIONAL:
			return ( _rule_set.DivGrpZOC == ZOC::REDUCED );
		case GroundUnit::Size::DIVISION:
			return ( _rule_set.DivisionZOC == ZOC::REDUCED );
		case GroundUnit::Size::CORPS:
			return ( _rule_set.CorpsZOC == ZOC::REDUCED );
	}
#if 0
	// cadres don't have ZOC's
	if ( IsCadre( ) )
		return false;
	// U2 or worse units have reduced ZOC's IF they normally have ZOC's
	if ( GetSupply( ) == SupplyStatus::NONE && GetSupplyTurns( ) >= 3 )
	{
		// all full strength divisional units have cadre
		if ( GetSize( ) >= GroundUnit::Size::DIVISIONAL )
			return true;

		// smaller units may have cadre (user changeable property)
		return GetFlag( UF_HAS_ZOC );
	}
	// in supply or U1, doesn't have reduced ZOC
#endif
	return false;
}

int GroundUnit::getMaxMP( void )
{
	return isCadre( ) ? getCdrMaxMP( ) : maximum_MPs_;
}

int GroundUnit::getFlak( void )
{
	return isCadre( ) ? getCdrFlak( ) : getRawFlak( );
}

float GroundUnit::getRealFlak( void )
{
	if ( getOverstack( ) )
		return 0;

	float fd = getFlak( );  // cadre/full

	// chk supply
	if ( getSupply( ) == SupplyStatus::NONE )
	{
		if ( getSupplyTurns( ) >= 3 )  // U2 or worse, halved
			fd /= 2;
	}
	return fd;
}

int GroundUnit::getAECA( void )
{
	// U2 (or worse) have no AECA
	if ( getSupply( ) == SupplyStatus::NONE && getSupplyTurns( ) > 2 )
		return ArmorAntitank::Proportion::NONE;

	// Soviet Mech XXX is Full AECA
	if ( getType( ) == GroundUnitType::WW2::MECH && isSoviet( ) && getSize( ) == GroundUnit::Size::CORPS )
		return ArmorAntitank::Proportion::FULL;

	// FWtBT: Loyalist Lt Arm units have 1/2 AECA
	if ( getType( ) == GroundUnitType::WW2::LT_ARM && isLoyalist( ) )
		return ArmorAntitank::Proportion::HALF;

	// all Mot units are Neutral, e.g. Mot Inf
	if ( _ground_unit_type[getType( )].AECA_value_.value( ) == ArmorEffectsValue::AECNONE )
		return isSupplementalMotorized( ) ? ArmorEffectsValue::NEUT : ArmorEffectsValue::AECNONE;

	return _ground_unit_type[getType( )].AECA_value_.value( );
}

int GroundUnit::getAECD( void )
{
	if ( getSupply( ) == SupplyStatus::NONE && getSupplyTurns( ) > 2 )
		return ArmorAntitank::Proportion::NONE;

	// soviet Mech XXX has full AECD (Mech is 1/2 AECD by default)
	if ( getType( ) == GroundUnitType::WW2::MECH && isSoviet( ) && getSize( ) == GroundUnit::Size::CORPS )
		return ArmorAntitank::Proportion::FULL;

	// British Lt Arm / Recon has full AECD (it's 1/2 AECD by default)
	if ( isAllied( ) && getSubType( ) == GroundArmedForce::Allied::BRITISH && ( getType( ) == GroundUnitType::WW2::LT_ARM || getType( ) == GroundUnitType::WW2::RECON ) )
		return ArmorAntitank::Proportion::FULL;

	// Axis non-German armored units have only ATEC (Finns have full!)
	if ( getType( ) == GroundUnitType::WW2::ARM && isAxis( ) && ! isGerman( ) && getSubType( ) != GroundArmedForce::Axis::FIN )
		return ArmorAntitank::Proportion::HALF;

	// FWtBT: Loyalist armor units have 1/2 AECD
	if ( getType( ) == GroundUnitType::WW2::ARM && isLoyalist( ) )
		return ArmorAntitank::Proportion::HALF;

	if ( _ground_unit_type[getType( )].AECD_value_.value( ) == ArmorEffectsValue::AECNONE )
		return isSupplementalMotorized( ) ? ArmorEffectsValue::NEUT : ArmorEffectsValue::AECNONE;

	return _ground_unit_type[getType( )].AECD_value_.value( );
}

int GroundUnit::getATEC( void )
{
	if ( getSupply( ) == SupplyStatus::NONE && getSupplyTurns( ) > 2 )
		return ArmorAntitank::Proportion::NONE;

	// Soviet Lt Arm / Recon ATEC
	if ( isSoviet( ) && ( getType( ) == GroundUnitType::WW2::LT_ARM || getType( ) == GroundUnitType::WW2::RECON ) )
		return ArmorAntitank::Proportion::HALF;

	// Axis non-German armored units have only ATEC (but Finnish have full!)
	if ( getType( ) == GroundUnitType::WW2::ARM && isAxis( ) && ! isGerman( ) && getSubType( ) != GroundArmedForce::Axis::FIN )
		return ArmorAntitank::Proportion::HALF;

	// British Lt Arm / Recon has full ATEC
	if ( isAllied( ) && getSubType( ) == GroundArmedForce::Allied::BRITISH && ( getType( ) == GroundUnitType::WW2::LT_ARM || getType( ) == GroundUnitType::WW2::RECON ) )
		return ArmorAntitank::Proportion::FULL;

	// self propelled guns have ATEC before Jul I 42, full after that
	if ( getType( ) == GroundUnitType::WW2::ASLT_GUN && ( _year < 42 || _month < 6 ) )
		return ArmorAntitank::Proportion::HALF;

	// axis AT units have ATEC before Apr I 42, full after that (Rule!)
	if ( _rule_set.AxisHalfAT == TRUE && ( getType( ) == GroundUnitType::WW2::AT && isAxis( ) && ( _year < 42 || _month < 4 ) ) )
		return ArmorAntitank::Proportion::HALF;

	// FWtBT: Loyalist mech units have 1/2 ATEC
	if ( getType( ) == GroundUnitType::WW2::MECH && isLoyalist( ) )
		return ArmorAntitank::Proportion::HALF;

	if ( _ground_unit_type[getType( )].ATEC_value_.value( ) == ArmorEffectsValue::AECNONE )
	{
		if ( isSupplementalMotorized( ) )
			return ArmorAntitank::Proportion::NEUT;  // all Mot units are Neutral
		else  // all German units in Jul 44 and later are neutral
		{
			if ( isGerman( ) && ( _year >= 44 && _month >= 6 ) )  // 0=jan, 6=jul
				return ArmorAntitank::Proportion::NEUT;
			else
				return ArmorAntitank::Proportion::NONE;
#if 0
			// 0=jan, 6=jul
			return ( isGerman( ) && ( Year >= 44 && Month >= 6 ) ) ? AEC_NEUT : AEC_NONE;
#endif
		}
	}
	return _ground_unit_type[getType( )].ATEC_value_.value( );
}

// return no.of hexes/MP in tact. rail movement
int GroundUnit::getRailHexes( void )
{
	if ( getFlag( GroundUnit::RAILONLY ) )
		return 40;

	if ( _rule_set.ToTrain1MP == TRUE ) // old system, 1MP to board, higher allowances
	{
		switch ( getMaxMP( ) )
		{
			case 10:
			case 9:
				return 5;
			case 8:
				return 6;
			case 7:
				return 6;
			case 6:
				return 8;
			case 5:
				return 10;
			case 4:
				return 10;
			case 3:
				return 10;
			case 2:
				return 30;
			case 1:  // not defined actually
				return 40;
			default:  // 0 MP or UFO
				return 0;
		}
	}
	else
	{  // Collector Series system (no MPs to board -> lower allowance)
		switch ( getMaxMP( ) )
		{
			case 10:
			case 9:
				return 4;
			case 8:
				return 5;
			case 7:
				return 6;
			case 6:
				return 7;
			case 5:
				return 8;
			case 4:
				return 10;
			case 3:  // not defined actually
				return 13;
			case 2:
				return 20;
			case 1:  // not defined actually
				return 40;
			default:  // 0 MP or ufo
				return 0;
		}
	}
}

// set hexes remaining in operative rail move for this MP
void GroundUnit::setRailRem( int i )
{
	data4_ &= 0xc0;
	i &= 0x3f;
	data4_ |= static_cast<unsigned char>( i );
}
int GroundUnit::getRailRem( void )
{
	return ( data4_ & 0x3f );
}

// unit in train (data3 bit 6)
void GroundUnit::setTrain( bool i )
{
	if ( i )
		data3_ |= 0x40;
	else
		data3_ &= 0xbf;
}
bool GroundUnit::getTrain( void )
{
	return ( data3_ & 0x40 );
}

// has used administrative movement?
void GroundUnit::setAdminMove( bool i )
{
	if ( i )
		data3_ |= 0x10;
	else
		data3_ &= 0xef;
}
bool GroundUnit::getAdminMove( void )
{
	return ( data3_ & 0x10 );
}

// has used tactical movement?
void GroundUnit::setTacticalMove( bool i )
{
	if ( i )
		data3_ |= 0x20;
	else
		data3_ &= 0xdf;
}
bool GroundUnit::getTacticalMove( void )
{
	return ( data3_ & 0x20 );
}

// has used strategic movement?
void GroundUnit::setStratMove( bool i )
{
	if ( i )
		data3_ |= 0x80;
	else
		data3_ &= 0x7f;
}
bool GroundUnit::getStratMove( void )
{
	return ( data3_ & 0x80 );
}

// has heavy equipment?
bool GroundUnit::hasHE( )
{
	// TODO: HQ deprecated in Series rules
	if ( isSupplementalMotorized( ) || getSize( ) == GroundUnit::Size::HQ || getSize( ) == GroundUnit::Size::ARMYHQ || getSize( ) == GroundUnit::Size::CORPSHQ )
		return true;

	if ( _ground_unit_type[getType( )].flags_ & GroundUnitType::HAS_HE )
		return true;

	return false;
}

// is mountain unit?
bool GroundUnit::isMtn( )
{
	if ( getType( ) == GroundUnitType::WW2::MTN || getType( ) == GroundUnitType::WW2::HIGH_MOUNTAIN )  // obvious ones
		return true;
	else if ( getType( ) == GroundUnitType::WW2::SKI )  // ski is mountain unit too (did you know that?)
		return true;
	else
		return getFlag( GroundUnit::MOUNTAIN );  // other can be flag'ed
}

bool GroundUnit::isCombatUnit( )
{
	switch ( getType( ) )
	{
		case GroundUnitType::WW2::POS_HV_AA:
		case GroundUnitType::WW2::POS_LT_AA:
		case GroundUnitType::WW2::PORT_FORT:
		case GroundUnitType::WW2::FORTAREA:
		case GroundUnitType::WW2::TRUCK:
			return false;
	}
	return true;
}

// Build strings like "5-7-6 Infantry XX  (122)" to "str"
void GroundUnit::getUnitString( char* str, bool verbose /* = false */ )
{
	if ( getType( ) == GroundUnitType::WW2::PARTISAN )
	{
		if ( ( getSide( ) == SidePlayer::AXIS && _rule_set.HideAxPartisans == TRUE ) || ( getSide( ) == SidePlayer::ALLIED && _rule_set.HideAlPartisans == TRUE ) )
		{
			strcpy( str, _ground_armed_force[getSide( )][getSubType( )].longname_ );
			if ( strstr( str, "artisan" ) == 0 )
				strcat( str, " partisan" );
			strcat( str, " (hidden)" );
			return;
		}
	}

	// pos AA is different!
	if ( getType( ) == GroundUnitType::WW2::POS_LT_AA )
	{
		sprintf( str, "%d pts Light Positional AA", getFlak( ) );
		return;
	}
	else if ( getType( ) == GroundUnitType::WW2::POS_HV_AA )
	{
		sprintf( str, "%d pts Heavy Positional AA", getFlak( ) );
		return;
	}
	else if ( getType( ) == GroundUnitType::WW2::TRUCK )
	{
		sprintf( str, getFlag( GroundUnit::IS_CADRE ) ? "Depleted Truck" : "Undepleted Truck" );
		return;
	}

	int side = ( isAxis( ) ? SidePlayer::AXIS : SidePlayer::ALLIED );
	strcpy( str, "" );

	strcat( str, _ground_armed_force[side][getSubType( )].longname_ );
	strcat( str, " " );
	if ( strlen( getID( ) ) > 0 )
	{
		strcat( str, getID( ) );
		line2spc( str );
		strcat( str, atoi( getID( ) ) > 0 ? ". " : " " );
	}

	if ( isSupplementalMotorized( ) && ! isCombatMotClass( ) )
		strcat( str, "Motorized " );

	if ( getFlag( GroundUnit::RESERVE ) && getType( ) != GroundUnitType::WW2::RES )
		strcat( str, "Reserve " );
	if ( getFlag( GroundUnit::TRAINING ) && getType( ) != GroundUnitType::WW2::TRNG )
		strcat( str, "Training " );
	if ( getFlag( GroundUnit::MOUNTAIN ) && getType( ) != GroundUnitType::WW2::MTN && getType( ) != GroundUnitType::WW2::HIGH_MOUNTAIN )
		strcat( str, "Mountain " );

	// unit name, modify it per nationality:
	if ( getType( ) == GroundUnitType::WW2::ARM && isGerman( ) )
		strcat( str, "Panzer" );
	else if ( getType( ) == GroundUnitType::WW2::FL_TANK && isGerman( ) )
		strcat( str, "Flammpanzer" );
	else if ( getType( ) == GroundUnitType::WW2::ARM && isSoviet( ) )
		strcat( str, "Tank" );
	else if ( getType( ) == GroundUnitType::WW2::MECH && isGerman( ) )
		strcat( str, "Panzergrenadier" );
	else if ( getType( ) == GroundUnitType::WW2::PARA_INF && getSubType( ) == GroundArmedForce::Allied::USA )
		strcat( str, "Airborne" );
	else if ( getType( ) == GroundUnitType::WW2::MTN && getSubType( ) == GroundArmedForce::Axis::ITA )
		strcat( str, "Alpine" );
	else if ( getType( ) == GroundUnitType::WW2::LT_INF && isGerman( ) )
		strcat( str, "Jager" );
	else
		strcat( str, _ground_unit_type[getType( )].name_.c_str( ) );

	strcat( str, " " );
	switch ( getSize( ) )
	{
		case GroundUnit::Size::NO_SIZE:
			break;
		case GroundUnit::Size::HQ:
			strcat( str, "division headquarters" );
			break;
		case GroundUnit::Size::CORPSHQ:
			strcat( str, "corps headquarters" );
			break;
		case GroundUnit::Size::ARMYHQ:
			strcat( str, "army headquarters" );
			break;
		case GroundUnit::Size::COMPANY:
			strcat( str, isCadre( ) ? "cadre " : "company" );
			break;
		case GroundUnit::Size::BATALLION:
			strcat( str, isCadre( ) ? "cadre " : "battalion" );
			break;
		case GroundUnit::Size::BATT_GROUP:
			strcat( str, isCadre( ) ? "cadre " : "battalion grouping" );
			break;
		case GroundUnit::Size::REGIMENT:
			strcat( str, isCadre( ) ? "cadre " : "regiment" );
			break;
		case GroundUnit::Size::REG_GROUP:
			strcat( str, isCadre( ) ? "cadre " : "regimental grouping" );
			break;
		case GroundUnit::Size::BRIGADE:
			strcat( str, isCadre( ) ? "cadre " : "brigade" );
			break;
		case GroundUnit::Size::BRIG_GROUP:
			strcat( str, isCadre( ) ? "cadre " : "brigade grouping" );
			break;
		case GroundUnit::Size::DIVISIONAL: // TODO: why different pattern for these?
			strcat( str, getFlag( GroundUnit::IS_CADRE ) ? "cadre" : "divisional grouping" );
			break;
		case GroundUnit::Size::DIVISION:
			strcat( str, getFlag( GroundUnit::IS_CADRE ) ? "cadre" : "division" );
			break;
		case GroundUnit::Size::CORPS:
			strcat( str, getFlag( GroundUnit::IS_CADRE ) ? "cadre" : "corps" );
			break;
		case GroundUnit::Size::ARMY:
			strcat( str, getFlag( GroundUnit::IS_CADRE ) ? "cadre" : "army" );
			break;
	}

	// movement counter size:
	if ( getType( ) == GroundUnitType::WW2::TRANSPORT || getType( ) == GroundUnitType::WW2::APC || getType( ) == GroundUnitType::WW2::LVT )
	{
		switch ( getCustomRE2( ) )
		{
			case 6:
				strcat( str, ", 3 RE" );
				break;
			case 4:
				strcat( str, ", 2 RE" );
				break;
			case 2:
			default:
				strcat( str, ", 1 RE" );
				break;
		}
	}

	strcat( str, " " );

	char att[8];
	strcpy( att, inttoa( getAtt( ) ) );

	char def[8];
	strcpy( def, inttoa( getDef( ) ) );

	char unitmp[8];
	strcpy( unitmp, inttoa( getMaxMP( ) ) );

	char flak[8];
	strcpy( flak, inttoa( getFlak( ) ) );

	strcat( str, "  " );
	strcat( str, att );
	strcat( str, "-" );
	if ( getAtt( ) != getDef( ) )
	{
		strcat( str, def );
		strcat( str, "-" );
	}
	strcat( str, getFlag( GroundUnit::RAILONLY ) ? "R" : unitmp );

	if (	getFlak( ) > 0
			|| getType( ) == GroundUnitType::WW2::LT_AA
			|| getType( ) == GroundUnitType::WW2::HV_AA
			|| getType( ) == GroundUnitType::WW2::POS_HV_AA
			|| getType( ) == GroundUnitType::WW2::POS_LT_AA )
	{
		strcat( str, " AA=" );
		strcat( str, flak );
	}

	// support status (not for artillery):
	if ( ! isArtillery( ) )
	{
		if ( ( ! isCadre( ) && getFlag( GroundUnit::SUPPORTED ) )
			|| ( getSize( ) >= GroundUnit::Size::DIVISIONAL && isCadre( ) && ! getFlag( GroundUnit::CDR_UNSUP ) ) )
			strcat( str, "  (self supported)" );
		else if ( ! isSupported( ) )
			strcat( str, "  (unsupported)" );
	}

	// supply status:
	char tmp[100];
	if ( getSupply( ) == SupplyStatus::NONE )
	{
		sprintf( tmp, ", Out of supply %.1f turns", ( (float)getSupplyTurns( ) ) / 2 );
		strcat( str, tmp );
	}

	if ( getOverstack( ) )
		strcat( str, "  In overstack" );

	// extra info
	if ( verbose )
	{
		sprintf( tmp, "\n%s, %.1f REs", isAxis( ) ? "AXIS" : "ALLIED", getRE( ) );
		strcat( str, tmp );
		if ( isSupplementalMotorized( ) )
			strcat( str, ",  c/m" );
		sprintf( tmp, ",  AECA: %s  AECD: %s  ATEC: %s", aec2str( getAECA( ) ), aec2str( getAECD( ) ), aec2str( getATEC( ) ) );
		strcat( str, tmp );

		// ZOC
		if ( hasNormalZOC( ) )
			strcat( str, ",  has ZOC" );
		else if ( hasReducedZOC( ) )
			strcat( str, ",  has reduced ZOC" );
		else
			strcat( str, ",  no ZOC" );

		// misc attribs
		if ( hasHE( ) )
			strcat( str, ",  has HE" );
		if ( isConstructionEngineer( ) )
			strcat( str, ",  construction capable" );
		if ( isWW1Infantry( ) )
			strcat( str, ",  WW1 infantry" );
#if 0
		strcat( str, "]\n" );
#endif
	}

	// append orders text
	if ( getMission( ) )
	{
		strcat( str, "\n" );
		switch ( getMission( ) )
		{
			case GroundUnit::Project::AIRFIELD:
				strcat( str, "Building airfield, " );
				break;
			case GroundUnit::Project::FORT:
				strcat( str, "Building fort, " );
				break;
			case GroundUnit::Project::PORT:
				strcat( str, "Repairing port, " );
				break;
		}
		sprintf( tmp, "%d turns done", getCompletedTurns( ) );
		strcat( str, tmp );
	}

	// show attack orders only if combat markers are visible
	if ( _show_combat_markers )
		switch ( getAttackDir( ) )
		{
			case Hex::WEST:
				strcat( str, "\nOrders: attack WEST" );
				break;
			case Hex::NORTHWEST:
				strcat( str, "\nOrders: attack NORTHWEST" );
				break;
			case Hex::SOUTHWEST:
				strcat( str, "\nOrders: attack SOUTHWEST" );
				break;
			case Hex::EAST:
				strcat( str, "\nOrders: attack EAST" );
				break;
			case Hex::NORTHEAST:
				strcat( str, "\nOrders: attack NORTHEAST" );
				break;
			case Hex::SOUTHEAST:
				strcat( str, "\nOrders: attack SOUTHEAST" );
				break;
		}

	// if disrupted, say so:
	switch ( getDisruption( ) )
	{
		case DisruptionStatus::DISRUPTED:
			strcat( str, "\nUnit is disrupted!" );
			break;
		case DisruptionStatus::BADLY_DISRUPTED:
			strcat( str, "\nUnit is badly disrupted!" );
			break;
		default:
			break;
	}

}

// Build strings like "XX 5-7-6  (122)" to "str" , used with symbols
void GroundUnit::getSymbolString( char* str )
{
	char tmp[100];

	if ( getType( ) == GroundUnitType::WW2::PARTISAN )
	{
		if ( ( getSide( ) == SidePlayer::AXIS && _rule_set.HideAxPartisans == TRUE ) || ( getSide( ) == SidePlayer::ALLIED && _rule_set.HideAlPartisans == TRUE ) )
		{
			sprintf( str, "Partisans (hidden) " );
			return;
		}
	}

	// pos AA is different!
	if ( getType( ) == GroundUnitType::WW2::POS_LT_AA )
	{
		sprintf( str, "%d pts lt AA       ", getFlak( ) );
		return;
	}
	else if ( getType( ) == GroundUnitType::WW2::POS_HV_AA )
	{
		sprintf( str, "%d pts hv AA       ", getFlak( ) );
		return;
	}
	else if ( getType( ) == GroundUnitType::WW2::TRUCK )
	{
		sprintf( str, "Truck              " );
		return;
	}

	strcpy( str, "" );
	strcpy( tmp, "" );

	strcat( tmp, getID( ) );
	line2spc( tmp );
	if ( strlen( tmp ) > 0 && isOkStr( tmp ) )
	{
		strcat( tmp, atoi( tmp ) > 0 ? ". " : " " );
		strcat( str, tmp );
	}

	if ( getFlag( GroundUnit::RESERVE ) && getType( ) != GroundUnitType::WW2::RES )
		strcat( str, "Res " );
	if ( getFlag( GroundUnit::TRAINING ) && getType( ) != GroundUnitType::WW2::TRNG )
		strcat( str, "Trng " );
	if ( getFlag( GroundUnit::MOUNTAIN ) && getType( ) != GroundUnitType::WW2::MTN && getType( ) != GroundUnitType::WW2::HIGH_MOUNTAIN )
		strcat( str, "Mtn " );

	// unit name, modify it per nationality:
	if ( getType( ) == GroundUnitType::WW2::ARM && isGerman( ) )
		strcat( str, "Pz" );
	else if ( getType( ) == GroundUnitType::WW2::FL_TANK && isGerman( ) )
		strcat( str, "Fl Pz" );
	else if ( getType( ) == GroundUnitType::WW2::ARM && isSoviet( ) )
		strcat( str, "Tank" );
	else if ( getType( ) == GroundUnitType::WW2::MECH && isGerman( ) )
		strcat( str, "PzG" );
	else if ( getType( ) == GroundUnitType::WW2::PARA_INF && getSubType( ) == GroundArmedForce::Allied::USA )
		strcat( str, "Abn" );
	else if ( getType( ) == GroundUnitType::WW2::MTN && getSubType( ) == GroundArmedForce::Axis::ITA )
		strcat( str, "Alpn" );
	else if ( getType( ) == GroundUnitType::WW2::LT_INF && isGerman( ) )
		strcat( str, "Jgr" );
	else
		strcat( str, _ground_unit_type[getType( )].shortname_.c_str( ) );

	strcat( str, " " );
	switch ( getSize( ) )
	{
		case GroundUnit::Size::HQ:
			strcat( str, "HQ " );
			break;
		case GroundUnit::Size::CORPSHQ:
			strcat( str, "XXX HQ " );
			break;
		case GroundUnit::Size::ARMYHQ:
			strcat( str, "XXXX HQ " );
			break;
		case GroundUnit::Size::COMPANY:
			strcat( str, isCadre( ) ? "i  " : "I  " );
			break;
		case GroundUnit::Size::BATALLION:
			strcat( str, isCadre( ) ? "ii" : "II" );
			break;
		case GroundUnit::Size::BATT_GROUP:
			strcat( str, isCadre( ) ? "[ii]" : "[II]" );
			break;
		case GroundUnit::Size::REGIMENT:
			strcat( str, isCadre( ) ? "iii" : "III" );
			break;
		case GroundUnit::Size::REG_GROUP:
			strcat( str, isCadre( ) ? "[iii]" : "[III]" );
			break;
		case GroundUnit::Size::BRIGADE:
			strcat( str, isCadre( ) ? "x  " : "X  " );
			break;
		case GroundUnit::Size::BRIG_GROUP:
			strcat( str, isCadre( ) ? "[x]" : "[X]" );
			break;
		case GroundUnit::Size::DIVISIONAL:
			strcat( str, isCadre( ) ? "Cdr" : "[XX]" );
			break;
		case GroundUnit::Size::DIVISION:
			if ( isCadre( ) )
				strcat( str, isGerman( ) ? "DG" : "Cdr" );
			else
				strcat( str, "XX " );
			break;
		case GroundUnit::Size::CORPS:
			if ( isCadre( ) )
				strcat( str, isGerman( ) ? "DG" : "Cdr" );
			else
				strcat( str, "XXX" );
			break;
		case GroundUnit::Size::ARMY:
			strcat( str, isCadre( ) ? "Cdr" : "XXXX" );
			break;
	}

	strcat( str, "          " );
}

void GroundUnit::drawBigCounter( wxDC* scr_hdc, int x, int y, bool border /* = true */ )
{
	if ( border )
	{
		scr_hdc->SetPen( *wxBLACK_PEN );  // always black border on counter
		scr_hdc->SetBrush( wxNullBrush ); // clear the brush, don't fill the rectangle
		scr_hdc->DrawRectangle( x, y, 54, 52 );
	}

	// NOTE:  due to the gotos in this function (and cross-initialization
	//	concerns therefrom), many of these declarations must be near the top
	//	of the function rather than the more preferred declare at point of
	//	first use

	wxMemoryDC memdc;
	wxMemoryDC mem2dc;
	wxMemoryDC dbuf_dc;

	wxDC* hdc;

	wxColour bgnd = _ground_armed_force[getSide( )][getSubType( )].crBg;
	wxBrush bgnd_brush( bgnd, wxSOLID );
	wxPen bgnd_pen( bgnd, 1, wxSOLID );

	wxColor fgnd = _ground_armed_force[getSide( )][getSubType( )].crFg;
	wxBrush fgnd_brush( fgnd, wxSOLID );
	wxPen fgnd_pen( fgnd, 1, wxSOLID );

	wxColor txtcolor = _ground_armed_force[getSide( )][getSubType( )].crText;

	// size the unit type symbol box
	const int W = 26; // 13
	const int W_SYM = 26;
	const int H = 22;
	wxBitmap bmp( W, H );

	// text sizing parameters
	int length;
	int x_pos;
	int x_cor = 0;
	int y_pos;

	bool draw_wheels = isSupplementalMotorized( ) && ! isCombatMotClass( );

	char str[20];

	// get just inside the upper left of the border (even if it's not drawn)
	int scr_x = x + 1;
	int scr_y = y + 1;

	if ( bmpCounter && image_ok_ && allow_cache_ )
	{
		scr_hdc->DrawBitmap( *bmpCounter, scr_x, scr_y );
		goto draw_supply_status;
	}

	if ( bmpCounter == nullptr )
		bmpCounter = new wxBitmap( 52, 50 );

	if ( bmpCounter == nullptr )
	{
		wxMessageBox( wxT("Memory allocation failed, can't create\nnew bitmap for counter!"), wxT("ERROR!") );
		return;
	}

	// draw to counter bitmap instead of the screen
	dbuf_dc.SelectObject( *bmpCounter );
	hdc = &dbuf_dc;

	// draw counter background
	hdc->SetBrush( bgnd_brush );
	hdc->SetPen( bgnd_pen );

	// no border, but need to align as if there is one
	x = -1, y = -1;

	// counter upper left is now zero point
	hdc->DrawRectangle( x + 1, y + 1, 52, 50 );

	// draw unit symbol
	mem2dc.SelectObject( bmp ); // mem2dc = counter color

	mem2dc.SetBrush( fgnd_brush );
	mem2dc.SetPen( fgnd_pen );

	mem2dc.DrawRectangle( 0, 0, W, H );

	memdc.SelectObject( *_ground_unit_type[getType( )].hbmBitmap );  // symbol-bitmap = mask

	hdc->Blit( x + 14, y + 12, W_SYM, H, &mem2dc, 0, 0, wxXOR ); // color
	hdc->Blit( x + 14, y + 12, W_SYM, H, &memdc, 0, 0, wxAND );

	// decorate with:
	if ( getFlag( GroundUnit::RESERVE ) )
	{
		memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::RES].hbmBitmap );
		hdc->Blit( x + 14, y + 12, W_SYM, H, &memdc, 0, 0, wxAND );
	}
	if ( getFlag( GroundUnit::TRAINING ) )
	{
		memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::TRNG].hbmBitmap );
		hdc->Blit( x + 14, y + 12, W_SYM, H, &memdc, 0, 0, wxAND );
	}
	if ( getFlag( GroundUnit::MOUNTAIN ) )
	{
		memdc.SelectObject( *hbmMtnMask );
		hdc->Blit( x + 14, y + 12, W_SYM, H, &memdc, 0, 0, wxAND );
	}

	hdc->Blit( x + 14, y + 12, W_SYM, H, &mem2dc, 0, 0, wxXOR ); // color again

	// draw wheels if motorized
	if ( draw_wheels )
	{
		hdc->SetBrush( fgnd_brush );
		hdc->SetPen( fgnd_pen );
		hdc->DrawRectangle( x + 18, y + 34, 3, 2 ); // left wheel
		hdc->DrawRectangle( x + 32, y + 34, 3, 2 ); // right wheel
	}

	// special handling for hidden partisans
	if ( ( _rule_set.HideAxPartisans == TRUE && getSide( ) == SidePlayer::AXIS && getType( ) == GroundUnitType::WW2::PARTISAN )
		|| ( _rule_set.HideAlPartisans == TRUE && getSide( ) == SidePlayer::ALLIED && getType( ) == GroundUnitType::WW2::PARTISAN ) )
	{
		hdc->SetTextBackground( bgnd );
		hdc->SetTextForeground( txtcolor );
		sprintf( str, "P%d", getMaxMP( ) );

		length = strLen( str );
		x_pos = x + 28 - ( length / 2 );  // width=28 -> 14 = half way
		y_pos = y + 30;

		drawStr( hdc, x_pos, y_pos + 6, str );

		goto end;
	}

	// draw size

	// HQs have flagpole but no size markers
	if ( getSize( ) == GroundUnit::Size::HQ || getSize( ) == GroundUnit::Size::CORPSHQ || getSize( ) == GroundUnit::Size::ARMYHQ )
	{
		hdc->SetPen( fgnd_pen );
		hdc->DrawLine( x + 6, y + 6, x + 6, y + 26 );
		hdc->DrawLine( x + 7, y + 17, x + 7, y + 26 );
		hdc->SetPen( *wxBLACK_PEN );  // needed???
	}

	// transport units have no size symbol, show RE-capacity in lieu
	if ( getType( ) == GroundUnitType::WW2::TRANSPORT || getType( ) == GroundUnitType::WW2::APC || getType( ) == GroundUnitType::WW2::LVT )
	{
		switch ( getCustomRE2( ) )
		{
			case 6:
				memdc.SelectObject( *hbm3RE );
				break;
			case 4:
				memdc.SelectObject( *hbm2RE );
				break;
			case 2:
			default:
				memdc.SelectObject( *hbm1RE );
				break;
		}
		hdc->Blit( x + 7, y + 1, 13, 5, &mem2dc, 0, 0, wxXOR );
		hdc->Blit( x + 7, y + 1, 13, 5, &memdc, 0, 0, wxAND );
		hdc->Blit( x + 7, y + 1, 13, 5, &mem2dc, 0, 0, wxXOR );
	}

	// port forts, fort areas, truck, and pos AA have no size
	else if (	getType( ) != GroundUnitType::WW2::PORT_FORT &&
				getType( ) != GroundUnitType::WW2::POS_LT_AA &&
				getType( ) != GroundUnitType::WW2::POS_HV_AA &&
				getType( ) != GroundUnitType::WW2::TRUCK	&&
				getType( ) != GroundUnitType::WW2::FORTAREA &&
				getSize( ) != GroundUnit::Size::NO_SIZE )
	{
		int unit_size = getSize( );
		switch ( unit_size )
		{
			case GroundUnit::Size::DIVISION:
			case GroundUnit::Size::DIVISIONAL:
				if ( isCadre( ) )
				{
					if ( isGerman( ) )
						memdc.SelectObject( *hbmDG );
					else
						memdc.SelectObject( *hbmCadre );
				}
				else
					memdc.SelectObject( unit_size == GroundUnit::Size::DIVISION ? *hbmDivision : *hbmDivisional );
				break;
			case GroundUnit::Size::REGIMENT:
				memdc.SelectObject( *hbmRegiment );
				break;
			case GroundUnit::Size::BRIGADE:
				memdc.SelectObject( *hbmBrigade );
				break;
			case GroundUnit::Size::BATALLION:
				memdc.SelectObject( *hbmBatallion );
				break;
			case GroundUnit::Size::HQ:
				memdc.SelectObject( *hbmDivision );
				break;
			case GroundUnit::Size::CORPSHQ:
				memdc.SelectObject( *hbmCorps );
				break;
			case GroundUnit::Size::ARMYHQ:
				memdc.SelectObject( *hbmArmy );
				break;
			case GroundUnit::Size::REG_GROUP:
				memdc.SelectObject( *hbmRegimentGrp );
				break;
			case GroundUnit::Size::BRIG_GROUP:
				memdc.SelectObject( *hbmBrigadeGrp );
				break;
			case GroundUnit::Size::BATT_GROUP:
				memdc.SelectObject( *hbmBatallionGrp );
				break;
			case GroundUnit::Size::CORPS:
				if ( isCadre( ) )
				{
					if ( isGerman( ) )
						memdc.SelectObject( *hbmDG );
					else
						memdc.SelectObject( *hbmCadre );
				}
				else
					memdc.SelectObject( *hbmCorps );
				break;
			case GroundUnit::Size::ARMY:
				if ( isCadre( ) )
					memdc.SelectObject( *hbmCadre );
				else
					memdc.SelectObject( *hbmArmy );
				break;
			case GroundUnit::Size::COMPANY:
			default:
				memdc.SelectObject( *hbmCompany );
				break;
		}

		// blit the size symbol
		if ( getSize( ) == GroundUnit::Size::ARMYHQ || getSize( ) == GroundUnit::Size::ARMY ) // 15x4 bitmap
		{
			hdc->Blit( x + 6, y + 1, 15, 4, &mem2dc, 0, 0, wxXOR );
			hdc->Blit( x + 6, y + 1, 15, 4, &memdc, 0, 0, wxAND );
			hdc->Blit( x + 6, y + 1, 15, 4, &mem2dc, 0, 0, wxXOR );
		}
		else // 22x8 bitmap
		{
			hdc->Blit( x + 16, y + 2, 22, 8, &mem2dc, 0, 0, wxXOR );
			hdc->Blit( x + 16, y + 2, 22, 8, &memdc, 0, 0, wxAND );
			hdc->Blit( x + 16, y + 2, 22, 8, &mem2dc, 0, 0, wxXOR );
		}

		// non-divisional cadres: draw line across symbol
		if ( getSize( ) < GroundUnit::Size::DIVISIONAL && isCadre( ) )
		{
			hdc->SetPen( fgnd_pen );
			hdc->DrawLine( x + 8, y + 3, x + 19, y + 3 );
		}
	}

	// draw supported dot in UL (except for fortified areas)
	if ( getType( ) != GroundUnitType::WW2::TRUCK
		&& ( ( getFlag( GroundUnit::SUPPORTED ) && getType( ) != GroundUnitType::WW2::FORTAREA )
			|| ( getSize( ) >= GroundUnit::Size::DIVISIONAL && isCadre( ) && ! getFlag( GroundUnit::CDR_UNSUP ) ) ) )
	{
		int sx = x + 6 ; // EJW: Always upper left
		hdc->SetPen( fgnd_pen );
		hdc->DrawLine( sx, y + 4, sx + 4, y + 4 );
		hdc->DrawLine( sx - 2, y + 6, sx + 6, y + 6 );
		hdc->DrawLine( sx - 2, y + 8, sx + 6, y + 8 );
		hdc->DrawLine( sx, y + 10, sx + 4, y + 10 );
#if 0
		hdc->DrawCircle( sx + 2, y + 6, 6 );
		hdc->DrawCircle( sx + 2, y + 6, 5 );
		hdc->DrawCircle( sx + 2, y + 6, 4 );
		hdc->DrawCircle( sx + 2, y + 6, 3 );
		hdc->DrawCircle( sx + 2, y + 6, 2 );
#endif
	}
	else if ( getFlag( GroundUnit::UNSUPPORTED ) ) // draw unsupported dot in UL
	{
		int sx = x + 6 ; // EJW: Always upper left
		hdc->SetPen( fgnd_pen );
		hdc->DrawLine( sx, y + 4, sx - 2, y + 6 );
		hdc->DrawLine( sx - 2, y + 6, sx - 2, y + 8 );
		hdc->DrawLine( sx - 2, y + 8, sx, y + 10 );
		hdc->DrawLine( sx, y + 10, sx + 2, y + 10 );
		hdc->DrawLine( sx + 2, y + 10, sx + 4, y + 8 );
		hdc->DrawLine( sx + 4, y + 8, sx + 4, y + 6 );
		hdc->DrawLine( sx + 4, y + 6, sx + 2, y + 4 );
		hdc->DrawLine( sx + 2, y + 4, sx, y + 4 );
#if 0
		hdc->DrawCircle( sx + 2, y + 6, 6 );
		hdc->DrawCircle( sx + 2, y + 6, 5 );
#endif
	}

	// write various text
	hdc->SetTextBackground( bgnd );
	hdc->SetTextForeground( txtcolor );
	// write AA str in UL
	if ( getFlak( ) )
	{
		sprintf( str, "%d", getFlak( ) );
		drawStr( hdc, x + 46, y + 8, str ); // EJW: now upper right
	}

	if ( getFlag( GroundUnit::RAILONLY ) )
	{
		if ( getAtt( ) == getDef( ) )
			sprintf( str, "%d-R", getAtt( ) );
		else
			sprintf( str, "%d-%d-R", getAtt( ), getDef( ) );
	}
	else if ( getFlag( GroundUnit::WATERONLY ) )
	{
		if ( getAtt( ) == getDef( ) )
			sprintf( str, "%d-W", getAtt( ) );
		else
			sprintf( str, "%d-%d-W", getAtt( ), getDef( ) );
	}
	else // transport units, trucks, and HQ / pos AA:  don't print str (it's zero)
	{
		if ( getType( ) == GroundUnitType::WW2::TRANSPORT || getType( ) == GroundUnitType::WW2::APC || getType( ) == GroundUnitType::WW2::LVT )
		{
			// print text in "(+1)-10" fashion
			if ( getAtt( ) == 0 && getDef( ) == 0 )
				sprintf( str, "%d", getMaxMP( ) );
			else if ( getAtt( ) == getDef( ) )
				{
				sprintf( str, "+%d-%d", getAtt( ), getMaxMP( ) );
				x_cor = -3;
				}
			else
				{
				sprintf( str, "+%d+%d-%d", getAtt( ), getDef( ), getMaxMP( ) );
				x_cor = -6;
				}
		}
		else if ( getType( ) == GroundUnitType::WW2::TRUCK )
		{
			if ( getFlag( GroundUnit::IS_CADRE ) ) // cadre'd truck means depleted ("D")
				sprintf( str, "%dD", getMaxMP( ) );
			else
				sprintf( str, "%d", getMaxMP( ) );
		}
		else if ( getSize( ) == GroundUnit::Size::HQ || getSize( ) == GroundUnit::Size::CORPSHQ || getSize( ) == GroundUnit::Size::ARMYHQ
				|| getType( ) == GroundUnitType::WW2::POS_LT_AA || getType( ) == GroundUnitType::WW2::POS_HV_AA )
			sprintf( str, "%d", getMaxMP( ) );
		else if ( getAtt( ) == getDef( ) )
		{
			sprintf( str, "%d-%d", getAtt( ), getMaxMP( ) );
			x_cor = -3;
		}

		else
			{
			sprintf( str, "%d-%d-%d", getAtt( ), getDef( ), getMaxMP( ) );
			x_cor = -6;
			}
	}

	// str text position
	length = strLen( str );
	// EJW: Something isn't quite right with the x position, maybe - aren't counted
	// DTM:  possible that you don't want to halve length here?  (if you've doubled font size)
	x_pos = x + 26 + x_cor - length / 2;  // width=28 -> 14 == half way
	y_pos = y + ( draw_wheels ? 36 : 34 );

	// write the text
#if 0
	// old code for reference:  why did you not use this?  not working right for double font/size?
	drawStr( hdc, x_pos, y_pos + 3, str );
#endif
	// DTM:  you need to set the font/size you want here, see wxFont docs at wxwidgets.org...
	hdc->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, true ) );
	hdc->DrawText( wxS( str ), x_pos, y_pos );

	// done preparing the image (rest of function is
	//		for supply/disruption markings only)
	image_ok_ = true;
	// TODO: but what about allow_cache_ ?

	end:
	dbuf_dc.SelectObject( wxNullBitmap );
	hdc = nullptr;
	scr_hdc->DrawBitmap( *bmpCounter, scr_x, scr_y );

	draw_supply_status:
	if ( getSupply( ) == SupplyStatus::NONE )
	{
		// which color to use in U-marker (sides reversed in reaction phase!)
		// TODO: does this work correctly for Axis P.T. during a reaction phase?
		if ( _phasing_player == SidePlayer::AXIS || _current_phase == Phase::REACTION_PHASE )
		{
			if ( getSupplyTurns( ) % 2 )
				scr_hdc->SetTextForeground( wxColour( 0, 0, 100 ) );
			else
				scr_hdc->SetTextForeground( wxColour( 255, 0, 0 ) );
		}
		else
		{
			if ( getSupplyTurns( ) % 2 )
				scr_hdc->SetTextForeground( wxColour( 255, 0, 0 ) );
			else
				scr_hdc->SetTextForeground( wxColour( 0, 0, 100 ) );
		}
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		scr_hdc->DrawRectangle( scr_x + 19, scr_y, 13, 13 );
		int t = ( ( getSupplyTurns( ) - 1 ) / 2 ) + 1;
		t = ( t > 4 ) ? 4 : t;
		sprintf( str, "U%d", t );
		drawStr( scr_hdc, scr_x + 21, scr_y + 2, str );
	}

	// if isolated then draw tiny "I" marker
	if ( getIsolated( ) )
	{
		scr_hdc->SetTextForeground( *wxBLACK );
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		scr_hdc->DrawRectangle( scr_x + 19, scr_y + 13, 13, 13 );
		sprintf( str, "I" );
		drawStr( scr_hdc, scr_x + 24, scr_y + 15, str );
	}

	// draw attack supply marker (if attack supply rule is enabled)
	if ( _rule_set.AttackSup == TRUE && getAttackSupply( ) )
	{
		scr_hdc->SetTextForeground( wxColour( 0, 150, 0 ) );
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		scr_hdc->DrawRectangle( x, y, 10, 10 );
		sprintf( str, "A" );
		scr_hdc->DrawText( wxS( str ), scr_x + 1, scr_y - 1 );
	}

	// draw disruption status symbol if disrupted or badly disrupted:
	if ( getDisruption( ) > DisruptionStatus::NOT_DISRUPTED )
	{
		scr_hdc->SetTextForeground( *wxBLACK );
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		if ( getDisruption( ) == DisruptionStatus::DISRUPTED )
		{
			scr_hdc->DrawRectangle( scr_x - 1, scr_y + 5, 10, 12 );
			sprintf( str, "D" );
			drawStr( scr_hdc, scr_x + 1, scr_y + 7, str );
		}
		else // must be badly disrupted
		{
			scr_hdc->DrawRectangle( scr_x - 1, scr_y + 5, 14, 12 );
			sprintf( str, "BD" );
			drawStr( scr_hdc, scr_x, scr_y + 7, str );
		}
	}
}

void GroundUnit::drawCounter( wxDC* scr_hdc, int x, int y, bool border /* = true */ )
{
	if ( border )
	{
		scr_hdc->SetPen( *wxBLACK_PEN );  // always black border on counter
		scr_hdc->SetBrush( wxNullBrush ); // clear the brush, don't fill the rectangle
		scr_hdc->DrawRectangle( x, y, 28, 27 );
	}

	// NOTE:  due to the gotos in this function (and cross-initialization
	//	concerns therefrom), many of these declarations must be near the top
	//	of the function rather than the more preferred declare at point of
	//	first use

	wxMemoryDC memdc;
	wxMemoryDC mem2dc;
	wxMemoryDC dbuf_dc;

	wxDC* hdc;

	wxColour bgnd = _ground_armed_force[getSide( )][getSubType( )].crBg;
	wxBrush bgnd_brush( bgnd, wxSOLID );
	wxPen bgnd_pen( bgnd, 1, wxSOLID );

	wxColor fgnd = _ground_armed_force[getSide( )][getSubType( )].crFg;
	wxBrush fgnd_brush( fgnd, wxSOLID );
	wxPen fgnd_pen( fgnd, 1, wxSOLID );

	wxColor txtcolor = _ground_armed_force[getSide( )][getSubType( )].crText;

	// size the unit type symbol box
	const int W = 15; // 13
	const int W_SYM = 13;
	const int H = 11;
	wxBitmap bmp( W, H );

	// text sizing parameters
	int length;
	int x_pos;
	int y_pos;

	bool draw_wheels = isSupplementalMotorized( ) && ! isCombatMotClass( );

	char str[20];

	// get just inside the upper left of the border (even if it's not drawn)
	int scr_x = x + 1;
	int scr_y = y + 1;

	if ( bmpCounter && image_ok_ && allow_cache_ )
	{
		scr_hdc->DrawBitmap( *bmpCounter, scr_x, scr_y );
		goto draw_supply_status;
	}

	if ( bmpCounter == nullptr )
		bmpCounter = new wxBitmap( 26, 25 );

	if ( bmpCounter == nullptr )
	{
		wxMessageBox( wxT("Memory allocation failed, can't create\nnew bitmap for counter!"), wxT("ERROR!") );
		return;
	}

	// draw to counter bitmap instead of the screen
	dbuf_dc.SelectObject( *bmpCounter );
	hdc = &dbuf_dc;

	// draw counter background
	hdc->SetBrush( bgnd_brush );
	hdc->SetPen( bgnd_pen );

	// no border, but need to align as if there is one
	x = -1, y = -1;

	// counter upper left is now zero point
	hdc->DrawRectangle( x + 1, y + 1, 26, 25 );

	// draw unit symbol
	mem2dc.SelectObject( bmp ); // mem2dc = counter color

	mem2dc.SetBrush( fgnd_brush );
	mem2dc.SetPen( fgnd_pen );

	mem2dc.DrawRectangle( 0, 0, W, H );

	memdc.SelectObject( *_ground_unit_type[getType( )].hbmBitmap );  // symbol-bitmap = mask

	hdc->Blit( x + 7, y + 6, W_SYM, H, &mem2dc, 0, 0, wxXOR ); // color
	hdc->Blit( x + 7, y + 6, W_SYM, H, &memdc, 0, 0, wxAND );

	// decorate with:
	if ( getFlag( GroundUnit::RESERVE ) )
	{
		memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::RES].hbmBitmap );
		hdc->Blit( x + 7, y + 6, W_SYM, H, &memdc, 0, 0, wxAND );
	}
	if ( getFlag( GroundUnit::TRAINING ) )
	{
		memdc.SelectObject( *_ground_unit_type[GroundUnitType::WW2::TRNG].hbmBitmap );
		hdc->Blit( x + 7, y + 6, W_SYM, H, &memdc, 0, 0, wxAND );
	}
	if ( getFlag( GroundUnit::MOUNTAIN ) )
	{
		memdc.SelectObject( *hbmMtnMask );
		hdc->Blit( x + 7, y + 6, W_SYM, H, &memdc, 0, 0, wxAND );
	}

	hdc->Blit( x + 7, y + 6, W_SYM, H, &mem2dc, 0, 0, wxXOR ); // color again

	// draw wheels if motorized
	if ( draw_wheels )
	{
		hdc->SetBrush( fgnd_brush );
		hdc->SetPen( fgnd_pen );
		hdc->DrawRectangle( x + 9, y + 17, 3, 2 ); // left wheel
		hdc->DrawRectangle( x + 16, y + 17, 3, 2 ); // right wheel
	}

	// special handling for hidden partisans
	if ( ( _rule_set.HideAxPartisans == TRUE && getSide( ) == SidePlayer::AXIS && getType( ) == GroundUnitType::WW2::PARTISAN )
		|| ( _rule_set.HideAlPartisans == TRUE && getSide( ) == SidePlayer::ALLIED && getType( ) == GroundUnitType::WW2::PARTISAN ) )
	{
		hdc->SetTextBackground( bgnd );
		hdc->SetTextForeground( txtcolor );
		sprintf( str, "P%d", getMaxMP( ) );

		length = strLen( str );
		x_pos = x + 14 - ( length / 2 );  // width=28 -> 14 = half way
		y_pos = y + 15;

		drawStr( hdc, x_pos, y_pos + 3, str );

		goto end;
	}

	// draw size

	// HQs have flagpole but no size markers
	if ( getSize( ) == GroundUnit::Size::HQ || getSize( ) == GroundUnit::Size::CORPSHQ || getSize( ) == GroundUnit::Size::ARMYHQ )
	{
		hdc->SetPen( fgnd_pen );
		hdc->DrawLine( x + 6, y + 6, x + 6, y + 26 );
		hdc->DrawLine( x + 7, y + 17, x + 7, y + 26 );
		hdc->SetPen( *wxBLACK_PEN );  // needed???
	}

	// transport units have no size symbol, show RE-capacity in lieu
	if ( getType( ) == GroundUnitType::WW2::TRANSPORT || getType( ) == GroundUnitType::WW2::APC || getType( ) == GroundUnitType::WW2::LVT )
	{
		switch ( getCustomRE2( ) )
		{
			case 6:
				memdc.SelectObject( *hbm3RE );
				break;
			case 4:
				memdc.SelectObject( *hbm2RE );
				break;
			case 2:
			default:
				memdc.SelectObject( *hbm1RE );
				break;
		}
		hdc->Blit( x + 7, y + 1, 13, 5, &mem2dc, 0, 0, wxXOR );
		hdc->Blit( x + 7, y + 1, 13, 5, &memdc, 0, 0, wxAND );
		hdc->Blit( x + 7, y + 1, 13, 5, &mem2dc, 0, 0, wxXOR );
	}

	// port forts, fort areas, truck, and pos AA have no size
	else if (	getType( ) != GroundUnitType::WW2::PORT_FORT &&
				getType( ) != GroundUnitType::WW2::POS_LT_AA &&
				getType( ) != GroundUnitType::WW2::POS_HV_AA &&
				getType( ) != GroundUnitType::WW2::TRUCK	&&
				getType( ) != GroundUnitType::WW2::FORTAREA &&
				getSize( ) != GroundUnit::Size::NO_SIZE )
	{
		int unit_size = getSize( );
		switch ( unit_size )
		{
			case GroundUnit::Size::DIVISION:
			case GroundUnit::Size::DIVISIONAL:
				if ( isCadre( ) )
				{
					if ( isGerman( ) )
						memdc.SelectObject( *hbmDG );
					else
						memdc.SelectObject( *hbmCadre );
				}
				else
					memdc.SelectObject( unit_size == GroundUnit::Size::DIVISION ? *hbmDivision : *hbmDivisional );
				break;
			case GroundUnit::Size::REGIMENT:
				memdc.SelectObject( *hbmRegiment );
				break;
			case GroundUnit::Size::BRIGADE:
				memdc.SelectObject( *hbmBrigade );
				break;
			case GroundUnit::Size::BATALLION:
				memdc.SelectObject( *hbmBatallion );
				break;
			case GroundUnit::Size::HQ:
				memdc.SelectObject( *hbmDivision );
				break;
			case GroundUnit::Size::CORPSHQ:
				memdc.SelectObject( *hbmCorps );
				break;
			case GroundUnit::Size::ARMYHQ:
				memdc.SelectObject( *hbmArmy );
				break;
			case GroundUnit::Size::REG_GROUP:
				memdc.SelectObject( *hbmRegimentGrp );
				break;
			case GroundUnit::Size::BRIG_GROUP:
				memdc.SelectObject( *hbmBrigadeGrp );
				break;
			case GroundUnit::Size::BATT_GROUP:
				memdc.SelectObject( *hbmBatallionGrp );
				break;
			case GroundUnit::Size::CORPS:
				if ( isCadre( ) )
				{
					if ( isGerman( ) )
						memdc.SelectObject( *hbmDG );
					else
						memdc.SelectObject( *hbmCadre );
				}
				else
					memdc.SelectObject( *hbmCorps );
				break;
			case GroundUnit::Size::ARMY:
				if ( isCadre( ) )
					memdc.SelectObject( *hbmCadre );
				else
					memdc.SelectObject( *hbmArmy );
				break;
			case GroundUnit::Size::COMPANY:
			default:
				memdc.SelectObject( *hbmCompany );
				break;
		}

		// blit the size symbol
		if ( getSize( ) == GroundUnit::Size::ARMYHQ || getSize( ) == GroundUnit::Size::ARMY ) // 15x4 bitmap
		{
			hdc->Blit( x + 6, y + 1, 15, 4, &mem2dc, 0, 0, wxXOR );
			hdc->Blit( x + 6, y + 1, 15, 4, &memdc, 0, 0, wxAND );
			hdc->Blit( x + 6, y + 1, 15, 4, &mem2dc, 0, 0, wxXOR );
		}
		else // 11x4 bitmap
		{
			hdc->Blit( x + 8, y + 1, 11, 4, &mem2dc, 0, 0, wxXOR );
			hdc->Blit( x + 8, y + 1, 11, 4, &memdc, 0, 0, wxAND );
			hdc->Blit( x + 8, y + 1, 11, 4, &mem2dc, 0, 0, wxXOR );
		}

		// non-divisional cadres: draw line across symbol
		if ( getSize( ) < GroundUnit::Size::DIVISIONAL && isCadre( ) )
		{
			hdc->SetPen( fgnd_pen );
			hdc->DrawLine( x + 8, y + 3, x + 19, y + 3 );
		}
	}

	hdc->SetPen( fgnd_pen );

	// draw supported dot in UL (except for fortified areas)
	if ( getType( ) != GroundUnitType::WW2::TRUCK
		&& ( ( getFlag( GroundUnit::SUPPORTED ) && getType( ) != GroundUnitType::WW2::FORTAREA )
			|| ( getSize( ) >= GroundUnit::Size::DIVISIONAL && isCadre( ) && ! getFlag( GroundUnit::CDR_UNSUP ) ) ) )
	{
		int sx = x + 3 ; // EJW: Always upper left
#if 0
		int sx = x + ( getFlak( ) ? 23 : 3 ); // when AA str, draw supported dot in UR
#endif
		hdc->DrawLine( sx, y + 2, sx + 2, y + 2 );
		hdc->DrawLine( sx - 1, y + 3, sx + 3, y + 3 );
		hdc->DrawLine( sx - 1, y + 4, sx + 3, y + 4 );
        hdc->DrawLine( sx, y + 5, sx + 2, y + 5 );
	}
	else if ( getFlag( GroundUnit::UNSUPPORTED ) ) // draw unsupported dot in UL
	{
		int sx = x + 3 ; // EJW: Always upper left
#if 0
		int sx = x + ( getFlak( ) ? 22 : 3 ); // when AA str, draw supported dot in UR
#endif
		hdc->DrawLine( sx, y + 2, sx - 1, y + 3 );
		hdc->DrawLine( sx - 1, y + 3, sx - 1, y + 4 );
		hdc->DrawLine( sx - 1, y + 4, sx, y + 5 );
		hdc->DrawLine( sx, y + 5, sx + 1, y + 5 );
		hdc->DrawLine( sx + 1, y + 5, sx + 2, y + 4 );
		hdc->DrawLine( sx + 2, y + 4, sx + 2, y + 3 );
		hdc->DrawLine( sx + 2, y + 3, sx + 1, y + 2 );
		hdc->DrawLine( sx + 1, y + 2, sx, y + 2 );
	}

	// write various text
	hdc->SetTextBackground( bgnd );
	hdc->SetTextForeground( txtcolor );

	// write AA str in UL
	if ( getFlak( ) )
	{
		sprintf( str, "%d", getFlak( ) );
        drawStr( hdc, x + 23, y + 2, str ); // EJW: now upper right
#if 0
        drawStr( hdc, x + 2, y + 2, str );
#endif
	}

	if ( getFlag( GroundUnit::RAILONLY ) )
	{
		if ( getAtt( ) == getDef( ) )
			sprintf( str, "%d-R", getAtt( ) );
		else
			sprintf( str, "%d-%d-R", getAtt( ), getDef( ) );
	}
	else if ( getFlag( GroundUnit::WATERONLY ) )
	{
		if ( getAtt( ) == getDef( ) )
			sprintf( str, "%d-W", getAtt( ) );
		else
			sprintf( str, "%d-%d-W", getAtt( ), getDef( ) );
	}
	else // transport units, trucks, and HQ / pos AA:  don't print str (it's zero)
	{
		if ( getType( ) == GroundUnitType::WW2::TRANSPORT || getType( ) == GroundUnitType::WW2::APC || getType( ) == GroundUnitType::WW2::LVT )
		{
			// print text in "(+1)-10" fashion
			if ( getAtt( ) == 0 && getDef( ) == 0 )
				sprintf( str, "%d", getMaxMP( ) );
			else if ( getAtt( ) == getDef( ) )
				sprintf( str, "+%d-%d", getAtt( ), getMaxMP( ) );
			else
				sprintf( str, "+%d+%d-%d", getAtt( ), getDef( ), getMaxMP( ) );
		}
		else if ( getType( ) == GroundUnitType::WW2::TRUCK )
		{
			if ( getFlag( GroundUnit::IS_CADRE ) ) // cadre'd truck means depleted ("D")
				sprintf( str, "%dD", getMaxMP( ) );
			else
				sprintf( str, "%d", getMaxMP( ) );
		}
		else if ( getSize( ) == GroundUnit::Size::HQ || getSize( ) == GroundUnit::Size::CORPSHQ || getSize( ) == GroundUnit::Size::ARMYHQ
				|| getType( ) == GroundUnitType::WW2::POS_LT_AA || getType( ) == GroundUnitType::WW2::POS_HV_AA )
			sprintf( str, "%d", getMaxMP( ) );
		else if ( getAtt( ) == getDef( ) )
			sprintf( str, "%d-%d", getAtt( ), getMaxMP( ) );
		else
			sprintf( str, "%d-%d-%d", getAtt( ), getDef( ), getMaxMP( ) );
	}

	// str text position
	length = strLen( str );
	x_pos = x + 14 - length / 2;  // width=28 -> 14 == half way
	y_pos = y + ( draw_wheels ? 16 : 15 );

	// write the text
	drawStr( hdc, x_pos, y_pos + 3, str );

	// done preparing the image (rest of function is
	//		for supply/disruption markings only)
	image_ok_ = true;
	// TODO: but what about allow_cache_ ?

	end:
	dbuf_dc.SelectObject( wxNullBitmap );
	hdc = nullptr;
	scr_hdc->DrawBitmap( *bmpCounter, scr_x, scr_y );

	draw_supply_status:
	if ( getSupply( ) == SupplyStatus::NONE )
	{
		// which color to use in U-marker (sides reversed in reaction phase!)
		// TODO: does this work correctly for Axis P.T. during a reaction phase?
		if ( _phasing_player == SidePlayer::AXIS || _current_phase == Phase::REACTION_PHASE )
		{
			if ( getSupplyTurns( ) % 2 )
				scr_hdc->SetTextForeground( wxColour( 0, 0, 100 ) );
			else
				scr_hdc->SetTextForeground( wxColour( 255, 0, 0 ) );
		}
		else
		{
			if ( getSupplyTurns( ) % 2 )
				scr_hdc->SetTextForeground( wxColour( 255, 0, 0 ) );
			else
				scr_hdc->SetTextForeground( wxColour( 0, 0, 100 ) );
		}
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		scr_hdc->DrawRectangle( scr_x + 19, scr_y, 13, 13 );
		int t = ( ( getSupplyTurns( ) - 1 ) / 2 ) + 1;
		t = ( t > 4 ) ? 4 : t;
		sprintf( str, "U%d", t );
		drawStr( scr_hdc, scr_x + 21, scr_y + 2, str );
	}

	// if isolated then draw tiny "I" marker
	if ( getIsolated( ) )
	{
		scr_hdc->SetTextForeground( *wxBLACK );
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		scr_hdc->DrawRectangle( scr_x + 19, scr_y + 13, 13, 13 );
		sprintf( str, "I" );
		drawStr( scr_hdc, scr_x + 24, scr_y + 15, str );
	}

	// draw attack supply marker (if attack supply rule is enabled)
	if ( _rule_set.AttackSup == TRUE && getAttackSupply( ) )
	{
		scr_hdc->SetTextForeground( wxColour( 0, 150, 0 ) );
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		scr_hdc->DrawRectangle( x, y, 10, 10 );
		sprintf( str, "A" );
		scr_hdc->DrawText( wxS( str ), scr_x + 1, scr_y - 1 );
	}

	// draw disruption status symbol if disrupted or badly disrupted:
	if ( getDisruption( ) > DisruptionStatus::NOT_DISRUPTED )
	{
		scr_hdc->SetTextForeground( *wxBLACK );
		scr_hdc->SetTextBackground( *wxWHITE );
		scr_hdc->SetPen( *wxBLACK_PEN );  // border
		scr_hdc->SetBrush( *wxWHITE_BRUSH );  // white marker
		if ( getDisruption( ) == DisruptionStatus::DISRUPTED )
		{
			scr_hdc->DrawRectangle( scr_x - 1, scr_y + 5, 10, 12 );
			sprintf( str, "D" );
			drawStr( scr_hdc, scr_x + 1, scr_y + 7, str );
		}
		else // must be badly disrupted
		{
			scr_hdc->DrawRectangle( scr_x - 1, scr_y + 5, 14, 12 );
			sprintf( str, "BD" );
			drawStr( scr_hdc, scr_x, scr_y + 7, str );
		}
	}
}

bool GroundUnit::isValidUnit( void )
{
#if 0
	if ( bAtt > 30 || bDef>30 || GetFlak( ) > 12 )
		wxMessageBox( wxT( "Combat strength is suspiciously high." ), wxT( "Hmm..." ), wxOK );

	if ( GetMaxMP( ) > 10 )
		wxMessageBox( wxT( "This unit moves pretty fast." ), wxT( "Hmm..." ), wxOK );
#endif
	if ( type_ > GroundUnitType::WW2::UNITTYPECOUNT || subtype_ > GroundArmedForce::Allied::COUNT )
	{
		wxMessageBox( wxT( "Illegal type!" ), wxT( "Error" ), wxOK );
		return 0;
	}

	if ( ( getType( ) == GroundUnitType::WW2::POS_LT_AA || getType( ) == GroundUnitType::WW2::POS_HV_AA ) && getFlak( ) == 0 )
		wxMessageBox( wxT("Your positional AA has AA strength of zero!"), wxT("Hmm..."), wxOK );

	if ( getSize( ) > GroundUnit::Size::CORPS )
	{
		wxMessageBox( wxT("Illegal size!"), wxT("Error"), wxOK );
		return 0;
	}
	return 1;
}

/* supply functions */
/* 0..2 (0=regular, 1=special, 2=none) */
void GroundUnit::setSupply( int i )
{
	if ( i < 0 || i > 2 )
	{
		wxMessageBox( wxT("Bug: illegal supply status"), wxT("GroundUnit::SetSupply"), wxOK );
		return;
	}
	data1_ &= 0xfc;
	data1_ |= static_cast<unsigned char>( i & 0x03 );
}

int GroundUnit::getSupply( )
{
	return ( data1_ & 0x03 );
}

void GroundUnit::setSupplyTurns( int i )
{
	// 15 is the integral maximum for 4 bits of storage
	i = limit( i, 0, 15 );

	// 0xc3 == 1100 0011, clear 3rd-->6th bits
	data1_ &= 0xc3;

	// set bitwise integer on 3rd-->6th bits
	data1_ |= static_cast<unsigned char>( ( i & 0x0f ) << 2 );
}

int GroundUnit::getSupplyTurns( )
{
	// right-shift 3rd-->6th bits twice, and mask for integer result
	return ( ( data1_ >> 2 ) & 0xf );
}

// set supply turns based on supply status (set by SetSupply)
// NOTE: supply increment flag must be cleared when player turn ends
void GroundUnit::updateSupplyTurns( )
{
	if ( getSupply( ) == SupplyStatus::NONE )
	{
		if ( ! supplyTurnFlagIsSet( ) )
			setSupplyTurns( getSupplyTurns( ) + 1 );

		setSupplyTurnFlag( );
	}
	else // in supply
	{
		if ( supplyTurnFlagIsSet( ) )
			setSupplyTurns( getSupplyTurns( ) - 1 ); // but now in supply -> decrement

		clearSupplyTurnFlag( );
	}
}

void GroundUnit::setSupplyTurnFlag( )
{
	data1_ |= 0x80;  // 1000 0000:  set supply incremented flag
}

void GroundUnit::clearSupplyTurnFlag( )
{
	data1_ &= 0x7f;  // 0111 1111:  clear supply incremented flag
}

bool GroundUnit::supplyTurnFlagIsSet( )
{
	return data1_ & 0x80 ? true : false;
}

void GroundUnit::setAttackDir( int dir )
{
	int i;

	switch ( dir )
	{
		case Hex::WEST:
			i = 1;
			break;
		case Hex::EAST:
			i = 2;
			break;
		case Hex::NORTHWEST:
			i = 3;
			break;
		case Hex::NORTHEAST:
			i = 4;
			break;
		case Hex::SOUTHWEST:
			i = 5;
			break;
		case Hex::SOUTHEAST:
			i = 6;
			break;
		default:
			i = 0;
			break;
	}
	data3_ = static_cast<unsigned char>( ( data3_ & 0xf0 ) | i );
}

int GroundUnit::getAttackDir( void )
{
	int dir;

	int i = data3_ & 0xf;
	switch ( i )
	{
		case 1:
			dir = Hex::WEST;
			break;
		case 2:
			dir = Hex::EAST;
			break;
		case 3:
			dir = Hex::NORTHWEST;
			break;
		case 4:
			dir = Hex::NORTHEAST;
			break;
		case 5:
			dir = Hex::SOUTHWEST;
			break;
		case 6:
			dir = Hex::SOUTHEAST;
			break;
		default:
			dir = 0;
			break;
	}
	return dir;
}

// is unit self supported even if no other support is available?
bool GroundUnit::isSupported( )
{
	// cadre -> supported unless told otherwise
	if ( getSize( ) >= GroundUnit::Size::DIVISIONAL && isCadre( ) )
	{
		if ( getFlag( GroundUnit::CDR_UNSUP ) )
			return false;
		return true;
	}

	// divisions are supported unless marked as unsupported
	if ( getSize( ) >= GroundUnit::Size::DIVISIONAL && ! isUnsupported( ) )
		return true;

	// otherwise support indicator tells if unit is supported
	return getFlag( GroundUnit::SUPPORTED );
}

bool GroundUnit::isConstructionEngineer( )
{
	if ( _ground_unit_type[getType( )].flags_ & GroundUnitType::CONSTRUCTION )
		return true;
	return false;
}

bool GroundUnit::isCombatEngineer( )
{
	if ( ( _ground_unit_type[getType( )].flags_ & GroundUnitType::ENGINEER ) && ! isAssaultEngineer( ) )
		return true;
	return false;
}

bool GroundUnit::isAssaultEngineer( )
{
	if (	getType( ) == GroundUnitType::WW2::ASLT_ENG ||
			getType( ) == GroundUnitType::WW2::AMPH_ASLT_ENG ||
			getType( ) == GroundUnitType::WW2::ASLT_ENG_TANK )
		return true;
	return false;
}

bool GroundUnit::isCombatMotClass( )
{
#if 0
	if ( _ground_unit_type[getType( )].type_ & UC_COMBATMOT )
#endif
	return ( _ground_unit_type[getType( )].class_ == GroundUnitType::COMBAT_MOTORIZED_CLASS ); //& UC_COMBATMOT )
}

bool GroundUnit::isArtillery( )
{
	/* TODO: same as Type() == UC_ARTILLERY
	 * thus, should call this method IsArtilleryClass() ? */
#if 0
	if ( UnitTypes[GetType()].Flags & UTF_ARTILLERY )
#endif
	return ( _ground_unit_type[getType( )].class_ == GroundUnitType::ARTILLERY_CLASS ); //& UC_COMBATMOT )
#if 0
	if ( _ground_unit_type[getType( )].type_ == UC_ARTILLERY )
#endif
}

bool GroundUnit::isSiegeArtillery( )
{
		return true;
	return false;
}

std::istream& operator>>( std::istream& is, GroundUnit& unit )
{
	int t;
	is >> t;
	unit.counter_type_ = t;

	int fl;
	is >> fl;
	unit.flags_ = fl;

	int x;
	is >> x;
	unit.homex_ = x;

	int y;
	is >> y;
	unit.homey_ = y;

	char id[32];
	is >> id;
	line2spc( id );
	strcpy( unit.id_, id );

	int a;
	is >> a;
	unit.attack_strength_ = a;

	int b;
	is >> b;
	unit.defense_strength_ = b;

	int l;
	is >> l;
	unit.cadre_strength_ = l;

	int c;
	is >> c;
	unit.type_ = c;

	int d;
	is >> d;
	unit.subtype_ = d;

	int e;
	is >> e;
	unit.unit_flags_ = e;

	int f;
	is >> f;
	unit.data1_ = f;

	int g;
	is >> g; // starting with scn v7, more sizes (see counter.defs)
	if ( _scn_version < 7 )
	{
		int sz = g >> 4;
		g &= 0x0f;
		switch( sz ) // translate old size indices to new indices
		{
			case 2: sz = GroundUnit::Size::COMPANY; break;
			case 3: sz = GroundUnit::Size::BATALLION; break;
			case 4: sz = GroundUnit::Size::REGIMENT; break;
			case 5: sz = GroundUnit::Size::BRIGADE; break;
			case 6: sz = GroundUnit::Size::DIVISIONAL; break;
			case 7: sz = GroundUnit::Size::DIVISION; break;
			case 8: sz = GroundUnit::Size::CORPS; break;
		}
		g |= (sz << 4);
	}
	unit.data2_ = g;

	int h;
	is >> h;
	unit.data3_ = h;

	int i;
	is >> i;
	unit.data4_ = i;

	int m;
	is >> m;
	unit.data5_ = m;

	int d6 = 0; // data6 not used in scn v3 and earlier
	if ( _scn_version >= 4 )
		is >> d6;
	unit.data6_ = d6;

	int j;
	is >> j;
	unit.current_MPs_ = j;

	int k;
	is >> k;
	unit.maximum_MPs_ = k;

	std::string path;
	is >> path;
	if ( path.compare( ht::Replay::NONE ) == 0 )
		unit.clearReplay();
	else
		unit.setReplay( const_cast<char*>( path.c_str( ) ) );

	return is;
}

std::ostream& operator<<( std::ostream& os, GroundUnit& unit )
{
	os << static_cast<int>( unit.counter_type_ ) << ' ';
	os << static_cast<int>( unit.flags_ ) << ' ';
	os << static_cast<int>( unit.homex_ ) << ' ';
	os << static_cast<int>( unit.homey_ ) << ' ';
	if ( isOkStr( unit.id_ ) && strcmp( unit.id_, "???" ) != 0 )
	{
		spc2line( unit.id_ );
		os << unit.id_ << ' ';
	}
	else
		os << "_" << ' ';
	os << static_cast<int>( unit.attack_strength_ ) << ' ';
	os << static_cast<int>( unit.defense_strength_ ) << ' ';
	os << static_cast<int>( unit.cadre_strength_ ) << ' ';
	os << static_cast<int>( unit.type_ ) << ' ';
	os << static_cast<int>( unit.subtype_ ) << ' ';
	os << static_cast<int>( unit.unit_flags_ ) << ' ';
	os << static_cast<int>( unit.data1_ ) << ' ';
	os << static_cast<int>( unit.data2_ ) << ' ';
	os << static_cast<int>( unit.data3_ ) << ' ';
	os << static_cast<int>( unit.data4_ ) << ' ';
	os << static_cast<int>( unit.data5_ ) << ' ';
	os << static_cast<int>( unit.data6_ ) << ' ';
	os << static_cast<int>( unit.current_MPs_ ) << ' ';
	os << static_cast<int>( unit.maximum_MPs_ ) << ' ';
	if ( unit.replay_string_ && unit.replay_string_[0] != ht::Replay::END )
		os << unit.replay_string_ << ' ';
	else
		os << ht::Replay::NONE << ' ';
	return os;
}

int GroundUnit::operator==( GroundUnit& u )  // == overloaded
{
	if ( strcmp( id_, u.id_ ) != 0 )  // different IDs
		return FALSE;

	if ( data1_ != u.data1_ || data2_ != u.data2_ || data5_ != u.data5_ )
		return FALSE;

	if ( cadre_strength_ != u.cadre_strength_ )
		return FALSE;

	if ( attack_strength_ != u.attack_strength_ || defense_strength_ != u.defense_strength_ || type_ != u.type_ )
		return FALSE;

	if ( subtype_ != u.subtype_ || unit_flags_ != u.unit_flags_ )
		return FALSE;

	return TRUE;
}

#endif
