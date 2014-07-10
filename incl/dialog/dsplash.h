#if defined HEXTOOLSPLAY
#ifndef DSPLASH_H
#define DSPLASH_H

class DSplash : public wxDialog
{

public:
	DSplash( wxWindow* parent );
	virtual ~DSplash( );

	int HasSent( );

protected:
	wxCheckBox* chkSent;

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
