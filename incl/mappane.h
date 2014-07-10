#ifndef MAPPANE_H
#define MAPPANE_H

class City;
class Hex;
class Frame;
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#if ! defined GROUNDUNITTYPE_INCLUDE
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#define GROUNDUNITTYPE_INCLUDE
#endif
class HexContents;
class Counter;
class GroundUnit;
class NavalUnit;
class HexContentsPane;
class HexInfoPane;
class DUnitMover;
class Views;
class DView;
#endif // HEXTOOLSPLAY forward class declarations

/**
decomposition of MapPane class into files (in alpha
order, except for mappane, which contains the class
ctor, dtor, etc.)

mappane			--	mappane
				->	mapairops
				->	mapcombat
				->	mapconsdemo
				->	mapcounter
mapfeature		--	mapfeature
mapfile			--	mapfile			(formerly loadsave)
mapmemory		--	mapmemory
mapmode			<-
mapmouseevent	--	mapmouseevent
				->	mapmove
				->	mapphase
maprenderer		--	maprenderer		(formerly paint)
				->	mapshowview
mapsheet		<-
				->	mapsit
				->	mapsupply
maputility		--	maputility
				->	mapzoc
*/

class MapPane : public wxScrolledWindow
{
///////////////
//   ENUMS   //
///////////////
#if defined HEXTOOLSMAP
public:
	struct RMBMenuEvent
	{
		enum
		{
			EDITCITY				=	10000,
			HEX_OPTIONSREMOVE_CITY,	//	10001
			HEX_OPTIONSADDCITY,		//	10002
			SHRINKMAP,				//	10003
			INSERTMAPHERE,			//	10004
			HEX_OPTIONSINFO,		//	10005
			MAPLEAFMODE,			//	10006
			HEX_OPTIONSSHOW_WZ,		//	10007
			HEX_OPTIONSHIDE_WZ,		//	10008
			SHOWHEXID,				//	10009
			HIDEHEXID,				//	10010
			CALC_HEXID,				//	10011
			MAPSHEETORIENTATION,	//	10012
			LISTMAPSHEETS,			//	10013
			NOPORT,					//	10014
			MINORPORT,				//	10015
			STANDARDPORT,			//	10016
			MAJORPORT,				//	10017
			GREATPORT,				//	10018
			STRONGPORT,				//	10019
			ARTIFICIALPORT,			//	10020
			COMMANDSEXTEND_MAP,		//	10021
			MARGINALPORT,			//	10022
			ANCHORAGE,				//	10023
			PORT_CENTER,			//	10024
			PORT_NW,				//	10025
			PORT_NE,				//	10026
			PORT_E,					//	10027
			PORT_W,					//	10028
			PORT_SW,				//	10029
			PORT_SE,				//	10030
			NO_RMY,					//	10031
			RMY1,					//	10032
			RMY2,					//	10033
			RMY3,					//	10034
			RMY_CENTER,				//	10035
			RMY_NW,					//	10036
			RMY_NE,					//	10037
			RMY_E,					//	10038
			RMY_W,					//	10039
			RMY_SW,					//	10040
			RMY_SE,					//	10041
			NO_INTRAF,				//	10042
			INTRAF1,				//	10043
			INTRAF3,				//	10044
			AF_CENTER,				//	10045
			AF_NW,					//	10046
			AF_NE,					//	10047
			AF_E,					//	10048
			AF_W,					//	10049
			AF_SW,					//	10050
			AF_SE,					//	10051
			NO_DAM,					//	10052
			DAM_SOUTHWEST,			//	10053
			DAM_SOUTH,				//	10054
			TOGGLE_LATE_SEAICE,		//	10055
			LIMSTACK_INDICATOR,		//	10056
			NO_WMD,					//	10057
			WMD1,					//	10058
			WMD2,					//	10059
			WMD3,					//	10060
			WMD_CENTER,				//	10061
			WMD_NW,					//	10062
			WMD_NE,					//	10063
			WMD_E,					//	10064
			WMD_W,					//	10065
			WMD_SW,					//	10066
			WMD_SE,					//	10067
			TOGGLE_RESTR_WATERS,	//	10068
			SEADISTRICTMODE,		//	10069
			LANDDISTRICTMODE,		//	10070
			CLEARSEACIRCLES,		//	10071
			CLEARLANDDISTRICTS		//	10072
		};
	};
#endif

///////////////
// FUNCTIONS //
///////////////

// implementation mappane.cpp

public:
	MapPane( wxWindow* p, int i, const wxPoint& pt, const wxSize& sz, long l = 0 );
	virtual ~MapPane( );

	// TODO: consider static functions for these, and/or inline
private:
	// helpers (for ctor)
	void define_hex_type( int index, const char* name, const char* filename );
	wxBitmap* define_hexside_type( const char* name, const char* filename );
	wxBitmap* define_symbol( const char* name, const char* filename );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	void initializeAirGraphics( wxBitmap* hbms[2], wxColour fgnd, wxColour bgnd );
	wxBitmap* initializeNavalGraphics( wxBitmap* hbm, wxColour fgnd, wxColour bgnd );
#endif

///////////////////////////////

// implementation in mapairops.cpp
//	|--> corresponds to Fly menu functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	bool Takeoff( bool staging = false );

	// PAPI (Frame)
	void Stage( );

	// PAPI (Frame)
	void CancelAirattack( );

	// PAPI (Frame)
	void AirCombat( );					// fight air combat

private:
	// used by AirCombat(..) and AirMission(..)
	void Intercept( );					// assign selected units to intercept mission

public:
	// PAPI (Frame)
	void AirMission( );					// assign mission to selected planes

private:
	// used by AirMission( )
	int GetEscortMission( );			// assign escort mission: select escortees

public:
	// PAPI (Frame)
	void AssignNavalPatrol( );

	// PAPI (Frame)
	void FireAA( );						// in selected hex

private:
	// used by FireAA( )
	bool IsAxisOwnedAtStart( int x, int y );
	bool IsAlliedOwnedAtStart( int x, int y );
	int IntrinsicCityAA( int x, int y );
	int IntrinsicAirfieldAA( int x, int y );
	int CalcHvyAA( int x, int y );		// only against heavy bombers
	int CalcDASAA( int x, int y );		// AA str against DAS mission in hex
	int CalcAA( int x, int y );

public:
	// PAPI (Frame)
	void ResolveBombing( );				// resolve bombing mission

private:
	// used by ResolveBombing( ), etc.
	int GetCoastalDef( int x, int y );
	int GetRawAFCapacity( int x, int y );		// total capacity (cap-hits)
	int GetCurrentAFCapacity( int x, int y );	// current capacity (cap-hits-usage)

public:
	// PAPI (Frame)
	void Land( );

	// PAPI (Frame)
	void CAPToEscort( );				// selected planes: switch from CAP to escort

	// PAPI (Frame)
	void CAPToInterception( );

#endif

///////////////////////////////

// implementation in mapcombat.cpp
//	|--> corresponds to Orders menu functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	void CombatInfo( int x = -1, int y = -1 ); // of current hex if no x,y given

	// PAPI (Frame)
	int ResolveCombat( bool just_show_combat_info = false, int x = -1, int y = -1, bool overrun = false );  // in current hex if no x,y given

	// PAPI (Frame)
	void Advance( bool isoverrun );		// victorious units can advance to conquered hex

	// PAPI (Frame)
	void CmCancelattack( );

private:
	// used only by CmCancelattack(..)
	void CancelAttackOrders( int x, int y, int dir );

	// private helper functions for ResolveCombat(..) function
	int GetRetreatDir( Counter* cntr );	// called by Retreat
	void Retreat( );					// auto retreat defeated units from the current hex

