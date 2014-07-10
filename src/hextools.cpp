#include <vector>
#include <sstream>

#include <wx/wx.h>
#include <wx/textfile.h>
#if defined HEXTOOLSPLAY
#include <wx/filename.h>
#endif

#include "application.h"
#include "hextools.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#if defined HEXTOOLSPLAY

#include "armorantitank.h"
#include "rules.h"
#include "counter.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "airunit.h"
#include "combatresult.h"
#endif // HEXTOOLSPLAY

// data common to both MAP/ and PLAY/

// for ht::logDebugString( )
extern wxTextFile _htlog;  // generic log file for debugging

// data specific to PLAY/ only
#if defined HEXTOOLSPLAY

extern Rules _rule_set;

// for ht::strLen( )
extern wxBitmap* hbmFont[];	// 127  // digits for numbers in counters, mappane

extern int _crt_lowest_die_roll;
extern int _crt_highest_die_roll;

// for ht::crt( )
extern int _crt[13][15];

// not const, so no extern needed to give external linkage
#if 0
extern std::vector<std::string> CRTResult; // used in mapfile.cpp
#endif
extern std::vector<std::string> _custom_result; // used in in ht::crt2str( )

#endif

namespace ht
{
#if defined HEXTOOLSPLAY
	const std::string Replay::NONE = "_";  // save this if no replay
#endif

	int getAdjHexVertex( int vertex )
	{
		// see directional.defs for a sense of the
		//	bit to named direction mapping of
		//	vertices to sides
		switch ( vertex )
		{
			case Hex::SOUTHWEST_CORNER:
			case Hex::SOUTH_CORNER:
				return 0;	// same hex
			case Hex::SOUTHEAST_CORNER:
				return Hex::EAST;
			case Hex::NORTHEAST_CORNER:
			case Hex::NORTH_CORNER:
				return Hex::NORTHEAST;
			case Hex::NORTHWEST_CORNER:
				return Hex::NORTHWEST;
		}
		return 0;
	}

	// returns hexside that is opposite to parameter
	int getAdjHexPart( int side )
	{
		switch ( side )
		{
			case Hex::WEST:
				return Hex::EAST;
			case Hex::EAST:
				return Hex::WEST;
			case Hex::NORTHEAST:
				return Hex::SOUTHWEST;
			case Hex::NORTHWEST:
				return Hex::SOUTHEAST;
			case Hex::SOUTHWEST:
				return Hex::NORTHEAST;
			case Hex::SOUTHEAST:
				return Hex::NORTHWEST;
		}
		return 0;
	}

	// calculates hex[y2][x2] that is adjacent to hex[y][x] in "direction"
	// returns true if success, false if failure (e.g. out of map)
	bool getAdjHexCoords( int direction, int x, int y, int* x2, int* y2, int x_max, int y_max )
	{
		switch ( direction )
		{
			case Hex::WEST:
				*y2 = y;
				*x2 = x - 1;
				break;
			case Hex::EAST:
				*y2 = y;
				*x2 = x + 1;
				break;
			case Hex::NORTHWEST:
				*y2 = y - 1;
				*x2 = x - ( y & 1 ? 0 : 1 );
				break;
			case Hex::NORTHEAST:
				*y2 = y - 1;
				*x2 = x + ( y & 1 ? 1 : 0 );
				break;
			case Hex::SOUTHWEST:
				*y2 = y + 1;
				*x2 = x - ( y & 1 ? 0 : 1 );
				break;
			case Hex::SOUTHEAST:
				*y2 = y + 1;
				*x2 = x + ( y & 1 ? 1 : 0 );
				break;
			default:
				return false;
		}

		return ! coordinatesOutOfBounds( *x2, *y2, x_max, y_max );

#if 0
		*y2 = y - ( side & 0x03 );
		*y2 += ( side & 0x18 );

		*x2 = x - ( side & Hex::WEST );
		*x2 -= ( ( ( y & 1 ) == 0 ) && ( side & 0x11 ) );
		*x2 += ( side & Hex::EAST );
		*x2 += ( ( y & 1 ) && ( side & 0x0a ) );

		switch ( side )
		{
			case Hex::NORTHWEST:
			case Hex::NORTHEAST:
				*y2 = y - 1;
				break;
			case Hex::WEST:
			case Hex::EAST:
				*y2 = y;
				break;
			case Hex::SOUTHWEST:
			case Hex::SOUTHEAST:
				*y2 = y + 1;
				break;
		}
		switch ( side )
		{
			case Hex::WEST:
				*x2 = x - 1;
				break;
			case Hex::NORTHWEST:
			case Hex::SOUTHWEST:
				*x2 = x - ( ( y & 1 ) == 0 ); // y is even
				break;
			case Hex::EAST:
				*x2 = x + 1;
				break;
			case Hex::NORTHEAST:
			case Hex::SOUTHEAST:
				*x2 = x + ( y & 1 ); // y is odd
				break;
		}
#endif
	}

