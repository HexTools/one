#if defined HEXTOOLSPLAY
#ifndef HEXCONTENTSPANE_H
#define HEXCONTENTSPANE_H

class Counter;
class MapPane;

class HexContentsPane : public wxScrolledWindow
{
public:
	static const unsigned short MAXUNITS = 100;

	HexContentsPane( wxWindow* p, int i, const wxPoint& pt, const wxSize& sz, long l = 0 );
	virtual ~HexContentsPane( );

	// PAPI (MapPane)
	void addUnit( Counter* unit );		// add one unit
	void addUnits( Counter* units );	// add list of units

	// PAPI (MapPane)
	void removeUnit( Counter* unit );	// remove one unit
	int getSelCount( );					// count selected units

	// PAPI (MapPane::Search)
	Counter* getUnit( int line )
	{
		if ( line < num_units_ )
			return unit_[line];
		return NULL;
	}

	// PAPI (Frame)
	void editUnit( );					// edit selected unit(s)
	void deleteUnit( );					// delete selected unit(s)

	// PAPI (MapPane)
	void clear( void );					// remove all units
	void clearDeselected( void );		// remove all non-selected units (teleporting...)

	// used by selectUnit(..) function
	void insertToSelectedList( Counter* unit );

	// PAPI (MapPane::March)
	void selectUnit( Counter* unit, bool skip_graying = false );

	// PAPI (MapPane::Search)
	void selectUnit( int i, bool skip_graying = false );        // select unit i

	// used by deselectUnit(..) function
	void removeFromSelectedList( Counter* unit );
	void deselectUnit( int i, bool skip_graying = false, bool skip_path_erasing = false );

	// PAPI (MapPane)
	void deselectUnit( Counter* unit, bool skip_graying = false );

	// PAPI (Frame)
	void selectAll( void );

	// PAPI (Frame and MapPane)
	void deselectAll( void );

	// PAPI (MapPane)
	void markAllSelected( void );		// mark all units as selected

	// not used anywhere, previously used in MapPane::CmUnitHome( ), which is the undo function
	void markAllUnselected( void );		// mark all units as unselected

	// PAPI (Frame)
	void attachMap( MapPane* map )
	{
		map_ = map;
	}

protected:

/// FUNCTIONS ///
	// not used anywhere
	void DrawBox( wxDC* dc, int x1, int y1, int x2, int y2 );

	// event functions and helpers
	void EvMouseMove( wxMouseEvent &e );
	void EvLButtonDown( wxMouseEvent &e );
	void EvLButtonDblClk( wxMouseEvent &e );
	void EvRButtonDown( wxMouseEvent &e );

	void RedrawGrid( wxDC* dc );
	void RedrawTexts( );
	void paint( wxPaintEvent &e );

/// DATA ///
	MapPane* map_;
	Counter* unit_[MAXUNITS];				// table of pointers to units
	int unitline_is_selected_[MAXUNITS];	// which units are selected
	int num_units_;
	wxPen* hpenDark;
	wxPen* hpenLight;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
