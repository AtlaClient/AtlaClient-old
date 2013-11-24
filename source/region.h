//---------------------------------------------------------------------------
#ifndef regionH
#define regionH
//---------------------------------------------------------------------------
#include <comctrls.hpp>
class AUnit;
class AUnits;
class ALocation{
public:
 ALocation():xloc(-1),yloc(-1),zloc(-1){};
 ALocation(int x,int y,int z):xloc(x),yloc(y),zloc(z){};
 int xloc,yloc,zloc;
 void Read(TFile & in);
 void Write(TFile & out);
/* int operator=(ALocation &loc){
  return xloc==loc.xloc&&yloc==loc.yloc&&zloc==loc.zloc;
 }*/
};

class AObject{
public:
    AnsiString name;
    AnsiString describe;
    bool inner;
//    ALocation inner;
    int num; //0 - DUMMY
    AnsiString type;
    int incomplete,endincomplete;
    int runes;
    AUnits *units;
    ARegion *basereg,*endreg;
    int cursailors;

    ATurn *turn;
    AObject(ATurn *t);
    virtual __fastcall ~AObject();
    void __fastcall Clear();

    bool WriteReport(TStream *strm,int obs,int truesight,bool detfac,int mapoptions);
    AnsiString Print();
    AnsiString FullName();
    AnsiString ObjectHeader(bool isreport);
    void CreateNew(AObject * obj);
    void Update(AObject * newobj);
    void UpdateFromPrev(AObject * prvobj);
    void Read(TFile & in);
    void Write(TFile & out);
    void AddUnit(AUnit* unit);
    void MakeTree(TTreeNode * parnode, VTreeNodes * vtns);
    void MakeView(VListItems * vlis);
    AUnit * GetUnit(int num);
    bool PresentLocals();
    int Protect();
    int Capacity();
    int Sailors();
    bool IsTower();
    bool IsBuilding();
    bool IsLair();
    bool IsShip();
    AUnit * GetOwner();
    int GetSize();
    bool Destroyable();

};
enum RunStop{
 rsNoStop=0, rsBeforeBuild,rsAfterGive
};
extern AObject *curObject;
class ARegionStatus{
public:
  bool HasTower:1;
  bool HasBuilding:1;
  bool HasShaft:1;
  bool HasLair:1;
  bool HasShip:1;

  ARegionStatus();
};
enum SaveStatus{
 ssNameOnly=0x0,
 ssRegionInfo=0x1,
 ssObjectInfo=0x2,
 ssUnitInfo=0x4,  //not used
 ssGateInfo=0x8,
 ssWeatherInfo=0x10,
 ssWagesInfo=0x20,
 ssEntertainInfo=0x40,
 ssExitsInfo=0x80,
 ssWantedInfo=0x100,
 ssSaledInfo=0x200,
 ssProductInfo=0x400,
};
class ARegion:public TList{
public:
    int num;
    AnsiString type;
    int xloc,yloc,zloc;
    bool hasInfo;
    int oldinfoturnnum;
//    int infoStatus;
    AnsiString name;
    AnsiString towntype;
    AnsiString townname;
    AnsiString race;        //isRegionInfo
    int population;         //isRegionInfo
    int money;              //isRegionInfo
    int taxincome;        //0 - set to auto
    int curtaxprod;
    int taxers;
    int workincome;        //0 - set to auto
    int workers;
    int entincome;         //0 - set to auto
    int entattempt; 
    AnsiString weather,nxtweather; //isWeatherInfo

    int wages;              //isWagesInfo
    int maxwages;           //isWagesInfo
    int entertainment;      //isEntertainInfo
    ALocation neighbors[NDIRS];   //isExitsInfo
    ALocation inner; //xloc=-1 - none
    int gate;               //isGateInfo 0 -not set
    AMarkets *wanteds;      //isWantedInfo
    AMarkets *saleds;       //isSaledInfo
    AMarkets *products;       //isProductInfo

/*    int oldreginfo,oldobjinfo,oldunitinfo,oldgateinfo;
    int oldweatherinfo,oldwagesinfo,oldentertaininfo;
    int oldexitsinfo,oldwantedinfo,oldsaledinfo,oldproductinfo;*/

    int lastviewfact;

/*    struct{
     bool hasLocal:1;
     bool hasLocalTaxers:1;
    };*/
    ATurn *turn;
    ARegion(ATurn *t);
    virtual _fastcall ~ARegion();
    void __fastcall Clear();
    void Add(AObject * obj);
    AObject * Get(int num);
    
