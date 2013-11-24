//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "turn.h"
#include "util.h"
#include "eapfile.h"
#include "region.h"
#include "faction.h"
#include "unit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
int BaseVer,LastBaseVer=8;
int curTurnNumber=-1;
extern AnsiString wedgeexpr[6];

ATurn*curTurn;
ATurn::ATurn()
 :RegionList(new ARegionList(this)),factions(new AFactions(this)),
  num(-1),attr(0)
{
}
ATurn::~ATurn()
{
 delete RegionList;
 delete factions;
 if(curTurn==this){curTurn=0;curTurnNumber=-1;}
}
AnsiString ATurn::GetFileName(int mode){
//mode =0:agb, 1:agp
 AnsiString fName="turn";
 fName+=num;
 if(mode) fName+=".agp";
 else fName+=".agb";
 return fName;
}
void ATurn::Read()
{
 TFile in;
 AnsiString fName=GetFileName(Package);
 in.Open(GetGameFileName(fName));
 if(!in.Opened()){num=-1;return;}
 if(BaseVer>=4){
  in>>BaseVer;
 }
 if(BaseVer>LastBaseVer){
  AnsiString s="Error loading turn";
  s+=num;
  s+=". Unknown format";
  throw Exception(s);
 }
 int newnum=-1;
 in>>newnum;
 if(newnum!=num)
  throw Exception(AnsiString("Error loading turn ")+num);
 num=newnum;
 in>>attr;
 factions->Read(in);
 RegionList->Read(in);
 Package=false;
 AddEvent(AnsiString("Turn ")+num+" loaded");
}
void ATurn::Write()
{
 TFile out;
 AnsiString fName=GetFileName(Package);
 out.Open(GetGameFileName(fName),true);
 BaseVer=LastBaseVer;
 out<<BaseVer;
 out<<num;
 out<<attr;
 factions->Write(out);
 RegionList->Write(out);
}
void ATurn::CreateNew(ATurn *prevturn)
{
 if(prevturn){
  num=prevturn->num+1;
//  RegionList->turn=num;
  factions->CreateNew(prevturn->factions);
  RegionList->CreateNew(prevturn->RegionList);
 }else{
  num=0;
//  RegionList->turn=num;
  factions->CreateNew(0);
  RegionList->CreateNew(0);
 }
 AddEvent(AnsiString("Turn ")+num+" created");
}
void ATurn::VerifyNextTurn()
{
 if(curTurns->count<=num+1)return;
 ATurn *nxtturn;
 if(num==0){
  nxtturn=curTurns->Get(curTurns->curbeginturn);
 }else{
  nxtturn=curTurns->Get(num+1);
 }
 nxtturn->RegionList->UpdateFromPrev(RegionList);
 nxtturn->VerifyNextTurn();
}
void __fastcall ATurn::SetPacked(bool value)
{
 if(FPacked==value) return;
 if(value){
  if(num==-1){
   FPacked=value;
   return;
  }
  if(num!=0){
   Package=true;
   Write();
   Package=false;
   FPacked = value;
   Compact();
  }
//  Compact();
//  Write();
 }else{
  int oldnum=num;
  Package=true;
  Read();
  Package=false;
  if(num==-1){
   num=oldnum;
   Read();
  }else FPacked = value;
//  Write();
/*  ATurn *t=curTurns->Get(num-1);
  if(t)
   t->VerifyNextTurn();
  curTurn=0;
  if(!curTurns->SetCurTurn(num))return;*/
 }
}
void ATurn::Compact()
{
 foreach(RegionList){
  ARegion *reg=*(ARegion**)iter;
  reg->Compact();
 }
 factions->Compact();
}