	bool getCornerAdjHexCoords( int corner, int x, int y, int* x2, int* y2, int x_max, int y_max )
	{
		switch ( corner )
		{
			case Hex::NORTHWEST_CORNER:
				*y2 = y - 1;
				*x2 = x - ( y & 1 ? 0 : 1 );
				break;
			case Hex::NORTH_CORNER:
			case Hex::NORTHEAST_CORNER:
				*y2 = y - 1;
				*x2 = x + ( y & 1 ? 1 : 0 );
				break;
			case Hex::SOUTHEAST_CORNER:
				*y2 = y;
				*x2 = x + 1;
				break;
			case Hex::SOUTH_CORNER:
				*y2 = y;
				*x2 = x;
				break;
			case Hex::SOUTHWEST_CORNER:
				*y2 = y;
				*x2 = x;
				break;
			default:
				return 0;
		}

		return ! coordinatesOutOfBounds( *x2, *y2, x_max, y_max );
#if 0
		if ( *x2 < 0 || *y2 < 0 || *x2 >= x_max || *y2 >= y_max )
			return 0;
		return 1;
#endif
	}

	void hexOtherCityTypePositionOffset( int dir, int* pxo, int* pyo )
	{
		switch ( dir )
		{
			// all position origin coordinates relative to
			//	(15,15) where (0,0) is the UL hex corner
			case 7: // NW
				*pxo = -6;	*pyo =  -9;		break;
			case 8: // N
				*pxo =  0;	*pyo = -10;		break;
			case 9: // NE
				*pxo =  6;	*pyo =  -9;		break;
			case 4: // W
				*pxo = -9;	*pyo =   0;		break;
			case 6: // E
				*pxo =  9;	*pyo =   0;		break;
			case 1: // SW
				*pxo = -6;	*pyo =   9;		break;
			case 2: // S
				*pxo =  0;	*pyo =  10;		break;
			case 3: // SE
				*pxo =  6;	*pyo =   9;		break;
			default: // C ( 5 ), or 0
				*pxo =  0;	*pyo =   0;		break;
		}
	}

	void hexMajorCityPositionOffset( int dir, int* pxo, int* pyo )
	{
		switch ( dir )
		{
			case 7: // NW
				*pxo = 0;	*pyo = -4;		break;
			case 8: // N
				*pxo = 3;	*pyo = -4;		break;
			case 9: // NE
				*pxo = 7;	*pyo = -4;		break;
			case 4: // W
				*pxo = -3;	*pyo = 4;		break;
			case 6: // E
				*pxo = 9;	*pyo = 3;		break;
			case 1: // SW
				*pxo = 1;	*pyo = 9;		break;
			case 2: // S
				*pxo = 3;	*pyo = 10;		break;
			case 3: // SE
				*pxo = 6;	*pyo = 8;		break;
			default: // C ( 5 ), or 0
				*pxo = 3;	*pyo = 3;		break;
		}
	}

	void hexIslandPositionOffset( int dir, int* pxo, int* pyo )
	{
		switch ( dir )
		{
			case 7: // NW
				*pxo = -3;	*pyo = -6;		break;
			case 8: // N
				*pxo = 0;	*pyo = -9;		break;
			case 9: // NE
				*pxo = 3;	*pyo = -6;		break;
			case 4: // W
				*pxo = -6;	*pyo = -3;		break;
			case 6: // E
				*pxo = 6;	*pyo = -3;		break;
			case 1: // SW
				*pxo = -3;	*pyo = 0;		break;
			case 2: // S
				*pxo = 0;	*pyo = 3;		break;
			case 3: // SE
				*pxo = 3;	*pyo = 0;		break;
			default: // C ( 5 ), or 0
				*pxo = 0;	*pyo = -3;		break;
		}
	}

