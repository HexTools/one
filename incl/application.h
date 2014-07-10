//
// The author disclaims copyright to this source code.
//
// Like a well, a bellows, or your mother, it will always
// be available to you in capability and possibility.
//
// You can use it any way you want.
//

#ifndef APPLICATION_H
#define APPLICATION_H

class Frame;

class Application : public wxApp
{
public:
	static const std::string NAME;
	static const std::string VERSION;
	static const std::string AUTHOR;
	static const std::string FOLDER;
	static const std::string HEX_BITMAPS_PATH;
	static const std::string HEXSIDE_BITMAPS_PATH;
	static const std::string SYMBOL_BITMAPS_PATH;
#if defined HEXTOOLSPLAY
	static const std::string CRT_FILENAME;
	static const std::string TEC_FILENAME;
	static const std::string UIC1_FILENAME;
	static const std::string UIC2_FILENAME;
	static const std::string UCC1_FILENAME;
	static const std::string UCC2_FILENAME;
#endif
	Frame* frame_;
	virtual bool OnInit( );
#if defined HEXTOOLSPLAY
	void OnKey( wxKeyEvent& event );
	void InitRules( );

	DECLARE_EVENT_TABLE()
#endif
};

#endif

