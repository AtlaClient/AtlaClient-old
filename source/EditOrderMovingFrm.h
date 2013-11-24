//---------------------------------------------------------------------------
#ifndef EditOrderMovingFrmH
#define EditOrderMovingFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditOrderMovingForm : public TForm
{
__published:	// IDE-managed Components
    TButton *bnOk;
    TButton *bnCancel;
    TLabel *labLoc;
    TCheckBox *cbCommented;
    TCheckBox *cbRepeating;
    TListBox *lbDirs;
    TButton *bnDelete;
    TListBox *lbAvDirs;
    TLabel *Label1;
    TEdit *edCustom;
    TLabel *Label2;
    TComboBox *cbType;
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall bnCancelClick(TObject *Sender);
    void __fastcall bnDeleteClick(TObject *Sender);
    void __fastcall lbAvDirsDblClick(TObject *Sender);
    void __fastcall cbCommentedClick(TObject *Sender);
    void __fastcall cbTypeChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TEditOrderMovingForm(TComponent* Owner, AOrderMoving *_ord);
    AOrderMoving *ord;
    TList *olddirs,*avdirs;
    int oldcomm;
    OrderTypes oldtype;
    virtual __fastcall ~TEditOrderMovingForm();
    void Show();
};
//---------------------------------------------------------------------------
extern PACKAGE TEditOrderMovingForm *EditOrderMovingForm;
//---------------------------------------------------------------------------
#endif
