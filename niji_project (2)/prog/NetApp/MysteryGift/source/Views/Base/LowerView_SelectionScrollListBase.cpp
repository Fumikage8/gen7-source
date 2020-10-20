//==============================================================================
/**
 * @file   LowerView_SelectionScrollListBase.cpp
 * @date   2015/07/31 Fri. 18:22:30
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Util/app_util_GetPaneHelper.h>
#include  <NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h>
#include  "../../ApplicationSystem/GiftWork.h"

#include  "../../Common/ButtonHelper.h"

#include  "../../GiftResource.h"
#include  "./LowerView_SelectionScrollListBase.h"

// =============================================================================
/**/
//==============================================================================
namespace {

  /**
    ButtonID
  */
  enum  {
    /*  スクロールリストはPaneList内でボタン化される  */
    BUTTON_Back,  /**<  戻る  */
  };


  static const NetApp::Gift::ButtonHelperInfo buttonHelperInfos[] = 
  {
    NetApp::Gift::ButtonHelperInfo(
      BUTTON_Back,
      PANENAME_GIFT_SLC_LOW_000_PANE_BACKBUTTON,
      PANENAME_GIFT_BTN_LOW_002_PANE_BOUND_00,
      true,
      LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_TOUCH,
      LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_RELEASE,
      LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_CANSEL,
      LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_ACTIVE,
      LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_PASSIVE
    ),
  };


  static const u32  ListItemsOnScreen = 7;    /*  画面上に同時に表示される項目数  */

}




namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_SelectionScrollListBase::LowerView_SelectionScrollListBase(GiftWork* pWork)
  : LowerView_SelectionBase(pWork)
  , m_pPaneListView(NULL)
  , m_pScrollListCursor(NULL)
  , m_pScrollBarPane(NULL)
{
  app::util::Heap*      pHeap     = pWork->GetAppHeap();
  gfl2::heap::HeapBase* pHeapBase = pHeap->GetDeviceHeap();

  CreateButton(
    pWork->GetAppHeap(),
    this,
    LAYOUTWORK_SelectionLow,
    0,
    buttonHelperInfos,
    GFL_NELEMS(buttonHelperInfos)
  );
  RegisterShortcut_B(BUTTON_Back);
  GetButtonManager()->SetButtonSelectSE(BUTTON_Back, SEID_CANCEL);
  SetInputListener( this );

  GetG2DUtil()->StartAnime( LAYOUTWORK_SelectionLow, LA_GIFT_GIFT_SLC_LOW_000_BUTTON_PATERN_LIST);

  m_pPaneListView = GFL_NEW(pHeapBase)  app::tool::PaneListView(pHeap, true);
  m_pPaneListView->SetListener(this);
  AddSubView(m_pPaneListView);

  /*  pane  */
  {
    app::util::G2DUtil*       pG2DUtil  = GetG2DUtil();
    gfl2::lyt::LytWk*         pLayout   = pG2DUtil->GetLayoutWork( LAYOUTWORK_SelectionLow );
    app::util::GetPaneHelper  helper(pLayout, &(pG2DUtil->GetLayoutResourceID( 0 )));

    m_pScrollBarPane  = helper.GetPane(PANENAME_GIFT_SLC_LOW_000_PANE_SCROLL);
  }

  /*  cursor  */
  m_pScrollListCursor = GFL_NEW(pHeapBase)  NetAppLib::UI::NetAppMenuCursor(pWork, GetG2DUtil(), RESOURCE_LayoutCommonMenuCursor, 0, LAYOUTWORK_SelectionLow);
  
  /*  Layoutのモード切り替えを終わらせておく  */
  GetG2DUtil()->Update2D();
}
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_SelectionScrollListBase::~LowerView_SelectionScrollListBase()
{
  CleanupList();

  if(m_pPaneListView) m_pPaneListView->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pScrollListCursor);
  GFL_SAFE_DELETE(m_pPaneListView);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectionScrollListBase::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  OnCursorMove(pos);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectionScrollListBase::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