	// TODO: magic number 20 comes from mapcombat const int DH_MAX_UNITS
	void DeleteMaskedUnits( GroundUnit* u[20], int mask );
	void CommitHalfLosses( Counter* cntr );
	void CommitQuarterLosses( Counter* cntr );
	void CommitLosses( int result );	// destroy defeated units if necessary
	int GetAttSupport( int x, int y );	// status marker attack strength
	int GetDefSupport( int x, int y );	// status marker defense strength
	// PTS_ATTACK and PTS_DEFENSE specific versions of addStatusMarker( )
	// TODO: perhaps rename these addAtkPtsStatusMarker( ) and addDefPtsStatusMarker( )
	//	but neither function is used anywhere
	void SetAttSupport( );
	void SetDefSupport( );
	int CalcWinterization( int dx, int dy, char* explain );
	int ModifyAEC( int x, int y, int aec );		// for terrain, weather, etc.
	int CalcNominalAECA( int x, int y );
	int CommandoUnitAttacking( int x, int y );
	int CalcEngMod( int x, int y );		// engineer modifier (city/fort)
	int GetMods( int x, int y, char* explain = 0 );		// ground combat DRM
	int GermanFirstWinterMod( int dx, int dy, char* explain );
	bool IsHexSupported( int x, int y, int dir = -1, bool include_all_sel = false );
	float CalcSelDefStr( bool supported );
	float CalcDefStr( int x, int y );					 // at 'x,y'
	float CalcUnitAttStr( GroundUnit* unit, int x, int y, int dir, bool sup );
	float CalcSelAttStr( int dir );		// units with attack orders to dir
	float CalcAttStr( int x, int y, int dir );  // from hex 'x,y' to direction 'dir'
	float CalcShipAttStr( NavalUnit* ship, int x, int y, int dir );
	float CalcSelNgsStr( int dir );
	float CalcNgsStr( int x, int y, int dir );
	float CalcGS( int x, int y );		// GS strength in hex
	float CalcDAS( int x, int y );		// DAS strength in hex
	void AddAttArtREs( int x, int y, int dir, float* art_re, float* norm_re );
	void CalcAttArtREs( int x, int y, float* art_re, float* norm_re );

#endif

///////////////////////////////

// implementation in mapconsdemo.cpp
//	|--> corresponds to Construction/Demolition functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	void addFacility( int type = 1 /* Facility::FORT */ ); // by Facility::* type

	// PAPI (Frame)
	void HitAF( );						// damage airbase

	// PAPI (Frame)
	void HitRail( );					// break RR

	// PAPI (Frame)
	void Repair( );						// interactive repairing of damaged RR/AF

	// PAPI (Frame)
	void RestoreAFCapacity( );			// restore 1 pt current hex's AF cap

	// PAPI (Frame)
	void Regauge( );

	// PAPI (Frame)
	void MagicGauge( int mode );		// 1=all, 2=visible

	// PAPI (Frame)
	void ClearObstructed( );			// clear obstructed railroad

	// PAPI (Frame)
	void MagicClearance( int mode );	// 1=all, 2=visible

	// PAPI (Frame)
	void RepairBridge( );  				// for selected hex, ask what dir to fix

	// PAPI (Frame)
	void BreakBridge( );   				// for selected hex, ask what dir to break

	// PAPI (Frame)
	void DestroyPort( );

	// PAPI (Frame)
	void RepairPort( );

	// PAPI (Frame)
	void Fieldworks( );					// place fieldworks to hex

#endif

//	end of Construction/Demolition functionality
///////////////////////////////

// implementation in mapcounter.cpp
//	|--> corresponds to Units menu functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	void CmAddUnit( );

	// PAPI (Frame)
	void addAirUnit( );

	// PAPI (Frame)
	void addNavalUnit( int type = 12 /* NavalUnitType::Ship::CL */ ); // ship or task force, based on rule

	// PAPI (Frame)
	void addResourceMarker( );

	// PAPI (Frame)
	void addPositionalAA( ) { ; }		// TODO: implementation currently empty

private:
	// used by insert/removeUnitTo/FromHex, March(..), Advance(..), Retreat(..)
	void insertUnit( int x, int y, Counter* unit );
	void removeUnit( int x, int y, Counter* unit );

public:
	// PAPI (HexContentsPane)
	void insertUnitToCurrentHex( Counter* unit );
	void removeUnitFromCurrentHex( Counter* unit );

private:
	// used by various combat functions
	void DestroyUnit( Counter* u, bool record_losses = true ); // remove to repl.pool
	void MakeCadre( Counter* u );		// make cadre and mark to log
	// TODO: move this to the unused module
	int EliminateUnits( int x, int y );	// annihilate (NOT to repl.pool) (return # of units)

public:
	// PAPI (Frame)
	void RmCMarkers( );					// remove all combat markers

	// PAPI (Frame)
	void DestroySelUnits( bool record_losses = true );   // remove to repl.pool

	// PAPI (Frame)
	void SetDisruption( int disr );

	// PAPI (Frame)
	void ToggleAttackSup( );			// attack supply of sel. units

	// PAPI (Frame)
	void FlipAirCounter( );				// change selected operative<->inoperative

	// PAPI (Frame)
	void CancelOrder( );				// clear attack/construction orders of sel units

	// PAPI (Frame)
	void ToggleDidPA( );				// 'did patrol attack' flag for sel. air units

	// PAPI (Frame)
	void SetAllREs( void );				// set RE sizes to default values
	void SetAllZOCs( void );			// set ZOCs to default values

	// PAPI (Frame)
	void SetUnitSide( );				// set side of selected units

	// PAPI (Frame)
	void SetNationSide( int side ); 	// set side of all units belonging to subtype

	// PAPI (Frame)
	void CmUnitView( );

private:
	// TODO: make these accessors of GroundUnit class
	bool IsOwnUnit( Counter* unit );	// owned by current player?
	bool IsPartisanUnit( Counter* cp );
	bool IsGermanUnit( Counter* cp );
	bool IsCm( Counter* cp );

#endif

//	end of mapcounter functionality
///////////////////////////////

// implementation MAP/mapfeature.cpp: //

#if defined HEXTOOLSMAP

public:
	void CmAddCity( wxCommandEvent& event );
	void CmEditcity( wxCommandEvent& event );
	void CmRemoveCity( wxCommandEvent& event );

	void CmLimitedStacking( wxCommandEvent& event );

	void CmNoport( wxCommandEvent& event );
	void CmGreatport( wxCommandEvent& event );
	void CmMajorport( wxCommandEvent& event );
	void CmStandardport( wxCommandEvent& event );
	void CmMinorport( wxCommandEvent& event );
	void CmMarginalport( wxCommandEvent& event );
	void CmAnchorage( wxCommandEvent& event );
	void CmStrongport( wxCommandEvent& event );
	void CmArtificialport( wxCommandEvent& event );
	void CmPortCenter( wxCommandEvent& event );
	void CmPortNW( wxCommandEvent& event );
	void CmPortNE( wxCommandEvent& event );
	void CmPortW( wxCommandEvent& event );
	void CmPortE( wxCommandEvent& event );
	void CmPortSW( wxCommandEvent& event );
	void CmPortSE( wxCommandEvent& event );

	void CmNointraf( wxCommandEvent& event );
	void CmIntraf1( wxCommandEvent& event );
	void CmIntraf3( wxCommandEvent& event );
	void CmAfCenter( wxCommandEvent& event );
	void CmAfNW( wxCommandEvent& event );
	void CmAfNE( wxCommandEvent& event );
	void CmAfW( wxCommandEvent& event );
	void CmAfE( wxCommandEvent& event );
	void CmAfSW( wxCommandEvent& event );
	void CmAfSE( wxCommandEvent& event );

	void CmNoDam( wxCommandEvent& event );
	void CmDamSW( wxCommandEvent& event );
	void CmDamS( wxCommandEvent& event );

	void CmToggleLateSeaIce( wxCommandEvent& event );
	void CmToggleRestrictedWaters( wxCommandEvent& event );

#endif // HEXTOOLSMAP

// implementation MAP/mapfeature.cpp and PLAY/mapfeature.cpp //

private:
	// helper functions
	void set_hex_side( int x, int y, int side );
	void clear_hex_side( int x, int y, int side );
	void clear_any_interior_mtn( int x, int y, int side );
	bool hex_ices( int h )
	{
		return ( h == 32 ); // ICINGSEA is 32
	}
	bool hexsides_ice( Hex h );
	int get_lake_sea_ferry_count( int x, int y );

// implementation PLAY/mapfeature.cpp: //

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

	// Hexside feature queries. These are needed because every hex only contains
	// these features for 3 sides (in order to save some memory).
private:
	bool IsRRInHex( int x, int y );
	bool IsRoadInHex( int x, int y );
	bool IsPort( int x, int y );

