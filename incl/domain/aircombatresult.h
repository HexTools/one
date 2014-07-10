#ifndef AIRCOMBATRESULT_H
#define AIRCOMBATRESULT_H

class AirCombatResult
{
public:
	enum
	{
		MISS		=	0,
		RETURN,		//	1
		ABORT,		//	2
		KILL,		//	3
		AUTOMISS	//	4 //didn't fire, automaticaly missed
	};
};

#endif
