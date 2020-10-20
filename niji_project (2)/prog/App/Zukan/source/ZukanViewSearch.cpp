//==============================================================================
/**
 @file    ZukanViewSearch.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の検索画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>
#include <str/include/gfl2_StrBuf.h>
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_ScrollBar.h>
#include <AppLib/include/Tool/app_tool_MessageMenuView.h>
#include <AppLib/include/Ui/UIViewManager.h>

#include <Sound/include/Sound.h>

#include <System/include/ThreadPriority.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/search.h>
#include <niji_conv_header/app/zukan/search_pane.h>
#include <niji_conv_header/app/zukan/search_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"
#include "ZukanDataManager.h"
#include "ZukanSearchThread.h"
#include "ZukanTool.h"
#include "ZukanFrameSearch.h"

#include "ZukanViewSearch.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewSearchListener
 @brief     図鑑の検索画面のビューのリスナー
 */
//==============================================================================
class ZukanViewSearchListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewSearchListener);

public:
  ZukanViewSearchListener(ZukanViewSearch* view);
  virtual ~ZukanViewSearchListener();

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 paneId );

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewSearch* m_view;
  InputState       m_inputState;
};


//==============================================================================
//
//  ZukanViewSearchListener
//
//==============================================================================
ZukanViewSearchListener::ZukanViewSearchListener(ZukanViewSearch* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)  // @fix cov_ctr[11680]: 未初期化を初期化
{}
ZukanViewSearchListener::~ZukanViewSearchListener()
{}

app::ui::UIInputListener::ListenerResult ZukanViewSearchListener::OnLayoutPaneEvent( const u32 painId )
{
  app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
  switch(m_inputState)
  {
  case INPUT_STATE_OCCUPY:
    {
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  case INPUT_STATE_ENABLE:
    { 
      u32 button_id = painId;
      m_view->Req(ZukanViewSearch::REQ_ID_BUTTON_DECIDE, button_id);
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
void ZukanViewSearchListener::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
  switch(m_inputState)
  {
  case INPUT_STATE_ENABLE:
    {
      u32 button_id = paneId;
      m_view->Req(ZukanViewSearch::REQ_ID_BUTTON_TOUCH, button_id);
      break;
    }
  }
}
void ZukanViewSearchListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
  
  app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
}




//==============================================================================
/**
 @class     ZukanSearchPaneListViewListener
 @brief     図鑑の検索画面のペインリストビューのリスナー
 */
//==============================================================================
class ZukanSearchPaneListViewListener : public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanSearchPaneListViewListener);

public:
  ZukanSearchPaneListViewListener(ZukanViewSearch* view);
  virtual ~ZukanSearchPaneListViewListener();

  virtual void PaneListItemSelectListener( u32 pos );
  //virtual void PaneListItemCatchListener( u32 pos );

private:
  ZukanViewSearch* m_view;
};


//==============================================================================
//
//  ZukanViewSearchListener
//
//==============================================================================
ZukanSearchPaneListViewListener::ZukanSearchPaneListViewListener(ZukanViewSearch* view)
  : app::tool::PaneListView::Listener(),
    m_view(view)
{}
ZukanSearchPaneListViewListener::~ZukanSearchPaneListViewListener()
{}

void ZukanSearchPaneListViewListener::PaneListItemSelectListener( u32 pos )
{
  m_view->Req( ZukanViewSearch::REQ_ID_PANE_LIST_DECIDE, static_cast<s32>(pos) );
}
//void ZukanSearchPaneListViewListener::PaneListItemCatchListener( u32 /*pos*/ )
//{}




//==============================================================================
/**
 @class     ZukanSearchPaneListCallBack
 @brief     図鑑の検索画面のペインリストのコールバック
 */
//==============================================================================
class ZukanSearchPaneListCallBack : public app::tool::PaneList::CallBack
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanSearchPaneListCallBack);

public:
  ZukanSearchPaneListCallBack(ZukanViewSearch* view);
  virtual ~ZukanSearchPaneListCallBack();

public:
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 );

private:
  ZukanViewSearch* m_view;
};

//==============================================================================
//
//  ZukanSearchPaneListCallBack
//
//==============================================================================
ZukanSearchPaneListCallBack::ZukanSearchPaneListCallBack(ZukanViewSearch* view)
  : app::tool::PaneList::CallBack(),
    m_view(view)
{}
ZukanSearchPaneListCallBack::~ZukanSearchPaneListCallBack()
{}

void ZukanSearchPaneListCallBack::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  //GFL_PRINT("ZukanSearchPaneListCallBack::CallBack_CursorMove  %d %d\n", pane_index, pos);
  m_view->MoveCursor( cur_pane, pane_index, pos );
}
void ZukanSearchPaneListCallBack::CallBack_CursorOff( void * /*work*/, const app::tool::PaneList::PANE_DATA * /*pane*/, u32 /*pane_index*/, u32 /*pos*/ )
{}
void ZukanSearchPaneListCallBack::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  //GFL_PRINT("ZukanSearchPaneListCallBack::CallBack_ListWrite  %d %d\n", pane_index, pos);
  m_view->WriteList( pane, pane_index, pos );
}
void ZukanSearchPaneListCallBack::CallBack_ItemChange( void * /*work*/, u32 /*pos1*/, u32 /*pos2*/ )
{}




//==============================================================================
/**
 @class     ZukanSearchMessageMenuEventHandler
 @brief     App::Tool::MessageMenuViewのイベントハンドラー
 */
//==============================================================================
class ZukanSearchMessageMenuEventHandler : public App::Tool::MessageMenuView::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanSearchMessageMenuEventHandler);
public:
  ZukanSearchMessageMenuEventHandler(ZukanViewSearch* view);
  virtual ~ZukanSearchMessageMenuEventHandler();
public:
  virtual void MessageMenuView_OnEvent(const App::Tool::MessageMenuView::IEventHandler::EventCode event);
private:
  ZukanViewSearch* m_view;
};

//==============================================================================
//
//  ZukanSearchMessageMenuEventHandler
//
//==============================================================================
ZukanSearchMessageMenuEventHandler::ZukanSearchMessageMenuEventHandler(ZukanViewSearch* view)
  : App::Tool::MessageMenuView::IEventHandler(),
    m_view(view)
{}
ZukanSearchMessageMenuEventHandler::~ZukanSearchMessageMenuEventHandler()
{}
void ZukanSearchMessageMenuEventHandler::MessageMenuView_OnEvent(const App::Tool::MessageMenuView::IEventHandler::EventCode event)
{
  switch(event)
  {
  case App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes:
    {
      m_view->Req(ZukanViewSearch::REQ_ID_MESSAGE_MENU, 0);  // YES
      break;
    }
  case App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo:
    {
      m_view->Req(ZukanViewSearch::REQ_ID_MESSAGE_MENU, 1);  // NO
      break;
    }
  }
}




