#if defined HEXTOOLSMAP

#include <iostream>

#include <wx/wx.h>

#include "hextools.h"
#include "terrainselector.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "hexsidetype.h"

enum
{
	CM_UNSETTERRAINSELECTION = 1,
	CM_TERRAINCLEAR,
	CM_TERRAINFOREST,
	CM_TERRAINLAKE,
	CM_TERRAINMOUNTAIN,
	CM_TERRAINHIGHMOUNTAIN,
	CM_TERRAINROUGH,
	CM_TERRAINSEA,
	CM_TERRAINSWAMP,
	CM_TERRAINWOODEDROUGH,
	CM_TERRAINWOODS,
	CM_TERRAINLIMITEDSTACKINGSIDE,
	CM_TERRAINMAJOR_RIVER,
	CM_TERRAINMINOR_RIVER,
	CM_TERRAINSTANDALONE_BRIDGE,
	CM_TERRAINSEASONAL_RIVER,
	CM_TERRAINGREAT_RIVER_FORD,
	CM_TERRAINMAJOR_RIVER_FORD,
	CM_TERRAINROAD,
	CM_TERRAINRAILROAD,
	CM_TERRAINRAILFERRY,
	CM_TERRAINNARROW_STRAITS,
	CM_TERRAINNARROW_STRAITS_FERRY,
	CM_TERRAINALL_WATER,
	CM_TERRAINBOCAGE,
	CM_TERRAINCANALS,
	CM_TERRAINESCARPMENT,
	CM_TERRAINHIGHMOUNTAINSIDE,
	CM_TERRAINHIGHMTNPASS,
	CM_TERRAINKARST,
	CM_TERRAINMOUNTAINSIDE,
	CM_TERRAINMOUNTAINSIDE2, // the interior hexside version
	CM_TERRAINWOODED_LAKE,
	CM_TERRAINERASE,
	CM_TERRAINDEMARCATIONLINE,
	CM_TERRAINBORDER,
	CM_TERRAININTERNALBORDER,
	CM_TERRAININTERNALSUBBORDER,
	CM_TERRAINFORTIFIED,
	CM_TERRAINCLACIERSIDE,
	CM_TERRAINILOWVOLRAILROAD,
	CM_TERRAINIMPASSABLEESCARPMENT,
	CM_TERRAINDOUBLEESCARPMENT,
	CM_TERRAINIMPASSABLEDOUBLEESCARPMENT,
	CM_TERRAINSALTDESERTSIDE,
	CM_TERRAINFILLINGRESERVOIRSIDE,
	CM_TERRAINWADI,
	CM_TERRAINGLACIER,
	CM_TERRAINSWAMPYTUNDRA,
	CM_TERRAINRAVINES,
	CM_TERRAINEXTREMERAVINES,
	CM_TERRAINRESTRICTEDWATERS,
	CM_TERRAINSALTDESERT,
	CM_TERRAINSALTLAKE,
	CM_TERRAINSALTMARSH,
	CM_TERRAINSAND,
	CM_TERRAINSTONYDESERT,
	CM_TERRAINWOODEDSWAMP,
	CM_TERRAINKILL,
	CM_TERRAINNONE,
	CM_TERRAINSEASIDE,
	CM_TERRAINSEARESTRSIDE,
	CM_TERRAINICINGSEASIDE,
	CM_TERRAINICINGSEARESTRSIDE,
	CM_TERRAINSWATERWAYROUTE,
	CM_TERRAININLANDSHIPROUTE,
	CM_TERRAINWATERWAYBAR,
	CM_TERRAINCANAL,
	CM_TERRAINMINOR_CANAL,
	CM_TERRAINJUNGLE,
	CM_TERRAINJUNGLEROUGH,
	CM_TERRAINJUNGLEMTN,
	CM_TERRAINJUNGLESWAMP,
	CM_TERRAINFILLINGRESERVOIR,
	CM_TERRAINATOLL,
	CM_TERRAINSMALLISLAND,
	CM_TERRAININTIRRIGATION,
	CM_TERRAINSEASONALSWAMP,
	CM_TERRAINHEAVYBUSH,
	CM_TERRAINROCKYTUNDRA,
	CM_TERRAINICINGSEA,
	CM_TERRAINICINGSEARESTR,
	CM_TERRAINSHIPCHANNEL,
	CM_TERRAINLAKESEAFERRY,
	CM_TERRAINTRACK,
	CM_TERRAINMOTORTRACK,
	CM_TERRAINRAILTRACK,
	CM_GWALL,
	CM_CLEARSIDE,
	CM_ROUGH_SIDE,
	CM_WOODS_SIDE,
	CM_FOREST_SIDE,
	CM_WOODEDROUGH_SIDE,
	CM_TERRAINSPECIALSWAMP,
	CM_TERRAINSPECIALSEASONALSWAMP
};

