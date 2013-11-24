//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <shellapi.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <classes.hpp>
#include <FileCtrl.hpp>

#include "util.h"
#include "EventsFrm.h"
#include "turn.h"
#include "faction.h"
#include "region.h"
#include "reports.h"
#include "mainfrm.h"
#include "unit.h"
#include "eapfile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
bool optShowEvents=true;
bool optRunRegionOrders=true;
int optTownNameLimit=5;
bool optMaximizeWindow=true;
int optDontShowEvents;
int optSaveMap=roNonuserObject|roAllRegion|roRegionInfo;

AnsiString OrdersDir="orders\\",ReportsDir="reports\\",StoreDir="store\\";

void AddEvent(AnsiString ev){
 EventsForm->AddEvent(ev);
}
void ClearEvents(){
 EventsForm->ClearEvents();
}
TMemoryStream* LoadFile(AnsiString fName){
 TMemoryStream *mem=new TMemoryStream;
 mem->LoadFromFile(fName);
 mem->SetSize(mem->Size+1);
 char *from=((char*)mem->Memory),*to=from;
 from[mem->Size-1]=0;
 while(*from){
  if(*from!='\r'){
   *to++=*from;
  }
  from++;
 }
 *to=0;
 return mem;
}
int ReportTab=0;
AnsiString Trunc(AnsiString &str,int len=70){
 if(str.Length()<=len) return "";
 char *s=str.c_str();
 int pos=len;
 while(pos>=0&&s[pos]!=' ') pos--;
 if(pos<0){
  return "";
 }
 AnsiString str2=str.SubString(pos+2,str.Length());
 str.SetLength(pos);
 return str2;
}
void SaveRepString(TStream *strm, AnsiString str){
 if(str.Pos("\n")){
  TStringList *list=new TStringList;
  list->Text=str;
  SaveRepStringList(strm,list);
  delete list;
  return;
 }
 bool comment=false;
 if(*str.c_str()==';'){
  comment=false;
  str=str.SubString(2,str.Length());
 }
 AnsiString tabchar=AnsiString::StringOfChar(' ',ReportTab);
 if(*str.c_str()!=' '&&*str.c_str()!=0)
  str=tabchar+str;
 AnsiString s,str2=Trunc(str);
 {
  if(comment) s=";"; else s="";
  s+=str+"\n";
  strm->Write(s.c_str(),s.Length());
 }
 while(str2.Length()){
  str="  "+tabchar+str2;
  str2=Trunc(str);
  if(comment) s=";"; else s="";
  s+=str+"\n";
  strm->Write(s.c_str(),s.Length());
 }
}
void SaveRepStringList(TStream *strm, TStringList *list){
 AnsiString s;
 for(int i=0,endi=list->Count;i<endi;i++){
  SaveRepString(strm,list->Strings[i]);
 }
}

void ReadStringList(TFile &in,TStringList *list){
 int kol;
 in>>kol;
 AnsiString s;
 list->Clear();
 for(int i=0;i<kol;i++){
  in>>s;
  list->Add(s);
 }
}
void WriteStringList(TFile &out,TStringList *list){
 int kol=list->Count;
 out<<kol;
 AnsiString s;
 for(int i=0;i<kol;i++){
  s=list->Strings[i];
  out<<s;
 }
}
int CompareIStr(AnsiString s1, AnsiString s2){
 return stricmp(s1.c_str(),s2.c_str());
}
bool MyCompare(const char *str,AnsiString pattern){
 return strncmpi(str, pattern.c_str(), pattern.Length());
/* register char *s=str,*pat=pattern.c_str();
 while(*pat){
  if(*pat++!=*s++)return true;
 }*/
/* _ESI=(long)str;_EDI=(long)pattern.c_str();
 while(*(char*)_EDI){
  if(*(char*)_ESI!=*(char*)_EDI)return true;
  _ESI++;_EDI++;
 }
 return false;*/
}
const char*endtoken=" \n,.():;[]&|>=<!{}"; //+-*/
AnsiString GetNextToken(char*&buf){
 char*s,*old=buf;
 if(*buf==0) return "";
 if(*buf=='\"'){
  old++;
  s=buf+1;
  while(*s!='\"')s++;
  s++;
  buf=s;
  return AnsiString(old,s-old-1);
 }else{
  s=buf+1;
  while(*s&&strchr(endtoken,*s)==0)s++;
  buf=s;
  return AnsiString(old,s-old);
 }
}
AnsiString GetNextString(char*&buf, int nextstrtab){
 bool verend=false;
 int worktab=nextstrtab;
 if(nextstrtab<0){ verend=true;worktab=-nextstrtab;}
 char *s=buf;
 while(*s&&*s!='\n')s++;
 AnsiString str(buf,s-buf);
 if(*s=='\n')s++;
 if(nextstrtab){
  int i;
  for(i=0;i<worktab;i++){
   if(s[i]!=' ')break;
  }
  if(i==worktab){
   if(!verend||(s[worktab+1]!='-'&&s[worktab+1]!='*')){
    s+=worktab;
    str+=GetNextString(s,nextstrtab);
   }
  }
 }
 buf=s;
 return str;
}
AnsiString GetNextFactionName(char*&buf){
 char* s=buf;
 while(*s!='('&&*s!='\n'&&*s/*&&*s!=','&&*s!=';'&&*s!='.'*/)s++;
 if(*s!='(')return "";
 AnsiString str(buf,s-buf-1);
 buf=s+1;
 return str;
}
AnsiString GetNextItemName(char*&buf){
 char* s=buf;
 while(*s!='['&&*s!='\n'&&*s)s++;
 if(*s!='[')return "";
 AnsiString str(buf,s-buf-1);
 buf=s+1;
 return str;
}
ALocation* GetNextLocation(char*&buf){
 int x,y,z;
 char *s=buf;
 if(*s!='(')return 0;
 s++;
 x=GetNextToken(s).ToInt();
 if(*s!=',')return 0;
 s++;
 y=GetNextToken(s).ToInt();
 AnsiString levelname;
 if(*s==','){
  s++;
  levelname=GetNextToken(s);
 }
 if(!ProcessUpdateRegionArray(levelname))return 0;
 z=curRegionList->RegionArrays->GetLevelNum(levelname);
 if(z==-1){
  return 0;
 }
 if(*s!=')')return 0;
 s++;
 ALocation *loc=new ALocation;
 loc->xloc=x;
 loc->yloc=y;
 loc->zloc=z;
 buf=s;
 return loc;
}
const char*TerrainTxt="terrain.txt";
ATerrainTypes*TerrainTypes;
ATerrainType::ATerrainType()
 :movepoints(0),mountstypes(0),color(clGray)
{
 advlist=new TStringList;
}
ATerrainType::~ATerrainType()
{
 delete advlist;
}
AnsiString ATerrainType::Print(bool quotation)
{
 AnsiString s;
 if(quotation)s+="\"";
 s+=name;
 if(quotation)s+="\"";
 s+=",";
 s+=movepoints;
 if(mountstypes&mtRiding)s+=",RIDING";
 if(mountstypes&mtFlying)s+=",FLYING";
 for(int i=0;i<advlist->Count;i++){
  s+=",\""+advlist->Strings[i]+"\"";
 }
 s+=":";
 s+=ColorToString(color);
 return s;
}
bool ATerrainType::CanProduceAdv(AnsiString abr)
{
 return advlist->IndexOf(abr)!=-1;
}

