#if defined HEXTOOLSPLAY
#ifndef DHEXINFO_H
#define DHEXINFO_H

class DHexInfo : public wxDialog
{
public:
	DHexInfo( wxWindow* parent );

public:
	void SetupDialog( );
	void CloseDialog( );

protected:
	void EvInitDialog( wxInitDialogEvent& );
	void OnPaintEv( wxPaintEvent& );
	void OwnerClicked( wxCommandEvent& );
	void GaugeClicked( wxCommandEvent& );

	wxPanel* pHex;
	wxStaticText* lblHexId;
	wxStaticText* lblTerrain;
	wxStaticText* lblOwner;
	wxStaticText* lblWeather;
	wxStaticText* lblRail;
	wxStaticText* lblHarr;
	wxStaticText* lblHarrInfo;
	wxStaticText* lblAirField;
	wxStaticText* lblPort;
	wxStaticText* lblCity;

	DECLARE_EVENT_TABLE()
};

struct HexDlgXfer
{
	int terrain;
	char hexid[50];
	char terrain_str[50];
	char owner[50];
	char weather[50];
	char rail[50];
	char harr[50];
	char airfield[50];
	char port[100];
	char city[200];
};

#endif
#endif
