//---------------------------------------------------------------------------
#ifndef utilH
#define utilH
//---------------------------------------------------------------------------
const NDIRS=6;
namespace EAP{class TFile;};
using namespace EAP;
extern AnsiString curGame;
extern TStringList*Games;
extern AnsiString OrdersDir,ReportsDir,StoreDir;
extern AnsiString IniName;
class AOrder;
class AOrders;

class ASkill;
class ASkills;

class AItem;
class AItems;

class AMarket;
class AMarkets;

class AUnit;

class ALocation;
class AObject;
class ARegion;

class AFaction;
class AFactions;

class ATurn;
class ATurns;

class VTreeNode;
class VTreeNodes;
class VListItem;
class VListItems;

// KeyPhrases->Get(keyAtlErrHead)
enum{
 keyAtlRepHead=0,
 keyWarSign,
 keyTradeSign,
 keyMagicSign,
 keyFacStat,
 keyTaxStatSign,
 keyTradeStatSign,
 keyMagesStatSign,
 keyAtlErrHead,
 keyAtlBatHead,
 keyAtlEvnHead,
 keySkillHead,
 keyItemHead,
 keyWeightSign,
 keyCapacSign,
 keyCanFly,
 keyCanRide,
 keyIsMan,
 keyIsMon,
 keyAttitudes,
 keyUnclaimed,
 keyInSign,
 keyContains,
 keyPeasants,
 keySilvSign,
 keyWages,
 keyMaxSign,
 keyNoneSign,
 keyUnlimitedSign,
 keyMarketAtSign,
 keyWantedSign,
 keySaledSign,
 keyEnteSign,
 keyProdSign,
 keyExitSign,
 keyGatesSign,
 keyGatesSign2,
 keyObjNameEndSign,
 keyObjNeedSign,
 keyObjContainsSign,
 keyObjEngravedSign,
 keyObjClosedSign,

 keyUnitOnGuard,
 keyUnitAvoid,
 keyUnitBehind,
 keyUnitRevUnit,
 keyUnitRevFaction,
 keyUnitHold,
 keyUnitTax,
 keyUnitNoAid,
 keyUnitConsumUnit,
 keyUnitConsumFaction,
 keyUnitSkills,
 keyUnitCombatSpell,
 keyUnitCanStudy,
 

 keyTemplateSign,
 keyTemplateBegin,
 keyItemIllusionSign,
 NKEYPHRASES
};

enum ReportOptions{
 roLocalUnit=0x0001,
 roLocalAsNonlocalUnit=0x0002,
 roNonlocalUnit=0x0004,
 roUserObject=0x0008,
 roNonuserObject=0x0010,
 roAllRegion=0x0020,
 roRegionInfo=0x0040,

 roNormalReport=roLocalUnit|roNonlocalUnit|roUserObject|roNonuserObject|roRegionInfo,
};

class AKeyPhrases:TStringList{
public:
    AKeyPhrases();
    virtual __fastcall ~AKeyPhrases();
    __fastcall AnsiString Get(int index);
    void Save();
};
extern AKeyPhrases *KeyPhrases;

void AddEvent(AnsiString ev);
void ClearEvents();
/*#define foreach(listptr,elem)  \
for(int abcdef=0;abcdef<listptr->count;abcdef++){\
 elem=listptr->Get(abcdef);*/
#define foreach(listptr) \
for(void **iter=listptr->List,**end=iter+listptr->count;iter<end;iter++)


/*#define foreachr(listptr,elem)  \
for(int abcdef=listptr->count-1;abcdef>=0;abcdef--){\
 elem=listptr->Get(abcdef);*/
#define foreachr(listptr) \
if(listptr->List) for(void **begin=listptr->List,**iter=begin+(listptr->count-1);iter>=begin;iter--)

TMemoryStream* LoadFile(AnsiString fName);
void ReadStringList(TFile &in,TStringList *list);
void WriteStringList(TFile &out,TStringList *list);
int CompareIStr(AnsiString s1, AnsiString s2);
bool MyCompare(const char *str,AnsiString pattern); //false - equal
extern int ReportTab;
void SaveRepString(TStream *strm, AnsiString str);
void SaveRepStringList(TStream *strm, TStringList *list);


