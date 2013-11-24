//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SaveMapFrm.h"
#include "util.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TSaveMapForm *SaveMapForm;
//---------------------------------------------------------------------------
__fastcall TSaveMapForm::TSaveMapForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSaveMapForm::FormShow(TObject *Sender)
{
 cbAllRegion->Checked=optSaveMap&roAllRegion;
 cbRegionInfo->Checked=optSaveMap&roRegionInfo;
 cbUserObject->Checked=optSaveMap&roUserObject;
 cbNonuserObject->Checked=optSaveMap&roNonuserObject;
 cbLocalUnit->Checked=optSaveMap&roLocalUnit;
 cbLocalAsNonlocalUnit->Checked=optSaveMap&roLocalAsNonlocalUnit;
 cbNonlocalUnit->Checked=optSaveMap&roNonlocalUnit;
 cbLocalUnitClick(0);
}
//---------------------------------------------------------------------------
void __fastcall TSaveMapForm::bnOkClick(TObject *Sender)
{
 SetBits(optSaveMap,roAllRegion,cbAllRegion->Checked);
 SetBits(optSaveMap,roRegionInfo,cbRegionInfo->Checked);
 SetBits(optSaveMap,roUserObject,cbUserObject->Checked);
 SetBits(optSaveMap,roNonuserObject,cbNonuserObject->Checked);
 SetBits(optSaveMap,roLocalUnit,cbLocalUnit->Checked);
 SetBits(optSaveMap,roLocalAsNonlocalUnit,cbLocalAsNonlocalUnit->Checked);
 SetBits(optSaveMap,roNonlocalUnit,cbNonlocalUnit->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TSaveMapForm::cbLocalUnitClick(TObject *Sender)
{
 cbLocalAsNonlocalUnit->Enabled=!cbLocalUnit->Checked;
}
//---------------------------------------------------------------------------

