#if defined HEXTOOLSPLAY
#ifndef VIEWS_H
#define VIEWS_H

class View;

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY( View, ArrayOfViews );

class Views
{
public:
	static const unsigned short MAX_VIEWS = 20;
private:
	friend std::istream& operator>>( std::istream&, Views& );
	friend std::ostream& operator<<( std::ostream&, Views& );
public:
	Views( );
	~Views( );
	ArrayOfViews views_;
	int getViewCount( );
	int getActiveViewCount( );
	bool viewIsActive( int nview );
	void setViewActive( int nview, bool Active );
	char* getViewName( int v );
	char* getViewText( int v );
	void setViewName( int nview, char* s );
	void setViewText( int nview, char* s );
	void setViewColor( int nview, char* sRed, char* sGreen, char* sBlue );
	bool hexInView( int nview, int x, int y );
	int firstActiveView( );
	int hexCount( int nview );
	int getViewX( int nview, int pt );
	int getViewY( int nview, int pt );
	int getRed( int nview );
	int getGreen( int nview );
	int getBlue( int nview );
	bool deletePointFromView( int nview, int x, int y );
	void addPointToView( int nview, int x, int y );
	void editView( int nview );
	bool createNewView( int nview );
	void clearAllViews( );
};

#endif
#endif
