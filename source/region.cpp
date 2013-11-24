//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "util.h"
#include "region.h"
#include "eapfile.h"
#include "turn.h"
#include "unit.h"
#include "viewitems.h"
#include "items.h"
#include "markets.h"
#include "faction.h"
#include "orders.h"
#include "skills.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
void ALocation::Read(TFile & in)
{
 if(BaseVer>=5){
  ReadIntAsShort(in,xloc);
  if(xloc!=-1){
   ReadIntAsShort(in,yloc);
   ReadIntAsShort(in,zloc);
  }
 }else{
  in>>xloc;
  if(xloc!=-1){
   in>>yloc;
   in>>zloc;
  }
 }
}
void ALocation::Write(TFile & out)
{
 WriteIntAsShort(out,xloc);
 if(xloc!=-1){
  WriteIntAsShort(out,yloc);
  WriteIntAsShort(out,zloc);
 }
}
ARegion*curRegion;
ARegion::ARegion(ATurn *t)
 :num(-1),
  hasInfo(false),
  xloc(-1),yloc(-1),zloc(-1),
  population(0),money(0),wages(0),maxwages(0),entertainment(0),
  gate(0),lastviewfact(0),taxincome(0),curtaxprod(0),taxers(0),
  workincome(0),workers(0),entincome(0),entattempt(0),
  turn(t),
  FRegionStatus(0)
{
 wanteds=new AMarkets;
 saleds=new AMarkets;
 products=new AMarkets;
 CreateDummy();
}
_fastcall ARegion::~ARegion()
{
 Clear();
 if(curRegion==this)curRegion=0;
 delete products;products=0;
 delete wanteds;wanteds=0;
 delete saleds;saleds=0;
 ClearRegionStatus();
}
AObject * ARegion::Get(int num)
{
 return (AObject*)Items[num];
}
AObject *ARegion::GetObject(int objnum)
{
 foreach(this){
  AObject *obj=*(AObject**) iter;
  if(obj->num==objnum)return obj;
 }
 return 0;
}
AUnit * ARegion::GetUnit(int num)
{
 AUnit *unit;
 foreach(this){
  AObject *obj=*(AObject**) iter;
  unit=obj->GetUnit(num);
  if(unit) return unit;
 }
 return 0;
}

