#if defined HEXTOOLSPLAY

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "dweather.h"
#include "weatherzone.h"
#if 0
extern WeatherDlgXfer WeatherDlgData;
#endif
extern const wxString _weather_condition_string[];	// 5  // from mappane.cpp
extern int _weather_condition[];					// [W_ZONES]	// W_MUD etc., per zone (frame.cpp)
extern int _rivers_lakes_frozen[];					// [W_ZONES]	// true if rivers/lakes are frozen (frame.cpp)
#if 0
extern int wturns[];		// for freezing calculations
#endif

static const char* wz_codes[WeatherZone::COUNT] =
{
"A", "B", "C", "D", "E", "F", "G", "H1", "H2", "H3", "I1", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11",
"12", "13", "14", "15", "16", "17", "18", "I2", "I3", "IS", "J1", "J2", "K", "L", "M", "N", "O", "P1", "P2", "Q"
};

enum
{
#if 0
	IDC_RADIO_A_CLEAR,
	IDC_RADIO_A_MUD,
	IDC_RADIO_A_SNOW,
	IDC_RADIO_B_CLEAR,
	IDC_RADIO_B_FROST,
	IDC_RADIO_B_MUD,
	IDC_RADIO_B_SNOW,
	IDC_RADIO_C_CLEAR,
	IDC_RADIO_C_FROST,
	IDC_RADIO_C_MUD,
	IDC_RADIO_C_SNOW,
	IDC_RADIO_D_CLEAR,
	IDC_RADIO_D_FROST,
	IDC_RADIO_D_MUD,
	IDC_RADIO_D_WINTER,
	IDC_RADIO_E_CLEAR,
	IDC_RADIO_E_MUD,
	IDC_RADIO_F_CLEAR,
	IDC_RADIO_G_CLEAR,
	IDC_RADIO_G_SNOW,
	IDC_RADIO_G_WINTER,
	IDC_RADIO_D_SNOW,
	IDC_CHKICEA,
	IDC_CHKICEB,
	IDC_CHKICEC,
	IDC_CHKICED,
	IDC_CHKICEG,
#endif
	IDC_LISTWZ,
	IDC_BTNCHANGEW,
	IDC_BTNCHANGEI
};

BEGIN_EVENT_TABLE(DWeather, wxDialog) EVT_INIT_DIALOG(DWeather::EvInitDialog)
EVT_BUTTON(IDC_BTNCHANGEW, DWeather::ChangeWeatherClick)
EVT_BUTTON(IDC_BTNCHANGEI, DWeather::ChangeIceClick)
END_EVENT_TABLE()