	bool IsIntlBorder( int x, int y, int dir );
	bool IsMajorRiver( int x, int y, int dir );
	bool IsMinorRiver( int x, int y, int dir );
	bool IsMinorCanal( int x, int y, int dir );
	bool IsCanal( int x, int y, int dir );
	bool IsMountain( int x, int y, int dir );
	bool IsHighMtn( int x, int y, int dir );
	bool IsHighMtnPass( int x, int y, int dir );
	bool IsKarst( int x, int y, int dir );
	bool IsWater( int x, int y, int dir );
	bool IsSeaSide( int x, int y, int dir );
	bool IsLakeSide( int x, int y, int dir );
	bool IsRWaterSide( int x, int y, int dir );
	bool IsNarrowStrait( int x, int y, int dir );
	bool IsWadi( int x, int y, int dir );
	bool IsSaltDesert( int x, int y, int dir );
	bool IsGlacier( int x, int y, int dir );
	bool IsRoad( int x, int y, int dir );
	bool IsTrail( int x, int y, int dir );
	bool IsRailRoad( int x, int y, int dir );
	bool IsLVRailRoad( int x, int y, int dir );
	bool IsEscarpment( int x, int y, int dir );
	bool IsImpEscarpment( int x, int y, int dir );
	bool IsHexsideFortified( int x, int y, int dir );
	bool IsNonClearHexSide( int x, int y, int dir );
	bool IsProhibited( int x, int y, int dir );
	int GetWeather( int x, int y );
	bool IsRivers( int x, int y );  	// any rivers for ships to sail
	bool IsCoastalHex( int x, int y );
	bool IsCoastalCliffs( int x, int y );
	int GetPortRawCapacity( int x, int y );
	int GetPortCapacity( int x, int y );
	bool IsBridge( int x, int y, int dir );
	bool IsBridgeBreak( int x, int y, int dir );
	void SetBridgeBreak( int x, int y, int dir, bool val );
	bool hexInExoticZone( int x, int y );

#endif // HEXTOOLSPLAY

// end of mapfeature functionality
///////////////////////////////

// implementation in mapfile.cpp
//	|--> corresponds to File menu functionality

public:
	// PAPI (Frame)
	void CmInfo( wxCommandEvent& );

	// PAPI (Frame::OnCloseQuery( ))
	bool file_dirty( )
	{
		return file_is_dirty_;
	}

	// PAPI (Frame::OnSave( ))
	bool filename_defined( )
	{
		return filename_is_defined_;
	}

	// PAPI (Frame::saveMap(..) and Frame::setCaption(..) in HT-m)
	char* filename( )
	{
#if defined HEXTOOLSMAP
		return map_filename_;
#elif defined HEXTOOLSPLAY
		return scn_filename_;
#endif
	}

	// PAPI (Frame::loadFiles(..) in HT-m and HT-p)
	bool load_files( const char* filename );

private:
	// used by loadFiles(..) and initializeScenario(..)
	bool load_map( const char* map_filename = nullptr );

#if defined HEXTOOLSMAP

public:
	// PAPI (Frame::....)
	void save_map( const char* filename = nullptr );
	void export_map( const char* image_filename );

private:
	// used by HT-m ctor
	void initialize_map( void );

#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame::OnNew)
	bool initialize_scenario( char* mapfilename );

	// PAPI (Frame::OnInsert)
	bool insert_scenario( const char* filename, int xofs, int yofs );

	// PAPI (Frame::OnSave and Frame::saveScenario)
	bool save_scenario( const char* filename );

	// PAPI (Frame::OnNew/CmChgcrt)
	void set_CRT_name( const char* s );
	bool initialize_CRT( const char* filename, bool from_tabular = true );

	// PAPI (Frame::CmShowcrt)
	void show_CRT( );

	// PAPI (Frame::OnNew/CmChgtec)
	void set_TEC_name( const char* s );
	bool initialize_TEC( const char* filename, bool from_tabular = true );

	// PAPI (MapPane::MapPane??)
	void set_UIC_name( const char* s );
	bool initialize_UIC( const char* filename, bool from_tabular = true );

	// PAPI (MapPane::MapPane??)
	void set_UCC_name( const char* s );
	bool initialize_UCC( const char* filename, bool from_tabular = true );

private:
	// helpers, used by initialize*(..)
	bool load_CRA( const char* filename = nullptr );
	bool load_CRT( const char* filename = nullptr );
	bool load_TEC( const char* filename = nullptr );
	bool load_UIC( const char* filename = nullptr );
	bool load_UCC( const char* filename = nullptr );

	// used by loadFiles(..) and saveScenario(..)
	void draw_load_status_box( wxDC* hdc, const char* str );

#endif // HEXTOOLSPLAY

//	end of File menu functionality
///////////////////////////////

// implementation in mapmemory.cpp
//	|--> supports File menu functions, and ctor
private:
	// for loadMap(..)
	// TODO: why doesn't HT-p ctor need to also allocMapMemory( )?
	void reserve_map_storage( int y_size, int x_size );
	void release_map_storage( int y_size, int x_size, bool delete_cities = true );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

private:
	// for loadFiles(..), initializeScenario(..), and ctor
	void reserve_sit_storage( int y_size, int x_size );
	void release_sit_storage( int y_size, int x_size );

#endif // HEXTOOLSPLAY

//	end of map/sit memory functionality
///////////////////////////////

// implementation MAP/mapmode.cpp: //

#if defined HEXTOOLSMAP
public:
	void CmEndModes( wxCommandEvent& event );
	void CmShowWZ( wxCommandEvent& event );
	void CmMapleafmode( wxCommandEvent& event );
	void CmSeadistrictmode( wxCommandEvent& event );
	void CmClearAllSeaCircles( wxCommandEvent& event );
	void CmLanddistrictmode( wxCommandEvent& event );
	void CmClearAllLandDistricts( wxCommandEvent& event );
	void CmShowhexid( wxCommandEvent& event );
	void CmHidehexid( wxCommandEvent& event );

	void set_weather_zone( int wz = 3 ); // WZ "D"
	void set_current_seacircle( wxString sc = _("NONE") );
	void set_current_district( wxString dist = _("NONE") );
	void set_weather_zone_mode( );
	void set_map_leaf_mode( );
	void set_seacircle_mode( );
	void set_district_mode( );
	void clear_all_modes( );
#elif defined HEXTOOLSPLAY
public:
	void CmSeazonemode( wxCommandEvent& event );
	void CmCountrymode( wxCommandEvent& event );
	void set_current_seazone( wxString sz = _("NONE") );
	void set_current_country( wxString country = _("NONE") );
	void set_seazone_mode( );
	void set_country_mode( );
	void clear_setup_modes( );
#endif

#if defined HEXTOOLSMAP
private:
	void enable_weather_zone_control( bool enable = true );
	void enable_seacircle_control( bool enable = true );
	void enable_district_control( bool enable = true );
#elif defined HEXTOOLSPLAY
private:
	void enable_seazone_control( bool enable = true );
	void enable_country_control( bool enable = true );
#endif

///////////////////////////////

// implementation mapmouseevent.cpp

	// mouse event functions

public:
	void EvLButtonDown( wxMouseEvent& );
	void EvLButtonDblClk( wxMouseEvent& );
	void EvRButtonDown( wxMouseEvent& );
	void EvMouseMove( wxMouseEvent& );

private:
	int get_hex_part( int x, int y );
	int get_hex_vertex( int x, int y );
	void map_district_to_country( int district_id, int country_id );
	void map_seabox_to_seazone( int seabox_id, int seazone_id );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	void EvLButtonUp( wxMouseEvent& );

private:
	// used by PLAY/EvMouseMove(..)
	void draw_path( wxDC* dc, int x0, int y0, int x1, int y1 );
	void draw_movement_path( int x, int y ); // draw path to x,y from current hex

	// used by PLAY/EvLButtonDown(..)
	void erase_movement_path( );			// erase above path
	char* movement_path( int x, int y );

	// used by PLAY/EvRButtonDown(..)
	bool air_combat_here( int x, int y );	// air combat coming?

#endif

// end of mapmouseevent functionality
///////////////////////////////

// implementation in mapmove.cpp
//	|--> corresponds to Move menu functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	void set_move_mode( int mode )
	{
		move_mode_ = mode;
	}

	// PAPI (Frame)
	void CmMoveUnits( );

	// PAPI (Frame)
	void show_rail_usage( );				// REs rail capacity used this turn

	// PAPI (Frame)
	void CmUnitNorthWest( );
	void CmUnitNorthEast( );
	void CmUnitEast( );
	void CmUnitSouthEast( );
	void CmUnitSouthWest( );
	void CmUnitWest( );

	// PAPI (Application via Frame)
	void MoveSelection( int dir );		// arrow keys

	// PAPI (Frame)
	void CmUnitHome( );					// undo movement ("home" is turn start)

	// PAPI (Frame)
	void ToTrain( );					// load selected units to train
	void FromTrain( );					// unload selected units from train

	// PAPI (Frame)
	void ToShip( );						// load selected units to ship
	void FromShip( );					// unload selected units from ship

	// PAPI (Frame)
	void ToOverstack( );				// place units to overstack
	void FromOverstack( );				// remove units from overstack

	// PAPI (Frame)
	void MPPlus( );						// increase MPs of selected unit(s)
	void MPMinus( );
	void MPMinus30( );

	// PAPI (HexContentsPane)
	void clearSelUnitMovementArea( );

	// PAPI (Frame)
	void showSelUnitMovementArea( bool lazy_repaint = false );

	// PAPI (Frame)
	void RestoreAllMPs( int currphase );

private:
	void MoveErr( const char* str );	// whine about illegal movement attempt

