//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include "util.h"
#include "eapfile.h"
#include "orders.h"
#include "unit.h"
#include "items.h"
#include "markets.h"
#include "region.h"
#include "faction.h"
#include "skills.h"
#include "EditOrderCustomFrm.h"
#include "EditOrderCommentFrm.h"
#include "EditOrderDescrFrm.h"
#include "EditOrderSimpleFrm.h"
#include "EditOrderByteFrm.h"
#include "EditOrderAbrFrm.h"
#include "EditOrderGiveFrm.h"
#include "EditOrderBuySellFrm.h"
#include "EditOrderTeachFrm.h"
#include "EditOrderIntFrm.h"
#include "EditOrderEnterFrm.h"
#include "EditOrderMovingFrm.h"
#include "EditOrderAutoGiveFrm.h"
#include "EditOrderBuild.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
struct OrderStr{
 OrderTypes index;
 char key[28];
}OrderStrs[]={
 {O_END,"#end"},
 {O_UNIT,"unit "},
 {O_COMMENT," "},
 {O_LOCALDESCR,";"},
 {O_FORM,"form "},
 {O_FORMEND,"end"},

 {O_ADVANCE,"advance "},
 {O_AUTOTAX,"autotax "},
 {O_AVOID,"avoid "},
 {O_BEHIND,"behind "},
 {O_BUILD,"build"},
 {O_BUY,"buy "},
 {O_CLAIM,"claim "},
 {O_COMBAT,"combat"},
 {O_CONSUME,"consume"},
 {O_OPTION,"option "},
 {O_DESTROY,"destroy"},
 {O_ENTERTAIN,"entertain"},
 {O_ENTER,"enter "},
 {O_FORGET,"forget "},
 {O_GIVE,"give "},
 {O_GUARD,"guard "},
 {O_HOLD,"hold "},
 {O_LEAVE,"leave"},
 {O_MOVE,"move "},
 {O_NOAID,"noaid "},
 {O_PILLAGE,"pillage"},
 {O_PRODUCE,"produce "},
 {O_REVEAL,"reveal"},
 {O_SAIL,"sail"},
 {O_SELL,"sell "},
 {O_STUDY,"study "},
 {O_TAX,"tax"},
 {O_TEACH,"teach "},
 {O_WORK,"work"},
 {O_NOP,"NOP"},

};
AnsiString keyUnit="unit";
AnsiString keyFaction="faction";
AnsiString keyIn="IN";
AnsiString keyOut="OUT";
AnsiString keyAutoOrder="@;!";
OrderTypes FindOrderType(char *&s){
 for(int i=sizeof(OrderStrs)/sizeof(OrderStrs[0])-1;i>=0;i--){
  OrderStr &os=OrderStrs[i];
  if(!strncmpi(s,os.key,strlen(os.key))){
   s+=strlen(os.key);
   while(*s==' ')s++;
   return os.index;
  }
 }
 return O_NONE;
}
AnsiString FindOrderKey(OrderTypes type){
 for(int i=sizeof(OrderStrs)/sizeof(OrderStrs[0])-1;i>=0;i--){
  if(OrderStrs[i].index==type)return OrderStrs[i].key;
 }
 return "";
}
int ParseAlias(char *&s){
 if(!MyCompare(s,"faction ")){
  s+=sizeof("faction ")-1;
//  int facnum=atoi(
  GetNextToken(s); //  .c_str());
  while(*s==' ')s++;
 }
 int num;
 if(!MyCompare(s,"new ")){
  s+=sizeof("new ")-1;
  num=-atoi(GetNextToken(s).c_str());
 }else{
  num=atoi(GetNextToken(s).c_str());
 }
 return num;
}
AOrder *ParseOrder(AnsiString str){
 for(int i=str.Length();i>=1;i--){
  if(str[i]=='\t') str[i]=' ';
 }
 char *s=str.c_str();
 while(*s==' ') s++;
 if(!MyCompare(s,keyAutoOrder)){
  s+=keyAutoOrder.Length();
  AOrder *ord=0;
//  char *olds=s;
  OrderTypes ot=FindOrderType(s);
  switch(ot){
   case O_GIVE:
    ord=new AOrderAutoGive;
    if(!ord->ParseOrder(s)){
     AddEvent("Error: Bad auto order format: "+str);
     delete ord;
     ord=0;
    }
   break;
   case O_NOP:
    ord=new AOrderSimple;
    ord->type=O_NOP;
    ord->commented=false;
    ord->repeating=true;
   break;
  }
  if(ord) return ord;
 }
 bool commented=false,repeating=false;
 if(*s==';')return 0;
 if(*s=='@'){
  repeating=true;
  s++;
 }
 if(*s==';'){
  commented=true;
  s++;
 }
 AOrder *ord;
 char *olds=s;
 OrderTypes ot=FindOrderType(s);
 switch(ot){
  case O_END:case O_UNIT:case O_FORM:case O_FORMEND:
   ord=new AOrder();
   ord->type=ot;
  break;
  case O_COMMENT:
   ord=new AOrderComment();
   if(!ord->ParseOrder(s)){delete ord;return 0;}
   ord->repeating=repeating;
  break;
  case O_LOCALDESCR:
   ord=new AOrderLocDescr();
   if(!ord->ParseOrder(s)){delete ord;return 0;}
  break;
  case O_DESTROY:
  case O_ENTERTAIN:
  case O_LEAVE:
  case O_PILLAGE:
  case O_TAX:
  case O_WORK:
   ord=new AOrderSimple();
   ord->type=ot;
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){delete ord;return 0;}
  break;
  case O_AUTOTAX:
  case O_AVOID:
  case O_BEHIND:
  case O_GUARD:
  case O_HOLD:
  case O_NOAID:
  case O_REVEAL:
  case O_CONSUME:
//  case O_OPTION:
   ord=new AOrderByte();
   ord->type=ot;
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){delete ord;return 0;}
  break;
  case O_COMBAT:
  case O_FORGET:
  case O_PRODUCE:
  case O_STUDY:
   ord=new AOrderAbr();
   ord->type=ot;
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){
    AddEvent("Error: Bad order format: "+str);
    delete ord;
    ord=new AOrderCustom();
    ord->commented=commented;
    ord->repeating=repeating;
    if(!ord->ParseOrder(olds)){delete ord;return 0;}
   }
  break;
  case O_GIVE:
   ord=new AOrderGive();
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){
    AddEvent("Error: Bad order format: "+str);
    delete ord;
    ord=new AOrderCustom();
    ord->commented=commented;
    ord->repeating=repeating;
    if(!ord->ParseOrder(olds)){delete ord;return 0;}
   }
  break;
  case O_BUY:
  case O_SELL:
   ord=new AOrderBuySell();
   ord->type=ot;
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){
    AddEvent("Error: Bad order format: "+str);
    delete ord;
    ord=new AOrderCustom();
    ord->commented=commented;
    ord->repeating=repeating;
    if(!ord->ParseOrder(olds)){delete ord;return 0;}
   }
  break;
  case O_TEACH:
   ord=new AOrderTeach();
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){
    AddEvent("Error: Bad order format: "+str);
    delete ord;
    ord=new AOrderCustom();
    ord->commented=commented;
    ord->repeating=repeating;
    if(!ord->ParseOrder(olds)){delete ord;return 0;}
   }
  break;
  case O_CLAIM:
  case O_ENTER:
   ord=new AOrderInt();
   ord->type=ot;
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){
    AddEvent("Error: Bad order format: "+str);
    delete ord;
    ord=new AOrderCustom();
    ord->commented=commented;
    ord->repeating=repeating;
    if(!ord->ParseOrder(olds)){delete ord;return 0;}
   }
  break;
  case O_MOVE:
  case O_ADVANCE:
  case O_SAIL:
   ord=new AOrderMoving();
   ord->type=ot;
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){
    AddEvent("Error: Bad order format: "+str);
    delete ord;
    ord=new AOrderCustom();
    ord->commented=commented;
    ord->repeating=repeating;
    if(!ord->ParseOrder(olds)){delete ord;return 0;}
   }
  break;
  case O_BUILD:
   ord=new AOrderBuild();
   ord->type=ot;
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(s)){
    AddEvent("Error: Bad order format: "+str);
    delete ord;
    ord=new AOrderCustom();
    ord->commented=commented;
    ord->repeating=repeating;
    if(!ord->ParseOrder(olds)){delete ord;return 0;}
   }
  break;
  default:
   ord=new AOrderCustom();
   ord->commented=commented;
   ord->repeating=repeating;
   if(!ord->ParseOrder(olds)){delete ord;return 0;}
 }
 return ord;
};

AOrder *ReadOrder(AOrders *orders,TFile &in){
  int i;
  OrderTypes type;
  ReadIntAsByte(in,i);
  type=OrderTypes(i);
  AOrder *ord;
  switch(type){
   case O_CUSTOMORDER:
    ord=new AOrderCustom();
    ord->Read(in);
    return ord;
   case O_COMMENT:
    ord=new AOrderComment();
    ord->Read(in);
    return ord;
   case O_LOCALDESCR:
    ord=new AOrderLocDescr();
    ord->Read(in);
    return ord;
   case O_DESTROY:
   case O_ENTERTAIN:
   case O_LEAVE:
   case O_PILLAGE:
   case O_TAX:
   case O_WORK:
   case O_NOP:
    ord=new AOrderSimple();
    ord->type=type;
    ord->Read(in);
    return ord;
   case O_AUTOTAX:
   case O_AVOID:
   case O_BEHIND:
   case O_GUARD:
   case O_HOLD:
   case O_NOAID:
   case O_REVEAL:
   case O_CONSUME:
//   case O_OPTION:
    ord=new AOrderByte();
    ord->type=type;
    ord->Read(in);
    return ord;
   case O_COMBAT:
   case O_FORGET:
   case O_PRODUCE:
   case O_STUDY:
    ord=new AOrderAbr();
    ord->type=type;
    ord->Read(in);
    return ord;
   case O_GIVE:
    ord=new AOrderGive();
    ord->Read(in);
    return ord;
   case O_BUY:
   case O_SELL:
    ord=new AOrderBuySell();
    ord->type=type;
    ord->Read(in);
    return ord;
   case O_TEACH:
    ord=new AOrderTeach();
    ord->Read(in);
    return ord;
   case O_CLAIM:
   case O_ENTER:
    ord=new AOrderInt;
    ord->type=type;
    ord->Read(in);
    return ord;
   case O_MOVE:
   case O_ADVANCE:
   case O_SAIL:
    ord=new AOrderMoving;
    ord->type=type;
    ord->Read(in);
    return ord;
   case O_AUTOGIVE:
    ord=new AOrderAutoGive;
    ord->Read(in);
    return ord;
   case O_BUILD:
    ord=new AOrderBuild;
    ord->Read(in);
    return ord;
/*
   case O_:
    ord=new AOrder;
    ord->type=type;
    ord->Read(in);
    return ord;
*/
  }
  return 0;
};
AOrder::AOrder()
 :type(O_NONE),orders(0),flags(0),isedited(false)
