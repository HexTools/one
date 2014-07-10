#if defined HEXTOOLSPLAY
#ifndef DENDPLAYERTURN_H
#define DENDPLAYERTURN_H

class DEndPlayerTurn : public wxDialog
{
public:
	DEndPlayerTurn( wxWindow* parent );
	virtual ~DEndPlayerTurn( );
protected:
	void BtnExit( wxCommandEvent& e );
	void BtnContinue( wxCommandEvent& e );
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
