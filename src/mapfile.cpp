#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
using std::endl;

#include <wx/wx.h>
#include <wx/stdpaths.h>
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#include <wx/hashmap.h>
#endif

#include "hextools.h"
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
using ht::wxS;
using ht::pathname;
using ht::basename;
using ht::skipSpace;
using ht::coordinatesOutOfBounds;
#include "rules.h"
#include "sideplayer.h"
#include "counter.h"
#endif
#include "frame.h"
#include "application.h"
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#endif
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#include "hexcontents.h"
#include "phase.h"
#include "weatherzone.h"
#include "weatherclass.h"
#include "groundunit.h"
#include "groundunitreplpool.h"
#include "airunitreplpool.h"
#include "hexcontentspane.h"
#include "view.h"
#include "views.h"
#endif

#if defined HEXTOOLSMAP
extern int _map_version;
#elif defined HEXTOOLSPLAY
DECLARE_APP(Application);

extern int _scn_version;

extern HexType _hex_types[]; //[TERRAINTYPECOUNT];  // terrain types
extern const char* _crt_odds_string[];

extern int _current_player;
extern int _current_phase;		// COMBAT_PHASE etc.
extern int _previous_phase;	// Phase before setup-mode
extern int _weather_condition[];	// W_ZONES // W_MUD etc., per zone (frame.cpp)
extern int _rivers_lakes_frozen[];	// W_ZONES // true if rivers/lakes are frozen (frame.cpp)
extern int _wturns[];	// W_ZONES // for freezing calculations
extern int _year;
extern int _month;
extern int _turn;
extern int _phasing_player;
extern int _turns_per_month;
extern int _show_combat_markers;
extern Rules _rule_set;
extern GroundUnitReplPool _repl_pool[2];  // AXIS and ALLIED
extern AirUnitReplPool _air_repl_pool[2];  // AXIS and ALLIED
extern int _backup_on_save;  // make backup copies when saving scenarios

// first and last row in current CRT
extern int _crt_lowest_die_roll;	// -1
extern int _crt_highest_die_roll;	// 8

extern int _crt[][15];
//extern const int DefaultCRT[13][15];

extern std::vector<std::string> CRTResult;
//extern crt_item_t CRTResult[CRT_ITEMS];
extern std::vector<std::string> _custom_result;
//extern crt_item_t CustomResult[50];
//static int num_custom_results = 0;

// TODO: experiment with removing this: (defined mappane, extern mappane2 and mapfile)
extern char msg[10000];
#endif

void MapPane::CmInfo( wxCommandEvent& WXUNUSED( e ) )
{
	double kilo = 1024.0f;

	int hex_cnt = world_x_size_ * world_y_size_;

	double total_hex_sz = static_cast<double>( hex_cnt * sizeof( Hex ) ) / kilo;

	long city_cnt = 0;
	long default_hex_cnt = 0;
	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
		{
			city_cnt = hex_[y][x].city_ != nullptr ? city_cnt + 1 : city_cnt;
			default_hex_cnt = hex_[y][x].getMapIdWord( ) == 0xffff ? default_hex_cnt + 1 : default_hex_cnt;
		}
	int default_hex_percent = default_hex_cnt * 100 / hex_cnt;

	double total_city_sz = static_cast<double>( city_cnt * sizeof( City ) ) / kilo;

	double total_mem_sz = ( total_hex_sz + total_city_sz ) / kilo;

	// some additional disk size calculations
	// 	cities
	double total_disk_sz = total_city_sz * kilo; // from KB back to B

	//	front matter
#if defined HEXTOOLSMAP
	//	magic numbers:
	//	mean 21 B per odd (rotated, or normal w/ right offset) mapsheet,
	//  plus another 10 B for the header line:
	//		2 B file version + two 3 B hex dimensions + two 1 B whitespaces
	inventory_mapsheets( );
	total_disk_sz += ( rotated_mapsheet_count( ) + rightoffset_mapsheet_count( ) ) * 21 + 10;
#elif defined HEXTOOLSPLAY
	// TODO: fix this for HT-p
	total_disk_sz += mapsheet_count_ / 2 * 21 + 10;
#endif

	int per_hex_mean_bytes = 0;
	//	magic numbers:
	//	x24 one-B (unsigned char) fields
	//		~ 26 B (extra 2 B due to default WZ of 'D' binary 11 vice 'A' 00)
	per_hex_mean_bytes += 26; // TODO: update this if you change default field settings in hex.h
	//	x2 two-B (unsigned short) fields
	//		~ 8 B (each short int has a mean 4-digit decimal, map ID & hex ID)
	per_hex_mean_bytes += 8;
	//	x27 one-B whitespaces
	//		~ 27 B (corresponds to 27 fields, the x26 above plus the x1 after the city storage)
	per_hex_mean_bytes += 27; // TODO: update this if you add fields to hex.h
	//	mean per citi(es) across all hexes:
	//		~ 2 B
	per_hex_mean_bytes += 2;
	//	26 + 8 + 27 + 2 = ~ 63 B mean per-decimal-encoded hex

	total_disk_sz += hex_cnt * per_hex_mean_bytes;

	total_disk_sz /= kilo; // convert to KB again
	total_disk_sz /= kilo; // convert to MB

	char infomsg[512];
	sprintf(infomsg,
			"Dimensions of this map = %d (across) x %d (down) hexes.\n"
			"Hex count = %d @ %d B each ~ %5.1f kB in RAM.\n"
			"     (of those, %ld are in their default state ~ %d %%)\n"
			"City count = %ld @ %d B each ~ %5.1f kB in RAM.\n\n"
			"Estimated memory consumed by map ~%5.1f MB in RAM.\n\n"
			"Estimated space occupied by .map ~%5.1f MB on disk.",
			world_x_size_, world_y_size_,
			hex_cnt, sizeof(Hex), total_hex_sz,
			default_hex_cnt, default_hex_percent,
			city_cnt, sizeof(City), total_city_sz,
			total_mem_sz,
			total_disk_sz );

	wxMessageBox( infomsg, "Map data consumption of resources" );
#if 0
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	long cities = 0;
	for ( int y = 0; y < iYWorldSize; ++y )
		for ( int x = 0; x < iXWorldSize; ++x )
			if ( hex[y][x].CityPtr != nullptr )
				++cities;

	long csize = cities * sizeof(City);
	long hsize = sizeof(Hex);
	long size = iXWorldSize * iYWorldSize * hsize;
	long dsize = sizeof(HexContents);
	long dtot = iXWorldSize * iYWorldSize * dsize;
	long tot = size + csize + dtot;

	char infomsg[512];
	sprintf( infomsg, "Map size is %dx%d.\n"
			"Size of one hex is %ld bytes.\n"
			"Whole map takes %ld bytes\n"
			"There are %ld cities, %d bytes each -> %ld bytes\n"
			"%ld bytes of dynamic data (%ld bytes per hex)\n"
			"\n"
			"TOTAL %ld Kb", iXWorldSize, iYWorldSize, hsize, size, cities, sizeof(City), csize, dtot, dsize, tot );

	wxMessageBox( wxS( infomsg ), wxT("Map info") );
#endif
#endif
}

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
// called only by the HEXTOOLSPLAY section of loadFiles(..)
static char* descramble_passwd( char* s )
{
	static char pw[100];

	unsigned int i;

	for ( i = 0; i < strlen( s ); i += 2 )
		pw[i / 2] = static_cast<char>( ( s[i] - '0' - ( i / 2 ) ) | ( ( s[i + 1] - 'A' ) << 4 ) );

	pw[i / 2] = '\0';

	return pw;
}
#endif