ATurns*curTurns;
ATurns::ATurns()
 :TList()/*,lastturn(0),lastx(0),lasty(0)*/,lastz(0),lastfac(0),
  tertypes(new TStringList),provincies(new TStringList),
  towns(new TStringList),towntypes(new TStringList),
  curbeginturn(1),newbeginturn(1),autobeginturn(-5)
{
 for(int i=0;i<MaxLastLevels;i++){
  lastx[i]=0;lasty[i]=0;
 }
}
__fastcall ATurns::~ATurns()
{
 Clear();
 if(curTurns==this)curTurns=0;
 delete tertypes;
 delete provincies;
 delete towns;
 delete towntypes;
}
void __fastcall ATurns::Clear()
{
 for(int i=Count-1;i>=0;i--){
  ATurn*it=Get(i);
//  Delete(i);
  delete it;
 }
 TList::Clear();
}
void ATurns::Read()
{
 Clear();
 TFile in;
 in.Open(GetGameFileName("turns.agb"));
 in>>BaseVer;
 if(BaseVer>LastBaseVer){
  throw Exception("Error loading turns. Unknown format");
 }
 in>>lastfac;
 if(BaseVer<7){
  int _lastx,_lasty;
  in>>_lastx>>_lasty>>lastz;
  if(lastz<MaxLastLevels){
   lastx[lastz]=_lastx;
   lasty[lastz]=_lasty;
  }
 }
 in>>ver1>>ver2;
 if(BaseVer>=3){
  for(int i=0;i<6;i++){
   in>>wedgeexpr[i];
  }
 }

 int size;
 in>>size;

 if(BaseVer>=6){
  in>>curbeginturn;
  newbeginturn=curbeginturn;
  in>>autobeginturn;
 }

 if(BaseVer>=5){
  ReadStringList(in,tertypes);
  ReadStringList(in,provincies);
  ReadStringList(in,towns);
  ReadStringList(in,towntypes);
 }
 ATurn *prev=0;
 for(int i=0;i<size;i++){
  if(i!=0&&i<curbeginturn){
   TList::Add(0);
   continue;
  }
  ATurn *turn=new ATurn;
  turn->num=i;
//  turn->RegionList->turn=i;
  turn->Read();
  if(turn->num==-1){
   turn->CreateNew(prev);
   turn->num=i; 
  }
  Add(turn);
  prev=turn;
 }
 if(BaseVer>=7){
  in>>lastz;
  ATurn *turn=Get(0);
  int levels=turn->RegionList->RegionArrays->count;
  if(levels>MaxLastLevels) levels=MaxLastLevels;
  int _lastx,_lasty;
  for(int i=0;i<levels;i++){
   in>>_lastx>>_lasty;
   lastx[i]=_lastx;
   lasty[i]=_lasty;
  }
 }
}
void ATurns::Write()
{
 TFile out;
 out.Open(GetGameFileName("turns.agb"),true);
 BaseVer=LastBaseVer;
 out<<BaseVer;
 out<<lastfac;
// out<<lastx<<lasty<<lastz;
 out<<ver1<<ver2;
 for(int i=0;i<6;i++){
  out<<wedgeexpr[i];
 }
 out<<count;


 tertypes->Clear();
 provincies->Clear();
 towns->Clear();
 towntypes->Clear();
 if(autobeginturn>0){
  newbeginturn=count-autobeginturn;
 }
 if(newbeginturn>curbeginturn){
  int i;
  for(i=curbeginturn;i<newbeginturn;i++){
   ATurn *turn=Get(i);
   if(!turn) break;
   Get(0)->Update0(turn);
   turn->Packed=true;
//   turn->Write();
   AnsiString fName=turn->GetFileName(1);
   if(MoveFiles(GetGameFileName(fName),GetGameFileName(StoreDir+fName))) break;
   Items[i]=0;
   DeleteFiles(GetGameFileName(turn->GetFileName(0)));
   delete turn;
  }
  curbeginturn=i;
  newbeginturn=i;
 }
 if(newbeginturn<1) newbeginturn=1;
 if(newbeginturn<curbeginturn){
  int i;
  for(i=curbeginturn-1;i>=newbeginturn;i--){
   ATurn *turn=new ATurn;
   try{
    turn->Packed=true;
    turn->num=i;
    AnsiString fName=turn->GetFileName(1);
    if(MoveFiles(GetGameFileName(StoreDir+fName),GetGameFileName(fName)))
/*     throw 1*/;
    turn->Packed=false;
    if(turn->num!=i)
     throw 1;
    Items[i]=turn;
    DeleteFiles(GetGameFileName(turn->GetFileName(1)));
   }catch(...){
    delete turn;
    break;
   }
  }
  i++;
  curbeginturn=i;
  newbeginturn=i;
 }
 foreach(this){
  ATurn*turn=*(ATurn**)iter;
  if(!turn) continue;
//  if(!turn->Packed)
   turn->Write();
 }
 out<<curbeginturn;
 out<<autobeginturn;
 WriteStringList(out,tertypes);
 WriteStringList(out,provincies);
 WriteStringList(out,towns);
 WriteStringList(out,towntypes);

 {
  out<<lastz;
  ATurn *turn=Get(0);
  int levels=turn->RegionList->RegionArrays->count;
  if(levels>MaxLastLevels) levels=MaxLastLevels;
  int _lastx,_lasty;
  for(int i=0;i<levels;i++){
   _lastx=lastx[i];
   _lasty=lasty[i];
   out<<_lastx<<_lasty;
  }
 }
}
void ATurns::RepackStore(){
 for(int i=curbeginturn-1;i>=1;i--){
  ATurn *turn=new ATurn;
  try{
   try{
    turn->Packed=true;
    turn->num=i;
    AnsiString fName=turn->GetFileName(1);
    if(MoveFiles(GetGameFileName(StoreDir+fName),GetGameFileName(fName)))
 /*     throw 1*/;
    turn->Packed=false;
    if(turn->num!=i)
     throw 1;
    turn->Packed=true;
    if(MoveFiles(GetGameFileName(fName),GetGameFileName(StoreDir+fName)))
 /*     throw 1*/;
 //   DeleteFiles(GetGameFileName(turn->GetFileName(1)));
   }__finally{
    delete turn;
   }
  }catch(...){
   break;
  }
 }
 AddEvent("Store repacking complete");
}

