#ifndef HEXINFOPANE_H
#define HEXINFOPANE_H

#if defined HEXTOOLSMAP
class Label; // internal class for displaying texts in HT-m
#endif
class HexInfoPane : public wxWindow
{
public:
	HexInfoPane( wxWindow* p, int i, const wxPoint& pt, const wxSize& sz, long l = 0 );
	virtual ~HexInfoPane( );

	void displayHexInfo( 
		int x,
		int y,
		char* mapid, 
		char* hexid, 
		wxString terrain, 
		int hexside, 
		int inthexside, 
		int road, 
		wxString city,
		bool stacking, 
		bool waterway, 
		bool vertex, 
		wxString seabox, 
		wxString district,
		wxString seazone,
		wxString country,
		char* wzone, 
		char* weather 
	);
#if defined HEXTOOLSPLAY
	void displayTurnInfo( char* turn, char* phase )
	{
		lblTurn->SetLabel( ht::wxS( turn ) );
		lblPhase->SetLabel( ht::wxS( phase ) );
	}
#endif

protected:
#if defined HEXTOOLSMAP
	Label* hex_number_;					// "x,y" HT-m only
	Label* hex_id_;						// "1A:1234"
	Label* terrain_;
	Label* hexside_;
	Label* road_;
	Label* external_hexside_;
	Label* city_;
	Label* seabox_;
	Label* district_;
#elif defined HEXTOOLSPLAY
	wxStaticText* lblTurn;				// HT-p only
	wxStaticText* lblPhase;				// HT-p only
	wxStaticText* hex_id_;				// "1A:1234"
	wxStaticText* terrain_;
	wxStaticText* hexside_;
	wxStaticText* road_;
	wxStaticText* external_hexside_;
	wxStaticText* lblWeather;			// HT-p only
	wxStaticText* city_;
	wxStaticText* seabox_;
	wxStaticText* district_;
	//wxStaticText* seazone_;				// HT-p only
	//wxStaticText* country_;				// HT-p only

	void Paint( wxPaintEvent& event );

	DECLARE_EVENT_TABLE()
#endif
};

#endif