// PAPI (Frame)
// 	filename is the .map, and the .scn, respectively
// load .scn file (and .map file indirectly)
bool MapPane::load_files( const char* ht_filename )
{
	wxBeginBusyCursor( );

	char map_filename[127];
#if defined HEXTOOLSMAP
	strcpy( map_filename, ht_filename ); // for HT-m, the filename is the .map
#elif defined HEXTOOLSPLAY
	char scn_filename[127];
	strcpy( scn_filename, ht_filename ); // for HT-p, the filename is the .scn

	std::ifstream scn_is( scn_filename );
	if ( ! scn_is )
	{
		std::string badstream = "Couldn't open scnfile " + std::string( scn_filename ) + "!";
		wxMessageBox( badstream, Application::NAME );
		return false;
	}

	std::string scn_ver_str;
	scn_is >> scn_ver_str;

	// check whether this an old JET scenario
	if ( scn_ver_str.compare( 0, 8, "JET/SCN:" ) == 0 )
	{
		const wxString& oldformatmsg = "This scenario file uses an old JET format!\nIf you save any changes, it will be in the new HT format.";

		wxMessageBox( oldformatmsg, wxT("Warning!"), wxOK | wxCANCEL );

		scn_ver_str.assign( scn_ver_str, 8, 2 ); // extract the 2 digits after the "JET/SCN:"
	}

	int scn_ver;
	std::stringstream( scn_ver_str ) >> scn_ver;

	if ( scn_ver > ht::SCN_VERSION ) // newer than current?!
	{
		std::ostringstream oldvermsg;
		oldvermsg << "This scenario file has version number " << scn_ver << ", but this software\n";
		oldvermsg << "only supports older version " << ht::SCN_VERSION << ", trying to load it will";
		oldvermsg << "probably crash HexTools.\n\n";
		oldvermsg << "Try to load anyway (not recommended)?";

		if ( wxMessageBox( oldvermsg.str( ), wxT("Error!"), wxOK | wxCANCEL ) != wxOK )
		{
			wxEndBusyCursor( );
			return false;
		}
	}
	else if ( scn_ver < ht::SCN_VERSION ) // older than current
	{
		std::ostringstream newvermsg;
		newvermsg << "This scenario file has version number " << scn_ver << ", but this software\n";
		newvermsg << "uses newer version " << ht::SCN_VERSION << ".  File may not contain all of the\n";
		newvermsg << "information supported by your version of HexTools.";

		wxMessageBox( newvermsg.str( ), Application::NAME, wxOK | wxICON_INFORMATION );
	}

	_scn_version = scn_ver;

	// TODO: hideUnitMover( ) impl is currently empty
	//hideUnitMover( );

	// map/scenario dimensions
	int new_y_scn_size;
	scn_is >> new_y_scn_size;

	int new_x_scn_size;
	scn_is >> new_x_scn_size;

	// map filename (w/o path) stored in scn file
	scn_is >> map_filename;

	// force map filename to have same directory name as scenario file
	char scn_path[127];
	strcpy( scn_path, pathname( scn_filename ) ); // extract path from .scn pathname
	strcat( scn_path, map_filename ); // append .map filename to same path
	strcpy( map_filename, scn_path ); // fully qualify map filename with path

	// file existence check, because the .map
	//	filename stored in the .scn may have been hacked
	FILE* fp;
	if ( ( fp = fopen( map_filename, "r" ) ) != nullptr )
		fclose( fp );
	else
	{
		// *nix filenames are case-sensitive, Windows filenames are not:
		std::ostringstream badfileloc;
		badfileloc << "Couldn't open mapfile " << map_filename << "!";
#if defined( __WXGTK__ )
		badfileloc << "This may happen if scenario was saved under Windows where" << endl;
		badfileloc << "file names are not case sensitive." << endl << endl;
#endif
		badfileloc << "Do you want to locate it yourself?" << endl;

		if ( wxMessageBox( badfileloc.str( ), Application::NAME, wxYES_NO ) != wxYES )
		{
			wxMessageBox( _("Loading aborted"), Application::NAME );
			return false;
		}
		else
		{
			const wxString &fn = wxFileSelector(	_("Choose a map file to use"),
													_(""),
													_(""),
													_("map"),
													_("HexTools Map (*.map)|*.map|All files (*.*)|*.*") );
			if ( ! fn )
			{
				wxMessageBox( _("Loading aborted"), Application::NAME );
				return false;
			}
			strcpy( map_filename, fn.ToAscii( ) );
		}
	}
#endif

	// now take a peek at the map dimensions
	std::ifstream map_is( map_filename );
	if ( ! map_is )
	{
		std::ostringstream badstream;
		badstream << "Couldn't open mapfile " << map_filename << "!";
		wxMessageBox( badstream.str( ), Application::NAME );
		wxEndBusyCursor( );
		return false;
	}

	// map version number; not needed here, just consume it
	int dummy;
	map_is >> dummy;

	// map dimensions
	int new_y_map_size;
	map_is >> new_y_map_size;

	int new_x_map_size;
	map_is >> new_x_map_size;

	map_is.close( );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	// force map dimensions to match scn dimensions
	if ( new_y_scn_size != new_y_map_size || new_x_scn_size != new_x_map_size )
	{
		std::ostringstream mapscnmismatchmsg;
		mapscnmismatchmsg << "Map " << map_filename << "does not match dimensions of ";
		mapscnmismatchmsg << "scenario " << scn_filename << "!\n\nLoad aborted.";
		wxMessageBox( mapscnmismatchmsg.str( ), Application::NAME );
		wxEndBusyCursor( );
		return false;
	}
#endif

	if ( ! load_map( map_filename ) )
		return false;

	// record the current dimensions
	//  (used for scrollbar re-initialization when a different .scn is later loaded)
	int old_y_map_size = world_y_size_;
	int old_x_map_size = world_x_size_;

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	// map load was good, so prepare the HT-p UI for the scenario load
	contents_pane_->deselectAll( );
	contents_pane_->clear( );
	ClearAllViews( );

	// setup progress bar and index
	wxClientDC hdc( this );
	draw_load_status_box( &hdc, "Loading scenario..." );

	release_sit_storage( old_y_map_size, old_x_map_size );

	reserve_sit_storage( new_y_map_size, new_x_map_size );

	// process scenario data
	scn_is >> _year;
	scn_is >> _month;
	scn_is >> _turn;
	scn_is >> _current_player;
	scn_is >> _current_phase;

	// weather data loading: different in ww2pacific-ready version ( >= 9 ):
	if ( _scn_version < 9 )
	{
		scn_is >> _weather_condition[0] >> _weather_condition[1] >> _weather_condition[2];
		scn_is >> _weather_condition[3] >> _weather_condition[4] >> _weather_condition[5];
		scn_is >> _weather_condition[6];
		scn_is >> _rivers_lakes_frozen[0] >> _rivers_lakes_frozen[1] >> _rivers_lakes_frozen[2];
		scn_is >> _rivers_lakes_frozen[3] >> _rivers_lakes_frozen[4] >> _rivers_lakes_frozen[5];
		scn_is >> _rivers_lakes_frozen[6];
		scn_is >> _wturns[0] >> _wturns[1] >> _wturns[2];
		scn_is >> _wturns[3] >> _wturns[4] >> _wturns[5];
		scn_is >> _wturns[6];
	}
	else
	{
		int weatherZoneCount = WeatherZone::COUNT;
		// before version 10, W_ZONES was 40 vice 42
		if ( _scn_version == 9 )
		{
			// only read file for 40 iterations
			weatherZoneCount -= 2;
			// but initialize the last two rows (indices 40 and 41)
			//	of WZ-related array storage:
			_weather_condition[WeatherZone::COUNT-2] = _rivers_lakes_frozen[WeatherZone::COUNT-2] = _wturns[WeatherZone::COUNT-2] = 0;
			_weather_condition[WeatherZone::COUNT-1] = _rivers_lakes_frozen[WeatherZone::COUNT-1] = _wturns[WeatherZone::COUNT-1] = 0;
		}
		for ( int i = 0; i < weatherZoneCount; ++i )
			scn_is >> _weather_condition[i] >> _rivers_lakes_frozen[i] >> _wturns[i];
	}

	// line buffer for scenario input file stream processing
	char ln[1000];

	// load rules
	while ( true )
	{
		scn_is >> ln;
		if ( strcmp( ln, "Rules>" ) == 0 )
			continue;
		else if ( strcmp( ln, "<Rules" ) == 0 )
			break;

		// game series
#if 0
		else if ( strcmp( ln, "RulesVariant" ) == 0 )
			scn_is >> _rule_set.rules_variant_;
#endif
		else if ( strcmp( ln, "GameEra" ) == 0 )
			scn_is >> _rule_set.Era;
		else if ( strcmp( ln, "TGWMPs" ) == 0 )
			scn_is >> _rule_set.WW1MPs;
		else if ( strcmp( ln, "TGWPhases" ) == 0 )
			scn_is >> _rule_set.WW1Phases;
		else if ( strcmp( ln, "TGWAirBaseCapacity" ) == 0 )
			scn_is >> _rule_set.WW1AirBaseCapacity;
		else if ( strcmp( ln, "TGWExploitMPs" ) == 0 )
			scn_is >> _rule_set.WW1ExploitMPs;
		else if ( strcmp( ln, "RailsObstructed" ) == 0 )
			scn_is >> _rule_set.RailsObstructed;

		// general

		else if ( strcmp( ln, "FirstPlayer" ) == 0 )
			scn_is >> _rule_set.FirstPlayer;
		else if ( strcmp( ln, "Turns" ) == 0 )
			scn_is >> _turns_per_month;
		else if ( strcmp( ln, "HideAxPartisans" ) == 0 )
			scn_is >> _rule_set.HideAxPartisans;
		else if ( strcmp( ln, "HideAlPartisans" ) == 0 )
			scn_is >> _rule_set.HideAlPartisans;
#if 0
		else if ( strcmp( ln, "UnitTypeFlags" ) == 0 )
			scn_is >> _rule_set.UnitTypeFlags;
#endif
		// sizes

		else if ( strcmp( ln, "AllowCustomREs" ) == 0 )
			scn_is >> _rule_set.AllowCustomREs;
		else if ( strcmp( ln, "HQRE2" ) == 0 )
			scn_is >> _rule_set.HQRE2;
		else if ( strcmp( ln, "CompanyRE2" ) == 0 )
			scn_is >> _rule_set.CompanyRE2;
		else if ( strcmp( ln, "BattalionRE2" ) == 0 )
			scn_is >> _rule_set.BattalionRE2;
		else if ( strcmp( ln, "RegimentRE2" ) == 0 )
			scn_is >> _rule_set.RegimentRE2;
		else if ( strcmp( ln, "BrigadeRE2" ) == 0 )
			scn_is >> _rule_set.BrigadeRE2;
		else if ( strcmp( ln, "CadreRE2" ) == 0 )
			scn_is >> _rule_set.CadreRE2;
		else if ( strcmp( ln, "DivGrpRE2" ) == 0 )
			scn_is >> _rule_set.DivGrpRE2;
		else if ( strcmp( ln, "DivisionRE2" ) == 0 )
			scn_is >> _rule_set.DivisionRE2;
		else if ( strcmp( ln, "CorpsRE2" ) == 0 )
			scn_is >> _rule_set.CorpsRE2;
		else if ( strcmp( ln, "ArmyRE2" ) == 0 )
			scn_is >> _rule_set.ArmyRE2;

		// ZOCs

		else if ( strcmp( ln, "AllowCustomZOCs" ) == 0 )
			scn_is >> _rule_set.AllowCustomZOCs;
		else if ( strcmp( ln, "BattalionZOC" ) == 0 )
			scn_is >> _rule_set.BattalionZOC >> _rule_set.BattalionU2ZOC;
		else if ( strcmp( ln, "RegimentZOC" ) == 0 )
			scn_is >> _rule_set.RegimentZOC >> _rule_set.RegimentU2ZOC;
		else if ( strcmp( ln, "BrigadeZOC" ) == 0 )
			scn_is >> _rule_set.BrigadeZOC >> _rule_set.BrigadeU2ZOC;
		else if ( strcmp( ln, "CadreZOC" ) == 0 )
			scn_is >> _rule_set.CadreZOC >> _rule_set.CadreU2ZOC;
		else if ( strcmp( ln, "DivGrpZOC" ) == 0 )
			scn_is >> _rule_set.DivGrpZOC >> _rule_set.DivGrpU2ZOC;
		else if ( strcmp( ln, "DivisionZOC" ) == 0 )
			scn_is >> _rule_set.DivisionZOC >> _rule_set.DivisionU2ZOC;
		else if ( strcmp( ln, "CorpsZOC" ) == 0 )
			scn_is >> _rule_set.CorpsZOC >> _rule_set.CorpsU2ZOC;

		// movement

		else if ( strcmp( ln, "DisableMPCalc" ) == 0 )
			scn_is >> _rule_set.DisableMPCalc;
		else if ( strcmp( ln, "OnlyClearAdmin" ) == 0 )
			scn_is >> _rule_set.OnlyClearAdmin;
		else if ( strcmp( ln, "ToTrain1MP" ) == 0 )
			scn_is >> _rule_set.ToTrain1MP;
		else if ( strcmp( ln, "ReducedZOCs" ) == 0 )
			scn_is >> _rule_set.ReducedZOCs;
		else if ( strcmp( ln, "SovietReducedZOCs" ) == 0 )
			scn_is >> _rule_set.SovietReducedZOCs;
		else if ( strcmp( ln, "MP6Swamp" ) == 0 )
			scn_is >> _rule_set.MP6Swamp;
		else if ( strcmp( ln, "NonPhasMove" ) == 0 )
		{
			scn_is >> _rule_set.NonPhasMove;
			if ( _rule_set.ReducedZOCs == TRUE )
				wxMessageBox( wxT(	"All XXs of both players have reduced ZOCs,\n"
									"see 'Options->Rules->Movement'" ), Application::NAME, wxOK | wxICON_INFORMATION );
		}
		else if ( strcmp( ln, "NoGaugeChk" ) == 0 )
			scn_is >> _rule_set.NoGaugeChk;
		else if ( strcmp( ln, "AllowUxAdminMove" ) == 0 )
			scn_is >> _rule_set.AllowUxAdminMove;
		else if ( strcmp( ln, "FortLevelMP" ) == 0 )
			scn_is >> _rule_set.FortLevelMP;
		else if ( strcmp( ln, "PermAFMP" ) == 0 )
			scn_is >> _rule_set.PermAFMP;
		else if ( strcmp( ln, "TempAFMP" ) == 0 )
			scn_is >> _rule_set.TempAFMP;
		else if ( strcmp( ln, "ZOCLeaveCost" ) == 0 )
			scn_is >> _rule_set.ZOCLeaveCost;
		else if ( strcmp( ln, "ZOC2ZOCCost" ) == 0 )
			scn_is >> _rule_set.ZOC2ZOCCost;
		else if ( strcmp( ln, "RZOCLeaveCost" ) == 0 )
			scn_is >> _rule_set.RZOCLeaveCost;
		else if ( strcmp( ln, "RZOC2ZOCCost" ) == 0 )
			scn_is >> _rule_set.RZOC2ZOCCost;
		else if ( strcmp( ln, "GermanCMZOCRule" ) == 0 )
			scn_is >> _rule_set.GermanCMZOCRule;
		else if ( strcmp( ln, "ZOCEnterMot" ) == 0 )
			scn_is >> _rule_set.ZOCEnterMot;
		else if ( strcmp( ln, "ZOCEnterNormal" ) == 0 )
			scn_is >> _rule_set.ZOCEnterNormal;
		else if ( strcmp( ln, "NoAlliedZOCAcrossBorders" ) == 0 )
			scn_is >> _rule_set.NoAlliedZOCAcrossBorders;
		else if ( strcmp( ln, "GermanRegaugeInExploitation" ) == 0 )
			scn_is >> _rule_set.GermanRegaugeInExploitation;
		else if ( strcmp( ln, "SovietReactionPhase" ) == 0 )
			scn_is >> _rule_set.SovietReactionPhase;
		else if ( strcmp( ln, "SovietExploitLimitation" ) == 0 )
			scn_is >> _rule_set.SovietExploitLimitation;
		else if ( strcmp( ln, "RoadsCanBeBroken" ) == 0 )
			scn_is >> _rule_set.RoadsCanBeBroken;
		else if ( strcmp( ln, "PavedRoads" ) == 0 )
			scn_is >> _rule_set.PavedRoads;
		else if ( strcmp( ln, "TEM40Overruns" ) == 0 )
			scn_is >> _rule_set.TEM40Overruns;

		// combat

		else if ( strcmp( ln, "AlliedCombatSide" ) == 0 )
			scn_is >> _rule_set.AlliedCombatSide;
		else if ( strcmp( ln, "ExtremeCrt" ) == 0 )
			scn_is >> _rule_set.ExtremeCrt;
		else if ( strcmp( ln, "AutoDR" ) == 0 )
			scn_is >> _rule_set.AutoDR;
		else if ( strcmp( ln, "AutoDH" ) == 0 )
			scn_is >> _rule_set.AutoDH;
		else if ( strcmp( ln, "AutoDE" ) == 0 )
			scn_is >> _rule_set.AutoDE;
		else if ( strcmp( ln, "OnlyIsolU1Halved" ) == 0 )
			scn_is >> _rule_set.OnlyIsolU1Halved;
		else if ( strcmp( ln, "WestCityMod" ) == 0 )
			scn_is >> _rule_set.WestCityMod;
		else if ( strcmp( ln, "NewSwamp" ) == 0 )
			scn_is >> _rule_set.NewSwamp;
		else if ( strcmp( ln, "AttackSup" ) == 0 )
			scn_is >> _rule_set.AttackSup;
		else if ( strcmp( ln, "DASMode" ) == 0 )
			scn_is >> _rule_set.DASMode;
		else if ( strcmp( ln, "GSHalved" ) == 0 )
			scn_is >> _rule_set.GSHalved;
		else if ( strcmp( ln, "AxisHalfAT" ) == 0 )
			scn_is >> _rule_set.AxisHalfAT;
		else if ( strcmp( ln, "WeatherDieMod" ) == 0 )
			scn_is >> _rule_set.WeatherDieMod;
		else if ( strcmp( ln, "SnowReducedAEC" ) == 0 )
			scn_is >> _rule_set.SnowReducedAEC;
		// TODO: fix the spelling here, wherever it is created
		else if ( strcmp( ln, "FrozingAllowed" ) == 0 )
			scn_is >> _rule_set.FreezingAllowed;
		else if ( strcmp( ln, "IgnoreFrozenLakeHexsides" ) == 0 )
			scn_is >> _rule_set.IgnoreFrozenLakeHexsides;
		else if ( strcmp( ln, "OneTenthAEC" ) == 0 )
			scn_is >> _rule_set.OneTenthAEC;
		else if ( strcmp( ln, "EngProportion" ) == 0 )
			scn_is >> _rule_set.EngProportion;
		else if ( strcmp( ln, "TGWModernArtTactics" ) == 0 )
			scn_is >> _rule_set.WW1ModernArtTactics;
		else if ( strcmp( ln, "BorderRiversIgnored" ) == 0 )
			scn_is >> _rule_set.BorderRiversIgnored;
		else if ( strcmp( ln, "FortifiedAreaLevel" ) == 0 )
			scn_is >> _rule_set.FortifiedAreaLevel;

		// air

		else if ( strcmp( ln, "OnDemandAirMissions" ) == 0 )
			scn_is >> _rule_set.OnDemandAirMissions;
		else if ( strcmp( ln, "NewAFCapacitySystem" ) == 0 )
			scn_is >> _rule_set.NewAFCapacitySystem;
		else if ( strcmp( ln, "EndAirCombatAtFirstResult" ) == 0 )
			scn_is >> _rule_set.EndAirCombatAtFirstResult;
		else if ( strcmp( ln, "HFIsF" ) == 0 )
			scn_is >> _rule_set.HFIsF;
		else if ( strcmp( ln, "AbortedPlanesToPool" ) == 0 )
			scn_is >> _rule_set.AbortedPlanesToPool;
		else if ( strcmp( ln, "NewAAMods" ) == 0 )
			scn_is >> _rule_set.NewAAMods;
		else if ( strcmp( ln, "StageRange" ) == 0 )
			scn_is >> _rule_set.StageRange;
		else if ( strcmp( ln, "TmpAFCap" ) == 0 )
			scn_is >> _rule_set.TmpAFCap;
		else if ( strcmp( ln, "TmpAFBadWeatherCap" ) == 0 )
			scn_is >> _rule_set.TmpAFBadWeatherCap;
		else if ( strcmp( ln, "JettisonEsc" ) == 0 )
			scn_is >> _rule_set.JettisonEsc;
		else if ( strcmp( ln, "BombingFBothReduced" ) == 0 )
			scn_is >> _rule_set.BombingFBothReduced;
		else if ( strcmp( ln, "AlliedFullCityAA" ) == 0 )
			scn_is >> _rule_set.AlliedFullCityAA;
		else if ( strcmp( ln, "AlliedPartialCityAA" ) == 0 )
			scn_is >> _rule_set.AlliedPartialCityAA;
		else if ( strcmp( ln, "AlliedDotCityAA" ) == 0 )
			scn_is >> _rule_set.AlliedDotCityAA;
		else if ( strcmp( ln, "AlliedReferenceCityAA" ) == 0 )
			scn_is >> _rule_set.AlliedReferenceCityAA;
		else if ( strcmp( ln, "AlliedUnImprovedFortressAA" ) == 0 )
			scn_is >> _rule_set.AlliedUnImprovedFortressAA;
		else if ( strcmp( ln, "AlliedImprovedFortressAA" ) == 0 )
			scn_is >> _rule_set.AlliedImprovedFortressAA;
		else if ( strcmp( ln, "AlliedAirfieldAA" ) == 0 )
			scn_is >> _rule_set.AlliedAirfieldAA;
		else if ( strcmp( ln, "AxisFullCityAA" ) == 0 )
			scn_is >> _rule_set.AxisFullCityAA;
		else if ( strcmp( ln, "AxisPartialCityAA" ) == 0 )
			scn_is >> _rule_set.AxisPartialCityAA;
		else if ( strcmp( ln, "AxisDotCityAA" ) == 0 )
			scn_is >> _rule_set.AxisDotCityAA;
		else if ( strcmp( ln, "AxisReferenceCityAA" ) == 0 )
			scn_is >> _rule_set.AxisReferenceCityAA;
		else if ( strcmp( ln, "AxisUnImprovedFortressAA" ) == 0 )
			scn_is >> _rule_set.AxisUnImprovedFortressAA;
		else if ( strcmp( ln, "AxisImprovedFortressAA" ) == 0 )
			scn_is >> _rule_set.AxisImprovedFortressAA;
		else if ( strcmp( ln, "AxisAirfieldAA" ) == 0 )
			scn_is >> _rule_set.AxisAirfieldAA;

		// naval

		else if ( strcmp( ln, "OldNavalSystem" ) == 0 )
			scn_is >> _rule_set.OldNavalSystem;
		else if ( strcmp( ln, "SeparateNavalPhases" ) == 0 )
			scn_is >> _rule_set.SeparateNavalPhases;
		else if ( strcmp( ln, "EnableCDCalc" ) == 0 )
			scn_is >> _rule_set.EnableCDCalc;

		// stacking

		else if ( strcmp( ln, "StackNDivType" ) == 0 )
			scn_is >> _rule_set.StackNDivType;
		else if ( strcmp( ln, "StackDivs" ) == 0 )
			scn_is >> _rule_set.StackDivs;
		else if ( strcmp( ln, "StackNDivs" ) == 0 )
			scn_is >> _rule_set.StackNDivs;
		else if ( strcmp( ln, "StackArt" ) == 0 )
			scn_is >> _rule_set.StackArt;
		else if ( strcmp( ln, "StackMDivs" ) == 0 )
			scn_is >> _rule_set.StackMDivs;
		else if ( strcmp( ln, "StackMNDivs" ) == 0 )
			scn_is >> _rule_set.StackMNDivs;
		else if ( strcmp( ln, "StackMArt" ) == 0 )
			scn_is >> _rule_set.StackMArt;
		else if ( strcmp( ln, "StackADivs" ) == 0 )
			scn_is >> _rule_set.StackADivs;
		else if ( strcmp( ln, "StackANDivs" ) == 0 )
			scn_is >> _rule_set.StackANDivs;
		else if ( strcmp( ln, "StackAArt" ) == 0 )
			scn_is >> _rule_set.StackAArt;
		else if ( strcmp( ln, "StackTGWAllowOneFort" ) == 0 )
			scn_is >> _rule_set.StackWW1AllowOneFort;
	}

	// pre-populate these names with defaults
	//	(in case the CRT and TEC config settings are empty or bogus)
	set_CRT_name( Application::CRT_FILENAME.c_str( ) );
	set_TEC_name( Application::TEC_FILENAME.c_str( ) );

	// load config
	while ( true )
	{
		scn_is >> ln;
		if ( strcmp( ln, "Config>" ) == 0 )
			continue;
		else if ( strcmp( ln, "<Config" ) == 0 )
			break;

		else if ( strcmp( ln, "CRT" ) == 0 )
			scn_is >> crt_filename_;
		else if ( strcmp( ln, "TEC" ) == 0 )
			scn_is >> tec_filename_;
		else if ( strcmp( ln, "CMV" ) == 0 )
			scn_is >> _show_combat_markers;
		else if ( strcmp( ln, "RailRE" ) == 0 )
			scn_is >> turn_REs_rail_used_;
		// INSERT new Config parameters here...

		// Combat Marker PassWord -- must be the final Config parameter
		else if ( strcmp( ln, "CMPW" ) == 0 )
		{
			scn_is >> marker_password_;
			strcpy( ln, marker_password_ );
			if ( strcmp( marker_password_, "<Config" ) == 0 )
			{
				strcpy( marker_password_, "" );  // CMPW was empty
				break;
			}
		}
	}

	// TODO: work this into the above logic, i.e. descramble it when you parse it
	// de-scramble encrypted password
	strcpy( marker_password_, descramble_passwd( marker_password_ ) );

	// .scn filename for path checking to custom (if any) CRT and TEC
	bool clean_CRT_load = initialize_CRT( scn_filename ); // from tabular subfolder

	if ( ! clean_CRT_load && strcmp( Application::CRT_FILENAME.c_str( ), crt_filename_ ) != 0 )
	{
		set_CRT_name( Application::CRT_FILENAME.c_str( ) );
		clean_CRT_load = initialize_CRT( wxStandardPaths::Get( ).GetExecutablePath( ).c_str( ) );
	}

	if ( ! clean_CRT_load )
	{
		wxCommandEvent dummyEvent;
		parent_->CmChgcrt( dummyEvent );

		if ( CRTResult.empty( ) )
		{
			// TODO: release memory and return to unloaded state
			wxMessageBox( _("Loading aborted"), Application::NAME );
			return false;
		}
	}

	bool clean_TEC_load = initialize_TEC( scn_filename ); // tabular

	if ( ! clean_TEC_load && strcmp( Application::TEC_FILENAME.c_str( ), tec_filename_ ) != 0 )
	{
		set_TEC_name( Application::TEC_FILENAME.c_str( ) );
		clean_TEC_load = initialize_TEC( wxStandardPaths::Get( ).GetExecutablePath( ).c_str( ) );
	}

	if ( ! clean_TEC_load )
	{
		// TODO: release memory and return to unloaded state
		wxMessageBox( _("Loading aborted"), Application::NAME );
		return false;
	}

	// load extensible data
	while ( true )
	{
		scn_is >> ln;
		if ( strcmp( ln, "Data>" ) == 0 )
			continue;
		// this turn losses
		// TODO: none of this is used for current or previous turn losses tracking
		else if ( strcmp( ln, "Losses>" ) == 0 )
			while ( true )
			{
				scn_is >> ln;
				if ( strcmp( ln, "<Losses" ) == 0 )
					break;
			}
		// previous turn losses
		// TODO: see below for how losses are currently accounted
		else if ( strcmp( ln, "OldLosses>" ) == 0 )
			while ( true )
			{
				scn_is >> ln;
				if ( strcmp( ln, "<OldLosses" ) == 0 )
					break;
			}
		else if ( strcmp( ln, "<Views>" ) == 0 )
			scn_is >> *views_;
		else if ( strcmp( ln, "<Data" ) == 0 )	// end of extensible data?
			break;
	}

	// load losses
	for ( int y = 0; y < 100; ++y ) // years 1900 through 1999 ??
		for ( int m = 0; m < 12; ++m )
			for ( int t = 0; t < 2; ++t )
			{
				scn_is >> unisol_losses_[y][m][t][SidePlayer::AXIS];
				scn_is >> isol_losses_[y][m][t][SidePlayer::AXIS];
				scn_is >> unisol_losses_[y][m][t][SidePlayer::ALLIED];
				scn_is >> isol_losses_[y][m][t][SidePlayer::ALLIED];
			}

	// calibrate the progress bar
	wxSize sz = GetClientSize( );
	int xpos = ( sz.GetWidth( ) / 2 ) - 50;
	int ypos = ( sz.GetHeight( ) / 2 ) - 50;
	hdc.SetPen( *wxBLACK_PEN );
	int linepos = -1;

	for ( int y = 0; y < new_y_scn_size ; ++y )
		for ( int x = 0; x < new_x_scn_size; ++x )
		{
			scn_is >> sit_[y][x];

			// rebuild the dynamic Sit data that are not saved
			updateHexCache( x, y );
			updateHitMarkers( x, y );

			// update progress bar
			if ( ( y / new_y_scn_size * 100 + xpos ) != linepos )
			{
				linepos = y / new_y_scn_size * 100 + xpos;
				hdc.DrawLine( linepos, ypos + 32, linepos, ypos + 48 );
				if ( linepos != 0 )
					hdc.DrawLine( linepos - 1, ypos + 32, linepos - 1, ypos + 48 );
			}
		}

	scn_is >> _repl_pool[SidePlayer::AXIS];
	scn_is >> _repl_pool[SidePlayer::ALLIED];
	scn_is >> _air_repl_pool[SidePlayer::AXIS];
	scn_is >> _air_repl_pool[SidePlayer::ALLIED];

	scn_is.close( );
#endif

	wxEndBusyCursor( );

	// map size works for both map and scenario loading
	//	(if scenario dimensions did not match map dimensions, would error out earlier)
	world_y_size_ = new_y_map_size;
	world_x_size_ = new_x_map_size;

	// save filename
#if defined HEXTOOLSMAP
	// TODO: already done in loadMap(..) above, correct?
	strcpy( map_filename_, map_filename );
#elif defined HEXTOOLSPLAY
	strcpy( scn_filename_, scn_filename );
#endif

	// same flags have .map meaning for HT-m and .scn meaning for HT-p
	filename_is_defined_ = true;
	file_is_dirty_ = false;

#if defined HEXTOOLSMAP
	StatusMsg( ".map file loaded ok" );
	parent_->SetTitle( wxT( Application::NAME ) + wxString(" ") + wxT( Application::VERSION ) + wxString(" - ") + map_filename );
#elif defined HEXTOOLSPLAY
	StatusMsg( ".scn file loaded ok" );
#endif

	// scroll to home if new map is smaller than current
	if ( old_y_map_size > new_y_map_size || old_x_map_size > new_x_map_size )
		setScrollbarRange( 0, 0 );
	else
		setScrollbarRange( ); // default ( -1, -1 )

	Refresh( );

	return true;
}

