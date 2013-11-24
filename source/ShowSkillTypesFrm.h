//---------------------------------------------------------------------------
#ifndef ShowSkillTypesFrmH
#define ShowSkillTypesFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TShowSkillTypesForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TButton *bnEdit;
    TListBox *List;
    TButton *bnCancel;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall bnEditClick(TObject *Sender);
 void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
 void __fastcall bnCancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TShowSkillTypesForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TShowSkillTypesForm *ShowSkillTypesForm;
//---------------------------------------------------------------------------
#endif