	// private helper functions for public RestoreAllMPs(..)
	bool ReactionCombatWithin( int x, int y, int minhexes, int maxhexes );
	bool EnemyUnitWithin( int x, int y, int minhexes, int maxhexes );
	bool IsEnemyCombatUnits( int x, int y );	// is enemy land units?

	// private helper functions for showSelUnitMovementArea(..)
	int calculateNavalHexCost( Counter* cp, int x, int y, int dir );

	void fillTmpShip( Counter* cntr, int x, int y, int dir, int mask );
	void showSelUnitMovementAreaShip( bool lazy_repaint = false );

	void inline fillTmpAdmin( Counter* cntr, int x, int y, int dir, int mask );
	void showSelUnitMovementAreaAdmin( bool lazy_repaint = false );

	void inline fillTmp( Counter* cntr, int x, int y, int dir, int mask );
	bool AllSelUnitUsedMoveMode( int mode ); // mode=ADM etc, true or false?
	void showSelUnitMovementAreaTrain( bool lazy_repaint = false );
	void showSelUnitMovementAreaStrat( bool lazy_repaint = false );
	void showSelUnitMovementAreaAir( bool lazy_repaint = false );
	//void showSelUnitMovementAreaOperational( bool lazy_repaint = false );

	// only called by CmUnit<dir>(..) and EvLButtonDown(..)
	void March( int direction );		// selected ground units on tact. move

	// used by March(..), during overrun
	float CalcAllSelAttStr( int dir );	// units with or without any orders
	int DestroyHexUnits( int x, int y );// remove to repl.pool (return # of units)
	bool IsGroundedEnemyPlanes( int x, int y );
	bool IsEnemyUnits( int x, int y );	// any enemy forces (planes...)?
	void AirMove( int direction );		// fly selected planes
	int CanMoveStack( int dir );		// are all the units in stack same type?
	void AdminMove( int direction );	// selected ground units on admin. move
	void StrategicMove( int direction );// selected ground units on strat. movement
	void TrainMove( int direction );	// units are in train (tactical move)
	void ShipMove( int direction );		// move ships
	int ReduceOverrunZOC( Counter* unit, int x, int y, int direction );  // return MPs paid for overrun ZOC

	// used by March(..), and AdminMove(..)
	int hexsideWeatherZone( int x, int y, int dir );
	int calculateHexsideCost( Counter* unit, int x, int y, int dir );
	// TODO: check this default xdir argument value in old mappane.h, perhaps xdir = -1?
	int calculateZOCCost( Counter* cp, int x, int y, int new_x, int new_y, int xdir = 0 );
	int calculateHexCost( Counter* unit, int x, int y, int dir, int show );  // calc cost to enter hex

#endif

//	end of Move menu functionality
///////////////////////////////

// implementation in mapphase.cpp
//	|--> corresponds to some Options menu functionality
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	bool okToEndThisPhase( );

	// PAPI (Frame)
	void initializeThePhase( );			// do any phase-specific things

	// PAPI (Frame::CmEndPhase)
	// hide the unit mover dialog (to be called at end phase, and during load or insert scenario)
	void hideUnitMover( )
	{
		// TODO:
		//if ( ! move_units_dlg )
		//	return;
		//if ( ! move_units_dlg->IsWindow( ) )
		//	return;
		//if ( ! move_units_dlg->IsWindowVisible( ) )
		//	return;
		//move_units_dlg->Show( false );
	}

#endif

///////////////////////////////

// implementation maprenderer.cpp

	// paint event functions

public:
	void paint( wxPaintEvent& );

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame::OnRepaint, etc.)
	void rePaint( );					// re-draw screen AND re-calculate caches

	// PAPI (HexContentsPane), and internally by PaintHex(..)
	void PaintPaths( );  				// draw paths for selected units, erase for non-selected

	void EraseBgnd( wxEraseEvent& ) { }

private:
	void DrawPath( wxDC* hdc, int x, int y, Counter* c );	// draw path for unit 'c'
	void ErasePath( wxDC* hdc, int x, int y, Counter* c, bool hold = false );  // erase path for unit 'c'

	// used by paint(..)
	void grayHexes( wxDC* dc, int min_x, int min_y, int max_x, int max_y );
	void DrawPaths( );   // draw paths for selected units
	// find out what colors to use in map symbols
	//int  GetUnitNat(int x, int y);	// return unit nationality
	//int  GetPlaneNat(int x, int y);	// return plane nationality
	bool IsShips( int x, int y );
	int GetShipSide( int x, int y );	// return ship side
	int GetShipNat( int x, int y );		// return ship nationality

	void initializeCCBitmap( wxBitmap** hbm, wxColour fgnd, wxColour bgnd, int what );

#endif

// end of maprenderer functionality
///////////////////////////////

// implementation in mapshowview.cpp

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

//	|--> corresponds to View menu functionality

public:
	// PAPI (Frame) but event ids not used anywhere
	void Scroll( int WXUNUSED(dir) )
	{
		wxMessageBox( wxT( "Scroll: TODO" ), Application::NAME );
#if 0
		x = Scroller->XPos;
		y = Scroller->YPos;
		switch ( dir )
		{
			case 8:	--y; break; // up
			case 2:	++y; break; // dn
			case 4:	--x; break; // left
			case 6:	++x; break; // rgt
		}
		Scroller->ScrollTo( x, y );
#endif
	}

	// PAPI (Frame)
	void Search( int what );			// search for stuff (0=hex, 1=city, 2=unit)

	// PAPI (Frame)
	void ShowCombatMarkers( );			// show/hide and ask passwd

	// PAPI (Frame)
	void peekAtSelectedPartisans( );

	// PAPI (Frame)
	void HiliteWZ( int wz );			// weather zone hilite, A == 0, B == 1, etc.

private:
	// used by HiliteStuff(..)
	bool IsValidStack( int x, int y );  // valid stacking in this hex?
	// TODO: move to unused module
	void CheckStacks( );				// obsolete version of IsValidStack(..)

	// enumeration for HiliteStuff(..):
	// 1=armor, 2=art, 3=cdr, 4=all planes, 5=in air planes,
	// 6=illegal stacks, 7=c/m, 8=cons, 9=rreng, 10=abort plns,
	// 11=trucks, 12=airbases, 13=inop planes, 14=disrupted units
public:
	// PAPI (Frame)
	void HiliteStuff( int what );

private:
	// used only by HiliteZones(..), but does paint-like work
	void HiLiteHexLight( int x, int y );	// highlight hex

	void HiliteArea( int x, int y, int range );

public:
	// PAPI (Frame)
	void HiliteZones( int what );		// zones, 1-4=interception, 5=patrol attack

//	end of View menu functionality

//	|--> corresponds to Show menu functionality

public:
	// PAPI (Frame)
	void ShowViewText( );

	// PAPI (Frame)
	void ShowAllViews( bool show );

	// PAPI (Frame)-
	void ToggleShowView( int nview );

	// PAPI (Frame), but Frame::CmEditView( ) not used anywhere
	void EditView( int nview );

	// PAPI (Frame), from Frame::SetupMenu( )
	int GetViewCount( );

	// PAPI (Frame)
	int GetActiveViewCount( );

	// PAPI (Frame)
	bool ViewIsActive( int nview );

	// PAPI (Frame), from Frame::SetupMenu( )
	char* GetViewName( int nview );

	// PAPI (Frame), from Frame::CmAllViewsOn( )
	int ViewHexCount( int nview );

	// PAPI (Frame), from Frame::UpdateViewData( )
	void UpdateViewData( );

private:
	bool HexInView( int nview, int x, int y );
	int FirstActiveView( );
	int GetViewX( int nview, int pt );
	int GetViewY( int nview, int pt );
	int GetViewRed( int nview );
	int GetViewGreen( int nview );
	int GetViewBlue( int nview );
	bool DeletePointFromView( int nview, int x, int y );
	void AddPointToView( int nview, int x, int y );
	void ClearAllViews( );
	 // TODO: this is not used, move to unused module
	bool CreateNewView( int nview );

	// end of Show menu functionality

#endif

///////////////////////////////

// implementation MAP/mapsheet.cpp: //

#if defined HEXTOOLSMAP

public:
	// map resize directions
	static const int RESIZE_MAP_TO_NORTH = 1;
	static const int RESIZE_MAP_TO_EAST = 2;
	static const int RESIZE_MAP_TO_SOUTH = 3;
	static const int RESIZE_MAP_TO_WEST = 4;

	void CmExpandMap( wxCommandEvent& event );
	void CmShrinkMap( wxCommandEvent& event );
	void CmInsertMapHere( wxCommandEvent& event );
	void CmMapsheetOrientation( wxCommandEvent& event );
	void CmListMapsheets( wxCommandEvent& event );
	void CmCalcHexID( wxCommandEvent& event );

