//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEventCheck.cpp
 *  @brief  イベントチェック処理
 *  @author tomoya takahashi
 *  @date   2015.05.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <ui/include/gfl2_UI.h>
#include <debug/include/gfl2_DebugWinSystem.h>

#include "GameSys/include/GameData.h"

#include "FieldEventCheck.h"
#include "FieldEventCheckTrainerEye.h"
#include "FieldEventCheckGimmickEncount.h"

// Field
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

// コリジョンシステム
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h"
#include "Field/FieldRo/include/Collision/CollisionSceneNone.h"


// Script
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptTypes.h"
#include "FieldScript/include/EventScriptCall.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

// Event
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldRo/include/Event/FieldEventKairikyRock.h"
#include "NetStatic/NetEvent/include/JoinFestaAttractionRetireEvent.h"

// Actor
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldEncountActor.h" // エンカウントテーブルデータID取得のため
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"

#include "Field/FieldRo/include/PlacementData/actor/FieldMapJumpActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldBgEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldItemActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
#include "niji_conv_header/field/mapjump/FieldMapJumpType.h"

// Encount
#include "Field/FieldRo/include/Encount/FieldEncount.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"

// Subscreen
#include "Field\FieldRo\include\Subscreen\FieldSubScreenProcManager.h"
#include "Field\FieldRo\include\Subscreen\FieldSubscreenLauncher.h"
#include "App/FieldMenu/include/FieldMenuSubWindowProc.h"
#include "App/FieldMenu/include/FieldMenuFrame.h"

// Debug
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// ActionButtonManager
#include "Field/FieldRo/include/ActionButton/ActionButtonManager.h"

// GimmickEncount
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"

// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"

// zone
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

// conv_header
#include "niji_conv_header/field/mapjump/FieldMapJumpType.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"

// joinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"
#include "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystem.h"
#include "NetStatic/NetEvent/include/Debug/JoinFestaDummySendEvent.h"
#include <niji_conv_header/field/script/inc/join_festa_attraction.inc>

#include "PokeTool\include\NakayoshiManager.h"
#include "PokeTool/include/NatsukiManager.h"
#include "niji_conv_header/field/script/inc/field_event.inc"

// FieldSodateya
#include "Field/FieldStatic/include/Sodateya/FieldSodateya.h"

#include "App/FieldMenu/source/ReportPlayer.h"

// エラーマネージャー
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// QR
#include "Savedata/include/QRReaderSave.h"
#include <niji_conv_header/field/script/inc/poke_search.inc>

// ロトムパワー
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"
#include "niji_conv_header/field/script/inc/field_item.inc"

