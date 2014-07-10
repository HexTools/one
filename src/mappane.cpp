#include <vector>
#include <algorithm>
using std::min;
using std::max;

#include <wx/wx.h>
#include <wx/stdpaths.h>

#include "application.h"

#include "hextools.h"
#if defined HEXTOOLSPLAY
using ht::wxS;
using ht::aec2str;
using ht::dir2replayExploitation;
using ht::dir2replay;
#include "gamesetting.h"
#include "sideplayer.h"
#include "weatherclass.h"
#endif

#include "frame.h"
#include "application.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#if defined HEXTOOLSPLAY
#include "rules.h"
#include "movementmode.h"
#include "counter.h"
#include "hexcontents.h"
#include "groundunittype.h"
#include "groundunitsubtype.h"
#include "groundunit.h"
#include "airunitsubtype.h"
#include "airunit.h"
#include "navalunitsubtype.h"
#include "navalunit.h"
#include "facility.h"
#include "hitmarker.h"
#include "statusmarker.h"
#include "hexnote.h"
#include "resourcemarker.h"
#include "groundunitreplpool.h"
#include "airunitreplpool.h"
#include "hexcontentspane.h"
#include "view.h"
#include "views.h"
#include "dunitmover.h"
#include "dhex.h"
#include "dreplacementpool.h"
#include "dsearch.h"
#include "dview.h"
#endif

// table that consists all terraintypes, this is populated in MapPane ctor
extern HexType _hex_types[]; //[TERRAINTYPECOUNT];	// terrain types

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#if 0
extern GroundUnitType _ground_unit_type[GroundUnitType::WW2::UNITTYPECOUNT];	// unit types
#endif
extern GroundArmedForce _ground_armed_force[][GroundArmedForce::Allied::COUNT];	// unit sub types, per side
extern AirArmedForce _air_armed_force[][AirArmedForce::Allied::COUNT];			// unit sub types, per side
extern NavalArmedForce _naval_armed_force[][NavalArmedForce::Allied::COUNT];	// unit sub types, per side

extern int _show_combat_markers;

extern wxWindow* _parent_dialog_window;  // parent wnd for dialogs (used in dialogs)

extern const int _counter_width;
extern const int _counter_height;

extern wxBitmap* hbmDestroyed;

extern wxBitmap* hbmAxis;
extern wxBitmap* hbmAlliedSov;
extern wxBitmap* hbmAlliedUS;
extern wxBitmap* hbmAlliedBr;
extern wxBitmap* hbmAllied;  // points to one of the above

extern wxBitmap* hbmUnitBgnd;

// these are accessed from the Counter subclasses
extern wxBitmap* hbmCompany;
extern wxBitmap* hbmBatallion;
extern wxBitmap* hbmBatallionGrp;
extern wxBitmap* hbmBrigade;
extern wxBitmap* hbmBrigadeGrp;
extern wxBitmap* hbmRegiment;
extern wxBitmap* hbmRegimentGrp;
extern wxBitmap* hbmDivisional;
extern wxBitmap* hbmDivision;
extern wxBitmap* hbmCorps;
extern wxBitmap* hbmArmy;
extern wxBitmap* hbmCadre;
extern wxBitmap* hbmDG;
extern wxBitmap* hbm1RE;				// for transports/APC's
extern wxBitmap* hbm2RE;
extern wxBitmap* hbm3RE;

extern wxBitmap* hbmWarShip;					// Battleship class
extern wxBitmap* hbmCruiser;
extern wxBitmap* hbmDestroyer;
extern wxBitmap* hbmAuxShip;
extern wxBitmap* hbmLC;
extern wxBitmap* hbmSubmarine;
extern wxBitmap* hbmCarrier;
extern wxBitmap* hbmCargoShip;

extern wxBitmap* hbmMtnMask;			// mountain symbol for mountain units

// graphics for various hit etc counters
extern wxBitmap* hbmSmallHit;			// tiny hit marker for hit counters
extern wxBitmap* hbmSmallPort;
extern wxBitmap* hbmCDHit;
extern wxBitmap* hbmHarPlane;
extern wxBitmap* hbmAxMine;
extern wxBitmap* hbmAlMine;
extern wxBitmap* hbmMulberry;
extern wxBitmap* hbmHexMask;  // hex graying
extern wxBitmap* hbmEntrenchent;
extern wxBitmap* hbmImpFort;
extern wxBitmap* hbmFieldWorks;
extern wxBitmap* hbmZepBase;
extern wxBitmap* hbmContested;
extern wxBitmap* hbmBalloons;
extern wxBitmap* hbmSupplyDepot;
extern wxBitmap* hbmRP;
extern wxBitmap* hbmAlert;

// graphics for font in counters
extern wxBitmap* hbmFont[];		// 127

// notes, for GroundUnit.cpp
extern wxBitmap* hbmNote;
extern wxBitmap* hbmCombatReport;
extern wxBitmap* hbmAirCombatReport;
extern wxBitmap* hbmNavalCombatReport;
extern wxBitmap* hbmBombReport;
extern wxBitmap* hbmDieRollReport;

// pens for path
extern wxPen* tac_pen;  // movement phase movement
extern wxPen* expl_pen;  // tactical exploitation movement
extern wxPen* combat_pen;  // advancing after combat

// TODO: is this a HT-m fx, HT-p, or both?
#if 0
static const wxString& SPATH( const char* bmp_name )
{
	static char bf[255];
	sprintf( bf, "%s/%s", "graphics/map/", bmp_name );
	return ht::wxS( bf );
}
#endif

#endif

BEGIN_EVENT_TABLE( MapPane, wxScrolledWindow )
	EVT_PAINT(				MapPane::paint )
	EVT_LEFT_DOWN(			MapPane::EvLButtonDown )
	EVT_RIGHT_DOWN(			MapPane::EvRButtonDown )
	EVT_MOTION(				MapPane::EvMouseMove )
	EVT_LEFT_DCLICK(		MapPane::EvLButtonDblClk )
#if defined HEXTOOLSMAP
	EVT_MENU( MapPane::RMBMenuEvent::LISTMAPSHEETS,				MapPane::CmListMapsheets )
	EVT_MENU( MapPane::RMBMenuEvent::EDITCITY,					MapPane::CmEditcity )
	EVT_MENU( MapPane::RMBMenuEvent::HEX_OPTIONSREMOVE_CITY,	MapPane::CmRemoveCity )
	EVT_MENU( MapPane::RMBMenuEvent::HEX_OPTIONSADDCITY,		MapPane::CmAddCity )
	EVT_MENU( MapPane::RMBMenuEvent::SHRINKMAP,					MapPane::CmShrinkMap )
	EVT_MENU( MapPane::RMBMenuEvent::INSERTMAPHERE,				MapPane::CmInsertMapHere )
	EVT_MENU( MapPane::RMBMenuEvent::HEX_OPTIONSINFO,			MapPane::CmInfo )
	EVT_MENU( MapPane::RMBMenuEvent::MAPLEAFMODE,				MapPane::CmMapleafmode )
	EVT_MENU( MapPane::RMBMenuEvent::SEADISTRICTMODE,			MapPane::CmSeadistrictmode )
	EVT_MENU( MapPane::RMBMenuEvent::CLEARSEACIRCLES,			MapPane::CmClearAllSeaCircles )
	EVT_MENU( MapPane::RMBMenuEvent::LANDDISTRICTMODE,			MapPane::CmLanddistrictmode )
	EVT_MENU( MapPane::RMBMenuEvent::CLEARLANDDISTRICTS,		MapPane::CmClearAllLandDistricts )
	EVT_MENU( MapPane::RMBMenuEvent::HEX_OPTIONSSHOW_WZ,		MapPane::CmShowWZ )
	EVT_MENU( MapPane::RMBMenuEvent::HEX_OPTIONSHIDE_WZ,		MapPane::CmEndModes )
	EVT_MENU( MapPane::RMBMenuEvent::SHOWHEXID,					MapPane::CmShowhexid )
	EVT_MENU( MapPane::RMBMenuEvent::HIDEHEXID,					MapPane::CmHidehexid )
	EVT_MENU( MapPane::RMBMenuEvent::CALC_HEXID,				MapPane::CmCalcHexID )
	EVT_MENU( MapPane::RMBMenuEvent::MAPSHEETORIENTATION,		MapPane::CmMapsheetOrientation )
	EVT_MENU( MapPane::RMBMenuEvent::LISTMAPSHEETS,				MapPane::CmListMapsheets )
	EVT_MENU( MapPane::RMBMenuEvent::NOPORT,					MapPane::CmNoport )
	EVT_MENU( MapPane::RMBMenuEvent::MINORPORT,					MapPane::CmMinorport )
	EVT_MENU( MapPane::RMBMenuEvent::STANDARDPORT,				MapPane::CmStandardport )
	EVT_MENU( MapPane::RMBMenuEvent::MAJORPORT,					MapPane::CmMajorport )
	EVT_MENU( MapPane::RMBMenuEvent::GREATPORT,					MapPane::CmGreatport )
	EVT_MENU( MapPane::RMBMenuEvent::MARGINALPORT,				MapPane::CmMarginalport )
	EVT_MENU( MapPane::RMBMenuEvent::ANCHORAGE,					MapPane::CmAnchorage )
	EVT_MENU( MapPane::RMBMenuEvent::STRONGPORT,				MapPane::CmStrongport )
	EVT_MENU( MapPane::RMBMenuEvent::ARTIFICIALPORT,			MapPane::CmArtificialport )
	EVT_MENU( MapPane::RMBMenuEvent::PORT_CENTER,				MapPane::CmPortCenter )
	EVT_MENU( MapPane::RMBMenuEvent::PORT_NW,					MapPane::CmPortNW )
	EVT_MENU( MapPane::RMBMenuEvent::PORT_NE,					MapPane::CmPortNE )
	EVT_MENU( MapPane::RMBMenuEvent::PORT_W,					MapPane::CmPortW )
	EVT_MENU( MapPane::RMBMenuEvent::PORT_E,					MapPane::CmPortE )
	EVT_MENU( MapPane::RMBMenuEvent::PORT_SW,					MapPane::CmPortSW )
	EVT_MENU( MapPane::RMBMenuEvent::PORT_SE,					MapPane::CmPortSE )
	EVT_MENU( MapPane::RMBMenuEvent::COMMANDSEXTEND_MAP,		MapPane::CmExpandMap )
	EVT_MENU( MapPane::RMBMenuEvent::LIMSTACK_INDICATOR,		MapPane::CmLimitedStacking )
	EVT_MENU( MapPane::RMBMenuEvent::NO_INTRAF,					MapPane::CmNointraf )
	EVT_MENU( MapPane::RMBMenuEvent::INTRAF1,					MapPane::CmIntraf1 )
	EVT_MENU( MapPane::RMBMenuEvent::INTRAF3,					MapPane::CmIntraf3 )
	EVT_MENU( MapPane::RMBMenuEvent::AF_CENTER,					MapPane::CmAfCenter )
	EVT_MENU( MapPane::RMBMenuEvent::AF_NW,						MapPane::CmAfNW )
	EVT_MENU( MapPane::RMBMenuEvent::AF_NE,						MapPane::CmAfNE )
	EVT_MENU( MapPane::RMBMenuEvent::AF_W,						MapPane::CmAfW )
	EVT_MENU( MapPane::RMBMenuEvent::AF_E,						MapPane::CmAfE )
	EVT_MENU( MapPane::RMBMenuEvent::AF_SW,						MapPane::CmAfSW )
	EVT_MENU( MapPane::RMBMenuEvent::AF_SE,						MapPane::CmAfSE )
	EVT_MENU( MapPane::RMBMenuEvent::TOGGLE_LATE_SEAICE,		MapPane::CmToggleLateSeaIce )
	EVT_MENU( MapPane::RMBMenuEvent::NO_DAM,					MapPane::CmNoDam )
	EVT_MENU( MapPane::RMBMenuEvent::DAM_SOUTHWEST,				MapPane::CmDamSW )
	EVT_MENU( MapPane::RMBMenuEvent::DAM_SOUTH,					MapPane::CmDamS )
	EVT_MENU( MapPane::RMBMenuEvent::TOGGLE_RESTR_WATERS,		MapPane::CmToggleRestrictedWaters )
#elif defined HEXTOOLSPLAY
	EVT_LEFT_UP(			MapPane::EvLButtonUp )
	EVT_ERASE_BACKGROUND(	MapPane::EraseBgnd	)
#endif
END_EVENT_TABLE()

