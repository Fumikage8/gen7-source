//==============================================================================
/**
 @file    ZukanViewMap.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の分布画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <App/TownMapParts/include/TownMapPartsDraw.h>
#include <App/TownMap/include/TownMapDef.h>

#include <Field/FieldStatic/include/Zone/ZoneDataAccessor.h>
#include <Field/FieldStatic/include/Zone/ZoneDataLoader.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/map.h>
#include <niji_conv_header/app/zukan/map_pane.h>
#include <niji_conv_header/app/zukan/map_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

#include <niji_conv_header/field/zone/zone_id.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanContents.h"
#include "ZukanUtil.h"
#include "ZukanFormOrder.h"
#include "ZukanPokeIconTextureManager.h"
#include "ZukanDistributionPokeDataAccessor.h"
#include "ZukanDistributionPositionDataAccessor.h"
#include "ZukanDistributionZoneDataAccessor.h"
#include "ZukanFrameMap.h"

#include "ZukanViewMap.h"

#include "ZukanDebugWin.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewMapListener
 @brief     図鑑の分布画面のビューのリスナー
 */
//==============================================================================
class ZukanViewMapListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewMapListener);

public:
  ZukanViewMapListener(ZukanViewMap* view);
  virtual ~ZukanViewMapListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );  // @fix GFNMCat[4317] 図鑑：分布画面でタッチ中にXボタンが反応する
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewMap*    m_view;
  InputState       m_inputState;
};


//==============================================================================
//
//  ZukanViewMapListener
//
//==============================================================================
ZukanViewMapListener::ZukanViewMapListener(ZukanViewMap* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewMapListener::~ZukanViewMapListener()
{}

app::ui::UIInputListener::ListenerResult ZukanViewMapListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )  // @fix GFNMCat[4317] 図鑑：分布画面でタッチ中にXボタンが反応する
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
        if( isTouch )  // タッチ中にキー入力を効かなくしたいだけなので特に何もしない。
        {}
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
app::ui::UIInputListener::ListenerResult ZukanViewMapListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
        /*
        if( pButton->IsTrigger(gfl2::ui::BUTTON_L) )
        {
          m_view->Req(ZukanViewMap::REQ_ID_KEY_TRIGGER_LEFT);
        }
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_R) )
        {
          m_view->Req(ZukanViewMap::REQ_ID_KEY_TRIGGER_RIGHT);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_L) )
        {
          m_view->Req(ZukanViewMap::REQ_ID_KEY_REPEAT_LEFT);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_R) )
        {
          m_view->Req(ZukanViewMap::REQ_ID_KEY_REPEAT_RIGHT);
        }
        */
        if( pButton->IsTrigger(gfl2::ui::BUTTON_X) )
        {
          m_view->Req(ZukanViewMap::REQ_ID_KEY_TRIGGER_X);
        }
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}

app::ui::UIInputListener::ListenerResult ZukanViewMapListener::OnLayoutPaneEvent( const u32 painId )
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
      m_view->Req(ZukanViewMap::REQ_ID_BUTTON_DECIDE, button_id);
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}

void ZukanViewMapListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
  
  app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
}




//==============================================================================
//
//  ZukanViewMap
//
//==============================================================================
ZukanViewMap::ZukanViewMap(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_townMapPartsDraw(NULL),
    m_viewListener(NULL),
    m_placeNameStrBuf(NULL),
    m_islandNameTextPane(NULL),
    //m_placeNameTextPane(NULL),  // 地名表示なくなりました
    m_unknownHabitatTextPane(NULL),
    m_xButtonPane(NULL),
    m_pokeIconPicPane(NULL),
    m_pokeIconTexManager(NULL),
    m_islandId(BUTTON_ID_ISLAND1),
    m_distributionPokeDataAccessor(NULL),
    m_distributionPositionDataAccessor(NULL),
    m_distributionZoneDataAccessor(NULL),
    m_townMapIconData(NULL),
    m_townMapIconDataPtr(NULL),
    m_townMapIconDataNum(0),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_resetIconPositionCallFlag(false)
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
    , m_debugIconDividedNo(0),
    m_debugIconDividedAnimStop(false)
#endif
{
  // 初期化
  {   
    for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
    {
      m_waitAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }
  }

  // 場所名StrBuf
  m_placeNameStrBuf = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf(512, m_param.appHeap->GetDeviceHeap());  // 街名として十分大きい文字数

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

  // 最初の島番号
  m_islandId = GetFirstIslandNo(m_param.contents);

  // ポケアイコン
  m_pokeIconTexManager = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPokeIconTextureManager(
      m_param.appHeap, m_param.dataManager, 1 );

  // タウンマップ
  m_townMapIconData = GFL_NEW_ARRAY(m_param.appHeap->GetDeviceHeap()) App::TownMap::TownMapPartsDraw::EX_ICON_DATA[TOWN_MAP_ICON_DATA_NUM_MAX];

  // ZukanViewMapListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewMapListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_MAP);
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
          LYTRES_MAP_ZUKAN_DSB_UPP_000_BFLYT,
          LA_MAP_ZUKAN_DSB_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
     
        // wk_1
        {
          // 下画面
          0,  // res_0
          LYTRES_MAP_ZUKAN_MIN_LOW_005_BFLYT,
          LA_MAP_ZUKAN_MIN_LOW_005___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          2,  // wk_0, wk_1の2個
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );
   
      // ボタンマネージャ
      this->createButtonManager();

      // ページ内容生成
      this->createPage();

      this->setCurrDispPoke();

      this->startInSeq();
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }
 
  // 入力受け付け
  m_viewListener->SetInputState(ZukanViewMapListener::INPUT_STATE_ENABLE);
}
ZukanViewMap::~ZukanViewMap()
{
  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // ボタンマネージャ
  this->destroyButtonManager();

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewMapListener
  GFL_SAFE_DELETE(m_viewListener);

  // タウンマップ
  GFL_SAFE_DELETE_ARRAY(m_townMapIconData);

  // ポケアイコン
  GFL_SAFE_DELETE(m_pokeIconTexManager);

  // distributionクラス
  GFL_SAFE_DELETE(m_distributionZoneDataAccessor);
  GFL_SAFE_DELETE(m_distributionPositionDataAccessor);
  GFL_SAFE_DELETE(m_distributionPokeDataAccessor);

  // 場所名StrBuf
  GFL_SAFE_DELETE(m_placeNameStrBuf);
}