//  OnUpdateItem();

  OnUpdateItem(pane[pane_index].text, pos);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectionScrollListBase::SetupList(const u32 itemNum)
{
  CleanupList();

  /*  panelist  */
  app::util::Heap*                pHeap                   = m_pWork->GetAppHeap();
  gfl2::heap::HeapBase*           pHeapBase               = pHeap->GetDeviceHeap();
  app::util::G2DUtil*             pG2DUtil                = GetG2DUtil();
  gfl2::lyt::LytWk*               pLayout                 = pG2DUtil->GetLayoutWork( LAYOUTWORK_SelectionLow );
  app::util::GetPaneHelper        helper(pLayout, &(pG2DUtil->GetLayoutResourceID( 0 )));

  app::tool::PaneList::SETUP_PARAM  paneListSetupParam    = 
  {
    pHeapBase,                      //  gfl2::heap::HeapBase * heap;  //!< ヒープ
                                    //  
    pLayout,                        //  gfl2::lyt::LytWk * pLytWk;    //!< レイアウトワーク
    m_PaneData,                     //  const PANE_DATA * paneList;   //!< ペインデータ
    PaneDataNum,                    //  u32	paneListMax;              //!< ペインデータ数
                                    //  
    itemNum,                        //  u32	valMax;					//!< 実際に使用するリストの項目数（ペインの数ではない）
    ListItemsOnScreen,              //  u32	listPosMax;			//!< カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
    0,                              //  u32	listPos;				//!< 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
    0,                              //  f32	listScroll;			//!< 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                                    //                      //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                                    //                      //   valMax-1になるようにする設定する。
                                    //  
    100.0f,                         //  f32	defPY;					//!< ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                                    //                      //   この座標で指定した場所にあるペインが描画開始位置のペインになる
    308.0f,                         //  f32	defSX;					//!< ペインのXサイズ(レイアウトデディターで厳密に確認）
     28.0f,                         //  f32	defSY;					//!< ペインのYサイズ(レイアウトデディターで厳密に確認）
                                    //  
    -1,                             //  s32	writeOffsetY;		//!< 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)
                                    //  
    NULL,                           //  gfl2::lyt::LytPane * listStartPane;   //!< 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )
                                    //  
    this,                           //  CallBack * cbFunc;	//!< コールバック関数
    NULL,                           //  void * cbWork;			//!< コールバック関数に渡すワーク
                                    //  
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(), //  gfl2::ui::DeviceManager * uiDevMan;		//!< UIデバイスマネージャ
    m_pScrollListCursor->GetAppToolMenuCursor()                         //  app::tool::MenuCursor * menuCursor;		//!< カーソル
  };


  app::util::ScrollBar::SETUP_PARAM scrollBarSetupParam =
  {
    pHeapBase,                                                    //    gfl2::heap::HeapBase * heap;                //!< ヒープ
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,              //    app::tool::ScrollBar::ScrollDirection dir;  //!< スクロールの向き
    paneListSetupParam.valMax - paneListSetupParam.listPosMax,    //    u32 max;                                    //!< スクロール最大値
                                                                  //
                                                                  //    // @note  以下、[set1]か[set2]を設定してください
                                                                  //    // [set1]
    pLayout,                                                      //    gfl2::lyt::LytWk * lytwk;                   //!< レイアウトワーク ( NULLの場合、[set2]を参照します )
    pG2DUtil->GetLayoutResourceID( 0 ),                           //    gfl2::lyt::LytMultiResID res_id;            //!< リソースID
    PANENAME_GIFT_SLC_LOW_000_PANE_SCROLL,                        //    gfl2::lyt::LytPaneIndex parts_index;        //!< 本体の部品ペインのインデックス
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,                 //    gfl2::lyt::LytPaneIndex mark_index;         //!< ツマミ部分のペインのインデックス
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,                    //    gfl2::lyt::LytPaneIndex top_index;          //!< スクロール値最小位置のペインのインデックス
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,                 //    gfl2::lyt::LytPaneIndex bottom_index;       //!< スクロール値最大位置のペインのインデックス
                                                                  //    // [set2]
    NULL,                                                         //    gfl2::lyt::LytPane * parts_pane;            //!< 本体の部品ペイン
    NULL,                                                         //    gfl2::lyt::LytPane * mark_pane;             //!< ツマミ部分のペイン
    NULL,                                                         //    gfl2::lyt::LytPane * top_pane;              //!< スクロール値最小位置のペイン
    NULL                                                          //    gfl2::lyt::LytPane * bottom_pane;           //!< スクロール値最大位置のペイン
  };


  /* 0 */
  helper.Clear();
  m_PaneData[0].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_00).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[0].text          = m_pTextBoxes[TEXTBOX_Small1of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[0].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[0].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[0].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_00_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[0].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_00_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[0].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_00_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[0].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_00_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 1 */
  helper.Clear();
  m_PaneData[1].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_01).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[1].text          = m_pTextBoxes[TEXTBOX_Small2of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[1].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[1].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[1].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[1].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[1].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[1].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 2 */
  helper.Clear();
  m_PaneData[2].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_02).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[2].text          = m_pTextBoxes[TEXTBOX_Small3of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[2].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[2].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[2].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[2].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[2].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[2].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 3 */
  helper.Clear();
  m_PaneData[3].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_03).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[3].text          = m_pTextBoxes[TEXTBOX_Small4of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[3].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[3].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[3].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[3].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[3].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[3].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 4 */
  helper.Clear();
  m_PaneData[4].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_04).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[4].text          = m_pTextBoxes[TEXTBOX_Small5of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[4].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[4].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[4].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[4].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[4].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[4].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 5 */
  helper.Clear();
  m_PaneData[5].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_05).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[5].text          = m_pTextBoxes[TEXTBOX_Small6of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[5].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[5].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[5].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[5].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[5].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[5].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 6 */
  helper.Clear();
  m_PaneData[6].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_06).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[6].text          = m_pTextBoxes[TEXTBOX_Small7of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[6].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[6].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[6].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_06_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[6].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_06_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[6].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_06_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[6].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_06_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 7 */
  helper.Clear();
  m_PaneData[7].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_07).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[7].text          = m_pTextBoxes[TEXTBOX_Small8of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[7].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[7].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[7].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_07_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[7].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_07_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[7].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_07_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[7].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_07_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ

  /* 8 */
  helper.Clear();
  m_PaneData[8].base          = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_08).Peek();      //    gfl2::lyt::LytPane * base;        //!< 親ペイン
  m_PaneData[8].text          = m_pTextBoxes[TEXTBOX_Small9of9];                                    //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
  m_PaneData[8].bound         = helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00);    //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
  m_PaneData[8].cursor_pos    = helper.GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);            //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
  m_PaneData[8].holdAnime     = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_08_TOUCH;                         //    u32 holdAnime;				//!< タッチ時のアニメ
  m_PaneData[8].releaseAnime  = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_08_RELEASE;                       //    u32 releaseAnime;			//!< リリース時のアニメ
  m_PaneData[8].cancelAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_08_CANSEL;                        //    u32 cancelAnime;			//!< キャンセル時のアニメ
  m_PaneData[8].selectAnime   = LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_08_TOUCH_RELEASE;                 //    u32 selectAnime;			//!< キー選択アニメ


  m_pPaneListView->CreatePaneList(&paneListSetupParam);

  if(ListItemsOnScreen < itemNum)
  {
    m_pScrollBarPane->SetVisible(true);
    m_pPaneListView->CreateScrollBar(&scrollBarSetupParam);
  }
  else
  {
    m_pScrollBarPane->SetVisible(false);
  }
  
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_BUTTON, true, false, true, false );
    pPaneList->SetSE(SEQ_SE_SELECT1, SEID_DECIDE, app::tool::PaneList::SE_NONE);


    { /*  タッチ範囲補正  */
      const gfl2::math::VEC3  pos     = m_PaneData[0].bound->GetTranslate();
            u32               pos_max = paneListSetupParam.listPosMax;
      if( paneListSetupParam.valMax < pos_max )
      {
        pos_max = paneListSetupParam.valMax;
      }
      
      const f32 listPX  = 0.0f;
      const f32 listPY  = paneListSetupParam.defPY;
      const f32 listSX  = paneListSetupParam.defSX;
      const f32 listSY  = paneListSetupParam.defSY;
      
      const f32 lx = listPX - listSX / 2.0f;
      const f32 rx = lx + listSX;
      const f32 uy = listPY + pos.y + listSY / 2.0f;
      const f32 dy = uy - listSY * pos_max;

      pPaneList->MakeTouchArea( lx, rx, uy, dy );
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectionScrollListBase::CleanupList(void)
{
  m_pPaneListView->DeleteScrollBar();
  m_pPaneListView->DeletePaneList();
}


/*
  UIView
*/
void  LowerView_SelectionScrollListBase::Update(void)
{
  LowerView_SelectionBase::Update();    /*  udpate super  */
  m_pScrollListCursor->Update();
}

void  LowerView_SelectionScrollListBase::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  LowerView_SelectionBase::Draw(displayNo);      /*  super */
  m_pScrollListCursor->Draw(displayNo);
}


