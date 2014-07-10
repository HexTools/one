#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dsplash.h"

BEGIN_EVENT_TABLE(DSplash, wxDialog)
#if 0
EVT_INIT_DIALOG( DSplash::EvInitDialog )
#endif
END_EVENT_TABLE()

extern char* PostCardTxt;

extern char* mbox3232[];

DSplash::DSplash( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Welcome to HexTools"), wxDefaultPosition )
{
	wxBitmap bmp( mbox3232 );

	wxSizer* main_v_sizer = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticBitmap* item2 = new wxStaticBitmap( this, -1, bmp, wxDefaultPosition, wxDefaultSize );
	item1->Add( item2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item3 = new wxBoxSizer( wxVERTICAL );

#ifdef __WXMSW__
	wxStaticText* item4 = new wxStaticText( this, -1, wxString::FromAscii( PostCardTxt ), wxDefaultPosition, wxSize( 280, -1 ), 0 );
#else
	wxStaticText* item4 = new wxStaticText( this, -1, wxString::FromAscii( PostCardTxt ), wxDefaultPosition, wxSize( 300, -1 ), 0 );
#endif
	item3->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	main_v_sizer->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	chkSent = new wxCheckBox( this, -1, wxT("I have already sent the postcard."), wxDefaultPosition, wxDefaultSize, 0 );
	main_v_sizer->Add( chkSent, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item6 = new wxButton( this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	main_v_sizer->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( main_v_sizer );
	main_v_sizer->Fit( this );
	main_v_sizer->SetSizeHints( this );
	Center( );
}

DSplash::~DSplash( )
{
}

int DSplash::HasSent( )
{
	return chkSent->GetValue( );
}

#endif
