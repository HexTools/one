#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "sideplayer.h"
#include "counter.h"
#include "dnavalunitship.h"
#include "navalunitsubtype.h"
#include "navalunittype.h"
#include "navalunit.h"

extern int _unit_editor_active;
extern char* _ship_type_string[];
extern NavalArmedForce _naval_armed_force[][NavalArmedForce::Allied::COUNT];
extern int _current_player;

extern ShipDlgXfer _ship_dialog_data;

BEGIN_EVENT_TABLE(DNavalUnitShip, wxDialog)
EVT_INIT_DIALOG(DNavalUnitShip::EvInitDialog)
END_EVENT_TABLE()

enum {
  IDC_EDITAA = 1,
  IDC_EDITNAME,
  IDC_EDITPRI,
  IDC_EDITSEC,
  IDC_EDITTHI,
  IDC_COMBOCLASS,
  IDC_EDITMAXHITS,
  IDC_EDITHITS,
  IDC_CMB_NAT,
  IDC_CHKRADAR,
  IDC_EDITPROT,
  IDC_EDITRELOADS,
  IDC_EDITSPEED,
  IDC_EDITTORP
};

DNavalUnitShip::DNavalUnitShip (wxWindow* parent)
: wxDialog(parent, -1, wxT("Ship Properties"),wxDefaultPosition)
{

    wxSizer* item0 = new wxBoxSizer(wxVERTICAL);
    wxSizer* item1 = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* item2 = new wxBoxSizer(wxVERTICAL);

    wxString strs8[] =
    {
        wxT("C"),
        wxT("C"),
        wxT("C"),
        wxT("C"),
        wxT("C"),
        wxT("C"),
        wxT("C"),
        wxT("C")
    };

    cmbNat = new wxComboBox(this, IDC_CMB_NAT, wxT(""), wxDefaultPosition, wxSize(200,-1), 8, strs8, wxCB_READONLY);
    item2->Add(cmbNat, 0, wxALIGN_CENTRE|wxALL|0, 2);

    wxSizer* item4 = new wxFlexGridSizer(2, 0, 0);
    wxStaticText* item5 = new wxStaticText(this, -1, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0);
    item4->Add(item5, 0, wxALIGN_CENTRE|wxALL|0, 2);
    editName = new wxTextCtrl(this, IDC_EDITNAME, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
    item4->Add(editName, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item7 = new wxStaticText(this, -1, wxT("Class"), wxDefaultPosition, wxDefaultSize, 0);
    item4->Add(item7, 0, wxALIGN_CENTRE|wxALL|0, 2);
    cmbClass = new wxComboBox(this, IDC_COMBOCLASS, wxT(""), wxDefaultPosition, wxSize(150,-1), 8, strs8, wxCB_READONLY);
    item4->Add(cmbClass, 0, wxALIGN_CENTRE|wxALL|0, 2);
    item2->Add(item4, 0, wxALIGN_CENTRE|wxALL|0, 2);
    item1->Add(item2, 0, wxALIGN_CENTRE|wxALL|0, 2);

    wxStaticBox* item10 = new wxStaticBox(this, -1, wxT("Hits"));
    wxSizer* item9 = new wxStaticBoxSizer(item10, wxVERTICAL);
    wxSizer* item11 = new wxFlexGridSizer(2, 0, 0);
    wxStaticText* item12 = new wxStaticText(this, -1, wxT("Max hits"), wxDefaultPosition, wxDefaultSize, 0);
    item11->Add(item12, 0, wxALIGN_CENTRE|wxALL|0, 2);
    editMaxHits = new wxTextCtrl(this, IDC_EDITMAXHITS, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item11->Add(editMaxHits, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item14 = new wxStaticText(this, -1, wxT("Hits left"), wxDefaultPosition, wxDefaultSize, 0);
    item11->Add(item14, 0, wxALIGN_CENTRE|wxALL|0, 2);
    editHits = new wxTextCtrl(this, IDC_EDITHITS, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item11->Add(editHits, 0, wxALIGN_CENTRE|wxALL|0, 2);
    item9->Add(item11, 0, wxALIGN_CENTRE|wxALL|0, 2);
    item1->Add(item9, 0, wxALIGN_CENTRE|wxALL|0, 2);
    item0->Add(item1, 0, wxALIGN_CENTRE|wxALL|0, 2);

    wxSizer* item16 = new wxBoxSizer(wxHORIZONTAL);
#if 0
	wxStaticBox* itemb1 = new wxStaticBox( this, -1, "Gunnery strengths" );
#endif
    wxSizer* item17 = new wxFlexGridSizer(2, 0, 0);
    wxStaticText* item18 = new wxStaticText(this, -1, wxT("Primary (2 hexes)"), wxDefaultPosition, wxDefaultSize, 0);
    item17->Add(item18, 0, wxALL|0, 2);
    editPri = new wxTextCtrl(this, IDC_EDITPRI, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item17->Add(editPri, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item20 = new wxStaticText(this, -1, wxT("Secondary (1 hex)"), wxDefaultPosition, wxDefaultSize, 0);
    item17->Add(item20, 0, wxALL|0, 2);
    editSec = new wxTextCtrl(this, IDC_EDITSEC, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item17->Add(editSec, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item22 = new wxStaticText(this, -1, wxT("Third (1 hex)"), wxDefaultPosition, wxDefaultSize, 0);
    item17->Add(item22, 0, wxALL|0, 2);
    editThi = new wxTextCtrl(this, IDC_EDITTHI, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item17->Add(editThi, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item24 = new wxStaticText(this, -1, wxT("Torpedo strength"), wxDefaultPosition, wxDefaultSize, 0);
    item17->Add(item24, 0, wxALL|0, 2);
    editTorp = new wxTextCtrl(this, IDC_EDITTORP, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item17->Add(editTorp, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item26 = new wxStaticText(this, -1, wxT("Torpedo reloads"), wxDefaultPosition, wxDefaultSize, 0);
    item17->Add(item26, 0, wxALL|0, 2);
    editReloads = new wxTextCtrl(this, IDC_EDITRELOADS, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item17->Add(editReloads, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item28 = new wxStaticText(this, -1, wxT("Antiaircraft"), wxDefaultPosition, wxDefaultSize, 0);
    item17->Add(item28, 0, wxALL|0, 2);
    editAA = new wxTextCtrl(this, IDC_EDITAA, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item17->Add(editAA, 0, wxALIGN_CENTRE|wxALL|0, 2);
    item16->Add(item17, 0, wxALIGN_CENTRE|wxALL|0, 2);

    wxSizer* item30 = new wxFlexGridSizer(2, 0, 0);
    wxStaticText* item31 = new wxStaticText(this, -1, wxT("Protection rating"), wxDefaultPosition, wxDefaultSize, 0);
    item30->Add(item31, 0, wxALL|0, 2);
    editProt = new wxTextCtrl(this, IDC_EDITPROT, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item30->Add(editProt, 0, wxALIGN_CENTRE|wxALL|0, 2);
    wxStaticText* item33 = new wxStaticText(this, -1, wxT("Speed"), wxDefaultPosition, wxDefaultSize, 0);
    item30->Add(item33, 0, wxALL|0, 2);
    editSpeed = new wxTextCtrl(this, IDC_EDITSPEED, wxT(""), wxDefaultPosition, wxSize(30,-1), 0);
    item30->Add(editSpeed, 0, wxALIGN_CENTRE|wxALL|0, 2);
    chkRadar = new wxCheckBox(this, IDC_CHKRADAR, wxT("Radar equipped"), wxDefaultPosition, wxDefaultSize, 0);
    item30->Add(chkRadar, 0, wxALIGN_CENTRE|wxALL|0, 2);
    item16->Add(item30, 0, wxALL|0, 2);
    item0->Add(item16, 0, wxALIGN_CENTRE|wxALL|0, 2);

    wxSizer* item36 = new wxBoxSizer( wxHORIZONTAL );
    wxButton* item37 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item37->SetDefault( );
    item36->Add( item37, 0, wxALIGN_CENTRE|wxALL|0, 2 );
    wxButton* item38 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item38, 0, wxALIGN_CENTRE|wxALL|0, 2 );
    item0->Add( item36, 0, wxALIGN_CENTRE|wxALL|0, 2 );
    SetAutoLayout( true );
    SetSizer( item0 );
    item0->Fit( this );
    item0->SetSizeHints( this );

    Centre( );

    strcpy( class_name_, _ship_type_string[0] );
    nationality_ = 0;
    type_ = 0;
}

DNavalUnitShip::~DNavalUnitShip ()
{
    _unit_editor_active = false;
}

void DNavalUnitShip::EvInitDialog(wxInitDialogEvent&)
{
    editAA->SetValue(wxString::FromAscii(_ship_dialog_data.editAA));
    editName->SetValue(wxString::FromAscii(_ship_dialog_data.editName));
    editPri->SetValue(wxString::FromAscii(_ship_dialog_data.editPri));
    editSec->SetValue(wxString::FromAscii(_ship_dialog_data.editSec));
    editThi->SetValue(wxString::FromAscii(_ship_dialog_data.editThi));
    editMaxHits->SetValue(wxString::FromAscii(_ship_dialog_data.editMaxHits));
    editHits->SetValue(wxString::FromAscii(_ship_dialog_data.editHits));

    int subtype_cnt;
    if ( _current_player == SidePlayer::AXIS )
    	subtype_cnt = NavalArmedForce::Axis::COUNT;
    else
    	subtype_cnt = NavalArmedForce::Allied::COUNT;

    cmbNat->Clear( );

    for ( int i = 0; i < subtype_cnt; ++i )
        cmbNat->Append( wxString::FromAscii( _naval_armed_force[_current_player][i].name_ ) );

    // select right
    // TODO: wxgtl 2.2.0 doesn't have GetCount, update & rebuild!
    int j;
#if defined __WXMSW__
    for ( j = 0; j < static_cast<int>( cmbNat->GetCount( ) ); ++j )
#else
    for ( j = 0; j < subtype_cnt; ++j )
#endif
        if ( strcmp(_naval_armed_force[_current_player][nationality_].name_, cmbNat->GetString( j ).ToAscii()) == 0 )
            break;

    cmbNat->SetSelection( j );

    cmbClass->Clear( );
    j = 0;
    for ( int i = 0; i < NavalUnitType::Ship::COUNT; ++i )
    {
       cmbClass->Append( wxString::FromAscii( _ship_type_string[i] ) );
       if ( strcmp( _ship_type_string[i], class_name_ ) == 0 )
           j = i;
    }
    cmbClass->SetSelection( j );

    chkRadar->SetValue( _ship_dialog_data.chkRadar );
    editProt->SetValue( wxString::FromAscii(_ship_dialog_data.editProt) );
    editSpeed->SetValue( wxString::FromAscii(_ship_dialog_data.editSpeed) );
    editTorp->SetValue( wxString::FromAscii(_ship_dialog_data.editTorp) );
    editReloads->SetValue( wxString::FromAscii(_ship_dialog_data.editReloads) );

    _unit_editor_active = true;
}

void DNavalUnitShip::CloseDialog()
{
	strncpy( _ship_dialog_data.editAA, editAA->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editName, editName->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editPri, editPri->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editSec, editSec->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editThi, editThi->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editMaxHits, editMaxHits->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editHits, editHits->GetValue().ToAscii(), 255 );

	// find out selected subtype
	int subtype_cnt;
	if ( _current_player == SidePlayer::AXIS )
		subtype_cnt = NavalArmedForce::Axis::COUNT;
	else
		subtype_cnt = NavalArmedForce::Allied::COUNT;

	int subtype = -1;
	char s[255];
	strcpy( s, cmbNat->GetString( cmbNat->GetSelection() ).ToAscii() );
	for ( int i = 0; i < subtype_cnt; ++i )
		if ( strcmp( _naval_armed_force[_current_player][i].name_, s ) == 0 )
			subtype = i;

	if ( subtype == -1 )
	{
		wxMessageBox( wxT("No nationality selected, using default!"), Application::NAME );
		subtype = 0;
	}
	nationality_ = subtype;
	type_ = cmbClass->GetSelection();

	_ship_dialog_data.chkRadar = chkRadar->GetValue();
	strncpy( _ship_dialog_data.editProt, editProt->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editSpeed, editSpeed->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editTorp, editTorp->GetValue().ToAscii(), 255 );
	strncpy( _ship_dialog_data.editReloads, editReloads->GetValue().ToAscii(), 255 );
}

#endif
