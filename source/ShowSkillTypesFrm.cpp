//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ShowSkillTypesFrm.h"
#include "util.h"
#include "EditSkillTypeFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TShowSkillTypesForm *ShowSkillTypesForm;
//---------------------------------------------------------------------------
__fastcall TShowSkillTypesForm::TShowSkillTypesForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TShowSkillTypesForm::FormCreate(TObject *Sender)
{
 if(!SkillTypes)return;
 int maxsize=0;
 foreach(SkillTypes){
  ASkillType*ttype=*(ASkillType**)iter;
  AnsiString str=ttype->Print();
  List->Items->Add(str);
  TSize siz=List->Canvas->TextExtent(str);
  if(siz.cx>maxsize)maxsize=siz.cx;
 }
 List->Perform(LB_SETHORIZONTALEXTENT,maxsize+5,0);
}
//---------------------------------------------------------------------------
void __fastcall TShowSkillTypesForm::FormShow(TObject *Sender)
{
 if(!SkillTypes)ModalResult=mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TShowSkillTypesForm::bnEditClick(TObject *Sender)
{
 int num=List->ItemIndex;
 if(num==-1)return;
 ASkillType*stype=SkillTypes->Get(num);
/* int num2=num+1;
 if(num2>=SkillTypes->count)num2=0;
 ASkillType*stype2=SkillTypes->Get(num2);*/
 TEditSkillTypeForm *frm=new TEditSkillTypeForm(this,stype);
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
//---------------------------------------------------------------------------

void __fastcall TShowSkillTypesForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 Action=caFree;
}
//---------------------------------------------------------------------------

void __fastcall TShowSkillTypesForm::bnCancelClick(TObject *Sender)
{
 Close(); 
}
//---------------------------------------------------------------------------