// private (keep with ctor impl, along with the ctor private helpers)
// TODO: consider inline
// check that bitmap was loaded ok
static void chk_bmp( wxBitmap* bmp, const char* name )
{
	bool fail = false;
	wxString filename( name );
	wxString s( filename + _(" failed,\nplease check your installation\n\nExit program now (recommend)?") );

	if ( ! bmp )
		fail = true, s = _("creation of bitmap object for ") + s;

	if ( ! fail && ! bmp->IsOk( ) )
		fail = true, s = _("loading of bitmap data for ") + s;

	if ( fail && wxMessageBox( s, "Bitmap load failed", wxYES_NO ) == wxYES )
		exit( -1 );
}

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
// private (keep with ctor impl)
// TODO: consider inline
static wxBitmap* load_bmp( const char* name )
{
	bool is_png = ht::wxS( name ).Lower( ).Find( wxT(".png") ) != wxNOT_FOUND;
	bool is_bmp = ht::wxS( name ).Lower( ).Find( wxT(".bmp") ) != wxNOT_FOUND;

    if ( ( ! is_png && ! is_bmp ) || ( is_png && is_bmp ) )
    {
    	wxString s( name );
    	s += _( "\nplease check your installation\nfor only .png or .bmp images\n\nExit program now (recommended)?" );
    	if ( wxMessageBox( s, "Image type not supported", wxYES_NO ) == wxYES )
    		exit( -1 );
    }

    wxBitmapType type = is_png ? wxBITMAP_TYPE_PNG : wxBITMAP_TYPE_BMP;

	wxBitmap* bmp = new wxBitmap( ht::wxS( name ), type );
	chk_bmp( bmp, name );
	return bmp;
}
#endif

