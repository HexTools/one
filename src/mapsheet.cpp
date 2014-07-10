#if defined HEXTOOLSPLAY
#elif defined HEXTOOLSMAP

#include <fstream>
using std::ifstream;

#include <wx/wx.h>

#include "hextools.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#include "dextendmap.h"

// this list contains all rotated mapsheets:
char rotated_map[ht::MAX_MAPSHEETS][ht::MAX_MAP_ID_LEN];
// this list contains all normal orientation right-angled offset mapsheets:
char rightoffset_map[ht::MAX_MAPSHEETS][ht::MAX_MAP_ID_LEN];

static int newiYWorldSize;
static int newiXWorldSize;

void MapPane::CmExpandMap( wxCommandEvent& WXUNUSED(event) )
{
	DExtendMap dl( this );
	dl.setCaption( "Expand map" );
	if ( dl.ShowModal( ) == FALSE )
		return;

	int cnt, dir;
	dl.getData( &cnt, &dir );

	if ( cnt <= 0 || ( dir != MapPane::RESIZE_MAP_TO_NORTH && dir != MapPane::RESIZE_MAP_TO_SOUTH && dir != MapPane::RESIZE_MAP_TO_WEST && dir != MapPane::RESIZE_MAP_TO_EAST ) )
	{
		wxMessageBox( "You must specify both count and direction!" );
		return;
	}

	// odd number of new rows causes TROUBLE!
	if ( ( dir == MapPane::RESIZE_MAP_TO_NORTH || dir == MapPane::RESIZE_MAP_TO_SOUTH ) && ( cnt & 1 ) )
	{
		wxMessageBox( "Number of rows must be even!" );
		return;
	}

	int thisiYWorldSize, thisiXWorldSize;
	Hex** newHex;
	switch ( dir )
	{
		case MapPane::RESIZE_MAP_TO_NORTH:
			thisiYWorldSize = world_y_size_ + cnt;
			thisiXWorldSize = world_x_size_;
			if ( ! allocate_new_map_memory( &newHex, thisiXWorldSize, thisiYWorldSize ) )
			{
				wxMessageBox( "Couldn't get memory!" );
				return;
			}
			copy_sectors_expand( newHex, 0, cnt );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = thisiXWorldSize;
			world_y_size_ = thisiYWorldSize;
			hex_ = newHex;
			break;
		case MapPane::RESIZE_MAP_TO_SOUTH:
			thisiYWorldSize = world_y_size_ + cnt;
			thisiXWorldSize = world_x_size_;
			if ( ! allocate_new_map_memory( &newHex, thisiXWorldSize, thisiYWorldSize ) )
			{
				wxMessageBox( "Couldn't get memory!" );
				return;
			}
			copy_sectors_expand( newHex, 0, 0 );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = thisiXWorldSize;
			world_y_size_ = thisiYWorldSize;
			hex_ = newHex;
			break;
		case MapPane::RESIZE_MAP_TO_WEST:
			thisiYWorldSize = world_y_size_;
			thisiXWorldSize = world_x_size_ + cnt;
			if ( ! allocate_new_map_memory( &newHex, thisiXWorldSize, thisiYWorldSize ) )
			{
				wxMessageBox( "Couldn't get memory!" );
				return;
			}
			copy_sectors_expand( newHex, cnt, 0 );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = thisiXWorldSize;
			world_y_size_ = thisiYWorldSize;
			hex_ = newHex;
			break;
		case MapPane::RESIZE_MAP_TO_EAST:
			thisiYWorldSize = world_y_size_;
			thisiXWorldSize = world_x_size_ + cnt;
			if ( ! allocate_new_map_memory( &newHex, thisiXWorldSize, thisiYWorldSize ) )
			{
				wxMessageBox( "Couldn't get memory!" );
				return;
			}
			copy_sectors_expand( newHex, 0, 0 );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = thisiXWorldSize;
			world_y_size_ = thisiYWorldSize;
			hex_ = newHex;
			break;
	}

	setScrollbarRange( );
	Refresh( false );
}