void ARegion::Add(AObject * obj)
{
 AObject *curobj=GetObject(obj->num);
 if(curobj)throw Exception("Error ARegion::Add(): object already present");
 TList::Add(obj);
 obj->basereg=this;
}
void __fastcall ARegion::Clear()
{
 int i=Count-1;
 AObject **iter=(AObject**)List+i;
 for(;i>=0;i--,iter--){
  delete *iter;
//  Delete(i);
 }
 TList::Clear();
}
void ARegion::Read(TFile& in)
{
 Clear();

 in>>num;
 if(BaseVer<5||turn->Package){
  in>>type>>name;
 }else{
  BYTE _type;
  WORD _name;
  in>>_type;
  type=curTurns->tertypes->Strings[_type];
  in>>_name;
  name=curTurns->provincies->Strings[_name];
 }
 if(BaseVer>=5){
  ReadIntAsShort(in,xloc);
  ReadIntAsShort(in,yloc);
  ReadIntAsShort(in,zloc);
 }else in>>xloc>>yloc>>zloc;

 if(BaseVer<4){
  in>>lastviewfact;
 }
 if(BaseVer>=8){
  in>>hasInfo;
  if(turn->Package){
   in>>towntype;
   if(towntype.Length()){
    in>>townname;
   }
  }else{
   BYTE _towntype;
   WORD _townname;
   in>>_towntype;
   towntype=curTurns->towntypes->Strings[_towntype];
   if(towntype.Length()){
    in>>_townname;
    townname=curTurns->towns->Strings[_townname];
   }
  }
  inner.Read(in);
  if(hasInfo){
   oldinfoturnnum=turn->num;

   ReadStringAsInt(in,race);
   race=ItemTypes->AbrToNames(race);
   in>>population>>money;
   in>>taxincome;
   in>>workincome;
   in>>entincome;
   in>>lastviewfact;
   in>>weather>>nxtweather;
   in>>wages>>maxwages;
   in>>entertainment;
   for(int i=0;i<NDIRS;i++){
    neighbors[i].Read(in);
   }
   in>>gate;
   wanteds->Read(in);
   saleds->Read(in);
   products->Read(in);
   int numreg;
   in>>numreg;
   for(int i=0;i<numreg;i++){
    AObject *obj=new AObject(turn);
    obj->Read(in);
    Add(obj);
   }
  }
 }else{
  int saveStatus;
  in>>saveStatus;
  hasInfo=saveStatus&ssWeatherInfo;
  if(BaseVer<5||turn->Package){
   in>>towntype;
   if(towntype.Length()){
    in>>townname;
   }
  }else{
   BYTE _towntype;
   WORD _townname;
   in>>_towntype;
   towntype=curTurns->towntypes->Strings[_towntype];
   if(towntype.Length()){
    in>>_townname;
    townname=curTurns->towns->Strings[_townname];
   }
  }
  if(hasInfo){
   oldinfoturnnum=turn->num;
  }
  inner.Read(in);
  if(saveStatus&ssRegionInfo){
   if(BaseVer>=5){
    ReadStringAsInt(in,race);
    race=ItemTypes->AbrToNames(race);
   }else in>>race;

   in>>population>>money;
   if(BaseVer>=4){
    in>>taxincome;
    in>>workincome;
    in>>entincome;
    in>>lastviewfact;
   }
  }
  if(BaseVer<4){
   in>>taxincome;
   if(BaseVer>=2){
    in>>workincome;
    in>>entincome;
   }
  }
  if(saveStatus&ssWeatherInfo){
   in>>weather>>nxtweather;
  }
  if(saveStatus&ssWagesInfo){
   in>>wages>>maxwages;
  }
  if(saveStatus&ssEntertainInfo){
   in>>entertainment;
  }
  if(saveStatus&ssExitsInfo){
   for(int i=0;i<NDIRS;i++){
    neighbors[i].Read(in);
   }
  }
  if(saveStatus&ssGateInfo){
   in>>gate;
  }
  if(saveStatus&ssWantedInfo){
   wanteds->Read(in);
  }
  if(saveStatus&ssSaledInfo){
   saleds->Read(in);
  }
  if(saveStatus&ssProductInfo){
   products->Read(in);
  }
  if(saveStatus&ssObjectInfo){
   int size;
   in>>size;
   for(int i=0;i<size;i++){
    AObject *obj=new AObject(turn);
    obj->Read(in);
    Add(obj);
   }
  }
  if(turn->num){
   if(saveStatus&ssWeatherInfo&&lastviewfact==0){
    AFaction *locfac=0;
    foreach(turn->factions){
     AFaction *fac=*(AFaction**) iter;
     if(!fac->local) continue;
     if(!locfac) locfac=fac;
     if(PresentFaction(fac)){
      lastviewfact=fac->number;
      break;
     }
    }
    if(locfac) lastviewfact=locfac->number;
   }
  }
 }
 if(!count) CreateDummy();
}
void ARegion::Write(TFile& out)
{
 out<<num;
 BYTE _type=curTurns->Tertypes(type);
 WORD _name=curTurns->Provincies(name);
 BYTE _towntype=curTurns->Towntypes(towntype);
 WORD _townname=curTurns->Towns(townname);
 if(turn->Package)
  out<<type<<name;
 else
  out<<_type<<_name;
 WriteIntAsShort(out,xloc);
 WriteIntAsShort(out,yloc);
 WriteIntAsShort(out,zloc);

 int turnnum=turn->num;

 bool curhasInfo=hasInfo&&oldinfoturnnum==turnnum;
 out<<curhasInfo;
 if(turn->Package){
  out<<towntype;
  if(towntype.Length()){
   out<<townname;
  }
 }else{
  out<<_towntype;
  if(towntype.Length()){
   out<<_townname;
  }
 }
 inner.Write(out);
 if(curhasInfo){
  AnsiString raceabr=ItemTypes->NameToAbr(race);
  WriteStringAsInt(out,raceabr);
  out<<population<<money;
  out<<taxincome;
  out<<workincome;
  out<<entincome;

  out<<lastviewfact;
  out<<weather<<nxtweather;
  out<<wages<<maxwages;
  out<<entertainment;
  for(int i=0;i<NDIRS;i++){
   neighbors[i].Write(out);
  }
  out<<gate;
  wanteds->Write(out);
  saleds->Write(out);
  products->Write(out);

  out<<count;
  foreach(this){
   AObject *obj=*(AObject**) iter;
   obj->Write(out);
  }
 }
}
void ARegion::CreateNew(ARegion * prevreg)
{
 if(!prevreg){
  num=0;
  type="nexus";
  xloc=0;
  yloc=0;
  zloc=0;
  name="Unnamed";
  return;
 }
 num=prevreg->num;
 type=prevreg->type;
 xloc=prevreg->xloc;
 yloc=prevreg->yloc;
 zloc=prevreg->zloc;

 name=prevreg->name;
 towntype=prevreg->towntype;
 townname=prevreg->townname;

 taxincome=prevreg->taxincome;
 workincome=prevreg->workincome;
 entincome=prevreg->entincome;
 UpdateFromPrev(prevreg);
}
AnsiString ARegion::FullName()
{
 AnsiString s=type+" (";
 s+=xloc;
 s+=",";
 s+=yloc;
 if(!curRegionList)throw Exception("Error: curRegionList not defined");
 ARegionArray *ra=curRegionList->GetRegionArray(zloc);
 if(!ra)throw Exception("Error: level not found");
 if(ra->name.Length()){
  s+=",";
  s+=ra->name;
 }
 s+=")"+KeyPhrases->Get(keyInSign);
 s+=name;
 if(towntype.Length()){
  s+=KeyPhrases->Get(keyContains);
  s+=townname;
  s+=" ["+towntype+"]";
 }
 return s;
}
bool ARegion::WriteReport(TStream *strm,int mapoptions)
{
 if(!hasInfo) return true; //??
 AnsiString s=FullName();
 if(population){
  s+=", ";
  s+=population;
  s+=KeyPhrases->Get(keyPeasants)+race+")";
  s+=KeyPhrases->Get(keySilvSign)+money;
 }
 s+=".";
 SaveRepString(strm,s);
 SaveRepString(strm,AnsiString::StringOfChar('-',60));
 ReportTab+=2;
 if(mapoptions&roRegionInfo){
  s=WeatherTypes->Get(weather)->curmonthname;
  s+=WeatherTypes->Get(nxtweather)->nextmonthname;

  SaveRepString(strm,s);
  s=KeyPhrases->Get(keyWages);
  s+=wages;
  if(wages){
   s+=KeyPhrases->Get(keyMaxSign);
   s+=maxwages;
   s+=")";
  }
  s+=".";
  SaveRepString(strm,s);

  s=KeyPhrases->Get(keyWantedSign);
  s+=wanteds->Print()+".";
  SaveRepString(strm,s);

  s=KeyPhrases->Get(keySaledSign);
  s+=saleds->Print()+".";
  SaveRepString(strm,s);

  if(entertainment){
   s=KeyPhrases->Get(keyEnteSign);
   s+=entertainment;
   s+=".";
   SaveRepString(strm,s);
  }
  s=KeyPhrases->Get(keyProdSign);
  s+=products->Print(7)+".";
  SaveRepString(strm,s);
 }
 SaveRepString(strm,"");

 ReportTab-=2;
 SaveRepString(strm,KeyPhrases->Get(keyExitSign));
 ReportTab+=2;
 for(int i=0;i<NDIRS;i++){
  ARegion *reg=curRegionList->Get(neighbors[i].xloc,neighbors[i].yloc,neighbors[i].zloc);
  if(!reg)continue;
  s=DirTypes->Get(i)->name+" : ";
  s+=reg->FullName()+".";
  SaveRepString(strm,s);
 }
 ReportTab-=2;
 SaveRepString(strm,"");
 if(mapoptions&roRegionInfo&&gate){
  s=KeyPhrases->Get(keyGatesSign);
  s+=gate;
  s+=KeyPhrases->Get(keyGatesSign2);
  s+=curRegionList->numberofgates;
  s+=").";
  SaveRepString(strm,s);
  SaveRepString(strm,"");
 }
 int obs = 100;
 int truesight = 0;
 bool detfac = false;
 if(!(mapoptions&roLocalAsNonlocalUnit))
// if( S_MIND_READING != -1 )
 {
  obs = GetObservation(curFaction);
  truesight = GetTrueSight(curFaction);
  foreach(this){
   AObject *obj=*(AObject **) iter;
   foreach(obj->units){
    if(detfac) break;
    AUnit *un=*(AUnit**)iter;
    if(un->faction!=curFaction) continue;
    if(un->GetSkill("MIND") > 2){
     detfac = true;
     break;
    }
   }
  }
 }
 foreach(this){
  AObject *obj=*(AObject **) iter;
  if(!obj->WriteReport(strm,obs,truesight,detfac,mapoptions)) return false;
 }
// }
 SaveRepString(strm,"");

 return true;
}
AnsiString ARegion::Print()
{
 return "ERROR";
}
void ARegion::MakeView(VListItems * vlis)
{
 VListItem *vli;
 AnsiString s=FullName();
 int turnnum=turn->num;
 if(!hasInfo||oldinfoturnnum!=turnnum){
  s="?"+s;
 }
 if(hasInfo){
  if(optRunRegionOrders)
   RunOrders();
  if(population){
   s+=", ";
   s+=population;
   s+=" peasants ("+race+")";
   vli=new VListItem(vlis);
   vli->type=litRegHeader;
   vli->value=num;
   vli->text=s;
   s="Tax: $";
   s+=money;
   s+="/";
   int taxinc=taxincome;
   if(taxincome)s+=taxincome;
   else{ s+="auto";taxinc=50;}
   s+=" Taxers: ";
   s+=taxers;
   s+="/";
   s+=money/taxinc;
  }
 }
 vli=new VListItem(vlis);
 vli->type=litRegHeader;
 vli->value=num;
 vli->text=s;

 vli=new VListItem(vlis);
 vli->text="---------------------------------------------";

 s="  ";
 AnsiString weat=WeatherRegions->GetWeather(xloc,yloc,zloc,curTurnNumber);
 AnsiString nxtweat=WeatherRegions->GetWeather(xloc,yloc,zloc,curTurnNumber+1);
 if(/*turn->num&&*/hasInfo&&oldinfoturnnum==turn->num){
  if(weat!=weather) s+="!";
  s+=WeatherTypes->Get(weather)->curmonthname;
  if(nxtweat!=nxtweather) s+="!";
  s+=WeatherTypes->Get(nxtweather)->nextmonthname;
 }else{
  s+=WeatherTypes->Get(weat)->curmonthname;
  s+=WeatherTypes->Get(nxtweat)->nextmonthname;
 }
 vli=new VListItem(vlis);
 vli->type=litRegWeather;
 vli->value=num;
 vli->text=s;
 if(hasInfo){
  s="  Wages: $";
  s+=wages;
  if(wages){
   s+="/";
   int wi=workincome;
   if(wi){ s+=wi;
   }else{ s+="auto"; wi=wages;}
   s+=" (Max: $";
   s+=maxwages;
   s+=")";
   s+=" Workers: ";
   s+=workers;
   s+="/";
   s+=maxwages/wi;
  }
  vli=new VListItem(vlis);
  vli->type=litRegWages;
  vli->value=num;
  vli->text=s;
  s="  Wanted: ";
  s+=wanteds->Print();
  vli=new VListItem(vlis);
  vli->type=litRegWanted;
  vli->value=num;
  vli->text=s;
  s="  For Sale: ";
  s+=saleds->Print();
  s+=".";
  vli=new VListItem(vlis);
  vli->type=litRegSaled;
  vli->value=num;
  vli->text=s;
  s="  Entertainment available: $";
  s+=entertainment;
  s+="/";
  int ei=entincome;
  if(ei){ s+=ei;
  }else{ s+="auto";}
  s+=" Attempted: $";
  s+=entattempt;

  vli=new VListItem(vlis);
  vli->type=litRegEnte;
  vli->value=num;
  vli->text=s;
  s="  Products: ";
  vli=new VListItem(vlis);
  vli->value=-1;
  vli->text=s;

  foreach(products){
   AMarket *mark=*(AMarket**) iter;
   s="    "+mark->Print(3);
   if(mark->price){
    s+=" Attempted: ";
    s+=mark->price;
   }
   vli=new VListItem(vlis);
   vli->type=litRegProd;
   vli->value=num;
   vli->text=s;
  }
 new VListItem(vlis);

  s="";
  s+="Exits:";
  vli=new VListItem(vlis);
  vli->type=litRegExit;
  vli->value=-1;
  vli->text=s;
  for(int i=0;i<NDIRS;i++){
   ARegion *reg=curRegionList->Get(neighbors[i].xloc,neighbors[i].yloc,neighbors[i].zloc);
   if(!reg)continue;
   s="  "+DirTypes->Get(i)->name+" : ";
   s+=reg->FullName()+".";
   vli=new VListItem(vlis);
   vli->type=litRegExit;
   vli->value=i;
   vli->text=s;
  }
 new VListItem(vlis);
  if(gate){
   s="";
   s+=KeyPhrases->Get(keyGatesSign);
   s+=gate;
   s+=KeyPhrases->Get(keyGatesSign2);
   s+=curRegionList->numberofgates;
   s+=").";
   vli=new VListItem(vlis);
   vli->type=litRegGate;
   vli->value=gate;
   vli->text=s;

   new VListItem(vlis);
  }

  foreach(this){
   AObject *obj=*(AObject **) iter;
   if(obj->num==0)continue;
   if(obj->units->count)s="++";
   else s="+ ";
   s+=obj->ObjectHeader(false);
   vli=new VListItem(vlis);
   vli->type=litRegObj;
   vli->value=obj->num;
   vli->text=s;
  }
  new VListItem(vlis);

  AObject *obj=GetDummy();
  obj->MakeView(vlis);
 }
}
void ARegion::MakeTree(TTreeNode * parnode, VTreeNodes * vtns)
{
 AnsiString str=FullName();
 VTreeNode *vtn;
 vtn=new VTreeNode(vtns);
 vtn->type=tntRegion;
 vtn->value=num;
 TTreeNode *node=parnode->Owner->AddChildObject(parnode,str,vtn);
 vtn->node=node;
 foreach(this){
  AObject *obj=*(AObject **) iter;
  obj->MakeTree(node,vtns);
 }
}

