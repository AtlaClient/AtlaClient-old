//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("AtlaClient.res");
USEFORM("source\MainFrm.cpp", MainForm);
USEUNIT("source\util.cpp");
USEUNIT("source\faction.cpp");
USEUNIT("source\items.cpp");
USEUNIT("source\skills.cpp");
USEUNIT("source\orders.cpp");
USEUNIT("source\unit.cpp");
USEUNIT("source\markets.cpp");
USEUNIT("source\region.cpp");
USEUNIT("source\turn.cpp");
USEUNIT("source\reports.cpp");
USEUNIT("source\exprlang.cpp");
USEUNIT("source\viewitems.cpp");
USEFORM("source\NewGameFrm.cpp", NewGameForm);
USEFORM("source\ChangeGameFrm.cpp", ChangeGameForm);
USEFORM("source\PackingFrm.cpp", PackingForm);
USEFORM("source\LinkShaftFrm.cpp", LinkShaftForm);
USEFORM("source\EditOrderDescrFrm.cpp", EditOrderDescrForm);
USEFORM("source\EditOrderTeachFrm.cpp", EditOrderTeachForm);
USEFORM("source\EditOrderBuild.cpp", EditOrderBuildForm);
USEFORM("source\EditOrderIntFrm.cpp", EditOrderIntForm);
USEFORM("source\EditOrderBuySellFrm.cpp", EditOrderBuySellForm);
USEFORM("source\EditOrderMovingFrm.cpp", EditOrderMovingForm);
USEFORM("source\EditOrderAutoGiveFrm.cpp", EditOrderAutoGiveForm);
USEFORM("source\EditOrderEnterFrm.cpp", EditOrderEnterForm);
USEFORM("source\EditOrderByteFrm.cpp", EditOrderByteForm);
USEFORM("source\EditOrderCustomFrm.cpp", EditOrderCustomForm);
USEFORM("source\EditOrderGiveFrm.cpp", EditOrderGiveForm);
USEFORM("source\EditOrderSimpleFrm.cpp", EditOrderSimpleForm);
USEFORM("source\EditOrderAbrFrm.cpp", EditOrderAbrForm);
USEFORM("source\EditOrderCommentFrm.cpp", EditOrderCommentForm);
USEFORM("source\ProdModeSetupFrm.cpp", ProdModeSetupForm);
USEFORM("source\WedgeModeSetupFrm.cpp", WedgeModeSetupForm);
USEFORM("source\EventsFrm.cpp", EventsForm);
USEFORM("source\EditItemTypeFrm.cpp", EditItemTypeForm);
USEFORM("source\ShowItemTypesFrm.cpp", ShowItemTypesForm);
USEFORM("source\EditSkillTypeFrm.cpp", EditSkillTypeForm);
USEFORM("source\ShowSkillTypesFrm.cpp", ShowSkillTypesForm);
USEFORM("source\ShowSkillInfosFrm.cpp", ShowSkillInfosForm);
USEFORM("source\StoreFrm.cpp", StoreForm);
USEFORM("source\RegInfoFrm.cpp", RegInfoForm);
USEFORM("source\MapFrm.cpp", MapForm);
USEUNIT("source\import.cpp");
USEFORM("source\OptionsFrm.cpp", OptionsForm);
USEFORM("source\EditOrderReceiveFrm.cpp", EditOrderReceiveForm);
USEFORM("source\AboutFrm.cpp", AboutForm);
USEFORM("source\FormNewUnitFrm.cpp", FormNewUnitForm);
USEFORM("source\SaveMapFrm.cpp", SaveMapForm);
USEUNIT("source\eapfile.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->CreateForm(__classid(TMainForm), &MainForm);
     Application->CreateForm(__classid(TEventsForm), &EventsForm);
     Application->CreateForm(__classid(TRegInfoForm), &RegInfoForm);
     Application->CreateForm(__classid(TMapForm), &MapForm);
     Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
