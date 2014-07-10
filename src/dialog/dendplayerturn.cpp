#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dendplayerturn.h"

extern char* mbox3232[];

enum
{
	IDC_PUSHEXIT,
	IDC_PUSHCONT
};

BEGIN_EVENT_TABLE(DEndPlayerTurn, wxDialog)
EVT_BUTTON(IDC_PUSHEXIT, DEndPlayerTurn::BtnExit)
EVT_BUTTON(IDC_PUSHCONT, DEndPlayerTurn::BtnContinue)
#if 0
EVT_INIT_DIALOG( DEndPlayerTurn::EvInitDialog )
#endif
END_EVENT_TABLE()

DEndPlayerTurn::DEndPlayerTurn( wxWindow* parent )
		: wxDialog( parent, -1, wxT("End of Turn"), wxDefaultPosition )
{
	wxBitmap bmp( mbox3232 );

	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxHORIZONTAL );
	wxSizer* item3 = new wxFlexGridSizer( 2, 0, 0 );
	wxStaticBitmap* item4 = new wxStaticBitmap( this, -1, bmp, wxDefaultPosition, wxDefaultSize );
	item3->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item5 = new wxBoxSizer( wxVERTICAL );
	wxStaticText* item6 = new wxStaticText( this, -1, wxT("Your player turn has ended!"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxStaticText* item7 = new wxStaticText( this, -1, wxT("You should now save the game and send"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item7, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item8 = new wxStaticText( this, -1, wxT("it to your opponent"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item8, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	item3->Add( item5, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item9 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* btnExit = new wxButton( this, wxID_OK, wxT("Save and Quit"), wxDefaultPosition, wxDefaultSize, 0 );
	item9->Add( btnExit, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* btnContinue = new wxButton( this, wxID_CANCEL, wxT("Continue Playing"), wxDefaultPosition, wxDefaultSize, 0 );
	item9->Add( btnContinue, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item9, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );
}

DEndPlayerTurn::~DEndPlayerTurn( )
{
}

void DEndPlayerTurn::BtnExit( wxCommandEvent& WXUNUSED(e) )
{
}

void DEndPlayerTurn::BtnContinue( wxCommandEvent& WXUNUSED(e) )
{
}

#endif
