//==============================================================================
/**
 @file    ZukanViewIslandTop.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の島図鑑トップ画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/island_top.h>
#include <niji_conv_header/app/zukan/island_top_pane.h>
#include <niji_conv_header/app/zukan/island_top_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanContents.h"
#include "ZukanUtil.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanFormOrder.h"
#include "ZukanFrameIslandTop.h"
#include "ZukanDistributionPokeDataAccessor.h"
#include "ZukanDistributionPositionDataAccessor.h"
#include "ZukanDistributionZoneDataAccessor.h"
#include "ZukanSmallIslandMap.h"

#include "ZukanViewIslandTop.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewIslandTopListener
 @brief     図鑑の島図鑑トップ画面のビューのリスナー
 */
//==============================================================================
class ZukanViewIslandTopListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewIslandTopListener);

public:
  ZukanViewIslandTopListener(ZukanViewIslandTop* view);
  virtual ~ZukanViewIslandTopListener();

  //virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
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
  ZukanViewIslandTop* m_view;
  InputState       m_inputState;
};


//==============================================================================
//
//  ZukanViewIslandTopListener
//
//==============================================================================
ZukanViewIslandTopListener::ZukanViewIslandTopListener(ZukanViewIslandTop* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewIslandTopListener::~ZukanViewIslandTopListener()
{}

/*
app::ui::UIInputListener::ListenerResult ZukanViewIslandTopListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_view->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )  // ボタンマネージャ動作中はタッチ入力させない
  {
    if( pTouchPanel->IsTouchTrigger() )
    {
      Sound::PlaySE( SEQ_SE_RELEASE );
      m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_DECIDE);
    }
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
*/
app::ui::UIInputListener::ListenerResult ZukanViewIslandTopListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
      if( m_view->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )  // ボタンマネージャ動作中はキー入力させない
      {
        if     ( pButton->IsTrigger(gfl2::ui::BUTTON_A    ) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_DECIDE);
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_UP   ) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_UP    , ZukanViewIslandTop::KEY_INPUT_TRIGGER);
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_DOWN ) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_DOWN  , ZukanViewIslandTop::KEY_INPUT_TRIGGER);
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_LEFT ) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_LEFT  , ZukanViewIslandTop::KEY_INPUT_TRIGGER);
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_RIGHT , ZukanViewIslandTop::KEY_INPUT_TRIGGER);
        else if( pButton->IsRepeat (gfl2::ui::BUTTON_UP   ) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_UP    , ZukanViewIslandTop::KEY_INPUT_REPEAT );
        else if( pButton->IsRepeat (gfl2::ui::BUTTON_DOWN ) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_DOWN  , ZukanViewIslandTop::KEY_INPUT_REPEAT );
        else if( pButton->IsRepeat (gfl2::ui::BUTTON_LEFT ) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_LEFT  , ZukanViewIslandTop::KEY_INPUT_REPEAT );
        else if( pButton->IsRepeat (gfl2::ui::BUTTON_RIGHT) ) m_view->Req(ZukanViewIslandTop::REQ_ID_KEY_RIGHT , ZukanViewIslandTop::KEY_INPUT_REPEAT );
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
app::ui::UIInputListener::ListenerResult ZukanViewIslandTopListener::OnLayoutPaneEvent( const u32 painId )
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
      case ZukanViewIslandTop::BUTTON_ID_BACK      : m_view->Req(ZukanViewIslandTop::REQ_ID_BUTTON_DECIDE, ZukanViewIslandTop::BUTTON_ID_BACK      ); break;
      default                                      : m_view->Req(ZukanViewIslandTop::REQ_ID_BUTTON_DECIDE, button_id                               ); break;
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
void ZukanViewIslandTopListener::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
  switch(m_inputState)
  {
  case INPUT_STATE_ENABLE:
    {
      u32 button_id = paneId;
      switch(button_id)
      {
      case ZukanViewIslandTop::BUTTON_ID_BACK       : m_view->Req(ZukanViewIslandTop::REQ_ID_BUTTON_TOUCH, ZukanViewIslandTop::BUTTON_ID_BACK      ); break;
      default                                       : m_view->Req(ZukanViewIslandTop::REQ_ID_BUTTON_TOUCH, button_id                               ); break;
      }
      break;
    }
  }
}

void ZukanViewIslandTopListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
  
  app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
}




