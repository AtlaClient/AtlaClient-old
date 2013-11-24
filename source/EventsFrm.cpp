//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <inifiles.hpp>

#include "EventsFrm.h"
#include "util.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEventsForm *EventsForm;
static AnsiString IniSect="EventsWin";
//---------------------------------------------------------------------------
__fastcall TEventsForm::TEventsForm(TComponent* Owner)
    : TForm(Owner)
{
 Left=Screen->Width-Width;
 TIniFile *ini=new TIniFile(IniName);
 Left=atoi(ini->ReadString(IniSect,"Left",Left).c_str());
 Top=atoi(ini->ReadString(IniSect,"Top",Top).c_str());
 Width=atoi(ini->ReadString(IniSect,"Width",Width).c_str());
 Height=atoi(ini->ReadString(IniSect,"Height",Height).c_str());
 delete ini;
}
//---------------------------------------------------------------------------
void TEventsForm::AddEvent(AnsiString ev)
{
 if(Memo->Lines->Count>350){
  Memo->Lines->BeginUpdate();
  while(Memo->Lines->Count>300){
   Memo->Lines->Delete(0);
  }
  Memo->Lines->EndUpdate();
 }
 Memo->Lines->Add(ev);
 if(!Visible){
  if(optShowEvents){
   Show();
  }
 }else{
  BringToFront();
//  Application->ProcessMessages();
 }
}


void TEventsForm::ClearEvents()
{
 Memo->Lines->Clear();
}
void __fastcall TEventsForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 Memo->Lines->Clear();    
}
//---------------------------------------------------------------------------

void __fastcall TEventsForm::FormDestroy(TObject *Sender)
{
 TIniFile *ini=new TIniFile(IniName);
 ini->WriteString(IniSect,"Left",Left);
 ini->WriteString(IniSect,"Top",Top);
 ini->WriteString(IniSect,"Width",Width);
 ini->WriteString(IniSect,"Height",Height);
 delete ini;

}
//---------------------------------------------------------------------------

