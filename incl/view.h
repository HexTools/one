#if defined HEXTOOLSPLAY
#ifndef VIEW_H
#define VIEW_H

#if ! defined INCLUDED_HEXNOTE
#include "hexnote.h"
#define INCLUDED_HEXNOTE
#endif

//#include "counter.defs"

class View
{
	friend std::istream& operator>>( std::istream&, View& );
	friend std::ostream& operator<<( std::ostream&, View& );
protected:
	char txt_[HexNote::MAX_TEXT_LEN];
public:
	View( char* name, int red = 255, int green = 0, int blue = 0 );
	View( );
	~View( );
	char name_[HexNote::MAX_CAPTION_LEN];
	int red_;
	int green_;
	int blue_;
	bool active_;

	wxArrayInt ax_;
	wxArrayInt ay_;

	void setName( char* s )
	{
		strncpy( name_, s, HexNote::MAX_CAPTION_LEN );
	}
	void setText( char* s )
	{
		strncpy( txt_, s, HexNote::MAX_TEXT_LEN );
	}
	char* getNoteText( )
	{
		return txt_;
	}

	int hexInView( int x, int y );
	bool viewIsActive( );
	int hexCount( );
	int getViewX( int pt );
	int getViewY( int pt );
	int edit( int nview );
	void clearView( int nview );
};

#endif
#endif
