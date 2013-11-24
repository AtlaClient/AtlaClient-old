//---------------------------------------------------------------------------
#ifndef WedgeModeSetupFrmH
#define WedgeModeSetupFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
extern AnsiString wedgeexpr[6];
class TWedgeModeSetupForm : public TForm
{
__published:	// IDE-managed Components
    TEdit *Edit1;
    TButton *bnOk;
    TEdit *Edit2;
    TEdit *Edit3;
    TEdit *Edit4;
    TEdit *Edit5;
    TEdit *Edit6;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TButton *bnCancel;
    TButton *bnExpr;
    TPopupMenu *PopupMenu;
    TMenuItem *N1;
    TMenuItem *N2;
    TMenuItem *N3;
    TMenuItem *N4;
    TMenuItem *N5;
    TMenuItem *N6;
    TMenuItem *N7;
    TButton *bnVerify;
    TLabel *Label11;
    TLabel *Label12;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *LabelReg;
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall EditEnter(TObject *Sender);
    void __fastcall EditExit(TObject *Sender);
    void __fastcall EditKeyPress(TObject *Sender, char &Key);
    void __fastcall bnExprClick(TObject *Sender);
    void __fastcall ExprClick(TObject *Sender);
    void __fastcall bnVerifyClick(TObject *Sender);
private:	// User declarations
    TLabel *labels[6];
    TLabel *colors[6];
    TEdit *edits[6];
    int lastedit;
public:		// User declarations
    __fastcall TWedgeModeSetupForm(TComponent* Owner);
    void CalcExpr();
};
//---------------------------------------------------------------------------
extern PACKAGE TWedgeModeSetupForm *WedgeModeSetupForm;
//---------------------------------------------------------------------------
#endif
