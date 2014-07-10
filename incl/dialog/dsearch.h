#if defined HEXTOOLSPLAY
#ifndef DSEARCH_H
#define DSEARCH_H

class DSearch : public wxDialog
{
public:
	DSearch( wxWindow* parent );
	virtual ~DSearch( );

	void SetSearchText( wxString s );
	wxString GetSearchText( );
	void AddHistory( wxString s );

	bool WholeNameOnly( );
	bool CaseSensitive( );
	bool UnitIdSelected( );
	bool UnitDescriptionSelected( );
	bool PlanesSelected( );
	bool CitiesSelected( );
	bool HexesSelected( );

protected:
	void EvInitDialog( wxInitDialogEvent& );

	void CmbTextChanged( wxCommandEvent& );
	void chkAllClicked( wxCommandEvent& );

	wxComboBox* cmbText;

	wxCheckBox* chkCaseSensitive;
	wxCheckBox* chkMatchWhole;

	wxCheckBox* chkAll;
	wxCheckBox* chkGndId;
	wxCheckBox* chkGndDesc;
	wxCheckBox* chkAir;
	wxCheckBox* chkCity;
	wxCheckBox* chkHex;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
