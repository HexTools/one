#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/hashmap.h>

#if defined __WXGTK__
#include <unistd.h>
#endif

#if defined HEXTOOLSPLAY
#include "wx/filename.h"
#include <wx/listctrl.h>
#include <wx/config.h>
#include <wx/textfile.h>
#endif

// common to both HT-m and HT-p
#include "application.h"
#include "hextools.h"
#include "frame.h"
#include "hexinfopane.h"				// hex info box (top left)

#if defined HEXTOOLSPLAY
#include "counter.h"
#include "facility.h"
#include "hexnote.h"
using ht::wxS;
using ht::basename;
using ht::pathname;
using ht::dice;
using ht::logDebugString;
#endif

// common to both HT-m and HT-p
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"					// map area (right)

#if defined HEXTOOLSMAP
#include "terrainselector.h"			// radiobuttons (left)
#endif

#if defined HEXTOOLSPLAY
#include "hexcontentspane.h"			// in-hex counter area (left)
#include "gameunitset.h"
#include "rulesvariant.h"
#include "rules.h"
#include "sideplayer.h"
#include "alliedcoalition.h"
#include "phase.h"
#include "zoc.h"
#include "dasmode.h"
#include "stackingmode.h"
#include "movementmode.h"
#include "disruptionstatus.h"
#include "dnewgame.h"
#include "dtimejump.h"
#include "dweather.h"
#include "dabout.h"
#include "ddisplaysettings.h"
#include "dendplayerturn.h"
#include "drules.h"
#include "dsplash.h"
#include "dinsertscenario.h"
#endif

#if defined HEXTOOLSMAP
#define HELP_CMD "maphelp.bat"
#elif defined HEXTOOLSPLAY
#define HELP_CMD "runhelp.bat"
#endif

const std::string Frame::CLOSE_WO_SAVE( "All the changes you have made will be lost, really exit?" );
#if defined HEXTOOLSMAP
const std::string Frame::FILE_TYPES( "HexTools Map (*.map)|*.map|All files (*.*)|*.*" );
#elif defined HEXTOOLSPLAY
const std::string Frame::FILE_TYPES( "HexTools Scenario (*.scn)|*.scn|All files (*.*)|*.*" );
#endif

// these are defined differently in globaldata.cpp, for MAP/ and PLAY/
extern const int _side_width;		// side bar width
extern const int _infopane_height;	// info bar (top-left) height

// HexTools window default size
extern int _frame_width;	// = 900;
extern int _frame_height; 	// = 700;

extern const wxString _land_district[][2];
extern const wxString _sea_circle[][2];

#if defined HEXTOOLSMAP
extern const char* _weather_zone_string[];
#elif defined HEXTOOLSPLAY

extern const wxString _land_country[][2];
extern const wxString _sea_zone[][2];

extern wxTextFile _htlog;

extern wxBitmap* stagebmp;
extern wxBitmap* hitrailbmp;
extern wxBitmap* hitafbmp;
extern wxBitmap* repairbmp;
extern wxBitmap* regaugebmp;
extern wxBitmap* totrainbmp;
extern wxBitmap* fromtrainbmp;
extern wxBitmap* bombbmp;

extern NewGameDlgXfer _new_game_dialog_data;
extern TurnDlgXfer _turn_dialog_data;
extern DisplayOptionsDlgXfer _display_options_dialog_data;

extern const char* _player_string[];
extern const char* _month_string[];		// = 12
extern const char* _turn_numeral[];		// = 20
extern const char* _phase_string[];		// = LASTPHASE + 1

extern int _armed_force_filter;				// = GAME_ALL;

extern int _show_combat_markers;		// show combat markers in the map?
extern int _show_path;					// = TRUE;
extern int _show_splash;				// = TRUE;
extern int _transparent_text;			// = FALSE;  // this in not currently adjustable (but works)
extern int _gray_out_hexes;				// = TRUE;  // gray unreachable hexes active
extern int _use_point_and_click_move;				// = TRUE;  // point'n'click moving active
extern int _backup_on_save;				// = TRUE;  // make backup copies when saving scenarios
extern int _tile_set;					// = 0;  // map graphics tiles, 0=standard

extern Rules _rule_set;

extern wxString _recent_file1;
extern wxString _recent_file2;
extern wxString _recent_file3;
extern wxString _recent_file4;
extern wxString _recent_file5;

// globals
extern int _current_player;			// SidePlayer::AXIS or ALLIED
extern int _current_phase;			// COMBAT_PHASE etc.
extern int _previous_phase;			// Phase before setup-mode
extern int _year;					// 42 etc...
extern int _month;					// 0...11  (0=Jan, 11=Dec, used as on index)
extern int _turn;					// 1 or 2 (I or II)
extern int _turns_per_month;		// turns per month

extern int _phasing_player;			// who is _phasing_ player (Player may be non-phasing too)
extern int _quick_scroll_enabled;	// = TRUE;
extern int _quick_scroll_mirrored;	// = TRUE;
extern int _quick_scroll_velocity;	// = 4;
extern bool _show_weather;	// = true; // weather aware terrain drawing
extern int _cache_axis_attack_strengths;	// = TRUE;
extern int _cache_allied_attack_strengths;	// = TRUE;

extern int _show_MPs_on_map;			// = FALSE;
#if 0
extern int _show_flagpoles_on_map;		// = FALSE;  // FALSE: counters on map, TRUE: flagpoles on map
#endif
extern int _show_corps_markers_on_map;	// = FALSE;

extern int _teleport_mode_on;			// = 0;  // click to teleport selected units?
extern int _show_units;					// = 0;  // hide units from map (in order to see geographics better)
extern bool _unit_editor_active;		// = false;  // king of all kludges: set this when dialog box is visible so keypresses don't cause trouble!

// stuff from mappane
extern wxBitmap* hbmAllied;
extern wxBitmap* hbmAlliedSov;
extern wxBitmap* hbmAlliedUS;
extern wxBitmap* hbmAlliedBr;

// TODO: MapPane has a member MoveMode, refactor or at least rename to
//	clearly discriminate between frame.cpp's static Movemode and MapPane's member Movemode
// code formatter messes this up without the forced space:
static /* */ int MoveMode = MovementMode::TACTICAL;

#if 0
extern const char* crt_odds_str[];	//= 13
extern int Weather[];		// [W_ZONES];  // W_MUD etc., per zone
extern int Frozen[];		// [W_ZONES];  // true if rivers/lakes are frozen
int wturns[W_ZONES]; 		// for freezing calculations
int NavalPhase;					// if separate navalphases, this is current naval phase
int FirstPlayer;					// who moves first
int FlatButtons = TRUE;   		// flat toolbar buttons?
#endif

#endif

// IDs for the controls and the menu commands
enum
{
#if defined HEXTOOLSMAP
	CMD_QUIT 				= 1, // (MAP)
	CMD_OPEN,
	CMD_SAVE,
	CMD_SAVEAS,
	CMD_EXPORT,
#endif
	CMD_REPAINT,			// 0 (PLAY), 6 (MAP)
#if defined HEXTOOLSMAP
	CMD_ZOOMIN,
	CMD_ZOOMOUT,
	CMD_ZOOM100,
	// mode selectors:
	CMD_WZMODE,
	CMD_WZCHOICE,			// 11
	CMD_SEABOXMODE,
	CMD_SEABOXCHOICE,		// 13
	CMD_DISTRICTMODE,
	CMD_DISTRICTCHOICE,		// 15
	CMD_MAPLEAFMODE,
	CMD_ENDMODES,
	// help menu:
	CMD_ABOUT,
	CMD_HELP				// 19
#elif defined HEXTOOLSPLAY
	CMD_SEAZONEMODE,		// 1 (PLAY)
	CMD_SEAZONECHOICE,		// 2
	CMD_COUNTRYMODE,		// 3
	CMD_COUNTRYCHOICE		// 4
#endif
};

#if defined HEXTOOLSMAP
// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler
DECLARE_EVENT_TYPE(wxEVT_UNSELECT_TERRAINTYPE, -1)
DEFINE_EVENT_TYPE(wxEVT_UNSELECT_TERRAINTYPE)
#endif

BEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_CLOSE(Frame::OnCloseQuery)

#if defined HEXTOOLSMAP
EVT_MENU(CMD_QUIT, Frame::OnQuit)
EVT_MENU(CMD_OPEN, Frame::OnOpen)
EVT_MENU(CMD_SAVE, Frame::OnSave)
EVT_MENU(CMD_SAVEAS, Frame::OnSaveAs)
EVT_MENU(CMD_EXPORT, Frame::OnExport)
EVT_MENU(CMD_REPAINT, Frame::OnRepaint)
EVT_MENU(CMD_ZOOMIN, Frame::OnZoomIn)
EVT_MENU(CMD_ZOOMOUT, Frame::OnZoomOut)
EVT_MENU(CMD_ZOOM100, Frame::OnZoom100)
EVT_MENU(CMD_ABOUT, Frame::OnAbout)
EVT_MENU(CMD_HELP, Frame::OnHelp)

EVT_BUTTON(CMD_WZMODE, Frame::OnWZMode)
EVT_CHOICE(CMD_WZCHOICE, Frame::OnWZSelection)
EVT_BUTTON(CMD_SEABOXMODE, Frame::OnSeaboxMode)
EVT_CHOICE(CMD_SEABOXCHOICE, Frame::OnSeaboxSelection)
EVT_BUTTON(CMD_DISTRICTMODE, Frame::OnDistrictMode)
EVT_CHOICE(CMD_DISTRICTCHOICE, Frame::OnDistrictSelection)
EVT_BUTTON(CMD_MAPLEAFMODE, Frame::OnMapLeafMode)
EVT_BUTTON(CMD_ENDMODES, Frame::OnEndModes)
#elif defined HEXTOOLSPLAY

EVT_BUTTON(	CMD_SEAZONEMODE,	Frame::OnSeazoneMode )
EVT_CHOICE(	CMD_SEAZONECHOICE,	Frame::OnSeazoneSelection )
EVT_BUTTON(	CMD_COUNTRYMODE,	Frame::OnCountryMode )
EVT_CHOICE(	CMD_COUNTRYCHOICE,	Frame::OnCountrySelection )
EVT_MENU(	CMD_REPAINT,		Frame::OnRepaint )

EVT_MENU( Frame::MenuEvent::EXIT,				Frame::OnQuit )
EVT_MENU( Frame::MenuEvent::FILENEW,			Frame::OnNew )
EVT_MENU( Frame::MenuEvent::FILEOPEN,			Frame::OnOpen )
EVT_MENU( Frame::MenuEvent::FILEINSERT,			Frame::OnInsert )
EVT_MENU( Frame::MenuEvent::FILESAVE,			Frame::OnSave )
EVT_MENU( Frame::MenuEvent::FILESAVEAS,			Frame::OnSaveAs )
EVT_MENU( Frame::MenuEvent::RECENT1,			Frame::OnRecent1 )
EVT_MENU( Frame::MenuEvent::RECENT2,			Frame::OnRecent2 )
EVT_MENU( Frame::MenuEvent::RECENT3,			Frame::OnRecent3 )
EVT_MENU( Frame::MenuEvent::RECENT4,			Frame::OnRecent4 )
EVT_MENU( Frame::MenuEvent::RECENT5,			Frame::OnRecent5 )

EVT_MENU( Frame::MenuEvent::ZOOMIN,				Frame::OnZoomIn )
EVT_MENU( Frame::MenuEvent::ZOOMOUT,			Frame::OnZoomOut )
EVT_MENU( Frame::MenuEvent::ZOOM100,			Frame::OnZoom100 )
EVT_MENU( Frame::MenuEvent::OPTIONSDISPLAY,		Frame::CmSettings )
EVT_MENU( Frame::MenuEvent::OPTIONSRULES,		Frame::CmOptionsrules )
EVT_MENU( Frame::MenuEvent::OPTIONSMAPINFO,		Frame::CmOptionsInfo )
EVT_MENU( Frame::MenuEvent::UNITEAST,			Frame::CmUnitEast )
EVT_MENU( Frame::MenuEvent::UNITNORTHEAST,		Frame::CmUnitNorthEast )
EVT_MENU( Frame::MenuEvent::UNITNORTHWEST,		Frame::CmUnitNorthWest )
EVT_MENU( Frame::MenuEvent::UNITSOUTHEAST,		Frame::CmUnitSouthEast )
EVT_MENU( Frame::MenuEvent::UNITSOUTHWEST,		Frame::CmUnitSouthWest )
EVT_MENU( Frame::MenuEvent::UNITWEST,			Frame::CmUnitWest )
EVT_MENU( Frame::MenuEvent::UNITVIEW,			Frame::CmUnitView )
EVT_MENU( Frame::MenuEvent::UNITHOME,			Frame::CmUnitHome )
EVT_MENU( Frame::MenuEvent::HEXCREATEUNIT,		Frame::CmHexCreateUnit )
EVT_MENU( Frame::MenuEvent::HEXDELETEUNIT,		Frame::CmHexDeleteUnit )
EVT_MENU( Frame::MenuEvent::MOVETOREPLPOOL,		Frame::CmMoveToReplPool )
EVT_MENU( Frame::MenuEvent::HEXUNITMOVER,		Frame::CmHexUnitMover )
EVT_MENU( Frame::MenuEvent::OPTIONSREPAINT,		Frame::CmRepaint )
EVT_MENU( Frame::MenuEvent::OPTIONSWEATHER,		Frame::CmWeather )
EVT_MENU( Frame::MenuEvent::HEXEDIT_UNIT,		Frame::CmEditUnit )
EVT_MENU( Frame::MenuEvent::HEXCREATEAIRUNIT,	Frame::CmCreateAirUnit )
EVT_MENU( Frame::MenuEvent::SCROLLUP,			Frame::CmScrollUp )
EVT_MENU( Frame::MenuEvent::SCROLLRIGHT,		Frame::CmScrollRight )
EVT_MENU( Frame::MenuEvent::SCROLLLEFT,			Frame::CmScrollLeft )
EVT_MENU( Frame::MenuEvent::SCROLLDOWN,			Frame::CmScrollDown )
EVT_MENU( Frame::MenuEvent::HEXCREATEFORT,		Frame::CmCreateFort )
EVT_MENU( Frame::MenuEvent::MOVEDOWN,			Frame::CmMoveDown )
EVT_MENU( Frame::MenuEvent::MOVELEFT,			Frame::CmMoveLeft )
EVT_MENU( Frame::MenuEvent::MOVERIGHT,			Frame::CmMoveRight )
EVT_MENU( Frame::MenuEvent::MOVEUP,				Frame::CmMoveUp )
EVT_MENU( Frame::MenuEvent::HEXCREATEPERMAF,	Frame::CmPermAF )
EVT_MENU( Frame::MenuEvent::HEXCREATETEMPAF,	Frame::CmTempAF )
EVT_MENU( Frame::MenuEvent::ANALYZE_DIE,		Frame::CmAnalyzeDie )
EVT_MENU( Frame::MenuEvent::HEXINFO,			Frame::CmHexInfo )
EVT_MENU( Frame::MenuEvent::HEXREGAUGE,			Frame::CmRegauge )
EVT_MENU( Frame::MenuEvent::HEXCOMBATINFO,		Frame::CmHexCombatInfo )
EVT_MENU( Frame::MenuEvent::COMMANDSCHANGEOWNER,Frame::CmChangeOwner )
EVT_MENU( Frame::MenuEvent::COMMANDSSELECTALL,	Frame::CmSelectAll )
EVT_MENU( Frame::MenuEvent::COMMANDSDESELECTALL,Frame::CmDeselectAll )
EVT_MENU( Frame::MenuEvent::MAKENEUTRAL,		Frame::CmMakeNeutral )
EVT_MENU( Frame::MenuEvent::HEXHITRAIL,			Frame::CmHitRail )
EVT_MENU( Frame::MenuEvent::HEXHITAF,			Frame::CmHitAF )
EVT_MENU( Frame::MenuEvent::HEXCREATESHIP,		Frame::CmCreateShip )
EVT_MENU( Frame::MenuEvent::UNITFROMTRAIN,		Frame::CmFromTrain )
EVT_MENU( Frame::MenuEvent::UNITTOTRAIN,		Frame::CmToTrain )
EVT_MENU( Frame::MenuEvent::REPAIR,				Frame::CmRepair )
EVT_MENU( Frame::MenuEvent::FIELDWORKS,			Frame::CmFieldworks )
EVT_MENU( Frame::MenuEvent::RP,					Frame::CmRP )
EVT_MENU( Frame::MenuEvent::POSAA,				Frame::CmPosAA )
EVT_MENU( Frame::MenuEvent::BOMB,				Frame::CmBomb )
EVT_MENU( Frame::MenuEvent::LAND,				Frame::CmLand )
EVT_MENU( Frame::MenuEvent::TAKEOFF,			Frame::CmTakeoff )
EVT_MENU( Frame::MenuEvent::COMBAT,				Frame::CmCombat )
EVT_MENU( Frame::MenuEvent::UNITREGAUGERAIL,	Frame::CmUnitregaugerail )
EVT_MENU( Frame::MenuEvent::CANCELATTACK,		Frame::CmCancelattack )
EVT_MENU( Frame::MenuEvent::CANCELAIR,			Frame::CmCancelair )
EVT_MENU( Frame::MenuEvent::RESOLVEBOMBING,		Frame::CmResolvebombing )
EVT_MENU( Frame::MenuEvent::FIREAA,				Frame::CmFireaa )
EVT_MENU( Frame::MenuEvent::STAGE,				Frame::CmStage )
EVT_MENU( Frame::MenuEvent::RESOLVEAIRCOMBAT,	Frame::CmResolveaircombat )
EVT_MENU( Frame::MenuEvent::RESTOREAFCAP,		Frame::CmRestoreafcap )
EVT_MENU( Frame::MenuEvent::ADVANCE,			Frame::CmAdvance )
EVT_MENU( Frame::MenuEvent::DESTROYUNIT,		Frame::CmDestroyunit )
EVT_MENU( Frame::MenuEvent::REPLPOOL,			Frame::CmReplpool )
EVT_MENU( Frame::MenuEvent::CAP,				Frame::CmCap )
EVT_MENU( Frame::MenuEvent::NAVALPATROL,		Frame::CmNavalpatrol )
EVT_MENU( Frame::MenuEvent::ACTIVATEAIR,		Frame::CmActivateair )
EVT_MENU( Frame::MenuEvent::SETAFHITS,			Frame::CmSetafhits )
EVT_MENU( Frame::MenuEvent::SETAFUSAGE,			Frame::CmSetafusage )
EVT_MENU( Frame::MenuEvent::SETHARASSMENT,		Frame::CmSetharassment )
#if 0
EVT_MENU( Frame::MenuEvent::SETAXMINES, Frame::CmSetmines )
#endif
EVT_MENU( Frame::MenuEvent::SETPORTHITS,		Frame::CmSetporthits )
EVT_MENU( Frame::MenuEvent::SETPORTUSAGE,		Frame::CmSetportusage )
EVT_MENU( Frame::MenuEvent::DESTROYPORT,		Frame::CmDestroyport )
EVT_MENU( Frame::MenuEvent::REPAIRPORT,			Frame::CmRepairport )
EVT_MENU( Frame::MenuEvent::SETCDHITS,			Frame::CmSetcdhits )
EVT_MENU( Frame::MenuEvent::CHGSUPPLY,			Frame::CmChgsupply )
EVT_MENU( Frame::MenuEvent::ADDCONTESTED,		Frame::CmAddcontested )
EVT_MENU( Frame::MenuEvent::ADDBARRAGE,			Frame::CmAddBarrage )
EVT_MENU( Frame::MenuEvent::UPDATESUPPLY,		Frame::CmUpdatesupply )
EVT_MENU( Frame::MenuEvent::ISOLATED,			Frame::CmIsolated )
EVT_MENU( Frame::MenuEvent::SPECIALSUPPLY,		Frame::CmSpecialsupply )
EVT_MENU( Frame::MenuEvent::CHGISOLATED,		Frame::CmChgisolated )
EVT_MENU( Frame::MenuEvent::CANCELORDER,		Frame::CmCancelorder )
EVT_MENU( Frame::MenuEvent::ADDNOTE,			Frame::CmAddnote )
EVT_MENU( Frame::MenuEvent::ADDALERT,			Frame::CmAddalert )
EVT_MENU( Frame::MenuEvent::DELNOTE,			Frame::CmDelnote )
EVT_MENU( Frame::MenuEvent::ADDSTATUSMARKER,	Frame::CmAddstatusmarker )
EVT_MENU( Frame::MenuEvent::CALCALLIED,			Frame::CmCalcallied )
EVT_MENU( Frame::MenuEvent::CALCAXIS,			Frame::CmCalcaxis )
EVT_MENU( Frame::MenuEvent::AXISLOSS,			Frame::CmAxisloss )
EVT_MENU( Frame::MenuEvent::SHOWATTACKMARKERS,	Frame::CmShowattackmarkers )
EVT_MENU( Frame::MenuEvent::BREAKBRIDGE,		Frame::CmBreakbridge )
EVT_MENU( Frame::MenuEvent::REPAIRBRIDGE,		Frame::CmRepairbridge )
EVT_MENU( Frame::MenuEvent::AIRREPLPOOL,		Frame::CmAirreplpool )
EVT_MENU( Frame::MenuEvent::STACKINFO,			Frame::CmStackinfo )
#if 0
EVT_MENU( Frame::MenuEvent::SETATTSUPPORT, Frame::CmSetsupport )
EVT_MENU( Frame::MenuEvent::SETDEFSUPPORT, Frame::CmSetdefsupport )
#endif
EVT_MENU( Frame::MenuEvent::FROMSHIP,			Frame::CmFromship )
EVT_MENU( Frame::MenuEvent::TOSHIP,				Frame::CmToship )
EVT_MENU( Frame::MenuEvent::MPMINUS, 			Frame::CmMpminus )
EVT_MENU( Frame::MenuEvent::MPPLUS,				Frame::CmMpplus )
#if 0
EVT_MENU( Frame::MenuEvent::LAYMINE, Frame::CmLaymine )
EVT_MENU( Frame::MenuEvent::SWEEPMINE, Frame::CmSweepmine )
EVT_MENU( Frame::MenuEvent::SETALMINES, Frame::CmSetalmines )
#endif
EVT_MENU( Frame::MenuEvent::MPMINUS30,			Frame::CmMpminus30 )
EVT_MENU( Frame::MenuEvent::TOOVERSTACK,		Frame::CmTooverstack )
EVT_MENU( Frame::MenuEvent::FROVERSTACK,		Frame::CmFroverstack )
EVT_MENU( Frame::MenuEvent::TOGGLE_DID_PA,		Frame::CmToggleDidPA )
EVT_MENU( Frame::MenuEvent::SETRE,				Frame::CmSetre )
EVT_MENU( Frame::MenuEvent::SHOWCRT,			Frame::CmShowcrt )
EVT_MENU( Frame::MenuEvent::DIEROLL,			Frame::CmDieroll )
EVT_MENU( Frame::MenuEvent::DIEROLL2,			Frame::CmDieroll2 )
EVT_MENU( Frame::MenuEvent::DIEROLL12,			Frame::CmDieroll12 )
EVT_MENU( Frame::MenuEvent::DIEROLL100,			Frame::CmDieroll100 )
EVT_MENU( Frame::MenuEvent::HILITEARMOR,		Frame::CmHilitearmor )
EVT_MENU( Frame::MenuEvent::HILITEARTILLERY,	Frame::CmHiliteartillery )
EVT_MENU( Frame::MenuEvent::HILITECADRES,		Frame::CmHilitecadres )
EVT_MENU( Frame::MenuEvent::HILITEUNMOVED,		Frame::CmHiliteunmoved )
EVT_MENU( Frame::MenuEvent::HILITEENEMYZOC,		Frame::CmHiliteEnemyZOCs )
EVT_MENU( Frame::MenuEvent::HILITEFULLENEMYZOC,	Frame::CmHiliteFullEnemyZOCs )
EVT_MENU( Frame::MenuEvent::HILITEREDUCEDENEMYZOC,Frame::CmHiliteReducedEnemyZOCs )
EVT_MENU( Frame::MenuEvent::HILITEFLYINGPLANES,	Frame::CmHiliteflyingplanes )
EVT_MENU( Frame::MenuEvent::HILITEPLANES,		Frame::CmHiliteplanes )
EVT_MENU( Frame::MenuEvent::HILITENORMAL,		Frame::CmHiliteNormalGaugeRail )
EVT_MENU( Frame::MenuEvent::HILITEWIDE,			Frame::CmHiliteWideGaugeRail )
EVT_MENU( Frame::MenuEvent::HILITEOWNNORMAL,	Frame::CmHiliteOwnNormalGaugeRail )
EVT_MENU( Frame::MenuEvent::HILITEOWNWIDE,		Frame::CmHiliteOwnWideGaugeRail )
EVT_MENU( Frame::MenuEvent::HILITEWZA,			Frame::CmHilitewza )
EVT_MENU( Frame::MenuEvent::HILITEWZB,			Frame::CmHilitewzb )
EVT_MENU( Frame::MenuEvent::HILITEWZC,			Frame::CmHilitewzc )
EVT_MENU( Frame::MenuEvent::HILITEWZD,			Frame::CmHilitewzd )
EVT_MENU( Frame::MenuEvent::HILITEWZE,			Frame::CmHilitewze )
EVT_MENU( Frame::MenuEvent::HILITEWZF,			Frame::CmHilitewzf )
EVT_MENU( Frame::MenuEvent::HILITEWZG,			Frame::CmHilitewzg )
EVT_MENU( Frame::MenuEvent::HILITEWZH1,			Frame::CmHilitewzh1 )
EVT_MENU( Frame::MenuEvent::HILITEWZH2,			Frame::CmHilitewzh2 )
EVT_MENU( Frame::MenuEvent::HILITEWZH3,			Frame::CmHilitewzh3 )
EVT_MENU( Frame::MenuEvent::HILITEWZI1,			Frame::CmHilitewzi1 )
EVT_MENU( Frame::MenuEvent::HILITEWZ1,			Frame::CmHilitewz1 )
EVT_MENU( Frame::MenuEvent::HILITEWZ2,			Frame::CmHilitewz2 )
EVT_MENU( Frame::MenuEvent::HILITEWZ3,			Frame::CmHilitewz3 )
EVT_MENU( Frame::MenuEvent::HILITEWZ4,			Frame::CmHilitewz4 )
EVT_MENU( Frame::MenuEvent::HILITEWZ5,			Frame::CmHilitewz5 )
EVT_MENU( Frame::MenuEvent::HILITEWZ6,			Frame::CmHilitewz6 )
EVT_MENU( Frame::MenuEvent::HILITEWZ7,			Frame::CmHilitewz7 )
EVT_MENU( Frame::MenuEvent::HILITEWZ8,			Frame::CmHilitewz8 )
EVT_MENU( Frame::MenuEvent::HILITEWZ9,			Frame::CmHilitewz9 )
EVT_MENU( Frame::MenuEvent::HILITEWZ10,			Frame::CmHilitewz10 )
EVT_MENU( Frame::MenuEvent::HILITEWZ11,			Frame::CmHilitewz11 )
EVT_MENU( Frame::MenuEvent::HILITEWZ12,			Frame::CmHilitewz12 )
EVT_MENU( Frame::MenuEvent::HILITEWZ13,			Frame::CmHilitewz13 )
EVT_MENU( Frame::MenuEvent::HILITEWZ14,			Frame::CmHilitewz14 )
EVT_MENU( Frame::MenuEvent::HILITEWZ15,			Frame::CmHilitewz15 )
EVT_MENU( Frame::MenuEvent::HILITEWZ16,			Frame::CmHilitewz16 )
EVT_MENU( Frame::MenuEvent::HILITEWZ17,			Frame::CmHilitewz17 )
EVT_MENU( Frame::MenuEvent::HILITEWZ18,			Frame::CmHilitewz18 )
EVT_MENU( Frame::MenuEvent::HILITEWZI2,			Frame::CmHilitewzi2 )
EVT_MENU( Frame::MenuEvent::HILITEWZI3,			Frame::CmHilitewzi3 )
EVT_MENU( Frame::MenuEvent::HILITEWZIS,			Frame::CmHilitewzis )
EVT_MENU( Frame::MenuEvent::HILITEWZJ1,			Frame::CmHilitewzj1 )
EVT_MENU( Frame::MenuEvent::HILITEWZJ2,			Frame::CmHilitewzj2 )
EVT_MENU( Frame::MenuEvent::HILITEWZK,			Frame::CmHilitewzk )
EVT_MENU( Frame::MenuEvent::HILITEWZL,			Frame::CmHilitewzl )
EVT_MENU( Frame::MenuEvent::HILITEWZM,			Frame::CmHilitewzm )
EVT_MENU( Frame::MenuEvent::HILITEWZN,			Frame::CmHilitewzn )
EVT_MENU( Frame::MenuEvent::HILITEWZO,			Frame::CmHilitewzo )
EVT_MENU( Frame::MenuEvent::HILITEWZP1,			Frame::CmHilitewzp1 )
EVT_MENU( Frame::MenuEvent::HILITEWZP2,			Frame::CmHilitewzp2 )
EVT_MENU( Frame::MenuEvent::HILITEWZQ,			Frame::CmHilitewzq )
#if 0
EVT_MENU( Frame::MenuEvent::EDITVIEW, Frame::CmEditView )
#endif
EVT_MENU( Frame::MenuEvent::VIEWTEXT,			Frame::CmViewText )
EVT_MENU( Frame::MenuEvent::ALLVIEWSON,			Frame::CmAllViewsOn )
EVT_MENU( Frame::MenuEvent::ALLVIEWSOFF,		Frame::CmAllViewsOff )
EVT_MENU( Frame::MenuEvent::SHOWVIEW01,			Frame::CmShowView01 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW02,			Frame::CmShowView02 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW03,			Frame::CmShowView03 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW04,			Frame::CmShowView04 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW05,			Frame::CmShowView05 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW06,			Frame::CmShowView06 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW07,			Frame::CmShowView07 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW08,			Frame::CmShowView08 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW09,			Frame::CmShowView09 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW10,			Frame::CmShowView10 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW11,			Frame::CmShowView11 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW12,			Frame::CmShowView12 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW13,			Frame::CmShowView13 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW14,			Frame::CmShowView14 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW15,			Frame::CmShowView15 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW16,			Frame::CmShowView16 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW17,			Frame::CmShowView17 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW18,			Frame::CmShowView18 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW19,			Frame::CmShowView19 )
EVT_MENU( Frame::MenuEvent::SHOWVIEW20,			Frame::CmShowView20 )
EVT_MENU( Frame::MenuEvent::ATTACKSUP,			Frame::CmAttacksup )
EVT_MENU( Frame::MenuEvent::CAPTOESCORT,		Frame::CmCaptoescort )
EVT_MENU( Frame::MenuEvent::CAPTOINTERCEPTION,	Frame::CmCaptointerception )
EVT_MENU( Frame::MenuEvent::CHKSTACKS,			Frame::CmChkstacks )
EVT_MENU( Frame::MenuEvent::HILITESTACKS,		Frame::CmHilitestacks )
EVT_MENU( Frame::MenuEvent::HILITEMOT,			Frame::CmHiliteitemot )
EVT_MENU( Frame::MenuEvent::ADDMULBERRY,		Frame::CmAddmulberry )
EVT_MENU( Frame::MenuEvent::HILITECONS,			Frame::CmHilitecons )
EVT_MENU( Frame::MenuEvent::HILITERRENG,		Frame::CmHiliterreng )
EVT_MENU( Frame::MenuEvent::HILITEABORTED,		Frame::CmHiliteaborted )
EVT_MENU( Frame::MenuEvent::HILITETRUCKS,		Frame::CmHilitetrucks )
EVT_MENU( Frame::MenuEvent::SHOW_DISRUPTED,		Frame::CmHilitedisrupted )
EVT_MENU( Frame::MenuEvent::SHOW_INTZONES1,		Frame::CmHiliteIntZones1 )
EVT_MENU( Frame::MenuEvent::SHOW_INTZONES2,		Frame::CmHiliteIntZones2 )
EVT_MENU( Frame::MenuEvent::SHOW_INTZONES3,		Frame::CmHiliteIntZones3 )
EVT_MENU( Frame::MenuEvent::SHOW_INTZONES4,		Frame::CmHiliteIntZones4 )
EVT_MENU( Frame::MenuEvent::SHOW_INTSOURCES,	Frame::CmHiliteIntSources )
EVT_MENU( Frame::MenuEvent::SHOW_PASOURCES,		Frame::CmHilitePASources )
EVT_MENU( Frame::MenuEvent::SHOW_PAZONES,		Frame::CmHilitePAZones )
EVT_MENU( Frame::MenuEvent::ALLALLIED,			Frame::CmAllallied )
EVT_MENU( Frame::MenuEvent::ALLAXIS,			Frame::CmAllaxis )
EVT_MENU( Frame::MenuEvent::ALLNEUTRAL,			Frame::CmAllneutral )
EVT_MENU( Frame::MenuEvent::VISIBLEALLIED,		Frame::CmVisibleallied )
EVT_MENU( Frame::MenuEvent::VISIBLEAXIS,		Frame::CmVisibleaxis )
EVT_MENU( Frame::MenuEvent::VISIBLENEUTRAL,		Frame::CmVisibleneutral )
EVT_MENU( Frame::MenuEvent::CHANGENATIONAXIS,	Frame::CmChangenationside )
EVT_MENU( Frame::MenuEvent::CHANGENATIONALLIED, Frame::CmChangenationallied )
EVT_MENU( Frame::MenuEvent::CHANGEUNITSIDE,		Frame::CmChangeunitside )
EVT_MENU( Frame::MenuEvent::GAUGE_ALL,			Frame::CmGaugeAll )
EVT_MENU( Frame::MenuEvent::GAUGE_VISIBLE,		Frame::CmGaugeVisible )
EVT_MENU( Frame::MenuEvent::CLEAR_ALL,			Frame::CmClearAll )
EVT_MENU( Frame::MenuEvent::CLEAR_VISIBLE,		Frame::CmClearVisible )
EVT_MENU( Frame::MenuEvent::COMMANDSENDPHASE,	Frame::CmEndPhase )
EVT_MENU( Frame::MenuEvent::OPTIONSSETUPMODE,	Frame::CmSetupMode )
EVT_MENU( Frame::MenuEvent::CHANGESIDE,			Frame::CmChangeSide )
EVT_MENU( Frame::MenuEvent::EVADMIN,				Frame::cmAdmin )
EVT_MENU( Frame::MenuEvent::EVSTRATEGIC,			Frame::CmStrategic )
EVT_MENU( Frame::MenuEvent::EVTACTICAL,				Frame::CmTactical )
EVT_MENU( Frame::MenuEvent::SUPPLY,				Frame::CmSupply )
EVT_MENU( Frame::MenuEvent::HEXOWNER,			Frame::cmHexOwner )
EVT_MENU( Frame::MenuEvent::TELEPORT,			Frame::CmTeleport )
EVT_MENU( Frame::MenuEvent::TIMEJUMP,			Frame::CmTimejump )
EVT_MENU( Frame::MenuEvent::SHOWPATH,			Frame::CmShowpath )
EVT_MENU( Frame::MenuEvent::CHGCRT,				Frame::CmChgcrt )
EVT_MENU( Frame::MenuEvent::CHANGETEC,			Frame::CmChgtec )
#if 0
EVT_MENU( Frame::MenuEvent::SEARCHHEX, Frame::CmSearchHex )
EVT_MENU( Frame::MenuEvent::SEARCHCITY, Frame::CmSearchCity )
#endif
EVT_MENU( Frame::MenuEvent::SEARCHUNIT,			Frame::CmSearchUnit )
EVT_MENU( Frame::MenuEvent::SEARCHAGAIN,		Frame::CmSearchAgain )
EVT_MENU( Frame::MenuEvent::HILITEAIRBASE,		Frame::CmHiliteAirbase )
EVT_MENU( Frame::MenuEvent::HILITEINOP,			Frame::CmHiliteInop )
EVT_MENU( Frame::MenuEvent::ALPARTISANS,		Frame::CmAlPartisans )
EVT_MENU( Frame::MenuEvent::AXPARTISANS,		Frame::CmAxPartisans )
EVT_MENU( Frame::MenuEvent::PEEKPARTISANS,		Frame::CmPeekPartisans )
EVT_MENU( Frame::MenuEvent::SETALLSIZES,		Frame::CmSetAllSizes )
EVT_MENU( Frame::MenuEvent::SETALLZOCS,			Frame::CmSetAllZOCs )
EVT_MENU( Frame::MenuEvent::REMOVECMARKERS,		Frame::CmRemoveCMarkers )
EVT_MENU( Frame::MenuEvent::EVNOT_DISRUPTED,		Frame::CmNotDisrupted )
EVT_MENU( Frame::MenuEvent::EVDISRUPTED,			Frame::CmDisrupted )
EVT_MENU( Frame::MenuEvent::BAD_DISRUPTED,		Frame::CmBadDisrupted )
EVT_MENU( Frame::MenuEvent::REMOVE_DISRUPTION,	Frame::CmRemoveDisruption )
EVT_MENU( Frame::MenuEvent::CLEAROBSTRUCTED,	Frame::CmClearObstructed )
EVT_MENU( Frame::MenuEvent::RAILUSAGE,			Frame::CmRailUsage )
EVT_MENU( Frame::MenuEvent::HIDEUNITS,			Frame::CmHideUnits )
EVT_MENU( Frame::MenuEvent::SHOWHITS, Frame::CmShowHits)
EVT_MENU( Frame::MenuEvent::CHANGEORIGOWNER, Frame::CmChangeOrigOwner)
EVT_MENU( Frame::MenuEvent::CHANGEORIGOWNERALL, Frame::CmChangeOrigOwnerAll)
EVT_MENU( Frame::MenuEvent::CHANGEORIGOWNERVISIBLE, Frame::CmChangeOrigOwnerVisible)
#if 0
EVT_MENU( Frame::MenuEvent::CHANGENATIONSIDE, Frame::CmChangeNationsSide )
#endif

