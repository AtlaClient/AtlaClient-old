//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include "util.h"
#include "eapfile.h"
#include "faction.h"
#include "viewitems.h"
#include "orders.h"
#include "items.h"
#include "turn.h"
#include "region.h"
#include "skills.h"
#include "unit.h"

#include "reports.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
AUnit *curUnit;
AUnit::AUnit(ATurn *t)
 :num(-1),faction(0),turn(t),
//  reveal(REVEAL_NONE),flags(0),info(INFO_NONE),guard(GUARD_NONE)
  globalflags(0),
  weight(0), walkweight(0), ridweight(0), flyweight(0),
  endflags(0),endguard(0),endreveal(0),endfaction(0),
  baseobj(0),endobj(0)
{
 if(hasskills||reserved||hascanstudy){
  globalflags=0;
 }
 orders=new AOrders(this);
 items=new AItems;
 enditems=new AItems;
 skills=new ASkills;
 endskills=new ASkills;
 canstudy=new TStringList;
}
__fastcall AUnit::~AUnit()
{
 if(curUnit==this) curUnit=0;
 delete orders;
 delete items;
 delete enditems;
 delete skills;
 delete endskills;
 delete canstudy;
}
void AUnit::Read(TFile & in)
{
 in>>name;
 in>>num;
 int facnum;
 in>>facnum;
 SetFaction(facnum);
 in>>globalflags;
 in>>describe;
 if(BaseVer<8){
  if(!MyCompare(describe.c_str(),KeyPhrases->Get(keyUnitCanStudy))){
   AnsiString str=describe+'.';
   char *s=str.c_str()+KeyPhrases->Get(keyUnitCanStudy).Length();
   TStringList *list=new TStringList;
   if(ProcessShortSkillsList(s,list)){
    canstudy->Assign(list);
    hascanstudy=true;
   }
   delete list;
  }
 }
 orders->Read(in);
 items->Read(in);
 enditems->Clear();
 if(hasskills)
  skills->Read(in);
 if(hascombatspell)
  ReadStringAsInt(in,combatspell);
 if(hascanstudy){
  int kol;
  in>>kol;
  AnsiString str;
  canstudy->Clear();
  for(int i=0;i<kol;i++){
   ReadStringAsInt(in,str);
   canstudy->Add(str);
  }
 }
}
void AUnit::Write(TFile &out)
{
 out<<name;
 out<<num;
 int facnum=0;
 if(faction) facnum=faction->number;
 out<<facnum;
 reserved=0;
// hascanstudy=0; ////////temp
 out<<globalflags;
/* int dlen=describe.Length();
 if(dlen){
  if(describe[dlen]=='.')describe.SetLength(dlen-1);
 }*/
 out<<describe;
 orders->Write(out);
 items->Write(out);
 if(hasskills)
  skills->Write(out);
 if(hascombatspell)
  WriteStringAsInt(out,combatspell);
 if(hascanstudy){
  int kol=canstudy->Count;
  out<<kol;
  for(int i=0;i<kol;i++)
   WriteStringAsInt(out,canstudy->Strings[i]);
 }
}
void AUnit::CreateNew(AUnit * prvunit)
{
// name=prvunit->name;
}
void AUnit::Update(AUnit *newun)
{
 if(info>newun->info)return;
 name=newun->name;
 info=newun->info;
 num=newun->num;
 if(!faction||newun->faction->number)
  faction=newun->faction;
 guard=newun->guard;
 reveal=newun->reveal;
 flags=newun->flags;

 describe=newun->describe;

 orders->Update(newun->orders);

 items->Clear();
 items->Update(newun->items);
 enditems->Clear();
 hasskills=newun->hasskills;
 skills->Update(newun->skills);
 hascombatspell=newun->hascombatspell;
 combatspell=newun->combatspell;

 hascanstudy=newun->hascanstudy;
 if(hascanstudy){
  canstudy->Assign(newun->canstudy);
 }
}
void AUnit::UpdateFromPrev(AUnit * prvun)
{
 if(IsLocal()) return;
 if(prvun->orders->count&&!orders->count){
  foreach(prvun->orders){
   AOrder *prvord=*(AOrder**) iter;
   if(!prvord->repeating) continue;
   AnsiString str=prvord->WriteTemplate();
   AOrder *ord=ParseOrder(str);
   if(!ord) continue;
   orders->Add(ord);
  }
 }
 if(prvun->skills->count/*&&!skills->count*/){
  skills->UpdateFromPrev(prvun->skills);
  hasskills=true;
 }
}

