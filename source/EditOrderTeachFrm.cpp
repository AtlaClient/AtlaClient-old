//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "util.h"
#include "orders.h"
#include "unit.h"
#include "skills.h"
#include "EditOrderTeachFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditOrderTeachForm *EditOrderTeachForm;
//---------------------------------------------------------------------------
__fastcall TEditOrderTeachForm::TEditOrderTeachForm(TComponent* Owner, AOrderTeach *_ord, AUnits *_units)
    : TForm(Owner),ord(_ord),units(_units)
{
 maxstudents=ord->orders->unit->GetMen()*10;
 cbCommented->Checked=ord->commented;
 cbRepeating->Checked=ord->repeating;
 targets=new AUnits(ord->orders->unit->baseobj);
 targets->autodelete=false;
 for(int i=0;i<ord->targets->Count;i++){
  AUnitPtr *ptr=ord->GetTarget(i);
  targets->Add(ptr->ptr);
 }
 MakeLists();
}
void __fastcall TEditOrderTeachForm::bnOkClick(TObject *Sender)
{
 ord->commented=cbCommented->Checked;
 ord->repeating=cbRepeating->Checked;
 ord->Clear();
 foreach(targets){
  AUnit *un=*(AUnit**)iter;
  ord->AddTarget(un->num,un);
 }
}
void __fastcall TEditOrderTeachForm::bnCancelClick(TObject *Sender)
{
 ord=0;
}
void TEditOrderTeachForm::MakeLists()
{
 lbTargets->Items->BeginUpdate();
 lbTargets->Items->Clear();
 int maxsize=0;
 int students=0;
 foreach(targets){
  AUnit *un=*(AUnit**)iter;
  AnsiString s=PrintUnit(un);
  students+=un->GetMen();
  lbTargets->Items->Add(s);
  TSize siz=lbTargets->Canvas->TextExtent(s);
  if(siz.cx>maxsize)maxsize=siz.cx;
 }
 lbTargets->Items->EndUpdate();
 lbTargets->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
 maxsize=0;
 lbUnits->Items->BeginUpdate();
 lbUnits->Items->Clear();
 foreach(units){
  AUnit *un=*(AUnit**)iter;
  if(targets->GetUnit(un->num))continue;
  AnsiString s=PrintUnit(un);
  lbUnits->Items->Add(s);
  TSize siz=lbUnits->Canvas->TextExtent(s);
  if(siz.cx>maxsize)maxsize=siz.cx;
 }
 lbUnits->Items->EndUpdate();
 lbUnits->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
 AnsiString s="Teaching ";
 s+=students;
 s+=" of ";
 s+=maxstudents;
 s+=" students";
 Label1->Caption=s;
}
void __fastcall TEditOrderTeachForm::FormDestroy(TObject *Sender)
{
 delete targets;
}
void __fastcall TEditOrderTeachForm::bnAddClick(TObject *Sender)
{
 int ind=lbUnits->ItemIndex;
 if(ind==-1)return;
 AnsiString str=lbUnits->Items->Strings[ind];
 foreach(units){
  AUnit *un=*(AUnit**)iter;
  if(str==PrintUnit(un)){
   targets->Add(un);
   MakeLists();
   return;
  }
 }
}
void __fastcall TEditOrderTeachForm::bnDeleteClick(TObject *Sender)
{
 int ind=lbTargets->ItemIndex;
 if(ind==-1)return;
 AnsiString str=lbTargets->Items->Strings[ind];
 foreach(targets){
  AUnit *un=*(AUnit**)iter;
  if(str==PrintUnit(un)){
   targets->Delete(un->num);
   MakeLists();
   return;
  }
 }
}


AnsiString TEditOrderTeachForm::PrintUnit(AUnit * un)
{
 AnsiString s;
 s=un->FullName();
 AOrderAbr *sord=dynamic_cast<AOrderAbr*>(un->orders->Find(O_STUDY,2));
 if(!sord)return s;
 s+=" ";
 ASkill sk;
 sk.type=sord->abr;
 sk.days=un->GetSkillDays(sk.type);
 s+=sk.Print();
 if(sk.days>=420) s="! "+s;
 return s;
}