// Help menu:
EVT_MENU( Frame::MenuEvent::HELPABOUT,		Frame::OnAbout )
EVT_MENU( Frame::MenuEvent::HELPHOMEPAGE,	Frame::OnHomePage )
EVT_MENU( Frame::MenuEvent::HELPCONTENTS,	Frame::OnHelp )

// intercept keyboard events:
#if 0
EVT_KEY_DOWN( Frame::OnKey )
#endif
#endif
EVT_SIZE(Frame::OnSize)
END_EVENT_TABLE()

Frame::Frame( const wxString& title, const wxPoint& pos, const wxSize& size )
		:
				wxFrame( (wxFrame*)nullptr, -1, title, pos, size,
						wxDEFAULT_FRAME_STYLE | wxRESIZE_BORDER | wxNO_FULL_REPAINT_ON_RESIZE )
{
	for ( int c = 0; c < ht::NUM_SEABOX; ++c )
	{
		seacircles_[_sea_circle[c][1]] = wxAtoi( _sea_circle[c][0] );	// seacircles["NONE"] = 256
		seacircles2_[wxAtoi( _sea_circle[c][0] )] = _sea_circle[c][1];	// seacircles2[256] = "NONE"
		if ( c != 0 ) // skipping index 0 "None", for later manual insertion post-sort
			seabox_strings_.Add( _sea_circle[c][1] );
	}
	seabox_strings_.Sort( );
	seabox_strings_.Insert( wxT("None"), 0 );

	for ( int d = 0; d < ht::NUM_DIST; ++d )
	{
		districts_[_land_district[d][1]] = wxAtoi( _land_district[d][0] );	// districts["NONE"] = 256
		districts2_[wxAtoi( _land_district[d][0] )] = _land_district[d][1];	// districts2[256] = "NONE"
		if ( d != 0 ) // skipping index 0 "None", for later manual insertion post-sort
			land_district_strings_.Add( _land_district[d][1] );
	}
	land_district_strings_.Sort( );
	land_district_strings_.Insert( wxT("None"), 0 );

#if defined HEXTOOLSPLAY
	for ( int z = 0; z < ht::NUM_SEAZONE; ++z )
	{
		seazones_[_sea_zone[z][1]] = wxAtoi( _sea_zone[z][0] );	// seacircles["NONE"] = 256
		seazones2_[wxAtoi( _sea_zone[z][0] )] = _sea_zone[z][1];	// seacircles2[256] = "NONE"
		if ( z != 0 ) // skipping index 0 "None", for later manual insertion post-sort
			seazone_strings_.Add( _sea_zone[z][1] );
	}
	seazone_strings_.Sort( );
	seazone_strings_.Insert( wxT("None"), 0 );

	for ( int c = 0; c < ht::NUM_COUNTRY; ++c )
	{
		countries_[_land_country[c][1]] = wxAtoi( _land_country[c][0] );	// districts["NONE"] = 256
		countries2_[wxAtoi( _land_country[c][0] )] = _land_country[c][1];	// districts2[256] = "NONE"
		if ( c != 0 ) // skipping index 0 "None", for later manual insertion post-sort
			land_country_strings_.Add( _land_country[c][1] );
	}
	land_country_strings_.Sort( );
	land_country_strings_.Insert( wxT("None"), 0 );
#endif
	toolbar_ = 0;
	map_ = 0;
	info_pane_ = 0;
#if defined HEXTOOLSMAP
	terrain_selector_ = 0;
#elif defined HEXTOOLSPLAY
	contentsPane = 0;

	_recent_file1 = _("-");
	_recent_file2 = _("-");
	_recent_file3 = _("-");
	_recent_file4 = _("-");
	_recent_file5 = _("-");

	loadSettings( );  // must be done early (graphics paths are loaded here)
#endif
	SetIcon( wxICON(frame_icon) );

	build_menu( );

	setup_toolbar( );

	int top = 0;

	info_pane_ = new HexInfoPane(	this,
									-1,
									wxPoint( 0, top ),
									wxSize( 255, _infopane_height ),
									wxRETAINED | wxNO_FULL_REPAINT_ON_RESIZE
								);

	if ( ! info_pane_ )
		wxMessageBox( _("HexInfoPane creation failed"), _("Error") );

#if defined HEXTOOLSMAP
	terrain_selector_ = new TerrainSelector(	this,
#elif defined HEXTOOLSPLAY
	contentsPane = new HexContentsPane(	this,
#endif
										-1,
										wxPoint( 0, top + _infopane_height ),
										wxSize( _side_width, 100 ),
										wxRETAINED | wxNO_FULL_REPAINT_ON_RESIZE
										);

#if defined HEXTOOLSMAP
	if ( ! terrain_selector_ )
		wxMessageBox( _("TerrainSelector creation failed"), _("Error") );
#elif defined HEXTOOLSPLAY
	if ( ! contentsPane )
		wxMessageBox( _("HexContentsPane creation failed"), _("Error") );
#endif

	map_ = new MapPane(	this,
						-1,
						wxPoint( _side_width, top ),
#if defined HEXTOOLSMAP
						wxSize( 400, 500 ),
#elif defined HEXTOOLSPLAY
						wxSize( 500, 600 ),
#endif
						wxRETAINED | wxNO_FULL_REPAINT_ON_RESIZE
						);

	if ( ! map_ )
		wxMessageBox( _("MapPane creation failed"), _("Error") );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	map_->attachUnitWindow( contentsPane );
	map_->attachInfoWindow( info_pane_ );
	contentsPane->attachMap( map_ );
#endif
	CreateStatusBar( );
	SetStatusText( "Welcome to " + Application::NAME + " " + Application::VERSION );
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	SetupMenu( );  // enable/disable items
#endif
	SetSize( _frame_width, _frame_height );
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	loadSettings( );  // must be done late again
#endif
}

void Frame::build_menu( )
{
	if ( GetMenuBar( ) ) // do this only once!
	{
		wxMessageBox( _("MenuBar already exists!"), _("ERROR") );
		return;
	}

	wxMenuBar* menuBar = new wxMenuBar( );
	assert( menuBar );
	// TODO: if assert fails, then won't get here, correct?
#if 0
	if ( ! menuBar )
	{
		wxMessageBox( _("Failed to create MenuBar!"), _("ERROR") );
		return;
	}
#endif

#if defined HEXTOOLSMAP
	wxMenu* menuFile = new wxMenu( _(""), wxMENU_TEAROFF );
	assert( menuFile );
	menuFile->Append( CMD_OPEN, _("&Open..."), _("Open mapfile") );
	menuFile->Append( CMD_SAVE, _("&Save..."), _("Save current map") );
	menuFile->Append( CMD_SAVEAS, _("Save &As..."), _("Save current map with new name") );
	menuFile->AppendSeparator( );
	menuFile->Append( CMD_EXPORT, _("&Export Image..."), _("Export map image to file") );
	menuFile->AppendSeparator( );
	menuFile->Append( CMD_QUIT, _("E&xit"), _("Quit this program") );

	wxMenu* menuCmd = new wxMenu( _(""), wxMENU_TEAROFF );
	assert( menuCmd );
	menuCmd->Append( CMD_REPAINT, _("Repaint Map"), _("Repaint map graphics") );
	menuCmd->Append( CMD_ZOOMIN, _("Zoom &In"), _("Zoom map in.") );
	menuCmd->Append( CMD_ZOOMOUT, _("Zoom &Out"), _("Zoom map out.") );
	menuCmd->Append( CMD_ZOOM100, _("Zoom &100%"), _("Restore normal zoom level.") );

	wxMenu* menuHelp = new wxMenu( _(""), wxMENU_TEAROFF );
	assert( menuHelp );
	menuHelp->Append( CMD_HELP, _("&Contents..."), _("Show help") );
	menuHelp->AppendSeparator( );
	menuHelp->Append( CMD_ABOUT, _("&About..."), "Show about dialog" );

	menuBar->Append( menuFile, "&File" );
	menuBar->Append( menuCmd, "&Commands" );
	menuBar->Append( menuHelp, "&Help" );

#elif defined HEXTOOLSPLAY
	const long noMenuStyle = 0; // was wxMENU_TEAROFF for GTK build

	/////////////////////////////
	// File menu
	/////////////////////////////

	wxMenu* menuFile = new wxMenu( wxT(""), noMenuStyle );
	assert( menuFile );

	menuFile->Append( Frame::MenuEvent::FILENEW, wxT("&New..."), wxT("Create new scenario. You will be asked map name and starting date.") );
	menuFile->Append( Frame::MenuEvent::FILEOPEN, wxT("&Open..."), wxT("Open existing scenario file.") );
	menuFile->Append( Frame::MenuEvent::FILEINSERT, wxT("&Insert..."), wxT("Insert existing scenario file to current scenario.") );
	menuFile->Append( Frame::MenuEvent::FILESAVE, wxT("&Save"), wxT("Save current scenario to file.") );
	menuFile->Append( Frame::MenuEvent::FILESAVEAS, wxT("Save &As..."), wxT("Save current scenario to file under new name.") );
	menuFile->AppendSeparator( );

	/////////////////////////////

	menuFile->Append( Frame::MenuEvent::RECENT1, wxT("Recent1"), wxT("Open this recently used scenario") );
	menuFile->Append( Frame::MenuEvent::RECENT2, wxT("Recent2"), wxT("Open this recently used scenario") );
	menuFile->Append( Frame::MenuEvent::RECENT3, wxT("Recent3"), wxT("Open this recently used scenario") );
	menuFile->Append( Frame::MenuEvent::RECENT4, wxT("Recent4"), wxT("Open this recently used scenario") );
	menuFile->Append( Frame::MenuEvent::RECENT5, wxT("Recent5"), wxT("Open this recently used scenario") );
	menuFile->AppendSeparator( );

	/////////////////////////////

	menuFile->Append( Frame::MenuEvent::EXIT, wxT("&Exit"), wxT("Exit HexTools!") );

	/////////////////////////////
	// Options menu
	/////////////////////////////

	wxMenu* menuOptions = new wxMenu( wxT(""), noMenuStyle );
	assert( menuOptions );

	menuOptions->Append( Frame::MenuEvent::COMMANDSENDPHASE, wxT("&End Phase"), wxT("End current phase and go the next one.") );
	menuOptions->AppendSeparator( );

	/////////////////////////////

	menuOptions->Append( Frame::MenuEvent::OPTIONSSETUPMODE, wxT("&Setup Mode"), wxT("Enter/exit setup mode. In setup mode all commands are available.") );
	menuOptions->Append( Frame::MenuEvent::CHANGESIDE, wxT("Change Pla&yer"), wxT("Toggle between phasing and nonphasing player.") );
	menuOptions->Append( Frame::MenuEvent::TIMEJUMP, wxT("Time &Jump..."), wxT("Jump to other turn/month/year.") );
	menuOptions->Append( Frame::MenuEvent::DIEROLL, wxT("Roll a &Die..."), wxT("Show result of one die roll.") );
	menuOptions->Append( Frame::MenuEvent::DIEROLL2, wxT("Roll 2D6..."), wxT("Show result of two 6 sided die roll.") );
	menuOptions->Append( Frame::MenuEvent::DIEROLL12, wxT("Roll 1D12..."), wxT("Show result of one 12 sided die roll.") );
	menuOptions->Append( Frame::MenuEvent::DIEROLL100, wxT("Roll D100..."), wxT("Show result of one percent die roll.") );
	menuOptions->AppendSeparator( );

	/////////////////////////////

	menuOptions->Append( Frame::MenuEvent::OPTIONSREPAINT, wxT("Repaint &Map"), wxT("Repaint the whole map area. Recalculates various stats too.") );
	menuOptions->AppendSeparator( );

	/////////////////////////////

	menuOptions->Append( Frame::MenuEvent::REMOVECMARKERS, wxT("Remove All Combat Reports"), wxT("Remove all combat reports from the map") );
	menuOptions->AppendSeparator( );

	/////////////////////////////

	menuOptions->Append( Frame::MenuEvent::REPLPOOL, wxT("Rep&lacement Pool..."), wxT("Show current player's replacement pool (tip: use 'Change Player' command if you want to see other side's pool).") );
	menuOptions->Append( Frame::MenuEvent::AIRREPLPOOL, wxT("&Air Replacement Pool..."), wxT("Show current player's air unit replacement pool (tip: use 'Change Player' command if you want to see other side's pool).") );
	menuOptions->AppendSeparator( );

	/////////////////////////////

	menuOptions->Append( Frame::MenuEvent::CALCAXIS, wxT("Axis Statistics..."), wxT("Show various statistics about axis forces.") );
	menuOptions->Append( Frame::MenuEvent::CALCALLIED, wxT("Allied Statistics..."), wxT("Show various statistics about allied forces.") );
	menuOptions->Append( Frame::MenuEvent::AXISLOSS, wxT("Losses..."), wxT("Show losses suffered by both axis and allied forces.") );
	menuOptions->Append( Frame::MenuEvent::ANALYZE_DIE, wxT("Analyze Die Rolls..."), wxT("Check combat die rolls, catch cheaters!") );
	menuOptions->AppendSeparator( );

	/////////////////////////////

	menuOptions->Append( Frame::MenuEvent::SHOWCRT, wxT("View CRT..."), wxT("Show currently used CRT (Combat Result Table).") );
	menuOptions->Append( Frame::MenuEvent::CHGCRT, wxT("Change CRT..."), wxT("Select new CRT (Combat Result Table) file.") );
	menuOptions->Append( Frame::MenuEvent::CHANGETEC, wxT("Change TEC..."), wxT("Select new TEC (Terrain Effects Chart) file.") );
	menuOptions->AppendSeparator( );

	/////////////////////////////

	menuOptions->Append( Frame::MenuEvent::OPTIONSDISPLAY, wxT("S&ettings..."), wxT("View/adjust HexTools settings. These settings are stored to your computer only.") );
	menuOptions->Append( Frame::MenuEvent::OPTIONSRULES, wxT("&Rules..."), wxT("View/adjust game rules. Rules are stored in the scenario file.") );

		// Options submenu Set
		wxMenu* menuOptionsSubSet = new wxMenu( wxT(""), noMenuStyle );
		assert( menuOptionsSubSet );

		menuOptionsSubSet->Append( Frame::MenuEvent::SETALLSIZES, wxT("&All RE size's to default"), wxT("Set RE size's of all units to default values (defines in Options->Rules).") );
		menuOptionsSubSet->Append( Frame::MenuEvent::SETALLZOCS, wxT("&All ZOC's to default"), wxT("Set ZOC settings of all units to default values (defines in Options->Rules).") );

	menuOptions->Append( Frame::MenuEvent::MENU_SET, wxT("&Set"), menuOptionsSubSet );
#if 0
	menuOptions->Append( Frame::MenuEvent::OPTIONSMAPINFO, wxT("&Map Info..." ), wxT("") );
#endif
	/////////////////////////////
	// View menu
	/////////////////////////////

	wxMenu* menuView = new wxMenu( wxT(""), noMenuStyle );
	assert( menuView );

	menuView->Append( Frame::MenuEvent::ZOOMIN, wxT("Zoom &In"), wxT("Zoom map in.") );
	menuView->Append( Frame::MenuEvent::ZOOMOUT, wxT("Zoom &Out"), wxT("Zoom map out.") );
	menuView->Append( Frame::MenuEvent::ZOOM100, wxT("Zoom &100%"), wxT("Restore normal zoom level.") );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::HIDEUNITS, wxT("Hide Counters From Map"), wxT("Hide all counters from map in order to see map underneath them."), true );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::SEARCHUNIT, wxT("&Search...         Ctrl+F"), wxT("Search for units, hex id's, cities, etc...") );
	menuView->Append( Frame::MenuEvent::SEARCHAGAIN, wxT("Search &Again     F3"), wxT("Repeat the last search operation.") );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::HEXOWNER, wxT("Show Hex &Owner Markers"), wxT("Toggle hex owner visibility on/off."), true );
	menuView->Append( Frame::MenuEvent::SHOWPATH, wxT("Show Pat&hs"), wxT("Toggle unit path visibility on/off."), true );
	menuView->Append( Frame::MenuEvent::SHOWATTACKMARKERS, wxT("Show &Combat Markers..."), wxT("Toggle combat marker visibility on/off (you may want to hide them from you opponent)."), true );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::ALPARTISANS, wxT("Show Allied &Partisans..."), wxT("Toggle partisan visibility of allied partisan units on/off (you may want to hide them from you opponent)."), true );
	menuView->Append( Frame::MenuEvent::AXPARTISANS, wxT("Show A&xis Partisans..."), wxT("Toggle partisan visibility of allied partisan units on/off (you may want to hide them from you opponent)."), true );
	menuView->Append( Frame::MenuEvent::PEEKPARTISANS, wxT("Peek Partisan"), wxT("Peek currently selected unit to see if it's real or dummy partisan.") );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::HILITEMOT, wxT("&Motorized Units"), wxT("Show hexes that contain motorized units.") );
	menuView->Append( Frame::MenuEvent::HILITEARMOR, wxT("&Armored Units"), wxT("Show hexes that contain units with half or more AECA.") );
	menuView->Append( Frame::MenuEvent::HILITEARTILLERY, wxT("A&rtillery Units"), wxT("Show hexes that contain artillery units.") );
	menuView->Append( Frame::MenuEvent::HILITECONS, wxT("C&onstruction units"), wxT("Show hexes that contain construction capable units.") );
	menuView->Append( Frame::MenuEvent::HILITERRENG, wxT("Rai&lroad engineers"), wxT("Show hexes that contain railroad engineers.") );
	menuView->Append( Frame::MenuEvent::HILITETRUCKS, wxT("&Trucks"), wxT("Show hexes that contain truck.") );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::HILITESTACKS, wxT("&Illegal stacks"), wxT("Show hexes that contain illegal stacks (stacking limit is set in Options->Rules->Stacking).") );
	menuView->Append( Frame::MenuEvent::HILITECADRES, wxT("&Cadres"), wxT("Show hexes that contain units reduced to cadre strength.") );
	menuView->Append( Frame::MenuEvent::HILITEUNMOVED, wxT("&Unmoved Ground Units"), wxT("Show hexes that contain units that have not moved this phase.") );
	menuView->Append( Frame::MenuEvent::HILITEENEMYZOC, wxT("Enemy ZOCs (Full and Reduced)"), wxT("Show hexes that are in Full or Reduced Enemy ZOCs.") );
	menuView->Append( Frame::MenuEvent::HILITEFULLENEMYZOC, wxT("Full Enemy ZOCs"), wxT("Show hexes that are in Full Enemy ZOCs.") );
	menuView->Append( Frame::MenuEvent::HILITEREDUCEDENEMYZOC, wxT("Reduced Enemy ZOCs"), wxT("Show hexes that are in Reduced Enemy ZOCs.") );
	menuView->Append( Frame::MenuEvent::HILITEUNMOVED, wxT("&Unmoved Units"), wxT("Show hexes that contain units that have not moved this phase.") );
	menuView->Append( Frame::MenuEvent::SHOW_DISRUPTED, wxT("&Disrupted Units"), wxT("Show hexes that contain disrupted or badly disrupted units.") );
	menuView->Append( Frame::MenuEvent::SHOWHITS, wxT("Hits"), wxT("Show hexes that contain hits of any kind.") );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::HILITEPLANES, wxT("&Planes"), wxT("Show hexes that contain any planes.") );
	menuView->Append( Frame::MenuEvent::HILITEFLYINGPLANES, wxT("&Flying Planes"), wxT("Show hexes that contain flying planes.") );
	menuView->Append( Frame::MenuEvent::HILITEABORTED, wxT("A&borted Planes"), wxT("Show hexes that contain aborted planes.") );
	menuView->Append( Frame::MenuEvent::HILITEINOP, wxT("I&noperative Planes"), wxT("Show hexes that contain inoperative planes.") );
	menuView->Append( Frame::MenuEvent::HILITEAIRBASE, wxT("Airba&ses"), wxT("Show hexes that contain functioning airbases.") );
	menuView->AppendSeparator( );

	/////////////////////////////

	menuView->Append( Frame::MenuEvent::HILITENORMAL, wxT("Normal Gauge Rail"), wxT("Show hexes that contain Normal Gauge Rail.") );
	menuView->Append( Frame::MenuEvent::HILITEWIDE, wxT("Wide Gauge Rail"), wxT("Show hexes that contain Wide Gauge Rail.") );
	menuView->Append( Frame::MenuEvent::HILITEOWNNORMAL, wxT("Own Normal Gauge Rail"), wxT("Show hexes that contain Own Normal Gauge Rail.") );
	menuView->Append( Frame::MenuEvent::HILITEOWNWIDE, wxT("Own Wide Gauge Rail"), wxT("Show hexes that contain Own Wide Gauge Rail.") );
	menuView->AppendSeparator( );

	/////////////////////////////

		// View submenu Interceptor Zones
		wxMenu* menuViewSubInterceptorZones = new wxMenu( wxT(""), noMenuStyle );
		assert( menuViewSubInterceptorZones );

		menuViewSubInterceptorZones->Append( Frame::MenuEvent::SHOW_INTZONES1, wxT("Any Interceptors"), wxT("Show hexes that are within enemy interceptor range.") );
		menuViewSubInterceptorZones->Append( Frame::MenuEvent::SHOW_INTZONES2, wxT("Att. Better That 4"), wxT("Show hexes that are within interceptor range of F's with attack strength above 3.") );
		menuViewSubInterceptorZones->Append( Frame::MenuEvent::SHOW_INTZONES3, wxT("Att. Better Than 6"), wxT("Show hexes that are within interceptor range of F's with attack strength above 5.") );
		menuViewSubInterceptorZones->Append( Frame::MenuEvent::SHOW_INTZONES4, wxT("Att. Better Than 8"), wxT("Show hexes that are within interceptor range of F's with attack strength above 7.") );

	menuView->Append( Frame::MenuEvent::MENU_INTERCP, wxT("Interceptor zones"), menuViewSubInterceptorZones );

	menuView->Append( Frame::MenuEvent::SHOW_PAZONES, wxT("Patrol Attack Zones"), wxT("Show hexes that within enemy patrol attack range.") );
	menuView->Append( Frame::MenuEvent::SHOW_INTSOURCES, wxT("Possible Interceptors"), wxT("Show hexes that contain interceptors that can reach selected hex.") );
	menuView->Append( Frame::MenuEvent::SHOW_PASOURCES, wxT("Possible Patrol Attackers"), wxT("Show hexes that contain interceptors that can reach selected hex for patrol attack.") );
	menuView->AppendSeparator( );

	/////////////////////////////

		// View submenu Weather Zone
		wxMenu* menuViewSubWeatherZone = new wxMenu( wxT(""), noMenuStyle );
		assert( menuViewSubWeatherZone );

		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZA, wxT("&A"), wxT("Show hexes belonging to weather zone A") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZB, wxT("&B"), wxT("Show hexes belonging to weather zone B") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZC, wxT("&C"), wxT("Show hexes belonging to weather zone C") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZD, wxT("&D"), wxT("Show hexes belonging to weather zone D") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZE, wxT("&E"), wxT("Show hexes belonging to weather zone E") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZF, wxT("&F"), wxT("Show hexes belonging to weather zone F") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZG, wxT("&G"), wxT("Show hexes belonging to weather zone G") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZH1, wxT("&H1"), wxT("Show hexes belonging to weather zone H1") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZH2, wxT("&H2"), wxT("Show hexes belonging to weather zone H2") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZH3, wxT("&H3"), wxT("Show hexes belonging to weather zone H3") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZI1, wxT("&I1"), wxT("Show hexes belonging to weather zone I1") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ1, wxT("&1"), wxT("Show hexes belonging to weather zone 1") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ2, wxT("&2"), wxT("Show hexes belonging to weather zone 2") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ3, wxT("&3"), wxT("Show hexes belonging to weather zone 3") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ4, wxT("&4"), wxT("Show hexes belonging to weather zone 4") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ5, wxT("&5"), wxT("Show hexes belonging to weather zone 5") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ6, wxT("&6"), wxT("Show hexes belonging to weather zone 6") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ7, wxT("&7"), wxT("Show hexes belonging to weather zone 7") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ8, wxT("&8"), wxT("Show hexes belonging to weather zone 8") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ9, wxT("&9"), wxT("Show hexes belonging to weather zone 9") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ10, wxT("&10"), wxT("Show hexes belonging to weather zone 10") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ11, wxT("&11"), wxT("Show hexes belonging to weather zone 11") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ12, wxT("&12"), wxT("Show hexes belonging to weather zone 12") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ13, wxT("&13"), wxT("Show hexes belonging to weather zone 13") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ14, wxT("&14"), wxT("Show hexes belonging to weather zone 14") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ15, wxT("&15"), wxT("Show hexes belonging to weather zone 15") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ16, wxT("&16"), wxT("Show hexes belonging to weather zone 16") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ17, wxT("&17"), wxT("Show hexes belonging to weather zone 17") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZ18, wxT("&18"), wxT("Show hexes belonging to weather zone 18") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZI2, wxT("&I2"), wxT("Show hexes belonging to weather zone I2") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZI3, wxT("&I3"), wxT("Show hexes belonging to weather zone I3") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZIS, wxT("&IS"), wxT("Show hexes belonging to weather zone IS") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZJ1, wxT("&J1"), wxT("Show hexes belonging to weather zone J1") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZJ2, wxT("&J2"), wxT("Show hexes belonging to weather zone J2") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZK, wxT("&K"), wxT("Show hexes belonging to weather zone K") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZL, wxT("&L"), wxT("Show hexes belonging to weather zone L") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZM, wxT("&M"), wxT("Show hexes belonging to weather zone M") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZN, wxT("&N"), wxT("Show hexes belonging to weather zone N") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZO, wxT("&O"), wxT("Show hexes belonging to weather zone O") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZP1, wxT("&P1"), wxT("Show hexes belonging to weather zone P1") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZP2, wxT("&P2"), wxT("Show hexes belonging to weather zone P2") );
		menuViewSubWeatherZone->Append( Frame::MenuEvent::HILITEWZQ, wxT("&Q"), wxT("Show hexes belonging to weather zone Q") );

	menuView->Append( Frame::MenuEvent::MENU_HILITEWZ, wxT("&Weather Zone"), menuViewSubWeatherZone );

	/////////////////////////////
	// Show menu (for "views")
	/////////////////////////////

	wxMenu* menuShow = new wxMenu( wxT(""), noMenuStyle );
	assert( menuShow );

	//menuShow->Append( Frame::MenuEvent::EDITVIEW, wxT("Edit View"), wxT("Edit FIRST Active View.") );
	menuShow->Append( Frame::MenuEvent::VIEWTEXT, wxT("Show View Text"), wxT("Show Text associated with FIRST Active View.") );
	menuShow->Append( Frame::MenuEvent::ALLVIEWSON, wxT("All Views On"), wxT("Set all Views with defined Hexes Active.") );
	menuShow->Append( Frame::MenuEvent::ALLVIEWSOFF, wxT("All Views Off"), wxT("Set all Views Inactive.") );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW01, wxT("Show View 01"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW02, wxT("Show View 02"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW03, wxT("Show View 03"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW04, wxT("Show View 04"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW05, wxT("Show View 05"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW06, wxT("Show View 06"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW07, wxT("Show View 07"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW08, wxT("Show View 08"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW09, wxT("Show View 09"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW10, wxT("Show View 10"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW11, wxT("Show View 11"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW12, wxT("Show View 12"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW13, wxT("Show View 13"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW14, wxT("Show View 14"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW15, wxT("Show View 15"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW16, wxT("Show View 16"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW17, wxT("Show View 17"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW18, wxT("Show View 18"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW19, wxT("Show View 19"), wxT("Show named view."), true );
	menuShow->Append( Frame::MenuEvent::SHOWVIEW20, wxT("Show View 20"), wxT("Show named view."), true );

	/////////////////////////////
	// Hex menu
	/////////////////////////////

	wxMenu* menuHex = new wxMenu( wxT(""), noMenuStyle );
	assert( menuHex );

	menuHex->Append( Frame::MenuEvent::HEXINFO, wxT("&Hex Info..."), wxT("Show information about the selected hex.") );
	menuHex->Append( Frame::MenuEvent::STACKINFO, wxT("&Stack Info..."), wxT("Show information about units in the selected hex.") );
	menuHex->AppendSeparator( );

	/////////////////////////////

		// Hex submenu Place Marker
		wxMenu* menuHexSubMarker = new wxMenu( wxT(""), noMenuStyle );
		assert( menuHexSubMarker );

		menuHexSubMarker->Append( Frame::MenuEvent::ADDNOTE, wxT("Text &Note..."), wxT("Add text note to the selected hex.") );
		//menuHexSubMarker->Append( Frame::MenuEvent::ADDALERT, wxT("Text &Alert..."), wxT("Add text alert to the selected hex (alert icon will appear to map).") );
		menuHexSubMarker->Append( Frame::MenuEvent::ADDSTATUSMARKER, wxT("Status &Marker..."), wxT("Add status marker to the selected hex.") );
		menuHexSubMarker->Append( Frame::MenuEvent::ADDMULBERRY, wxT("&Mulberry"), wxT("Create new mulberry.") );
		menuHexSubMarker->Append( Frame::MenuEvent::ADDBARRAGE, wxT("&Balloon Barrage"), wxT("Create new balloon barrage marker.") );
		menuHexSubMarker->Append( Frame::MenuEvent::ADDCONTESTED, wxT("&Contested hex"), wxT("Create new contested hex marker.") );

	menuHex->Append( Frame::MenuEvent::MENU_MARKERS, wxT("&Place Marker"), menuHexSubMarker );
	menuHex->AppendSeparator( );

	/////////////////////////////

	menuHex->Append( Frame::MenuEvent::COMMANDSCHANGEOWNER, wxT("Change Hex &Owner\tO"), wxT("Toggle selected hex's ownership between axis and allied.") );
	menuHex->Append( Frame::MenuEvent::MAKENEUTRAL, wxT("Make Hex N&eutral\tN"), wxT("Set selected hex's ownership to neutral.") );
	menuHex->Append( Frame::MenuEvent::CHANGEORIGOWNER, wxT("Set Old Ownership to Ownership\tL"), wxT("Make owner of the hex the old owner too (the one who owned the hex at the beginning of the turn).") );
	menuHex->AppendSeparator( );

	/////////////////////////////

	menuHex->Append( Frame::MenuEvent::CHGSUPPLY, wxT("Toggle &Unsupplied\tCtrl+s"), wxT("Toggle supply status of the selected hex between supplied and unsupplied.") );
	menuHex->Append( Frame::MenuEvent::SPECIALSUPPLY, wxT("Toggle &Special Supply"), wxT("Toggle supply status of the selected hex between supplied and special supplied.") );
	menuHex->Append( Frame::MenuEvent::ISOLATED, wxT("Toggle &Isolated\tCtrl+i"), wxT("Toggle isolation status of the selected hex between isolated and non-isolated.") );
	menuHex->AppendSeparator( );

	/////////////////////////////

#if 0
	menuHex->Append( Frame::MenuEvent::HEXREGAUGE, "Re&gauge Rail", "Regauge rail in the selected hex. If not in setup mode railroad engineer with sufficient MP's must be selected." );
	if ( Phase == SETUP_Phase )
	{
		menuHex->Append( Frame::MenuEvent::REPAIR, "&Repair Rail", "Repair rail in the selected hex. If not in setup mode construction unit with sufficient MP's must be selected." );
		if ( RuleSet.RoadsCanBeBroken )
		{
#endif
	menuHex->Append( Frame::MenuEvent::HEXHITRAIL, wxT("&Break Road/Railroad"), wxT("Break road or rail line in the selected hex. If not in setup mode unit with sufficient MP's must be selected.") );
	menuHex->Append( Frame::MenuEvent::REPAIR, wxT("&Repair Road/Railroad..."), wxT("Repair damaged road or rail line. If not in setup mode construction capable unit with sufficient MP's must be selected.") );
	menuHex->Append( Frame::MenuEvent::CLEAROBSTRUCTED, wxT("&Clear Obstructed Railroad"), wxT("Clear obstructed rail line. Only in setup mode.") );
#if 0
	}
	else
	{
		menuHex->Append( Frame::MenuEvent::HEXHITRAIL, "&Break Rail", "Break rail in the selected hex. If not in setup mode unit with sufficient MP's must be selected." );
		menuHex->Append( Frame::MenuEvent::REPAIR, "&Repair Rail...", "Repair damaged rail. If not in setup mode construction capable unit with sufficient MP's must be selected." );
	}
#endif
	menuHex->AppendSeparator( );

	/////////////////////////////

	menuHex->Append( Frame::MenuEvent::REPAIRBRIDGE, wxT("Repair Bri&dge..."), wxT("Break bridge adjacent to the selected hex. If there are several bridges you will be prompted which one to break.") );
	menuHex->Append( Frame::MenuEvent::BREAKBRIDGE, wxT("Break Bridg&e..."), wxT("Repair bridge adjacent to the selected hex. If there are several bridges you will be prompted which one to repair.") );
	menuHex->AppendSeparator( );

	/////////////////////////////

	menuHex->Append( Frame::MenuEvent::SETAFHITS, wxT("&Airbase Hits..."), wxT("Adjust number of airbase hits in the selected hex.") );
	menuHex->Append( Frame::MenuEvent::SETAFUSAGE, wxT("Ai&rbase Usage..."), wxT("Adjust used airbase capacity in the selected hex.") );
	menuHex->Append( Frame::MenuEvent::SETPORTHITS, wxT("&Port Hits..."), wxT("Adjust number of port hits in the selected hex.") );
	menuHex->Append( Frame::MenuEvent::SETPORTUSAGE, wxT("P&ort Usage..."), wxT("Adjust used port capacity in the selected hex.") );
	menuHex->Append( Frame::MenuEvent::DESTROYPORT, wxT("(Un)&Destroy Port"), wxT("Destroy/undestroy port in the selected hex.") );
	menuHex->Append( Frame::MenuEvent::SETCDHITS, wxT("&CD Hits..."), wxT("Adjust number of CD (Coastal defenses) hits in the selected hex.") );
	menuHex->Append( Frame::MenuEvent::SETHARASSMENT, wxT("&Harassment Hits..."), wxT("Adjust number of harassment hits in the selected hex.") );
#if 0
	menuHex->Append( Frame::MenuEvent::SETAXMINES, wxT("Axis &Mines..."), wxT("Adjust number of axis mines points in the selected hex.") );
	menuHex->Append( Frame::MenuEvent::SETALMINES, wxT("Allied Mi&nes..."), wxT("Adjust number of allied mine points in the selected hex.") );
#endif
	/////////////////////////////
	// Units menu
	/////////////////////////////

	wxMenu* menuUnits = new wxMenu( wxT(""), noMenuStyle );
	assert( menuUnits );

	menuUnits->Append( Frame::MenuEvent::HEXCREATEUNIT, wxT("New &Unit..."), wxT("Create new combat unit.") );
	menuUnits->Append( Frame::MenuEvent::HEXCREATEAIRUNIT, wxT("New &Air Group..."), wxT("Create new air group.") );
	menuUnits->Append( Frame::MenuEvent::HEXCREATESHIP, wxT("New &Ship..."), wxT("Create new ship.") );
	menuUnits->Append( Frame::MenuEvent::RP, wxT("New &Resource Point(s)..."), wxT("Create new resource point(s).") );
	menuUnits->AppendSeparator( );

	/////////////////////////////

	menuUnits->Append( Frame::MenuEvent::HEXEDIT_UNIT, wxT("&Edit Unit(s)..."), wxT("Edit selected unit's attributes (combat units, planes, ships, etc.)") );
	menuUnits->Append( Frame::MenuEvent::DESTROYUNIT, wxT("&Destroy/Make cadre..."), wxT("Destroy selected unit(s) by moving it to the replacement pool or making it cadre. Record losses (see Options->Losses).") );
	menuUnits->Append( Frame::MenuEvent::MOVETOREPLPOOL, wxT("Move To Repl. Pool..."), wxT("Move selected unit(s) to replacement pool. Do not record losses.") );
	menuUnits->Append( Frame::MenuEvent::HEXDELETEUNIT, wxT("Rem&ove From Play..."), wxT("Completely remove selected unit(s) from the game. Do not record losses. (see Options->Losses)") );
	menuUnits->AppendSeparator( );

	/////////////////////////////

	menuUnits->Append( Frame::MenuEvent::EVNOT_DISRUPTED, wxT("Set &Non-disrupted"), wxT("Set selected units to non-disrupted status.") );
	menuUnits->Append( Frame::MenuEvent::EVDISRUPTED, wxT("Set D&isrupted"), wxT("Set selected units to disrupted status.") );
	menuUnits->Append( Frame::MenuEvent::BAD_DISRUPTED, wxT("Set &Badly Disrupted"), wxT("Set selected units to badly disrupted status.") );
	menuUnits->AppendSeparator( );

	/////////////////////////////

	menuUnits->Append( Frame::MenuEvent::REMOVE_DISRUPTION, wxT("Remove Disruption From ALL Units"), wxT("Remove disruption from ALL units.") );
	menuUnits->AppendSeparator( );

	/////////////////////////////

	menuUnits->Append( Frame::MenuEvent::ATTACKSUP, wxT("Toggle Unit's Attack Supply"), wxT("Toggle unit's attack supply status on/off (if attack supply rule is enabled).") );
	menuUnits->Append( Frame::MenuEvent::SUPPLY, wxT("Show Supply Range"), wxT("Show current supply range of unit.") );
	menuUnits->AppendSeparator( );

	/////////////////////////////

	menuUnits->Append( Frame::MenuEvent::ACTIVATEAIR, wxT("To&ggle Air Group Operative"), wxT("Toggle air group status between operative and inoperative.") );
	menuUnits->Append( Frame::MenuEvent::TOGGLE_DID_PA, wxT("Toggle 'Has &Patrol Attacked' status"), wxT("Toggle air group status between has/has not patrol attacked. This makes it easier to keep trakc of p.a.'d planes.") );
	menuUnits->AppendSeparator( );

	/////////////////////////////

	menuUnits->Append( Frame::MenuEvent::TELEPORT, wxT("&Teleport Unit(s)..."), wxT("Teleport selected units. After selecting this command selected units will be magically teleported to the hex you click next.") );
	menuUnits->AppendSeparator( );

	/////////////////////////////

	menuUnits->Append( Frame::MenuEvent::CHANGEUNITSIDE, wxT("&Change Unit's Side"), wxT("Change unit's side between axis and allied. Note that not all units can appear in both axis and allied sides.") );

	/////////////////////////////
	// Move menu
	/////////////////////////////

	wxMenu* menuMove = new wxMenu( wxT(""), noMenuStyle );
	assert( menuMove );

	menuMove->Append( Frame::MenuEvent::HEXUNITMOVER, wxT("&Unit Mover..."), wxT("Show/hide unit mover panel.") );
	menuMove->AppendSeparator( );

	/////////////////////////////

	menuMove->Append( Frame::MenuEvent::EVTACTICAL, wxT("Tactical Movement"), wxT("Move units with tactical movement."), true );
	menuMove->Append( Frame::MenuEvent::EVADMIN, wxT("Administrative Movement"), wxT("Move units with administrative movement."), true );
	menuMove->Append( Frame::MenuEvent::EVSTRATEGIC, wxT("Strategic Rail Movement"), wxT("Move units with strategic rail movement."), true );
	menuMove->AppendSeparator( );

	/////////////////////////////

	menuMove->Append( Frame::MenuEvent::UNITEAST, wxT("East"), wxT("Move selected unit(s) to east.") );
	menuMove->Append( Frame::MenuEvent::UNITNORTHEAST, wxT("Northeast"), wxT("Move selected unit(s) to northeast.") );
	menuMove->Append( Frame::MenuEvent::UNITSOUTHEAST, wxT("Southeast"), wxT("Move selected unit(s) to southeast.") );
	menuMove->Append( Frame::MenuEvent::UNITWEST, wxT("West"), wxT("Move selected unit(s) to west.") );
	menuMove->Append( Frame::MenuEvent::UNITNORTHWEST, wxT("Northwest"), wxT("Move selected unit(s) to northwest.") );
	menuMove->Append( Frame::MenuEvent::UNITSOUTHWEST, wxT("Southwest"), wxT("Move selected unit(s) to southwest.") );
	menuMove->AppendSeparator( );

	/////////////////////////////

	menuMove->Append( Frame::MenuEvent::ADVANCE, wxT("Advance"), wxT("Advance victorious units to the conquered hex after combat or overrun.") );
	menuMove->AppendSeparator( );

	/////////////////////////////

	menuMove->Append( Frame::MenuEvent::TOOVERSTACK, wxT("Enter &Overstack"), wxT("Put selected unit's to overstack.") );
	menuMove->Append( Frame::MenuEvent::FROVERSTACK, wxT("Leave O&verstack"), wxT("Remove selected unit's from overstack.") );
	menuMove->AppendSeparator( );

	/////////////////////////////

	menuMove->Append( Frame::MenuEvent::UNITTOTRAIN, wxT("&Board Train"), wxT("Load selected unit to train (for operative train movement).") );
	menuMove->Append( Frame::MenuEvent::UNITFROMTRAIN, wxT("&Leave Train"), wxT("Unload selected unit from train (after operative train movement).") );
	menuMove->Append( Frame::MenuEvent::TOSHIP, wxT("B&oard Ship"), wxT("Load selected unit to ship (for naval transport).") );
	menuMove->Append( Frame::MenuEvent::FROMSHIP, wxT("L&eave Ship"), wxT("Unload selected unit from ship (after naval transpor).") );
	menuMove->AppendSeparator( );

	/////////////////////////////

	//menuMove->Append( Frame::MenuEvent::RAILUSAGE, "Rail Capacity Usage...", "Show rail capacity used in this turn" );
	//menuMove->AppendSeparator();

	/////////////////////////////

	menuMove->Append( Frame::MenuEvent::MPPLUS, wxT("&Increase MP's"), wxT("Increase available MP's of selected unit(s) by one.") );
	menuMove->Append( Frame::MenuEvent::MPMINUS, wxT("&Decrease MP's"), wxT("Decrease available MP's of selected unit(s) by one.") );
	menuMove->AppendSeparator( );

	/////////////////////////////

	menuMove->Append( Frame::MenuEvent::UNITHOME, wxT("&Undo Movement"), wxT("Return selected unit(s) to the starting hex and restore it's MP's.") );

	/////////////////////////////
	// Fly menu
	/////////////////////////////

	wxMenu* menuFly = new wxMenu( wxT(""), noMenuStyle );
	assert( menuFly );

	menuFly->Append( Frame::MenuEvent::TAKEOFF, wxT("&Takeoff"), wxT("Takeoff from airbase.") );
	menuFly->Append( Frame::MenuEvent::STAGE, wxT("&Stage"), wxT("Stage.") );
	menuFly->Append( Frame::MenuEvent::RESOLVEAIRCOMBAT, wxT("&Fight Air Combat..."), wxT("Show air combat dialog box.") );
	menuFly->Append( Frame::MenuEvent::BOMB, wxT("&Assign Mission"), wxT("Assign bombing/transport mission for selected (flying) air groups.") );
	menuFly->Append( Frame::MenuEvent::FIREAA, wxT("Fire AA"), wxT("Fire AA against planes in the selected hex (DAS mission: adjacent to selected hex).") );
	menuFly->Append( Frame::MenuEvent::RESOLVEBOMBING, wxT("Resolve Bombing"), wxT("Resolve bombing mission. All selected planes make one combined bombing attempt.") );
	menuFly->Append( Frame::MenuEvent::LAND, wxT("&Land"), wxT("Land selected planes to friendly airbase.") );
	menuFly->AppendSeparator( );

	/////////////////////////////

	menuFly->Append( Frame::MenuEvent::CAPTOINTERCEPTION, wxT("&CAP To Interception"), wxT("Switch planes flying CAP to interception mission.") );
	menuFly->Append( Frame::MenuEvent::CAPTOESCORT, wxT("&CAP To Escort"), wxT("Switch planes flying CAP to escort mission.") );

	/////////////////////////////
	// Orders menu
	/////////////////////////////

	wxMenu* menuOrders = new wxMenu( wxT(""), noMenuStyle );
	assert( menuOrders );

	menuOrders->Append( Frame::MenuEvent::CANCELORDER, wxT("&Cancel Orders"), wxT("Cancel any pending combat/construction orders of the selected unit(s).") );
	menuOrders->AppendSeparator( );

	/////////////////////////////

	menuOrders->Append( Frame::MenuEvent::COMBAT, wxT("Resolve Combat"), wxT("Resolve ground combat in the selected hex.") );
	// 'Advance' moved to 'Move' menu
	//menuOrders->Append( Frame::MenuEvent::ADVANCE, "Advance", "Advance victorious units to the conquered hex after combat or overrun." );
	menuOrders->AppendSeparator( );

	/////////////////////////////

	//Append( menuOrders, MF_ENABLED, CM_CAP, "Fly &Combat Air Patrol" );
	menuOrders->Append( Frame::MenuEvent::NAVALPATROL, wxT("Assign To &Naval Patrol"), wxT("Assign selected air groups to naval patrol.") );
	menuOrders->AppendSeparator( );

	/////////////////////////////

	menuOrders->Append( Frame::MenuEvent::HEXHITAF, wxT("&Damage Airbase"), wxT("Damage airbase in the selected hex. Unit with sufficient MPs must be selected.") );
	menuOrders->Append( Frame::MenuEvent::HEXHITRAIL, wxT("&Break Road/Railroad"), wxT("Break road or rail line in the selected hex. Unit with sufficient MPs must be selected.") );
	menuOrders->AppendSeparator( );

	/////////////////////////////

	menuOrders->Append( Frame::MenuEvent::HEXCREATEFORT, wxT("Build &Fort..."), wxT("Start construction of fort. Construction capable unit must be selected.") );
	menuOrders->Append( Frame::MenuEvent::HEXCREATEPERMAF, wxT("Build/Augment A&irfield..."), wxT("Start construction of airfield. Construction capable unit must be selected.") );
	menuOrders->Append( Frame::MenuEvent::HEXCREATETEMPAF, wxT("Build &Temporary Airfield..."), wxT("Build temporary. Construction capable unit with sufficient MPs must be selected.") );
	menuOrders->Append( Frame::MenuEvent::FIELDWORKS, wxT("Build Fi&eldworks"), wxT("Build fieldworks (WW1). Construction capable unit with sufficient MPs must be selected.") );
	menuOrders->Append( Frame::MenuEvent::REPAIR, wxT("&Repair Rail/Airbase/Port..."), wxT("Repair damaged rail/airbase/port. Construction capable unit with sufficient MPs must be selected.") );
	menuOrders->Append( Frame::MenuEvent::HEXREGAUGE, wxT("Re&gauge Rail"), wxT("Regauge rail in the selected hex. If not in setup mode railroad engineer with sufficient MPs must be selected.") );
	menuOrders->Append( Frame::MenuEvent::REPAIRPORT, wxT("Rebuild &Port") );

	/////////////////////////////
	// Misc menu
	/////////////////////////////

	wxMenu* menuMisc = new wxMenu( wxT(""), noMenuStyle );
	assert( menuMisc );

	menuMisc->Append( Frame::MenuEvent::HEXCOMBATINFO, wxT("&Combat Info"), wxT("Show combat odd's in the selected hex.") );
	menuMisc->Append( Frame::MenuEvent::CANCELATTACK, wxT("Cancel A&ttack"), wxT("Cancel attack orders from all units attacking to the selected hex.") );
	menuMisc->Append( Frame::MenuEvent::CANCELAIR, wxT("Cancel Air Attack"), wxT("Cancel attack orders from all planes attacking to the selected hex.") );
	menuMisc->AppendSeparator( );

	/////////////////////////////

	menuMisc->Append( Frame::MenuEvent::OPTIONSWEATHER, wxT("Set &Weather..."), wxT("Adjust weather conditions for all weather zones.") );
	menuMisc->AppendSeparator( );

	/////////////////////////////

		// Misc submenu Ownership
		wxMenu* menuMiscSubOwnership = new wxMenu( wxT(""), noMenuStyle );
		assert( menuMiscSubOwnership );

		menuMiscSubOwnership->Append( Frame::MenuEvent::VISIBLEAXIS, wxT("Visible to A&xis..."), wxT("Set ownership of all currently visible hexes to axis (requires SETUP MODE).") );
		menuMiscSubOwnership->Append( Frame::MenuEvent::VISIBLEALLIED, wxT("Visible to &Allied..."), wxT("Set ownership of all currently visible hexes to allied (requires SETUP MODE).") );
		menuMiscSubOwnership->Append( Frame::MenuEvent::VISIBLENEUTRAL, wxT("Visible to &Neutral..."), wxT("Set ownership of all currently visible hexes to neutral (requires SETUP MODE).") );
		menuMiscSubOwnership->AppendSeparator( );

		menuMiscSubOwnership->Append( Frame::MenuEvent::ALLAXIS, wxT("All to Ax&is..."), wxT("Set ownership of ALL hexes to axis (requires SETUP MODE).") );
		menuMiscSubOwnership->Append( Frame::MenuEvent::ALLALLIED, wxT("All to Alli&ed..."), wxT("Set ownership of ALL hexes to allied (requires SETUP MODE).") );
		menuMiscSubOwnership->Append( Frame::MenuEvent::ALLNEUTRAL, wxT("All to Ne&utral..."), wxT("Set ownership of ALL hexes to neutral (requires SETUP MODE).") );

	menuMisc->Append( 0, wxT("Set O&wnership"), menuMiscSubOwnership );

		// Misc submenu Old Ownership
		wxMenu* menuMiscSubOldOwnership = new wxMenu( wxT(""), noMenuStyle );
		assert( menuMiscSubOldOwnership );

		menuMiscSubOldOwnership->Append( Frame::MenuEvent::CHANGEORIGOWNERALL, wxT("&All Hexes To Current Owner"), wxT("Set old owner (=beginning of the turn) of ALL hexes to be the current owner") );
		menuMiscSubOldOwnership->Append( Frame::MenuEvent::CHANGEORIGOWNERVISIBLE, wxT("&Visible Hexes To Current Owner"), wxT("Set old owner (=beginning of the turn) of currently visible hexes to be the current owner") );

	menuMisc->Append( 0, wxT("Set Old Ownership"), menuMiscSubOldOwnership );
	menuMisc->AppendSeparator( );

	/////////////////////////////

		// Misc submenu Rail Regauging
		wxMenu* menuMiscSubRailRegauge = new wxMenu( wxT(""), noMenuStyle );
		assert( menuMiscSubRailRegauge );

		menuMiscSubRailRegauge->Append( Frame::MenuEvent::GAUGE_ALL, wxT("&All Hexes..."), wxT("Toggle gauge of all rail hexes (requires SETUP MODE)") );
		menuMiscSubRailRegauge->Append( Frame::MenuEvent::GAUGE_VISIBLE, wxT("&Visible Hexes..."), wxT("Toggle gauge of currently visible rail hexes (requires SETUP MODE)") );

	menuMisc->Append( 0, wxT("Magical &Rail Regauging"), menuMiscSubRailRegauge );

		// Misc submenu Rail Clearance
		wxMenu* menuMiscSubRailClear = new wxMenu( wxT(""), noMenuStyle );
		assert( menuMiscSubRailClear );

		menuMiscSubRailClear->Append( Frame::MenuEvent::CLEAR_ALL, wxT("&All Hexes..."), wxT("Toggle obstruction of all rail hexes (requires SETUP MODE)") );
		menuMiscSubRailClear->Append( Frame::MenuEvent::CLEAR_VISIBLE, wxT("&Visible Hexes..."), wxT("Toggle obstruction of currently visible rail hexes (requires SETUP MODE)") );

	menuMisc->Append( 0, wxT("Magical Rail &Clearance"), menuMiscSubRailClear );
	menuMisc->AppendSeparator( );

	/////////////////////////////
#if 0
	menuMisc->Append( Frame::MenuEvent::ATTACKSUP, "Toggle Unit's Attack Supply", "Toggle unit's attack supply status on/off." );
	menuMisc->AppendSeparator( );
#endif
	/////////////////////////////

	menuMisc->Append( Frame::MenuEvent::UPDATESUPPLY, wxT("Update &Supply Status"), wxT("Update supply status of all units based on supply status of hexes they occupy.") );
	menuMisc->Append( Frame::MenuEvent::CHGISOLATED, wxT("Update Is&olation Status"), wxT("Update isolation status of all units based on isolation status of hexes they occupy.") );
	menuMisc->AppendSeparator( );

	/////////////////////////////

		// Misc submenu Change Side
		// nation side change doesn't work for planes/ships -> disabled
		wxMenu* menuMiscSubChangeSide = new wxMenu( wxT(""), noMenuStyle );
		assert( menuMiscSubChangeSide );

		menuMiscSubChangeSide->Append( Frame::MenuEvent::CHANGENATIONAXIS, wxT("N&ation/Group to Axis side") );
		menuMiscSubChangeSide->Append( Frame::MenuEvent::CHANGENATIONALLIED, wxT("Na&tion/Group to Allied side") );

	menuMisc->Append( 0, wxT("Change Nation/Group side"), menuMiscSubChangeSide );

	/////////////////////////////
	// Help menu
	/////////////////////////////

	wxMenu* menuHelp = new wxMenu( wxT(""), noMenuStyle );
	assert( menuHelp );

	menuHelp->Append( Frame::MenuEvent::HELPCONTENTS, wxT("&Help Contents..."), wxT("Show HexTools help. If help does not open go to 'help' subdirectory and open 'jet.htm'.") );
	menuHelp->AppendSeparator( );

	/////////////////////////////

	menuHelp->Append( Frame::MenuEvent::HELPHOMEPAGE, wxT("&HexTools Home Page..."), wxT("Open HexTools home page (http://groups.yahoo.com/group/jet-users/ in a web browser") );
	menuHelp->AppendSeparator( );

	/////////////////////////////

	menuHelp->Append( Frame::MenuEvent::HELPABOUT, wxT("&About HexTools..."), wxT("Show copyright information.") );

	/////////////////////////////
	/////////////////////////////
	/////////////////////////////

	menuBar->Append( menuFile, wxT("&File") );
	menuBar->Append( menuOptions, wxT("&Options") );
	menuBar->Append( menuView, wxT("&View") );
	menuBar->Append( menuShow, wxT("&Show") );
	menuBar->Append( menuHex, wxT("He&x") );
	menuBar->Append( menuUnits, wxT("&Units") );
	menuBar->Append( menuMove, wxT("&Move") );
	menuBar->Append( menuFly, wxT("F&ly") );
	menuBar->Append( menuOrders, wxT("O&rders") );
	menuBar->Append( menuMisc, wxT("M&isc") );
	menuBar->Append( menuHelp, wxT("&Help") );
#endif
	SetMenuBar( menuBar );
}

void Frame::setup_toolbar( )
{
#if defined HEXTOOLSMAP
	wxBitmap* openbmp = new wxBitmap( "graphics\\toolbar\\fileopen_map.bmp", wxBITMAP_TYPE_BMP );
	wxBitmap* savebmp = new wxBitmap( "graphics\\toolbar\\filesave_map.bmp", wxBITMAP_TYPE_BMP );
	wxBitmap* pntbmp = new wxBitmap( "graphics\\toolbar\\repaint.bmp", wxBITMAP_TYPE_BMP );
	wxBitmap* zoominbmp = new wxBitmap( "graphics\\toolbar\\zoomin.bmp", wxBITMAP_TYPE_BMP );
	wxBitmap* zoomoutbmp = new wxBitmap( "graphics\\toolbar\\zoomout.bmp", wxBITMAP_TYPE_BMP );

	bool whined = false;
	wxString whinestr = _("Failed to open bitmap for toolbar, please check your installation");

	if ( ! openbmp->Ok( ) )
		wxMessageBox( whinestr ), whined = true;

	if ( ! savebmp->Ok( ) && ! whined )
		wxMessageBox( whinestr ), whined = true;

	if ( ! pntbmp->Ok( ) && ! whined )
		wxMessageBox( whinestr ), whined = true;

	if ( ! zoominbmp->Ok( ) && ! whined )
		wxMessageBox( whinestr ), whined = true;

	if ( ! zoomoutbmp->Ok( ) && ! whined )
		wxMessageBox( whinestr ), whined = true;

#elif defined HEXTOOLSPLAY

#define PICDIR ""
#define PICEXT "bmp"
#define PICTYPE wxBITMAP_TYPE_BMP

	if ( ! stagebmp )
		stagebmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"stage."PICEXT), PICTYPE );
	static wxBitmap aabmp( wxT("graphics/toolbar/"PICDIR"aa."PICEXT), PICTYPE );
	static wxBitmap activateairbmp( wxT("graphics/toolbar/"PICDIR"activateair."PICEXT), PICTYPE );
	static wxBitmap adminbmp( wxT("graphics/toolbar/"PICDIR"admin."PICEXT), PICTYPE );
	static wxBitmap advancebmp( wxT("graphics/toolbar/"PICDIR"advance."PICEXT), PICTYPE );
	static wxBitmap aircombatbmp( wxT("graphics/toolbar/"PICDIR"aircombat."PICEXT), PICTYPE );
	static wxBitmap assignplanebmp( wxT("graphics/toolbar/"PICDIR"assignplane."PICEXT), PICTYPE );

	if ( ! bombbmp )
		bombbmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"bomb."PICEXT), PICTYPE );
	static wxBitmap changesidebmp( wxT("graphics/toolbar/"PICDIR"changeside."PICEXT), PICTYPE );
	static wxBitmap chownerbmp( wxT("graphics/toolbar/"PICDIR"chowner."PICEXT), PICTYPE );
	static wxBitmap combatbmp( wxT("graphics/toolbar/"PICDIR"combat."PICEXT), PICTYPE );
	static wxBitmap destroyunitbmp( wxT("graphics/toolbar/"PICDIR"destroyunit."PICEXT), PICTYPE );
	static wxBitmap editunitbmp( wxT("graphics/toolbar/"PICDIR"editunit."PICEXT), PICTYPE );
	static wxBitmap filenewbmp( wxT("graphics/toolbar/"PICDIR"filenew."PICEXT), PICTYPE );
	static wxBitmap fileopenbmp( wxT("graphics/toolbar/"PICDIR"fileopen_scn."PICEXT), PICTYPE );
	static wxBitmap filesavebmp( wxT("graphics/toolbar/"PICDIR"filesave_scn."PICEXT), PICTYPE );
	static wxBitmap fromshipbmp( wxT("graphics/toolbar/"PICDIR"fromship."PICEXT), PICTYPE );

	if ( ! fromtrainbmp )
		fromtrainbmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"fromtrain."PICEXT), PICTYPE );
	static wxBitmap froverstackbmp( wxT("graphics/toolbar/"PICDIR"froverstack."PICEXT), PICTYPE );
	static wxBitmap helpcontentsbmp( wxT("graphics/toolbar/"PICDIR"helpcontents."PICEXT), PICTYPE );
	static wxBitmap hexownerbmp( wxT("graphics/toolbar/"PICDIR"hexowner."PICEXT), PICTYPE );

	if ( ! hitafbmp )
		hitafbmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"hitaf."PICEXT), PICTYPE );

	if ( ! hitrailbmp )
		hitrailbmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"hitrail."PICEXT), PICTYPE );
	static wxBitmap interceptbmp( wxT("graphics/toolbar/"PICDIR"intercept."PICEXT), PICTYPE );
	static wxBitmap landbmp( wxT("graphics/toolbar/"PICDIR"land."PICEXT), PICTYPE );
	static wxBitmap layminebmp( wxT("graphics/toolbar/"PICDIR"laymine."PICEXT), PICTYPE );
	static wxBitmap moveunitsbmp( wxT("graphics/toolbar/"PICDIR"moveunits."PICEXT), PICTYPE );
	static wxBitmap mpminusbmp( wxT("graphics/toolbar/"PICDIR"mpminus."PICEXT), PICTYPE );
	static wxBitmap mpminus30bmp( wxT("graphics/toolbar/"PICDIR"mpminus30."PICEXT), PICTYPE );
	static wxBitmap mpplusbmp( wxT("graphics/toolbar/"PICDIR"mpplus."PICEXT), PICTYPE );
	static wxBitmap newairunitbmp( wxT("graphics/toolbar/"PICDIR"newairunit."PICEXT), PICTYPE );
	static wxBitmap newfortbmp( wxT("graphics/toolbar/"PICDIR"newfort."PICEXT), PICTYPE );
	static wxBitmap newshipbmp( wxT("graphics/toolbar/"PICDIR"newship."PICEXT), PICTYPE );
	static wxBitmap newunitbmp( wxT("graphics/toolbar/"PICDIR"newunit."PICEXT), PICTYPE );
	static wxBitmap permafbmp( wxT("graphics/toolbar/"PICDIR"permaf."PICEXT), PICTYPE );

	if ( ! regaugebmp )
		regaugebmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"regauge."PICEXT), PICTYPE );
	static wxBitmap removeunitbmp( wxT("graphics/toolbar/"PICDIR"removeunit."PICEXT), PICTYPE );
	static wxBitmap repaintbmp( wxT("graphics/toolbar/"PICDIR"repaint."PICEXT), PICTYPE );

	if ( ! repairbmp )
		repairbmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"repair."PICEXT), PICTYPE );
	static wxBitmap replpoolbmp( wxT("graphics/toolbar/"PICDIR"replpool."PICEXT), PICTYPE );
	static wxBitmap rmownerbmp( wxT("graphics/toolbar/"PICDIR"rmowner."PICEXT), PICTYPE );
	static wxBitmap rpbmp( wxT("graphics/toolbar/"PICDIR"rp."PICEXT), PICTYPE );
	static wxBitmap setupmodebmp( wxT("graphics/toolbar/"PICDIR"setupmode."PICEXT), PICTYPE );
	static wxBitmap setweatherbmp( wxT("graphics/toolbar/"PICDIR"setweather."PICEXT), PICTYPE );
	static wxBitmap showpathbmp( wxT("graphics/toolbar/"PICDIR"showpath."PICEXT), PICTYPE );
	static wxBitmap stratbmp( wxT("graphics/toolbar/"PICDIR"strat."PICEXT), PICTYPE );
	static wxBitmap sweepminebmp( wxT("graphics/toolbar/"PICDIR"sweepmine."PICEXT), PICTYPE );
	static wxBitmap tacticalbmp( wxT("graphics/toolbar/"PICDIR"tactical."PICEXT), PICTYPE );
	static wxBitmap takeoffbmp( wxT("graphics/toolbar/"PICDIR"takeoff."PICEXT), PICTYPE );
	static wxBitmap teleportbmp( wxT("graphics/toolbar/"PICDIR"teleport."PICEXT), PICTYPE );
	static wxBitmap tempafbmp( wxT("graphics/toolbar/"PICDIR"tempaf."PICEXT), PICTYPE );
	static wxBitmap tooverstackbmp( wxT("graphics/toolbar/"PICDIR"tooverstack."PICEXT), PICTYPE );
	static wxBitmap toshipbmp( wxT("graphics/toolbar/"PICDIR"toship."PICEXT), PICTYPE );

	if ( ! totrainbmp )
		totrainbmp = new wxBitmap( wxT("graphics/toolbar/"PICDIR"totrain."PICEXT), PICTYPE );
	static wxBitmap undomovebmp( wxT("graphics/toolbar/"PICDIR"undomove."PICEXT), PICTYPE );
	static wxBitmap zoominbmp( wxT("graphics/toolbar/"PICDIR"zoomin."PICEXT), PICTYPE );
	static wxBitmap zoomoutbmp( wxT("graphics/toolbar/"PICDIR"zoomout."PICEXT), PICTYPE );
	static wxBitmap hideunitsbmp( wxT("graphics/toolbar/"PICDIR"hideunits."PICEXT), PICTYPE );
#endif

	long style = wxNO_BORDER | wxTB_FLAT | wxTB_DOCKABLE | wxTB_HORIZONTAL;

	if ( ! toolbar_ )
		toolbar_ = CreateToolBar( style, 9999 );
	if ( ! toolbar_ )
		wxMessageBox( wxT("Failed to create toolbar!") );

	while ( toolbar_->GetToolsCount( ) > 0 )
		toolbar_->DeleteToolByPos( 0 );

	toolbar_->SetMargins( 1, 1 );

	toolbar_->SetToolBitmapSize( wxSize( 20, 20 ) );

#if defined HEXTOOLSMAP
	toolbar_->AddTool( CMD_OPEN, *openbmp, wxNullBitmap, false, -1, -1, nullptr, _("Open") );
	toolbar_->AddTool( CMD_SAVE, *savebmp, wxNullBitmap, false, -1, -1, nullptr, _("Save") );
	toolbar_->AddSeparator( );

	toolbar_->AddTool( CMD_REPAINT, *pntbmp, wxNullBitmap, false, -1, -1, nullptr, _("Repaint") );
	toolbar_->AddTool( CMD_ZOOMIN, *zoominbmp, wxNullBitmap, false, -1, -1, nullptr, _("Zoom In") );
	toolbar_->AddTool( CMD_ZOOMOUT, *zoomoutbmp, wxNullBitmap, false, -1, -1, nullptr, _("Zoom Out") );
	toolbar_->AddSeparator( );

	// map sheet mode button
	wxButton* mapLeafModeButton = new wxButton( toolbar_, CMD_MAPLEAFMODE, _("Map Sheet Mode") );

	toolbar_->AddControl( mapLeafModeButton );
	toolbar_->AddSeparator( );

	// weather zone selector
	wxButton* wzSelectorLabel = new wxButton( toolbar_, CMD_WZMODE, _("Weather Zone:") );
	toolbar_->AddControl( wzSelectorLabel );

	wxArrayString wzStrings;
	for ( int wz = 0; wz < ht::NUM_WZ; ++wz )
		wzStrings.Add( _(_weather_zone_string[wz]) );

	wxChoice* wzselector = new wxChoice( toolbar_, CMD_WZCHOICE, wxDefaultPosition, wxDefaultSize, wzStrings );
	wzselector->SetSelection( 3 ); // WZ "D"

	toolbar_->AddControl( wzselector );
	toolbar_->AddSeparator( );

	// maritime district (sea circle) selector
	wxButton* seaDistrictSelectorLabel = new wxButton( toolbar_, CMD_SEABOXMODE, _("Sea Circle:") );
	toolbar_->AddControl( seaDistrictSelectorLabel );

	wxChoice* seaboxselector = new wxChoice( toolbar_, CMD_SEABOXCHOICE, wxDefaultPosition, wxDefaultSize, seabox_strings_ );
	seaboxselector->SetSelection( seabox_strings_.Index("NONE") );

	toolbar_->AddControl( seaboxselector );
	toolbar_->AddSeparator( );

	// terrestrial district selector
	wxButton* landDistrictSelectorLabel = new wxButton( toolbar_, CMD_DISTRICTMODE, _("District:") );
	toolbar_->AddControl( landDistrictSelectorLabel );

	wxChoice* districtselector = new wxChoice( toolbar_, CMD_DISTRICTCHOICE, wxDefaultPosition, wxDefaultSize, land_district_strings_ );
	districtselector->SetSelection( land_district_strings_.Index("NONE") );

	toolbar_->AddControl( districtselector );
	toolbar_->AddSeparator( );

	wxButton* endModesButton = new wxButton( toolbar_, CMD_ENDMODES, _("End All Modes") );

	toolbar_->AddControl( endModesButton );

#elif defined HEXTOOLSPLAY
	if ( _current_phase == Phase::NO_GAME )
		toolbar_->AddTool( Frame::MenuEvent::FILENEW, filenewbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("New") );
	toolbar_->AddTool( Frame::MenuEvent::FILEOPEN, fileopenbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Open") );
	if ( _current_phase == Phase::NO_GAME )
	{
		toolbar_->Realize( );
		return;
	}
	toolbar_->AddTool( Frame::MenuEvent::FILESAVE, filesavebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Save") );

	toolbar_->AddSeparator( );

	toolbar_->AddTool( Frame::MenuEvent::ZOOMIN, zoominbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Zoom In") );
	toolbar_->AddTool( Frame::MenuEvent::ZOOMOUT, zoomoutbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Zoom Out") );
	toolbar_->AddTool( CMD_REPAINT, repaintbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Repaint") );
	toolbar_->AddTool( Frame::MenuEvent::CHANGESIDE, changesidebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Change side") );
	toolbar_->AddTool( Frame::MenuEvent::OPTIONSSETUPMODE, setupmodebmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Toggle setup mode") );
	if ( _current_phase == Phase::SETUP_PHASE )
		toolbar_->ToggleTool( Frame::MenuEvent::OPTIONSSETUPMODE, true );
	toolbar_->AddSeparator( );
	toolbar_->AddTool( Frame::MenuEvent::HEXOWNER, hexownerbmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Toggle hex ownership visibility") );
	toolbar_->AddTool( Frame::MenuEvent::SHOWPATH, showpathbmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Toggle path visibility") );
	toolbar_->AddTool( Frame::MenuEvent::HIDEUNITS, hideunitsbmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Toggle counter visibility") );
	if ( _show_path )
		toolbar_->ToggleTool( Frame::MenuEvent::SHOWPATH, true );
	toolbar_->AddSeparator( );

	wxButton* seaZoneSelectorLabel;
	wxChoice* seazoneselector;
	wxButton* countrySelectorLabel;
	wxChoice* countryselector;

	switch ( _current_phase )
	{
		case Phase::SETUP_PHASE:
			toolbar_->AddTool( Frame::MenuEvent::OPTIONSWEATHER, setweatherbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Set weather") );
			toolbar_->AddSeparator( );

			toolbar_->AddTool( Frame::MenuEvent::COMMANDSCHANGEOWNER, chownerbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Toggle hex ownership") );
			toolbar_->AddTool( Frame::MenuEvent::MAKENEUTRAL, rmownerbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Make hex neutral") );
			toolbar_->AddSeparator( );

			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEUNIT, newunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new unit") );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEAIRUNIT, newairunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new air group") );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATESHIP, newshipbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new ship") );
			toolbar_->AddTool( Frame::MenuEvent::RP, rpbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new resource point(s)") );
			toolbar_->AddSeparator( );

			toolbar_->AddTool( Frame::MenuEvent::HEXEDIT_UNIT, editunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Edit unit") );
			toolbar_->AddTool( Frame::MenuEvent::DESTROYUNIT, destroyunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Destroy to replacement pool") );
			toolbar_->AddTool( Frame::MenuEvent::HEXDELETEUNIT, removeunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Remove from game") );
			toolbar_->AddSeparator( );

			toolbar_->AddTool( Frame::MenuEvent::REPLPOOL, replpoolbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("View replacement pool") );
			toolbar_->AddTool( Frame::MenuEvent::ACTIVATEAIR, activateairbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Toggle operative") );
			toolbar_->AddSeparator( );

			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEFORT, newfortbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Build fort") );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEPERMAF, permafbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Build permanent airfield") );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATETEMPAF, tempafbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Build temporary airfield") );
			toolbar_->AddTool( Frame::MenuEvent::HEXREGAUGE, *regaugebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Regauge rail") );
			toolbar_->AddTool( Frame::MenuEvent::TELEPORT, teleportbmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Teleport unit(s)") );
			toolbar_->ToggleTool( Frame::MenuEvent::TELEPORT, _teleport_mode_on );
			toolbar_->AddSeparator( );

			seaZoneSelectorLabel = new wxButton( toolbar_, CMD_SEAZONEMODE, _("Sea Zone:") );
			toolbar_->AddControl( seaZoneSelectorLabel );
			seazoneselector = new wxChoice( toolbar_, CMD_SEAZONECHOICE, wxDefaultPosition, wxDefaultSize, seazone_strings_ );
			seazoneselector->SetSelection( seazone_strings_.Index("NONE") );
			toolbar_->AddControl( seazoneselector );
			toolbar_->AddSeparator( );

			countrySelectorLabel = new wxButton( toolbar_, CMD_COUNTRYMODE, _("Country:") );
			toolbar_->AddControl( countrySelectorLabel );
			countryselector = new wxChoice( toolbar_, CMD_COUNTRYCHOICE, wxDefaultPosition, wxDefaultSize, land_country_strings_ );
			countryselector->SetSelection( land_country_strings_.Index("NONE") );
			toolbar_->AddControl( countryselector );
			toolbar_->AddSeparator( );
			break;
		case Phase::INITIAL_PHASE:
			toolbar_->AddTool( Frame::MenuEvent::OPTIONSWEATHER, setweatherbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Set weather") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEUNIT, newunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new unit") );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEAIRUNIT, newairunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new air group") );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATESHIP, newshipbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new ship") );
			toolbar_->AddTool( Frame::MenuEvent::RP, rpbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Create new resource point(s)") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::HEXEDIT_UNIT, editunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Edit unit") );
			toolbar_->AddTool( Frame::MenuEvent::HEXDELETEUNIT, removeunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Remove from game") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::REPLPOOL, replpoolbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("View replacement pool") );
			toolbar_->AddTool( Frame::MenuEvent::ACTIVATEAIR, activateairbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Toggle operative") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEFORT, newfortbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Build fort") );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATEPERMAF, permafbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Build permanent airfield") );
			toolbar_->AddSeparator( );

			toolbar_->AddTool( Frame::MenuEvent::TAKEOFF, takeoffbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Takeoff") );
			toolbar_->AddTool( Frame::MenuEvent::STAGE, *stagebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Stage") );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				//SpeedBar->AddTool( Frame::MenuEvent::RESOLVEAIRCOMBAT, stagebmp, wxNullBitmap, false, -1, -1, nullptr, "Stage" );
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEAIRCOMBAT, aircombatbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Stage") );
				toolbar_->AddTool( Frame::MenuEvent::FIREAA, aabmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fire AA") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEBOMBING, *bombbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Bomb") );
			}
			else
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );

			toolbar_->AddTool( Frame::MenuEvent::LAND, landbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Land") );
			break;
		case Phase::NAVAL_PHASE_1:
		case Phase::NAVAL_PHASE_2:
		case Phase::NAVAL_PHASE_3:
		case Phase::NAVAL_PHASE_4:
		case Phase::NAVAL_PHASE_5:
		case Phase::NAVAL_PHASE_6:
		case Phase::NAVAL_PHASE_7:
		case Phase::NAVAL_PHASE_8:
		case Phase::NAVAL_PHASE_9:
		case Phase::NAVAL_PHASE_10:
			toolbar_->AddTool( Frame::MenuEvent::HEXUNITMOVER, moveunitsbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Unit mover panel") );
			toolbar_->AddTool( Frame::MenuEvent::UNITHOME, undomovebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Undo movement") );
			toolbar_->AddTool( Frame::MenuEvent::TOSHIP, toshipbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Board ship for naval transport") );
			toolbar_->AddTool( Frame::MenuEvent::FROMSHIP, fromshipbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Leave ship") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::MPPLUS, mpplusbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("+1 MP") );
			toolbar_->AddTool( Frame::MenuEvent::MPMINUS, mpminusbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("-1 MP") );
			toolbar_->AddTool( Frame::MenuEvent::MPMINUS30, mpminus30bmp, wxNullBitmap, false, -1, -1, nullptr, wxT("-30 MP's") );
			toolbar_->AddSeparator( );
#if 0
			toolbar_->AddTool( Frame::MenuEvent::LAYMINE, layminebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Lay mine") );
			toolbar_->AddTool( Frame::MenuEvent::SWEEPMINE, sweepminebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Sweep mine") );
			toolbar_->AddSeparator( );
#endif
			toolbar_->AddTool( Frame::MenuEvent::TAKEOFF, takeoffbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Takeoff") );
			toolbar_->AddTool( Frame::MenuEvent::STAGE, *stagebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Stage") );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEAIRCOMBAT, aircombatbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fight air combat") );
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
				toolbar_->AddTool( Frame::MenuEvent::FIREAA, aabmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fire AA") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEBOMBING, *bombbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Bomb") );
			}
			else
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
			toolbar_->AddTool( Frame::MenuEvent::LAND, landbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Land") );
			break;
		case Phase::REACT_MOVE_PHASE:
		case Phase::MOVEMENT_PHASE:
			toolbar_->AddTool( Frame::MenuEvent::EVTACTICAL, tacticalbmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Tactical movement") );
			toolbar_->AddTool( Frame::MenuEvent::EVADMIN, adminbmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Administrative movement") );
			toolbar_->AddTool( Frame::MenuEvent::EVSTRATEGIC, stratbmp, wxNullBitmap, true, -1, -1, nullptr, wxT("Strategic movement") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::HEXUNITMOVER, moveunitsbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Unit mover panel") );
			toolbar_->AddTool( Frame::MenuEvent::UNITHOME, undomovebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Undo movement") );
			toolbar_->AddTool( Frame::MenuEvent::UNITTOTRAIN, *totrainbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Board train for oper. rail movement") );
			toolbar_->AddTool( Frame::MenuEvent::UNITFROMTRAIN, *fromtrainbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Leave train") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::MPPLUS, mpplusbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Increase MP's") );
			toolbar_->AddTool( Frame::MenuEvent::MPMINUS, mpminusbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Decrease MP's") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::HEXHITRAIL, *hitrailbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Break road/railroad") );
			toolbar_->AddTool( Frame::MenuEvent::HEXHITAF, *hitafbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Hit airbase") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::HEXCREATETEMPAF, tempafbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Build temporary airbase") );
			toolbar_->AddTool( Frame::MenuEvent::REPAIR, *repairbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Repair damaged stuff") );
			toolbar_->AddTool( Frame::MenuEvent::HEXREGAUGE, *regaugebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Regauge rail") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::TAKEOFF, takeoffbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Takeoff") );
			toolbar_->AddTool( Frame::MenuEvent::STAGE, *stagebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Stage") );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEAIRCOMBAT, aircombatbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fight air combat") );
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
				toolbar_->AddTool( Frame::MenuEvent::FIREAA, aabmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fire AA") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEBOMBING, *bombbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Bomb") );
			}
			else
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
			toolbar_->AddTool( Frame::MenuEvent::LAND, landbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Land") );
			break;
		case Phase::REACTION_PHASE:
			toolbar_->AddTool( Frame::MenuEvent::HEXUNITMOVER, moveunitsbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Unit mover panel") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::TAKEOFF, takeoffbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Takeoff") );
			//SpeedBar->Insert(*btnIntercept);
			toolbar_->AddTool( Frame::MenuEvent::RESOLVEAIRCOMBAT, aircombatbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fight air combat") );
			toolbar_->AddTool( Frame::MenuEvent::LAND, landbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Land") );
			toolbar_->AddTool( Frame::MenuEvent::UNITHOME, undomovebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Undo movement") );
			break;
		case Phase::REACT_COMB_PHASE:
		case Phase::COMBAT_PHASE:
			toolbar_->AddTool( Frame::MenuEvent::HEXUNITMOVER, moveunitsbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Unit mover panel") );
			toolbar_->AddTool( Frame::MenuEvent::UNITHOME, undomovebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Undo movement") );
			toolbar_->AddSeparator( );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				toolbar_->AddTool( Frame::MenuEvent::TAKEOFF, takeoffbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Takeoff") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEAIRCOMBAT, aircombatbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fight air combat") );
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
				toolbar_->AddTool( Frame::MenuEvent::FIREAA, aabmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fire AA") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEBOMBING, *bombbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Bomb") );
				toolbar_->AddTool( Frame::MenuEvent::LAND, landbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Land") );
			}
			else
			{
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
				toolbar_->AddTool( Frame::MenuEvent::FIREAA, aabmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fire AA") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEBOMBING, *bombbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Bomb") );
			}
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::COMBAT, combatbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Resolve combat") );
			toolbar_->AddTool( Frame::MenuEvent::ADVANCE, advancebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Advance to conquered hex") );
			toolbar_->AddTool( Frame::MenuEvent::DESTROYUNIT, destroyunitbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Destroy/cadre unit") );
			break;
		case Phase::EXPLOITATION_PHASE:
			toolbar_->AddTool( Frame::MenuEvent::HEXUNITMOVER, moveunitsbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Unit mover panel") );
			toolbar_->AddTool( Frame::MenuEvent::UNITHOME, undomovebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Undo movement") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::MPPLUS, mpplusbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Increase MP's") );
			toolbar_->AddTool( Frame::MenuEvent::MPMINUS, mpminusbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Decrease MP's") );
			toolbar_->AddSeparator( );
			toolbar_->AddTool( Frame::MenuEvent::HEXHITRAIL, *hitrailbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Break road/railroad") );
			toolbar_->AddTool( Frame::MenuEvent::HEXHITAF, *hitafbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Hit airbase") );
			toolbar_->AddSeparator( );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				toolbar_->AddTool( Frame::MenuEvent::TAKEOFF, takeoffbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Takeoff") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEAIRCOMBAT, aircombatbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fight air combat") );
				toolbar_->AddTool( Frame::MenuEvent::BOMB, assignplanebmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Assign mission") );
				toolbar_->AddTool( Frame::MenuEvent::FIREAA, aabmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Fire AA") );
				toolbar_->AddTool( Frame::MenuEvent::RESOLVEBOMBING, *bombbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Bomb") );
			}
			toolbar_->AddTool( Frame::MenuEvent::LAND, landbmp, wxNullBitmap, false, -1, -1, nullptr, wxT("Land") );
			break;
		case Phase::NO_GAME:
			//if ( prev_phase != NO_GAME )
			//    toolbar_->Remove( *btnFileSave );  // no game to save
			break;
		default:
			wxMessageBox( wxT("Illegal phase, unable to setup speedbar"), Application::NAME );
			return;
	}
	SetMoveModeButtons( );
#endif
	toolbar_->Realize( );
}

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
// enable/disable menus for current phase
void Frame::SetupMenu( )
{
	wxMenuBar* menu;
	if ( ( menu = GetMenuBar( ) ) == 0 )
	{
		wxMessageBox( wxT("No menubar!"), wxT("ERROR") );
		return;
	}

	bool mode = ( _current_phase == Phase::SETUP_PHASE );

	// file
	menu->Enable( Frame::MenuEvent::FILEINSERT, mode );
	menu->Enable( Frame::MenuEvent::FILESAVE, mode );
	menu->Enable( Frame::MenuEvent::FILESAVEAS, mode );

	if ( _recent_file1.Cmp( wxT("-") ) == 0 )
	{
		menu->SetLabel( Frame::MenuEvent::RECENT1, wxT("&1 -") );
		menu->Enable( Frame::MenuEvent::RECENT1, false );
	}
	else
	{
		menu->SetLabel( Frame::MenuEvent::RECENT1, wxT("&1 ") + _recent_file1 );
		menu->Enable( Frame::MenuEvent::RECENT1, true );
	}

	if ( _recent_file2.Cmp( wxT("-") ) == 0 )
	{
		menu->SetLabel( Frame::MenuEvent::RECENT2, wxT("&2 -") );
		menu->Enable( Frame::MenuEvent::RECENT2, false );
	}
	else
	{
		menu->SetLabel( Frame::MenuEvent::RECENT2, wxT("&2 ") + _recent_file2 );
		menu->Enable( Frame::MenuEvent::RECENT2, true );
	}

	if ( _recent_file3.Cmp( wxT("-") ) == 0 )
	{
		menu->SetLabel( Frame::MenuEvent::RECENT3, wxT("&3 -") );
		menu->Enable( Frame::MenuEvent::RECENT3, false );
	}
	else
	{
		menu->SetLabel( Frame::MenuEvent::RECENT3, wxT("&3 ") + _recent_file3 );
		menu->Enable( Frame::MenuEvent::RECENT3, true );
	}

	if ( _recent_file4.Cmp( wxT("-") ) == 0 )
	{
		menu->SetLabel( Frame::MenuEvent::RECENT4, wxT("&4 -") );
		menu->Enable( Frame::MenuEvent::RECENT4, false );
	}
	else
	{
		menu->SetLabel( Frame::MenuEvent::RECENT4, wxT("&4 ") + _recent_file4 );
		menu->Enable( Frame::MenuEvent::RECENT4, true );
	}

	if ( _recent_file5.Cmp( wxT("-") ) == 0 )
	{
		menu->SetLabel( Frame::MenuEvent::RECENT5, wxT("&5 -") );
		menu->Enable( Frame::MenuEvent::RECENT5, false );
	}
	else
	{
		menu->SetLabel( Frame::MenuEvent::RECENT5, wxT("&5 ") + _recent_file5 );
		menu->Enable( Frame::MenuEvent::RECENT5, true );
	}

	bool showview;
	char menustr[HexNote::MAX_CAPTION_LEN];
	for ( int nview = 0; nview < 20; ++nview )
	{
		showview = nview < map_->GetViewCount( );
		if ( showview )
			strcpy( menustr, map_->GetViewName( nview ) );  //*views.aViews[nview].achName;
		else
			sprintf( menustr, "view %d", nview );
		menu->SetLabel( Frame::MenuEvent::SHOWVIEW01 + nview, menustr );
		sprintf( menustr, "Show %s", map_->GetViewName( nview ) );
		menu->SetHelpString( Frame::MenuEvent::SHOWVIEW01 + nview, menustr );
		menu->Check( Frame::MenuEvent::SHOWVIEW01 + nview, false );
		//menu->Enable( Frame::MenuEvent::SHOWVIEW01 + nview, showview );
	}

	// options
	menu->Enable( Frame::MenuEvent::COMMANDSENDPHASE, mode );
	menu->Enable( Frame::MenuEvent::OPTIONSSETUPMODE, mode );
	menu->Enable( Frame::MenuEvent::CHANGESIDE, mode );
	menu->Enable( Frame::MenuEvent::TIMEJUMP, mode );
	menu->Enable( Frame::MenuEvent::DIEROLL, mode );
	menu->Enable( Frame::MenuEvent::DIEROLL2, mode );
	menu->Enable( Frame::MenuEvent::DIEROLL12, mode );
	menu->Enable( Frame::MenuEvent::DIEROLL100, mode );
	menu->Enable( Frame::MenuEvent::OPTIONSREPAINT, mode );
	menu->Enable( Frame::MenuEvent::HEXOWNER, mode );
	menu->Enable( Frame::MenuEvent::SHOWPATH, mode );
	menu->Enable( Frame::MenuEvent::SHOWCRT, mode );
	menu->Enable( Frame::MenuEvent::CHGCRT, mode );
	menu->Enable( Frame::MenuEvent::CHANGETEC, mode );
	menu->Enable( Frame::MenuEvent::SHOWATTACKMARKERS, mode );
	menu->Enable( Frame::MenuEvent::REMOVECMARKERS, mode );
	menu->Enable( Frame::MenuEvent::ALPARTISANS, mode );
	menu->Enable( Frame::MenuEvent::AXPARTISANS, mode );
	menu->Enable( Frame::MenuEvent::PEEKPARTISANS, mode );
	menu->Enable( Frame::MenuEvent::REPLPOOL, mode );
	menu->Enable( Frame::MenuEvent::AIRREPLPOOL, mode );
	menu->Enable( Frame::MenuEvent::CALCAXIS, mode );
	menu->Enable( Frame::MenuEvent::CALCALLIED, mode );
	menu->Enable( Frame::MenuEvent::AXISLOSS, mode );
	menu->Enable( Frame::MenuEvent::ANALYZE_DIE, mode );
	menu->Enable( Frame::MenuEvent::OPTIONSDISPLAY, mode );
	menu->Enable( Frame::MenuEvent::OPTIONSRULES, mode );
	menu->Enable( Frame::MenuEvent::MENU_SET, mode );

	// view menu
	menu->Enable( Frame::MenuEvent::ZOOMIN, mode );
	menu->Enable( Frame::MenuEvent::ZOOMOUT, mode );
	menu->Enable( Frame::MenuEvent::ZOOM100, mode );
	menu->Enable( Frame::MenuEvent::HIDEUNITS, mode );
	menu->Enable( Frame::MenuEvent::SEARCHUNIT, mode );
	menu->Enable( Frame::MenuEvent::SEARCHAGAIN, mode );
	menu->Enable( Frame::MenuEvent::HILITEMOT, mode );
	menu->Enable( Frame::MenuEvent::HILITEARMOR, mode );
	menu->Enable( Frame::MenuEvent::HILITEARTILLERY, mode );
	menu->Enable( Frame::MenuEvent::HILITECONS, mode );
	menu->Enable( Frame::MenuEvent::HILITERRENG, mode );
	menu->Enable( Frame::MenuEvent::HILITETRUCKS, mode );
	menu->Enable( Frame::MenuEvent::HILITESTACKS, mode );
	menu->Enable( Frame::MenuEvent::HILITECADRES, mode );
	menu->Enable( Frame::MenuEvent::HILITEUNMOVED, mode );
	menu->Enable( Frame::MenuEvent::HILITEENEMYZOC, mode );
	menu->Enable( Frame::MenuEvent::HILITEFULLENEMYZOC, mode );
	menu->Enable( Frame::MenuEvent::HILITEREDUCEDENEMYZOC, mode );
	menu->Enable( Frame::MenuEvent::HILITEPLANES, mode );
	menu->Enable( Frame::MenuEvent::HILITEFLYINGPLANES, mode );
	menu->Enable( Frame::MenuEvent::HILITEABORTED, mode );
	menu->Enable( Frame::MenuEvent::HILITEINOP, mode );
	menu->Enable( Frame::MenuEvent::HILITEAIRBASE, mode );
	menu->Enable( Frame::MenuEvent::SHOWHITS, mode );
	menu->Enable( Frame::MenuEvent::HILITENORMAL, mode );
	menu->Enable( Frame::MenuEvent::HILITEWIDE, mode );
	menu->Enable( Frame::MenuEvent::HILITEOWNNORMAL, mode );
	menu->Enable( Frame::MenuEvent::HILITEOWNWIDE, mode );

	// Weather Zone submenu
	menu->Enable( Frame::MenuEvent::HILITEWZA, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZB, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZC, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZD, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZE, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZF, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZG, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZH1, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZH2, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZH3, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZI1, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ1, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ2, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ3, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ4, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ5, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ6, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ7, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ8, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ9, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ10, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ11, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ12, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ13, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ14, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ15, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ16, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ17, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZ18, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZI2, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZI3, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZIS, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZJ1, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZJ2, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZK, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZL, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZM, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZN, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZO, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZP1, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZP2, mode );
	menu->Enable( Frame::MenuEvent::HILITEWZQ, mode );

	// search submenu
	//menu->Enable(MENU_SEARCH, mode);
	//menu->Enable(CM_SEARCHHEX, mode);
	//menu->Enable(CM_SEARCHCITY, mode);
	menu->Enable( Frame::MenuEvent::SEARCHUNIT, mode );
	menu->Enable( Frame::MenuEvent::SEARCHAGAIN, mode );

	// hex
	menu->Enable( Frame::MenuEvent::HEXINFO, mode );
	menu->Enable( Frame::MenuEvent::STACKINFO, mode );
	menu->Enable( Frame::MenuEvent::ADDNOTE, mode );
	//menu->Enable(CM_ADDALERT, mode);
	menu->Enable( Frame::MenuEvent::ADDSTATUSMARKER, mode );
	menu->Enable( Frame::MenuEvent::COMMANDSCHANGEOWNER, mode );
	menu->Enable( Frame::MenuEvent::CHANGEORIGOWNER, mode );
	menu->Enable( Frame::MenuEvent::MAKENEUTRAL, mode );
	menu->Enable( Frame::MenuEvent::CHGSUPPLY, mode );
	menu->Enable( Frame::MenuEvent::SPECIALSUPPLY, mode );
	menu->Enable( Frame::MenuEvent::ISOLATED, mode );
	menu->Enable( Frame::MenuEvent::HEXHITRAIL, mode );
	menu->Enable( Frame::MenuEvent::REPAIRBRIDGE, mode );
	menu->Enable( Frame::MenuEvent::BREAKBRIDGE, mode );
	menu->Enable( Frame::MenuEvent::SETAFHITS, mode );
	menu->Enable( Frame::MenuEvent::SETAFUSAGE, mode );
	menu->Enable( Frame::MenuEvent::SETPORTHITS, mode );
	menu->Enable( Frame::MenuEvent::SETPORTUSAGE, mode );
	menu->Enable( Frame::MenuEvent::DESTROYPORT, mode );
	menu->Enable( Frame::MenuEvent::SETCDHITS, mode );
	menu->Enable( Frame::MenuEvent::SETHARASSMENT, mode );
#if 0
	menu->Enable( Frame::MenuEvent::SETAXMINES, mode );
	menu->Enable( Frame::MenuEvent::SETALMINES, mode );
#endif
	menu->Enable( Frame::MenuEvent::ADDCONTESTED, mode );
	menu->Enable( Frame::MenuEvent::ADDBARRAGE, mode );

	// units
	menu->Enable( Frame::MenuEvent::HEXCREATEUNIT, mode );
	menu->Enable( Frame::MenuEvent::HEXCREATEAIRUNIT, mode );
	menu->Enable( Frame::MenuEvent::HEXCREATESHIP, mode );
	menu->Enable( Frame::MenuEvent::RP, mode );
	menu->Enable( Frame::MenuEvent::ADDMULBERRY, mode );
	menu->Enable( Frame::MenuEvent::HEXEDIT_UNIT, mode );
	menu->Enable( Frame::MenuEvent::DESTROYUNIT, mode );
	menu->Enable( Frame::MenuEvent::HEXDELETEUNIT, mode );
	menu->Enable( Frame::MenuEvent::MOVETOREPLPOOL, mode );
	menu->Enable( Frame::MenuEvent::ACTIVATEAIR, mode );
	menu->Enable( Frame::MenuEvent::TELEPORT, mode );
	menu->Enable( Frame::MenuEvent::CHANGEUNITSIDE, mode );
	menu->Enable( Frame::MenuEvent::EVNOT_DISRUPTED, mode );
	menu->Enable( Frame::MenuEvent::EVDISRUPTED, mode );
	menu->Enable( Frame::MenuEvent::BAD_DISRUPTED, mode );
	menu->Enable( Frame::MenuEvent::REMOVE_DISRUPTION, mode );

	// orders
	menu->Enable( Frame::MenuEvent::CANCELORDER, mode );
	menu->Enable( Frame::MenuEvent::COMBAT, mode );
	menu->Enable( Frame::MenuEvent::ADVANCE, mode );
	menu->Enable( Frame::MenuEvent::NAVALPATROL, mode );
	menu->Enable( Frame::MenuEvent::HEXHITAF, mode );
	menu->Enable( Frame::MenuEvent::HEXCREATEFORT, mode );
	menu->Enable( Frame::MenuEvent::HEXCREATEPERMAF, mode );
	menu->Enable( Frame::MenuEvent::HEXCREATETEMPAF, mode );
	menu->Enable( Frame::MenuEvent::REPAIRPORT, mode );
	menu->Enable( Frame::MenuEvent::REPAIR, mode );
	menu->Enable( Frame::MenuEvent::HEXREGAUGE, mode );
	menu->Enable( Frame::MenuEvent::HEXHITRAIL, mode );

	// move
	menu->Enable( Frame::MenuEvent::HEXUNITMOVER, mode );
	menu->Enable( Frame::MenuEvent::EVTACTICAL, mode );
	menu->Enable( Frame::MenuEvent::EVADMIN, mode );
	menu->Enable( Frame::MenuEvent::EVSTRATEGIC, mode );
	menu->Enable( Frame::MenuEvent::UNITEAST, mode );
	menu->Enable( Frame::MenuEvent::UNITNORTHEAST, mode );
	menu->Enable( Frame::MenuEvent::UNITSOUTHEAST, mode );
	menu->Enable( Frame::MenuEvent::UNITWEST, mode );
	menu->Enable( Frame::MenuEvent::UNITNORTHWEST, mode );
	menu->Enable( Frame::MenuEvent::UNITSOUTHWEST, mode );
	menu->Enable( Frame::MenuEvent::TOOVERSTACK, mode );
	menu->Enable( Frame::MenuEvent::FROVERSTACK, mode );
	menu->Enable( Frame::MenuEvent::UNITTOTRAIN, mode );
	menu->Enable( Frame::MenuEvent::UNITFROMTRAIN, mode );
	menu->Enable( Frame::MenuEvent::TOSHIP, mode );
	menu->Enable( Frame::MenuEvent::FROMSHIP, mode );
	menu->Enable( Frame::MenuEvent::MPPLUS, mode );
	menu->Enable( Frame::MenuEvent::MPMINUS, mode );
	menu->Enable( Frame::MenuEvent::UNITHOME, mode );
	//menu->Enable(CM_RAILUSAGE, mode);

	// air
	menu->Enable( Frame::MenuEvent::TAKEOFF, mode );
	menu->Enable( Frame::MenuEvent::STAGE, mode );
	menu->Enable( Frame::MenuEvent::RESOLVEAIRCOMBAT, mode );
	menu->Enable( Frame::MenuEvent::BOMB, mode );
	menu->Enable( Frame::MenuEvent::FIREAA, mode );
	menu->Enable( Frame::MenuEvent::RESOLVEBOMBING, mode );
	menu->Enable( Frame::MenuEvent::LAND, mode );
	menu->Enable( Frame::MenuEvent::CAPTOINTERCEPTION, mode );
	menu->Enable( Frame::MenuEvent::CAPTOESCORT, mode );

	// misc
	menu->Enable( Frame::MenuEvent::HEXCOMBATINFO, mode );
	menu->Enable( Frame::MenuEvent::CANCELATTACK, mode );
	menu->Enable( Frame::MenuEvent::CANCELAIR, mode );
	menu->Enable( Frame::MenuEvent::OPTIONSWEATHER, mode );
	// owner menu is submenu of the misc menu
	menu->Enable( Frame::MenuEvent::VISIBLEAXIS, mode );
	menu->Enable( Frame::MenuEvent::VISIBLEALLIED, mode );
	menu->Enable( Frame::MenuEvent::VISIBLENEUTRAL, mode );
	menu->Enable( Frame::MenuEvent::ALLAXIS, mode );
	menu->Enable( Frame::MenuEvent::ALLALLIED, mode );
	menu->Enable( Frame::MenuEvent::ALLNEUTRAL, mode );
	menu->Enable( Frame::MenuEvent::CHANGEORIGOWNERALL, mode );
	menu->Enable( Frame::MenuEvent::CHANGEORIGOWNERVISIBLE, mode );
	menu->Enable( Frame::MenuEvent::GAUGE_ALL, mode );
	menu->Enable( Frame::MenuEvent::GAUGE_VISIBLE, mode );
	menu->Enable( Frame::MenuEvent::CLEAR_ALL, mode );
	menu->Enable( Frame::MenuEvent::CLEAR_VISIBLE, mode );
	menu->Enable( Frame::MenuEvent::ATTACKSUP, mode );
	menu->Enable( Frame::MenuEvent::SUPPLY, mode );
	menu->Enable( Frame::MenuEvent::UPDATESUPPLY, mode );
	menu->Enable( Frame::MenuEvent::CHGISOLATED, mode );

	if ( _current_phase == Phase::NO_GAME || _current_phase == Phase::SETUP_PHASE )
		return;

	// file
	menu->Enable( Frame::MenuEvent::FILESAVE, true );
	menu->Enable( Frame::MenuEvent::FILESAVEAS, true );
	menu->Enable( Frame::MenuEvent::FILEINSERT, true );

	// options always enabled
	menu->Enable( Frame::MenuEvent::COMMANDSENDPHASE, true );
	menu->Enable( Frame::MenuEvent::OPTIONSSETUPMODE, true );
	menu->Enable( Frame::MenuEvent::CHANGESIDE, true );
	menu->Enable( Frame::MenuEvent::TIMEJUMP, true );
	menu->Enable( Frame::MenuEvent::DIEROLL, true );
	menu->Enable( Frame::MenuEvent::DIEROLL2, true );
	menu->Enable( Frame::MenuEvent::DIEROLL12, true );
	menu->Enable( Frame::MenuEvent::DIEROLL100, true );
	menu->Enable( Frame::MenuEvent::OPTIONSREPAINT, true );
	menu->Enable( Frame::MenuEvent::HEXOWNER, true );
	menu->Enable( Frame::MenuEvent::SHOWPATH, true );
	menu->Enable( Frame::MenuEvent::SHOWCRT, true );
	menu->Enable( Frame::MenuEvent::CHGCRT, true );
	menu->Enable( Frame::MenuEvent::SHOWATTACKMARKERS, true );
	menu->Enable( Frame::MenuEvent::REMOVECMARKERS, true );
	menu->Enable( Frame::MenuEvent::ALPARTISANS, true );
	menu->Enable( Frame::MenuEvent::AXPARTISANS, true );
	menu->Enable( Frame::MenuEvent::PEEKPARTISANS, true );
	menu->Enable( Frame::MenuEvent::REPLPOOL, true );
	menu->Enable( Frame::MenuEvent::AIRREPLPOOL, true );
	menu->Enable( Frame::MenuEvent::CALCAXIS, true );
	menu->Enable( Frame::MenuEvent::CALCALLIED, true );
	menu->Enable( Frame::MenuEvent::AXISLOSS, true );
	menu->Enable( Frame::MenuEvent::ANALYZE_DIE, true );
	menu->Enable( Frame::MenuEvent::OPTIONSDISPLAY, true );
	menu->Enable( Frame::MenuEvent::OPTIONSRULES, true );
	menu->Enable( Frame::MenuEvent::CHANGETEC, true );
	menu->Enable( Frame::MenuEvent::MENU_SET, true );

	// hilite menu always enabled
	menu->Enable( Frame::MenuEvent::ZOOMIN, true );
	menu->Enable( Frame::MenuEvent::ZOOMOUT, true );
	menu->Enable( Frame::MenuEvent::ZOOM100, true );
	menu->Enable( Frame::MenuEvent::HIDEUNITS, true );
	menu->Enable( Frame::MenuEvent::SEARCHUNIT, true );
	menu->Enable( Frame::MenuEvent::SEARCHAGAIN, true );
	menu->Enable( Frame::MenuEvent::HILITEMOT, true );
	menu->Enable( Frame::MenuEvent::HILITEARMOR, true );
	menu->Enable( Frame::MenuEvent::HILITEARTILLERY, true );
	menu->Enable( Frame::MenuEvent::HILITECONS, true );
	menu->Enable( Frame::MenuEvent::HILITERRENG, true );
	menu->Enable( Frame::MenuEvent::HILITETRUCKS, true );
	menu->Enable( Frame::MenuEvent::HILITESTACKS, true );
	menu->Enable( Frame::MenuEvent::HILITECADRES, true );
	menu->Enable( Frame::MenuEvent::HILITEPLANES, true );
	menu->Enable( Frame::MenuEvent::HILITEFLYINGPLANES, true );
	menu->Enable( Frame::MenuEvent::HILITEABORTED, true );
	menu->Enable( Frame::MenuEvent::HILITEAIRBASE, true );
	menu->Enable( Frame::MenuEvent::HILITEINOP, true );
	menu->Enable( Frame::MenuEvent::SHOW_DISRUPTED, true );
	menu->Enable( Frame::MenuEvent::SHOW_INTZONES1, true );
	menu->Enable( Frame::MenuEvent::SHOW_INTZONES2, true );
	menu->Enable( Frame::MenuEvent::SHOW_INTZONES3, true );
	menu->Enable( Frame::MenuEvent::SHOW_INTZONES4, true );
	menu->Enable( Frame::MenuEvent::SHOW_PAZONES, true );
	menu->Enable( Frame::MenuEvent::SHOWHITS, true );
	menu->Enable( Frame::MenuEvent::HILITENORMAL, true );
	menu->Enable( Frame::MenuEvent::HILITEWIDE, true );
	menu->Enable( Frame::MenuEvent::HILITEOWNNORMAL, true );
	menu->Enable( Frame::MenuEvent::HILITEOWNWIDE, true );
	menu->Enable( Frame::MenuEvent::HILITEENEMYZOC, true );
	menu->Enable( Frame::MenuEvent::HILITEFULLENEMYZOC, true );
	menu->Enable( Frame::MenuEvent::HILITEREDUCEDENEMYZOC, true );

	// search submenu
	//menu->Enable( MENU_SEARCH, true );
	//menu->Enable( Frame::MenuEvent::SEARCHHEX, true );
	//menu->Enable( Frame::MenuEvent::SEARCHCITY, true );
	menu->Enable( Frame::MenuEvent::SEARCHUNIT, true );
	menu->Enable( Frame::MenuEvent::SEARCHAGAIN, true );

	// Weather Zone submenu
	menu->Enable( Frame::MenuEvent::HILITEWZA, true );
	menu->Enable( Frame::MenuEvent::HILITEWZB, true );
	menu->Enable( Frame::MenuEvent::HILITEWZC, true );
	menu->Enable( Frame::MenuEvent::HILITEWZD, true );
	menu->Enable( Frame::MenuEvent::HILITEWZE, true );
	menu->Enable( Frame::MenuEvent::HILITEWZF, true );
	menu->Enable( Frame::MenuEvent::HILITEWZG, true );
	menu->Enable( Frame::MenuEvent::HILITEWZH1, true );
	menu->Enable( Frame::MenuEvent::HILITEWZH2, true );
	menu->Enable( Frame::MenuEvent::HILITEWZH3, true );
	menu->Enable( Frame::MenuEvent::HILITEWZI1, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ1, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ2, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ3, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ4, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ5, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ6, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ7, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ8, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ9, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ10, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ11, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ12, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ13, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ14, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ15, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ16, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ17, true );
	menu->Enable( Frame::MenuEvent::HILITEWZ18, true );
	menu->Enable( Frame::MenuEvent::HILITEWZI2, true );
	menu->Enable( Frame::MenuEvent::HILITEWZI3, true );
	menu->Enable( Frame::MenuEvent::HILITEWZIS, true );
	menu->Enable( Frame::MenuEvent::HILITEWZJ1, true );
	menu->Enable( Frame::MenuEvent::HILITEWZJ2, true );
	menu->Enable( Frame::MenuEvent::HILITEWZK, true );
	menu->Enable( Frame::MenuEvent::HILITEWZL, true );
	menu->Enable( Frame::MenuEvent::HILITEWZM, true );
	menu->Enable( Frame::MenuEvent::HILITEWZN, true );
	menu->Enable( Frame::MenuEvent::HILITEWZO, true );
	menu->Enable( Frame::MenuEvent::HILITEWZP1, true );
	menu->Enable( Frame::MenuEvent::HILITEWZP2, true );
	menu->Enable( Frame::MenuEvent::HILITEWZQ, true );

	// disruption cmds always available
	menu->Enable( Frame::MenuEvent::EVNOT_DISRUPTED, true );
	menu->Enable( Frame::MenuEvent::EVDISRUPTED, true );
	menu->Enable( Frame::MenuEvent::BAD_DISRUPTED, true );
	menu->Enable( Frame::MenuEvent::REMOVE_DISRUPTION, true );

	// can always check rail usage:
#if 0
	menu->Enable( Frame::MenuEvent::RAILUSAGE, true );
#endif
#if 0
	if ( Phase == Phase::SETUP_PHASE )
		optionsmenu->Append( CM_OPTIONSSETUPMODE, "Exit &Setup Mode" );
	else
		optionsmenu->Append( CM_OPTIONSSETUPMODE, "Enter &Setup Mode" );

	if ( Player == RealPlayer )
		optionsmenu->Append( CM_CHANGESIDE, "Change To &Non-phasing Player" );
	else
		optionsmenu->Append( CM_CHANGESIDE, "Change To &Phasing Player" );

	if ( CombatMarkersVisible )
		optionsmenu->Append( CM_SHOWATTACKMARKERS, "Hide &Combat Markers..." );
	else
		optionsmenu->Append( CM_SHOWATTACKMARKERS, "Show &Combat Markers..." );
#endif
	switch ( _current_phase )
	{
		case Phase::SETUP_PHASE:
			//Append(optionsmenu, MF_ENABLED, CM_CHANGESIDE, "&Change Side");
		case Phase::INITIAL_PHASE:
			// hex
			menu->Enable( Frame::MenuEvent::HEXINFO, true );
			menu->Enable( Frame::MenuEvent::STACKINFO, true );
			menu->Enable( Frame::MenuEvent::ADDNOTE, true );
			//menu->Enable( Frame::MenuEvent::ADDALERT, true );
			menu->Enable( Frame::MenuEvent::ADDSTATUSMARKER, true );
			menu->Enable( Frame::MenuEvent::COMMANDSCHANGEOWNER, true );
			menu->Enable( Frame::MenuEvent::CHANGEORIGOWNER, true );
			menu->Enable( Frame::MenuEvent::MAKENEUTRAL, true );
			menu->Enable( Frame::MenuEvent::CHGSUPPLY, true );
			menu->Enable( Frame::MenuEvent::SPECIALSUPPLY, true );
			menu->Enable( Frame::MenuEvent::ISOLATED, true );
			menu->Enable( Frame::MenuEvent::HEXHITRAIL, true );
			menu->Enable( Frame::MenuEvent::REPAIRBRIDGE, true );
			menu->Enable( Frame::MenuEvent::BREAKBRIDGE, true );
			menu->Enable( Frame::MenuEvent::SETAFHITS, true );
			menu->Enable( Frame::MenuEvent::SETAFUSAGE, true );
			menu->Enable( Frame::MenuEvent::SETPORTHITS, true );
			menu->Enable( Frame::MenuEvent::SETPORTUSAGE, true );
			menu->Enable( Frame::MenuEvent::DESTROYPORT, true );
			menu->Enable( Frame::MenuEvent::SETCDHITS, true );
			menu->Enable( Frame::MenuEvent::SETHARASSMENT, true );
#if 0
			menu->Enable( Frame::MenuEvent::SETAXMINES, true );
			menu->Enable( Frame::MenuEvent::SETALMINES, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDCONTESTED, true );
			menu->Enable( Frame::MenuEvent::ADDBARRAGE, true );
			menu->Enable( Frame::MenuEvent::ADDMULBERRY, true );

			// units
			menu->Enable( Frame::MenuEvent::HEXCREATEUNIT, true );
			menu->Enable( Frame::MenuEvent::HEXCREATEAIRUNIT, true );
			menu->Enable( Frame::MenuEvent::HEXCREATESHIP, true );
			menu->Enable( Frame::MenuEvent::RP, true );
			menu->Enable( Frame::MenuEvent::ADDMULBERRY, true );
			menu->Enable( Frame::MenuEvent::HEXEDIT_UNIT, true );
			menu->Enable( Frame::MenuEvent::DESTROYUNIT, true );
			menu->Enable( Frame::MenuEvent::HEXDELETEUNIT, true );
			menu->Enable( Frame::MenuEvent::MOVETOREPLPOOL, true );
			menu->Enable( Frame::MenuEvent::ACTIVATEAIR, true );
			menu->Enable( Frame::MenuEvent::TELEPORT, true );
			menu->Enable( Frame::MenuEvent::CHANGEUNITSIDE, true );
			menu->Enable( Frame::MenuEvent::SUPPLY, true );

			// orders
			menu->Enable( Frame::MenuEvent::CANCELORDER, true );
			menu->Enable( Frame::MenuEvent::COMBAT, true );
			menu->Enable( Frame::MenuEvent::NAVALPATROL, true );
			menu->Enable( Frame::MenuEvent::HEXHITAF, true );
			menu->Enable( Frame::MenuEvent::HEXCREATEFORT, true );
			menu->Enable( Frame::MenuEvent::HEXCREATEPERMAF, true );
			menu->Enable( Frame::MenuEvent::HEXCREATETEMPAF, true );
			menu->Enable( Frame::MenuEvent::REPAIRPORT, true );
			menu->Enable( Frame::MenuEvent::REPAIR, true );
			menu->Enable( Frame::MenuEvent::HEXREGAUGE, true );

			// move
			menu->Enable( Frame::MenuEvent::HEXUNITMOVER, true );
			menu->Enable( Frame::MenuEvent::UNITWEST, true );
			menu->Enable( Frame::MenuEvent::UNITEAST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHWEST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHWEST, true );
			menu->Enable( Frame::MenuEvent::ADVANCE, true );
			menu->Enable( Frame::MenuEvent::MPPLUS, true );
			menu->Enable( Frame::MenuEvent::MPMINUS, true );
			menu->Enable( Frame::MenuEvent::UNITHOME, true );

			// air
			menu->Enable( Frame::MenuEvent::TAKEOFF, true );
			menu->Enable( Frame::MenuEvent::STAGE, true );
			menu->Enable( Frame::MenuEvent::BOMB, true );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				menu->Enable( Frame::MenuEvent::CAPTOINTERCEPTION, true );
				menu->Enable( Frame::MenuEvent::CAPTOESCORT, true );
				menu->Enable( Frame::MenuEvent::RESOLVEAIRCOMBAT, true );
				menu->Enable( Frame::MenuEvent::FIREAA, true );
				menu->Enable( Frame::MenuEvent::RESOLVEBOMBING, true );
			}
			menu->Enable( Frame::MenuEvent::LAND, true );
			menu->Enable( Frame::MenuEvent::CAPTOINTERCEPTION, true );
			menu->Enable( Frame::MenuEvent::CAPTOESCORT, true );

			// misc
			menu->Enable( Frame::MenuEvent::OPTIONSWEATHER, true );
			// owner menu is submenu of the misc menu
			menu->Enable( Frame::MenuEvent::VISIBLEAXIS, true );
			menu->Enable( Frame::MenuEvent::VISIBLEALLIED, true );
			menu->Enable( Frame::MenuEvent::VISIBLENEUTRAL, true );
			menu->Enable( Frame::MenuEvent::ALLAXIS, false );
			menu->Enable( Frame::MenuEvent::ALLALLIED, false );
			menu->Enable( Frame::MenuEvent::ALLNEUTRAL, false );
			menu->Enable( Frame::MenuEvent::CHANGEORIGOWNERALL, true );
			menu->Enable( Frame::MenuEvent::CHANGEORIGOWNERVISIBLE, true );
			menu->Enable( Frame::MenuEvent::GAUGE_ALL, false );
			menu->Enable( Frame::MenuEvent::GAUGE_VISIBLE, true );
			menu->Enable( Frame::MenuEvent::CLEAR_ALL, false );
			menu->Enable( Frame::MenuEvent::CLEAR_VISIBLE, false );
			menu->Enable( Frame::MenuEvent::UPDATESUPPLY, true );
			menu->Enable( Frame::MenuEvent::CHGISOLATED, true );
			break;
		case Phase::NAVAL_PHASE_1:
		case Phase::NAVAL_PHASE_2:
		case Phase::NAVAL_PHASE_3:
		case Phase::NAVAL_PHASE_4:
		case Phase::NAVAL_PHASE_5:
		case Phase::NAVAL_PHASE_6:
		case Phase::NAVAL_PHASE_7:
		case Phase::NAVAL_PHASE_8:
		case Phase::NAVAL_PHASE_9:
		case Phase::NAVAL_PHASE_10:
			// hex
			menu->Enable( Frame::MenuEvent::HEXINFO, true );
			menu->Enable( Frame::MenuEvent::STACKINFO, true );
			menu->Enable( Frame::MenuEvent::ADDNOTE, true );
#if 0
			menu->Enable( Frame::MenuEvent::ADDALERT, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDSTATUSMARKER, true );
#if 0
			menu->Enable( Frame::MenuEvent::SETATTSUPPORT, true );
			menu->Enable( Frame::MenuEvent::SETDEFSUPPORT, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDCONTESTED, true );
			menu->Enable( Frame::MenuEvent::ADDBARRAGE, true );
			menu->Enable( Frame::MenuEvent::ADDMULBERRY, true );

			// move
			menu->Enable( Frame::MenuEvent::HEXUNITMOVER, true );
			menu->Enable( Frame::MenuEvent::EVTACTICAL, true );
			menu->Enable( Frame::MenuEvent::EVADMIN, true );
			menu->Enable( Frame::MenuEvent::EVSTRATEGIC, true );
			menu->Enable( Frame::MenuEvent::UNITEAST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITWEST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHWEST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHWEST, true );
			menu->Enable( Frame::MenuEvent::TOOVERSTACK, true );
			menu->Enable( Frame::MenuEvent::FROVERSTACK, true );
			menu->Enable( Frame::MenuEvent::UNITTOTRAIN, true );
			menu->Enable( Frame::MenuEvent::UNITFROMTRAIN, true );
			menu->Enable( Frame::MenuEvent::TOSHIP, true );
			menu->Enable( Frame::MenuEvent::FROMSHIP, true );
			menu->Enable( Frame::MenuEvent::MPPLUS, true );
			menu->Enable( Frame::MenuEvent::MPMINUS, true );
			menu->Enable( Frame::MenuEvent::UNITHOME, true );

			// air
			menu->Enable( Frame::MenuEvent::TAKEOFF, true );
			menu->Enable( Frame::MenuEvent::STAGE, true );
			menu->Enable( Frame::MenuEvent::BOMB, true );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				menu->Enable( Frame::MenuEvent::CAPTOINTERCEPTION, true );
				menu->Enable( Frame::MenuEvent::CAPTOESCORT, true );
				menu->Enable( Frame::MenuEvent::RESOLVEAIRCOMBAT, true );
				menu->Enable( Frame::MenuEvent::FIREAA, true );
				menu->Enable( Frame::MenuEvent::RESOLVEBOMBING, true );
			}
			menu->Enable( Frame::MenuEvent::LAND, true );
#if 0
			menu->Enable( Frame::MenuEvent::LAYMINE, true );
			menu->Enable( Frame::MenuEvent::SWEEPMINE, true );
#endif
			menu->Enable( Frame::MenuEvent::HEXINFO, true );
			break;
		case Phase::REACT_MOVE_PHASE:
		case Phase::MOVEMENT_PHASE:
			menu->Enable( Frame::MenuEvent::HILITEUNMOVED, true );
			// hex
			menu->Enable( Frame::MenuEvent::HEXINFO, true );
			menu->Enable( Frame::MenuEvent::STACKINFO, true );
			menu->Enable( Frame::MenuEvent::ADDNOTE, true );
#if 0
			menu->Enable( Frame::MenuEvent::ADDALERT, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDSTATUSMARKER, true );
#if 0
			menu->Enable( Frame::MenuEvent::SETATTSUPPORT, true );
			menu->Enable( Frame::MenuEvent::SETDEFSUPPORT, true );
#endif
			menu->Enable( Frame::MenuEvent::ISOLATED, true );
			menu->Enable( Frame::MenuEvent::ADDCONTESTED, true );
			menu->Enable( Frame::MenuEvent::ADDBARRAGE, true );
			menu->Enable( Frame::MenuEvent::ADDMULBERRY, true );

			// move
			menu->Enable( Frame::MenuEvent::HEXUNITMOVER, true );
			menu->Enable( Frame::MenuEvent::EVTACTICAL, true );
			menu->Enable( Frame::MenuEvent::EVADMIN, true );
			menu->Enable( Frame::MenuEvent::EVSTRATEGIC, true );
			menu->Enable( Frame::MenuEvent::UNITEAST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITWEST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHWEST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHWEST, true );
			menu->Enable( Frame::MenuEvent::ADVANCE, true );
			menu->Enable( Frame::MenuEvent::TOOVERSTACK, true );
			menu->Enable( Frame::MenuEvent::FROVERSTACK, true );
			menu->Enable( Frame::MenuEvent::UNITTOTRAIN, true );
			menu->Enable( Frame::MenuEvent::UNITFROMTRAIN, true );
			menu->Enable( Frame::MenuEvent::TOSHIP, true );
			menu->Enable( Frame::MenuEvent::FROMSHIP, true );
			menu->Enable( Frame::MenuEvent::MPPLUS, true );
			menu->Enable( Frame::MenuEvent::MPMINUS, true );
			menu->Enable( Frame::MenuEvent::UNITHOME, true );

			// air
			menu->Enable( Frame::MenuEvent::TAKEOFF, true );
			menu->Enable( Frame::MenuEvent::STAGE, true );
			menu->Enable( Frame::MenuEvent::BOMB, true );
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				menu->Enable( Frame::MenuEvent::CAPTOINTERCEPTION, true );
				menu->Enable( Frame::MenuEvent::CAPTOESCORT, true );
				menu->Enable( Frame::MenuEvent::RESOLVEAIRCOMBAT, true );
				menu->Enable( Frame::MenuEvent::FIREAA, true );
				menu->Enable( Frame::MenuEvent::RESOLVEBOMBING, true );
			}
			menu->Enable( Frame::MenuEvent::LAND, true );

			// orders
			menu->Enable( Frame::MenuEvent::CANCELORDER, true );
			menu->Enable( Frame::MenuEvent::HEXHITRAIL, true );
			menu->Enable( Frame::MenuEvent::HEXHITAF, true );
			menu->Enable( Frame::MenuEvent::HEXCREATEFORT, false );
			menu->Enable( Frame::MenuEvent::HEXCREATEPERMAF, false );
			menu->Enable( Frame::MenuEvent::HEXCREATETEMPAF, true );
			menu->Enable( Frame::MenuEvent::REPAIR, true );
			menu->Enable( Frame::MenuEvent::HEXREGAUGE, true );
			menu->Enable( Frame::MenuEvent::REPAIRBRIDGE, true );
			menu->Enable( Frame::MenuEvent::BREAKBRIDGE, true );

			// misc menu
			menu->Enable( Frame::MenuEvent::HEXCOMBATINFO, true );
			menu->Enable( Frame::MenuEvent::CANCELATTACK, true );
			menu->Enable( Frame::MenuEvent::CANCELAIR, true );
			menu->Enable( Frame::MenuEvent::UPDATESUPPLY, true );
			menu->Enable( Frame::MenuEvent::CHGISOLATED, true );
			menu->Enable( Frame::MenuEvent::ATTACKSUP, _rule_set.AttackSup == TRUE );

			if ( _rule_set.TEM40Overruns == TRUE )
				menu->Enable( Frame::MenuEvent::DESTROYUNIT, true );  // needed with low-odds overruns
			break;
		case Phase::REACTION_PHASE:
			// hex
			menu->Enable( Frame::MenuEvent::HEXINFO, true );
			menu->Enable( Frame::MenuEvent::STACKINFO, true );
			menu->Enable( Frame::MenuEvent::ADDNOTE, true );
#if 0
			menu->Enable( Frame::MenuEvent::ADDALERT, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDSTATUSMARKER, true );
#if 0
			menu->Enable( Frame::MenuEvent::SETATTSUPPORT, true );
			menu->Enable( Frame::MenuEvent::SETDEFSUPPORT, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDCONTESTED, true );
			menu->Enable( Frame::MenuEvent::ADDBARRAGE, true );
			menu->Enable( Frame::MenuEvent::ADDMULBERRY, true );

			// move
			menu->Enable( Frame::MenuEvent::HEXUNITMOVER, true );
			menu->Enable( Frame::MenuEvent::EVTACTICAL, true );
			menu->Enable( Frame::MenuEvent::UNITEAST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITWEST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHWEST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHWEST, true );
			menu->Enable( Frame::MenuEvent::ADVANCE, true );
			menu->Enable( Frame::MenuEvent::TOOVERSTACK, true );
			menu->Enable( Frame::MenuEvent::FROVERSTACK, true );
			menu->Enable( Frame::MenuEvent::MPPLUS, true );
			menu->Enable( Frame::MenuEvent::MPMINUS, true );
			menu->Enable( Frame::MenuEvent::UNITHOME, true );

			// fly
			menu->Enable( Frame::MenuEvent::TAKEOFF, true );
			menu->Enable( Frame::MenuEvent::RESOLVEAIRCOMBAT, true );
			menu->Enable( Frame::MenuEvent::LAND, true );
			menu->Enable( Frame::MenuEvent::UNITHOME, true );
			break;
		case Phase::REACT_COMB_PHASE:
		case Phase::COMBAT_PHASE:
			// hex
			menu->Enable( Frame::MenuEvent::HEXINFO, true );
			menu->Enable( Frame::MenuEvent::STACKINFO, true );
			menu->Enable( Frame::MenuEvent::ADDNOTE, true );
#if 0
			menu->Enable( Frame::MenuEvent::ADDALERT, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDSTATUSMARKER, true );
#if 0
			menu->Enable( Frame::MenuEvent::SETATTSUPPORT, true );
			menu->Enable( Frame::MenuEvent::SETDEFSUPPORT, true );
#endif
			menu->Enable( Frame::MenuEvent::ISOLATED, true );
			menu->Enable( Frame::MenuEvent::ADDCONTESTED, true );
			menu->Enable( Frame::MenuEvent::ADDBARRAGE, true );
			menu->Enable( Frame::MenuEvent::ADDMULBERRY, true );

			// move
			menu->Enable( Frame::MenuEvent::HEXUNITMOVER, true );
			menu->Enable( Frame::MenuEvent::EVTACTICAL, true );
			menu->Enable( Frame::MenuEvent::UNITEAST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITWEST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHWEST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHWEST, true );
			menu->Enable( Frame::MenuEvent::ADVANCE, true );
			menu->Enable( Frame::MenuEvent::TOOVERSTACK, true );
			menu->Enable( Frame::MenuEvent::FROVERSTACK, true );
			menu->Enable( Frame::MenuEvent::UNITHOME, true );

			// orders
			menu->Enable( Frame::MenuEvent::CANCELORDER, true );
			menu->Enable( Frame::MenuEvent::BOMB, true );
			menu->Enable( Frame::MenuEvent::FIREAA, true );
			menu->Enable( Frame::MenuEvent::RESOLVEBOMBING, true );
			menu->Enable( Frame::MenuEvent::COMBAT, true );
			menu->Enable( Frame::MenuEvent::DESTROYUNIT, true );

			// fly
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				menu->Enable( Frame::MenuEvent::TAKEOFF, true );
				menu->Enable( Frame::MenuEvent::STAGE, true );
				menu->Enable( Frame::MenuEvent::CAPTOINTERCEPTION, true );
				menu->Enable( Frame::MenuEvent::CAPTOESCORT, true );
				menu->Enable( Frame::MenuEvent::RESOLVEAIRCOMBAT, true );
				menu->Enable( Frame::MenuEvent::BOMB, true );
				menu->Enable( Frame::MenuEvent::FIREAA, true );
				menu->Enable( Frame::MenuEvent::RESOLVEBOMBING, true );
				menu->Enable( Frame::MenuEvent::LAND, true );
			}

			// misc menu
			menu->Enable( Frame::MenuEvent::HEXCOMBATINFO, true );
			menu->Enable( Frame::MenuEvent::ATTACKSUP, true );
			menu->Enable( Frame::MenuEvent::CANCELATTACK, true );
			menu->Enable( Frame::MenuEvent::CANCELAIR, true );
			menu->Enable( Frame::MenuEvent::UPDATESUPPLY, true );
			menu->Enable( Frame::MenuEvent::CHGISOLATED, true );

			break;
		case Phase::EXPLOITATION_PHASE:
			menu->Enable( Frame::MenuEvent::HILITEUNMOVED, true );
			// hex
			menu->Enable( Frame::MenuEvent::HEXINFO, true );
			menu->Enable( Frame::MenuEvent::STACKINFO, true );
			menu->Enable( Frame::MenuEvent::ADDNOTE, true );
#if 0
			menu->Enable( Frame::MenuEvent::ADDALERT, true );
#endif
			menu->Enable( Frame::MenuEvent::ADDSTATUSMARKER, true );
			menu->Enable( Frame::MenuEvent::ADDCONTESTED, true );
			menu->Enable( Frame::MenuEvent::ADDBARRAGE, true );
			menu->Enable( Frame::MenuEvent::ADDMULBERRY, true );

			// move
			menu->Enable( Frame::MenuEvent::HEXUNITMOVER, true );
			menu->Enable( Frame::MenuEvent::EVTACTICAL, true );
			menu->Enable( Frame::MenuEvent::UNITEAST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHEAST, true );
			menu->Enable( Frame::MenuEvent::UNITWEST, true );
			menu->Enable( Frame::MenuEvent::UNITNORTHWEST, true );
			menu->Enable( Frame::MenuEvent::UNITSOUTHWEST, true );
			menu->Enable( Frame::MenuEvent::TOOVERSTACK, true );
			menu->Enable( Frame::MenuEvent::FROVERSTACK, true );
			menu->Enable( Frame::MenuEvent::TOSHIP, true );
			menu->Enable( Frame::MenuEvent::FROMSHIP, true );
			menu->Enable( Frame::MenuEvent::MPPLUS, true );
			menu->Enable( Frame::MenuEvent::MPMINUS, true );
			menu->Enable( Frame::MenuEvent::UNITHOME, true );
			menu->Enable( Frame::MenuEvent::ADVANCE, true );

			// fly
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				menu->Enable( Frame::MenuEvent::TAKEOFF, true );
				menu->Enable( Frame::MenuEvent::STAGE, true );
				menu->Enable( Frame::MenuEvent::RESOLVEAIRCOMBAT, true );
				menu->Enable( Frame::MenuEvent::BOMB, true );
				menu->Enable( Frame::MenuEvent::FIREAA, true );
				menu->Enable( Frame::MenuEvent::RESOLVEBOMBING, true );
			}
			menu->Enable( Frame::MenuEvent::LAND, true );

			// orders
			menu->Enable( Frame::MenuEvent::HEXHITRAIL, true );
			menu->Enable( Frame::MenuEvent::HEXHITAF, true );
			menu->Enable( Frame::MenuEvent::HEXREGAUGE, false );
			break;
		default:
			wxMessageBox( wxT("Illegal phase, unable to setup menu"), Application::NAME );
			return;
	}	// switch( Phase )
	SetMarkerButtons( );
}

void Frame::SetMoveModeButtons( )
{
	wxToolBar* ToolBar = GetToolBar( );
	wxMenuBar* menu = GetMenuBar( );

	if ( ! ToolBar || ! menu )
		return;
	if ( MoveMode == MovementMode::TACTICAL )
	{
		ToolBar->ToggleTool( Frame::MenuEvent::EVTACTICAL, true );
		ToolBar->ToggleTool( Frame::MenuEvent::EVADMIN, false );
		ToolBar->ToggleTool( Frame::MenuEvent::EVSTRATEGIC, false );
		menu->Check( Frame::MenuEvent::EVTACTICAL, true );
		menu->Check( Frame::MenuEvent::EVADMIN, false );
		menu->Check( Frame::MenuEvent::EVSTRATEGIC, false );
	}
	else if ( MoveMode == MovementMode::STRATEGIC )
	{
		ToolBar->ToggleTool( Frame::MenuEvent::EVTACTICAL, false );
		ToolBar->ToggleTool( Frame::MenuEvent::EVADMIN, false );
		ToolBar->ToggleTool( Frame::MenuEvent::EVSTRATEGIC, true );
		menu->Check( Frame::MenuEvent::EVTACTICAL, false );
		menu->Check( Frame::MenuEvent::EVADMIN, false );
		menu->Check( Frame::MenuEvent::EVSTRATEGIC, true );
	}
	else if ( MoveMode == MovementMode::ADMIN )
	{
		ToolBar->ToggleTool( Frame::MenuEvent::EVTACTICAL, false );
		ToolBar->ToggleTool( Frame::MenuEvent::EVADMIN, true );
		ToolBar->ToggleTool( Frame::MenuEvent::EVSTRATEGIC, false );
		menu->Check( Frame::MenuEvent::EVTACTICAL, false );
		menu->Check( Frame::MenuEvent::EVADMIN, true );
		menu->Check( Frame::MenuEvent::EVSTRATEGIC, false );
	}
	map_->set_move_mode( MoveMode );
}

void Frame::SetMarkerButtons( )
{
	wxToolBar* ToolBar = GetToolBar( );
	wxMenuBar* menu = GetMenuBar( );

	if ( ! ToolBar || ! menu )
		return;

	ToolBar->ToggleTool( Frame::MenuEvent::HEXOWNER, map_->show_owner_ );
	menu->Check( Frame::MenuEvent::HEXOWNER, map_->show_owner_ );

	ToolBar->ToggleTool( Frame::MenuEvent::SHOWPATH, _show_path );
	menu->Check( Frame::MenuEvent::SHOWPATH, _show_path );

	ToolBar->ToggleTool( Frame::MenuEvent::SHOWATTACKMARKERS, _show_combat_markers );
	menu->Check( Frame::MenuEvent::SHOWATTACKMARKERS, _show_combat_markers );

	ToolBar->ToggleTool( Frame::MenuEvent::ALPARTISANS, _rule_set.HideAlPartisans == FALSE );
	ToolBar->ToggleTool( Frame::MenuEvent::AXPARTISANS, _rule_set.HideAxPartisans == FALSE );
	menu->Check( Frame::MenuEvent::ALPARTISANS, _rule_set.HideAlPartisans == FALSE );
	menu->Check( Frame::MenuEvent::AXPARTISANS, _rule_set.HideAxPartisans == FALSE );
}

// in format "II Jun 41"
static void create_turn_string( char* msg )
{
	const char* turn_format;
	switch ( strlen( _turn_numeral[_turn - 1] ) )
	{
		case 4:
			turn_format = "%s%s%d";
			break;
		case 3:
			turn_format = "%s%s %d";
			break;
		default:  // 1 or 2
			turn_format = " %s %s %d";
			break;
	}

	sprintf( msg, turn_format, _turn_numeral[_turn-1], _month_string[_month], _year );
}

// in format "Axis Combat"
static void create_phase_string( char* msg )
{
	int p = _phasing_player;

	// but, in reaction phases print non-phasing player:
	if ( _current_phase == Phase::REACT_COMB_PHASE || _current_phase == Phase::REACT_MOVE_PHASE )
		p = _phasing_player == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS;

	if ( _current_phase > Phase::LASTPHASE )
		sprintf( msg, "%s [%d]", _player_string[p], _current_phase );
	else
		sprintf( msg, "%s %s", _player_string[p], _phase_string[_current_phase] );
}

#endif

void Frame::setCaption( )
{
	//char msg[200];
#if defined HEXTOOLSMAP
	wxString msg( Application::NAME + " " + Application::VERSION + " - " + map_->filename( ) );
	//sprintf( msg, Application::NAME.c_str( )" "Application::VERSION.c_str( )" - %s", map_->getFileName( ) );
#elif defined HEXTOOLSPLAY
	wxString msg( Application::NAME + " " + Application::VERSION + " - " + ( _current_player == SidePlayer::AXIS ? "Axis" : "Allied" ) + " playing - " + scnFileName );
	//sprintf( msg, APP_STR" "VER_STR" - %s playing - %s", ( _current_player == SidePlayer::AXIS ? "Axis" : "Allied" ), scnFileName.ToAscii() );
#endif
	SetTitle( msg );
	//SetTitle( wxString::FromAscii( msg ) );
}

bool Frame::loadFiles( const char* filename )
{
	if ( ! map_->load_files( filename ) ) // .map from HT-m, .scn from HT-p
		return false;

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	scnFileName = wxS( filename );
	_phasing_player = _current_player;

	char turn[30];
	create_turn_string( turn );

	char phase[30];
	create_phase_string( phase );

	info_pane_->displayTurnInfo( turn, phase );

	setup_toolbar( );
	setCaption( );

	wxString ws = _recent_file1;
	_recent_file1 = wxS( filename );
	if ( ws.Cmp( wxS( filename ) ) != 0 )
	{
		wxString ws2 = _recent_file2;
		_recent_file2 = ws;
		if ( ws2.Cmp( wxS( filename ) ) != 0 )
		{
			ws = _recent_file3;
			_recent_file3 = ws2;
			if ( ws.Cmp( wxS( filename ) ) != 0 )
			{
				ws2 = _recent_file4;
				_recent_file4 = ws;
				if ( ws2.Cmp( wxS( filename ) ) != 0 )
					_recent_file5 = ws2;
			}
		}
	}

	SetupMenu( );

	saveSettings( );

	wxString logfname;
	logfname.Printf( "%sjet.log", pathname( filename ) );
	wxFileName logfilename = logfname;
	if ( ! logfilename.FileExists( ) )
		_htlog.Create( logfilename.GetFullPath( ) );
	_htlog.Open( logfilename.GetFullPath( ) );

#endif

	return true;
}

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
void Frame::OnNew( wxCommandEvent& WXUNUSED(event) )
{
	// 1. collect user input (filenames)
	DNewGame dlg( this );
	if ( dlg.ShowModal( ) != wxID_OK )
		return;

	// .map, .gcrt, and .tec filenames (w/o path) injected to data structure NewGameDlgData
	dlg.CloseDialog( );

	// load external files:
	char map_filename[100];
	strcpy( map_filename, _new_game_dialog_data.editMap );
	if ( ! map_->initialize_scenario( map_filename ) )
	{
		wxMessageBox( wxT( "Failed to initialize a new game" ), Application::NAME );
		return;
	}

	map_->set_CRT_name( _new_game_dialog_data.editCrt );	// basename only
	map_->initialize_CRT( wxStandardPaths::Get( ).GetExecutablePath( ) );

	map_->set_TEC_name( _new_game_dialog_data.editTec );	// basename only
	map_->initialize_TEC( wxStandardPaths::Get( ).GetExecutablePath( ) );

	// 2. select starting turn
	_year = 41;	// default 1941 year
	_month = 5;	// default June month
	_turn = 2;	// default II turn

	_turns_per_month = 2;	// default 2 turns per month

	DTimeJump tdlg( this );
	if ( tdlg.ShowModal( ) != wxID_OK )
		return;
	tdlg.CloseDialog( );  // collect results kludge

	// 3. select rules
	wxCommandEvent ev;
	CmOptionsrules( ev );

	// turn:
	_current_player = _phasing_player = _turn_dialog_data.cmbPlayer;
	_year = _turn_dialog_data.cmbYear;
	_month = _turn_dialog_data.cmbMonth;
	_turn = _turn_dialog_data.cmbTurn;

	// TODO:  why do the below instead of just Phase = INITIAL_PHASE; ?
	_current_phase = Phase::NO_GAME;
	wxCommandEvent e;
	CmEndPhase( e );  // end NO_GAME phase

	map_->Refresh( );  // repaint map

	setCaption( );
}

void Frame::OnInsert( wxCommandEvent& WXUNUSED(event) )
{
	_unit_editor_active = true;

	DInsertScenario dlg( this );
	if ( dlg.ShowModal( ) == wxID_OK )
		map_->insert_scenario( dlg.GetFileName( ).ToAscii( ), dlg.GetX( ), dlg.GetY( ) );

	_unit_editor_active = false;
}
#endif

int Frame::getSeaCircleDistrictId( wxString seacirc )
{
	return seacircles_[ seacirc ];
}

wxString Frame::getSeaCircleDistrict( int scId )
{
	return seacircles2_[ scId ];
}

int Frame::getLandDistrictId( wxString dist )
{
	return districts_[ dist ];
}

wxString Frame::getLandDistrict( int distId )
{
	return districts2_[ distId ];
}

#if defined HEXTOOLSPLAY
int Frame::getSeaZoneId( wxString seazone )
{
	return seazones_[ seazone ];
}

wxString Frame::getSeaZone( int szId )
{
	return seazones2_[ szId ];
}

int Frame::getCountryId( wxString country )
{
	return countries_[ country ];
}

wxString Frame::getCountry( int countryId )
{
	return countries2_[ countryId ];
}
#endif

void Frame::OnOpen( wxCommandEvent& WXUNUSED(event) )
{
#if defined HEXTOOLSMAP
	// reset terrain selector to no selection, and thus no painting on left click
	terrain_selector_->setSelectedType( HexType::TERRAINTYPECOUNT );

	wxCommandEvent infoPaneEvent( wxEVT_UNSELECT_TERRAINTYPE, GetId() );
	infoPaneEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent( infoPaneEvent );
#elif defined HEXTOOLSPLAY
	_unit_editor_active = true;
#endif

	wxFileDialog dialog(	this,
#if defined HEXTOOLSMAP
							wxT("Open map"),
#elif defined HEXTOOLSPLAY
							wxT("Open scenario"),
#endif
							wxT(""),
							wxT(""),
							Frame::FILE_TYPES,
							0 );

	bool good_load = true;
	if ( dialog.ShowModal( ) == wxID_OK )
		good_load = loadFiles( dialog.GetPath( ).ToAscii( ) );
#if 0
		map->loadMap( dialog.GetPath( ).ToAscii( ) );
		loadScenario( dialog.GetPath( ).ToAscii( ) );
#endif
	if ( ! good_load )
	{
		wxMessageBox( wxT("bad file load!... ") + dialog.GetPath( ) );
		wxEndBusyCursor( );
	}

#if defined HEXTOOLSPLAY
	_unit_editor_active = false;
#endif
}

void Frame::OnSave( wxCommandEvent& event )
{
	if ( map_->filename_defined( ) )
#if defined HEXTOOLSMAP
		map_->save_map( (char*)nullptr );
#elif defined HEXTOOLSPLAY
		map_->save_scenario( (char*)nullptr );
#endif
	else
		OnSaveAs( event );
}

void Frame::OnSaveAs( wxCommandEvent& WXUNUSED(event) )
{
#if defined HEXTOOLSMAP
	saveMap( );
#elif defined HEXTOOLSPLAY
	_unit_editor_active = true;
	saveScenario( );
	_unit_editor_active = false;
#endif
}

#if defined HEXTOOLSMAP
void Frame::saveMap( )
{
	wxFileDialog dialog(	this,
							wxT("Save map"),
							wxT(""),
							map_->filename_defined( ) ? map_->filename( ) : wxT("default.map"),
							Frame::FILE_TYPES,
							wxSAVE | wxOVERWRITE_PROMPT );

	if ( dialog.ShowModal( ) == wxID_OK )
	{
		// TODO:  static ??
		static char name[255];
		strcpy( name, dialog.GetPath( ).ToAscii( ) );
		map_->save_map( name );
#if 0
		setCaption( ); // already done at end of saveMap(..)
#endif
	}
}
#endif

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
bool Frame::saveScenario( )
{
	if ( _current_phase == Phase::NO_GAME )
		return false;

	wxFileDialog dialog(	this,
							wxT("Save scenario"),
							wxT(""),
// TODO: consider this from Frame::saveMap(..)
#if 0
							map_->filename_defined( ) ? map_->filename( ) : wxT( "default.scn" ),
#endif
							wxT("default.scn"),
							Frame::FILE_TYPES,
							wxSAVE | wxOVERWRITE_PROMPT );

	if ( dialog.ShowModal( ) == wxID_OK )
	{
		// TODO:  static ??
		static char name[255];
		strcpy( name, dialog.GetPath( ).ToAscii( ) );
		if ( map_->save_scenario( name ) )
		{
			setCaption( );
			return true;
		}
	}

	return false;
}

void Frame::OnRecent1( wxCommandEvent& WXUNUSED(e) )
{
	loadFiles( _recent_file1.ToAscii( ) );
}

void Frame::OnRecent2( wxCommandEvent& WXUNUSED(e) )
{
	loadFiles( _recent_file2.ToAscii( ) );
}

void Frame::OnRecent3( wxCommandEvent& WXUNUSED(e) )
{
	loadFiles( _recent_file3.ToAscii( ) );
}

void Frame::OnRecent4( wxCommandEvent& WXUNUSED(e) )
{
	loadFiles( _recent_file4.ToAscii( ) );
}

void Frame::OnRecent5( wxCommandEvent& WXUNUSED(e) )
{
	loadFiles( _recent_file5.ToAscii( ) );
}
#endif

void Frame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
	Close( false );  // 'true' would force the frame to close
}

void Frame::OnCloseQuery( wxCloseEvent& event )
{
	wxMessageDialog dlg(	this,
							Frame::CLOSE_WO_SAVE,
							Application::NAME,
							wxYES_NO | wxICON_QUESTION
						);

	// if data are dirty (this is the only known check/use for dirty flag),
	//	then give user opportunity to cancel out of the file close
	if ( map_->file_dirty( ) && dlg.ShowModal( ) != wxID_YES )
		event.Veto( );
	else
		Destroy( );
}

void Frame::OnRepaint( wxCommandEvent& WXUNUSED(event) )
{
#if defined HEXTOOLSMAP
	map_->Refresh( false );
#elif defined HEXTOOLSPLAY
	map_->rePaint( );
#endif
}

void Frame::OnZoomIn( wxCommandEvent& WXUNUSED(event) )
{
	map_->zoom( -1 );
}

void Frame::OnZoomOut( wxCommandEvent& WXUNUSED(event) )
{
	map_->zoom( 1 );
}

void Frame::OnZoom100( wxCommandEvent& WXUNUSED(event) )
{
	map_->zoom( 0 );
}

#if defined HEXTOOLSMAP
void Frame::OnWZMode( wxCommandEvent& WXUNUSED(event) )
{
	map_->set_weather_zone_mode( );
	wxControl* selector = GetToolBar()->FindControl( CMD_WZCHOICE );
	selector->Enable( true );
	map_->set_weather_zone( static_cast<wxChoice*>( selector )->GetSelection() );
	map_->Refresh( );
}

void Frame::OnWZSelection( wxCommandEvent& WXUNUSED(event) )
{
	wxControl* selector = GetToolBar()->FindControl( CMD_WZCHOICE );
	map_->set_weather_zone( static_cast<wxChoice*>( selector )->GetSelection() );
}

void Frame::OnSeaboxMode( wxCommandEvent& WXUNUSED(event) )
{
	map_->set_seacircle_mode( );
	GetToolBar()->FindControl( CMD_SEABOXCHOICE )->Enable( true );
	SetStatusText( "Sea circle mode" );
	map_->Refresh( );
}

void Frame::OnSeaboxSelection( wxCommandEvent& WXUNUSED(event) )
{
	wxChoice* selector = static_cast<wxChoice*>( GetToolBar()->FindControl( CMD_SEABOXCHOICE ) );
	map_->set_current_seacircle( selector->GetLabel( ) );
}

void Frame::OnDistrictMode( wxCommandEvent& WXUNUSED(event) )
{
	map_->set_district_mode( );
	GetToolBar()->FindControl( CMD_DISTRICTCHOICE )->Enable( );
	SetStatusText( "Land district mode" );
	map_->Refresh( );
}

void Frame::OnDistrictSelection( wxCommandEvent& WXUNUSED(event) )
{
	wxChoice* selector = static_cast<wxChoice*>( GetToolBar()->FindControl( CMD_DISTRICTCHOICE ) );
	map_->set_current_district( selector->GetLabel( ) );
}

void Frame::OnMapLeafMode( wxCommandEvent& event )
{
	// forward this event to the MapPane version
	//	(when map leaf mode is triggered via right-click menu option)
	map_->CmMapleafmode( event );
}

void Frame::OnEndModes( wxCommandEvent& WXUNUSED(event) )
{
	map_->clear_all_modes( );
	SetStatusText( "" );
	map_->Refresh( );
}
#endif

void Frame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
#if defined HEXTOOLSMAP
	wxString msg( Application::NAME );
	msg += _(" ");
	msg += wxT( Application::VERSION );
	msg += _("\n");
	msg += wxT( Application::AUTHOR );

	wxMessageBox( msg, "About " + Application::NAME, wxOK | wxICON_INFORMATION, this );
#elif defined HEXTOOLSPLAY
	DAbout dlg( this );
	dlg.ShowModal( );
#endif
}

#if defined HEXTOOLSPLAY
void Frame::OnHomePage( wxCommandEvent& WXUNUSED(event) )
{
	int e = (int)ShellExecute(	nullptr,
								"open",
								"http://groups.yahoo.com/group/jet-users",
								nullptr,
								nullptr,  // help // pointer to string that specifies default directory
								SW_SHOWNORMAL );

	if ( e < 32 )
	{
		char s[200];
		char bf[200];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, e, 0, bf, 200, 0 );
		sprintf( s, "%s Failed to open web browser: %s\n", HELP_CMD, bf );
		wxMessageBox( s, "Error", wxOK | wxICON_INFORMATION );
	}
}
#endif

void Frame::OnHelp( wxCommandEvent& WXUNUSED(event) )
{
#if defined HEXTOOLSMAP
	system( HELP_CMD ); // calls maphelp.bat on Help | Contents... invocation
#elif defined HEXTOOLSPLAY
	int e = (int)ShellExecute(	nullptr,
								"open",
								//"help\\jet.htm",
								HELP_CMD,
								nullptr,
								nullptr,  // help // pointer to string that specifies default directory
								SW_SHOWNORMAL );

	if ( e < 32 )
	{
		char s[200];
		char bf[200];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, e, 0, bf, 200, 0 );
		sprintf( s, "Failed to open help file '%s': %s\n", HELP_CMD, bf );
		wxMessageBox( s, "Error", wxOK | wxICON_INFORMATION );
	}
#endif
}

void Frame::OnSize( wxSizeEvent& WXUNUSED(event) )
{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	if ( map_ && _current_phase == Phase::NO_GAME )
		map_->Refresh( true );
#endif

#if defined HEXTOOLSMAP
	// somehow, in Windows ::OnSize is called before these subwindows are created:
	if ( terrain_selector_ )
		terrain_selector_->SetSize(
#elif defined HEXTOOLSPLAY
	// somehow, in Windows ::OnSize is called before these subwindows are created:
	if ( contentsPane )
		contentsPane->SetSize(
#endif
								_side_width,
								GetClientSize( ).GetHeight( ) - _infopane_height
							);
	if ( info_pane_ )
		info_pane_->SetSize( _side_width, _infopane_height );
	if ( map_ )
		map_->SetSize( GetClientSize( ).GetWidth( ) - _side_width, GetClientSize( ).GetHeight( ) );
}

#if defined HEXTOOLSMAP
void Frame::OnExport( wxCommandEvent& WXUNUSED(event) )
{
	wxFileDialog dialog(	this,
							_("Export map"),
							_(""),
							_("map.bmp"),
							_("Bitmap images (*.bmp)|*.bmp|All files (*.*)|*.*"),
							wxSAVE | wxOVERWRITE_PROMPT );

	if ( dialog.ShowModal( ) == wxID_OK )
		map_->export_map( dialog.GetPath().ToAscii( ) );
}
#endif

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
void Frame::CmUnitEast( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitEast( );
}
void Frame::CmUnitNorthEast( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitNorthEast( );
}
void Frame::CmUnitNorthWest( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitNorthWest( );
}
void Frame::CmUnitSouthEast( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitSouthEast( );
}
void Frame::CmUnitSouthWest( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitSouthWest( );
}
void Frame::CmUnitWest( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitWest( );
}
void Frame::CmUnitHome( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitHome( );
}
void Frame::CmUnitView( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmUnitView( );
}
void Frame::CmHexCreateUnit( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmAddUnit( );
}
void Frame::CmHexDeleteUnit( wxCommandEvent& WXUNUSED(event) )
{
	contentsPane->deleteUnit( );
}
void Frame::CmEditUnit( wxCommandEvent& WXUNUSED(event) )
{
	contentsPane->editUnit( );
}

void Frame::CmHexUnitMover( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmMoveUnits( );
}
void Frame::CmRepaint( wxCommandEvent& WXUNUSED(event) )
{
	map_->rePaint( );
}
void Frame::CmCreateAirUnit( wxCommandEvent& WXUNUSED(event) )
{
	map_->addAirUnit( );
}
void Frame::CmOptionsInfo( wxCommandEvent& event )
{
	map_->CmInfo( event );
}

void Frame::CmScrollUp( wxCommandEvent& WXUNUSED(event) )
{
	map_->Scroll( 8 );
}
void Frame::CmScrollRight( wxCommandEvent& WXUNUSED(event) )
{
	map_->Scroll( 6 );
}
void Frame::CmScrollLeft( wxCommandEvent& WXUNUSED(event) )
{
	map_->Scroll( 4 );
}
void Frame::CmScrollDown( wxCommandEvent& WXUNUSED(event) )
{
	map_->Scroll( 2 );
}

void Frame::CmMoveDown( wxCommandEvent& WXUNUSED(event) )
{
	map_->MoveSelection( 2 );
}
void Frame::CmMoveLeft( wxCommandEvent& WXUNUSED(event) )
{
	map_->MoveSelection( 4 );
}
void Frame::CmMoveRight( wxCommandEvent& WXUNUSED(event) )
{
	map_->MoveSelection( 6 );
}
void Frame::CmMoveUp( wxCommandEvent& WXUNUSED(event) )
{
	map_->MoveSelection( 8 );
}

void Frame::CmCreateFort( wxCommandEvent& WXUNUSED(event) )
{
	map_->addFacility( Facility::FORT );
}
void Frame::CmPermAF( wxCommandEvent& WXUNUSED(event) )
{
	map_->addFacility( Facility::PERMAF3 );
}
void Frame::CmTempAF( wxCommandEvent& WXUNUSED(event) )
{
	map_->addFacility( Facility::TEMPAF );
}

void Frame::CmHexInfo( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmViewHex( );
}
void Frame::CmRegauge( wxCommandEvent& WXUNUSED(event) )
{
	map_->Regauge( );
}
void Frame::CmHexCombatInfo( wxCommandEvent& WXUNUSED(event) )
{
	map_->CombatInfo( );
}
void Frame::CmChangeOwner( wxCommandEvent& WXUNUSED(event) )
{
	map_->ChangeOwner( );
}
void Frame::CmSelectAll( wxCommandEvent& WXUNUSED(event) )
{
	contentsPane->selectAll( );
}
void Frame::CmDeselectAll( wxCommandEvent& WXUNUSED(event) )
{
	contentsPane->deselectAll( );
}

void Frame::CmMakeNeutral( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetOwner( 2 /* NEUTRAL */ );
}

void Frame::CmHitRail( wxCommandEvent& WXUNUSED(event) )
{
	map_->HitRail( );
}
void Frame::CmHitAF( wxCommandEvent& WXUNUSED(event) )
{
	map_->HitAF( );
}

void Frame::CmToTrain( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToTrain( );
}
void Frame::CmFromTrain( wxCommandEvent& WXUNUSED(event) )
{
	map_->FromTrain( );
}
void Frame::CmToship( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToShip( );
}
void Frame::CmFromship( wxCommandEvent& WXUNUSED(event) )
{
	map_->FromShip( );
}

void Frame::CmRP( wxCommandEvent& WXUNUSED(event) )
{
	map_->addResourceMarker( );
}
void Frame::CmPosAA( wxCommandEvent& WXUNUSED(event) )
{
	map_->addPositionalAA( );
}
void Frame::CmCreateShip( wxCommandEvent& WXUNUSED(event) )
{
	map_->addNavalUnit( );
}

void Frame::CmRepair( wxCommandEvent& WXUNUSED(event) )
{
	map_->Repair( );
}
void Frame::CmFieldworks( wxCommandEvent& WXUNUSED(event) )
{
	map_->Fieldworks( );
}
void Frame::CmClearObstructed( wxCommandEvent& WXUNUSED(event) )
{
	map_->ClearObstructed( );
}

void Frame::CmBomb( wxCommandEvent& WXUNUSED(event) )
{
	map_->AirMission( );
}  // set mission
void Frame::CmLand( wxCommandEvent& WXUNUSED(event) )
{
	map_->Land( );
}
void Frame::CmStage( wxCommandEvent& WXUNUSED(event) )
{
	map_->Stage( );
}
void Frame::CmTakeoff( wxCommandEvent& WXUNUSED(event) )
{
	map_->Takeoff( );
}
void Frame::CmResolvebombing( wxCommandEvent& WXUNUSED(event) )
{
	map_->ResolveBombing( );
}  // resolve mission
void Frame::CmFireaa( wxCommandEvent& WXUNUSED(event) )
{
	map_->FireAA( );
}
void Frame::CmRestoreafcap( wxCommandEvent& WXUNUSED(event) )
{
	map_->RestoreAFCapacity( );
}
void Frame::CmCap( wxCommandEvent& WXUNUSED(event) )
{
	map_->Takeoff( );
}
void Frame::CmNavalpatrol( wxCommandEvent& WXUNUSED(event) )
{
	map_->AssignNavalPatrol( );
}

void Frame::CmDestroyunit( wxCommandEvent& WXUNUSED(event) )
{
	map_->DestroySelUnits( );
}
void Frame::CmMoveToReplPool( wxCommandEvent& WXUNUSED(event) )
{
	map_->DestroySelUnits( false ); // don't record losses
}

void Frame::CmUnitregaugerail( wxCommandEvent& WXUNUSED(event) )
{
	wxMessageBox( wxT("Frame::CmUnitregaugerail()") );
}

void Frame::CmCombat( wxCommandEvent& WXUNUSED(event) )
{
	map_->ResolveCombat( );
}
void Frame::CmAdvance( wxCommandEvent& WXUNUSED(event) )
{
	map_->Advance( false );
}
void Frame::CmCancelattack( wxCommandEvent& WXUNUSED(event) )
{
	map_->CmCancelattack( );
}
void Frame::CmCancelair( wxCommandEvent& WXUNUSED(event) )
{
	map_->CancelAirattack( );
}
void Frame::CmResolveaircombat( wxCommandEvent& WXUNUSED(event) )
{
	map_->AirCombat( );
}

void Frame::CmReplpool( wxCommandEvent& WXUNUSED(event) )
{
	map_->ViewReplPool( );
}
void Frame::CmAirreplpool( wxCommandEvent& WXUNUSED(event) )
{
	map_->ViewAirReplPool( );
}

void Frame::CmActivateair( wxCommandEvent& WXUNUSED(event) )
{
	map_->FlipAirCounter( );
}

void Frame::CmSetafhits( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAFHits( );
}
void Frame::CmSetafusage( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAFUsage( );
}
void Frame::CmSetharassment( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetHarassment( );
}
#if 0
void Frame::CmSetmines( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAxMines( );
}
void Frame::CmSetalmines( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAlMines( );
}
#endif
void Frame::CmSetporthits( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetPortHits( );
}
void Frame::CmSetportusage( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetPortUsage( );
}
void Frame::CmDestroyport( wxCommandEvent& WXUNUSED(event) )
{
	map_->DestroyPort( );
}
void Frame::CmSetcdhits( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetCDHits( );
}
void Frame::CmRepairport( wxCommandEvent& WXUNUSED(event) )
{
	map_->RepairPort( );
}
/*
void Frame::CmSetsupport( wxCommandEvent& WXUNUSED(event) )
{
	map->SetAttSupport();
}
void Frame::CmSetdefsupport( wxCommandEvent& WXUNUSED(event) )
{
	map->SetDefSupport();
}
*/
void Frame::CmMpminus( wxCommandEvent& WXUNUSED(event) )
{
	map_->MPMinus( );
}
void Frame::CmMpminus30( wxCommandEvent& WXUNUSED(event) )
{
	map_->MPMinus30( );
}
void Frame::CmMpplus( wxCommandEvent& WXUNUSED(event) )
{
	map_->MPPlus( );
}

void Frame::CmTooverstack( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToOverstack( );
}
void Frame::CmFroverstack( wxCommandEvent& WXUNUSED(event) )
{
	map_->FromOverstack( );
}

void Frame::CmChgsupply( wxCommandEvent& WXUNUSED(event) )
{
	map_->changeHexSupply( );
}
void Frame::CmIsolated( wxCommandEvent& WXUNUSED(event) )
{
	map_->changeHexIsolation( );
}
void Frame::CmSpecialsupply( wxCommandEvent& WXUNUSED(event) )
{
	map_->changeHexSpecialSupply( );
}
void Frame::CmUpdatesupply( wxCommandEvent& WXUNUSED(event) )
{
	map_->updateAllSupply( );
}
void Frame::CmChgisolated( wxCommandEvent& WXUNUSED(event) )
{
	map_->updateAllIsolation( );
}

void Frame::CmCancelorder( wxCommandEvent& WXUNUSED(event) )
{
	map_->CancelOrder( );
}

void Frame::CmAddnote( wxCommandEvent& WXUNUSED(event) )
{
	map_->AddNote( );
}
void Frame::CmAddalert( wxCommandEvent& WXUNUSED(event) )
{
	map_->AddNote( true );
}
void Frame::CmDelnote( wxCommandEvent& WXUNUSED(event) )
{
	// TODO: CmDelNote(..) is not presently wired to any GUI component
	//	if later so wired, you must make MapPane::RemoveNote( ) public visibility
	//map->RemoveNote( );
}
void Frame::CmAddstatusmarker( wxCommandEvent& WXUNUSED(event) )
{
	map_->addStatusMarker( );
}

void Frame::CmShowattackmarkers( wxCommandEvent& WXUNUSED(event) )
{
	map_->ShowCombatMarkers( );
	SetMarkerButtons( );
}

void Frame::CmCalcallied( wxCommandEvent& WXUNUSED(event) )
{
	map_->Statistics( SidePlayer::ALLIED );
}
void Frame::CmCalcaxis( wxCommandEvent& WXUNUSED(event) )
{
	map_->Statistics( SidePlayer::AXIS );
}
void Frame::CmAxisloss( wxCommandEvent& WXUNUSED(event) )
{
	map_->ViewLosses( ); // both sides actually
}

void Frame::CmBreakbridge( wxCommandEvent& WXUNUSED(event) )
{
	map_->BreakBridge( );
}
void Frame::CmRepairbridge( wxCommandEvent& WXUNUSED(event) )
{
	map_->RepairBridge( );
}

void Frame::CmStackinfo( wxCommandEvent& WXUNUSED(event) )
{
	map_->ViewStack( );
}
#if 0
void Frame::CmLaymine( wxCommandEvent& WXUNUSED(event) )
{
	map_->LayMines( );
}
void Frame::CmSweepmine( wxCommandEvent& WXUNUSED(event) )
{
	map_->SweepMines( );
}
#endif
void Frame::CmSetre( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAllREs( );
}

void Frame::CmShowcrt( wxCommandEvent& WXUNUSED(event) )
{
	map_->show_CRT( );
}

void Frame::CmHilitearmor( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 1 );
}
void Frame::CmHiliteartillery( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 2 );
}
void Frame::CmHilitecadres( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 3 );
}
void Frame::CmHiliteplanes( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 4 );
}
void Frame::CmHiliteflyingplanes( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 5 );
}
void Frame::CmHilitestacks( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 6 );
}
void Frame::CmHiliteitemot( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 7 );
}
void Frame::CmHilitecons( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 8 );
}
void Frame::CmHiliterreng( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 9 );
}
void Frame::CmHiliteaborted( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 10 );
}
void Frame::CmHilitetrucks( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 11 );
}
void Frame::CmHiliteAirbase( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 12 );
}
void Frame::CmHiliteInop( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 13 );
}
void Frame::CmHilitedisrupted( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 14 );
}
void Frame::CmHiliteIntSources( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 15 );
}
void Frame::CmHilitePASources( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 16 );
}

void Frame::CmHiliteOwnNormalGaugeRail( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 21 );
}
void Frame::CmHiliteOwnWideGaugeRail( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 22 );
}
void Frame::CmHiliteEnemyZOCs( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 23 );
}
void Frame::CmHiliteFullEnemyZOCs( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 24 );
}
void Frame::CmHiliteReducedEnemyZOCs( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 25 );
}

