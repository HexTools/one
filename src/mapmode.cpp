#include <wx/wx.h>

#include "hextools.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "frame.h"
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#include "application.h"
#endif
#include "mappane.h"

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
extern int _show_units;
#endif

#if defined HEXTOOLSMAP

extern const char* _weather_zone_string[];

// end any mode
void MapPane::CmEndModes( wxCommandEvent& WXUNUSED(event) )
{
	clear_all_modes( );
	parent_->SetStatusText( "" );
	Refresh( );
}

void MapPane::CmShowWZ( wxCommandEvent& WXUNUSED(event) )
{
	set_weather_zone_mode( );
	set_weather_zone( selected_weather_zone_ );
	enable_weather_zone_control( true );
	Refresh( );
}

void MapPane::CmMapleafmode( wxCommandEvent& WXUNUSED(event) )
{
	set_map_leaf_mode( );
	inventory_mapsheets( );
	Refresh( );

	wxString s = wxGetTextFromUser( _("Enter map ID (e.g. '4^^^' or '12^a')"), _("Map sheet mode") );

	if ( ! Hex::validateMapIdStr( &s ) && s.Len( ) > 0 )
	{
		char msg[50];
		sprintf( msg, "User error: illegal map ID '%s' specified", s.ToAscii() );
		wxMessageBox( msg, "Map ID Input Error" );
		clear_all_modes( );
		Refresh( );
		return;
	}

	if ( s.Len( ) == 0 )
		current_mapid_[0] = '\0';
	else
		strcpy( current_mapid_, s );

	Refresh( );
}

void MapPane::CmSeadistrictmode( wxCommandEvent& WXUNUSED(event) )
{
	set_seacircle_mode( );
	enable_seacircle_control( );
	parent_->SetStatusText( "Sea circle mode" );
	Refresh( );
}

void MapPane::CmClearAllSeaCircles( wxCommandEvent& WXUNUSED(event) )
{
	const wxString& message1( _("This will clear all sea circle definitions currently established in this map file!\n\nContinue anyway?") );
	if ( wxMessageBox( message1, _("Warning!"), wxYES_NO ) != wxYES )
		return;

	const wxString& message2( _("This really will clear ALL sea circle definitions currently established in this map file!\n\nReally continue??") );
	if ( wxMessageBox( message2, _("Warning!!"), wxYES_NO ) != wxYES )
		return;

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			hex_[y][x].setSeaDistrictID( 256 );

	Refresh( );
}

void MapPane::CmLanddistrictmode( wxCommandEvent& WXUNUSED(event) )
{
	set_district_mode( );
	enable_district_control( );
	parent_->SetStatusText( "Land district mode" );
	Refresh( );
}

void MapPane::CmClearAllLandDistricts( wxCommandEvent& WXUNUSED(event) )
{
	const wxString& message1( _("This will clear all land district definitions currently established in this map file!\n\nContinue anyway?") );
	if ( wxMessageBox( message1, _("Warning!"), wxYES_NO ) != wxYES )
		return;

	const wxString& message2( _("This really will clear ALL land district definitions currently established in this map file!\n\nReally continue??") );
	if ( wxMessageBox( message2, _("Warning!!"), wxYES_NO ) != wxYES )
		return;

	for ( int y = 0; y < world_y_size_; ++y )
		for ( int x = 0; x < world_x_size_; ++x )
			hex_[y][x].setLandDistrictID( 256 );

	Refresh( );
}

void MapPane::CmShowhexid( wxCommandEvent& WXUNUSED(event) )
{
	show_hexid_ = true;
	inventory_mapsheets( );
	Refresh( );
}

void MapPane::CmHidehexid( wxCommandEvent& WXUNUSED(event) )
{
	show_hexid_ = false;
	Refresh( );
}

void MapPane::set_weather_zone( int wz )
{
	selected_weather_zone_ = wz;
	wxString txt = wxString::FromAscii( "Weather zone selected: " );
	txt += wxString::FromAscii( _weather_zone_string[wz] );
	parent_->SetStatusText( txt );
}

void MapPane::set_current_seacircle( wxString sc )
{
	selected_sea_district_ = parent_->getSeaCircleDistrictId( sc );
	parent_->SetStatusText( wxT( "Sea circle selected: " ) + sc );
}

