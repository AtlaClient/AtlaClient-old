//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include "PackingFrm.h"
#include "util.h"
#include "turn.h"
#include "region.h"
#include "unit.h"
#include "mainfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPackingForm *PackingForm;
static AnsiString marker="     X  ",null="";
//---------------------------------------------------------------------------
__fastcall TPackingForm::TPackingForm(TComponent* Owner)
    : TForm(Owner)
{
 Grid->Cells[0][0]="¹";
 Grid->Cells[1][0]="Month";
 Grid->Cells[2][0]="Packed";
 Grid->Cells[3][0]="Regions";
 Grid->Cells[4][0]="Units";
 Grid->RowCount=curTurns->count+1-curTurns->curbeginturn+1;
 Grid->Cells[0][1]=0;
 for(int i=curTurns->curbeginturn,j=2;i<curTurns->count;i++,j++){
  Grid->Cells[0][j]=i;
  Grid->Cells[1][j]=TurnNumberToString(i);
 }
 UpdateTable();
}
void TPackingForm::UpdateTable()
{
 for(int j=0,i=1;j<curTurns->count;j++){
  ATurn *t=curTurns->Get(j);
  if(!t) continue;
  Grid->Cells[2][i]=t->Packed?marker:null;
  Grid->Cells[3][i]=t->RegionList->count;
  int num=0;
  foreach(t->RegionList){
   ARegion *reg=*(ARegion**)iter;
   foreach(reg){
    AObject *obj=*(AObject**)iter;
    num+=obj->units->count;
   }
  }
  Grid->Cells[4][i]=num;
  i++;
 }
 RecallWidths();
}
void TPackingForm::RecallWidths()
{
 for(int j=Grid->ColCount-1;j>=0;j--){
  int size,maxsize=0;
  for(int i=Grid->RowCount-1;i>=0;i--){
   size=Grid->Canvas->TextWidth(Grid->Cells[j][i]);
   if(size>maxsize) maxsize=size;
  }
//  maxsize*="M");
  Grid->ColWidths[j]=maxsize+10;
 }
}

void __fastcall TPackingForm::bnApplyClick(TObject *Sender)
{
 curTurns->StoreLastParam();
 for(int i=Grid->RowCount-1;i>=1;i--){
  int num=atoi(Grid->Cells[0][i].c_str());
  ATurn *t=curTurns->Get(num);
  if(t) t->Packed=Grid->Cells[2][i].Length();
 }
 curTurns->Get(0)->VerifyNextTurn();
 int num=curTurn->num;
 curTurn=0;
 curTurns->SetCurTurn(num);
 MainForm->ChangeTurn(0);
 UpdateTable();
}
//---------------------------------------------------------------------------
void __fastcall TPackingForm::GridDblClick(TObject *Sender)
{
 int row=Grid->Row;
 if(Grid->Col!=2) return;
 if(row<1) return;
 Grid->Cells[2][row]=Grid->Cells[2][row].Length()?null:marker;
}
//---------------------------------------------------------------------------
void __fastcall TPackingForm::GridKeyPress(TObject *Sender, char &Key)
{
 if(Key==VK_SPACE||Key==VK_RETURN){
  Key=0;
  GridDblClick(Sender);
 }
}
void __fastcall TPackingForm::bnUndoClick(TObject *Sender)
{
 UpdateTable();
}
void __fastcall TPackingForm::bnDeleteClick(TObject *Sender)
{
// AnsiString mes;
 foreach(curTurns){
  ATurn *t=*(ATurn**) iter;
  if(!t) continue;
  if(t->Packed) continue;
  AnsiString fName=t->GetFileName(1);
  fName=GetGameFileName(fName);
/*  mes="Deleting package for turn ";
  mes+=t->num;*/
  DeleteFiles(fName);
 }
}
//---------------------------------------------------------------------------