//==============================================================================
//
//  ZukanViewIslandTop
//
//==============================================================================
ZukanViewIslandTop::ZukanViewIslandTop(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL),
    m_cursorVector(0,0),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_pokeBgPane(NULL),
    m_pokeNoTextPane(NULL),
    m_pokeNameTextPane(NULL),
    m_whereTextPane(NULL),
    m_cursorPane(NULL),
    m_isKeyDecide(false),
    m_isMoveCursorByTouch(false),
    m_distributionPokeDataAccessor(NULL),
    m_distributionPositionDataAccessor(NULL),
    m_distributionZoneDataAccessor(NULL),
    m_smallIslandMap(NULL)
{
  // 初期化
  {
    for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
    {
      m_waitAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }

    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      m_buttonPaneInfo[i] = ZukanType::ButtonPaneInfo();
    }
 
    for(u32 i=0; i<BUTTON_ID_POKE_NUM; ++i)
    {
      m_pokeIconTexBuf[i] = NULL;
    }
  }

  // distributionクラス
  {
    const void* pokeBuf     = m_param.dataManager->GetZukanDistributionPackElemBuf(ZukanDataManager::ZUKAN_DISTRIBUTION_PACK_ELEM_ID_POKE_DATA);
    const void* positionBuf = m_param.dataManager->GetZukanDistributionPackElemBuf(ZukanDataManager::ZUKAN_DISTRIBUTION_PACK_ELEM_ID_POSITION_DATA);
    const void* zoneBuf     = m_param.dataManager->GetZukanDistributionPackElemBuf(ZukanDataManager::ZUKAN_DISTRIBUTION_PACK_ELEM_ID_ZONE_DATA);
    GFL_ASSERT(pokeBuf);  // 開発中に気付かせるためのASSERT
    GFL_ASSERT(positionBuf);  // 開発中に気付かせるためのASSERT
    GFL_ASSERT(zoneBuf);  // 開発中に気付かせるためのASSERT

    m_distributionPokeDataAccessor     = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanDistributionPokeDataAccessor();
    m_distributionPositionDataAccessor = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanDistributionPositionDataAccessor();
    m_distributionZoneDataAccessor     = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanDistributionZoneDataAccessor();

    m_distributionPokeDataAccessor->SetupData(pokeBuf);
    m_distributionPositionDataAccessor->SetupData(positionBuf);
    m_distributionZoneDataAccessor->SetupData(zoneBuf);
  }

  // ZukanViewIslandTopListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewIslandTopListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  {
    // @fix[1875] 島図鑑に出入りする度にカーソルがデフォルト位置に戻ることについて  // 『@fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る』にカーソル位置記憶に関してまとめてある。
    // カーソル位置(セーブデータが覚えているカーソル位置も合わせて修正)
    bool cursorSet = false; 
    if(m_param.prevSceneType == ZukanType::SCENE_TYPE_LAUNCH)
    {
      u32 indexOfDispPokeDataIndexList = 0;
      const ZukanContents::PokeData* pokeData = m_param.contents->GetDispPokeDataFromSaveData(&indexOfDispPokeDataIndexList);
      if(pokeData)  // 「最後に見ていた図鑑が違うとき、および、初めて図鑑を起動したとき」はNULLなので、ここでは何もせずこれより後のブロックに任せる。
      {
        m_param.contents->SetCurrIndexOfDispPokeDataIndexList(indexOfDispPokeDataIndexList);
        cursorSet = true;
      }
    }
    else if(m_param.prevSceneType == ZukanType::SCENE_TYPE_INFO)
    {
      // m_param.contents->GetCurrIndexOfDispPokeDataIndexList(),
      // をそのまま使う。
      cursorSet = true;
    }

    if(!cursorSet)
    {
      u32 dispIndexNum = m_param.contents->GetDispPokeDataIndexNum();
      for(u32 dispIndex=0; dispIndex<dispIndexNum; ++dispIndex)
      {
        const ZukanContents::PokeData* dispPokeData = m_param.contents->GetDispPokeData(dispIndex);
        if( dispPokeData->figureSeeNum > 0 )  // 見たポケモンにカーソルを合わせておく
        {
          m_param.contents->SetCurrIndexOfDispPokeDataIndexList(dispIndex);
          cursorSet = true;
          break;
        }
      }
    
      if(!cursorSet)  // 全条件から漏れてきたときの最後の受け皿
      {
        m_param.contents->SetCurrIndexOfDispPokeDataIndexList(0);
        // セーブデータが覚えているカーソル位置
        m_param.contents->ResetDispPokeDataOfSaveData();
      }
    }    

    // @fix[1875] 島図鑑に出入りする度にカーソルがデフォルト位置に戻ることについて  // 『@fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る』にカーソル位置記憶に関してまとめてある。
    // 島図鑑を立ち上げたときのカーソル位置を記憶しておく
    // セーブデータが覚えているカーソル位置
    m_param.contents->SetDispPokeDataToSaveData(m_param.contents->GetCurrIndexOfDispPokeDataIndexList());
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_ISLAND_TOP);
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
        // wk_0
        {
          // 上画面
          0,  // res_0
          LYTRES_ISLAND_TOP_ZUKAN_ISD_UPP_000_BFLYT,
          LA_ISLAND_TOP_ZUKAN_ISD_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
     
        // wk_1
        {
          // 下画面
          0,  // res_0
          LYTRES_ISLAND_TOP_ZUKAN_MIN_LOW_003_BFLYT,
          LA_ISLAND_TOP_ZUKAN_MIN_LOW_003___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },

        // wk_2
        {
          // 上画面
          0,  // res_0
          LYTRES_ISLAND_TOP_ZUKAN_ISD_UPP_001_BFLYT,
          LA_ISLAND_TOP_ZUKAN_ISD_UPP_001___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          3,  // wk_0, wk_1, wk_2の3個 
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );

      // 島マップ
      {
        app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
        m_smallIslandMap = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanSmallIslandMap(
            this->GetG2DUtil(),  // 親であるapp::ui::UIViewのメンバ
            pG2DUtil->GetLayoutResourceID( 0 ),  // res_0
            LYT_WK_ID_UPPER,  // wk_0
            this->GetLayoutWork( LYT_WK_ID_UPPER ),  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
            m_param.contents->GetInfoType(),
            m_distributionPokeDataAccessor,
            m_distributionPositionDataAccessor,
            m_distributionZoneDataAccessor
        );
      }

      // ボタンマネージャ
      this->createButtonManager();
   
      // ページ内容生成
      this->createPage();
 
      // インアニメ
      this->startInSeq();
   }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  // @fix GFNMCat[4238] 図鑑：島図鑑を立ち上げたときのカーソル位置が見ていないポケモンのときに、上画面にポケモンモデルが一瞬表示される  →島図鑑起動時はポケモンモデルを非表示にしておく。
  ZukanUtil::SetPokeModelVisible(m_param.tool, false);

  //m_param.tool->SetPokeModelVisible(true);
  //m_param.tool->ChangePokeModel(m_param.contents->GetCurrDispPokeData()->sp);はsetCurrDispPoke内で呼んでいる
  m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_RIGHT);
  this->setCurrDispPoke();
}
ZukanViewIslandTop::~ZukanViewIslandTop()
{
  //m_param.tool->SetPokeModelVisible(false);
  m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_CENTER);

  // ポケモンアイコン
  for(u32 i=0; i<BUTTON_ID_POKE_NUM; ++i)
  {
    GflHeapSafeFreeMemory(m_pokeIconTexBuf[i]);
  }

  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // ボタンマネージャ
  this->destroyButtonManager();

  // 島マップ
  GFL_SAFE_DELETE(m_smallIslandMap);

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewIslandTopListener
  GFL_SAFE_DELETE(m_viewListener);

  // distributionクラス
  GFL_SAFE_DELETE(m_distributionZoneDataAccessor);
  GFL_SAFE_DELETE(m_distributionPositionDataAccessor);
  GFL_SAFE_DELETE(m_distributionPokeDataAccessor);
}

