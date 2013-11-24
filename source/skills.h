//---------------------------------------------------------------------------
#ifndef skillsH
#define skillsH
//---------------------------------------------------------------------------
class ASkill{
public:
    AnsiString type;
    int days;
    ASkill();
    virtual __fastcall ~ASkill();
    void Read(TFile& in);
    void Write(TFile & out);
    void CreateNew(ASkill * newskill);
    AnsiString Print(int mode=15);
    ASkillType* GetType();
    int GetLevel();
    int GetSize();

};
class ASkills:TList{
public:
    ASkills();
    virtual __fastcall ~ASkills();
    void __fastcall Clear();
    void Read(TFile& in);
    void Write(TFile& out);
    ASkill * Get(int index);
    void Add(ASkill * mark);
    ASkill * GetSkill(AnsiString type);
    void CreateNew(ASkills * prvmarks);
    void Update(ASkills * newmarks);
    void UpdateFromPrev(ASkills * prvmarks);
    AnsiString Print(int mode=15);
    void DeleteEmpty();
    int GetSize();

    __property int count  = { read = Getcount };
    __property List;
private:
    int __fastcall Getcount(){return Count;}
};


#endif
