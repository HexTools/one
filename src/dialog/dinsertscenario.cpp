#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dinsertscenario.h"
#include "frame.h"

enum
{
	IDC_BTNSELECTSCN = 1
};

BEGIN_EVENT_TABLE(DInsertScenario, wxDialog)
EVT_INIT_DIALOG(DInsertScenario::EvInitDialog)
EVT_BUTTON(IDC_BTNSELECTSCN, DInsertScenario::MapClicked)
END_EVENT_TABLE()

#define ID_TEXT -1
#define ID_BUTTON -1
#define ID_TEXTCTRL -1

DInsertScenario::DInsertScenario( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Insert Scenario"), wxDefaultPosition )
{
	wxBoxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxStaticBoxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

	wxFlexGridSizer* item3 = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText* item4 = new wxStaticText( this, ID_TEXT, wxT("Scenario file"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item4, 0, wxALIGN_CENTRE | wxALL, 5 );

	wxBoxSizer* item5 = new wxBoxSizer( wxHORIZONTAL );

	editFileName = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize( 190, -1 ), 0 );
	item5->Add( editFileName, 0, wxALIGN_CENTRE | wxTOP | wxBOTTOM, 5 );

	btnSelectFile = new wxButton( this, IDC_BTNSELECTSCN, wxT("..."), wxDefaultPosition, wxSize( 20, -1 ), 0 );
	item5->Add( btnSelectFile, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | wxBOTTOM, 5 );

	item3->Add( item5, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, 5 );

	wxStaticText* item8 = new wxStaticText( this, ID_TEXT, wxT("X-offset"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item8, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	editX = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 0 );
	item3->Add( editX, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxStaticText* item10 = new wxStaticText( this, ID_TEXT, wxT("Y-offset"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item10, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	editY = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 0 );
	item3->Add( editY, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	item1->Add( item3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxStaticText* item12 = new wxStaticText( this, ID_TEXT, wxT("Offset is location in the _existing_ scenario where\n"
			"new scenario will be inserted. It is possible to enter\n"
			"negative values."), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( item12, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxStaticText* item13 = new wxStaticText( this, ID_TEXT, wxT("Offset is given in x,y hex coordinates, not as map\n"
			"hex number (i.e. 1:1234). You can find coordinates\n"
			"of any hex by double-clicking the hex. Coordinates\n"
			"are show after map hex number."), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( item13, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL, 5 );

	wxBoxSizer* item14 = new wxBoxSizer( wxHORIZONTAL );

	wxButton* item15 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item14->Add( item15, 0, wxALIGN_CENTRE | wxALL, 5 );

	wxButton* item16 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item14->Add( item16, 0, wxALIGN_CENTRE | wxALL, 5 );

	item0->Add( item14, 0, wxALIGN_CENTRE | wxALL, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	Centre( );
}

void DInsertScenario::EvInitDialog( wxInitDialogEvent& )
{
	SetupDialog( );
}

void DInsertScenario::SetupDialog( )
{
}

void DInsertScenario::CloseDialog( )
{
}

void DInsertScenario::MapClicked( wxCommandEvent& )
{
	wxFileDialog dialog(	this,
							wxT("Open scenario"),
							wxT(""),
							wxT(""),
							Frame::FILE_TYPES,
							0 );

	if ( dialog.ShowModal( ) == wxID_OK )
		editFileName->SetValue( dialog.GetFilename( ) );
}

#endif
