#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "counter.h"
#include "dstatusmarker.h"
#include "statusmarker.h"

extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)

extern PtsDlgXfer _points_dialog_data;

enum
{
	IDC_EDITTXT = 1,
	IDC_EDITPNTS,
	IDC_RDAA,
	IDC_RDATT,
	IDC_RDDEF,
	IDC_RDNONE,
	IDC_RDCD,
	IDC_RDDIE
};

BEGIN_EVENT_TABLE(DStatusMarker, wxDialog)
EVT_RADIOBUTTON(IDC_RDAA, DStatusMarker::rdAAClicked)
EVT_RADIOBUTTON(IDC_RDATT, DStatusMarker::rdAttClicked)
EVT_RADIOBUTTON(IDC_RDDEF, DStatusMarker::rdDefClicked)
EVT_RADIOBUTTON(IDC_RDNONE, DStatusMarker::rdNoneClicked)
EVT_RADIOBUTTON(IDC_RDCD, DStatusMarker::CDClicked)
EVT_RADIOBUTTON(IDC_RDDIE, DStatusMarker::DieClicked)
EVT_INIT_DIALOG(DStatusMarker::EvInitDialog)
END_EVENT_TABLE()

DStatusMarker::DStatusMarker( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Status Marker Properties"), wxDefaultPosition )
{
	type_ = StatusMarker::STATUS;

	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

	wxSizer* item3 = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText* item4 = new wxStaticText( this, -1, wxT("Caption:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editTxt = new wxTextCtrl( this, IDC_EDITTXT, wxT(""), wxDefaultPosition, wxSize( 140, -1 ), 0 );
	item3->Add( editTxt, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxStaticText* item6 = new wxStaticText( this, -1, wxT("Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item6, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	editPoints = new wxTextCtrl( this, IDC_EDITPNTS, wxT(""), wxDefaultPosition, wxSize( 50, -1 ), 0 );
	item3->Add( editPoints, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );

	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	rdNone = new wxRadioButton( this, IDC_RDNONE, wxT("Ignore for all calculations."), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdNone, 0, wxLEFT | wxRIGHT | 0, 5 );

	rdAtt = new wxRadioButton( this, IDC_RDATT, wxT("Add to attack strength."), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdAtt, 0, wxLEFT | wxRIGHT | 0, 5 );

	rdDef = new wxRadioButton( this, IDC_RDDEF, wxT("Add to defense strength."), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdDef, 0, wxLEFT | wxRIGHT | 0, 5 );

	rdAA = new wxRadioButton( this, IDC_RDAA, wxT("Add to AA strength."), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdAA, 0, wxLEFT | wxRIGHT | 0, 5 );

	rdCD = new wxRadioButton( this, IDC_RDCD, wxT("Add to coastal defense strength."), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdCD, 0, wxLEFT | wxRIGHT | 0, 5 );

	rdDie = new wxRadioButton( this, IDC_RDDIE, wxT("Add to combat die roll modifier."), wxDefaultPosition,wxDefaultSize, 0 );
	item1->Add( rdDie, 0, wxLEFT | wxRIGHT | 0, 5 );

	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* item14 = new wxBoxSizer( wxHORIZONTAL );

	wxButton* item15 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item14->Add( item15, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxButton* item16 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item14->Add( item16, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	item0->Add( item14, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	Center( );
}

DStatusMarker::~DStatusMarker( )
{
	_unit_editor_active = false;
}

void DStatusMarker::SetType( int t )
{
	type_ = t;
}

void DStatusMarker::rdAAClicked( wxCommandEvent& )
{
	type_ = StatusMarker::AA;
}

void DStatusMarker::rdAttClicked( wxCommandEvent& )
{
	type_ = StatusMarker::ATTACK;
}

void DStatusMarker::rdDefClicked( wxCommandEvent& )
{
	type_ = StatusMarker::DEFENSE;
}

void DStatusMarker::rdNoneClicked( wxCommandEvent& )
{
	type_ = StatusMarker::STATUS;
}

void DStatusMarker::CDClicked( wxCommandEvent& )
{
	type_ = StatusMarker::CD;
}

void DStatusMarker::DieClicked( wxCommandEvent& )
{
	type_ = StatusMarker::DIE;
}

void DStatusMarker::EvInitDialog( wxInitDialogEvent& )
{
	rdNone->SetValue( false );
	rdAtt->SetValue( false );
	rdDef->SetValue( false );
	rdAA->SetValue( false );
	rdCD->SetValue( false );
	rdDie->SetValue( false );
	switch ( type_ )
	{
		case StatusMarker::STATUS:
			rdNone->SetValue( true );
			break;
		case StatusMarker::ATTACK:
			rdAtt->SetValue( true );
			break;
		case StatusMarker::DEFENSE:
			rdDef->SetValue( true );
			break;
		case StatusMarker::AA:
			rdAA->SetValue( true );
			break;
		case StatusMarker::CD:
			rdCD->SetValue( true );
			break;
		case StatusMarker::DIE:
			rdDie->SetValue( true );
			break;
	}
	editTxt->SetValue( wxString::FromAscii( _points_dialog_data.editTxt ) );
	editPoints->SetValue( wxString::FromAscii( _points_dialog_data.editPoints ) );
	_unit_editor_active = true;
}

void DStatusMarker::CloseDialog( )
{
	strcpy( _points_dialog_data.editTxt, editTxt->GetValue( ).ToAscii( ) );
	strcpy( _points_dialog_data.editPoints, editPoints->GetValue( ).ToAscii( ) );
}

#endif
