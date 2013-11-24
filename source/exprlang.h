//---------------------------------------------------------------------------
#ifndef exprlangH
#define exprlangH
//---------------------------------------------------------------------------
#include <vector.h>


enum EDataTypes{
 dtInt=0,
 dtInts,
 dtString,


 dtMaxDT
};
typedef vector<int> ints;
class EData{
public:
    EDataTypes type;
    EData();
    virtual ~EData();
    virtual AnsiString GetDataTypeName();
    virtual int IsEqual(EData * b);
    virtual int IsNotEqual(EData* b);
    virtual int IsLess(EData* b);
    virtual int IsGreat(EData * b);
    virtual int IsNot();
    virtual int IsAnd(EData * b);
    virtual int IsOr(EData* b);
    virtual int GetValue();
    virtual EData * CreateCopy();
};
class EDataInt:public EData{
public:
    int value;

    EDataInt();
    virtual ~EDataInt();
    virtual AnsiString GetDataTypeName();
    virtual int IsEqual(EData * b);
    virtual int IsNotEqual(EData* b);
    virtual int IsLess(EData* b);
    virtual int IsGreat(EData * b);
    virtual int IsNot();
    virtual int IsAnd(EData * b);
    virtual int IsOr(EData* b);
    virtual int GetValue();
    virtual EData* CreateCopy();

};
class EDataInts:public EData{
public:
    ints values;

    EDataInts();
    virtual ~EDataInts();
    virtual AnsiString GetDataTypeName();
    virtual int IsEqual(EData * b);
    virtual int IsNotEqual(EData* b);
    virtual int IsLess(EData* b);
    virtual int IsGreat(EData * b);
    virtual int GetValue();
    virtual EData * CreateCopy();

};
class EDataString:public EData{
public:
    AnsiString value;

    EDataString();
    virtual ~EDataString();
    virtual int IsEqual(EData* b);
    virtual int IsNotEqual(EData* b);
    virtual EData* CreateCopy();
};

class ARegion;
void ExprPrepareRegion (ARegion *r);
bool ExprProcessExpr(AnsiString expr);

class EVariable;
typedef void (*initfunc)(EVariable &var);
class EVariable{
public:
    AnsiString name;
    EData *data;
    initfunc Init;
    EVariable(AnsiString n, initfunc in):
     name(n),data(0),Init(in){}
    void ClearData(){delete data;data=0;}
};
extern EVariable variables[];
extern int VarsCount;

typedef EData* (*funcfunc)(char*&);
class EFunction{
public:
    AnsiString name,params;
    funcfunc func;
    EFunction(AnsiString n,AnsiString p, funcfunc f):
     name(n),params(p),func(f){}
};
extern EFunction functions[];
extern int FuncsCount;

#endif
