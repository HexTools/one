#include <wx/wx.h>
#include <wx/textfile.h>
#if defined HEXTOOLSPLAY
#include <algorithm>
#include <vector>
#include <wx/listctrl.h>
#endif

#include "hextools.h"
#include "hextype.h"

#if defined HEXTOOLSPLAY
#include "gamesetting.h"
#include "rules.h"
#include "counter.h"
#include "groundunit.h"
#include "movementcategory.h"
#include "weatherzone.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunitsubtype.h"
#include "airunit.h"
#include "airunitsubtype.h"
#include "navalunitsubtype.h"
#include "navalunittype.h"
#include "groundunitreplpool.h"
#include "airunitreplpool.h"

#include "daafire.h"
#include "dairunit.h"
#include "dairunitbombingmission.h"
#include "dairunitoperation.h"
#include "ddisplaysettings.h"
#include "dfacility.h"
#include "dgroundunit.h"
#include "dhex.h"
#include "dhexnote.h"
#include "dnavalunit.h"
#include "dnavalunitship.h"
#include "dnewgame.h"
#include "dstatusmarker.h"
#include "dtimejump.h"
#include "dview.h"
#include "dweather.h"
#endif // HEXTOOLSPLAY

// some const definitions must be explicitly extern'd for linkage

// global data common to both MAP/ and PLAY/maprenderer (hexes, fonts, weather strings)

// HexTools window default size
// TODO: not really global, only used in class Frame (frame.cpp)
int _frame_width = 900;
int _frame_height = 700;

char msg[10000];						// tmp buffer for user messages

HexType _hex_types[HexType::TERRAINTYPECOUNT];	// table that consists of all terraintypes

// for ht::logDebugString( )
wxTextFile _htlog;  // generic log file for debugging

const char* _weather_zone_string[] =
{
#include "wzstring.defs"
};

// TODO: const keyword implies internal linkage...
//	need to add extern even though this is a definition
extern const wxString _land_district[][2] =
{
#include "district.defs"
};

// TODO: const keyword implies internal linkage...
//	need to add extern even though this is a definition
extern const wxString _sea_circle[][2] =
{
#include "seacircle.defs"
};

// MAP/ -only globals

#if defined HEXTOOLSMAP

extern const int _side_width =		240;  // side bar (left) width
extern const int _infopane_height =	170;  // info bar (top-left) height

int _map_version;

#endif

// PLAY/ -only globals

#if defined HEXTOOLSPLAY

extern const int _side_width =		210;  // side bar (left) width
extern const int _infopane_height =	190;  // info bar (top-left) height

int _scn_version;

Rules _rule_set;

// TODO: const keyword implies internal linkage...
//	need to add extern even though this is a definition
extern const wxString _land_country[][2] =
{
#include "country.defs"
};

// TODO: const keyword implies internal linkage...
//	need to add extern even though this is a definition
extern const wxString _sea_zone[][2] =
{
#include "seazone.defs"
};

// TODO: not used anywhere, but it is stored and loaded from the .scn file
int _wturns[WeatherZone::COUNT];  // for freezing calculations

// although this is a definition, the extern keyword sets
//	*external* linkage (overriding const's implicit internal linkage)
extern const wxString _weather_condition_string[5] =
{
	_("Clear"),
	_("Frost"),
	_("Mud"),
	_("Snow"),
	_("Winter")
};

GroundUnitType _ground_unit_type[GroundUnitType::WW2::UNITTYPECOUNT];	// ground unit types
GroundArmedForce _ground_armed_force[2][GroundArmedForce::Allied::COUNT];					// ground armed forces, per side
AirArmedForce _air_armed_force[2][AirArmedForce::Allied::COUNT];		// air armed forces, per side
NavalArmedForce _naval_armed_force[2][NavalArmedForce::Allied::COUNT];	// naval armed forces, per side

GroundUnitReplPool _repl_pool[2];	// per side
AirUnitReplPool _air_repl_pool[2];

