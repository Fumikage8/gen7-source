//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventEntranceOut.h
 *	@brief  エントランスアウト
 *	@author	tomoya takahashi
 *	@date		2015.05.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/Event/FieldEventEntranceOut.h"

#include "GameSys/include/GameEventManager.h"

#include "Field/FieldRo/include/PlacementData/PlacementMapJumpData.h"
#include "niji_conv_header/field/mapjump/FieldMapJumpType.h"

#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

#include <Fade/include/gfl2_FadeManager.h>

#include "Sound/include/Sound.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif // PM_DEBUG

GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
const FADE_TYPE EventEntranceOut::sFadeType[MC_FIELD_MAX][MC_FIELD_MAX] = {
  { FADE_TYPE_BLACK,  FADE_TYPE_CIRCLE, FADE_TYPE_BLACK, FADE_TYPE_BLACK,  FADE_TYPE_BLACK,},
  { FADE_TYPE_BLACK,  FADE_TYPE_BLACK,  FADE_TYPE_BLACK, FADE_TYPE_BLACK,  FADE_TYPE_BLACK,},
  { FADE_TYPE_BLACK,  FADE_TYPE_BALL,   FADE_TYPE_BLACK, FADE_TYPE_BLACK,  FADE_TYPE_BLACK,},
  { FADE_TYPE_BLACK,  FADE_TYPE_DIA,    FADE_TYPE_BLACK, FADE_TYPE_BLACK,  FADE_TYPE_BLACK,},
  { FADE_TYPE_BLACK,  FADE_TYPE_CIRCLE, FADE_TYPE_BLACK, FADE_TYPE_BLACK,  FADE_TYPE_BLACK,},
};