void Frame::CmHiliteIntZones1( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteZones( 1 );
}
void Frame::CmHiliteIntZones2( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteZones( 2 );
}
void Frame::CmHiliteIntZones3( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteZones( 3 );
}
void Frame::CmHiliteIntZones4( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteZones( 4 );
}
void Frame::CmHilitePAZones( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteZones( 5 );
}

void Frame::CmHilitewza( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 0 );
}
void Frame::CmHilitewzb( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 1 );
}
void Frame::CmHilitewzc( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 2 );
}
void Frame::CmHilitewzd( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 3 );
}
void Frame::CmHilitewze( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 4 );
}
void Frame::CmHilitewzf( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 5 );
}
void Frame::CmHilitewzg( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 6 );
}
void Frame::CmHilitewzh1( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 7 );
}
void Frame::CmHilitewzh2( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 8 );
}
void Frame::CmHilitewzh3( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 9 );
}
void Frame::CmHilitewzi1( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 10 );
}
void Frame::CmHilitewz1( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 11 );
}
void Frame::CmHilitewz2( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 12 );
}
void Frame::CmHilitewz3( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 13 );
}
void Frame::CmHilitewz4( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 14 );
}
void Frame::CmHilitewz5( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 15 );
}
void Frame::CmHilitewz6( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 16 );
}
void Frame::CmHilitewz7( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 17 );
}
void Frame::CmHilitewz8( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 18 );
}
void Frame::CmHilitewz9( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 19 );
}
void Frame::CmHilitewz10( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 20 );
}
void Frame::CmHilitewz11( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 21 );
}
void Frame::CmHilitewz12( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 22 );
}
void Frame::CmHilitewz13( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 23 );
}
void Frame::CmHilitewz14( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 24 );
}
void Frame::CmHilitewz15( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 25 );
}
void Frame::CmHilitewz16( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 26 );
}
void Frame::CmHilitewz17( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 27 );
}
void Frame::CmHilitewz18( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 28 );
}
void Frame::CmHilitewzi2( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 29 );
}
void Frame::CmHilitewzi3( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 30 );
}
void Frame::CmHilitewzis( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 31 );
}
void Frame::CmHilitewzj1( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 32 );
}
void Frame::CmHilitewzj2( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 33 );
}
void Frame::CmHilitewzk( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 34 );
}
void Frame::CmHilitewzl( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 35 );
}
void Frame::CmHilitewzm( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 36 );
}
void Frame::CmHilitewzn( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 37 );
}
void Frame::CmHilitewzo( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 38 );
}
void Frame::CmHilitewzp1( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 39 );
}
void Frame::CmHilitewzp2( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 40 );
}
void Frame::CmHilitewzq( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteWZ( 41 );
}


