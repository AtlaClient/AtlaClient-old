//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "util.h"
#include "skills.h"
#include "eapfile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

ASkill::ASkill()
 :days(0)
{
}
__fastcall ASkill::~ASkill()
{
}
void ASkill::Read(TFile& in)
{
 ReadStringAsInt(in,type);
 ReadIntAsShort(in,days);
}
void ASkill::Write(TFile & out)
{
 WriteStringAsInt(out,type);
 WriteIntAsShort(out,days);
}
void ASkill::CreateNew(ASkill * newskill)
{
 type=newskill->type;
 days=newskill->days;
}
AnsiString ASkill::Print(int mode)
{
 AnsiString s;
 ASkillType *stype=GetType();
 if(!stype) return "";

 if(mode&1)s=stype->name;
 if(mode&2){
  s+=" [";
  s+=type;
  s+="]";
 }
 if(mode&4){
  s+=" ";
  s+=GetLevel();
 }
 if(mode&8){
  s+=" (";
  s+=days;
  s+=")";
 }
 return s;
}
ASkillType* ASkill::GetType()
{
 return SkillTypes->Get(type,2); //abr find
}

int ASkill::GetLevel()
{
 int dayspermen=days;
 int z=30;
 int i=0;
 while(dayspermen>=z){
  i++;
  dayspermen-=z;
  z+=30;
 }
 return i;
}
ASkills::ASkills()
{
}
__fastcall ASkills::~ASkills()
{
}
void __fastcall ASkills::Clear()
{
 for(int i=count-1;i>=0;i--){
  ASkill *sk=Get(i);
//  TList::Delete(i);
  delete sk;
 }
 TList::Clear();
}
void ASkills::Read(TFile& in)
{
 Clear();
 int kol;
 in>>kol;
 for(int i=0;i<kol;i++){
  ASkill *sk=new ASkill;
  sk->Read(in);
  Add(sk);
 }
}
void ASkills::Write(TFile& out)
{
 int kol=count;
 out<<kol;
 for(int i=0;i<kol;i++){
  ASkill *sk=Get(i);
  sk->Write(out);
 }
}
ASkill * ASkills::Get(int index)
{
 return (ASkill*)Items[index];
}
ASkill * ASkills::GetSkill(AnsiString type)
{
 foreach(this){
  ASkill *sk=*(ASkill**)iter;
  if(sk->type==type)return sk;
 }
 return 0;
}
void ASkills::Add(ASkill *sk)
{
 if(GetSkill(sk->type))
  throw Exception("Skills::Add: Already Exits");
 TList::Add(sk);
}
void ASkills::CreateNew(ASkills * prvsks)
{
 Clear();
 UpdateFromPrev(prvsks);
}
void ASkills::Update(ASkills *newsks)
{
 foreach(newsks){
  ASkill *newsk=*(ASkill**)iter;
  ASkill *sk=GetSkill(newsk->type);
  if(!sk){
   sk=new ASkill;
   sk->CreateNew(newsk);
   Add(sk);
  }else{
   sk->days=newsk->days;
  }
 }
}
void ASkills::UpdateFromPrev(ASkills *prvsks)
{
 foreach(prvsks){
  ASkill *prvsk=*(ASkill**)iter;
  ASkill *sk=GetSkill(prvsk->type);
  if(sk)continue;
  sk=new ASkill;
  sk->CreateNew(prvsk);
  Add(sk);
 }
}
AnsiString ASkills::Print(int mode)
{
 AnsiString s;
 if(!count) return KeyPhrases->Get(keyNoneSign);
 foreach(this){
  ASkill *sk=*(ASkill**)iter;
  s+=sk->Print(mode)+", ";
 }
 s.SetLength(s.Length()-2);
 return s;
}
void ASkills::DeleteEmpty()
{
 for(int i=Count-1;i>=0;i--){
  ASkill *sk=Get(i);
  if(sk->days<=0){
   TList::Delete(i);
   delete sk;
  }
 }
}


int ASkill::GetSize()
{
 int siz=sizeof(*this);
 siz+=type.Length()+1;
 return siz;
}


int ASkills::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 foreach(this){
  ASkill *sk=*(ASkill**)iter;
  siz+=sk->GetSize();
 }
 return siz;
}