AADlgXfer _aa_dialog_data;
AddAirUnitXfer _add_airunit_dialog_data;
AirMissionDlgXfer _air_mission_dialog_data;
TakeOffDlgXfer _airunit_takeoff_dialog_data;
DisplayOptionsDlgXfer _display_options_dialog_data;
MiscDlgXfer _misc_dialog_data;
UnitDlgXfer _unit_dialog_data;
HexDlgXfer _hex_dialog_data;
NoteDlgXfer _note_dialog_data;
FleetDlgXfer _fleet_dialog_data;
ShipDlgXfer _ship_dialog_data;
NewGameDlgXfer _new_game_dialog_data;
PtsDlgXfer _points_dialog_data;
TurnDlgXfer _turn_dialog_data;
ViewDlgXfer _view_dialog_data;
WeatherDlgXfer _weather_dialog_data;

// first and last row in current CRT
int _crt_lowest_die_roll = -1;
int _crt_highest_die_roll = 8;

int _crt[13][15];

std::vector<std::string> CRTResult;
std::vector<std::string> _custom_result;

int _armed_force_filter = GameSetting::ALL;

int _show_combat_markers;  // show combat markers in the map?
int _show_path = TRUE;
int _show_splash = TRUE;
int _transparent_text = FALSE;  // this in not currently adjustable (but works)
int _gray_out_hexes = TRUE;  // gray unreachable hexes active
int _use_point_and_click_move = TRUE;  // point'n'click moving active
int _backup_on_save = TRUE;  // make backup copies when saving scenarios

// TODO: not used anywhere, but it is stored and loaded
int _tile_set = 0;  // map graphics tiles, 0=standard

int _current_player;	// AXIS or ALLIED
int _current_phase;		// COMBAT_PHASE etc.
int _previous_phase;	// Phase before setup-mode
int _year;				// 42 etc...
int _month;				// 0...11  (0=Jan, 11=Dec, used as on index)
int _turn;				// 1 or 2 (I or II)
//int _naval_phase;		// if separate navalphases, this is current naval phase
int _turns_per_month;	// turns per month

int _phasing_player;  // who is _phasing_ player (Player may be non-phasing too)
//int _first_player;// who moves first

//int _use_flat_buttons = TRUE;   // flat toolbar buttons?
int _quick_scroll_enabled = TRUE;
int _quick_scroll_mirrored = TRUE;
int _quick_scroll_velocity = 4;
bool _show_weather = true; // weather aware terrain drawing
int _cache_axis_attack_strengths = TRUE;
int _cache_allied_attack_strengths = TRUE;

int _weather_condition[WeatherZone::COUNT];		// W_MUD etc., per zone
int _rivers_lakes_frozen[WeatherZone::COUNT];	// true if rivers/lakes are frozen
//int wturns[WeatherZone::COUNT];				// for freezing calculations

int _show_MPs_on_map = FALSE;
int _show_corps_markers_on_map = FALSE;

int _teleport_mode_on = FALSE;  // click to teleport selected units?
bool _show_units = true;  // clear to hide units from map (in order to see geography better)
bool _unit_editor_active = false;  // king of all kludges: set this when dialog box is visible so keypresses don't cause trouble!

Counter* _selected_unit_list = nullptr;  // list of the selected units;

wxString _recent_file1;
wxString _recent_file2;
wxString _recent_file3;
wxString _recent_file4;
wxString _recent_file5;

wxWindow* _parent_dialog_window;  // parent wnd for dialogs (used in dialogs)

// draw corps markers 18X14 256 colors
// for static draw methods:
extern const int _counter_width = 18;
extern const int _counter_height = 13;

