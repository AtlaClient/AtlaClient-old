//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <ctype.h>
#include "exprlang.h"
#include "util.h"
#include "unit.h"
#include "turn.h"
#include "faction.h"
#include "region.h"
#include "markets.h"
#include "items.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
static ARegion *reg=0;
void initregfactions(EVariable &var);
void initlocalfactions(EVariable &var);
void initlocalobservation(EVariable &var);
void initregioninfoage(EVariable &var);
void initgatenumber(EVariable &var);
void initconsumetype(EVariable &var);

EVariable variables[]={
 EVariable("presentfaction",initregfactions),
 EVariable("guardfaction",initregfactions),
 EVariable("localfaction",initlocalfactions),
 EVariable("localobservation",initlocalobservation),
 EVariable("regioninfoage",initregioninfoage),
 EVariable("gatenumber",initgatenumber),
 EVariable("consumetype",initconsumetype),
};
int VarsCount=sizeof(variables)/sizeof(variables[0]);

EData* productamount(char*&buf);
EData* saleamount(char*&buf);
EData* saleprice(char*&buf);
EData* wantedamount(char*&buf);
EData* wantedprice(char*&buf);
EData* localobservation(char*&buf);
EData* presentitem(char*&buf);
EData* presentnonlocalitem(char*&buf);
EData* presentlocalitem(char*&buf);
EFunction functions[]={
 EFunction("productamount","(\"\")",productamount),
 EFunction("saleamount","(\"\")",saleamount),
 EFunction("saleprice","(\"\")",saleprice),
 EFunction("wantedamount","(\"\")",wantedamount),
 EFunction("wantedprice","(\"\")",wantedprice),
 EFunction("presentitem","(\"\")",presentitem),
 EFunction("presentlocalitem","(\"\")",presentlocalitem),
 EFunction("presentnonlocalitem","(\"\")",presentnonlocalitem),
};
int FuncsCount=sizeof(functions)/sizeof(functions[0]);

int FindMas(ints &mas, int val){
 for(unsigned i=0;i<mas.size();i++){
  if(mas[i]==val) return i;
 }
 return -1;
}
void AddMas(ints &mas, int val){
 if(FindMas(mas,val)!=-1) return;
 mas.push_back(val);
}
EData::EData(){
}
EData::~EData(){
}
AnsiString EData::GetDataTypeName()
{
 throw Exception("Data::GetDataTypeName");
}
int EData::IsEqual(EData * b)
{
 throw Exception("Data::IsEqual");
}
int EData::IsNotEqual(EData* b)
{
 throw Exception("Data::IsNotEqual");
}
int EData::IsLess(EData* b)
{
 throw Exception("Data::IsLess");
}
int EData::IsGreat(EData * b)
{
 throw Exception("Data::IsGreat");
}
int EData::IsNot()
{
 throw Exception("Data::IsNot");
}
int EData::IsAnd(EData* b)
{
 throw Exception("Data::IsAnd");
}
int EData::IsOr(EData * b)
{
 throw Exception("Data::IsOr");
}
int EData::GetValue()
{
 throw Exception("Data::GetValue");
}
EData * EData::CreateCopy()
{
 throw Exception("Data::CreateCopy");
}
EDataInt::EDataInt()
 :value(0)
{
 type=dtInt;
}
EDataInt::~EDataInt()
{
}
AnsiString EDataInt::GetDataTypeName()
{
 return "Int";
}
int EDataInt::IsEqual(EData * b)
{
 if(b->type!=dtInt)
  throw Exception("Int == "+b->GetDataTypeName());
 return value==b->GetValue();
}
int EDataInt::IsNotEqual(EData* b)
{
 if(b->type!=dtInt)
  throw Exception("Int != "+b->GetDataTypeName());
 return value!=b->GetValue();
}
int EDataInt::IsLess(EData* b)
{
 if(b->type!=dtInt)
  throw Exception("Int < "+b->GetDataTypeName());
 return value<b->GetValue();
}
int EDataInt::IsGreat(EData * b)
{
 if(b->type!=dtInt)
  throw Exception("Int > "+b->GetDataTypeName());
 return value>b->GetValue();
}
int EDataInt::IsNot()
{
 return !value;
}
int EDataInt::IsAnd(EData* b)
{
 if(b->type!=dtInt)
  throw Exception("Int && "+b->GetDataTypeName());
 return value && b->GetValue();
}
int EDataInt::IsOr(EData * b)
{
 if(b->type!=dtInt)
  throw Exception("Int || "+b->GetDataTypeName());
 return value || b->GetValue();
}
int EDataInt::GetValue()
{
 return value;
}
EData* EDataInt::CreateCopy()
{
 EDataInt *n=new EDataInt;
 n->value=value;
 return n;
}

