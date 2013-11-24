//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <comctrls.hpp>

#include "util.h"
#include "faction.h"
#include "eapfile.h"
#include "turn.h"
#include "viewitems.h"
#include "region.h"
#include "unit.h"
#include "orders.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
char *AttitudeStrs[] ={
  "Hostile",
  "Unfriendly",
  "Neutral",
  "Friendly",
  "Ally"
};
int AttitudeStrToInt(AnsiString str){
 for(int i=0;i<5;i++){
  if(str.Pos(AttitudeStrs[i])==1)return i;
 }
 return -1;
}

void Attitude::CreateNew(Attitude * prevat)
{
 if(!prevat)throw(Exception("Can't create attitude from NULL"));
 attitude=prevat->attitude;
 facnum=prevat->facnum;
}
AFaction*curFaction;
AFaction::AFaction(AFactions *facs)
 :TList(),defattitude(A_HOSTILE),number(0),local(false),unclaimed(0),
 warpts(0),warmax(0),warused(0),
 tradepts(0),trademax(0),tradeused(0),
 magepts(0),magemax(0),mageused(0),
 factions(facs)
{
 orders=new AOrders((AUnit*)this);
 errors=new TStringList;
 battles=new TStringList;
 events=new TStringList;
}
__fastcall AFaction::~AFaction()
{
// Clear();
 if(curFaction==this) curFaction=0;
 delete errors;errors=0;
 delete battles;battles=0;
 delete events;events=0;
 delete orders;orders=0;
}
void __fastcall AFaction::Clear()
{
 for(int i=Count-1;i>=0;i--){
  Attitude*t=(Attitude*)Items[i];
  delete t;
 }
 if(orders) orders->Clear();
 if(errors) errors->Clear();
 if(battles) battles->Clear();
 if(events) events->Clear();
 TList::Clear();
}
void AFaction::Read(TFile & in)
{
 Clear();
 in>>number>>name>>local;
 if(local){
  in>>unclaimed;
  in>>address>>password;
  in>>defattitude;
  in>>warpts>>warmax>>warused;
  in>>tradepts>>trademax>>tradeused;
  in>>magepts>>magemax>>mageused;

  int size;
  in>>size;
  for(int i=0;i<size;i++){
   int facnum,attitude;
   in>>facnum>>attitude;
   SetAttitude(facnum,attitude);
  }
 }
 orders->Read(in);
 ReadStringList(in,errors);
 ReadStringList(in,battles);
 ReadStringList(in,events);
}
void AFaction::Write(TFile& out)
{
 out<<number<<name<<local;
 if(local){
  out<<unclaimed;
  out<<address<<password;
  out<<defattitude;
  out<<warpts<<warmax<<warused;
  out<<tradepts<<trademax<<tradeused;
  out<<magepts<<magemax<<mageused;

  out<<count;
  for(int i=0;i<count;i++){
   Attitude*at=(Attitude*)Items[i];
   out<<(at->facnum)<<(at->attitude);
  }
 }
 orders->Write(out);
 WriteStringList(out,errors);
 WriteStringList(out,battles);
 WriteStringList(out,events);
}
int AFaction::GetAttitude(int facnum)
{
 if(facnum==number)return A_ALLY;
 Attitude*a=Get(facnum);
 if(a) return a->attitude;
 return defattitude;
}
void AFaction::SetAttitude(int facnum, int att)
{
 Attitude*a=Get(facnum);
 if(att==-1){Remove(a); delete a;return;}
 if(!a){
  a=new Attitude;
  a->facnum=facnum;
  Add(a);
 }
 a->attitude=att;
}
Attitude* AFaction::Get(int facnum)
{

 for(int i=0;i<count;i++){
  Attitude*a=(Attitude*)Items[i];
  if(a->facnum==facnum) return a;
 }
 return 0;
}
void AFaction::CreateNew(AFaction *prevfac)
{
 if(!prevfac)
  throw(Exception("Can't create faction from NULL"));
 number=prevfac->number;
 name=prevfac->name;
 local=prevfac->local;
 unclaimed=0;//prevfac->unclaimed;
 address=prevfac->address;
 password=prevfac->password;
 defattitude=prevfac->defattitude;
 warpts=prevfac->warpts;
 warmax=prevfac->warmax;
 warused=0;//prevfac->warused;
 tradepts=prevfac->tradepts;
 trademax=prevfac->trademax;
 tradeused=0;//prevfac->tradeused;
 magepts=prevfac->magepts;
 magemax=prevfac->magemax;
 mageused=0;//prevfac->mageused;
 Clear();
 for(int i=0;i<prevfac->count;i++){
  Attitude*prevat=(Attitude*)prevfac->Items[i];
  Attitude*at=new Attitude;
  at->CreateNew(prevat);
  Add(at);
 }
}
AnsiString AFaction::FullName()
{
 return name+" ("+number+")";
}
AnsiString AFaction::ReportHeader()
{
 AnsiString end="\r\n";
 AnsiString s;
 s+=KeyPhrases->Get(keyAtlRepHead)+end;
 s+=FullName()+" (";
 bool first=true;
 if(warpts){
  first=false;
  s+=KeyPhrases->Get(keyWarSign)+" "+warpts;
 }
 if(tradepts){
  if(!first) s+=", ";
  first=false;
  s+=KeyPhrases->Get(keyTradeSign)+" "+tradepts;
 }
 if(magepts){
  if(!first) s+=", ";
  s+=KeyPhrases->Get(keyMagicSign)+" "+magepts;
 }
 s+=")"+end;
 s+=TurnNumberToString(curTurn->num)+end+end;
 s+=curTurns->ver1+end;
 s+=curTurns->ver2+end+end;
 s+=KeyPhrases->Get(keyFacStat)+end;
 s+=KeyPhrases->Get(keyTaxStatSign)+warused+" ("+warmax+")"+end;
 s+=KeyPhrases->Get(keyTradeStatSign)+tradeused+" ("+trademax+")"+end;
 s+=KeyPhrases->Get(keyMagesStatSign)+mageused+" ("+magemax+")"+end;
 s+=end;
 return s;
}
void AFaction::MakeView(VListItems * vlis)
{
 VListItem *vli;
 AnsiString s;
 s=FullName()+" (";
 s+=KeyPhrases->Get(keyWarSign)+" "+warpts+", ";
 s+=KeyPhrases->Get(keyTradeSign)+" "+tradepts+", ";
 s+=KeyPhrases->Get(keyMagicSign)+" "+magepts+")";

 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;
 vli->text=s;

 s=KeyPhrases->Get(keyFacStat);
 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;
 vli->text=s;

 s=KeyPhrases->Get(keyTaxStatSign)+warused+" : "+CountTaxRegions()+" ("+warmax+")";
 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;
 vli->text=s;

 s=KeyPhrases->Get(keyTradeStatSign)+" "+tradeused+" ("+trademax+")";
 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;
 vli->text=s;

 s=KeyPhrases->Get(keyMagesStatSign)+" "+mageused+" ("+magemax+")";
 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;
 vli->text=s;

 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;

 s=KeyPhrases->Get(keyAttitudes)+AttitudeStrs[defattitude]+")";
 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;
 vli->text=s;

 for(int i=0;i<NATTITUDES;i++){
  s=AttitudeStrs[i];
  s+=" : ";
  bool found=false;
  for(int j=0;j<count;j++){
   Attitude *t=(Attitude*)Items[j];
   if(t->attitude==i){
    AFaction *fac=curFactions->GetFaction(t->facnum);
    if(!fac)continue;
    if(found){
     s+=", ";
    }else{
     found=true;
    }
    s+=fac->FullName();
   }
  }
  if(!found)s+="none";
  vli=new VListItem(vlis);
  vli->type=litFactHeader;
  vli->value=number;
  vli->text=s;
 }
 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;

 s=KeyPhrases->Get(keyUnclaimed)+unclaimed;
 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;
 vli->text=s;

 vli=new VListItem(vlis);
 vli->type=litFactHeader;
 vli->value=number;

 int kol;
 kol=errors->Count;
 if(kol){
  s="Errors: ";
  s+=kol;
  vli=new VListItem(vlis);
  vli->type=litFactErrors;
  vli->value=number;
  vli->text=s;
 }
 kol=battles->Count;
 if(kol){
  s="Battles: ";
  s+=kol;
  vli=new VListItem(vlis);
  vli->type=litFactBattles;
  vli->value=number;
  vli->text=s;
 }
 kol=events->Count;
 if(kol){
  s="Events: ";
  s+=kol;
  vli=new VListItem(vlis);
  vli->type=litFactEvents;
  vli->value=number;
  vli->text=s;
 }
}
void AFaction::MakeViewErrors(VListItems * vlis)
{
 AnsiString s;
 VListItem *vli;
 
 s=KeyPhrases->Get(keyAtlErrHead);
 vli=new VListItem(vlis);
 vli->type=litFactErrorsHeader;
 vli->value=number;
 vli->text=s;

 for(int i=0;i<errors->Count;i++){
  vli=new VListItem(vlis);
  vli->type=litFactError;
  vli->value=number;
  vli->text=errors->Strings[i];
 }
}
void AFaction::MakeViewBattles(VListItems * vlis)
{
 AnsiString s;
 VListItem *vli;

 s=KeyPhrases->Get(keyAtlBatHead);
 vli=new VListItem(vlis);
 vli->type=litFactBattlesHeader;
 vli->value=number;
 vli->text=s;

 for(int i=0;i<battles->Count;i++){
  vli=new VListItem(vlis);
  vli->type=litFactBattle;
  vli->value=number;
  vli->text=battles->Strings[i];
 }
}
void AFaction::MakeViewEvents(VListItems * vlis)
{
 AnsiString s;
 VListItem *vli;

 s=KeyPhrases->Get(keyAtlEvnHead);
 vli=new VListItem(vlis);
 vli->type=litFactEventsHeader;
 vli->value=number;
 vli->text=s;

 for(int i=0;i<events->Count;i++){
  vli=new VListItem(vlis);
  vli->type=litFactEvent;
  vli->value=number;
  vli->text=events->Strings[i];
 }
}
void AFaction::MakeTree(TTreeNode * parnode, VTreeNodes * vtns)
{
 AnsiString str=FullName();
 VTreeNode *vtn;
 vtn=new VTreeNode(vtns);
 vtn->type=tntFaction;
 vtn->value=number;
 TTreeNode *node=parnode->Owner->AddChildObject(parnode,str,vtn);
 vtn->node=node;
 TTreeNode *node2;
 if(errors->Count){
  vtn=new VTreeNode(vtns);
  vtn->type=tntFactionErrors;
  vtn->value=number;
  node2=parnode->Owner->AddChildObject(node,"Errors",vtn);
  vtn->node=node2;
 }
 if(battles->Count){
  vtn=new VTreeNode(vtns);
  vtn->type=tntFactionBattles;
  vtn->value=number;
  node2=parnode->Owner->AddChildObject(node,"Battles",vtn);
  vtn->node=node2;
 }
 if(events->Count){
  vtn=new VTreeNode(vtns);
  vtn->type=tntFactionEvents;
  vtn->value=number;
  node2=parnode->Owner->AddChildObject(node,"Events",vtn);
  vtn->node=node2;
 }
// node->Expand(true);
}