namespace Field{

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

// 会話判定　Dist算出　開始距離
const f32 FieldEventCheck::SELECT_TALK_START_DIST = 30.0f;

#if PM_DEBUG
f32 FieldEventCheck::m_DebugTalkCheckCosLimit = 80.0f;  ///< 会話出来る角度のリミット値
u32 FieldEventCheck::m_DebugMoveStepCount = 0;
u32 FieldEventCheck::m_DebugStepCount = 0;
#else
const f32 FieldEventCheck::m_DebugTalkCheckCosLimit = 80.0f;  ///< 会話出来る角度のリミット値
#endif // PM_DEBUG

#if PM_DEBUG
b32 FieldEventCheck::m_IsDebugPokemonHeapMemorySizeCheck  = false;
u32 FieldEventCheck::m_DebugPokemonHeapFreeMemorySize     = 0;
u32 FieldEventCheck::m_DebugPokemonHeapAllocatableSize    = 0;
u32 FieldEventCheck::m_DebugPokemonHeapCheckZoneID    = 0;
#endif


//-----------------------------------------------------------------------------
/**
*         クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *  @brief    イベントチェッククラス　データ
 */
//-----------------------------------------------------------------------------


/* コンストラクタ・デストラクタ */
FieldEventCheck::FieldEventCheck() : 
  m_Mode(CHECKMODE_NORMAL)
{
  std::memset( &m_Request, 0, sizeof(EventCheckRequest));
}
FieldEventCheck::~FieldEventCheck()
{
}

//----------------------------------------------------------------------------
/**
 *  @brief    イベントチェック　初期化
 *
 *  @param    mode         チェックモード
 *  @param    p_gameman    ゲームマネージャ
 *  @param    p_fieldmap   フィールドマップ
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::Initialize( const Desc& desc )
{
  if( desc.mode < CHECKMODE_MAX ){
    m_Mode = desc.mode;
    ICHI_PRINT("FieldEventCheckMode:%d \n", m_Mode );
  }else{
    m_Mode = CHECKMODE_MAX;  //何もチェックしない。
  }

  // リクエストワークの初期化
  InitializeRequest( &m_Request, desc.p_gameman, desc.p_fieldmap );


  // イベント用コリジョンシーンの作成
  {
    m_pCollsionSceneForEventPos         = desc.pCollisionSceneForEventPos;
    m_pCollsionSceneForEventIntrPos     = desc.pCollisionSceneForEventIntrPos;
    m_pCollsionSceneForEventTalk        = desc.pCollisionSceneForEventTalk;
    m_pCollsionSceneForEventPush        = desc.pCollisionSceneForEventPush;
    m_pCollsionSceneForEncount          = desc.pCollisionSceneForEncount;
    m_pCollsionSceneForTrainerEye       = desc.pCollisionSceneForTrainerEye;
    m_pCollsionSceneForEventNaminori    = desc.pCollisionSceneForEventNaminori;
    m_pCollsionSceneForGimmickEncount       = desc.pCollisionSceneForGimmickEncount;
    m_pCollsionSceneForGimmickEncountAction = desc.pCollisionSceneForGimmickEncountAction;
    m_pCollsionSceneForEventFinder          = desc.pCollisionSceneForEventFinder;
    m_pCollisionSceneForFishingPos          = desc.pCollisionSceneForFishingPos;
  }
}



//----------------------------------------------------------------------------
/**
 *  @brief    イベントチェック
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::EventCheck( void )
{
  // 初期化していない場合には、何もしない。
  if( m_Mode == CHECKMODE_MAX ){
    return ;
  }

#if PM_DEBUG
  // デバッグメニュー起動中は、EventCheckを実行しない。 #5710対策
  //こうしないとデバッグメニューの選択肢を選んだ際にAボタンで反応するものがあると、メニューの裏でイベントが起動してしまう
  if( gfl2::debug::DebugWin_IsOpen()  ){
    return;
  }
#endif
  EventCheckRet ret = RET_EVENT_NONE;

  // リクエストアップデート
  UpdateRequest( &m_Request );

  // コリジョン更新　会話ターゲットの選出
  UpdateCollision();


  // アクションボタンの注視処理をクリアしてOK
  m_ActionButtonInterestNotClear = false;

  // メニュー操作可能にする
  m_Request.p_fieldmap->GetFieldSubScreenManager()->Resume();

  // ギミックエンカウントイベントチェック前更新
  if( m_Request.p_gameman->GetFieldResident()->GetGimmickEncountManager() ) m_Request.p_gameman->GetFieldResident()->GetGimmickEncountManager()->EventCheckPreUpdate();

  // カメラサポート機能のSTOP解除
  m_Request.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::RESUME );
  
  // カメラフォーカス機能のSTOP解除
  if( m_Request.p_fieldmap->GetCameraManager()->GetFocus() )
  {
    m_Request.p_fieldmap->GetCameraManager()->GetFocus()->ClearEventStopBit();
  }

  // @fix GFNMCat[887] 1フレーム内でV字回復ができるようにAddEventRequestAllとの排他をやめ、AddEventRequestAllよりも先に行う
  //                   eventCntが"1->2->1"になるのではなく"1->0->1"になるようにする
  m_Request.p_gameman->GetGameData()->GetFieldCharaModelManager()->PullEventRequestAll();


#if PM_DEBUG
  if(FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_EXTHEAP_CHECK))
  {
    // GetFieldExtHeapの残量が変わっていないかチェック
    if( m_IsDebugPokemonHeapMemorySizeCheck )
    {
      gfl2::heap::HeapBase* pFieldExtHeap = m_Request.p_gameman->GetGameData()->GetFieldArea()->GetFieldExtHeap();
      if(pFieldExtHeap)
      {
        if( m_DebugPokemonHeapCheckZoneID == m_Request.p_fieldmap->GetZoneID() )  // ZONEIDが違う＝マップジャンプなのでチェック無用
        {
          u32 free        = pFieldExtHeap->GetTotalFreeSize();
          u32 allocatable = pFieldExtHeap->GetTotalAllocatableSize();
          if( !((free == m_DebugPokemonHeapFreeMemorySize) && (allocatable == m_DebugPokemonHeapAllocatableSize)) )
          {
            GFL_ASSERT_MSG( (free == m_DebugPokemonHeapFreeMemorySize) && (allocatable == m_DebugPokemonHeapAllocatableSize), "イベント完了時にFieldExtHeapの残量が変動しています。before 0x%x 0x%x  after 0x%x 0x%x\n", m_DebugPokemonHeapFreeMemorySize, m_DebugPokemonHeapAllocatableSize, free, allocatable );
          }
        }
      }
      m_IsDebugPokemonHeapMemorySizeCheck = false;
    }
  }
#endif
  
  // デバッグチェック
#if PM_DEBUG
  {
    ret = CheckDebug( m_Request );
    if( ret == RET_DEBUG_FORCE_END )
    {
      return;
    }
  }
#endif


  // INTRイベント
  // @note INTRイベントの起動では通常行なう、イベント時のクリア処理やフック処理は実行されない。
  {
    const EventCheckRequest& reqData = m_Request;

    if ( reqData.moveRequest )
    {
      CheckIntrPosEvent(m_Request);
    }
  }

  //-------------------------------------
  /// 各種タイプごとのイベントチェック
  //=====================================
  if( ret == RET_EVENT_NONE )
  {
    // 各モードのイベント起動チェック
    switch( m_Mode ){
    case CHECKMODE_NORMAL:  // 通常時用イベントチェック
      ret = CheckNormal( m_Request );
      break;

    case CHECKMODE_JOIN_FESTA:  ///< ジョインフェスタ時用イベントチェック
      ret = CheckJoinFesta( m_Request );

      // ジョインフェスタではイベント発動時一律一言ウィンドウ非表示にする
      if( ret != RET_EVENT_NONE )
      {
        // ジョインフェスタ外マップのみ
        u32 gimmickId = m_Request.p_fieldmap->GetFieldGimmickID( Fieldmap::GIMMICK_IDX_ZONE );
        if( gimmickId == Field::GIMMICK_TYPE_JOIN_FESTA )
        {
          // 一言ウィンドウ非表示
          FieldGimmickJoinFesta* p_jfGimmick= static_cast<FieldGimmickJoinFesta*>(m_Request.p_fieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
          p_jfGimmick->GetJoinFestaEffectSystem()->HideMessageEffect(true);
        }
      }
      break;

    default:
      break;
    }
  }

  // 弱いリザーブ
  {
    FieldScript::FieldScriptSystem* p_script = m_Request.p_gameman->GetFieldScriptSystem();
    if ( !IsEventCheckRetNoneEvent(ret) || m_Request.enableWeakReserve == false )
    {
      // 他にイベントが発生した場合や発動タイミングでない場合は弱いリザーブは破棄
      p_script->SetWeakReservedScript( Field::FieldScript::SCRID_DUMMY );
    }
    // 弱いリザーブ発動チェック
    else if ( p_script->CheckRunWeakReserveScript() )
    {
     // stopInterestBGFlag = true;
      ret = RET_EVENT_NORMAL_EVENT;
    }
  }

  // デバッグチェック(下流)
#if PM_DEBUG
  {
    if( ret == RET_EVENT_NONE )
    {
      ret = CheckDebugBottom( m_Request );
    }
  }
#endif

  //
  // イベントの有無をフィールドオブジェクトに通知
  //
  {
    if( IsEventCheckRetNoneEvent(ret) )
    {
      #if PM_DEBUG
        FieldScript::FieldScriptSystem::DebugRegdentHeapCheckDisableFlag = false;
      #endif
    }
    else
    {
#if PM_DEBUG
      if(FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_EXTHEAP_CHECK))
      {
        // GetFieldExtHeapの残量が変わっていないかチェック
        gfl2::heap::HeapBase* pFieldExtHeap = m_Request.p_gameman->GetGameData()->GetFieldArea()->GetFieldExtHeap();
        if(pFieldExtHeap)
        {
          m_DebugPokemonHeapFreeMemorySize  = pFieldExtHeap->GetTotalFreeSize();
          m_DebugPokemonHeapAllocatableSize = pFieldExtHeap->GetTotalAllocatableSize();
          m_IsDebugPokemonHeapMemorySizeCheck = true;
          m_DebugPokemonHeapCheckZoneID     = m_Request.p_fieldmap->GetZoneID();
        }
      }
#endif



      // イベントステータスを立てる。
      m_Request.p_gameman->GetGameData()->GetFieldCharaModelManager()->AddEventRequestAll();

      //話しかけ対象アクターをクリアするためにNULLをセット
      m_Request.ptalkActor = NULL;

      // 下画面を一時停止
      if( ret != RET_EVENT_MENU_EVENT )
      {
        m_Request.p_fieldmap->GetFieldSubScreenManager()->Suspend();
    
    // @fix GFNMcat[3975]:Xメニューを押した際に、振り向きが解除される、対処。下画面がパッシブになるイベント起動時に注目を切るように変更
        StopNpcInterest();
      }

      // アクションイベントの注目処理をクリアしないフラグ設定
      if( ret == RET_EVENT_NOT_CLEAR_INTEREST_EVENT )
      {
        m_ActionButtonInterestNotClear = true;
      }
    }
  }

  // ギミックエンカウントイベントチェック前更新
  if( m_Request.p_gameman->GetFieldResident()->GetGimmickEncountManager() ) m_Request.p_gameman->GetFieldResident()->GetGimmickEncountManager()->EventCheckPostUpdate();
}



//-----------------------------------------------------------------------------
/**
 *      共通処理
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  リクエストワークの初期化
 *
 *  @param  p_reqData     データ設定するリクエストワーク
 *  @param  p_gameman     ゲームマネージャ
 *  @param  p_fieldmap    フィールドマップ
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::InitializeRequest( EventCheckRequest* p_reqData, GameSys::GameManager* p_gameman, Fieldmap* p_fieldmap )
{
  p_reqData->p_heap       = p_fieldmap->GetDeviceHeap();
  p_reqData->p_gameman    = p_gameman;
  p_reqData->p_gamedata   = p_gameman->GetGameData();
  p_reqData->p_evman      = p_gameman->GetGameEventManager();
  p_reqData->p_fieldmap   = p_fieldmap;
  p_reqData->p_player     = p_fieldmap->GetPlayerCharacter();
}

//----------------------------------------------------------------------------
/**
 *  @brief  リクエストワークの更新
 *
 *  @param  p_reqData     データ設定するリクエストワーク
 *
 *  ここに、イベント起動に必要なデータをまとめる。
 *
 *  デバッグ時に、ここを書き換えることで、強制的にイベントを起動させたり、
 *  何かしらの操作ができることを目指す。
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::UpdateRequest( EventCheckRequest* p_reqData )
{
  p_reqData->zone_id    = static_cast<u16>(p_reqData->p_fieldmap->GetZoneID());

  // リクエスト初期化
  p_reqData->talkRequest = false;
  p_reqData->menuRequest = false;
  p_reqData->moveRequest = false;
  p_reqData->stepRequest = false;
  p_reqData->subscreenRequest = false;
  p_reqData->debugRequest = false;
  p_reqData->isEncount  = true;
  p_reqData->encountRequest = false;
  p_reqData->moveStepRequest = false;
  p_reqData->enableWeakReserve = false;
  p_reqData->isHitHiddenItem = false;
  p_reqData->pFinderActor = NULL;

  Field::MoveModel::FieldMoveModelPlayer* pPlayer = reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(p_reqData->p_player);

  // 自機情報
  {
    p_reqData->now_attr = p_reqData->p_player->GetGroundAttribute();
  }

  // キー入力
  {
    const gfl2::ui::DeviceManager* p_deviceman = p_reqData->p_gameman->GetUiDeviceManager();

    p_reqData->cp_button   = p_deviceman->GetButton(0);
    p_reqData->cp_stick    = p_deviceman->GetStick(0);
    p_reqData->cp_crossKey = p_deviceman->GetCrossKey(0);


    if( p_reqData->cp_button->IsTrigger( gfl2::ui::BUTTON_A ) && pPlayer->IsEnableInput() )
    {
      p_reqData->talkRequest = true;
    }

    if( pPlayer->IsMoveState() ){
      p_reqData->moveRequest = true;
      p_reqData->moveStepRequest = p_reqData->p_player->IsOneStepFrame();
    }
  }

  // 1動作の完了リクエスト
  {
    p_reqData->stepRequest = ( p_reqData->p_player->IsOneStepFrame() || p_reqData->p_player->IsTurnFrame() );
    // @note 振り向き動作と、グリッドサイズ分移動した移動の終了時にtrue
    // @fix BTS[1611]対応 ゾーンチェンジとエンカウントが同時に発生すると、バトルとゾーンチェンジ内のBGM変更がかぶる
    if( !p_reqData->p_fieldmap->IsZoneChange() )
    { 
      p_reqData->encountRequest = ( p_reqData->p_player->IsOneStepFrame() || p_reqData->p_player->IsTurnFrame() );
    }
  }

#if PM_DEBUG
  {
    if( p_reqData->moveStepRequest )
    {
      m_DebugMoveStepCount ++;
    }
    if( p_reqData->stepRequest )
    {
      m_DebugStepCount ++;
    }
  }
#endif // PM_DEBUG

  // 下画面タッチリクエストチェック
  if( pPlayer->IsEnableInput() )
  {
    p_reqData->subscreenRequest = CheckSubScreenReq( p_reqData->p_fieldmap->GetFieldSubScreenManager() );
  }

  // デバッグ操作でのイベント起動フック処理
  {
#if PM_DEBUG
    // 
    p_reqData->debugRequest = p_reqData->cp_button->IsHold( gfl2::ui::BUTTON_L );
    if(p_reqData->debugRequest ){

      if(p_reqData->cp_button->IsHold(gfl2::ui::BUTTON_B))
      { //L+Bボタンのとき、各種イベントチェックを外す
        p_reqData->talkRequest = false;
        p_reqData->moveRequest = false;
        p_reqData->subscreenRequest = false;
        p_reqData->stepRequest = false;
        p_reqData->encountRequest = false;
      }
      // Lボタンでは野生エンカウント・トレーナー視線はチェックしない
      p_reqData->isEncount   = false;
    }
#endif

  }

}



#if PM_DEBUG

//----------------------------------------------------------------------------
/**
 *  @brief    デバッグ　起動　イベントチェック
 *
 *  @param  reqData リクエストデータ
 *
 *  @return イベント起動結果
 */
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckDebug( const EventCheckRequest& reqData )
{
  // 各モード終了処理
  {
  }

#if 0
  //レポート演出 デバッグ起動
  if( reqData.cp_button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    EventFieldReportPlayerTest::StartEvent(reqData.p_gameman);
    return RET_EVENT_NORMAL_EVENT;

  }
#endif

  return RET_EVENT_NONE;
}

//----------------------------------------------------------------------------
/**
* @brief    デバッグ　起動　イベントチェック(イベントチェックの一番最後に実行される)
*
* @param  reqData リクエストデータ
*
* @return イベント起動結果
*/
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckDebugBottom( const EventCheckRequest& reqData )
{
  return RET_EVENT_NONE;
}

#endif // #if PM_DEBUG


//-----------------------------------------------------------------------------
/**
 *      通常イベントチェック用
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  通常時用　イベントチェック
 *
 *  @param  リクエストデータ
 *
 *  @retval RET_EVENT_NONE,         //　イベントなし
 *  @retval RET_EVENT_NORMAL_EVENT, // 通常イベント
 *  @retval RET_EVENT_MENU_EVENT,   // フィールドメニューを動かす必要があるイベント
 */
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckNormal( const EventCheckRequest& reqData )
{
  // イベントチェックは、順序に気をつけて。

  //☆☆☆特殊スクリプト起動チェックがここに入る
  {
    FieldScript::FieldScriptSystem* p_script = reqData.p_gameman->GetFieldScriptSystem();
    bool is_event_start = false;
    
    // 予約スクリプトの発動チェック＆発動
    if( p_script->CheckRunReserveScript() )
    {
      is_event_start = true;
    }
#if PM_DEBUG
    // シーン起動スクリプト起動しないデバッグ
    else if ( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_SCENESCRIPT ) )
    {
      // シーン起動スクリプト発動チェック＆RUN
      if( p_script->SearchRunSceneScript() )
      {
        is_event_start = true;
      }
    }
#else
    // シーン起動スクリプト発動チェック＆RUN
    else if( p_script->SearchRunSceneScript() )
    {
      is_event_start = true;
    }
#endif

    if ( is_event_start )
    {
      reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
      if( reqData.p_fieldmap->GetCameraManager()->GetFocus() )
      {
        reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::FOCUS | Camera::Area::Focus::EventStopBit::SE );
      }

      return RET_EVENT_NORMAL_EVENT;
    }
    
    if( p_script->GetLastKeyXMenuOpen() )
    {
      p_script->ResetLastKeyXMenuOpen();
      // LatKeyWait()時の Xボタンによるメニュー起動
      b32 dummy;
      if( Field::SubScreen::CallEventForceMenuOpen( reqData.p_fieldmap->GetFieldSubScreenManager(), reqData.p_fieldmap->GetSubScreen(), reqData.p_gameman, &dummy ) )
      {
         reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );

         // カメラフォーカスBGM影響のSTOP
         if( reqData.p_fieldmap->GetCameraManager()->GetFocus() )
         {
           reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::BGM );
         }

        //動作終了
        return RET_EVENT_MENU_EVENT;
      }
    }
  }
  //☆☆☆特殊スクリプト起動チェック　ここまで
  
  //フィールドフェード終了チェック(フリー移動の際に画面フェードがかかりっぱなしのことはない)
#if 0
  {
    if(gfl::grp::Fade_IsFadeInStandbyState(gfl::grp::Fade::DISP_APPER) != false ){
      SCRIPT_ASSERT_MSG(0,"上画面フェード未解除\n");
      gfl::grp::Fade_Reset(gfl::grp::Fade::DISP_APPER); //フェード強制解除
    } 
    if(gfl::grp::Fade_IsFadeInStandbyState(gfl::grp::Fade::DISP_LOWER) != false ){
      SCRIPT_ASSERT_MSG(0,"下画面フェード未解除\n");
      gfl::grp::Fade_Reset(gfl::grp::Fade::DISP_LOWER); //フェード強制解除
    }
  }
