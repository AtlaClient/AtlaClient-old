//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include "util.h"
#include "MapFrm.h"
#include "region.h"
#include "MainFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMapForm *MapForm;
static AnsiString IniSect="MapWin";
//---------------------------------------------------------------------------
__fastcall TMapForm::TMapForm(TComponent* Owner)
        : TForm(Owner),oldzloc(-1)
{
 TIniFile *ini=new TIniFile(IniName);
 Left=atoi(ini->ReadString(IniSect,"Left",Left).c_str());
 Top=atoi(ini->ReadString(IniSect,"Top",Top).c_str());
 delete ini;
}
void __fastcall TMapForm::FormDestroy(TObject *Sender)
{
 TIniFile *ini=new TIniFile(IniName);
 ini->WriteString(IniSect,"Left",Left);
 ini->WriteString(IniSect,"Top",Top);
 delete ini;
}
//---------------------------------------------------------------------------
void TMapForm::UpdateMap(int zloc)
{
 if(!ShowMap) return;
 if(zloc==oldzloc) return;
 CreateMap(zloc);
}
void TMapForm::ResetMap()
{
 oldzloc=-1;
}


void TMapForm::CreateMap(int zloc)
{
 ARegionArray *ra=curRegionList->RegionArrays->Get(zloc);
 if(!ra) return;
 oldzloc=zloc;
 int ex=ra->x,ey=ra->y;
 if(zloc==0||ex==1){
  Hide();
  return;
 }
// Caption="Map "+ra->name;
 FMul=2;
 if(ex*FMul<120) FMul*=2;
 int mul=FMul;
 int w=ex*mul,h=(ey+1)*(mul/2);

 Image->Picture->Bitmap->Width=w;
 Image->Picture->Bitmap->Height=h;
 TCanvas *canv=Image->Canvas;
 TRect r(0,0,w+mul,h+mul);
 canv->Brush->Color=clWhite;
 canv->FillRect(r);
 for(int y=0;y<ey;y++){
  for(int x=0;x<ex;x++){
   if((x+y)&1)continue;
   ARegion *reg=curRegionList->Get(x,y,zloc);
   if(!reg) continue;
   ATerrainType *ttype=TerrainTypes->Get(reg->type);
   if(!ttype) return;
   r.right=(r.left=x*mul)+mul;
   r.bottom=(r.top=y*(mul/2))+mul;
   canv->Brush->Color=ttype->color;
   canv->FillRect(r);
  }
 }
 if(!Visible) Show();
}


void __fastcall TMapForm::SetShowMap(bool value)
{
 if(FShowMap== value) return;
 FShowMap = value;
 if(value){
  if(curRegion)
   UpdateMap(curRegion->zloc);
 }else{
  ResetMap();
  Hide();
 }
}
void __fastcall TMapForm::ImageMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 int xloc,yloc;
 PointToCoord(X,Y,xloc,yloc);
 ARegionArray *ra=curRegionList->RegionArrays->Get(oldzloc);
 if(!ra) return;
 ra->NormCoord(xloc,yloc);
 AnsiString mes;
 MainForm->xcentr=xloc;
 MainForm->ycentr=yloc;
 MainForm->DrawMap();
}
//---------------------------------------------------------------------------
void TMapForm::PointToCoord(int x, int y, int & xloc, int& yloc)
{
 int mul=FMul;
 xloc=x/mul;
 yloc=y/(mul/2);
}


void __fastcall TMapForm::SetMul(int value)
{
    FMul = value;
}
//---------------------------------------------------------------------------
void __fastcall TMapForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 MainForm->actShowMap->Checked=false;
 ShowMap=false;
 Action=caHide;
}
//---------------------------------------------------------------------------
void __fastcall TMapForm::ImageMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
 int xloc,yloc;
 PointToCoord(X,Y,xloc,yloc);
 MainForm->SetlbCoords(xloc,yloc); 
}
//---------------------------------------------------------------------------

