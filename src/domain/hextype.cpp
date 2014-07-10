#include <wx/wx.h>

#include "hextools.h"
#include "hextype.h"

HexType::HexType( )
{
	int wxClasses = sizeof( mp_ ) / sizeof( mp_[0] );
	assert( wxClasses == ht::NUM_WEATHER_CLASSES );

	int mvmtColumns = sizeof( mp_[0] ) / sizeof( mp_[0][0] );
	assert( mvmtColumns == ht::NUM_MVMT_EFFECTS_COLUMNS );

	// initialize MP table for each hex type on the TEC
	for ( int i = 0; i < wxClasses; ++i )
	{
		drm_[i] = 0;
		for ( int j = 0; j < mvmtColumns; ++j )
			mp_[i][j] = 0;
	}

	defense_flags_ = 0;
	name_ = 0;
	hbmBitmap = nullptr;
#if defined HEXTOOLSPLAY
	hbmMudBitmap = nullptr;
	hbmSnowBitmap = nullptr;
	hbmFrostBitmap = nullptr;
#endif
}

HexType::~HexType( )
{
}
