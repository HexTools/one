#include <wx/wx.h>

#include "hextools.h"
#include "application.h"
#include "frame.h"

#if defined HEXTOOLSPLAY
#include "gameunitset.h"
#include "rulesvariant.h"
#include "sideplayer.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "rules.h"
#include "zoc.h"
#include "dasmode.h"
#include "stackingmode.h"
#endif

const std::string Application::VERSION( "v2.3.0 BETA [09Jan2014]" );
const std::string Application::AUTHOR( "HexTools Development Group" );
const std::string Application::FOLDER( "HEXTOOLS" );
const std::string Application::HEX_BITMAPS_PATH( "graphics/hexes/" );
const std::string Application::HEXSIDE_BITMAPS_PATH( "graphics/hexsides/" );
const std::string Application::SYMBOL_BITMAPS_PATH( "graphics/symbols/" );

#if defined HEXTOOLSMAP
const std::string Application::NAME( "HexTools Map" );
#elif defined HEXTOOLSPLAY
const std::string Application::NAME( "HexTools Play" );
const std::string Application::CRT_FILENAME( "ge.gcrt" );
const std::string Application::TEC_FILENAME( "ge.tec" );
const std::string Application::UIC1_FILENAME( "ww1.uic" );
const std::string Application::UIC2_FILENAME( "ww2.uic" );
const std::string Application::UCC1_FILENAME( "ww1.ucc" );
const std::string Application::UCC2_FILENAME( "ww2.ucc" );
#endif

#if defined HEXTOOLSPLAY
extern Rules _rule_set;
extern int _teleport_mode_on;					// click to teleport selected units?
extern int _unit_editor_active;			// kludge: set this when dialog box is visible so keypresses don't cause trouble

BEGIN_EVENT_TABLE(Application, wxApp)
EVT_KEY_DOWN(Application::OnKey)
END_EVENT_TABLE()
#endif

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type
// (i.e. Application, not wxApp)

IMPLEMENT_APP( Application )

bool Application::OnInit( )
{
	// to load graphics from correct place, go to APP_DIR (if specified)
	char* path = getenv( Application::FOLDER.c_str( ) );
	if ( path )
		wxSetWorkingDirectory( ht::wxS( path ) );

	// create the main application window (size set in Frame ctor)
	wxString title( Application::NAME + " " + Application::VERSION );
	frame_ = new Frame( title, wxDefaultPosition, wxSize( ) );

	// Show it and tell the application that it's our main window
	frame_->Show( );
	SetTopWindow( frame_ );

	//frame->Maximize( );

#if defined HEXTOOLSPLAY
	// initialize random number generator
#if defined(__WXMSW__) && ! defined(__MINGW32__)
	randomize( );
#else
	srand( time( 0 ) );
#endif
	InitRules( );
#endif

	// success: wxApp::OnRun() will be called which will enter the
	// main message loop and the application will run. If we returned
	// 'false' here, the application would exit immediately.
	return true;
}

#if defined HEXTOOLSPLAY
void Application::OnKey( wxKeyEvent& event )
{
	wxCommandEvent e;

	if ( _unit_editor_active )
	{
		event.Skip( );
		return;
	}

	// ASCII characters
	if ( event.m_controlDown )
	{
		switch ( event.m_keyCode )
		{
			case 'T':
				_teleport_mode_on = ( _teleport_mode_on == TRUE ? FALSE : TRUE );
				frame_->toolbar_->ToggleTool( Frame::MenuEvent::TELEPORT, _teleport_mode_on );
				break;
			case 'O':
				frame_->CmChangeOwner( e );
				break;
			case 'N':
				frame_->CmMakeNeutral( e );
				break;
			case 'S':
				frame_->CmChgsupply( e );
				break;
			case 'I':
				frame_->CmIsolated( e );
				break;
			case 'F':
				frame_->CmSearchUnit( e );
				break;
			default:
				event.Skip( );
				break;
		}
	}
	else
	{
		// special keys
		switch ( event.m_keyCode )
		{
			case 'T':
				_teleport_mode_on = ( _teleport_mode_on == TRUE ? FALSE : TRUE );
				frame_->toolbar_->ToggleTool( Frame::MenuEvent::TELEPORT, _teleport_mode_on );
				break;
			case 'O':
				frame_->CmChangeOwner( e );
				break;
			case 'L':
				frame_->CmChangeOrigOwner( e );
				break;
			case 'N':
				frame_->CmMakeNeutral( e );
				break;
			case 'S':
				frame_->CmChgsupply( e );
				break;
			case 'I':
				frame_->CmIsolated( e );
				break;
			case WXK_NUMPAD1:
				frame_->CmUnitSouthWest( e );
				break;
			case WXK_NUMPAD2:
				break;
			case WXK_NUMPAD3:
				frame_->CmUnitSouthEast( e );
				break;
			case WXK_NUMPAD4:
				frame_->CmUnitWest( e );
				break;
			case WXK_NUMPAD5:
				frame_->CmUnitHome( e );
				break;
			case WXK_NUMPAD6:
				frame_->CmUnitEast( e );
				break;
			case WXK_NUMPAD7:
				frame_->CmUnitNorthWest( e );
				break;
			case WXK_NUMPAD8:
				break;
			case WXK_NUMPAD9:
				frame_->CmUnitNorthEast( e );
				break;
			case WXK_F3:
				frame_->CmSearchAgain( e );
				break;
			case WXK_LEFT:
				frame_->map_->MoveSelection( 4 );
				break;
			case WXK_RIGHT:
				frame_->map_->MoveSelection( 6 );
				break;
			case WXK_UP:
				frame_->map_->MoveSelection( 8 );
				break;
			case WXK_DOWN:
				frame_->map_->MoveSelection( 2 );
				break;
			case WXK_ESCAPE:
				frame_->map_->cancel( );
				break;
			default:
				event.Skip( );
				break;
		}
	}
}