const u32 EventEntranceOut::sFadeTime[MC_FIELD_MAX][MC_FIELD_MAX] = {
  { 1,  6,  2,  2,  2,},
  { 2,  1,  2,  2,  2,},
  { 2,  6,  2,  2,  2,},
  { 2,  6,  2,  2,  2,},
  { 2, 20,  2,  2,  1,},
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
 *  @brief デスクリプションの初期化
 */
void EventEntranceOut::InitializeDesc( Desc* p_desc )
{
  p_desc->mapJumpType = MAPJUMP_TYPE_NORMAL;
  p_desc->eventID     = 0;
  p_desc->nowZoneID   = ZONEID_ERROR;
  p_desc->nextZoneID  = ZONEID_ERROR;
  p_desc->rotate      = gfl2::math::Quaternion(gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(0));
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
bool EventEntranceOut::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventEntranceOut::InitFunc(GameSys::GameManager* gmgr)
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventEntranceOut::MainFunc(GameSys::GameManager* gmgr)
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
    CallEntranceOut(gmgr, m_Data.mapJumpType);
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
void EventEntranceOut::EndFunc(GameSys::GameManager* gmgr)
{
}

/**
 * @brief データをセットする
 * @param param パラメーター
 */
void EventEntranceOut::SetData(const EventEntranceOut::Desc& param)
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
void EventEntranceOut::CallEntranceOut(GameSys::GameManager* p_gman, u32 mapJumpType )
{

  switch( mapJumpType ){
  case MAPJUMP_TYPE_DOOR:
    {
      //演出用イベントの起動
      EventEntranceOutPerformanceDOOR* p_event;

      // イベントをコール
      p_event = reinterpret_cast<EventEntranceOutPerformanceDOOR*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceOutPerformanceDOOR > (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;

  case MAPJUMP_TYPE_LADDER_UP:
    {
      // Upタイプの接続に復帰する場合＝降りてきたとき

      //演出用イベントの起動
      EventEntranceOutPerformanceLadderDown* p_event;

      // イベントをコール
      p_event = reinterpret_cast<EventEntranceOutPerformanceLadderDown*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceOutPerformanceLadderDown > (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;

  case MAPJUMP_TYPE_LADDER_DOWN:
    {
      // DOWNタイプの接続に復帰する場合＝登ってきたとき

      //演出用イベントの起動
      EventEntranceOutPerformanceLadderUp* p_event;

      // イベントをコール
      p_event = reinterpret_cast<EventEntranceOutPerformanceLadderUp*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceOutPerformanceLadderUp > (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;

  case MAPJUMP_TYPE_THROUGH_A_GATE:
    {
      //演出用イベントの起動
      EventEntranceOutPerformanceThroughAGate* p_event;

      // イベントをコール
      p_event = reinterpret_cast<EventEntranceOutPerformanceThroughAGate*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceOutPerformanceThroughAGate > (p_gman->GetGameEventManager()));
      // パラメータをセット
      p_event->SetData(m_Data);
    }
    break;


  case MAPJUMP_TYPE_MAT:
  case MAPJUMP_TYPE_MAT_C:
    {
      //演出用イベントの起動
      EventEntranceOutPerformanceMAT* p_event;

      // イベントをコール
      p_event = reinterpret_cast<EventEntranceOutPerformanceMAT*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceOutPerformanceMAT > (p_gman->GetGameEventManager()));
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
      EventEntranceOutPerformanceNORMAL* p_event;

      // イベントをコール
      p_event = reinterpret_cast<EventEntranceOutPerformanceNORMAL*> (GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceOutPerformanceNORMAL > (p_gman->GetGameEventManager()));
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
bool EventEntranceOutPerformanceBase::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
}

/**
 * @brief bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param p_gman ゲームマネージャークラス
 */
void EventEntranceOutPerformanceBase::InitFunc(GameSys::GameManager* p_gman)
{
  // 主人公キャラクター取得
  m_pPlayer = p_gman->GetFieldmap()->GetPlayerCharacter();

  // 配置モデルマネージャー
  m_pStaticModelManager = p_gman->GetFieldmap()->GetStaticModelManager();

  // niji BTS[5787]BugFix
  m_Seq = SEQ_PERFORMANCE;

  // カメラセットアップ
  this->SetupCameraProcess(p_gman);
}

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param p_gman ゲームマネージャークラス
 * @return GMEVENT_RESULT イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventEntranceOutPerformanceBase::MainFunc(GameSys::GameManager* p_gman)
{
  switch (m_Seq) {
  case SEQ_FADE:
  {
    //u8 exitType = m_Data.mapJumpType;
    bool isFade = true;
    /*if (exitType == EXIT_TYPE_MAT_NOT_FADE_IN ||
      exitType == EXIT_TYPE_STAIRS_NOT_FADE_IN) {
      isFade = false;
    }*/

    if (isFade) {
      // Fade処理
      FadeProcess( p_gman->GetGameData()->GetFieldZoneDataLoader() );
    }

    ++m_Seq;
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;
  }

  case SEQ_PERFORMANCE:
  {
    CAMERA_PROC_RET camera_ret = this->CameraProcess();

    if( camera_ret != CAMERA_PROC_RET_CONTINUE )
    {
      // 演出処理
      EventEntranceOutPerformanceBase::RET ret = PerformanceProcess();
      // 演出処理
      if ((camera_ret==CAMERA_PROC_RET_FINISH) && (ret == RET_FINISH)) 
      {
        EndCameraProcess();
        return GameSys::GMEVENT_RES_FINISH;
      }
      else if (ret == RET_CONTINUE) 
      {
        return GameSys::GMEVENT_RES_CONTINUE;
      }
      else if(ret == RET_CONTINUE_DIRECT)
      {
        return GameSys::GMEVENT_RES_CONTINUE_DIRECT;
      }
      else
      {
        // カメラの更新待ち
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
void EventEntranceOutPerformanceBase::EndFunc(GameSys::GameManager* /*p_gman*/)
{
}

/**
 * @brief データをセットする
 * @param param パラメーター
 */
void EventEntranceOutPerformanceBase::SetData(const EventEntranceOut::Desc & param)
{
  m_Data = param;
}

/**
 * @brief フェード処理
 */
void EventEntranceOutPerformanceBase::FadeProcess(ZoneDataLoader* zoneDataLoader)
{
  // 現ZONE　と　次のZONEからフェードを決定
  void * pZoneData = zoneDataLoader->GetZoneData( m_Data.nowZoneID );
  ZoneDataAccessor nowZoneDataAccessor( pZoneData);

  pZoneData = zoneDataLoader->GetZoneData( m_Data.nextZoneID );
  ZoneDataAccessor nextZoneDataAccessor( pZoneData);

  FADE_TYPE type = EventEntranceOut::sFadeType[nowZoneDataAccessor.GetMapChangeType()][nextZoneDataAccessor.GetMapChangeType()];
  u32 sync = EventEntranceOut::sFadeTime[nowZoneDataAccessor.GetMapChangeType()][nextZoneDataAccessor.GetMapChangeType()];

  gfl2::Fade::FADE_TYPE fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;

  switch (type) {
  case FADE_TYPE_BLACK:
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    break;
  case FADE_TYPE_WHITE:
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    break;
  case FADE_TYPE_CIRCLE:
    fadeInType = gfl2::Fade::FADE_TYPE_CIRCLE;
    break;
  case FADE_TYPE_BALL:
    fadeInType = gfl2::Fade::FADE_TYPE_BALL;
    break;
  case FADE_TYPE_DIR:
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    break;
  case FADE_TYPE_DIA:
    fadeInType = gfl2::Fade::FADE_TYPE_DIAMOND;
    break;
  }

#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ENTRANCE_FADE_CUT ) )
  {
    fadeInType = gfl2::Fade::FADE_TYPE_ALPHA;
    sync = 1;
  }
#endif


  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, fadeInType, sync);

  //@fix 増田さん要望 : レイアウトフェードのアニメーションスピードを1/2にする。
  if( (type == FADE_TYPE_CIRCLE) ||
      (type == FADE_TYPE_BALL) ||
      (type == FADE_TYPE_DIA)  )
  {
    const f32 LAYOUT_FADE_TIME = 10.0f;

    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->SetAnimeSpeed( gfl2::Fade::DISP_UPPER, LAYOUT_FADE_TIME / static_cast<f32>(sync) );
  }
}

/**
 *  @brief カメラパラメータの設定
 */
void EventEntranceOutPerformanceBase::SetupCameraProcess( GameSys::GameManager* p_gman )
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

      // @fix GFNMCat[2746] エントランスカメラのOUT時に自機が待機していない
      m_CameraPlayWait = m_CameraParameter.play_start_frame;
    }
  }

}


/**
 * @brief カメラ処理
 */
EventEntranceOutPerformanceBase::CAMERA_PROC_RET EventEntranceOutPerformanceBase::CameraProcess(void)
{
  if( m_IsCameraAnimation == false )
  {
    enum
    {
      STATE_NOTANIM_SETUP, 
      STATE_NOTANIM_END,
    };

    switch(m_CameraSeq)
    {
    case STATE_NOTANIM_SETUP:
      {
        //@fix GFNMCat[2746]  動作の初期化待ち。
        // SetUpを実行
        if( SetUp() )
        {
          m_CameraSeq ++;
          return CAMERA_PROC_RET_CONTINUE; // RET_CONTINUEにして、processは動作させない。
        }
      }
      return CAMERA_PROC_RET_CONTINUE;

    case STATE_NOTANIM_END:
     return CAMERA_PROC_RET_FINISH; // processを動作させる。
    }
  }

  enum
  {
    STATE_CAMERA_PARAMETER_SET, ///< メインループを一度回し、今のカメラアングルを確定してもらう。
    STATE_SET_END_CAMERA,       ///< カメラの終了座標にする。
    STATE_EVENT_CAMERA_START,
    STATE_WAIT_PLAY,
    STATE_WAIT_CAMERA,
    STATE_END,
  };

  switch( m_CameraSeq )
  {
  case STATE_CAMERA_PARAMETER_SET: ///< メインループを一度回し、今のカメラアングルを確定してもらう。
    m_CameraSeq ++;
    return CAMERA_PROC_RET_CONTINUE;

  case STATE_SET_END_CAMERA:       ///< カメラの終了座標にする。
    {

      //====================================================================================================
      // カメラ切り替え

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

      // カメラ切り替え
      //====================================================================================================


      //====================================================================================================
      // 現在の座標、回転、距離を取得
      const poke_3d::model::BaseCamera * camera = m_pCameraUnit->GetBaseCamera();

      // @note  アニメーション再生中はTargetPositionが最後に指定された座標になっている
      Camera::Area::Utility::GetEventScriptCameraParameter( camera, &m_CameraTargetPosition, &m_CameraPosition, &m_CameraRotate, &m_CameraDistance );
      m_CameraRotate.x       = GFL_MATH_RAD_TO_DEG(m_CameraRotate.x);
      m_CameraRotate.y       = GFL_MATH_RAD_TO_DEG(m_CameraRotate.y);
      m_CameraRotate.z       = GFL_MATH_RAD_TO_DEG(m_CameraRotate.z);
      m_CameraBank           = m_CameraRotate.GetZ();
      // 現在の座標、回転、距離を取得
      //====================================================================================================


      //====================================================================================================
      // 1フレームで、終了カメラにする。
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
            1,
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
            1,
            static_cast<gfl2::math::Easing::EaseFunc>(m_CameraParameter.easing)
            );
        }
        m_CameraSeq ++;
      }
    }
    return CAMERA_PROC_RET_CONTINUE;


  case STATE_EVENT_CAMERA_START:
    {
      //@fix GFNMCat[2746]  動作の初期化待ち。
      if( (m_pCameraController->IsMoving() == false) && (SetUp() == true) )
      {
        // アニメーション開始
        {
          if( m_CameraParameter.type == EventEntranceCameraParameter::TYPE_TARGET )
          {
            m_pCameraController->Move(
              m_CameraPosition,
              m_CameraTargetPosition,
              m_CameraParameter.frame,
              static_cast<gfl2::math::Easing::EaseFunc>(m_CameraParameter.easing),
              m_CameraBank 
              );
          }
          else if( m_CameraParameter.type == EventEntranceCameraParameter::TYPE_ROTATE )
          {
            m_pCameraController->Move(
              m_CameraTargetPosition,
              m_CameraRotate,
              m_CameraDistance,
              m_CameraParameter.frame,
              static_cast<gfl2::math::Easing::EaseFunc>(m_CameraParameter.easing)
              );
          }
          m_CameraSeq ++;
        }
      }
    }
    return CAMERA_PROC_RET_CONTINUE;

  case STATE_WAIT_PLAY:
    {
      // @fix GFNMCat[2746] エントランスカメラのOUT時に自機が待機していない
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
void EventEntranceOutPerformanceBase::EndCameraProcess()
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

/**
 *  @brief フェードアウト終了
 */
b32 EventEntranceOutPerformanceBase::IsFadeEnd(void) const
{
  return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_UPPER);
}


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの通常演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/**
 * @brief 初期処理
 */
bool EventEntranceOutPerformanceMAT::SetUp(void)
{
  // 方向を指定
  gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
  srt.rotation = m_Data.rotate;
  m_pPlayer->SetLocalSRT( srt );

  // @fix GFNMCat[1032] 衝突が解決(=触れている箇所が一箇所)されている場合は、衝突後の座標を使う
  gfl2::math::Vector3 adjustPos(0.0f,0.0f,0.0f);
  u32 hitCnt = m_pPlayer->GetHitCntFromOshikaeshiScene(m_pPlayer->GetLocalSRT().translate,&adjustPos);
  if( hitCnt == 1 )
  {
    m_pPlayer->GetCharaDrawInstance()->SetPosition(adjustPos);
  }

  // @fix GFNMCat[1032] １歩後退（１フレーム内で）とシーンの押し返しにより裏世界に行ったしまった場合のセーフティ処理
  if( !m_pPlayer->CheckValidGround(true) )
  {
    m_pPlayer->GetCharaDrawInstance()->SetPosition(m_Data.jumpCenterPosition);

#if PM_DEBUG
    u32 hitCnt = m_pPlayer->GetHitCntFromOshikaeshiScene(m_Data.jumpCenterPosition,&adjustPos);
    if( hitCnt > 1 )
    {
      GFL_ASSERT_MSG(false,"出口がふさがれているため出られない");
    }
#endif

  }

  // フェード開始
  this->FadeStart();

  return true;
}

/**
 * @brief 演出処理
 */
EventEntranceOutPerformanceBase::RET EventEntranceOutPerformanceMAT::PerformanceProcess(void)
{
  enum
  {
    SEQ_WAIT,
  };

  switch (m_PerformSeq) 
  {
  case SEQ_WAIT:
    {
      if( this->IsFadeEnd() ) {
        return EventEntranceOutPerformanceBase::RET_FINISH;
      }
    }
    break;
  }
  return EventEntranceOutPerformanceBase::RET_CONTINUE;
}


/**
 * @brief 初期処理
 */
bool EventEntranceOutPerformanceNORMAL::SetUp(void)
{
  switch (m_PerformSeq) 
  {
  case SEQ_INIT:
    {
      // 方向を指定
      gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
      srt.rotation = m_Data.rotate * gfl2::math::Quaternion(gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(180));
      m_pPlayer->SetLocalSRT( srt );


      TOMOYA_PRINT( "before %f,%f,%f\n", srt.translate.GetX(), srt.translate.GetY(), srt.translate.GetZ() );

      // １歩後退（１フレーム内で）
      if( m_pPlayer->GetOnMoveModel() == NULL )
      {
        m_pPlayer->SetActionCommandStepMoveWithoutStartEnd( MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE_WITHOUT_START_END, 1, true ); 
      }
   
      m_PerformSeq++;
    }
    return false;

  case SEQ_FADESTART:
    if( (!m_pPlayer->IsExecuteActionCommand()) )
    {
      // @fix GFNMCat[1032] 衝突が解決(=触れている箇所が一箇所)されている場合は、衝突後の座標を使う
      gfl2::math::Vector3 adjustPos(0.0f,0.0f,0.0f);
      u32 hitCnt = m_pPlayer->GetHitCntFromOshikaeshiScene(m_pPlayer->GetLocalSRT().translate,&adjustPos);
      if( hitCnt == 1 )
      {
        m_pPlayer->GetCharaDrawInstance()->SetPosition(adjustPos);
      }

      m_IsPositionAdjust = false;

      // @fix GFNMCat[1032] １歩後退（１フレーム内で）とシーンの押し返しにより裏世界に行ったしまった場合のセーフティ処理
      if( !m_pPlayer->CheckValidGround(true) )
      {
        m_pPlayer->GetCharaDrawInstance()->SetPosition(m_Data.jumpCenterPosition);
        m_IsPositionAdjust = true;  // 裏世界に行って、センターポジションに設定しているときは、1歩移動をカットする。

#if PM_DEBUG
        u32 hitCnt = m_pPlayer->GetHitCntFromOshikaeshiScene(m_Data.jumpCenterPosition,&adjustPos);
        if( hitCnt > 1 )
        {
          GFL_ASSERT_MSG(false,"出口がふさがれているため出られない");
        }
#endif

      }

      // 方向を指定
      gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
      srt.rotation = m_Data.rotate;
      m_pPlayer->SetLocalSRT( srt );

      TOMOYA_PRINT( "after %f,%f,%f\n", srt.translate.GetX(), srt.translate.GetY(), srt.translate.GetZ() );


      // フェード開始
      this->FadeStart();

      m_PerformSeq++;
      // -----------------------------------------------------------

      // もしカメラアニメーションがないならここでアニメーションを
      if( m_IsCameraAnimation == false )
      {
        // １歩分歩く
        if( (m_pPlayer->GetOnMoveModel() == NULL) && (m_IsPositionAdjust == false) )
        {
          m_pPlayer->SetActionCommandStepMove( (MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE),true,1,true,1.0f, false, false );
        }
        m_PerformSeq++;
      }
    }
    break; // return true

  default:
    break; // return true
  }

  return true;
}

/**
 * @brief 演出処理
 */
EventEntranceOutPerformanceBase::RET EventEntranceOutPerformanceNORMAL::PerformanceProcess(void)
{
  switch (m_PerformSeq) 
  {
  case SEQ_ANIMESTART:
    {
      // １歩分歩く
      if( (m_pPlayer->GetOnMoveModel() == NULL) && (m_IsPositionAdjust == false) )
      {
        m_pPlayer->SetActionCommandStepMove( (MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE),true,1,true,1.0f, false, false );
      }
      m_PerformSeq++;
    }
    break;

  case SEQ_WAIT:
    {
#if PM_DEBUG
      static b32 isEnable = true;
#else
      static const b32 isEnable = true;
#endif

      if( this->IsFadeEnd() && (!m_pPlayer->IsExecuteActionCommand()) ) 
      {
        // ライド以外で発動 
        if(isEnable && (m_pPlayer->GetOnMoveModel() == NULL))
        {
          // 待機に変更して終わり
          m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( m_pPlayer->GetDefaultIdleAnimationId() );
        }

        gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
        TOMOYA_PRINT( "end %f,%f,%f\n", srt.translate.GetX(), srt.translate.GetY(), srt.translate.GetZ() );
        return EventEntranceOutPerformanceBase::RET_FINISH;
      }
    }
    break;

  default:
    GFL_ASSERT(0);
    return EventEntranceOutPerformanceBase::RET_FINISH;
  }
  return EventEntranceOutPerformanceBase::RET_CONTINUE;
}



//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのドア演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/**
 * @brief 初期処理
 */
bool EventEntranceOutPerformanceDOOR::SetUp(void)
{
  walkCounter = 0;

  // 方向を指定
  gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
  srt.rotation = m_Data.rotate;
  m_pPlayer->SetLocalSRT( srt );

  // フェード開始
  this->FadeStart();

  // ドアをアニメさせる
  m_pDoorModel = m_pStaticModelManager->GetEventIDStaticModel( m_Data.eventID );
  if( m_pDoorModel ){
    static const int Frame = 3;
    StaticModel::StaticModel::Handle_StartDoorCloseAnimation handle( Frame );
    m_pDoorModel->Set( &handle );
  }
  return true;
}

/**
 * @brief 演出処理
 */
EventEntranceOutPerformanceBase::RET EventEntranceOutPerformanceDOOR::PerformanceProcess(void)
{
  enum
  {
    SEQ_WAIT,
  };


  switch (m_PerformSeq) {
  case SEQ_WAIT:
    {
      bool is_doorEnd = true;
      m_pDoorModel = m_pStaticModelManager->GetEventIDStaticModel( m_Data.eventID );
      if( m_pDoorModel ){
        StaticModel::StaticModel::Handle_IsEndDoorAnimation handle;
        is_doorEnd = m_pDoorModel->IsEnd( handle );
      }

      if ( is_doorEnd && this->IsFadeEnd() ) {

        return EventEntranceOutPerformanceBase::RET_FINISH;
      }

      walkCounter++;
    }
    break;
  }
  return EventEntranceOutPerformanceBase::RET_CONTINUE;
}


/**
 * @brief 演出処理
 */
EventEntranceOutPerformanceBase::RET EventEntranceOutPerformanceAnimationBase::PerformanceProcess(void)
{
  switch (m_PerformSeq) {
  case 0:
    {
      walkCounter = 0;

      // 方向を指定
      gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
      srt.rotation = m_Data.rotate;
      m_pPlayer->SetLocalSRT( srt );

      m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( this->GetAnimationNo() );


      // フェード開始
      this->FadeStart();

      // ループアニメが指定されたらそのまま終わる。
      if( !m_pPlayer->GetCharaDrawInstance()->IsAnimationLoop() )
      {
        m_PerformSeq++;
      }
      else
      {
        m_PerformSeq = 2;
      }
    }
    break;

  case 1:
    {
      walkCounter++;
      if ( !m_pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame() ) 
      {
        break;
      }
      else
      {
        m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( m_pPlayer->GetDefaultIdleAnimationId() );
        m_PerformSeq++;
      }
    }
    //break; through


  case 2:
    if( this->IsFadeEnd() )
    {
      return EventEntranceOutPerformanceBase::RET_FINISH;
    }
    break;
  }

  return EventEntranceOutPerformanceBase::RET_CONTINUE;
}

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの梯子イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
u32 EventEntranceOutPerformanceLadderUp::GetAnimationNo()
{
  return P2_BASE_FI082_SQUAT01_UP;
}

u32 EventEntranceOutPerformanceLadderDown::GetAnimationNo()
{
  return m_pPlayer->GetDefaultIdleAnimationId();
}

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのしゃがみ移動イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

u32 EventEntranceOutPerformanceThroughAGate::GetAnimationNo()
{
  return P2_BASE_FI082_SQUAT01_UP;
}



GFL_NAMESPACE_END(Field);
