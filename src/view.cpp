#if defined HEXTOOLSPLAY

#include <iostream>
using std::endl;

#include <wx/wx.h>

#include <string.h>
#include "hextools.h"
#include "counter.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "view.h"
#include "dview.h"
#include "dviewcolor.h"

using ht::line2spc;
using ht::spc2line;
using ht::isOkStr;
using ht::inttoa;

extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)
extern wxWindow* _parent_dialog_window;
extern ViewColorDlgXfer _view_dialog_data;

View::View( char* name, int red, int green, int blue )
{
	strcpy( name_, name );
	red_ = red;
	green_ = green;
	blue_ = blue;
	active_ = false;
}

View::View( )
{
	clearView( -1 );
}

View::~View( )
{
}

int View::hexInView( int x, int y )
{
	for ( int pt = 0; pt < static_cast<int>( ax_.GetCount( ) ); ++pt )
		if ( ax_[pt] == x && ay_[pt] == y ) 
			return pt;

	return -1;
}

bool View::viewIsActive( )
{
	return active_;
}

int View::hexCount( )
{
	return ax_.GetCount( );
}

int View::getViewX( int pt )
{
	if ( pt < static_cast<int>( ax_.GetCount( ) ) ) 
		return ax_[pt];

	return 0;
}

int View::getViewY( int pt )
{
	if ( pt < static_cast<int>( ay_.GetCount( ) ) ) 
		return ay_[pt];

	return 0;
}

int View::edit( int nview )
{
	static DView* dlg;
	if ( ! dlg )
		dlg = new DView( _parent_dialog_window );
	strcpy( _view_dialog_data.editName, name_ );
	line2spc( _view_dialog_data.editName );
	strcpy( _view_dialog_data.editRed, inttoa( red_ ) );
	strcpy( _view_dialog_data.editGreen, inttoa( green_ ) );
	strcpy( _view_dialog_data.editBlue, inttoa( blue_ ) );

	// go!
	if ( dlg->ShowModal( ) != wxID_OK )
	{
		_unit_editor_active = false;  // bug?
		return FALSE;
	}
	_unit_editor_active = false;     // bug? why not just check this before the modal check?

	dlg->CloseDialog( );  // hack: transfer results from controls

	// get results
	red_ = atoi( _view_dialog_data.editRed );
	green_ = atoi( _view_dialog_data.editGreen );
	blue_ = atoi( _view_dialog_data.editBlue );
	strcpy( name_, _view_dialog_data.editName );
	if ( strlen( name_ ) == 0 )
		clearView( nview );
	return 1;  // TODO: determine if something more informative
}

void View::clearView( int nview )
{
	if ( nview >= 0 )
		sprintf( name_, "view %d", nview );
	else
		strcpy( name_, " " );
	red_ = 255;
	green_ = 0;
	blue_ = 0;
	active_ = false;
	ax_.Clear( );
	ay_.Clear( );
}

std::istream& operator>>( std::istream& is, View& view )
{
	char name[HexNote::MAX_CAPTION_LEN * 5];
	int red, green, blue, numpts;
	is >> name >> red >> green >> blue >> numpts;

	line2spc( name );
	strcpy( view.name_, name );
	view.red_ = red;
	view.green_ = green;
	view.blue_ = blue;

	int x, y;
	for ( int pt = 0; pt < numpts; ++pt )
	{
		is >> x >> y;
		view.ax_.Add( x );
		view.ay_.Add( y );
	}

	char vtxt[HexNote::MAX_TEXT_LEN * 5];
	is >> vtxt;
	line2spc( vtxt );
	strncpy( view.txt_, vtxt, HexNote::MAX_TEXT_LEN - 1 );

	return is;
}

std::ostream& operator<<( std::ostream& os, View& view )
{
	char name[HexNote::MAX_CAPTION_LEN * 5];
	strcpy(name, view.name_);
	spc2line(name);
	os << "<View ";

	if ( isOkStr( name ) )
		os << name << ' ';
	else
		os << "-" << ' ';

	os << view.red_ << ' ';
	os << view.green_ << ' ';
	os << view.blue_ << ' ';
	os << view.ax_.GetCount() << ' ';

	for ( int pt = 0; pt < static_cast<int>( view.ax_.GetCount() ); ++pt )
		os << view.ax_[pt] << ' ' << view.ay_[pt] << ' ';

	char vtxt[HexNote::MAX_TEXT_LEN * 5];
	strncpy( vtxt, view.txt_, HexNote::MAX_TEXT_LEN - 1 );
	spc2line( vtxt );
	
	if ( isOkStr( vtxt ) )
		os << vtxt << ' ';
	else
		os << "-" << ' ';

	os << "/>";

	return os;
}

#endif
