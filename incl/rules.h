#if defined HEXTOOLSPLAY
#ifndef RULES_H
#define RULES_H

class Rules								// IB = "integral boolean"
{
public:
	// game series rules //
#if 0
	int rules_variant_;					// 0=WW2, 1=WW1, 2=Variant A
#endif
	int Era;							// 0x1=ww2, 0x2=scw, 0x4=ww1
	int WW1MPs;							// IB:  0=ww2 MP table, 1=ww1 MP table
	int WW1Phases;						// IB:  0=no, 1=ww1 reaction combat&movement phases
	int WW1AirBaseCapacity;				// ~IB:  0=ww2, 1=ww1 airbase capacity system (very different)
	int WW1ExploitMPs;					// IB:  0=no, 1=ww1 cav/inf/etc. units have some MPs in expl. phase
	int RailsObstructed;				// IB:  0=no, 1=ww1 captured rail lines are obstructed

	// general rules //

	int FirstPlayer;
	// also, Frame::Turns, a global "turns/month" setting is stored here in .scn
	//int UnitTypeFlags;					// ww1 or ww2 unit types (UTF_WW2 or UTF_WW1 or both)?

	// RE-related rules //

	int AllowCustomREs;					// IB, can individual unit's RE size be changed from unit properties dlg box?
	int HideAxPartisans;				// IB, hide partisans from the enemy sight
	int HideAlPartisans;				// IB
	int HQRE2;							// RE sizes (.5 units --> 3RE == 6)
	int CompanyRE2;
	int BattalionRE2;
	int RegimentRE2;
	int BrigadeRE2;
	int CadreRE2;
	int DivGrpRE2;						// divisional grouping, NOT German divisiongruppe(=cadre)
	int DivisionRE2;
	int CorpsRE2;
	int ArmyRE2;

	// ZOC rules //

	int AllowCustomZOCs;				// IB, can individual unit's RE size be changed from unit properties dialog box?
	int BattalionZOC;					// normal (in supply or U1) ZOCs
	int BattalionU2ZOC;					// U2 (or worse) ZOCs
	int RegimentZOC;
	int RegimentU2ZOC;
	int BrigadeZOC;
	int BrigadeU2ZOC;
	int CadreZOC;
	int CadreU2ZOC;
	int DivGrpZOC;						// divisional grouping, NOT German divisiongruppe(=cadre)
	int DivGrpU2ZOC;
	int DivisionZOC;
	int DivisionU2ZOC;
	int CorpsZOC;
	int CorpsU2ZOC;

	// movement rules //

	int DisableMPCalc;					// IB, entirely disable computer MP calculation
	int OnlyClearAdmin;					// IB, can units use only clear hexes in admin move
	int ToTrain1MP;						// ~IB, 0=0 MPs to board train (in operative rail movement), 1=1 MP to board
	int ReducedZOCs;					// IB, _ALL_ units have reduced ZOCs (FitE opening turns)
	int SovietReducedZOCs;				// IB, _ALL Soviet units have reduced ZOCs (FitE opening turns)
	int NonPhasMove;					// IB, are non-phasing player's units allowed to move?
	int NoGaugeChk;						// IB, disable rail gauge checking (checking works best in FitE)
	int MP6Swamp;						// ~IB, 0=1/2MPs+1 c/m or art to enter swamp, 1=6MPs
	int AllowUxAdminMove;				// IB, 0=no admin. mvmt for unsupplied units, 1=admin. mvmt allowed
	int FortLevelMP;					// MPs needed to build a fortification level
	int PermAFMP;						// MPs needed to build a permanent airfield
	int TempAFMP;						// MPs needed to build temporary airfield
	int ZOCLeaveCost;					// MP cost to leave enemy ZOC
	int ZOC2ZOCCost;					// MP cost to move from enemy ZOC to enemy ZOC
	int RZOCLeaveCost;					// MP cost to leave enemy Reduced ZOC
	int RZOC2ZOCCost;					// MP cost to move from enemy Reduced ZOC to Reduced enemy ZOC
	int GermanCMZOCRule;				// IB, 1=german c/m units pay 1MP less for leaving/entering enemy ZOC
	int ZOCEnterMot;					// costs for entering enemy ZOC (only needed in FtF(?))
	int ZOCEnterNormal;
	int NoAlliedZOCAcrossBorders;		// IB, Add rule that Allied ZOC's do not extend across border
	int GermanRegaugeInExploitation;	// IB, Variant A: Add option to allow German RR Engineers to regauge in exploitation phase
	int SovietReactionPhase;			// IB, Add option that Soviets do not have a Reaction Phase
	int SovietExploitLimitation;		// IB, Variant A Soviet Exploitation Movement Limitation
	int RoadsCanBeBroken;				// IB, can ordinary roads be broken?
	int PavedRoads;						// ~IB, 0=roads as clear terrain MP cost in bad wx, 1=1 MP to enter via road in bad w
	int TEM40Overruns;					// IB, 1=low-odds overruns from TEM40

	// combat rules //