// TODO: not used anywhere
void Frame::CmEditView( wxCommandEvent& WXUNUSED(event) )
{
	if ( map_->GetActiveViewCount( ) == 0 )
		return;

	for ( int nview = 0; nview < map_->GetViewCount( ); ++nview )
		if ( map_->ViewIsActive( nview ) )
		{
			map_->EditView( nview );
			return;
		}
}
void Frame::CmViewText( wxCommandEvent& WXUNUSED(event) )
{
	map_->ShowViewText( );
}
void Frame::CmAllViewsOn( wxCommandEvent& WXUNUSED(event) )
{
	wxMenuBar* menu;
	if ( ( menu = GetMenuBar( ) ) == 0 )
		return;
	else
	{
		for ( int nview = 0; nview < 20; ++nview )
			menu->Check( Frame::MenuEvent::SHOWVIEW01 + nview, map_->ViewHexCount( nview ) > 0 );
		map_->ShowAllViews( true );
	}
}
void Frame::CmAllViewsOff( wxCommandEvent& WXUNUSED(event) )
{
	wxMenuBar* menu;
	if ( ( menu = GetMenuBar( ) ) == 0 )
		return;
	else
	{
		for ( int nview = 0; nview < 20; ++nview )
			menu->Check( Frame::MenuEvent::SHOWVIEW01 + nview, false );
		map_->ShowAllViews( false );
	}
}
void Frame::CmShowView01( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 0 );
}
void Frame::CmShowView02( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 1 );
}
void Frame::CmShowView03( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 2 );
}
void Frame::CmShowView04( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 3 );
}
void Frame::CmShowView05( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 4 );
}
void Frame::CmShowView06( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 5 );
}
void Frame::CmShowView07( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 6 );
}
void Frame::CmShowView08( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 7 );
}
void Frame::CmShowView09( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 8 );
}
void Frame::CmShowView10( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 9 );
}
void Frame::CmShowView11( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 10 );
}
void Frame::CmShowView12( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 11 );
}
void Frame::CmShowView13( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 12 );
}
void Frame::CmShowView14( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 13 );
}
void Frame::CmShowView15( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 14 );
}
void Frame::CmShowView16( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 15 );
}
void Frame::CmShowView17( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 16 );
}
void Frame::CmShowView18( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 17 );
}
void Frame::CmShowView19( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 18 );
}
void Frame::CmShowView20( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleShowView( 19 );
}