// order must match SHIP_xx #defines
const char* _ship_types[NavalUnitType::Ship::COUNT] =
{
	"V",
	"VE",
	"VL",
	"VS",
	"B",
	"BB",
	"BC",
	"BD",
	"BP",
	"CA",
	"CC",
	"CD",
	"CL",
	"DA",
	"DC",
	"DD",
	"DE",
	"DT",
	"FF",
	"GB",
	"LC",
	"MW",
	"NT",
	"RF",
	"TB",
	"SC",
	"SS",
	"SO"
};

const char* _ship_type_string[NavalUnitType::Ship::COUNT] =
{
	"Fleet Carrier",
	"Escort Carrier",
	"Light Carrier",
	"Seaplane Carrier",
	"Old (WWI) Battleship",
	"Battleship",
	"Battlecruiser",
	"Coast Defense Battleship",
	"Pocket Battleship",
	"Heavy Cruiser",
	"Coast Defense Cruiser",
	"Heavy Destroyer",
	"Light Cruiser",
	"AA Cruiser",
	"Obsolete Light Cruiser",
	"Destroyer",
	"Destroyer Escort",
	"Heavy Torpedo Boat",
	"Frigate",
	"Gunboat",
	"Landing Craft",
	"Minewarfare",
	"Naval Transport",
	"River Flotilla",
	"Torpedo Boat",
	"Coastal Submarine",
	"Sea going Submarine",
	"Ocean Going Submarine"
};

const char* _fleet_type[NavalUnitType::Fleet::COUNT] =
{
	"NTP",
	"LC",
	"TF",
	"CG",
	"SS",
	"RF",
	"BB",
	"BP",
	"BD",
	"B",
	"BC",
	"CV",
	"CA",
	"CL",
	"CD",
	"DD"
};

const char* _fleet_type_name[NavalUnitType::Fleet::COUNT] =
{
	"Naval Transport",
	"Landing Craft",
	"Task Force",
	"Carrier Group",
	"Submarine Fleet",
	// SOS type task forces:
	"River Flotilla",
	"Battleship",
	"Pocket Battleship",
	"Coastal Defense Battleship",
	"WWI Class Battleship",
	"Battlecruiser",
	"Carrier",
	"Heavy Cruiser",
	"Light Cruiser",
	"Coastal Defense Cruiser",
	"Destroyer Flotilla"
};

// TODO: although this is a definition, const's implicit
//	internal linkage needs extern for external linkage
// used in mapfile.cpp
const char* _crt_odds_string[] = // 13
{
	"1:4",
	"1:3",
	"1:2",
	"1:1",
	"1.5:1",
	"2:1",
	"3:1",
	"4:1",
	"5:1",
	"6:1",
	"7:1",
	"8:1",
	"9:1"
};

// TODO: although this is a definition, const's implicit
//	internal linkage needs extern for external linkage
// used in hexnote.cpp
const char* _player_string[] =
{
	"Axis",
	"Allied",
	"Neutral"
};

// TODO: although this is a definition, const's implicit
//	internal linkage needs extern for external linkage
// used in mappane.cpp, mappane2.cpp, and hexnote.cpp
const char* _month_string[] = // 12
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

// TODO: although this is a definition, const's implicit
//	internal linkage needs extern for external linkage
// used in mappane.cpp, mappane2.cpp, and hexnote.cpp
const char* _turn_numeral[] = // 20
{
	"I",
	"II",
	"III",
	"IV",
	"V",
	"VI",
	"VII",
	"VIII",
	"IX",
	"X",
	"XI",
	"XII",
	"XIII",
	"XIV",
	"XV",
	"XVI",
	"XVII",
	"XVIII",
	"XIX",
	"XX"
};

// TODO: this should be extern const to override const's implicit
//	internal linkage with external linkage (used by dtimejump.cpp)
const char* _phase_string[] = // LASTPHASE + 1
{
	"No Game",
	"Setup",
	"Initial",
	"1st Naval",	// opt
	"2nd Naval",	// opt
	"3rd Naval",	// opt
	"4th Naval",	// opt
	"5th Naval",	// opt
	"Movement",
	"Reaction",		// opt
	"Combat",
	"React move",	// opt
	"React cmbt",	// opt
	"6th Naval",	// opt
	"7th Naval",	// opt
	"8th Naval",	// opt
	"9th Naval",	// opt
	"10th Naval",	// opt
	"Exploitation"
};

