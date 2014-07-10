#if defined HEXTOOLSPLAY
#ifndef DWEATHER_H
#define DWEATHER_H

struct WeatherDlgXfer
{
	bool RdAClear;
	bool RdAMud;
	bool RdASnow;
	bool RdAFrost;
	bool RdAWinter;

	bool RdBClear;
	bool RdBMud;
	bool RdBSnow;
	bool RdBFrost;
	bool RdBWinter;

	bool RdCClear;
	bool RdCMud;
	bool RdCSnow;
	bool RdCFrost;
	bool RdCWinter;

	bool RdDClear;
	bool RdDMud;
	bool RdDSnow;
	bool RdDFrost;
	bool RdDWinter;

	bool RdEClear;
	bool RdEMud;
	bool RdESnow;
	bool RdEFrost;
	bool RdEWinter;

	bool RdFClear;
	bool RdFMud;
	bool RdFSnow;
	bool RdFFrost;
	bool RdFWinter;

	bool RdGClear;
	bool RdGMud;
	bool RdGSnow;
	bool RdGFrost;
	bool RdGWinter;

	int chkIceA;
	int chkIceB;
	int chkIceC;
	int chkIceD;
	int chkIceE;
	int chkIceF;
	int chkIceG;
};

class DWeather : public wxDialog
{

public:
	DWeather( wxWindow* parent );
	virtual ~DWeather( );

	void CloseDialog( );
	void EvInitDialog( wxInitDialogEvent& );

protected:
	wxListCtrl* listWZ;
/*
	wxRadioButton* RdAClear;
	wxRadioButton* RdAFrost;
	wxRadioButton* RdAMud;
	wxRadioButton* RdASnow;
	wxRadioButton* RdAWinter;

	wxRadioButton* RdBClear;
	wxRadioButton* RdBFrost;
	wxRadioButton* RdBMud;
	wxRadioButton* RdBSnow;
	wxRadioButton* RdBWinter;

	wxRadioButton* RdCClear;
	wxRadioButton* RdCFrost;
	wxRadioButton* RdCMud;
	wxRadioButton* RdCSnow;
	wxRadioButton* RdCWinter;

	wxRadioButton* RdDClear;
	wxRadioButton* RdDFrost;
	wxRadioButton* RdDMud;
	wxRadioButton* RdDSnow;
	wxRadioButton* RdDWinter;

	wxRadioButton* RdEClear;
	wxRadioButton* RdEFrost;
	wxRadioButton* RdEMud;
	wxRadioButton* RdESnow;
	wxRadioButton* RdEWinter;

	wxRadioButton* RdFClear;
	wxRadioButton* RdFFrost;
	wxRadioButton* RdFMud;
	wxRadioButton* RdFSnow;
	wxRadioButton* RdFWinter;

	wxRadioButton* RdGClear;
	wxRadioButton* RdGFrost;
	wxRadioButton* RdGMud;
	wxRadioButton* RdGSnow;
	wxRadioButton* RdGWinter;

	wxCheckBox* chkIceA;
	wxCheckBox* chkIceB;
	wxCheckBox* chkIceC;
	wxCheckBox* chkIceD;
	wxCheckBox* chkIceE;
	wxCheckBox* chkIceF;
	wxCheckBox* chkIceG;
*/

	void ChangeWeatherClick( wxCommandEvent& );
	void ChangeIceClick( wxCommandEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