//==============================================================================
//
//  ZukanViewSearch
//
//==============================================================================
ZukanViewSearch::ZukanViewSearch(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_pokeIconDisplayDevMem(NULL),
    m_viewListener(NULL),
    m_lowerIsSub(false),
    m_paneListViewForLowerMain(NULL),
    m_paneListViewListenerForLowerMain(NULL),
    m_paneListCallBackForLowerMain(NULL),
    m_paneListViewForLowerSub(NULL),
    m_paneListViewListenerForLowerSub(NULL),
    m_paneListCallBackForLowerSub(NULL),
    m_gameLang(ZukanType::ZUKAN_LANG_JAPAN),
    m_paneListPosInfoForLowerMain(),
    m_paneListPosInfoForLowerSubTmp(),
    m_resetButtonPartsPane(NULL),
    m_decideButtonPartsPane(NULL),
    m_subjectNameTextPaneForLowerSub(NULL),
    m_scrollBarTopPaneForLowerMain(NULL),
    m_scrollBarTopPaneForLowerSub(NULL),
    m_listStartPaneForLowerMain(NULL),
    m_listStartPaneForLowerSub(NULL),
    m_mesMenuViewEventHandler(NULL),
    m_mesMenuViewResultIsYes(false),
    m_mesMenuViewStrBufWindowMessage(NULL),
    m_mesMenuViewStrBufYes(NULL),
    m_mesMenuViewStrBufNo(NULL),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_pokeIconDisplay(NULL),
    m_searchCriticalSection(NULL),
    m_searchEvent(NULL),
    m_searchThread(NULL),
    m_drawFigureDataIndexList(NULL),
    m_dispPokeDataIndexNum(0),
    m_dispPokeDataIndexList(NULL),
    m_searchIsFinished(false),
    m_startPokeIconDisplay(false),
    m_uiViewManInputExclusiveKey(false)
{
  m_uiViewManInputExclusiveKey = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->GetInputExclusiveKeyByButtonAnimation();
  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetInputExclusiveKeyByButtonAnimation(true);  // @fix NMCat[930] ポケモン図鑑の検索画面で操作中の他の操作が排他されていないことによる不具合
                                                                                                  //                 同じツリーに存在するUIVIewのボタンマネージャを排他する機能を使う
                                                                                                  
  // 初期化
  {   
    for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
    {
      m_waitAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }

    for(u32 i=0; i<BUTTON_ID_NUM_FOR_LOWER_MAIN; ++i)
    {
      m_buttonPaneInfoForLowerMain[i] = ZukanType::ButtonPaneInfo();
    }
    for(u32 i=0; i<BUTTON_ID_NUM_FOR_LOWER_SUB; ++i)
    {
      m_buttonPaneInfoForLowerSub[i] = ZukanType::ButtonPaneInfo();
    }

    // 構造体の配列の初期化をしない。コメントアウトしておく。
    //for(u32 i=0; i<PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN; ++i)
    //{
    //  m_paneListDataForLowerMain[i] = ;
    //}
    //for(u32 i=0; i<PANE_LIST_ELEM_NUM_FOR_LOWER_SUB; ++i)
    //{
    //  m_paneListDataForLowerSub[i] = ;
    //}
  
    for(u32 i=0; i<PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN; ++i)
    {
      m_paneIndexPosForLowerMain[i] = ZUKAN_CONST_PANE_LIST_PANE_INDEX_NULL;
    }
 
    m_gameLang = ZukanType::ConvertPokemonLang2ZukanLang(System::GetLang());
  }

  // @fix NMCat[1301] 【図鑑】フォルムチェンジで検索条件から外れた時の挙動について  →検索画面で「今見ているフォルム」を使うようにするために、「セーブデータのフォルム」をそれに書き換える
  gfl2::std::MemClear(m_saveDrawData, sizeof(ZukanContents::SaveDrawData)*MONSNO_END);
  ZukanContents::StoreSaveDrawData(m_saveDrawData);
  m_param.contents->SetDrawFigureDataIndexToSaveData();

  // メモリ
  m_pokeIconDisplayDevMem = GFL_CREATE_LOCAL_HEAP(m_param.appHeap->GetDeviceHeap(), POKE_ICON_DISPLAY_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);

  // 検索設定
  {
    this->setSearchSubjectPaneListPosInfoOnStarting();
  }

  // ZukanViewSearchListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewSearchListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_SEARCH);
    if(buf)
    {
      const app::util::G2DUtil::LytResData resTbl[] =
      {
        // res_0
        {
          buf,
          0,
          app::util::G2DUtil::ATTACH_ROOT
        },
      };
     
      const app::util::G2DUtil::LytwkData wkTbl[] =
      {
        // LWT_WK_ID_UPPER  wk_0
        {
          // 上画面
          0,  // res_0
          LYTRES_SEARCH_ZUKAN_SRC_UPP_000_BFLYT,
          LA_SEARCH_ZUKAN_SRC_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
     
        // LWT_WK_ID_LOWER_MAIN  wk_1
        {
          // 下画面
          0,  // res_0
          LYTRES_SEARCH_ZUKAN_MIN_LOW_006_BFLYT,
          LA_SEARCH_ZUKAN_MIN_LOW_006___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          LYT_WK_ID_NUM,   // wk_0, wk_1の2個
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );
      
      app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

      // ページ内容生成
      this->createPageOnce();

      // ポケアイコン表示クラス
      {
        gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_UPPER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
        gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

        m_pokeIconDisplay = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSearchPokeIconDisplay(
            m_pokeIconDisplayDevMem,
            m_param.dataManager,
            m_param.contents,
            pG2DUtil,
            LYT_WK_ID_UPPER,
            lytWk_0,
            lytRes_0
        );

        m_pokeIconDisplay->StartDisplayPage(
          m_drawFigureDataIndexList,
          m_dispPokeDataIndexNum,
          m_dispPokeDataIndexList
        );
        m_startPokeIconDisplay = true;
        m_pokeIconDisplay->ForcePause();
      }

      // ボタンマネージャ
      //this->createButtonManager();  // @fix GFCHECK[8400] ページ切り替えで破棄/生成を繰り返すので、一番最後に生成するようにした。だからここはコメントアウト。

      // メニューカーソルレイアウト生成
      m_param.tool->CreateMenuCursorLayoutWork(pG2DUtil, gfl2::lyt::DISPLAY_LOWER, app::tool::MenuCursor::SIZE_SMALL); 
     
      // この時点では(m_lowerIsSub == false)が真なので、LowerMainのほうだけ行う。
      {
        // ペインリストに使われているペインに値を設定したいので一度つくる。すぐ破棄する。
        // ペインリスト
        this->createPaneListForLowerMain();
        this->destroyPaneListForLowerMain();
        m_scrollBarTopPaneForLowerMain->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
      }

      // インアニメ
      //this->startInSeq();  // @fix GFCHECK[8400] ボタンマネージャ生成後にインアニメを行うので、ここはコメントアウト。
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  ZukanUtil::SetPokeModelVisible(m_param.tool, false);

  // MessageMenuView
  this->createMesMenuViewItems();

  // スレッド
  {
    m_searchCriticalSection = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::thread::CriticalSection();
    m_searchCriticalSection->Initialize();
    m_searchEvent           = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::thread::Event(m_param.appHeap->GetDeviceHeap());
    m_searchEvent->Initialize();
    m_searchThread          = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSearchThread(m_param.appHeap, this, System::ThreadPriority::ZUKAN_SEARCH_THREAD_PRIORITY);
  }

  // ボタンマネージャ
  this->createButtonManager();  // @fix GFCHECK[8400] ページ切り替えで破棄/生成を繰り返すので、一番最後に生成する。

  // インアニメ
  this->startInSeq();  // @fix GFCHECK[8400] ボタンマネージャ生成後にインアニメを行う。

  // @fix NMCat[1301] 【図鑑】フォルムチェンジで検索条件から外れた時の挙動について
  // 検索画面に入ったときの検索項目で検索を開始する
  {
    // ポケアイコン表示をやめる
    m_pokeIconDisplay->CancelDisplay();
    m_startPokeIconDisplay = false;

    GFL_ASSERT(m_searchIsFinished);  // 開発中に気付かせるためのASSERT。この時点ではm_searchIsFinishedはtrueになっていなければならないので、trueじゃない場合は初期化処理のどこかがおかしい。
    
    this->startSearch();
      // m_searchIsFinishedは最初はtrueになっているので、ここでstartSearchを呼べば必ず検索を開始する。
      // ユーザが検索結果のフォルムを変更してから検索画面にやって来たとしても、
      // 検索画面では検索項目にマッチしたフォルムに戻して表示できるよう起動時に検索しに行くようにした。
      // 決定で終了すれば「検索項目にマッチしたフォルムに戻したもの」で終わり、
      // キャンセルで終了すれば「ユーザが検索結果から変更したフォルム(検索画面に入る前のフォルム)」で終わる。
  }
}
ZukanViewSearch::~ZukanViewSearch()
{
  // スレッドを終了させる
  {
    this->requestSearchThreadEnd();
    
    u32 endlessLoopCount = 0;
    while(!(m_searchThread->HasEnded()))
    {
      gfl2::thread::ThreadStatic::CurrentSleep(1);
 
      if(++endlessLoopCount >= ZUKAN_CONST_ENDLESS_LOOP_COUNT_MAX_LONG)
      {
        GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
        break;
      }
    }
  }

  // スレッド
  {
    GFL_SAFE_DELETE(m_searchThread);
    m_searchEvent->Finalize();
    GFL_SAFE_DELETE(m_searchEvent);
    GFL_SAFE_DELETE(m_searchCriticalSection);
  }

  // MessageMenuView
  this->destroyMesMenuViewItems();

  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // メニューカーソルレイアウト破棄
  //m_param.tool->DestroyMenuCursorLayoutWork();

  // ペインリスト
  this->destroyPaneListForLowerSub();
  this->destroyPaneListForLowerMain();

  // ボタンマネージャ
  this->destroyButtonManager();

  // ポケアイコン表示クラス
  GFL_SAFE_DELETE(m_pokeIconDisplay);

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewSearchListener
  GFL_SAFE_DELETE(m_viewListener);

  // 検索設定
  {
    GFL_SAFE_DELETE_ARRAY(m_dispPokeDataIndexList);
    GFL_SAFE_DELETE_ARRAY(m_drawFigureDataIndexList);
  }

  // メモリ
  GFL_DELETE_HEAP(m_pokeIconDisplayDevMem);
  m_pokeIconDisplayDevMem = NULL;

  // @fix NMCat[1301] 【図鑑】フォルムチェンジで検索条件から外れた時の挙動について  →元々の「セーブデータのフォルム」に戻す
  ZukanContents::RestoreSaveDrawData(m_saveDrawData);

  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetInputExclusiveKeyByButtonAnimation(m_uiViewManInputExclusiveKey);
}

void ZukanViewSearch::Update(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  
  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      break;
    }
  case DISP_STATE_ON_IN:
    {
      if( pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
      {
        if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
        {
          // この時点では(m_lowerIsSub == false)が真なので、LowerMainのほうだけ行う。
          // インアニメが終わってから生成しないと正しい座標が取れない。
          // ペインリスト
          this->createPaneListForLowerMain();
          m_scrollBarTopPaneForLowerMain->SetVisible(true);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
          m_paneListViewForLowerMain->SetInputEnabledAllView(true);  // app::ui::UIViewのメンバ  // タッチ連打するとボタンが凹みっぱなしになったのでこの対策を入れた。
          m_paneListViewForLowerMain->GetPaneList()->SetInputEnabled(true);  // PaneListはUIViewの入力禁止だけでは防げない
       
          // ポケアイコン表示クラス
          m_pokeIconDisplay->ReleasePause();
       
          this->changeDispState(DISP_STATE_ON_WORK);
          m_viewListener->SetInputState(ZukanViewSearchListener::INPUT_STATE_ENABLE);
        }
      }
      break;
    }

  case DISP_STATE_ON_WORK:
    {
      if(this->isSearchSubjectPaneListPosInfoDifferent())
      {
        this->startSearch();
      }
      else
      {
        if( !m_startPokeIconDisplay )
        {
          // スレッド  クリティカルセクション
          gfl2::thread::ScopedLock reqListScopedLock(*m_searchCriticalSection);
          if(m_searchIsFinished)
          {
            // ポケアイコン表示
            m_pokeIconDisplay->StartDisplay(
              m_drawFigureDataIndexList,
              m_dispPokeDataIndexNum,
              m_dispPokeDataIndexList
            );
            m_startPokeIconDisplay = true;

            // 1個以上検索できているなら、決定ボタンを押せるようにする
            this->setDecideButtonEnable();
          }
        }
      }
      break;
    }

  case DISP_STATE_ON_WAIT_START_OUT:
    {
      if( !(m_pokeIconDisplay->IsDisplayProgressing()) )
      {
        this->startOutSeq2();
      }
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        // スレッド
        bool isEnd = true;
        if( !(m_searchThread->HasEnded()) )
        {
          this->requestSearchThreadEnd();  // もう一度終了リクエストを出す
          isEnd = false;
        }
        
        if(isEnd)
        {
          if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
          {
            pFadeManager->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &ZukanType::CROSS_FADE_COLOR_START, &ZukanType::CROSS_FADE_COLOR_END, 1);  // ぱっと消す
          }
          this->changeDispState(DISP_STATE_ON_FADE_OUT);
        }
      }
      break;
    }

  case DISP_STATE_ON_LOWER_START_CHANGE:
    {       
      this->startLowerChangeSeq2();
      break;
    }
  case DISP_STATE_ON_LOWER_CHANGING:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        // この時点ではこれからcreateするほうの画面がm_lowerIsSubに指定されている。
        // インアニメが終わってから生成しないと正しい座標が取れない。
        // ペインリスト
        if(m_lowerIsSub)
        {
          //this->destroyPaneListForLowerMain();  // 前の画面  // @fix GFCHECK[8400] 次の画面の生成前に破棄しておいたので、ここはコメントアウト。
          this->createPaneListForLowerSub();    // これからの画面
          m_scrollBarTopPaneForLowerSub->SetVisible(true);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
          m_paneListViewForLowerSub->SetInputEnabledAllView(true);  // app::ui::UIViewのメンバ
          m_paneListViewForLowerSub->GetPaneList()->SetInputEnabled(true);  // PaneListはUIViewの入力禁止だけでは防げない
        }
        else
        {
          //this->destroyPaneListForLowerSub();  // 前の画面  // @fix GFCHECK[8400] 次の画面の生成前に破棄しておいたので、ここはコメントアウト。
          this->createPaneListForLowerMain();  // これからの画面
          m_scrollBarTopPaneForLowerMain->SetVisible(true);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
          m_paneListViewForLowerMain->SetInputEnabledAllView(true);  // app::ui::UIViewのメンバ
          m_paneListViewForLowerMain->GetPaneList()->SetInputEnabled(true);  // PaneListはUIViewの入力禁止だけでは防げない
        }

        this->changeDispState(DISP_STATE_ON_WORK);
        m_viewListener->SetInputState(ZukanViewSearchListener::INPUT_STATE_ENABLE);
      }
      break;
    }

  case DISP_STATE_ON_START_CONFIRM:
    {
      this->startConfirm2();
      break;
    }
  case DISP_STATE_ON_WAIT_CONFIRM:
    {
      break;
    }
  case DISP_STATE_ON_END_CONFIRM:
    {
      this->endConfirm2(m_mesMenuViewResultIsYes);
      break;
    }

  case DISP_STATE_ON_FADE_OUT:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        this->changeDispState(DISP_STATE_WAIT_SAFETY);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_WAIT_SAFETY:
    {
      // メニューカーソルレイアウト破棄
      if( m_param.tool->DestroyMenuCursorLayoutWork() )  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  ←true/false判定が逆だったのを修正。
      {
        m_param.frameCell->Req(ZukanFrameSearchCell::REQ_ID_END);
        this->changeDispState(DISP_STATE_WAIT_END);
      }
      break;
    }
  case DISP_STATE_WAIT_END:
    {
      break;
    }
  }

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
  menuCursor->Update();
  m_pokeIconDisplay->Update();
}
void ZukanViewSearch::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_LOWER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_1

  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->Draw( m_param.appRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
}