bool AUnit::GetFlag(int flag)
{
 return flags&flag;
}
void AUnit::SetFlag(int flag, bool value)
{
 if(value){
  flags|=flag;
 }else{
  flags&=~flag;
 }
}
bool AUnit::GetEndFlag(int flag)
{
 return endflags&flag;
}
void AUnit::SetEndFlag(int flag, bool value)
{
 if(value){
  endflags|=flag;
 }else{
  endflags&=~flag;
 }
}
AnsiString AUnit::FullName(bool fortemplate)
{
 AnsiString s;
 if(num<0){
  AnsiString ld=GetLocalDescr();
  if(ld.Length()){
   s+=ld+" (";
  }else s+=" ";
  s+="new ";
  s+=-num;
  if(ld.Length()){
   s+=")";
  }
 }else{
  if(name.Length()){
   s+=name;
  }else if(fortemplate){
   s+="Unit";
  }else{
   AnsiString ld=GetLocalDescr();
   if(ld.Length())s+=ld;
   else s+="Unit";
  }
  s+=" (";
  s+=num;
  s+=")";
 }
 return s;
}
AnsiString AUnit::FullName2()
{
 AnsiString s;
 if(num<0){
  AnsiString ld=GetLocalDescr();
  if(ld.Length()){
   s+=ld+" (";
  }else s+=" ";
  s+="new ";
  s+=-num;
  if(ld.Length()){
   s+=")";
  }
 }else{
  AnsiString ld=GetLocalDescr();
  if(ld.Length()){
   s+=ld;
  }
  if(name.Length()){
   if(ld.Length()) s+=" ";
   s+=name;
  }
  s+=" (";
  s+=num;
  s+=")";
 }
 return s;
}

AnsiString AUnit::UnitHeader(bool fortemplate)
{
 AnsiString s;
 s+=FullName(fortemplate);
 if(guard==GUARD_GUARD){
  s+=KeyPhrases->Get(keyUnitOnGuard);
 }
 if(faction->number){
  s+=", ";
  s+=faction->FullName();
 }
 if(guard==GUARD_AVOID){
  s+=KeyPhrases->Get(keyUnitAvoid);
 }
 if(GetFlag(FLAG_BEHIND)){
  s+=KeyPhrases->Get(keyUnitBehind);
 }
 if(reveal==REVEAL_UNIT){
  s+=KeyPhrases->Get(keyUnitRevUnit);
 }
 if(reveal==REVEAL_FACTION){
  s+=KeyPhrases->Get(keyUnitRevFaction);
 }
 if(GetFlag(FLAG_HOLDING)){
  s+=KeyPhrases->Get(keyUnitHold);
 }
 if(GetFlag(FLAG_AUTOTAX)){
  s+=KeyPhrases->Get(keyUnitTax);
 }
 if(GetFlag(FLAG_NOAID)){
  s+=KeyPhrases->Get(keyUnitNoAid);
 }
 if(GetFlag(FLAG_CONSUMING_UNIT)){
  s+=KeyPhrases->Get(keyUnitConsumUnit);
 }
 if(GetFlag(FLAG_CONSUMING_FACTION)){
  s+=KeyPhrases->Get(keyUnitConsumFaction);
 }
 return s;
}

