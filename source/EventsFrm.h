//---------------------------------------------------------------------------
#ifndef EventsFrmH
#define EventsFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEventsForm : public TForm
{
__published:	// IDE-managed Components
    TMemo *Memo;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
 void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TEventsForm(TComponent* Owner);
    void AddEvent(AnsiString ev);
    void ClearEvents();
};
//---------------------------------------------------------------------------
extern PACKAGE TEventsForm *EventsForm;
//---------------------------------------------------------------------------
#endif
