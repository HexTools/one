#if defined HEXTOOLSPLAY
#ifndef AIRUNITTYPE_H
#define AIRUNITTYPE_H

class AirUnitType
{
public:
	enum
	{
		F		=	0,	// fighter
		HF,		//	1	// heavy fighter
		B,		//	2	// bomber
		A,		//	3	// attack
		D,		//	4	// dive bomber
		HB,		//	5	// heavy bomber
		T,		//	6	// transport
		HT,		//	7	// heavy transport
		GT,		//	8	// glider transport
		GHT,	//	9	// heavy glider transport
		R,		//	10  // reconnaissance (ww1)
		Z,		//	11  // zeppelin (ww1)
		O		//	12  // observation balloon (ww1)
	};
	struct Prefix
	{
		enum
		{
			NONE	=	0,
			N,		//	1  // night
			J,		//	2  // jet
			R,		//	3  // rocket
		};
	};
};

#endif
#endif
