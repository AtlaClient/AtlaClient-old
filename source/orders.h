//---------------------------------------------------------------------------
#ifndef ordersH
#define ordersH
//---------------------------------------------------------------------------
enum OrderTypes{
  O_NONE,
  O_END,
  O_UNIT,
  O_CUSTOMORDER,
  O_LOCALDESCR,
  O_COMMENT,
  O_ADDRESS,
  O_ADVANCE,
  O_ASSASSINATE,
  O_ATTACK,
  O_AUTOTAX,
  O_AVOID,
  O_BEHIND,
  O_BUILD,
  O_BUY,
  O_CAST,
  O_CLAIM,
  O_COMBAT,
  O_CONSUME,
  O_DECLARE,
  O_DESCRIBE,
  O_DESTROY,
  O_ENDFORM,
  O_ENTER,
  O_ENTERTAIN,
  O_FACTION,
  O_FIND,
  O_FORGET,
  O_FORM,
  O_FORMEND,
  O_GIVE,
  O_MESSAGE,
  O_GUARD,
  O_HOLD,
  O_LEAVE,
  O_MOVE,
  O_NAME,
  O_NOAID,
  O_OPTION,
  O_PASSWORD,
  O_PILLAGE,
  O_PRODUCE,
  O_PROMOTE,
  O_QUIT,
  O_RESTART,
  O_REVEAL,
  O_SAIL,
  O_SELL,
  O_SHOW,
  O_STEAL,
  O_STUDY,
  O_TAX,
  O_TEACH,
  O_TEACHED,
  O_WORK,
  O_AUTOGIVE,
  O_NOP,
  NORDERS
};
class AOrderMessage;
class AOrder{
public:
    OrderTypes type;
    AOrders *orders;

    bool isedited; //not saved
//    int index;
    union{
     struct{
      bool commented:1;
      bool repeating:1;
      unsigned reserved:6;
     };
     unsigned char flags;
    };
    AOrder();
    virtual ~AOrder();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    AnsiString Begin();
    virtual bool IsMonthLong();
    virtual int Productivity();
    virtual void DeleteMessage(AOrderMessage *mes);
    virtual void Prepare();
    virtual int GetSize();
};
AOrder *ParseOrder(AnsiString str);

class AOrders:TList{
public:
    bool autodelete;

    AUnit *unit;
    AOrders(AUnit *un);
    __fastcall ~AOrders();
    void __fastcall Clear();
    void Read(TFile & in);
    void Write(TFile & out);
    AOrder* Get(int index);
    void Add(AOrder* ord);
    void Update(AOrders * orders);
    bool Replace(AOrder * from, AOrder * to);
//mode = 1 -comm=true, 2 - comm =false , 4 rep=true, 8 rep= false
    AOrder * Find(OrderTypes type, int mode = 15);
    void Delete(int index);
    void Delete(AOrder * ord);
    bool Distribute(int amount, int attempted);
    void Compact();
    int GetSize();
    bool Swap(int i, int j); //exit when producivity<=0
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}

};
class AOrderCustom:public AOrder{
public:
    AnsiString text;

    AOrderCustom();
    virtual ~AOrderCustom();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual AOrder * Edit();
    virtual bool IsMonthLong();
    virtual int GetSize();
};
class AOrderComment:public AOrder{
public:
    AnsiString text;

    AOrderComment();
    virtual ~AOrderComment();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual AOrder * Edit();
    virtual int GetSize();
};
class AOrderLocDescr:public AOrder{
public:
    AnsiString text;

    AOrderLocDescr();
    virtual ~AOrderLocDescr();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual AOrder * Edit();
    virtual int GetSize();
};
//destroy-, entertain, leave-, pillage-, Tax, Work
//autoNOP
class AOrderSimple:public AOrder{
private:
    int number;
public:
    AOrderSimple();
    virtual ~AOrderSimple();
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual bool IsMonthLong();
    virtual int Productivity();
    virtual void Prepare();
    virtual int GetSize();
};
//autotax, avoid, behind, guard, hold, noaid
//consume, option-, reveal
class AOrderByte:public AOrder{
public:
    unsigned char value;
    AOrderByte();
    virtual ~AOrderByte();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual int GetSize();
};
//combat-, forget-, produce-, study
class AOrderAbr:public AOrder{
public:
    int number;
    AnsiString abr;