private:
	void copy_sectors_expand( Hex** ptr, int x_offset, int y_offset );
	void copy_sectors_shrink( Hex** ptr, int x_offset, int y_offset );
	// used by CmExpandMap(..) and CmShrinkMap(..):
	bool allocate_new_map_memory( Hex*** ptr, int x, int y ); // for map resizing

	void inventory_mapsheets( );

	char* mapsheet_name( int sheetindex );
	int mapsheet_index( char* sheetname );
#if 0
	char* mapSheetName( int i );
#endif
	int rotated_mapsheet_count( void );
	int rightoffset_mapsheet_count( void );

	void set_rotated( const char* sheetname, bool do_rotate = true );
	void set_right_offset( const char* sheetname, bool do_offset = true );

	// map sheet functions below use 'mapsheet_count_' either directly
	//	or indirectly; thus, call inventory_mapsheets( ) before calling
	//	any of them (optimization kludge...)
	bool find_map_origin( const char* sheetname, int* ox, int* oy );

	bool mapsheet_rotated( const char* sheetname );
	void clear_all_rotated_mapsheets( );

	bool mapsheet_normal_right_offset( const char* sheetname );
	void clear_all_right_offset_mapsheets( );

	void clear_odd_mapsheet( const char* sheetname );

#endif // HEXTOOLSMAP

///////////////////////////////

// implementation in mapsit.cpp
//	|--> corresponds to Hex menu functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	void CmViewHex( );					// show hex info dialog

	// PAPI (Frame)
	void ViewStack( );					// stack in the selected hex

	// PAPI (Frame)
	void AddNote( bool alert = false );	// place text note to selected hex

private:
	// do not call this function
	// TODO: then why not move it to unused module?
	void RemoveNote( );

public:
	// PAPI (Frame)
	void addStatusMarker( );

	// PAPI (Frame)
	void ChangeOwner( );

	// PAPI (Frame)
	void SetOwner( int owner );			// current hex

	// PAPI (Frame)
	void SetVisibleOwner( int owner );	// currently visible hexes

	// PAPI (Frame)
	void SetAllOwner( int owner );		// all hexes

	// PAPI (Frame)
	void ChangeOrigOwner( int i = 0 );	// from 0=selected hex, 1=all, 2=visible

private:
	// used by any function that sets hits
	void updateHitMarkers( int x, int y );  // creates/destroys necessary hit counters

	bool IsAxisOwned( int x, int y );
	bool IsAlliedOwned( int x, int y );

	bool IsEnemyOccupied( int x, int y );
	bool IsAdjEnemies( int x, int y );

	bool IsOwnOccupied( int x, int y );

	int GetNoOfAttackingUnits( int x, int y, int dir ); // # of units attacking from 'x,y' *to* direction 'dir'
	bool IsHexAttacked( int x, int y );

	bool IsHexAirAttacked( int x, int y );

public:
	// PAPI (Frame)
	void changeHexSupply( );			// change supply status of selected hex

	// PAPI (Frame)
	void updateAllSupply( );			// update supply status of all units

	// PAPI (Frame)
	void changeHexSpecialSupply( );

	// PAPI (Frame)
	void changeHexIsolation( );

	// PAPI (Frame)
	void updateAllIsolation( );			// update isolation status of all units

	// PAPI (Frame)
	void SetAFHits( );
	void SetAFUsage( );

	// PAPI (Frame)
	void SetPortHits( );
	void SetPortUsage( );

	// PAPI (Frame)
	void SetCDHits( );

	// PAPI (Frame)
	void SetHarassment( );

	// PAPI (Frame)
	void SetAxMines( );
	void SetAlMines( );

	// PAPI (Frame)
	// from toolbar
	void LayMines( );					// from ships (TF)
	void SweepMines( );					// from ships (TF)

#endif

//	end of mapsit (Hex menu) functionality
///////////////////////////////

// implementation in mapsupply.cpp
//	|--> corresponds to unit supply functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	void showSelUnitSupplyRange( bool lazy_repaint = false );

private:
	// used by showSelUnitSupplyRange(..) function
	float TotalOwnREs( int x, int y );
	float TotalEnemyREs( int x, int y );
	int calculateHexsideSupplyLineCost( Counter* cp, int x, int y, int dir );
	int calculateHexSupplyLineCost( Counter* cp, int x, int y, int dir, bool show );
	void inline fillTmpSupply( Counter* cntr, int x, int y, int dir, int mask );

#endif

//	end of supply functionality
///////////////////////////////

// implementation in maputility.cpp
//	|--> miscellaneous functionality

public:
	// TODO: is this public in HT-m?  it's private in HT-p
	void centerHex( int x, int y );		// center to screen

	// PAPI (Frame)
	void zoom( int dir );

private:
	void StatusMsg( const char* s );	// message in StatusBar

	void setScrollbarRange( int x = -1, int y = -1 );  // call this if map size changes (load..)

	void setZoomScale( float f )
	{
		scale_ = f;
		// TODO: in HT-m, why doesn't this function call setScrollbarRange(..) like the version in HT-p does?
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
		setScrollbarRange( );
#endif
	}

	// convert screen coordinates to hex coordinates, takes care of scrollbars.
	inline void screen2Hex( int* x, int* y )
	{
		int rx, ry;
		CalcUnscrolledPosition( 0, 0, &rx, &ry );

		rx = static_cast<int>( ( (float)rx ) * scale_ );
		ry = static_cast<int>( ( (float)ry ) * scale_ );
		rx += *x;
		ry += *y;

		*y = ry / Hex::SIZE_Y;
		*x = ( ( rx ) - ( ( *y % 2 ) * ( Hex::SIZE_X / 2 ) ) ) / Hex::SIZE_X;
	}

	// converts hexes to screen coordinates, takes care of the scrollbars
	inline void hex2Screen( int* x, int* y )
	{
		int sx, sy;
		CalcScrolledPosition( 0, 0, &sx, &sy );

		sx = static_cast<int>( sx * scale_ );
		sy = static_cast<int>( sy * scale_ );

		*x = ( *x * Hex::SIZE_X ) + ( *y % 2 ) * ( Hex::SIZE_X / 2 ) + sx;
		*y = *y * Hex::SIZE_Y + sy;
	}

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

public:
	// PAPI (Frame)
	void DieRoll( int numdie = 1, int diemax = 6 );

	// PAPI (Frame)
	void ViewReplPool( );				// view and optionally replace
	void ViewAirReplPool( );			// view and optionally replace

	// PAPI (Frame)
	void Statistics( int side );		// some info about strengths, etc.

	// PAPI (Frame)
	void ViewLosses( int side = -1 );	// -1 defaults to both sides

	// PAPI (Frame)
	void analyzeGroundCombatReports( );

	// PAPI (HexContentsPane)
	void paintCurrentHex( );

	// PAPI (Application)
	void cancel( );						// deselect units in HexContentsPane

	// PAPI (Frame ctor)
	void attachUnitWindow( HexContentsPane* w )
	{
		contents_pane_ = w;
	}

	// PAPI (Frame ctor)
	void attachInfoWindow( HexInfoPane* w )
	{
		info_pane_ = w;
	}

private:
	// TODO:  should rename this updateSitCache(..)??
	void updateHexCache( int x, int y );// some values are cached in Sit[][]

	void HiLiteHex( int x, int y, bool is_red = false );	// highlight hex

	void UnHiLiteHex( );

	void SelectHex( int x, int y );

	void SelHexErr( void )				// whine about hex not being selected
	{
		wxMessageBox( wxT("Select hex first!"), Application::NAME, wxOK );
	}

	void SelUnitErr( void )				// whine about unit(s) not being selected
	{
		wxMessageBox( wxT("Select unit(s) first!"), Application::NAME, wxOK );
	}

	void SideErr( void )				// whine about wrong side
	{
		wxMessageBox( wxT("Wrong side (you can't control enemy units)!"), Application::NAME, wxOK );
	}

	void PaintHex( int x, int y );		// re-paint one hex

	bool isCurrentHex( int x, int y )
	{
		return ( x == selected_hex_point_.x && y == selected_hex_point_.y );
	}

	// used by IsZOCSource(..) and March(..)
	// return true if user has selected some hex
	bool isHexSelected( )
	{
		return ( selected_hex_point_.x < 0 || selected_hex_point_.y < 0 || selected_hex_point_.x >= world_x_size_ || selected_hex_point_.y > world_y_size_ ) ? false : true;
	}

#if defined HEXTOOLSDEBUG
	void dump_cur_hex_units( );
#endif // HEXTOOLSDEBUG

#endif // HEXTOOLSPLAY