#endif

  // フィールドBGMのステータスケア
  {
    FieldSound* pFieldSound = reqData.p_gameman->GetGameData()->GetFieldSound();
    pFieldSound->CareFieldBGMState();
  }

  //フィールドフェード終了チェック ここまで

  //☆☆☆トレーナー視線チェック
  // エンカウントチェックフラグ
#if PM_DEBUG
  if( reqData.isEncount && FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_TRAINER_ENCOUNT ) )
#else
  if( reqData.isEncount )
#endif
  {
    if( EventCheckTrainerEye::CheckTrainerEye( reqData.p_gameman, reqData.p_player ) )
    {
      reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
      return RET_EVENT_NORMAL_EVENT;
    }
  }
  //☆☆☆トレーナー視線チェック ここまで

  //☆☆☆ギミックエンカウントチェック
  if( reqData.isEncount )
  {
    if( EventCheckGimmickEncount::Check( reqData.p_gameman, reqData.p_player) )
    {
      reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );

      if( reqData.p_fieldmap->GetCameraManager()->GetFocus() )
      {
        reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::EFFECT | Camera::Area::Focus::EventStopBit::BGM | Camera::Area::Focus::EventStopBit::SE | Camera::Area::Focus::EventStopBit::INTERES );
      }

      return RET_EVENT_NORMAL_EVENT;
    }
  }
  //☆☆☆ギミックエンカウントチェック ここまで

  //☆☆☆一歩移動チェックがここから
  //@note アナログ移動は、距離で行う。
  Attr step_encount_attr = ATTR_ERROR;
  if( reqData.encountRequest )
  {
    step_encount_attr = reqData.now_attr;

    ZoneDataLoader* pZoneDataLoader = reqData.p_gameman->GetGameData()->GetFieldZoneDataLoader();
    ZoneDataAccessor zoneDataAccessor( pZoneDataLoader->GetZoneData( reqData.zone_id));

    //エンカウント率計算(POSイベントなどの前でやる)
    reqData.p_fieldmap->GetEncount()->CalcEncountProb( step_encount_attr, reqData.moveRequest, reqData.exMoveRequest, reqData.encountDataID, zoneDataAccessor.GetAttributeBgNearPattern() ); 
  }
  //☆☆☆一歩移動チェックここまで

  //☆☆☆移動チェックがここから
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>( reqData.p_player );
  if( reqData.moveRequest )
  {
    // POSチェック
    EventCheckRet ret = CheckPosEvent(reqData);
    if( ret != RET_EVENT_NONE )
    {
      return ret;
    }
  }

  // 常に釣りコリジョンヒットから反映 
  {
    EventCheckRet ret = CheckFishingPosEvent(reqData);
    if( ret != RET_EVENT_NONE )
    {
      return ret;
    }
  }

  
  //☆☆☆押し込み操作チェック（マットでのマップ遷移など）
  if( reqData.moveRequest ){
    // Pushチェック
    EventCheckRet ret = CheckPushEvent( reqData );
    if( ret != RET_EVENT_NONE ){
      return ret;
    }
  }

#if 0
  // @note アナログ移動でもグリッドをまたいだときに起動する。
  if( reqData.moveGridRequest )
  {
    // 座標イベントチェック
    {
      // ユニークシーケンス 特殊ステップチェック関数
      if( reqData.p_fieldmap )
      {
        FieldUniqueSequence* p_uniq = reqData.p_fieldmap->GetFieldUniqueSequence();
        if( p_uniq ){
          // ゾーンユニークシーケンス イベント発生チェック
          if( p_uniq->MoveRequestCheck( &reqData ) )
          {
            *pStopInterestBG = true;
            return RET_EVENT_NORMAL_EVENT;
          }
        }
        for( int i = 0;i < Fieldmap::U_SEQUENCE_AREA_IDX_MAX; i++ ){
          p_uniq = reqData.p_fieldmap->GetFieldUniqueSequenceArea(static_cast<Fieldmap::UniqueSequenceAreaIndex>(i));
          if( p_uniq ){
            // エリアユニークシーケンス イベント発生チェック
            if( p_uniq->MoveRequestCheck( &reqData ) )
            {
              *pStopInterestBG = true;
              return RET_EVENT_NORMAL_EVENT;
            }
          }
        }
      }

      // POSイベントチェック
      if( CheckPosEvent( reqData ) )
      {
        *pStopInterestBG = true;
        HOSAKA_PRINT( "Check Pos Event\n" );
        return RET_EVENT_NORMAL_EVENT;
      }
      // 座標接続チェック
      if( CheckExit( reqData ) )
      { 
        *pStopInterestBG = true;
        return RET_EVENT_NORMAL_EVENT; 
      }

      // アトリビュートチェック
      if( CheckAttribute( reqData ) )
      { 
        *pStopInterestBG = true;
        return RET_EVENT_NORMAL_EVENT; 
      }
    }
    
    // エンカウントチュートリアル処理
#if PM_DEBUG
    if( reqData.isEncount && FieldDebug::EnableWildEncount )
#else
    if( reqData.isEncount )
#endif
    {
      if ( CalcEncountTutorial(reqData.now_attr, reqData) ) 
      {
        *pStopInterestBG = true;
        reqData.p_player->ForceStop();
        return RET_EVENT_NORMAL_EVENT;
      }
    }
  }
#endif
  //☆☆☆移動チェックここまで

  // 汎用　一歩移動をカウント　イベント
  // @note ここ以下の処理は、すべて距離で見る。
  if( CheckMoveEvent( reqData ) )
  {
    reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
    if( reqData.p_fieldmap->GetCameraManager()->GetFocus() )
    {
      reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::FOCUS | Camera::Area::Focus::EventStopBit::SE );
    }

#if 0
    *pStopInterestBG = true;
#endif
    return RET_EVENT_NORMAL_EVENT;
  }

  // 汎用　一歩移動をカウント　ここまで
  //☆☆☆ステップチェック（一歩移動or振り向き）がここから
  //@note アナログ移動は移動距離で1歩と考える。
  if( reqData.encountRequest )
  {
    // @note 方向付きPOSイベントのチェックタイミング
    // xyでは方向付きPOSイベントが削除された

    // POS イベント 侵入
    // CheckPosEvent_IntrudeDir();

    // エンカウントチェック
#if PM_DEBUG
    if( reqData.isEncount && FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_WILD_ENCOUNT ) )
#else
    if( reqData.isEncount )
#endif
    {
      // 通常エンカウント
      if( CheckNormalEncountEvent(reqData, step_encount_attr) )
      {
#if 0
        *pStopInterestBG = true;
        reqData.p_player->ForceStop();
#endif

        if( reqData.p_fieldmap->GetCameraManager()->GetFocus() )
        {
          reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::EFFECT | Camera::Area::Focus::EventStopBit::BGM | Camera::Area::Focus::EventStopBit::SE | Camera::Area::Focus::EventStopBit::INTERES );
        }

        reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
        return RET_EVENT_NORMAL_EVENT;
      }

    }

#if 0
    // エンカウントフラグが折れていても発生させる
    reqData.p_gamedata->GetFieldContactManager()->UpdatePlayerStep( reqData.p_gameman );

    // 自機のフォルムチェンジ（ステップタイミング）
    if( reqData.p_player->CheckStepEvent( reqData.p_evman, reqData ) )
    {
      *pStopInterestBG = true;
      return RET_EVENT_NORMAL_EVENT;
    }