AnsiString AUnit::Print()
{
 AnsiString s;
 if(info==INFO_LOCAL)s="* ";
 else if(info==INFO_FACTION) s="- ";
 else s="--";
 s+=UnitHeader();
 AnsiString itemstr=items->Print();
 if(itemstr.Length()){
  s+=", ";
  s+=itemstr;
 }
 if(describe.Length())s+=/*"; "+*/describe;
 s+=".";
 return s;
}
bool AUnit::WriteReport(TStream *strm,int obs,int truesight,bool detfac,bool autosee)
{
 if(num<0) return true;
 AnsiString s;
 int stealth=GetBegStealth();
 int tmpreveal;
 if(faction->local){
  tmpreveal=reveal;
 }else tmpreveal=REVEAL_FACTION;
 if(obs==-1){
  obs=2;
 }else{
  if(obs<stealth) {
            /* The unit cannot be seen */
   if(tmpreveal==REVEAL_FACTION){
    obs=1;
   }else{
    if(guard==GUARD_GUARD||tmpreveal==REVEAL_UNIT||autosee){
     obs=0;
    }else{
     return true;
    }
   }
  }else{
   if(obs==stealth){
    /* Can see unit, but not Faction */
    if(tmpreveal==REVEAL_FACTION){
     obs=1;
    }else{
     obs=0;
    }
   }else{
    /* Can see unit and Faction */
    obs = 1;
   }
  }
 }
 /* Setup True Sight */
 if(obs==2){
  truesight=1;
 }else{
  if(GetBegSkill("ILLU")>truesight){
   truesight=0;
  }else{
   truesight=1;
  }
 }
 if(detfac&&obs!=2) obs=1;
 
 if(obs==2) s="* ";
 else s="- ";
 s+=FullName(true);
 if(guard==GUARD_GUARD){
  s+=KeyPhrases->Get(keyUnitOnGuard);
 }
 if(faction->number&&!faction->local){
  s+=", ";
  s+=faction->FullName();
 }
 if(obs>0){
  if(faction->number&&faction->local){
   s+=", ";
   s+=faction->FullName();
  }
  if(guard==GUARD_AVOID){
   s+=KeyPhrases->Get(keyUnitAvoid);
  }
  if(GetFlag(FLAG_BEHIND)){
   s+=KeyPhrases->Get(keyUnitBehind);
  }
 }
 if(obs==2){
  if(reveal==REVEAL_UNIT){
   s+=KeyPhrases->Get(keyUnitRevUnit);
  }
  if(reveal==REVEAL_FACTION){
   s+=KeyPhrases->Get(keyUnitRevFaction);
  }
  if(GetFlag(FLAG_HOLDING)){
   s+=KeyPhrases->Get(keyUnitHold);
  }
  if(GetFlag(FLAG_AUTOTAX)){
   s+=KeyPhrases->Get(keyUnitTax);
  }
  if(GetFlag(FLAG_NOAID)){
   s+=KeyPhrases->Get(keyUnitNoAid);
  }
  if(GetFlag(FLAG_CONSUMING_UNIT)){
   s+=KeyPhrases->Get(keyUnitConsumUnit);
  }
  if(GetFlag(FLAG_CONSUMING_FACTION)){
   s+=KeyPhrases->Get(keyUnitConsumFaction);
  }
 }
 s+=items->WriteReport(obs,truesight,false);
 if(obs==2) {
  s+=KeyPhrases->Get(keyUnitSkills);
  s+=skills->Print();
 }
 if(obs==2&&IsMage()) {
  if(combatspell.Length()){
   s+=KeyPhrases->Get(keyUnitCombatSpell);
   s+=SkillTypes->AbrToName(combatspell)+" [";
   s+=combatspell+"]";
  }
  if(canstudy->Count){
   s+=KeyPhrases->Get(keyUnitCanStudy);
   for(int i=0,endi=canstudy->Count;i<endi;i++){
    AnsiString skill=canstudy->Strings[i];
    if(i) s+=", ";
    s+=SkillTypes->AbrToName(skill)+" [";
    s+=skill+"]";
   }
  }
//  temp += MageReport();
 }



 if(describe.Length())
  s+=describe;
 s+=".";
 SaveRepString(strm,s);
 return true;
}
AnsiString AUnit::PrintForTree()
{
 AnsiString s;
// s=" ";         //compare
// if(IsLocal())s+=" "; //compare
 s+=UnitHeader();
/* AnsiString itemstr=items->Print();
 if(itemstr.Length()){
  s+=", ";
  s+=itemstr;
 }*/
// s+=".";
 return s;
}
void AUnit::MakeView(VListItems * vlis)
{
 AnsiString s;
 VListItem *vli;
 if(info==INFO_LOCAL)s="* ";
 else if(info==INFO_FACTION) s="- ";
 else s="--";
 s+=UnitHeader();
 vli=new VListItem(vlis);
 vli->type=litUnitHeader;
 vli->value=num;
 vli->text=s;

 vli=new VListItem(vlis);
 vli->type=litUnitInfo;
// vli->value=num;
 vli->text=items->Print();

 if(hasskills){
  vli=new VListItem(vlis);
  vli->type=litUnitInfo;
// vli->value=num;
  vli->text=skills->Print(1+4+8);
 }
 if(hascombatspell){
  s="Combat spell: ";
  ASkill *sk=skills->GetSkill(combatspell);
  if(sk)s+=sk->Print(1+4);
  vli=new VListItem(vlis);
  vli->type=litUnitInfo;
  vli->text=s;
 }
 if(hascanstudy){
  s="Can Study: ";
  for(int i=0;i<canstudy->Count;i++){
   ASkillType *sk=SkillTypes->Get(canstudy->Strings[i],2);
   if(sk){
    s+=sk->name;
    s+=" [";
    s+=sk->abr;
    s+="]";  
   }
   s+=", ";
  }
  s.SetLength(s.Length()-2);
  vli=new VListItem(vlis);
  vli->type=litUnitInfo;
  vli->text=s;
 }

 vli=new VListItem(vlis);
 vli->type=litUnitInfo;
// vli->value=num;
 vli->text=describe;

 if(optRunRegionOrders)
  RunOrders();

 int i=0;
 foreach(orders){
  AOrder *ord=*(AOrder**) iter;
  vli=new VListItem(vlis);
  vli->type=litUnitOrder;
  vli->value=i;
  s=ord->Print();
  if(ord->type!=O_MESSAGE) s=" "+s;
  vli->text=s;
  i++;
 }
 vli=new VListItem(vlis);
 vli->type=litUnitInfo;

 vli=new VListItem(vlis);
 vli->type=litUnitInfo;
 vli->text="At end turn:";

 s=FullName();
 if(endguard==GUARD_GUARD){
  s+=KeyPhrases->Get(keyUnitOnGuard);
 }
 if(endfaction->number){
  s+=", ";
  s+=endfaction->FullName();
 }
 if(endguard==GUARD_AVOID){
  s+=KeyPhrases->Get(keyUnitAvoid);
 }
 if(GetEndFlag(FLAG_BEHIND)){
  s+=KeyPhrases->Get(keyUnitBehind);
 }
 if(endreveal==REVEAL_UNIT){
  s+=KeyPhrases->Get(keyUnitRevUnit);
 }
 if(endreveal==REVEAL_FACTION){
  s+=KeyPhrases->Get(keyUnitRevFaction);
 }
 if(GetEndFlag(FLAG_HOLDING)){
  s+=KeyPhrases->Get(keyUnitHold);
 }
 if(GetEndFlag(FLAG_AUTOTAX)){
  s+=KeyPhrases->Get(keyUnitTax);
 }
 if(GetEndFlag(FLAG_NOAID)){
  s+=KeyPhrases->Get(keyUnitNoAid);
 }
 if(GetEndFlag(FLAG_CONSUMING_UNIT)){
  s+=KeyPhrases->Get(keyUnitConsumUnit);
 }
 if(GetEndFlag(FLAG_CONSUMING_FACTION)){
  s+=KeyPhrases->Get(keyUnitConsumFaction);
 }
 vli=new VListItem(vlis);
 vli->type=litUnitInfo;
 vli->text=s;

 if(endobj!=baseobj||endobj->endreg!=baseobj->basereg){
  s="Located in ";
  if(endobj->num){
   s+=endobj->FullName();
   s+=" in ";
  }
  s+=endobj->endreg->FullName();

  vli=new VListItem(vlis);
  vli->type=litUnitInfo;
  vli->text=s;
 }
 vli=new VListItem(vlis);
 vli->type=litUnitInfo;
 vli->text=enditems->Print();

 if(endskills->count){
  vli=new VListItem(vlis);
  vli->type=litUnitInfo;
// vli->value=num;
  vli->text=endskills->Print(1+4+8);
 }
 s="Weight: ";
 s+=weight;
 s+="/";
 s+=walkweight;
 if(walkweight<weight) s+="-";
 s+="/";
 s+=ridweight;
 if(ridweight<weight) s+="-";
 s+="/";
 s+=flyweight;
 if(flyweight<weight) s+="-";
 vli=new VListItem(vlis);
 vli->type=litUnitInfo;
 vli->text=s;

 AFaction *fac=faction;
// if(!fac) return;
 if(!fac->local) return; 
 TStringList *slist=new TStringList;

 for(int i=0;i<fac->events->Count;i++){
  AnsiString str=fac->events->Strings[i];
  int pos=str.Pos('(');
  if(!pos) continue;
  int unnum=atoi(str.c_str()+pos);
  if(unnum!=num) continue;
   slist->Add(str);
 }
 if(!slist->Count){
  delete slist;
  return;
 }
 vli=new VListItem(vlis);
 vli->type=litUnitInfo;

 vli=new VListItem(vlis);
 vli->type=litUnitEvents;
 vli->value=-fac->number;
 vli->text="Events:";

 for(int i=0;i<slist->Count;i++){
  vli=new VListItem(vlis);
  vli->type=litUnitEvent;
  vli->value=-fac->number;
  vli->text=slist->Strings[i];
 }
 delete slist;
}
void AUnit::MakeTree(TTreeNode * parnode, VTreeNodes * vtns)
{
 AnsiString str=PrintForTree();
 VTreeNode *vtn;
 vtn=new VTreeNode(vtns);
 vtn->type=tntUnit;
 vtn->value=num;
 vtn->data=this;
 TTreeNode *node=parnode->Owner->AddChildObject(parnode,str,vtn);
 vtn->node=node;
 bool value=IsLocal();
 TV_ITEM tv;
 tv.hItem=node->ItemId;
 tv.mask=TVIF_STATE;
 tv.stateMask=TVIS_BOLD;
 if(value){
  tv.state=0xffffffff;
 }else{
  tv.state=0;
 }
 TreeView_SetItem(node->Handle,&tv);

}
bool AUnit::IsLocal()
{
 return info==INFO_LOCAL;
}

