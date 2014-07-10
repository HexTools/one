#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dresourcemarker.h"
#include "hextools.h"
using ht::wxS;

BEGIN_EVENT_TABLE(DResourceMarker, wxDialog)
EVT_INIT_DIALOG(DResourceMarker::EvInitDialog)
END_EVENT_TABLE()

RpDlgXfer RpDlgData;

wxBitmap RPBitmaps( size_t index )
{
	if ( index == 0 )
	{
		/* XPM */
		static const char* xpm_data[] =
		{
		/* columns rows colors chars-per-pixel */
		"16 16 3 1", "  c None", "a c Black", "b c #FFFFFF",
		/* pixels */
		"                ", "     aaaa       ", "   aaaabbaa     ", "  aaaaabbbba    ", " aaaaaabbbbba   ",
			"aaaaaaabbbbbba  ", "aaaaaaabbbbbbba ", "aaaaaaabbbbbbba ", "aaaaaaabbbbbbba ", "aaaaaaabbbbbbba ",
			"aaaaaaabbbbbbba ", " aaaaaabbbbbbba ", " aaaaaabbbbbba  ", "  aaaaabbbba    ", "   aaaabbaa     ",
			"     aaaa       ", };
		wxBitmap bitmap( xpm_data );
		return bitmap;
	}
	return wxNullBitmap;
}

DResourceMarker::DResourceMarker( wxWindow* parent )
		: wxDialog( parent, -1, wxT("RP Properties"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticBitmap* item2 = new wxStaticBitmap( this, -1, RPBitmaps( 0 ), wxDefaultPosition, wxDefaultSize );
	item1->Add( item2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALL | 0, 5 );
	wxSizer* item3 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item4 = new wxStaticText( this, -1, wxT("Resource points"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

#define IDC_EDITPTS 1
	editPts = new wxTextCtrl( this, IDC_EDITPTS, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 0 );

	item3->Add( editPts, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item6 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item7 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item7->SetDefault( );
	item6->Add( item7, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item8 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item8, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	Centre( );
}

DResourceMarker::~DResourceMarker( )
{
}

void DResourceMarker::EvInitDialog( wxInitDialogEvent& )
{
	editPts->SetValue( wxS( RpDlgData.editPts ) );
}

void DResourceMarker::CloseRpDialog( )
{
	strcpy( RpDlgData.editPts, editPts->GetValue( ).ToAscii( ) );
}

#endif