void ZukanViewMap::Update(void)
{
  GameSys::GameData*           gameData       = m_param.gameManager->GetGameData();
  const Field::ZoneDataLoader* zoneDataLoader = gameData->GetFieldZoneDataLoader();

  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  
  if(m_townMapPartsDraw)
  {
    // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →一瞬違うアイコンが表示されるので、ResetIconをResetIcon(アニメを1フレーム進める)とResetIconPosition(表示する)に分けてもらった。
    if( m_resetIconPositionCallFlag )
    {
      // アイコンを表示する
      m_townMapPartsDraw->ResetIconPosition();
      m_resetIconPositionCallFlag = false;
    }

#if PM_DEBUG
    {
      /*  DebugWindow表示のためにコール  */
      m_townMapPartsDraw->GetCursorPosZoneID();
    }
#endif

    /*
    地名表示なくなりました
    ZoneID zoneID = m_townMapPartsDraw->GetCursorPosZoneID();
    if(zoneID == ZONE_ID_MAX)
    {
      m_placeNameTextPane->SetVisible(false);  
    }
    else
    {
      // 地名
      Field::ZoneDataAccessor zoneDataAccessor( zoneDataLoader->GetZoneData(zoneID) );
      u32 placeLabelID = zoneDataAccessor.GetPlaceLabelID();

      ZukanTool::ExpandStrBufParam expandStrBufParam;
      {
        expandStrBufParam.dstStrBuf         = m_placeNameStrBuf;
        expandStrBufParam.srcMsgType        = ZukanTool::MSG_TYPE_ZUKAN;
        expandStrBufParam.srcMsgStrId       = msg_zkn_05_01;
        expandStrBufParam.srcRegBufType[0]  = ZukanTool::REG_BUF_TYPE_PLACE_NAME;
        expandStrBufParam.srcPlaceNameID[0] = placeLabelID +1;
      }
      m_param.tool->ExpandStrBuf(expandStrBufParam);  // WordSetLoaderで地名はPRELOADしている

      pG2DUtil->SetTextBoxPaneString(m_placeNameTextPane, m_placeNameStrBuf);   
 
      m_placeNameTextPane->SetVisible(true);  
    }
    */

    // タウンマップのボタンのアニメ中（ボタンが決定を返す前）に、このViewが入力できないようにしておく。
    if(m_townMapPartsDraw->GetButtonManager())
    {
      if( m_townMapPartsDraw->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )
      {
        m_viewListener->SetInputState(ZukanViewMapListener::INPUT_STATE_ENABLE);
      }
      else
      {
        m_viewListener->SetInputState(ZukanViewMapListener::INPUT_STATE_OCCUPY);
      }
    }
  }


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
        ZukanUtil::SetPokeModelVisible(m_param.tool, true);

        m_param.rootView->SetInputEnabledAllView(true);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
        this->changeDispState(DISP_STATE_ON_WORK);
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
      s32 mapIconCheckMode = 0;
#if PM_DEBUG
      mapIconCheckMode = ZukanDebugWin::GetMapIconCheckMode();
#endif
      if(mapIconCheckMode != 0)  // デバッグモードがONになったときだけ毎フレーム呼ぶ。
      {
        // デバッグ用
        this->SetTownMapPartsDrawIconData(m_townMapPartsDraw->IsZoom(), mapIconCheckMode);
        m_townMapPartsDraw->ResetIcon();  // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →SetExIconDataと「ResetIcon,ResetIconPosition」を分けたので、必要なところでは明示的に呼ぶ。
        m_townMapPartsDraw->ResetIconPosition();
      }
#endif
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        this->changeDispState(DISP_STATE_WAIT_END);
        m_param.frameCell->Req(ZukanFrameMapCell::REQ_ID_END);
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
void ZukanViewMap::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_LOWER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_1
}

void ZukanViewMap::SetTownMapPartsDraw(App::TownMap::TownMapPartsDraw* townMapPartsDraw)
{
  m_townMapPartsDraw = townMapPartsDraw;
  this->setCurrDispIsland();

  // m_townMapPartsDrawを設定したとき、今のポケモンの分布を反映しなければならないので、外部から明示的にResetIconを呼んでやる。
  if(m_townMapPartsDraw)
  {
    m_townMapPartsDraw->ResetIcon();  // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →SetExIconDataと「ResetIcon,ResetIconPosition」を分けたので、必要なところでは明示的に呼ぶ。
    m_resetIconPositionCallFlag = true;  // 次のフレームで表示する
  }
}

