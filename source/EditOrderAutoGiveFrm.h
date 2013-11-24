//---------------------------------------------------------------------------
#ifndef EditOrderAutoGiveFrmH
#define EditOrderAutoGiveFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
//---------------------------------------------------------------------------
class TEditOrderAutoGiveForm : public TForm
{
__published:	// IDE-managed Components
    TButton *bnOk;
    TButton *bnCancel;
    TComboBox *cbItems;
    TRadioButton *rbExcept;
    TRadioButton *rbAll;
    TRadioButton *rbNumber;
    TEdit *edNumber;
    TEdit *edBegStr;
    TLabel *Label1;
    TEdit *edSubStr;
    TLabel *Label2;
    TActionList *ActionList1;
    TAction *Action1;
    TAction *Action2;
    TLabel *Label3;
    TAction *Action3;
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall bnCancelClick(TObject *Sender);
    void __fastcall rbExceptClick(TObject *Sender);
    void __fastcall rbAllClick(TObject *Sender);
    void __fastcall rbNumberClick(TObject *Sender);
    void __fastcall Action1Execute(TObject *Sender);
    void __fastcall Action2Execute(TObject *Sender);
    void __fastcall Action3Execute(TObject *Sender);
private:	// User declarations
    TStringList *namelist, *abrlist;
public:		// User declarations
    __fastcall TEditOrderAutoGiveForm(TComponent* Owner, AOrderAutoGive *_ord);
    AOrderAutoGive *ord;
};
//---------------------------------------------------------------------------
extern PACKAGE TEditOrderAutoGiveForm *EditOrderAutoGiveForm;
//---------------------------------------------------------------------------
#endif
