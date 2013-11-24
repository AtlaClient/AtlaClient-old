//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <inifiles.hpp>
#include <stdlib.h>
#include "RegInfoFrm.h"
#include "util.h"
#include "region.h"
#include "markets.h"
#include "MainFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRegInfoForm *RegInfoForm;
static AnsiString IniSect="RegInfoWin";

//---------------------------------------------------------------------------
__fastcall TRegInfoForm::TRegInfoForm(TComponent* Owner)
        : TForm(Owner),oxloc(-1)
{
 TIniFile *ini=new TIniFile(IniName);
 Left=atoi(ini->ReadString(IniSect,"Left",Left).c_str());
 Top=atoi(ini->ReadString(IniSect,"Top",Top).c_str());
 Width=atoi(ini->ReadString(IniSect,"Width",Width).c_str());
 Height=atoi(ini->ReadString(IniSect,"Height",Height).c_str());
 delete ini;
}
//---------------------------------------------------------------------------
void TRegInfoForm::UpdateRegionInfo(int xloc,int yloc, int zloc)
{
 if(!ShowRegInfo) return;
 if(oxloc==xloc&&oyloc==yloc&&ozloc==zloc) return;
 oxloc=xloc;oyloc=yloc;ozloc=zloc;
 ARegion *reg=curRegionList->Get(xloc,yloc,zloc);
 if(!reg){
  Memo->Lines->Clear();
  Memo->Height=10;Memo->Width=10;
  return;
 }
 Memo->Lines->BeginUpdate();
 Memo->Lines->Clear();
 int h=0,w=0;
 AnsiString s;


 s=reg->FullName();
 if(!reg->hasInfo||reg->oldinfoturnnum!=curTurnNumber){
  s="?"+s;
 }
 AddLine(s,w,h);
// h*=2; //для пустой строки в конце

 if(reg->hasInfo){
// if(reg->infoStatus&isRegionInfo){
  s="";
//  if(reg->oldreginfo!=curTurnNumber)s+="?";
  s+=reg->population;
  s+=" peasants";
  if(reg->population){
   s+=" ("+reg->race+")";
   AddLine(s,w,h);
   s="Tax: $"; s+=reg->money;
   AddLine(s,w,h);
// }
// if(reg->infoStatus&isWagesInfo){
   s="Wages: $"; s+=reg->wages;
   if(reg->wages){
    s+=" (Max: $"; s+=reg->maxwages; s+=")";
   }
   AddLine(s,w,h);
// }
// if(reg->infoStatus&isEntertainInfo){
   s="Entertainment: $"; s+=reg->entertainment;
  }
  AddLine(s,w,h);
 }
// {
  s="";
  AnsiString weat=WeatherRegions->GetWeather(xloc,yloc,zloc,curTurnNumber);
  AnsiString nxtweat=WeatherRegions->GetWeather(xloc,yloc,zloc,curTurnNumber+1);
  if(/*turn->num&&*/reg->hasInfo&&reg->oldinfoturnnum==curTurnNumber){
//  if(reg->infoStatus&isWeatherInfo){
   if(weat!=reg->weather) s+="!";
   s+=WeatherTypes->Get(reg->weather)->curmonthname;
   AddLine(s,w,h);
   s=" ";
   if(nxtweat!=reg->nxtweather) s+="!";
   s+=WeatherTypes->Get(reg->nxtweather)->nextmonthname;
  }else{
//   s+="?";
   s+=WeatherTypes->Get(weat)->curmonthname;
   AddLine(s,w,h);
   s=" "+WeatherTypes->Get(nxtweat)->nextmonthname;
  }
  AddLine(s,w,h);
// }
 if(reg->hasInfo){
  if(reg->gate){
 // if(reg->infoStatus&isGateInfo){
   s=KeyPhrases->Get(keyGatesSign);
   s+=reg->gate;
   s+=KeyPhrases->Get(keyGatesSign2);
   s+=curRegionList->numberofgates;
   s+=").";
   AddLine(s,w,h);
  }
// if(reg->infoStatus&isProductInfo){
  s="Products:";
  AddLine(s,w,h);
  foreach(reg->products){
   AMarket *mark=*(AMarket**) iter;
   s=" "+mark->Print(3);
   AddLine(s,w,h);
  }
 }

 Memo->Lines->EndUpdate();
 Memo->Width=w+5;
 Memo->Height=h+5;
 if(!Visible) Show();
}


void __fastcall TRegInfoForm::SetShowRegInfo(bool value)
{
 if(FShowRegInfo == value) return;
 FShowRegInfo = value;
 if(value){
  if(curRegion){
   UpdateRegionInfo(curRegion->xloc,curRegion->yloc,curRegion->zloc);
  }
 }else{
  oxloc=-1;
  Hide();
 }
}
void __fastcall TRegInfoForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 MainForm->actShowRegInfo->Checked=false;
 ShowRegInfo=false;
 Action=caHide;
}
//---------------------------------------------------------------------------


void TRegInfoForm::AddLine(const AnsiString & str, int & w, int & h)
{
 TSize sz=Canvas->TextExtent(str);
 h+=sz.cy;
 if(w<sz.cx) w=sz.cx;
 Memo->Lines->Add(str);
}
void __fastcall TRegInfoForm::FormDestroy(TObject *Sender)
{
 TIniFile *ini=new TIniFile(IniName);
 ini->WriteString(IniSect,"Left",Left);
 ini->WriteString(IniSect,"Top",Top);
 ini->WriteString(IniSect,"Width",Width);
 ini->WriteString(IniSect,"Height",Height);
 delete ini;
}
//---------------------------------------------------------------------------