// which axis subtype maps to which allied subtype {axis, allied}
#define SIDEMAP_SIZE 24
int _unit_subtype_side_map[SIDEMAP_SIZE][2] =
{
	{ GroundArmedForce::Axis::FIN, GroundArmedForce::Allied::AL_FIN },
	{ GroundArmedForce::Axis::ITA, GroundArmedForce::Allied::AL_ITA },
	{ GroundArmedForce::Axis::AX_TURK, GroundArmedForce::Allied::AL_TURK },
	{ GroundArmedForce::Axis::AX_IRAQ, GroundArmedForce::Allied::AL_IRAQ },
	{ GroundArmedForce::Axis::AX_TJ, GroundArmedForce::Allied::AL_TJ },
	{ GroundArmedForce::Axis::AX_IRAN, GroundArmedForce::Allied::AL_IRAN },
	{ GroundArmedForce::Axis::AX_SWE, GroundArmedForce::Allied::AL_SWE },
	{ GroundArmedForce::Axis::AX_EST, GroundArmedForce::Allied::AL_EST },
	{ GroundArmedForce::Axis::AX_YUGO, GroundArmedForce::Allied::YUGO },
	{ GroundArmedForce::Axis::AX_SOV, GroundArmedForce::Allied::SOV },
	{ GroundArmedForce::Axis::AX_GUARDS, GroundArmedForce::Allied::GUARDS },
	{ GroundArmedForce::Axis::AX_NKVD, GroundArmedForce::Allied::NKVD },
	{ GroundArmedForce::Axis::AX_SOVNAVY, GroundArmedForce::Allied::SOVNAVY },
	{ GroundArmedForce::Axis::AX_LITH, GroundArmedForce::Allied::AL_LITH },
	{ GroundArmedForce::Axis::AX_SPA, GroundArmedForce::Allied::AL_SPA },
	{ GroundArmedForce::Axis::AX_POR, GroundArmedForce::Allied::AL_POR },
	{ GroundArmedForce::Axis::AX_EGY, GroundArmedForce::Allied::AL_EGY },
	{ GroundArmedForce::Axis::AX_BUL, GroundArmedForce::Allied::BUL },
	{ GroundArmedForce::Axis::AXFRENCH, GroundArmedForce::Allied::FRENCH },
	{ GroundArmedForce::Axis::AXFR_AFR, GroundArmedForce::Allied::FR_AFR },
	{ GroundArmedForce::Axis::AXFR_COL, GroundArmedForce::Allied::FR_COL },
	{ GroundArmedForce::Axis::AXFR_FOR, GroundArmedForce::Allied::FR_FOR },
	{ GroundArmedForce::Axis::AX_WW1_DUT, GroundArmedForce::Allied::AL_WW1_DUT },
	{ GroundArmedForce::Axis::AX_WW1_DUTC, GroundArmedForce::Allied::AL_WW1_DUTC }
// TODO: Romania, and  other Eastern European nations...
};

