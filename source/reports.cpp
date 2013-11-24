//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "reports.h"
#include "region.h"
#include "faction.h"
#include "turn.h"
#include "editskilltypefrm.h"
#include "edititemtypefrm.h"
#include "unit.h"
#include "orders.h"
#include "turn.h"
#include "items.h"
#include "markets.h"
#include "skills.h"
#include "mapfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

bool ProcessReport(AnsiString fName){
 TMemoryStream*mem=LoadFile(fName);
 char*s=(char*)mem->Memory;
 if(!ProcessHeaderReport(s))return false;
 if(!ProcessErrorReport(s))return false;
 if(!ProcessBattleReport(s))return false;
 if(!ProcessEventReport(s))return false;
 if(!ProcessSkillReport(s))return false;
 if(!ProcessItemReport(s))return false;
 if(!ProcessFactionReport(s))return false;
 if(!ProcessRegionsReport(s))return false;
 if(!ProcessOrdersReport(s))return false;
 delete mem;
 curRegionList->Sort();
 int prvnum=curTurn->num-1;
 if(prvnum<curTurns->curbeginturn) prvnum=0;
 ATurn *turn=curTurns->Get(prvnum);
 turn->VerifyNextTurn();
 curTurn->PrepareOrders();
 AddEvent("Report processed");
 MapForm->ResetMap();
 return true;
}
bool ProcessOrder(AnsiString fName){
 TMemoryStream*mem=LoadFile(fName);
 char*memory=(char*)mem->Memory,*s=memory;
 if(!ProcessOrdersReport(s))return false;
 delete mem;
 curTurn->PrepareOrders();
 AddEvent("Order processed");
 return true;
}

