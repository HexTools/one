#if defined HEXTOOLSMAP

#ifndef DEXTENDMAP_H
#define DEXTENDMAP_H

class DExtendMap : public wxDialog
{
public:
	DExtendMap( wxWindow* p, wxString title = "Extend map" );
	void setCaption( const char* t )
	{
		SetTitle( t );
	}
	void getData( int* cnt, int* dir );

protected:
	void Ok( wxCommandEvent& );
	void Cancel( wxCommandEvent& );
	void EvInitDialog( wxInitDialogEvent& );

	wxTextCtrl* editCnt;
	wxRadioButton* rdNorth;
	wxRadioButton* rdSouth;
	wxRadioButton* rdWest;
	wxRadioButton* rdEast;

	DECLARE_EVENT_TABLE()
};

#endif

#endif