ATerrainTypes::ATerrainTypes()
 :TList()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(TerrainTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 ATerrainType*ttype;
 while(*s=='\"'){
  ttype=new ATerrainType;
  ttype->name=GetNextToken(s);
  if(*s!=','){
   throw Exception(AnsiString("Bad format by \"")+ttype->name+"\": movecost not found");
  }
  s++;
  str=GetNextToken(s);
  ttype->movepoints=str.ToInt();
  while(*s==','){
   s++;
   if(*s=='\"'){
    str=GetNextToken(s);
    AItemType *itype=ItemTypes->Get(str);
    if(!itype)
     throw Exception(AnsiString("Bad format by \"")+ttype->name+"\": unknown adv item type: "+str);
    if((itype->type&IT_ADVANCED)==0)
     throw Exception(AnsiString("Bad format by \"")+ttype->name+"\": item type: "+str+" is not advanced");
    ttype->advlist->Add(itype->abr);
   }else{
    str=GetNextToken(s);
    if(str=="RIDING")  ttype->mountstypes|=mtRiding;
    else if(str=="FLYING") ttype->mountstypes|=mtFlying;
    else throw Exception(AnsiString("Bad format by \"")+ttype->name+"\": unknown mountstype: "+str);
   }
  }
  if(*s==':'){
   s++;
   str=GetNextToken(s);
   ttype->color=StringToColor(str);
  }
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+ttype->name+"\": unknown mountstype");
  s++;
  AddTerrainType(ttype);
 }
 delete mem;
 modified=false;
}
__fastcall ATerrainTypes::~ATerrainTypes()
{
 for(int i=Count-1;i>=0;i--){
  ATerrainType*tt=(ATerrainType*)Items[i];
//  Delete(i);
  delete tt;
 }
 TList::Clear();
 if(TerrainTypes==this)TerrainTypes=0;
}
ATerrainType* ATerrainTypes::Get(int index)
{
 return (ATerrainType*)Items[index];
}
void ATerrainTypes::AddTerrainType(ATerrainType * ttype)
{
 for(int i=Count-1;i>=0;i--){
  if(ttype->name==Get(i)->name)throw Exception(AnsiString("Terrain \"")+ttype->name+"\" already present");
 }
 Add(ttype);
 modified=true;
}
void ATerrainTypes::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(TerrainTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print(true)+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
 modified=false;
}

ATerrainType* ATerrainTypes::Get(AnsiString terrain)
{
 if(!terrain.Length()) return 0;
 for(int i=Count-1;i>=0;i--){
  if(!CompareIStr(terrain,Get(i)->name))return Get(i);
 }
 return 0;
}
const char*WeatherTxt="weather.txt";
AWeatherTypes*WeatherTypes;
AWeatherType::AWeatherType()
 :movemult(2)
{
}
AnsiString AWeatherType::Print()
{
 AnsiString s="\"";
 s+=name+"\","+movemult+",\""+curmonthname+"\",\""+nextmonthname+"\"";
 return s;
}
AWeatherTypes::AWeatherTypes()
 :TList()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(WeatherTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 AWeatherType*wtype;
 while(*s=='\"'){
  wtype=new AWeatherType;
  wtype->name=GetNextToken(s);
  if(*s!=','){
   throw Exception(AnsiString("Bad format by \"")+wtype->name+"\": movemult not found");
  }
  s++;
  str=GetNextToken(s);
  wtype->movemult=str.ToInt();
  if(*s!=','||s[1]!='\"'){
   throw Exception(AnsiString("Bad format by \"")+wtype->name+"\": curmounthname not found");
  }
  s++;
  str=GetNextToken(s);
  wtype->curmonthname=str;
  if(*s!=','||s[1]!='\"'){
   throw Exception(AnsiString("Bad format by \"")+wtype->name+"\": nextmounthname not found");
  }
  s++;
  str=GetNextToken(s);
  wtype->nextmonthname=str;
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+wtype->name+"\": bad end");
  s++;
  AddWeatherType(wtype);
 }
 delete mem;
 modified=false;
}
__fastcall AWeatherTypes::~AWeatherTypes()
{
 for(int i=Count-1;i>=0;i--){
  AWeatherType*wt=(AWeatherType*)Items[i];
//  Delete(i);
  delete wt;
 }
 TList::Clear();
 if(WeatherTypes==this)WeatherTypes=0;
}
AWeatherType* AWeatherTypes::Get(int index)
{
 return (AWeatherType*)Items[index];
}
AWeatherType* AWeatherTypes::Get(AnsiString name)
{
 foreach(this){
  AWeatherType *wtype=*(AWeatherType**)iter;
  if(wtype->name==name)return wtype;
 }
 return 0;
}
void AWeatherTypes::AddWeatherType(AWeatherType * wtype)
{
 for(int i=Count-1;i>=0;i--){
  if(wtype->name==Get(i)->name)throw Exception(AnsiString("Weather \"")+wtype->name+"\" already present");
 }
 Add(wtype);
 modified=true;
}
void AWeatherTypes::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(WeatherTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print()+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
 modified=false;
}
AnsiString AWeatherTypes::FindCurWeather(AnsiString text)
{
 foreach(this){
  AWeatherType *wtype=*(AWeatherType**)iter;
  if(wtype->curmonthname==text)return wtype->name;
 }
 return "";
}
AnsiString AWeatherTypes::FindNextWeather(AnsiString text)
{
 foreach(this){
  AWeatherType *wtype=*(AWeatherType**)iter;
  if(wtype->nextmonthname==text)return wtype->name;
 }
 return "";
}
const char*DirectTxt="direct.txt";
AnsiString ADirType::Print()
{
 AnsiString s="\"";
 s+=name;
 s+="\",\"";
 s+=abr;
 s+="\",";
 s+=num;
 return s;
}
ADirTypes*DirTypes;
ADirTypes::ADirTypes()
 :TList()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(DirectTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 ADirType*dtype;
 while(*s=='\"'){
  dtype=new ADirType;
  dtype->name=GetNextToken(s);
  if(*s!=','){
   throw Exception(AnsiString("Bad format by \"")+dtype->name+"\": abridgement not found");
  }
  s++;
  str=GetNextToken(s);
  dtype->abr=str;
  if(*s!=','){
   throw Exception(AnsiString("Bad format by \"")+dtype->name+"\": number not found");
  }
  s++;
  dtype->num=GetNextToken(s).ToInt();
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+dtype->name+"\": bad end");
  s++;
  Add(dtype);
 }
 if(Count>NDIRS)throw Exception("Too many directions");
 delete mem;
 modified=false;
}
__fastcall ADirTypes::~ADirTypes()
{
 for(int i=Count-1;i>=0;i--){
  ADirType*dt=Get(i);
//  Delete(i);
  delete dt;
 }
 TList::Clear();
 if(DirTypes==this)DirTypes=0;
}
ADirType* ADirTypes::Get(int index)
{
 return (ADirType*)Items[index];
}
void ADirTypes::AddDirType(ADirType * dtype)
{
 if(dtype->num!=count)throw Exception("Error ADirTypes::AddDirType(): number not match");
 if(Find(dtype->name)!=-1)throw Exception(AnsiString("Direction \"")+dtype->name+"\" already present");
 if(Find(dtype->abr)!=-1)throw Exception(AnsiString("Direction \"")+dtype->abr+"\" already present");
 Add(dtype);
 modified=true;
}
int ADirTypes::Find(AnsiString str)
{
 for(int i=Count-1;i>=0;i--){
  ADirType *dt=Get(i);
  if(!dt->name.AnsiCompareIC(str)||!dt->abr.AnsiCompareIC(str))return i;
 }
 return -1;
}
void ADirTypes::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(DirectTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<count;i++){
  AnsiString s=Get(i)->Print()+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
}
const char*SkillsTxt="skills.txt";
ASkillTypes*SkillTypes;
ASkillType::ASkillType()
 :cost(100),flags(0)
{
}
AnsiString ASkillType::Print(bool quotation)
{
 AnsiString s;
 if(quotation)s+="\"";
 s+=name;
 if(quotation)s+="\"";
 s+=",";
 if(quotation)s+="\"";
 s+=abr;
 if(quotation)s+="\"";
 s+=",";
 s+=cost;
 if(flags&MAGIC)s+=",MAGIC";
 if(flags&COMBAT)s+=",COMBAT";
 if(flags&CAST)s+=",CAST";
 if(flags&FOUNDATION)s+=",FOUNDATION";
 for(int i=0;i<3;i++){
  if(depends[i].Length()==0)break;
  s+=",";
  if(quotation)s+="\"";
  s+=depends[i];
  if(quotation)s+="\"";
  s+=",";
  s+=levels[i];
 }
 return s;
}
void ASkillType::CreateNew(ASkillType* prevskill)
{
 name=prevskill->name;
 abr=prevskill->abr;
 cost=prevskill->cost;
 flags=prevskill->flags;
 for(int i=0;i<3;i++){
  depends[i]=prevskill->depends[i];
  levels[i]=prevskill->levels[i];
 }
}
AnsiString ASkillType::FullName()
{
 return name+" ["+abr+"]";
}

