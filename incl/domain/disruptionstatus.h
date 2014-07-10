#ifndef DISRUPTIONSTATUS_H
#define DISRUPTIONSTATUS_H

class DisruptionStatus
{
public:
	enum
	{
		NOT_DISRUPTED		=	0,
		DISRUPTED,			//	1
		BADLY_DISRUPTED		//	2
	};
	static const short UNDISRUPT_ALL = -1;
};

#endif
