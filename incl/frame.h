#ifndef FRAME_H
#define FRAME_H

class MapPane;
class HexInfoPane;
#if defined HEXTOOLSMAP
class TerrainSelector;
#elif defined HEXTOOLSPLAY
class HexContentsPane;
#endif

class Frame : public wxFrame
{
#if defined HEXTOOLSPLAY
public:
	struct MenuEvent
	{
		enum
		{
			UNITNORTHEAST			=	1,
			UNITSOUTHEAST,			//	2
			UNITSOUTHWEST,			//	3
			UNITEAST,				//	4
			UNITWEST,				//	5
			UNITNORTHWEST,			//	6
			UNUSED_7,
			UNITHOME,				//	8
			UNUSED_9,
			UNUSED_10,
			UNUSED_11,
			UNUSED_12,
			UNITREGAUGERAIL,		//	13
			COMMANDSCHANGEOWNER,	//	14
			COMMANDSSELECTALL,		//	15
			COMMANDSDESELECTALL,	//	16
			MAKENEUTRAL,			//	17
			UNITTOTRAIN,			//	18
			UNITFROMTRAIN,			//	19
			REPAIR,					//	20
			TAKEOFF,				//	21
			LAND,					//	22
			BOMB,					//	23
			HEXOWNER,				//	24
			UNUSED_25_100,
			OPTIONSDISPLAY			=	101,
			UNUSED_102,							// was COMMANDSREPLAY
			COMMANDSENDPHASE,		//	103
			OPTIONSMAPINFO,			//	104
			UNUSED_105,
			UNUSED_106,
			UNITVIEW,				//	107
			UNUSED_108,
			HEXCREATEUNIT,			//	109
			HEXDELETEUNIT,			//	110
			HEXUNITMOVER,			//	111
			OPTIONSREPAINT,			//	112
			OPTIONSSETUPMODE,		//	113
			OPTIONSWEATHER,			//	114
			HEXEDIT_UNIT,			//	115
			HEXCREATEAIRUNIT,		//	116
			SCROLLUP,				//	117
			SCROLLDOWN,				//	118
			SCROLLLEFT,				//	119
			SCROLLRIGHT,			//	120
			HEXCREATEFORT,			//	121
			HEXCREATETEMPAF,		//	122
			HEXCREATEPERMAF,		//	123
			HEXREGAUGE,				//	124
			MOVEUP,					//	125
			MOVEDOWN,				//	126
			MOVELEFT,				//	127
			MOVERIGHT,				//	128
			CHANGESIDE,				//	129
			HEXINFO,				//	130
			HEXCOMBATINFO,			//	131
			HEXCREATESHIP,			//	132
			HEXHITRAIL,				//	133
			HEXHITAF,				//	134
			EVADMIN,					//	135
			RP,						//	136
			POSAA,					//	137
			EVTACTICAL,					//	138
			EVSTRATEGIC,				//	139
			COMBAT,					//	140
			SUPPLY,					//	141
			UNUSED_142_2000,
			HELPCONTENTS			=	2001,
			UNUSED_2002,
			UNUSED_2003,
			UNUSED_2004,
			UNUSED_2005,
			UNUSED_2006,
			UNUSED_2007,
			UNUSED_2008,
			HELPABOUT,				//	2009
			HELPHOMEPAGE,			//	2010
			UNUSED_2011_11841,
			CANCELATTACK			=	11842,
			CANCELAIR,				//	11843
			FIREAA,					//	11844
			RESOLVEBOMBING,			//	11845
			STAGE,					//	11846
			TELEPORT,				//	11847
			RESOLVEAIRCOMBAT,		//	11848
			RESTOREAFCAP,			//	11849
			OPTIONSRULES,			//	11850
			ADVANCE,				//	11851
			DESTROYUNIT,			//	11852
			REPLPOOL,				//	11853
			NAVALPATROL,			//	11854
			CAP,					//	11855
			ACTIVATEAIR,			//	11856
			SETAFHITS,				//	11857
			SETAFUSAGE,				//	11858
			SETPORTHITS,			//	11859
			SETPORTUSAGE,			//	11860
			UNUSED_11861,
			SETHARASSMENT,			//	11862
			DESTROYPORT,			//	11863
			REPAIRPORT,				//	11864
			SETCDHITS,				//	11865
			CHGSUPPLY,				//	11866
			CHGISOLATED,			//	11867
			TIMEJUMP,				//	11868
			UPDATESUPPLY,			//	11869
			SPECIALSUPPLY,			//	11870
			ISOLATED,				//	11871
			UNUSED_11872,
			CANCELORDER,			//	11873
			DELNOTE,				//	11874
			ADDNOTE,				//	11875
			ADDSTATUSMARKER,		//	11876
			UNUSED_11877,						// was SETAXMINES
			UNUSED_11878,
			UNUSED_11879,
			CALCALLIED,				//	11880
			CALCAXIS,				//	11881
			UNUSED_11882,
			AXISLOSS,				//	11883
			SHOWATTACKMARKERS,		//	11884
			REPAIRBRIDGE,			//	11885
			BREAKBRIDGE,			//	11886
			AIRREPLPOOL,			//	11887
			SHOWPATH,				//	11888
			STACKINFO,				//	11889
			UNUSED_11890,
			UNUSED_11891,
			UNUSED_11892,
			UNUSED_11893,
			TOSHIP,					//	11894
			FROMSHIP,				//	11895
			MPPLUS,					//	11896
			MPMINUS,				//	11897
			UNUSED_11898,						// was LAYMINE
			UNUSED_11899,						// was SWEEPMINE
			UNUSED_11900,						// was SETALMINES or EDITVIEW
			MPMINUS30,				//	11901
			FROVERSTACK,			//	11902
			TOOVERSTACK,			//	11903
			HILITEPLANES,			//	11904
			HILITEARMOR,			//	11905
			HILITECADRES,			//	11906
			HILITEARTILLERY,		//	11907
			SETRE,					//	11908
			SHOWCRT,				//	11909
			DIEROLL,				//	11910
			CHGCRT,					//	11911
			HILITEWZA,				//	11912
			HILITEFLYINGPLANES,		//	11913
			HILITEWZB,				//	11914
			HILITEWZC,				//	11915
			HILITEWZD,				//	11916
			HILITEWZE,				//	11917
			HILITEWZF,				//	11918
			HILITEWZG,				//	11919
			CAPTOINTERCEPTION,		//	11920
			ATTACKSUP,				//	11921
			CAPTOESCORT,			//	11922
			UNUSED_11923,
			CHKSTACKS				=	11924,
			UNUSED_11925,
			UNUSED_11926,
			UNUSED_11927,
			UNUSED_11928,
			UNUSED_11929,
			HILITESTACKS,			//	11930
			HILITEMOT,				//	11931
			ADDMULBERRY,			//	11932
			HILITECONS,				//	11933
			HILITERRENG,			//	11934
			HILITEABORTED,			//	11935
			HILITETRUCKS,			//	11936
			ALLALLIED,				//	11937
			ALLAXIS,				//	11938
			ALLNEUTRAL,				//	11939
			VISIBLEAXIS,			//	11940
			VISIBLEALLIED,			//	11941
			VISIBLENEUTRAL,			//	11942
			FILEINSERT,				//	11943
			UNUSED_11944,
			UNUSED_11945,
			CHANGENATIONAXIS,		//	11946
			CHANGEUNITSIDE,			//	11947
			CHANGENATIONALLIED,		//	11948
			DIEROLL2,				//	11949
			DIEROLL100,				//	11950
			HILITEUNMOVED,			//	11951
			HILITENORMAL,			//	11952
			HILITEWIDE,				//	11953
			HILITEOWNNORMAL,		//	11954
			HILITEOWNWIDE,			//	11955
			HILITEENEMYZOC,			//	11956
			HILITEFULLENEMYZOC,		//	11957
			HILITEREDUCEDENEMYZOC,	//	11958
			UNUSED_11959,
			DIEROLL12,				//	11960
			UNUSED_11961_11990,
			ALLVIEWSON				=	11991,
			ALLVIEWSOFF,			//	11992
			VIEWTEXT,				//	11993
			UNUSED_11994,
			UNUSED_11995,
			UNUSED_11996,
			UNUSED_11997,
			UNUSED_11998,
			UNUSED_11999,
			UNUSED_12000,
			SHOWVIEW01				=	12001,
			SHOWVIEW02,				//	12002
			SHOWVIEW03,				//	12003
			SHOWVIEW04,				//	12004
			SHOWVIEW05,				//	12005
			SHOWVIEW06,				//	12006
			SHOWVIEW07,				//	12007
			SHOWVIEW08,				//	12008
			SHOWVIEW09,				//	12009
			SHOWVIEW10,				//	12010
			SHOWVIEW11,				//	12011
			SHOWVIEW12,				//	12012
			SHOWVIEW13,				//	12013
			SHOWVIEW14,				//	12014
			SHOWVIEW15,				//	12015
			SHOWVIEW16,				//	12016
			SHOWVIEW17,				//	12017
			SHOWVIEW18,				//	12018
			SHOWVIEW19,				//	12019
			SHOWVIEW20,				//	12020
			HILITEWZH1,				//	12021
			HILITEWZH2,				//	12022
			HILITEWZH3,				//	12023
			HILITEWZI1,				//	12024
			HILITEWZ1,				//	12025
			HILITEWZ2,				//	12026
			HILITEWZ3,				//	12027
			HILITEWZ4,				//	12028
			HILITEWZ5,				//	12029
			HILITEWZ6,				//	12030
			HILITEWZ7,				//	12031
			HILITEWZ8,				//	12032
			HILITEWZ9,				//	12033
			HILITEWZ10,				//	12034
			HILITEWZ11,				//	12035
			HILITEWZ12,				//	12036
			HILITEWZ13,				//	12037
			HILITEWZ14,				//	12038
			HILITEWZ15,				//	12039
			HILITEWZ16,				//	12040
			HILITEWZ17,				//	12041
			HILITEWZ18,				//	12042
			HILITEWZI2,				//	12043
			HILITEWZI3,				//	12044
			HILITEWZIS,				//	12045
			HILITEWZJ1,				//	12046
			HILITEWZJ2,				//	12047
			HILITEWZK,				//	12048
			HILITEWZL,				//	12049
			HILITEWZM,				//	12050
			HILITEWZN,				//	12051
			HILITEWZO,				//	12052
			HILITEWZP1,				//	12053
			HILITEWZP2,				//	12054
			HILITEWZQ,				//	12055
			UNUSED_12056_24309,
			EXIT					=	24310,
			UNUSED_24311_24330,
			FILENEW					=	24331,
			FILEOPEN,				//	24332
			FILESAVE,				//	24333
			FILESAVEAS,				//	24334
			UNUSED_24335,
			UNUSED_24336,
			UNUSED_24337,
			UNUSED_24338,
			UNUSED_24339,
			UNUSED_24340,
			UNUSED_24341,
			SEARCHUNIT,				//	24342
			SEARCHAGAIN,			//	24343
			UNUSED_24344,						//	was MENU_SEARCH
			MENU_HILITEWZ,			//	24345
			HILITEAIRBASE,			//	24346
			HILITEINOP,				//	24347
			MOVETOREPLPOOL,			//	24348
			ZOOMIN,					//	24349
			ZOOMOUT,				//	24350
			ZOOM100,				//	24351
			ALPARTISANS,			//	24352
			SETALLSIZES,			//	24353
			SETALLZOCS,				//	24354
			MENU_SET,				//	24355
			REMOVECMARKERS,			//	24356
			RECENT1,				//	24357
			RECENT2,				//	24358
			RECENT3,				//	24359
			RECENT4,				//	24360
			RECENT5,				//	24361
			ADDBARRAGE,				//	24362
			ADDCONTESTED,			//	24363
			MENU_MARKERS,			//	24364
			CHANGETEC,				//	24365
			EVNOT_DISRUPTED,			//	24366
			EVDISRUPTED,				//	24367
			BAD_DISRUPTED,			//	24368
			SHOW_DISRUPTED,			//	24369
			MENU_INTERCP,			//	24370
			SHOW_PAZONES,			//	24371
			SHOW_INTZONES1,			//	24372
			SHOW_INTZONES2,			//	24373
			SHOW_INTZONES3,			//	24374
			SHOW_INTZONES4,			//	24375
			SHOW_INTSOURCES,		//	24376
			SHOW_PASOURCES,			//	24377
			TOGGLE_DID_PA,			//	24378
			GAUGE_ALL,				//	24379
			GAUGE_VISIBLE,			//	24380
			ANALYZE_DIE,			//	24381
			REMOVE_DISRUPTION,		//	24382
			FIELDWORKS,				//	24383
			CLEAROBSTRUCTED,		//	24384
			UNUSED_24385,
			AXPARTISANS,			//	24386
			PEEKPARTISANS,			//	24387
			RAILUSAGE,				//	24388
			ADDALERT,				//	24389
			HIDEUNITS,				//	24390
			CHANGEORIGOWNER,		//	24391
			CHANGEORIGOWNERALL,		//	24392
			CHANGEORIGOWNERVISIBLE,	//	24393
			SHOWHITS,				//	24394
			CLEAR_ALL,				//	24395
			CLEAR_VISIBLE			//	24396
		};
	};
#endif

protected:
	bool loadFiles( const char* filename );
#if defined HEXTOOLSMAP
	void saveMap( );
#elif defined HEXTOOLSPLAY
	friend class Application;
	bool saveScenario( );
	wxString scnFileName;
#endif

public:
	static const std::string CLOSE_WO_SAVE;
	static const std::string FILE_TYPES;

