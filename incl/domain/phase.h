#ifndef PHASE_H
#define PHASE_H

class Phase
{
public:
	enum
	{
		NO_GAME				=	0,  // No game has been started (yet)
		SETUP_PHASE,		//	1
		INITIAL_PHASE,		//	2
		NAVAL_PHASE_1,		//	3
		NAVAL_PHASE_2,		//	4
		NAVAL_PHASE_3,		//	5
		NAVAL_PHASE_4,		//	6
		NAVAL_PHASE_5,		//	7
		MOVEMENT_PHASE,		//	8
		REACTION_PHASE,		//	9   // traditional air rules: non-phasing player flies interception
		COMBAT_PHASE,		//	10
		REACT_MOVE_PHASE,	//	11  // ww1 reaction movement phase
		REACT_COMB_PHASE,	//	12  // ww1 reaction combat phase
		NAVAL_PHASE_6,		//	13  // +2
		NAVAL_PHASE_7,		//	14  // +2
		NAVAL_PHASE_8,		//	15  // +2
		NAVAL_PHASE_9,		//	16  // +2
		NAVAL_PHASE_10,		//	17  // +2
		EXPLOITATION_PHASE	//	18  // +2
	};
	static const unsigned short LASTPHASE = EXPLOITATION_PHASE; // if Phase == LASTPAHSE -> next player
};

#endif