void AUnit::RunOrders()
{
// enditems->Update(items);

/* foreach(orders, AOrder *ord)
  ord->Run(this);
 }*/
 baseobj->basereg->RunOrders();
// enditems->DeleteEmpty();
}

AUnits::AUnits(AObject *obj)
 :TList(),object(obj),autodelete(true)
{
}
__fastcall AUnits::~AUnits()
{
}
void __fastcall AUnits::Clear()
{
 if(autodelete){
  int i=Count-1;
  if(i<0) return;
  AObject **iter=(AObject**)List+i;
  for(;i>=0;i--,iter--){
   delete *(AUnit**)iter;
 /* for(int i=Count-1;i>=0;i--){
   AUnit * u=Get(i);*/
//   TList::Delete(i);
 //  delete u;
  }
 }
 TList::Clear();
}
void AUnits::Read(TFile& in)
{
 Clear();
 int kol;
 in>>kol;
 for(int i=0;i<kol;i++){
  AUnit *un=new AUnit(object->turn);
  un->Read(in);
  Add(un);
 }
}
void AUnits::Write(TFile & out)
{
 int kol=count;
 out<<kol;
 foreach(this){
  AUnit *un=*(AUnit**)iter;
  un->Write(out);
 }
}
AUnit* AUnits::Get(int index)
{
 return (AUnit*)Items[index];
}
AUnit* AUnits::GetUnit(int num)
{
 foreach(this){
  AUnit *unit=*(AUnit**) iter;
  if(unit->num==num)return unit;
 }
 return 0;
}
bool AUnits::Delete(int num)
{
// for(int i=Count-1;i>=0;i--){
 int i=Count-1;
 AObject **iter=(AObject**)List+i;
 for(;i>=0;i--,iter--){
  AUnit * u=*(AUnit**)iter; //Get(i);
  if(u->num==num){
   TList::Delete(i);
   if(autodelete) delete u;
   return true;
  }
 }
 return false;
}
void AUnits::Add(AUnit* un)
{
 if(GetUnit(un->num))
  throw Exception("Units::Add: Already Exits");
 if(autodelete)un->baseobj=object;
 TList::Add(un);
}
void AUnits::CreateNew(AUnits* prvunits)
{
 Clear();
//do nothing
}
void AUnits::Update(AUnits *newunits)
{
 AUnit *un;
 foreach(newunits){
  AUnit *newun=*(AUnit**) iter;
  un=GetUnit(newun->num);
  if(!un){
   un=new AUnit(object->turn);
   un->Update(newun);
   Add(un);
  }else{
   un->Update(newun);
  }
 }
}
AnsiString AUnit::GetLocalDescr()
{
 AOrderLocDescr *ord=dynamic_cast<AOrderLocDescr *>(orders->Find(O_LOCALDESCR));
 if(!ord)return "";
 return ord->text;
}