bool ProcessHeaderReport(char*&s){
 if(MyCompare(s,KeyPhrases->Get(keyAtlRepHead))){
  int ret=Application->MessageBox("No header found. Activate turn 0?","Warning",MB_YESNOCANCEL);
  if(ret==IDCANCEL) return false;
  if(ret==IDYES){
   AddEvent("No header found. Activate turn 0.");
   curTurns->SetCurTurn(0);
  }
  curFactions->SetCurFaction(0);
  return true;
 }
 AnsiString str;
 GetNextString(s);
 AnsiString factionstr=GetNextString(s);
 AnsiString turnstr=GetNextString(s);
 int turn=GetTurnNumberFromString(turnstr);
 char*ss=factionstr.c_str();
 AFaction *faction=new AFaction(curFactions);
 faction->name=GetNextFactionName(ss);
 if(faction->name.Length()==0)throw Exception("Error Process Header: faction not found");
 faction->number=atoi(ss);
 faction->local=true;
 AddEvent(AnsiString("Header found. Turn ")+
  turn+" for faction "+faction->FullName());
 curTurns->PrepareTurn(turn);
 if(curTurn->Packed){
  str="Turn ";
  str+=turn;
  str+=" is PACKED";
  AddEvent("Error: "+str);
  Application->MessageBox(str.c_str(),"ERROR",MB_OK);
  return false;
 }
 if(!ProcessUpdateFaction(faction))return false;
 curFactions->SetCurFaction(faction->number);
 delete faction;
 curFaction->Clear(); ///
 int pos;
 pos=factionstr.Pos(KeyPhrases->Get(keyWarSign));
 if(pos==0) curFaction->warpts=0;
 else{
  curFaction->warpts=atoi(factionstr.c_str()+pos
   +KeyPhrases->Get(keyWarSign).Length());
 }
 pos=factionstr.Pos(KeyPhrases->Get(keyTradeSign));
 if(pos==0) curFaction->tradepts=0;
 else{
  curFaction->tradepts=atoi(factionstr.c_str()+pos
   +KeyPhrases->Get(keyTradeSign).Length());
 }
 pos=factionstr.Pos(KeyPhrases->Get(keyMagicSign));
 if(pos==0) curFaction->magepts=0;
 else{
  curFaction->magepts=atoi(factionstr.c_str()+pos
   +KeyPhrases->Get(keyMagicSign).Length());
 }
 if(*s!='\n')return false;
 GetNextString(s);
 curTurns->ver1=GetNextString(s);
 curTurns->ver2=GetNextString(s);
 GetNextString(s);
 if(MyCompare(s,KeyPhrases->Get(keyFacStat)))return false;
 GetNextString(s);

 if(MyCompare(s,KeyPhrases->Get(keyTaxStatSign)))return false;
 s+=KeyPhrases->Get(keyTaxStatSign).Length();
 curFaction->warused=atoi(GetNextToken(s).c_str());
 if(*s!=' '||s[1]!='(')return false;
 s+=2;
 curFaction->warmax=atoi(GetNextToken(s).c_str());
 GetNextString(s);

 if(MyCompare(s,KeyPhrases->Get(keyTradeStatSign)))return false;
 s+=KeyPhrases->Get(keyTradeStatSign).Length();
 curFaction->tradeused=atoi(GetNextToken(s).c_str());
 if(*s!=' '||s[1]!='(')return false;
 s+=2;
 curFaction->trademax=atoi(GetNextToken(s).c_str());
 GetNextString(s);

 if(MyCompare(s,KeyPhrases->Get(keyMagesStatSign)))return false;
 s+=KeyPhrases->Get(keyMagesStatSign).Length();
 curFaction->mageused=atoi(GetNextToken(s).c_str());
 if(*s!=' '||s[1]!='(')return false;
 s+=2;
 curFaction->magemax=atoi(GetNextToken(s).c_str());
 GetNextString(s);

 GetNextString(s);
 AddEvent(curFaction->ReportHeader());
 return true;
}
bool ProcessErrorReport(char*&s){
 if(MyCompare(s,KeyPhrases->Get(keyAtlErrHead)))return true;
 AddEvent(KeyPhrases->Get(keyAtlErrHead));
 AnsiString str=GetNextString(s);
 TStringList *l=curFaction->errors;
 l->Clear();
 do{
  str=GetNextString(s,1);
  if(!str.Length())break;
  l->Add(str);
//  AddEvent(" "+str);
 }while(*s);
 return true;
}
bool ProcessBattleReport(char*&s){
 if(MyCompare(s,KeyPhrases->Get(keyAtlBatHead)))return true;
 AddEvent(KeyPhrases->Get(keyAtlBatHead));
 AnsiString str=GetNextString(s);
 TStringList *l=curFaction->battles;
 l->Clear();
 while(*s&&!IsEventReport(s)){
  str=GetNextString(s,1);
  if(IsEventReport(s)) break;
  l->Add(str);
//  AddEvent(" "+str);
 }
 return true;
}
bool IsEventReport(const char*s){
 return !MyCompare(s,KeyPhrases->Get(keyAtlEvnHead));
}
bool ProcessEventReport(char*&s){
 if(!IsEventReport(s))return true;
 AddEvent(KeyPhrases->Get(keyAtlEvnHead));
 AnsiString str=GetNextString(s);
 TStringList *l=curFaction->events;
 l->Clear();
 do{
  str=GetNextString(s,1);
  if(!str.Length())break;
  l->Add(str);
//  AddEvent(" "+str);
 }while(*s);
 return true;
}
bool IsSkillReport(const char*s){
 return !MyCompare(s,KeyPhrases->Get(keySkillHead));
}
bool ProcessSkillReport(char*&s){
 if(!IsSkillReport(s))return true;
 GetNextString(s);
 ASkillInfo *si;
 AnsiString str;
 while(1){
  while(*s=='\n')s++;
  AnsiString skill=GetNextSkillName(s);
  if(!skill.Length())return true;
  AnsiString skillabr=GetNextToken(s);
  if(*s!=']')throw Exception("Error Process Skill Report: abridgement not found");
  si=new ASkillInfo;
  si->skill=skillabr;
  ASkillType *sk=new  ASkillType;
  sk->name=skill;
  sk->abr=skillabr;
  if(!ProcessUpdateSkill(sk))return false;
  delete sk;
  s++;
  while(*s==' ')s++;
  str=GetNextToken(s);
  si->level=str.ToInt();
  s+=2;
  AnsiString str=GetNextString(s,1);
  si->text=str;
  if(!ProcessUpdateSkillInfo(si))return false;
  delete si;
 }
}
bool ProcessItemReport(char*&s){
 if(MyCompare(s,KeyPhrases->Get(keyItemHead)))return true;
 GetNextString(s);
 AItemType *it;
 AnsiString str,str2;
 char* ss;
 while(1){
  str=GetNextString(s);
  if(str.Length()==0)break;
  it=new AItemType;
//  while(*s=='\n')s++;
  ss=str.c_str();
  it->name=GetNextItemName(ss);
  if(!it->name.Length())return false;
//  it->names=it->name;
  it->abr=GetNextToken(ss);
  if(MyCompare(ss,KeyPhrases->Get(keyWeightSign)))return true;
  ss+=KeyPhrases->Get(keyWeightSign).Length();
  it->weight=GetNextToken(ss).ToInt();
  if(!MyCompare(ss,KeyPhrases->Get(keyCapacSign))){
   ss+=KeyPhrases->Get(keyCapacSign).Length();
   it->walk=it->weight+GetNextToken(ss).ToInt();
  }
  if(!MyCompare(ss,KeyPhrases->Get(keyCanFly))){
   ss+=KeyPhrases->Get(keyCanFly).Length();
   it->ride=it->walk;
   it->fly=it->walk;
  }
  if(!MyCompare(ss,KeyPhrases->Get(keyCanRide))){
   ss+=KeyPhrases->Get(keyCanRide).Length();
   it->ride=it->walk;
  }
  if(!MyCompare(ss,KeyPhrases->Get(keyIsMan))){
   ss+=KeyPhrases->Get(keyIsMan).Length();
   it->type|=IT_MAN;
  }
  if(!MyCompare(ss,KeyPhrases->Get(keyIsMon))){
   ss+=KeyPhrases->Get(keyIsMon).Length();
   it->type|=IT_MONSTER;
  }
  if(!ProcessUpdateItem(it))return false;
  delete it;
 }
 return true;
}
bool ProcessFactionReport(char*&s){
 if(MyCompare(s,KeyPhrases->Get(keyAttitudes)))return true;
 s+=KeyPhrases->Get(keyAttitudes).Length();

 AnsiString defattstr=GetNextToken(s);
 GetNextString(s);
 int defatt=AttitudeStrToInt(defattstr);
 if(defatt==-1)return false;
 if(defatt!=curFaction->defattitude){
  curFaction->defattitude=defatt;
  AddEvent("Default attitude changed to "+defattstr);
 }
 for(int curatt=A_HOSTILE;curatt<=A_ALLY;curatt++){
  if(MyCompare(s,AnsiString(AttitudeStrs[curatt])))return false;
  s+=strlen(AttitudeStrs[curatt]);
  if(*s==' ')s++;
  if(*s==':')s++;
  {
   AnsiString atts=GetNextString(s,1);
   char *ss=atts.c_str();
   do{
    if(*ss==',')ss++;
    if(*ss==' ')ss++;
    AnsiString facname=GetNextFactionName(ss);
    if(facname.Length()==0)break;
    int facnum=GetNextToken(ss).ToInt();
    if(*ss!=')')return false;
    AFaction *fac=new AFaction(curFactions);
    fac->name=facname;
    fac->number=facnum;
    if(!ProcessUpdateFaction(fac))return false;
    if(curFaction->GetAttitude(facnum)!=curatt){
     AddEvent("Attitude to "+fac->FullName()+" changed to "+AttitudeStrs[curatt]);
    }
    delete fac;
    curFaction->SetAttitude(facnum,curatt);
    ss++;
   }while(*ss==',');
  }
 }
 GetNextString(s);
 if(MyCompare(s,KeyPhrases->Get(keyUnclaimed)))return false;
 s+=KeyPhrases->Get(keyUnclaimed).Length();
 curFaction->unclaimed=atoi(s);
 AddEvent(KeyPhrases->Get(keyUnclaimed)+curFaction->unclaimed);
 GetNextString(s);
 GetNextString(s);
 return true;
}
bool IsRegionReport(const char *s){
 if(*s==0||*s=='\n') return false;
 char*ss=const_cast<char*>(s);
 AnsiString terrain=GetNextToken(ss);
 if(TerrainTypes->Get(terrain))return true;
 return false;
}
bool ProcessRegionsReport(char*&s){
/* while(1){
  if(*s==0||IsOrderTemplate(s))break;
  if(IsRegionReport(s)){
   if(!ProcessRegionReport(s))return false;
  }else GetNextString(s);
 }*/
 while(IsRegionReport(s)){
  if(!ProcessRegionReport(s))return false;
 }
 if(*s=='\n')s++;
 return true;
}
ARegion* ProcessRegionHeader(AnsiString header){
 ARegion *reg=new ARegion(curTurn);
// reg->turn=curTurnNumber;
 char* s=header.c_str();
 reg->type=GetNextToken(s);
 if(*s!=' ')return 0;
 s++;
 ALocation *loc=GetNextLocation(s);
 if(!loc)return 0;
 reg->xloc=loc->xloc;
 reg->yloc=loc->yloc;
 reg->zloc=loc->zloc;
 delete loc;
 if(MyCompare(s,KeyPhrases->Get(keyInSign)))return 0;
 s+=KeyPhrases->Get(keyInSign).Length();
 char*ss=s;
 while(*ss!='.'&&*ss!=','&&*ss!='\n') ss++;
 if(*ss=='\n')return 0;
 reg->name=AnsiString(s,ss-s);
 s=ss;
 if(!MyCompare(s,KeyPhrases->Get(keyContains))){
  s+=KeyPhrases->Get(keyContains).Length();
  reg->townname=GetNextItemName(s);
  reg->towntype=GetNextToken(s);
  if(*s!=']')return 0;
  s++;
  if(!ProcessUpdateTownType(reg->towntype))return 0;
 }
 if(*s==','&&s[1]==' '){
  s+=2;
  reg->hasInfo=true;
  reg->oldinfoturnnum=curTurnNumber;
//  reg->infoStatus|=isRegionInfo;
//  reg->oldreginfo=curTurnNumber;
  reg->population=GetNextToken(s).ToInt();
  if(MyCompare(s,KeyPhrases->Get(keyPeasants)))return 0;
  s+=KeyPhrases->Get(keyPeasants).Length();
  ss=s;
  while(*ss!=')'&&*ss!='\n')ss++;
  if(*ss!=')')return 0;
  reg->race=AnsiString(s,ss-s);
  s=ss+1;
  if(!MyCompare(s,KeyPhrases->Get(keySilvSign))){
   s+=KeyPhrases->Get(keySilvSign).Length();
   reg->money=GetNextToken(s).ToInt();
  }
 }
 if(*s!='.')return 0;
 return reg;
}
bool ProcessWeatherInfo(ARegion *reg,char*&buf){
 char*ss=buf;
 while(*ss==' ')ss++;
 char* s=ss;
 while(*s!=';'&&*s!='\n')s++;
 if(*s!=';')return false;
 if(s[1]!=' ')return false;
 s+=2;
 AnsiString curweat(ss,s-ss);
 AnsiString nextweat=GetNextString(s,3);
 AnsiString curname=WeatherTypes->FindCurWeather(curweat);
 if(!curname.Length())return false;
 AnsiString nxtname=WeatherTypes->FindNextWeather(nextweat);
 if(!nxtname.Length())return false;
 reg->weather=curname;
 reg->nxtweather=nxtname;
 reg->hasInfo=true;
 reg->oldinfoturnnum=curTurnNumber;
// reg->infoStatus|=isWeatherInfo;
// reg->oldweatherinfo=curTurnNumber;
 buf=s;
// GetNextString(buf);
 return true;
};
bool IsWagesReport(char *s){
 return !MyCompare(s,KeyPhrases->Get(keyWages));
}
bool ProcessItemsList(char *&s, AItems *list){
 if(!MyCompare(s,KeyPhrases->Get(keyNoneSign))){
  s+=KeyPhrases->Get(keyNoneSign).Length();return true;
 }
 while(1){
  int count=1;
  if(*s>='0'&&*s<='9'){
   count=atoi(GetNextToken(s).c_str());
   if(*s!=' ')return false;
   s++;
  }
  AnsiString itname=GetNextItemName(s);
  if(!itname.Length())return false;
  AnsiString itabr=GetNextToken(s);
  if(*s!=']') return false;
  s++;
  AItemType *itype=new AItemType;
  if(count<2)itype->name=itname;
  else itype->names=itname;
  bool isillus=false;
  if(!MyCompare(s,KeyPhrases->Get(keyItemIllusionSign))){
   s+=KeyPhrases->Get(keyItemIllusionSign).Length();
   isillus=true;
  }
  if(!isillus && list->GetItem(itabr)){
   AnsiString mes;
   mes="Possible illusion item detected \""+itabr+"\"";
   AddEvent(mes);
   isillus=true;
  }
  if(isillus){
   itype->abr=ItemTypes->NormalAbrToIllusion(itabr);
  }else{
   itype->abr=itabr;
  }
  itype->weight=-1;
  if(!ProcessUpdateItem(itype))return false;
  itabr=itype->abr;
  delete itype;
  AItem *it=new AItem;
  it->count=count;
  it->type=itabr;
  list->Add(it);
  if(*s=='.'||*s==';')break;
  if(*s!=','||s[1]!=' ')return false;
  s+=2;
 }
 return true;
}
bool ProcessMarketsList(char *&s, AMarkets *list){
 if(!MyCompare(s,KeyPhrases->Get(keyNoneSign))){
  s+=KeyPhrases->Get(keyNoneSign).Length();return true;
 }
 while(1){
  int amount=1;
  if(!MyCompare(s,KeyPhrases->Get(keyUnlimitedSign))){
   s+=KeyPhrases->Get(keyUnlimitedSign).Length();
   if(*s!=' ')return false;
   s++;
   amount=9999;
  }else if(*s>='0'&&*s<='9'){
   amount=atoi(GetNextToken(s).c_str());
   if(*s!=' ')return false;
   s++;
  }
  AnsiString itname=GetNextItemName(s);
  if(!itname.Length())return false;
  AnsiString itabr=GetNextToken(s);
  if(*s!=']') return false;
  s++;
  if(MyCompare(s,KeyPhrases->Get(keyMarketAtSign)))return false;
  s+=KeyPhrases->Get(keyMarketAtSign).Length();
  int price=atoi(GetNextToken(s).c_str());
  AItemType *itype=new AItemType;
  if(amount<2)itype->name=itname;
  else itype->names=itname;
  itype->abr=itabr;
  itype->weight=-1;
  if(!ProcessUpdateItem(itype))return false;
  delete itype;
  AMarket *mt=new AMarket;
  mt->type=itabr;
  mt->amount=amount;
  mt->price=price;
  list->Add(mt);
  if(*s=='.')break;
  if(*s!=','||s[1]!=' ')return false;
  s+=2;
 }
 return true;
}
bool ProcessSkillsList(char *&s, ASkills *list){
 if(!MyCompare(s,KeyPhrases->Get(keyNoneSign))){
  s+=KeyPhrases->Get(keyNoneSign).Length();return true;
 }
 while(1){
  AnsiString skillname=GetNextSkillName(s);
  if(!skillname.Length())return false;
  AnsiString skillabr=GetNextToken(s);
  ASkillType *stype=new ASkillType;
  stype->name=skillname;
  stype->abr=skillabr;
  if(!ProcessUpdateSkill(stype))return false;
  delete stype;

  if(*s!=']'||s[1]!=' ')return false;
  s+=2;
  int level=atoi(GetNextToken(s).c_str());
  if(*s!=' '||s[1]!='(')return false;
  s+=2;
  int days=atoi(GetNextToken(s).c_str());
  if(*s!=')')return false;
  s++;
  ASkill *sk=new ASkill;
  sk->type=skillabr;
  sk->days=days;
  if(sk->GetLevel()!=level)throw Exception("ProcessSkillsList: level not match");
  list->Add(sk);
  if(*s=='.'||*s==';')break;
  if(*s!=','||s[1]!=' ')return false;
  s+=2;
 }
 return true;
}
bool ProcessShortSkillsList(char *&s, TStringList *list){
 if(!MyCompare(s,KeyPhrases->Get(keyNoneSign))){
  s+=KeyPhrases->Get(keyNoneSign).Length();return true;
 }
 while(1){
  AnsiString skillname=GetNextSkillName(s);
  if(!skillname.Length())return false;
  AnsiString skillabr=GetNextToken(s);
  ASkillType *stype=new ASkillType;
  stype->name=skillname;
  stype->abr=skillabr;
  if(!ProcessUpdateSkill(stype))return false;
  delete stype;

  if(*s!=']')return false;
  s++;
  list->Add(skillabr);
  if(*s=='.'||*s==';')break;
  if(*s!=','||s[1]!=' ')return false;
  s+=2;
 }
 return true;
}
bool ProcessExitInfo(AnsiString str,ARegion *basereg){
 char *s=str.c_str();
 AnsiString dirstr=GetNextToken(s);
 int dir=DirTypes->Find(dirstr);
 if(dir==-1)return false;
 if(*s!=' '||s[1]!=':'||s[2]!=' ')return false;
 s+=3;
 ARegion *reg=ProcessRegionHeader(s);
 if(!reg)return false;
// AddEvent(reg->WriteReport()); //debug
 bool ret=ProcessUpdateRegion(reg);
 if(ret){
  basereg->neighbors[dir].xloc=reg->xloc;
  basereg->neighbors[dir].yloc=reg->yloc;
  basereg->neighbors[dir].zloc=reg->zloc;
 }
 delete reg;
 return ret;
}
bool inline IsUnitReport(const char*s,int tab){
 for(int i=0;i<tab;i++){
  if(s[i]!=' ')return false;
 }
 if(s[tab]=='-'||s[tab]=='*')return true;
 return false;
}
bool IsObjectReport(const char*s){
 if(*s=='+'||IsUnitReport(s,0))return true;
 return false;
}
bool ProcessUnitReport(AnsiString str,AObject *baseobj){
 AnsiString mes;
 char *s=str.c_str();
 AUnit *unit=new AUnit(curTurn);
 unit->info=(*s=='*')?INFO_LOCAL:INFO_UNIT;
 s+=2;
 unit->name=GetNextFactionName(s);
 if(unit->name=="Unit")unit->name="";
 unit->num=atoi(GetNextToken(s).c_str());

 if(*s!=')')return false;
 s++;
 if(!MyCompare(s,KeyPhrases->Get(keyUnitOnGuard))){
  s+=KeyPhrases->Get(keyUnitOnGuard).Length();
  unit->guard=GUARD_GUARD;
 }
 if(*s==','&&s[1]==' '){
  s+=2;
  AnsiString facname=GetNextFactionName(s);
  if(facname.Length()){
   int facnum=atoi(GetNextToken(s).c_str());
   if(facnum==0)return false;
   if(*s!=')')return false;
   s++;
   AFaction *fac=new AFaction(curFactions);
   fac->name=facname;
   fac->number=facnum;
   if(!ProcessUpdateFaction(fac))return false;
   delete fac;
   unit->SetFaction(facnum);
   if(unit->info!=INFO_LOCAL) unit->info=INFO_FACTION;
  }else{
   unit->SetFaction(0);
   s-=2;
  }
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitAvoid))){
  s+=KeyPhrases->Get(keyUnitAvoid).Length();
  unit->guard=GUARD_AVOID;
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitBehind))){
  s+=KeyPhrases->Get(keyUnitBehind).Length();
  unit->SetFlag(FLAG_BEHIND,true);
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitRevUnit))){
  s+=KeyPhrases->Get(keyUnitRevUnit).Length();
  unit->reveal=REVEAL_UNIT;
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitRevFaction))){
  s+=KeyPhrases->Get(keyUnitRevFaction).Length();
  unit->reveal=REVEAL_FACTION;
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitHold))){
  s+=KeyPhrases->Get(keyUnitHold).Length();
  unit->SetFlag(FLAG_HOLDING,true);
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitTax))){
  s+=KeyPhrases->Get(keyUnitTax).Length();
  unit->SetFlag(FLAG_AUTOTAX,true);
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitNoAid))){
  s+=KeyPhrases->Get(keyUnitNoAid).Length();
  unit->SetFlag(FLAG_NOAID,true);
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitConsumUnit))){
  s+=KeyPhrases->Get(keyUnitConsumUnit).Length();
  unit->SetFlag(FLAG_CONSUMING_UNIT,true);
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitConsumFaction))){
  s+=KeyPhrases->Get(keyUnitConsumFaction).Length();
  unit->SetFlag(FLAG_CONSUMING_FACTION,true);
 }
 if(*s!=','||s[1]!=' ')return false;
 s+=2;
 if(!ProcessItemsList(s,unit->items))return false;
 if(!MyCompare(s,KeyPhrases->Get(keyUnitSkills))){
  s+=KeyPhrases->Get(keyUnitSkills).Length();
  if(!ProcessSkillsList(s,unit->skills))return false;
  unit->hasskills=true;
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitCombatSpell))){
  s+=KeyPhrases->Get(keyUnitCombatSpell).Length();
  AnsiString skillname=GetNextSkillName(s);
  if(!skillname.Length())return false;
  AnsiString skillabr=GetNextToken(s);
  ASkillType *stype=new ASkillType;
  stype->name=skillname;
  stype->abr=skillabr;
  if(!ProcessUpdateSkill(stype))return false;
  delete stype;
  stype=SkillTypes->Get(skillabr,2);
  if(!stype)return false;
  int mustflags=MAGIC|COMBAT;
  if((stype->flags&mustflags)!=mustflags){
   mes="Make skill \""+stype->name+"\" combat spell skill?";
   if(Application->MessageBox(mes.c_str(),"New combat spell detected",MB_YESNO)!=IDYES)return false;
   stype->flags|=mustflags;
  }
  if(*s!=']')return false;
  s++;
  unit->combatspell=skillabr;
  unit->hascombatspell=true;
 }
 if(!MyCompare(s,KeyPhrases->Get(keyUnitCanStudy))){
  s+=KeyPhrases->Get(keyUnitCanStudy).Length();
  TStringList *list=new TStringList;
  if(!ProcessShortSkillsList(s,list))return false;
  unit->canstudy->Assign(list);
  unit->hascanstudy=true;
  delete list;
 }

 AnsiString descr=GetNextString(s);
 int dlen=descr.Length();
 if(dlen){
  if(descr[dlen]=='.')descr.SetLength(dlen-1);
 }
 unit->describe=descr;
 baseobj->AddUnit(unit);
 return true;
}
bool ProcessObjectReport(char*&s,ARegion *basereg){
 int tab=0;
 AnsiString str;
 AObject *obj;
 if(*s=='+'){
  str=GetNextString(s,-1);
  char*olds=s;
  s=str.c_str();
  tab=2;
  s+=2;
  char *ss;
/*  char *ss=s;
  while(*ss!='['&&*ss!='\r'&&*ss!='\n')ss++;
  if(*ss!='[')return false;
  AnsiString name(s,ss-s-1);
  s=ss+1;*/
  AnsiString name=GetNextItemName(s);
  if(name.Length()==0)return false;
  int num=atoi(GetNextToken(s).c_str());
  if(MyCompare(s,KeyPhrases->Get(keyObjNameEndSign)))return false;
  s+=KeyPhrases->Get(keyObjNameEndSign).Length();
  ss=s;
  while(*ss!=','&&*ss!=';'&&*ss!='.'&&*ss!='\n')ss++;
  if(*ss=='\n')return false;
  AnsiString type(s,ss-s);
  s=ss;
  if(!ProcessUpdateObjectType(type,2))return false;
  obj=new AObject(curTurn);
  obj->num=num;
  obj->name=name;
  obj->type=type;
  basereg->Add(obj);
  int canenter=1;
  if(!MyCompare(s,KeyPhrases->Get(keyObjNeedSign))){
   s+=KeyPhrases->Get(keyObjNeedSign).Length();
   obj->incomplete=atoi(GetNextToken(s).c_str());
  }
  if(!MyCompare(s,KeyPhrases->Get(keyObjContainsSign))){
   s+=KeyPhrases->Get(keyObjContainsSign).Length();
   obj->inner=true;
  }
  if(!MyCompare(s,KeyPhrases->Get(keyObjEngravedSign))){
   s+=KeyPhrases->Get(keyObjEngravedSign).Length();
   obj->runes=1;
  }
  if(*s==';'){
   s+=2;
   ss=s;
   while(1){
    while(*ss!=','&&*ss!=0) ss++;
    if(*ss==','){
     if(!MyCompare(ss,KeyPhrases->Get(keyObjClosedSign)))
      break;
     ss++;
    }else break;
   }
   AnsiString descr(s,ss-s);
   s=ss;
   int dlen=descr.Length();
   if(dlen&&descr[dlen]=='.')
    descr.SetLength(dlen-1);
   obj->describe=descr;
  }
  if(!MyCompare(s,KeyPhrases->Get(keyObjClosedSign))){
   s+=KeyPhrases->Get(keyObjClosedSign).Length();
   canenter=0;
//  To Do
  }
  s=olds;
  if(!ProcessUpdateObjectType(type,canenter))return false;
 }else{
  obj=basereg->GetDummy();
 }
 while(IsUnitReport(s,tab)){
  s+=tab;
  str=GetNextString(s,tab+1);
  if(!ProcessUnitReport(str,obj))return false;
 }
 if(*s=='\n')s++;

 if(obj->inner&&obj->describe.Pos(";!")==1){
  ARegion *reg=ProcessRegionHeader(obj->describe.SubString(3,obj->describe.Length())+".");
  if(reg){
   if(ProcessUpdateRegion(reg)){
    ARegion *reg2=curRegionList->Get(reg->xloc,reg->yloc,reg->zloc);
    if(reg2){
     if(reg2->inner.xloc!=-1){
      if(reg2->inner.xloc==basereg->xloc&&
         reg2->inner.yloc==basereg->yloc&&
         reg2->inner.zloc==basereg->zloc){
       obj->describe="";
      }else{
      //to do
      }
     }
     if(reg2->inner.xloc==-1){
      reg2->inner.xloc=basereg->xloc;
      reg2->inner.yloc=basereg->yloc;
      reg2->inner.zloc=basereg->zloc;
      basereg->inner.xloc=reg2->xloc;
      basereg->inner.yloc=reg2->yloc;
      basereg->inner.zloc=reg2->zloc;
      obj->describe="";
     }
    }
   }
   delete reg;
  }
 }
 return true;
}
bool ProcessRegionReport(char*&s){
 AnsiString header=GetNextString(s,1);
 char *ss;
 if(*s!='-')return false;
 GetNextString(s);
 ARegion *reg=ProcessRegionHeader(header);
 if(!reg)return false;
 if(curFaction&&curFaction->number)
  reg->lastviewfact=curFaction->number;

 ProcessWeatherInfo(reg,s);

 if(*s=='\n')s++;
 while(!IsRegionReport(s)&&!IsWagesReport(s)&&(*s!='\n'/*||s[1]!='\n'*/)
  &&!IsObjectReport(s)
  ){
  GetNextString(s);
 }
 if(*s=='\n')s++;
 if(IsWagesReport(s)){
  s+=sizeof("  Wages: $")-1;
//  reg->infoStatus|=isWagesInfo;
//  reg->oldwagesinfo=curTurnNumber;
  reg->wages=GetNextToken(s).ToInt();
  if(reg->wages){
   if(MyCompare(s,KeyPhrases->Get(keyMaxSign)))return false;
   s+=KeyPhrases->Get(keyMaxSign).Length();
   reg->maxwages=GetNextToken(s).ToInt();
  }
  GetNextString(s);
  if(MyCompare(s,KeyPhrases->Get(keyWantedSign)))return false;
  s+=KeyPhrases->Get(keyWantedSign).Length();
  AnsiString wantedstr=GetNextString(s,3);
  ss=wantedstr.c_str();
  if(!ProcessMarketsList(ss,reg->wanteds))return false;
//  reg->infoStatus|=isWantedInfo;
//  reg->oldwantedinfo=curTurnNumber;


  if(MyCompare(s,KeyPhrases->Get(keySaledSign)))return false;
  s+=KeyPhrases->Get(keySaledSign).Length();
  AnsiString saledstr=GetNextString(s,3);
  ss=saledstr.c_str();
  if(!ProcessMarketsList(ss,reg->saleds))return false;
//  reg->infoStatus|=isSaledInfo;
//  reg->oldsaledinfo=curTurnNumber;

  if(!MyCompare(s,KeyPhrases->Get(keyEnteSign))){
   s+=KeyPhrases->Get(keyEnteSign).Length();
//   reg->infoStatus|=isEntertainInfo;
//   reg->oldentertaininfo=curTurnNumber;
   reg->entertainment=GetNextToken(s).ToInt();
   GetNextString(s);
  }
  if(MyCompare(s,KeyPhrases->Get(keyProdSign)))return false;
  s+=KeyPhrases->Get(keyProdSign).Length();
  AnsiString prodstr=GetNextString(s,3);
  ss=prodstr.c_str();
  AItems *items=new AItems;
  if(!ProcessItemsList(ss,items))return false;
  reg->products->Assign(items);
  delete items;
//  reg->infoStatus|=isProductInfo;
//  reg->oldproductinfo=curTurnNumber;
  GetNextString(s);
 }
 if(*s=='\n')s++;
 if(!MyCompare(s,KeyPhrases->Get(keyExitSign))){
//  reg->infoStatus|=isExitsInfo;
//  reg->oldexitsinfo=curTurnNumber;

  GetNextString(s);
  while(*s==' '&&s[1]==' '){
   s+=2;
   AnsiString exitstr=GetNextString(s,3);
   if(!ProcessExitInfo(exitstr,reg))return false;
  }
 }
 if(*s=='\n')s++;
 if(!MyCompare(s,KeyPhrases->Get(keyGatesSign))){
  s+=KeyPhrases->Get(keyGatesSign).Length();
  int gnum=atoi(GetNextToken(s).c_str());
  if(MyCompare(s,KeyPhrases->Get(keyGatesSign2)))return false;
  s+=KeyPhrases->Get(keyGatesSign2).Length();
  int gkol=atoi(GetNextToken(s).c_str());
  reg->gate=gnum;
  if(curRegionList->numberofgates<gkol)curRegionList->numberofgates=gkol;
//  reg->infoStatus|=isGateInfo;
//  reg->oldgateinfo=curTurnNumber;
  GetNextString(s);
  if(*s=='\n')s++;
 }
 if(!IsObjectReport(s)){
  if(*s=='\n')s++;
 };
 if(IsObjectReport(s)){
  reg->hasInfo=true;
  reg->oldinfoturnnum=curTurnNumber;
//  reg->infoStatus|=isObjectInfo;
//  reg->oldobjinfo=curTurnNumber;
 }
 while(IsObjectReport(s)){
  if(!ProcessObjectReport(s,reg))return false;
 }
 if(*s=='\n')s++;
 if(!(optDontShowEvents&dseRegDataFound))
  AddEvent("Region data for "+reg->FullName()+" found");
// AddEvent(reg->WriteReport());
 bool ret=ProcessUpdateRegion(reg);
 delete reg;
 return ret;
}
bool IsOrderReport(const char*s){
 return !MyCompare(s,KeyPhrases->Get(keyTemplateSign));
}
bool IsOrdersBegin(const char*s){
 return !MyCompare(s,KeyPhrases->Get(keyTemplateBegin));
}
bool ProcessOrdersReport(char*&s){
 if(!IsOrdersBegin(s)){
  if(!IsOrderReport(s))return true;
  GetNextString(s);
  GetNextString(s);
 }
 if(!IsOrdersBegin(s))return false;
 s+=KeyPhrases->Get(keyTemplateBegin).Length();
 {
  int facnum=atoi(GetNextToken(s).c_str());
  if(!curFaction||curFaction->number!=facnum){
   curFactions->SetCurFaction(facnum);
  }
  if(!curFaction||curFaction->number!=facnum)return false;
  AnsiString pas;
  if(*s==' '){
   s++;
   pas=GetNextToken(s);
  }
  if(pas!=curFaction->password){
   AnsiString mes="Found faction "+curFaction->FullName();
   mes+="\npassword changing.";
   mes+="\nChange password?";
   int ret=IDYES;//Application->MessageBox(mes.c_str(),"Updating Faction Password",MB_YESNOCANCEL);
   if(ret==IDCANCEL)return false;
   if(ret==IDYES){
    curFaction->password=pas;
    AddEvent("Faction "+curFaction->FullName()+" password changed");
   }
  }
  GetNextString(s);
 }
 curFaction->orders->Clear();
 AOrder *ord;
 AUnit *unit=0, *former=0;
 AnsiString str;
 while(1){
  while(*s=='\n')s++;
  if(*s==0)return false;
  if(*s==';'){
   GetNextString(s);
   continue;
  }
  str=GetNextString(s);
  ord=ParseOrder(str);
  if(!ord)return false;
  if(ord->type==O_TEACH){
   AOrderTeach *tord=dynamic_cast<AOrderTeach*>(ord);
   char *ss;
   bool cont=true;
   while(cont&&*s==' '){
    ss=s;
    str=GetNextString(ss);
    char *_s=str.c_str();
    while(*_s==' ') _s++;
    cont=tord->ParseOrder(_s);
    if(cont){
     s=ss;
    }
   };

  }else if(ord->type==O_END){
   delete ord;
   if(former){
    AddEvent("FORM without END");
   }
   break;
  }else if(ord->type==O_UNIT){
   delete ord;
   char *ss=str.c_str();
   while(*ss&&(*ss<'0'||*ss>'9'))ss++;
   if(!*ss)return false;
   int unitnum=atoi(ss);
   unit=curRegionList->GetUnit(unitnum);
   if(!unit){
    str="Unit ";
    str+=unitnum;
    str+=" not found";
    AddEvent(str);
    return false;
   }
   unit->orders->Clear();
   continue;
  }
  if(!unit){
   AddEvent("no UNIT decalred");
   return false;
  }
  if(ord->type==O_FORM){
   if(former){
    AddEvent("FORM in FORM");
    return false;
   }
   char *ss=str.c_str();
   while(*ss&&(*ss<'0'||*ss>'9'))ss++;
   if(!*ss)return false;
   int unitnum=-atoi(ss);
   former=unit;
   unit=former->baseobj->basereg->GetUnit(unitnum);
   if(!unit){
    unit=new AUnit(curTurn);
    unit->num=unitnum;
    former->baseobj->units->Add(unit);
   }
   unit->FormNew(former);
   unit->orders->Clear();
   continue;
  }
  if(ord->type==O_FORMEND){
   if(!former){
    AddEvent("END without FORM");
    return false;
   }
   unit=former;
   former=0;
   continue;
  }
  unit->orders->Add(ord);
 }
 return true;
}