#define AIRSIDEMAP_SIZE 16
int _airunit_subtype_side_map[AIRSIDEMAP_SIZE][2] =
{
	{ AirArmedForce::Axis::AIR_FIN, AirArmedForce::Allied::AIR_ALFIN },
	{ AirArmedForce::Axis::AIR_ITA, AirArmedForce::Allied::AIR_ALITA },
	{ AirArmedForce::Axis::AIR_AXTRK, AirArmedForce::Allied::AIR_ALTRK },
	{ AirArmedForce::Axis::AIR_AXIRAQ, AirArmedForce::Allied::AIR_ALIRAQ },
// TJ
	{ AirArmedForce::Axis::AIR_AXIRAN, AirArmedForce::Allied::AIR_ALIRAN },
	{ AirArmedForce::Axis::AIR_AXSWE, AirArmedForce::Allied::AIR_ALSWE },
	{ AirArmedForce::Axis::AIR_AXEST, AirArmedForce::Allied::AIR_ALEST },
	{ AirArmedForce::Axis::AIR_AXYG, AirArmedForce::Allied::AIR_YUGO },
	{ AirArmedForce::Axis::AIR_AXSOV, AirArmedForce::Allied::AIR_SOV },
	{ AirArmedForce::Axis::AIR_AXGUARD, AirArmedForce::Allied::AIR_GUARDS },
	// nkvd
	// sovnavy
	{ AirArmedForce::Axis::AIR_AXLITH, AirArmedForce::Allied::AIR_ALLITH },
	{ AirArmedForce::Axis::AIR_AXSPA, AirArmedForce::Allied::AIR_ALSPA },
	{ AirArmedForce::Axis::AIR_AXPOR, AirArmedForce::Allied::AIR_ALPOR },
	{ AirArmedForce::Axis::AIR_AXEGY, AirArmedForce::Allied::AIR_ALEGY },
	{ AirArmedForce::Axis::AIR_AXBUL, AirArmedForce::Allied::AIR_ALBUL },
	{ AirArmedForce::Axis::AIR_AXVICH, AirArmedForce::Allied::AIR_FRENCH }
// 3*french
};

#define SHIPSIDEMAP_SIZE 15
int _navalunit_subtype_side_map[SHIPSIDEMAP_SIZE][2] =
{
	{ NavalArmedForce::Axis::NAV_FIN, NavalArmedForce::Allied::NAV_AL_FIN },
	{ NavalArmedForce::Axis::NAV_ITA, NavalArmedForce::Allied::NAV_AL_ITA },
	{ NavalArmedForce::Axis::NAV_AX_TURK, NavalArmedForce::Allied::NAV_AL_TURK },
	{ NavalArmedForce::Axis::NAV_AX_IRAQ, NavalArmedForce::Allied::NAV_AL_IRAQ },
	// TJ
	{ NavalArmedForce::Axis::NAV_AX_IRAN, NavalArmedForce::Allied::NAV_AL_IRAN },
	{ NavalArmedForce::Axis::NAV_AX_SWE, NavalArmedForce::Allied::NAV_AL_SWE },
	{ NavalArmedForce::Axis::NAV_AX_EST, NavalArmedForce::Allied::NAV_AL_EST },
	{ NavalArmedForce::Axis::NAV_AX_YUGO, NavalArmedForce::Allied::NAV_YUGO },
	// sov
	// guards
	// nkvd
	{ NavalArmedForce::Axis::NAV_AX_SOVNAVY, NavalArmedForce::Allied::NAV_SOV },
	{ NavalArmedForce::Axis::NAV_AX_LITH, NavalArmedForce::Allied::NAV_AL_LITH },

	{ NavalArmedForce::Axis::NAV_AXSPA, NavalArmedForce::Allied::NAV_ALSPA },
	{ NavalArmedForce::Axis::NAV_AXPOR, NavalArmedForce::Allied::NAV_ALPOR },
	{ NavalArmedForce::Axis::NAV_AX_EGY, NavalArmedForce::Allied::NAV_AL_EGY },
	{ NavalArmedForce::Axis::NAV_AX_BUL, NavalArmedForce::Allied::NAV_BUL },
	{ NavalArmedForce::Axis::NAV_AXVICH, NavalArmedForce::Allied::NAV_FRA }
// 3*french
};

wxBitmap* hbmHexInfoPaneBgnd = nullptr;
wxBitmap* hbmHexContentsPaneBgnd = nullptr;