#endif
  }
  //☆☆☆ステップチェック（一歩移動or振り向き）ここまで

  // QRスキャン時間経過チェック
  if( CheckQRScanTimeout( reqData))
  {
    return RET_EVENT_NORMAL_EVENT;
  }

  //☆☆☆会話チェック ここから
  if( reqData.talkRequest )
  {
    // 話しかけチェック
    EventCheckRet ret;
    if( (ret = CheckTalkEvent( reqData )) != RET_EVENT_NONE ){
      return ret;
    }

#if 0
    HOSAKA_PRINT("talkRequest\n");

    s32 index = 0;
    for( ; index<EVENT_CHECK_REQ_DIR_TBL_MAX; ++index )
    {
      // チェックする方向のポジションがあるか？
      if(!reqData.dirPosIsExist[index]){
        continue;
      }

      // Priority的に同一オブジェクトでのループ数を求める。
      s32 roop_end = index + 1;
      for( ; roop_end<EVENT_CHECK_REQ_DIR_TBL_MAX; ++roop_end )  // OBJ話しかけ
      {
        if( reqData.dirPriority[roop_end] != reqData.dirPriority[index] ){
          break;
        }
      }

      // NPC話しかけ
      for( s32 j=index; j<roop_end; ++j )  
      { 
        if( CheckFrontTalkModel(reqData, j) )
        {
          *pStopInterestBG = true;

          // オブジェクトの動作をキャンセル
          reqData.p_player->ForceStop();
          return RET_EVENT_NORMAL_EVENT;
        }
      }

      // 注目BG話しかけ top 優先度でのみチェック
      // @note 看板注目への話しかけでは看板注目を停止しない
      if( index == 0 ){
        s32 bgid = reqData.p_player->GetInterestEventID();
        TOMOYA_PRINT("BG talk Interest %d\n", bgid);
        // @note イベント開始時の看板注目停止に抜けがあった場合、イベントIDのクリアが1フレーム遅れている可能性がある
        if( bgid != PlayerGrid::INTEREST_EVENT_ID_NONE )
        {
          EventDataSystem* evsys = reqData.p_gamedata->GetEventDataSystem();
          u16 scrid = evsys->GetBGTalkByID( bgid )->id;
          TOMOYA_PRINT("ScrID %d\n", scrid);
          if( IsExistBGEventScrID(scrid, reqData) ){  // ダミーチェック
            EventScriptCall* ev;
            ev = EventScriptCall::CallScriptBG( reqData.p_gameman, scrid, bgid );
            // オブジェクトの動作をキャンセル
            reqData.p_player->ForceStop();
            return RET_EVENT_NORMAL_EVENT;
          }
        }
      }

      // プレイヤーの一歩前の座標を求める
      Position calc_pos( reqData.p_fieldmap->GetMapper()->GetRailSystem(), reqData.p_fieldmap->GetMapper() );
      Dir dir = static_cast<Dir>(reqData.player_dir);

      // @fix BTS[4584] の類似: Vectorベースの計算では、四捨五入により、グリッドが変更されることがあった。
      // グリッドベースの計算にすることにより、四捨五入ででる、グリッドのズレを解消した。
      bool is_ret = reqData.p_player->GetModel()->GetDirGridCenterPos( dir, &calc_pos );

      // 隠しアイテムチェック
      for( s32 j=index; j<roop_end; ++j )  
      {
        if( is_ret && (dir == reqData.dirTbl[j]) )  // 目の前が取得できたときだけ
        {
          u32 scrid = SCRID_DUMMY;
          EventWork* evwork = reqData.p_gamedata->GetEventWork();
          
          const PositionData* pos = calc_pos.GetOriginalConst();
          hideitem::Manager* hideitem = reqData.p_gamedata->GetHideItemManager();

          if( pos->type == TYPE_VECTOR ){ // グリッド座標の場合にしか対応していない。
            u32 gx = pos->vector.x / GRID_SIZE;
            u32 gz = pos->vector.z / GRID_SIZE;
            u32 seed = reqData.p_gamedata->GetRandomGroup()->GetTodayRandSeed();
            u32 item_no;
            u32 flag_no;
            savedata::Misc * p_misc = reqData.p_gamedata->GetMisc();

            b32 ret = hideitem->SearchHideItem( evwork, p_misc, seed, reqData.zone_id, gx, gz, &item_no, &flag_no );
            
            if( ret )
            {
              PlayerGrid* p_field_player = reqData.p_fieldmap->GetFieldPlayer();
              // ダウジング中なら番組を生成
              if (p_field_player->GetMoveForm() == UNIT_MOVE_FORM_DOWSING)
              {
                u16 count = reqData.p_gamedata->GetSangoNetworkSaveData()->GetDowsingCount();
                netapp::util::SangoTVNaviProgramUtility::CreateTVProgramData_08( item_no, count );
                HOSAKA_PRINT("ダウジング番組生成 count=%d item_no=%d \n", count, item_no);
              }

              // 隠しアイテムイベント発動
              EventScriptCall* ev;
              ev = EventScriptCall::CallScript( reqData.p_gameman, SCRID_HIDEITEM, NULL, item_no, flag_no );
            
              // @fix GFSBTS[387]:自機が話しかけ対象方向を向かない対処、スクリプト起動イベントに自機方向を指定
              ev->SetTargetDir( reqData.dirTbl[j] );

              *pStopInterestBG = true;

              return RET_EVENT_NORMAL_EVENT;
            }
          }
        }
      }

      // BG話し掛け イベントチェック
      for( s32 j=index; j<roop_end; ++j )  
      {
        if( is_ret && (dir == reqData.dirTbl[j]) )  // 目の前が取得できたときだけ
        {
          u32 bgid;
          u32 scrid = SCRID_DUMMY;
            
          EventDataSystem* evsys = reqData.p_gamedata->GetEventDataSystem();

          // カイオーガ例外処理
          if( reqData.p_player->GetMoveForm() == UNIT_MOVE_FORM_DIVING_KAIOUGA )
          {
            f32 length = 2.5f * GRID_SIZE; //GFSBTS[1598] ダイビングカイオーガのBG話しかけレンジ範囲調整(D118R0102にしか無い。基点とレンジは感覚値)
            gfl::math::VEC3 front_pos(0,0,0);
            calc_pos.GetTranslate( &front_pos );
            bgid = evsys->SearchTalkBGEvent( reqData.p_fieldmap, &front_pos, dir, length );
          }
          else
          {
            EventWork* evwork = reqData.p_gamedata->GetEventWork();
            bgid = evsys->CheckTalkBGEvent( evwork, &calc_pos, dir );
          }

          if ( bgid != EVENTDATA_ID_NONE )
          {
            scrid = evsys->GetBGTalkByID( bgid )->id;
          }
          
          if( IsExistBGEventScrID(scrid, reqData) )  // ダミーチェック
          {
            // 座標イベント発動
            EventScriptCall* ev;
            ev = EventScriptCall::CallScriptBG( reqData.p_gameman, scrid, bgid );
            // @fix GFSBTS[387]:自機が話しかけ対象方向を向かない対処、スクリプト起動イベントに自機方向を指定
            ev->SetTargetDir( reqData.dirTbl[j] );

            *pStopInterestBG = true;
            return RET_EVENT_NORMAL_EVENT;
          }
        }
      }

      // 秘伝技
      for( s32 j=index; j<roop_end; ++j )  
      {
        if( CheckTalkSkillUse( reqData, j ) ){
          *pStopInterestBG = true;
          return RET_EVENT_NORMAL_EVENT;
        }
      }

      // チェック完了したインデックスにする。
      index = roop_end-1;
    }
#endif
  }
  //☆☆☆会話チェック  ここまで


#if 0
  //キー入力接続チェック
  if( reqData.pushRequest )
  {
    // POSイベントチェック
    if( CheckPushPosEventDir( reqData ) )
    { 
      *pStopInterestBG = true;
      return RET_EVENT_NORMAL_EVENT; 
    }
    
    // マットや階段などのマップ遷移
    if( CheckPushExit( reqData ) )
    {
      *pStopInterestBG = true;
      return RET_EVENT_NORMAL_EVENT; 
    }

    // フィールド押し込み系ギミック
    if( CheckPushGimmick( reqData ) )
    { 
      *pStopInterestBG = true;
      return RET_EVENT_NORMAL_EVENT; 
    }

    // ふれあいエンカウント押し込み
    if ( CheckPushContactEncount( reqData ) ) 
    { 
      *pStopInterestBG = true;
      return RET_EVENT_NORMAL_EVENT; 
    }

    // 自機のフォルムチェンジ（プッシュタイミング）
    if( reqData.p_player->CheckPushEvent( reqData.p_evman, reqData ) )
    {
      *pStopInterestBG = true;
      return RET_EVENT_NORMAL_EVENT;
    }

    // ふれあいエンカウントイベント
  }
#endif
  //☆☆☆押し込み操作チェック（マットでのマップ遷移など）　ここまで


  //☆☆☆自機フォルム変更イベント ここから
  {
    // ☆波乗り解除チェック
    if( CheckCancelNaminori( reqData ) )
    {
      return RET_EVENT_NORMAL_EVENT;
    }

    // ☆波乗り開始チェック
    if( reqData.talkRequest )
    {
      if( CheckStartNaminori( reqData ) )
      {
        return RET_EVENT_NORMAL_EVENT;  
      }
    } 

  }
#if 0
  {
    // ポケモンに乗る
    // ポケモンに乗るアクション
    // イスから立つ
    // 怪力
    // クライム
    // ローラースケート
    // @note アナログ歩き〜グリッド歩き間のチェンジイベントでは看板注目を停止しない
    if( reqData.p_player->CheckMoveEvent( reqData.p_evman, reqData, pStopInterestBG ) )
    {
      return RET_EVENT_NORMAL_EVENT;
    }
  }
#endif
  //☆☆☆自機フォルム変更イベント ここまで
  
  //☆☆☆ギミックによるイベント起動チェック　ここから
#if 1
  if( reqData.p_fieldmap )
  {
    for( int i = 0;i < Fieldmap::GIMMICK_IDX_MAX; i++ ){
      FieldGimmick * pGimmick = reqData.p_fieldmap->GetFieldGimmick(static_cast<Fieldmap::GimmickIndex>(i));
      if( pGimmick ){
        // エリアユニークシーケンス イベント発生チェック
        if( pGimmick->EventRequestCheck( &reqData ) )
        {
          return RET_EVENT_NORMAL_EVENT;
        }
      }
    }
  }
#endif
  //☆☆☆ギミックによるイベント起動チェック ここまで

  //通信エラー画面呼び出しチェック


#if 1
  //メニュー起動チェック
  // @note Xメニューの起動では看板注目を停止しない、メニューの中でイベント発動があった際は停止する
  if( reqData.subscreenRequest && reqData.p_fieldmap ){ // @fix cov_ctr[11022]NullPtrCheck
    b32 dummy;
    u32 focusStopBit;
    int mode = Field::SubScreen::CallEvent( reqData.p_fieldmap->GetFieldSubScreenManager(), reqData.p_fieldmap->GetSubScreen(), reqData.p_gameman, &dummy, &focusStopBit );
    // イベントが起動した場合
    if( mode != Field::SubScreen::FIELDSUBSCREEN_NONE )
    {
      reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
    
      // カメラフォーカスBGM影響のSTOP
      if( reqData.p_fieldmap->GetCameraManager()->GetFocus() && focusStopBit != 0 )
      {
        reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( focusStopBit );
      }

      //動作終了
      if(mode == Field::SubScreen::FIELDSUBSCREEN_NORMAL){
        return RET_EVENT_NORMAL_EVENT;
      }
      return RET_EVENT_MENU_EVENT;
    }
  }
#endif

  // ロトムパワー終了イベントチェック
  if (CheckRotomPowerTimeout(reqData))
  {
    return RET_EVENT_NORMAL_EVENT;
  }

  return RET_EVENT_NONE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  一歩移動イベントチェック
 */
//-----------------------------------------------------------------------------
bool FieldEventCheck::CheckMoveEvent( const EventCheckRequest& reqData )
{
  // 以下は通常一歩移動のみ対応
  // @note 歩数カウントは、アナログ移動は、距離で見る。
  if( !reqData.moveStepRequest )  
  {
    return false;
  }

  
  
  // なつきど上昇
  // 可愛がりカウントチェック
  checkFriendlyStepCount( reqData );


  // 育て屋 経験値加算とタマゴ発生チェック
  reqData.p_fieldmap->GetSodateya()->PokeBreed();
  
  // 育てやチェック
  if( CheckSodateya(reqData) != false ){
    return true;
  }

  // 卵負荷以外のイベントも念のためフックする。
  if( !reqData.p_fieldmap->IsZoneChange() )
  { 
    // 虫除けスプレーチェック
    if( CheckSpray(reqData) != false )
    {
      return true;
    }

  }
  
  return false;
}


//----------------------------------------------------------------------------
/**
 *  @brief  なつきど上昇処理
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::checkFriendlyStepCount( const EventCheckRequest& reqData )
{
  // なかよしど
  {
    enum {
      KAWAIGARI_ASOBI_MANPUKU_COUNT_MAX = 100, // 100歩に1回更新する  ///< まんぷく/なでなで：100歩　歩くたびにそれぞれ[-1]する 谷要望 2016/04/13
    };

    Savedata::Situation* p_situation = reqData.p_gamedata->GetSituation();
    u16 step_count = p_situation->GetKawaigariStepCount();

    ++ step_count;
    if( (step_count % KAWAIGARI_ASOBI_MANPUKU_COUNT_MAX) == 0 )
    {
      step_count = 0;
      pml::PokeParty* party = reqData.p_gamedata->GetPlayerParty();
      PokeTool::nakayoshi::UpdateFeedAndNadenadeOnField( party );
    }
    p_situation->SetKawaigariStepCount(step_count);
  }

  //なつきど
  {
    enum {
      NATSUKI_STEP_MAX = 256, ///<256歩に1回チェックする  ///< なつき度：128歩毎にチェックしているのを256歩に変更する 谷要望 2016/04/12
      NATSUKI_UP_RATE = 2,    ///<1/2の確率でアップする
    };

    Savedata::Situation* p_situation = reqData.p_gamedata->GetSituation();

    u16 step_count;
    bool result = FALSE;
    step_count = p_situation->GetFriendlyStepCount();
    step_count ++;
    if ( step_count >= NATSUKI_STEP_MAX )
    {
      step_count = 0;
      result = true;
    }
    p_situation->SetFriendlyStepCount( step_count );
    if ( result )
    {
      // GFL_PRINT("check Natsuki up\n");
      pml::PokeParty * party = reqData.p_gamedata->GetPlayerParty();

      ZoneDataLoader  *zone_data = reqData.p_gameman->GetGameData()->GetFieldZoneDataLoader();
      ZoneDataAccessor zoneDataAccessor( zone_data->GetZoneData( reqData.p_fieldmap->GetZoneID() ) );
      u16 place_no  = zoneDataAccessor.GetPlaceLabelID();
      int party_count = party->GetMemberCount();
      for ( int i = 0; i < party_count; i++ )
      {
        if ( System::GflUse::GetPublicRand(NATSUKI_UP_RATE) == 0 )
        {
          pml::pokepara::PokemonParam * pp = party->GetMemberPointer( i );
          PokeTool::NatsukiManager::CalcWhenWalk( pp, place_no/*, reqData.p_gameman->GetAssistPowerManager()*/ );
        }
      }
    }
  }

}


