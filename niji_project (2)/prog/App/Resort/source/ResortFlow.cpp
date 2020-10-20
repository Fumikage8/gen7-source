//======================================================================
/**
 * @file    ResortFlow.cpp
 * @date    2015/10/08 20:50:30
 * @author  fukushima_yuya
 * @brief   ポケリゾート：画面フロー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ResortFlow.h"
#include "ResortConst.h"
#include "ResortUtility.h"
#include "ResortSequenceListener.h"
#include "ResortFlowListener.h"
#include "ResortInitLogic.h"
#include "ResortTouchToFlowListener.h"
#include "ResortChangeIslandListener.h"

#include <App/Resort/include/ResortAppParam.h>
#include <App/Resort/source/upper/ResortUpperHomeMenuDraw.h>
#include <App/Resort/source/upper/ResortUpperDraw.h>
#include <App/Resort/source/lower/ResortLowerAdventureResultDraw.h>
#include <App/Resort/source/lower/ResortLowerBeansSelectDraw.h>
#include <App/Resort/source/lower/ResortLowerCourseSelectDraw.h>
#include <App/Resort/source/lower/ResortLowerFarmDraw.h>
#include <App/Resort/source/lower/ResortLowerFarmListDraw.h>
#include <App/Resort/source/lower/ResortLowerGimDraw.h>
#include <App/Resort/source/parts/ResortTalkMessage.h>

#include <AppLib/include/Tool/TalkWindow.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ResortSave.h>
#include <Savedata/include/BoxPokemonSave.h>
#include <Savedata/include/MyStatus.h>
#include <Savedata/include/MyItemSave.h>
#include <sound/include/Sound.h>
#include <Print/include/WordSet.h>
#include <Fade/include/gfl2_FadeManager.h>

#include <pml/include/pmlib.h>

#include <niji_conv_header/message/script/msg_pokeresort_talk.h>
#include <niji_conv_header/field/script/inc/scene_work_const.inc>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  // 起動時イベントシーケンス
  enum {
    FEF_BEGIN,
    FEF_BONUS_BEGIN,
    FEF_BONUS,
    FEF_DEVELOP_BEGIN,
    FEF_DEVELOP,
    FEF_TUTORIAL,
    FEF_RESORT_END,
    FEF_RESORT_END_WAIT,
    FEF_END,
  };

  // 管理人メニューシーケンス
  enum {
    HMF_INTRO,
    HMF_INTRO_WAIT,
    HMF_USER_SELECT,
    HMF_DEVELOP,
    HMF_TRADE,
    HMF_NOT_TRADE,
    HMF_BEANS_SEND,
    HMF_TUTORIAL,
    HMF_END,
  };
  // きのみリゾートシーケンス
  enum {
    FFF_FIELD,
    FFF_FIELD_END,
    FFF_LIST,
    FFF_END,
  };
  // たんけんリゾート：コース選択シーケンス
  enum {
    ACSF_BEGIN,
    ACSF_SELECT_INTRO,
    ACSF_SELECT,
    ACSF_ADVENTURE,
    ACSF_ERROR,
    ACSF_END,
  };
  // うんどうリゾート：うんどう状態シーケンス
  enum {
    GSF_BEGIN,
    GSF_START,
    GSF_TRAINING,
    GSF_FINISH,
    GSF_END,
  };
  // おんせんリゾート：タマゴ孵化シーケンス
  enum {
    HESF_BEGIN,
    HESF_CALL_DEMO,
    HESF_NOT_SPACE,
    HESF_STATE,
    HESF_EVENT_END_BEGIN,
    HESF_EVENT_END_END,
    HESF_END,
  };
  // Boxポケモンイベント
  enum {
    BPEF_BEGIN,
    BPEF_SELECT_BEGIN,
    BPEF_SELECT_OPEN,
    BPEF_SELECT_END,
    BPEF_EVENT_NORMAL_BEGIN,
    BPEF_EVENT_NORMAL_END,
    BPEF_EVENT_SEIKAKU_BEGIN,
    BPEF_EVENT_SEIKAKU_END,
    BPEF_EVENT_FRIENDSHIP_BEGIN,
    BPEF_EVENT_FRIENDSHIP_END,
    BPEF_EVENT_POKEMAME_BEGIN,
    BPEF_EVENT_POKEMAME_GET,
    BPEF_EVENT_POKEMAME_END,
    BPEF_EVENT_FARM_BEGIN,
    BPEF_EVENT_FARM_END,
    BPEF_EVENT_GIM_BEGIN,
    BPEF_EVENT_GIM_END,
    BPEF_EVENT_HOTSPA_BEGIN,
    BPEF_EVENT_HOTSPA_END,
    BPEF_EVENT_HOTSPA_DIZZY_BEGIN,
    BPEF_EVENT_HOTSPA_DIZZY_END,
    BPEF_EVENT_END_BEGIN,
    BPEF_EVENT_END_END,
    BPEF_END,
  };
  // 野生ポケモンイベント
  enum {
    WPEF_BEGIN,
    WPEF_APPEARANCE_BEGIN,
    WPEF_APPEARANCE_END,
    WPEF_CARE_BEGIN,
    WPEF_CARE_END,
    WPEF_SELECT_BEGIN,
    WPEF_SELECT_END,
    WPEF_TAKE_CHARGE,
    WPEF_EVENT_END_BEGIN,
    WPEF_EVENT_END_END,
    WPEF_END,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//------------------------------------------------------------------------------
ResortFlow::ResortFlow( DrawList& drawList, app::util::Heap* heap, print::WordSet* wordSet )
  : m_pResortSave(NULL)
  , m_pBoxPokemon(NULL)
  , m_pMyStatus(NULL)
  , m_pHeap(heap)
  , m_pWordSet(wordSet)
  , m_pFadeMan(GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager))
  , m_DrawList(drawList)
  , m_StartFlowType(FLOW_TYPE_NONE)
  , m_MainSeq(0)
  , m_SubSeq(0)
  , m_WaitFrame(0)
  , m_FadeFlg(false)
  , m_pParamTable(NULL)
  , m_GimGroupIndex(0)
  , m_MsgTableMax(0)
  , m_MsgSeq(0)
{
  GameSys::GameData* gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  {
    m_pResortSave  = gd->GetResortSave();
    m_pBoxPokemon  = gd->GetBoxPokemon();
    m_pMyStatus    = gd->GetPlayerStatus();
    m_pMyItem      = gd->GetMyItem();
    
    // パーティ内のポケモンの数
    m_PartyMemberNum = gd->GetPlayerParty()->GetMemberCount();
  }

  gfl2::heap::HeapBase* sysHeap = m_pHeap->GetSystemHeap();
  {
    m_MenuParam.pokeCoreParam = GFL_NEW(sysHeap) pml::pokepara::CoreParam( sysHeap );
    GFL_ASSERT( m_MenuParam.pokeCoreParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
ResortFlow::~ResortFlow( void )
{
  GFL_SAFE_DELETE( m_MenuParam.pokeCoreParam );
}



//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの開始
 *
 * @param   flowType    フローの種類
 */
