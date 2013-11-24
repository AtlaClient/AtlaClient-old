//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include "util.h"
#include "orders.h"
#include "EditOrderEnterFrm.h"
#include "region.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditOrderEnterForm *EditOrderEnterForm;
//---------------------------------------------------------------------------
__fastcall TEditOrderEnterForm::TEditOrderEnterForm(TComponent* Owner, AOrderInt *_ord, TList *objs)
    : TForm(Owner),ord(_ord),objects(objs)
{
 cbCommented->Checked=ord->commented;
 cbRepeating->Checked=ord->repeating;

 AnsiString s;
 cbTargets->Items->BeginUpdate();
 for(int i=0;i<objects->Count;i++){
  AObject *obj=(AObject *)objects->Items[i];
  s=obj->ObjectHeader(false);
  cbTargets->Items->Add(s);
  if(obj->num==ord->value){
   cbTargets->ItemIndex=i;
  }
 }
 cbTargets->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderEnterForm::bnOkClick(TObject *Sender)
{

 ord->commented=cbCommented->Checked;
 ord->repeating=cbRepeating->Checked;

 int indtar=cbTargets->ItemIndex;
 if(indtar==-1)return;
 AObject *obj=(AObject *)objects->Items[indtar];
 ord->value=obj->num;
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderEnterForm::bnCancelClick(TObject *Sender)
{
 ord=0;
}