void ATurns::Add(ATurn * turn)
{
 if(turn->num==-1) turn->num=count;
 if(turn->num!=count)
  throw Exception("Error ATurns::Add(): number not match");
 TList::Add(turn);
}
ATurn* ATurns::Get(int turnnum)
{
 return (ATurn*)Items[turnnum];
}
bool ATurns::SetCurTurn(int turnnum)
{
 if(curTurns!=this)return false;
 ATurn *t=Get(turnnum);
 if(t&&t==curTurn)return true;
 curTurn=0;
 curFactions=0;
 curRegionList=0;
 curFaction=0;
 curRegion=0;
 curRegionArray=0;
 if(!t)return false;
 AddEvent(AnsiString("Turn ")+turnnum+" activated");
 curTurn=t;
 curTurnNumber=turnnum;
 curFactions=t->factions;
 curRegionList=t->RegionList;
 if(!curFactions->SetCurFaction(lastfac)){
  if(!curFactions->SetCurFaction(0))return false;
 }
 int _lastx=0,_lasty=0;
 if(lastz<MaxLastLevels){
  _lastx=lastx[lastz];
  _lasty=lasty[lastz];
 }
 if(!curRegionList->SetCurRegion(_lastx,_lasty,lastz)){
  ARegion *reg=curRegionList->GetNearestRegion(_lastx,_lasty,lastz);
  if(!reg){
   if(!curRegionList->SetCurRegion(0,0,0))return false;
  }else{
   if(!curRegionList->SetCurRegion(reg->xloc,reg->yloc,reg->zloc))return false;
  }
 }
 curTurn->PrepareOrders();
 return true;
}
bool ATurns::RestoreLastParam()
{
 return SetCurTurn(count-1);
}
void ATurns::StoreLastParam()
{
 if(curFaction){
  lastfac=curFaction->number;
 }else lastfac=0;
 if(curRegion){

//  lastx=curRegion->xloc;
//  lasty=curRegion->yloc;
  lastz=curRegion->zloc;
  StoreLastXY();
 }else{
  /*lastx=lasty=*/lastz=0;
 }
}
void ATurns::RestoreLastXY(int &xloc,int &yloc,int zloc){
 if(zloc<MaxLastLevels){
  xloc=lastx[zloc];
  yloc=lasty[zloc];
 }else{
  xloc=0;
  yloc=0;
 }
}
void ATurns::StoreLastXY(){
 if(!curRegion) return;
 int level=curRegion->zloc;
 if(level>=MaxLastLevels) return;
 lastx[level]=curRegion->xloc;
 lasty[level]=curRegion->yloc;
}
void ATurns::CreateNew()
{
 /*lastturn=lastx=lasty=*/lastz=lastfac=0;
 ver1="";ver2="";
 Clear();
 ATurn*turn=new ATurn;
 turn->CreateNew(0);
 Add(turn);
}
void ATurns::PrepareTurn(int num)
{
 if(num<=0){
  SetCurTurn(0);
  return;
 }
///// todo
 if(count==1){
  curbeginturn=num;
  newbeginturn=num;
  for(int t=1;t<num;t++){
   TList::Add(0);
  }
  ATurn *prevturn=Get(0);
  ATurn *newturn=new ATurn;
  newturn->CreateNew(prevturn);
  newturn->num=num;
  Add(newturn);
 }else if(num<curbeginturn){
  newbeginturn=num;
  SaveGameBase();
  ATurn *prevturn=Get(0);
  prevturn->VerifyNextTurn();
  for(int t=num;t<curbeginturn;t++){
   ATurn *newturn=new ATurn;
   newturn->CreateNew(prevturn);
   newturn->num=t;
   Items[t]=newturn;
   prevturn=newturn;
  }
  curbeginturn=num;
  newbeginturn=num;
 }else{
  ATurn *prevturn=Get(count-1);
  for(int t=count-1;t<num;t++){
   ATurn *newturn=new ATurn;
   newturn->CreateNew(prevturn);
   Add(newturn);
   prevturn=newturn;
  }
 }
 SetCurTurn(num);
}
void ATurn::PrepareOrders()
{
 foreach(RegionList){
  ARegion *reg=*(ARegion**)iter;
  reg->PrepareOrders();
 }
}
void ATurn::RunOrders()
{
 foreach(RegionList){
  ARegion *reg=*(ARegion**)iter;
  reg->RunOrders();
 }
 AnsiString str;
 foreach(curFactions){
  AFaction *fac=*(AFaction**)iter;
  if(!fac->local) continue;
  if(fac->CountTaxRegions()>fac->warmax){
   str="Error: faction ";
   str+=fac->FullName();
   str+=" tax in many regions";
   AddEvent(str);
  }
 }
}
void ATurn::RunAutoOrders()
{
 foreach(RegionList){
  ARegion *reg=*(ARegion**)iter;
  reg->RunAutoOrders1();
 }
// RunOrders();
}
void ATurn::Update0(ATurn *newturn)
{
 if(num!=0) return;
 if(newturn->num==0) return;
 foreach(newturn->RegionList){
  ARegion *newreg=*(ARegion**) iter;
  ARegion *reg0=RegionList->Get(newreg->xloc,newreg->yloc,newreg->zloc);
  if(!reg0){
   reg0=new ARegion(this);
   reg0->xloc=newreg->xloc;
   reg0->yloc=newreg->yloc;
   reg0->zloc=newreg->zloc;
   RegionList->Add(reg0);
   ARegionArray *ra=RegionList->GetRegionArray(reg0->zloc);
   ra->VerifySize(reg0->xloc, reg0->yloc);
  }
  reg0->Update0(newreg);
 }
}