// commented(false),repeating(false)
{

}
AOrder::~AOrder()
{
}
void AOrder::Read(TFile & in)
{
 in>>flags;
}
void AOrder::Write(TFile & out)
{
 WriteIntAsByte(out,type);
 out<<flags;
}
AnsiString AOrder::Print()
{
 return "ERROR";
}
AnsiString AOrder::WriteTemplate()
{
 return "ERROR";
}
bool AOrder::ParseOrder(char * s)
{
 return false;
}
void AOrder::Run(int value)
{
}
AOrder * AOrder::Edit()
{
 return 0;
}
AnsiString AOrder::Begin()
{
 AnsiString s;
 if(repeating)s+="@";
 if(commented)s+=";";
 return s;
}
bool AOrder::IsMonthLong()
{
 return false;
}
int AOrder::Productivity()
{
 return 0;
}
void AOrder::DeleteMessage(AOrderMessage *mes){
}
void AOrder::Prepare(){
}
AOrders::AOrders(AUnit *un)
 :unit(un),autodelete(true)
{
}
__fastcall AOrders::~AOrders()
{
 Clear();
}
void __fastcall AOrders::Clear()
{
 if(autodelete){
  for(int i=Count-1;i>=0;i--){
   AOrder *ord=Get(i);
 /*  if(ord->type==O_RECEIVED){
    AOrderMessage *o=dynamic_cast<AOrderMessage*>(ord);
    if(o->base){
     o->base->target=0;
    }
   }*/
   TList::Delete(i);
   delete ord;
  }
 }
 TList::Clear();
}
void AOrders::Read(TFile & in)
{
 int kol;
 in>>kol;
 for(int i=0;i<kol;i++){
  AOrder *ord=ReadOrder(this,in);
  if(ord)Add(ord);
 }
}
void AOrders::Write(TFile & out)
{
 Compact();
 int kol=count;
 out<<kol;
 foreach(this){
  AOrder *ord=*(AOrder**)iter;
  ord->Write(out);
 }
}
AOrder* AOrders::Get(int index)
{
 return (AOrder*)Items[index];
}
void AOrders::Add(AOrder* ord)
{
// ord->index=count;
 if(autodelete)ord->orders=this;
 TList::Add(ord);
}
void AOrders::Delete(AOrder * ord)
{
 for(int i=count-1;i>=0;i--){\
  AOrder *o=Get(i);
  if(o==ord){
   TList::Delete(i);
   if(autodelete) delete ord;
   break;
  }
 }
}
void AOrders::Delete(int index)
{
 if(index<0||index>=count)return;
 AOrder *ord=Get(index);
 TList::Delete(index);
 if(autodelete) delete ord;
}
void AOrders::Update(AOrders * orders)
{
 if(!autodelete)
  throw Exception("Can't update orders by autodelete=false");
 Clear();
 foreach(orders){
  AOrder *ord=*(AOrder**)iter;
  Add(ord);
 }
 bool oldad=orders->autodelete;
 orders->autodelete=false;
 orders->Clear();
 orders->autodelete=oldad;
}
bool AOrders::Replace(AOrder * from, AOrder * to)
{
 for(int i=count-1;i>=0;i--){
  if(Get(i)==from){
   if(autodelete)to->orders=this;
   Items[i]=to;
   if(autodelete)delete from;
   return true;
  }
 }
 return false;
}
AOrder * AOrders::Find(OrderTypes type, int mode)
{
 if(type==O_LOCALDESCR){
  foreach(this){
   AOrder *ord=*(AOrder**)iter;
   if(ord->type!=type)continue;
   return ord;
  }
  return 0;
 }
 foreachr(this){
  AOrder *ord=*(AOrder**)iter;
  if(ord->type!=type)continue;
  if(ord->commented==true){
   if(mode&1)return ord;
  }else{
   if(mode&2)return ord;
  }
  if(ord->repeating==true){
   if(mode&4)return ord;
  }else{
   if(mode&8)return ord;
  }
 }
 return 0;
}
bool AOrders::Distribute(int amount, int attempted)
{
 if(attempted<amount) attempted=amount;
 foreach(this){
  AOrder *ord=*(AOrder**)iter;
  if(!amount)break;
  if(!attempted)break;
  int prod=ord->Productivity(),ubucks;
  if(prod==-1)return false;
//  if(prod){
   ubucks=int(double(amount)*prod/attempted);
//  }else ubucks=0;
  amount-=ubucks;
  attempted-=prod;
  ord->Run(ubucks);
 }
 return true;
}
void AOrders::Compact()
{
 if(!count)return;
 for(int i=Count-1;i>=0;i--){
  AOrder *ord=Get(i);
  if(ord->type!=O_MESSAGE)continue;
  TList::Delete(i);
  if(autodelete) delete ord;
 }
 if(!count)return;
 bool autotax=unit->GetFlag(FLAG_AUTOTAX);
 AOrderByte *o=dynamic_cast<AOrderByte*>(Find(O_AUTOTAX,2));
 if(o) autotax=o->value;
 if(autotax){
  for(int i=Count-1;i>=0;i--){
   AOrder *ord=Get(i);
   if(ord->type!=O_TAX)continue;
   if(ord->repeating)continue;
   if(ord->commented)continue;
   if(autodelete)delete ord;
   TList::Delete(i);
  }
 }
 if(!count)return;
 {
  AOrder *ord=Get(0);
  if(ord->type!=O_LOCALDESCR){
   for(int i=count-1;i>0;i--){
    ord=Get(i);
    if(ord->type!=O_LOCALDESCR)continue;
    TList::Delete(i);
    TList::Insert(0,ord);
    break;
   }
  }
 }
}
bool AOrders::Swap(int i, int j)
{
 if(i==j) return false;
 if(i<0||i>=count) return false;
 if(j<0||j>=count) return false;
 void *ord=Items[i];
 Items[i]=Items[j];
 Items[j]=ord;
 return true;
}
AOrderCustom::AOrderCustom()
{
 type=O_CUSTOMORDER;
}
AOrderCustom::~AOrderCustom()
{
}
void AOrderCustom::Read(TFile & in)
{
 AOrder::Read(in);
 in>>text;
}
void AOrderCustom::Write(TFile & out)
{
 AOrder::Write(out);
 out<<text;
}
AnsiString AOrderCustom::Print()
{
 AnsiString s=Begin()+text+" (custom)";
 if(IsMonthLong())s+="+";
 return s;
}
AnsiString AOrderCustom::WriteTemplate()
{
 return Begin()+text;
}
bool AOrderCustom::ParseOrder(char * s)
{
 text=s;
 return true;
}
AOrder * AOrderCustom::Edit()
{
 TEditOrderCustomForm *frm=new TEditOrderCustomForm(0,this);
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 return ord;
}
bool AOrderCustom::IsMonthLong()
{
 if(commented)return false;
// if(!MyCompare(text.c_str(),FindOrderKey(O_BUILD)))return true;
// if(!MyCompare(text.c_str(),FindOrderKey(O_PRODUCE)))return true;
// if(!MyCompare(text.c_str(),FindOrderKey(O_TEACH)))return true;
// if(!MyCompare(text.c_str(),FindOrderKey(O_MOVE)))return true;
// if(!MyCompare(text.c_str(),FindOrderKey(O_ADVANCE)))return true;
// if(!MyCompare(text.c_str(),FindOrderKey(O_SAIL)))return true;
 return false;
}

AOrderComment::AOrderComment()
{
 type=O_COMMENT;
 commented=true;
 repeating=true;
}
AOrderComment::~AOrderComment()
{
}
void AOrderComment::Read(TFile & in)
{
 AOrder::Read(in);
 in>>text;
 commented=true;
}
void AOrderComment::Write(TFile & out)
{
 AOrder::Write(out);
 out<<text;
}
AnsiString AOrderComment::Print()
{
 return Begin()+" "+text+" (comment)";
}
AnsiString AOrderComment::WriteTemplate()
{
 return Begin()+" "+text;
}
bool AOrderComment::ParseOrder(char * s)
{
 commented=true;
 text=s;
 return true;
}
AOrder* AOrderComment::Edit()
{
 TEditOrderCommentForm *frm=new TEditOrderCommentForm(0,this);
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 return ord;
}

AOrderLocDescr::AOrderLocDescr()
{
 type=O_LOCALDESCR;
}
AOrderLocDescr::~AOrderLocDescr()
{
}
void AOrderLocDescr::Read(TFile & in)
{
 AOrder::Read(in);
 in>>text;
 commented=true;
 repeating=true;
}
void AOrderLocDescr::Write(TFile & out)
{
 AOrder::Write(out);
 out<<text;
}
AnsiString AOrderLocDescr::Print()
{
 return "@;;"+text+" (local descr)";
}
AnsiString AOrderLocDescr::WriteTemplate()
{
 return "@;;"+text;
}
bool AOrderLocDescr::ParseOrder(char * s)
{
 commented=true;
 repeating=true;
 text=s;
 return true;
}
AOrder * AOrderLocDescr::Edit()
{
 TEditOrderDescrForm *frm=new TEditOrderDescrForm(0,this);
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 return ord;
}
AOrderSimple::AOrderSimple()
 :number(-1)
{
}
AOrderSimple::~AOrderSimple()
{
}
AnsiString AOrderSimple::Print()
{
 if(type==O_NOP){
  return keyAutoOrder+FindOrderKey(O_NOP);
 }
 AnsiString s=Begin()+FindOrderKey(type);
 if(commented)return s;
 if(type==O_PILLAGE||type==O_TAX||type==O_ENTERTAIN||type==O_WORK){
  s+=" ;+$";
  s+=number;
 }
 return s;
}
AnsiString AOrderSimple::WriteTemplate()
{
 if(type==O_NOP)
  return keyAutoOrder+FindOrderKey(O_NOP);
 return Begin()+FindOrderKey(type);
}
bool AOrderSimple::ParseOrder(char * s)
{
 return true;
}
AOrder * AOrderSimple::Edit()
{
 if(type==O_NOP) return this;
 TEditOrderSimpleForm *frm=new TEditOrderSimpleForm(0,this);
 switch(type){
  case O_DESTROY:
   frm->Caption="Edit DESTROY order";
   frm->labInfo->Caption="";
  break;
  case O_ENTERTAIN:
   frm->Caption="Edit ENTERTAIN order";
   if(orders->unit->GetSkill("ENTE")==0){
    frm->labInfo->Caption="Warning: Unit cannot ENTERTAIN";
   }else{
    frm->labInfo->Caption="";
   }
  break;
  case O_LEAVE:
   frm->Caption="Edit LEAVE order";
   frm->labInfo->Caption="";
  break;
  case O_PILLAGE:
   frm->Caption="Edit PILLAGE order";
   if(!curUnit->Taxers()){
    frm->labInfo->Caption="Warning: Unit cannot TAX";
   }else if(!curUnit->baseobj->basereg->money){
    frm->labInfo->Caption="Warning: No money";
   }else if(!curUnit->baseobj->basereg->CanTax(curUnit)){
    frm->labInfo->Caption="Warning: Unit on guard";
   }
  break;
  case O_TAX:
   frm->Caption="Edit TAX order";
   if(!curUnit->Taxers()){
    if(orders->unit->GetLocalDescr()!="empty")
     frm->labInfo->Caption="Warning: Unit cannot TAX";
   }else if(!curUnit->baseobj->basereg->money){
    frm->labInfo->Caption="Warning: No money";
   }else if(!curUnit->baseobj->basereg->CanTax(curUnit)){
    frm->labInfo->Caption="Warning: Unit on guard";
   }
  break;
  case O_WORK:
   frm->Caption="Edit WORK order";
   frm->labInfo->Caption="";
  break;
 }
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 return ord;
}
void AOrderSimple::Run(int value)
{
 if(commented)return;
 AUnit *un=orders->unit;
 AnsiString mes;
 switch(type){
  case O_DESTROY:
  case O_NOP:
  break;
  case O_LEAVE:
   un->endobj=un->baseobj->basereg->GetDummy(); 
  break;
  case O_PILLAGE:
  case O_ENTERTAIN:
  case O_TAX:
  case O_WORK:
   number=value;
   un->SetMoney(un->GetMoney() + value);
  break;
 }
}

