#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "daafire.h"
#include "application.h"

static int manu = 0;

extern AADlgXfer _aa_dialog_data;

enum
{
	IDC_EDITSTR = 1,
	IDC_EDITDIE,
	IDC_EDITMOD,
	IDC_LBLDIE,
	IDC_LBLTARGET,
	IDC_RDCOMP,
	IDC_RDMANU,
	IDC_LDLDIE_2,
	IDC_EDITDIE_2
};

BEGIN_EVENT_TABLE(DAAFire, wxDialog)
EVT_INIT_DIALOG(DAAFire::EvInitDialog)
EVT_RADIOBUTTON(IDC_RDCOMP, DAAFire::CompClicked)
EVT_RADIOBUTTON(IDC_RDMANU, DAAFire::ManuClicked)
END_EVENT_TABLE()

DAAFire::DAAFire( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Fire AA"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
	wxStaticBox* item4 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item3 = new wxStaticBoxSizer( item4, wxHORIZONTAL );
	lblTarget = new wxStaticText( this, IDC_LBLTARGET, wxT("text\nline 2"), wxDefaultPosition, wxSize( 380, 30 ), 0 );
	item3->Add( lblTarget, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item6 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item7 = new wxStaticText( this, -1, wxT(" "), wxDefaultPosition, wxDefaultSize );
	item6->Add( item7, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item8 = new wxStaticText( this, -1, _("AA strength"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item8, 0, wxALIGN_CENTRE | wxLEFT | 0, 25 );
	editStr = new wxTextCtrl( this, IDC_EDITSTR, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item6->Add( editStr, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item10 = new wxStaticText( this, -1, wxT("Die modifier"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item10, 0, wxALIGN_CENTRE | wxLEFT | 0, 25 );
	editMod = new wxTextCtrl( this, IDC_EDITMOD, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item6->Add( editMod, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	item1->Add( item6, 0, wxALL | 0, 5 );
	wxSizer* item12 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item13 = new wxStaticText( this, -1, wxT(" "), wxDefaultPosition, wxDefaultSize );
	item12->Add( item13, 0, wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );
	wxSizer* item14 = new wxBoxSizer( wxVERTICAL );
	rdComp = new wxRadioButton( this, IDC_RDCOMP, wxT("Computer die roll"), wxDefaultPosition, wxDefaultSize, 0 );
	item14->Add( rdComp, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdManu = new wxRadioButton( this, IDC_RDMANU, wxT("Manual die roll"), wxDefaultPosition, wxDefaultSize, 0 );
	item14->Add( rdManu, 0, wxLEFT | wxRIGHT | 0, 5 );
	wxSizer* item17 = new wxFlexGridSizer( 2, 0, 0 );
	lblDie = new wxStaticText( this, IDC_LBLDIE, wxT("1st die roll"), wxDefaultPosition, wxDefaultSize, 0 );
	item17->Add( lblDie, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	editDie = new wxTextCtrl( this, IDC_EDITDIE, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item17->Add( editDie, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	lblDie2 = new wxStaticText( this, IDC_LDLDIE_2, wxT("2nd die roll"), wxDefaultPosition, wxDefaultSize, 0 );
	item17->Add( lblDie2, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	editDie2 = new wxTextCtrl( this, IDC_EDITDIE_2, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item17->Add( editDie2, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	item14->Add( item17, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	item12->Add( item14, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxStaticText* item22 = new wxStaticText(
			this,
			-1,
			wxT( "NOTE: " + Application::NAME + " does NOT include AA strength\nof any naval units to precalculated strength.\nYou must add them manually (just edit the\n'AA strength' field)." ),
			wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item22, 0, wxALL | 0, 5 );
	item1->Add( item12, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item23 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item24 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item23->Add( item24, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item25 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item23->Add( item25, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item23, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );
}

DAAFire::~DAAFire( )
{
}

void DAAFire::EvInitDialog( wxInitDialogEvent& )
{
	editStr->SetValue( wxString::FromAscii( _aa_dialog_data.editStr ) );
	editDie->SetValue( wxString::FromAscii( _aa_dialog_data.editDie ) );
	editDie2->SetValue( wxString::FromAscii( _aa_dialog_data.editDie2 ) );
	editMod->SetValue( wxString::FromAscii( _aa_dialog_data.editMod ) );
	lblTarget->SetLabel( wxString::FromAscii( _aa_dialog_data.lblTarget ) );

	if ( manu )
	{
		rdManu->SetValue( true );
		rdComp->SetValue( false );
	}
	else
	{
		rdManu->SetValue( false );
		rdComp->SetValue( true );
	}
	SetDieEna( );
}

void DAAFire::CloseDialog( )
{
	strncpy( _aa_dialog_data.editStr, editStr->GetValue( ).ToAscii( ), 255 );
	strncpy( _aa_dialog_data.editDie, editDie->GetValue( ).ToAscii( ), 255 );
	strncpy( _aa_dialog_data.editDie2, editDie2->GetValue( ).ToAscii( ), 255 );
	strncpy( _aa_dialog_data.editMod, editMod->GetValue( ).ToAscii( ), 255 );
#if 0
	lblTarget->GetValue( AADlgData.lblTarget, 255 );
#endif
	_aa_dialog_data.rdComp = rdComp->GetValue( );
	_aa_dialog_data.rdManu = rdManu->GetValue( );
}

void DAAFire::SetDieEna( )
{
	if ( rdManu->GetValue( ) )
	{
		lblDie->Enable( );
		lblDie2->Enable( );
		editDie->Enable( );
		editDie2->Enable( );
	}
	else
	{
		lblDie->Enable( false );
		lblDie2->Enable( false );
		editDie->Enable( false );
		editDie2->Enable( false );
	}
}

void DAAFire::CompClicked( wxCommandEvent& )
{
	SetDieEna( );
	manu = 0;
}

void DAAFire::ManuClicked( wxCommandEvent& )
{
	SetDieEna( );
	manu = 1;
}

#endif
