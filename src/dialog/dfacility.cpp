#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dfacility.h"

extern MiscDlgXfer _misc_dialog_data;

enum
{
	IDC_RDFORT = 1,
	IDC_RDPERMAF,
	IDC_RDTEMPAF,
	IDC_RDPERMAF12,
	IDC_RDPERMAF6,
	IDC_RDPERMAF9,
	IDC_RDIMPFORT,
	IDC_RDFIELDWORKS,
	IDC_RDENTR,
	IDC_RDZEPBASE
};

BEGIN_EVENT_TABLE(DFacility, wxDialog)
EVT_INIT_DIALOG(DFacility::EvInitDialog)
END_EVENT_TABLE()

DFacility::DFacility( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Facility Properties"), wxDefaultPosition )
{
//#if 0 TEST THIS, why was it compile-conditionalized out?
	rdTempAF = new wxRadioButton( this, IDC_RDTEMPAF, "Temporary airfield", wxDefaultPosition, wxDefaultSize, 0 );
//#endif
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
	wxSizer* item3 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item4 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxSize( 20, 20 ) );
	item3->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxStaticText* item5 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxSize( 20, 20 ) );
	item3->Add( item5, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxStaticText* item6 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxSize( 20, 20 ) );
	item3->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item1->Add( item3, 0, wxALL | 0, 5 );
	rdFort = new wxRadioButton( this, IDC_RDFORT, wxT("Fort"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdFort, 0, wxLEFT | wxRIGHT | 0, 15 );

	rdImpFort = new wxRadioButton( this, IDC_RDIMPFORT, wxT("Improved fort"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdImpFort, 0, wxLEFT | wxRIGHT | 0, 15 );
	rdFieldWorks = new wxRadioButton( this, IDC_RDFIELDWORKS, wxT("Fieldworks"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdFieldWorks, 0, wxLEFT | wxRIGHT | 0, 15 );
	rdEntr = new wxRadioButton( this, IDC_RDENTR, wxT("Entrenchment"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdEntr, 0, wxLEFT | wxRIGHT | 0, 15 );

	rdPermAF = new wxRadioButton( this, IDC_RDPERMAF, wxT("Permanent airfield, 3 capacity"), wxDefaultPosition,
			wxDefaultSize, 0 );
	item1->Add( rdPermAF, 0, wxLEFT | wxRIGHT | 0, 15 );
	rdPermAF6 = new wxRadioButton( this, IDC_RDPERMAF6, wxT("Perm AF 6"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdPermAF6, 0, wxLEFT | wxRIGHT | 0, 15 );
	rdPermAF9 = new wxRadioButton( this, IDC_RDPERMAF9, wxT("Perm AF 9"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdPermAF9, 0, wxLEFT | wxRIGHT | 0, 15 );
	rmPermAF12 = new wxRadioButton( this, IDC_RDPERMAF12, wxT("Perm AF 12"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rmPermAF12, 0, wxLEFT | wxRIGHT | 0, 15 );
	rdZepBase = new wxRadioButton( this, IDC_RDZEPBASE, wxT("Zeppelin base"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdZepBase, 0, wxLEFT | wxRIGHT | 0, 15 );

	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item12 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item13 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item13, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item14 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item14, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item12, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( TRUE );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );
}

DFacility::~DFacility( )
{
}

void DFacility::CloseDialog( )
{
	_misc_dialog_data.rdFort = rdFort->GetValue( );
	_misc_dialog_data.rdPermAF = rdPermAF->GetValue( );
	_misc_dialog_data.rdPermAF6 = rdPermAF6->GetValue( );
	_misc_dialog_data.rdPermAF9 = rdPermAF9->GetValue( );
	_misc_dialog_data.rdPermAF12 = rmPermAF12->GetValue( );
	_misc_dialog_data.rdFieldWorks = rdFieldWorks->GetValue( );
	_misc_dialog_data.rdEntr = rdEntr->GetValue( );
	_misc_dialog_data.rdImpFort = rdImpFort->GetValue( );
	_misc_dialog_data.rdZepBase = rdZepBase->GetValue( );
#if 0
	_misc_dialog_data.rdTempAF = rdTempAF->GetValue( );
#endif
}

void DFacility::EvInitDialog( wxInitDialogEvent& )
{
	rdFort->SetValue( _misc_dialog_data.rdFort );
	rdPermAF->SetValue( _misc_dialog_data.rdPermAF );
	rdPermAF6->SetValue( _misc_dialog_data.rdPermAF6 );
	rdPermAF9->SetValue( _misc_dialog_data.rdPermAF9 );
	rmPermAF12->SetValue( _misc_dialog_data.rdPermAF12 );
	rdFieldWorks->SetValue( _misc_dialog_data.rdFieldWorks );
	rdEntr->SetValue( _misc_dialog_data.rdEntr );
	rdImpFort->SetValue( _misc_dialog_data.rdImpFort );
	rdZepBase->SetValue( _misc_dialog_data.rdZepBase );
#if 0
	rdTempAF->SetValue( _misc_dialog_data.rdTempAF );
#endif
}

#endif