    AOrderAbr();
    virtual ~AOrderAbr();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual bool IsMonthLong();
    virtual int Productivity();
    virtual void Prepare();
    virtual int GetSize();
};
class AOrderGive:public AOrder{
public:
    int tarnum;
    AUnit *target;
    int count,added;
    AnsiString item;
    AOrderMessage *message;

    AOrderGive();
    virtual ~AOrderGive();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual void DeleteMessage(AOrderMessage *mes);
    void SetMessage();
    virtual void Prepare();
    virtual int GetSize();
};
class AOrderMessage:public AOrder{
public:
    AOrder *base;
    AUnit *target;
    AnsiString text;

    AOrderMessage(AOrder *b);
    virtual ~AOrderMessage();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual int GetSize();
};
class AOrderBuySell:public AOrder{
public:
    int number;


    AnsiString item;
    int amount;

    AOrderBuySell();
    virtual ~AOrderBuySell();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual int Productivity();
    virtual int GetSize();
};
struct AUnitPtr{
 int num;
 AUnit *ptr;
 AOrderMessage *mes;
};
class AOrderTeach:public AOrder{
public:
    TList *targets;  //*AUnitPtr

    AOrders *bases;
    AOrderTeach();
    virtual ~AOrderTeach();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual void DeleteMessage(AOrderMessage *mes);
    void Clear();
    AUnitPtr * GetTarget(int index);
    void AddTarget(int num, AUnit *target);
    void VerTargets();
    virtual bool IsMonthLong();
    virtual void Prepare();
    virtual int GetSize();
};
//claim, enter-, assasinate-, promote-
class AOrderInt:public AOrder{
public:
    int value;

    AOrderInt();
    virtual ~AOrderInt();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual int Productivity();
    virtual int GetSize();
};
enum {
  M_NONE,
  M_WALK,
  M_RIDE,
  M_FLY,
  M_SAIL
};
const MOVE_IN=10;
const MOVE_OUT=11;
/* Enter is MOVE_ENTER + num of object */
const MOVE_ENTER=12;

class AOrderMoving:public AOrder{
friend class TEditOrderMovingForm;
protected:
    TList *dirs;
    AnsiString custom;
public:

    AOrderMoving();
    virtual ~AOrderMoving();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    int Get(int ind);
    void Add(int dir);
    void Delete(int ind);
    virtual bool IsMonthLong();
    virtual int GetSize();

    __property int count  = { read = Getcount };
    __property int* List={read =GetList};
private:
    int __fastcall Getcount(){return dirs->Count;}
    int* __fastcall GetList(){return (int*)dirs->List;}
};
class AOrderAutoGive:public AOrder{
public:
    int number;
    AnsiString item;
    AnsiString begstr;
    AnsiString substr;

    AOrderAutoGive();
    virtual ~AOrderAutoGive();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
//    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual int GetSize();
};
class AOrderBuild:public AOrder{
public:
    AnsiString object;
    int work;

    AOrderBuild();
    virtual ~AOrderBuild();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual bool IsMonthLong();
    virtual int GetSize();
};
/*
class AOrderCustom:public AOrder{
public:

    AOrderCustom();
    virtual ~AOrderCustom();
    virtual void Read(TFile & in);
    virtual void Write(TFile & out);
    virtual AnsiString Print();
    virtual AnsiString WriteTemplate();
    virtual bool ParseOrder(char * s);
    virtual void Run(int value);
    virtual AOrder * Edit();
    virtual int Productivity();
};
*/
#endif
