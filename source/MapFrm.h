//---------------------------------------------------------------------------
#ifndef MapFrmH
#define MapFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMapForm : public TForm
{
__published:	// IDE-managed Components
        TImage *Image;
        void __fastcall ImageMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ImageMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
private:	// User declarations
        int oldzloc;
    bool FShowMap;
    int FMul;
    void __fastcall SetShowMap(bool value);
    void __fastcall SetMul(int value);
public:		// User declarations
        __fastcall TMapForm(TComponent* Owner);
    void UpdateMap(int zloc);
    void ResetMap();
    void PointToCoord(int x, int y, int & xloc, int& yloc);
    __property bool ShowMap  = { read = FShowMap, write = SetShowMap };
    __property int Mul  = { read = FMul, write = SetMul };
protected:
    void CreateMap(int zloc);
};
//---------------------------------------------------------------------------
extern PACKAGE TMapForm *MapForm;
//---------------------------------------------------------------------------
#endif
