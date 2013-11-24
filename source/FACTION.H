//---------------------------------------------------------------------------
#ifndef factionH
#define factionH
//---------------------------------------------------------------------------
#include <comctrls.hpp>

extern char *AttitudeStrs[];
int AttitudeStrToInt(AnsiString str);

enum {
    A_HOSTILE,
    A_UNFRIENDLY,
    A_NEUTRAL,
    A_FRIENDLY,
    A_ALLY,
    NATTITUDES
};

class Attitude{
public:
    int facnum;
    int attitude;
    void CreateNew(Attitude * prevat);
};
class AFaction:TList{
public:
    int number;
    AnsiString name;
    bool local;
    int unclaimed;
    AnsiString address;
    AnsiString password;
    int defattitude;
    int warpts,warmax,warused;
    int tradepts,trademax,tradeused;
    int magepts,magemax,mageused;
    TStringList *errors;
    TStringList *battles;
    TStringList *events;
    AOrders *orders;

    AFactions *factions;
    AFaction(AFactions *facs);
    virtual __fastcall ~AFaction();
    void __fastcall Clear();//-1 - set to default

    void Read(TFile & in);
    void Write(TFile& out);
    int GetAttitude(int facnum);
    void SetAttitude(int facnum, int att);
    void CreateNew(AFaction * prevfac);
    AnsiString FullName();
    AnsiString ReportHeader();
    void MakeTree(TTreeNode * parnode, VTreeNodes * vtns);
    void MakeView(VListItems * vlis);
    void MakeViewErrors(VListItems * vlis);
    void MakeViewBattles(VListItems * vlis);
    void MakeViewEvents(VListItems * vlis);
    bool SaveTemplate(TFileStream * file);
    bool SaveReport(TFileStream * file);
    int CanSee(ARegion * r, AUnit * u);
    int CountTaxRegions();
    int GetSize();
    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
protected:
    Attitude* Get(int facnum);
};
extern AFaction*curFaction;
class AFactions:TList{
public:
    ATurn *turn;
    AFactions(ATurn *t);
    virtual __fastcall ~AFactions();
    void __fastcall Clear();
    void Read(TFile& in);
    void Write(TFile& out);
    AFaction * Get(int index);
    AFaction* GetFaction(int facnum);
    AFaction* GetFaction(AnsiString name);
    void Add(AFaction* fac);
    bool SetCurFaction(int facnum);
    void CreateNew(AFactions * prevfacs);
//0 - not found, 1 - present, 2 - name, not num, 3 - num, not name
    int VerFaction(AnsiString name, int num);
    void Delete(int facnum);
    void MakeTree(TTreeNode * parnode, VTreeNodes * vtns);
    void Compact();
    int GetSize();

    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}

};
extern AFactions*curFactions;
#endif
