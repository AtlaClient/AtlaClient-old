//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include "util.h"
#include "orders.h"
#include "markets.h"
#include "unit.h"
#include "EditOrderBuySellFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditOrderBuySellForm *EditOrderBuySellForm;
//---------------------------------------------------------------------------
__fastcall TEditOrderBuySellForm::TEditOrderBuySellForm(TComponent* Owner, AOrderBuySell *_ord, AMarkets *marks)
    : TForm(Owner),ord(_ord),markets(marks)
{
 cbCommented->Checked=ord->commented;
 cbRepeating->Checked=ord->repeating;
 edCount->Text=ord->amount;

 AnsiString s;
 cbItems->Items->BeginUpdate();
 foreach(markets){
  AMarket *mt=*(AMarket**)iter;
  s=mt->Print();
  cbItems->Items->Add(s);
  if(mt->type==ord->item){
   cbItems->ItemIndex=cbItems->Items->Count-1;
  }
 }
 cbItems->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderBuySellForm::bnOkClick(TObject *Sender)
{
 int inditem=cbItems->ItemIndex;
 ord->commented=cbCommented->Checked;
 ord->repeating=cbRepeating->Checked;
 ord->amount=atoi(edCount->Text.c_str());
 if(inditem==-1){
//  ord=0;
//  ModalResult=mrCancel;
  return;
 }


 AMarket *mt=markets->Get(inditem);
 ord->item=mt->type;
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderBuySellForm::bnCancelClick(TObject *Sender)
{
 ord=0;
}

void __fastcall TEditOrderBuySellForm::edCountExit(TObject *Sender)
{
 int i=atoi(edCount->Text.c_str());
 edCount->Text=i;
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderBuySellForm::bnAllClick(TObject *Sender)
{
 int inditem=cbItems->ItemIndex;
 if(inditem<0) return;
 AMarket *mt=markets->Get(inditem);
 edCount->Text=mt->amount;
}
//---------------------------------------------------------------------------