// TODO: make static if possible (refactor out class members -map and -play)
// used only by -map and -play loadFiles(..) and -play initializeScenario(..)
bool MapPane::load_map( const char* map_filename )
{
	// file stream check
	std::ifstream is( map_filename );
	if ( ! is )
	{
		std::string badstream = "Couldn't open mapfile " + std::string( map_filename ) + "!";
		wxMessageBox( badstream, Application::NAME );
		return false;
	}

	int ver;
	is >> ver;

// different checks for HT-m vice HT-p
#if defined HEXTOOLSMAP
	if ( ver < ht::MAP_VERSION )
	{
		std::ostringstream old_map_format;
		old_map_format << "NOTE: This map file uses old format." << endl;
		old_map_format << "It will be converted to new format when" << endl;
		old_map_format << "you save it. Please note that HexTools versions" << endl;
		old_map_format << "older than " + Application::VERSION + " will not be able to read\n" << endl;
		old_map_format << "new map files.";
		wxMessageBox( old_map_format.str( ), Application::NAME, wxICON_INFORMATION );
	}
	else if ( ver != ht::MAP_VERSION )
	{
		std::ostringstream bogus_map_format;
		bogus_map_format << "This map file uses unrecognized format!" << endl << endl;
		bogus_map_format << "Load it anyway???";
		if ( wxMessageBox( bogus_map_format.str( ), "Warning!", wxYES_NO ) != wxYES )
			return false;
	}
#elif defined HEXTOOLSPLAY
	if ( ver > ht::MAP_VERSION || ver < ht::OK_MAP_VERSION )
	{
		std::ostringstream bogus_map_format;
		bogus_map_format << "This map file uses unrecognized or obsolete format " << map_filename << " !" << endl;
		bogus_map_format << "Please use HexTools-Map to upgrade your map version before" << endl;
		bogus_map_format << "opening or creating a scenario with it." << endl;
		bogus_map_format << "Loading aborted.";
		wxMessageBox( bogus_map_format.str( ), Application::NAME );
		return false;
	}
#endif

	// all checks passed, now ready to load the data
	wxBeginBusyCursor( );

	int new_y_map_size;
	is >> new_y_map_size;

	int new_x_map_size;
	is >> new_x_map_size;

	char maphdr_line[260];
#if defined HEXTOOLSMAP
	// process map file header
	clear_all_rotated_mapsheets( );
	clear_all_right_offset_mapsheets( );

	// TODO: global data??
	//	HT-p not used globally, only in this function
	//  HT-m (NEED TO CHECK THIS)
	// map version number is acceptable, store it in a global
	_map_version = ver;

	if ( _map_version >= 8 )
	{
		while ( true )
		{
			is >> maphdr_line;
			if ( strcmp( maphdr_line, "Ext>" ) == 0 )
				continue;
			else if ( strcmp( maphdr_line, "<Ext" ) == 0 )
				break;
			else if ( strcmp( maphdr_line, "RotatedMapSheet" ) == 0 )
			{
				is >> maphdr_line;
				if ( _map_version < ht::OK_MAP_VERSION )
				{
					unsigned short mapid = atoi( maphdr_line ) & 0x00ff;
					if ( isalpha( maphdr_line[strlen( maphdr_line ) - 1] ) )
						mapid |= maphdr_line[strlen( maphdr_line ) - 1] << 8;

					strcpy( maphdr_line, Hex::convertMapIdEncoding( mapid ) );
				}

				set_rotated( maphdr_line );
			}
			else if ( strcmp( maphdr_line, "NormalRightMapSheet" ) == 0 )
			{
				is >> maphdr_line;
				set_right_offset( maphdr_line );
			}
		}
	}
#elif defined HEXTOOLSPLAY
	// just consume the entire map file header
	do
		is >> maphdr_line;
	while ( strcmp( maphdr_line, "<Ext" ) != 0 );
#endif

	int old_y_map_size = world_y_size_;
	int old_x_map_size = world_x_size_;

	release_map_storage( old_y_map_size, old_x_map_size );

	reserve_map_storage( new_y_map_size, new_x_map_size );

	for ( int y = 0; y < new_y_map_size; ++y )
		for ( int x = 0; x < new_x_map_size; ++x )
			is >> hex_[y][x];

	wxEndBusyCursor( );

	// store map filename into the global variable
	strcpy( map_filename_, map_filename );

	return true;
}

