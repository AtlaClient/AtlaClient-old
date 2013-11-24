//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WedgeModeSetupFrm.h"
#include "util.h"
#include "exprlang.h"
#include "region.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWedgeModeSetupForm *WedgeModeSetupForm;
//---------------------------------------------------------------------------
AnsiString wedgeexpr[6]={
 "presentfaction==localfaction",
 "guardfaction==localfaction",
 "presentfaction==10"
};
extern TColor wedgecolors[6];
__fastcall TWedgeModeSetupForm::TWedgeModeSetupForm(TComponent* Owner)
    : TForm(Owner)
{
 ExprPrepareRegion(curRegion);
 LabelReg->Caption=curRegion->FullName();
 edits[0]=Edit1;
 edits[1]=Edit2;
 edits[2]=Edit3;
 edits[3]=Edit4;
 edits[4]=Edit5;
 edits[5]=Edit6;

 labels[0]=Label1;
 labels[1]=Label2;
 labels[2]=Label3;
 labels[3]=Label4;
 labels[4]=Label5;
 labels[5]=Label6;

 colors[0]=Label11;
 colors[1]=Label12;
 colors[2]=Label13;
 colors[3]=Label14;
 colors[4]=Label15;
 colors[5]=Label16;
 for(int i=0;i<6;i++){
  edits[i]->Text=wedgeexpr[i];
  colors[i]->Color=wedgecolors[i];
 }
 CalcExpr();
 TMenuItem *mi;
 mi=new TMenuItem(PopupMenu);
 PopupMenu->Items->Add(mi);
 mi->Caption="-";
 for(int i=0;i<VarsCount;i++){
  mi=new TMenuItem(PopupMenu);
  PopupMenu->Items->Add(mi);
  mi->Caption=variables[i].name;
  mi->OnClick=ExprClick;
 }
 mi=new TMenuItem(PopupMenu);
 PopupMenu->Items->Add(mi);
 mi->Caption="-";
 for(int i=0;i<FuncsCount;i++){
  mi=new TMenuItem(PopupMenu);
  PopupMenu->Items->Add(mi);
  mi->Caption=functions[i].name+functions[i].params;
  mi->OnClick=ExprClick;
 }
}
//---------------------------------------------------------------------------
void __fastcall TWedgeModeSetupForm::bnOkClick(TObject *Sender)
{
 for(int i=0;i<6;i++){
  wedgeexpr[i]=edits[i]->Text;
 }
}
//---------------------------------------------------------------------------
void TWedgeModeSetupForm::CalcExpr()
{
 int i;
 try{
  for(i=0;i<6;i++){
   bool res=ExprProcessExpr(edits[i]->Text);
   labels[i]->Caption=res?"true":"false";
  }
 }catch (Exception &exception){
  Application->ShowException(&exception);
  if(Visible) edits[i]->SetFocus();
 }
}



void __fastcall TWedgeModeSetupForm::EditEnter(TObject *Sender)
{
 lastedit=((TComponent*)Sender)->Tag;
}
//---------------------------------------------------------------------------

void __fastcall TWedgeModeSetupForm::EditExit(TObject *Sender)
{
// CalcExpr();
}
//---------------------------------------------------------------------------

void __fastcall TWedgeModeSetupForm::EditKeyPress(TObject *Sender,
      char &Key)
{
 if(Key==13){
  EditExit(Sender);
  Key=0;
 }
}
//---------------------------------------------------------------------------

void __fastcall TWedgeModeSetupForm::bnExprClick(TObject *Sender)
{
 TPoint p(bnExpr->Left,bnExpr->Top+bnExpr->Height);
 p=ClientToScreen(p);
 PopupMenu->Popup(p.x,p.y);
}
//---------------------------------------------------------------------------

void __fastcall TWedgeModeSetupForm::ExprClick(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 AnsiString str=mi->Caption;
 if(str=="&&&&") str="&&";
 edits[lastedit]->SelText=str;
 int start=edits[lastedit]->SelStart;
 edits[lastedit]->SetFocus();
 edits[lastedit]->SelStart=start;
}
//---------------------------------------------------------------------------

void __fastcall TWedgeModeSetupForm::bnVerifyClick(TObject *Sender)
{
 CalcExpr();
}
//---------------------------------------------------------------------------

