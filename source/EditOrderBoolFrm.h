//---------------------------------------------------------------------------
#ifndef EditOrderBoolFrmH
#define EditOrderBoolFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditOrderBoolForm : public TForm
{
__published:	// IDE-managed Components
    TButton *bnOk;
    TButton *bnCancel;
    TLabel *labInfo;
    TCheckBox *cbCommented;
    TCheckBox *cbRepeating;
    TRadioButton *rbOn;
    TRadioButton *rbOff;
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall bnCancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TEditOrderBoolForm(TComponent* Owner, AOrderByte *_ord);
    AOrderByte *ord;
};
//---------------------------------------------------------------------------
extern PACKAGE TEditOrderBoolForm *EditOrderBoolForm;
//---------------------------------------------------------------------------
#endif