void ZukanViewIslandTop::Update(void)
{
  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      break;
    }
  case DISP_STATE_ON_IN:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        {
          const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
          ZukanUtil::SetPokeModelVisible(m_param.tool, pokeData->figureSeeNum>0);
        }
      
        this->putCursorPane();
        this->changeDispState(DISP_STATE_ON_WORK);
        m_viewListener->SetInputState(ZukanViewIslandTopListener::INPUT_STATE_ENABLE);
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      break;
    }
  case DISP_STATE_ON_POKE_BUTTON_DECIDE:
    {
      // @fix NMCat[1886] 島図鑑でポケモンアイコンをタッチリリース直後にポケモン情報画面に遷移する  →「カーソル合わせタッチ」と「決定タッチ」を分ける。
      app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
      u32 pokeButtonId = this->convertCursorVectorToButtonId(m_cursorVector);  // ポケモンボタン
      if( pG2DUtil->IsAnimeEnd(LYT_WK_ID_LOWER, BUTTON_CONST_INFO[pokeButtonId].other2_anime_index) )
      {
        *m_param.nextSceneType = ZukanType::SCENE_TYPE_INFO;
        this->startOutSeq();
      }
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        ZukanUtil::SetPokeModelVisible(m_param.tool, false);
        
        m_param.frameCell->Req(ZukanFrameIslandTopCell::REQ_ID_END);
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
}
void ZukanViewIslandTop::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER_BG, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_2
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_LOWER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_1
}

