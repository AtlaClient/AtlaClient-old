//---------------------------------------------------------------------------
#ifndef LinkShaftFrmH
#define LinkShaftFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TLinkShaftForm : public TForm
{
__published:	// IDE-managed Components
    TListBox *List;
    TButton *bnLink;
    TButton *bnCancel;
    void __fastcall bnLinkClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
    AObject *obj;
public:		// User declarations
    __fastcall TLinkShaftForm(TComponent* Owner,AObject *_obj);
};
//---------------------------------------------------------------------------
extern PACKAGE TLinkShaftForm *LinkShaftForm;
//---------------------------------------------------------------------------
#endif
