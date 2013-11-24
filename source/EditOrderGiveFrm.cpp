//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include <vector.h>
#include "util.h"
#include "orders.h"
#include "items.h"
#include "unit.h"
#include "EditOrderGiveFrm.h"
#include "faction.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditOrderGiveForm *EditOrderGiveForm;
//---------------------------------------------------------------------------
__fastcall TEditOrderGiveForm::TEditOrderGiveForm(TComponent* Owner, AOrderGive *_ord, AItems *its, AUnits *tars, AUnits *tars2)
    : TForm(Owner),ord(_ord),items(its),targets(tars),targets2(tars2)
{
 cbCommented->Checked=ord->commented;
 cbRepeating->Checked=ord->repeating;

 edCount->Text=ord->count;
 curtarnum=ord->tarnum;
 curitem=ord->item;
 AnsiString s;
 cbItems->Items->BeginUpdate();
 cbItems->Items->Add("Full Unit");
 if(!ord->item.Length()) cbItems->ItemIndex=0;
 foreach(items){
  AItem *it=*(AItem**)iter;
  AItemType *itype=ItemTypes->Get(it->type);
  if(itype->flags&CANTGIVE) continue;
  s=itype->name;
  s+=" (max ";
  s+=it->count;
  s+=")";
  cbItems->Items->AddObject(s,(TObject*)itype);
  if(it->type==curitem){
   cbItems->ItemIndex=cbItems->Items->Count-1;
  }
 }
 cbItems->Items->EndUpdate();
 cbDiscard->Checked=curtarnum==0;
 foreach(targets2){
  AUnit *un=*(AUnit**)iter;
  if(un->num==curtarnum){
   cbShowAll->Checked=true;
   break;
  }
 }
 MakeTargetList();
 cbItemsChange(this);
}
struct SortItem{
 int num;
 AUnit *unit;
};
int SortItemCompare(const SortItem &a,const SortItem &b){
 return a.num>b.num;
}
void TEditOrderGiveForm::MakeTargetList()
{
/* if(cbDiscard->Checked){
  cbTargets->Visible=false;
 }else{
  cbTargets->Visible=true;
 }*/
 AnsiString s;

 vector<SortItem> list;
 int itemcount=0;
 foreach(targets){
  AUnit *un=*(AUnit**)iter;
  SortItem si;
  si.unit=un;
  si.num=un->enditems->GetNum(curitem);
  if(si.num) itemcount+=si.num;
  list.push_back(si);
 }
 if(cbShowAll->Checked){
  foreach(targets2){
   AUnit *un=*(AUnit**)iter;
   SortItem si;
   si.unit=un;
   si.num=un->enditems->GetNum(curitem);
   if(si.num) itemcount+=si.num;
   list.push_back(si);
  }
 }
 if(itemcount)
  stable_sort(list.begin(),list.end(),SortItemCompare);
 cbTargets->Items->BeginUpdate();
 cbTargets->Items->Clear();
 for(int i=0,endi=list.size();i<endi;i++){
  SortItem &si=list[i];
  AUnit *un=si.unit;
  s=un->FullName();
  if(un->faction->number){
   s+=", ";
   s+=un->faction->FullName();
  }
  s+=" - ";
  s+=si.num; //un->PrintForTree()

  cbTargets->Items->AddObject(s,(TObject*)un);
  if(un->num==curtarnum){
   cbTargets->ItemIndex=cbTargets->Items->Count-1;
  }
 }
 cbTargets->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderGiveForm::bnOkClick(TObject *Sender)
{
 ord->commented=cbCommented->Checked;
 ord->repeating=cbRepeating->Checked;
 ord->count=atoi(edCount->Text.c_str());

 int inditem=cbItems->ItemIndex;
 if(inditem!=-1){
  AItemType *itype=(AItemType*)cbItems->Items->Objects[inditem];
  if(itype){
   ord->item=itype->abr;
  }else ord->item="";
 }

 if(cbDiscard->Checked){
  if(ord->target!=0){
   ord->tarnum=0;
   ord->DeleteMessage(ord->message);
   ord->target=0;
  }else ord->tarnum=0;
  return;
 }
 int indtar=cbTargets->ItemIndex;
 if(indtar<0) return;
 AUnit *un=(AUnit*)cbTargets->Items->Objects[indtar];
 if(ord->target!=un){
  ord->tarnum=un->num;
  ord->DeleteMessage(ord->message);
  ord->target=un;
 }
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderGiveForm::bnCancelClick(TObject *Sender)
{
 ord=0;
}
void __fastcall TEditOrderGiveForm::edCountExit(TObject *Sender)
{
 int i=atoi(edCount->Text.c_str());
 edCount->Text=i;
}
void __fastcall TEditOrderGiveForm::cbItemsChange(TObject *Sender)
{
 int ind=cbItems->ItemIndex;
 edCount->Visible=(ind>0);
 bnAll->Visible=(ind>0);
 if(ind<=0) return;
 AItemType *itype=(AItemType*)cbItems->Items->Objects[ind];
 if(itype){
   curitem=itype->abr;
 }else curitem="";
 MakeTargetList();
}
void __fastcall TEditOrderGiveForm::cbShowAllClick(TObject *Sender)
{
 MakeTargetList();
}
void __fastcall TEditOrderGiveForm::bnAllClick(TObject *Sender)
{
 int ind=cbItems->ItemIndex;
 if(ind<=0) return;
 AItemType *itype=(AItemType*)cbItems->Items->Objects[ind];
 if(!itype) return;
 edCount->Text=items->GetNum(itype->abr);
}
void __fastcall TEditOrderGiveForm::cbTargetsChange(TObject *Sender)
{
 int indtar=cbTargets->ItemIndex;
 if(indtar==-1) return;
 AUnit *un=(AUnit*)cbTargets->Items->Objects[indtar];
 curtarnum=un->num;
 cbDiscard->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TEditOrderGiveForm::cbDiscardClick(TObject *Sender)
{
 MakeTargetList();
}
//---------------------------------------------------------------------------

