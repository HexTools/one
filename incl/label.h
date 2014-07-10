#if defined HEXTOOLSMAP

#ifndef LABEL_H
#define LABEL_H

// nice looking label that looks identical in GTK/MSW
class Label : public wxPanel
{
public:
	Label( wxWindow* parent, const wxPoint& pos, const wxSize& size )
			: wxPanel( parent, -1, pos, size, wxSUNKEN_BORDER )
	{
		wxSize s = size;
		s.SetWidth( s.GetWidth( ) - 2 );
		label_ = new wxStaticText( this, -1, " ", wxPoint( 0, 1 ), s, wxST_NO_AUTORESIZE | wxALIGN_LEFT );
		label_->SetBackgroundColour( wxColour( 255, 255, 255 ) );	// needed in MSW
		label_->SetForegroundColour( wxColour( 0, 0, 0 ) );
		SetBackgroundColour( wxColour( 255, 255, 255 ) );			// needed in GTK
	}
	void setLabel( wxString ws )
	{
		ws = " " + ws;
		if ( label_->GetLabel( ) != ws )  // not checked in wxLabel...
			label_->SetLabel( ws );
	}
protected:
	wxStaticText* label_;
};

#endif

#endif
