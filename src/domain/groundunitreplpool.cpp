#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;
using std::endl;

#include <wx/wx.h>

#include "hextools.h"
#include "application.h"
#include "groundunitreplpool.h"
#include "counter.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunit.h"

extern int _year;
extern int _turn;
extern int _month;

// TODO:
// Strange bug: Axis repl pool gets emptied every now and then. This happened
// because of text notes overwriting repl pool memory. This should be fixed
// in 1.1, but heres some buffer zone just in case (buggy notes may freely
// overwrite this memory).
#define BUG_CATCH_SIZE 10000
static char bug_catcher[BUG_CATCH_SIZE];

#if 0
extern GroundUnitReplPool ReplPool[];  // AXIS and ALLIED
#endif

GroundUnitReplPool::GroundUnitReplPool( )
{
	for ( int i = 0; i < REPLPOOLSIZE; ++i )
		pool_[i].cntr_ = nullptr;

	count_ = 0;
}

GroundUnitReplPool::~GroundUnitReplPool( )
{
	clear( );
}

void GroundUnitReplPool::clear( )
{
	for ( int i = 0; i < count_; ++i )
		if ( pool_[i].cntr_ )
			delete pool_[i].cntr_;

	count_ = 0;

	for ( int i = 0; i < BUG_CATCH_SIZE; ++i )	// touch this mem so that compiler
		bug_catcher[i] = static_cast<char>( rand( ) );			// won't optimize it away
}

void GroundUnitReplPool::add( GroundUnit* cntr )
{
	if ( count_ >= REPLPOOLSIZE )
		return;

	pool_[count_].cntr_ = cntr;
	pool_[count_].year_ = static_cast<unsigned short>( _year );
	pool_[count_].month_ = static_cast<unsigned char>( _month );
	pool_[count_].turn_ = static_cast<unsigned char>( _turn );

	++count_;
}

GroundUnit* GroundUnitReplPool::remove( GroundUnit* cntr )
{
	for ( int i = 0; i < count_; ++i )
		if ( pool_[i].cntr_ == cntr )
			return remove( i );

	return nullptr;
}

GroundUnit* GroundUnitReplPool::remove( int idx )
{
	if ( idx < 0 || idx >= count_ )
		return nullptr;

	GroundUnit* c = pool_[idx].cntr_;

	for ( int i = idx; i < count_; ++i )
		pool_[i] = pool_[i + 1];
	pool_[count_].cntr_ = nullptr;

	--count_;

	return c;
}

GroundUnit* GroundUnitReplPool::get( int idx )
{
	if ( idx < 0 || idx >= count_ )
		return nullptr;

	return pool_[idx].cntr_;
}

int GroundUnitReplPool::getYear( int idx )
{
	if ( idx < 0 || idx >= count_ )
		return 0;	// bogus year

	return pool_[idx].year_;
}

int GroundUnitReplPool::getMonth( int idx )
{
	if ( idx < 0 || idx >= count_ )
		return 0;	// bogus month

	return pool_[idx].month_;
}

int GroundUnitReplPool::getTurn( int idx )
{
	if ( idx < 0 || idx >= count_ )
		return 0;	// bogus turn

	return pool_[idx].turn_;
}

// loading
std::istream &operator>>( std::istream &is, GroundUnitReplPool& grpool )
{
	grpool.clear( );

	int c;
	is >> c;

	for ( int i = 0; i < c; ++i )
	{
		GroundUnit* gu = new GroundUnit;
		if ( gu == nullptr )
		{
			wxMessageBox( wxT("Memory allocation failed, can't load replacement pool"), Application::NAME, wxOK | wxICON_HAND );
			break;
		}
		is >> *gu;
		grpool.add( gu );
	}
	return is;
}

// saving
std::ostream &operator<<( std::ostream &os, GroundUnitReplPool& r )
{
	int c = r.count( );

	os << c << endl;
	for ( int i = 0; i < c; ++i )
		os << *( static_cast<GroundUnit*>( r.get( i ) ) ) << endl;

	return os;
}

#endif