void ZukanViewSearch::Req(ReqId reqId, s32 reqParam)
{
  if(m_lowerIsSub)
  {
    switch(reqId)
    {
    case REQ_ID_BUTTON_DECIDE:
      {
        switch(reqParam)
        {
        case BUTTON_ID_BACK  :
          {
            this->startLowerChangeSeq1(CANCEL_POS);
            break;
          }
        }
      }
      break;
    case REQ_ID_BUTTON_TOUCH :
      break;  // 何もしない
    case REQ_ID_PANE_LIST_DECIDE:
      {
        this->startLowerChangeSeq1(reqParam);
        break;
      }
    }
  }
  else
  {
    switch(reqId)
    {
    case REQ_ID_BUTTON_DECIDE:
      {
        switch(reqParam)
        {
        case BUTTON_ID_BACK  :
          {
            if( this->isSearchSubjectPaneListPosInfoChanged() )  // 「表示中の検索設定」が「開始時の検索設定」から変更されていたら
            {
              this->startConfirm1();
            }
            else
            {
              this->startOutSeq1(true);
            }
            break;
          }
        case BUTTON_ID_RESET :
          {
            this->resetSearchSetting();
            break;
          }
        case BUTTON_ID_DECIDE:
          {
            this->startOutSeq1(false);
            break;
          }
        }
      }
      break;
    case REQ_ID_BUTTON_TOUCH :
      break;  // 何もしない
    case REQ_ID_PANE_LIST_DECIDE:
      {
        this->startLowerChangeSeq1(reqParam);
      }
      break;
    case REQ_ID_MESSAGE_MENU:
      {
        this->endConfirm1(reqParam==0);  // 0ならYES, 1ならNO
        break;
      }
    }
  }
}