DECLARE_EVENT_TYPE(wxEVT_UNSELECT_TERRAINTYPE, -1)

BEGIN_EVENT_TABLE(TerrainSelector, wxScrolledWindow)
EVT_COMMAND(-1, wxEVT_UNSELECT_TERRAINTYPE, TerrainSelector::CmUnsetTerrain)
EVT_RADIOBUTTON(CM_TERRAINCLEAR, TerrainSelector::CmTerrainClear)
EVT_RADIOBUTTON(CM_TERRAINFOREST, TerrainSelector::CmTerrainForest)
EVT_RADIOBUTTON(CM_TERRAINLAKE, TerrainSelector::CmTerrainLake)
EVT_RADIOBUTTON(CM_TERRAINMOUNTAIN, TerrainSelector::CmTerrainMountain)
EVT_RADIOBUTTON(CM_TERRAINHIGHMOUNTAIN, TerrainSelector::CmTerrainHighMountain)
EVT_RADIOBUTTON(CM_TERRAINROUGH, TerrainSelector::CmTerrainRough)
EVT_RADIOBUTTON(CM_TERRAINSEA, TerrainSelector::CmTerrainSea)
EVT_RADIOBUTTON(CM_TERRAINSWAMP, TerrainSelector::CmTerrainSwamp)
EVT_RADIOBUTTON(CM_TERRAINWOODEDROUGH, TerrainSelector::CmTerrainWoodedRough)
EVT_RADIOBUTTON(CM_TERRAINWOODS, TerrainSelector::CmTerrainWoods)
EVT_RADIOBUTTON(CM_TERRAINLIMITEDSTACKINGSIDE, TerrainSelector::CmTerrainLimitedStackingSide)
EVT_RADIOBUTTON(CM_TERRAINMAJOR_RIVER, TerrainSelector::CmTerrainMajorRiver)
EVT_RADIOBUTTON(CM_TERRAINMINOR_RIVER, TerrainSelector::CmTerrainMinorRiver)
EVT_RADIOBUTTON(CM_TERRAINSTANDALONE_BRIDGE, TerrainSelector::CmTerrainStandaloneBridge)
EVT_RADIOBUTTON(CM_TERRAINSEASONAL_RIVER, TerrainSelector::CmTerrainSeasonalRiver)
EVT_RADIOBUTTON(CM_TERRAINGREAT_RIVER_FORD, TerrainSelector::CmTerrainFordableGreatRiver)
EVT_RADIOBUTTON(CM_TERRAINMAJOR_RIVER_FORD, TerrainSelector::CmTerrainFordableMajorRiver)
EVT_RADIOBUTTON(CM_TERRAINTRACK, TerrainSelector::CmTerrainTrack)
EVT_RADIOBUTTON(CM_TERRAINMOTORTRACK, TerrainSelector::CmTerrainMotorTrack)
EVT_RADIOBUTTON(CM_TERRAINROAD, TerrainSelector::CmTerrainRoad)
EVT_RADIOBUTTON(CM_TERRAINRAILTRACK, TerrainSelector::CmTerrainRailTrack)
EVT_RADIOBUTTON(CM_TERRAINRAILROAD, TerrainSelector::CmTerrainRailRoad)
EVT_RADIOBUTTON(CM_TERRAINNARROW_STRAITS, TerrainSelector::CmTerrainNarrowStraits)
EVT_RADIOBUTTON(CM_TERRAINNARROW_STRAITS_FERRY, TerrainSelector::CmTerrainNarrowStraitsFerry)
EVT_RADIOBUTTON(CM_TERRAINALL_WATER, TerrainSelector::CmAllWater)
EVT_RADIOBUTTON(CM_TERRAINBOCAGE, TerrainSelector::CmTerrainBocage)
EVT_RADIOBUTTON(CM_TERRAINCANALS, TerrainSelector::CmTerrainCanals)
EVT_RADIOBUTTON(CM_TERRAINESCARPMENT, TerrainSelector::CmTerrainEscarpment)
EVT_RADIOBUTTON(CM_TERRAINDOUBLEESCARPMENT, TerrainSelector::CmTerrainDoubleEscarpment)
EVT_RADIOBUTTON(CM_TERRAINFORTIFIED, TerrainSelector::CmTerrainFortified)
EVT_RADIOBUTTON(CM_TERRAINHIGHMOUNTAINSIDE, TerrainSelector::CmTerrainHighMountainSide)
EVT_RADIOBUTTON(CM_TERRAINHIGHMTNPASS, TerrainSelector::CmTerrainHighMntPass)
EVT_RADIOBUTTON(CM_TERRAINKARST, TerrainSelector::CmTerrainKarst)
EVT_RADIOBUTTON(CM_TERRAINMOUNTAINSIDE, TerrainSelector::CmTerrainMountainSide)
EVT_RADIOBUTTON(CM_TERRAINMOUNTAINSIDE2, TerrainSelector::CmTerrainMountainSideInterior)
EVT_RADIOBUTTON(CM_TERRAINWOODED_LAKE, TerrainSelector::CmTerrainWoodedLake)
EVT_RADIOBUTTON(CM_TERRAINERASE, TerrainSelector::CmErase)
/*
EVT_RADIOBUTTON(CM_TERRAINDEMARCATIONLINE, InfoPane::CmTerrainDemarcationLine)
EVT_RADIOBUTTON(CM_TERRAINBORDER, InfoPane::CmTerrainBorder)
EVT_RADIOBUTTON(CM_TERRAININTERNALBORDER, InfoPane::CmInternalBorder)
EVT_RADIOBUTTON(CM_TERRAININTERNALSUBBORDER, InfoPane::CmInternalSubBorder)
*/
EVT_RADIOBUTTON(CM_TERRAINFORTIFIED, TerrainSelector::CmFortified)
EVT_RADIOBUTTON(CM_TERRAINCLACIERSIDE, TerrainSelector::CmGlacierSide)
EVT_RADIOBUTTON(CM_TERRAINILOWVOLRAILROAD, TerrainSelector::CmLowVolRailRoad)
EVT_RADIOBUTTON(CM_TERRAINIMPASSABLEESCARPMENT, TerrainSelector::CmTerrainImpassableEscarpment)
EVT_RADIOBUTTON(CM_TERRAINIMPASSABLEDOUBLEESCARPMENT, TerrainSelector::CmTerrainImpassableDoubleEscarpment)
EVT_RADIOBUTTON(CM_TERRAINSALTDESERTSIDE, TerrainSelector::CmSaltDesertSide)
EVT_RADIOBUTTON(CM_TERRAINFILLINGRESERVOIRSIDE, TerrainSelector::CmFillingReservoirSide)
EVT_RADIOBUTTON(CM_TERRAINWADI, TerrainSelector::CmTerrainWadi)
EVT_RADIOBUTTON(CM_TERRAINGLACIER, TerrainSelector::CmGlacier)
EVT_RADIOBUTTON(CM_TERRAINSWAMPYTUNDRA, TerrainSelector::CmSwampyTundra)
EVT_RADIOBUTTON(CM_TERRAINRAVINES, TerrainSelector::CmRavines)
EVT_RADIOBUTTON(CM_TERRAINEXTREMERAVINES, TerrainSelector::CmExtremeRavines)
EVT_RADIOBUTTON(CM_TERRAINSALTDESERT, TerrainSelector::CmSaltDesert)
EVT_RADIOBUTTON(CM_TERRAINSALTLAKE, TerrainSelector::CmSaltLake)
EVT_RADIOBUTTON(CM_TERRAINSALTMARSH, TerrainSelector::CmSaltMarsh)
EVT_RADIOBUTTON(CM_TERRAINSAND, TerrainSelector::CmSand)
EVT_RADIOBUTTON(CM_TERRAINSTONYDESERT, TerrainSelector::CmStonyDesert)
EVT_RADIOBUTTON(CM_TERRAINWOODEDSWAMP, TerrainSelector::CmWoodedSwamp)
EVT_RADIOBUTTON(CM_TERRAINKILL, TerrainSelector::CmKill)
EVT_RADIOBUTTON(CM_TERRAINNONE, TerrainSelector::CmNone)
EVT_RADIOBUTTON(CM_TERRAINCANAL, TerrainSelector::CmTerraincanal)
EVT_RADIOBUTTON(CM_TERRAINMINOR_CANAL, TerrainSelector::CmTerrainminorcanal)
EVT_RADIOBUTTON(CM_TERRAINJUNGLE, TerrainSelector::CmTerrainjungle)
EVT_RADIOBUTTON(CM_TERRAINJUNGLEROUGH, TerrainSelector::CmTerrainjunglerough)
EVT_RADIOBUTTON(CM_TERRAINJUNGLEMTN, TerrainSelector::CmTerrainjunglemtn)
EVT_RADIOBUTTON(CM_TERRAINJUNGLESWAMP, TerrainSelector::CmTerrainjungleswamp)
//EVT_RADIOBUTTON(CM_TERRAINATOLL, InfoPane::CmTerrainatoll)
//EVT_RADIOBUTTON(CM_TERRAINSMALLISLAND, InfoPane::CmTerrainsmallisland)
EVT_RADIOBUTTON(CM_TERRAININTIRRIGATION, TerrainSelector::CmTerrainirrigation)
EVT_RADIOBUTTON(CM_TERRAINSEASONALSWAMP, TerrainSelector::CmTerrainseasonalswamp)
EVT_RADIOBUTTON(CM_TERRAINHEAVYBUSH, TerrainSelector::CmTerrainheavybush)
EVT_RADIOBUTTON(CM_TERRAINROCKYTUNDRA, TerrainSelector::CmTerrainrockytundra)
EVT_RADIOBUTTON(CM_TERRAINICINGSEA, TerrainSelector::CmIcingsea)
EVT_RADIOBUTTON(CM_CLEARSIDE, TerrainSelector::CmClearSide)
EVT_RADIOBUTTON(CM_GWALL, TerrainSelector::CmGWall)
EVT_RADIOBUTTON(CM_ROUGH_SIDE, TerrainSelector::CmRoughSide)
EVT_RADIOBUTTON(CM_WOODS_SIDE, TerrainSelector::CmWoodsSide)
EVT_RADIOBUTTON(CM_FOREST_SIDE, TerrainSelector::CmForestSide)
EVT_RADIOBUTTON(CM_WOODEDROUGH_SIDE, TerrainSelector::CmWoodedRoughSide)
EVT_RADIOBUTTON(CM_TERRAINSPECIALSWAMP, TerrainSelector::CmTerrainspecialswamp)
EVT_RADIOBUTTON(CM_TERRAINSPECIALSEASONALSWAMP, TerrainSelector::CmTerrainspecialseasonalswamp)
EVT_RADIOBUTTON(CM_TERRAINFILLINGRESERVOIR, TerrainSelector::CmTerrainfillingreservoir)
EVT_RADIOBUTTON(CM_TERRAINSWATERWAYROUTE, TerrainSelector::CmWaterwayRoute)
EVT_RADIOBUTTON(CM_TERRAININLANDSHIPROUTE, TerrainSelector::CmInlandPortShippingRoute)
EVT_RADIOBUTTON(CM_TERRAINWATERWAYBAR, TerrainSelector::CmBarToWaterwayMovement)
EVT_RADIOBUTTON(CM_TERRAINSHIPCHANNEL, TerrainSelector::CmShippingChannel)
EVT_RADIOBUTTON(CM_TERRAINLAKESEAFERRY, TerrainSelector::CmLakeSeaFerry)
EVT_RADIOBUTTON(CM_TERRAINRAILFERRY, TerrainSelector::CmRailFerry)
EVT_RADIOBUTTON(CM_TERRAINSEASIDE, TerrainSelector::CmTerrainseaside)
EVT_RADIOBUTTON(CM_TERRAINSEARESTRSIDE, TerrainSelector::CmTerrainsearestrside)
EVT_RADIOBUTTON(CM_TERRAINICINGSEASIDE, TerrainSelector::CmTerrainicingseaside)
EVT_RADIOBUTTON(CM_TERRAINICINGSEARESTRSIDE, TerrainSelector::CmTerrainicingsearestrside)
END_EVENT_TABLE()