// end Frame "views" functions

void Frame::CmDieroll( wxCommandEvent& WXUNUSED(event) )
{
	map_->DieRoll( 1, 6 );
#if 0
	 char s[200];

	 //#define NOPPA_TST

	 #if defined NOPPA_TST
	 // TST: does random dice thrower work ok (yes it does)
	 int t[7] = {0,0,0,0,0,0,0};
	 for (int i=0; i<60; ++i)
	 t[dice()]++;
	 sprintf(s, "1: %d\n"
	 "2: %d\n"
	 "3: %d\n"
	 "4: %d\n"
	 "5: %d\n"
	 "6: %d\n",
	 t[1],t[2],t[3],t[4],t[5],t[6]);
	 #else
	 sprintf (s, "Die roll: %d", dice());
	 #endif
	 wxMessageBox(wxS(s), Application::NAME);
#endif
}
void Frame::CmDieroll2( wxCommandEvent& WXUNUSED(event) )
{
	map_->DieRoll( 2, 6 );
}
void Frame::CmDieroll12( wxCommandEvent& WXUNUSED(event) )
{
	map_->DieRoll( 1, 12 );
}
void Frame::CmDieroll100( wxCommandEvent& WXUNUSED(event) )
{
	map_->DieRoll( 1, 100 );
}

