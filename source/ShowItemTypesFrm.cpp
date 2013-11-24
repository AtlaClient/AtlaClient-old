//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ShowItemTypesFrm.h"
#include "util.h"
#include "EditItemTypeFrm.h"
#include "unit.h"
#include "orders.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TShowItemTypesForm *ShowItemTypesForm;
//---------------------------------------------------------------------------
__fastcall TShowItemTypesForm::TShowItemTypesForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TShowItemTypesForm::FormCreate(TObject *Sender)
{
 if(!ItemTypes)return;
 int maxsize=0;
 foreach(ItemTypes){
  AItemType*ttype=*(AItemType**)iter;
  AnsiString str=ttype->Print();
  List->Items->Add(str);
  TSize siz=List->Canvas->TextExtent(str);
  if(siz.cx>maxsize)maxsize=siz.cx;
 }
 List->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
 bnRequery->Enabled=(curUnit!=0);
}
//---------------------------------------------------------------------------
void __fastcall TShowItemTypesForm::FormShow(TObject *Sender)
{
 if(!ItemTypes)ModalResult=mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TShowItemTypesForm::bnEditClick(TObject *Sender)
{
 int num=List->ItemIndex;
 if(num==-1)return;
 AItemType*stype=ItemTypes->Get(num);
/* int num2=num+1;
 if(num2>=ItemTypes->count)num2=0;
 AItemType*stype2=ItemTypes->Get(num2);*/
 TEditItemTypeForm *frm=new TEditItemTypeForm(this,stype);
 if(frm->ShowModal()==mrOk){
  AnsiString str=stype->Print();
  List->Items->Strings[num]=str;
  int maxsize=List->Perform(LB_GETHORIZONTALEXTENT,0,0)-5;
  TSize siz=List->Canvas->TextExtent(str);
  if(siz.cx>maxsize)maxsize=siz.cx;
  List->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
 }
 delete frm;
}
void __fastcall TShowItemTypesForm::bnRequeryClick(TObject *Sender)
{
 if(!curUnit)return;
 int num=List->ItemIndex;
 if(num==-1)return;
 AItemType *itype=ItemTypes->Get(num);
 if(ItemTypes->IsIllusionAbr(itype->abr)) return;
 AOrderCustom *ord=new AOrderCustom;
 ord->text="show item "+itype->abr;
 curUnit->orders->Add(ord);
}


void __fastcall TShowItemTypesForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 Action=caFree;
}
//---------------------------------------------------------------------------

void __fastcall TShowItemTypesForm::bnCancelClick(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------