void ZukanViewIslandTop::Req(ReqId reqId, s32 reqParam)
{
  bool moveFlag = false;
  bool moveIsTrigger = false;
  ZukanType::CursorVector moveDir(0,0);

  switch(reqId)
  {
  case REQ_ID_KEY_DECIDE   :
    {
      // @fix NMCat[1886] 島図鑑でポケモンアイコンをタッチリリース直後にポケモン情報画面に遷移する  →「カーソル合わせタッチ」と「決定タッチ」を分ける。
      // キー入力で決定したとき
      app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
      u32 pokeButtonId = this->convertCursorVectorToButtonId(m_cursorVector);  // ポケモンボタン
      if( buttonManager->IsButtonActive(pokeButtonId) )
      {
        m_isKeyDecide = true;
        buttonManager->StartSelectedAct(pokeButtonId);
      }
      break;
    }
  case REQ_ID_KEY_UP   :
    {
      moveFlag = true;
      moveIsTrigger = (reqParam==KEY_INPUT_TRIGGER);
      moveDir.y -= 1;
      break;
    }
  case REQ_ID_KEY_DOWN :
    {
      moveFlag = true;
      moveIsTrigger = (reqParam==KEY_INPUT_TRIGGER);
      moveDir.y += 1;
      break;
    }
  case REQ_ID_KEY_LEFT :
    {
      moveFlag = true;
      moveIsTrigger = (reqParam==KEY_INPUT_TRIGGER);
      moveDir.x -= 1;
      break;
    }
  case REQ_ID_KEY_RIGHT:
    {
      moveFlag = true;
      moveIsTrigger = (reqParam==KEY_INPUT_TRIGGER);
      moveDir.x += 1;
      break;
    }
  case REQ_ID_BUTTON_DECIDE:
    {
      u32 button_id = reqParam;
      switch(reqParam)
      {
      case BUTTON_ID_BACK  : *m_param.nextSceneType = ZukanType::SCENE_TYPE_LAUNCH;  this->startOutSeq();  break;
      default              :
        {
          // @fix NMCat[1886] 島図鑑でポケモンアイコンをタッチリリース直後にポケモン情報画面に遷移する  →「カーソル合わせタッチ」と「決定タッチ」を分ける。
          bool isPokeButtonDecided = false;
          if(m_isKeyDecide)
          {
            isPokeButtonDecided = true;
          }
          else
          {
            if(m_isMoveCursorByTouch == false)
            {
              u32 pokeButtonId = this->convertCursorVectorToButtonId(m_cursorVector);  // ポケモンボタン
              if(button_id == pokeButtonId)
              {
                isPokeButtonDecided = true;
              }
            }
          }
          
          this->setCursorVector(button_id);
          
          if(isPokeButtonDecided)
          {
            // 決定
            app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
            Sound::PlaySE( SEQ_SE_DECIDE1 );
            pG2DUtil->StartAnime(LYT_WK_ID_LOWER, BUTTON_CONST_INFO[button_id].other2_anime_index);
         
            this->changeDispState(DISP_STATE_ON_POKE_BUTTON_DECIDE);

            // 占有するが入力受け付けなし
            m_viewListener->SetInputState(ZukanViewIslandTopListener::INPUT_STATE_OCCUPY);
          }
          break;
        }
      }
    }
    break;
  case REQ_ID_BUTTON_TOUCH :
    {
      u32 button_id = reqParam;
      switch(reqParam)
      {
      case BUTTON_ID_BACK  :                                    break;
      default              :
        {
          // @fix NMCat[1886] 島図鑑でポケモンアイコンをタッチリリース直後にポケモン情報画面に遷移する  →「カーソル合わせタッチ」と「決定タッチ」を分ける。
          u32 pokeButtonId = this->convertCursorVectorToButtonId(m_cursorVector);  // ポケモンボタン
          m_isMoveCursorByTouch = (button_id != pokeButtonId);
          this->setCursorVector(button_id);  // button_idはポケモンボタンのIDの範囲内
          break;
        }
      }
      break;
    } 
  }

  if(moveFlag)
  {
    this->moveCursorVector(moveDir, moveIsTrigger);
  }
}

void ZukanViewIslandTop::createButtonManager(void)
{
  ZukanUtil::CreateButtonManager(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_1
      BUTTON_ID_NUM,
      BUTTON_CONST_INFO,
      NULL,
      m_buttonPaneInfo
  );
}
void ZukanViewIslandTop::destroyButtonManager(void)
{
  ZukanUtil::DestroyButtonManager(this);
}