void ZukanViewSearch::MoveCursor( const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  if(m_lowerIsSub)
  {
    // カーソルの位置で即時検索を開始したいので、カーソルの位置を反映する。
    m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos].pos = pos;
    // listPos, listScrollは決定したときにだけ反映すればいいので、ここでは反映しない。

    // ポケアイコン表示
    this->cancelPokeIconDisplayIfChanged();
  }
}
void ZukanViewSearch::WriteList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk    = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  if(m_lowerIsSub)
  {
    gfl2::lyt::LytParts*    paneListPartsPane  = static_cast<gfl2::lyt::LytParts*>(m_paneListDataForLowerSub[pane_index].base);
    gfl2::lyt::LytParts*    itemPartsPane      = lytWk->GetPartsPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_010_PANE_ITEM, &lytRes_0);

    this->setItem(
        itemPartsPane,
        PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_SUB[pane_index].other1Anime,
        PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_SUB[pane_index].other2Anime,
        static_cast<ZukanType::Subject>(m_paneListPosInfoForLowerMain.pos),
        pos
    );
  }
  else
  {
    this->writeListElemForLowerMain(pane_index, pos);

    // pane_indexからposを取得できるように記憶しておく
    m_paneIndexPosForLowerMain[pane_index] = pos;
  }
}

void ZukanViewSearch::SearchFunction(void)
{
  m_param.contents->MakeDispPokeDataIndexList(
    m_searchSubjectPaneListPosInfoOnSearching,
    m_drawFigureDataIndexList,
    &m_dispPokeDataIndexNum,
    m_dispPokeDataIndexList
  );

  {
    // スレッド  クリティカルセクション
    gfl2::thread::ScopedLock reqListScopedLock(*m_searchCriticalSection);
    m_searchIsFinished = true;
  }
}

void ZukanViewSearch::createButtonManager(void)
{
  u32 buttonIdNum = BUTTON_ID_NUM_FOR_LOWER_MAIN;
  const ZukanType::ButtonConstInfo* BUTTON_CONST_INFO = BUTTON_CONST_INFO_FOR_LOWER_MAIN;
  ZukanType::ButtonPaneInfo* buttonPaneInfo = m_buttonPaneInfoForLowerMain;
  if(m_lowerIsSub)
  {
    buttonIdNum = BUTTON_ID_NUM_FOR_LOWER_SUB;
    BUTTON_CONST_INFO = BUTTON_CONST_INFO_FOR_LOWER_SUB;
    buttonPaneInfo = m_buttonPaneInfoForLowerSub;
  }

  if(m_lowerIsSub)
  {
    // 使わないボタンは消しておく、消されていたボタンを復活させる
    m_resetButtonPartsPane->SetVisible(m_lowerIsSub!=false);
    m_decideButtonPartsPane->SetVisible(m_lowerIsSub!=false);
  }

  ZukanUtil::CreateButtonManager(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_1
      buttonIdNum,
      BUTTON_CONST_INFO,
      NULL,
      buttonPaneInfo
  );

  // 決定ボタンの初期状態
  this->setDecideButtonEnable();
}
void ZukanViewSearch::destroyButtonManager(void)
{
  ZukanUtil::DestroyButtonManager(this);
}

void ZukanViewSearch::createPaneListForLowerMain(void)
{
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(true);

  m_paneListViewListenerForLowerMain = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSearchPaneListViewListener(this);
  m_paneListCallBackForLowerMain     = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSearchPaneListCallBack(this);

  m_paneListViewForLowerMain = ZukanUtil::CreatePaneList(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_1
      m_paneListViewListenerForLowerMain,
      PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN,
      PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_MAIN,
      m_paneListDataForLowerMain,
      &PANE_LIST_CONST_INFO_FOR_LOWER_MAIN,
      m_listStartPaneForLowerMain,  // @fix NMCat[2101] ポケモンナンバー上部に上のアイコンの影が被って表示される  ←ペインの並び順を変更できるように、リストの親ペインを渡す。
      m_paneListCallBackForLowerMain,
      menuCursor,
      m_param.gameManager->GetUiDeviceManager(),
      &SCROLL_BAR_CONST_INFO_FOR_LOWER_MAIN,
      ZukanType::SUBJECT_NUM,
      m_paneListPosInfoForLowerMain.listPos,
      m_paneListPosInfoForLowerMain.listScroll,
      true
  );

  m_paneListViewForLowerMain->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ
  m_paneListViewForLowerMain->GetPaneList()->SetInputEnabled(false);  // PaneListはUIViewの入力禁止だけでは防げない
}
void ZukanViewSearch::destroyPaneListForLowerMain(void)
{
  if( m_paneListViewForLowerMain )
  {
    ZukanUtil::DestroyPaneList(this, m_paneListViewForLowerMain);
    m_paneListViewForLowerMain = NULL;
 
    GFL_SAFE_DELETE(m_paneListCallBackForLowerMain);
    GFL_SAFE_DELETE(m_paneListViewListenerForLowerMain);
    
    app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
    menuCursor->SetVisible(false);
  }
}