/* -------------------------------------------------------------------------*/
/**
 * @brief 育て屋チェック
 *
 * @param reqData リクエストデータ
 *
 * @return trueの時イベント実行
 */
/* -------------------------------------------------------------------------*/
bool FieldEventCheck::CheckSodateya( const EventCheckRequest& reqData )
{
  pml::PokeParty* party = reqData.p_gamedata->GetPlayerParty();
  pml::pokepara::CoreParam * p_birth;
  u32 pos = 0;

  //タマゴ孵化イベントチェック
  updatePartyEggHatching( reqData.p_gamedata, party );

  if( checkPartyEggHatching( party, &p_birth, &pos ) )
  {
    // スクリプト起動
    EventScriptCall::CallScript( reqData.p_gameman, SCRID_EGG_HATCHING_MSG, NULL, NULL, pos );
    return true;
  }

  return false;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ポケパーティ　卵孵化チェック
 *
 *  @param  party     パーティ
 *  @param  pp_param  生まれたポケモン
 *
 *  @retval true    孵化
 *  @retval false   なし
 */
//-----------------------------------------------------------------------------
bool FieldEventCheck::checkPartyEggHatching( pml::PokeParty* party, pml::pokepara::CoreParam** pp_param, u32 *pos )
{
  int i;
  int pokeCount = party->GetMemberCount();

  for( i=0; i<pokeCount; ++i )
  {
    pml::pokepara::PokemonParam * pp = party->GetMemberPointer(i);
    u32      tamago_flag = pp->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG );
    u32      familiarity = pp->GetOriginalFamiliarity();

    if( (tamago_flag) && (familiarity == 0) ) 
    { // カウンタ0のタマゴを発見

      // 生まれたポケモン
      *pp_param = pp;
      *pos      = i;
      return true;
    }
  } 
  return false;

}


//----------------------------------------------------------------------------
/**
 *  @brief  ポケパーティ　孵化カウンタ更新
 *
 *  @param  gdata   ゲームデータ
 *  @param  party   パーティ
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::updatePartyEggHatching( GameSys::GameData * gdata, pml::PokeParty* party )
{
  enum {  ///< (MAX_STEP_COUNT / ONE_STEP_COUNT) がなつき度を更新に必要な歩数となる.
    ONE_STEP_COUNT = 0x0016C, ///< 要望リスト#429:タマゴ孵化環境の調整をしたい Modify. N.Takeda 2016/05/12
    MAX_STEP_COUNT = 0x10000, ///<カウント最大値
  };
  u32 count;

  // 状況データ取得
  Savedata::Situation* situation = gdata->GetSituation();

  // タマゴ孵化カウンタを更新
  count = situation->GetEggStepCount();

  count += GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->CalcHatch(ONE_STEP_COUNT);

  // 手持ちのタマゴに反映
  if( MAX_STEP_COUNT < count ) {
    updatePartyEggHatchingCount( party ); // タマゴのなつき度を更新
    count = 0; // カウンタ初期化
  }

  // セーブデータに反映
  situation->SetEggStepCount( count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケパーティ　孵化カウンタ更新  実際の更新処理
 *
 *  @param  party   パーティ
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::updatePartyEggHatchingCount( pml::PokeParty* party )
{
  int i;
  int party_count = party->GetMemberCount();
  u8 sub_value;

  sub_value  = getPartyEggHatchingValue( party );

  TOMOYA_PRINT( "update egg hatching value %d\n", sub_value );

  // 手持ちタマゴの残り歩数を減らす
  for( i=0; i<party_count; i++ )
  {
    pml::pokepara::PokemonParam * pp = party->GetMemberPointer( i );
    u32      tamago_flag  = pp->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG );

    // タマゴを発見
    if( tamago_flag ) { 
      // なつき度を更新
      pp->SubOriginalFamiliarity( sub_value );
      TOMOYA_PRINT( "sub poke familiarity value %d\n", pp->GetOriginalFamiliarity() );
    }
  }

}


//----------------------------------------------------------------------------
/**
 *  @brief  パーティ内のポケモンを見て、負荷のカウントスピードを選ぶ
 */
//-----------------------------------------------------------------------------
u32 FieldEventCheck::getPartyEggHatchingValue( const pml::PokeParty* party )
{
  int i;
  int pokeCount = party->GetMemberCount();

  for( i=0; i<pokeCount; i++ )
  {
    const pml::pokepara::PokemonParam * pp = party->GetMemberPointerConst(i);
    u32      tamago_flag = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
    TokuseiNo tokusei = pp->GetTokuseiNo();

    // タマゴは無視
    if( tamago_flag ) { continue; }

    // 特性「ほのおのからだ」「マグマのよろい」を持つポケモンがいる
    if( (tokusei == TOKUSEI_HONOONOKARADA) || (tokusei == TOKUSEI_MAGUMANOYOROI) ) {
      return 2;
    }
  }

  return 1;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief 虫除けスプレー使用チェック
 *
 * @param reqData リクエストデータ
 *
 * @return trueの時イベント実行
 */
/* -------------------------------------------------------------------------*/
bool FieldEventCheck::CheckSpray( const EventCheckRequest& reqData )
{
  EventWork* evwork = reqData.p_gamedata->GetEventWork();
  u8 end_f;

  //セーブデータ取得
  end_f = evwork->DecMushiyokeCount();
  //減算した結果0になったら効果切れメッセージ
  if( end_f ){
    // 継続して使用するチェック
    {
      u16 item_no = evwork->GetMushiyokeItemNo();
      Savedata::MyItem* p_myitem = reqData.p_gamedata->GetMyItem();

      if( p_myitem->GetItemNum( item_no ) )
      {
        // まだスプレーが残っている
        EventScriptCall::CallScript( reqData.p_gameman, SCRID_FIELD_EVENT_SPRAY_USE_CONTINUE, NULL );
        return true;
      }
    }

    EventScriptCall::CallScript( reqData.p_gameman, SCRID_FIELD_EVENT_SPRAY_EFFECT_END, NULL );
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ジョインフェスタ時用　イベントチェック
 *
 *  @param  リクエストデータ
 *
 *  @retval RET_EVENT_NONE,         //　イベントなし
 *  @retval RET_EVENT_NORMAL_EVENT, // 通常イベント
 *  @retval RET_EVENT_MENU_EVENT,   // フィールドメニューを動かす必要があるイベント
 */
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckJoinFesta( const EventCheckRequest& reqData )
{
  // イベントチェックは、順序に気をつけて。

  // 通信エラーチェックは最優先
  {
    NetLib::Error::NijiNetworkErrorManager* pErrorManager = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager);

    if( pErrorManager->IsError() )
    {
      // 共通通信エラー対処
      pErrorManager->CommonNetworkErrorHandling();

      // 通信エラークリア
      pErrorManager->Clear();

      //ビーコンシステム再開
      NetLib::NijiNetworkSystem::RestartBeaconSystem();
    }
  }


  //☆☆☆特殊スクリプト起動チェックがここに入る
  {
    FieldScript::FieldScriptSystem* p_script = reqData.p_gameman->GetFieldScriptSystem();
    bool is_event_start = false;
    
    // 予約スクリプトの発動チェック＆発動
    if( p_script->CheckRunReserveScript() )
    {
      is_event_start = true;
    }
#if PM_DEBUG
    // シーン起動スクリプト起動しないデバッグ
    else if ( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_SCENESCRIPT ) )
    {
      // シーン起動スクリプト発動チェック＆RUN
      if( p_script->SearchRunSceneScript() )
      {
        is_event_start = true;
      }
    }
#else
    // シーン起動スクリプト発動チェック＆RUN
    else if( p_script->SearchRunSceneScript() )
    {
      is_event_start = true;
    }
#endif

    if ( is_event_start )
    {
      reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
      return RET_EVENT_NORMAL_EVENT;
    }
  }
  //☆☆☆特殊スクリプト起動チェック　ここまで

  //☆☆☆ジョインフェスタのアトラクション終了チェック　ここから
  {
    u32 gimmickId = reqData.p_fieldmap->GetFieldGimmickID( Fieldmap::GIMMICK_IDX_ZONE );
    
    // ジョインフェスタ外マップのみ
    if( gimmickId == Field::GIMMICK_TYPE_JOIN_FESTA )
    {
      // アトラクション中かどうか
      if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
      {
        // @fix GFNMcat[275] アトラクション中の無線通信OFF対応
        // @fix GFNMcat[2366]：無線スイッチOFF→ON対策
        //      ※最優先で処理する
        if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->CheckForceRetire() )
        {
          // アトラクションリタイアイベント
          GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaAttractionRetireEvent>( reqData.p_evman );
          return RET_EVENT_MENU_EVENT;
        }

        // 制限時間が0になったら終了
        if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetTimeLimitFrame() == 0 )
        {
          // ローカルアトラクション終了スクリプト起動
          EventScriptCall::CallScript( reqData.p_gameman, SCRID_JOIN_FESTA_ATTRACTION_END, NULL );
          return RET_EVENT_NORMAL_EVENT;
        }

        // ケンタロスライドでマップの岩全部壊したら
        if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsKentarosuRideRockBreakerAllBreak() )
        {
          // 岩が全部壊れた時スクリプト起動
          EventScriptCall::CallScript( reqData.p_gameman, SCRID_JOIN_FESTA_ATTRACTION_ROCK_ALL_BREAK, NULL );
          return RET_EVENT_NORMAL_EVENT;
        }
      }
    }
  }
  //☆☆☆ジョインフェスタのアトラクション終了チェック　ここまで

  
  //フィールドフェード終了チェック(フリー移動の際に画面フェードがかかりっぱなしのことはない)
  //フィールドフェード終了チェック ここまで

  //☆☆☆トレーナー視線チェック
  //ジョインフェスタはなし
  //☆☆☆トレーナー視線チェック ここまで

  //☆☆☆ギミックエンカウントチェック
  //ジョインフェスタはなし
  //☆☆☆ギミックエンカウントチェック ここまで

  //☆☆☆一歩移動チェックがここから
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>( reqData.p_player );
  if( reqData.moveRequest )
  {
    // POSチェック
    EventCheckRet ret = CheckPosEvent(reqData);
    if( ret != RET_EVENT_NONE )
    {
      return ret;
    }
  }
  //☆☆☆一歩移動チェックここまで

  //☆☆☆押し込み操作チェック（マットでのマップ遷移など）
  if( reqData.moveRequest ){
    // Pushチェック
    if( CheckPushEvent( reqData ) ){
      return RET_EVENT_NORMAL_EVENT;
    }
  }
  //☆☆☆押し込み操作チェック（マットでのマップ遷移など）　ここまで

  //☆☆☆移動チェックがここから
  //☆☆☆移動チェックここまで

  // 汎用　一歩移動をカウント　イベント
  // 汎用　一歩移動をカウント　ここまで

  //☆☆☆ステップチェック（一歩移動or振り向き）がここから
  //@note アナログ移動は移動距離で1歩と考える。
  //☆☆☆ステップチェック（一歩移動or振り向き）ここまで

  //☆☆☆会話チェック ここから
  if( reqData.talkRequest )
  {
    // 話しかけチェック
    EventCheckRet ret;
    if( (ret = CheckTalkEvent( reqData )) != RET_EVENT_NONE )
    {
      return ret;
    }
  }
  //☆☆☆会話チェック  ここまで


  //☆☆☆自機フォルム変更イベント ここから
  //☆☆☆自機フォルム変更イベント ここまで
  
  //☆☆☆ギミックによるイベント起動チェック　ここから
  //☆☆☆ギミックによるイベント起動チェック ここまで

  //通信エラー画面呼び出しチェック

  //メニュー起動チェック
  // @note Xメニューの起動では看板注目を停止しない、メニューの中でイベント発動があった際は停止する
  if( reqData.subscreenRequest ){
    b32 dummy;
    u32 focusStopBit;
    int mode = Field::SubScreen::CallEvent( reqData.p_fieldmap->GetFieldSubScreenManager(), reqData.p_fieldmap->GetSubScreen(), reqData.p_gameman, &dummy, &focusStopBit );
    // イベントが起動した場合
    if( mode != Field::SubScreen::FIELDSUBSCREEN_NONE )
    {
       reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );

       // カメラフォーカスBGM影響のSTOP
       if( reqData.p_fieldmap->GetCameraManager()->GetFocus() && focusStopBit != 0 )
       {
         reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( focusStopBit );
       }

      //動作終了
      if(mode == Field::SubScreen::FIELDSUBSCREEN_NORMAL){
        return RET_EVENT_NORMAL_EVENT;
      }
      return RET_EVENT_MENU_EVENT;
    }
  }

  // 

  //☆☆☆ジョインフェスタの一言ウィンドウ起動チェック　ここから
  {
    u32 gimmickId = reqData.p_fieldmap->GetFieldGimmickID( Fieldmap::GIMMICK_IDX_ZONE );
    
    // ジョインフェスタ外マップ
    if( gimmickId == Field::GIMMICK_TYPE_JOIN_FESTA )
    {
      if( reqData.ptalkActor )
      {
        IFieldActorBase *pBaseActor = reinterpret_cast<IFieldActorBase*>( reqData.ptalkActor->GetUserData() );
        FieldMoveModelActor* pFieldMoveModelActor = reinterpret_cast<FieldMoveModelActor*>(pBaseActor);

        // 話しかけ対象アクターが動作モデルかチェック
        if( pFieldMoveModelActor->GetType() == Field::FIELD_ACTOR_TYPE_MOVEMODEL )
        {
          // 動作モデルのイベントID取得
          u32 eventId = pFieldMoveModelActor->GetMoveModel()->GetEventId();

          FieldGimmickJoinFesta* p_jfGimmick= static_cast<FieldGimmickJoinFesta*>(reqData.p_fieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));

          // ジョインフェスタパーソナルデータ取得
          NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal;
          {
            NetApp::JoinFesta::JFModelManager*  p_jfModelManager = p_jfGimmick->GetJFDynamicLoadSystem()->GetJFModelManager();
            jfPersonal = p_jfModelManager->GetJoinFestaPersonalData( eventId ); 
          }

          // 一言ウィンドウ表示
          if( jfPersonal != NULL )
          {
            NetApp::JoinFesta::JoinFestaEffectSystem* p_jfEffectSystem = p_jfGimmick->GetJoinFestaEffectSystem();
            p_jfEffectSystem->ShowMessageEffect( jfPersonal );
          }

        }
      }
      else
      {
        // 一言ウィンドウ非表示
        FieldGimmickJoinFesta* p_jfGimmick= static_cast<FieldGimmickJoinFesta*>(reqData.p_fieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
        p_jfGimmick->GetJoinFestaEffectSystem()->HideMessageEffect();
      }
    }
  }
  //☆☆☆ジョインフェスタの一言ウィンドウ起動チェック　ここまで

