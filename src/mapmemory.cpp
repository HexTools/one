#include <wx/wx.h>

#include "application.h"
#if defined HEXTOOLSPLAY
#include "sideplayer.h"
#endif
#include "hextools.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"
#if defined HEXTOOLSPLAY
#include "hexcontents.h"
#include "counter.h"
#endif

const wxString insufficientMapMemory = wxT( "Can't allocate memory for map data!\n\nMap is now corrupted, exiting." );
const wxString insufficientSitMemory = wxT( "Can't allocate memory for situational data!\n\nScenario is now corrupted, exiting." );
const wxString errorHeader = wxT( "ERROR" );

void MapPane::reserve_map_storage( int y_size, int x_size )
{
	if ( ( hex_ = new Hex*[y_size] ) == nullptr )
	{
		wxMessageBox( insufficientMapMemory, errorHeader );
		exit( -1 );
	}
	for ( int i = 0; i < y_size; ++i )
	{
		if ( ( hex_[i] = new Hex[x_size] ) == nullptr )
		{
			wxMessageBox( insufficientMapMemory, errorHeader );
			exit( -1 );
		}
	}
}

void MapPane::release_map_storage( int y_size, int x_size, bool delete_cities /* = true */ )
{
	for ( int y = 0; y < y_size; ++y )
	{
		if ( delete_cities )
			for ( int x = 0; x < x_size; ++x )
			{
				if ( hex_[y][x].city_ == nullptr )
					continue;

				City* city = hex_[y][x].city_;
				while ( city != nullptr )
				{
					City* next = city->getNext( );
					delete city;
					city = next;
				}

				hex_[y][x].city_ = nullptr;
			}
		delete[] hex_[y];
		hex_[y] = nullptr;
	}
	delete[] hex_;
	hex_ = nullptr;
}

#if defined HEXTOOLSPLAY

void MapPane::reserve_sit_storage( int y_size, int x_size )
{
	if ( ( sit_ = new HexContents*[y_size] ) == nullptr )
	{
		wxMessageBox( insufficientSitMemory, errorHeader );
		exit( -1 );
	}

	for ( int i = 0; i < y_size; ++i )
	{
		if ( ( sit_[i] = new HexContents[x_size] ) == nullptr )
		{
			wxMessageBox( insufficientSitMemory, errorHeader );
			exit( -1 );
		}
	}
}

void MapPane::release_sit_storage( int y_size, int x_size )
{
	for ( int y = 0; y < y_size; ++y )
	{
		for ( int x = 0; x < x_size; ++x )
		{
			if ( sit_[y][x].unit_list_ == nullptr )
				continue;

			Counter* counter = sit_[y][x].unit_list_;
			while ( counter != nullptr )
			{
				Counter* next = counter->getNext( );
				delete counter;
				counter = next;
			}

			sit_[y][x].unit_list_ = nullptr;
		}
		delete[] sit_[y];
		sit_[y] = nullptr;
	}
	delete[] sit_;
	sit_ = nullptr;
}

#endif