void ZukanViewSearch::createPaneListForLowerSub(void)
{
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(true);

  m_paneListViewListenerForLowerSub = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSearchPaneListViewListener(this);
  m_paneListCallBackForLowerSub     = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSearchPaneListCallBack(this);

  u32 valMax = 0;
  switch(m_paneListPosInfoForLowerMain.pos)
  {
  case ZukanType::SUBJECT_ORDER:     valMax = ZukanType::ORDER_TYPE_NUM; break;
  case ZukanType::SUBJECT_INITIAL:
    {
      valMax = ZukanType::GetInitialTypeElemNum(m_gameLang); 
    }
    break;
  case ZukanType::SUBJECT_POKETYPE1: valMax = ZukanType::POKETYPE_TYPE_ELEM_NUM; break;
  case ZukanType::SUBJECT_POKETYPE2: valMax = ZukanType::POKETYPE_TYPE_ELEM_NUM; break;
  case ZukanType::SUBJECT_COLOR:     valMax = ZukanType::COLOR_TYPE_ELEM_NUM; break;
  case ZukanType::SUBJECT_STYLE:     valMax = ZukanType::STYLE_TYPE_ELEM_NUM; break;
  case ZukanType::SUBJECT_FIGURE:    valMax = ZukanType::FIGURE_TYPE_ELEM_NUM; break;
  case ZukanType::SUBJECT_GET:       valMax = ZukanType::GET_TYPE_ELEM_NUM; break;
  }

  m_paneListViewForLowerSub = ZukanUtil::CreatePaneList(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_1
      m_paneListViewListenerForLowerSub,
      PANE_LIST_ELEM_NUM_FOR_LOWER_SUB,
      PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_SUB,
      m_paneListDataForLowerSub,
      &PANE_LIST_CONST_INFO_FOR_LOWER_SUB,
      m_listStartPaneForLowerSub,  // @fix NMCat[2101] ポケモンナンバー上部に上のアイコンの影が被って表示される  ←ペインの並び順を変更できるように、リストの親ペインを渡す。
      m_paneListCallBackForLowerSub,
      menuCursor,
      m_param.gameManager->GetUiDeviceManager(),
      &SCROLL_BAR_CONST_INFO_FOR_LOWER_SUB,
      valMax,
      m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos].listPos,
      m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos].listScroll,
      true
  );
  
  m_paneListViewForLowerSub->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ
  m_paneListViewForLowerSub->GetPaneList()->SetInputEnabled(false);  // PaneListはUIViewの入力禁止だけでは防げない
}
void ZukanViewSearch::destroyPaneListForLowerSub(void)
{
  if( m_paneListViewForLowerSub )
  {
    ZukanUtil::DestroyPaneList(this, m_paneListViewForLowerSub);
    m_paneListViewForLowerSub = NULL;

    GFL_SAFE_DELETE(m_paneListCallBackForLowerSub);
    GFL_SAFE_DELETE(m_paneListViewListenerForLowerSub);
  
    app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
    menuCursor->SetVisible(false);
  }
}

void ZukanViewSearch::createMesMenuViewItems(void)
{
  {
    m_mesMenuViewEventHandler = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSearchMessageMenuEventHandler(this);
  
    m_mesMenuViewStrBufWindowMessage = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf(STR_BUF_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap());
    m_mesMenuViewStrBufYes           = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf(STR_BUF_SHORT_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap());
    m_mesMenuViewStrBufNo            = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf(STR_BUF_SHORT_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap());

    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_mesMenuViewStrBufWindowMessage;
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_search_10_03;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_mesMenuViewStrBufYes;
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_16_02;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_mesMenuViewStrBufNo;
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_16_03;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
  }
}

void ZukanViewSearch::destroyMesMenuViewItems(void)
{
  {
    GFL_SAFE_DELETE(m_mesMenuViewStrBufWindowMessage);
    GFL_SAFE_DELETE(m_mesMenuViewStrBufYes          );
    GFL_SAFE_DELETE(m_mesMenuViewStrBufNo           );
    GFL_SAFE_DELETE(m_mesMenuViewEventHandler);
  }
}

void ZukanViewSearch::startInSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  // この時点では(m_lowerIsSub == false)が真なので、LowerMainのほうだけ行う。
  m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_SEARCH_ZUKAN_SRC_UPP_000_IN;
  m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_SEARCH_ZUKAN_MIN_LOW_006_IN;

  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_IN);

  if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
  {
    pFadeManager->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS);
  }

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewSearchListener::INPUT_STATE_OCCUPY);
}
void ZukanViewSearch::startOutSeq1(bool isBack)
{
  // この時点では(m_lowerIsSub == false)が真なので、LowerMainのほうだけ行う。
  bool canStartOut = false;
  if(isBack)
  {
    // 検索中でも終了できる
    canStartOut = true;
  }
  else
  {
    // 検索中は終了できない(ボタンを押せないようにしているが念のためここでもチェック)
    if(!this->isSearchSubjectPaneListPosInfoDifferent())
    {
      // スレッド  クリティカルセクション
      gfl2::thread::ScopedLock reqListScopedLock(*m_searchCriticalSection);
      if(m_searchIsFinished)
      {
        if(m_dispPokeDataIndexNum >= 1)
        {
          // ここで変更したリストを反映しておく
          {
            m_param.contents->SetDispPokeDataIndexListMadeBySearch(
                m_searchSubjectPaneListPosInfoOnDisplay,
                m_drawFigureDataIndexList,
                m_dispPokeDataIndexNum,
                m_dispPokeDataIndexList
            );
          }
          canStartOut = true;
        }
      }
    }
  }

  if(canStartOut)
  {
    *m_param.nextSceneType = ZukanType::SCENE_TYPE_ALOLA_TOP;
    
    // ポケアイコン表示
    m_pokeIconDisplay->RequestPause();

    // スレッド
    this->requestSearchThreadEnd();
    
    app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
    menuCursor->SetVisible(false);
 
    this->changeDispState(DISP_STATE_ON_WAIT_START_OUT);
    
    // 占有するが入力受け付けなし
    m_viewListener->SetInputState(ZukanViewSearchListener::INPUT_STATE_OCCUPY);
    
    m_paneListViewForLowerMain->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ
    m_paneListViewForLowerMain->GetPaneList()->SetInputEnabled(false);  // PaneListはUIViewの入力禁止だけでは防げない

    // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →検索画面をYで決定してきたときは、カーソルの位置が元々指していたところから変わる。
    if(!isBack)
    {
      *m_param.alolaTopSceneChangeFlag = true;  // 「指しているポケモンが変わる＝アローラ図鑑のカーソル位置が変わる」ときはこのフラグを立てておく
    }
  }  
}
void ZukanViewSearch::startOutSeq2(void)
{
  // この時点では(m_lowerIsSub == false)が真なので、LowerMainのほうだけ行う。
  switch(*m_param.nextSceneType)
  {
  case ZukanType::SCENE_TYPE_ALOLA_TOP:
  default:
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_SEARCH_ZUKAN_SRC_UPP_000_RETURN;
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_SEARCH_ZUKAN_MIN_LOW_006_RETURN;
    break;
  }

  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_OUT);

  m_scrollBarTopPaneForLowerMain->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
}