TerrainSelector::TerrainSelector( wxWindow *p, int i, const wxPoint& pt, const wxSize& size, long WXUNUSED(l) )
		: wxScrolledWindow( p, i, pt, size, wxTAB_TRAVERSAL, "testwin" )
{
	int y = 1;
	int s = 17;  // y = 120;
	wxSize sz( 250, 18 );

	new wxRadioButton( this, CM_TERRAINCLEAR, "Clear", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINROUGH, "Rough", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINWOODS, "Woods", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINWOODEDROUGH, "Wooded Rough", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINFOREST, "Forest", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINMOUNTAIN, "Mountain", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSWAMP, "Swamp", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSEASONALSWAMP, "Seasonal Swamp", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSPECIALSWAMP, "Special Swamp", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSPECIALSEASONALSWAMP, "Special Seasonal Swamp", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINFILLINGRESERVOIR, "Filling Reservoir", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINWOODEDSWAMP, "Wooded Swamp", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSWAMPYTUNDRA, "Swampy Tundra", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINROCKYTUNDRA, "Rocky Tundra", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINCANALS, "Canal Intensive", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAININTIRRIGATION, "Intensive Irrigation", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINLAKE, "Intermittent Lake", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSALTLAKE, "Intermittent Salt Lake", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINWOODED_LAKE, "Wooded Intermittent Lake", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINBOCAGE, "Bocage", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINGLACIER, "Glacier", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINHIGHMOUNTAIN, "High Mountain", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINRAVINES, "Ravines", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINEXTREMERAVINES, "Extreme Ravines", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSAND, "Sand", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSTONYDESERT, "Stony Desert", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSALTDESERT, "Salt Desert", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSALTMARSH, "Salt Marsh", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINHEAVYBUSH, "Heavy Bush", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINJUNGLE, "Jungle", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINJUNGLEROUGH, "Jungle Rough", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINJUNGLEMTN, "Jungle Mountain", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINJUNGLESWAMP, "Jungle Swamp", wxPoint( 20, y += s ), sz );
	//new wxRadioButton( this, CM_TERRAINATOLL, "Atoll", wxPoint( 20, y += s ), sz );
	//new wxRadioButton( this, CM_TERRAINSMALLISLAND, "Small Island", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSEA, "All Sea/Lake", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINICINGSEA, "All Sea/Lake (can ice)", wxPoint( 20, y += s ), sz );
	y += 7;  // extra space
	new wxRadioButton( this, CM_TERRAINMINOR_CANAL, "Minor Canal", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINCANAL, "Canal", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINWADI, "Wadi", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSEASONAL_RIVER, "Seasonal River", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINMINOR_RIVER, "River", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINMAJOR_RIVER, "Major River", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINMAJOR_RIVER_FORD, "Major River w/ Ford", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINALL_WATER, "Lake/Great River", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINGREAT_RIVER_FORD, "Great River w/ Ford", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSEARESTRSIDE, "Sea (can be restricted)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSEASIDE, "Sea (non-restricted)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINICINGSEARESTRSIDE, "Sea (can ice, can be restricted)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINICINGSEASIDE, "Sea (can ice, non-restricted)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSWATERWAYROUTE, "Waterway Route", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAININLANDSHIPROUTE, "Inland Port Route", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINWATERWAYBAR, "Bar to Waterway Movement", wxPoint( 20, y += s ), sz );
	y += 7;  // extra space
	new wxRadioButton( this, CM_TERRAINTRACK, "Track", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINMOTORTRACK, "Motor Track", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINROAD, "Road", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSTANDALONE_BRIDGE, "Standalone Bridge", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINRAILTRACK, "Rail Track", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINRAILROAD, "Railroad", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINRAILFERRY, "Rail Ferry", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINNARROW_STRAITS, "Narrow Straits", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINNARROW_STRAITS_FERRY, "Narrow Straits Ferry", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINLAKESEAFERRY, "Lake/Sea Ferry", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSHIPCHANNEL, "Shipping Channel", wxPoint( 20, y += s ), sz );
	y += 7;  // extra space
	new wxRadioButton( this, CM_TERRAINLIMITEDSTACKINGSIDE, "Limited Stacking", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINMOUNTAINSIDE, "Mountain", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINKARST, "Karst", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINHIGHMOUNTAINSIDE, "High Mountain", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINHIGHMTNPASS, "High Mountain Pass", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINCLACIERSIDE, "Glacier", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINSALTDESERTSIDE, "Salt Desert", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINFILLINGRESERVOIRSIDE, "Filling Reservoir", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINDOUBLEESCARPMENT, "Double Escarpment", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINIMPASSABLEDOUBLEESCARPMENT, "Impassable Double Escarpment", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINMOUNTAINSIDE2, "Mountain (along River/Wadi)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINESCARPMENT, "Escarpment", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINIMPASSABLEESCARPMENT, "Impassable Escarpment", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINFORTIFIED, "Fortified", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_GWALL, "Great Wall", wxPoint( 20, y += s ), sz );
	/*
	new wxRadioButton( this, CM_TERRAINBORDER, "International Border", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAININTERNALBORDER, "Internal Border", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAININTERNALSUBBORDER, "Internal Sub-Border", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINDEMARCATIONLINE, "Int'l/Int Border Demarcation", wxPoint( 20, y += s ), sz );
	*/
	y += 7;  //extra space
	new wxRadioButton( this, CM_CLEARSIDE, "Clear (decorative)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_ROUGH_SIDE, "Rough (decorative)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_WOODS_SIDE, "Woods (decorative)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_FOREST_SIDE, "Forest (decorative)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_WOODEDROUGH_SIDE, "Wooded Rough (decorative)", wxPoint( 20, y += s ), sz );
	y += 7;  //extra space
	new wxRadioButton( this, CM_TERRAINNONE, "Erase hex terrain (only)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINERASE, "Erase hexside (terr., feat.)", wxPoint( 20, y += s ), sz );
	new wxRadioButton( this, CM_TERRAINKILL, "Kill hex and hexsides (ALL)", wxPoint( 20, y += s ), sz );

	int yUnitsInPixels = 11;
	SetScrollbars( 1, yUnitsInPixels, 1, y / yUnitsInPixels + 3 ); // + 3 for top and bottom space
	parent_window_ = p;
	selected_terrain_type_ = HexType::TERRAINTYPECOUNT; // by default no selection, and thus no painting on left click
}

TerrainSelector::~TerrainSelector( )
{
}

int TerrainSelector::getSelectedType( )
{
	return selected_terrain_type_;
}

void TerrainSelector::setSelectedType( int type )
{
	selected_terrain_type_ = type;
}

void TerrainSelector::CmUnsetTerrain( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::TERRAINTYPECOUNT;
}

void TerrainSelector::CmTerrainClear( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::CLEAR;
}

void TerrainSelector::CmTerrainForest( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::FOREST;
}

void TerrainSelector::CmTerrainLake( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::LAKE;
}

void TerrainSelector::CmTerrainMountain( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::MOUNTAIN;
}

void TerrainSelector::CmTerrainHighMountain( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::HIGHMOUNTAIN;
}

void TerrainSelector::CmTerrainRough( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::ROUGH;
}

void TerrainSelector::CmTerrainSea( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SEA;
}

void TerrainSelector::CmTerrainSwamp( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SWAMP;
}

void TerrainSelector::CmTerrainWoodedRough( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::WOODEDROUGH;
}

void TerrainSelector::CmTerrainWoods( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::WOODS;
}

void TerrainSelector::CmTerrainLimitedStackingSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::LIMITED_STACKING_SIDE;
}

void TerrainSelector::CmTerrainMajorRiver( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::MAJORRIVER;
}

void TerrainSelector::CmTerrainMinorRiver( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::MINORRIVER;
}

void TerrainSelector::CmTerrainStandaloneBridge( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::STANDALONEBRIDGE;
}

void TerrainSelector::CmTerrainSeasonalRiver( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::SEASONALRIVER;
}

void TerrainSelector::CmTerrainFordableGreatRiver( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::GREATRIVERFORD;
}

void TerrainSelector::CmTerrainFordableMajorRiver( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::MAJORRIVERFORD;
}

void TerrainSelector::CmTerrainTrack( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::TRACK;
}

void TerrainSelector::CmTerrainMotorTrack( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::MOTORTRACK;
}

void TerrainSelector::CmTerrainRoad( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::ROAD;
}

void TerrainSelector::CmTerrainRailTrack( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::RAILTRACK;
}

void TerrainSelector::CmTerrainRailRoad( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::RAILROAD;
}

void TerrainSelector::CmRailFerry( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::RAIL_FERRY;
}

void TerrainSelector::CmTerrainNarrowStraits( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::NARROW_STRAITS;
}

void TerrainSelector::CmTerrainNarrowStraitsFerry( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::NARROW_STRAITS_FERRY;
}

void TerrainSelector::CmAllWater( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::LAKE_SIDE;
}

void TerrainSelector::CmTerrainBocage( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::BOCAGE;
}

void TerrainSelector::CmTerrainCanals( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::CANALS;
}

void TerrainSelector::CmTerrainEscarpment( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::ESCARPMENT;
}

void TerrainSelector::CmTerrainDoubleEscarpment( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::DBL_ESCARPMENT;
}

void TerrainSelector::CmTerrainFortified( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::FORTIFIED;
}

void TerrainSelector::CmTerrainHighMountainSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::HIGH_MTN_SIDE;
}

