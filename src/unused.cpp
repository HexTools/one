#if 0
#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "hexrender.defs"
#include "counter.defs"
#include "ww2.defs"

class MapPaneUnused : public wxScrolledWindow
{
private:
	void initializeCCBitmaps( wxBitmap* hbms[4], wxColour fgnd, wxColour bgnd );
	void GreyHex( int x, int y );							// gray out given hex
	void HiLiteHexColour( int x, int y, int red, int green, int blue, int is_red );	// highlight hex
	int MajorityDefenderSide( int x, int y );
	bool IsOnlyArtillery( int x, int y );
	void ShowReplay( );					// move counters to old places and show replay dialog
	void HideReplay( );					// move counters to current places and hide replay dialog

	// stubbed out to get unused.cpp to compile
	bool coordinatesOutOfBounds( int x, int y, int x_max, int y_max ) { return false; }
	void PaintHex( int x, int y ) { }		// re-paint one hex
	void hex2Screen( int* x, int* y ) { }	// convert hex coordinates to screen
	bool isCurrentHex( int x, int y ) { return false; }

	// data references to get unused.cpp to compile
	int iXWorldSize;					// size of the world
	int iYWorldSize;
	float Scale;						// 1=32 pixels/hex, 2=16 pixels/hex, 4=8, ...

	HexContents** Sit;					// pointer to hex map (dynamic data)

	wxPen* hpenHiliteRed; 	      		// thick
	wxPen* hpenHiliteBlack;       		// thick
	int HiliteX;
	int HiliteY;						// hilighted hex ( HiliteX < 0 if none )

	wxClientDC* mapDC;
};

static const int unitBitmapWidth = 18;
static const int unitBitmapHeight = 13;

static void draw_arm( wxDC* hdc ) { }
static void draw_inf( wxDC* hdc ) { }
static void draw_mxd( wxDC* hdc ) { }
static void draw_mec( wxDC* hdc ) { }

// TODO: not used anywhere?
// initialize ALL XX counter bitmaps
void MapPaneUnused::initializeCCBitmaps( wxBitmap* hbms[4], wxColour fgnd, wxColour bgnd )
{
	if ( ( hbms[CC_INF] = new wxBitmap( unitBitmapWidth, unitBitmapHeight ) ) == nullptr )
		wxMessageBox( wxT("Failed to create Inf XXX bitmap!"), MSGHDR );

	if ( ( hbms[CC_TNK] = new wxBitmap( unitBitmapWidth, unitBitmapHeight ) ) == nullptr )
		wxMessageBox( wxT("Failed to create Arm XXX bitmap!"), MSGHDR );

	if ( ( hbms[CC_MXD] = new wxBitmap( unitBitmapWidth, unitBitmapHeight ) ) == nullptr )
		wxMessageBox( wxT("Failed to create Mixed XXX bitmap!"), MSGHDR );

	if ( ( hbms[CC_MEC] = new wxBitmap( unitBitmapWidth, unitBitmapHeight ) ) == nullptr )
		wxMessageBox( wxT("Failed to create Mech XXX bitmap!"), MSGHDR );

	wxBrush bg_brush( wxColour( 0, 0, 0 ), wxSOLID );
	bg_brush.SetColour( bgnd );

	wxPen fg_pen( fgnd, 1, wxSOLID );
	// fg_pen.SetColour( fgnd );

	wxMemoryDC hdc;

	hdc.SelectObject( *hbms[CC_TNK] );
	hdc.SetBrush( bg_brush );
	hdc.SetPen( *wxBLACK_PEN );
	hdc.DrawRectangle( 0, 0, unitBitmapWidth, unitBitmapHeight );
	hdc.SetPen( fg_pen );
	draw_arm( &hdc );

	hdc.SelectObject( *hbms[CC_INF] );
	hdc.SetPen( *wxBLACK_PEN );
	hdc.DrawRectangle( 0, 0, unitBitmapWidth, unitBitmapHeight );
	hdc.SetPen( fg_pen );
	draw_inf( &hdc );

	hdc.SelectObject( *hbms[CC_MXD] );
	hdc.SetPen( *wxBLACK_PEN );
	hdc.DrawRectangle( 0, 0, unitBitmapWidth, unitBitmapHeight );
	hdc.SetPen( fg_pen );
	draw_mxd( &hdc );

	hdc.SelectObject( *hbms[CC_MEC] );
	hdc.SetPen( *wxBLACK_PEN );
	hdc.DrawRectangle( 0, 0, unitBitmapWidth, unitBitmapHeight );
	hdc.SetPen( fg_pen );
	draw_mec( &hdc );
}