	int AlliedCombatSide;				// Variant A add German/Finnish CEV; 0 = Other, 1 = British, 2 = French, 3 = Vichy French, 4 = Soviet, 5 = USA, 6 = Greeks
	int ExtremeCrt;						// IB
	int AutoDR;							// IB, computer retreats DR units
	int AutoDH;							// IB, computer selects destroyed DH units
	int AutoDE;							// IB, computer automatically destroys all defending units in case of DE/HX/EX
	int OnlyIsolU1Halved;				// IB, in new system U1 units attack str is halved ONLY if it's isolated too
	int WestCityMod;					// ~IB, 0=partial/full city DRMs -1/-2 (east front), 1=partial/fullhex city DRMs 0/-1 (west front)
	int NewSwamp;						// ~IB, 0=old swamp combat DRM -2, 1=new (SF+) swamp DRM -1
	int AttackSup;						// IB, 0=attack supply ignored, 1=units without attack supply are halved when attacking
	int DASMode;						// DAS_100MOD: printed, modified by terrain; DAS_100: printed, unmodified; DAS_50: printed/2, unmodified
	int GSHalved;						// IB, 0=GS full, 1=GS halved
	int AxisHalfAT;						// IB, axis AT units have only 1/2 ATEC before 3.43 (in FitE/SE they do!)
	int WeatherDieMod;					// IB, 0=weather does not affect die roll modifier, 1=mud=-2, snow=-1
	int SnowReducedAEC;					// IB, allow reduced AEC in snow weather
	int FreezingAllowed;				// IB, swamp/wooded swamp/tundra freezes in snow weather
	int IgnoreFrozenLakeHexsides;		// IB, frozen lake hexsides ignored in SE
	int OneTenthAEC;					// ~IB, 0=1/7 AEC proportion (old), 1=1/10 AEC proportion (new)
	int EngProportion;					// 1/10 or 1/5 Eng proportion gives +/-1 instead of old 1/7
	// ww1: Artillery units operate at full effectiveness only if their nation
	// uses modern artillery tactics. Nations use these tactics as follows:
	// • From Oct I 17 on: Austria-Hungary and Germany.
	// • From Jul I 18 on: America, Britain, and France.
	// • From Feb I 19 on: All other countries.
	int WW1ModernArtTactics;			// IB
	int BorderRiversIgnored;			// IB, border rivers do not affect combat
	int FortifiedAreaLevel;				// Variant A Fortified Area Level (1-4)

	// air rules //

	int OnDemandAirMissions;			// IB, Can air missions be flown by "on demand" principle during movement/combat/exploitation phases
	int NewAFCapacitySystem;			// ~IB, 0=takeoffs and staging eat capacity, no 'inactive' mode, 1=capacity is needed to make units active (otherwise inactive)
	int EndAirCombatAtFirstResult;		// ~IB, 0=in FitE results are cumulative (2*abort=kill etc.), 1=first adverse result ends air combat
	int HFIsF;							// IB, HF doesn't suffer  when fighting F (FitE: Me110=F)
	int AbortedPlanesToPool;			// IB, 0=aborted planes stay on map, 1=go to repl. pool
	int NewAAMods;						// ~IB, 0=old (FitE) AA table & DRMs, 1=new (SF) AA table & DRMs
	int StageRange;						// staging range, % of normal range (i.e. 100 -> normal range like FitE)
	int TmpAFCap;						// tmp airfield may have different capacities in different games (2 or 3)
	int TmpAFBadWeatherCap;				// temp AF in mud/snow weather may have smaller capacity
	int JettisonEsc;					// ~IB, 0=Fs in bombing mission jettison but remain in body, 1=Fs in bombing missions jettison and become escorts
	int BombingFBothReduced;			// ~IB, 0=Fs carrying bombs atk reduced by 2, 1=Fs carrying bombs both atk&def reduced by 2
	int AlliedFullCityAA;				// add Allied and Axis intrinsic AA values for Cities, Fortresses and Airfields
	int AlliedPartialCityAA;
	int AlliedDotCityAA;
	int AlliedReferenceCityAA;
	int AlliedUnImprovedFortressAA;
	int AlliedImprovedFortressAA;
	int AlliedAirfieldAA;
	int AxisFullCityAA;
	int AxisPartialCityAA;
	int AxisDotCityAA;
	int AxisReferenceCityAA;
	int AxisUnImprovedFortressAA;
	int AxisImprovedFortressAA;
	int AxisAirfieldAA;

	//  naval rules //

	int OldNavalSystem;					// ~IB, 0=TFs (SF/WitD), 1=separate capital ships (old FitE/SE)
	int SeparateNavalPhases;			// IB, 1=5 separate naval sub-phases (e.g. SF), 0=one naval sub-phase (e.g. WitD)
	int EnableCDCalc;					// IB, enable/disable automatic CD strength calculation (may be wrong, there are many kind of CD rules)

	// stacking rules //

	int StackNDivType;					// STACK_UNIT or STACK_RE
	int StackDivs;						// normal divisional units
	int StackNDivs;						// normal non-divisional
	int StackArt;						// normal artillery
	int StackMDivs;						// mountain divisional units
	int StackMNDivs;					// mountain non-divisional
	int StackMArt;						// mountain artillery
	int StackADivs;						// arctic divisional units
	int StackANDivs;					// arctic non-divisional
	int StackAArt;						// arctic artillery
	int StackWW1AllowOneFort;			// IB:  0=no, 1=ww1 one fortification unit is allowed
};

#endif
#endif