void ZukanViewSearch::startLowerChangeSeq1(u32 pos)  // Reqから呼ぶので軽い処理
{
  ////////////////
  // 前の画面
  ////////////////
  if(m_lowerIsSub)
  {
    if(pos != CANCEL_POS)
    {
      // 決定されたので、サブ画面の今の状態を反映しておく。
      m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos].pos = pos;
      m_paneListViewForLowerSub->GetPaneList()->GetCursorData(
          &(m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos].listPos),
          &(m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos].listScroll)
      );  // カーソル位置を覚えておく
    }
    else
    {
      // キャンセルされたので、サブ画面に入ったときの状態に戻しておく。
      m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos] = m_paneListPosInfoForLowerSubTmp;
    }

    // ポケアイコン表示
    this->cancelPokeIconDisplayIfChanged();
    
    m_paneListViewForLowerSub->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ
    m_paneListViewForLowerSub->GetPaneList()->SetInputEnabled(false);  // PaneListはUIViewの入力禁止だけでは防げない
  }
  else
  {
    m_paneListPosInfoForLowerMain.pos = pos;
    m_paneListViewForLowerMain->GetPaneList()->GetCursorData( &m_paneListPosInfoForLowerMain.listPos, &m_paneListPosInfoForLowerMain.listScroll );  // カーソル位置を覚えておく
      
    // サブ画面に入ったときの状態を覚えておく。
    m_paneListPosInfoForLowerSubTmp = m_searchSubjectPaneListPosInfoOnDisplay[m_paneListPosInfoForLowerMain.pos];
  
    m_paneListViewForLowerMain->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ
    m_paneListViewForLowerMain->GetPaneList()->SetInputEnabled(false);  // PaneListはUIViewの入力禁止だけでは防げない
  }

  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(false);

  this->changeDispState(DISP_STATE_ON_LOWER_START_CHANGE);

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewSearchListener::INPUT_STATE_OCCUPY);

  // 画面切り替えはstartLowerChangeSeq2で行う
}
void ZukanViewSearch::startLowerChangeSeq2(void)  // Updateから呼ぶので重い処理
{
  ////////////////
  // 前の画面
  ////////////////
  // ボタンマネージャ
  this->destroyButtonManager();

  if(m_lowerIsSub)
  {
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_SEARCH_ZUKAN_MIN_LOW_006_OUT_SWITCHING;
    m_scrollBarTopPaneForLowerSub->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
    this->destroyPaneListForLowerSub();  // 前の画面  // @fix GFCHECK[8400] 次の画面の生成前に前の画面のペインリストを破棄しておく。
  }
  else
  {
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_SEARCH_ZUKAN_MIN_LOW_006_IN_SWITCHING;
    m_scrollBarTopPaneForLowerMain->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
    this->destroyPaneListForLowerMain();  // 前の画面  // @fix GFCHECK[8400] 次の画面の生成前に前の画面のペインリストを破棄しておく。
  }

  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_LOWER_CHANGING);

  ////////////////
  // 前→次
  ////////////////
  // 画面切り替え
  m_lowerIsSub = !m_lowerIsSub;

  ////////////////
  // 次の画面
  ////////////////
  // ボタンマネージャ
  this->createButtonManager();

  if(m_lowerIsSub)
  {
    this->createPageForLowerSub();

    // ペインリストに使われているペインに値を設定したいので一度つくる。すぐ破棄する。
    // ペインリスト
    this->createPaneListForLowerSub();
    this->destroyPaneListForLowerSub();
    m_scrollBarTopPaneForLowerSub->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
  }
  else
  {
    // ペインリストに使われているペインに値を設定したいので一度つくる。すぐ破棄する。
    // ペインリスト
    this->createPaneListForLowerMain();
    this->destroyPaneListForLowerMain();
    m_scrollBarTopPaneForLowerMain->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
  }
}

void ZukanViewSearch::startConfirm1(void)  // Reqから呼ぶので軽い処理
{
  // ここでボタンマネージャとペインリストへの入力を切っておかなければならない。

  this->changeDispState(DISP_STATE_ON_START_CONFIRM);
  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewSearchListener::INPUT_STATE_OCCUPY);
}
void ZukanViewSearch::startConfirm2(void)  // Updateから呼ぶので重い処理
{
  App::Tool::MessageMenuView* mesMenuView = m_param.tool->GetMessageMenuView();
  mesMenuView->SetEventHandler(m_mesMenuViewEventHandler);
  mesMenuView->SetYNListMode(true);

  mesMenuView->SetModal(true);    // 親であるapp::ui::UIViewのメンバ
  this->AddSubView(mesMenuView);  // 親であるapp::ui::UIViewのメンバ

  mesMenuView->ShowMessageWithListMenu(
      m_mesMenuViewStrBufWindowMessage,
      false,
      m_mesMenuViewStrBufYes,
      m_mesMenuViewStrBufNo
  );

  this->changeDispState(DISP_STATE_ON_WAIT_CONFIRM);
}

void ZukanViewSearch::endConfirm1(bool isYes)  // Reqから呼ぶので軽い処理
{
  m_mesMenuViewResultIsYes = isYes;
  this->changeDispState(DISP_STATE_ON_END_CONFIRM);
}
void ZukanViewSearch::endConfirm2(bool isYes)  // Updateから呼ぶので重い処理
{
  App::Tool::MessageMenuView* mesMenuView = m_param.tool->GetMessageMenuView();
  mesMenuView->RemoveFromSuperView();
  mesMenuView->SetEventHandler(NULL);

  if(isYes)
  {
    this->startOutSeq1(true);
  }
  else
  {
    // ここでボタンマネージャとペインリストへの入力を復活させておかなければならない
    
    this->changeDispState(DISP_STATE_ON_WORK);
    m_viewListener->SetInputState(ZukanViewSearchListener::INPUT_STATE_ENABLE);
  }
}

