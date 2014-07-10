#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dabout.h"
#include "hextools.h"
#include "application.h"

const char* PostCardTxt =	\
"HT was originally developed by Janne Viitala.\n"
"He spent a lot of time developing JET without\n"
"asking more rewards than a simple postcard.\n"
"To learn more about it see the Help File. See\n"
"http://groups.yahoo.com/jet-users for the\n"
"latest information on HexTools or to ask\n"
"questions and make suggestions.\n"
"Posting requires a no-cost Yahoo Groups\n"
"registration.";

const char* mbox3232[] =
 {
 /* width height num_colors chars_per_pixel */
 "    32    32        6            1",
 /* colors */
 ". c #000000", "# c #808000", "a c #808080", "b c none", "c c #ff0000", "d c #ffffff",
 /* pixels */
 "bbbbbbb.....bbbbbbbbbbbbbbbbbbbb", "bbbbb..bbbaa..bbbbbbbbbbbbbbbbbb", "bbbb.adbbbbbaa..bbbbbbbbbbbbbbbb",
 "bbbb.dbdbdbbbbaa..bbbbbbbbbbbbbb", "bbb.dddddbdbdbbbaa..bbbbbbbbbbbb", "bbb.ddddddddbdbbbbaa..bbbbbbbbbb",
 "bb.dbddddddddbdbdbbbaa..bbbbbbbb", "bb.bdbdbddddddddbdbbbbaa..bbbbbb", "bb.dbdbdbdbddddddbdbdb..ba.bbbbb",
 "bb.bbbbbdbdbddddddddb.ababa.bbbb", "bb.bb..abbbdbdbddddd.ababab.bbbb", "bb.bb.c..abbdbdbdbdb.bab..ab.bbb",
 "bb.bb.cc...abbbdbdb.bab.aa.a.bbb", "bb.bb.cc.....abbdbd.aba.bba..bbb", "bb.bb.cc.ba....abdb.baba.da..bbb",
 "bb.bb..c.bbba....ab.abab.da..bbb", "bb..bba..bbbbba..bb.babab..a.bbb", "bbbb..bbbbbbbbbbabb.abababab.bbb",
 "bbbbbb..bbbbbbbbbbb.babababa.bbb", "bbbbbbbb..bbbbbbbbb.abababab.bbb", "bbbbbbbbb...bbbbbbb.bababab..bbb",
 "bbbbbbbbb.##..bbbbb.ababa..bbbbb", "bbbbbbbbb.###...bbb.bab..bbbbbbb", "bbbbbbbbb.###..a..b.a..bbbbbbbbb",
 "bbbbbbbbb.###.a.a....bbbbbbbbbbb", "bbbbbbbbb.###..a..bbbbbbbbbbbbbb", "bbbbbbbbb.###.a.a.bbbbbbbbbbbbbb",
 "bbbbbbbbb.###..a..bbbbbbbbbbbbbb", "bbbbbbbbb.###.a.a.bbbbbbbbbbbbbb", "bbbbbbbbb.###..a..bbbbbbbbbbbbbb",
 "bbbbbbbbbbb##.a.a.bbbbbbbbbbbbbb", "bbbbbbbbbbbbb..abbbbbbbbbbbbbbbb"
};

#if 0
#include "icon.xpm"
// Platform specific setting
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "icon.xpm"
#endif
#endif

BEGIN_EVENT_TABLE(DAbout, wxDialog)
EVT_INIT_DIALOG(DAbout::EvInitDialog)
END_EVENT_TABLE()

