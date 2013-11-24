//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "util.h"
#include "orders.h"
#include "EditOrderBoolFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditOrderBoolForm *EditOrderBoolForm;
//---------------------------------------------------------------------------
__fastcall TEditOrderBoolForm::TEditOrderBoolForm(TComponent* Owner, AOrderByte *_ord)
    : TForm(Owner),ord(_ord)
{
 cbCommented->Checked=ord->commented;
 cbRepeating->Checked=ord->repeating;
 rbOn->Checked=ord->value;
 rbOff->Checked=!ord->value;
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderBoolForm::bnOkClick(TObject *Sender)
{
 ord->commented=cbCommented->Checked;
 ord->repeating=cbRepeating->Checked;
 ord->value=rbOn->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TEditOrderBoolForm::bnCancelClick(TObject *Sender)
{
 ord=0;
}