void ZukanViewIslandTop::startInSeq(void)
{
  switch(m_param.prevSceneType)
  {
  case ZukanType::SCENE_TYPE_INFO:
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_000_BACK;
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ISLAND_TOP_ZUKAN_MIN_LOW_003_BACK;
    m_waitAnimIndex[LYT_WK_ID_UPPER_BG] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_001_BACK;
    break;
  case ZukanType::SCENE_TYPE_LAUNCH:
  default:
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_000_IN;
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ISLAND_TOP_ZUKAN_MIN_LOW_003_IN;
    m_waitAnimIndex[LYT_WK_ID_UPPER_BG] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_001_IN;
    break;
  }
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_IN);

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewIslandTopListener::INPUT_STATE_OCCUPY);
}
void ZukanViewIslandTop::startOutSeq(void)
{
  switch(*m_param.nextSceneType)
  {
  case ZukanType::SCENE_TYPE_INFO:
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_000_NEXT;
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ISLAND_TOP_ZUKAN_MIN_LOW_003_NEXT;
    m_waitAnimIndex[LYT_WK_ID_UPPER_BG] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_001_NEXT;
    break;
  case ZukanType::SCENE_TYPE_LAUNCH:
  default:
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_000_RETURN;
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_ISLAND_TOP_ZUKAN_MIN_LOW_003_RETURN;
    m_waitAnimIndex[LYT_WK_ID_UPPER_BG] = LA_ISLAND_TOP_ZUKAN_ISD_UPP_001_RETURN;
    break;
  }
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_OUT);

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewIslandTopListener::INPUT_STATE_OCCUPY);

  m_cursorPane->SetVisible(false);
}

void ZukanViewIslandTop::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewIslandTop::createPage(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_UPPER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytWk*        lytWk_1  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytWk*        lytWk_2  = this->GetLayoutWork( LYT_WK_ID_UPPER_BG );  // wk_2  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0
  
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ

  ////////////////
  ////////////////
  // 上画面
  ////////////////
  ////////////////
  {
    m_pokeNoTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_ISD_UPP_000_PANE_TEXTBOX_01);
    m_pokeNameTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_ISD_UPP_000_PANE_TEXTBOX_00);
    m_whereTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_ISD_UPP_000_PANE_TEXTBOX_02);
   
    // テキスト
    static const u32 ISLAND_NAME_MES[] =
    {
      msg_zkn_04_01,
      msg_zkn_04_02,
      msg_zkn_04_03,
      msg_zkn_04_04,
    };
    u32 islandNameMsgId = ISLAND_NAME_MES[m_param.contents->GetInfoType() - ZukanType::INFO_TYPE_ISLAND_START];

    pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
    {
      // 島名
      pG2DUtil->SetTextBoxPaneString(lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_ISD_UPP_000_PANE_NAME), islandNameMsgId);
    }
    pG2DUtil->SetMessageData(NULL, NULL);
  }

  ////////////////
  ////////////////
  // ポケモンモデルの背景（上画面）
  ////////////////
  ////////////////
  {
    m_pokeBgPane = lytWk_2->GetPane(PANENAME_ZUKAN_ISD_UPP_001_PANE_POKE_BG);
  }

  ////////////////
  ////////////////
  // 下画面
  ////////////////
  ////////////////
  {
    // 枠
    static const u32 ISLAND_FRAME_PANE_ID[] =
    {
      PANENAME_ZUKAN_MIN_LOW_003_PANE_ISLAND1,
      PANENAME_ZUKAN_MIN_LOW_003_PANE_ISLAND2_3,
      PANENAME_ZUKAN_MIN_LOW_003_PANE_ISLAND2_3,
      PANENAME_ZUKAN_MIN_LOW_003_PANE_ISLAND4,
    };
    gfl2::lyt::LytPane* pane = lytWk_1->GetPane( ISLAND_FRAME_PANE_ID[m_param.contents->GetInfoType() - ZukanType::INFO_TYPE_ISLAND_START] );
    pane->SetVisible(true);
  }
  
  {
    // ポケモンアイコン
    u32 j = 0;
    for(u32 i=BUTTON_ID_POKE_START; i<=BUTTON_ID_POKE_END; ++i)  // iとjを間違えて使わないように！  ボタンに使うのはi、ポケモンに使うのはj
    {
      if( j < m_param.contents->GetDispPokeDataIndexNum() )
      {
        gfl2::lyt::LytPicture*   pokeIconPane = lytWk_1->GetPicturePane(m_buttonPaneInfo[i].partsPane, PANENAME_ZUKAN_BTN_LOW_005_PANE_POKE_ICON, &lytRes_0);
        
        const ZukanContents::PokeData* pokeData = m_param.contents->GetDispPokeData(j);

        // アローラ種、方角のいるポケモンに関してはフォルム番号を変更する
        if(pokeData->formOrderType != ZukanFormOrder::FORM_ORDER_TYPE_FORM_NO)
        {
          ZukanFormOrder::FormOrder formOrder;
          bool formOrderEnable = ZukanFormOrder::GetFormOrder(static_cast<MonsNo>(pokeData->monsNo), &formOrder);
          if( formOrderEnable && ( (formOrder.formOrderType == ZukanFormOrder::FORM_ORDER_TYPE_ALOLA) || (formOrder.formOrderType == ZukanFormOrder::FORM_ORDER_TYPE_DIRECTION) ) )
          {
            m_param.contents->SetDispDrawFigureDataIndexSpecialFormNo(j, formOrder);
          }
        }

        PokeTool::SimpleParam sp = m_param.contents->GetDispPokeSimpleParam(j);
        
        m_pokeIconTexBuf[j] = ZukanUtil::CreatePokeIconTexture(m_param.appHeap->GetDeviceHeap(), m_param.appHeap->GetDeviceHeap()->GetLowerHandle(), pokeIconPane, 0, 0, sp, false, m_param.dataManager);

        // 捕まえた、見たフラグ
        s32 getSeeAnimFrame = 0;
        if(pokeData->getFlag) getSeeAnimFrame = 2;
        else if(pokeData->figureSeeNum>0) getSeeAnimFrame = 1;
        ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, BUTTON_CONST_INFO[i].other1_anime_index, getSeeAnimFrame);
        //if(pokeData->figureSeeNum>0 && pokeData->getFlag==0)  // other1_anime_indexに設定したStatusという名前のアニメでペインの色を暗くしてくれているようなのでコメントアウトした。
        //{
        //  nw::ut::Color8 vtxColor(80,80,80,255);
        //  for(s32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx) pokeIconPane->SetVtxColor(vtxIdx, vtxColor);
        //}
        
        if(pokeData->figureSeeNum==0)
        {
          buttonManager->SetButtonPassive(i);
        }
      }
      else
      {
        ZukanUtil::SetButtonVisible(this->GetButtonManager(), i, false);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
      }
      ++j;
    }
  }

  {
    // カーソル
    m_cursorPane = lytWk_1->GetPane(PANENAME_ZUKAN_MIN_LOW_003_PANE_CURSOR);
    m_cursorPane->SetVisible(false);
    pG2DUtil->StartAnime( LYT_WK_ID_LOWER, LA_ISLAND_TOP_ZUKAN_MIN_LOW_003__CURSOR_KEEP );  // 第一引数がwk_1
  }

  // 最初に指すポケモン
  m_cursorVector = convertButtonIdToCursorVector(m_param.contents->GetCurrIndexOfDispPokeDataIndexList() +BUTTON_ID_POKE_START);  // 起動画面から来たときは、svZukanDataの閲覧中の図鑑モードが変わっていなかったらdefaultMonsNoを使うべきか？
}

