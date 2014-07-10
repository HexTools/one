#include <wx/wx.h>

#if defined HEXTOOLSPLAY
#endif
#include "hextools.h"
using ht::coordinatesOutOfBounds;
#if defined HEXTOOLSMAP
#include "hexsidetype.h"
#endif
#if defined HEXTOOLSPLAY
#include "dirs.loop"
#include "sideplayer.h"
#endif
#include "port.h"
#include "city.h"
#if defined HEXTOOLSPLAY
#include "hexcontents.h"
#endif
#include "hextype.h"
#include "hex.h"
#if defined HEXTOOLSMAP
#include "frame.h"
#endif
#include "application.h"
#include "mappane.h"
#if defined HEXTOOLSMAP
#include "terrainselector.h"
#include "dcity.h"
#endif

#if defined HEXTOOLSPLAY
extern int _weather_condition[]; //W_ZONES // W_MUD etc., per zone (frame.cpp)
#endif

#if defined HEXTOOLSMAP
// Add city to map
void MapPane::CmAddCity( wxCommandEvent& WXUNUSED(event) )
{
	if ( selected_hex_point_.y < 0 || selected_hex_point_.x < 0 )
		return;

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	// nullptr is for no city to edit (we're adding not editing)
	DCity dl( this, hex_[y][x].city_, nullptr );

	// empty city name by default
	dl.setCitySize( City::Type::MAPTEXT );	// Small Text
	dl.setCityPos( 5 );			// center (magic number from dcity.cpp)
	dl.setTxtPos( 2 );			// bottom (magic number from dcity.cpp)

	if ( dl.ShowModal( ) != wxID_OK )
		return;

	int size;
	if ( ( size = dl.getCitySize( ) ) == 0 )
	{
		wxMessageBox( wxT("You must specify city size!") );
		return;
	}

	char name[City::MAX_CITY_NAME_LEN]; // same as for City::achName
	name[0] = '\0';
	dl.getCityName( name );

	if ( City::illegalCityName( name ) )
	{
		if ( strlen( name ) > City::MAX_CITY_NAME_LEN - 1 )
		{
			name[City::MAX_CITY_NAME_LEN - 1] = '\0';
			wxMessageBox( wxT("You provided a city name (including any escape characters) longer than 19 characters.  It has been truncated!") );
		}
		else
		{
			// big cities default to black bold, everything else to non-displayed
			strcpy( name, size == City::Type::FULLHEX || size == City::Type::PARTIALHEX ? ";b" : ";n" );
			strcat( name, City::cityDefaultName( dl.getCitySize( ) ) );
		}
	}

	// although use City::hexFeatureProfile(..) restricts more than one primary
	//	or atoll/small island city from being added to a given hex, the below
	//	check makes sure (e.g. if the city profiling code is ever disabled)
	if ( ( size == City::Type::ATOLL_CUSTOM && hex_[y][x].terrain_ == HexType::ATOLL ) ||
		 ( size == City::Type::SMALL_ISLAND_CUSTOM && hex_[y][x].terrain_ == HexType::SMALLISLAND ) )
	{
		wxMessageBox( wxT("A city of this type already exists!") );
		return;
	}

	// grab reference to the list before setting the new city at the head
	City* next = hex_[y][x].city_;

	hex_[y][x].city_ = new City( name, size );
	hex_[y][x].city_->position_ = dl.getCityPos( );
	hex_[y][x].city_->text_position_ = dl.getTxtPos( );

	if ( size == City::Type::FULLHEX || size == City::Type::PARTIALHEX )
		hex_[y][x].terrain_ = HexType::CLEAR;

	if ( size == City::Type::ATOLL_CUSTOM )
		hex_[y][x].terrain_ = HexType::ATOLL;

	if ( size == City::Type::SMALL_ISLAND_CUSTOM )
		hex_[y][x].terrain_ = HexType::SMALLISLAND;

	// when a great/major city is in the current list
	if ( next && (	next->type_ == City::Type::FULLHEX ||
					next->type_ == City::Type::PARTIALHEX ||
					next->type_ == City::Type::ATOLL_CUSTOM ||
					next->type_ == City::Type::SMALL_ISLAND_CUSTOM ) )
	{
		// point the new "city" to the "city" after the great/major
		hex_[y][x].city_->setNext( next->getNext( ) );

		// point the great/major to the new "city"
		next->setNext( hex_[y][x].city_ );

		// then point the hex's city list to the great/major
		hex_[y][x].city_ = next;
	}
	else
	{
		// append the existing "city" list to the newly-added city
		//  this case also handles when "city" list is empty
		hex_[y][x].city_->setNext( next );
	}

	// convert hex coordinates to screen coordinates (same logic as hex2Screen(..))
	int sx, sy;
	CalcScrolledPosition( 0, 0, &sx, &sy );

	sx = static_cast<int>( sx * scale_ );
	sy = static_cast<int>( sy * scale_ );

	x = ( x * Hex::SIZE_X ) + ( y % 2 ) * ( Hex::SIZE_X / 2 ) + sx;
	y = y * Hex::SIZE_Y + sy;

	// now in screen coordinates
	x = static_cast<int>( x / scale_ );
	y = static_cast<int>( y / scale_ );

	wxRect rect;
	rect.x = x - static_cast<int>( ( Hex::SIZE_X << 1 ) / scale_ );
	rect.y = y - static_cast<int>( ( Hex::SIZE_Y << 1 ) / scale_ );
	rect.width = static_cast<int>( ( Hex::SIZE_X << 2 ) / scale_ );
	rect.height = static_cast<int>( ( Hex::SIZE_Y << 2 ) / scale_ );

	Refresh( false, &rect );
	file_is_dirty_ = true;
}

void MapPane::CmEditcity( wxCommandEvent& WXUNUSED(event) )
{
	if ( selected_hex_point_.y < 0 || selected_hex_point_.x < 0 )
		return;

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	City* city = hex_[y][x].city_;

	if ( ! city )
		return;

	if ( city->getNext( ) ) // if only one city, no need to prompt
	{
		while ( city )
		{
			wxString text( city->name_ );
			text = City::scrubCityNameStyleAndRotationEncodings( text );
			text = "Edit city " + text + "?";
			if ( wxMessageBox( text, "HexTools Map", wxYES_NO ) == wxYES )
				break;
			city = city->getNext( );
		}
	}
	if ( ! city )
		return; // iterated list of cities, user did not choose one

	DCity dl( this, hex_[y][x].city_, city );

	dl.setCityName( city->name_ );
	dl.setCitySize( city->type_ );
	dl.setCityPos( city->position_ );
	dl.setTxtPos( city->text_position_ );

	if ( dl.ShowModal( ) != wxID_OK )
		return;

	int size;
	if ( ( size = dl.getCitySize( ) ) == 0 )
		return;

	char name[City::MAX_CITY_NAME_LEN];
	dl.getCityName( name );

	if ( City::illegalCityName( name ) )
	{
		if ( strlen( name ) > City::MAX_CITY_NAME_LEN - 1 )
		{
			name[City::MAX_CITY_NAME_LEN - 1] = '\0';
			wxMessageBox( wxT("You provided a city name (including any escape characters) longer than 19 characters.  It has been truncated!") );
		}
		else
		{	// all city types default to non-displayed
			strcpy( name, ";n" );
			strcat( name, City::cityDefaultName( dl.getCitySize( ) ) );
		}
	}

	city->type_ = size;
	strcpy( city->name_, name );
	city->position_ = dl.getCityPos( );
	city->text_position_ = dl.getTxtPos( );

	// convert hex coordinates to screen coordinates (same logic as hex2Screen(..))
	int sx, sy;
	CalcScrolledPosition( 0, 0, &sx, &sy );

	sx = static_cast<int>( sx * scale_ );
	sy = static_cast<int>( sy * scale_ );

	x = ( x * Hex::SIZE_X ) + ( y % 2 ) * ( Hex::SIZE_X / 2 ) + sx;
	y = y * Hex::SIZE_Y + sy;

	// now in screen coordinates
	x = static_cast<int>( x / scale_ );
	y = static_cast<int>( y / scale_ );

	wxRect rect;
	rect.x = x - static_cast<int>( ( Hex::SIZE_X << 1 ) / scale_ );
	rect.y = y - static_cast<int>( ( Hex::SIZE_Y << 1 ) / scale_ );
	rect.width = static_cast<int>( ( Hex::SIZE_X << 2 ) / scale_ );
	rect.height = static_cast<int>( ( Hex::SIZE_Y << 2 ) / scale_ );

	Refresh( false, &rect );
	file_is_dirty_ = true;
}

