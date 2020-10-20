//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventEntranceIn.h
 *	@brief  エントランスアウト
 *	@author	tomoya takahashi
 *	@date		2015.05.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/Event/FieldEventEntranceIn.h"

#include "GameSys/include/GameEventManager.h"
#include "Sound/include/Sound.h"

#include "Field/FieldRo/include/PlacementData/PlacementMapJumpData.h"
#include "niji_conv_header/field/mapjump/FieldMapJumpType.h"
#include "niji_conv_header/field/chara/p2_base_fi.h"


#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaSupport.h"

#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif // PM_DEBUG


#include <Fade/include/gfl2_FadeManager.h>

GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
const FADE_TYPE EventEntranceIn::sFadeType[MC_FIELD_MAX][MC_FIELD_MAX] = {
  { FADE_TYPE_BLACK,  FADE_TYPE_BLACK,  FADE_TYPE_BALL,   FADE_TYPE_DIA,    FADE_TYPE_CIRCLE,},
  { FADE_TYPE_CIRCLE, FADE_TYPE_BLACK,  FADE_TYPE_BALL,   FADE_TYPE_DIA,    FADE_TYPE_CIRCLE,},
  { FADE_TYPE_BLACK,  FADE_TYPE_BLACK,  FADE_TYPE_BLACK,  FADE_TYPE_DIA,    FADE_TYPE_CIRCLE,},
  { FADE_TYPE_BLACK,  FADE_TYPE_BLACK,  FADE_TYPE_BALL,   FADE_TYPE_BLACK,  FADE_TYPE_CIRCLE,},
  { FADE_TYPE_BLACK,  FADE_TYPE_BLACK,  FADE_TYPE_BALL,   FADE_TYPE_DIA,    FADE_TYPE_BLACK,},
};

const u32 EventEntranceIn::sFadeTime[MC_FIELD_MAX][MC_FIELD_MAX] =
{
  { 8,   30,  40,  40,  40,},
  { 40,   2,  40,  40,  50,},
  { 8,   10,   8,  40,  40,},
  { 8,  10,  40,   8,  40,},
  { 8,  20,  40,  40,   10,},
};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

/**
 *  @brief デスクリプション初期化
 */
void EventEntranceIn::InitializeDesc( Desc* p_desc )
{
  p_desc->mapJumpType = MAPJUMP_TYPE_MAT;
  p_desc->playSE      = PlacementMapJumpData::SE_LABEL_NULL;
  p_desc->eventID     = 0;
  p_desc->nowZoneID   = ZONEID_ERROR;
  p_desc->nextZoneID  = ZONEID_ERROR;
  p_desc->targetPos   = gfl2::math::Vector3(0,0,0);
  p_desc->quaternion  = gfl2::math::Quaternion(0,0,0,1);
  p_desc->isExistenceCameraAnime = false; // カメラアニメがあるかどうか
  p_desc->cameraAnimeIndex = MJ_CP_NONE;  ///< カメラアニメーションのインデックス
  p_desc->connectCenterPosition = gfl2::math::Vector3(0,0,0);
}


