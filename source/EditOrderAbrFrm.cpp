//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "util.h"
#include "orders.h"
#include "EditOrderAbrFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditOrderAbrForm *EditOrderAbrForm;
//---------------------------------------------------------------------------
__fastcall TEditOrderAbrForm::TEditOrderAbrForm(TComponent* Owner, AOrderAbr *_ord,TStringList *names, TStringList *abrs)
    : TForm(Owner),ord(_ord),namelist(names),abrlist(abrs)
{
 cbCommented->Checked=ord->commented;
 cbRepeating->Checked=ord->repeating;
 cbList->Items->Assign(names);
 cbList->ItemIndex=abrlist->IndexOf(ord->abr);
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderAbrForm::bnOkClick(TObject *Sender)
{
 int ind=cbList->ItemIndex;
 ord->commented=cbCommented->Checked;
 ord->repeating=cbRepeating->Checked;
 if(ind!=-1){
  ord->abr=abrlist->Strings[ind];
 }
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderAbrForm::bnCancelClick(TObject *Sender)
{
 ord=0;
}


