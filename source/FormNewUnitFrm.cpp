//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FormNewUnitFrm.h"
#include "util.h"
#include "unit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFormNewUnitForm *FormNewUnitForm;
//---------------------------------------------------------------------------
__fastcall TFormNewUnitForm::TFormNewUnitForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormNewUnitForm::edAliasKeyPress(TObject *Sender,
      char &Key)
{
 if(Key==8) return;
 if(Key<'0'||Key>'9') Key=0; 
}
//---------------------------------------------------------------------------
void __fastcall TFormNewUnitForm::cbGuardClick(TObject *Sender)
{
 if(cbGuard->Checked)
  cbAvoid->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormNewUnitForm::cbAvoidClick(TObject *Sender)
{
 if(cbAvoid->Checked)
  cbGuard->Checked=false;
}
//---------------------------------------------------------------------------


void __fastcall TFormNewUnitForm::FormShow(TObject *Sender)
{
 cbGuard->Checked=(curUnit->endguard==GUARD_GUARD);
 cbAvoid->Checked=(curUnit->endguard==GUARD_AVOID);
 if(curUnit->endreveal==REVEAL_FACTION){
  rgReveal->ItemIndex=2;
 }else if(curUnit->endreveal==REVEAL_UNIT){
  rgReveal->ItemIndex=1;
 }else{
  rgReveal->ItemIndex=0;
 }
 cbBehind->Checked=(curUnit->GetEndFlag(FLAG_BEHIND));
 cbAutoTax->Checked=(curUnit->GetEndFlag(FLAG_AUTOTAX));
 cbHold->Checked=(curUnit->GetEndFlag(FLAG_HOLDING));
 cbNoAid->Checked=(curUnit->GetEndFlag(FLAG_NOAID));
 if(curUnit->GetEndFlag(FLAG_CONSUMING_FACTION)){
  rgConsume->ItemIndex=2;
 }else if(curUnit->GetEndFlag(FLAG_CONSUMING_UNIT)){
  rgConsume->ItemIndex=1;
 }else{
  rgConsume->ItemIndex=0;
 }
}

void __fastcall TFormNewUnitForm::bnOkClick(TObject *Sender)
{
 int num=atoi(edAlias->Text.c_str());
 if(num<=0){
  Application->MessageBox("Bad alias","Error creating unit",MB_OK);
  ModalResult=mrNone;
 }
}
//---------------------------------------------------------------------------