EDataInts::EDataInts()
{
 type=dtInts;
}
EDataInts::~EDataInts()
{
}
AnsiString EDataInts::GetDataTypeName()
{
 return "Ints";
}
int EDataInts::IsEqual(EData * b)
{
 if(b->type==dtInts){
  EDataInts *d=dynamic_cast<EDataInts*>(b);
  if(!d)
   throw Exception("Ints == "+b->GetDataTypeName()+" bad conversion");
  for(unsigned i=0;i<d->values.size();i++){
   if(FindMas(values,d->values[i])>=0) return true;
  }
  return false;
 }
 if(b->type!=dtInt)
  throw Exception("Ints == "+b->GetDataTypeName());
 return FindMas(values,b->GetValue())!=-1;
}
int EDataInts::IsNotEqual(EData* b)
{
 if(b->type==dtInts){
  return !IsEqual(b);
 }
 if(b->type!=dtInt)
  throw Exception("Ints != "+b->GetDataTypeName());
 return FindMas(values,b->GetValue())==-1;
}
int EDataInts::IsLess(EData* b)
{
 if(b->type==dtInts)
  return b->IsGreat(this);
 throw Exception("DataInts::IsLess "+b->GetDataTypeName());
}
int EDataInts::IsGreat(EData * b)
{
 if(b->type!=dtInts)
  throw Exception("Ints > "+b->GetDataTypeName());
 EDataInts *d=dynamic_cast<EDataInts*>(b);
 if(!d)
  throw Exception("Ints > "+b->GetDataTypeName()+" bad conversion");
 for(unsigned i=0;i<d->values.size();i++){
  if(FindMas(values,d->values[i])<0) return false;
 }
 return true;
}
int EDataInts::GetValue()
{
 throw Exception("DataInts::GetValue");
}
EData * EDataInts::CreateCopy()
{
 EDataInts *n=new EDataInts;
 n->values=values;
 return n;
}
EDataString::EDataString()
{
 type=dtString;
}
EDataString::~EDataString()
{
}
int EDataString::IsEqual(EData* b)
{
 if(b->type!=dtString)
  throw Exception("String == "+b->GetDataTypeName());
 EDataString *d=dynamic_cast<EDataString*>(b);
 if(!d)
  throw Exception("String == String Converion error");
 return value.AnsiCompareIC(d->value)==0;
}
int EDataString::IsNotEqual(EData* b)
{
 if(b->type!=dtString)
  throw Exception("String != "+b->GetDataTypeName());
 EDataString *d=dynamic_cast<EDataString*>(b);
 if(!d)
  throw Exception("String != String Converion error");
 return value.AnsiCompareIC(d->value)!=0;
}
EData* EDataString::CreateCopy()
{
 EDataString *n=new EDataString;
 n->value=value;
 return n;
}