ASkillTypes::ASkillTypes()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(SkillsTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 ASkillType*stype;
 while(*s=='\"'){
  stype=new ASkillType;
  stype->name=GetNextToken(s);
  if(*s!=','||s[1]!='\"'){
   throw Exception(AnsiString("Bad format by \"")+stype->name+"\": abridgement not found");
  }
  s++;
  stype->abr=GetNextToken(s);
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+stype->name+"\": cost not found");
  s++;
  str=GetNextToken(s);
  stype->cost=str.ToInt();
  while(*s==','&&s[1]!='\"'){
   s++;
   str=GetNextToken(s);
   if(str=="MAGIC")  stype->flags|=MAGIC;
   else if(str=="COMBAT")  stype->flags|=COMBAT;
   else if(str=="CAST")  stype->flags|=CAST;
   else if(str=="FOUNDATION")  stype->flags|=FOUNDATION;
   else throw Exception(AnsiString("Bad format by \"")+stype->name+"\": unknown flag: "+str);
  }
  int i=0;
  while(*s==','&&s[1]=='\"'){
   if(i>2)
    throw Exception(AnsiString("Bad format by \"")+stype->name+"\": to many depends");
   s++;
   str=GetNextToken(s);
   if(!Get(str)){
//    throw Exception(AnsiString("Bad format by \"")+stype->name+"\": unknown depends: "+str);
    AddEvent("Warning for \""+stype->name+"\": may be unknown depends: "+str);
   }
   stype->depends[i]=str;
   if(*s!=',')throw Exception(AnsiString("Bad format by \"")+stype->name+"\": depends level not found");
   s++;
   str=GetNextToken(s);
   stype->levels[i]=str.ToInt();
   i++;
  }
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+stype->name+"\": bad end");
  s++;
  AddSkillType(stype);
 }
 delete mem;
 modified=false;
}
__fastcall ASkillTypes::~ASkillTypes()
{
 for(int i=Count-1;i>=0;i--){
  ASkillType*dt=Get(i);
//  Delete(i);
  delete dt;
 }
 TList::Clear();
 if(SkillTypes==this)SkillTypes=0;
}
ASkillType* ASkillTypes::Get(int index)
{
 return (ASkillType*)Items[index];
}
ASkillType* ASkillTypes::Get(AnsiString name,int mode)
{
 if(mode&1)for(int i=Count-1;i>=0;i--){
  ASkillType *st=Get(i);
  if(!CompareIStr(st->name,name))return st;
 }
 if(mode&2)for(int i=Count-1;i>=0;i--){
  ASkillType *st=Get(i);
  if(!CompareIStr(st->abr,name))return st;
 }
 return 0;
}
void ASkillTypes::AddSkillType(ASkillType* stype)
{
 if(Get(stype->name))throw Exception(AnsiString("Skill \"")+stype->name+"\" already present");
 if(Get(stype->abr))throw Exception(AnsiString("Skill \"")+stype->abr+"\" already present");
 Add(stype);
 modified=true;
}
void ASkillTypes::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(SkillsTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print(true)+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
 modified=false;
}
//0 - not found, 1 - present, 2 - name, not abr, 3 - abr, not name
int ASkillTypes::VerSkills(AnsiString name, AnsiString abr)
{
 ASkillType*st;
 st=Get(name,1);
 if(st){
  if(st->abr==abr)return 1;
  return 2;
 }
 st=Get(abr,2);
 if(st)return 3;
 return 0;
}
void ASkillTypes::MakeSkillList(TStringList *list)
{
 if(!list)return;
 foreach(this){
  ASkillType *stype=*(ASkillType**)iter;
  list->Add(stype->name);
 }
}
AnsiString ASkillTypes::AbrToName(AnsiString abr)
{
 ASkillType*stype=Get(abr,2);
 if(stype)return stype->name;
 return "";
}
AnsiString ASkillTypes::NameToAbr(AnsiString name)
{
 ASkillType*stype=Get(name,1);
 if(stype)return stype->abr;
 return "";
}
const char*SkillRepTxt="skillrep.txt";
ASkillInfos*SkillInfos;
ASkillInfo::ASkillInfo()
 :level(0)
{
}
AnsiString ASkillInfo::Print(bool quotation)
{
 AnsiString s;
 if(quotation)s+="\"";
 s+=skill;
 if(quotation)s+="\"";
 s+=",";
 s+=level;
 s+=",";
 if(quotation)s+="\"";
 s+=text;
 if(quotation)s+="\"";
 return s;
}
AnsiString ASkillInfo::FullName()
{
 return skill+" ("+level+")";
}
void ASkillInfo::CreateNew(ASkillInfo * prevsi)
{
 skill=prevsi->skill;
 level=prevsi->level;
 text=prevsi->text;
}