//------------------------------------------------------------------------------
void ResortFlow::StartMenuFlow( FlowType flowType )
{
  { // メンバ変数の初期化
    m_StartFlowType = flowType;
    m_MainSeq        = 0;
  }
  
  if( flowType == FLOW_TYPE_TUTORIAL_1_START ) return;

  if( flowType != FLOW_TYPE_FARM_FIELD && flowType != FLOW_TYPE_ONSEN_EGG_STATE )
  {
    SetupMenuFlow( flowType );
  }

  if( flowType != FLOW_TYPE_BOX_POKE_EVENT && flowType != FLOW_TYPE_WILD_POKE_EVENT && flowType != FLOW_TYPE_ONSEN_EGG_STATE )
  {
    SetLowerUIType( LowerUIDraw::DISP_TYPE_MASK );
  }
  else {
    SetLowerUIType( LowerUIDraw::DISP_TYPE_INPUT_WAIT_NO_MASK );
  }

  // 3D側の入力無効に
  ResortSequenceListener* seqListener = m_DrawList.seqListener;
  {
    if( seqListener )
    {
      seqListener->OpenHomeMneu( true );
    }
  }

#if PM_DEBUG
#ifdef DEBUG_ONLY_FOR_fukushima_yuya
  static const char* c_FlowTypeName[] = {
    "FLOW_TYPE_NONE",
    "FLOW_TYPE_FIRST_EVENT",
    "FLOW_TYPE_HOME_MENU",
    "FLOW_TYPE_MAME_BOX_FUREAI",
    "FLOW_TYPE_MAME_BOX_KINOMI",
    "FLOW_TYPE_MAME_BOX_TANKEN",
    "FLOW_TYPE_MAME_BOX_UNDOU",
    "FLOW_TYPE_MAME_BOX_ONSEN",
    "FLOW_TYPE_FARM_FIELD",
    "FLOW_TYPE_FARM_LIST",
    "FLOW_TYPE_TANKEN_SELECT",
    "FLOW_TYPE_TANKEN_RESULT",
    "FLOW_TYPE_UNDOU_STATE",
    "FLOW_TYPE_UNDOU_SELECT",
    "FLOW_TYPE_UNDOU_TAKE_BACK",
    "FLOW_TYPE_ONSEN_POKE_SELECT",
    "FLOW_TYPE_ONSEN_EGG_STATE",
    "FLOW_TYPE_BOX_POKE_EVENT",
    "FLOW_TYPE_WILD_POKE_EVENT",
    "FLOW_TYPE_POKE_GET",
    "FLOW_TYPE_TUTORIAL_1_START",
    "FLOW_TYPE_TUTORIAL_1",
    "FLOW_TYPE_TUTORIAL_2",
    "FLOW_TYPE_TUTORIAL_3",
    "FLOW_TYPE_TUTORIAL_4",
    "FLOW_TYPE_END_RESORT",
  };
  FUKUSHIMA_PRINT( "StartMenuFlow[%s]\n", c_FlowTypeName[flowType] );
#endif
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   フロー開始に伴うセットアップ
 *
 * @param   flowType    フローの種類
 */
//------------------------------------------------------------------------------
void ResortFlow::SetupMenuFlow( FlowType flowType )
{
  // 各リゾートの上画面を非表示
  //SetVisible_UpperDraw( false );

  // 管理人メニューを表示
  UpperHomeMenuDraw* homeMenu = m_DrawList.homeMenu;
  {
    if( homeMenu )
    {
      UpperHomeMenuDraw::DispType dispType = UpperHomeMenuDraw::DISP_TYPE_TALK;

      if( flowType == FLOW_TYPE_HOME_MENU || flowType == FLOW_TYPE_TUTORIAL_4 )
      {
        dispType = UpperHomeMenuDraw::DISP_TYPE_HOME_MENU;
      }

      // 画面表示の設定
      homeMenu->SetDispType( dispType );
    }
  }

  if( flowType != FLOW_TYPE_FIRST_EVENT )
  {
    // 開く
    SetVisible_HomeMenu( true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   フロー開始に伴うセットアップ解除
 */
//------------------------------------------------------------------------------
void ResortFlow::UnsetupMenuFlow( void )
{
  m_StartFlowType = FLOW_TYPE_NONE;

  // シーケンスの初期化
  SetMainSeq( 0 );
  SetSubSeq( 0 );

  // 各リゾートの上画面を表示
  SetVisible_UpperDraw( true );
  // 管理人メニューを非表示
  SetVisible_HomeMenu( false );
  // 表示を変更
  SetLowerUIType( LowerUIDraw::DISP_TYPE_MAIN );
  m_DrawList.lowerUI->SetInputEnabled( true );

  // 3D側の入力を有効に戻す
  ResortSequenceListener* seqListener = m_DrawList.seqListener;
  {
    if( seqListener )
    {
      seqListener->OpenHomeMneu( false );
    }
  }

}

//------------------------------------------------------------------------------
/**
 * @brief   管理人メニューの表示
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetVisible_HomeMenu( bool visible )
{
  // 管理人メニューを非表示
  UpperHomeMenuDraw* homeMenu = m_DrawList.homeMenu;
  {
    if( homeMenu )
    {
      if( visible )
      {
        homeMenu->Open();
      }
      else {
        homeMenu->Close();
      }
    }
  }

  if( !visible )
  {
    // 会話ウィンドウを閉じる
    if( m_pTalkMessage )
    {
      m_pTalkMessage->TalkEnd();
    }
  }
  else {
    if( m_StartFlowType == FLOW_TYPE_FIRST_EVENT )
    {
      // リゾートの開始
      m_DrawList.flowListener->RequestFadeIn();

      m_FadeFlg = true;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面の表示
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetVisible_UpperDraw( bool visible )
{
  UpperDraw* upperMenu = m_DrawList.upperMenu;
  {
    if( upperMenu )
    {
      upperMenu->SetVisible( visible );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow( void )
{
  void (ResortFlow::*UPDATE_MENU_FLOW[FLOW_TYPE_NUM])( void ) = {
    &ResortFlow::UpdateMenuFlow_None,
    &ResortFlow::UpdateMenuFlow_FirstEvent,
    &ResortFlow::UpdateMenuFlow_HomeMenu,
    &ResortFlow::UpdateMenuFlow_SelectBeans,
    &ResortFlow::UpdateMenuFlow_SelectBeans,
    &ResortFlow::UpdateMenuFlow_SelectBeans,
    &ResortFlow::UpdateMenuFlow_SelectBeans,
    &ResortFlow::UpdateMenuFlow_SelectBeans,
    &ResortFlow::UpdateMenuFlow_FarmField,
    &ResortFlow::UpdateMenuFlow_FarmList,
    &ResortFlow::UpdateMenuFlow_AdvSelect,
    &ResortFlow::UpdateMenuFlow_AdvResult,
    &ResortFlow::UpdateMenuFlow_GimState,
    &ResortFlow::UpdateMenuFlow_GimSelect,
    &ResortFlow::UpdateMenuFlow_GimTakeBack,
    &ResortFlow::UpdateMenuFlow_HotspaSelect,
    &ResortFlow::UpdateMenuFlow_HotspaEgg,
    &ResortFlow::UpdateMenuFlow_PokeEventBox,
    &ResortFlow::UpdateMenuFlow_PokeEventWild,
    &ResortFlow::UpdateMenuFlow_PokeEventGet,
    &ResortFlow::UpdateMenuFlow_Tutorial1_Start,  // kawa_resort_modify
    &ResortFlow::UpdateMenuFlow_Tutorial1,
    &ResortFlow::UpdateMenuFlow_Tutorial2,
    &ResortFlow::UpdateMenuFlow_Tutorial3,
    &ResortFlow::UpdateMenuFlow_Tutorial4,
    &ResortFlow::UpdateMenuFlow_EndResort,
  };

  (this->*UPDATE_MENU_FLOW[m_StartFlowType])();
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：起動時イベント
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_FirstEvent( void )
{
  switch( m_MainSeq )
  {
  case FEF_BEGIN:
    {
      // 開始分岐
      FirstEvent_Begin();
    } break;

  case FEF_BONUS_BEGIN:
    {
      // ボーナス分岐
      FirstEvent_Bonus();
    } break;

  case FEF_BONUS:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( FEF_DEVELOP_BEGIN );
    } break;

  case FEF_DEVELOP_BEGIN:
    {
      // 開発分岐
      FirstEvent_Develop();
    } break;

  case FEF_DEVELOP:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( FEF_END );
    } break;

  case FEF_TUTORIAL:
    {
      if( !IsTalkEnd_Loop() ) break;

      // ポケマメに指カーソルを表示
      VisibleFingerCursor( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS );
      // モデルアニメの再生
      StartModelAnim( ResortChangeIslandListener::ISLAND_MODELANIM_BEANS );

      SetMainSeq( FEF_END );
    } break;

  case FEF_RESORT_END:
    {
      if( !IsTalkEnd() ) break;
      
      CallApplication( CALL_RESORT_END );

      SetMainSeq( FEF_RESORT_END_WAIT );
    } break;

  case FEF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：管理人画面
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_HomeMenu( void )
{
  switch( m_MainSeq )
  {
  case HMF_INTRO:
    {
      // 開始分岐
      HomeMenuEvent_Begin();
    } break;

  case HMF_INTRO_WAIT:
    {
      if( !IsTalkEnd() ) break;

      // 選択ボタンを表示
      SetDispType_Select( LowerSelectDraw::SEL_TYPE_HOUSE );
      OpenLowerMenu( LOW_MENU_SELECT );
      SetMainSeq( HMF_USER_SELECT );
    } break;

  case HMF_USER_SELECT:
    {
      // メニュー選択
      HomeMenuEvent_MenuSelect();
    } break;

  case HMF_DEVELOP:
    {
      // 開発
      HomeMenuEvent_Develop();
    } break;

  case HMF_TRADE:
    {
      // 交換
      HomeMenuEvent_Trade();
    } break;

  case HMF_NOT_TRADE:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( HMF_INTRO );
    } break;

  case HMF_BEANS_SEND:
    {
      // マメビン流し
      HomeMenuEvent_BeansSend();
    } break;

  case HMF_TUTORIAL:
    {
      // チュートリアル
      StartMenuFlow( FLOW_TYPE_TUTORIAL_4 );
    } break;

  case HMF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：ポケマメ選択
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_SelectBeans( void )
{
  enum {
    BSF_BEGIN,
    BSF_SELECT_BEGIN,
    BSF_SELECT_END,
    BSF_SYSTEM_MSG_BEGIN,
    BSF_SYSTEM_MSG_END,
    BSF_ERROR_MSG,
    BSF_END,
  };

  u8 type = BEANS_SEL_TYPE_FRIENDSHIP + (m_StartFlowType - FLOW_TYPE_MAME_BOX_FUREAI);

  switch( m_MainSeq )
  {
  case BSF_BEGIN:
    {
      // ポケモンがいない時
      if( !IsValidPokemon( type ) )
      {
        SetMessage_BeansSelectPokeNone( type );

        SetMainSeq( BSF_ERROR_MSG );
      }
      // きのみの生育を行っていない
      else if( type == BEANS_SEL_TYPE_FARM && IsFarmPlantNone() )
      {
        TalkStart( msg_prwin_info_06_09 );
        SetMainSeq( BSF_ERROR_MSG );
      }
      // たんけんを行っていない
      else if( type == BEANS_SEL_TYPE_ADVENTURE && m_pResortSave->GetAdventureCoursePattern() == Savedata::ResortSave::RESORT_ADV_COURSE_NONE )
      {
        TalkStart( msg_prwin_sys_03_09 );
        SetMainSeq( BSF_ERROR_MSG );
      }
      // うんどうを行っているグループがない
      else if( type == BEANS_SEL_TYPE_GIM && IsGimTrainingNone() )
      {
        TalkStart( msg_prwin_sys_04_13 );
        SetMainSeq( BSF_ERROR_MSG );
      }
      // ポケモンがいる時
      else {
        // ボーナスタイムの取得
        TimeParam time;
        {
          util::GetBonusTime( m_pResortSave, type, &time );
        }

        if( !time.IsInvalid() && time.GetTotalSec() > 0 )
        {
          TalkStart( msg_prwin_sys_common_03 );
          SetMainSeq( BSF_ERROR_MSG );
        }
        else if( util::GetAllBeansCount( m_pResortSave ) == 0 )
        {
          TalkStart( msg_prwin_sys_00_04 );
          SetMainSeq( BSF_ERROR_MSG );
        }
        else {
          TalkStart( msg_prwin_sys_common_01, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
          SetMainSeq( BSF_SELECT_BEGIN );
        }
      }
    } break;

  case BSF_SELECT_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      // ポケマメ選択を開く
      SetDispType_BeansSelect( type );
      OpenLowerMenu( LOW_MENU_BEANS_SELECT );

      SetMainSeq( BSF_SELECT_END );
    } break;

  case BSF_SELECT_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_BEANS_SELECT );

      if( result != RESULT_CONTINUE )
      {
        // ポケマメ選択を閉じる
        CloseLowerMenu( LOW_MENU_BEANS_SELECT );

        if( result == RESULT_DECIDE )
        {
          // ポケマメいれ後のメッセージをセット
          SetMessage_BeansSelectAfter( type );

          SetMainSeq( BSF_SYSTEM_MSG_BEGIN );
        }
        else {
          SetMainSeq( BSF_END );
        }
      }
    } break;

  case BSF_SYSTEM_MSG_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      // システムメッセージの表示
      SetMessage_BeansSelectSystem( type );

      SetMainSeq( BSF_SYSTEM_MSG_END );
    } break;

  case BSF_SYSTEM_MSG_END:
  case BSF_ERROR_MSG:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( BSF_END );
    } break;

  case BSF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：はたけのようす画面
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_FarmField( void )
{
  switch( m_MainSeq )
  {
  case FFF_FIELD:
    {
      // はたけのようす
      FarmEvent_FieldState();
    } break;

  case FFF_FIELD_END:
    {
      SetupMenuFlow( m_StartFlowType );
      SetSubSeq( 0 );
    } break;

  case FFF_LIST:
    {
      // きのみリスト
      FarmEvent_KinomiList();
    } break;

  case FFF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：きのみリスト画面
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_FarmList( void )
{
  switch( m_MainSeq )
  {
  case FFF_FIELD:
  case FFF_FIELD_END:
  case FFF_LIST:
    {
      // きのみリスト
      FarmEvent_KinomiList();
    } break;

  case FFF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：たんけんコース選択
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_AdvSelect( void )
{
  switch( m_MainSeq )
  {
  case ACSF_BEGIN:
    {
      // 開始分岐
      AdventureSelectEvent_Begin();
    } break;

  case ACSF_SELECT_INTRO:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( ACSF_SELECT );
    } break;

  case ACSF_SELECT:
    {
      // コース選択
      AdventureSelectEvent_Select();
    } break;

  case ACSF_ADVENTURE:
    {
      // たんけん中断確認
      AdventureSelectEvent_Adventure();
    } break;

  case ACSF_ERROR:
    {
      // エラー
      AdventureSelectEvent_Error();
    } break;

  case ACSF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：たんけん結果
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_AdvResult( void )
{
  enum {
    AIRF_BEGIN,
    AIRF_RES_OK,
    AIRF_LIST_BEGIN,
    AIRF_LIST,
    AIRF_RES_FAILED,
    AIRF_MSG,
    AIRF_END,
  };

  switch( m_MainSeq )
  {
  case AIRF_BEGIN:
    {
      if( GetAdvItemCount() > 0 )
      {
        // 演出
        SetMainSeq( AIRF_RES_OK );
      }
      else {
        // 演出
        SetMainSeq( AIRF_RES_FAILED );
      }
    } break;

  case AIRF_RES_OK:
    {
      TalkStart( msg_prwin_sys_03_01, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

      SetMainSeq( AIRF_LIST_BEGIN );
    } break;

  case AIRF_LIST_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      // たんけん結果を表示
      OpenLowerMenu( LOW_MENU_ADV_RESULT );

      SetMainSeq( AIRF_LIST );
    } break;

  case AIRF_LIST:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_ADV_RESULT );

      if( result != RESULT_CONTINUE )
      {
        // たんけん結果を非表示
        CloseLowerMenu( LOW_MENU_ADV_RESULT );

        SetMainSeq( AIRF_END );
      }
    } break;

  case AIRF_RES_FAILED:
    {
      TalkStart( msg_prwin_sys_03_02 );

      SetMainSeq( AIRF_MSG );
    } break;

  case AIRF_MSG:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( AIRF_END );
    } break;

  case AIRF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：うんどう状態
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_GimState( void )
{
  switch( m_MainSeq )
  {
  case GSF_BEGIN:
    {
      GimStatetEvent_Begin();       //!< 開始分岐
    } break;

  case GSF_START:
    {
      GimStatetEvent_Start();       //!< うんどうの開始
    } break;

  case GSF_TRAINING:
    {
      GimStatetEvent_Training();    //!< うんどう中
    } break;

  case GSF_FINISH:
    {
      GimStatetEvent_Finish();      //!< うんどうの終了
    } break;

  case GSF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：うんどうコース選択
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_GimSelect( void )
{
  enum {
    GSFA_BEGIN,
    GSFA_COURSE_BEGIN,
    GSFA_COURSE,
    GSFA_SET_BEGIN,
    GSFA_SET,
    GSFA_SET_END,
    GSFA_END,
  };

  // グループインデックス
  u8 gimGroup = GetIndex_GimGroup();

  switch( m_MainSeq )
  {
  case GSFA_BEGIN:
    {
      // うんどうするポケモン数の取得
      u8 pokeCount = GetGimPokeCount();

      if( pokeCount == 0 )
      {
        SetMainSeq( GSFA_END );
      }
      else {
        TalkStart( msg_prwin_sys_04_02, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
        SetMainSeq( GSFA_COURSE_BEGIN );
      }
    } break;

  case GSFA_COURSE_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      if( m_DrawList.lowerCourseSel == NULL ) break;    //!< @fix NMCat[3456]：コース選択画面が生成されるまで待つ

      // コース選択を表示：うんどう
      SetDispType_Course( COURSE_SELECT_GIM );
      OpenLowerMenu( LOW_MENU_COURSE );

      SetMainSeq( GSFA_COURSE );
    } break;

  case GSFA_COURSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_COURSE );

      if( result != RESULT_CONTINUE )
      {
        // コース選択を非表示：うんどう
        CloseLowerMenu( LOW_MENU_COURSE );

        if( result == RESULT_CANCEL )
        {
          CallApplication( CALL_APP_BOX_GIM );
          SetMainSeq( GSFA_END );
        }
        else {
          TalkStart( msg_prwin_sys_04_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
          SetMainSeq( GSFA_SET_BEGIN );
        }
      }
    } break;

  case GSFA_SET_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      // うんどう状態を表示：セット回数
      SetDispType_Gim( GIM_DECIDE_BTN_ENABLE );
      OpenLowerMenu( LOW_MENU_GIM );

      SetMainSeq( GSFA_SET );
    } break;

  case GSFA_SET:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_GIM );

      if( result != RESULT_CONTINUE )
      {
        // うんどう状態を非表示：セット回数
        CloseLowerMenu( LOW_MENU_GIM );

        if( result == RESULT_CANCEL )
        {
          TalkStart( msg_prwin_sys_04_02, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
          SetMainSeq( GSFA_COURSE_BEGIN );
        }
        else {
          m_pResortSave->SetGimState( gimGroup, Savedata::ResortSave::RESORT_GIM_STATE_TRAINING );
          TalkStart( msg_prwin_sys_04_04 );
          SetMainSeq( GSFA_SET_END );
        }
      }
    } break;

  case GSFA_SET_END:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( GSFA_END );
    } break;

  case GSFA_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：うんどう引き取る
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_GimTakeBack( void )
{
  enum {
    GTBF_BEGIN,
    GTBF_FIN_SUCCESS,
    GTBF_FIN_FAILED,
    GTBF_END,
  };

  // グループインデックス
  u8 gimGroup = GetIndex_GimGroup();

  switch( m_MainSeq )
  {
  case GTBF_BEGIN:
    {
      // うんどうが終了したポケモン数
      u8 pokeCount = GetGimPokeCount();

      if( pokeCount == 0 )
      {
        TalkStart( msg_prwin_sys_04_10 );
        SetMainSeq( GTBF_FIN_SUCCESS );
      }
      else {
        TalkStart( msg_prwin_sys_04_09 );
        SetMainSeq( GTBF_FIN_FAILED );
      }
    } break;

  case GTBF_FIN_SUCCESS:
    {
      if( !IsTalkEnd() ) break;

      m_pResortSave->SetGimState( gimGroup, Savedata::ResortSave::RESORT_GIM_STATE_INIT );

      SetMainSeq( GTBF_END );
    } break;

  case GTBF_FIN_FAILED:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( GTBF_END );
    } break;

  case GTBF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：おんせんポケモン選択
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_HotspaSelect( void )
{
  enum {
    HPSF_BEGIN,
    HPSF_SELECT,
    HPSF_END,
  };

  switch( m_MainSeq )
  {
  case HPSF_BEGIN:
    {
      TalkStart( msg_prwin_sys_05_01 );

      SetMainSeq( HPSF_SELECT );
    } break;

  case HPSF_SELECT:
    {
      if( !IsTalkEnd() ) break;

      CallApplication( CALL_APP_BOX_HOTSPA );

      SetMainSeq( HPSF_END );
    } break;

  case HPSF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：おんせんタマゴ孵化
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_HotspaEgg( void )
{
  switch( m_MainSeq )
  {
  case HESF_BEGIN:
    {
      // 開始分岐
      HotspaEggEvent_Begin();
    } break;

  case HESF_CALL_DEMO:
    {
      HotspaEggEvent_CallDemo();
    } break;

  case HESF_NOT_SPACE:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( HESF_EVENT_END_BEGIN );
    } break;

  case HESF_STATE:
    {
      HotspaEggEvent_State();
    } break;

  case HESF_EVENT_END_BEGIN:
    {
      // ポケモンイベントの終了
      EndPokeEvent();

      SetMainSeq( HESF_EVENT_END_END );
    } break;

  case HESF_EVENT_END_END:
    {
      // ポケモンイベントの終了チェック
      if( !IsEndPokeEvent() ) break;

      SetMainSeq( HESF_END );
    } break;

  case HESF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：BOXポケモンイベント
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_PokeEventBox( void )
{
  switch( m_MainSeq )
  {
  case BPEF_BEGIN:
    {
      // 開始分岐
      BoxPokemonEvent_Begin();
    } break;

  case BPEF_SELECT_BEGIN:
    {
      TalkStart( msg_prwin_poke_ev_00_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE );
      SetMainSeq( BPEF_SELECT_OPEN );
    } break;

  case BPEF_SELECT_OPEN:
    {
      if( !IsTalkEnd() ) break;

      SetLowerUIType( LowerUIDraw::DISP_TYPE_BOX_POKEMON_EVENT );

      SetMainSeq( BPEF_SELECT_END );
    } break;

  case BPEF_SELECT_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_UI );

      if( result != RESULT_CONTINUE )
      {
        if( result == RESULT_STATUS )
        {
          CallApplication( CALL_APP_STATUS );
        }

        SetMainSeq( BPEF_EVENT_END_BEGIN );
      }
    } break;

  case BPEF_EVENT_NORMAL_BEGIN:
    {
      // ノーマルイベントの開始
      SetMessage_EventNormal();
      // イベントワークを０に
      SetPokemonEventWork( pml::pokepara::RESORTEVENT_NONE );

      SetMainSeq( BPEF_EVENT_NORMAL_END );
    } break;

  case BPEF_EVENT_SEIKAKU_BEGIN:
    {
      // 性格メッセージ
      SetMessage_EventSeikaku();

      SetMainSeq( BPEF_EVENT_SEIKAKU_END );
    } break;

  case BPEF_EVENT_FRIENDSHIP_BEGIN:
    {
      // いちゃいちゃ
      if( SetMessage_EventFriendship() )
      {
        // イベントワークを３に
        SetPokemonEventWork( pml::pokepara::RESORTEVENT_LIKE_RESORT );

        // 開始分岐へ
        SetMainSeq( BPEF_BEGIN );
      }
      else {
        // イベントワークを０に
        SetPokemonEventWork( pml::pokepara::RESORTEVENT_NONE );

        SetMainSeq( BPEF_EVENT_FRIENDSHIP_END );
      }
    } break;

  case BPEF_EVENT_POKEMAME_BEGIN:
    {
      // ポケマメ取得
      TalkStart( msg_prwin_poke_ev_07_01, MSG_TYPE_POKE_NICKNAME );

      SetMainSeq( BPEF_EVENT_POKEMAME_GET );
    } break;

  case BPEF_EVENT_POKEMAME_GET:
    {
      if( !IsTalkEnd() ) break;

      Sound::PlaySE( SEQ_SE_DECIDE1 );

      // ポケマメ取得システムメッセージ
      SetMessage_GetPokemame();
      // もらったポケマメの反映
      SetPokemame();
      // イベントワークを０に
      SetPokemonEventWork( pml::pokepara::RESORTEVENT_NONE );

      SetMainSeq( BPEF_EVENT_POKEMAME_END );
    } break;

  case BPEF_EVENT_FARM_BEGIN:
    {
      // きのみ専用のメッセージ
      SetMessage_FarmResort();

      SetMainSeq( BPEF_EVENT_FARM_END );
    } break;

  case BPEF_EVENT_GIM_BEGIN:
    m_MsgValue = m_pResortSave->GetGimStartValue( m_MenuParam.pokemonIndex );
    TalkStart( GetEventStartGimMsgID(m_MenuParam.pokemonIndex/6,&m_MsgValue), MSG_TYPE_EVENT_GIM, gfl2::str::StrWin::FINISH_NONE );
    SetMainSeq( BPEF_EVENT_GIM_END );
    break;

  case BPEF_EVENT_HOTSPA_BEGIN:
    TalkStart( GetEventStartHotSpaMsgID(), MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE );
    SetMainSeq( BPEF_EVENT_HOTSPA_END );
    break;

  case BPEF_EVENT_HOTSPA_DIZZY_BEGIN:
    TalkStart( msg_prwin_poke_ev_18_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE );
    SetMainSeq( BPEF_EVENT_HOTSPA_DIZZY_END );
    break;

  case BPEF_EVENT_NORMAL_END:
  case BPEF_EVENT_SEIKAKU_END:
  case BPEF_EVENT_FRIENDSHIP_END:
  case BPEF_EVENT_FARM_END:
  case BPEF_EVENT_GIM_END:
  case BPEF_EVENT_HOTSPA_END:
  case BPEF_EVENT_HOTSPA_DIZZY_END:
    {
      SetMainSeq( BPEF_SELECT_OPEN );
    } break;

  case BPEF_EVENT_POKEMAME_END:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( BPEF_SELECT_BEGIN );
    } break;

  case BPEF_EVENT_END_BEGIN:
    {
      // ポケモンイベントの終了
      EndPokeEvent();

      SetMainSeq( BPEF_EVENT_END_END );
    } break;

  case BPEF_EVENT_END_END:
    {
      // ポケモンイベントの終了チェック
      if( !IsEndPokeEvent() ) break;

      SetMainSeq( BPEF_END );
    } break;

  case BPEF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：野生ポケモンイベント
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_PokeEventWild( void )
{
  switch( m_MainSeq )
  {
  case WPEF_BEGIN:
    {
      // 開始分岐
      WildPokemonEvent_Begin();
    } break;

  case WPEF_APPEARANCE_BEGIN:
    {
      TalkStart( msg_prwin_poke_ev_10_01 );

      SetMainSeq( WPEF_APPEARANCE_END );
    } break;

  case WPEF_APPEARANCE_END:
    {
      if( !IsTalkEnd() ) break;

      SetPokemonEventWork( pml::pokepara::RESORTEVENT_WILD_LOVE );
      m_DrawList.upperMenu->SetupResort( RESORT_ID_FRIENDSHIP );

      SetMainSeq( WPEF_CARE_BEGIN );
    } break;

  case WPEF_CARE_BEGIN:
    {
      TalkStart( msg_prwin_poke_ev_00_02, MSG_TYPE_POKE_MONSNAME, gfl2::str::StrWin::FINISH_NONE );

      SetMainSeq( WPEF_CARE_END );
    } break;

  case WPEF_CARE_END:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( WPEF_SELECT_BEGIN );
    } break;

  case WPEF_SELECT_BEGIN:
    {
      SetLowerUIType( LowerUIDraw::DISP_TYPE_WILD_POKEMON_EVENT );

      SetMainSeq( WPEF_SELECT_END );
    } //break;

  case WPEF_SELECT_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_UI );

      if( result != RESULT_CONTINUE )
      {
        SetMainSeq( WPEF_EVENT_END_BEGIN );
      }
    } break;

  case WPEF_TAKE_CHARGE:
    {
      // 引き取る
      WildPokemonEvent_TakeCharge();
    } break;

  case WPEF_EVENT_END_BEGIN:
    {
      // ポケモンイベントの終了
      EndPokeEvent();

      SetMainSeq( WPEF_EVENT_END_END );
    } break;

  case WPEF_EVENT_END_END:
    {
      // ポケモンイベントの終了チェック
      if( !IsEndPokeEvent() ) break;

      SetMainSeq( WPEF_END );
    } break;

  case WPEF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：図鑑後
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_PokeEventGet( void )
{
  enum {
    WPZF_BEGIN,
    WPZF_MSG,
    WPZF_END,
  };

  switch( m_MainSeq )
  {
  case WPZF_BEGIN:
    {
      //TalkStart( msg_prwin_poke_ev_11_05 );
      SetMainSeq( WPZF_MSG );
    } break;

  case WPZF_MSG:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( WPZF_END );
    } break;

  case WPZF_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


// kawa_resort_modify
//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：チュートリアル：ポケマメタッチ開始
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_Tutorial1_Start( void )
{
  enum {
    TUTORIAL_1_BEGIN,
    TUTORIAL_1_ANIM,
  };

  switch( m_MainSeq )
  {
  case TUTORIAL_1_BEGIN:
    {
      // 指カーソルを非表示
      InvisibleFingerCursor();
      m_WaitFrame = 0;
      SetMainSeq( TUTORIAL_1_ANIM );
    } break;

  case TUTORIAL_1_ANIM:  // アニメの終了を待つ  // kawa_resort_modify
    {
      m_WaitFrame++;
      if( m_WaitFrame < TUTORIAL_BEANS_ANIM_PICKUP_WAIT_FRAME ) break;
      this->StartMenuFlow( ResortFlow::FLOW_TYPE_TUTORIAL_1 );  // メニュー画面にいく
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：チュートリアル：ポケマメタッチ
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_Tutorial1( void )
{
  enum {
    TUTORIAL_1_MENU,  // kawa_resort_modify
    TUTORIAL_1_MSG,
    TUTORIAL_1_ARROW,
    TUTORIAL_1_END,
  };

  switch( m_MainSeq )
  {
  case TUTORIAL_1_MENU:  // メニュー画面開始  // kawa_resort_modify
    {
      TalkStart( msg_prwin_tutorial_01_04 );
      SetMainSeq( TUTORIAL_1_MSG );
    } break;

  case TUTORIAL_1_MSG:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( TUTORIAL_1_ARROW );
    } break;

  case TUTORIAL_1_ARROW:
    {
      // ポケマメの木に矢印を表示
      VisibleFingerCursor( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE );
      // モデルアニメの再生
      StartModelAnim( ResortChangeIslandListener::ISLAND_MODELANIM_TREE );

      SetMainSeq( TUTORIAL_1_END );
    } break;

  case TUTORIAL_1_END:
    {
      // 進行度の更新
      SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS );

      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：チュートリアル：ポケマメの木タッチ
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_Tutorial2( void )
{
  enum {
    TUTORIAL_2_BEGIN,
    TUTORIAL_2_MSG,
    TUTORIAL_2_ARROW,
    TUTORIAL_2_END,
  };

  switch( m_MainSeq )
  {
  case TUTORIAL_2_BEGIN:
    {
      // 指カーソルを非表示
      InvisibleFingerCursor();

      static const TalkParam c_Tutorial2TalkParam[] = {
        { msg_prwin_tutorial_01_05, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
        { msg_prwin_tutorial_01_06, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
      };
      // 連続会話の開始
      TalkStart_Loop( c_Tutorial2TalkParam, GFL_NELEMS(c_Tutorial2TalkParam) );
      SetMainSeq( TUTORIAL_2_MSG );
    } break;

  case TUTORIAL_2_MSG:
    {
      if( !IsTalkEnd_Loop() ) break;

      SetMainSeq( TUTORIAL_2_ARROW );
    } break;

  case TUTORIAL_2_ARROW:
    {
      // ポケマメ入れの矢印を表示
      VisibleFingerCursor( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BOX );
      // モデルアニメの再生
      StartModelAnim( ResortChangeIslandListener::ISLAND_MODELANIM_BEANSBOX );

      SetMainSeq( TUTORIAL_2_END );
    } break;

  case TUTORIAL_2_END:
    {
      // 進行度の更新
      SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE );

      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：チュートリアル：ポケマメいれ
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_Tutorial3( void )
{
  enum {
    TUTORIAL_3_BEGIN,
    TUTORIAL_3_MSG_BEGIN,
    TUTORIAL_3_MSG_END,
    TUTORIAL_3_ARROW,
    TUTORIAL_3_END,
  };
  static const TalkParam c_Tutorial3TalkParam[] = {
    { msg_prwin_tutorial_01_07, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_01_08, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };

  switch( m_MainSeq )
  {
  case TUTORIAL_3_BEGIN:
    {
      // 指カーソルを非表示
      InvisibleFingerCursor();

      SetMainSeq( TUTORIAL_3_MSG_BEGIN );
      SetSubSeq( 0 );
    } break;

  case TUTORIAL_3_MSG_BEGIN:
    {
      // 連続会話の開始
      TalkStart_Loop( c_Tutorial3TalkParam, GFL_NELEMS(c_Tutorial3TalkParam) );

      SetMainSeq( TUTORIAL_3_MSG_END );
    } break;

  case TUTORIAL_3_MSG_END:
    {
      // 連続会話の終了
      if( IsTalkEnd_Loop() )
      {
        SetMainSeq( TUTORIAL_3_ARROW );
      }
    } break;

  case TUTORIAL_3_ARROW:
    {
      // 管理人メニューの矢印を表示
      VisibleFingerCursor( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU );

      SetMainSeq( TUTORIAL_3_END );
    } break;

  case TUTORIAL_3_END:
    {
      // 進行度の更新
      SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BOX );

      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：チュートリアル：管理人メニュー
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_Tutorial4( void )
{
  enum {
    TUTORIAL_4_BEGIN,
    TUTORIAL_4_MSG_BEGIN,
    TUTORIAL_4_MSG_END,
    TUTORIAL_4_FADEOUT,
    TUTORIAL_4_FADEWAIT,
    TUTORIAL_4_FADEIN,
    TUTORIAL_4_END,
  };
  static const TalkParam c_Tutorial4TalkParam[] = {
    { msg_prwin_tutorial_01_09, MSG_TYPE_NONE,        gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_01_10, MSG_TYPE_PLAYER_NAME, gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_01_11, MSG_TYPE_NONE,        gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_01_12, MSG_TYPE_NONE,        gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_01_13, MSG_TYPE_NONE,        gfl2::str::StrWin::FINISH_USER },
  };

  switch( m_MainSeq )
  {
  case TUTORIAL_4_BEGIN:
    {
      // 管理人メニューの矢印を非表示
      InvisibleFingerCursor();

      SetMainSeq( TUTORIAL_4_MSG_BEGIN );
      SetSubSeq( 0 );
    } break;

  case TUTORIAL_4_MSG_BEGIN:
    {
      // 連続会話の開始
      TalkStart_Loop( c_Tutorial4TalkParam, GFL_NELEMS(c_Tutorial4TalkParam) );

      SetMainSeq( TUTORIAL_4_MSG_END );
    } break;

  case TUTORIAL_4_MSG_END:
    {
      // 連続会話の終了
      if( IsTalkEnd_Loop() )
      {
        SetMainSeq( TUTORIAL_4_FADEOUT );
      }
    } break;

  case TUTORIAL_4_FADEOUT:
    {
      FadeOut();

      SetMainSeq( TUTORIAL_4_FADEWAIT );
    } break;

  case TUTORIAL_4_FADEWAIT:
    {
      if( !IsFadeEnd() ) break;

      // 進行度の更新
      SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU );
      // ふれあい上画面のセットアップ
      SetupUpperFriendship();

      // 各リゾートの上画面を表示
      SetVisible_UpperDraw( true );
      // 開く
      SetVisible_HomeMenu( false );

      m_WaitFrame = 0;

      SetMainSeq( TUTORIAL_4_FADEIN );
    } break;

  case TUTORIAL_4_FADEIN:
    {
      m_WaitFrame++;

      if( m_WaitFrame < TUTORIAL_FADE_WAIT_FRAME ) break;

      FadeIn();

      SetMainSeq( TUTORIAL_4_END );
    } break;

  case TUTORIAL_4_END:
    {
      if( !IsFadeEnd() ) break;

      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューフローの更新：リゾートの終了
 */
//------------------------------------------------------------------------------
void ResortFlow::UpdateMenuFlow_EndResort( void )
{
  enum {
    END_RESORT_BEGIN,
    END_RESORT_CONFIRM_BEGIN,
    END_RESORT_CONFIRM_END,
    END_RESORT_YES,
    END_RESORT_NO,
    END_RESORT_END,
  };

  switch( m_MainSeq )
  {
  case END_RESORT_BEGIN:
    {
      TalkStart( msg_prwin_info_06_11, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

      SetMainSeq( END_RESORT_CONFIRM_BEGIN );
    } break;

  case END_RESORT_CONFIRM_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      // YESNO表示
      SetDispType_Select( LowerSelectDraw::SEL_TYPE_YESNO );
      OpenLowerMenu( LOW_MENU_SELECT );

      SetMainSeq( END_RESORT_CONFIRM_END );
    } break;

  case END_RESORT_CONFIRM_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_SELECT );

      if( result != RESULT_CONTINUE )
      {
        // ボタン選択画面を閉じる
        CloseLowerMenu( LOW_MENU_SELECT );

        if( result == RESULT_YES )
        {
          TalkStart( msg_prwin_info_06_12 );

          SetMainSeq( END_RESORT_YES );
        }
        else {
          SetMainSeq( END_RESORT_NO );
        }
      }
    } break;

  case END_RESORT_YES:
    {
      if( !IsTalkEnd() ) break;

      // リゾートの終了
      CallApplication( CALL_RESORT_END );

      SetMainSeq( END_RESORT_END );
    } break;

  case END_RESORT_NO:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンイベントの終了
 */
//------------------------------------------------------------------------------
void ResortFlow::EndPokeEvent( void )
{
  // ポケモンイベントの終了を通知
  ResortSequenceListener* listener = m_DrawList.seqListener;
  {
    if( listener )
    {
      listener->ClosePokemonTouchMenu();
    }
  }

  // 管理人メニューを非表示
  SetVisible_HomeMenu( false );
  // 表示を変更
  SetLowerUIType( LowerUIDraw::DISP_TYPE_INPUT_WAIT_NO_MASK );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンイベントの終了チェック
 *
 * @return  "true  == 終了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsEndPokeEvent( void )
{
  // ポケモンイベントの終了を通知
  ResortSequenceListener* listener = m_DrawList.seqListener;
  {
    if( listener )
    {
      return listener->IsPokemonTouchEnable();
    }
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   リゾートの進行度の設定
 *
 * @param   progressID    進行度
 */
//------------------------------------------------------------------------------
void ResortFlow::SetResortProgress( u8 progressID )
{
  if( m_pResortSave )
  {
    m_pResortSave->SetResortProgress( static_cast<Savedata::ResortSave::ResortTutorialState>(progressID) );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リゾートの進行度チェック
 *
 * @param   progressID    進行度
 *
 * @return  "true  == 終了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsResortProgress( u8 progressID ) const
{
  if( !m_pResortSave )
  {
    GFL_ASSERT(0);
    return 0;
  }

  return m_pResortSave->GetResortProgress(
    static_cast<Savedata::ResortSave::ResortTutorialState>(progressID) );
}


//------------------------------------------------------------------------------
/**
 * @brief   フェードイン
 *
 * @param   frame   フェードフレーム
 */
//------------------------------------------------------------------------------
void ResortFlow::FadeIn( u32 frame )
{
  if( m_pFadeMan )
  {
    m_pFadeMan->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, frame );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードアウト
 *
 * @param   frame   フェードフレーム
 */
//------------------------------------------------------------------------------
void ResortFlow::FadeOut( u32 frame )
{
  if( m_pFadeMan )
  {
    gfl2::math::Vector4 beginColor( 0, 0, 0, 0 );
    gfl2::math::Vector4 endColor( 0, 0, 0, 255 );

    m_pFadeMan->RequestOut(
      gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA,
      &beginColor, &endColor, frame );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードの終了判定
 *
 * @return  "true  == 終了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsFadeEnd( void )
{
  if( m_pFadeMan )
  {
    if( !m_pFadeMan->IsEnd( gfl2::Fade::DISP_UPPER ) ) return false;
    if( !m_pFadeMan->IsEnd( gfl2::Fade::DISP_LOWER ) ) return false;
  }

  return true;
}




//==============================================================================
/**
 * @brief   メニュー(下)関連
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   下画面UIの表示を変更
 *
 * @param   type    表示タイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetLowerUIType( LowerUIDraw::DispType type )
{
  LowerUIDraw* lowerUI = m_DrawList.lowerUI;
  {
    if( lowerUI )
    {
      lowerUI->SetDisplayType( type );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示
 *
 * @param   menuID    メニューID
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu( LowerMenu menuID )
{
  void (ResortFlow::*OPEN_LOWER_MENU[LOW_MENU_NUM])( void ) = {
    &ResortFlow::OpenLowerMenu_Farm,
    &ResortFlow::OpenLowerMenu_FarmList,
    &ResortFlow::OpenLowerMenu_AdvResult,
    &ResortFlow::OpenLowerMenu_Gim,
    &ResortFlow::OpenLowerMenu_Select,
    &ResortFlow::OpenLowerMenu_Course,
    &ResortFlow::OpenLowerMenu_BeansSelect,
    &ResortFlow::OpenLowerMenu_UI,
  };

  (this->*OPEN_LOWER_MENU[menuID])();
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示：はたけのようす
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu_Farm( void )
{
  LowerFarmDraw* farm = m_DrawList.lowerFarm;
  {
    if( farm )
    {
      farm->Open();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示：きのみリスト
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu_FarmList( void )
{
  LowerFarmListDraw* farmList = m_DrawList.lowerFarmList;
  {
    if( farmList )
    {
      farmList->Open();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示：たんけん結果
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu_AdvResult( void )
{
  LowerAdventureResultDraw* advResult = m_DrawList.lowerAdvRes;
  {
    if( advResult )
    {
      advResult->Open();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示：うんどう
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu_Gim( void )
{
  LowerGimDraw* gim = m_DrawList.lowerGim;
  {
    if( gim )
    {
      gim->Open();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示：ボタン選択
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu_Select( void )
{
  LowerSelectDraw* slect = m_DrawList.lowerSelect;
  {
    if( slect )
    {
      slect->Open();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示：コース選択
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu_Course( void )
{
  LowerCourseSelectDraw* courseSelect = m_DrawList.lowerCourseSel;
  {
    if( courseSelect )
    {
      courseSelect->Open();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を表示：ポケマメ選択
 */
//------------------------------------------------------------------------------
void ResortFlow::OpenLowerMenu_BeansSelect( void )
{
  LowerBeansSelectDraw* beansSelect = m_DrawList.lowerBeansSel;
  {
    if( beansSelect )
    {
      beansSelect->Open();
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示
 *
 * @param   menuID    メニューID
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu( LowerMenu menuID )
{
  void (ResortFlow::*CLOSE_LOWER_MENU[LOW_MENU_NUM])( void ) = {
    &ResortFlow::CloseLowerMenu_Farm,
    &ResortFlow::CloseLowerMenu_FarmList,
    &ResortFlow::CloseLowerMenu_AdvResult,
    &ResortFlow::CloseLowerMenu_Gim,
    &ResortFlow::CloseLowerMenu_Select,
    &ResortFlow::CloseLowerMenu_Course,
    &ResortFlow::CloseLowerMenu_BeansSelect,
    &ResortFlow::CloseLowerMenu_UI,
  };

  (this->*CLOSE_LOWER_MENU[menuID])();
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示：はたけのようす
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu_Farm( void )
{
  LowerFarmDraw* farm = m_DrawList.lowerFarm;
  {
    if( farm )
    {
      farm->Close();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示：きのみリスト
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu_FarmList( void )
{
  LowerFarmListDraw* farmList = m_DrawList.lowerFarmList;
  {
    if( farmList )
    {
      farmList->Close();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示：たんけん結果
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu_AdvResult( void )
{
  LowerAdventureResultDraw* advResult = m_DrawList.lowerAdvRes;
  {
    if( advResult )
    {
      advResult->Close();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示：うんどう
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu_Gim( void )
{
  LowerGimDraw* gim = m_DrawList.lowerGim;
  {
    if( gim )
    {
      gim->Close();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示：ボタン選択
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu_Select( void )
{
  LowerSelectDraw* slect = m_DrawList.lowerSelect;
  {
    if( slect )
    {
      slect->Close();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示：コース選択
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu_Course( void )
{
  LowerCourseSelectDraw* courseSelect = m_DrawList.lowerCourseSel;
  {
    if( courseSelect )
    {
      courseSelect->Close();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)を非表示：ポケマメ選択
 */
//------------------------------------------------------------------------------
void ResortFlow::CloseLowerMenu_BeansSelect( void )
{
  LowerBeansSelectDraw* beansSelect = m_DrawList.lowerBeansSel;
  {
    if( beansSelect )
    {
      beansSelect->Close();
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得
 *
 * @param   menuID    メニューID
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult( ResortFlow::LowerMenu menuID )
{
  u8 (ResortFlow::*GET_LOWER_MENU_RESULT[LOW_MENU_NUM])( void ) = {
    &ResortFlow::GetLowerMenuResult_Farm,
    &ResortFlow::GetLowerMenuResult_FarmList,
    &ResortFlow::GetLowerMenuResult_AdvResult,
    &ResortFlow::GetLowerMenuResult_Gim,
    &ResortFlow::GetLowerMenuResult_Select,
    &ResortFlow::GetLowerMenuResult_Course,
    &ResortFlow::GetLowerMenuResult_BeansSelect,
    &ResortFlow::GetLowerMenuResult_UI,
  };

  return (this->*GET_LOWER_MENU_RESULT[menuID])();
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：はたけのようす
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_Farm( void )
{
  LowerFarmDraw* farm = m_DrawList.lowerFarm;
  {
    if( farm )
    {
      return farm->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：きのみリスト
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_FarmList( void )
{
  LowerFarmListDraw* farmList = m_DrawList.lowerFarmList;
  {
    if( farmList )
    {
      return farmList->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：たんけん結果
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_AdvResult( void )
{
  LowerAdventureResultDraw* advResult = m_DrawList.lowerAdvRes;
  {
    if( advResult )
    {
      return advResult->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：うんどう
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_Gim( void )
{
  LowerGimDraw* gim = m_DrawList.lowerGim;
  {
    if( gim )
    {
      return gim->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：ボタン選択
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_Select( void )
{
  LowerSelectDraw* slect = m_DrawList.lowerSelect;
  {
    if( slect )
    {
      return slect->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：コース選択
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_Course( void )
{
  LowerCourseSelectDraw* courseSelect = m_DrawList.lowerCourseSel;
  {
    if( courseSelect )
    {
      return courseSelect->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：ポケマメ選択
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_BeansSelect( void )
{
  LowerBeansSelectDraw* beansSelect = m_DrawList.lowerBeansSel;
  {
    if( beansSelect )
    {
      return beansSelect->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニュー(下)のリザルトを取得：UI
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetLowerMenuResult_UI( void )
{
  LowerUIDraw* ui = m_DrawList.lowerUI;
  {
    if( ui )
    {
      return ui->GetResult();
    }
  }

  GFL_ASSERT(0);
  return RESULT_NONE;
}


//------------------------------------------------------------------------------
/**
 * @brief   うんどうの表示タイプを設定
 *
 * @param   enable    決定ボタンの表示フラグ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetDispType_Gim( u8 enable )
{
  LowerGimDraw* gim = m_DrawList.lowerGim;
  {
    if( gim )
    {
      gim->SetDecideBtnEnable( GFL_BOOL_CAST(enable) );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   うんどうの表示タイプを設定
 *
 * @param   enable    決定ボタンの表示フラグ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetDispType_Select( LowerSelectDraw::SelectType type )
{
  LowerSelectDraw* select = m_DrawList.lowerSelect;
  {
    if( select )
    {
      select->SetSelectType( type );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   コース選択の表示タイプを設定
 *
 * @param   type    表示タイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetDispType_Course( u8 type )
{
  LowerCourseSelectDraw* courseSelect = m_DrawList.lowerCourseSel;
  {
    if( courseSelect )
    {
      courseSelect->SetCourseSelectType( type );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケマメ選択の表示タイプを設定
 *
 * @param   type    表示タイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetDispType_BeansSelect( u8 type )
{
  LowerBeansSelectDraw* beansSelect = m_DrawList.lowerBeansSel;
  {
    if( beansSelect )
    {
      beansSelect->SetBeansSelectType( type );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   アプリケーションのコール
 *
 * @param   callID    コールID
 */
//------------------------------------------------------------------------------
void ResortFlow::CallApplication( u8 callID )
{
  ResortFlowListener* listener = m_DrawList.flowListener;
  {
    switch( callID )
    {
    case CALL_RESORT_END:
      {
        listener->OnCallResortEnd();
      } break;
    case CALL_APP_BOX_GIM:
      {
        listener->OnCallBoxGim( NEXTSEQ_BOX_GIM );
      } break;
    case CALL_APP_BOX_GIM_RE:
      {
        listener->OnCallBoxGim( NEXTSEQ_BOX_GIM_RE );
      } break;
    case CALL_APP_BOX_HOTSPA:
      {
        listener->OnCallBoxHotSpa();
      } break;
    case CALL_APP_EGG_DEMO:
      {
        listener->OnCallEggDemo();
      } break;
    case CALL_APP_STATUS:
      {
        listener->OnCallStatus();
      } break;
    case CALL_APP_ZUKAN:
      {
        listener->OnCallZukan();
      } break;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   インデックスの取得：うんどうグループ
 *
 * @param   グループインデックス
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetIndex_GimGroup( void ) const
{
  /*
  LowerGimDraw* gim = m_DrawList.lowerGim;
  {
    if( gim )
    {
      return gim->GetGroupIndex();
    }
  }

  GFL_ASSERT(0);
  return 0;
  */
  return this->GetGimGroupIndex();
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンが配置されているかどうかの取得
 *
 * @param   resortID    リゾートID
 *
 * @return  "true  == 配置あり"
 * @return  "false == 配置なし"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsValidPokemon( u8 resortID )
{
  if( m_pResortSave == NULL )
  {
    return false;
  }

  // ポケモンのインデックス：グループの先頭インデックス
  static const int c_PokeIndexTable[] = {
    0, 6, 12,
  };

  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();
  bool null = false;

  for( u8 i=0; i<GFL_NELEMS(c_PokeIndexTable); ++i )
  {
    // 取得用に生成
    pml::pokepara::CoreParam* cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam(heap);
    {
      int index = c_PokeIndexTable[i];
      {
        // ポケモンをセーブデータから取得
        GetPokemon_FromSavedata( resortID, index, cp );
      }

      bool fastmode = cp->StartFastMode();
      {
        // NULLかどうかを取得
        null = cp->IsNull();
      }
      cp->EndFastMode(fastmode);
    }
    // 生成したパラメータを破棄
    GFL_DELETE cp;

    // NULLじゃなければ配置あり
    if( !null )
    {
      return true;
    }
  }

  // 配置なし
  return false;
}







//==============================================================================
/**
 * @brief   メッセージ関連
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   会話の開始
 *
 * @param   msgID     メッセージID
 * @param   msgType   メッセージタイプ
 * @param   finType   終了タイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::TalkStart( u32 msgID, MsgType msgType, gfl2::str::StrWin::FinishType finType )
{
  TalkParam param;
  {
    param.msgID    = msgID;
    param.msgType  = msgType;
    param.finType  = finType;
  }

  TalkStart( param );
}

//------------------------------------------------------------------------------
/**
 * @brief   会話の開始
 *
 * @param   param   会話パラメータ
 */
//------------------------------------------------------------------------------
void ResortFlow::TalkStart( TalkParam param )
{
  // メッセージタイプごとの設定
  SetMessageType( param.msgType );

  if( m_pTalkMessage )
  {
    m_pTalkMessage->TalkStart( param.msgID, param.finType );
  }
#if PM_DEBUG
  GFL_PRINT( "会話の開始：" );
  GFL_PRINT( "MessageID=[%d]\n", param.msgID );
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   システムメッセージの開始
 *
 * @param   msgID     メッセージID
 */
//------------------------------------------------------------------------------
void ResortFlow::SystemMsgStart( u32 msgID )
{
  if( m_pTalkMessage )
  {
    m_pTalkMessage->TalkStart( msgID, gfl2::str::StrWin::FINISH_USER, App::Tool::WIN_TYPE_SYS_DOWN );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   会話の終了確認
 *
 * @return  "true  == 終了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsTalkEnd( void ) const
{
  if( m_pTalkMessage )
  {
    return m_pTalkMessage->IsTalkEnd();
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   連続会話メッセージの開始
 *
 * @param   paramTable  会話パラメータ配列
 * @param   tableMax    メッセージ配列数
 */
//------------------------------------------------------------------------------
void ResortFlow::TalkStart_Loop( const TalkParam* paramTable, u32 tableMax )
{
  m_pParamTable = paramTable;
  m_MsgTableMax = tableMax;
  m_MsgSeq      = 0;

  // 会話の開始
  TalkStart( m_pParamTable[m_MsgSeq] );

  FUKUSHIMA_PRINT( "TalkStart_Loop\n" );
  FUKUSHIMA_PRINT( "MsgTableMax[%d]\n", m_MsgTableMax );

  for( u32 i=0; i<m_MsgTableMax; ++i )
  {
    FUKUSHIMA_PRINT( "MSG[%d] : ID[%d], TYPE[%d]\n",
      i, m_pParamTable[i].msgID, m_pParamTable[i].msgType );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   連続会話メッセージが終了したかどうか
 *
 * @return  "true  == 終了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsTalkEnd_Loop( void )
{
  // 会話が終了していない
  if( !IsTalkEnd() ) return false;

  m_MsgSeq++;

  FUKUSHIMA_PRINT( "MsgSeq[%d] == MsgTableMax[%d]\n", m_MsgSeq, m_MsgTableMax );

  // 連続会話メッセージの終了
  if( m_MsgSeq == m_MsgTableMax )
  {
    return true;
  }
  // 次の会話の開始
  else {
    TalkStart( m_pParamTable[m_MsgSeq] );
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージタイプの設定
 *
 * @param   type    メッセージタイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessageType( MsgType type )
{
  switch( type )
  {
  case MSG_TYPE_PLAYER_NAME:
    {
      m_pWordSet->RegisterPlayerName( 0, m_pMyStatus );
    } break;

  case MSG_TYPE_POKE_NICKNAME:
    {
      m_pWordSet->RegisterPokeNickName( 0, m_MenuParam.pokeCoreParam );
    } break;

  case MSG_TYPE_POKE_MONSNAME:
    {
      m_pWordSet->RegisterPokeMonsName( 0, m_MenuParam.pokeCoreParam );
    } break;

  case MSG_TYPE_EVENT_GIM:
    m_pWordSet->RegisterPokeMonsName( 0, m_MenuParam.pokeCoreParam );
    m_pWordSet->RegisterNumber( 1, m_MsgValue, 3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
    break;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：プラスボーナス
 *
 * @param   bonus   ボーナス値
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_PlusBonus( u8 bonus )
{
  static const u32 c_PlusBonusMsgID[] = {
    msg_prwin_info_01_01, //!< ボーナス値０
    msg_prwin_info_01_01, //!< ボーナス値１
    msg_prwin_info_01_02, //!< ボーナス値２
    msg_prwin_info_01_03, //!< ボーナス値３
  };

  TalkStart( c_PlusBonusMsgID[bonus] );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：マイナスボーナス
 *
 * @param   bonus   ボーナス値
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_MinusBonus( u8 bonus )
{
  static const u32 c_MinusBonusMsgID[] = {
    msg_prwin_info_01_04, //!< ボーナス値０
    msg_prwin_info_01_05, //!< ボーナス値１
    msg_prwin_info_01_06, //!< ボーナス値２
    msg_prwin_info_01_06, //!< ボーナス値３
  };

  TalkStart( c_MinusBonusMsgID[bonus] );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：開発完了
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_ResortDeveloped( void )
{
  static const u32 c_FriendshipCompMsgID[RESORT_DEV_LV_NUM] = {
    msg_prwin_tutorial_02_01,   //!< 数合わせ
    msg_prwin_tutorial_02_01,   //!< ふれあいLV2
    msg_prwin_tutorial_02_02,   //!< ふれあいLV3
  };
  static const u32 c_FarmCompMsgID[RESORT_DEV_LV_NUM] = {
    msg_prwin_tutorial_02_03,   //!< きのみLV1
    msg_prwin_tutorial_02_04,   //!< きのみLV2
    msg_prwin_tutorial_02_05,   //!< きのみLV3
  };
  static const u32 c_AdventureCompMsgID[RESORT_DEV_LV_NUM] = {
    msg_prwin_tutorial_02_06,   //!< たんけんLV1
    msg_prwin_tutorial_02_07,   //!< たんけんLV2
    msg_prwin_tutorial_02_08,   //!< たんけんLV3
  };
  static const u32 c_GimCompMsgID[RESORT_DEV_LV_NUM] = {
    msg_prwin_tutorial_02_09,   //!< うんどうLV1
    msg_prwin_tutorial_02_10,   //!< うんどうLV2
    msg_prwin_tutorial_02_11,   //!< うんどうLV3
  };
  static const u32 c_HotspaCompMsgID[RESORT_DEV_LV_NUM] = {
    msg_prwin_tutorial_02_12,   //!< おんせんLV1
    msg_prwin_tutorial_02_13,   //!< おんせんLV2
    msg_prwin_tutorial_02_14,   //!< おんせんLV3
  };
  static const u32* c_ResortCompMsgID[] = {
    c_FriendshipCompMsgID,    //!< ふれあい
    c_FarmCompMsgID,          //!< きのみ
    c_AdventureCompMsgID,     //!< たんけん
    c_GimCompMsgID,           //!< うんどう
    c_HotspaCompMsgID,        //!< おんせん
  };

  u8 resort = m_DrawList.lowerCourseSel->GetDevResort();
  {
    u8 level  = util::GetResortLevel( m_pResortSave, resort );
    {
      const u32* table = c_ResortCompMsgID[resort];
      {
        TalkStart( table[level - 1] );
      }
    }

    FUKUSHIMA_PRINT( "SetMessage_ResortDeveloped : resort[%d], level[%d]\n", resort, level );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：リゾートの説明
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_ResortDescription( void )
{
  static const TalkParam c_FriendshipDescLv2Msg[] = {
    { msg_prwin_tutorial_02_15, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_FriendshipDescLv3Msg[] = {
    { msg_prwin_tutorial_02_16, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_FarmDescLv1Msg[] = {
    { msg_prwin_tutorial_02_17, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_02_29, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_FarmDescLv2Msg[] = {
    { msg_prwin_tutorial_02_18, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_FarmDescLv3Msg[] = {
    { msg_prwin_tutorial_02_19, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_AdventureDescLv1Msg[] = {
    { msg_prwin_tutorial_02_20, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_02_30, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_AdventureDescLv2Msg[] = {
    { msg_prwin_tutorial_02_21, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_AdventureDescLv3Msg[] = {
    { msg_prwin_tutorial_02_22, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_GimDescLv1Msg[] = {
    { msg_prwin_tutorial_02_23, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_02_31, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_GimDescLv2Msg[] = {
    { msg_prwin_tutorial_02_24, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_GimDescLv3Msg[] = {
    { msg_prwin_tutorial_02_25, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_HotspaDescLv1Msg[] = {
    { msg_prwin_tutorial_02_26, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
    { msg_prwin_tutorial_02_32, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_HotspaDescLv2Msg[] = {
    { msg_prwin_tutorial_02_27, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };
  static const TalkParam c_HotspaDescLv3Msg[] = {
    { msg_prwin_tutorial_02_28, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
  };

  // 各リゾートレベルごとのメッセージ
  static const TalkParam* c_FriendshipLvMsg[] = {
    NULL,
    c_FriendshipDescLv2Msg,
    c_FriendshipDescLv3Msg,
  };
  static const TalkParam* c_FarmLvMsg[] = {
    c_FarmDescLv1Msg,
    c_FarmDescLv2Msg,
    c_FarmDescLv3Msg,
  };
  static const TalkParam* c_AdventureLvMsg[] = {
    c_AdventureDescLv1Msg,
    c_AdventureDescLv2Msg,
    c_AdventureDescLv3Msg,
  };
  static const TalkParam* c_GimLvMsg[] = {
    c_GimDescLv1Msg,
    c_GimDescLv2Msg,
    c_GimDescLv3Msg,
  };
  static const TalkParam* c_HotspaLvMsg[] = {
    c_HotspaDescLv1Msg,
    c_HotspaDescLv2Msg,
    c_HotspaDescLv3Msg,
  };

  // 各リゾートのメッセージ
  static const TalkParam** c_ResortDescMsg[] = {
    c_FriendshipLvMsg,
    c_FarmLvMsg,
    c_AdventureLvMsg,
    c_GimLvMsg,
    c_HotspaLvMsg,
  };

  static const u32 c_FriendshipLvMsgNum[] = {
    0,
    GFL_NELEMS(c_FriendshipDescLv2Msg),
    GFL_NELEMS(c_FriendshipDescLv3Msg),
  };
  static const u32 c_FarmLvMsgNum[] = {
    GFL_NELEMS(c_FarmDescLv1Msg),
    GFL_NELEMS(c_FarmDescLv2Msg),
    GFL_NELEMS(c_FarmDescLv3Msg),
  };
  static const u32 c_AdventureLvMsgNum[] = {
    GFL_NELEMS(c_AdventureDescLv1Msg),
    GFL_NELEMS(c_AdventureDescLv2Msg),
    GFL_NELEMS(c_AdventureDescLv3Msg),
  };
  static const u32 c_GimLvMsgNum[] = {
    GFL_NELEMS(c_GimDescLv1Msg),
    GFL_NELEMS(c_GimDescLv2Msg),
    GFL_NELEMS(c_GimDescLv3Msg),
  };
  static const u32 c_HotspaLvMsgNum[] = {
    GFL_NELEMS(c_HotspaDescLv1Msg),
    GFL_NELEMS(c_HotspaDescLv2Msg),
    GFL_NELEMS(c_HotspaDescLv3Msg),
  };
  static const u32* c_ResortDescMsgNum[] = {
    c_FriendshipLvMsgNum,
    c_FarmLvMsgNum,
    c_AdventureLvMsgNum,
    c_GimLvMsgNum,
    c_HotspaLvMsgNum,
  };

  u8 resort = m_DrawList.lowerCourseSel->GetDevResort();
  {
    const TalkParam** resortTable = c_ResortDescMsg[resort];
    const u32* resortTableNum     = c_ResortDescMsgNum[resort];

    u8 level = util::GetResortLevel( m_pResortSave, resort );
    {
      const TalkParam* levelTable = resortTable[level - 1];
      const u32 levelTableNum     = resortTableNum[level - 1];

      // 連続会話メッセージの開始
      TalkStart_Loop( levelTable, levelTableNum );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケモンがいない
 *
 * @param   type    ポケマメいれのタイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_BeansSelectPokeNone( u8 type )
{
  static const u32 c_BeansSelectPokeNoneMsgID[] = {
    msg_prwin_sys_common_04,
    msg_prwin_sys_common_04,
    msg_prwin_sys_03_08,
    msg_prwin_sys_04_12,
    msg_prwin_sys_05_07,
    msg_prwin_sys_common_02,
  };
  
  TalkStart( c_BeansSelectPokeNoneMsgID[type] );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケマメ入れ後
 *
 * @param   type    ポケマメいれのタイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_BeansSelectAfter( u8 type )
{
  static const u32 c_BeansSelectAfterMsgID[] = {
    msg_prwin_sys_06_01,
    msg_prwin_sys_06_02,
    msg_prwin_sys_06_03,
    msg_prwin_sys_06_04,
    msg_prwin_sys_06_05,
    msg_prwin_sys_common_02,
  };
  
  TalkStart( c_BeansSelectAfterMsgID[type] );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケマメ入れ後(システム)
 *
 * @param   type    ポケマメいれのタイプ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_BeansSelectSystem( u8 type )
{
  static const u32 c_BeansSelectSystemMsg[] = {
    msg_prwin_sys_06_06,
    msg_prwin_sys_06_07,
    msg_prwin_sys_06_08,
    msg_prwin_sys_06_09,
    msg_prwin_sys_06_10,
    msg_prwin_sys_common_02,
  };

  SystemMsgStart( c_BeansSelectSystemMsg[type] );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：おんせんタマゴ状態
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_EggState( void )
{
  u32 natuki = GetEggFamiliarity();

  if( natuki <= 5 )
  {
    TalkStart( msg_prwin_sys_05_04, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
  }
  else if( natuki <= 10 )
  {
    TalkStart( msg_prwin_sys_05_05, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
  }
  else if( natuki <= 40 )
  {
    TalkStart( msg_prwin_sys_05_06, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
  }
  else {
    TalkStart( msg_prwin_info_06_06, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケモンイベント(BOX)性格
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_EventSeikaku( void )
{
  static const u32 c_SeikakuMsg[] = {
    msg_prwin_poke_ev_01_01,
    msg_prwin_poke_ev_01_02,
    msg_prwin_poke_ev_01_03,
    msg_prwin_poke_ev_01_04,
    msg_prwin_poke_ev_01_05,
    msg_prwin_poke_ev_01_06,
    msg_prwin_poke_ev_01_07,
    msg_prwin_poke_ev_01_08,
    msg_prwin_poke_ev_01_09,
    msg_prwin_poke_ev_01_10,
    msg_prwin_poke_ev_01_11,
    msg_prwin_poke_ev_01_12,
    msg_prwin_poke_ev_01_13,
    msg_prwin_poke_ev_01_14,
    msg_prwin_poke_ev_01_15,
    msg_prwin_poke_ev_01_16,
    msg_prwin_poke_ev_01_17,
    msg_prwin_poke_ev_01_18,
    msg_prwin_poke_ev_01_19,
    msg_prwin_poke_ev_01_20,
    msg_prwin_poke_ev_01_21,
    msg_prwin_poke_ev_01_22,
    msg_prwin_poke_ev_01_23,
    msg_prwin_poke_ev_01_24,
    msg_prwin_poke_ev_01_25,
  };

  pml::pokepara::Seikaku seikaku;
  pml::pokepara::CoreParam* cp = m_MenuParam.pokeCoreParam;
  {
    bool fastmode_flg = cp->StartFastMode();
    {
      seikaku = cp->GetSeikaku();
    }
    cp->EndFastMode( fastmode_flg );
  }

  // メッセージの設定
  TalkStart( c_SeikakuMsg[seikaku], MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケモンイベント(BOX)いちゃいちゃ
 *
 * @return  "true  == 一致"
 * @return  "false == 不一致"
 */
//------------------------------------------------------------------------------
bool ResortFlow::SetMessage_EventFriendship( void )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();
  {
    // ふれあいリゾートの０番目が対象のポケモンとなるので、
    // 直接パラメータを取得する
    pml::pokepara::CoreParam* subCoreParam = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );
    {
      u8 resortID = GetMenuParam()->resortID;
      GetPokemon_FromSavedata( resortID, 0, subCoreParam ); // @fix NMCat[1779]
    }

    // タッチしたポケモン名
    gfl2::str::StrBuf* mainStrBuf = GFL_NEW_LOW(heap) gfl2::str::StrBuf( 64, heap );
    {
      pml::pokepara::CoreParam* cp = m_MenuParam.pokeCoreParam;
      {
        bool fastmode = cp->StartFastMode();
        {
          cp->GetNickName( mainStrBuf );
        }
        cp->EndFastMode( fastmode );
      }
    }

    // 対象のポケモン名
    gfl2::str::StrBuf* subStrBuf  = GFL_NEW_LOW(heap) gfl2::str::StrBuf( 64, heap );
    {
      pml::pokepara::CoreParam* cp = subCoreParam;
      {
        bool fastmode = cp->StartFastMode();
        {
          cp->GetNickName( subStrBuf );
        }
        cp->EndFastMode( fastmode );
      }
    }

    // 名前を比較
    bool comp = mainStrBuf->Compare( *subStrBuf );
    {
      if( !comp )
      {
        // 不一致
        m_pWordSet->RegisterPokeNickName( 0, m_MenuParam.pokeCoreParam );
        m_pWordSet->RegisterPokeNickName( 1, subCoreParam );

        TalkStart( msg_prwin_poke_ev_06_01, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
      }
    }

    GFL_DELETE mainStrBuf;
    GFL_DELETE subStrBuf;
    GFL_DELETE subCoreParam;

    return comp;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケモンイベント(BOX)通常
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_EventNormal( void )
{
  static const TalkParam c_NormalEventMsgID[] = {
    { 0,                       MSG_TYPE_NONE,          gfl2::str::StrWin::FINISH_NONE },
    { 0,                       MSG_TYPE_NONE,          gfl2::str::StrWin::FINISH_NONE },
    { msg_prwin_poke_ev_02_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE },
    { msg_prwin_poke_ev_03_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE },
    { msg_prwin_poke_ev_04_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE },
    { msg_prwin_poke_ev_05_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE },
    { 0,                       MSG_TYPE_NONE,          gfl2::str::StrWin::FINISH_NONE },
    { msg_prwin_poke_ev_08_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE },
    { msg_prwin_poke_ev_09_01, MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE },
  };

  // ポケモンイベントワークの取得
  u8 work = GetPokemonEventWork();
  {
    TalkStart( c_NormalEventMsgID[work] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケモンイベント(BOX)ポケマメ取得
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_GetPokemame( void )
{
  static const u32 c_SysGetPokemameMsg[] = {
    msg_prwin_poke_ev_07_02,
    msg_prwin_poke_ev_07_03,
    msg_prwin_poke_ev_07_04,
  };

  // システムメッセージ
  u8 work = GetPokemonEventWork();
  {
    // @fix cov_ctr[11899]
    if( work >= pml::pokepara::RESORTEVENT_POKEMAME_GET1 && work <= pml::pokepara::RESORTEVENT_POKEMAME_GET3 )
    {
      u32 index = work - pml::pokepara::RESORTEVENT_POKEMAME_GET1;

      SystemMsgStart( c_SysGetPokemameMsg[index] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定：ポケモンイベント(BOX)きのみリゾート専用
 */
//------------------------------------------------------------------------------
void ResortFlow::SetMessage_FarmResort( void )
{
  pml::pokepara::Seikaku seikaku = pml::pokepara::SEIKAKU_NUM;
  {
    const pml::pokepara::CoreParam* cp = m_MenuParam.pokeCoreParam;
    {
      bool fastmode = cp->StartFastMode();
      {
        // 性格の取得
        seikaku = cp->GetSeikaku();
      }
      cp->EndFastMode( fastmode );
    }
  }

  // きのみリゾート用イベントメッセージ
  static const u32 c_FarmEventMsgTable[] = {
    msg_prwin_poke_ev_15_01,
    msg_prwin_poke_ev_15_02,
    msg_prwin_poke_ev_15_06,
    msg_prwin_poke_ev_15_03,
    msg_prwin_poke_ev_15_04,
    msg_prwin_poke_ev_15_05,
    msg_prwin_poke_ev_15_07,
    msg_prwin_poke_ev_15_08,
    msg_prwin_poke_ev_15_09,
    msg_prwin_poke_ev_15_10,
    msg_prwin_poke_ev_15_11,
    msg_prwin_poke_ev_15_12,
    msg_prwin_poke_ev_15_13,
    msg_prwin_poke_ev_15_14,
    msg_prwin_poke_ev_15_15,
    msg_prwin_poke_ev_15_16,
    msg_prwin_poke_ev_15_17,
    msg_prwin_poke_ev_15_18,
    msg_prwin_poke_ev_15_19,
    msg_prwin_poke_ev_15_20,
    msg_prwin_poke_ev_15_21,
    msg_prwin_poke_ev_15_22,
    msg_prwin_poke_ev_15_23,
    msg_prwin_poke_ev_15_24,
    msg_prwin_poke_ev_15_25,
  };

  GFL_ASSERT( seikaku < GFL_NELEMS(c_FarmEventMsgTable) );

  GFL_PRINT( "FarmEvent : Seikaku[%d], msgID[%d]\n", seikaku, c_FarmEventMsgTable[seikaku] );

  TalkStart( c_FarmEventMsgTable[seikaku], MSG_TYPE_POKE_NICKNAME, gfl2::str::StrWin::FINISH_NONE );
}



//==============================================================================
/**
 * @brief   イベント関連
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   起動時イベント：開始分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::FirstEvent_Begin( void )
{
  // 進行度チェック
  if( IsResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING ) )
  {
    // レコード：回数をカウント
    Savedata::IncRecord( Savedata::Record::RECID_POKERESORT );

    // チュートリアルが終わっているならボーナス分岐へ
    SetMainSeq( FEF_BONUS_BEGIN );
  }
  else if( GetBoxPokemonCount() == 0 )
  {
    // 管理人メニューの表示
    SetVisible_HomeMenu( true );

    // BOXにポケモンがいないのでリゾートを終わらせる
    TalkStart( msg_prwin_tutorial_03_01 );
    SetMainSeq( FEF_RESORT_END );
  }
  // 初回チュートリアル
  else {
    static const TalkParam c_FirstTutorialParam[] = {
      { msg_prwin_tutorial_01_01, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
      { msg_prwin_tutorial_01_02, MSG_TYPE_PLAYER_NAME, gfl2::str::StrWin::FINISH_USER },
      { msg_prwin_tutorial_01_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
    };
    static const TalkParam c_FirstTutorialReunionParam[] = {
      { msg_prwin_tutorial_01_14, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
      { msg_prwin_tutorial_01_02, MSG_TYPE_PLAYER_NAME, gfl2::str::StrWin::FINISH_USER },
      { msg_prwin_tutorial_01_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
    };

    // イベントワークの取得
    GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
    Field::EventWork* eventWork = gameData->GetEventWork();

    // モーンと出会っているか？
    if( eventWork->GetEventWork( WK_SCENE_MAIN_FLOW_001 ) < SEQ_M01_EV1090_END )
    {
      /// 出会っていない：初回チュートリアルメッセージ
      TalkStart_Loop( c_FirstTutorialParam, GFL_NELEMS( c_FirstTutorialParam ) );
    }
    else {
      /// 出会った：初回チュートリアル(モーンと再会版)メッセージ
      TalkStart_Loop( c_FirstTutorialReunionParam, GFL_NELEMS( c_FirstTutorialReunionParam ) );
    }

    // 初回フラグの更新
    SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING );

    // 管理人メニューの表示
    SetVisible_HomeMenu( true );

    // レコード：回数をカウント
    Savedata::IncRecord( Savedata::Record::RECID_POKERESORT );

    SetMainSeq( FEF_TUTORIAL );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   起動時イベント：ボーナス分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::FirstEvent_Bonus( void )
{
  u8 prevBonus = m_pInitLogic->GetPreviousBonusLevel();
  u8 nextBonus = m_pInitLogic->GetBonusLevel();

  GFL_PRINT( "ボーナスイベント\n" );
  GFL_PRINT( "prevBonus[%d] nextBonus[%d]\n", prevBonus, nextBonus );

  if( prevBonus == nextBonus )
  {
    SetMainSeq( FEF_DEVELOP_BEGIN );
  }
  else {
    // 管理人メニューの表示
    SetVisible_HomeMenu( true );

    if( prevBonus < nextBonus )
    {
      SetMessage_PlusBonus( nextBonus );
    }
    else {
      SetMessage_MinusBonus( nextBonus );
    }

    SetMainSeq( FEF_BONUS );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   起動時イベント：開発分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::FirstEvent_Develop( void )
{
  // おすすめリゾートID
  u8 recResortID = GetDevelopResort();

  if( recResortID == RESORT_ID_MAX )
  {
    if( !m_FadeFlg )
    {
      // リゾートの開始
      m_DrawList.flowListener->RequestFadeIn();
    }

    SetMainSeq( FEF_END );
  }
  else {
    // 管理人メニューの表示
    SetVisible_HomeMenu( true );

    static const u32 c_RecDevResort[] = {
      msg_prwin_info_06_13,
      msg_prwin_info_06_14,
      msg_prwin_info_06_15,
      msg_prwin_info_06_16,
      msg_prwin_info_06_17,
    };
    GFL_ASSERT( recResortID < RESORT_ID_MAX );
    TalkStart( c_RecDevResort[recResortID] );

    SetMainSeq( FEF_DEVELOP );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   開発可能なリゾートIDの取得
 *
 * @return  開発可能なリゾートID
 * @return  開発不可の場合、RESORT_ID_MAXを返す
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetDevelopResort( void )
{
  // ボックス(タマゴ以外)＋うんどう＋おんせんのポケモンの数
  u32 pokeNum = util::GetBoxPokeAndResortPokeNum( m_pResortSave, m_pBoxPokemon, m_pHeap );
  // 所持ポケマメの取得
  u32 beansCount[3] = { 0, 0, 0 };
  {
    util::GetRankBeansCount( m_pResortSave, beansCount );
  }

  u32 recResortID = RESORT_ID_MAX;  //!< おすすめ開発リゾートID
  u8  recResortLV = RESORT_LV_NUM;  //!< おすすめ開発リゾートLV

  // 開発可能なリゾートのチェック
  for( u32 i=0; i<RESORT_ID_MAX; ++i )
  {
    // レベルを取得
    u8 level = util::GetResortLevel( m_pResortSave, i );

    // レベルが最大なら次へ
    if( level == RESORT_LV_3 ) continue;

    u32 devID = RESORT_ID_MAX;
    u8  devLV = RESORT_LV_NUM;

    u32 needPoke  = c_NeedItemNum[i][level][0]; //!< 必要なポケモン数
    u32 needBeans = c_NeedItemNum[i][level][1]; //!< 必要なポケマメ数

    // 開発可能なリゾート
    if( needPoke <= pokeNum && needBeans <= beansCount[0] )
    {
      devID = i;
      devLV = level;
    }

    // おすすめ開発の更新
    if( recResortID != devID && recResortLV > devLV )
    {
      recResortID = devID;
      recResortLV = devLV;
    }
  }

  return recResortID;
}




//--------------------------------------------------------------------------
/**
 * @brief   管理人イベント：開始分岐
 */
//--------------------------------------------------------------------------
void ResortFlow::HomeMenuEvent_Begin( void )
{
  // 初回チュートリアル：管理人メニュー起動時
  if( !IsResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ) )
  {
    SetMainSeq( HMF_TUTORIAL );
  }
  // 管理人メニューへ
  else {
    TalkStart( msg_prwin_sys_00_01, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );   //!< 「なんのようだい？」
    SetMainSeq( HMF_INTRO_WAIT );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   管理人イベント：メニュー選択
 */
//--------------------------------------------------------------------------
void ResortFlow::HomeMenuEvent_MenuSelect( void )
{
  u8 result = GetLowerMenuResult( LOW_MENU_SELECT );

  // ボタンの選択が行われるまで待機
  if( result == RESULT_CONTINUE ) return;

  // 選択ボタンを非表示
  CloseLowerMenu( LOW_MENU_SELECT );

  // サブシーケンスを初期化
  SetSubSeq( 0 );

  // 開発へ
  if( result == RESULT_DEVELOP )
  {
    SetMainSeq( HMF_DEVELOP );
  }
  // 交換へ
  else if( result == RESULT_TRADE )
  {
    u32 beansCount[3];
    {
       util::GetRankBeansCount( m_pResortSave, beansCount );
    }

    // ノーマルポケマメが規定数未満なら交換
    if( beansCount[0] < 300 )
    {
      SetMainSeq( HMF_TRADE );
    }
    else {
      TalkStart( msg_prwin_info_06_07 );
      SetMainSeq( HMF_NOT_TRADE );
    }
  }
  // マメビン流し
  else if( result == RESULT_MAME_SEND )
  {
    SetMainSeq( HMF_BEANS_SEND );
  }
  // キャンセル
  else if( result == RESULT_CANCEL )
  {
    SetMainSeq( HMF_END );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   管理人イベント：開発
 */
//--------------------------------------------------------------------------
void ResortFlow::HomeMenuEvent_Develop( void )
{
  enum {
    HMF_DEV_BEGIN,
    HMF_DEV_TUTORIAL_BEGIN,
    HMF_DEV_TUTORIAL_1,
    HMF_DEV_TUTORIAL_2,
    HMF_DEV_TUTORIAL_END,
    HMF_DEV_SELECT_BEGIN,
    HMF_DEV_SELECT_END,
    HMF_DEV_DIRECT_BEGIN,
    HMF_DEV_DIRECT_FADE_OUT,
    HMF_DEV_DIRECT_DEVELOP,
    HMF_DEV_DIRECT_FADE_IN,
    HMF_DEV_DIRECT_COMPLETE_BEGIN,
    HMF_DEV_DIRECT_COMPLETE_END,
    HMF_DEV_DIRECT_EXPLAIN_BEGIN,
    HMF_DEV_DIRECT_EXPLAIN_END,
    HMF_DEV_DIRECT_ALL_DEVELOPED_BEGIN,
    HMF_DEV_DIRECT_ALL_DEVELOPED_MSG,
    HMF_DEV_DIRECT_ALL_DEVELOPED_GET_BEANS_BEGIN,
    HMF_DEV_DIRECT_ALL_DEVELOPED_GET_BEANS_END,
    HMF_DEV_DIRECT_ALL_DEVELOPED_END,
    HMF_DEV_ALL_DEVELOPED,
    HMF_DEV_ALL_DEVELOPED_END,
    HMF_DEV_END_FADE_OUT,
    HMF_DEV_END_FADE_IN,
    HMF_DEV_END_FADE_END,
    HMF_DEV_END,
  };

  switch( m_SubSeq )
  {
  case HMF_DEV_BEGIN:
    {
      // チュートリアル：開発
      if( !IsResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_DEVELOP ) )
      {
        SetSubSeq( HMF_DEV_TUTORIAL_BEGIN );
      }
      // すべて開発済み
      else if( IsAllResortDeveloped() )
      {
        SetSubSeq( HMF_DEV_ALL_DEVELOPED );
      }
      else {
        SetSubSeq( HMF_DEV_SELECT_BEGIN );
      }
    } break;

  //---[ チュートリアル ]---//
  case HMF_DEV_TUTORIAL_BEGIN:
    {
      // チュートリアルフラグを更新：開発初回
      SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_DEVELOP );

      TalkStart( msg_prwin_info_06_01 );
      SetSubSeq( HMF_DEV_TUTORIAL_1 );
    } break;

  case HMF_DEV_TUTORIAL_1:
    {
      if( !IsTalkEnd() ) break;

      TalkStart( msg_prwin_info_06_02 );
      SetSubSeq( HMF_DEV_TUTORIAL_2 );
    } break;

  case HMF_DEV_TUTORIAL_2:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( HMF_DEV_SELECT_BEGIN );
    } break;

  //---[ 開発するリゾートの選択 ]---//
  case HMF_DEV_SELECT_BEGIN:
    {
      SetDispType_Course( COURSE_SELECT_DEVELOP );
      OpenLowerMenu( LOW_MENU_COURSE );

      SetSubSeq( HMF_DEV_SELECT_END );
    } break;

  case HMF_DEV_SELECT_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_COURSE );

      if( result != RESULT_CONTINUE )
      {
        // コース選択を閉じる
        CloseLowerMenu( LOW_MENU_COURSE );

        if( result == RESULT_CANCEL )
        {
          SetMainSeq( HMF_INTRO );  //!< 管理人メニュートップへ
        }
        else {
          SetSubSeq( HMF_DEV_DIRECT_BEGIN );
        }
      }
    } break;

  //---[ 開発演出の開始 ]---//
  case HMF_DEV_DIRECT_BEGIN:
    {
      TalkStart( msg_prwin_sys_00_09 );   //!< 「オッケー！さっそくさぎょう開始だ！」
      SetSubSeq( HMF_DEV_DIRECT_FADE_OUT );
    } break;

  case HMF_DEV_DIRECT_FADE_OUT:   // 暗転開始
    {
      if( !IsTalkEnd() ) break;

      // フェードアウト
      FadeOut();

      SetSubSeq( HMF_DEV_DIRECT_DEVELOP );
    } break;

  case HMF_DEV_DIRECT_DEVELOP:  // 暗転後の更新処理
    {
      if( !IsFadeEnd() ) break;

      // 会話ウィンドウを閉じる
      if( m_pTalkMessage )
      {
        m_pTalkMessage->TalkEnd( false );    // @fix NMCat[3177]
      }

      // 開発中サウンドの再生
      Sound::PlaySE( SEQ_SE_SYS_NJ_051 );
      // レベルアップによる表示を更新
      UpperDraw* upperMenu = m_DrawList.upperMenu;
      {
        upperMenu->UpdateResortLevel();
      }
      // 下画面の表示を切り替え
      SetLowerUIType( LowerUIDraw::DISP_TYPE_INPUT_WAIT_NO_MASK );
      m_DrawList.lowerUI->SetInputEnabled( false );

      // 移動できるリゾートが増える可能性もあるので更新
      ResortFlowListener* flowListener = m_DrawList.flowListener;
      u32 resort_id = m_DrawList.lowerCourseSel->GetDevResort();
      {
        flowListener->OnUpdateResort();
        flowListener->ChangeResort( resort_id );
      }
      // ポケモン更新
      if( resort_id != RESORT_ID_GIM && resort_id != RESORT_ID_HOTSPA )
      {
        m_pResortSave->SetPokemonEventTime( 0, 0, 0 );
        m_pInitLogic->PokemonEventStartFunc(true);//例外的に更新する
      }

      SetSubSeq( HMF_DEV_DIRECT_FADE_IN );
    } break;

  case HMF_DEV_DIRECT_FADE_IN:    // 明転開始
    {
      if( !Sound::IsSEFinished( SEQ_SE_SYS_NJ_051 ) ) break;

      // フェードイン
      FadeIn();

      SetSubSeq( HMF_DEV_DIRECT_COMPLETE_BEGIN );
    } break;

  case HMF_DEV_DIRECT_COMPLETE_BEGIN:
    {
      if( !IsFadeEnd() ) break;

      // リゾートの完成メッセージのセット
      SetMessage_ResortDeveloped();

      // リゾートの完成MEの再生
      Sound::StartME( STRM_ME_RESORT_FINISH );

      SetSubSeq( HMF_DEV_DIRECT_COMPLETE_END );
    } break;

  case HMF_DEV_DIRECT_COMPLETE_END:
    {
      // 会話の終了
      if( !IsTalkEnd() ) break;

      // 開発したリゾートの説明
      SetSubSeq( HMF_DEV_DIRECT_EXPLAIN_BEGIN );
    } break;

  case HMF_DEV_DIRECT_EXPLAIN_BEGIN:
    {
      // リゾートの説明メッセージのセット
      SetMessage_ResortDescription();

      SetSubSeq( HMF_DEV_DIRECT_EXPLAIN_END );
    } break;

  case HMF_DEV_DIRECT_EXPLAIN_END:
    {
      if( !IsTalkEnd_Loop() ) break;

      // 全ての開発が完了したか
      if( !IsAllResortDeveloped() )
      {
        SetSubSeq( HMF_DEV_END_FADE_OUT );
      }
      else {
        // すべての開発が完了
        SetSubSeq( HMF_DEV_DIRECT_ALL_DEVELOPED_BEGIN );
      }
    } break;

  //---[ 全開発の完了演出 ]---//
  case HMF_DEV_DIRECT_ALL_DEVELOPED_BEGIN:
    {
      TalkStart( msg_prwin_info_03_01, MSG_TYPE_PLAYER_NAME );

      SetSubSeq( HMF_DEV_DIRECT_ALL_DEVELOPED_MSG );
    } break;

  case HMF_DEV_DIRECT_ALL_DEVELOPED_MSG:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( HMF_DEV_DIRECT_ALL_DEVELOPED_GET_BEANS_BEGIN );
    } break;

  case HMF_DEV_DIRECT_ALL_DEVELOPED_GET_BEANS_BEGIN:
    {
      m_pResortSave->SetPokeBeansNum( Savedata::ResortSave::POKEBEANS_NIJI, 10 );
      SystemMsgStart( msg_prwin_info_03_02 );

      SetSubSeq( HMF_DEV_DIRECT_ALL_DEVELOPED_GET_BEANS_END );
    } break;

  case HMF_DEV_DIRECT_ALL_DEVELOPED_GET_BEANS_END:
    {
      if( !IsTalkEnd() ) break;

      TalkStart( msg_prwin_info_03_03 );

      SetSubSeq( HMF_DEV_DIRECT_ALL_DEVELOPED_END );
    } break;

  case HMF_DEV_DIRECT_ALL_DEVELOPED_END:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( HMF_DEV_END_FADE_OUT );
    } break;

  //---[ 開発済み ]---//
  case HMF_DEV_ALL_DEVELOPED:
    {
      TalkStart( msg_prwin_info_06_05, MSG_TYPE_PLAYER_NAME );

      SetSubSeq( HMF_DEV_ALL_DEVELOPED_END );
    } break;

  case HMF_DEV_ALL_DEVELOPED_END:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( HMF_INTRO );    //!< 管理人メニューへ
    } break;

  //---[ 開発終了演出 ]---//
  case HMF_DEV_END_FADE_OUT:
    {
      FadeOut();

      SetSubSeq( HMF_DEV_END_FADE_IN );
    } break;

  case HMF_DEV_END_FADE_IN:
    {
      if( !IsFadeEnd() ) break;

      // 各リゾートの上画面を表示
      SetVisible_UpperDraw( true );
      // 管理人メニューを非表示
      SetVisible_HomeMenu( false );
      // 下画面UIの表示を変更
      SetLowerUIType( LowerUIDraw::DISP_TYPE_MASK );

      FadeIn();

      SetSubSeq( HMF_DEV_END_FADE_END );
    } break;

  case HMF_DEV_END_FADE_END:
    {
      if( !IsFadeEnd() ) break;

      SetSubSeq( HMF_DEV_END );
    } break;

  //---[ 開発フローの終了 ]---//
  case HMF_DEV_END:
    {
      SetMainSeq( HMF_END );
    } break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   管理人イベント：交換
 */
//--------------------------------------------------------------------------
void ResortFlow::HomeMenuEvent_Trade( void )
{
  enum {
    HMF_TRADE_BEGIN,
    HMF_TRADE_TUTORIAL_BEGIN,
    HMF_TRADE_TUTORIAL_END,
    HMF_TRADE_SELECT_BEGIN,
    HMF_TRADE_SELECT_OPEN,
    HMF_TRADE_SELECT_CLOSE,
    HMF_TRADE_SELECT_MSG,
    HMF_TRADE_SELECT_SYS_MSG,
    HMF_TRADE_END,
  };

  switch( m_SubSeq )
  {
  case HMF_TRADE_BEGIN:
    {
      if( !IsResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_CHANGE ) )
      {
        SetSubSeq( HMF_TRADE_TUTORIAL_BEGIN );
      }
      else {
        SetSubSeq( HMF_TRADE_SELECT_BEGIN );
      }
    } break;

  case HMF_TRADE_TUTORIAL_BEGIN:
    {
      // チュートリアルフラグを更新：交換初回
      SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_CHANGE );

      static const TalkParam c_TutorialTradeMsg[] = {
        { msg_prwin_info_05_01, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
        { msg_prwin_info_05_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
        { msg_prwin_info_06_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER },
      };
      TalkStart_Loop( c_TutorialTradeMsg, GFL_NELEMS(c_TutorialTradeMsg) );

      SetSubSeq( HMF_TRADE_TUTORIAL_END );
    } break;

  case HMF_TRADE_TUTORIAL_END:
    {
      if( !IsTalkEnd_Loop() ) break;

      SetSubSeq( HMF_TRADE_SELECT_BEGIN );
    } break;

  case HMF_TRADE_SELECT_BEGIN:
    {
      // 交換に必要なポケマメがある
      if( IsHaveTradeBeans() )
      {
        TalkStart( msg_prwin_sys_00_12, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
        SetSubSeq( HMF_TRADE_SELECT_OPEN );
      }
      //else if( false )
      //{
      //  TalkStart( msg_prwin_sys_00_14 );
      //  SetSubSeq( HMF_TRADE_END );
      //}
      else {
        TalkStart( msg_prwin_sys_00_14 );
        SetSubSeq( HMF_TRADE_END );
      }
    } break;

  case HMF_TRADE_SELECT_OPEN:
    {
      if( !IsTalkEnd() ) break;

      // ポケマメ選択を表示
      SetDispType_BeansSelect( BEANS_SEL_TYPE_TRADE );
      OpenLowerMenu( LOW_MENU_BEANS_SELECT );

      SetSubSeq( HMF_TRADE_SELECT_CLOSE );
    } break;

  case HMF_TRADE_SELECT_CLOSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_BEANS_SELECT );

      if( result != RESULT_CONTINUE )
      {
        // ポケマメ選択を閉じる
        CloseLowerMenu( LOW_MENU_BEANS_SELECT );

        if( result == RESULT_CANCEL )
        {
          SetMainSeq( HMF_INTRO );
        }
        else {
          SetSubSeq( HMF_TRADE_SELECT_MSG );
        }
      }
    } break;

  case HMF_TRADE_SELECT_MSG:
    {
      TalkStart( msg_prwin_info_05_02 );
      SetSubSeq( HMF_TRADE_SELECT_SYS_MSG );
    } break;

  case HMF_TRADE_SELECT_SYS_MSG:
    {
      if( !IsTalkEnd() ) break;

      LowerBeansSelectDraw* beansSelect = m_DrawList.lowerBeansSel;
      {
        if( beansSelect )
        {
          m_pWordSet->RegisterNumber(
            0, beansSelect->GetTradePokeMameCount(),
            3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
        }
      }

      SystemMsgStart( msg_prwin_sys_00_16 );
      SetSubSeq( HMF_TRADE_END );
    } break;

  case HMF_TRADE_END:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( HMF_INTRO );
    } break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   管理人イベント：マメビン流し
 */
//--------------------------------------------------------------------------
void ResortFlow::HomeMenuEvent_BeansSend( void )
{
  enum {
    HMF_SEND_BEGIN,
    HMF_SEND_TUTORIAL,
    HMF_SEND_TUTORIAL_1,
    HMF_SEND_TUTORIAL_2,
    HMF_SEND_AFTER_BEGIN,
    HMF_SEND_AFTER,
    HMF_SEND_AFTER_END,
    HMF_SEND_NOT_NEED_BEGIN,
    HMF_SEND_NOT_NEED,
    HMF_SEND_NOT_NEED_END,
    HMF_SEND_CONF_BEGIN,
    HMF_SEND_CONF,
    HMF_SEND_CONF_END,
    HMF_SEND_PAYMENT,
    HMF_SEND_PAYMENT_FADE_OUT,
    HMF_SEND_PAYMENT_FADE_WAIT,
    HMF_SEND_PAYMENT_FADE_IN,
    HMF_SEND_PAYMENT_RESULT,
    HMF_SEND_END,
  };

  switch( m_SubSeq )
  {
  case HMF_SEND_BEGIN:
    {
      if( !IsResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_BOTTLE ) )
      {
        SetSubSeq( HMF_SEND_TUTORIAL );
      }
      else {
        SetSubSeq( HMF_SEND_AFTER_BEGIN );
      }
    } break;

  case HMF_SEND_TUTORIAL:
    {
      // チュートリアルフラグを更新：マメビン流し初回
      SetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_BOTTLE );

      TalkStart( msg_prwin_info_04_01 );
      SetSubSeq( HMF_SEND_TUTORIAL_1 );
    } break;

  case HMF_SEND_TUTORIAL_1:
    {
      if( !IsTalkEnd() ) break;

      TalkStart( msg_prwin_info_04_02 );
      SetSubSeq( HMF_SEND_TUTORIAL_2 );
    } break;

  case HMF_SEND_TUTORIAL_2:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( HMF_SEND_AFTER_BEGIN );
    } break;

  case HMF_SEND_AFTER_BEGIN:
    {
      // マメビンを送ったかチェック
      bool sent = GFL_BOOL_CAST(m_pResortSave->GetUseBeansBottle());

      if( sent )
      {
        // 送付済み
        SetSubSeq( HMF_SEND_AFTER );
      }
      else {
        // 所持マメチェック
        SetSubSeq( HMF_SEND_NOT_NEED_BEGIN );
      }
    } break;

  case HMF_SEND_AFTER:
    {
      TalkStart( msg_prwin_sys_00_02 );
      SetSubSeq( HMF_SEND_AFTER_END );
    } break;

  case HMF_SEND_AFTER_END:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( HMF_INTRO );
    } break;

  case HMF_SEND_NOT_NEED_BEGIN:
    {
      u32 beansCount[3] = { 0, 0, 0 };
      {
        util::GetRankBeansCount( m_pResortSave, beansCount );
      }

      if( beansCount[0] < 7 )
      {
        // ポケマメが足らない
        SetSubSeq( HMF_SEND_NOT_NEED );
      }
      else {
        SetSubSeq( HMF_SEND_CONF_BEGIN );
      }
    } break;

  case HMF_SEND_NOT_NEED:
    {
      TalkStart( msg_prwin_sys_00_15 );
      SetSubSeq( HMF_SEND_NOT_NEED_END );
    } break;

  case HMF_SEND_NOT_NEED_END:
    {
      if( !IsTalkEnd() ) break;

      SetMainSeq( HMF_INTRO );
    } break;

  case HMF_SEND_CONF_BEGIN:
    {
      TalkStart( msg_prwin_sys_00_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
      SetSubSeq( HMF_SEND_CONF );
    } break;

  case HMF_SEND_CONF:
    {
      if( !IsTalkEnd() ) break;

      // YESNO表示
      SetDispType_Select( LowerSelectDraw::SEL_TYPE_YESNO );
      OpenLowerMenu( LOW_MENU_SELECT );

      SetSubSeq( HMF_SEND_CONF_END );
    } break;

  case HMF_SEND_CONF_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_SELECT );

      if( result != RESULT_CONTINUE )
      {
        // ボタン選択画面を閉じる
        CloseLowerMenu( LOW_MENU_SELECT );

        if( result == RESULT_NO )
        {
          SetMainSeq( HMF_INTRO );
        }
        else {
          SetSubSeq( HMF_SEND_PAYMENT );
        }
      }
    } break;

  case HMF_SEND_PAYMENT:
    {
      TalkStart( msg_prwin_sys_00_05 );
      SetSubSeq( HMF_SEND_PAYMENT_FADE_OUT );
    } break;

  case HMF_SEND_PAYMENT_FADE_OUT:
    {
      if( !IsTalkEnd() ) break; //@fix GFNMCat[2557]

      FadeOut();

      SetSubSeq( HMF_SEND_PAYMENT_FADE_WAIT );
    } break;

  case HMF_SEND_PAYMENT_FADE_WAIT:
    {
      if( !IsFadeEnd() ) break;

      Sound::PlaySE( SEQ_SE_SYS_NJ_054 );

      SetSubSeq( HMF_SEND_PAYMENT_FADE_IN );
    } break;

  case HMF_SEND_PAYMENT_FADE_IN:
    {
      if( !Sound::IsSEFinished( SEQ_SE_SYS_NJ_054 ) ) break;

      FadeIn();

      SetSubSeq( HMF_SEND_PAYMENT_RESULT );
    } break;

  case HMF_SEND_PAYMENT_RESULT:
    {
      if( !IsFadeEnd() ) break;

      TalkStart( msg_prwin_sys_00_06 );

      SetSubSeq( HMF_SEND_END );
    } break;

  case HMF_SEND_END:
    {
      if( !IsTalkEnd() ) break;

      // びんをながす
      m_pInitLogic->SendBeansBottle();

      SetMainSeq( HMF_INTRO );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   すべてのリゾートの開発が完了しているかどうか
 *
 * @return  "true  == 完了"
 * @return  "false == 開発可能"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsAllResortDeveloped( void )
{
  for( u8 i=0; i<RESORT_ID_MAX; ++i )
  {
    u8 level = Resort::util::GetResortLevel( m_pResortSave, i );

    if( level < RESORT_LV_3 ) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   交換に必要なポケマメを所持しているか？
 *
 * @return  "true  = 所持"
 * @return  "false = 未所持"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsHaveTradeBeans( void )
{
  u32 beansNum[3] = {0, 0, 0};
  util::GetRankBeansCount( m_pResortSave, beansNum );

  return (beansNum[1] + beansNum[2]) > 0;
}



//------------------------------------------------------------------------------
/**
 * @brief   きのみの生育を行っていないかどうか
 *
 * @return  "true  == 生育を行っていない"
 * @return  "false == 生育を行っている"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsFarmPlantNone( void )
{
  u32 fieldMax = m_pResortSave->GetResortLevel(RESORT_ID_FARM) * 6;

  for( u32 i=0; i<fieldMax; ++i )
  {
    // 植えていない
    if( m_pResortSave->GetFarmItemKind(i) == ITEM_DUMMY_DATA ) continue;
    // 生育済み
    if( m_pResortSave->GetFarmNum(i) > 0 ) continue;

    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   うんどうをおこなっているかどうか
 *
 * @return  "true  == うんどうしていない"
 * @return  "false == うんどうしている"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsGimTrainingNone( void )
{
  for( u32 i=0; i<3; ++i )
  {
    if( m_pResortSave->GetGimState( static_cast<int>(i) ) == Savedata::ResortSave::RESORT_GIM_STATE_TRAINING )
    {
      return false;
    }
  }

  return true;
}



//------------------------------------------------------------------------------
/**
 * @brief   きのみリゾート：はたけのようす
 */
//------------------------------------------------------------------------------
void ResortFlow::FarmEvent_FieldState( void )
{
  enum {
    FFF_FIELD_BEGIN,
    FFF_FIELD_OPEN,
    FFF_FIELD_CLOSE,
    FFF_FIELD_END,
  };

  switch( m_SubSeq )
  {
  case FFF_FIELD_BEGIN:
    {
      SetSubSeq( FFF_FIELD_OPEN );
    } break;

  case FFF_FIELD_OPEN:
    {
      // はたけのようすを開く
      OpenLowerMenu( LOW_MENU_FARM );

      SetSubSeq( FFF_FIELD_CLOSE );
    } break;

  case FFF_FIELD_CLOSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_FARM );

      if( result != RESULT_CONTINUE )
      {
        // はたけのようすを閉じる
        CloseLowerMenu( LOW_MENU_FARM );

        if( result == RESULT_CANCEL )
        {
          SetSubSeq( FFF_FIELD_END );
        }
        else {
          SetMainSeq( FFF_FIELD_END );
        }
      }
    } break;

  case FFF_FIELD_END:
    {
      SetMainSeq( FFF_END );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   きのみリゾート：きのみリスト
 */
//------------------------------------------------------------------------------
void ResortFlow::FarmEvent_KinomiList( void )
{
  enum {
    FLF_LIST_BEGIN,
    FLF_LIST_OPEN,
    FLF_LIST_CLOSE,
    FLF_LIST_MSG,
    FLF_LIST_END,
  };

  switch( m_SubSeq )
  {
  case FLF_LIST_BEGIN:
    {
      // ポケモンがいない
      if( !IsValidPokemon( RESORT_ID_FARM ) )
      {
        TalkStart( msg_prwin_sys_02_04 );

        SetSubSeq( FLF_LIST_MSG );
      }
      // はたけに空きがある
      else if( !IsFarmNotSpace() )
      {
        TalkStart( msg_prwin_sys_02_01, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

        SetSubSeq( FLF_LIST_OPEN );
      }
      // はたけが全て埋まっている
      else {
        TalkStart( msg_prwin_info_06_04 );

        SetSubSeq( FLF_LIST_MSG );
      }
    } break;

  case FLF_LIST_OPEN:
    {
      if( !IsTalkEnd() ) break;

      // きのみリストが開けるかどうか
      if( !IsReadyFarmList() )
      {
        // きのみリストの準備が完了するまで待つ
        break;
      }

      // きのみリストを開く
      OpenLowerMenu( LOW_MENU_FARM_LIST );

      SetSubSeq( FLF_LIST_CLOSE );
    } break;

  case FLF_LIST_CLOSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_FARM_LIST );

      if( result != RESULT_CONTINUE )
      {
        // きのみリストを閉じる
        CloseLowerMenu( LOW_MENU_FARM_LIST );

        if( result == RESULT_CANCEL )
        {
          SetSubSeq( FLF_LIST_END );
        }
        else {
          TalkStart( msg_prwin_sys_02_02 );
          SetSubSeq( FLF_LIST_MSG );
        }
      }
    } break;

  case FLF_LIST_MSG:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( FLF_LIST_END );
    } break;

  case FLF_LIST_END:
    {
      SetMainSeq( FFF_END );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   はたけが全て埋まっているか？
 *
 * @return  "true  == 埋まっている"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsFarmNotSpace( void )
{
  // はたけのインデックス
  u8 fieldIndex = m_DrawList.lowerFarmList->GetFieldIndex();

  u8 start = fieldIndex * 6;
  u8 end   = start + 6;

  for( u8 i=start; i<end; ++i )
  {
    u16 itemNo = m_pResortSave->GetFarmItemKind( static_cast<int>(i) );

    if( itemNo == ITEM_DUMMY_DATA )
    {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   きのみリストの準備が完了しているか？
 *
 * @return  "true  == 完了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsReadyFarmList( void )
{
  LowerFarmListDraw* farmList = m_DrawList.lowerFarmList;
  {
    return farmList->IsCanOpen();
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   たんけんコース選択：開始分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::AdventureSelectEvent_Begin( void )
{
  SetSubSeq( 0 );

  // たんけんコース未選択
  if( m_pResortSave->GetAdventureCoursePattern() == Savedata::ResortSave::RESORT_ADV_COURSE_NONE )
  {
    // お宝を回収済みかどうか
    if( m_pResortSave->GetAdventureItem(0) != ITEM_DUMMY_DATA )
    {
      TalkStart( msg_prwin_sys_03_10 );
      SetMainSeq( ACSF_ERROR );
    }
    // ポケモンがいる
    else if( IsValidPokemon( RESORT_ID_ADVENTURE ) )
    {
      TalkStart( msg_prwin_sys_03_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
      SetMainSeq( ACSF_SELECT_INTRO );
    }
    // ポケモンがいない
    else {
      TalkStart( msg_prwin_sys_03_05 );
      SetMainSeq( ACSF_ERROR );
    }
  }
  // 冒険中
  else {
    SetMainSeq( ACSF_ADVENTURE );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   たんけんコース選択：コース選択
 */
//--------------------------------------------------------------------------
void ResortFlow::AdventureSelectEvent_Select( void )
{
  enum {
    ACSF_SELECT_BEGIN,
    ACSF_SELECT_COURSE,
    ACSF_SELECT_DECIDE,
    ACSF_SELECT_END,
  };

  switch( m_SubSeq )
  {
  // コース選択を開く
  case ACSF_SELECT_BEGIN:
    {
      SetDispType_Course( COURSE_SELECT_ADV );
      OpenLowerMenu( LOW_MENU_COURSE );

      SetSubSeq( ACSF_SELECT_COURSE );
    } break;

  // コース選択
  case ACSF_SELECT_COURSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_COURSE );

      if( result != RESULT_CONTINUE )
      {
        // コース選択を閉じる
        CloseLowerMenu( LOW_MENU_COURSE );

        // キャンセル
        if( result == RESULT_CANCEL )
        {
          SetMainSeq( ACSF_END );
        }
        // 決定
        else {
          TalkStart( msg_prwin_sys_03_04 );
          SetSubSeq( ACSF_SELECT_DECIDE );
        }
      }
    } break;

  // 決定後
  case ACSF_SELECT_DECIDE:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( ACSF_SELECT_END );
    } break;

  case ACSF_SELECT_END:
    {
      // リゾートトップ画面の復帰
      UnsetupMenuFlow();

      // モデルアニメの再生
      StartModelAnim( ResortChangeIslandListener::ISLAND_MODELANIM_ADVENTURE_IN );
    } break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   たんけんコース選択：冒険中
 */
//--------------------------------------------------------------------------
void ResortFlow::AdventureSelectEvent_Adventure( void )
{
  enum {
    ACSF_ADV_BEGIN,
    ACSF_ADV_CONF_BEGIN,
    ACSF_ADV_CONF_END,
    ACSF_ADV_BREAK_BEGIN,
    ACSF_ADV_BREAK_END,
  };

  switch( m_SubSeq )
  {
  case ACSF_BEGIN:
    {
      TalkStart( msg_prwin_sys_03_06, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

      SetSubSeq( ACSF_ADV_CONF_BEGIN );
    } break;

  case ACSF_ADV_CONF_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      // YESNOを開く
      SetDispType_Select( LowerSelectDraw::SEL_TYPE_YESNO );
      OpenLowerMenu( LOW_MENU_SELECT );

      SetSubSeq( ACSF_ADV_CONF_END );
    } break;

  case ACSF_ADV_CONF_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_SELECT );

      if( result != RESULT_CONTINUE )
      {
        // YESNOを閉じる
        CloseLowerMenu( LOW_MENU_SELECT );

        if( result == RESULT_YES )
        {
          SetSubSeq( ACSF_ADV_BREAK_BEGIN );
        }
        else {
          SetMainSeq( ACSF_END );
        }
      }
    } break;

  case ACSF_ADV_BREAK_BEGIN:
    {
      /*
      m_pResortSave->SetAdventureCoursePattern( Savedata::ResortSave::RESORT_ADV_COURSE_NONE );
      m_pResortSave->SetAdventureTime( 0, 0, 0 );
      m_pResortSave->SetUpdateFlg();
*/
//      StartModelAnim(ResortChangeIslandListener::ISLAND_MODELANIM_ADVENTURE_OUT);

      
      TalkStart( msg_prwin_sys_03_07 );
      SetSubSeq( ACSF_ADV_BREAK_END );
    } break;

  case ACSF_ADV_BREAK_END:
    {
      if( !IsTalkEnd() ) break;

      // リゾートトップ画面の復帰
      UnsetupMenuFlow();

      // モデルアニメの再生
      StartModelAnim( ResortChangeIslandListener::ISLAND_MODELANIM_ADVENTURE_OUT );
    } break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   たんけんコース選択：エラー表示
 */
//--------------------------------------------------------------------------
void ResortFlow::AdventureSelectEvent_Error( void )
{
  if( IsTalkEnd() )
  {
    SetMainSeq( ACSF_END );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   たんけんリゾートの結果アイテム数の取得
 *
 * @return  アイテム数
 */
//------------------------------------------------------------------------------
u32 ResortFlow::GetAdvItemCount( void )
{
  u32 count = 0;

  for( u32 i=0; i<Savedata::ResortSave::ADVENTURE_ITEM_MAX; ++i )
  {
    u16 itemNo = m_pResortSave->GetAdventureItem( i );

    if( itemNo == ITEM_DUMMY_DATA ) continue;

    count++;
  }

  return count;
}



//------------------------------------------------------------------------------
/**
 * @brief   うんどう状態：開始分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::GimStatetEvent_Begin( void )
{
  SetSubSeq(0);

  switch( m_pResortSave->GetGimState( GetIndex_GimGroup() ) )
  {
  case Savedata::ResortSave::RESORT_GIM_STATE_INIT:
    {
      GFL_PRINT( "うんどう開始イベント\n" );
      SetMainSeq( GSF_START );
    } break;

  case Savedata::ResortSave::RESORT_GIM_STATE_TRAINING:
    {
      GFL_PRINT( "うんどう中イベント\n" );
      SetMainSeq( GSF_TRAINING );
    } break;

  case Savedata::ResortSave::RESORT_GIM_STATE_END:
    {
      GFL_PRINT( "うんどう終了イベント\n" );
      SetMainSeq( GSF_FINISH );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   うんどう状態：うんどう開始
 */
//------------------------------------------------------------------------------
void ResortFlow::GimStatetEvent_Start( void )
{
  enum {
    GSF_START_BEGIN,
    GSF_START_END,
  };

  switch( m_SubSeq )
  {
  case GSF_START_BEGIN:
    {
      TalkStart( msg_prwin_sys_04_01 );

      SetSubSeq( GSF_START_END );
    } break;

  case GSF_START_END:
    {
      if( !IsTalkEnd() ) break;

      CallApplication( CALL_APP_BOX_GIM );    //!< ボックスのコール

      SetMainSeq( GSF_END );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   うんどう状態：うんどう中
 */
//------------------------------------------------------------------------------
void ResortFlow::GimStatetEvent_Training( void )
{
  enum {
    GSF_TRAINING_BEGIN,
    GSF_TRAINING_OPEN,
    GSF_TRAINING_CLOSE,
    GSF_TRAINING_CONF_OPEN,
    GSF_TRAINING_CONF_CLOSE,
    GSF_TRAINING_BREAK,
    GSF_TRAINING_END,
  };

  switch( m_SubSeq )
  {
  case GSF_TRAINING_BEGIN:
    {
      TalkStart( msg_prwin_sys_04_05, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

      SetSubSeq( GSF_TRAINING_OPEN );
    } break;

  case GSF_TRAINING_OPEN:
    {
      if( !IsTalkEnd() ) break;

      // うんどう状態の表示
      SetDispType_Gim( GIM_DECIDE_BTN_ENABLE );
      OpenLowerMenu( LOW_MENU_GIM );

      SetSubSeq( GSF_TRAINING_CLOSE );
    } break;

  case GSF_TRAINING_CLOSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_GIM );

      if( result != RESULT_CONTINUE )
      {
        // うんどう状態を閉じる
        CloseLowerMenu( LOW_MENU_GIM );

        if( result == RESULT_CANCEL )
        {
          SetSubSeq( GSF_TRAINING_END );
        }
        else {
          TalkStart( msg_prwin_sys_04_06, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );
          SetSubSeq( GSF_TRAINING_CONF_OPEN );
        }
      }
    } break;

  case GSF_TRAINING_CONF_OPEN:
    {
      if( !IsTalkEnd() ) break;

      // ボタン選択画面の表示
      SetDispType_Select( LowerSelectDraw::SEL_TYPE_YESNO );
      OpenLowerMenu( LOW_MENU_SELECT );

      SetSubSeq( GSF_TRAINING_CONF_CLOSE );
    } break;

  case GSF_TRAINING_CONF_CLOSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_SELECT );

      if( result != RESULT_CONTINUE )
      {
        // ボタン選択を非表示
        CloseLowerMenu( LOW_MENU_SELECT );

        if( result == RESULT_YES )
        {
          TalkStart( msg_prwin_sys_04_07 );

          SetSubSeq( GSF_TRAINING_BREAK );
        }
        else {
          SetSubSeq( GSF_TRAINING_END );
        }
      }
    } break;

  case GSF_TRAINING_BREAK:
    {
      if( !IsTalkEnd() ) break;

      // うんどうの状態を「終了」へ
      m_pResortSave->SetGimState(
        GetIndex_GimGroup(),
        Savedata::ResortSave::RESORT_GIM_STATE_END );
      m_pResortSave->SetGimTrainingCount( GetIndex_GimGroup(), 0 );
      m_pResortSave->SetGimTrainingCountMax( GetIndex_GimGroup(), 0 );
      m_pResortSave->SetGimEndTime( GetIndex_GimGroup(), 0, 0, 0 );

      SetSubSeq( GSF_TRAINING_END );
    } break;

  case GSF_TRAINING_END:
    {
      SetMainSeq( GSF_END );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   うんどう状態：うんどう終了
 */
//------------------------------------------------------------------------------
void ResortFlow::GimStatetEvent_Finish( void )
{
  enum {
    GSF_FINISH_BEGIN,
    GSF_FINISH_OPEN,
    GSF_FINISH_CLOSE,
    GSF_FINISH_CONTINUE,
    GSF_FINISH_END,
  };

  switch( m_SubSeq )
  {
  case GSF_FINISH_BEGIN:
    {
      // ボックスの空き数を取得
      u32 spaceCount = GetBoxSpaceCount();
      // うんどうしたポケモン数を取得
      u32 pokeCount = GetGimPokeCount();

      if( spaceCount >= pokeCount )
      {
        TalkStart( msg_prwin_sys_04_08, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

        // ひきとるありで表示
        SetDispType_Gim( GIM_DECIDE_BTN_ENABLE );
      }
      else {
        TalkStart( msg_prwin_sys_04_11, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

        // ひきとるなしで表示
        SetDispType_Gim( GIM_DECIDE_BTN_DISABLE );
      }

      SetSubSeq( GSF_FINISH_OPEN );
    } break;

  case GSF_FINISH_OPEN:
    {
      if( !IsTalkEnd() ) break;

      OpenLowerMenu( LOW_MENU_GIM );    //!< うんどう状態のオープン

      SetSubSeq( GSF_FINISH_CLOSE );
    } break;

  case GSF_FINISH_CLOSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_GIM );

      if( result != RESULT_CONTINUE )
      {
        // うんどう状態を非表示
        CloseLowerMenu( LOW_MENU_GIM );

        if( result == RESULT_GIM_CONTINUE )
        {
          TalkStart( msg_prwin_sys_04_14, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_USER );
          SetSubSeq( GSF_FINISH_CONTINUE );
        }
        else if( result == RESULT_GIM_TAKEBACK )
        {
          CallApplication( CALL_APP_BOX_GIM_RE ); //!< ひきとるモードのボックスをコール
          SetSubSeq( GSF_FINISH_END );
        }
        else {
          SetSubSeq( GSF_FINISH_END );
        }
      }
    } break;

  case GSF_FINISH_CONTINUE:
  {
    if( !IsTalkEnd() ) break;

    // 「つづける」場合はステートを変更しておく
    m_pResortSave->SetGimState( GetGimGroupIndex(), Savedata::ResortSave::RESORT_GIM_STATE_INIT );
    // ドリンク選択画面を表示するためグループを設定しておく
    m_DrawList.lowerCourseSel->SetGimGroupIndex( m_GimGroupIndex );

    StartMenuFlow( ResortFlow::FLOW_TYPE_UNDOU_SELECT );
  } break;

  case GSF_FINISH_END:
    {
      SetMainSeq( GSF_END );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   うんどうリゾートの選択ポケモン数の取得
 *
 * @return  count   ポケモン数
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetGimPokeCount( void )
{
  u8 count = 0;
  u8 group = GetIndex_GimGroup();

  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();
  {
    for( u8 i=0; i<6; ++i )
    {
      // インデックス
      u8 index = i + (group * 6);

      // ポケモンのコアパラメータの生成
      pml::pokepara::CoreParam* cp =
        GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );
      {
        GFL_ASSERT(cp);

        // リゾートのセーブデータからパラメータを取得
        m_pResortSave->GetGimPokemon( index, cp );

        if( !cp->IsNull() )
        {
          count++;    //!< ポケモンがいるならカウント
        }
      }

      GFL_DELETE cp;
    }
  }

  return count;
}



//------------------------------------------------------------------------------
/**
 * @brief   おんせんタマゴイベント：開始分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::HotspaEggEvent_Begin( void )
{
  SetSubSeq( 0 );

  if( GetEggFamiliarity() == 0 && m_MenuParam.pokeCoreParam->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ) )
  {
    SetupMenuFlow( m_StartFlowType );

    SetMainSeq( HESF_CALL_DEMO );
  }
  else {
    SetMainSeq( HESF_STATE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   おんせんタマゴイベント：デモの開始
 */
//------------------------------------------------------------------------------
void ResortFlow::HotspaEggEvent_CallDemo( void )
{
  enum {
    HESF_DEMO_BEGIN,
    HESF_DEMO_END,
  };

  switch( m_SubSeq )
  {
  case HESF_DEMO_BEGIN:
    {
#if 0 // @fix NMCat[4264]
      // ボックスの空きをチェック
      if( GetBoxSpaceCount() == 0 )
      {
        TalkStart( msg_prwin_sys_05_03, MSG_TYPE_NONE, gfl2::str::StrWin::FINISH_NONE );

        SetMainSeq( HESF_NOT_SPACE );
      }
      else {
        TalkStart( msg_prwin_info_06_08 );

        SetSubSeq( HESF_DEMO_END );
      }
#else
      TalkStart( msg_prwin_info_06_08 );

      SetSubSeq( HESF_DEMO_END );
#endif
    } break;

  case HESF_DEMO_END:
    {
      if( !IsTalkEnd() ) break;

      CallApplication( CALL_APP_EGG_DEMO );   //!< 孵化デモの開始

      SetMainSeq( HESF_EVENT_END_BEGIN );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   おんせんタマゴイベント：タマゴ状態
 */
//------------------------------------------------------------------------------
void ResortFlow::HotspaEggEvent_State( void )
{
  enum {
    HESF_STATE_BEGIN,
    HESF_STATE_OPEN,
    HESF_STATE_CLOSE,
    HESF_STATE_END,
  };

  switch( m_SubSeq )
  {
  case HESF_STATE_BEGIN:
    {
      SetupMenuFlow( m_StartFlowType );

      SetMessage_EggState();

      SetSubSeq( HESF_STATE_OPEN );
    } break;

  case HESF_STATE_OPEN:
    {
      if( !IsTalkEnd() ) break;

      SetLowerUIType( LowerUIDraw::DISP_TYPE_WILD_POKEMON_EVENT );
      
      SetSubSeq( HESF_STATE_CLOSE );
    } break;

  case HESF_STATE_CLOSE:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_UI );

      if( result != RESULT_CONTINUE )
      {
        SetSubSeq( HESF_STATE_END );
      }
    } break;

  case HESF_STATE_END:
    {
      SetMainSeq( HESF_EVENT_END_BEGIN );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   たまごのなつき度の取得(孵化までの状態を取得)
 *
 * @return  なつき度
 */
//------------------------------------------------------------------------------
u32 ResortFlow::GetEggFamiliarity( void )
{
  u32 natuki = 0;

  pml::pokepara::CoreParam* cp = m_MenuParam.pokeCoreParam;
  {
    bool fastmode_flag = cp->StartFastMode();
    {
      natuki = cp->GetOriginalFamiliarity();
    }
    cp->EndFastMode( fastmode_flag );

    FUKUSHIMA_PRINT( "natuki=[%d]\n", natuki );
  }

  return natuki;
}



//------------------------------------------------------------------------------
/**
 * @brief   Boxポケモンイベント：開始分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::BoxPokemonEvent_Begin( void )
{
  switch( GetPokemonEventWork() )
  {
  case pml::pokepara::RESORTEVENT_SEIKAKU:
    {
      SetMainSeq( BPEF_EVENT_SEIKAKU_BEGIN );
    } break;

  case pml::pokepara::RESORTEVENT_CARE:
  case pml::pokepara::RESORTEVENT_LIKE_RESORT:
  case pml::pokepara::RESORTEVENT_LIKE_BATTLE:
  case pml::pokepara::RESORTEVENT_LIKE_ADV:
  case pml::pokepara::RESORTEVENT_GIM:
  case pml::pokepara::RESORTEVENT_HOTSPA:
    {
      SetMainSeq( BPEF_EVENT_NORMAL_BEGIN );
    } break;

  case pml::pokepara::RESORTEVENT_GOOD_FRIEND:
    {
      SetMainSeq( BPEF_EVENT_FRIENDSHIP_BEGIN );
    } break;

  case pml::pokepara::RESORTEVENT_POKEMAME_GET1:
  case pml::pokepara::RESORTEVENT_POKEMAME_GET2:
  case pml::pokepara::RESORTEVENT_POKEMAME_GET3:
    {
      SetMainSeq( BPEF_EVENT_POKEMAME_BEGIN );
    } break;

  case pml::pokepara::RESORTEVENT_PLAY_STATE:
    SetMainSeq( BPEF_EVENT_GIM_BEGIN );
    break;

  case pml::pokepara::RESORTEVENT_KINOMI_HELP:
    {
      SetMainSeq( BPEF_EVENT_FARM_BEGIN );
    } break;

  case pml::pokepara::RESORTEVENT_HOTSPA_STATE:
    SetMainSeq( BPEF_EVENT_HOTSPA_BEGIN );
    break;

  case pml::pokepara::RESORTEVENT_HOTSPA_DIZZY:
    SetMainSeq( BPEF_EVENT_HOTSPA_DIZZY_BEGIN );
    break;

  default:
    {
      SetMainSeq( BPEF_SELECT_BEGIN );
    }
  }

#if PM_DEBUG
  GFL_PRINT( "ポケモンイベント開始：" );
  GFL_PRINT( "EventWork=[%d]\n", GetPokemonEventWork() );
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   野生ポケモンイベント：開始分岐
 */
//------------------------------------------------------------------------------
void ResortFlow::WildPokemonEvent_Begin( void )
{
  // サブシーケンスの初期化
  SetSubSeq(0);

  switch( GetPokemonEventWork() )
  {
  case pml::pokepara::RESORTEVENT_WILD:
    {
      SetMainSeq( WPEF_APPEARANCE_BEGIN );
    } break;

  case pml::pokepara::RESORTEVENT_WILD_LOVE:
    {
      SetMainSeq( WPEF_CARE_BEGIN );
    } break;

  case pml::pokepara::RESORTEVENT_WILD_LIVE:
    {
      SetMainSeq( WPEF_TAKE_CHARGE );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   野生ポケモンイベント：引き取りイベント
 */
//------------------------------------------------------------------------------
void ResortFlow::WildPokemonEvent_TakeCharge( void )
{
  enum {
    WPEF_TAKE_CHARGE_BEGIN,
    WPEF_TAKE_CHARGE_CONF_BEGIN,
    WPEF_TAKE_CHARGE_CONF_END,
    WPEF_TAKE_CHARGE_NOT_BEGIN,
    WPEF_TAKE_CHARGE_NOT_END,
    WPEF_TAKE_CHARGE_BOX_CHECK,
    WPEF_TAKE_CHARGE_SUCCESS,
    WPEF_TAKE_CHARGE_FAILED,
    WPEF_TAKE_CHARGE_SYSMSG,
    WPEF_TAKE_CHARGE_END,
  };

  switch( m_SubSeq )
  {
  case WPEF_TAKE_CHARGE_BEGIN:
    {
      {
        m_pWordSet->RegisterPokeMonsName( 0, m_MenuParam.pokeCoreParam );
        m_pWordSet->RegisterPlayerName( 1, m_pMyStatus );
        TalkStart( msg_prwin_poke_ev_11_01 );
      }

      SetSubSeq( WPEF_TAKE_CHARGE_CONF_BEGIN );
    } break;

  case WPEF_TAKE_CHARGE_CONF_BEGIN:
    {
      if( !IsTalkEnd() ) break;

      // 開く
      SetDispType_Select( LowerSelectDraw::SEL_TYPE_YESNO );
      OpenLowerMenu( LOW_MENU_SELECT );

      SetSubSeq( WPEF_TAKE_CHARGE_CONF_END );
    } break;

  case WPEF_TAKE_CHARGE_CONF_END:
    {
      u8 result = GetLowerMenuResult( LOW_MENU_SELECT );

      if( result != RESULT_CONTINUE )
      {
        // 閉じる
        CloseLowerMenu( LOW_MENU_SELECT );

        if( result == RESULT_YES )
        {
          SetSubSeq( WPEF_TAKE_CHARGE_BOX_CHECK );
        }
        else {
          SetSubSeq( WPEF_TAKE_CHARGE_NOT_BEGIN );
        }
      }
    } break;

  case WPEF_TAKE_CHARGE_NOT_BEGIN:
    {
      TalkStart( msg_prwin_poke_ev_11_02 );

      SetSubSeq( WPEF_TAKE_CHARGE_NOT_END );
    } break;

  case WPEF_TAKE_CHARGE_NOT_END:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( WPEF_TAKE_CHARGE_END );
    } break;

  case WPEF_TAKE_CHARGE_BOX_CHECK:
    {
      if( IsWildPokeTakeCharge() )
      {
        // ひきとる
        TalkStart( msg_prwin_poke_ev_11_04, MSG_TYPE_POKE_MONSNAME );
        
        SetSubSeq( WPEF_TAKE_CHARGE_SUCCESS );
      }
      else {
        // ボックスがいっぱい
        TalkStart( msg_prwin_poke_ev_11_03 );

        SetSubSeq( WPEF_TAKE_CHARGE_FAILED );
      }
    } break;

  case WPEF_TAKE_CHARGE_SUCCESS:
    {
      if( !IsTalkEnd() ) break;

      // システムメッセージの表示
      m_pWordSet->RegisterPokeMonsName( 0, m_MenuParam.pokeCoreParam );
      SystemMsgStart( msg_prwin_info_06_18 );

      SetSubSeq( WPEF_TAKE_CHARGE_SYSMSG );
    } break;

  case WPEF_TAKE_CHARGE_SYSMSG:
    {
      if( !IsTalkEnd() ) break;

      // 図鑑をコール
      CallApplication( CALL_APP_ZUKAN );

      SetSubSeq( WPEF_TAKE_CHARGE_END );
    } break;

  case WPEF_TAKE_CHARGE_FAILED:
    {
      if( !IsTalkEnd() ) break;

      SetSubSeq( WPEF_TAKE_CHARGE_END );
    } break;

  case WPEF_TAKE_CHARGE_END:
    {
      SetMainSeq( WPEF_EVENT_END_BEGIN );
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   もらったポケマメのセット
 */
//------------------------------------------------------------------------------
void ResortFlow::SetPokemame( void )
{
  switch( GetPokemonEventWork() )
  {
  case pml::pokepara::RESORTEVENT_POKEMAME_GET1:
    util::AddPokeMame( m_pResortSave, Savedata::ResortSave::POKEBEANS_RANK_NORMAL, 1 );     // @fix NMCat[1977]
    break;
  case pml::pokepara::RESORTEVENT_POKEMAME_GET2:
    util::AddPokeMame( m_pResortSave, Savedata::ResortSave::POKEBEANS_RANK_RARE, 1 );       // @fix NMCat[1977]
    break;
  case pml::pokepara::RESORTEVENT_POKEMAME_GET3:
    util::AddPokeMame( m_pResortSave, Savedata::ResortSave::POKEBEANS_RANK_S_RARE, 1 );     // @fix NMCat[1977]
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンイベントワークの取得
 *
 * @param   work    イベントワーク
 */
//------------------------------------------------------------------------------
void ResortFlow::SetPokemonEventWork( u8 work )
{
  pml::pokepara::CoreParam* pp = m_MenuParam.pokeCoreParam;
  {
    if( pp && !pp->IsNull() )
    {
      bool fastmode = pp->StartFastMode();
      {
        pp->SetResortEventStatus( work );
      }
      pp->EndFastMode( fastmode );

      FUKUSHIMA_PRINT( "SetResortEventStatus : [%d] -> [%d]\n", GetPokemonEventWork(), work );
    }
  }

  // 更新したポケモンパラメータをセーブデータに反映
  SetPokemonParam();
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンイベントワークの取得
 *
 * @return  イベントワーク
 */
//------------------------------------------------------------------------------
u8 ResortFlow::GetPokemonEventWork( void ) const
{
  const pml::pokepara::CoreParam* pp = m_MenuParam.pokeCoreParam;
  {
    if( pp && !pp->IsNull() )
    {
      u8 work = 0;

      bool fastmode = pp->StartFastMode();
      {
        work = pp->GetResortEventStatus();
      }
      pp->EndFastMode( fastmode );
      
      return work;
    }
  }

  return pml::pokepara::RESORTEVENT_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータのセット
 */
//------------------------------------------------------------------------------
void ResortFlow::SetPokemonParam( void )
{
  pml::pokepara::CoreParam* cp = m_MenuParam.pokeCoreParam;
  u32 pokeIndex = m_MenuParam.pokemonIndex;

  FUKUSHIMA_PRINT( "ポケモンパラメータの更新：" );
  FUKUSHIMA_PRINT( "resortID=[%d], index=[%d]\n", m_MenuParam.resortID, pokeIndex );

  switch( m_MenuParam.resortID )
  {
  case RESORT_ID_FRIENDSHIP:
    {
      if( m_StartFlowType == FLOW_TYPE_BOX_POKE_EVENT )
      {
        m_pResortSave->SetFriendShipPokemon( pokeIndex, cp );
      }
      else {
        m_pResortSave->SetWildPokemon( pokeIndex, cp );
      }
    } break;
  case RESORT_ID_FARM:
    {
      m_pResortSave->SetFarmPokemon( pokeIndex, cp );
    } break;
  case RESORT_ID_ADVENTURE:
    {
      m_pResortSave->SetAdventurePokemon( pokeIndex, cp );
    } break;
  case RESORT_ID_GIM:
    {
      m_pResortSave->SetGimPokemon( pokeIndex, cp );
    } break;
  case RESORT_ID_HOTSPA:
    {
      m_pResortSave->SetHotSpaPokemon( pokeIndex, cp, false );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンを引き取れるか？
 *
 * @return  "true  == 引き取れる"
 * @return  "false == 引き取れない"
 */
//------------------------------------------------------------------------------
bool ResortFlow::IsWildPokeTakeCharge( void )
{
  if( GetBoxSpaceCount() == 0 && m_PartyMemberNum == pml::PokeParty::MAX_MEMBERS )
  {
    return false;
  }

  return true;
}



//------------------------------------------------------------------------------
/**
 * @brief   チュートリアルイベント：指カーソルの表示
 *
 * @param   progID    表示する進行度
 */
//------------------------------------------------------------------------------
void ResortFlow::VisibleFingerCursor( u8 progID )
{
  s32 x = 0, y = 0;
  {
    switch( progID )
    {
    case Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS:
      {
        x = ResortTouchToFlowListener::TUTORIAL_BEANS_POS_X;
        y = ResortTouchToFlowListener::TUTORIAL_BEANS_POS_Y;
      } break;
    case Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE:
      {
        x = ResortTouchToFlowListener::TUTORIAL_TREE_POS_X;
        y = ResortTouchToFlowListener::TUTORIAL_TREE_POS_Y;
      } break;
    case Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BOX:
      {
        x = ResortTouchToFlowListener::TUTORIAL_BEANSBOX_POS_X;
        y = ResortTouchToFlowListener::TUTORIAL_BEANSBOX_POS_Y;
      } break;
    }
  }

  LowerUIDraw* ui = m_DrawList.lowerUI;
  {
    if( ui )
    {
      ui->VisibleTutorialArrow( progID, x, y ); 
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   チュートリアルイベント：指カーソルの非表示
 */
//------------------------------------------------------------------------------
void ResortFlow::InvisibleFingerCursor( void )
{
  LowerUIDraw* ui = m_DrawList.lowerUI;
  {
    if( ui )
    {
      ui->InvisibleTurorialArrow(); 
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   チュートリアルイベント：モデルアニメの再生
 *
 * @param   animID    モデルアニメID
 */
//------------------------------------------------------------------------------
void ResortFlow::StartModelAnim( u32 animID )
{
  ResortChangeIslandListener* listener = m_DrawList.islandListener;
  {
    if( listener )
    {
      listener->StartModelAnim( animID );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   チュートリアルイベント：ふれあい上画面のセットアップ
 */
//------------------------------------------------------------------------------
void ResortFlow::SetupUpperFriendship( void )
{
  UpperDraw* upper = m_DrawList.upperMenu;
  {
    if( upper )
    {
      upper->UpdateResort();
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   メニューパラメータの取得
 * @return  メニューパラメータ構造体のポインタ
 */
//------------------------------------------------------------------------------
ResortFlow::MenuParam* ResortFlow::GetMenuParam( void )
{
  return &m_MenuParam;
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの取得：セーブデータから取得
 *
 * @param   resortID  リゾートID
 * @param   index     インデックス
 * @param   cp        pml::pokepara::CoreParam
 */
//------------------------------------------------------------------------------
void ResortFlow::GetPokemon_FromSavedata( u8 resortID, int index, pml::pokepara::CoreParam* cp )
{
  void (Savedata::ResortSave::*GET_RESORT_POKEMON_FUNC[LOW_MENU_NUM])( int, pml::pokepara::CoreParam* ) = {
    &Savedata::ResortSave::GetFriendShipPokemon,
    &Savedata::ResortSave::GetFarmPokemon,
    &Savedata::ResortSave::GetAdventurePokemon,
    &Savedata::ResortSave::GetGimPokemon,
    &Savedata::ResortSave::GetHotSpaPokemon,
  };

  (m_pResortSave->*GET_RESORT_POKEMON_FUNC[resortID])( index, cp );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックス内のポケモンの数を取得
 *
 * @return  ボックス内のポケモン数
 */
//------------------------------------------------------------------------------
u32 ResortFlow::GetBoxPokemonCount( void )
{
  if( m_pBoxPokemon == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();

  // タマゴはカウントしない
  return m_pBoxPokemon->GetPokemonCountAll( heap, false );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックス内の空きの数を取得
 *
 * @return  ボックス内の空きの数
 */
//------------------------------------------------------------------------------
u32 ResortFlow::GetBoxSpaceCount( void )
{
  if( m_pBoxPokemon == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();

  return m_pBoxPokemon->GetSpaceCountAll( heap );
}

//------------------------------------------------------------------
/**
 * @brief   Drawクラスのインスタンスの設定
 *
 * @param   drawList   Drawクラスのインスタンスをまとめた構造体
 */
//------------------------------------------------------------------
void ResortFlow::SetDrawList( DrawList& drawList )
{
  m_DrawList = drawList;
}

//------------------------------------------------------------------------------
/**
 * @brief   温泉イベント開始時のメッセージIDを取得
 * @return  メッセージID
 */
//------------------------------------------------------------------------------
u32 ResortFlow::GetEventStartHotSpaMsgID(void)
{
  pml::pokepara::CoreParam * cp = m_MenuParam.pokeCoreParam;
  u32 val = cp->GetFamiliarity();
  if( val <= 99 )
  {
    return msg_prwin_poke_ev_17_01;
  }
  else if( val >= 100 && val <= 199 )
  {
    return msg_prwin_poke_ev_17_02;
  }
  else if( val >= 200 && val <= 254 )
  {
    return msg_prwin_poke_ev_17_03;
  }
  else if( val == 255 )
  {
    return msg_prwin_poke_ev_17_04;
  }
  GFL_ASSERT( 0 );
  return msg_prwin_poke_ev_17_01;
}

//------------------------------------------------------------------------------
/**
 * @brief   運動イベント開始時のメッセージIDを取得
 * @param   gim_index   運動のインデックス
 * @param   val         in:運動開始時の値 out:上昇値
 * @return  メッセージID
 */
//------------------------------------------------------------------------------
u32 ResortFlow::GetEventStartGimMsgID( int gim_index, s32 * val )
{
  pml::pokepara::CoreParam * cp = m_MenuParam.pokeCoreParam;

  Savedata::ResortSave::PokeResortGimKind kind = m_pResortSave->GetGimKind( gim_index );

  u32 msg_id = msg_prwin_poke_ev_16_01;

  switch( kind )
  {
  case Savedata::ResortSave::RESORT_GIM_KIND_LV:
    {
      int lv = cp->GetLevel();
      *val = lv - *val;
      if( *val == 0 )
      {
        if( lv == pml::MAX_POKE_LEVEL )
        {
          msg_id = msg_prwin_poke_ev_16_04;
        }
        else
        {
          msg_id = msg_prwin_poke_ev_16_01;
        }
      }
      else
      {
        if( lv == pml::MAX_POKE_LEVEL )
        {
          msg_id = msg_prwin_poke_ev_16_03;
        }
        else
        {
          msg_id = msg_prwin_poke_ev_16_02;
        }
      }
    }
    break;

  case Savedata::ResortSave::RESORT_GIM_KIND_HP:
  case Savedata::ResortSave::RESORT_GIM_KIND_AT:
  case Savedata::ResortSave::RESORT_GIM_KIND_DF:
  case Savedata::ResortSave::RESORT_GIM_KIND_SA:
  case Savedata::ResortSave::RESORT_GIM_KIND_SD:
  case Savedata::ResortSave::RESORT_GIM_KIND_SP:
    {
      pml::pokepara::PowerID eff = m_pResortSave->GetGimKindPowerID( kind );
      int power = 0;
      int total = 0;
      for( int i=Savedata::ResortSave::RESORT_GIM_KIND_HP; i<=Savedata::ResortSave::RESORT_GIM_KIND_SP; i++ )
      {
        pml::pokepara::PowerID tmp_eff = m_pResortSave->GetGimKindPowerID( static_cast<Savedata::ResortSave::PokeResortGimKind>(i) );
        int tmp = cp->GetEffortPower( tmp_eff ); 
        total += tmp;
        if( tmp_eff == eff )
        {
          power = tmp;
        }
      }
      *val = power - *val;
      if( *val == 0 )
      {
        if( power >= pml::MAX_EFFORT_POWER || total >= pml::MAX_TOTAL_EFFORT_POWER )
        {
          static const u32 tbl[] =
          {
            msg_prwin_poke_ev_16_13,
            msg_prwin_poke_ev_16_23,
            msg_prwin_poke_ev_16_33,
            msg_prwin_poke_ev_16_43,
            msg_prwin_poke_ev_16_53,
            msg_prwin_poke_ev_16_63,
          };
          msg_id = tbl[kind-Savedata::ResortSave::RESORT_GIM_KIND_HP];
        }
        else
        {
          msg_id = msg_prwin_poke_ev_16_01;
        }
      }
      else
      {
        if( power >= pml::MAX_EFFORT_POWER || total >= pml::MAX_TOTAL_EFFORT_POWER )
        {
          static const u32 tbl[] =
          {
            msg_prwin_poke_ev_16_12,
            msg_prwin_poke_ev_16_22,
            msg_prwin_poke_ev_16_32,
            msg_prwin_poke_ev_16_42,
            msg_prwin_poke_ev_16_52,
            msg_prwin_poke_ev_16_62,
          };
          msg_id = tbl[kind-Savedata::ResortSave::RESORT_GIM_KIND_HP];
        }
        else
        {
          static const u32 tbl[] =
          {
            msg_prwin_poke_ev_16_11,
            msg_prwin_poke_ev_16_21,
            msg_prwin_poke_ev_16_31,
            msg_prwin_poke_ev_16_41,
            msg_prwin_poke_ev_16_51,
            msg_prwin_poke_ev_16_61,
          };
          msg_id = tbl[kind-Savedata::ResortSave::RESORT_GIM_KIND_HP];
        }
      }
    }
    break;
  }

  return msg_id;
}


GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