#if PM_DEBUG
  // デバッグ用イベント起動
  {
    if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_IsDummySendEventCheck() )
    {
      // 起動フラグは落とす
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetIsDummySendEvent( false );
      NetEvent::JoinFestaDummySendEvent::StartEvent( reqData.p_gameman );
      return RET_EVENT_NORMAL_EVENT;
    }
  }
#endif

  return RET_EVENT_NONE;
}

FieldEventCheck::EventCheckRet FieldEventCheck::checkPosEventCore( const EventCheckRequest& reqData, DynamicActor* dynamic_actor, int result_index, b32 isIntr )
{
  if(dynamic_actor && dynamic_actor->GetCollisionResult(result_index)->pTargetActor )
  {
    // 裏世界にいるかチェック
    Field::MoveModel::FieldMoveModel* pCharaModel = reqData.p_player;
    if( pCharaModel && !pCharaModel->CheckValidGround() )
    {
      return RET_EVENT_NONE;
    }

    void *pUserData = dynamic_actor->GetCollisionResult(result_index)->pTargetActor->GetUserData();
    bool isEventCreate = false;
    // インスタンス取得
    IFieldActorBase *pBaseActor = reinterpret_cast<IFieldActorBase*>( pUserData );
    // 起動する
    IFieldActorBase::KickRequest kick_req;
    kick_req.pPlayer = reqData.p_player;
    kick_req.pActor  = dynamic_actor->GetCollisionResult(result_index)->pTargetActor;
    kick_req.pStaticModelManager = reqData.p_fieldmap->GetStaticModelManager();

    b32 result = pBaseActor->Kick( kick_req, &isEventCreate );

    // イベント起動した場合のみ。
    if( isEventCreate )
    {
      if( !isIntr )
      {
        // カメラサポート挙動停止
        if( pBaseActor->IsCameraSupportStop() )
        {
          reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
        }

        // カメラフォーカス機能のSTOP
        if( pBaseActor->IsCameraFocusStop() && reqData.p_fieldmap->GetCameraManager()->GetFocus() )
        {
          reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::FOCUS | Camera::Area::Focus::EventStopBit::SE );
        }
      }
      return RET_EVENT_NORMAL_EVENT;  // 通常起動
    }
    else if( result )
    {
      return RET_EVENT_NO_EVENT_ON;
    }
  }
  
  return RET_EVENT_NONE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベント POSイベント発動チェック
 *
 *  @param  reqData  リクエストデータ
 *
 *  @retval true  イベント起動 / false イベント起動なし
 */
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckPosEvent( const EventCheckRequest& reqData )
{
  DynamicActor* dynamic_actor = reqData.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_POS );
  return checkPosEventCore( reqData, dynamic_actor, 0, false );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベント つりPOSイベント発動チェック
 *
 *  @param  reqData  リクエストデータ
 *
 *  @retval true  イベント起動 / false イベント起動なし
 */
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckFishingPosEvent( const EventCheckRequest& reqData )
{
  DynamicActor* dynamic_actor = reqData.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_FISHING_POS );
  return checkPosEventCore( reqData, dynamic_actor, 0, true );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  イベント INTRPOSイベント発動チェック
 *
 *  @param  reqData  リクエストデータ
 *
 *  @retval true  イベント起動 / false イベント起動なし
 */
//-----------------------------------------------------------------------------
bool FieldEventCheck::CheckIntrPosEvent( const EventCheckRequest& reqData )
{
  DynamicActor* dynamic_actor = reqData.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_INTRPOS );
  if(dynamic_actor && dynamic_actor->GetCollisionResult(0)->pTargetActor )
  {
    void *pUserData = dynamic_actor->GetCollisionResult(0)->pTargetActor->GetUserData();
    bool isEventCreate = false;
    // インスタンス取得
    IFieldActorBase *pBaseActor = reinterpret_cast<IFieldActorBase*>( pUserData );
    // 起動する
    IFieldActorBase::KickRequest kick_req;
    kick_req.pPlayer = reqData.p_player;
    kick_req.pActor  = dynamic_actor->GetCollisionResult(0)->pTargetActor;
    kick_req.pStaticModelManager = reqData.p_fieldmap->GetStaticModelManager();

    pBaseActor->Kick( kick_req, &isEventCreate );

    GFL_ASSERT( isEventCreate == false );

    // IntrPosでイベントは起動しない。
    return false;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イベント 会話イベントチェック
 *
 * @param   reqData リクエストデータ
 *
 * @return  RET_EVENT_NORMAL_EVENT or RET_EVENT_NOT_CLEAR_INTEREST_EVENT イベント起動 / RET_EVENT_NONE イベント起動なし
 */
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckTalkEvent( const EventCheckRequest& reqData )
{
  if(reqData.ptalkActor){

    // 裏世界にいるかチェック
    Field::MoveModel::FieldMoveModel* pCharaModel = reqData.p_player;
    if( pCharaModel && !pCharaModel->CheckValidGround() )
    {
      return RET_EVENT_NONE;
    }
    
    void *pUserData = reqData.ptalkActor->GetUserData();
    bool isEventCreate = false;
    // インスタンス取得
    IFieldActorBase *pBaseActor = reinterpret_cast<IFieldActorBase*>( pUserData );
    // 起動する
    IFieldActorBase::KickRequest kick_req;
    kick_req.pPlayer = reqData.p_player;
    kick_req.pActor = reqData.ptalkActor;
    kick_req.pStaticModelManager = reqData.p_fieldmap->GetStaticModelManager();
    b32 result = pBaseActor->Kick( kick_req, &isEventCreate );

    // イベント起動した場合のみ。
    if( isEventCreate )
    {
      // カメラサポート挙動停止
      if( pBaseActor->IsCameraSupportStop() )
      {
        reqData.p_fieldmap->GetCameraManager()->SetSupportAction( Camera::SupportAction::STOP );
      }
      // カメラフォーカス機能のSTOP
      if( pBaseActor->IsCameraFocusStop() && reqData.p_fieldmap->GetCameraManager()->GetFocus() )
      {
        reqData.p_fieldmap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::FOCUS | Camera::Area::Focus::EventStopBit::SE );
      }

      if( !pBaseActor->IsActionButtonInterestClear() )
      {
        return RET_EVENT_NOT_CLEAR_INTEREST_EVENT;         // 注視クリアなし起動
      }
      return RET_EVENT_NORMAL_EVENT;  // 通常起動
    }
    // イベントは起動していないが　何かしら実行した場合
    else if( result )
    {
      return RET_EVENT_NO_EVENT_ON;
    }

  }
  return RET_EVENT_NONE;
}


//-----------------------------------------------------------------------------
/**
 * @brief   イベント Pushイベントチェック
 *
 * @param   reqData リクエストデータ
 *
 * @return  true  イベント起動 / false イベント起動なし
 */
//-----------------------------------------------------------------------------
FieldEventCheck::EventCheckRet FieldEventCheck::CheckPushEvent( const EventCheckRequest& reqData )
{
  DynamicActor* dynamic_actor = reqData.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_PUSH );

  // @fix GFNMCat[3544] ２つのPUSHイベントが重なっている場所で、片方が反応しない。
  for( int index=0; index<BaseActor::MAX_COLLISION_RESULT_CNT; ++index )
  {
    // @fix GFNMCat[519] PushイベントとPosイベントの処理内容を一致させました
    FieldEventCheck::EventCheckRet result = checkPosEventCore( reqData, dynamic_actor, index, false );
    if( result != FieldEventCheck::RET_EVENT_NONE )
    {
      return result;
    }
  }

  return RET_EVENT_NONE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  通常エンカウントチェック
 *
 *  @param  reqData リクエストデータ
 *
 *  @retval true  エンカウント起動
 *  @retval false エンカウントしない
 */