AOrder * AUnit::GetOrder(int num)
{
 if(num<0||num>=orders->count)return 0;
 return orders->Get(num);
}


bool AUnit::SaveTemplate(TFileStream * file)
{
 AnsiString s;
 if(num<0){
  s="form ";
  s+=-num;
  s+="\n";
 }else{
  s="unit ";
  s+=num;
  s+="\n";
 }
 file->Write(s.c_str(),s.Length());
 orders->Compact();
 foreach(orders){
  AOrder *ord=*(AOrder**) iter;
  s=ord->WriteTemplate();
  if(!s.Length())continue;
  s+="\n";
  file->Write(s.c_str(),s.Length());
 }
 if(num<0){
  s="end\n";
  file->Write(s.c_str(),s.Length());
 }

 s="\n";
 file->Write(s.c_str(),s.Length());

 return true;
}


void AUnit::FormNew(AUnit *base)
{
 name="";
 faction=base->faction;
 info=INFO_LOCAL;
 orders->Clear();
 items->Clear();
 guard=GUARD_NONE;
 reveal=REVEAL_NONE;
 flags=0;
}
int AUnit::GetMen(AnsiString type)
{
 return enditems->GetNum(type);
}
int AUnit::GetMen()
{
 int n=0;
 foreach(enditems){
  AItem *i=*(AItem**) iter;
  AItemType *itype=i->GetType();
  if(!itype)continue;
  if(itype->type&IT_MAN)n+=i->count;
 }
 return n;
}
int AUnit::GetBegMen()
{
 int n=0;
 foreach(items){
  AItem *i=*(AItem**) iter;
  AItemType *itype=i->GetType();
  if(!itype)continue;
  if(itype->type&IT_MAN)n+=i->count;
 }
 return n;
}