#if defined HEXTOOLSMAP

// PAPI (Frame)
void MapPane::save_map( const char* map_filename )
{
	// TODO: vet logic with MapPane::saveScenario( .. )

	std::ofstream os;

	if ( map_filename != nullptr )
	{
		os.open( map_filename );
		filename_is_defined_ = false;  // force title bar caption update later
	}
	else
	{
		if ( filename_is_defined_ )
			os.open( map_filename_ );
		else
		{
			wxMessageBox( wxT("No filename defined!") );
			return;
		}
	}

	char msg[50];
	if ( ! os )
	{
		sprintf( msg, "Couldn't open file \"%s\"!", map_filename );
		wxMessageBox( wxT(msg) );
		return;
	}

	wxBeginBusyCursor( );

	// write data to file
	os << ht::MAP_VERSION << ' ';
	os << world_y_size_ << ' ';
	os << world_x_size_ << endl;

	// extended stuff: things can be added here without breaking file compatibility
	os << "Ext>" << endl;
	inventory_mapsheets( );
	for ( int i = 0; i < mapsheet_count_; ++i )
		if ( mapsheet_normal_right_offset( mapsheet_name( i ) ) )
			os << "NormalRightMapSheet " << mapsheet_name( i ) << endl;
	for ( int i = 0; i < mapsheet_count_; ++i )
		if ( mapsheet_rotated( mapsheet_name( i ) ) )
			os << "RotatedMapSheet " << mapsheet_name( i ) << endl;
	os << "<Ext" << endl;

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			os << hex_[y][x];

	os.close( );

	file_is_dirty_ = false;
	if ( ! filename_is_defined_ )
	{
		strcpy( map_filename_, map_filename );
		filename_is_defined_ = true;
		parent_->SetTitle( wxT( Application::NAME ) + wxString(" ") + wxT( Application::VERSION ) + wxString(" - ") + map_filename_ );
	}

	wxEndBusyCursor( );
}

// PAPI (Frame)
// export map to bitmap file
void MapPane::export_map( const char* image_filename )
{
	wxString ch[] = { "200% (large image)", "100%", "50% (small image)" };

	int z = wxGetSingleChoiceIndex( _("Select zoom level"), _("Export image"), 3, ch, this );
	float zoomScale = 1.0;
	switch ( z )
	{
		case 0:
			zoomScale = 2.0;
			break;
		case 1:
			zoomScale = 1.0;
			break;
		case 2:
			zoomScale = 0.5;
			break;
		default:
			return;
	}

	// TODO: is int truncation the correct operation?
	wxBitmap bmp( (int) ( world_x_size_ * 32 * zoomScale ), (int)( world_y_size_ * 32 * zoomScale ) );

	wxMemoryDC hdcMem;
	hdcMem.SelectObject( bmp );
	hdcMem.SetPen( *wxLIGHT_GREY_PEN );
	hdcMem.SetBrush( *wxLIGHT_GREY_BRUSH );
	// TODO: is int truncation the correct operation?
	hdcMem.DrawRectangle( 0, 0, (int) ( world_x_size_ * 32 * zoomScale ), (int)( world_y_size_ * 32 * zoomScale ) );

	// TODO:  I believe the zoom should be implemented into this call, not the bmp c-tor (above)
	//doPaintToDC( hdcMem, 0, 0, iXWorldSize - 1, iYWorldSize - 1 );

	bmp.SaveFile( image_filename, wxBITMAP_TYPE_BMP );
}

// used only by ctor
void MapPane::initialize_map( )
{
	// remove old cities (dynamically allocated!)
	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			if ( hex_[y][x].city_ != nullptr )
			{
				// TODO: naked delete, and does this null multiple cities per hex?
				delete hex_[y][x].city_;
				hex_[y][x].city_ = nullptr;
			}

	file_is_dirty_ = false;
	filename_is_defined_ = false;
	Refresh( );
}

#endif

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
// PAPI (Frame)
// new .scn given .map filename
bool MapPane::initialize_scenario( char* map_filename )
{
	std::ifstream is( map_filename );
	if ( ! is )
	{
		wxMessageBox( wxT("Couldn't open mapfile"), Application::NAME );
		return false;
	}

	// map version number; not needed here, just consume it
	int dummy;
	is >> dummy;

	// map dimensions
	int new_y_map_size;
	is >> new_y_map_size;

	int new_x_map_size;
	is >> new_x_map_size;

	is.close( );

	if ( ! load_map( map_filename ) )
		return false;

	contents_pane_->deselectAll( );
	contents_pane_->clear( );

	ClearAllViews( );

	// inspect the map's dimensions, for sizing scrollbars
	// (used for scrollbar re-initialization when a different .scn is later loaded)
	int old_y_map_size = world_y_size_;
	int old_x_map_size = world_x_size_;

	// release old memory before reservation for new scenario dimensions
	release_sit_storage( old_y_map_size, old_x_map_size );

	// reserve new situational memory (and initialize situation)
	reserve_sit_storage( new_y_map_size, new_x_map_size );

	// map and scenario dimensions stored
	world_y_size_ = new_y_map_size;
	world_x_size_ = new_x_map_size;

	// TODO: already done in loadMap(..) earlier, correct?
	// >>map<< filename formally defined now:
	//strcpy( achMapFileName, map_filename );

	// note: this is a HT-p only function, so here
	//	fileNameDefined means the >>scenario<< filename
	filename_is_defined_ = false;

	// TODO: is dirty flag true or false at this point?

	StatusMsg( "scn initialized ok" );

	if ( old_x_map_size > new_x_map_size || old_y_map_size > new_y_map_size )
		setScrollbarRange( 0, 0 );
	else
		setScrollbarRange( ); // default ( -1, -1 )

	Refresh( );

	return true;
}

// PAPI (Frame)
// insert .scn file to current game at given location
// NOTE:  if no units in new file -> don't remove old units
// TODO: known defect:  if units in new and old file -> old units disappear (i.e. memory leaks)
bool MapPane::insert_scenario( const char* filename_to_insert, int xofs, int yofs )
{
	std::ifstream is( filename_to_insert );
	if ( ! is )
	{
		char insertmsg[5000];
		sprintf( insertmsg, "Couldn't open file \"%s\"!", filename_to_insert );
		wxMessageBox( wxS( insertmsg ), Application::NAME );
		return false;
	}

	int ver;
	is >> ver;  // check if this is valid .scn file
	if ( ver != ht::SCN_VERSION )
	{
		const wxChar* scnmessage = wxT(	"This scenario file uses unrecognized or obsolete format!\n\n"
										"Try to load anyway (not recommended)?" );
		if ( wxMessageBox( scnmessage, wxT("Warning!"), wxYES_NO ) != wxYES )
			return false;
	}

	int xsize, ysize;
	is >> ysize;
	is >> xsize;

	// TODO: hideUnitMover( ) impl is currently empty
	//hideUnitMover( );

	contents_pane_->deselectAll( );
	contents_pane_->clear( );

	// discard through end of "Data" section
	char dummy_msg[5000];
	is >> dummy_msg;
	while ( strcmp( dummy_msg, "<Data" ) != 0 )
		is >> dummy_msg;

	// old Data discard loop
	//while ( true )
	//{
	//	is >> dummy_msg;
	//	if ( strcmp( dummy_msg, "<Data" ) == 0 )
	//		break;
	//}

	// discard losses, 100 years of 12 months of 2 turns per month of 4 integral side unisol and isol losses
	int i;
	//int j[4];
	for ( int y = 0; y < 100; ++y )
		for ( int m = 0; m < 12; ++m )
			for ( int t = 0; t < 2; ++t )
				//is >> j;
	// old per-integer discard
			{
				is >> i;  //Losses[y][m][t][AXIS];
				is >> i;  //IsolLosses[y][m][t][AXIS];
				is >> i;  //Losses[y][m][t][ALLIED];
				is >> i;  //IsolLosses[y][m][t][ALLIED];
			}

	// insert scenario data
	// note: the upper-left insertion coordinate comes from user input (xofs, yofs)
	//	likewise, the lower-right insertion coordinate comes from the source .scn
	//	file itself (xsize, ysize)
	// note:	xofs, yofs, xsize, and ysize are all from the source .scn
	//			x and y are relative to the target .scn file
	HexContents dummy_sit;
	for ( int y = yofs; y < yofs + ysize; ++y )
		for ( int x = xofs; x < xofs + xsize; ++x )
		{
			if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
			{
				// this happens when (x,y) is not within target scenario dimensions
				// and is due to either:
				//	1/ xofs and/or yofs are negative, and loop has not
				//		incremented them into positive values yet; OR
				//	2/ xofs + xsize and/or yofs + ysize are larger than
				//		the dimensions of the target scenario
				// in either case, just ignore the source situational data:
				is >> dummy_sit;
				dummy_sit.clear( );
			}
			else
			{
				// discard any existing situational data at this location in the target .scn
				sit_[y][x].clear( );

				// copy situational data at this location from source to target
				is >> sit_[y][x];

				// synchronize per-stack strength cache to new situational data
				updateHexCache( x, y );

				// synchronize hit markers at this location to new situational data
				updateHitMarkers( x, y );
			}
		}

	is.close( );

	wxMessageBox( wxT("Note: replacement pools are not merged"), Application::NAME );

	Refresh( );

	return true;
}

static char* mk_bkp_filename( const char* fname ) // used only by saveScenario(..)
{
	if ( ! fname )
		return nullptr;

	static char s[300];

	// TODO: harden this string manipulation
	// assumption:  given fname has a 3-length file extension
	strcpy( s, fname );
	char* cp = &( s[strlen( s ) - 4] );  // back position by dot and 3-length extension
	strcpy( cp, ".bak" );	// copy new .bak extension onto existing extension

	return s;
}

static char* scramble_passwd( char* pw ) // used only by saveScenario(..)
{
	static char bf[100];

	unsigned int i;

	for ( i = 0; i < strlen( pw ); ++i )
	{
		bf[i * 2] = 	static_cast<char>( ( pw[i] & 0x0f ) + '0' + i );
		bf[i * 2 + 1] =	static_cast<char>( ( ( pw[i] & 0xf0 ) >> 4 ) + 'A' );
	}

	bf[i * 2] = '\0';

	return bf;
}

