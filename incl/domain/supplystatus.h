#ifndef SUPPLYSTATUS_H
#define SUPPLYSTATUS_H

class SupplyStatus
{
public:
	enum
	{
		REG		=	0,	// regular supply
		SPC,	//	1	// special supply
		NONE	//	2	// out of supply
	};
};

#endif
