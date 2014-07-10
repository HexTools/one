#ifndef HEXTOOLS_H
#define HEXTOOLS_H

#define nullptr NULL

#if ! defined INCLUDED_WX
#include <wx/wx.h>
#undef wxS
#define INCLUDED_WX
#endif

#if defined HEXTOOLSPLAY
#if ! defined INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif
#endif

namespace ht
{
	const unsigned short MAP_VERSION = 12;		// format of the map file
	const unsigned short OK_MAP_VERSION	= 12;	// earliest-supported format of .map file
	const unsigned short MAX_MAPSHEETS = 200;	// 200 mapsheets ought to be enough for everyone...
	const unsigned short MAX_MAP_ID_LEN = 6;	// 6:  a 4 or 5 len map ID string + 1 null terminator
	const unsigned short MAX_HEX_ID_LEN = 5;	// 5:  a 4 len hex ID string + 1 null terminator

	const unsigned short NUM_WEATHER_CLASSES = 3;
	const unsigned short NUM_MVMT_EFFECTS_COLUMNS = 6;

	const unsigned short DEFAULT_HEX_ID = 101;

	//used as arguments to the ht::limit(..) function
	const int NoLowerBound = INT_MIN;
	const int NoUpperBound = INT_MAX;

	// IMPORTANT:  need an accurate line count here from wzstrings.defs
	static const int NUM_WZ = 42;

	// IMPORTANT:  need an accurate line count here from district.defs
	static const int NUM_DIST = 408;

	// IMPORTANT:  need an accurate line count here from seacircle.defs
	static const int NUM_SEABOX = 404;

	inline bool coordinatesOutOfBounds( int x, int y, int x_max, int y_max )
	{
		return ( x < 0 || y < 0 || x >= x_max || y >= y_max );
	}
	int getAdjHexPart( int side );
	int getAdjHexVertex( int vertex );
	bool getAdjHexCoords( int direction, int x, int y, int* x2, int* y2, int x_max, int y_max );
	bool getCornerAdjHexCoords( int corner, int x, int y, int* x2, int* y2, int x_max, int y_max );
	void hexOtherCityTypePositionOffset( int dir, int* pxo, int* pyo );
	void hexMajorCityPositionOffset( int dir, int* pxo, int* pyo );
	void hexIslandPositionOffset( int dir, int* pxo, int* pyo );
	void hexPortSymbolPositionOffset( int dir, int* pxo, int* pyo );
	char* skipSpace(char* cp);
	const wxString& wxS(const char* s);
	void spc2line(char* orig_cp);
	void line2spc(char* orig_cp);
	int is_ok_str( const char *cp );
	int isOkStr(const char* cp);
	int oppositeDir(int dir);
	char* basename(const char* path);
	char* pathname(const char* path);
	void logDebugString(wxString descr, wxString logdata);
	char* inttoa(int i);
	int limit(int v, int min, int max);
	int DistBetweenHexes( int x1, int y1, int x2, int y2 );

#if defined HEXTOOLSPLAY
	const unsigned short SCN_VERSION = 12;		// format of the scenario file

	// IMPORTANT:  need an accurate line count here from district.defs
	static const int NUM_COUNTRY = 166;

	// IMPORTANT:  need an accurate line count here from seacircle.defs
	static const int NUM_SEAZONE = 36;

	const int SMALLEST_DIE = -4;
	const int LARGEST_DIE = 10;

	struct Replay
	{
		static const std::string NONE; // = "_" save this if no replay
		static const short MAX_LEN		=	50;
		static const char END			=	'\0';

		struct Movement
		{
			static const char EAST		=	'a';
			static const char SOUTHEAST	=	'b';
			static const char NORTHEAST	=	'c';
			static const char WEST		=	'd';
			static const char SOUTHWEST	=	'e';
			static const char NORTHWEST	=	'f';

			// 'A' through 'I' available

			static const char CLEARRAIL	=	'J'; // engineers: stuff cleared
			static const char STAGE		=	'K'; // planes: stage in this hex:
			static const char HITRAIL	=	'L'; // land units: railroad exploded
			static const char HITAF		=	'M'; // land units: airbase exploded
			static const char HITPORT	=	'N'; // land units: port exploded
			static const char FIXRAIL	=	'O'; // engineers: stuff repaired
			static const char FIXAF		=	'P'; // engineers: stuff repaired
			static const char FIXPORT	=	'Q'; // engineers: stuff repaired
			static const char REGAUGE	=	'R'; // engineers: stuff repaired
			static const char TOTRAIN	=	'S'; // unit loaded to train
			static const char FRTRAIN	=	'T'; // unit unloaded from train
			static const char BOMB		=	'U'; // plane: bombed

			static const char END		=	'V'; // end of movement phase marker

			// 'W' through 'Z' available
		};

		struct Admin
		{
			static const char EAST		=	'g';
			static const char SOUTHEAST	=	'h';
			static const char NORTHEAST	=	'i';
			static const char WEST		=	'j';
			static const char SOUTHWEST	=	'k';
			static const char NORTHWEST	=	'l';
		};

		struct Exploit
		{
			static const char EAST		=	'm';
			static const char SOUTHEAST	=	'n';
			static const char NORTHEAST	=	'o';
			static const char WEST		=	'p';
			static const char SOUTHWEST	=	'q';
			static const char NORTHWEST	=	'r';
		};

		struct Combat
		{
			static const char EAST		=	's';
			static const char SOUTHEAST	=	't';
			static const char NORTHEAST	=	'u';
			static const char WEST		=	'w'; // out of order
			static const char SOUTHWEST	=	'v'; // not sure why/how
			static const char NORTHWEST	=	'x';
		};

		// 'y' and 'z' available
	};

	void message_box_warning( const char*, const std::string& );
	int strLen(char* s);
	std::vector<std::string>& split( const std::string&, char, std::vector<std::string>& );
	int cdr_str( int i );
	void drawStr(wxDC* dc, int x, int y, char* s);
	int crt(float r, int die);
	const char* crt2str(int result);
	int calculateOptimalAEC(float full_re, float half_re, float neut_re, float none_re);
	int aeca2mod(int aeca);
	int aecd2mod(int aecd);
	int atec2mod(int atec);
	const char* aec2str(int aec);
	int dice(void);
	int rollDiceN(int max);
	const char* mission2str(int m);
	int dir2replayExploitation(int dir);
	int dir2replayCombat(int dir);
	int dir2replay(int dir);
	int replay2dir(int replay);
	const char* dir2Str( int dir );
#endif
};

#if defined HEXTOOLSPLAY
// for ht::Crt2Str() and mappane2
#if 0
typedef struct
{
    char name[5];	// "AE"
    int  val;   	// CRT_AE
} crt_item_t;
#endif
#endif

#endif