int AUnit::GetAttitude(ARegion * reg, AUnit * u)
{
 if(faction==u->faction)return A_ALLY;
 if(!faction->number)
  if(u->faction->number)return A_NEUTRAL;
//  else return A_ALLY;
 if(!u->faction->number) return faction->defattitude;
 int att=faction->GetAttitude(u->faction->number);
 if(!faction->local)return att;
 if (att>=A_FRIENDLY && att >= faction->defattitude) return att;
 if (CanSee(reg,u) == 2) {
  return att;
 }else{
  return faction->defattitude;
 }
}
bool AUnit::IsAlive()
{
 foreach(enditems){
  AItem *it=*(AItem**) iter;
  if(!it->count) continue;
  AItemType *itype=it->GetType();
  if(itype->type&(IT_MAN|IT_MONSTER)) return true;
 }
 return false;
}
int AUnit::CanSee(ARegion * r, AUnit * u)
{
 if(!r) return 2;
 return faction->CanSee(r,u);
}
void AUnit::SetFaction(int facnum)
{
 faction=turn->factions->GetFaction(facnum);
}


bool AUnit::HasTaxOrder()
{
 AOrder *ord=orders->Find(O_TAX,2);
 if(ord)return true;
 //autotax ?
 bool b=GetEndFlag(FLAG_AUTOTAX);

 AOrderByte *bord=dynamic_cast<AOrderByte*>(orders->Find(O_AUTOTAX,2));
 if(bord){
  b=bord->value;
 }
 if(!b)return false;
 ord=new AOrderSimple;
 ord->type=O_TAX;
 orders->Add(ord);
 return true;
}