// PAPI (Frame)
// save .scn file, return true if ok
bool MapPane::save_scenario( const char* filename_to_save )
{
	// new name given, discard old and change caption in end of this func

	// if called from Frame::saveScenario( ) via Frame::OnSaveAs( ) and	a file dialog
	if ( filename_to_save != nullptr )
		filename_is_defined_ = false; // no need to create a .bak when doing a save-as

	// create backup file '<filename>.bak' (before saving <filename>.scn)
	char savemsg[260];
	if ( filename_is_defined_ && _backup_on_save )
	{
		char* f = scn_filename_;
		char* bkpf = mk_bkp_filename( f );
#if defined __WXMSW__
		if ( CopyFile( f, bkpf, FALSE ) == FALSE )
		{
			sprintf( savemsg, "Failed to create backup file, error %d", static_cast<int>( GetLastError( ) ) );
			wxMessageBox( wxS( savemsg ), Application::NAME );
		}
#else
		sprintf( savemsg, "cp %s %s", f, bkpf );
		if ( system( savemsg ) != 0 )
		{
			sprintf( savemsg, "Failed to create backup file!" );
			wxMessageBox( wxS(savemsg), Application::NAME );
		}
#endif
	}

	std::ofstream os;
	// if called from Frame::saveScenario( ) via Frame::OnSaveAs( ) and	a file dialog
	if ( filename_to_save != nullptr )
		os.open( filename_to_save ); // try to open new file for output (first save, or save-as)
	else
	{
		if ( filename_is_defined_ )
			os.open( scn_filename_ );  // try to open existing file for output
		else
		{
			// not a save-as, and no file name previously defined
			wxMessageBox( wxT("Somehow no file name was defined!"), Application::NAME );
			return false;
		}
	}

	if ( ! os ) // could not open file for output
	{
		sprintf( savemsg, "Couldn't open file \"%s\"!\n\nSave aborted.", filename_to_save );
		wxMessageBox( wxS( savemsg ), Application::NAME );
		return false;
	}

	contents_pane_->deselectAll( );  // save selected units too!

	wxBeginBusyCursor( );

	////// header line //////

	os << ht::SCN_VERSION << ' ';
	os << world_y_size_ << ' ';
	os << world_x_size_ << ' ';
	os << basename( map_filename_ ) << ' ';
	os << _year << ' ';
	os << _month << ' ';
	os << _turn << ' ';
	os << _phasing_player << ' ';

	if ( _current_phase == Phase::SETUP_PHASE )
		os << _previous_phase << endl;
	else
		os << _current_phase << endl;

	///// store weather /////

	// one line per zone
	for ( int i = 0; i < WeatherZone::COUNT; ++i )
		os << _weather_condition[i] << ' ' << _rivers_lakes_frozen[i] << ' ' << _wturns[i] << endl;

	/* TODO: seems like the below should be supported for major versions
	 * earlier than 09 */
#if 0
	 os << Weather[0] <<' '<< Weather[1] <<' '<< Weather[2]<<' '<<Weather[3]<<' ';
	 os << Weather[4] << ' '<< Weather[5] <<' '<< Weather[6] << endl;
	 os << Frozen[0] <<' '<< Frozen[1] <<' '<< Frozen[2]<<' '<<Frozen[3]<<' ';
	 os << Frozen[4] << ' '<< Frozen[5] <<' '<< Frozen[6] << endl;
	 os << wturns[0] <<' '<< wturns[1] <<' '<< wturns[2]<<' '<<wturns[3]<<' ';
	 os << wturns[4] << ' '<< wturns[5] <<' '<< wturns[6] << endl;
#endif

	////// store rules //////
	os << "Rules>" << endl;

	// game series
#if 0
	os << "RulesVariant " << _rule_set.rules_variant_ << endl;
#endif
	os << "GameEra " << _rule_set.Era << endl;
	os << "TGWMPs " << _rule_set.WW1MPs << endl;
	os << "TGWPhases " << _rule_set.WW1Phases << endl;
	os << "TGWAirBaseCapacity " << _rule_set.WW1AirBaseCapacity << endl;
	os << "TGWExploitMPs " << _rule_set.WW1ExploitMPs << endl;
	os << "RailsObstructed " << _rule_set.RailsObstructed << endl;

	// general
	os << "FirstPlayer " << _rule_set.FirstPlayer << endl;
	os << "Turns " << _turns_per_month << endl;
#if 0
	os << "UnitTypeFlags " << _rule_set.UnitTypeFlags << endl; // TODO: only 512 or 1024?
#endif
	// REs
	os << "AllowCustomREs " << _rule_set.AllowCustomREs << endl;
	os << "HideAxPartisans " << _rule_set.HideAxPartisans << endl;
	os << "HideAlPartisans " << _rule_set.HideAlPartisans << endl;
	os << "HQRE2 " << _rule_set.HQRE2 << endl;
	os << "CompanyRE2 " << _rule_set.CompanyRE2 << endl;
	os << "BattalionRE2 " << _rule_set.BattalionRE2 << endl;
	os << "RegimentRE2 " << _rule_set.RegimentRE2 << endl;
	os << "BrigadeRE2 " << _rule_set.BrigadeRE2 << endl;
	os << "CadreRE2 " << _rule_set.CadreRE2 << endl;
	os << "DivGrpRE2 " << _rule_set.DivGrpRE2 << endl;
	os << "DivisionRE2 " << _rule_set.DivisionRE2 << endl;
	os << "CorpsRE2 " << _rule_set.CorpsRE2 << endl;
	os << "ArmyRE2 " << _rule_set.ArmyRE2 << endl;

	// ZOCs
	os << "AllowCustomZOCs " << _rule_set.AllowCustomZOCs << endl;
	os << "BattalionZOC " << _rule_set.BattalionZOC << " " << _rule_set.BattalionU2ZOC << endl;
	os << "RegimentZOC " << _rule_set.RegimentZOC << " " << _rule_set.RegimentU2ZOC << endl;
	os << "BrigadeZOC " << _rule_set.BrigadeZOC << " " << _rule_set.BrigadeU2ZOC << endl;
	os << "CadreZOC " << _rule_set.CadreZOC << " " << _rule_set.CadreU2ZOC << endl;
	os << "DivGrpZOC " << _rule_set.DivGrpZOC << " " << _rule_set.DivGrpU2ZOC << endl;
	os << "DivisionZOC " << _rule_set.DivisionZOC << " " << _rule_set.DivisionU2ZOC << endl;
	os << "CorpsZOC " << _rule_set.CorpsZOC << " " << _rule_set.CorpsU2ZOC << endl;

	// movement
	os << "DisableMPCalc " << _rule_set.DisableMPCalc << endl;
	os << "OnlyClearAdmin " << _rule_set.OnlyClearAdmin << endl;
	os << "ToTrain1MP " << _rule_set.ToTrain1MP << endl;
	os << "ReducedZOCs " << _rule_set.ReducedZOCs << endl;
	os << "SovietReducedZOCs " << _rule_set.SovietReducedZOCs << endl;
	os << "NonPhasMove " << _rule_set.NonPhasMove << endl;
	os << "NoGaugeChk " << _rule_set.NoGaugeChk << endl;
	os << "MP6Swamp " << _rule_set.MP6Swamp << endl;
	os << "AllowUxAdminMove " << _rule_set.AllowUxAdminMove << endl;
	os << "FortLevelMP " << _rule_set.FortLevelMP << endl;
	os << "PermAFMP " << _rule_set.PermAFMP << endl;
	os << "TempAFMP " << _rule_set.TempAFMP << endl;
	os << "ZOCLeaveCost " << _rule_set.ZOCLeaveCost << endl;
	os << "ZOC2ZOCCost " << _rule_set.ZOC2ZOCCost << endl;
	os << "RZOCLeaveCost " << _rule_set.RZOCLeaveCost << endl;
	os << "RZOC2ZOCCost " << _rule_set.RZOC2ZOCCost << endl;
	os << "GermanCMZOCRule " << _rule_set.GermanCMZOCRule << endl;
	os << "ZOCEnterMot " << _rule_set.ZOCEnterMot << endl;
	os << "ZOCEnterNormal " << _rule_set.ZOCEnterNormal << endl;
	os << "NoAlliedZOCAcrossBorders " << _rule_set.NoAlliedZOCAcrossBorders << endl;
	os << "GermanRegaugeInExploitation " << _rule_set.GermanRegaugeInExploitation << endl;
	os << "SovietReactionPhase " << _rule_set.SovietReactionPhase << endl;
	os << "SovietExploitLimitation " << _rule_set.SovietExploitLimitation << endl;
	os << "RoadsCanBeBroken " << _rule_set.RoadsCanBeBroken << endl;
	os << "PavedRoads " << _rule_set.PavedRoads << endl;
	os << "TEM40Overruns " << _rule_set.TEM40Overruns << endl;

	// combat
	os << "AlliedCombatSide " << _rule_set.AlliedCombatSide << endl;
	os << "ExtremeCrt " << _rule_set.ExtremeCrt << endl;
	os << "AutoDR " << _rule_set.AutoDR << endl;
	os << "AutoDH " << _rule_set.AutoDH << endl;
	os << "AutoDE " << _rule_set.AutoDE << endl;
	os << "OnlyIsolU1Halved " << _rule_set.OnlyIsolU1Halved << endl;
	os << "WestCityMod " << _rule_set.WestCityMod << endl;
	os << "NewSwamp " << _rule_set.NewSwamp << endl;
	os << "AttackSup " << _rule_set.AttackSup << endl;
	os << "DASMode " << _rule_set.DASMode << endl;
	os << "GSHalved " << _rule_set.GSHalved << endl;
	os << "AxisHalfAT " << _rule_set.AxisHalfAT << endl;
	os << "WeatherDieMod " << _rule_set.WeatherDieMod << endl;
	os << "SnowReducedAEC " << _rule_set.SnowReducedAEC << endl;
	os << "FrozingAllowed " << _rule_set.FreezingAllowed << endl;
	os << "IgnoreFrozenLakeHexsides " << _rule_set.IgnoreFrozenLakeHexsides << endl;
	os << "OneTenthAEC " << _rule_set.OneTenthAEC << endl;
	os << "EngProportion " << _rule_set.EngProportion << endl;
	os << "TGWModernArtTactics " << _rule_set.WW1ModernArtTactics << endl;
	os << "BorderRiversIgnored " << _rule_set.BorderRiversIgnored << endl;
	os << "FortifiedAreaLevel " << _rule_set.FortifiedAreaLevel << endl;

	// air
	os << "OnDemandAirMissions " << _rule_set.OnDemandAirMissions << endl;
	os << "NewAFCapacitySystem " << _rule_set.NewAFCapacitySystem << endl;
	os << "EndAirCombatAtFirstResult " << _rule_set.EndAirCombatAtFirstResult << endl;
	os << "HFIsF " << _rule_set.HFIsF << endl;
	os << "AbortedPlanesToPool " << _rule_set.AbortedPlanesToPool << endl;
	os << "NewAAMods " << _rule_set.NewAAMods << endl;
	os << "StageRange " << _rule_set.StageRange << endl;
	os << "TmpAFCap " << _rule_set.TmpAFCap << endl;
	os << "TmpAFBadWeatherCap " << _rule_set.TmpAFBadWeatherCap << endl;
	os << "JettisonEsc " << _rule_set.JettisonEsc << endl;
	os << "BombingFBothReduced " << _rule_set.BombingFBothReduced << endl;
	os << "AlliedFullCityAA " << _rule_set.AlliedFullCityAA << endl;
	os << "AlliedPartialCityAA " << _rule_set.AlliedPartialCityAA << endl;
	os << "AlliedDotCityAA " << _rule_set.AlliedDotCityAA << endl;
	os << "AlliedReferenceCityAA " << _rule_set.AlliedReferenceCityAA << endl;
	os << "AlliedUnImprovedFortressAA " << _rule_set.AlliedUnImprovedFortressAA << endl;
	os << "AlliedImprovedFortressAA " << _rule_set.AlliedImprovedFortressAA << endl;
	os << "AlliedAirfieldAA " << _rule_set.AlliedAirfieldAA << endl;
	os << "AxisFullCityAA " << _rule_set.AxisFullCityAA << endl;
	os << "AxisPartialCityAA " << _rule_set.AxisPartialCityAA << endl;
	os << "AxisDotCityAA " << _rule_set.AxisDotCityAA << endl;
	os << "AxisReferenceCityAA " << _rule_set.AxisReferenceCityAA << endl;
	os << "AxisUnImprovedFortressAA " << _rule_set.AxisUnImprovedFortressAA << endl;
	os << "AxisImprovedFortressAA " << _rule_set.AxisImprovedFortressAA << endl;
	os << "AxisAirfieldAA " << _rule_set.AxisAirfieldAA << endl;

	// naval
	os << "OldNavalSystem " << _rule_set.OldNavalSystem << endl;
	os << "SeparateNavalPhases " << _rule_set.SeparateNavalPhases << endl;
	os << "EnableCDCalc " << _rule_set.EnableCDCalc << endl;

	// stacking
	os << "StackNDivType " << _rule_set.StackNDivType << endl;
	os << "StackDivs " << _rule_set.StackDivs << endl;
	os << "StackNDivs " << _rule_set.StackNDivs << endl;
	os << "StackArt " << _rule_set.StackArt << endl;
	os << "StackMDivs " << _rule_set.StackMDivs << endl;
	os << "StackMNDivs " << _rule_set.StackMNDivs << endl;
	os << "StackMArt " << _rule_set.StackMArt << endl;
	os << "StackADivs " << _rule_set.StackADivs << endl;
	os << "StackANDivs " << _rule_set.StackANDivs << endl;
	os << "StackAArt " << _rule_set.StackAArt << endl;
	os << "StackTGWAllowOneFort " << _rule_set.StackWW1AllowOneFort << endl;

	os << "<Rules" << endl;

	///// store config //////
	os << "Config>" << endl;

	// add new entries HERE! (see below: password must end the section)
	if ( strlen( crt_filename_ ) > 0 )
		os << "CRT " << basename( crt_filename_ ) << endl;  // CRT filename

	if ( strlen( tec_filename_ ) > 0 )
		os << "TEC " << basename( tec_filename_ ) << endl;  // TEC filename

	// passwd must be last of the entries here
	os << "CMV " << _show_combat_markers << endl;  // markers visible?

	// rail RE usage:
	os << "RailRE " << turn_REs_rail_used_ << endl;

	// scramble password
	char* pw = scramble_passwd( marker_password_ );
	os << "CMPW " << pw << endl;  // marker passwd

	os << "<Config" << endl;

	/// store other data ////
	os << "Data>" << endl;

	// this turn losses
	os << "Losses>" << endl;
	// TODO: current and previous turn losses saving does not seem to be functional
	// os << unittype.nimi[GER] << ' ' << losses[GER] << ' '; // nimi is Finnish for name
	os << "<Losses" << endl;
	// previous turn losses
	// TODO: see below for how losses are currently saved
	os << "OldLosses>" << endl;
	// os << unittype.nimi[GER] << ' ' << oldlosses[GER] << ' '; // nimi is Finnish for name
	os << "<OldLosses" << endl;

	os << "<Views>" << endl;
	os << *views_;
	os << "</Views>" << endl;

	os << "<Data" << endl;

	// store 100 yrs of loss statistics //
	//  one line per year
	// TODO: magic number 100
	for ( int y = 0; y < 100; ++y ) // years 1900 through 1999?
	{
		for ( int m = 0; m < 12; ++m )
			for ( int t = 0; t < 2; ++t )
			{
				os << unisol_losses_[y][m][t][SidePlayer::AXIS] << ' ';
				os << isol_losses_[y][m][t][SidePlayer::AXIS] << ' ';
				os << unisol_losses_[y][m][t][SidePlayer::ALLIED] << ' ';
				os << isol_losses_[y][m][t][SidePlayer::ALLIED] << "  ";
			}
		os << endl;
	}

	wxClientDC hdc( this );
	draw_load_status_box( &hdc, "Saving scenario..." );
	hdc.SetPen( *wxBLACK_PEN );

	// store on-map contents,  i.e. the situation //

	// size up progress bar dimensions
	wxSize sz = GetClientSize( );
	int xpos = ( sz.GetWidth( ) / 2 ) - 50;
	int ypos = ( sz.GetHeight( ) / 2 ) - 50;

	int linepos = -1;
	for ( int y = 0; y < world_y_size_; ++y )
	{
		for ( int x = 0; x < world_x_size_; ++x )
		{
			os << sit_[y][x];

			// progress bar update
			if ( ( ( y * 100 / world_y_size_ ) + xpos ) != linepos )
			{
				linepos = ( y * 100 / world_y_size_ ) + xpos;
				hdc.DrawLine( linepos, ypos + 32, linepos, ypos + 48 );
				if ( linepos != 0 )
					hdc.DrawLine( linepos - 1, ypos + 32, linepos - 1, ypos + 48 );
			}
		}
	}

	// store off-map contents //

	os << _repl_pool[SidePlayer::AXIS];
	os << _repl_pool[SidePlayer::ALLIED];
	os << _air_repl_pool[SidePlayer::AXIS];
	os << _air_repl_pool[SidePlayer::ALLIED];

	os.close( );

	// when new scenario save, or existing scenario save-as
	if ( ! filename_is_defined_ )
	{
		strcpy( scn_filename_, filename_to_save );  // save scenario filename
		filename_is_defined_ = true;
		wxGetApp( ).frame_->setFileName( wxS( filename_to_save ) );
		wxGetApp( ).frame_->setCaption( );
	}

	file_is_dirty_ = false;

	wxEndBusyCursor( );
	Refresh( );

	return true;
}