void ZukanViewIslandTop::setCurrDispPoke(void)
{ 
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();

  // アローラ種、方角のいるポケモンに関してはフォルム番号を変更する
  // createPage関数内でアイコンを表示する際にやっているので、この関数内ではフォルム番号の変更はやらなくてよい。

  PokeTool::SimpleParam sp = m_param.contents->GetCurrDispPokeSimpleParam();

  // 捕まえた、見たフラグ
  s32 getSeeAnimFrame = 0;
  if(pokeData->getFlag) getSeeAnimFrame = 2;
  else if(pokeData->figureSeeNum>0) getSeeAnimFrame = 1;
  ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_ISLAND_TOP_ZUKAN_ISD_UPP_000_STATUS, getSeeAnimFrame);

  // テキスト
  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    // 図鑑番号、ポケモン種族名
    pG2DUtil->SetTextBoxPaneNumber(m_pokeNoTextPane, msg_zkn_03_01, pokeData->alolaNo, 3, 0, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU);
    if(pokeData->figureSeeNum>0)
    {
      pG2DUtil->SetRegisterMonsNameNo(0, sp.monsNo);
      pG2DUtil->SetTextBoxPaneStringExpand(m_pokeNameTextPane, msg_zkn_03_03);
    }
    else
    {
      pG2DUtil->SetTextBoxPaneStringExpand(m_pokeNameTextPane, msg_zkn_08_03);
    }
  }
  pG2DUtil->SetMessageData(NULL, NULL);

  // ポケモンモデルの背景
  m_pokeBgPane->SetVisible(pokeData->figureSeeNum>0);

  // ポケモンモデル
  if(m_dispState == DISP_STATE_ON_WORK)
  {
    //m_param.tool->SetPokeModelVisible(pokeData->figureSeeNum>0);
    ZukanUtil::SetPokeModelVisible(m_param.tool, pokeData->figureSeeNum>0);
  }
  m_param.tool->ChangePokeModel(m_param.contents->GetCurrDispPokeSimpleParam());

  // 島マップ
  ZukanSmallIslandMap::HabitatType habitatType = m_smallIslandMap->SetPokeData( m_param.contents->GetInfoType(), sp.monsNo, (pokeData->figureSeeNum>0) );

  // 生息地不明
  s32 whereAnimFrame = 0;
  if(habitatType != ZukanSmallIslandMap::HABITAT_TYPE_EXIST && habitatType != ZukanSmallIslandMap::HABITAT_TYPE_NOT_SEE)  // 対象のポケモンを見ていて生息地不明のとき
  {
    whereAnimFrame = 1;
  
    pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
    {
      if(habitatType == ZukanSmallIslandMap::HABITAT_TYPE_UNKNOWN)
      {
        // 生息地不明
        pG2DUtil->SetTextBoxPaneString(m_whereTextPane, msg_zkn_10_01);
      }
      else  // (habitatType == ZukanSmallIslandMap::HABITAT_TYPE_OTHER)
      {
        // 他の島にいます
        pG2DUtil->SetTextBoxPaneString(m_whereTextPane, msg_zkn_10_02);
      }
    }
    pG2DUtil->SetMessageData(NULL, NULL);
  } 
  ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_ISLAND_TOP_ZUKAN_ISD_UPP_000_HABITAT, whereAnimFrame);

  // @fix[1875] 島図鑑に出入りする度にカーソルがデフォルト位置に戻ることについて  // 『@fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る』にカーソル位置記憶に関してまとめてある。
  // 島図鑑で見ているポケモンが変わったら、セーブデータが覚えているカーソル位置を変更する
  m_param.contents->SetDispPokeDataToSaveData(m_param.contents->GetCurrIndexOfDispPokeDataIndexList());
}
 