void MapPane::CmShrinkMap( wxCommandEvent& WXUNUSED(event) )
{
	DExtendMap dl( this );
	dl.setCaption( "Shrink map" );
	if ( dl.ShowModal( ) == FALSE )
		return;

	int cnt, dir;
	dl.getData( &cnt, &dir );

	if ( cnt <= 0 || ( dir != MapPane::RESIZE_MAP_TO_NORTH && dir != MapPane::RESIZE_MAP_TO_SOUTH && dir != MapPane::RESIZE_MAP_TO_WEST && dir != MapPane::RESIZE_MAP_TO_EAST ) )
	{
		wxMessageBox( "You must specify both count and direction!" );
		return;
	}

	// odd number of new rows causes TROUBLE!
	if ( ( dir == MapPane::RESIZE_MAP_TO_NORTH || dir == MapPane::RESIZE_MAP_TO_SOUTH ) && ( cnt & 1 ) )
	{
		wxMessageBox( "Number of rows must be even!" );
		return;
	}

	Hex** newHex;
	switch ( dir )
	{
		case MapPane::RESIZE_MAP_TO_SOUTH:
			newiYWorldSize = world_y_size_ - cnt;
			newiXWorldSize = world_x_size_;
			if ( ! allocate_new_map_memory( &newHex, newiXWorldSize, newiYWorldSize ) )
			{
				wxMessageBox( "Could not get memory!" );
				return;
			}
			copy_sectors_shrink( newHex, 0, 0 );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = newiXWorldSize;
			world_y_size_ = newiYWorldSize;
			hex_ = newHex;
			break;
		case MapPane::RESIZE_MAP_TO_NORTH:
			newiYWorldSize = world_y_size_ - cnt;
			newiXWorldSize = world_x_size_;
			if ( ! allocate_new_map_memory( &newHex, newiXWorldSize, newiYWorldSize ) )
			{
				wxMessageBox( "Could not get memory!" );
				return;
			}
			copy_sectors_shrink( newHex, 0, cnt );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = newiXWorldSize;
			world_y_size_ = newiYWorldSize;
			hex_ = newHex;
			break;
		case MapPane::RESIZE_MAP_TO_EAST:
			newiYWorldSize = world_y_size_;
			newiXWorldSize = world_x_size_ - cnt;
			if ( ! allocate_new_map_memory( &newHex, newiXWorldSize, newiYWorldSize ) )
			{
				wxMessageBox( "Could not get memory!" );
				return;
			}
			copy_sectors_shrink( newHex, 0, 0 );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = newiXWorldSize;
			world_y_size_ = newiYWorldSize;
			hex_ = newHex;
			break;
		case MapPane::RESIZE_MAP_TO_WEST:
			newiYWorldSize = world_y_size_;
			newiXWorldSize = world_x_size_ - cnt;
			if ( ! allocate_new_map_memory( &newHex, newiXWorldSize, newiYWorldSize ) )
			{
				wxMessageBox( "Could not get memory!" );
				return;
			}
			copy_sectors_shrink( newHex, cnt, 0 );
			release_map_storage( world_y_size_, world_x_size_, false );
			world_x_size_ = newiXWorldSize;
			world_y_size_ = newiYWorldSize;
			hex_ = newHex;
			break;
	}

	setScrollbarRange( );
	Refresh( true );
}