void ARegion::Update(ARegion * newreg)
{
 type=newreg->type;
 name=newreg->name;
 towntype=newreg->towntype;
 townname=newreg->townname;
 if(newreg->lastviewfact)
  lastviewfact=newreg->lastviewfact;
 if(inner.xloc==-1)inner=newreg->inner;
 if(!newreg->hasInfo) return;
  hasInfo=true;
  oldinfoturnnum=newreg->oldinfoturnnum;
  population=newreg->population;
  race=newreg->race;
  money=newreg->money;
  weather=newreg->weather;
  nxtweather=newreg->nxtweather;
  wages=newreg->wages;
  maxwages=newreg->maxwages;
  wanteds->Update(newreg->wanteds);
  saleds->Update(newreg->saleds);
  entertainment=newreg->entertainment;
  products->Update(newreg->products);
  for(int i=0;i<NDIRS;i++) neighbors[i]=newreg->neighbors[i];
  if(newreg->gate) gate=newreg->gate;
  if(!hasInfo||oldinfoturnnum!=turn->num){
   Clear();
  }

//verify destroyed
  foreachr(this){
   AObject *obj=*(AObject **) iter;
   if(!obj->num) continue;
   AObject *newobj=newreg->GetObject(obj->num);
   if(!newobj){
    TList::Remove(obj);
    delete obj;
   }
  }
//verify present
  foreach(newreg){
   AObject *newobj=*(AObject **) iter;
   AObject *obj=GetObject(newobj->num);
   if(obj){
    obj->Update(newobj);
   }else{
    obj=new AObject(turn);
    obj->Update(newobj);
    Add(obj);
   }
  }
}
void ARegion::Update0(ARegion * newreg)
{
 type=newreg->type;
 name=newreg->name;
 towntype=newreg->towntype;
 townname=newreg->townname;
 if(inner.xloc==-1)inner=newreg->inner;
 if(!newreg->hasInfo) return;
 hasInfo=true;
 oldinfoturnnum=turn->num;
  population=newreg->population;
  race=newreg->race;
  money=newreg->money;
  weather=newreg->weather;
  nxtweather=newreg->nxtweather;
  wages=newreg->wages;
  maxwages=newreg->maxwages;
  wanteds->Update(newreg->wanteds);
  saleds->Update(newreg->saleds);
  entertainment=newreg->entertainment;
  products->Update(newreg->products);
  for(int i=0;i<NDIRS;i++) neighbors[i]=newreg->neighbors[i];
  if(newreg->gate) gate=newreg->gate;
  Clear();
  CreateDummy();
  foreach(newreg){
   AObject *newobj=*(AObject **) iter;
   if(newobj->num==0) continue;
   AObjectType *ot=ObjectTypes->Get(newobj->type);
   if(ot->skill.Length()) continue;

   AObject *obj=new AObject(turn);
   obj->UpdateFromPrev(newobj);
   Add(obj);
  }
}
void ARegion::UpdateFromPrev(ARegion *prvreg)
{
 if(!prvreg->hasInfo) return;
 if(inner.xloc==-1){
  if(prvreg->RegionStatus->HasShaft) inner=prvreg->inner;
  prvreg->ClearRegionStatus(); //??
  ClearRegionStatus();
 }
 if(prvreg->gate)
  gate=prvreg->gate;
 if(!hasInfo||oldinfoturnnum<=prvreg->oldinfoturnnum){
  hasInfo=true;
  oldinfoturnnum=prvreg->oldinfoturnnum;

  race=prvreg->race;
  population=prvreg->population;
  money=prvreg->money;
  wages=prvreg->wages;
  maxwages=prvreg->maxwages;
  entertainment=prvreg->entertainment;
  for(int i=0;i<NDIRS;i++)neighbors[i]=prvreg->neighbors[i];
  Clear();
  foreach(prvreg){
   AObject *prvobj=*(AObject **)iter;
   if(prvobj->num==0){
    CreateDummy();
    continue;
   }
   AObjectType *ot=ObjectTypes->Get(prvobj->type);
   if(!ot) continue;
   if(ot->sailors&&prvobj->incomplete==0) continue;
   AObject *obj;
   obj=new AObject(turn);
   obj->CreateNew(prvobj);
   Add(obj);
  }
 }
  foreach(prvreg){
   AObject *prvobj=*(AObject**)iter;
   foreach(prvobj->units){
    AUnit *prvun=*(AUnit**)iter;
    if(prvun->IsLocal()) continue;
    AUnit *un=GetUnit(prvun->num);
    if(!un) continue;
//    if(un->IsLocal()) continue;
    un->UpdateFromPrev(prvun);
   }
  }
  wanteds->UpdateFromPrev(prvreg->wanteds);
  saleds->UpdateFromPrev(prvreg->saleds);
  products->UpdateFromPrev(prvreg->products);
}
int ARegion::GetDistance(int x, int y, int z)
{
 if( zloc != z )
 {
  return( 10000000 );
 }

 ARegionArray *pArr = curRegionList->GetRegionArray(z);
 int maxy;
 maxy = yloc - y;
 if (maxy < 0) maxy = -maxy;

 int maxx = xloc - x;
 if (maxx < 0) maxx = -maxx;

 int max2 = xloc + pArr->x - x;
 if (max2 < 0) max2 = -max2;
 if (max2 < maxx) maxx = max2;

 max2 = xloc - (x + pArr->x);
 if (max2 < 0) max2 = -max2;
 if (max2 < maxx) maxx = max2;

 if (maxy > maxx) return (maxx + maxy) / 2;
 return maxx;
}

ARegionArray*curRegionArray;
ARegionArray::ARegionArray()
 :x(1),y(1),levelnum(-1)
{
}
ARegionArray::~ARegionArray()
{
 if(curRegionArray==this)curRegionArray=0;
}
ARegion* ARegionArray::Get(int x, int y)
{
 return curRegionList->Get(x,y,levelnum);
}
void ARegionArray::Read(TFile& in)
{
 in>>levelnum>>name;
 in>>x>>y;
}
void ARegionArray::Write(TFile& out)
{
 out<<levelnum<<name;
 out<<x<<y;
}
void ARegionArray::CreateNew(ARegionArray* prevra)
{
 if(!prevra)throw Exception("Can't create RegionArray from NULL");
 levelnum=prevra->levelnum;
 name=prevra->name;
 x=prevra->x;
 y=prevra->y;
}
void ARegionArray::NormCoord(int & xloc, int & yloc)
{
 int xx=x;
 if(xx&1) xx++;
 while(xloc<0) xloc+=xx;
 while(xloc>=xx) xloc-=xx;

 if(yloc<0) yloc=0;
 if((xloc+yloc)&1) yloc++;
 if(yloc>=y) yloc=y-1;
 if((xloc+yloc)&1) yloc--;
 if(yloc<0){ yloc++;xloc--; }
}
void ARegionArray::NormCoord2(int & xloc, int & yloc)
{
 if(xloc<=-x) xloc=-x+1;
 if(xloc>=2*x-1) xloc=2*x-2;
 if(yloc<0) yloc=0;
 if(yloc>=y) yloc=y-1;
}

void ARegionArray::UpdateFromPrev(ARegionArray * prvar)
{
 if(x<prvar->x) x=prvar->x;
 if(y<prvar->y) y=prvar->y;
}
void ARegionArray::VerifySize(int xloc, int yloc)
{
 if(x<=xloc) x=xloc+1;
 if(y<=yloc) y=yloc+1;
}

ARegionArrays::ARegionArrays()
{
}
__fastcall ARegionArrays::~ARegionArrays()
{
 Clear();
}
void __fastcall ARegionArrays::Clear()
{
 int i=Count-1;
 if(i<0) return;
 ARegionArray **iter=(ARegionArray**)List+i;
 for(;i>=0;i--,iter--){
  delete *iter;
//  Delete(i);
 }
 TList::Clear();
}
void ARegionArrays::Read(TFile& in)
{
 Clear();
 int size;
 in>>size;
 for(int i=0;i<size;i++){
  ARegionArray *ar=new ARegionArray;
  ar->Read(in);
  Add(ar);
 }
}
void ARegionArrays::Write(TFile& out)
{
 out<<count;
 foreach(this){
  ARegionArray*ar=*(ARegionArray**) iter;
  ar->Write(out);
 }
}
void ARegionArrays::Add(ARegionArray* ar)
{
 if(ar->levelnum==-1)ar->levelnum=count;
 if(ar->levelnum!=count)throw Exception("Error ARegionArrays::Add(): number not match");
 TList::Add(ar);
}
ARegionArray* ARegionArrays::Get(int levelnum)
{
 return (ARegionArray*)Items[levelnum];
}
ARegionArray* ARegionArrays::Get(AnsiString levelname)
{
 foreach(this){
  ARegionArray *ra=*(ARegionArray**)iter;
  if(ra->name==levelname)return ra;
 }
 return 0;
}
int ARegionArrays::GetLevelNum(AnsiString levelname)
{
 ARegionArray *ra=Get(levelname);
 if(!ra)return -1;
 return ra->levelnum;
}
void ARegionArrays::CreateNew(ARegionArrays * prevras)
{
 Clear();
 if(!prevras){
  ARegionArray *ra;
  ra=new ARegionArray;
  ra->levelnum=0;
  ra->name="nexus";
  Add(ra);
  ra=new ARegionArray;
  ra->levelnum=1;
  Add(ra);
  return;
 }
 foreach(prevras){
  ARegionArray*prevra=*(ARegionArray**)iter;
  ARegionArray *ra=new ARegionArray;
  ra->CreateNew(prevra);
  Add(ra);
 }
}
void ARegionArrays::UpdateFromPrev(ARegionArrays * prvars)
{
 foreach(prvars){
  ARegionArray *prvar=*(ARegionArray**)iter;
  ARegionArray *ar=Get(prvar->name);
  if(!ar){
   ar=new ARegionArray;
   ar->CreateNew(prvar);
   Add(ar);
   continue;
  }
  ar->UpdateFromPrev(prvar);
 }
}
ARegionList*curRegionList;
ARegionList::ARegionList(ATurn *t)
 :TList(),numberofgates(0),turn(t),RegionArrays(new ARegionArrays)
{
}
__fastcall ARegionList::~ARegionList()
{
 Clear();
 delete RegionArrays;
 if(curRegionList==this)curRegionList=0;
}
void __fastcall ARegionList::Clear()
{
 int i=Count-1;
 if(i<0) return;
 ARegion **iter=(ARegion**)List+i;
 for(;i>=0;i--,iter--){
  delete *iter;
//  Delete(i);
 }
 TList::Clear();
}
void ARegionList::Read(TFile& in)
{
 Clear();
 in>>numberofgates;
 RegionArrays->Read(in);
 int size;
 in>>size;
 for(int i=0;i<size;i++){
  ARegion *reg=new ARegion(turn);
//  reg->turn=turn;
  reg->Read(in);
  Add(reg);
 }
}
void ARegionList::Write(TFile& out)
{
 out<<numberofgates;
 RegionArrays->Write(out);
 out<<count;
 foreach(this){
  ARegion*reg=*(ARegion**)iter;
  reg->Write(out);
 }
}
void ARegionList::Add(ARegion * reg)
{
 if(reg->num==-1)reg->num=count;
 if(reg->num!=count)throw Exception("Error ARegionList::Add(): number not match");
 TList::Add(reg);
}
ARegion* ARegionList::Get(int regnum)
{
 return (ARegion*)Items[regnum];
}
ARegion* ARegionList::Get(int x, int y, int z)
{
 foreach(this){
  ARegion *reg=*(ARegion**)iter;
  if(reg->xloc==x&&reg->yloc==y&&reg->zloc==z)return reg;
 }
 return 0;
}
ARegion * ARegionList::Get(ALocation & loc)
{
 if(loc.xloc==-1) return 0;
 foreach(this){
  ARegion *reg=*(ARegion**)iter;
  if(reg->xloc==loc.xloc&&reg->yloc==loc.yloc&&reg->zloc==loc.zloc)return reg;
 }
 return 0;
}