static const char* icon_xpm[] = {
#if defined HEXTOOLSMAP
/* width height num_colors chars_per_pixel */
"    32    32       10            1",
/* colors */
"h c #c6c6c6",
"e c #848400",
"g c #00ff00",
"c c #000000",
"# c #848484",
"d c #840000",
"a c #0000ff",
"f c #008400",
". c #ffffff",
"b c none",
/* pixels */
"......#........aabbbbbbbbbbbbbbb",
".c....a.........aabbbbbbbbcddcbb",
"..c..aaaa......aaabbbbbbbcddddcb",
"...caaa#aa....aaa##bbbbbceeeddcb",
"#.aac....aaaaaa....##bbceeeedcbb",
".aa.cfgf..aaaaa......#ceeeeccbbb",
".a.g.c...g.#.........ceeeeccbbbb",
".a.f.gcf.f.#........ceeeeccbbbbb",
".#a..c.....#.......ceeeeccbbbbbb",
".aag.c..f..#......ceeeeccbbbbbbb",
"aa..c.f....#.....ceeeeccbbbbbbbb",
"aa##c....##.##..ceeeecccbbbbbbbb",
"...c##.##.....#ceeeeccccccbbbbbb",
"...c..#.......ceeeeccccccc#bbbbb",
"..c...#.......ceeeccccccc.#bbbbb",
"..c...#......chhecccccc...#bbbbb",
".c....#......chhccccc.....#bbbbb",
"..c...#.....cchcchh.......#bbbbb",
"..c...#.....ccc.#.........#bbbbb",
"...c..#....cc...#.........#bbbbb",
"....c#.##.....##.##.....##.##bbb",
"#.##c....##.##.....##.##.....##b",
"b#...c.....#.........#.........#",
"b#...cc....#.........#.........#",
"b#..cccc...#.........#.........#",
"b#..cccc...#.........#.........#",
"b#...cc....#.........#.........#",
"b#.........#.........#.........#",
"bb##.....##b##.....##b##.....##b",
"bbbb##.##bbbbb##.##bbbbb##.##bbb",
"bbbbbb#bbbbbbbbb#bbbbbbbbb#bbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
#elif defined HEXTOOLSPLAY
/* width height num_colors chars_per_pixel */
"    32    32        3            1",
/* colors */
". c #0000ff",
"# c none",
"a c #ffffff",
/* pixels */
"################################",
"#############.......############",
"##########...aaaaaaa...#########",
"########..aaaaaaaaaaaaa..#######",
"#######.aaaaaaaaaaaaaaaa..######",
"######.aaaaaaaaaaaaaaaaaaa.#####",
"#####.aaaaaaaaaaaaaaaaaaaaa.####",
"####.aaaaaaaaaaaaaaaaaaa.aaa.###",
"###.aaaaaaaaaaaaa..aaaaa..aaa.##",
"##.aaaaaaaaaaaaa.....aa...aaa.##",
"##.aaaaaaaaaaaa.....aa....aaaa.#",
"##.aaaaaaaaaaa...a..a.....aaaa.#",
"#.aaaaaaa.aaa...a..........aaa.#",
"#.aaaaa...aaaa..aaa........aaaa.",
"#.aaaaaaa.aaa...aa..........aaa.",
"#.aaaaaa..aaaaaa............aaa.",
"#.aaaaa.aa...................aa.",
"#.aaaaaa..................aaaaa.",
"#.aaa.aa...................aaaa.",
"#.aa......................aaaaa.",
"#.a......aa..a.....a.a...aaaaaa.",
"##.a....aaaa.a.....aaa....aaaa.#",
"##.aaaaaaa.aa.a...aaaaa...aaaa.#",
"##.aaaaaaaaaa.a.aaaaaaaa...aaa.#",
"###.aaaaaaaaaaa..aaaaaaaaaaaa.##",
"####.aaaaaaaaaaaaa.aaaaaaaaa.###",
"####.aaaaaaaaaaaaaaaaaaaaaa.####",
"#####.aaaaaaaaaaaaaaaaaaaa..####",
"######..aaaaaaaaaaaaaaaaa..#####",
"########.aaaaaaaaaaaaaa..#######",
"#########...aaaaaaaaa...########",
"############.........###########"
#endif
};