EData *ExprLast(char *&buf);
EData *Expr1(char*&buf){
 AnsiString str;
 while(*buf==' ') buf++;
 switch(*buf){
  case 0: return 0;
  case '0':case '1':case '2':case '3':case '4':
  case '5':case '6':case '7':case '8':case '9':{
   EDataInt *expr=new EDataInt;
   expr->value=atoi(buf);
   GetNextToken(buf);
   return expr;
  }
  case '\"':{
   EDataString *e=new EDataString;
   e->value=GetNextToken(buf);
   return e;
  }
  case '!':{
   buf++;
   EData *e=ExprLast(buf);
   if(!e)
    throw Exception("\'Not\' error");
   EDataInt *expr=new EDataInt;
   expr->value=e->IsNot();
   delete e;
   return expr;
  }
  case '-':{
   buf++;
   EData *e=ExprLast(buf);
   if(!e)
    throw Exception("\Unary '-\' error");
   if(e->type!=dtInt)
    throw Exception("\Unary '-\' error: not int");
   EDataInt *expr=new EDataInt;
   expr->value=-e->GetValue();
   delete e;
   return expr;
  }

  case '(':{
   buf++;
   EData *e=ExprLast(buf);
   if(!e)
    throw Exception("Brackes error");
   EDataInt *expr=new EDataInt;
   if(*buf!=')')
    throw Exception("Brackes error: can't find another brakes");
   buf++;
   expr->value=e->GetValue();
   delete e;
   return expr;
  }
  case '{':{
   buf++;
   EData *e=ExprLast(buf);
   if(!e)
    throw Exception("{ error");
   if(e->type!=dtInt)
    throw Exception("{ error: bad type "+e->GetDataTypeName());
   EDataInts *mas=new EDataInts;
   AddMas(mas->values,e->GetValue());
   EDataTypes type=e->type;
   delete e;
   while(*buf!='}'){
    if(*buf!=',')
     throw Exception("{ error: } or , not found");
    buf++;
    e=ExprLast(buf);
    if(e->type!=type)
     throw Exception("{ error: type changing");
    AddMas(mas->values,e->GetValue());
    delete e;
   }
   buf++;
   return mas;
  }
  default:{
   str=GetNextToken(buf);
   int i;
   if(*buf!='('){
    for(i=0;i<VarsCount;i++){
     if(variables[i].name.AnsiCompareIC(str)==0) break;
    }
    if(i==VarsCount)
     throw Exception("Unknown variable "+str);
    EData *e1=variables[i].data;
    if(!e1){
     variables[i].Init(variables[i]);
     e1=variables[i].data;
    }
    if(!e1)
     throw Exception("Variable "+str+" can't initialing");
    return e1->CreateCopy();
   }
   buf++;
   for(i=0;i<FuncsCount;i++){
    if(functions[i].name.AnsiCompareIC(str)==0) break;
   }
   if(i==FuncsCount)
    throw Exception("Unknown function "+str);
   EData *e=functions[i].func(buf);
   if(*buf!=')'||e==0)
    throw Exception("Error call function "+str);
   buf++;
   return e;
  }
 }
// return 0;
}
EData *Expr2(char*&buf){
 EData *left=Expr1(buf);
 int usl=0;
 if(buf[0]=='='&&buf[1]=='='){
  buf+=2;
  usl=1;
 }else if(buf[0]=='!'&&buf[1]=='='){
  buf+=2;
  usl=2;
 }else if(buf[0]=='>'){
  buf++;
  usl=3;
 }else if(buf[0]=='<'){
  buf++;
  usl=4;
 }
 if(usl==0) return left;
 EData *right=Expr1(buf);
 EDataInt *expr=new EDataInt;
 switch(usl){
  case 1:
   expr->value=left->IsEqual(right);
  break;
  case 2:
   expr->value=left->IsNotEqual(right);
  break;
  case 3:
   expr->value=left->IsGreat(right);
  break;
  case 4:
   expr->value=left->IsLess(right);
  break;
 }
 delete left;
 delete right;
 return expr;
}
EData *Expr3(char*&buf){
 EData *left=Expr2(buf);
 if(buf[0]!='&'||buf[1]!='&'){
  return left;
 }
 EDataInt *expr=new EDataInt;
 expr->value=left->GetValue();
 delete left;
 while(buf[0]=='&'&&buf[1]=='&'){
  buf+=2;
  EData *right=Expr2(buf);
  expr->value=expr->IsAnd(right);
  delete right;
 }
 return expr;
}
EData *Expr4(char*&buf){
 EData *left=Expr3(buf);
 if(buf[0]!='|'||buf[1]!='|'){
  return left;
 }
 EDataInt *expr=new EDataInt;
 expr->value=left->GetValue();
 delete left;
 while(buf[0]=='|'&&buf[1]=='|'){
  buf+=2;
  EData *right=Expr3(buf);
  expr->value=expr->IsOr(right);
  delete right;
 }
 return expr;
}

EData *ExprLast(char *&buf){
 return Expr4(buf);
}
void ExprPrepareRegion (ARegion *r){
 reg=r;
 for(int i=0;i<VarsCount;i++){
  variables[i].ClearData();
 }
}
bool ExprProcessExpr(AnsiString expr){
 char *s=expr.c_str();
 if(!*s) return false;
 EData *e=ExprLast(s);
 if(!e)
  throw Exception("ProcessExpr Error");
 if(*s)
  throw Exception("ProcessExpr Error: Bad end");
 bool ret=e->GetValue();
 delete e;
 return ret;
}