// end of maputility functionality
///////////////////////////////

// implementation in mapzoc.cpp
//	|--> corresponds to ZOC and RZOC functionality

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

private:
	// TODO: these 6 functions can be private, used elsewhere in MapPane
	bool isHexInEnemyZOC( int x, int y, int xdir = 0 );
	bool isHexInEnemyRZOC( int x, int y, int xdir = 0 );
	bool isHexInFriendlyZOC( int x, int y );
	bool isHexInFriendlyRZOC( int x, int y );
	bool IsZOCSource( int x, int y );
	bool IsReducedZOCSource( int x, int y );

	// TODO: the remaining 10 functions can be static/internal linkage,
	//	used only in mapzoc.cpp
#if 0
	bool ReducedZOCHexside( int x, int y, int dir );
#endif
	bool IsAxisZOC( int x, int y, int xdir = 0 );
	bool IsAxisReducedZOC( int x, int y, int xdir = 0 );
	bool AxisZOCFrom( int x, int y, int dir );
	bool AxisReducedZOCFrom( int x, int y, int dir );

	bool IsAlliedZOC( int x, int y, int xdir = 0 );
	bool IsAlliedReducedZOC( int x, int y, int xdir = 0 );
	bool AlliedZOCFrom( int x, int y, int dir );
	bool AlliedReducedZOCFrom( int x, int y, int dir );

	bool NoAlliedZOCAcrossBorder( int x, int y, int dir );

#endif // HEXTOOLSPLAY

//	end of ZOC functionality
///////////////////////////////

////////////////////
//  PUBLIC DATA   //
////////////////////

#if defined HEXTOOLSMAP
public:
	char mapsheet_[ht::MAX_MAPSHEETS][ht::MAX_MAP_ID_LEN];

#elif defined HEXTOOLSPLAY
public:
	// MapPane public data members/flags for HT-p only
	// this is the only truly public member, used by Frame::3 reads, 2 writes (one a toggle)
	int show_owner_;        // show hex owner in map
	// rest of these are only accessed w/i MapPane class, can be made private
	int path_drawn_;
	int previous_x_;
	int previous_y_;
	bool unit_mover_visible_;
	Views* views_;
	int dont_redraw_map_;  // kludge for skipping some painting...
	int gray_out_radius_;
	int gray_out_rail_radius_;
	int gray_out_admin_radius_;
	int gray_out_strat_radius_;
#endif // HEXTOOLSPLAY public data

////////////////////
// PROTECTED DATA //
////////////////////

protected:

	Frame* parent_;
	Hex** hex_;							// pointer to hex map (static data)
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	HexContents** sit_;					// pointer to hex map (dynamic data)
	HexInfoPane* info_pane_;			// hex info display
	HexContentsPane* contents_pane_;	// unit display and selection window
#endif

	int world_x_size_;					// size of the world
	int world_y_size_;
	wxPoint selected_hex_point_;		// Location of the hex selected
	float scale_;						// 1=32 pixels/hex, 2=16 pixels/hex, 4=8, ...
	wxFont* hfCityFont;					// font for city names
	char map_filename_[127];			// name of the current .map file
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	char scn_filename_[127];			// name of the current .scn file
#endif

	// flags for current working file (.map for HT-m and .scn for HT-p)
	bool filename_is_defined_;
	bool file_is_dirty_;				// whether file has been modified

	int mapsheet_count_;				// mapsheets in current map

#if defined HEXTOOLSMAP
	bool show_hexid_;
	char current_mapid_[10];			// "11A"
	bool weather_zone_mode_;			// show *and* draw
	int selected_weather_zone_;			// weather zone to be drawn (if in WZ mode)
	bool mapleaf_mode_;
	bool sea_district_mode_;
	bool land_district_mode_;
	int selected_sea_district_;
	int selected_land_district_;
#elif defined HEXTOOLSPLAY
	int move_mode_;						// TACTICAL, ADMIN or STRATEGIC
	int left_button_down_;				// left mouse button is down
	char marker_password_[50];
	int drawing_path_;					// kludge to avoid recursion in path drawing
	char crt_filename_[127];
	char tec_filename_[127];
	char uic_filename_[127];
	char ucc_filename_[127];

	// wxString hash, mapped to GroundUnitType
	WX_DECLARE_STRING_HASH_MAP( GroundUnitType, GroundUnitTypeMap );
	GroundUnitTypeMap ground_unit_ww1_types_;
	GroundUnitTypeMap ground_unit_ww2_types_;
	GroundUnitTypeMap* ground_unit_types_;

	// clickmarch target hex
	int click_target_x_;
	int click_target_y_;

	int minor_canal_MP_cost_;
	int minor_canal_frozen_MP_cost_;
	int minor_river_MP_cost_;
	int minor_river_frozen_MP_cost_;
	int major_river_MP_cost_;
	int major_river_frozen_MP_cost_;
	int lake_MP_cost_;
	int lake_frozen_MP_cost_;
	int use_WW1_hexsides_;					// toggled from TEC file
	float turn_REs_rail_used_;

	// quickscroll:
	int qscroll_mouse_x0_;
	int qscroll_mouse_y0_;					// mouse pos at scroll begin
	int qscroll_scroller_x0_;
	int qscroll_scroller_y0_;					// scroller position at scroll begin

	// losses
	int unisol_losses_[100][12][2][2];		// [year][month][turn][player] unisolated
	int isol_losses_[100][12][2][2];		// isolated losses
	//    int SovSpecialInfRP[100][12][2];  // [year][month][turn]
	//    int SovSpecialArmRP[100][12][2];  // [year][month][turn]
	//    int SovSpecialArtRP[100][12][2];  // [year][month][turn]

	int highlighted_hex_x_;
	int highlighted_hex_y_;				// highlighted hex ( HiliteX < 0 if none )
	int click_marching_mode_;			// true if March( ) is called from mouse click moving

	bool sea_zone_mode_;
	bool country_mode_;
	int selected_sea_zone_;
	int selected_country_;

	// Dialog boxes
	DUnitMover* unit_mover_dialog_;
	DView* view_text_dialog_;

	wxClientDC* map_DC_;

#endif // HEXTOOLSPLAY protected data

/////////////////////////
// PUBLIC DRAWING DATA //
/////////////////////////