// TODO: not used anywhere?
// gray out hex
void MapPaneUnused::GreyHex( int x, int y )
{
	if ( coordinatesOutOfBounds( x, y, iXWorldSize, iYWorldSize ) )
		return;
	Sit[y][x].setGrayed( true );
	PaintHex( x, y );
}

// TODO: not used anywhere?
// hilite selected hex (just graphics)
void MapPaneUnused::HiLiteHexColour( int x, int y, int red, int green, int blue, int is_red )
{
	int x2, y2;
	//static int was_red = 1;

	if ( ! mapDC )
	{
		mapDC = new wxClientDC( this );
		if ( ! mapDC )
		{
			wxMessageBox( wxT("Panic: failed to allocate DC!"), wxT("FATAL ERROR"), 0 );
			return;
		}
	}

	mapDC->SetUserScale( 1.0 / Scale, 1.0 / Scale );

	x2 = x;
	y2 = y;
	hex2Screen( &x2, &y2 );

	if ( is_red )
	{
		hpenHiliteRed->SetColour( red, green, blue );
		mapDC->SetPen( *hpenHiliteRed );
	}
	else
	{
		mapDC->SetPen( *hpenHiliteBlack );
		// kludge #2232: save coordinates for Paint()
		HiliteX = x;
		HiliteY = y;
	}
	mapDC->DrawLine( x2, y2 + 3, x2 + Hex::HALF, y2 - 3 );
	mapDC->DrawLine( x2 + Hex::HALF, y2 - 3, x2 + Hex::SIZE_X, y2 + 3 );
	mapDC->DrawLine( x2 + Hex::SIZE_X, y2 + 3, x2 + Hex::SIZE_X, y2 + Hex::SIZE_Y - 3 );
	mapDC->DrawLine( x2 + Hex::SIZE_X, y2 + Hex::SIZE_Y - 3, x2 + Hex::HALF, y2 + Hex::SIZE_Y + 3 );
	mapDC->DrawLine( x2 + Hex::HALF, y2 + Hex::SIZE_Y + 3, x2, y2 + Hex::SIZE_Y - 3 );
	mapDC->DrawLine( x2, y2 + Hex::SIZE_Y - 3, x2, y2 + 3 );
	hpenHiliteRed->SetColour( *wxRED );
}

#define MAXSIDES 2

int MapPaneUnused::MajorityDefenderSide( int x, int y )
{
	int defender = -1;

	static int improved_fortress = 0;

	int side[MAXSIDES];

	Counter*  c;
	ENUM_HEXUNITS( x, y, c )
		if ( c->Type() == CNTR_MISC && ((Facility*)c)->GetMiscType() == MISC_FORT )
		{
			improved_fortress = 0;
			break;
		}

	return defender;
}

// is hex occupied by artillery only (defends with 1)?
bool MapPaneUnused::IsOnlyArtillery(int x, int y)
{
	Counter* c;
	ENUM_HEXUNITS( x, y, c )
	{
		if ( c->Type() != CNTR_UNIT )
			continue;

		GroundUnit* u = static_cast<GroundUnit*>( c );

		if (	u->GetType() == ART ||
				u->GetSize() == HQ  ||
				u->GetType() == LR_SIEGE_ART ||
				u->GetType() == MORTAR ||
				u->GetType() == RKT_ART ||
				u->GetType() == SIEGE_ART )
			continue;

		return false;
	}

	if ( isCurrentHex( x, y ) )
		ENUM_SELUNITS( c )
		{
			if ( c->Type() != CNTR_UNIT )
				continue;

			GroundUnit* u = static_cast<GroundUnit*>( c );

			if (	u->GetType() == ART ||
					u->GetSize() == HQ ||
					u->GetType() == LR_SIEGE_ART ||
					u->GetType() == MORTAR ||
					u->GetType() == RKT_ART ||
					u->GetType() == SIEGE_ART )
				continue;

			return false;
		}

	return true;
}

