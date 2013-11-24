//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "util.h"
#include "EditItemTypeFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditItemTypeForm *EditItemTypeForm;
//---------------------------------------------------------------------------
__fastcall TEditItemTypeForm::TEditItemTypeForm(TComponent* Owner,AItemType*oldItem,AItemType*newItem)
    : TForm(Owner)
{
 FoldItem=oldItem;
 FnewItem=newItem;
 if(!oldItem)return;
 if(!ItemTypes)return;
 if(!SkillTypes) return;
}
//---------------------------------------------------------------------------
__fastcall TEditItemTypeForm::~TEditItemTypeForm()
{
}
void __fastcall TEditItemTypeForm::FormCreate(TObject *Sender)
{
 if(!FoldItem)return;
 if(!ItemTypes)return;
 if(!SkillTypes) return;
 TStringList*list=new TStringList;
 ItemTypes->MakeItemList(list);
 int ind=list->IndexOf(FoldItem->name);
 if(ind!=-1)list->Delete(ind);
 list->Sort();
 cbInput->Items->Clear();
 cbInput->Items->Add("None");
 cbInput->Items->AddStrings(list);
 cbInput->ItemIndex=0;
 
 cbHelpProd->Items->Clear();
 cbHelpProd->Items->Add("None");
 cbHelpProd->Items->AddStrings(list);
 cbHelpProd->ItemIndex=0;

 list->Clear();
 SkillTypes->MakeSkillList(list);
 list->Sort();
 cbSkill->Items->Clear();
 cbSkill->Items->Add("None");
 cbSkill->Items->AddStrings(list);
 delete list;

 Table->ColCount=22;
 Table->ColWidths[0]=25;
 Table->Cells[1][0]="Name";
 Table->ColWidths[1]=100;
 Table->Cells[2][0]="Names";
 Table->ColWidths[2]=100;
 Table->Cells[3][0]="Abr";
 Table->ColWidths[3]=35;
 Table->Cells[4][0]="!Give";
 Table->ColWidths[4]=35;
 Table->Cells[5][0]="Skill";
 Table->ColWidths[5]=80;
 Table->Cells[6][0]="Input";
 Table->ColWidths[6]=80;
 Table->Cells[7][0]="Weig";
 Table->ColWidths[7]=35;
 Table->Cells[ 8][0]="Norm";
 Table->ColWidths[ 8]=35;
 Table->Cells[ 9][0]="Adv";
 Table->ColWidths[ 9]=35;
 Table->Cells[10][0]="Trade";
 Table->ColWidths[10]=35;
 Table->Cells[11][0]="Man";
 Table->ColWidths[11]=35;
 Table->Cells[12][0]="Mon";
 Table->ColWidths[12]=35;
 Table->Cells[13][0]="Magic";
 Table->ColWidths[13]=35;
 Table->Cells[14][0]="Weap";
 Table->ColWidths[14]=35;
 Table->Cells[15][0]="Armor";
 Table->ColWidths[15]=35;
 Table->Cells[16][0]="Mount";
 Table->ColWidths[16]=35;
 Table->Cells[17][0]="Battle";
 Table->ColWidths[17]=35;
 Table->Cells[18][0]="Food";
 Table->ColWidths[18]=35;
 Table->Cells[19][0]="Walk";
 Table->ColWidths[19]=35;
 Table->Cells[20][0]="Ride";
 Table->ColWidths[20]=35;
 Table->Cells[21][0]="Fly";
 Table->ColWidths[21]=35;

 Table->Cells[1][1]=FoldItem->name;
 Table->Cells[2][1]=FoldItem->names;
 Table->Cells[3][1]=FoldItem->abr;
 Table->Cells[4][1]=FoldItem->flags&CANTGIVE?"   x":"";
 Table->Cells[5][1]=MakeDepSkillStr(FoldItem->skill,FoldItem->level);
 Table->Cells[6][1]=MakeInputItemStr(FoldItem->input,FoldItem->number);
 Table->Cells[7][1]=FoldItem->weight;
 Table->Cells[ 8][1]=FoldItem->type&IT_NORMAL?"   x":"";
 Table->Cells[ 9][1]=FoldItem->type&IT_ADVANCED?"   x":"";
 Table->Cells[10][1]=FoldItem->type&IT_TRADE?"   x":"";
 Table->Cells[11][1]=FoldItem->type&IT_MAN?"   x":"";
 Table->Cells[12][1]=FoldItem->type&IT_MONSTER?"   x":"";
 Table->Cells[13][1]=FoldItem->type&IT_MAGIC?"   x":"";
 Table->Cells[14][1]=FoldItem->type&IT_WEAPON?"   x":"";
 Table->Cells[15][1]=FoldItem->type&IT_ARMOR?"   x":"";
 Table->Cells[16][1]=FoldItem->type&IT_MOUNT?"   x":"";
 Table->Cells[17][1]=FoldItem->type&IT_BATTLE?"   x":"";
 Table->Cells[18][1]=FoldItem->type&IT_FOOD?"   x":"";
 Table->Cells[19][1]=FoldItem->walk;
 Table->Cells[20][1]=FoldItem->ride;
 Table->Cells[21][1]=FoldItem->fly;

 if(FnewItem){
  Table->RowCount=3;
  Table->Cells[0][1]="Cur";
  Table->Cells[0][2]="New";

  Table->Cells[1][2]=FnewItem->name;
  Table->Cells[2][2]=FnewItem->names;
  Table->Cells[3][2]=FnewItem->abr;
  Table->Cells[4][2]=FnewItem->flags&CANTGIVE?"   x":"";
  Table->Cells[5][2]=MakeDepSkillStr(FnewItem->skill,FnewItem->level);
  Table->Cells[6][2]=MakeInputItemStr(FnewItem->input,FnewItem->number);
  Table->Cells[7][2]=FnewItem->weight;
  Table->Cells[ 8][2]=FnewItem->type&IT_NORMAL?"   x":"";
  Table->Cells[ 9][2]=FnewItem->type&IT_ADVANCED?"   x":"";
  Table->Cells[10][2]=FnewItem->type&IT_TRADE?"   x":"";
  Table->Cells[11][2]=FnewItem->type&IT_MAN?"   x":"";
  Table->Cells[12][2]=FnewItem->type&IT_MONSTER?"   x":"";
  Table->Cells[13][2]=FnewItem->type&IT_MAGIC?"   x":"";
  Table->Cells[14][2]=FnewItem->type&IT_WEAPON?"   x":"";
  Table->Cells[15][2]=FnewItem->type&IT_ARMOR?"   x":"";
  Table->Cells[16][2]=FnewItem->type&IT_MOUNT?"   x":"";
  Table->Cells[17][2]=FnewItem->type&IT_BATTLE?"   x":"";
  Table->Cells[18][2]=FnewItem->type&IT_FOOD?"   x":"";
  Table->Cells[19][2]=FnewItem->walk;
  Table->Cells[20][2]=FnewItem->ride;
  Table->Cells[21][2]=FnewItem->fly;
 }
 PrepareName(FoldItem);
 PrepareNames(FoldItem);
 PrepareAbr(FoldItem);
 PrepareCantGive(FoldItem);
 PrepareSkill(FoldItem);
 PrepareInput(FoldItem);
 PrepareWeight(FoldItem);
 PrepareNormal(FoldItem);
 PrepareAdvanced(FoldItem);
 PrepareTrade(FoldItem);
 PrepareMan(FoldItem);
 PrepareMonster(FoldItem);
 PrepareMagic(FoldItem);
 PrepareWeapon(FoldItem);
 PrepareArmor(FoldItem);
 PrepareMount(FoldItem);
 PrepareBattle(FoldItem);
 PrepareFood(FoldItem);
 PrepareWalk(FoldItem);
 PrepareRide(FoldItem);
 PrepareFly (FoldItem);
 PrepareHelpProd(FoldItem);
}
void __fastcall TEditItemTypeForm::FormShow(TObject *Sender)
{
 if(!FoldItem||!ItemTypes||!SkillTypes){ModalResult=mrCancel;return;}
}
void TEditItemTypeForm::PrepareName(AItemType *itype)
{
 edName->Text=itype->name;
}
void TEditItemTypeForm::PrepareNames(AItemType *itype)
{
 edNames->Text=itype->names;
}
void TEditItemTypeForm::PrepareAbr(AItemType* itype)
{
 edAbr->Text=itype->abr;
}
void TEditItemTypeForm::PrepareCantGive(AItemType* itype)
{
 cbCantGive->Checked=(itype->flags&CANTGIVE)||ItemTypes->IsIllusionAbr(itype->abr);
}
void TEditItemTypeForm::PrepareSkill(AItemType* itype)
{
 int ind=cbSkill->Items->IndexOf(SkillTypes->AbrToName(itype->skill));
 if(ind==-1)ind=0;
 cbSkill->ItemIndex=ind;
 if(ind){
  cbLevel->ItemIndex=itype->level-1;
 }
 cbSkillChange(this);
}
void __fastcall TEditItemTypeForm::cbSkillChange(TObject *Sender)
{
 if(cbSkill->ItemIndex<=0){
  cbLevel->Visible=false;
  cbLevel->ItemIndex=0;
  cbInput->ItemIndex=0;
  cbInputChange(Sender);
  cbInput->Visible=false;
 }else{
  cbLevel->Visible=true;
  cbInput->Visible=true;
 }
}
void TEditItemTypeForm::PrepareInput(AItemType* itype)
{
 if(cbInput->Visible==false)return;
 int ind=cbInput->Items->IndexOf(ItemTypes->AbrToName(itype->input));
 if(ind==-1)ind=0;
 cbInput->ItemIndex=ind;
 if(ind){
  edNumber->Text=itype->number;
 }
 cbInputChange(this);
}
void __fastcall TEditItemTypeForm::cbInputChange(TObject *Sender)
{
 if(cbInput->ItemIndex<=0){
  edNumber->Visible=false;
  edNumber->Text=0;
 }else{
  edNumber->Visible=true;
 }
}
void TEditItemTypeForm::PrepareWeight(AItemType *itype)
{
 edWeight->Text=itype->weight;
}
void TEditItemTypeForm::PrepareNormal(AItemType* itype)
{
 cbNormal->Checked=itype->type&IT_NORMAL;
}
void TEditItemTypeForm::PrepareAdvanced(AItemType* itype)
{
 cbAdvanced->Checked=itype->type&IT_ADVANCED;
}
void TEditItemTypeForm::PrepareTrade(AItemType* itype)
{
 cbTrade->Checked=itype->type&IT_TRADE;
}
void TEditItemTypeForm::PrepareMan(AItemType* itype)
{
 cbMan->Checked=itype->type&IT_MAN;
}
void TEditItemTypeForm::PrepareMonster(AItemType* itype)
{
 cbMonster->Checked=itype->type&IT_MONSTER;
}
void TEditItemTypeForm::PrepareMagic(AItemType* itype)
{
 cbMagic->Checked=itype->type&IT_MAGIC;
}
void TEditItemTypeForm::PrepareWeapon(AItemType* itype)
{
 cbWeapon->Checked=itype->type&IT_WEAPON;
}
void TEditItemTypeForm::PrepareArmor(AItemType* itype)
{
 cbArmor->Checked=itype->type&IT_ARMOR;
}
void TEditItemTypeForm::PrepareMount(AItemType* itype)
{
 cbMount->Checked=itype->type&IT_MOUNT;
}
void TEditItemTypeForm::PrepareBattle(AItemType* itype)
{
 cbBattle->Checked=itype->type&IT_BATTLE;
}
void TEditItemTypeForm::PrepareFood(AItemType * itype)
{
 cbFood->Checked=itype->type&IT_FOOD;
}
void TEditItemTypeForm::PrepareWalk(AItemType *itype)
{
 edWalk->Text=itype->walk;
}
void TEditItemTypeForm::PrepareRide(AItemType *itype)
{
 edRide->Text=itype->ride;
}
void TEditItemTypeForm::PrepareFly(AItemType *itype)
{
 edFly->Text=itype->fly;
}
void TEditItemTypeForm::PrepareHelpProd(AItemType* itype)
{
 int ind=cbHelpProd->Items->IndexOf(ItemTypes->AbrToName(itype->helpproditem));
 if(ind==-1) ind=0;
 cbHelpProd->ItemIndex=ind;
}
void __fastcall TEditItemTypeForm::bnOkClick(TObject *Sender)
{
 FoldItem->name=edName->Text;
 FoldItem->names=edNames->Text;
 FoldItem->abr=edAbr->Text;
 FoldItem->flags=0;
 if(cbCantGive->Checked)FoldItem->flags|=CANTGIVE;
 int ind;
 ind=cbSkill->ItemIndex;
 if(ind==0){
  FoldItem->skill="";
 }else{
  FoldItem->skill=SkillTypes->NameToAbr(cbSkill->Items->Strings[ind]);
  FoldItem->level=cbLevel->ItemIndex+1;
 }
 ind=cbInput->ItemIndex;
 if(ind==0){
  FoldItem->input="";
 }else{
  FoldItem->input=ItemTypes->NameToAbr(cbInput->Items->Strings[ind]);
  FoldItem->number=edNumber->Text.ToInt();
 }
 FoldItem->weight=edWeight->Text.ToInt();

 FoldItem->type=0;
 if(cbNormal->Checked)FoldItem->type|=  IT_NORMAL;
 if(cbAdvanced->Checked)FoldItem->type|=IT_ADVANCED;
 if(cbTrade->Checked)FoldItem->type|=   IT_TRADE;
 if(cbMan->Checked)FoldItem->type|=     IT_MAN;
 if(cbMonster->Checked)FoldItem->type|= IT_MONSTER;
 if(cbMagic->Checked)FoldItem->type|=   IT_MAGIC;
 if(cbWeapon->Checked)FoldItem->type|=  IT_WEAPON;
 if(cbArmor->Checked)FoldItem->type|=   IT_ARMOR;
 if(cbMount->Checked)FoldItem->type|=   IT_MOUNT;
 if(cbBattle->Checked)FoldItem->type|=  IT_BATTLE;
 if(cbFood->Checked)FoldItem->type|=    IT_FOOD;

 FoldItem->walk=edWalk->Text.ToInt();
 FoldItem->ride=edRide->Text.ToInt();
 FoldItem->fly=edFly->Text.ToInt();

 ind=cbHelpProd->ItemIndex;
 if(ind==0){
  FoldItem->helpproditem="";
 }else{
  FoldItem->helpproditem=ItemTypes->NameToAbr(cbHelpProd->Items->Strings[ind]);
 }
}
void __fastcall TEditItemTypeForm::TableDblClick(TObject *Sender)
{
 if(Table->Row==0||Table->Col==0)return;
 AItemType *it=FoldItem;
 if(Table->Row==2)it=FnewItem;
 switch(Table->Col){
  case 1:PrepareName(it);break;
  case 2:PrepareNames(it);break;
  case 3:PrepareAbr(it);break;
  case 4:PrepareCantGive(it);break;
  case 5:PrepareSkill(it);break;
  case 6:PrepareInput(it);break;
  case 7:PrepareWeight(it);break;
  case 8:PrepareNormal  (it);break;
  case 9:PrepareAdvanced(it);break;
  case 10:PrepareTrade   (it);break;
  case 11:PrepareMan     (it);break;
  case 12:PrepareMonster (it);break;
  case 13:PrepareMagic   (it);break;
  case 14:PrepareWeapon  (it);break;
  case 15:PrepareArmor   (it);break;
  case 16:PrepareMount   (it);break;
  case 17:PrepareBattle  (it);break;
  case 18:PrepareFood    (it);break;
  case 19:PrepareWalk(it);break;
  case 20:PrepareRide(it);break;
  case 21:PrepareFly (it);break;
 }
}

