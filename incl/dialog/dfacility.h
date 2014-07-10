#if defined HEXTOOLSPLAY
#ifndef DFACILITY_H
#define DFACILITY_H

struct MiscDlgXfer
{
	bool rdFort;
	bool rdPermAF;
	bool rdTempAF;
	int rdPermAF12;
	int rdPermAF6;
	int rdPermAF9;
	int rdFieldWorks;
	int rdEntr;
	int rdImpFort;
	int rdZepBase;
};

class DFacility : public wxDialog
{
public:
	DFacility( wxWindow* parent );
	virtual ~DFacility( );

protected:
	wxRadioButton* rdFort;
	wxRadioButton* rdPermAF;
	wxRadioButton* rdTempAF;
	wxRadioButton* rmPermAF12;
	wxRadioButton* rdPermAF6;
	wxRadioButton* rdPermAF9;
	wxRadioButton* rdFieldWorks;
	wxRadioButton* rdEntr;
	wxRadioButton* rdImpFort;
	wxRadioButton* rdZepBase;

public:
	void CloseDialog( );

protected:
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
