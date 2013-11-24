//---------------------------------------------------------------------------

#ifndef SaveMapFrmH
#define SaveMapFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TSaveMapForm : public TForm
{
__published:	// IDE-managed Components
    TButton *bnOk;
    TButton *bnCancel;
    TCheckBox *cbAllRegion;
    TCheckBox *cbRegionInfo;
    TCheckBox *cbUserObject;
    TCheckBox *cbNonuserObject;
    TCheckBox *cbLocalUnit;
    TCheckBox *cbLocalAsNonlocalUnit;
    TCheckBox *cbNonlocalUnit;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall cbLocalUnitClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TSaveMapForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TSaveMapForm *SaveMapForm;
//---------------------------------------------------------------------------
#endif