public:
	// pens common to HT-m and HT-p
	wxPen* hpenPortRoute;		// inland port access
	wxPen* hpenWaterwayRoute;	// waterway network extent
	wxPen* hpenWaterwayRouteOnIce;	// waterway network extent on icing seas
	wxPen* hpenDemarcateBorder;	// international borders
	wxPen* hpenDemarcateIBorder;// internal borders
	wxPen* hpenLakeSeaFerry;	// open sea/lake ferries
	wxPen* hpenLakeSeaFerryOnIce;	// iced sea/lake ferries
	wxPen* hpenShippingChannel;	// open water shipping channels
	wxPen* hpenTrack;			// tracks (trails)
	wxPen* hpenMotorTrack;		// motorable tracks (fair wx roads)
	wxPen* hpenRoad;			// roads (all wx roads)
	wxPen* hpenRailTrack;		// railtracks
	wxPen* hpenRailRoad;
	wxPen* hpenPriCityBoulevard;// route boulevards in primary cities
	wxPen* hpenRailFerry;
	wxPen* hpenBar;				// bars to waterway movement
	wxPen* hpenEscarpment;
	wxPen* hpenImpEscarpment;
	//wxPen* hpenFort;			// fortified hexside

	// graphics common to HT-m and HT-p
	wxBitmap* hbmRestrictedWatersOverlay; // restricted waters black mask
	wxBitmap* hbmIslandCustomMask; // atoll/small island black mask

	// city type graphics
	wxBitmap* hbmMinorCity;		// reference city
	wxBitmap* hbmMediumCity;	// dot city
	wxBitmap* hbmPartialHexCity;
	wxBitmap* hbmFullHexCity;
	wxBitmap* hbmPtCity;
	wxBitmap* hbmRefPt;
	wxBitmap* hbmCityBgnd;
	wxBitmap* hbmHydroRes;
	wxBitmap* hbmOreRes;
	wxBitmap* hbmMineralRes;
	wxBitmap* hbmCoalRes;
	wxBitmap* hbmNaturalGasRes;
	wxBitmap* hbmOilRes;
	wxBitmap* hbmOilShaleRes;
	wxBitmap* hbmSynthOilRes;
	wxBitmap* hbmPeatRes;
	wxBitmap* hbmCementRes;
	wxBitmap* hbmRubberRes;
	wxBitmap* hbmSynthRubberRes;
	wxBitmap* hbmResCtrMask;
	wxBitmap* hbmResCtrTextMask;
	wxBitmap* hbmOasis;
	wxBitmap* hbmOasisMask;
	wxBitmap* hbmShipyard;
	wxBitmap* hbmLtdStack;
	wxBitmap* hbmWW1OFort;		// OLD fortress
	wxBitmap* hbmWW1NFort;		// NEW fortress
	wxBitmap* hbmWW1GFort;		// GREAT fortress
	wxBitmap* hbmNWGWall;
	wxBitmap* hbmNEGWall;
	wxBitmap* hbmEGWall;
	wxBitmap* hbmWGWall;
	wxBitmap* hbmSEGWall;
	wxBitmap* hbmSWGWall;
	wxBitmap* hbmFortMask;
	wxBitmap* hbmTribalArea;
	wxBitmap* hbmTribalMask;
	wxBitmap* hbmBerberArea;
	wxBitmap* hbmBerberMask;
	wxBitmap* hbmCamelArea;
	wxBitmap* hbmCamelMask;
	wxBitmap* hbmTsetseArea;
	wxBitmap* hbmTsetseMask;
	wxBitmap* hbmIndianArea;
	wxBitmap* hbmIndianMask;

	// graphics for hexsides
	wxBitmap* hbmWWater;
	wxBitmap* hbmSWWater;
	wxBitmap* hbmSEWater;
	wxBitmap* hbmWIcingSea;
	wxBitmap* hbmSWIcingSea;
	wxBitmap* hbmSEIcingSea;
	wxBitmap* hbmWRWater;
	wxBitmap* hbmSWRWater;
	wxBitmap* hbmSERWater;
	wxBitmap* hbmWIcingRWater;
	wxBitmap* hbmSWIcingRWater;
	wxBitmap* hbmSEIcingRWater;
	wxBitmap* hbmWLakeRiverMask;
	wxBitmap* hbmSWLakeRiverMask;
	wxBitmap* hbmSELakeRiverMask;
	wxBitmap* hbmWMountainMask;
	wxBitmap* hbmSWMountainMask;
	wxBitmap* hbmSEMountainMask;
	wxBitmap* hbmWMountain;
	wxBitmap* hbmSWMountain;
	wxBitmap* hbmSEMountain;
	wxBitmap* hbmWHighMtnMask;
	wxBitmap* hbmSWHighMtnMask;
	wxBitmap* hbmSEHighMtnMask;
	wxBitmap* hbmWHighMtn;
	wxBitmap* hbmSWHighMtn;
	wxBitmap* hbmSEHighMtn;
	wxBitmap* hbmWHighMtnPass;
	wxBitmap* hbmSWHighMtnPass;
	wxBitmap* hbmSEHighMtnPass;
	wxBitmap* hbmWMinRivMask;
	wxBitmap* hbmSWMinRivMask;
	wxBitmap* hbmSEMinRivMask;
	wxBitmap* hbmWMinRiv;
	wxBitmap* hbmSWMinRiv;
	wxBitmap* hbmSEMinRiv;
	wxBitmap* hbmWStandaloneBridge;
	wxBitmap* hbmSWStandaloneBridge;
	wxBitmap* hbmSEStandaloneBridge;
	wxBitmap* hbmWFord;
	wxBitmap* hbmSWFord;
	wxBitmap* hbmSEFord;
	wxBitmap* hbmWFordMask;
	wxBitmap* hbmSWFordMask;
	wxBitmap* hbmSEFordMask;
	wxBitmap* hbmWSeasonalRiv;
	wxBitmap* hbmSWSeasonalRiv;
	wxBitmap* hbmSESeasonalRiv;
	wxBitmap* hbmWSeasonalRivMask;
	wxBitmap* hbmSWSeasonalRivMask;
	wxBitmap* hbmSESeasonalRivMask;
	wxBitmap* hbmWCanal;
	wxBitmap* hbmSWCanal;
	wxBitmap* hbmSECanal;
	wxBitmap* hbmWCanalMask;
	wxBitmap* hbmSWCanalMask;
	wxBitmap* hbmSECanalMask;
	wxBitmap* hbmWMinorCanal;
	wxBitmap* hbmSWMinorCanal;
	wxBitmap* hbmSEMinorCanal;
	wxBitmap* hbmWMajRivMask;
	wxBitmap* hbmSWMajRivMask;
	wxBitmap* hbmSEMajRivMask;
	wxBitmap* hbmWMajRiv;
	wxBitmap* hbmSWMajRiv;
	wxBitmap* hbmSEMajRiv;
	wxBitmap* hbmWFillingReservoir;
	wxBitmap* hbmSWFillingReservoir;
	wxBitmap* hbmSEFillingReservoir;
	wxBitmap* hbmWKarst;
	wxBitmap* hbmSWKarst;
	wxBitmap* hbmSEKarst;
	wxBitmap* hbmWRBorder; // Red Border, RBdr == red solid border
	wxBitmap* hbmSWRBorder;
	wxBitmap* hbmSERBorder;
	wxBitmap* hbmWBorder; // Border, Bdr == international border
	wxBitmap* hbmSWBorder;
	wxBitmap* hbmSEBorder;
	wxBitmap* hbmWBdrSeasonRiver;
	wxBitmap* hbmSWBdrSeasonRiver;
	wxBitmap* hbmSEBdrSeasonRiver;
	wxBitmap* hbmWBdrMinRiver;
	wxBitmap* hbmSWBdrMinRiver;
	wxBitmap* hbmSEBdrMinRiver;
	wxBitmap* hbmWBdrCanal;
	wxBitmap* hbmSWBdrCanal;
	wxBitmap* hbmSEBdrCanal;
	wxBitmap* hbmWBdrMinCanal;
	wxBitmap* hbmSWBdrMinCanal;
	wxBitmap* hbmSEBdrMinCanal;
	wxBitmap* hbmWBdrWadi;
	wxBitmap* hbmSWBdrWadi;
	wxBitmap* hbmSEBdrWadi;
	wxBitmap* hbmWIBorder; // IBorder, IBdr == internal border
	wxBitmap* hbmSWIBorder;
	wxBitmap* hbmSEIBorder;
	wxBitmap* hbmWIBdrSeasonRiver;
	wxBitmap* hbmSWIBdrSeasonRiver;
	wxBitmap* hbmSEIBdrSeasonRiver;
	wxBitmap* hbmWIBdrMinRiver;
	wxBitmap* hbmSWIBdrMinRiver;
	wxBitmap* hbmSEIBdrMinRiver;
	wxBitmap* hbmWIBdrCanal;
	wxBitmap* hbmSWIBdrCanal;
	wxBitmap* hbmSEIBdrCanal;
	wxBitmap* hbmWIBdrMinCanal;
	wxBitmap* hbmSWIBdrMinCanal;
	wxBitmap* hbmSEIBdrMinCanal;
	wxBitmap* hbmWIBdrWadi;
	wxBitmap* hbmSWIBdrWadi;
	wxBitmap* hbmSEIBdrWadi;
	wxBitmap* hbmWISBorder; // ISBorder, ISBdr == internal sub-border
	wxBitmap* hbmSWISBorder;
	wxBitmap* hbmSEISBorder;
	wxBitmap* hbmWIBBorder; // IBBorder, IBBdr == internal black sub-border
	wxBitmap* hbmSWIBBorder;
	wxBitmap* hbmSEIBBorder;
	wxBitmap* hbmWISBorderMask;
	wxBitmap* hbmSWISBorderMask;
	wxBitmap* hbmSEISBorderMask;
	wxBitmap* hbmWISBdrSeasonRiver;
	wxBitmap* hbmSWISBdrSeasonRiver;
	wxBitmap* hbmSEISBdrSeasonRiver;
	wxBitmap* hbmWIBBdrSeasonRiver;
	wxBitmap* hbmSWIBBdrSeasonRiver;
	wxBitmap* hbmSEIBBdrSeasonRiver;
	wxBitmap* hbmWISBdrMinRiver;
	wxBitmap* hbmSWISBdrMinRiver;
	wxBitmap* hbmSEISBdrMinRiver;
	wxBitmap* hbmWIBBdrMinRiver;
	wxBitmap* hbmSWIBBdrMinRiver;
	wxBitmap* hbmSEIBBdrMinRiver;
	wxBitmap* hbmWISBdrMinCanal;
	wxBitmap* hbmSWISBdrMinCanal;
	wxBitmap* hbmSEISBdrMinCanal;
	wxBitmap* hbmWIBBdrMinCanal;
	wxBitmap* hbmSWIBBdrMinCanal;
	wxBitmap* hbmSEIBBdrMinCanal;
	wxBitmap* hbmWISBdrCanal;
	wxBitmap* hbmSWISBdrCanal;
	wxBitmap* hbmSEISBdrCanal;
	wxBitmap* hbmWIBBdrCanal;
	wxBitmap* hbmSWIBBdrCanal;
	wxBitmap* hbmSEIBBdrCanal;
	wxBitmap* hbmWISBdrWadi;
	wxBitmap* hbmSWISBdrWadi;
	wxBitmap* hbmSEISBdrWadi;
	wxBitmap* hbmWIBBdrWadi;
	wxBitmap* hbmSWIBBdrWadi;
	wxBitmap* hbmSEIBBdrWadi;
	wxBitmap* hbmNWFort;
	wxBitmap* hbmNEFort;
	wxBitmap* hbmEFort;
	wxBitmap* hbmSEFort;
	wxBitmap* hbmSWFort;
	wxBitmap* hbmWFort;
	wxBitmap* hbmFortress;
	wxBitmap* hbmOuvrage;
	wxBitmap* hbmWestWall;
	wxBitmap* hbmVolksDeutsch;
	wxBitmap* hbmWWadiMask;
	wxBitmap* hbmSWWadiMask;
	wxBitmap* hbmSEWadiMask;
	wxBitmap* hbmWWadi;
	wxBitmap* hbmSWWadi;
	wxBitmap* hbmSEWadi;
	wxBitmap* hbmWGlacierMask;
	wxBitmap* hbmSWGlacierMask;
	wxBitmap* hbmSEGlacierMask;
	wxBitmap* hbmWGlacier;
	wxBitmap* hbmSWGlacier;
	wxBitmap* hbmSEGlacier;
	wxBitmap* hbmWSaltDesertMask;
	wxBitmap* hbmSWSaltDesertMask;
	wxBitmap* hbmSESaltDesertMask;
	wxBitmap* hbmWSaltDesert;
	wxBitmap* hbmSWSaltDesert;
	wxBitmap* hbmSESaltDesert;
	wxBitmap* hbmWDblEsc;
	wxBitmap* hbmSWDblEsc;
	wxBitmap* hbmSEDblEsc;
	wxBitmap* hbmWMD1;
	wxBitmap* hbmWMD2;
	wxBitmap* hbmWMD3;
	wxBitmap* hbmWMDMask;
	wxBitmap* hbmMajorNavalBase;
	wxBitmap* hbmMinorNavalBase;
	wxBitmap* hbmMinorPort;
	wxBitmap* hbmStandardPort;
	wxBitmap* hbmMajorPort;
	wxBitmap* hbmGreatPort;
	wxBitmap* hbmMarginalPort;
	wxBitmap* hbmAnchorage;
	wxBitmap* hbmArtificialPort;
	wxBitmap* hbmStrongPort;
	wxBitmap* hbmSquarePortMask;
	wxBitmap* hbmRoundPortMask;
	wxBitmap* hbmPortIndicatorMask;
	wxBitmap* hbmAirfield1;
	wxBitmap* hbmAirfield3;
	wxBitmap* hbmAirNavalBaseMask;
	wxBitmap* hbmRMY1;
	wxBitmap* hbmRMY2;
	wxBitmap* hbmRMY3;
	wxBitmap* hbmEscColor;
	wxBitmap* hbmNWEsc;
	wxBitmap* hbmNEEsc;
	wxBitmap* hbmEEsc;
	wxBitmap* hbmSWEsc;
	wxBitmap* hbmSEEsc;
	wxBitmap* hbmWEsc;
	wxBitmap* hbmSWMask;
	wxBitmap* hbmSEMask;
	wxBitmap* hbmWMask;
	wxBitmap* hbmSWDecoMask;
	wxBitmap* hbmSEDecoMask;
	wxBitmap* hbmWDecoMask;
	wxBitmap* hbmSWRestrMask;
	wxBitmap* hbmSERestrMask;
	wxBitmap* hbmWRestrMask;
	wxBitmap* hbmNERestrMask;
	wxBitmap* hbmNWRestrMask;
	wxBitmap* hbmERestrMask;
	wxBitmap* hbmNRestrVertexMask;
	wxBitmap* hbmSRestrVertexMask;
	wxBitmap* hbmNWRestrVertexMask;
	wxBitmap* hbmSWRestrVertexMask;
	wxBitmap* hbmNERestrVertexMask;
	wxBitmap* hbmSERestrVertexMask;
	wxBitmap* hbmSWClear;
	wxBitmap* hbmSEClear;
	wxBitmap* hbmWClear;
	wxBitmap* hbmSWRough;
	wxBitmap* hbmSERough;
	wxBitmap* hbmWRough;
	wxBitmap* hbmSWForest;
	wxBitmap* hbmSEForest;
	wxBitmap* hbmWForest;
	wxBitmap* hbmSWWoods;
	wxBitmap* hbmSEWoods;
	wxBitmap* hbmWWoods;
	wxBitmap* hbmSWWoodedRough;
	wxBitmap* hbmSEWoodedRough;
	wxBitmap* hbmWWoodedRough;
	wxBitmap* hbmNWMtnInterior;
	wxBitmap* hbmNEMtnInterior;
	wxBitmap* hbmEMtnInterior;
	wxBitmap* hbmSEMtnInterior;
	wxBitmap* hbmSWMtnInterior;
	wxBitmap* hbmWMtnInterior;
	wxBitmap* hbmNWMtnInteriorMask;
	wxBitmap* hbmNEMtnInteriorMask;
	wxBitmap* hbmEMtnInteriorMask;
	wxBitmap* hbmSEMtnInteriorMask;
	wxBitmap* hbmSWMtnInteriorMask;
	wxBitmap* hbmWMtnInteriorMask;

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY

	wxPen* hpenHiliteRed; 	      // thick
	wxPen* hpenHiliteBlack;       // thick
	wxPen* hpenHiliteWhite;       // thick

