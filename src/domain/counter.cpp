#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "hextools.h"
#include "application.h"
#include "rules.h"
#include "counter.h"

Counter::Counter( )
{
	next_ = nullptr;
	replay_string_ = nullptr;
	flags_ = 0;
	counter_type_ = Counter::Type::NONE;
	homex_ = 0;
	homey_ = 0;
}

Counter::~Counter( )
{
	if ( replay_string_ )
		delete replay_string_;

	replay_string_ = 0;
}

///////////////////////////////////////////////////////////////////////
//
// replays
//

/* path is recorded backwards starting from unit's current position.
 value recorded is opposite to movement direction. this makes it easy
 to travel path backwards as long as possible.

 eg:
 - unit moves: e,<rail-hit>,e,sw,se
 - str=nw,ne,w,<rail-hit>,w

 */

void Counter::recordReplay( char ch, int /* cnt = 1 */ )
{
	// alloc memory if necessary
	if ( ! replay_string_ )
	{
		replay_string_ = (char*)malloc( ht::Replay::MAX_LEN + 1 );  // +1: space for zero too
		if ( ! replay_string_ )
		{
			wxMessageBox( wxT("Failed to allocate memory for the replay data,\nreplay NOT saved"), Application::NAME, wxOK | wxICON_HAND );
			return;
		}
		memset( replay_string_, ht::Replay::END, ht::Replay::MAX_LEN );
		replay_string_[ht::Replay::MAX_LEN] = '\0';  // make sure there is always zero
	}

	for ( int i = ht::Replay::MAX_LEN - 1; i > 0; --i )
		replay_string_[i] = replay_string_[i - 1];

	replay_string_[0] = ch;
}

int Counter::getReplay( int idx )
{
	if ( ! replay_string_ )
		return ht::Replay::END;

	return replay_string_[idx];
}

// shorten replay (expl. move undo)
void Counter::clearExplReplay( )
{
	if ( replay_string_ )
		while ( replay_string_[0] != ht::Replay::END && replay_string_[0] != ht::Replay::Movement::END )
		{
			for ( int i = 0; i < ht::Replay::MAX_LEN; ++i )
				replay_string_[i] = replay_string_[i + 1];

			replay_string_[ht::Replay::MAX_LEN - 1] = ht::Replay::END;
		}
}

void Counter::clearReplay( void )
{
	if ( replay_string_ )
	{
		memset( replay_string_, ht::Replay::END, ht::Replay::MAX_LEN );  // reduce heap fragmentation
#if 0
		free( replay_string_ );
		replay_string_ = 0;
#endif
	}
}

void Counter::setReplay( char* s )
{
	// alloc memory if necessary
	if ( ! replay_string_ )
	{
		replay_string_ = (char*)malloc( ht::Replay::MAX_LEN + 1 );  // +1: space for zero too
		if ( ! replay_string_ )
		{
			wxMessageBox( wxT("Failed to allocate memory for the replay data,\nreplay NOT loaded"), Application::NAME, wxOK | wxICON_HAND );
			return;
		}
		replay_string_[ht::Replay::MAX_LEN] = '\0';  // make sure there is always zero
		memset( replay_string_, ht::Replay::END, ht::Replay::MAX_LEN );
	}
	strncpy( replay_string_, s, ht::Replay::MAX_LEN );
}

#endif