int AUnit::Taxers()
{
 int TotalMen=GetMen();
 if(!TotalMen) return 0;
 if(GetSkill("COMB")) return TotalMen;
 int numNoWeapons=TotalMen;
 foreach(enditems){
  AItem *item=*(AItem**) iter;
  AItemType *itype=ItemTypes->Get(item->type);
  if(!(itype->type&IT_WEAPON)) continue;
  numNoWeapons-=item->count;
  if(numNoWeapons<=0) return TotalMen;
 }
 return TotalMen-numNoWeapons;
}
void AUnit::SetMoney(int count)
{
 enditems->SetNum("SILV",count);
}
int AUnit::GetMoney()
{
 return enditems->GetNum("SILV");
}


AnsiString AUnit::Alias()
{
 if(num>=0)return AnsiString(num);
 AnsiString str;
 if(faction&&faction->number){
  str="faction ";
  str+=faction->number;
  str+=" ";
 }
 str+="new ";
 str+=-num;
 return str;
}


int AUnit::GetBegRealSkill(AnsiString type)
{
 ASkill *sk=skills->GetSkill(type);
 if(!sk)return 0;
 return sk->GetLevel();
}
int AUnit::GetBegSkill(AnsiString type)
{
 if(type=="OBSE") return GetBegObservation();
 if(type=="STEA") return GetBegStealth();
 return GetBegRealSkill(type);
}
int AUnit::GetRealSkill(AnsiString type)
{
 ASkill *sk=endskills->GetSkill(type);
 if(!sk)return 0;
 return sk->GetLevel();
}
int AUnit::GetSkill(AnsiString type)
{
 return GetRealSkill(type);
}
int AUnit::GetBegSkillDays(AnsiString type)
{
 ASkill *sk=skills->GetSkill(type);
 if(!sk)return 0;
 return sk->days;
}
int AUnit::GetSkillDays(AnsiString type)
{
 ASkill *sk=endskills->GetSkill(type);
 if(!sk)return 0;
 return sk->days;
}
void AUnit::SetSkillDays(AnsiString type, int val)
{
 ASkill *sk=endskills->GetSkill(type);
 if(!sk){
  sk=new ASkill;
  sk->type=type;
  endskills->Add(sk);
 }
// if(val>450) val=450;
 sk->days=val;
}
bool AUnit::IsLeader()
{
 return (enditems->GetItem("LEAD")!=0);
}
bool AUnit::IsNormal()
{
 return GetMen()&&!IsLeader();
}
void AUnit::UpdateWeight()
{
 int w=0,ww=0,rw=0,fw=0;
 foreach(enditems){
  AItem *it=*(AItem**) iter;
  AItemType *itype=ItemTypes->Get(it->type);
  w+=itype->weight*it->count;
  ww+=itype->walk*it->count;
  rw+=itype->ride*it->count;
  fw+=itype->fly*it->count;
 }
 int wagons = enditems->GetNum("WAGO");
 int horses = enditems->GetNum("HORS");
 if(wagons>horses) wagons=horses;
 ww+=wagons*250;// Globals->WAGON_CAPACITY;
 weight=w;
 walkweight=ww;
 ridweight=rw;
 flyweight=fw;
}
bool AUnit::CheckDepend(int lev, AnsiString dep, int deplev)
{
 int temp=GetRealSkill(dep);
 if(temp<deplev) return false;
 if(lev>=temp) return false;
 return true;
}