#if 0
	// deprecated in HT-p??
	wxPen* hpenRoad;				// pen for roads
	wxPen* hpenTrack;				// ww2pacific trails
	wxPen* hpenAxisStdRR;      		// pen for railroads
	wxPen* hpenAlliedStdRR;			// pen for railroads
	wxPen* hpenAxisWideRR;			// pen for railroads
	wxPen* hpenAlliedWideRR;		// pen for railroads
	wxPen* hpenAxisLVStdRR;			// pen for low volume railroads
	wxPen* hpenAlliedLVStdRR;		// pen for low volume railroads
	wxPen* hpenAxisLVWideRR;		// pen for low volume railroads
	wxPen* hpenAlliedLVWideRR;		// pen for low volume railroads
	wxPen* hpenAxisRailRoad;		// pen for railroads
	wxPen* hpenAlliedRailRoad;		// pen for railroads
	wxPen* hpenAxisLVRailRoad;		// pen for low volume railroads
	wxPen* hpenAlliedLVRailRoad;	// pen for low volume railroads
	wxPen* hpenEscarpment;			// guess twice
	wxPen* hpenImpEscarpment;
	wxPen* hpenFort;				// fortified hexside

	wxBitmap* hbmWWaterIce;
	wxBitmap* hbmSWWaterIce;
	wxBitmap* hbmSEWaterIce;
#endif
	wxBitmap* hbmWMinRivIce;	// when wz is frozen
	wxBitmap* hbmSWMinRivIce;	// "
	wxBitmap* hbmSEMinRivIce;	// "
	wxBitmap* hbmWMajRivIce;	// "
	wxBitmap* hbmSWMajRivIce;	// "
	wxBitmap* hbmSEMajRivIce;	// "

	wxBitmap* hbmInfoBox;		// infobox in NO_GAME mode
	wxBitmap* hbmAirUnit;		// until something more clever is done...

	wxBitmap* hbmTarget;
	wxBitmap* hbmAirTarget;
	wxBitmap* hbmHit;
	wxBitmap* hbmFort;
	wxBitmap* hbmPermAF;
	wxBitmap* hbmTempAF;
	wxBitmap* hbmSovShip;
	wxBitmap* hbmAxisShip;

	wxBitmap* hbmArmCorps;
	wxBitmap* hbmInfCorps;
	wxBitmap* hbmMecCorps;
	wxBitmap* hbmMxdCorps;

	wxBitmap* hbmNoSupply;  	// allied hex out of supply
	wxBitmap* hbmAxNoSupply;	// axis hex out of supply
	wxBitmap* hbmSpcSupply;
	wxBitmap* hbmAxSpcSupply;
	wxBitmap* hbmIsolated;
	wxBitmap* hbmAxIsolated;
	wxBitmap* hbmMapNote;
	wxBitmap* hbmMapRP;
	wxBitmap* hbmMapSP;
	wxBitmap* hbmMapHit;
	wxBitmap* hbmMapHitLite;
	// unit graphics are in subtypes.h

#endif // HEXTOOLSPLAY public drawing data

	DECLARE_EVENT_TABLE()
};

#endif
