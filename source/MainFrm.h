//---------------------------------------------------------------------------
#ifndef MainFrmH
#define MainFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include <inifiles.hpp>
#include <ActnList.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *miGame;
    TMenuItem *Exit1;
    TMenuItem *Help1;
    TMenuItem *Terraintypes1;
    TMenuItem *Weathertypes1;
    TMenuItem *N1;
    TMenuItem *miGameNew;
    TMenuItem *miGameChange;
    TMenuItem *N2;
    TMenuItem *miLoadReport;
    TMenuItem *miShowSkilltypes;
    TMenuItem *miShowItemTypes;
    TMenuItem *miShowSkillReports;
    TPanel *PanelMain;
    TPanel *PanelMap;
    TSplitter *Splitter1;
    TPanel *PanelViewer;
    TImage *ImageMap;
    TMenuItem *miView;
    TImageList *MapImages;
    TMenuItem *miLevelUp;
    TMenuItem *miLevelDown;
    TToolBar *ToolBar;
    TImageList *InfoImages;
    TOpenDialog *OpenDialog;
    TMenuItem *N3;
    TMenuItem *miPrevTurn;
    TMenuItem *miNextTurn;
    TTreeView *TreeView;
    TSplitter *Splitter2;
    TListBox *View;
    TToolButton *bnShowEvents;
    TPopupMenu *popupViewRegObject;
    TMenuItem *miLinkInner;
    TMenuItem *miGotoLinked;
    TMenuItem *N4;
  TMenuItem *miSaveOrder;
    TMenuItem *miLoadOrder;
    TPopupMenu *popupViewUnit;
    TMenuItem *miEditOrder;
    TMenuItem *miAddOrder;
    TMenuItem *miAddOrderCustom;
    TMenuItem *miAddOrderLocDescr;
    TMenuItem *N5;
    TMenuItem *miAddOrderComment;
    TMenuItem *miDeleteOrder;
    TPopupMenu *popupFactionMessages;
    TMenuItem *miFacClearErrors;
    TMenuItem *miFacClearError;
    TMenuItem *miFacClearBattles;
    TMenuItem *miFacClearBattle;
    TMenuItem *miFacClearEvents;
    TMenuItem *miFacClearEvent;
  TToolButton *sep2;
    TLabel *lbCoord;
    TEdit *edFindUnit;
  TMenuItem *miAddOrderFormNew;
    TMenuItem *N7;
    TMenuItem *miDeleteNewUnit;
    TPopupMenu *popupViewRegion;
    TMenuItem *miSetTaxInc;
    TImageList *ToolBarImages;
  TToolButton *sep1;
    TLabel *Label1;
    TMenuItem *miAddOrderDestroy;
    TMenuItem *miAddOrderEntertain;
    TMenuItem *miAddOrderLeave;
    TMenuItem *miAddOrderPillage;
    TMenuItem *miAddOrderTax;
    TMenuItem *miAddOrderWork;
    TMenuItem *miAddOrderAutotax;
    TMenuItem *miAddOrderAvoid;
    TMenuItem *miAddOrderBehind;
    TMenuItem *miAddOrderGuard;
    TMenuItem *miAddOrderHold;
    TMenuItem *miAddOrderNoaid;
  TToolButton *sep6;
    TMenuItem *miAddOrderStudy;
    TMenuItem *miAddOrderCombat;
    TMenuItem *miAddOrderForget;
    TMenuItem *miAddOrderProduce;
    TMenuItem *miAddOrderGive;
    TMenuItem *miAddOrderBuy;
    TMenuItem *miAddOrderSell;
    TToolButton *bnRunOrders;
    TMenuItem *miGotoOrderTarget;
    TLabel *Label2;
    TEdit *edTurnNumber;
    TMenuItem *miAddOrderTeach;
    TMenuItem *miAnalyze;
    TMenuItem *miAdvProds;
  TToolButton *sep4;
    TMenuItem *N6;
    TMenuItem *miProdModeSetup;
    TMenuItem *miGameSave;
    TMenuItem *miVerifynextturn;
    TMenuItem *miAddOrderClaim;
    TMenuItem *miAddOrderReveal;
    TMenuItem *miAddOrderConsume;
    TMenuItem *miAddOrderEnter;
    TMenuItem *N8;
    TMenuItem *miCopyOrder;
    TMenuItem *miPasteOrder;
    TMenuItem *miSetWorkInc;
    TMenuItem *miSetEntInc;
    TMenuItem *miAddOrderMove;
    TMenuItem *miAddOrderAdvance;
    TMenuItem *miAddOrderSail;
    TToolButton *bnLevelUp;
    TToolButton *bnLevelDown;
  TToolButton *sep5;
    TToolButton *bnRunAutoOrders;
    TMenuItem *miAddOrderAuto;
    TMenuItem *miAddOrderAutoGive;
    TMenuItem *miFix;
    TMenuItem *miMapSizeFix;
    TMenuItem *miAdvancedProductsFix;
    TToolButton *bnTownMode;
    TToolButton *bnProdMode;
    TToolButton *bnWedgeMode;
    TMenuItem *miWedgemodesettings;
 TMenuItem *miPackUnpack;
 TMenuItem *Exitwithoutsaving1;
 TMenuItem *N9;
 TMenuItem *miPackTurns;
 TMenuItem *VerTime1;
    TMenuItem *miTemporary;
    TActionList *ActionList1;
    TAction *actEnableTemp;
    TMenuItem *miVerSize;
 TMenuItem *miAddOrderBuild;
 TAction *actLevelUp;
 TAction *actLevelDown;
 TAction *actPrevTurn;
 TAction *actNextTurn;
 TToolButton *bnPrevTurn;
 TToolButton *bnNextTurn;
  TToolButton *sep7;
 TMenuItem *miGotoLastInfo;
        TMenuItem *UpdTurn0;
        TMenuItem *miCompactTurn;
    TMenuItem *miStoreTurns;
        TMenuItem *miWeather;
        TMenuItem *t21;
        TMenuItem *t31;
        TMenuItem *t41;
        TMenuItem *r51;
        TMenuItem *t61;
        TMenuItem *t71;
        TMenuItem *t81;
        TMenuItem *t91;
        TMenuItem *t101;
        TMenuItem *t111;
        TMenuItem *t11;
        TMenuItem *t121;
        TMenuItem *N10;
        TMenuItem *miShowRegionInfo;
        TAction *actShowRegInfo;
        TMenuItem *miShowMap;
        TAction *actShowMap;
  TMenuItem *miSaveReport;
  TMenuItem *miRepackStore;
  TMenuItem *miImport;
  TMenuItem *miAtlaCart;
  TMenuItem *miOptions;
  TMenuItem *N11;
  TMenuItem *miAddClienOrder;
  TMenuItem *miReceive;
  TMenuItem *miAbout;
  TToolButton *bnPrevUnit;
  TToolButton *bnNextUnit;
  TToolButton *sep3;
  TAction *actPrevUnit;
  TAction *actNextUnit;
  TMenuItem *N12;
  TMenuItem *miPrevUnit;
  TMenuItem *miNextUnit;
 TMenuItem *N13;
 TMenuItem *N14;
 TMenuItem *miSavereginfo;
 TMenuItem *miCopyToClipboard2;
 TMenuItem *miCopyToClipboard1;
 TMenuItem *miCopyToClipboard3;
 TMenuItem *miCopyToClipboard4;
 TPopupMenu *popupViewDefault;
 TMenuItem *miCopyToClipboard0;
 TAction *actCopyToClipboard;
 TMenuItem *miSaveMap;
    TMenuItem *miAddOrderNOP;
    TMenuItem *miMoveOrderUp;
    TMenuItem *miMoveOrderDown;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall Terraintypes1Click(TObject *Sender);
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall Weathertypes1Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall miGameNewClick(TObject *Sender);
    void __fastcall miGameChangeClick(TObject *Sender);
    void __fastcall miLoadReportClick(TObject *Sender);
    void __fastcall miShowSkilltypesClick(TObject *Sender);
    void __fastcall miShowSkillReportsClick(TObject *Sender);
    void __fastcall miShowItemTypesClick(TObject *Sender);
    void __fastcall PanelMapResize(TObject *Sender);
    void __fastcall actLevelUpExecute(TObject *Sender);
    void __fastcall actLevelDownExecute(TObject *Sender);
    void __fastcall actNextTurnExecute(TObject *Sender);
    void __fastcall ImageMapMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall bnShowEventsClick(TObject *Sender);
    void __fastcall actPrevTurnExecute(TObject *Sender);
    void __fastcall TreeViewMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall TreeViewChange(TObject *Sender, TTreeNode *Node);
    void __fastcall ViewDblClick(TObject *Sender);
    void __fastcall ViewClick(TObject *Sender);
    void __fastcall ViewMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall popupViewRegObjectPopup(TObject *Sender);
    void __fastcall miLinkInnerClick(TObject *Sender);
    void __fastcall miGotoLinkedClick(TObject *Sender);
    void __fastcall miSaveOrderClick(TObject *Sender);
    void __fastcall miLoadOrderClick(TObject *Sender);
    void __fastcall popupViewUnitPopup(TObject *Sender);
    void __fastcall miEditOrderClick(TObject *Sender);
    void __fastcall miAddOrderCustomClick(TObject *Sender);
    void __fastcall miAddOrderCommentClick(TObject *Sender);
    void __fastcall miAddOrderLocDescrClick(TObject *Sender);
    void __fastcall miDeleteOrderClick(TObject *Sender);
    void __fastcall TreeViewEnter(TObject *Sender);
    void __fastcall miFacClearErrorsClick(TObject *Sender);
    void __fastcall miFacClearErrorClick(TObject *Sender);
    void __fastcall miFacClearBattlesClick(TObject *Sender);
    void __fastcall miFacClearEventsClick(TObject *Sender);
    void __fastcall miFacClearBattleClick(TObject *Sender);
    void __fastcall miFacClearEventClick(TObject *Sender);
    void __fastcall ImageMapMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall edFindUnitKeyPress(TObject *Sender, char &Key);
    void __fastcall miAddOrderFormNewClick(TObject *Sender);
    void __fastcall miDeleteNewUnitClick(TObject *Sender);
    void __fastcall miSetTaxIncClick(TObject *Sender);
    void __fastcall popupViewRegionPopup(TObject *Sender);
    void __fastcall miAddOrderSimpleClick(TObject *Sender);
    void __fastcall miAddOrderByteClick(TObject *Sender);
    void __fastcall miAddOrderAbrClick(TObject *Sender);
    void __fastcall miAddOrderGiveClick(TObject *Sender);
    void __fastcall miAddOrderBuySellClick(TObject *Sender);
    void __fastcall bnRunOrdersClick(TObject *Sender);
    void __fastcall miGotoOrderTargetClick(TObject *Sender);
    void __fastcall edTurnNumberKeyPress(TObject *Sender, char &Key);
    void __fastcall miAddOrderTeachClick(TObject *Sender);
    void __fastcall bnTownModeClick(TObject *Sender);
    void __fastcall bnProdModeClick(TObject *Sender);
    void __fastcall miProdModeSetupClick(TObject *Sender);
    void __fastcall miAdvProdsClick(TObject *Sender);
    void __fastcall miGameSaveClick(TObject *Sender);
    void __fastcall miVerifynextturnClick(TObject *Sender);
    void __fastcall miAddOrderInt(TObject *Sender);
    void __fastcall TreeViewCompare(TObject *Sender, TTreeNode *Node1,
          TTreeNode *Node2, int Data, int &Compare);
    void __fastcall miAddOrderIntClick(TObject *Sender);
    void __fastcall miCopyOrderClick(TObject *Sender);
    void __fastcall miPasteOrderClick(TObject *Sender);
    void __fastcall miSetWorkIncClick(TObject *Sender);
    void __fastcall miSetEntIncClick(TObject *Sender);
    void __fastcall miAddOrderMovingClick(TObject *Sender);
    void __fastcall bnRunAutoOrdersClick(TObject *Sender);
    void __fastcall miAddOrderAutoGiveClick(TObject *Sender);
    void __fastcall miMapSizeFixClick(TObject *Sender);
    void __fastcall miAdvancedProductsFixClick(TObject *Sender);
    void __fastcall bnWedgeModeClick(TObject *Sender);
    void __fastcall miWedgemodesettingsClick(TObject *Sender);
 void __fastcall miPackUnpackClick(TObject *Sender);
 void __fastcall Exitwithoutsaving1Click(TObject *Sender);
 void __fastcall miPackTurnsClick(TObject *Sender);
 void __fastcall VerTime1Click(TObject *Sender);
    void __fastcall actEnableTempExecute(TObject *Sender);
    void __fastcall miVerSizeClick(TObject *Sender);
    void __fastcall miAddOrderBuildClick(TObject *Sender);
 void __fastcall miGotoLastInfoClick(TObject *Sender);
        void __fastcall UpdTurn0Click(TObject *Sender);
        void __fastcall miCompactTurnClick(TObject *Sender);
    void __fastcall miStoreTurnsClick(TObject *Sender);
        void __fastcall actShowRegInfoExecute(TObject *Sender);
        void __fastcall actShowMapExecute(TObject *Sender);
  void __fastcall miSaveReportClick(TObject *Sender);
  void __fastcall miRepackStoreClick(TObject *Sender);
  void __fastcall miAtlaCartClick(TObject *Sender);
  void __fastcall miOptionsClick(TObject *Sender);
  void __fastcall miReceiveClick(TObject *Sender);
  void __fastcall miAboutClick(TObject *Sender);
  void __fastcall actPrevUnitExecute(TObject *Sender);
  void __fastcall actNextUnitExecute(TObject *Sender);
 void __fastcall miSavereginfoClick(TObject *Sender);
 void __fastcall actCopyToClipboardClick(TObject *Sender);
 void __fastcall popupFactionMessagesPopup(TObject *Sender);
 void __fastcall popupViewDefaultPopup(TObject *Sender);
 void __fastcall miSaveMapClick(TObject *Sender);
    void __fastcall miMoveOrderUpClick(TObject *Sender);
    void __fastcall miMoveOrderDownClick(TObject *Sender);