//////////////////////////////
// CRT & TEC file functions //
//////////////////////////////

// PAPI (Frame)
// switch to a custom ground CRT
bool MapPane::initialize_CRT( const char* relative_filename, bool from_tabular /* = true */ )
{
	std::string CRT_path;

	// force CRT to be in the "tabular" subfolder of the provided (e.g. .scn) filename
	if ( from_tabular )
	{
		CRT_path = std::string( pathname( relative_filename ) );
		CRT_path += std::string( "tabular\\" );
		CRT_path += std::string( crt_filename_ );
	}
	else
		CRT_path = std::string( relative_filename );

	return load_CRT( CRT_path.c_str( ) );
#if 0
	if ( ! load_CRT( CRT_path.c_str( ) ) )
	{
		set_CRT_name( DEFAULT_CRT );
		initialize_CRT( wxStandardPaths::Get( ).GetExecutablePath( ).c_str( ) );
	}

	bool successfulLoad = false;

	if ( strncmp( achCRTName, DEFAULT_CRT, strlen( DEFAULT_CRT ) ) != 0 )
	{
		// force CRT to be in the "tabular" subfolder of the provided filename

		std::string temp_CRT_path( pathname( filename ) );
		temp_CRT_path += std::string( "tabular\\" );
		temp_CRT_path += std::string( achCRTName );

		strcpy( achCRTName, temp_CRT_path.c_str( ) );

		// try to load the custom CRT
		if ( load_CRT( ) )
			successfulLoad = true;
		else
		{
			strcpy( achCRTName, DEFAULT_CRT );
			wxMessageBox( wxT("Custom CRT load failed, using built-in CRT in lieu"), Application::NAME );
		}
	}

	if ( ! successfulLoad )
		initializeDefaultCRTvalues( );

	return successfulLoad;
#endif
}

// PAPI (Frame)
void MapPane::set_CRT_name( const char* s )
{
	strcpy( crt_filename_, basename( s ) );
}

// PAPI (Frame)
void MapPane::show_CRT( )
{
	char tmp[200];
	sprintf( msg, "CRT: %s\n\n", crt_filename_ );
	strcat( msg, "    " );
	for ( int j = 0; j < 13; ++j )
	{
		sprintf( tmp, "   %s", _crt_odds_string[j] );
		strcat( msg, tmp );
	}
	strcat( msg, "\n" );
	for ( int die = _crt_lowest_die_roll - ht::SMALLEST_DIE; die <= _crt_highest_die_roll - ht::SMALLEST_DIE; ++die )
	{
		sprintf( tmp, "%2d: ", die + ht::SMALLEST_DIE );
		strcat( msg, tmp );
		for ( int j = 0; j < 13; ++j ) // TODO: lose this magic number
		{
			if ( _crt[j][die] <= 0 )
				sprintf( tmp, "  %s*", _custom_result[- ( _crt[j][die] )].c_str( ) );
			else
				sprintf( tmp, "  %s ", CRTResult[_crt[j][die]].c_str( ) );
			strcat( msg, tmp );
		}
		strcat( msg, "\n" );
	}
	if ( ! _custom_result.empty( ) )
	{
		strcat(	msg, "\n"
				"NOTE: asterisk after result name ('XX*') means non-standard "
				"combat result that is NOT automatically handled by HexTools." );
	}
	wxMessageBox( wxS( msg ), Application::NAME, wxCENTRE );
}

// PAPI (Frame)
// switch to a custom TEC
bool MapPane::initialize_TEC( const char* relative_filename, bool from_tabular /* = true */ )
{
	std::string TEC_path;

	// force TEC to be in the "tabular" subfolder of the provided (e.g. .scn) filename
	if ( from_tabular )
	{
		TEC_path = std::string( pathname( relative_filename ) );
		TEC_path += std::string( "tabular\\" );
		TEC_path += std::string( tec_filename_ );
	}
	else
		TEC_path = std::string( relative_filename );

	return load_TEC( TEC_path.c_str( ) );
#if 0
	if ( ! load_TEC( TEC_path.c_str( ) ) )
	{
		set_TEC_name( DEFAULT_TEC );
		initialize_TEC( wxStandardPaths::Get( ).GetExecutablePath( ).c_str( ) );
	}

	strcpy( achTECName, temp_TEC_path.c_str( ) );

	bool successfulLoad = false;

	if ( load_TEC( ) )
		successfulLoad = true;
	else
	{
		strcpy( achTECName, DEFAULT_TEC );
		wxMessageBox( wxT("Custom TEC load failed, using built-in TEC in lieu"), Application::NAME );
	}

	if ( strncmp( achTECName, DEFAULT_TEC, strlen( DEFAULT_TEC ) ) != 0 )
	{

	}

	if ( ! successfulLoad )
	{
		set_TEC_name( DEFAULT_TEC );
		load_TEC( );
	}
#endif
}

// PAPI (Frame)
void MapPane::set_TEC_name( const char* s )
{
	strcpy( tec_filename_, basename( s ) );
}

// PAPI (Frame)
// switch to a custom UIC
bool MapPane::initialize_UIC( const char* relative_filename, bool from_tabular /* = true */ )
{
	std::string UIC_path;

	// force UIC to be in the "tabular" subfolder of the provided (e.g. .scn) filename
	if ( from_tabular )
	{
		UIC_path = std::string( pathname( relative_filename ) );
		UIC_path += std::string( "tabular\\" );
		UIC_path += std::string( uic_filename_ );
	}
	else
		UIC_path = std::string( relative_filename );

	return load_UIC( UIC_path.c_str( ) );
}

void MapPane::set_UIC_name( const char* s )
{
	strcpy( uic_filename_, basename( s ) );
}

// PAPI (Frame)
// switch to a custom UCC
bool MapPane::initialize_UCC( const char* relative_filename, bool from_tabular /* = true */ )
{
	std::string UIC_path;

	// force UIC to be in the "tabular" subfolder of the provided (e.g. .scn) filename
	if ( from_tabular )
	{
		UIC_path = std::string( pathname( relative_filename ) );
		UIC_path += std::string( "tabular\\" );
		UIC_path += std::string( uic_filename_ );
	}
	else
		UIC_path = std::string( relative_filename );

	return load_UCC( UIC_path.c_str( ) );
}

void MapPane::set_UCC_name( const char* s )
{
	strcpy( ucc_filename_, basename( s ) );
}

// used only by loadCRT(..)
static int get_crt_val( char* _cp, char** endp, char* name )
{
	char bf[255];
	strncpy( bf, _cp, 255 );

	char* cp = bf;
	while ( *cp != ' ' && *cp != '\t' && *cp != '\0' && *cp != '\n' && *cp != '\r' )
		++cp;
	*cp = '\0';
	cp = bf;

	strcpy( name, cp );

	std::string cps( cp );
	int pos = std::find( CRTResult.begin( ), CRTResult.end( ), cps ) - CRTResult.begin( );

	*endp = skipSpace( _cp + strlen( cp ) );
	return pos > 0 && pos < static_cast<int>( CRTResult.size( ) ) ? pos : -1;

#if 0
	for ( int i = 0; i < CRT_ITEMS; ++i )
		if ( strcmp( CRTResult[i].name, cp ) == 0 )
		{
			*endp = skipSpace( _cp + strlen( CRTResult[i].name ) );
			return CRTResult[i].val;
		}

	*endp = skipSpace( _cp + strlen( cp ) );
	return -1;
#endif
}

bool MapPane::load_CRA( const char* cra_filename /* = nullptr */ )
{
	std::ifstream is( cra_filename );
	if ( ! is )
	{
		std::ostringstream CRA_bad_open;
		CRA_bad_open << "Failed to open CRA file " << cra_filename << "!" << endl;
		wxMessageBox( CRA_bad_open.str( ), Application::NAME, wxOK | wxICON_HAND );
		return false;
	}

	std::string cra_line;
	while ( std::getline( is, cra_line ) )
	{
		if ( cra_line.empty( ) || cra_line[0] == '#' )
			continue;
		CRTResult.push_back( cra_line );
	}

	is.close( );
#if 0
	FILE* fp = fopen( filename, "r" );
	if ( ! fp )
	{
	}
#endif
	return true;
}

// TODO: make this static if possible (refactor out class members to initializeCRT(..))
// used only by initializeCRT(..)
bool MapPane::load_CRT( const char* crt_filename )
{
	FILE* fp = fopen( crt_filename, "r" );

	if ( ! fp )
	{
		sprintf( msg, "Failed to open CRT file '%s'!\nReverting back to default CRT", crt_filename );
		wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
		return false;
	}

	//num_custom_results = 0;
	const int NUM_GCRT_COLUMNS = 13;

	char* cp;
	char* cp2;
	bool allow_custom = false;
	char name[100];
	int die;
	int result;
	int first = -99;
	int last = 0;

	char l[255];
	while ( fgets( l, 255, fp ) )
	{
		cp = skipSpace( l );
		if ( *cp == '#' || *cp == ';' || *cp == '\n' )
			continue;
		if ( strncmp( cp, "COMBAT_RESULTS_ARRAY", 20 ) == 0 )
		{
			cp += 20;
			if ( *cp != ' ' )
			{
				std::ostringstream bad_CRA_name;
				bad_CRA_name << "CRT load error: no space before CRA name" << endl;
				wxMessageBox( bad_CRA_name.str( ), Application::NAME, wxOK | wxICON_HAND );
				return false;
			}
			++cp; // advance to CRA filename

			char* bf = cp;
			while ( bf && *bf && *bf != '\n' && *bf != '\t' && *bf != '\r' && *bf != '\0' && *bf != ' ' )
				++bf;
			*bf = '\0'; // null terminate from first non-filename char

			// enforce same path as GCRT
			std::string GCRA_path;
			GCRA_path = std::string( pathname( crt_filename ) );
			GCRA_path += std::string( cp );

			if ( ! load_CRA( GCRA_path.c_str( ) ) )
			{
				std::ostringstream bad_CRA;
				bad_CRA << "CRT load error: bad CRA load" << endl;
				wxMessageBox( bad_CRA.str( ), Application::NAME, wxOK | wxICON_HAND );
				return false;
			}
			continue;
		}
		if ( strncmp( cp, "ALLOW_CUSTOM_RESULTS", 20 ) == 0 )
		{
			allow_custom = true;
			continue;
		}
		die = strtol( cp, &cp, 10 );
		if ( die < ht::SMALLEST_DIE )
		{
			sprintf( msg, "CRT load error: smallest possible modified die result is %d\n", ht::SMALLEST_DIE );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
			fclose( fp );
			return false;
		}
		if ( die > ht::LARGEST_DIE )
		{
			sprintf( msg, "CRT load error: largest possible modified die result is %d\n", ht::LARGEST_DIE );
			wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
			fclose( fp );
			return false;
		}
		if ( first == -99 )
			first = die;
		cp = skipSpace( cp );
		// get results
		last = die;
		die -= ht::SMALLEST_DIE;  // make it valid table index
		for ( int i = 0; i < NUM_GCRT_COLUMNS; ++i )
		{
			result = get_crt_val( cp, &cp2, name );
			if ( result <= 0 )
			{
				if ( allow_custom )
				{
					_custom_result.push_back( name );
					_crt[i][die] = -( _custom_result.size( ) - 1 );
#if 0
					strcpy( _custom_result[num_custom_results].name, name );
					_crt[i][die] = -num_custom_results;
					++num_custom_results;
#endif
				}
				else
				{
					sprintf(	msg,
								"Illegal CRT entry for die %d / odds %s\n%s^here",
								die,
								_crt_odds_string[i],
								cp );
					wxMessageBox( wxS( msg ), Application::NAME, wxOK | wxICON_HAND );
					fclose( fp );
					return false;
				}
			}
			else
				_crt[i][die] = result;
			cp = cp2;
		}
	}
	fclose( fp );

	_crt_lowest_die_roll = first;
	_crt_highest_die_roll = last;

	return true;
}

// used only by loadTEC(..)
static int str2terrain( char* s )
{
	if ( strncmp( s, "NONE", 4 ) == 0 ) return HexType::NONE;
	if ( strncmp( s, "SEA", 3 ) == 0 ) return HexType::SEA;
	if ( strncmp( s, "RESTRICTEDWATERS", 16 ) == 0 ) return HexType::RESTRICTEDWATERS;
	if ( strncmp( s, "CLEAR", 5 ) == 0 ) return HexType::CLEAR;
	if ( strncmp( s, "ROUGH", 5 ) == 0 ) return HexType::ROUGH;
	if ( strncmp( s, "WOODS", 5 ) == 0 ) return HexType::WOODS;
	if ( strncmp( s, "WOODEDROUGH", 11 ) == 0 ) return HexType::WOODEDROUGH;
	if ( strncmp( s, "FOREST", 6 ) == 0 ) return HexType::FOREST;
	if ( strncmp( s, "MOUNTAIN", 8 ) == 0 ) return HexType::MOUNTAIN;
	if ( strncmp( s, "SWAMP", 5 ) == 0 ) return HexType::SWAMP;
	if ( strncmp( s, "LAKE", 4 ) == 0 ) return HexType::LAKE;
	if ( strncmp( s, "WOODEDLAKE", 10 ) == 0 ) return HexType::WOODEDLAKE;
	if ( strncmp( s, "BOCAGE", 6 ) == 0 ) return HexType::BOCAGE;
	if ( strncmp( s, "CANALS", 6 ) == 0 ) return HexType::CANALS;
	if ( strncmp( s, "GLACIER", 7 ) == 0 ) return HexType::GLACIER;
	if ( strncmp( s, "RAVINES", 7 ) == 0 ) return HexType::RAVINES;
	if ( strncmp( s, "SWAMPYTUNDRA", 12 ) == 0 ) return HexType::SWAMPYTUNDRA;
	if ( strncmp( s, "ROCKYTUNDRA", 11 ) == 0 ) return HexType::ROCKYTUNDRA;
	if ( strncmp( s, "SALTDESERT", 10 ) == 0 ) return HexType::SALTDESERT;
	if ( strncmp( s, "SALTLAKE", 8 ) == 0 ) return HexType::SALTLAKE;
	if ( strncmp( s, "SALTMARSH", 9 ) == 0 ) return HexType::SALTMARSH;
	if ( strncmp( s, "SAND", 4 ) == 0 ) return HexType::SAND;
	if ( strncmp( s, "STONYDESERT", 11 ) == 0 ) return HexType::STONYDESERT;
	if ( strncmp( s, "WOODEDSWAMP", 11 ) == 0 ) return HexType::WOODEDSWAMP;
	if ( strncmp( s, "JUNGLEROUGH", 11 ) == 0 ) return HexType::JUNGLEROUGH;
	if ( strncmp( s, "JUNGLEMTN", 9 ) == 0 ) return HexType::JUNGLEMTN;
	if ( strncmp( s, "JUNGLESWAMP", 11 ) == 0 ) return HexType::JUNGLESWAMP;
	// note: JUNGLE must be AFTER other JUNGLEXXXs
	if ( strncmp( s, "JUNGLE", 6 ) == 0 ) return HexType::JUNGLE;
	if ( strncmp( s, "EXTREMERAVINES", 14 ) == 0 ) return HexType::EXTREMERAVINES;
	if ( strncmp( s, "INTIRRIGATION", 13 ) == 0 ) return HexType::INTIRRIGATION;
	if ( strncmp( s, "SEASONALSWAMP", 13 ) == 0 ) return HexType::SEASONALSWAMP;
	if ( strncmp( s, "HEAVYBUSH", 9 ) == 0 ) return HexType::HEAVYBUSH;
	return -1;
}