bool ProcessUpdateFaction(AFaction *newFaction,int ver){
 if(ver==-1) ver=curFactions->VerFaction(newFaction->name,newFaction->number);
//0 - not found, 1 - present, 2 - name, not num, 3 - num, not name
 AFaction *fac,*fac2;
 AnsiString mes;
 switch(ver){
  case 0:
   fac=new AFaction(curFactions);
   fac->CreateNew(newFaction);
   curFactions->Add(fac);
   AddEvent(AnsiString("New faction ")+fac->FullName()+(fac->local?" added as local faction":" added"));
  break;
  case 1:
   if(newFaction->local){
    fac=curFactions->GetFaction(newFaction->number);
    if(!fac->local){
     fac->local=true;
     AddEvent("Set faction "+fac->FullName()+" as local faction");
    }
   }
  break;
  case 2:
   fac=curFactions->GetFaction(newFaction->name);
   mes="Found new faction: "+newFaction->FullName();
   mes+="\nwith present name: "+fac->FullName();
   mes+="\nDelete old faction?";
   int ret;
   if(fac->name=="Faction"||
    (ret=Application->MessageBox(mes.c_str(),"Updating Faction",MB_YESNOCANCEL))==IDNO){
    fac=new AFaction(curFactions);
    fac->CreateNew(newFaction);
    curFactions->Add(fac);
    AddEvent(AnsiString("New faction ")+fac->FullName()+(fac->local?" added as local faction":" added"));
    return true;
   }
   if(ret==IDCANCEL) return false;
   mes=AnsiString("Old faction ")+fac->FullName()+" deleted";
   curFactions->Delete(fac->number);
   AddEvent(mes);
   fac=new AFaction(curFactions);
   fac->CreateNew(newFaction);
   curFactions->Add(fac);
   AddEvent(AnsiString("New faction ")+fac->FullName()+(fac->local?" added as local faction":" added"));
  break;
  case 3:
   fac=curFactions->GetFaction(newFaction->number);
   if(newFaction->name=="Faction") fac2=0;
   else fac2=curFactions->GetFaction(newFaction->name);
   if(!fac2){
    mes="Found faction (";
    mes+=newFaction->number;
    mes+=") name changing:\nOld name: "+fac->name;
    mes+="\nNew name: "+newFaction->name+"\nRename faction?";
    if(Application->MessageBox(mes.c_str(),"Updating Faction",MB_YESNO)!=IDYES){
     return false;
    }
    fac->name=newFaction->name;
    fac->local=newFaction->local;
    mes="Faction (";
    mes+=fac->number;
    mes+=") renamed";
    AddEvent(mes);
   }else{
    mes="Found faction (";
    mes+=newFaction->number;
    mes+=") name changing to present name:\nOld name: "+fac->name;
    mes+="\nNew name: "+newFaction->name;
    mes+="\nOld faction: "+fac2->FullName();
    mes+="\nRename faction and delete old?";
    if(/*fac2->name=="Faction"||*/
     Application->MessageBox(mes.c_str(),"Updating Faction",MB_YESNOCANCEL)==IDNO){
     mes+="Faction (";
     mes+=fac->number;
     mes+=") renamed";
     fac->name=newFaction->name;
     fac->local=newFaction->local;
     AddEvent(mes);
     return true;
    }
    if(ret==IDCANCEL) return false;
    mes+="Faction (";
    mes+=fac->number;
    mes+=") renamed";
    mes+="; old faction (";
    mes+=fac2->number;
    mes+=") deleted";
    curFactions->Delete(fac2->number);
    fac->name=newFaction->name;
    fac->local=newFaction->local;
    AddEvent(mes);
   }
  break;
 }
 return true;
}
bool ProcessUpdateSkill(ASkillType *newSkill,int ver){
 if(ver==-1)ver=SkillTypes->VerSkills(newSkill->name, newSkill->abr);
//0 - not found, 1 - present, 2 - name, not abr, 3 - abr, not name
 ASkillType *sk=0;
 AnsiString mes;
 TEditSkillTypeForm *estf;
 switch(ver){
  case 0:
   sk=new ASkillType;
   sk->CreateNew(newSkill);
   estf=new TEditSkillTypeForm(0,sk);
   estf->Caption=estf->Caption+": New skill found";
   if(estf->ShowModal()!=mrOk){delete estf;return false;}
   delete estf;
   SkillTypes->AddSkillType(sk);
   AddEvent(AnsiString("New skill ")+sk->FullName()+" added");
  break;
  case 1:
  break;
  case 2:
   sk=SkillTypes->Get(newSkill->name,1);
  case 3:
   if(!sk)sk=SkillTypes->Get(newSkill->abr,2);
   if(!sk)throw Exception("Fatal error in ProcessUpdateSkill: "+newSkill->abr);
   estf=new TEditSkillTypeForm(0,sk,newSkill);
   estf->Caption=estf->Caption+": skill changes found";
   if(estf->ShowModal()!=mrOk){delete estf;return false;}
   delete estf;
/*   mes="Found new skill with present name or abridgement.\nOld skill: ";
   mes+=sk->FullName();
   mes+="\nNew skill: ";
   mes+=newSkill->FullName();
   mes+="\nRename skill?";
   if(Application->MessageBox(mes.c_str(),"Updating Skill types",MB_YESNO)!=IDYES){
    return false;
   }
   mes="Old skill "+sk->FullName()+" renamed to "+newSkill->FullName();
   sk->name=newSkill->name;
   sk->abr=newSkill->abr;*/
   mes="Skill "+sk->FullName()+" updated";
   AddEvent(mes);
  break;
 }
 return true;
}
bool ProcessUpdateSkillInfo(ASkillInfo *newSi){
 ASkillInfo *si=SkillInfos->Get(newSi->skill,newSi->level);
 AnsiString mes;
 if(!si){
  si=new ASkillInfo;
  si->CreateNew(newSi);
  SkillInfos->AddSkillInfo(si);
  AddEvent(AnsiString("New skill info ")+si->FullName()+" added");
 }else{
  si=SkillInfos->Get(newSi->skill,newSi->level);
  if(si->text==newSi->text)return true;
  mes="Found skill "+si->FullName()+" with other description\nOld descr: ";
  mes+=si->text;
  mes+="\nNew descr: ";
  mes+=newSi->text;
  mes+="\nReplace?";
  int ret=Application->MessageBox(mes.c_str(),"Updating skill report",MB_YESNOCANCEL);
  if(ret==IDCANCEL)return false;
  if(ret==IDYES){
   si->text=newSi->text;
   mes="Description for "+si->FullName()+" changed";
   AddEvent(mes);
  }
 }
 return true;
}
bool ProcessUpdateItem(AItemType *newit){
 AItemType *it=ItemTypes->Get(newit->abr,4);
 if(!it){
  if(!it)it=ItemTypes->Get(newit->name,1);
  if(!it)it=ItemTypes->Get(newit->names,2);
  if(it&&newit->abr.Length()&&it->abr.Length()&&
   (ItemTypes->IsIllusionAbr(it->abr)!=
   ItemTypes->IsIllusionAbr(newit->abr)))
   it=0;
 }
 AnsiString mes;
 TEditItemTypeForm *eitf;
 if(!it){
  it=new AItemType;
  it->CreateNew(newit);
  eitf=new TEditItemTypeForm(0,it);
  eitf->Caption=eitf->Caption+": New item found";
  if(eitf->ShowModal()!=mrOk){delete eitf;return false;}
  delete eitf;
  ItemTypes->AddItemType(it);
  AddEvent(AnsiString("New item ")+it->Print(1)+" added");
 }else{
  if(!it->IsNewInfo(newit))return true;
/*  if(newit->name.Length()==0)newit->name=newit->names;
  if(newit->names.Length()==0)newit->names=newit->name;*/
  eitf=new TEditItemTypeForm(0,it,newit);
  eitf->Caption=eitf->Caption+": item changes found";
  if(eitf->ShowModal()!=mrOk){delete eitf;return false;}
  delete eitf;
  mes="Item "+it->FullName()+" updated";
  AddEvent(mes);

/*  mes="Found item "+it->name+" with other paremeters\nOld param: ";
  mes+=it->Print(true);
  mes+="\nNew param: ";
  mes+=newit->Print(true);
  mes+="\nUpdate parameters?";
  int ret=Application->MessageBox(mes.c_str(),"Updating items types",MB_YESNOCANCEL);
  if(ret==IDCANCEL)return false;
  if(ret==IDYES){
   it->UpdateNewInfo(newit);
   mes="Parameners for "+it->name+" changed";
   AddEvent(mes);
  }*/
 }
 return true;
}
bool ProcessUpdateTownType(AnsiString newttype){
 int ind=TownTypes->GetIndex(newttype);
 if(ind!=-1)return true;
 AnsiString mes="New Town type found: "+newttype;
 mes+="\nAdd it?";
 if(Application->MessageBox(mes.c_str(),"Updating Town types",MB_YESNO)!=IDYES){
  return false;
 }
 ATownType *ttype=new ATownType;
 ttype->name=newttype;
 TownTypes->AddTownType(ttype);
 mes="new town types \""+newttype+"\" added";
 AddEvent(mes);
 return true;
}
bool ProcessUpdateRegionArray(AnsiString newname){
 int ind=curRegionList->RegionArrays->GetLevelNum(newname);
 if(ind!=-1)return true;
 AnsiString mes="New Level found: "+newname;
 mes+="\nAdd it?";
 if(Application->MessageBox(mes.c_str(),"Updating Level",MB_YESNO)!=IDYES){
  return false;
 }
/* ARegionArray *ra=new ARegionArray;
 ra->name=newname;
 curRegionList->RegionArrays->Add(ra);*/
 mes="new level \""+newname+"\" added";
 AddEvent(mes);
 ATurn *turn0=curTurns->Get(0);
 if(!turn0)return false;
// ra=turn0->RegionList->RegionArrays->Get(newname);
// if(!ra){
  ARegionArray *ra=new ARegionArray;
  ra->name=newname;
  turn0->RegionList->RegionArrays->Add(ra);
  turn0->VerifyNextTurn();
// }
 return true;
}
bool ProcessUpdateRegion(ARegion *newreg){
 ARegion *reg=curRegionList->Get(newreg->xloc,newreg->yloc,newreg->zloc);
 if(!reg){
  reg=new ARegion(curTurn);
//  reg->turn=curTurnNumber;
  reg->xloc=newreg->xloc;
  reg->yloc=newreg->yloc;
  reg->zloc=newreg->zloc;
  curRegionList->Add(reg);
 }
 reg->Update(newreg);
 ARegionArray *ra=curRegionList->GetRegionArray(reg->zloc);
 ra->VerifySize(reg->xloc, reg->yloc);
 return true;
}
bool ProcessUpdateObjectType(AnsiString name,int canenter){
 AObjectType *ot=ObjectTypes->Get(name);
 AnsiString mes;
 if(ot){
  if(canenter==2)return true;
  if(ot->canenter==2){
   ot->canenter=canenter;
   return true;
  }
  if(ot->canenter!=canenter){
   mes="Object \""+name+"\" canenter status changing detected\n";
   mes+="Old value: ";
   mes+=ot->canenter;
   mes+="\nNew value: ";
   mes+=canenter;
   mes+="\nChange it?";
   int r=Application->MessageBox(mes.c_str(),"Updating Object type",MB_YESNOCANCEL);
   if(r==IDCANCEL)return false;
   if(r==IDYES){
    ot->canenter=canenter;
   }
  }
  return true;
 }
 ot=new AObjectType;
 ot->name=name;
 ot->canenter=canenter;
 mes="New Object Type : "+name+" added";
 ObjectTypes->AddObjectType(ot);
 AddEvent(mes);
 return true;
}

