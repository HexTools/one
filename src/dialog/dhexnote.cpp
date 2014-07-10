#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "counter.h"
#include "dhexnote.h"
#include "hexnote.h"

extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)

extern NoteDlgXfer _note_dialog_data;

enum
{
	IDC_EDITCAPTION = 1,
	IDC_EDITTEXT,
	IDC_CHKALERT
};

BEGIN_EVENT_TABLE(DHexNote, wxDialog)
EVT_INIT_DIALOG(DHexNote::EvInitDialog)
END_EVENT_TABLE()

DHexNote::DHexNote( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Note"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item2 = new wxStaticText( this, -1, wxT("Caption"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( item2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editCaption = new wxTextCtrl( this, IDC_EDITCAPTION, wxT(""), wxDefaultPosition, wxSize( 140, -1 ), 0 );
	item1->Add( editCaption, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* item4 = new wxBoxSizer( wxHORIZONTAL );
	editText = new wxTextCtrl( this, IDC_EDITTEXT, wxT(""), wxDefaultPosition, wxSize( 220, 140 ), wxTE_MULTILINE );
	item4->Add( editText, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* itemxx = new wxBoxSizer( wxHORIZONTAL );
	chkAlert = new wxCheckBox( this, IDC_CHKALERT, wxT("Use alert icon for this note"), wxDefaultPosition,
			wxSize( 220, -1 ), 0 );
	itemxx->Add( chkAlert, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( itemxx, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* item6 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item7 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item7, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item8 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item8, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	Center( );
}

DHexNote::~DHexNote( )
{
	_unit_editor_active = false;
}

void DHexNote::EvInitDialog( wxInitDialogEvent& )
{
	editCaption->SetValue( wxString::FromAscii( _note_dialog_data.editCaption ) );
	editText->SetValue( wxString::FromAscii( _note_dialog_data.editText ) );
	chkAlert->SetValue( _note_dialog_data.AlertIcon );
	_unit_editor_active = true;
}

void DHexNote::CloseDialog( )
{
	strncpy( _note_dialog_data.editCaption, editCaption->GetValue( ).ToAscii( ), HexNote::MAX_CAPTION_LEN );
	strncpy( _note_dialog_data.editText, editText->GetValue( ).ToAscii( ), HexNote::MAX_TEXT_LEN );
	_note_dialog_data.AlertIcon = chkAlert->GetValue( );
}

#endif
