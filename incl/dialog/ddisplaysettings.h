#if defined HEXTOOLSPLAY
#ifndef DDISPLAYSETTINGS_H
#define DDISPLAYSETTINGS_H

struct DisplayOptionsDlgXfer
{
	bool rdAlliedAtt;
	bool rdAlliedDef;
	bool rdAxisAtt;
	bool rdAxisDef;
	bool ChkOwner;
	bool ChkBak;
	bool ChkWeather;
	bool ChkTrans;
	char cmbSubtypeFilt[255];
	bool chkQScrollEnable;
	bool chkQScrollMirror;
	bool chkGrayHexes;
	bool chkClickToMove;
	bool chkMpsOnMap;
	bool chkUseCorpsMarkers;
	int tileset;
	int alliedhexownersym;
};

class DDisplaySettings : public wxDialog
{
public:
	DDisplaySettings( wxWindow* parent );
	/* virtual */
	~DDisplaySettings( void );

	int SubTypePalette( )
	{
		return subtype_palette_;
	}
	void SetSubTypePalette( int i )
	{
		subtype_palette_ = i;
	}

	int quickscroll_velocity_;
protected:
	int subtype_palette_;

public:
	void CloseDialog( );

protected:
	wxRadioButton* rdAlliedAtt;
	wxRadioButton* rdAlliedDef;
	wxRadioButton* rdAxisAtt;
	wxRadioButton* rdAxisDef;
	wxCheckBox* chkWeather;
	wxCheckBox* chkTrans;
	wxCheckBox* chkQScrollEnable;
	wxCheckBox* chkQScrollMirror;
#if 0
	wxComboBox* cmbSubtypeFilt;
#endif
	wxCheckBox* chkBak;
	wxCheckBox* chkGrayHexes;
	wxCheckBox* chkClickToMove;
	wxCheckBox* chkMpsOnMap;
	wxCheckBox* chkUseCorpsMarkers;
	wxComboBox* cmbTileset;
	wxComboBox* cmbAlliedSymbol;

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