ASkillInfos::ASkillInfos()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(SkillRepTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 ASkillInfo*sInfo;
 while(*s=='\"'){
  sInfo=new ASkillInfo;
  sInfo->skill=GetNextToken(s);
  if(!SkillTypes->Get(sInfo->skill))throw Exception(AnsiString("Unknown skill \"")+sInfo->skill+"\"");
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+sInfo->skill+"\": level not found");
  s++;
  str=GetNextToken(s);
  sInfo->level=str.ToInt();
  if(*s!=','||s[1]!='\"'){
   throw Exception(AnsiString("Bad format by \"")+sInfo->skill+"\": text not found");
  }
  s++;
  sInfo->text=GetNextToken(s);
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+sInfo->skill+"\": bad end");
  s++;
  AddSkillInfo(sInfo);
 }
 delete mem;
 modified=false;
}
__fastcall ASkillInfos::~ASkillInfos()
{
 for(int i=Count-1;i>=0;i--){
  ASkillInfo*dt=Get(i);
//  Delete(i);
  delete dt;
 }
 TList::Clear();
 if(SkillInfos==this)SkillInfos=0;
}
ASkillInfo* ASkillInfos::Get(int index)
{
 return (ASkillInfo*)Items[index];
}
ASkillInfo* ASkillInfos::Get(AnsiString skill, int level)
{
 for(int i=Count-1;i>=0;i--){
  ASkillInfo*st=Get(i);
  if(!CompareIStr(st->skill,skill)&&st->level==level)return st;
 }
 return 0;
}
void ASkillInfos::AddSkillInfo(ASkillInfo* sInfo)
{
 if(sInfo->level<1||sInfo->level>5)throw Exception(AnsiString("Skill Info ")+sInfo->skill+" ("+sInfo->level+"): bad level");
 if(Get(sInfo->skill,sInfo->level))throw Exception(AnsiString("Skill Info for ")+sInfo->skill+" ("+sInfo->level+") already present");
 Add(sInfo);
 modified=true;
}
void ASkillInfos::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(SkillRepTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print(true)+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
 modified=false;
}
const char*ItemsTxt="items.txt";
AItemTypes*ItemTypes;
AItemType::AItemType()
 :flags(0),weight(0),type(0),walk(0),ride(0),fly(0),swim(0)
{
}
AnsiString AItemType::Print(bool quotation)
{
 AnsiString s;
 if(quotation)s+="\"";
 s+=name;
 if(quotation)s+="\"";
 s+=",";
 if(quotation)s+="\"";
 s+=names;
 if(quotation)s+="\"";
 s+=",";
 if(quotation)s+="\"";
 s+=abr;
 if(quotation)s+="\"";
 if(flags&CANTGIVE)s+=",CANTGIVE";
 if(skill.Length()){
  s+=",";
  if(quotation)s+="\"";
  s+=skill;
  if(quotation)s+="\"";
  s+=",";
  s+=level;
  if(input.Length()){
   s+=",";
   if(quotation)s+="\"";
   s+=input;
   if(quotation)s+="\"";
   s+=",";
   s+=number;
  }
 }
 s+=",";
 s+=weight;
 if(type&IT_NORMAL)s+=",NORMAL";
 if(type&IT_ADVANCED)s+=",ADVANCED";
 if(type&IT_TRADE)s+=",TRADE";
 if(type&IT_MAN)s+=",MAN";
 if(type&IT_MONSTER)s+=",MONSTER";
 if(type&IT_MAGIC)s+=",MAGIC";
 if(type&IT_WEAPON)s+=",WEAPON";
 if(type&IT_ARMOR)s+=",ARMOR";
 if(type&IT_MOUNT)s+=",MOUNT";
 if(type&IT_BATTLE)s+=",BATTLE";
 if(type&IT_FOOD)s+=",FOOD";
 s+=",";
 s+=walk;
 s+=",";
 s+=ride;
 s+=",";
 s+=fly;
 s+=",";
 s+=swim;
 if(helpproditem.Length()){
  s+=",HELPPROD:";
  if(quotation)s+="\"";
  s+=helpproditem;
  if(quotation)s+="\"";
 }
 return s;
}
void AItemType::CreateNew(AItemType * previt)
{
 name=previt->name;
 names=previt->names;
 abr=previt->abr;
 flags=previt->flags;
 skill=previt->skill;
 level=previt->level;
 input=previt->input;
 number=previt->number;
 weight=previt->weight;
 type=previt->type;
 walk=previt->walk;
 ride=previt->ride;
 fly=previt->fly;
 swim=previt->swim;
}
bool AItemType::IsNewInfo(AItemType *it)
{
 if(abr!=it->abr)return true;
 if(it->name.Length()&&name!=it->name)return true;
 if(it->names.Length()&&names!=it->names)return true;
 if(it->weight==-1)return false;
 if(weight!=it->weight)return true;
 if((type&it->type)!=it->type)return true;
 if(walk!=it->walk)return true;
 if(ride!=it->ride)return true;
 if(fly!=it->fly)return true;
 return false;
}
void AItemType::UpdateNewInfo(AItemType * newit)
{
 name=newit->name;
 abr=newit->abr;
 weight=newit->weight;
 type|=newit->type;
 walk=newit->walk;
 ride=newit->ride;
 fly=newit->fly;
}
AnsiString AItemType::FullName(bool plural)
{
 AnsiString s;
 if(plural)s=names;else s=name;
 s+=" ["+abr+"]";
 return s;
}
AItemTypes::AItemTypes()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(ItemsTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 AItemType*itype;
 while(*s=='\"'){
  itype=new AItemType;
  itype->name=GetNextToken(s);
  if(*s!=','||s[1]!='\"')
   throw Exception(AnsiString("Bad format by \"")+itype->name+"\": plural not found");
  s++;
  itype->names=GetNextToken(s);
  if(*s!=','||s[1]!='\"')
   throw Exception(AnsiString("Bad format by \"")+itype->name+"\": abridgement not found");
  s++;
  itype->abr=GetNextToken(s);
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": comma not found");
  if(s[1]!='\"'&&!isdigit(s[1])&&s[1]!='-'){
   s++;
   str=GetNextToken(s);
   if(str=="CANTGIVE")itype->flags|=CANTGIVE;
   if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": comma not found");
  }
  if(s[1]=='\"'){
   s++;
   str=GetNextToken(s);
   if(!SkillTypes->Get(str))throw Exception(AnsiString("Bad format by \"")+itype->name+"\": unknown skill: "+str);
   itype->skill=str;
   if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": level not found");
   s++;
   str=GetNextToken(s);
   itype->level=str.ToInt();
   if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": comma not found");
   if(s[1]=='\"'){
    s++;
    str=GetNextToken(s);
    if(!Get(str)){
     AddEvent("Warning for \""+itype->name+"\": may be unknown input item: "+str);
//     throw Exception(AnsiString("Bad format by \"")+itype->name+"\": unknown input item: "+str);
    }
    itype->input=str;
    if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": input number not found");
    s++;
    str=GetNextToken(s);
    itype->number=str.ToInt();
    if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": comma not found");
   }
  }
  s++;
  str=GetNextToken(s);
  itype->weight=str.ToInt();
  while(*s==','&&!isdigit(s[1])){
   s++;
   str=GetNextToken(s);
   if(str=="NORMAL")  itype->type|=IT_NORMAL;
   else if(str=="ADVANCED")  itype->type|=IT_ADVANCED;
   else if(str=="TRADE")  itype->type|=IT_TRADE;
   else if(str=="MAN")  itype->type|=IT_MAN;
   else if(str=="MONSTER")  itype->type|=IT_MONSTER;
   else if(str=="MAGIC")  itype->type|=IT_MAGIC;
   else if(str=="WEAPON")  itype->type|=IT_WEAPON;
   else if(str=="ARMOR")  itype->type|=IT_ARMOR;
   else if(str=="MOUNT")  itype->type|=IT_MOUNT;
   else if(str=="BATTLE")  itype->type|=IT_BATTLE;
   else if(str=="FOOD")  itype->type|=IT_FOOD;
   else throw Exception(AnsiString("Bad format by \"")+itype->name+"\": unknown type: "+str);
  }
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": walk not found");
  s++;
  str=GetNextToken(s);
  itype->walk=str.ToInt();
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": ride not found");
  s++;
  str=GetNextToken(s);
  itype->ride=str.ToInt();
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": fly not found");
  s++;
  str=GetNextToken(s);
  itype->fly=str.ToInt();
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": swim not found");
  s++;
  str=GetNextToken(s);
  itype->swim=str.ToInt();
  while(*s==','){
   s++;
   str=GetNextToken(s);
   if(str=="HELPPROD"){
    if(*s!=':')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": ':' not found in HELPPROD");
    s++;
    if(*s!='\"')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": item not found in HELPPROD");
    itype->helpproditem=GetNextToken(s);
   }
  }
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+itype->name+"\": bad end");
  s++;
  AddItemType(itype);
 }
 delete mem;
 modified=false;
}
__fastcall AItemTypes::~AItemTypes()
{
 for(int i=Count-1;i>=0;i--){
  AItemType*it=Get(i);
//  Delete(i);
  delete it;
 }
 TList::Clear();
 if(ItemTypes==this)ItemTypes=0;
}
AItemType* AItemTypes::Get(int index)
{
 return (AItemType*)Items[index];
}


