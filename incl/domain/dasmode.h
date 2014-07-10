#ifndef DASMODE_H
#define DASMODE_H

class DASMode
{
public:
	enum
	{
		FULL_STR_MOD	=	0,	// printed str, modified my TEC
		FULL_STR,		//	1	// printed str, unmodified
		HALF_STR		//	2	// printed str/2, unmodified
	};
};



#endif /* DASMODE_H_ */
