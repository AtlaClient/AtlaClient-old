//---------------------------------------------------------------------------
#ifndef OptionsFrmH
#define OptionsFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TOptionsForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TPageControl *PageControl1;
  TTabSheet *tsColors;
  TLabel *labcurregColor;
  TButton *bnOk;
  TButton *bnCancel;
  TButton *bnApply;
  TLabel *labgridColor;
  TLabel *labwallColor;
  TLabel *labtextColor;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TLabel *Label4;
  TColorDialog *ColorDialog;
  TLabel *labWedge0;
  TLabel *labWedge5;
  TLabel *labWedge4;
  TLabel *labWedge3;
  TLabel *labWedge2;
  TLabel *labWedge1;
  TLabel *Wedges;
    TTabSheet *tsOptions;
 TCheckBox *cbShowEvents;
 TCheckBox *cbRunRegionOrders;
 TLabel *Label5;
 TEdit *edTownNameLimit;
    TCheckBox *cbMaximizeWindow;
    TTabSheet *tsEvents;
    TCheckBox *cbdseDiscard;
    TCheckBox *cbdseNoMonth;
    TCheckBox *cbdseRegDataFound;
  void __fastcall bnApplyClick(TObject *Sender);
  void __fastcall labColorDblClick(TObject *Sender);
private:
    void Load();	// User declarations
public:		// User declarations
  __fastcall TOptionsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOptionsForm *OptionsForm;
//---------------------------------------------------------------------------
#endif