	// calc port symbol position offsets
	void hexPortSymbolPositionOffset( int dir, int* pxo, int* pyo )
	{
		switch ( dir )
		{
			case Hex::NORTHWEST:
				*pxo = -6;	*pyo = -2; 		break;
			case Hex::NORTHEAST:
				*pxo = 8;	*pyo = -2;		break;
			case Hex::WEST:
				*pxo = -8;	*pyo = 5;		break;
			case Hex::EAST:
				*pxo = 10;	*pyo = 5;		break;
			case Hex::SOUTHWEST:
				*pxo = -6;	*pyo = 14; 		break;
			case Hex::SOUTHEAST:
				*pxo = 8;	*pyo = 14; 		break;
			default:  // CENTER
				*pxo = 1;	*pyo = 5;		break;
		}
	}

	char* skipSpace( char* cp )
	{
		while ( cp && *cp && ( *cp == '\t' || *cp == ' ' ) )
			++cp;

		return cp;
	}

	// convert char* to wxString&
	const wxString& wxS( const char* s )
	{
		static wxString ws;
		ws = wxString::FromAscii( s );
		return ws;
	}

	// spaces can't be saved, so we'll convert them to '_''s and back
	void spc2line( char* orig_cp )
	{
		char* sp;
		char* cp;

		// convert ' ' to '_'
		cp = orig_cp;
		while ( true )
		{
			sp = strchr( cp, ' ' );
			if ( !sp )
				break;
			*sp = '_';
			cp = sp + 1;
		}
		// convert '\n' to '@'
		cp = orig_cp;
		while ( true )
		{
			sp = strchr( cp, '\n' );
			if ( !sp )
				break;
			*sp = '@';
			cp = sp + 1;
		}
		// remove non-printable characters from text (replace an underscore - acceptable characters are #32 to #127)
		cp = orig_cp;
		while ( *cp )
		{
			if ( ( (unsigned char)*cp < 32 ) || ( (unsigned char)*cp > 127 ) )
				*cp = '_';
			++cp;
		}
	}

	void line2spc( char* orig_cp )
	{
		char* sp;
		char* cp;

		cp = orig_cp;
		while ( true )
		{
			sp = strchr( cp, '_' );
			if ( !sp )
				break;
			*sp = ' ';
			cp = sp + 1;
		}
		cp = orig_cp;
		while ( true )
		{
			sp = strchr( cp, '@' );
			if ( !sp )
				break;
			*sp = '\n';
			cp = sp + 1;
		}
	}

	// is string ascii-streamable? it's not if it has zero len or only spaces
	int is_ok_str( const char *cp )
	{
		if ( ! cp )
			return 0;

		if ( strlen( cp ) == 0 )
			return 0;

		while ( *cp )
		{
			if ( *cp != ' ' && *cp != '\t' )
				return 1;
			++cp;
		}
		return 0;
	}

	// is string ASCII-streamable? it's not if it has zero length or only spaces
	int isOkStr( const char* cp )
	{
		if ( ! cp || strlen( cp ) == 0 )
			return FALSE;

		while ( *cp )
		{
			if ( *cp != ' ' && *cp != '\t' )
				return TRUE;
			++cp;
		}
		return FALSE;
	}

	int oppositeDir( int dir )
	{
		return ( ( dir << 3 ) & 0x3f ) + ( dir >> 3 );
	}

	/// start at end of C string, and rewind until
	///	 first encounter with pathname separator
	/// return reference to everything after it
	/// if no separator found, the original C string
	///  is the basename
	char* basename( const char* fullname )
	{
		static char bf[300];
		strcpy( bf, fullname );

		int i = strlen( bf ) - 1;
		while ( i > 0 )
		{
			if ( bf[i] == '\\' || bf[i] == '/' ) // win || unix
				return &( bf[i + 1] );

			--i;
		}

		return bf;
	}

	/// start at end of C string, and rewind until
	///  first encounter with pathname separator
	/// null terminate the position after it
	///  and return reference to the full C string
	/// if no separator found, null terminate the
	///  first position and return it (no path)
	char* pathname( const char* fullname )
	{
		static char bf[300];
		strcpy( bf, fullname );

		int i = strlen( bf ) - 1;
		while ( i > 0 )
		{
			if ( bf[i] == '\\' || bf[i] == '/' ) // win || unix
			{
				bf[i + 1] = '\0';
				return bf;
			}
			--i;
		}

		bf[0] = '\0';
		return bf;
	}