ARegion * ARegionList::GetNearestRegion(int x, int y, int z)
{
 int nearnum=-1,neardist=1000000,cur;
 foreach(this){
  ARegion *reg=*(ARegion**)iter;
  if(reg->zloc!=z)continue;
  cur=reg->GetDistance(x,y,z);
//if(cur==0) 
  if(cur<neardist){neardist=cur;nearnum=reg->num;}
 }
 if(nearnum==-1)return 0;
 return Get(nearnum);
}

ARegionArray* ARegionList::GetRegionArray(int level)
{
 return RegionArrays->Get(level);
}
bool ARegionList::SetCurRegionArray(int levelnum)
{
 if(curRegionList!=this)return false;
 ARegionArray *ra=GetRegionArray(levelnum);
 if(!ra)return false;
 curRegionArray=ra;
 return true;
}
bool ARegionList::SetCurRegion(int x, int y, int z)
{
 if(!SetCurRegionArray(z))return false;
 ARegion *reg=Get(x,y,z);
 if(!reg)return false;
 curRegion=reg;
 return true;
}
void ARegionList::CreateNew(ARegionList* prevrl)
{
 Clear();
 if(!prevrl){
  numberofgates=0;
  RegionArrays->CreateNew(0);
  ARegion *reg=new ARegion(turn);
  reg->turn=turn;
  reg->CreateNew(0);
  Add(reg);
  return;
 }
 numberofgates=prevrl->numberofgates;
 RegionArrays->CreateNew(prevrl->RegionArrays);
 foreach(prevrl){
  ARegion*prevreg=*(ARegion**)iter;
  ARegion*reg=new ARegion(turn);
  reg->turn=turn;
  reg->CreateNew(prevreg);
  Add(reg);
 }
}
void ARegionList::UpdateFromPrev(ARegionList * prvrl)
{
//static int number=0;
 if(numberofgates<prvrl->numberofgates)
  numberofgates=prvrl->numberofgates;
 RegionArrays->UpdateFromPrev(prvrl->RegionArrays);
 foreach(prvrl){
  ARegion *prvreg=*(ARegion**)iter;
  ARegion *reg=Get(prvreg->xloc,prvreg->yloc,prvreg->zloc);
  if(!reg){
   reg=new ARegion(turn);
   reg->turn=turn;
   reg->CreateNew(prvreg);
   reg->num=-1;
   Add(reg);
   continue;
  }
  reg->UpdateFromPrev(prvreg);
 }
}
void ARegion::CreateDummy()
{
 AObject *obj=new AObject(turn);
 obj->num=0;
 Add(obj);
}
AObject * ARegion::GetDummy()
{
 AObject *obj=Get(0);
 if(obj->num==0)return obj;
 return GetObject(0);
}
AObject * ARegion::GetInner()
{
 foreach(this){
  AObject *obj=*(AObject **) iter;
  if(obj->inner)return obj;
 }
 return 0;
}

AObject *curObject;
AObject::AObject(ATurn *t)
 :num(-1),incomplete(0),runes(0),basereg(0),inner(false),
  turn(t),endreg(0)
{
 units=new AUnits(this);
}
__fastcall AObject::~AObject()
{
 Clear();
 if(curObject==this)curObject=0;
 delete units;units=0;
}
void __fastcall AObject::Clear()
{
 if(units){delete units; units=0;}
}
AUnit * AObject::GetUnit(int num)
{
 return units->GetUnit(num);
}

AnsiString AObject::FullName()
{
 AnsiString s;
 s=name;
 s+=" [";
 s+=num;
 s+=KeyPhrases->Get(keyObjNameEndSign);
 s+=type;
 return s;
}
AnsiString AObject::ObjectHeader(bool isreport)
{
 if(num==0)return "";
 AnsiString s,desc2;
// s="+ ";
 s+=FullName();
 if(incomplete){
  s+=KeyPhrases->Get(keyObjNeedSign);
  if(incomplete==-1)s+="??";
  else{
   s+=incomplete;
   if(!isreport&&endincomplete!=incomplete){
    s+=" (";
    s+=endincomplete;
    s+=")";
   }
  }
 }
 if(inner){
  s+=KeyPhrases->Get(keyObjContainsSign);
  if(!isreport){
   if(basereg->inner.xloc==-1){
    s+=" (not linked)";
   }else{
    s+=" (";
    ARegion *reg=curRegionList->Get(basereg->inner);
    s+=reg->FullName()+")";
   }
  }else{
   if(basereg->inner.xloc!=-1){
    ARegion *reg=curRegionList->Get(basereg->inner);
    desc2=";!"+reg->FullName();
   }
  }
 }
 if(runes){
  s+=KeyPhrases->Get(keyObjEngravedSign);
 }
 if(describe.Length()){
  s+="; ";
  s+=describe;
 }else if(desc2.Length()){
  s+="; ";
  s+=desc2;
 }
 AObjectType *ot=ObjectTypes->Get(type);
 if(ot&&ot->canenter==0){
  s+=KeyPhrases->Get(keyObjClosedSign);
 }
// s+=".";
 return s;
}
bool AObject::WriteReport(TStream *strm,int obs,int truesight,bool detfac,int mapoptions)
{
 AnsiString s;
 if(num){
  bool isuser=Destroyable();
  if(isuser){
   if(!(mapoptions&roUserObject))
    return true;
  }else{
   if(!(mapoptions&roNonuserObject))
    return true;
  }
  s="+ "+ObjectHeader(true)+".";
  SaveRepString(strm,s);
 }
 if(!(mapoptions&(roLocalUnit|roLocalAsNonlocalUnit|roNonlocalUnit))){
  SaveRepString(strm,"");
  return true;
 }

 if(num!=0) ReportTab+=2;
 foreach(units){
  AUnit *un=*(AUnit**)iter;
  if(un->faction->local){
   if(mapoptions&roLocalUnit){
    if(un->faction==curFaction){
     if(!un->WriteReport(strm,-1,1,1,true)) return false;
     continue;
    }
   }else{
    if(!(mapoptions&roLocalAsNonlocalUnit))
     continue;
   }
  }else{
   if(!(mapoptions&roNonlocalUnit))
    continue;
  }
  if(!un->WriteReport(strm,obs,truesight,detfac,num!=0)) return false;
 }
 SaveRepString(strm,"");
 if(num!=0) ReportTab-=2;
 return true;
}
AnsiString AObject::Print()
{
 AnsiString s;
/* if(num)s="+ "+ObjectHeader(false)+"\r\n";
 if(num==0)return s;
 AObjectType *ot=ObjectTypes->Get(type);
 if(!ot||ot->canenter)return s;
 AnsiString tab="  ";
 foreach(units){
  AUnit *un=*(AUnit**)iter;
  s+=tab+un->Print()+"\r\n";
 }
 s+="\r\n";*/
 return s;
}
void AObject::MakeView(VListItems * vlis)
{
 AnsiString s;
 VListItem *vli;
 if(num){
  if(units->count)s="++";
  else s="+ ";
  s+=ObjectHeader(false);
  vli=new VListItem(vlis);
  vli->type=litObjHeader;
  vli->value=num;
  vli->text=s;

  int protect=Protect();
  if(protect){
   int protect2=0;
   foreach(units){
    AUnit *un=*(AUnit**)iter;
    protect2+=un->GetMen();
   }
   s="Protected: ";
   s+=protect2;
   s+="/";
   s+=protect;
   vli=new VListItem(vlis);
   vli->type=litObjHeader;
   vli->value=num;
   vli->text=s;
  }
  int capacity=Capacity();
  if(capacity){
   int cap2=0;
   foreach(units){
    AUnit *un=*(AUnit**)iter;
    cap2+=un->weight;
   }
   s="Capacity: ";
   s+=cap2;
   s+="/";
   s+=capacity;
   vli=new VListItem(vlis);
   vli->type=litObjHeader;
   vli->value=num;
   vli->text=s;
  }
  int sailors=Sailors();
  if(sailors){
   s="Sailors: ";
   s+=cursailors;
   s+="/";
   s+=sailors;
   vli=new VListItem(vlis);
   vli->type=litObjHeader;
   vli->value=num;
   vli->text=s;
  }
 }
 AnsiString tab;
 if(num)tab="  ";
 foreach(units){
  AUnit *un=*(AUnit**)iter;
  s=tab+un->Print();
  vli=new VListItem(vlis);
  vli->type=litObjUnit;
  vli->value=un->num;
  vli->text=s;
 }
}