// used only by loadTEC(..)
static int parse_flags( char* s )
{
	int f = 0;

	char* cp = s;
	while ( 1 )
	{
		cp = skipSpace( cp );
		if ( *cp == '\0' || *cp == '\n' )
			break;
		if ( *cp == '0' )
			cp += 1;
		else if ( strncmp( cp, "NO_AEC", 6 ) == 0 )
			f |= Hex::DEF_NO_AEC, cp += 6;
		else if ( strncmp( cp, "CM_HALVED", 9 ) == 0 )
			f |= Hex::DEF_CM_HALVED, cp += 9;
		else if ( strncmp( cp, "GS_HALVED", 9 ) == 0 )
			f |= Hex::DEF_GS_HALVED, cp += 9;
		else if ( strncmp( cp, "INF_HALVED", 10 ) == 0 )
			f |= Hex::DEF_INF_HALVED, cp += 10;
		else if ( strncmp( cp, "MTN_HALVED", 10 ) == 0 )
			f |= Hex::DEF_MTN_HALVED, cp += 10;
		else if ( strncmp( cp, "ART_HALVED", 10 ) == 0 )
			f |= Hex::DEF_ART_HALVED, cp += 10;
		else if ( strncmp( cp, "FREEZES", 7 ) == 0 )
			f |= Hex::DEF_FREEZES, cp += 7;
		else
			return -1;
	}
	// TODO: this may be wrong, perhaps should be return f; ???
	return 0;
}

// TODO: make this static if possible (refactor out class members to initializeCustomTEC(..))
// used only by initializeCustomTEC(..)
// load TEC from .tec format file, name defined in .scn file
bool MapPane::load_TEC( const char* tec_filename )
{
#if 0
	if ( ! tec_filename )
	{
		if ( strlen( achTECName ) > 0 )
			tec_filename = achTECName;
		else
			return false;
	}

	initializeDefaultTECvalues( );
#endif
	FILE* fp = fopen( tec_filename, "r" );

	if ( ! fp )
	{
		char tecmsg[300];
		sprintf( tecmsg, "Failed to open TEC file '%s'!\nReverting back to default TEC", tec_filename );
		wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
		return false;
	}

	int terrain = HexType::NONE;

	int line = 0;
	char* cp;

	char s[300];
	while ( fgets( s, 300, fp ) )
	{
		++line;
		cp = skipSpace( s );
		// empty or comment line
		if ( strlen( cp ) == 0 || cp[0] == '#' || cp[0] == '\n' )
			continue;
		// new terrain type
		if ( cp[0] == '*' )
		{
			++cp;
			terrain = str2terrain( cp );
			if ( terrain < 0 )
			{
				char tecmsg[300];
				sprintf( tecmsg, "Unknown terrain type '%s' in line %d in TEC file '%s',\nTEC loading aborted!", cp, line, tec_filename );
				wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
				fclose( fp );
				return false;
			}
			continue;
		}
		// set some option:
		if ( cp[0] == '?' )
		{
			if ( strncmp( cp + 1, "TGW_HEXSIDES", 12 ) == 0 )
			{
				cp += 13;
				use_WW1_hexsides_ = strtol( cp, 0, 0 );
				continue;
			}
			char tecmsg[300];
			sprintf( tecmsg, "Unknown option '%s' in line %d in TEC file '%s',\nTEC loading aborted!", cp, line, tec_filename );
			wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
			fclose( fp );
			return false;
		}
		// hexsides:
		if ( cp[0] == '+' )
		{
			if ( strncmp( cp + 1, "MINORCANAL", 10 ) == 0 )
			{
				cp += 11;
				minor_canal_MP_cost_ = strtol( cp, &cp, 0 );
				minor_canal_frozen_MP_cost_ = strtol( cp + 1, 0, 0 );
				continue;
			}
			else if ( strncmp( cp + 1, "RIVER", 5 ) == 0 )
			{  // same for canal
				cp += 6;
				minor_river_MP_cost_ = strtol( cp, &cp, 0 );
				minor_river_frozen_MP_cost_ = strtol( cp + 1, 0, 0 );
				continue;
			}
			else if ( strncmp( cp + 1, "MAJORRIVER", 10 ) == 0 )
			{
				cp += 11;
				major_river_MP_cost_ = strtol( cp, &cp, 0 );
				major_river_frozen_MP_cost_ = strtol( cp + 1, 0, 0 );
				continue;
			}
			char tecmsg[300];
			sprintf( tecmsg, "Unknown hexside '%s' in line %d in TEC file '%s',\nTEC loading aborted!", cp, line, tec_filename );
			wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
			fclose( fp );
			return false;
		}

		// settings for current terrain type
		if ( strncmp( cp, "DIEMOD", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].drm_[WeatherClass::CLEAR] = strtol( cp, 0, 0 );
			// TEC DRMs for Mud and Winter
			cp = skipSpace( cp + 2 );
			if ( cp )
			{
				_hex_types[terrain].drm_[WeatherClass::MUD] = strtol( cp, 0, 0 );
				cp = skipSpace( cp + 2 );
				_hex_types[terrain].drm_[WeatherClass::WINTER] = cp ? strtol( cp, 0, 0 ) : _hex_types[terrain].drm_[WeatherClass::MUD];
			}
			else
			{
				_hex_types[terrain].drm_[WeatherClass::MUD] = _hex_types[terrain].drm_[WeatherClass::CLEAR];
				_hex_types[terrain].drm_[WeatherClass::WINTER] = _hex_types[terrain].drm_[WeatherClass::CLEAR];
			}
		}
		else if ( strncmp( cp, "FLAGS", 5 ) == 0 )
		{
			cp = skipSpace( cp + 5 );
			int i = parse_flags( cp );
			if ( i < 0 )
			{
				char tecmsg[300];
				sprintf( tecmsg, "unknown text in FLAGS field in line %d in TEC file '%s':\n'%s'", line, tec_filename, cp );
				wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
			}
			else
				_hex_types[terrain].defense_flags_ = i; // initializes (to zero)
		}
		else if ( strncmp( cp, "MP_OTHER", 8 ) == 0 )
		{
			cp = skipSpace( cp + 8 );
			_hex_types[terrain].mp_[WeatherClass::CLEAR][MovementCategory::OTHER] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::MUD][MovementCategory::OTHER] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::WINTER][MovementCategory::OTHER] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_CM_ART", 9 ) == 0 )
		{
			cp = skipSpace( cp + 9 );
			_hex_types[terrain].mp_[WeatherClass::CLEAR][MovementCategory::MOT_ART] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::MUD][MovementCategory::MOT_ART] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::WINTER][MovementCategory::MOT_ART] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_LIGHT", 8 ) == 0 )
		{
			cp = skipSpace( cp + 8 );
			_hex_types[terrain].mp_[WeatherClass::CLEAR][MovementCategory::LIGHT] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::MUD][MovementCategory::LIGHT] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::WINTER][MovementCategory::LIGHT] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_CAV", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].mp_[WeatherClass::CLEAR][MovementCategory::MP_CAV] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::MUD][MovementCategory::MP_CAV] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::WINTER][MovementCategory::MP_CAV] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_MTN", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].mp_[WeatherClass::CLEAR][MovementCategory::MP_MTN] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::MUD][MovementCategory::MP_MTN] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::WINTER][MovementCategory::MP_MTN] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_SKI", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].mp_[WeatherClass::CLEAR][MovementCategory::MP_SKI] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::MUD][MovementCategory::MP_SKI] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[WeatherClass::WINTER][MovementCategory::MP_SKI] = strtol( cp, 0, 0 );
		}
		else
		{
			char tecmsg[300];
			sprintf( tecmsg, "Unknown text in line %d in TEC file '%s':\n'%s'", line, tec_filename, cp );
			wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
		}
	}
	fclose( fp );
	return true;
}

static void chk_bmp( wxBitmap* bmp, const char* name )
{
	bool fail = false;
	wxString filename( name );
	wxString s( filename + _(" failed,\nplease check your installation\n\nExit program now (recommend)?") );

	if ( ! bmp )
		fail = true, s = _("creation of bitmap object for ") + s;

	if ( ! fail && ! bmp->Ok( ) )
		fail = true, s = _("loading of bitmap data for ") + s;

	if ( fail && wxMessageBox( s, "Bitmap load failed", wxYES_NO ) == wxYES )
		exit( -1 );
}

static wxBitmap* load_bmp( const char* name )
{
	wxBitmap* bmp = new wxBitmap( ht::wxS( name ), wxBITMAP_TYPE_BMP );
	chk_bmp( bmp, name );
	return bmp;
}

