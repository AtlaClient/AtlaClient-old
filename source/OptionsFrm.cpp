//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "util.h"
#include "OptionsFrm.h"
#include "MainFrm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOptionsForm *OptionsForm;
//---------------------------------------------------------------------------
extern TColor curregColor,wallColor;
extern TColor gridColor,textColor;
extern TColor wedgecolors[6];

__fastcall TOptionsForm::TOptionsForm(TComponent* Owner)
  : TForm(Owner)
{
 Load();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsForm::bnApplyClick(TObject *Sender)
{
 curregColor=labcurregColor->Color;
 wallColor=labwallColor->Color;
 gridColor=labgridColor->Color;
 textColor=labtextColor->Color;
 wedgecolors[0]=labWedge0->Color;
 wedgecolors[1]=labWedge1->Color;
 wedgecolors[2]=labWedge2->Color;
 wedgecolors[3]=labWedge3->Color;
 wedgecolors[4]=labWedge4->Color;
 wedgecolors[5]=labWedge5->Color;
 optShowEvents=cbShowEvents->Checked;
 optRunRegionOrders=cbRunRegionOrders->Checked;
 optMaximizeWindow=cbMaximizeWindow->Checked;
 optTownNameLimit=atoi(edTownNameLimit->Text.c_str());
 SetBits(optDontShowEvents,dseDiscard,cbdseDiscard->Checked);
 SetBits(optDontShowEvents,dseNoMonth,cbdseNoMonth->Checked);
 SetBits(optDontShowEvents,dseRegDataFound,cbdseRegDataFound->Checked);
 MainForm->DrawMap();
}
//---------------------------------------------------------------------------
void TOptionsForm::Load()
{
 labcurregColor->Color=curregColor;
 labwallColor->Color=wallColor;
 labgridColor->Color=gridColor;
 labtextColor->Color=textColor;

 labWedge0->Color=wedgecolors[0];
 labWedge1->Color=wedgecolors[1];
 labWedge2->Color=wedgecolors[2];
 labWedge3->Color=wedgecolors[3];
 labWedge4->Color=wedgecolors[4];
 labWedge5->Color=wedgecolors[5];

 cbShowEvents->Checked=optShowEvents;
 cbRunRegionOrders->Checked=optRunRegionOrders;
 cbMaximizeWindow->Checked=optMaximizeWindow;
 edTownNameLimit->Text=optTownNameLimit;

 cbdseDiscard->Checked=optDontShowEvents&dseDiscard;
 cbdseNoMonth->Checked=optDontShowEvents&dseNoMonth;
 cbdseRegDataFound->Checked=optDontShowEvents&dseRegDataFound;

}
void __fastcall TOptionsForm::labColorDblClick(TObject *Sender)
{
 TLabel *lab=dynamic_cast<TLabel*>(Sender);
 if(!lab) return;
 ColorDialog->Color=lab->Color;
 if(ColorDialog->Execute()){
  lab->Color=ColorDialog->Color;
 }
}
//---------------------------------------------------------------------------