	void logDebugString( wxString descr, wxString logdata )
	{
		if ( _htlog.IsOpened( ) )
		{
			_htlog.AddLine( descr + logdata );
			_htlog.Write( );
		}
	}

	char* inttoa( int i )
	{
		static char bfr[100];

		sprintf( bfr, "%d", i );
		return bfr;
	}

	int limit( int v, int min, int max )
	{
		if ( v < min )
			return min;
		if ( v > max )
			return max;
		return v;
	}

	// return distance between given hexes (in hexes)
	int DistBetweenHexes( int x1, int y1, int x2, int y2 )
	{
		int dx = x2 - x1;
		dx *= 2;

		int dy = y2 - y1;
		// dist from odd to even row: needs to be adjusted
		//int r = dy % 2;
		//
		if ( y2 % 2 && dx < 0 )
			++dx;

		if ( ! ( y2 % 2 ) && dx > 0 )
			--dx;

		// to positive
		if ( dy < 0 )
			dy = -dy;

		if ( dx < 0 )
			dx = -dx;

		int dist = 0;
		while ( dy > 0 )
			--dy, --dx, ++dist;
		while ( dx > 0 )
			dx -= 2, ++dist;

		return dist;
	}

#if defined HEXTOOLSPLAY

	void message_box_warning( const char* msg, const std::string& data )
	{
		wxMessageBox( wxT( msg ) + data );
	}

	// return str len (internal fonts)
	int strLen( char* s )
	{
		int l = 0;
		while ( s && *s )
		{
			switch ( *s )
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					if ( *s == '1' )
						l += 4;
					else
						l += 5;
					break;
				case '-':
					l += 3;
					break;
				default:
					if ( hbmFont[(int) ( *s )] )
						l += hbmFont[(int) ( *s )]->GetWidth( );
					++l;
					break;
			}
			++s;
		}
		return l;
	}

	std::vector<std::string>& split( const std::string& s, char delim, std::vector<std::string>& elems )
	{
	    std::stringstream ss( s );
	    std::string item;
	    while ( std::getline( ss, item, delim ) )
	        elems.push_back( item );
	    return elems;
	}

	int cdr_str( int i )
	{
		int id;

		switch ( i )
		{
			case 1:
				id = 0;
				break;
			case 2:
				id = 0;
				break;
			case 3:
				id = 1;
				break;
			case 4:
				id = 1;
				break;
			case 5:
				id = 2;
				break;
			case 6:
				id = 2;
				break;
			case 7:
				id = 3;
				break;
			case 8:
				id = 3;
				break;
			case 9:
				id = 4;
				break;
			case 10:
				id = 4;
				break;
			case 11:
				id = 5;
				break;
			case 12:
				id = 5;
				break;
			case 13:
				id = 6;
				break;
			case 14:
				id = 6;
				break;
			case 15:
				id = 7;
				break;
			case 16:
				id = 7;
				break;
			case 17:
				id = 8;
				break;
			case 18:
				id = 8;
				break;
			case 19:
				id = 9;
				break;
			case 20:
				id = 9;
				break;
			default:
				id = ( i * 49 ) / 100;
				break;
		}
		return id;
	}

	// draw null terminated number string using built-in font:
	void drawStr( wxDC* dc, int x, int y, char* s )
	{
		wxMemoryDC hdcMem;
		// bug in WxMSW: colors are wrong!
#if 0
#if defined __WXMSW__
		wxColor c = dc->GetTextBackground( );
		dc->SetTextBackground( dc->GetTextForeground( ) );
		dc->SetTextForeground( c );
#endif
#endif
		while ( s && *s )
		{
			if ( *s >= 'A' && *s <= 'Z' )
			{
				dc->DrawBitmap( *hbmFont[(int)*s], x, y, false );
				x += hbmFont[(int)*s]->GetWidth( );
				++x;
			}
			else
			{
//				int sx;
				switch ( *s )
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
#if 0
						if ( *s == '1' )
							sx = 4;
						else
							sx = 5;
#endif
						dc->DrawBitmap( *hbmFont[*s - '0'], x, y, false );
						x += ( *s == '1' ? 4 : 5 ); // sx;
						break;
					case '-':
						dc->DrawBitmap( *hbmFont[10], x, y, false );
						x += 3;
						break;
				}
			}
			++s;
		}
