#if defined HEXTOOLSMAP

#ifndef TERRAINSELECTOR_H
#define TERRAINSELECTOR_H

// Drawing mode radiobuttons
class TerrainSelector : public wxScrolledWindow
{
public:
	TerrainSelector( wxWindow* p, int i, const wxPoint& pt, const wxSize& sz, long l = 0 );
	~TerrainSelector( );

	// map reads this variable to find out selected terrain
	int getSelectedType( );
	void setSelectedType( int type );

protected:
	void CmTerrainClear( wxCommandEvent& event );
	void CmTerrainForest( wxCommandEvent& event );
	void CmTerrainLake( wxCommandEvent& event );
	void CmTerrainMountain( wxCommandEvent& event );
	void CmTerrainHighMountain( wxCommandEvent& event );
	void CmTerrainRough( wxCommandEvent& event );
	void CmTerrainSea( wxCommandEvent& event );
	void CmTerrainSwamp( wxCommandEvent& event );
	void CmTerrainWoodedRough( wxCommandEvent& event );
	void CmTerrainWoods( wxCommandEvent& event );
	void CmTerrainLimitedStackingSide( wxCommandEvent& event );
	void CmTerrainMajorRiver( wxCommandEvent& event );
	void CmTerrainMinorRiver( wxCommandEvent& event );
	void CmTerrainStandaloneBridge( wxCommandEvent& event );
	void CmTerrainSeasonalRiver( wxCommandEvent& event );
	void CmTerrainFordableGreatRiver( wxCommandEvent& event );
	void CmTerrainFordableMajorRiver( wxCommandEvent& event );
	void CmTerrainTrack( wxCommandEvent& event );
	void CmTerrainMotorTrack( wxCommandEvent& event );
	void CmTerrainRoad( wxCommandEvent& event );
	void CmTerrainRailTrack( wxCommandEvent& event );
	void CmTerrainRailRoad( wxCommandEvent& event );
	void CmRailFerry( wxCommandEvent& event );
	void CmTerrainNarrowStraits( wxCommandEvent& event );
	void CmTerrainNarrowStraitsFerry( wxCommandEvent& event );
	void CmAllWater( wxCommandEvent& event );
	void CmTerrainBocage( wxCommandEvent& event );
	void CmTerrainCanals( wxCommandEvent& event );
	void CmTerrainEscarpment( wxCommandEvent& event );
	void CmTerrainDoubleEscarpment( wxCommandEvent& event );
	void CmTerrainFortified( wxCommandEvent& event );
	void CmTerrainHighMountainSide( wxCommandEvent& event );
	void CmTerrainHighMntPass( wxCommandEvent& event );
	void CmTerrainKarst( wxCommandEvent& event );
	void CmTerrainMountainSide( wxCommandEvent& event );
	void CmTerrainMountainSideInterior( wxCommandEvent& event );
	void CmTerrainWoodedLake( wxCommandEvent& event );
	void CmErase( wxCommandEvent& event );
	/*
	void CmTerrainDemarcationLine( wxCommandEvent& event );
	void CmTerrainBorder( wxCommandEvent& event );
	void CmInternalBorder( wxCommandEvent& event );
	void CmInternalSubBorder( wxCommandEvent& event );
	*/
	void CmWaterwayRoute( wxCommandEvent& event );
	void CmInlandPortShippingRoute( wxCommandEvent& event );
	void CmBarToWaterwayMovement( wxCommandEvent& event );
	void CmShippingChannel( wxCommandEvent& event );
	void CmLakeSeaFerry( wxCommandEvent& event );
	void CmFortified( wxCommandEvent& event );
	void CmGlacierSide( wxCommandEvent& event );
	void CmLowVolRailRoad( wxCommandEvent& event );
	void CmTerrainImpassableEscarpment( wxCommandEvent& event );
	void CmTerrainImpassableDoubleEscarpment( wxCommandEvent& event );
	void CmSaltDesertSide( wxCommandEvent& event );
	void CmFillingReservoirSide( wxCommandEvent& event );
	void CmTerrainWadi( wxCommandEvent& event );
	void CmGlacier( wxCommandEvent& event );
	void CmSwampyTundra( wxCommandEvent& event );
	void CmRavines( wxCommandEvent& event );
	void CmExtremeRavines( wxCommandEvent& event );
	void CmSaltDesert( wxCommandEvent& event );
	void CmSaltLake( wxCommandEvent& event );
	void CmSaltMarsh( wxCommandEvent& event );
	void CmSand( wxCommandEvent& event );
	void CmStonyDesert( wxCommandEvent& event );
	void CmWoodedSwamp( wxCommandEvent& event );
	void CmKill( wxCommandEvent& event );
	void CmNone( wxCommandEvent& event );
	void CmTerrainseaside( wxCommandEvent& event );
	void CmTerrainsearestrside( wxCommandEvent& event );
	void CmTerrainicingseaside( wxCommandEvent& event );
	void CmTerrainicingsearestrside( wxCommandEvent& event );
	void CmTerraincanal( wxCommandEvent& event );
	void CmTerrainminorcanal( wxCommandEvent& event );
	void CmTerrainjungle( wxCommandEvent& event );
	void CmTerrainjunglerough( wxCommandEvent& event );
	void CmTerrainjunglemtn( wxCommandEvent& event );
	void CmTerrainjungleswamp( wxCommandEvent& event );
	void CmTerrainatoll( wxCommandEvent& event );
	void CmTerrainsmallisland( wxCommandEvent& event );
	void CmTerrainirrigation( wxCommandEvent& event );
	void CmTerrainseasonalswamp( wxCommandEvent& event );
	void CmTerrainheavybush( wxCommandEvent& event );
	void CmTerrainrockytundra( wxCommandEvent& event );
	void CmTerrainspecialswamp( wxCommandEvent& event );
	void CmTerrainspecialseasonalswamp( wxCommandEvent& event );
	void CmTerrainfillingreservoir( wxCommandEvent& event );
	void CmIcingsea( wxCommandEvent& event );
	void CmClearSide( wxCommandEvent& event );
	void CmGWall( wxCommandEvent& event );
	void CmRoughSide( wxCommandEvent& event );
	void CmWoodsSide( wxCommandEvent& event );
	void CmForestSide( wxCommandEvent& event );
	void CmWoodedRoughSide( wxCommandEvent& event );
	void CmUnsetTerrain( wxCommandEvent& event );

protected:
	wxWindow* parent_window_;
	int selected_terrain_type_;

	DECLARE_EVENT_TABLE()
};

#endif

#endif