void ZukanViewIslandTop::setCursorVector(u32 button_id, bool playSEFlag, bool notSetIfSameIndexFlag)
{
  GFL_ASSERT(BUTTON_ID_POKE_START<=button_id && button_id<=BUTTON_ID_POKE_END);  // 開発中に気付かせるためのASSERT  // button_idはポケモンボタンのIDの範囲内

  bool setFlag = false;
  if( !notSetIfSameIndexFlag )
  {
    setFlag = true;
  }
  else if( notSetIfSameIndexFlag && (button_id != convertCursorVectorToButtonId(m_cursorVector)) )
  {
    setFlag = true;
  }

  if(setFlag)
  {
    m_cursorVector = convertButtonIdToCursorVector(button_id);

    // カーソル移動音
    if(playSEFlag) Sound::PlaySE( SEQ_SE_SELECT1 );

    // カーソルの表示位置を変更
    this->putCursorPane();

    // ポケモン
    m_param.contents->SetCurrIndexOfDispPokeDataIndexList(convertCursorVectorToButtonId(m_cursorVector) -BUTTON_ID_POKE_START);
    this->setCurrDispPoke();
  } 
}

void ZukanViewIslandTop::moveCursorVector(const ZukanType::CursorVector& moveDir, bool moveIsTrigger)
{
  // 縦横両方に同時に移動することはない
  // 1以上移動することはない
  // そういう前提のプログラム

  s32 startButtonId = convertCursorVectorToButtonId(m_cursorVector);
  s32 buttonNum  = m_param.contents->GetDispPokeDataIndexNum();

  s32 buttonId = startButtonId;

  if(moveDir.x != 0)
  {
    // xの移動は1列に繋がっているかのように動かす
    if(moveDir.x < 0)
    {
      if(buttonId == 0)
      {
        // 先頭のボタン
        if(moveIsTrigger)
        {
          buttonId = buttonNum -1;
        }
      }
      else
      {
        --buttonId;
      }
    }
    else  // (moveDir.x > 0)
    {
      if(buttonId == buttonNum -1)
      {
        // 最後のボタン
        if(moveIsTrigger)
        {
          buttonId = 0;
        }
      }
      else
      {
        ++buttonId;
      }
    }

    if(0<=buttonId && buttonId<buttonNum)
    {
      if(buttonId != startButtonId)
      {
        // 移動できた
        m_cursorVector = convertButtonIdToCursorVector(buttonId);
      
        // カーソル移動音
        Sound::PlaySE( SEQ_SE_SELECT1 );
      
        // カーソルの表示位置を変更
        this->putCursorPane();
      
        // ポケモン
        m_param.contents->SetCurrIndexOfDispPokeDataIndexList(convertCursorVectorToButtonId(m_cursorVector) -BUTTON_ID_POKE_START);
        this->setCurrDispPoke();
      }
    }
  }
  else if(moveDir.y != 0)
  {
    // yの移動は普通に縦横に動かす
    this->moveCursorVectorMatrix(moveDir, moveIsTrigger);
  }
}

