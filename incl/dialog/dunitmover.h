#if defined HEXTOOLSPLAY
#ifndef DUNITMOVER_H
#define DUNITMOVER_H

class DUnitMover : public wxDialog
{

public:
	DUnitMover( wxWindow* parent );
	virtual ~DUnitMover( );

protected:
	wxWindow* Parent;

	void BtnEast( wxCommandEvent& e );
	void BtnNorthEast( wxCommandEvent& e );
	void BtnNorthWest( wxCommandEvent& e );
	void BtnSouthEast( wxCommandEvent& e );
	void BtnSouthWest( wxCommandEvent& e );
	void BtnWest( wxCommandEvent& e );
	void BtnClose( wxCommandEvent& e );
	void BtnHome( wxCommandEvent& e );
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
