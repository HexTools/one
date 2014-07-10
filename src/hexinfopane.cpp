#include <wx/wx.h>

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#endif
#include "hextools.h"
using ht::wxS;
#include "hexinfopane.h"
#include "hexsidetype.h"
#if defined HEXTOOLSMAP
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "label.h"
#endif

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
BEGIN_EVENT_TABLE( HexInfoPane, wxWindow )
EVT_PAINT( HexInfoPane::Paint )
END_EVENT_TABLE( )

extern wxBitmap* hbmHexInfoPaneBgnd;

static wxFont lblfont( 11, wxSWISS, wxNORMAL, wxBOLD );
static wxFont lblfont2( 10, wxSWISS, wxNORMAL, wxNORMAL );
#endif

HexInfoPane::HexInfoPane( wxWindow* p, int i, const wxPoint& pt, const wxSize& sz, long WXUNUSED(l) )
		: wxWindow( p, i, pt, sz, wxTAB_TRAVERSAL, _("hexinfopane") )
{
#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
#define BOXHEIGHT		17				// height of labels
// display setup
#define GENINFO_Y		2
#define TURN_Y			GENINFO_Y+3		// "JUN II"
#define HEXINFO_Y		33
#define LOC_Y			HEXINFO_Y+3		// y-location for "6,5 clear"
#define HEXSIDE_Y		HEXINFO_Y+27	// y-location for "mountain"
#define PRIVATE_HEXSIDE_Y HEXINFO_Y+51	// y-location for "escarpment"
#define ROAD_Y			HEXINFO_Y+75	// y-location for "railroad"
#define WEATHER_Y		HEXINFO_Y+99	// "mud"
#define TEXT_Y			HEXINFO_Y+123	// y-location for "Berlin"
#define UNITS_Y			182				// Units window texts
#endif

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
	hbmHexInfoPaneBgnd = new wxBitmap( wxT("graphics/images/ibbgnd.bmp"), wxBITMAP_TYPE_BMP );
#endif

	// separator line
#if defined HEXTOOLSMAP
	new wxStaticBox( this, -1, "", wxPoint( -2, -8 ), wxSize( 260, 136 ), wxSUNKEN_BORDER );
#elif defined HEXTOOLSPLAY
	//new wxStaticBox( this, -1, "", wxPoint( -2, -8 ), wxSize( 220, 136 ), wxSUNKEN_BORDER );
	lblTurn = new wxStaticText( this, -1, wxT(""), wxPoint( 3, 5 ), wxSize( 70, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	lblTurn->SetBackgroundColour( *wxWHITE );
	lblTurn->SetFont( lblfont );

	lblPhase = new wxStaticText( this, -1, wxT(""), wxPoint( 75, 5 ), wxSize( 130, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	lblPhase->SetBackgroundColour( *wxWHITE );
	lblPhase->SetFont( lblfont );
#endif

	// hex location
#if defined HEXTOOLSMAP
	hex_number_ = new Label( this, wxPoint( 5, 5 ), wxSize( 80, 20 ) );
	hex_id_ = new Label( this, wxPoint( 90, 5 ), wxSize( 150, 20 ) );
#elif defined HEXTOOLSPLAY
	hex_id_ = new wxStaticText( this, -1, wxT(""), wxPoint( 3, 30 ), wxSize( 60, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	hex_id_->SetBackgroundColour( *wxWHITE );
	hex_id_->SetFont( lblfont2 );
#endif

	// terrain type
#if defined HEXTOOLSMAP
	new wxStaticText( this, -1, "Terrain", wxPoint( 5, 25 ) );
	terrain_ = new Label( this, wxPoint( 50, 25 ), wxSize( 190, 20 ) );
#elif defined HEXTOOLSPLAY
	terrain_ = new wxStaticText( this, -1, wxT(""), wxPoint( 70, 30 ), wxSize( 130, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	terrain_->SetBackgroundColour( *wxWHITE );
	terrain_->SetFont( lblfont2 );
#endif

	// hexside
#if defined HEXTOOLSMAP
	new wxStaticText( this, -1, "Hexside", wxPoint( 5, 45 ) );
	hexside_ = new Label( this, wxPoint( 50, 45 ), wxSize( 190, 20 ) );
#elif defined HEXTOOLSPLAY
	hexside_ = new wxStaticText( this, -1, wxT(""), wxPoint( 70, 50 ), wxSize( 130, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	hexside_->SetBackgroundColour( *wxWHITE );
	hexside_->SetFont( lblfont2 );
#endif

	// road
#if defined HEXTOOLSMAP
	new wxStaticText( this, -1, "Route", wxPoint( 5, 65 ) );
	road_ = new Label( this, wxPoint( 50, 65 ), wxSize( 190, 20 ) );
#elif defined HEXTOOLSPLAY
	road_ = new wxStaticText( this, -1, wxT(""), wxPoint( 70, 70 ), wxSize( 130, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	road_->SetBackgroundColour( *wxWHITE );
	road_->SetFont( lblfont2 );
#endif

	// extended hexside
#if defined HEXTOOLSMAP
	new wxStaticText( this, -1, "Feature", wxPoint( 5, 85 ) );
	external_hexside_ = new Label( this, wxPoint( 50, 85 ), wxSize( 190, 20 ) );
#elif defined HEXTOOLSPLAY
	external_hexside_ = new wxStaticText( this, -1, wxT(""), wxPoint( 70, 90 ), wxSize( 130, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	external_hexside_->SetBackgroundColour( *wxWHITE );
	external_hexside_->SetFont( lblfont2 );
	// weather
	//new wxStaticText(this, -1, "Weather:", wxPoint(3,111));
	lblWeather = new wxStaticText( this, -1, wxT(""), wxPoint( 70, 110 ), wxSize( 130, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	lblWeather->SetBackgroundColour( *wxWHITE );
	lblWeather->SetFont( lblfont2 );
#endif

	// city
#if defined HEXTOOLSMAP
	city_ = new Label( this, wxPoint( 5, 105 ), wxSize( 235, 20 ) );
#elif defined HEXTOOLSPLAY
	city_ = new wxStaticText( this, -1, wxT(""), wxPoint( 3, 130 ), wxSize( 197, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	city_->SetBackgroundColour( *wxWHITE );
	city_->SetFont( lblfont );
#endif

	// sea circle and district names
#if defined HEXTOOLSMAP
	seabox_ = new Label( this, wxPoint( 5, 125 ), wxSize( 235, 20 ) );
	district_ = new Label( this, wxPoint( 5, 145 ), wxSize( 235, 20 ) );
#elif defined HEXTOOLSPLAY
	seabox_ = new wxStaticText( this, -1, wxT(""), wxPoint( 3, 150 ), wxSize( 197, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	seabox_->SetBackgroundColour( *wxWHITE );
	seabox_->SetFont( lblfont2 );

	district_ = new wxStaticText( this, -1, wxT(""), wxPoint( 3, 170 ), wxSize( 197, BOXHEIGHT ), wxST_NO_AUTORESIZE );
	district_->SetBackgroundColour( *wxWHITE );
	district_->SetFont( lblfont2 );
#endif
}

#undef LBL_TYPE

HexInfoPane::~HexInfoPane( )
{
}

void HexInfoPane::displayHexInfo( 
#if defined HEXTOOLSMAP
	int x, 
	int y, 
#elif defined HEXTOOLSPLAY
	int x __attribute__ ((unused)),
	int y __attribute__ ((unused)),
#endif
	char* mapid, 
	char* hexid, 
	wxString terrain, 
	int hexside, 
	int road,
	int inthexside, 
	wxString city,
	bool stacking, 
	bool waterway, 
	bool vertex, 
	wxString seabox, 
	wxString district,
#if defined HEXTOOLSMAP
	wxString seazone __attribute__ ((unused)),
	wxString country __attribute__ ((unused)),
	char* wzone __attribute__ ((unused)),
	char* weather __attribute__ ((unused))
#elif defined HEXTOOLSPLAY
	wxString seazone,
	wxString country,
	char* wzone,
	char* weather 
#endif
)
{
	char s[100];
#if defined HEXTOOLSMAP
	sprintf( s, "%d, %d", x, y );
	hex_number_->setLabel( s );
#endif
	sprintf( s, "%s:%s", mapid, hexid );
	wxString maphex( s );
	maphex.Replace( "^", "" );
	hex_id_->SetLabel( wxS( maphex ) );

#if defined HEXTOOLSMAP
	terrain_->setLabel( (char*)terrain.mb_str(wxConvUTF8) );
#elif defined HEXTOOLSPLAY
	terrain_->SetLabel( wxS( terrain ) );
#endif

	switch ( hexside )
	{
		case HexsideType::MINORRIVER:
			sprintf( s, "River" );
			break;
		case HexsideType::CANAL:
			sprintf( s, "Canal" );
			break;
		case HexsideType::MINOR_CANAL:
			sprintf( s, "Minor Canal" );
			break;
		case HexsideType::MAJORRIVER:
			sprintf( s, "Major River" );
			break;
		case HexsideType::SEASONALRIVER:
			sprintf( s, "Seasonal River" );
			break;
		case HexsideType::GREATRIVERFORD:
			sprintf( s, "Great River [ford]" );
			break;
		case HexsideType::MAJORRIVERFORD:
			sprintf( s, "Major River [ford]" );
			break;
		case HexsideType::LAKE_SIDE:
			sprintf( s, "Lake/Great River" );
			break;
		case HexsideType::RWATER_SIDE:
			sprintf( s, "Sea (restr. waters capable)" );
			break;
		case HexsideType::SEA_SIDE:
			sprintf( s, "Sea (restr. waters lock out)" );
			break;
		case HexsideType::ICING_RWATER_SIDE:
			sprintf( s, "Sea (can ice, restr. capable)" );
			break;
		case HexsideType::ICING_SEA_SIDE:
			sprintf( s, "Sea (can ice, no restr.)" );
			break;
		case HexsideType::MTN_SIDE:
			sprintf( s, "Mountain" );
			break;
		case HexsideType::HIGH_MTN_SIDE:
			sprintf( s, "High Mountain Side" );
			break;
		case HexsideType::KARST:
			sprintf( s, "Karst" );
			break;
		case HexsideType::HIGH_MTN_PASS:
			sprintf( s, "High Mountain Pass" );
			break;
		case HexsideType::BORDER:
			sprintf( s, "International Border" );
			break;
		case HexsideType::IBORDER:
			sprintf( s, "Internal Border" );
			break;
		case HexsideType::SUBIBORDER:
			sprintf( s, "Internal Sub-Border" );
			break;
		case HexsideType::WADI:
			sprintf( s, "Wadi" );
			break;
		case HexsideType::GLACIER_SIDE:
			sprintf( s, "Glacier" );
			break;
		case HexsideType::SALT_DESERT_SIDE:
			sprintf( s, "Salt Desert" );
			break;
		case HexsideType::FILLING_RESERVOIR_SIDE:
			sprintf( s, "Filling Reservoir" );
			break;
		case HexsideType::DBL_ESCARPMENT:
			sprintf( s, "Double Escarpment" );
			break;
		case HexsideType::IMP_DBL_ESCARPMENT:
			sprintf( s, "Impassable Double Escarpment" );
			break;
		default:
			sprintf( s, stacking || waterway ? "" : " " );
			break;
	}

	// for limited stacking, insert trailing space if any hexside terrain is present
	// and append a limited label suffix
	if ( waterway && stacking && strlen( s ) > 8 )
		strcat( s, " [water] [lim]");
	else
	{
		if ( waterway )
			strcat( s, " [waterway]" );
		if ( stacking )
			strcat( s, " [limited]" );
	}
#if defined HEXTOOLSMAP
	hexside_->setLabel( s );
#elif defined HEXTOOLSPLAY
	hexside_->SetLabel( wxS( s ) );
#endif
	switch ( road )
	{
		case HexsideType::RAILROAD:
			sprintf( s, "Railroad" );
			break;
		case HexsideType::RAILTRACK:
			sprintf( s, "Railtrack" );
			break;
		case HexsideType::ROAD:
			sprintf( s, "Road" );
			break;
		case HexsideType::STANDALONEBRIDGE:
			sprintf( s, "Standalone Bridge" );
			break;
		case HexsideType::MOTORTRACK:
			sprintf( s, "Motortrack" );
			break;
		case HexsideType::TRACK:
			sprintf( s, "Track" );
			break;
		case HexsideType::RAIL_FERRY:
			sprintf( s, "Rail Ferry" );
			break;
		case HexsideType::NARROW_STRAITS:
			sprintf( s, "Narrow Straits" );
			break;
		case HexsideType::NARROW_STRAITS_FERRY:
			sprintf( s, "Narrow Straits Ferry" );
			break;
		case HexsideType::LAKESEA_FERRY:
			sprintf( s, "Lake/Sea Ferry" );
			break;
		case HexsideType::SHIP_CHANNEL:
			sprintf( s, "Shipping Channel" );
			break;
		default:
			sprintf( s, " " );
			break;
	}
#if defined HEXTOOLSMAP
	road_->setLabel( s );
#elif defined HEXTOOLSPLAY
	road_->SetLabel( wxS( s ) );
#endif
	switch ( inthexside )
	{
		case HexsideType::ESCARPMENT:
			sprintf( s, "Escarpment" );
			break;
		case HexsideType::IMP_ESCARPMENT:
			sprintf( s, "Impassable Escarpment" );
			break;
		case HexsideType::FORTIFIED:
			sprintf( s, "Fortified" );
			break;
		case HexsideType::GREAT_WALL:
			sprintf( s, "Great Wall" );
			break;
		case HexsideType::MTN_SIDE2:
			sprintf( s, "Interior Mountain" );
			break;
		default:
			s[0] = '\0';
			break;
	}

	if ( vertex ) // append "Dam" when there is a dam on the nearest vertex
		strcat( s, strlen( s ) ? "; Dam" : "Dam" );

#if defined HEXTOOLSMAP
	external_hexside_->setLabel( s );
	city_->setLabel( city );
	seabox_->setLabel( const_cast<char*>( seabox.mb_str() ) );
	district_->setLabel( const_cast<char*>( district.mb_str() ) );
#elif defined HEXTOOLSPLAY
	external_hexside_->SetLabel( wxS( s ) );

	//sprintf( s, "%s [%s]", weather, wzone );
	//lblWeather->SetLabel( wxS( s ) );
	lblWeather->SetLabel( wxS( weather ) + " [" + wxS( wzone ) + "]" );

	city_->SetLabel( wxS( city ) );
	seabox_->SetLabel( seazone + " [" + seabox + "]" );
	district_->SetLabel( country + " [" + district + "]" );
#endif
}

#if defined HEXTOOLSMAP
#elif defined HEXTOOLSPLAY
void HexInfoPane::Paint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc( this );
	PrepareDC( dc );  // must be called or automatic scrolling won't work

	if ( hbmHexInfoPaneBgnd )
		dc.DrawBitmap( *hbmHexInfoPaneBgnd, 0, 0 );
	dc.DrawText( wxT("Hexside:"), 3, 51 );
	dc.DrawText( wxT("Road:"), 3, 71 );
	dc.DrawText( wxT("Other:"), 3, 91 );
	dc.DrawText( wxT("Weather:"), 3, 111 );
}
#endif
