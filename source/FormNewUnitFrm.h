//---------------------------------------------------------------------------
#ifndef FormNewUnitFrmH
#define FormNewUnitFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormNewUnitForm : public TForm
{
__published:	// IDE-managed Components
  TButton *bnOk;
  TButton *bnCancel;
  TLabel *Label1;
  TEdit *edAlias;
  TLabel *Label2;
  TLabel *Label3;
  TEdit *edName;
  TEdit *edLocalDescr;
  TLabel *Label4;
  TEdit *edDescr;
    TCheckBox *cbGuard;
    TCheckBox *cbAvoid;
    TRadioGroup *rgReveal;
    TCheckBox *cbBehind;
    TCheckBox *cbAutoTax;
    TCheckBox *cbHold;
    TCheckBox *cbNoAid;
    TRadioGroup *rgConsume;
  void __fastcall edAliasKeyPress(TObject *Sender, char &Key);
    void __fastcall cbGuardClick(TObject *Sender);
    void __fastcall cbAvoidClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall bnOkClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TFormNewUnitForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFormNewUnitForm *FormNewUnitForm;
//---------------------------------------------------------------------------
#endif