// move counters to old places and show replay dlg box
void MapPaneUnused::ShowReplay( )
{
	ShowPath = TRUE;
	if ( SelectedUnits )
		Refresh( );
}

// move counters to current places and hide replay dlg box
void MapPaneUnused::HideReplay( )
{
	ShowPath = FALSE;
	if ( SelectedUnits )
		Refresh( );
}

static char tmp[100];	// for use only in dbg_msg( )

// dump list of units:
void dbg_msg( )
{
	strcpy( msg, "" );

	int i = 0;
	Counter* unit = SelectedUnits;
	while ( unit )
	{
		unit->getUnitString( tmp );
		strcat( msg, tmp );
		strcat( msg, " -->\n" );
		unit = unit->getNext( );
		i++;
	}

	strcat( msg, "NULL\n" );
	sprintf( tmp, "Unitcount=%d", i );
	strcat( msg, tmp );
	sprintf( tmp, "%d units LineSelected", i );
	wxMessageBox( wxS( msg ), wxString::FromAscii( tmp ), wxOK );
}
#endif
#endif

// calculate MPs needed to travel path to destination
/* int MapPane::calcPathMP( Counter* cntr, const char* path, int xdst, int ydst )
{
	int mp = 0, x, y, dir = 0, dir2;
	const char* cp = path;

	if ( *path == '\0' )  // no path
		return 0;
	do
	{
		switch ( *cp )
		{
			case RPL_E:
				dir = WEST;
				break;
			case RPL_W:
				dir = EAST;
				break;
			case RPL_SW:
				dir = NORTHEAST;
				break;
			case RPL_SE:
				dir = NORTHWEST;
				break;
			case RPL_NW:
				dir = SOUTHEAST;
				break;
			case RPL_NE:
				dir = SOUTHWEST;
				break;
		}
		dir2 = ht::getAdjHexPart( dir );
		ht::getAdjHexCoords( dir, xdst, ydst, &x, &y, iXWorldSize, iYWorldSize );
		mp += calculateHexCost( cntr, x, y, dir2, 0 );  // TODO: DLT
		xdst = x;
		ydst = y;
		cp++;
	}
	while ( *cp );
	return mp;
}
*/

// move selected units to given hex
/* int MapPane::moveUnit( Counter* c, char* path, int x, int y )
{
	// flying:
	if ( c->getCounterType( ) == CNTR_PLANE )
	{
		wxMessageBox( wxT("TODO") );
		return FALSE;
	}
	// tac:
	if ( MoveMode == TACTICAL)
	{
		if ( c->getAdminMove( ) || c->getStratMove( ) )
		{
			MoveErr( "Can't combine administrative or strategic movement with tactical one!" );
			UnitWin->deselectUnit( c );
			return FALSE;
		}
		if ( c->getTrain( ) )
		{
			int mpleft = ( c->getMP( ) * c->getRailHexes( ) ) + c->getRailRem( );
			int mp = mpleft - calcPathMP( c, path, x, y );
			c->setMP( mp / c->getRailHexes( ) );
			c->setRailRem( mp % c->getRailHexes( ) );
		}
		else
			// on foot
			c->setMP( c->getMP( ) - calcPathMP( c, path, x, y ) );
		c->setTacticalMove( TRUE );
	}
	// admin:
	else if ( MoveMode == ADMIN)
	{
		if ( c->getTacticalMove( ) || c->getStratMove( ) )
		{
			MoveErr( "Can't combine tactical or strategic movement with administrative one!" );
			UnitWin->deselectUnit( c );
			return FALSE;
		}

		// how many MPs for this path
		int mp = calcPathMP( c, path, x, y );
		//int mprem = mp & 1; // same as mp % 2 for +int

		c->setMP( c->getMP( ) - mp / 2 );
		if ( mp % 2 )  // if there is a remainder (odd number of MPs) // mprem
		{
			if ( c->isHalfMP( ) )
				c->setHalfMP( 0 );  // remove the 1/2 MP
			else
			{
				c->setMP( c->getMP( ) - 1 );  // knock down full MP
				c->setHalfMP( 1 );  // then add the 1/2 MP
			}
		}
		c->setAdminMove( TRUE );
	}
	return TRUE;
}
*/

