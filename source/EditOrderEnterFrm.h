//---------------------------------------------------------------------------
#ifndef EditOrderEnterFrmH
#define EditOrderEnterFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditOrderEnterForm : public TForm
{
__published:	// IDE-managed Components
    TButton *bnOk;
    TButton *bnCancel;
    TCheckBox *cbCommented;
    TCheckBox *cbRepeating;
    TComboBox *cbTargets;
    TLabel *Label1;
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall bnCancelClick(TObject *Sender);
private:	// User declarations
    TList *objects;
public:		// User declarations
    __fastcall TEditOrderEnterForm(TComponent* Owner, AOrderInt *_ord, TList *objs);
    AOrderInt *ord;
};
//---------------------------------------------------------------------------
extern PACKAGE TEditOrderEnterForm *EditOrderEnterForm;
//---------------------------------------------------------------------------
#endif