void MapPane::set_current_district( wxString dist )
{
	selected_land_district_ = parent_->getLandDistrictId( dist );
	parent_->SetStatusText( wxT( "Land district selected: " ) + dist );
}

void MapPane::set_weather_zone_mode( )
{
	clear_all_modes( );
	weather_zone_mode_ = true;
}

void MapPane::set_map_leaf_mode( )
{
	clear_all_modes( );
	mapleaf_mode_ = true;
	show_hexid_ = true;
}

void MapPane::set_seacircle_mode( )
{
	clear_all_modes( );
	sea_district_mode_ = true;
}

void MapPane::set_district_mode( )
{
	clear_all_modes( );
	land_district_mode_ = true;
}

void MapPane::clear_all_modes( )
{
	weather_zone_mode_ = false;
	enable_weather_zone_control( false );

	mapleaf_mode_ = false;
	show_hexid_ = false;

	sea_district_mode_ = false;
	enable_seacircle_control( false );

	land_district_mode_ = false;
	enable_district_control( false );
}

void MapPane::enable_weather_zone_control( bool enable )
{
	// save this code snippet for accessing Frame menus from MapPane
	//wxMenuBar* wzMenuBar = parent->GetMenuBar( );
	// wzMenuBar->EnableTop( wzMenuBar->FindMenu( "WZ" ), enable );

	// TODO: fix this reference to a magic number (control ID 11 for the WZ chooser)
	wxControl* selector = parent_->GetToolBar()->FindControl( 11 /* CMD_WZCHOICE */ );
	selector->Enable( enable );
}

void MapPane::enable_seacircle_control( bool enable )
{
	// TODO: fix this reference to a magic number (control ID 13 for the Sea Circle chooser)
	wxControl* selector = parent_->GetToolBar()->FindControl( 13 /* CMD_SEABOXCHOICE */ );
	selector->Enable( enable );
}

void MapPane::enable_district_control( bool enable )
{
	// TODO: fix this reference to a magic number (control ID 15 for the District chooser)
	wxControl* selector = parent_->GetToolBar()->FindControl( 15 /* CMD_DISTRICTCHOICE */ );
	selector->Enable( enable );
}

#elif defined HEXTOOLSPLAY

void MapPane::CmSeazonemode( wxCommandEvent& WXUNUSED(event) )
{
	set_seazone_mode( );
	enable_seazone_control( );
	parent_->SetStatusText( "Sea zone mode" );
	Refresh( );
}

void MapPane::CmCountrymode( wxCommandEvent& WXUNUSED(event) )
{
	set_country_mode( );
	enable_country_control( );
	parent_->SetStatusText( "Country mode" );
	Refresh( );
}

void MapPane::set_current_seazone( wxString sz /* = _("NONE") */ )
{
	selected_sea_zone_ = parent_->getSeaZoneId( sz );
	parent_->SetStatusText( wxT( "Sea zone selected: " ) + sz );
}

void MapPane::set_current_country( wxString country /* = _("NONE") */ )
{
	selected_country_ = parent_->getCountryId( country );
	parent_->SetStatusText( wxT( "Country selected: " ) + country );
}

void MapPane::set_seazone_mode( )
{
	country_mode_ = false;
	sea_zone_mode_ = true;
	_show_units = false;
	Refresh( );
}

void MapPane::set_country_mode( )
{
	sea_zone_mode_ = false;
	country_mode_ = true;
	_show_units = false;
	Refresh( );
}

void MapPane::clear_setup_modes( )
{
	country_mode_ = false;
	sea_zone_mode_ = false;
	_show_units = true;
	Refresh( );
}

void MapPane::enable_seazone_control( bool enable /* = true */ )
{
	// TODO: fix this reference to a magic number (control ID 2 for the Sea Zone chooser)
	wxControl* selector = parent_->GetToolBar()->FindControl( 2 /* CMD_SEAZONECHOICE */ );
	selector->Enable( enable );
}

void MapPane::enable_country_control( bool enable /* = true */ )
{
	// TODO: fix this reference to a magic number (control ID 4 for the Country chooser)
	wxControl* selector = parent_->GetToolBar()->FindControl( 4 /* CMD_COUNTRYCHOICE */ );
	selector->Enable( enable );
}

#endif
