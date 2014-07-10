#if defined HEXTOOLSMAP

#ifndef DCITY_H
#define DCITY_H

class City;

class DCity : public wxDialog
{
public:
	DCity( wxWindow* p, City* cityPtr, City* editedCity, wxString title = "City/text properties" );

	void setCitySize( int i )
	{
		size_ = i;
	}
	void setTxtPos( int i )
	{
		text_position_ = i;
	}
	void setCityPos( int i )
	{
		position_ = i;
	}
	void setCityName( char* n )
	{
		editName->SetValue( n );
	}
	int getCitySize( )
	{
		return size_;
	}
	int getTxtPos( )
	{
		return text_position_;
	}
	int getCityPos( )
	{
		return position_;
	}
	void getCityName( char* name );

protected:
	// common stuff
	void Ok( wxCommandEvent& );
	void Cancel( wxCommandEvent& );
	void EvInitDialog( wxInitDialogEvent& );

	void collectResults( );  // ctrls to vars

	int size_;
	int text_position_;
	int position_;
	unsigned char profile_;

	wxTextCtrl* editName;
	wxRadioButton* rdFullHex;
	wxRadioButton* rdPartialHex;
	wxRadioButton* rdMajor;
	wxRadioButton* rdMinor;
	wxRadioButton* rdPtCity;
	wxRadioButton* rdRefPt;
	wxRadioButton* rdOasis;
	wxRadioButton* rdTribalArea;
	wxRadioButton* rdBerberArea;
	wxRadioButton* rdIndianArea;
	wxRadioButton* rdCamelArea;
	wxRadioButton* rdTsetseArea;
	wxRadioButton* rdWW1OFort;
	wxRadioButton* rdWW1NFort;
	wxRadioButton* rdWW1GFort;
	wxRadioButton* rdFortess;
	wxRadioButton* rdOuvrage;
	wxRadioButton* rdWestWall1;
	wxRadioButton* rdWestWall2;
	wxRadioButton* rdWestWall3;
	wxRadioButton* rdRailyard;
	wxRadioButton* rdShipyard;
	wxRadioButton* rdWaterwayDock;
	wxRadioButton* rdFerryTerminal;
	wxRadioButton* rdMajorNavalBase;
	wxRadioButton* rdMinorNavalBase;
	wxRadioButton* rdMapText;
	wxRadioButton* rdSmallText;
	wxRadioButton* rdVolksdeutsch;
	wxRadioButton* rdAtoll;
	wxRadioButton* rdSmallIsland;

	wxRadioButton* rdCoalRes;
	wxRadioButton* rdNaturalGasRes;
	wxRadioButton* rdOilRes;
	wxRadioButton* rdOilShaleRes;
	wxRadioButton* rdPeatRes;
	wxRadioButton* rdSyntheticOilRes;
	wxRadioButton* rdHydroRes;
	wxRadioButton* rdOreRes;
	wxRadioButton* rdMineralRes;
	wxRadioButton* rdCementRes;
	wxRadioButton* rdRubberRes;
	wxRadioButton* rdSyntheticRubberRes;

	wxRadioButton* rdCityCenter;
	wxRadioButton* rdCityE;
	wxRadioButton* rdCityN;
	wxRadioButton* rdCityNE;
	wxRadioButton* rdCityNW;
	wxRadioButton* rdCityS;
	wxRadioButton* rdCitySE;
	wxRadioButton* rdCitySW;
	wxRadioButton* rdCityW;

	wxRadioButton* rdTxtCenter;
	wxRadioButton* rdTxtN;
	wxRadioButton* rdTxtE;
	wxRadioButton* rdTxtNE;
	wxRadioButton* rdTxtNW;
	wxRadioButton* rdTxtS;
	wxRadioButton* rdTxtSE;
	wxRadioButton* rdTxtSW;
	wxRadioButton* rdTxtW;

	// handlers are called when btn goes *up* too
	void BNFullHex( wxCommandEvent& ) { }
	void BNMinor( wxCommandEvent& ) { }
	void BNPartialHex( wxCommandEvent& ) { }
	void BNMajor( wxCommandEvent& ) { }
	void BNMapText( wxCommandEvent& ) { }
	void BNImpFortress( wxCommandEvent& ) { }
	void BNImpOuvrage( wxCommandEvent& ) { }
	void BNWestWall1( wxCommandEvent& ) { }
	void BNWestWall2( wxCommandEvent& ) { }
	void BNWestWall3( wxCommandEvent& ) { }
	void PtCityClicked( wxCommandEvent& ) { }
	void RefPtClicked( wxCommandEvent& ) { }
	void SmallTextClicked( wxCommandEvent& ) { }

	DECLARE_EVENT_TABLE()
};

#endif

#endif