AItemType* AItemTypes::Get(AnsiString name,int mode)
{
 if(name.Length()==0)return 0;
 if(mode&1)for(int i=Count-1;i>=0;i--){
  AItemType*st=Get(i);
  if(!CompareIStr(st->name,name))return st;
 }
 if(mode&2)for(int i=Count-1;i>=0;i--){
  AItemType*st=Get(i);
  if(!CompareIStr(st->names,name))return st;
 }
 if(mode&4)for(int i=Count-1;i>=0;i--){
  AItemType*st=Get(i);
  if(!CompareIStr(st->abr,name))return st;
 }
 return 0;
}
void AItemTypes::AddItemType(AItemType* itype)
{
 bool curill=IsIllusionAbr(itype->abr);
 AItemType *it;
 it=Get(itype->name);
 if(it&&IsIllusionAbr(it->abr)==curill)throw Exception(AnsiString("Item \"")+itype->name+"\" already present");
 it=Get(itype->names);
 if(it&&IsIllusionAbr(it->abr)==curill)throw Exception(AnsiString("Item \"")+itype->names+"\" already present");
 it=Get(itype->abr);
 if(it&&IsIllusionAbr(it->abr)==curill)throw Exception(AnsiString("Item \"")+itype->abr+"\" already present");
 Add(itype);
 modified=true;
}
void AItemTypes::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(ItemsTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print(true)+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
 modified=false;
}
void AItemTypes::MakeItemList(TStringList* list)
{
 if(!list)return;
 foreach(this){
  AItemType *itype=*(AItemType**)iter;
  list->Add(itype->name);
 }
}
AnsiString AItemTypes::AbrToName(AnsiString abr)
{
 foreach(this){
  AItemType *itype=*(AItemType**)iter;
  if(itype->abr==abr)return itype->name;
 }
 return "";
}
AnsiString AItemTypes::AbrToNames(AnsiString abr)
{
 foreach(this){
  AItemType *itype=*(AItemType**)iter;
  if(itype->abr==abr){
   if(itype->names.Length())
    return itype->names;
   else
    return itype->name;
  }
 }
 return "";
}
AnsiString AItemTypes::NameToAbr(AnsiString name)
{
 foreach(this){
  AItemType *itype=*(AItemType**)iter;
  if(itype->name==name||itype->names==name)return itype->abr;
 }
 return "";
}
const unsigned char charRusA='À', charEngA='A';
bool AItemTypes::IsIllusionAbr(AnsiString abr)
{
 if(abr.Length()<1) return false;
 unsigned char c=abr[1];
 if(c>=charRusA) return true;
 return false;
}
AnsiString AItemTypes::IllusionAbrToNormal(AnsiString iabr)
{
 AnsiString abr=iabr;
 if(abr.Length()<1) return abr;
 unsigned char c=abr[1];
 if(c<charRusA) return abr;
 c=(unsigned char)(c-charRusA+charEngA);
 abr[1]=c;
 return abr;
}
AnsiString AItemTypes::NormalAbrToIllusion(AnsiString abr)
{
 if(abr.Length()<1) return abr;
 unsigned char c=abr[1];
 if(c>=charRusA) return abr;
 c=(unsigned char)(c-charEngA+charRusA);
 abr[1]=c;
 return abr;
}

const char*TownTypeTxt="towntype.txt";
ATownTypes*TownTypes;
ATownType::ATownType()
 :imgnum(-1)
{
}
AnsiString ATownType::Print(){
 AnsiString s="\"";
 s+=name+"\"";
 if(imgnum>=0){
  s+=",\""+imgname+"\"";
 }
 return s;
};
ATownTypes::ATownTypes()
{
 MainForm->MapImages->Clear();
 TMemoryStream *mem=LoadFile(GetGameFileName(TownTypeTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 ATownType *ttype;
 while(*s=='\"'){
  ttype=new ATownType;
  ttype->name=GetNextToken(s);
  if(*s==','){
   s++;
   ttype->imgname=GetNextToken(s);
   ttype->imgnum=MainForm->AddInfoImage(ttype->imgname+".bmp");
  }
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+ttype->name+"\": bad end");
  s++;
  AddTownType(ttype);
 }
 delete mem;
 modified=false;
}
__fastcall ATownTypes::~ATownTypes()
{
 for(int i=Count-1;i>=0;i--){
  ATownType*it=Get(i);
//  Delete(i);
  delete it;
 }
 TList::Clear();
 if(TownTypes==this)TownTypes=0;
}
ATownType* ATownTypes::Get(int index){
 return (ATownType*)Items[index];
}
ATownType* ATownTypes::Get(AnsiString type)
{
 for(int i=0;i<count;i++){
  if(Get(i)->name==type)return Get(i);
 }
 return 0;
}
int ATownTypes::GetIndex(AnsiString type)
{
 for(int i=0;i<count;i++){
  if(Get(i)->name==type)return i;
 }
 return -1;
}
void ATownTypes::AddTownType(ATownType *type)
{
 if(GetIndex(type->name)!=-1)throw Exception(AnsiString("Town type \"")+type->name+"\" already present");
 Add(type);
 modified=true;
}
void ATownTypes::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(TownTypeTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print()+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
}
const char*KeyPhrasesTxt="keyphrases.txt";
AKeyPhrases*KeyPhrases;
AKeyPhrases::AKeyPhrases()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(KeyPhrasesTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 while(*s=='\"'){
  str=GetNextToken(s);
  Add(str);
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+str+"\": bad end");
  s++;
 }
 if(NKEYPHRASES!=Count)throw Exception(AnsiString("Count mismatch"));
 delete mem;
}
__fastcall AKeyPhrases::~AKeyPhrases()
{
 if(KeyPhrases==this)KeyPhrases=0;
}
AnsiString __fastcall AKeyPhrases::Get(int index)
{
 if(index<0||index>=Count)return "";
 return TStringList::Get(index);
}
void AKeyPhrases::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(KeyPhrasesTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s="\""+Get(i)+"\"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
}