bool AOrderSimple::IsMonthLong()
{
 if(commented)return false;
 if(type==O_ENTERTAIN||type==O_WORK||type==O_NOP)return true;
 return false;
}
int AOrderSimple::Productivity()
{
 AUnit *unit=orders->unit;
 ARegion *reg=unit->baseobj->basereg;
 if(type==O_TAX)return reg->curtaxprod*unit->Taxers();
 if(type==O_PILLAGE)return 100*unit->Taxers();
 if(type==O_ENTERTAIN){
  if(orders->unit->GetSkill("ENTE")==0){
   AddEvent(orders->unit->FullName()+" Error: Unit doesn't have ENTERTAIN skill");
   return 0;
  }
  int ei=reg->entincome;
  if(ei==0) ei=20;
  return ei*unit->GetSkill("ENTE")*unit->GetMen();
 }
 if(type==O_WORK){
  int wi=reg->workincome;
  if(wi==0) wi=reg->wages;
  return wi*orders->unit->GetMen();
 }
 return 0;
}
void AOrderSimple::Prepare()
{
 switch(type){
  case O_ENTERTAIN:
  case O_TAX:
  case O_WORK:
  case O_PILLAGE:
   number=0;
  break;
 }
}

AOrderByte::AOrderByte()
 :value(false)
{
}
AOrderByte::~AOrderByte()
{
}
void AOrderByte::Read(TFile & in)
{
 AOrder::Read(in);
 in>>value;
}
void AOrderByte::Write(TFile & out)
{
 AOrder::Write(out);
 out<<value;
}
AnsiString AOrderByte::Print()
{
 AnsiString s=Begin()+FindOrderKey(type);
 if(type==O_REVEAL||type==O_CONSUME){
  switch(value){
   case 0:
   break;
   case 1:
    s+=" "+keyUnit;
   break;
   case 2:
    s+=" "+keyFaction;
   break;
  }
 }else{
  s+=(value?"1":"0");
 }
 return s;
}
AnsiString AOrderByte::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(type);
 if(type==O_REVEAL||type==O_CONSUME){
  switch(value){
   case 0:
   break;
   case 1:
    s+=" "+keyUnit;
   break;
   case 2:
    s+=" "+keyFaction;
   break;
  }
 }else{
  s+=(value?"1":"0");
 }
 return s;
}
static AnsiString TrueStr="true";
bool AOrderByte::ParseOrder(char *s)
{
 if(type==O_REVEAL||type==O_CONSUME){
  if(*s=='\0'){
   value=0;
  }else if(!MyCompare(s,keyUnit)){
   value=1;
  }else if(!MyCompare(s,keyFaction)){
   value=2;
  }else return false;
  return true;
 }
 if(*s=='1')value=true;
 else if(!MyCompare(s,TrueStr))value=true;
 return true;
}
void AOrderByte::Run(int v)
{
 if(commented)return;
 AUnit *un=orders->unit;
 switch(type){
  case O_AUTOTAX:
   un->SetEndFlag(FLAG_AUTOTAX,value);
  break;
  case O_AVOID:
   if(value){
    un->endguard=GUARD_AVOID;
   }else if(un->endguard==GUARD_AVOID){
    un->endguard=GUARD_NONE;
   }
  break;
  case O_BEHIND:
   un->SetEndFlag(FLAG_BEHIND,value);
  break;
  case O_GUARD:
   if(value){
    un->endguard=GUARD_SET;
   }else if(un->endguard!=GUARD_AVOID){
    un->endguard=GUARD_NONE;
   }
  break;
  case O_HOLD:
   un->SetEndFlag(FLAG_HOLDING,value);
  break;
  case O_NOAID:
   un->SetEndFlag(FLAG_NOAID,value);
  break;
  case O_REVEAL:
   un->endreveal=value;
  break;
  case O_CONSUME:
   {
    bool confac=false,conun=false;
    if(value==1) conun=true;
    else if(value==2) confac=true;
    un->SetEndFlag(FLAG_CONSUMING_UNIT,conun);
    un->SetEndFlag(FLAG_CONSUMING_FACTION,confac);
   }
  break;
 }
}
AOrder * AOrderByte::Edit()
{
 TStringList *list=new TStringList;
 list->Add("O&ff");
 list->Add("O&n");
 AnsiString Caption,Info;
 switch(type){
  case O_AUTOTAX:
   Caption="Edit AUTOTAX order";
   if(!curUnit->Taxers()){
    Info="Warning: Unit cannot TAX";
   }else if(!curUnit->baseobj->basereg->money){
    Info="Warning: No money";
   }else if(!curUnit->baseobj->basereg->CanTax(curUnit)){
    Info="Warning: Unit on guard";
   }
  break;
  case O_AVOID:
   Caption="Edit AVOID order";
  break;
  case O_BEHIND:
   Caption="Edit BEHIND order";
  break;
  case O_GUARD:
   Caption="Edit GUARD order";
   if(!curUnit->baseobj->basereg->CanTax(curUnit)){
    Info="Warning: Unit on guard";
   }
  break;
  case O_HOLD:
   Caption="Edit HOLD order";
  break;
  case O_NOAID:
   Caption="Edit NOAID order";
  break;
  case O_REVEAL:
  case O_CONSUME:
   list->Clear();
   list->Add("&None");
   list->Add("&"+keyUnit);
   list->Add("&"+keyFaction);
   if(type==O_REVEAL) Caption="Edit REVEAL order";
   else Caption="Edit CONSUME order";
  break;
 }
 TEditOrderByteForm *frm=new TEditOrderByteForm(0,this,list);
 frm->Caption=Caption;
 frm->labInfo->Caption=Info;
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 delete list;
 return ord;
}
AOrderAbr::AOrderAbr()
 :number(0)
{
}
AOrderAbr::~AOrderAbr()
{
}
void AOrderAbr::Read(TFile & in)
{
 AOrder::Read(in);
 ReadStringAsInt(in,abr);
}
void AOrderAbr::Write(TFile & out)
{
 AOrder::Write(out);
 WriteStringAsInt(out,abr);
}
AnsiString AOrderAbr::Print()
{
 AnsiString name;
 switch(type){
  case O_COMBAT:
   name=" ";
//break;   
  case O_FORGET:
   {
    name+=SkillTypes->AbrToName(abr);
   }
  break;
  case O_PRODUCE:
   {
    name=ItemTypes->AbrToName(abr);
    if(!commented&&number>0){
     name+=" ;+";
     name+=number;
    }
   }
  break;
  case O_STUDY:
   {
    name=SkillTypes->AbrToName(abr);
    if(commented)break;
    ASkillType *stype=SkillTypes->Get(abr,2);
    if(!stype)break;
    AUnit *un=orders->unit;
    if(!un)break;
    int cost=un->GetMen()*stype->cost;
    name+=" ;-$";
    name+=cost;
    name+=":";
    int beg=un->GetBegRealSkill(abr);
    name+=beg;
    name+="+";
    name+=(un->GetRealSkill(abr)-beg);
   }
  break;
  default:
   name=abr;
 }
 return Begin()+FindOrderKey(type)+name;
}
AnsiString AOrderAbr::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(type);
 if(type==O_COMBAT) s+=" ";
 return s+abr;
}
bool AOrderAbr::ParseOrder(char * s)
{
 AnsiString str=GetNextToken(s);

 switch(type){
  case O_COMBAT:
   {
    if(!str.Length()){
     abr="";
     return true;
    }
    ASkillType *stype=SkillTypes->Get(str);
    if(!stype)return false;
    if((stype->flags&MAGICCOMBAT)!=MAGICCOMBAT)return false;
    abr=stype->abr;
   }
  break;
  case O_FORGET:
   {
    ASkillType *stype=SkillTypes->Get(str);
    if(!stype)return false;
    abr=stype->abr;
   }
  break;
  case O_PRODUCE:
   {
    AItemType *itype=ItemTypes->Get(str);
    if(!itype)return false;
    abr=itype->abr;
   }
  break;
  case O_STUDY:
   {
    ASkillType *stype=SkillTypes->Get(str);
    if(!stype)return false;
    abr=stype->abr;
   }
  break;
 }
 return true;
}
void AOrderAbr::Run(int value)
{
 if(commented)return;
 AUnit *un=orders->unit;
 AnsiString mes;
 switch(type){
  case O_COMBAT:return;
  case O_FORGET:
   if(un->GetSkillDays(abr)){
    un->SetSkillDays(abr,0);
    un->endskills->DeleteEmpty();
   }else{
    mes="Error ";
    mes+=un->FullName();
    mes+=" FORGET: unit doesn't know skill";
    AddEvent(mes);
   }
  break;
  case O_PRODUCE:
   {
    AItemType *itype=ItemTypes->Get(abr,4);
    if(!itype) return;
    if(itype->input.Length()){
     if(value!=-1)
      throw Exception("Error in PRODUCE::run");
     value=Productivity();
     int inputno=itype->number;
     value/=inputno;
     int present=un->enditems->GetNum(itype->input);
     if(!present){
      mes="Error ";
      mes+=un->FullName();
      mes+=" PRODUCE: has no materials (";
      mes+=ItemTypes->AbrToName(itype->input);
      mes+=")";
      AddEvent(mes);
      value=0;
     }else if(value*inputno>present){
      value=present/inputno;
     }
     un->enditems->SetNum(itype->input,present-value*inputno);
    }else if(value<0)return;
    un->enditems->SetNum(abr,un->enditems->GetNum(abr)+value);
    number=value;
    if(value){
     if(itype->type&IT_FOOD) un->needed/=2;
    }
   }
  break;
  case O_STUDY:
   {
    if(!un->GetMen()){
     number=0;
     return;
    }
    if(un->endskills->count>0&&!un->IsLeader()){
     if(un->endskills->Get(0)->type!=abr){
      mes="Error : ";
      mes+=un->FullName();
      mes+=" STUDY: Can know only 1 skill";
      AddEvent(mes);
      number=0;
      return;
     }
    }
    if(!un->CanStudy(abr)){
     mes="Error: ";
     mes+=un->FullName();
     mes+=" STUDY: Doesn't have the pre-requisite skills to study that";
     AddEvent(mes);
     number=0;
     return;
    }
    ASkillType *stype=SkillTypes->Get(abr,2);
    if(!stype)return;
    if((stype->flags&MAGIC)&&un->GetSkill(abr)>=2&&!un->endobj->Protect()){
     mes="Warning: ";
     mes+=un->FullName();
     mes+=" STUDY: Magic study rate outside of a tower is cut in half above level 2.";
     AddEvent(mes);
     number/=2;
    }
    if(un->GetSkill(abr)>=5){
     mes="Error : ";
     mes+=un->FullName();
     mes+=" STUDY: already knows this skills";
     AddEvent(mes);
     number=0;
     return;
    }
    int cost=un->GetMen()*stype->cost;
    int lastmoney=un->GetMoney()-cost;
    if(lastmoney<0){
     mes+="Error : ";
     mes+=un->FullName();
     mes+=" STUDY: Not enough funds ";
     mes+=-lastmoney;
     AddEvent(mes);
     return;
    }
    un->SetMoney(lastmoney);
    number/=un->GetMen();
    un->SetSkillDays(abr,un->GetSkillDays(abr)+number);
   }
  break;
 }
}
AOrder * AOrderAbr::Edit()
{
 TStringList *names=new TStringList, *abrs=new TStringList;
 AnsiString Caption,mes;
 AUnit *unit=orders->unit;
 switch(type){
  case O_COMBAT:
   Caption="Edit COMBAT order";
   abrs->Add("");
   names->Add("None");
   foreach(unit->skills){
    ASkill *skill=*(ASkill**)iter;
    ASkillType *stype=SkillTypes->Get(skill->type);
    if(!stype)throw Exception("Bad skill type");
    if((stype->flags&MAGICCOMBAT)!=MAGICCOMBAT)continue;
    abrs->Add(stype->abr);
    names->Add(stype->name);
   }
  break;
  case O_FORGET:
   Caption="Edit FORGET order";
   isedited=true;
   unit->baseobj->basereg->RunOrders();
   isedited=false;
   unit->endskills->DeleteEmpty();
   foreach(unit->endskills){
    ASkill *skill=*(ASkill**)iter;
    ASkillType *stype=SkillTypes->Get(skill->type);
    if(!stype)throw Exception("Bad skill type");
    abrs->Add(stype->abr);
    names->Add(stype->name);
   }
  break;
  case O_PRODUCE:
   Caption="Edit PRODUCE order";
   foreach(ItemTypes){
    AItemType *itype=*(AItemType**)iter;
    if(!itype->skill.Length())continue;
    if(itype->level>unit->GetSkill(itype->skill))continue;
    mes=itype->name;
    if(itype->input.Length()){
     mes+=" (from ";
     AItem it;
     it.type=itype->input;
     it.count=itype->number;
     mes+=it.Print(3);
     mes+=")";
    }else{
     AMarket *mark=orders->unit->baseobj->basereg->products->GetMarket(itype->abr);
     if(!mark)continue;
     if(!mark->amount)continue;
    }
    abrs->Add(itype->abr);
    names->Add(mes);
   }
  break;
  case O_STUDY:
  {
   Caption="Edit STUDY order";
   bool menunchanged=(unit->GetMen()==unit->GetBegMen());
   foreach(SkillTypes){
    ASkillType *stype=*(ASkillType**)iter;
//    mes=stype->name;
    {
     ASkill sk;
     sk.type=stype->abr;
     sk.days=unit->GetBegSkillDays(sk.type);
     if(sk.days>=450&&menunchanged) continue;
     mes=sk.Print(1+4+8);
    }
    if(stype->flags&MAGIC){
     if(!(stype->flags&FOUNDATION))continue;
     mes+=" (foundation)";
    }
    abrs->Add(stype->abr);
    names->Add(mes);
   }
   TStringList *list=orders->unit->canstudy;
   for(int i=0;i<list->Count;i++){
    ASkillType *stype=SkillTypes->Get(list->Strings[i],2);
    if(!stype) throw Exception("Error in STUDY::EDIT");
    {
     ASkill sk;
     sk.type=stype->abr;
     sk.days=unit->GetBegSkillDays(sk.type);
     if(sk.days>=450) continue;
     mes=sk.Print(1+4+8);
    }
    abrs->Add(stype->abr);
    names->Add(/*stype->name*/mes+" (magic)");
   }
  }
  break;
  default:
   delete names;
   delete abrs;
   return 0;
 }
 TEditOrderAbrForm *frm=new TEditOrderAbrForm(0,this,names,abrs);
 frm->Caption=Caption;
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 delete names;
 delete abrs;
 return ord;
}
bool AOrderAbr::IsMonthLong()
{
 if(commented)return false;
 if(type==O_PRODUCE||type==O_STUDY)return true;
 return false;
}
int AOrderAbr::Productivity(){
 if(type!=O_PRODUCE)return 0;
 if(number==-1)return -1;
 AUnit *unit=orders->unit;
 AItemType *itype=ItemTypes->Get(abr,4);
 int mens=unit->GetMen();
 int prod=mens*unit->GetSkill(itype->skill);
 if(itype->helpproditem.Length()){
  int bonus=unit->enditems->GetNum(itype->helpproditem);
  if(bonus>mens) bonus=mens;
  prod+=bonus;
 }
 return prod;
}
void AOrderAbr::Prepare(){
 number=0;
 if(commented)return;
 if(type!=O_PRODUCE)return;
 AItemType *itype=ItemTypes->Get(abr,4);
 AnsiString mes;
 if(!itype||itype->skill.Length()==0){
  mes=orders->unit->FullName();
  mes+=" PRODUCE: can't produce ";
  if(itype) mes+=itype->name;
  else mes+="nothing";
  AddEvent(mes);
  number=-1;
  return;
 }
 if(itype->input.Length()==0){
  AMarket *mark=orders->unit->baseobj->basereg->products->GetMarket(abr);
  if(!mark){
   mes=orders->unit->FullName();
   mes+=" PRODUCE: can't produce ";
   mes+=itype->name;
   mes+=" in this region";
   AddEvent(mes);
   number=-1;
   return;
  }
 }
}
AOrderGive::AOrderGive()
 :target(0),count(0),tarnum(0),message(0),item("SILV")
{
 type=O_GIVE;
}
AOrderGive::~AOrderGive()
{
 DeleteMessage(message);
}
void AOrderGive::DeleteMessage(AOrderMessage *mes){
 if(mes!=message)return;
 if(!mes)return;
 mes->base=0;
 mes->orders->Delete(mes);
 message=0;
 target=0;
}
void AOrderGive::SetMessage(){
 if(!message){
  if(!target)return;
  message=new AOrderMessage(this);
  message->target=orders->unit;
  target->orders->Add(message);
 }
 AnsiString s="received ";
 if(item.Length()){
  s+=added;
  s+=" ";
  AnsiString str=ItemTypes->AbrToName(item);
  if(str.Length())s+=str;
  else s+="???";
  s+=" from";
 }else s+=keyUnit;
 s+=" ";
 s+=orders->unit->FullName();
 message->text=s;
}
void AOrderGive::Prepare(){
 added=0;
 AnsiString mes;
 if(!target){
  if(tarnum) target=orders->unit->baseobj->basereg->GetUnit(tarnum);
  if(commented)return;
  if(!target){
   if(tarnum){
    mes="Error ";
    mes+=orders->unit->FullName();
    mes+=" GIVE: unit ";
    AUnit *un=new AUnit(0);
    un->num=tarnum;
    mes+=un->Alias();
    delete un;
    mes+=" is not found";
    AddEvent(mes);
   }
   return;
  }
 }
 if(message){
  if(commented){ //???
   DeleteMessage(message);
  }
  return;
 }
 if(!commented)
  SetMessage();
}
void AOrderGive::Read(TFile & in)
{
 AOrder::Read(in);
 in>>tarnum;
 if(BaseVer<7) if(!tarnum)return;
 in>>count;
 ReadStringAsInt(in,item);
}
void AOrderGive::Write(TFile & out)
{
 AOrder::Write(out);
 out<<tarnum;
// if(!tarnum)return;
 out<<count;
 WriteStringAsInt(out,item);
}
AnsiString AOrderGive::Print()
{
 AnsiString s=Begin()+FindOrderKey(type);
 AUnit *un=target;
 if(un)s+=un->FullName()+" ";
 else if(tarnum==0){
  s+="0 ";
 }else{
//  if(commented){
   un=orders->unit->baseobj->basereg->GetUnit(tarnum);
   if(un){
    s+=un->FullName()+" ";
   }
//  }
  if(!un){
   un=new AUnit(orders->unit->turn);
   un->num=tarnum;
   un->faction=orders->unit->endfaction;
   s+="?"+un->Alias()+" ";
   delete un;
  }
 }
 if(item.Length()){
  s+=count;
  s+=" ";
  AnsiString ss=ItemTypes->AbrToName(item);
  if(ss.Length()) s+=ss;
  else{
   s+="???";
  }
 }else{
  s+=keyUnit;
 }
 return s;
}
AnsiString AOrderGive::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(type);

 if(!target){
  if(tarnum) target=orders->unit->baseobj->basereg->GetUnit(tarnum);
 }
 if(tarnum==0){
  s+="0 ";
 }else if(!target){
  if(!commented){
   s=orders->unit->FullName()+" GIVE: Unknown target ";
   AUnit *un=new AUnit(orders->unit->turn);
   un->num=tarnum;
   un->faction=orders->unit->endfaction;
   s+=un->Alias();
   delete un;
   AddEvent(s);

   throw Exception(s);
  }else{
   AUnit *un=new AUnit(orders->unit->turn);
   un->num=tarnum;
   un->faction=orders->unit->endfaction;
   s+=un->Alias()+" ";
   delete un;
  }
 }else s+=target->Alias()+" ";

 if(item.Length()){
  s+=count;
  s+=" ";
  AItemType *itype=ItemTypes->Get(item,4);
  if(itype)s+=item;
  else{
   s=orders->unit->FullName()+" GIVE: unknown item "+item;
   AddEvent(s);
   throw Exception(s);
  }
 }else{
  s+=keyUnit;
 }
 return s;
}
bool AOrderGive::ParseOrder(char * s)
{
 tarnum=ParseAlias(s);
// if(!tarnum)return false;
 while(*s==' ')s++;
 AnsiString str=GetNextToken(s);
 if(!CompareIStr(str,keyUnit)){
  item="";
  return true;
 }
 if(!str.Length())return false;
 if(str[1]<'0'||str[1]>'9') return false;
 count=atoi(str.c_str());
 while(*s==' ')s++;
 AnsiString itemname=GetNextToken(s);
 AItemType *itype=ItemTypes->Get(itemname);
 if(!itype)return false;
 item=itype->abr;
 return true;
}
void AOrderGive::Run(int value)
{
 if(commented)return;
 AUnit *un=orders->unit;
// if(un->endfaction!=un->faction) return;
 AnsiString mes;
 if(!item.Length()){
  if(!tarnum){
   mes+="Error : ";
   mes+=un->FullName();
   mes+=" GIVE: Can't discard a whole unit";
   AddEvent(mes);
   return;
  }
  if(!target){
   return;
  }
  if(un->endfaction==target->endfaction){
   mes+="Warning : ";
   mes+=un->FullName();
   mes+=" GIVE: unit is given to its own faction";
   AddEvent(mes);
  }
  orders->unit->endfaction=target->endfaction;
  return;
 }
 added=count;
 int lastitem=un->enditems->GetNum(item)-added;
 if(lastitem<0){
  mes+="Error : ";
  mes+=un->FullName();
  mes+=" GIVE: Not enough ";
  mes+=-lastitem;
  mes+=" "+ItemTypes->AbrToName(item);
  AddEvent(mes);
  added+=lastitem;
  lastitem=0;
 }
 AItemType *itype=ItemTypes->Get(item);
 if(!itype)
  throw Exception("Give: bad type");
 if(tarnum==0){
  un->enditems->SetNum(item,lastitem);
  if(!(optDontShowEvents&dseDiscard))
  {
   mes="Warning: ";
   mes+=un->FullName();
   mes+=" discards ";
   mes+=added;
   mes+=" "+ItemTypes->AbrToNames(item);
   AddEvent(mes);
  }
  return;
 }
 if(itype->flags&CANTGIVE){
  mes="Error: ";
  mes+=un->FullName();
  mes+=" GIVE: Can't give ";
  mes+=itype->names;
  AddEvent(mes);
  return;
 }
 if(!target)return;
 if(itype->type&IT_MAN){
  if(un->endfaction!=target->endfaction){
   mes="Error: ";
   mes+=un->FullName();
   mes+=" GIVE: Can't give men to another faction";
   AddEvent(mes);
   return;
  }
  int men=target->GetMen();
  foreach(target->endskills){
   ASkill *skill=*(ASkill**)iter;
   skill->days*=men;
  }
  foreach(un->skills){
   ASkill *skill=*(ASkill**)iter;
   target->SetSkillDays(skill->type,target->GetSkillDays(skill->type)+skill->days*added);
  }
  men+=added;
  foreach(target->endskills){
   ASkill *skill=*(ASkill**)iter;
   skill->days/=men;
  }
 }
 un->enditems->SetNum(item,lastitem);
 target->enditems->SetNum(item,target->enditems->GetNum(item)+added);
 SetMessage();
}
AOrder * AOrderGive::Edit()
{
 AUnit *unit=orders->unit;
 if(!unit)return 0;
 ARegion *reg=unit->baseobj->basereg;
 if(!reg)return 0;
 isedited=true;
 reg->RunOrders();
 isedited=false;
 AItems *items=unit->enditems;
 items->DeleteEmpty(); 
 AUnits *units=new AUnits(orders->unit->baseobj);
 units->autodelete=false;
 AUnits *units2=new AUnits(orders->unit->baseobj);
 units2->autodelete=false;

 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un==unit)continue;

