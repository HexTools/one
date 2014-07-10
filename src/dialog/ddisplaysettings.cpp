#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "gamesetting.h"
#include "ddisplaysettings.h"

enum
{
	IDC_RADIOALLIEDATT,
	IDC_RADIOALLIEDDEF,
	IDC_RADIOAXISATT,
	IDC_RADIOAXISDEF,
	IDC_CHECKFLAT,
	IDC_CHECKWEATHER,
	IDC_CMBSUBFILT,
	IDC_CHKQSCROLLENABLE,
	IDC_CHKQSCROLLMIRROR,
	IDC_CHKBAK,
	IDC_CHKGRAYING,
	IDC_CHKCLICKTOMOVE
};

BEGIN_EVENT_TABLE(DDisplaySettings, wxDialog)
EVT_INIT_DIALOG(DDisplaySettings::EvInitDialog)
END_EVENT_TABLE()

extern DisplayOptionsDlgXfer _display_options_dialog_data;

DDisplaySettings::DDisplaySettings( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Settings"), wxDefaultPosition )
{
	SetSubTypePalette( GameSetting::ALL );

	quickscroll_velocity_ = 2;  // reasonable default

	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	// qscroll box
	wxStaticBox* item12 = new wxStaticBox( this, -1, wxT("Quickscroll") );
	wxSizer* item11 = new wxStaticBoxSizer( item12, wxVERTICAL );
	chkQScrollEnable = new wxCheckBox( this, -1, wxT("Enable Quickscroll (by moving mouse while pressing left button)"),
			wxDefaultPosition, wxDefaultSize, 0 );
	item11->Add( chkQScrollEnable, 0, wxALL | 0, 2 );
	chkQScrollMirror = new wxCheckBox( this, -1, wxT("Mirror directions"), wxDefaultPosition, wxDefaultSize, 0 );
	item11->Add( chkQScrollMirror, 0, wxALL | 0, 2 );
	// movement box
	wxStaticBox* itema1 = new wxStaticBox( this, -1, wxT("Movement") );
	wxSizer* itema2 = new wxStaticBoxSizer( itema1, wxVERTICAL );
	chkGrayHexes = new wxCheckBox( this, -1, wxT("Gray unreachable hexes"), wxDefaultPosition, wxDefaultSize, 0 );
	itema2->Add( chkGrayHexes, 0, wxALL | 0, 2 );
	chkClickToMove = new wxCheckBox( this, -1, wxT("Mouse click moves selected units"), wxDefaultPosition,
			wxDefaultSize, 0 );
	itema2->Add( chkClickToMove, 0, wxALL | 0, 2 );
	chkMpsOnMap = new wxCheckBox( this, -1, wxT("Show MPs on map"), wxDefaultPosition, wxDefaultSize, 0 );
	itema2->Add( chkMpsOnMap, 0, wxALL | 0, 2 );

	wxStaticBox* item15a = new wxStaticBox( this, -1, wxT("Graphics") );
	wxSizer* item15 = new wxStaticBoxSizer( item15a, wxVERTICAL );

	wxSizer* sz_tiles = new wxBoxSizer( wxHORIZONTAL );
	wxString ws[2];
	ws[0] = wxT("Standard HexTools map tiles");
	ws[1] = wxT("Tiles by Paul S. Hines");
	cmbTileset = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxSize( 200, -1 ), 2, ws,
			wxCB_DROPDOWN | wxCB_READONLY);

	sz_tiles->Add( new wxStaticText( this, -1, wxT("Map tiles:") ), 0, wxALL | 0, 2 );
	sz_tiles->Add( cmbTileset, 0, wxALL | 0, 2 );
	item15->Add( sz_tiles, 0, wxALL | 0, 2 );

	chkUseCorpsMarkers = new wxCheckBox( this, -1, wxT("Show corps markers displaying stack strength on map"),
			wxDefaultPosition, wxDefaultSize, 0 );
	item15->Add( chkUseCorpsMarkers, 0, wxALL | 0, 2 );

	wxSizer* item2 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticBox* item4 = new wxStaticBox( this, -1, wxT("Axis corps markers") );
	wxSizer* item3 = new wxStaticBoxSizer( item4, wxVERTICAL );
	rdAxisAtt = new wxRadioButton( this, -1, wxT("Show attack strength"), wxDefaultPosition, wxDefaultSize,
			wxRB_GROUP );
	item3->Add( rdAxisAtt, 0, wxLEFT | wxRIGHT | wxTOP | 0, 2 );
	rdAxisDef = new wxRadioButton( this, -1, wxT("Show defense strength"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( rdAxisDef, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 2 );
	item2->Add( item3, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | wxBOTTOM | 0, 2 );
	wxStaticBox* item8 = new wxStaticBox( this, -1, wxT("Allied corps markers") );
	wxSizer* item7 = new wxStaticBoxSizer( item8, wxVERTICAL );
	rdAlliedAtt = new wxRadioButton( this, -1, wxT("Show attack strength"), wxDefaultPosition, wxDefaultSize,
			wxRB_GROUP );
	item7->Add( rdAlliedAtt, 0, wxLEFT | wxRIGHT | wxTOP | 0, 2 );
	rdAlliedDef = new wxRadioButton( this, -1, wxT("Show defense strength"), wxDefaultPosition, wxDefaultSize, 0 );
	item7->Add( rdAlliedDef, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 2 );
	item2->Add( item7, 0, wxALIGN_CENTRE | wxLEFT | wxTOP | wxBOTTOM | 0, 2 );

	item15->Add( item2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	chkWeather = new wxCheckBox( this, -1, wxT("Weather aware map graphics"), wxDefaultPosition, wxDefaultSize, 0 );
	item15->Add( chkWeather, 0, wxALL | 0, 2 );

	// hex ownership
	wxSizer* sz_ownersym = new wxBoxSizer( wxHORIZONTAL );
	wxString ws2[3];
	ws2[0] = wxT("Soviet");
	ws2[1] = wxT("US");
	ws2[2] = wxT("British");
	cmbAlliedSymbol = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxSize( 100, -1 ), 3, ws2,
			wxCB_DROPDOWN | wxCB_READONLY);
	sz_ownersym->Add( new wxStaticText( this, -1, wxT("Allied hex ownership symbol:") ), 0, wxALL | 0, 2 );
	sz_ownersym->Add( cmbAlliedSymbol, 0, wxALL | 0, 2 );
	item15->Add( sz_ownersym, 0, wxALL | 0, 2 );

	wxSizer* item16 = new wxBoxSizer( wxVERTICAL );
	chkTrans = new wxCheckBox( this, -1, wxT("Transparent stack strength text background"), wxDefaultPosition,
			wxDefaultSize, 0 );
	item16->Add( chkTrans, 0, wxALL | 0, 2 );

	chkBak = new wxCheckBox( this, -1, wxT("Make backup copy ('name.bak') of the scenario file when saving"),
			wxDefaultPosition, wxDefaultSize, 0 );
	item16->Add( chkBak, 0, wxALL | 0, 2 );

	item0->Add( item15, 0, wxGROW | wxALL | 0, 5 );  // graphics
	item0->Add( item11, 0, wxGROW | wxALL | 0, 5 );  // qscroll
	item0->Add( itema2, 0, wxGROW | wxALL | 0, 5 );  // movement
	item0->Add( item16, 0, wxGROW | wxALL | 0, 5 );
	wxSizer* item20 = new wxBoxSizer( wxHORIZONTAL );
	item0->Add( item20, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item21 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item22 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( item22, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item23 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( item23, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item21, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	Center( );
}

DDisplaySettings::~DDisplaySettings( void )
{
}

typedef struct
{
	const char* name;
	int flag;
} subtype_flt;

#if 0
#define FLTS 9
subtype_flt flts[FLTS] =
{
	{ "FitE/SE",					GAME_FITE	},
	{ "Second Front",				GAME_SF		},
	{ "War in the Desert",			GAME_WITD	},
	{ "Balkan Front", 				GAME_BF		},
	{ "A Winter War", 				GAME_WW		},
	{ "For Whom the Bell Tolls",	GAME_FWTBT	},
	{ "Western theater",			GAME_WEST	},
	{ "All but FWtBT",				GAME_WW2	},
	{ "All",						GAME_ALL	}
};
#endif

void DDisplaySettings::CloseDialog( )
{
	_display_options_dialog_data.ChkWeather = chkWeather->GetValue( );
	_display_options_dialog_data.ChkTrans = chkTrans->GetValue( );
	_display_options_dialog_data.rdAxisAtt = rdAxisAtt->GetValue( );
	_display_options_dialog_data.rdAxisDef = rdAxisDef->GetValue( );
	_display_options_dialog_data.rdAlliedAtt = rdAlliedAtt->GetValue( );
	_display_options_dialog_data.rdAlliedDef = rdAlliedDef->GetValue( );
	_display_options_dialog_data.chkQScrollEnable = chkQScrollEnable->GetValue( );
	_display_options_dialog_data.chkQScrollMirror = chkQScrollMirror->GetValue( );
	_display_options_dialog_data.ChkBak = chkBak->GetValue( );
	_display_options_dialog_data.chkGrayHexes = chkGrayHexes->GetValue( );
	_display_options_dialog_data.chkClickToMove = chkClickToMove->GetValue( );
	_display_options_dialog_data.chkMpsOnMap = chkMpsOnMap->GetValue( );
	_display_options_dialog_data.chkUseCorpsMarkers = chkUseCorpsMarkers->GetValue( );
	_display_options_dialog_data.tileset = cmbTileset->GetSelection( );
	_display_options_dialog_data.alliedhexownersym = cmbAlliedSymbol->GetSelection( );
#if 0
	char s[255];
	strcpy( s, cmbSubtypeFilt->GetValue( ) );

	int  i;
	for ( i = 0; i < FLTS; ++i )
		if ( strcmp( s, flts[i].name ) == 0 )
			break;

	subtype_palette = flts[i].flag;
#endif

#if 0
	trackVel->SetPosition( QSVel );
#endif
}

void DDisplaySettings::EvInitDialog( wxInitDialogEvent& )
{
	chkWeather->SetValue( _display_options_dialog_data.ChkWeather );
	chkTrans->SetValue( _display_options_dialog_data.ChkTrans );
	rdAxisAtt->SetValue( _display_options_dialog_data.rdAxisAtt );
	rdAxisDef->SetValue( _display_options_dialog_data.rdAxisDef );
	rdAlliedAtt->SetValue( _display_options_dialog_data.rdAlliedAtt );
	rdAlliedDef->SetValue( _display_options_dialog_data.rdAlliedDef );
	chkQScrollEnable->SetValue( _display_options_dialog_data.chkQScrollEnable );
	chkQScrollMirror->SetValue( _display_options_dialog_data.chkQScrollMirror );
	chkBak->SetValue( _display_options_dialog_data.ChkBak );
	chkGrayHexes->SetValue( _display_options_dialog_data.chkGrayHexes );
	chkClickToMove->SetValue( _display_options_dialog_data.chkClickToMove );
	chkMpsOnMap->SetValue( _display_options_dialog_data.chkMpsOnMap );
	chkUseCorpsMarkers->SetValue( _display_options_dialog_data.chkUseCorpsMarkers );
	cmbTileset->SetSelection( _display_options_dialog_data.tileset );
	cmbAlliedSymbol->SetSelection( _display_options_dialog_data.alliedhexownersym );

#if 0
	 cmbSubtypeFilt->Clear( );
	 for ( i = 0; i < FLTS; ++i )
	 	cmbSubtypeFilt->Append( flts[i].name );

	 cmbSubtypeFilt->SetSelection( 0 );

	 for ( i = 0; i < FLTS; ++i )
		 if ( flts[i].flag == subtype_palette ) 
		 {
			 cmbSubtypeFilt->SetSelection( i );
			 break;
		 }
#endif
	if ( quickscroll_velocity_ > 4 ) 
		quickscroll_velocity_ = 4;

	if ( quickscroll_velocity_ < 1 ) 
		quickscroll_velocity_ = 1;
#if 0
	trackVel->SetPosition( QSVel );
#endif
}

#endif