void MapPane::CmInsertMapHere( wxCommandEvent& WXUNUSED(event) )
{
	const wxString& s = wxFileSelector( "Choose file to insert", "", "", "*.map", "*.map" );
	if ( ! s )
		return;

	// load new map
	ifstream is( s );
	if ( ! is )
	{
		const wxString& message( _("Couldn't open file") + s + _("!") );
		wxMessageBox( message );
		return;
	}

	// check if this is valid map-file
	int ver;
	is >> ver;
	if ( ver != ht::MAP_VERSION )
	{
		const wxString& message( _("This map file uses unrecognized format!\n\nLoad it anyway???") );
		if ( wxMessageBox( message, _("Warning!"), wxYES_NO ) != wxYES )
			return;
	}

	wxBeginBusyCursor( );
	int x_size, y_size;
	is >> y_size;
	is >> x_size;

	// discard extended stuff
	char msg[50];
	if ( ver >= 8 )
	{
		while ( 1 )
		{
			is >> msg;
			if ( strcmp( msg, "Ext>" ) == 0 )
				continue;
			else if ( strcmp( msg, "<Ext" ) == 0 )
				break;
		}
	}

	Hex** tmp_hex;
	if ( ( tmp_hex = new Hex*[y_size] ) == nullptr )
	{
		wxMessageBox( "Can't allocate memory for map" );
		return;
	}

	for ( int i = 0; i < y_size; ++i )
	{
		if ( ( tmp_hex[i] = new Hex[x_size] ) == nullptr )
		{
			wxMessageBox( "Can't allocate memory map!\n" );
			for ( int j = 0; j < i; ++j )
				delete[] tmp_hex[j];
			delete tmp_hex;
			return;
		}
	}

	// load new map
	for ( int y = 0; y < y_size; ++y )
		for ( int x = 0; x < x_size; ++x )
			is >> tmp_hex[y][x];

	// copy hexes from new map to old
	int odd_row = ( selected_hex_point_.y & 1 );
	for ( int y = selected_hex_point_.y; y < world_y_size_; ++y )
	{
		if ( y >= selected_hex_point_.y + y_size )
			break;

		for ( int x = selected_hex_point_.x; x < world_x_size_; ++x )
		{
			if ( x - selected_hex_point_.x >= x_size )
				break;

			// don't copy 'NONE' hexes (they are considered "transparent")
			if ( tmp_hex[y - selected_hex_point_.y][x - selected_hex_point_.x].terrain_ == HexType::NONE )
				continue;

			if ( odd_row && ( y & 1 ) && ( x > 0 ) )
				hex_[y][x - 1] 	= tmp_hex[y - selected_hex_point_.y][x - selected_hex_point_.x];
			else
				hex_[y][x]		= tmp_hex[y - selected_hex_point_.y][x - selected_hex_point_.x];

		}
	}

	// free memory
	for ( int y = 0; y < y_size; ++y )
	{
		// but don't free cities:
		//  they are referenced from the real map now
		for ( int x = 0; x < x_size; ++x )
			tmp_hex[y][x].city_ = nullptr;

		delete[] tmp_hex[y];
		tmp_hex[y] = nullptr;
	}
	delete[] tmp_hex;
	tmp_hex = nullptr;

	Refresh( );
	wxEndBusyCursor( );
}

void MapPane::CmMapsheetOrientation( wxCommandEvent& WXUNUSED(event) )
{
	inventory_mapsheets( );

	const wxString& mapIdStr = wxGetTextFromUser( _("Enter map ID of map to change") );
	// TODO: refactor these magic numbers
	if ( ! mapIdStr || mapIdStr.Len( ) < 4 || mapIdStr.Len( ) > 5 )
		return;

	char mapid[ht::MAX_MAP_ID_LEN];
	strcpy( mapid, mapIdStr );

	char msg[100];
	int x, y;
	if ( ! find_map_origin( mapid, &x, &y ) )
	{
		sprintf( msg, "Map '%s' does not exist!", mapid );
		wxMessageBox( msg, "Error" );
		return;
	}

	wxBeginBusyCursor( );
	set_rotated( mapid, ! mapsheet_rotated( mapid ) );
	wxEndBusyCursor( );

	sprintf( msg, "Mapsheet '%s' orientation is now '%s'.\n"
			"Select \"Recalc hex ids\" command to\n"
			"update the map.\n", mapid, mapsheet_rotated( mapid ) ? "rotated" : "normal" );
	wxMessageBox( msg );

	if ( mapsheet_rotated( mapid ) )
		return;

	// for normal orientation, also need to allow "right" angled offset, vice default "left" angled
	const wxString& normalAngleStr = wxGetTextFromUser( _("Enter '<' (left-angled) or '>' (right-angled) offset pattern") );

	wxBeginBusyCursor( );
	set_right_offset( mapid, normalAngleStr.Cmp( ">" ) == 0 );
	wxEndBusyCursor( );
}

void MapPane::CmListMapsheets( wxCommandEvent& WXUNUSED(event) )
{
	char msg[100];
	char s[10000];
	strcpy( s, "" );

	wxBeginBusyCursor( );

	inventory_mapsheets( );

	int x_org = 1;
	int y_org = 1;

	int sheetindex = 0;
	while ( sheetindex < mapsheet_count_ )
	{
		char* mapid = mapsheet_name( sheetindex );
		if ( find_map_origin( mapid, &x_org, &y_org ) )
		{
			// note:  take care...pre-increment of sheetindex handles two things:
			//		[] proper one-based mapsheet enumeration reporting to user, and
			//		[] loop counter maintenance
			sprintf(	msg,
						"%d. Mapsheet '%s'\t\torigin: %d,%d\torientation: %s %s  \n",
						++sheetindex,
						mapid,
						x_org,
						y_org,
						mapsheet_rotated( mapid ) ? "rotated" : "normal ",
						! mapsheet_rotated( mapid ) ? mapsheet_normal_right_offset( mapid ) ? "'>'" : "'<'" : "" );
		}
		else
		{
			sprintf( msg, "Bug: no map '%s' found, please report this bug \n", mapid );
			wxMessageBox( msg, _("Error") );
		}
		strcat( s, msg );
	}

	wxEndBusyCursor( );

	wxMessageBox( s, _("Mapsheet Inventory") );
}