//   if(un->faction->GetAttitude(unit->faction->number)<A_FRIENDLY)continue;
   bool add1=true;
/*   if(unit->endfaction){
    if(!un->endfaction) add1=false;
    else */
   if(unit->endfaction->GetAttitude(un->endfaction->number)<A_FRIENDLY) add1=false;
//   }
   if(add1) units->Add(un); else units2->Add(un);
  }
 }
 TEditOrderGiveForm *frm=new TEditOrderGiveForm(0,this,items,units,units2);
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 delete units;
 delete units2;
 return ord;
}
AOrderMessage::AOrderMessage(AOrder *b)
 :base(b),target(0)
{
 type=O_MESSAGE;
}
AOrderMessage::~AOrderMessage()
{
 if(base) base->DeleteMessage(this);
}
void AOrderMessage::Read(TFile & in)
{
// AOrder::Read(in);
}
void AOrderMessage::Write(TFile & out)
{
 WriteIntAsByte(out,type);
// AOrder::Write(out);
}
AnsiString AOrderMessage::Print()
{
 return ";"+text;
}
AnsiString AOrderMessage::WriteTemplate()
{
 return "";
}
AOrderBuySell::AOrderBuySell()
 :amount(0),number(0)
{
}
AOrderBuySell::~AOrderBuySell()
{
}
void AOrderBuySell::Read(TFile & in)
{
 AOrder::Read(in);
 ReadStringAsInt(in,item);
 in>>amount;
}
void AOrderBuySell::Write(TFile & out)
{
 AOrder::Write(out);
 WriteStringAsInt(out,item);
 out<<amount;
}
AnsiString AOrderBuySell::Print()
{
 AnsiString s=Begin()+FindOrderKey(type);
 s+=amount;
 s+=" ";
 s+=ItemTypes->AbrToName(item);
 if(commented) return s;
 ARegion *reg=orders->unit->baseobj->basereg;
 AMarkets *markets;
 if(type==O_BUY) markets=reg->saleds;
 else markets=reg->wanteds;
 int price=0;
 AMarket *mark=markets->GetMarket(item);
 if(mark) price=mark->price;
 price*=number;
 if(price){
  s+=(type==O_BUY?" ;-$":" ;+$");
  s+=price;
  s+="/";
  s+=number;
 }
 return s;
}
AnsiString AOrderBuySell::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(type);
 s+=amount;
 s+=" ";
 s+=item;
 return s;
}
bool AOrderBuySell::ParseOrder(char * s)
{
 amount=atoi(GetNextToken(s).c_str());
 if(!amount)return false;
 number=amount;
 while(*s==' ')s++;
 AnsiString itemname=GetNextToken(s);
 AItemType *itype=ItemTypes->Get(itemname);
 if(!itype)return false;
 item=itype->abr;
 return true;
}
void AOrderBuySell::Run(int value)
{
 if(commented)return;
 AnsiString mes;
 AUnit *un=orders->unit;
 ARegion *reg=un->baseobj->basereg;
 AMarkets *markets;
 if(type==O_BUY) markets=reg->saleds;
 else markets=reg->wanteds;
 int price=0;
 AMarket *mark=markets->GetMarket(item);
 if(mark) price=mark->price;
 if(type==O_BUY){
  price*=value;
  AItemType *itype=ItemTypes->Get(item);
  if(!itype)
   throw Exception("Buy: bad type");
  if(itype->type&IT_MAN){
   int men=un->GetMen();
   foreach(un->endskills){
    ASkill *skill=*(ASkill**)iter;
    skill->days*=men;
   }
   men+=value;
   foreach(un->endskills){
    ASkill *skill=*(ASkill**)iter;
    skill->days/=men;
   }
  }
  un->enditems->SetNum(item,un->enditems->GetNum(item)+value);
  un->SetMoney(un->GetMoney()-price);
 }else{
  int last=un->enditems->GetNum(item)-value;
  price*=value;
  un->enditems->SetNum(item,last);
  un->SetMoney(un->GetMoney()+price);
 }
 number=value;
}
AOrder * AOrderBuySell::Edit()
{
 AUnit *unit=orders->unit;
 if(!unit)return 0;
 ARegion *reg=unit->baseobj->basereg;
 if(!reg)return 0;
 AMarkets *markets=new AMarkets;
 AnsiString caption,label;
 isedited=true;
 reg->RunOrders();
 isedited=false;
 int money=unit->GetMoney();
 if(type==O_BUY){
  caption="Edit BUY order";
  label="buy";
  markets->Update(reg->saleds);
  foreach(markets){
   AMarket *market=*(AMarket**)iter;
   int maxbuy=money/market->price;
   if(maxbuy<market->amount)market->amount=maxbuy;
  }
 }else{
  caption="Edit SELL order";
  label="sell";
  markets->Update(reg->wanteds);
  foreach(markets){
   AMarket *market=*(AMarket**)iter;
   int maxsell=unit->enditems->GetNum(market->type);
   if(maxsell<market->amount)market->amount=maxsell;
  }
  markets->DeleteEmpty();
 }
 TEditOrderBuySellForm *frm=new TEditOrderBuySellForm(0,this,markets);
 frm->Label->Caption=label;
 frm->Caption=caption;
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 delete markets;
 return ord;
}
int AOrderBuySell::Productivity()
{
 if(isedited)return -1;
 return number;
}
AOrderTeach::AOrderTeach()
 :targets(new TList),bases(0)
{
 type=O_TEACH;
}
AOrderTeach::~AOrderTeach()
{
 delete bases;bases=0;
 Clear();
 delete targets;
}
void AOrderTeach::Read(TFile & in)
{
 Clear();
 AOrder::Read(in);
 int kol,num;
 in>>kol;
 for(int i=0;i<kol;i++){
  in>>num;
  AddTarget(num,0);
 }
}
void AOrderTeach::Write(TFile & out)
{
 AOrder::Write(out);
 int kol=targets->Count;
 out<<kol;
 for(int i=0;i<kol;i++){
  AUnitPtr *ptr=GetTarget(i);
  out<<ptr->num;
 }
}
AnsiString AOrderTeach::Print()
{
 AnsiString s=Begin()+FindOrderKey(type);
 int kol=targets->Count;
 for(int i=0;i<kol;i++){
  AUnitPtr *ptr=GetTarget(i);
  AUnit *un=ptr->ptr;
  if(un)s+=un->FullName();
  else{
 //  if(commented){
    un=orders->unit->baseobj->basereg->GetUnit(ptr->num);
    if(un){
     s+=un->FullName();
    }
 //  }
   if(!un){
    un=new AUnit(orders->unit->turn);
    un->num=ptr->num;
    un->faction=orders->unit->endfaction;
    s+="?"+un->Alias();
    delete un;
   }
  }
  s+=" ";
 }
 s.SetLength(s.Length()-1);
 return s;
}
AnsiString AOrderTeach::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(type);

 int kol=targets->Count;
 for(int i=0;i<kol;i++){
  AUnitPtr *ptr=GetTarget(i);
  AUnit *target=ptr->ptr;
  if(!target){
   if(ptr->num) target=orders->unit->baseobj->basereg->GetUnit(ptr->num);
  }
  if(!target){
   if(!commented){
    s=orders->unit->FullName()+" TEACH: Unknown target ";
    AUnit *un=new AUnit(orders->unit->turn);
    un->num=ptr->num;
    un->faction=orders->unit->endfaction;
    s+=un->Alias();
    delete un;
    AddEvent(s);
    throw Exception(s);
   }else{
    AUnit *un=new AUnit(orders->unit->turn);
    un->num=ptr->num;
    un->faction=orders->unit->endfaction;
    s+=un->Alias();
    delete un;
   }
  }else s+=target->Alias();
  s+=" ";
 }
 s.SetLength(s.Length()-1);
 return s;
}
bool AOrderTeach::ParseOrder(char * s)
{
 int tarnum=ParseAlias(s);
 if(!tarnum)return false;
 while(1){
  AddTarget(tarnum,0);
  if(*s==0)break;
  if(*s!=' ') return false;
  while(*s==' ')s++;
  tarnum=ParseAlias(s);
  if(!tarnum)return false;
 }
 return true;
}

