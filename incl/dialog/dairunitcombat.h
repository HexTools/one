#if defined HEXTOOLSPLAY
#ifndef DAIRUNITCOMBAT_H
#define DAIRUNITCOMBAT_H

class AirUnit;

struct DAirUnitCombatXfer
{
	char lblInfo[255];
};

class DAirUnitCombat : public wxDialog
{
public:
	static const unsigned short MAX_PLANES = 200;

	DAirUnitCombat( wxWindow* parent );
	virtual ~DAirUnitCombat( );

	// before executing: add participating planes with these funcs:
	void AddTarget( AirUnit* au );
	void AddEscort( AirUnit* au );
	void AddInterceptor( AirUnit* au );

	// after executing (with IDOK result) these planes were shot down,
	// aborted/returned planes are modified by the dialog.
	AirUnit* killed_plane_list_[MAX_PLANES * 3];

	int	num_killed_planes_;

protected:
	void CalcGeometry( void );
	// AirCrt: noppa=0 meaning computer will throw dice
	int AirCrt( AirUnit* att, AirUnit* def, int noppa1 = 0, int noppa2 = 0 );
	int GetAirMods( AirUnit* att, AirUnit* def ); // die modifier
	int IsEscortAttacked( int n );
	int IsInterceptorAttacked( int n );
	int IsAttackingBombers( int n );// is interceptor attacking bombers
	void GetDiffAndMods( AirUnit* att, AirUnit* def, int* diff, int* mods );
	int FightAirCombat( AirUnit* att, AirUnit* def, int no_def_shoot = 0 );
	void ApplyResult( AirUnit* au, int res );
	bool CanBeAttacked( AirUnit* au );// has unit already been damaged/returned
	void KillPlane( AirUnit* au, bool only_aborted = false );

	int get_i_idx( int x, int y );
	int get_t_idx( int x, int y );
	int get_e_idx( int x, int y );
	void get_i_xy( int i, int* x, int* y );
	void get_t_xy( int i, int* x, int* y );
	void get_e_xy( int i, int* x, int* y );

	wxDC* dc_;

	AirUnit* target_list_[MAX_PLANES]; // ptrs to target planes
	AirUnit* escort_list_[MAX_PLANES]; // ptrs to escorts
	AirUnit* interceptor_list_[MAX_PLANES]; // ptrs to interceptors
	int num_targets_;
	int num_escorts_;
	int num_interceptors_;

	// indexes of target bombers/escorts, 0..MAX_PLANES = bombers,
	// MAX_PLANES..2*MAX_PLANES = escorts
	int int_target_[MAX_PLANES*2];
	int esc_target_[MAX_PLANES]; // indexes of target interceptors
	int selected_interceptor_; // currently selected interceptor ( -1 = none )

	int target_x_;
	int target_y_;
	int target_width_;
	int target_height_;

	int escort_x_;
	int escort_y_;
	int escort_width_;
	int escort_height_;

	int interceptor_x_;
	int interceptor_y_;
	int interceptor_width_;
	int interceptor_height_;

public:
	void SetupDialog( );
	void CloseDialog( );

protected:
	void EvInitDialog( wxInitDialogEvent& );
	void EvLButtonDown( wxMouseEvent& e );
	void Paint( wxPaintEvent& );
	void EvSize( wxSizeEvent& event );
	void CancelClicked( wxCommandEvent& );
	void OkClicked( wxCommandEvent& );

protected:
	wxButton* btnOk;
	wxButton* btnCancel;
	wxStaticText* lblInfo;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