AFactions*curFactions;
AFactions::AFactions(ATurn *t)
 :TList(),turn(t)
{
}
__fastcall AFactions::~AFactions()
{
// Clear();
 if(curFactions==this) curFactions=0;
}
void __fastcall AFactions::Clear()
{
 for(int i=Count-1;i>=0;i--){
  AFaction * t=Get(i);
//  TList::Delete(i);
  delete t;
 }
 TList::Clear();
}
AFaction * AFactions::Get(int index)
{
 return (AFaction*)Items[index];
}
void AFactions::Read(TFile& in)
{
 Clear();
 int size;
 in>>size;

 if(BaseVer<8){
  AFaction *fac;
  fac=new AFaction(this);
  fac->number=0;
  fac->name="Undetected";
  Add(fac);
 }

 for(int i=0;i<size;i++){
  AFaction*fac=new AFaction(this);
  fac->Read(in);
  Add(fac);
 }
}
void AFactions::Write(TFile& out)
{
 out<<count;
 for(int i=0;i<count;i++){
  AFaction *fac=Get(i);
  fac->Write(out);
 }
}
AFaction* AFactions::GetFaction(int facnum)
{
// if(facnum==0) return 0; //temp

 for(int i=0;i<count;i++){
  AFaction*fac=Get(i);
  if(fac->number==facnum)return fac;
 }
 return 0;
}
AFaction* AFactions::GetFaction(AnsiString name)
{
 for(int i=0;i<count;i++){
  AFaction*fac=Get(i);
  if(fac->name==name)return fac;
 }
 return 0;
}
void AFactions::Add(AFaction* fac)
{
 if(GetFaction(fac->number))
  throw Exception(AnsiString("Error AFactions::Add(): number ")+fac->number+" already present");
 TList::Add(fac);
}
bool AFactions::SetCurFaction(int facnum)
{
 if(curFactions!=this)return false;
 if(facnum==0){curFaction=0;return true;}
 AFaction *fac=GetFaction(facnum);
 if(curFaction==fac) return true;
 if(!fac)return false;
 if(!fac->local)return false;
 curFaction=fac;
 AddEvent("Faction "+fac->FullName()+" activated");
 return true;
}
void AFactions::CreateNew(AFactions *prevfacs)
{
 Clear();
 if(!prevfacs){
  AFaction *fac;
  fac=new AFaction(this);
  fac->number=0;
  fac->name="Undetected";
  Add(fac);
  fac=new AFaction(this);
  fac->number=1;
  fac->name="The Guardsmen";
  Add(fac);
  fac=new AFaction(this);
  fac->number=2;
  fac->name="Creatures";
  Add(fac);
  return;
 }
 for(int i=0;i<prevfacs->count;i++){
  AFaction *prevfac=prevfacs->Get(i);
  AFaction *fac=new AFaction(this);
  fac->CreateNew(prevfac);
  Add(fac);
 }
}
//0 - not found, 1 - present, 2 - name, not num, 3 - num, not name
int AFactions::VerFaction(AnsiString name, int num)
{
 AFaction*fac;
 fac=GetFaction(num);
 if(fac){
  if(fac->name==name)return 1;
  return 3;
 }
 fac=GetFaction(name);
 if(fac)return 2;
 return 0;
}
void AFactions::Delete(int facnum)
{
 AFaction *fac=GetFaction(facnum);
 if(fac) TList::Remove(fac);
 delete fac;
}
void AFactions::MakeTree(TTreeNode * parnode, VTreeNodes * vtns)
{
 for(int i=0;i<count;i++){
  AFaction*fac=Get(i);
  if(!fac->local)continue;
  fac->MakeTree(parnode,vtns);
 }
}
bool AFaction::SaveTemplate(TFileStream * file)
{
 AnsiString s;
 s=KeyPhrases->Get(keyTemplateBegin);
 s+=number;
 s+=" \"";
 s+=password;
 s+="\"\n\n";
 file->Write(s.c_str(),s.Length());
 foreach(curRegionList){
  ARegion *reg=*(ARegion**)iter;
  reg->PrepareOrders();
  foreach(reg){
   AObject *obj=*(AObject**)iter;
   bool newpresent=false;
   foreach(obj->units){
    AUnit *unit=*(AUnit**)iter;
    if(unit->faction!=this)continue;
    if(unit->num<0){newpresent=true;continue;}
    if(!unit->SaveTemplate(file))return false;
   }
   if(newpresent)foreach(obj->units){
    AUnit *unit=*(AUnit**)iter;
    if(unit->num>0)continue;
    if(unit->faction!=this)continue;
    if(!unit->SaveTemplate(file))return false;
   }
  }
 }
 s="#end\n\n";
 file->Write(s.c_str(),s.Length());

 return true;
}
int AFaction::CanSee(ARegion * r, AUnit * u)
{
 int detfac=0;
 if(u->faction==this) return 2;
 if(u->reveal==REVEAL_FACTION) return 2;
 int retval=0;
 if(u->reveal==REVEAL_UNIT) retval=1;
 foreach(r){
  AObject *obj=*(AObject**)iter;
  bool dummy=false;
  if(obj->num==0) dummy=true;
  foreach(obj->units){
   AUnit *temp=*(AUnit**)iter;
   if(u==temp&&dummy==false) retval=1;
   if(temp->faction==this){
//    if (temp->GetSkill(S_OBSERVATION) > u->GetSkill(S_STEALTH)) {
     return 2;
/*
    }else{
    if (temp->GetSkill(S_OBSERVATION) ==  u->GetSkill(S_STEALTH))
     retval=1;
    }*/
   }
  }
 }
 if (retval==1&&detfac) return 2;
 return retval;
}
int AFaction::CountTaxRegions()
{
 int count=0;
 foreach(factions->turn->RegionList){
  ARegion *reg=*(ARegion**)iter;
  bool found=false;
  foreach(reg){
   AObject *obj=*(AObject**)iter;
   foreach(obj->units){
    AUnit *u=*(AUnit**)iter;
    if(u->endfaction!=this)continue;
    if(!u->HasTaxOrder())continue;
    if(!reg->CanTax(u))continue;
    count++;
    found=true;
    break;
   }
   if(found)break;
  }
 }
 return count;
}
void AFactions::Compact()
{
 foreachr(this){
  AFaction *fac=*(AFaction**)iter;
  fac->Clear();
  if(fac->local) continue;
  if(fac->number<=2) continue;
  TList::Remove(fac);
  delete fac; 
 }
}
bool AFaction::SaveReport(TFileStream * file)
{
 AnsiString s;
 s=ReportHeader();
// file->Write(s.c_str(),s.Length());
 SaveRepString(file,s);
/* {
  TStringList *list=new TStringList;
  list->Text=s;
  SaveRepStringList(file,list);
  delete list;
 }*/
 if(errors->Count){
  SaveRepString(file,KeyPhrases->Get(keyAtlErrHead));
  SaveRepStringList(file,errors);
  SaveRepString(file,"");
 }
 if(battles->Count){
  SaveRepString(file,KeyPhrases->Get(keyAtlBatHead));
  SaveRepStringList(file,battles);
  SaveRepString(file,"");
 }
 {
  SaveRepString(file,KeyPhrases->Get(keyAtlEvnHead));
  SaveRepStringList(file,events);
  SaveRepString(file,"");
 }
 {
  s=KeyPhrases->Get(keyAttitudes)+AttitudeStrs[defattitude]+"):";
  SaveRepString(file,s);

  for(int i=0;i<NATTITUDES;i++){
   s=AttitudeStrs[i];
   s+=" : ";
   bool found=false;
   for(int j=0;j<count;j++){
    Attitude *t=(Attitude*)Items[j];
    if(t->attitude==i){
     AFaction *fac=curFactions->GetFaction(t->facnum);
     if(!fac)continue;
     if(found){
      s+=", ";
     }else{
      found=true;
     }
     s+=fac->FullName();
    }
   }
   if(!found)s+="none";
   s+=".";
   SaveRepString(file,s);
  }
  SaveRepString(file,"");
 }
 {
  s=KeyPhrases->Get(keyUnclaimed)+unclaimed+".";
  SaveRepString(file,s);
  SaveRepString(file,"");
 }
 foreach(factions->turn->RegionList){
  ARegion *reg=*(ARegion**)iter;
  if(reg->lastviewfact!=number&&!reg->PresentFaction(this))
   continue;
  if(!reg->WriteReport(file,roNormalReport)) return false;
 }
 SaveRepString(file,"");
 
 s=KeyPhrases->Get(keyTemplateSign);
 s+="(Short Format):";
 SaveRepString(file,s);
 SaveRepString(file,"");

 return SaveTemplate(file);
}



int AFaction::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 siz+=count*sizeof(Attitude);
 siz+=3*sizeof(TStringList);
 siz+=errors->Text.Length()+1;
 siz+=battles->Text.Length()+1;
 siz+=events->Text.Length()+1;
 siz+=orders->GetSize();
 return siz;
}
int AFactions::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 foreach(this){
  AFaction *fac=*(AFaction**) iter;
  siz+=fac->GetSize();
 }
 return siz;
}