void ZukanViewIslandTop::moveCursorVectorMatrix(const ZukanType::CursorVector& moveDir, bool moveIsTrigger)
{
  // 縦横両方に同時に移動することはない
  // 1以上移動することはない
  // そういう前提のプログラム

  ZukanType::CursorVector cursorVector = m_cursorVector;

  s32  moveDir_val           = 0;
  s32  startCursorVector_val = 0;
  s32* cursorVector_val      = NULL;
  s32  VAL_NUM_MAX           = 0;

  if(moveDir.x != 0)
  {
    // x方向
    moveDir_val           = moveDir.x;
    startCursorVector_val = m_cursorVector.x;
    cursorVector_val      = &(cursorVector.x);
    VAL_NUM_MAX           = BUTTON_ID_POKE_X_NUM;
  }
  else if(moveDir.y != 0)
  {
    // y方向
    moveDir_val           = moveDir.y;
    startCursorVector_val = m_cursorVector.y;
    cursorVector_val      = &(cursorVector.y);
    VAL_NUM_MAX           = BUTTON_ID_POKE_Y_NUM;
  }

  if(cursorVector_val)
  {
    // 移動する
    s32 prevVal = (*cursorVector_val);
    (*cursorVector_val) += moveDir_val;
    if( (*cursorVector_val) < 0 )
    {
      if(moveIsTrigger) (*cursorVector_val) = VAL_NUM_MAX -1;
      else              (*cursorVector_val) = 0;
    }
    else if( (*cursorVector_val) >= VAL_NUM_MAX )
    {
      if(moveIsTrigger) (*cursorVector_val) = 0;
      else              (*cursorVector_val) = VAL_NUM_MAX -1;
    }
    while(
           ((*cursorVector_val) != prevVal)
        && ((*cursorVector_val) != startCursorVector_val)
        && (ZukanUtil::IsButtonVisible(this->GetButtonManager(), convertCursorVectorToButtonId(cursorVector)) == false)  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
    )
    {
      prevVal = (*cursorVector_val);
      (*cursorVector_val) += moveDir_val;
      if( (*cursorVector_val) < 0 )
      {
        if(moveIsTrigger) (*cursorVector_val) = VAL_NUM_MAX -1;
        else              (*cursorVector_val) = 0;
      }
      else if( (*cursorVector_val) >= VAL_NUM_MAX )
      {
        if(moveIsTrigger) (*cursorVector_val) = 0;
        else              (*cursorVector_val) = VAL_NUM_MAX -1;
      }
    }

    // 移動できたか
    if(
           ((*cursorVector_val) != startCursorVector_val)
        && (ZukanUtil::IsButtonVisible(this->GetButtonManager(), convertCursorVectorToButtonId(cursorVector)) != false)
    )
    {
      // 移動できた
      m_cursorVector = cursorVector;

      // カーソル移動音
      Sound::PlaySE( SEQ_SE_SELECT1 );

      // カーソルの表示位置を変更
      this->putCursorPane();
  
      // ポケモン
      m_param.contents->SetCurrIndexOfDispPokeDataIndexList(convertCursorVectorToButtonId(m_cursorVector) -BUTTON_ID_POKE_START);
      this->setCurrDispPoke();
    }
  }
}

void ZukanViewIslandTop::putCursorPane(void)
{
  if(m_cursorPane->IsVisible() == false) m_cursorPane->SetVisible(true);

  gfl2::math::VEC3 pos = m_buttonPaneInfo[convertCursorVectorToButtonId(m_cursorVector)].partsPane->GetTranslate();
  pos.y += 6;
  m_cursorPane->SetTranslate(pos);
}

ZukanType::CursorVector ZukanViewIslandTop::convertButtonIdToCursorVector(u32 button_id)  // button_idはポケモンボタンのIDの範囲内
{
  u32 l_button_id = button_id -BUTTON_ID_POKE_START;

  ZukanType::CursorVector cursorVector(0,0);
  cursorVector.x = l_button_id % BUTTON_ID_POKE_X_NUM;  // xyこれで合ってます
  cursorVector.y = l_button_id / BUTTON_ID_POKE_X_NUM;
  if( !( 0<=cursorVector.x && cursorVector.x<BUTTON_ID_POKE_X_NUM ) ) {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    cursorVector.x = 0;
  }
  if( !( 0<=cursorVector.y && cursorVector.y<BUTTON_ID_POKE_Y_NUM ) )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    cursorVector.y = 0;
  }
  return cursorVector;
}
u32 ZukanViewIslandTop::convertCursorVectorToButtonId(const ZukanType::CursorVector& cursorVector)  // 戻り値はポケモンボタンのIDの範囲内
{
  u32 button_id = cursorVector.y*BUTTON_ID_POKE_X_NUM + cursorVector.x +BUTTON_ID_POKE_START;  // xyこれで合ってます
  if( !( BUTTON_ID_POKE_START<=button_id && button_id<=BUTTON_ID_POKE_END ) )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    button_id = BUTTON_ID_POKE_START;
  }
  return button_id;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

