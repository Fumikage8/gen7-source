//==============================================================================
/**
 @file    ZukanViewAlolaTop.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑のアローラ図鑑トップ画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>
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
#include <AppLib/include/Ui/UIViewManager.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/alola_top.h>
#include <niji_conv_header/app/zukan/alola_top_pane.h>
#include <niji_conv_header/app/zukan/alola_top_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"
#include "ZukanDataManager.h"
#include "ZukanPokeIconTextureManager.h"
#include "ZukanTool.h"
#include "ZukanPicScrollDataAccessor.h"
#include "ZukanPicScrollBook.h"
#include "ZukanFrameAlolaTop.h"

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
#include "ZukanPrismSheet.h"
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

#include "ZukanViewAlolaTop.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewAlolaTopListener
 @brief     図鑑のアローラ図鑑トップ画面のビューのリスナー
 */
//==============================================================================
class ZukanViewAlolaTopListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewAlolaTopListener);

public:
  ZukanViewAlolaTopListener(ZukanViewAlolaTop* view);
  virtual ~ZukanViewAlolaTopListener();

  //virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  //virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
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

private:
  ZukanViewAlolaTop* m_view;
  InputState         m_inputState;
};

//==============================================================================
//
//  ZukanViewAlolaTopListener
//
//==============================================================================
ZukanViewAlolaTopListener::ZukanViewAlolaTopListener(ZukanViewAlolaTop* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewAlolaTopListener::~ZukanViewAlolaTopListener()
{}

/*
app::ui::UIInputListener::ListenerResult ZukanViewAlolaTopListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_view->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )  // ボタンマネージャ動作中はタッチ入力させない
  {
    if( pTouchPanel->IsTouchTrigger() )
    {
      Sound::PlaySE( SEQ_SE_RELEASE );
      m_view->Req(ZukanViewAlolaTop::REQ_ID_KEY_DECIDE);
    }
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult ZukanViewAlolaTopListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_view->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )  // ボタンマネージャ動作中はキー入力させない
  {
    if( pButton->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      Sound::PlaySE( SEQ_SE_DECIDE1 );
      m_view->Req(ZukanViewAlolaTop::REQ_ID_KEY_DECIDE);
    }
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
*/
app::ui::UIInputListener::ListenerResult ZukanViewAlolaTopListener::OnLayoutPaneEvent( const u32 painId )
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
      switch(button_id)
      {
      case ZukanViewAlolaTop::BUTTON_ID_SEARCH      : m_view->Req(ZukanViewAlolaTop::REQ_ID_BUTTON_DECIDE, ZukanViewAlolaTop::BUTTON_ID_SEARCH      ); break;
      case ZukanViewAlolaTop::BUTTON_ID_BACK        : m_view->Req(ZukanViewAlolaTop::REQ_ID_BUTTON_DECIDE, ZukanViewAlolaTop::BUTTON_ID_BACK        ); break;
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
void ZukanViewAlolaTopListener::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
  switch(m_inputState)
  {
  case INPUT_STATE_ENABLE:
    {
      u32 button_id = paneId;
      switch(button_id)
      {
      case ZukanViewAlolaTop::BUTTON_ID_SEARCH      : m_view->Req(ZukanViewAlolaTop::REQ_ID_BUTTON_TOUCH, ZukanViewAlolaTop::BUTTON_ID_SEARCH      ); break;
      case ZukanViewAlolaTop::BUTTON_ID_BACK        : m_view->Req(ZukanViewAlolaTop::REQ_ID_BUTTON_TOUCH, ZukanViewAlolaTop::BUTTON_ID_BACK        ); break;
      }
      break;
    }
  }
}

void ZukanViewAlolaTopListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
  
  app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
}




//==============================================================================
/**
 @class     ZukanAlolaTopPaneListViewListener
 @brief     図鑑のアローラ図鑑トップ画面のペインリストビューのリスナー
 */
//==============================================================================
class ZukanAlolaTopPaneListViewListener : public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanAlolaTopPaneListViewListener);

public:
  ZukanAlolaTopPaneListViewListener(ZukanViewAlolaTop* view);
  virtual ~ZukanAlolaTopPaneListViewListener();

  virtual void PaneListItemSelectListener( u32 pos );
  //virtual void PaneListItemCatchListener( u32 pos );

private:
  ZukanViewAlolaTop* m_view;
};


//==============================================================================
//
//  ZukanViewAlolaTopListener
//
//==============================================================================
ZukanAlolaTopPaneListViewListener::ZukanAlolaTopPaneListViewListener(ZukanViewAlolaTop* view)
  : app::tool::PaneListView::Listener(),
    m_view(view)
{}
ZukanAlolaTopPaneListViewListener::~ZukanAlolaTopPaneListViewListener()
{}

void ZukanAlolaTopPaneListViewListener::PaneListItemSelectListener( u32 pos )
{
  m_view->Req( ZukanViewAlolaTop::REQ_ID_PANE_LIST_DECIDE, static_cast<s32>(pos) );
}
//void ZukanAlolaTopPaneListViewListener::PaneListItemCatchListener( u32 /*pos*/ )
//{}




//==============================================================================
/**
 @class     ZukanAlolaTopPaneListCallBack
 @brief     図鑑のアローラ図鑑トップ画面のペインリストのコールバック
 */
//==============================================================================
class ZukanAlolaTopPaneListCallBack : public app::tool::PaneList::CallBack
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanAlolaTopPaneListCallBack);

public:
  ZukanAlolaTopPaneListCallBack(ZukanViewAlolaTop* view);
  virtual ~ZukanAlolaTopPaneListCallBack();

public:
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 );

private:
  ZukanViewAlolaTop* m_view;
};

//==============================================================================
//
//  ZukanAlolaTopPaneListCallBack
//
//==============================================================================
ZukanAlolaTopPaneListCallBack::ZukanAlolaTopPaneListCallBack(ZukanViewAlolaTop* view)
  : app::tool::PaneList::CallBack(),
    m_view(view)
{}
ZukanAlolaTopPaneListCallBack::~ZukanAlolaTopPaneListCallBack()
{}