void TerrainSelector::CmTerrainKarst( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::KARST;
}

void TerrainSelector::CmTerrainHighMntPass( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::HIGH_MTN_PASS;
}

void TerrainSelector::CmTerrainMountainSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::MTN_SIDE;
}

void TerrainSelector::CmTerrainMountainSideInterior( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::MTN_SIDE2;
}

void TerrainSelector::CmTerrainWoodedLake( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::WOODEDLAKE;
}

void TerrainSelector::CmErase( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::NOTHING;
}
#if 0
void InfoPane::CmTerrainDemarcationLine( wxCommandEvent& WXUNUSED(event) )
{
	iSelectedType = HexsideType::DEMARCATION;
}

void InfoPane::CmTerrainBorder( wxCommandEvent& WXUNUSED(event) )
{
	iSelectedType = HexsideType::BORDER;
}

void InfoPane::CmInternalBorder( wxCommandEvent& WXUNUSED(event) )
{
	iSelectedType = HexsideType::IBORDER;
}

void InfoPane::CmInternalSubBorder( wxCommandEvent& WXUNUSED(event) )
{
	iSelectedType = HexsideType::SUBIBORDER;
}
#endif
void TerrainSelector::CmWaterwayRoute( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::WATERWAY;
}

void TerrainSelector::CmInlandPortShippingRoute( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::INLAND_SHIPPING;
}