MapPane::MapPane( wxWindow* p, int id, const wxPoint& pt, const wxSize& sz, long WXUNUSED(l) ) : wxScrolledWindow( p, id, pt, sz )
{
	parent_ = static_cast<Frame*>( p );

#if defined HEXTOOLSMAP
	world_x_size_ = 20;   // initial size of the world
	world_y_size_ = 20;
#elif defined HEXTOOLSPLAY
	wxImage::AddHandler( new wxPNGHandler );
	_parent_dialog_window = this;

	map_DC_ = 0;
	click_marching_mode_ = FALSE;
	SetBackgroundColour( *wxWHITE );
	show_owner_ = FALSE;
	_show_combat_markers = TRUE;
	strcpy( marker_password_, "" );
	move_mode_ = MovementMode::TACTICAL;
	left_button_down_ = FALSE;
	dont_redraw_map_ = 0;
	gray_out_radius_ = gray_out_rail_radius_ = gray_out_admin_radius_ = gray_out_strat_radius_ = 9999;
	//	GrayingRadiusOperational =
	//SelectedUnits = nullptr;
	selected_hex_point_.x = selected_hex_point_.y = -1;
	unit_mover_visible_ = FALSE;
	//initializeDefaultCRTvalues( );
	turn_REs_rail_used_ = 0.0F;
	views_ = new Views;
	//aViews.Clear();

	// initialize graphics objects
#if 0
	// deprecate the old JET track and road pens
	if ( ( hpenTrack = new wxPen( wxColour( 150, 0, 0 ), 1, wxDOT ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for trails"), MSGHDR );
		exit( -1 );
	}
	hpenRoad = const_cast<wxPen*>( wxRED_PEN );  // not used (graphics instead)
	hpenAxisRailRoad = wxBLACK_PEN;
	if ((hpenAlliedRailRoad =  new wxPen(wxColour(0, 0, 150), 1, wxSOLID)) == nullptr )
	{
		wxMessageBox(wxT("Couldn't create pen for allied railroads"), MSGHDR);
		exit(-1);
	}
	if ((hpenAxisLVRailRoad = new wxPen(wxColour(0,0,0), 1, wxLONG_DASH)) == nullptr )
	{
		wxMessageBox(wxT("Couldn't create pen for axis railroads"), MSGHDR);
		exit(-1);
	}
	if ((hpenAlliedLVRailRoad = new wxPen(wxColour(0,0,150), 1, wxLONG_DASH)) == nullptr )
	{
		wxMessageBox(wxT("Couldn't create pen for allied railroads"), MSGHDR);
		exit(-1);
	}
	if ( ( hpenAxisStdRR = new wxPen( wxColor( 149, 162, 116 ), 1, wxSOLID ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Axis standard-gauge railroads"), MSGHDR );
		exit( -1 );
	}
	if ( ( hpenAlliedStdRR = new wxPen( wxColor( 79, 148, 205 ), 1, wxSOLID ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Allied standard-gauge railroads"), MSGHDR );
		exit( -1 );
	}
	if ( ( hpenAxisWideRR = new wxPen( wxColor( 118, 100, 155 ), 1, wxSOLID ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Axis broad-gauge railroads"), MSGHDR );
		exit( -1 );
	}
	if ( ( hpenAlliedWideRR = new wxPen( *wxRED, 1, wxSOLID ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Allied broad-gauge railroads"), MSGHDR );
		exit( -1 );
	}
	if ( ( hpenAxisLVStdRR = new wxPen( wxColor( 149, 162, 116 ), 1, wxLONG_DASH ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Axis low-volume standard-gauge railroads"), MSGHDR );
		exit( -1 );
	}
	if ( ( hpenAlliedLVStdRR = new wxPen( wxColor( 79, 148, 205 ), 1, wxLONG_DASH ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Allied low-volume standard-gauge railroads"), MSGHDR );
		exit( -1 );
	}
	if ( ( hpenAxisLVWideRR = new wxPen( wxColor( 118, 100, 155 ), 1, wxLONG_DASH ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Axis low-volume broad-gauge railroads"), MSGHDR );
		exit( -1 );
	}
	if ( ( hpenAlliedLVWideRR = new wxPen( *wxRED, 1, wxLONG_DASH ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for Allied low-volume broad-gauge railroads"), MSGHDR );
		exit( -1 );
	}
#endif
	if ( ( hpenHiliteRed = new wxPen( *wxRED, 3, wxSOLID ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for red hilite"), Application::NAME );
		exit( -1 );
	}
	if ( ( hpenHiliteBlack = new wxPen( *wxBLACK, 4, wxSOLID ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for black hilite"), Application::NAME );
		exit( -1 );
	}
	if ( ( hpenHiliteWhite = new wxPen( *wxWHITE, 4, wxSOLID ) ) == nullptr )
	{
		wxMessageBox( wxT("Couldn't create pen for white hilite"), Application::NAME );
		exit( -1 );
	}
#endif

	hpenPortRoute = new wxPen( *wxWHITE, 3, wxUSER_DASH ); // 3 is width
	wxDash whiteAngles[2];
	whiteAngles[0] = 1;
	whiteAngles[1] = 3;
	hpenPortRoute->SetDashes( sizeof( whiteAngles ), whiteAngles );

	hpenWaterwayRoute = new wxPen( *wxWHITE, 1, wxUSER_DASH ); // 1 is width
	hpenWaterwayRouteOnIce = new wxPen( wxColour(20, 150, 192), 1, wxUSER_DASH ); // dark blue
	wxDash dots[2];
	dots[0] = 1;
	dots[1] = 3; // was 2
	hpenWaterwayRoute->SetDashes( sizeof( dots ), dots );
	hpenWaterwayRouteOnIce->SetDashes( sizeof( dots ), dots );

	hpenDemarcateBorder = new wxPen( *wxWHITE, 1, wxDOT );
	hpenDemarcateIBorder = new wxPen( *wxBLACK, 1, wxDOT );

	// map-16: lake/sea ferry color dependent on underlying sea type
	hpenLakeSeaFerry = new wxPen( *wxWHITE, 1, wxDOT );
	hpenLakeSeaFerryOnIce = new wxPen( wxColour(20, 150, 192), 1, wxDOT ); // dark blue
	hpenShippingChannel = new wxPen( *wxBLACK, 1, wxDOT );

	hpenBar = new wxPen( *wxBLACK, 2, wxSOLID );

	// map-34: rail ferries like perpendicular, black inland port routes closer together
	hpenRailFerry = new wxPen( *wxBLACK, 3, wxUSER_DASH ); // 3 is width
	wxDash blackDashes[2];
	blackDashes[0] = 1;
	blackDashes[1] = 2;
	hpenRailFerry->SetDashes( sizeof( blackDashes ), blackDashes );

	hpenTrack = new wxPen( wxColour(96, 56, 56), 1, wxDOT ); // map-25
	hpenMotorTrack = new wxPen( wxColour(176, 132, 84), 1, wxDOT );
	hpenRoad = new wxPen( wxColour(176, 132, 84), 1, wxSOLID );
	hpenRailTrack = new wxPen( wxColour(128, 128, 128), 1, wxSOLID );
	hpenRailRoad = const_cast<wxPen*>( wxBLACK_PEN ); // black, 1, solid
	hpenPriCityBoulevard = new wxPen( wxColour(255, 251, 240), 3, wxSOLID );

	hpenEscarpment = new wxPen( wxColor( 130, 130, 0 ), 5, wxSOLID );
	hpenImpEscarpment = new wxPen( wxColor( 60, 60, 0 ), 5, wxSOLID );
	//hpenFort = new wxPen( wxColor( 0, 0, 0 ), 3, wxSOLID );

// TODO:  confirm that nothing is omitted by commenting out these include-macro graphics loading
#if 0
#if defined HEXTOOLSPLAY
#undef	LH
#define	LH(tt, tstr, bmstr) \
do { \
	HexTypes[tt].achName = tstr; \
    HexTypes[tt].hbmBitmap = new wxBitmap( CLEAR_PATH(bmstr), wxBITMAP_TYPE_BMP ); \
    HexTypes[tt].hbmMudBitmap = HexTypes[tt].hbmBitmap; \
    HexTypes[tt].hbmSnowBitmap = HexTypes[tt].hbmBitmap; \
    HexTypes[tt].hbmFrostBitmap = HexTypes[tt].hbmBitmap; \
} while ( 0 )
#undef	LH

#undef	LHW
#define	LHW(tt, tstr, bmstr) \
do { \
	HexTypes[tt].achName = tstr; \
    HexTypes[tt].hbmBitmap = new wxBitmap( CLEAR_PATH(bmstr), wxBITMAP_TYPE_BMP ); \
    HexTypes[tt].hbmMudBitmap = new wxBitmap( MUD_PATH(bmstr), wxBITMAP_TYPE_BMP ); \
    HexTypes[tt].hbmSnowBitmap = new wxBitmap( SNOW_PATH(bmstr), wxBITMAP_TYPE_BMP ); \
    HexTypes[tt].hbmFrostBitmap = new wxBitmap( FROST_PATH(bmstr), wxBITMAP_TYPE_BMP ); \
} while ( 0 )
#undef	LHW

#undef	LB
#define	LB(name, bmstr) \
do { \
	name = new wxBitmap( SPATH(bmstr), wxBITMAP_TYPE_BMP ); \
} while ( 0 )

	// cities
	LB(hbmFullHexCity   , "fullhex.bmp");
	LB(hbmPartialHexCity, "partialhex.bmp");
	LB(hbmMinorCity     , "refcity.bmp");
	LB(hbmMediumCity    , "dotcity.bmp");
	LB(hbmFortress      , "fortress.bmp");
	LB(hbmOuvrage       , "ouvrage.bmp");
	LB(hbmWestWall      , "westwall.bmp");
	LB(hbmPtCity        , "point_city.bmp");
	LB(hbmRefPt         , "refpoint.bmp");
	LB(hbmCityBgnd      , "citybgnd.bmp");
	LB(hbmCoalRes       , "coalrefpt.bmp");
	LB(hbmOasis         , "oasis.bmp");
	LB(hbmWW1OFort      , "oldfortress.bmp");
	LB(hbmWW1NFort      , "newfortress.bmp");
	LB(hbmWW1GFort      , "greatfortress.bmp");
	LB(hbmNWGWall		, "nw_gwall.bmp");
	LB(hbmNEGWall		, "ne_gwall.bmp");
	LB(hbmEGWall		, "e_gwall.bmp");
	LB(hbmWGWall		, "w_gwall.bmp");
	LB(hbmSEGWall		, "se_gwall.bmp");
	LB(hbmSWGWall		, "sw_gwall.bmp");

	// hexsides
	LB(hbmWWater		, "w_water.bmp");
	LB(hbmSWWater		, "sw_water.bmp");
	LB(hbmSEWater		, "se_water.bmp");
	LB(hbmWRWater		, "w_rwater.bmp");
	LB(hbmSWRWater		, "sw_rwater.bmp");
	LB(hbmSERWater		, "se_rwater.bmp");
	LB(hbmWMountain		, "w_mountain.bmp");
	LB(hbmSWMountain	, "sw_mountain.bmp");
	LB(hbmSEMountain	, "se_mountain.bmp");
	LB(hbmWHighMtn		, "w_highmtn.bmp");
	LB(hbmSWHighMtn		, "sw_highmtn.bmp");
	LB(hbmSEHighMtn		, "se_highmtn.bmp");
	LB(hbmWHighMtnPass	, "w_highmtnpass.bmp");
	LB(hbmSWHighMtnPass	, "sw_highmtnpass.bmp");
	LB(hbmSEHighMtnPass	, "se_highmtnpass.bmp");
	LB(hbmWKarst 		, "w_karst.bmp");
	LB(hbmSWKarst		, "sw_karst.bmp");
	LB(hbmSEKarst		, "se_karst.bmp");
	LB(hbmWCanal  		, "w_canal.bmp");
	LB(hbmSWCanal 		, "sw_canal.bmp");
	LB(hbmSECanal 		, "se_canal.bmp");
	LB(hbmWMinorCanal  	, "w_minor_canal.bmp");
	LB(hbmSWMinorCanal 	, "sw_minor_canal.bmp");
	LB(hbmSEMinorCanal 	, "se_minor_canal.bmp");
	LB(hbmWMinRiv		, "w_river.bmp");
	LB(hbmSWMinRiv		, "sw_river.bmp");
	LB(hbmSEMinRiv		, "se_river.bmp");
	LB(hbmWMinRivIce	, "w_minriver_ice.bmp");
	LB(hbmSWMinRivIce	, "sw_minriver_ice.bmp");
	LB(hbmSEMinRivIce	, "se_minriver_ice.bmp");
	LB(hbmWMajRiv		, "w_major_river.bmp");
	LB(hbmSWMajRiv		, "sw_major_river.bmp");
	LB(hbmSEMajRiv		, "se_major_river.bmp");
	LB(hbmWMajRivIce	, "w_majriver_ice.bmp");
	LB(hbmSWMajRivIce	, "sw_majriver_ice.bmp");
	LB(hbmSEMajRivIce	, "se_majriver_ice.bmp");
	LB(hbmWBorder		, "w_border.bmp");
	LB(hbmSWBorder		, "sw_border.bmp");
	LB(hbmSEBorder		, "se_border.bmp");
	LB(hbmWIBorder		, "w_iborder.bmp");
	LB(hbmSWIBorder		, "sw_iborder.bmp");
	LB(hbmSEIBorder		, "se_iborder.bmp");
	LB(hbmWBdrMinRiver	, "w_border_river.bmp");
	LB(hbmSWBdrMinRiver	, "sw_border_river.bmp");
	LB(hbmSEBdrMinRiver	, "se_border_river.bmp");
	LB(hbmWIBdrMinRiver	, "w_iborder_river.bmp");
	LB(hbmSWIBdrMinRiver, "sw_iborder_river.bmp");
	LB(hbmSEIBdrMinRiver, "se_iborder_river.bmp");
	LB(hbmWWadi			, "w_wadi.bmp");
	LB(hbmSWWadi		, "sw_wadi.bmp");
	LB(hbmSEWadi		, "se_wadi.bmp");
	LB(hbmWGlacier		, "w_glacier.bmp");
	LB(hbmSWGlacier		, "sw_glacier.bmp");
	LB(hbmSEGlacier		, "se_glacier.bmp");
	LB(hbmWSaltDesert	, "w_saltdesert.bmp");
	LB(hbmSWSaltDesert	, "sw_saltdesert.bmp");
	LB(hbmSESaltDesert	, "se_saltdesert.bmp");
	LB(hbmMinorPort		, "port_pacificminor.bmp");
	LB(hbmStandardPort	, "port_standard.bmp");
	LB(hbmMajorPort		, "port_major.bmp");
	LB(hbmGreatPort		, "port_great.bmp");
	LB(hbmMarginalPort	, "port_marginal.bmp");
	LB(hbmAnchorage		, "port_anchorage.bmp");
	LB(hbmArtificialPort, "port_artifical.bmp");
	LB(hbmStrongPort	, "port_strong.bmp");
	LB(hbmNWFort		, "nw_fort.bmp");
	LB(hbmNEFort		, "ne_fort.bmp");
	LB(hbmEFort 		, "e_fort.bmp");
	LB(hbmSEFort		, "se_fort.bmp");
	LB(hbmSWFort		, "sw_fort.bmp");
	LB(hbmWFort 		, "w_fort.bmp");
	LB(hbmNWEsc 		, "nw_escarpment.bmp");
	LB(hbmNEEsc 		, "ne_escarpment.bmp");
	LB(hbmEEsc  		, "e_escarpment.bmp");
	LB(hbmSWEsc 		, "sw_escarpment.bmp");
	LB(hbmSEEsc 		, "se_escarpment.bmp");
	LB(hbmWEsc  		, "w_escarpment.bmp");
	LB(hbmWMask			, "w_mask.bmp");
	LB(hbmSWMask		, "sw_mask.bmp");
	LB(hbmSEMask		, "se_mask.bmp");
	LB(hbmSWClear		, "sw_clear.bmp");
	LB(hbmSEClear		, "se_clear.bmp");
	LB(hbmWClear		, "w_clear.bmp");
	LB(hbmSWRough 		, "sw_rough.bmp");
	LB(hbmSERough 		, "se_rough.bmp");
	LB(hbmWRough  		, "w_rough.bmp");
	LB(hbmSWForest		, "sw_forest.bmp");
	LB(hbmSEForest		, "se_forest.bmp");
	LB(hbmWForest 		, "w_forest.bmp");
	LB(hbmSWWoods		, "sw_woods.bmp");
	LB(hbmSEWoods		, "se_woods.bmp");
	LB(hbmWWoods 		, "w_woods.bmp");
	LB(hbmEscColor		, "esccolor.bmp");
#undef	LB

#endif
#endif

	// set up and load hex terrain graphics

	define_hex_type( HexType::HEXMASK, "Hex Mask", "HexMask" );
	define_hex_type( HexType::NONE, "None", "None" );
	define_hex_type( HexType::CLEAR, "Clear", "Clear" );
	define_hex_type( HexType::ROUGH, "Rough", "Rough" );
	define_hex_type( HexType::WOODS, "Woods", "Woods" );
	define_hex_type( HexType::WOODEDROUGH, "Wooded Rough", "RoughWooded" );
	define_hex_type( HexType::FOREST, "Forest", "Forest" );
	define_hex_type( HexType::MOUNTAIN, "Mountain", "Mountain" );
	define_hex_type( HexType::SWAMP, "Swamp", "Swamp" );
	define_hex_type( HexType::SEA, "Sea/Lake", "Sea" );
	define_hex_type( HexType::LAKE, "Intermittent Lake", "LakeIntermittent" );
	define_hex_type( HexType::WOODEDLAKE, "Wooded Intermittent Lake", "LakeIntermittentWooded" );
	define_hex_type( HexType::BOCAGE, "Bocage", "Bocage" );
	define_hex_type( HexType::CANALS, "Canal Intensive", "CanalIntensive" );
	define_hex_type( HexType::GLACIER, "Glacier", "Glacier" );
	define_hex_type( HexType::RAVINES, "Ravines", "Ravines" );
	define_hex_type( HexType::EXTREMERAVINES, "Extreme Ravines", "RavinesExtreme" );
	define_hex_type( HexType::SWAMPYTUNDRA, "Swampy Tundra", "TundraSwampy" );
	define_hex_type( HexType::SALTDESERT, "Salt Desert", "DesertSalt" );
	define_hex_type( HexType::SALTLAKE, "Intermittent Salt Lake", "LakeIntermittentSalt" );
	define_hex_type( HexType::SALTMARSH, "Salt Marsh", "DesertMarshSalt" );
	define_hex_type( HexType::SAND, "Sand", "DesertSand" );
	define_hex_type( HexType::STONYDESERT, "Stony Desert", "DesertStony" );
	define_hex_type( HexType::WOODEDSWAMP, "Wooded Swamp", "SwampWooded" );
	define_hex_type( HexType::JUNGLE, "Jungle", "Jungle" );
	define_hex_type( HexType::JUNGLEROUGH, "Jungle Rough", "JungleRough" );
	define_hex_type( HexType::JUNGLEMTN, "Jungle Mountain", "JungleMountain" );
	define_hex_type( HexType::JUNGLESWAMP, "Jungle Swamp", "JungleSwamp" );
	define_hex_type( HexType::FILLINGRESERVOIR, "Filling Reservoir", "ReservoirFilling" );
	define_hex_type( HexType::ATOLL, "Atoll", "IslandAtoll" );
	define_hex_type( HexType::SMALLISLAND, "Small Island", "IslandSmall" );
	define_hex_type( HexType::INTIRRIGATION, "Intensive Irrigation", "IrrigationIntensive" );
	define_hex_type( HexType::ICINGSEA, "Sea/Lake", "SeaIce" );
	define_hex_type( HexType::SEASONALSWAMP, "Seasonal Swamp", "SwampSeasonal" );
	define_hex_type( HexType::HEAVYBUSH, "Heavy Bush", "BushHeavy" );
	define_hex_type( HexType::ROCKYTUNDRA, "Rocky Tundra", "TundraRocky" );
	define_hex_type( HexType::SPECIALSWAMP, "Special Swamp", "SwampSpecial" );
	define_hex_type( HexType::SPECIALSEASONALSWAMP, "Special Seasonal Swamp", "SwampSeasonalSpecial" );
	define_hex_type( HexType::HIGHMOUNTAIN, "High Mountain", "MountainHigh" );

	std::string restricted_waters_overlay_filename( Application::HEX_BITMAPS_PATH );
	restricted_waters_overlay_filename.append( "OverlayRestricted.bmp" );
	hbmRestrictedWatersOverlay = new wxBitmap( restricted_waters_overlay_filename.c_str( ), wxBITMAP_TYPE_BMP );
	chk_bmp( hbmRestrictedWatersOverlay, "OverlayRestricted" );

	std::string custom_island_mask_filename( Application::HEX_BITMAPS_PATH );
	custom_island_mask_filename.append( "IslandMaskFullInv.bmp" );
	hbmIslandCustomMask = new wxBitmap( custom_island_mask_filename.c_str( ), wxBITMAP_TYPE_BMP );
	chk_bmp( hbmIslandCustomMask, "IslandMaskFullInv" );

	// load hexside terrain graphics

	hbmWWater = define_hexside_type( "Sea/Lake (W)", "Sea_w" );
	hbmSWWater = define_hexside_type( "Sea/Lake (SW)", "Sea_sw" );
	hbmSEWater = define_hexside_type( "Sea/Lake (SE)", "Sea_se" );

	hbmWIcingSea = define_hexside_type( "Sea/Lake (ices) (W)", "SeaIce_w" );
	hbmSWIcingSea = define_hexside_type( "Sea/Lake (ices) (SW)", "SeaIce_sw" );
	hbmSEIcingSea = define_hexside_type( "Sea/Lake (ices) (SE)", "SeaIce_se" );

	hbmWLakeRiverMask = define_hexside_type( "Lake/Great River Mask (W)", "RiverGreatMask_w" );
	hbmSWLakeRiverMask = define_hexside_type( "Lake/Great River Mask (SW)", "RiverGreatMask_sw" );
	hbmSELakeRiverMask = define_hexside_type( "Lake/Great River Mask (SE)", "RiverGreatMask_se" );

	hbmWRWater = define_hexside_type( "Restricted Sea (W)", "SeaRestricted_w" );
	hbmSWRWater = define_hexside_type( "Restricted Sea (SW)", "SeaRestricted_sw" );
	hbmSERWater = define_hexside_type( "Restricted Sea (SE)", "SeaRestricted_se" );

	hbmWIcingRWater = define_hexside_type( "Restricted Sea (ices) (W)", "SeaRestrictedIce_w" );
	hbmSWIcingRWater = define_hexside_type( "Restricted Sea (ices) (SW)", "SeaRestrictedIce_sw" );
	hbmSEIcingRWater = define_hexside_type( "Restricted Sea (ices) (SE)", "SeaRestrictedIce_se" );

	hbmWMountainMask = define_hexside_type( "Mountain Mask (W)", "MtnMask_w" );
	hbmSWMountainMask = define_hexside_type( "Mountain Mask (SW)", "MtnMask_sw" );
	hbmSEMountainMask = define_hexside_type( "Mountain Mask (SE)", "MtnMask_se" );
	hbmWMountain = define_hexside_type( "Mountain (W)", "Mtn_w" );
	hbmSWMountain = define_hexside_type( "Mountain (SW)", "Mtn_sw" );
	hbmSEMountain = define_hexside_type( "Mountain (SE)", "Mtn_se" );

	hbmWHighMtnMask = define_hexside_type( "High Mountain Mask (W)", "MtnHighMask_w" );
	hbmSWHighMtnMask = define_hexside_type( "High Mountain Mask (SW)", "MtnHighMask_sw" );
	hbmSEHighMtnMask = define_hexside_type( "High Mountain Mask (SE)", "MtnHighMask_se" );
	hbmWHighMtn = define_hexside_type( "High Mountain (W)", "MtnHigh_w" );
	hbmSWHighMtn = define_hexside_type( "High Mountain (SW)", "MtnHigh_sw" );
	hbmSEHighMtn = define_hexside_type( "High Mountain (SE)", "MtnHigh_se" );

	hbmWHighMtnPass = define_hexside_type( "High Mountain Pass (W)", "MtnHighPass_w" );
	hbmSWHighMtnPass = define_hexside_type( "High Mountain Pass (SW)", "MtnHighPass_sw" );
	hbmSEHighMtnPass = define_hexside_type( "High Mountain Pass (SE)", "MtnHighPass_se" );

	hbmWFordMask = define_hexside_type( "Ford Mask (W)", "FordMask_w" );
	hbmSWFordMask = define_hexside_type( "Ford Mask (SW)", "FordMask_sw" );
	hbmSEFordMask = define_hexside_type( "Ford Mask (SE)", "FordMask_se" );
	hbmWFord = define_hexside_type( "Ford (W)", "Ford_w" );
	hbmSWFord = define_hexside_type( "Ford (SW)", "Ford_sw" );
	hbmSEFord = define_hexside_type( "Ford (SE)", "Ford_se" );

	hbmWMajRivMask = define_hexside_type( "Major River Mask (W)", "RiverMajorMask_w" );
	hbmSWMajRivMask = define_hexside_type( "Major River Mask (SW)", "RiverMajorMask_sw" );
	hbmSEMajRivMask = define_hexside_type( "Major River Mask (SE)", "RiverMajorMask_se" );
	hbmWMajRiv = define_hexside_type( "Major River (W)", "RiverMajor_w" );
	hbmSWMajRiv = define_hexside_type( "Major River (SW)", "RiverMajor_sw" );
	hbmSEMajRiv = define_hexside_type( "Major River (SE)", "RiverMajor_se" );

	hbmWRBorder = define_hexside_type( "Red Solid Border (W)", "BorderRed_w" );
	hbmSWRBorder = define_hexside_type( "Red Solid Border (SW)", "BorderRed_sw" );
	hbmSERBorder = define_hexside_type( "Red Solid Border (SE)", "BorderRed_se" );

	hbmWBorder = define_hexside_type( "International Border (W)", "BorderBlack_w" );
	hbmSWBorder = define_hexside_type( "International Border (SW)", "BorderBlack_sw" );
	hbmSEBorder = define_hexside_type( "International Border (SE)", "BorderBlack_se" );

	hbmWIBorder = define_hexside_type( "Internal Border (W)", "BorderGray_w" );
	hbmSWIBorder = define_hexside_type( "Internal Border (SW)", "BorderGray_sw" );
	hbmSEIBorder = define_hexside_type( "Internal Border (SE)", "BorderGray_se" );

	hbmWISBorder = define_hexside_type( "Gray Sub-Border (W)", "BorderSubGray_w" );
	hbmSWISBorder = define_hexside_type( "Gray Sub-Border (SW)", "BorderSubGray_sw" );
	hbmSEISBorder = define_hexside_type( "Gray Sub-Border (SE)", "BorderSubGray_se" );

	hbmWIBBorder = define_hexside_type( "Black Sub-Border (W)", "BorderSubBlack_w" );
	hbmSWIBBorder = define_hexside_type( "Black Sub-Border (SW)", "BorderSubBlack_sw" );
	hbmSEIBBorder = define_hexside_type( "Black Sub-Border (SE)", "BorderSubBlack_se" );

	hbmWISBorderMask = define_hexside_type( "Sub-Border Mask (W)", "BorderSubGrayMask_w" );
	hbmSWISBorderMask = define_hexside_type( "Sub-Border Mask (SW)", "BorderSubGrayMask_sw" );
	hbmSEISBorderMask = define_hexside_type( "Sub-Border Mask (SE)", "BorderSubGrayMask_se" );

	// seasonal rivers
	hbmWSeasonalRiv = define_hexside_type( "Seasonal River (W)", "RiverSeasonal_w" );
	hbmSWSeasonalRiv = define_hexside_type( "Seasonal River (SW)", "RiverSeasonal_sw" );
	hbmSESeasonalRiv = define_hexside_type( "Seasonal River (SE)", "RiverSeasonal_se" );
	hbmWSeasonalRivMask = define_hexside_type( "Seasonal River (W)", "RiverSeasonalMask_w" );
	hbmSWSeasonalRivMask = define_hexside_type( "Seasonal River (SW)", "RiverSeasonalMask_sw" );
	hbmSESeasonalRivMask = define_hexside_type( "Seasonal River (SE)", "RiverSeasonalMask_se" );

	hbmWBdrSeasonRiver = define_hexside_type( "Int'l Bdr on Seasonal River (W)", "BorderBlack_RiverSeasonal_w" );
	hbmSWBdrSeasonRiver = define_hexside_type( "Int'l Bdr on Seasonal River (SW)", "BorderBlack_RiverSeasonal_sw" );
	hbmSEBdrSeasonRiver = define_hexside_type( "Int'l Bdr on Seasonal River (SE)", "BorderBlack_RiverSeasonal_se" );

	hbmWIBdrSeasonRiver = define_hexside_type( "Int. Bdr on Seasonal River (W)", "BorderGray_RiverSeasonal_w" );
	hbmSWIBdrSeasonRiver = define_hexside_type( "Int. Bdr on Seasonal River (SW)", "BorderGray_RiverSeasonal_sw" );
	hbmSEIBdrSeasonRiver = define_hexside_type( "Int. Bdr on Seasonal River (SE)", "BorderGray_RiverSeasonal_se" );

	hbmWISBdrSeasonRiver = define_hexside_type( "Gray Sub-Bdr on Seasonal River (W)", "BorderSubGray_RiverSeasonal_w" );
	hbmSWISBdrSeasonRiver = define_hexside_type( "Gray Sub-Bdr on Seasonal River (SW)", "BorderSubGray_RiverSeasonal_sw" );
	hbmSEISBdrSeasonRiver = define_hexside_type( "Gray Sub-Bdr on Seasonal River (SE)", "BorderSubGray_RiverSeasonal_se" );
#if 0
	hbmWIBBdrSeasonRiver = define_hexside_type( "Black Sub-Bdr on Seasonal River (W)", "BorderSubBlack_RiverSeasonal_w" );
	hbmSWIBBdrSeasonRiver = define_hexside_type( "Black Sub-Bdr on Seasonal River (SW)", "BorderSubBlack_RiverSeasonal_sw" );
	hbmSEIBBdrSeasonRiver = define_hexside_type( "Black Sub-Bdr on Seasonal River (SE)", "BorderSubBlack_RiverSeasonal_se" );
#endif
	// "minor" rivers
	hbmWMinRiv = define_hexside_type( "River (W)", "RiverStd_w" );
	hbmSWMinRiv = define_hexside_type( "River (SW)", "RiverStd_sw" );
	hbmSEMinRiv = define_hexside_type( "River (SE)", "RiverStd_se" );

	hbmWBdrMinRiver = define_hexside_type( "Int'l Bdr on River (W)", "BorderBlack_River_w" );
	hbmSWBdrMinRiver = define_hexside_type( "Int'l Bdr on River (SW)", "BorderBlack_River_sw" );
	hbmSEBdrMinRiver = define_hexside_type( "Int'l Bdr on River (SE)", "BorderBlack_River_se" );

	hbmWIBdrMinRiver = define_hexside_type( "Int. Bdr on River (W)", "BorderGray_River_w" );
	hbmSWIBdrMinRiver = define_hexside_type( "Int. Bdr on River (SW)", "BorderGray_River_sw" );
	hbmSEIBdrMinRiver = define_hexside_type( "Int. Bdr on River (SE)", "BorderGray_River_se" );

	hbmWISBdrMinRiver = define_hexside_type( "Gray Sub-Bdr on River (W)", "BorderSubGray_River_w" );
	hbmSWISBdrMinRiver = define_hexside_type( "Gray Sub-Bdr on River (SW)", "BorderSubGray_River_sw" );
	hbmSEISBdrMinRiver = define_hexside_type( "Gray Sub-Bdr on River (SE)", "BorderSubGray_River_se" );
#if 0
	hbmWIBBdrMinRiver = define_hexside_type( "Black Sub-Bdr on River (W)", "BorderSubBlack_River_w" );
	hbmSWIBBdrMinRiver = define_hexside_type( "Black Sub-Bdr on River (SW)", "BorderSubBlack_River_sw" );
	hbmSEIBBdrMinRiver = define_hexside_type( "Black Sub-Bdr on River (SE)", "BorderSubBlack_River_se" );
#endif
	hbmWMinRivMask = define_hexside_type( "River Mask (W)", "RiverStdMask_w" );
	hbmSWMinRivMask = define_hexside_type( "River Mask (SW)", "RiverStdMask_sw" );
	hbmSEMinRivMask = define_hexside_type( "River Mask (SE)", "RiverStdMask_se" );

	// canals
	hbmWCanal = define_hexside_type( "Canal (W)", "Canal_w" );
	hbmSWCanal = define_hexside_type( "Canal (SW)", "Canal_sw" );
	hbmSECanal = define_hexside_type( "Canal (SE)", "Canal_se" );

	hbmWBdrCanal = define_hexside_type( "Int'l Bdr on Canal (W)", "BorderBlack_Canal_w" );
	hbmSWBdrCanal = define_hexside_type( "Int'l Bdr on Canal (SW)", "BorderBlack_Canal_sw" );
	hbmSEBdrCanal = define_hexside_type( "Int'l Bdr on Canal (SE)", "BorderBlack_Canal_se" );

	hbmWIBdrCanal = define_hexside_type( "Int. Bdr on Canal (W)", "BorderGray_Canal_w" );
	hbmSWIBdrCanal = define_hexside_type( "Int. Bdr on Canal (SW)", "BorderGray_Canal_sw" );
	hbmSEIBdrCanal = define_hexside_type( "Int. Bdr on Canal (SE)", "BorderGray_Canal_se" );

	hbmWISBdrCanal = define_hexside_type( "Gray Sub-Bdr on Canal (W)", "BorderSubGray_Canal_w" );
	hbmSWISBdrCanal = define_hexside_type( "Gray Sub-Bdr on Canal (SW)", "BorderSubGray_Canal_sw" );
	hbmSEISBdrCanal = define_hexside_type( "Gray Sub-Bdr on Canal (SE)", "BorderSubGray_Canal_se" );
#if 0
	hbmWIBBdrCanal = define_hexside_type( "Black Sub-Bdr on Canal (W)", "BorderSubBlack_Canal_w" );
	hbmSWIBBdrCanal = define_hexside_type( "Black Sub-Bdr on Canal (SW)", "BorderSubBlack_Canal_sw" );
	hbmSEIBBdrCanal = define_hexside_type( "Black Sub-Bdr on Canal (SE)", "BorderSubBlack_Canal_se" );
#endif
	hbmWCanalMask = define_hexside_type( "Canal Mask (W)", "CanalMask_w" );
	hbmSWCanalMask = define_hexside_type( "Canal Mask (SW)", "CanalMask_sw" );
	hbmSECanalMask = define_hexside_type( "Canal Mask (SE)", "CanalMask_se" );

	// minor canals
	hbmWMinorCanal = define_hexside_type( "Minor Canal (W)", "CanalMinor_w" );
	hbmSWMinorCanal = define_hexside_type( "Minor Canal (SW)", "CanalMinor_sw" );
	hbmSEMinorCanal = define_hexside_type( "Minor Canal (SE)", "CanalMinor_se" );

	hbmWBdrMinCanal = define_hexside_type( "Int'l Bdr on Minor Canal (W)", "BorderBlack_CanalMinor_w" );
	hbmSWBdrMinCanal = define_hexside_type( "Int'l Bdr on Minor Canal (SW)", "BorderBlack_CanalMinor_sw" );
	hbmSEBdrMinCanal = define_hexside_type( "Int'l Bdr on Minor Canal (SE)", "BorderBlack_CanalMinor_se" );

	hbmWIBdrMinCanal = define_hexside_type( "Int. Bdr on Minor Canal (W)", "BorderGray_CanalMinor_w" );
	hbmSWIBdrMinCanal = define_hexside_type( "Int. Bdr on Minor Canal (SW)", "BorderGray_CanalMinor_sw" );
	hbmSEIBdrMinCanal = define_hexside_type( "Int. Bdr on Minor Canal (SE)", "BorderGray_CanalMinor_se" );

	hbmWISBdrMinCanal = define_hexside_type( "Gray Sub-Bdr on Minor Canal (W)", "BorderSubGray_CanalMinor_w" );
	hbmSWISBdrMinCanal = define_hexside_type( "Gray Sub-Bdr on Minor Canal (SW)", "BorderSubGray_CanalMinor_sw" );
	hbmSEISBdrMinCanal = define_hexside_type( "Gray Sub-Bdr on Minor Canal (SE)", "BorderSubGray_CanalMinor_se" );
#if 0
	hbmWIBBdrMinCanal = define_hexside_type( "Black Sub-Bdr on Minor Canal (W)", "BorderSubBlack_CanalMinor_w" );
	hbmSWIBBdrMinCanal = define_hexside_type( "Black Sub-Bdr on Minor Canal (SW)", "BorderSubBlack_CanalMinor_sw" );
	hbmSEIBBdrMinCanal = define_hexside_type( "Black Sub-Bdr on Minor Canal (SE)", "BorderSubBlack_CanalMinor_se" );
#endif
	// wadis
	hbmWWadiMask = define_hexside_type( "Wadi Mask (W)", "WadiMask_w" );
	hbmSWWadiMask = define_hexside_type( "Wadi Mask (SW)", "WadiMask_sw" );
	hbmSEWadiMask = define_hexside_type( "Wadi Mask (SE)", "WadiMask_se" );
	hbmWWadi = define_hexside_type( "Wadi (W)", "Wadi_w" );
	hbmSWWadi = define_hexside_type( "Wadi (SW)", "Wadi_sw" );
	hbmSEWadi = define_hexside_type( "Wadi (SE)", "Wadi_se" );

	hbmWBdrWadi = define_hexside_type( "Int'l Bdr on Wadi (W)", "BorderBlack_Wadi_w" );
	hbmSWBdrWadi = define_hexside_type( "Int'l Bdr on Wadi (SW)", "BorderBlack_Wadi_sw" );
	hbmSEBdrWadi = define_hexside_type( "Int'l Bdr on Wadi (SE)", "BorderBlack_Wadi_se" );

	hbmWIBdrWadi = define_hexside_type( "Int. Bdr on Wadi (W)", "BorderGray_Wadi_w" );
	hbmSWIBdrWadi = define_hexside_type( "Int. Bdr on Wadi (SW)", "BorderGray_Wadi_sw" );
	hbmSEIBdrWadi = define_hexside_type( "Int. Bdr on Wadi (SE)", "BorderGray_Wadi_se" );

	hbmWISBdrWadi = define_hexside_type( "Gray Sub-Bdr on Wadi (W)", "BorderSubGray_Wadi_w" );
	hbmSWISBdrWadi = define_hexside_type( "Gray Sub-Bdr on Wadi (SW)", "BorderSubGray_Wadi_sw" );
	hbmSEISBdrWadi = define_hexside_type( "Gray Sub-Bdr on Wadi (SE)", "BorderSubGray_Wadi_se" );
#if 0
	hbmWIBBdrWadi = define_hexside_type( "Black Sub-Bdr on Wadi (W)", "BorderSubBlack_Wadi_w" );
	hbmSWIBBdrWadi = define_hexside_type( "Black Sub-Bdr on Wadi (SW)", "BorderSubBlack_Wadi_sw" );
	hbmSEIBBdrWadi = define_hexside_type( "Black Sub-Bdr on Wadi (SE)", "BorderSubBlack_Wadi_se" );
#endif
	hbmWFillingReservoir = define_hexside_type( "Filling Reservoir (W)", "ReservoirFilling_w" );
	hbmSWFillingReservoir = define_hexside_type( "Filling Reservoir (SW)", "ReservoirFilling_sw" );
	hbmSEFillingReservoir = define_hexside_type( "Filling Reservoir (SE)", "ReservoirFilling_se" );

	hbmWKarst = define_hexside_type( "Karst (W)", "MtnKarst_w" );
	hbmSWKarst = define_hexside_type( "Karst (SW)", "MtnKarst_sw" );
	hbmSEKarst = define_hexside_type( "Karst (SE)", "MtnKarst_se" );

	hbmWGlacierMask = define_hexside_type( "Glacier Mask (W)", "GlacierMask_w" );
	hbmSWGlacierMask = define_hexside_type( "Glacier Mask (SW)", "GlacierMask_sw" );
	hbmSEGlacierMask = define_hexside_type( "Glacier Mask (SE)", "GlacierMask_se" );
	hbmWGlacier = define_hexside_type( "Glacier (W)", "Glacier_w" );
	hbmSWGlacier = define_hexside_type( "Glacier (SW)", "Glacier_sw" );
	hbmSEGlacier = define_hexside_type( "Glacier (SE)", "Glacier_se" );

	hbmWSaltDesertMask = define_hexside_type( "Salt Desert Mask (W)", "DesertSaltMask_w" );
	hbmSWSaltDesertMask = define_hexside_type( "Salt Desert Mask (SW)", "DesertSaltMask_sw" );
	hbmSESaltDesertMask = define_hexside_type( "Salt Desert Mask (SE)", "DesertSaltMask_se" );
	hbmWSaltDesert = define_hexside_type( "Salt Desert (W)", "DesertSalt_w" );
	hbmSWSaltDesert = define_hexside_type( "Salt Desert (SW)", "DesertSalt_sw" );
	hbmSESaltDesert = define_hexside_type( "Salt Desert (SE)", "DesertSalt_se" );

	hbmEscColor = define_hexside_type( "Escarpment (Color)", "EscColor" );
	hbmWDblEsc = define_hexside_type( "Double Escarpment (W)", "Escarpment_Double_w" );
	hbmSWDblEsc = define_hexside_type( "Double Escarpment (SW)", "Escarpment_Double_sw" );
	hbmSEDblEsc = define_hexside_type( "Double Escarpment (SE)", "Escarpment_Double_se" );

	hbmNWFort = define_hexside_type( "Fortified (NW)", "Fort_nw" );
	hbmNEFort = define_hexside_type( "Fortified (NE)", "Fort_ne" );
	hbmEFort = define_hexside_type( "Fortified (E)", "Fort_e" );
	hbmSEFort = define_hexside_type( "Fortified (SE)", "Fort_se" );
	hbmSWFort = define_hexside_type( "Fortified (SW)", "Fort_sw" );
	hbmWFort = define_hexside_type( "Fortified (W)", "Fort_w" );

	hbmNWGWall = define_hexside_type( "Great Wall (NW)", "GtWall_nw" );
	hbmNEGWall = define_hexside_type( "Great Wall (NE)", "GtWall_ne" );
	hbmEGWall = define_hexside_type( "Great Wall (E)", "GtWall_e" );
	hbmSEGWall = define_hexside_type( "Great Wall (SE)", "GtWall_se" );
	hbmSWGWall = define_hexside_type( "Great Wall (SW)", "GtWall_sw" );
	hbmWGWall = define_hexside_type( "Great Wall (W)", "GtWall_w" );

	hbmNWEsc = define_hexside_type( "Escarpment (NW)", "Escarpment_nw" );
	hbmNEEsc = define_hexside_type( "Escarpment (NE)", "Escarpment_ne" );
	hbmEEsc = define_hexside_type( "Escarpment (E)", "Escarpment_e" );
	hbmSWEsc = define_hexside_type( "Escarpment (SW)", "Escarpment_sw" );
	hbmSEEsc = define_hexside_type( "Escarpment (SE)", "Escarpment_se" );
	hbmWEsc = define_hexside_type( "Escarpment (W)", "Escarpment_w" );

	hbmNWMtnInterior = define_hexside_type( "Interior Mountain (NW)", "Mtn_Interior_nw" );
	hbmNEMtnInterior = define_hexside_type( "Interior Mountain (NE)", "Mtn_Interior_ne" );
	hbmEMtnInterior = define_hexside_type( "Interior Mountain (E)", "Mtn_Interior_e" );
	hbmSEMtnInterior = define_hexside_type( "Interior Mountain (SE)", "Mtn_Interior_se" );
	hbmSWMtnInterior = define_hexside_type( "Interior Mountain (SW)", "Mtn_Interior_sw" );
	hbmWMtnInterior = define_hexside_type( "Interior Mountain (W)", "Mtn_Interior_w" );

	hbmNWMtnInteriorMask = define_hexside_type( "Interior Mountain Mask (NW)", "Mtn_InteriorMask_nw" );
	hbmNEMtnInteriorMask = define_hexside_type( "Interior Mountain Mask (NE)", "Mtn_InteriorMask_ne" );
	hbmEMtnInteriorMask = define_hexside_type( "Interior Mountain Mask (E)", "Mtn_InteriorMask_e" );
	hbmSEMtnInteriorMask = define_hexside_type( "Interior Mountain Mask (SE)", "Mtn_InteriorMask_se" );
	hbmSWMtnInteriorMask = define_hexside_type( "Interior Mountain Mask (SW)", "Mtn_InteriorMask_sw" );
	hbmWMtnInteriorMask = define_hexside_type( "Interior Mountain Mask (W)", "Mtn_InteriorMask_w" );

	hbmWMask = define_hexside_type( "Water Mask (W)", "WaterMask_w" );
	hbmSWMask = define_hexside_type( "Water Mask (SW)", "WaterMask_sw" );
	hbmSEMask = define_hexside_type( "Water Mask (SE)", "WaterMask_se" );

	hbmWDecoMask = define_hexside_type( "Decorative Mask (W)", "DecoMask_w" );
	hbmSWDecoMask = define_hexside_type( "Decorative (SW)", "DecoMask_sw" );
	hbmSEDecoMask = define_hexside_type( "Decorative (SE)", "DecoMask_se" );

	hbmWRestrMask = define_hexside_type( "Restricted Waters Overlay (W)", "OverlayRestricted_w" );
	hbmSWRestrMask = define_hexside_type( "Restricted Waters Overlay (SW)", "OverlayRestricted_sw" );
	hbmSERestrMask = define_hexside_type( "Restricted Waters Overlay(SE)", "OverlayRestricted_se" );
	hbmERestrMask = define_hexside_type( "Restricted Waters Overlay (E)", "OverlayRestricted_e" );
	hbmNERestrMask = define_hexside_type( "Restricted Waters Overlay (NE)", "OverlayRestricted_ne" );
	hbmNWRestrMask = define_hexside_type( "Restricted Waters Overlay(NW)", "OverlayRestricted_nw" );

	hbmNRestrVertexMask = define_hexside_type( "Restricted Waters Vertex Overlay (N)", "OverlayVertex_n" );
	hbmSRestrVertexMask = define_hexside_type( "Restricted Waters Vertex Overlay (S)", "OverlayVertex_s" );
	hbmNWRestrVertexMask = define_hexside_type( "Restricted Waters Vertex Overlay (NW)", "OverlayVertex_nw" );
	hbmSWRestrVertexMask = define_hexside_type( "Restricted Waters Vertex Overlay (SW)", "OverlayVertex_sw" );
	hbmNERestrVertexMask = define_hexside_type( "Restricted Waters Vertex Overlay (NE)", "OverlayVertex_ne" );
	hbmSERestrVertexMask = define_hexside_type( "Restricted Waters Vertex Overlay (SE)", "OverlayVertex_se" );

	hbmWClear = define_hexside_type( "Clear (W, decor.)", "Clear_w" );
	hbmSWClear = define_hexside_type( "Clear (SW, decor.)", "Clear_sw" );
	hbmSEClear = define_hexside_type( "Clear (SE, decor.)", "Clear_se" );

	hbmWRough = define_hexside_type( "Rough (W, decor.)", "Rough_w" );
	hbmSWRough = define_hexside_type( "Rough (SW, decor.)", "Rough_sw" );
	hbmSERough = define_hexside_type( "Rough (SE, decor.)", "Rough_se" );

	hbmWWoods = define_hexside_type( "Woods (W, decor.)", "Woods_w" );
	hbmSWWoods = define_hexside_type( "Woods (SW, decor.)", "Woods_sw" );
	hbmSEWoods = define_hexside_type( "Woods (SE, decor.)", "Woods_se" );

	hbmWForest = define_hexside_type( "Forest (W, decor.)", "Forest_w" );
	hbmSWForest = define_hexside_type( "Forest (SW, decor.)", "Forest_sw" );
	hbmSEForest = define_hexside_type( "Forest (SE, decor.)", "Forest_se" );

	hbmWWoodedRough = define_hexside_type( "Wooded Rough (W, decor.)", "RoughWooded_w" );
	hbmSWWoodedRough = define_hexside_type( "Wooded Rough (SW, decor.)", "RoughWooded_sw" );
	hbmSEWoodedRough = define_hexside_type( "Wooded Rough (SE, decor.)", "RoughWooded_se" );

	// standalone bridges
	hbmWStandaloneBridge = define_hexside_type( "Bridge (W)", "Bridge_w" );
	hbmSWStandaloneBridge = define_hexside_type( "Bridge (SW)", "Bridge_sw" );
	hbmSEStandaloneBridge = define_hexside_type( "Bridge (SE)", "Bridge_se" );

	// load city and symbol graphics

	hbmCityBgnd = define_symbol( "City Background", "CityBkgnd" );
	hbmFullHexCity = define_symbol( "Full Hex (Great) City", "CityHexFull" );
	hbmPartialHexCity = define_symbol( "Partial Hex (Major) City", "CityHexPartial" );
	hbmMediumCity = define_symbol( "Dot (Medium) City", "CityDot" );
	hbmMinorCity = define_symbol( "Reference (Minor) City", "CityReference" );
	hbmPtCity = define_symbol( "Town", "CityPoint" );
	hbmRefPt = define_symbol( "Reference Point", "MtnRef" );
	hbmVolksDeutsch = define_symbol( "Volksdeutsch", "CityVolksD" );

	hbmOasis = define_symbol( "Oasis", "Oasis" );
	hbmOasisMask = define_symbol( "OasisMask", "OasisMask" );

	hbmWW1OFort = define_symbol( "Old Fortress (WW1)", "FortressOld" );
	hbmWW1NFort = define_symbol( "New Fortress (WW1)", "FortressNew" );
	hbmWW1GFort = define_symbol( "Great Fortress (WW1)", "FortressGreat" );
	hbmFortress = define_symbol( "Fortress", "FortressNew" ); // map-25, was "FortressStd"
	hbmOuvrage = define_symbol( "Ouvrage", "FortressOuvrage" );
	hbmWestWall = define_symbol( "Westwall", "FortressWestWall" );
	hbmFortMask = define_symbol( "Fortress Mask", "FortressMask" );

	hbmRMY1 = define_symbol( "Rail Marshaling Yard 1-cap", "RMY1" );
	hbmRMY2 = define_symbol( "Rail Marshaling Yard 2-cap", "RMY2" );
	hbmRMY3 = define_symbol( "Rail Marshaling Yard", "RMY3" );
	hbmWMD1 = define_symbol( "Waterway Marshaling Dock 1-cap", "WMD1" );
	hbmWMD2 = define_symbol( "Waterway Marshaling Dock 2-cap", "WMD2" );
	hbmWMD3 = define_symbol( "Waterway Marshaling Dock", "WMD3" );
	hbmWMDMask = define_symbol( "Waterway Marshaling Dock Mask", "WMDMask" );
	hbmShipyard = define_symbol( "Shipyard", "Shipyard" );
	hbmMajorNavalBase = define_symbol( "Major Naval Base", "NavalBaseMajor" );
	hbmMinorNavalBase = define_symbol( "Minor Naval Base", "NavalBaseMinor" );

	hbmLtdStack = define_symbol( "Limited Stacking Indicator", "LtdStack" );

	hbmHydroRes = define_symbol( "Hydroelectric RC", "RCHydroelectric" );
	hbmOreRes = define_symbol( "Ore RC", "RCOre" );
	hbmMineralRes = define_symbol( "Mineral RC", "RCMineral" );
	hbmCoalRes = define_symbol( "Coal RC", "RCCoal" );
	hbmNaturalGasRes = define_symbol( "Natural Gas RC", "RCNaturalGas" );
	hbmOilRes = define_symbol( "Oil RC", "RCOil" );
	hbmOilShaleRes = define_symbol( "Oil Shale RC", "RCOilShale" );
	hbmSynthOilRes = define_symbol( "Synth Oil RC", "RCSyntheticOil" );
	hbmPeatRes = define_symbol( "Peat RC", "RCPeat" );
	hbmCementRes = define_symbol( "Cement RC", "RCCement" );
	hbmRubberRes = define_symbol( "Rubber RC", "RCRubber" );
	hbmSynthRubberRes = define_symbol( "Synth Rubber RC", "RCSyntheticRubber" );
	hbmResCtrMask = define_symbol( "RC Mask", "RCMask" );
	hbmResCtrTextMask = define_symbol( "RC Text Mask", "RCTextMask" );

	// load port, airfield, and RMY graphics

	hbmGreatPort = define_symbol( "Great Port", "PortGreat" );
	hbmMajorPort = define_symbol( "Major Port", "PortMajor" );
	hbmStandardPort = define_symbol( "Standard Port", "PortStandard" );
	hbmMinorPort = define_symbol( "Minor Port", "PortMinor" );
	hbmMarginalPort = define_symbol( "Marginal Port", "PortMarginal" );
	hbmAnchorage = define_symbol( "Anchorage", "PortAnchorage" );
	hbmSquarePortMask = define_symbol( "Square Port Mask", "PortSquareMask" );
	hbmRoundPortMask = define_symbol( "Round Port Mask", "PortRoundMask" );

	hbmArtificialPort = define_symbol( "Artificial Port", "Port_Artificial" );
	hbmStrongPort = define_symbol( "Strong Port", "Port_Strong" );
	hbmPortIndicatorMask = define_symbol( "Port Indicator", "PortIndicatorMask" );

	hbmAirfield1 = define_symbol( "Intrinsic Airfield 1-cap", "Airfield1" );
	hbmAirfield3 = define_symbol( "Intrinsic Airfield 3-cap", "Airfield3" );
	hbmAirNavalBaseMask = define_symbol( "Air/Naval Base Mask", "AirNavalBaseMask" );

	hbmTribalArea = define_symbol( "Tribal Area", "Tribal2" );
	hbmTribalMask = define_symbol( "TribalMask", "TribalMask" );
	hbmBerberArea = define_symbol( "Berber Area", "Berber" );
	hbmBerberMask = define_symbol( "BerberMask", "BerberMask" );
	hbmCamelArea = define_symbol( "Camel Area", "Camel" );
	hbmCamelMask = define_symbol( "CamelMask", "CamelMask" );
	hbmTsetseArea = define_symbol( "Tsetse Area", "TsetseFly" );
	hbmTsetseMask = define_symbol( "TsetseMask", "TsetseFlyMask" );
	hbmIndianArea = define_symbol( "Indian Area", "Indian" );
	hbmIndianMask = define_symbol( "IndianMask", "IndianMask" );

	// init font
	//hfCityFont = const_cast<wxFont*>( wxSWISS_FONT );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

#undef  PATH
#if 0
#define PATH "graphics/map/"
	hbmHit = load_bmp( wxT(PATH"hit.bmp") );
	hbmWMinRivIce = load_bmp( wxT(PATH"w_minriver_ice.bmp") );
	hbmSWMinRivIce = load_bmp( wxT(PATH"sw_minriver_ice.bmp") );
	hbmSEMinRivIce = load_bmp( wxT(PATH"se_minriver_ice.bmp") );
	hbmWMajRivIce = load_bmp( wxT(PATH"w_majriver_ice.bmp") );
	hbmSWMajRivIce = load_bmp( wxT(PATH"sw_majriver_ice.bmp") );
	hbmSEMajRivIce = load_bmp( wxT(PATH"se_majriver_ice.bmp") );
	hbmHexMask = load_bmp( wxT(PATH"hexmask.bmp") );
#undef  PATH
#endif

#define PATH "graphics/images/"
	// EJW: PNG test
	// Ownership roundels
	hbmAxis = load_bmp( wxT(PATH"RoundelGerman.png") ); // washed set out to dominate less
	hbmAlliedSov = load_bmp( wxT(PATH"RoundelSoviet.png") );
	hbmAlliedUS = load_bmp( wxT(PATH"RoundelAmerican.png") );
	hbmAlliedBr = load_bmp( wxT(PATH"RoundelBritish.png") );
	hbmAllied = hbmAlliedSov;  // default
	// Fortifications
	hbmFort = load_bmp( wxT(PATH"MapFort.png") ); // works nicely but might be too subtle
	hbmEntrenchent = load_bmp( wxT(PATH"MapEntrench.png") ); // probably blitted, png is black TODO: hbm spelled wrong
	hbmImpFort = load_bmp( wxT(PATH"MapFortImp.png") ); // black/gray, no counter, displace from center?
	hbmFieldWorks = load_bmp( wxT(PATH"MapFieldWorks.png") ); // brown/reddish brown, no counter, displace from center?
	// Airfields
	hbmTempAF = load_bmp( wxT(PATH"AirfieldTemp.png") );
	hbmPermAF = load_bmp( wxT(PATH"AirfieldPerm.png") );
	hbmZepBase = load_bmp( wxT(PATH"AirfieldZep.png") );
	// Unit size symbols
	hbmCompany = load_bmp( wxT(PATH"company.bmp") ); // due to XOR blitting, transparency backfires
	hbmBatallion = load_bmp( wxT(PATH"battalion.bmp") );
	hbmBatallionGrp = load_bmp( wxT(PATH"battalion_grp.bmp") );
	hbmRegiment = load_bmp( wxT(PATH"regiment.bmp") );
	hbmRegimentGrp = load_bmp( wxT(PATH"regiment_grp.bmp") );
	hbmBrigade = load_bmp( wxT(PATH"brigade.bmp") );
	hbmBrigadeGrp = load_bmp( wxT(PATH"brigade_grp.bmp") );
	hbmDivisional = load_bmp( wxT(PATH"divisional.bmp") );
	hbmDivision = load_bmp( wxT(PATH"division.bmp") );
	hbmCorps = load_bmp( wxT(PATH"corps.bmp") );
	hbmArmy = load_bmp( wxT(PATH"army.bmp") );
	hbmCadre = load_bmp( wxT(PATH"cadre.bmp") );
	hbmDG = load_bmp( wxT(PATH"divisiongruppe.bmp") );
	hbm1RE = load_bmp( wxT(PATH"1re.bmp") );
	hbm2RE = load_bmp( wxT(PATH"2re.bmp") );
	hbm3RE = load_bmp( wxT(PATH"3re.bmp") );
	// Hit markers
	hbmHit = load_bmp( wxT(PATH"BridgeHit.png") ); // transparent edges
	hbmDestroyed = load_bmp( wxT(PATH"destroyed.bmp") ); // only used to represent destroyed ports
	hbmSmallHit = load_bmp( wxT(PATH"HitSmallOrange.png") ); // RR, port, CD, harassment hits for unit list
	hbmCDHit = load_bmp( wxT(PATH"HitCD.bmp") );	// no advantage for a png
	hbmHarPlane = load_bmp( wxT(PATH"PlaneHar.png") ); // changed to a scaled down MC202, though it could be almost anything
	hbmAxMine = load_bmp( wxT(PATH"axmine.bmp") ); // can't test until reenabled
	hbmAlMine = load_bmp( wxT(PATH"almine.bmp") );
	hbmSmallPort = define_symbol( "Port Anchorage", "portanchorage" ); // Used to place a small port symbol on the unit list display for a port hit
	hbmMulberry = load_bmp( wxT(PATH"mulberry.bmp") ); // png appears entirely black
	// Targets
	hbmTarget = load_bmp( wxT(PATH"target.png") ); // transparent edges
	hbmAirTarget = load_bmp( wxT(PATH"airtarget.bmp") ); // png is too hard to see
	// Notes EJW: no obvious room for improvement
	hbmInfoBox = load_bmp( wxT(PATH"infobox.bmp") ); // no change
	hbmNote = load_bmp( wxT(PATH"note.bmp") );
	hbmCombatReport = load_bmp( wxT(PATH"gndreport.bmp") );
	hbmBombReport = load_bmp( wxT(PATH"bombreport.bmp") );
	hbmDieRollReport = load_bmp( wxT(PATH"dierollreport.bmp") );
	hbmAirCombatReport = load_bmp( wxT(PATH"airreport.bmp") );
	hbmNavalCombatReport = load_bmp( wxT(PATH"navreport.bmp") );
	hbmMapNote = load_bmp( wxT(PATH"mapnote.bmp") );
	hbmMapRP = load_bmp( wxT(PATH"maprp.bmp") );
	hbmMapSP = load_bmp( wxT(PATH"mapsp.bmp") );
	hbmMapHit = load_bmp( wxT(PATH"maphit.bmp") );
	hbmMapHitLite = load_bmp( wxT(PATH"maphitoutline.bmp") );
	hbmAlert = load_bmp( wxT(PATH"mapalert.bmp") );
	// Supply, Isolation & Resource Points
	hbmNoSupply = load_bmp( wxT(PATH"nosupply.bmp") );
	hbmAxNoSupply = load_bmp( wxT(PATH"axnosupply.bmp") );
	hbmSpcSupply = load_bmp( wxT(PATH"spcsupply.bmp") );
	hbmAxSpcSupply = load_bmp( wxT(PATH"axisspcsupply.bmp") );
	hbmIsolated = load_bmp( wxT(PATH"isolated.bmp") );
	hbmAxIsolated = load_bmp( wxT(PATH"axisisolated.bmp") );
	hbmSupplyDepot = load_bmp( wxT(PATH"supplydepot.bmp") );
	hbmRP = load_bmp( wxT(PATH"rp.bmp") );
	// Misc
	hbmAirUnit = load_bmp( wxT(PATH"airunit.bmp") ); // function unknown
	hbmHexMask = load_bmp( wxT(PATH"hexmask.png") );
	hbmUnitBgnd = load_bmp( wxT(PATH"unitbgnd.bmp") );
	hbmContested = load_bmp( wxT(PATH"contest.bmp") );
	hbmBalloons = load_bmp( wxT(PATH"bal_barrage.bmp") );
#undef  PATH

#define PATH "graphics/units/"
	hbmWarShip = load_bmp( wxT(PATH"warship.bmp") );
	hbmCargoShip = load_bmp( wxT(PATH"cargoship.bmp") );
	hbmCruiser = load_bmp( wxT(PATH"cruiser.bmp") );
	hbmDestroyer = load_bmp( wxT(PATH"destroyer.bmp") );
	hbmAuxShip = load_bmp( wxT(PATH"auxship.bmp") );
	hbmLC = load_bmp( wxT(PATH"lc.bmp") );
	hbmSubmarine = load_bmp( wxT(PATH"submarine.bmp") );
	hbmCarrier = load_bmp( wxT(PATH"carrier.bmp") );

	hbmArmCorps = load_bmp( wxT(PATH"armcorps.bmp") );
	hbmInfCorps = load_bmp( wxT(PATH"infcorps.bmp") );
	hbmMecCorps = load_bmp( wxT(PATH"meccorps.bmp") );
	hbmMxdCorps = load_bmp( wxT(PATH"mxdcorps.bmp") );

	hbmMtnMask = load_bmp( wxT(PATH"mtnmask.bmp") );
#undef  PATH

	// 4x7 font graphics
	char d_0_bits[] =
	{ 0xf6, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf6 };
	char d_1_bits[] =
	{ 0xfb, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xff };  // 3x7
	char d_2_bits[] =
	{ 0xf6, 0xf9, 0xf8, 0xf4, 0xf2, 0xf1, 0xff };
	char d_3_bits[] =
	{ 0xf7, 0xf8, 0xf4, 0xf6, 0xf8, 0xf8, 0xf7 };
	char d_4_bits[] =
	{ 0xf4, 0xf6, 0xf6, 0xf5, 0xff, 0xf4, 0xf4 };
	char d_5_bits[] =
	{ 0xff, 0xf1, 0xf7, 0xf8, 0xf8, 0xf8, 0xf7 };
	char d_6_bits[] =
	{ 0xfc, 0xf2, 0xf7, 0xf9, 0xf9, 0xf9, 0xf6 };
	char d_7_bits[] =
	{ 0xff, 0xf8, 0xf4, 0xf4, 0xf4, 0xf2, 0xf2 };
	char d_8_bits[] =
	{ 0xf6, 0xf9, 0xf9, 0xf6, 0xf9, 0xf9, 0xf6 };
	char d_9_bits[] =
	{ 0xf6, 0xf9, 0xf9, 0xf9, 0xfe, 0xf4, 0xf3 };
	char d_minus_bits[] =
	{ 0xfc, 0xfc, 0xfc, 0xff, 0xfc, 0xfc, 0xfc };  // 2x7

	// TODO: digits indented by 1 space are not done!!!
	char d_A_bits[] =
	{ 0x04, 0x04, 0x0a, 0x0a, 0x0e, 0x11, 0x11 };  // 5x7
	char d_B_bits[] =
	{ 0x0f, 0x11, 0x11, 0x0f, 0x11, 0x11, 0x0f };  // 5x7
	char d_C_bits[] =
	{ 0x0e, 0x01, 0x01, 0x01, 0x01, 0x01, 0x0e };  // 4x7
	char d_D_bits[] =
	{ 0x0f, 0x12, 0x12, 0x12, 0x12, 0x12, 0x0f };  // 5x7
	char d_E_bits[] =
	{ 0x0f, 0x01, 0x01, 0x0f, 0x01, 0x01, 0x0f };  // 4x7
	char d_F_bits[] =
	{ 0x0f, 0x01, 0x01, 0x0f, 0x01, 0x01, 0x01 };  // 4x7
	char d_G_bits[] =
	{ 0x0e, 0x01, 0x01, 0x19, 0x11, 0x11, 0x0e };  // 5x7
	char d_H_bits[] =
	{ 0x09, 0x09, 0x09, 0x0f, 0x09, 0x09, 0x09 };  // 4x7
	char d_I_bits[] =
	{ 0x07, 0x02, 0x02, 0x02, 0x02, 0x02, 0x07 };  // 4x7
	char d_J_bits[] =
	{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x06 };  // 4x7
	//char d_K_bits[] = {0x11,0x11,0x11,0x11,0x11,0x11,0x0e};     // 5x7
	char d_L_bits[] =
	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x0f };  // 4x7
	char d_M_bits[] =
	{ 0x11, 0x1b, 0x1b, 0x15, 0x15, 0x11, 0x11 };  // 5x7
	char d_N_bits[] =
	{ 0x11, 0x13, 0x13, 0x15, 0x19, 0x19, 0x11 };  // 5x7
	// char d_O_bits[] = {0x11,0x11,0x11,0x11,0x11,0x11,0x0e};     // 5x7
	char d_P_bits[] =
	{ 0x0f, 0x11, 0x11, 0x0f, 0x01, 0x01, 0x01 };  // 5x7
	//char d_Q_bits[] = {0x11,0x11,0x11,0x11,0x11,0x11,0x0e};     // 5x7
	char d_R_bits[] =
	{ 0x0f, 0x12, 0x12, 0x0e, 0x0a, 0x12, 0x37 };  // 6x7
	char d_S_bits[] =
	{ 0x0e, 0x11, 0x01, 0x0e, 0x10, 0x11, 0x0e };  // 5x7
	char d_T_bits[] =
	{ 0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 };  // 5x7
	char d_U_bits[] =
	{ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e };  // 5x7
	char d_V_bits[] =
	{ 0x11, 0x11, 0x0a, 0x0a, 0x0a, 0x04, 0x04 };  // 5x7
	char d_W_bits[] =
	{ 0x11, 0x11, 0x15, 0x15, 0x1b, 0x1b, 0x11 };  // 5x7
	char d_X_bits[] =
	{ 0x11, 0x0a, 0x0a, 0x04, 0x0a, 0x0a, 0x11 };  // 5x7
	//char d_Y_bits[] = {0x11,0x11,0x11,0x11,0x11,0x11,0x0e};     // 5x7
	char d_Z_bits[] =
	{ 0x0f, 0x08, 0x04, 0x06, 0x02, 0x01, 0x0f };  // 4x7

	hbmFont[0] = new wxBitmap( d_0_bits, 4, 7 );
	hbmFont[1] = new wxBitmap( d_1_bits, 3, 7 );
	hbmFont[2] = new wxBitmap( d_2_bits, 4, 7 );
	hbmFont[3] = new wxBitmap( d_3_bits, 4, 7 );
	hbmFont[4] = new wxBitmap( d_4_bits, 4, 7 );
	hbmFont[5] = new wxBitmap( d_5_bits, 4, 7 );
	hbmFont[6] = new wxBitmap( d_6_bits, 4, 7 );
	hbmFont[7] = new wxBitmap( d_7_bits, 4, 7 );
	hbmFont[8] = new wxBitmap( d_8_bits, 4, 7 );
	hbmFont[9] = new wxBitmap( d_9_bits, 4, 7 );
	hbmFont[10] = new wxBitmap( d_minus_bits, 2, 7 );
	hbmFont[(int)'A'] = new wxBitmap( d_A_bits, 5, 7 );
	hbmFont[(int)'B'] = new wxBitmap( d_B_bits, 5, 7 );
	hbmFont[(int)'C'] = new wxBitmap( d_C_bits, 4, 7 );
	hbmFont[(int)'D'] = new wxBitmap( d_D_bits, 5, 7 );
	hbmFont[(int)'E'] = new wxBitmap( d_E_bits, 4, 7 );
	hbmFont[(int)'F'] = new wxBitmap( d_F_bits, 4, 7 );
	hbmFont[(int)'G'] = new wxBitmap( d_G_bits, 5, 7 );
	hbmFont[(int)'H'] = new wxBitmap( d_H_bits, 4, 7 );
	hbmFont[(int)'I'] = new wxBitmap( d_I_bits, 4, 7 );
	hbmFont[(int)'J'] = new wxBitmap( d_J_bits, 4, 7 );
	hbmFont[(int)'K'] = hbmFont[10];  //new wxBitmap(d_K_bits, 5, 7);
	hbmFont[(int)'L'] = new wxBitmap( d_L_bits, 4, 7 );
	hbmFont[(int)'M'] = new wxBitmap( d_M_bits, 5, 7 );
	hbmFont[(int)'N'] = new wxBitmap( d_N_bits, 5, 7 );
	hbmFont[(int)'O'] = hbmFont[10];  //new wxBitmap(d_O_bits, 5, 7);
	hbmFont[(int)'P'] = new wxBitmap( d_P_bits, 5, 7 );
	hbmFont[(int)'Q'] = hbmFont[10];  //new wxBitmap(d_Q_bits, 5, 7);
	hbmFont[(int)'R'] = new wxBitmap( d_R_bits, 6, 7 );
	hbmFont[(int)'S'] = new wxBitmap( d_S_bits, 5, 7 );
	hbmFont[(int)'T'] = new wxBitmap( d_T_bits, 5, 7 );
	hbmFont[(int)'U'] = new wxBitmap( d_U_bits, 5, 7 );
	hbmFont[(int)'V'] = new wxBitmap( d_V_bits, 5, 7 );
	hbmFont[(int)'W'] = new wxBitmap( d_W_bits, 5, 7 );
	hbmFont[(int)'X'] = new wxBitmap( d_X_bits, 5, 7 );
	hbmFont[(int)'Y'] = hbmFont[10];  //new wxBitmap(d_Y_bits, 5, 7);
	hbmFont[(int)'Z'] = new wxBitmap( d_Z_bits, 4, 7 );

	chk_bmp( hbmFont[0], "Font character '0'" );
	chk_bmp( hbmFont[1], "Font character '1'" );
	chk_bmp( hbmFont[2], "Font character '2'" );
	chk_bmp( hbmFont[3], "Font character '3'" );
	chk_bmp( hbmFont[4], "Font character '4'" );
	chk_bmp( hbmFont[5], "Font character '5'" );
	chk_bmp( hbmFont[6], "Font character '6'" );
	chk_bmp( hbmFont[7], "Font character '7'" );
	chk_bmp( hbmFont[8], "Font character '8'" );
	chk_bmp( hbmFont[9], "Font character '9'" );
	chk_bmp( hbmFont[10], "Font character '-'" );

	// replay path pens
	if ( ( tac_pen = new wxPen( wxColour( 0, 0, 140 ), 3, wxSOLID ) ) == nullptr ) // blue
	{
		wxMessageBox( wxT("Couldn't create pen for replay"), Application::NAME );
		exit( -1 );
	}
	if ( ( combat_pen = new wxPen( wxColour( 140, 0, 0 ), 3, wxSOLID ) ) == nullptr ) // red
	{
		wxMessageBox( wxT("Couldn't create pen for replay"), Application::NAME );
		exit( -1 );
	}
	if ( ( expl_pen = new wxPen( wxColour( 0, 140, 0 ), 3, wxSOLID ) ) == nullptr ) // green
	{
		wxMessageBox( wxT("Couldn't create pen for replay"), Application::NAME );
		exit( -1 );
	}

	// set up terrain type table
	set_TEC_name( Application::TEC_FILENAME.c_str( ) );
	initialize_TEC( wxStandardPaths::Get( ).GetExecutablePath( ).c_str( ) ); // tabular folder

	// set up unit type tables
	set_UIC_name( Application::UIC1_FILENAME.c_str( ) );
	initialize_UIC( wxStandardPaths::Get( ).GetExecutablePath( ).c_str( ) ); // tabular folder

	set_UIC_name( Application::UIC2_FILENAME.c_str( ) );
	initialize_UIC( wxStandardPaths::Get( ).GetExecutablePath( ).c_str( ) ); // tabular folder

	// (any AEC/ATEC exceptions are implemented in GroundUnit::GetAECA() etc.)

#if 0
// unit classes (set in the  *.uic files)
#define UC_NONMOT    1  // non-motorized combat unit (TODO: not used in code?)
#define UC_COMBATMOT 2  // intrinsically motorized, e.g. armor, duplicated by UTF_MOTORIZED unit type flag
#define UC_ARTILLERY 3  // fights as art, duplicated by UTF_ARTILLERY unit type flag
#define UC_CAVALRY   4  // includes qualifying bicycle units (?)
#define UT( ut, name, abbrev, bmpfile, movtyp, typ, aeca, aecd, atec, flags ) \
do { \
  strcpy( _ground_unit_type[ut].name_, name ); \
  strcpy( _ground_unit_type[ut].shortname_, abbrev ); \
  _ground_unit_type[ut].movement_category_ = movtyp; \
  _ground_unit_type[ut].type_ = typ; \
  _ground_unit_type[ut].AECA_value_ = aeca; \
  _ground_unit_type[ut].AECD_value_ = aecd; \
  _ground_unit_type[ut].ATEC_value_ = atec; \
  _ground_unit_type[ut].flags_ = flags; \
  _ground_unit_type[ut].hbmBitmap = load_bmp(bmpfile); \
} while ( 0 )

#undef	PATH
#define	PATH "graphics/units/"
#include "ww2.uic"

#undef	WW1PATH
#define	WW1PATH "graphics/units/ww1/"
#include "ww1.uic"
#undef WW1PATH

#undef PATH
#undef UT
#endif
	// common colors:
#define C(r,g,b)		wxColour(r,g,b)
#define WHITE			(*wxWHITE)
#define BLACK			(*wxBLACK)

#define GER_GREEN		C(149,162,116)
#define ITA_GREEN		C(201,211,176)
#define FRBLUE			C(79,148,205)  // french blue
#define SFRED			C(205,51,51)   // second front red
#define GOLDENBROWN		C(193,129,57)
#define LNAVY			C(144,159,198) // light navy
#define DNAVY			C(46,70,150)   // dark navy
#define WW1_GER_GRAY	C(83,83,83)
#define WW1_GER_LT_GRAY	C(123,123,123)  // just guessing
#define OLIVE			C(88,89,57)         // US
#define BBGRAY			C(80,80,80)
#define LTGRAY			C(153,153,153)
#define DKBROWN			C(139,90,43) // or C(139,115,85)
#define BROWN			C(139,137,112)
#define CLYEL			C(249,230,132) // WoR Chinese light yellow bgnd
#define CYEL			C(249,229,72)  // WoR Chinese yellow bgnd
#define CDYEL			C(240,220,50)  // WoR Chinese dark yellow bgnd

// ground unit definitions

	// first, initialize (zero) unit graphics
	for ( int i = SidePlayer::AXIS; i <= SidePlayer::ALLIED; ++i ) // 0..1
		for ( int j = 0; j < GroundArmedForce::Allied::COUNT; ++j ) // 0..110
			for ( int k = GroundUnitType::WW2Corps::INF; k <= GroundUnitType::WW2Corps::TNK; ++k ) // 0..3
				_ground_armed_force[i][j].hbm[k] = nullptr;

#if 0
		SubTypes[AXIS][i].hbm[CC_MXD] = 0;
		SubTypes[AXIS][i].hbm[CC_MEC] = 0;
		SubTypes[AXIS][i].hbm[CC_TNK] = 0;
		SubTypes[ALLIED][i].hbm[CC_MXD] = 0;
		SubTypes[ALLIED][i].hbm[CC_MEC] = 0;
		SubTypes[ALLIED][i].hbm[CC_TNK] = 0;
#endif

#undef	UC
#define	UC(side, type, id_str, long_str, fgnd, text, bgnd, g) \
do { \
	strcpy( _ground_armed_force[side][type].name_, id_str ); \
	strcpy( _ground_armed_force[side][type].longname_, long_str ); \
	_ground_armed_force[side][type].crFg = fgnd; \
	_ground_armed_force[side][type].crText = text; \
	_ground_armed_force[side][type].crBg = bgnd; \
	_ground_armed_force[side][type].games_ = g; \
} while ( 0 )
#if 0
	InitCCGraphics(	SubTypes[side][type].hbm, \
					SubTypes[side][type].crFg, \
					SubTypes[side][type].crBg );
#endif

#include "groundunit.ucc"
#undef	UC

// air unit definitions

	// first, initialize (zero) air unit graphics
	for ( int i = SidePlayer::AXIS; i <= SidePlayer::ALLIED; ++i ) // 0..1
		for ( int j = 0; j < AirArmedForce::Allied::COUNT; ++j ) // 0..48
			for ( int k = AirUnit::Status::ACTIVE; k <= AirUnit::Status::INACTIVE; ++k ) // 0..1
				_air_armed_force[i][j].hbm[k] = nullptr;

#if 0
	for ( j = 0; j < AIRSUBTYPECOUNT; ++j )   // initialize all to zero
	{
		AirSubTypes[SidePlayer::AXIS][j].hbm[AIR_ACTIVE] = nullptr;
		AirSubTypes[SidePlayer::AXIS][j].hbm[AIR_INACTIVE]= nullptr;
		AirSubTypes[SidePlayer::ALLIED][j].hbm[AIR_ACTIVE] = nullptr;
		AirSubTypes[SidePlayer::ALLIED][j].hbm[AIR_INACTIVE] = nullptr;
	}
#endif

#define	UC(side, type, id_str, fgnd, fgnd2, bgnd, g) \
do { \
	strcpy( _air_armed_force[side][type].name_, id_str ); \
	_air_armed_force[side][type].crFg = fgnd; \
	_air_armed_force[side][type].crFg2 = fgnd2; \
	_air_armed_force[side][type].crBg = bgnd; \
	_air_armed_force[side][type].games_ = g; \
	initializeAirGraphics(	_air_armed_force[side][type].hbm, \
							_air_armed_force[side][type].crFg, \
							_air_armed_force[side][type].crBg ); \
} while ( 0 )
#include "airunit.ucc"
#undef	UC

	// TODO: vet if this initialize is okay (added by DTM 12apr2014)
	// first, initialize (zero) air unit graphics
	for ( int i = SidePlayer::AXIS; i <= SidePlayer::ALLIED; ++i ) // 0..1
		for ( int j = 0; j < NavalArmedForce::Allied::COUNT; ++j ) // 0..40
			_naval_armed_force[i][j].hbm = nullptr;

// naval unit definitions
#define	UC(side, type, id_str, fgnd, bgnd, g) \
do { \
	strcpy( _naval_armed_force[side][type].name_, id_str ); \
	_naval_armed_force[side][type].crFg = fgnd; \
	_naval_armed_force[side][type].crBg = bgnd; \
	_naval_armed_force[side][type].games_ = g; \
	_naval_armed_force[side][type].hbm = initializeNavalGraphics(	_naval_armed_force[side][type].hbm, \
																	_naval_armed_force[side][type].crFg, \
																	_naval_armed_force[side][type].crBg ); \
	} while ( 0 )
#include "navalunit.ucc"
#undef	UC

	unit_mover_dialog_ = nullptr;
	view_text_dialog_ = nullptr;
	drawing_path_ = FALSE;

	// initial world setup
	// TODO: this overrides the HEXTOOLSMAP-only settings at the top of this ctor
	world_x_size_ = 40;
	world_y_size_ = 40;

	// initialize (zero) losses counters
	for ( int y = 0; y < 100; ++y ) // year 1900 to 1999 ??
		for ( int m = 0; m < 12; ++m )
			for ( int t = 0; t < 2; ++t ) // two player turns
				unisol_losses_[y][m][t][SidePlayer::AXIS] = isol_losses_[y][m][t][SidePlayer::AXIS] = unisol_losses_[y][m][t][SidePlayer::ALLIED] = isol_losses_[y][m][t][SidePlayer::ALLIED] = 0;
#endif

	reserve_map_storage( world_y_size_, world_x_size_ );

#if 0
	// test: guts of HT-m's MapPane::intializeMap( ):
	for ( int y = 0; y < iYWorldSize; ++y )
		for ( int x = 0; x < iXWorldSize; ++x )
			if ( hex[y][x].CityPtr != nullptr )
			{
				// TODO: naked delete, and does this null multiple cities per hex?
				delete hex[y][x].CityPtr;
				hex[y][x].CityPtr = nullptr;
			}
#endif

#if defined HEXTOOLSMAP
	initialize_map( );
	selected_weather_zone_ = 3; // WZ 'D'
	selected_sea_district_ = 256;
	selected_land_district_ = 256;
	strcpy( current_mapid_, "0" );
	clear_all_modes( );
	clear_all_rotated_mapsheets( );
	clear_all_right_offset_mapsheets( );

	show_hexid_ = false;
#elif defined HEXTOOLSPLAY
	reserve_sit_storage( world_y_size_, world_x_size_ );

	sea_zone_mode_ = false;
	selected_sea_zone_ = 32;	// 32 == NONE
	country_mode_ = false;
	selected_country_ = 8;		// 8 == NONE
#endif

	scale_ = 1.0;

	// flags for .map in HT-m and .scn in HT-p
	filename_is_defined_ = false;
	file_is_dirty_ = false;
}

MapPane::~MapPane( )
{
	// TODO: why doesn't HT-p's dtor for MapPane also free map memory?
#if defined HEXTOOLSMAP
	release_map_storage( world_y_size_, world_x_size_ );
#endif
}

// private helpers for ctor
// TODO: consider inline for these three
void MapPane::define_hex_type( int index, const char* name, const char* image_filename )
{
	wxString bitmapPathname( Application::HEX_BITMAPS_PATH );
	bitmapPathname += image_filename;
	bitmapPathname += ".bmp";
	_hex_types[index].name_ = name;
	_hex_types[index].hbmBitmap = new wxBitmap( bitmapPathname, wxBITMAP_TYPE_BMP );
	chk_bmp( _hex_types[index].hbmBitmap, _hex_types[index].name_ );
}

wxBitmap* MapPane::define_hexside_type( const char* WXUNUSED(name), const char* image_filename )
{
	wxString bitmapPathname( Application::HEXSIDE_BITMAPS_PATH );
	bitmapPathname += image_filename;
	bitmapPathname += ".bmp";
	wxBitmap* bmp = new wxBitmap( bitmapPathname, wxBITMAP_TYPE_BMP );
	chk_bmp( bmp, image_filename );
	return bmp;
}

wxBitmap* MapPane::define_symbol( const char* WXUNUSED(name), const char* image_filename )
{
	wxString bitmapPathname( Application::SYMBOL_BITMAPS_PATH );
	bitmapPathname += image_filename;
	bitmapPathname += ".bmp";
	wxBitmap* bmp = new wxBitmap( bitmapPathname, wxBITMAP_TYPE_BMP );
	chk_bmp( bmp, image_filename );
	return bmp;
}

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

// private helper draw_active_air(..) and draw_ship(..) and ctor
static void draw_counter( wxDC* hdc )
{
	//hdc->SetBrush( *wxWHITE_BRUSH );
	hdc->DrawRectangle( 0, 0, _counter_width, _counter_height );
}

// private helper initializeAirGraphics(..) and ctor
static void draw_active_air( wxDC* hdc )
{
	int l = 1;
	const int mid = _counter_width / 2;

	draw_counter( hdc );
	// nose
	hdc->DrawLine( mid - 2, l, mid + 2, l );
	++l;
	hdc->DrawLine( mid - 2, l, mid + 2, l );
	++l;
	// wing
	hdc->DrawLine( mid - 8, l, mid + 8, l );
	++l;
	hdc->DrawLine( mid - 8, l, mid + 8, l );
	++l;
	hdc->DrawLine( mid - 7, l, mid + 7, l );
	++l;
	// hull
	hdc->DrawLine( mid - 2, l, mid + 2, l );
	++l;
	hdc->DrawLine( mid - 2, l, mid + 2, l );
	++l;
	hdc->DrawLine( mid - 2, l, mid + 2, l );
	++l;
	hdc->DrawLine( mid - 2, l, mid + 2, l );
	++l;
	// tail
	hdc->DrawLine( mid - 4, l, mid + 4, l );
	++l;
	hdc->DrawLine( mid - 5, l, mid + 5, l );
}

#if 0
static void draw_inactive_air( wxDC* hdc )
{
	//draw_counter( hdc );
	draw_active_air( hdc );  // TODO
}
#endif

// private helper initializeNavalGraphics(..) and ctor
static void draw_ship( wxDC* hdc )
{
	int l = 1;
	const int mid = _counter_width / 2;
	draw_counter( hdc );
	// ship's mast
	hdc->DrawLine( mid - 1, l, mid, l );
	++l;
	hdc->DrawLine( mid - 1, l, mid, l );
	++l;
	hdc->DrawLine( mid - 1, l, mid, l );
	++l;
	hdc->DrawLine( mid - 1, l, mid, l );
	hdc->DrawLine( mid + 3, l, mid + 4, l );
	++l;

	hdc->DrawLine( mid - 3, l, mid - 2, l );
	hdc->DrawLine( mid - 1, l, mid + 1, l );
	hdc->DrawLine( mid + 3, l, mid + 5, l );
	++l;

	hdc->DrawLine( mid - 7, l, mid - 4, l );
	hdc->DrawLine( mid - 2, l, mid + 1, l );
	hdc->DrawLine( mid + 3, l, mid + 5, l );
	++l;

	hdc->DrawLine( mid - 6, l, mid - 4, l );
	hdc->DrawLine( mid - 2, l, mid + 6, l );
	++l;

	hdc->DrawLine( mid - 9, l, mid + 10, l );
	++l;
	hdc->DrawLine( mid - 8, l, mid + 10, l );
	++l;
	hdc->DrawLine( mid - 7, l, mid + 7, l );
	++l;
}

// private helper for ctor
// init air group graphics
void MapPane::initializeAirGraphics( wxBitmap* hbms[2], wxColour fgnd, wxColour bgnd )
{
	wxMemoryDC hdc;
	wxPen fg_pen( wxColour( 255, 255, 255 ), 1, wxSOLID );
	wxBrush bg_brush( wxColour( 0, 0, 0 ), wxSOLID );

	if ( ( hbms[AirUnit::Status::ACTIVE] = new wxBitmap( _counter_width, _counter_height ) ) == nullptr )
		wxMessageBox( wxT("Failed to create active air unit bitmap!"), Application::NAME );
#if 0
	if ( ( hbms[AirUnit::Status::INACTIVE] = new wxBitmap( UBM_W, UBM_H ) ) == nullptr )
		wxMessageBox("Failed to create inactive air unit bitmap!", MSGHDR );
#endif
	fg_pen.SetColour( fgnd );
	bg_brush.SetColour( bgnd );
	hdc.SelectObject( *hbms[AirUnit::Status::ACTIVE] );
	hdc.SetPen( fg_pen );
	hdc.SetBrush( bg_brush );
	draw_active_air( &hdc );
#if 0
	hdc.SelectObject( *hbms[AirUnit::Status::INACTIVE] );
	draw_inactive_air( &hdc );
#endif
}

// private helper for ctor
// init air group graphics
wxBitmap* MapPane::initializeNavalGraphics( wxBitmap* hbm, wxColour fgnd, wxColour bgnd )
{
	wxMemoryDC hdc;
	wxPen fg_pen( wxColour( 255, 255, 255 ), 1, wxSOLID );
	wxBrush bg_brush( wxColour( 0, 0, 0 ), wxSOLID );

	if ( ( hbm = new wxBitmap( _counter_width, _counter_height ) ) == nullptr )
		wxMessageBox( wxT("Failed to create ship bitmap!"), Application::NAME );

	// set colors here
	fg_pen.SetColour( fgnd );
	bg_brush.SetColour( bgnd );
	hdc.SelectObject( *hbm );
	hdc.SetPen( fg_pen );
	hdc.SetBrush( bg_brush );
	draw_ship( &hdc );
	return hbm;
}

#endif