void ZukanAlolaTopPaneListCallBack::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  //GFL_PRINT("ZukanAlolaTopPaneListCallBack::CallBack_CursorMove  %d %d\n", pane_index, pos);
  m_view->DoAtCursorMoving( cur_pane, pane_index, pos );
}
void ZukanAlolaTopPaneListCallBack::CallBack_CursorOff( void * /*work*/, const app::tool::PaneList::PANE_DATA * /*pane*/, u32 /*pane_index*/, u32 /*pos*/ )
{}
void ZukanAlolaTopPaneListCallBack::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  //GFL_PRINT("ZukanAlolaTopPaneListCallBack::CallBack_ListWrite  %d %d\n", pane_index, pos);
  m_view->WriteList( pane, pane_index, pos );
}
void ZukanAlolaTopPaneListCallBack::CallBack_ItemChange( void * /*work*/, u32 /*pos1*/, u32 /*pos2*/ )
{}




//==============================================================================
//
//  ZukanViewAlolaTop
//
//==============================================================================
ZukanViewAlolaTop::ZukanViewAlolaTop(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL),
    m_orderTypeIsForId(false),
    m_buttonPaneInfo(NULL),
    m_paneListView(NULL),
    m_paneListViewListener(NULL),
    m_paneListCallBack(NULL),
    m_paneListElemNum(0),
    m_paneListData(NULL),
    m_searchTopPane(NULL),
    m_searchOrderTextPane(NULL),
    m_searchCountTextPane(NULL),
    m_scrollBarTopPane(NULL),
    m_listStartPane(NULL),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_pokeIconTexManager(NULL),
    m_picScrollDataAccessor(NULL),
    m_picScrollBook(NULL)
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    , m_prismSheet(NULL)
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    , m_uiViewManInputExclusiveKey(false)
{
  m_uiViewManInputExclusiveKey = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->GetInputExclusiveKeyByButtonAnimation();
  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetInputExclusiveKeyByButtonAnimation(true);  // @fix NMCat[1463] 【図鑑】アローラ図鑑にて、検索ボタンを押した直後に入力を受け付けている
 

  // 初期化
  {
    for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
    {
      m_waitAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }

    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      m_buttonPaneInfoForId[i] = ZukanType::ButtonPaneInfo();
    }
    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      m_buttonPaneInfoForSpec[i] = ZukanType::ButtonPaneInfo();
    }

    for(u32 i=0; i<PANE_LIST_ELEM_NUM_FOR_ID; ++i)
    {
      m_paneListElemButtonSeIdForId[i]    = app::tool::ButtonManager::SE_NULL;
    }
    for(u32 i=0; i<PANE_LIST_ELEM_NUM_FOR_SPEC; ++i)
    {
      m_paneListElemButtonSeIdForSpec[i]  = app::tool::ButtonManager::SE_NULL;
    }

    // 構造体の配列の初期化をしない。コメントアウトしておく。
    //for(u32 i=0; i<PANE_LIST_ELEM_NUM_FOR_ID; ++i)
    //{
    //  m_paneListDataForId[i] = ;
    //}
    //for(u32 i=0; i<PANE_LIST_ELEM_NUM_FOR_ID; ++i)
    //{
    //  m_paneListDataForSpec[i] = ;
    //}
  }

  // ポケアイコン
  m_pokeIconTexManager = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPokeIconTextureManager(
      m_param.appHeap, m_param.dataManager, PANE_LIST_ELEM_NUM_MAX );

  // ZukanViewAlolaTopListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewAlolaTopListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  // 判定
  {
    if(    m_param.contents->GetOrderType() == ZukanType::ORDER_TYPE_NO
        || m_param.contents->GetOrderType() == ZukanType::ORDER_TYPE_NAME )
    {
      m_orderTypeIsForId = true;
    }

    /*
    @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →検索画面をBでキャンセルしてきたときは、カーソル位置はそのままにしたいので、コメントアウトしておく。
    // セーブデータが覚えているカーソル位置のリセット
    if(m_param.prevSceneType == ZukanType::SCENE_TYPE_SEARCH)
    {
      m_param.contents->ResetDispPokeDataOfSaveData();
      // 何も変更せずに( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )がtrueの状態で
      // 検索から戻ってきたときはリセットしないほうがいいかもしれない。
    }
    */

    /*
    カーソル位置の記憶に関するメモ書き

    ■アローラ図鑑（デフォルト状態、検索結果状態どちらも）のカーソル

    起動画面→アローラ図鑑
    　最後に見ていた図鑑がアローラ図鑑のとき、最後に指していたポケモンを指す。見ていないポケモン(名前非表示)も指せる。検索結果状態が変わっていても、最後に指していたポケモンを指す。
    　最後に見ていた図鑑が違うとき、および、初めて図鑑を起動したとき、見ているポケモンで一番若い番号のものを指す。

    情報画面→アローラ図鑑
    　情報画面で最後に見ていたポケモンを指す。

    検索画面→アローラ図鑑
    　Bでキャンセルしてきたとき、最後に指していたところを指す。
    　Yで決定してきたとき、先頭を指す。ただし、決定した状態がデフォルト状態のときは、見ているポケモンで一番若い番号のものを指す。


    ■島図鑑のカーソル

    起動画面→島X図鑑
    　最後に見ていた図鑑が島X図鑑のとき、最後に指していたポケモンを指す。見ていないポケモン(名前非表示)も指せる。
    　最後に見ていた図鑑が違うとき、および、初めて図鑑を起動したとき、見ているポケモンで一番若い番号のものを指す。

    情報画面→島X図鑑
    　情報画面で最後に見ていたポケモンを指す。

    以上。
    */

    // カーソル位置(セーブデータが覚えているカーソル位置も合わせて修正)
    bool cursorSet = false; 
    if(m_param.prevSceneType == ZukanType::SCENE_TYPE_LAUNCH)
    {
      u32 indexOfDispPokeDataIndexList = 0;
      const ZukanContents::PokeData* pokeData = m_param.contents->GetDispPokeDataFromSaveData(&indexOfDispPokeDataIndexList);

      // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →アローラ図鑑のカーソル判定フラグを追加。
      if(*m_param.launchAndAlolaTopSceneDefaultFlag)
      {
        //////// 最後に見ていた図鑑がアローラ図鑑のとき、かつ、最後に見ていたアローラ図鑑の並びがデフォルト状態(検索リセット状態)だったとき ////////
        
        // *m_param.alolaTopScenePaneListSetupParamListPos
        // *m_param.alolaTopScenePaneListSetupParamListScroll
        // はそのまま使う。
        // m_param.contents->GetCurrIndexOfDispPokeDataIndexList(),
        // はリストがつくり直されているので求め直す（同じリストをつくっているので、求め直しても最後に見ていたときと同じ値になる）。
        
        if(pokeData)
        {
          m_param.contents->SetCurrIndexOfDispPokeDataIndexList(indexOfDispPokeDataIndexList);
          cursorSet = true;
        }
      }
      else
      {
        //////// 最後に見ていた図鑑がアローラ図鑑ではないとき、または、初めて図鑑を起動したとき、または、最後に見ていた図鑑がアローラ図鑑だが並びがデフォルト状態(検索リセット状態)でなかったとき ////////
        
        if(pokeData)  // 「最後に見ていた図鑑が違うとき、および、初めて図鑑を起動したとき」はNULLなので、ここでは何もせずこれより後のブロックに任せる。
        {
          *m_param.alolaTopSceneChangeFlag = true;  // アローラ図鑑のカーソル、リスト関連の位置計算を行うためにtrueにする。
          m_param.contents->SetCurrIndexOfDispPokeDataIndexList(indexOfDispPokeDataIndexList);
          cursorSet = true;
        }
      }
    }
    else if(m_param.prevSceneType == ZukanType::SCENE_TYPE_INFO)
    {
      if((*m_param.alolaTopSceneChangeFlag) == false)
      {
        // *m_param.alolaTopScenePaneListSetupParamListPos
        // *m_param.alolaTopScenePaneListSetupParamListScroll
        // m_param.contents->GetCurrIndexOfDispPokeDataIndexList(),
        // をそのまま使う。
        cursorSet = true;
      }
      else
      {
        // *m_param.alolaTopScenePaneListSetupParamListPos
        // *m_param.alolaTopScenePaneListSetupParamListScroll
        // は求めなおし、
        // m_param.contents->GetCurrIndexOfDispPokeDataIndexList(),
        // はそのまま使う。
        cursorSet = true;
      }
    }
    else if(m_param.prevSceneType == ZukanType::SCENE_TYPE_SEARCH)
    {
      // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →検索画面をBでキャンセルしてきたとき、最後に指していたところを指す。
      if((*m_param.alolaTopSceneChangeFlag) == false)
      {
        // *m_param.alolaTopScenePaneListSetupParamListPos
        // *m_param.alolaTopScenePaneListSetupParamListScroll
        // m_param.contents->GetCurrIndexOfDispPokeDataIndexList(),
        // をそのまま使う。
        cursorSet = true;
      }
    }

    if(!cursorSet)
    {
      if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
      {
        u32 dispIndexNum = m_param.contents->GetDispPokeDataIndexNum();
        for(u32 dispIndex=0; dispIndex<dispIndexNum; ++dispIndex)
        {
          const ZukanContents::PokeData* dispPokeData = m_param.contents->GetDispPokeData(dispIndex);
          if( dispPokeData->figureSeeNum > 0 )  // 見たポケモンにカーソルを合わせておく
          {
            if(dispPokeData->alolaNo > 0)
            {
              *m_param.alolaTopSceneChangeFlag = true;
              m_param.contents->SetCurrIndexOfDispPokeDataIndexList(m_param.contents->GetIndexOfDispPokeDataIndexForAlolaDefault(dispPokeData->alolaNo));
              cursorSet = true;
              break;
            }
          }
        }
      }
    
      if(!cursorSet)  // 全条件から漏れてきたときの最後の受け皿
      {
        *m_param.alolaTopSceneChangeFlag = true;
        m_param.contents->SetCurrIndexOfDispPokeDataIndexList(0);
        // セーブデータが覚えているカーソル位置
        m_param.contents->ResetDispPokeDataOfSaveData();
      }
    }    

    // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →アローラ図鑑（デフォルト状態、検索結果状態どちらも）を立ち上げたときのカーソル位置を記憶しておく
    // セーブデータが覚えているカーソル位置
    m_param.contents->SetDispPokeDataToSaveData(m_param.contents->GetCurrIndexOfDispPokeDataIndexList());
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_ALOLA_TOP);
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
    
      // ID(個体を識別するもの)
      const app::util::G2DUtil::LytwkData wkTbl_orderTypeForId[] =
      {
        // wk_0
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_1
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_2
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },

        // wk_3
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_SRC_UPP_001_BFLYT,
          LA_ALOLA_TOP_ZUKAN_SRC_UPP_001___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },

        // wk_4
        {
          // 下画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_LOW_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_LOW_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },

        // wk_5
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_003_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_003___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_6
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_004_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_004___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        // wk_7
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_PRISM_SHEET_BFLYT,
          LA_ALOLA_TOP_PRISM_SHEET___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
      };

      // SPEC(個体の性能)
      const app::util::G2DUtil::LytwkData wkTbl_orderTypeForSpec[] =
      {
        // wk_0
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_1
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_2
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },

        // wk_3
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_SRC_UPP_001_BFLYT,
          LA_ALOLA_TOP_ZUKAN_SRC_UPP_001___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },

        // wk_4
        {
          // 下画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_LOW_004_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_LOW_004___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },

        // wk_5
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_003_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_003___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_6
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_004_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_004___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        // wk_7
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_PRISM_SHEET_BFLYT,
          LA_ALOLA_TOP_PRISM_SHEET___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
      };

      if(m_orderTypeIsForId)
      {
        this->Create2D(  // 親であるapp::ui::UIViewのメンバ
            m_param.appHeap,
            m_param.lytSys,
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
            8,  // wk_0, wk_1, wk_2, wk_3, wk_4, wk_5, wk_6, wk_7の8個
#else  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABL
            7,  // wk_0, wk_1, wk_2, wk_3, wk_4, wk_5, wk_6の7個
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
            resTbl,
            GFL_NELEMS(resTbl), 
            wkTbl_orderTypeForId,
            GFL_NELEMS(wkTbl_orderTypeForId), 
            app::ui::UIView::UNUSE_MESSAGE_DATA
        );
      }
      else
      {
        this->Create2D(  // 親であるapp::ui::UIViewのメンバ
            m_param.appHeap,
            m_param.lytSys,
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
            8,  // wk_0, wk_1, wk_2, wk_3, wk_4, wk_5, wk_6, wk_7の8個
#else  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABL
            7,  // wk_0, wk_1, wk_2, wk_3, wk_4, wk_5, wk_6の7個
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
            resTbl,
            GFL_NELEMS(resTbl), 
            wkTbl_orderTypeForSpec,
            GFL_NELEMS(wkTbl_orderTypeForSpec), 
            app::ui::UIView::UNUSE_MESSAGE_DATA
        );
      }

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
      // キラカード
      {
        m_prismSheet = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPrismSheet(
          m_param.appHeap,
          this->GetLayoutWork( LYT_WK_ID_UPPER_PRISM_SHEET ),  // wk_?  // 親であるapp::ui::UIViewのメンバ関数
          LYT_WK_ID_UPPER_PRISM_SHEET  // wk_?
        );
      }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

      // 絵巻
      {
        if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
        {
          const void* picScrollDataBuf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_PIC_SCROLL_DATA);
          GFL_ASSERT(picScrollDataBuf);  // 開発中に気付かせるためのASSERT
          m_picScrollDataAccessor = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPicScrollDataAccessor();
          m_picScrollDataAccessor->SetupData(picScrollDataBuf);
         
          m_picScrollBook = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPicScrollBook(
              m_param.appHeap,
              this->GetG2DUtil(),  // 親であるapp::ui::UIViewのメンバ
              this->GetG2DUtil()->GetLayoutResourceID( 0 ),  // res_0
              this->GetLayoutWork( LYT_WK_ID_UPPER_0 ),  // wk_?  // 親であるapp::ui::UIViewのメンバ関数
              this->GetLayoutWork( LYT_WK_ID_UPPER_1 ),  // wk_?  // 親であるapp::ui::UIViewのメンバ関数
              this->GetLayoutWork( LYT_WK_ID_UPPER_2 ),  // wk_?  // 親であるapp::ui::UIViewのメンバ関数
              LYT_WK_ID_UPPER_0,  // wk_?
              LYT_WK_ID_UPPER_1,  // wk_?
              LYT_WK_ID_UPPER_2,  // wk_?
              this->GetLayoutWork( LYT_WK_ID_UPPER_BACKMOST ),  // wk_?  // 親であるapp::ui::UIViewのメンバ関数
              this->GetLayoutWork( LYT_WK_ID_UPPER_CURTAIN ),   // wk_?  // 親であるapp::ui::UIViewのメンバ関数
              LYT_WK_ID_UPPER_BACKMOST,  // wk_?
              LYT_WK_ID_UPPER_CURTAIN,   // wk_?
              m_picScrollDataAccessor,
              m_param.tool->GetPokeExtendData(),
              m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN),
              m_param.tool->GetWordSet(),
              m_param.contents,
              m_param.pokePicManager
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
              , m_prismSheet
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
          );

          // カーソル位置
          {
            const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
            u32 alolaNo = pokeData->alolaNo;
            if(alolaNo > 0)
            {
              m_picScrollBook->SetSheet(alolaNo, true);
            }
          }
        }
        else
        {
          // 絵巻がないときは使わないレイアウトは非表示にしておく
          gfl2::lyt::LytWk* lytWk = this->GetLayoutWork( LYT_WK_ID_UPPER_BACKMOST );  // wk_?  // 親であるapp::ui::UIViewのメンバ関数
          lytWk->GetPane(PANENAME_ZUKAN_MIN_UPP_004_PANE_BG_EVO3)->SetVisible(false);
          lytWk->GetPane(PANENAME_ZUKAN_MIN_UPP_004_PANE_BG_RARE)->SetVisible(false);
          lytWk->GetPane(PANENAME_ZUKAN_MIN_UPP_004_PANE_BG_SR  )->SetVisible(false);
        }
      }

      // ボタンマネージャ
      this->createButtonManager();

      // メニューカーソルレイアウト生成
      app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
      m_param.tool->CreateMenuCursorLayoutWork(pG2DUtil, gfl2::lyt::DISPLAY_LOWER, app::tool::MenuCursor::SIZE_SMALL); 

      {
        // ペインのアドレス
        gfl2::lyt::LytWk*   lytWk_lower   = this->GetLayoutWork( LYT_WK_ID_LOWER );         // wk_?
        if(m_orderTypeIsForId)
        {
          m_listStartPane    = lytWk_lower->GetPane(PANENAME_ZUKAN_MIN_LOW_000_PANE_LIST_NULL_00);
        }
        else
        {
          m_listStartPane    = lytWk_lower->GetPane(PANENAME_ZUKAN_MIN_LOW_004_PANE_LIST_NULL_00);
        }
      }

      // ペインリストに使われているペインに値を設定したいので一度つくる。すぐ破棄する。
      // ペインリスト
      this->createPaneList();
      this->destroyPaneList();

      // ページ内容生成
      this->createPage();
      
      m_scrollBarTopPane->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する

      // インアニメ
      this->startInSeq();
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  ZukanUtil::SetPokeModelVisible(m_param.tool, false);
}
ZukanViewAlolaTop::~ZukanViewAlolaTop()
{
  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // メニューカーソルレイアウト破棄
  //m_param.tool->DestroyMenuCursorLayoutWork();

  // ペインリスト
  this->destroyPaneList();

  // ボタンマネージャ
  this->destroyButtonManager();

  // 絵巻
  GFL_SAFE_DELETE(m_picScrollBook);
  GFL_SAFE_DELETE(m_picScrollDataAccessor);

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  {
    GFL_SAFE_DELETE(m_prismSheet);
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewAlolaTopListener
  GFL_SAFE_DELETE(m_viewListener);

  // ポケアイコン
  GFL_SAFE_DELETE(m_pokeIconTexManager);
  
  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetInputExclusiveKeyByButtonAnimation(m_uiViewManInputExclusiveKey);
}

void ZukanViewAlolaTop::Update(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
 

  // まずは処理し残していることに対処する
  if(m_paneListView)
  {
    app::tool::PaneList* paneList = m_paneListView->GetPaneList();
    if(paneList)
    {
      u32  PANE_LIST_ELEM_NUM     = 0; 
      u32* paneListElemButtonSeId = NULL;
      if(m_orderTypeIsForId)
      {
        PANE_LIST_ELEM_NUM = PANE_LIST_ELEM_NUM_FOR_ID;
        paneListElemButtonSeId = m_paneListElemButtonSeIdForId;
      }
      else
      {
        PANE_LIST_ELEM_NUM = PANE_LIST_ELEM_NUM_FOR_SPEC;
        paneListElemButtonSeId = m_paneListElemButtonSeIdForSpec;
      }

      for(u32 i=0; i<PANE_LIST_ELEM_NUM; ++i)
      {
        if(paneListElemButtonSeId[i] != app::tool::ButtonManager::SE_NULL)
        {
          paneList->SetButtonSEPaneIndex( i, paneListElemButtonSeId[i] );
          paneListElemButtonSeId[i] = app::tool::ButtonManager::SE_NULL;
        }
      }
    }
  }


  // それから今フレームの処理
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
          // インアニメが終わってから生成しないと正しい座標が取れない。
          // ペインリスト
          this->createPaneList();
          m_scrollBarTopPane->SetVisible(true);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する
       
          this->changeDispState(DISP_STATE_ON_WORK);
          m_viewListener->SetInputState(ZukanViewAlolaTopListener::INPUT_STATE_ENABLE);
        }
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        if( pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
        {
 #if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
          // キラカード
          {
            if( m_prismSheet->CanDestroy() )
            {
              m_prismSheet->SetVisible(false);
            }
            else
            {
              break;
            }
          }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

          if(m_paneListView) ZukanUtil::RemovePaneListViewFromSuperView(m_paneListView);  // ペインリストのインプットは止めてあるがUpdateは止まっていない。カーソルを先に破棄するのでペインリストのUpdateも止める。
          this->changeDispState(DISP_STATE_WAIT_SAFETY);
        }
      }
      break;
    }
  case DISP_STATE_WAIT_SAFETY:
    {
      // メニューカーソルレイアウト破棄
      if( m_param.tool->DestroyMenuCursorLayoutWork() )
      {
        m_param.frameCell->Req(ZukanFrameAlolaTopCell::REQ_ID_END);
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
  if(m_picScrollBook) m_picScrollBook->Update();
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  m_prismSheet->Update();
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
}

void ZukanViewAlolaTop::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_BACKMOST, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_CURTAIN , gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_PRISM_SHEET, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_0       , gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_1       , gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_2       , gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_SEARCH  , gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_LOWER         , gfl2::lyt::ORDER_NORMAL, 0 );  // wk_?
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  {
    //描画順変更pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_PRISM_SHEET, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_?
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->Draw( m_param.appRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
}

void ZukanViewAlolaTop::Req(ReqId reqId, s32 reqParam)
{
  switch(reqId)
  {
  //case REQ_ID_KEY_DECIDE   :
  //  {
  //    *m_param.nextSceneType = ZukanType::SCENE_TYPE_INFO;
  //    this->startOutSeq();
  //  }
  //  break;
  case REQ_ID_BUTTON_DECIDE:
    {
      switch(reqParam)
      {
      case BUTTON_ID_SEARCH: *m_param.nextSceneType = ZukanType::SCENE_TYPE_SEARCH; break;
      case BUTTON_ID_BACK  : *m_param.nextSceneType = ZukanType::SCENE_TYPE_LAUNCH; break;
      }
      // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →決定以外でも差していたポケモンを覚えておく
      {
        *m_param.alolaTopSceneChangeFlag = false;
        m_paneListView->GetPaneList()->GetCursorData(m_param.alolaTopScenePaneListSetupParamListPos, m_param.alolaTopScenePaneListSetupParamListScroll);
      }
      this->startOutSeq();
    }
    break;
  case REQ_ID_BUTTON_TOUCH :
    break;  // 何もしない
  case REQ_ID_PANE_LIST_DECIDE:
    {
      u32 currIndex = static_cast<u32>(reqParam);
      const ZukanContents::PokeData* pokeData = m_param.contents->GetDispPokeData(currIndex);
      if(pokeData->figureSeeNum>0)  // 見ていないと先に進めないようにしておく
      {
        *m_param.nextSceneType = ZukanType::SCENE_TYPE_INFO;
        m_param.contents->SetCurrIndexOfDispPokeDataIndexList(currIndex);
        {
          *m_param.alolaTopSceneChangeFlag = false;
          m_paneListView->GetPaneList()->GetCursorData(m_param.alolaTopScenePaneListSetupParamListPos, m_param.alolaTopScenePaneListSetupParamListScroll);
        }
        this->startOutSeq();
      }
    }
    break;
  }
}

void ZukanViewAlolaTop::DoAtCursorMoving( const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  if(m_picScrollBook)
  {
    const ZukanContents::PokeData* pokeData = m_param.contents->GetDispPokeData(pos);
    u32 alolaNo = pokeData->alolaNo;
    if(alolaNo > 0)
    {
      m_picScrollBook->SetSheet(alolaNo);  // カーソルが真逆に出たときはimmediateFlagをtrueにするか？→trueにしないで絵巻の高速移動で対応した。
    }
  }

  // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →アローラ図鑑がどんな並びになっていようと、カーソルが指しているところを記憶しておく
  m_param.contents->SetDispPokeDataToSaveData(pos);
   
  // @fix NMCat[2695] Bボタンで「検索」画面から抜けると不自然なスクロールが行われる
  //                  →カーソルが移動したらその位置を反映しておかないと、セーブデータに頼らない画面生成のとき(検索画面からBキャンセルで戻ってきたとき)のカーソル位置がおかしくなる
  m_param.contents->SetCurrIndexOfDispPokeDataIndexList(pos);
}

void ZukanViewAlolaTop::WriteList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk    = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_?  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0
  
  app::tool::PaneList* paneList = NULL;
  if(m_paneListView) paneList = m_paneListView->GetPaneList();

  gfl2::lyt::LytParts*   paneListPartsPane = static_cast<gfl2::lyt::LytParts*>(m_paneListData[pane_index].base);

  const ZukanType::PaneListElemConstInfo* PANE_LIST_ELEM_CONST_INFO = NULL;
  gfl2::lyt::LytParts*   pokeIconPartsPane      = NULL;
  gfl2::lyt::LytTextBox* pokeNamePane           = NULL;
  gfl2::lyt::LytTextBox* pokeNoPane             = NULL;
  gfl2::lyt::LytTextBox* pokeSpecPane           = NULL;
  u32                    pokeGetSeeAnimIndex    = app::tool::ButtonManager::ANIMATION_NULL;
  u32                    pokeNotSeeAnimIndex    = app::tool::ButtonManager::ANIMATION_NULL;
  u32                    pokeSeeAnimIndex       = app::tool::ButtonManager::ANIMATION_NULL;
  u32*                   paneListElemButtonSeId = NULL;
  if(m_orderTypeIsForId)
  {
    PANE_LIST_ELEM_CONST_INFO = PANE_LIST_ELEM_CONST_INFO_FOR_ID;
    pokeIconPartsPane      = lytWk->GetPartsPane  (paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_000_PANE_POKEMONICON, &lytRes_0);
    pokeNamePane           = lytWk->GetTextBoxPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_000_PANE_TEXTBOX_00 , &lytRes_0);
    pokeNoPane             = lytWk->GetTextBoxPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_000_PANE_TEXTBOX_01 , &lytRes_0);
    pokeSpecPane           = NULL;
    pokeGetSeeAnimIndex    = PANE_LIST_ELEM_CONST_INFO[pane_index].other1Anime;
    pokeNotSeeAnimIndex    = PANE_LIST_ELEM_CONST_INFO[pane_index].other2Anime;
    pokeSeeAnimIndex       = PANE_LIST_ELEM_CONST_INFO[pane_index].other3Anime;  // 3フレーム目だけ使う
    paneListElemButtonSeId = m_paneListElemButtonSeIdForId;
  }
  else
  {
    PANE_LIST_ELEM_CONST_INFO = PANE_LIST_ELEM_CONST_INFO_FOR_SPEC;
    pokeIconPartsPane   = lytWk->GetPartsPane  (paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_006_PANE_POKEMONICON, &lytRes_0);
    pokeNamePane        = lytWk->GetTextBoxPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_006_PANE_TEXTBOX_00 , &lytRes_0);
    pokeNoPane          = lytWk->GetTextBoxPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_006_PANE_TEXTBOX_01 , &lytRes_0);
    pokeSpecPane        = lytWk->GetTextBoxPane(paneListPartsPane, PANENAME_ZUKAN_BTN_LOW_006_PANE_TEXTBOX_02 , &lytRes_0);
    pokeGetSeeAnimIndex = PANE_LIST_ELEM_CONST_INFO[pane_index].other1Anime;
    //pokeNotSeeAnimIndex = PANE_LIST_ELEM_CONST_INFO[pane_index].other2Anime;  // ForSpecにはこういうアニメは用意されていないのでコメントアウト
    //pokeSeeAnimIndex    = PANE_LIST_ELEM_CONST_INFO[pane_index].other3Anime;  // ForSpecにはこういうアニメは用意されていないのでコメントアウト
    paneListElemButtonSeId = m_paneListElemButtonSeIdForSpec;
  }
  gfl2::lyt::LytPicture* pokeIconPane = lytWk->GetPicturePane(pokeIconPartsPane, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &lytRes_0);


  const ZukanContents::PokeData* pokeData = m_param.contents->GetDispPokeData(pos);
  PokeTool::SimpleParam sp = m_param.contents->GetDispPokeSimpleParam(pos);
  u32 tableIndex = m_param.tool->GetPokeExtendData()->GetTableIndex(sp.monsNo, sp.formNo);


  // ポケアイコン
  m_pokeIconTexManager->CreatePokeIconTexture(
      pane_index,
      pokeIconPane,
      0,
      0,
      sp,
      false
  );


  // 捕まえた、見たフラグ
  s32 getSeeAnimFrame = 0;
  if(pokeData->getFlag) getSeeAnimFrame = 2;
  else if(pokeData->figureSeeNum>0) getSeeAnimFrame = 1;
  ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, pokeGetSeeAnimIndex, getSeeAnimFrame);
  if(pokeData->figureSeeNum>0)
  {
    nw::ut::Color8 vtxColor = ZukanType::POKE_GET_COLOR;
    if(pokeData->getFlag==0)
    {
      vtxColor = ZukanType::POKE_SEE_COLOR;
    }
    for(s32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx) pokeIconPane->SetVtxColor(vtxIdx, vtxColor);

    // 押せる押せない状態
    if( pokeNotSeeAnimIndex != app::tool::ButtonManager::ANIMATION_NULL )
    {
      pG2DUtil->StopAnime(LYT_WK_ID_LOWER, pokeNotSeeAnimIndex);  // アンバインドしておかないと他のアニメが付けられない。
                                                                  // [テクニック]
                                                                  // pokeNotSeeAnimIndexで指定しているアニメは、アニメーションの種類全部が出力されている。
                                                                  // そのおかげで、このアニメを付けておけばボタン用の挙動を行うアニメ全てが付かなくなり、
                                                                  // 「ボタンマネージャとしてタッチスクロールはできるが、ボタンが凹むことはない」ということが実現できる！
    }
    if( pokeSeeAnimIndex != app::tool::ButtonManager::ANIMATION_NULL )
    {
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, pokeSeeAnimIndex, 3);
      if(paneList)
      {
        paneList->SetButtonSEPaneIndex( pane_index, SEQ_SE_DECIDE1 );
        paneListElemButtonSeId[pane_index] = app::tool::ButtonManager::SE_NULL;
      }
      else
      {
        paneListElemButtonSeId[pane_index] = SEQ_SE_DECIDE1;  // ここではボタンに設定できなかったので後で設定する
      }
    }
  }
  else
  {
    // 押せる押せない状態
    if( pokeSeeAnimIndex != app::tool::ButtonManager::ANIMATION_NULL )
    {
      pG2DUtil->StopAnime(LYT_WK_ID_LOWER, pokeSeeAnimIndex);
    }
    if( pokeNotSeeAnimIndex != app::tool::ButtonManager::ANIMATION_NULL )
    {
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, pokeNotSeeAnimIndex, 0);
      if(paneList)
      {
        paneList->SetButtonSEPaneIndex( pane_index, SEQ_SE_BEEP );
        paneListElemButtonSeId[pane_index] = app::tool::ButtonManager::SE_NULL;
      }
      else
      {
        paneListElemButtonSeId[pane_index] = SEQ_SE_BEEP;  // ここではボタンに設定できなかったので後で設定する
      }
    }
  }


  // テキスト
  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    // 図鑑番号
    pG2DUtil->SetTextBoxPaneNumber(pokeNoPane, msg_zkn_03_01, pokeData->alolaNo, 3, 0, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU);
    // ポケモン種族名
    if(pokeData->figureSeeNum>0)
    {
      pG2DUtil->SetRegisterMonsNameNo(0, sp.monsNo);
      pG2DUtil->SetTextBoxPaneStringExpand(pokeNamePane, msg_zkn_03_03);
      //if(paneList) paneList->SetButtonActive(pane_index, true);
    }
    else
    {
      pG2DUtil->SetTextBoxPaneStringExpand(pokeNamePane, msg_zkn_08_03);
      //if(paneList) paneList->SetButtonPassive(pane_index, true);  // ボタンをパッシブにできない。暗くならない。押せてしまう。タッチはできなくなったが、そのせいでスクロールもできない。→コメントアウトし、押せるが禁止音を鳴らすことで対応した。
    }
  }
  pG2DUtil->SetMessageData(NULL, NULL);


  // specテキスト
  if(m_orderTypeIsForId == false)
  {
    gfl2::str::MsgData* msgDataForSpec = NULL;
    if(m_param.contents->GetOrderType() == ZukanType::ORDER_TYPE_TALL || m_param.contents->GetOrderType() == ZukanType::ORDER_TYPE_SHORT)
    {
      msgDataForSpec = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_HEIGHT);
    }
    else  // if(m_param.contents->GetOrderType() == ZukanType::ORDER_TYPE_HEAVY || m_param.contents->GetOrderType() == ZukanType::ORDER_TYPE_LIGHT)
    {
      msgDataForSpec = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_WEIGHT);
    }

    pG2DUtil->SetMessageData( msgDataForSpec, m_param.tool->GetWordSet() );
    {
      if(tableIndex >= msgDataForSpec->GetStrCount())
      {
        tableIndex = 0;
      }
      pG2DUtil->SetTextBoxPaneString(pokeSpecPane, tableIndex);
    }
    pG2DUtil->SetMessageData(NULL, NULL);
  }
}

