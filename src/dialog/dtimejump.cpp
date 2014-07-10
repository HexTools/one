#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dtimejump.h"
#include "phase.h"

enum
{
	IDC_CMBMONTH,
	IDC_CMBTURN,
	IDC_CMBYEAR,
	IDC_CMBPHASE,
	IDC_CMBPLAYER
};

BEGIN_EVENT_TABLE(DTimeJump, wxDialog)
EVT_INIT_DIALOG(DTimeJump::EvInitDialog)
END_EVENT_TABLE()

extern const char* _phase_string[];
extern int _current_phase;
extern int _phasing_player;
extern int _month;
extern int _turn;
extern int _year;
extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)

extern TurnDlgXfer _turn_dialog_data;

DTimeJump::DTimeJump( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Time Jump"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("Turn") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxHORIZONTAL );
	wxSizer* item3 = new wxFlexGridSizer( 2, 0, 0 );
	wxStaticText* item4 = new wxStaticText( this, -1, wxT("Year"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item4, 0, wxALL | 0, 5 );
	wxString strs5[] =
	{ wxT("14"), wxT("15"), wxT("16"), wxT("17"), wxT("18"), wxT("19"), wxT("20"), wxT("36"), wxT("37"), wxT("38"),
		wxT("39"), wxT("40"), wxT("41"), wxT("42"), wxT("43"), wxT("44"), wxT("45"), wxT("46") };
	cmbYear = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 15, strs5, 0 );
	item3->Add( cmbYear, 0, wxALL | 0, 5 );
	wxStaticText* item6 = new wxStaticText( this, -1, wxT("Month"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item6, 0, wxALL | 0, 5 );
	wxString strs7[] =
	{ wxT("Jan"), wxT("Feb"), wxT("Mar"), wxT("Apr"), wxT("May"), wxT("Jun"), wxT("Jul"), wxT("Aug"), wxT("Sep"),
		wxT("Oct"), wxT("Nov"), wxT("Dec") };
	cmbMonth = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 12, strs7, wxCB_READONLY );
	item3->Add( cmbMonth, 0, wxALL | 0, 5 );
	wxStaticText* item8 = new wxStaticText( this, -1, wxT("Turn"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item8, 0, wxALL | 0, 5 );
	wxString strs9[] =
	{ wxT("I"), wxT("II"), wxT("III"), wxT("IV") };
	cmbTurn = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 4, strs9, wxCB_READONLY );
	item3->Add( cmbTurn, 0, wxALL | 0, 5 );
	wxStaticText* item10 = new wxStaticText( this, -1, wxT("Player"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item10, 0, wxALL | 0, 5 );
	wxString strs11[] =
	{ wxT("Axis"), wxT("Allied") };
	cmbPlayer = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 2, strs11, wxCB_READONLY );
	item3->Add( cmbPlayer, 0, wxALL | 0, 5 );
	wxStaticText* item12 = new wxStaticText( this, -1, wxT("Phase"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item12, 0, wxALL | 0, 5 );
	wxString strs13[] =
	{ wxT("Initial"), wxT("Movement"), wxT("Combat"), wxT("Exploitation") };
	cmbPhase = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxSize( 100, -1 ), 4, strs13, wxCB_READONLY );
	item3->Add( cmbPhase, 0, wxALL | 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
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

DTimeJump::~DTimeJump( )
{
	_unit_editor_active = false;
}

void DTimeJump::EvInitDialog( wxInitDialogEvent& )
{
#if 0
	for ( i = 38; i <= 46; ++i )
		cmbYear->AddString( static_cast<LPSTR>( itoa( i, tmp, 10 ) ) );
	for ( i = 0; i < 12; ++i )
		cmbMonth->AddString( static_cast<LPSTR>( months[i] ) );
	for ( i = 0; i < Turns; ++i )
		cmbTurn->AddString( turns[i] );
	cmbYear->SetSelection( Year - 38 );
#endif
	wxString tmp;
	tmp.Printf( wxT("%d"), _year );
	cmbYear->SetValue( tmp );
#if 0
	cmbYear->SetSelIndex( 2 );
#endif
	cmbMonth->SetSelection( _month );
	cmbTurn->SetSelection( _turn - 1 );
#if 0
	cmbPlayer->ClearList( );
	cmbPlayer->InsertString( "Axis" ), 0 );
	cmbPlayer->InsertString( "Allied" ), 1 );
#endif
	cmbPlayer->SetSelection( _phasing_player );

	cmbPhase->Clear( );
	for ( int i = 2; i <= Phase::LASTPHASE; ++i )  // skip no-game & setup
		cmbPhase->Append( wxString::FromAscii( _phase_string[i] ) );

	cmbPhase->SetSelection( _current_phase < 2 ? 0 : _current_phase - 2 );
	_unit_editor_active = true;
}

void DTimeJump::CloseDialog( )
{
	_turn_dialog_data.cmbYear = strtol( cmbYear->GetValue( ).ToAscii( ), 0, 0 ) % 100;
	_turn_dialog_data.cmbMonth = cmbMonth->GetSelection( );
	_turn_dialog_data.cmbTurn = cmbTurn->GetSelection( ) + 1;
	_turn_dialog_data.cmbPlayer = cmbPlayer->GetSelection( );
	_turn_dialog_data.cmbPhase = cmbPhase->GetSelection( ) + 2;
}

#endif