/*
 Network multiplayer game
 ========================

 Idea: There is a server running somewhere in the internet. Several
 players can connect this server. When someone moves counter everybody
 will see that counter moving on their screens.

 Network game: game state is stored at the server. When someone connects
 game state is downloaded from the server (just like File->Open).
 When someone changes game state in the client (moves or creates units etc)
 event is sent from client to server. Server updates game state and dispatces
 event to everyone else connected.

 Possible event are:
 - counter appears to hex
 - counter disappears from hex
 - counter's properties are changed
 - hex properties (Sit[][]) are changed
 - phase change
 - rules change
 - chatting

 Protocol is needed to transfer these events, for example:
 "<cmd> <agrs>"

 cmd:
 'a' add counter; "a <type> <data>"
 <type>: CNTR_xxx from GroundUnit.h
 <data>: result of opreator:<<(GroundUnit/AirUnit/...)

 'r' remove counter; "r <type> <data>"
 just like 'a'

 'p'	change counter (=change properties); "p <type> <old-data> <new-data>"

 'h'	change hex's properties; "h <x> <y> <data>"
 <x>: x-coordinate
 <y>: y-coordinate
 <data>: result of opreator:<<(HexContents)

 'p' phase changes; "p <new-phase>"
 <new-phase>: phase
 NOTE: server decides when phase ends. Menu command just sends wish
 to the server.

 'r' rule change; "r <...>"
 TODO

 'c' chat message; "c <who> <msg>"
 <who>: to whom to send message (0 = all)
 <msg>: rest of the packet is message


 Following changes would be needed:
 - MapPane::Load equivalent MapPane::DownLoad
 - hooks to send event to the server if local game state is modified
 - ability to receive asynchronous events from the server and update
 local game state

 - Necessary commands: "File->Connect", "Misc->Chat", ...

 */

/*
void MapPane::OnTimerTick( wxTimerEvent& )
{
	Counter* c;
	static int busy = 0;

	return;

	// this is no good:
	if ( busy )
		return;

	++busy;

	if ( ClickMarching )
	{
		UnitWin->ClearDeselected( );
		char* path = GetMovementPath( MarchTargetX, MarchTargetY );

		// are all units ok to move?
		int ok = 0;
		ENUM_SELUNITS( c )
		{
			if ( c->GetSide() != RealPlayer )
			{
				UnitWin->DeselectAll( );	// new hex selected, get rid of old
				UnitWin->Clear( );			// selections.
				break;
			}
			ok = 1;
		}

		if ( ! path || ! *path )
		{
			ClickMarching = 0;
			return;
		}

		if ( true )
		{
			// yes they were, now move
			// note: path is upsidedown
			char* cp = path;

			while ( *cp )				// seek to end
				++cp;

			--cp;

			int ox = ptCurrentHex.x;
			int oy = ptCurrentHex.y;

			EraseMovementPath();
			ClickMarching = TRUE;

			if ( cp >= path )			// travel backwards
				March( replay2dir( --(*cp_ ) );	// NOTE: March() alters current hex!

			ShowSelUnitMovementArea( FALSE );
			goto end;
		}
	}
end:
	busy = 0;
	return;
}
 */
