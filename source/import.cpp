//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>

#include "import.h"
#include "util.h"
#include "region.h"
#include "faction.h"
#include "turn.h"
#include "reports.h"
#include "markets.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern int movingdx[NDIRS];
extern int movingdy[NDIRS];

void ImportAtlaCartRegion(char *&s){
 AnsiString errbeg="Error loading region ",err=errbeg;
 ARegion *reg=new ARegion(curTurn);
 reg->zloc=1;
 bool skipregion=false,addexits=true;
 try{
  s+=3;
  if(!isdigit(*s))
   throw Exception(err);
  reg->xloc=atoi(GetNextToken(s).c_str());

  if(*s!=' ')
   throw Exception(err);
  s++;
  if(!isdigit(*s))
   throw Exception(err);
  reg->yloc=atoi(GetNextToken(s).c_str());
  if(*s!=' ')
   throw Exception(err);
  s++;
  if(!isdigit(*s))
   throw Exception(err);
  {
   int type=atoi(GetNextToken(s).c_str());
   if(type<1||type>TerrainTypes->count)
    throw Exception(AnsiString("Bad terrain type ")+type);
   ATerrainType *ttype=TerrainTypes->Get(type-1);
   reg->type=ttype->name;
  }
  err=errbeg+reg->FullName()+"?";
  if(*s!='\n')
   throw Exception(err);
  s++;

  if(!MyCompare(s,"LV ")){
   int lev=atoi(s+3)+1;
   addexits=false;
   if(curRegionList->RegionArrays->count>lev){
    reg->zloc=lev;
   }else
    skipregion=true;
   GetNextString(s);
  }

  if(MyCompare(s,"NM "))
   throw Exception(err);
  s+=3;
  reg->name=GetNextString(s);
  err=errbeg+reg->FullName();
  if(!MyCompare(s,"CT ")){
   s+=3;
   if(!isdigit(*s))
    throw Exception(err);
   int towntype=atoi(GetNextToken(s).c_str());
   ATownType *ttype=TownTypes->Get(towntype);
   reg->towntype=ttype->name;
   if(!ttype)
    throw Exception(AnsiString("Bad town type ")+towntype);
   if(*s!='\n')
    throw Exception(err);
   s++;
   if(MyCompare(s,"CN "))
    throw Exception(err);
   s+=3;
   reg->townname=GetNextString(s);
   err=errbeg+reg->FullName();
  }
  if(!MyCompare(s,"PP ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s))
    throw Exception(err);
   reg->population=atoi(GetNextToken(s).c_str());
   if(*s!='\n')
    throw Exception(err);
   s++;
   if(!MyCompare(s,"RA ")){
    s+=3;
    if(!isdigit(*s))
     throw Exception(err);
    int race=atoi(GetNextToken(s).c_str());
//   if(type<1||type>TerrainTypes->count)
    AItemType *itype=ItemTypes->Get(race);
    if(!itype||!(itype->type&IT_MAN))
     throw Exception(AnsiString("Bad race type ")+race);
    reg->race=itype->names;
    if(*s!='\n')
     throw Exception(err);
    s++;
   }else{
    reg->race="LEAD";
    AddEvent("Warning: region "+reg->FullName()+" has bad peasants");
   }
  }
  if(!MyCompare(s,"IN ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s))
    throw Exception(err);
   reg->money=atoi(GetNextToken(s).c_str());
   if(*s!='\n')
    throw Exception(err);
   s++;
  }
  if(!MyCompare(s,"WG ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s))
    throw Exception(err);
   reg->wages=atoi(GetNextToken(s).c_str());
   if(*s!='\n')
    throw Exception(err);
   s++;
  }
  if(!MyCompare(s,"MW ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s))
    throw Exception(err);
   reg->maxwages=atoi(GetNextToken(s).c_str());
   if(*s!='\n')
    throw Exception(err);
   s++;
  }
  if(!MyCompare(s,"EN ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s))
    throw Exception(err);
   reg->entertainment=atoi(GetNextToken(s).c_str());
   if(*s!='\n')
    throw Exception(err);
   s++;
  }
  if(!MyCompare(s,"TN ")){
   GetNextString(s);
  }
  while(!MyCompare(s,"DM ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s)&&*s!='-')
    throw Exception(err);
   int kolvo=atoi(GetNextToken(s).c_str());
   if(kolvo<0){
    kolvo=9999;
   }
   if(*s!=' ')
    throw Exception(err);
   s++;
   AnsiString type=GetNextToken(s);
   AItemType *itype=ItemTypes->Get(type);
   if(!itype)
    throw Exception(AnsiString("Bad wanted type ")+type);
   if(*s!=' ')
    throw Exception(err);
   s++;
   if(!isdigit(*s))
    throw Exception(err);
   int price=atoi(GetNextToken(s).c_str());
   AMarket *mark=new AMarket;
   mark->type=type;
   mark->amount=kolvo;
   mark->price=price;
   reg->wanteds->Add(mark);
   if(*s!='\n')
    throw Exception(err);
   s++;
  }
  while(!MyCompare(s,"OF ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s)&&*s!='-')
    throw Exception(err);
   int kolvo=atoi(GetNextToken(s).c_str());
   if(kolvo<0){
    kolvo=9999;
   }
   if(*s!=' ')
    throw Exception(err);
   s++;
   AnsiString type=GetNextToken(s);
   AItemType *itype=ItemTypes->Get(type);
   if(!itype)
    throw Exception(AnsiString("Bad saled type ")+type);
   if(*s!=' ')
    throw Exception(err);
   s++;
   if(!isdigit(*s))
    throw Exception(err);
   int price=atoi(GetNextToken(s).c_str());
   AMarket *mark=new AMarket;
   mark->type=type;
   mark->amount=kolvo;
   mark->price=price;
   reg->saleds->Add(mark);
   if(*s!='\n')
    throw Exception(err);
   s++;
  }
  while(!MyCompare(s,"PR ")){
   reg->hasInfo=true;
   s+=3;
   if(!isdigit(*s))
    throw Exception(err);
   int kolvo=atoi(GetNextToken(s).c_str());
   if(*s!=' ')
    throw Exception(err);
   s++;
   AnsiString type=GetNextToken(s);
   AItemType *itype=ItemTypes->Get(type);
   if(!itype)
    throw Exception(AnsiString("Bad product type ")+type);
   AMarket *mark=new AMarket;
   mark->type=type;
   mark->amount=kolvo;
   reg->products->Add(mark);
   if(*s!='\n')
    throw Exception(err);
   s++;
  }

  if(MyCompare(s,"EH\n"))
   throw Exception(err);
  s+=3;
  if(*s=='\n') s++;
  if(!skipregion){
   reg->oldinfoturnnum=0;
   reg->weather=WeatherRegions->GetWeather(reg->xloc,reg->yloc,reg->zloc,curTurnNumber);
   reg->nxtweather=WeatherRegions->GetWeather(reg->xloc,reg->yloc,reg->zloc,curTurnNumber+1);
   if(addexits) for(int i=0;i<6;i++){
    int x=reg->xloc+movingdx[i];
    int y=reg->yloc+movingdy[i];
    if(y<0||y>63) continue;
    if(x<0) x+=64;
    if(x>63) x-=64;
    reg->neighbors[i].xloc=x;
    reg->neighbors[i].yloc=y;
    reg->neighbors[i].zloc=1;
   }
   if(!ProcessUpdateRegion(reg))
    throw Exception(err);
   AddEvent("Region info added for "+reg->FullName());
  }else{
   AddEvent("Underground region info skipped for "+reg->FullName());
  }
 }__finally{
  delete reg;
 }
}
bool ImportAtlaCart(AnsiString fName){
 TMemoryStream*mem=0;
 curTurns->SetCurTurn(0);
 curFactions->SetCurFaction(0);
 try{
  try{
   mem=LoadFile(fName);
   char*s=(char*)mem->Memory;
   while(!MyCompare(s,"HX ")){
    ImportAtlaCartRegion(s);
   }
  }__finally{
   delete mem;
  }
 }catch(Exception &e){
  Application->ShowException(&e);
  return false;
 }
 return true;
}

