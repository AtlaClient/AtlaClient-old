//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "util.h"
#include "items.h"
#include "eapfile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

AItem::AItem()
 :count(0)
{
}
__fastcall AItem::~AItem()
{
}
void AItem::Read(TFile& in)
{
 ReadStringAsInt(in,type);
 in>>count;
}
void AItem::Write(TFile& out)
{
 WriteStringAsInt(out,type);
 out<<count;
}
void AItem::CreateNew(AItem *newit)
{
 type=newit->type;
 count=newit->count;
}
AnsiString AItem::Print(int mode)
{
 AnsiString s;
 if(mode&1)
 if(count!=1){
  s+=count;
  s+=" ";
 }
 AItemType *itype=GetType();
 if(!itype){
  s+="? [?]";
  return s;
 }
 if(mode&2){
  if(count<2)s+=itype->name;
  else s+=itype->names;
 }
 if(mode&4){
  bool illusion=ItemTypes->IsIllusionAbr(itype->abr);
  s+=" [";
  if(illusion)
   s+=ItemTypes->IllusionAbrToNormal(itype->abr);
  else
   s+=itype->abr;
  s+="]";
  if(illusion)
   s+=KeyPhrases->Get(keyItemIllusionSign);
 }
 //illus
 return s;
}
AnsiString AItem::WriteReport(bool seeillusions){
 AnsiString s;
 if(count!=1){
  s+=count;
  s+=" ";
 }
 AItemType *itype=GetType();
 if(!itype) return "";
 if(count<2)s+=itype->name;
 else s+=itype->names;

 bool illusion=ItemTypes->IsIllusionAbr(itype->abr);
 s+=" [";
 if(illusion)
  s+=ItemTypes->IllusionAbrToNormal(itype->abr);
 else
  s+=itype->abr;
 s+="]";
 if(seeillusions&&illusion)
  s+=KeyPhrases->Get(keyItemIllusionSign);
 return s;
}

AItemType * AItem::GetType()
{
 return ItemTypes->Get(type,4); //abr find
}
AItems::AItems()
{
}
__fastcall AItems::~AItems()
{
 Clear();
}
void AItems::Read(TFile& in)
{
 Clear();
 int kol;
 in>>kol;
 for(int i=0;i<kol;i++){
  AItem *it=new AItem;
  it->Read(in);
  Add(it);
 }
}
void AItems::Write(TFile & out)
{
 int kol=count;
 out<<kol;
 for(int i=0;i<kol;i++){
  AItem *it=Get(i);
  it->Write(out);
 }
}
void __fastcall AItems::Clear()
{
 for(int i=Count-1;i>=0;i--){
  AItem *it=Get(i);
//  TList::Delete(i);
  delete it;
 }
 TList::Clear();
}
AItem* AItems::Get(int index)
{
 return (AItem*)Items[index];
}
AItem* AItems::GetItem(AnsiString type)
{
 foreach(this){
  AItem *it=*(AItem**)iter;
  if(it->type==type)return it;
 }
 return 0;
}
void AItems::Add(AItem *item)
{
 if(GetItem(item->type))
  throw Exception("Items::Add: Already Exits");
 TList::Add(item);
}
void AItems::CreateNew(AItems * prvitems)
{
 Clear();
 UpdateFromPrev(prvitems);
}
void AItems::UpdateFromPrev(AItems *prvitems)
{
 foreach(prvitems){
  AItem *prvit=*(AItem**)iter;
  AItem *it=GetItem(prvit->type);
  if(it)continue;
  it=new AItem;
  it->CreateNew(prvit);
  Add(it);
 }
}
void AItems::Update(AItems *newitems)
{
 foreach(newitems){
  AItem *newit=*(AItem**)iter;
  AItem *it=GetItem(newit->type);
  if(!it){
   it=new AItem;
   it->CreateNew(newit);
   Add(it);
  }else it->count=newit->count;
 }
}

AnsiString AItems::Print()
{
 AnsiString s;
 foreach(this){
  AItem *it=*(AItem**)iter;
  s+=it->Print()+", ";
 }
 s.SetLength(s.Length()-2);
 return s;
}
AnsiString AItems::WriteReport(int obs,bool seeillusions,bool nofirstcomma){
 AnsiString s;
 foreach(this){
  AItem *it=*(AItem**)iter;
  if(obs!=2){
   AItemType *itype=it->GetType();
   if(!itype->weight) continue;
  }
  if(nofirstcomma){
   nofirstcomma=false;
  }else{
   s+=", ";
  }
  s+=it->WriteReport(seeillusions);
 }
 return s;
}
int AItems::GetNum(AnsiString type)
{
 AItem *it=GetItem(type);
 if(!it)return 0;
 return it->count;
}
void AItems::SetNum(AnsiString type, int num)
{
 AItem *it=GetItem(type);
 if(!it){
  it=new AItem;
  it->type=type;
  Add(it);
 }
 it->count=num;
}
void AItems::DeleteEmpty()
{
 for(int i=Count-1;i>=0;i--){
  AItem *it=Get(i);
  if(it->count==0){
   TList::Delete(i);
   delete it;
  }
 }
}


int AItem::GetSize()
{
 int siz=sizeof(*this);
 siz+=type.Length()+1;
 return siz;
}
int AItems::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 foreach(this){
  AItem *it=*(AItem**)iter;
  siz+=it->GetSize();
 }
 return siz;
}