// ports. these could be Map members, but minorport is needed in GroundUnit.cpp
// for port hit painting, thus all are here
wxBitmap* hbmMinorPort;
wxBitmap* hbmStandardPort;
wxBitmap* hbmMajorPort;
wxBitmap* hbmGreatPort;
wxBitmap* hbmMarginalPort;
wxBitmap* hbmAnchorage;
wxBitmap* hbmArtificialPort;
wxBitmap* hbmStrongPort;
wxBitmap* hbmDestroyed;			// in hitmarker.cpp & mappane.cpp
wxBitmap* hbmRoundPortMask;

wxBitmap* hbmAxis;
wxBitmap* hbmAlliedSov;
wxBitmap* hbmAlliedUS;
wxBitmap* hbmAlliedBr;
wxBitmap* hbmAllied;  // points to one of the above

wxBitmap* hbmUnitBgnd;

// these are accessed from the Counter subclasses
wxBitmap* hbmCompany;
wxBitmap* hbmBatallion;
wxBitmap* hbmBatallionGrp;
wxBitmap* hbmBrigade;
wxBitmap* hbmBrigadeGrp;
wxBitmap* hbmRegiment;
wxBitmap* hbmRegimentGrp;
wxBitmap* hbmDivisional;
wxBitmap* hbmDivision;
wxBitmap* hbmCorps;
wxBitmap* hbmArmy;
wxBitmap* hbmCadre;
wxBitmap* hbmDG;
wxBitmap* hbm1RE;  // for transports/APC's
wxBitmap* hbm2RE;
wxBitmap* hbm3RE;

wxBitmap* hbmWarShip;  // Battleship class
wxBitmap* hbmCruiser;
wxBitmap* hbmDestroyer;
wxBitmap* hbmAuxShip;
wxBitmap* hbmLC;
wxBitmap* hbmSubmarine;
wxBitmap* hbmCarrier;
wxBitmap* hbmCargoShip;

wxBitmap* hbmMtnMask;  // mountain symbol for mountain units

// graphics for various hit etc counters
wxBitmap* hbmSmallHit;  // tiny hit marker for hit counters
wxBitmap* hbmSmallPort;
wxBitmap* hbmCDHit;
wxBitmap* hbmHarPlane;
wxBitmap* hbmAxMine;
wxBitmap* hbmAlMine;
wxBitmap* hbmMulberry;
wxBitmap* hbmAttSupport;
wxBitmap* hbmDefSupport;
wxBitmap* hbmCapital;
wxBitmap* hbmHexMask;  // hex graying
wxBitmap* hbmEntrenchent;
wxBitmap* hbmImpFort;
wxBitmap* hbmFieldWorks;
wxBitmap* hbmZepBase;
wxBitmap* hbmContested;
wxBitmap* hbmBalloons;
wxBitmap* hbmSupplyDepot;
wxBitmap* hbmRP;
wxBitmap* hbmAlert;

// graphics for font in counters
wxBitmap* hbmFont[127];

// notes, for GroundUnit.cpp
wxBitmap* hbmNote;
wxBitmap* hbmCombatReport;
wxBitmap* hbmAirCombatReport;
wxBitmap* hbmNavalCombatReport;
wxBitmap* hbmBombReport;
wxBitmap* hbmDieRollReport;

// some toolbar bitmap ptr's from frame.cpp (for path icons)
wxBitmap* stagebmp;
wxBitmap* hitrailbmp;
wxBitmap* hitafbmp;
wxBitmap* repairbmp;
wxBitmap* regaugebmp;
wxBitmap* totrainbmp;
wxBitmap* fromtrainbmp;
wxBitmap* bombbmp;

// pens for path
wxPen* tac_pen;  // movement phase movement
wxPen* expl_pen;  // tactical exploitation movement
wxPen* combat_pen;  // advancing after combat
wxPen* adm_pen;  // administrative movement
//wxPen* opm_pen;	// operational movement
wxPen* rr_pen;  // operational rail
wxPen* str_pen;  // strategic rail
wxPen* ship_pen;  // in ship
wxPen* air_pen;  // in plane

#endif
