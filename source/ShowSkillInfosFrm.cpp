//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ShowSkillInfosFrm.h"
#include "util.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TShowSkillInfosForm *ShowSkillInfosForm;
//---------------------------------------------------------------------------
__fastcall TShowSkillInfosForm::TShowSkillInfosForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TShowSkillInfosForm::FormCreate(TObject *Sender)
{
 if(!SkillInfos)return;
 int maxsize=0;
 List->Sorted=true; 
 foreach(SkillInfos){
  ASkillInfo*si=*(ASkillInfo**)iter;
  AnsiString str=si->Print();
  List->Items->Add(str);
  TSize siz=List->Canvas->TextExtent(str);
  if(siz.cx>maxsize)maxsize=siz.cx;
 }
 List->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
}
//---------------------------------------------------------------------------
void __fastcall TShowSkillInfosForm::FormShow(TObject *Sender)
{
 if(!SkillInfos)ModalResult=mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TShowSkillInfosForm::bnEditClick(TObject *Sender)
{
/* int num=List->ItemIndex;
 if(num==-1)return;
 AItemType*stype=SkillInfos->Get(num);
// int num2=num+1;
 if(num2>=SkillInfos->count)num2=0;
 AItemType*stype2=SkillInfos->Get(num2);//
 TEditItemTypeForm *frm=new TEditItemTypeForm(this,stype);
 if(frm->ShowModal()==mrOk){
  AnsiString str=stype->Print();
  List->Items->Strings[num]=str;
  int maxsize=List->Perform(LB_GETHORIZONTALEXTENT,0,0)-5;
  TSize siz=List->Canvas->TextExtent(str);
  if(siz.cx>maxsize)maxsize=siz.cx;
  List->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
 }
 delete frm;*/
}
//---------------------------------------------------------------------------

void __fastcall TShowSkillInfosForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 Action=caFree;
}
//---------------------------------------------------------------------------

void __fastcall TShowSkillInfosForm::bnCancelClick(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------