void ZukanViewMap::SetIslandButtonVisible(bool isVisible)
{
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ

  for(u32 i=BUTTON_ID_ISLAND1; i<=BUTTON_ID_ISLAND4; ++i)
  {
    if(m_islandId != static_cast<s32>(i))
    {
      ZukanUtil::SetButtonVisible(buttonManager, i, isVisible);
    }
  }

  m_xButtonPane->SetVisible(isVisible);  // @fix NMCat[1556] 拡大マップでは反応しないXボタンが表示されている  // 拡大/縮小に合わせてXボタンのオフ/オンを切り替える
}

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
void ZukanViewMap::SetTownMapPartsDrawIconData(bool isZoom, s32 mapIconCheckMode)
{
  if(mapIconCheckMode == 0)
  {
    this->SetTownMapPartsDrawIconData(isZoom);
    return;
  }

  APP_ZUKAN_DISTRIBUTION_ZONE_DATA_BIT_FLAG_DECLARATION(zoneDataBitFlag);

  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  // 生息地不明を非表示
  ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, LA_MAP_ZUKAN_MIN_LOW_005_HABITAT, 0);

  // タウンマップに表示するアイコン
  m_townMapIconDataPtr = m_townMapIconData;
  m_townMapIconDataNum = 0;

  // 操作
  if(isZoom)
  {
    const gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    if(button->IsTrigger(gfl2::ui::BUTTON_X))
    {
      if( m_debugIconDividedAnimStop )
      {
        ++m_debugIconDividedNo;
      }
      else
      {
        m_debugIconDividedAnimStop = true;
      }
    }
  }
  else
  {
    m_debugIconDividedAnimStop = false;
  }

  if(isZoom)
  {
    /////////////////////////////////
    //
    // 配置IDのデータを全て表示する
    //
    /////////////////////////////////

    // (1) アイコンの総数を数える
    u32 placeIdDataNum = m_distributionPositionDataAccessor->GetPlaceIdDataNum();
   
    u32 islandIconNum = 0;
    u32 islandPlaceIdNum = 0;
    for(u32 placeId=0; placeId<placeIdDataNum; ++placeId)
    {
      ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
      m_distributionPositionDataAccessor->GetPlaceIdData(placeId, &placeIdData);
       
      if((static_cast<s32>(placeIdData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり
      {
        ++islandPlaceIdNum;
        for(u32 j=0; j<placeIdData.posDataNum; ++j)
        {
          ++islandIconNum;
        }
      }
    }

    // (2) アイコンを何回に分けて表示するか
    u32 dividedNum = islandIconNum / TOWN_MAP_ICON_DATA_NUM_MAX;
    if( islandIconNum % TOWN_MAP_ICON_DATA_NUM_MAX != 0 ) ++dividedNum;

    // (3) 何番目の分割か
    if( !m_debugIconDividedAnimStop )
    {
      ++m_debugIconDividedNo;
    }
    if( m_debugIconDividedNo >= dividedNum ) m_debugIconDividedNo = 0;

    // (4) 情報出力
    if( m_debugIconDividedNo == 0 )
    {
      GFL_RELEASE_PRINT("[placeId] island %d: icon total number %d, placeId number = %d\n", m_islandId +1, islandIconNum, islandPlaceIdNum);  // 島番号は1始まり、m_islandIdは0始まり
    }
    if( m_debugIconDividedAnimStop )
    {
      GFL_RELEASE_PRINT("[placeId] island %d: icon No %d -\n", m_islandId +1, TOWN_MAP_ICON_DATA_NUM_MAX*m_debugIconDividedNo);  // 島番号は1始まり、m_islandIdは0始まり
    }

    // (5) 今回の分割分のアイコンを表示する
    u32 islandIconNo = 0;
    for(u32 placeId=0; placeId<placeIdDataNum; ++placeId)
    {
      ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
      m_distributionPositionDataAccessor->GetPlaceIdData(placeId, &placeIdData);
       
      if((static_cast<s32>(placeIdData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり
      {
        for(u32 j=0; j<placeIdData.posDataNum; ++j)
        {
          ++islandIconNo;
          if( TOWN_MAP_ICON_DATA_NUM_MAX*m_debugIconDividedNo <= islandIconNo && islandIconNo < TOWN_MAP_ICON_DATA_NUM_MAX*(m_debugIconDividedNo+1) )
          {
            if( m_townMapIconDataNum < TOWN_MAP_ICON_DATA_NUM_MAX)
            {
              m_townMapIconData[m_townMapIconDataNum].type  = App::TownMap::PLACE_TYPE_MONS;
              m_townMapIconData[m_townMapIconDataNum].param = placeId;  // 使っていないが、placeIdでも入れておく
              m_townMapIconData[m_townMapIconDataNum].x     = placeIdData.posData[j].intX /ZukanDistributionPositionDataAccessor::POS_SCALL_VAL;
              m_townMapIconData[m_townMapIconDataNum].y     = placeIdData.posData[j].intZ /ZukanDistributionPositionDataAccessor::POS_SCALL_VAL;
              ++m_townMapIconDataNum;
            }
            else
            {
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。起こり得ないはず。
            }
          }
        }
      }
    }
  }
  else
  {
    /////////////////////////////////
    //
    // ゾーンデータを全て表示する
    //
    /////////////////////////////////

    // (1) アイコンの総数を数える
    u32 zoneDataNum = m_distributionZoneDataAccessor->GetZoneDataNum();
   
    u32 islandIconNum = 0;
    for(u32 zoneDataNo=0; zoneDataNo<zoneDataNum; ++zoneDataNo)
    {
      ZukanDistributionZoneDataAccessor::ZoneData zoneData;
      m_distributionZoneDataAccessor->GetZoneData(zoneDataNo, &zoneData);
       
      if((static_cast<s32>(zoneData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり
      {
        ++islandIconNum;
      }
    }

    // (2) アイコンを何回に分けて表示するか
    u32 dividedNum = islandIconNum / TOWN_MAP_ICON_DATA_NUM_MAX;
    if( islandIconNum % TOWN_MAP_ICON_DATA_NUM_MAX != 0 ) ++dividedNum;

    // (3) 何番目の分割か
    ++m_debugIconDividedNo;
    if( m_debugIconDividedNo >= dividedNum ) m_debugIconDividedNo = 0;

    // (4) 情報出力
    if( m_debugIconDividedNo == 0 )
    {
      GFL_RELEASE_PRINT("[zoneData] island %d: icon total number %d\n", m_islandId +1, islandIconNum);  // 島番号は1始まり、m_islandIdは0始まり
    }

    // (5) 今回の分割分のアイコンを表示する
    u32 islandIconNo = 0;
    for(u32 zoneDataNo=0; zoneDataNo<zoneDataNum; ++zoneDataNo)
    {
      ZukanDistributionZoneDataAccessor::ZoneData zoneData;
      m_distributionZoneDataAccessor->GetZoneData(zoneDataNo, &zoneData);
       
      if((static_cast<s32>(zoneData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり
      {
        ++islandIconNo;
        if( TOWN_MAP_ICON_DATA_NUM_MAX*m_debugIconDividedNo <= islandIconNo && islandIconNo < TOWN_MAP_ICON_DATA_NUM_MAX*(m_debugIconDividedNo+1) )
        {
          if( m_townMapIconDataNum < TOWN_MAP_ICON_DATA_NUM_MAX)
          {
            m_townMapIconData[m_townMapIconDataNum].type  = App::TownMap::PLACE_TYPE_MONS;
            m_townMapIconData[m_townMapIconDataNum].param = zoneDataNo;  // 使っていないが、zoneDataNoでも入れておく
            m_townMapIconData[m_townMapIconDataNum].x     = zoneData.intX /ZukanDistributionZoneDataAccessor::POS_SCALL_VAL;
            m_townMapIconData[m_townMapIconDataNum].y     = zoneData.intZ /ZukanDistributionZoneDataAccessor::POS_SCALL_VAL;
            ++m_townMapIconDataNum;
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。起こり得ないはず。
          }
        }
      }
    }
  }

  // タウンマップ
  if(m_townMapPartsDraw)
  {
    m_townMapPartsDraw->SetExIconData(m_townMapIconDataPtr, m_townMapIconDataNum);
    //m_townMapPartsDraw->ResetIcon();  // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →SetExIconDataと「ResetIcon,ResetIconPosition」を分けたので、ここはコメントアウト。
    // この関数はデバッグ用の関数なので m_resetIconPositionCallFlag = false; はやらない。
  }
}
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE

void ZukanViewMap::SetTownMapPartsDrawIconData(bool isZoom)
{
  APP_ZUKAN_DISTRIBUTION_ZONE_DATA_BIT_FLAG_DECLARATION(zoneDataBitFlag);
  
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  
  // ポケモンごとの分布データ
  ZukanDistributionPokeDataAccessor::PokeDistributionData pokeDistData;
  m_distributionPokeDataAccessor->GetPokeDistributionData(m_param.contents->GetCurrDispPokeSimpleParam().monsNo, &pokeDistData);

  // 今の島に生息しているか
  bool currIslandFlag  = false;  // 今の島に生息しているときはtrue  // 今の島にも他の島にも生息しているときはどちらもtrueになる
  bool otherIslandFlag = false;  // 他の島に生息しているときはtrue  // 今の島に生息しているときは使用しないのでそのときは間違った値が入っているかもしれない
  if(pokeDistData.placeIdDataNum > 0)
  {
    for(u32 i=0; i<pokeDistData.placeIdDataNum; ++i)
    {
      ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
      m_distributionPositionDataAccessor->GetPlaceIdData(pokeDistData.placeIdData[i].placeId, &placeIdData);
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
      if( ((static_cast<s32>(placeIdData.islandNo)) == m_islandId +1) || ((static_cast<s32>(placeIdData.islandNo))==0&&m_islandId==0) ) // 島番号は1始まり、m_islandIdは0始まり  // 開発中はデータにまだ正しい島番号が入っていないので。正しいデータになったら下行に変える。
#else
      if((static_cast<s32>(placeIdData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり
#endif
      {
        currIslandFlag  = true;
        break;
      }
      else
      {
        if((static_cast<s32>(placeIdData.islandNo)) != 0)  // 島番号は1始まり、0は無効なデータ  // @fix GFNMCat[4769] 図鑑：無効な分布データしかないときに「他の島にいます」と出る可能性がある
        {
          otherIslandFlag = true;
        }
      }
    }
  }

  if(currIslandFlag)
  {
    // 生息地不明を非表示
    //m_unknownHabitatTextPane->SetVisible(false);
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, LA_MAP_ZUKAN_MIN_LOW_005_HABITAT, 0);
   
    if(isZoom)
    {
      // タウンマップに表示するアイコン
      m_townMapIconDataPtr = m_townMapIconData;
      m_townMapIconDataNum = 0;
      for(u32 i=0; i<pokeDistData.placeIdDataNum; ++i)
      {
        ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
        m_distributionPositionDataAccessor->GetPlaceIdData(pokeDistData.placeIdData[i].placeId, &placeIdData);
          
        if((static_cast<s32>(placeIdData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり  // @fix NMCat[1586] 島図鑑画面と分布画面でエンカウントマークの表示位置が異なる  →選択中の島の分しか出さないようにする
        {
          for(u32 j=0; j<placeIdData.posDataNum; ++j)
          {
            if( m_townMapIconDataNum < TOWN_MAP_ICON_DATA_NUM_MAX)
            {
              m_townMapIconData[m_townMapIconDataNum].type  = App::TownMap::PLACE_TYPE_MONS;
              m_townMapIconData[m_townMapIconDataNum].param = pokeDistData.placeIdData[i].placeId;  // 使っていないが、placeIdでも入れておく
              m_townMapIconData[m_townMapIconDataNum].x     = placeIdData.posData[j].intX /ZukanDistributionPositionDataAccessor::POS_SCALL_VAL;
              m_townMapIconData[m_townMapIconDataNum].y     = placeIdData.posData[j].intZ /ZukanDistributionPositionDataAccessor::POS_SCALL_VAL;
              {
                bool markDayNormal   = false;
                bool markNightNormal = false;
                bool markFishing     = false;
                bool markDayEnter    = false;
                bool markNightEnter  = false;
                // 通常エンカウント
                if( ZukanDistributionPokeDataAccessor::IsAppearanceDay(&(pokeDistData.placeIdData[i])) )
                {
                  markDayNormal = true;
                }
                if( ZukanDistributionPokeDataAccessor::IsAppearanceNight(&(pokeDistData.placeIdData[i])) )
                {
                  markNightNormal = true;
                }
                // 乱入
                if( ZukanDistributionPokeDataAccessor::IsAppearanceDayEnter(&(pokeDistData.placeIdData[i])) )
                {
                  markDayEnter = true;
                }
                if( ZukanDistributionPokeDataAccessor::IsAppearanceNightEnter(&(pokeDistData.placeIdData[i])) )
                {
                  markNightEnter = true;
                }
                // 釣り
                if(placeIdData.encountType == ZukanDistributionPositionDataAccessor::ENCOUNT_TYPE_FISHING)
                {
                  markFishing = true;
                }
                GFL_PRINT("[%d](%f,%f)Day=%d,Night=%d,Fishing=%d\n", m_townMapIconDataNum, m_townMapIconData[m_townMapIconDataNum].x, m_townMapIconData[m_townMapIconDataNum].y,
                    markDayNormal, markNightNormal, markFishing );
         
                App::TownMap::PlaceType placeType = App::TownMap::PLACE_TYPE_MONS;
                if(markFishing)
                {
                  placeType = App::TownMap::PLACE_TYPE_FISH;
                  if(markDayNormal)
                  {
                    if(!markNightNormal)
                    {
                      placeType = App::TownMap::PLACE_TYPE_FISH_A;
                    }
                  }
                  else if(markNightNormal)
                  {                 
                    placeType = App::TownMap::PLACE_TYPE_FISH_E;
                  }
                  else if(markDayEnter)
                  {
                    if(markNightEnter)
                    {
                      placeType = App::TownMap::PLACE_TYPE_FISH_ENTER;
                    }
                    else
                    {
                      placeType = App::TownMap::PLACE_TYPE_FISH_ENTER_A;
                    }
                  }
                  else if(markNightEnter)
                  {
                    placeType = App::TownMap::PLACE_TYPE_FISH_ENTER_E;
                  }
                }
                else
                {
                  if(markDayNormal)
                  {
                    if(!markNightNormal)
                    {
                      placeType = App::TownMap::PLACE_TYPE_MONS_A;
                    }
                  }
                  else if(markNightNormal)
                  {
                    placeType = App::TownMap::PLACE_TYPE_MONS_E;
                  }
                  else if(markDayEnter)
                  {
                    if(markNightEnter)
                    {
                      placeType = App::TownMap::PLACE_TYPE_MONS_ENTER;
                    }
                    else
                    {
                      placeType = App::TownMap::PLACE_TYPE_MONS_ENTER_A;
                    }
                  }
                  else if(markNightEnter)
                  {
                    placeType = App::TownMap::PLACE_TYPE_MONS_ENTER_E;
                  }
                }
                m_townMapIconData[m_townMapIconDataNum].type  = placeType;
              }
              ++m_townMapIconDataNum;
            }
            else
            {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
              GFL_PRINT("WARNING(ZukanViewMap.cpp): m_townMapIconDataNum=%d\n", m_townMapIconDataNum);
#else    
              GFL_RELEASE_PRINT("#########################################\n");
              GFL_RELEASE_PRINT("WARNING: iconDataNum Over: iconDataNum=%d\n", m_townMapIconDataNum);
              GFL_RELEASE_PRINT("#########################################\n");
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。分布用のアイコンの個数が最大数を越えた。  // 開発中は最大数を越えることがあるのでdefine分岐。仮データならいいが、そうでないなら分布をまとめるか。
#endif 
              break;
            }
          }
        }
      }
    }
    else
    {
      // 配置ID→ゾーンデータ
      for(u32 i=0; i<pokeDistData.placeIdDataNum; ++i)
      {
        ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
        m_distributionPositionDataAccessor->GetPlaceIdData(pokeDistData.placeIdData[i].placeId, &placeIdData);
     
        // ゾーンデータ
        if( placeIdData.zoneDataNo != ZukanDistributionPositionDataAccessor::POSITION_PLACE_ID_ZONE_DATA_NO_INVALID )
        {
          if((static_cast<s32>(placeIdData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり
          {
            ZukanDistributionZoneDataAccessor::ZoneDataBitFlagSetOn(zoneDataBitFlag, placeIdData.zoneDataNo);
          }
        }
        else
        {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT==0
          GFL_RELEASE_PRINT("########################################################################\n");
          GFL_RELEASE_PRINT("WARNING: zoneDataNo INVALID: placeId=%d: islandNo=%d, zoneDataNo=INVALID\n", pokeDistData.placeIdData[i].placeId, placeIdData.islandNo);
          GFL_RELEASE_PRINT("########################################################################\n");
          if((static_cast<s32>(placeIdData.islandNo))!=0)  // 島番号は1始まり。
          {
            // 島番号に値が入っているときにゾーンデータ番号が不明だったらASSERTにする。島番号に値が入っていなかったら(0だったら)テスト用の場所だろうから無視していい。
            GFL_ASSERT(0);  // 開発中に気付かせるASSERT。ゾーンデータ番号が不明。  // 開発中はデータがまだ正しくないので今だけdefine分岐しておく。
          }
#endif
        }
      }
      // タウンマップに表示するアイコン
      m_townMapIconDataPtr = m_townMapIconData;
      m_townMapIconDataNum = 0;
      for(u32 zoneDataNo=0; zoneDataNo<ZukanDistributionZoneDataAccessor::ZONE_DATA_NUM_MAX; ++zoneDataNo)
      {
        if( ZukanDistributionZoneDataAccessor::ZoneDataBitFlagIsOn(zoneDataBitFlag, zoneDataNo) )
        {
          if( m_townMapIconDataNum < TOWN_MAP_ICON_DATA_NUM_MAX)
          {
            ZukanDistributionZoneDataAccessor::ZoneData zoneData;
            m_distributionZoneDataAccessor->GetZoneData(zoneDataNo, &zoneData);

            if((static_cast<s32>(zoneData.islandNo)) == m_islandId +1)  // 島番号は1始まり、m_islandIdは0始まり  // zoneDataBitFlagを立てるときに島番号をチェックしているが、念のためここでもチェックしておく。
            {
              m_townMapIconData[m_townMapIconDataNum].type  = App::TownMap::PLACE_TYPE_MONS;
              m_townMapIconData[m_townMapIconDataNum].param = zoneDataNo;  // 使っていないが、zoneDataNoでも入れておく
              m_townMapIconData[m_townMapIconDataNum].x     = zoneData.intX /ZukanDistributionZoneDataAccessor::POS_SCALL_VAL;
              m_townMapIconData[m_townMapIconDataNum].y     = zoneData.intZ /ZukanDistributionZoneDataAccessor::POS_SCALL_VAL;
              ++m_townMapIconDataNum;
            }
            else
            {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
              GFL_PRINT("WARNING(ZukanViewMap.cpp): place's islandNo=%d, zone's islandNo=%d\n", m_islandId +1, zoneData.islandNo);
#else
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。配置IDの島番号とゾーンデータの島番号が一致していない。  // 開発中は一致していなくても通す。
#endif
            }
          }
          else
          {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
            GFL_PRINT("WARNING(ZukanViewMap.cpp): m_townMapIconDataNum=%d\n", m_townMapIconDataNum);
#else
            GFL_RELEASE_PRINT("#########################################\n");
            GFL_RELEASE_PRINT("WARNING: iconDataNum Over: iconDataNum=%d\n", m_townMapIconDataNum);
            GFL_RELEASE_PRINT("#########################################\n");
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。分布用のアイコンの個数が最大数を越えた。  // 開発中は最大数を越えることがあるのでdefine分岐。仮データならいいが、そうでないなら分布をまとめるか。
#endif
            break;
          }
        }
      }
    }
  }
  else
  {
    // 生息地不明を表示
    //m_unknownHabitatTextPane->SetVisible(true);
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, LA_MAP_ZUKAN_MIN_LOW_005_HABITAT, 1);
    
    pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
    {
      if(otherIslandFlag)
      {
        // 他のところにいるよ
        pG2DUtil->SetTextBoxPaneString(m_unknownHabitatTextPane, msg_zkn_10_02);
      }
      else
      {
        // 生息地不明
        pG2DUtil->SetTextBoxPaneString(m_unknownHabitatTextPane, msg_zkn_10_01);
      }
    }
    pG2DUtil->SetMessageData(NULL, NULL);

    // タウンマップに表示するアイコン
    m_townMapIconDataPtr = NULL;
    m_townMapIconDataNum = 0;
  }

  // タウンマップ
  if(m_townMapPartsDraw)
  {
    m_townMapPartsDraw->SetExIconData(m_townMapIconDataPtr, m_townMapIconDataNum);
    //m_townMapPartsDraw->ResetIcon();  // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →SetExIconDataと「ResetIcon,ResetIconPosition」を分けたので、ここはコメントアウト。
    m_resetIconPositionCallFlag = false;  // 次のフレームで表示するか否かは呼び出し元に任せる
  }
}

void ZukanViewMap::ReqEnd(void)
{
  //m_param.frameCell->Req(ZukanFrameMapCell::REQ_ID_END);
  if(m_param.prevSceneType == ZukanType::SCENE_TYPE_INFO)
  {
    *m_param.nextSceneType = ZukanType::SCENE_TYPE_INFO;
  }
  //else
  //{
  //  *m_param.nextSceneType = ZukanType::SCENE_TYPE_DISCOVER;
  //}

  this->startOutSeq();
}

s32 ZukanViewMap::GetFirstIslandNo(const ZukanContents* contents)
{
  s32 islandNo = 0;
  // 最初の島番号
  {
    if(contents->GetInfoType() == ZukanType::INFO_TYPE_ALOLA)
    {
      // アローラ図鑑のときは先頭の島を最初に表示する
      islandNo = ZukanType::INFO_TYPE_ISLAND1 - ZukanType::INFO_TYPE_ISLAND_START;
    }
    else
    {
      // 島図鑑のときはその島を最初に表示する
      islandNo = contents->GetInfoType() - ZukanType::INFO_TYPE_ISLAND_START;
    }
  }
  return islandNo;
}

void ZukanViewMap::Req(ReqId reqId, s32 reqParam)
{
  switch(reqId)
  {
  /*
  case REQ_ID_KEY_TRIGGER_LEFT:
    m_islandId = (m_islandId +4 -1)%4;
    this->setCurrDispIsland();
    break;
  case REQ_ID_KEY_TRIGGER_RIGHT:
    m_islandId = (m_islandId +1)%4;
    this->setCurrDispIsland();
    break;
  case REQ_ID_KEY_REPEAT_LEFT:
    if(m_islandId > 0)
    {
      m_islandId = (m_islandId -1)%4;
      this->setCurrDispIsland();
    }
    break;
  case REQ_ID_KEY_REPEAT_RIGHT:
    if(m_islandId < 4 -1)
    {
      m_islandId = (m_islandId +1)%4;
      this->setCurrDispIsland();
    }
    break;
  */
  case REQ_ID_KEY_TRIGGER_X:
    {
      app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
      s32 nextIslandId = (m_islandId +1)%4;
      if( ZukanUtil::IsButtonVisible(buttonManager, nextIslandId) )  // 拡大中はボタンが表示されていないので変更しない。
      {
        // 入力を禁止するリクエスト。入力の復活はm_townMapPartsDrawが行ってくれる。
        if(m_townMapPartsDraw) this->SetInputEnabledAllView(false);  // 親であるapp::ui::UIViewのメンバ
        m_islandId = nextIslandId;
        Sound::PlaySE(SEQ_SE_DECIDE1);
        this->setCurrDispIsland();
      }
      break;
    }
  
  case REQ_ID_BUTTON_DECIDE:
    {
      switch(reqParam)
      {
      case BUTTON_ID_ISLAND1:
      case BUTTON_ID_ISLAND2:
      case BUTTON_ID_ISLAND3:
      case BUTTON_ID_ISLAND4:
        {
          // 入力を禁止するリクエスト。入力の復活はm_townMapPartsDrawが行ってくれる。
          if(m_townMapPartsDraw) this->SetInputEnabledAllView(false);  // 親であるapp::ui::UIViewのメンバ
          m_islandId = reqParam;
          this->setCurrDispIsland();
          break;
        }
      case BUTTON_ID_PREV_MONS:
        {
          m_param.contents->DecreaseCurrIndexOfDispPokeDataIndexListLimitedSee();
          this->setCurrDispPoke();
          // @fix NMCat[1346] 分布画面から一覧へ遷移した際にポケモンが切り替わっていない  // 『@fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る』にカーソル位置記憶に関してまとめてある。
          *m_param.alolaTopSceneChangeFlag = true;  // 「指しているポケモンが変わる＝アローラ図鑑のカーソル位置が変わる」ときはこのフラグを立てておく。
          break;
        }
      case BUTTON_ID_NEXT_MONS:
        {
          m_param.contents->IncreaseCurrIndexOfDispPokeDataIndexListLimitedSee();
          this->setCurrDispPoke();
          // @fix NMCat[1346] 分布画面から一覧へ遷移した際にポケモンが切り替わっていない  // 『@fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る』にカーソル位置記憶に関してまとめてある。
          *m_param.alolaTopSceneChangeFlag = true;  // アローラ図鑑から来ていないときにもこのフラグを変更しても問題ない。他の図鑑からアローラ図鑑に入り直すと設定し直されるので。
          break;
        }
      }
    }
    break;
  }
}

void ZukanViewMap::createButtonManager(void)
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
void ZukanViewMap::destroyButtonManager(void)
{
  ZukanUtil::DestroyButtonManager(this);
}

void ZukanViewMap::createPage(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_UPPER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytWk*        lytWk_1  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  
  // ペインを覚えておく
  // 島名
  m_islandNameTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_DSB_UPP_000_PANE_TEXTBOX_00);
  // 地名  // 地名表示なくなりました
  //m_placeNameTextPane = lytWk_0->GetTextBoxPane(地名テキストペインなくなりました);  
  //m_placeNameTextPane->SetVisible(false);
  // ポケモン番号、ポケモン名
  m_pokeNoTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_DSB_UPP_000_PANE_TEXTBOX_09);
  m_pokeNameTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_DSB_UPP_000_PANE_TEXTBOX_10);
  // 生息地不明
  m_unknownHabitatTextPane = lytWk_1->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_005_PANE_UNKNOWNHABITATTEXT);
  //m_unknownHabitatTextPane->SetVisible(false);

  // Xボタンペイン
  m_xButtonPane = lytWk_1->GetPane(PANENAME_ZUKAN_MIN_LOW_005_PANE_BUTTONICON_X);

  // ポケアイコン
  gfl2::lyt::LytParts* pokeIconPartsPane = lytWk_1->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_005_PANE_POKEMON_ICONS);
  m_pokeIconPicPane = lytWk_1->GetPicturePane(pokeIconPartsPane, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &lytRes_0);


  ////////////////////////////
  // ポケモン切り替えボタン
  ////////////////////////////
  if(m_param.contents->GetDispPokeDataIndexNumLimitedSee() < 2)  // こっちはsee
  {
    buttonManager->SetButtonPassive(BUTTON_ID_PREV_MONS);
    buttonManager->SetButtonPassive(BUTTON_ID_NEXT_MONS);
  }
  else
  {
    buttonManager->SetButtonActive(BUTTON_ID_PREV_MONS);
    buttonManager->SetButtonActive(BUTTON_ID_NEXT_MONS);
  }
}

void ZukanViewMap::setCurrDispPoke(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
 
  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  
  // アローラ種、方角のいるポケモンに関してはフォルム番号を変更する
  if(pokeData->formOrderType != ZukanFormOrder::FORM_ORDER_TYPE_FORM_NO)
  {
    ZukanFormOrder::FormOrder formOrder;
    bool formOrderEnable = ZukanFormOrder::GetFormOrder(static_cast<MonsNo>(pokeData->monsNo), &formOrder);
    if( formOrderEnable && ( (formOrder.formOrderType == ZukanFormOrder::FORM_ORDER_TYPE_ALOLA) || (formOrder.formOrderType == ZukanFormOrder::FORM_ORDER_TYPE_DIRECTION) ) )
    {
      m_param.contents->SetCurrDispDrawFigureDataIndexSpecialFormNo(formOrder);
    }
  }

  PokeTool::SimpleParam sp = m_param.contents->GetCurrDispPokeSimpleParam();

  // ポケモン番号、ポケモン名
  {
    pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
    {
      pG2DUtil->SetTextBoxPaneNumber(m_pokeNoTextPane, msg_zkn_01_02, pokeData->alolaNo, 3, 0, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU);

      pG2DUtil->SetRegisterMonsNameNo(0, sp.monsNo);
      pG2DUtil->SetTextBoxPaneStringExpand(m_pokeNameTextPane, msg_zkn_03_03);
    }
    pG2DUtil->SetMessageData(NULL, NULL);
  }

  // 捕まえた、見たフラグ
  s32 getSeeAnimFrame = 0;
  if(pokeData->getFlag) getSeeAnimFrame = 2;
  else if(pokeData->figureSeeNum>0) getSeeAnimFrame = 1;
  ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_MAP_ZUKAN_DSB_UPP_000_STATUS, getSeeAnimFrame);

  // ポケモンアイコン
  m_pokeIconTexManager->CreatePokeIconTexture(
      0,
      m_pokeIconPicPane,
      0,
      0,
      sp,
      false
  );

  // 捕まえた、見たフラグ
  if(pokeData->figureSeeNum>0)  // ポケアイコンのキラキラは止める。派手過ぎて、色違いではなく伝説と勘違いされそうだから。つまり情報ページの★マーク以外出さない。
  {
    nw::ut::Color8 vtxColor = ZukanType::POKE_GET_COLOR;
    if(pokeData->getFlag==0)
    {
      vtxColor = ZukanType::POKE_SEE_COLOR;
    }
    for(s32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx) m_pokeIconPicPane->SetVtxColor(vtxIdx, vtxColor);
  }

  // ポケモンモデル
  m_param.tool->ChangePokeModel(sp);

  ////////////////////////////
  // ポケモン切り替えボタン
  ////////////////////////////
  // もしループさせないなら、端に到達したときここでパッシブにする。ループさせるとき、リストではないので端でトリガーにするのは難しい。

  this->setCurrDispIsland();

  if(m_townMapPartsDraw)
  {
    // ポケモンを切り替えたときは、タウンマップは何も変わっていないので、外部から明示的にResetIconを呼んでやる。
    m_townMapPartsDraw->ResetIcon();  // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →SetExIconDataと「ResetIcon,ResetIconPosition」を分けたので、必要なところでは明示的に呼ぶ。
    m_resetIconPositionCallFlag = true;  // 次のフレームで表示する
  }
}

void ZukanViewMap::setCurrDispIsland(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ



  // テキスト
  static const u32 ISLAND_NAME_MES[] =
  {
    msg_zkn_04_01,
    msg_zkn_04_02,
    msg_zkn_04_03,
    msg_zkn_04_04,
  };
  u32 islandNameMsgId = ISLAND_NAME_MES[m_islandId];  // m_islandIdは0始まり

  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    // 島名
    pG2DUtil->SetTextBoxPaneString(m_islandNameTextPane, islandNameMsgId);
  }
  pG2DUtil->SetMessageData(NULL, NULL);



  // ボタン有効無効
  {
    for(u32 i=BUTTON_ID_ISLAND1; i<=BUTTON_ID_ISLAND4; ++i)
    {
      if(static_cast<s32>(i) == m_islandId)
      {
        buttonManager->SetButtonPassive(i, false);
      }
      else
      {
        buttonManager->SetButtonActive(i, false);
      }
    }
  }

  // カーソル位置
  ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, LA_MAP_ZUKAN_MIN_LOW_005_CURSOR, m_islandId);

  if(m_townMapPartsDraw)
  {
    m_townMapPartsDraw->ChangeLandNo(m_islandId, App::TownMap::TownMapPartsDraw::LAND_CHANGE_FREE);
    
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
    s32 mapIconCheckMode = 0;
#if PM_DEBUG
    mapIconCheckMode = ZukanDebugWin::GetMapIconCheckMode();
#endif
    this->SetTownMapPartsDrawIconData(m_townMapPartsDraw->IsZoom(), mapIconCheckMode);
#else
    this->SetTownMapPartsDrawIconData(m_townMapPartsDraw->IsZoom());
#endif
    //m_townMapPartsDraw->ResetIcon();  // ここは元からコメントアウトされていた
  }
}

void ZukanViewMap::startInSeq(void)
{
  m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_MAP_ZUKAN_DSB_UPP_000_IN;
  m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_MAP_ZUKAN_MIN_LOW_005_IN;
 
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_IN);
}
void ZukanViewMap::startOutSeq(void)
{
  m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_MAP_ZUKAN_DSB_UPP_000_RETURN;
  m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_MAP_ZUKAN_MIN_LOW_005_RETURN;
  
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_OUT);

  m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
}

void ZukanViewMap::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