//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventEntranceIn::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventEntranceIn::InitFunc(GameSys::GameManager* gmgr)
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventEntranceIn::MainFunc(GameSys::GameManager* gmgr)
{
  Fieldmap* p_fieldmap = gmgr->GetFieldmap();
  enum
  {
    SEQ_EVENT_CALL,
    SEQ_END
  };

  if (!p_fieldmap->IsReady()) {
    return GameSys::GMEVENT_RES_CONTINUE;
  }

  switch (this->GetSeqNo()) {
  case SEQ_EVENT_CALL:
    CallEntranceIn(gmgr, m_Data.mapJumpType);
    this->AddSeqNo();
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;

}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventEntranceIn::EndFunc(GameSys::GameManager* gmgr)
{
}

/**
 * @brief データをセットする
 * @param param パラメーター
 */
void EventEntranceIn::SetData(const EventEntranceIn::Desc& param)
{
  m_Data = param;

}

//==========================================================================
// private
//==========================================================================

/**
 * @brief エントランス・アウトを呼び出す
 * @param p_gman ゲームマネージャークラス
 * @param idx アタイプ
 */
void EventEntranceIn::CallEntranceIn(GameSys::GameManager* p_gman, u32 mapJumpType )
{

  switch( mapJumpType ){
  case MAPJUMP_TYPE_DOOR:
    {
      //演出用イベントの起動
      EventEntranceInPerformanceDOOR* p_event;
      // イベントをコール
      p_event = reinterpret_cast<EventEntranceInPerformanceDOOR*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceInPerformanceDOOR > (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;

  case MAPJUMP_TYPE_MAT:
  case MAPJUMP_TYPE_MAT_C:
    {
      //演出用イベントの起動
      EventEntranceInPerformanceMAT* p_event;
      // イベントをコール
      p_event = reinterpret_cast<EventEntranceInPerformanceMAT*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceInPerformanceMAT > (p_gman->GetGameEventManager()));
      
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;

  default:
  case MAPJUMP_TYPE_NORMAL:
  case MAPJUMP_TYPE_NORMAL_C:
  case MAPJUMP_TYPE_NORMAL_OUT:
  case MAPJUMP_TYPE_NORMAL_OUT_C:
  case MAPJUMP_TYPE_NORMAL_WATER:
  case MAPJUMP_TYPE_NORMAL_WATER_C:
    {
      //演出用イベントの起動
      EventEntranceInPerformanceNORMAL* p_event;
      // イベントをコール
      p_event = reinterpret_cast<EventEntranceInPerformanceNORMAL*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceInPerformanceNORMAL > (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;

  case MAPJUMP_TYPE_LADDER_UP:
    {
      //演出用イベントの起動
      EventEntranceInPerformanceLadderUp* p_event;
      // イベントをコール
      p_event = reinterpret_cast<EventEntranceInPerformanceLadderUp*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceInPerformanceLadderUp> (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;
    
  case MAPJUMP_TYPE_LADDER_DOWN:
    {
      //演出用イベントの起動
      EventEntranceInPerformanceLadderDown* p_event;
      // イベントをコール
      p_event = reinterpret_cast<EventEntranceInPerformanceLadderDown*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceInPerformanceLadderDown> (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);

    }
    break;

  case MAPJUMP_TYPE_THROUGH_A_GATE:
    {
      //演出用イベントの起動
      EventEntranceInPerformanceThroughAGate* p_event;
      // イベントをコール
      p_event = reinterpret_cast<EventEntranceInPerformanceThroughAGate*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceInPerformanceThroughAGate> (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);

    }
    break;

  }
}






//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの演出イベントのベースクラス 
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/**
 * @brief イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param p_gman ゲームマネージャークラス
 * @retval true 起動してよい
 * @retval false 起動しない
 */
bool EventEntranceInPerformanceBase::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
}

/**
 * @brief bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param p_gman ゲームマネージャークラス
 */
void EventEntranceInPerformanceBase::InitFunc(GameSys::GameManager* p_gman)
{
  m_pPlayer = p_gman->GetFieldmap()->GetPlayerCharacter(); 
  m_pStaticModelManager = p_gman->GetFieldmap()->GetStaticModelManager();

  m_Seq = SEQ_PERFORMANCE;

  // @fix BTS[2219] 準備処理はフェードイン前に行うようにした
  this->InitProcess();

  // カメラ初期化
  this->SetupCameraProcess( p_gman );
}

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param p_gman ゲームマネージャークラス
 * @return GMEVENT_RESULT イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventEntranceInPerformanceBase::MainFunc(GameSys::GameManager* p_gman)
{
  switch (m_Seq) {
  case SEQ_FADE:
  {
    bool isFade = true;
    //u8 exitType = m_Data.mapJumpType;
    /*if (exitType == EXIT_TYPE_MAT_NOT_FADE_IN ||
      exitType == EXIT_TYPE_STAIRS_NOT_FADE_IN) {
      isFade = false;
    }*/

    if (isFade) {
      // Fade処理
      FadeProcess( p_gman->GetGameData()->GetFieldZoneDataLoader() );
    }

    if( (m_Data.playSE != PlacementMapJumpData::SE_LABEL_NULL) ){
      Sound::PlaySE( m_Data.playSE );
    }

    ++m_Seq;
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;
  }

  case SEQ_PERFORMANCE:
  {
    if( m_FadeCaptureWait > 0 ){ m_FadeCaptureWait--; }// @fix GFNMCat[1226]

    // カメラ
    CAMERA_PROC_RET camera_ret = CameraProcess();

    if( camera_ret != CAMERA_PROC_RET_CONTINUE )
    {
      // 演出処理
      EventEntranceInPerformanceBase::RET ret = PerformanceProcess( p_gman );

      // 終了チェック
      if ((camera_ret==CAMERA_PROC_RET_FINISH) && (ret == RET_FINISH) && (m_FadeCaptureWait == 0)) // @fix GFNMCat[1226]
      {
        // プレイヤーがアクションコマンド実行中であればクリア
        if( m_pPlayer->IsExecuteActionCommand() )
        {
          m_pPlayer->ClearActionCommand(false);
        }

        EndCameraProcess();
        return GameSys::GMEVENT_RES_FINISH;
      } else if (ret == RET_CONTINUE) {
        return GameSys::GMEVENT_RES_CONTINUE;
      } else if (ret == RET_CONTINUE_DIRECT) {
        return GameSys::GMEVENT_RES_CONTINUE_DIRECT;
      } else {
        return GameSys::GMEVENT_RES_CONTINUE;
      }
    }
  }
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param p_gman ゲームマネージャークラス
 */
void EventEntranceInPerformanceBase::EndFunc(GameSys::GameManager* /*p_gman*/)
{
}

/**
 * @brief データをセットする
 * @param param パラメーター
 */
void EventEntranceInPerformanceBase::SetData(const EventEntranceIn::Desc & param)
{
  m_Data = param;
}

/**
 * @brief フェード処理
 */
void EventEntranceInPerformanceBase::FadeProcess(ZoneDataLoader* zoneDataLoader)
{
  gfl2::Fade::FADE_TYPE fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;

  // 現ZONE　と　次のZONEからフェードを決定
  void * pZoneData = zoneDataLoader->GetZoneData( m_Data.nowZoneID );
  ZoneDataAccessor nowZoneDataAccessor( pZoneData);

  pZoneData = zoneDataLoader->GetZoneData( m_Data.nextZoneID );
  ZoneDataAccessor nextZoneDataAccessor( pZoneData);


  FADE_TYPE type = EventEntranceIn::sFadeType[nowZoneDataAccessor.GetMapChangeType()][nextZoneDataAccessor.GetMapChangeType()];
  u32 sync = EventEntranceIn::sFadeTime[nowZoneDataAccessor.GetMapChangeType()][nextZoneDataAccessor.GetMapChangeType()];
  gfl2::math::Vector4 start_col(0,0,0,0), end_col(0,0,0,255);

  switch (type) {
  case FADE_TYPE_BLACK:
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    m_IsFadeEnd = false;
    break;
  case FADE_TYPE_WHITE:
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    start_col.Set(255,255,255,0);
    end_col.Set(255,255,255,255);
    m_IsFadeEnd = false;
    break;
  case FADE_TYPE_CIRCLE:
    fadeInType = gfl2::Fade::FADE_TYPE_CIRCLE;
    m_IsFadeEnd = false;
    break;
  case FADE_TYPE_BALL:
    fadeInType = gfl2::Fade::FADE_TYPE_BALL;
    m_IsFadeEnd = false;
    break;
  case FADE_TYPE_DIR:
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    m_IsFadeEnd = true;
    break;
  case FADE_TYPE_DIA:
    fadeInType = gfl2::Fade::FADE_TYPE_DIAMOND;
    m_IsFadeEnd = false;
    break;
  }

#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ENTRANCE_FADE_CUT ) )
  {
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    start_col.Set(0,0,0,0);
    end_col.Set(0,0,0,255);
    m_IsFadeEnd = true;
    sync = 1;
  }
#endif


  gfl2::Fade::FadeManager * pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  pFadeManager->RequestOut(gfl2::Fade::DISP_UPPER, fadeInType, &start_col, &end_col, sync,!m_IsFadeEnd);

  //@fix 増田さん要望 : レイアウトフェードのアニメーションスピードを1/2にする。
  if( (type == FADE_TYPE_CIRCLE) ||
      (type == FADE_TYPE_BALL) ||
      (type == FADE_TYPE_DIA)  )
  {
    const f32 LAYOUT_FADE_TIME = 20.0f;

    pFadeManager->SetAnimeSpeed( gfl2::Fade::DISP_UPPER,  LAYOUT_FADE_TIME / static_cast<f32>(sync) );
  }

  m_FadeCaptureWait = 2;  // @fix GFNMCat[1226]   エントランスカメラのついた接続ポイントで、接続イベントを発生させると、接続演出時にエントランスカメラが解除されてしまう。
}

/**
 *  @brief フェード終了待ち
 */
b32 EventEntranceInPerformanceBase::IsFadeEnd(void) const
{
  if(m_IsFadeEnd)
  {
    return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_UPPER);
  }
  return true;
}


/**
 *  @brief カメラパラメータの設定
 */
void EventEntranceInPerformanceBase::SetupCameraProcess(GameSys::GameManager* p_gman)
{
  // 演出データからカメラのセットアップパラメータ取得
  if( m_Data.isExistenceCameraAnime )
  {
    if( m_Data.cameraAnimeIndex != MJ_CP_NONE )
    {
      EventEntranceCameraParameter params;
      params.Initialize( p_gman->GetFieldResident()->GetResource( BL_IDX_FIELD_RESIDENT__MAPJUMPCAMERAPARAMETER_BIN ), p_gman->GetFieldResident()->GetResourceSize( BL_IDX_FIELD_RESIDENT__MAPJUMPCAMERAPARAMETER_BIN ) );
      m_CameraParameter = params.GetParam( m_Data.cameraAnimeIndex );
      m_IsCameraAnimation = true;

      m_pCameraManager = p_gman->GetFieldmap()->GetCameraManager();  
      m_pCameraHeap    = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

      const Camera::Area::Support * cpSupport = p_gman->GetFieldmap()->GetCameraManager()->GetSupport();
      if( cpSupport->IsDistanceSupport() )
      {
        m_CameraSupportValue = cpSupport->GetValue();
      }
      
      m_CameraPlayWait = m_CameraParameter.play_start_frame * m_CameraSupportValue;
    }
  }
}

/**
 * @brief カメラ処理
 */
EventEntranceInPerformanceBase::CAMERA_PROC_RET EventEntranceInPerformanceBase::CameraProcess(void)
{
  if( m_IsCameraAnimation == false )
  {
    return CAMERA_PROC_RET_FINISH;
  }

  enum
  {
    STATE_EVENT_CAMERA_START,
    STATE_WAIT_PLAY,
    STATE_WAIT_CAMERA,
    STATE_END,
  };

  switch( m_CameraSeq )
  {
  case STATE_EVENT_CAMERA_START:
    {
      // FieldUnitの基本パラメーターをEventUnitにコピー
      // イベントスクリプト用コントローラー内部で値を使用する可能性があるので先に行う
      m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT )->CopyBaseParam(
        *m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )
        );
      
      // 使用カメラをField->Eventに変更( Fieldはスリープ状態になる )
      m_pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::EVENT );
      
      // イベントスクリプト用コントローラーを生成
      // EventUnitはポケファインダーなどで使いまわすので、毎回使用開始時点で作ります
      // downcastせずアクセスする為に、作成したコントローラーはScriptWorkなどで保持してください
      {
        Camera::Controller::ControllerTypeEventScript::InitializeDescription initDesc;
        initDesc.dummy = 0; // 引数はない
        m_pCameraController = GFL_NEW( m_pCameraHeap )Camera::Controller::ControllerTypeEventScript( initDesc );
      }

      // 操作コントローラーをイベントスクリプト用コントローラに変更
      {
        // ↓ChangeMainCameraByUnitNo()で指定したカメラが返ります
        m_pCameraUnit = m_pCameraManager->GetMainGamePlayCamera();

        // ↓描画に使うUnitを上書き設定している場合は、違うUnitが返ります、用途はデバッグ用で基本はGetMainGamePlayCameraと同じ
        // Camera::CameraUnit* pCameraUnit = pCameraManager->GetMainViewCamera();
        
        // この時点でフィールドシステムが更新してくれるようになります
        m_pCameraUnit->ChangeController( m_pCameraController );
      }


      // アニメーション開始
      {
        if( m_CameraParameter.type == EventEntranceCameraParameter::TYPE_TARGET )
        {
          gfl2::math::Vector3 camera_pos;
          gfl2::math::Vector3 target_pos;

          EventEntranceCameraParameter::GetTypeTarget_CameraPos( m_CameraParameter, &camera_pos );
          EventEntranceCameraParameter::GetTypeTarget_TargetPos( m_CameraParameter, &target_pos );

          camera_pos += m_Data.connectCenterPosition;
          target_pos += m_Data.connectCenterPosition;

          m_pCameraController->Move(
            camera_pos,
            target_pos,
            m_CameraParameter.frame * m_CameraSupportValue,
            static_cast<gfl2::math::Easing::EaseFunc>(m_CameraParameter.easing),
            m_CameraParameter.bank 
            );
        }
        else if( m_CameraParameter.type == EventEntranceCameraParameter::TYPE_ROTATE )
        {
          gfl2::math::Vector3 target_pos;
          gfl2::math::Vector3 rotate;
          f32 distance = 0;
          EventEntranceCameraParameter::GetTypeRotate_TargetPos( m_CameraParameter, &target_pos );
          target_pos += m_Data.connectCenterPosition;

          EventEntranceCameraParameter::GetTypeRotate_Rotate( m_CameraParameter, &rotate );
          distance = EventEntranceCameraParameter::GetTypeRotate_Distance( m_CameraParameter );

          m_pCameraController->Move(
            target_pos,
            rotate,
            distance,
            m_CameraParameter.frame  * m_CameraSupportValue,
            static_cast<gfl2::math::Easing::EaseFunc>(m_CameraParameter.easing)
            );
        }
        m_CameraSeq ++;
      }
    }
    return CAMERA_PROC_RET_CONTINUE;

  case STATE_WAIT_PLAY:
    {
      if( m_CameraPlayWait > 0 )
      {
        m_CameraPlayWait--;
      }
      else
      {
        m_CameraSeq ++;
      }
    }
    return CAMERA_PROC_RET_CONTINUE;

  case STATE_WAIT_CAMERA:
    {
     // カメラ動作完了を待つ)
     if( m_pCameraController->IsMoving() == false )
     {
       m_CameraSeq ++;
     }

    }
    return CAMERA_PROC_RET_CONTINUE_DO_PROCESS;

  case STATE_END:
    return CAMERA_PROC_RET_FINISH;

  default:
    GFL_ASSERT(0);
    break;
  }
  
  return CAMERA_PROC_RET_FINISH;
}



/**
 * @brief カメラ終了
 */
void EventEntranceInPerformanceBase::EndCameraProcess()
{
  // カメラを元に戻す
  if( m_pCameraManager )
  {
    // コントローラーを解除
    m_pCameraUnit->ChangeController( NULL );  // 内部でメモリ破棄してくれる　
    m_pCameraController = NULL;
    m_pCameraUnit = NULL;
     
    // 使用カメラをEvent->Fieldに変更( Eventはスリープ状態になる )
    m_pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );
    m_pCameraManager = NULL;
  }

}


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの通常演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/**
 * @brief 演出処理
 */
EventEntranceInPerformanceBase::RET EventEntranceInPerformanceNORMAL::PerformanceProcess(GameSys::GameManager* /*p_gman*/)
{
  switch (m_PerformSeq) {
  case 0:
  {
    walkCounter = 0;
    m_fadeCounter = FADE_START_WAIT;

    // 向きを固定する。
    gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
    srt.rotation = m_Data.quaternion * gfl2::math::Quaternion(gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(180));
    m_pPlayer->SetLocalSRT( srt );

    // 1歩歩く
    m_pPlayer->SetActionCommandStepMove( (MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE),true,1,true,1.0f );
    m_PerformSeq++;
  }
    //break; through
  case 1:
  {
    if( m_fadeCounter >= 0 )
    {
      m_fadeCounter --;
      if( m_fadeCounter == 0 )
      {
        FadeStart();
      }
    }

    walkCounter++;
    if ( (m_fadeCounter < 0) && (walkCounter >= WALK_WAIT_TIME) && this->IsFadeEnd() ) {
      m_pPlayer->ClearActionCommand(false);

      m_PerformSeq++;
      return EventEntranceInPerformanceBase::RET_FINISH;
    }
  }
    break;

  case 2:
    return EventEntranceInPerformanceBase::RET_FINISH;
  }
  return EventEntranceInPerformanceBase::RET_CONTINUE;
}

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのドア演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/**
 * @brief 演出処理
 */
EventEntranceInPerformanceBase::RET EventEntranceInPerformanceDOOR::PerformanceProcess( GameSys::GameManager* /*p_gman*/ )
{
  switch (m_PerformSeq) {
  case 0:
  {
    walkCounter = 0;

    m_pDoorModel = m_pStaticModelManager->GetEventIDStaticModel( m_Data.eventID );

    // ドアの方を向く。
    gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
    if( m_pDoorModel && (m_pDoorModel->GetBaseModel(0)->GetJointCount() > 0) )
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = m_pDoorModel->GetBaseModel(0)->GetModelInstanceNode();
      s32 jointIndex = -1;
      s32 strlen;
      const char* pivotName = "PivotPos";
      u32 pivotNameLen = 8; // \0なし
      for( u32 i=0; i<m_pDoorModel->GetBaseModel(0)->GetJointCount(); ++i )
      {
        const char * cp_name = pModelInstance->GetJointInstanceNode(i)->GetName();

        TOMOYA_PRINT( "door joint name %s\n", cp_name );

        strlen = std::strlen( cp_name );

        if( strlen >= pivotNameLen )
        {
          TOMOYA_PRINT( "  %s\n", &cp_name[strlen - pivotNameLen] );
          if( std::strncmp( &cp_name[strlen - pivotNameLen], pivotName, pivotNameLen ) == 0 )
          {
            jointIndex = i;
          }
        }
      }

      if( jointIndex != -1 )
      {
        // 0版
        gfl2::math::Vector3 jointPos = m_pDoorModel->GetBaseModel(0)->GetJointWorldPosition(jointIndex);

        // その方向を向く。
        jointPos = jointPos - srt.translate;
        jointPos.y = 0.0f;
        if( jointPos.LengthSq() > 1.0f )
        {
          gfl2::math::Vector3 kijun(0,0,1);
          gfl2::math::Vector3 now_dir(0,0,1);

          now_dir = srt.rotation.Transform( now_dir );  // 今の方向

          jointPos = jointPos.Normalize();
          f32 rot_rad = gfl2::math::AcosRad( kijun.Dot(jointPos) );
          gfl2::math::Vector3 cross_vec = kijun.Cross(jointPos);

          // @fix GFNMCat[1705]ポケモンセンターからの出入りを素早く行おうとすると、扉を開ける際、自機が非表示になる類似
          if( !gfl2::math::IsAlmostZero( cross_vec.LengthSq() ) )
          {
            cross_vec = cross_vec.Normalize();
            srt.rotation = gfl2::math::Quaternion( cross_vec, rot_rad );  // 向くべき方向
          }
          else
          {
            srt.rotation = gfl2::math::Quaternion( gfl2::math::Vector3(0,1,0), rot_rad );  // 向くべき方向
          }

          f32 diff_deg = gfl2::math::AcosDeg( now_dir.Dot(jointPos) );  // 方向の差

          static f32 DEGREE = 5.0f;
          static u32 FRAME = 6;

          if( cross_vec.y < 0.0f )
          {
            rot_rad = -rot_rad;
          }

          TOMOYA_PRINT( "jointWay[%f,%f,%f] diff_deg %f rot_deg %f cross_y %f\n", jointPos[0], jointPos[1], jointPos[2], diff_deg, GFL_MATH_RAD_TO_DEG(rot_rad), cross_vec.y);

          // 振り向き挙動開始。
          if( diff_deg <= DEGREE )
          {
            m_pPlayer->SetLocalSRT( srt );
          }
          else
          {
            // 大森さん要望：ドアを開ける演出のレスポンスを向上させるため、回転中に手をあげるフローに変更
            // 完了後に待機モーションに移行しないモードで回転を実行
            m_pPlayer->SetActionCommandRotation( MoveModel::FIELD_ACTION_COMMAND_ROTATION, GFL_MATH_RAD_TO_DEG(rot_rad), 0, 0, FRAME, false );
          }
        }
      }
    }

    walkCounter = 0;
    m_PerformSeq++;
  }
  // break  through

  case 1:
    {
      b32 isRotationEnd( true );

      // 回転コマンドの完了判定
      if( m_pPlayer->IsExecuteActionCommand() )
      {
        isRotationEnd = false;
      }

      // 手をあげるタイミング判定
      b32 startHandAnimeFlag( false );
      static u32 HAND_ANIME_WAIT = 2;
      walkCounter++;
      if( walkCounter == HAND_ANIME_WAIT || ( isRotationEnd && walkCounter < HAND_ANIME_WAIT ) )
      {
        // 回転中の特定のフレーム + そのフレームに到達していない場合は回転完了時
        startHandAnimeFlag = true;
      }

      // 手をあげるアニメを再生
      if( startHandAnimeFlag )
      {
        Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(m_pPlayer);
        if( !pPlayer->GetOnMoveModel() )
        {
          m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI040_DOOR01 );
        }
      }

      // 回転終了待ち
      if( isRotationEnd == false )
      {
        break;
      }

      walkCounter = 0;
      m_PerformSeq ++;
    }
    //break; through

  case 2:
  {
    static u32 DOOR_ANIME_WAIT = 7;
    walkCounter++;
    if( walkCounter >= DOOR_ANIME_WAIT ){
      // ドアをアニメさせる
      if( m_pDoorModel ){
        StaticModel::StaticModel::Handle_StartDoorOpenAnimation handle;
        m_pDoorModel->Set( &handle );
      }

      m_fadeCounter = FADE_START_WAIT;
      m_PerformSeq ++;
    }
  }
    break;

  case 3:
    {
      m_fadeCounter --;
      if( m_fadeCounter <= 0 )
      {
        FadeStart();
        m_PerformSeq ++;
      }
    }
    break;

  case 4:
  {
    if (IsFadeEnd()) {
      m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( m_pPlayer->GetDefaultIdleAnimationId() );
      Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(m_pPlayer);
      m_PerformSeq++;
      return EventEntranceInPerformanceBase::RET_FINISH;
    }
  }
  break;

  case 5:
    return EventEntranceInPerformanceBase::RET_FINISH;
  }
  return EventEntranceInPerformanceBase::RET_CONTINUE;
}


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのMAT演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/**
 * @brief 演出処理
 */
EventEntranceInPerformanceBase::RET EventEntranceInPerformanceMAT::PerformanceProcess( GameSys::GameManager* /*p_gman*/ )
{
  switch (m_PerformSeq) {
  case 0:
  {
    walkCounter = 0;

    gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
    srt.rotation = m_Data.quaternion * gfl2::math::Quaternion(gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(180));
    m_pPlayer->SetLocalSRT( srt );

    m_fadeCounter = FADE_START_WAIT;

    m_PerformSeq++;
  }
    //break; through
  case 1:
  {
    if( m_fadeCounter >= 0 )
    {
      m_fadeCounter --;
      if( m_fadeCounter == 0 )
      {
        // フェード開始
        FadeStart();
      }
    }

    walkCounter++;
    if ( (m_fadeCounter < 0) && (walkCounter >= WALK_WAIT_TIME) && this->IsFadeEnd() ) {
      m_PerformSeq++;
      return EventEntranceInPerformanceBase::RET_FINISH;
    }
  }
    break;


  case 2:
    return EventEntranceInPerformanceBase::RET_FINISH;
  }
  return EventEntranceInPerformanceBase::RET_CONTINUE;
}


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief 吸着後アニメーションを再生する演出ベース
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief 演出処理
 */
EventEntranceInPerformanceBase::RET EventEntranceInPerformanceTargetMoveAndAnimationBase::PerformanceProcess( GameSys::GameManager* p_gman )
{
  enum
  {
    RIDE_OFF,         ///< ライドからおろす
    TARGET_MOVE,      ///< 吸着
    ANIMATION_START,  ///< アニメーション再生開始
    FADE_WAIT,        ///< フェード待ち
    END,
  };
  
  switch (m_PerformSeq) {

  case RIDE_OFF:         ///< ライドからおろす
    // ライドからおろすイベントの生成
    if( static_cast<MoveModel::FieldMoveModelPlayer*>(m_pPlayer)->IsRideMoveModel() )
    {
      Field::EventPokemonRideOff* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOff>( p_gman->GetGameEventManager() );
      m_PerformSeq++;
      break;
    }
    else
    {
      m_PerformSeq++;
    }
    //break;


  case TARGET_MOVE:      ///< 吸着
    {
      gfl2::math::Vector3 pos;
      gfl2::math::Quaternion qua;

      this->GetPos(&pos);
      this->GetQua(&qua);

      // 吸着
      //m_pPlayer->SetActionCommandTargetMove( MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, true, pos, qua );
      //一定距離以下だったら、モーションしながら吸着
      m_pPlayer->SetActionCommandTargetMoveWithOptionalMotion(Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION, this->GetAnimationNo(), pos, qua );

      m_fadeCounter = FADE_START_WAIT;
    }
    m_PerformSeq++;
    break;

  case ANIMATION_START:  ///< アニメーション再生開始
    {
      if( m_fadeCounter >= 0 )
      {
        m_fadeCounter --;
        if( m_fadeCounter == 0 )
        {
          this->FadeStart();
        }
      }

      // アクションコマンド実行中まつ
      if( m_pPlayer->IsExecuteActionCommand() && (this->walkCounter == 0) && (m_fadeCounter > 0) )
      {
        break;
      }

      // アニメーション再生
      TOMOYA_PRINT( "AnimationStepFrame %f\n", m_pPlayer->GetCharaDrawInstance()->GetAnimationStepFrame() );
      TOMOYA_PRINT( "AnimationFrameMax %f\n", m_pPlayer->GetCharaDrawInstance()->GetAnimationEndFrame() );

      m_PerformSeq++;
    }
    break;
  case FADE_WAIT:        ///< フェード待ち
    if( this->IsFadeEnd() )
    {
      m_PerformSeq++;
      return EventEntranceInPerformanceBase::RET_FINISH;
    }
    break;
  case END:
    return EventEntranceInPerformanceBase::RET_FINISH;
  }
  return EventEntranceInPerformanceBase::RET_CONTINUE;
}

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの梯子イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// 吸着するポイントの座標
void EventEntranceInPerformanceLadderUp::GetPos( gfl2::math::Vector3* p_pos )
{
  *p_pos = m_Data.targetPos;
}

// 吸着後の向き
void EventEntranceInPerformanceLadderUp::GetQua( gfl2::math::Quaternion* p_qua )
{

  *p_qua = m_Data.quaternion;

  // Y軸反転
  gfl2::math::Quaternion rotate( gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(180) );

  *p_qua = *p_qua * rotate;
}

// 再生するアニメーションナンバー
u32 EventEntranceInPerformanceLadderUp::GetAnimationNo()
{
  return P2_BASE_FI061_LADDERHOLD01_UP;
}


// 吸着するポイントの座標
void EventEntranceInPerformanceLadderDown::GetPos( gfl2::math::Vector3* p_pos )
{
  *p_pos = m_Data.targetPos;
}

// 吸着後の向き
void EventEntranceInPerformanceLadderDown::GetQua( gfl2::math::Quaternion* p_qua )
{
  *p_qua = m_Data.quaternion;

  // Y軸反転
  gfl2::math::Quaternion rotate( gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(180) );

  *p_qua = *p_qua * rotate;
}

// 再生するアニメーションナンバー
u32 EventEntranceInPerformanceLadderDown::GetAnimationNo()
{
  return P2_BASE_FI081_SQUAT01_DOWN;
}


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのしゃがみ移動イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// 吸着するポイントの座標
void EventEntranceInPerformanceThroughAGate::GetPos( gfl2::math::Vector3* p_pos )
{
  *p_pos = m_Data.targetPos;
}

// 吸着後の向き
void EventEntranceInPerformanceThroughAGate::GetQua( gfl2::math::Quaternion* p_qua )
{
  *p_qua = m_Data.quaternion;

  // Y軸反転
  gfl2::math::Quaternion rotate( gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(180) );

  *p_qua = *p_qua * rotate;
}

// 再生するアニメーションナンバー
u32 EventEntranceInPerformanceThroughAGate::GetAnimationNo()
{
  return P2_BASE_FI081_SQUAT01_DOWN; 
}

GFL_NAMESPACE_END(Field);