void AObject::MakeTree(TTreeNode * parnode, VTreeNodes * vtns)
{
 AnsiString str;
 VTreeNode *vtn;
 TTreeNode *node;
 if(num!=0){
  str=ObjectHeader(false);
  vtn=new VTreeNode(vtns);
  vtn->type=tntObject;
  vtn->value=num;
  node=parnode->Owner->AddChildObject(parnode,str,vtn);
  vtn->node=node;
 }else{
  node=parnode;
 }
 foreach(units){
  AUnit *unit=*(AUnit**)iter;
  unit->MakeTree(node,vtns);
 }
}

void AObject::CreateNew(AObject * prvobj)
{
 if(!prvobj){
  return;
 }
 UpdateFromPrev(prvobj);
}
void AObject::Update(AObject * newobj)
{
 name=newobj->name;
 describe=newobj->describe;
 inner=newobj->inner;
 num=newobj->num;
 type=newobj->type;
 incomplete=newobj->incomplete;
 runes=newobj->runes;
 units->Update(newobj->units);
}
void AObject::UpdateFromPrev(AObject * prvobj)
{
 name=prvobj->name;
 describe=prvobj->describe;
 inner=prvobj->inner;
 num=prvobj->num;
 type=prvobj->type;
 if(prvobj->incomplete)incomplete=-1;
 else incomplete=0;
 runes=prvobj->runes;
}
void AObject::Read(TFile & in)
{
 in>>num;
 if(num){
  in>>name;
  in>>describe;
  in>>inner;
  in>>type;
  in>>incomplete;
  in>>runes;
 }
 units->Read(in);
}
void AObject::Write(TFile & out)
{
 out<<num;
 if(num){
  out<<name;
/*  int dlen=describe.Length();
  if(dlen){
   if(describe[dlen]=='.')describe.SetLength(dlen-1);
  }*/
  out<<describe;
  out<<inner;
  out<<type;
  out<<incomplete;
  out<<runes;
 }
 units->Write(out);
}


bool AObject::IsTower()
{
 if(num==0) return false;
 AObjectType *otype=ObjectTypes->Get(type);
 if(otype->protect) return true;
/* if(!otype->canenter) return false;
 if(otype->sailors) return false;
 if(!otype->production.IsEmpty()) return false;*/
 return false;
}
bool AObject::IsBuilding()
{
 if(num==0) return false;
 AObjectType *otype=ObjectTypes->Get(type);
 if(otype->protect) return false;
 if(!otype->canenter) return false;
 if(otype->sailors) return false;
 if(inner) return false;
 return true;
}
bool AObject::IsLair()
{
 if(num==0) return false;
 AObjectType *otype=ObjectTypes->Get(type);
 return !otype->canenter;
}
bool AObject::IsShip()
{
 if(num==0) return false;
 AObjectType *otype=ObjectTypes->Get(type);
 return otype->sailors;
}
bool AObject::Destroyable()
{
 if(num==0) return false;
 AObjectType *otype=ObjectTypes->Get(type);
 return otype->cost!=-1;
}


void AObject::AddUnit(AUnit* unit)
{
 units->Add(unit);
}
AUnit * ARegionList::GetUnit(int num)
{
 foreach(this){
  ARegion *reg=*(ARegion**)iter;
  AUnit *un=reg->GetUnit(num);
  if(un)return un;
 }
 return 0;
}

int __fastcall RegionListCompare(void * Item1, void * Item2){
 ARegion *reg1=(ARegion*)Item1,*reg2=(ARegion*)Item2;
 if(reg1->zloc<reg2->zloc)return -1;
 if(reg1->zloc>reg2->zloc)return 1;
 if(reg1->yloc<reg2->yloc)return -1;
 if(reg1->yloc>reg2->yloc)return 1;
 if(reg1->xloc<reg2->xloc)return -1;
 if(reg1->xloc>reg2->xloc)return 1;
 return 0;
}
void ARegionList::Sort()
{
 TList::Sort(RegionListCompare);
 for(int i=count-1;i>=0;i--){
  Get(i)->num=i;
 }
}
bool AObject::PresentLocals()
{
 foreach(units){
  AUnit *un=*(AUnit**)iter;
  bool isloc=un->IsLocal();
  if(isloc)return true;
 }
 return false;
}
bool ARegion::PresentLocals()
{
 foreach(this){
  AObject *obj=*(AObject **) iter;
  bool presloc=obj->PresentLocals();
  if(presloc)return true;
 }
 return false;
}
bool ARegion::CanTax(AUnit * u)
{
 foreach(this){
  AObject *obj=*(AObject **) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un->endguard==GUARD_GUARD && un->IsAlive())
    if(un->GetAttitude(this,u)<=A_NEUTRAL)
//    if(u->GetAttitude(this,un)<=A_NEUTRAL) //временно
     return false;
  }
 }
 return true;
}
void ARegion::RunOrders(RunStop rs)
{
//begin order processing
 PrepareOrders();
 PrepareOrders2();
 RunFlagOrders();
 RunEnterLeaveOrders();
// RunPromoteOrders();

// RunStealOrders();
 if(!RunGiveOrders())return;
 if(rs==rsAfterGive) return;
// RunDestroyOrders(); //verify
 if(!RunPillageOrders())
   RunTaxOrders();
 RunGuard1Orders();
//  RunCastOrders(); //verify
 if(!RunSellOrders())return;
 if(!RunBuyOrders())return;
 if(!RunForgetOrders())return;
 PrepareConsume();
 RunSailOrders();
 RunMoveOrders();
 if(!RunTeachOrders())return;
 RunStudyOrders();
 RunWorkOrders();
 RunEntertainOrders();
 RunProduceOrdersA();
 RunProduceOrders();
 if(rs==rsBeforeBuild) return;
 RunBuildOrders();


// RunTeleportOrders();
 RunConsumeOrders();
//end order processing
 foreach(this){
  AObject *obj=*(AObject **) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   un->enditems->DeleteEmpty();
   un->endskills->DeleteEmpty();
   un->UpdateWeight();
  }
 }
}
bool ARegion::RunPillageOrders()
{
 AnsiString mes;
 int t=0;
 AOrders *ords=new AOrders(0);
 ords->autodelete=false;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrder *ord=u->orders->Find(O_PILLAGE,2);
   if(!ord)continue;
   if(money<1){
    mes="Error: PILLAGE: ";
    mes+=u->FullName();
    mes+=" no money";
    AddEvent(mes);
    return false;
   }
   if(wages<11){
    mes="Error: PILLAGE: ";
    mes+=u->FullName();
    mes+=" poor wages";
    AddEvent(mes);
    return false;
   }
   if(!CanTax(u)){
    AddEvent("Error: "+u->FullName()+" PILLAGE: A unit is on guard.");
   }else{
    int men=u->Taxers();
    if(men){
     t+=men;
     ords->Add(ord);
    }else{
     mes="Error: PILLAGE: ";
     mes+=u->FullName();
     mes+=" cannot tax";
     AddEvent(mes);
    }
   }
  }
 }
 if(!t){ delete ords;return false;}
 if(t<money/100){
  mes=ords->Get(0)->orders->unit->FullName();
  mes+=" PILLAGE: Not enough men to pillage (";
  mes+=t;
  mes+="/";
  mes+=money/100;
  mes+=")";
  AddEvent(mes);
  delete ords;
  return false;
 }
 ords->Distribute(2*money,t*100);
 delete ords;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrder *ord=u->orders->Find(O_TAX,2);
   if(!ord)continue;
   mes="Error: TAX: ";
   mes+=u->FullName();
   mes+=" region pillaged";
   AddEvent(mes);
  }
 }
 return true;
}

void ARegion::RunTaxOrders()
{
 AnsiString mes;
 int t=0;
 AOrders *ords=new AOrders(0);
 ords->autodelete=false;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrder *ord=u->orders->Find(O_TAX,2);
   if(!ord)continue;
   if(!CanTax(u)){
    if(u->IsLocal()) AddEvent("Error: "+u->FullName()+" TAX: A unit is on guard.");
   }else{
    int men=u->Taxers();
    if(men){
     t+=men;
     ords->Add(ord);
    }else{
     if(u->GetLocalDescr()!="empty"){
      mes="Error: TAX: ";
      mes+=u->FullName();
      mes+=" cannot tax.";
      AddEvent(mes);
     }
    }
   }
  }
 }
 taxers=t;
 if(!taxers){ delete ords;return;}
 int curincome=taxincome;
 if(!curincome) curincome=50;
 curtaxprod=curincome;
 {
  int inc=money/taxers;
  if(inc<curincome){
   mes="Warnign: tax income = ";
   mes+=inc;
   mes+=" in "+FullName();
   AddEvent(mes);
  }
 }
 ords->Distribute(money,taxers*curincome);
 delete ords;
}
bool ARegion::HasLocalTaxers()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   if(!u->IsLocal())continue;
   if(u->HasTaxOrder())return true;
  }
 }
 return false;
}


