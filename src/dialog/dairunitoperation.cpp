#if defined HEXTOOLSPLAY

#include <wx/wx.h>
#include <wx/statline.h>

#include "dairunitoperation.h"
#include "phase.h"
#include "rules.h"

extern int _current_phase;
extern int _current_player;
extern int _phasing_player;
extern Rules _rule_set;

extern TakeOffDlgXfer _airunit_takeoff_dialog_data;

enum
{
	IDC_RDBOMB = 1,
	IDC_RDCAP,
	IDC_RDESC,
	IDC_RDINT,
	IDC_RDTRANSFER,
	IDC_RDSTAGE,
	IDC_CHKEXTRANGE,
	IDC_CHKNIGHT,
	IDC_CHECKJETTISON,
	IDC_CHECKSHOWALL
};

BEGIN_EVENT_TABLE(DAirUnitOperation, wxDialog)
EVT_INIT_DIALOG(DAirUnitOperation::EvInitDialog)
EVT_CHECKBOX(IDC_CHECKSHOWALL, DAirUnitOperation::ShowAllClicked)
END_EVENT_TABLE()

DAirUnitOperation::DAirUnitOperation( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Takeoff"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("Mission") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
	rdInt = new wxRadioButton( this, IDC_RDINT, wxT("Interception"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdInt, 0, 0, 5 );
	rdEsc = new wxRadioButton( this, IDC_RDESC, wxT("Escort"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdEsc, 0, 0, 5 );
	rdCAP = new wxRadioButton( this, IDC_RDCAP, wxT("Combat Air Patrol"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdCAP, 0, 0, 5 );
	rdBomb = new wxRadioButton( this, IDC_RDBOMB, wxT("Bombing/Transport"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdBomb, 0, 0, 5 );
	rdTranfer = new wxRadioButton( this, IDC_RDTRANSFER, wxT("Transfer"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdTranfer, 0, 0, 5 );
	rdStage = new wxRadioButton( this, IDC_RDSTAGE, wxT("Stage"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( rdStage, 0, 0, 5 );
	chkExtRange = new wxCheckBox( this, IDC_CHKEXTRANGE, wxT("Extended range"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( chkExtRange, 0, 0, 5 );
	chkNight = new wxCheckBox( this, IDC_CHKNIGHT, wxT("Night mission"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( chkNight, 0, wxTOP | 0, 5 );
	chkJettison = new wxCheckBox( this, IDC_CHECKJETTISON, wxT("Fighters: jettison bombs if attacked"),
			wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( chkJettison, 0, wxTOP | 0, 5 );
	wxStaticLine* item12 = new wxStaticLine( this, -1, wxDefaultPosition, wxSize( 150, -1 ), wxLI_HORIZONTAL );
	item1->Add( item12, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	chkShowAll = new wxCheckBox( this, IDC_CHECKSHOWALL, wxT("Show all missions"), wxDefaultPosition, wxDefaultSize,
			0 );
	item1->Add( chkShowAll, 0, wxTOP | wxBOTTOM | 0, 5 );
	item0->Add( item1, 0, wxGROW | wxALL | 0, 5 );
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
	show_all_ = FALSE;
}

DAirUnitOperation::~DAirUnitOperation( )
{
}

void DAirUnitOperation::EnableButtons( )
{
	int ena;

	if ( show_all_ )
	{
		rdBomb->Enable( true );  // har
		rdCAP->Enable( true );
		rdEsc->Enable( true );
		rdInt->Enable( true );   // to har
		rdTranfer->Enable( true );
		rdStage->Enable( true );
		return;
	}
	// else:
	switch ( _current_phase )
	{
		case Phase::INITIAL_PHASE:
			rdBomb->Enable( true );  // har
			rdCAP->Enable( true );
			rdEsc->Enable( false );
			rdInt->Enable( true );   // to har
			rdTranfer->Enable( false );
			rdStage->Enable( false );
			break;
		case Phase::EXPLOITATION_PHASE:
		case Phase::MOVEMENT_PHASE:
			if ( _rule_set.OnDemandAirMissions == FALSE && _current_player != _phasing_player )
				ena = FALSE;
			else
				ena = TRUE;
			rdBomb->Enable( ena );
			rdCAP->Enable( false );
			rdEsc->Enable( ena );
			rdInt->Enable( _rule_set.OnDemandAirMissions == TRUE );
			rdTranfer->Enable( true );
			rdStage->Enable( ena );
			break;
		case Phase::REACTION_PHASE:       // must be old rules
			if ( _current_player == _phasing_player )
			{
				rdInt->Enable( false );
				rdBomb->Enable( false );
				rdCAP->Enable( false );
				rdEsc->Enable( false );
				rdTranfer->Enable( false );
				rdStage->Enable( false );
			}
			else
			{
				rdInt->Enable( true );
				rdBomb->Enable( false );
				rdCAP->Enable( false );
				rdEsc->Enable( false );
				rdTranfer->Enable( false );
				rdStage->Enable( false );
			}
			break;
		case Phase::COMBAT_PHASE:
			if ( _rule_set.OnDemandAirMissions == TRUE )
			{
				rdInt->Enable( true );
				rdBomb->Enable( true );
				rdCAP->Enable( false );
				rdEsc->Enable( true );
				rdTranfer->Enable( false );
				rdStage->Enable( true );
			}
			else
			{
				rdInt->Enable( false );
				rdBomb->Enable( false );
				rdCAP->Enable( false );
				rdEsc->Enable( false );
				rdTranfer->Enable( false );
				rdStage->Enable( false );
			}
			break;
	}
}

void DAirUnitOperation::SetupDialog( )
{
	EnableButtons( );
	chkJettison->SetValue( false );
}

void DAirUnitOperation::EvInitDialog( wxInitDialogEvent& )
{
	EnableButtons( );
}

void DAirUnitOperation::CloseDialog( )
{
	_airunit_takeoff_dialog_data.rdBomb = rdBomb->GetValue( );
	_airunit_takeoff_dialog_data.rdCAP = rdCAP->GetValue( );
	_airunit_takeoff_dialog_data.rdEsc = rdEsc->GetValue( );
	_airunit_takeoff_dialog_data.rdInt = rdInt->GetValue( );
	_airunit_takeoff_dialog_data.rdTranfer = rdTranfer->GetValue( );
	_airunit_takeoff_dialog_data.rdStage = rdStage->GetValue( );
	_airunit_takeoff_dialog_data.chkExtRange = chkExtRange->GetValue( );
	_airunit_takeoff_dialog_data.chkNight = chkNight->GetValue( );
	_airunit_takeoff_dialog_data.chkJettison = chkJettison->GetValue( );
}

void DAirUnitOperation::ShowAllClicked( wxCommandEvent& )
{
	show_all_ = chkShowAll->GetValue( );
	EnableButtons( );
}

#endif