//-----------------------------------------------------------------------------
bool FieldEventCheck::CheckNormalEncountEvent( const EventCheckRequest& reqData, Attr attr )
{
  bool ret;

  // エンカウント制限ゾーンかチェック
  ZoneDataLoader* pZoneDataLoader = reqData.p_gameman->GetGameData()->GetFieldZoneDataLoader();
  const EventWork* pEventWork = reqData.p_gamedata->GetEventWork();
  if( pZoneDataLoader->CheckEncountNGZone( reqData.zone_id, pEventWork))
  {
    return false;
  }

  // 通常エンカウント
  ret = reqData.p_fieldmap->GetEncount()->CheckEncount( Encount::Encount::TYPE_NORMAL, reqData.encountDataID, attr );
  return ret;
}


//----------------------------------------------------------------------------
/**
 *  @brief  下画面タッチリクエストチェック
 *
 *  @param  pManager FieldSubScreenProcManager
 *  @retval true  起動
 *  @retval false リクエストは無い
 */
//-----------------------------------------------------------------------------

bool FieldEventCheck::CheckSubScreenReq( Field::SubScreen::FieldSubScreenProcManager* pManager)
{
  if( pManager )
  {
    return Field::SubScreen::CheckEvent( pManager );
  }
  return false;
}

  //----------------------------------------------------------------------------
  /**
   *  @brief  波乗り解除リクエストチェック
   *
   *  @param  
   *  @retval true  起動
   *  @retval false リクエストは無い
   */
  //-----------------------------------------------------------------------------