private:
    void LoadOptions();
    void SaveOptions();	// User declarations
protected:
    VTreeNodes *vtreenodes;
    VListItems *vlistitems;
    TTreeNode *curNode;
    int MapMode;
    int NoSave;
// 1 - food, 2 - normal, 4 - adv, 8 - show null
    unsigned char ProdModeAttr;
public:		// User declarations
    __fastcall TMainForm(TComponent* Owner);
    virtual __fastcall ~TMainForm();
    void SaveGameList();
    void VerGameLoaded();
    void DrawMap();
    void DrawRegion(TCanvas *Canv,int x,int y,int z, int xc, int yc);
    void DrawGrid(TCanvas *Canv,int xc, int yc, int grids[6]); //0-none,1-wall,2-prohod,3-selected
    void DrawRegionTown(TCanvas * Canv, int x, int y, int z, int xc, int yc);
    void DrawRegionProd(TCanvas * Canv, int x, int y, int z, int xc, int yc);
    void DrawRegionWedge(TCanvas * Canv, int x, int y, int z, int xc, int yc);
    int AddMapImage(AnsiString fName);
    int AddInfoImage(AnsiString fName);
    int AddToolBarImage(AnsiString fName);
    int xcentr,ycentr,zcentr;
    void ChangeLevel(int dir);
    void PointToCoord(int x, int y, int & xloc, int & yloc);
    void ChangeTurn(int dir);
    void ViewShow();
    void OnSetCurRegion();
    void MakeTreeView();
    VTreeNode* FindTreeNode(int type, int value);
    void EditOrder(int ind, AOrder *neworder=0);
    void ShowMapMode();
    bool ProcessNonLocalOrders(AOrder * ord);
    void SetlbCoords(int xloc, int yloc);
};
typedef Graphics::TBitmap TBmp;
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