void Frame::CmAttacksup( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleAttackSup( );
}

void Frame::CmToggleDidPA( wxCommandEvent& WXUNUSED(event) )
{
	map_->ToggleDidPA( );
}

void Frame::CmCaptoescort( wxCommandEvent& WXUNUSED(event) )
{
	map_->CAPToEscort( );
}
void Frame::CmCaptointerception( wxCommandEvent& WXUNUSED(event) )
{
	map_->CAPToInterception( );
}

void Frame::CmChkstacks( wxCommandEvent& WXUNUSED(event) )
{
	// TODO: CmChkstacks(..) is not presently wired to any GUI component
	//	if later so wired, you must make MapPane::CheckStacks( ) public visibility
	//map->CheckStacks( );  	// consider IsValidStack(..) in lieu
}

void Frame::CmAddmulberry( wxCommandEvent& WXUNUSED(event) )
{
	map_->addFacility( Facility::MULBERRY );
}

void Frame::CmAllallied( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAllOwner( SidePlayer::ALLIED );
}
void Frame::CmAllaxis( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAllOwner( SidePlayer::AXIS );
}
void Frame::CmAllneutral( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAllOwner( 2 /* NEUTRAL */ );
}
void Frame::CmVisibleallied( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetVisibleOwner( SidePlayer::ALLIED );
}
void Frame::CmVisibleaxis( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetVisibleOwner( SidePlayer::AXIS );
}
void Frame::CmVisibleneutral( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetVisibleOwner( 2 /* NEUTRAL */ );
}