void ZukanViewSearch::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewSearch::createPageOnce(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_1  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  // よくアクセスするペインのアドレスを記憶しておく
  m_resetButtonPartsPane           = lytWk_1->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_006_PANE_BTN_X);
  m_decideButtonPartsPane          = lytWk_1->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_006_PANE_BTN_Y);
  m_subjectNameTextPaneForLowerSub = lytWk_1->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_006_PANE_TEXTBOX_S_00);

  m_scrollBarTopPaneForLowerMain = lytWk_1->GetPane(PANENAME_ZUKAN_MIN_LOW_006_PANE_SCROLL_S_PROG);
  m_scrollBarTopPaneForLowerSub  = lytWk_1->GetPane(PANENAME_ZUKAN_MIN_LOW_006_PANE_SCROLL_S_00_PROG);

  m_listStartPaneForLowerMain = lytWk_1->GetPane(PANENAME_ZUKAN_MIN_LOW_006_PANE_LIST_NULL_00);
  m_listStartPaneForLowerSub  = lytWk_1->GetPane(PANENAME_ZUKAN_MIN_LOW_006_PANE_LIST_NULL_01);

  // テキスト
  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    // リセットボタン
    gfl2::lyt::LytTextBox* resetButtonTextPane = lytWk_1->GetTextBoxPane(m_resetButtonPartsPane, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(resetButtonTextPane, msg_zkn_search_10_01);
    // けっていボタン
    gfl2::lyt::LytTextBox* decideButtonTextPane = lytWk_1->GetTextBoxPane(m_decideButtonPartsPane, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(decideButtonTextPane, msg_zkn_search_10_02);
  }
  pG2DUtil->SetMessageData(NULL, NULL);
}

void ZukanViewSearch::createPageForLowerSub(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    pG2DUtil->SetTextBoxPaneString(m_subjectNameTextPaneForLowerSub, getSubjectMsgId(static_cast<ZukanType::Subject>(m_paneListPosInfoForLowerMain.pos)));
  }
  pG2DUtil->SetMessageData(NULL, NULL);
}

void ZukanViewSearch::setItem(gfl2::lyt::LytParts* itemPartsPane, u32 itemAnim, u32 styleAnim, ZukanType::Subject subject, u32 pos)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk    = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  gfl2::lyt::LytTextBox*  itemTextPane          = lytWk->GetTextBoxPane(itemPartsPane, PANENAME_ZUKAN_ICN_LOW_001_PANE_TEXTBOX, &lytRes_0);
  gfl2::lyt::LytParts*    itemTypeIconPartsPane = lytWk->GetPartsPane(itemPartsPane, PANENAME_ZUKAN_ICN_LOW_001_PANE_WAZA_TYPE, &lytRes_0);
  gfl2::lyt::LytPicture*  itemTypeIconPicPane   = lytWk->GetPicturePane(itemTypeIconPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &lytRes_0);


  gfl2::str::MsgData* msgDataZukan        = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN);
  gfl2::str::MsgData* msgDataInitial      = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_INITIAL);
  
  switch(subject)
  {
  case ZukanType::SUBJECT_ORDER    :
  case ZukanType::SUBJECT_INITIAL  :
  case ZukanType::SUBJECT_COLOR    :
  case ZukanType::SUBJECT_FIGURE   :
  case ZukanType::SUBJECT_GET      :
    {
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, itemAnim, 0);
      switch(subject)
      {
      case ZukanType::SUBJECT_ORDER    :
        {
          pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
          {
            pG2DUtil->SetTextBoxPaneString(itemTextPane, getOrderMsgId(ZukanType::GetOrderTypeFromPos(pos)));
          }
          pG2DUtil->SetMessageData(NULL, NULL);
          break;
        }
      case ZukanType::SUBJECT_INITIAL  :
        {
          ZukanType::InitialType initialType = ZukanType::GetInitialTypeFromPos(pos, m_gameLang);
          if(initialType == ZukanType::INITIAL_TYPE_NONE)
          {
            pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, msg_zkn_search_001);
            }
            pG2DUtil->SetMessageData(NULL, NULL);
          }
          else
          {
            pG2DUtil->SetMessageData( msgDataInitial, m_param.tool->GetWordSet() );
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, getInitialMsgId(initialType, m_gameLang));
            }
            pG2DUtil->SetMessageData(NULL, NULL);
          }
          break;
        }
      case ZukanType::SUBJECT_COLOR    :
        {
          pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
          {
            ZukanType::ColorType colorType = ZukanType::GetColorTypeFromPos(pos);
            if(colorType == ZukanType::COLOR_TYPE_NONE)
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, msg_zkn_search_003);
            }
            else
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, getColorMsgId(colorType));
            }
          }
          pG2DUtil->SetMessageData(NULL, NULL);
          break;
        }
      case ZukanType::SUBJECT_FIGURE   :
        {
          pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
          {
            ZukanType::FigureType figureType = ZukanType::GetFigureTypeFromPos(pos);
            if(figureType == ZukanType::FIGURE_TYPE_NONE)
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, msg_zkn_search_003);
            }
            else
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, getFigureMsgId(figureType));
            }
          }
          pG2DUtil->SetMessageData(NULL, NULL);
          break;
        }
      case ZukanType::SUBJECT_GET      :
        {
          pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
          {
            ZukanType::GetType getType = ZukanType::GetGetTypeFromPos(pos);
            if(getType == ZukanType::GET_TYPE_NONE)
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, msg_zkn_search_003);
            }
            else
            {
              pG2DUtil->SetTextBoxPaneString(itemTextPane, getGetMsgId(getType));
            }
          }
          pG2DUtil->SetMessageData(NULL, NULL);
          break;
        }
      }
      break;
    } 
  case ZukanType::SUBJECT_POKETYPE1:
  case ZukanType::SUBJECT_POKETYPE2:
    {
      ZukanType::PoketypeType poketypeType = ZukanType::GetPoketypeTypeFromPos(pos);
      if(poketypeType == ZukanType::POKETYPE_TYPE_NONE)
      {
        ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, itemAnim, 0);
        pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
        {
          pG2DUtil->SetTextBoxPaneString(itemTextPane, msg_zkn_search_002);
        }
      }
      else
      {
        ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, itemAnim, 2);
        m_param.tool->ReplaceTypeIconTextureByCommonGrpIcon(
            itemTypeIconPicPane, static_cast<PokeType_tag>(poketypeType)
        );
      }
      break;
    }
  case ZukanType::SUBJECT_STYLE    :
    {
      ZukanType::StyleType styleType = ZukanType::GetStyleTypeFromPos(pos);
      if(styleType == ZukanType::STYLE_TYPE_NONE)
      {
        ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, itemAnim, 0);
        pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
        {
          pG2DUtil->SetTextBoxPaneString(itemTextPane, msg_zkn_search_003);
        }
        pG2DUtil->SetMessageData(NULL, NULL);
      }
      else
      {
        ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, itemAnim, 1);
        ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, styleAnim, this->getStyleTexPatternIndex(styleType));
      }
      break;
    }
 }
}

void ZukanViewSearch::rewriteListElemsForLowerMain(void)
{
  for(u32 pane_index=0; pane_index<PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN; ++pane_index)
  {
    u32 pos = m_paneIndexPosForLowerMain[pane_index];
    if(pos != ZUKAN_CONST_PANE_LIST_PANE_INDEX_NULL)
    {
      this->writeListElemForLowerMain(pane_index, pos);
    }
  }
}

