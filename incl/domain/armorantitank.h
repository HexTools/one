#ifndef ARMORANTITANK_H
#define ARMORANTITANK_H

class ArmorAntitank
{
public:
	struct Proportion
	{
		enum
		{
			NONE	=	0,
			FULL,	//	1
			HALF,	//	2
			NEUT,	//	3
			THIRD,	//	4  // mixed units
			PARTIAL	//	5  // for stack aec calculations, not for units (1/7 OR 1/10 if that rule is selected)
		};
	};
};

#endif