void MapPane::CmCalcHexID( wxCommandEvent& WXUNUSED(event) )
{
	// TODO: vet this new instance of calling
	inventory_mapsheets( );

	wxString oneMap = wxGetTextFromUser( "Enter map ID to recalc (e.g. '4^^^', '12^a', or 'ALL' for all maps)" );
	if ( oneMap.Len( ) == 0 )
		return;

	// default 0101 origin
	int y_offset = 0;
	int x_offset = 0;

	bool recalculateAll = false;

	if ( strncmp( oneMap.ToAscii( ), "ALL", 3 ) == 0 )
		recalculateAll = true;
	else
	{
		int x, y;
		if ( ! find_map_origin( oneMap.ToAscii( ), &x, &y ) )
		{
			wxMessageBox( "Map '" + oneMap + "' does not exist!", "Error" );
			return;
		}
		else
		{
			if ( ! mapsheet_rotated( oneMap.ToAscii( ) ) )
			{
				wxString offsetOrigin = wxGetTextFromUser( "Enter any non-standard origin (number): " );
				// 3 to 4 numeric digits
				if ( offsetOrigin.Length( ) > 2 && offsetOrigin.Length( ) < 5 && offsetOrigin.IsNumber( ) )
				{
					int hexNumber = wxAtoi( offsetOrigin );
					int yy = hexNumber / 100; // yy00
					int xx = hexNumber % 100; // 00xx
					// legitimate hex ID
					if ( hexNumber > ht::DEFAULT_HEX_ID ) // otherwise no offset, use conventional 0101 origin
					{
						y_offset = yy - 1;
						x_offset = xx - 1;
					}
				}
			}
		}
	}

	wxBeginBusyCursor( );
	inventory_mapsheets( );

	int sheetindex = 0;
	while ( sheetindex < mapsheet_count_ )
	{
		char* sheetname = mapsheet_name( sheetindex );

		if ( ! recalculateAll && strcmp( sheetname, oneMap.ToAscii( ) ) )
		{
			++sheetindex;
			continue;
		}

		int x_org, y_org;
		if ( ! find_map_origin( sheetname, &x_org, &y_org ) )
		{
			char msg[50];
			sprintf( msg, "Bug: no map '%s' found, please report this bug", sheetname );
			wxMessageBox( msg, "Error" );
			wxEndBusyCursor( );
			return;
		}
		int m = hex_[y_org][x_org].getMapIdWord( );

		if ( ! mapsheet_rotated( sheetname ) ) // "normal" orientation
		{
			// hexnumbering starts from an upper-right origin
			// and default "left-angled" offset:
			//    0102  0101   <-- origin (0101) is always topmost row in rightmost col
		    // 0202  0201      <-- first of the next row is southwest of the origin
			//    0302  0301
			// note: on the hard copy map, this is shown in yyxx (col:row) format
			bool normalRightOffset = mapsheet_normal_right_offset( sheetname );

			int hexnumberY, hexnumberX;
			for ( int y = 0; y < world_y_size_; ++y )
			{
				hexnumberY = ( y - y_org ) + 1 + y_offset;

				int x_fix = 0;
				if ( ! normalRightOffset )	// default offset
					x_fix = ( ( y_org % 2 == 0 ) && ( y & 1 ) ) ? -1 : 0;
				else						// right offset
					x_fix = ( ( y_org & 1 ) && ( y % 2 == 0 ) ) ? +1 : 0;

				for ( int x = world_x_size_ - 1; x >= 0; --x )
				{
					hexnumberX = x_org - x + 1 + x_offset;
					if ( hex_[y][x].getMapIdWord( ) == m )
					{
						// this additional offset handles the even-hex-numbered rows
						// for "right-angled" offset:
						// 0102  0101    <-- origin (0101) still topmost row in rightmost col
						//    0202  0201 <-- first of the next row is southEAST of the origin
						// 0302  0301
						hex_[y][x].setHexNumber( hexnumberY * 100 + hexnumberX + x_fix );
					}
				}
			}
		}
		else								// "rotated" orientation
		{
			// hexnumbering starts from the upper-left origin:
			//             0101   <-- origin (0101) is always topmost hex
			//          0102  0201  0301
			//       0103  0202  0302  0401  0501
			//    0104  0203  0303  0402  0502 0601  0701
			// 0105  0204  0304  0403  0503 0602  0702  0801  0901

			// x	y 	 y = f(x) = (int)( 3x / 2 )
			// -------
			// 0	0
			// 1	1
			// 2	3
			// 3	4
			// 4	6
			// 5	7
			// 6	9
			// 7	10

			// x_yone is x-coord where xx is 01 (hex number is xxyy)
			// x_xone is x-coord where yy is 01 (hex number is xxyy)
			int xx_x_fix = ( y_org & 1 ) ? 0 : 1;
			int yy_x_fix = ( y_org & 1 ) ? 1 : 0;
			for ( int y = y_org; y < world_y_size_; ++y )
				for ( int x = 0; x < world_x_size_; ++x )
					if ( hex_[y][x].getMapIdWord( ) == m )
					{
						int x_xone = x_org - ( ( y - y_org ) / 2 );
						if ( y & 1 ) // odd y
							x_xone -= xx_x_fix;

						int xx = ht::limit( ( x - x_xone ) + 1, 0, ht::NoUpperBound );

						int x_yone = ( ( ( y - y_org ) * 3 ) / 2 ) + x_org;
						if ( ( y & 1 ) == 0 ) // even y
							x_yone += yy_x_fix;

						int yy = ht::limit( ( ( x_yone - x ) / 2 ) + 1, 0, ht::NoUpperBound );

						hex_[y][x].setHexNumber( xx * 100 + yy );
					}
		}

		if ( ! recalculateAll )
			break;

		++sheetindex;
	}
	wxEndBusyCursor( );
	Refresh( );
}

