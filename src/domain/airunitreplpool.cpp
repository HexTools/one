#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;
using std::endl;

#include <wx/wx.h>

#include "hextools.h"
#include "application.h"
#include "counter.h"
#include "airunitreplpool.h"
#include "airunit.h"

extern int _year;
extern int _turn;
extern int _month;

#if 0
extern AirUnitReplPool _air_repl_pool[];
#endif

AirUnitReplPool::AirUnitReplPool( )
{
	for ( int i = 0; i < REPLPOOLSIZE; ++i )
		pool_[i].cntr_ = nullptr;

	count_ = 0;
}

AirUnitReplPool::~AirUnitReplPool( )
{
	clear( );
}

void AirUnitReplPool::clear( )
{
	for ( int i = 0; i < count_; ++i )
		if ( pool_[i].cntr_ )
			delete pool_[i].cntr_;

	count_ = 0;
}

void AirUnitReplPool::add( AirUnit* cntr )
{
	if ( count_ == REPLPOOLSIZE )
		return;

	pool_[count_].cntr_ = cntr;
	pool_[count_].year_ = static_cast<unsigned short>( _year );
	pool_[count_].month_ = static_cast<unsigned char>( _month );
	pool_[count_].turn_ = static_cast<unsigned char>( _turn );

	++count_;
}

AirUnit* AirUnitReplPool::remove( AirUnit* cntr )
{
	for ( int i = 0; i < count_; ++i )
		if ( pool_[i].cntr_ == cntr )
			return remove( i );

	return nullptr;
}

AirUnit* AirUnitReplPool::remove( int idx )
{
	if ( idx < 0 || idx >= count_ )
		return nullptr;

	AirUnit* c = pool_[idx].cntr_;

	for ( int i = idx; i < count_; ++i )
		pool_[i] = pool_[i + 1];
	pool_[count_].cntr_ = nullptr;

	--count_;

	return c;
}

AirUnit* AirUnitReplPool::get( int idx )
{

	if ( idx < 0 || idx >= count_ )
		return nullptr;

	return pool_[idx].cntr_;
}

int AirUnitReplPool::getYear( int idx )
{

	if ( idx < 0 || idx >= count_ )
		return 0;	// bogus year

	return pool_[idx].year_;
}

int AirUnitReplPool::getMonth( int idx )
{

	if ( idx < 0 || idx >= count_ )
		return 0;	// bogus month

	return pool_[idx].month_;
}

int AirUnitReplPool::getTurn( int idx )
{

	if ( idx < 0 || idx >= count_ )
		return 0;	// bogus turn

	return pool_[idx].turn_;
}

// loading
std::istream &operator>>( std::istream &is, AirUnitReplPool& arpool )
{
	arpool.clear( );

	int c;
	is >> c;

	for ( int i = 0; i < c; ++i )
	{
		AirUnit* au = new AirUnit;
		if ( au == nullptr )
		{
			wxMessageBox( wxT("Memory allocation failed, can't load replacement pool"), Application::NAME, wxOK | wxICON_HAND );
			break;
		}
		is >> *au;
		arpool.add( au );
	}
	return is;
}

// saving
std::ostream &operator<<( std::ostream &os, AirUnitReplPool& arpool )
{
	int c = arpool.count( );

	os << c << endl;
	for ( int i = 0; i < c; ++i )
		os << *( static_cast<AirUnit*>( arpool.get( i ) ) ) << endl;

	return os;

#if 0
	Counter* c;
	os << (int)arpool.count( ) << endl;
	for ( int i = 0; i < arpool.count( ); i++ )
	{
		c = arpool.get( i );
		os << *(AirUnit*) ( c );
		os << endl;
	}
#endif

}

#endif