void AOrderTeach::Run(int value)
{
 AnsiString mes;
 AUnit *unit=orders->unit;
 if(!unit->IsLeader()){
  mes=unit->FullName()+" TEACH: Only leaders can teach";
  AddEvent(mes);
  return;
 }
 int students=0;
 for(int i=0;i<targets->Count;i++){
  AUnitPtr *ptr=GetTarget(i);
  AUnit *target=ptr->ptr;
  if(!target){
   targets->Delete(i);i--;
   delete ptr;
   continue;
  }
  if(unit->GetAttitude(0,target)<A_FRIENDLY){ //replace
//  if(target->GetAttitude(0,unit)<A_FRIENDLY){ //replace
   mes=unit->FullName()+" TEACH warning: unit ";
   mes+=target->FullName()+" is not a member of a friendly faction";
   AddEvent(mes);
/*   targets->Delete(i);i--;
   continue;*/
  }
  AOrderAbr *study=dynamic_cast<AOrderAbr*>(target->orders->Find(O_STUDY,2));
  if(!study){
   mes=unit->FullName()+" TEACH error: unit ";
   mes+=target->FullName()+" is not studying";
   AddEvent(mes);
   targets->Delete(i);i--;
   delete ptr;
   continue;
  }
  AnsiString skill=study->abr;
  if(unit->GetRealSkill(skill)<=target->GetRealSkill(skill)){
   mes=unit->FullName()+" TEACH error: unit ";
   mes+=target->FullName()+" is not studying a skill you can teach";
   AddEvent(mes);
   targets->Delete(i);i--;
   delete ptr;
   continue;
  }
  if(target->GetSkillDays(skill)>=420){
   mes=unit->FullName()+" TEACH warning: unit ";
   mes+=target->FullName()+" does not need teaching";
   AddEvent(mes);
  }
  students+=target->GetMen();
 }
 if(!students)return;
 int maxstud=unit->GetMen()*10;
 if(students>maxstud){
  mes=unit->FullName()+" TEACH warning: to many students";
  AddEvent(mes);
 }
 int days=30*maxstud,dif;

 for(int i=0;i<targets->Count;i++){
  AUnitPtr *ptr=GetTarget(i);
  AUnit *u=ptr->ptr;
  int umen=u->GetMen();
  if(!umen)continue;
  int tempdays=(umen*days)/students;
  if(tempdays>30*umen) tempdays=30*umen;
  days-=tempdays;
  students-=umen;
  AOrderAbr *o=dynamic_cast<AOrderAbr*>(u->orders->Find(O_STUDY,2));
  o->number+=tempdays;
  if(o->number>60*umen){
   dif=o->number-60*umen;
   if(tempdays==dif){
    mes=unit->FullName()+" TEACH error: unit ";
    mes+=u->FullName()+" is already teached";
    AddEvent(mes);
   }
   days+=dif;
   o->number=60*umen;
  }else dif=0;
  dif=(tempdays-dif)/umen;
  AOrderMessage *m=new AOrderMessage(0);
  if(bases){
   foreach(bases){
    AOrder *o=*(AOrder**)iter;
    AOrderTeach *ord=dynamic_cast<AOrderTeach*>(o);
    for(int i=0;i<ord->targets->Count;i++){
     AUnitPtr *ptr2=ord->GetTarget(i);
     if(ptr->num!=ptr2->num)continue;
     m->base=ord;
     ptr2->mes=m; 
     break;
    }
    if(m->base)break;
   }
  }
  mes="Taught by ";
  mes+=unit->FullName();
  mes+=" ";
  mes+=dif;
  mes+=" days";
  m->text=mes;
  m->target=unit; 
  u->orders->Add(m);
  
//
 }
}
AOrder * AOrderTeach::Edit()
{
 AUnit *unit=orders->unit;
 if(!unit)return 0;
 ARegion *reg=unit->baseobj->basereg;
 if(!reg)return 0;
 AUnits *units=new AUnits(orders->unit->baseobj);
 units->autodelete=false;
 AUnits *newun=new AUnits(orders->unit->baseobj);
 isedited=true;
 reg->RunOrders();
 isedited=false;
 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un==unit)continue;