void ZukanViewSearch::writeListElemForLowerMain(u32 pane_index, u32 pos)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk    = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  gfl2::lyt::LytParts*   paneListPartsPane = static_cast<gfl2::lyt::LytParts*>(m_paneListDataForLowerMain[pane_index].base);
  gfl2::lyt::LytTextBox* subjectTextPane   = lytWk->GetTextBoxPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_008_PANE_TEXTBOX, &lytRes_0);
  gfl2::lyt::LytParts*   itemPartsPane     = lytWk->GetPartsPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_010_PANE_ITEM, &lytRes_0);
  
  // 題目
  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    pG2DUtil->SetTextBoxPaneString(subjectTextPane, getSubjectMsgId(static_cast<ZukanType::Subject>(pos)));
  }
  pG2DUtil->SetMessageData(NULL, NULL);

  // 現在の値
  this->setItem(
      itemPartsPane,
      PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_MAIN[pane_index].other1Anime,
      PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_MAIN[pane_index].other2Anime,
      static_cast<ZukanType::Subject>(pos),
      m_searchSubjectPaneListPosInfoOnDisplay[pos].pos
  );
}




// ポケアイコン表示
void ZukanViewSearch::cancelPokeIconDisplayIfChanged(void)
{
  if( this->isSearchSubjectPaneListPosInfoDifferent() )
  {
    // ポケアイコン表示
    m_pokeIconDisplay->CancelDisplay();
    m_startPokeIconDisplay = false;
  }
}


// 決定ボタンの押せる/押せない 
void ZukanViewSearch::setDecideButtonEnable(void)  // 現在の状態で決定ボタンの押せる/押せないを変更する
{
  if(!m_lowerIsSub)
  {
    // 「検索中」や「検索結果が1個もないとき」はボタンを押せないようにしておく
    bool inputEnable = false;

    // @fix NMCat[1759] 決定不可の「けってい」ボタンが一瞬、反応する
    //      検索条件を変更した瞬間に変わるフラグはm_startPokeIconDisplayである。
    //      m_searchIsFinishedやm_dispPokeDataIndexNumは前の検索条件の結果なので、
    //      最新の検索条件で判定するにはm_startPokeIconDisplayも確認しなければならない。
    if(m_startPokeIconDisplay)
    { 
      if(m_searchIsFinished)
      {
        if(m_dispPokeDataIndexNum >= 1)
        {
          inputEnable = true;
        }
      }
    }

    app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
    if(inputEnable)
    {
      buttonManager->SetButtonPassive(BUTTON_ID_DECIDE);  // ボタンの内部状態を必ず変更させるために一度offにしてからonにする
      buttonManager->SetButtonActive(BUTTON_ID_DECIDE);   
    }
    else
    {
      buttonManager->SetButtonActive(BUTTON_ID_DECIDE);   // ボタンの内部状態を必ず変更させるために一度onにしてからoffにする 
      buttonManager->SetButtonPassive(BUTTON_ID_DECIDE);
    }
  }
  // サブ画面のときは何もしない
}


// 検索開始
void ZukanViewSearch::startSearch(void)
{
  // スレッド  クリティカルセクション
  gfl2::thread::ScopedLock reqListScopedLock(*m_searchCriticalSection);
  if(m_searchIsFinished)
  {
    for(u32 i=0; i<ZukanType::SUBJECT_NUM; ++i)
    {
      m_searchSubjectPaneListPosInfoOnSearching[i] = m_searchSubjectPaneListPosInfoOnDisplay[i];
    }
    m_searchIsFinished = false;
    // 検索開始
    // スレッド
    this->requestSearchThreadSearch();

    // 決定ボタンを押せなくする
    this->setDecideButtonEnable();
  }
}


void ZukanViewSearch::setSearchSubjectPaneListPosInfoOnStarting(void)
{
  // ここで最初のリストを反映しておく
  const ZukanType::PaneListPosInfo* paneListPosInfo = m_param.contents->GetPaneListPosInfoOfSearchSubject();
  for(u32 i=0; i<ZukanType::SUBJECT_NUM; ++i)
  {
    m_searchSubjectPaneListPosInfoOnDisplay[i]   = paneListPosInfo[i];
    m_searchSubjectPaneListPosInfoOnSearching[i] = paneListPosInfo[i];
  }

  m_drawFigureDataIndexList = GFL_NEW_ARRAY(m_param.appHeap->GetDeviceHeap()) u8[m_param.contents->GetPokeDataNumMax()];
  m_dispPokeDataIndexNum    = m_param.contents->GetDispPokeDataIndexNum();
  m_dispPokeDataIndexList   = GFL_NEW_ARRAY(m_param.appHeap->GetDeviceHeap()) u16[m_param.contents->GetPokeDataNumMax()];
 
  gfl2::std::MemClear( m_drawFigureDataIndexList, sizeof(u8)*m_param.contents->GetPokeDataNumMax() );
  gfl2::std::MemClear( m_dispPokeDataIndexList, sizeof(u16)*m_param.contents->GetPokeDataNumMax() );
  
  for(u32 i=0; i<m_dispPokeDataIndexNum; ++i)  // 使うのはm_dispPokeDataIndexNumまでだから、m_param.contents->GetPokeDataNumMax()にはしなかった。
  {
    m_dispPokeDataIndexList[i] = m_param.contents->GetDispPokeDataIndexList()[i];
  }
  for(u32 i=0; i<m_dispPokeDataIndexNum; ++i)
  {
    // @fix NMCat[955] ポケモン図鑑で検索するとフォルムが「アローラのすがた」になる
    // m_drawFigureDataIndexListはm_pokeDataListと同じ順に並んでいる。
    m_drawFigureDataIndexList[m_dispPokeDataIndexList[i]] = m_param.contents->GetDrawFigureDataIndexList()[m_dispPokeDataIndexList[i]];
  }

  {
    // スレッド  クリティカルセクション
    //gfl2::thread::ScopedLock reqListScopedLock(*m_searchCriticalSection);  // この時点ではまだスレッドは生成されていないのでコメントアウト。
    m_searchIsFinished = true;
  }
}
void ZukanViewSearch::resetSearchSetting(void)
{
  ZukanType::ResetSubjectPaneListPosInfo(m_searchSubjectPaneListPosInfoOnDisplay);

  // ポケアイコン表示
  this->cancelPokeIconDisplayIfChanged();

  this->rewriteListElemsForLowerMain();
}
bool ZukanViewSearch::isSearchSubjectPaneListPosInfoDifferent(void) const
{
  bool isDifferent = false;
  for(u32 i=0; i<ZukanType::SUBJECT_NUM; ++i)
  {
    if(m_searchSubjectPaneListPosInfoOnDisplay[i].pos != m_searchSubjectPaneListPosInfoOnSearching[i].pos)
    {
      isDifferent = true;
      break;
    }
  }
  return isDifferent;
}
bool ZukanViewSearch::isSearchSubjectPaneListPosInfoChanged(void) const
{
  bool isChanged = false;
  const ZukanType::PaneListPosInfo* paneListPosInfo = m_param.contents->GetPaneListPosInfoOfSearchSubject();
  for(u32 i=0; i<ZukanType::SUBJECT_NUM; ++i)
  {
    if(m_searchSubjectPaneListPosInfoOnDisplay[i].pos != paneListPosInfo[i].pos)
    {
      isChanged = true;
      break;
    }
  }
  return isChanged;
}



// スレッド
void ZukanViewSearch::requestSearchThreadSearch(void)
{
  // スレッド  イベント
  m_searchEvent->Signal();
}
void ZukanViewSearch::requestSearchThreadEnd(void)
{
  m_searchThread->RequestEnd();
  // スレッド  イベント
  m_searchEvent->Signal();
}




GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