DAbout::DAbout( wxWindow* parent )
		: wxDialog( parent, -1, wxT( "About " + Application::NAME ), wxDefaultPosition )
{
	wxBitmap bmp( icon_xpm );
	wxBitmap bmpmbox( mbox3232 );

	wxStaticBox* item1 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item0 = new wxStaticBoxSizer( item1, wxVERTICAL );
	wxSizer* item2 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBitmap* item3 = new wxStaticBitmap( this, -1, bmp, wxDefaultPosition, wxDefaultSize );
	item2->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 2 );

	wxSizer* item4 = new wxBoxSizer( wxVERTICAL );
	wxStaticText* item5 = new wxStaticText( this, -1, wxT("HexTools Wargame Utilities"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item5, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	wxSizer* item6 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item7 = new wxStaticText( this, -1, wxT("Version"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item7, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	wxStaticText* item8 = new wxStaticText( this, -1, Application::VERSION, wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item8, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	item4->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	item2->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 2 );

	wxStaticBitmap* item9 = new wxStaticBitmap( this, -1, bmp, wxDefaultPosition, wxDefaultSize );
	item2->Add( item9, 0, wxALIGN_CENTRE | wxALL | 0, 2 );

	item0->Add( item2, 0, wxALIGN_CENTRE | wxALL/*|wxGROW*/, 2 );
	wxSizer* item10 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* item11 = new wxStaticText( this, -1,
			wxT("The authors disclaim copyright to the source code."),
			wxDefaultPosition, wxDefaultSize, 0 );
	item10->Add( item11, 0, wxALIGN_CENTRE | wxALL | 0, 2 );

	wxStaticText* item12 = new wxStaticText( this, -1,
			wxT("HexTools is free software, unlicensed into the public domain."), wxDefaultPosition,
			wxDefaultSize, wxST_NO_AUTORESIZE );
	item10->Add( item12, 0, wxALIGN_CENTRE | wxALL | 0, 2 );

	wxStaticText* item13 = new wxStaticText( this, -1, wxT("See file 'UNLICENSE.TXT' for more details."),
			wxDefaultPosition, wxDefaultSize, 0 );
	item10->Add( item13, 0, wxALIGN_CENTRE | wxALL | 0, 2 );

	item0->Add( item10, 0, wxALIGN_CENTRE | wxALL | 0, 2 );

	wxStaticBox* sb = new wxStaticBox( this, -1, wxT("") );
	wxSizer* itemhs = new wxStaticBoxSizer( sb, wxHORIZONTAL );
	wxStaticBitmap* bmp2 = new wxStaticBitmap( this, -1, bmpmbox, wxDefaultPosition, wxDefaultSize );
	itemhs->Add( bmp2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item3b = new wxBoxSizer( wxVERTICAL );
	wxStaticText* item4b = new wxStaticText( this, -1, wxString::FromAscii( PostCardTxt ), wxDefaultPosition,
			wxSize( 300, -1 ), 0 );
	item3b->Add( item4b, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	itemhs->Add( item3b, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( itemhs, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
#if 0
	 wxStaticBox* sb2 = new wxStaticBox(this, -1, "");
	 wxSizer* itemhs2 = new wxStaticBoxSizer(sb2, wxHORIZONTAL);
	 wxStaticText* item5b = new wxStaticText(this, -1,
	 "Visit HexTools home page at http://groups.yahoo.com/group/jet-users/",
	 wxDefaultPosition, wxSize(350,-1), wxALIGN_CENTRE );
	 itemhs2->Add(item5b, 0, wxALIGN_CENTRE|wxALL|0, 5);
	 item0->Add(itemhs2, 0, wxALIGN_CENTRE|wxALL|0, 2);
#endif
	wxButton* item14 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item0->Add( item14, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );
}

DAbout::~DAbout( )
{
}

void DAbout::EvInitDialog( wxInitDialogEvent& )
{
}

#endif
