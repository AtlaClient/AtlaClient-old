//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "util.h"
#include "region.h"
#include "reports.h"
#include "LinkShaftFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLinkShaftForm *LinkShaftForm;
//---------------------------------------------------------------------------
__fastcall TLinkShaftForm::TLinkShaftForm(TComponent* Owner,AObject *_obj)
    : TForm(Owner),obj(_obj)
{
}
void __fastcall TLinkShaftForm::FormCreate(TObject *Sender)
{
 AnsiString str;
 int zloc=obj->basereg->zloc;
 List->Items->BeginUpdate();
 foreach(curRegionList){
  ARegion *reg=*(ARegion**)iter;
  if(reg->zloc==zloc)continue;
  if(!reg->RegionStatus->HasShaft)continue;
  str=reg->FullName()+".";
  if(reg->inner.xloc!=-1)str+=" (linked)";
  List->Items->Add(str);
 }
 List->Items->EndUpdate();
 if(obj->basereg->inner.xloc==-1)return;
 ARegion *reg=curRegionList->Get(obj->basereg->inner);
 str=reg->FullName()+".";
 if(reg->inner.xloc!=-1)str+=" (linked)";
 int ind=List->Items->IndexOf(str);
 List->ItemIndex=ind;
}
void __fastcall TLinkShaftForm::bnLinkClick(TObject *Sender)
{
 int ind=List->ItemIndex;
 if(ind<0)return;
 AnsiString str=List->Items->Strings[ind];
 ARegion *_reg=ProcessRegionHeader(str),*reg2,*reg;
 if(!_reg)return;
 reg=obj->basereg;
 if(reg->inner.xloc!=-1){
  if(reg->inner.xloc==_reg->xloc&&
     reg->inner.yloc==_reg->yloc&&
     reg->inner.zloc==_reg->zloc){
   ModalResult=mrOk;
   return;
  }
  reg2=curRegionList->Get(reg->inner);
  reg2->inner.xloc=-1;
 }
 reg->inner.xloc=_reg->xloc;
 reg->inner.yloc=_reg->yloc;
 reg->inner.zloc=_reg->zloc;
 delete _reg;
 reg2=curRegionList->Get(reg->inner);
 if(reg2->inner.xloc!=-1){
  ARegion *reg3=curRegionList->Get(reg2->inner);
  reg3->inner.xloc=-1;
 }

 reg2->inner.xloc=reg->xloc;
 reg2->inner.yloc=reg->yloc;
 reg2->inner.zloc=reg->zloc;

 ModalResult=mrOk;
}