void ARegion::RunStudyOrders()
{
 AnsiString mes;

 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   if(!u->IsLocal())continue;
   AOrderAbr *ord=dynamic_cast<AOrderAbr*>(u->orders->Find(O_STUDY,2));
   if(!ord)continue;
   ord->Run(0);
  }
 }
}


bool ARegion::RunGiveOrders()
{
 AnsiString mes;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   if(!u->IsLocal())continue;
   foreach(u->orders){
    AOrder *o=*(AOrder**)iter;
    if(o->type!=O_GIVE)continue;
    if(o->isedited)return false;
/*    if(u->endfaction!=u->faction){
     mes="Error
     break;
    }*/
    AOrderGive *ord=dynamic_cast<AOrderGive*>(o);
//    if(!ord)continue;
    ord->Run(0);
   }
  }
 }
 return true;
}


void ARegion::PrepareOrders()
{
 AnsiString mes;
// hasLocal=false;
// hasLocalTaxers=false;
/* if(!hasInfo){  //not needed
// if(!(infoStatus&isWeatherInfo)){*/
 {
  int turnnum=turn->num;
  if(!weather.Length())
   weather=WeatherRegions->GetWeather(xloc,yloc,zloc,turnnum);
  if(!nxtweather.Length())
   nxtweather=WeatherRegions->GetWeather(xloc,yloc,zloc,turnnum+1);
 }
 foreach(this){
  AObject *o=*(AObject **) iter;
  o->endreg=o->basereg;
  o->endincomplete=o->incomplete;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   u->endobj=u->baseobj;
   u->enditems->CreateNew(u->items);
   u->endskills->CreateNew(u->skills);

   u->endflags=u->flags;
   u->endguard=u->guard;
   u->endreveal=u->reveal;
   u->endfaction=u->faction;
   foreachr(u->orders){
    AOrder *o=*(AOrder**)iter;
    if(o->type!=O_MESSAGE)continue;
    u->orders->Delete(o);
   }
//   if(u->IsLocal()) hasLocal=true;
  }
 }
}
void ARegion::PrepareOrders2()
{
 AnsiString mes;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   u->HasTaxOrder();
   if(!u->IsLocal())continue;
   AOrder *monthlong=0;
   bool warned=false;
   foreach(u->orders){
    AOrder *ord=*(AOrder**)iter;
    if(ord->commented) continue;
    if(ord->IsMonthLong()){
     if(monthlong){
      if(ord->type==O_TEACH&&monthlong->type==O_TEACH){
//      }else if(o->type==O_MOVE&&monthlong->type==O_MOVE){
//      }else if(o->type==O_ADVANCE&&monthlong->type==O_ADVANCE){
//      }else if(o->type==O_SAIL&&monthlong->type==O_SAIL){
      }else if(ord->type==O_ATTACK&&monthlong->type==O_ATTACK){
      }else{
       if(!warned){
        mes="Error: to many monthlong orders for ";
        mes+=u->FullName();
        AddEvent(mes);
        warned=true;
       }
       monthlong->commented=true;
      }
     }
     monthlong=ord;
    }
    ord->Prepare();
/*    if(o->type==O_STUDY){
     AOrderAbr *ord=dynamic_cast<AOrderAbr*>(o);
     ord->number=30;
    }
//    if(o->type!=O_GIVE)continue;
//    if(o->isedited)continue;
    //throw Exception("Bad give orders");
    AOrderGive *ord=dynamic_cast<AOrderGive*>(o);
//    if(!ord)continue;
    if(!ord->target){
     if(ord->tarnum)ord->target=GetUnit(ord->tarnum);
     if(ord->commented)continue;
     if(!ord->target){
      mes="Error ";
      mes+=u->FullName();
      mes+=" GIVE: unit ";
      AUnit *un=new AUnit(0);
      un->num=ord->tarnum;
      mes+=un->Alias();
      delete un;
      mes+=" not found";
      AddEvent(mes);
      continue;
     }
    }
    if(ord->ordrec){
     if(ord->commented){
      ord->DeleteOrdRec();
     }
     continue;
    }
    if(!ord->commented)
       ord->SetMessage();

//     ord->target->orders->Add(new AOrderReceived(ord));*/
   }
   if(!monthlong){
    if(!(optDontShowEvents&dseNoMonth)&&
       u->GetLocalDescr()!="empty"){
     mes="Error: no monthlong orders by ";
     mes+=u->FullName();
     AddEvent(mes);
    }
   }
  }
 }
}


bool ARegion::RunSellOrders()
{
 bool notedited=true;
 foreach(wanteds){
  AMarket *mark=*(AMarket**)iter;
  if(!RunBuySellOrders(O_SELL,mark))
   notedited=false;
 }
 return notedited;
}
bool ARegion::RunBuyOrders()
{
 bool notedited=true;
 foreach(saleds){
  AMarket *mark=*(AMarket**)iter;
  if(!RunBuySellOrders(O_BUY,mark))
   notedited=false;
 }
 return notedited;
}
bool ARegion::RunBuySellOrders(int type,AMarket *mark)
{
 AnsiString mes;
 int attempted=0;
 AOrders *ords=new AOrders(0);
 ords->autodelete=false;
 AnsiString item=mark->type;
 int price=mark->price;
 bool notedited=true;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   foreach(u->orders){
    AOrder *_ord=*(AOrder**)iter;
    if(_ord->type!=type)continue;
    if(_ord->isedited){
     notedited=false;
     continue;
    }
    if(_ord->commented)continue;

    AOrderBuySell *ord=dynamic_cast<AOrderBuySell*>(_ord);
    if(ord->item!=item)continue;
    int mod=ord->amount;
    if(type==O_BUY){
     int money=u->GetMoney();
     if(price*mod>money){
      mes="Warning: ";
      mes+=u->FullName();
      mes+=" attempt buy to many items : needed $";
      mes+=mod*price-money;
      AddEvent(mes);
      mod=money/price;
     }
    }else{
     int kolvo=u->enditems->GetNum(item);
     if(kolvo<mod){
      mes="Warning: ";
      mes+=u->FullName();
      mes+=" attempt sell to many items : needed ";
      mes+=mod-kolvo;
      mes+=" ";
      mes+=ItemTypes->AbrToName(item);
      AddEvent(mes);
      mod=kolvo;
     }
    }
    ord->number=mod;
    attempted+=mod;
    ords->Add(ord);
   }
  }
 }
 if(!attempted){ delete ords;return notedited;}
 if(attempted>mark->amount){
  if(type==O_BUY)
   mes="Warning: attempt buy to many ";
  else
   mes="Warning: attempt sell to many ";
  mes+=ItemTypes->AbrToNames(mark->type);
  mes+=" in ";
  mes+=FullName();
  mes+=" (";
  mes+=attempted;
  mes+="/";
  mes+=mark->amount;
  mes+=")";
  AddEvent(mes);
 }
// bool b=
 ords->Distribute(mark->amount,attempted);
 delete ords;
 return notedited;
}
void ARegion::RunEntertainOrders()
{
 AnsiString mes;
 int t=0;
 AOrders *ords=new AOrders(0);
 ords->autodelete=false;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrder *ord=u->orders->Find(O_ENTERTAIN,2);
   if(!ord)continue;
   int prod=ord->Productivity();
   if(prod){
    t+=prod;
    ords->Add(ord);
   }
  }
 }
 entattempt=t;
 if(!t){ delete ords;return;}
 if(entertainment<t){
  mes="Warnign: entertain ";
  mes+=t;
  mes+="/";
  mes+=entertainment;
  mes+=" in "+FullName();
  AddEvent(mes);
 }
 ords->Distribute(entertainment,t);
 delete ords;
}
void ARegion::RunWorkOrders()
{
 AnsiString mes;
 int t=0;
 AOrders *ords=new AOrders(0);
 ords->autodelete=false;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrder *ord=u->orders->Find(O_WORK,2);
   if(!ord)continue;
   int prod=ord->Productivity();
   if(prod){
    t+=prod;
    ords->Add(ord);
   }
  }
 }
 workers=0;
 if(!t){ delete ords;return;}
 int wi=workincome;
 if(wi==0) wi=wages;
 workers=t/wi;
 if(maxwages<t){
  mes="Warnign: work ";
  mes+=maxwages*wi/t;
  mes+="/";
  mes+=wages;
  mes+=" in "+FullName();
  AddEvent(mes);
 }
 ords->Distribute(maxwages,t);
 delete ords;
}
void ARegion::RunFlagOrders()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   foreach(u->orders){
    AOrder *_ord=*(AOrder**)iter;
    if(_ord->type==O_CLAIM){
     AOrderInt *ord=dynamic_cast<AOrderInt*>(_ord);
     u->SetMoney(u->GetMoney()+ord->value);
    }else{
     AOrderByte *ord=dynamic_cast<AOrderByte *>(_ord);
     if(!ord) continue;
//     if(ord->type==O_GUARD&&ord->value)continue;
     ord->Run(0);
    }
   }
  }
 }
}
void ARegion::PrepareConsume()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  o->cursailors=0;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   u->HasTaxOrder();
   if(!u->IsLocal())continue;
   if(u->enditems->GetNum("LEAD")){
    u->needed=20;
   }else u->needed=10;
   u->needed*=u->GetMen();
   u->UpdateWeight();
   u->movepoints=u->CalcMovePoints();
  }
 }
}
void ARegion::CheckUnitMaintenanceItem(AUnits *units,AnsiString item, int value, bool consume)
{
 foreach(units){
  AUnit *u=*(AUnit**)iter;
  if(!u->needed)continue;
  if(consume&&!u->GetEndFlag(FLAG_CONSUMING))continue;
  int amount=u->enditems->GetNum(item);
  if(amount){
   u->needed-=amount*value;
   if(u->needed<0){
    u->enditems->SetNum(item,-(u->needed/value));
    u->needed=0;
   }else{
    u->enditems->SetNum(item,0);
   }
  }
 }
}
int ARegion::CheckFactionMaintenanceItem(AUnits *units,AnsiString item, int value, bool consume)
{
 AnsiString namein,nameout,s,itemname=ItemTypes->AbrToName(item),itemnames=ItemTypes->AbrToNames(item);
 int borrowed=0,num;
 AOrderMessage *mes;

 foreach(units){
  AUnit *u=*(AUnit**)iter;
  if(!u->needed)continue;
  if(u->endobj->endreg!=this) continue;
  if(consume&&!u->GetEndFlag(FLAG_CONSUMING_FACTION))continue;
  namein=u->FullName();
  foreach(units){
   AUnit *u2=*(AUnit**)iter;
   if(u==u2||u->endfaction!=u2->endfaction)continue;
   if(u2->endobj->endreg!=this) continue;
   int amount=u2->enditems->GetNum(item);
   if(amount){
    nameout=u2->FullName();
    if(borrowed==0) borrowed=u->num;
    u->needed-=amount*value;
    num=amount;
    if(u->needed<0){
     num+=(u->needed/value);
     u2->enditems->SetNum(item,-(u->needed/value));
     u->needed=0;
    }else{
     u2->enditems->SetNum(item,0);
    }
    mes=new AOrderMessage(0);
    s=namein+" borrows ";
    if(num!=1){
     s+=num;
     s+=" "+itemnames;
    }else{
     s+=itemname;
    }
    s+=" for maintenance";
    mes->text=s;
    mes->target=u;
    u2->orders->Add(mes);

    mes=new AOrderMessage(0);
    s="Borrows ";
    if(num!=1){
     s+=num;
     s+=" "+itemnames;
    }else{
     s+=itemname;
    }
    s+=" from "+nameout;
    s+=" for maintenance";
    mes->text=s;
    mes->target=u2;
    u->orders->Add(mes);
   }
   if(u->needed==0)break;
  }
 }
 return borrowed;
}

