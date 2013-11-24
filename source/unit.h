//---------------------------------------------------------------------------
#ifndef unitH
#define unitH
//---------------------------------------------------------------------------
#include <comctrls.hpp>
enum {
 INFO_NONE=0,
 INFO_UNIT,
 INFO_FACTION,
 INFO_LOCAL
};
enum {
    GUARD_NONE=0,
    GUARD_GUARD,
    GUARD_AVOID,
    GUARD_SET,
    GUARD_ADVANCE
};
enum {
  REVEAL_NONE=0,
  REVEAL_UNIT,
  REVEAL_FACTION
};
enum{
 FLAG_BEHIND=1,
 FLAG_AUTOTAX=4,
 FLAG_HOLDING=8,
 FLAG_NOAID=16,
 FLAG_INVIS=32,
 FLAG_CONSUMING_UNIT=64,
 FLAG_CONSUMING_FACTION=128,
 FLAG_CONSUMING=FLAG_CONSUMING_UNIT|FLAG_CONSUMING_FACTION,

};
class AUnit{
public:
    AnsiString name;
    AnsiString describe;
    int num; //minus for new
    union{
     struct{
      unsigned flags :16;
      unsigned info :3;
      unsigned guard :3;
      unsigned reveal:3;
      unsigned hasskills:1;
      unsigned hascombatspell:1;
      unsigned hascanstudy:1;
      unsigned reserved:4;
     };
     int globalflags;
    };
    int endflags;
    int endguard;
    int endreveal;
    AFaction *endfaction;
    int needed; //used for maint

    AOrders *orders;
    AItems *items, *enditems;
    ASkills *skills, *endskills;
    AnsiString combatspell;
    TStringList *canstudy;

    int weight, walkweight,ridweight, flyweight;
    AObject *baseobj, *endobj;
    AFaction *faction;
    ATurn *turn;

    int movepoints;

    AUnit(ATurn *t);
    virtual __fastcall ~AUnit();
    void Read(TFile & in);
    void Write(TFile& out);
    void CreateNew(AUnit * prvunit);
    void Update(AUnit* newun);
    void UpdateFromPrev(AUnit * prvun);
    AnsiString Print();
    bool WriteReport(TStream *strm,int obs,int truesight,bool detfac,bool autosee);
    AnsiString FullName(bool fortemplate=false);
    void MakeTree(TTreeNode * parnode, VTreeNodes * vtns);
    AnsiString PrintForTree();
    void MakeView(VListItems * vlis);
    AnsiString UnitHeader(bool fortemplate=false);
    bool GetFlag(int flag);
    void SetFlag(int flag, bool value);
    bool GetEndFlag(int flag);
    void SetEndFlag(int flag, bool value);
    AnsiString GetLocalDescr();
    AOrder * GetOrder(int num);
    bool IsLocal();
    bool SaveTemplate(TFileStream * file);
    void FormNew(AUnit *base);
    void RunOrders();
    int GetMen(AnsiString type);
    int GetMen();
    int GetBegMen();
    int GetAttitude(ARegion * reg, AUnit * u);
    bool IsAlive();
    int CanSee(ARegion * r, AUnit * u);
    void SetFaction(int facnum);
    bool HasTaxOrder();
    int Taxers();
    void SetMoney(int count);
    int GetMoney();
    AnsiString Alias();
    int GetBegRealSkill(AnsiString type);
    int GetBegSkill(AnsiString type);
    int GetRealSkill(AnsiString type);
    int GetSkill(AnsiString type);
    int GetBegSkillDays(AnsiString type);
    int GetSkillDays(AnsiString type);
    void SetSkillDays(AnsiString type, int val);
    bool IsLeader();
    bool IsNormal();
    void UpdateWeight();
    bool CanStudy(AnsiString skill);
    bool CheckDepend(int lev, AnsiString dep, int deplev);
    int MoveType();
    int CalcMovePoints();
    AnsiString FullName2();
    int GetBegObservationBonus();
    int GetObservationBonus();
    int GetBegObservation();
    int GetBegStealthBonus();
    int GetStealthBonus();
    int GetBegStealth();
    bool IsMage();
    int GetSize();

};
extern AUnit *curUnit;
class AUnits:TList{
public:
    bool autodelete;

    AObject *object;
    AUnits(AObject *obj);
    virtual __fastcall ~AUnits();
    void __fastcall Clear();
    void Read(TFile& in);
    void Write(TFile & out);
    AUnit* Get(int index);
    void Add(AUnit* un);
    void CreateNew(AUnits* prvunits);
    void Update(AUnits* newunits);
    AUnit* GetUnit(int num);
    bool Delete(int num);
    int GetSize();

    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}

};
#endif
