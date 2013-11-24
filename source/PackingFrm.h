//---------------------------------------------------------------------------
#ifndef PackingFrmH
#define PackingFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TPackingForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
 TButton *bnApply;
    TButton *bnCancel;
 TStringGrid *Grid;
 TButton *bnUndo;
        TButton *bnDelete;
 void __fastcall bnApplyClick(TObject *Sender);
 void __fastcall GridDblClick(TObject *Sender);
 void __fastcall GridKeyPress(TObject *Sender, char &Key);
 void __fastcall bnUndoClick(TObject *Sender);
        void __fastcall bnDeleteClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TPackingForm(TComponent* Owner);
    void RecallWidths();
    void UpdateTable();
};
//---------------------------------------------------------------------------
extern PACKAGE TPackingForm *PackingForm;
//---------------------------------------------------------------------------
#endif