AnsiString curGame;
TStringList*Games;
extern AnsiString ClientPath;
AnsiString GetGameFileName(AnsiString fName){
 AnsiString str=ClientPath;
 if(curGame.Length())str+=curGame+"\\";
 str+=fName;
 return str;
}
AnsiString GetOrderName(int facnum){
 char buf[80];
 sprintf(buf,"%sord%05d.%03d",OrdersDir.c_str(),curTurn->num,facnum);
 return GetGameFileName(buf);
}
AnsiString GetReportName(int facnum){
 char buf[80];
 sprintf(buf,"%srep%05d.%03d",ReportsDir.c_str(),curTurn->num,facnum);
 return GetGameFileName(buf);
}
void LoadGameBase(){
//#define LOAD_TIME
#ifdef LOAD_TIME
 long t0=GetTickCount(),t1;
 AnsiString tt="Loading ";
#endif
 if(curGame.Length()){
  AddEvent("Loading game \""+curGame+"\" ...");
  ForceDirectories(GetGameFileName(ReportsDir));
  ForceDirectories(GetGameFileName(OrdersDir));
  ForceDirectories(GetGameFileName(StoreDir));

 }else
  AddEvent("Loading default settings ...");
/* for(int i=0;i<TerrainTypes->count;i++){
  ATerrainType*tt=TerrainTypes->Get(i);
  AddEvent(" "+tt->Print());
 }
 ATerrainType *tt1=new ATerrainType;
 tt1->name="dsfdsgf";
 TerrainTypes->AddTerrainType(tt1);*/
 try{
  delete WeatherTypes;WeatherTypes=0;
  WeatherTypes=new AWeatherTypes;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init weather types",MB_OK);
  _exit(0);
 }
 AddEvent("Weather types initialized");
 try{
  delete WeatherRegions;WeatherRegions=0;
  WeatherRegions=new AWeatherRegions;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init weather regions",MB_OK);
  _exit(0);
 }
 AddEvent("Weather regions initialized");
/* foreach(WeatherTypes,AWeatherType*tt)
  AddEvent(" "+tt->Print());
 }
 AWeatherType *tt1=new AWeatherType;
 tt1->name="dsfdsgf";
 WeatherTypes->AddWeatherType(tt1);*/
 try{
  delete DirTypes;DirTypes=0;
  DirTypes=new ADirTypes;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init direction types",MB_OK);
  _exit(0);
 }
 AddEvent("Direction types initialized");
/* foreach(DirTypes,ADirType*dt)
  AddEvent(" "+dt->abrname+"="+dt->fullname);
 }*/
 try{
  delete SkillTypes;
  SkillTypes=new ASkillTypes;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init skill types",MB_OK);
  _exit(0);
 }
 AddEvent("Skill types initialized");
/* ASkillType *tt1=new ASkillType;
 tt1->name="farsight";
 tt1->abrname="FARS";
 tt1->depends[0]="PATT";tt1->levels[0]=1;
 tt1->depends[1]="SPIR";tt1->levels[1]=1;
 SkillTypes->AddSkillType(tt1);
 foreach(SkillTypes,ASkillType*st)
  AddEvent(" "+st->Print());
 }*/
 try{
  delete SkillInfos;
  SkillInfos=new ASkillInfos;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init skill infos",MB_OK);
  _exit(0);
 }
 AddEvent("Skill infos initialized");
 try{
  delete ItemTypes;ItemTypes=0;
  ItemTypes=new AItemTypes;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init item types",MB_OK);
  _exit(0);
 }
 AddEvent("Item types initialized");
/* AItemType*tt=new AItemType;
 tt->name="crossbow";
 tt->names="crossbows";
 tt->abr="XBOW";
 tt->flags=1;
 tt->skill="WEAP";
 tt->level=1;
 tt->input="WOOD";
 tt->number=1;
 tt->weight=1;
 tt->type=IT_NORMAL|IT_WEAPON;
 ItemTypes->AddItemType(tt);

 foreach(ItemTypes,AItemType*it)
  AddEvent(" "+it->Print(true));
 }*/
 try{
  delete TerrainTypes;TerrainTypes=0;
  TerrainTypes=new ATerrainTypes;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init terrain types",MB_OK);
  _exit(0);
 }
 AddEvent("Terrain types initialized");
 try{
  delete TownTypes;TownTypes=0;
  TownTypes=new ATownTypes;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init town types",MB_OK);
  _exit(0);
 }
 AddEvent("Town types initialized");
/* ATownType *tt=new ATownType;
 tt->name="gorod";
 TownTypes->AddTownType(tt);

 foreach(TownTypes,ATownType* it)
  AddEvent(" "+it->Print());
 }*/
 try{
  delete ObjectTypes;
  ObjectTypes=new AObjectTypes;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init object types",MB_OK);
  _exit(0);
 }
 AddEvent("Object types initialized");
 try{
  delete KeyPhrases;KeyPhrases=0;
  KeyPhrases=new AKeyPhrases;
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init key phrases",MB_OK);
  _exit(0);
 }
 AddEvent("Key phrases initialized");
 if(!curGame.Length())return;
 try{
  delete curTurns;curTurns=0;
  curTurns=new ATurns;
  curTurns->Read();
#ifdef LOAD_TIME
 t1=GetTickCount();
 t0=t1-t0;
 tt+=t0;
 tt+=" ";
#endif
  curTurns->Get(0)->VerifyNextTurn();
  curTurns->RestoreLastParam();
 }catch(Exception&e){
  Application->MessageBox(e.Message.c_str(),"Error init turns info",MB_OK);
  _exit(0);
 }
 AddEvent("Loading complete");
#ifdef LOAD_TIME
 t1=GetTickCount()-t1;
 tt+=t1;
 ShowMessage(tt);
#endif
// MainForm->ChangeLevel(0);
}
void SaveGameBase(){
 if(curGame.Length()){
  AddEvent("Saving game \""+curGame+"\" ...");
  ForceDirectories(GetGameFileName(ReportsDir));
  ForceDirectories(GetGameFileName(OrdersDir));
  ForceDirectories(GetGameFileName(StoreDir));
  curTurns->StoreLastParam();
  curTurns->Write();
 }else
  AddEvent("Saving default settings ...");

 try{
  TerrainTypes->Save();
  WeatherTypes->Save();
  WeatherRegions->Save();
  DirTypes->Save();
  SkillTypes->Save();
  SkillInfos->Save();
  ItemTypes->Save();
  TownTypes->Save();
  KeyPhrases->Save();
  ObjectTypes->Save();
 }catch(Exception&e){
   Application->MessageBox(e.Message.c_str(),"Error save types",MB_OK);
   _exit(0);
 }
 if(!curGame.Length())return;
 AddEvent("Saving complete");
}
void DeleteGameBase(){
 delete WeatherTypes;
 delete WeatherRegions;
 delete DirTypes;
 delete SkillTypes;
 delete SkillInfos;
 delete ItemTypes;
 delete TerrainTypes;
 delete TownTypes;
 delete ObjectTypes;
 delete KeyPhrases;
 delete curTurns;
}