bool FieldEventCheck::CheckCancelNaminori( const EventCheckRequest& reqData )
{
// note: "自身の座標( GetCharaDrawInstance()->GetPosition() ) == 波乗りイベント用アクターの補正される前の座標"なので↓の処理がなりたつ。
//       この時点では波乗りイベント用アクターは補正された後の座標です
  MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<MoveModel::FieldMoveModelPlayer*>( reqData.p_player );
  if(  ( pPlayer->GetOnMoveModel() )
    && ( pPlayer->IsWalkLoopState() || pPlayer->IsRunLoopState() || pPlayer->IsWalkEndState() || pPlayer->IsRunEndState() )
    && ( pPlayer->GetOnMoveModel()->GetCharacterId() == CHARA_MODEL_ID_PM0319_00 || pPlayer->GetOnMoveModel()->GetCharacterId() == CHARA_MODEL_ID_PM0131_00 )
    && (EventPokemonRideOff::IsPokemonRideOffEnable( reqData.p_gameman ) == RCR_OK) )
  {
    // 自身の進行方向を取得
    gfl2::math::Vector3 vecZ(0.0f,0.0f,1.0f);
    vecZ = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform( vecZ );

    // 自身に触れているフィルターの法線
    gfl2::math::Vector3 adjustVec       ( pPlayer->GetCharaDrawInstance()->GetPosition() - pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetPosition() );
    gfl2::math::Vector3 adjustVecNormal ( adjustVec.Normalize() );

    // 陸地に向かって移動したら下ろす
    f32 THRESHOLD = 45.0f;
    f32 dot       = gfl2::math::Clamp( adjustVecNormal.Dot(vecZ), -1.0f , 1.0f );
    f32 angle     = gfl2::math::ConvRadToDeg( acosf( dot ) );

  #if PM_DEBUG
    Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
    if( pFieldDebugMenu )
    {
      THRESHOLD = pFieldDebugMenu->GetRideOffAngle();
    }
  #endif

    if( angle < THRESHOLD )
    {
      Field::EventPokemonRideOff* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOff>( reqData.p_evman );
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
  *  @brief  波乗り開始リクエストチェック
  *
  *  @param  
  *  @retval true  起動
  *  @retval false リクエストは無い
  * MCat[#255] システムフラグを参照するように対応
  */
//-----------------------------------------------------------------------------
bool FieldEventCheck::CheckStartNaminori( const EventCheckRequest& reqData )
{
  // プレイヤーが波乗りフィルターに触れているか
  MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<MoveModel::FieldMoveModelPlayer*>( reqData.p_player );
  if( !pPlayer || !pPlayer->IsHitNaminoriWall() )
  {
    return false;
  }

  // 既に海ライドしているかチェック
  if( pPlayer->GetOnMoveModel() )
  {
    RIDE_POKEMON_ID id = EventPokemonRideTool::GetCharactorId_to_RIDE_POKEMON_ID( pPlayer->GetOnMoveModel()->GetCharacterId() );
    if( EventPokemonRideTool::IsNaminori(id) )
    {
      return false;
    }
  }

  if( !reqData.p_gamedata->GetEventWork()->CheckEventFlag( SYS_FLAG_POKE_RIDE_OPEN ) )
  {
    return false;
  }

  // ラプラスが解放されているかチェック
  if( !reqData.p_gamedata->GetEventWork()->CheckEventFlag( SYS_FLAG_RAPURASU_RIDE_OPEN ) )
  {
    return false;
  }
  
  // 最後にのっていた海ライドに乗る
  Field::RIDE_POKEMON_ID pokemonID = Field::RIDE_LAPLACE;

  // サメハダーが解放されているかチェック
  if( reqData.p_gamedata->GetEventWork()->CheckEventFlag( SYS_FLAG_SAMEHADAA_RIDE_OPEN ) )
  {
    if( pPlayer->GetLastSeaRideCharacterId() == CHARA_MODEL_ID_PM0319_00 )
    {
      pokemonID = Field::RIDE_SAMEHADA;
    }
  }

  {
    RIDE_CHECK_RESULT rcr_result = EventPokemonRideOn::IsPokemonRideOnEnable( reqData.p_gameman,pokemonID );

    if( rcr_result == RCR_OK )
    {
      EventScriptCall::CallScript( reqData.p_gameman, SCRID_FLD_EV_RIDE_NAMINORI_GO, NULL, NULL, pokemonID );
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  QRスキャン時間経過チェック
 *  @retval true  起動
 *  @retval false リクエストは無い
 */
//-----------------------------------------------------------------------------
bool FieldEventCheck::CheckQRScanTimeout( const EventCheckRequest& reqDat )
{
  Savedata::QRReaderSaveData* pQRReaderSaveData = reqDat.p_gamedata->GetQRReaderSave();

  Savedata::QRReaderSaveData::FieldScanQueryInfo info;
  pQRReaderSaveData->FieldScanQuery( NULL, &info);

  if( info.status == Savedata::QRReaderSaveData::FIELDSCAN_STATUS_Timeout)
  {
    Field::EventWork* pEventWork = reqDat.p_gamedata->GetEventWork();
    pEventWork->ClearEventFlags( SYS_FLAG_POKE_SEARCH_SCAN_00, SYS_FLAG_POKE_SEARCH_SCAN_27);

    pQRReaderSaveData->InactivateFieldScan();

    EventScriptCall::CallScript( reqDat.p_gameman, SCRID_POKE_SEARCH_FINISH, NULL, NULL );

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  コリジョンの更新を行なう
 *  @param  reqData リクエストデータ
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::UpdateCollision(void )
{
  // コリジョンの更新
  {
    // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_EVENT_POS          );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_EVENT_INTRPOS      );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_EVENT_TALK         );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_EVENT_PUSH         );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_ENCOUNT            );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_TRAINER_EYE        );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_EVENT_NAMINORI     );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_TALK  );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_FUREAI_TALK  );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_TRAINER_EYE );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_TRAINER_EYE_NEW );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT                );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION         );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT         );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT_ACTION  );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_EVENT_FINDER          );
    m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_FISHING_POS          );


    m_pCollsionSceneForEventPos->Update();
    m_pCollsionSceneForEventIntrPos->Update();
    m_pCollsionSceneForEventTalk->Update();
    m_pCollsionSceneForEventPush->Update();
    m_pCollsionSceneForEncount->Update();
    m_pCollsionSceneForTrainerEye->Update();
    m_pCollsionSceneForEventNaminori->Update();
    m_pCollsionSceneForGimmickEncount->Update();
    m_pCollsionSceneForGimmickEncountAction->Update();
    m_pCollsionSceneForEventFinder->Update();
    m_pCollisionSceneForFishingPos->Update();

    // Talkイベントについて
    // このフレームで採用するヒット情報を取得
    this->SelectTalkTarget();

    // このフレームで採用するエンカウントデータIDを取得
    this->SelectEncountDataID();

    // このフレームで採用するPokeFinderポイントを取得
    this->SelectPokeFinder();


    // @fix NMCat[3014] 特定操作でイベントラインを踏まずにバス停が調べられ、イベントをスキップできる
    // ライドに乗った際に、POSにヒットする状態の場合は、会話系のイベントが起動できないようにフックを行なう。
    HookTalkEvent();
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  話しかけターゲットの決定
 *
 *  @param  reqData リクエストデータ
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::SelectTalkTarget( void )
{
  DynamicActor* dynamic_actor = m_Request.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_TALK );
  m_Request.ptalkActor = NULL;
  m_Request.isHitHiddenItem = false;
  if(dynamic_actor == NULL){ return ; }

  f32 length[BaseActor::MAX_COLLISION_RESULT_CNT];
  f32 cos[BaseActor::MAX_COLLISION_RESULT_CNT];
  u32 index = 0;
  f32 max_length = 0.0f;

  // 自機の位置と方向
  gfl2::math::Vector3 player_pos = m_Request.p_player->GetCharaDrawInstance()->GetPosition();
  gfl2::math::Matrix34 rot_mtx;
  m_Request.p_player->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix( rot_mtx );
  gfl2::math::Vector3 player_dir( 0.0f, 0.0f, 1.0f );
  player_dir = rot_mtx * player_dir;

  /*
  TOMOYA_PRINT( "player_pos(%f,%f,%f) dir(%f,%f,%f)\n", 
    player_pos.GetX(), player_pos.GetY(), player_pos.GetZ(),
    player_dir.GetX(), player_dir.GetY(), player_dir.GetZ() );
    */

  // 情報収集
  for( index=0; index<BaseActor::MAX_COLLISION_RESULT_CNT; ++index )
  {
    BaseActor* pActor;
    pActor = dynamic_actor->GetCollisionResult(index)->pTargetActor;
    if( pActor == NULL ){
      break;
    }

    IFieldActorBase * fieldActor = reinterpret_cast<IFieldActorBase *>(pActor->GetUserData());
    // アクターが無効なら除外
    if( fieldActor && fieldActor->IsInvalid() )
    {
      continue; // MCat[303]
    }


    // 注目点の取得
    gfl2::math::Vector3 actor_pos;
    {
      u32 target_event_id = 0xffffffff;
      u32 wait_count = 0;
      ActionButton::ActionButtonManager::ICON_FRAME icon_frame = ActionButton::ActionButtonManager::NONE;
      gfl2::math::Vector3 attention_pos(0,0,0);
      b32 attention = false;

      if( !m_Request.p_fieldmap->GetActionButtonManager()->GetActionButtonParam( pActor, &target_event_id, &wait_count, &icon_frame, &actor_pos, &attention_pos, &attention ) )
      {
        // false = 動作モデル GetPositionでOK
        actor_pos = pActor->GetPosition();
      }
    }
    actor_pos.SetY( player_pos.GetY() );

    // 注目点とプレイヤー座標の方向を取得
    gfl2::math::Vector3 dist = (actor_pos - player_pos);


    // BGイベントの場合
    // 方向のScore計算を行なわないケースがあるのでチェック
    bool is_skip_cos_calc = false;
    if( fieldActor && fieldActor->GetType() == FIELD_ACTOR_TYPE_BG )
    {
      FieldBgEventActor * pBgEventActor = reinterpret_cast<FieldBgEventActor*>(fieldActor);
      is_skip_cos_calc = m_Request.p_fieldmap->GetActionButtonManager()->IsAllRangeFocus( pBgEventActor->GetData()->attentionID );
    }
    else if(fieldActor && fieldActor->GetType() == FIELD_ACTOR_TYPE_MAPJUMP)
    {
      FieldMapJumpActor * pMapJumpEventActor = reinterpret_cast<FieldMapJumpActor*>(fieldActor);
      is_skip_cos_calc = m_Request.p_fieldmap->GetActionButtonManager()->IsAllRangeFocus( pMapJumpEventActor->GetData()->focus );
    }


    // 距離を求める
    length[index] = dist.Length();
    if( max_length < length[index] ){
      max_length = length[index];
    }

    // 内積を求める
    if( is_skip_cos_calc )
    {
      cos[index] = 1.0f;
    }
    else
    {
      dist /= length[index];  // Normal
      cos[index] = player_dir.Dot( dist );
                // (cos - cos(limit)) / 1 - cos(limit) = cos(limit)を最大値(1.0f)とした値にする。
      cos[index] = (cos[index] - gfl2::math::CosDeg(m_DebugTalkCheckCosLimit)) / (1.0f - gfl2::math::CosDeg(m_DebugTalkCheckCosLimit));
      cos[index] = gfl2::math::Clamp( cos[index], -1.0f, 1.0f );
    }
  }

  // 計算
  f32 score_max = 0;
  for( u32 i=0; i<index; ++i )
  {
    BaseActor* pActor = dynamic_actor->GetCollisionResult(i)->pTargetActor;
    IFieldActorBase * fieldActor = reinterpret_cast<IFieldActorBase *>(pActor->GetUserData());
    if( !fieldActor )
    {
      continue; // @fix cov_ctr[11249]NullPtrCheck
    }
    // アクターが無効なら除外
    if( fieldActor->IsInvalid() )
    {
      continue; // MCat[303]
    }

    f32 length_score = (max_length > 0.0f) ? 1 - (gfl2::math::Clamp((length[i] - SELECT_TALK_START_DIST), 0.0f, max_length) / max_length) : 0.0f;
    f32 cos_score    = cos[i];

    // どちらかがマイナススコアなら反応しない。
    if( !((length_score < 0.0f) || (cos_score < 0.0f)) ){

      if( fieldActor && fieldActor->GetType() == FIELD_ACTOR_TYPE_ITEM )
      {
        FieldItemActor* pItemActor = reinterpret_cast<FieldItemActor*>( fieldActor );

        // 隠しアイテムにHITしている
        if( FieldItemAccessor::IsHiddenShapeType( pItemActor->GetData()->shapeType ) )
        {
          m_Request.isHitHiddenItem = true;
        }
      }

      f32 score = length_score + cos_score;

      // 最大スコアのアクターが会話ターゲット
      if( score > score_max ){
        m_Request.ptalkActor = dynamic_actor->GetCollisionResult(i)->pTargetActor;
        score_max = score;  // スコア更新 BTS[6279] fix
      }

#if PM_DEBUG
#if defined( DEBUG_ONLY_FOR_takahashi_tomoya )
      TOMOYA_PRINT( "index:%d length[%f] cos[%f] score[%f] max_length[%f] actor_pos(%f,%f,%f)\n",
        i, length[i], cos[i], score, max_length, 
        pActor->GetPosition().GetX(), pActor->GetPosition().GetY(), pActor->GetPosition().GetZ() );
#endif //PM_DEBUG_PRINT
#endif 
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  エンカウントテーブルデータIDの決定
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::SelectEncountDataID( void )
{
  m_Request.encountDataID = CheckEncount( m_Request.p_player );
}


/**
 *  @brief エンカウントのコリジョンチェックを実行する。
 *  @note 次にCheckEncountを実行
 */
void FieldEventCheck::CalcCollisionEncount( MoveModel::FieldMoveModelManager& moveModelMan, BaseCollisionScene& collisionSceneEncount )
{
  moveModelMan.ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_ENCOUNT );
  collisionSceneEncount.Update();
}

/**
 *  @brief エンカウントするかチェックする。
 *  @note 必ず先にCalcCollisionEncountを実行
 *  @retval Encount::Encount::ENCOUNT_DATA_ID_ERROR エンカウントしない
 *  @retval それ以外  エンカウントする。
 */
u32 FieldEventCheck::CheckEncount( MoveModel::FieldMoveModel* p_player )
{
  // エラー値で初期化
  u32 encountDataID = Encount::Encount::ENCOUNT_DATA_ID_ERROR;
  u32 now_attr = p_player->GetGroundAttribute();


  // アトリビュートがエラー値なら終了
  if( now_attr == ATTR_ERROR)
  {
    return encountDataID;
  }

  // プレイヤーにエンカウントコリジョンアクターがなければ終了
  DynamicActor* dynamic_actor = p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_ENCOUNT );
  if(dynamic_actor == NULL)
  {
    return encountDataID; 
  }

  for( u32 i = 0; i < BaseActor::MAX_COLLISION_RESULT_CNT; ++i)
  {
    BaseActor* pHitActor = dynamic_actor->GetCollisionResult( i)->pTargetActor;

    // 当たっていなければ終了
    if( pHitActor == NULL)
    {
      break; 
    }
 
    void* pUserData = pHitActor->GetUserData();
    // 念のためユーザーデータをチェック
    if( pUserData == NULL)
    {
      GFL_ASSERT( 0); // ここにきたらActorでUserDataが設定されていない
      continue;
    }

    FieldEncountActor* pEncountActor = reinterpret_cast<FieldEncountActor*>( pUserData );

    // 足元アトリビュートからエンカウントできるかチェック
    if( pEncountActor->IsEncount( now_attr))
    {
      const FieldEncountAccessor::EncountData* pData = pEncountActor->GetData();
      encountDataID = pData->dataID;
      break;
    }
  }

  return encountDataID;
}



//----------------------------------------------------------------------------
/**
 *  @brief  ポケファインダーの決定
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::SelectPokeFinder( void )
{
  DynamicActor* dynamic_actor = m_Request.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_FINDER );
  m_Request.pFinderActor = NULL;

  BaseActor * pHitActor = dynamic_actor->GetCollisionResult(0)->pTargetActor;
  if( !pHitActor )
  {
    return ;
  }

  void* pUserData = pHitActor->GetUserData();
  // 念のためユーザーデータをチェック
  if( pUserData == NULL)
  {
    GFL_ASSERT( 0); // ここにきたらActorでUserDataが設定されていない
    return ;
  }

  FieldPokeFinderActor* pActor = reinterpret_cast<FieldPokeFinderActor*>( pUserData );
  m_Request.pFinderActor = pActor;
}

//----------------------------------------------------------------------------
/**
*  @brief  NPCの注目を停止
*/
//-----------------------------------------------------------------------------
void FieldEventCheck::StopNpcInterest( void )
{
  for( u32 i = 0; i < PlacementDataManager::MOVEMODEL_EVENT_ACTOR_MAX; i++ )
  {
    FieldMoveModelActor* pWorkActor(  
      reinterpret_cast<Field::FieldMoveModelActor*>( m_Request.p_gameman->GetFieldResident()->GetPlacementDataManager()->GetMoveModelActor( i ) ));

    if( pWorkActor == NULL || !pWorkActor->IsSetupMoveModel() ) // @note IsRegistでは、非表示状態の動作モデルが省かれてしまう
    {
      continue;
    }

    if( pWorkActor->IsNpc() )
    {
      pWorkActor->StopNpcInterest();
    }
#if 0 // @note NPCと違い再利用される事がないので、一旦そのまま
    if (pWorkActor->IsContactPokemon())
    {
      pWorkActor->StopContactInterest();
    }
#endif
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  "なみのり"のコリジョンの更新を行なう
 *  @fix GFNMCat[3882] 追加対処
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::UpdateCollisionNaminori(void)
{
  m_Request.p_fieldmap->GetMoveModelManager()->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_EVENT_NAMINORI     );
  m_pCollsionSceneForEventNaminori->Update();
}


//----------------------------------------------------------------------------
/**
 *  @brief 会話イベントHookのチェック
 */
//-----------------------------------------------------------------------------
void FieldEventCheck::HookTalkEvent(void)
{
  b32 is_hook = false;
  DynamicActor* dynamic_actor = NULL;
  
  dynamic_actor = m_Request.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_POS );
  if( dynamic_actor )
  {
    if( IsHitCollisionResult( dynamic_actor, 0 ) )
    {
      is_hook = true;
    }
  }

  dynamic_actor = m_Request.p_player->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_PUSH );
  if( dynamic_actor )
  {
    for( int index=0; index<BaseActor::MAX_COLLISION_RESULT_CNT; ++index )
    {
      if( IsHitCollisionResult( dynamic_actor, index ) )
      {
        is_hook = true;
      }
    }
  }

  // Hook条件にヒットしていたら会話イベント起動をHook
  if( is_hook )
  {
    m_Request.talkRequest = false;
    m_Request.ptalkActor = NULL;
  }
}

/**
 *  @brief dynamic_actorにindex番目のヒット情報があるかをチェック
 */
b32 FieldEventCheck::IsHitCollisionResult( DynamicActor* dynamic_actor, int result_index )
{
  if(dynamic_actor && dynamic_actor->GetCollisionResult(result_index)->pTargetActor )
  {
    void *pUserData = dynamic_actor->GetCollisionResult(result_index)->pTargetActor->GetUserData();
    // インスタンス取得
    IFieldActorBase *pBaseActor = reinterpret_cast<IFieldActorBase*>( pUserData );

    // @fix GFNMCat[5059]ActionEventは無視
    if(pBaseActor->GetType() != FIELD_ACTOR_TYPE_ACTION)
    {
      // 無効状態？
      if( !pBaseActor->IsInvalid() )
      {
        return true;
      }
    }

  }

  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  ロトムパワーチェック
*  @retval true  終了した
*  @retval false 使用中または未使用
*/
//-----------------------------------------------------------------------------
bool FieldEventCheck::CheckRotomPowerTimeout( const EventCheckRequest& reqData )
{
  RotomPower::RotomPowerManager *pRotomPowerManager = reqData.p_gameman->GetRotomPowerManager();
  u32 endRotomPowerID = pRotomPowerManager->GetEndRotomPowerID();
  // ロトムパワー終了
  if (endRotomPowerID > RotomPower::ROTOM_POWER_ID_NULL)
  {
    u32 itemNo = ITEM_TAMAGOHUKAPON + endRotomPowerID - 1;
    pRotomPowerManager->ClearEndRotomPowerID();
    EventScriptCall::CallScript(reqData.p_gameman, SCRID_ROTOPON_FINISH, NULL, NULL, itemNo);
    return true;
  }

  return false;
}


} // namespace field

