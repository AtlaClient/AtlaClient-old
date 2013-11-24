//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <clipbrd.hpp>
#include <stdlib.h>
#include "util.h"
#include "MainFrm.h"
#include "NewGameFrm.h"
#include "turn.h"
#include "ChangeGameFrm.h"
#include "reports.h"
#include "ShowSkillTypesFrm.h"
#include "ShowItemTypesFrm.h"
#include "ShowSkillInfosFrm.h"
#include "region.h"
#include "viewitems.h"
#include "faction.h"
#include "unit.h"
#include "LinkShaftFrm.h"
#include "orders.h"
#include "EventsFrm.h"
#include "markets.h"
#include "ProdModeSetupFrm.h"
#include "skills.h"
#include "WedgeModeSetupFrm.h"
#include "exprlang.h"
#include "PackingFrm.h"
#include "StoreFrm.h"
#include "RegInfoFrm.h"
#include "MapFrm.h"
#include "import.h"
#include "OptionsFrm.h"
#include "EditOrderReceiveFrm.h"
#include "AboutFrm.h"
#include "FormNewUnitFrm.h"
#include "SaveMapFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
AnsiString ClientPath;
AnsiString IniName="AtlaClient.ini";
const char*IniSect="games", *SettingsSect="settings";
const MapImageWidth=46,MapImageHeight=39;
const MapDX=35,MapDY=20;
TMainForm *MainForm;
int GridXs[7]={
 -12,11,23,11,-12,-24,-12
};
int GridYs[7]={
 -20,-20,0,20,20,0,-20
};
int PoligXs[7]={
 -12,11,23,11,-12,-24,-12
};
int PoligYs[7]={
 -20,-20,0,20,20,0,-20
};
TColor curregColor=clRed,wallColor=clWhite/*clLtGray*/;
TColor gridColor=clBlack,textColor=clYellow;
static int curregGrids[NDIRS]={3,3,3,3,3,3};
static TPopupMenu *TreeViewPopupMenu=0;

int HasTowerIndex=-1,HasBuildingIndex=-1, HasShaftIndex=-1, HasLairIndex=-1, HasShipIndex=-1;
static int MakingTree=0;
AnsiString orderBuf;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner),NoSave(false)
{
 vtreenodes=new VTreeNodes;
 vlistitems=new VListItems;
 ClientPath=ExtractFilePath(Application->ExeName);
 IniName=ClientPath+IniName;
 MapMode=0;
 ProdModeAttr=15;

 miTemporary->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
// int t1=GetTickCount();
 LoadOptions();
 LoadGameBase();
// int t2=GetTickCount();
 VerGameLoaded();
 ChangeTurn(0);
 PanelMapResize(this);
// int t3=GetTickCount();
// t3-=t2;
// t2-=t1;
// ShowMessage(AnsiString(t2)+" "+t3);
}
__fastcall TMainForm::~TMainForm()
{
 DeleteGameBase();
/* delete TerrainTypes;
 delete WeatherTypes;
 delete DirTypes;
 delete SkillTypes;
 delete ItemTypes;
 delete TownTypes;*/

 delete Games;

 delete vtreenodes;
 delete vlistitems;
}
void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
/* if(TerrainTypes->modified){
  int i=Application->MessageBox("Terrain types modified\nSave it?","Warning",MB_YESNOCANCEL);
  if(i==IDCANCEL){
   CanClose=false;
  }else if(i==IDYES){
   TerrainTypes->Save();
  }
 }
 if(WeatherTypes->modified){
  int i=Application->MessageBox("Weather types modified\nSave it?","Warning",MB_YESNOCANCEL);
  if(i==IDCANCEL){
   CanClose=false;
  }else if(i==IDYES){
   WeatherTypes->Save();
  }
 }
 if(SkillTypes->modified){
  int i=Application->MessageBox("Skill types modified\nSave it?","Warning",MB_YESNOCANCEL);
  if(i==IDCANCEL){
   CanClose=false;
  }else if(i==IDYES){
   SkillTypes->Save();
  }
 }
 if(ItemTypes->modified){
  int i=Application->MessageBox("Item types modified\nSave it?","Warning",MB_YESNOCANCEL);
  if(i==IDCANCEL){
   CanClose=false;
  }else if(i==IDYES){
   ItemTypes->Save();
  }
 }
 if(TownTypes->modified){
  int i=Application->MessageBox("Town types modified\nSave it?","Warning",MB_YESNOCANCEL);
  if(i==IDCANCEL){
   CanClose=false;
  }else if(i==IDYES){
   TownTypes->Save();
  }
 }*/
 if(NoSave) return;
 SaveOptions();
 SaveGameBase();
}
void __fastcall TMainForm::Terraintypes1Click(TObject *Sender)
{
/* TShowTerrainTypesForm *stt=new TShowTerrainTypesForm(this);
 stt->ShowModal();
 delete stt;*/
}
void __fastcall TMainForm::Exit1Click(TObject *Sender)
{
 Close();
}
void __fastcall TMainForm::Weathertypes1Click(TObject *Sender)
{
/* TShowWeatherTypesForm *swt=new TShowWeatherTypesForm(this);
 swt->ShowModal();
 delete swt;*/
}

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
 int i;
 Games=new TStringList;
 TIniFile *ini=new TIniFile(IniName);
 curGame=ini->ReadString(IniSect,"current","");
// actShowMap->Checked=atoi(ini->ReadString(SettingsSect,"ShowMap","").c_str());
// actShowRegInfo->Checked=atoi(ini->ReadString(SettingsSect,"ShowRegInfo","").c_str());
 TStringList*slst=new TStringList;
 ini->ReadSectionValues(IniSect,slst);
 for(i=0;i<slst->Count;i++){
  AnsiString str=slst->Strings[i];
  if(str.Pos("game")!=1)continue;
  int pos=str.Pos("=");
  if(pos==0)continue;
  Games->Add(str.SubString(pos+1,str.Length()-pos));
 }
 delete slst;
 miGameChange->Enabled=(Games->Count>1);
 for(i=0;i<Games->Count;i++){
  if(Games->Strings[i]==curGame)break;
 }
 if(i==Games->Count) curGame="";
 delete ini;
 MapImages->Width=MapImageWidth;
 MapImages->Height=MapImageHeight;
// AddMapImage("null.bmp");
 OpenDialog->InitialDir=ClientPath;
 OpenDialog->Filter="All files|*.*";

 HasTowerIndex=AddInfoImage("tower.bmp");
 HasBuildingIndex=AddInfoImage("building.bmp");
 HasShaftIndex=AddInfoImage("shaft.bmp");
 HasLairIndex=AddInfoImage("lair.bmp");
 HasShipIndex=AddInfoImage("ship.bmp");

 bnShowEvents->ImageIndex=AddToolBarImage("showevents.bmp");
 bnRunOrders->ImageIndex=AddToolBarImage("runorders.bmp");
 bnRunAutoOrders->ImageIndex=AddToolBarImage("runautoorders.bmp");
 bnTownMode->ImageIndex=AddToolBarImage("townmode.bmp");
 bnProdMode->ImageIndex=AddToolBarImage("prodmode.bmp");
 bnWedgeMode->ImageIndex=AddToolBarImage("wedgemode.bmp");
 actLevelUp->ImageIndex=AddToolBarImage("levelup.bmp");
 actLevelDown->ImageIndex=AddToolBarImage("leveldown.bmp");
 actPrevTurn->ImageIndex=AddToolBarImage("turnprev.bmp");
 actNextTurn->ImageIndex=AddToolBarImage("turnnext.bmp");

 actPrevUnit->ImageIndex=actPrevTurn->ImageIndex;
 actNextUnit->ImageIndex=actNextTurn->ImageIndex;

 miAddOrderDestroy->Tag=O_DESTROY;
 miAddOrderEntertain->Tag=O_ENTERTAIN;
 miAddOrderPillage->Tag=O_PILLAGE;
 miAddOrderTax->Tag=O_TAX;
 miAddOrderWork->Tag=O_WORK;

 miAddOrderNOP->Tag=O_NOP;

 miAddOrderAutotax->Tag=O_AUTOTAX;
 miAddOrderAvoid->Tag=O_AVOID;
 miAddOrderBehind->Tag=O_BEHIND;
 miAddOrderGuard->Tag=O_GUARD;
 miAddOrderHold->Tag=O_HOLD;
 miAddOrderNoaid->Tag=O_NOAID;
 miAddOrderReveal->Tag=O_REVEAL;
 miAddOrderConsume->Tag=O_CONSUME;

 miAddOrderCombat->Tag=O_COMBAT;
 miAddOrderForget->Tag=O_FORGET;
 miAddOrderProduce->Tag=O_PRODUCE;
 miAddOrderStudy->Tag=O_STUDY;

 miAddOrderBuy->Tag=O_BUY;
 miAddOrderSell->Tag=O_SELL;

 miAddOrderClaim->Tag=O_CLAIM;

 miAddOrderLeave->Tag=O_LEAVE;
 miAddOrderEnter->Tag=O_ENTER;
 miAddOrderMove->Tag=O_MOVE;
 miAddOrderAdvance->Tag=O_ADVANCE;
 miAddOrderSail->Tag=O_SAIL;

 bnTownMode->Down=true;
 ShowMapMode();
}
//---------------------------------------------------------------------------
void TMainForm::SaveGameList()
{
 TIniFile *ini=new TIniFile(IniName);
 ini->EraseSection(IniSect);
 for(int i=0;i<Games->Count;i++){
  ini->WriteString(IniSect,AnsiString("game")+i,Games->Strings[i]);
 }
 delete ini;
}

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
 SaveGameList();
 TIniFile *ini=new TIniFile(IniName);
 ini->WriteString(IniSect,"current",curGame);
// ini->WriteString(SettingsSect,"ShowMap",int(actShowMap->Checked));
// ini->WriteString(SettingsSect,"ShowRegInfo",int(actShowRegInfo->Checked));
 delete ini;