BYTE ATurns::Tertypes(AnsiString s)
{
 int ind=tertypes->IndexOf(s);
 if(ind==-1){
  ind=tertypes->Add(s);
 }
 if(ind>250)
  throw Exception("To many terrain types");
 return BYTE(ind);
}
WORD ATurns::Provincies(AnsiString s)
{
 int ind=provincies->IndexOf(s);
 if(ind==-1){
  ind=provincies->Add(s);
 }
 if(ind>65000)
  throw Exception("To many provincies");
 return (WORD)ind;
}
WORD ATurns::Towns(AnsiString s)
{
 int ind=towns->IndexOf(s);
 if(ind==-1){
  ind=towns->Add(s);
 }
 if(ind>65000)
  throw Exception("To many towns");
 return (WORD)ind;
}
BYTE ATurns::Towntypes(AnsiString s)
{
 int ind=towntypes->IndexOf(s);
 if(ind==-1){
  ind=towntypes->Add(s);
 }
 if(ind>250)
  throw Exception("To many city types");
 return BYTE(ind);
}


int ATurns::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 siz+=ver1.Length();
 siz+=ver2.Length();
 siz+=4*sizeof(TStringList);
 
 foreach(this){
  ATurn *turn=*(ATurn**) iter;
  if(!turn) continue;
  siz+=turn->GetSize();
 }
 return siz;
}


int ATurn::GetSize()
{
 int siz=sizeof(*this);
 siz+=RegionList->GetSize();
 siz+=factions->GetSize();
 return siz;
}




bool ATurn::SaveMap(TFileStream * file,int mapoptions)
{
 bool notallreg=!(mapoptions&roAllRegion);
 foreach(RegionList){
  ARegion *reg=*(ARegion**)iter;
  if(reg->zloc==0) continue;
  if(notallreg){
   if(reg->oldinfoturnnum!=num)
    continue;
/*   AFaction *fac=factions->GetFaction(reg->lastviewfact);
   if(!fac->local)
//   if(=number&&!reg->PresentFaction(this))
    continue;*/
  }
  if(!reg->WriteReport(file,mapoptions)) return false;
 }
 return true;
}