/*
  end UIView
*/



/*
  UIInputListener
*/
//--------------------------------------------------------------------------------------------
/**
  * @brief ペインイベントの検知
  */
//--------------------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_SelectionScrollListBase::OnLayoutPaneEvent( const u32 buttonID )
{
  app::ui::UIInputListener::ListenerResult  result  = app::ui::UIInputListener::ENABLE_ACCESS;

  switch(buttonID)
  {
    /*
      exit
    */
    case  BUTTON_Back:  /**<  戻る  */
      OnExit();
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    default:
      break;
  }


  return  result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
//  app::ui::UIInputListener::ListenerResult  LowerView_SelectionScrollListBase::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
//  {
//    app::ui::UIInputListener::ListenerResult  result;
//    
//    
//    result  = LowerView_SelectionBase::OnKeyAction( pButton, pKey, pStick );
//    
//    /*  paneList    */
//    if(m_pPaneListTool)
//    {
//      if(m_pPaneListTool && (result == ENABLE_ACCESS))
//      {
//        m_pPaneListTool->SetKeyEnable(true);
//      }
//    }
//    
//    
//    return result;
//  }

/*
  end UIInputListener
*/


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectionScrollListBase::PaneListItemSelectListener( u32 pos )
{
  GFL_PRINT("[%s]:%2d\n", __FUNCTION__, pos);
  OnSelectedItem(m_pPaneListView->GetPaneList()->GetItemPos());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectionScrollListBase::PaneListItemCatchListener( u32 pos )
{
  GFL_PRINT("[%s]:%2d\n", __FUNCTION__, pos);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectionScrollListBase::PaneListResultHandler(const app::tool::PaneList::Result result)
{
  switch(result)
  {
    case  app::tool::PaneList::RET_LIST_SELECT:        //!< 項目選択
//      OnSelectedItem(m_pPaneListTool->GetItemPos());
      break;

    case  app::tool::PaneList::RET_LIST_CANCEL:        //!< キャンセル ( Bボタン )
//      OnCancel();
      break;


    /*  nop  */
    case  app::tool::PaneList::RET_NONE:               //!< 動作なし
    case  app::tool::PaneList::RET_LIST_UP:            //!< 上へスクロール中
    case  app::tool::PaneList::RET_LIST_DOWN:          //!< 下へスクロール中
    case  app::tool::PaneList::RET_LIST_PAGE_UP:       //!< 上へページ送り
    case  app::tool::PaneList::RET_LIST_PAGE_DOWN:     //!< 下へページ送り
    case  app::tool::PaneList::RET_LIST_TOUCH:         //!< タッチ処理中
    case  app::tool::PaneList::RET_LIST_AUTO_SCRILL:   //!< オートスクロール中
    case  app::tool::PaneList::RET_LIST_CATCH:         //!< 項目キャッチ
    case  app::tool::PaneList::RET_LIST_PROCESSING:    //!< なんらかの処理を実行中 ( Aボタンアニメなど )
    default:
      break;
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::lyt::LytBounding* LowerView_SelectionScrollListBase::GetBoundingPaneFromItemParts(gfl2::lyt::LytParts* pParts)
{
  app::util::G2DUtil*             pG2DUtil                = GetG2DUtil();
  gfl2::lyt::LytWk*               pLayout                 = pG2DUtil->GetLayoutWork( LAYOUTWORK_SelectionLow );
  const gfl2::lyt::LytMultiResID& rLayoutMultiResourceID  = pG2DUtil->GetLayoutResourceID( 0 );

  return pLayout->GetBoundingPane(pParts, PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00, &rLayoutMultiResourceID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void LowerView_SelectionScrollListBase::OnSetInputEnabled( bool isEnable )
{
  m_pPaneListView->SetInputEnabled(isEnable);
}



} /*  namespace Gift */
} /*  namespace NetApp      */