AnsiString GetNextToken(char*&buf);
AnsiString GetNextString(char*&buf, int nextstrtab=0);
AnsiString GetNextFactionName(char*&buf);
AnsiString GetNextUnitName(char*&buf){return GetNextFactionName(buf);}
AnsiString GetNextItemName(char*&buf);
AnsiString GetNextSkillName(char*&buf){return GetNextItemName(buf);}
ALocation* GetNextLocation(char*&buf);
enum{
 mtRiding=1,
 mtFlying=2
};
class ATerrainType{
public:
    AnsiString name;
    int movepoints;
    int mountstypes;//1-riding,2-flying
    TColor color;
    TStringList *advlist;
    ATerrainType();
    ~ATerrainType();
    AnsiString Print(bool quotation=false);
    bool CanProduceAdv(AnsiString abr);
};
class ATerrainTypes:TList{
public:
    ATerrainTypes();
    virtual __fastcall ~ATerrainTypes();
    ATerrainType* Get(int index);
    void AddTerrainType(ATerrainType * ttype);
    bool modified;
    void Save();
    ATerrainType* Get(AnsiString terrain);
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern ATerrainTypes*TerrainTypes;
class AWeatherType{
public:
    AnsiString name;
    int movemult;
    AnsiString curmonthname;
    AnsiString nextmonthname;
    AWeatherType();
    AnsiString Print();
};
class AWeatherTypes:TList{
public:
    AWeatherTypes();
    virtual __fastcall ~AWeatherTypes();
    AWeatherType* Get(int index);
    void AddWeatherType(AWeatherType * wtype);
    bool modified;
    void Save();
    AWeatherType* Get(AnsiString name);
    AnsiString FindCurWeather(AnsiString text);
    AnsiString FindNextWeather(AnsiString text);
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern AWeatherTypes*WeatherTypes;
class ADirType{
public:
    AnsiString name;
    AnsiString abr;
    int num;
    AnsiString Print();
};
class ADirTypes:TList{
public:
    ADirTypes();
    virtual __fastcall ~ADirTypes();
    ADirType* Get(int index);
    int Find(AnsiString str);
    void Save();
    bool modified;
    void AddDirType(ADirType * dtype);
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern ADirTypes*DirTypes;
enum{
 MAGIC = 0x1,
 COMBAT = 0x2,
 CAST = 0x4,
 FOUNDATION = 0x8,
 MAGICCOMBAT = MAGIC |COMBAT,
};
class ASkillType{
public:
    AnsiString name;
    AnsiString abr;
    int cost;
    int flags;
    AnsiString depends[3];
    int levels[3];
    ASkillType();
    AnsiString Print(bool quotation = false);
    void CreateNew(ASkillType* prevskill);
    AnsiString FullName();
};
class ASkillTypes:TList{
public:
    ASkillTypes();
    virtual __fastcall ~ASkillTypes();
    ASkillType* Get(int index);
    ASkillType* Get(AnsiString name,int mode=3);
    void AddSkillType(ASkillType* stype);
    bool modified;
    void Save();
//0 - not found, 1 - present, 2 - name, not abr, 3 - abr, not name
    int VerSkills(AnsiString name, AnsiString abr);
    void MakeSkillList(TStringList * list);
    AnsiString AbrToName(AnsiString abr);
    AnsiString NameToAbr(AnsiString name);
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern ASkillTypes*SkillTypes;
class ASkillInfo{
public:
    AnsiString skill;
    int level;
    AnsiString text;
    AnsiString Print(bool quotation=false);
    ASkillInfo();
    AnsiString FullName();
    void CreateNew(ASkillInfo * prevsi);
};
class ASkillInfos:TList{
public:
    ASkillInfos();
    virtual __fastcall ~ASkillInfos();
    ASkillInfo* Get(int index);
    ASkillInfo* Get(AnsiString skill, int level);
    void AddSkillInfo(ASkillInfo * si);
    bool modified;
    void Save();
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern ASkillInfos *SkillInfos;

enum {
    IT_NORMAL = 0x01,
    IT_ADVANCED = 0x02,
    IT_TRADE = 0x04,
    IT_MAN = 0x08,
    IT_MONSTER = 0x10,
    IT_MAGIC = 0x20,
    IT_WEAPON = 0x40,
    IT_ARMOR = 0x80,
    IT_MOUNT = 0x100,
    IT_BATTLE = 0x200,
    IT_FOOD = 0x400,
};
enum{
    CANTGIVE=1,
};
class AItemType{
public:
    AnsiString name;
    AnsiString names;
    AnsiString abr;
    int flags; //cantgive
    AnsiString skill;  //requared to prod
    int level;
    AnsiString input;
    int number;
    int weight;
    int type;
    int walk;
    int ride;
    int fly;
    int swim;
    AnsiString helpproditem;
    AItemType();
    AnsiString Print(bool quotation = false);
    bool IsNewInfo(AItemType* it);
    void CreateNew(AItemType * previt);
    void UpdateNewInfo(AItemType * newit);
    AnsiString FullName(bool plural = false);
};
class AItemTypes:TList{
public:
    AItemTypes();
    virtual __fastcall ~AItemTypes();
    AItemType* Get(int index);
    AItemType* Get(AnsiString name,int mode=7);
    void AddItemType(AItemType* itype);
    bool modified;
    void Save();
    void MakeItemList(TStringList* list);
    AnsiString AbrToName(AnsiString abr);
    AnsiString AbrToNames(AnsiString abr);
    AnsiString NameToAbr(AnsiString name);
    static bool IsIllusionAbr(AnsiString abr);
    static AnsiString IllusionAbrToNormal(AnsiString iabr);
    static AnsiString NormalAbrToIllusion(AnsiString abr);
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern AItemTypes*ItemTypes;
class ATownType{
public:
    AnsiString name;
    AnsiString imgname;
    int imgnum;
    AnsiString Print();
    ATownType();
};
class ATownTypes:TList{
public:
    ATownTypes();
    virtual __fastcall ~ATownTypes();
    ATownType*Get(int index);
    ATownType* Get(AnsiString type);
    int GetIndex(AnsiString type);
    void AddTownType(ATownType *type);
    bool modified;
    void Save();
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};
extern ATownTypes*TownTypes;
extern AnsiString WoodOrStoneStr;
class AObjectType{
public:
    AnsiString name;
    int protect;
    int capacity;
    AnsiString item;
    int cost;
    AnsiString skill;
    int level;
    int sailors;
    int canenter;
    int road;
    AnsiString production;
    AnsiString Print();
    AObjectType();
};
class AObjectTypes:TList{
public:
    AObjectTypes();
    virtual __fastcall ~AObjectTypes();
    AObjectType* Get(int index);
    AObjectType* Get(AnsiString name);
    void AddObjectType(AObjectType *otype);
    bool modified;
    void Save();
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}

};
extern AObjectTypes *ObjectTypes;

class AWeatherRegion{
public:
    AnsiString name;
    int level;
    TRect rect;
    AnsiString weathers[12];
    AWeatherRegion();
    AnsiString Print();

};
class AWeatherRegions:TList{
public:
    AWeatherRegions();
    virtual __fastcall ~AWeatherRegions();
    AWeatherRegion *Get(int index);
    AWeatherRegion *Get(AnsiString name);
    void AddWeatherRegion(AWeatherRegion * reg);
    void Save();
    AnsiString GetWeather(int x, int y, int z, int turnnum=-1);
    __property int count  = { read = Getcount };
    __property List;

private:
    int __fastcall Getcount(){return Count;}

};
extern AWeatherRegions *WeatherRegions;
AnsiString MakeDepSkillStr(AnsiString skill,int level);
AnsiString MakeInputItemStr(AnsiString item,int number);

AnsiString GetGameFileName(AnsiString fName);
AnsiString GetOrderName(int facnum);
AnsiString GetReportName(int facnum);
void LoadGameBase();
void SaveGameBase();
void DeleteGameBase();

void SetMainFormCaption();
int GetTurnNumberFromString(AnsiString str);
AnsiString TurnNumberToString(int number);

extern int curTurnNumber;
extern ATurn*curTurn;
extern ATurns*curTurns;
extern ARegion*curRegion;
extern int BaseVer;
extern bool optShowEvents;
extern bool optRunRegionOrders;
extern int optTownNameLimit;
extern bool optMaximizeWindow;
enum EnDontShowEvents{
 dseDiscard=1<<0,
 dseNoMonth=1<<1,
 dseRegDataFound=1<<2,
};
extern int optDontShowEvents;
extern int optSaveMap;

void ReadStringAsInt(TFile &in,AnsiString &str);
void WriteStringAsInt(TFile &out,AnsiString str);
void ReadIntAsShort(TFile &in, int &i);
void WriteIntAsShort(TFile &out, int i);
void ReadIntAsByte(TFile &in, int &i);
void WriteIntAsByte(TFile &out, int i);

int DeleteFiles(AnsiString fname);
int MoveFiles(AnsiString oldname,AnsiString newname);
int CopyFiles(AnsiString oldname,AnsiString newname);
void SetBits(int &value,int mask,bool set);
#endif