void TerrainSelector::CmBarToWaterwayMovement( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::WATERWAY_BAR;
}

void TerrainSelector::CmShippingChannel( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::SHIP_CHANNEL;
}

void TerrainSelector::CmLakeSeaFerry( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::LAKESEA_FERRY;
}

void TerrainSelector::CmFortified( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::FORTIFIED;
}

void TerrainSelector::CmGlacierSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::GLACIER_SIDE;
}

void TerrainSelector::CmLowVolRailRoad( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::LOW_VOL_RR;
}

void TerrainSelector::CmTerrainImpassableEscarpment( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::IMP_ESCARPMENT;
}

void TerrainSelector::CmTerrainImpassableDoubleEscarpment( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::IMP_DBL_ESCARPMENT;
}

void TerrainSelector::CmSaltDesertSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::SALT_DESERT_SIDE;
}

void TerrainSelector::CmFillingReservoirSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::FILLING_RESERVOIR_SIDE;
}

void TerrainSelector::CmTerrainWadi( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::WADI;
}

void TerrainSelector::CmGlacier( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::GLACIER;
}

void TerrainSelector::CmSwampyTundra( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SWAMPYTUNDRA;
}

void TerrainSelector::CmRavines( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::RAVINES;
}

void TerrainSelector::CmExtremeRavines( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::EXTREMERAVINES;
}