void initregfactions(EVariable &var){
 if(!reg) return;
 EDataInts *presfac=new EDataInts;
 EDataInts *guardfac=new EDataInts;
 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   int facnum=/*0;
   if(un->faction) facnum=*/un->faction->number;
   AddMas(presfac->values,facnum);
   if(un->guard==GUARD_GUARD) AddMas(guardfac->values,facnum);
  }
 }
 variables[0].data=presfac;
 variables[1].data=guardfac;
}
void initlocalfactions(EVariable &var){
 if(!reg) return;
 EDataInts *locfac=new EDataInts;
 AFactions *facs=reg->turn->factions;
 foreach(facs){
  AFaction *fac=*(AFaction**)iter;
  if(!fac->local) continue;
  AddMas(locfac->values,fac->number);
 }
 var.data=locfac;
}
void initlocalobservation(EVariable &var){
 if(!reg) return;
 EDataInt *locobs=new EDataInt;
 int maxobs=0,obs;
 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
//   if(!un->faction) continue;
   if(!un->faction->local) continue;
   obs=un->GetBegObservation();
   if(obs>maxobs) maxobs=obs;
  }
 }
 locobs->value=maxobs;
 var.data=locobs;
}
void initregioninfoage(EVariable &var){
 if(!reg) return;
 EDataInt *regia=new EDataInt;
 regia->value=curTurnNumber-reg->oldinfoturnnum;
 var.data=regia;
}
void initgatenumber(EVariable &var){
 if(!reg) return;
 EDataInt *gate=new EDataInt;
 gate->value=reg->gate;
 var.data=gate;
}
void initconsumetype(EVariable &var){
 if(!reg) return;
 int ct=0;
 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(!un->faction->local) continue;
   if(un->endflags&FLAG_CONSUMING_UNIT) ct|=1;
   if(un->endflags&FLAG_CONSUMING_FACTION) ct|=2;
  }
 }

 EDataInt *ctype=new EDataInt;
 ctype->value=ct;
 var.data=ctype;
}

EData* productamount(char*&buf){
 if(*buf!='\"')
  throw Exception("ProductAmount: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("ProductAmount: "+str+" is not item");
 str=itype->abr;
 e->value=0;
 foreach(reg->products){
  AMarket *prod=*(AMarket**)iter;
  if(prod->type!=str) continue;
  e->value=prod->amount;
 }
 return e;
}
EData* saleamount(char*&buf){
 if(*buf!='\"')
  throw Exception("saleamount: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("saleamount: "+str+" is not item");
 str=itype->abr;
 e->value=0;
 foreach(reg->saleds){
  AMarket *mark=*(AMarket**)iter;
  if(mark->type!=str) continue;
  e->value=mark->amount;
 }
 return e;
}
EData* saleprice(char*&buf){
 if(*buf!='\"')
  throw Exception("saleprice: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("saleprice: "+str+" is not item");
 str=itype->abr;
 e->value=-1;
 foreach(reg->saleds){
  AMarket *mark=*(AMarket**)iter;
  if(mark->type!=str) continue;
  e->value=mark->price;
 }
 return e;
}
EData* wantedamount(char*&buf){
 if(*buf!='\"')
  throw Exception("wantedamount: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("wantedamount: "+str+" is not item");
 str=itype->abr;
 e->value=0;
 foreach(reg->wanteds){
  AMarket *mark=*(AMarket**)iter;
  if(mark->type!=str) continue;
  e->value=mark->amount;
 }
 return e;
}
EData* wantedprice(char*&buf){
 if(*buf!='\"')
  throw Exception("wantedprice: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("wantedprice: "+str+" is not item");
 str=itype->abr;
 e->value=-1;
 foreach(reg->wanteds){
  AMarket *mark=*(AMarket**)iter;
  if(mark->type!=str) continue;
  e->value=mark->price;
 }
 return e;
}
EData* presentitem(char*&buf){
 if(*buf!='\"')
  throw Exception("presentitem: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("presentitem: "+str+" is not item");
 str=itype->abr;
 e->value=0;
 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   e->value+=un->items->GetNum(str);
  }
 }
 return e;
}
EData* presentlocalitem(char*&buf){
 if(*buf!='\"')
  throw Exception("presentlocalitem: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("presentlocalitem: "+str+" is not item");
 str=itype->abr;
 e->value=0;
 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(!un->faction->local) continue;
   e->value+=un->items->GetNum(str);
  }
 }
 return e;
}
EData* presentnonlocalitem(char*&buf){
 if(*buf!='\"')
  throw Exception("presentnonlocalitem: string needed");
 AnsiString str=GetNextToken(buf);
 EDataInt *e=new EDataInt;
 AItemType *itype=ItemTypes->Get(str);
 if(!itype)
  throw Exception("presentnonlocalitem: "+str+" is not item");
 str=itype->abr;
 e->value=0;
 foreach(reg){
  AObject *obj=*(AObject**)iter;
  foreach(obj->units){
   AUnit *un=*(AUnit**)iter;
   if(un->faction->local) continue;
   e->value+=un->items->GetNum(str);
  }
 }
 return e;
}