/*   if(unit->endfaction){
    if(!un->endfaction)continue;*/
   if(unit->endfaction->GetAttitude(un->endfaction->number)<A_FRIENDLY)continue;
//   }
   AOrderAbr *study=dynamic_cast<AOrderAbr*>(un->orders->Find(O_STUDY,2));
   if(!study)continue;
   AnsiString skill=study->abr;
   if(unit->GetRealSkill(skill)<=un->GetRealSkill(skill)) continue;
   units->Add(un);
  }
 }
 for(int i=targets->Count-1;i>=0;i--){
  AUnitPtr *ptr=GetTarget(i);
  if(ptr->ptr) continue;
  if(newun->GetUnit(ptr->num))continue;
  AUnit *un=reg->GetUnit(ptr->num);
  if(!un){
   un=new AUnit(unit->turn);
   un->num=ptr->num;
   newun->Add(un);
  }
  ptr->ptr=un;
 }
 TEditOrderTeachForm *frm=new TEditOrderTeachForm(0,this,units);
 frm->ShowModal();
 AOrder *ord=frm->ord;
 delete frm;
 delete newun;
 for(int i=targets->Count-1;i>=0;i--){
  AUnitPtr *ptr=GetTarget(i);
  ptr->ptr=0;
 }
 delete units;
 return ord;
}
void AOrderTeach::DeleteMessage(AOrderMessage *mes){
 AUnit *tar=mes->orders->unit;
 for(int i=targets->Count-1;i>=0;i--){
  AUnitPtr *ptr=GetTarget(i);
  if(ptr->ptr==tar){
   ptr->ptr=0;
   if(ptr->mes!=mes){
    ptr->mes->orders->Delete(ptr->mes);
   }
   ptr->mes=0;
   mes->base=0;
   mes->orders->Delete(mes);
  }
 }
}
void AOrderTeach::Clear()
{
 for(int i=targets->Count-1;i>=0;i--){
  AUnitPtr *ptr=GetTarget(i);
  targets->Delete(i);
  if(ptr->mes){
   ptr->mes->base=0;
   ptr->mes->orders->Delete(ptr->mes);
  }
  delete ptr;
 }
}
AUnitPtr * AOrderTeach::GetTarget(int index)
{
 return (AUnitPtr*)targets->Items[index];
}
void AOrderTeach::AddTarget(int num, AUnit *target)
{
 if(!target){
  if(orders)target=orders->unit->baseobj->basereg->GetUnit(num);
 }
 for(int i=targets->Count-1;i>=0;i--){
  AUnitPtr *ptr=GetTarget(i);
  if(target&&ptr->ptr==target)return;
  if(ptr->num==num){
   ptr->ptr=target;
   return;
  }
 }
 AUnitPtr *ptr=new AUnitPtr;
 ptr->ptr=target;
 ptr->num=num;
 ptr->mes=0;
 targets->Add(ptr);
}
void AOrderTeach::VerTargets()
{
 for(int i=targets->Count-1;i>=0;i--){
  AUnitPtr *ptr=GetTarget(i);
  if(ptr->ptr) continue;
  ptr->ptr=orders->unit->baseobj->basereg->GetUnit(ptr->num);
  if(ptr->ptr) continue;
  if(ptr->mes){
   ptr->mes->orders->Delete(ptr->mes);
   ptr->mes=0;
  }
  AnsiString mes;
  mes=orders->unit->FullName();
  mes+=" TEACH Error: unit ";
  AUnit *un=new AUnit(orders->unit->turn);
  un->num=ptr->num;
  un->faction=0;
  mes+=un->Alias()+" is not found";
  delete un;
  AddEvent(mes);
 }
}
bool AOrderTeach::IsMonthLong(){
 return !commented;
}
void AOrderTeach::Prepare(){
 VerTargets();
 int kol=0;
 for(int i=targets->Count-1;i>=0;i--){
  if(GetTarget(i)->ptr)kol++;
 }
 if(!kol){
  AddEvent(orders->unit->FullName()+" TEACH: No students");
 }
}
AOrderInt::AOrderInt()
 :value(0)
{
}
AOrderInt::~AOrderInt()
{
}
void AOrderInt::Read(TFile & in)
{
 AOrder::Read(in);
 in>>value;
}
void AOrderInt::Write(TFile & out)
{
 AOrder::Write(out);
 out<<value;
}
AnsiString AOrderInt::Print()
{
 AnsiString s=Begin()+FindOrderKey(type);
 if(type==O_CLAIM){
  s+=value;
  return s;
 }
 if(type==O_ENTER){
  AObject *obj=orders->unit->baseobj->basereg->GetObject(value);
  if(value&&obj) s+=obj->FullName();
  else{
   s+="?";
   s+=value;
  }
  return s;
 }
 if(commented) return s;

 return s;
}
AnsiString AOrderInt::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(type);
 if(type==O_CLAIM||type==O_ENTER){
  s+=value;
  return s;
 }

 return s;
}
bool AOrderInt::ParseOrder(char * s)
{
 if(type==O_CLAIM||type==O_ENTER){
  AnsiString str=GetNextToken(s);
  if(!str.Length()) return false;
  value=atoi(str.c_str());
  return true;
 }
 return false;
}
void AOrderInt::Run(int val)
{
 AnsiString mes;
 if(type==O_CLAIM) return;
 if(type==O_ENTER){
  AObject *obj=orders->unit->baseobj->basereg->GetObject(value);
  if(obj) orders->unit->endobj=obj;
  else{
   mes="Error ";
   mes+=orders->unit->FullName();
   mes+=" ENTER: object ¹";
   mes+=value;
   mes+=" does not exist";
   AddEvent(mes);
  }
 }
}
AOrder * AOrderInt::Edit()
{
 AnsiString text,caption;
 AOrder *ord=0;
 switch(type){
  case O_CLAIM:
  {
   TEditOrderIntForm *frm=new TEditOrderIntForm(0,this);
   frm->Caption="Edit CLAIM order";
   frm->labText->Caption="Money";
   frm->ShowModal();
   ord=frm->ord;
   delete frm;
  }
  break;
  case O_ENTER:
  {
   TList *list=new TList;
   ARegion *reg=orders->unit->baseobj->basereg;
   foreach(reg){
    AObject *obj=*(AObject**)iter;
    if(obj->num==0) continue;
    list->Add(obj);
   }
   TEditOrderEnterForm *frm=new TEditOrderEnterForm(0,this,list);
   frm->ShowModal();
   ord=frm->ord;
   delete frm;
   delete list;
  }
  break;
 }
 return ord;
}
int AOrderInt::Productivity()
{
 return 0;
}
AOrderMoving::AOrderMoving()
{
 dirs=new TList;
}
AOrderMoving::~AOrderMoving()
{
 delete dirs;
}
void AOrderMoving::Read(TFile & in)
{
 AOrder::Read(in);
 int kol,dir;
 in>>kol;
 for(int i=0;i<kol;i++){
  in>>dir;
  Add(dir);
 }
 in>>custom;
}
void AOrderMoving::Write(TFile & out)
{
 AOrder::Write(out);
 int kol=count,dir;
 out<<kol;
 for(int i=0;i<kol;i++){
  dir=Get(i);
  out<<dir;
 }
 out<<custom;
}
AnsiString AOrderMoving::Print()
{
 return WriteTemplate();
}
AnsiString AOrderMoving::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(type);
 if(type==O_SAIL) s+=" ";
 for(int i=0;i<count;i++){
  if(i) s+=" ";
  int dir=Get(i);
  if(dir<MOVE_IN){
   ADirType *dtype=DirTypes->Get(dir);
   if(dtype)
    s+=dtype->abr; 
  }else if(dir==MOVE_IN){
   s+=keyIn;
  }else if(dir==MOVE_OUT){
   s+=keyOut;
  }else{
   s+=dir-MOVE_OUT;
  }
 }
 if(custom.Length()){
  s+=" "+custom;
 }
 return s;
}
bool AOrderMoving::ParseOrder(char *s)
{
 AnsiString str;
 int dir;
 while(*s){
  str=GetNextToken(s);
  if(!str.Length()) return false;
  if(isdigit(str[1])){
   dir=MOVE_OUT+atoi(str.c_str());
  }else if(!str.AnsiCompareIC(keyIn)){
   dir=MOVE_IN;
  }else if(!str.AnsiCompareIC(keyOut)){
   dir=MOVE_OUT;
  }else{
   dir=DirTypes->Find(str);
   if(dir<0) return false;
  }
  Add(dir);
  while(*s==' ') s++;
 }
 if(type!=O_SAIL&&!count) return false;
 return true;
}
int movingdx[NDIRS]={
 0, 1, 1, 0, -1, -1
};
int movingdy[NDIRS]={
 -2, -1, 1, 2, 1, -1
};
void AOrderMoving::Run(int value)
{
 AnsiString mes;
 int dir;
 AUnit *unit=orders->unit;
 AObject *curobj=unit->endobj, *nextobj;
 ARegion *nextreg, *curreg=curobj->endreg;
 int movecost;
 if(type==O_SAIL){
  if(!count){
   if(!isedited){
    mes="Error Sailing: ";
    mes+=unit->FullName();
    mes+=" has empty sailing order";
    AddEvent(mes);
   }
//   return;
  }
  AUnits *units=new AUnits(curobj);
  units->autodelete=false;
  int movepoints=4;
  int wgt=0,slr=0;
  foreach(curobj->units){
   AUnit *un=*(AUnit**) iter;
   if(un->endobj!=curobj) continue;
   wgt+=un->weight;
   if(!un->orders->Find(O_SAIL,2)) continue;
   slr+=un->GetSkill("SAIL")*un->GetMen();
   units->Add(un);
  }
  curobj->cursailors=slr;
  if(wgt>curobj->Capacity()){
   mes="Error SAIL: ";
   mes+=unit->FullName();
   mes+=": Ship is overloaded";
   AddEvent(mes);
   movepoints=0;
  }else if(slr<curobj->Sailors()){
   mes="Error SAIL: ";
   mes+=unit->FullName();
   mes+=": Not enough sailors";
   AddEvent(mes);
   movepoints=0;
  }
  if(movepoints) for(int i=0;i<count;i++){
   dir=Get(i);
   if(dir>=NDIRS){
    mes="Error SAIL: ";
    mes+=unit->FullName();
    mes+=" must be moved in normal direction";
    AddEvent(mes);
    break;
   }
   nextreg=curRegionList->Get(curreg->neighbors[dir]);
   if(!nextreg){
    int newx=curreg->xloc+movingdx[dir];
    int newy=curreg->yloc+movingdy[dir];
    nextreg=curRegionList->Get(newx, newy,curreg->zloc);
    if(!nextreg){
     mes="Error SAIL: ";
     mes+=unit->FullName();
     mes+=": region does not exist or is not explored";
     AddEvent(mes);
     break;
    }
   }
   if(curobj->type!="Balloon"&&!nextreg->IsCoastal()){
    mes="Error SAIL: ";
    mes+=unit->FullName();
    mes+=": can't sail inland";
    AddEvent(mes);
    break;
   }
   if(curobj->type!="Balloon"&&curreg->type!="ocean"&&nextreg->type!="ocean"){
    mes="Error SAIL: ";
    mes+=unit->FullName();
    mes+=": can't sail inland";
    AddEvent(mes);
    break;
   }
   movecost=1;
   if(nextreg->NxtWeather()==""){
//    nextreg->PrepareOrders();
    throw Exception("Sail error");
   }
   if(nextreg->NxtWeather()!="Normal") movecost=2;
   movepoints-=movecost;
   if(movepoints<0){
    mes="Error SAIL: ";
    mes+=unit->FullName();
    mes+=" can't sail that far";
    AddEvent(mes);
    break;
   }
   curreg=nextreg;
  }
  curobj->endreg=curreg;

  foreach(units){
   AUnit *un=*(AUnit**) iter;
   un->movepoints=movepoints;
  }
  delete units;
  return;
 }
 if(curobj->endreg!=curobj->basereg){
  mes="Error MOVE: ";
  mes+=unit->FullName();
  mes+=" can't move while aboard a sailing ship";
  AddEvent(mes);
  return;
 }
 if(!count){
  if(isedited) return;
  mes="Error MOVE: ";
  mes+=unit->FullName();
  mes+=" has empty MOVE order";
  AddEvent(mes);
  return;
 }
 int movetype=unit->MoveType();
 for(int i=0;i<count;i++){
  dir=Get(i);
  if(dir<NDIRS||dir==MOVE_IN){
   if(dir==MOVE_IN){
    if(!curobj->inner){
     mes="Error MOVE: ";
     mes+=unit->FullName();
     mes+=": Can't move IN there";
     AddEvent(mes);
     break;
    }
    nextreg=curRegionList->Get(curobj->endreg->inner);
    if(!nextreg){
     mes="Error MOVE: ";
     mes+=unit->FullName();
     mes+=": an unknown inner region";
     AddEvent(mes);
     break;
    }
   }else{
    nextreg=curRegionList->Get(curobj->endreg->neighbors[dir]);
    if(!nextreg){
     int newx=curobj->endreg->xloc+movingdx[dir];
     int newy=curobj->endreg->yloc+movingdy[dir];
     ARegionArray *ar=curRegionList->GetRegionArray(curobj->endreg->zloc);
     newx=(newx+ar->x)%ar->x;
     newy=(newy+ar->y)%ar->y;
     nextreg=curRegionList->Get(newx, newy,curobj->endreg->zloc);
     if(!nextreg){
      mes="Error MOVE: ";
      mes+=unit->FullName();
      mes+=": region does not exist or not explored";
      AddEvent(mes);
      break;
     }/*else{
      mes="Warning MOVE: ";
      mes+=unit->FullName();
      mes+=": region not explored";
      AddEvent(mes);
     }*/
    }
   }
   if(nextreg->type=="ocean"){
     mes="Error MOVE: ";
     mes+=unit->FullName();
     mes+=": Can't move while in the ocean";
     AddEvent(mes);
     break;
   }
   movecost=nextreg->MoveCost(movetype);
   if(movecost>1&&movetype!=M_FLY&&curobj->endreg->HasExitRoad(dir) &&
            nextreg->HasConnectingRoad(dir))
   {
    movecost/=2;
   }
   if((unit->movepoints-=movecost)<0){
    mes="Error MOVE: ";
    mes+=unit->FullName();
    mes+=" does not have enough movement points.";
    AddEvent(mes);
    break;
   }
// to do
   curobj=nextreg->GetDummy();
  }else if(dir<MOVE_IN){
   mes="Error MOVE: ";
   mes+=unit->FullName();
   mes+=" unknown direction";
   AddEvent(mes);
   break;
  }else if(dir==MOVE_OUT){
   curobj=curobj->endreg->GetDummy();
  }else{
   int bnum=dir-MOVE_OUT;
   nextobj=curobj->endreg->GetObject(bnum);
   if(!nextobj){
    mes="Error MOVE: ";
    mes+=unit->FullName();
    mes+=": object ¹";
    mes+=bnum;
    mes+=" does not exist";
    AddEvent(mes);
    break;
   }
   curobj=nextobj;
  }
 }
 unit->endobj=curobj;
 if(custom.Length()){
  mes="Warning MOVE: ";
  mes+=unit->FullName();
  mes+=" has custom MOVE order";
  AddEvent(mes);
 }
}
AOrder * AOrderMoving::Edit()
{
 AOrder *ord;
 TEditOrderMovingForm *frm=new TEditOrderMovingForm(0,this);
 frm->ShowModal();
 ord=frm->ord;
 return ord;
}
int AOrderMoving::Get(int ind)
{
 return (int)dirs->Items[ind];
}
void AOrderMoving::Add(int dir)
{
 dirs->Add((void*)dir);
}
void AOrderMoving::Delete(int ind)
{
 dirs->Delete(ind);
}
bool AOrderMoving::IsMonthLong()
{
 return !commented;
}

