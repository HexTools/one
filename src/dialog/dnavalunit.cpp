#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "sideplayer.h"
#include "counter.h"
#include "hextools.h"
using ht::wxS;
#include "dnavalunit.h"
#include "navalunitsubtype.h"
#include "navalunittype.h"
#include "navalunit.h"

extern int _unit_editor_active;
extern char* _fleet_type_name[];
extern NavalArmedForce _naval_armed_force[2][NavalArmedForce::Allied::COUNT];
extern int _current_player;

extern FleetDlgXfer _fleet_dialog_data;

BEGIN_EVENT_TABLE(DNavalUnit, wxDialog)
EVT_INIT_DIALOG(DNavalUnit::EvInitDialog)
END_EVENT_TABLE()

enum
{
	IDC_CMBNAT = 1,
	IDC_CMBTYPE,
	IDC_EDITCURMP,
	IDC_EDIT_MP,
	IDC_EDIT_STR,
	IDC_EDIT_HITS,
	IDC_EDIT_AA,
	IDC_EDITNAME
};

DNavalUnit::DNavalUnit( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Fleet Properties"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
	wxSizer* item3 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item4 = new wxFlexGridSizer( 2, 0, 0 );
	wxStaticText* item5 = new wxStaticText( this, -1, wxT("Nationality"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item5, 0, wxALL | 0, 5 );
	wxString strs6[] =
	{ wxT("c1"), wxT("c1"), wxT("c1"), wxT("c1"), wxT("c1"), wxT("c1"), wxT("c1"), wxT("c1") };
	cmbNat = new wxComboBox( this, IDC_CMBNAT, wxT(""), wxDefaultPosition, wxSize( 150, -1 ), 8, strs6, wxCB_READONLY );
	item4->Add( cmbNat, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxStaticText* item7 = new wxStaticText( this, -1, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item7, 0, wxALL | 0, 5 );
	cmbType = new wxComboBox( this, IDC_CMBTYPE, wxT(""), wxDefaultPosition, wxSize( 150, -1 ), 8, strs6,
			wxCB_READONLY );
	item4->Add( cmbType, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxStaticText* item9 = new wxStaticText( this, -1, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item9, 0, wxALL | 0, 5 );
	editName = new wxTextCtrl( this, IDC_EDITNAME, wxT(""), wxDefaultPosition, wxSize( 150, -1 ), 0 );
	item4->Add( editName, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item3->Add( item4, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item11 = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* item12 = new wxFlexGridSizer( 2, 0, 0 );
	wxStaticText* item13 = new wxStaticText( this, -1, wxT("Strength"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item13, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	editStr = new wxTextCtrl( this, IDC_EDIT_STR, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item12->Add( editStr, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxStaticText* item15 = new wxStaticText( this, -1, wxT("Max MP"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item15, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	editMP = new wxTextCtrl( this, IDC_EDIT_MP, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item12->Add( editMP, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxStaticText* item17 = new wxStaticText( this, -1, wxT("AA"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item17, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editAA = new wxTextCtrl( this, IDC_EDIT_AA, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item12->Add( editAA, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item11->Add( item12, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxSizer* item19 = new wxFlexGridSizer( 2, 0, 0 );
	wxStaticText* item20 = new wxStaticText( this, -1, wxT("Hits"), wxDefaultPosition, wxDefaultSize, 0 );
	item19->Add( item20, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	editHits = new wxTextCtrl( this, IDC_EDIT_HITS, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item19->Add( editHits, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxStaticText* item22 = new wxStaticText( this, -1, wxT("MP's left"), wxDefaultPosition, wxDefaultSize, 0 );
	item19->Add( item22, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editCurMP = new wxTextCtrl( this, IDC_EDITCURMP, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item19->Add( editCurMP, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item11->Add( item19, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	item1->Add( item11, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item24 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item25 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item24->Add( item25, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item26 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item24->Add( item26, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item24, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	type_ = 0;
	subtype_ = 0;
}

DNavalUnit::~DNavalUnit( )
{
	_unit_editor_active = false;
}

void DNavalUnit::CloseDialog( )
{
	type_ = cmbType->GetSelection( );
	subtype_ = cmbNat->GetSelection( );
	strcpy( _fleet_dialog_data.editName, editName->GetValue( ).ToAscii( ) );
	strcpy( _fleet_dialog_data.editCurMP, editCurMP->GetValue( ).ToAscii( ) );
	strcpy( _fleet_dialog_data.editMP, editMP->GetValue( ).ToAscii( ) );
	strcpy( _fleet_dialog_data.editStr, editStr->GetValue( ).ToAscii( ) );
	strcpy( _fleet_dialog_data.editHits, editHits->GetValue( ).ToAscii( ) );
	strcpy( _fleet_dialog_data.editAA, editAA->GetValue( ).ToAscii( ) );
}

void DNavalUnit::EvInitDialog( wxInitDialogEvent& )
{
	cmbNat->Clear( );
	if ( _current_player == SidePlayer::AXIS )
		for ( int i = 0; i < NavalArmedForce::Axis::COUNT; ++i )
			cmbNat->Append( wxS( _naval_armed_force[SidePlayer::AXIS][i].name_ ) );
	else
		for ( int i = 0; i < NavalArmedForce::Allied::COUNT; ++i )
			cmbNat->Append( wxS( _naval_armed_force[SidePlayer::ALLIED][i].name_ ) );
	cmbNat->SetSelection( subtype_ );

	cmbType->Clear( );
	for ( int i = 0; i < NavalUnitType::Fleet::COUNT; ++i )
		cmbType->Append( wxS( _fleet_type_name[i] ) );
	cmbType->SetSelection( type_ );

	editName->SetValue( wxS( _fleet_dialog_data.editName ) );
	editCurMP->SetValue( wxS( _fleet_dialog_data.editCurMP ) );
	editMP->SetValue( wxS( _fleet_dialog_data.editMP ) );
	editStr->SetValue( wxS( _fleet_dialog_data.editStr ) );
	editHits->SetValue( wxS( _fleet_dialog_data.editHits ) );
	editAA->SetValue( wxS( _fleet_dialog_data.editAA ) );
	_unit_editor_active = true;
}

#endif