void MapPane::CmRemoveCity( wxCommandEvent& WXUNUSED(event) )
{
	if ( selected_hex_point_.y < 0 || selected_hex_point_.x < 0 )
		return;

	int x = selected_hex_point_.x;
	int y = selected_hex_point_.y;

	City* city = hex_[y][x].city_;
	if ( ! city )
		return;

	City* prev = nullptr;
	if ( city->getNext( ) ) // if only one city, no need to prompt
	{
		while ( city )
		{
			wxString text( city->name_ );
			text = City::scrubCityNameStyleAndRotationEncodings( text );
			text = "Remove city " + text + "?";
			if ( wxMessageBox( text, "HexTools Map", wxYES_NO ) == wxYES )
				break;
			prev = city;
			city = city->getNext( );
		}
	}
	if ( ! city ) // iterated list of cities, user did not choose one
		return;

	if ( prev == nullptr ) // removing first city
		hex_[y][x].city_ = city->getNext( ); // will be nullptr if only one city
	else
		prev->setNext( city->getNext( ) ); // link out city

	delete city->name_;
	city->setNext( nullptr ); // because the City destructor will otherwise delete next, and so on

	// convert hex coordinates to screen coordinates (same logic as hex2Screen(..))
	int sx, sy;
	CalcScrolledPosition( 0, 0, &sx, &sy );

	sx = static_cast<int>( sx * scale_ );
	sy = static_cast<int>( sy * scale_ );

	x = ( x * Hex::SIZE_X ) + ( y % 2 ) * ( Hex::SIZE_X / 2 ) + sx;
	y = y * Hex::SIZE_Y + sy;

	// now in screen coordinates
	x = static_cast<int>( x / scale_ );
	y = static_cast<int>( y / scale_ );

	wxRect rect;
	rect.x = x - static_cast<int>( ( Hex::SIZE_X << 1 ) / scale_ );
	rect.y = y - static_cast<int>( ( Hex::SIZE_Y << 1 ) / scale_ );
	rect.width = static_cast<int>( ( Hex::SIZE_X << 2 ) / scale_ );
	rect.height = static_cast<int>( ( Hex::SIZE_Y << 2 ) / scale_ );

	Refresh( false, &rect );
	file_is_dirty_ = true;
}

void MapPane::CmLimitedStacking( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].toggleLimitedStackingHex( );
	Refresh( );
}

void MapPane::CmNoport( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( 0 );
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( 0 );
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortAttribute( 0 );
	Refresh( );
}

void MapPane::CmGreatport( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::GREAT );
	Refresh( );
}

void MapPane::CmMajorport( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MAJOR );
	Refresh( );
}

void MapPane::CmStandardport( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::STANDARD );
	Refresh( );
}

void MapPane::CmMinorport( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmMarginalport( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MARGINAL );
	Refresh( );
}

void MapPane::CmAnchorage( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::ANCHORAGE );
	Refresh( );
}

void MapPane::CmStrongport( wxCommandEvent& WXUNUSED(event) )
{
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortAttribute( ) == Port::Attribute::STRONG )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortAttribute( static_cast<int>( Port::Attribute::NORMAL ) );
	else
	{
		if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == Port::Type::UNSET )
			hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortAttribute( static_cast<int>( Port::Attribute::STRONG ) );
	}
	Refresh( );
}

void MapPane::CmArtificialport( wxCommandEvent& WXUNUSED(event) )
{
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortAttribute( ) == Port::Attribute::ARTIFICIAL )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortAttribute( static_cast<int>( Port::Attribute::NORMAL ) );
	else
	{
		if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == Port::Type::UNSET )
			hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortAttribute( static_cast<int>( Port::Attribute::ARTIFICIAL ) );
	}
	Refresh( );
}