void Frame::CmChangenationside( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetNationSide( SidePlayer::AXIS );
}
void Frame::CmChangenationallied( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetNationSide( SidePlayer::ALLIED );
}
void Frame::CmChangeunitside( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetUnitSide( );
}

void Frame::CmEndPhase( wxCommandEvent& WXUNUSED(event) )
{
	char msg[60];

	int prvphase = _current_phase;
	int nextphase;

	DEndPlayerTurn eot_dlg( this );

	_current_player = _phasing_player;

	// TODO: hideUnitMover( ) impl is currently empty
#if 0
	if ( Phase == MOVEMENT_PHASE || Phase == REACT_MOVE_PHASE )  // hide unit mover if it's visible
		map->hideUnitMover( );
#endif

	if ( ! map_->okToEndThisPhase( ) ) // explains why cannot switch to next phase yet
		return;

	bool do_exit = false;

	// setup mode ended?
	if ( _current_phase == Phase::SETUP_PHASE )
	{
		//Player = PrevPlayer;
		_current_phase = _previous_phase;
	}
	else
	{
		// end of player turn:
		if ( _current_phase == Phase::LASTPHASE )
		{
			if ( wxMessageBox( wxT("This will end your player turn!"), Application::NAME, wxOK | wxCANCEL | wxICON_HAND ) == wxOK )
			{
				if ( _current_player != _rule_set.FirstPlayer && _turn == _turns_per_month )
				{  // go to next month
					_turn = 1;
					++_month;
					if ( _month == 12 )
					{  // next year
						_month = 0;
						++_year;
					}
				}
				else if ( _current_player != _rule_set.FirstPlayer )
					++_turn;  // next turn in this month
				_current_phase = Phase::INITIAL_PHASE;
				_current_player = _phasing_player = ( _current_player == SidePlayer::AXIS ) ? SidePlayer::ALLIED : SidePlayer::AXIS;  // next player
				if ( eot_dlg.ShowModal( ) == wxID_OK )
				//if (wxMessageBox("Save and exit now?", Application::NAME, wxYES_NO|wxICON_QUESTION) == wxYES) {
				if ( saveScenario( ) )
					exit( 0 );
				//}
			}
			else
				return;
		}
		else
		{
			// game has just started?
			if ( _current_phase == Phase::NO_GAME )
				_current_phase = Phase::INITIAL_PHASE;
			else
			{
				// TODO: clean up this first logic clause
				// skip WW1 reaction phases if they are not enabled
				if (	_current_phase == Phase::COMBAT_PHASE && _rule_set.WW1Phases == TRUE )
					nextphase = Phase::REACT_MOVE_PHASE;
				// skip naval phases if they are not enabled
				else if ( _current_phase == Phase::INITIAL_PHASE && _rule_set.SeparateNavalPhases == FALSE )
					nextphase = Phase::MOVEMENT_PHASE;
				else if ( ( _current_phase == Phase::COMBAT_PHASE || _current_phase == Phase::REACT_COMB_PHASE ) && _rule_set.SeparateNavalPhases == FALSE )
					nextphase = Phase::EXPLOITATION_PHASE;
				else if ( _current_phase == Phase::COMBAT_PHASE && _rule_set.WW1Phases == FALSE )
					nextphase = _rule_set.SeparateNavalPhases == TRUE ? Phase::NAVAL_PHASE_6 : Phase::EXPLOITATION_PHASE;
				// skip air reaction phase if "on demand" air system
				else if ( _current_phase == Phase::MOVEMENT_PHASE && _rule_set.OnDemandAirMissions == TRUE )
					nextphase = Phase::COMBAT_PHASE;
				else
					nextphase = _current_phase + 1;
				sprintf( msg, "Really move to the %s phase?", _phase_string[nextphase] );
				if ( wxMessageBox( wxS( msg ), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
				{
					if ( nextphase == Phase::REACTION_PHASE )
					{  // change player to enemy for reaction phase
						_current_player = _phasing_player = ( _current_player == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );
						_current_phase = nextphase;
						if ( eot_dlg.ShowModal( ) == wxID_OK )
							if ( wxMessageBox( wxT("Save and exit now?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES)
							{
								if ( saveScenario( ) )
									exit( 0 );
							}
					}
					else if ( _current_phase == Phase::REACTION_PHASE || _current_phase == Phase::REACT_COMB_PHASE )
					{  // change player back to normal phase
						_current_player = _phasing_player = ( _current_player == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );
						_current_phase = nextphase;
						if ( eot_dlg.ShowModal( ) == wxID_OK )
							if ( wxMessageBox( wxT("Save and exit now?"), Application::NAME, wxYES_NO | wxICON_QUESTION ) == wxYES )
								do_exit = true;
					}
					else
					{
						// switch to non-phasing player to ww1 reaction phase
						if ( nextphase == Phase::REACT_MOVE_PHASE || nextphase == Phase::REACT_COMB_PHASE	)
							_current_player = ( _phasing_player == SidePlayer::AXIS ? SidePlayer::ALLIED : SidePlayer::AXIS );
						_current_phase = nextphase;
					}
				}
				else
					return;
			}
		}
	}

	contentsPane->deselectAll( );

	// do any phase-special things
	switch ( _current_phase )
	{
		case Phase::EXPLOITATION_PHASE:
			// TODO: If there are unresolved combats, ask for confirmation
			//       and cancel them!
			if ( prvphase != Phase::SETUP_PHASE )
			{
				map_->RestoreAllMPs( _current_phase );
				MoveMode = MovementMode::TACTICAL;
			}
			map_->set_move_mode( MoveMode );
			//SetMoveModeButtons(MoveMode);
			break;
		case Phase::INITIAL_PHASE:
			if ( prvphase != Phase::SETUP_PHASE )
				map_->RestoreAllMPs( _current_phase );
			if ( _year == 44 && _month == 6 )  // months start from 0 -> Jul
				wxMessageBox(	wxT("Because of infantry antitank weapons\n"
									"all German units without ATEC capability\n"
									"are now ATEC neutral" ),
									Application::NAME, wxOK | wxICON_INFORMATION );
			break;
		case Phase::NAVAL_PHASE_1:
			if ( prvphase != Phase::SETUP_PHASE )
			{
				map_->RestoreAllMPs( _current_phase );
				MoveMode = MovementMode::TACTICAL;
			}
			map_->set_move_mode( MoveMode );
#if 0
			SetMoveModeButtons(MoveMode);
#endif
			break;
		case Phase::REACT_MOVE_PHASE:
		case Phase::MOVEMENT_PHASE:
			if ( prvphase != Phase::SETUP_PHASE && _rule_set.SeparateNavalPhases == FALSE )
			{
				map_->RestoreAllMPs( _current_phase );
				MoveMode = MovementMode::TACTICAL;
			}
			map_->set_move_mode( MoveMode );
#if 0
			SetMoveModeButtons( MoveMode );
#endif
			break;
	}

	if ( prvphase != Phase::SETUP_PHASE )
		map_->initializeThePhase( );

	_phasing_player = _current_player;
	setCaption( );

	char phase[30];
	create_phase_string( phase );

	char turn[30];
	create_turn_string( turn );

	info_pane_->displayTurnInfo( turn, phase );

	if ( prvphase != Phase::SETUP_PHASE )  // no need to repaint; unnecessary flicker
		map_->Refresh( );

	SetupMenu( );
	setup_toolbar( );

	// exit game if player answered "yes"
	if ( do_exit && saveScenario( ) )
		exit( 0 );
}

void Frame::CmSetupMode( wxCommandEvent& WXUNUSED(event) )
{
	if ( _current_phase == Phase::SETUP_PHASE )
	{
		map_->clear_setup_modes( ); // country & seazone modes = false
		wxCommandEvent e;
		CmEndPhase( e );
		return;
	}

	_previous_phase = _current_phase;
	_current_phase = Phase::SETUP_PHASE;

	char phase[30];
	create_phase_string( phase );

	char turn[30];
	create_turn_string( turn );

	info_pane_->displayTurnInfo( turn, phase );
	SetupMenu( );
	setup_toolbar( );
}

void Frame::OnSeazoneMode( wxCommandEvent& WXUNUSED(event) )
{
	map_->set_seazone_mode( );
	GetToolBar()->FindControl( CMD_SEAZONECHOICE )->Enable( );
	SetStatusText( "Sea circle mode" );
	map_->Refresh( );
}

void Frame::OnSeazoneSelection( wxCommandEvent& WXUNUSED(event) )
{
	map_->set_seazone_mode( );
	wxChoice* selector = static_cast<wxChoice*>( GetToolBar()->FindControl( CMD_SEAZONECHOICE ) );
	map_->set_current_seazone( selector->GetLabel( ) );
}

void Frame::OnCountryMode( wxCommandEvent& WXUNUSED(event) )
{
	map_->set_country_mode( );
	GetToolBar()->FindControl( CMD_COUNTRYCHOICE )->Enable( );
	SetStatusText( "Land district mode" );
	map_->Refresh( );
}

void Frame::OnCountrySelection( wxCommandEvent& WXUNUSED(event) )
{
	map_->set_country_mode( );
	wxChoice* selector = static_cast<wxChoice*>( GetToolBar()->FindControl( CMD_COUNTRYCHOICE ) );
	map_->set_current_country( selector->GetLabel( ) );
}


void Frame::CmChangeSide( wxCommandEvent& WXUNUSED(event) )
{
	_current_player = ( _current_player == SidePlayer::AXIS ) ? SidePlayer::ALLIED : SidePlayer::AXIS;

	char phase[30];
	create_phase_string( phase );

	char turn[30];
	create_turn_string( turn );

	info_pane_->displayTurnInfo( turn, phase );

	SetupMenu( );
	setCaption( );
}

void Frame::CmTimejump( wxCommandEvent& WXUNUSED(event) )
{
	DTimeJump dlg( this );
	if ( dlg.ShowModal( ) != wxID_OK )
		return;
	dlg.CloseDialog( );

	_year = _turn_dialog_data.cmbYear;
	_month = _turn_dialog_data.cmbMonth;
	_turn = _turn_dialog_data.cmbTurn;
	_current_player = _phasing_player = _turn_dialog_data.cmbPlayer;
	_current_phase = _turn_dialog_data.cmbPhase;

	char phase[50];
	create_phase_string( phase );

	char turn[50];
	create_turn_string( turn );

	info_pane_->displayTurnInfo( turn, phase );

	SetupMenu( );
	setup_toolbar( );
	setCaption( );
}

void Frame::CmChgcrt( wxCommandEvent& WXUNUSED(event) )
{
	wxFileDialog dialog(	this,
							wxT("Load CRT"),
							wxT(""),
							wxT(Application::CRT_FILENAME),
							wxT("HexTools CRT (*.gcrt)|*.gcrt|All files (*.*)|*.*"),
							wxOPEN );

	if ( dialog.ShowModal( ) == wxID_OK )
	{
		map_->set_CRT_name( dialog.GetFilename( ).ToAscii( ) ); // parses basename only

		// just pass in the fully-qualified pathname, which initializeCRT(..) enforces
		map_->initialize_CRT( dialog.GetFilename( ).ToAscii( ), false ); // direct load this filename

		map_->show_CRT( );
	}
}

void Frame::CmChgtec( wxCommandEvent& WXUNUSED(event) )
{
	wxFileDialog dialog(	this,
							wxT("Load TEC"),
							wxT(""),
							wxT(Application::TEC_FILENAME),
							wxT("HexTools TEC (*.tec)|*.tec|All files (*.*)|*.*"),
							wxOPEN );

	if ( dialog.ShowModal( ) == wxID_OK )
	{
		map_->set_TEC_name( dialog.GetFilename( ).ToAscii( ) ); // basename only
#if 0
		if ( ! map->set_TEC_name( dialog.GetPath( ).ToAscii( ) ) )
		{
			wxMessageBox( wxT("Failed to load TEC"), Application::NAME );
			return;
		}
#endif
		// just pass in the fully-qualified pathname, which initializeCustomTEC(..) enforces
		map_->initialize_TEC( dialog.GetFilename( ).ToAscii( ), false ); // direct load this filename
	}
}

void Frame::CmWeather( wxCommandEvent& WXUNUSED(event) )
{
	// init dialog controls:
#if 0
	WeatherDlgData.RdAClear = WeatherDlgData.RdAMud = WeatherDlgData.RdASnow = WeatherDlgData.RdAFrost = WeatherDlgData.RdAWinter = FALSE;
	switch ( Weather[0] )
	{
		case W_CLEAR:
			WeatherDlgData.RdAClear = TRUE;
			break;
		case W_MUD:
			WeatherDlgData.RdAMud = TRUE;
			break;
		case W_SNOW:
			WeatherDlgData.RdASnow = TRUE;
			break;
		case W_FROST:
			WeatherDlgData.RdAFrost = TRUE;
			break;
		case W_WINTER:
			WeatherDlgData.RdAWinter = TRUE;
			break;
	}

	WeatherDlgData.RdBClear = WeatherDlgData.RdBMud = WeatherDlgData.RdBSnow = WeatherDlgData.RdBFrost = WeatherDlgData.RdBWinter = FALSE;
	switch ( Weather[1] )
	{
		case W_CLEAR:
			WeatherDlgData.RdBClear = TRUE;
			break;
		case W_MUD:
			WeatherDlgData.RdBMud = TRUE;
			break;
		case W_SNOW:
			WeatherDlgData.RdBSnow = TRUE;
			break;
		case W_FROST:
			WeatherDlgData.RdBFrost = TRUE;
			break;
		case W_WINTER:
			WeatherDlgData.RdBWinter = TRUE;
			break;
	}

	WeatherDlgData.RdCClear = WeatherDlgData.RdCMud = WeatherDlgData.RdCSnow = WeatherDlgData.RdCFrost = WeatherDlgData.RdCWinter = FALSE;
	switch ( Weather[2] )
	{
		case W_CLEAR:
			WeatherDlgData.RdCClear = TRUE;
			break;
		case W_MUD:
			WeatherDlgData.RdCMud = TRUE;
			break;
		case W_SNOW:
			WeatherDlgData.RdCSnow = TRUE;
			break;
		case W_FROST:
			WeatherDlgData.RdCFrost = TRUE;
			break;
		case W_WINTER:
			WeatherDlgData.RdCWinter = TRUE;
			break;
	}

	WeatherDlgData.RdDClear = WeatherDlgData.RdDMud = WeatherDlgData.RdDSnow = WeatherDlgData.RdDFrost = WeatherDlgData.RdDWinter = FALSE;
	switch ( Weather[3] )
	{
		case W_CLEAR:
			WeatherDlgData.RdDClear = TRUE;
			break;
		case W_MUD:
			WeatherDlgData.RdDMud = TRUE;
			break;
		case W_SNOW:
			WeatherDlgData.RdDSnow = TRUE;
			break;
		case W_FROST:
			WeatherDlgData.RdDFrost = TRUE;
			break;
		case W_WINTER:
			WeatherDlgData.RdDWinter = TRUE;
			break;
	}

	WeatherDlgData.RdEClear = WeatherDlgData.RdEMud = WeatherDlgData.RdESnow = WeatherDlgData.RdEFrost = WeatherDlgData.RdEWinter = FALSE;
	switch ( Weather[4] )
	{
		case W_CLEAR:
			WeatherDlgData.RdEClear = TRUE;
			break;
		case W_MUD:
			WeatherDlgData.RdEMud = TRUE;
			break;
		case W_SNOW:
			WeatherDlgData.RdESnow = TRUE;
			break;
		case W_FROST:
			WeatherDlgData.RdEFrost = TRUE;
			break;
		case W_WINTER:
			WeatherDlgData.RdEWinter = TRUE;
			break;
	}

	WeatherDlgData.RdFClear = WeatherDlgData.RdFMud = WeatherDlgData.RdFSnow = WeatherDlgData.RdFFrost = WeatherDlgData.RdFWinter = FALSE;
	switch ( Weather[5] )
	{
		case W_CLEAR:
			WeatherDlgData.RdFClear = TRUE;
			break;
		case W_MUD:
			WeatherDlgData.RdFMud = TRUE;
			break;
		case W_SNOW:
			WeatherDlgData.RdFSnow = TRUE;
			break;
		case W_FROST:
			WeatherDlgData.RdFFrost = TRUE;
			break;
		case W_WINTER:
			WeatherDlgData.RdFWinter = TRUE;
			break;
	}

	WeatherDlgData.RdGClear = WeatherDlgData.RdGMud = WeatherDlgData.RdGSnow = WeatherDlgData.RdGFrost = WeatherDlgData.RdGWinter = FALSE;
	switch ( Weather[6] )
	{
		case W_CLEAR:
			WeatherDlgData.RdGClear = TRUE;
			break;
		case W_MUD:
			WeatherDlgData.RdGMud = TRUE;
			break;
		case W_SNOW:
			WeatherDlgData.RdGSnow = TRUE;
			break;
		case W_FROST:
			WeatherDlgData.RdGFrost = TRUE;
			break;
		case W_WINTER:
			WeatherDlgData.RdGWinter = TRUE;
			break;
	}

	WeatherDlgData.chkIceA = Frozen[0];
	WeatherDlgData.chkIceB = Frozen[1];
	WeatherDlgData.chkIceC = Frozen[2];
	WeatherDlgData.chkIceD = Frozen[3];
	WeatherDlgData.chkIceE = Frozen[4];
	WeatherDlgData.chkIceF = Frozen[5];
	WeatherDlgData.chkIceG = Frozen[6];
#endif
	DWeather dlg( this );
	if ( dlg.ShowModal( ) != wxID_OK )
		return;
	dlg.CloseDialog( );
#if 0
	// get results:
	if ( WeatherDlgData.RdAClear )
		Weather[0] = W_CLEAR;
	else if ( WeatherDlgData.RdAMud )
		Weather[0] = W_MUD;
	else if ( WeatherDlgData.RdASnow )
		Weather[0] = W_SNOW;
	else if ( WeatherDlgData.RdAFrost )
		Weather[0] = W_FROST;
	else if ( WeatherDlgData.RdAWinter )
		Weather[0] = W_WINTER;

	if ( WeatherDlgData.RdBClear )
		Weather[1] = W_CLEAR;
	else if ( WeatherDlgData.RdBMud )
		Weather[1] = W_MUD;
	else if ( WeatherDlgData.RdBSnow )
		Weather[1] = W_SNOW;
	else if ( WeatherDlgData.RdBFrost )
		Weather[1] = W_FROST;
	else if ( WeatherDlgData.RdBWinter )
		Weather[1] = W_WINTER;

	if ( WeatherDlgData.RdCClear )
		Weather[2] = W_CLEAR;
	else if ( WeatherDlgData.RdCMud )
		Weather[2] = W_MUD;
	else if ( WeatherDlgData.RdCSnow )
		Weather[2] = W_SNOW;
	else if ( WeatherDlgData.RdCFrost )
		Weather[2] = W_FROST;
	else if ( WeatherDlgData.RdCWinter )
		Weather[2] = W_WINTER;

	if ( WeatherDlgData.RdDClear )
		Weather[3] = W_CLEAR;
	else if ( WeatherDlgData.RdDMud )
		Weather[3] = W_MUD;
	else if ( WeatherDlgData.RdDSnow )
		Weather[3] = W_SNOW;
	else if ( WeatherDlgData.RdDFrost )
		Weather[3] = W_FROST;
	else if ( WeatherDlgData.RdDWinter )
		Weather[3] = W_WINTER;

	if ( WeatherDlgData.RdEClear )
		Weather[4] = W_CLEAR;
	else if ( WeatherDlgData.RdEMud )
		Weather[4] = W_MUD;
	else if ( WeatherDlgData.RdESnow )
		Weather[4] = W_SNOW;
	else if ( WeatherDlgData.RdEFrost )
		Weather[4] = W_FROST;
	else if ( WeatherDlgData.RdEWinter )
		Weather[4] = W_WINTER;

	if ( WeatherDlgData.RdFClear )
		Weather[5] = W_CLEAR;
	else if ( WeatherDlgData.RdFMud )
		Weather[5] = W_MUD;
	else if ( WeatherDlgData.RdFSnow )
		Weather[5] = W_SNOW;
	else if ( WeatherDlgData.RdFFrost )
		Weather[5] = W_FROST;
	else if ( WeatherDlgData.RdFWinter )
		Weather[5] = W_WINTER;

	if ( WeatherDlgData.RdGClear )
		Weather[6] = W_CLEAR;
	else if ( WeatherDlgData.RdGMud )
		Weather[6] = W_MUD;
	else if ( WeatherDlgData.RdGSnow )
		Weather[6] = W_SNOW;
	else if ( WeatherDlgData.RdGFrost )
		Weather[6] = W_FROST;
	else if ( WeatherDlgData.RdGWinter )
		Weather[6] = W_WINTER;

	Frozen[0] = WeatherDlgData.chkIceA;
	Frozen[1] = WeatherDlgData.chkIceB;
	Frozen[2] = WeatherDlgData.chkIceC;
	Frozen[3] = WeatherDlgData.chkIceD;
	Frozen[4] = WeatherDlgData.chkIceE;
	Frozen[5] = WeatherDlgData.chkIceF;
	Frozen[6] = WeatherDlgData.chkIceG;
#endif
	map_->Refresh( );  // repaint map
}

void Frame::CmSettings( wxCommandEvent& WXUNUSED(event) )
{
	DDisplaySettings dlg( this );

	// init dialog data
	dlg.SetSubTypePalette( _armed_force_filter );
	_display_options_dialog_data.rdAlliedAtt = _display_options_dialog_data.rdAlliedDef = FALSE;
	_display_options_dialog_data.rdAxisAtt = _display_options_dialog_data.rdAxisDef = FALSE;
	_display_options_dialog_data.ChkWeather = _show_weather;
	_display_options_dialog_data.ChkTrans = _transparent_text;

	if ( _cache_allied_attack_strengths == TRUE )
		_display_options_dialog_data.rdAlliedAtt = TRUE;
	else
		_display_options_dialog_data.rdAlliedDef = TRUE;

	if ( _cache_axis_attack_strengths == TRUE )
		_display_options_dialog_data.rdAxisAtt = TRUE;
	else
		_display_options_dialog_data.rdAxisDef = TRUE;

	_display_options_dialog_data.ChkOwner = map_->show_owner_;
	_display_options_dialog_data.ChkBak = _backup_on_save;

	_display_options_dialog_data.chkQScrollEnable = _quick_scroll_enabled;
	_display_options_dialog_data.chkQScrollMirror = _quick_scroll_mirrored;
	dlg.quickscroll_velocity_ = _quick_scroll_velocity;
	_display_options_dialog_data.chkGrayHexes = _gray_out_hexes;
	_display_options_dialog_data.chkClickToMove = _use_point_and_click_move;
	_display_options_dialog_data.chkMpsOnMap = _show_MPs_on_map;
	//_display_options_dialog_data.chkOldGraph = _show_flagpoles_on_map;
	_display_options_dialog_data.chkUseCorpsMarkers = _show_corps_markers_on_map;
	_display_options_dialog_data.tileset = _tile_set;

	if ( hbmAllied == hbmAlliedUS )
		_display_options_dialog_data.alliedhexownersym = 1;
	else if ( hbmAllied == hbmAlliedBr )
		_display_options_dialog_data.alliedhexownersym = 2;
	else
		_display_options_dialog_data.alliedhexownersym = 0;

	if ( dlg.ShowModal( ) != wxID_OK )
		return;
	dlg.CloseDialog( );  // TODO: look into this

	// collect results
	_cache_allied_attack_strengths = _display_options_dialog_data.rdAlliedAtt ? TRUE : FALSE;
	_cache_axis_attack_strengths = _display_options_dialog_data.rdAxisAtt ? TRUE : FALSE;
	_armed_force_filter = dlg.SubTypePalette( );
	_transparent_text = _display_options_dialog_data.ChkTrans;
	map_->show_owner_ = _display_options_dialog_data.ChkOwner;
	_show_weather = _display_options_dialog_data.ChkWeather;
	_quick_scroll_enabled = _display_options_dialog_data.chkQScrollEnable;
	_quick_scroll_mirrored = _display_options_dialog_data.chkQScrollMirror;
	_quick_scroll_velocity = dlg.quickscroll_velocity_;
	_backup_on_save = _display_options_dialog_data.ChkBak;
	//QScrollVel = DisplayOptionsDlgData.trackVel;
	_gray_out_hexes = _display_options_dialog_data.chkGrayHexes;
	_use_point_and_click_move = _display_options_dialog_data.chkClickToMove;
	_show_MPs_on_map = _display_options_dialog_data.chkMpsOnMap;
	//_show_flagpoles_on_map = _display_options_dialog_data.chkOldGraph;
	_show_corps_markers_on_map = _display_options_dialog_data.chkUseCorpsMarkers;

	if ( _tile_set != _display_options_dialog_data.tileset )
		wxMessageBox( wxT("You need to restart HexTools in order to\nload new tileset!"), Application::NAME );

	_tile_set = _display_options_dialog_data.tileset;

	if ( _display_options_dialog_data.alliedhexownersym == 1 )
		hbmAllied = hbmAlliedUS;
	else if ( _display_options_dialog_data.alliedhexownersym == 2 )
		hbmAllied = hbmAlliedBr;
	else
		hbmAllied = hbmAlliedSov;

	saveSettings( );
	map_->rePaint( );
}

// edit rules
void Frame::CmOptionsrules( wxCommandEvent& WXUNUSED(event) )
{
	DRules dlg( this );

	RuleOption re_opts[] =
	{
		{ "0.5", 1 },
		{ "1.0", 2 },
		{ "1.5", 3 },
		{ "2.0", 4 },
		{ "2.5", 5 },
		{ "3.0", 6 },
		{ "3.5", 7 },
		{ "4.0", 8 },
		{ "4.5", 9 },
		{ "5.0", 10 },
		{ "5.5", 11 },
		{ "6.0", 12 },
		{ "6.5", 13 },
		{ "7.0", 14 },
		{ "7.5", 15 },
		{ "8.0", 16 }
	};

	RuleOption zoc_opts[] =
	{
	{ "None", ZOC::NONE },
	{ "Reduced", ZOC::REDUCED },
	{ "Normal", ZOC::NORMAL } };
#if 0
	RuleOption rulesvariant_opts[] =
	{
		{ "World War II", RulesVariant::WW2 },
		{ "World War I", RulesVariant::WW1 },
		{ "World War II Variant A", RulesVariant::A }
	};
	dlg.AddRule( RPAGE_GAMESERIE, "Rules Variant", &_rule_set.rules_variant_, rulesvariant_opts, 3 );
#endif
	// game era
	RuleOption era[] =
	{
			{	"World War II", GameEra::WW2 },
			{	"Spanish Civil War", GameEra::SCW },
			{	"World War I", GameEra::WW1 }
	};
	dlg.AddRule( RPAGE_GAMESERIE, "Available game eras", &_rule_set.Era, era, 3 );
#if 0
	RuleOption unittype_opts[] =
	{
		{ "World War II Europe", GameUnitSet::WW2EUR },
		{ "World War II Pacific", GameUnitSet::WW2PAC },
		{ "World War II", GameUnitSet::WW2PAC | GameUnitSet::WW2EUR },
		{ "World War I", GameUnitSet::WW1 },
		{ "All of them", GameUnitSet::WW1 | GameUnitSet::WW2PAC | GameUnitSet::WW2EUR }
	};
	dlg.AddRule( RPAGE_GAMESERIE, "Available nationalities", &_rule_set.GameUnits, unittype_opts, 5 );

	RuleOption unittype_opts2[] =
	{
		{ "World War II", GroundUnitType::WW2 },
		{ "World War I", GroundUnitType::WW1 }
	};
	dlg.AddRule( RPAGE_GAMESERIE, "Available unit types", &_rule_set.UnitTypeFlags, unittype_opts2, 2 );
#endif
	//    RuleOption wwii_opts[] = {{"World War II",  0},
	//                          {"World War I",  1}};
	dlg.AddBoolRule( RPAGE_GAMESERIE, "Enable WW1 reaction phases (movement & combat)", &_rule_set.WW1Phases );
	dlg.AddBoolRule( RPAGE_GAMESERIE, "WW1 modern artillery tactics modifier (see rule 12B)", &_rule_set.WW1ModernArtTactics );

	RuleOption abase_opts[] =
	{
		{ "World War II", FALSE },
		{ "World War I", TRUE }
	};
	dlg.AddRule( RPAGE_GAMESERIE, "Airbase capacity system", &_rule_set.WW1AirBaseCapacity, abase_opts, 2 );

	dlg.AddBoolRule( RPAGE_GAMESERIE, "Exploitation phase MPs", &_rule_set.WW1ExploitMPs );
	dlg.AddBoolRule( RPAGE_GAMESERIE, "Captured rail lines are obstructed", &_rule_set.RailsObstructed );

	// general
	dlg.AddIntRule( RPAGE_GENERAL, "Turns per month", &_turns_per_month, 1, 60 );

	RuleOption firstmove_opts[] =
	{
		{ "Axis player", SidePlayer::AXIS },
		{ "Allied player", SidePlayer::ALLIED }
	};
	dlg.AddRule( RPAGE_GENERAL, "Who moves first", &_rule_set.FirstPlayer, firstmove_opts, 2 );

	dlg.AddBoolRule( RPAGE_GENERAL, "Allow individual unit's RE size to be changed", &_rule_set.AllowCustomREs );
	dlg.AddRule( RPAGE_GENERAL, "HQ size in RE's", &_rule_set.HQRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Company size in RE's", &_rule_set.CompanyRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Battalion size in RE's", &_rule_set.BattalionRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Regiment size in RE's", &_rule_set.RegimentRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Brigade size in RE's", &_rule_set.BrigadeRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Cadre size in RE's", &_rule_set.CadreRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Divisional grouping size in RE's", &_rule_set.DivGrpRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Division size in RE's", &_rule_set.DivisionRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Corps size in RE's", &_rule_set.CorpsRE2, re_opts, 16 );
	dlg.AddRule( RPAGE_GENERAL, "Army size in RE's", &_rule_set.ArmyRE2, re_opts, 16 );

	dlg.AddBoolRule( RPAGE_GENERAL, "Allow individual unit's ZOC to be changed", &_rule_set.AllowCustomZOCs );
	dlg.AddRule( RPAGE_GENERAL, "Regiment ZOC", &_rule_set.RegimentZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "U2 Regiment ZOC", &_rule_set.RegimentU2ZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "Brigade ZOC", &_rule_set.BrigadeZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "U2 Brigade ZOC", &_rule_set.BrigadeU2ZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "Cadre ZOC", &_rule_set.CadreZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "U2 Cadre ZOC", &_rule_set.CadreU2ZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "Divisional grp ZOC", &_rule_set.DivGrpZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "U2 Divisional grp Z ZOC", &_rule_set.DivGrpU2ZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "Division ZOC", &_rule_set.DivisionZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "U2 Division ZOC", &_rule_set.DivisionU2ZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "Corps ZOC", &_rule_set.CorpsZOC, zoc_opts, 3 );
	dlg.AddRule( RPAGE_GENERAL, "U2 Corps ZOC", &_rule_set.CorpsU2ZOC, zoc_opts, 3 );

	// movement:
	dlg.AddBoolRule( RPAGE_MOVEMENT, "Disable automatic MP calculation", &_rule_set.DisableMPCalc );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "Disable automatic rail gauge checking", &_rule_set.NoGaugeChk );

	RuleOption swamp_opts[] =
	{
		{ "6 MP", TRUE },
		{ "1/2MP+1", FALSE }
	};
	dlg.AddRule( RPAGE_MOVEMENT, "MP's needed to enter swamp hex (c/m, art)", &_rule_set.MP6Swamp, swamp_opts, 2 );

	dlg.AddBoolRule( RPAGE_MOVEMENT, "Administrative movement: only clear hexes allowed", &_rule_set.OnlyClearAdmin );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "Administrative movement: allow out-of supply units to use it", &_rule_set.AllowUxAdminMove );

	RuleOption rr_opts[] =
	{
		{ "1 MP to board, FitE/SE allowances", TRUE },
		{ "0 MP to board, SF allowances", FALSE }
	};
	dlg.AddRule( RPAGE_MOVEMENT, "Operative rail movement MP cost", &_rule_set.ToTrain1MP, rr_opts, 2 );

	dlg.AddBoolRule( RPAGE_MOVEMENT, "ALL divisional units have reduced ZOCs", &_rule_set.ReducedZOCs );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "ALL Soviet divisional units have reduced ZOCs", &_rule_set.SovietReducedZOCs );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "Allow non-phasing player's units to move", &_rule_set.NonPhasMove );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to build a fort level", &_rule_set.FortLevelMP, 1, 10 );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to build permanent airfield", &_rule_set.PermAFMP, 1, 10 );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to build temporary airfield", &_rule_set.TempAFMP, 1, 10 );

	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to leave ZOC", &_rule_set.ZOCLeaveCost, 0, 10 );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to move from ZOC to ZOC", &_rule_set.ZOC2ZOCCost, 0, 10 );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to leave reduced ZOC", &_rule_set.RZOCLeaveCost, 0, 10 );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to move from reduced ZOC to ZOC", &_rule_set.RZOC2ZOCCost, 0, 10 );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "German c/m units pay 1MP less for ZOC", &_rule_set.GermanCMZOCRule );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to enter ZOC (c/m units)", &_rule_set.ZOCEnterMot, 0, 10 );
	dlg.AddIntRule( RPAGE_MOVEMENT, "MPs needed to enter ZOC (non-c/m units)", &_rule_set.ZOCEnterNormal, 0, 10 );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "No Allied ZOC across borders", &_rule_set.NoAlliedZOCAcrossBorders );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "German Regauge in Exploitation", &_rule_set.GermanRegaugeInExploitation );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "Soviets have a Reaction Phase", &_rule_set.SovietReactionPhase );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "Soviets have limitations in Exploitation Phase", &_rule_set.SovietExploitLimitation );
	dlg.AddBoolRule( RPAGE_MOVEMENT, "Roads can be broken", &_rule_set.RoadsCanBeBroken );

	RuleOption road_opts[] =
	{
		{ "Like clear terrain", FALSE },
		{ "1 MP (paved roads)", TRUE }
	};
	dlg.AddRule( RPAGE_MOVEMENT, "Road MPs during bad weather", &_rule_set.PavedRoads, road_opts, 2 );

	dlg.AddBoolRule( RPAGE_MOVEMENT, "Optional low-odds overruns from TEM #40", &_rule_set.TEM40Overruns );

	// combat:
	RuleOption alliedcombatside_opts[] =
	{
		{ "Other",		AlliedCoalition::OTHER },
		{ "British",	AlliedCoalition::BRITISH },
		{ "French",		AlliedCoalition::FRENCH },
		{ "Vichy",		AlliedCoalition::VICHY },
		{ "Soviet",		AlliedCoalition::SOVIET },
		{ "USA",		AlliedCoalition::USA },
		{ "Greeks",		AlliedCoalition::GREEKS }
	};
	dlg.AddRule( RPAGE_COMBAT, "Allied Combat Side", &_rule_set.AlliedCombatSide, alliedcombatside_opts, 7 );

	dlg.AddBoolRule( RPAGE_COMBAT, "Extreme CRT (Internal)", &_rule_set.ExtremeCrt );
	dlg.AddBoolRule( RPAGE_COMBAT, "DR: Automatic defender retreat to random direction", &_rule_set.AutoDR );
	dlg.AddBoolRule( RPAGE_COMBAT, "DH: Automatic selection of destroyed units", &_rule_set.AutoDH );
	dlg.AddBoolRule( RPAGE_COMBAT, "DE/EX/HX: Automatically destroy all defending units", &_rule_set.AutoDE );
	dlg.AddBoolRule( RPAGE_COMBAT, "Only isolated U1 unit's attack strength is halved", &_rule_set.OnlyIsolU1Halved );

	RuleOption city_opts[] =
	{
		{ "Full:-2, Partial:-1 (east)", FALSE },
		{ "Full:-1, Partial:0 (west)", TRUE }
	};
	dlg.AddRule( RPAGE_COMBAT, "Full/Partial hex city die roll modifiers", &_rule_set.WestCityMod, city_opts, 2 );

	RuleOption swmp_opts[] =
	{
		{ "-1", TRUE },
		{ "-2", FALSE }
	};
	dlg.AddRule( RPAGE_COMBAT, "Swamp die roll modifier", &_rule_set.NewSwamp, swmp_opts, 2 );

	dlg.AddBoolRule( RPAGE_COMBAT, "Require attack supply for full strength attack", &_rule_set.AttackSup );
	dlg.AddBoolRule( RPAGE_COMBAT, "Axis AT units have only  ATEC before 3.43 (FitE/SE)", &_rule_set.AxisHalfAT );

	RuleOption das_opts[] =
	{
		{ "Printed, modified by terrain", DASMode::FULL_STR_MOD },
		{ "Printed, unmodified", DASMode::FULL_STR },
		{ "Printed/2, unmodified", DASMode::HALF_STR }
	};
	dlg.AddRule( RPAGE_COMBAT, "DAS bombing strength calculation", &_rule_set.DASMode, das_opts, 3 );

	dlg.AddBoolRule( RPAGE_COMBAT, "GS bombing strength halved on mtn, forest, swmp, and maj.city", &_rule_set.GSHalved );
	dlg.AddBoolRule( RPAGE_COMBAT, "Weather affect die roll (if yes: mud=-2, snow=-1)", &_rule_set.WeatherDieMod );
	dlg.AddBoolRule( RPAGE_COMBAT, "Swamps freeze in snow/winter weather (die mod.=0)", &_rule_set.FreezingAllowed );
	dlg.AddBoolRule( RPAGE_COMBAT, "Frozen lake hexsides are ignored in SE", &_rule_set.IgnoreFrozenLakeHexsides );
	dlg.AddBoolRule( RPAGE_COMBAT, "Allow reduced AEC in snow weather (otherwise no AEC)", &_rule_set.SnowReducedAEC );

	RuleOption aec_opts[] =
	{
		{ "1/7", FALSE },
		{ "1/10", TRUE }
	};
	dlg.AddRule( RPAGE_COMBAT, "AEC proportion needed for +/-1 die modifier", &_rule_set.OneTenthAEC, aec_opts, 2 );

	RuleOption eng_opts[] =
	{
		{ "1/5", 4 },
		{ "1/7", 6 },
		{ "1/10", 9 }
	};
	dlg.AddRule( RPAGE_COMBAT, "Engineer proportion needed for +/-1 die modifier", &_rule_set.EngProportion, eng_opts, 3 );

	dlg.AddBoolRule( RPAGE_COMBAT, "Border Rivers Ignored for Combat", &_rule_set.BorderRiversIgnored );
	dlg.AddIntRule( RPAGE_COMBAT, "Fortified Area Fort Level", &_rule_set.FortifiedAreaLevel, 1, 4 );

	// air:
	dlg.AddBoolRule( RPAGE_AIR, "On-demand air missions", &_rule_set.OnDemandAirMissions );

	RuleOption acap_opts[] =
	{
		{ "Needed for takeoffs and staging", FALSE },
		{ "Needed for air group activation", TRUE }
	};
	dlg.AddRule( RPAGE_AIR, "Airbase capacity usage", &_rule_set.NewAFCapacitySystem, acap_opts, 2 );

	RuleOption acomb_opts[] =
	{
		{ "End combat after first result", TRUE },
		{ "Cumulative results; 2*abort=kill", FALSE }
	};
	dlg.AddRule( RPAGE_AIR, "When several planes are firing one plane", &_rule_set.EndAirCombatAtFirstResult, acomb_opts, 2 );

	//dlg.AddBoolRule( RPAGE_AIR, "HF is like F", &RuleSet.HFIsF );

	dlg.AddBoolRule( RPAGE_AIR, "Aborted planes are placed to replacement pool", &_rule_set.AbortedPlanesToPool );

	RuleOption aa_opts[] =
	{
		{ "New (SF etc.)", TRUE },
		{ "Old (FitE/SE)", FALSE }
	};
	dlg.AddRule( RPAGE_AIR, "AA table and die modifiers", &_rule_set.NewAAMods, aa_opts, 2 );

	dlg.AddIntRule( RPAGE_AIR, "Staging range, % of printed range (100=printed)", &_rule_set.StageRange, 1, 1000 );
	dlg.AddIntRule( RPAGE_AIR, "Temporary airfield capacity in clear/frost weather", &_rule_set.TmpAFCap, 1, 4 );
	dlg.AddIntRule( RPAGE_AIR, "Temporary airfield capacity in mud/snow weather", &_rule_set.TmpAFBadWeatherCap, 1, 4 );

	RuleOption esc_opts[] =
	{
		{ "Stay in the mission force", FALSE },
		{ "Become escorts", TRUE }
	};
	dlg.AddRule( RPAGE_AIR, "Fighters jettisoning their bombs", &_rule_set.JettisonEsc, esc_opts, 2 );

	RuleOption fb_opts[] =
	{
		{ "-2 for both air attack and defense", TRUE },
		{ "-2 for air attack only", FALSE }
	};
	dlg.AddRule( RPAGE_AIR, "Fighters carrying bombs", &_rule_set.BombingFBothReduced, fb_opts, 2 );

	dlg.AddIntRule( RPAGE_AIR, "Allied Full City Intrinsic AA", &_rule_set.AlliedFullCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Allied Partial City Intrinsic AA", &_rule_set.AlliedPartialCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Allied Dot City Intrinsic AA", &_rule_set.AlliedDotCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Allied Reference City Intrinsic AA", &_rule_set.AlliedReferenceCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Allied Unimproved Fortress Intrinsic AA", &_rule_set.AlliedUnImprovedFortressAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Allied Improved Fortress Intrinsic AA", &_rule_set.AlliedImprovedFortressAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Allied Airfield Intrinsic AA", &_rule_set.AlliedAirfieldAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Axis Full City Intrinsic AA", &_rule_set.AxisFullCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Axis Partial City Intrinsic AA", &_rule_set.AxisPartialCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Axis Dot City Intrinsic AA", &_rule_set.AxisDotCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Axis Reference City Intrinsic AA", &_rule_set.AxisReferenceCityAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Axis Unimproved Fortress Intrinsic AA", &_rule_set.AxisUnImprovedFortressAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Axis Improved Fortress Intrinsic AA", &_rule_set.AxisImprovedFortressAA, 0, 10 );
	dlg.AddIntRule( RPAGE_AIR, "Axis Airfield Intrinsic AA", &_rule_set.AxisAirfieldAA, 0, 10 );

	// naval
	RuleOption nav_opts[] =
	{
		{ "Separate capital ships (FitE/SE)", TRUE },
		{ "Task forces, carrier groups (SF)", FALSE }
	};
	dlg.AddRule( RPAGE_NAVAL, "Naval system", &_rule_set.OldNavalSystem, nav_opts, 2 );

	dlg.AddBoolRule( RPAGE_NAVAL, "5 separate naval phases (SF)", &_rule_set.SeparateNavalPhases );
	dlg.AddBoolRule( RPAGE_NAVAL, "Ports increase CD strength (min/std: 1, maj/gt: 2, fortss: 4)", &_rule_set.EnableCDCalc );

	// stacking limits
	RuleOption stc_opts[] =
	{
		{ "Number of units", StackingMode::NONDIV_UNIT },
		{ "RE's", StackingMode::NONDIV_RE }
	};
	dlg.AddRule( RPAGE_STACKING, "Calculate non-divisional units per", &_rule_set.StackNDivType, stc_opts, 2 );

	dlg.AddIntRule( RPAGE_STACKING, "Normal: divisions", &_rule_set.StackDivs, 1, 12 );
	dlg.AddIntRule( RPAGE_STACKING, "Normal: non-divisional units", &_rule_set.StackNDivs, 1, 12 );
	dlg.AddIntRule( RPAGE_STACKING, "Normal: artillery units", &_rule_set.StackArt, 1, 10 );
	dlg.AddIntRule( RPAGE_STACKING, "Mountain: divisions", &_rule_set.StackMDivs, 1, 10 );
	dlg.AddIntRule( RPAGE_STACKING, "Mountain: non-divisional units", &_rule_set.StackMNDivs, 1, 12 );
	dlg.AddIntRule( RPAGE_STACKING, "Mountain: artillery units", &_rule_set.StackMArt, 1, 10 );
	dlg.AddIntRule( RPAGE_STACKING, "Arctic: divisions", &_rule_set.StackADivs, 1, 10 );
	dlg.AddIntRule( RPAGE_STACKING, "Arctic: non-divisional units", &_rule_set.StackANDivs, 1, 12 );
	dlg.AddIntRule( RPAGE_STACKING, "Arctic: artillery units", &_rule_set.StackAArt, 1, 10 );
	dlg.AddBoolRule( RPAGE_STACKING, "Allow one fortification unit (WW1)", &_rule_set.StackWW1AllowOneFort );

	if ( dlg.ShowModal( ) != wxID_OK )
		return;
	dlg.CloseDialog( );  // copies results to rules
}