DWeather::DWeather( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Weather Zones"), wxDefaultPosition )
{
	wxSizer* main_v_sizer = new wxBoxSizer( wxVERTICAL );
#if 0
	wxSizer* h_sizer = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* txte2 = new wxStaticText( this, -1, wxT("NOTE: 'Ice' means frozen rivers and lakes."), wxDefaultPosition, wxDefaultSize, 0 );
	//main_v_sizer->Add( txte2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxStaticText* txte = new wxStaticText( this, -1, wxT("European Weather Zones:"), wxDefaultPosition, wxDefaultSize, 0 );
	//main_v_sizer->Add( txte, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxStaticBox* item3 = new wxStaticBox( this, -1, wxT("Zone A") );
	wxSizer* item2 = new wxStaticBoxSizer( item3, wxVERTICAL );
	RdAClear = new wxRadioButton( this, -1, wxT("Clear"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	item2->Add( RdAClear, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdAFrost = new wxRadioButton( this, -1, wxT("Frost"), wxDefaultPosition, wxDefaultSize, 0 );
	item2->Add( RdAFrost, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdAMud = new wxRadioButton( this, -1, wxT("Mud"), wxDefaultPosition, wxDefaultSize, 0 );
	item2->Add( RdAMud, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdASnow = new wxRadioButton( this, -1, wxT("Snow"), wxDefaultPosition, wxDefaultSize, 0 );
	item2->Add( RdASnow, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdAWinter = new wxRadioButton( this, -1, wxT("Winter"), wxDefaultPosition, wxDefaultSize, 0 );
	item2->Add( RdAWinter, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	chkIceA = new wxCheckBox( this, -1, wxT("Ice"), wxDefaultPosition, wxDefaultSize, 0 );
	item2->Add( chkIceA, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	h_sizer->Add( item2, 0, wxALIGN_CENTRE | wxALL | 0, 1 );

	wxStaticBox* itemb = new wxStaticBox( this, -1, wxT("Zone B") );
	wxSizer* itemb2 = new wxStaticBoxSizer( itemb, wxVERTICAL );
	RdBClear = new wxRadioButton( this, -1, wxT("Clear"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	itemb2->Add( RdBClear, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdBFrost = new wxRadioButton( this, -1, wxT("Frost"), wxDefaultPosition, wxDefaultSize, 0 );
	itemb2->Add( RdBFrost, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdBMud = new wxRadioButton( this, -1, wxT("Mud"), wxDefaultPosition, wxDefaultSize, 0 );
	itemb2->Add( RdBMud, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdBSnow = new wxRadioButton( this, -1, wxT("Snow"), wxDefaultPosition, wxDefaultSize, 0 );
	itemb2->Add( RdBSnow, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdBWinter = new wxRadioButton( this, -1, wxT("Winter"), wxDefaultPosition, wxDefaultSize, 0 );
	itemb2->Add( RdBWinter, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	chkIceB = new wxCheckBox( this, -1, wxT("Ice"), wxDefaultPosition, wxDefaultSize, 0 );
	itemb2->Add( chkIceB, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	h_sizer->Add( itemb2, 0, wxALIGN_CENTRE | wxALL | 0, 1 );

	wxStaticBox* itemc = new wxStaticBox( this, -1, wxT("Zone C") );
	wxSizer* itemc2 = new wxStaticBoxSizer( itemc, wxVERTICAL );
	RdCClear = new wxRadioButton( this, -1, wxT("Clear"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	itemc2->Add( RdCClear, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdCFrost = new wxRadioButton( this, -1, wxT("Frost"), wxDefaultPosition, wxDefaultSize, 0 );
	itemc2->Add( RdCFrost, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdCMud = new wxRadioButton( this, -1, wxT("Mud"), wxDefaultPosition, wxDefaultSize, 0 );
	itemc2->Add( RdCMud, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdCSnow = new wxRadioButton( this, -1, wxT("Snow"), wxDefaultPosition, wxDefaultSize, 0 );
	itemc2->Add( RdCSnow, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdCWinter = new wxRadioButton( this, -1, wxT("Winter"), wxDefaultPosition, wxDefaultSize, 0 );
	itemc2->Add( RdCWinter, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	chkIceC = new wxCheckBox( this, -1, wxT("Ice"), wxDefaultPosition, wxDefaultSize, 0 );
	itemc2->Add( chkIceC, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	h_sizer->Add( itemc2, 0, wxALIGN_CENTRE | wxALL | 0, 1 );

	wxStaticBox* itemd = new wxStaticBox( this, -1, wxT("Zone D") );
	wxSizer* itemd2 = new wxStaticBoxSizer( itemd, wxVERTICAL );
	RdDClear = new wxRadioButton( this, -1, wxT("Clear"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	itemd2->Add( RdDClear, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdDFrost = new wxRadioButton( this, -1, wxT("Frost"), wxDefaultPosition, wxDefaultSize, 0 );
	itemd2->Add( RdDFrost, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdDMud = new wxRadioButton( this, -1, wxT("Mud"), wxDefaultPosition, wxDefaultSize, 0 );
	itemd2->Add( RdDMud, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdDSnow = new wxRadioButton( this, -1, wxT("Snow"), wxDefaultPosition, wxDefaultSize, 0 );
	itemd2->Add( RdDSnow, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdDWinter = new wxRadioButton( this, -1, wxT("Winter"), wxDefaultPosition, wxDefaultSize, 0 );
	itemd2->Add( RdDWinter, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	chkIceD = new wxCheckBox( this, -1, wxT("Ice"), wxDefaultPosition, wxDefaultSize, 0 );
	itemd2->Add( chkIceD, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	h_sizer->Add( itemd2, 0, wxALIGN_CENTRE | wxALL | 0, 1 );

	wxStaticBox* iteme = new wxStaticBox( this, -1, wxT("Zone E") );
	wxSizer* iteme2 = new wxStaticBoxSizer( iteme, wxVERTICAL );
	RdEClear = new wxRadioButton( this, -1, wxT("Clear"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	iteme2->Add( RdEClear, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdEFrost = new wxRadioButton( this, -1, wxT("Frost"), wxDefaultPosition, wxDefaultSize, 0 );
	iteme2->Add( RdEFrost, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdEMud = new wxRadioButton( this, -1, wxT("Mud"), wxDefaultPosition, wxDefaultSize, 0 );
	iteme2->Add( RdEMud, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdESnow = new wxRadioButton( this, -1, wxT("Snow"), wxDefaultPosition, wxDefaultSize, 0 );
	iteme2->Add( RdESnow, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdEWinter = new wxRadioButton( this, -1, wxT("Winter"), wxDefaultPosition, wxDefaultSize, 0 );
	iteme2->Add( RdEWinter, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	chkIceE = new wxCheckBox( this, -1, wxT("Ice"), wxDefaultPosition, wxDefaultSize, 0 );
	iteme2->Add( chkIceE, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	h_sizer->Add( iteme2, 0, wxALIGN_CENTRE | wxALL | 0, 1 );

	wxStaticBox* itemf = new wxStaticBox( this, -1, wxT("Zone F") );
	wxSizer* itemf2 = new wxStaticBoxSizer( itemf, wxVERTICAL );
	RdFClear = new wxRadioButton( this, -1, wxT("Clear"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	itemf2->Add( RdFClear, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdFFrost = new wxRadioButton( this, -1, wxT("Frost"), wxDefaultPosition, wxDefaultSize, 0 );
	itemf2->Add( RdFFrost, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdFMud = new wxRadioButton( this, -1, wxT("Mud"), wxDefaultPosition, wxDefaultSize, 0 );
	itemf2->Add( RdFMud, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdFSnow = new wxRadioButton( this, -1, wxT("Snow"), wxDefaultPosition, wxDefaultSize, 0 );
	itemf2->Add( RdFSnow, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdFWinter = new wxRadioButton( this, -1, wxT("Winter"), wxDefaultPosition, wxDefaultSize, 0 );
	itemf2->Add( RdFWinter, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	chkIceF = new wxCheckBox( this, -1, wxT("Ice"), wxDefaultPosition, wxDefaultSize, 0 );
	itemf2->Add( chkIceF, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	h_sizer->Add( itemf2, 0, wxALIGN_CENTRE | wxALL | 0, 1 );

	wxStaticBox* itemg = new wxStaticBox( this, -1, wxT("Zone G") );
	wxSizer* itemg2 = new wxStaticBoxSizer( itemg, wxVERTICAL );
	RdGClear = new wxRadioButton( this, -1, wxT("Clear"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	itemg2->Add( RdGClear, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdGFrost = new wxRadioButton( this, -1, wxT("Frost"), wxDefaultPosition, wxDefaultSize, 0 );
	itemg2->Add( RdGFrost, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdGMud = new wxRadioButton( this, -1, wxT("Mud"), wxDefaultPosition, wxDefaultSize, 0 );
	itemg2->Add( RdGMud, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdGSnow = new wxRadioButton( this, -1, wxT("Snow"), wxDefaultPosition, wxDefaultSize, 0 );
	itemg2->Add( RdGSnow, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	RdGWinter = new wxRadioButton( this, -1, wxT("Winter"), wxDefaultPosition, wxDefaultSize, 0 );
	itemg2->Add( RdGWinter, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	chkIceG = new wxCheckBox( this, -1, wxT("Ice"), wxDefaultPosition, wxDefaultSize, 0 );
	itemg2->Add( chkIceG, 0, wxALIGN_LEFT | wxALL | 0, 0 );
	h_sizer->Add( itemg2, 0, wxALIGN_CENTRE | wxALL | 0, 1 );

	//main_v_sizer->Add( h_sizer, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	//wxStaticText* txt = new wxStaticText( this, -1, wxT("Pacific Weather Zones:"), wxDefaultPosition, wxDefaultSize, 0 );
	//main_v_sizer->Add( txt, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
#endif
	wxSizer* ghsz = new wxBoxSizer( wxHORIZONTAL ); // below was 384, 200
	listWZ = new wxListCtrl( this, IDC_LISTWZ, wxDefaultPosition, wxSize( 384, 875 ), wxLC_REPORT | wxSUNKEN_BORDER /* | wxLC_SINGLE_SEL */ );
	ghsz->Add( listWZ, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* gvsz = new wxBoxSizer( wxVERTICAL );
	wxButton* btn1 = new wxButton( this, IDC_BTNCHANGEW, wxT("Change Condition"), wxDefaultPosition, wxSize( 120, 20 ), 0 );
	wxButton* btn2 = new wxButton( this, IDC_BTNCHANGEI, wxT("Change Freezing"), wxDefaultPosition, wxSize( 120, 20 ), 0 );
	gvsz->Add( btn1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	gvsz->Add( btn2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	ghsz->Add( gvsz, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	main_v_sizer->Add( ghsz, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	listWZ->InsertColumn( 0, wxT("Zone") );
	listWZ->InsertColumn( 1, wxT("Condition") );
	listWZ->InsertColumn( 2, wxT("Frozen") );
	listWZ->SetColumnWidth( 0, 80 );
	listWZ->SetColumnWidth( 1, 200 );
	listWZ->SetColumnWidth( 2, 80 );

	for ( int i = 0; i < WeatherZone::COUNT; ++i )
	{
		char s[10];
		sprintf( s, "%s", wz_codes[i] );
		listWZ->InsertItem( i, wxString::FromAscii( s ) );
		listWZ->SetItem( i, 1, _weather_condition_string[_weather_condition[i]] );
		listWZ->SetItem( i, 2, _rivers_lakes_frozen[i] ? wxT("Yes") : wxT("No") );
	}
#if 0
	char s[100];
	for ( int i = 0; i < 25; i++ )
	{
		sprintf( s, "Zone %d", i + 1 );
		listWZ->InsertItem( i, wxString::FromAscii( s ) );
		listWZ->SetItem( i, 1, weathers[Weather[i + 11]] );
		if ( Frozen[i + 11] )
			listWZ->SetItem( i, 2, wxT("Yes") );
		else
			listWZ->SetItem( i, 2, wxT("No") );
	}
#endif
	wxSizer* item23 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item24 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item24->SetDefault( );
	item23->Add( item24, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item25 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item23->Add( item25, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	main_v_sizer->Add( item23, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	SetAutoLayout( true );
	SetSizer( main_v_sizer );
	main_v_sizer->Fit( this );
	main_v_sizer->SetSizeHints( this );

	Centre( );
}

DWeather::~DWeather( )
{
}

void DWeather::CloseDialog( )
{
#if 0
	WeatherDlgData.RdAClear = RdAClear->GetValue( );
	WeatherDlgData.RdAMud = RdAMud->GetValue( );
	WeatherDlgData.RdASnow = RdASnow->GetValue( );
	WeatherDlgData.RdAWinter = RdAWinter->GetValue( );
	WeatherDlgData.RdAFrost = RdAFrost->GetValue( );

	WeatherDlgData.RdBClear = RdBClear->GetValue( );
	WeatherDlgData.RdBMud = RdBMud->GetValue( );
	WeatherDlgData.RdBSnow = RdBSnow->GetValue( );
	WeatherDlgData.RdBWinter = RdBWinter->GetValue( );
	WeatherDlgData.RdBFrost = RdBFrost->GetValue( );

	WeatherDlgData.RdCClear = RdCClear->GetValue( );
	WeatherDlgData.RdCMud = RdCMud->GetValue( );
	WeatherDlgData.RdCSnow = RdCSnow->GetValue( );
	WeatherDlgData.RdCWinter = RdCWinter->GetValue( );
	WeatherDlgData.RdCFrost = RdCFrost->GetValue( );

	WeatherDlgData.RdDClear = RdDClear->GetValue( );
	WeatherDlgData.RdDMud = RdDMud->GetValue( );
	WeatherDlgData.RdDSnow = RdDSnow->GetValue( );
	WeatherDlgData.RdDWinter = RdDWinter->GetValue( );
	WeatherDlgData.RdDFrost = RdDFrost->GetValue( );

	WeatherDlgData.RdEClear = RdEClear->GetValue( );
	WeatherDlgData.RdEMud = RdEMud->GetValue( );
	WeatherDlgData.RdESnow = RdESnow->GetValue( );
	WeatherDlgData.RdEWinter = RdEWinter->GetValue( );
	WeatherDlgData.RdEFrost = RdEFrost->GetValue( );

	WeatherDlgData.RdFClear = RdFClear->GetValue( );
	WeatherDlgData.RdFMud = RdFMud->GetValue( );
	WeatherDlgData.RdFSnow = RdFSnow->GetValue( );
	WeatherDlgData.RdFWinter = RdFWinter->GetValue( );
	WeatherDlgData.RdFFrost = RdFFrost->GetValue( );

	WeatherDlgData.RdGClear = RdGClear->GetValue( );
	WeatherDlgData.RdGMud = RdGMud->GetValue( );
	WeatherDlgData.RdGSnow = RdGSnow->GetValue( );
	WeatherDlgData.RdGWinter = RdGWinter->GetValue( );
	WeatherDlgData.RdGFrost = RdGFrost->GetValue( );

	WeatherDlgData.chkIceA = chkIceA->GetValue( );
	WeatherDlgData.chkIceB = chkIceB->GetValue( );
	WeatherDlgData.chkIceC = chkIceC->GetValue( );
	WeatherDlgData.chkIceD = chkIceD->GetValue( );
	WeatherDlgData.chkIceE = chkIceE->GetValue( );
	WeatherDlgData.chkIceF = chkIceF->GetValue( );
	WeatherDlgData.chkIceG = chkIceG->GetValue( );
#endif
}

void DWeather::EvInitDialog( wxInitDialogEvent& )
{
#if 0
	RdAClear->SetValue( WeatherDlgData.RdAClear );
	RdAMud->SetValue( WeatherDlgData.RdAMud );
	RdASnow->SetValue( WeatherDlgData.RdASnow );
	RdAFrost->SetValue( WeatherDlgData.RdAFrost );
	RdAWinter->SetValue( WeatherDlgData.RdAWinter );

	RdBClear->SetValue( WeatherDlgData.RdBClear );
	RdBMud->SetValue( WeatherDlgData.RdBMud );
	RdBSnow->SetValue( WeatherDlgData.RdBSnow );
	RdBFrost->SetValue( WeatherDlgData.RdBFrost );
	RdBWinter->SetValue( WeatherDlgData.RdBWinter );

	RdCClear->SetValue( WeatherDlgData.RdCClear );
	RdCMud->SetValue( WeatherDlgData.RdCMud );
	RdCSnow->SetValue( WeatherDlgData.RdCSnow );
	RdCFrost->SetValue( WeatherDlgData.RdCFrost );
	RdCWinter->SetValue( WeatherDlgData.RdCWinter );

	RdDClear->SetValue( WeatherDlgData.RdDClear );
	RdDMud->SetValue( WeatherDlgData.RdDMud );
	RdDSnow->SetValue( WeatherDlgData.RdDSnow );
	RdDFrost->SetValue( WeatherDlgData.RdDFrost );
	RdDWinter->SetValue( WeatherDlgData.RdDWinter );

	RdEClear->SetValue( WeatherDlgData.RdEClear );
	RdEMud->SetValue( WeatherDlgData.RdEMud );
	RdESnow->SetValue( WeatherDlgData.RdESnow );
	RdEFrost->SetValue( WeatherDlgData.RdEFrost );
	RdEWinter->SetValue( WeatherDlgData.RdEWinter );

	RdFClear->SetValue( WeatherDlgData.RdFClear );
	RdFMud->SetValue( WeatherDlgData.RdFMud );
	RdFSnow->SetValue( WeatherDlgData.RdFSnow );
	RdFFrost->SetValue( WeatherDlgData.RdFFrost );
	RdFWinter->SetValue( WeatherDlgData.RdFWinter );

	RdGClear->SetValue( WeatherDlgData.RdGClear );
	RdGMud->SetValue( WeatherDlgData.RdGMud );
	RdGSnow->SetValue( WeatherDlgData.RdGSnow );
	RdGFrost->SetValue( WeatherDlgData.RdGFrost );
	RdGWinter->SetValue( WeatherDlgData.RdGWinter );

	chkIceA->SetValue( WeatherDlgData.chkIceA );
	chkIceB->SetValue( WeatherDlgData.chkIceB );
	chkIceC->SetValue( WeatherDlgData.chkIceC );
	chkIceD->SetValue( WeatherDlgData.chkIceD );
	chkIceE->SetValue( WeatherDlgData.chkIceE );
	chkIceF->SetValue( WeatherDlgData.chkIceF );
	chkIceG->SetValue( WeatherDlgData.chkIceG );
#endif
}

void DWeather::ChangeWeatherClick( wxCommandEvent& )
{
	wxString ws = wxGetSingleChoice( wxT("New weather for selected zone(s):"), wxT("HexTools"), 5, _weather_condition_string, this );
	if ( ws.IsEmpty( ) )
		return;

	int i = 0;
	int selection = listWZ->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	while ( selection >= 0 )
	{
#if 0
		int zone = selections + 11;  // because selection is zero based and zones are 1-based
#endif
		for ( i = 0; i < 5; ++i ) // TODO: variable-ize this wx condition count
			if ( strcmp( _weather_condition_string[i].ToAscii( ), ws.ToAscii( ) ) == 0 )
			{
#if 0
				Weather[zone] = i;
#endif
				_weather_condition[selection] = i;
				listWZ->SetItem( selection, 1, _weather_condition_string[i] );
				break;
			}

		selection = listWZ->GetNextItem( selection, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	}
	if ( i == 5 ) // TODO: variable-ize this wx condition count
		wxMessageBox( wxT("For some reason setting of the weather failed!"), wxT("HexTools"), wxOK | wxICON_HAND, this );
}

void DWeather::ChangeIceClick( wxCommandEvent& )
{
	const wxString opts[2] = { wxT("No"), wxT("Yes") };

	wxString ws = wxGetSingleChoice( wxT("Ice on selected zone(s):"), wxT("HexTools"), 2, opts, this );
	if ( ws.IsEmpty( ) )
		return;

	int selection = listWZ->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	while ( selection >= 0 )
	{
		_rivers_lakes_frozen[selection] = strcmp( ws.ToAscii( ), "Yes" ) == 0 ? TRUE : FALSE;
		listWZ->SetItem( selection, 2, strcmp( ws.ToAscii( ), "Yes" ) == 0 ? wxT("Yes") : wxT("No") );
#if 0
		//int zone = selections + 11;     // because choice is 0-based and WZs are 1-based
		if ( strcmp( ws.ToAscii( ), "Yes" ) == 0 )
		{
			//Frozen[zone] = TRUE;
			Frozen[selections] = TRUE;
			listWZ->SetItem( selections, 2, wxT("Yes") );
		}
		else
		{
			//Frozen[zone] = FALSE;
			Frozen[selections] = FALSE;
			listWZ->SetItem( selections, 2, wxT("No") );
		}
#endif
		selection = listWZ->GetNextItem( selection, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	}
}

#endif