void MapPane::CmPortCenter( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( 0 );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmPortNW( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( Hex::NORTHWEST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmPortNE( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( Hex::NORTHEAST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmPortW( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( Hex::WEST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmPortE( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( Hex::EAST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmPortSW( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( Hex::SOUTHWEST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmPortSE( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSymbolDir( Hex::SOUTHEAST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getPortSize( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setPortSize( Port::Type::MINOR );
	Refresh( );
}

void MapPane::CmNointraf( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 0 );
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( 0 );
	Refresh( );
}

void MapPane::CmIntraf1( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmIntraf3( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 3 );
	Refresh( );
}

void MapPane::CmAfCenter( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( 0 );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getIntrinsicAFCapacity( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmAfNW( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( Hex::NORTHWEST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getIntrinsicAFCapacity( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmAfNE( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( Hex::NORTHEAST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getIntrinsicAFCapacity( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmAfW( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( Hex::WEST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getIntrinsicAFCapacity( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmAfE( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( Hex::EAST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getIntrinsicAFCapacity( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmAfSW( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( Hex::SOUTHWEST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getIntrinsicAFCapacity( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmAfSE( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFSymbolDir( Hex::SOUTHEAST );
	if ( hex_[selected_hex_point_.y][selected_hex_point_.x].getIntrinsicAFCapacity( ) == 0 )
		hex_[selected_hex_point_.y][selected_hex_point_.x].setIntrinsicAFCapacity( 1 );
	Refresh( );
}

void MapPane::CmNoDam( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].clrDam( Hex::WEST );
	hex_[selected_hex_point_.y][selected_hex_point_.x].clrDam( Hex::SOUTHWEST );
	Refresh( );
}

void MapPane::CmDamSW( wxCommandEvent& WXUNUSED(event) )
{
	// vertices and sides are off-by-one by convention:
	//	SW vertex corresponds to W hexside
	hex_[selected_hex_point_.y][selected_hex_point_.x].setDam( Hex::WEST );
	Refresh( );
}

void MapPane::CmDamS( wxCommandEvent& WXUNUSED(event) )
{
	// vertices and sides are off-by-one by convention:
	//	S vertex corresponds to SW hexside
	hex_[selected_hex_point_.y][selected_hex_point_.x].setDam( Hex::SOUTHWEST );
	Refresh( );
}

void MapPane::CmToggleLateSeaIce( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].toggleLateSeaIce( );
	Refresh( );
}

void MapPane::CmToggleRestrictedWaters( wxCommandEvent& WXUNUSED(event) )
{
	hex_[selected_hex_point_.y][selected_hex_point_.x].toggleRestrictedWaters( );
	Refresh( );
}

// this enforces no-interior-mtn (on either side) except along Wadis (or Rivers)
void MapPane::clear_any_interior_mtn( int x, int y, int side )
{
	if ( hex_[y][x].getMountainInterior( side ) )
	{
		hex_[y][x].clrMountainInterior( side );
		hex_[y][x].updateInteriorHexsidesFlag( );
	}

	int x2, y2;
	int adj = ht::getAdjHexCoords( side, x, y, &x2, &y2, world_x_size_, world_y_size_ );
	int side2 = ht::getAdjHexPart( side );
	if ( adj && hex_[y2][x2].getMountainInterior( side2 ) )
	{
		hex_[y2][x2].clrMountainInterior( side2 );
		hex_[y2][x2].updateInteriorHexsidesFlag( );
	}
}

// clear mutually exclusive hexside features
void MapPane::clear_hex_side( int x, int y, int side )
{
	// 10 routes
	hex_[y][x].clrAllRoutes( side );

	// 12 water sides, plus restricted waters side
	hex_[y][x].clrMinorCanal( side );
	hex_[y][x].clrCanal( side );
	hex_[y][x].clrWadi( side );
	hex_[y][x].clrSeasonalRiver( side );
	hex_[y][x].clrStandaloneBridge( side );
	hex_[y][x].clrMinorRiver( side );
	hex_[y][x].clrFordableMajorRiver( side );
	hex_[y][x].clrMajorRiver( side );
	hex_[y][x].clrFordableGreatRiver( side );
	hex_[y][x].clrLakeSide( side ); // same as great river
	hex_[y][x].clrSeaSide( side );
	hex_[y][x].clrIcingSeaSide( side );
	hex_[y][x].clrRestrictedWaterSide( side );

	// 9 rare sides
	hex_[y][x].clrMountain( side );
	hex_[y][x].clrHighMtn( side );
	hex_[y][x].clrHighMtnPass( side );
	hex_[y][x].clrKarst( side );
	hex_[y][x].clrGlacier( side );
	hex_[y][x].clrSaltDesert( side );
	hex_[y][x].clrDoubleEscarpment( side );
	hex_[y][x].clrImpassableDoubleEscarpment( side );
	hex_[y][x].clrFillingReservoir( side );

	// by definition, clearing Minor Rivers or Wadis
	//	should also clear any interior mtn hexsides
	clear_any_interior_mtn( x, y, side );

	hex_[y][x].updateHexFlags( );
}

// set hexside according to selected hexside type
void MapPane::set_hex_side( int x, int y, int side )
{
	switch ( parent_->terrain_selector_->getSelectedType( ) )
	{
		case HexsideType::CLEAR_HEXSIDE:
			if ( hex_[y][x].getClearOrnament( side ) )
				hex_[y][x].clrClearOrnament( side );
			else
			{
				hex_[y][x].clrAllOrnaments( side );
				hex_[y][x].setClearOrnament( side );
			}
			break;
		case HexsideType::ROUGH_SIDE:
			if ( hex_[y][x].getRoughOrnament( side ) )
				hex_[y][x].clrRoughOrnament( side );
			else
			{
				hex_[y][x].clrAllOrnaments( side );
				hex_[y][x].setRoughOrnament( side );
			}
			break;
		case HexsideType::WOODS_SIDE:
			if ( hex_[y][x].getWoodsOrnament( side ) )
				hex_[y][x].clrWoodsOrnament( side ); // clears rough & forest
			else
			{
				hex_[y][x].clrAllOrnaments( side );
				hex_[y][x].setWoodsOrnament( side ); // sets rough & forest
			}
			break;
		case HexsideType::FOREST_SIDE:
			if ( hex_[y][x].getForestOrnament( side ) )
				hex_[y][x].clrForestOrnament( side );
			else
			{
				hex_[y][x].clrAllOrnaments( side );
				hex_[y][x].setForestOrnament( side );
			}
			break;
		case HexsideType::WOODEDROUGH_SIDE:
			if ( hex_[y][x].getWoodedRoughOrnament( side ) )
				hex_[y][x].clrWoodedRoughOrnament( side );
			else
			{
				hex_[y][x].clrAllOrnaments( side );
				hex_[y][x].setWoodedRoughOrnament( side );
			}
			break;

		case HexsideType::LIMITED_STACKING_SIDE:
			if ( hex_[y][x].getLimitedStacking( side ) )
				hex_[y][x].clrLimitedStacking( side );
			else
				hex_[y][x].setLimitedStacking( side );
			break;

		// routes (all 10 of these are mutex w/ each other, except lake/sea ferry,
		//			which needs to coexist with narrow straits in some cases)
		case HexsideType::TRACK:
			if ( hex_[y][x].getTrack( side ) && ! hex_[y][x].getRoad( side ) && ! hex_[y][x].getRailRoad( side ) )
				hex_[y][x].clrTrack( side );
			else
				hex_[y][x].setTrack( side );
			break;
		case HexsideType::MOTORTRACK:
			if ( hex_[y][x].getTrack( side ) && hex_[y][x].getRoad( side ) && ! hex_[y][x].getRailRoad( side ) )
				hex_[y][x].clrMotorTrack( side );
			else
				hex_[y][x].setMotorTrack( side );
			break;
		case HexsideType::ROAD:
			if ( hex_[y][x].getTrack( side ) == 0 && hex_[y][x].getRoad( side ) > 0
				&& hex_[y][x].getRailRoad( side ) == 0 && hex_[y][x].getNarrowStrait( side ) == 0 )
				hex_[y][x].clrRoad( side );
			else
				hex_[y][x].setRoad( side );
			break;
		case HexsideType::STANDALONEBRIDGE:
			if ( hex_[y][x].getStandaloneBridge( side ) )
				hex_[y][x].clrStandaloneBridge( side );
			else if ( hex_[y][x].getMinorRiver( side ) || hex_[y][x].getSeasonalRiver( side ) || hex_[y][x].getMajorRiver( side ) )
				hex_[y][x].setStandaloneBridge( side );
			break;
		case HexsideType::RAILTRACK:
			if ( hex_[y][x].getTrack( side ) && ! hex_[y][x].getRoad( side ) && hex_[y][x].getRailRoad( side ) )
				hex_[y][x].clrRailTrack( side );
			else
				hex_[y][x].setRailTrack( side );
			break;
		case HexsideType::RAILROAD:
			if ( ! hex_[y][x].getTrack( side ) && ! hex_[y][x].getRoad( side ) && hex_[y][x].getRailRoad( side ) )
				hex_[y][x].clrRailRoad( side );
			else
				hex_[y][x].setRailRoad( side );
			break;
		case HexsideType::RAIL_FERRY:
			if ( hex_[y][x].getRailFerry( side ) )
				hex_[y][x].clrRailFerry( side );
			else
			{
				// only set a rail ferry if the hexside is:
				//	- between two non-sea hexes, and
				//  - water (incl. major river, but not smaller rivers)
				int x2, y2;
				int hasAdj = ht::getAdjHexCoords( side, x, y, &x2, &y2, world_x_size_, world_y_size_ );

				if (	(	hex_[y][x].hasWaterSide( side ) ||
							hex_[y][x].getMajorRiver( side ) ||
							hex_[y][x].getFordableMajorRiver( side ) )
						&&
						( 	! hex_[y][x].isWaterHex( ) ||
							( hasAdj && ! hex_[y2][x2].isWaterHex( ) ) )
					)
				{
					hex_[y][x].setRailFerry( side );
				}
			}
			break;
		case HexsideType::NARROW_STRAITS:
			if ( hex_[y][x].getNarrowStrait( side ) && !hex_[y][x].getRoad( side ) )
				hex_[y][x].clrNarrowStrait( side );
			else if ( hex_[y][x].hasWaterSide( side ) )
				hex_[y][x].setNarrowStrait( side );
			break;
		case HexsideType::NARROW_STRAITS_FERRY:
			if ( hex_[y][x].getNarrowStraitsFerry( side ) )
				hex_[y][x].clrNarrowStraitsFerry( side );
			else if ( hex_[y][x].hasWaterSide( side ) )
				hex_[y][x].setNarrowStraitsFerry( side );
			break;
		case HexsideType::SHIP_CHANNEL:
			if ( hex_[y][x].getShippingChannel( side ) )
				hex_[y][x].clrShippingChannel( side );
			else
			{
				// only set a shipping channel if the hex is water
				//	or the adjacent hex (if there is one) is water
				int x2, y2;
				int hasAdj = ht::getAdjHexCoords( side, x, y, &x2, &y2, world_x_size_, world_y_size_ );
				if (	hex_[y][x].isWaterHex( )
						|| ( hasAdj && hex_[y2][x2].isWaterHex( ) ) )
				{
					hex_[y][x].setShippingChannel( side );
				}
			}
			break;
		case HexsideType::LAKESEA_FERRY:
			if ( hex_[y][x].getLakeSeaFerry( side ) )
				hex_[y][x].clrLakeSeaFerry( side );
			else
			{
				// only set a lake/sea ferry under the following 3 conditions (all must be true):
				// 1. current ferry count in from-hex must be 0 or 1, OR from-hex is terminal; AND
				// 2. there is a hex adjacent in the direction of the to-hex; AND
				// 3. current ferry count in to-hex must be 0 or 1, OR to-hex is terminal
				int x2, y2;
				if (	( get_lake_sea_ferry_count( x, y ) < 2 || hex_[y][x].hasFerryTerminal( ) ) &&
						ht::getAdjHexCoords( side, x, y, &x2, &y2, world_x_size_, world_y_size_ ) &&
						( get_lake_sea_ferry_count( x2, y2 ) < 2 || hex_[y2][x2].hasFerryTerminal( ) ) )
				{
					hex_[y][x].setLakeSeaFerry( side );
				}
			}
			break;

		case HexsideType::MINOR_CANAL:
			if ( hex_[y][x].getMinorCanal( side ) )
				hex_[y][x].clrMinorCanal( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setMinorCanal( side );
			}
			break;
		case HexsideType::CANAL:
			if ( hex_[y][x].getCanal( side ) )
				hex_[y][x].clrCanal( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setCanal( side );
			}
			break;
		case HexsideType::WADI:
			if ( hex_[y][x].getWadi( side ) )
			{
				hex_[y][x].clrWadi( side );
				clear_any_interior_mtn( x, y, side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setWadi( side );
			}
			break;
		case HexsideType::SEASONALRIVER:
			if ( hex_[y][x].getSeasonalRiver( side ) )
			{
				hex_[y][x].clrSeasonalRiver( side );
				hex_[y][x].clrInlandRouteBar( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setSeasonalRiver( side );
			}
			break;
		case HexsideType::MINORRIVER:
			if ( hex_[y][x].getMinorRiver( side ) )
			{
				hex_[y][x].clrMinorRiver( side );
				hex_[y][x].clrInlandRouteBar( side );
				clear_any_interior_mtn( x, y, side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setMinorRiver( side );
			}
			break;
		case HexsideType::MAJORRIVERFORD:
			if ( hex_[y][x].getFordableMajorRiver( side ) )
			{
				hex_[y][x].clrFordableMajorRiver( side );
				hex_[y][x].clrInlandRouteBar( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setFordableMajorRiver( side );
			}
			break;
		case HexsideType::MAJORRIVER:
			if ( hex_[y][x].getMajorRiver( side ) )
			{
				hex_[y][x].clrMajorRiver( side );
				hex_[y][x].clrInlandRouteBar( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setMajorRiver( side );
			}
			break;
		case HexsideType::GREATRIVERFORD:
			if ( hex_[y][x].getFordableGreatRiver( side ) )
			{
				hex_[y][x].clrFordableGreatRiver( side );
				hex_[y][x].clrInlandRouteBar( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setFordableGreatRiver( side );
			}
			break;
		case HexsideType::LAKE_SIDE:
			if ( hex_[y][x].getLakeSide( side ) )
			{
				hex_[y][x].clrLakeSide( side );
				hex_[y][x].clrInlandRouteBar( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setLakeSide( side );
			}
			break;
		case HexsideType::SEA_SIDE:
			if ( hex_[y][x].getSeaSide( side ) && ! hex_[y][x].getRestrictedWaterSide( side ) )
			{
				hex_[y][x].clrSeaSide( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setSeaSide( side );
				hex_[y][x].clrRestrictedWaterSide( side );
			}
			break;
		case HexsideType::RWATER_SIDE:
			if ( hex_[y][x].getSeaSide( side ) && hex_[y][x].getRestrictedWaterSide( side ) )
			{
				hex_[y][x].clrSeaSide( side );
				hex_[y][x].clrRestrictedWaterSide( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setSeaSide( side );
				hex_[y][x].setRestrictedWaterSide( side );
			}
			break;
		case HexsideType::ICING_SEA_SIDE:
			if ( hex_[y][x].getIcingSeaSide( side ) && ! hex_[y][x].getRestrictedWaterSide( side ) )
			{
				hex_[y][x].clrIcingSeaSide( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setIcingSeaSide( side );
				hex_[y][x].clrRestrictedWaterSide( side );
			}
			break;
		case HexsideType::ICING_RWATER_SIDE:
			if ( hex_[y][x].getIcingSeaSide( side ) && hex_[y][x].getRestrictedWaterSide( side ) )
			{
				hex_[y][x].clrIcingSeaSide( side );
				hex_[y][x].clrRestrictedWaterSide( side );
			}
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setIcingSeaSide( side );
				hex_[y][x].setRestrictedWaterSide( side );
			}
			break;
		case HexsideType::INLAND_SHIPPING:
			if ( hex_[y][x].getInlandShippingChannel( side ) )
				hex_[y][x].clrInlandShippingChannel( side );
			else
			{
				if ( hex_[y][x].terrain_ == HexType::SEA || hex_[y][x].terrain_ == HexType::ICINGSEA )
				{
					hex_[y][x].setInlandShippingChannel( side );
					break; // INLAND_SHIPPING case
				}
				// need to check the 3 adjoining hexes
				//	for All Sea/Lake hex terrain
				int sidemask = 0x00;
				switch ( side )
				{
					case Hex::WEST:
						sidemask = 0x31;	// 00110001, NW-W-SW
						break;
					case Hex::SOUTHWEST:
						sidemask = 0x38;	// 00111000, W-SW-SE
						break;
					case Hex::SOUTHEAST:
						sidemask = 0x1c;	// 00011100, SW-SE-E
						break;
				}
				int x2, y2;
				int hasAdj = 0;
				bool adjWater = false;
				for ( int sd = Hex::WEST; sd >= Hex::NORTHWEST; sd >>= 1 )
				{
					if ( sd & sidemask )
					{
						hasAdj = ht::getAdjHexCoords( sd, x, y, &x2, &y2, world_x_size_, world_y_size_ );
						if ( hasAdj &&
						( hex_[y2][x2].terrain_ == HexType::SEA || hex_[y2][x2].terrain_ == HexType::ICINGSEA ) )
						{
							adjWater = true;
							break; // from the side check, not the inland shipping case
						}
					}
				}
				if ( adjWater )
				{
					hex_[y][x].setInlandShippingChannel( side );
					break; // INLAND_SHIPPING case
				}

				// could already be inland -- check 4 hexsides adjacent to
				//	the hexside being set for inland shipping
				switch ( side )
				{
					case Hex::WEST: // SW hexside; SE hexside of W hex; SW-SE hexsides of NW hex
						if ( hex_[y][x].getInlandShippingChannel( Hex::SOUTHWEST ) )
						{
							adjWater = true;
							break;
						}
						hasAdj = ht::getAdjHexCoords( Hex::WEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
						if ( hasAdj && hex_[y2][x2].getInlandShippingChannel( Hex::SOUTHEAST ) )
						{
							adjWater = true;
							break;
						}
						hasAdj = ht::getAdjHexCoords( Hex::NORTHWEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
						if ( hasAdj
						&& ( hex_[y2][x2].getInlandShippingChannel( Hex::SOUTHEAST )
							|| hex_[y2][x2].getInlandShippingChannel( Hex::SOUTHWEST ) ) )
						{
							adjWater = true;
							break;
						}
						break;
					case Hex::SOUTHWEST: // W & SE hexsides; W hexside of SE hex; SE hexside of W hex
						if ( hex_[y][x].getInlandShippingChannel( Hex::WEST )
						|| hex_[y][x].getInlandShippingChannel( Hex::SOUTHEAST ) )
						{
							adjWater = true;
							break;
						}
						hasAdj = ht::getAdjHexCoords( Hex::WEST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
						if ( hasAdj && hex_[y2][x2].getInlandShippingChannel( Hex::SOUTHEAST ) )
						{
							adjWater = true;
							break;
						}
						hasAdj = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
						if ( hasAdj && hex_[y2][x2].getInlandShippingChannel( Hex::WEST ) )
						{
							adjWater = true;
							break;
						}
						break;
					case Hex::SOUTHEAST: // SW hexside; W hexside of SE hex; W-SW hexsides of E hex
						if ( hex_[y][x].getInlandShippingChannel( Hex::SOUTHWEST ) )
						{
							adjWater = true;
							break;
						}
						hasAdj = ht::getAdjHexCoords( Hex::SOUTHEAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
						if ( hasAdj && hex_[y2][x2].getInlandShippingChannel( Hex::WEST ) )
						{
							adjWater = true;
							break;
						}
						hasAdj = ht::getAdjHexCoords( Hex::EAST, x, y, &x2, &y2, world_x_size_, world_y_size_ );
						if ( hasAdj
						&& ( hex_[y2][x2].getInlandShippingChannel( Hex::WEST )
							|| hex_[y2][x2].getInlandShippingChannel( Hex::SOUTHWEST ) ) )
						{
							adjWater = true;
							break;
						}
						break;
				}
				if ( adjWater )
				{
					hex_[y][x].setInlandShippingChannel( side );
					hex_[y][x].clrInlandRouteBar( side );
				}
			} // end-else inland shipping channel not already set
			break; // INLAND_SHIPPING case
		case HexsideType::WATERWAY:
			if ( hex_[y][x].getWaterwayRoute( side ) )
				hex_[y][x].clrWaterwayRoute( side );
			else
			{
				// only set a waterway route if the hexside is water
				//	or the adjacent hex (if there is one) is water
				// TODO: the hasWaterSide / hasAnyWater logic needs to check for rivers
				int x2, y2;
				int hasAdj = ht::getAdjHexCoords( side, x, y, &x2, &y2, world_x_size_, world_y_size_ );
				if ( hex_[y][x].isWaterHex( )
				|| ( hasAdj && hex_[y2][x2].isWaterHex( ) )
				|| hex_[y][x].hasWaterSide( side ) // inland waterway (includes great river)
				|| hex_[y][x].getFordableGreatRiver( side ) // inland waterway (rivers)
				|| hex_[y][x].getMajorRiver( side )
				|| hex_[y][x].getFordableMajorRiver( side )
				|| hex_[y][x].getStandaloneBridge( side )
				|| hex_[y][x].getMinorRiver( side )
				|| hex_[y][x].getSeasonalRiver( side ) // NOTE: no canals!
				|| hex_[y][x].getCanal( side )
				|| hex_[y][x].getMinorCanal( side )
				|| hex_[y][x].hasAnyWater( )			// coastal waterway
				|| ( hasAdj && hex_[y2][x2].hasAnyWater( ) ) )
				{
					hex_[y][x].setWaterwayRoute( side );
					hex_[y][x].clrInlandRouteBar( side );
				}
			}
			break;
		case HexsideType::WATERWAY_BAR:
			if ( hex_[y][x].getInlandRouteBar( side ) )
				hex_[y][x].clrInlandRouteBar( side );
			else
			{
				// TODO: vet these 2 (premature?) calls
				hex_[y][x].clrWaterwayRoute( side );
				hex_[y][x].clrInlandShippingChannel( side );
				// bar to waterway movement only along rivers
				if ( hex_[y][x].getLakeSide( side ) // same as great river
				|| hex_[y][x].getFordableGreatRiver( side )
				|| hex_[y][x].getMajorRiver( side )
				|| hex_[y][x].getFordableMajorRiver( side )
				|| hex_[y][x].getStandaloneBridge( side )
				|| hex_[y][x].getMinorRiver( side )
				|| hex_[y][x].getSeasonalRiver( side ) ) // NOTE: no canals can be barred!
				{
					hex_[y][x].setInlandRouteBar( side );
					hex_[y][x].clrWaterwayRoute( side ); // mutex w/ waterway net extent
					hex_[y][x].clrInlandShippingChannel( side ); // mutex w/ port access route
				}
			}
			break;

		// the 9 rare terrain hexsides
		// 	they are all regulated with the rare sides flag
		case HexsideType::MTN_SIDE:
			if ( hex_[y][x].getMountain( side ) )
				hex_[y][x].clrMountain( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setMountain( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::HIGH_MTN_SIDE:
			if ( hex_[y][x].getHighMtn( side ) )
				hex_[y][x].clrHighMtn( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setHighMtn( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::HIGH_MTN_PASS:
			if ( hex_[y][x].getHighMtnPass( side ) )
				hex_[y][x].clrHighMtnPass( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setHighMtnPass( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::KARST:
			if ( hex_[y][x].getKarst( side ) )
				hex_[y][x].clrKarst( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setKarst( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::GLACIER_SIDE:
			if ( hex_[y][x].getGlacier( side ) )
				hex_[y][x].clrGlacier( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setGlacier( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::SALT_DESERT_SIDE:
			if ( hex_[y][x].getSaltDesert( side ) )
				hex_[y][x].clrSaltDesert( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setSaltDesert( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::DBL_ESCARPMENT:
			if ( hex_[y][x].getDoubleEscarpment( side ) )
				hex_[y][x].clrDoubleEscarpment( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setDoubleEscarpment( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::IMP_DBL_ESCARPMENT:
			if ( hex_[y][x].getImpassableDoubleEscarpment( side ) )
				hex_[y][x].clrImpassableDoubleEscarpment( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setImpassableDoubleEscarpment( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;
		case HexsideType::FILLING_RESERVOIR_SIDE:
			if ( hex_[y][x].getFillingReservoir( side ) )
				hex_[y][x].clrFillingReservoir( side );
			else
			{
				clear_hex_side( x, y, side );
				hex_[y][x].setFillingReservoir( side );
			}
			hex_[y][x].updateRareHexsidesFlag( );
			break;

		// with the exception of demarcation and internal sub-border alone,
		//	these 4 borders can co-exist and (when rendered) underlay each other
		//	** this is the z order of display from top to bottom:
#if 0
		case HexsideType::DEMARCATION:	// bBorder2 HI
			// can only toggle demarcation for int'l or internal borders
			if ( hex[y][x].getInternationalBorder( side ) || hex[y][x].getInternalBorder( side ) )
				hex[y][x].toggleDemarkationLine( side );	// toggle bBorder2 HI
			if ( hex[y][x].getInternalSubBorder( side ) )
				hex[y][x].clrDemarkationLine( side );
			break;
		case HexsideType::BORDER:		// bBorder HI
			hex[y][x].toggleInternationalBorder( side );	// toggle bBorder HI
			// map-23: int'l border is mutex w/ internal and sub-internal borders
			if ( hex[y][x].getInternationalBorder( side ) )
			{
				hex[y][x].clrInternalBorder( side );
				hex[y][x].clrInternalSubBorder( side );
			}
			if ( ! hex[y][x].getDemarkationLine( side ) )
				break;
			// if demarkation line is set, and have toggled off both
			//	int'l and internal borders, must clear demarkation:
			if ( ! hex[y][x].getInternationalBorder( side ) && !hex[y][x].getInternalBorder( side ) )
				hex[y][x].clrDemarkationLine( side );		// clear bBorder2 HI
			break;
		case HexsideType::IBORDER:		// bBorder LO
			hex[y][x].toggleInternalBorder( side );			// toggle bBorder LO
			// map-23: int'l border is mutex w/ internal and sub-internal borders
			if ( hex[y][x].getInternalBorder( side ) )
			{
				hex[y][x].clrInternationalBorder( side );
				// note: acceptable to leave demarkation line setting intact here
			}
			if ( ! hex[y][x].getDemarkationLine( side ) )
				break;
			// if demarkation line is set, and have toggled off both
			//	int'l and internal borders, must clear demarkation:
			if ( ! hex[y][x].getInternationalBorder( side ) && !hex[y][x].getInternalBorder( side ) )
				hex[y][x].clrDemarkationLine( side );		// clear bBorder2 HI
			break;
		case HexsideType::SUBIBORDER:	// bBorder2 LO
			hex[y][x].toggleInternalSubBorder( side );		// toggle bBorder2 LO
			// map-23: int'l border is mutex w/ internal and sub-internal borders
			if ( hex[y][x].getInternalSubBorder( side ) )
			{
				hex[y][x].clrInternationalBorder( side );
				hex[y][x].clrDemarkationLine( side ); // no demarkation on internal sub-borders?
			}
			break;
#endif
		case HexsideType::MTN_SIDE2: // this is a *really* rare mtn interior hexside
			if ( hex_[y][x].getMountainInterior( side ) )
			{
				hex_[y][x].clrMountainInterior( side );
				hex_[y][x].updateInteriorHexsidesFlag( );
			}
			else
			{
				// only allow an interior mtn hexside
				//	adjacent to a minor river or wadi
				int x2, y2;
				int adj = ht::getAdjHexCoords( side, x, y, &x2, &y2, world_x_size_, world_y_size_ );
				if ( ! hex_[y][x].getMinorRiver( side )
					&& ! hex_[y][x].getWadi( side )
					&& adj
					&& ! hex_[y2][x2].getMinorRiver( ht::getAdjHexPart( side ) )
					&& ! hex_[y2][x2].getWadi( ht::getAdjHexPart( side ) ) )
					break;

				// no other interior hexside allowed
				hex_[y][x].clrFortifiedSide( side );
				hex_[y][x].setMountainInterior( side );
				hex_[y][x].setInteriorHexsidesFlag( );
			}
			break;
		case HexsideType::ESCARPMENT:
			if ( hex_[y][x].getEscarpment( side ) && ! hex_[y][x].getImpassableEscarpment( side ) )
			{
				hex_[y][x].clrEscarpment( side );
				hex_[y][x].updateInteriorHexsidesFlag( );
			}
			else
			{
				hex_[y][x].clrMountainInterior( side );
				hex_[y][x].setEscarpment( side );
				hex_[y][x].clrImpassableEscarpment( side );
				hex_[y][x].setInteriorHexsidesFlag( );
			}
			break;
		case HexsideType::IMP_ESCARPMENT:
			if ( hex_[y][x].getImpassableEscarpment( side ) && ! hex_[y][x].getEscarpment( side ) )
			{
				hex_[y][x].clrImpassableEscarpment( side );
				hex_[y][x].updateInteriorHexsidesFlag( );
			}
			else
			{
				hex_[y][x].clrMountainInterior( side );
				hex_[y][x].clrEscarpment( side );
				hex_[y][x].setImpassableEscarpment( side );
				hex_[y][x].setInteriorHexsidesFlag( );
			}
			break;
		case HexsideType::FORTIFIED:
			if ( hex_[y][x].getFortifiedSide( side ) )
			{
				hex_[y][x].clrFortifiedSide( side );
				hex_[y][x].updateInteriorHexsidesFlag( );
			}
			else
			{
				hex_[y][x].clrMountainInterior( side );
				hex_[y][x].setFortifiedSide( side );
				hex_[y][x].setInteriorHexsidesFlag( );
			}
			break;
		case HexsideType::GREAT_WALL:
			if ( hex_[y][x].getGreatWall( side ) )
			{
				hex_[y][x].clrGreatWall( side );
				hex_[y][x].updateInteriorHexsidesFlag( );
			}
			else
			{
				hex_[y][x].clrMountainInterior( side );
				hex_[y][x].setGreatWall( side );
				hex_[y][x].setInteriorHexsidesFlag( );
			}
			break;

		case HexsideType::NOTHING:
			// clear private hexsides:
			hex_[y][x].clrEscarpment( side );
			hex_[y][x].clrImpassableEscarpment( side );

			// map-14:
			hex_[y][x].clrAllOrnaments( side );
#if 0
			hex_[y][x].clrAllBorders( side );
#endif
			hex_[y][x].clrLimitedStacking( side );
			hex_[y][x].clrWaterwayRoute( side );
			hex_[y][x].clrInlandShippingChannel( side );
			hex_[y][x].clrInlandRouteBar( side );
			hex_[y][x].clrLakeSeaFerry( side );

			// blow away 12 water sides, 8 rare land sides, and clear interior mtn (either side)
			clear_hex_side( x, y, side ); // this also updates the hex's hexside flags
			break;
	}

	if ( hex_[y][x].getLateSeaIce( )			// some ice was late
		&& ! hexsides_ice( hex_[y][x] )		// now no ice hexsides
		&& ! hex_ices( hex_[y][x].terrain_ ) )	// and hex is not ice
	{
		hex_[y][x].clrLateSeaIce( );			// clear late sea ice flag for consistency
	}
}
#endif

// used by either HT-m or HT-p
bool MapPane::hexsides_ice( Hex h )
{
	return ( h.hasIcingSeaSides( ) > 0 );
}

// used by either HT-m or HT-p
int MapPane::get_lake_sea_ferry_count( int x, int y )
{
	int count = 0;

	// in-hex dirs, SOUTHEAST, SOUTHWEST, WEST
	for ( int dir = Hex::SOUTHEAST; dir <= Hex::WEST; dir <<= 1 )
		count += hex_[y][x].getLakeSeaFerry( dir ) ? 1 : 0;

	// adjacent-hex dirs, NORTHWEST, NORTHEAST, EAST
	int x2, y2;
	for ( int dir = Hex::NORTHWEST; dir <= Hex::EAST; dir <<= 1 )
		if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) )
			count += hex_[y2][x2].getLakeSeaFerry( ht::getAdjHexPart( dir ) ) ? 1 : 0;

	return count;
}

#if defined HEXTOOLSPLAY

bool MapPane::IsRRInHex( int x, int y )
{
	int d;
	ENUM_DIRS( d )
		if ( IsRailRoad( x, y, d ) )
			return true;

	return false;
#if 0
	if (	IsRailRoad( x, y, Hex::WEST ) ||
			IsRailRoad( x, y, Hex::EAST ) ||
			IsRailRoad( x, y, Hex::NORTHWEST ) ||
			IsRailRoad( x, y, Hex::NORTHEAST ) ||
			IsRailRoad( x, y, Hex::SOUTHEAST ) ||
			IsRailRoad( x, y, Hex::SOUTHWEST ) )
			IsLVRailRoad( x, y, Hex::WEST ) ||
			IsLVRailRoad( x, y, Hex::EAST ) ||
			IsLVRailRoad( x, y, Hex::NORTHWEST ) ||
			IsLVRailRoad( x, y, Hex::NORTHEAST ) ||
			IsLVRailRoad( x, y, Hex::SOUTHEAST ) ||
			IsLVRailRoad( x, y, Hex::SOUTHWEST ) )

		return true;

	return false;
#endif
}

bool MapPane::IsRoadInHex( int x, int y )
{
	int d;
	ENUM_DIRS( d )
		if ( IsRoad( x, y, d ) )
			return true;

	return false;

#if 0
	if (	IsRoad( x, y, WEST ) ||
			IsRoad( x, y, EAST ) ||
			IsRoad( x, y, NORTHWEST ) ||
			IsRoad( x, y, NORTHEAST ) ||
			IsRoad( x, y, SOUTHEAST ) ||
			IsRoad( x, y, SOUTHWEST ) )
		return true;

	return false;
#endif
}

bool MapPane::IsPort( int x, int y )
{
	if ( hex_[y][x].getPortSize( ) > 0 )
		return true;

	City* city = hex_[y][x].city_;
	if ( city )
	{
		// TODO: appears to be a defect -- perhaps check for MAJOR_NAVAL_BASE
		//	and MINOR_NAVAL_BASE though
#if 0
		if (	city->iType == City::Type::MAJOR_NAVAL_BASE ||
				city->iType == City::Type::MINOR_NAVAL_BASE )
			return true;
#endif
		if (	city->type_ == City::Type::MINOR ||
				city->type_ == City::Type::MAJOR ||
				city->type_ == City::Type::PARTIALHEX ||
				city->type_ == City::Type::FULLHEX )
			return true;
	}

	return false;
}

// Add rule that Allied ZOCs do not extend across border
bool MapPane::IsIntlBorder( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	// TODO: refactor for y,x and y2,x2 have different country codes
#if 0
	int dir2 = getAdjHexPart( dir );
	if ( hex[y][x].getInternationalBorder( dir ) || hex[y2][x2].getInternationalBorder( dir2 ) )
		return true;
#endif
	return false;
}

bool MapPane::IsMajorRiver( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getMajorRiver( dir ) || hex_[y2][x2].getMajorRiver( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsMinorRiver( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getMinorRiver( dir ) || hex_[y2][x2].getMinorRiver( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsMinorCanal( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getMinorCanal( dir ) || hex_[y2][x2].getMinorCanal( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsCanal( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getCanal( dir ) || hex_[y2][x2].getCanal( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsMountain( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getMountain( dir ) || hex_[y2][x2].getMountain( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsHighMtn( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getHighMtn( dir ) || hex_[y2][x2].getHighMtn( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsHighMtnPass( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getHighMtnPass( dir ) || hex_[y2][x2].getHighMtnPass( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsKarst( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getKarst( dir ) || hex_[y2][x2].getKarst( dir2 ) )
		return true;

	return false;
}

// returns true when hexside is sea, lake, or restricted waters
bool MapPane::IsWater( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].hasWaterSide( dir ) || hex_[y2][x2].hasWaterSide( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsSeaSide( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getSeaSide( dir ) || hex_[y2][x2].getSeaSide( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsLakeSide( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getLakeSide( dir ) || hex_[y2][x2].getLakeSide( dir2 ) )
		return true;

	return false;
}

// TODO: refactor to -map style
bool MapPane::IsRWaterSide( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getRestrictedWaterSide( dir ) || hex_[y2][x2].getRestrictedWaterSide( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsNarrowStrait( int x, int y, int dir )
{
#if 0
	 int x2, y2, dir2;
	 if (MapPane::getAdjHex(dir, x, y, &x2, &y2, iXWorldSize, iYWorldSize) == 0)
		 return false;
	 dir2 = getAdjHexPart(dir);
	 if (Hex[y][x].GetNarrowStrait(dir) || Hex[y2][x2].GetNarrowStrait(dir2))
		 return true;
	 return false;
#endif
	if (	IsWater( x, y, dir ) &&
			( IsRoad( x, y, dir ) || IsLVRailRoad( x, y, dir ) || IsRailRoad( x, y, dir ) )
		)
		return true;

	return false;
}

bool MapPane::IsWadi( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getWadi( dir ) || hex_[y2][x2].getWadi( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsSaltDesert( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getSaltDesert( dir ) || hex_[y2][x2].getSaltDesert( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsGlacier( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getGlacier( dir ) || hex_[y2][x2].getGlacier( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsRoad( int x, int y, int dir )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getRoad( dir ) || hex_[y2][x2].getRoad( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsTrail( int x, int y, int dir )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getTrack( dir ) || hex_[y2][x2].getTrack( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsRailRoad( int x, int y, int dir )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getRailRoad( dir ) || hex_[y2][x2].getRailRoad( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsLVRailRoad( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;
#if 0
	// TODO: refactor
	int dir2 = getAdjHexPart( dir );

	// TODO: refactor this to use HexContents:: railroad capacity/state
	//	as of HT v2.3, no more storage of rail volume in Hex::
	if ( hex[y][x].getLVRailRoad( dir ) || hex[y2][x2].getLVRailRoad( dir2 ) )
		return true;
#endif
	return false;
}

bool MapPane::IsEscarpment( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	int dir2 = ht::getAdjHexPart( dir );

	if ( hex_[y2][x2].getFortifiedSide( dir2 ) )  // fort=esc+imp.esc!!!
		return false;

	// only escarpment up counts:
	if ( /* hex_[y][x].GetEscarpment(dir) || */ hex_[y2][x2].getEscarpment( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsImpEscarpment( int x, int y, int dir )
{
	int x2, y2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	// impassable: both count
	// fort=esc+imp.esc, thus check!!!
	if ( ! hex_[y][x].getFortifiedSide( dir ) && hex_[y][x].getImpassableEscarpment( dir ) )
		return true;

	int dir2 = ht::getAdjHexPart( dir );
	if ( ! hex_[y2][x2].getFortifiedSide( dir2 ) && hex_[y2][x2].getImpassableEscarpment( dir2 ) )
		return true;

	return false;
}

bool MapPane::IsHexsideFortified( int x, int y, int dir )
{
	int x2, y2, dir2;
	if ( ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ ) == 0 )
		return false;

	dir2 = ht::getAdjHexPart( dir );
	if ( hex_[y][x].getFortifiedSide( dir ) || hex_[y2][x2].getFortifiedSide( dir2 ) )
		return true;
	return false;
}

bool MapPane::IsNonClearHexSide( int x, int y, int dir )
{
	if (	IsMajorRiver( x, y, dir ) ||
			IsMinorRiver( x, y, dir ) ||
			IsMountain( x, y, dir ) ||
			IsHighMtn( x, y, dir ) ||
			IsHighMtnPass( x, y, dir ) ||
			IsKarst( x, y, dir ) ||
			IsWater( x, y, dir ) ||
			IsNarrowStrait( x, y, dir ) ||
			IsWadi( x, y, dir ) ||
			IsSaltDesert( x, y, dir ) ||
			IsGlacier( x, y, dir ) ||
			IsEscarpment( x, y, dir ) ||
			IsImpEscarpment( x, y, dir ) )
		return true;

	return false;
}

bool MapPane::IsProhibited( int x, int y, int dir )
{
	// TODO: except high-mtn units
	if (	IsHighMtn( x, y, dir ) ||
			( ( IsKarst( x, y, dir ) || IsHighMtnPass( x, y, dir ) ) && GetWeather( x, y ) != HexType::CLEAR ) ||
			IsSaltDesert( x, y, dir ) ||
			IsGlacier( x, y, dir ) ||
			IsWater( x, y, dir ) ||  // TODO: except if frozen
			IsImpEscarpment( x, y, dir ) )
		return true;

	int new_x, new_y;
	if ( ht::getAdjHexCoords( dir, x, y, &new_x, &new_y, world_x_size_, world_y_size_ ) )
	{
		if (	hex_[new_y][new_x].terrain_ == HexType::GLACIER ||
				// TODO: mot/art: tundra is prohibited!?
				hex_[new_y][new_x].terrain_ == HexType::SALTMARSH ||
				hex_[new_y][new_x].terrain_ == HexType::SALTDESERT ||
				hex_[new_y][new_x].terrain_ == HexType::SEA ||
				hex_[new_y][new_x].terrain_ == HexType::RESTRICTEDWATERS ||
				hex_[new_y][new_x].terrain_ == HexType::ICINGSEA )
			return true;
	}

	return false;
}

int MapPane::GetWeather( int x, int y )
{
	return _weather_condition[hex_[y][x].getWeatherZone( )];
}

// any rivers for ships to sail?
bool MapPane::IsRivers( int x, int y )
{
	if ( IsMajorRiver( x, y, Hex::WEST ) || IsMajorRiver( x, y, Hex::SOUTHWEST ) || IsMajorRiver( x, y, Hex::NORTHWEST )
		|| IsMajorRiver( x, y, Hex::EAST ) || IsMajorRiver( x, y, Hex::SOUTHEAST ) || IsMajorRiver( x, y, Hex::NORTHEAST )
		|| IsMinorRiver( x, y, Hex::WEST ) || IsMinorRiver( x, y, Hex::SOUTHWEST ) || IsMinorRiver( x, y, Hex::NORTHWEST )
		|| IsMinorRiver( x, y, Hex::EAST ) || IsMinorRiver( x, y, Hex::SOUTHEAST ) || IsMinorRiver( x, y, Hex::NORTHEAST ) )
		return true;

	if ( IsCanal( x, y, Hex::WEST ) || IsCanal( x, y, Hex::SOUTHWEST ) || IsCanal( x, y, Hex::NORTHWEST ) || IsCanal( x, y, Hex::EAST )
		|| IsCanal( x, y, Hex::SOUTHEAST ) || IsCanal( x, y, Hex::NORTHEAST ) || IsMinorCanal( x, y, Hex::WEST )
		|| IsMinorCanal( x, y, Hex::SOUTHWEST ) || IsMinorCanal( x, y, Hex::NORTHWEST ) || IsMinorCanal( x, y, Hex::EAST )
		|| IsMinorCanal( x, y, Hex::SOUTHEAST ) || IsMinorCanal( x, y, Hex::NORTHEAST ) )
		return true;

	return false;
}

// simply check if there are any water hexsides in the hex
bool MapPane::IsCoastalHex( int x, int y )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	if ( hex_[y][x].terrain_ == HexType::SEA || hex_[y][x].terrain_ == HexType::RESTRICTEDWATERS || hex_[y][x].terrain_ == HexType::ICINGSEA )
		return false;

	if ( IsWater( x, y, Hex::WEST ) )
		return true;
	if ( IsWater( x, y, Hex::NORTHWEST ) )
		return true;
	if ( IsWater( x, y, Hex::SOUTHWEST ) )
		return true;
	if ( IsWater( x, y, Hex::EAST ) )
		return true;
	if ( IsWater( x, y, Hex::NORTHEAST ) )
		return true;
	if ( IsWater( x, y, Hex::SOUTHEAST ) )
		return true;

	return false;
}

bool MapPane::IsCoastalCliffs( int x, int y )
{
	// TODO: any escarpments in a coastal hex
#if 0
	if ( ! Hex[y][x].hasEscarpments( ) )
		return false;
	for ( int dir = 1; dir <= 32; dir *= 2 )
		if ( IsWater( x, y, dir ) )
			return true; // is this coastal only?
#endif
	for ( int dir = 1; dir <= 32; dir *= 2 )
		if ( IsWater( x, y, dir ) && hex_[y][x].getEscarpment( dir ) )
			return true;

	return false;
}

int MapPane::GetPortRawCapacity( int x, int y )
{
	// TODO: consider a negative return to discriminate from an in-bounds hex with no port cap
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return 0;

	int cap = 0;
	switch ( hex_[y][x].getPortSize( ) )
	{
		case Port::Type::MINOR:
			cap = 3;
			break;
		case Port::Type::STANDARD:
			cap = 6;
			break;
		case Port::Type::MAJOR:
			cap = 12;
			break;
		case Port::Type::GREAT:
			cap = 24;
			break;
	}

	if ( sit_[y][x].isMulberry( ) )
		cap += 6;

	return cap;
}

int MapPane::GetPortCapacity( int x, int y )
{
	// TODO: consider a negative return to discriminate from an in-bounds hex with no port cap
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return 0;

	if ( sit_[y][x].isPortDestroyed( ) )
		return 0;

	return GetPortRawCapacity( x, y ) - sit_[y][x].getPortHits( );
}

// bridges:
// is bridge at all, destroyed or not
bool MapPane::IsBridge( int x, int y, int dir )
{
	// TODO: account for standalone bridges

	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	if (	IsMinorRiver( x, y, dir )
			|| IsMajorRiver( x, y, dir )
			|| IsMinorCanal( x, y, dir )
			|| IsCanal( x, y, dir )
			|| IsNarrowStrait( x, y, dir ) )
	{
		// narrow strait == water hexside + road!
		if (	( IsRoad( x, y, dir ) && ! IsNarrowStrait( x, y, dir ) )
				|| IsRailRoad( x, y, dir )
				|| IsLVRailRoad( x, y, dir ) )
			return true;
	}
	return false;
}

bool MapPane::IsBridgeBreak( int x, int y, int dir )
{
	// TODO: account for standalone bridges

	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return false;

	if ( dir & ( Hex::SOUTHEAST | Hex::SOUTHWEST | Hex::WEST ) )  // requested dir is stored to that sit[][]
		return sit_[y][x].getBridgeBreak( dir );
	// else data is stored to adjacent sit[][]

	int x2, y2;
	ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ );

	int dir2 = ht::getAdjHexPart( dir );

	return sit_[y2][x2].getBridgeBreak( dir2 ) > 0 ? true : false;
}

void MapPane::SetBridgeBreak( int x, int y, int dir, bool val )
{
	if ( coordinatesOutOfBounds( x, y, world_x_size_, world_y_size_ ) )
		return;

	if ( dir & ( Hex::SOUTHEAST | Hex::SOUTHWEST | Hex::WEST ) )  // store requested dir to that Sit[][]
		sit_[y][x].setBridgeBreak( dir, val );
	else
	{	// store data to adjacent Sit[][]
		int x2, y2;
		ht::getAdjHexCoords( dir, x, y, &x2, &y2, world_x_size_, world_y_size_ );

		int dir2 = ht::getAdjHexPart( dir );

		sit_[y2][x2].setBridgeBreak( dir2, val );
	}
}

bool MapPane::hexInExoticZone( int x, int y )
{
	int wz = hex_[y][x].getWeatherZone( );
	return ( wz == 0 || wz > 4 ); // weather zones B(1), C(2), D(3) or E(4) are *not* exotic
}

#endif