	Frame( const wxString& title, const wxPoint& pos, const wxSize& size );

	wxToolBar* toolbar_;
	MapPane* map_;
	HexInfoPane* info_pane_;
#if defined HEXTOOLSMAP
	TerrainSelector* terrain_selector_;
#elif defined HEXTOOLSPLAY
	HexContentsPane* contentsPane;
#endif

	// event handlers (these functions should _not_ be virtual)
	void OnAbout( wxCommandEvent& event );
	void OnHelp( wxCommandEvent& event );

	void OnSize( wxSizeEvent& event );
	void OnCloseQuery( wxCloseEvent& event );	
	void OnQuit( wxCommandEvent& event );
	void OnOpen( wxCommandEvent& event );
	void OnSave( wxCommandEvent& event );
	void OnSaveAs( wxCommandEvent& event );

	void OnRepaint( wxCommandEvent& event );
	void OnZoomIn( wxCommandEvent& event );
	void OnZoomOut( wxCommandEvent& event );
	void OnZoom100( wxCommandEvent& event );

	wxToolBar* getToolbar( )
	{
		return toolbar_;
	}

	void setCaption( );

#if defined HEXTOOLSMAP
	void OnExport( wxCommandEvent& event );

	void OnWZMode( wxCommandEvent& event );
	void OnWZSelection( wxCommandEvent& event );
	void OnSeaboxMode( wxCommandEvent& event );
	void OnSeaboxSelection( wxCommandEvent& event );
	void OnDistrictMode( wxCommandEvent& event );
	void OnDistrictSelection( wxCommandEvent& event );
	void OnMapLeafMode( wxCommandEvent& event );
	void OnEndModes( wxCommandEvent& event );
#endif

#if defined HEXTOOLSPLAY
	void OnNew( wxCommandEvent& event );
	void OnInsert( wxCommandEvent& event );