void SetMainFormCaption(){
 AnsiString s="AtlaClient";
 if(curGame.Length()){
  s+=" Game \"";
  s+=curGame+"\"";
  if(curTurn){
   s+=" turn ";
   s+=curTurn->num;
   if(curTurn->num) s+=" ("+TurnNumberToString(curTurn->num)+") ";
  }
  if(curTurn->Packed){
   s+=" (PACKED)";
  }
  if(curFaction&&curFaction->number){
   s+=" for ";
   s+=curFaction->FullName();
  }
  if(curRegion){
   s+=" ";
   s+=curRegion->FullName();
   s+=" ";
   s+=curRegion->lastviewfact;
  }
/*  if(curUnit){
   s+=" ";
   s+=curUnit->FullName();
  }*/
 }
 MainForm->Caption=s;
}
AnsiString MakeDepSkillStr(AnsiString skill,int level){
 if(!SkillTypes)return "";
 ASkillType*stype=SkillTypes->Get(skill);
 if(!stype)return "";//{skill=stype->name;}
 return stype->name+","+level;
}
AnsiString MakeInputItemStr(AnsiString item,int number){
 if(!ItemTypes)return "";
 AItemType*itype=ItemTypes->Get(item);
 if(!itype)return "";//{skill=stype->name;}
 return itype->name+","+number;
}

char *MonthNames[] = {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};
int GetTurnNumberFromString(AnsiString str){
 char*s=str.c_str();
 AnsiString monthname=GetNextToken(s);
 int month;
 for(month=0;month<12;month++){
  if(monthname==MonthNames[month])break;
 }
 if(month==12)throw Exception(AnsiString("Error Process Turn Number: unknown month ")+monthname);
 while(*s&&!isdigit(*s))s++;
 if(!*s)throw Exception("Error Process Turn Number: year number not found");
 int year=atoi(s);
 return (year-1)*12+month+1;
}
AnsiString TurnNumberToString(int number){
 number--;
 int month=(number%12);
 int year=number/12+1;
 AnsiString str=MonthNames[month];
 str+=", Year ";
 str+=year;
 return str;
}

AnsiString WoodOrStoneStr="WOOD_OR_STONE";
const char*ObjectTypeTxt="objtype.txt";
AObjectTypes *ObjectTypes;
static AnsiString RoadStr=",ROAD:";
AObjectType::AObjectType()
 :canenter(0),capacity(0),cost(-1),level(-1),
  protect(0),sailors(0),road(0)
{
}
AnsiString AObjectType::Print()
{
 AnsiString s;
 s+="\""+name+"\",";
 s+=protect;
 s+=",";
 s+=capacity;
 s+=",";
 if(cost!=-1){
  s+="\""+item+"\",";
  s+=cost;
  s+=",";
  if(level!=-1){
   s+="\""+skill+"\",";
   s+=level;
   s+=",";
  }
 }
 s+=sailors;
 s+=",";
 s+=canenter;
 if(production.Length()){
  s+=",";
  s+="\""+production+"\"";
 }
 if(road){
  s+=RoadStr;
  bool first=true;
  for(int i=0;i<NDIRS;i++){
   if(!(road&(1<<i))) continue;
   ADirType *dt=DirTypes->Get(i);
   if(first) first=false;
   else s+=",";
   s+=dt->abr;
  }
 }
 return s;
}


AObjectTypes::AObjectTypes()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(ObjectTypeTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 AObjectType *otype;
 while(*s=='\"'){
  otype=new AObjectType;
  otype->name=GetNextToken(s);
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": protect not found");
  s++;
  str=GetNextToken(s);
  otype->protect=atoi(str.c_str());
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": capacity not found");
  s++;
  str=GetNextToken(s);
  otype->capacity=atoi(str.c_str());
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": sailors not found");
  if(s[1]=='\"'){
   s++;
   str=GetNextToken(s);
   if(str==WoodOrStoneStr);
   else if(!ItemTypes->Get(str,4))throw Exception(AnsiString("Bad format by \"")+otype->name+"\": item \""+str+"\" not found");
   otype->item=str;
   if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": cost not found");
   s++;
   str=GetNextToken(s);
   otype->cost=atoi(str.c_str());
   if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": sailors not found");
   if(s[1]=='\"'){
    s++;
    str=GetNextToken(s);
    if(!SkillTypes->Get(str,2))throw Exception(AnsiString("Bad format by \"")+otype->name+"\": skill \""+str+"\" not found");
    otype->skill=str;
    if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": level not found");
    s++;
    str=GetNextToken(s);
    otype->level=atoi(str.c_str());
   }
  }
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": sailors not found");
  s++;
  str=GetNextToken(s);
  otype->sailors=atoi(str.c_str());
  if(*s!=',')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": canenter not found");
  s++;
  str=GetNextToken(s);
  otype->canenter=atoi(str.c_str());
  if(*s==','&&s[1]=='\"'){
   s+=1;
   str=GetNextToken(s);
   if(!ItemTypes->Get(str,4))throw Exception(AnsiString("Bad format by \"")+otype->name+"\": item \""+str+"\" not found");
   otype->production=str;
  }
  if(!MyCompare(s,RoadStr)){
   s+=RoadStr.Length();
   char *olds=s;
   while(*s!='\n'){
    str=GetNextToken(s);
    int dir=DirTypes->Find(str);
    if(dir==-1) break;
    olds=s;
    otype->road|=1<<dir;
    if(*s==',') s++;
   }
   s=olds;
  }
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+otype->name+"\": bad end");
  s++;
  AddObjectType(otype);
 }
 delete mem;
 modified=false;
}
__fastcall AObjectTypes::~AObjectTypes()
{
 for(int i=Count-1;i>=0;i--){
  AObjectType*it=Get(i);
//  Delete(i);
  delete it;
 }
 TList::Clear();
 if(ObjectTypes==this)ObjectTypes=0;
}


AObjectType* AObjectTypes::Get(int index)
{
 return (AObjectType*)Items[index];
}
AObjectType* AObjectTypes::Get(AnsiString name)
{
 foreachr(this){
  AObjectType*ot=*(AObjectType**)iter;
  if(!CompareIStr(ot->name,name))return ot;
 }
 return 0;
}
void AObjectTypes::AddObjectType(AObjectType*otype)
{
 if(Get(otype->name))throw Exception(AnsiString("ObjectType \"")+otype->name+"\" already present");
 Add(otype);
 modified=true;
}
void AObjectTypes::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(ObjectTypeTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print()+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
 modified=false;
}
void ReadStringAsInt(TFile &in,AnsiString &str){
 int i;
 in>>i;
 char buf[5]={0,0,0,0,0};
 *(int*)buf=i;
 str=buf;
}
void WriteStringAsInt(TFile &out,AnsiString str){
 if(str.Length()>4)throw Exception("String \""+str+"\" to long");
 int i=*(int*)str.c_str();
 out<<i;
}
void ReadIntAsShort(TFile &in, int &i){
 short sh;
 in>>sh;
 i=sh;
}
void WriteIntAsShort(TFile &out, int i){
 if(i>MAXSHORT)throw Exception(AnsiString("Int number ")+i+" to big (short)");
 short sh=(short)i;
 out<<sh;
}
void ReadIntAsByte(TFile &in, int &i){
 unsigned char ch;
 in>>ch;
 i=ch;
}
void WriteIntAsByte(TFile &out, int i){
 if(i>255)throw Exception(AnsiString("Int number ")+i+" to big (byte)");
 unsigned char ch=(unsigned char)i;
 out<<ch;
}

