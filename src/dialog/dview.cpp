#if defined HEXTOOLSPLAY

#include <wx/wx.h>
#include <wx/colordlg.h>

#include "frame.h"
#include "application.h"
#include "hextools.h"
using ht::wxS;
using ht::inttoa;
#include "counter.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunit.h"
#include "dview.h"

DECLARE_APP(Application);

extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)

extern ViewDlgXfer _view_dialog_data;

enum
{
	IDC_EDITCAPTION = 1,
	IDC_EDITTEXT,
	IDC_SAVEBTN,
	IDC_COLOURBTN,
};

BEGIN_EVENT_TABLE(DView, wxDialog) EVT_INIT_DIALOG(DView::EvInitDialog)
EVT_BUTTON(IDC_SAVEBTN, DView::buttonSaveClicked)
EVT_BUTTON(IDC_COLOURBTN, DView::buttonColorClicked)
END_EVENT_TABLE()

DView::DView( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Note"), wxDefaultPosition )
{
	view_index_ = 0;
	wxSizer* sizerVMain = new wxBoxSizer( wxVERTICAL );
	wxSizer* sizerHCaption = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* textCaption = new wxStaticText( this, -1, wxT("Caption"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerHCaption->Add( textCaption, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editCaption = new wxTextCtrl( this, IDC_EDITCAPTION, wxT(""), wxDefaultPosition, wxSize( 140, -1 ), 0 );
	sizerHCaption->Add( editCaption, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	sizerVMain->Add( sizerHCaption, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* sizerHText = new wxBoxSizer( wxHORIZONTAL );
	editText = new wxTextCtrl( this, IDC_EDITTEXT, wxT(""), wxDefaultPosition, wxSize( 220, 140 ), wxTE_MULTILINE );
	sizerHText->Add( editText, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	sizerVMain->Add( sizerHText, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* sizerColor = new wxFlexGridSizer( 4, 0, 0 );
	wxStaticText* textColor = new wxStaticText( this, -1, wxT("Colour (RGB):"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerColor->Add( textColor, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	editRed = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	sizerColor->Add( editRed, 0, wxALIGN_CENTRE | 0, 2 );
	editGreen = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	sizerColor->Add( editGreen, 0, wxALIGN_CENTRE | 0, 2 );
	editBlue = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	sizerColor->Add( editBlue, 0, wxALIGN_CENTRE | 0, 2 );
	sizerVMain->Add( sizerColor, 0, wxALIGN_CENTRE | 0, 5 );

	wxSizer* sizerColorButton = new wxBoxSizer( wxHORIZONTAL );
	buttonColor = new wxButton( this, IDC_COLOURBTN, wxT("Select Colour..."), wxDefaultPosition, wxSize( 120, 20 ), 0 );
	sizerColorButton->Add( buttonColor, 0, wxLEFT, 5 );
	sizerVMain->Add( sizerColorButton, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );

	wxSizer* sizerHButtons = new wxBoxSizer( wxHORIZONTAL );
	wxButton* buttonSave = new wxButton( this, IDC_SAVEBTN, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerHButtons->Add( buttonSave, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* buttonClose = new wxButton( this, wxID_CANCEL, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerHButtons->Add( buttonClose, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	sizerVMain->Add( sizerHButtons, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	SetAutoLayout( true );
	SetSizer( sizerVMain );
	sizerVMain->Fit( this );
	sizerVMain->SetSizeHints( this );

	Center( );
}

DView::~DView( )
{
	_unit_editor_active = false;
}

void DView::InitControls( )
{
	editCaption->SetValue( wxString::FromAscii( _view_dialog_data.editCaption ) );
	editText->SetValue( wxString::FromAscii( _view_dialog_data.editText ) );
	editRed->SetValue( wxS( _view_dialog_data.editRed ) );
	editGreen->SetValue( wxS( _view_dialog_data.editGreen ) );
	editBlue->SetValue( wxS( _view_dialog_data.editBlue ) );
	view_index_ = _view_dialog_data.CurrentView;
}

void DView::EvInitDialog( wxInitDialogEvent& )
{
	InitControls( );
	_unit_editor_active = true;
}

void DView::CloseDialog( )
{
}

void DView::buttonSaveClicked( wxCommandEvent& )
{
	strncpy( _view_dialog_data.editCaption, editCaption->GetValue( ).ToAscii( ), HexNote::MAX_CAPTION_LEN );
	strncpy( _view_dialog_data.editText, editText->GetValue( ).ToAscii( ), HexNote::MAX_TEXT_LEN );
	strncpy( _view_dialog_data.editRed, editRed->GetValue( ).ToAscii( ), 255 );
	strncpy( _view_dialog_data.editGreen, editGreen->GetValue( ).ToAscii( ), 255 );
	strncpy( _view_dialog_data.editBlue, editBlue->GetValue( ).ToAscii( ), 255 );
	_view_dialog_data.CurrentView = view_index_;
	wxGetApp( ).frame_->UpdateViewData( );
}

void DView::buttonColorClicked( wxCommandEvent& )
{
	wxColourData data;
	data.SetChooseFull( true );
	wxColour colour( atoi( editRed->GetValue( ).ToAscii( ) ), atoi( editGreen->GetValue( ).ToAscii( ) ),
			atoi( editBlue->GetValue( ).ToAscii( ) ) );
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
	dlg.SetZOCStr( ViewDlgData.zocstr );
	if ( dlg.ShowModal() == wxID_OK )
	{
		strcpy( ViewDlgData.zocstr, dlg.GetZOCStr( ) );
		lblZOC->SetLabel( wxS( dlg.GetZOCStr( ) ) );
	}
#endif
}

#endif