// copies sectors from Hex to ptr
// from SMALL MAP to LARGE MAP
void MapPane::copy_sectors_expand( Hex** ptr, int x_offset, int y_offset )
{
	for ( int y = 0; y < world_y_size_; y++ )
		for ( int x = 0; x < world_x_size_; x++ )
			ptr[y + y_offset][x + x_offset] = hex_[y][x];
}

// copies sectors from Hex to ptr
// from LARGE MAP to SMALL MAP
void MapPane::copy_sectors_shrink( Hex** ptr, int x_offset, int y_offset )
{
	for ( int y = 0; y < newiYWorldSize; y++ )
		for ( int x = 0; x < newiXWorldSize; x++ )
			ptr[y][x] = hex_[y + y_offset][x + x_offset];
}

// allocate mem for pointer (for resizing)
bool MapPane::allocate_new_map_memory( Hex*** ptr, int x, int y )
{
	// mem for columns
	if ( ( *ptr = new Hex*[y] ) == nullptr )
		return false;

	for ( int i = 0; i < y; ++i )
	{
		// mem for rows
		if ( ( ( *ptr )[i] = new Hex[x] ) == nullptr )
		{
			wxMessageBox( wxT("No mem for Rows...") );
			return false;
		}
	}

	return true;
}

char* MapPane::mapsheet_name( int sheetindex )
{
	return mapsheet_[sheetindex];
}

int MapPane::mapsheet_index( char* sheetname )
{
	for ( int i = 0; i < mapsheet_count_; ++i )
		if ( strcmp( mapsheet_[i], sheetname ) == 0 )
			return i;
	return -1;
}
#if 0
// no need for this complexity, just use mapsheet_string function in lieu:
char* MapPane::mapSheetName( int i )
{
	if ( map_rightoffset( i ) && ! map_rotated( i ) )
		return rightoffset_map[i];

	else if ( map_rotated( i ) && ! map_rightoffset( i ) )
		return rotated_map[i];

	// TODO: remove magic string, use a pre-set constant bogus map name
	return const_cast<char*>( "BOGUS" );
}
#endif

int MapPane::rotated_mapsheet_count( void )
{
	int count = 0;
	for ( int i = 0; i < mapsheet_count_; ++i )
		if ( strlen( rotated_map[i] ) > 0 )
			++count;
	return count;
}

int MapPane::rightoffset_mapsheet_count( void )
{
	int count = 0;
	for ( int i = 0; i < mapsheet_count_; ++i )
		if ( strlen( rightoffset_map[i] ) > 0 )
			++count;
	return count;
}

