//---------------------------------------------------------------------------
#ifndef EditOrderGiveFrmH
#define EditOrderGiveFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditOrderGiveForm : public TForm
{
__published:	// IDE-managed Components
    TButton *bnOk;
    TButton *bnCancel;
    TCheckBox *cbCommented;
    TCheckBox *cbRepeating;
    TComboBox *cbTargets;
    TLabel *Label1;
    TComboBox *cbItems;
    TEdit *edCount;
    TLabel *Label2;
    TCheckBox *cbShowAll;
    TButton *bnAll;
  TCheckBox *cbDiscard;
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall bnCancelClick(TObject *Sender);
    void __fastcall edCountExit(TObject *Sender);
    void __fastcall cbItemsChange(TObject *Sender);
    void __fastcall cbShowAllClick(TObject *Sender);
    void __fastcall bnAllClick(TObject *Sender);
        void __fastcall cbTargetsChange(TObject *Sender);
  void __fastcall cbDiscardClick(TObject *Sender);
private:	// User declarations
    AItems *items;
    AUnits *targets;
    AUnits *targets2;
    int curtarnum;
    AnsiString curitem;
public:		// User declarations
    __fastcall TEditOrderGiveForm(TComponent* Owner, AOrderGive *_ord, AItems *its, AUnits *tars, AUnits *tars2);
    AOrderGive *ord;
    void MakeTargetList();
};
//---------------------------------------------------------------------------
extern PACKAGE TEditOrderGiveForm *EditOrderGiveForm;
//---------------------------------------------------------------------------
#endif