bool AUnit::CanStudy(AnsiString sk)
{
 ASkillType *stype=SkillTypes->Get(sk,2);
 if(!stype) return false;
 int curlev = GetRealSkill(sk);
 if(stype->depends[0].Length()){
  if(!CheckDepend(curlev,stype->depends[0],stype->levels[0]))
   return false;
 }else return true;
 if(stype->depends[1].Length()){
  if(!CheckDepend(curlev,stype->depends[1],stype->levels[1]))
   return false;
 }else return true;
 if(stype->depends[2].Length()){
  if(!CheckDepend(curlev,stype->depends[2],stype->levels[2]))
   return false;
 }else return true;
 return true;
}
int AUnit::MoveType()
{
 int w=weight;
 if(w<=flyweight) return M_FLY;
 if(w<=ridweight) return M_RIDE;
 if(w<=walkweight) return M_WALK;
 return M_NONE;
}


int AUnit::CalcMovePoints()
{
 switch (MoveType()) {
  case M_NONE:
  return 0;
  case M_WALK:
  return 2;
  case M_RIDE:
  return 4;
  case M_FLY:
   if (GetSkill("SWIN")) return 6 + 2;
  return 6;
 }
 return 0;
}
int AUnit::GetBegObservationBonus()
{
 int bonus=0;
 int men=GetMen();
 if(men==1)
  bonus=(GetBegSkill("TRUE")+1)/2;
 if(bonus!=3&&men&&items->GetNum("AMTS"))
  bonus=2;
 return bonus;
}
int AUnit::GetObservationBonus()
{
 int bonus=0;
 int men=GetMen();
 if(men==1)
  bonus=(GetSkill("TRUE")+1)/2;
 if(bonus!=3&&men&&enditems->GetNum("AMTS"))
  bonus=2;
 return bonus;
}


int AUnit::GetBegObservation()
{
 int retval=GetBegRealSkill("OBSE");
 retval+=GetBegObservationBonus();
//todo
 return retval;
}

int AUnit::GetBegStealthBonus()
{
 /*   if (GetFlag(FLAG_INVIS) || */
 if(GetMen()<=items->GetNum("RING")){
  return 3;
 }
 return 0;
}
int AUnit::GetStealthBonus()
{
 /*   if (GetFlag(FLAG_INVIS) || */
 if(GetMen()<=enditems->GetNum("RING")){
  return 3;
 }
 return 0;
}


int AUnit::GetBegStealth()
{
 int retval=GetBegRealSkill("STEA");
 retval+=GetBegStealthBonus();
//todo
 return retval;
}


bool AUnit::IsMage()
{
 if(canstudy->Count) return true;
 if(combatspell.Length()) return true;
 if(GetBegRealSkill("FORC")) return true;
 if(GetBegRealSkill("PATT")) return true;
 if(GetBegRealSkill("SPIR")) return true;
 return false;
}

int AUnit::GetSize()
{
 int siz=sizeof(*this);
 siz+=name.Length()+1;
 siz+=describe.Length()+1;
 siz+=orders->GetSize();
 siz+=items->GetSize();
 siz+=enditems->GetSize();
 siz+=skills->GetSize();
 siz+=endskills->GetSize();
 siz+=combatspell.Length()+1;
 siz+=sizeof(TStringList);
 siz+=canstudy->Text.Length()+1;
 return siz;
}
int AUnits::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 foreach(this){
  AUnit *un=*(AUnit**)iter;
  siz+=un->GetSize();
 }
 return siz;
}

