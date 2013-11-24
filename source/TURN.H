//---------------------------------------------------------------------------
#ifndef turnH
#define turnH
//---------------------------------------------------------------------------
const MaxLastLevels=10;
class ARegionList;
class AFactions;
class ATurn{
private:
    void __fastcall SetPacked(bool value);
    union{
     struct{
      bool FPacked:1;
      bool FPackage:1;
      unsigned reserved:6;
     };
     unsigned char attr;
    };
public:
    int num;
    ARegionList*RegionList;
    AFactions*factions;
    ATurn();
    ~ATurn();
    void Read();
    void Write();
    void CreateNew(ATurn * prevturn);
    void Update0(ATurn *newturn);
    void VerifyNextTurn();
    void PrepareOrders();
    void RunOrders();
    void RunAutoOrders();
    void Compact();
    int GetSize();
    AnsiString GetFileName(int mode);
    bool SaveMap(TFileStream * file,int mapoptions);
    __property bool Packed  = { read = FPacked, write = SetPacked };
    __property bool Package  = { read = FPackage, write = FPackage };
};
class ATurns:TList{
public:
    TStringList *tertypes,*provincies,*towns,*towntypes;
    int lastfac;
    int /*lastx,lasty,*/lastz;
    int lastx[MaxLastLevels],lasty[MaxLastLevels];
    AnsiString ver1,ver2;
    int curbeginturn,newbeginturn;
    int autobeginturn;

    ATurns();
    virtual __fastcall ~ATurns();
    void __fastcall Clear();
    void Read();
    void Write();
    void RepackStore();
    void Add(ATurn * turn);
    ATurn* Get(int turnnum);
    bool SetCurTurn(int turnnum);
    void CreateNew();
    bool RestoreLastParam();
    void StoreLastParam();
    void RestoreLastXY(int &xloc,int &yloc,int zloc);
    void StoreLastXY();
    void PrepareTurn(int num);
    BYTE Tertypes(AnsiString);
    WORD Provincies(AnsiString);
    WORD Towns(AnsiString);
    BYTE Towntypes(AnsiString);
    int GetSize();
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}

};

#endif