	void setFileName( const wxString &s )
	{
		scnFileName = s;
	}

	void OnRecent1( wxCommandEvent& event );
	void OnRecent2( wxCommandEvent& event );
	void OnRecent3( wxCommandEvent& event );
	void OnRecent4( wxCommandEvent& event );
	void OnRecent5( wxCommandEvent& event );

	void CmUnitEast( wxCommandEvent& event );
	void CmUnitNorthEast( wxCommandEvent& event );
	void CmUnitNorthWest( wxCommandEvent& event );
	void CmUnitSouthEast( wxCommandEvent& event );
	void CmUnitSouthWest( wxCommandEvent& event );
	void CmUnitWest( wxCommandEvent& event );
	void CmUnitHome( wxCommandEvent& event );
	void CmUnitView( wxCommandEvent& event );
	void CmHexCreateUnit( wxCommandEvent& event );
	void CmHexDeleteUnit( wxCommandEvent& event );
	void CmEditUnit( wxCommandEvent& event );
	void CmWeather( wxCommandEvent& event );
	void CmHexUnitMover( wxCommandEvent& event );
	void CmRepaint( wxCommandEvent& event );
	void CmCreateAirUnit( wxCommandEvent& event );
	void CmOptionsInfo( wxCommandEvent& event );
	void CmScrollUp( wxCommandEvent& event );
	void CmScrollRight( wxCommandEvent& event );
	void CmScrollLeft( wxCommandEvent& event );
	void CmScrollDown( wxCommandEvent& event );
	void CmMoveDown( wxCommandEvent& event );
	void CmMoveLeft( wxCommandEvent& event );
	void CmMoveRight( wxCommandEvent& event );
	void CmMoveUp( wxCommandEvent& event );
	void CmCreateFort( wxCommandEvent& event );
	void CmPermAF( wxCommandEvent& event );
	void CmTempAF( wxCommandEvent& event );
	void CmHexInfo( wxCommandEvent& event );
	void CmRegauge( wxCommandEvent& event );
	void CmHexCombatInfo( wxCommandEvent& event );
	void CmChangeOwner( wxCommandEvent& event );
	void CmSelectAll( wxCommandEvent& event );
	void CmDeselectAll( wxCommandEvent& event );
	void CmMakeNeutral( wxCommandEvent& event );
	void CmHitRail( wxCommandEvent& event );
	void CmHitAF( wxCommandEvent& event );
	void CmToTrain( wxCommandEvent& event );
	void CmFromTrain( wxCommandEvent& event );
	void CmToship( wxCommandEvent& event );
	void CmFromship( wxCommandEvent& event );
	void CmRP( wxCommandEvent& event );
	void CmPosAA( wxCommandEvent& event );
	void CmCreateShip( wxCommandEvent& event );
	void CmRepair( wxCommandEvent& event );
	void CmToggleDidPA( wxCommandEvent& event );
	void CmBomb( wxCommandEvent& event );
	void CmLand( wxCommandEvent& event );
	void CmStage( wxCommandEvent& event );
	void CmTakeoff( wxCommandEvent& event );
	void CmResolvebombing( wxCommandEvent& event );
	void CmFireaa( wxCommandEvent& event );
	void CmRestoreafcap( wxCommandEvent& event );
	void CmCap( wxCommandEvent& event );
	void CmNavalpatrol( wxCommandEvent& event );
	void CmDestroyunit( wxCommandEvent& event );
	void CmMoveToReplPool( wxCommandEvent& event );
	void CmUnitregaugerail( wxCommandEvent& event );
	void CmCombat( wxCommandEvent& event );
	void CmAdvance( wxCommandEvent& event );
	void CmCancelattack( wxCommandEvent& event );
	void CmCancelair( wxCommandEvent& event );
	void CmResolveaircombat( wxCommandEvent& event );
	void CmReplpool( wxCommandEvent& event );
	void CmAirreplpool( wxCommandEvent& event );
	void CmActivateair( wxCommandEvent& event );
	void CmSetafhits( wxCommandEvent& event );
	void CmSetafusage( wxCommandEvent& event );
	void CmSetharassment( wxCommandEvent& event );
	void CmSetmines( wxCommandEvent& event );
	void CmSetalmines( wxCommandEvent& event );
	void CmSetporthits( wxCommandEvent& event );
	void CmSetportusage( wxCommandEvent& event );
	void CmDestroyport( wxCommandEvent& event );
	void CmSetcdhits( wxCommandEvent& event );
	void CmRepairport( wxCommandEvent& event );
	void CmSetsupport( wxCommandEvent& event );
	void CmSetdefsupport( wxCommandEvent& event );
	void CmMpminus( wxCommandEvent& event );
	void CmMpminus30( wxCommandEvent& event );
	void CmMpplus( wxCommandEvent& event );
	void CmTooverstack( wxCommandEvent& event );
	void CmFroverstack( wxCommandEvent& event );
	void CmChgsupply( wxCommandEvent& event );
	void CmIsolated( wxCommandEvent& event );
	void CmSpecialsupply( wxCommandEvent& event );
	void CmUpdatesupply( wxCommandEvent& event );
	void CmChgisolated( wxCommandEvent& event );
	void CmCancelorder( wxCommandEvent& event );
	void CmAddnote( wxCommandEvent& event );
	void CmAddalert( wxCommandEvent& event );
	void CmDelnote( wxCommandEvent& event );
	void CmAddstatusmarker( wxCommandEvent& event );
	void CmShowattackmarkers( wxCommandEvent& event );
	void CmCalcallied( wxCommandEvent& event );
	void CmCalcaxis( wxCommandEvent& event );
	void CmAxisloss( wxCommandEvent& event );
	void CmBreakbridge( wxCommandEvent& event );
	void CmRepairbridge( wxCommandEvent& event );
	void CmStackinfo( wxCommandEvent& event );
	void CmLaymine( wxCommandEvent& event );
	void CmSweepmine( wxCommandEvent& event );
	void CmSetre( wxCommandEvent& event );
	void CmShowcrt( wxCommandEvent& event );
	void CmHilitearmor( wxCommandEvent& event );
	void CmHiliteartillery( wxCommandEvent& event );
	void CmHilitecadres( wxCommandEvent& event );
	void CmHiliteunmoved( wxCommandEvent& event );
	void CmHiliteEnemyZOCs( wxCommandEvent& event );  // Enemy Full or Reduced ZOCs
	void CmHiliteFullEnemyZOCs( wxCommandEvent& event );
	void CmHiliteReducedEnemyZOCs( wxCommandEvent& event );
	void CmHiliteplanes( wxCommandEvent& event );
	void CmHiliteflyingplanes( wxCommandEvent& event );
	void CmHilitestacks( wxCommandEvent& event );
	void CmHiliteitemot( wxCommandEvent& event );
	void CmHilitecons( wxCommandEvent& event );
	void CmHiliterreng( wxCommandEvent& event );
	void CmHiliteaborted( wxCommandEvent& event );
	void CmHilitetrucks( wxCommandEvent& event );
	void CmHiliteNormalGaugeRail( wxCommandEvent& event );
	void CmHiliteWideGaugeRail( wxCommandEvent& event );
	void CmHiliteOwnNormalGaugeRail( wxCommandEvent& event );
	void CmHiliteOwnWideGaugeRail( wxCommandEvent& event );
	void CmHilitewza( wxCommandEvent& event );
	void CmHilitewzb( wxCommandEvent& event );
	void CmHilitewzc( wxCommandEvent& event );
	void CmHilitewzd( wxCommandEvent& event );
	void CmHilitewze( wxCommandEvent& event );
	void CmHilitewzf( wxCommandEvent& event );
	void CmHilitewzg( wxCommandEvent& event );
	void CmHilitewzh1( wxCommandEvent& event );
	void CmHilitewzh2( wxCommandEvent& event );
	void CmHilitewzh3( wxCommandEvent& event );
	void CmHilitewzi1( wxCommandEvent& event );
	void CmHilitewz1( wxCommandEvent& event );
	void CmHilitewz2( wxCommandEvent& event );
	void CmHilitewz3( wxCommandEvent& event );
	void CmHilitewz4( wxCommandEvent& event );
	void CmHilitewz5( wxCommandEvent& event );
	void CmHilitewz6( wxCommandEvent& event );
	void CmHilitewz7( wxCommandEvent& event );
	void CmHilitewz8( wxCommandEvent& event );
	void CmHilitewz9( wxCommandEvent& event );
	void CmHilitewz10( wxCommandEvent& event );
	void CmHilitewz11( wxCommandEvent& event );
	void CmHilitewz12( wxCommandEvent& event );
	void CmHilitewz13( wxCommandEvent& event );
	void CmHilitewz14( wxCommandEvent& event );
	void CmHilitewz15( wxCommandEvent& event );
	void CmHilitewz16( wxCommandEvent& event );
	void CmHilitewz17( wxCommandEvent& event );
	void CmHilitewz18( wxCommandEvent& event );
	void CmHilitewzi2( wxCommandEvent& event );
	void CmHilitewzi3( wxCommandEvent& event );
	void CmHilitewzis( wxCommandEvent& event );
	void CmHilitewzj1( wxCommandEvent& event );
	void CmHilitewzj2( wxCommandEvent& event );
	void CmHilitewzk( wxCommandEvent& event );
	void CmHilitewzl( wxCommandEvent& event );
	void CmHilitewzm( wxCommandEvent& event );
	void CmHilitewzn( wxCommandEvent& event );
	void CmHilitewzo( wxCommandEvent& event );
	void CmHilitewzp1( wxCommandEvent& event );
	void CmHilitewzp2( wxCommandEvent& event );
	void CmHilitewzq( wxCommandEvent& event );
	void CmHilitedisrupted( wxCommandEvent& event );
	void CmHiliteIntZones1( wxCommandEvent& event );
	void CmHiliteIntZones2( wxCommandEvent& event );
	void CmHiliteIntZones3( wxCommandEvent& event );
	void CmHiliteIntZones4( wxCommandEvent& event );
	void CmHiliteIntSources( wxCommandEvent& event );
	void CmHilitePASources( wxCommandEvent& event );
	void CmHilitePAZones( wxCommandEvent& event );
	void CmEditView( wxCommandEvent& event );
	void CmViewText( wxCommandEvent& event );
	void CmAllViewsOn( wxCommandEvent& event );
	void CmAllViewsOff( wxCommandEvent& event );
	void CmShowView01( wxCommandEvent& event );
	void CmShowView02( wxCommandEvent& event );
	void CmShowView03( wxCommandEvent& event );
	void CmShowView04( wxCommandEvent& event );
	void CmShowView05( wxCommandEvent& event );
	void CmShowView06( wxCommandEvent& event );
	void CmShowView07( wxCommandEvent& event );
	void CmShowView08( wxCommandEvent& event );
	void CmShowView09( wxCommandEvent& event );
	void CmShowView10( wxCommandEvent& event );
	void CmShowView11( wxCommandEvent& event );
	void CmShowView12( wxCommandEvent& event );
	void CmShowView13( wxCommandEvent& event );
	void CmShowView14( wxCommandEvent& event );
	void CmShowView15( wxCommandEvent& event );
	void CmShowView16( wxCommandEvent& event );
	void CmShowView17( wxCommandEvent& event );
	void CmShowView18( wxCommandEvent& event );
	void CmShowView19( wxCommandEvent& event );
	void CmShowView20( wxCommandEvent& event );
	void CmDieroll( wxCommandEvent& event );
	void CmDieroll2( wxCommandEvent& event );
	void CmDieroll12( wxCommandEvent& event );
	void CmDieroll100( wxCommandEvent& event );
	void CmGaugeAll( wxCommandEvent& event );
	void CmGaugeVisible( wxCommandEvent& event );
	void CmClearAll( wxCommandEvent& event );
	void CmClearVisible( wxCommandEvent& event );
	void CmAttacksup( wxCommandEvent& event );
	void CmCaptoescort( wxCommandEvent& event );
	void CmCaptointerception( wxCommandEvent& event );
	void CmChkstacks( wxCommandEvent& event );
	void CmAddmulberry( wxCommandEvent& event );
	void CmAllallied( wxCommandEvent& event );
	void CmAllaxis( wxCommandEvent& event );
	void CmAllneutral( wxCommandEvent& event );
	void CmVisibleallied( wxCommandEvent& event );
	void CmVisibleaxis( wxCommandEvent& event );
	void CmVisibleneutral( wxCommandEvent& event );
	void CmChangenationside( wxCommandEvent& event );
	void CmChangenationallied( wxCommandEvent& event );
	void CmChangeunitside( wxCommandEvent& event );
	void CmSettings( wxCommandEvent& event );
	void CmOptionsrules( wxCommandEvent& event );
	void CmEndPhase( wxCommandEvent& event );
	void CmSetupMode( wxCommandEvent& event );
	void CmCreateUnit( wxCommandEvent& event );
	void CmChangeSide( wxCommandEvent& event );
	void cmAdmin( wxCommandEvent& event );
	void CmStrategic( wxCommandEvent& event );
	void CmTactical( wxCommandEvent& event );
	void CmSupply( wxCommandEvent& event );
	void cmHexOwner( wxCommandEvent& event );
	void CmTeleport( wxCommandEvent& event );
	void CmTimejump( wxCommandEvent& event );
	void CmShowpath( wxCommandEvent& event );
	void CmChgcrt( wxCommandEvent& event );
	void CmChgtec( wxCommandEvent& event );
	void CmFileinsert( wxCommandEvent& event );
	void CmSearchHex( wxCommandEvent& event );
	void CmSearchCity( wxCommandEvent& event );
	void CmSearchUnit( wxCommandEvent& event );
	void CmSearchAgain( wxCommandEvent& event );
	void CmHiliteAirbase( wxCommandEvent& event );
	void CmHiliteInop( wxCommandEvent& event );
	void CmAxPartisans( wxCommandEvent& event );
	void CmAlPartisans( wxCommandEvent& event );
	void CmPeekPartisans( wxCommandEvent& event );
	void CmSetAllSizes( wxCommandEvent& event );
	void CmSetAllZOCs( wxCommandEvent& event );
	void CmRemoveCMarkers( wxCommandEvent& event );
	void CmAddcontested( wxCommandEvent& event );
	void CmAddBarrage( wxCommandEvent& event );
	void CmNotDisrupted( wxCommandEvent& event );
	void CmDisrupted( wxCommandEvent& event );
	void CmBadDisrupted( wxCommandEvent& event );
	void CmAnalyzeDie( wxCommandEvent& event );
	void CmRemoveDisruption( wxCommandEvent& event );
	void CmFieldworks( wxCommandEvent& event );
	void CmClearObstructed( wxCommandEvent& event );
	void CmRailUsage( wxCommandEvent& event );
	void CmHideUnits( wxCommandEvent& event );
	void CmChangeOrigOwner( wxCommandEvent& event );
	void CmChangeOrigOwnerAll( wxCommandEvent& event );
	void CmChangeOrigOwnerVisible( wxCommandEvent& event );
	void CmShowHits( wxCommandEvent& event );
	void CmChangeNationsSide( wxCommandEvent& event );

