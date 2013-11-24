//---------------------------------------------------------------------------
#ifndef EditItemTypeFrmH
#define EditItemTypeFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TEditItemTypeForm : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *Table;
    TLabel *Label1;
    TEdit *edName;
    TLabel *Label2;
    TEdit *edAbr;
    TLabel *Label3;
    TEdit *edFly;
    TCheckBox *cbNormal;
    TLabel *Label5;
    TComboBox *cbSkill;
    TLabel *Label6;
    TButton *bnOk;
    TButton *bnCancel;
    TComboBox *cbLevel;
    TEdit *edNames;
    TLabel *Label4;
    TLabel *Label7;
    TComboBox *cbInput;
    TLabel *Label8;
    TEdit *edNumber;
    TCheckBox *cbBattle;
    TCheckBox *cbMount;
    TCheckBox *cbWeapon;
    TCheckBox *cbMagic;
    TCheckBox *cbMonster;
    TCheckBox *cbMan;
    TCheckBox *cbTrade;
    TCheckBox *cbAdvanced;
    TCheckBox *cbArmor;
    TLabel *Label9;
    TEdit *edRide;
    TLabel *Label10;
    TEdit *edWeight;
    TLabel *Label11;
    TEdit *edWalk;
    TCheckBox *cbCantGive;
    TCheckBox *cbFood;
    TComboBox *cbHelpProd;
    TLabel *Label12;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall cbSkillChange(TObject *Sender);
    void __fastcall bnOkClick(TObject *Sender);
    void __fastcall TableDblClick(TObject *Sender);
    void __fastcall cbInputChange(TObject *Sender);

private:	// User declarations
public:		// User declarations
        //old Item do not deleted
    __fastcall TEditItemTypeForm(TComponent* Owner,AItemType*oldItem,AItemType*newItem=0);
    virtual __fastcall ~TEditItemTypeForm();
protected:
    AItemType* FnewItem, *FoldItem;
    void PrepareName(AItemType *itype);
    void PrepareNames(AItemType *itype);
    void PrepareAbr(AItemType *itype);
    void PrepareCantGive(AItemType *itype);
    void PrepareSkill(AItemType *itype);
    void PrepareInput(AItemType *itype);
    void PrepareWeight(AItemType *itype);
    void PrepareNormal  (AItemType *itype);
    void PrepareAdvanced(AItemType *itype);
    void PrepareTrade   (AItemType *itype);
    void PrepareMan     (AItemType *itype);
    void PrepareMonster (AItemType *itype);
    void PrepareMagic   (AItemType *itype);
    void PrepareWeapon  (AItemType *itype);
    void PrepareArmor   (AItemType *itype);
    void PrepareMount   (AItemType *itype);
    void PrepareBattle  (AItemType *itype);
    void PrepareFood    (AItemType *itype);
    void PrepareWalk(AItemType *itype);
    void PrepareRide(AItemType *itype);
    void PrepareFly (AItemType *itype);
    void PrepareHelpProd(AItemType *itype);


};
//---------------------------------------------------------------------------
extern PACKAGE TEditItemTypeForm *EditItemTypeForm;
//---------------------------------------------------------------------------
#endif