void ZukanViewAlolaTop::createButtonManager(void)
{
  const ZukanType::ButtonConstInfo* BUTTON_CONST_INFO = BUTTON_CONST_INFO_FOR_SPEC;
  m_buttonPaneInfo = m_buttonPaneInfoForSpec;
  if(m_orderTypeIsForId)
  {
    BUTTON_CONST_INFO = BUTTON_CONST_INFO_FOR_ID;
    m_buttonPaneInfo = m_buttonPaneInfoForId;
  }

  ZukanUtil::CreateButtonManager(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_?
      BUTTON_ID_NUM,
      BUTTON_CONST_INFO,
      NULL,
      m_buttonPaneInfo
  );
}
void ZukanViewAlolaTop::destroyButtonManager(void)
{
  ZukanUtil::DestroyButtonManager(this);
}

void ZukanViewAlolaTop::createPaneList(void)
{
  const ZukanType::PaneListConstInfo*     PANE_LIST_CONST_INFO      = &PANE_LIST_CONST_INFO_FOR_SPEC;
  const ZukanType::PaneListElemConstInfo* PANE_LIST_ELEM_CONST_INFO = PANE_LIST_ELEM_CONST_INFO_FOR_SPEC;
  const ZukanType::ScrollBarConstInfo*    SCROLL_BAR_CONST_INFO     = &SCROLL_BAR_CONST_INFO_FOR_SPEC;
  m_paneListElemNum = PANE_LIST_ELEM_NUM_FOR_SPEC;
  m_paneListData = m_paneListDataForSpec;
  if(m_orderTypeIsForId)
  {
    PANE_LIST_CONST_INFO      = &PANE_LIST_CONST_INFO_FOR_ID;
    PANE_LIST_ELEM_CONST_INFO = PANE_LIST_ELEM_CONST_INFO_FOR_ID;
    SCROLL_BAR_CONST_INFO     = &SCROLL_BAR_CONST_INFO_FOR_ID;
    m_paneListElemNum = PANE_LIST_ELEM_NUM_FOR_ID;
    m_paneListData = m_paneListDataForId;
  }

  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(true);

  m_paneListViewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanAlolaTopPaneListViewListener(this);
  m_paneListCallBack     = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanAlolaTopPaneListCallBack(this);

  u32 listPos    = *m_param.alolaTopScenePaneListSetupParamListPos;
  f32 listScroll = *m_param.alolaTopScenePaneListSetupParamListScroll; 
  if(*m_param.alolaTopSceneChangeFlag)
  {
    ZukanUtil::MakePaneListSetupParam(
      PANE_LIST_CONST_INFO,
      m_param.contents->GetDispPokeDataIndexNum(),
      m_param.contents->GetCurrIndexOfDispPokeDataIndexList(),
      &listPos,
      &listScroll
    );
  }

  m_paneListView = ZukanUtil::CreatePaneList(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_?
      m_paneListViewListener,
      m_paneListElemNum,
      PANE_LIST_ELEM_CONST_INFO,
      m_paneListData,
      PANE_LIST_CONST_INFO,
      m_listStartPane,  // @fix NMCat[2101] ポケモンナンバー上部に上のアイコンの影が被って表示される  ←ペインの並び順を変更できるように、リストの親ペインを渡す。
      m_paneListCallBack,
      menuCursor,
      m_param.gameManager->GetUiDeviceManager(),
      SCROLL_BAR_CONST_INFO,
      m_param.contents->GetDispPokeDataIndexNum(),
      listPos,
      listScroll 
  );
}
void ZukanViewAlolaTop::destroyPaneList(void)
{
  ZukanUtil::DestroyPaneList(this, m_paneListView);
  m_paneListView = NULL;

  GFL_SAFE_DELETE(m_paneListCallBack);
  GFL_SAFE_DELETE(m_paneListViewListener);
  
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(false);
}