void ARegion::RunConsumeOrders()
{
 AnsiString mes;
 AUnits *uns=new AUnits(0);
 uns->autodelete=false;

 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   if(!u->IsLocal())continue;
   int mens=u->GetMen();
   if(!mens){
    if(u->GetLocalDescr()!="empty"){
     mes="Warning: Unit ";
     mes+=u->FullName();
     mes+=" is empty";
     AddEvent(mes);
    }
   }
   uns->Add(u);
  }                 
 }
 foreach(ItemTypes){
  AItemType *itype=*(AItemType**)iter;
  if(!(itype->type&IT_FOOD))continue;
  CheckUnitMaintenanceItem(uns,itype->abr,10,true);
 }
 int borrowed=0,bor2;
 foreach(ItemTypes){
  AItemType *itype=*(AItemType**)iter;
  if(!(itype->type&IT_FOOD))continue;
  bor2=CheckFactionMaintenanceItem(uns,itype->abr,10,true);
  if(borrowed==0)borrowed=bor2;
 }

 CheckUnitMaintenanceItem(uns,"SILV",1,false);
 bor2=CheckFactionMaintenanceItem(uns,"SILV",1,false);
 if(borrowed==0) borrowed=bor2;

 foreach(ItemTypes){
  AItemType *itype=*(AItemType**)iter;
  if(!(itype->type&IT_FOOD))continue;
  CheckUnitMaintenanceItem(uns,itype->abr,10,false);
 }

 foreach(ItemTypes){
  AItemType *itype=*(AItemType**)iter;
  if(!(itype->type&IT_FOOD))continue;
  bor2=CheckFactionMaintenanceItem(uns,itype->abr,10,false);
  if(borrowed==0) borrowed=bor2;
 }


 foreach(uns){
  AUnit *u=*(AUnit**)iter;
  if(!u->needed)continue;
  mes="Warning: Unit ";
  mes+=u->FullName();
  mes+=" needs ";
  mes+=u->needed;
  mes+=" silver for maintenance";
  AddEvent(mes);
  borrowed=0;
 }
 delete uns;
 if(borrowed){
  mes="Borrows ";
  AUnit *un=GetUnit(borrowed);
  if(un){
   mes+=un->FullName();
  }else{
   un=new AUnit(0);
   un->num=borrowed;
   mes+="unit ";
   mes+=un->Alias();
   delete un;
  }
  mes+=" in ";
  mes+=FullName();
  AddEvent(mes);
 }
}


bool ARegion::RunForgetOrders()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   foreach(u->orders){
    AOrder *ord=*(AOrder**)iter;
    if(ord->type!=O_FORGET)continue;
    if(ord->isedited)return false;
    ord->Run(0);
   }
  }
 }
 return true;
}


bool ARegion::RunTeachOrders()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrderAbr *or=dynamic_cast<AOrderAbr*>(u->orders->Find(O_STUDY,2));
   if(!or)continue;
   or->number=30*u->GetMen();
  }
 }
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrderTeach *ot=new AOrderTeach();
   ot->orders=u->orders;
   ot->bases=new AOrders(u);
   ot->bases->autodelete=false;
   foreach(u->orders){
    AOrder *o=*(AOrder**)iter;
    if(o->type!=O_TEACH)continue;
    if(o->commented)continue;
    if(o->isedited){
     delete ot;
     return false;
    }
    AOrderTeach *ord=dynamic_cast<AOrderTeach*>(o);
    int kol=ord->targets->Count;
    if(kol){
     ot->bases->Add(o);
    }
    for(int i=0;i<kol;i++){
     AUnitPtr *ptr=ord->GetTarget(i);
     ot->AddTarget(ptr->num,ptr->ptr);
    }
   }
   if(ot->targets->Count)
    ot->Run(0);
   delete ot;
  }
 }
 return true;
}


void ARegion::RunProduceOrdersA()
{
 foreach(products){
  AMarket *mark=*(AMarket**)iter;
  RunProduceOrdersA(mark);
 }
}
void ARegion::RunProduceOrdersA(AMarket *mark)
{
 AnsiString mes;
 AOrders *ords=new AOrders(0);
 ords->autodelete=false;
 int attempted=0;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrderAbr *or=dynamic_cast<AOrderAbr*>(u->orders->Find(O_PRODUCE,2));
   if(!or)continue;
   if(or->abr!=mark->type)continue;
   int att=or->Productivity();
   if(att>0){
    attempted+=att;
    ords->Add(or);
   }else if(att==0){
    mes=u->FullName();
    mes+=" PRODUCE: Unit isn't skilled enough";
    AddEvent(mes);
   }
  }
 }
 mark->price=attempted; 
 if(attempted)
 if(mark->amount){
  ords->Distribute(mark->amount,attempted);
 }else{
  mes="Can't produce ";
  mes+=ItemTypes->AbrToName(mark->type);
  mes+=" in ";
  mes+=FullName();
  AddEvent(mes);
 }
 delete ords;
}


void ARegion::RunProduceOrders()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrderAbr *or=dynamic_cast<AOrderAbr*>(u->orders->Find(O_PRODUCE,2));
   if(!or)continue;
   or->Run(-1);
  }
 }
}


bool ARegion::CanMakeAdv(AFaction * fac, AnsiString item)
{
 AItemType *itype=ItemTypes->Get(item);
 if(!itype->skill.Length()) return true;
 AnsiString skill=itype->skill;
 int level=itype->level;
 foreach(this){
  AObject *obj=*(AObject **) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un->endfaction!=fac)continue;
   if(un->GetBegSkill(skill)>=level)return true;
  }
 }
 return false;
}




void ARegion::RunEnterLeaveOrders()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   foreach(u->orders){
    AOrder *_ord=*(AOrder**)iter;
    if(_ord->commented) continue; 
    if(_ord->type==O_LEAVE||_ord->type==O_ENTER){
     _ord->Run(0);
    }
   }
  }
 }
}


bool ARegion::IsCoastal()
{
 if(type=="ocean") return 1;
 for(int i=0;i<NDIRS;i++){
  ARegion *reg=turn->RegionList->Get(neighbors[i]);
  if(!reg) continue;
  if(reg->type=="ocean") return 1;
 }
 return 0;
}


int ARegion::MoveCost(int movetype)
{
 int mult=2;
 AWeatherType *wtype=WeatherTypes->Get(NxtWeather());
 if(wtype) mult=wtype->movemult;
 if(movetype==M_WALK||movetype==M_RIDE){
  ATerrainType *ttype=TerrainTypes->Get(type);
  return ttype->movepoints*mult;
 }
 return mult;
}