bool MapPane::load_UIC( const char* uic_filename )
{
	// file stream check
	std::ifstream is( uic_filename );
	if ( ! is )
	{
		std::string badstream = "Couldn't open UIC file " + std::string( uic_filename ) + "!";
		wxMessageBox( badstream, Application::NAME );
		return false;
	}

	bool era_defined = false;

	std::string line;
	while ( std::getline( is, line ) )
	{
	    // blank or white-space character starting lines okay (but skipped)
	    if ( line.empty( ) || line[0] == ' ' || line[0] == '\t' || line[0] == '\n' || line[0] == '\r' )
	    	continue;

	    // now tokenize
	    std::istringstream iss( line );

	    std::string first_token;
	    iss >> first_token;

	    // ignore comment lines
	    if ( first_token[0] == '#' )
	    	continue;

	    // uppercase it
	    std::transform( first_token.begin( ), first_token.end( ), first_token.begin( ), ::toupper );

	    // era-definition line format:  "ERA:WW<n>"
	    // first_token[0-2] must be "ERA"
	    const std::string ERA = "ERA";
    	// first_token[3-5] must be ":WW"
	    const std::string WW = ":WW";
	    // first_token[6] "<n>" must be "1" or "2"
	    const char WW1 = '1';
	    const char WW2 = '2';
	    if ( ! era_defined && first_token.compare( 0, 3, ERA ) == 0 )
	    {
	    	if (	first_token.compare( 3, 3, WW ) == 0 &&
	    			( first_token[6] == WW1 || first_token[6] == WW2 ) )
	    		era_defined = true;
	    	else
	    		ht::message_box_warning( "95VG: bad ERA definition in UIC file", first_token );

	    	if ( era_defined )
	    		ground_unit_types_ = first_token[6] == WW1 ? &ground_unit_ww1_types_ : &ground_unit_ww2_types_;

	    	continue;
	    }

	    if ( era_defined ) // only other allowed line is a unit type line item
	    {
	    	// Column #1:  must be a proper unit class string
	    	if (	( GroundUnitType::NON_MOTORIZED_CLASS.compare( first_token ) != 0 ) &&
	    			( GroundUnitType::ARTILLERY_CLASS.compare( first_token ) != 0 ) &&
	    			( GroundUnitType::COMBAT_MOTORIZED_CLASS.compare( first_token ) != 0 ) &&
	    			( GroundUnitType::CAVALRY_CLASS.compare( first_token ) != 0 ) )
	    	{
		    	std::string second_token;
		    	iss >> second_token;
	    		ht::message_box_warning( "kMtm: bad unit class ", second_token + " " + first_token );
	    		continue;
	    	}

	    	// Column #2:  unit type key (i.e. unique)
	    	std::string second_token;
	    	iss >> second_token;

		    // uppercase it
		    std::transform( second_token.begin( ), second_token.end( ), second_token.begin( ), ::toupper );

		    // Column #3:  unit type image filename (no extension)
	    	std::string third_token;
	    	iss >> third_token;

		    // lowercase it
		    std::transform( second_token.begin( ), second_token.end( ), second_token.begin( ), ::tolower );

		    // Column #4:  Longname(Shortname)
	    	std::string fourth_token;
	    	iss >> fourth_token;

	    	// retain case, but convert _ to space
	    	std::replace( fourth_token.begin( ), fourth_token.end( ), '_', ' ');

	    	std::size_t found_open_paren = fourth_token.find_first_of("(");
	    	std::size_t found_close_paren = fourth_token.find_first_of(")");

	    	if ( found_open_paren == std::string::npos || found_close_paren == std::string::npos )
	    	{
	    		ht::message_box_warning( "t7to: bad unit type long(short) name format ", fourth_token );
	    		continue;
	    	}

	    	std::string longname = fourth_token.substr( 0, found_open_paren );

	    	std::string shortname = fourth_token.substr( found_open_paren + 1, found_close_paren - found_open_paren - 1 );

	    	// Column #5:  armor/AT values
	    	std::string fifth_token;
	    	iss >> fifth_token;

		    // uppercase it
		    std::transform( fifth_token.begin( ), fifth_token.end( ), fifth_token.begin( ), ::toupper );

		    // exactly 3 armor/AT value codes
		    const std::string::size_type ARMOR_AT_len = 3;
		    const char FULL = 'F';
		    const char HALF = 'H';
		    const char THIRD = 'T';
		    const char NEUTRAL = 'N';
		    const char NONE = '-';
		    if ( fifth_token.length( ) != ARMOR_AT_len )
		    {
	    		ht::message_box_warning( "cqO4: bad unit type armor/AT values ", second_token + " " + fifth_token );
	    		continue;
		    }

		    bool good_armor_AT_values = true;
		    for ( std::string::size_type i = 0; i < ARMOR_AT_len; ++i )
		    	switch ( fifth_token[i] )
		    	{
		    		case FULL:
		    		case HALF:
		    		case THIRD:		// 1/3 (for unit type Mixed, WW2 only)
		    		case NEUTRAL:	// (for WW2 only)
		    		case NONE:
		    			break;
		    		default:
		    			good_armor_AT_values = false;
		    			break;
		    	}

		    if ( ! good_armor_AT_values )
    		{
	    		ht::message_box_warning( "paRp: bad unit type armor/AT values ", second_token + " " + fifth_token );
	    		continue;
    		}

		    // Column #6:  unit type attribute flags
	    	std::string sixth_token;
	    	iss >> sixth_token;

		    // uppercase it
		    std::transform( sixth_token.begin( ), sixth_token.end( ), sixth_token.begin( ), ::toupper );

		    std::vector<std::string> unit_type_attribs;
		    ht::split( sixth_token, '|', unit_type_attribs );

		    bool good_unit_type_attribs = true;

		    for ( std::vector<std::string>::iterator it = unit_type_attribs.begin(); it != unit_type_attribs.end( ); ++it )
		    	if (	( GroundUnitType::HEAVY_EQUIP_ATTRIB.compare( *it ) != 0 ) &&
		    			( GroundUnitType::COMBAT_ENGINGEER_ATTRIB.compare( *it ) != 0 ) &&
		    			( GroundUnitType::CONSTRUCTION_ATTRIB.compare( *it ) != 0 ) &&
		    			( GroundUnitType::LIGHT_INFANTRY_ATTRIB.compare( *it ) != 0 ) &&
		    			( GroundUnitType::MOUNTAIN_ATTRIB.compare( *it ) != 0 ) &&
		    			( GroundUnitType::SKI_ATTRIB.compare( *it ) != 0 ) &&
		    			( GroundUnitType::RAIL_ONLY_ATTRIB.compare( *it ) != 0 ) &&
		    			( GroundUnitType::AMPHIBIOUS_ATTRIB.compare( *it ) != 0 ) )
		    	{
		    		good_unit_type_attribs = false;
		    		break;
		    	}

		    if ( ! good_unit_type_attribs )
    		{
	    		ht::message_box_warning( "GnJ5: bad unit type attribs ", second_token + " " + sixth_token );
	    		continue;
    		}

		    MovementCategory movement_category;

		    // rough mutually exclusive setting C/m-Art or Other (incl. Cav class)
		    if (	GroundUnitType::ARTILLERY_CLASS.compare( first_token ) == 0 ||
		    		GroundUnitType::COMBAT_MOTORIZED_CLASS.compare( first_token ) == 0 )
		    	movement_category.set_mot_art( );
		    else
		    	movement_category.set_other( );

		    // exceptional flag for Cav class
		    if ( GroundUnitType::CAVALRY_CLASS.compare( first_token ) == 0 )
		    	movement_category.set_cav( );

		    for( std::vector<std::string>::iterator it = unit_type_attribs.begin(); it != unit_type_attribs.end( ); ++it )
		    {
		    	if ( ! movement_category.light( )&& GroundUnitType::LIGHT_INFANTRY_ATTRIB.compare( *it ) == 0 )
		    		movement_category.set_light( );
		    	else if ( ! movement_category.mtn( ) && GroundUnitType::MOUNTAIN_ATTRIB.compare( *it ) == 0 )
		    		movement_category.set_mtn( );
		    	else if ( ! movement_category.ski( ) && GroundUnitType::SKI_ATTRIB.compare( *it ) == 0 )
		    		movement_category.set_ski( );
		    	else if ( ! movement_category.camel( ) && GroundUnitType::CAMEL_ATTRIB.compare( *it ) == 0 )
		    		movement_category.set_camel( );
		    }

			ArmorEffectsValue aeca;
			aeca.set_by_char( fifth_token[0] );
			ArmorEffectsValue aecd;
			aecd.set_by_char( fifth_token[1] );
			ArmorEffectsValue atec;
			atec.set_by_char( fifth_token[2] );

			GroundUnitType type;
			type.name_ = longname;
			type.shortname_ = shortname;
			type.movement_category_ = movement_category;
			type.class_ = first_token;
			type.AECA_value_ = aeca;
			type.AECD_value_ = aecd;
			type.ATEC_value_ = atec;

		    for ( std::vector<std::string>::iterator it = unit_type_attribs.begin(); it != unit_type_attribs.end( ); ++it )
		    {
		    	if ( GroundUnitType::HEAVY_EQUIP_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setHE( );
		    		continue;
		    	}
		    	if ( GroundUnitType::HEAVY_ARMS_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setHA( );
		    		continue;
		    	}
		    	if ( GroundUnitType::RAIL_ONLY_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setRailOnly( );
		    		continue;
		    	}
		    	if ( GroundUnitType::COMBAT_ENGINGEER_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setEng( );
		    		continue;
		    	}
		    	if ( GroundUnitType::CONSTRUCTION_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setCons( );
		    		continue;
		    	}
		    	if ( GroundUnitType::WINTERIZED_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setWinterized( );
		    		continue;
		    	}
		    	if ( GroundUnitType::AMPHIBIOUS_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setAmphibious( );
		    		continue;
		    	}
		    	if ( GroundUnitType::AIRDROP_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setAirdroppable( );
		    		continue;
		    	}
		    	if ( GroundUnitType::COMMANDO_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setCommando( );
		    		continue;
		    	}
		    	if ( GroundUnitType::LIGHT_INFANTRY_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setLightInf( );
		    		continue;
		    	}
		    	if ( GroundUnitType::MOUNTAIN_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setMountain( );
		    		continue;
		    	}
		    	if ( GroundUnitType::SKI_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setSki( );
		    		continue;
		    	}
		    	if ( GroundUnitType::CAMEL_ATTRIB.compare( *it ) == 0 )
		    	{
		    		type.setCamel( );
		    		continue;
		    	}
		    }

const wxString ww2path = "graphics/units/";
const wxString ww1path = "graphics/units/ww1/";

			if ( first_token[6] == '2' )
				type.hbmBitmap = load_bmp( wxT( ww2path + third_token ).c_str( ) );
			else
				type.hbmBitmap = load_bmp( wxT( ww1path + third_token ).c_str( ) );

		    (*ground_unit_types_)[wxT( second_token )] = type;
	    } // some era defined
	}

	if ( ! era_defined )
		wxMessageBox( "idp7: Insufficient UIC file, Era not defined." );

	return era_defined;
#if 0
	FILE* fp = fopen( filename, "r" );

	if ( ! fp )
	{
		char tecmsg[300];
		sprintf( tecmsg, "Failed to open TEC file '%s'!\nReverting back to default TEC", uic_filename );
		wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
		return false;
	}

	int terrain = NONE;

	int line = 0;
	char* cp;

	char s[300];
	while ( fgets( s, 300, fp ) )
	{
		++line;
		cp = skipSpace( s );
		// empty or comment line
		if ( strlen( cp ) == 0 || cp[0] == '#' || cp[0] == '\n' )
			continue;
		// new terrain type
		if ( cp[0] == '*' )
		{
			++cp;
			terrain = str2terrain( cp );
			if ( terrain < 0 )
			{
				char tecmsg[300];
				sprintf( tecmsg, "Unknown terrain type '%s' in line %d in TEC file '%s',\nTEC loading aborted!", cp, line, uic_filename );
				wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
				fclose( fp );
				return false;
			}
			continue;
		}
		// set some option:
		if ( cp[0] == '?' )
		{
			if ( strncmp( cp + 1, "TGW_HEXSIDES", 12 ) == 0 )
			{
				cp += 13;
				use_WW1_hexsides_ = strtol( cp, 0, 0 );
				continue;
			}
			char tecmsg[300];
			sprintf( tecmsg, "Unknown option '%s' in line %d in TEC file '%s',\nTEC loading aborted!", cp, line, uic_filename );
			wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
			fclose( fp );
			return false;
		}
		// hexsides:
		if ( cp[0] == '+' )
		{
			if ( strncmp( cp + 1, "MINORCANAL", 10 ) == 0 )
			{
				cp += 11;
				minor_canal_MP_cost_ = strtol( cp, &cp, 0 );
				minor_canal_frozen_MP_cost_ = strtol( cp + 1, 0, 0 );
				continue;
			}
			else if ( strncmp( cp + 1, "RIVER", 5 ) == 0 )
			{  // same for canal
				cp += 6;
				minor_river_MP_cost_ = strtol( cp, &cp, 0 );
				minor_river_frozen_MP_cost_ = strtol( cp + 1, 0, 0 );
				continue;
			}
			else if ( strncmp( cp + 1, "MAJORRIVER", 10 ) == 0 )
			{
				cp += 11;
				major_river_MP_cost_ = strtol( cp, &cp, 0 );
				major_river_frozen_MP_cost_ = strtol( cp + 1, 0, 0 );
				continue;
			}
			char tecmsg[300];
			sprintf( tecmsg, "Unknown hexside '%s' in line %d in TEC file '%s',\nTEC loading aborted!", cp, line, uic_filename );
			wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
			fclose( fp );
			return false;
		}

		// settings for current terrain type
		if ( strncmp( cp, "DIEMOD", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].drm_[MP_CLEAR] = strtol( cp, 0, 0 );
			// TEC DRMs for Mud and Winter
			cp = skipSpace( cp + 2 );
			if ( cp )
			{
				_hex_types[terrain].drm_[MP_MUD] = strtol( cp, 0, 0 );
				cp = skipSpace( cp + 2 );
				_hex_types[terrain].drm_[MP_WINTER] = cp ? strtol( cp, 0, 0 ) : _hex_types[terrain].drm_[MP_MUD];
			}
			else
			{
				_hex_types[terrain].drm_[MP_MUD] = _hex_types[terrain].drm_[MP_CLEAR];
				_hex_types[terrain].drm_[MP_WINTER] = _hex_types[terrain].drm_[MP_CLEAR];
			}
		}
		else if ( strncmp( cp, "FLAGS", 5 ) == 0 )
		{
			cp = skipSpace( cp + 5 );
			int i = parse_flags( cp );
			if ( i < 0 )
			{
				char tecmsg[300];
				sprintf( tecmsg, "unknown text in FLAGS field in line %d in TEC file '%s':\n'%s'", line, uic_filename, cp );
				wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
			}
			else
				_hex_types[terrain].defense_flags_ = i;
		}
		else if ( strncmp( cp, "MP_OTHER", 8 ) == 0 )
		{
			cp = skipSpace( cp + 8 );
			_hex_types[terrain].mp_[MP_CLEAR][MP_OTHER] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_MUD][MP_OTHER] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_WINTER][MP_OTHER] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_CM_ART", 9 ) == 0 )
		{
			cp = skipSpace( cp + 9 );
			_hex_types[terrain].mp_[MP_CLEAR][MP_CM_ART] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_MUD][MP_CM_ART] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_WINTER][MP_CM_ART] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_LIGHT", 8 ) == 0 )
		{
			cp = skipSpace( cp + 8 );
			_hex_types[terrain].mp_[MP_CLEAR][MP_LIGHT] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_MUD][MP_LIGHT] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_WINTER][MP_LIGHT] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_CAV", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].mp_[MP_CLEAR][MP_CAV] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_MUD][MP_CAV] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_WINTER][MP_CAV] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_MTN", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].mp_[MP_CLEAR][MP_MTN] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_MUD][MP_MTN] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_WINTER][MP_MTN] = strtol( cp, 0, 0 );
		}
		else if ( strncmp( cp, "MP_SKI", 6 ) == 0 )
		{
			cp = skipSpace( cp + 6 );
			_hex_types[terrain].mp_[MP_CLEAR][MP_SKI] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_MUD][MP_SKI] = strtol( cp, 0, 0 );
			cp = skipSpace( cp + 2 );
			_hex_types[terrain].mp_[MP_WINTER][MP_SKI] = strtol( cp, 0, 0 );
		}
		else
		{
			char tecmsg[300];
			sprintf( tecmsg, "Unknown text in line %d in TEC file '%s':\n'%s'", line, uic_filename, cp );
			wxMessageBox( wxS( tecmsg ), Application::NAME, wxOK | wxICON_HAND );
		}
	}
	fclose( fp );
	return true;
#endif

}

bool MapPane::load_UCC( const char* ucc_filename )
{
	// file stream check
	std::ifstream is( ucc_filename );
	if ( ! is )
	{
		std::string badstream = "Couldn't open UCC file " + std::string( ucc_filename ) + "!";
		wxMessageBox( badstream, Application::NAME );
		return false;
	}

	bool era_defined = false;


	return era_defined;
}

// TODO: make static if possible (no class member references)
// used only by loadFiles(..) and saveScenario(..)
void MapPane::draw_load_status_box( wxDC* hdc, const char* statmsg )
{
	wxPen darkpen( wxColour( 60, 60, 60 ), 1, wxSOLID ), lightpen( wxColour( 230, 230, 230 ), 1, wxSOLID );

	hdc->SetBrush( *wxLIGHT_GREY_BRUSH );
	hdc->SetPen( *wxBLACK_PEN );

	int rx, ry;
	GetClientSize( &rx, &ry );

	int xpos = ( rx / 2 ) - 100;
	int ypos = ( ry / 2 ) - 50;

	hdc->DrawRectangle( xpos, ypos, 200, 80 );
	hdc->DrawText( wxS( statmsg ), xpos + 30, ypos + 10 );

	hdc->SetPen( darkpen );
	hdc->DrawLine( xpos + 49, ypos + 50, xpos + 49, ypos + 30 );
	hdc->DrawLine( xpos + 49, ypos + 30, xpos + 151, ypos + 30 );

	hdc->SetPen( lightpen );
	hdc->DrawLine( xpos + 151, ypos + 30, xpos + 151, ypos + 50 );
	hdc->DrawLine( xpos + 151, ypos + 50, xpos + 49, ypos + 50 );

	hdc->SetBrush( *wxWHITE_BRUSH );
	hdc->SetPen( *wxWHITE_PEN );
	hdc->DrawRectangle( xpos + 50, ypos + 31, 100, 18 );
}

#endif