bool SaveReport(AnsiString fName,int facnum){
 if(!curFaction||curFaction->number!=facnum){
  curFactions->SetCurFaction(facnum);
 }
 if(!curFaction||curFaction->number!=facnum)return false;
 TFileStream *file=0;
 try{
  file=new TFileStream(fName,fmCreate);
 }catch(Exception &c){
  Application->ShowException(&c);
 }
 if(!file)return false;
 ReportTab=0;
 try{
  if(!curFaction->SaveReport(file))return false;
 }catch(Exception&e){
  delete file;
  Application->MessageBox(e.Message.c_str(),"error saving report",MB_OK);
  return false;
 }
 delete file;
 AnsiString s="Report for faction ";
 s+=curFaction->FullName();
 s+=" saved";
 AddEvent(s);
 return true;
}
bool SaveOrder(AnsiString fName,int facnum){
 if(!curFaction||curFaction->number!=facnum){
  curFactions->SetCurFaction(facnum);
 }
 if(!curFaction||curFaction->number!=facnum)return false;
 TFileStream *file=0;
 try{
  file=new TFileStream(fName,fmCreate);
 }catch(Exception &c){
  Application->ShowException(&c);
 }
 if(!file)return false;
 try{
  if(!curFaction->SaveTemplate(file))return false;
 }catch(Exception&e){
  delete file;
  Application->MessageBox(e.Message.c_str(),"error saving order",MB_OK);
  return false;
 }
 delete file;
 AnsiString s="Order for faction ";
 s+=curFaction->FullName();
 s+=" saved";
 AddEvent(s);
 return true;
}
bool SaveMap(AnsiString fName,int mapoptions){
 TFileStream *file=0;
 try{
  file=new TFileStream(fName,fmCreate);
 }catch(Exception &c){
  Application->ShowException(&c);
 }
 if(!file)return false;

 try{
  if(!curTurn->SaveMap(file,mapoptions))return false;
 }catch(Exception&e){
  delete file;
  Application->MessageBox(e.Message.c_str(),"error saving map",MB_OK);
  return false;
 }
 delete file;
 AddEvent("Map saved");
 return true;
}

