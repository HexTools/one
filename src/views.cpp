#if defined HEXTOOLSPLAY

#include <iostream>
using std::endl;

#include <string.h>

#include <wx/wx.h>

#include "hextools.h"
#include "counter.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "view.h"
#include "views.h"
#include "dview.h"
#include "dviewcolor.h"

using ht::line2spc;
using ht::spc2line;
using ht::isOkStr;
using ht::inttoa;

/* TODO: disabled -Wshadow for wxWidgets dynamic array definition, which causes this warning:
./src/views.cpp: In member function 'int ArrayOfViews::Index(const _wxObjArrayArrayOfViews&, bool) const':
./src/views.cpp:43:1: warning: declaration of 'Item' shadows a member of 'this' [-Wshadow]
*/
#pragma GCC diagnostic ignored "-Wshadow"
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( ArrayOfViews );
#pragma GCC diagnostic warning "-Wshadow"

Views::Views( )
{
	for ( int nview = 0; nview < Views::MAX_VIEWS; ++nview )
		createNewView( nview );
}

Views::~Views( )
{
}

int Views::getViewCount( )
{
	return views_.GetCount( );
}

int Views::getActiveViewCount( )
{
	int numviews = 0;
	for ( int nview = 0; nview < static_cast<int>( views_.GetCount( ) ); ++nview )
		if ( viewIsActive( nview ) )
			++numviews;

	return numviews;
}

bool Views::viewIsActive( int nview )
{
	if ( nview < getViewCount( ) )
		return views_[nview].viewIsActive( );

	return false;
}

void Views::setViewActive( int nview, bool Active )
{
	if ( nview < getViewCount( ) )
		views_[nview].active_ = Active;
}

char* Views::getViewName( int v )
{
	if ( v < getViewCount( ) )
		return views_[v].name_;
	return 0;
}

char* Views::getViewText( int v )
{
	if ( v < getViewCount( ) )
		return views_[v].getNoteText( );
	return 0;
}

void Views::setViewName( int nview, char* s )
{
	if ( nview < getViewCount( ) )
		views_[nview].setName( s );
}

void Views::setViewText( int nview, char* s )
{
	if ( nview < getViewCount( ) )
		views_[nview].setText( s );
}

void Views::setViewColor( int nview, char* sRed, char* sGreen, char* sBlue )
{
	if ( nview < getViewCount( ) )
	{
		views_[nview].red_ = atoi( sRed );
		views_[nview].green_ = atoi( sGreen );
		views_[nview].blue_ = atoi( sBlue );
	}
}

bool Views::hexInView( int nview, int x, int y )
{
	if ( nview < getViewCount( ) && views_[nview].hexInView( x, y ) >= 0 )
		return true;
	return false;
}

int Views::firstActiveView( )
{
	for ( int nview = 0; nview < static_cast<int>( views_.GetCount( ) ); ++nview )
		if ( viewIsActive( nview ) )
			return nview;

	return -1;
}

int Views::hexCount( int nview )
{
	if ( nview < getViewCount( ) )
		return views_[nview].hexCount( );

	return 0;
}

int Views::getViewX( int nview, int pt )
{
	if ( nview < getViewCount( ) )
		return views_[nview].getViewX( pt );

	return 0;
}

int Views::getViewY( int nview, int pt )
{
	if ( nview < getViewCount( ) )
		return views_[nview].getViewY( pt );

	return 0;
}

int Views::getRed( int nview )
{
	if ( nview < getViewCount( ) )
		return views_[nview].red_;

	return 0;
}

int Views::getGreen( int nview )
{
	if ( nview < getViewCount( ) )
		return views_[nview].green_;

	return 0;
}

int Views::getBlue( int nview )
{
	if ( nview < getViewCount( ) )
		return views_[nview].blue_;

	return 0;
}

bool Views::deletePointFromView( int nview, int x, int y )
{
	if ( nview < getViewCount( ) )
	{
		int pt = views_[nview].hexInView( x, y );
		if ( pt >= 0 )
		{
			views_[nview].ax_.RemoveAt( pt );
			views_[nview].ay_.RemoveAt( pt );
			return true;
		}
	}
	return false;
}

void Views::addPointToView( int nview, int x, int y )
{
	if ( nview < getViewCount( ) )
	{
		views_[nview].ax_.Add( x );
		views_[nview].ay_.Add( y );
	}
}

void Views::editView( int nview )
{
	if ( nview < getViewCount( ) )
		views_[nview].edit( nview );
}

bool Views::createNewView( int WXUNUSED(nview) )
{
	View* view = new View;
	if ( view == 0 || view == (View*)1 )
	{
		printf( "error: failed to allocate memory for view\n" );
		return false;
	}
	else
	{
		views_.Add( view );
		return true;
	}
}

void Views::clearAllViews( )
{
	for ( int nview = 0; nview < getViewCount( ); ++nview )
		views_[nview].clearView( nview );
}

std::istream& operator>>( std::istream& is, Views& views )
{
	char msg[1000];
	int nview = 0;
	while ( 1 )
	{
		is >> msg;
		if ( strcmp( msg, "<View" ) == 0 )
		{
			if ( nview >= Views::MAX_VIEWS )
				printf( "error: too many views\n" );
			else
			{
				is >> views.views_[nview];
				nview++;
				is >> msg;  // i.e. the /> at the end of the line
			}
		}
		else if ( strcmp( msg, "</Views>" ) == 0 )
			break;
	}
	return is;  // TODO: is this intentional?
}

std::ostream& operator<<( std::ostream& os, Views& views )
{
	for ( int nview = 0; nview < static_cast<int>( views.views_.GetCount( ) ); nview++ )
		if ( views.views_[nview].hexCount( ) > 0 )
			os << views.views_[nview] << endl;

	return os;  // TODO: is this intentional?
}

#endif