AOrderAutoGive::AOrderAutoGive()
 :number(0),item("SILV")
{
 type=O_AUTOGIVE;
}
AOrderAutoGive::~AOrderAutoGive()
{
}
void AOrderAutoGive::Read(TFile & in)
{
// AOrder::Read(in);
 in>>number;
 ReadStringAsInt(in,item);
 in>>begstr;
 in>>substr;
}
void AOrderAutoGive::Write(TFile & out)
{
 WriteIntAsByte(out,type);
// AOrder::Write(out);
 out<<number;
 WriteStringAsInt(out,item);
 out<<begstr;
 out<<substr;
}
AnsiString AOrderAutoGive::Print()
{
 AnsiString s=keyAutoOrder+FindOrderKey(O_GIVE);
 if(begstr.Length()){
  s+='\"';
  s+=begstr+'\"';
 }
 if(substr.Length()){
  s+="+\""+substr+'\"';
 }
 s+=' ';
 if(number==0){
  s+="all";
 }else if(number<0){
  s+="except ";
  s+=-number;
 }else{
  s+=number;
 }
 s+=' ';
 AItemType *itype=ItemTypes->Get(item);
 if(!itype){
  s+=item;
 }else{
  if(itype->names.Length())
   s+=itype->names;
  else
   s+=itype->name;
 }
 s+=" (autoorder)";
 return s;
}
AnsiString AOrderAutoGive::WriteTemplate()
{
 AnsiString s=keyAutoOrder+FindOrderKey(O_GIVE);
 if(begstr.Length()){
  s+='\"';
  s+=begstr+'\"';
 }else if(!substr.Length()){
  s=orders->unit->FullName()+": AUTOGIVE: no target selected";
  AddEvent(s);
  throw Exception(s);
 }
 if(substr.Length()){
  s+="+\""+substr+'\"';
 }
 s+=' ';
 s+=number;
 s+=' ';
/* AItemType *itype=ItemTypes->Get(item);
 if(!itype){
  s+=item;
 }else{*/
 s+=item;
// }
 return s;
}
bool AOrderAutoGive::ParseOrder(char * s)
{
 begstr.SetLength(0);
 if(*s=='\"'){
  begstr=GetNextToken(s);
 }
 if(*s=='+'&&s[1]=='\"'){
  s++;
  substr=GetNextToken(s);
 }else if(!begstr.Length()) return false;
 if(*s!=' ') return false;
 s++;
 number=atoi(GetNextToken(s).c_str());
 if(*s!=' ') return false;
 s++;
 AnsiString it=GetNextToken(s);
 AItemType *itype=ItemTypes->Get(it);
 if(!itype) return false;
 item=itype->abr;
 return true;
}
/*void AOrderAutoGive::Run(int value)
{
}*/
AOrder * AOrderAutoGive::Edit()
{
 AOrder *ord;
 TEditOrderAutoGiveForm *frm=new TEditOrderAutoGiveForm(0,this);
 frm->ShowModal();
 ord=frm->ord;
 return ord;
}
AOrderBuild::AOrderBuild()
 :work(0)
{
 type=O_BUILD;
}
AOrderBuild::~AOrderBuild()
{
}
void AOrderBuild::Read(TFile & in)
{
 AOrder::Read(in);
 in>>object;
}
void AOrderBuild::Write(TFile & out)
{
 AOrder::Write(out);
 out<<object;
}
AnsiString AOrderBuild::Print()
{
 AnsiString s=WriteTemplate();
 if(!commented){
  s+=" ;";
  s+=work;
 }
 return s;
}
AnsiString AOrderBuild::WriteTemplate()
{
 AnsiString s=Begin()+FindOrderKey(O_BUILD);
 if(object.Length()==0) return s;
 s+=" \""+object+"\"";
 return s;
}
bool AOrderBuild::ParseOrder(char * s)
{
 if(*s==0){
  object="";
  return true;
 }
 AnsiString objname=GetNextToken(s);
 AObjectType *otype=ObjectTypes->Get(objname);
 if(!otype) return false;
 object=otype->name;
 while(*s==' ')s++;
 return *s==0;
}
void AOrderBuild::Run(int value)
{
 work=0;
 AnsiString mes;
 AObject *obj=0;
 AObjectType *otype;
 int needed;
 AUnit *unit=orders->unit;
 if(!object.Length()){
  obj=unit->endobj;
  otype=ObjectTypes->Get(obj->type);
  needed=obj->endincomplete;
 }else{
  otype=ObjectTypes->Get(object);
  needed=otype->cost;
 }
 if(!otype){
  mes="Error BUILD: ";
  mes+=unit->FullName();
  mes+=" internal error";
  AddEvent(mes);
  return;
 }
 AnsiString sk=otype->skill;
 if(!sk.Length()){
  mes="Error BUILD: ";
  mes+=unit->FullName();
  mes+=" can't build ";
  mes+=otype->name;
  AddEvent(mes);
  return;
 }
 int usk=unit->GetSkill(sk);
 if(usk<otype->level){
  mes="Error BUILD: ";
  mes+=unit->FullName();
  mes+=" not skilled to build ";
  mes+=otype->name;
  AddEvent(mes);
  return;
 }
 if(!needed){
  mes="Error BUILD: ";
  mes+=unit->FullName();
  mes+=": Object is finished";
  AddEvent(mes);
  return;
 }
 AnsiString it=otype->item;
 int itn;
 if(it==WoodOrStoneStr){
  itn=unit->enditems->GetNum("WOOD")+unit->enditems->GetNum("STON");
 }else{
  itn=unit->enditems->GetNum(it);
 }
 if(itn==0){
  mes="Error BUILD: ";
  mes+=unit->FullName();
  mes+=" don't have the required item "+it;
  AddEvent(mes);
  return;
 }
 int num=unit->GetMen()*usk;
 if(num>needed) num=needed;
 if(itn<num){
  num=itn;
  mes="Warning BUILD: ";
  mes+=unit->FullName();
  mes+=" don't have enough materials";
  AddEvent(mes);
 }
 work=num;
 if(it==WoodOrStoneStr){
  int snum=unit->enditems->GetNum("STON");
  if(num>snum) {
   num-=snum;
   unit->enditems->SetNum("STON",0);
   unit->enditems->SetNum("WOOD",unit->enditems->GetNum("WOOD")-num);
  }else{
   unit->enditems->SetNum("STON",snum - num);
  }
 }else{
  unit->enditems->SetNum(it,itn - num);
 }
 if(obj){
  obj->endincomplete-=num;
 }
}
AOrder * AOrderBuild::Edit()
{
 AOrder *ord;
 orders->unit->baseobj->basereg->RunOrders(rsBeforeBuild);   
 TEditOrderBuildForm *frm=new TEditOrderBuildForm(0,this);
 frm->ShowModal();
 ord=frm->ord;
 return ord;
}
bool AOrderBuild::IsMonthLong(){
 return true;
}

