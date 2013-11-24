//---------------------------------------------------------------------------
#ifndef RegInfoFrmH
#define RegInfoFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TRegInfoForm : public TForm
{
__published:	// IDE-managed Components
        TMemo *Memo;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormDestroy(TObject *Sender);
private:
    bool FShowRegInfo;
    void __fastcall SetShowRegInfo(bool value);	// User declarations
    int oxloc, oyloc,ozloc;
public:		// User declarations
        __fastcall TRegInfoForm(TComponent* Owner);
    void UpdateRegionInfo(int xloc,int yloc,int zloc);
    void AddLine(const AnsiString & str, int & w, int & h);
    __property bool ShowRegInfo  = { read = FShowRegInfo, write = SetShowRegInfo };
};
//---------------------------------------------------------------------------
extern PACKAGE TRegInfoForm *RegInfoForm;
//---------------------------------------------------------------------------
#endif