void Application::InitRules( )
{
	// game series rules //
#if 0
	_rule_set.rules_variant_ = RulesVariant::WW2;  // 0 = WW2, 1 = WW1, 2 = WW2 Variant A
#endif
	_rule_set.Era = GameEra::WW2;
	_rule_set.WW1MPs = FALSE;
	_rule_set.WW1Phases = FALSE;
	_rule_set.WW1AirBaseCapacity = FALSE;
	_rule_set.WW1ExploitMPs = FALSE;
	_rule_set.RailsObstructed = FALSE;

	// general rules //
	_rule_set.FirstPlayer = SidePlayer::AXIS;
	// Frame::Turns default setting?
#if 0
	_rule_set.UnitTypeFlags = GroundUnitType::WW2;
#endif

	// RE-related rules //
	_rule_set.AllowCustomREs = FALSE;
	_rule_set.HideAlPartisans = TRUE;
	_rule_set.HideAxPartisans = TRUE;
	_rule_set.HQRE2 = 2;
	_rule_set.CompanyRE2 = 1;
	_rule_set.BattalionRE2 = 1;
	_rule_set.RegimentRE2 = 2;
	_rule_set.BrigadeRE2 = 2;
	_rule_set.CadreRE2 = 2;
	_rule_set.DivGrpRE2 = 6;
	_rule_set.DivisionRE2 = 6;
	_rule_set.CorpsRE2 = 6;
	_rule_set.ArmyRE2 = 12;

	// ZOC-related rules //
	_rule_set.AllowCustomZOCs = FALSE;
	_rule_set.BattalionZOC = ZOC::NONE;
	_rule_set.BattalionU2ZOC = ZOC::NONE;
	_rule_set.RegimentZOC = ZOC::NONE;
	_rule_set.RegimentU2ZOC = ZOC::NONE;
	_rule_set.BrigadeZOC = ZOC::NONE;
	_rule_set.BrigadeU2ZOC = ZOC::NONE;
	_rule_set.CadreZOC = ZOC::NONE;
	_rule_set.CadreU2ZOC = ZOC::NONE;
	_rule_set.DivGrpZOC = ZOC::NORMAL;
	_rule_set.DivGrpU2ZOC = ZOC::REDUCED;
	_rule_set.DivisionZOC = ZOC::NORMAL;
	_rule_set.DivisionU2ZOC = ZOC::REDUCED;
	_rule_set.CorpsZOC = ZOC::NORMAL;
	_rule_set.CorpsU2ZOC = ZOC::REDUCED;

	// movement rules //
	_rule_set.DisableMPCalc = FALSE;
	_rule_set.OnlyClearAdmin = FALSE;
	_rule_set.ToTrain1MP = FALSE;
	_rule_set.ReducedZOCs = FALSE;
	_rule_set.SovietReducedZOCs = FALSE;
	_rule_set.NonPhasMove = FALSE;
	_rule_set.NoGaugeChk = TRUE;
	_rule_set.MP6Swamp = TRUE;
	_rule_set.AllowUxAdminMove = FALSE;
	_rule_set.FortLevelMP = 5;
	_rule_set.PermAFMP = 3;
	_rule_set.TempAFMP = 6;
	_rule_set.ZOCLeaveCost = 2;
	_rule_set.ZOC2ZOCCost = 3;
	_rule_set.RZOCLeaveCost = 0;
	_rule_set.RZOC2ZOCCost = 1;
	_rule_set.GermanCMZOCRule = TRUE;
	_rule_set.ZOCEnterMot = 0;  // only FtF(?)
	_rule_set.ZOCEnterNormal = 0;  // only FtF(?)
	_rule_set.NoAlliedZOCAcrossBorders = FALSE;
	_rule_set.GermanRegaugeInExploitation = FALSE;
	_rule_set.SovietReactionPhase = TRUE;
	_rule_set.SovietExploitLimitation = FALSE;
	_rule_set.RoadsCanBeBroken = FALSE;
	_rule_set.PavedRoads = FALSE;
	_rule_set.TEM40Overruns = FALSE;

	// combat rules //
	_rule_set.AlliedCombatSide = 0; //ACS_OTHER;  // 0 = Other, 1 = British, 2 = French, 3 = Vichy French, 4 = Soviet, 5 = USA, 6 = Greeks
	_rule_set.ExtremeCrt = FALSE;
	_rule_set.AutoDR = TRUE;
	_rule_set.AutoDH = TRUE;
	_rule_set.AutoDE = TRUE;
	_rule_set.OnlyIsolU1Halved = TRUE;
	_rule_set.WestCityMod = TRUE;
	_rule_set.NewSwamp = TRUE;
	_rule_set.AttackSup = FALSE;  // only WitD and FWtBT(?)
	_rule_set.DASMode = DASMode::HALF_STR;
	_rule_set.GSHalved = TRUE;   // halved in some hextypes (swamp etc.)
	_rule_set.AxisHalfAT = FALSE;  // only FitE/SE
	_rule_set.WeatherDieMod = TRUE;
	_rule_set.SnowReducedAEC = TRUE;   // allow reduced AEC (otherwise none)
	_rule_set.FreezingAllowed = TRUE;
	_rule_set.IgnoreFrozenLakeHexsides = TRUE;
	_rule_set.OneTenthAEC = FALSE;
	_rule_set.EngProportion = 6;
	_rule_set.WW1ModernArtTactics = TRUE;
	_rule_set.BorderRiversIgnored = FALSE;
	_rule_set.FortifiedAreaLevel = 3;

	// air rules //
	_rule_set.OnDemandAirMissions = TRUE;
	_rule_set.NewAFCapacitySystem = TRUE;
	_rule_set.EndAirCombatAtFirstResult = TRUE;
	_rule_set.HFIsF = FALSE;
	_rule_set.AbortedPlanesToPool = TRUE;
	_rule_set.NewAAMods = TRUE;
	_rule_set.StageRange = 300;  // %
	_rule_set.TmpAFCap = 3;
	_rule_set.TmpAFBadWeatherCap = 2;
	_rule_set.JettisonEsc = FALSE;
	_rule_set.BombingFBothReduced = FALSE;  // only attack str is reduced by 2
	_rule_set.AlliedFullCityAA = 0;
	_rule_set.AlliedPartialCityAA = 0;
	_rule_set.AlliedDotCityAA = 0;
	_rule_set.AlliedReferenceCityAA = 0;
	_rule_set.AlliedUnImprovedFortressAA = 0;
	_rule_set.AlliedImprovedFortressAA = 0;
	_rule_set.AlliedAirfieldAA = 0;
	_rule_set.AxisFullCityAA = 0;
	_rule_set.AxisPartialCityAA = 0;
	_rule_set.AxisDotCityAA = 0;
	_rule_set.AxisReferenceCityAA = 0;
	_rule_set.AxisUnImprovedFortressAA = 0;
	_rule_set.AxisImprovedFortressAA = 0;
	_rule_set.AxisAirfieldAA = 0;

	// naval rules //
	_rule_set.OldNavalSystem = FALSE;
	_rule_set.SeparateNavalPhases = FALSE;
	_rule_set.EnableCDCalc = TRUE;

	// stacking rules //
	_rule_set.StackNDivType = StackingMode::NONDIV_RE;
	_rule_set.StackDivs = 3;		// normal
	_rule_set.StackNDivs = 3;
	_rule_set.StackArt = 2;
	_rule_set.StackMDivs = 2;		// mountain
	_rule_set.StackMNDivs = 2;
	_rule_set.StackMArt = 1;
	_rule_set.StackADivs = 1;		// Arctic
	_rule_set.StackANDivs = 1;
	_rule_set.StackAArt = 1;
	_rule_set.StackWW1AllowOneFort = FALSE;
}
#endif