void TerrainSelector::CmSaltDesert( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SALTDESERT;
}

void TerrainSelector::CmSaltLake( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SALTLAKE;
}

void TerrainSelector::CmSaltMarsh( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SALTMARSH;
}

void TerrainSelector::CmSand( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SAND;
}

void TerrainSelector::CmStonyDesert( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::STONYDESERT;
}

void TerrainSelector::CmWoodedSwamp( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::WOODEDSWAMP;
}

void TerrainSelector::CmKill( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::HEXTYPEKILL;
	Refresh();	// TODO: refresh hack?  see ::CmNone comment
}

void TerrainSelector::CmNone( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::NONE;
	Refresh();	// TODO: am not sure why this refresh hack
				// is needed for terrain type None, but not
				// for any other terrain type
				// ...but, it works
}

void TerrainSelector::CmTerrainseaside( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::SEA_SIDE;
}

void TerrainSelector::CmTerrainsearestrside( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::RWATER_SIDE;
}

void TerrainSelector::CmTerrainicingseaside( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::ICING_SEA_SIDE;
}

void TerrainSelector::CmTerrainicingsearestrside( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::ICING_RWATER_SIDE;
}

void TerrainSelector::CmTerraincanal( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::CANAL;
}

void TerrainSelector::CmTerrainminorcanal( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::MINOR_CANAL;
}

