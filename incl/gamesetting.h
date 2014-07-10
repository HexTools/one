#if defined HEXTOOLSPLAY
#ifndef GAMESETTING_H
#define GAMESETTING_H

// TODO: should be able to retire this class and flags
//	after building the UCC / armed force parsing
class GameSetting
{
public:
	static const unsigned short FITE		=	0x0001;	// fire in the east & second front
	static const unsigned short SF			=	0x0002;	// second front
	static const unsigned short WITD		=	0x0004;	// war in the desert
	static const unsigned short BF			=	0x0008;	// balkan front
	static const unsigned short WW			=	0x0010;	// winter war
	static const unsigned short FWTBT		=	0x0020;	// for whom the bell tolls
	static const unsigned short FTF			=	0x0040;	// first to fight
	static const unsigned short SOS			=	0x0080;	// storm over scandinavia
	static const unsigned short GE			=	0x0100;	// ww2eur only (example: finns in allied side)
	static const unsigned short FOF			=	0x0200;	// fall of france
	// 0x0400;
	// 0x0800;
	static const unsigned short ALL			=	0x0fff;	// all ww2eur games (not ww1 or ww2pac)
	static const unsigned short WW2			=	(FITE|SF|WITD|BF|FTF|SOS|FOF);	// all but FWtBT & AWW
	static const unsigned short WEST		=	(SF|WITD|FOF);					// Western Europe / Africa
	static const unsigned short WW1			=	0x1000;
	// 0x2000;
	static const unsigned short WW2PAC		=	0x4000;
	// 0x8000;
};

#endif
#endif