#if 0
#if defined __WXMSW__
		c = dc->GetTextBackground( );
		dc->SetTextBackground( dc->GetTextForeground( ) );
		dc->SetTextForeground( c );
#endif
#endif
	}

	int crt( float r, int die )
	{
		int row;
		if ( r >= 9.0 )
			row = 12;
		else if ( r >= 8.0 )
			row = 11;
		else if ( r >= 7.0 )
			row = 10;
		else if ( r >= 6.0 )
			row = 9;
		else if ( r >= 5.0 )
			row = 8;
		else if ( r >= 4.0 )
			row = 7;
		else if ( r >= 3.0 )
			row = 6;
		else if ( r >= 2.0 )
			row = 5;
		else if ( r >= 1.5 )
			row = 4;
		else if ( r >= 1.0 )
			row = 3;
		else if ( r >= 0.5 )
			row = 2;
		else if ( r >= 0.33333333333 )
			row = 1;
		else
			row = 0;

		die = limit( die, _crt_lowest_die_roll, _crt_highest_die_roll );
// TODO: remove the below when the above is confirmed
#if 0
		 if (die < FirstDie)
		 	 die = FirstDie;
		 if (die > LastDie)
		 	 die = LastDie;
#endif
		return _crt[row][die + 4];  // index must begin from zero, NOT from -4
	}

	// convert crt result to string
	const char* crt2str( int result )
	{
		switch ( result )
		{
			case CombatResult::AE:
				return "AE";
			case CombatResult::AH:
				return "AH";
			case CombatResult::AR:
				return "AR";
			case CombatResult::AS:
				return "AS";
			case CombatResult::EX:
				return "EX";
			case CombatResult::HX:
				return "HX";
			case CombatResult::DR:
				return "DR";
			case CombatResult::DH:
				return "DH";
			case CombatResult::DE:
				return "DE";
				// ww1:
			case CombatResult::AQ:
				return "AQ";
			case CombatResult::AD:
				return "AD";
			case CombatResult::DD:
				return "DD";
			case CombatResult::AL:
				return "AL";
			case CombatResult::DL:
				return "DL";
			case CombatResult::AX:
				return "AX";
			case CombatResult::DX:
				return "DX";
			case CombatResult::DXM:
				return "DXM";
			case CombatResult::BX:
				return "BX";
			case CombatResult::BXM:
				return "BXM";
			// custom result:
			default:
				return _custom_result[result < 0 ? -result : result].c_str( ); //.name;
		}
	}

	// find optimal AEC for given RE combination
	// TODO: optimize treatment of half-AEC units (now just all half and all neutral
	// combinations are tried). Or maybe that is enough?
	// TODO: consider a case where defender is full AECD/ATEC, wherein it may be
	// more efficient to treat fewer half REs as full (and more as none) in order to
	// keep the AECA ratio below 1/2, and thus the defender pegged at AECD (lesser DRM)
	// returns:  0=None, 1=Full, 2=Half, 5=Partial (e.g. 1/7 or 1/10)
	int calculateOptimalAEC( float full_re, float half_re, float neut_re, float none_re )
	{
		// I. set local variables for 1st alternative
		float capable_REs = full_re;
		float neutral_REs = neut_re;
		float non_capable_REs = none_re;

		// a. treat half-capable REs as capable
		// [note: shows treatment of half-capable REs as capable for
		//	the purposes of calculating excess neutral REs, see b.]
		capable_REs += half_re;

		// b. enforce neutral limit: 2x number of capable REs
		float excess_neutral_REs = neutral_REs - ( 2.0F * capable_REs );

		// c. shift any excess neutral REs to non-capable
		non_capable_REs += ( excess_neutral_REs > 0.0F ) ? excess_neutral_REs : 0.0F;

		// d. now shift 1/2 of the half-capable REs FROM (-) capable
		//	TO (+) non-capable prior to proportion calculation
		capable_REs -= half_re / 2.0F;
		non_capable_REs += half_re / 2.0F;

		float half_capable_ratio = capable_REs / ( capable_REs + non_capable_REs );

		// II. reset local variables for 2nd alternative
		capable_REs = full_re;
		neutral_REs = neut_re;
		non_capable_REs = none_re;

		// a. treat all half-capable REs as neutral REs
		neutral_REs += half_re;

		// b. enforce neutral limit: 2x number of capable REs
		// [note: residual_half_capable_REs moot, still set to zero]
		excess_neutral_REs = neutral_REs - ( 2.0F * capable_REs );

		// c. shift any excess neutral REs to non-capable
		non_capable_REs += ( excess_neutral_REs > 0.0F ) ? excess_neutral_REs : 0.0F;

		float neutral_ratio = capable_REs / ( capable_REs + non_capable_REs );

		// I.c. and II.c. select the higher ratio of the above cases
		float optimal_ratio = neutral_ratio > half_capable_ratio ? neutral_ratio : half_capable_ratio;

		// close to 1/1
		if ( optimal_ratio > 0.999F )
			return ArmorAntitank::Proportion::FULL;

		// close to 1/2
		if ( optimal_ratio > 0.499F )
			return ArmorAntitank::Proportion::HALF;

		// close to 1/10 and 1/7, respectively
		if ( optimal_ratio > ( _rule_set.OneTenthAEC == TRUE ? 0.0999F : 0.142F ) )
			return ArmorAntitank::Proportion::PARTIAL;

		return ArmorAntitank::Proportion::NONE;
	}

	int aeca2mod( int aeca )
	{
		switch ( aeca )
		{
			case ArmorAntitank::Proportion::FULL:
				return 3;
			case ArmorAntitank::Proportion::HALF:
				return 2;
			case ArmorAntitank::Proportion::PARTIAL:
				return 1;
			default:
				return 0;
		}
	}

	int aecd2mod( int aecd )
	{
		switch ( aecd )
		{
			case ArmorAntitank::Proportion::FULL:
			case ArmorAntitank::Proportion::HALF:
				return 2;
			case ArmorAntitank::Proportion::PARTIAL:
				return 1;
			default:
				return 0;
		}
	}

	int atec2mod( int atec )
	{
		switch ( atec )
		{
			case ArmorAntitank::Proportion::FULL:
				return 4;
			case ArmorAntitank::Proportion::HALF:
				return 2;
			case ArmorAntitank::Proportion::PARTIAL:
				return 1;
			default:
				return 0;
		}
	}

	const char* aec2str( int aec )
	{
		switch ( aec )
		{
			case ArmorAntitank::Proportion::NONE:
				return "none";
			case ArmorAntitank::Proportion::PARTIAL:
				return "partial"; // ( RuleSet.OneTenthAEC ) ? "1/10" : "1/7";
			case ArmorAntitank::Proportion::THIRD:
				return "third";
			case ArmorAntitank::Proportion::HALF:
				return "half";
			case ArmorAntitank::Proportion::FULL:
				return "full";
			case ArmorAntitank::Proportion::NEUT:
				return "neutral";
			default:
				return "unknown";
		}
	}

	int dice( )
	{
		return ( rand( ) % 6 ) + 1;
	}

	int rollDiceN( int max )
	{
		return ( rand( ) % max ) + 1;
	}

	const char* mission2str( int m )
	{
		static char b[50];
		if ( m < 0 )
			return "";

		switch ( m )
		{
			case AirUnit::Mission::INTERCEPT:
				return "intercept";
			case AirUnit::Mission::BOMB_GS:
				return "ground support";
			case AirUnit::Mission::BOMB_AF:
				return "airfield bombing";
			case AirUnit::Mission::BOMB_RR:
				return "railroad bombing";
			case AirUnit::Mission::BOMB_HARASS:
				return "harassment bombing";
			case AirUnit::Mission::BOMB_NAVAL:
				return "ship in port bombing";
			case AirUnit::Mission::BOMB_DAS:
				return "defensive air support";
			case AirUnit::Mission::BOMB_PLANES:
				return "plane bombing";
			case AirUnit::Mission::BOMB_PORT:
				return "port bombing";
			case AirUnit::Mission::BOMB_CF:
				return "coastal fortification bombing";
			case AirUnit::Mission::BOMB_OTHER:
				return "general purpose tactical bombing";
			case AirUnit::Mission::NAVALPATROL:
				return "naval patrol";
			case AirUnit::Mission::TRANSPORT:
				return "transport";
			case AirUnit::Mission::AIRDROP:
				return "airdrop";
			case AirUnit::Mission::MINELAYING:
				return "minelaying";
			case AirUnit::Mission::RAILMARSHAL:
				return "strategic rail marshal yard bombing";
			case AirUnit::Mission::STRAT_PORT:
				return "strategic port bombing";
			case AirUnit::Mission::STRAT_OTHER:
				return "general strategic bombing";
			case AirUnit::Mission::ANYBOMB:
				return "bombing";
			case AirUnit::Mission::CAP:
				return "combat air patrol";
			case AirUnit::Mission::ESCORT:
				return "escort";
			case AirUnit::Mission::TRANSFER:
				return "transfer";
			case AirUnit::Mission::STAGE:
				return "staging";
			case AirUnit::Mission::NONE:
				return "";
		}
		// TODO: make this a default case in the above switch
		sprintf( b, "escort %s", mission2str( m - AirUnit::Mission::ESCORT ) );
		return const_cast<const char*>( b );
	}

	int dir2replay( int dir )
	{
		switch ( dir )
		{
			case Hex::WEST:
				return ht::Replay::Movement::WEST;
			case Hex::SOUTHWEST:
				return ht::Replay::Movement::SOUTHWEST;
			case Hex::NORTHWEST:
				return ht::Replay::Movement::NORTHWEST;
			case Hex::EAST:
				return ht::Replay::Movement::EAST;
			case Hex::SOUTHEAST:
				return ht::Replay::Movement::SOUTHEAST;
			case Hex::NORTHEAST:
				return ht::Replay::Movement::NORTHEAST;
			default:
				wxMessageBox( wxT("BUG: dir2replay: illegal dir\nplease report this bug"), Application::NAME );
				return 0;
		}
	}

	int dir2replayExploitation( int dir )
	{
		switch ( dir )
		{
			case Hex::WEST:
				return ht::Replay::Exploit::WEST;
			case Hex::SOUTHWEST:
				return ht::Replay::Exploit::SOUTHWEST;
			case Hex::NORTHWEST:
				return ht::Replay::Exploit::NORTHWEST;
			case Hex::EAST:
				return ht::Replay::Exploit::EAST;
			case Hex::SOUTHEAST:
				return ht::Replay::Exploit::SOUTHEAST;
			case Hex::NORTHEAST:
				return ht::Replay::Exploit::NORTHEAST;
			default:
				wxMessageBox( wxT("BUG: dir2replay: illegal dir\nplease report this bug"), Application::NAME );
				return 0;
		}
	}

	int dir2replayCombat( int dir )
	{
		switch ( dir )
		{
			case Hex::WEST:
				return ht::Replay::Combat::WEST;
			case Hex::SOUTHWEST:
				return ht::Replay::Combat::SOUTHWEST;
			case Hex::NORTHWEST:
				return ht::Replay::Combat::NORTHWEST;
			case Hex::EAST:
				return ht::Replay::Combat::EAST;
			case Hex::SOUTHEAST:
				return ht::Replay::Combat::SOUTHEAST;
			case Hex::NORTHEAST:
				return ht::Replay::Combat::NORTHEAST;
			default:
				wxMessageBox( wxT("BUG: dir2replay: illegal dir\nplease report this bug"), Application::NAME );
				return 0;
		}
	}

	int replay2dir( int replay )
	{
		switch ( replay )
		{
			case ht::Replay::Movement::WEST:
				return Hex::WEST;
			case ht::Replay::Movement::SOUTHWEST:
				return Hex::SOUTHWEST;
			case ht::Replay::Movement::NORTHWEST:
				return Hex::NORTHWEST;
			case ht::Replay::Movement::EAST:
				return Hex::EAST;
			case ht::Replay::Movement::SOUTHEAST:
				return Hex::SOUTHEAST;
			case ht::Replay::Movement::NORTHEAST:
				return Hex::NORTHEAST;
			default:
				wxMessageBox( wxT("BUG: replay2dir: illegal dir\nplease report this bug"), Application::NAME );
				return 0;
		}
	}

#if defined HEXTOOLSPLAY
	const char* dir2Str( int dir )
	{
		switch ( dir )
		{
			case Hex::WEST:
				return "west";
			case Hex::NORTHWEST:
				return "northwest";
			case Hex::SOUTHWEST:
				return "southwest";
			case Hex::EAST:
				return "east";
			case Hex::NORTHEAST:
				return "northeast";
			case Hex::SOUTHEAST:
				return "southeast";
			default:
				return "buggy dir";
		}
	}
#endif

#endif
}