void ZukanViewAlolaTop::startInSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  if(m_orderTypeIsForId)
  {
    switch(m_param.prevSceneType)
    {
    case ZukanType::SCENE_TYPE_INFO:
    case ZukanType::SCENE_TYPE_SEARCH:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_000_BACK_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_IN;
      break;
    case ZukanType::SCENE_TYPE_LAUNCH:
    default:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_000_IN_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_IN;
      break;
    }
  }
  else
  {
    switch(m_param.prevSceneType)
    {
    case ZukanType::SCENE_TYPE_INFO:
    case ZukanType::SCENE_TYPE_SEARCH:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_004_BACK_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_IN;
      break;
    case ZukanType::SCENE_TYPE_LAUNCH:
    default:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_004_IN_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_IN;
      break;
    }
  }

  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_IN);
 
  if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
  {
    pFadeManager->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS);
  }

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewAlolaTopListener::INPUT_STATE_OCCUPY);
}

void ZukanViewAlolaTop::startOutSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  if(m_orderTypeIsForId)
  {
    switch(*m_param.nextSceneType)
    {
    case ZukanType::SCENE_TYPE_INFO:
    case ZukanType::SCENE_TYPE_SEARCH:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_000_NEXT_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_OUT;
      break;
    case ZukanType::SCENE_TYPE_LAUNCH:
    default:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_000_RETURN_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_OUT;
      break;
    }
  }
  else
  {
    switch(*m_param.nextSceneType)
    {
    case ZukanType::SCENE_TYPE_INFO:
    case ZukanType::SCENE_TYPE_SEARCH:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_004_NEXT_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_OUT;
      break;
    case ZukanType::SCENE_TYPE_LAUNCH:
    default:
      m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ALOLA_TOP_ZUKAN_MIN_LOW_004_RETURN_PLATE;
      m_waitAnimIndex[LYT_WK_ID_UPPER_SEARCH] = LA_ALOLA_TOP_ZUKAN_SRC_UPP_001_OUT;
      break;
    }
  }

  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(false);
 
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_OUT);

  // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →アローラ図鑑のカーソル判定フラグを追加。
  *m_param.launchAndAlolaTopSceneDefaultFlag = false;  // アローラ図鑑画面を抜けるときにfalseで初期化。この後の判定でtrueに変える。

  if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
  {
    pFadeManager->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &ZukanType::CROSS_FADE_COLOR_START, &ZukanType::CROSS_FADE_COLOR_END, 1);  // ぱっと消す

    // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →アローラ図鑑のカーソル判定フラグを追加。
    if(*m_param.nextSceneType == ZukanType::SCENE_TYPE_LAUNCH)
    {
      *m_param.launchAndAlolaTopSceneDefaultFlag = true;
    }
  }

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewAlolaTopListener::INPUT_STATE_OCCUPY);

  m_scrollBarTopPane->SetVisible(false);  // @fix NMCat[1496][1497]  スクロールバーの表示非表示を１階層上のペインで制御する

  this->SetInputEnabledAllView(false);  // Updateは子から処理される。
}