    AObject * GetObject(int objnum);
    void Read(TFile &in);
    void Write(TFile &out);
    void CreateNew(ARegion * prevreg);
    AnsiString FullName();
    bool WriteReport(TStream *strm,int mapoptions);
    void Update(ARegion * newreg);
    void Update0(ARegion * newreg);
    int GetDistance(int x, int y, int z);
    void UpdateFromPrev(ARegion * prvreg);
    AnsiString Print();
    void CreateDummy();
    AObject * GetDummy();
    void MakeTree(TTreeNode * parnode, VTreeNodes * vtns);
    void MakeView(VListItems * vlis);
    AUnit * GetUnit(int num);
    AObject * GetInner();
    bool PresentLocals();
    bool CanTax(AUnit * u);
    void RunOrders(RunStop rs=rsNoStop);
    void RunTaxOrders();
    bool HasLocalTaxers();
    void RunConsumeOrders();
    void RunStudyOrders();
    bool RunGiveOrders();
    void PrepareOrders();
    void PrepareOrders2();
    bool RunSellOrders();
    bool RunBuyOrders();
    bool RunBuySellOrders(int type,AMarket * mark);
    void RunEntertainOrders();
    void RunWorkOrders();
    void PrepareConsume();
    void RunFlagOrders();
    void CheckUnitMaintenanceItem(AUnits *units,AnsiString item, int value, bool consume);
    int CheckFactionMaintenanceItem(AUnits *units,AnsiString item, int value, bool consume);
    bool RunForgetOrders();
    bool RunTeachOrders();
    bool RunPillageOrders();
    void RunProduceOrdersA();
    void RunProduceOrdersA(AMarket *mark);
    void RunProduceOrders();
    bool CanMakeAdv(AFaction * fac, AnsiString item);
    void RunEnterLeaveOrders();
    bool IsCoastal();
    int MoveCost(int movetype);
    AnsiString NxtWeather();
    void RunGuard1Orders();
    void RunMoveOrders();
    void RunAutoOrders1();
    void Compact();
    void RunBuildOrders();
    void RunSailOrders();
    void ClearRegionStatus();
    void UpdateRegionStatus();
    bool PresentFaction(AFaction *fac);
    int GetObservation(AFaction * fac);
    int GetTrueSight(AFaction * fac);
    int GetSize();
    bool HasExitRoad(int dir);
    bool HasConnectingRoad(int dir);

    __property int count  = { read = Getcount };
    __property List;
    __property ARegionStatus* RegionStatus  = { read = GetRegionStatus };
private:
    ARegionStatus* FRegionStatus;
    int __fastcall Getcount(){return Count;}
    ARegionStatus* __fastcall GetRegionStatus();
};
extern ARegion*curRegion;
class ARegionArray{//:TList{
public:
    ARegionArray();
    int x,y;
    AnsiString name;
    int levelnum;
    ARegion* Get(int num);
    ARegion* Get(int x, int y);
    void Read(TFile &in);
    void Write(TFile &out);
    void CreateNew(ARegionArray* prevra);
    ~ARegionArray();
    void NormCoord(int & xloc, int & yloc);
    void UpdateFromPrev(ARegionArray * prvar);
    void VerifySize(int xloc, int yloc);
    void NormCoord2(int & xloc, int & yloc);
    int GetSize();
//    void Add(ARegion* reg);
//    DYNAMIC void __fastcall Clear();
};
extern ARegionArray*curRegionArray;
class ARegionArrays:TList{
public:
    ARegionArrays();
    virtual __fastcall ~ARegionArrays();
    void Read(TFile &in);
    void Write(TFile& out);
    void Add(ARegionArray* ar);
    ARegionArray* Get(int levelnum);
    void __fastcall Clear();
    void CreateNew(ARegionArrays * prevras);
    ARegionArray* Get(AnsiString levelname);
    int GetLevelNum(AnsiString levelname);
    void UpdateFromPrev(ARegionArrays * prvars);
    int GetSize();

    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
class ARegionList:TList{
public:
    int numberofgates;
    ARegionArrays*RegionArrays;

    ATurn *turn;
    ARegionList(ATurn *t);
    virtual __fastcall ~ARegionList();
    void __fastcall Clear();
    void Read(TFile& in);
    void Write(TFile& out);
    void Add(ARegion * reg);
    ARegion* Get(int regnum);
    ARegion* Get(int x, int y, int z);
    ARegion * Get(ALocation & loc);
    ARegionArray* GetRegionArray(int level);
    bool SetCurRegionArray(int levelnum);
    void CreateNew(ARegionList* prevrl);
    bool SetCurRegion(int x, int y, int z);
    ARegion * GetNearestRegion(int x, int y, int z);
    void UpdateFromPrev(ARegionList * prvrl);
    AUnit * GetUnit(int num);
    void Sort();
    int GetSize();
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern ARegionList*curRegionList;
#endif