/*
AOrderCustom::AOrderCustom()
{
 type=O_;
}
AOrderCustom::~AOrderCustom()
{
}
void AOrderCustom::Read(TFile & in)
{
 AOrder::Read(in);
}
void AOrderCustom::Write(TFile & out)
{
 AOrder::Write(out);
}
AnsiString AOrderCustom::Print()
{
 return Begin();
}
AnsiString AOrderCustom::WriteTemplate()
{
 return Begin();
}
bool AOrderCustom::ParseOrder(char * s)
{
 return false;
}
void AOrderCustom::Run(int value)
{
}
AOrder * AOrderCustom::Edit()
{
 return 0;
}
int AOrderCustom::Productivity()
{
 return 0;
}
*/


int AOrder::GetSize()
{
 int siz=sizeof(*this);
// siz+=Capacity*4;
 return siz;
}
int AOrders::GetSize()
{
 int siz=sizeof(*this);
 siz+=Capacity*4;
 foreach(this){
  AOrder *ord=*(AOrder**)iter;
  siz+=ord->GetSize()+0;
 }
 return siz;
}
int AOrderCustom::GetSize(){
 int siz=sizeof(*this);
 siz+=text.Length()+1;
 return siz;
}
int AOrderComment::GetSize(){
 int siz=sizeof(*this);
 siz+=text.Length()+1;
 return siz;
}
int AOrderLocDescr::GetSize(){
 int siz=sizeof(*this);
 siz+=text.Length()+1;
 return siz;
}
int AOrderSimple::GetSize(){
 int siz=sizeof(*this);
 return siz;
}
int AOrderByte::GetSize(){
 int siz=sizeof(*this);
 return siz;
}
int AOrderAbr::GetSize(){
 int siz=sizeof(*this);
 siz+=abr.Length()+1;
 return siz;
}
int AOrderGive::GetSize(){
 int siz=sizeof(*this);
 siz+=item.Length()+1;
 return siz;
}
int AOrderMessage::GetSize(){
 int siz=sizeof(*this);
 siz+=text.Length()+1;
 return siz;
}
int AOrderBuySell::GetSize(){
 int siz=sizeof(*this);
 siz+=item.Length()+1;
 return siz;
}
int AOrderTeach::GetSize(){
 int siz=sizeof(*this);
 siz+=sizeof(TList);
 siz+=targets->Capacity*4;
 siz+=targets->Count*sizeof(AUnitPtr);
 return siz;
}
int AOrderInt::GetSize(){
 int siz=sizeof(*this);
 return siz;
}
int AOrderMoving::GetSize(){
 int siz=sizeof(*this);
 siz+=sizeof(TList);
 siz+=dirs->Capacity*4;
 siz+=custom.Length()+1;
 return siz;
}
int AOrderAutoGive::GetSize(){
 int siz=sizeof(*this);
 siz+=item.Length()+1;
 siz+=begstr.Length()+1;
 siz+=substr.Length()+1;
 return siz;
}
int AOrderBuild::GetSize(){
 int siz=sizeof(*this);
 siz+=object.Length()+1;
 return siz;
}
/*
int AOrder::GetSize(){
 int siz=sizeof(*this);
 return siz;
}
*/