void ZukanViewAlolaTop::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewAlolaTop::createPage(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*   lytWk_serach  = this->GetLayoutWork( LYT_WK_ID_UPPER_SEARCH );  // wk_?
  gfl2::lyt::LytWk*   lytWk_lower   = this->GetLayoutWork( LYT_WK_ID_LOWER );         // wk_?
 
  // ペインのアドレス
  m_searchTopPane       = lytWk_serach->GetPane(PANENAME_ZUKAN_SRC_UPP_001_PANE_ALL);

  if(m_orderTypeIsForId)
  {
    m_scrollBarTopPane = lytWk_lower->GetPane(PANENAME_ZUKAN_MIN_LOW_000_PANE_SCROLL_S_PROG);
  }
  else
  {
    m_scrollBarTopPane = lytWk_lower->GetPane(PANENAME_ZUKAN_MIN_LOW_004_PANE_SCROLL_S_00_PROG);
  }

  if( !ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
  {
    gfl2::lyt::LytWk*  lytWk_0  = this->GetLayoutWork( LYT_WK_ID_UPPER_0 );  // wk_?
    gfl2::lyt::LytWk*  lytWk_1  = this->GetLayoutWork( LYT_WK_ID_UPPER_1 );  // wk_?
    gfl2::lyt::LytWk*  lytWk_2  = this->GetLayoutWork( LYT_WK_ID_UPPER_2 );  // wk_?

    ////////////////
    ////////////////
    // 上画面 
    ////////////////
    ////////////////
    gfl2::lyt::LytPane* topPane_0 = lytWk_0->GetPane(PANENAME_ZUKAN_MIN_UPP_000_PANE_ALL);
    gfl2::lyt::LytPane* topPane_1 = lytWk_1->GetPane(PANENAME_ZUKAN_MIN_UPP_000_PANE_ALL);
    gfl2::lyt::LytPane* topPane_2 = lytWk_2->GetPane(PANENAME_ZUKAN_MIN_UPP_000_PANE_ALL);

    topPane_0->SetVisible(false);
    topPane_1->SetVisible(false);
    topPane_2->SetVisible(false);


    // ペインのアドレス
    m_searchOrderTextPane = lytWk_serach->GetTextBoxPane(PANENAME_ZUKAN_SRC_UPP_001_PANE_TEXTBOX_00);
    m_searchCountTextPane = lytWk_serach->GetTextBoxPane(PANENAME_ZUKAN_SRC_UPP_001_PANE_TEXTBOX_01);
  
    // 検索結果を表示する
    m_searchTopPane->SetVisible(true);

    pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
    {
      pG2DUtil->SetTextBoxPaneStringExpand(m_searchOrderTextPane, msg_zkn_search_11_01);
      
      u32 currNum  = m_param.contents->GetDispPokeDataIndexNum();
      u32 totalNum = m_param.contents->GetRealSeeNum(ZukanType::INFO_TYPE_ALOLA);
      pG2DUtil->SetRegisterNumber(0, currNum, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU);
      pG2DUtil->SetRegisterNumber(1, totalNum, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU);
      pG2DUtil->SetTextBoxPaneStringExpand(m_searchCountTextPane, msg_zkn_search_11_02);
    }
    pG2DUtil->SetMessageData(NULL, NULL);
  }
  else
  {
    // 検索結果を表示しない
    m_searchTopPane->SetVisible(false);
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