// delete curTurns;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miGameNewClick(TObject *Sender)
{
 TNewGameForm *ngf=new TNewGameForm(this);
 ngf->ShowModal();
 delete ngf;
 VerGameLoaded();
 MapForm->ResetMap();
 ChangeTurn(0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miGameChangeClick(TObject *Sender)
{
 TChangeGameForm *cgf=new TChangeGameForm(this);
 cgf->ShowModal();
 delete cgf;
 VerGameLoaded();
 MapForm->ResetMap();
 ChangeTurn(0);
}
void TMainForm::VerGameLoaded()
{
 bool loaded=true;
 if(!curGame.Length())loaded=false;
 if(!curTurns)loaded=false;
// N2->Enaible=loaded;
 miLoadReport->Enabled=loaded;
 miView->Visible=loaded;

 miLoadOrder->Enabled=loaded;
 miSaveOrder->Enabled=loaded;
 miSaveReport->Enabled=loaded;
 miSaveMap->Enabled=loaded;

 miView->Visible=loaded;
 miAnalyze->Visible=loaded;
 miFix->Visible=loaded;

 PanelMain->Visible=loaded;

 miPackTurns->Enabled=loaded;
 miStoreTurns->Enabled=loaded;
 miImport->Enabled=loaded;
}

void __fastcall TMainForm::miLoadReportClick(TObject *Sender)
{
 curTurns->StoreLastParam();
 OpenDialog->Filter="All files|*.*";
 if(OpenDialog->Execute()){
  OpenDialog->InitialDir=ExtractFilePath(OpenDialog->FileName);
  bool ok=true;
  TStringList *list=new TStringList;
  list->Assign(OpenDialog->Files);
  list->Sort();
  for(int i=0;ok&&i<list->Count;i++){
   if(!ProcessReport(list->Strings[i])) ok=false;
  }
  delete list;
  ChangeTurn(0);
 }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miShowSkilltypesClick(TObject *Sender)
{
 TShowSkillTypesForm *sst=new TShowSkillTypesForm(this);
 sst->Show();
/* sst->ShowModal();
 delete sst;*/
}
void __fastcall TMainForm::miShowSkillReportsClick(TObject *Sender)
{
 TShowSkillInfosForm *ssi=new TShowSkillInfosForm(this);
 ssi->Show();
/* ssi->ShowModal();
 delete ssi;*/
}
void __fastcall TMainForm::miShowItemTypesClick(TObject *Sender)
{
 TShowItemTypesForm *sit=new TShowItemTypesForm(this);
 sit->Show();
/* sit->ShowModal();
 delete sit;*/
}
void TMainForm::DrawMap()
{
 TCanvas *Canv=ImageMap->Canvas;
 Canv->Brush->Color=clLtGray;
 TRect r(0,0,ImageMap->Width,ImageMap->Height);
 Canv->FillRect(r);
 if(!curRegion)return;
 int xc=ImageMap->Width/2,yc=ImageMap->Height/2;
 int xloc=xcentr,yloc=ycentr,zloc=zcentr;
 int bx=xloc-xc/MapDX-1,by=yloc-yc/MapDY-1;
 curRegionArray->NormCoord2(bx,by);
 int ex=xloc+xc/MapDX+1,ey=yloc+yc/MapDY+1;
 curRegionArray->NormCoord2(ex,ey);

 int _x,_y;
 for(int y=by;y<=ey;y++){
  for(int x=bx;x<=ex;x++){
   if((x+y)&1)continue;
   int xx=x;
   curRegionArray->NormCoord(xx,y);
   _x=xc+MapDX*(x-xloc);
   _y=yc+MapDY*(y-yloc);
   DrawRegion(Canv,xx,y,zloc,_x,_y);
  }
 }
 for(int y=by;y<=ey;y++){
  for(int x=bx;x<=ex;x++){
   if((x+y)&1)continue;
   _x=xc+MapDX*(x-xloc);
   _y=yc+MapDY*(y-yloc);
   int xx=x;
   curRegionArray->NormCoord(xx,y);
   ARegion *reg=curRegionList->Get(xx,y,zloc);
   if(!reg) continue;
   if(!reg->hasInfo) continue;
//   if(!(reg->infoStatus&isExitsInfo)) continue;
   int grids[NDIRS];
   for(int dir=0;dir<NDIRS;dir++){
    grids[dir]=reg->neighbors[dir].xloc==-1?1:(zloc>1?2:0);
   }
   DrawGrid(Canv,_x,_y,grids);
  }
 }
 int maxx=curRegionArray->x;
 if(maxx&1) maxx++;
 int xx=curRegion->xloc-xloc;
 if(xx<-(maxx/2)) xx+=maxx;
 if(xx>maxx/2) xx-=maxx;
 _x=xc+MapDX*(xx);
 _y=yc+MapDY*(curRegion->yloc-yloc);
 DrawGrid(Canv,_x,_y,curregGrids);
 Canv->Font->Color=textColor;
 Canv->Font->Name="Arial";
 Canv->Brush->Style=bsClear;
 if(MapMode==0){
  Canv->Font->Size=-11;
  for(int y=by;y<=ey;y++){
   for(int x=bx;x<=ex;x++){
    if((x+y)&1)continue;
    int xx=x;
    curRegionArray->NormCoord(xx,y);
    _x=xc+MapDX*(x-xloc);
    _y=yc+MapDY*(y-yloc);
    DrawRegionTown(Canv,xx,y,zloc,_x,_y);
   }
  }
 }else if(MapMode==1){
  Canv->Font->Size=-9;
  for(int y=by;y<=ey;y++){
   for(int x=bx;x<=ex;x++){
    if((x+y)&1)continue;
    int xx=x;
    curRegionArray->NormCoord(xx,y);
    _x=xc+MapDX*(x-xloc);
    _y=yc+MapDY*(y-yloc);
    DrawRegionProd(Canv,xx,y,zloc,_x,_y);
   }
  }
 }else if(MapMode==2){
  for(int y=by;y<=ey;y++){
   for(int x=bx;x<=ex;x++){
    if((x+y)&1)continue;
    int xx=x;
    curRegionArray->NormCoord(xx,y);
    _x=xc+MapDX*(x-xloc);
    _y=yc+MapDY*(y-yloc);
    DrawRegionWedge(Canv,xx,y,zloc,_x,_y);
   }
  }
 }
}

void __fastcall TMainForm::PanelMapResize(TObject *Sender)
{
 TBmp *bmp=ImageMap->Picture->Bitmap;
 if(!bmp)return;
 bmp->Width=ImageMap->Width;
 bmp->Height=ImageMap->Height;
 DrawMap();
}
void TMainForm::DrawRegion(TCanvas *Canv,int x,int y,int z, int xc, int yc)
{
 TPoint p[6];
 ARegion *reg=curRegionList->Get(x,y,z);
 for(int i=0;i<NDIRS;i++){
  p[i].x=PoligXs[i]+xc;
  p[i].y=PoligYs[i]+yc;
 }
 Canv->Pen->Color=gridColor; //clBlack;
 ATerrainType *ttype=0;
 if(reg){
  ttype=TerrainTypes->Get(reg->type);
 }
 if(ttype){
  Canv->Brush->Color=ttype->color;
 }else{
  Canv->Brush->Color=clWhite;
 }
 Canv->Polygon(p,5);
}
void TMainForm::DrawRegionTown(TCanvas * Canv, int x, int y, int z, int xc, int yc)
{
 ARegion *reg=curRegionList->Get(x,y,z);
 if(!reg)return;
 if(reg->townname.Length()){
  ATownType *tt=TownTypes->Get(reg->towntype);
  if(tt&&tt->imgnum!=-1){
   InfoImages->Draw(Canv,xc-7,yc-18,tt->imgnum,true);
  }
  AnsiString name=reg->townname;
  if(optTownNameLimit>0){
   if(name.Length()>optTownNameLimit)
    name.SetLength(optTownNameLimit);
  }
  TSize s=Canv->TextExtent(name);
  Canv->TextOut(xc-s.cx/2,yc,name);
 }
 ARegionStatus *rs=reg->RegionStatus;
 if(rs->HasTower){
  InfoImages->Draw(Canv,xc+2,yc-18,HasTowerIndex,true);
 }
 if(rs->HasBuilding){
  InfoImages->Draw(Canv,xc+2,yc-8,HasBuildingIndex,true);
 }
 if(rs->HasShaft){
  InfoImages->Draw(Canv,xc-17,yc-8,HasShaftIndex,true);
 }
 if(rs->HasLair){
  InfoImages->Draw(Canv,xc+11,yc-8,HasLairIndex,true);
 }
 if(rs->HasShip){
  InfoImages->Draw(Canv,xc-7,yc-8,HasShipIndex,true);
 }
 if(reg->PresentLocals()){
  Canv->Pen->Color=textColor;
  Canv->MoveTo(xc-12,yc-MapImageHeight/2+1);
  Canv->LineTo(xc-11,yc-9);
  Canv->LineTo(xc-7,yc-MapImageHeight/2);
  if(reg->HasLocalTaxers()){
   Canv->MoveTo(xc-12,yc-MapImageHeight/2+1);
   Canv->LineTo(xc-7,yc-MapImageHeight/2+1);
  }
 }
}
void TMainForm::DrawRegionProd(TCanvas * Canv, int x, int y, int z, int xc, int yc)
{
 ARegion *reg=curRegionList->Get(x,y,z);
 if(!reg)return;
 int xx=xc-13,yy=yc-MapImageHeight/2-0;
 int num=0;
 foreach(reg->products){
  AMarket *mark=*(AMarket**)iter;
  AItemType *itype=ItemTypes->Get(mark->type);
  if(itype->type&IT_FOOD){
   if(!(ProdModeAttr&1))continue;
  }else if(itype->type&IT_NORMAL){
   if(!(ProdModeAttr&2))continue;
  }else if(itype->type&IT_ADVANCED){
   if(!(ProdModeAttr&4))continue;
  }
  if(mark->amount==0){
   if(!(ProdModeAttr&8))continue;
   Canv->TextOut(xx,yy,mark->type);
   Canv->MoveTo(xx,yy+5);Canv->LineTo(xc+13,yy+5);
  }else{
   Canv->TextOut(xx,yy,mark->type);
  }
  yy+=9;
  if(++num>=4)break;
 }
}
static int secpoint[]={0,1,2,3,4,5},tripoint[]={1,2,3,4,5,0};
TColor wedgecolors[6]={
 clRed, clGreen, clBlue, clYellow,clOlive,TColor(0xff00b0)
};
void TMainForm::DrawRegionWedge(TCanvas * Canv, int x, int y, int z, int xc, int yc)
{
 ARegion *reg=curRegionList->Get(x,y,z);
 if(!reg)return;
 if(reg->townname.Length()){
  ATownType *tt=TownTypes->Get(reg->towntype);
  if(tt&&tt->imgnum!=-1){
   InfoImages->Draw(Canv,xc-7,yc-MapImageHeight/2+1,tt->imgnum,true);
  }
 }
 TPoint p[4];
 p[0].x=p[3].x=xc;
 p[0].y=p[3].y=yc;
 AnsiString expr;
 ExprPrepareRegion(reg);
#define AAA *2/3
 for(int i=0;i<6;i++){
  expr=wedgeexpr[i];
  if(!expr.Length()) continue;
  if(!ExprProcessExpr(expr)) continue;
  p[1].x=PoligXs[secpoint[i]] AAA+xc;
  p[1].y=PoligYs[secpoint[i]] AAA+yc;
  p[2].x=PoligXs[tripoint[i]] AAA+xc;
  p[2].y=PoligYs[tripoint[i]] AAA+yc;
#undef AAA
  Canv->Pen->Color=clBlack;
  Canv->Brush->Color=wedgecolors[i];
  Canv->Polygon(p,3);
 }
}
void TMainForm::DrawGrid(TCanvas *Canv,int xc, int yc, int grids[NDIRS])
{
 for(int dir=0;dir<NDIRS;dir++){
  switch(grids[dir]){
   case 0:
    Canv->Pen->Color=gridColor;
   break;
   case 1:
    Canv->Pen->Color=wallColor;
    Canv->Pen->Width=1;
   break;
   case 2:{
    Canv->Pen->Color=wallColor;
    Canv->Pen->Width=1;
    int x1=xc+GridXs[dir],y1=yc+GridYs[dir];
    int x2=xc+GridXs[dir+1],y2=yc+GridYs[dir+1];
    int dx=(x2-x1)/4,dy=(y2-y1)/4;
    Canv->MoveTo(x1,y1);
    Canv->LineTo(x1+dx,y1+dy);
    Canv->MoveTo(x2,y2);
    Canv->LineTo(x2-dx,y2-dy);
   }
   continue;
   case 3:{
    Canv->Pen->Color=curregColor;
    Canv->Pen->Width=2;
    int x1=xc+GridXs[dir],y1=yc+GridYs[dir];
    int x2=xc+GridXs[dir+1],y2=yc+GridYs[dir+1];
    int dx=(x2-x1)/4,dy=(y2-y1)/4;
    Canv->MoveTo(x1,y1);
    Canv->LineTo(x1+dx,y1+dy);
    Canv->MoveTo(x2,y2);
    Canv->LineTo(x2-dx,y2-dy);
   }
   continue;
//       Canv->Pen->Color=curregColor;
//   break;
  }
  Canv->MoveTo(xc+GridXs[dir],yc+GridYs[dir]);
  Canv->LineTo(xc+GridXs[dir+1],yc+GridYs[dir+1]);
 }
 Canv->Pen->Width=1;
}

int TMainForm::AddMapImage(AnsiString fName)
{
 TBmp *bmp=new TBmp;
 bmp->LoadFromFile(ClientPath+"images\\"+fName);
 bmp->TransparentMode=tmAuto;
 int ind=MapImages->AddMasked(bmp,bmp->TransparentColor);
 delete bmp;
 return ind;
}
int TMainForm::AddInfoImage(AnsiString fName)
{
 TBmp *bmp=new TBmp;
 int ind;
 try{
  bmp->LoadFromFile(ClientPath+"images\\"+fName);
  bmp->TransparentMode=tmAuto;
  ind=InfoImages->AddMasked(bmp,bmp->TransparentColor);
  delete bmp;
 }catch(...){
  delete bmp;
  return -1;
 }
 return ind;
}
int TMainForm::AddToolBarImage(AnsiString fName)
{
 TBmp *bmp=new TBmp;
 int ind;
 try{
  bmp->LoadFromFile(ClientPath+"images\\"+fName);
  bmp->TransparentMode=tmAuto;
  ind=ToolBarImages->AddMasked(bmp,bmp->TransparentColor);
  delete bmp;
 }catch(...){
  delete bmp;
  return -1;
 }
 return ind;
}


void __fastcall TMainForm::actLevelUpExecute(TObject *Sender)
{
 ChangeLevel(-1);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actLevelDownExecute(TObject *Sender)
{
 ChangeLevel(1);
}
//---------------------------------------------------------------------------


void TMainForm::ChangeLevel(int dir)
{
/* if(dir!=0){

 }*/
 curTurns->StoreLastXY();
 int newlevel=curRegion->zloc+dir;
 int newx;//=curRegion->xloc;
 int newy;//=curRegion->yloc;
 curTurns->RestoreLastXY(newx,newy,newlevel);
/* curRegionList->SetCurRegionArray(newlevel);
 if(newx>curRegionArray->x)newx=curRegionArray->x;
 if(newy>curRegionArray->y)newy=curRegionArray->y;*/
 ARegion *reg=curRegionList->GetNearestRegion(newx,newy,newlevel);
 if(!reg)return;
 curRegionList->SetCurRegion(reg->xloc,reg->yloc,reg->zloc);
 {
  bool ena=(newlevel!=0);
  actLevelUp->Enabled=ena;
//  bnLevelUp->Enabled=ena;
 }
 {
  bool ena=(curRegionList->RegionArrays->count>newlevel+1);
  actLevelDown->Enabled=ena;
//  bnLevelDown->Enabled=ena;
 }
 xcentr=curRegion->xloc;
 ycentr=curRegion->yloc;
 zcentr=curRegion->zloc;
 OnSetCurRegion();
}

void TMainForm::PointToCoord(int x, int y, int & xloc, int & yloc)
{
 int sign,_xloc,_yloc;
 _xloc=x-ImageMap->Width/2;
 sign=_xloc<0?-1:1;
 _xloc=(sign*_xloc+MapDX/2)/MapDX;

 xloc=xcentr+_xloc*sign;
 _yloc=y-ImageMap->Height/2;
 sign=_yloc<0?-1:1;
 if(_xloc&1){
  _yloc=(sign*_yloc/MapDY)|1;
 }else{
  _yloc=(sign*_yloc/MapDY+1)&(~1);
 }
 yloc=ycentr+_yloc*sign;
}
void __fastcall TMainForm::ImageMapMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 int xloc,yloc;
 PointToCoord(X,Y,xloc,yloc);
 if(!curRegionArray) return;
 curRegionArray->NormCoord(xloc,yloc);
 AnsiString mes;
 if(Button==mbRight){
  xcentr=xloc;
  ycentr=yloc;
  DrawMap();
  ImageMapMouseMove(Sender,Shift,X,Y);
  return;
 }
 if(Button==mbLeft){
  ARegion *reg=curRegionList->GetNearestRegion(xloc,yloc,zcentr);
  curRegionList->SetCurRegion(reg->xloc,reg->yloc,reg->zloc);
  OnSetCurRegion();
  return;
 }
}
void __fastcall TMainForm::ImageMapMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
 int xloc,yloc;
 if(!curRegionArray) return;
 PointToCoord(X,Y,xloc,yloc);
 SetlbCoords(xloc,yloc);
}
void TMainForm::SetlbCoords(int xloc, int yloc)
{
 curRegionArray->NormCoord(xloc,yloc);
 AnsiString mes;
 mes+=xloc;
 mes+=",";
 mes+=yloc;
 lbCoord->Caption=mes;
 RegInfoForm->UpdateRegionInfo(xloc,yloc,curRegion->zloc);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actPrevTurnExecute(TObject *Sender)
{
 ChangeTurn(-1);
}
void __fastcall TMainForm::actNextTurnExecute(TObject *Sender)
{
 ChangeTurn(1);
}
void TMainForm::ChangeTurn(int dir)
{
 if(!curTurn)return;
 int newturn=curTurn->num+dir;
 if(newturn<0)newturn=0;
 if(newturn>=curTurns->count)newturn=curTurns->count-1;
 if(newturn!=0&&newturn<curTurns->curbeginturn){
  if(dir<0) newturn=0; else newturn=curTurns->curbeginturn;
 }
 curTurns->StoreLastParam();
 if(!curTurns->SetCurTurn(newturn))return;
 actPrevTurn->Enabled=newturn>0;
 actNextTurn->Enabled=newturn+1<curTurns->count;
 ChangeLevel(0);
}
void TMainForm::ViewShow()
{
static int InViewShow=0;
 curUnit=0;
 if(!curNode)return;
 VTreeNode *vnode=(VTreeNode*)curNode->Data;
 if(!vnode)return;
 if(InViewShow) return;
 InViewShow++;
 vlistitems->Clear();
 AFaction *fac;
 TreeViewPopupMenu=0; //
 switch(vnode->type){
  case tntFaction:
   fac=curFactions->GetFaction(vnode->value);
   if(!fac)break;
   fac->MakeView(vlistitems);
  break;
  case tntFactionErrors:
   fac=curFactions->GetFaction(vnode->value);
   if(!fac)break;
   fac->MakeViewErrors(vlistitems);

   TreeViewPopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vnode->value;
   miFacClearErrors->Visible=true;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=false;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=false;
   miFacClearEvent->Visible=false;
  break;
  case tntFactionBattles:
   fac=curFactions->GetFaction(vnode->value);
   if(!fac)break;
   fac->MakeViewBattles(vlistitems);

   TreeViewPopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vnode->value;
   miFacClearErrors->Visible=false;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=true;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=false;
   miFacClearEvent->Visible=false;
  break;
  case tntFactionEvents:
   fac=curFactions->GetFaction(vnode->value);
   if(!fac)break;
   fac->MakeViewEvents(vlistitems);

   TreeViewPopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vnode->value;
   miFacClearErrors->Visible=false;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=false;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=true;
   miFacClearEvent->Visible=false;
  break;
  case tntRegion:
   curRegion->MakeView(vlistitems);
   TreeViewPopupMenu=popupViewRegion;
   popupViewRegion->Tag=-1;
  break;
  case tntObject:
  {
   AObject *obj=curRegion->GetObject(vnode->value);
   if(!obj)break;
   obj->MakeView(vlistitems);

   TreeViewPopupMenu=popupViewRegObject;
   popupViewRegObject->Tag=vnode->value;

  }
  break;
  case tntUnit:
  {
   AUnit *unit=(AUnit*) vnode->data;//curRegion->GetUnit(vnode->value);
   if(!unit)break;
   unit->MakeView(vlistitems);
   curUnit=unit;

   TreeViewPopupMenu=popupViewUnit;
   popupViewUnit->Tag=-1;
  }
  break;
 }
// View->Items->Clear();
// View->Items->Add(curRegion->Print());
// View->Items->Text=curRegion->Print();;
 View->ItemIndex=-1;
 TStrings *list=View->Items;
 int maxsize=0;
 list->BeginUpdate();
 list->Clear();
 for(int i=0;i<vlistitems->count;i++){
  AnsiString str=vlistitems->Get(i)->text;
  list->Add(str);
  TSize siz=View->Canvas->TextExtent(str);
  if(siz.cx>maxsize)maxsize=siz.cx;
 }
 list->EndUpdate();
 View->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
// SetMainFormCaption();
 InViewShow--;
}

void __fastcall TMainForm::bnShowEventsClick(TObject *Sender)
{
 EventsForm->Show();
 EventsForm->BringToFront();
}
void TMainForm::OnSetCurRegion()
{
 DrawMap();
 SetMainFormCaption();
 MakeTreeView();
 RegInfoForm->ShowRegInfo=actShowRegInfo->Checked;
 RegInfoForm->UpdateRegionInfo(curRegion->xloc,curRegion->yloc,curRegion->zloc);

 MapForm->ShowMap=actShowMap->Checked;
 MapForm->UpdateMap(curRegion->zloc);
}
void TMainForm::MakeTreeView()
{

 TreeView->Selected=0;
 MakingTree++;
 TTreeNodes *tns=TreeView->Items;
 tns->BeginUpdate();
 tns->Clear();
 TTreeNode *tn;
 vtreenodes->Clear();
 VTreeNode *vtn;

 vtn=new VTreeNode(vtreenodes);
 tn=tns->AddChildObject(0,"Local factions",vtn);
 vtn->node=tn;
 curFactions->MakeTree(tn,vtreenodes);
 tn->Expand(true); //false
 tn->Collapse(false);
 vtn=new VTreeNode(vtreenodes);
 tn=tns->AddChildObject(0,"Region",vtn);
 vtn->node=tn;
 curRegion->MakeTree(tn,vtreenodes);
 tn->Expand(true);

 tn=tn->getFirstChild();
 TreeView->Selected=0;
 TreeView->AlphaSort();
 tns->EndUpdate();
 MakingTree--;
 TreeView->Selected=tn;
}


void __fastcall TMainForm::TreeViewMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button!=mbRight) return;
 TTreeNode *TN;
 TN=TreeView->GetNodeAt(X,Y);
 if (TN)
 {
  TreeView->Selected=TN;
  TreeViewChange(TreeView,TN);
 }
 TPoint mp=Mouse->CursorPos;
 if(TreeViewPopupMenu)
  TreeViewPopupMenu->Popup(mp.x,mp.y); 
/* if(Button==mbRight){
  TTreeNode *selnode=TreeView->Selected;
  TreeView->Selected=0;
  TreeView->Selected=selnode;
 }*/
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TreeViewChange(TObject *Sender, TTreeNode *Node)
{
 if(curNode==Node)return;
 curNode=Node;
 if(MakingTree)return;
 ViewShow();
 TreeView->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ViewClick(TObject *Sender)
{
 int ind=View->ItemIndex;
 if(ind==-1)return;
 VListItem *vli=vlistitems->Get(ind);
 if(!vli)return;
 View->PopupMenu=0;
 switch(vli->type){
  case litRegHeader:
  case litRegWages:
  case litRegEnte:

   View->PopupMenu=popupViewRegion;
   popupViewRegion->Tag=-1;
  break;
  case litRegObj:case litObjHeader:
   View->PopupMenu=popupViewRegObject;
   popupViewRegObject->Tag=vli->value;
  break;
  case litUnitHeader:case litUnitOrder:case litUnitInfo:
   View->PopupMenu=popupViewUnit;
   if(vli->type!=litUnitOrder){
    popupViewUnit->Tag=-1;
   }else{
    popupViewUnit->Tag=vli->value;
   }
  break;
  case litFactErrors:case litFactErrorsHeader:
   View->PopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vli->value;
   miFacClearErrors->Visible=true;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=false;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=false;
   miFacClearEvent->Visible=false;
  break;
  case litFactError:
   View->PopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vli->value;
   miFacClearErrors->Visible=false;
   miFacClearError->Visible=true;
   miFacClearBattles->Visible=false;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=false;
   miFacClearEvent->Visible=false;
  break;
  case litFactBattles:case litFactBattlesHeader:
   View->PopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vli->value;
   miFacClearErrors->Visible=false;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=true;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=false;
   miFacClearEvent->Visible=false;
  break;
  case litFactBattle:
   View->PopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vli->value;
   miFacClearErrors->Visible=false;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=false;
   miFacClearBattle->Visible=true;
   miFacClearEvents->Visible=false;
   miFacClearEvent->Visible=false;
  break;
  case litFactEvents: case litFactEventsHeader:
  case litUnitEvents:
   View->PopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vli->value;
   miFacClearErrors->Visible=false;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=false;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=true;
   miFacClearEvent->Visible=false;
  break;
  case litFactEvent: case litUnitEvent:
   View->PopupMenu=popupFactionMessages;
   popupFactionMessages->Tag=vli->value;
   miFacClearErrors->Visible=false;
   miFacClearError->Visible=false;
   miFacClearBattles->Visible=false;
   miFacClearBattle->Visible=false;
   miFacClearEvents->Visible=false;
   miFacClearEvent->Visible=true;
  break;
  default:
   View->PopupMenu=popupViewDefault;
 }
}
void __fastcall TMainForm::ViewDblClick(TObject *Sender)
{
 int ind=View->ItemIndex;
 if(ind==-1)return;
 VListItem *vli=vlistitems->Get(ind);
 if(!vli)return;
 VTreeNode *vnode=0;
 switch(vli->type){
  case litFactErrors:
   vnode=FindTreeNode(tntFactionErrors,vli->value);
  break;
  case litFactBattles:
   vnode=FindTreeNode(tntFactionBattles,vli->value);
  break;
  case litFactEvents:
   vnode=FindTreeNode(tntFactionEvents,vli->value);
  break;
  case litFactErrorsHeader:
  case litFactBattlesHeader:
  case litFactEventsHeader:
   vnode=FindTreeNode(tntFaction,vli->value);
  break;
  case litRegHeader:
//////
  break;
  case litRegObj:
   vnode=FindTreeNode(tntObject,vli->value);
  break;
  case litObjHeader:
   vnode=FindTreeNode(tntRegion,curRegion->num);
  break;
  case litObjUnit:
   vnode=FindTreeNode(tntUnit,vli->value);
  break;
  case litUnitHeader:
   {
    if(!curUnit)break;
    AObject *obj=curUnit->baseobj;
    if(!obj)break;
    if(obj->num){
     vnode=FindTreeNode(tntObject,obj->num);
    }else{
     vnode=FindTreeNode(tntRegion,curRegion->num);
    }
   }
  break;
  case litUnitOrder:
   EditOrder(vli->value);
  break;
 }
 if(vnode)TreeView->Selected=vnode->node;
}
//---------------------------------------------------------------------------


VTreeNode* TMainForm::FindTreeNode(int type, int value)
{
 for(int i=vtreenodes->count-1;i>=0;i--){
  VTreeNode *vnode=vtreenodes->Get(i);
  if(vnode->type==type&&vnode->value==value)return vnode;
 }
 return 0;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::ViewMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button==mbRight){
  {
   TWMMouse mes;
   mes.Msg=WM_LBUTTONDOWN;
   mes.Keys=MK_LBUTTON;
   mes.XPos=short(X);
   mes.YPos=short(Y);
   View->Dispatch(&mes);
  }
  {
   TWMMouse mes;
   mes.Msg=WM_LBUTTONUP;
   mes.Keys=MK_LBUTTON;
   mes.XPos=short(X);
   mes.YPos=short(Y);
   View->Dispatch(&mes);
  }
 }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::popupViewRegObjectPopup(TObject *Sender)
{
 AObject *obj=curRegion->GetObject(popupViewRegObject->Tag);
 if(!obj)return;
 bool IsInner=(obj->inner);
 miLinkInner->Enabled=IsInner;
 miGotoLinked->Enabled=IsInner&&obj->basereg->inner.xloc!=-1;
 miCopyToClipboard1->Visible=popupViewRegObject->PopupComponent==View;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miLinkInnerClick(TObject *Sender)
{
 AObject *obj=curRegion->GetObject(popupViewRegObject->Tag);
 if(!obj)return;
 TLinkShaftForm *frm=new TLinkShaftForm(this,obj);
 int res=frm->ShowModal();
 delete frm;
 if(res==mrOk){
  ViewShow();
 }
}
void __fastcall TMainForm::miGotoLinkedClick(TObject *Sender)
{
/* ARegion *reg=curRegionList->Get(obj->inner);
 if(!reg)return;*/
 ARegion *reg=curRegion;
 if(reg->inner.xloc==-1)return;
 curTurns->StoreLastXY();
 curRegionList->SetCurRegion(reg->inner.xloc,reg->inner.yloc,reg->inner.zloc);
 ChangeLevel(0);
}
void __fastcall TMainForm::miLoadOrderClick(TObject *Sender)
{
 if(curTurn->Packed){
  AnsiString str="Turn ";
  str+=curTurn->num;
  str+=" is PACKED";
  AddEvent("Error: "+str);
  Application->MessageBox(str.c_str(),"ERROR",MB_OK);
  return;
 }

 curTurns->StoreLastParam();
 char buf[10];
 sprintf(buf,"ord%05d.*",curTurnNumber);
 OpenDialog->Filter=AnsiString("Order file for current turn|")+buf+"|All files|*.*";
 if(OpenDialog->Execute()){
  OpenDialog->InitialDir=ExtractFilePath(OpenDialog->FileName);
  bool ok=true;
  TStringList *list=new TStringList;
  list->Assign(OpenDialog->Files);
  list->Sort();
  for(int i=0;ok&&i<list->Count;i++){
   if(!ProcessOrder(list->Strings[i]))ok=false;
  }
  delete list;
  ChangeTurn(0);
 }
}

void __fastcall TMainForm::miSaveOrderClick(TObject *Sender)
{
 if(curTurn->Packed){
  AnsiString str="Turn ";
  str+=curTurn->num;
  str+=" is PACKED";
  AddEvent("Error: "+str);
  Application->MessageBox(str.c_str(),"ERROR",MB_OK);
  return;
 }
 foreach(curFactions){
  AFaction *fac=*(AFaction**)iter;
  if(!fac->local)continue;
  int facnum=fac->number;
  AnsiString fName=GetOrderName(facnum);
  if(!SaveOrder(fName,facnum)){
   AnsiString str="Error saving order for faction ";
   str+=fac->FullName();
   AddEvent(str);
   break;
  }
 }
}
void __fastcall TMainForm::popupViewUnitPopup(TObject *Sender)
{
 int ind=popupViewUnit->Tag;
 miEditOrder->Enabled=(ind>=0);
 miDeleteOrder->Enabled=(ind>=0);
// miAddOrder->Enabled=(curUnit->IsLocal());
 miDeleteNewUnit->Enabled=(curUnit->num<0);

 miAddOrderFormNew->Enabled=/*(curUnit->num>0)&&*/(curUnit->IsLocal());
 miGotoOrderTarget->Enabled=(ind>=0);

 miPasteOrder->Enabled=(orderBuf.Length());

 miCopyToClipboard2->Visible=popupViewUnit->PopupComponent==View;

 miMoveOrderUp->Enabled=(ind>=1);
 miMoveOrderDown->Enabled=(ind>=0&&ind<curUnit->orders->count-1);

}
void __fastcall TMainForm::miEditOrderClick(TObject *Sender)
{
 EditOrder(popupViewUnit->Tag);
}
void __fastcall TMainForm::miDeleteOrderClick(TObject *Sender)
{
 if(!curUnit)return;
 int ind=popupViewUnit->Tag;
 AOrder *ord=curUnit->GetOrder(ind);
 if(!ord)return;
 AnsiString mes;
 if(ord->type==O_MESSAGE){
  AOrderMessage *mess=(AOrderMessage*) ord;
  AOrder *targ=mess->base;
  if(targ->type!=O_GIVE) return;
  mes="You are sure to delete base GIVE order?\n";
  mes+=targ->Print();
  if(Application->MessageBox(mes.c_str(),"Deleting order",MB_YESNO)!=IDYES)return;
  targ->orders->Delete(targ);

  ViewShow();
  return;
 }
 mes="You are sure to delete this order?\n";
 mes+=ord->Print();
 if(Application->MessageBox(mes.c_str(),"Deleting order",MB_YESNO)!=IDYES)return;
 curUnit->orders->Delete(ind);
 ViewShow();
}
void TMainForm::EditOrder(int ind, AOrder *neworder)
{
 if(!curUnit)return;
 AOrder *ord;
 if(neworder){
  ord=neworder;
  curUnit->orders->Add(ord);//=curUnit->orders;
 }else
  ord=curUnit->GetOrder(ind);
 if(!ord)return;
 if(ord->type==O_MESSAGE){
  if(neworder)throw Exception("Can't Create & Edit Message");
  miGotoOrderTargetClick(0);
  return;
 }
 AOrder *neword=ord->Edit();
 if(neworder){
  if(ord!=neword){
   curUnit->orders->Delete(ord);
   if(!neword)return;
   curUnit->orders->Add(neword);
  }
  if(!curUnit->IsLocal()){
   if(ProcessNonLocalOrders(neword))
    curUnit->orders->Delete(neword);
  }
 }else{
  if(!neword)return;
  if(ord!=neword){
   if(!curUnit->orders->Replace(ord,neword))delete neword;
  }
 }
 ViewShow();
}
bool TMainForm::ProcessNonLocalOrders(AOrder *ord)
{
 if(ord->type==O_STUDY){
  AOrderAbr *_ord=dynamic_cast<AOrderAbr*>(ord);
  AnsiString type=_ord->abr;
  ASkill *sk=curUnit->skills->GetSkill(type);
  if(!sk){
   sk=new ASkill;
   sk->type=type;
   curUnit->skills->Add(sk);
  }
 // if(val>450) val=450;
  int lev=sk->GetLevel();
  do{
   sk->days+=30;
  }while(lev>=sk->GetLevel());
  curUnit->hasskills=true;
  return true;
 }
 if(ord->type==O_FORGET){
  AOrderAbr *_ord=dynamic_cast<AOrderAbr*>(ord);
  AnsiString type=_ord->abr;
  ASkill *sk=curUnit->skills->GetSkill(type);
  if(sk){
   sk->days=0;
   curUnit->skills->DeleteEmpty();
  }
  return true;
 }
 return false;
}

void __fastcall TMainForm::miAddOrderCustomClick(TObject *Sender)
{
 EditOrder(0,new AOrderCustom);
}
void __fastcall TMainForm::miAddOrderCommentClick(TObject *Sender)
{
 EditOrder(0,new AOrderComment);
}
void __fastcall TMainForm::miAddOrderLocDescrClick(TObject *Sender)
{
 EditOrder(0,new AOrderLocDescr);
}

void __fastcall TMainForm::TreeViewEnter(TObject *Sender)
{
/* TTreeNode *nod=TreeView->Selected;
 TreeView->Selected=0;
 TreeView->Selected=nod;*/
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miFacClearErrorsClick(TObject *Sender)
{
 AFaction *fac=curFactions->GetFaction(popupFactionMessages->Tag);
 if(!fac)return;
 fac->errors->Clear();
 MakeTreeView();
}
void __fastcall TMainForm::miFacClearBattlesClick(TObject *Sender)
{
 AFaction *fac=curFactions->GetFaction(popupFactionMessages->Tag);
 if(!fac)return;
 fac->battles->Clear();
 MakeTreeView();
}
void __fastcall TMainForm::miFacClearEventsClick(TObject *Sender)
{
 int facnum=popupFactionMessages->Tag;
 if(facnum>0){
  AFaction *fac=curFactions->GetFaction(facnum);
  if(!fac)return;
  fac->events->Clear();
  MakeTreeView();
 }else{
  AFaction *fac=curFactions->GetFaction(-facnum);
  if(!fac)return;
  int ind=View->ItemIndex;
  if(ind<=0)return;
  AnsiString str=View->Items->Strings[ind+1];
  int pos=str.Pos(')');
  if(!pos) return;
  str.SetLength(pos);
  for(int i=fac->events->Count-1;i>=0;i--){
   char* s=fac->events->Strings[i].c_str();
   if(MyCompare(s,str)) continue;
   fac->events->Delete(i);
  }
  if(fac->events->Count){
   int ind=View->ItemIndex;
   ViewShow();
   View->ItemIndex=ind;
  }else MakeTreeView();
 }
}
void __fastcall TMainForm::miFacClearErrorClick(TObject *Sender)
{
 AFaction *fac=curFactions->GetFaction(popupFactionMessages->Tag);
 if(!fac)return;
 int ind=View->ItemIndex;
 int tind=View->TopIndex;
 if(ind<=0)return;
 ind--;
/* AnsiString str=View->Items->Strings[ind];
 ind=fac->errors->IndexOf(str);
 if(ind==-1)return;*/
 fac->errors->Delete(ind);
 if(fac->errors->Count){
  int ind=View->ItemIndex;
  ViewShow();
  View->ItemIndex=ind;
  View->TopIndex=tind;
  ViewClick(this);
 }else MakeTreeView();
}
void __fastcall TMainForm::miFacClearBattleClick(TObject *Sender)
{
 AFaction *fac=curFactions->GetFaction(popupFactionMessages->Tag);
 if(!fac)return;
 int ind=View->ItemIndex;
 int tind=View->TopIndex;
 if(ind<=0)return;
 ind--;
/* AnsiString str=View->Items->Strings[ind];
 ind=fac->battles->IndexOf(str);
 if(ind==-1)return;*/
 fac->battles->Delete(ind);
 if(fac->battles->Count){
  int ind=View->ItemIndex;
  ViewShow();
  View->ItemIndex=ind;
  View->TopIndex=tind;
  ViewClick(this);
 }else MakeTreeView();
}
void __fastcall TMainForm::miFacClearEventClick(TObject *Sender)
{
 int facnum=popupFactionMessages->Tag;
 AFaction *fac=curFactions->GetFaction(abs(facnum));
 if(!fac)return;
 int ind=View->ItemIndex;
 int tind=View->TopIndex;
 if(ind<=0)return;
 if(facnum>0){
  ind--;
 }else{
  AnsiString str=View->Items->Strings[ind];
  ind=fac->events->IndexOf(str);
  if(ind<0)return;
 }
 fac->events->Delete(ind);
 if(fac->events->Count){
  int ind=View->ItemIndex;
  ViewShow();
  View->ItemIndex=ind;
  View->TopIndex=tind;
  ViewClick(this);
 }else MakeTreeView();
}
void __fastcall TMainForm::edFindUnitKeyPress(TObject *Sender, char &Key)
{
 if(Key==13){
  int num=atoi(edFindUnit->Text.c_str());
  AUnit *un;
  un=curRegion->GetUnit(num);
  if(!un){
 //  if(num<1)return;
   un=curRegionList->GetUnit(num);
   if(!un)return;
   ARegion *reg=un->baseobj->basereg;
   curRegionList->SetCurRegion(reg->xloc,reg->yloc,reg->zloc);
   MakingTree++;
   ChangeLevel(0);
   MakingTree--;
  }
  if(un){
   VTreeNode *vnode=FindTreeNode(tntUnit,num);
   if(vnode)TreeView->Selected=vnode->node;
  }
  edFindUnit->Text="";
  Key=0;
 }
}

void __fastcall TMainForm::miAddOrderFormNewClick(TObject *Sender)
{
 if(!curUnit)return;
 AObject *obj=curUnit->baseobj;
 if(!obj)return;
 AnsiString mes;
 int num;
 AnsiString name,localdescr,descr;
 bool guard,avoid,behind,autotax,hold,noaid;
 int reveal,consume;
 TFormNewUnitForm *frm=new TFormNewUnitForm(this);
 int res=frm->ShowModal();
 if(res==mrOk){
  num=atoi(frm->edAlias->Text.c_str());
  name=frm->edName->Text;
  localdescr=frm->edLocalDescr->Text;
  descr=frm->edDescr->Text;
  guard=frm->cbGuard->Checked;
  avoid=frm->cbAvoid->Checked;
  behind=frm->cbBehind->Checked;
  autotax=frm->cbAutoTax->Checked;
  hold=frm->cbHold->Checked;
  noaid=frm->cbNoAid->Checked;
  reveal=frm->rgReveal->ItemIndex;
  consume=frm->rgConsume->ItemIndex;
 }
 delete frm;
 if(res!=mrOk)
  return;
// if(!InputQuery("Form new unit","New unit alias",mes))return;
// num=atoi(mes.c_str());
 if(num<=0)
  return;
 num=-num;
 if(curRegion->GetUnit(num)){
  mes="Unit with alias ";
  mes+=-num;
  mes+=" already exist";
  Application->MessageBox(mes.c_str(),"Error creating unit",MB_OK);
  return;
 }
 AUnit *newun=new AUnit(curTurn);
 newun->num=num;
 newun->FormNew(curUnit);
 obj->units->Add(newun);
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_GUARD;
  ord->value=guard;
  newun->orders->Add(ord);
 }
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_AVOID;
  ord->value=avoid;
  newun->orders->Add(ord);
 }
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_REVEAL;
  ord->value=reveal;
  newun->orders->Add(ord);
 }
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_BEHIND;
  ord->value=behind;
  newun->orders->Add(ord);
 }
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_AUTOTAX;
  ord->value=autotax;
  newun->orders->Add(ord);
 }
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_HOLD;
  ord->value=hold;
  newun->orders->Add(ord);
 }
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_NOAID;
  ord->value=noaid;
  newun->orders->Add(ord);
 }
 {
  AOrderByte *ord=new AOrderByte;
  ord->type=O_CONSUME;
  ord->value=consume;
  newun->orders->Add(ord);
 }
 if(name.Length()){
  AOrderCustom *ord=new AOrderCustom;
  ord->text="name unit \""+name+"\"";
  newun->orders->Add(ord);
 }
 if(localdescr.Length()){
  AOrderLocDescr *ord=new AOrderLocDescr;
  ord->text=localdescr;
  newun->orders->Add(ord);
 }
 if(descr.Length()){
  AOrderCustom *ord=new AOrderCustom;
  ord->text="DESCRIBE UNIT \""+descr+"\"";
  newun->orders->Add(ord);
 }
 MakingTree++;
 OnSetCurRegion();
 MakingTree--;
 VTreeNode *vnode=FindTreeNode(tntUnit,num);
 if(vnode)TreeView->Selected=vnode->node;
}
void __fastcall TMainForm::miDeleteNewUnitClick(TObject *Sender)
{
 if(!curUnit)return;
 if(curUnit->num>0)return;
 AnsiString mes;
 mes="You are sure to delete this unit?\n";
 mes+=curUnit->FullName();
 if(Application->MessageBox(mes.c_str(),"Deleting order",MB_YESNO)!=IDYES)return;
 curUnit->baseobj->units->Delete(curUnit->num);
 curUnit=0;
 OnSetCurRegion();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miSetTaxIncClick(TObject *Sender)
{
 if(!curRegion)return;
 AnsiString mes=curRegion->taxincome;
 if(!InputQuery("Tax income","Enter tax producivity (0 - auto)",mes))return;
 curRegion->taxincome=atoi(mes.c_str());
 ViewShow();
}
void __fastcall TMainForm::miSetWorkIncClick(TObject *Sender)
{
 if(!curRegion)return;
 AnsiString mes=curRegion->workincome;
 if(!InputQuery("Work income","Enter work producivity (0 - auto)",mes))return;
 curRegion->workincome=atoi(mes.c_str());
 ViewShow();
}
void __fastcall TMainForm::miSetEntIncClick(TObject *Sender)
{
 if(!curRegion)return;
 AnsiString mes=curRegion->entincome;
 if(!InputQuery("Entartainment income","Enter entertainment producivity (0 - auto)",mes))return;
 curRegion->entincome=atoi(mes.c_str());
 ViewShow();
}

void __fastcall TMainForm::popupViewRegionPopup(TObject *Sender)
{
 AnsiString s="&Goto last info turn (";
// int lastturn=curRegion->oldexitsinfo;
 int lastturn=curRegion->oldinfoturnnum;
 s+=lastturn;
 s+=")";
 miGotoLastInfo->Visible=(lastturn>=0);
 miGotoLastInfo->Caption=s;
 for(int i=0;i<12;i++){
  TMenuItem *mi=miWeather->Items[i];
  int newturn=curTurn->num+i+1;
  AnsiString s="Turn ";
  s+=newturn;
  s+=": ";
  s+=TurnNumberToString(newturn);
  s+=": "+WeatherRegions->GetWeather(curRegion->xloc,curRegion->yloc,
   curRegion->zloc,newturn);
  mi->Caption=s;
 }
 miCopyToClipboard4->Visible=popupViewRegion->PopupComponent==View;
}
void __fastcall TMainForm::miAddOrderSimpleClick(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 if(!mi)return;
 if(mi->Tag==0)return;
 AOrderSimple *ord=new AOrderSimple;
 ord->type=OrderTypes(mi->Tag);
 EditOrder(0,ord);
}
void __fastcall TMainForm::miAddOrderByteClick(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 if(!mi)return;
 if(mi->Tag<=0)return;
 AOrderByte *ord=new AOrderByte;
 ord->type=OrderTypes(mi->Tag);
 EditOrder(0,ord);
}
void __fastcall TMainForm::miAddOrderAbrClick(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 if(!mi)return;
 if(mi->Tag<=0)return;
 AOrderAbr *ord=new AOrderAbr;
 ord->type=OrderTypes(mi->Tag);
 EditOrder(0,ord);
}
void __fastcall TMainForm::miAddOrderInt(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 if(!mi)return;
 if(mi->Tag<=0)return;
 AOrderInt *ord=new AOrderInt;
 ord->type=OrderTypes(mi->Tag);
 EditOrder(0,ord);
}

void __fastcall TMainForm::miAddOrderGiveClick(TObject *Sender)
{
 EditOrder(0,new AOrderGive);
}
void __fastcall TMainForm::miAddOrderAutoGiveClick(TObject *Sender)
{
 EditOrder(0,new AOrderAutoGive);
}
void __fastcall TMainForm::miAddOrderBuySellClick(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 if(!mi)return;
 if(mi->Tag<=0)return;
 AOrderBuySell *ord=new AOrderBuySell;
 ord->type=OrderTypes(mi->Tag);
 EditOrder(0,ord);
}
void __fastcall TMainForm::miAddOrderIntClick(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 if(!mi)return;
 if(mi->Tag==0)return;
 AOrderInt *ord=new AOrderInt;
 ord->type=OrderTypes(mi->Tag);
 EditOrder(0,ord);
}

void __fastcall TMainForm::miAddOrderMovingClick(TObject *Sender)
{
 TMenuItem *mi=dynamic_cast<TMenuItem*>(Sender);
 if(!mi)return;
 if(mi->Tag==0)return;
 AOrderMoving *ord=new AOrderMoving;
 ord->type=OrderTypes(mi->Tag);
 EditOrder(0,ord);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::bnRunOrdersClick(TObject *Sender)
{
 curTurn->RunOrders();
}
void __fastcall TMainForm::bnRunAutoOrdersClick(TObject *Sender)
{
 curTurn->RunAutoOrders();
 ViewShow();
}


void __fastcall TMainForm::miGotoOrderTargetClick(TObject *Sender)
{
 if(!curUnit)return;
 int ind=popupViewUnit->Tag;
 AOrder *ord=curUnit->GetOrder(ind);
 if(!ord)return;
 int num=0;
 if(ord->type==O_MESSAGE){
  AOrderMessage *o=dynamic_cast<AOrderMessage*>(ord);
  if(o->target) num=o->target->num;
 }else if(ord->type==O_GIVE){
  AOrderGive *o=dynamic_cast<AOrderGive*>(ord);
  num=o->tarnum; 
 }
 if(!num)return;
 VTreeNode *vnode=FindTreeNode(tntUnit,num);
 if(vnode)TreeView->Selected=vnode->node;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::edTurnNumberKeyPress(TObject *Sender, char &Key)
{
 if(Key==13){
  AnsiString str=edTurnNumber->Text;
  if(!str.Length())return;
  int i=atoi(str.c_str());
  i-=curTurnNumber;
  if(!i)return;
  ChangeTurn(i);
  View->SetFocus();
  edTurnNumber->Text="";
  Key=0;
 }
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::miAddOrderTeachClick(TObject *Sender)
{
 EditOrder(0,new AOrderTeach);
}
//---------------------------------------------------------------------------


void TMainForm::ShowMapMode()
{
/* bnTownMode->Down=(MapMode==0);
 bnProdMode->Down=(MapMode==1);
*/
}

void __fastcall TMainForm::bnTownModeClick(TObject *Sender)
{
 MapMode=0;
 ShowMapMode();
 DrawMap();
}
void __fastcall TMainForm::bnProdModeClick(TObject *Sender)
{
 MapMode=1;
 ShowMapMode();
 DrawMap();
}
void __fastcall TMainForm::bnWedgeModeClick(TObject *Sender)
{
 MapMode=2;
 ShowMapMode();
 DrawMap();

}

void __fastcall TMainForm::miProdModeSetupClick(TObject *Sender)
{
 TProdModeSetupForm *frm=new TProdModeSetupForm(this,ProdModeAttr);
 if(frm->ShowModal()==mrOk){
  ProdModeAttr=frm->mode;
  DrawMap();
 }
 delete frm;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miWedgemodesettingsClick(TObject *Sender)
{
 TWedgeModeSetupForm *frm=new TWedgeModeSetupForm(this);
 if(frm->ShowModal()==mrOk){
  DrawMap();
 }
 delete frm;
}


void __fastcall TMainForm::miAdvProdsClick(TObject *Sender)
{
 AnsiString mes;
 bool added=false;
 int numreg=0;
 foreach(curRegionList){
  ARegion *reg=*(ARegion**)iter;
  if(!reg->hasInfo||reg->oldinfoturnnum!=reg->turn->num)
//  if(!(reg->infoStatus&isExitsInfo) ||(reg->oldexitsinfo!=curTurnNumber))
   continue;
  numreg++;
  ATerrainType *ttype=TerrainTypes->Get(reg->type);
  foreach(ItemTypes){
   AItemType *itype=*(AItemType**)iter;
   if(!ttype->CanProduceAdv(itype->abr)) continue;
   if(!(itype->type&IT_ADVANCED))continue;
   if(!itype->skill.Length())continue;
   if(itype->input.Length())continue;
   if(reg->products->GetMarket(itype->abr))continue;
   bool canmake=false;
   foreach(curFactions){
    AFaction *fac=*(AFaction**)iter;
    if(!fac->local) continue;
    if(!reg->CanMakeAdv(fac,itype->abr)) continue;
    canmake=true;
    break;
   }
   if(!canmake)continue;
   added=true;
   AMarket *mark=new AMarket;
   mark->type=itype->abr;
   reg->products->Add(mark);
   mes="Advanced product \"";
   mes+=itype->name;
   mes+="\" added in ";
   mes+=reg->FullName();
   AddEvent(mes);
  }
 }
 if(added){
  curTurn->VerifyNextTurn();
  DrawMap();
 }
 mes="Advanced products analyze ended. Scaned ";
 mes+=numreg;
 mes+=" regions.";
 AddEvent(mes);
}
void __fastcall TMainForm::miGameSaveClick(TObject *Sender)
{
 SaveGameBase();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miVerifynextturnClick(TObject *Sender)
{
 curTurn->VerifyNextTurn();
}
void __fastcall TMainForm::miMapSizeFixClick(TObject *Sender)
{
 foreach(curTurns){
  ATurn *turn=*(ATurn**)iter;
  if(!turn) continue;
  foreach(turn->RegionList){
   ARegion* reg=*(ARegion**)iter;
   ARegionArray *ra=turn->RegionList->GetRegionArray(reg->zloc);
   if(!ra) continue;
   ra->VerifySize(reg->xloc,reg->yloc);
  }
 }
 DrawMap();
 AddEvent("Map Size fixed");
}
void __fastcall TMainForm::miAdvancedProductsFixClick(TObject *Sender)
{
 AnsiString mes;
 foreach(curTurns){
  ATurn *turn=*(ATurn**)iter;
  if(!turn) continue;
  foreach(turn->RegionList){
   ARegion* reg=*(ARegion**)iter;
   ATerrainType *ttype=TerrainTypes->Get(reg->type);
   for(int i=reg->products->count-1;i>=0;i--){\
    AMarket*prod=reg->products->Get(i);
    if(prod->amount) continue;
    AItemType *itype=ItemTypes->Get(prod->type,4);
    if(ttype->CanProduceAdv(itype->abr)) continue;
    reg->products->Delete(i);
    mes="Null advanced product \"";
    mes+=itype->name;
    mes+="\" removed in ";
    mes+=reg->FullName();
    AddEvent(mes);
   }
  }
 }
 DrawMap();
 AddEvent("Advanced products fixed");
}

void __fastcall TMainForm::TreeViewCompare(TObject *Sender,
      TTreeNode *Node1, TTreeNode *Node2, int Data, int &Compare)
{
 int strcomp=Node1->Text.AnsiCompare(Node2->Text);
 VTreeNode *vn1=(VTreeNode*)(Node1->Data);
 if(!vn1) throw Exception("Error1 in TreeCompare");
 VTreeNode *vn2=(VTreeNode*)(Node2->Data);
 if(!vn2) throw Exception("Error2 in TreeCompare");
 if(vn1->type==tntObject){
  if(vn2->type==tntObject){
   Compare=vn1->value-vn2->value;
  }else{
   Compare=1;
  }
 }else if(vn2->type==tntObject){
  Compare=-1;
 }else if(vn1->type==tntUnit&&vn2->type==tntUnit){
  AUnit *un1=(AUnit*) vn1->data;
  if(!un1) throw Exception("Error3 in TreeCompare");
  AUnit *un2=(AUnit*) vn2->data;
  if(!un2) throw Exception("Error4 in TreeCompare");
  if(un1->IsLocal()){
   if(un2->IsLocal()){
    Compare=strcomp;
   }else{
    Compare=-1;
   }
  }else if(un2->IsLocal()){
   Compare=1;
  }else{
   Compare=strcomp;
  }
 }else{
  Compare=strcomp;
 }
}
void __fastcall TMainForm::miCopyOrderClick(TObject *Sender)
{
 if(!curUnit)return;
 int ind=popupViewUnit->Tag;
 AOrder *ord=curUnit->GetOrder(ind);
 if(!ord)return;
 orderBuf=ord->WriteTemplate();
}
void __fastcall TMainForm::miPasteOrderClick(TObject *Sender)
{
 AOrder *ord=ParseOrder(orderBuf);
 if(!ord) return;
 EditOrder(0, ord);
}
void __fastcall TMainForm::miPackUnpackClick(TObject *Sender)
{
 curTurns->StoreLastParam();
 curTurn->Packed=!curTurn->Packed;
 int num=curTurn->num;
 ATurn *t;
 if(num<=curTurns->curbeginturn)
  t=curTurns->Get(0);
 else
  t=curTurns->Get(num-1);
 if(t)
   t->VerifyNextTurn();
 curTurn=0;
 curTurns->SetCurTurn(num);
 ChangeTurn(0);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Exitwithoutsaving1Click(TObject *Sender)
{
 if(MessageDlg("Exit without saving\nYou are sure?",mtWarning,TMsgDlgButtons()<<mbYes<<mbNo,0)!=mrYes) return;
 NoSave=true;
 Close();
}

void __fastcall TMainForm::miPackTurnsClick(TObject *Sender)
{
 PackingForm=new TPackingForm(this);
 PackingForm->ShowModal();
 delete PackingForm; PackingForm=0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::VerTime1Click(TObject *Sender)
{
 int t1=GetTickCount();
 for(int i=0;i<10;i++)
  curTurns->Get(0)->VerifyNextTurn();
//  LoadGameBase();
 int t2=GetTickCount();
 VerGameLoaded();
 ChangeTurn(0);
 PanelMapResize(this);
 t2-=t1;
 Beep();
 ShowMessage(AnsiString(t2));

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actEnableTempExecute(TObject *Sender)
{
 miTemporary->Visible=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miVerSizeClick(TObject *Sender)
{
 if(!curTurns) return;
 int siz=curTurns->GetSize();
 AnsiString str="Size ";
 str+=siz;
 str+=" bytes";
 Application->MessageBox(str.c_str(),"",MB_OK);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miAddOrderBuildClick(TObject *Sender)
{
 EditOrder(0,new AOrderBuild);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miGotoLastInfoClick(TObject *Sender)
{
 if(!curRegion) return;
 if(curRegion->hasInfo&&curRegion->oldinfoturnnum==curTurn->num)
// if(curRegion->infoStatus&isExitsInfo&&curRegion->oldexitsinfo==curTurn->num)
  return;
/* ALocation loc(curRegion->xloc,curRegion->yloc,curRegion->zloc);
 int turnnum=curTurn->num-1;
 for(turnnum;turnnum>=0;turnnum--){
  ATurn *turn=curTurns->Get(turnnum);
  ARegion *reg=turn->RegionList->Get(loc);
  if(!reg) return;
  if(!reg->infoStatus&isRegionInfo) return;
  if(reg->oldreginfo==turnnum) break;
 }
 if(turnnum<0) return;*/
 ChangeTurn(curRegion->oldinfoturnnum-curTurn->num);
// ChangeTurn(curRegion->oldexitsinfo-curTurn->num);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UpdTurn0Click(TObject *Sender)
{
 if(curTurn->num==0) return;
 ATurn *t0=curTurns->Get(0);
 t0->Update0(curTurn);
/* ARegion *reg0=t0->RegionList->Get(curRegion->xloc,curRegion->yloc,curRegion->zloc);
 if(!reg0){
  reg0=new ARegion(t0);
  reg0->xloc=curRegion->xloc;
  reg0->yloc=curRegion->yloc;
  reg0->zloc=curRegion->zloc;
  t0->RegionList->Add(reg0);
  ARegionArray *ra=t0->RegionList->GetRegionArray(reg0->zloc);
  ra->VerifySize(reg0->xloc, reg0->yloc);
 }
 reg0->Update0(curRegion);*/
 t0->VerifyNextTurn();
// ChangeTurn(-curTurnNumber);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miCompactTurnClick(TObject *Sender)
{
 curTurn->Compact();
 ChangeTurn(0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miStoreTurnsClick(TObject *Sender)
{
 TStoreForm *frm=new TStoreForm(this);
 int res=frm->ShowModal();
 delete frm;
 if(res!=mrOk) return;
 SaveGameBase();
 LoadGameBase();
 VerGameLoaded();
 ChangeTurn(0);
}
void __fastcall TMainForm::actShowRegInfoExecute(TObject *Sender)
{
 bool ch=!actShowRegInfo->Checked;
 actShowRegInfo->Checked=ch;
 RegInfoForm->ShowRegInfo=ch;
}
void __fastcall TMainForm::actShowMapExecute(TObject *Sender)
{
 bool ch=!actShowMap->Checked;
 actShowMap->Checked=ch;
 MapForm->ShowMap=ch;
}

void __fastcall TMainForm::miSaveReportClick(TObject *Sender)
{
 if(curTurn->Packed){
  AnsiString str="Turn ";
  str+=curTurn->num;
  str+=" is PACKED";
  AddEvent("Error: "+str);
  Application->MessageBox(str.c_str(),"ERROR",MB_OK);
  return;
 }
 foreach(curFactions){
  AFaction *fac=*(AFaction**)iter;
//  if(fac->number!=198) continue; //temp
  if(!fac->local)continue;
  int facnum=fac->number;
  AnsiString fName=GetReportName(facnum);
  if(!SaveReport(fName,facnum)){
   AnsiString str="Error saving report for faction ";
   str+=fac->FullName();
   AddEvent(str);
   break;
  }
 }
}
void __fastcall TMainForm::miSaveMapClick(TObject *Sender)
{
 if(curTurn->Packed){
  AnsiString str="Turn ";
  str+=curTurn->num;
  str+=" is PACKED";
  AddEvent("Error: "+str);
  Application->MessageBox(str.c_str(),"ERROR",MB_OK);
  return;
 }
 {
  TSaveMapForm *frm=new TSaveMapForm(this);
  if(frm->ShowModal()!=mrOk) return;
  delete frm;
 }
 AnsiString fName=GetGameFileName(ReportsDir+"map."+curTurn->num);
/* int mapoptions=0
//                |roLocalAsNonlocalUnit
//                |roNonlocalUnit
//                |roUserObject
                |roNonuserObject
                |roAllRegion
                |roRegionInfo
 ;*/
 if(!SaveMap(fName,optSaveMap)){
  AnsiString str="Error saving map";
  AddEvent(str);
 }
}
void __fastcall TMainForm::miRepackStoreClick(TObject *Sender)
{
 if(curTurns) curTurns->RepackStore();  
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::miAtlaCartClick(TObject *Sender)
{
 OpenDialog->Filter="All files|*.*";
 if(OpenDialog->Execute()){
  OpenDialog->InitialDir=ExtractFilePath(OpenDialog->FileName);
  bool ok=true;
  TStringList *list=new TStringList;
  list->Assign(OpenDialog->Files);
  list->Sort();
  for(int i=0;ok&&i<list->Count;i++){
   if(!ImportAtlaCart(list->Strings[i])) ok=false;
  }
  delete list;
  ChangeTurn(0);
  if(ok) curTurn->VerifyNextTurn();
 }
}
//---------------------------------------------------------------------------
void LoadIniColor(TColor &color,AnsiString paramname,TIniFile *ini){
 AnsiString str=ini->ReadString(SettingsSect,paramname,"");
 if(!str.Length()) return;
 try{
  color=StringToColor(str);
 }catch(...){}
}
void TMainForm::LoadOptions()
{
 TIniFile *ini=new TIniFile(IniName);
 try{
  actShowMap->Checked=atoi(ini->ReadString(SettingsSect,"ShowMap","").c_str());
  actShowRegInfo->Checked=atoi(ini->ReadString(SettingsSect,"ShowRegInfo","").c_str());
  optShowEvents=atoi(ini->ReadString(SettingsSect,"ShowEvents","1").c_str());
  optRunRegionOrders=atoi(ini->ReadString(SettingsSect,"RunRegionOrders","1").c_str());
  optTownNameLimit=atoi(ini->ReadString(SettingsSect,"TownNameLimit","0").c_str());
  optMaximizeWindow=atoi(ini->ReadString(SettingsSect,"MaximizeWindow","1").c_str());
  WindowState=optMaximizeWindow?wsMaximized:wsNormal;
  optDontShowEvents=atoi(ini->ReadString(SettingsSect,"DontShowEvents","4").c_str());
  optSaveMap=atoi(ini->ReadString(SettingsSect,"SaveMap","0").c_str());

  LoadIniColor(curregColor,"curregColor",ini);
  LoadIniColor(wallColor,"wallColor",ini);
  LoadIniColor(gridColor,"gridColor",ini);
  LoadIniColor(textColor,"textColor",ini);
  for(int i=0;i<6;i++){
   LoadIniColor(wedgecolors[i],AnsiString("wedgeColor")+i,ini);
  }
 }__finally{
  delete ini;
 }
}
void SaveIniColor(TColor color,AnsiString paramname,TIniFile *ini){
 ini->WriteString(SettingsSect,paramname,ColorToString(color));
}
void TMainForm::SaveOptions()
{
 TIniFile *ini=new TIniFile(IniName);
 try{
  ini->WriteString(SettingsSect,"ShowMap",int(actShowMap->Checked));
  ini->WriteString(SettingsSect,"ShowRegInfo",int(actShowRegInfo->Checked));
  ini->WriteString(SettingsSect,"ShowEvents",int(optShowEvents));
  ini->WriteString(SettingsSect,"RunRegionOrders",int(optRunRegionOrders));
  ini->WriteString(SettingsSect,"TownNameLimit",int(optTownNameLimit));
  ini->WriteString(SettingsSect,"MaximizeWindow",int(optMaximizeWindow));
  ini->WriteString(SettingsSect,"DontShowEvents",optDontShowEvents);
  ini->WriteString(SettingsSect,"SaveMap",optSaveMap);

  SaveIniColor(curregColor,"curregColor",ini);
  SaveIniColor(wallColor,"wallColor",ini);
  SaveIniColor(gridColor,"gridColor",ini);
  SaveIniColor(textColor,"textColor",ini);
  for(int i=0;i<6;i++){
   SaveIniColor(wedgecolors[i],AnsiString("wedgeColor")+i,ini);
  }
 }__finally{
  delete ini;
 }
}
void __fastcall TMainForm::miOptionsClick(TObject *Sender)
{
 TOptionsForm *frm=new TOptionsForm(this);
 frm->ShowModal();
 delete frm;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miReceiveClick(TObject *Sender)
{
 TEditOrderReceiveForm *frm=new TEditOrderReceiveForm(this);
 int ret=frm->ShowModal();
 delete frm;
 if(ret==mrOk){
  ViewShow();
 }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miAboutClick(TObject *Sender)
{
 TAboutForm *frm=new TAboutForm(this);
 frm->ShowModal();
 delete frm;
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::actPrevUnitExecute(TObject *Sender)
{
 if(!curNode)return;
 VTreeNode *vnode;
 TTreeNode *findNode=curNode;
 bool needprev=true;
 do{
  while(findNode){
   if(needprev){
    findNode=findNode->GetPrev();
   }
   needprev=true;
   if(!findNode) continue;
   vnode=(VTreeNode*)findNode->Data;
   if(vnode->type!=tntUnit) continue;
   AUnit *unit=(AUnit*) vnode->data;
   if(!unit->IsLocal()) continue;
   TreeView->Selected=findNode;
   return;
  }
  if(!findNode){
   for(int i=curRegion->num-1;i>=0;i--){
    ARegion *reg=curRegionList->Get(i);
    if(!reg->PresentLocals()) continue;
    curRegionList->SetCurRegion(reg->xloc,reg->yloc,reg->zloc);
    xcentr=curRegion->xloc;
    ycentr=curRegion->yloc;
    zcentr=curRegion->zloc;
    OnSetCurRegion();
//    findNode=TreeView->Items->GetFirstNode();
    findNode=TreeView->Items->Item[TreeView->Items->Count-1];
    needprev=false;

    break;
   }
  }
 }while(findNode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actNextUnitExecute(TObject *Sender)
{
 if(!curNode)return;
 VTreeNode *vnode;
 TTreeNode *findNode=curNode;
 do{
  while(findNode){
   findNode=findNode->GetNext();
   if(!findNode) continue;
   vnode=(VTreeNode*)findNode->Data;
   if(vnode->type!=tntUnit) continue;
   AUnit *unit=(AUnit*) vnode->data;
   if(!unit->IsLocal()) continue;
   TreeView->Selected=findNode;
   return;
  }
  if(!findNode){
   for(int i=curRegion->num+1,endi=curRegionList->count-1;i<endi;i++){
    ARegion *reg=curRegionList->Get(i);
    if(!reg->PresentLocals()) continue;
    curRegionList->SetCurRegion(reg->xloc,reg->yloc,reg->zloc);
    xcentr=curRegion->xloc;
    ycentr=curRegion->yloc;
    zcentr=curRegion->zloc;
    OnSetCurRegion();
    findNode=TreeView->Items->GetFirstNode();
    break;
   }
  }
 }while(findNode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miSavereginfoClick(TObject *Sender)
{
 int x=curRegion->xloc,y=curRegion->yloc,z=curRegion->zloc;
 FILE *f=fopen("atlaclient.log","w");
 if(!f) return;
 for(int i=13,endi=curTurns->count;i<endi;i++){
  bool packed=false;
  ATurn *turn=new ATurn;
  turn->num=i;
  turn->Read();
  if(turn->num==-1){
    packed=true;
    turn->Packed=true;
    turn->num=i;
    AnsiString fName=turn->GetFileName(1);
    if(!CopyFiles(GetGameFileName(StoreDir+fName),GetGameFileName(fName)))
/*     throw 1;*/
     turn->Packed=false;
    else turn->num=-1;
  }
  if(turn->num==i){
   ARegion *reg=turn->RegionList->Get(x,y,z);
   if(reg&&reg->population){
    fprintf(f,"%d",reg->population);
    fprintf(f,",%d",reg->wanteds->GetNum("GRAI"));
    fprintf(f,",%d",reg->wanteds->GetNum("LIVE"));
    fprintf(f,",%d",reg->wanteds->GetNum("HERB"));
    fprintf(f,",%d",reg->wanteds->GetNum("BAXE"));
    fprintf(f,",%d",reg->wanteds->GetNum("PARM"));
    fprintf(f,",%d",reg->wanteds->GetNum("LARM"));
    fprintf(f,",%d",reg->wanteds->GetNum("JEWE"));
    fprintf(f,",%d",reg->wanteds->GetNum("CAVI"));
    fprintf(f,"\n");
   }
  }
  if(packed){
   DeleteFiles(GetGameFileName(turn->GetFileName(1)));
  }
  delete turn;
 }
 fclose(f);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actCopyToClipboardClick(TObject *Sender)
{
 int ind=View->ItemIndex;
 AnsiString str;
 if(ind>=0)
  str=View->Items->Strings[ind];
 TClipboard *clp=Clipboard();
 clp->AsText=str;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::popupFactionMessagesPopup(TObject *Sender)
{
 miCopyToClipboard3->Visible=popupFactionMessages->PopupComponent==View;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::popupViewDefaultPopup(TObject *Sender)
{
 miCopyToClipboard0->Visible=popupViewDefault->PopupComponent==View;
}
void __fastcall TMainForm::miMoveOrderUpClick(TObject *Sender)
{
 int ind=popupViewUnit->Tag;
 if(curUnit->orders->Swap(ind, ind-1))
  ViewShow();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miMoveOrderDownClick(TObject *Sender)
{
 int ind=popupViewUnit->Tag;
 if(curUnit->orders->Swap(ind, ind+1))
  ViewShow();
}
//---------------------------------------------------------------------------