void TerrainSelector::CmTerrainjungle( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::JUNGLE;
}

void TerrainSelector::CmTerrainjunglerough( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::JUNGLEROUGH;
}

void TerrainSelector::CmTerrainjunglemtn( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::JUNGLEMTN;
}

void TerrainSelector::CmTerrainjungleswamp( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::JUNGLESWAMP;
}

void TerrainSelector::CmTerrainatoll( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::ATOLL;
}

void TerrainSelector::CmTerrainsmallisland( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SMALLISLAND;
}

void TerrainSelector::CmTerrainirrigation( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::INTIRRIGATION;
}

void TerrainSelector::CmTerrainseasonalswamp( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SEASONALSWAMP;
}

void TerrainSelector::CmTerrainheavybush( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::HEAVYBUSH;
}

void TerrainSelector::CmTerrainrockytundra( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::ROCKYTUNDRA;
}

void TerrainSelector::CmTerrainspecialswamp( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SPECIALSWAMP;
}

void TerrainSelector::CmTerrainspecialseasonalswamp( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::SPECIALSEASONALSWAMP;
}

void TerrainSelector::CmTerrainfillingreservoir( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::FILLINGRESERVOIR;
}

void TerrainSelector::CmIcingsea( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexType::ICINGSEA;
}

void TerrainSelector::CmClearSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::CLEAR_HEXSIDE;
}

void TerrainSelector::CmGWall( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::GREAT_WALL;
}

void TerrainSelector::CmRoughSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::ROUGH_SIDE;
}

void TerrainSelector::CmWoodsSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::WOODS_SIDE;
}

void TerrainSelector::CmForestSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::FOREST_SIDE;
}

void TerrainSelector::CmWoodedRoughSide( wxCommandEvent& WXUNUSED(event) )
{
	selected_terrain_type_ = HexsideType::WOODEDROUGH_SIDE;
}

#endif
