#if defined HEXTOOLSPLAY
#ifndef DUNITZOC_H
#define DUNITZOC_H

class DUnitZOC : public wxDialog
{

public:
	DUnitZOC( wxWindow* parent );
	virtual ~DUnitZOC( );

	// ZOCs are transferred as strings "F-/R-"
	void SetZOCStr( char* s );
	char* GetZOCStr( );

protected:
	void EvInitDialog( wxInitDialogEvent& );
	void UpdateCombos( );  // update combo boxes per zocstr

	void CmbU0FullChanged( wxCommandEvent& );
	void CmbU0CdrChanged( wxCommandEvent& );
	void CmbU2FullChanged( wxCommandEvent& );
	void CmbU2CdrChanged( wxCommandEvent& );

	char ZOC_string_[10];
	wxChoice* cmbU0F;
	wxChoice* cmbU0C;
	wxChoice* cmbU2F;
	wxChoice* cmbU2C;
	wxStaticText* lblZOC;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
