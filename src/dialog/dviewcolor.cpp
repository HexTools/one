#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include <wx/colordlg.h>

#include "hextools.h"
using ht::inttoa;
using ht::wxS;
#include "counter.h"
#include "dview.h"
#include "dviewcolor.h"
#include "view.h"

extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)

// control ID's
enum
{
	IDC_COLOURBTN,
};
//
// Build a response table for all messages/commands handled by the application.
//
BEGIN_EVENT_TABLE(DViewColor, wxDialog) EVT_PAINT(DViewColor::OnPaintEv)
EVT_INIT_DIALOG(DViewColor::EvInitDialog)

EVT_BUTTON(IDC_COLOURBTN, DViewColor::btnColourClicked)
END_EVENT_TABLE()

ViewColorDlgXfer ViewColorDlgData;

// constructor
DViewColor::DViewColor( wxWindow* parent )
		: wxDialog( parent, -1, wxT("View Properties"), wxDefaultPosition )
{
	NotInitialized = TRUE;

	wxSizer* sizerVMain = new wxBoxSizer( wxVERTICAL );
	wxSizer* sizerHControls = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* sizerVControls = new wxBoxSizer( wxVERTICAL );

	wxSizer* sizerName = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* textName = new wxStaticText( this, -1, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerName->Add( textName, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editName = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 120, -1 ), 0 );
	sizerName->Add( editName, 0, wxALIGN_CENTRE | 0, 5 );
	sizerVControls->Add( sizerName, 0, wxALL | 0, 5 );

	wxSizer* sizerColor = new wxFlexGridSizer( 4, 0, 0 );
	wxStaticText* textColor = new wxStaticText( this, -1, wxT("Colour (RGB):"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerColor->Add( textColor, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	editRed = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	sizerColor->Add( editRed, 0, wxALIGN_CENTRE | 0, 2 );
	editGreen = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	sizerColor->Add( editGreen, 0, wxALIGN_CENTRE | 0, 2 );
	editBlue = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	sizerColor->Add( editBlue, 0, wxALIGN_CENTRE | 0, 2 );
	sizerVControls->Add( sizerColor, 0, wxALIGN_CENTRE | 0, 5 );

	wxSizer* sizerColorButton = new wxBoxSizer( wxHORIZONTAL );
	buttonColor = new wxButton( this, IDC_COLOURBTN, wxT("Select Colour..."), wxDefaultPosition, wxSize( 120, 20 ), 0 );
	sizerColorButton->Add( buttonColor, 0, wxLEFT, 5 );
	sizerVControls->Add( sizerColorButton, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );

	sizerHControls->Add( sizerVControls, 0, wxALIGN_CENTRE | 0, 5 );
	sizerVMain->Add( sizerHControls, 0, wxGROW | wxALL | 0, 2 );

	wxSizer* sizerButtons = new wxBoxSizer( wxHORIZONTAL );
	wxButton* buttonOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonOK->SetDefault( );
	sizerButtons->Add( buttonOK, 0, wxGROW | wxALL | 0, 5 );
	wxButton* buttonCanclel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( buttonCanclel, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	sizerVMain->Add( sizerButtons, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( sizerVMain );

	sizerVMain->Fit( this );
	sizerVMain->SetSizeHints( this );

	Centre( );
}

DViewColor::~DViewColor( )
{
	_unit_editor_active = true; // TODO: check this, shouldn't it be set to false?!
}

void DViewColor::EvInitDialog( wxInitDialogEvent& )
{
	SetupDialog( );
	_unit_editor_active = true;
}

void DViewColor::SetupDialog( )
{
	// edits
	editName->SetValue( wxS( ViewColorDlgData.editName ) );
	editRed->SetValue( wxS( ViewColorDlgData.editRed ) );
	editGreen->SetValue( wxS( ViewColorDlgData.editGreen ) );
	editBlue->SetValue( wxS( ViewColorDlgData.editBlue ) );

	NotInitialized = FALSE;
	return;
}

void DViewColor::EvCloseDialog( wxCloseEvent& )
{
	CloseDialog( );
}

void DViewColor::CloseDialog( )
{
	strncpy( ViewColorDlgData.editName, editName->GetValue( ).ToAscii( ), 255 );
	strncpy( ViewColorDlgData.editRed, editRed->GetValue( ).ToAscii( ), 255 );
	strncpy( ViewColorDlgData.editGreen, editGreen->GetValue( ).ToAscii( ), 255 );
	strncpy( ViewColorDlgData.editBlue, editBlue->GetValue( ).ToAscii( ), 255 );

	_unit_editor_active = false;
}

void DViewColor::OnPaintEv( wxPaintEvent& )
{
	wxPaintDC dc( this );

	PrepareDC( dc );
#if 0
	wxWindow::OnPaint( e );
#endif
}

void DViewColor::btnColourClicked( wxCommandEvent& )
{
	wxColourData data;
	data.SetChooseFull( true );
	wxColour colour( atoi( editRed->GetValue( ).ToAscii( ) ), atoi( editGreen->GetValue( ).ToAscii( ) ), atoi( editBlue->GetValue( ).ToAscii( ) ) );
	data.SetCustomColour( 0, colour );
	for ( int i = 1; i < 16; ++i )
	{
		wxColour color( i * 16, i * 16, i * 16 );
		data.SetCustomColour( i, color );
	}

	wxColourDialog dialog( this, &data );
	if ( dialog.ShowModal( ) == wxID_OK )
	{
		char numstr[255];
		wxColourData retData = dialog.GetColourData( );
		wxColour col = retData.GetColour( );
		strcpy( numstr, inttoa( col.Red( ) ) );
		editRed->SetValue( wxS( numstr ) );
		strcpy( numstr, inttoa( col.Green( ) ) );
		editGreen->SetValue( wxS( numstr ) );
		strcpy( numstr, inttoa( col.Blue( ) ) );
		editBlue->SetValue( wxS( numstr ) );
	}
#if 0
	ZOCDlg dlg( this );
	dlg.SetZOCStr( ViewColorDlgData.zocstr );
	if ( dlg.ShowModal() == wxID_OK )
	{
		strcpy( ViewColorDlgData.zocstr, dlg.GetZOCStr( ) );
		lblZOC->SetLabel( wxS( dlg.GetZOCStr( ) ) );
	}
#endif
}

#endif