const char*WeatherRegionTxt="weatherregion.txt";
AWeatherRegions *WeatherRegions;
AWeatherRegion::AWeatherRegion()
 :rect(-1,-1,-1,-1)
{
 for(int i=0;i<12;i++) weathers[i]="Normal";
}
AnsiString AWeatherRegion::Print()
{
 AnsiString s;
 s+="\""+name+"\"";
 s+=",";
 s+=level;
 s+=",";
 s+=rect.left;
 s+=",";
 s+=rect.top;
 s+=",";
 s+=rect.right;
 s+=",";
 s+=rect.bottom;
 foreach(WeatherTypes){
  AWeatherType *wt=*(AWeatherType**)iter;
  if(wt->name=="Normal") continue;
  bool first=true;
  for(int i=0;i<12;i++){
   if(weathers[i]==wt->name){
    if(first){
     first=false;
     s+=",\"";
     s+=wt->name;
     s+="\"";
    }
    s+=",";
    s+=i;
   }
  }
 }
 return s;
}
AWeatherRegions::AWeatherRegions()
{
 TMemoryStream *mem=LoadFile(GetGameFileName(WeatherRegionTxt));
 char*memory=(char*)mem->Memory,*s=memory;
 AnsiString str;
 AWeatherRegion *wreg;
 while(*s=='\"'){
  wreg=new AWeatherRegion;
  wreg->name=GetNextToken(s);
  if(*s!=','){
   throw Exception(AnsiString("Bad format by Weather Region \"")+wreg->name+"\" level not found");
  }
  s++;
  str=GetNextToken(s);
  wreg->level=str.ToInt();
  if(*s!=','){
   throw Exception(AnsiString("Bad format by Weather Region \"")+wreg->name+"\" top not found");
  }
  s++;
  str=GetNextToken(s);
  wreg->rect.left=str.ToInt();
  if(*s!=','){
   throw Exception(AnsiString("Bad format by Weather Region \"")+wreg->name+"\" top not found");
  }
  s++;
  str=GetNextToken(s);
  wreg->rect.top=str.ToInt();
  if(*s!=','){
   throw Exception(AnsiString("Bad format by Weather Region \"")+wreg->name+"\" right not found");
  }
  s++;
  str=GetNextToken(s);
  wreg->rect.right=str.ToInt();
  if(*s!=','){
   throw Exception(AnsiString("Bad format by Weather Region \"")+wreg->name+"\" bottom not found");
  }
  s++;
  str=GetNextToken(s);
  wreg->rect.bottom=str.ToInt();
  
  while(*s==','&&s[1]=='\"'){
   s++;
   AnsiString wname=GetNextToken(s);
   {
    AWeatherType *wtype=WeatherTypes->Get(wname);
    if(!wtype){
     throw Exception(AnsiString("Bad format by Weather Region \"")+wreg->name+"\" unknown weather \""+wname+"\"");
    }
    wname=wtype->name;
   }
   while(*s==','&&s[1]!='\"'){
    s++;
    str=GetNextToken(s);
    int month=str.ToInt();
    if(month<0||month>11){
     throw Exception(AnsiString("Bad format by Weather Region \"")+wreg->name+"\" bad month number "+month);
    }
    wreg->weathers[month]=wname;
   }
  }
  if(*s!='\n')throw Exception(AnsiString("Bad format by \"")+wreg->name+"\": bad end");
  s++;
  AddWeatherRegion(wreg);
 }
 delete mem;
}
__fastcall AWeatherRegions::~AWeatherRegions()
{
 for(int i=Count-1;i>=0;i--){
  AWeatherRegion *wr=(AWeatherRegion*)Items[i];
//  Delete(i);
  delete wr;
 }
 TList::Clear();
 if(WeatherRegions==this)WeatherRegions=0;
}
AWeatherRegion *AWeatherRegions::Get(int index)
{
 return (AWeatherRegion*)Items[index];
}
AWeatherRegion * AWeatherRegions::Get(AnsiString name)
{
 foreach(this){
  AWeatherRegion *wreg=*(AWeatherRegion**)iter;
  if(wreg->name==name)return wreg;
 }
 return 0;
}
void AWeatherRegions::AddWeatherRegion(AWeatherRegion * wreg)
{
 for(int i=Count-1;i>=0;i--){
  if(wreg->name==Get(i)->name) throw Exception(AnsiString("Weather Region \"")+wreg->name+"\" already present");
 }
 Add(wreg);
}


void AWeatherRegions::Save()
{
 TFileStream *file=new TFileStream(GetGameFileName(WeatherRegionTxt),fmCreate);//fmOpenWrite);
 for(int i=0;i<Count;i++){
  AnsiString s=Get(i)->Print()+"\r\n";
  file->Write(s.c_str(),s.Length());
 }
 delete file;
}
AnsiString AWeatherRegions::GetWeather(int x, int y, int z,int turnnum)
{
 if(turnnum==-1) turnnum=curTurnNumber;
 turnnum--;
 int monthnum=turnnum%12;
 if(monthnum<0) monthnum=11;
 foreach(this){
  AWeatherRegion *wreg=*(AWeatherRegion**)iter;
  if(wreg->level!=z) continue;
  if(wreg->rect.left>x||wreg->rect.right<x) continue;
  if(wreg->rect.top>y||wreg->rect.bottom<y) continue;
  return wreg->weathers[monthnum];
 }
 return "Normal";
}
int DeleteFiles(AnsiString fname){
 SHFILEOPSTRUCT fos;
 char buf[1000];

 memset(&fos,0,sizeof(fos));
 memset(buf,0,sizeof(buf));
// fos.hwnd=0;
 fos.wFunc=FO_DELETE;
 strcpy(buf,fname.c_str());
 fos.pFrom=buf;
 fos.fFlags=FOF_SILENT|FOF_NOCONFIRMATION;
 return SHFileOperation(&fos);
}
int MoveFiles(AnsiString oldname,AnsiString newname){
 SHFILEOPSTRUCT fos;
 char buf1[1000],buf2[1000];

 memset(&fos,0,sizeof(fos));
 memset(buf1,0,sizeof(buf1));
 memset(buf2,0,sizeof(buf2));
// fos.hwnd=0;
 fos.wFunc=FO_MOVE;
 strcpy(buf1,oldname.c_str());
 strcpy(buf2,newname.c_str());
 fos.pFrom=buf1;
 fos.pTo=buf2;
 fos.fFlags=FOF_SILENT|FOF_NOCONFIRMATION;
 return SHFileOperation(&fos);
}
int CopyFiles(AnsiString oldname,AnsiString newname){
 SHFILEOPSTRUCT fos;
 char buf1[1000],buf2[1000];

 memset(&fos,0,sizeof(fos));
 memset(buf1,0,sizeof(buf1));
 memset(buf2,0,sizeof(buf2));
// fos.hwnd=0;
 fos.wFunc=FO_COPY;
 strcpy(buf1,oldname.c_str());
 strcpy(buf2,newname.c_str());
 fos.pFrom=buf1;
 fos.pTo=buf2;
 fos.fFlags=FOF_SILENT|FOF_NOCONFIRMATION;
 return SHFileOperation(&fos);
}
void SetBits(int &value,int mask,bool set){
 if(set) value|=mask;
 else value&=~mask;
}