AnsiString ARegion::NxtWeather()
{
 if(hasInfo&&oldinfoturnnum==turn->num) return nxtweather;
 return WeatherRegions->GetWeather(xloc,yloc,zloc,curTurnNumber+1);
}
void ARegion::RunGuard1Orders()
{
 AnsiString mes;
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   if(u->endguard!=GUARD_GUARD&&u->endguard!=GUARD_SET) continue;

   if(u->IsLocal()&&u->Taxers()==0){
    u->endguard=GUARD_NONE;
    mes="Error ";
    mes+=u->FullName();
    mes+=": Must be combat ready to be on guard";
    AddEvent(mes);
    continue;
   }
   u->endguard=GUARD_GUARD;
  }
 }
}
void ARegion::RunSailOrders()
{
 AnsiString mes;
 foreach(this){
  AObject *o=*(AObject **) iter;
  if(o->IsShip()){
   AUnit *u=o->GetOwner();
   AOrder *ord=0;
   if(u&&u->endobj==o){
    ord=u->orders->Find(O_SAIL,2);
   }
   if(ord){
    foreach(u->orders){
     AOrder *o=*(AOrder **)iter;
     if(o->commented) continue;
     if(o->type!=O_SAIL) continue;
     o->Run(-1);
    }
//    ord->Run(-1);
    continue;
   }
  }
  foreach(o->units){
   AUnit *un=*(AUnit**) iter;
   AOrder *ord=un->orders->Find(O_SAIL,2);
   if(ord){
    un->movepoints=0;
    mes="Error SAIL: ";
    mes+=un->FullName();
    mes+=" must be ship owner";
    AddEvent(mes);
   }
  }
 }
}
void ARegion::RunMoveOrders()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   foreach(u->orders){
    AOrder *ord=*(AOrder**)iter;
    if(ord->commented) continue;
    if(ord->type!=O_MOVE&&ord->type!=O_ADVANCE) continue;
    ord->Run(0);
    if(ord->isedited) return;
   }
  }
 }
}


int AObject::Protect()
{
 if(num==0) return 0;
 if(incomplete) return 0;
 AObjectType *ot=ObjectTypes->Get(type);
/* if(!ot)
  return 0;*/
 return ot->protect;
}
int AObject::Capacity()
{
 if(num==0) return 0;
 if(incomplete) return 0;
 AObjectType *ot=ObjectTypes->Get(type);
/* if(!ot)
  return 0;*/
 return ot->capacity;
}
int AObject::Sailors()
{
 if(num==0) return 0;
 if(incomplete) return 0;
 AObjectType *ot=ObjectTypes->Get(type);
/* if(!ot)
  return 0;*/
 return ot->sailors;
}


void ARegion::RunAutoOrders1()
{
 PrepareOrders();
 PrepareOrders2();
 RunFlagOrders();
 RunEnterLeaveOrders();
// RunPromoteOrders();

// RunStealOrders(); //not maked
// if(!RunGiveOrders())return;

 foreach(this){
  AObject *obj=*(AObject **) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(!un->IsLocal()) continue;
   foreach(un->orders){
    AOrder *o=*(AOrder**)iter;
    if(o->type!=O_GIVE)continue;
//    if(o->isedited)return false;
    AOrderGive *ord=dynamic_cast<AOrderGive*>(o);
    ord->Run(0);
   }
   for(int i=0;i<un->orders->count;i++){
    AOrder *_ord=un->orders->Get(i);
    if(_ord->type==O_AUTOGIVE){
     AOrderAutoGive *ord=dynamic_cast<AOrderAutoGive*>(_ord);
     int have=un->enditems->GetNum(ord->item);
     if(have==0) continue;
     int gived=0;
     foreach(this){
      AObject *obj2=*(AObject **) iter;
      if(!have) break;
      foreach(obj2->units){
       AUnit *un2=*(AUnit**)iter;
       if(un2==un) continue;
       if(!gived){
        have=un->enditems->GetNum(ord->item);
        if(ord->number==0){
         gived=have;
        }else if(ord->number<0){
         gived=have+ord->number;
         if(gived<=0){
          have=0;break;
         }
        }else{
         gived=ord->number;
         if(gived>have) gived=have;
        }
       }

       if(!gived) break;
       AnsiString name=un2->FullName2();
       if(ord->begstr.Length()){
        if(MyCompare(name.c_str(),ord->begstr)) continue;
       }
       if(ord->substr.Length()){
        if(name.Pos(ord->substr)==0) continue;
       }
       bool AddOrder=true;
       foreach(un->orders){
        AOrder *_ordg=*(AOrder**)iter;
        if(_ordg->type!=O_GIVE) continue;
        AOrderGive *ordg=dynamic_cast<AOrderGive*>(_ordg);
        if(ordg->target==un2&&ordg->item==ord->item){
         AddOrder=false;
         break;
        }
       }
       if(!AddOrder) continue;
       AOrderGive *ordg=new AOrderGive();
       ordg->target=un2;
       ordg->tarnum=un2->num;
       ordg->count=gived;
       ordg->item=ord->item;
       un->orders->Add(ordg);
       ordg->Run(0);
//       have=un->enditems->GetNum(ord->item)-gived;
//       un->enditems->SetNum(ord->item,have);
       AnsiString mes;
       mes=un->FullName();
       mes+=": AUTOGIVE give ";
       mes+=gived;
       mes+=" "+ItemTypes->AbrToNames(ord->item);
       mes+=" to "+un2->FullName();
       AddEvent(mes);
       have-=gived;
       gived=0;
      }
     }
    }
   }
  }
 }
}
void ARegion::Compact()
{
 bool is0=turn->num==0;
// if(oldobjinfo!=turn->num) return;
 if(oldinfoturnnum!=turn->num) return;
 foreachr(this){
  AObject *obj=*(AObject **) iter;
  if(obj->num==0){
   obj->units->Clear();
   continue;
  }
  AObjectType *ot=ObjectTypes->Get(obj->type);
  if(!ot) continue;
  if(!ot->canenter){
   if(is0) obj->units->Clear();
   continue;
  }
  obj->units->Clear();
  if(obj->inner) continue;
  TList::Remove(obj);
  delete obj;
 }
}


void ARegion::RunBuildOrders()
{
 foreach(this){
  AObject *o=*(AObject **) iter;
  foreach(o->units){
   AUnit *u=*(AUnit**)iter;
   AOrderBuild *or=dynamic_cast<AOrderBuild*>(u->orders->Find(O_BUILD,2));
   if(!or)continue;
   or->Run(-1);
  }
 }
}
AUnit * AObject::GetOwner()
{
 if(units->count<1) return 0;
 return *(AUnit**)units->List; 
}




ARegionStatus* __fastcall ARegion::GetRegionStatus()
{
 if(!FRegionStatus) UpdateRegionStatus();     
 return FRegionStatus;
}
void ARegion::ClearRegionStatus(){
 delete FRegionStatus;
 FRegionStatus=0;
}
void ARegion::UpdateRegionStatus(){
 delete FRegionStatus;
 FRegionStatus=new ARegionStatus;
 ARegionStatus *rs=FRegionStatus;
 foreach(this){
  AObject *obj=*(AObject **) iter;
  if(obj->IsTower()) rs->HasTower=true;
  if(obj->IsBuilding()) rs->HasBuilding=true;
  if(obj->inner) rs->HasShaft=true;
  if(obj->IsLair()) rs->HasLair=true;
  if(obj->IsShip()) rs->HasShip=true;
 }
}
ARegionStatus::ARegionStatus()
 :HasTower(0),HasBuilding(0),HasShaft(0),HasLair(0),HasShip(0)
{}
bool ARegion::PresentFaction(AFaction *fac){
 if(!fac) return false;
 foreach(this){
  AObject *obj=*(AObject **) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un->faction==fac) return true;
  }
 }
 return false;
}


int ARegion::GetObservation(AFaction * fac)
{
 int obs=0;
 foreach(this){
  AObject *obj=*(AObject **) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un->faction!=fac) continue;
   int temp=un->GetBegObservation();
   if(temp>obs) obs=temp;
  }
 }
 return obs;
}
int ARegion::GetTrueSight(AFaction * fac)
{
 int truesight=0;
//    if( S_TRUE_SEEING != -1 )
 foreach(this){
  AObject *obj=*(AObject **) iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un->faction!=fac) continue;
   int temp=un->GetBegSkill("TRUE");
   if(temp>truesight) truesight=temp;
  }
 }
 return truesight;
}
bool ARegion::HasExitRoad(int dir)
{
 int mask=1<<dir;
 foreach(this){
  AObject *obj=*(AObject **) iter;
  if(!obj->num) continue;
  AObjectType *otype=ObjectTypes->Get(obj->type);
  if(otype->road&mask)
   return true;
 }
 return false;
}

bool ARegion::HasConnectingRoad(int dir)
{
 int dir2=(dir+3)%6;
 return HasExitRoad(dir2);
}


int AObject::GetSize()
{
 int siz=sizeof(*this);
 siz+=name.Length();
 siz+=describe.Length();
 siz+=type.Length();
 siz+=units->GetSize();
 return siz;
}
int ARegion::GetSize()
{
 int siz=sizeof(*this);
/* if(!hasInfo||oldinfoturnnum<turn->num)
  return 50;*/
 siz+=Capacity*4;
 siz+=type.Length();
 siz+=name.Length();
 siz+=towntype.Length();
 siz+=townname.Length();
 siz+=race.Length();
 siz+=weather.Length();
 siz+=nxtweather.Length();
 siz+=wanteds->GetSize();
 siz+=saleds->GetSize();
 siz+=products->GetSize();
 if(FRegionStatus)
  siz+=sizeof(*FRegionStatus);
 foreach(this){
  AObject *obj=*(AObject**)iter;
  siz+=obj->GetSize();
 }
 return siz;
}


int ARegionArray::GetSize()
{
 int siz=sizeof(*this);
 siz+=name.Length();
 return siz;
}
int ARegionArrays::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 foreach(this){
  ARegionArray *ra=*(ARegionArray**)iter;
  siz+=ra->GetSize();
 }
 return siz;
}
int ARegionList::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 siz+=RegionArrays->GetSize();
 foreach(this){
  ARegion *reg=*(ARegion**)iter;
  siz+=reg->GetSize();
 }
 return siz;
}





