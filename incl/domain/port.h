#ifndef PORT_H
#define PORT_H

struct Port
{
	struct Type
	{
		enum
		{
			UNSET		= 0,
			MINOR,		// 1
			STANDARD,	// 2
			MAJOR,		// 3
			GREAT,		// 4
			ANCHORAGE,	// 5 // ww2pac
			MARGINAL	// 6 // ww2pac
		};
	};

	struct Attribute
	{
		enum
		{
			NORMAL		= 0x00,
			STRONG		= 0x01,
			ARTIFICIAL	= 0x02
		};
	};
};

#endif
