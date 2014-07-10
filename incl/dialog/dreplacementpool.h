#if defined HEXTOOLSPLAY
#ifndef DREPLACEMENTPOOL_H
#define DREPLACEMENTPOOL_H

class Counter;

/*
 struct ReplDlgXfer {
 int     Scroller;
 char    Frame[ 255 ];
 char    lblInfo[ 255 ];
 };
 */

class DReplacementPool : public wxDialog
{

public:
	DReplacementPool( wxWindow* parent );
	virtual ~DReplacementPool( );

	//void EvVScroll(UINT code, UINT pos, HWND wnd);
	Counter *GetSelCntr( )
	{
		return selected_counter_;
	}
	void SetAirMode( int v )
	{
		air_mode_ = v;
	}

protected:
	int air_mode_;  // nonzero -> show air repl pool, zero -> gnd repl pool
	int y0_;
	int selected_index_;
	Counter* selected_counter_;

	void SetScrollBarRange( );
	void Redraw( );

	wxScrollBar* Scroller;
	wxStaticText* lblInfo, *lblInfo2;
	wxButton* btnRebuild;
	wxButton* btnRemove;
	wxButton* btnAdd;
	wxButton* btnEdit;

public:
	void CloseDialog( );

protected:
	void RebuildClicked( wxCommandEvent& e );
	void RemoveClicked( wxCommandEvent& e );
	void Paint( wxPaintEvent& );
	void EvVScroll( wxScrollEvent& );
	void EvLButtonDown( wxMouseEvent& e );
	void AddClicked( wxCommandEvent& e );
	void EditClicked( wxCommandEvent& e );
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
