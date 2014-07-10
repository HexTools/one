#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;
using std::endl;

#include <wx/wx.h>

#include "hextools.h"
using ht::wxS;
#include "dhex.h"
#include "weatherclass.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"

extern HexType _hex_types[]; //[TERRAINTYPECOUNT];    // terrain types

extern HexDlgXfer _hex_dialog_data;

enum
{
	IDC_BTNOWNER = 1,
	IDC_BTNGAUGE
};
#define ID_TEXT -1
#define ID_BUTTON -1
#define ID_PANEL -1

BEGIN_EVENT_TABLE(DHexInfo, wxDialog)
EVT_INIT_DIALOG(DHexInfo::EvInitDialog)
EVT_PAINT(DHexInfo::OnPaintEv)
EVT_BUTTON(IDC_BTNOWNER, DHexInfo::OwnerClicked)
EVT_BUTTON(IDC_BTNGAUGE, DHexInfo::GaugeClicked)
END_EVENT_TABLE()

static wxFont bfont( 8, wxROMAN, wxNORMAL, wxBOLD );

DHexInfo::DHexInfo( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Hex Info"), wxDefaultPosition )
{
	wxBoxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("Hex properties") );
	wxStaticBoxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

	wxFlexGridSizer* item3 = new wxFlexGridSizer( 2, 0, 0 );

	pHex = new wxPanel( this, ID_PANEL, wxDefaultPosition, wxSize( 30, 30 ), wxRAISED_BORDER );
	item3->Add( pHex, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, 5 );
	pHex->Show( false );

	lblHexId = new wxStaticText( this, ID_TEXT, wxT("1A:1234 (10,20)"), wxDefaultPosition, wxSize( 140, -1 ), 0 );
	item3->Add( lblHexId, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxStaticText* item6 = new wxStaticText( this, ID_TEXT, wxT("Terrain:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item6, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	lblTerrain = new wxStaticText( this, ID_TEXT, wxT("Wooded rough"), wxDefaultPosition, wxDefaultSize, 0 );
	lblTerrain->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	lblTerrain->SetSize(lblTerrain->GetBestSize());
#endif
	item3->Add( lblTerrain, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxStaticText* item8 = new wxStaticText( this, ID_TEXT, wxT("Owner:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item8, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxBoxSizer* item9 = new wxBoxSizer( wxHORIZONTAL );

	lblOwner = new wxStaticText( this, ID_TEXT, wxT("Allied"), wxDefaultPosition, wxSize( 100, -1 ),
			wxST_NO_AUTORESIZE );
	lblOwner->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	lblOwner->SetSize(lblOwner->GetBestSize());
#endif
	item9->Add( lblOwner, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM, 5 );
#if 0
	wxButton* item11 = new wxButton( this, ID_BUTTON, "<>", wxDefaultPosition, wxSize( 20,-1 ), 0 );
	item9->Add( item11, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5 );
#endif
	item3->Add( item9, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 0 );

	wxStaticText* item12 = new wxStaticText( this, ID_TEXT, wxT("Weather"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item12, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	lblWeather = new wxStaticText( this, ID_TEXT, wxT("Clear (zone D)"), wxDefaultPosition, wxDefaultSize, 0 );
	lblWeather->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	lblWeather->SetSize(lblWeather->GetBestSize());
#endif
	item3->Add( lblWeather, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxStaticText* item14 = new wxStaticText( this, ID_TEXT, wxT("Railroad:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item14, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxBoxSizer* item15 = new wxBoxSizer( wxHORIZONTAL );

	lblRail = new wxStaticText( this, ID_TEXT, wxT("Wide, broken!"), wxDefaultPosition, wxSize( 120, -1 ), wxST_NO_AUTORESIZE );
	item15->Add( lblRail, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

#if 0
	wxButton* item17 = new wxButton( this, ID_BUTTON, "<>", wxDefaultPosition, wxSize( 20, -1 ), 0 );
	item15->Add( item17, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5 );
#endif
	item3->Add( item15, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 0 );

	wxStaticText* item18 = new wxStaticText( this, ID_TEXT, wxT("Airfield:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item18, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	lblAirField = new wxStaticText( this, ID_TEXT, wxT("Capacity: 2 (net 3, 1 hits)"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( lblAirField, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxStaticText* item20 = new wxStaticText( this, ID_TEXT, wxT("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item20, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	lblPort = new wxStaticText( this, ID_TEXT, wxT("Major port, Capacity 12 (net 12, 0 hits).\n"
			"This is strong natural port."), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( lblPort, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxStaticText* item22 = new wxStaticText( this, ID_TEXT, wxT("Coastal defense:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item22, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxStaticText* item23 = new wxStaticText( this, ID_TEXT, wxT("2 points (net 2, 0 hits)"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item23, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	wxStaticText* item24 = new wxStaticText( this, ID_TEXT, wxT("Stuff in hex:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item24, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	lblCity = new wxStaticText( this, ID_TEXT, wxT("- Point city: Urjala\n"
			"- Improved Fortress: juhka"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( lblCity, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	item1->Add( item3, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	item0->Add( item1, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxStaticBox* item27 = new wxStaticBox( this, -1, wxT("Combat effects") );
	wxStaticBoxSizer* item26 = new wxStaticBoxSizer( item27, wxVERTICAL );

	wxStaticText* item28 = new wxStaticText( this, ID_TEXT, wxT("- No AEC\n"
			"- Attacker halved"), wxDefaultPosition, wxDefaultSize, 0 );
	item28->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item28->SetSize(item28->GetBestSize());
#endif
	item26->Add( item28, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 20 );

	wxFlexGridSizer* item29 = new wxFlexGridSizer( 3, 0, 0 );

	wxStaticText* item30 = new wxStaticText( this, ID_TEXT, wxT("Die modifier:"), wxDefaultPosition, wxDefaultSize, 0 );
	item29->Add( item30, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM, 5 );

	wxStaticText* item31 = new wxStaticText( this, ID_TEXT, wxT("-2"), wxDefaultPosition, wxSize( 20, -1 ), 0 );
	item31->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item31->SetSize(item31->GetBestSize());
#endif
	item29->Add( item31, 0, wxALIGN_CENTRE | wxALL, 5 );

	wxStaticText* item32 = new wxStaticText( this, ID_TEXT, wxT("(Hex: -1, Fort: -1)"), wxDefaultPosition, wxSize( 200, -1 ), 0 );
	item29->Add( item32, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | wxBOTTOM, 5 );

	item26->Add( item29, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	item0->Add( item26, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxStaticBox* item34 = new wxStaticBox( this, -1, wxT("Movement points") );
	wxStaticBoxSizer* item33 = new wxStaticBoxSizer( item34, wxHORIZONTAL );

	wxFlexGridSizer* item35 = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText* item36 = new wxStaticText( this, ID_TEXT, wxT("Infantry:"), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( item36, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item37 = new wxStaticText( this, ID_TEXT, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	item37->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item37->SetSize(item37->GetBestSize());
#endif
	item35->Add( item37, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item38 = new wxStaticText( this, ID_TEXT, wxT("Mot/Art:"), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( item38, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item39 = new wxStaticText( this, ID_TEXT, wxT("1/2MP+1"), wxDefaultPosition, wxDefaultSize, 0 );
	item39->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item39->SetSize(item39->GetBestSize());
#endif
	item35->Add( item39, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item40 = new wxStaticText( this, ID_TEXT, wxT("Light:"), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( item40, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item41 = new wxStaticText( this, ID_TEXT, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	item41->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item41->SetSize(item41->GetBestSize());
#endif
	item35->Add( item41, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item42 = new wxStaticText( this, ID_TEXT, wxT("Cavalry:"), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( item42, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item43 = new wxStaticText( this, ID_TEXT, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	item43->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item43->SetSize(item43->GetBestSize());
#endif
	item35->Add( item43, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item44 = new wxStaticText( this, ID_TEXT, wxT("Mountain:  "), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( item44, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item45 = new wxStaticText( this, ID_TEXT, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	item45->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item45->SetSize(item45->GetBestSize());
#endif
	item35->Add( item45, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item46 = new wxStaticText( this, ID_TEXT, wxT("Ski:"), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( item46, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item47 = new wxStaticText( this, ID_TEXT, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	item47->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	item47->SetSize(item47->GetBestSize());
#endif
	item35->Add( item47, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5 );

	item33->Add( item35, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT, 15 );

	wxBoxSizer* item48 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* item49 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* item50 = new wxStaticText( this, ID_TEXT, wxT("Harassment:"), wxDefaultPosition, wxDefaultSize, 0 );
	item49->Add( item50, 0, wxALIGN_CENTRE | wxLEFT, 0 );

	lblHarr = new wxStaticText( this, ID_TEXT, wxT("+1 MP"), wxDefaultPosition, wxDefaultSize, 0 );
	lblHarr->SetFont( bfont );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	lblHarr->SetSize(lblHarr->GetBestSize());
#endif
	item49->Add( lblHarr, 0, wxALIGN_CENTRE | wxALL, 5 );

	item48->Add( item49, 0, wxGROW | wxLEFT, 0 );

	lblHarrInfo = new wxStaticText( this, ID_TEXT, wxT("Harassment is not included in\n"
			"MP's show left."), wxDefaultPosition, wxDefaultSize, 0 );
	item48->Add( lblHarrInfo, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM, 5 );

	item33->Add( item48, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT, 30 );

	item0->Add( item33, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxBoxSizer* item53 = new wxBoxSizer( wxHORIZONTAL );

	wxButton* item54 = new wxButton( this, wxID_OK, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	item53->Add( item54, 0, wxALIGN_CENTRE | wxALL, 5 );
#if 0
	wxButton *item55 = new wxButton( this, wxID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0 );
	item53->Add( item55, 0, wxALIGN_CENTRE|wxALL, 5 );
#endif
	item0->Add( item53, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	Centre( );
}

void DHexInfo::EvInitDialog( wxInitDialogEvent& WXUNUSED(e) )
{
	SetupDialog( );
}

void DHexInfo::SetupDialog( )
{
	//values to controls...
}

void DHexInfo::CloseDialog( )
{
	// controls->values
}

void DHexInfo::GaugeClicked( wxCommandEvent& WXUNUSED(e) )
{
}

void DHexInfo::OwnerClicked( wxCommandEvent& WXUNUSED(e) )
{
}

void DHexInfo::OnPaintEv( wxPaintEvent& WXUNUSED(e) )
{
	wxPaintDC dc( this );
	int x, y;
	pHex->GetPosition( &x, &y );
	dc.DrawBitmap( *_hex_types[_hex_dialog_data.terrain].hbmBitmap, x, y - 3 );
	lblHexId->SetLabel( wxS( _hex_dialog_data.hexid ) );
	lblTerrain->SetLabel( wxS( _hex_dialog_data.terrain_str ) );
	lblOwner->SetLabel( wxS( _hex_dialog_data.owner ) );
	lblWeather->SetLabel( wxS( _hex_dialog_data.weather ) );
	lblRail->SetLabel( wxS( _hex_dialog_data.rail ) );
	lblHarr->SetLabel( wxS( _hex_dialog_data.harr ) );
	lblHarrInfo->Show( strlen( _hex_dialog_data.harr ) > 1 ? true : false );
	lblAirField->SetLabel( wxS( _hex_dialog_data.airfield ) );
	lblPort->SetLabel( wxS( _hex_dialog_data.port ) );
	lblCity->SetLabel( wxS( _hex_dialog_data.city ) );
}

#endif
