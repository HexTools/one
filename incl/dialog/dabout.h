#if defined HEXTOOLSPLAY
#ifndef DABOUT_H
#define DABOUT_H

class DAbout : public wxDialog
{
  public:
    DAbout( wxWindow* parent );
    virtual ~DAbout();

  protected:
    void EvInitDialog( wxInitDialogEvent& );

    DECLARE_EVENT_TABLE()
};

#endif
#endif