void Frame::saveSettings( )
{
#if defined __WXMSW__
	wxRegConfig config( wxT("jet") );
#else
	wxFileConfig config(wxT("jet"));
#endif
	config.Write( wxT("ShowAxisAtt"), (long)_cache_axis_attack_strengths );
	config.Write( wxT("ShowAlliedAtt"), (long)_cache_allied_attack_strengths );
	config.Write( wxT("ShowWeather"), (long)_show_weather );
	config.Write( wxT("QScrollEna"), (long)_quick_scroll_enabled );
	config.Write( wxT("QScrollMir"), (long)_quick_scroll_mirrored );
	config.Write( wxT("QScrollVel"), (long)_quick_scroll_velocity );
	config.Write( wxT("MakeBackups"), (long) ( _backup_on_save ) );
	config.Write( wxT("ShowSplash"), (long)_show_splash );
	config.Write( wxT("TransparentText"), (long)_transparent_text );
	config.Write( wxT("GrayHexes"), (long)_gray_out_hexes );
	config.Write( wxT("ClickToMove"), (long)_use_point_and_click_move );
	config.Write( wxT("RecentFile1"), _recent_file1 );
	config.Write( wxT("RecentFile2"), _recent_file2 );
	config.Write( wxT("RecentFile3"), _recent_file3 );
	config.Write( wxT("RecentFile4"), _recent_file4 );
	config.Write( wxT("RecentFile5"), _recent_file5 );
	config.Write( wxT("ShowMpsOnMap"), (long)_show_MPs_on_map );
	//config.Write( wxT("OldMapSymbols"), (long)_show_flagpoles_on_map );
	config.Write( wxT("UseCorpsMarkers"), (long)_show_corps_markers_on_map );
	config.Write( wxT("TileSet"), (long)_tile_set );

	int v = 0;
	if ( hbmAllied == hbmAlliedUS )
		v = 1;
	else if ( hbmAllied == hbmAlliedBr )
		v = 2;

	config.Write( wxT("AlliedOwnerSym"), (long)v );
}

// Options->Settings stuff from file
void Frame::loadSettings( )
{
#if defined __WXMSW__
	wxRegConfig config( wxT("jet") );
#else
	wxFileConfig config( wxT("jet") );
#endif
	config.Read( wxT("ShowAxisAtt"), (long*)&_cache_axis_attack_strengths );
	config.Read( wxT("ShowAlliedAtt"), (long*)&_cache_allied_attack_strengths );
	config.Read( wxT("ShowWeather"), (long*)&_show_weather );
	config.Read( wxT("QScrollEna"), (long*)&_quick_scroll_enabled );
	config.Read( wxT("QScrollMir"), (long*)&_quick_scroll_mirrored );
	config.Read( wxT("QScrollVel"), (long*)&_quick_scroll_velocity );
	config.Read( wxT("MakeBackups"), (long*)&_backup_on_save );
	config.Read( wxT("ShowSplash"), (long*)&_show_splash );
	if ( _show_splash == FALSE )  // 2.1: re-show splash for lazy bums who send no
		_show_splash = TRUE;  //      postcards  >:-(
	config.Read( wxT("TransparentText"), (long*)&_transparent_text );
	config.Read( wxT("GrayHexes"), (long*)&_gray_out_hexes );
	config.Read( wxT("ClickToMove"), (long*)&_use_point_and_click_move );
	config.Read( wxT("RecentFile1"), &_recent_file1 );
	config.Read( wxT("RecentFile2"), &_recent_file2 );
	config.Read( wxT("RecentFile3"), &_recent_file3 );
	config.Read( wxT("RecentFile4"), &_recent_file4 );
	config.Read( wxT("RecentFile5"), &_recent_file5 );
	config.Read( wxT("ShowMpsOnMap"), (long*)&_show_MPs_on_map );
	//config.Read( wxT("OldMapSymbols"), (long*)&_show_flagpoles_on_map );
	config.Read( wxT("UseCorpsMarkers"), (long*)&_show_corps_markers_on_map );
	config.Read( wxT("TileSet"), (long*)&_tile_set );

	int v;
	config.Read( wxT("AlliedOwnerSym"), (long*)&v );

	if ( v == 1 )
		hbmAllied = hbmAlliedUS;
	else if ( v == 2 )
		hbmAllied = hbmAlliedBr;
	else
		hbmAllied = hbmAlliedSov;
}

void Frame::CmShowpath( wxCommandEvent& WXUNUSED(event) )
{
	_show_path = _show_path == TRUE ? FALSE : TRUE;
	SetMarkerButtons( );
	map_->Refresh( false );
}

void Frame::CmFileinsert( wxCommandEvent& WXUNUSED(event) )  // not used
{
	wxMessageBox( wxT("TODO") );
}

void Frame::CmCreateUnit( wxCommandEvent& WXUNUSED(event) )  // not used
{
	wxMessageBox( wxT("TODO") );
}

void Frame::cmAdmin( wxCommandEvent& WXUNUSED(event) )
{
	MoveMode = MovementMode::ADMIN;
	SetMoveModeButtons( );
	map_->showSelUnitMovementArea( );
}

void Frame::CmStrategic( wxCommandEvent& WXUNUSED(event) )
{
	MoveMode = MovementMode::STRATEGIC;
	SetMoveModeButtons( );
	map_->showSelUnitMovementArea( );
}

void Frame::CmTactical( wxCommandEvent& WXUNUSED(event) )
{
	MoveMode = MovementMode::TACTICAL;
	SetMoveModeButtons( );
	map_->showSelUnitMovementArea( );
}

void Frame::CmSupply( wxCommandEvent& WXUNUSED(event) )
{
	map_->showSelUnitSupplyRange( );
}

void Frame::cmHexOwner( wxCommandEvent& WXUNUSED(event) )
{
	map_->show_owner_ = map_->show_owner_ == TRUE ? FALSE : TRUE;
	SetMarkerButtons( );
	map_->Refresh( false );
}

void Frame::CmTeleport( wxCommandEvent& WXUNUSED(event) )
{
	_teleport_mode_on = ( _teleport_mode_on == TRUE ? FALSE : TRUE );
}

void Frame::CmAlPartisans( wxCommandEvent& WXUNUSED(event) )
{
	if ( _rule_set.HideAlPartisans == TRUE )
	{
		const wxChar* unhidePartisanMessage = wxT("Are you sure you want to un-hide all SidePlayer::ALLIED partisans?");
		if ( wxMessageBox( unhidePartisanMessage, Application::NAME, wxYES_NO ) != wxYES )
			return;
		_rule_set.HideAlPartisans = FALSE;
	}
	else
		_rule_set.HideAlPartisans = TRUE;

	SetMarkerButtons( );
	map_->rePaint( );
	contentsPane->Refresh( true );
}

void Frame::CmPeekPartisans( wxCommandEvent& WXUNUSED(event) )
{
	map_->peekAtSelectedPartisans( );
}

void Frame::CmAxPartisans( wxCommandEvent& WXUNUSED(event) )
{
	if ( _rule_set.HideAxPartisans == TRUE )
	{
		const wxChar* unhidePartisanMessage = wxT("Are you sure you want to un-hide all SidePlayer::AXIS partisans?");
		if ( wxMessageBox( unhidePartisanMessage, Application::NAME, wxYES_NO ) != wxYES )
			return;
		_rule_set.HideAxPartisans = FALSE;
	}
	else
		_rule_set.HideAxPartisans = TRUE;

	SetMarkerButtons( );
	map_->rePaint( );
	contentsPane->Refresh( true );
}

// TODO: this isn't really a facility -- move this functionality to an appropriate class
void Frame::CmAddcontested( wxCommandEvent& WXUNUSED(event) )
{
	map_->addFacility( Facility::CONTEST );
}

void Frame::CmAddBarrage( wxCommandEvent& WXUNUSED(event) )
{
	map_->addFacility( Facility::BALLOON );
}

void Frame::CmSetAllSizes( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAllREs( );
}
void Frame::CmSetAllZOCs( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetAllZOCs( );
}
void Frame::CmRemoveCMarkers( wxCommandEvent& WXUNUSED(event) )
{
	map_->RmCMarkers( );
}

void Frame::CmSearchHex( wxCommandEvent& WXUNUSED(event) )
{
	map_->Search( 0 );
}
void Frame::CmSearchCity( wxCommandEvent& WXUNUSED(event) )
{
	map_->Search( 1 );
}
void Frame::CmSearchUnit( wxCommandEvent& WXUNUSED(event) )
{
	map_->Search( 2 );
}
void Frame::CmSearchAgain( wxCommandEvent& WXUNUSED(event) )
{
	map_->Search( -1 );
}

void Frame::CmNotDisrupted( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetDisruption( DisruptionStatus::NOT_DISRUPTED );
}

void Frame::CmDisrupted( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetDisruption( DisruptionStatus::DISRUPTED );
}

void Frame::CmBadDisrupted( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetDisruption( DisruptionStatus::BADLY_DISRUPTED );
}

void Frame::CmRemoveDisruption( wxCommandEvent& WXUNUSED(event) )
{
	map_->SetDisruption( DisruptionStatus::UNDISRUPT_ALL );
}

void Frame::CmGaugeAll( wxCommandEvent& WXUNUSED(event) )
{
	map_->MagicGauge( 1 );
}

void Frame::CmGaugeVisible( wxCommandEvent& WXUNUSED(event) )
{
	map_->MagicGauge( 2 );
}

void Frame::CmClearAll( wxCommandEvent& WXUNUSED(event) )
{
	map_->MagicClearance( 1 );
}

void Frame::CmClearVisible( wxCommandEvent& WXUNUSED(event) )
{
	map_->MagicClearance( 2 );
}

void Frame::CmAnalyzeDie( wxCommandEvent& WXUNUSED(event) )
{
	map_->analyzeGroundCombatReports( );
}

void Frame::CmRailUsage( wxCommandEvent& WXUNUSED(event) )
{
	map_->show_rail_usage( );
}

void Frame::CmHideUnits( wxCommandEvent& WXUNUSED(event) )
{
	_show_units = ( _show_units ? false : true );

	((wxMenuBar*)GetMenuBar())->Check( Frame::MenuEvent::HIDEUNITS, _show_units );

	((wxToolBar*)GetToolBar())->ToggleTool( Frame::MenuEvent::HIDEUNITS, _show_units );

	map_->Refresh( );
}

void Frame::CmChangeOrigOwner( wxCommandEvent& WXUNUSED(event) )
{
	map_->ChangeOrigOwner( );
}

void Frame::CmChangeOrigOwnerAll( wxCommandEvent& WXUNUSED(event) )
{
	map_->ChangeOrigOwner( 1 );
}

void Frame::CmChangeOrigOwnerVisible( wxCommandEvent& WXUNUSED(event) )
{
	map_->ChangeOrigOwner( 2 );
}

void Frame::CmShowHits( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 17 );
}

void Frame::UpdateViewData( )
{
	map_->UpdateViewData( );
}

void Frame::CmHiliteunmoved( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 18 );
}

void Frame::CmHiliteNormalGaugeRail( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 19 );
}

void Frame::CmHiliteWideGaugeRail( wxCommandEvent& WXUNUSED(event) )
{
	map_->HiliteStuff( 20 );
}
//void Frame::CmChangeNationsSide(wxCommandEvent& event) { Map->SetNationSide(); }

// override wxEvtHandler::ProcessEvent(wxEvent&)
/*
 bool Frame::ProcessEvent(wxEvent& event) {
 static wxEvent* s_lastEvent = nullptr;
 if ( & WXUNUSED(event) == s_lastEvent )
 return false;
 if ( event.IsCommandEvent() &&
 !event.IsKindOf(CLASSINFO(wxChildFocusEvent)) &&
 !event.IsKindOf(CLASSINFO(wxContextMenuEvent))) {
 s_lastEvent = & event;
 wxControl* focusWin = wxDynamicCast( FindFocus(), wxControl);
 bool success = false;
 if ( focusWin )
 success = focusWin->GetEventHandler()->ProcessEvent( WXUNUSED(event) );
 if ( !success )
 success = wxFrame::ProcessEvent( WXUNUSED(event) );
 s_lastEvent = nullptr;
 return success;
 } else {
 return wxFrame::ProcessEvent( WXUNUSED(event) );
 }
 }
 */

/*
 void Frame::OnKey(wxKeyEvent& event) {
 wxCommandEvent e;

 if (UnitEditorActive) {
 event.Skip();
 return;
 }
 // ASCII characters
 if (event.m_controlDown) {
 switch (event.m_keyCode) {
 case 'T':
 if (Teleport) {
 Teleport = 0;
 } else {
 Teleport = 1;
 }
 //            frame->SpeedBar->ToggleTool(CM_TELEPORT, Teleport);
 SpeedBar->ToggleTool(CM_TELEPORT, Teleport);
 break;
 case 'O':
 //            frame->CmChangeOwner(e);
 CmChangeOwner(e);
 break;
 case 'N':
 //            frame->CmMakeNeutral(e);
 CmMakeNeutral(e);
 break;
 case 'S':
 //            frame->CmChgsupply(e);
 CmChgsupply(e);
 break;
 case 'I':
 //            frame->CmIsolated(e);
 CmIsolated(e);
 break;
 case 'F':
 //            frame->CmSearchUnit(e);
 CmSearchUnit(e);
 break;
 default:
 event.Skip();
 }
 } else {
 // special keys
 switch (event.m_keyCode) {
 case 'T':
 if (Teleport)
 Teleport = 0;
 else
 Teleport = 1;
 //            frame->SpeedBar->ToggleTool(CM_TELEPORT, Teleport);
 SpeedBar->ToggleTool(CM_TELEPORT, Teleport);
 break;
 case 'O':
 //            frame->CmChangeOwner(e);
 CmChangeOwner(e);
 break;
 case 'L':
 //            frame->CmChangeOrigOwner(e);
 CmChangeOrigOwner(e);
 break;
 case 'N':
 //            frame->CmMakeNeutral(e);
 CmMakeNeutral(e);
 break;
 case 'S':
 //            frame->CmChgsupply(e);
 CmChgsupply(e);
 break;
 case 'I':
 //            frame->CmIsolated(e);
 CmIsolated(e);
 break;
 case WXK_NUMPAD1:
 //        	frame->CmUnitSouthWest(e);
 CmUnitSouthWest(e);
 break;
 case WXK_NUMPAD2:
 break;
 case WXK_NUMPAD3:
 //        	frame->CmUnitSouthEast(e);
 CmUnitSouthEast(e);
 break;
 case WXK_NUMPAD4:
 //        	frame->CmUnitWest(e);
 CmUnitWest(e);
 break;
 case WXK_NUMPAD5:
 //        	frame->CmUnitHome(e);
 CmUnitHome(e);
 break;
 case WXK_NUMPAD6:
 //        	frame->CmUnitEast(e);
 CmUnitEast(e);
 break;
 case WXK_NUMPAD7:
 //        	frame->CmUnitNorthWest(e);
 CmUnitNorthWest(e);
 break;
 case WXK_NUMPAD8:
 break;
 case WXK_NUMPAD9:
 //        	frame->CmUnitNorthEast(e);
 CmUnitNorthEast(e);
 break;
 case WXK_F3:
 //            frame->CmSearchAgain(e);
 CmSearchAgain(e);
 break;
 case WXK_LEFT :
 //            frame->Map->MoveSelection(4);
 Map->MoveSelection(4);
 break;
 case WXK_RIGHT :
 //            frame->Map->MoveSelection(6);
 Map->MoveSelection(6);
 break;
 case WXK_UP :
 //            frame->Map->MoveSelection(8);
 Map->MoveSelection(8);
 break;
 case WXK_DOWN :
 //            frame->Map->MoveSelection(2);
 Map->MoveSelection(2);
 break;
 case WXK_ESCAPE :
 //            frame->Map->Cancel();
 Map->Cancel();
 break;
 default :
 event.Skip();
 break;
 }
 }
 }
 */
#endif