	void OnHomePage( wxCommandEvent& event );

	void OnSeazoneMode( wxCommandEvent& event );
	void OnSeazoneSelection( wxCommandEvent& event );
	void OnCountryMode( wxCommandEvent& event );
	void OnCountrySelection( wxCommandEvent& event );

	void SetupMenu( void );
	void UpdateViewData( );

	void SetMoveModeButtons( );
	void SetMarkerButtons( );

	void saveSettings( );  // Options->Settings stuff to file
	void loadSettings( );  // Options->Settings stuff from file
#endif

	void build_menu( void );
	void setup_toolbar( );

	wxArrayString seabox_strings_;
	wxArrayString land_district_strings_;

	WX_DECLARE_STRING_HASH_MAP( int, SeaCircles );
    WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, SeaCirclesInverted );
	WX_DECLARE_STRING_HASH_MAP( int, Districts ); // String hash, mapped to int
    WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, DistrictsInverted ); // int hash, mapped to String

	SeaCircles seacircles_;
	SeaCirclesInverted seacircles2_;
	Districts districts_;
	DistrictsInverted districts2_;

	int getSeaCircleDistrictId( wxString sc );
	wxString getSeaCircleDistrict( int scId );
	int getLandDistrictId( wxString dist );
	wxString getLandDistrict( int distId );

#if defined HEXTOOLSPLAY
	wxArrayString seazone_strings_;
	wxArrayString land_country_strings_;

	WX_DECLARE_STRING_HASH_MAP( int, SeaZones );
	WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, SeaZonesInverted );
	WX_DECLARE_STRING_HASH_MAP( int, Countries );
	WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, CountriesInverted );

	SeaZones seazones_;
	SeaZonesInverted seazones2_;
	Countries countries_;
	CountriesInverted countries2_;

	int getSeaZoneId( wxString sz );
	wxString getSeaZone( int szId );
	int getCountryId( wxString country );
	wxString getCountry( int countryId );
#endif

	DECLARE_EVENT_TABLE()
};

#endif