// is/should be the only function in which
//	mapsheet_[] and mapsheet_count_ are modified
void MapPane::inventory_mapsheets( )
{
	char s[ht::MAX_MAP_ID_LEN];

	mapsheet_count_ = 0;

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = world_x_size_ - 1; x >= 0; --x )
		{
			hex_[y][x].getMapIdStr( s );
			bool found = false;
			for ( int i = 0; i < mapsheet_count_; ++i )
			{
				found = strcmp( mapsheet_[i], s ) == 0;
				if ( found )
					break;
			}
			if ( ! found )
				strcpy( mapsheet_[mapsheet_count_++], s );
		}
}

void MapPane::set_rotated( const char* sheetname, bool do_rotate )
{
	// unconditionally remove the mapsheet from the rotated and right-offset lists
	clear_odd_mapsheet( sheetname );
	// if flag is true, add sheet back to the list in the first empty slot encountered
	if ( do_rotate )
	{
		for ( int i = 0; i < ht::MAX_MAPSHEETS; ++i )
		{
			if ( strcmp( rotated_map[i], "" ) == 0 )
			{
				strcpy( rotated_map[i], sheetname );
				break;
			}
		}
	}
}

void MapPane::set_right_offset( const char* sheetname, bool do_offset )
{
	// unconditionally remove the mapsheet from the rotated and right-offset lists
	clear_odd_mapsheet( sheetname );
	// if flag is true, add sheet back to the list in the first empty slot encountered
	if ( do_offset )
	{
		for ( int i = 0; i < ht::MAX_MAPSHEETS; ++i )
		{
			if ( strcmp( rightoffset_map[i], "" ) == 0 )
			{
				strcpy( rightoffset_map[i], sheetname );
				break;
			}
		}
	}
}

// note: be sure to always call inventory_mapsheets( ) before
bool MapPane::find_map_origin( const char* sheetname, int* ox, int* oy )
{
	char s[ht::MAX_MAP_ID_LEN];

	// don't need to call inventory_mapsheets( ) before this call,
	//	because it's called before find_map_origin( ) in all cases
	if ( ! mapsheet_rotated( sheetname ) ) // "normal" orientation
	{
		// top to bottom, right to left
		for ( int y = 0; y < world_y_size_; ++y )
			for ( int x = world_x_size_ - 1; x >= 0; --x )
			{
				hex_[y][x].getMapIdStr( s );
				if ( strcmp( s, sheetname ) == 0 )
				{
					*ox = x;
					*oy = y;
					return true;
				}
			}
	}
	else								// "rotated" orientation
	{
		// top to bottom, left to right
		for ( int y = 0; y < world_y_size_; ++y )
			for ( int x = 0; x < world_x_size_ - 1; ++x )
			{
				hex_[y][x].getMapIdStr( s );
				if ( strcmp( s, sheetname ) == 0 )
				{
					*ox = x;
					*oy = y;
					return true;
				}
			}
	}
	return false; // no such map ID found anywhere on the map
}

// return false if normal mapsheet, true if rotated (like FtF map)
bool MapPane::mapsheet_rotated( const char* sheetname )
{
	for ( int i = 0; i < mapsheet_count_; ++i )
		if ( strcmp( rotated_map[i], sheetname ) == 0 )
			return true;

	return false;  // not found from the list of rotated maps
}

void MapPane::clear_all_rotated_mapsheets( )
{
	for ( int i = 0; i < ht::MAX_MAPSHEETS; ++i )
		strcpy( rotated_map[i], "" );
}

// return false if normal mapsheet left-angled offset, true if right-angled offset
bool MapPane::mapsheet_normal_right_offset( const char* sheetname )
{
	for ( int i = 0; i < mapsheet_count_; ++i )
		if ( strcmp( rightoffset_map[i], sheetname ) == 0 )
			return true;

	return false;  // not found from the list of normal right-angled offset maps
}

void MapPane::clear_all_right_offset_mapsheets( )
{
	for ( int i = 0; i < ht::MAX_MAPSHEETS; ++i )
		strcpy( rightoffset_map[i], "" );
}

void MapPane::clear_odd_mapsheet( const char* sheetname )
{
	for ( int i = 0; i < ht::MAX_MAPSHEETS; ++i )
	{
		if ( strcmp( rotated_map[i], sheetname ) == 0 )
			strcpy( rotated_map[i], "" );
		if ( strcmp( rightoffset_map[i], sheetname ) == 0 )
			strcpy( rightoffset_map[i], "" );
	}
}

#endif
