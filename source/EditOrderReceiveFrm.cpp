//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <vector.h>

#include "util.h"
#include "orders.h"
#include "unit.h"
#include "region.h"
#include "items.h"
#include "EditOrderReceiveFrm.h"
#include "faction.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditOrderReceiveForm *EditOrderReceiveForm;
static AnsiString lastitem;
//---------------------------------------------------------------------------
__fastcall TEditOrderReceiveForm::TEditOrderReceiveForm(TComponent* Owner)
  : TForm(Owner),items(new TStringList),targets(new AUnits(curRegion->GetDummy())),
    added(false)
{
 targets->autodelete=false;
 if(!curUnit||!curRegion) return;
 curRegion->RunOrders(rsAfterGive);
 foreach(curRegion){
  AObject *obj=*(AObject**) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**) iter;
   if(un==curUnit) continue;
   if(!un->IsLocal()) continue;
   targets->Add(un);
  }
 }
 Init();
}
void TEditOrderReceiveForm::Init(){
 AnsiString s;
 curtarget=0;
 curitem="";
 items->Clear();
 foreach(targets){
  AUnit *un=*(AUnit**)iter;
  un->enditems->DeleteEmpty();  
  foreach(un->enditems){
   AItem *it=*(AItem**) iter;
   if(items->IndexOf(it->type)==-1){
    items->Add(it->type);
   }
  }
 }
 for(int i=items->Count-1;i>=0;i--){
  s=items->Strings[i];
  AItemType *itype=ItemTypes->Get(s);
  if(!itype||(itype->flags&CANTGIVE)){
   items->Delete(i);
  }else{
   items->Objects[i]=(TObject*) itype;
  }
 }
 cbItemsChange(0);
 cbTargetsChange(0);
 if(items->IndexOf(lastitem)>=0){
  curitem=lastitem;
 }
 MakeTargetsList();
 MakeItemsList();
}
__fastcall TEditOrderReceiveForm::~TEditOrderReceiveForm()
{
 delete items;
 delete targets;
}

struct SortUnItem{
 int num;
 AnsiString name;
 AUnit *unit;
};
int SortUnItemCompare(const SortUnItem &a,const SortUnItem &b){
 if(a.num>b.num) return true;
 if(a.num<b.num) return false;
 return a.name<b.name;
}
//---------------------------------------------------------------------------
void TEditOrderReceiveForm::MakeTargetsList()
{
 AnsiString s;
 vector<SortUnItem> list;
 foreach(targets){
  AUnit *un=*(AUnit**)iter;
  SortUnItem si;
  si.unit=un;
  si.name=un->FullName();
  if(!curitem.Length())
   si.num=0;
  else
   si.num=un->enditems->GetNum(curitem);
  list.push_back(si);
 }
 sort(list.begin(),list.end(),SortUnItemCompare);

 TStrings *List=cbTargets->Items;
 List->BeginUpdate();
 List->Clear();

 for(int i=0,endi=list.size();i<endi;i++){
  SortUnItem &si=list[i];
  AUnit *un=si.unit;
  s=si.name;
  if(un->faction->number){
   s+=", ";
   s+=un->faction->FullName();
  }
  if(curitem.Length()){
   if(!si.num) continue;
   s+=" (max ";
   s+=si.num;
   s+=")";
  }
  List->AddObject(s,(TObject*) un);
  if(un==curtarget) cbTargets->ItemIndex=List->Count-1;
 }
 List->EndUpdate();
}
struct SortInItem{
 int num;
 AnsiString name;
 AItemType *itype;
};
int SortInItemCompare(const SortInItem &a,const SortInItem &b){
 if(a.num>b.num) return true;
 if(a.num<b.num) return false;
 return a.name<b.name;
}

void TEditOrderReceiveForm::MakeItemsList()
{
 vector<SortInItem> lst;
 for(int i=0,endi=items->Count;i<endi;i++){
  SortInItem si;
  si.itype=(AItemType*) items->Objects[i];
  si.name=si.itype->name;
  if(curtarget)
   si.num=curtarget->enditems->GetNum(si.itype->abr);
  else
   si.num=0;
  lst.push_back(si);
 }
 sort(lst.begin(),lst.end(),SortInItemCompare);

 TStrings *list=cbItems->Items;
 AnsiString s;
 list->BeginUpdate();
 list->Clear();
 for(int i=0,endi=lst.size();i<endi;i++){
  SortInItem &si=lst[i];
  s=si.name;
  if(curtarget){
   s+=" (max ";
   s+=si.num;
   s+=")";
  }
  list->AddObject(s,(TObject*)si.itype);
  if(si.itype->abr==curitem) cbItems->ItemIndex=list->Count-1;
 }
 list->EndUpdate();
}
void __fastcall TEditOrderReceiveForm::cbItemsChange(TObject *Sender)
{
 int ind=cbItems->ItemIndex;
 int ind2=cbTargets->ItemIndex;
 edCount->Visible=(ind>=0)&&(ind2>=0);
 bnAll->Visible=(ind>=0)&&(ind2>=0);
 if(ind<0){ curitem=""; return;}
 AItemType *itype=(AItemType*) cbItems->Items->Objects[ind];
 curitem=itype->abr;

 MakeTargetsList();
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderReceiveForm::cbTargetsChange(TObject *Sender)
{
 int ind=cbTargets->ItemIndex;
 int ind2=cbItems->ItemIndex;
 edCount->Visible=(ind>=0)&&(ind2>=0);
 bnAll->Visible=(ind>=0)&&(ind2>=0);
 if(ind<0){ curtarget=0; return;}
 AUnit *un=(AUnit*) cbTargets->Items->Objects[ind];
 curtarget=un;
 MakeItemsList();
}
//---------------------------------------------------------------------------

void __fastcall TEditOrderReceiveForm::FormPaint(TObject *Sender)
{
static bool first=true;
 if(first){
  first=true;
  if(!curUnit||!curRegion) ModalResult=mrCancel;
//  if(!targets->count||!items->Count) ModalResult=mrCancel;
 }
}
//---------------------------------------------------------------------------

void __fastcall TEditOrderReceiveForm::bnAllClick(TObject *Sender)
{
 edCount->Text=curtarget->enditems->GetNum(curitem);
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderReceiveForm::bnOkClick(TObject *Sender)
{
 if(!curtarget) return;
 if(!curitem.Length()) return;
 int count=atoi(edCount->Text.c_str());
 if(count<=0) return;
 lastitem=curitem;
 AOrderGive *ord=new AOrderGive;
 ord->tarnum=curUnit->num;
 ord->target=curUnit;
 ord->item=curitem;
 ord->count=count;
 curtarget->orders->Add(ord);

 if(Sender==bnOk){
  ModalResult=mrOk;
  return;
 }
 added=true;
 curRegion->RunOrders(rsAfterGive);
 SetFocus();
 Init();
}
//---------------------------------------------------------------------------

void __fastcall TEditOrderReceiveForm::bnCancelClick(TObject *Sender)
{
 if(added) ModalResult=mrOk;  
}
//---------------------------------------------------------------------------

